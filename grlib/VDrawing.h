#pragma once

//���������� ��� ����, ����� ������������ ����������� ���� M_PI �� math.h
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

/** ����������� ���� XPM*/
typedef char* xpm_t;

//��������� FixMe!!! ��������� � ����������� ����
//FixMe!!! ������������ ��������� �����

/** ������������*/
static const cairo_antialias_t antialias=CAIRO_ANTIALIAS_NONE;
/** ������������*/
static const cairo_antialias_t font_antialias=CAIRO_ANTIALIAS_NONE;
/** ������� ����� �� ���������*/
static const double default_line_width=0.2;
/** ����������� ������� �����*/
static const double min_line_width=0.0;
/** ������� �����*/
static const double heavy_line_width=0.4;
/** ��� �������� ������� ������*/
static  const double font_size_step=0.025;
/** ������ ������ (��)*/
static const double text_space=0.1;
/** ������ �����*/
static const double default_left_margin=15;
/** ������ ������*/
static const double default_top_margin=15;
/** ������ ������*/
static const double default_right_margin=0;
/** ������ �����*/
static const double default_bottom_margin=5;


/// ��������� ����� � ����������
static double inch2cm(double in_value_inch)
{
	return in_value_inch/2.54;
};

/** ������� ����������*/
class VDrawingDevice
{
public:
	/** ���������� ������ �� X*/
	double GetSizeX() const;
	/** ���������� ������ �� Y*/
	double GetSizeY() const;	
	/** ������������� ���������� �� ����������� �� ���������� �� ��� X*/
	double DrawingToDeviceX(double in_value) const;
	/** ������������� ���������� �� ���������� � ����������� �� ��� X*/
	double DeviceToDrawingX(double in_value) const;
	/** ������������� ���������� �� ����������� �� ���������� �� ��� Y*/
	double DrawingToDeviceY(double in_value) const;
	/** ������������� ���������� �� ���������� � ����������� �� ��� Y*/
	double DeviceToDrawingY(double in_value) const;
protected:
	/** ������ �� ��� X*/
	double size_x;
	/** ������ �� ��� �*/
	double size_y;
	/** ���������� �� X*/
	double aspect_x;
	/** ���������� �� Y*/
	double aspect_y;
};
typedef boost::shared_ptr<VDrawingDevice> VDrawingDevicePtr;

/** ���������� Win32*/
class VDrawingDeviceWin32  : public VDrawingDevice
{
public:
	/** �����������*/
	VDrawingDeviceWin32(HDC in_hdc);
};

/** ������*/
class VDrawingDevicePaper : public VDrawingDevice
{
public:
	/** �����������*/
	VDrawingDevicePaper(double in_ppi_x, double in_ppi_y, double in_size_mm_x, double in_size_mm_y);
};


/** �����*/
class VDrawingDeviceDisplay : public VDrawingDevice
{
public:
	/** �����������*/
	VDrawingDeviceDisplay(double in_ppi_x, double in_ppi_y, double in_size_x, double in_size_y);
};

//���� ������� ��������� ��������
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

//���� ������� ���������� ���������� ������
#define EVENT_DATA_ACTIVATE_POINT 101
#define EVENT_DATA_START_DRAG 102
#define EVENT_DATA_DRAG 103
#define EVENT_DATA_DROP 104
#define EVENT_DATA_DELETE_POINT 105


