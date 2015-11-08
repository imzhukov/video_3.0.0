#include "VCurveExport.h"
#include "DataExportFunctions.h"
#include "VKmResult.h"
#include "VCurveCard.h"
#include "sqlgen.h"

#include <iomanip>

// ---------------------------------------------------------------------------
// Обновление информации о поездке
// ---------------------------------------------------------------------------

std::wstring UpdateTrail(VIBDatabasePtr db, 
						 VIBTransaction & trans, 
						 VDataSetPtr ds)
{
	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);

	VSQLGenerator sqlgen;
	std::vector<VIBParameter> sqlprm;
	std::string sqlstr;

	std::wstring error;

	VIBDataHandlerParameter handler;

	sql << "select filename from msr_trail where filename='" << ds->GetFileName().GetShortName() << "';";
	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(VException &)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	if (handler.GetRowCount() == 0)
	{
		VWaySection sect = ds->GetSection();
		VTime creation = VTime(ds->GetCreateTime().ToTimeT() - 1);

		sqlgen.Init("msr_trail");

		sql << "select adm_way.id from adm_way inner join ";
		sql << "adm_direction on adm_way.adm_direction_id = adm_direction.id where ";
		sql << "adm_direction.code = '" << ds->GetDir() << "' ";
		sql << "and adm_way.code = '" << ds->GetWay() << "'";
		sqlgen.AddSQL("adm_way_id", sql.str());

		sqlgen.AddString("nvps", ds->GetDivision());
		sqlgen.AddTimestamp("date_trail", ds->GetRailDate());
		sqlgen.AddFloat("start_abs", ds->GetAbsCoord().GetItem(0));
		sqlgen.AddFloat("end_abs", ds->GetAbsCoord().GetLastItem());
		sqlgen.AddInteger("start_km", sect.start.km);
		sqlgen.AddInteger("start_m", sect.start.m);
		sqlgen.AddInteger("end_km", sect.end.km);
		sqlgen.AddInteger("end_m", sect.end.m);
		sqlgen.AddString("filename", ds->GetFileName().GetShortName());
		sqlgen.AddString("prog_version", revision);

		std::string version_rt = ds->GetHistory().ExtractCreatorVersion();
		if (!version_rt.empty() && version_rt.length() < 33) 
			sqlgen.AddString("version_rt", version_rt);

		std::string data_source = ds->GetDataSource();
		if (data_source.length() < 33) 
			sqlgen.AddString("source_rt", data_source);

		sqlgen.AddInteger("otst_count", 0);
		sqlgen.AddTimestamp("otst_date", creation);

		sqlgen.AddInteger("crv_count", 0);
		sqlgen.AddInteger("crv_optics", 0);
		sqlgen.AddTimestamp("crv_date", creation);

		sqlgen.AddInteger("pasp_count", 0);
		sqlgen.AddTimestamp("pasp_date", creation);

		sqlgen.AddInteger("stat_count", 0);
		sqlgen.AddTimestamp("stat_date", creation);

		sqlgen.AddInteger("zaz_count", 0);
		sqlgen.AddTimestamp("zaz_date", creation);

		sqlgen.AddInteger("prof_count", 0);
		sqlgen.AddTimestamp("prof_date", creation);

		sqlgen.AddInteger("planspd_count", 0);
		sqlgen.AddTimestamp("planspd_date", creation);

		sqlgen.AddInteger("jag_count", 0);
		sqlgen.AddTimestamp("jag_date", creation);

		sqlgen.AddInteger("spala_marker_count", 0);
		sqlgen.AddTimestamp("spala_marker_date", creation);

		sqlgen.AddInteger("opmark_count", 0);
		sqlgen.AddTimestamp("opmark_date", creation);

		sqlgen.AddInteger("per_tel_count", 0);
		sqlgen.AddTimestamp("per_tel_date", creation);

		sqlgen.AddInteger("per_kuz_count", 0);
		sqlgen.AddTimestamp("per_kuz_date", creation);

		sqlgen.AddInteger("uklon_count", 0);
		sqlgen.AddTimestamp("uklon_date", creation);

		sqlgen.AddInteger("pulled_count", 0);
		sqlgen.AddTimestamp("pulled_date", creation);

		sqlgen.AddInteger("pr_ngr_count", 0);
		sqlgen.AddTimestamp("pr_ngr_date", creation);

		sqlstr = sqlgen.GetInsertSQL();

		try
		{
			db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(VException &)
		{
			error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
			return error;
		}

		return L"";
	}

	else if (handler.GetRowCount() == 1)
	{
		VWaySection sect = ds->GetSection();
		sqlgen.Init("msr_trail");

		std::stringstream subsql;
		subsql << "select adm_way.id from adm_way inner join ";
		subsql << "adm_direction on adm_way.adm_direction_id = adm_direction.id where ";
		subsql << "adm_direction.code = '" << ds->GetDir() << "' ";
		subsql << "and adm_way.code = '" << ds->GetWay() << "'";
		sqlgen.AddSQL("adm_way_id", subsql.str());

		sqlgen.AddString("nvps", ds->GetDivision());
		sqlgen.AddTimestamp("date_trail", ds->GetRailDate());
		sqlgen.AddFloat("start_abs", ds->GetAbsCoord().GetItem(0));
		sqlgen.AddFloat("end_abs", ds->GetAbsCoord().GetLastItem());
		sqlgen.AddInteger("start_km", sect.start.km);
		sqlgen.AddInteger("start_m", sect.start.m);
		sqlgen.AddInteger("end_km", sect.end.km);
		sqlgen.AddInteger("end_m", sect.end.m);
		sqlgen.AddString("prog_version", revision);

		std::string version_rt = ds->GetHistory().ExtractCreatorVersion();
		if (!version_rt.empty() && version_rt.length() < 33) 
			sqlgen.AddString("version_rt", version_rt);

		std::string data_source = ds->GetDataSource();
		if (data_source.length() < 33) 
			sqlgen.AddString("source_rt", data_source);

		subsql.str("");
		subsql << "filename = '" << ds->GetFileName().GetShortName() << "'";

		try
		{
			db->ExecuteStatementImmediate(trans, sqlgen.GetUpdateSQL(subsql.str()).c_str(), sqlgen.GetParameters());
		}
		catch(VException &)
		{
			error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
			return error;
		}
		return L"";
	}

	else
	{
		return L"Неуникальная запись о поездке";
	}
}

// ---------------------------------------------------------------------------
// ID поездки в базе
// ---------------------------------------------------------------------------

__int64 GetTrailID(VIBDatabasePtr db, VIBTransaction & trans, const VFileName & file)
{	
	VIBDataHandlerParameter handler;

	std::stringstream sql;
	sql << "select id from msr_trail where filename='" << file.GetShortName() << "';";
	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		db->RollbackTransaction(trans);
		return -1;
	}

	if (handler.GetRowCount() == 0)		return 0;
	else								return handler.GetParameter(0, 0).GetInteger();
}

// ---------------------------------------------------------------------------
// Кривые и элементы плана
// ---------------------------------------------------------------------------

std::wstring AddCurves(VIBDatabasePtr db, 
					   VIBTransaction & trans,
					   VDataSetPtr ds,
					   __int64 trail_id,
					   VProgressBox * prog,
					   CURVE_TYPE crvType,
					   bool optics)
{
	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);
	std::wstring error;
	std::wstring message;

	VCurveExport crvx(crvType, ds, db, trans, trail_id, 0, optics);

	if (crvType == CURVE_MAGISTRAL)		message = L"Очистка таблицы кривых";
	else if (crvType == CURVE_STATION)	message = L"Очистка таблицы кривых на ст.путях";
	else /* crvType == CURVE_MODEL */	message = L"Очистка таблицы модельных кривых";

	if (prog)
	{
		prog->SetPosition(0, 0);
		prog->SetMessage(0, message.c_str());
		prog->Update();
	}

	if (!crvx.ClearTables())
	{
		LOG_ERROR(crvx.GetMessage());
		return crvx.GetMessage();
	}

	if (crvType == CURVE_MAGISTRAL)		message = L"Загрузка кривых";
	else if (crvType == CURVE_STATION)	message = L"Загрузка кривых на ст.путях";
	else /* crvType == CURVE_MODEL */	message = L"Загрузка модельных кривых";

	if (prog)
	{
		prog->SetMessage(0, message.c_str());
		prog->Update();
	}

    // Проверяем наличие разметки в файле, при её отсутствии уходим без генерации ошибок
    if (!crvx.CheckCurves()) return L"";

	if (!crvx.Load()) 
	{
		LOG_ERROR(crvx.GetMessage());
		return crvx.GetMessage();
	}
	if (!crvx.ExportPlanElem()) 
	{
		LOG_ERROR(crvx.GetMessage());
    	return crvx.GetMessage();
	}

	PARAMETER_TYPE param;
	if (crvType == CURVE_MAGISTRAL || crvType == CURVE_STATION) param = PRM_CURVE_MARKS;
	else /* crvType == CURVE_MODEL */ param = PRM_MODEL_CURVES;

	size_t crvCount = ds->GetCurves(param).GetCount();
	size_t skipped_broken = 0;
	size_t skipped_off_coord = 0;

	if (prog)
	{
		prog->SetRange(0, crvCount);
	}

	// Занесение кривых
	for (size_t crvNdx = 0; crvNdx < crvCount; ++crvNdx)
	{
		if (prog != NULL)
		{
			if (prog->QueryCancel()) 
			{
				return OPERATION_CANCELED;
			}

			std::wstringstream msg;
			msg << message << L" :: запись " << crvNdx+1 << L" из " << crvCount;
			prog->SetMessage(0, msg.str().c_str());
			prog->Step(0);
			prog->Update();
		}

		const VCurve & crv = ds->GetCurves(param).GetItem(crvNdx);

		// кривая вписывается в границы участка, согласно апринфо
		if (!crv.FitsWayRanges(*ds))
		{
			++skipped_off_coord;
			continue;
		}

		// кривая корректно размечена
		if (!crv.CheckMarks())
		{
			++skipped_broken;
			continue;
		}

		// занесение кривой в базу
		if (!crvx.Export(crv, crvNdx))
		{
			LOG_ERROR(crvx.GetMessage());
			return crvx.GetMessage();
		}
	}

	if (prog)
	{
		prog->SetPosition(0, crvCount);
		prog->Update();
	}

	if (crvType == CURVE_MAGISTRAL)
	{
		sql << "update msr_trail set crv_count = (select count(msr_curve.id) from msr_curve inner join ";
		sql << "msr_plan_elem on msr_curve.msr_plan_elem_id=msr_plan_elem.id ";
		sql << "where msr_plan_elem.msr_trail_id=" << trail_id << ", ";
		sql << "crv_optics = " << (optics ? 1 : 0) << ", ";
		sql << "crv_date = '" << VTime().ToStringISO() << "', ";
		sql << "enu_iznos_type_id = " << (OPTIONS.GetOption(L"WEAR_SOURCE").Compare(OPTION_PPR) ? 2 : 1) << " ";
		sql << "where id=" << trail_id << ";";
	}
	else if (crvType == CURVE_MODEL)
	{
		sql << "update msr_trail set pasp_count = (select count(msr_curve_model.id) from msr_curve_model inner join ";
		sql << "msr_plan_elem_model on msr_curve_model.msr_plan_elem_model_id=msr_plan_elem_model.id ";
		sql << "where msr_plan_elem_model.msr_trail_id=" << trail_id << ", ";
		sql << "pasp_date = '" << VTime().ToStringISO() << "' where id=" << trail_id << ";";
	}
	else 
	{
		return error;
	}

	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}

	return error;
}

// ---------------------------------------------------------------------------
// Занесение статистики
// ---------------------------------------------------------------------------

