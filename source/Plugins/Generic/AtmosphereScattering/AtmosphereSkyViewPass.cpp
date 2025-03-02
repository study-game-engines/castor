#include "AtmosphereScattering/AtmosphereSkyViewPass.hpp"

#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace skyview
	{
		enum Bindings : uint32_t
		{
			eCamera,
			eAtmosphere,
			eTransmittance,
		};

		static castor3d::ShaderPtr getVertexProgram()
		{
			sdw::VertexWriter writer;
			sdw::Vec2 position = writer.declInput< sdw::Vec2 >( "position", 0u );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::VertexIn{ writer }
				, sdw::VertexOut{ writer }
				, [&]( sdw::VertexIn in
					, sdw::VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static castor3d::ShaderPtr getPixelProgram( castor3d::Engine const & engine
			, VkExtent3D const & renderSize
			, VkExtent3D const & transmittanceExtent )
		{
			sdw::FragmentWriter writer;

			ATM_Camera( writer
				, Bindings::eCamera
				, 0u );
			C3D_AtmosphereScattering( writer
				, uint32_t( Bindings::eAtmosphere )
				, 0u );
			auto transmittanceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "transmittanceMap"
				, uint32_t( Bindings::eTransmittance )
				, 0u );

			auto  sampleCountIni = writer.declConstant( "sampleCountIni"
				, 30.0_f );	// Can go a low as 10 sample but energy lost starts to be visible.
			auto  depthBufferValue = writer.declConstant( "depthBufferValue"
				, -1.0_f );
			auto planetRadiusOffset = writer.declConstant( "planetRadiusOffset"
				, 0.01_f );

			// Fragment Outputs
			auto outColour( writer.declOutput< sdw::Vec4 >( "outColour", 0u ) );

			AtmosphereModel atmosphere{ writer
				, c3d_atmosphereData
				, AtmosphereModel::Settings{ castor::Length::fromUnit( 1.0f, engine.getLengthUnit() ) }
					.setCameraData( &atm_cameraData )
					.setVariableSampleCount( true )
					.setMieRayPhase( true )
				, { transmittanceExtent.width, transmittanceExtent.height } };
			atmosphere.setTransmittanceMap( transmittanceMap );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::FragmentIn{ writer }
				, sdw::FragmentOut{ writer }
				, [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
				{
					auto targetSize = writer.declLocale( "targetSize"
						, vec2( sdw::Float{ float( renderSize.width + 1u ) }, float( renderSize.height + 1u ) ) );
					auto pixPos = writer.declLocale( "pixPos"
						, in.fragCoord.xy() );
					auto uv = writer.declLocale( "uv"
						, pixPos / targetSize );
					auto ray = writer.declLocale( "ray"
						, atmosphere.castRay( uv ) );

					auto viewHeight = writer.declLocale( "viewHeight"
						, length( ray.origin ) );

					auto viewZenithCosAngle = writer.declLocale< sdw::Float >( "viewZenithCosAngle" );
					auto lightViewCosAngle = writer.declLocale< sdw::Float >( "lightViewCosAngle" );
					atmosphere.uvToSkyViewLutParams( viewZenithCosAngle, lightViewCosAngle, viewHeight, uv, targetSize );

					auto sunDir = writer.declLocale< sdw::Vec3 >( "sunDir" );
					{
						auto upVector = writer.declLocale( "upVector"
							, ray.origin / viewHeight );
						auto sunZenithCosAngle = writer.declLocale( "sunZenithCosAngle"
							, dot( upVector, c3d_atmosphereData.sunDirection() ) );
						auto sunZenithSinAngle = writer.declLocale( "sunZenithSinAngle"
							, sqrt( 1.0_f - sunZenithCosAngle * sunZenithCosAngle ) );
						sunDir = normalize( vec3( sunZenithSinAngle, sunZenithCosAngle, 0.0_f ) );
					}

					ray.origin = vec3( 0.0_f, viewHeight, 0.0_f );

					auto lightViewSinAngle = writer.declLocale( "lightViewSinAngle"
						, sqrt( 1.0_f - lightViewCosAngle * lightViewCosAngle ) );
					auto viewZenithSinAngle = writer.declLocale( "viewZenithSinAngle"
						, sqrt( 1.0_f - viewZenithCosAngle * viewZenithCosAngle ) );
					ray.direction = vec3( viewZenithSinAngle * lightViewCosAngle
						, viewZenithCosAngle
						, -viewZenithSinAngle * lightViewSinAngle );

					// Move to top atmospehre
					IF( writer, !atmosphere.moveToTopAtmosphere( ray ) )
					{
						// Ray is not intersecting the atmosphere
						outColour = vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f );
					}
					ELSE
					{
						auto ss = writer.declLocale( "ss"
							, atmosphere.integrateScatteredLuminance( pixPos
							, ray
							, sunDir
							, sampleCountIni
							, depthBufferValue ) );
						outColour = vec4( ss.luminance(), 1.0_f );
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	AtmosphereSkyViewPass::AtmosphereSkyViewPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, CameraUbo const & cameraUbo
		, AtmosphereScatteringUbo const & atmosphereUbo
		, crg::ImageViewId const & transmittanceView
		, crg::ImageViewId const & resultView
		, uint32_t index
		, bool const & enabled )
		: castor::Named{ "SkyViewPass" + castor::string::toString( index ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), skyview::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), skyview::getPixelProgram( *device.renderSystem.getEngine(), getExtent( resultView ), getExtent( transmittanceView ) ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
	{
		auto renderSize = getExtent( resultView );
		auto & pass = graph.createPass( getName()
			, [this, &device, &enabled, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderSize( { renderSize.width, renderSize.height } )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.instances( renderSize.depth )
					.enabled( &enabled )
					.build( framePass, context, graph );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );
		cameraUbo.createPassBinding( pass
			, skyview::eCamera );
		atmosphereUbo.createPassBinding( pass
			, skyview::eAtmosphere );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR };
		pass.addSampledView( transmittanceView
			, skyview::eTransmittance
			, linearSampler );
		pass.addOutputColourView( resultView );
		m_lastPass = &pass;
	}

	void AtmosphereSkyViewPass::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	//************************************************************************************************
}