/** ��������� ������� ������*/
class VHotKey
{
private:
	/** ��� �������*/
	int key;
	/** ����������� - alt*/
	bool alt;
	/** ����������� - ctrl*/
	bool ctrl;
	/** ����������� - shift*/
	bool shift;
public: 
	/** �����������*/
	VHotKey() : key(0), alt(false), ctrl(false), shift(false) {};
	/** �����������*/
	VHotKey(int in_key) : 
	  key(in_key), alt(false), ctrl(false), shift(false) {};
	/** �����������*/
	VHotKey(int in_key, bool in_alt, bool in_ctrl, bool in_shift) : 
	  key(in_key), alt(in_alt), ctrl(in_ctrl), shift(in_shift) {};
	/** �����������*/
	VHotKey(const std::wstring& in_string);
	/** ���������� ���*/
	int GetCode() const {return key;};
	/** ���������� ������� Alt*/
	bool GetAlt() const {return alt;};
	/** ���������� ������� Ctrl*/
	bool GetCtrl() const {return ctrl;};
	/** ���������� ������� Shift*/
	bool GetShift() const {return shift;};
	/** �������� ==*/ //FixME!!! ��� shift,ctrl,alt ����� ��������� ������ ��� �������
	bool operator==(const VHotKey& in_right) const {return key==in_right.key && alt==in_right.alt && 
		ctrl==in_right.ctrl && shift==in_right.shift;};
	/** ��������� <*/
	bool operator<(const VHotKey& in_right) const
	{
		return key<in_right.key;
	};
	/** ���������� ������ ��������*/
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

//���������� ��������� ������� �������

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


/** �������*/
class VEvent
{
protected:
	/** ��� �������*/
	uint32_t code;
	/** ������� ���� ��� ������� ����������*/
	bool handled; 	
public:	
	/** �����������*/
	VEvent(uint32_t in_code) : code(in_code), handled(false){};
	/** ����������*/
	virtual ~VEvent(){};
	/** ���������� ���*/
	virtual uint32_t GetCode() const {return code;};
	/** ���������� ������� ����, ��� ������� ����������*/
	virtual bool GetHandled() const {return handled;};
	/** ������������� ������� ����, ��� ������� ����������*/
	virtual void SetHandled(bool in_handled) {handled=in_handled;};	
};
typedef boost::shared_ptr<VEvent> VEventPtr;

/** ��������� ����������� �������*/
class VEventHandler
{
public:
	/** ��������� �������*/
	virtual bool ProcessEvent(const VEvent& in_event)=0;
};
typedef boost::shared_ptr<VEventHandler> VEventHandlerPtr;

/** ������� �� ����������*/
class VKeyEvent : public VEvent
{
private:
	/** ��� �������*/
	VHotKey hot_key;	
public:
	/** �����������*/
	VKeyEvent(uint32_t in_code, const VHotKey& in_hot_key) : VEvent(in_code), hot_key(in_hot_key){};
	/** ���������� ��� �������*/	
	const VHotKey& GetKey() const {return hot_key;};
};
typedef boost::shared_ptr<VKeyEvent> VKeyEventPtr;

/** ������� �� ����*/
class VMouseEvent : public VEvent
{
private:
	/** ���������� X ���� � ������ �������*/
	double x;
	/** ���������� Y ���� � ������ �������*/
	double y;
public:
	/** �����������*/
	VMouseEvent(uint32_t in_code, double in_x, double in_y) : VEvent(in_code), x(in_x), y(in_y){};
	/** ���������� ��������� X*/
	double GetX() const {return x;};
	/** ���������� ��������� Y*/
	double GetY() const {return y;};
};
typedef boost::shared_ptr<VMouseEvent> VMouseEventPtr;

/** ������� �� ������ ����*/
class VMouseWheelEvent : public VEvent
{
private:
	/** �������� �������� ������ ����*/
	int r;	
public:
	/** �����������*/
	VMouseWheelEvent(uint32_t in_code, int in_r) : VEvent(in_code), r(in_r){};
	/** ���������� �������� ��������*/
	double GetRotation() const {return r;};	
};
typedef boost::shared_ptr<VMouseWheelEvent> VMouseWheelEventPtr;

/** �������� � �������*/
class VAction : public VEventHandler
{
protected:
	/** ��� - �������� ��������*/
	std::wstring name;
	/** ��� ������� �������*/
	std::set<VHotKey> hot_keys;	
	/** ��������� �� XPM � ��������� ���������*/
	xpm_t* enabled_xpm;
	/** ��������� �� XPM � ����������� ���������*/
	xpm_t* disabled_xpm;
public:		
	/** ����������*/
	virtual ~VAction(){};
	/** ��������� �������� (����� ������ ���� ��������� � ��������)*/
	virtual void Execute()=0;
	/** ���������� ��� - �������� ��������*/
	virtual std::wstring& GetName(){return name;};
	/** ������������� ���-�������� ��������*/
	virtual void SetName(const std::wstring& in_name){name=in_name;};
	/** ������������� ������� �������*/
	virtual void SetHotKey(const VHotKey& in_hot_key) {hot_keys.insert(in_hot_key);};
	/** ���������� true, ���� �������� ��������*/
	virtual bool Allowed() const{return true;};	
	/** ���������� ������� ������������� ������� �������������*/
	virtual bool NeedAccept() const{return false;};
	/** ���������� ������ ������� �������������*/
	virtual std::wstring GetAcceptQuestion() const{return L"";};
	/** ������������� XPM � ��������� ���������*/
	virtual void SetEnabledXPM(xpm_t* in_xpm){enabled_xpm=in_xpm;};
	/** ������������� XPM � ����������� ���������*/
	virtual void SetDisabledXPM(xpm_t* in_xpm){disabled_xpm=in_xpm;};
	/** ���������� XPM � ��������� ���������*/
	virtual xpm_t* const GetEnabledXPM(){return enabled_xpm;};
	/** ���������� XPM � ����������� ���������*/
	virtual xpm_t* const GetDisabledXPM(){return disabled_xpm;};
	/** ��������� �������*/
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

/** ������ ������� ����� ���� �������� ��� ����������*/
class VActiveObject
{
protected:
	/** ������� ����������*/
	bool active;
public:
	/** �����������*/
	VActiveObject() : active(true){};
	/** ����������� ����������*/
	virtual ~VActiveObject(){};
	/** ������������� ������� ����������� ��������� ������*/
	virtual void SetActive(bool in_active) {active=in_active;};
	/** ���������� ������� ����������� ��������� ������*/
	virtual bool GetActive() const {return active;};
};
typedef boost::shared_ptr<VActiveObject> VActiveObjectPtr;

//������ � ���� ������
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

/** ������������ �����*/
class VTextProvider
{
public:
	/** ���������� ����� �����*/
	virtual size_t GetCount() const = 0;
	/** ���������� ������*/
	virtual const std::wstring& GetItem(size_t in_index) const = 0;
};
typedef boost::shared_ptr<VTextProvider> VTextProviderPtr;

/** ������ �������*/
class VDataProvider : public VEventHandler, public VActiveObject
{
private:
	
