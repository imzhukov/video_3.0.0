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

	const bool optics; /// оптика
	bool hs, hs_bt; /// высокие скорости

	VDataSetPtr dataSet;
	VIBDatabasePtr dataBase;
	VIBTransaction & transaction;
	VIBDataHandlerParameter handler;
	VSQLGenerator sqlgen;

	std::stringstream sql;
	std::wstring message;

	// справочник по таблицам
	enum TABLE
	{
		TABLE_TRAIL, TABLE_PLAN_ELEM, TABLE_CURVE, TABLE_MARK, TABLE_RADIUS, 
		TABLE_SPDAREA, TABLE_SPDAREA_HS, TABLE_SPDAREA_HS_BT,
		TABLE_RADIUS_SPDAREA, TABLE_RADIUS_SPDAREA_HS, TABLE_RADIUS_SPDAREA_HS_BT,
		TABLE_PICKET_SPDLIM, TABLE_PICKET_WEAR,
		TABLE_LONG_RAILS, TABLE_WEAR_DENSITY
	} tables;
	std::string GetTable(TABLE table) const;

	// выполнить sql-запрос
	bool ExecuteStatement(VSQLGenerator & sqlgen);
	bool ExecuteStatement(const std::string & sql);

	// общий экспорт участков скоростей
	bool ExportSpdAreas(const VCurve & curve, TABLE table, TABLE parent_table, 
		__int64 parent_id, size_t radiusNdx);

	// экспорт участков высоких скоростей [с наклоном кузова]
	bool ExportHiSpdAreas(const VCurve & curve, TABLE table, TABLE parent_table, bool bt,
		__int64 parent_id, size_t radiusNdx);

	// Запрос типа элемента плана
	std::string GetPlanElemTypeSQL(PLAN_ELEM_TYPE planElemType) const;

public:

	VCurveExport(CURVE_TYPE ct, VDataSetPtr ds, VIBDatabasePtr db, VIBTransaction & trans, 
		__int64 trail, __int64 repair, bool optics);

	// очистка таблиц
	bool ClearTables();
	
	// проверка наличия таблиц
	// для случаев, когда отсутствие разметки - это не ошибка
	bool CheckCurves();

	// загрузка данных
	bool Load();

	// экспорт элементов плана
	bool ExportPlanElem();

	// экспорт кривой
	bool Export(const VCurve & crv, size_t crvNdx);

	const std::wstring & GetMessage() const;
};