std::wstring AddStatistics(VIBDatabasePtr db, 
						   VIBTransaction & trans,
						   VDataSetPtr ds,
						   __int64 id,
						   VProgressBox * prog)
{
	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);

	VSQLGenerator sqlgen;
	std::vector<VIBParameter> sqlprm;
	std::string sqlstr;

	std::wstring error;

	VIBDataHandlerParameter handler;

	if (prog)
	{
		prog->SetPosition(0, 0);
		prog->SetMessage(0, "Очистка таблиц со статистической информацией");
		prog->Update();
	}

	sql << "delete from msr_sssp_km where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "delete from msr_sssp_picket where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "update msr_trail set stat_count=0, stat_date=NULL where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	if (prog)
	{
		prog->SetMessage(0, "Загрузка статистической информации");
		prog->Update();
	}

	if (!ds->Load(PRM_STATISTICS))
	{
		error = L"Cбой при загрузке статистики: " + ds->GetErrorMessage();
		LOG_ERROR(error);
		return error;
	}

	if (prog)
	{
		prog->SetRange(0, ds->GetStatistics().GetKmList().GetCount() + 
			ds->GetStatistics().GetPkList().GetCount());
	}

	for (size_t i = 0; i < ds->GetStatistics().GetKmList().GetCount(); i++)
	{
		if (prog && prog->QueryCancel())
		{	
			return OPERATION_CANCELED;
		}

		const VStatData & item = ds->GetStatistics().GetKmList().GetItem(i);
		if (item.pntCalc == 0) continue;

		sqlgen.Init("msr_sssp_km");
		sqlgen.AddInteger("msr_trail_id", id);
		sqlgen.AddInteger("km", item.km);
		sqlgen.AddFloat("sko_pit_l", item.sskoPitLeft);
		sqlgen.AddFloat("sko_pit_r", item.sskoPitRight);
		sqlgen.AddFloat("sko_lvl", item.sskoLevel);
		sqlgen.AddFloat("sko_lvl_diff", item.sskoLevelDiff);
		sqlgen.AddFloat("sko_width", item.sskoWidth);
		sqlgen.AddFloat("sko_rich_l", item.sskoRichLeft);
		sqlgen.AddFloat("sko_rich_r", item.sskoRichRight);

		sqlgen.AddFloat("mo_pit_l", item.moPitLeft);
		sqlgen.AddFloat("mo_pit_r", item.moPitRight);
		sqlgen.AddFloat("mo_lvl", item.moLevel);
		sqlgen.AddFloat("mo_lvl_diff", item.moLevelDiff);
		sqlgen.AddFloat("mo_width", item.moWidth);
		sqlgen.AddFloat("mo_rich_l", item.moRichLeft);
		sqlgen.AddFloat("mo_rich_r", item.moRichRight);

		sqlgen.AddFloat("start_abs", item.absBegin);
		sqlgen.AddFloat("end_abs", item.absEnd);
		sqlgen.AddInteger("all_points", item.pntSum);
		sqlgen.AddInteger("calc_points", item.pntCalc);
		sqlgen.AddInteger("exclude_points", item.pntEx);
		sqlgen.AddFloat("sigma", item.sigma);
		sqlgen.AddFloat("sssp", item.sssp);
		sqlstr = sqlgen.GetInsertSQL();

		try
		{
			db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(...)
		{
			error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
			return error;
		}

		if (prog)
		{
			prog->Step(0);
			prog->Update();
		}
	}

	int km = -1;
	for (size_t i = 0; i < ds->GetStatistics().GetPkList().GetCount(); i++)
	{
		const VStatData & item = ds->GetStatistics().GetPkList().GetItem(i);

		if (prog)
		{
			if (item.km != km)
			{
				std::wstringstream msg;
				msg << L"Загрузка статистической информации :: " << item.km << L" км";

				prog->SetMessage(0, msg.str().c_str());
				prog->Update();
				km = item.km;
			}

			if (prog->QueryCancel()) 
			{	
				return OPERATION_CANCELED;
			}

			sqlgen.Init("msr_sssp_picket");
			sqlgen.AddInteger("msr_trail_id", id);
			sqlgen.AddInteger("km", item.km);
			sqlgen.AddInteger("picket", item.pk);
			sqlgen.AddFloat("sko_pit_l", item.sskoPitLeft);
			sqlgen.AddFloat("sko_pit_r", item.sskoPitRight);
			sqlgen.AddFloat("sko_lvl", item.sskoLevel);
			sqlgen.AddFloat("sko_lvl_diff", item.sskoLevelDiff);
			sqlgen.AddFloat("sko_width", item.sskoWidth);
			sqlgen.AddFloat("sko_rich_l", item.sskoRichLeft);
			sqlgen.AddFloat("sko_rich_r", item.sskoRichRight);
			sqlgen.AddFloat("sko_pr_l", item.sskoPrLeft);
			sqlgen.AddFloat("sko_pr_r", item.sskoPrRight);
			sqlgen.AddFloat("sko_ngr_l", item.sskoNgrLeft);
			sqlgen.AddFloat("sko_ngr_r", item.sskoNgrRight);

			sqlgen.AddFloat("mo_pit_l", item.moPitLeft);
			sqlgen.AddFloat("mo_pit_r", item.moPitRight);
			sqlgen.AddFloat("mo_lvl", item.moLevel);
			sqlgen.AddFloat("mo_lvl_diff", item.moLevelDiff);
			sqlgen.AddFloat("mo_width", item.moWidth);
			sqlgen.AddFloat("mo_rich_l", item.moRichLeft);
			sqlgen.AddFloat("mo_rich_r", item.moRichRight);
			sqlgen.AddFloat("mo_pr_l", item.moPrLeft);
			sqlgen.AddFloat("mo_pr_r", item.moPrRight);
			sqlgen.AddFloat("mo_ngr_l", item.moNgrLeft);
			sqlgen.AddFloat("mo_ngr_r", item.moNgrRight);

			sqlgen.AddFloat("start_abs", item.absBegin);
			sqlgen.AddFloat("end_abs", item.absEnd);
			sqlgen.AddInteger("all_points", item.pntSum);
			sqlgen.AddInteger("calc_points", item.pntCalc);
			sqlgen.AddInteger("exclude_points", item.pntEx);
			sqlgen.AddFloat("sigma", item.sigma);
			sqlgen.AddFloat("sssp", item.sssp);
			sqlstr = sqlgen.GetInsertSQL();

			try
			{
				db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
			}
			catch(...)
			{
				error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
				LOG_ERROR(error);
				return error;
			}
		}

		if (prog)
		{
			prog->Step(0);
		}
	}

	if (prog)
	{
		prog->SetPosition(0, ds->GetStatistics().GetKmList().GetCount() + 
			ds->GetStatistics().GetPkList().GetCount());

		prog->Update();
	}

	sql << "update msr_trail set ";
	sql << "stat_count = (select count(id) from msr_sssp_km where msr_trail_id=" << id << ", ";
	sql << "stat_date = '" << VTime().ToStringISO() << "' where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}

	return L"";
}

// ---------------------------------------------------------------------------
// Занесение отступлений 
// ---------------------------------------------------------------------------

bool OverstepConversion(VOverstep3 & ovr, const VOverstep2 & ovr_old, int moveDir)
{
	ZeroMemory(&ovr, sizeof(ovr));

	std::wstring type;
	switch(ovr_old.type)
	{
	case ovrWidthPl:	type = WIDEN; break;
	case ovrWidthMn:	type = NARROW; break;
	case ovrOtvod:		type = OTVOD_WIDTH; break;
	case ovrIznos:		type = WEAR; break;
	case ovrPitL:		type = PIT_LEFT; break;
	case ovrPitR:		type = PIT_RIGHT; break;
	case ovrLevel:		type = LEVEL; break;
	case ovrOtvodH:		type = SHORT_DISTORTION; break;
	case ovrDistort:	type = DISTORTION; break;
	case ovrRichL:		type = RICH_LEFT; break;
	case ovrRichR:		type = RICH_RIGHT; break;
	case ovrRich:		type = RICH; break;
	case ovrRichN:		type = RICH_NR; break;
	case ovrAnp:		type = ANP; break;
	case ovrKsi:		type = PSI; break;
	case ovrZazorL:		type = ZAZOR_LEFT; break;
	case ovrZazorR:		type = ZAZOR_RIGHT; break;
	case ovrHoleSh:		type = WIDTH_CORRUPT; break;
	case ovrHoleLev:	type = LEVEL_CORRUPT; break;
	case ovrHolePrR:	type = PIT_RIGHT_CORRUPT; break;
	case ovrHolePrL:	type = PIT_LEFT_CORRUPT; break;
	case ovrHoleRhR:	type = RICH_RIGHT_CORRUPT; break;
	case ovrHoleRhL:	type = RICH_LEFT_CORRUPT; break;
	default: return false;
	}
	wcsncpy(ovr.type, type.c_str(), 31);

	// координата
	ovr.absCoord = ovr_old.absCoord;
	ovr.km = ovr_old.wayCoord.km;
	ovr.m = ovr_old.wayCoord.m;

	// длина, начало и конец
	ovr.length = ovr_old.len;
	if (ovr.absCoord == 0.0)
	{
		ovr.begin = 0.0;
		ovr.end = 0.0;
	}	
	else
	{
		double real_length = ovr.length;
		if (ovr_old.type == ovrHoleSh || ovr_old.type == ovrHoleLev || 
			ovr_old.type == ovrHolePrR || ovr_old.type == ovrHolePrL || 
			ovr_old.type == ovrHoleRhR || ovr_old.type == ovrHoleRhL)
		{
			ovr.end = ovr.absCoord;
			ovr.begin = ovr.end - moveDir * real_length;
		}
		else
		{
			if (ovr_old.type == ovrRichL || ovr_old.type == ovrRichR || 
				ovr_old.type == ovrRich || ovr_old.type == ovrRichN)
			{
				real_length /= 2.0;
			}

			ovr.begin = ovr.absCoord - moveDir * real_length / 2.0;
			ovr.end = ovr.absCoord + moveDir * real_length / 2.0;
		}
	}

	// величина
	ovr.value = ovr_old.value;

	// степени

	// отступления, у которых __должны быть степени__ 2-3-4
	if (ovr_old.type == ovrWidthPl || ovr_old.type == ovrWidthMn ||
		ovr_old.type == ovrPitL || ovr_old.type == ovrPitR || 
		ovr_old.type == ovrLevel || ovr_old.type == ovrDistort ||
		ovr_old.type == ovrRichL || ovr_old.type == ovrRichR || 
		ovr_old.type == ovrRich || ovr_old.type == ovrRichN)
	{
		if (ovr_old.degree == 2 || ovr_old.degree == 3 || ovr_old.degree == 4)
		{
			ovr.degree = ovr_old.degree;
		}
		else
		{
			return false;
		}
	}
	else // отступления, у которых __не должно быть степени__
	{
		// nb! какой-то мусор
		if (ovr_old.degree != 0)
		{
			int Z = 0;
		}

		ovr.degree = 0;
	}

	// скорости
	ovr.spd1 = ovr_old.setSpd.GetPassSpeed();
	ovr.spd2 = ovr_old.setSpd.GetTruckSpeed();
	ovr.spd3 = ovr_old.setSpd.GetEmptySpeed();
	ovr.rst1 = ovr_old.limSpd.GetPassSpeed();
	ovr.rst2 = ovr_old.limSpd.GetTruckSpeed();
	ovr.rst3 = ovr_old.limSpd.GetEmptySpeed();
	ovr.hasRestriction = ovr_old.IsLimit();

	// к-во эл.отступлений
	ovr.amount = 1;
	if (ovr_old.type == ovrWidthPl || ovr_old.type == ovrWidthMn || ovr_old.type == ovrLevel)
	{
		ovr.amount = ovr_old.numElem;
	}

	// баллы
//	ovr.penalty = ovr_old.GetPenalty();
	ovr.penalty = 0;
	ovr.closeToUpper = 0;

	// примечания
	std::wstring comment;
	if (ovr_old.code.C)			comment += L"[соч]";
	if (ovr_old.code.PP)		comment += L"[3Р]";
	if (ovr_old.code.ThreePr)	comment += L"[3Пр]";
	if (ovr_old.code.Bridge)	comment += L"[мост]";
	if (ovr_old.code.Tonnel)	comment += L"[тоннель]";
	if (ovr_old.code.Switch)	comment += L"[стрелка]";
	wcsncpy(ovr.comment, comment.c_str(), 255);

	return true;
}