	//���������� ����������� ������!*/
	VDataProvider(VDataProvider &){};
	//�������� ������������ ������!
	VDataProvider & operator=(VDataProvider &){};	

protected:	
		
	/** ��������� �����*/
	size_t selected_point;
	/** ���������� �����*/
	std::vector<size_t> marked_points;
	/** ���������� ��������� ������*/
	VActionPtr on_data_changed;	
	/** �������*/
	std::wstring legend;
	/** �������� �� X*/
	pro::interval<double> range_x;
	/** �������� �� Y*/
	pro::interval<double> range_y;

public:
	/** ����������� ��� ����������*/
	VDataProvider() : VActiveObject(), selected_point(BAD_INDEX),
		range_x(0-DBL_MAX, DBL_MAX), range_y(0-DBL_MAX, DBL_MAX) {};
	/** ����������*/
	virtual ~VDataProvider(){};	
	/** ��������� ������*/
	virtual void RefreshData(){};
	/** ������������� �������*/
	virtual void SetLegend(const std::wstring& in_legend) {legend=in_legend;};
	/** ���������� �������*/
	virtual const std::wstring& GetLegend() const {return legend;};
	/** ���������� ������ ������*/
	virtual size_t GetCount() const=0;
	/** ���������� ���������� �� ��� X*/
	virtual double GetX(size_t in_index) const=0;
	/** ���������� ���������� �� ��� Y*/
	virtual double GetY(size_t in_index) const=0;
	/** ���������� ������ "�����" �� X (����� ����� ��� ����������� ��������)*/
	virtual double GetSizeX(size_t in_index) const {return 0;};
	/** ���������� ������ "�����" �� Y (����� ����� ��� ����������� ��������)*/
	virtual double GetSizeY(size_t in_index) const {return 0;};
	/** ���������� ����� ��� �����, ����� ������������ � �������� �� ��������� ���������� 0*/
	virtual uint64_t GetFlags(size_t in_index) const {return 0;};
	/** ���������� ��������� �� ��������������� ��������� ������ �� ��������� NULL*/
	virtual const void * GetUserData(size_t in_index) const {return NULL;};
	/** ���������� �����*/
	virtual std::wstring GetLabel(size_t in_index) const {return std::wstring(L"");};
	/** ���������� ����� �� ��� �*/
	virtual std::wstring GetLabelX(size_t in_index) const {return std::wstring(L"");};
	/** ���������� ����� �� ��� Y*/
	virtual std::wstring GetLabelY(size_t in_index) const {return std::wstring(L"");};			
	/** ���������� ����������� �������� �� Y*/
	virtual double GetMinY() = 0;
	/** ���������� ������������ �������� �� Y*/
	virtual double GetMaxY() = 0;
	/** ���������� ����������� �������� �� X*/
	virtual double GetMinX() = 0;
	/** ���������� ������������ �������� �� X*/
	virtual double GetMaxX() = 0;		
	
	//�������������� ������
	
	/** ���������� true ���� ����� ����� ������. �� ��������� ����� ������ ����� �����*/
	virtual bool CanChange(size_t in_index) const { return true; };
	/** ������������� �������� X*/
	virtual void SetXY(size_t in_index, double in_x, double in_y){};	
	/** ������������� �������� ������*/
	virtual void SetFlags(size_t in_index, uint64_t in_flags){};
	/** ��������� �����*/
	virtual void AddXY(size_t in_index, double in_x, double in_y, double in_size_x=0, double in_size_y=0, 
		const std::wstring& in_label=L"", uint64_t in_flags=0){};
	/** ������� �����*/
	virtual void DeleteItem(size_t in_index){};
	/** ������� ������ �����*/
	virtual void DeleteRange(size_t in_from, size_t in_to){};
	/** ������� ��� �����*/
	virtual void DeleteAll(){};
	/** �������� �����*/
	virtual void SelectPoint(size_t in_index) {selected_point=in_index;};
	/** ���������� ���������� �����*/
	virtual size_t GetSelectedPoint() const {return selected_point;};
	/** ������� ���������*/
	virtual void DeselectPoint() {selected_point=BAD_INDEX;};		
	/** �������� ������ �����*/
	virtual void MarkPoints(size_t in_from, size_t in_to) 
	{
		marked_points.clear();
		for (size_t i=in_from; i<=in_to; ++i)
		{
			marked_points.push_back(i);
		}
	};
	/** ������� �������*/
	virtual void UnmarkPoints() 
	{
		marked_points.clear();		
	};
	
