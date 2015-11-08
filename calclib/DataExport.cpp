#include "DataExport.h"
#include "strlib.h"
#include "dblib.h"
#include "dbfunc.h"
#include "revision.h"
#include "VCurveCard.h"
#include "DataExportFunctions.h"
#include "VProgressInterface.h"
#include "sqlgen.h"

#include <fstream>
#include <sstream>
#include <iomanip>

#include "Shiny.h"

const std::wstring OPERATION_CANCELED = L"__OPERATION_CANCELED__";

const std::wstring DE_OVERSTEPS_NAME = L"Отступления";
const std::wstring DE_CURVES_NAME = L"Кривые";
const std::wstring DE_MODEL_CURVES_NAME = L"Модели кривых";
const std::wstring DE_STATISTICS_NAME = L"Статистика";
const std::wstring DE_MARKS_NAME = L"Метки оператора и шпал";
const std::wstring DE_ZAZOR_NAME = L"Зазоры";
const std::wstring DE_UKLON_NAME = L"Уклоны";
const std::wstring DE_PEREKOS_NAME = L"Перекосы на базе вагона и тележки";
const std::wstring DE_PROFILE_NAME = L"Элементы профиля";
const std::wstring DE_JAGGY_NAME = L"Неровности на профиле и в плане";
const std::wstring DE_DIFF_WIDTH_NAME = L"Отжатия";
const std::wstring DE_PLAN_SPD_NAME = L"Оценка по план.скоростям";
const std::wstring DE_PPR_NAME = L"Полный профиль рельса";
const std::wstring DE_MARKERS_NAME = L"Маркеры";



static bool DataIsOutdated(const VTime & date_in_file, const VTime & date_in_base)
{
	// измерения отсутствуют в файле
	if (date_in_file.IsZero()) return false;
	// данные не занесены
	if (date_in_base.IsZero()) return true;
	// данные в базе устарели
	if (date_in_base < date_in_file) return true;
	// данные в базе актуальны
	return false;
}

static std::set<DATA_EXPORT> GetAll() 
{
	std::set<DATA_EXPORT> exports;
	exports.insert(DE_OVERSTEPS);
	exports.insert(DE_CURVES);
	exports.insert(DE_MODEL_CURVES);
	exports.insert(DE_STATISTICS);
	exports.insert(DE_MARKS);
	exports.insert(DE_ZAZOR);
	exports.insert(DE_UKLON);
	exports.insert(DE_PEREKOS);
	exports.insert(DE_PROFILE);
	exports.insert(DE_JAGGY);
	exports.insert(DE_DIFF_WIDTH);
	exports.insert(DE_PPR);
	exports.insert(DE_MARKERS);
	return exports;
}