std::wstring AddOvrs(VIBDatabasePtr db, 
					  VIBTransaction & trans,
					  VDataSetPtr ds,
					  __int64 id,
					  VProgressBox * prog)
{
	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);

	VSQLGenerator sqlgen;
	std::vector<VIBParameter> sqlprm;
	std::string sqlstr;

	std::wstring error;

	VIBDataHandlerParameter handler;

	if (prog)
	{
		prog->SetPosition(0, 0);
		prog->SetMessage(0, L"Очистка таблицы отступлений");
		prog->Update();
	}

	sql << "delete from msr_oversteps where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "delete from msr_count_oversteps where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "update msr_trail set otst_count=0, otst_date=NULL where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	if (prog)
	{
		prog->SetMessage(0, "Загрузка отступлений");
	}

	size_t ovrCount; 
	bool version3;
	if (ds->DataExists(PRM_OVRS3))
	{
		if (!ds->Load(PRM_OVRS3))	
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}

		ovrCount = ds->GetOvrs3().GetCount();

		if (ovrCount > 0)
		{
			if (ds->GetOvrs3().GetCP515Revision() == 0)
			{
				LOG_INFO(L"В расшифровке версии 3 не указана ревизия инструкции");
			}
		}

		version3 = true;
	}
	else if (ds->DataExists(PRM_OVRS2))
	{
		if (!ds->Load(PRM_OVRS2))	
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}

		ovrCount = ds->GetOvrs2().GetCount();
		version3 = false;
	}
	else return L"";

	if (prog)
	{
		prog->SetRange(0, ovrCount);
	}

	VKmResultList kms;

	if (version3)
	{
		kms.CreateKmList(ds->GetAbsCoord(), ds->GetAprinfo());
	}

	std::set<std::wstring> missing;
	std::map<std::wstring, __int64> ovr_types;

	std::wstring msg_base;
	if (version3) msg_base = L"Загрузка отступлений (v3)";
	else msg_base = L"Загрузка отступлений (v2)";

	VOverstep3 conv_v2;
	for (size_t i = 0; i < ovrCount; i++)
	{
		if (prog && (i % 100) == 0)
		{
			std::wstringstream msg;
			msg << msg_base << L" :: запись " << i << L" из " << ovrCount;

			prog->SetMessage(0, msg.str().c_str());
			prog->SetPosition(0, i);
			prog->Update();
		}

		if (prog && prog->QueryCancel()) 
		{	
			return OPERATION_CANCELED;
		}

		const VOverstep3 * ovr;

		if (version3)
		{
			ovr = &ds->GetOvrs3().GetItem(i);

			kms.ApplyOverstep(ds->GetAprinfo(), *ovr);
		}

		else
		{
			if (!OverstepConversion(conv_v2, ds->GetOvrs2().GetItem(i), ds->GetMoveDirection())) 
				continue;

			ovr = &conv_v2;
		}

		// отступление пропущено - тип отсутствует в справочнике
		if (missing.find(ovr->type) != missing.end())
			continue;

		__int64 ovr_type_id;

		// уже известный тип отступления
		if (ovr_types.find(ovr->type) != ovr_types.end())
		{
			ovr_type_id = ovr_types[ovr->type];
		}

		else
		{
			VIBDataHandlerParameter handler;
			sql.str("");
			sql << "select id from enu_oversteps_type where s_name = '";
			sql << wstring_to_string(ovr->type) << "';";
			try	
			{
				db->ExecuteStatement(trans, sql.str().c_str(), handler);		
			}
			catch(...)
			{
				error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
				LOG_ERROR(error);
				return error;
			}
			if (handler.GetRowCount() == 0)
			{
				std::wstringstream msg;
				msg << L"В справочнике ENU_OVERSTEPS_TYPE отсутствует запись для отступления ";
				msg << ovr->type;
				LOG_ERROR(msg.str());
				missing.insert(ovr->type);
				continue;
			}
			ovr_type_id = handler.GetParameter(0, 0).GetInteger();
			ovr_types[ovr->type] = ovr_type_id;
		}

		sqlgen.Init("msr_oversteps");
		sqlgen.AddInteger("msr_trail_id", id);
		sqlgen.AddInteger("enu_oversteps_type_id", ovr_type_id);
		sqlgen.AddFloat("abscoord", ovr->absCoord);
		sqlgen.AddInteger("km", ovr->km);
		sqlgen.AddFloat("m", ovr->m);
		sqlgen.AddFloat("start_abs", ovr->begin);
		sqlgen.AddFloat("end_abs", ovr->end);
		sqlgen.AddFloat("val", ovr->value);
		sqlgen.AddFloat("len", ovr->length);
		sqlgen.AddInteger("degree", ovr->degree);
		sqlgen.AddInteger("speed1", ovr->spd1);
		sqlgen.AddInteger("speed2", ovr->spd2);
		sqlgen.AddInteger("speed3", ovr->spd3);
		sqlgen.AddInteger("speedlim1", ovr->rst1);
		sqlgen.AddInteger("speedlim2", ovr->rst2);
		sqlgen.AddInteger("speedlim3", ovr->rst3);

		if (ovr->hi_speeds && ovr->spd_hi != NO_SPEED) sqlgen.AddInteger("speed_hs", ovr->spd_hi);
		if (ovr->hi_speeds && ovr->spd_hi_bt != NO_SPEED) sqlgen.AddInteger("speed_hs_bt", ovr->spd_hi_bt);
		if (ovr->hi_speeds && ovr->rst_hi != NO_SPEED) sqlgen.AddInteger("speedlim_hs", ovr->rst_hi);
		if (ovr->hi_speeds && ovr->rst_hi_bt != NO_SPEED) sqlgen.AddInteger("speedlim_hs_bt", ovr->rst_hi_bt);

		sqlgen.AddInteger("amount", ovr->amount);
		sqlgen.AddInteger("penalty", ovr->penalty);
		sqlgen.AddInteger("has_restriction", ovr->hasRestriction);
		sqlgen.AddInteger("close_to", ovr->closeToUpper);
		sqlgen.AddInteger("exclude", ovr->is_excluded);
		sqlgen.AddString("exclude_reason", ovr->excl_reason);
		sqlgen.AddString("comment", ovr->comment);
		sqlstr = sqlgen.GetInsertSQL();

		try	
		{
			db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(VException & ex)
    	{
            std::wstringstream msg;
            msg << L"Отступление " << ovr->km << L" км " << int(ovr->m) << L" м [" << ovr->type << L"]" << std::endl;
            msg << ex.GetDescription() << std::endl << ex.GetDetails();
            LOG_ERROR(msg.str());
            return error;
		}
		catch(...)
		{
            std::wstringstream msg;
            msg << L"Отступление " << ovr->km << L" км " << int(ovr->m) << L" м [" << ovr->type << L"]" << std::endl;
            msg << L"неизвестная ошибка";
            LOG_ERROR(msg.str());
            return error;
		}
	}

	if (version3)
	{
		if (prog)
		{
			prog->SetPosition(0, ovrCount);
			prog->SetMessage(0, "Загрузка покилометровой оценки");
			prog->Update();
		}

		for (size_t i = 0; i < kms.GetCount(); ++i)
		{
			if (kms.GetItem(i).GetCP515Revision() == 1)	
				kms.GetItem(i).ApplyOldStylePenalty();

			sqlgen.Init("msr_count_oversteps");
			sqlgen.AddInteger("msr_trail_id", id);
			sqlgen.AddInteger("km", kms.GetItem(i).GetKm());
			sqlgen.AddFloat("start_abs", kms.GetItem(i).GetKmBegin());
			sqlgen.AddFloat("end_abs", kms.GetItem(i).GetKmEnd());

			sqlgen.AddInteger("suzh2", kms.GetItem(i).GetOvrAmount(NARROW, 2));
			sqlgen.AddInteger("suzh3", kms.GetItem(i).GetOvrAmount(NARROW, 3));
			sqlgen.AddInteger("suzh4", kms.GetItem(i).GetOvrAmount(NARROW, 4));
			sqlgen.AddInteger("ush2", kms.GetItem(i).GetOvrAmount(WIDEN, 2));
			sqlgen.AddInteger("ush3", kms.GetItem(i).GetOvrAmount(WIDEN, 3));
			sqlgen.AddInteger("ush4", kms.GetItem(i).GetOvrAmount(WIDEN, 4));
			sqlgen.AddInteger("u2", kms.GetItem(i).GetOvrAmount(LEVEL, 2));
			sqlgen.AddInteger("u3", kms.GetItem(i).GetOvrAmount(LEVEL, 3));
			sqlgen.AddInteger("u4", kms.GetItem(i).GetOvrAmount(LEVEL, 4));
			sqlgen.AddInteger("p2", kms.GetItem(i).GetOvrAmount(DISTORTION, 2));
			sqlgen.AddInteger("p3", kms.GetItem(i).GetOvrAmount(DISTORTION, 3));
			sqlgen.AddInteger("p4", kms.GetItem(i).GetOvrAmount(DISTORTION, 4));
			sqlgen.AddInteger("pr2_left", kms.GetItem(i).GetOvrAmount(PIT_LEFT, 2));
			sqlgen.AddInteger("pr3_left", kms.GetItem(i).GetOvrAmount(PIT_LEFT, 3));
			sqlgen.AddInteger("pr4_left", kms.GetItem(i).GetOvrAmount(PIT_LEFT, 4));
			sqlgen.AddInteger("pr2_right", kms.GetItem(i).GetOvrAmount(PIT_RIGHT, 2));
			sqlgen.AddInteger("pr3_right", kms.GetItem(i).GetOvrAmount(PIT_RIGHT, 3));
			sqlgen.AddInteger("pr4_right", kms.GetItem(i).GetOvrAmount(PIT_RIGHT, 4));
			sqlgen.AddInteger("r2", kms.GetItem(i).GetOvrAmount(RICH, 2));
			sqlgen.AddInteger("r3", kms.GetItem(i).GetOvrAmount(RICH, 3));
			sqlgen.AddInteger("r4", kms.GetItem(i).GetOvrAmount(RICH, 4));
			sqlgen.AddInteger("rnr2", kms.GetItem(i).GetOvrAmount(RICH_NR, 2));
			sqlgen.AddInteger("rnr3", kms.GetItem(i).GetOvrAmount(RICH_NR, 3));
			sqlgen.AddInteger("rnr4", kms.GetItem(i).GetOvrAmount(RICH_NR, 4));

			sqlgen.AddInteger("ur", kms.GetItem(i).GetOvrAmount(SHORT_DISTORTION));
			sqlgen.AddInteger("usk", kms.GetItem(i).GetOvrAmount(ANP));
			sqlgen.AddInteger("usk_sk", kms.GetItem(i).GetOvrAmount(PSI));
			sqlgen.AddInteger("oshk", kms.GetItem(i).GetOvrAmount(OTVOD_WIDTH));
			sqlgen.AddInteger("three_pr_left", kms.GetItem(i).GetOvrAmount(PIT_LEFT_GROUP));
			sqlgen.AddInteger("three_pr_right", kms.GetItem(i).GetOvrAmount(PIT_RIGHT_GROUP));
			sqlgen.AddInteger("three_p", kms.GetItem(i).GetOvrAmount(DISTORTION_GROUP));
			sqlgen.AddInteger("three_r_left", kms.GetItem(i).GetOvrAmount(RICH_LEFT_GROUP));
			sqlgen.AddInteger("three_r_right", kms.GetItem(i).GetOvrAmount(RICH_RIGHT_GROUP));
			sqlgen.AddInteger("r_plus_pr", kms.GetItem(i).GetOvrAmount(RICH_PIT_COMBINATION));
			sqlgen.AddInteger("r_plus_p", kms.GetItem(i).GetOvrAmount(RICH_DISTORTION_COMBINATION));

			sqlgen.AddInteger("penalty", kms.GetItem(i).GetPenalty());
			sqlgen.AddInteger("crv_penalty", kms.GetItem(i).GetCrvPenalty());
			sqlgen.AddInteger("enu_mark_ovrst_mark_id", kms.GetItem(i).GetMark());
			
    		if (kms.GetItem(i).GetCP515Revision() == 1)
    			sqlgen.AddInteger("has_restriction", kms.GetItem(i).GetCP515Restrictions());
			else
	    		sqlgen.AddInteger("has_restriction", kms.GetItem(i).GetRestrictions());
			
			std::wstring comment = kms.GetItem(i).GetComment();
			if (comment.length() > 255)
				sqlgen.AddString("comment", comment.substr(0, 255));
			else
				sqlgen.AddString("comment", comment);

			sqlstr = sqlgen.GetInsertSQL();

			try	
			{
				db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
			}
			catch(...)
			{
				error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
				LOG_ERROR(error);
				return error;
			}
		}
	}

	if (prog)
	{
		prog->Update();
	}

	sql.str("");
	sql << "update msr_trail set ";
	sql << "otst_count = (select count(id) from msr_oversteps where msr_trail_id=" << id << ", ";
	sql << "enu_tsp_515_id = " << ds->GetOvrs3().GetCP515Revision() << ", ";
	sql << "otst_date = '" << VTime().ToStringISO() << "' where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}

	return L"";
}

