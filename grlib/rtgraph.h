#pragma once

#include "VDrawing.h"
#include "elements.h"
#include "oversteps.h"
#include "shiny.h"

#define ZERO_COLOR COLOR_HTML_BLUE
#define CONSTANT_COLOR COLOR_HTML_BLUE
#define ORIGINAL_NORMA_COLOR VColor(0,128,0)
#define CORRECTED_NORMA_COLOR VColor(0,0,128)
#define OVERSTEP_COLOR VColor(255,0,0,255)
#define OVERSTEP_FILL_COLOR VColor(255,0,0,80)
#define PARAMETER_COLOR COLOR_HTML_BLACK
#define CORRUPT_COLOR VColor(255,0,0,80)
#define Y_LINE_COLOR COLOR_HTML_BLACK
#define APR_SWITCH_COLOR VColor(0,0,255,128)
#define OPT_SWITCH_COLOR COLOR_HTML_BLACK
#define ZERO_DASH VDash(1.0, 1.0)
#define CONSTANT_DASH VDash(0.25, 1.0)
#define Y_LINE_DASH VDash(0.25, 1.0)

#define FRAME_WIDTH min_line_width
#define CONSTANT_LINE_WIDTH min_line_width
#define PARAMETER_LINE_WIDTH min_line_width
#define NORMA_LINE_WIDTH min_line_width


/// Режим записи PDF
enum PDF_SPLIT_POLICY {NO_SPLIT=0, SPLIT_KM=1};

/// Путеизмерительная лента
class VRTGraph : public VDrawing
{
protected:	

	//Данные

	/// Контекст устройства принтера
	HDC printer_hdc;	
	/// Опция включения записи в PDF
	bool pdf_enabled;	
	/// Опция включения печати
	bool printing_enabled;
	/// Разрешение 
	double dpi;
	/// Имя файла PDF
	std::wstring pdf_file_name;
	/// Номер страницы
	size_t page_number;	
	/// Километры	
	VRTKmProcessor km;
	/// Вагон
	std::wstring vps;
	/// ФИО
	std::wstring person;
	/// Время начала проезда
	VTime ride_time;
	/// Время получения отчетной формы
	VTime report_time;
	/// Источник данных
	std::wstring data_source;	

	
	/// Список сайзеров
	std::vector<VAreaSizerPtr> area_sizers;	
	typedef std::vector<VAreaSizerPtr>::iterator area_sizer_itr;
	/// Список осей координат
	std::vector<VAxesPtr> axes_areas;	
	typedef std::vector<VAxesPtr>::iterator axes_itr;

	/// Область заголовка
	VAreaPtr header_area;
	/// Область информации
	VAreaPtr summary_area;
	/// Область информации доп.
	VAreaPtr summary_area_ext;
	/// Область номера страницы
	VAreaPtr page_area;
	/// Область априорных данных
	VAreaPtr aprinfo_area;
	/// Область огр. скорости
	VAreaPtr restriction_area;
	/// Область количества элементарных отступлений
	VAreaPtr count_area;
	/// Область длин отступлений
	VAreaPtr length_area;
	/// Область величин отступлений
	VAreaPtr value_area;
	/// Область степеней отступлений
	VAreaPtr degree_area;
	/// Область типа отступлений
	VAreaPtr type_area;
	/// Область координат отступлений
	VAreaPtr coord_area;
	/// Область событий
	VAreaPtr event_area;	
	/// Область всех графиков
	VAreaPtr graph_area;
	

	/// Ось априорной информации
	VAxesPtr aprinfo_axes;
	/// Область огр. скорости
	VAxesPtr restriction_axes;
	/// Область количества элементарных отступлений
	VAxesPtr count_axes;
	/// Область длин отступлений
	VAxesPtr length_axes;
	/// Область величин отступлений
	VAxesPtr value_axes;
	/// Область степеней отступлений
	VAxesPtr degree_axes;
	/// Область типа отступлений
	VAxesPtr type_axes;
	/// Область координат отступлений
	VAxesPtr coord_axes;
	/// Область событий
	VAxesPtr event_axes;
	/// Область графиков
	VAxesPtr graph_axes;
	
	/// Координаты
	VCoordinatesDataPtr coordinates;
	/// Заголовок
	VUserTextProviderPtr header;
	/// Сводка
	VUserTextProviderPtr summary;
	/// Перегон
	VUserTextProviderPtr peregon;
	/// События
	VRTEventInfoDataPtr events;
	/// Границы участков
	VDequeDataPtr borders;
	/// Отладочные события
	VRTEventInfoDataPtr debug_events;
	/// Состояние паспорта
	VDequeDataPtr passport_status;