// Проверка необходимости обновления данных в базе
std::set<DATA_EXPORT> CheckDataUpdates(VIBDatabasePtr db, const VFileName & file)
{
	std::set<DATA_EXPORT> exports;

	// файл на занесен в базу
	if (!CheckTrailInBase(db, file))
	{
		return GetAll();
	}

	VDataSet ds;
	if (!ds.OpenFile(file)) return exports;

	VIBTransaction trans = db->StartTransaction();
	VIBDataHandlerParameter handler;
	std::stringstream sql;

	// таймштампы данных в файле

	// создание файла
	VTime file_creation = VTime(ds.GetCreateTime().ToTimeT() - 1);

	// абс.к-та
	VTime file_abs_coord = ds.GetDataBlockTime(PRM_ABS_COORD);

	// сбои
	VTime file_corrupt = ds.GetDataBlockTime(PRM_CORRUPT);
	VTime file_hand_corrupt = ds.GetDataBlockTime(PRM_HAND_CORRUPT);

	// отступления
	VTime file_ovrs = ds.GetDataBlockTime(PRM_OVRS2);
	if (file_ovrs.IsZero()) file_ovrs = ds.GetDataBlockTime(PRM_OVRS3);

	// кривые и всё что от них зависит
	VTime file_curve_marks = ds.GetDataBlockTime(PRM_CURVE_MARKS);
	VTime file_curv = ds.GetDataBlockTime(PRM_CURV);
	VTime file_level = ds.GetDataBlockTime(PRM_LEVEL);
	VTime file_kurs = ds.GetDataBlockTime(PRM_KURS);
	VTime file_width = ds.GetDataBlockTime(PRM_WIDTH);
	VTime file_width_front = ds.GetDataBlockTime(PRM_WIDTH_FRONT);
	VTime file_width_back = ds.GetDataBlockTime(PRM_WIDTH_BACK);
	VTime file_wear_left = ds.GetDataBlockTime(PRM_WEAR_LEFT);
	VTime file_wear_right = ds.GetDataBlockTime(PRM_WEAR_RIGHT);

	// статистика
	VTime file_pit_left = ds.GetDataBlockTime(PRM_PIT_LEFT);
	VTime file_pit_right = ds.GetDataBlockTime(PRM_PIT_RIGHT);
	VTime file_rich_left = ds.GetDataBlockTime(PRM_RICH_LEFT);
	VTime file_rich_right = ds.GetDataBlockTime(PRM_RICH_RIGHT);

	// модели кривых
	VTime file_model_curves = ds.GetDataBlockTime(PRM_MODEL_CURVES);
	
	// метки
	VTime file_op_marks = ds.GetDataBlockTime(PRM_OP_MARKS);
	VTime file_spala_marker = ds.GetDataBlockTime(PRM_SPALA_MARKER);

	// зазоры
	VTime file_zazor_left = ds.GetDataBlockTime(PRM_ZAZOR_LEFT);
	VTime file_zazor_right = ds.GetDataBlockTime(PRM_ZAZOR_RIGHT);
	VTime file_mag_zazor = ds.GetDataBlockTime(PRM_MAG_ZAZOR);
	VTime file_temp_left = ds.GetDataBlockTime(PRM_TEMP_LEFT);
	VTime file_temp_right = ds.GetDataBlockTime(PRM_TEMP_RIGHT);

	// профиль
	VTime file_prof_marks = ds.GetDataBlockTime(PRM_PROF_MARKS);
	VTime file_gradient = ds.GetDataBlockTime(PRM_GRADIENT);

	// перекосы
	VTime file_per_tel = ds.GetDataBlockTime(PRM_DIST_SHORT);
	VTime file_per_kuz = ds.GetDataBlockTime(PRM_DIST_LONG);

	// ппр
	VTime file_pod_left = ds.GetDataBlockTime(PRM_PPR_POD_LEFT);
	VTime file_pod_right = ds.GetDataBlockTime(PRM_PPR_POD_RIGHT);
	VTime file_nak_left = ds.GetDataBlockTime(PRM_PPR_NAK_LEFT);
	VTime file_nak_right = ds.GetDataBlockTime(PRM_PPR_NAK_RIGHT);

	// id поездки и таймштампы данных в базе
	sql << "select id, date_comp, otst_date, crv_date, pasp_date, stat_date, zaz_date, ";
	sql << "prof_date, planspd_date, jag_date, opmark_date, spala_marker_date, ";
	sql << "per_kuz_date, per_tel_date, uklon_date, pulled_date, pr_ngr_date, marker_date, ";
	sql << "stat_count from msr_trail where filename = '" << file.GetShortName() << "';";
	db->ExecuteStatement(trans, sql.str().c_str(), handler);
	sql.str("");
	if (handler.GetRowCount() == 0) return exports;

	__int64 trail_id = handler.GetParameter(0, 0).GetInteger(); 
	VTime db_trail = VTime(handler.GetParameter(0, 1).GetTimestamp());
	VTime db_ovrs = VTime(handler.GetParameter(0, 2).GetTimestamp());
	VTime db_curves = VTime(handler.GetParameter(0, 3).GetTimestamp());
	VTime db_model_curves = VTime(handler.GetParameter(0, 4).GetTimestamp());
	VTime db_stat = VTime(handler.GetParameter(0, 5).GetTimestamp());
	VTime db_zazor = VTime(handler.GetParameter(0, 6).GetTimestamp());
	VTime db_profile = VTime(handler.GetParameter(0, 7).GetTimestamp());
	VTime db_plan_spd = VTime(handler.GetParameter(0, 8).GetTimestamp());
	VTime db_jaggy = VTime(handler.GetParameter(0, 9).GetTimestamp());
	VTime db_op_marks = VTime(handler.GetParameter(0, 10).GetTimestamp());
	VTime db_spala_marker = VTime(handler.GetParameter(0, 11).GetTimestamp());
	VTime db_per_kuz = VTime(handler.GetParameter(0, 12).GetTimestamp());
	VTime db_per_tel = VTime(handler.GetParameter(0, 13).GetTimestamp());
	VTime db_uklon = VTime(handler.GetParameter(0, 14).GetTimestamp());
	VTime db_pulled = VTime(handler.GetParameter(0, 15).GetTimestamp());
	VTime db_pr_ngr = VTime(handler.GetParameter(0, 16).GetTimestamp());
	VTime db_marker = VTime(handler.GetParameter(0, 17).GetTimestamp());
	__int64 stat_count = handler.GetParameter(0, 18).GetInteger();

	// обновление счетчиков msr_trail
	VSQLGenerator sqlgen;
	sqlgen.Init("msr_trail");
	bool counters_need_upd = false;

	// отступления
	if (db_ovrs.IsZero())
	{
		sql << "select max(date_comp) from msr_oversteps where msr_trail_id=" << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_ovrs = VTime(handler.GetParameter(0, 0).GetTimestamp());
		
		counters_need_upd = true;

		if (db_ovrs.IsZero())
		{
			sqlgen.AddString("otst_date", file_creation.ToStringISO());
			sqlgen.AddInteger("otst_count", 0);
		}
		else
		{
			sqlgen.AddString("otst_date", db_ovrs.ToStringISO());
			sql << "select count(id) from msr_oversteps where msr_trail_id=" << trail_id;
			sqlgen.AddSQL("otst_count", sql.str());
			sql.str("");
		}
	}

	// кривые
	if (db_curves.IsZero())
	{
		sql << "select max(date_comp) from msr_plan_elem where msr_trail_id = " << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_curves = VTime(handler.GetParameter(0, 0).GetTimestamp());

		counters_need_upd = true;

		if (db_curves.IsZero())
		{
			sqlgen.AddString("crv_date", file_creation.ToStringISO());
			sqlgen.AddInteger("crv_count", 0);
		}
		else
		{
			sqlgen.AddString("crv_date", db_curves.ToStringISO());
			sql << "select count(msr_curve.id) from msr_curve inner join msr_plan_elem ";
			sql << "on msr_curve.msr_plan_elem_id=msr_plan_elem.id ";
			sql << "where msr_plan_elem.msr_trail_id=" << trail_id;
			sqlgen.AddSQL("crv_count", sql.str());
			sql.str("");
		}
	}

	// модельные кривые
	if (db_model_curves.IsZero())
	{
		sql << "select max(date_comp) from msr_plan_elem_model where msr_trail_id = " << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_model_curves = VTime(handler.GetParameter(0, 0).GetTimestamp());

		counters_need_upd = true;

		if (db_model_curves.IsZero())
		{
			sqlgen.AddString("pasp_date", file_creation.ToStringISO());
			sqlgen.AddInteger("pasp_count", 0);
		}
		else
		{
			sqlgen.AddString("pasp_date", db_model_curves.ToStringISO());
			sql << "select count(msr_curve_model.id) from msr_curve_model inner join msr_plan_elem_model ";
			sql << "on msr_curve_model.msr_plan_elem_model_id=msr_plan_elem_model.id ";
			sql << "where msr_plan_elem_model.msr_trail_id=" << trail_id;
			sqlgen.AddSQL("pasp_count", sql.str());
			sql.str("");
		}
	}

	// статистика
	if (db_stat.IsZero())
	{
		sql << "select max(date_comp) from msr_sssp_km where msr_trail_id = " << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_stat = VTime(handler.GetParameter(0, 0).GetTimestamp());

		counters_need_upd = true;

		if (db_stat.IsZero())
		{
			sqlgen.AddString("stat_date", file_creation.ToStringISO());
			sqlgen.AddInteger("stat_count", 0);
		}
		else
		{
			sqlgen.AddString("stat_date", db_stat.ToStringISO());
			sql << "select count(id) from msr_sssp_km where msr_trail_id=" << trail_id;
			sqlgen.AddSQL("stat_count", sql.str());
			sql.str("");
		}
	}

	// зазоры
	if (db_zazor.IsZero())
	{
		sql << "select max(date_comp) from msr_zaz where msr_trail_id=" << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_zazor = VTime(handler.GetParameter(0, 0).GetTimestamp());

		counters_need_upd = true;

		if (db_zazor.IsZero())
		{
			sqlgen.AddString("zaz_date", file_creation.ToStringISO());
			sqlgen.AddInteger("zaz_count", 0);
		}
		else
		{
			sqlgen.AddString("zaz_date", db_zazor.ToStringISO());
			sql << "select count(id) from msr_zaz where msr_trail_id=" << trail_id;
			sqlgen.AddSQL("zaz_count", sql.str());
			sql.str("");
		}
	}

	// профиль
	if (db_profile.IsZero())
	{
		sql << "select max(date_comp) from msr_profile_elem where msr_trail_id=" << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_profile = VTime(handler.GetParameter(0, 0).GetTimestamp());

		counters_need_upd = true;

		if (db_profile.IsZero())
		{
			sqlgen.AddString("prof_date", file_creation.ToStringISO());
			sqlgen.AddInteger("prof_count", 0);
		}
		else
		{
			counters_need_upd = true;
			sqlgen.AddString("prof_date", db_profile.ToStringISO());
			sql << "select count(id) from msr_profile_elem where msr_trail_id=" << trail_id;
			sqlgen.AddSQL("prof_count", sql.str());
			sql.str("");
		}
	}

	// оценка по план.скоростям
	if (db_plan_spd.IsZero())
	{
		sql << "select max(date_comp) from speed_sections where msr_trail_id=" << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_plan_spd = VTime(handler.GetParameter(0, 0).GetTimestamp());

		counters_need_upd = true;

		if (db_plan_spd.IsZero())
		{
			sqlgen.AddString("planspd_date", file_creation.ToStringISO());
			sqlgen.AddInteger("planspd_count", 0);
		}
		else
		{
			counters_need_upd = true;
			sqlgen.AddString("planspd_date", db_plan_spd.ToStringISO());
			sql << "select count(id) from speed_sections where msr_trail_id=" << trail_id;
			sqlgen.AddSQL("planspd_count", sql.str());
			sql.str("");
		}
	}

	// неровности на плане и профиле
	if (db_jaggy.IsZero())
	{
		sql << "select max(date_comp) from msr_jag_plan_profile where msr_trail_id = " << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_jaggy = VTime(handler.GetParameter(0, 0).GetTimestamp());

		counters_need_upd = true;

		if (db_jaggy.IsZero())
		{
			sqlgen.AddString("jag_date", file_creation.ToStringISO());
			sqlgen.AddInteger("jag_count", 0);
		}
		else
		{
			counters_need_upd = true;
			sqlgen.AddString("jag_date", db_jaggy.ToStringISO());
			sql << "select count(id) from msr_jag_plan_profile where msr_trail_id=" << trail_id;
			sqlgen.AddSQL("jag_count", sql.str());
			sql.str("");
		}
	}

	// метки оператора
	if (db_op_marks.IsZero())
	{
		sql << "select max(date_comp) from msr_opmark where msr_trail_id = " << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_op_marks = VTime(handler.GetParameter(0, 0).GetTimestamp());

		counters_need_upd = true;

		if (db_op_marks.IsZero())
		{
			sqlgen.AddString("opmark_date", file_creation.ToStringISO());
			sqlgen.AddInteger("opmark_count", 0);
		}
		else
		{
			counters_need_upd = true;
			sqlgen.AddString("opmark_date", db_op_marks.ToStringISO());
			sql << "select count(id) from msr_opmark where msr_trail_id=" << trail_id;
			sqlgen.AddSQL("opmark_count", sql.str());
			sql.str("");
		}
	}

	// метки шпал
	if (db_spala_marker.IsZero())
	{
		sql << "select max(date_comp) from msr_spala_marker where msr_trail_id = " << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_spala_marker = VTime(handler.GetParameter(0, 0).GetTimestamp());

		counters_need_upd = true;

		if (db_spala_marker.IsZero())
		{
			sqlgen.AddString("spala_marker_date", file_creation.ToStringISO());
			sqlgen.AddInteger("spala_marker_count", 0);
		}
		else
		{
			counters_need_upd = true;
			sqlgen.AddString("spala_marker_date", db_op_marks.ToStringISO());
			sql << "select count(id) from msr_spala_marker where msr_trail_id=" << trail_id;
			sqlgen.AddSQL("spala_marker_count", sql.str());
			sql.str("");
		}
	}

	// перекосы на базе кузова
	if (db_per_kuz.IsZero())
	{
		sql << "select max(date_comp) from msr_per_kuz where msr_trail_id = " << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_per_kuz = VTime(handler.GetParameter(0, 0).GetTimestamp());
	
		counters_need_upd = true;

		if (db_per_kuz.IsZero())
		{
			sqlgen.AddString("per_kuz_date", file_creation.ToStringISO());
			sqlgen.AddInteger("per_kuz_count", 0);
		}
		else
		{
			counters_need_upd = true;
			sqlgen.AddString("per_kuz_date", db_per_kuz.ToStringISO());
			sql << "select count(id) from msr_per_kuz where msr_trail_id=" << trail_id;
			sqlgen.AddSQL("per_kuz_count", sql.str());
			sql.str("");
		}
	}

	// перекосы на базе тележки
	if (db_per_tel.IsZero())
	{
		sql << "select max(date_comp) from msr_per_tel where msr_trail_id = " << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_per_tel = VTime(handler.GetParameter(0, 0).GetTimestamp());
	
		counters_need_upd = true;

		if (db_per_tel.IsZero())
		{
			sqlgen.AddString("per_tel_date", file_creation.ToStringISO());
			sqlgen.AddInteger("per_tel_count", 0);
		}
		else
		{
			counters_need_upd = true;
			sqlgen.AddString("per_tel_date", db_per_tel.ToStringISO());
			sql << "select count(id) from msr_per_tel where msr_trail_id=" << trail_id;
			sqlgen.AddSQL("per_tel_count", sql.str());
			sql.str("");
		}
	}

	// уклоны
	if (db_uklon.IsZero())
	{
		sql << "select max(date_comp) from msr_uklon where msr_trail_id = " << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_uklon = VTime(handler.GetParameter(0, 0).GetTimestamp());
	
		counters_need_upd = true;

		if (db_uklon.IsZero())
		{
			sqlgen.AddString("uklon_date", file_creation.ToStringISO());
			sqlgen.AddInteger("uklon_count", 0);
		}
		else
		{
			counters_need_upd = true;
			sqlgen.AddString("uklon_date", db_uklon.ToStringISO());
			sql << "select count(id) from msr_uklon where msr_trail_id=" << trail_id;
			sqlgen.AddSQL("uklon_count", sql.str());
			sql.str("");
		}
	}

	// отжатия
	if (db_pulled.IsZero())
	{
		sql << "select max(date_comp) from msr_pulled_rails where msr_trail_id = " << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		if (handler.GetRowCount() > 0) db_pulled = VTime(handler.GetParameter(0, 0).GetTimestamp());
	
		counters_need_upd = true;

		if (db_pulled.IsZero())
		{
			sqlgen.AddString("pulled_date", file_creation.ToStringISO());
			sqlgen.AddInteger("pulled_count", 0);
		}
		else
		{
			counters_need_upd = true;
			sqlgen.AddString("pulled_date", db_pulled.ToStringISO());
			sql << "select count(id) from msr_pulled_rails where msr_trail_id=" << trail_id;
			sqlgen.AddSQL("pulled_count", sql.str());
			sql.str("");
		}
	}

	// ппр
	if (db_pr_ngr.IsZero())
	{
		sql << "select max(date_comp) from msr_ovr_canting where msr_trail_id = " << trail_id << ";";
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		sql.str("");
		
		if (handler.GetRowCount() > 0) db_pr_ngr = VTime(handler.GetParameter(0, 0).GetTimestamp());

		counters_need_upd = true;

		if (db_pr_ngr.IsZero())
		{
			sqlgen.AddString("pr_ngr_date", file_creation.ToStringISO());
			sqlgen.AddInteger("pr_ngr_count", 0);
		}
		else
		{
			counters_need_upd = true;
			sqlgen.AddString("pr_ngr_date", db_pr_ngr.ToStringISO());
			sql << "select count(id) from msr_ovr_canting where msr_trail_id=" << trail_id;
			sqlgen.AddSQL("pr_ngr_count", sql.str());
			sql.str("");
		}
	}
	
	// маркеры
	if (db_marker.IsZero())
	{
	    sql << "select max(date_comp) from msr_way_marker where msr_trail_id = "<< trail_id;
	    db->ExecuteStatement(trans, sql.str().c_str(), handler);
	    sql.str("");
	    
	    if (handler.GetRowCount() > 0) db_marker = VTime(handler.GetParameter(0, 0).GetTimestamp());
	    
	    counters_need_upd = true;
	    
	    if (db_marker.IsZero())
	    {
	        sqlgen.AddString("marker_date", file_creation.ToStringISO());
	        sqlgen.AddInteger("marker_count", 0);
	    }
	    else
	    {
	        sql << "select count(id) from msr_way_marker where msr_trail_id=" << trail_id;
            sqlgen.AddSQL("marker_count", sql.str());
            sqlgen.AddString("marker_date", db_marker.ToStringISO());
            sql.str("");
        }
	}

	if (counters_need_upd)
	{
		LOG_INFO(file.GetShortName() + L": Обновление счетчиков");

		sql << "id=" << trail_id;
		std::string sqlstr = sqlgen.GetUpdateSQL(sql.str());

		try
		{
			db->ExecuteStatement(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(VException & ex)
		{
			LOG_ERROR(ex.GetDescription() + L": " + ex.GetDetails() + L": " + ex.GetDebug());
		}

		sql.str("");
		db->CommitTransaction(trans);
	}
	else
	{
		db->RollbackTransaction(trans);
	}

	// вся поездка от абс.координаты и сбоев
	if (file_abs_coord.IsZero() || file_corrupt.IsZero())
	{
		return exports;
	}
	
	else if (DataIsOutdated(file_abs_coord, db_trail) || DataIsOutdated(file_hand_corrupt, db_trail) ||
		DataIsOutdated(file_corrupt, db_trail))
	{
		return GetAll();
	}

	// -------------------------------------------------------------------------------
	// отступления
	// -------------------------------------------------------------------------------
	if (DataIsOutdated(file_ovrs, db_ovrs))
	{
		exports.insert(DE_OVERSTEPS);
	}

	// -------------------------------------------------------------------------------
	// кривые
	// -------------------------------------------------------------------------------
	if (file_curve_marks.IsZero() || file_curv.IsZero() || file_level.IsZero() || file_kurs.IsZero())
	{
		; 
	}
	else if (DataIsOutdated(file_curve_marks, db_curves) || 
		DataIsOutdated(file_curv, db_curves) || DataIsOutdated(file_level, db_curves) ||
		DataIsOutdated(file_kurs, db_curves) || DataIsOutdated(file_width, db_curves) || 
		DataIsOutdated(file_width_front, db_curves) || DataIsOutdated(file_width_back, db_curves) || 
		DataIsOutdated(file_wear_left, db_curves) || DataIsOutdated(file_wear_right, db_curves))
	{
		exports.insert(DE_CURVES);
	}

	// -------------------------------------------------------------------------------
	// модели кривых
	// -------------------------------------------------------------------------------
	if (file_model_curves.IsZero() || file_level.IsZero() || file_level.IsZero())
	{
		; 
	}
	else if (DataIsOutdated(file_model_curves, db_model_curves) || 
		DataIsOutdated(file_level, db_model_curves) || 
		DataIsOutdated(file_kurs, db_model_curves))
	{
		exports.insert(DE_MODEL_CURVES);
	}

	// -------------------------------------------------------------------------------
	// статистика от кривых
	// -------------------------------------------------------------------------------
	if (file_pit_left.IsZero() || file_pit_right.IsZero() || file_rich_left.IsZero() || 
		file_rich_right.IsZero() || file_level.IsZero() || file_width.IsZero())
	{
		;
	}
	else if (DataIsOutdated(file_pit_left, db_stat) || DataIsOutdated(file_pit_right, db_stat) ||
		DataIsOutdated(file_rich_left, db_stat) || DataIsOutdated(file_rich_right, db_stat) ||
		DataIsOutdated(file_curve_marks, db_stat) || DataIsOutdated(file_level, db_stat) ||
		DataIsOutdated(file_width, db_stat) || DataIsOutdated(file_curve_marks, db_stat) || stat_count == 0)
	{
		exports.insert(DE_STATISTICS);
	}

	// -------------------------------------------------------------------------------
	// зазоры
	// -------------------------------------------------------------------------------
	if (file_zazor_left.IsZero() && file_zazor_right.IsZero() && file_mag_zazor.IsZero())
	{
		;
	}
	else if (DataIsOutdated(file_zazor_left, db_zazor) || DataIsOutdated(file_zazor_right, db_zazor) ||
		DataIsOutdated(file_mag_zazor, db_zazor) || DataIsOutdated(file_temp_left, db_zazor) ||
		DataIsOutdated(file_temp_right, db_zazor))
	{
		exports.insert(DE_ZAZOR);
	}

	// -------------------------------------------------------------------------------
	// профиль
	// -------------------------------------------------------------------------------
	if (file_prof_marks.IsZero() || file_gradient.IsZero())
	{
		;
	}
	else if (DataIsOutdated(file_prof_marks, db_profile) || DataIsOutdated(file_gradient, db_profile))
	{
		exports.insert(DE_PROFILE);
	}

	// -------------------------------------------------------------------------------
	// нер.план.прф 
	// -------------------------------------------------------------------------------
	if (file_curve_marks.IsZero() || file_prof_marks.IsZero() || file_kurs.IsZero() || 
		file_gradient.IsZero())
	{
		;
	}
	else if (DataIsOutdated(file_curve_marks, db_jaggy) || DataIsOutdated(file_prof_marks, db_jaggy) ||
		DataIsOutdated(file_kurs, db_jaggy) || DataIsOutdated(file_gradient, db_jaggy))
	{
		exports.insert(DE_JAGGY);
	}

	// -------------------------------------------------------------------------------
	// метки оператора и метки шпал
	// -------------------------------------------------------------------------------
	if (DataIsOutdated(file_op_marks, db_op_marks) || DataIsOutdated(file_spala_marker, db_spala_marker))
	{
		exports.insert(DE_MARKS);
	}
	
	// -------------------------------------------------------------------------------
	// маркеры - от разметки кривых, меток оператора (кмс) и меток шпал (бредюк)
	// -------------------------------------------------------------------------------
	if (DataIsOutdated(file_op_marks, db_marker) || DataIsOutdated(file_spala_marker, db_marker) || 
	    DataIsOutdated(file_curve_marks, db_marker))
	{
	    exports.insert(DE_MARKERS);
	}

	// -------------------------------------------------------------------------------
	// перекосы на базе тележки и кузова
	// -------------------------------------------------------------------------------
	if (DataIsOutdated(file_per_tel, db_per_tel) || DataIsOutdated(file_per_kuz, db_per_kuz))
	{
		exports.insert(DE_PEREKOS);
	}

	// -------------------------------------------------------------------------------
	// уклоны
	// -------------------------------------------------------------------------------
	if (DataIsOutdated(file_gradient, db_uklon))
	{
		exports.insert(DE_UKLON);
	}

	// -------------------------------------------------------------------------------
	// отжатия
	// -------------------------------------------------------------------------------
	if (file_width.IsZero() || (file_width_front.IsZero() && file_width_back.IsZero()))
	{
		;
	}
	else if (DataIsOutdated(file_width_front, db_pulled) || 
		DataIsOutdated(file_width_back, db_pulled) || 
		DataIsOutdated(file_width, db_pulled)) 
	{
		exports.insert(DE_DIFF_WIDTH);
	}

	// -------------------------------------------------------------------------------
	// ппр
	// -------------------------------------------------------------------------------
	if (file_pod_left.IsZero() || file_pod_right.IsZero() || 
		file_nak_left.IsZero() || file_nak_right.IsZero())
	{
		;
	}
	else if (DataIsOutdated(file_pod_left, db_pr_ngr) ||
		DataIsOutdated(file_pod_right, db_pr_ngr) ||
		DataIsOutdated(file_nak_left, db_pr_ngr) ||
		DataIsOutdated(file_nak_right, db_pr_ngr))
	{
		exports.insert(DE_PPR);
	}

	return exports;
}

// Проверка наличия поездки в базе
bool CheckTrailInBase(VIBDatabasePtr db, const VFileName & file)
{	
	VIBTransaction trans = db->StartTransaction();
	VIBDataHandlerParameter handler;

	std::stringstream sql;
	sql << "select filename from msr_trail where filename='" << file.GetShortName() << "';";

	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
		db->CommitTransaction(trans);
	}
	catch(...)
	{
		db->RollbackTransaction(trans);
		return false; 
	}

	return handler.GetRowCount() > 0;
}

std::wstring DeleteTrail(VIBDatabasePtr db, const VFileName & file)
{
	VIBTransaction trans_delete;
	std::stringstream sql;
	sql << "delete from msr_trail where filename='" << file.GetShortName() << "';";

	try
	{
		trans_delete = db->StartTransaction();
		db->ExecuteStatementImmediate(trans_delete, sql.str().c_str());
	}
	catch (VException &)
	{
		std::string error = sql.str() + "\n\n" + db->GetErrorDescription();
		db->RollbackTransaction(trans_delete);
		return string_to_wstring(error);
	}

	db->CommitTransaction(trans_delete);
	return L"";	
}

std::wstring DeleteTrails(VIBDatabasePtr db, const std::vector<VFileName> & files)
{
	std::wstring result;
	VProgressBox prog(1, L"Удаление поездок из базы", false);
	prog.SetRange(0, files.size());
	for (size_t i = 0; i < files.size(); ++i)
	{
		prog.SetMessage(0, files[i].GetShortName().c_str());
		std::wstring loc_result = DeleteTrail(db, files[i]);
		if (!loc_result.empty())
		{
			result += files[i].GetShortName();
			result += L"\n";
			result += loc_result;
			result += L"\n\n";
		}
		prog.Step(0);
	}
	return result;
}

// Удаление станционных кривых из базы
std::wstring DeleteStCurves(VIBDatabasePtr db, const VFileName & file)
{
	VIBTransaction trans_delete;
	std::stringstream sql;
	sql << "delete from stw_plan_elem where stw_trail_id =";
	sql << "(select id from stw_trail where filename = '" << file.GetShortName() << "');";
	try
	{
		trans_delete = db->StartTransaction();
		db->ExecuteStatementImmediate(trans_delete, sql.str().c_str());
	}
	catch (VException &)
	{
		std::string error = sql.str() + "\n\n" + db->GetErrorDescription();
		db->RollbackTransaction(trans_delete);
		return string_to_wstring(error);
	}

	db->CommitTransaction(trans_delete);
	return L"";
}

// Занесение в базу одного файла станционных путей
std::wstring ExportSingleStationFileDB(const VFileName & fn,
									   VIBDatabasePtr db, 
									   VProgressBox & prog)
{
	if (prog.QueryCancel()) return OPERATION_CANCELED;

	// поток в памяти для формирования запросов
	std::stringstream sql;

	// главная транзакция
	VIBTransaction trans = db->StartTransaction();

	// обработчик данных из базы
	VIBDataHandlerParameter handler;

	// файл д.б. занесен
	sql << "select filename, id from stw_trail where filename='" << fn.GetShortName() << "';";
	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		std::string db_error = db->GetErrorDescription();
		db->RollbackTransaction(trans);
		return string_to_wstring(sql.str() + "\n\n" + db_error);
	}
	sql.str("");
	if (handler.GetRowCount() == 0)
	{
		db->RollbackTransaction(trans);
		return L"Необходимо занести поездку в базу";
	}

	__int64 trail_id = handler.GetParameter(0, 1).GetInteger();

	// станционные пути - проверка наличия элементов плана
	sql << "select count(id) from stw_plan_elem where stw_trail_id = " << trail_id << ";";
	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		std::string error = sql.str() + "\n\n" + db->GetErrorDescription();
		db->RollbackTransaction(trans);
		return string_to_wstring(error);
	}
	sql.str("");

	if (handler.GetParameter(0, 0).GetInteger() > 0)
	{
		db->RollbackTransaction(trans);
		return L"Кривые уже присутствуют в базе";
	}

	// набор данных
	prog.SetMessage(0, "Подготовка данных");
	VDataSetPtr ds(new VDataSet(fn));

	// апринфо из базы
	prog.SetMessage(0, "Получение априорной информации из базы");
	try
	{
		if (!ds->GetAprinfo().MakeStWayInfo(*db,
			ds->GetStationCode(), ds->GetParkName(), ds->GetWay(), fn.GetShortName()))
		{
			std::wstring error = ds->GetAprinfo().GetErrorMessage();
			db->RollbackTransaction(trans);
			return error;
		}
	}
	catch (std::exception& e)
	{
		db->RollbackTransaction(trans);
		return string_to_wstring(string(e.what()));
	}

	// Кривые
	std::wstring error = AddCurves(db, trans, ds, trail_id, &prog, CURVE_STATION, true);
	if (error == OPERATION_CANCELED)
	{
		prog.SetMessage(0, "Откат измерений");
		db->RollbackTransaction(trans);
		return OPERATION_CANCELED;
	}

	prog.SetMessage(0, "Подтверждение транзакции");
	db->CommitTransaction(trans);

	prog.Step(1); 
	prog.Update();
	return error;
}

