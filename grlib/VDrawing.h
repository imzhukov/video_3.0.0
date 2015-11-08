#pragma once

//Необходимо для того, чтобы использовать определения типа M_PI из math.h
#define _USE_MATH_DEFINES
#include <math.h>

/** stdlib*/
#include <map>
#include <vector>
#include <set>
#include <string>
#include <algorithm>


/** Cairo*/
#include <cairo.h>
#ifdef _WIN32
	#include <windows.h>
	#include <cairo-win32.h>
#endif
#include <cairo-svg.h>
#include <cairo-pdf.h>
#include <cairo-ps.h>

#include "c4stdint.h"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include "VException.h"
#include "VLog.h"
#include "calclib.h"
#include "strlib.h"
#include "aui.h"
#include "options.h"
#include "cairo_ex.h"
#include "interval.h"

using namespace boost;


#define DPI_AUTO 0
#define DPI_DEFAULT_PRINTER 300
#define DPI_DEFAULT_SCREEN 96
#define HEIGHT_A4 20.15
#define WIDTH_A4 28.7

/** Определение типа XPM*/
typedef char* xpm_t;

//Константы FixMe!!! перенести в специальный файл
//FixMe!!! использовать заглавные буквы

/** Антиалиасинг*/
static const cairo_antialias_t antialias=CAIRO_ANTIALIAS_NONE;
/** Антиалиасинг*/
static const cairo_antialias_t font_antialias=CAIRO_ANTIALIAS_NONE;
/** Толщина линии по умолчанию*/
static const double default_line_width=0.2;
/** Минимальная толщина линии*/
static const double min_line_width=0.0;
/** Толстая линия*/
static const double heavy_line_width=0.4;
/** Шаг изменеия размера шрифта*/
static  const double font_size_step=0.025;
/** Отступ текста (см)*/
static const double text_space=0.1;
/** Отступ слева*/
static const double default_left_margin=15;
/** Отступ сверху*/
static const double default_top_margin=15;
/** Отступ справа*/
static const double default_right_margin=0;
/** Отступ снизу*/
static const double default_bottom_margin=5;


/// Переводит дюймы в сантиметры
static double inch2cm(double in_value_inch)
{
	return in_value_inch/2.54;
};

/** Текущее устройство*/
class VDrawingDevice
{
public:
	/** Возвращает размер по X*/
	double GetSizeX() const;
	/** Возвращает размер по Y*/
	double GetSizeY() const;	
	/** Пересчитывает координаты из изображения на устройство по оси X*/
	double DrawingToDeviceX(double in_value) const;
	/** Пересчитывает координаты из устройства в изображение по оси X*/
	double DeviceToDrawingX(double in_value) const;
	/** Пересчитывает координаты из изображения на устройство по оси Y*/
	double DrawingToDeviceY(double in_value) const;
	/** Пересчитывает координаты из устройства в изображение по оси Y*/
	double DeviceToDrawingY(double in_value) const;
protected:
	/** Размер по оси X*/
	double size_x;
	/** Размер по оси Н*/
	double size_y;
	/** Разрешение по X*/
	double aspect_x;
	/** Разрешение по Y*/
	double aspect_y;
};
typedef boost::shared_ptr<VDrawingDevice> VDrawingDevicePtr;

/** Устройство Win32*/
class VDrawingDeviceWin32  : public VDrawingDevice
{
public:
	/** Конструктор*/
	VDrawingDeviceWin32(HDC in_hdc);
};

/** Бумага*/
class VDrawingDevicePaper : public VDrawingDevice
{
public:
	/** Конструктор*/
	VDrawingDevicePaper(double in_ppi_x, double in_ppi_y, double in_size_mm_x, double in_size_mm_y);
};


/** Экран*/
class VDrawingDeviceDisplay : public VDrawingDevice
{
public:
	/** Конструктор*/
	VDrawingDeviceDisplay(double in_ppi_x, double in_ppi_y, double in_size_x, double in_size_y);
};

//Коды событий посылемых объектам
#define EVENT_MOUSE_LEFT_DOWN 1
#define EVENT_MOUSE_LEFT_UP 2
#define EVENT_MOUSE_MIDDLE_DOWN 3
#define EVENT_MOUSE_MIDDLE_UP 4
#define EVENT_MOUSE_RIGHT_DOWN 5
#define EVENT_MOUSE_RIGHT_UP 6
#define EVENT_MOUSE_MOTION 7
#define EVENT_MOUSE_WHEEL 8
#define EVENT_KEY_DOWN 9
#define EVENT_KEY_UP 10

//Коды событий посылаемых источникам данных
#define EVENT_DATA_ACTIVATE_POINT 101
#define EVENT_DATA_START_DRAG 102
#define EVENT_DATA_DRAG 103
#define EVENT_DATA_DROP 104
#define EVENT_DATA_DELETE_POINT 105


/** Контейнер горячих клавиш*/
class VHotKey
{
private:
	/** Код клавиши*/
	int key;
	/** Модификатор - alt*/
	bool alt;
	/** Модификатор - ctrl*/
	bool ctrl;
	/** Модификатор - shift*/
	bool shift;
public: 
	/** Конструктор*/
	VHotKey() : key(0), alt(false), ctrl(false), shift(false) {};
	/** Конструктор*/
	VHotKey(int in_key) : 
	  key(in_key), alt(false), ctrl(false), shift(false) {};
	/** Конструктор*/
	VHotKey(int in_key, bool in_alt, bool in_ctrl, bool in_shift) : 
	  key(in_key), alt(in_alt), ctrl(in_ctrl), shift(in_shift) {};
	/** Конструктор*/
	VHotKey(const std::wstring& in_string);
	/** Возвращает код*/
	int GetCode() const {return key;};
	/** Возвращает признак Alt*/
	bool GetAlt() const {return alt;};
	/** Возвращает признак Ctrl*/
	bool GetCtrl() const {return ctrl;};
	/** Возвращает признак Shift*/
	bool GetShift() const {return shift;};
	/** Оператор ==*/ //FixME!!! для shift,ctrl,alt нужно проверять только код клавиши
	bool operator==(const VHotKey& in_right) const {return key==in_right.key && alt==in_right.alt && 
		ctrl==in_right.ctrl && shift==in_right.shift;};
	/** Опрератор <*/
	bool operator<(const VHotKey& in_right) const
	{
		return key<in_right.key;
	};
	/** Возвращает строку описания*/
	std::wstring GetDescription()
	{
		std::wstring result=L"";
		if (alt)
			result+=L"ALT+";
		if (ctrl)
			result+=L"CTRL+";
		if (shift)
			result+=L"SHIFT+";
		result+=boost::str(boost::wformat(L"%c") % (char)key);
		return result;
	}
};