// ---------------------------------------------------------------------------
// Занесение меток оператора, меток шпал итп
// ---------------------------------------------------------------------------

std::wstring AddMarks(VIBDatabasePtr db, 
					  VIBTransaction & trans,
					  VDataSetPtr ds,
					  __int64 id,
					  VProgressBox * prog)
{
	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);

	VSQLGenerator sqlgen;
	std::vector<VIBParameter> sqlprm;
	std::string sqlstr;

	std::wstring error;

	VIBDataHandlerParameter handler;

	if (prog)
	{
		prog->SetPosition(0, 0);
		prog->SetMessage(0, L"Очистка таблиц меток");
		prog->Update();
	}

	sql << "delete from msr_opmark where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "delete from msr_nst_km where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "delete from msr_spala_marker where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
		}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "update msr_trail set opmark_count=0, opmark_date=NULL, ";
	sql << "spala_marker_count=0, spala_marker_date=NULL where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	// Метки оператора
	if (ds->DataExists(PRM_OP_MARKS) || ds->DataExists(PRM_SPALA_MARKER))
	{
		ds->Load(PRM_OP_MARKS);
		ds->Load(PRM_SPALA_MARKER);

		if (prog)
		{
			prog->SetMessage(0, L"Загрузка меток оператора");
			prog->Update();
		}

		if (ds->GetOpMarks().GetCount() > 0 || ds->GetSpalaMarker().IsLoaded())
		{
			size_t CountSpalaMark = ds->GetSpalaMarker().GetCount();
			size_t CountOpMark = ds->GetOpMarks().GetCount();

			if (prog)
			{
				prog->SetRange(0, CountOpMark + CountSpalaMark);
			}

			double prev_abscoord = 0;
			int32_t prev_km = 0;
			for (size_t i = 0; i < CountOpMark; ++i)
			{
				if (prog && (i % 100) == 0)
				{
					std::wstringstream msg;
					msg << L"Загрузка меток оператора :: запись " << i << L" из " << CountOpMark;
					prog->SetMessage(0, msg.str().c_str());
					prog->SetPosition(0, i);
					prog->Update();
				}
				if (prog && prog->QueryCancel()) 
				{	
					return OPERATION_CANCELED;
				}

				const VOpMark & mark = ds->GetOpMarks().GetItem(i);
				if (mark.index >= ds->GetAbsCoord().GetCount()) 
				{
					std::wstringstream msg;
					msg << L"Метка " << i << L" пропущена: индекс (";
					msg << mark.index << L" за пределами границ файла";
					LOG_ERROR(msg.str());
					continue;
				}

				sqlgen.Init("msr_opmark");
				sqlgen.AddInteger("msr_trail_id", id);
				sqlgen.AddFloat("abscoord", ds->GetAbsCoord().GetItem(mark.index));
				sqlgen.AddInteger("enu_opmark_type_id", mark.DataBaseIndex());
				sqlstr = sqlgen.GetInsertSQL();

				try	
				{
					db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
				}
				catch(...)
				{
					error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
					LOG_ERROR(error);
					return error;
				}
				
				if(mark.isPole && !mark.isIgnore)
				{					
					VWayCoord way_coord = ds->GetAprinfo().GetRealCoord(ds->GetAbsCoord().GetItem(mark.index));
					if(way_coord.m > 500)
						way_coord.km++;
					if(prev_abscoord)
					{
						if(way_coord.km-prev_km == 1)
						{
							sqlgen.Init("msr_nst_km");
							sqlgen.AddInteger("msr_trail_id", id);
							sqlgen.AddInteger("km", way_coord.km-1);
							sqlgen.AddFloat("len", ds->GetAbsCoord().GetItem(mark.index) - prev_abscoord);

							std::stringstream subsql;
							subsql << "select date_trail from msr_trail where id = " << id;
							sqlgen.AddSQL("data", subsql.str());
							sqlgen.AddString("ignore", "F");
							sqlstr = sqlgen.GetInsertSQL();

							try	
							{
								db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
							}
							catch(...)
							{
								error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
								LOG_ERROR(error);
								return error;
							}
						}
						prev_abscoord = ds->GetAbsCoord().GetItem(mark.index);
						prev_km = way_coord.km;
					}	
					else
					{
						prev_abscoord = ds->GetAbsCoord().GetItem(mark.index);
						prev_km = way_coord.km;
					}
				}
			}

			/*std::string spala_marker_name;*/

			std::set<uint32_t> used_ids;

			for (size_t k = 0; k < CountSpalaMark; ++k)
			{
				if (prog && (k % 100) == 0)
				{
					std::wstringstream msg;
					msg << L"Загрузка меток шпал - запись " << k << L" из " << CountSpalaMark;
					prog->SetMessage(0, msg.str().c_str());
					prog->SetPosition(0, k + CountOpMark);
					prog->Update();
				}
				if (prog && prog->QueryCancel()) 
				{	
					return OPERATION_CANCELED;
				}

				const VSpalaMarker & mark = ds->GetSpalaMarker().GetItem(k);

				if (mark.value == 0)
				{
					LOG_WARNING(L"Пустая метка шпалы");
					continue;
				}
				
				if (used_ids.find(mark.value) != used_ids.end())
				{
					std::wstringstream msg;
					msg << L"Пропущена дублирующаяся метка шпалы ";
					msg << std::hex << mark.value;
					LOG_WARNING(msg.str().c_str());
					continue;
				}
				used_ids.insert(mark.value);

				sqlgen.Init("msr_spala_marker");
				sqlgen.AddInteger("msr_trail_id", id);
				sqlgen.AddFloat("abscoord", ds->GetAbsCoord().GetItem(mark.index));

				char s2[15] = {0};
				sprintf(s2,"%0#10x", mark.value);
				sqlgen.AddString("name", s2);

				sqlstr = sqlgen.GetInsertSQL();

				try	
				{
					db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
				}
				catch(...)
				{
					error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
					LOG_ERROR(error);
					return error;
				}
				sql.str("");
			}

			if (prog)
			{
				prog->SetPosition(0, CountOpMark + CountSpalaMark);
			}
		}
	}

	sql << "update msr_trail set ";
	sql << "opmark_count = (select count(id) from msr_opmark where msr_trail_id=" << id << ", ";
	sql << "opmark_date = '" << VTime().ToStringISO() << "', ";
	sql << "spala_marker_count = (select count(id) from msr_spala_marker where msr_trail_id=" << id << ", ";
	sql << "spala_marker_date = '" << VTime().ToStringISO() << "'";
	sql << " where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}

	return L"";
}

// ---------------------------------------------------------------------------
// Занесение зазоров
// ---------------------------------------------------------------------------

std::wstring AddZaz(VIBDatabasePtr db, 
					VIBTransaction & trans,
					VDataSetPtr ds,
					__int64 id,
					VProgressBox * prog)
{

	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);

	VSQLGenerator sqlgen;
	std::vector<VIBParameter> sqlprm;
	std::string sqlstr;

	std::wstring error;

	VIBDataHandlerParameter handler;

	if (prog)
	{
		prog->SetPosition(0, 0);
		prog->SetMessage(0, L"Очистка таблиц зазоров");
		prog->Update();
	}

	sql << "delete from msr_zaz where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "update msr_trail set zaz_count=0, zaz_date=NULL where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	if (prog)
	{
		prog->SetMessage(0, L"Загрузка зазоров");
		prog->Update();
	}

	if (ds->DataExists(PRM_ZAZOR_LEFT) || ds->DataExists(PRM_ZAZOR_RIGHT) || 
		ds->DataExists(PRM_MAG_ZAZOR_LEFT) || ds->DataExists(PRM_MAG_ZAZOR_RIGHT))
	{
		ds->Load(PRM_ABS_COORD);
		ds->Load(PRM_ZAZOR_LEFT);
		ds->Load(PRM_ZAZOR_RIGHT);
		ds->Load(PRM_MAG_ZAZOR_LEFT);
		ds->Load(PRM_MAG_ZAZOR_RIGHT);
		ds->Load(PRM_TEMP_LEFT);
		ds->Load(PRM_TEMP_RIGHT);

		if (ds->GetZazorLeft().IsLoaded() || ds->GetZazorRight().IsLoaded() || 
			ds->GetMagZazorLeft().IsLoaded() || ds->GetMagZazorRight().IsLoaded())
		{
			size_t CountZazLeft = ds->GetZazorLeft().GetCount();
			size_t CountZazRight = ds->GetZazorRight().GetCount();
			size_t CountMagZazLeft = ds->GetMagZazorLeft().GetCount();
			size_t CountMagZazRight = ds->GetMagZazorRight().GetCount();

			if (prog)
			{
				prog->SetRange(0, CountZazLeft + CountZazRight + CountMagZazLeft + CountMagZazRight);
			}

			// левые зазоры
			for (size_t i = 0; i < CountZazLeft; ++i)
			{
				if (prog && (i % 100) == 0)
				{
					char msg[4096] = {0};
					_snprintf(msg, 4095, "Загрузка оптических зазоров (левая нить) :: запись %u из %u",
						i, CountZazLeft);

					prog->SetMessage(0, msg);
					prog->SetPosition(0, i);
					prog->Update();
				}
				if (prog && prog->QueryCancel()) 
				{	
					return OPERATION_CANCELED;
				}

				const VZazor & zazor = ds->GetZazorLeft().GetItem(i);
				bool corrupt = ds->GetZazorLeft().IsCorrupt(i);

				sqlgen.Init("msr_zaz");
				sqlgen.AddInteger("msr_trail_id", id);
				sqlgen.AddFloat("abscoord", ds->GetAbsCoord().GetItem(zazor.index));
				sqlgen.AddFloat("val", zazor.value);
				sqlgen.AddSQL("enu_side_id", "select id from enu_side where s_name='Л'");
				sqlgen.AddInteger("exclude", __int64(corrupt));

				if (ds->IsLoaded(PRM_TEMP_LEFT))
				{
					sqlgen.AddFloat("temperature", ds->GetParameter(PRM_TEMP_LEFT).GetItem(zazor.index));
				}

				sqlstr = sqlgen.GetInsertSQL();

				try	
				{
					db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
				}
				catch(...)
				{
					error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
					LOG_ERROR(error);
					return error;
				}
			}

			// правые зазоры
			for (size_t i = 0; i < CountZazRight; ++i)
			{
				if (prog && (i % 100) == 0)
				{
					char msg[4096] = {0};
					_snprintf(msg, 4095, "Загрузка оптических зазоров (правая нить) :: запись %u из %u",
						i, CountZazRight);

					prog->SetMessage(0, msg);
					prog->SetPosition(0, CountZazLeft + i);
					prog->Update();
				}
				if (prog && prog->QueryCancel()) 
				{	
					return OPERATION_CANCELED;
				}

				const VZazor & zazor = ds->GetZazorRight().GetItem(i);
				bool corrupt = ds->GetZazorRight().IsCorrupt(i);

				sqlgen.Init("msr_zaz");
				sqlgen.AddInteger("msr_trail_id", id);
				sqlgen.AddFloat("abscoord", ds->GetAbsCoord().GetItem(zazor.index));
				sqlgen.AddFloat("val", zazor.value);
				sqlgen.AddSQL("enu_side_id", "select id from enu_side where s_name='П'");
				sqlgen.AddInteger("exclude", __int64(corrupt));

				if (ds->IsLoaded(PRM_TEMP_RIGHT))
				{
					sqlgen.AddFloat("temperature", ds->GetParameter(PRM_TEMP_RIGHT).GetItem(zazor.index));
				}

				sqlstr = sqlgen.GetInsertSQL();

				try	
				{
					db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
				}
				catch(...)
				{
					error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
					LOG_ERROR(error);
					return error;
				}
			}

			// левый магнитные зазоры
			for (size_t i = 0; i < CountMagZazLeft; ++i)
			{
				if (prog && (i % 100) == 0)
				{
					char msg[4096] = {0};
					_snprintf(msg, 4095, "Загрузка магнитных зазоров (левая нить) :: запись %u из %u",
						i, CountMagZazLeft);

					prog->SetMessage(0, msg);
					prog->SetPosition(0, CountZazLeft + CountZazRight + i);
					prog->Update();
				}
				if (prog && prog->QueryCancel()) 
				{	
					return OPERATION_CANCELED;
				}

				const VZazor & zazor = ds->GetMagZazorLeft().GetItem(i);
				bool corrupt = ds->GetMagZazorLeft().IsCorrupt(i);

				sqlgen.Init("msr_zaz_magn");
				sqlgen.AddInteger("msr_trail_id", id);
				sqlgen.AddFloat("abscoord", ds->GetAbsCoord().GetItem(zazor.index));
				sqlgen.AddFloat("val", zazor.value);
				sqlgen.AddSQL("enu_side_id", "select id from enu_side where s_name='Л'");
				sqlgen.AddInteger("exclude", __int64(corrupt));

				if (ds->IsLoaded(PRM_TEMP_LEFT))
				{
					sqlgen.AddFloat("temperature", ds->GetParameter(PRM_TEMP_LEFT).GetItem(zazor.index));
				}

				sqlstr = sqlgen.GetInsertSQL();

				try	
				{
					db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
				}
				catch(...)
				{
					error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
					LOG_ERROR(error);
					return error;
				}
			}

			// правый магнитный зазор
			for (size_t i = 0; i < CountMagZazRight; ++i)
			{
				if (prog && (i % 100) == 0)
				{
					char msg[4096] = {0};
					_snprintf(msg, 4095, "Загрузка магнитных зазоров (правая нить) :: запись %u из %u",
						i, CountMagZazRight);

					prog->SetMessage(0, msg);
					prog->SetPosition(0, CountZazLeft + CountZazRight + CountMagZazLeft + i);
				}

				if (prog && prog->QueryCancel()) 
				{	
					return OPERATION_CANCELED;
				}

				const VZazor & zazor = ds->GetMagZazorRight().GetItem(i);
				bool corrupt = ds->GetMagZazorRight().IsCorrupt(i);

				sqlgen.Init("msr_zaz_magn");
				sqlgen.AddInteger("msr_trail_id", id);
				sqlgen.AddFloat("abscoord", ds->GetAbsCoord().GetItem(zazor.index));
				sqlgen.AddFloat("val", zazor.value);
				sqlgen.AddSQL("enu_side_id", "select id from enu_side where s_name='П'");
 				sqlgen.AddInteger("exclude", __int64(corrupt));

				if (ds->IsLoaded(PRM_TEMP_RIGHT))
				{
					sqlgen.AddFloat("temperature", ds->GetParameter(PRM_TEMP_RIGHT).GetItem(zazor.index));
				}

				sqlstr = sqlgen.GetInsertSQL();

				try	
				{
					db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
				}
				catch(...)
				{
					error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
					LOG_ERROR(error);
					return error;
				}
			}

			if (prog)
			{
				prog->SetPosition(0, CountZazLeft + CountZazRight + CountMagZazLeft + CountMagZazRight);
			}
		}
	}

	sql << "update msr_trail set ";
	sql << "zaz_count = (select count(id) from msr_zaz where msr_trail_id=" << id << ", ";
	sql << "zaz_date = '" << VTime().ToStringISO() << "' ";
	sql << "where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}

	return L"";
}