// Занесение в базу одного файла
std::wstring ExportSingleMagistralFileDB(const VFileName & fn,
										 VIBDatabasePtr db, 
										 VProgressBox & prog,
										 bool rewrite,
										 const std::set<DATA_EXPORT> & exports,
										 bool & success)
{
	success = false;
	if (prog.QueryCancel()) return OPERATION_CANCELED;

	prog.SetPosition(0, 0);
	prog.SetMessage(0, "Проверка уникальности поездки");

	// поток в памяти для формирования запросов
	std::stringstream sql;

	// главная транзакция
	VIBTransaction trans = db->StartTransaction();

	// обработчик данных из базы
	VIBDataHandlerParameter handler;

	// ошибки при занесении
	std::wstring error, local_error;

	std::wstringstream msg;
	if (rewrite)	msg << L"Обновление информации о поездке " << fn.GetShortName();
	else			msg << L"Занесение информации о поездке " << fn.GetShortName();
	LOG_INFO(msg.str());

	sql << "select filename from msr_trail where filename='" << fn.GetShortName() << "';";
	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		std::string db_error = db->GetErrorDescription();
		db->RollbackTransaction(trans);
		return string_to_wstring(sql.str() + "\n\n" + db_error);
	}
	sql.str("");

	if (rewrite)
	{
		if (handler.GetRowCount() == 0)
		{
			db->RollbackTransaction(trans);
			error = L"Обновление работает только для уже занесенного файла";
			LOG_ERROR(error);
			return error;
		}
	}
	else
	{
		if (handler.GetRowCount() > 0)
		{
			db->RollbackTransaction(trans);
			error = L"Файл уже присутствует в базе - для обновления выберите пункт \"Обновить\"";
			LOG_ERROR(error);
			return error;
		}
	}

	// набор данных
	prog.SetMessage(0, "Подготовка данных");
	VDataSetPtr ds(new VDataSet);
	if (!ds->OpenFile(fn))
	{
		db->RollbackTransaction(trans);
		error = L"Не удалось открыть файл";
		LOG_ERROR(error);
		return error;
	}

	ds->Load(PRM_ABS_COORD);
	ds->Load(PRM_HISTORY);

	// добавить/одновить инфу о поездке
	prog.SetMessage(0, rewrite ? L"Обновление информации о поездке" : L"Занесение информации о поездке");
	error = UpdateTrail(db, trans, ds);
	if (!error.empty())
	{
		db->RollbackTransaction(trans);
		msg.str(L"");
		if (rewrite)	msg << L"Обновление файла " << fn.GetShortName() << L" не удалось";
		else			msg << L"Занесение файла " << fn.GetShortName() << L" не удалось";
		LOG_INFO(msg.str());
		return error;
	}

	__int64 trail_id = GetTrailID(db, trans, ds->GetFileName());

	if (!exports.empty())
	{
		// апринфо из базы
		prog.SetMessage(0, "Подготовка априорной информации");
		try
		{
			if (!ds->Makeinfo(db, false, false))
			{
				error = ds->GetAprinfo().GetErrorMessage();
				db->RollbackTransaction(trans);
				LOG_ERROR(error);
				return error;
			}
		}
		catch (std::exception& e)
		{
			db->RollbackTransaction(trans);
			error = string_to_wstring(string(e.what()));
			LOG_ERROR(error);
			return error;
		}

		// апринфо из базы
		prog.SetMessage(0, "Подготовка данных");
		ds->Load(PRM_CORRUPT);

		// Отступления
		if (exports.find(DE_OVERSTEPS) != exports.end())
		{
			local_error = AddOvrs(db, trans, ds, trail_id, &prog);
			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return OPERATION_CANCELED;
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}

		// Кривые
		if (exports.find(DE_CURVES) != exports.end())
		{
			local_error = AddCurves(db, trans, ds, trail_id, &prog, CURVE_MAGISTRAL, true);

			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return OPERATION_CANCELED;
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}

		// Модельные кривые
		if (exports.find(DE_MODEL_CURVES) != exports.end())
		{
			local_error = AddCurves(db, trans, ds, trail_id, &prog, CURVE_MODEL, false);
			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return OPERATION_CANCELED;
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}

		// Cтатистическая информация
		if (exports.find(DE_STATISTICS) != exports.end())
		{
			local_error = AddStatistics(db, trans, ds, trail_id, &prog);
			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return OPERATION_CANCELED;
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}

		// Метки оператора
		if (exports.find(DE_MARKS) != exports.end())
		{
			local_error = AddMarks(db, trans, ds, trail_id, &prog);
			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return OPERATION_CANCELED;
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}

		// Зазоры
		if (exports.find(DE_ZAZOR) != exports.end())
		{
			local_error = AddZaz(db, trans, ds, trail_id, &prog);
			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return OPERATION_CANCELED;
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}

		// Занесение макс. изменений уклона
		if (exports.find(DE_UKLON) != exports.end())
		{
			local_error = AddUklon(db, trans, ds, trail_id, &prog);
			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return OPERATION_CANCELED;
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}

		// Занесение перекосов на базе кузова и тележки
		if (exports.find(DE_PEREKOS) != exports.end())
		{
			local_error = AddPerekos(db, trans, ds, trail_id, &prog);
			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return OPERATION_CANCELED;
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}

		// Занесение разметки профиля
		if (exports.find(DE_PROFILE) != exports.end())
		{
			local_error = AddProfile(db, trans, ds, trail_id, &prog);
			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return L"Операция прервана";
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}

		// Занесение неровностей в плане и профиле
		if (exports.find(DE_JAGGY) != exports.end())
		{
			local_error = AddJaggy(db, trans, ds, trail_id, &prog);
			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return L"Операция прервана";
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}

		// Занесение отжатий рельсов
		if (exports.find(DE_DIFF_WIDTH) != exports.end())
		{
			local_error = AddDiffWidth(db, trans, ds, trail_id, &prog);
			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return L"Операция прервана";
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}

		if (exports.find(DE_PPR) != exports.end())
		{
			local_error = AddPPR(db, trans, ds, trail_id, &prog);
			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return L"Операция прервана";
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}

		if (exports.find(DE_MARKERS) != exports.end())
		{
			local_error = AddMarkers(db, trans, ds, trail_id, &prog);
			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return L"Операция прервана";
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}

		// Оценка по проектным скоростям
		if (exports.find(DE_PLAN_SPD) != exports.end())
		{
			local_error = AddPlanSpd(db, trans, ds, trail_id, &prog);
			if (local_error == OPERATION_CANCELED)
			{
				prog.SetMessage(0, "Откат измерений");
				db->RollbackTransaction(trans);
				return L"Операция прервана";
			}
			else if (!local_error.empty())
			{
				error += local_error;
				error += L"\n\n";
			}
		}
	}

	// добавить/одновить инфу о поездке
	prog.SetPosition(0, 0);
	prog.SetMessage(0, L"Подтверждение информации");
	try
	{
		db->CommitTransaction(trans);
	}
	catch(...)
	{
		error += string_to_wstring(db->GetErrorDescription());
	}

	msg.str(L"");
	if (error.empty())
	{
		if (rewrite)	msg << L"Обновление файла " << fn.GetShortName() << L" успешно завершено";
		else			msg << L"Занесение файла " << fn.GetShortName() << L" успешно завершено";
		LOG_INFO(msg.str());
	}
	else
	{
		if (rewrite)	msg << L"Обновление файла " << fn.GetShortName() << L" завершено с ошибками";
		else			msg << L"Занесение файла " << fn.GetShortName() << L" завершено с ошибками";
		LOG_WARNING(msg.str());
	}

	success = true;
	return error;
}