//Определяем некоторые горячие клавиши

//SPACE
#define HK_SPACE VHotKey(32)
//Tab
#define HK_TAB VHotKey(9)
//Ctrl+Q
#define HK_CTRL_Q VHotKey(81, false, true, false)
//Ctrl+O
#define HK_CTRL_O VHotKey(79, false, true, false)
//Ctrl+W
#define HK_CTRL_W VHotKey(87, false, true, false)
//Ctrl+V
#define HK_CTRL_V VHotKey(86, false, true, false)
//Esc
#define HK_ESC VHotKey(27)
//Delete
#define HK_DELETE VHotKey(127)
//Shift
#define HK_SHIFT_DOWN VHotKey(306, false, false, true)
#define HK_SHIFT_UP VHotKey(306, false, false, false)
//+
#define HK_PLUS VHotKey(43, false, false, false)
//-
#define HK_MINUS VHotKey(45, false, false, false)
//Ctrl++
#define HK_CTRL_PLUS VHotKey(43, false, true, false)
//Ctrl+-
#define HK_CTRL_MINUS VHotKey(45, false, true, false)
//Alt++
#define HK_ALT_PLUS VHotKey(43, true, false, false)
//Alt+-
#define HK_ALT_MINUS VHotKey(45, true, false, false)
//numpad +
#define HK_NUM_PLUS VHotKey(388, false, false, false)
//numpad -
#define HK_NUM_MINUS VHotKey(390, false, false, false)
//Ctrl+ numpad +
#define HK_CTRL_NUM_PLUS VHotKey(388, false, true, false)
//Ctrl+ numapd -
#define HK_CTRL_NUM_MINUS VHotKey(390, false, true, false)
//z
#define HK_Z VHotKey(90, false, false, false)
//x
#define HK_X VHotKey(88, false, false, false)
//Up
#define HK_UP VHotKey(315, false, false, false)
//Down
#define HK_DOWN VHotKey(317, false, false, false)
//Left arrow
#define HK_CTRL_LEFT_ARROW VHotKey(314,false,true,false)
//Right arrow
#define HK_CTRL_RIGHT_ARROW VHotKey(316,false,true,false)
//Left arrow
#define HK_CTRL_UP_ARROW VHotKey(315,false,true,false)
//Right arrow
#define HK_CTRL_DOWN_ARROW VHotKey(317,false,true,false)


/** Событие*/
class VEvent
{
protected:
	/** Код события*/
	uint32_t code;
	/** Признак того что событие обработано*/
	bool handled; 	
public:	
	/** Конструктор*/
	VEvent(uint32_t in_code) : code(in_code), handled(false){};
	/** Деструктор*/
	virtual ~VEvent(){};
	/** Возвращает код*/
	virtual uint32_t GetCode() const {return code;};
	/** Возвращает признак того, что событие обработано*/
	virtual bool GetHandled() const {return handled;};
	/** Устанавливает признак того, что событие обработано*/
	virtual void SetHandled(bool in_handled) {handled=in_handled;};	
};
typedef boost::shared_ptr<VEvent> VEventPtr;

/** Интерфейс обработчика собития*/
class VEventHandler
{
public:
	/** Обработка события*/
	virtual bool ProcessEvent(const VEvent& in_event)=0;
};
typedef boost::shared_ptr<VEventHandler> VEventHandlerPtr;

/** Событие от клавиатуры*/
class VKeyEvent : public VEvent
{
private:
	/** Код клавиши*/
	VHotKey hot_key;	
public:
	/** Конструктор*/
	VKeyEvent(uint32_t in_code, const VHotKey& in_hot_key) : VEvent(in_code), hot_key(in_hot_key){};
	/** Возвращает код клавиши*/	
	const VHotKey& GetKey() const {return hot_key;};
};
typedef boost::shared_ptr<VKeyEvent> VKeyEventPtr;

/** Событие от мыши*/
class VMouseEvent : public VEvent
{
private:
	/** Координата X мыши в момент события*/
	double x;
	/** Координата Y мыши в момент события*/
	double y;
public:
	/** Конструктор*/
	VMouseEvent(uint32_t in_code, double in_x, double in_y) : VEvent(in_code), x(in_x), y(in_y){};
	/** Возвращает кординату X*/
	double GetX() const {return x;};
	/** Возвращает кординату Y*/
	double GetY() const {return y;};
};
typedef boost::shared_ptr<VMouseEvent> VMouseEventPtr;

/** Событие от колеса мыши*/
class VMouseWheelEvent : public VEvent
{
private:
	/** Величина поворота колеса мыши*/
	int r;	
public:
	/** Конструктор*/
	VMouseWheelEvent(uint32_t in_code, int in_r) : VEvent(in_code), r(in_r){};
	/** Возвращает величину поворота*/
	double GetRotation() const {return r;};	
};
typedef boost::shared_ptr<VMouseWheelEvent> VMouseWheelEventPtr;

/** Действие с данными*/
class VAction : public VEventHandler
{
protected:
	/** Имя - описание действия*/
	std::wstring name;
	/** Код горячей клавиши*/
	std::set<VHotKey> hot_keys;	
	/** Указатель на XPM в доступном состоянии*/
	xpm_t* enabled_xpm;
	/** Указатель на XPM в недоступном состоянии*/
	xpm_t* disabled_xpm;
public:		
	/** Деструктор*/
	virtual ~VAction(){};
	/** Выполняет действие (метод должен быть релизован в потомках)*/
	virtual void Execute()=0;
	/** Возвращает имя - описание действия*/
	virtual std::wstring& GetName(){return name;};
	/** Устанавливает имя-описание действия*/
	virtual void SetName(const std::wstring& in_name){name=in_name;};
	/** Устанавливает горячую клавишу*/
	virtual void SetHotKey(const VHotKey& in_hot_key) {hot_keys.insert(in_hot_key);};
	/** Возвращает true, если действие доступно*/
	virtual bool Allowed() const{return true;};	
	/** Возвращает признак необходимости запроса подтверждения*/
	virtual bool NeedAccept() const{return false;};
	/** Возвращает строку запроса подтверждения*/
	virtual std::wstring GetAcceptQuestion() const{return L"";};
	/** Устанавливает XPM в доступном состоянии*/
	virtual void SetEnabledXPM(xpm_t* in_xpm){enabled_xpm=in_xpm;};
	/** Устанавливает XPM в недоступном состоянии*/
	virtual void SetDisabledXPM(xpm_t* in_xpm){disabled_xpm=in_xpm;};
	/** Возвращает XPM в доступном состоянии*/
	virtual xpm_t* const GetEnabledXPM(){return enabled_xpm;};
	/** Возвращает XPM в недоступном состоянии*/
	virtual xpm_t* const GetDisabledXPM(){return disabled_xpm;};
	/** Обработка события*/
	virtual bool ProcessEvent(const VEvent& in_event)
	{
		if (Allowed())
		{
			switch (in_event.GetCode())
			{
				case EVENT_KEY_DOWN: 					
				{				
					if (std::find(hot_keys.begin(), hot_keys.end(), ((VKeyEvent&)in_event).GetKey())!=hot_keys.end())
						Execute();
					break;
				}
				case EVENT_KEY_UP:
				{				
					if (std::find(hot_keys.begin(), hot_keys.end(), ((VKeyEvent&)in_event).GetKey())!=hot_keys.end())
						Execute();
					break;
				}
				default:
					break;
			}
			return true;
		}
		return false;
	};	
};
typedef boost::shared_ptr<VAction> VActionPtr;