// ---------------------------------------------------------------------------
// Занесение макс. изменений уклона
// ---------------------------------------------------------------------------

std::wstring AddUklon(VIBDatabasePtr db, 
					  VIBTransaction & trans,
					  VDataSetPtr ds,
					  __int64 id,
					  VProgressBox * prog)
{
	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);

	VSQLGenerator sqlgen;
	std::vector<VIBParameter> sqlprm;
	std::string sqlstr;

	std::wstring error;

	VIBDataHandlerParameter handler;

	if (prog)
	{
		prog->SetPosition(0, 0);
		prog->SetMessage(0, L"Очистка таблицы макс. изменений уклона");
		prog->Update();
	}

	sql << "delete from msr_uklon where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "update msr_trail set uklon_count=0, uklon_date=NULL where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	if (ds->GetAprinfo().DefaultSpeeds())
	{
		LOG_WARNING(L"Скорость по приказу отсутствует; расчет изменений уклона будет произведен под скорость по умолчанию");
	}

	if (prog)
	{
		prog->SetMessage(0, L"Поиск максимальных изменений уклона");
		prog->SetPosition(0, 0);
		prog->Update();
	}

	if(ds->GetUklon().Load(*ds, NULL))
	{
		size_t CountUklon = ds->GetUklon().GetCount();
		if (prog)
		{
			prog->SetRange(0, CountUklon);
		}

		for (size_t i = 0; i < CountUklon; ++i)
		{
			if (prog && (i % 100) == 0)
			{
				std::wstringstream msg;
				msg << L"Загрузка макимальных изменений уклона :: запись " << i << L" из " << CountUklon;
				prog->SetMessage(0, msg.str().c_str());
				prog->SetPosition(0, i);
				prog->Update();
			}

			if (prog && prog->QueryCancel()) 
			{	
				return OPERATION_CANCELED;
			}

			const VUklon & uklon = ds->GetUklon().GetItem(i);

			sqlgen.Init("msr_uklon");
			sqlgen.AddInteger("msr_trail_id", id);
			sqlgen.AddFloat("abscoord", uklon.absCoord);
			sqlgen.AddInteger("name_len", uklon.name_len);
			sqlgen.AddFloat("len", uklon.length);
			sqlgen.AddFloat("val", uklon.value);
			sqlgen.AddInteger("porog", uklon.porog);
			sqlstr = sqlgen.GetInsertSQL();

			try	
			{
				db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
			}
			catch(...)
			{
				error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
				LOG_ERROR(error);
				return error;
			}
		}

		if (prog)
		{
			prog->SetRange(0, CountUklon);
			prog->Update();
		}
	}

	sql << "update msr_trail set ";
	sql << "uklon_count = (select count(id) from msr_uklon where msr_trail_id=" << id << ", ";
	sql << "uklon_date = '" << VTime().ToStringISO() << "' ";
	sql << "where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}

	return L"";
}

// ---------------------------------------------------------------------------
// Занесение перекосов на базе кузова и тележки
// ---------------------------------------------------------------------------

std::wstring AddPerekos(VIBDatabasePtr db, 
					   VIBTransaction & trans,
					   VDataSetPtr ds,
					   __int64 id,
					   VProgressBox * prog)
{
	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);

	VSQLGenerator sqlgen;
	std::vector<VIBParameter> sqlprm;
	std::string sqlstr;

	std::wstring error;

	VIBDataHandlerParameter handler;

	if (prog)
	{
		prog->SetPosition(0, 0);
		prog->SetMessage(0, L"Очистка таблицы перекосов");
		prog->Update();
	}

	sql << "delete from msr_per_kuz where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "delete from msr_per_tel where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "update msr_trail set per_kuz_count=0, per_kuz_date=NULL, ";
	sql << "per_tel_count=0, per_tel_date=NULL where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	if (prog)
	{
		prog->SetMessage(0, L"Поиск перекосов");
		prog->SetPosition(0, 0);
		prog->Update();
	}

	if (ds->GetAprinfo().DefaultSpeeds())
	{
		LOG_WARNING(L"Скорость по приказу отсутствует; расчет перекосов будет произведен под скорость по умолчанию");
	}

	size_t CountPerKuz = 0;
	size_t CountPerTel = 0;
	std::wstring loc_error = L"";

	if (ds->GetPerTel().Load(*ds, NULL))
	{
		CountPerTel = ds->GetPerTel().GetCount();
	}
	else
	{
		loc_error = ds->GetPerTel().GetErrorMessage();
	}

	if (ds->GetPerKuz().Load(*ds, NULL))
	{
		CountPerKuz = ds->GetPerKuz().GetCount();
	}
	else
	{
		if (!loc_error.empty()) loc_error += L"\n";
		loc_error += ds->GetPerKuz().GetErrorMessage();
	}

	if (prog)
	{
		prog->SetRange(0, CountPerKuz + CountPerTel);
	}

	for (size_t i = 0; i < CountPerKuz; ++i)
	{
		if (prog && (i % 100) == 0)
		{
			std::wstringstream msg;
			msg << L"Загрузка перекосов на базе кузова :: запись " << i << L" из " << CountPerKuz;
			prog->SetMessage(0, msg.str().c_str());
			prog->SetPosition(0, i);
			prog->Update();
		}

		if (prog && prog->QueryCancel()) 
		{	
			return OPERATION_CANCELED;
		}

		const VPer & per = ds->GetPerKuz().GetItem(i);

		sqlgen.Init("msr_per_kuz");
		sqlgen.AddInteger("msr_trail_id", id);
		sqlgen.AddFloat("abscoord", per.absPer);
		sqlgen.AddFloat("val", per.value);
		sqlgen.AddInteger("porog", per.porog);
		sqlgen.AddInteger("speed", per.speed);
		sqlstr = sqlgen.GetInsertSQL();

		try	
		{
			db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(...)
		{
			error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
			return error;
		}
	}

	if (prog)
	{
		prog->SetPosition(0, CountPerKuz);
		prog->Update();
	}

	for (size_t i = 0; i < CountPerTel; ++i)
	{
		if (prog && (i % 100) == 0)
		{
			std::wstringstream msg;
			msg << L"Загрузка перекосов на базе тележки :: запись " << i << L" из " << CountPerTel;
			prog->SetMessage(0, msg.str().c_str());
			prog->SetPosition(0, CountPerKuz + i);
			prog->Update();
		}

		if (prog->QueryCancel()) 
		{	
			return OPERATION_CANCELED;
		}

		const VPer & per = ds->GetPerTel().GetItem(i);

		sqlgen.Init("msr_per_tel");
		sqlgen.AddInteger("msr_trail_id", id);
		sqlgen.AddFloat("abscoord", per.absPer);
		sqlgen.AddFloat("val", per.value);
		sqlgen.AddInteger("porog", per.porog);
		sqlgen.AddInteger("speed", per.speed);
		sqlstr = sqlgen.GetInsertSQL();

		try	
		{
			db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(...)
		{
			error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
			return error;
		}
	}

	if (prog)
	{
		prog->SetPosition(0, CountPerKuz + CountPerTel);
		prog->Update();
	}

	sql << "update msr_trail set ";
	sql << "per_kuz_count = (select count(id) from msr_per_kuz where msr_trail_id=" << id << ", ";
	sql << "per_kuz_date = '" << VTime().ToStringISO() << "', ";
	sql << "per_tel_count = (select count(id) from msr_per_tel where msr_trail_id=" << id << ", ";
	sql << "per_tel_date = '" << VTime().ToStringISO() << "' ";
	sql << "where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}

	return loc_error;
}

// ---------------------------------------------------------------------------
// Занесение разметки профиля
// ---------------------------------------------------------------------------

std::wstring AddProfile(VIBDatabasePtr db, 
						VIBTransaction & trans,
						VDataSetPtr ds,
						__int64 id,
						VProgressBox * prog)
{
	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);

	VSQLGenerator sqlgen;
	std::vector<VIBParameter> sqlprm;
	std::string sqlstr;

	std::wstring error;

	VIBDataHandlerParameter handler;

	if (prog)
	{
		prog->SetMessage(0, L"Очистка таблицы разметки профиля");
		prog->SetPosition(0, 0);
		prog->Update();
	}

	sql << "delete from msr_profile_elem where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "update msr_trail set prof_count=0, prof_date=NULL where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	///Загрузка разметки профиля
	if (ds->DataExists(PRM_PROF_MARKS))
	{
		if (prog)
		{
			prog->SetMessage(0, L"Загрузка разметки профиля");
			prog->SetPosition(0, 0);
			prog->Update();
		}
		if (ds->Load(PRM_PROFILE_SLOPE_ELEMENT))
		{
			if (prog)
			{
				prog->SetRange(0, ds->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).GetCount());
			}
			for (size_t i=0; i<ds->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).GetCount(); ++i)
			{
				if (prog)
				{
					std::wstringstream msg;
					msg <<  L"Загрузка разметки профиля :: запись " << i+1 << L" из ";
					msg << ds->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).GetCount();
					prog->SetMessage(0, msg.str().c_str());
					prog->SetPosition(0, i);
					prog->Update();
					if (prog->QueryCancel()) 
					{	
						return OPERATION_CANCELED;
					}
				}
				
				const VSlope & slope = ds->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).GetItem(i);

				sqlgen.Init("msr_profile_elem");
				sqlgen.AddInteger("msr_trail_id", id);
				sqlgen.AddFloat("start_abs", slope.GetStart().x);
				sqlgen.AddFloat("end_abs", slope.GetEnd().x);
				sqlgen.AddFloat("start_height", slope.GetStart().y);
				sqlgen.AddFloat("end_height", slope.GetEnd().y);
				sqlgen.AddString("type_elem", slope.IsCurve() ? "кривая" : "прямая");
				sqlgen.AddFloat("uklon", slope.Slope());

				if (slope.IsCurve()) 
				{
					sqlgen.AddFloat("radius", slope.R());
				}

				if (i > 0)
				{
					const VSlope & prev = ds->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).GetItem(i-1);
					if (!prev.IsCurve() && !slope.IsCurve())
					{
						sqlgen.AddFloat("razn_ukl", slope.Slope() - prev.Slope());
					}
				}
				sqlgen.AddString("prog_version", revision);
				sqlstr = sqlgen.GetInsertSQL();
				
				try	
				{
					db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
				}
				catch(...)
				{
					error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
					LOG_ERROR(error);
					return error;
				}
			}

			if (prog)
			{
				prog->SetPosition(0, ds->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).GetCount());
				prog->Update();
			}
		}
	}

	sql << "update msr_trail set ";
	sql << "prof_count = (select count(id) from msr_profile_elem where msr_trail_id=" << id << ", ";
	sql << "prof_date = '" << VTime().ToStringISO() << "' ";
	sql << "where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}

	return L"";
}

