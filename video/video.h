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

/** Просмотр журнала*/
#define ID_SET_LOG_LEVEL 600
class VLogViewer : public wxListCtrl
{
private:
	
	/** Аттрибуты строки сообщения об ошибке*/
	wxListItemAttr error_attr;
	/** Аттрибуты строки сообщения об ошибке*/
	wxListItemAttr warning_attr;
	/** Аттрибуты строки информации*/
	wxListItemAttr info_attr;
	/** Аттрибуты строки отладки*/
	wxListItemAttr debug_attr;
	/** Всплывающее меню*/
	wxMenu popup_menu;	

	/** Обработчик правого клика*/
	void OnRightClick(wxListEvent& in_event);
	/** Установить уровень журналирования*/
	void OnSetLogLevel(wxCommandEvent & in_event);
	/** Обновление UI*/
	void OnUpdateUI(wxUpdateUIEvent& evt);
	
public:

	/** Конструктор*/
	VLogViewer(wxWindow * in_parent, wxWindowID in_id);
	/** Возвращает содержимое ячейки*/
	virtual wxString OnGetItemText(long item, long column) const;
	/** Возвращает аттрибуты ячейки*/
	virtual wxListItemAttr * OnGetItemAttr(long item) const;
	/** Обновляет вид*/
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
	///Кнопки тулбара
	ID_FOLDER_BUTTON,
	///Кнопки Ок, Отмена и Применить
	ID_OK_BUTTON,
	ID_CANCEL_BUTTON,
	ID_APPLY_BUTTON,   
	///Radiobutton для выбора типа камеры
	ID_IPCHECK,
	ID_OLDCHECK,
	///Список камеры в окне потокового воспроизведения видео
	ID_CHECKLISTBOX,
	ID_COMBO,

	///Идентификаторы элементов главного окна
	ID_LOG	
};

/// Главное окно
class VMainFrame : public wxFrame
{
public:
	/// Конструктор
	VMainFrame(wxWindow * parent, wxWindowID id = wxID_ANY, const wxString & title = wxEmptyString,
		const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxCLOSE_BOX|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|
		wxSYSTEM_MENU|wxCAPTION|wxRAISED_BORDER|wxRESIZE_BORDER);
	/// Деструктор
	~VMainFrame();
	
	/// Настраивает меню
	void SetupMenu();
	/// Добавляет камеру
	void AddCamera(VCameraBase * in_camera_props, int cameraId);
	///Обновление списка камер в главном окне
	void UpdateList(wxCommandEvent &ce);
private:
	VRegisteredDatabase * RegisteredDatabase;

	///Переменная для временной обработки окон камер
	wxWindow *buffWindow;
	/// Список с настройками
	std::vector<VCameraBase *> cameraProperties;
	/// Флаги существования экранов отображения видео
	std::vector<bool> cameraControlsExisting;
	/// ID окон управления камерами
	std::vector<int> cameraWndId;
	/// Счётчик окон
	long cameraWndCounter;
	/// AuiManager
	wxAuiPaneInfo CameraPaneInfo;
	/// Журнал
	VLogViewer * LogViewer;
	/// Меню
	wxMenuBar* MenuBar;
	/// Диалог настройки программы
	void OnProperties(wxCommandEvent& event);
	/// Диалог настройки камер
	void OnPropertiesCams(wxCommandEvent& event);
	/// Просмотр файлов формата c4v
	void OnViewC4v(wxCommandEvent& event);
	/// Пункт меню "Выход"
	void OnQuit(wxCommandEvent& event);
	/// Справка о программе
	void OnAbout(wxCommandEvent& event);
	/// Закрытие окна 
	void OnClose(wxCloseEvent& event);
	/// Обновление UI
	void OnUpdateUI(wxUpdateUIEvent& event);
	///Включает камеру по выбору из списка
	void OnTurnCamera(wxCommandEvent& event);
	///Функция завершения потока координат от программы RT
	//void OnCoordinatesThreadCompletion(wxThreadEvent&);
protected:
	/// Источник координат
	VCoordinatesThread * coordinates;
	/// Метод окончания работы с потоком координат
	void OnCompleteCoordThread(wxThreadEvent & event);
	/// Источник кодов ДПП
	VCodeDPPThread * dpp;
	/// Метод окончания работы с потоком кодов ДПП
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

/// Класс "приложение wxWidgets"
class VApp : public wxApp
{
private:	
public:
	/// Метод, реализующий запуск приложения
	virtual bool OnInit();	
};

DECLARE_APP(VApp);
IMPLEMENT_APP(VApp);

DECLARE_VARIABLE(std::wstring, config_folder, VFileName(VFileName::GetHomeDir(), L"\\AppData\\Roaming\\video\\config\\").GetPath())
#define CONFIG_FOLDER config_folder().Value()
DECLARE_VARIABLE(std::wstring, log_folder, VFileName(VFileName::GetHomeDir(), L"\\AppData\\Roaming\\video\\log\\").GetPath())
#define LOG_FOLDER log_folder().Value()