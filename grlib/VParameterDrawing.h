#pragma once

//std
#include <stack>

//boost
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

//V
#include "calclib.h"
#include "dbfunc.h"
#include "axes.h"
#include "elements.h"
#include "providers.h"
#include "actions.h"
#include "VXMLEditor.h"
#include "VProgressInterface.h"

using namespace xercesc;

/** Запас по координате*/
#define ABSCOORD_SPACE 100

/** Парсер команды*/
class VCommandParser
{
private:

	/** Триммер строки*/
	struct trimmer
	{	
		void operator() (std::wstring & in_out_string) { boost::trim(in_out_string); };
	};

	/** Команда*/
	std::wstring command;
	/** Параметры*/
	std::vector<std::wstring> parameters;
	/** Параметры*/
	std::wstring parameter;
public:
	/** Конструктор*/
	VCommandParser(std::wstring& in_command);
	/** Возвращает команду*/
	const std::wstring& GetCommand() const
	{
		return command;
	};
	/** Возвращает число параметров*/
	size_t GetParameterCount() const
	{
		return parameters.size();
	};
	/** Возвращает параметр*/
	const std::wstring& GetParameter(size_t in_index)
	{
		return parameters.at(in_index);
	};
	/** Возвращает параметры одной строкой*/
	const std::wstring& GetParameters()
	{
		return parameter;
	};
};

/** Базовый класс для графика параметров*/
class VParameterDrawing : public VDrawing
{
protected:	

	/** Участок пути*/
	VRouteSection route_section;

	/** Список параметров SQL*/
	typedef std::map<std::wstring, std::wstring> subst_t;
	typedef std::map<std::wstring, std::wstring>::iterator subst_itr;
	typedef std::pair<std::wstring, std::wstring> subst_pair;
	subst_t subst;


	/** Список областей*/
	std::map<std::wstring, VAreaPtr> areas;
	/** Объявление итератора для списка областей*/
	typedef std::map<std::wstring, VAreaPtr>::iterator area_itr;
	/** Объявление пары для списка областей*/
	typedef std::pair<std::wstring, VAreaPtr> area_pair;

	/** Список цветов линий по умолчанию*/
	VColorQueue default_colors;

	/** Список сайзеров*/
	std::vector<VAreaSizerPtr> area_sizers;
	/** Объявление итератора для списка сайзеров*/
	typedef std::vector<VAreaSizerPtr>::iterator area_sizer_itr;

	/** Список осей координат*/
	std::vector<VAxesPtr> axes_areas;	
	typedef std::vector<VAxesPtr>::iterator axes_itr;
	
	/** Область заголовка*/
	VAreaPtr header_area;	
	/** Область под графиком*/
	VAreaPtr footer_area;	
		
	/** Список действий */	
	typedef std::map<VAxesPtr, std::vector<VActionPtr>> actions_t;
	typedef std::map<VAxesPtr, std::vector<VActionPtr>>::iterator actions_itr;
	typedef std::pair<VAxesPtr, std::vector<VActionPtr>> actions_pair;
	actions_t actions;
	
	/** Данные о текущей странице*/
	VPageInfoPtr page_info;

	/** Данные о глобальной области выделения*/
	VSelectionInfoPtr selection_info;
						
	/** Список выполненных SQL*/
	std::set<std::wstring> sql_set;
	/** Список констант*/
	std::set<std::wstring> constant_set;


	/** Путь к файлу шаблона документа*/
	wstring template_path;		
	/** Шаблон графика*/
	VXMLEditor drawing_template;	
	
	/** Файлы данных*/
	std::vector<VDataSetPtr> data_set;
	/** Указатель на активный файл данных*/
	VDataSetPtr active_data_set;
	/** Указатель на эталонный файл данных*/
	VDataSetPtr reference_data_set;
	/** Указатель на БД*/
	VIBDatabasePtr database;
	/** Транзакция*/
	VIBTransactionPtr transaction;
	/** Априорная информация*/
	VAprinfoPtr aprinfo;			
	/** Указатель на диалог сохранения файла*/
	VAUISaveFileDialogPtr save_file_dialog;	
	/** Указатель на диалог выбора из списка*/
	VAUISelectItemDialogPtr select_dialog;	
	/** Указатель на диалог прогресса*/
	VAUIProgressDialogPtr progress_dialog;
	/** Указатель на диалог редактирования свойств*/
	VAUIPropertyDialogPtr property_dialog;
	/** Указатель на диалог ввода текста*/
	VAUIInputDialogPtr input_dialog;
	/** Координатная сетка*/
	VCoordinatesDataPtr coordinates;
	
	//Методы
		
	/** Сохранение SVG*/
	virtual void SaveDocumentSVG(const char * const file_name);
	/** Сохранение PDF*/
	virtual void SaveDocumentPDF(const char * const file_name);	
	/** Очистить список областей*/
	void ClearAreas();	
	/** Формирует список допустимых действий для точки*/
	virtual void CreateContextMenu(double x, double y);	
	/** Возвращает корневой узел*/
	VXMLGraphics& GetGraphics();
	/** Возвращает корневой узел*/
	const VXMLGraphics& GetGraphics() const;	
	/** Создает заголовок*/
	void CreateHeader();
	/** Создает слои*/
	void CreateLayers();
	/** Создает область*/	
	VAreaPtr CreateArea(VXMLArea& in_xml_area);
	/** Создает оси*/
	VAxesPtr CreateAxes(VXMLAxes& in_xml_axes, VAreaPtr in_area);
	/** Добавляет действие в меню*/
	void AddMenuAction(VActionPtr in_action);
	/** Связывает действие с системой координат*/
	void RegisterAction(VActionPtr in_action, VAxesPtr in_axes);
	/** Посмещает действие в тулбар*/	
	void AddToolbarAction(VActionPtr in_action, xpm_t* in_enabled_xpm, xpm_t* in_disabled_xpm);		
	/** Устанавливат способ масштабирования для осей*/
	void SetScaleMethod(VXMLAxes in_xml_axes, VAxesPtr in_axes);
	/** Возвращает true если имеем дело со станционными путями*/
	bool IsStationWay() const;