	//����� ��������
	
	/** ���������� �������� Y �� X (����� ��������� ������, �� ��������� �� ��������)*/
	virtual double GetYbyX(double in_x){return DBL_MAX;};

	/** ���������� �������� X �� Y (����� ��������� ������, �� ��������� �� ��������)*/
	virtual double GetXbyY(double in_y){return DBL_MAX;};	
	
		
	/** ���������� ������� �������*/
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

	/** ������������� ���������� ��������� ������*/
	virtual void SetOnDataChanged(VActionPtr in_action)
	{
		on_data_changed=in_action;
	};
	
	/** ������������� �������� �� X*/
	virtual void SetRangeX(const pro::interval<double>& in_range_x) {range_x=in_range_x;};
	/** ������������� �������� �� Y*/
	virtual void SetRangeY(const pro::interval<double>& in_range_y) {range_y=in_range_y;};

	/** ��������� �������*/
	virtual bool ProcessEvent(const VEvent& in_event){return false;};
};
typedef boost::shared_ptr<VDataProvider> VDataProviderPtr;


/// ������ ������������� �� X
class VXOrderedDataProvider : public VDataProvider
{
private:
	/// ���������� ������ ����� ����� ��� �� ���������� ������ X � ���������� ��������� �� ��� ������ X
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
	/// ���������� ������ ����� ����� ��� �� ���������� ������ X � ���������� ���������� ������ X
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
	/** ���������� �������� Y �� X*/
	double GetYbyX(double in_x)
	{
		size_t start=FindPrevIndex(in_x);
		size_t end=FindNextIndex(in_x);
		if (GetSizeX(start)==0 && start!=end)
			return line_t(point_t(GetX(start), GetY(start)), point_t(GetX(end), GetY(end))).Y(in_x);
		else
			return GetY(start);
	};
	/// ���������� ������� �������
	std::vector<size_t> GetVisible(double in_left, double in_top, double in_right, double in_bottom)
	{		
		//FixMe!!! ������ ������ ����� ������ �������������� �� ��������		
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
	/// ���������� ����������� �������� �� X
	double GetMinX()
	{
		return (GetCount() > 0) ? std::min<double>(GetX(0), GetX(GetCount()-1)) : 0;		
	}
	/// ���������� ������������ �������� �� X
	double GetMaxX()
	{
		return (GetCount() > 0) ? std::max<double>(GetX(0), GetX(GetCount()-1)) : 0;
	}
	/// ���������� ����������� �������� �� Y
	double GetMinY()
	{
		return 0;		
	}
	/// ���������� ������������ �������� �� Y
	double GetMaxY()
	{
		return 0;
	}
};
typedef boost::shared_ptr<VXOrderedDataProvider> VXOrderedDataProviderPtr;




/** ����*/
class VColor
{
private:
	/** ��������������� �������������: RGBA (0xA40000ff)*/
	uint32_t color; 
public:

	//������������

	/** ����������� ��� ���������*/
	VColor()
	{
		color=0;
	};
	/** ����������� �� ���������������� ������*/
	VColor(uint32_t in_color)
	{
		*this=in_color;
	};
	/** ����������� �� ������� ����� (�������)*/
	VColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
	{		
		color=std::min<uint32_t>(255,r)<<24 | std::min<uint32_t>(255,g)<<16 | std::min<uint32_t>(255,b)<<8 | std::min<uint32_t>(255,a);		
	};
	/** ����������� �� ���� ����� (����� ����� �� ��������� ����� 255)*/
	VColor(uint32_t r, uint32_t g, uint32_t b)
	{		
		color=std::min<uint32_t>(255,r)<<24 | std::min<uint32_t>(255,g)<<16 | std::min<uint32_t>(255,b)<<8 | 255;
	};
	/** ����������� �� ������������������ ������������� (������ ���� FFFFFFFF)*/	
	VColor(const std::string& in_color)
	{	
		*this=in_color;
	};
	/** ����������� �� ������������������ ������������� (������ ���� FFFFFFFF)*/
	VColor(const std::wstring& in_color)
	{		
		*this=in_color;
	};

	//������
	
	/** ���������� ������� �����*/
	uint32_t Red() const
	{		
		return (color & 0xff000000)>>24;
	};
	/** ���������� ������� �����*/
	uint32_t Green() const
	{		
		return (color & 0x00ff0000)>>16;
	};
	/** ���������� ����� �����*/
	uint32_t Blue() const
	{		
		return (color & 0x0000ff00)>>8;
	};
	/** ���������� ����� �����*/
	uint32_t Alpha() const
	{		
		return (color & 0x000000ff);
	};
	
	//��������� ������������

	/** �������� ������������ �� ���������������� ������*/
	VColor& operator=(uint32_t& in_color)
	{
		color=in_color;
		return *this;
	};
		
	/// �������� =	
	template<typename C, typename T> VColor& operator=(const std::basic_string<C,T>& in_color)
	{
		std::basic_stringstream<C,T> stream;
		stream << in_color;
		stream >> *this;
		return *this;
	}

