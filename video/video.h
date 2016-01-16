#pragma once

#include <boost/asio.hpp>
#include <wx/wx.h>
#include <wx/fileconf.h>
#include <wx/aui/aui.h>
#include <wx/listctrl.h>
#include <wx/wupdlock.h>
#include <wx/event.h>

//boost
#include <boost/shared_ptr.hpp>

//#include "options.h"

#include "dbmodule.h"
//#include "V4fSynchronization.h"
#include "VCoordinatesThread.h"
#include "VCodeDPPThread.h"
#include "VCameraProperties.h"
#include "V4fViewerDialog.h"
#include "VCameraControl.h"
#include "VMainPropDialog.h"
#include "VCameraPropDialog.h"
#include "VVideoOptions.h"
#include "revision.h"

/** �������� �������*/
#define ID_SET_LOG_LEVEL 600
class VLogViewer : public wxListCtrl
{
private:
	
	/** ��������� ������ ��������� �� ������*/
	wxListItemAttr error_attr;
	/** ��������� ������ ��������� �� ������*/
	wxListItemAttr warning_attr;
	/** ��������� ������ ����������*/
	wxListItemAttr info_attr;
	/** ��������� ������ �������*/
	wxListItemAttr debug_attr;
	/** ����������� ����*/
	wxMenu popup_menu;	

	/** ���������� ������� �����*/
	void OnRightClick(wxListEvent& in_event);
	/** ���������� ������� ��������������*/
	void OnSetLogLevel(wxCommandEvent & in_event);
	/** ���������� UI*/
	void OnUpdateUI(wxUpdateUIEvent& evt);
	
public:

	/** �����������*/
	VLogViewer(wxWindow * in_parent, wxWindowID in_id);
	/** ���������� ���������� ������*/
	virtual wxString OnGetItemText(long item, long column) const;
	/** ���������� ��������� ������*/
	virtual wxListItemAttr * OnGetItemAttr(long item) const;
	/** ��������� ���*/
	void Refresh();	

	DECLARE_EVENT_TABLE();
};
typedef boost::shared_ptr<wxFileConfig> wxFileConfigPtr;

#define ID_QUIT 1
#define ID_ADD_CAMERA 2
#define ID_PROPERTIES 5
#define ID_PROPERTIES_CAMS 6
#define ID_VIEW_V4F 7
#define ID_RIGHT_CLICK 8
#define ID_ABOUT 9
#define ID_CAM 8610

enum
{
    ID_TOGGLE_STATUSBAR = 10001,
    ID_SPLITTER_WINDOW,
	///������ �������
	ID_FOLDER_BUTTON,
	///������ ��, ������ � ���������
	ID_OK_BUTTON,
	ID_CANCEL_BUTTON,
	ID_APPLY_BUTTON,   
	///Radiobutton ��� ������ ���� ������
	ID_IPCHECK,
	ID_OLDCHECK,
	///������ ������ � ���� ���������� ��������������� �����
	ID_CHECKLISTBOX,
	ID_COMBO,

	///�������������� ��������� �������� ����
	ID_LOG	
};

/// ������� ����
class VMainFrame : public wxFrame
{
public:
	/// �����������
	VMainFrame(wxWindow * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString,
		const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxCLOSE_BOX|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|
		wxSYSTEM_MENU|wxCAPTION|wxRAISED_BORDER|wxRESIZE_BORDER);
	/// ����������
	~VMainFrame();
	
	/// ����������� ����
	void SetupMenu();
	/// ��������� ������
	void AddCamera(VCameraBase * in_camera_props, int cameraId);
	///���������� ������ ����� � ������� ����
	void UpdateList(wxCommandEvent &ce);
private:
	VRegisteredDatabase * RegisteredDatabase;

	///���������� ��� ��������� ��������� ���� �����
	wxWindow *buffWindow;
	/// ������ � �����������
	std::vector<VCameraBase *> cameraProperties;
	/// ����� ������������� ������� ����������� �����
	std::vector<bool> cameraControlsExisting;
	/// ID ���� ���������� ��������
	std::vector<int> cameraWndId;
	/// ������� ����
	long cameraWndCounter;
	/// AuiManager
	wxAuiPaneInfo CameraPaneInfo;
	/// ������
	VLogViewer * LogViewer;
	/// ����
	wxMenuBar* MenuBar;
	/// ������ ��������� ���������
	void OnProperties(wxCommandEvent& event);
	/// ������ ��������� �����
	void OnPropertiesCams(wxCommandEvent& event);
	/// �������� ������ ������� c4v
	void OnViewC4v(wxCommandEvent& event);
	/// ����� ���� "�����"
	void OnQuit(wxCommandEvent& event);
	/// ������� � ���������
	void OnAbout(wxCommandEvent& event);
	/// �������� ���� 
	void OnClose(wxCloseEvent& event);
	/// ���������� UI
	void OnUpdateUI(wxUpdateUIEvent& event);
	///�������� ������ �� ������ �� ������
	void OnTurnCamera(wxCommandEvent& event);
	///������� ���������� ������ ��������� �� ��������� RT
	//void OnCoordinatesThreadCompletion(wxThreadEvent&);
protected:
	/// �������� ���������
	VCoordinatesThread * coordinates;
	/// ����� ��������� ������ � ������� ���������
	void OnCompleteCoordThread(wxThreadEvent & event);
	/// �������� ����� ���
	VCodeDPPThread * dpp;
	/// ����� ��������� ������ � ������� ����� ���
	void OnCompleteDppThread(wxThreadEvent & event);
	DECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(VMainFrame, wxFrame)	
	EVT_CLOSE(VMainFrame::OnClose)
	EVT_MENU(ID_PROPERTIES_CAMS, VMainFrame::OnPropertiesCams)
	EVT_MENU(ID_PROPERTIES, VMainFrame::OnProperties)
	EVT_MENU(ID_VIEW_V4F, VMainFrame::OnViewC4v)
	EVT_MENU(ID_QUIT, VMainFrame::OnQuit)
	EVT_MENU(ID_ABOUT, VMainFrame::OnAbout)
	EVT_UPDATE_UI(ID_LOG, VMainFrame::OnUpdateUI)	
wxEND_EVENT_TABLE()

/// ����� "���������� wxWidgets"
class VApp : public wxApp
{
private:	
public:
	/// �����, ����������� ������ ����������
	virtual bool OnInit();	
};

DECLARE_APP(VApp);
IMPLEMENT_APP(VApp);

DECLARE_VARIABLE(std::wstring, config_folder, VFileName(VFileName::GetHomeDir(), L"\\AppData\\Roaming\\video\\config\\").GetPath())
#define CONFIG_FOLDER config_folder().Value()
DECLARE_VARIABLE(std::wstring, log_folder, VFileName(VFileName::GetHomeDir(), L"\\AppData\\Roaming\\video\\log\\").GetPath())
#define LOG_FOLDER log_folder().Value()