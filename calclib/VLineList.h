#pragma once
#include "VParameter.h"
#include "VAprinfo.h"
#include "VPlanElemList.h"
#include "VPlan.h"

// ����� �� ������
struct VLineMark
{
	uint32_t index;					// ������ ����� �����

	bool operator < (const VLineMark & other)
	{
		return this->index < other.index;
	}

	VLineMark() : index(NOT_FOUND)
	{}

	VLineMark(uint32_t __i__) : index(__i__)
	{}
};

// ������ ����� 
class VLineMarkList : public VIndexList<VLineMark>
{
	// �������� �� ����� ������
	bool Load(const VBasicParameter & data, 
		const VAbsCoordData & absCoord, const VAprinfo & aprInfo);

public:

	// �������� �� �����
	template <class FILETYPE> bool Load(FILETYPE & file, uint32_t type,
		const VAbsCoordData & absCoord, const VAprinfo & aprInfo);

	// �����������
	bool CopyFrom(const VLineMarkList & src, 
		const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs);

	// ���������� � ����
	bool VLineMarkList::Save(VMainFile & file, uint32_t type, 
		const VAbsCoordData & absCoord, const VAprinfo & aprInfo);

	std::vector<VLineMark> ExportMarks(size_t idxFrom, size_t idxTo) const;
};

//class VDataSet;

// ������ - ������� � ������ �����
class VLine
{
	// ������� ������ ������ � ����� ������
	uint32_t idxStart, idxEnd;		

public:

	// �����������
	VLine() : idxStart(NOT_FOUND), idxEnd(NOT_FOUND)
	{}

	VLine(uint32_t __s__, uint32_t __e__) : idxStart(__s__), idxEnd(__e__)
	{
	}

	// ���������� ������� ������
	void SetRanges(uint32_t __start, uint32_t __end)
	{
		this->idxStart = __start;
		this->idxEnd = __end;
	}

	// ������ ������ ������
	uint32_t GetStartIndex() const
	{
		return this->idxStart;
	}

	// ������ ����� ������
	uint32_t GetEndIndex() const
	{
		return this->idxEnd;
	}

	// ����� �������� �� ������
	bool Fits(const VLineMark & mark) const
	{
		return mark.index > this->GetStartIndex() && mark.index < this->GetEndIndex();
	}

	// ���������� �����
	double GetLength(const VAbsCoordData& in_abscoord)
	{
		return in_abscoord.GetItem(idxEnd) - in_abscoord.GetItem(idxStart);
	}

	// ���������� ���� (�� ��������� ������)
	double GetAngle(const VPlan& in_plan)
	{		
		line_t line(in_plan.GetItem(idxStart), in_plan.GetItem(idxEnd));
		return atan(line.k());
	}
};

// ������ ������
class VLineList : public VIndexList<VLine>
{
public:

	// �������� �� ��������� ���������
	bool Load(const VCurveList & curves, const VLineMarkList & linemarks, const VSidewayList & sideWays, 
		const VAprinfo & aprInfo, const VAbsCoordData & absCoord);	

	// ���������� ���� ����� �������� ������� ��� 0, ���� ������ �� �������
	double GetAngle(const VPlan& in_plan, size_t in_l, size_t in_r)
	{
		if (in_l+1==in_r && GetItem(in_l).GetEndIndex()==GetItem(in_r).GetStartIndex())
		{
			return GetItem(in_r).GetAngle(in_plan)-GetItem(in_l).GetAngle(in_plan);
		}
		else
		{
			return 0;
		}
	}
};

// �������� �� ������� ����� 
template <class FILETYPE>
bool VLineMarkList::Load(FILETYPE & file, uint32_t type,
						 const VAbsCoordData & absCoord, 
						 const VAprinfo & aprInfo)
{
	if (this->IsLoaded()) return true;

	if (!absCoord.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	if (!aprInfo.IsLoaded())
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
	if (data.Load(file, type, false))
	{
		return this->Load(data, absCoord, aprInfo);
	}
	else
	{
		this->message = ERRMSG_BLOCK_INCORRECT;
		this->SetLoaded(false);
		return false;
	}
}