	/** �������� ==*/
	bool operator==(VColor& rval)
	{
		return color==rval.color;
	}

	/** �������� !=*/
	bool operator!=(VColor& rval)
	{
		return color!=rval.color;
	}
	
	/// �������� <<	
	template<typename C, typename T> friend std::basic_ostream<C,T>& operator<< (std::basic_ostream<C,T>& stream, VColor& in_color)
	{
		stream << std::hex << std::showbase << in_color.color;
		return stream;
	}
	
	/// �������� >>
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

/** ����� ������*/
class VColorQueue
{
private:
	/** �����*/
	std::vector<VColor> colors;
	/** �������*/
	size_t position;
	/** ���� �� ���������*/
	VColor default_color;
public:
	/** ����������� ��� ����������*/
	VColorQueue()
	{
		position=0;
		default_color=VColor(0,0,0);
		//��������� ������ ������ �� ���������

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
	/** ����������� �� ������ ���� fffffff;00ff00ff;ff00ffff*/
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
	/** ����������� �� ������ ���� fffffff;00ff00ff;ff00ffff*/
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
	/** ���������� ������ ��������*/
	void AddColor(VColor in_color)
	{
		colors.push_back(in_color);
	};
	/** �������*/
	void Clear()
	{
		colors.clear();
	};
	/** ���������� ��������� ����*/
	VColor& GetColor()
	{
		if (colors.empty())
			return default_color;
		return colors.at(position);		
	};
	/** ��������� � ���������� �����*/
	void NextColor()
	{
		position++;
		if (position>=colors.size())		
			position=0;
	}
};

/// �������
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

/** ����� ���������*/
class VScaleQueue
{
private:
	/** ��������*/
	std::set<double> scales;
	typedef std::set<double>::iterator scale_itr;
	/** �������*/
	scale_itr current;
	/** ������� �� ���������*/
	double default_scale;

	// ������

