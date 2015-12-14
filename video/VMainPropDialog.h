#pragma once

#include <boost/asio.hpp>

#include <wx/aui/aui.h>
#include <wx/wx.h>
#include <wx/spinctrl.h>

#include <string> 
#include "strlib.h"

#include "VLog.h"
#include "VVideoOptions.h"

/// ������ ��������� ���������

#define ID_VIDEO_SERVER 90
#define ID_INTERVAL_RECORDING 91
#define ID_TMPLT_SUBTITLE 92
#define ID_LOCATION_BOTTOM 93
#define ID_LOCATION_TOP 94
#define ID_HEIGHT_SUBTITLE 95
#define ID_SHIFT_SUBTITLE 96
#define ID_COM 97
#define ID_HOST 98
#define ID_DBFILENAME 99
#define ID_IPUDP 100
#define ID_PORTUDP 101
#define ID_SHOWSUBTITLE 102
#define ID_SHIFT_COORDINATE_VIEWER	103
#define ID_SHIFT_COORDINATE_SPEED_POSITIVE	104
#define ID_SHIFT_COORDINATE_SPEED_NEGATIVE	105

class VMainPropDialog : public wxDialog
{
public:
	VMainPropDialog(wxWindow * parent);
private:
	std::wstring prev_video_server;				//���������� ������������
	int prev_intervalRecording;					//�������� ������
	bool prev_showSubtitle;						//���������� ��������
	std::wstring prev_tmpltSubtitle;			//������ ���������
	int prev_locationSubtitle;					//��������� ���������
	int prev_heightSubtitle;					//������ ���������
	int prev_shiftSubtitle;						//����� ��������� �� �������
	std::wstring prev_com;						//��� com-�����
	std::wstring prev_in_server_db;				//������ ��
	std::wstring prev_in_database;				//���� ��
	std::wstring prev_ip_udp;					//IP ��������� �������
	int prev_port_udp;							//���� udp
	float prev_shiftCoordinateViewer;			//������� ���������� ��� viewer
	float prev_shiftCoordinateSpeedPositive;	//������� ���������� ��� ������������� ��������
	float prev_shiftCoordinateSpeedNegative;	//������� ���������� ��� ������������� ��������

	wxSpinCtrl *spinIntRecord;
	wxCheckBox *showSubtitleBox;
	wxTextCtrl *textCtrlVideoServer, *textCtrlTmplt, *textCtrlHeight, *textCtrlShiftSubtitle, *textCtrlCom, 
			*textCtrlDbHost, *textCtrlDbFile, *textCtrlIpUdp, *textCtrlPortUdp, 
			*textCtrlShiftCoordinateViewer, *textCtrlShiftCoordinateSpeedPositive, *textCtrlShiftCoordinateSpeedNegative;
	wxRadioButton *locationRdBtnBottom, *locationRdBtnTop;

	wxButton * okButton;
	wxButton * cancelButton;
	void OnOkButton(wxCommandEvent &event);
	void OnCancelButton(wxCommandEvent &event);
	//���������� ����������
	void OnUpdateUI(wxUpdateUIEvent & event);
	//���������� ���������� ������������
	void UpdateVideoServer(wxCommandEvent & event);
	//���������� ��������� ������
	void UpdateInetrvalRecording(wxSpinEvent & event);
	//���������� ������� ���������
	void UpdateTmpltSubtitle(wxCommandEvent & event);
	//���������� ��������� ���������
	void UpdateLocationSubtitle(wxCommandEvent & event);
	//������ ���������
	void UpdateHeightSubtitle(wxCommandEvent & event);
	//�������� ���������
	void UpdateShiftSubtitle(wxCommandEvent & event);
	//��� com-�����
	void UpdateCom(wxCommandEvent & event);
	//������ ��
	void UpdateHost(wxCommandEvent & event);
	//���� ��
	void UpdateFileDb(wxCommandEvent & event);
	//IP-�������� 
	void UpdateIpUdp(wxCommandEvent & event);
	//���� ��������
	void UpdatePortUdp(wxCommandEvent & event);
	//�������� ���������� ��� ������������ ������
	void UpdateShiftCoordinateViewer(wxCommandEvent & event);
	//�������� ���������� ��� ������������� ��������
	void UpdateShiftCoordinateSpeedPositive(wxCommandEvent & event);
	//�������� ���������� ��� ������������� ��������
	void UpdateShiftCoordinateSpeedNegative(wxCommandEvent & event);
	DECLARE_EVENT_TABLE();
};