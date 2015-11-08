#pragma once

#include "VDrawing.h"
#include "axes.h"
#include "dblib.h"
#include "VParamCorrections.h"

/// Тип значка стрелки
enum SWITCH_GLYPH_TYPE
{
	SW_UNDEFINED=0,
	SW_LEFT_ALONG=1,
	SW_LEFT_COUNTER=2,
	SW_RIGHT_ALONG=3,
	SW_RIGHT_COUNTER=4
};

/// Вычисляет тип значка стреклки
static SWITCH_GLYPH_TYPE SwitchGlyphType(side_t in_side, SWITCH_DIR in_dir)
{
	if (in_side==SD_LEFT && in_dir==SDIR_ALONG)
	{
		return SW_LEFT_ALONG;
	}
	else if (in_side==SD_LEFT && in_dir==SDIR_COUNTER)
	{
		return SW_LEFT_COUNTER;
	}
	else if (in_side==SD_RIGHT && in_dir==SDIR_ALONG)
	{
		return SW_RIGHT_ALONG;
	}
	else if (in_side==SD_RIGHT && in_dir==SDIR_COUNTER)
	{
		return SW_RIGHT_COUNTER;
	}
	else
	{
		return SW_UNDEFINED;
	}
};

/** Информация о графике*/
class VGraphInfo
{
private:
	/** Имя графика*/
	std::wstring graph_name;
	/** Имена файлов*/
	std::vector<std::wstring> file_names;
	/** Индекс активного файла*/
	size_t active_file_index;
	/** Описание участка*/
	std::wstring way_section;	
public:
	/** Конструктор*/
	VGraphInfo(const std::wstring in_graph_name, const std::vector<VDataSetPtr>& in_datasets, 
		const VDataSetPtr& active_data_set, const std::wstring& in_actual_dir, 
		const std::wstring& in_actual_way, const std::wstring& in_actual_date);
	/** Возвращает имя графика*/
	const std::wstring& GetGraphName() const;
	/** Возвращает имена файлов*/
	const std::vector<std::wstring>& GetFileNames() const;
	/** Возвращает индекс активного файла*/
	size_t GetActiveFileIndex() const;
	/** Возвращает описание участка*/
	const std::wstring& GetWaySection() const;
};
typedef boost::shared_ptr<VGraphInfo> VGraphInfoPtr;

/** Положение мыши*/
class VMousePositionData : public VXOrderedDataProvider
{
private:
	double x;
	double y;
public:
	/** Конструктор*/	
	VMousePositionData(){};	
	/** Возвращает размер данных*/
	size_t GetCount() const {return 1;};	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const {return x;};	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const {return y;};	
	/** Возвращает флаги для точки, признаки сбоев*/
	virtual uint64_t GetFlags(size_t in_index) const {return 0;};	
	/** Возвращает минимальное значение по Y*/
	double GetMinY() {return y;};
	/** Возвращает максимальное значение по Y*/
	double GetMaxY() {return y;};
	/** Возвращает минимальное значение по X*/
	double GetMinX() {return x;};
	/** Возвращает максимальное значение по X*/
	double GetMaxX() {return x;};	
	/** Обработчик сообщений*/
	bool ProcessEvent(const VEvent& in_event)
	{
		switch (in_event.GetCode())
		{
			case EVENT_MOUSE_MOTION:
			{
				x=((VMouseEvent&)in_event).GetX();
				y=((VMouseEvent&)in_event).GetY();
				return true;
			}
			default:
				return false;
		}
	}	
};
typedef boost::shared_ptr<VMousePositionData> VMousePositionDataPtr;

/** Константа*/
class VYConstantData : public VDataProvider
{
private:
	/** Значение*/	
	double value;	
	/** Отображаемоый текст*/
	std::wstring label;
	/** Оси координат*/
	VAxesWeakPtr axes;
public:
	/** Конструктор*/	
	VYConstantData(VAxesWeakPtr in_axes, double in_value, const std::wstring& in_label) : 
	  axes(in_axes), value(in_value), label(in_label){};
	/** Возвращает размер данных*/
	size_t GetCount() const {return 2;};	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const 
	{
		if (VAxesPtr a=axes.lock())
			return (in_index==0) ? a->GetMinX() : a->GetMaxX();
		else
			return 0;		
	};	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const {return value;};		
	/** Возвращает минимальное значение по Y*/
	double GetMinY() {return value;};
	/** Возвращает максимальное значение по Y*/
	double GetMaxY() {return value;};
	/** Возвращает минимальное значение по X*/
	double GetMinX() {return GetX(0);};
	/** Возвращает максимальное значение по X*/
	double GetMaxX() {return GetX(1);};
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const {return label;};
};
typedef boost::shared_ptr<VYConstantData> VYConstantDataPtr;

/** Коридор*/
class VYRangeData : public VDataProvider
{
private:
	/** Минимальное значение*/	
	double min_value;
	/** Максимальное значение*/	
	double max_value;	
	/** Оси координат*/
	VAxesWeakPtr axes;
public:
	/** Конструктор*/	
	VYRangeData(VAxesWeakPtr in_axes, double in_min_value, double in_max_value) : axes(in_axes), min_value(in_min_value), max_value(in_max_value){};	
	/** Возвращает размер данных*/
	size_t GetCount() const {return 1;};	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const 
	{
		if (VAxesPtr a=axes.lock())
			return a->GetMinX();
		else
			return 0;
	};	
	/** Возвращает координату по оси X*/
	double GetSizeX(size_t in_index) const 
	{
		if (VAxesPtr a=axes.lock())
			return a->GetMaxX()-a->GetMinX();
		else
			return 0;
	};
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const {return min_value;};
	/** Возвращает координату по оси X*/
	double GetSizeY(size_t in_index) const {return max_value-min_value;};
	/** Возвращает минимальное значение по Y*/
	double GetMinY() {return min_value;};
	/** Возвращает максимальное значение по Y*/
	double GetMaxY() {return max_value;};
	/** Возвращает минимальное значение по X*/
	double GetMinX() {return GetX(0);};
	/** Возвращает максимальное значение по X*/
	double GetMaxX() {return GetX(0)+GetSizeX(0);};		
};
typedef boost::shared_ptr<VYRangeData> VYRangeDataPtr;

/** Опция*/
class VYOptionData : public VDataProvider
{
private:
	/** Имя опции*/	
	std::wstring option_name;
	/// Оси координат
	VAxesWeakPtr axes;
public:
	/** Конструктор*/	
	VYOptionData(VAxesWeakPtr in_axes, const std::wstring in_option_name) : axes(in_axes), option_name(in_option_name)
	{
		//Проверяем наличие опции
		if (!OPTIONS.HasOption(option_name))
		{
			throw VSimpleException(L"Некорректный параметр", in_option_name);
		}
	};	
	/** Возвращает размер данных*/
	size_t GetCount() const {return 2;};	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const 
	{
		if (VAxesPtr a=axes.lock())
			return (in_index==0) ? a->GetMinX() : a->GetMaxX();
		else
			return 0;
	};	
	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const {return OPTIONS.GetOption(option_name).GetValue<double>();};	
	/** Возвращает флаги для точки, признаки сбоев*/
	virtual uint64_t GetFlags(size_t in_index) const {return 0;};	
	/** Возвращает минимальное значение по Y*/
	double GetMinY() {return OPTIONS.GetOption(option_name).GetValue<double>();};
	/** Возвращает максимальное значение по Y*/
	double GetMaxY() {return OPTIONS.GetOption(option_name).GetValue<double>();};
	/** Возвращает минимальное значение по X*/
	double GetMinX() 
	{
		return GetX(0);
	};
	/** Возвращает максимальное значение по X*/
	double GetMaxX() 
	{
		return GetX(1);
	};	
};
typedef boost::shared_ptr<VYOptionData> VYOptionDataPtr;

