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
///Состояние выбранной камеры
extern bool isConnected;

/// Окно управления камерой
class VCameraControl : public wxFrame
{
	///Папка для записи
	//std::wstring directory;
	/// Сайзер
	wxSizer * sizer;
	/// Панель инструментов
	wxToolBar * toolbar;
	///Режим записи видео
	int type_rec;
	/// Экран
	wxPanel * screen;
	/// Поток захвата
	VCaptureThread * capture;
	///Сайзер
	wxSizer *hbox;
	///Текущее время
	time_t now;
	///Нажата клавиша записи видео
	bool isRecording;
	///Интервал съёмки
	int intervalRecording;
	/// Начать запись в файл
	void OnStartRecording(wxCommandEvent & in_event);
	/// Остановить запись в файл
	void OnStopRecording(wxCommandEvent & in_event);
	/// Обработчик события UPDATE_UI
	void OnUpdateUI(wxUpdateUIEvent & in_event);
	/// Обработка перерисовки окна
	void OnPaint(wxPaintEvent& event);
	/// Флаг существования окна управления камерой
	bool * existing;
	void OnCompleteCaptureThread(wxThreadEvent & event);
public:
	/// Конструктор
	VCameraControl(VCameraBase * camera_props, wxWindow* parent, int WindowId, bool * _existing);
	/// Деструктор
	~VCameraControl();
	/// Возвращает размер окна
	wxSize GetBestSize() const;
	
	DECLARE_EVENT_TABLE();
};