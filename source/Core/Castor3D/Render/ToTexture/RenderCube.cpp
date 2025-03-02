#include "Castor3D/Render/ToTexture/RenderCube.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/DirectUploadData.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/InstantUploadData.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Math/TransformationMatrix.hpp>

#include <ashespp/Core/Device.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineInputAssemblyStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineMultisampleStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineRasterizationStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

CU_ImplementSmartPtr( castor3d, RenderCube )

namespace castor3d
{
	namespace rendcube
	{
		static uint32_t constexpr MtxUboIdx = 0u;
		static uint32_t constexpr InputImgIdx = 1u;

		static SamplerObs doCreateSampler( RenderSystem & renderSystem
			, bool nearest )
		{
			castor::String const name = nearest
				? castor::String{ cuT( "RenderCube_Nearest" ) }
				: castor::String{ cuT( "RenderCube_Linear" ) };
			VkFilter const filter = nearest
				? VK_FILTER_NEAREST
				: VK_FILTER_LINEAR;
			auto & engine = *renderSystem.getEngine();
			SamplerObs result{};

			if ( engine.hasSampler( name ) )
			{
				result = engine.findSampler( name );
			}
			else if ( auto sampler = engine.addNewSampler( name, engine ) )
			{	
				sampler->setMinFilter( filter );
				sampler->setMagFilter( filter );
				sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result = sampler;
			}

			return result;
		}

		static UniformBufferUPtrT< castor::Matrix4x4f > doCreateMatrixUbo( RenderDevice const & device
			, ashes::Queue const & queue
			, ashes::CommandPool const & pool
			, bool srcIsCube )
		{
			static castor::Matrix4x4f const projection = device.renderSystem.getPerspective( 90.0_degrees, 1.0f, 0.1f, 10.0f );

			static std::array< castor::Matrix4x4f, 6u > const views = []()
			{
				std::array< castor::Matrix4x4f, 6u > result
				{
					castor::matrix::lookAt( castor::Point3f{ 0.0f, 0.0f, 0.0f }, castor::Point3f{ +1.0f, +0.0f, +0.0f }, castor::Point3f{ 0.0f, -1.0f, +0.0f } ),
					castor::matrix::lookAt( castor::Point3f{ 0.0f, 0.0f, 0.0f }, castor::Point3f{ -1.0f, +0.0f, +0.0f }, castor::Point3f{ 0.0f, -1.0f, +0.0f } ),
					castor::matrix::lookAt( castor::Point3f{ 0.0f, 0.0f, 0.0f }, castor::Point3f{ +0.0f, +1.0f, +0.0f }, castor::Point3f{ 0.0f, +0.0f, +1.0f } ),
					castor::matrix::lookAt( castor::Point3f{ 0.0f, 0.0f, 0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f }, castor::Point3f{ 0.0f, +0.0f, -1.0f } ),
					castor::matrix::lookAt( castor::Point3f{ 0.0f, 0.0f, 0.0f }, castor::Point3f{ +0.0f, +0.0f, +1.0f }, castor::Point3f{ 0.0f, -1.0f, +0.0f } ),
					castor::matrix::lookAt( castor::Point3f{ 0.0f, 0.0f, 0.0f }, castor::Point3f{ +0.0f, +0.0f, -1.0f }, castor::Point3f{ 0.0f, -1.0f, +0.0f } )
				};
				return result;
			}();
			auto result = makeUniformBuffer< castor::Matrix4x4f >( device.renderSystem
				, 6u
				, ( VK_BUFFER_USAGE_TRANSFER_DST_BIT
					| VK_BUFFER_USAGE_TRANSFER_SRC_BIT )
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "RenderCubeMatrix" );

			for ( uint32_t i = 0u; i < 6u; ++i )
			{
				result->getData( i ) = projection * views[i];
			}

			result->initialise( device );
			result->upload( 0u, 6u );
			return result;
		}

