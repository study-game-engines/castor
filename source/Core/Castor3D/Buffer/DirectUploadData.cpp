#include "Castor3D/Buffer/DirectUploadData.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Command/CommandPool.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Queue.hpp>

namespace castor3d
{
	DirectUploadData::DirectUploadData( RenderDevice const & device
		, std::string debugName
		, ashes::CommandBuffer const & commandBuffer )
		: CommandBufferHolder{ nullptr }
		, UploadData{ device, std::move( debugName ), &commandBuffer }
	{
	}

	DirectUploadData::DirectUploadData( RenderDevice const & device
		, std::string debugName
		, ashes::CommandPool const & commandPool )
		: CommandBufferHolder{ commandPool.createCommandBuffer( debugName ) }
		, UploadData{ device, debugName, CommandBufferHolder::getData().get() }
	{
	}

	void DirectUploadData::doBegin()
	{
		m_commandBuffer->begin();
	}

	VkDeviceSize DirectUploadData::doUpload( BufferDataRange & data )
	{
		doUploadBuffer( data, nullptr, 0u );
		return data.srcSize;
	}

	VkDeviceSize DirectUploadData::doUpload( ImageDataRange & data )
	{
		log::debug << "    Registering image upload commands: [" << data.dstImage->getName()
			<< "], Layout: [" << data.dstLayout
			<< "], Range: [" << data.dstRange
			<< ", Upload Size: " << data.srcSize
			<< std::endl;
		auto mappedSize = ashes::getAlignedSize( data.srcSize
			, m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) );
		auto & buffer = *m_buffers.emplace_back( makeBufferBase( m_device
			, mappedSize
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, m_debugName + "/StagingBuffer" ) );

		if ( doCopyData( data.srcData, data.srcSize, buffer, 0u ) )
		{
			m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, buffer.makeTransferSource() );
			doUploadImage( data, buffer, 0u );
		}

		return data.srcSize;
	}

	UploadData::SemaphoreUsed DirectUploadData::doEnd( ashes::Queue const & queue
		, ashes::Fence const * fence
		, castor::Milliseconds timeout )
	{
		m_commandBuffer->end();
		queue.submit( getCommandBuffer(), fence );

		if ( fence )
		{
			fence->wait( uint64_t( timeout.count() ) );
		}

		return {};
	}
}