// Занесение кривых в базу - много файлов
std::wstring DataMultiExportDB(VIBDatabasePtr db, 
							   const std::vector<VFileName> & files, 
							   bool stationFile, bool rewrite, 
							   const std::set<DATA_EXPORT> & exports,
							   std::vector<VFileName> & exported)
{
	const wchar_t * title = rewrite ? L"Обновление измерительных данных в БД" : L"Занесение измерительных данных в БД";
	VProgressBox prog(2, title, true);

	time_t startProc = time(NULL);
	prog.SetRange(1, files.size());

	if (!db->Connected())
	{
		std::string error = db->GetErrorDescription();
		return string_to_wstring(error);
	}	

	std::wstring result;
	size_t finished = 0;
	size_t skipped = 0;
	size_t not_chg = 0;
	for (size_t i = 0; i < files.size(); ++i)
	{
		std::wstringstream msg;
		msg << L"Файл " << i+1 << L" из " << files.size() << L" :: " << files[i].GetShortName();
		if (finished > 0)
		{
			msg << (rewrite ? L" :: обновлено " : L" :: занесено ") << finished;
		}
		if (skipped > 0)
		{
			msg << L" :: пропущено " << skipped;
		}
		if (not_chg > 0)
		{
			msg << L" :: не изменено " << not_chg;
		}
		prog.SetMessage(1, msg.str().c_str());

		msg.str(L"");
		msg << i+1;

		if (prog.QueryCancel()) break;

		bool success = true;
		try
		{
			std::wstring cur_result;
			if (stationFile)
			{
				cur_result = ExportSingleStationFileDB(files[i], db, prog);
			}
			else 
			{
				std::set<DATA_EXPORT> local_exports;

				if (exports.find(DE_AUTO) != exports.end())
				{
					prog.SetMessage(0, L"Проверка актуальности данных");
					local_exports = CheckDataUpdates(db, files[i]);
					if (local_exports.empty()) 
					{
						++not_chg;
						prog.Step(1);
						continue;
					}
					else
					{
						std::set<DATA_EXPORT>::iterator it;
						std::wstring lst;
						for (it = local_exports.begin(); it != local_exports.end(); it++)
						{
							if (!lst.empty()) lst += L", ";
							switch(*it)
							{
							case DE_OVERSTEPS:      lst += DE_OVERSTEPS_NAME; break;
							case DE_CURVES:         lst += DE_CURVES_NAME; break;
							case DE_MODEL_CURVES:   lst += DE_MODEL_CURVES_NAME; break;
							case DE_STATISTICS:     lst += DE_STATISTICS_NAME; break;
							case DE_MARKS:          lst += DE_MARKS_NAME; break; 
							case DE_ZAZOR:          lst += DE_ZAZOR_NAME; break;
							case DE_UKLON:          lst += DE_UKLON_NAME; break;
							case DE_PEREKOS:        lst += DE_PEREKOS_NAME; break;
							case DE_PROFILE:        lst += DE_PROFILE_NAME; break;
							case DE_JAGGY:          lst += DE_JAGGY_NAME; break;
							case DE_DIFF_WIDTH:     lst += DE_DIFF_WIDTH_NAME; break;
							case DE_PLAN_SPD:       lst += DE_PLAN_SPD_NAME; break;
							case DE_PPR:            lst += DE_PPR_NAME; break;
							case DE_MARKERS:        lst += DE_MARKERS_NAME; break;
							default:                lst += L"???"; 
							}
						}
						lst = files[i].GetShortName() + L": Данные, требующие обновления: " + lst;
						LOG_INFO(lst);
					}
				}
				else
				{
					local_exports = exports;
				}

				cur_result = ExportSingleMagistralFileDB(files[i], db, prog, rewrite,
					local_exports, success);
			}

			if (success)
			{
				++finished;
				exported.push_back(files[i]);
			}
			else
			{
				++skipped;
			}

			if (cur_result == OPERATION_CANCELED)
			{
				result += msg.str() + L": " + files[i].GetShortName() + L"\n";
				result += L"Операция прервана\n";
				break;
			}

			else if (!cur_result.empty())
			{
				result += msg.str() + L": " + files[i].GetShortName() + L"\n";
				result += cur_result;
				result += L"\n\n";
			}
		}

		catch(VException & e)
		{
			result += msg.str() + L": " + files[i].GetShortName() + L"\n";
			result += e.GetDescription() + L"\n\n";
		}

		catch(std::exception & e)
		{
			result = msg.str() + L": " + files[i].GetShortName() + L"\n";
			result += string_to_wstring(e.what()) + L"\n\n";
		}

		catch(...)
		{
			result += msg.str() + L": " + files[i].GetShortName() + L"\n";
			result += L"Неизвестное исключение\n\n";
		}

		prog.Step(1);
	}

	time_t elapsed = time(NULL) - startProc;
	time_t min = elapsed / 60, sec = elapsed % 60;
	std::wstringstream msg; 
	msg << title << L" заняло " << min << L" мин " << sec << L" сек";
	LOG_INFO(msg.str());

	return result;
}