/** Объект который может быть активным или неактивным*/
class VActiveObject
{
protected:
	/** Признак активности*/
	bool active;
public:
	/** Конструктор*/
	VActiveObject() : active(true){};
	/** Виртуальный деструктор*/
	virtual ~VActiveObject(){};
	/** Устанавливает признак возможности изменения данных*/
	virtual void SetActive(bool in_active) {active=in_active;};
	/** Возвращает признак возможности изменения данных*/
	virtual bool GetActive() const {return active;};
};
typedef boost::shared_ptr<VActiveObject> VActiveObjectPtr;

//Разбор и сбор флагов
static uint32_t low_word(uint64_t in_flags)
{	
	return static_cast<uint32_t>((in_flags & 0xffffffff00000000) >> 32);
};
static uint32_t hi_word(uint64_t in_flags)
{
	return static_cast<uint32_t>(in_flags & 0x00000000ffffffff);
};
static uint64_t compose_flags(uint32_t in_low, uint32_t in_hi)
{	
	uint64_t flags=static_cast<uint64_t>(in_low) << 32 | static_cast<uint64_t>(in_hi);
	return flags;
};

/** Произвольный текст*/
class VTextProvider
{
public:
	/** Возвращает число строк*/
	virtual size_t GetCount() const = 0;
	/** Возвращает строку*/
	virtual const std::wstring& GetItem(size_t in_index) const = 0;
};
typedef boost::shared_ptr<VTextProvider> VTextProviderPtr;

/** Данные графика*/
class VDataProvider : public VEventHandler, public VActiveObject
{
private:
	
	//Копирующий конструктор закрыт!*/
	VDataProvider(VDataProvider &){};
	//Оператор присваивания закрыт!
	VDataProvider & operator=(VDataProvider &){};	

protected:	
		
	/** Выбранная точка*/
	size_t selected_point;
	/** Помеченные точки*/
	std::vector<size_t> marked_points;
	/** Обработчик изменения данных*/
	VActionPtr on_data_changed;	
	/** Легенда*/
	std::wstring legend;
	/** Диапазон по X*/
	pro::interval<double> range_x;
	/** Диапазон по Y*/
	pro::interval<double> range_y;

public:
	/** Конструктор без параметров*/
	VDataProvider() : VActiveObject(), selected_point(BAD_INDEX),
		range_x(0-DBL_MAX, DBL_MAX), range_y(0-DBL_MAX, DBL_MAX) {};
	/** Деструктор*/
	virtual ~VDataProvider(){};	
	/** Обновляет данные*/
	virtual void RefreshData(){};
	/** Устанавливает легенду*/
	virtual void SetLegend(const std::wstring& in_legend) {legend=in_legend;};
	/** Возвращает легенду*/
	virtual const std::wstring& GetLegend() const {return legend;};
	/** Возвращает размер данных*/
	virtual size_t GetCount() const=0;
	/** Возвращает координату по оси X*/
	virtual double GetX(size_t in_index) const=0;
	/** Возвращает координату по оси Y*/
	virtual double GetY(size_t in_index) const=0;
	/** Возвращает размер "точки" по X (имеет смысл для протяженных объектов)*/
	virtual double GetSizeX(size_t in_index) const {return 0;};
	/** Возвращает размер "точки" по Y (имеет смысл для протяженных объектов)*/
	virtual double GetSizeY(size_t in_index) const {return 0;};
	/** Возвращает флаги для точки, флаги определяются в потомках по умолчанию возвращает 0*/
	virtual uint64_t GetFlags(size_t in_index) const {return 0;};
	/** Возвращает указатель на вспомогательную структуру данных по умолчанию NULL*/
	virtual const void * GetUserData(size_t in_index) const {return NULL;};
	/** Возвращает метку*/
	virtual std::wstring GetLabel(size_t in_index) const {return std::wstring(L"");};
	/** Возвращает метку по оси Х*/
	virtual std::wstring GetLabelX(size_t in_index) const {return std::wstring(L"");};
	/** Возвращает метку по оси Y*/
	virtual std::wstring GetLabelY(size_t in_index) const {return std::wstring(L"");};			
	/** Возвращает минимальное значение по Y*/
	virtual double GetMinY() = 0;
	/** Возвращает максимальное значение по Y*/
	virtual double GetMaxY() = 0;
	/** Возвращает минимальное значение по X*/
	virtual double GetMinX() = 0;
	/** Возвращает максимальное значение по X*/
	virtual double GetMaxX() = 0;		
	
	//Редактирование данных
	
	/** Возвращает true если точку можно менять. По умолчанию можно менять любую точку*/
	virtual bool CanChange(size_t in_index) const { return true; };
	/** Устанавливает значение X*/
	virtual void SetXY(size_t in_index, double in_x, double in_y){};	
	/** Устанавливает значения флагов*/
	virtual void SetFlags(size_t in_index, uint64_t in_flags){};
	/** Добавляет точку*/
	virtual void AddXY(size_t in_index, double in_x, double in_y, double in_size_x=0, double in_size_y=0, 
		const std::wstring& in_label=L"", uint64_t in_flags=0){};
	/** Удаляет точку*/
	virtual void DeleteItem(size_t in_index){};
	/** Удаляет группу точек*/
	virtual void DeleteRange(size_t in_from, size_t in_to){};
	/** Удаляет все точки*/
	virtual void DeleteAll(){};
	/** Выделяет точку*/
	virtual void SelectPoint(size_t in_index) {selected_point=in_index;};
	/** Возвращает выделенную точку*/
	virtual size_t GetSelectedPoint() const {return selected_point;};
	/** Снимает выделение*/
	virtual void DeselectPoint() {selected_point=BAD_INDEX;};		
	/** Помечает группу точек*/
	virtual void MarkPoints(size_t in_from, size_t in_to) 
	{
		marked_points.clear();
		for (size_t i=in_from; i<=in_to; ++i)
		{
			marked_points.push_back(i);
		}
	};
	/** Снимает пометку*/
	virtual void UnmarkPoints() 
	{
		marked_points.clear();		
	};
	