		static ashes::VertexBufferPtr< castor::Point4f > doCreateVertexBuffer( RenderDevice const & device
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool )
		{
			std::vector< castor::Point4f > vertexData
			{
				castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ -1, -1, -1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ +1, +1, -1, +1 },// Back
				castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ -1, -1, -1, +1 }, castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ -1, +1, +1, +1 },// Left
				castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ +1, -1, +1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ +1, +1, -1, +1 },// Right
				castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ -1, +1, +1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ +1, -1, +1, +1 },// Front
				castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ +1, +1, -1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ -1, +1, +1, +1 },// Top
				castor::Point4f{ -1, -1, -1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ +1, -1, +1, +1 }, castor::Point4f{ -1, -1, +1, +1 },// Bottom
			};
			auto result = makeVertexBuffer< castor::Point4f >( device
				, uint32_t( vertexData.size() )
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "RenderCube" );
			{
				InstantDirectUploadData uploader{ queue
					, device
					, "RenderCube"
					, commandPool };
				uploader->pushUpload( vertexData.data()
					, result->getSize()
					, result->getBuffer()
					, 0u
					, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			}
			return result;
		}

		static ashes::PipelineVertexInputStateCreateInfo doCreateVertexLayout()
		{
			return ashes::PipelineVertexInputStateCreateInfo
			{
				0u,
				{
					{ 0u, uint32_t( sizeof( castor::Point4f ) ), VK_VERTEX_INPUT_RATE_VERTEX },
				},
				{
					{ 0u, 0u, VK_FORMAT_R32G32B32A32_SFLOAT, 0u }
				},
			};
		}
	}

	RenderCube::RenderCube( RenderDevice const & device
		, bool nearest
		, SamplerObs sampler )
		: m_device{ device }
		, m_sampler{ ( sampler
			? std::move( sampler )
			: rendcube::doCreateSampler( m_device.renderSystem, nearest ) ) }
	{
	}

	RenderCube::~RenderCube()
	{
		cleanup();
	}

	void RenderCube::createPipelines( VkExtent2D const & size
		, castor::Position const & position
		, ashes::PipelineShaderStageCreateInfoArray const & program
		, ashes::ImageView const & view
		, ashes::RenderPass const & renderPass
		, ashes::VkPushConstantRangeArray const & pushRanges )
	{
		createPipelines( size
			, position
			, program
			, view
			, renderPass
			, pushRanges
			, ashes::PipelineDepthStencilStateCreateInfo{ 0u, false, false } );
	}

	void RenderCube::createPipelines( VkExtent2D const & size
		, castor::Position const & position
		, ashes::PipelineShaderStageCreateInfoArray const & program
		, ashes::ImageView const & view
		, ashes::RenderPass const & renderPass
		, ashes::VkPushConstantRangeArray const & pushRanges
		, ashes::PipelineDepthStencilStateCreateInfo const & dsState )
	{
		auto queueData = m_device.graphicsData();
		m_sampler->initialise( m_device );
		m_matrixUbo = rendcube::doCreateMatrixUbo( m_device
			, *queueData->queue
			, *queueData->commandPool
			, view->viewType == VK_IMAGE_VIEW_TYPE_CUBE );
		m_vertexBuffer = rendcube::doCreateVertexBuffer( m_device
			, *queueData->queue
			, *queueData->commandPool );
		auto vertexLayout = rendcube::doCreateVertexLayout();

		// Initialise the descriptor set.
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( rendcube::MtxUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ),
			makeDescriptorSetLayoutBinding( rendcube::InputImgIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		doFillDescriptorLayoutBindings( bindings );
		m_descriptorLayout = m_device->createDescriptorSetLayout( "RenderCube"
			, std::move( bindings ) );
		m_pipelineLayout = m_device->createPipelineLayout( "RenderCube"
			, { *m_descriptorLayout }, pushRanges );
		m_descriptorPool = m_descriptorLayout->createPool( "RenderCube", 6u );
		uint32_t face = 0u;

		for ( auto & facePipeline : m_faces )
		{
			facePipeline.pipeline = m_device->createPipeline( "RenderCubeFace" + castor::string::toString( face )
				, ashes::GraphicsPipelineCreateInfo
				{
					0u,
					program,
					vertexLayout,
					ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST },
					ashes::nullopt,
					ashes::PipelineViewportStateCreateInfo{ 0u, 1u, { VkViewport{ 0.0f, 0.0f, float( size.width ), float( size.height ), 0.0f, 1.0f } }, 1u, { VkRect2D{ 0, 0, size.width, size.height } } },
					ashes::PipelineRasterizationStateCreateInfo{ 0u, false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE },
					ashes::PipelineMultisampleStateCreateInfo{},
					ashes::PipelineDepthStencilStateCreateInfo{ 0u, false, false },
					ashes::PipelineColorBlendStateCreateInfo{},
					ashes::nullopt,
					*m_pipelineLayout,
					renderPass,
				} );
			facePipeline.descriptorSet = m_descriptorPool->createDescriptorSet( "RenderCubeFace" + castor::string::toString( face ) );
			facePipeline.descriptorSet->createSizedBinding( m_descriptorLayout->getBinding( 0u )
				, m_matrixUbo->getBuffer()
				, face
				, 1u );
			facePipeline.descriptorSet->createBinding( m_descriptorLayout->getBinding( 1u )
				, view
				, m_sampler->getSampler() );
			doFillDescriptorSet( *m_descriptorLayout, *facePipeline.descriptorSet, face );
			facePipeline.descriptorSet->update();
			++face;
		}
	}

	void RenderCube::cleanup()
	{
		m_commandBuffer.reset();

		for ( auto & facePipeline : m_faces )
		{
			facePipeline.descriptorSet.reset();
			facePipeline.pipeline.reset();
		}

		m_descriptorPool.reset();
		m_pipelineLayout.reset();
		m_descriptorLayout.reset();
		m_matrixUbo->cleanup();
		m_matrixUbo.reset();
		m_vertexBuffer.reset();
	}

	void RenderCube::prepareFrame( ashes::RenderPass const & renderPass
		, uint32_t subpassIndex
		, uint32_t face )
	{
		m_commandBuffer = m_device.graphicsData()->commandPool->createCommandBuffer( "RenderCube"
			, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( renderPass
				, subpassIndex
				, VK_NULL_HANDLE
				, VK_FALSE
				, 0u
				, 0u ) );
		registerFrame( *m_commandBuffer, face );
		m_commandBuffer->end();
	}

	void RenderCube::registerFrame( ashes::CommandBuffer & commandBuffer
		, uint32_t face )const
	{
		auto & facePipeline = m_faces[face];
		commandBuffer.bindPipeline( *facePipeline.pipeline );
		commandBuffer.bindDescriptorSet( *facePipeline.descriptorSet, *m_pipelineLayout );
		commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		doRegisterFrame( commandBuffer, face );
		commandBuffer.draw( 36u );
	}

	void RenderCube::doFillDescriptorLayoutBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )
	{
	}

	void RenderCube::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet
		, uint32_t face )
	{
	}

	void RenderCube::doRegisterFrame( ashes::CommandBuffer & commandBuffer
		, uint32_t face )const
	{
	}
}