	/// Данные о текущей странице
	VPageInfoPtr page_info;	

	/// Документ PDF
	VPDFDocumentPtr pdf_document;


	/// Папка для записи
	std::wstring folder;
	/// Префикс файла PDF
	std::wstring prefix;
	/// Режим записи PDF
	PDF_SPLIT_POLICY pdf_split_policy;
	/// Признак открытого PDF
	bool pdf_ready;
	/// Очередь страниц для печати
	std::deque<VRawData> print_queue;

	// Функции
	
	/// Подготовка очередной страницы
	virtual void PrepareData(const VRTKmDataPtr& in_km)=0;	
	/// Очистка данных
	virtual void ClearData()=0;
	/// Очистка информации о километре
	virtual void ClearSummary()=0;
	/// Рендеринг страницы в памяти
	void RenderPage(const VRTKmDataPtr& in_km);
	/// Формирует имя файла PDF
	virtual std::wstring FormatPDFFileName(int in_km);
	/// Запись страницы в PDF
	void SaveCurrentKmPDF(cairo_surface_t * in_source_surface);
	/// Открывает PDF
	void OpenPDF(const std::wstring& in_filename);
	/// Закрывает PDF
	void ClosePDF(); 
	/// Печать страницы
	void PrintCurrentKm(cairo_surface_t * in_source_surface);
	///Возвращает true если километр нужно печатать
	virtual bool MustBePrinted(const VRTKmDataPtr& in_km)=0;

public:

	/// Конструктор
	VRTGraph(const std::wstring& in_folder, const std::wstring& in_prefix);
	/// Деструктор
	~VRTGraph();
	/// Устанавливает разрешение
	void SetDPI(double in_dpi);
	/// Подготовка
	virtual void Prepare()=0;
	/// Сохраняет страницу
	void ProcessKm(const VRTKmDataPtr& in_km);
	/// Масштабирует области
	void AdjustAreas();
	/// Устанавливает текущий масштаб
	void SetScaleX(double in_scale);
	/// Устанавливает начало координат по оси X
	void SetStartX(double in_start_x);

	/// Устанавливает номер вагона
	void SetVPS(const std::wstring& in_vps);
	/// Устанавливает ФИО
	void SetPerson(const std::wstring& in_person);
	/// Устанавливает время проезда
	void SetTime(const VTime& in_time);
	/// Устанавливает источник данных
	void SetDataSource(const std::wstring& in_data_source);

	/// Устанавливает опцию записи в PDF
	void SetPDFEnabled(bool in_pdf_enabled);
	/// Возвращает true если включена запись в PDF
	bool GetPDFEnabled() const;

	/// Устанавливает опцию печати на принере
	void SetPrintingEnabled(bool in_printing_enabled);
	/// Возвращает true если включена печать на принтер
	bool GetPrintingEnabled() const;
	
	/// Устанавливает опцию режима записи PDF
	void SetPDFSplitPolicy(PDF_SPLIT_POLICY in_pdf_split_policy);
	/// Возвращает опцию режима записи PDF
	PDF_SPLIT_POLICY GetPDFSplitPolicy();
	
	/// Возвращает количество страниц для печати
	size_t GetPageCount();
	/// Возвращает страницу
	VRawData GetPage();
	/// Удаляет страницу
	void PopPage();
};
typedef boost::shared_ptr<VRTGraph> VRTGraphPtr;

/// Лента ГРК
class VRTGeometryGraph : public VRTGraph
{
private:
	
	/// Область просадки левой
	VAreaPtr pit_left_area;
	/// Область просадки правой
	VAreaPtr pit_right_area;
	/// Область ШК
	VAreaPtr width_area;
	/// Область рихтовки левой
	VAreaPtr rich_left_area;
	/// Область рихтовки правой
	VAreaPtr rich_right_area;
	/// Область уровня
	VAreaPtr level_area;
	/// Область значков Анп Пси Ошк
	VAreaPtr anp_area;
		
	
	/// Область просадки левой
	VAxesPtr pit_left_axes;
	/// Область просадки правой
	VAxesPtr pit_right_axes;
	/// Область ШК
	VAxesPtr width_axes;
	/// Износ
	VAxesPtr wear_axes;
	/// Область рихтовки левой
	VAxesPtr rich_left_axes;
	/// Область рихтовки правой
	VAxesPtr rich_right_axes;
	/// Область уровня
	VAxesPtr level_axes;
	/// Область значков Анп Пси и т.д.
	VAxesPtr anp_axes;				