	//Поиск значений
	
	/** Возвращает значение Y по X (через уравнение прямой, по умолчанию не работает)*/
	virtual double GetYbyX(double in_x){return DBL_MAX;};

	/** Возвращает значение X по Y (через уравнение прямой, по умолчанию не работает)*/
	virtual double GetXbyY(double in_y){return DBL_MAX;};	
	
		
	/** Возвращает видимую область*/
	virtual std::vector<size_t> GetVisible(double in_left, double in_top, double in_right, double in_bottom)
	{				
		std::vector<size_t> result;		
		if (GetCount() > 0)
		{	
			result.push_back(0);
			result.push_back(GetCount()-1);			
		}
		return result;
	};

	/** Устанавливает обработчик изменения данных*/
	virtual void SetOnDataChanged(VActionPtr in_action)
	{
		on_data_changed=in_action;
	};
	
	/** Устанавливает диапазон по X*/
	virtual void SetRangeX(const pro::interval<double>& in_range_x) {range_x=in_range_x;};
	/** Устанавливает диапазон по Y*/
	virtual void SetRangeY(const pro::interval<double>& in_range_y) {range_y=in_range_y;};

	/** Обработка события*/
	virtual bool ProcessEvent(const VEvent& in_event){return false;};
};
typedef boost::shared_ptr<VDataProvider> VDataProviderPtr;


/// Данные упорядоченные по X
class VXOrderedDataProvider : public VDataProvider
{
private:
	/// Возвращает индекс точки такой что ее координата меньше X а координате следующей за ней больше X
	size_t FindPrevIndex(double in_x)
	{
		if (this->GetCount()==0) return NOT_FOUND;		
		if (in_x <= this->GetX(0)) return 0;
		if (in_x >= this->GetX(GetCount()-1)) return this->GetCount()-1;				
		size_t left=0;
		size_t right=this->GetCount()-1;			
		while (right-left>1)
		{	
			(GetX((right+left)/2) >= in_x) ? right=(right+left)/2 : left=(right+left)/2;
		}	
		return left;	
	}
	/// Возвращает индекс точки такой что ее координата больше X а координата предыдущей меньше X
	size_t FindNextIndex(double in_x)
	{
		if (this->GetCount()==0) return NOT_FOUND;		
		if (in_x <= this->GetX(0)) return 0;
		if (in_x >= this->GetX(GetCount()-1)) return this->GetCount()-1;				
		size_t left=0;
		size_t right=this->GetCount()-1;			
		while (right-left>1)
		{	
			(GetX((right+left)/2) <= in_x) ? left=(right+left)/2 : right=(right+left)/2;				
		}	
		return right;	
	}
public:
	/** Возвращает значение Y по X*/
	double GetYbyX(double in_x)
	{
		size_t start=FindPrevIndex(in_x);
		size_t end=FindNextIndex(in_x);
		if (GetSizeX(start)==0 && start!=end)
			return line_t(point_t(GetX(start), GetY(start)), point_t(GetX(end), GetY(end))).Y(in_x);
		else
			return GetY(start);
	};
	/// Возвращает видимую область
	std::vector<size_t> GetVisible(double in_left, double in_top, double in_right, double in_bottom)
	{		
		//FixMe!!! учесть случай когда данные остсортированы на убывание		
		std::vector<size_t> result;		
		if (GetCount() > 0)
		{												
			size_t start=FindPrevIndex(std::max<double>(in_left, range_x.left_bound().value));
			size_t end=FindNextIndex(std::min<double>(in_right, range_x.right_bound().value));
			result.push_back(start!=BAD_INDEX ? start : 0);
			result.push_back(end!=BAD_INDEX ? end : GetCount()-1);			
		}
		return result;
	};
	/// Возвращает минимальное значение по X
	double GetMinX()
	{
		return (GetCount() > 0) ? std::min<double>(GetX(0), GetX(GetCount()-1)) : 0;		
	}
	/// Возвращает максимальное значение по X
	double GetMaxX()
	{
		return (GetCount() > 0) ? std::max<double>(GetX(0), GetX(GetCount()-1)) : 0;
	}
	/// Возвращает минимальное значение по Y
	double GetMinY()
	{
		return 0;		
	}
	/// Возвращает максимальное значение по Y
	double GetMaxY()
	{
		return 0;
	}
};
typedef boost::shared_ptr<VXOrderedDataProvider> VXOrderedDataProviderPtr;




/** Цвет*/
class VColor
{
private:
	/** Четырехбайтовое представление: RGBA (0xA40000ff)*/
	uint32_t color; 
public:

	//Конструкторы

	/** Конструктор без парамтров*/
	VColor()
	{
		color=0;
	};
	/** Конструктор из четырехбайтового целого*/
	VColor(uint32_t in_color)
	{
		*this=in_color;
	};
	/** Конструктор из четырех целых (каналов)*/
	VColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
	{		
		color=std::min<uint32_t>(255,r)<<24 | std::min<uint32_t>(255,g)<<16 | std::min<uint32_t>(255,b)<<8 | std::min<uint32_t>(255,a);		
	};
	/** Конструктор из трех целых (альфа канал по умолчанию равен 255)*/
	VColor(uint32_t r, uint32_t g, uint32_t b)
	{		
		color=std::min<uint32_t>(255,r)<<24 | std::min<uint32_t>(255,g)<<16 | std::min<uint32_t>(255,b)<<8 | 255;
	};
	/** Конструктор из шестнадцатеричного представления (строки вида FFFFFFFF)*/	
	VColor(const std::string& in_color)
	{	
		*this=in_color;
	};
	/** Конструктор из шестнадцатеричного представления (строки вида FFFFFFFF)*/
	VColor(const std::wstring& in_color)
	{		
		*this=in_color;
	};

	//Методы
	
