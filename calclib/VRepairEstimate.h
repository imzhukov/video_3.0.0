#pragma once

#include "dblib.h"
#include "sqlgen.h"
#include "VProgressInterface.h"
#include "DataExport.h"
#include "DataExportFunctions.h"
#include "strlib.h"

class VRepairEstimate
{
	// служебное
	std::wstring message;
	boost::shared_ptr<VProgressBox> prog;

	// поездки до и после
	VFileName fn_before, fn_after, fn_after_year;
	VDataSetPtr ds_before, ds_after, ds_after_year;
	__int64 trail_id_before, trail_id_after, trail_id_after_year;

	// база данных
	VIBDatabasePtr database;
	VIBTransaction trans;
	VIBDataHandlerParameter handler;
	__int64 repair_id;

	// описание ремонта из базы
	VWaySection repair_section;
	__int64 waycat_after_repair;
	std::wstring repair_type;
	bool jaggy_plan_on_lines;

	// проектные скорости
	struct PrjSpeedArea
	{
		VWaySection section;
		__int64 spd1, spd2;
	};
	std::vector<PrjSpeedArea> prj_speeds;

	// отступления
	struct OvrCount
	{
		__int64 km;
		__int64 ovr1count;
		__int64 ovr2count;
		__int64 ovr3count;
	};
	std::vector<OvrCount> ovrs;

	// оценка километра
	struct KmEstimate
	{
		int km;
		float maxDiffProfile;
		float maxDiffPlan;
		float maxLevel;
		float maxWidthPlus;
		float maxWidthMinus;
		float sssp;
		speed_t dopSpd;
		int grade;

		KmEstimate() : km(0), maxDiffProfile(0), maxDiffPlan(0), maxLevel(0),
			maxWidthPlus(0), maxWidthMinus(0), sssp(0), dopSpd(NO_SPEED), grade(0)
		{}

		// экспорт данных
		void Export(VSQLGenerator & sqlgen);
	};

	// общая оценка участка ремонта
	struct RepairArea
	{
		double absStart, absEnd;
		double maxGrad, maxAngle, minLen, maxJaggy, sssp;
		std::vector<KmEstimate> kms;

		// конструктор
		RepairArea();
		
		// процент пересечения кривой с участком
		double Crossing(const VCurve & curve, VDataSetPtr ds) const;

		// оценить участок ремонта
		void Estimate(const VWaySection & rs, VDataSetPtr ds, const std::wstring & repair_type,
			const std::vector<OvrCount> & ovrs, bool jaggy_plan_on_lines);

		// экспорт данных
		void Export(VSQLGenerator & sqlgen);

	} repair_area_before, repair_area_after;

	// километры через год после ремонта (для статистики)
	std::vector<KmEstimate> kms_after_year;

	// выполнить запрос
	bool ExecuteStatement(const std::string & sql);

	// выполнить запрос
	bool ExecuteStatement(VSQLGenerator & sqlgen);

	// априорная информация
	bool Makeinfo(VDataSetPtr ds, const wchar_t * msg);

	// Экспорт кривых
	bool ExportCurves(__int64 trail_id, RepairArea & area, VDataSetPtr ds);

public:

	// конструктор
	VRepairEstimate(VIBDatabasePtr db, __int64 rep_id);

	// Данные о ремонте из базы
	bool Load();

	// Оценка ремонта
	bool Estimate(const VFileName & fnb, const VFileName & fna);

	// СССП через год после ремонта
	bool CalcSSSPAfterYear(const VFileName & fnay);

	// сообщение об ошибке
	const std::wstring & GetMessage() const
	{
		return this->message;
	}

	// Выгрузка
	void Finalize();
};

