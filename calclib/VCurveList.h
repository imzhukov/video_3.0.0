#pragma once

#include "c4flib.h"
#include "VCurve.h"
#include "VParameter.h"

/// ������ ������
class VCurveList : public VIndexList<VCurve>
{
	/// �������� ����������� ������
	/// ����������� ������� CopyFrom()
	VCurveList & operator = (const VCurveList &);

	//������

	/// ������ �������� ������
	size_t active_curve_index;
	/// ��������� ����� �������� ������
	VCurve active_curve_bak;

	mutable size_t find_crv_cache_curve_index;
	mutable size_t find_crv_cache_record_index;
	mutable size_t find_lvl_cache_curve_index;
	mutable size_t find_lvl_cache_record_index;

	bool Load(const VBasicParameter & data, const VAbsCoordData & absCoord);

public:

	/// �����������
	VCurveList()
	{
		active_curve_index=BAD_INDEX;
		find_crv_cache_curve_index=BAD_INDEX;
		find_crv_cache_record_index=BAD_INDEX;
		find_lvl_cache_curve_index=BAD_INDEX;
		find_lvl_cache_record_index=BAD_INDEX;
	}

	/// �������� �� �����
	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, const VAbsCoordData & absCoord);

	/// ���������� �������������� ���������
	bool SetGeoCoord(const VDoubleParameter & latutude, const VDoubleParameter & longitude);

	/// ������ � ����
	bool Save(VMainFile & file, uint32_t type, const VAbsCoordData & absCoord);

	/// ���������� ������ �� ������� ������, � ������������� �������� � ����� ���.�-�
	bool CopyFrom(const VCurveList & src, const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs);

	// ������� ��������� ��������

	/// ������� ������ �� ������� ��������� ����� � �������� ��������
	size_t FindCrv(size_t in_index) const;
	/// ������� ������ �� ������� ��������� ����� � �������� ��������
	size_t FindLvl(size_t in_index) const;
	/// ������������� �������� ������
	void ActivateCurve(size_t in_active_curve_index);
	/// ������� �������� ������
	void DeleteActiveCurve();
	/// ���������� �������� ������
	void DeactivateCurve();
	/// ��������� ����� �������� ������
	void BackupActiveCurve();
	/// �������������� �������� ������ �� ��������� �����
	void RestoreActiveCurve();
	/// ���������� ������ �������� ������
	size_t GetActiveCurve() const;
	/// ���������� ��������� ��������
	float AproxPlan(size_t in_index, const VAbsCoordData & absCoord) const;
	/// ���������� ��������� ����������
	float AproxLevel(size_t in_index, const VAbsCoordData & absCoord) const;	
};

template <class FILETYPE>
bool VCurveList::Load(FILETYPE & file, uint32_t type, const VAbsCoordData & absCoord)
{
	if (this->IsLoaded()) return true;
	if (!absCoord.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	if (!file.Loaded())
	{
		this->message = ERRMSG_FILE_NOT_OPEN;
		return false;
	}

	VBasicParameter data;
	if (!data.Load(file, type, false)) 
	{
		this->message = data.GetErrorMessage();
		return false;
	}

	bool result = this->Load(data, absCoord);
	data.UnloadFile(file, type);
	data.Unload();
	return result;
}

class VAprinfo;

/// ������� ����� 
class VZLine : public VParameter
{

	struct Element
	{
		size_t idxBegin, idxEnd;
		double valueBegin, valueEnd;
	};

public:

	/// ��������
	/// bool level - �������� �� ������ (true) ��� � ����� (false)
	/// ��� aprInfo != NULL ������������ ����� ����������, ����� - ����
	bool Load(const VAbsCoordData & absCoord, const VCurveList & curves, 
		bool level, const VAprinfo * aprInfo);

};


/// ������������������ ������� (FixM�!!! ���������)
class VDiffLevel : public VParameter
{
public:
	/// ��������
	bool Load(const VCurveList& in_curves, const VParameter& in_level, const VAbsCoordData& in_abscoord, 
		const VAprinfo& in_aprinfo);
	/// ����������
	bool Update(const VCurveList& in_curves, const VParameter& in_level, const VAbsCoordData& in_abscoord, 
		const VAprinfo& in_aprinfo, size_t in_index);
};