#pragma once

#include <vector>
#include <string>
#include "os.h"
#include "types.h"
#include "VParameter.h"

class VDataSet;
class VAprinfo;
class VCurveList;
class VLineMarkList;
class VSidewayList;
class VAbsCoordData;
struct VWaySection;

/// ���� ��������� �����
enum PLAN_ELEM_TYPE
{
	PLAN_ELEM_LINE,			/// ������
	PLAN_ELEM_HALFLINE,		/// �������� ������
	PLAN_ELEM_CURVE,		/// ������
	PLAN_ELEM_HALFCURVE,	/// �������� ������
	PLAN_ELEM_SIDEWAY,		/// ������� ����
	PLAN_ELEM_RADIUS,		/// �������� ������
	PLAN_ELEM_INTERMEDIATE	/// ���������� ������
};

///���������� �������� ���� ��������
std::wstring GetPlanElementTypeName(PLAN_ELEM_TYPE in_type);

/// ������� �����
struct VPlanElement
{
	PLAN_ELEM_TYPE type;
	VWaySection section;
	double absStart, absEnd;
	uint32_t idxStart, idxEnd;

	//������ ������ (FixMe!!! �����������)
	size_t curve_index;
	/// ��������� ���������� � ������� ���������� �� �������
	void SetCoords(const VAbsCoordData & absCoord, const VAprinfo & aprInfo);
	/// ���������� true ���� ����� ��������� �� ��������
	bool HasPoint(double in_abscoord) const;
	/// ���������� ����� ��������
	double GetLength() const;
};

/// ������ ��������� �����
class VPlanElemList : public VIndexList<VPlanElement>
{
	
	// �������� �������� ������
	void AddLineElemList(size_t idxFrom, size_t idxTo, bool detailed_line,
		const VLineMarkList & linemarks, const VAprinfo & aprInfo, 
		const VAbsCoordData & absCoord);

public:

	/// �������� �� DataSet
	bool Load(VDataSet & ds, bool model, bool detailed_curve, bool detailed_line);

	/// �������� �� ��������� ���������
	bool Load(const VWaySection & sumsection,
		const VCurveList & curves, const VLineMarkList & linemarks, const VSidewayList & sideWays, 
		const VAprinfo & aprInfo, const VAbsCoordData & absCoord,
		bool detailed_curve, bool detailed_line);

	/// ������� ������� �� ������� ��������� �������� �����
	size_t Find(double in_abscoord) const;
	/// ���������� ������ ��������� �� ������� ��������� ����� � ��������� ��������
	std::vector<size_t> GetAffectedElements(size_t in_index) const;
};
