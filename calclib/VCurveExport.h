#pragma once

#include "VDataSet.h"
#include "dblib.h"
#include "sqlgen.h"
#include <iomanip>

int GetWearDegree(double wear);

class VCurveExport
{
	const CURVE_TYPE crvType;
	const __int64 trailId;
	const __int64 repairId;

	const bool optics; /// ������
	bool hs, hs_bt; /// ������� ��������

	VDataSetPtr dataSet;
	VIBDatabasePtr dataBase;
	VIBTransaction & transaction;
	VIBDataHandlerParameter handler;
	VSQLGenerator sqlgen;

	std::stringstream sql;
	std::wstring message;

	// ���������� �� ��������
	enum TABLE
	{
		TABLE_TRAIL, TABLE_PLAN_ELEM, TABLE_CURVE, TABLE_MARK, TABLE_RADIUS, 
		TABLE_SPDAREA, TABLE_SPDAREA_HS, TABLE_SPDAREA_HS_BT,
		TABLE_RADIUS_SPDAREA, TABLE_RADIUS_SPDAREA_HS, TABLE_RADIUS_SPDAREA_HS_BT,
		TABLE_PICKET_SPDLIM, TABLE_PICKET_WEAR,
		TABLE_LONG_RAILS, TABLE_WEAR_DENSITY
	} tables;
	std::string GetTable(TABLE table) const;

	// ��������� sql-������
	bool ExecuteStatement(VSQLGenerator & sqlgen);
	bool ExecuteStatement(const std::string & sql);

	// ����� ������� �������� ���������
	bool ExportSpdAreas(const VCurve & curve, TABLE table, TABLE parent_table, 
		__int64 parent_id, size_t radiusNdx);

	// ������� �������� ������� ��������� [� �������� ������]
	bool ExportHiSpdAreas(const VCurve & curve, TABLE table, TABLE parent_table, bool bt,
		__int64 parent_id, size_t radiusNdx);

	// ������ ���� �������� �����
	std::string GetPlanElemTypeSQL(PLAN_ELEM_TYPE planElemType) const;

public:

	VCurveExport(CURVE_TYPE ct, VDataSetPtr ds, VIBDatabasePtr db, VIBTransaction & trans, 
		__int64 trail, __int64 repair, bool optics);

	// ������� ������
	bool ClearTables();
	
	// �������� ������� ������
	// ��� �������, ����� ���������� �������� - ��� �� ������
	bool CheckCurves();

	// �������� ������
	bool Load();

	// ������� ��������� �����
	bool ExportPlanElem();

	// ������� ������
	bool Export(const VCurve & crv, size_t crvNdx);

	const std::wstring & GetMessage() const;
};

