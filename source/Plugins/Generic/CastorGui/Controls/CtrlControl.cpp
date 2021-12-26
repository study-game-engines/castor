#include "CastorGui/Controls/CtrlControl.hpp"

#include "CastorGui/ControlsManager.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	Control::Control( ControlType p_type
		, String const & p_name
		, Engine & engine
		, ControlRPtr p_parent
		, uint32_t p_id
		, Position const & p_position
		, Size const & p_size
		, uint32_t p_flags
		, bool p_visible )
		: NonClientEventHandler< Control >( p_type != ControlType::eStatic )
		, Named( p_name )
		, m_parent( p_parent )
		, m_cursor( MouseCursor::eHand )
		, m_id( p_id )
		, m_type( p_type )
		, m_flags( p_flags )
		, m_position( p_position )
		, m_size( p_size )
		, m_borders( 0, 0, 0, 0 )
		, m_engine( engine )
	{
		OverlayRPtr parentOv{};
		ControlRPtr parent = getParent();

		if ( parent )
		{
			auto bg = parent->getBackground();

			if ( !bg )
			{
				CU_Exception( "No background set" );
			}

			parentOv = &bg->getOverlay();
		}

		auto overlay = getEngine().getOverlayCache().add( p_name
			, getEngine()
			, OverlayType::eBorderPanel
			, nullptr
			, parentOv ).lock();

		if ( !overlay )
		{
			CU_Exception( "No background set" );
		}

		overlay->setPixelPosition( getPosition() );
		overlay->setPixelSize( getSize() );
		BorderPanelOverlaySPtr panel = overlay->getBorderPanelOverlay();
		panel->setBorderPosition( BorderPosition::eInternal );
		panel->setVisible( p_visible );
		m_background = panel;
	}

	void Control::create( ControlsManagerSPtr p_ctrlManager )
	{
		m_ctrlManager = p_ctrlManager;
		ControlRPtr parent = getParent();

		if ( parent )
		{
			parent->m_children.push_back( std::static_pointer_cast< Control >( shared_from_this() ) );
		}

		BorderPanelOverlaySPtr panel = getBackground();
		panel->setMaterial( getBackgroundMaterial() );
		panel->setBorderMaterial( getForegroundMaterial() );
		panel->setBorderPixelSize( m_borders );
		doCreate();
	}

	void Control::destroy()
	{
		doDestroy();
	}

	void Control::setBackgroundBorders( castor::Rectangle const & p_value )
	{
		m_borders = p_value;
		BorderPanelOverlaySPtr panel = getBackground();

		if ( panel )
		{
			panel->setBorderPixelSize( m_borders );
		}
	}

	void Control::setPosition( Position const & p_value )
	{
		m_position = p_value;
		BorderPanelOverlaySPtr panel = getBackground();

		if ( panel )
		{
			panel->setPixelPosition( m_position );
			panel.reset();
		}

		doSetPosition( m_position );
	}

	Position Control::getAbsolutePosition()const
	{
		ControlRPtr parent = getParent();
		Position result = m_position;

		if ( parent )
		{
			result += parent->getAbsolutePosition();
		}

		return result;
	}

	void Control::setSize( Size const & p_value )
	{
		m_size = p_value;
		BorderPanelOverlaySPtr panel = getBackground();

		if ( panel )
		{
			panel->setPixelSize( m_size );
			panel.reset();
		}

		doSetSize( m_size );
	}

	void Control::setBackgroundMaterial( MaterialRPtr p_value )
	{
		CU_Require( p_value );
		m_backgroundMaterial = p_value;
		BorderPanelOverlaySPtr panel = getBackground();

		if ( panel )
		{
			panel->setMaterial( getBackgroundMaterial() );
			panel.reset();
		}

		doSetBackgroundMaterial( getBackgroundMaterial() );
	}

	void Control::setForegroundMaterial( MaterialRPtr p_value )
	{
		m_foregroundMaterial = p_value;
		BorderPanelOverlaySPtr panel = getBackground();

		if ( panel )
		{
			panel->setBorderMaterial( getForegroundMaterial() );
			panel.reset();
		}

		doSetForegroundMaterial( getForegroundMaterial() );
	}

	void Control::setCaption( castor::String const & p_caption )
	{
		doSetCaption( p_caption );
	}

	void Control::setVisible( bool p_value )
	{
		BorderPanelOverlaySPtr panel = getBackground();

		if ( !panel )
		{
			CU_Exception( "No background set" );
		}

		panel->setVisible( p_value );
		panel.reset();
		doSetVisible( p_value );
	}

	bool Control::isVisible()const
	{
		BorderPanelOverlaySPtr panel = getBackground();

		if ( !panel )
		{
			CU_Exception( "No background set" );
		}

		bool visible = panel->isVisible();
		ControlRPtr parent = getParent();

		if ( visible && parent )
		{
			visible = parent->isVisible();
		}

		return visible;
	}

	ControlSPtr Control::getChildControl( uint32_t p_id )
	{
		auto it = std::find_if( std::begin( m_children )
			, std::end( m_children )
			, [&p_id]( ControlWPtr p_ctrl )
			{
				auto ctrl = p_ctrl.lock();
				return ctrl
					? ( ctrl->getId() == p_id )
					: false;
			} );

		if ( it == m_children.end() )
		{
			CU_Exception( "This control does not exist in my childs" );
		}

		return it->lock();
	}

	void Control::addFlag( uint32_t p_flags )
	{
		m_flags |= p_flags;
		doUpdateFlags();
	}

	void Control::removeFlag( uint32_t p_flags )
	{
		m_flags &= ~p_flags;
		doUpdateFlags();
	}

	bool Control::doIsVisible()const
	{
		auto panel = getBackground();

		if ( !panel )
		{
			CU_Exception( "No background set" );
		}

		return panel->isVisible();
	}
}