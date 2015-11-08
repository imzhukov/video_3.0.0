#pragma once

#include "VDrawing.h"
#include "axes.h"
#include "dblib.h"
#include "VParamCorrections.h"

/// ��� ������ �������
enum SWITCH_GLYPH_TYPE
{
	SW_UNDEFINED=0,
	SW_LEFT_ALONG=1,
	SW_LEFT_COUNTER=2,
	SW_RIGHT_ALONG=3,
	SW_RIGHT_COUNTER=4
};

/// ��������� ��� ������ ��������
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

/** ���������� � �������*/
class VGraphInfo
{
private:
	/** ��� �������*/
	std::wstring graph_name;
	/** ����� ������*/
	std::vector<std::wstring> file_names;
	/** ������ ��������� �����*/
	size_t active_file_index;
	/** �������� �������*/
	std::wstring way_section;	
public:
	/** �����������*/
	VGraphInfo(const std::wstring in_graph_name, const std::vector<VDataSetPtr>& in_datasets, 
		const VDataSetPtr& active_data_set, const std::wstring& in_actual_dir, 
		const std::wstring& in_actual_way, const std::wstring& in_actual_date);
	/** ���������� ��� �������*/
	const std::wstring& GetGraphName() const;
	/** ���������� ����� ������*/
	const std::vector<std::wstring>& GetFileNames() const;
	/** ���������� ������ ��������� �����*/
	size_t GetActiveFileIndex() const;
	/** ���������� �������� �������*/
	const std::wstring& GetWaySection() const;
};
typedef boost::shared_ptr<VGraphInfo> VGraphInfoPtr;