/** Симметричные границы - опция*/
class VSymmetricYRangeOptionData : public VDataProvider
{
private:
	/** Имя опции*/	
	std::wstring option_name;
	/// Оси координат
	VAxesWeakPtr axes;
public:
	/** Конструктор*/	
	VSymmetricYRangeOptionData(VAxesWeakPtr in_axes, const std::wstring in_option_name) : axes(in_axes), option_name(in_option_name)
	{
		//Проверяем наличие опции
		if (!OPTIONS.HasOption(option_name))
		{
			throw VSimpleException(L"Некорректный параметр", in_option_name);
		}
	};	
	/** Возвращает размер данных*/
	size_t GetCount() const {return 1;};	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const 
	{
		if (VAxesPtr a=axes.lock())
			return a->GetMinX();
		else
			return 0;
	};
	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const
	{
		if (VAxesPtr a=axes.lock())
			return a->GetMaxX()-a->GetMinX();
		else
			return 0;
	};	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const {return -OPTIONS.GetOption(option_name).GetValue<double>();};
	/** Возвращает размер по оси Y*/
	double GetSizeY(size_t in_index) const {return 2*OPTIONS.GetOption(option_name).GetValue<double>();};	
	/** Возвращает флаги для точки, признаки сбоев*/
	virtual uint64_t GetFlags(size_t in_index) const {return 0;};	
	/** Возвращает минимальное значение по Y*/
	double GetMinY() {return -OPTIONS.GetOption(option_name).GetValue<double>();
	};
	/** Возвращает максимальное значение по Y*/
	double GetMaxY() {return OPTIONS.GetOption(option_name).GetValue<double>();
	};
	/** Возвращает минимальное значение по X*/
	double GetMinX() {return GetX(0);};
	/** Возвращает максимальное значение по X*/
	double GetMaxX() {return GetX(0)+GetSizeX(0);};	
};
typedef boost::shared_ptr<VSymmetricYRangeOptionData> VSymmetricYRangeOptionDataPtr;

/** Элемент данных*/ 
struct VGraphPointData
{
	double x;
	double y;
	double xsize;
	double ysize;
	std::wstring label;
	uint64_t flags;

	VGraphPointData() : x(0), y(0), xsize(0), ysize(0), label(L""), flags(0){};

	VGraphPointData(double in_x, double in_y, double in_xsize, double in_ysize, const std::wstring& in_label,
		uint64_t in_flags) : x(in_x), y(in_y), xsize(in_xsize), ysize(in_ysize), label(in_label), flags(in_flags){};

	bool operator<(const VGraphPointData& in_r) const {return x<in_r.x;};
};

#define COORDINATE_KM_5 0
#define COORDINATE_KM 1
#define COORDINATE_PK 2