	/** Возвращает красный канал*/
	uint32_t Red() const
	{		
		return (color & 0xff000000)>>24;
	};
	/** Возвращает зеленый канал*/
	uint32_t Green() const
	{		
		return (color & 0x00ff0000)>>16;
	};
	/** Возвращает синий канал*/
	uint32_t Blue() const
	{		
		return (color & 0x0000ff00)>>8;
	};
	/** Возвращает альфа канал*/
	uint32_t Alpha() const
	{		
		return (color & 0x000000ff);
	};
	
	//Операторы присваивания

	/** Оператор присваивания из четырехбайтового целого*/
	VColor& operator=(uint32_t& in_color)
	{
		color=in_color;
		return *this;
	};
		
	/// Оператор =	
	template<typename C, typename T> VColor& operator=(const std::basic_string<C,T>& in_color)
	{
		std::basic_stringstream<C,T> stream;
		stream << in_color;
		stream >> *this;
		return *this;
	}

	/** Оператор ==*/
	bool operator==(VColor& rval)
	{
		return color==rval.color;
	}

	/** Оператор !=*/
	bool operator!=(VColor& rval)
	{
		return color!=rval.color;
	}
	
	/// Оператор <<	
	template<typename C, typename T> friend std::basic_ostream<C,T>& operator<< (std::basic_ostream<C,T>& stream, VColor& in_color)
	{
		stream << std::hex << std::showbase << in_color.color;
		return stream;
	}
	
	/// Оператор >>
	template<typename C, typename T> friend std::basic_istream<C,T>& operator>> (std::basic_istream<C,T>& stream, VColor& in_color)
	{
		stream >> std::hex >> std::showbase >> in_color.color;
		return stream;
	}
};



#define COLOR_HTML_WHITE VColor(L"0xffffffff")
#define COLOR_HTML_SILVER VColor(L"0xc0c0c0ff")
#define COLOR_HTML_GRAY VColor(L"0x808080ff")
#define COLOR_HTML_BLACK VColor(L"0x000000ff")
#define COLOR_HTML_RED VColor(L"0xff0000ff")
#define COLOR_HTML_MAROON VColor(L"0x800000ff")
#define COLOR_HTML_YELLOW VColor(L"0xffff00ff")
#define COLOR_HTML_OLIVE VColor(L"0x808000ff")
#define COLOR_HTML_LIME VColor(L"0x00ff00ff")
#define COLOR_HTML_GREEN VColor(L"0x008000ff")
#define COLOR_HTML_AQUA VColor(L"0x00ffffff")
#define COLOR_HTML_TEAL VColor(L"0x008080ff")
#define COLOR_HTML_BLUE VColor(L"0x0000ffff")
#define COLOR_HTML_NAVY VColor(L"0x000080ff")
#define COLOR_HTML_FUCHSIA VColor(L"0xff00ffff")
#define COLOR_HTML_PURPLE VColor(L"0x800080ff")

#define COLOR_TRANSPARENT VColor(0,0,0,0)
#define GRAPH_COLOURS		L"0x3333ffff;0x33ff33ff;0xff00ffff;0x999900ff;0x3399ccff;0x993366ff;0xffcc33ff;0x339933ff;0x33ffffff;0x000000ff"
#define ALL_RED_COLOURS		L"0xff0000ff;0xff0000ff;0xff0000ff;0xff0000ff;0xff0000ff;0xff0000ff;0xff0000ff;0xff0000ff;0xff0000ff;0xff0000ff"

/** Набор цветов*/
class VColorQueue
{
private:
	/** Цвета*/
	std::vector<VColor> colors;
	/** Позиция*/
	size_t position;
	/** Цвет по умолчанию*/
	VColor default_color;
public:
	/** Конструктор без параметров*/
	VColorQueue()
	{
		position=0;
		default_color=VColor(0,0,0);
		//Формируем список цветов по умолчанию

		AddColor(VColor(51,51,255));
		AddColor(VColor(153,102,204));
		AddColor(VColor(255,102,204));
		AddColor(VColor(255,102,51));
		AddColor(VColor(204,204,102));
		AddColor(VColor(51,255,51));
		AddColor(VColor(204,204,0));
		AddColor(VColor(51,153,204));
		AddColor(VColor(204,102,102));
		AddColor(VColor(102,102,153));
		AddColor(VColor(204,255,51));
	};
	/** Конструктор из строки вида fffffff;00ff00ff;ff00ffff*/
	VColorQueue(const std::string& in_string)
	{
		position=0;
		default_color=VColor(0,0,0);		
		boost::char_separator<char> sep(";");
		boost::tokenizer< boost::char_separator<char> > tok(in_string, sep);
		for (boost::tokenizer< boost::char_separator<char> >::iterator beg=tok.begin(); beg!=tok.end();++beg)
		{
			VColor color(*beg);
			colors.push_back(color);
		}
	};
	/** Конструктор из строки вида fffffff;00ff00ff;ff00ffff*/
	VColorQueue(const std::wstring& in_string)
	{
		position=0;
		default_color=VColor(0,0,0);		
		boost::char_separator<wchar_t> sep(L";");
		boost::tokenizer< boost::char_separator<wchar_t>, std::wstring::const_iterator, std::wstring> tok(in_string, sep);
		for (boost::tokenizer< boost::char_separator<wchar_t>, std::wstring::const_iterator, std::wstring>::iterator beg=tok.begin(); beg!=tok.end();++beg)
		{
			VColor color(*beg);
			colors.push_back(color);
		}
	};
	/** Добавление одного элемента*/
	void AddColor(VColor in_color)
	{
		colors.push_back(in_color);
	};
	/** Очистка*/
	void Clear()
	{
		colors.clear();
	};
	/** Возвращает очередной цвет*/
	VColor& GetColor()
	{
		if (colors.empty())
			return default_color;
		return colors.at(position);		
	};
	/** Переходит к следующему цвету*/
	void NextColor()
	{
		position++;
		if (position>=colors.size())		
			position=0;
	}
};

/// Пунктир
class VDash
{
private:	
	double on;
	double off;
public:	
	VDash() : on(1.0), off(1.0) {};	
	VDash(double in_on, double in_off) : on(in_on), off(in_off) {};
	double On() const {return on;};
	double Off() const {return off;};
};

#define DASH1 VDash(1.0,1.0)
#define DASH2 VDash(2.0,2.0)
#define DASH3 VDash(3.0,3.0)