/// Частичное удаление данных из базы
std::wstring PartialDataDelete(VIBDatabasePtr db,
                               const std::vector<VFileName> & files,
                               const std::set<DATA_EXPORT> & deletion)
{
    std::vector<std::string> sql_list;
    std::wstring error;
    std::string sqlstr;
    std::stringstream sql;
	__int64 trail_id;
	VSQLGenerator sqlgen;
    
    VIBTransaction trans = db->StartTransaction();
	sqlgen.Init("msr_trail");
    
	VProgressBox prog(2, L"Частичное удаление измерительных данных", false);
	prog.SetRange(0, files.size());
	prog.SetMessage(0, L"Подготовка данных");
   
    if (deletion.find(DE_OVERSTEPS) != deletion.end())
    {
        sql_list.push_back("delete from msr_oversteps"); 
        sql_list.push_back("delete from msr_count_oversteps");
        sqlgen.AddInteger("otst_count", 0);
        sqlgen.AddSQL("otst_date", "NULL");
    }

    if (deletion.find(DE_CURVES) != deletion.end())
    {
        sql_list.push_back("delete from msr_plan_elem");
        sqlgen.AddInteger("crv_count", 0);
        sqlgen.AddSQL("crv_date", "NULL");
    }
    
    if (deletion.find(DE_MODEL_CURVES) != deletion.end())
    {
        sql_list.push_back("delete from msr_plan_elem_model");
        sqlgen.AddInteger("pasp_count", 0);
        sqlgen.AddSQL("pasp_date", "NULL");
    }

    if (deletion.find(DE_STATISTICS) != deletion.end())
    {
        sql_list.push_back("delete from msr_sssp_km");
        sql_list.push_back("delete from msr_sssp_picket");
        sqlgen.AddInteger("stat_count", 0);
        sqlgen.AddSQL("stat_date", "NULL");
    }
    
    if (deletion.find(DE_MARKS) != deletion.end())
    {
        sql_list.push_back("delete from msr_opmark");
        sql_list.push_back("delete from msr_nst_km");
        sql_list.push_back("delete from msr_spala_marker");
        sqlgen.AddInteger("opmark_count", 0);
        sqlgen.AddSQL("opmark_date", "NULL");
        sqlgen.AddInteger("spala_marker_count", 0);
        sqlgen.AddSQL("spala_marker_date", "NULL");
    }
   
    if (deletion.find(DE_ZAZOR) != deletion.end())
    {
        sql_list.push_back("delete from msr_zaz");    
        sqlgen.AddInteger("zaz_count", 0);
        sqlgen.AddSQL("zaz_date", "NULL");
    }
    
    if (deletion.find(DE_UKLON) != deletion.end())
    {
        sql_list.push_back("delete from msr_uklon"); 
        sqlgen.AddInteger("uklon_count", 0);
        sqlgen.AddSQL("uklon_date", "NULL");
    }
   
    if (deletion.find(DE_PEREKOS) != deletion.end())
    {
        sql_list.push_back("delete from msr_per_kuz");
        sql_list.push_back("delete from msr_per_tel");
        sqlgen.AddInteger("per_kuz_count", 0);
        sqlgen.AddSQL("per_kuz_date", "NULL");
        sqlgen.AddInteger("per_tel_count", 0);
        sqlgen.AddSQL("per_tel_date", "NULL");
    }
    
    if (deletion.find(DE_PROFILE) != deletion.end())
    {
        sql_list.push_back("delete from msr_profile_elem");
        sqlgen.AddInteger("prof_count", 0);
        sqlgen.AddSQL("prof_date", "NULL");
    }
    
    if (deletion.find(DE_JAGGY) != deletion.end())
    {
        sql_list.push_back("delete from msr_jag_plan_profile");
        sqlgen.AddInteger("jag_count", 0);
        sqlgen.AddSQL("jag_date", "NULL");
    }
    
    if (deletion.find(DE_DIFF_WIDTH) != deletion.end())
    {
        sql_list.push_back("delete from msr_pulled_rails");
        sqlgen.AddInteger("pulled_count", 0);
        sqlgen.AddSQL("pulled_date", "NULL");
    }
    
    if (deletion.find(DE_PPR) != deletion.end())
    {
        sql_list.push_back("delete from msr_sections_canting");
        sql_list.push_back("delete from msr_ovr_canting");
    }

    if (deletion.find(DE_MARKERS) != deletion.end())
    {
        sql_list.push_back("delete from msr_way_marker");
        sqlgen.AddInteger("marker_count", 0);
        sqlgen.AddSQL("marker_date", "NULL");
    }
    
    if (deletion.find(DE_PLAN_SPD) != deletion.end())
    {
        sql_list.push_back("delete from speed_sections");
        sqlgen.AddInteger("planspd_count", 0);
        sqlgen.AddSQL("planspd_date", "NULL");
    }

	prog.SetRange(1, sql_list.size());

    for (size_t i = 0; i < files.size(); ++i)
    {
        prog.SetMessage(0, files.at(i).GetShortName().c_str());
        prog.SetMessage(1, L"Удаление данных");
    
        trail_id = GetTrailID(db, trans, files.at(i));
        
        sql.str("");
        sql << " where msr_trail_id = " << trail_id << ";";

        for (size_t req = 0; req < sql_list.size(); ++req)
        {
            sqlstr = sql_list.at(req) + sql.str();
        	try
        	{
		        db->ExecuteStatementImmediate(trans, sqlstr.c_str());
        	}
	        catch(...)
        	{
    	        error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
	    	    LOG_ERROR(error);
		        return error;
        	}
        	prog.Step(1);
        }
        
        prog.SetMessage(1, L"Обновление информации о поездке");

        sql.str("");
        sql << "id = " << trail_id;
        sqlstr = sqlgen.GetUpdateSQL(sql.str());

   		try
    	{
	    	db->ExecuteStatement(trans, sqlstr.c_str(), sqlgen.GetParameters());
   		}
    	catch(...)
	    {
   	        error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
    	    LOG_ERROR(error);
	        return error;
       	}        	

        prog.Step(0);
    }
    
    prog.SetMessage(0, L"Подтверждение транзакции");
    db->CommitTransaction(trans);

    prog.SetMessage(0, L"Завершение");
    return L"";
}
                               
