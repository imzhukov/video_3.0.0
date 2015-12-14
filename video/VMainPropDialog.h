#pragma once

#include <boost/asio.hpp>

#include <wx/aui/aui.h>
#include <wx/wx.h>
#include <wx/spinctrl.h>

#include <string> 
#include "strlib.h"

#include "VLog.h"
#include "VVideoOptions.h"

/// Диалог настройки программы

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
	std::wstring prev_video_server;				//директория видеосервера
	int prev_intervalRecording;					//Интервал съёмки
	bool prev_showSubtitle;						//Показывать субтитры
	std::wstring prev_tmpltSubtitle;			//Шаблон субтитров
	int prev_locationSubtitle;					//Положение субтитров
	int prev_heightSubtitle;					//Высота субтитров
	int prev_shiftSubtitle;						//Сдвиг субтитров от границы
	std::wstring prev_com;						//Имя com-порта
	std::wstring prev_in_server_db;				//Сервер БД
	std::wstring prev_in_database;				//Файл БД
	std::wstring prev_ip_udp;					//IP реального времени
	int prev_port_udp;							//Порт udp
	float prev_shiftCoordinateViewer;			//Сдвижка координаты для viewer
	float prev_shiftCoordinateSpeedPositive;	//Сдвижка координаты для положительной скорости
	float prev_shiftCoordinateSpeedNegative;	//Сдвижка координаты для отрицательной скорости

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
	//Обновление интерфейса
	void OnUpdateUI(wxUpdateUIEvent & event);
	//Обновление директории видеосервера
	void UpdateVideoServer(wxCommandEvent & event);
	//Обновление интервала съёмки
	void UpdateInetrvalRecording(wxSpinEvent & event);
	//Обновление шаблона субтитров
	void UpdateTmpltSubtitle(wxCommandEvent & event);
	//Обновление положения субтитров
	void UpdateLocationSubtitle(wxCommandEvent & event);
	//Высота субтитров
	void UpdateHeightSubtitle(wxCommandEvent & event);
	//Смещение субтитров
	void UpdateShiftSubtitle(wxCommandEvent & event);
	//Имя com-порта
	void UpdateCom(wxCommandEvent & event);
	//Сервер БД
	void UpdateHost(wxCommandEvent & event);
	//Файл БД
	void UpdateFileDb(wxCommandEvent & event);
	//IP-приёмника 
	void UpdateIpUdp(wxCommandEvent & event);
	//Порт приёмника
	void UpdatePortUdp(wxCommandEvent & event);
	//Смещение координаты для просмотрщика кадров
	void UpdateShiftCoordinateViewer(wxCommandEvent & event);
	//Смещение координаты для положительной скорости
	void UpdateShiftCoordinateSpeedPositive(wxCommandEvent & event);
	//Смещение координаты для отрицательной скорости
	void UpdateShiftCoordinateSpeedNegative(wxCommandEvent & event);
	DECLARE_EVENT_TABLE();
};