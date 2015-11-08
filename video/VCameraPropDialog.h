#pragma once
#include <boost/asio.hpp>

#include <wx/aui/aui.h>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>
#include <wx/aui/auibook.h>

#include "xpm.h"
#include "VLog.h"
#include "options.h"
#include "VFileName.h"

#include <string> 
#include "strlib.h"
#include "VCameraProperties.h"

#define ID_CHOOSE_DIRECTORY 100
#define ID_RTSPRB 101
#define ID_JAIRB 102
#define ID_TOOLBAR 103
#define ID_ADD_BUTTON 104
#define ID_REMOVE_BUTTON 105
#define ID_LIST_CTRL 106

///����� ��� ����������� �������� ������� ������
#define ID_NAME_PROP 108
#define ID_DIRECTORY_PROP ID_NAME_PROP + 1
#define ID_ADDRESS_PROP ID_DIRECTORY_PROP + 1
#define ID_NUM_PROP ID_ADDRESS_PROP + 1

//������� ���������
class BaseProp : public wxPanel
{
	//��������� �� ������ � �����������, ������� �������� �������������
	std::wstring * directory_str;
	//��������� �� ������ � ������ ������, ������� �������� �������������
	std::wstring * name_str;
public:
	///���� ��� ����� ����� ������
	wxTextCtrl *name;
	///���� ��� ����� ���� � ����� ��� �������
	//wxTextCtrl *directory;
	///������ ��� ������ �����
	//wxButton * chooseDir;
	//���������� ������ �����
	//void OnChooseDirectory(wxCommandEvent &event);
	void UpdateNameProp(wxCommandEvent &event);
	//void UpdateDirectoryProp(wxCommandEvent &event);
	BaseProp(wxWindow* parent);
	//���������� ������� ��������
	void setProp(VCameraBase * prop);
	DECLARE_EVENT_TABLE();
};

///����� ������� IP-������
class RTSPProp : public wxPanel
{
	//��������� �� ������ � �����������, ������� �������� �������������
	//std::wstring * directory_str;
	//��������� �� ������ � ������ ������, ������� �������� �������������
	std::wstring * name_str;
	//�����
	std::wstring * address_str;
public:
	wxStaticBoxSizer * vbox;
	///���� ��� ����� ����� ������
	wxTextCtrl * name;
	///���� ��� ����� ���� � ����� ��� �������
	//wxTextCtrl *directory;
	///������ ��� ������ �����
	//wxButton * chooseDir;
	///���� ��� ����� ������ ������
	wxTextCtrl * address;
	///�����������
	RTSPProp(wxWindow *parent);
	///���������� ����
	void UpdateProp(wxCommandEvent &event);
	void UpdateNameProp(wxCommandEvent &event);
	//void UpdateDirectoryProp(wxCommandEvent &event);
	//���������� ��������� �� ������ � ������� ������
	void setProp(VRTSPCameraProperties * prop);
	//���������� ������ �����
	//void OnChooseDirectory(wxCommandEvent &event);
	DECLARE_EVENT_TABLE();
};

///����� ������� USB-������
class JAIProp : public wxPanel
{
	//��������� �� ������ � �����������, ������� �������� �������������
	//std::wstring * directory_str;
	//��������� �� ������ � ������ ������, ������� �������� �������������
	std::wstring * name_str;
	/// ����� ������ 
	int * numCameraPtr;
public:
	wxStaticBoxSizer * vbox;
	///���� ��� ����� ����� ������
	wxTextCtrl *name;
	///���� ��� ����� ���� � ����� ��� �������
	//wxTextCtrl *directory;
	///������ ��� ������ �����
	//wxButton * chooseDir;
	/// ���� ��� ����� ������ ������
	wxTextCtrl * numCamera;
	///�����������
	JAIProp(wxWindow *parent);
	///���������� ����
	void UpdateProp(wxCommandEvent &event);
	void UpdateNameProp(wxCommandEvent &event);
	//void UpdateDirectoryProp(wxCommandEvent &event);
	//���������� ��������� �� ������ � ������� ������
	void setProp(VJAICameraProperties * prop);
	//���������� ������ �����
	//void OnChooseDirectory(wxCommandEvent &event);
	DECLARE_EVENT_TABLE();
};

///���� �������� ����� 
class VCameraPropDialog : public wxDialog
{
public:
	/// �����������
	VCameraPropDialog(wxWindow * parent, std::vector<VCameraBase*> & _prop);
	/// ���������� ������ �� ����� ���������� �����������
	std::vector<VCameraBase*> & GetCameraProperties();
private:

	///����������� ������ � ����������� �����
	std::vector<VCameraBase*> prop;
	///������� ������ � ����������� �����
	std::vector<VCameraBase*> temp_prop;
	int current_prop;
	int selected_type_setup;

	wxToolBar * toolBar;
	///������� ��� ����������� ������ �����
	wxListCtrl *camList;
	wxSplitterWindow * m_SplitterWindow;

	///������ ������ ������ ������ �����
	wxRadioButton *RtspRadioButton;
	wxRadioButton *JaiRadioButton;
	///�������� �������� ��� ������ ����� �����
	wxAuiNotebook * NoteBook;
	RTSPProp * rtspPropPanel;
	JAIProp  * jaiPropPanel;

	///������ �������������/������ ����������� ��������
	wxButton* okButton;
	wxButton* cancelButton;
	wxButton* applyButton;

	void OnPageChanging(wxAuiNotebookEvent & evt);
	void setTypeCamera(wxCommandEvent& event);

	///��������� ������� ������ ���������� ������� �����
	void OnAddTool(wxCommandEvent &event);
	void OnRemoveTool(wxCommandEvent &event);

	///��������� ������� ������ ������������� � ������ ��������
	void OnOkButton(wxCommandEvent &event);
	void OnCancelButton(wxCommandEvent &event);
	void OnApplyButton(wxCommandEvent &event);

	///��������� ����� �� ������ �����
	void OnListClick(wxListEvent &event);

	///���������� ��������� ����������
	void OnUpdateUI(wxUpdateUIEvent& evt);
	void OnClose(wxCloseEvent& event);
	DECLARE_EVENT_TABLE();
};