/** Источник данных - путевые координаты (отметки километров, пикетов и через 10 метров)*/
class VCoordinatesData : public VXOrderedDataProvider
{	
private:		
	/** Абсолютные координаты*/
	std::deque<double> xdata;
	/** Размеры*/
	std::deque<double> xsizedata;
	/** Флаги (признаки: километр, пикет)*/
	std::deque<uint64_t> flags;
	/** Метки*/
	std::deque<std::wstring> label;
public:		
	/** Конструктор*/	
	VCoordinatesData(const VAprinfo& in_aprinfo, pro::interval<double> in_range);	
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси X*/
	double GetSizeX(size_t in_index) const;
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает флаги для точки, признаки сбоев*/
	virtual uint64_t GetFlags(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VCoordinatesData> VCoordinatesDataPtr;

/** Данные для графика*/
class VParameterData : public VXOrderedDataProvider
{	
private:	
	/** Тип данных*/
	PARAMETER_TYPE data_type;	
	/** Указатель на источник данных*/
	VDataSetPtr data_set;	
public:		
	/** Конструктор*/	
	VParameterData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает флаги для точки, признаки сбоев*/
	virtual uint64_t GetFlags(size_t in_index) const;
	/** Устанавливает флаги, признаки сбоев*/
	virtual void SetFlags(size_t in_index, uint64_t in_flags);
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();				
	/** Возвращает значение Y по X (через уравнение прямой, по умолчанию не работает)*/
	virtual double GetYbyX(double in_x);
	/** Возвращает видимую область*/
	virtual std::vector<size_t> GetVisible(double in_left, double in_top, double in_right, double in_bottom);
};
typedef boost::shared_ptr<VParameterData> VParameterDataPtr;

/** Источник данных - функция*/
class VParameterFunctionData : public VXOrderedDataProvider
{	
private:	
	/** Источник данных*/
	VDataSetPtr data_set;	
	/** Параметр*/
	VParameter parameter;
public:		
	/** Конструктор*/	
	VParameterFunctionData(VDataSetPtr in_data_set, const std::wstring& in_function);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает флаги для точки, признаки сбоев*/
	virtual uint64_t GetFlags(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();				
	/** Возвращает значение Y по X (через уравнение прямой, по умолчанию не работает)*/
	virtual double GetYbyX(double in_x);
	/** Возвращает видимую область*/
	virtual std::vector<size_t> GetVisible(double in_left, double in_top, double in_right, double in_bottom);
};
typedef boost::shared_ptr<VParameterFunctionData> VParameterFunctionDataPtr;

/** Скорость вагона*/
class VCurrentSpeedData : public VXOrderedDataProvider
{	
private:		
	/** Указатель на источник данных*/
	VDataSetPtr data_set;
	/** Признак пустого параметра*/
	bool empty;	
public:		
	/** Конструктор*/	
	VCurrentSpeedData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();				
	/** Возвращает значение Y по X (через уравнение прямой, по умолчанию не работает)*/
	virtual double GetYbyX(double in_x);	
};
typedef boost::shared_ptr<VCurrentSpeedData> VCurrentSpeedDataPtr;

/** Зазор левый*/
class VZazorLeftData : public VXOrderedDataProvider
{
private:			
	/** Данные*/
	VDataSetPtr data_set;	
public:		
	/** Конструктор*/	
	VZazorLeftData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;
	/** Возвращает флаги для точки, признаки сбоев*/
	uint64_t GetFlags(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();			
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VZazorLeftData> VZazorLeftDataPtr;

/** Зазор правый*/
class VZazorRightData : public VXOrderedDataProvider
{
private:			
	/** Данные*/
	VDataSetPtr data_set;	
public:		
	/** Конструктор*/	
	VZazorRightData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;
	/** Возвращает флаги для точки, признаки сбоев*/
	uint64_t GetFlags(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();			
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VZazorRightData> VZazorRightDataPtr;

/** Зазор магнитный левый*/
class VMagZazorLeftData : public VXOrderedDataProvider
{
private:			
	/** Данные*/
	VDataSetPtr data_set;	
public:		
	/** Конструктор*/	
	VMagZazorLeftData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;
	/** Возвращает флаги для точки, признаки сбоев*/
	uint64_t GetFlags(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();			
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VMagZazorLeftData> VMagZazorLeftDataPtr;

/** Зазор магнитный правый*/
class VMagZazorRightData : public VXOrderedDataProvider
{
private:			
	/** Данные*/
	VDataSetPtr data_set;	
public:		
	/** Конструктор*/	
	VMagZazorRightData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;
	/** Возвращает флаги для точки, признаки сбоев*/
	uint64_t GetFlags(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();			
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VMagZazorRightData> VMagZazorRightDataPtr;

/** Угон левый*/
class VUgonLeftData : public VXOrderedDataProvider
{
private:			
	/** Данные*/
	VDataSetPtr data_set;	
public:		
	/** Конструктор*/	
	VUgonLeftData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;
	/** Возвращает флаги для точки, признаки сбоев*/
	uint64_t GetFlags(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();			
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VUgonLeftData> VUgonLeftDataPtr;

/** Угон правый*/
class VUgonRightData : public VXOrderedDataProvider
{
private:			
	/** Данные*/
	VDataSetPtr data_set;	
public:		
	/** Конструктор*/	
	VUgonRightData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;
	/** Возвращает флаги для точки, признаки сбоев*/
	uint64_t GetFlags(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();			
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VUgonRightData> VUgonRightDataPtr;



/** Метки шпал*/
class VSpalaMarkerData : public VXOrderedDataProvider
{
private:	
	/** Данные*/
	VDataSetPtr data_set;
	/** Тип данных для координат по оси Y*/
	PARAMETER_TYPE data_type;
public:		
	/** Конструктор*/	
	VSpalaMarkerData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();			
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
};
typedef boost::shared_ptr<VSpalaMarkerData> VSpalaMarkerDataPtr;

/** Источник данных - метки оператора, где координаты по Y могут быть привязаны к некоторому параметру*/
class VOperatorMarkData : public VXOrderedDataProvider
{
private:
	/** Указатель на данные*/
	VDataSetPtr data_set;
	/** Тип данных для координат по оси Y*/
	PARAMETER_TYPE data_type;	
public:
	/** Конструктор*/
	VOperatorMarkData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type=PRM_NONE);
	/** Возвращает число меток*/
	size_t GetCount() const;
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
	/** Выделяет точку*/
	void SelectPoint(size_t in_index);
	/** Возвращает выделенную точку*/
	size_t GetSelectedPoint() const;
	/** Снимает выделение*/
	void DeselectPoint();
};
typedef boost::shared_ptr<VOperatorMarkData> VOperatorMarkDataPtr;

/** Маркеры на пути*/
class VWayMarkerData : public VXOrderedDataProvider
{
private:	
	/** Данные*/
	VDataSetPtr data_set;
	/** Тип данных для координат по оси Y*/
	PARAMETER_TYPE data_type;
public:		
	/** Конструктор*/	
	VWayMarkerData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();		
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
};
typedef boost::shared_ptr<VWayMarkerData> VWayMarkerDataPtr;


/** Данные для проектых меток профиля (FixMe!!! Изменить термин)*/
class VProfileMarkData : public VXOrderedDataProvider
{
private:	

	/** Значение по оси Y*/
	std::vector<float> ydata;
	/** Значение по оси X*/
	std::vector<double> xdata;	
	
public:
	/** Конструктор*/
	VProfileMarkData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();
};

/** Данные для разметки профиля (элементов) FixMe!!! изменить идентификатор*/
class VProfileElementsData : public VXOrderedDataProvider
{
private:	

	/** Указатель на источник данных в файле*/
	VDataSetPtr data_set;	
	/** Перемещение метки*/
	bool drag;
	/** Индекс перемещаемой метки*/
	size_t drag_index;	
public:
	/** Конструктор*/
	VProfileElementsData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;			
	/** Устанавливает значение X*/
	void SetXY(size_t in_index, double in_x, double in_y);	
	/** Добавляет точку*/
	void AddXY(size_t in_index, double in_x, double in_y, double in_size_x=0, double in_size_y=0, 
		const std::wstring& in_label=L"", uint64_t in_flags=0);
	/** Удаляет точку*/
	void DeleteItem(size_t in_index);	
	/** Удаляет все метки*/
	void DeleteAll();	
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
	/** Устанавливает флаг*/
	virtual void SetFlags(size_t in_index, uint64_t in_flags);
	/** Возвращает флаг (признак типа элемента)*/
	virtual uint64_t GetFlags(size_t in_index) const;		
};
typedef boost::shared_ptr<VProfileElementsData> VProfileElementsDataPtr;

#define SLOPE_ZERO 0
#define SLOPE_POSITIVE 1
#define SLOPE_NEGATIVE 2
#define SLOPE_HILL 3
#define SLOPE_PIT 4

/** Источник данных - уклоны профиля*/
class VSlopeData : public VXOrderedDataProvider
{
private:
	/** Данные*/
	VDataSetPtr data_set;
	/** Тип уклонов*/
	PARAMETER_TYPE data_type;	
public:
	/** Конструктор*/
	VSlopeData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;		
	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const;	
	/** Возвращает размер по оси Y*/
	double GetSizeY(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
	/** Возвращает флаг*/
	uint64_t GetFlags(size_t in_index) const;	
	/** Переключает источник данных*/
	void SetType(PARAMETER_TYPE in_data_type);
	/** Возвращает тип загруженных данных*/
	PARAMETER_TYPE GetType();
	/** Возвращает структуру данных*/
	const void * GetUserData(size_t in_index) const;
};
typedef boost::shared_ptr<VSlopeData> VSlopeDataPtr;

/** Источник данных - углы между элементами профиля*/
class VSlopeAngleData : public VXOrderedDataProvider
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Тип уклонов*/
	PARAMETER_TYPE data_type;
public:
	/** Конструктор*/
	VSlopeAngleData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;			
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Переключает источник данных*/
	void SetType(PARAMETER_TYPE in_data_type);
	/** Возвращает тип загруженных данных*/
	PARAMETER_TYPE GetType();
};
typedef boost::shared_ptr<VSlopeAngleData> VSlopeAngleDataPtr;


/** Источник данных - отметки профиля по элементам/километрам/пикетам*/
class VHeightData : public VXOrderedDataProvider
{
private:
	/** Данные*/
	VDataSetPtr data_set;
	/** Тип уклонов*/
	PARAMETER_TYPE data_type;	
public:
	/** Конструктор*/
	VHeightData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;			
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
	/** Возвращает флаг*/
	uint64_t GetFlags(size_t in_index) const;	
	/** Переключает источник данных*/
	void SetType(PARAMETER_TYPE in_data_type);
	/** Возвращает тип загруженных данных*/
	PARAMETER_TYPE GetType();
};
typedef boost::shared_ptr<VHeightData> VHeightDataPtr;

/** Источник данных - непогашенное ускорение модельное*/
class VAnpModelData : public VXOrderedDataProvider
{
private:	

	/** Указатель на источник данных в файле*/
	VDataSetPtr data_set;	
	/** Тип скорости*/
	speed_type speed;
	/// Инверсия параметра
	double sign_y;

public:
	/** Конструктор*/	
	VAnpModelData(VDataSetPtr in_data_set, speed_type in_speed, bool in_inverse);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();		
};
typedef boost::shared_ptr<VAnpModelData> VAnpModelDataPtr;

/** Источник данных информация о параметрах кривых*/
class VCurvesInfo
{
private:
	/** Данные*/
	VDataSetPtr data_set;	
	/** Признак модель/факт*/
	bool is_model;
public:
	/** Конструктор*/
	VCurvesInfo(VDataSetPtr in_data_set, bool in_model=false);
	/** Возвращает число кривых*/
	size_t GetCount() const;
	/** Возвращает кривую*/
	const VCurve& GetCurve(size_t in_index) const;
	/** Возвращает валдность кривой*/
	bool Valid(size_t in_index);
	/** Возвращает начало кривой*/
	double GetAbsStart(size_t in_index) const;
	/** Возвращает конец кривой*/
	double GetAbsEnd(size_t in_index) const;
	/** Возвращает начало кривой (уровень)*/
	double GetAbsLevelStart(size_t in_index) const;
	/** Возвращает конец кривой (уровень)*/
	double GetAbsLevelEnd(size_t in_index) const;
	/** Возвращает ориентацию*/
	long GetOrientation(size_t in_index) const;
	/** Возвращает число радиусов*/
	size_t GetRadiusCount(size_t in_index) const;
	/** Возвращает начало радиуса*/
	double GetAbsStart(size_t in_index, size_t in_radius_index) const;
	/** Возвращает конец радиуса*/
	double GetAbsEnd(size_t in_index, size_t in_radius_index) const;
	/** Возвращает радиус*/
	double GetRadius(size_t in_index, size_t in_radius_index) const;
	/** Возвращает возвышение*/
	double GetLevel(size_t in_index, size_t in_radius_index) const;
	/** Возвращает длину круговой*/
	double GetLength(size_t in_index, size_t in_radius_index) const;
	/** Возвращает длину первой переходной*/
	double GetLength1(size_t in_index, size_t in_radius_index) const;
	/** Возвращает длину второй переходной*/
	double GetLength2(size_t in_index, size_t in_radius_index) const;
	/** Возвращает угол поворота*/
	double GetAngle(size_t in_index, size_t in_radius_index) const;
	/** Возвращает метры путевой координаты начала радиуса*/
	double GetMStart(size_t in_index, size_t in_radius_index) const;
	/** Возвращает метры путевой координаты конца радиуса*/
	double GetMEnd(size_t in_index, size_t in_radius_index) const;
	/** Возвращает статус кривой*/
	std::wstring GetStatus(size_t in_index) const;
	/** Возвращает true если элемент кривой - круговая*/
	bool GetElementType(size_t in_index, size_t in_element_index) const;
	/** Возвращает длину элемента кривой*/
	double GetElementLength(size_t in_index, size_t in_element_index) const;
	/** Возвращает радиус элемента кривой*/
	double GetElementRadius(size_t in_index, size_t in_element_index) const;
	/** Возвращает длину элемента кривой (уровень)*/
	double GetElementLengthLevel(size_t in_index, size_t in_element_index) const;
	/** Возвращает возвышение элемента кривой*/
	double GetElementLevel(size_t in_index, size_t in_element_index) const;
	/** Возвращает Vкр*/
	double GetModelVkr_high_bt(size_t in_index, size_t in_element_index) const;	
	/** Возвращает Vкр*/
	double GetModelVkr_high(size_t in_index, size_t in_element_index) const;
	/** Возвращает Vкр*/
	double GetModelVkr_pass(size_t in_index, size_t in_element_index) const;	
	/** Возвращает Vкр*/
	double GetModelVkr_truck(size_t in_index, size_t in_element_index) const;
	/** Возвращает Vиз*/
	double GetModelViz_high_bt(size_t in_index, size_t in_element_index) const;
	/** Возвращает Vиз*/
	double GetModelViz_high(size_t in_index, size_t in_element_index) const;
	/** Возвращает Vиз*/
	double GetModelViz_pass(size_t in_index, size_t in_element_index) const;
	/** Возвращает Vпр*/
	double GetModelVpr(size_t in_index, size_t in_element_index) const;
	/** Возвращает отвод возвышения*/
	double GetModelTap(size_t in_index, size_t in_element_index) const;
	/** Возвращает пси*/
	double GetModelPsi_high_bt(size_t in_index, size_t in_element_index) const;
	/** Возвращает пси*/
	double GetModelPsi_high(size_t in_index, size_t in_element_index) const;
	/** Возвращает пси*/
	double GetModelPsi_pass(size_t in_index, size_t in_element_index) const;
	/** Возвращает пси*/
	double GetModelPsi_truck(size_t in_index, size_t in_element_index) const;
	/** Возвращает пси*/
	double GetModelPsi_avg(size_t in_index, size_t in_element_index) const;
	/** Возвращает Aнп скор. НК*/
	double GetModelAnpHighBT(size_t in_index, size_t in_element_index) const;	
	/** Возвращает Aнп скор.*/
	double GetModelAnpHigh(size_t in_index, size_t in_element_index) const;	
	/** Возвращает Aнп пасс.*/
	double GetModelAnpPass(size_t in_index, size_t in_element_index) const;	
	/** Возвращает Aнп груз.*/
	double GetModelAnpTruck(size_t in_index, size_t in_element_index) const;	
	/** Возвращает Aнп средн.*/
	double GetModelAnpAvg(size_t in_index, size_t in_element_index) const;
	/** Возвращает Vhigh_bt*/
	speed_t GetModelVhigh_bt(size_t in_index) const;
	/** Возвращает Vhigh*/
	speed_t GetModelVhigh(size_t in_index) const;
	/** Возвращает Vpass*/
	speed_t GetModelVpass(size_t in_index) const;
	/** Возвращает Vcrg*/
	speed_t GetModelVcrg(size_t in_index) const;
	/** Возвращает Vavg*/
	speed_t GetModelVavg(size_t in_index) const;
	/** Возвращает максимальное отклонение в плане*/
	double GetMaxPlanOverstep(size_t in_index);
};
typedef boost::shared_ptr<VCurvesInfo> VCurvesInfoPtr;

/** Источник данных - прямые участки*/
class VLinesData : public VXOrderedDataProvider
{
private:
	/** Данные*/
	VDataSetPtr data_set;
public:
	/** Конструктор*/
	VLinesData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает размер "точки" по X*/
	double GetSizeX(size_t in_index) const;
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
};
typedef boost::shared_ptr<VLinesData> VLinesDataPtr;

/** Источник данных - углы перелома плана*/
class VLineAngleData : public VXOrderedDataProvider
{
private:
	/** Данные*/
	VDataSetPtr data_set;
public:
	/** Конструктор*/
	VLineAngleData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
};
typedef boost::shared_ptr<VLineAngleData> VLineAngleDataPtr;

/** Источник данных - съезды на боковые пути*/
class VSidewayData : public VDataProvider
{
private:
	/** Данные*/
	VDataSetPtr data_set;
public:
	/** Конструктор*/
	VSidewayData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает размер "точки" по X*/
	double GetSizeX(size_t in_index) const;
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;

	/** Добавляет боковой путь*/
	void AddXY(size_t in_index, double in_x, double in_y, double in_size_x=0, double in_size_y=0, 
		const std::wstring& in_label=L"", uint64_t in_flags=0);
	/** Удаляет боковой путь*/
	void DeleteItem(size_t in_index);
	/** Устанавливает тип участка*/
	void SetFlags(size_t in_index, uint64_t in_flags);
};
typedef boost::shared_ptr<VSidewayData> VSidewayDataPtr;

/** Структура данных*/
struct VPointData
{
	double x;
	double y;
	double xsize;
	double ysize;
	std::wstring label;
	std::wstring label_x;
	std::wstring label_y;
	uint64_t flags;
};


/** Источник данных - БД*/
class VQueryData : public VXOrderedDataProvider
{
private:
	/** Указатель на БД*/
	VIBDatabasePtr database;
	/** Указатель на транзакцию*/
	VIBTransactionPtr transaction;
	/** Текст запроса*/
	std::wstring query;
	/** Данные*/
	std::vector<VPointData> data;

	double min_x;
	double max_x;
	double min_y;
	double max_y;

public:
	/** Конструктор*/
	VQueryData(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, const std::wstring& in_query);
	/** Обновляет данные*/
	virtual void RefreshData();
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;
	/** Возвращает размер "точки" по X (имеет смысл для протяженных объектов)*/
	double GetSizeX(size_t in_index) const;
	/** Возвращает размер "точки" по Y (имеет смысл для протяженных объектов)*/
	double GetSizeY(size_t in_index) const;
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
	/** Возвращает флаг - тип метки*/
	uint64_t GetFlags(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();		
};

/** Данные о текущей выделенной области*/
class VSelectionInfo : public VActiveObject
{
private:
	/** Область выделения - координта x первой точки*/
	double select_start_x;
	/** Область выделения - координта y первой точки*/
	double select_start_y;
	/** Область выделения - координта x второй точки*/
	double select_end_x;
	/** Область выделения - координта y второй точки*/
	double select_end_y;
	/** Мышь над левой границей*/
	bool hit_left;
	/** Мышь над правой границей*/
	bool hit_right;
	/** Признак того что происходит выделение*/
	bool selecting;	
	/** Признак того что мышь над одной из сторон*/
	bool hit;
public:
	/** Конструктор*/
	VSelectionInfo();
	/** Начало выделения*/
	void SetStart(double in_x, double in_y);
	/** Конец выделения*/
	void SetEnd(double in_x, double in_y);
	/** Возвращает X координату начальной точки*/
	double GetStartX() const;
	/** Возвращает Y координату начальной точки*/
	double GetStartY() const;
	/** Возвращает X координату конечной точки*/
	double GetEndX() const;
	/** Возвращает Y координату конечной точки*/
	double GetEndY() const;	
	/** Сброс значений в 0*/
	void Reset();
	/** Возвращает true, если область ненулевая*/
	bool Valid() const;	
	/** Устанавливает признак того, что происходит выделение*/
	void SetSelecting(bool in_selecting);
	/** Возвращает признак того, что происходит выделение*/
	bool GetSelecting() const;	
	/** Устанавливает признак активности*/
	void SetActive(bool in_active);
	
	/** Устанавливает признак того что мышь над одной из сторон*/
	void SetHit(bool in_hit){hit=in_hit;};
	/** Возвращает признак того что мышь над одной из сторон*/
	bool GetHit() const {return hit;};

};
typedef boost::shared_ptr<VSelectionInfo> VSelectionInfoPtr;

/** Данные о текущей странице*/
class VPageInfo
{
private:
	/** Текущая страница*/
	size_t page;
public:
	/** Возвращает текущую страницу*/
	size_t GetPage() const;
	/** Устанавливает текущую страницу*/
	void SetPage(size_t in_page);
};
typedef boost::shared_ptr<VPageInfo> VPageInfoPtr;

//----------------- Второй вариант меток кривых -------------------------

//Флаги меток
#define FIRST_MARK 1
#define INNER_MARK 2
#define LAST_MARK 3

#define ACTIVE_FIRST_MARK 4
#define ACTIVE_INNER_MARK 5
#define ACTIVE_LAST_MARK 6

struct index_cache_t
{	
	size_t first;
	size_t second;
	size_t third;
	index_cache_t() : first(0), second(0), third(0){};
	index_cache_t(size_t in_first, size_t in_second, size_t in_third) : 
		first(in_first), second(in_second), third(in_third){};
	bool operator<(const index_cache_t& right) const
	{
		return third<right.third;
	};
};



/** Источник данных - метки кривых по кривизне*/
class VCurveMarksData : public VXOrderedDataProvider
{
private:		
	/** Источник данных*/
	VDataSetPtr data_set;	
	/** Число кривых*/
	mutable size_t curves_count;
	/** Число меток активной кривой*/
	mutable size_t active_mark_count;
	/** Кэш*/
	mutable std::vector<index_cache_t> cache;	
	/** Дип данных кривизна/уровень*/
	PARAMETER_TYPE data_type;
	/** Тип разметки факт/паспорт*/
	PARAMETER_TYPE mark_type;
	/** Преобразует сквозной индекс в пару - кривая/метка*/
	index_cache_t IndexToPair(size_t in_index) const;
	/** Возвращает число меток кривой*/
	size_t GetMarkCount(size_t in_index) const;
	/** Возвращает ссылку на метку*/
	VCurveMark& GetMark(size_t in_curve_index, size_t in_mark_index);
	/** Возвращает ссылку на метку*/
	const VCurveMark& GetMark(size_t in_curve_index, size_t in_mark_index) const;
	/** Добавляет метку*/
	bool AddMark(VCurve& in_curve, size_t in_index, float in_value);	
	/** Удаляет метку*/
	void DeleteMark(size_t in_index);
	/** Нормализует метки*/
	void NormalizeMarks();
	/** Формирует кэш*/
	void BuildCache() const;	
	
public:
	/** Конструктор*/
	VCurveMarksData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type=PRM_CURV, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;
	/** Возвращает флаг - тип метки*/
	uint64_t GetFlags(size_t in_index) const;
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
	/** Устанавливает значение X*/
	void SetXY(size_t in_index, double in_x, double in_y);	
	/** Добавляет точку*/
	void AddXY(size_t in_index, double in_x, double in_y, double in_size_x=0, double in_size_y=0, 
		const std::wstring& in_label=L"", uint64_t in_flags=0);
	/** Удаляет метку*/
	void DeleteItem(size_t in_index);	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();		

	/** Выделяет точку*/
	void SelectPoint(size_t in_index);
	/** Возвращает выделенную точку*/
	size_t GetSelectedPoint() const;
	/** Снимает выделение*/
	void DeselectPoint();

	/** Помечает группу точек*/
	virtual void MarkPoints(size_t in_from, size_t in_to);
	/** Снимает пометку*/
	virtual void UnmarkPoints();

	/** Возвращает видимую область*/
	virtual std::vector<size_t> GetVisible(double in_left, double in_top, double in_right, double in_bottom);
};
typedef boost::shared_ptr<VCurveMarksData> VCurveMarksDataPtr;

class VCoordPointData : public VXOrderedDataProvider
{
private:
	/** Указатель на источник данных*/
	VDataSetPtr data_set;
	/** Указатель на тип объектов*/
	POINT_TYPE point_type;
	/* Индексы начала и конца в общем массивеобъектов пути*/
	std::vector<size_t> index;
public:
	/** Конструктор*/	
	VCoordPointData(VDataSetPtr in_data_set, VIBDatabasePtr in_database, POINT_TYPE in_type);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
	/*Выделяет точку*/
	void SelectPoint(size_t in_index);
	/*Возвращает выделенную точку*/
	size_t GetSelectedPoint() const;
	/*Снимает выделение*/
	void DeselectPoint();
	/* Удаление объекта*/
	//void DeleteItem(size_t in_index);
	/* Добавим объект в массив     */
	/* Нужно для функции выделения */
	void RefreshData();
};

class VLine_tData : public VXOrderedDataProvider
{
private:
	/** Указатель на источник данных*/
	VDataSetPtr data_set;
	/*Тип данных*/
	POINT_TYPE point_type;
public:
	/** Конструктор*/
	VLine_tData(VDataSetPtr in_data_set, POINT_TYPE type);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();
};

/** Типы параметров кривой*/
enum CURVE_PARAM_TYPE
{
	ANP_HIGH,
	ANP_HIGH_BT,
	ANP_PASS,
	ANP_TRUCK,
	PLAN_OFFSET
};

/** Источник данных - параметр внутри кривой*/
class VCurveParameterData : public VXOrderedDataProvider
{
private:	
	/** Указатель на данные*/
	VDataSetPtr data_set;
	/** Индекс кривой*/
	size_t curve_index;
	/** Данные*/
	std::vector<double> ydata;	
	/** Индекс начала кривой*/
	size_t start_index;
public:
	/** Конструктор*/
	VCurveParameterData(VDataSetPtr in_data_set, size_t in_curve_index, CURVE_PARAM_TYPE in_type, 
		PARAMETER_TYPE in_mark_type);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();		
};
typedef boost::shared_ptr<VCurveParameterData> VCurveParameterDataPtr;


/** Источник данных объекты на главном пути*/
class VMainWayObjectData : public VXOrderedDataProvider
{
private:
	/** Указатель на датасет*/
	VDataSetPtr data_set;
	/** Указатель на БД*/
	VIBDatabasePtr database;
	/** Указатель на транзакцию*/
	VIBTransactionPtr transaction;
	/** Запрос*/
	std::wstring query;
	/** Обработчик данных*/
	VIBDataHandlerParameter handler;
	
public:

	/** Конструктор*/
	VMainWayObjectData(VIBDatabasePtr in_database, VIBTransactionPtr transction, const std::wstring& in_query, 
		VDataSetPtr in_data_set);
	/** Обновляет данные*/
	void RefreshData();
	/** Возвращает размер данных*/
	size_t GetCount() const;
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает флаги (ID метки)*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VMainWayObjectData> VMainWayObjectDataPtr;


//-------------------------------------- Для станционных путей -------------------------------------------


/** Источник данных - профиль на станции*/
class VStationProfileData : public VXOrderedDataProvider
{
private:
	
	/*
	FixMe!!!
	1. Использовать VGraphPointData
	2. Добавить доп. точки.
	*/

	/// Данные
	std::vector<point_t> data;				

public:

	/** Конструктор*/
	VStationProfileData(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, 
		VDataSetPtr in_data_set);	
	/** Возвращает размер данных*/
	size_t GetCount() const;
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();		
};
typedef boost::shared_ptr<VStationProfileData> VStationProfileDataPtr;


/** Источник данных метки оператора на станции*/
class VStationOperatorMarkData : public VXOrderedDataProvider
{
private:
	/** Указатель на датасет*/
	VDataSetPtr data_set;
	/** Указатель на БД*/
	VIBDatabasePtr database;
	/** Транзакция*/
	VIBTransactionPtr transaction;
	/** Запрос ID пути*/
	std::wstring trail_id;
	/** Обработчик данных*/
	VIBDataHandlerParameter handler;	
	
public:

	/** Конструктор*/
	VStationOperatorMarkData(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, 
		VDataSetPtr in_data_set, const std::wstring& in_trail_id);
	/** Обновляет данные*/
	void RefreshData();
	/** Возвращает размер данных*/
	size_t GetCount() const;
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает флаги (ID метки)*/
	uint64_t GetFlags(size_t in_index) const;
	/** Возвращает true если точку можно менять. По умолчанию можно менять любую точку*/
	bool CanChange(size_t in_index) const;
	/** Добавляет метку*/
	void AddXY(size_t in_index, double in_x, double in_y, double in_size_x=0, double in_size_y=0,
		const std::wstring& in_label=L"", uint64_t in_flags=0);
	/** Изменяет метку*/
	void SetXY(size_t in_index, double in_x, double in_y);
	/** Удаляет метку*/	
	void DeleteItem(size_t in_index);
	/** Выделяет точку*/
	void SelectPoint(size_t in_index);
	/** Возвращает выделенную точку*/
	size_t GetSelectedPoint() const;
	/** Снимает выделение*/
	void DeselectPoint();
};
typedef boost::shared_ptr<VStationOperatorMarkData> VStationOperatorMarkDataPtr;

/** Источник данных объекты на станционном пути*/
class VStationWayObjectData : public VXOrderedDataProvider
{
private:
	/** Указатель на датасет*/
	VDataSetPtr data_set;
	/** Указатель на БД*/
	VIBDatabasePtr database;
	/** Указатель на транзакцию*/
	VIBTransactionPtr transaction;
	/** Запрос ID пути*/
	std::wstring trail_id;
	/** Обработчик данных*/
	VIBDataHandlerParameter handler;
	/// Флаг, включает отображение объктов за границей пути
	bool show_all;

public:

	/** Конструктор*/
	VStationWayObjectData(VIBDatabasePtr in_database, VIBTransactionPtr transction, VDataSetPtr in_data_set, bool in_show_all);
	/** Обновляет данные*/
	void RefreshData();
	/** Возвращает размер данных*/
	size_t GetCount() const;
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;
	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает флаги (ID метки)*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VStationWayObjectData> VStationWayObjectDataPtr;


/** Источник данных километровые столбы на станционном пути*/
class VStationWayKmData : public VXOrderedDataProvider
{
private:
	/** Указатель на датасет*/
	VDataSetPtr data_set;
	/** Указатель на БД*/
	VIBDatabasePtr database;
	/** Указатель на транзакцию*/
	VIBTransactionPtr transaction;
	/** Запрос ID пути*/
	std::wstring trail_id;
	/** Обработчик данных*/
	VIBDataHandlerParameter handler;
	
public:

	/** Конструктор*/
	VStationWayKmData(VIBDatabasePtr in_database, VIBTransactionPtr transction, VDataSetPtr in_data_set);
	/** Обновляет данные*/
	void RefreshData();
	/** Возвращает размер данных*/
	size_t GetCount() const;
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;		
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает флаги (ID метки)*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VStationWayKmData> VStationWayKmDataPtr;


///Контейнер для предустановленных запросов
class VPredefinedQueries
{
private:	

	/// Список запросов
	std::map<std::wstring, std::wstring> queries;
	typedef std::pair<std::wstring, std::wstring> query_t;
	typedef std::map<std::wstring, std::wstring>::const_iterator query_itr;
	/// Пустой запрос
	std::wstring zero;

public:
	///Конструктор
	VPredefinedQueries();
	///Возвращает команду по описанию	
	const std::wstring& GetCommand(const std::wstring& in_description) const;
};
static const VPredefinedQueries PredefinedQueries;

/** Разметка прямых*/
class VLineMarkData : public VXOrderedDataProvider
{
private:	

	/** Указатель на источник данных в файле*/
	VDataSetPtr data_set;	
	/** Перемещение метки*/
	bool drag;
	/** Индекс перемещаемой метки*/
	size_t drag_index;	
public:
	/** Конструктор*/
	VLineMarkData(VDataSetPtr in_data_set);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;			
	/** Устанавливает значение X*/
	void SetXY(size_t in_index, double in_x, double in_y);	
	/** Добавляет точку*/
	void AddXY(size_t in_index, double in_x, double in_y, double in_size_x=0, double in_size_y=0, 
		const std::wstring& in_label=L"", uint64_t in_flags=0);
	/** Удаляет точку*/
	void DeleteItem(size_t in_index);	
	/** Удаляет все метки*/
	void DeleteAll();
	/** Автоматическая разметка*/
	void Automark();
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VLineMarkData> VLineMarkDataPtr;

/** Отступления в реальном времени*/
class VRTOverstepData : public VDataProvider
{
private:		
	/** Список отступлений*/
	std::vector<VBasicOverstepPtr> oversteps;	
public:
	/** Конструктор*/
	VRTOverstepData();
	/** Добавляет данные*/
	void Put(const VBasicOverstepPtr in_overstep);
	/** Удаляет данные*/
	void Clear();
	/** Возвращает размер данных*/
	size_t GetCount() const;
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;
	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const;	
	/** Возвращает размер по оси Y*/
	double GetSizeY(size_t in_index) const;
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
	/** Возвращает флаги (степень)*/
	uint64_t GetFlags(size_t in_index) const;
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VRTOverstepData> VRTOverstepDataPtr;

/// Событие начала объекта
#define START_EVENT 1
/// Событие конец объекта
#define END_EVENT -1
/// Событие - точечный объект
#define SING_EVENT 0
/// Событие - комментарий
#define COMMENT_EVENT 2


/** События*/
class VRTEventInfoData : public VDataProvider
{
private:		
	
	struct VRTEvent
	{
		double x;
		std::wstring coord;
		std::wstring type;
		std::wstring degree;
		std::wstring value;
		std::wstring length;
		std::wstring count;
		std::wstring restriction;
		bool has_restriction;
		int flag;
		bool drawn;
		bool is_draw_frame;
		bool is_bold_font;

		VRTEvent(const VBasicOverstepPtr& in_overstep, bool in_is_draw_frame)
		{
			x=in_overstep->GetCoord();
			coord=in_overstep->GetCoordString();
			type=in_overstep->GetTypeString();
			degree=in_overstep->GetDegreeString();
			value=in_overstep->GetValueString();
			length=in_overstep->GetLengthString();
			if (!in_overstep->IsExcluded())
				count=DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515) ? in_overstep->GetAmountString() : in_overstep->GetPenaltyString();
			else
				count=L"";
			restriction=boost::str(boost::wformat(L"%s %s") % in_overstep->GetRestrictionString() 
				% in_overstep->GetComment());
			has_restriction=in_overstep->HasRestriction();
			flag=SING_EVENT;
			drawn=false;
			is_draw_frame=in_is_draw_frame;
			is_bold_font=!in_overstep->IsExcluded() && (in_overstep->HasRestriction() || in_overstep->GetDegree()==DEGREE3 || in_overstep->CloseToUpperBound());
		};
		VRTEvent(double in_x, const std::wstring& in_coord, const std::wstring& in_description, int in_flag)
		{
			x=in_x;
			coord=in_coord;
			type=in_description;
			degree=L"";
			value=L"";
			length=L"";
			count=L"";
			restriction=L"";
			has_restriction=false;
			flag=in_flag;
			drawn=false;
			is_draw_frame=true;
			is_bold_font=false;
		};
		bool operator<(const VRTEvent& in_event) const {return x<in_event.x || x==in_event.x && flag<in_event.flag;};
	};

	/** Список событий*/
	std::vector<VRTEvent> data;	
public:	
	/** Конструктор*/
	VRTEventInfoData();
	/** Добавляет отступление*/
	void Put(const VBasicOverstepPtr in_overstep);
	/** Добавляет событие*/
	void Put(double in_x, const std::wstring& in_coord, const std::wstring& in_description, int in_flag);
	/** Сортирует события*/
	void Sort();
	/** Удаляет данные*/
	void Clear();
	/// Помечает событие как отображенное
	void SetDrawn(size_t in_index);
	/** Удаляет  отображенные события*/
	void ClearDrawn();

	std::wstring GetCoord(size_t in_index) const;
	std::wstring GetType(size_t in_index) const;
	std::wstring GetDegree(size_t in_index) const;
	std::wstring GetValue(size_t in_index) const;
	std::wstring GetLength(size_t in_index) const;
	std::wstring GetAmount(size_t in_index) const;
	std::wstring GetRestriction(size_t in_index) const;
	bool HasRestriction(size_t in_index) const;
	bool IsDrawFrame(size_t in_index) const;
	bool IsBoldFont(size_t in_index) const;
	/** Возвращает размер данных*/
	size_t GetCount() const;
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает флаги*/
	uint64_t GetFlags(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VRTEventInfoData> VRTEventInfoDataPtr;


/** Коэффициент корреляции кривизны в двух файлах*/
class VCorrelationInfo : public VTextProvider
{
	private:
		/** Источник данных 1*/
		VDataSetPtr data_set1;
		/** Источник данных 2*/
		VDataSetPtr data_set2;
		/** Параметр*/
		PARAMETER_TYPE parameter_type;

		mutable double start1;
		mutable double start2;
		mutable double end1;
		mutable double end2;
		mutable std::wstring text;

		void Update() const
		{			
			if (start1!=data_set1->GetAbsCoord().GetItem(0) ||
				start2!=data_set2->GetAbsCoord().GetItem(0) ||
				end1!=data_set1->GetAbsCoord().GetLastItem() ||
				end2!=data_set2->GetAbsCoord().GetLastItem())
			{
				double correlation;
				correlation=corr(data_set1->GetParameter(parameter_type), data_set1->GetAbsCoord(), 
					data_set2->GetParameter(parameter_type), data_set2->GetAbsCoord());
				start1=data_set1->GetAbsCoord().GetItem(0);
				start2=data_set2->GetAbsCoord().GetItem(0);
				end1=data_set1->GetAbsCoord().GetLastItem();
				end2=data_set2->GetAbsCoord().GetLastItem();
				text=boost::str(boost::wformat(L"Корреляция: %0.4f") % correlation);
			}			
		};

	public:	
		/** Конструктор*/
		VCorrelationInfo(VDataSetPtr in_data_set1, VDataSetPtr in_data_set2, PARAMETER_TYPE in_parameter_type)
		{
			if (in_data_set1 && in_data_set2)
			{				
				data_set1=in_data_set1;
				data_set2=in_data_set2;
				parameter_type=in_parameter_type;
				data_set1->Load(PRM_ABS_COORD);
				data_set2->Load(PRM_ABS_COORD);
				data_set1->Load(parameter_type);
				data_set2->Load(parameter_type);
				start1=0;
				start2=0;
				end1=0;
				end2=0;

			}
			else
			{
				throw NULL_POINTER;
			}
		};
		/** Возвращает число строк*/
		virtual size_t GetCount() const
		{
			return 1;
		}
		/** Возвращает коэффициент корреляции*/
		virtual const std::wstring& GetItem(size_t in_index) const
		{
			Update();
			return text;
		};
};
typedef boost::shared_ptr<VCorrelationInfo> VCorrelationInfoPtr;

//---------------------------------------------- Источнико данных из aprinfo -------------------------------------

/** Источник данных - участки скорости по приказу*/
class VAprinfoSpeedSectionData : public VXOrderedDataProvider
{
private:	
	/** Список координат*/
	std::vector<double> xdata;	
	/** Список длин участков*/
	std::vector<double> xsize;	
	/** Список меток*/
	std::vector<std::wstring> labels;
public:
	/** Конcтруктор*/
	VAprinfoSpeedSectionData(const VAprinfo& in_aprinfo, double in_start, double in_end);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;

	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const;	
	/** Возвращает размер по оси Y*/
	double GetSizeY(size_t in_index) const;

	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();
};
typedef boost::shared_ptr<VAprinfoSpeedSectionData> VAprinfoSpeedSectionDataPtr;

/** Поиск объекта в Aprinfo*/
template<class T> struct find_object
{
	double coord;	
	find_object(double in_coord) : coord(in_coord){};
	bool operator()(const T& in_object) const
	{
		return in_object.base<=coord && in_object.end>=coord;
	};
};

/** Источник данных - крестовины стрелок*/
class VSwitchData : public VXOrderedDataProvider
{
private:	
	/** Список стрелок*/
	std::deque<VAprinfoSwitchRecord> data;
public:
	/** Конcтруктор*/
	VSwitchData();
	/// Добавляет стрелку
	void Put(const VAprinfoSwitchRecord& in_data);
	/// Очищает список
	void Clear();
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
	/** Возвращает флаги*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VSwitchData> VSwitchDataPtr;


/** Источник данных - участки стрелок*/
class VSwitchSectionsData : public VXOrderedDataProvider
{
private:	
	/** Список стрелок*/
	std::deque<VAprinfoSwitchRecord> data;
public:
	/** Конcтруктор*/
	VSwitchSectionsData();
	/// Добавляет стрелку
	void Put(const VAprinfoSwitchRecord& in_data);
	/// Очищает список
	void Clear();
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const;
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
	/** Возвращает флаги*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VSwitchSectionsData> VSwitchSectionsDataPtr;


/** Источник данных - мосты из aprinfo*/
class VAprinfoBridgeData : public VXOrderedDataProvider
{
private:	
	/** Список стрелок*/
	std::vector<VAprinfoIndexRecord> data;
public:
	/** Конcтруктор*/
	VAprinfoBridgeData(const VAprinfo& in_aprinfo, bool in_include_areas);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const;	
	/** Возвращает размер по оси Y*/
	double GetSizeY(size_t in_index) const;
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoBridgeData> VAprinfoBridgeDataPtr;


/** Источник данных - уравнительные приборы из aprinfo*/
class VAprinfoChamferedJointData : public VXOrderedDataProvider
{
private:	
	/** Список стрелок*/
	std::vector<VAprinfoObjectRecord> data;
public:
	/** Конcтруктор*/
	VAprinfoChamferedJointData(const VAprinfo& in_aprinfo);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const;	
	/** Возвращает размер по оси Y*/
	double GetSizeY(size_t in_index) const;
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoChamferedJointData> VAprinfoChamferedJointDataPtr;


/** Источник данных - тоннели из aprinfo*/
class VAprinfoTunnelData : public VXOrderedDataProvider
{
private:	
	/** Список стрелок*/
	std::vector<VAprinfoIndexRecord> data;
public:
	/** Конcтруктор*/
	VAprinfoTunnelData(const VAprinfo& in_aprinfo, bool in_include_areas);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const;	
	/** Возвращает размер по оси Y*/
	double GetSizeY(size_t in_index) const;
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoTunnelData> VAprinfoTunnelDataPtr;


/** Источник данных - переезды из aprinfo*/
class VAprinfoCrossingData : public VXOrderedDataProvider
{
private:	
	/** Список стрелок*/
	std::vector<VAprinfoCrossing> data;
public:
	/** Конcтруктор*/
	VAprinfoCrossingData(const VAprinfo& in_aprinfo);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const;	
	/** Возвращает размер по оси Y*/
	double GetSizeY(size_t in_index) const;
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoCrossingData> VAprinfoCrossingDataPtr;


/** Источник данных - тип шпал из aprinfo*/
class VAprinfoSpalaData : public VXOrderedDataProvider
{
private:	
	/** Список стрелок*/
	std::vector<VAprinfoIndexRecord> data;
public:
	/** Конcтруктор*/
	VAprinfoSpalaData(const VAprinfo& in_aprinfo);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const;	
	/** Возвращает размер по оси Y*/
	double GetSizeY(size_t in_index) const;
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
	/** Возвращает флаги (ID метки)*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VAprinfoSpalaData> VAprinfoSpalaDataPtr;


/** Источник данных - рихтовочная нить на прямых из aprinfo*/
class VAprinfoStraightSideData : public VXOrderedDataProvider
{
private:	
	/** Список стрелок*/
	std::vector<VAprinfoIndexRecord> data;
public:
	/** Конcтруктор*/
	VAprinfoStraightSideData(const VAprinfo& in_aprinfo, bool is_passport);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;	
	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const;	
	/** Возвращает размер по оси Y*/
	double GetSizeY(size_t in_index) const;
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;	
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
	/** Возвращает флаги (ID метки)*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VAprinfoStraightSideData> VAprinfoStraightSideDataPtr;


/** Источник данных - норма возвышения (нулевая линия) из aprinfo*/
class VAprinfoNormaLevelData : public VXOrderedDataProvider
{
private:		
	/** Данные*/
	std::vector<double> xdata;
	std::vector<double> ydata;
public:
	/** Конcтруктор*/
	VAprinfoNormaLevelData(const VAprinfo& in_aprinfo);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;			
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoNormaLevelData> VAprinfoNormaLevelDataPtr;


/** Источник данных - нулевая линия рихтовки из aprinfo*/
class VAprinfoNormaRichData : public VXOrderedDataProvider
{
private:		
	/** Данные*/
	std::vector<double> xdata;
	std::vector<double> ydata;
public:
	/** Конcтруктор*/
	VAprinfoNormaRichData(const VAprinfo& in_aprinfo);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;			
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoNormaRichData> VAprinfoNormaRichDataPtr;

/** Источник данных - нулевая линия ШК из aprinfo*/
class VAprinfoNormaWidthData : public VXOrderedDataProvider
{
private:		
	/** Данные*/
	std::vector<double> xdata;
	std::vector<double> ydata;
public:
	/** Конcтруктор*/
	VAprinfoNormaWidthData(const VAprinfo& in_aprinfo);
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;			
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoNormaWidthData> VAprinfoNormaWidthDataPtr;


/** Источник данных - пополняемый произвольный массив*/
class VDequeData : public VDataProvider
{
private:		
	/** Данные*/	
	std::deque<VGraphPointData> data;
	/** Минимум Y*/
	double min_y;
	/** Максимум Y*/
	double max_y;
public:
	/** Конcтруктор*/
	VDequeData();
	/** Добавление кадра*/
	void Put(double in_x, double in_y, uint64_t in_flags);
	/** Добавление кадра*/
	void Put(double in_x, double in_size_x, double in_y, double in_size_y, const std::wstring& in_label, uint64_t in_flags);
	/** Удалеение данных*/
	void Clear();
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;		
	/** Возвращает размер по оси X*/
	double GetSizeX(size_t in_index) const;	
	/** Возвращает размер по оси Y*/
	double GetSizeY(size_t in_index) const;		
	/** Возвращает метку*/
	std::wstring GetLabel(size_t in_index) const;
	/** Возвращает флаги*/
	uint64_t GetFlags(size_t in_index) const;
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VDequeData> VDequeDataPtr;


/** Источник данных - произвольный массив*/
class VVectorData : public VDataProvider
{
private:		
	/** Данные*/
	std::vector<double> xdata;
	std::vector<double> ydata;
	std::vector<uint64_t> flags;
public:
	/** Конcтруктор*/
	VVectorData();
	/** Устанавливает размер*/ 
	void SetSize(size_t in_size);
	/** Добавление кадра*/
	void Set(size_t in_index, double in_x, double in_y, uint64_t in_flags);
	/** Удалеение данных*/
	void Clear();
	/** Возвращает размер данных*/
	size_t GetCount() const;	
	/** Возвращает координату по оси X*/
	double GetX(size_t in_index) const;	
	/** Возвращает координату по оси Y*/
	double GetY(size_t in_index) const;		
	/** Возвращает флаги*/
	uint64_t GetFlags(size_t in_index) const;
	/** Возвращает минимальное значение по Y*/
	double GetMinY();
	/** Возвращает максимальное значение по Y*/
	double GetMaxY();
	/** Возвращает минимальное значение по X*/
	double GetMinX();
	/** Возвращает максимальное значение по X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VVectorData> VVectorDataPtr;


/** Произвольный текст*/
class VUserTextProvider : public VTextProvider
{
private:
	std::vector<std::wstring> items;
public:
	/** Возвращает число строк*/
	virtual size_t GetCount() const;
	/** Возвращает строку*/
	virtual const std::wstring& GetItem(size_t in_index) const;
	/** Добавляет строку*/
	void AddItem(const std::wstring& in_line);
	/** Удаляет все строки*/ 
	void Clear();
};
typedef boost::shared_ptr<VUserTextProvider> VUserTextProviderPtr;


/// Отступления
class VJaggyProfileOverstepData : public VXOrderedDataProvider
{
private:
	/// Указатель на источник данных в файле
	VDataSetPtr data_set;	
public:
	/// Конструктор
	VJaggyProfileOverstepData(VDataSetPtr in_data_set);
	/// Возвращает размер данных
	size_t GetCount() const;	
	/// Возвращает координату по оси X
	double GetX(size_t in_index) const;	
	/// Возвращает размер по оси X
	double GetSizeX(size_t in_index) const;
	/// Возвращает координату по оси Y
	double GetY(size_t in_index) const;		
	/// Возвращает размер по оси Y
	double GetSizeY(size_t in_index) const;	
	/// Возвращает минимальное значение по Y
	double GetMinY();
	/// Возвращает максимальное значение по Y
	double GetMaxY();
	/// Возвращает минимальное значение по X
	double GetMinX();
	/// Возвращает максимальное значение по X
	double GetMaxX();
	/// Возвращает метку
	std::wstring GetLabel(size_t in_index) const;
};

/// Отступления
class VJaggyPlanOverstepData : public VXOrderedDataProvider
{
private:
	/// Указатель на источник данных в файле
	VDataSetPtr data_set;	
public:
	/// Конструктор
	VJaggyPlanOverstepData(VDataSetPtr in_data_set);
	/// Возвращает размер данных
	size_t GetCount() const;	
	/// Возвращает координату по оси X
	double GetX(size_t in_index) const;	
	/// Возвращает размер по оси X
	double GetSizeX(size_t in_index) const;
	/// Возвращает координату по оси Y
	double GetY(size_t in_index) const;		
	/// Возвращает размер по оси Y
	double GetSizeY(size_t in_index) const;	
	/// Возвращает минимальное значение по Y
	double GetMinY();
	/// Возвращает максимальное значение по Y
	double GetMaxY();
	/// Возвращает минимальное значение по X
	double GetMinX();
	/// Возвращает максимальное значение по X
	double GetMaxX();
	/// Возвращает метку
	std::wstring GetLabel(size_t in_index) const;
};

/// Отступления
class VDiffWidthFrontOverstepData : public VXOrderedDataProvider
{
private:
	/// Указатель на источник данных в файле
	VDataSetPtr data_set;	
public:
	/// Конструктор
	VDiffWidthFrontOverstepData(VDataSetPtr in_data_set);
	/// Возвращает размер данных
	size_t GetCount() const;	
	/// Возвращает координату по оси X
	double GetX(size_t in_index) const;	
	/// Возвращает размер по оси X
	double GetSizeX(size_t in_index) const;
	/// Возвращает координату по оси Y
	double GetY(size_t in_index) const;		
	/// Возвращает размер по оси Y
	double GetSizeY(size_t in_index) const;	
	/// Возвращает минимальное значение по Y
	double GetMinY();
	/// Возвращает максимальное значение по Y
	double GetMaxY();
	/// Возвращает минимальное значение по X
	double GetMinX();
	/// Возвращает максимальное значение по X
	double GetMaxX();
	/// Возвращает метку
	std::wstring GetLabel(size_t in_index) const;
};

/// Отступления
class VDiffWidthBackOverstepData : public VXOrderedDataProvider
{
private:
	/// Указатель на источник данных в файле
	VDataSetPtr data_set;	
public:
	/// Конструктор
	VDiffWidthBackOverstepData(VDataSetPtr in_data_set);
	/// Возвращает размер данных
	size_t GetCount() const;	
	/// Возвращает координату по оси X
	double GetX(size_t in_index) const;	
	/// Возвращает размер по оси X
	double GetSizeX(size_t in_index) const;
	/// Возвращает координату по оси Y
	double GetY(size_t in_index) const;		
	/// Возвращает размер по оси Y
	double GetSizeY(size_t in_index) const;	
	/// Возвращает минимальное значение по Y
	double GetMinY();
	/// Возвращает максимальное значение по Y
	double GetMaxY();
	/// Возвращает минимальное значение по X
	double GetMinX();
	/// Возвращает максимальное значение по X
	double GetMaxX();
	/// Возвращает метку
	std::wstring GetLabel(size_t in_index) const;
};

/// Отступления
class VCantingOverstepData : public VXOrderedDataProvider
{
private:
	/// Указатель на источник данных в файле
	VDataSetPtr data_set;
	side_t side;
public:
	/// Конструктор
	VCantingOverstepData(VDataSetPtr in_data_set, side_t in_side);
	/// Возвращает размер данных
	size_t GetCount() const;	
	/// Возвращает координату по оси X
	double GetX(size_t in_index) const;	
	/// Возвращает размер по оси X
	double GetSizeX(size_t in_index) const;
	/// Возвращает координату по оси Y
	double GetY(size_t in_index) const;		
	/// Возвращает размер по оси Y
	double GetSizeY(size_t in_index) const;	
	/// Возвращает минимальное значение по Y
	double GetMinY();
	/// Возвращает максимальное значение по Y
	double GetMaxY();
	/// Возвращает минимальное значение по X
	double GetMinX();
	/// Возвращает максимальное значение по X
	double GetMaxX();
	/// Возвращает метку
	std::wstring GetLabel(size_t in_index) const;
};


/// Отступления
class VExtWearOverstepData : public VXOrderedDataProvider
{
private:
	/// Указатель на источник данных в файле
	VDataSetPtr data_set;
	side_t side;
public:
	/// Конструктор
	VExtWearOverstepData(VDataSetPtr in_data_set, side_t in_side);
	/// Возвращает размер данных
	size_t GetCount() const;	
	/// Возвращает координату по оси X
	double GetX(size_t in_index) const;	
	/// Возвращает размер по оси X
	double GetSizeX(size_t in_index) const;
	/// Возвращает координату по оси Y
	double GetY(size_t in_index) const;		
	/// Возвращает размер по оси Y
	double GetSizeY(size_t in_index) const;	
	/// Возвращает минимальное значение по Y
	double GetMinY();
	/// Возвращает максимальное значение по Y
	double GetMaxY();
	/// Возвращает минимальное значение по X
	double GetMinX();
	/// Возвращает максимальное значение по X
	double GetMaxX();
	/// Возвращает метку
	std::wstring GetLabel(size_t in_index) const;
};