	/// Просадка левая
	VVectorDataPtr pit_left; 
	/// Просадка правая
	VVectorDataPtr pit_right; 
	/// ШК
	VVectorDataPtr width;
	/// Износ фактический
	VVectorDataPtr wear; 
	/// Износ паспортный
	VVectorDataPtr wear_passport; 
	/// Рихтовка левая
	VVectorDataPtr rich_left; 
	/// Рихтовка правая
	VVectorDataPtr rich_right; 
	/// Уровень
	VVectorDataPtr level;
	/// Метки RFID
	VDequeDataPtr rfid;

	/// Паспортная линия по уровню
	VDequeDataPtr level_passport_line_original;
	/// Паспортная линия по рихтовке
	VDequeDataPtr rich_passport_line_original;
	/// Паспортная линия по ШК
	VDequeDataPtr width_passport_line_original;
	
	/// Паспортная линия по уровню (скорректированная)
	VDequeDataPtr level_passport_line_corrected;
	/// Паспортная линия по рихтовке (скорректированная)
	VDequeDataPtr rich_left_passport_line_corrected;
	/// Паспортная линия по рихтовке (скорректированная)
	VDequeDataPtr rich_right_passport_line_corrected;
	/// Паспортная линия по ШК (скорректированная)
	VDequeDataPtr width_passport_line_corrected;
	
	/// Стрелки обнаруженные	
	VSwitchDataPtr opt_switches;
	/// Стрелки априорные
	VSwitchDataPtr apr_switches;
	/// Стрелки обнаруженные	
	VSwitchSectionsDataPtr opt_switch_sections;
	/// Стрелки априорные
	VSwitchSectionsDataPtr apr_switch_sections;
	
	/// Тип шпал
	VDataProviderPtr aprinfo_spala;
	/// Рихтовочная нить
	VDataProviderPtr aprinfo_straight_side;	

	/// Просадки левые
	VRTOverstepDataPtr pit_left_oversteps;
	/// Просадки левые
	VRTOverstepDataPtr pit_right_oversteps;
	/// ШК
	VRTOverstepDataPtr width_oversteps;
	/// Рихтовки левые
	VRTOverstepDataPtr rich_left_oversteps;
	/// Рихтовки левые
	VRTOverstepDataPtr rich_right_oversteps;
	/// Уровень
	VRTOverstepDataPtr level_oversteps;
	/// Непогашенное ускорение
	VDequeDataPtr anp_oversteps;
	/// Скорость изменения непогашенного ускорения
	VDequeDataPtr psi_oversteps;
	/// Отвод ШК
	VDequeDataPtr otvod_width_oversteps;			

	/// Подготовка очередной страницы
	void PrepareData(const VRTKmDataPtr& in_km);
	/// Формирует имя файла PDF
	std::wstring FormatPDFFileName(int in_km);
	///Возвращает true если километр нужно печатать
	bool MustBePrinted(const VRTKmDataPtr& in_km);

public:
	/// Конструктор
	VRTGeometryGraph(const std::wstring& in_folder, const std::wstring& in_prefix) : VRTGraph(in_folder, in_prefix) 
	{
		Prepare();
	};
	/// Подготовка
	void Prepare();	
	/// Очистка данных
	void ClearData();
	/// Очистка информации о километре
	void ClearSummary();	
};


/// Лента доп. параметров
class VRTExtraGraph : public VRTGraph
{
private:
	
	/// Область износа
	VAreaPtr wear_area;		
	/// Область зазоров левых
	VAreaPtr zazor_left_area;
	/// Область зазоров правых
	VAreaPtr zazor_right_area;			
	
	/// Область износа
	VAxesPtr wear_axes;		
	/// Область зазора левого
	VAxesPtr zazor_left_axes;
	/// Область зазора правого
	VAxesPtr zazor_right_axes;
	
	/// Износ
	VVectorDataPtr wear;
	/// Износ паспортный
	VVectorDataPtr wear_passport;  	
	/// Зазор левый (оптика)
	VDequeDataPtr zazor_left_opt;
	/// Зазор правый (оптика)
	VDequeDataPtr zazor_right_opt;
	/// Зазор левый (магнитный)
	VDequeDataPtr zazor_left_mag;
	/// Зазор правый (магнитный)
	VDequeDataPtr zazor_right_mag;
	/// Метки RFID
	VDequeDataPtr rfid;
			
	/// Износ
	VRTOverstepDataPtr wear_oversteps;	
	/// Зазор левый
	VRTOverstepDataPtr zazor_left_oversteps;
	/// Зазор правый
	VRTOverstepDataPtr zazor_right_oversteps;	

