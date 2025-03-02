#include "Castor3D/Render/Transform/MeshletBoundsTransformPass.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Transform/VertexTransforming.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"

#include <RenderGraph/RecordContext.hpp>

#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

CU_ImplementSmartPtr( castor3d, MeshletBoundsTransformPass )

namespace castor3d
{
	//*********************************************************************************************

	namespace vtxtrs
	{
		static ashes::DescriptorSetPtr createDescriptorSet( BoundsTransformPipeline const & pipeline
			, ObjectBufferOffset const & source
			, GpuBufferOffsetT< Meshlet > const & meshlets
			, GpuBufferOffsetT< MeshletCullData > const & output )
		{
			ashes::WriteDescriptorSetArray writes;
			writes.push_back( source.getStorageBinding( SubmeshFlag::ePositions
				, MeshletBoundsTransformPass::ePositions ) );

			if ( pipeline.normals )
			{
				writes.push_back( source.getStorageBinding( SubmeshFlag::eNormals
					, MeshletBoundsTransformPass::eNormals ) );
			}

			writes.push_back( meshlets.getStorageBinding( MeshletBoundsTransformPass::eMeshlets ) );
			writes.push_back( output.getStorageBinding( MeshletBoundsTransformPass::eOutCullData ) );

			auto descriptorSet = pipeline.descriptorSetPool->createDescriptorSet( pipeline.getName() );
			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}
	}

	//*********************************************************************************************

	MeshletBoundsTransformPass::MeshletBoundsTransformPass( RenderDevice const & device
		, SubmeshRenderNode const & node
		, BoundsTransformPipeline const & pipeline
		, ObjectBufferOffset const & source
		, GpuBufferOffsetT< Meshlet > const & meshlets
		, GpuBufferOffsetT< MeshletCullData > const & output )
		: m_device{ device }
		, m_pipeline{ pipeline }
		, m_source{ source }
		, m_meshlets{ meshlets }
		, m_output{ output }
		, m_meshletCount{ uint32_t( m_output.getCount() ) }
		, m_descriptorSet{ vtxtrs::createDescriptorSet( pipeline
			, source
			, m_meshlets
			, m_output ) }
	{
	}

	void MeshletBoundsTransformPass::recordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		context.memoryBarrier( commandBuffer
			, m_meshlets.buffer->getBuffer()
			, { m_meshlets.chunk.offset, m_meshlets.chunk.size }
			, VK_ACCESS_HOST_WRITE_BIT
			, VK_PIPELINE_STAGE_HOST_BIT
			, { VK_ACCESS_SHADER_READ_BIT
				, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );

		if ( m_output.buffer )
		{
			context.memoryBarrier( commandBuffer
				, m_output.buffer->getBuffer()
				, { m_output.chunk.offset, m_output.chunk.size }
				, VK_ACCESS_HOST_WRITE_BIT
				, VK_PIPELINE_STAGE_HOST_BIT
				, { VK_ACCESS_SHADER_WRITE_BIT
					, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
		}

		context.getContext().vkCmdBindPipeline( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipeline.pipeline );
		context.getContext().vkCmdPushConstants( commandBuffer
			, *m_pipeline.pipelineLayout
			, VK_SHADER_STAGE_COMPUTE_BIT
			, 0u
			, sizeof( m_meshletCount )
			, &m_meshletCount );
		VkDescriptorSet set = *m_descriptorSet;
		context.getContext().vkCmdBindDescriptorSets( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipeline.pipelineLayout
			, 0u
			, 1u
			, &set
			, 0u
			, nullptr );
		auto size = uint32_t( m_device.properties.limits.nonCoherentAtomSize );
		context.getContext().vkCmdDispatch( commandBuffer
			, uint32_t( ashes::getAlignedSize( m_output.getCount(), size ) ) / size
			, 1u
			, 1u );

		if ( m_output.buffer )
		{
			context.memoryBarrier( commandBuffer
				, m_output.buffer->getBuffer()
				, { m_output.chunk.offset, m_output.chunk.size }
				, VK_ACCESS_SHADER_WRITE_BIT
				, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
				, { VK_ACCESS_SHADER_READ_BIT
					, VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV } );
		}

		context.memoryBarrier( commandBuffer
			, m_meshlets.buffer->getBuffer()
			, { m_meshlets.chunk.offset, m_meshlets.chunk.size }
			, VK_ACCESS_SHADER_READ_BIT
			, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
			, { VK_ACCESS_SHADER_READ_BIT
				, VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV } );

		for ( auto & buffer : m_source.buffers )
		{
			if ( buffer.buffer )
			{
				context.memoryBarrier( commandBuffer
					, buffer.buffer->getBuffer()
					, { buffer.chunk.offset, buffer.chunk.size }
					, VK_ACCESS_SHADER_READ_BIT
					, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
					, { VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
						, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT } );
			}
		}
	}

	//*********************************************************************************************
}
