#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include "VParameter.h"
#include "VAprinfo.h"
#include "VPlanElemList.h"
#include "VCurveList.h"
#include "oversteps.h"
#include <vector>

using namespace std;

/** ���������� ����� VDataSet*/
class VDataSet;

static point_t CalcNextPoint(const point_t& in_prev_point, double in_course, double in_delta_abs)
{
	return point_t(in_prev_point.x+sin(in_course)*in_delta_abs, in_prev_point.y+cos(in_course)*in_delta_abs);
}

/** ������ ��������� ���� � �����*/
class VPlan : public VParameterProto<point_t>
{
private:

	//������

	/** ������ ������*/
	vector<point_t> data;
	/** ������� ����, ��� ������ ��������� � ����������*/
	bool is_loaded;

public:
	/** �����������*/
	VPlan()
	{};

	/** ����������*/
	~VPlan()
	{
		this->Unload();
	};

	/** ��������*/
	void Unload();

	/** �������� � ������*/
	bool Load(VDataSet& ds);	

	/** ����� �������*/
	size_t GetCount() const 
	{
		return data.size();
	};

	/** ������� ���������� ������ �� i-� �������*/
	const point_t & GetItem(size_t in_index) const 
	{
		return data.at(in_index);
	};

	/** ������� ������ �� i-� �������*/
	point_t & GetItem(size_t in_index) 
	{
		return data.at(in_index);
	};	
};

/** "���������" ����*/
class VIdealPlan : public VParameterProto<point_t>
{
protected:

	//������

	/** ������ ������*/
	vector<point_t> data;

	/** ������� ����, ��� ��� ���������� ����*/
	bool is_model;


	//������

	/** ������ ����� ��� ������*/
	void CalcCurvePlan(const VCurve & curve, const VParameter & in_course, 
		const VAbsCoordData & in_abscoord);
	/** ������ ����� ��� ������*/
	void CalcLinePlan(const VPlanElement & plan_element, const VPlan & in_plan, const VAbsCoordData & in_abscoord);	
	/** �������� ���������� ����*/
	void CopyMeasuredPlan(const VPlanElement & plan_element, const VPlan & in_plan);
	/** �������� ����� �� ��������*/
	void CorrectPlan(const VPlanElement & in_plan_element, const VPlan & in_plan);
	/** ����������� ����� ������� �����*/
	void Bezier(const VPlanElemList& in_plan_elem_list, size_t in_first, size_t in_last);

public:

	/** �����������*/
	VIdealPlan(bool in_is_model) : is_model(in_is_model)
	{};

	/** ����������*/
	~VIdealPlan() 
	{
		Unload();
	};

	/** ��������*/
	void Unload();

	/** �������� � ������*/
	bool Load(VDataSet& ds);

	/** �������� � ������*/
	bool Load(const VParameter& in_course, const VPlan& in_plan, 
		const VAbsCoordData& in_abscoord, const VPlanElemList& in_plan_elements, 
		const VCurveList& in_curves);

	/** ����������*/
	void Update(VDataSet& ds, size_t in_first_element_index, size_t in_last_element_index);

	/** ����� �������*/
	size_t GetCount() const 
	{
		return data.size();
	};

	/** ������� ���������� ������ �� i-� �������*/
	const point_t & GetItem(size_t in_index) const 
	{
		return data.at(in_index);
	};

	/** ������� ������ �� i-� �������*/
	point_t & GetItem(size_t in_index) 
	{
		return data.at(in_index);
	};	
};

/// ���� �� ����������� ��������
class VIdealPlanReal : public VIdealPlan
{
public:
	///�����������
	VIdealPlanReal() : VIdealPlan(false){};
};

/// ���� �� ���������� ��������
class VIdealPlanModel : public VIdealPlan
{
public:
	///�����������
	VIdealPlanModel() : VIdealPlan(true){};
};