	/// Стрелки обнаруженные	
	VSwitchDataPtr opt_switches;
	/// Стрелки априорные
	VSwitchDataPtr apr_switches;
	/// Стрелки обнаруженные	
	VSwitchSectionsDataPtr opt_switch_sections;
	/// Стрелки априорные
	VSwitchSectionsDataPtr apr_switch_sections;

	/// Подготовка очередной страницы
	void PrepareData(const VRTKmDataPtr& in_km);
	/// Формирует имя файла PDF
	std::wstring FormatPDFFileName(int in_km);
	///Возвращает true если километр нужно печатать
	bool MustBePrinted(const VRTKmDataPtr& in_km);

public:
	/// Конструктор
	VRTExtraGraph(const std::wstring& in_folder, const std::wstring& in_prefix) : VRTGraph(in_folder, in_prefix) 
	{
		Prepare();
	};
	/// Подготовка
	void Prepare();	
	/// Очистка данных
	void ClearData();
	/// Очистка информации о километре
	void ClearSummary();
};


/// Класс управления графиками
class VRTGraphProcessor : public VDataIn<VRTKmDataPtr>, public VDataOut<VRawData>
{
private:
	VRTGeometryGraph GeometryGraph;
	VRTExtraGraph ExtraGraph;
	std::deque<VRawData> print_queue;
public:
	/// Конструктор
	VRTGraphProcessor(const std::wstring& in_folder, const std::wstring& in_prefix, const VTime& in_time, const std::wstring& in_source) : GeometryGraph(in_folder, in_prefix),
		ExtraGraph(in_folder, in_prefix) 
	{
		GeometryGraph.SetDPI(DPI_DEFAULT_PRINTER);	
		GeometryGraph.SetVPS(DETECT_OPTIONS.GetOption(L"VPS").GetValue());
		GeometryGraph.SetPerson(DETECT_OPTIONS.GetOption(L"CHIEF").GetValue());
		GeometryGraph.SetTime(in_time);
		GeometryGraph.SetDataSource(in_source);
		GeometryGraph.SetPDFSplitPolicy(DETECT_OPTIONS.GetOption(L"SPLIT_DIAGRAM").Compare(OPTION_YES) ? SPLIT_KM : NO_SPLIT);
		GeometryGraph.SetPDFEnabled(DETECT_OPTIONS.GetOption(L"SAVE_GEOMETRY").Compare(OPTION_YES));
		//Путевая лента доппараметров	
		ExtraGraph.SetDPI(DPI_DEFAULT_PRINTER);	
		ExtraGraph.SetVPS(DETECT_OPTIONS.GetOption(L"VPS").GetValue());
		ExtraGraph.SetPerson(DETECT_OPTIONS.GetOption(L"CHIEF").GetValue());
		ExtraGraph.SetTime(in_time);
		ExtraGraph.SetDataSource(in_source);
		ExtraGraph.SetPDFSplitPolicy(DETECT_OPTIONS.GetOption(L"SPLIT_DIAGRAM").Compare(OPTION_YES) ? SPLIT_KM : NO_SPLIT);
		ExtraGraph.SetPDFEnabled(DETECT_OPTIONS.GetOption(L"SAVE_EXTRA").Compare(OPTION_YES));
	};
	/// Ввод данных
	void Put(const VRTKmDataPtr& in_data)
	{
		GeometryGraph.ProcessKm(in_data);
		while (GeometryGraph.GetPageCount()>0)
		{
			print_queue.push_back(GeometryGraph.GetPage());
			GeometryGraph.PopPage();
		}
		ExtraGraph.ProcessKm(in_data);
		while (ExtraGraph.GetPageCount()>0)
		{
			print_queue.push_back(ExtraGraph.GetPage());
			ExtraGraph.PopPage();
		}
	};
	/// Переключает печать
	void SetPrintGeometry(bool in_enabled)///FixMe!!! использовать OPTION
	{
		GeometryGraph.SetPrintingEnabled(in_enabled);
	};
	/// Переключает печать
	void SetPrintExtra(bool in_enabled)///FixMe!!! использовать OPTION
	{
		ExtraGraph.SetPrintingEnabled(in_enabled);
	};
	/// Возвращает true, если есть обработанные данные
	bool Ready() const 
	{
		return print_queue.size()>0;
	};	
	/// Возвращает true, если обработанных данных нет
	bool Empty() const
	{
		return print_queue.empty();
	};
	/// Возвращает первый элемент данных
	const VRawData& Front() const
	{
		return print_queue.front();
	};
	/// Выталкивает первый элемент данных
	void Pop()
	{
		print_queue.pop_front();
	};		
};

typedef boost::shared_ptr<VRTGraphProcessor> VRTGraphProcessorPtr;