	/** Возвращает легенду графика*/
	std::wstring FormatGraphLegend(VXMLStaticGraph& xml_graph, size_t in_index, const std::wstring& in_comment=L"") const;
	/** Создает источник данных определенного типа и возвращает указатель на него*/
	std::vector<VDataProviderPtr> CreateDataProvider(VAxesPtr in_axes, VXMLStaticGraph& in_xml_graph);
	/** Создает график*/
	VElementPtr CreateGraph(const std::wstring& in_element_name, VDataProviderPtr in_data_provider, 
		VAxesPtr in_axes, VXMLStaticGraph& in_xml_static_graph);	

	/** Возвращает актуальную дату графика*/
	VTime GetActualDate();
	/** Возвращает код направления*/
	std::wstring GetActualDir();
	/** Возвращает код пути*/
	std::wstring GetActualWay();
	/** Возвращает участок пути*/
	VWaySection GetActualSection();
	/** Возвращает активный файл*/
	VDataSetPtr GetActiveDataSet();
	/** Возвращает эталонный файл*/
	VDataSetPtr GetReferenceDataSet();
	/** Возвращает минимальную координату из файлов*/
	double GetMinAbs();
	/** Возвращает максимальную координату из файлов*/
	double GetMaxAbs();
	/** Загрузка данных*/
	void LoadData(const std::vector<std::wstring>& in_data_file_names, VIBDatabasePtr in_database,
		const VRouteSection& in_route_section, bool in_from_db);	

public:
	/** Конструктор*/
	VParameterDrawing();
	/** Деструктор*/
	virtual ~VParameterDrawing();
	/** Загрузка шаблона*/
	void LoadTemplate(const wstring& in_template_path);	
	/** Сохранение шаблона*/
	void SaveTemplate(const wstring& in_template_path);	
	/** Возвращает название графика*/
	const std::wstring& GetName() const;
	/** Возвращает название группы*/
	std::wstring GetGroup();
	/** Возвращает фильтр*/
	std::wstring GetFilter();	
	/** Подготовка данных*/
	void Prepare(const std::vector<std::wstring>& in_data_file_names, VIBDatabasePtr in_database, 
		const VRouteSection& in_route_section, bool in_from_db);
	/** Закрытие окна*/
	virtual void OnClose();
	/** Возвращает true, если изменились исходные данные*/
	virtual bool IsDataChanged();
	/** Устанавливает указатель на диалог сохранения файла*/
	void SetSaveFileDialog(VAUISaveFileDialogPtr in_save_file_dialog);
	/** Устанавливает указатель на диалог выбора из списка*/
	void SetSelectDialog(VAUISelectItemDialogPtr in_select_dialog);	
	/** Устанавливает указатель на диалог прогресса*/
	void SetProgressDialog(VAUIProgressDialogPtr in_progress_dialog);	
	/** Устанавливает указатель на диалог редактирования свойств*/
	void SetPropertyDialog(VAUIPropertyDialogPtr in_property_dialog);
	/** Устанавливает указатель на диалог ввода текста*/
	void SetInputDialog(VAUIInputDialogPtr in_input_dialog);	
	/** Подставляет параметр в SQL запросы*/	
	void SubstParameter(const wstring& in_name, const wstring& in_value);
	/** Сохранение в файл только видимой части*/
	virtual void SaveVisible(const char * const file_name, FILE_FORMAT format);	
	/** Сохранение документа целиком - реализуется в потомках*/
	virtual void SaveDocument(const char * const file_name, FILE_FORMAT format);
	/** Возвращает число страниц документа*/
	virtual size_t GetPageCount();
	/** Переходит на страницу с указанным номером*/
	virtual void GoToPage(int in_page);
	/** Прокрутить к нужной позиции*/
	virtual void ScrollToHorz(double in_value);	

	/** Возвращает позицию по горизонтали*/
	virtual double GetHorzPos();
	/** Возвращает минимально возможную позицию по горизонтали*/
	virtual double GetHorzMin();
	/** Возвращает максимально возможную позицию по горизонтали*/
	virtual double GetHorzMax();
	
	/** Устанавливает размеры областей*/
	void AdjustAreas();
			
	/** Устанавливает текущий масштаб по X*/
	void SetScaleX(double in_scale);
	/** Возвращает текущий масштаб*/
	double GetScaleX();	
	/** Устанавливает начало координат по оси X*/
	void SetStartX(double in_start_x);
	/** Возвращает начало координат по оси X*/
	double GetStartX();		
	/** Возвращает размер страницы в абс. координатах*/
	double GetPageSize();
	
	/** Выполняется при первой отрисовке*/
	void OnShow();	
};
typedef boost::shared_ptr<VParameterDrawing> VParameterDrawingPtr;

