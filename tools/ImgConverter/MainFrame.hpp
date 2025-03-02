/* See LICENSE file in root folder */
#ifndef ___IX_MainFrame___
#define ___IX_MainFrame___

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#pragma warning( disable: 5054 )
#include <wx/frame.h>
#include <wx/listbox.h>
#include <wx/button.h>
#pragma warning( pop )

namespace ImgToIco
{
	class MainFrame
		: public wxFrame
	{
	private:
		enum eIDs
		{
			eProcess,
			eBrowse,
			eExit,
		};

		wxListBox * m_pListImages;
		wxButton * m_pButtonProcess;
		wxButton * m_pButtonBrowse;
		wxButton * m_pButtonExit;

	public:
		MainFrame();

	protected:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void _onBrowse( wxCommandEvent & event );
		void _onProcess( wxCommandEvent & event );
		void _onExit( wxCommandEvent & event );
	};
}

#endif
