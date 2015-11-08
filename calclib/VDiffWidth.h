#pragma once

#include "VParameter.h"
#include "oversteps.h"

class VDataSet;

/// �������� �� � ��������� � ��� 
class VDiffWidth : public VParameter
{
public:
	/// ��������
	bool Load(VParameter & in_width, VParameter & in_width_front, VAbsCoordData & in_abscoord);
};


/// �������
class VDiffWidthOverstep : public VBasicOverstep
{
private:
	/// ����������
	double coord;
public:
	/// �����������
	VDiffWidthOverstep(const VExcess& in_excess, const VAprinfo& in_aprinfo);
	/// ��������� ��������
	std::wstring GetDescription() const;
	/// ���������� ����������	
	double GetCoord() const;
};


/// �������
class VDiffWidthOverstepList
{
private:	
	/// ������� ���� ��� ������ ���������
	bool loaded;	
	/// ������ �����������
	std::deque<VBasicOverstepPtr> oversteps;		
	
	/// ��������� ����������� � ������
	void AddOverstep(const VExcess& in_excess, const VAprinfo& in_aprinfo);
	/// ������
	void Calculate(VDataSet& in_ds, PARAMETER_TYPE in_data_type);	

public:
	/// �����������
	VDiffWidthOverstepList();
	/// ��������
	bool Load(VDataSet& in_ds, PARAMETER_TYPE in_data_type);
	/// ��������
	void Unload();
	/// ���������� ����� �������
	size_t GetCount() const;
	/// ���������� ������ �� ������
	const VBasicOverstepPtr& GetItem(size_t in_index) const;
};