// ---------------------------------------------------------------------------
// Сверхнормативные неровности
// ---------------------------------------------------------------------------

std::wstring AddJaggy(VIBDatabasePtr db, VIBTransaction & trans,
					  VDataSetPtr ds, __int64 id, VProgressBox * prog)
{
	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);

	VSQLGenerator sqlgen;
	std::vector<VIBParameter> sqlprm;
	std::string sqlstr;

	std::wstring error;

	VIBDataHandlerParameter handler;

	if (prog)
	{
		prog->SetMessage(0, L"Очистка таблицы разметки профиля");
		prog->SetPosition(0, 0);
		prog->Update();
	}

	sql << "delete from msr_jag_plan_profile where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "update msr_trail set jag_count=0, jag_date=NULL where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	if (prog)
	{
		prog->SetMessage(0, L"Очистка таблицы разметки профиля");
		prog->SetPosition(0, 0);
		prog->Update();
	}

	// профиль
	if (ds->DataExists(PRM_PROF_MARKS))
	{
		if (prog)
		{
			prog->SetMessage(0, L"Загрузка неровностей на профиле");
		}

		if (!ds->Load(PRM_JAGGY_PROFILE_OVERSTEP)) 
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}
	}
	size_t ovrCount = ds->GetJaggyProfileOversteps().GetCount();

	// план
	if (ds->DataExists(PRM_CURVE_MARKS))
	{
		if (prog)
		{
			prog->SetMessage(0, L"Загрузка неровностей в плане");
		}

		if (!ds->Load(PRM_JAGGY_PLAN_OVERSTEP)) 
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}
	}
	ovrCount += ds->GetJaggyPlanOversteps().GetCount();

	if (prog)
	{
		prog->SetRange(0, ovrCount);
	}

	if (prog)
	{
		prog->SetMessage(0, L"Импорт неровностей на профиле");
	}

	for (size_t i = 0; i < ds->GetJaggyProfileOversteps().GetCount(); ++i)
	{
		VJaggyProfileOverstepPtr ovr = ds->GetJaggyProfileOversteps().GetItem(i);

		double val =  ovr->GetValue();
		double otvod = ovr->GetOtvod();
		std::wstring descr = ovr->GetDescription();
		double coord = ovr->GetCoord();
		double len = ovr->GetLength();

		sqlgen.Init("msr_jag_plan_profile");
		sqlgen.AddInteger("msr_trail_id", id);
		sqlgen.AddFloat("jag", val);
		sqlgen.AddFloat("otvod", otvod);
		sqlgen.AddString("type_elem", "профиль");
		sqlgen.AddString("description", descr);
		sqlgen.AddString("prog_version", revision);
		sqlgen.AddFloat("abscoord", coord);
		sqlgen.AddFloat("len", len);
		sqlstr = sqlgen.GetInsertSQL();

		try	
		{
			db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(...)
		{
			error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
			return error;
		}

		if (prog)
		{
			prog->Step(0);
			prog->Update();
		}
	}

	if (prog)
	{
		prog->SetMessage(0, L"Импорт неровностей в плане");
	}

	for (size_t i = 0; i < ds->GetJaggyPlanOversteps().GetCount(); ++i)
	{
		VJaggyPlanOverstepPtr ovr = ds->GetJaggyPlanOversteps().GetItem(i);
	
		double val =  ovr->GetValue();
		double otvod = ovr->GetOtvod();
		std::wstring descr = ovr->GetDescription();
		double coord = ovr->GetCoord();
		double len = ovr->GetLength();

		sqlgen.Init("msr_jag_plan_profile");
		sqlgen.AddInteger("msr_trail_id", id);
		sqlgen.AddFloat("jag", val);
		sqlgen.AddFloat("otvod", otvod);
		sqlgen.AddString("type_elem", "план");
		sqlgen.AddString("description", descr);
		sqlgen.AddString("prog_version", revision);
		sqlgen.AddFloat("abscoord", coord);
		sqlgen.AddFloat("len", len);
		sqlstr = sqlgen.GetInsertSQL();

		try	
		{
			db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(...)
		{
			error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
			return error;
		}

		if (prog)
		{
			prog->Step(0);
			prog->Update();
		}
	}

	sql << "update msr_trail set ";
	sql << "jag_count = (select count(id) from msr_jag_plan_profile where msr_trail_id=" << id << ", ";
	sql << "jag_date = '" << VTime().ToStringISO() << "' ";
	sql << "where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	return L"";
}

// ---------------------------------------------------------------------------
// Маркеры
// ---------------------------------------------------------------------------

std::wstring TestCloseMarkers(const VWayMarker & mark, __int64 way_id, 
					  VIBDatabasePtr db, VIBTransaction & trans)
{

	std::wstringstream mark_cmp;
	std::vector<VWayMarker> markers = mark.GetCloseMarkers(db, trans, way_id, 100, 60);

	mark_cmp << string_to_wstring(mark.GetTypeString()) << "; ";
	mark_cmp << L"coord=" << mark.GetCoord() << L"; ";
	mark_cmp << L"id=" << mark.GetID() << L"; ";

	mark_cmp << L"совпадений: " << markers.size() << std::endl;
	for (size_t i = 0; i < markers.size(); ++i)
	{
		mark_cmp << L"coord=" << markers[i].GetCoord() << L" [";
		mark_cmp << mark.GetCoord() - markers[i].GetCoord() <<  L"]";
		if (mark.GetType() != MARKER_KM)
		{
			mark_cmp << L"; id=" << markers[i].GetID() << L" [";
			mark_cmp <<  mark.GetID() - markers[i].GetID() << "]";
		}
		mark_cmp << std::endl;
	}
	LOG_INFO(mark_cmp.str());
	return L"";
}

std::wstring AddMarkers(VIBDatabasePtr db, VIBTransaction & trans,
					   VDataSetPtr ds, __int64 id, VProgressBox * prog)
{
	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);

	VSQLGenerator sqlgen;
	std::vector<VIBParameter> sqlprm;
	std::string sqlstr;

	std::wstring error;

	VIBDataHandlerParameter handler;

	if (prog)
	{
		prog->SetMessage(0, L"Очистка таблицы маркеров");
		prog->SetPosition(0, 0);
		prog->Update();
	}

	sql << "delete from msr_way_marker where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "update msr_trail set marker_count=0, marker_date=NULL where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	if (prog)
	{
		prog->SetMessage(0, L"Загрузка маркеров");
	}

	if (!ds->Load(PRM_WAY_MARKERS))
	{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
	}

	size_t markerCount = ds->GetMarkers().GetCount();

	if (prog)
	{
		prog->SetMessage(0, L"Импорт маркеров");
		prog->SetRange(0, markerCount);
	}

	for (size_t i = 0; i < markerCount; ++i)
	{
		if (prog)
		{
			prog->SetPosition(0, i);
			prog->Update();
		}

		const VWayMarker & mark = ds->GetMarkers().GetItem(i);

		if (!AllowedMarkerType(mark.GetType()))
		{
			continue;
		}

		sqlgen.Init("msr_way_marker");

		std::string id_sql = "select id from enu_marker_type where s_name='" + 
			mark.GetTypeString() + "'";

		sqlgen.AddInteger("msr_trail_id", id);
		sqlgen.AddSQL("enu_marker_type_id", id_sql);
		sqlgen.AddFloat("abscoord", mark.GetCoord());
		sqlgen.AddFloat("latitude", mark.GetLatitude());
		sqlgen.AddFloat("longitude", mark.GetLongitude());
		sqlgen.AddInteger("ident_object", mark.GetID());
		sqlstr = sqlgen.GetInsertSQL();

		try	
		{
			db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(...)
		{
			error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
			return error;
		}
	}

	sql << "update msr_trail set ";
	sql << "marker_count = (select count(id) from msr_way_marker where msr_trail_id=" << id << ", ";
	sql << "marker_date = '" << VTime().ToStringISO() << "' ";
	sql << "where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	return L"";
}

// ---------------------------------------------------------------------------
// Отжатия рельсов
// ---------------------------------------------------------------------------

std::wstring AddDiffWidth(VIBDatabasePtr db, VIBTransaction & trans,
						  VDataSetPtr ds, __int64 id, VProgressBox * prog)
{
	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);

	VSQLGenerator sqlgen;
	std::vector<VIBParameter> sqlprm;
	std::string sqlstr;

	std::wstring error;

	VIBDataHandlerParameter handler;

	if (prog)
	{
		prog->SetMessage(0, L"Очистка таблицы отжатия рельсов");
		prog->SetPosition(0, 0);
		prog->Update();
	}

	sql << "delete from msr_pulled_rails where msr_trail_id = " << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "update msr_trail set pulled_count=0, pulled_date=NULL where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	// отжатия спереди
	if (ds->DataExists(PRM_WIDTH_FRONT))
	{
		if (prog)
		{
			prog->SetMessage(0, L"Загрузка отжатий рельс (ШК - ШК спереди)");
		}

		if (!ds->Load(PRM_DIFF_WIDTH_FRONT_OVERSTEP))
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}
	}
	size_t ovrCount = ds->GetDiffWidthFrontOversteps().GetCount();

	// отжатия сзади
	if (ds->DataExists(PRM_WIDTH_BACK))
	{
		if (prog)
		{
			prog->SetMessage(0, L"Загрузка отжатий рельс (ШК - ШК сзади)");
		}
				
		if (!ds->Load(PRM_DIFF_WIDTH_BACK_OVERSTEP))
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}
	}
	ovrCount += ds->GetDiffWidthBackOversteps().GetCount();

	if (prog)
	{
		prog->SetRange(0, ovrCount);
	}

	if (prog)
	{
		prog->SetMessage(0, L"Импорт отжатий рельс (ШК - ШК спереди)");
	}

	double porog = OPTIONS.GetOption(L"MAX_DIFF_WIDTH").GetValue<double>();
	for (size_t i = 0; i < ds->GetDiffWidthFrontOversteps().GetCount(); ++i)
	{
		VBasicOverstepPtr ovr = ds->GetDiffWidthFrontOversteps().GetItem(i);

		double val = ovr->GetValue();
		double coord = ovr->GetCoord();
		double begin = ovr->GetStart();
		double end = ovr->GetEnd();

		sqlgen.Init("msr_pulled_rails");
		sqlgen.AddInteger("msr_trail_id", id);
		sqlgen.AddInteger("enu_width_type_id", 1);
		sqlgen.AddFloat("start_abs", begin);
		sqlgen.AddFloat("end_abs", end);
		sqlgen.AddFloat("abscoord", coord);
		sqlgen.AddFloat("val", val);
		sqlgen.AddFloat("porog", porog);
		sqlstr = sqlgen.GetInsertSQL();

		try	
		{
			db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(...)
		{
			error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
			return error;
		}

		if (prog)
		{
			prog->Step(0);
			prog->Update();
		}
	}

	if (prog)
	{
		prog->SetMessage(0, L"Импорт отжатий рельс (ШК - ШК сзади)");
	}

	for (size_t i = 0; i < ds->GetDiffWidthBackOversteps().GetCount(); ++i)
	{
		VBasicOverstepPtr ovr = ds->GetDiffWidthBackOversteps().GetItem(i);

		double val = ovr->GetValue();
		double coord = ovr->GetCoord();
		double begin = ovr->GetStart();
		double end = ovr->GetEnd();

		sqlgen.Init("msr_pulled_rails");
		sqlgen.AddInteger("msr_trail_id", id);
		sqlgen.AddInteger("enu_width_type_id", 2);
		sqlgen.AddFloat("start_abs", begin);
		sqlgen.AddFloat("end_abs", end);
		sqlgen.AddFloat("abscoord", coord);
		sqlgen.AddFloat("val", val);
		sqlgen.AddFloat("porog", porog);
		sqlstr = sqlgen.GetInsertSQL();

		try	
		{
			db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(...)
		{
			error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
			return error;
		}

		if (prog)
		{
			prog->Step(0);
			prog->Update();
		}
	}

	sql << "update msr_trail set ";
	sql << "pulled_count = (select count(id) from msr_pulled_rails where msr_trail_id=" << id << ", ";
	sql << "pulled_date = '" << VTime().ToStringISO() << "' ";
	sql << "where id=" << id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	return L"";
}

// ---------------------------------------------------------------------------
// Оценка ППР
// ---------------------------------------------------------------------------

std::wstring AddPPR(VIBDatabasePtr db, VIBTransaction & trans,
					VDataSetPtr ds, __int64 trail_id, VProgressBox * prog)
{
	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);

	VSQLGenerator sqlgen;
	std::vector<VIBParameter> sqlprm;
	std::string sqlstr;

	std::wstring error;

	VIBDataHandlerParameter handler;

	if (prog)
	{
		prog->SetMessage(0, L"Очистка таблиц оценки ППР");
		prog->SetPosition(0, 0);
		prog->Update();
	}

	sql << "delete from msr_sections_canting where msr_trail_id = " << trail_id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "delete from msr_ovr_canting where msr_trail_id = " << trail_id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	// отжатия спереди
	if (ds->DataExists(PRM_PPR_POD_LEFT) && ds->DataExists(PRM_PPR_POD_RIGHT) &&
		ds->DataExists(PRM_PPR_NAK_LEFT) && ds->DataExists(PRM_PPR_NAK_RIGHT))
	{
		if (prog)
		{
			prog->SetMessage(0, L"Загрузка оценки ПР/НГР");
		}

		if (!ds->Load(PRM_PR_NGR_AREAS))
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}

		if (!ds->Load(PRM_CANTING_LEFT_OVERSTEP))
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}

		if (!ds->Load(PRM_CANTING_RIGHT_OVERSTEP))
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}
	}

	if (prog)
	{
		prog->SetMessage(0, L"Импорт отступлений подуклонки");
		prog->SetRange(0, 
			ds->GetCantingOversteps(SD_LEFT).GetCount() + 
			ds->GetCantingOversteps(SD_RIGHT).GetCount() +
			ds->GetPrNgrAreas().GetCount());
	}

	for (size_t i = 0; i < ds->GetCantingOversteps(SD_LEFT).GetCount(); ++i)
	{
		const VBasicOverstepPtr & over = ds->GetCantingOversteps(SD_LEFT).GetItem(i);

		sqlgen.Init("msr_ovr_canting");
		sqlgen.AddInteger("msr_trail_id", trail_id);
		sqlgen.AddSQL("enu_side_id", "select id from enu_side where s_name='Л'");
		sqlgen.AddFloat("start_abs", over->GetStart());
		sqlgen.AddFloat("end_abs", over->GetEnd());
		sqlgen.AddFloat("abscoord", over->GetCoord());
		sqlgen.AddFloat("val_avg", over->GetAverage());
		sqlgen.AddFloat("val_max", over->GetValue());
		sqlstr = sqlgen.GetInsertSQL();

		try	
		{
			db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(...)
		{
			error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
			return error;
		}

		if (prog)
		{
			prog->Step(0);
			prog->Update();
		}
	}

	for (size_t i = 0; i < ds->GetCantingOversteps(SD_RIGHT).GetCount(); ++i)
	{
		const VBasicOverstepPtr & over = ds->GetCantingOversteps(SD_RIGHT).GetItem(i);

		sqlgen.Init("msr_ovr_canting");
		sqlgen.AddInteger("msr_trail_id", trail_id);
		sqlgen.AddSQL("enu_side_id", "select id from enu_side where s_name='П'");
		sqlgen.AddFloat("start_abs", over->GetStart());
		sqlgen.AddFloat("end_abs", over->GetEnd());
		sqlgen.AddFloat("abscoord", over->GetCoord());
		sqlgen.AddFloat("val_avg", over->GetAverage());
		sqlgen.AddFloat("val_max", over->GetValue());
		sqlstr = sqlgen.GetInsertSQL();

		try	
		{
			db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(...)
		{
			error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
			return error;
		}

		if (prog)
		{
			prog->Step(0);
			prog->Update();
		}
	}

	if (prog)
	{
		prog->SetMessage(0, L"Импорт оценки ПР/НГР");
	}

	for (size_t i = 0; i < ds->GetPrNgrAreas().GetCount(); ++i)
	{
		const VPrNgrAreaDescr & area = ds->GetPrNgrAreas().GetItem(i);
		if (area.railsType.empty() || area.railsBrace.empty()) continue;

		sqlgen.Init("msr_sections_canting");
		sqlgen.AddInteger("msr_trail_id", trail_id);

		sqlgen.AddSQL("enu_rails_type_id", 
			"select id from enu_rails_type where type_of_rail='" + 
			wstring_to_string(area.railsType) + "'");

		sqlgen.AddSQL("enu_brace_type_id", 
			"select id from enu_brace_type where type_of_brace='" + 
			wstring_to_string(area.railsBrace) + "'");

		sqlgen.AddInteger("speed1", area.maxSpeed1);

		sqlgen.AddInteger("start_km", area.section.start.km);
		sqlgen.AddFloat("start_m", area.section.start.m);
		sqlgen.AddInteger("end_km", area.section.end.km);
		sqlgen.AddFloat("end_m", area.section.end.m);

		sqlgen.AddFloat("pr_avg_l", area.prLeftAvg);
		sqlgen.AddFloat("pr_min_l", area.prLeftMin);
		sqlgen.AddFloat("pr_max_l", area.prLeftMax);
		sqlgen.AddFloat("pr_less_1div60_l", area.lenPrLeftUnderEdge);
		sqlgen.AddFloat("pr_more_1div12_l", area.lenPrLeftOverEdge);
		sqlgen.AddFloat("pr_avg_r", area.prRightAvg);
		sqlgen.AddFloat("pr_min_r", area.prRightMin);
		sqlgen.AddFloat("pr_max_r", area.prRightMax);
		sqlgen.AddFloat("pr_less_1div60_r", area.lenPrRightUnderEdge);
		sqlgen.AddFloat("pr_more_1div12_r", area.lenPrRightOverEdge);
		sqlgen.AddFloat("ngr_avg_l", area.ngrLeftAvg);
		sqlgen.AddFloat("ngr_min_l", area.ngrLeftMin);
		sqlgen.AddFloat("ngr_max_l", area.ngrLeftMax);
		sqlgen.AddFloat("ngr_less_1div60_l", area.lenNgrLeftUnderEdge);
		sqlgen.AddFloat("ngr_more_1div12_l", area.lenNgrLeftOverEdge);
		sqlgen.AddFloat("ngr_avg_r", area.ngrRightAvg);
		sqlgen.AddFloat("ngr_min_r", area.ngrRightMin);
		sqlgen.AddFloat("ngr_max_r", area.ngrRightMax);
		sqlgen.AddFloat("ngr_less_1div60_r", area.lenNgrRightUnderEdge);
		sqlgen.AddFloat("ngr_more_1div12_r", area.lenNgrRightOverEdge);
		sqlstr = sqlgen.GetInsertSQL();

		try	
		{
			db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
		}
		catch(...)
		{
			error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
			return error;
		}

		if (prog)
		{
			prog->Step(0);
			prog->Update();
		}
	}

	sql << "update msr_trail set ";
	sql << "pr_ngr_count = (select count(id) from msr_ovr_canting where msr_trail_id=" << trail_id << " + ";
	sql << "(select count(id) from msr_sections_canting where msr_trail_id=" << trail_id << ", ";
	sql << "pr_ngr_date = '" << VTime().ToStringISO() << "' ";
	sql << "where id=" << trail_id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}


	return L"";
}

