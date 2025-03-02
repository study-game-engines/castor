#pragma once

#include "CastorDvpTDPrerequisites.hpp"

#include <wx/defs.h>
#include <wx/frame.h>
#include <wx/windowptr.h>

namespace castortd
{
	class MainFrame
		: public wxFrame
	{
	public:
		MainFrame();

	private:
		void doLoadScene();

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
#pragma clang diagnostic ignored "-Wsuggest-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void OnPaint( wxPaintEvent  & event );
		void OnClose( wxCloseEvent  & event );
		void OnEraseBackground( wxEraseEvent & event );
		void OnRenderTimer( wxTimerEvent & event );
		void onKeyDown( wxKeyEvent & event );
		void onKeyUp( wxKeyEvent & event );
		void OnMouseLdown( wxMouseEvent & event );
		void OnMouseLUp( wxMouseEvent & event );
		void OnMouseRUp( wxMouseEvent & event );
		void OnMouseWheel( wxMouseEvent & event );

	private:
		wxWindowPtr< RenderPanel > m_panel;
		std::unique_ptr< Game > m_game;
		wxTimer * m_timer{ nullptr };
	};
}
