#include "Castor3D/Render/Clustered/ComputeClustersAABB.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslAABB.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace cptclsb
	{
		enum BindingPoints
		{
			eCamera,
			eClusters,
			eClustersAABB,
		};

		static ShaderPtr createShader()
		{
			using namespace sdw;
			ComputeWriter writer;

			// Inputs
			C3D_Camera( writer
				, eCamera
				, 0u );
			C3D_Clusters( writer
				, eClusters
				, 0u );
			C3D_ClustersAABB( writer
				, eClustersAABB
				, 0u );

			auto screenToView = writer.implementFunction< sdw::Vec4 >( "screenToView"
				, [&]( sdw::Vec4 const screen )
				{
					// Convert to normalized texture coordinates in the range [0 .. 1].
					auto texCoord = writer.declLocale( "texCoord"
						, screen.xy() / vec2( c3d_cameraData.renderSize() ) );

					// Convert to clip space
					auto clip = writer.declLocale( "clip"
						, sdw::vec4( sdw::fma( texCoord.xy(), vec2( 2.0_f ), vec2( -1.0_f ) )
							, screen.zw() ) );

					auto view = writer.declLocale( "view"
						, c3d_cameraData.projToView( clip ) );
					view /= view.w();
					writer.returnStmt( view );
				}
				, sdw::InVec4{ writer, "screen" } );

			auto intersectLinePlane = writer.implementFunction< sdw::Boolean >( "c3d_intersectLinePlane"
				, [&]( sdw::Vec3 const a
					, sdw::Vec3 const b
					, shader::Plane const p
					, sdw::Vec3 q )
				{
					auto ab = b - a;
					auto t = writer.declLocale( "t"
						, ( p.distance() - dot( p.normal(), a ) ) / dot( p.normal(), ab ) );

					auto intersect = writer.declLocale( "intersect"
						, ( t >= 0.0f && t <= 1.0f ) );

					q = vec3( 0.0_f );

					IF( writer, intersect )
					{
						q = a + ab * t;
					}
					FI;

					writer.returnStmt( intersect );
				}
				, sdw::InVec3{ writer, "a" }
				, sdw::InVec3{ writer, "b" }
				, shader::InPlane{ writer, "p" }
				, sdw::OutVec3{ writer, "q" } );

			writer.implementMainT< VoidT >( 1u, 1u, 1u
				, [&]( ComputeIn in )
				{
					auto clusterIndex3D = in.globalInvocationID;
					auto clusterIndex1D = writer.declLocale( "clusterIndex1D"
						, c3d_clustersData.computeClusterIndex1D( clusterIndex3D ) );

					// Compute the near and far planes for cluster K.
					auto nearPlane = writer.declLocale( "nearPlane"
						, shader::Plane{ vec3( 0.0_f, 0.0f, 1.0f )
							, writer.cast< sdw::Float >( -c3d_clustersData.viewNear() * pow( abs( c3d_clustersData.nearK() ), writer.cast< sdw::Float >( clusterIndex3D.z() ) ) ) } );
					auto farPlane = writer.declLocale( "farPlane"
						, shader::Plane{ vec3( 0.0_f, 0.0f, 1.0f )
							, writer.cast< sdw::Float >( -c3d_clustersData.viewNear() * pow( abs( c3d_clustersData.nearK() ), writer.cast< sdw::Float >( clusterIndex3D.z() + 1_u ) ) ) } );

					// Reversed depth implies maxZ is 0.0f instead of 1.0f.
					float constexpr maxZ = 0.0f;
					// The top-left point of cluster K in screen space.
					auto pMin = writer.declLocale( "pMin"
						, sdw::vec4( vec2( clusterIndex3D.xy() * c3d_clustersData.clusterSize() ), maxZ, 1.0f ) );
					// The bottom-right point of cluster K in screen space.
					auto pMax = writer.declLocale( "pMax"
						, sdw::vec4( vec2( ( clusterIndex3D.xy() + u32vec2( 1_u ) ) * c3d_clustersData.clusterSize() ), maxZ, 1.0f ) );

					// Transform the screen space points to view space.
					pMin = screenToView( pMin );
					pMax = screenToView( pMax );

					// Find the min and max points on the near and far planes.
					auto nearMin = writer.declLocale< sdw::Vec3 >( "nearMin" );
					auto nearMax = writer.declLocale< sdw::Vec3 >( "nearMax" );
					auto farMin = writer.declLocale< sdw::Vec3 >( "farMin" );
					auto farMax = writer.declLocale< sdw::Vec3 >( "farMax" );
					// Origin (camera eye position)
					auto eye = writer.declLocale< sdw::Vec3 >( "eye", vec3( 0.0_f ) );
					intersectLinePlane( eye, pMin.xyz(), nearPlane, nearMin );
					intersectLinePlane( eye, pMax.xyz(), nearPlane, nearMax );
					intersectLinePlane( eye, pMin.xyz(), farPlane, farMin );
					intersectLinePlane( eye, pMax.xyz(), farPlane, farMax );

					auto aabbMin = writer.declLocale( "aabbMin"
						, min( nearMin, min( nearMax, min( farMin, farMax ) ) ) );
					auto aabbMax = writer.declLocale( "aabbMax"
						, max( nearMin, max( nearMax, max( farMin, farMax ) ) ) );
					auto aabbCenter = writer.declLocale( "aabbCenter"
						, aabbMin + ( aabbMax - aabbMin ) / 2.0f );

					c3D_clustersAABB[clusterIndex1D] = shader::AABB{ vec4( aabbMin, 1.0f )
						, vec4( aabbMax, 1.0f ) };

				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		class FramePass
			: private castor::DataHolderT< ShaderModule >
			, private castor::DataHolderT< ashes::PipelineShaderStageCreateInfoArray >
			, public crg::ComputePass
		{
			using ShaderHolder = DataHolderT< ShaderModule >;
			using CreateInfoHolder = DataHolderT< ashes::PipelineShaderStageCreateInfoArray >;

		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, crg::cp::Config config )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, "ComputeClustersAABB", createShader() } }
				, CreateInfoHolder{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, ShaderHolder::getData() ) } }
				, crg::ComputePass{framePass
					, context
					, graph
					, crg::ru::Config{}
					, config
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( CreateInfoHolder::getData() ) )
						.end( RecordCallback{ [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t idx ) { doPostRecord( ctx, cb, idx ); } } ) }
			{
			}

		private:
			void doPostRecord( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				for ( auto & attach : m_pass.buffers )
				{
					auto buffer = attach.buffer;

					if ( !attach.isNoTransition()
						&& attach.isStorageBuffer()
						&& attach.isClearableBuffer() )
					{
						auto currentState = context.getAccessState( buffer.buffer.buffer( index )
							, buffer.range );
						context.memoryBarrier( commandBuffer
							, buffer.buffer.buffer( index )
							, buffer.range
							, currentState.access
							, currentState.pipelineStage
							, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
					}
				}
			}
		};
	}

	//*********************************************************************************************

	crg::FramePass const & createComputeClustersAABBPass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, FrustumClusters & clusters )
	{
		auto & pass = graph.createPass( "ComputeClustersAABB"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< cptclsb::FramePass >( framePass
					, context
					, graph
					, device
					, crg::cp::Config{}
						.groupCountX( clusters.getDimensions()->x )
						.groupCountY( clusters.getDimensions()->y )
						.groupCountZ( clusters.getDimensions()->z )
						.enabled( &clusters.needsClustersUpdate() ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			});
		pass.addDependency( *previousPass );
		cameraUbo.createPassBinding( pass, cptclsb::eCamera );
		clusters.getClustersUbo().createPassBinding( pass, cptclsb::eClusters );
		createClearableOutputStorageBinding( pass, uint32_t( cptclsb::eClustersAABB ), "C3D_ClustersAABB", clusters.getClustersAABBBuffer(), 0u, ashes::WholeSize );
		return pass;
	}

	//*********************************************************************************************
}