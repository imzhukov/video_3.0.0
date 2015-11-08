#include "VRepairEstimate.h"

#include "DataExport.h"
#include "DataExportFunctions.h"
#include "strlib.h"
#include "dblib.h"
#include "revision.h"
#include "VCurveCard.h"

#include "VProgressInterface.h"

#include "sqlgen.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <wx/wx.h>

// список файлов, по которым оценен ремонт
std::vector<std::wstring> GetRepairTrails(VIBDatabasePtr db, int repair_id)
{
	VIBDataHandlerParameter handler;
	VIBTransaction trans;
	std::vector<std::wstring> trails;
	std::stringstream sql;

	trans = db->StartTransaction();

	sql << "select t.filename from msr_trail t inner join repair_sections rs ";
	sql << "on t.id = rs.msr_trail_id inner join repair_estim re ";
	sql << "on rs.repair_estim_id = re.id ";
	sql << "where re.repair_project_id = " << repair_id;
	sql << "order by rs.enu_repair_section_type_id;";

	db->ExecuteStatement(trans, sql.str().c_str(), handler);

	if (handler.GetRowCount() == 2)
	{
		trails.push_back(handler.GetParameter(0, 0).GetString()); // before
		trails.push_back(handler.GetParameter(1, 0).GetString()); // after
	}	

	sql.str("");
	sql << "select distinct t.filename from msr_trail t inner join repair_sssp_in_year rsy ";
	sql << "on t.id = rsy.msr_trail_id inner join repair_estim re ";
	sql << "on rsy.repair_estim_id = re.id ";
	sql << "where re.repair_project_id = " << repair_id << ";";

	db->ExecuteStatement(trans, sql.str().c_str(), handler);

	if (handler.GetRowCount() > 0)
	{
		trails.push_back(handler.GetParameter(0, 0).GetString()); // after year
	}

	db->RollbackTransaction(trans);
	return trails;
}

// Ремонт уже оценен
bool RepairAlreadyEstimated(VIBDatabasePtr db, int repair_id)
{
	VIBDataHandlerParameter handler;
	VIBTransaction trans;
	std::stringstream sql;

	trans = db->StartTransaction();
	sql << "select id from repair_estim where repair_project_id = " << repair_id << ";";
	db->ExecuteStatement(trans, sql.str().c_str(), handler);
	bool already = handler.GetRowCount() > 0;
	db->RollbackTransaction(trans);
	return already;
}

// Дата проезда
VTime GetTrailDate(VIBDatabasePtr db, const std::wstring & trail)
{
	VIBDataHandlerParameter handler;
	VIBTransaction trans;
	std::stringstream sql;

	trans = db->StartTransaction();
	sql << "select date_trail from msr_trail where filename = '" << wstring_to_string(trail) << "';";
	db->ExecuteStatement(trans, sql.str().c_str(), handler);

	VTime timeStamp; timeStamp.ZeroTime();
	if (handler.GetRowCount() > 0)
	{
		timeStamp = handler.GetParameter(0, 0).GetTimestamp();
	}
	db->RollbackTransaction(trans);
	return timeStamp;
}