// ---------------------------------------------------------------------------
// Оценка проектных скоростей
// ---------------------------------------------------------------------------

std::wstring AddPlanSpd(VIBDatabasePtr db, VIBTransaction & trans,
						VDataSetPtr ds, __int64 trail_id, VProgressBox * prog)
{
	std::stringstream sql;
	sql << std::fixed << std::setprecision(6);

	VSQLGenerator sqlgen;
	std::vector<VIBParameter> sqlprm;
	std::string sqlstr;

	std::wstring error;

	VIBDataHandlerParameter handler;

	// удаление предыдущей оценки
	if (prog)
	{
		prog->SetMessage(0, L"Оценка по план.скоростям :: очистка таблиц");
		prog->SetRange(0, 0);
		prog->SetPosition(0, 0);
		prog->Update();
	}

	sql << "delete from speed_sections where msr_trail_id = " << trail_id << ";";
	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(VException &)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	sql << "update msr_trail set planspd_count=0, planspd_date=NULL where id = " << trail_id << ";";
	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(VException &)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	// список вариантов
	if (prog)
	{
		prog->SetMessage(0, L"Оценка по план.скоростям :: загрузка списка участков");
		prog->Update();
	}

	if (!ds->Load(PRM_ABS_COORD))
	{
		error = ds->GetErrorMessage();
		LOG_ERROR(error);
		return error;
	}
	VAbsCoordData & absCoord = ds->GetAbsCoord();

	sql << "select sp.id, sp.var_name, sp.start_km, sp.end_km, ";
	sql << "(select abscoord from get_abscoord(sp.adm_way_id, sp.start_km, sp.start_m, mt.date_trail)), ";
	sql << "(select abscoord from get_abscoord(sp.adm_way_id, sp.end_km, sp.end_m, mt.date_trail)) ";
	sql << "from speed_project sp inner join msr_trail mt on ";
	sql << "sp.adm_way_id = mt.adm_way_id where mt.id = '" << trail_id << "';";
	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(VException &)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		db->RollbackTransaction(trans);
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	std::vector<__int64> var_id, var_start_km, var_end_km;
	std::vector<std::wstring> var_name;
	std::vector<double> var_begin, var_end;
	for (size_t i = 0; i < handler.GetRowCount(); ++i)
	{
		double cur_var_begin = handler.GetParameter(i, 4).GetFloat();
		double cur_var_end = handler.GetParameter(i, 5).GetFloat();
		if (cur_var_begin > absCoord.GetLastItem() || cur_var_end < absCoord.GetItem(0)) continue;

		var_id.push_back(handler.GetParameter(i, 0).GetInteger());
		var_name.push_back(handler.GetParameter(i, 1).GetString());
		var_start_km.push_back(handler.GetParameter(i, 2).GetInteger());
		var_end_km.push_back(handler.GetParameter(i, 3).GetInteger());
		var_begin.push_back(cur_var_begin);
		var_end.push_back(cur_var_end);
	}

	if (!var_id.empty())
	{
		// данные для оценки
		// шк
		if (prog)
		{
			prog->SetMessage(0, L"Оценка по план.скоростям :: загрузка данных :: ШК");
			prog->Step(0);
			prog->Update();
		}

		if (!ds->Load(PRM_WIDTH))
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}
		VParameter & width = ds->GetParameter(PRM_WIDTH);

		// уровень
		if (prog)
		{
			prog->SetMessage(0, L"Оценка по план.скоростям :: загрузка данных :: уровень");
			prog->Step(0);
			prog->Update();
		}

		if (!ds->Load(PRM_LEVEL))
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}
		VParameter & level = ds->GetParameter(PRM_LEVEL);

		// статистика
		if (prog)
		{
			prog->SetMessage(0, L"Оценка по план.скоростям :: загрузка данных :: статистика");
			prog->Step(0);
			prog->Update();
		}

		if (!ds->Load(PRM_STATISTICS))
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}
		VStatistics & stat = ds->GetStatistics();

		// неровности профиля
		if (prog)
		{
			prog->SetMessage(0, L"Оценка по план.скоростям :: загрузка данных :: неровности профиля");
			prog->Step(0);
			prog->Update();
		}

		if (!ds->Load(PRM_JAGGY_PROFILE))
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}
		VParameter & jaggy_prof = ds->GetParameter(PRM_JAGGY_PROFILE);

		// неровности плана
		if (prog)
		{
			prog->SetMessage(0, L"Оценка по план.скоростям :: загрузка данных :: неровности плана");
			prog->Step(0);
			prog->Update();
		}

		if (!ds->Load(PRM_JAGGY_PLAN))
		{
			error = ds->GetErrorMessage();
			LOG_ERROR(error);
			return error;
		}
		VParameter & jaggy_plan = ds->GetParameter(PRM_JAGGY_PLAN);

		// кривые
		if (prog)
		{
			prog->SetMessage(0, L"Оценка по план.скоростям :: загрузка данных :: кривые");
			prog->Step(0);
			prog->Update();
		}

		if (!ds->Load(PRM_CURVE_MARKS))
		{
			LOG_WARNING(L"В файле отсутствует разметка кривых");
		}
		VCurveList & curves = ds->GetCurves(PRM_CURVE_MARKS);

		for (size_t var_idx = 0; var_idx < var_id.size(); ++var_idx)
		{
			// id & абс. границы участка план.скоростей
			std::wstring project_name = var_name[var_idx];
			__int64 project_id = var_id[var_idx];
			double project_start = var_begin[var_idx];
			double project_end = var_end[var_idx];
			__int64 project_start_km = var_start_km[var_idx];
			__int64 project_end_km = var_end_km[var_idx];

			if (prog)
			{
				prog->SetMessage(0, (L"Оценка по план.скоростям :: вариант " + project_name).c_str());
				prog->Step(0);
				prog->Update();
			}

			VAprinfo & aprInfo = ds->GetAprinfo();

			// список проектных скоростей
			sql << "select start_km, start_m, end_km, end_m, speed1, speed2 ";
			sql << "from speed_speeds where speed_project_id = " << project_id << " ";
			sql << "order by start_km, start_m";
			try
			{
				db->ExecuteStatement(trans, sql.str().c_str(), handler);
			}
			catch(VException &)
			{
				error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
				LOG_ERROR(error);
				return error;
			}
			if (handler.GetRowCount() == 0)
			{
				error = L"Список план.скоростей пуст";
				LOG_ERROR(error);
				return error;
			}
			sql.str("");

			std::vector<VAprinfoSpeedRecord> spdlist;
			for (size_t i = 0; i < handler.GetRowCount(); ++i)
			{
				VAprinfoSpeedRecord spd;

				spd.base = aprInfo.GetAbsCoord(VWayCoord(
					handler.GetParameter(i, 0).GetInteger(),
					handler.GetParameter(i, 1).GetFloat()));

				spd.end = aprInfo.GetAbsCoord(VWayCoord(
					handler.GetParameter(i, 2).GetInteger(),
					handler.GetParameter(i, 3).GetFloat()));

				spd.spd = VSpeeds(handler.GetParameter(i, 4).GetInteger(), 
					handler.GetParameter(i, 5).GetInteger(), 60);

				spdlist.push_back(spd);
			}

			// причёсывание границ
			spdlist.front().base = aprInfo.GetAbsCoordBase();
			spdlist.back().end = aprInfo.GetAbsCoordEnd();
			for (size_t i = 1; i < spdlist.size(); ++i)
			{
				double axis = 0.5 * (spdlist[i-1].end +spdlist[i].base);
				spdlist[i-1].end = axis;
				spdlist[i].base = axis;
			}

			// подмена скоростей на проектные
			if (!aprInfo.ReplaceSpeedList(spdlist))
			{
				error = L"Список планируемых скоростей не соответствует существующей априорной информации";
				LOG_ERROR(error);
				return error;
			}

			// граница поездки
			ds->Load(PRM_ABS_COORD);
			double trail_start = ds->GetAbsCoord().GetItem(0);
			double trail_end = ds->GetAbsCoord().GetLastItem();
			if (trail_start >= project_end || trail_end <= project_start)
			{
				error = L"Участок план.скоростей не пересекается с поездкой";
				LOG_ERROR(error);
				return error;
			}

			// граница секции оценки
			double section_start = std::max<double>(trail_start, project_start);
			double section_end = std::min<double>(trail_end, project_end);

			// занесение новой секции
			sqlgen.Init("speed_sections");
			sqlgen.AddInteger("msr_trail_id", trail_id);
			sqlgen.AddInteger("speed_project_id", project_id);
			sqlgen.AddFloat("start_abs", section_start);
			sqlgen.AddFloat("end_abs", section_end);
			sqlgen.AddString("prog_version", revision);
			sqlstr = sqlgen.GetInsertSQL();
			try
			{
				db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
			}
			catch(VException &)
			{
				error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
				LOG_ERROR(error);
				return error;
			}

			// id секции
			sql << "select id from speed_sections where ";
			sql << "msr_trail_id = " << trail_id << " and ";
			sql << "speed_project_id = " << project_id << ";";
			try
			{
				db->ExecuteStatement(trans, sql.str().c_str(), handler);
			}
			catch(VException &)
			{
				error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
				LOG_ERROR(error);
				return error;
			}
			sql.str("");

			if (handler.GetRowCount() == 0)
			{
				error = L"Вариант план.скоростей отсутствует в базе"; 
				LOG_ERROR(error);
				return error;
			}
			__int64 speed_sections_id = handler.GetParameter(0, 0).GetInteger();

			if (prog)
			{
				prog->SetMessage(0, (L"Оценка по план.скоростям :: вариант " + project_name + L" :: покилометровая оценка").c_str());
				prog->Step(0);
				prog->Update();
			}

			// общее число записей - км + кривые
			// если записей 0, оценка не имеет смысла
			int sumRecCount = 0;

			// километровая оценка
			for (__int64 km = project_start_km; km <= project_end_km; ++km)
			{
				VStatData km_data = stat.GetKmList().Find(km, 0);
				if (km_data.km != km)
				{
					continue;
				}

				// границы км с учетом границ секции
				double project_km_start = std::max<double>(km_data.absBegin, project_start);
				double project_km_end = std::min<double>(km_data.absEnd, project_end);

				// неполностью оцененный километр пропускается
				if (trail_start > project_km_start || trail_end < project_km_end) continue;

				// участок файла поездки, оценивающий км
				size_t idx_start = absCoord.FindIndex(project_km_start, true);
				size_t idx_end = absCoord.FindIndex(project_km_end, true);

				double ner_prof = 0, ner_plan = 0;
				size_t cnt_ner_prof = 0;

				double otkl_shk_plus = 0, otkl_shk_minus = 0;

				for (size_t idx = idx_start; idx < idx_end; ++idx)
				{
					ner_prof += 10.0 * fabs(jaggy_prof.GetItem(idx));
					++cnt_ner_prof;

					ner_plan = std::max<float>(ner_plan, fabs(jaggy_plan.GetItem(idx)));

					if (!width.IsCorrupt(idx))
					{
						double delta = width.GetItem(idx) - aprInfo.GetPassportCurveNormaWidth(absCoord.GetItem(idx));

						if (delta > 0)
						{
							if (delta > otkl_shk_plus) otkl_shk_plus = delta;
						}
						else
						{
							if (delta < otkl_shk_minus) otkl_shk_minus = delta;
						}
					}
				}
				if (cnt_ner_prof > 0) ner_prof /= cnt_ner_prof;

				sqlgen.Init("speed_km_estim");
				sqlgen.AddInteger("speed_sections_id", speed_sections_id);
				sqlgen.AddInteger("num_km", km);
				sqlgen.AddFloat("start_abs", km_data.absBegin);
				sqlgen.AddFloat("end_abs", km_data.absEnd);
				sqlgen.AddFloat("ner_prof", ner_prof);
				sqlgen.AddFloat("ner_plan", ner_plan);
				sqlgen.AddFloat("otkl_level_plus", km_data.maxLevel);
				sqlgen.AddFloat("otkl_level_minus", km_data.minLevel);
				sqlgen.AddFloat("otkl_shk_plus", otkl_shk_plus);
				sqlgen.AddFloat("otkl_shk_minus", otkl_shk_minus);
				sqlgen.AddFloat("sssp", km_data.sssp);
				sqlgen.AddString("prog_version", revision);

				sqlstr = sqlgen.GetInsertSQL();
				try
				{
					db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
				}
				catch(VException &)
				{
					error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
					LOG_ERROR(error);
					return error;
				}

				++sumRecCount;
			}

			if (prog)
			{
				prog->SetMessage(0, (L"Оценка по план.скоростям :: вариант " + project_name + L" :: оценка кривых").c_str());
				prog->Step(0);
				prog->Update();
			}

			VCurveExport crvx(CURVE_SPEED, ds, db, trans, speed_sections_id, 0, false);

			// оценка кривых
			int crvCur = 0;
			for (size_t i = 0; i < curves.GetCount(); ++i)
			{
			    if (i == 8)
			    {
			        int Z = 0;
			    }
			
				VCurve & crv = curves.GetItem(i);
				double crv_start = crv.GetRangeCoord(*ds, true, false, true);
				double crv_end = crv.GetRangeCoord(*ds, true, false, false);
				if (crv_end < project_start || crv_start > project_end) continue;

				++crvCur;
				double crv_axis = 0.5 * (crv_start + crv_end);

				if (prog)
				{
					std::wstringstream msg;
					msg << L"Оценка по план.скоростям :: вариант " << project_name;
					msg << L" :: кривая №" << i+1;
					prog->SetMessage(0, msg.str().c_str());
					prog->Step(0);
					prog->Update();
				}

				crv.DropCalculations();

				if (!crvx.Export(crv, i))
				{
					error = crvx.GetMessage();
					LOG_ERROR(error);
					return error;
				}

				++sumRecCount;
			}
		}
	}

	sql << "update msr_trail set ";
	sql << "planspd_count = (select count(id) from speed_sections where msr_trail_id=" << trail_id << ", ";
	sql << "planspd_date = '" << VTime().ToStringISO() << "' ";
	sql << "where id=" << trail_id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str());
	}
	catch(...)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	return L"";
}

