#pragma once

#include "VProfile.h"
#include "VPlan.h"
#include "oversteps.h"

class VDataSet;



//---------------------- ����� ���������������� ����������� (�����������!!!) ----------------------------

/// �������� ������� �����������
class condition_t : public VUnaryPredicate
{
private:
	range_t<double> values;	
	std::wstring description;
public:
	condition_t(){};
	condition_t(const range_t<double>& in_values, const range_t<double>& in_length, const std::wstring& in_description) :
		values(in_values), description(in_description) {};
	bool TestValue(double in_value) const {return values.Contains(in_value);};	
	std::wstring GetDescription() const {return description;};
};

/// ���������� ���������������� �������
#define NULL_CONDITION condition_t(range_t<double>(DBL_MAX, DBL_MAX), range_t<double>(0, 0), _T(""))


/// ������� �����������
enum JAGGY_OVERSTEP_DEGREE
{
	DEGREE_LOW,
	DEGREE_HIGH
};

/// ���������������� ����������
class VJaggyOverstep : public VBasicOverstep
{
private:
	/// ������� ����������
	JAGGY_OVERSTEP_DEGREE degree;
	/// �������� ������
	double otvod;
public:
	VJaggyOverstep();
	VJaggyOverstep(const VBasicOverstep& in_source);
	void SetDegree(JAGGY_OVERSTEP_DEGREE in_degree);
	void SetOtvod(double in_otvod);	
	std::wstring GetDescription() const;
	void CalcOtvod(VDataSet& ds, PARAMETER_TYPE type, double in_scale);
	
};

/// ������ ���������������� �������������� �����������
class VJaggyPlanOverstepList : public VIndexList<VJaggyOverstep>
{
private:
	/// ��������� �����������
	void AddOversteps(const VFindOverstep& in_oversteps, JAGGY_OVERSTEP_DEGREE in_degree);
public:
	/// ��������
	bool Load(VDataSet& ds);	
	/// �����
	size_t FindGreater(double in_abscoord);
	/// �����
	size_t FindLess(double in_abscoord);
};

/// ������ ���������������� ������������ �����������
class VJaggyProfileOverstepList : public VIndexList<VJaggyOverstep>
{
private:
	/// ��������� �����������
	void AddOversteps(const VFindOverstep& in_oversteps);
public:
	/// ��������
	bool Load(VDataSet& ds);
	/// �����
	size_t FindGreater(double in_abscoord);
	/// �����
	size_t FindLess(double in_abscoord);
};

/// ����������� (�����������)
class VOversteps : public VIndexList<VBasicOverstep>
{
private:				
public:
	/// ��������
	bool Load(VDataSet& ds);
	/// �����
	size_t FindGreater(double in_abscoord);
	/// �����
	size_t FindLess(double in_abscoord);
};