	/** ��������� ������ ��������� �� ���������*/
	void InitDefaultScales()
	{			
		//FixMe!!! ���������� �������� ���������� �� ��������� � ��������� �����
		//��� ����� �������� � ���������� ��� �����������
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

	/** ����������� ��� ����������*/
	VScaleQueue()
	{
		default_scale=10;
		InitDefaultScales();		
	};
	/** ����������� �� ������ ���� 10;20;50;...*/
	VScaleQueue(const std::string& in_string)
	{		
		InitFromString<std::string>(in_string, ";", "/");
	};
	/** ����������� �� ������ ���� 10;20;50;...*/
	VScaleQueue(const std::wstring& in_string)
	{	
		InitFromString<std::wstring>(in_string, L";", L"/");
	};	
	/** �����������*/
	VScaleQueue(double in_min, double in_max, double in_step)
	{
		for (double scale = in_min; scale<=in_max; scale+=in_step)
			AddScale(scale);
		current=scales.begin();
	};
	/** ���������� ������ ��������*/
	void AddScale(double in_scale)
	{
		scales.insert(in_scale);
	};	
	/** �������*/
	void Clear()
	{
		scales.clear();
	};
	/** ���������� ������� �������*/
	double GetScale() const
	{
		if (!scales.empty())
			return *current;		
		else
			return default_scale;
	};
	/** ������������� ������� �������*/
	void SetScale(double in_scale)
	{	
		scales.insert(in_scale);
		current=scales.find(in_scale);
	}
	/** ��������� � ���������� ��������*/
	void NextScale()
	{		
		if (!scales.empty())
		{
			current++;
			if (current==scales.end())
				current--;
		}
	}
	/** ��������� � ����������� ��������*/
	void PrevScale()
	{		
		if (!scales.empty() && current!=scales.begin())
			current--;
	}
	/** ���������� ����� ���������*/
	size_t GetCount() const
	{
		return scales.size();
	}
	/** ���������� ������� �� �������*/
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
	/** ����������� ������*/
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
	/** ����������� ������*/
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

/** �����*/
class VFont
{
private:
	/** ��� ������*/
	std::string face;
	/** ������*/
	double size;
	/** ������*/
	_cairo_font_slant slant;
	/** �������*/
	_cairo_font_weight weight;

public:
	/** �����������*/
	VFont(std::string in_face, double in_size, _cairo_font_slant in_slant, _cairo_font_weight in_weight) :
		face(in_face), size(in_size), slant(in_slant), weight(in_weight){};	

	/** ���������� ���*/
	const std::string& GetFace() const {return face;};
	/** ���������� ������*/
	double GetSize() const {return size;};
	/** ���������� ������*/
	_cairo_font_slant GetSlant() const {return slant;};
	/** ���������� �������*/
	_cairo_font_weight GetWeight() const {return weight;};	
};

/// ������� ������ ������
DECLARE_VARIABLE(double, BASE_FONT_SIZE, 0.275)

/** ������� ����������� ������*/
enum FILE_FORMAT {SVG=1, PDF=2, PNG=3};

/** ������ ���������*/

#define CLIP_TOP 0x00000001
#define CLIP_BOTTOM 0x00000002
#define CLIP_LEFT 0x00000004
#define CLIP_RIGHT 0x00000008

#define CLIP_NONE 0
#define CLIP_VERTICAL (CLIP_TOP | CLIP_BOTTOM)
#define CLIP_HORIZONTAL (CLIP_LEFT | CLIP_RIGHT)
#define CLIP_AREA (CLIP_TOP | CLIP_BOTTOM | CLIP_LEFT | CLIP_RIGHT)

/** �������*/
class VArea : public VEventHandler, public VActiveObject
{
	private:

		//���������� ����������� ������!*/
		VArea(VArea &){};
		//�������� ������������ ������!
		VArea & operator=(VArea &){};		
		
	protected:
		/** ���������� ������ �������� ���� �� ��� X*/
		double left;
		/** ���������� ������ �������� ���� �� ��� Y*/
		double top;
		/** ������*/
		double width;
		/** ������*/
		double height;					

	public:	
		/** �����������*/
		VArea() : left(0), top(0), width(0), height(0){active=false;};
		/** ������������� �������*/
		void SetSize(double in_width, double in_height);
		/** ���������� ������*/
		double GetWidth() const;
		/** ���������� ������*/
		double GetHeight() const;
		/** ������������� ���������*/
		void SetPosition(double in_left, double in_top);
		/** ���������� ���������� �������� ������ ���� �� ��� X*/
		double GetLeft();
		/** ���������� ���������� �������� ������ ���� �� ��� Y*/
		double GetTop();
		/** ��������� �������*/
		void Apply(cairo_t * cr, int clip);
		/** ���������� �������� ���������*/
		void Restore(cairo_t * cr);
		/** ���������� true, ���� ������ ������ �������*/
		bool HitTest(double in_x, double in_y);	
		/** ������������� ���������� X �� �������� � �������*/
		double ScreenToAreaX(double in_x);
		/** ������������� ���������� Y �� �������� � �������*/
		double ScreenToAreaY(double in_y);
		/** ������������� ���������� X �� ������� � ��������*/
		double AreaToScreenX(double in_x);
		/** ������������� ���������� Y �� ������� � ��������*/
		double AreaToScreenY(double in_y);
		/** ��������� �������*/
		bool ProcessEvent(const VEvent& in_event);
};
typedef boost::shared_ptr<VArea> VAreaPtr;

/** ����� ����������� ��������� �������*/
class VAreaSizer
{
private:		
	/** ��������� �� �������*/
	VAreaPtr area;
	/** ������������� ��������� ������ �������� ���� �� ��� X*/
	double rel_left;
	/** ������������� ��������� ������ �������� ���� �� ��� Y*/
	double rel_top;		
	/** ������������� ������*/
	double rel_width;
	/** ������������� ������*/
	double rel_height;					
public:
	/** �����������*/
	VAreaSizer(VAreaPtr in_area, double in_rel_left, double in_rel_top, double in_rel_width, double in_rel_height);
	/** ���������� ������������� ��������� ������ �������� ���� �� ��� X*/
	void SetRelLeft(const double in_rel_left);
	/** �������� ������������� ��������� ������ �������� ���� �� ��� X*/
	double GetRelLeft();
	/** ���������� ������������� ��������� ������ �������� ���� �� ��� Y*/
	void SetRelTop(const double in_rel_top);
	/** �������� ������������� ��������� ������ �������� ���� �� ��� Y*/
	double GetRelTop();
	/** ���������� ������������� ������*/
	void SetRelWidth(const double in_rel_width);
	/** �������� ������������� ������*/
	double GetRelWidth();
	/** ���������� ������������� ������*/
	void SetRelHeight(const double in_rel_height);
	/** �������� ������������� ������*/
	double GetRelHeight();																		
	/** ��������� ����������*/
	void CalcSizeAndPos(double top_margin, double left_margin, double bottom_margin, double right_margin, 
		double in_visible_width, double in_visible_height, VDrawingDevicePtr in_device);
};
typedef boost::shared_ptr<VAreaSizer> VAreaSizerPtr;


/** �������*/	
class VElement : public VEventHandler, public VActiveObject
{		
	protected:	
		
		//������

		/** ����������� ����� ����������*/
		virtual void PrePaint(cairo_t * cr);
		/** ����������� ����� ���������*/
		virtual void PostPaint(cairo_t * cr);
		/** ���������� ��������� (���� ����� ������ ���� ���������� � ��������)*/
		virtual void Paint(cairo_t * cr)=0;
	public:	
		/** �����������*/
		VElement() : VActiveObject(){};
		/** ����������*/
		virtual ~VElement(){};		
		/** ���������*/
		void Render(cairo_t * cr);		
		/** ��������� ��������� �� ������ ���� ��� ��������� (�� ��������� ���������� false (���������� �������))*/
		virtual bool HitTest(double x, double y);		
		/** ������� ���� ��� ���� ��������� ������� ��������*/
		virtual bool HitChanged();		
		/** ��������� �������*/
		virtual bool ProcessEvent(const VEvent& in_event){return false;};
};
typedef boost::shared_ptr<VElement> VElementPtr;

/** ����*/
class VLayer : public VEventHandler, public VActiveObject
{
	private:	

		//���������� ����������� ������!*/
		VLayer(VLayer &){};
		//�������� ������������ ������!
		VLayer & operator=(VLayer &){};
		/** ������ ���������*/		
		std::list<VElementPtr> elements;
		/** ����������� ���������*/		
		typedef std::list<VElementPtr>::iterator element_itr;		
		/** ������� ���������*/
		bool visible;		
		/** ��� ����*/
		std::wstring name;		
	public:
		/** �����������*/
		VLayer(const std::wstring& in_name);		
		/** ���������*/
		void Paint(cairo_t * cr);		
		/** ��������� ������� � ����*/			
		void AddElement(VElementPtr element);		
		/** ������� ��� ��������*/
		void ClearElements();
		/** ������ ���� �������/���������*/
		void SetVisible(bool in_visible);
		/** ���������� ������� ��������� ����*/
		bool GetVisible();		
		/** ���������� ��� ����*/
		std::wstring& GetName();
		/** ������� ������� ���� ��������� ����*/
		bool ProcessEvent(const VEvent& in_event);


		/* 
			To Do !!! 
			�������������:
			�������������� ��������� ���������� ���� 
			����������� �������� �������� ����� ������
		*/			
};			
typedef boost::shared_ptr<VLayer> VLayerPtr;

/** ����������� - ����� ������*/
class VRawData
{
private:
	/// ������ �����������
	size_t width;
	/// ������ �����������
	size_t height;
	/// ��������� �� ������
	unsigned char * dataRGB;
	
	/// �������� ������ ��� ������
	void Allocate(size_t in_width, size_t in_height);
	/// ��������� ������ ������
	void Assign(unsigned char * in_dataBGRA);

public:
	/// �����������
	VRawData(size_t in_width, size_t in_height, unsigned char * in_dataBGRA);
	/// �����������
	VRawData(cairo_surface_t * in_surface);
	/// �����������
	VRawData(const VRawData& in_source);
	/// ����������
	~VRawData();
	/// ���������� ������ �����������
	size_t GetWidth() const;
	/// ���������� ������ �����������
	size_t GetHeight() const;
	/// ���������� ������
	unsigned char * GetData() const;
};


/** ������� ����� - ���������� ���������*/
class VDrawing
{
private:
	//���������� ����������� ������!
	VDrawing(VDrawing &){};
	//�������� ������������ ������!
	VDrawing & operator=(VDrawing &){};
protected:
			
	//������	
	
			
	/** ��������� �������*/
	std::wstring name;
	/** ������� �����*/
	cairo_format_t cairo_format;
	/** ����������*/
	VDrawingDevicePtr device;
	/** ������ ������� ������� � ��������*/
	double visible_width;
	/** ������ ������� ������� � ��������*/
	double visible_height;		
	/** ������� ����, ��� ������� ���������*/
	bool changed;	
	/** ������ �����*/
	std::vector<VLayerPtr> layers;
	/** ���������� ��������� ��� ������ �����*/	
	typedef std::vector<VLayerPtr>::iterator layer_itr;		

	/** ����������� ���� �������� �� �������*/
	typedef std::pair<uint32_t, VEventHandlerPtr> subscriber_t;
	/** �������� �� �������*/
	std::multimap<uint32_t, VEventHandlerPtr> subscribers;	
	/** ����������� ��������� ������ ��������*/
	typedef std::multimap<uint32_t, VEventHandlerPtr>::iterator subscribers_itr;

	/** ������ �������� ��������� ������*/
	std::vector<VActionPtr> standart_actions;
	/** ������ �������� ��������� � ������ ������*/
	std::vector<VActionPtr> current_actions;
	/** ������ �������� ��������� �� ������ ������������*/
	std::vector<VActionPtr> toolbar_actions;
	/** ���������� ��������� ��� ������ ��������*/
	typedef std::vector<VActionPtr>::iterator action_itr;

	//�������

	/** ���������� ��������� - ���� ����� ������ ���� ������������� � ��������*/
	virtual void Paint(cairo_t * cr);
	/** ���������� SVG*/
	virtual void SaveSVG(const char * const file_name);	
	/** ���������� PNG*/
	virtual void SavePNG(const char * const file_name);	
	/** ���������� PDF*/
	virtual void SavePDF(const char * const file_name);		
	
public:

	/** �����������*/
	VDrawing();
	/** ����������*/
	virtual ~VDrawing();
		
	//�������
	
	/** ������ ��� �������*/
	virtual void SetName(const std::wstring& in_name);
	/** ���������� ��� �������*/
	virtual const std::wstring& GetName() const;
	/** ������������� ����������*/
	virtual void SetDevice(VDrawingDevicePtr in_device);
	/** ������������� ������� ��������*/
	virtual void AdjustAreas() {};
	/** ��������� � ������*/
	virtual VRawData Render();
	/** ���������� ��� ������ ���������*/
	virtual void OnShow();
	/** ���������� � ���� ������ ������� �����*/
	virtual void SaveVisible(const char * const file_name, FILE_FORMAT format);
	/** ���������� ��������� ������� - ����������� � ��������*/
	virtual void SaveDocument(const char * const file_name, FILE_FORMAT format);
	/** ���������� PNG � �����*/
	virtual void SaveStreamPNG(std::ostream& in_outstream);
	/** ���������� ����� ������� ���������, �� ��������� ���������� 1. ������� ���������� � ��������*/
	virtual size_t GetPageCount(){return 1;};
	/** ���������� ������ �������� � ���. �����������*/
	virtual double GetPageSize() {return 0;};
	/** ��������� �� �������� � ��������� �������*/
	virtual void GoToPage(int in_page) {};
	/** ������������� ������ ������� �����*/
	virtual void SetVisibleSize(const double width, const double height);		
	/** ���������� ������� ������*/
	virtual double GetVisibleWidth() const {return visible_width;};
	/** ���������� ������� ������*/
	virtual double GetVisibleHeight() const {return visible_height;};
	/** ���������� true ���� ������� ��������� � ������� �����������*/
	bool Changed();
	/** �������� ����*/
	void AddLayer(const std::wstring& name);
	/** ���������� ��������� �� ����*/
	VLayerPtr GetLayer(const std::wstring& name);
	/** ���������� true, ���� ���� � ����� ������ ����������*/
	bool LayerExists(const std::wstring& name);
	/** ������� ����*/
	void RemoveLayer(const std::wstring& name);
	/** ������� ��� ����*/
	void ClearLayers();	
	/** ���������� true, ���� ���������� �������� ������*/
	virtual bool IsDataChanged();
	/** ��������� ��������*/
	void AddAction(VActionPtr in_action);
	/** ��������� ������ ���������� �������� ��� �����*/
	virtual void CreateContextMenu(double x, double y) {};

	/** �������� ������ �� �������*/
	virtual void Subscribe(uint32_t in_event_code, VEventHandlerPtr in_event_handler);	
	/** ���������� ������� ���� �����������*/
	virtual bool ProcessEvent(const VEvent& in_event);

	/** �������� ����*/
	virtual void OnClose();
	/** ���������� ������� �� �����������*/
	virtual double GetHorzPos(){return 0;};
	/** ���������� ���������� ��������� ������� �� �����������*/
	virtual double GetHorzMin(){return 0;};
	/** ���������� ����������� ��������� ������� �� �����������*/
	virtual double GetHorzMax(){return 0;};	
	
	/** ���������� � ������ �������*/
	virtual void ScrollToHorz(double in_value);

	
	/** ���������� ����� ��������� ��������*/
	virtual size_t GetActionCount() const {return current_actions.size();};		
	/** ���������� ��������� �� ��������*/
	virtual VActionPtr GetAction(size_t in_index)
	{
		return current_actions.at(in_index);
	};
	/** ���������� ����� ��������� �������� � ������ ������������*/
	virtual size_t GetToolbarActionCount() const {return toolbar_actions.size();};
	/** ���������� ��������� �� �������� � ������ ������������*/
	virtual VActionPtr GetToolbarAction(size_t in_index)
	{
		return toolbar_actions.at(in_index);
	};

};
typedef boost::shared_ptr<VDrawing> VDrawingPtr;


/// �������� PDF
class VPDFDocument
{
private:
	/// ����� cairo ��� PDF
	VCairoSurface surface;
	/// �������� Cairo ��� PDF
	VCairoContext context;
public:
	/// �����������
	VPDFDocument(const std::wstring& in_filename, double in_width, double in_height);
	/// ��������� ��������
	void Paint(cairo_surface_t * in_surface);
	/// ��������� ��������
	void AddPage(cairo_surface_t * in_surface);
	/// ���������� ��������
	cairo_t * GetContext();
	/// ��������� ��������
	void AddPage();
};
typedef boost::shared_ptr<VPDFDocument> VPDFDocumentPtr;

//��������������� ������� FixMe!!! �� ����� ���� ����� ������� ������ ������� ��� cairo_t � ������� ���������

/** ������������� ����*/
void CairoSetColor(cairo_t * in_cr, const VColor& in_color);

/** ������������� ������� �����*/
void CairoSetLineWidth(cairo_t * in_cr , double in_width);

/** ������������� �����*/
void CairoSetFont(cairo_t * in_cr, const VFont& in_font);

/** ������������� ����� ��������*/
void CairoSetDash(cairo_t * in_cr, double in_on=0, double in_off=0);

/** ������� �����*/
void CairoShowText(cairo_t * in_cr, const std::wstring& in_text);

/** ������� ����� ��������� ������*/
void CairoShowText(cairo_t * in_cr, const std::wstring& in_text, const VColor& in_color);