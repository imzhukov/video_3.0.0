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

///Класс для отображения основных свойств камеры
#define ID_NAME_PROP 108
#define ID_DIRECTORY_PROP ID_NAME_PROP + 1
#define ID_ADDRESS_PROP ID_DIRECTORY_PROP + 1
#define ID_NUM_PROP ID_ADDRESS_PROP + 1

//Базовые настройки
class BaseProp : public wxPanel
{
	//Указатель на строку с директорией, которая правится пользователем
	std::wstring * directory_str;
	//Указатель на строку с именем камеры, которая правится пользователем
	std::wstring * name_str;
public:
	///Поле для ввода имени камеры
	wxTextCtrl *name;
	///Поле для ввода пути к папке для записей
	//wxTextCtrl *directory;
	///Кнопка для выбора папки
	//wxButton * chooseDir;
	//Обработчик выбора папки
	//void OnChooseDirectory(wxCommandEvent &event);
	void UpdateNameProp(wxCommandEvent &event);
	//void UpdateDirectoryProp(wxCommandEvent &event);
	BaseProp(wxWindow* parent);
	//Установить базовое свойство
	void setProp(VCameraBase * prop);
	DECLARE_EVENT_TABLE();
};

///Класс свойств IP-камеры
class RTSPProp : public wxPanel
{
	//Указатель на строку с директорией, которая правится пользователем
	//std::wstring * directory_str;
	//Указатель на строку с именем камеры, которая правится пользователем
	std::wstring * name_str;
	//Адрес
	std::wstring * address_str;
public:
	wxStaticBoxSizer * vbox;
	///Поле для ввода имени камеры
	wxTextCtrl * name;
	///Поле для ввода пути к папке для записей
	//wxTextCtrl *directory;
	///Кнопка для выбора папки
	//wxButton * chooseDir;
	///Поле для ввода адреса камеры
	wxTextCtrl * address;
	///Конструктор
	RTSPProp(wxWindow *parent);
	///Обновление окна
	void UpdateProp(wxCommandEvent &event);
	void UpdateNameProp(wxCommandEvent &event);
	//void UpdateDirectoryProp(wxCommandEvent &event);
	//Установить указатель на строку с адресом камеры
	void setProp(VRTSPCameraProperties * prop);
	//Обработчик выбора папки
	//void OnChooseDirectory(wxCommandEvent &event);
	DECLARE_EVENT_TABLE();
};

///Класс свойств USB-камеры
class JAIProp : public wxPanel
{
	//Указатель на строку с директорией, которая правится пользователем
	//std::wstring * directory_str;
	//Указатель на строку с именем камеры, которая правится пользователем
	std::wstring * name_str;
	/// Номер камеры 
	int * numCameraPtr;
public:
	wxStaticBoxSizer * vbox;
	///Поле для ввода имени камеры
	wxTextCtrl *name;
	///Поле для ввода пути к папке для записей
	//wxTextCtrl *directory;
	///Кнопка для выбора папки
	//wxButton * chooseDir;
	/// Поле для ввода номера камеры
	wxTextCtrl * numCamera;
	///Конструктор
	JAIProp(wxWindow *parent);
	///Обновление окна
	void UpdateProp(wxCommandEvent &event);
	void UpdateNameProp(wxCommandEvent &event);
	//void UpdateDirectoryProp(wxCommandEvent &event);
	//Установить указатель на строку с адресом камеры
	void setProp(VJAICameraProperties * prop);
	//Обработчик выбора папки
	//void OnChooseDirectory(wxCommandEvent &event);
	DECLARE_EVENT_TABLE();
};

///Окно настроек камер 
class VCameraPropDialog : public wxDialog
{
public:
	/// Конструктор
	VCameraPropDialog(wxWindow * parent, std::vector<VCameraBase*> & _prop);
	/// Возвращает вектор со всеми остальными настройками
	std::vector<VCameraBase*> & GetCameraProperties();
private:

	///Действующий список с настройками камер
	std::vector<VCameraBase*> prop;
	///Рабочий список с настройками камер
	std::vector<VCameraBase*> temp_prop;
	int current_prop;
	int selected_type_setup;

	wxToolBar * toolBar;
	///Элемент для отображения списка камер
	wxListCtrl *camList;
	wxSplitterWindow * m_SplitterWindow;

	///Кнопки выбора режима записи видео
	wxRadioButton *RtspRadioButton;
	wxRadioButton *JaiRadioButton;
	///Страницы настроек для разных типов камер
	wxAuiNotebook * NoteBook;
	RTSPProp * rtspPropPanel;
	JAIProp  * jaiPropPanel;

	///Кнопки подтверждения/отмены проведенных операций
	wxButton* okButton;
	wxButton* cancelButton;
	wxButton* applyButton;

	void OnPageChanging(wxAuiNotebookEvent & evt);
	void setTypeCamera(wxCommandEvent& event);

	///Обработка событий кнопок управления списком камер
	void OnAddTool(wxCommandEvent &event);
	void OnRemoveTool(wxCommandEvent &event);

	///Обработка событий кнопок подтверждения и отмены операций
	void OnOkButton(wxCommandEvent &event);
	void OnCancelButton(wxCommandEvent &event);
	void OnApplyButton(wxCommandEvent &event);

	///Обработка клика по списку камер
	void OnListClick(wxListEvent &event);

	///Обновление элементов интерфейса
	void OnUpdateUI(wxUpdateUIEvent& evt);
	void OnClose(wxCloseEvent& event);
	DECLARE_EVENT_TABLE();
};