/** ��������� ����*/
class VMousePositionData : public VXOrderedDataProvider
{
private:
	double x;
	double y;
public:
	/** �����������*/	
	VMousePositionData(){};	
	/** ���������� ������ ������*/
	size_t GetCount() const {return 1;};	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const {return x;};	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const {return y;};	
	/** ���������� ����� ��� �����, �������� �����*/
	virtual uint64_t GetFlags(size_t in_index) const {return 0;};	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY() {return y;};
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY() {return y;};
	/** ���������� ����������� �������� �� X*/
	double GetMinX() {return x;};
	/** ���������� ������������ �������� �� X*/
	double GetMaxX() {return x;};	
	/** ���������� ���������*/
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

/** ���������*/
class VYConstantData : public VDataProvider
{
private:
	/** ��������*/	
	double value;	
	/** ������������� �����*/
	std::wstring label;
	/** ��� ���������*/
	VAxesWeakPtr axes;
public:
	/** �����������*/	
	VYConstantData(VAxesWeakPtr in_axes, double in_value, const std::wstring& in_label) : 
	  axes(in_axes), value(in_value), label(in_label){};
	/** ���������� ������ ������*/
	size_t GetCount() const {return 2;};	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const 
	{
		if (VAxesPtr a=axes.lock())
			return (in_index==0) ? a->GetMinX() : a->GetMaxX();
		else
			return 0;		
	};	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const {return value;};		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY() {return value;};
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY() {return value;};
	/** ���������� ����������� �������� �� X*/
	double GetMinX() {return GetX(0);};
	/** ���������� ������������ �������� �� X*/
	double GetMaxX() {return GetX(1);};
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const {return label;};
};
typedef boost::shared_ptr<VYConstantData> VYConstantDataPtr;

/** �������*/
class VYRangeData : public VDataProvider
{
private:
	/** ����������� ��������*/	
	double min_value;
	/** ������������ ��������*/	
	double max_value;	
	/** ��� ���������*/
	VAxesWeakPtr axes;
public:
	/** �����������*/	
	VYRangeData(VAxesWeakPtr in_axes, double in_min_value, double in_max_value) : axes(in_axes), min_value(in_min_value), max_value(in_max_value){};	
	/** ���������� ������ ������*/
	size_t GetCount() const {return 1;};	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const 
	{
		if (VAxesPtr a=axes.lock())
			return a->GetMinX();
		else
			return 0;
	};	
	/** ���������� ���������� �� ��� X*/
	double GetSizeX(size_t in_index) const 
	{
		if (VAxesPtr a=axes.lock())
			return a->GetMaxX()-a->GetMinX();
		else
			return 0;
	};
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const {return min_value;};
	/** ���������� ���������� �� ��� X*/
	double GetSizeY(size_t in_index) const {return max_value-min_value;};
	/** ���������� ����������� �������� �� Y*/
	double GetMinY() {return min_value;};
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY() {return max_value;};
	/** ���������� ����������� �������� �� X*/
	double GetMinX() {return GetX(0);};
	/** ���������� ������������ �������� �� X*/
	double GetMaxX() {return GetX(0)+GetSizeX(0);};		
};
typedef boost::shared_ptr<VYRangeData> VYRangeDataPtr;

/** �����*/
class VYOptionData : public VDataProvider
{
private:
	/** ��� �����*/	
	std::wstring option_name;
	/// ��� ���������
	VAxesWeakPtr axes;
public:
	/** �����������*/	
	VYOptionData(VAxesWeakPtr in_axes, const std::wstring in_option_name) : axes(in_axes), option_name(in_option_name)
	{
		//��������� ������� �����
		if (!OPTIONS.HasOption(option_name))
		{
			throw VSimpleException(L"������������ ��������", in_option_name);
		}
	};	
	/** ���������� ������ ������*/
	size_t GetCount() const {return 2;};	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const 
	{
		if (VAxesPtr a=axes.lock())
			return (in_index==0) ? a->GetMinX() : a->GetMaxX();
		else
			return 0;
	};	
	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const {return OPTIONS.GetOption(option_name).GetValue<double>();};	
	/** ���������� ����� ��� �����, �������� �����*/
	virtual uint64_t GetFlags(size_t in_index) const {return 0;};	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY() {return OPTIONS.GetOption(option_name).GetValue<double>();};
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY() {return OPTIONS.GetOption(option_name).GetValue<double>();};
	/** ���������� ����������� �������� �� X*/
	double GetMinX() 
	{
		return GetX(0);
	};
	/** ���������� ������������ �������� �� X*/
	double GetMaxX() 
	{
		return GetX(1);
	};	
};
typedef boost::shared_ptr<VYOptionData> VYOptionDataPtr;

/** ������������ ������� - �����*/
class VSymmetricYRangeOptionData : public VDataProvider
{
private:
	/** ��� �����*/	
	std::wstring option_name;
	/// ��� ���������
	VAxesWeakPtr axes;
public:
	/** �����������*/	
	VSymmetricYRangeOptionData(VAxesWeakPtr in_axes, const std::wstring in_option_name) : axes(in_axes), option_name(in_option_name)
	{
		//��������� ������� �����
		if (!OPTIONS.HasOption(option_name))
		{
			throw VSimpleException(L"������������ ��������", in_option_name);
		}
	};	
	/** ���������� ������ ������*/
	size_t GetCount() const {return 1;};	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const 
	{
		if (VAxesPtr a=axes.lock())
			return a->GetMinX();
		else
			return 0;
	};
	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const
	{
		if (VAxesPtr a=axes.lock())
			return a->GetMaxX()-a->GetMinX();
		else
			return 0;
	};	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const {return -OPTIONS.GetOption(option_name).GetValue<double>();};
	/** ���������� ������ �� ��� Y*/
	double GetSizeY(size_t in_index) const {return 2*OPTIONS.GetOption(option_name).GetValue<double>();};	
	/** ���������� ����� ��� �����, �������� �����*/
	virtual uint64_t GetFlags(size_t in_index) const {return 0;};	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY() {return -OPTIONS.GetOption(option_name).GetValue<double>();
	};
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY() {return OPTIONS.GetOption(option_name).GetValue<double>();
	};
	/** ���������� ����������� �������� �� X*/
	double GetMinX() {return GetX(0);};
	/** ���������� ������������ �������� �� X*/
	double GetMaxX() {return GetX(0)+GetSizeX(0);};	
};
typedef boost::shared_ptr<VSymmetricYRangeOptionData> VSymmetricYRangeOptionDataPtr;

/** ������� ������*/ 
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

/** �������� ������ - ������� ���������� (������� ����������, ������� � ����� 10 ������)*/
class VCoordinatesData : public VXOrderedDataProvider
{	
private:		
	/** ���������� ����������*/
	std::deque<double> xdata;
	/** �������*/
	std::deque<double> xsizedata;
	/** ����� (��������: ��������, �����)*/
	std::deque<uint64_t> flags;
	/** �����*/
	std::deque<std::wstring> label;
public:		
	/** �����������*/	
	VCoordinatesData(const VAprinfo& in_aprinfo, pro::interval<double> in_range);	
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� X*/
	double GetSizeX(size_t in_index) const;
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� ����� ��� �����, �������� �����*/
	virtual uint64_t GetFlags(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VCoordinatesData> VCoordinatesDataPtr;

/** ������ ��� �������*/
class VParameterData : public VXOrderedDataProvider
{	
private:	
	/** ��� ������*/
	PARAMETER_TYPE data_type;	
	/** ��������� �� �������� ������*/
	VDataSetPtr data_set;	
public:		
	/** �����������*/	
	VParameterData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� ����� ��� �����, �������� �����*/
	virtual uint64_t GetFlags(size_t in_index) const;
	/** ������������� �����, �������� �����*/
	virtual void SetFlags(size_t in_index, uint64_t in_flags);
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();				
	/** ���������� �������� Y �� X (����� ��������� ������, �� ��������� �� ��������)*/
	virtual double GetYbyX(double in_x);
	/** ���������� ������� �������*/
	virtual std::vector<size_t> GetVisible(double in_left, double in_top, double in_right, double in_bottom);
};
typedef boost::shared_ptr<VParameterData> VParameterDataPtr;

/** �������� ������ - �������*/
class VParameterFunctionData : public VXOrderedDataProvider
{	
private:	
	/** �������� ������*/
	VDataSetPtr data_set;	
	/** ��������*/
	VParameter parameter;
public:		
	/** �����������*/	
	VParameterFunctionData(VDataSetPtr in_data_set, const std::wstring& in_function);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� ����� ��� �����, �������� �����*/
	virtual uint64_t GetFlags(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();				
	/** ���������� �������� Y �� X (����� ��������� ������, �� ��������� �� ��������)*/
	virtual double GetYbyX(double in_x);
	/** ���������� ������� �������*/
	virtual std::vector<size_t> GetVisible(double in_left, double in_top, double in_right, double in_bottom);
};
typedef boost::shared_ptr<VParameterFunctionData> VParameterFunctionDataPtr;

/** �������� ������*/
class VCurrentSpeedData : public VXOrderedDataProvider
{	
private:		
	/** ��������� �� �������� ������*/
	VDataSetPtr data_set;
	/** ������� ������� ���������*/
	bool empty;	
public:		
	/** �����������*/	
	VCurrentSpeedData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();				
	/** ���������� �������� Y �� X (����� ��������� ������, �� ��������� �� ��������)*/
	virtual double GetYbyX(double in_x);	
};
typedef boost::shared_ptr<VCurrentSpeedData> VCurrentSpeedDataPtr;

/** ����� �����*/
class VZazorLeftData : public VXOrderedDataProvider
{
private:			
	/** ������*/
	VDataSetPtr data_set;	
public:		
	/** �����������*/	
	VZazorLeftData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;
	/** ���������� ����� ��� �����, �������� �����*/
	uint64_t GetFlags(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();			
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VZazorLeftData> VZazorLeftDataPtr;

/** ����� ������*/
class VZazorRightData : public VXOrderedDataProvider
{
private:			
	/** ������*/
	VDataSetPtr data_set;	
public:		
	/** �����������*/	
	VZazorRightData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;
	/** ���������� ����� ��� �����, �������� �����*/
	uint64_t GetFlags(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();			
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VZazorRightData> VZazorRightDataPtr;

/** ����� ��������� �����*/
class VMagZazorLeftData : public VXOrderedDataProvider
{
private:			
	/** ������*/
	VDataSetPtr data_set;	
public:		
	/** �����������*/	
	VMagZazorLeftData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;
	/** ���������� ����� ��� �����, �������� �����*/
	uint64_t GetFlags(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();			
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VMagZazorLeftData> VMagZazorLeftDataPtr;

/** ����� ��������� ������*/
class VMagZazorRightData : public VXOrderedDataProvider
{
private:			
	/** ������*/
	VDataSetPtr data_set;	
public:		
	/** �����������*/	
	VMagZazorRightData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;
	/** ���������� ����� ��� �����, �������� �����*/
	uint64_t GetFlags(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();			
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VMagZazorRightData> VMagZazorRightDataPtr;

/** ���� �����*/
class VUgonLeftData : public VXOrderedDataProvider
{
private:			
	/** ������*/
	VDataSetPtr data_set;	
public:		
	/** �����������*/	
	VUgonLeftData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;
	/** ���������� ����� ��� �����, �������� �����*/
	uint64_t GetFlags(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();			
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VUgonLeftData> VUgonLeftDataPtr;

/** ���� ������*/
class VUgonRightData : public VXOrderedDataProvider
{
private:			
	/** ������*/
	VDataSetPtr data_set;	
public:		
	/** �����������*/	
	VUgonRightData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;
	/** ���������� ����� ��� �����, �������� �����*/
	uint64_t GetFlags(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();			
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
};
typedef boost::shared_ptr<VUgonRightData> VUgonRightDataPtr;



/** ����� ����*/
class VSpalaMarkerData : public VXOrderedDataProvider
{
private:	
	/** ������*/
	VDataSetPtr data_set;
	/** ��� ������ ��� ��������� �� ��� Y*/
	PARAMETER_TYPE data_type;
public:		
	/** �����������*/	
	VSpalaMarkerData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();			
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
};
typedef boost::shared_ptr<VSpalaMarkerData> VSpalaMarkerDataPtr;

/** �������� ������ - ����� ���������, ��� ���������� �� Y ����� ���� ��������� � ���������� ���������*/
class VOperatorMarkData : public VXOrderedDataProvider
{
private:
	/** ��������� �� ������*/
	VDataSetPtr data_set;
	/** ��� ������ ��� ��������� �� ��� Y*/
	PARAMETER_TYPE data_type;	
public:
	/** �����������*/
	VOperatorMarkData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type=PRM_NONE);
	/** ���������� ����� �����*/
	size_t GetCount() const;
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
	/** �������� �����*/
	void SelectPoint(size_t in_index);
	/** ���������� ���������� �����*/
	size_t GetSelectedPoint() const;
	/** ������� ���������*/
	void DeselectPoint();
};
typedef boost::shared_ptr<VOperatorMarkData> VOperatorMarkDataPtr;

/** ������� �� ����*/
class VWayMarkerData : public VXOrderedDataProvider
{
private:	
	/** ������*/
	VDataSetPtr data_set;
	/** ��� ������ ��� ��������� �� ��� Y*/
	PARAMETER_TYPE data_type;
public:		
	/** �����������*/	
	VWayMarkerData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();		
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
};
typedef boost::shared_ptr<VWayMarkerData> VWayMarkerDataPtr;


/** ������ ��� �������� ����� ������� (FixMe!!! �������� ������)*/
class VProfileMarkData : public VXOrderedDataProvider
{
private:	

	/** �������� �� ��� Y*/
	std::vector<float> ydata;
	/** �������� �� ��� X*/
	std::vector<double> xdata;	
	
public:
	/** �����������*/
	VProfileMarkData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();
};

/** ������ ��� �������� ������� (���������) FixMe!!! �������� �������������*/
class VProfileElementsData : public VXOrderedDataProvider
{
private:	

	/** ��������� �� �������� ������ � �����*/
	VDataSetPtr data_set;	
	/** ����������� �����*/
	bool drag;
	/** ������ ������������ �����*/
	size_t drag_index;	
public:
	/** �����������*/
	VProfileElementsData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;			
	/** ������������� �������� X*/
	void SetXY(size_t in_index, double in_x, double in_y);	
	/** ��������� �����*/
	void AddXY(size_t in_index, double in_x, double in_y, double in_size_x=0, double in_size_y=0, 
		const std::wstring& in_label=L"", uint64_t in_flags=0);
	/** ������� �����*/
	void DeleteItem(size_t in_index);	
	/** ������� ��� �����*/
	void DeleteAll();	
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
	/** ������������� ����*/
	virtual void SetFlags(size_t in_index, uint64_t in_flags);
	/** ���������� ���� (������� ���� ��������)*/
	virtual uint64_t GetFlags(size_t in_index) const;		
};
typedef boost::shared_ptr<VProfileElementsData> VProfileElementsDataPtr;

#define SLOPE_ZERO 0
#define SLOPE_POSITIVE 1
#define SLOPE_NEGATIVE 2
#define SLOPE_HILL 3
#define SLOPE_PIT 4

/** �������� ������ - ������ �������*/
class VSlopeData : public VXOrderedDataProvider
{
private:
	/** ������*/
	VDataSetPtr data_set;
	/** ��� �������*/
	PARAMETER_TYPE data_type;	
public:
	/** �����������*/
	VSlopeData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;		
	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const;	
	/** ���������� ������ �� ��� Y*/
	double GetSizeY(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
	/** ���������� ����*/
	uint64_t GetFlags(size_t in_index) const;	
	/** ����������� �������� ������*/
	void SetType(PARAMETER_TYPE in_data_type);
	/** ���������� ��� ����������� ������*/
	PARAMETER_TYPE GetType();
	/** ���������� ��������� ������*/
	const void * GetUserData(size_t in_index) const;
};
typedef boost::shared_ptr<VSlopeData> VSlopeDataPtr;

/** �������� ������ - ���� ����� ���������� �������*/
class VSlopeAngleData : public VXOrderedDataProvider
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ��� �������*/
	PARAMETER_TYPE data_type;
public:
	/** �����������*/
	VSlopeAngleData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;			
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ����������� �������� ������*/
	void SetType(PARAMETER_TYPE in_data_type);
	/** ���������� ��� ����������� ������*/
	PARAMETER_TYPE GetType();
};
typedef boost::shared_ptr<VSlopeAngleData> VSlopeAngleDataPtr;


/** �������� ������ - ������� ������� �� ���������/����������/�������*/
class VHeightData : public VXOrderedDataProvider
{
private:
	/** ������*/
	VDataSetPtr data_set;
	/** ��� �������*/
	PARAMETER_TYPE data_type;	
public:
	/** �����������*/
	VHeightData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;			
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
	/** ���������� ����*/
	uint64_t GetFlags(size_t in_index) const;	
	/** ����������� �������� ������*/
	void SetType(PARAMETER_TYPE in_data_type);
	/** ���������� ��� ����������� ������*/
	PARAMETER_TYPE GetType();
};
typedef boost::shared_ptr<VHeightData> VHeightDataPtr;

/** �������� ������ - ������������ ��������� ���������*/
class VAnpModelData : public VXOrderedDataProvider
{
private:	

	/** ��������� �� �������� ������ � �����*/
	VDataSetPtr data_set;	
	/** ��� ��������*/
	speed_type speed;
	/// �������� ���������
	double sign_y;

public:
	/** �����������*/	
	VAnpModelData(VDataSetPtr in_data_set, speed_type in_speed, bool in_inverse);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();		
};
typedef boost::shared_ptr<VAnpModelData> VAnpModelDataPtr;

/** �������� ������ ���������� � ���������� ������*/
class VCurvesInfo
{
private:
	/** ������*/
	VDataSetPtr data_set;	
	/** ������� ������/����*/
	bool is_model;
public:
	/** �����������*/
	VCurvesInfo(VDataSetPtr in_data_set, bool in_model=false);
	/** ���������� ����� ������*/
	size_t GetCount() const;
	/** ���������� ������*/
	const VCurve& GetCurve(size_t in_index) const;
	/** ���������� ��������� ������*/
	bool Valid(size_t in_index);
	/** ���������� ������ ������*/
	double GetAbsStart(size_t in_index) const;
	/** ���������� ����� ������*/
	double GetAbsEnd(size_t in_index) const;
	/** ���������� ������ ������ (�������)*/
	double GetAbsLevelStart(size_t in_index) const;
	/** ���������� ����� ������ (�������)*/
	double GetAbsLevelEnd(size_t in_index) const;
	/** ���������� ����������*/
	long GetOrientation(size_t in_index) const;
	/** ���������� ����� ��������*/
	size_t GetRadiusCount(size_t in_index) const;
	/** ���������� ������ �������*/
	double GetAbsStart(size_t in_index, size_t in_radius_index) const;
	/** ���������� ����� �������*/
	double GetAbsEnd(size_t in_index, size_t in_radius_index) const;
	/** ���������� ������*/
	double GetRadius(size_t in_index, size_t in_radius_index) const;
	/** ���������� ����������*/
	double GetLevel(size_t in_index, size_t in_radius_index) const;
	/** ���������� ����� ��������*/
	double GetLength(size_t in_index, size_t in_radius_index) const;
	/** ���������� ����� ������ ����������*/
	double GetLength1(size_t in_index, size_t in_radius_index) const;
	/** ���������� ����� ������ ����������*/
	double GetLength2(size_t in_index, size_t in_radius_index) const;
	/** ���������� ���� ��������*/
	double GetAngle(size_t in_index, size_t in_radius_index) const;
	/** ���������� ����� ������� ���������� ������ �������*/
	double GetMStart(size_t in_index, size_t in_radius_index) const;
	/** ���������� ����� ������� ���������� ����� �������*/
	double GetMEnd(size_t in_index, size_t in_radius_index) const;
	/** ���������� ������ ������*/
	std::wstring GetStatus(size_t in_index) const;
	/** ���������� true ���� ������� ������ - ��������*/
	bool GetElementType(size_t in_index, size_t in_element_index) const;
	/** ���������� ����� �������� ������*/
	double GetElementLength(size_t in_index, size_t in_element_index) const;
	/** ���������� ������ �������� ������*/
	double GetElementRadius(size_t in_index, size_t in_element_index) const;
	/** ���������� ����� �������� ������ (�������)*/
	double GetElementLengthLevel(size_t in_index, size_t in_element_index) const;
	/** ���������� ���������� �������� ������*/
	double GetElementLevel(size_t in_index, size_t in_element_index) const;
	/** ���������� V��*/
	double GetModelVkr_high_bt(size_t in_index, size_t in_element_index) const;	
	/** ���������� V��*/
	double GetModelVkr_high(size_t in_index, size_t in_element_index) const;
	/** ���������� V��*/
	double GetModelVkr_pass(size_t in_index, size_t in_element_index) const;	
	/** ���������� V��*/
	double GetModelVkr_truck(size_t in_index, size_t in_element_index) const;
	/** ���������� V��*/
	double GetModelViz_high_bt(size_t in_index, size_t in_element_index) const;
	/** ���������� V��*/
	double GetModelViz_high(size_t in_index, size_t in_element_index) const;
	/** ���������� V��*/
	double GetModelViz_pass(size_t in_index, size_t in_element_index) const;
	/** ���������� V��*/
	double GetModelVpr(size_t in_index, size_t in_element_index) const;
	/** ���������� ����� ����������*/
	double GetModelTap(size_t in_index, size_t in_element_index) const;
	/** ���������� ���*/
	double GetModelPsi_high_bt(size_t in_index, size_t in_element_index) const;
	/** ���������� ���*/
	double GetModelPsi_high(size_t in_index, size_t in_element_index) const;
	/** ���������� ���*/
	double GetModelPsi_pass(size_t in_index, size_t in_element_index) const;
	/** ���������� ���*/
	double GetModelPsi_truck(size_t in_index, size_t in_element_index) const;
	/** ���������� ���*/
	double GetModelPsi_avg(size_t in_index, size_t in_element_index) const;
	/** ���������� A�� ����. ��*/
	double GetModelAnpHighBT(size_t in_index, size_t in_element_index) const;	
	/** ���������� A�� ����.*/
	double GetModelAnpHigh(size_t in_index, size_t in_element_index) const;	
	/** ���������� A�� ����.*/
	double GetModelAnpPass(size_t in_index, size_t in_element_index) const;	
	/** ���������� A�� ����.*/
	double GetModelAnpTruck(size_t in_index, size_t in_element_index) const;	
	/** ���������� A�� �����.*/
	double GetModelAnpAvg(size_t in_index, size_t in_element_index) const;
	/** ���������� Vhigh_bt*/
	speed_t GetModelVhigh_bt(size_t in_index) const;
	/** ���������� Vhigh*/
	speed_t GetModelVhigh(size_t in_index) const;
	/** ���������� Vpass*/
	speed_t GetModelVpass(size_t in_index) const;
	/** ���������� Vcrg*/
	speed_t GetModelVcrg(size_t in_index) const;
	/** ���������� Vavg*/
	speed_t GetModelVavg(size_t in_index) const;
	/** ���������� ������������ ���������� � �����*/
	double GetMaxPlanOverstep(size_t in_index);
};
typedef boost::shared_ptr<VCurvesInfo> VCurvesInfoPtr;

/** �������� ������ - ������ �������*/
class VLinesData : public VXOrderedDataProvider
{
private:
	/** ������*/
	VDataSetPtr data_set;
public:
	/** �����������*/
	VLinesData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� ������ "�����" �� X*/
	double GetSizeX(size_t in_index) const;
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
};
typedef boost::shared_ptr<VLinesData> VLinesDataPtr;

/** �������� ������ - ���� �������� �����*/
class VLineAngleData : public VXOrderedDataProvider
{
private:
	/** ������*/
	VDataSetPtr data_set;
public:
	/** �����������*/
	VLineAngleData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
};
typedef boost::shared_ptr<VLineAngleData> VLineAngleDataPtr;

/** �������� ������ - ������ �� ������� ����*/
class VSidewayData : public VDataProvider
{
private:
	/** ������*/
	VDataSetPtr data_set;
public:
	/** �����������*/
	VSidewayData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� ������ "�����" �� X*/
	double GetSizeX(size_t in_index) const;
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;

	/** ��������� ������� ����*/
	void AddXY(size_t in_index, double in_x, double in_y, double in_size_x=0, double in_size_y=0, 
		const std::wstring& in_label=L"", uint64_t in_flags=0);
	/** ������� ������� ����*/
	void DeleteItem(size_t in_index);
	/** ������������� ��� �������*/
	void SetFlags(size_t in_index, uint64_t in_flags);
};
typedef boost::shared_ptr<VSidewayData> VSidewayDataPtr;

/** ��������� ������*/
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


/** �������� ������ - ��*/
class VQueryData : public VXOrderedDataProvider
{
private:
	/** ��������� �� ��*/
	VIBDatabasePtr database;
	/** ��������� �� ����������*/
	VIBTransactionPtr transaction;
	/** ����� �������*/
	std::wstring query;
	/** ������*/
	std::vector<VPointData> data;

	double min_x;
	double max_x;
	double min_y;
	double max_y;

public:
	/** �����������*/
	VQueryData(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, const std::wstring& in_query);
	/** ��������� ������*/
	virtual void RefreshData();
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;
	/** ���������� ������ "�����" �� X (����� ����� ��� ����������� ��������)*/
	double GetSizeX(size_t in_index) const;
	/** ���������� ������ "�����" �� Y (����� ����� ��� ����������� ��������)*/
	double GetSizeY(size_t in_index) const;
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
	/** ���������� ���� - ��� �����*/
	uint64_t GetFlags(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();		
};

/** ������ � ������� ���������� �������*/
class VSelectionInfo : public VActiveObject
{
private:
	/** ������� ��������� - ��������� x ������ �����*/
	double select_start_x;
	/** ������� ��������� - ��������� y ������ �����*/
	double select_start_y;
	/** ������� ��������� - ��������� x ������ �����*/
	double select_end_x;
	/** ������� ��������� - ��������� y ������ �����*/
	double select_end_y;
	/** ���� ��� ����� ��������*/
	bool hit_left;
	/** ���� ��� ������ ��������*/
	bool hit_right;
	/** ������� ���� ��� ���������� ���������*/
	bool selecting;	
	/** ������� ���� ��� ���� ��� ����� �� ������*/
	bool hit;
public:
	/** �����������*/
	VSelectionInfo();
	/** ������ ���������*/
	void SetStart(double in_x, double in_y);
	/** ����� ���������*/
	void SetEnd(double in_x, double in_y);
	/** ���������� X ���������� ��������� �����*/
	double GetStartX() const;
	/** ���������� Y ���������� ��������� �����*/
	double GetStartY() const;
	/** ���������� X ���������� �������� �����*/
	double GetEndX() const;
	/** ���������� Y ���������� �������� �����*/
	double GetEndY() const;	
	/** ����� �������� � 0*/
	void Reset();
	/** ���������� true, ���� ������� ���������*/
	bool Valid() const;	
	/** ������������� ������� ����, ��� ���������� ���������*/
	void SetSelecting(bool in_selecting);
	/** ���������� ������� ����, ��� ���������� ���������*/
	bool GetSelecting() const;	
	/** ������������� ������� ����������*/
	void SetActive(bool in_active);
	
	/** ������������� ������� ���� ��� ���� ��� ����� �� ������*/
	void SetHit(bool in_hit){hit=in_hit;};
	/** ���������� ������� ���� ��� ���� ��� ����� �� ������*/
	bool GetHit() const {return hit;};

};
typedef boost::shared_ptr<VSelectionInfo> VSelectionInfoPtr;

/** ������ � ������� ��������*/
class VPageInfo
{
private:
	/** ������� ��������*/
	size_t page;
public:
	/** ���������� ������� ��������*/
	size_t GetPage() const;
	/** ������������� ������� ��������*/
	void SetPage(size_t in_page);
};
typedef boost::shared_ptr<VPageInfo> VPageInfoPtr;

//----------------- ������ ������� ����� ������ -------------------------

//����� �����
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



/** �������� ������ - ����� ������ �� ��������*/
class VCurveMarksData : public VXOrderedDataProvider
{
private:		
	/** �������� ������*/
	VDataSetPtr data_set;	
	/** ����� ������*/
	mutable size_t curves_count;
	/** ����� ����� �������� ������*/
	mutable size_t active_mark_count;
	/** ���*/
	mutable std::vector<index_cache_t> cache;	
	/** ��� ������ ��������/�������*/
	PARAMETER_TYPE data_type;
	/** ��� �������� ����/�������*/
	PARAMETER_TYPE mark_type;
	/** ����������� �������� ������ � ���� - ������/�����*/
	index_cache_t IndexToPair(size_t in_index) const;
	/** ���������� ����� ����� ������*/
	size_t GetMarkCount(size_t in_index) const;
	/** ���������� ������ �� �����*/
	VCurveMark& GetMark(size_t in_curve_index, size_t in_mark_index);
	/** ���������� ������ �� �����*/
	const VCurveMark& GetMark(size_t in_curve_index, size_t in_mark_index) const;
	/** ��������� �����*/
	bool AddMark(VCurve& in_curve, size_t in_index, float in_value);	
	/** ������� �����*/
	void DeleteMark(size_t in_index);
	/** ����������� �����*/
	void NormalizeMarks();
	/** ��������� ���*/
	void BuildCache() const;	
	
public:
	/** �����������*/
	VCurveMarksData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type=PRM_CURV, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;
	/** ���������� ���� - ��� �����*/
	uint64_t GetFlags(size_t in_index) const;
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
	/** ������������� �������� X*/
	void SetXY(size_t in_index, double in_x, double in_y);	
	/** ��������� �����*/
	void AddXY(size_t in_index, double in_x, double in_y, double in_size_x=0, double in_size_y=0, 
		const std::wstring& in_label=L"", uint64_t in_flags=0);
	/** ������� �����*/
	void DeleteItem(size_t in_index);	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();		

	/** �������� �����*/
	void SelectPoint(size_t in_index);
	/** ���������� ���������� �����*/
	size_t GetSelectedPoint() const;
	/** ������� ���������*/
	void DeselectPoint();

	/** �������� ������ �����*/
	virtual void MarkPoints(size_t in_from, size_t in_to);
	/** ������� �������*/
	virtual void UnmarkPoints();

	/** ���������� ������� �������*/
	virtual std::vector<size_t> GetVisible(double in_left, double in_top, double in_right, double in_bottom);
};
typedef boost::shared_ptr<VCurveMarksData> VCurveMarksDataPtr;

class VCoordPointData : public VXOrderedDataProvider
{
private:
	/** ��������� �� �������� ������*/
	VDataSetPtr data_set;
	/** ��������� �� ��� ��������*/
	POINT_TYPE point_type;
	/* ������� ������ � ����� � ����� ��������������� ����*/
	std::vector<size_t> index;
public:
	/** �����������*/	
	VCoordPointData(VDataSetPtr in_data_set, VIBDatabasePtr in_database, POINT_TYPE in_type);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
	/*�������� �����*/
	void SelectPoint(size_t in_index);
	/*���������� ���������� �����*/
	size_t GetSelectedPoint() const;
	/*������� ���������*/
	void DeselectPoint();
	/* �������� �������*/
	//void DeleteItem(size_t in_index);
	/* ������� ������ � ������     */
	/* ����� ��� ������� ��������� */
	void RefreshData();
};

class VLine_tData : public VXOrderedDataProvider
{
private:
	/** ��������� �� �������� ������*/
	VDataSetPtr data_set;
	/*��� ������*/
	POINT_TYPE point_type;
public:
	/** �����������*/
	VLine_tData(VDataSetPtr in_data_set, POINT_TYPE type);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();
};

/** ���� ���������� ������*/
enum CURVE_PARAM_TYPE
{
	ANP_HIGH,
	ANP_HIGH_BT,
	ANP_PASS,
	ANP_TRUCK,
	PLAN_OFFSET
};

/** �������� ������ - �������� ������ ������*/
class VCurveParameterData : public VXOrderedDataProvider
{
private:	
	/** ��������� �� ������*/
	VDataSetPtr data_set;
	/** ������ ������*/
	size_t curve_index;
	/** ������*/
	std::vector<double> ydata;	
	/** ������ ������ ������*/
	size_t start_index;
public:
	/** �����������*/
	VCurveParameterData(VDataSetPtr in_data_set, size_t in_curve_index, CURVE_PARAM_TYPE in_type, 
		PARAMETER_TYPE in_mark_type);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();		
};
typedef boost::shared_ptr<VCurveParameterData> VCurveParameterDataPtr;


/** �������� ������ ������� �� ������� ����*/
class VMainWayObjectData : public VXOrderedDataProvider
{
private:
	/** ��������� �� �������*/
	VDataSetPtr data_set;
	/** ��������� �� ��*/
	VIBDatabasePtr database;
	/** ��������� �� ����������*/
	VIBTransactionPtr transaction;
	/** ������*/
	std::wstring query;
	/** ���������� ������*/
	VIBDataHandlerParameter handler;
	
public:

	/** �����������*/
	VMainWayObjectData(VIBDatabasePtr in_database, VIBTransactionPtr transction, const std::wstring& in_query, 
		VDataSetPtr in_data_set);
	/** ��������� ������*/
	void RefreshData();
	/** ���������� ������ ������*/
	size_t GetCount() const;
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� ����� (ID �����)*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VMainWayObjectData> VMainWayObjectDataPtr;


//-------------------------------------- ��� ����������� ����� -------------------------------------------


/** �������� ������ - ������� �� �������*/
class VStationProfileData : public VXOrderedDataProvider
{
private:
	
	/*
	FixMe!!!
	1. ������������ VGraphPointData
	2. �������� ���. �����.
	*/

	/// ������
	std::vector<point_t> data;				

public:

	/** �����������*/
	VStationProfileData(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, 
		VDataSetPtr in_data_set);	
	/** ���������� ������ ������*/
	size_t GetCount() const;
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();		
};
typedef boost::shared_ptr<VStationProfileData> VStationProfileDataPtr;


/** �������� ������ ����� ��������� �� �������*/
class VStationOperatorMarkData : public VXOrderedDataProvider
{
private:
	/** ��������� �� �������*/
	VDataSetPtr data_set;
	/** ��������� �� ��*/
	VIBDatabasePtr database;
	/** ����������*/
	VIBTransactionPtr transaction;
	/** ������ ID ����*/
	std::wstring trail_id;
	/** ���������� ������*/
	VIBDataHandlerParameter handler;	
	
public:

	/** �����������*/
	VStationOperatorMarkData(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, 
		VDataSetPtr in_data_set, const std::wstring& in_trail_id);
	/** ��������� ������*/
	void RefreshData();
	/** ���������� ������ ������*/
	size_t GetCount() const;
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� ����� (ID �����)*/
	uint64_t GetFlags(size_t in_index) const;
	/** ���������� true ���� ����� ����� ������. �� ��������� ����� ������ ����� �����*/
	bool CanChange(size_t in_index) const;
	/** ��������� �����*/
	void AddXY(size_t in_index, double in_x, double in_y, double in_size_x=0, double in_size_y=0,
		const std::wstring& in_label=L"", uint64_t in_flags=0);
	/** �������� �����*/
	void SetXY(size_t in_index, double in_x, double in_y);
	/** ������� �����*/	
	void DeleteItem(size_t in_index);
	/** �������� �����*/
	void SelectPoint(size_t in_index);
	/** ���������� ���������� �����*/
	size_t GetSelectedPoint() const;
	/** ������� ���������*/
	void DeselectPoint();
};
typedef boost::shared_ptr<VStationOperatorMarkData> VStationOperatorMarkDataPtr;

/** �������� ������ ������� �� ����������� ����*/
class VStationWayObjectData : public VXOrderedDataProvider
{
private:
	/** ��������� �� �������*/
	VDataSetPtr data_set;
	/** ��������� �� ��*/
	VIBDatabasePtr database;
	/** ��������� �� ����������*/
	VIBTransactionPtr transaction;
	/** ������ ID ����*/
	std::wstring trail_id;
	/** ���������� ������*/
	VIBDataHandlerParameter handler;
	/// ����, �������� ����������� ������� �� �������� ����
	bool show_all;

public:

	/** �����������*/
	VStationWayObjectData(VIBDatabasePtr in_database, VIBTransactionPtr transction, VDataSetPtr in_data_set, bool in_show_all);
	/** ��������� ������*/
	void RefreshData();
	/** ���������� ������ ������*/
	size_t GetCount() const;
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;
	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� ����� (ID �����)*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VStationWayObjectData> VStationWayObjectDataPtr;


/** �������� ������ ������������ ������ �� ����������� ����*/
class VStationWayKmData : public VXOrderedDataProvider
{
private:
	/** ��������� �� �������*/
	VDataSetPtr data_set;
	/** ��������� �� ��*/
	VIBDatabasePtr database;
	/** ��������� �� ����������*/
	VIBTransactionPtr transaction;
	/** ������ ID ����*/
	std::wstring trail_id;
	/** ���������� ������*/
	VIBDataHandlerParameter handler;
	
public:

	/** �����������*/
	VStationWayKmData(VIBDatabasePtr in_database, VIBTransactionPtr transction, VDataSetPtr in_data_set);
	/** ��������� ������*/
	void RefreshData();
	/** ���������� ������ ������*/
	size_t GetCount() const;
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;		
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� ����� (ID �����)*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VStationWayKmData> VStationWayKmDataPtr;


///��������� ��� ����������������� ��������
class VPredefinedQueries
{
private:	

	/// ������ ��������
	std::map<std::wstring, std::wstring> queries;
	typedef std::pair<std::wstring, std::wstring> query_t;
	typedef std::map<std::wstring, std::wstring>::const_iterator query_itr;
	/// ������ ������
	std::wstring zero;

public:
	///�����������
	VPredefinedQueries();
	///���������� ������� �� ��������	
	const std::wstring& GetCommand(const std::wstring& in_description) const;
};
static const VPredefinedQueries PredefinedQueries;

/** �������� ������*/
class VLineMarkData : public VXOrderedDataProvider
{
private:	

	/** ��������� �� �������� ������ � �����*/
	VDataSetPtr data_set;	
	/** ����������� �����*/
	bool drag;
	/** ������ ������������ �����*/
	size_t drag_index;	
public:
	/** �����������*/
	VLineMarkData(VDataSetPtr in_data_set);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;			
	/** ������������� �������� X*/
	void SetXY(size_t in_index, double in_x, double in_y);	
	/** ��������� �����*/
	void AddXY(size_t in_index, double in_x, double in_y, double in_size_x=0, double in_size_y=0, 
		const std::wstring& in_label=L"", uint64_t in_flags=0);
	/** ������� �����*/
	void DeleteItem(size_t in_index);	
	/** ������� ��� �����*/
	void DeleteAll();
	/** �������������� ��������*/
	void Automark();
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VLineMarkData> VLineMarkDataPtr;

/** ����������� � �������� �������*/
class VRTOverstepData : public VDataProvider
{
private:		
	/** ������ �����������*/
	std::vector<VBasicOverstepPtr> oversteps;	
public:
	/** �����������*/
	VRTOverstepData();
	/** ��������� ������*/
	void Put(const VBasicOverstepPtr in_overstep);
	/** ������� ������*/
	void Clear();
	/** ���������� ������ ������*/
	size_t GetCount() const;
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;
	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const;	
	/** ���������� ������ �� ��� Y*/
	double GetSizeY(size_t in_index) const;
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
	/** ���������� ����� (�������)*/
	uint64_t GetFlags(size_t in_index) const;
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VRTOverstepData> VRTOverstepDataPtr;

/// ������� ������ �������
#define START_EVENT 1
/// ������� ����� �������
#define END_EVENT -1
/// ������� - �������� ������
#define SING_EVENT 0
/// ������� - �����������
#define COMMENT_EVENT 2


/** �������*/
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

	/** ������ �������*/
	std::vector<VRTEvent> data;	
public:	
	/** �����������*/
	VRTEventInfoData();
	/** ��������� �����������*/
	void Put(const VBasicOverstepPtr in_overstep);
	/** ��������� �������*/
	void Put(double in_x, const std::wstring& in_coord, const std::wstring& in_description, int in_flag);
	/** ��������� �������*/
	void Sort();
	/** ������� ������*/
	void Clear();
	/// �������� ������� ��� ������������
	void SetDrawn(size_t in_index);
	/** �������  ������������ �������*/
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
	/** ���������� ������ ������*/
	size_t GetCount() const;
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� �����*/
	uint64_t GetFlags(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VRTEventInfoData> VRTEventInfoDataPtr;


/** ����������� ���������� �������� � ���� ������*/
class VCorrelationInfo : public VTextProvider
{
	private:
		/** �������� ������ 1*/
		VDataSetPtr data_set1;
		/** �������� ������ 2*/
		VDataSetPtr data_set2;
		/** ��������*/
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
				text=boost::str(boost::wformat(L"����������: %0.4f") % correlation);
			}			
		};

	public:	
		/** �����������*/
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
		/** ���������� ����� �����*/
		virtual size_t GetCount() const
		{
			return 1;
		}
		/** ���������� ����������� ����������*/
		virtual const std::wstring& GetItem(size_t in_index) const
		{
			Update();
			return text;
		};
};
typedef boost::shared_ptr<VCorrelationInfo> VCorrelationInfoPtr;

//---------------------------------------------- ��������� ������ �� aprinfo -------------------------------------

/** �������� ������ - ������� �������� �� �������*/
class VAprinfoSpeedSectionData : public VXOrderedDataProvider
{
private:	
	/** ������ ���������*/
	std::vector<double> xdata;	
	/** ������ ���� ��������*/
	std::vector<double> xsize;	
	/** ������ �����*/
	std::vector<std::wstring> labels;
public:
	/** ���c�������*/
	VAprinfoSpeedSectionData(const VAprinfo& in_aprinfo, double in_start, double in_end);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;

	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const;	
	/** ���������� ������ �� ��� Y*/
	double GetSizeY(size_t in_index) const;

	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();
};
typedef boost::shared_ptr<VAprinfoSpeedSectionData> VAprinfoSpeedSectionDataPtr;

/** ����� ������� � Aprinfo*/
template<class T> struct find_object
{
	double coord;	
	find_object(double in_coord) : coord(in_coord){};
	bool operator()(const T& in_object) const
	{
		return in_object.base<=coord && in_object.end>=coord;
	};
};

/** �������� ������ - ���������� �������*/
class VSwitchData : public VXOrderedDataProvider
{
private:	
	/** ������ �������*/
	std::deque<VAprinfoSwitchRecord> data;
public:
	/** ���c�������*/
	VSwitchData();
	/// ��������� �������
	void Put(const VAprinfoSwitchRecord& in_data);
	/// ������� ������
	void Clear();
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
	/** ���������� �����*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VSwitchData> VSwitchDataPtr;


/** �������� ������ - ������� �������*/
class VSwitchSectionsData : public VXOrderedDataProvider
{
private:	
	/** ������ �������*/
	std::deque<VAprinfoSwitchRecord> data;
public:
	/** ���c�������*/
	VSwitchSectionsData();
	/// ��������� �������
	void Put(const VAprinfoSwitchRecord& in_data);
	/// ������� ������
	void Clear();
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const;
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
	/** ���������� �����*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VSwitchSectionsData> VSwitchSectionsDataPtr;


/** �������� ������ - ����� �� aprinfo*/
class VAprinfoBridgeData : public VXOrderedDataProvider
{
private:	
	/** ������ �������*/
	std::vector<VAprinfoIndexRecord> data;
public:
	/** ���c�������*/
	VAprinfoBridgeData(const VAprinfo& in_aprinfo, bool in_include_areas);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const;	
	/** ���������� ������ �� ��� Y*/
	double GetSizeY(size_t in_index) const;
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoBridgeData> VAprinfoBridgeDataPtr;


/** �������� ������ - ������������� ������� �� aprinfo*/
class VAprinfoChamferedJointData : public VXOrderedDataProvider
{
private:	
	/** ������ �������*/
	std::vector<VAprinfoObjectRecord> data;
public:
	/** ���c�������*/
	VAprinfoChamferedJointData(const VAprinfo& in_aprinfo);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const;	
	/** ���������� ������ �� ��� Y*/
	double GetSizeY(size_t in_index) const;
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoChamferedJointData> VAprinfoChamferedJointDataPtr;


/** �������� ������ - ������� �� aprinfo*/
class VAprinfoTunnelData : public VXOrderedDataProvider
{
private:	
	/** ������ �������*/
	std::vector<VAprinfoIndexRecord> data;
public:
	/** ���c�������*/
	VAprinfoTunnelData(const VAprinfo& in_aprinfo, bool in_include_areas);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const;	
	/** ���������� ������ �� ��� Y*/
	double GetSizeY(size_t in_index) const;
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoTunnelData> VAprinfoTunnelDataPtr;


/** �������� ������ - �������� �� aprinfo*/
class VAprinfoCrossingData : public VXOrderedDataProvider
{
private:	
	/** ������ �������*/
	std::vector<VAprinfoCrossing> data;
public:
	/** ���c�������*/
	VAprinfoCrossingData(const VAprinfo& in_aprinfo);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const;	
	/** ���������� ������ �� ��� Y*/
	double GetSizeY(size_t in_index) const;
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoCrossingData> VAprinfoCrossingDataPtr;


/** �������� ������ - ��� ���� �� aprinfo*/
class VAprinfoSpalaData : public VXOrderedDataProvider
{
private:	
	/** ������ �������*/
	std::vector<VAprinfoIndexRecord> data;
public:
	/** ���c�������*/
	VAprinfoSpalaData(const VAprinfo& in_aprinfo);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const;	
	/** ���������� ������ �� ��� Y*/
	double GetSizeY(size_t in_index) const;
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
	/** ���������� ����� (ID �����)*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VAprinfoSpalaData> VAprinfoSpalaDataPtr;


/** �������� ������ - ����������� ���� �� ������ �� aprinfo*/
class VAprinfoStraightSideData : public VXOrderedDataProvider
{
private:	
	/** ������ �������*/
	std::vector<VAprinfoIndexRecord> data;
public:
	/** ���c�������*/
	VAprinfoStraightSideData(const VAprinfo& in_aprinfo, bool is_passport);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;	
	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const;	
	/** ���������� ������ �� ��� Y*/
	double GetSizeY(size_t in_index) const;
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;	
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
	/** ���������� ����� (ID �����)*/
	uint64_t GetFlags(size_t in_index) const;	
};
typedef boost::shared_ptr<VAprinfoStraightSideData> VAprinfoStraightSideDataPtr;


/** �������� ������ - ����� ���������� (������� �����) �� aprinfo*/
class VAprinfoNormaLevelData : public VXOrderedDataProvider
{
private:		
	/** ������*/
	std::vector<double> xdata;
	std::vector<double> ydata;
public:
	/** ���c�������*/
	VAprinfoNormaLevelData(const VAprinfo& in_aprinfo);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;			
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoNormaLevelData> VAprinfoNormaLevelDataPtr;


/** �������� ������ - ������� ����� �������� �� aprinfo*/
class VAprinfoNormaRichData : public VXOrderedDataProvider
{
private:		
	/** ������*/
	std::vector<double> xdata;
	std::vector<double> ydata;
public:
	/** ���c�������*/
	VAprinfoNormaRichData(const VAprinfo& in_aprinfo);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;			
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoNormaRichData> VAprinfoNormaRichDataPtr;

/** �������� ������ - ������� ����� �� �� aprinfo*/
class VAprinfoNormaWidthData : public VXOrderedDataProvider
{
private:		
	/** ������*/
	std::vector<double> xdata;
	std::vector<double> ydata;
public:
	/** ���c�������*/
	VAprinfoNormaWidthData(const VAprinfo& in_aprinfo);
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;			
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VAprinfoNormaWidthData> VAprinfoNormaWidthDataPtr;


/** �������� ������ - ����������� ������������ ������*/
class VDequeData : public VDataProvider
{
private:		
	/** ������*/	
	std::deque<VGraphPointData> data;
	/** ������� Y*/
	double min_y;
	/** �������� Y*/
	double max_y;
public:
	/** ���c�������*/
	VDequeData();
	/** ���������� �����*/
	void Put(double in_x, double in_y, uint64_t in_flags);
	/** ���������� �����*/
	void Put(double in_x, double in_size_x, double in_y, double in_size_y, const std::wstring& in_label, uint64_t in_flags);
	/** ��������� ������*/
	void Clear();
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;		
	/** ���������� ������ �� ��� X*/
	double GetSizeX(size_t in_index) const;	
	/** ���������� ������ �� ��� Y*/
	double GetSizeY(size_t in_index) const;		
	/** ���������� �����*/
	std::wstring GetLabel(size_t in_index) const;
	/** ���������� �����*/
	uint64_t GetFlags(size_t in_index) const;
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VDequeData> VDequeDataPtr;


/** �������� ������ - ������������ ������*/
class VVectorData : public VDataProvider
{
private:		
	/** ������*/
	std::vector<double> xdata;
	std::vector<double> ydata;
	std::vector<uint64_t> flags;
public:
	/** ���c�������*/
	VVectorData();
	/** ������������� ������*/ 
	void SetSize(size_t in_size);
	/** ���������� �����*/
	void Set(size_t in_index, double in_x, double in_y, uint64_t in_flags);
	/** ��������� ������*/
	void Clear();
	/** ���������� ������ ������*/
	size_t GetCount() const;	
	/** ���������� ���������� �� ��� X*/
	double GetX(size_t in_index) const;	
	/** ���������� ���������� �� ��� Y*/
	double GetY(size_t in_index) const;		
	/** ���������� �����*/
	uint64_t GetFlags(size_t in_index) const;
	/** ���������� ����������� �������� �� Y*/
	double GetMinY();
	/** ���������� ������������ �������� �� Y*/
	double GetMaxY();
	/** ���������� ����������� �������� �� X*/
	double GetMinX();
	/** ���������� ������������ �������� �� X*/
	double GetMaxX();	
};
typedef boost::shared_ptr<VVectorData> VVectorDataPtr;


/** ������������ �����*/
class VUserTextProvider : public VTextProvider
{
private:
	std::vector<std::wstring> items;
public:
	/** ���������� ����� �����*/
	virtual size_t GetCount() const;
	/** ���������� ������*/
	virtual const std::wstring& GetItem(size_t in_index) const;
	/** ��������� ������*/
	void AddItem(const std::wstring& in_line);
	/** ������� ��� ������*/ 
	void Clear();
};
typedef boost::shared_ptr<VUserTextProvider> VUserTextProviderPtr;


/// �����������
class VJaggyProfileOverstepData : public VXOrderedDataProvider
{
private:
	/// ��������� �� �������� ������ � �����
	VDataSetPtr data_set;	
public:
	/// �����������
	VJaggyProfileOverstepData(VDataSetPtr in_data_set);
	/// ���������� ������ ������
	size_t GetCount() const;	
	/// ���������� ���������� �� ��� X
	double GetX(size_t in_index) const;	
	/// ���������� ������ �� ��� X
	double GetSizeX(size_t in_index) const;
	/// ���������� ���������� �� ��� Y
	double GetY(size_t in_index) const;		
	/// ���������� ������ �� ��� Y
	double GetSizeY(size_t in_index) const;	
	/// ���������� ����������� �������� �� Y
	double GetMinY();
	/// ���������� ������������ �������� �� Y
	double GetMaxY();
	/// ���������� ����������� �������� �� X
	double GetMinX();
	/// ���������� ������������ �������� �� X
	double GetMaxX();
	/// ���������� �����
	std::wstring GetLabel(size_t in_index) const;
};

/// �����������
class VJaggyPlanOverstepData : public VXOrderedDataProvider
{
private:
	/// ��������� �� �������� ������ � �����
	VDataSetPtr data_set;	
public:
	/// �����������
	VJaggyPlanOverstepData(VDataSetPtr in_data_set);
	/// ���������� ������ ������
	size_t GetCount() const;	
	/// ���������� ���������� �� ��� X
	double GetX(size_t in_index) const;	
	/// ���������� ������ �� ��� X
	double GetSizeX(size_t in_index) const;
	/// ���������� ���������� �� ��� Y
	double GetY(size_t in_index) const;		
	/// ���������� ������ �� ��� Y
	double GetSizeY(size_t in_index) const;	
	/// ���������� ����������� �������� �� Y
	double GetMinY();
	/// ���������� ������������ �������� �� Y
	double GetMaxY();
	/// ���������� ����������� �������� �� X
	double GetMinX();
	/// ���������� ������������ �������� �� X
	double GetMaxX();
	/// ���������� �����
	std::wstring GetLabel(size_t in_index) const;
};

/// �����������
class VDiffWidthFrontOverstepData : public VXOrderedDataProvider
{
private:
	/// ��������� �� �������� ������ � �����
	VDataSetPtr data_set;	
public:
	/// �����������
	VDiffWidthFrontOverstepData(VDataSetPtr in_data_set);
	/// ���������� ������ ������
	size_t GetCount() const;	
	/// ���������� ���������� �� ��� X
	double GetX(size_t in_index) const;	
	/// ���������� ������ �� ��� X
	double GetSizeX(size_t in_index) const;
	/// ���������� ���������� �� ��� Y
	double GetY(size_t in_index) const;		
	/// ���������� ������ �� ��� Y
	double GetSizeY(size_t in_index) const;	
	/// ���������� ����������� �������� �� Y
	double GetMinY();
	/// ���������� ������������ �������� �� Y
	double GetMaxY();
	/// ���������� ����������� �������� �� X
	double GetMinX();
	/// ���������� ������������ �������� �� X
	double GetMaxX();
	/// ���������� �����
	std::wstring GetLabel(size_t in_index) const;
};

/// �����������
class VDiffWidthBackOverstepData : public VXOrderedDataProvider
{
private:
	/// ��������� �� �������� ������ � �����
	VDataSetPtr data_set;	
public:
	/// �����������
	VDiffWidthBackOverstepData(VDataSetPtr in_data_set);
	/// ���������� ������ ������
	size_t GetCount() const;	
	/// ���������� ���������� �� ��� X
	double GetX(size_t in_index) const;	
	/// ���������� ������ �� ��� X
	double GetSizeX(size_t in_index) const;
	/// ���������� ���������� �� ��� Y
	double GetY(size_t in_index) const;		
	/// ���������� ������ �� ��� Y
	double GetSizeY(size_t in_index) const;	
	/// ���������� ����������� �������� �� Y
	double GetMinY();
	/// ���������� ������������ �������� �� Y
	double GetMaxY();
	/// ���������� ����������� �������� �� X
	double GetMinX();
	/// ���������� ������������ �������� �� X
	double GetMaxX();
	/// ���������� �����
	std::wstring GetLabel(size_t in_index) const;
};

/// �����������
class VCantingOverstepData : public VXOrderedDataProvider
{
private:
	/// ��������� �� �������� ������ � �����
	VDataSetPtr data_set;
	side_t side;
public:
	/// �����������
	VCantingOverstepData(VDataSetPtr in_data_set, side_t in_side);
	/// ���������� ������ ������
	size_t GetCount() const;	
	/// ���������� ���������� �� ��� X
	double GetX(size_t in_index) const;	
	/// ���������� ������ �� ��� X
	double GetSizeX(size_t in_index) const;
	/// ���������� ���������� �� ��� Y
	double GetY(size_t in_index) const;		
	/// ���������� ������ �� ��� Y
	double GetSizeY(size_t in_index) const;	
	/// ���������� ����������� �������� �� Y
	double GetMinY();
	/// ���������� ������������ �������� �� Y
	double GetMaxY();
	/// ���������� ����������� �������� �� X
	double GetMinX();
	/// ���������� ������������ �������� �� X
	double GetMaxX();
	/// ���������� �����
	std::wstring GetLabel(size_t in_index) const;
};


/// �����������
class VExtWearOverstepData : public VXOrderedDataProvider
{
private:
	/// ��������� �� �������� ������ � �����
	VDataSetPtr data_set;
	side_t side;
public:
	/// �����������
	VExtWearOverstepData(VDataSetPtr in_data_set, side_t in_side);
	/// ���������� ������ ������
	size_t GetCount() const;	
	/// ���������� ���������� �� ��� X
	double GetX(size_t in_index) const;	
	/// ���������� ������ �� ��� X
	double GetSizeX(size_t in_index) const;
	/// ���������� ���������� �� ��� Y
	double GetY(size_t in_index) const;		
	/// ���������� ������ �� ��� Y
	double GetSizeY(size_t in_index) const;	
	/// ���������� ����������� �������� �� Y
	double GetMinY();
	/// ���������� ������������ �������� �� Y
	double GetMaxY();
	/// ���������� ����������� �������� �� X
	double GetMinX();
	/// ���������� ������������ �������� �� X
	double GetMaxX();
	/// ���������� �����
	std::wstring GetLabel(size_t in_index) const;
};