/** Набор масштабов*/
class VScaleQueue
{
private:
	/** Масштабы*/
	std::set<double> scales;
	typedef std::set<double>::iterator scale_itr;
	/** Позиция*/
	scale_itr current;
	/** Масштаб по умолчанию*/
	double default_scale;

	// Методы

	/** Формирует список масштабов по умолчанию*/
	void InitDefaultScales()
	{			
		//FixMe!!! полученные значения отличаются от требуемых в последнем знаке
		//это может привести к артефактам при отображении
		for (double i=0.1; i<1.0; i+=0.1) 
		{
			AddScale(i);
		}												
		for (double i=2.0; i<10.0; i+=1.0) 
		{
			AddScale(i);
		}		
		for (double i=15.0; i<100.0; i+=5.0) 
		{
			AddScale(i);
		}		
		for (double i=150.0; i<=1000.0; i+=50.0) 
		{
			AddScale(i);
		}		
		current=scales.begin();
	}

	template <class T> void InitFromString(const T& in_string, const T& in_separator1, const T& in_separator2);

public:

	/** Конструктор без параметров*/
	VScaleQueue()
	{
		default_scale=10;
		InitDefaultScales();		
	};
	/** Конструктор из строки вида 10;20;50;...*/
	VScaleQueue(const std::string& in_string)
	{		
		InitFromString<std::string>(in_string, ";", "/");
	};
	/** Конструктор из строки вида 10;20;50;...*/
	VScaleQueue(const std::wstring& in_string)
	{	
		InitFromString<std::wstring>(in_string, L";", L"/");
	};	
	/** Конструктор*/
	VScaleQueue(double in_min, double in_max, double in_step)
	{
		for (double scale = in_min; scale<=in_max; scale+=in_step)
			AddScale(scale);
		current=scales.begin();
	};
	/** Добавление одного элемента*/
	void AddScale(double in_scale)
	{
		scales.insert(in_scale);
	};	
	/** Очистка*/
	void Clear()
	{
		scales.clear();
	};
	/** Возвращает текущий масштаб*/
	double GetScale() const
	{
		if (!scales.empty())
			return *current;		
		else
			return default_scale;
	};
	/** Устанавливает текущий масштаб*/
	void SetScale(double in_scale)
	{	
		scales.insert(in_scale);
		current=scales.find(in_scale);
	}
	/** Переходит к следующему масштабу*/
	void NextScale()
	{		
		if (!scales.empty())
		{
			current++;
			if (current==scales.end())
				current--;
		}
	}
	/** Переходит к предыдущему масштабу*/
	void PrevScale()
	{		
		if (!scales.empty() && current!=scales.begin())
			current--;
	}
	/** Возвращает число масштабов*/
	size_t GetCount() const
	{
		return scales.size();
	}
	/** Возвращает масштаб по индексу*/
	double GetScale(size_t in_index)
	{
		size_t index=0;
		for (scale_itr i=scales.begin(); i!=scales.end(); ++i)
		{
			if (index==in_index)
				return *i;
			else
				index++;
		}
		return default_scale;
	}
	/** Форматирует строку*/
	void FormatString(std::string& out_value)
	{
		std::stringstream out;
		for (scale_itr i=scales.begin(); i!=scales.end(); ++i)
		{
			out << boost::lexical_cast<std::string>(*i);
			if (i!=scales.end())
				out << ";";
		}
		out_value=out.str();
	}
	/** Форматирует строку*/
	void FormatString(std::wstring& out_value)
	{
		std::wstringstream out;
		for (scale_itr i=scales.begin(); i!=scales.end(); ++i)
		{
			out << boost::lexical_cast<std::wstring>(*i);
			if (i!=scales.end())
				out << ";";
		}
		out_value=out.str();
	}
};

#define DEFAULT_FONT_FACE "Arial"

/** Шрифт*/
class VFont
{
private:
	/** Имя шрифта*/
	std::string face;
	/** Размер*/
	double size;
	/** Наклон*/
	_cairo_font_slant slant;
	/** Толщина*/
	_cairo_font_weight weight;

public:
	/** Конструктор*/
	VFont(std::string in_face, double in_size, _cairo_font_slant in_slant, _cairo_font_weight in_weight) :
		face(in_face), size(in_size), slant(in_slant), weight(in_weight){};	

	/** Возвращает имя*/
	const std::string& GetFace() const {return face;};
	/** Возвращает размер*/
	double GetSize() const {return size;};
	/** Возвращает наклон*/
	_cairo_font_slant GetSlant() const {return slant;};
	/** Возвращает толщину*/
	_cairo_font_weight GetWeight() const {return weight;};	
};

/// Базовый размер шрифта
DECLARE_VARIABLE(double, BASE_FONT_SIZE, 0.275)

/** Форматы графических файлов*/
enum FILE_FORMAT {SVG=1, PDF=2, PNG=3};

/** Режимы обрезания*/

#define CLIP_TOP 0x00000001
#define CLIP_BOTTOM 0x00000002
#define CLIP_LEFT 0x00000004
#define CLIP_RIGHT 0x00000008

#define CLIP_NONE 0
#define CLIP_VERTICAL (CLIP_TOP | CLIP_BOTTOM)
#define CLIP_HORIZONTAL (CLIP_LEFT | CLIP_RIGHT)
#define CLIP_AREA (CLIP_TOP | CLIP_BOTTOM | CLIP_LEFT | CLIP_RIGHT)

/** Область*/
class VArea : public VEventHandler, public VActiveObject
{
	private:

		//Копирующий конструктор закрыт!*/
		VArea(VArea &){};
		//Оператор присваивания закрыт!
		VArea & operator=(VArea &){};		
		
	protected:
		/** Координата левого верхнего угла по оси X*/
		double left;
		/** Координата левого верхнего угла по оси Y*/
		double top;
		/** Ширина*/
		double width;
		/** Высота*/
		double height;					

	public:	
		/** Конструктор*/
		VArea() : left(0), top(0), width(0), height(0){active=false;};
		/** Устанавливает размеры*/
		void SetSize(double in_width, double in_height);
		/** Возвращает ширину*/
		double GetWidth() const;
		/** Возвращает высоту*/
		double GetHeight() const;
		/** Устанавливает положение*/
		void SetPosition(double in_left, double in_top);
		/** Возвращает координату верхнего левого угла по оси X*/
		double GetLeft();
		/** Возвращает координату верхнего левого угла по оси Y*/
		double GetTop();
		/** Применяет область*/
		void Apply(cairo_t * cr, int clip);
		/** Возвращает исходное состояние*/
		void Restore(cairo_t * cr);
		/** Возвращает true, если курсор внутри области*/
		bool HitTest(double in_x, double in_y);	
		/** Пересчитывает координату X из экранных в область*/
		double ScreenToAreaX(double in_x);
		/** Пересчитывает координату Y из экранных в область*/
		double ScreenToAreaY(double in_y);
		/** Пересчитывает координату X из области в экранные*/
		double AreaToScreenX(double in_x);
		/** Пересчитывает координату Y из области в экранные*/
		double AreaToScreenY(double in_y);
		/** Обработка события*/
		bool ProcessEvent(const VEvent& in_event);
};
typedef boost::shared_ptr<VArea> VAreaPtr;

