#include "Castor3D/Render/Passes/BackgroundPassBase.hpp"

#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Background/Background.hpp"

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnableGraph.hpp>

#include <ashes/ashes.hpp>

namespace castor3d
{
	//*********************************************************************************************

	BackgroundPassBase::BackgroundPassBase( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, SceneBackground & background
		, bool forceVisible )
		: m_device{ device }
		, m_background{ &background }
		, m_viewport{ *device.renderSystem.getEngine() }
		, m_onBackgroundChanged{ background.onChanged.connect( [this, forceVisible]( SceneBackground const & bg )
			{
				doResetPipeline( bg.getPassIndex( forceVisible ) );
			} ) }
	{
	}

	void BackgroundPassBase::update( CpuUpdater & updater )
	{
		updater.viewport = &m_viewport;
		m_background->update( updater );
	}

	void BackgroundPassBase::update( GpuUpdater & updater )
	{
		m_background->update( updater );
	}

	bool BackgroundPassBase::doIsEnabled()const
	{
		return m_background->isInitialised();
	}

	//*********************************************************************************************
}
