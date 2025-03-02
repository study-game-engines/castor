/*
See LICENSE file in root folder
*/
#ifndef ___MainFrame___
#define ___MainFrame___

#include <GuiCommon/GuiCommonPrerequisites.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>
#include <wx/aui/auibar.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#include <GuiCommon/GuiCommonPrerequisites.hpp>

#include <GuiCommon/System/Recorder.hpp>
#include <GuiCommon/System/SceneObjectsList.hpp>

#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Data/Path.hpp>

#if defined( __WXOSX_COCOA__ )
#	define CV_MainFrameToolbar 0
#else
#	define CV_MainFrameToolbar 1
#endif

namespace CastorViewer
{
	class RenderPanel;

	struct LogContainer
	{
		std::vector< std::pair< wxString, bool > > queue;
		std::mutex mutex;
		wxListBox * listBox{ nullptr };
	};

	typedef enum eBMP
	{
		eBMP_SCENES = GuiCommon::eBMP_COUNT,
		eBMP_MATERIALS,
		eBMP_EXPORT,
		eBMP_LOGS,
		eBMP_PROPERTIES,
		eBMP_PRINTSCREEN
	}	eBMP;

	class MainFrame
		: public wxFrame
	{
	public:
		MainFrame( wxString const & title );
		~MainFrame()override;

		bool initialise( GuiCommon::SplashScreen & splashScreen );
		void loadScene( wxString const & fileName = wxEmptyString );
		void toggleFullScreen( bool fullscreen );
		void select( castor3d::GeometryRPtr geometry, castor3d::Submesh const * submesh );

	private:
		void doInitialiseTimers();
		void doInitialiseGUI();
		bool doInitialiseImages();
		void doPopulateStatusBar();
		void doPopulateToolBar( GuiCommon::SplashScreen & splashScreen );
		void doInitialisePerspectives();
		void doLogCallback( castor::String const & log, castor::LogType type, bool newLine );
		void doCleanupScene();
		void doSaveFrame();
		bool doStartRecord();
		void doRecordFrame();
		void doStopRecord();
		void doSceneLoadEnd( castor3d::RenderWindowDesc const & window );

	private:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void onRenderTimer( wxTimerEvent & event );
		void onTimer( wxTimerEvent & event );
		void onFpsTimer( wxTimerEvent & event );
		void onPaint( wxPaintEvent & event );
		void onSize( wxSizeEvent & event );
		void onInit( wxInitDialogEvent & event );
		void onClose( wxCloseEvent  & event );
		void onEnterWindow( wxMouseEvent & event );
		void onLeaveWindow( wxMouseEvent & event );
		void onEraseBackground( wxEraseEvent & event );
		void onKeyUp( wxKeyEvent & event );
		void onLoadScene( wxCommandEvent & event );
		void onExportScene( wxCommandEvent & event );
		void onShowLogs( wxCommandEvent & event );
		void onShowLists( wxCommandEvent & event );
		void onPrintScreen( wxCommandEvent & event );
		void onRecord( wxCommandEvent & event );
		void onStop( wxCommandEvent & event );
		void onSceneLoadEnd( wxThreadEvent & event );

	private:
		int m_logsHeight{ 100 };
		int m_propertiesWidth{ 240 };
		wxAuiManager m_auiManager;
		RenderPanel * m_renderPanel{ nullptr };
		wxTimer * m_timer{ nullptr };
		wxTimer * m_fpsTimer{ nullptr };
#if CV_MainFrameToolbar
		wxAuiToolBar * m_toolBar{ nullptr };
#else
		wxMenu * m_fileMenu{ nullptr };
		wxMenu * m_tabsMenu{ nullptr };
		wxMenu * m_captureMenu{ nullptr };
		wxMenuBar * m_menuBar{ nullptr };
#endif
		wxAuiNotebook * m_logTabsContainer{ nullptr };
		wxAuiNotebook * m_sceneTabsContainer{ nullptr };
		LogContainer m_messageLog;
		LogContainer m_errorLog;
#ifndef NDEBUG
		LogContainer m_debugLog;
#endif
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsList > * m_sceneTree{ nullptr };
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsList > * m_objectsTree{ nullptr };
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsList > * m_nodesTree{ nullptr };
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsList > * m_lightsTree{ nullptr };
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsList > * m_materialsTree{ nullptr };
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsList > * m_overlaysTree{ nullptr };
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsList > * m_guiTree{ nullptr };
		castor3d::SceneRPtr m_mainScene{};
		castor3d::CameraRPtr m_mainCamera{};
		castor3d::SceneNodeRPtr m_sceneNode{};
		castor::Path m_filePath;
		wxString m_currentPerspective;
		wxString m_fullScreenPerspective;
		wxString m_debugPerspective;
		wxTimer * m_timerErr{ nullptr };
		wxTimer * m_timerMsg{ nullptr };
		GuiCommon::Recorder m_recorder;
		int m_recordFps{ 0 };
		wxString m_title;
		uint32_t m_minCount{};
		uint32_t m_maxCount{};
	};
}

#endif
