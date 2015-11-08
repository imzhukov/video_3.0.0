#pragma once


#include "VParameter.h"
#include "oversteps.h"

class VDataSet;

//--------------------- ����������� �� ���������� --------------------------------

#define CANTING_LEVEL_THRESHOLD 85.0
#define CANTING_1_DIV_12 100.0/12.0
#define CANTING_1_DIV_30 100.0/30.0
#define CANTING_1_DIV_60 100.0/60.0

///�����������
class VCantingOverstep : public VBasicOverstep
{
private:
	/// �������
	double average_value;
public:
	/// �����������
	VCantingOverstep(const std::wstring& in_type, double in_start, double in_end, double in_min_value, double in_max_value, double in_average_value);
	/// ��������� ��������
	std::wstring GetDescription() const;
	/// ���������� �������
	double GetAverage() const;
};

///����������
class VCantingOverstepList
{
private:	
	/// ������� ���� ��� ������ ���������
	bool loaded;	
	/// ������ �����������
	std::deque<VBasicOverstepPtr> oversteps;		
	/// �������
	side_t side;
	/// ��������� ����������� � ������
	void AddOverstep(double start, double end, double min_value, double max_value, double average_value);
	/// ������
	void Calculate(VDataSet& in_ds);	

public:
	/// �����������
	VCantingOverstepList(side_t in_side);
	/// ��������
	bool Load(VDataSet& in_ds);
	/// ��������
	void Unload();
	/// ���������� ����� �������
	size_t GetCount() const;
	/// ���������� ������ �� ������
	const VBasicOverstepPtr& GetItem(size_t in_index) const;
};

/// ������� �����

///�����������
class VExtWearOverstep : public VBasicOverstep
{
private:
	/// �������
	double average_value;
public:
	/// �����������
	VExtWearOverstep(const std::wstring& in_type, double in_start, double in_end, double in_min_value, double in_max_value, double in_average_value);
	/// ��������� ��������
	std::wstring GetDescription() const;
	/// ���������� �������
	double GetAverage() const;
};

///������� �����
class VExtWearOverstepList
{
private:	
	/// ������� ���� ��� ������ ���������
	bool loaded;	
	/// ������ �����������
	std::deque<VBasicOverstepPtr> oversteps;		
	/// �������
	side_t side;
	/// ��������� ����������� � ������
	void AddOverstep(double start, double end, double min_value, double max_value, double average_value);
	/// ������
	void Calculate(VDataSet& in_ds);	

public:
	/// �����������
	VExtWearOverstepList(side_t in_side);
	/// ��������
	bool Load(VDataSet& in_ds);
	/// ��������
	void Unload();
	/// ���������� ����� �������
	size_t GetCount() const;
	/// ���������� ������ �� ������
	const VBasicOverstepPtr& GetItem(size_t in_index) const;
};