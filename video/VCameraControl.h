#pragma once

#include <boost/asio.hpp>
#include <wx/wx.h>

#include <string> 
#include "strlib.h"

#include "VCaptureThread.h"
#include "VCameraProperties.h"
#include "xpm.h"

#define ID_START_RECORDING 10201
#define ID_STOP_RECORDING 10202
#define ID_TOOLBAR_CAMERACONTROL 10203

//extern int NotConnectedWidth;
//extern int NotConnectedHeight;
///��������� ��������� ������
extern bool isConnected;

/// ���� ���������� �������
class VCameraControl : public wxFrame
{
	///����� ��� ������
	//std::wstring directory;
	/// ������
	wxSizer * sizer;
	/// ������ ������������
	wxToolBar * toolbar;
	///����� ������ �����
	int type_rec;
	/// �����
	wxPanel * screen;
	/// ����� �������
	VCaptureThread * capture;
	///������
	wxSizer *hbox;
	///������� �����
	time_t now;
	///������ ������� ������ �����
	bool isRecording;
	///�������� ������
	int intervalRecording;
	/// ������ ������ � ����
	void OnStartRecording(wxCommandEvent & in_event);
	/// ���������� ������ � ����
	void OnStopRecording(wxCommandEvent & in_event);
	/// ���������� ������� UPDATE_UI
	void OnUpdateUI(wxUpdateUIEvent & in_event);
	/// ��������� ����������� ����
	void OnPaint(wxPaintEvent& event);
	/// ���� ������������� ���� ���������� �������
	bool * existing;
	void OnCompleteCaptureThread(wxThreadEvent & event);
public:
	/// �����������
	VCameraControl(VCameraBase * camera_props, wxWindow* parent, int WindowId, bool * _existing);
	/// ����������
	~VCameraControl();
	/// ���������� ������ ����
	wxSize GetBestSize() const;
	
	DECLARE_EVENT_TABLE();
};