/// ���������� ����� �� ����������
class VPlanOffset : public VParameter
{	
protected:
	///������� ��������� ������ ������ - �����������/����������
	bool is_model;
public:
	/// �����������
	VPlanOffset(bool in_is_model) : is_model(in_is_model){};
	/// ��������
	bool Load(const VPlan& in_plan, const VIdealPlan& in_ideal_plan);
	/// ��������
	bool Load(VDataSet& ds);
	/// ����������
	bool Update(VDataSet& ds, size_t in_start, size_t in_end);
};

/// ���������� ����� �� ��������� �� ����������� �������� 
class VPlanOffsetReal : public VPlanOffset
{
public:
	///�����������
	VPlanOffsetReal() : VPlanOffset(false) {};
};

/// ���������� ����� �� ��������� �� ���������� ��������
class VPlanOffsetModel : public VPlanOffset
{
public:
	///�����������
	VPlanOffsetModel() : VPlanOffset(true) {};
};

/// ������������ ��������� �����������
class VAnpReal : public VParameter
{
public:
	/// ��������
	bool Load(VDataSet& ds, speed_type in_speed);
};

/// �������� ���������� ������������� ��������� �����������
class VPsiReal : public VParameter
{
public:
	///��������
	bool Load(VDataSet& ds, speed_type in_speed);
};

/// ������������ ��������� �� ������
class VAnpModel : public VParameterProto<VAnp>
{
public:
	/// ��������
	bool Load(const VAbsCoordData & in_abscoord, const VCurveList & in_curves, const VAprinfo& in_aprinfo, speed_type in_speed, bool in_use_passport_speeds=true);
	///��������
	void Unload();
	/// ����� �������
	virtual size_t GetCount() const;
	/// ������� ���������� ������ �� i-� �������
	virtual const VAnp & GetItem(size_t index) const;
	/// ������� ������ �� i-� �������
	virtual VAnp & GetItem(size_t index);
	/// ���������� ����������� ���
	VAnp & GetMin();
	/// ���������� ������������ ���
	VAnp & GetMax();
private:
	///������
	std::vector<VAnp> data;
};


/// ���������� ����� (���������� �� ���������)
class VJaggyPlan : public VParameter
{
public:
	/// ��������
	bool Load(VDataSet& ds, bool lines_only);
};



/// ������� ���������
enum JAGGY_PLAN_SIGNIFICANCE {PLAN_SIGN_1=1, PLAN_SIGN_2=2};

/// ���������������� ����������
class VJaggyPlanOverstep : public VBasicOverstep
{
private:
	/// ������� ���������
	JAGGY_PLAN_SIGNIFICANCE significance;
	/// ���������� FixMe!!! � ������� �����
	double coord;
	/// �������� ������
	double otvod;
	/// ������� �����
	bool on_bridge;
	/// ������� �������
	bool on_switch;
	/// ������� �������
	bool on_station;
public:
	/// �����������
	VJaggyPlanOverstep(double in_start, double in_end, double in_coord, double in_value, 
		double in_otvod, JAGGY_PLAN_SIGNIFICANCE in_significance, const VAprinfo& in_aprinfo);
	/// ���������� ����������
	double GetCoord() const;
	/// ���������� ������������ �����
	double GetOtvod() const;
	/// ��������� ��������
	std::wstring GetDescription() const;
	/// ���������� ������ � ��������� �������
	std::wstring GetDegreeString() const;
};
typedef boost::shared_ptr<VJaggyPlanOverstep> VJaggyPlanOverstepPtr;


/// ���������������� ���������� �����
class VJaggyPlanOverstepList
{
private:
	/// ������� ���� ��� ������ ���������
	bool loaded;
	/// ������ �����������
	std::deque<VJaggyPlanOverstepPtr> oversteps;

	/// ��������� ����������� � ������
	void AddOverstep(double start, double end, double center, double value, double otvod, const VAprinfo& aprinfo);	
	/// ������
	void Calculate(VDataSet& in_ds);

public:
	/// �����������
	VJaggyPlanOverstepList();
	/// ��������
	bool Load(VDataSet& in_ds);
	/// ��������
	void Unload();
	/// ���������� ����� �������
	size_t GetCount() const;
	/// ���������� ������ �� ������
	const VJaggyPlanOverstepPtr& GetItem(size_t in_index) const;
};