/** Класс управляющий размерами области*/
class VAreaSizer
{
private:		
	/** Указатель на область*/
	VAreaPtr area;
	/** Относительное положение левого верхнего угла по оси X*/
	double rel_left;
	/** Относительное положение левого верхнего угла по оси Y*/
	double rel_top;		
	/** Относительная ширина*/
	double rel_width;
	/** Относительная высота*/
	double rel_height;					
public:
	/** Конструктор*/
	VAreaSizer(VAreaPtr in_area, double in_rel_left, double in_rel_top, double in_rel_width, double in_rel_height);
	/** Установить относительное положение левого верхнего угла по оси X*/
	void SetRelLeft(const double in_rel_left);
	/** Получить относительное положение левого верхнего угла по оси X*/
	double GetRelLeft();
	/** Установить относительное положение левого верхнего угла по оси Y*/
	void SetRelTop(const double in_rel_top);
	/** Получить относительное положение левого верхнего угла по оси Y*/
	double GetRelTop();
	/** Установить относительную ширину*/
	void SetRelWidth(const double in_rel_width);
	/** Получить относительную ширину*/
	double GetRelWidth();
	/** Установить относительную высоту*/
	void SetRelHeight(const double in_rel_height);
	/** Получить относительную высоту*/
	double GetRelHeight();																		
	/** Вычисляет координаты*/
	void CalcSizeAndPos(double top_margin, double left_margin, double bottom_margin, double right_margin, 
		double in_visible_width, double in_visible_height, VDrawingDevicePtr in_device);
};
typedef boost::shared_ptr<VAreaSizer> VAreaSizerPtr;


/** Элемент*/	
class VElement : public VEventHandler, public VActiveObject
{		
	protected:	
		
		//Методы

		/** Выполняется перед отрисовкой*/
		virtual void PrePaint(cairo_t * cr);
		/** Выполняется после отрисовки*/
		virtual void PostPaint(cairo_t * cr);
		/** Собственно отрисовка (этот метод должен быть реализован в потомках)*/
		virtual void Paint(cairo_t * cr)=0;
	public:	
		/** Конструктор*/
		VElement() : VActiveObject(){};
		/** Деструктор*/
		virtual ~VElement(){};		
		/** Рисование*/
		void Render(cairo_t * cr);		
		/** Проверяет находится ли курсор мыши над элементом (по умолчанию возвращает false (неактивный элемент))*/
		virtual bool HitTest(double x, double y);		
		/** Признак того что мышь пересекла границу элемента*/
		virtual bool HitChanged();		
		/** Обработка события*/
		virtual bool ProcessEvent(const VEvent& in_event){return false;};
};
typedef boost::shared_ptr<VElement> VElementPtr;

/** Слой*/
class VLayer : public VEventHandler, public VActiveObject
{
	private:	

		//Копирующий конструктор закрыт!*/
		VLayer(VLayer &){};
		//Опреатор присваивания закрыт!
		VLayer & operator=(VLayer &){};
		/** Список элементов*/		
		std::list<VElementPtr> elements;
		/** Определение итератора*/		
		typedef std::list<VElementPtr>::iterator element_itr;		
		/** Признак видимости*/
		bool visible;		
		/** Имя слоя*/
		std::wstring name;		
	public:
		/** Конструктор*/
		VLayer(const std::wstring& in_name);		
		/** Рисование*/
		void Paint(cairo_t * cr);		
		/** Добавляет элемент в слой*/			
		void AddElement(VElementPtr element);		
		/** Удаляет все элементы*/
		void ClearElements();
		/** Делает слой видимым/невидимым*/
		void SetVisible(bool in_visible);
		/** Возвращает признак видимости слоя*/
		bool GetVisible();		
		/** Возвращает имя слоя*/
		std::wstring& GetName();
		/** Посылка события всем элементам слоя*/
		bool ProcessEvent(const VEvent& in_event);


		/* 
			To Do !!! 
			Предусмотреть:
			автоматическую генерацию уникальных имен 
			возможность переноса объектов между слоями
		*/			
};			
typedef boost::shared_ptr<VLayer> VLayerPtr;

/** Изображение - сырые данные*/
class VRawData
{
private:
	/// Ширина изображения
	size_t width;
	/// Высота изображения
	size_t height;
	/// Указатель на данные
	unsigned char * dataRGB;
	
	/// Выделяет память для данных
	void Allocate(size_t in_width, size_t in_height);
	/// Заполняет массив данных
	void Assign(unsigned char * in_dataBGRA);

public:
	/// Конструктор
	VRawData(size_t in_width, size_t in_height, unsigned char * in_dataBGRA);
	/// Конструктор
	VRawData(cairo_surface_t * in_surface);
	/// Конструктор
	VRawData(const VRawData& in_source);
	/// Деструктор
	~VRawData();
	/// Возвращает ширину изображения
	size_t GetWidth() const;
	/// Возвращает высоту изображения
	size_t GetHeight() const;
	/// Возвращает данные
	unsigned char * GetData() const;
};


/** Базовый класс - инструкции рисования*/
class VDrawing
{
private:
	//Копирующий конструктор закрыт!
	VDrawing(VDrawing &){};
	//Опреатор присваивания закрыт!
	VDrawing & operator=(VDrawing &){};
protected:
			
	//Данные	
	
			
	/** Заголовок графика*/
	std::wstring name;
	/** Глубина цвета*/
	cairo_format_t cairo_format;
	/** Устройство*/
	VDrawingDevicePtr device;
	/** Ширина видимой области в пикселях*/
	double visible_width;
	/** Высота видимой области в пикселях*/
	double visible_height;		
	/** Признак того, что рисунок изменился*/
	bool changed;	
	/** Список слоев*/
	std::vector<VLayerPtr> layers;
	/** Объявление итератора для списка слоев*/	
	typedef std::vector<VLayerPtr>::iterator layer_itr;		