// Оценка ремонтов
std::wstring EstimateRepair(VIBDatabasePtr db, int repair_id,
							const VFileName & before, const VFileName & after, 
							const VFileName & after_year)
{
	if (RepairAlreadyEstimated(db, repair_id))
	{
		return L"Ремонт уже оценен";
	}

	if (before.IsEmpty())
	{
		return L"Необходимо указать один файл до поездки";
	}

	if (after.IsEmpty())
	{
		return L"Необходимо указать один файл после поездки";
	}

	bool do_after_year = true;
	if (after_year.IsEmpty())
	{
		do_after_year = false;
	}

	// при необходимости заносим поездку
	std::vector<VFileName> trails_update, exported;
	if (!CheckTrailInBase(db, before))	trails_update.push_back(before);
	if (!CheckTrailInBase(db, after))	trails_update.push_back(after);

	if (do_after_year && !CheckTrailInBase(db, after_year)) trails_update.push_back(after_year);

	std::wstring result;
	if (!trails_update.empty()) 
	{
		std::set<DATA_EXPORT> exports;
		exports.insert(DE_CURVES);
		exports.insert(DE_STATISTICS);

		result = DataMultiExportDB(db, trails_update, false, false,
			exports, exported);
	}
	result.clear();

	if (!CheckTrailInBase(db, before))
		return L"Файл 'до ремонта' не может быть занесён в базу";

	if (!CheckTrailInBase(db, after))
		return L"Файл 'после ремонта' не может быть занесён в базу";

	if (do_after_year && !CheckTrailInBase(db, after_year))
		return L"Файл 'через год после ремонта' не может быть занесён в базу";

	VRepairEstimate repair(db, repair_id);
	if (!repair.Load())
	{
		return repair.GetMessage();
	}

	if (!repair.Estimate(before, after))
	{
		return repair.GetMessage();
	}

	if (do_after_year && !repair.CalcSSSPAfterYear(after_year))
	{
		return repair.GetMessage();
	}

	repair.Finalize();

	return L"";
}

// СССП через год после ремонта
std::wstring RepairSSSPAfterYear(VIBDatabasePtr db, int repair_id, const VFileName & after)
{
	if (!RepairAlreadyEstimated(db, repair_id))
	{
		return L"Необходимо предварительно оценить ремонт";
	}

	if (after.IsEmpty())
	{
		return L"Необходимо указать один файл после поездки";
	}

	std::vector<std::wstring> trails = GetRepairTrails(db, repair_id);

	if (trails.size() < 2)
	{
		return L"Необходимо предварительно оценить ремонт";
	}

	if (trails.size() == 3)
	{
		return L"СССП через год уже занесено в базу. Для пересчета необходимо переоценить ремонт";
	}

	// проверка даты файла "через год" - должна быть после даты "после ремонта"
	time_t after_repair = GetTrailDate(db, trails[1]).ToTimeT();
	time_t year_after_repair = GetTrailDate(db, after.GetShortName()).ToTimeT();

	if (year_after_repair <= after_repair)
	{
		return L"Выбранный файл должен иметь дату проезда позднее файла 'после ремонта'";
	}

	// менее полугода
	if (year_after_repair - after_repair < 6 * 30 * 24 * 3600)
	{
		LOG_WARNING(L"Файл 'через год после ремонта' имеет дату позже файла 'после ремонта' менее чем на пол-года");
	}

	std::vector<VFileName> trails_update, exported;
	if (!CheckTrailInBase(db, after)) trails_update.push_back(after);
	
	std::wstring result;
	if (!trails_update.empty())
	{
		std::set<DATA_EXPORT> exports;
		exports.insert(DE_CURVES);

		result = DataMultiExportDB(db, trails_update, false, false,
			exports, exported);
	}
	result.clear();

	if (!CheckTrailInBase(db, after))
		return L"Файл 'через год после ремонта' не может быть занесён в базу";

	VRepairEstimate repair(db, repair_id);

	if (!repair.Load())
	{
		return repair.GetMessage();
	}

	if (!repair.CalcSSSPAfterYear(after))
	{
		return repair.GetMessage();
	}

	repair.Finalize();
	return L"";
}

// Удаление оценки ремонта
std::wstring DeleteRepair(VIBDatabasePtr db, int repair_id)
{
	VIBTransaction trans;

	std::stringstream sql;
	sql << "delete from repair_estim where repair_project_id = " << repair_id <<";";

	try
	{
		trans = db->StartTransaction();
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		std::string error = sql.str() + "\n\n" + db->GetErrorDescription();
		db->RollbackTransaction(trans);
		return string_to_wstring(error);
	}

	db->CommitTransaction(trans);
	return L"";
}