	/** Определение типа подписки на событие*/
	typedef std::pair<uint32_t, VEventHandlerPtr> subscriber_t;
	/** Подписки на события*/
	std::multimap<uint32_t, VEventHandlerPtr> subscribers;	
	/** Определение итератора списка подписок*/
	typedef std::multimap<uint32_t, VEventHandlerPtr>::iterator subscribers_itr;

	/** Список действий доступных всегда*/
	std::vector<VActionPtr> standart_actions;
	/** Список действий доступных в данный момент*/
	std::vector<VActionPtr> current_actions;
	/** Список действий доступных из панели инструментов*/
	std::vector<VActionPtr> toolbar_actions;
	/** Объявление итератора для списка действий*/
	typedef std::vector<VActionPtr>::iterator action_itr;

	//Функции

	/** Инструкции рисования - этот метод должен быть переопределен в потомках*/
	virtual void Paint(cairo_t * cr);
	/** Сохранение SVG*/
	virtual void SaveSVG(const char * const file_name);	
	/** Сохранение PNG*/
	virtual void SavePNG(const char * const file_name);	
	/** Сохранение PDF*/
	virtual void SavePDF(const char * const file_name);		
	
public:

	/** Конструктор*/
	VDrawing();
	/** Деструктор*/
	virtual ~VDrawing();
		
	//Функции
	
	/** Задает имя графика*/
	virtual void SetName(const std::wstring& in_name);
	/** Возвращает имя графика*/
	virtual const std::wstring& GetName() const;
	/** Устанавливает устройство*/
	virtual void SetDevice(VDrawingDevicePtr in_device);
	/** Устанавливает размеры областей*/
	virtual void AdjustAreas() {};
	/** Рисование в памяти*/
	virtual VRawData Render();
	/** Вызывается при первой отрисовке*/
	virtual void OnShow();
	/** Сохранение в файл только видимой части*/
	virtual void SaveVisible(const char * const file_name, FILE_FORMAT format);
	/** Сохранение документа целиком - реализуется в потомках*/
	virtual void SaveDocument(const char * const file_name, FILE_FORMAT format);
	/** Сохранение PNG в поток*/
	virtual void SaveStreamPNG(std::ostream& in_outstream);
	/** Возвращает число страниц документа, по умолчанию возвращает 1. Требует реализации в потомках*/
	virtual size_t GetPageCount(){return 1;};
	/** Возвращает размер страницы в абс. координатах*/
	virtual double GetPageSize() {return 0;};
	/** Переходит на страницу с указанным номером*/
	virtual void GoToPage(int in_page) {};
	/** Устанавливает размер видимой части*/
	virtual void SetVisibleSize(const double width, const double height);		
	/** Возвращает видимую ширину*/
	virtual double GetVisibleWidth() const {return visible_width;};
	/** Возвращает видимую высоту*/
	virtual double GetVisibleHeight() const {return visible_height;};
	/** Возвращает true если рисунок изменился и требует перерисовки*/
	bool Changed();
	/** Добавить слой*/
	void AddLayer(const std::wstring& name);
	/** Возвращает указатель на слой*/
	VLayerPtr GetLayer(const std::wstring& name);
	/** Возвращает true, если слой с наким именем существует*/
	bool LayerExists(const std::wstring& name);
	/** Удалить слой*/
	void RemoveLayer(const std::wstring& name);
	/** Удалить все слои*/
	void ClearLayers();	
	/** Возвращает true, если изменились исходные данные*/
	virtual bool IsDataChanged();
	/** Добавляет действие*/
	void AddAction(VActionPtr in_action);
	/** Формирует список допустимых действий для точки*/
	virtual void CreateContextMenu(double x, double y) {};

	/** Одписать объект на событие*/
	virtual void Subscribe(uint32_t in_event_code, VEventHandlerPtr in_event_handler);	
	/** Пересылает событие всем подписчикам*/
	virtual bool ProcessEvent(const VEvent& in_event);

	/** Закрытие окна*/
	virtual void OnClose();
	/** Возвращает позицию по горизонтали*/
	virtual double GetHorzPos(){return 0;};
	/** Возвращает минимально возможную позицию по горизонтали*/
	virtual double GetHorzMin(){return 0;};
	/** Возвращает максимально возможную позицию по горизонтали*/
	virtual double GetHorzMax(){return 0;};	
	
	/** Прокрутить к нужной позиции*/
	virtual void ScrollToHorz(double in_value);

	
	/** Возвращает число доступных действий*/
	virtual size_t GetActionCount() const {return current_actions.size();};		
	/** Возвращает указатель на действие*/
	virtual VActionPtr GetAction(size_t in_index)
	{
		return current_actions.at(in_index);
	};
	/** Возвращает число доступных действий в панели инструментов*/
	virtual size_t GetToolbarActionCount() const {return toolbar_actions.size();};
	/** Возвращает указатель на действие в панели инструментов*/
	virtual VActionPtr GetToolbarAction(size_t in_index)
	{
		return toolbar_actions.at(in_index);
	};

};
typedef boost::shared_ptr<VDrawing> VDrawingPtr;


/// Документ PDF
class VPDFDocument
{
private:
	/// Холст cairo для PDF
	VCairoSurface surface;
	/// Контекст Cairo для PDF
	VCairoContext context;
public:
	/// Конструктор
	VPDFDocument(const std::wstring& in_filename, double in_width, double in_height);
	/// Добавляет страницу
	void Paint(cairo_surface_t * in_surface);
	/// Добавляет страницу
	void AddPage(cairo_surface_t * in_surface);
	/// Возвращает контекст
	cairo_t * GetContext();
	/// Добавляет страницу
	void AddPage();
};
typedef boost::shared_ptr<VPDFDocument> VPDFDocumentPtr;

//Вспомогательные функции FixMe!!! на самом деле лучше сделать ПОЛНУЮ обертку для cairo_t и методов рисования

/** Устанавливает цвет*/
void CairoSetColor(cairo_t * in_cr, const VColor& in_color);

/** Устанавливает толщину линии*/
void CairoSetLineWidth(cairo_t * in_cr , double in_width);

/** Устанавливает шрифт*/
void CairoSetFont(cairo_t * in_cr, const VFont& in_font);

/** Устанавливает стиль пунктира*/
void CairoSetDash(cairo_t * in_cr, double in_on=0, double in_off=0);

/** Выводит текст*/
void CairoShowText(cairo_t * in_cr, const std::wstring& in_text);

/** Выводит текст указанным цветом*/
void CairoShowText(cairo_t * in_cr, const std::wstring& in_text, const VColor& in_color);