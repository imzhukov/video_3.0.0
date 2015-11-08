#include <algorithm>

#include "DataExport.h"
#include "revision.h"
#include "VProgressInterface.h"
#include "VFileName.h"
#include "dblib.h"
#include "dbfunc.h"
#include "VCurveCard.h"
#include "sqlgen.h"

// Обновление информации о поездке на участке план.скорости
std::wstring PlanSpeedExport(VIBDatabasePtr db, const VFileName & fn, VProgressBox & prog)
{
	// поток в памяти для формирования запросов
	std::stringstream sql;

	// поток в памяти для формирования сообщений
	std::wstringstream msg;

	// транзакция
	VIBTransaction trans = db->StartTransaction();

	// обработчик данных из базы
	VIBDataHandlerParameter handler;

	// генератор запросов
	VSQLGenerator sqlgen;

	// запрос из генератора
	std::string sqlstr;

	// суммарное сообщение об ошибке
	std::wstring error;

	// ---------------------------------------------------------------------------

	msg << _T("Обновление поездки ") << fn.GetShortName();
	LOG_INFO(msg.str());
	msg.str(_T(""));

	// id поездки в базе
	sql << "select id, ";
	sql << "(select abscoord from get_abscoord(mt.adm_way_id, mt.start_km, mt.start_m, mt.date_trail)), ";
	sql << "(select abscoord from get_abscoord(mt.adm_way_id, mt.end_km, mt.end_m, mt.date_trail)) ";
	sql << "from msr_trail mt where mt.filename = '" << fn.GetShortName() << "';";
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

	if (handler.GetRowCount() == 0)
	{
		error = _T("Поездка отсутствует в базе"); 
		db->RollbackTransaction(trans);
		LOG_ERROR(error);
		return error;
	}

	__int64 trail_id = handler.GetParameter(0, 0).GetInteger();
	double trail_start = handler.GetParameter(0, 1).GetFloat();
	double trail_end = handler.GetParameter(0, 2).GetFloat();

	// список вариантов
	sql << "select sp.id, sp.var_name, sp.start_km, sp.end_km, ";
	sql << "(select abscoord from get_abscoord(sp.adm_way_id, sp.start_km, sp.start_m, mt.date_trail)), ";
	sql << "(select abscoord from get_abscoord(sp.adm_way_id, sp.end_km, sp.end_m, mt.date_trail)) ";
	sql << "from speed_project sp inner join msr_trail mt on ";
	sql << "sp.adm_way_id = mt.adm_way_id where mt.filename = '" << fn.GetShortName() << "';";
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
		if (cur_var_begin > trail_end || cur_var_end < trail_start) continue;

		var_id.push_back(handler.GetParameter(i, 0).GetInteger());
		var_name.push_back(handler.GetParameter(i, 1).GetString());
		var_start_km.push_back(handler.GetParameter(i, 2).GetInteger());
		var_end_km.push_back(handler.GetParameter(i, 3).GetInteger());
		var_begin.push_back(cur_var_begin);
		var_end.push_back(cur_var_end);
	}

	if (var_id.empty())
	{
		db->RollbackTransaction(trans);
		return _T("");
	}

	prog.SetMessage(0, (fn.GetShortName() + _T(" :: загрузка данных")).c_str());
	prog.Update();

	// набор данных
	VDataSetPtr ds(new VDataSet);
	if (!ds->OpenFile(fn))
	{
		error = _T("Не удалось открыть файл"); 
		db->RollbackTransaction(trans);
		LOG_ERROR(error);
		return error;
	}

	// абс.к-та
	if (!ds->Load(PRM_ABS_COORD))
	{
		error = ds->GetErrorMessage();
		db->RollbackTransaction(trans);
		LOG_ERROR(error);
		return error;
	}
	VAbsCoordData & absCoord = ds->GetAbsCoord();

	// апринфо из базы
	try
	{
		if (!ds->AprinfoLoadFromDB(*db, APRINFO_STANDARD))
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
	VAprinfo & aprInfo = ds->GetAprinfo();

	// шк
	if (!ds->Load(PRM_WIDTH))
	{
		error = ds->GetErrorMessage();
		db->RollbackTransaction(trans);
		LOG_ERROR(error);
		return error;
	}
	VParameter & width = ds->GetParameter(PRM_WIDTH);

	// уровень
	if (!ds->Load(PRM_LEVEL))
	{
		error = ds->GetErrorMessage();
		db->RollbackTransaction(trans);
		LOG_ERROR(error);
		return error;
	}
	VParameter & level = ds->GetParameter(PRM_LEVEL);

	prog.SetMessage(0, (fn.GetShortName() + _T(" :: статистика")).c_str());
	prog.Update();

	// статистика
	if (!ds->Load(PRM_STATISTICS))
	{
		error = ds->GetErrorMessage();
		db->RollbackTransaction(trans);
		LOG_ERROR(error);
		return error;
	}
	VStatistics & stat = ds->GetStatistics();

	prog.SetMessage(0, (fn.GetShortName() + _T(" :: неровности профиля")).c_str());
	prog.Update();

	// неровности профиля
	if (!ds->Load(PRM_JAGGY_PROFILE))
	{
		error = ds->GetErrorMessage();
		db->RollbackTransaction(trans);
		LOG_ERROR(error);
		return error;
	}
	VParameter & jaggy_prof = ds->GetParameter(PRM_JAGGY_PROFILE);

	prog.SetMessage(0, (fn.GetShortName() + _T(" :: неровности плана")).c_str());
	prog.Update();

	// неровности плана
	if (!ds->Load(PRM_JAGGY_PLAN))
	{
		error = ds->GetErrorMessage();
		db->RollbackTransaction(trans);
		LOG_ERROR(error);
		return error;
	}
	VParameter & jaggy_plan = ds->GetParameter(PRM_JAGGY_PLAN);

	// кривые
	if (!ds->Load(PRM_CURVE_MARKS))
	{
		LOG_WARNING(_T("В файле отсутствует разметка кривых"));
	}
	VCurveList & curves = ds->GetCurves(PRM_CURVE_MARKS);

	// удаление предыдущей оценки
	sql << "delete from speed_sections where msr_trail_id = " << trail_id << ";";
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

	sql << "update msr_trail set planspd_count = 0 where id = " << trail_id << ";";
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

	for (size_t var_idx = 0; var_idx < var_id.size(); ++var_idx)
	{
		// id & абс. границы участка план.скоростей
		std::wstring project_name = var_name[var_idx];
		__int64 project_id = var_id[var_idx];
		double project_start = var_begin[var_idx];
		double project_end = var_end[var_idx];
		__int64 project_start_km = var_start_km[var_idx];
		__int64 project_end_km = var_end_km[var_idx];

		prog.SetMessage(0, (fn.GetShortName() + _T(": вариант ") + project_name).c_str());
		prog.Update();
			
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
//			error = e.GetDescription();
//			error += _T(": ");
//			error += string_to_wstring(db->GetErrorDescription());
			db->RollbackTransaction(trans);
			LOG_ERROR(error);
			return error;
		}
		if (handler.GetRowCount() == 0)
		{
			db->RollbackTransaction(trans);
			error = _T("Список план.скоростей пуст");
			LOG_ERROR(error);
			return error;
		}
		sql.str("");

		std::vector<VAprinfoSpeedRecord> spdlist;
		for (size_t i = 0; i < handler.GetRowCount(); ++i)
		{
			VAprinfoSpeedRecord spd;

			spd.base = aprInfo.GetAbsCoord(VWayCoord(
				handler.GetParameter(0, 0).GetInteger(),
				handler.GetParameter(0, 1).GetFloat()));

			spd.end = aprInfo.GetAbsCoord(VWayCoord(
				handler.GetParameter(0, 2).GetInteger(),
				handler.GetParameter(0, 3).GetFloat()));

			spd.spd1 = handler.GetParameter(0, 4).GetInteger();
			spd.spd2 = handler.GetParameter(0, 5).GetInteger();
			spd.spd3 = 60;
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
			db->RollbackTransaction(trans);
			error = _T("Список планируемых скоростей не соответствует существующей априорной информации");
			LOG_ERROR(error);
			return error;
		}

		// граница поездки
		ds->Load(PRM_ABS_COORD);
		double trail_start = ds->GetAbsCoord().GetItem(0);
		double trail_end = ds->GetAbsCoord().GetLastItem();
		if (trail_start >= project_end || trail_end <= project_start)
		{
			error = _T("Участок план.скоростей не пересекается с поездкой");
			LOG_ERROR(error);
			return error;
		}

		// граница секции оценки
		double section_start = max(trail_start, project_start);
		double section_end = min(trail_end, project_end);

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
			db->RollbackTransaction(trans);
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
			db->RollbackTransaction(trans);
			LOG_ERROR(error);
			return error;
		}
		sql.str("");

		if (handler.GetRowCount() == 0)
		{
			error = _T("Вариант план.скоростей отсутствует в базе"); 
			db->RollbackTransaction(trans);
			LOG_ERROR(error);
			return error;
		}
		__int64 speed_sections_id = handler.GetParameter(0, 0).GetInteger();

		prog.SetMessage(0, (fn.GetShortName() + _T(" :: вариант ") + project_name + _T(" :: покилометровая оценка")).c_str());
		prog.Update();

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
			double project_km_start = max(km_data.absBegin, project_start);
			double project_km_end = min(km_data.absEnd, project_end);

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

				ner_plan = max(ner_plan, fabs(jaggy_plan.GetItem(idx)));

				if (!width.IsCorrupt(idx))
				{
					double delta = width.GetItem(idx) - aprInfo.GetNormaWidth(absCoord.GetItem(idx));
					
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
				db->RollbackTransaction(trans);
				LOG_ERROR(error);
				return error;
			}

			++sumRecCount;
		}

		prog.SetMessage(0, (fn.GetShortName() + _T(" :: вариант ") + project_name + _T(" :: оценка кривых")).c_str());
		prog.Update();

		// оценка кривых
		int crvCur = 0;
		for (size_t i = 0; i < curves.GetCount(); ++i)
		{
			VCurve & crv = curves.GetItem(i);
			double crv_start = crv.GetRangeCoord(*ds, true, false, true);
			double crv_end = crv.GetRangeCoord(*ds, true, false, false);
			if (crv_end < project_start || crv_start > project_end) continue;

			++crvCur;
			double crv_axis = 0.5 * (crv_start + crv_end);

			msg.str(_T(""));
			msg << fn.GetShortName() << _T(" :: вариант ") << project_name;
			msg << _T(" :: кривая №") << i+1;
			prog.SetMessage(0, msg.str().c_str());

			crv.DropCalculations();

			sqlgen.Init("speed_curve_estim");
			sqlgen.AddInteger("number_in_trail", i+1);
			sqlgen.AddInteger("enu_side_id", crv.GetOrientation(*ds));
			sqlgen.AddInteger("enu_waycat_id", crv.GetWayCat(*ds));

			sqlgen.AddInteger("speed_sections_id", speed_sections_id);

			double curAbsCoord = 0.0;
			float value = crv.GetMaxAnpPass(*ds, &curAbsCoord);
			sqlgen.AddFloat("anp_pass_max_abs", curAbsCoord);
			sqlgen.AddFloat("anp_pass_max", value);
			sqlgen.AddFloat("anp_truck_max", crv.GetMaxAnpTruck(*ds));
			sqlgen.AddFloat("anp_pass_avg", crv.GetAvgAnpPass(*ds));
			sqlgen.AddFloat("anp_truck_avg", crv.GetAvgAnpTruck(*ds));
			sqlgen.AddFloat("anp_truck_max_v60", crv.GetMaxAnpThird(*ds));
			sqlgen.AddFloat("anp_truck_avg_v60", crv.GetAvgAnpThird(*ds));

			curAbsCoord = 0.0;
			value = crv.GetMaxPsi(*ds, &curAbsCoord);
			sqlgen.AddFloat("psi_pass_max_abs", curAbsCoord);
			sqlgen.AddFloat("psi_pass_max", value);

			sqlgen.AddFloat("p_crv", crv.GetPRPlan(*ds));
			sqlgen.AddFloat("p_lvl", crv.GetPRLevel(*ds));
			sqlgen.AddFloat("p_coord", crv.GetPRCoord(*ds));
			sqlgen.AddFloat("p_anp", crv.GetPRAnp(*ds));
			sqlgen.AddFloat("p_int", crv.GetPRSum(*ds));

			sqlgen.AddFloat("degr_crv", crv.GetDegreePlan(*ds));
			sqlgen.AddFloat("degr_lvl", crv.GetDegreeLevel(*ds));
			sqlgen.AddFloat("degr_coord", crv.GetDegreeCoord(*ds));
			sqlgen.AddFloat("degr_anp", crv.GetDegreeAnp(*ds));

			sqlgen.AddFloat("angle", fabs(crv.GetAngle(*ds)));
			sqlgen.AddFloat("radius_avg_circ", crv.GetAvgRadius(*ds));
			sqlgen.AddFloat("radius_max_circ", crv.GetMaxRadius(*ds));
			sqlgen.AddFloat("radius_min_circ", crv.GetMinRadius(*ds));
			sqlgen.AddFloat("lvl_avg_circ", crv.GetAvgLevel(*ds));
			sqlgen.AddFloat("lvl_max_circ", crv.GetMaxLevel(*ds));
			sqlgen.AddFloat("lvl_min_circ", crv.GetMinLevel(*ds));
			sqlgen.AddFloat("width_avg_circ", crv.GetAvgWidth(*ds));
			sqlgen.AddFloat("width_max_circ", crv.GetMaxWidth(*ds));
			sqlgen.AddFloat("width_min_circ", crv.GetMinWidth(*ds));
			sqlgen.AddFloat("spd_anp_plus_03", crv.GetVPlus03(*ds));
			sqlgen.AddFloat("spd_anp_minus_03", crv.GetVMinus03(*ds));
			sqlgen.AddFloat("wear_avg", crv.GetAvgWear(*ds));
			sqlgen.AddFloat("wear_max", crv.GetMaxWear(*ds));
			sqlgen.AddFloat("wear_len_over6", crv.GetWearLenOverLimit(*ds, 6.0f));
			sqlgen.AddFloat("wear_len_over10", crv.GetWearLenOverLimit(*ds, 10.0f));
			sqlgen.AddFloat("wear_len_over15", crv.GetWearLenOverLimit(*ds, 15.0f));
			sqlgen.AddFloat("wear_bug_percent", crv.GetWearErrorPercent(*ds));
			sqlgen.AddFloat("width_bug_percent", crv.GetWidthErrorPercent(*ds));
			sqlgen.AddFloat("incr_width_plus_max", crv.GetMaxDeltaWidthFront(*ds));
			sqlgen.AddFloat("incr_width_plus_avg", crv.GetAvgDeltaWidthFront(*ds));
			sqlgen.AddFloat("incr_width_minus_max", crv.GetMaxDeltaWidthBack(*ds));
			sqlgen.AddFloat("incr_width_minus_avg", crv.GetAvgDeltaWidthBack(*ds));

			// график для карточки
			ISC_QUAD graph_id;
			std::stringstream static_graph;
			VCurveCard card(ds, PRM_CURVE_MARKS, i);
			try
			{
				card.Prepare();
				card.SaveVisible(static_graph);
				if (!static_graph.str().empty())
				{
					boost::scoped_ptr<VBlob> blob(new VBlob(static_graph));
					graph_id = db->AddBlob(trans, *blob);
					static_graph.str("");				
					sqlgen.AddParameter("picture", VIBParameter(SQL_BLOB+1, (char*)&graph_id));
				}
			}
			catch(VException & ex)
			{
				error = _T("Сбой при создании картинки\n") + ex.GetDescription() + _T("\n\n") + ex.GetDetails();
				db->RollbackTransaction(trans);
				LOG_ERROR(error);
				return error;
			}
			catch(std::exception & ex)
			{
				error = _T("Сбой при создании картинки\n") + string_to_wstring(ex.what());
				db->RollbackTransaction(trans);
				LOG_ERROR(error);
				return error;
			}

			sqlgen.AddString("prog_version", revision);
			sqlstr = sqlgen.GetInsertSQL();
			try
			{
				db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
			}
			catch(VException &)
			{
				error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
				db->RollbackTransaction(trans);
				LOG_ERROR(error);
				return error;
			}

			// id кривой
			VIBDataHandlerParameter searchCrvHandler;
			sql << "select id from speed_curve_estim where number_in_trail = " << i+1;
			sql << " and speed_sections_id = " << speed_sections_id << ";";
			try
			{
				db->ExecuteStatement(trans, sql.str().c_str(), searchCrvHandler);
			}
			catch(...)
			{
				error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
				db->RollbackTransaction(trans);
				LOG_ERROR(error);
				return error;
			}
			sql.str("");

			if (searchCrvHandler.GetRowCount() == 0)
			{
				LOG_WARNING(_T("Только что занесённая кривая не обнаружена в базе :("));
				continue;
			}
			int64_t crv_id = searchCrvHandler.GetParameter(0, 0).GetInteger();

			// метки кривой
			for (size_t markNdx = 0; markNdx < crv.GetMarkCount(); ++markNdx)
			{
				sqlgen.Init("speed_curve_mark");
				sqlgen.AddInteger("speed_curve_estim_id", crv_id);
				sqlgen.AddInteger("num_mark", markNdx);
				sqlgen.AddFloat("crv_abs", absCoord.GetItem(crv.GetPlanMark(markNdx).index));
				sqlgen.AddFloat("crv_value", crv.GetPlanMark(markNdx).value);
				sqlgen.AddFloat("lvl_abs", absCoord.GetItem(crv.GetLevelMark(markNdx).index));
				sqlgen.AddFloat("lvl_value", crv.GetLevelMark(markNdx).value);
				sqlgen.AddFloat("latitude", crv.GetPlanMark(markNdx).latitude);
				sqlgen.AddFloat("longitude", crv.GetPlanMark(markNdx).longitude);
				sqlgen.AddString("prog_version", revision);

				sqlstr = sqlgen.GetInsertSQL();
				try
				{
					db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
				}
				catch(VException &)
				{
					error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
					db->RollbackTransaction(trans);
					LOG_ERROR(error);
					return error;
				}
			}

			// Участки скоростей
			std::vector<VCurveSpdArea> vprk_pass  = crv.GetVprkPass(*ds);
			std::vector<VCurveSpdArea> vprk_truck = crv.GetVprkTruck(*ds);
			std::vector<VCurveSpdArea> vkr_pass  = crv.GetVkrPass(*ds);
			std::vector<VCurveSpdArea> vkr_truck = crv.GetVkrTruck(*ds);
			std::vector<VCurveSpdArea> vpr  = crv.GetVpr(*ds);
			std::vector<VCurveSpdArea> viz = crv.GetViz(*ds);

			for (size_t spdNdx = 0; spdNdx < vprk_pass.size(); ++spdNdx)
			{
				sqlgen.Init("speed_curve_spdarea");
				sqlgen.AddInteger("speed_curve_estim_id", crv_id);
				sqlgen.AddFloat("start_abs", vprk_pass[spdNdx].begin);
				sqlgen.AddFloat("end_abs", vprk_pass[spdNdx].end);
				sqlgen.AddInteger("spd_vpz_pass", vprk_pass[spdNdx].spd);
				sqlgen.AddInteger("spd_vpz_truck", vprk_truck[spdNdx].spd);
				sqlgen.AddInteger("spd_vkr_pass", vkr_pass[spdNdx].spd);
				sqlgen.AddInteger("spd_vkr_truck", vkr_truck[spdNdx].spd);
				sqlgen.AddInteger("spd_viz", viz[spdNdx].spd);
				sqlgen.AddInteger("spd_vpr", vpr[spdNdx].spd);
				sqlgen.AddString("prog_version", revision);
				sqlstr = sqlgen.GetInsertSQL();

				try
				{
					db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
				}
				catch(VException &)
				{
					error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
					db->RollbackTransaction(trans);
					LOG_ERROR(error);
					return error;
				}
			}

			// элементарные кривые
			size_t radiusCount = crv.GetRadiusCount();
			for (size_t radiusNdx = 0; radiusNdx < radiusCount; ++radiusNdx)
			{
				// Расчет оценки кривой
				int isLimAnp = 0;
				int isLimPsi = 0;
				int isLimTap = 0;
				float fMaxAnp = crv.GetMaxAnpPass(*ds, NULL, radiusNdx);
				float fAvgAnp = crv.GetAvgAnpPass(*ds, radiusNdx);
				float fMaxPsi = crv.GetMaxPsi(*ds, NULL, radiusNdx);
				if (fMaxAnp > crv.GetAnpPassRangeMax()+0.005)
				{
					isLimAnp = 1;
				}
				if (fMaxPsi > crv.GetPsiRangeMax()+0.005)
				{
					isLimPsi = 1;
				}

				vector<float> sd = crv.GetArrayVprkPass(*ds);
				vector<float> tap = crv.GetArrayTapLevel(*ds);
				int ssixe = tap.size();
				for (size_t i = 0; i < sd.size(); i++)
				{
					if (sd[i] >= 140 && tap[i] >= 0.705)
					{
						isLimTap = 1;
					}
					else if (sd[i] >= 120 && tap[i] >= 1.005) 
					{
						isLimTap = 1;
					}
				}

				sqlgen.Init("speed_elcurve_estim");
				sqlgen.AddInteger("speed_curve_estim_id", crv_id);

				sqlgen.AddFloat("anp_pass_max", fMaxAnp);
				sqlgen.AddFloat("anp_pass_avg", fAvgAnp);
				sqlgen.AddFloat("anp_truck_max", crv.GetMaxAnpTruck(*ds, radiusNdx));
				sqlgen.AddFloat("anp_truck_avg", crv.GetAvgAnpTruck(*ds, radiusNdx));
				sqlgen.AddFloat("psi_pass_max", fMaxPsi);
				sqlgen.AddInteger("is_limit_anp", isLimAnp);
				sqlgen.AddInteger("is_limit_psi", isLimPsi);
				sqlgen.AddInteger("is_limit_tap", isLimTap);
				sqlgen.AddInteger("radius_number", radiusNdx);
				sqlgen.AddFloat("angle", crv.GetAngle(*ds, radiusNdx));

				sqlgen.AddFloat("radius_avg_circ", crv.GetAvgRadius(*ds, radiusNdx));
				sqlgen.AddFloat("radius_max_circ", crv.GetMaxRadius(*ds, radiusNdx));
				sqlgen.AddFloat("radius_min_circ", crv.GetMinRadius(*ds, radiusNdx));

				sqlgen.AddFloat("lvl_avg_circ", crv.GetAvgLevel(*ds, radiusNdx));
				sqlgen.AddFloat("lvl_max_circ", crv.GetMaxLevel(*ds, radiusNdx));
				sqlgen.AddFloat("lvl_min_circ", crv.GetMinLevel(*ds, radiusNdx));

				sqlgen.AddFloat("width_avg_circ", crv.GetAvgWidth(*ds, radiusNdx));
				sqlgen.AddFloat("width_max_circ", crv.GetMaxWidth(*ds, radiusNdx));
				sqlgen.AddFloat("width_min_circ", crv.GetMinWidth(*ds, radiusNdx));

				sqlgen.AddFloat("wear_avg", crv.GetAvgWear(*ds, radiusNdx));
				sqlgen.AddFloat("wear_max", crv.GetMaxWear(*ds, radiusNdx));
				sqlgen.AddFloat("corr_wear", crv.GetCorrWear(*ds, radiusNdx));
				sqlgen.AddFloat("wear_bug_percent", crv.GetWearErrorPercent(*ds, radiusNdx));
				sqlgen.AddFloat("width_bug_percent", crv.GetWidthErrorPercent(*ds, radiusNdx));

				sqlgen.AddFloat("o_crv_max_1", crv.GetMaxPlanTap(*ds, radiusNdx));
				sqlgen.AddFloat("o_crv_avg_1", crv.GetAvgPlanTap(*ds, radiusNdx));
				sqlgen.AddFloat("o_lvl_max_1", crv.GetMaxLevelTap(*ds, radiusNdx));
				sqlgen.AddFloat("o_lvl_avg_1", crv.GetAvgLevelTap(*ds, radiusNdx));

				if (radiusNdx == radiusCount-1)
				{
					sqlgen.AddFloat("o_crv_max_2", crv.GetMaxPlanTap(*ds, radiusCount));
					sqlgen.AddFloat("o_crv_avg_2", crv.GetAvgPlanTap(*ds, radiusCount));
					sqlgen.AddFloat("o_lvl_max_2", crv.GetMaxLevelTap(*ds, radiusCount));
					sqlgen.AddFloat("o_lvl_avg_2", crv.GetAvgLevelTap(*ds, radiusCount));
				}
				else
				{
					sqlgen.AddFloat("o_crv_max_2", 0);
					sqlgen.AddFloat("o_crv_avg_2", 0);
					sqlgen.AddFloat("o_lvl_max_2", 0);
					sqlgen.AddFloat("o_lvl_avg_2", 0);
				}

				sqlgen.AddString("prog_version", revision);
				sqlstr = sqlgen.GetInsertSQL();

				try
				{
					db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
				}
				catch(VException &)
				{
					error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
					db->RollbackTransaction(trans);
					LOG_ERROR(error);
					return error;
				}

				// Участки скоростей
				vprk_pass  = crv.GetVprkPass(*ds, radiusNdx);
				vprk_truck = crv.GetVprkTruck(*ds, radiusNdx);
				vkr_pass  = crv.GetVkrPass(*ds, radiusNdx);
				vkr_truck = crv.GetVkrTruck(*ds, radiusNdx);
				vpr  = crv.GetVpr(*ds, radiusNdx);
				viz = crv.GetViz(*ds, radiusNdx);

				for (size_t spdNdx = 0; spdNdx < vprk_pass.size(); ++spdNdx)
				{
					sqlgen.Init("speed_elcurve_spdarea");

					sql << "select id from speed_elcurve_estim where speed_curve_estim_id = " << crv_id;
					sql << " and radius_number = " << radiusNdx;
					sqlgen.AddSQL("speed_elcurve_estim_id", sql.str());
					sql.str("");

					sqlgen.AddFloat("start_abs", vprk_pass[spdNdx].begin);
					sqlgen.AddFloat("end_abs", vprk_pass[spdNdx].end);

					sqlgen.AddInteger("spd_vpz_pass", vprk_pass[spdNdx].spd);
					sqlgen.AddInteger("spd_vpz_truck", vprk_truck[spdNdx].spd);
					sqlgen.AddInteger("spd_vkr_pass", vkr_pass[spdNdx].spd);
					sqlgen.AddInteger("spd_vkr_truck", vkr_truck[spdNdx].spd);
					sqlgen.AddInteger("spd_viz", viz[spdNdx].spd);
					sqlgen.AddInteger("spd_vpr", vpr[spdNdx].spd);

					sqlgen.AddString("prog_version", revision);
					sqlstr = sqlgen.GetInsertSQL();

					try
					{
						db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
					}
					catch(VException &)
					{
						error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
						db->RollbackTransaction(trans);
						LOG_ERROR(error);
						return error;
					}
				}
			}

			// Пикеты с ограничениями скоростей
			std::vector<VCurvePicket> crvPks = crv.GetPkList(*ds);
			for (size_t pkNdx = 0; pkNdx < crvPks.size(); ++pkNdx)
			{
				VCurvePicket & crvPk = crvPks[pkNdx];
				if (!crvPk.HasLimit()) continue;

				sqlgen.Init("speed_curve_picket_spd");
				sqlgen.AddInteger("speed_curve_estim_id", crv_id);

				sqlgen.AddInteger("km_piket", crvPk.km);
				sqlgen.AddInteger("num_piket", crvPk.pk);

				sqlgen.AddInteger("is_limit_vkr_pass", crvPk.isVkrPass);
				sqlgen.AddInteger("is_limit_vkr_truck", crvPk.isVkrTruck);
				sqlgen.AddInteger("is_limit_vpr", crvPk.isVpr);
				sqlgen.AddInteger("is_limit_viz", crvPk.isViz);

				sqlgen.AddString("prog_version", revision);
				sqlstr = sqlgen.GetInsertSQL();

				try
				{
					db->ExecuteStatementImmediate(trans, sqlstr.c_str(), sqlgen.GetParameters());
				}
				catch(VException &)
				{
					error = string_to_wstring(sqlstr + "\n\n" + db->GetErrorDescription());
					db->RollbackTransaction(trans);
					LOG_ERROR(error);
					return error;
				}
			}

			++sumRecCount;
		}
	}

	prog.SetMessage(0, (fn.GetShortName() + _T(" :: завершение операции")).c_str());
	prog.Update();

	sql	<< "update msr_trail mt set planspd_count = ";
	sql << "(select count(id) from speed_sections ss where ss.msr_trail_id = mt.id) ";
	sql << "where mt.id = " << trail_id << ";";
	try
	{
		db->ExecuteStatementImmediate(trans, sql.str().c_str(), sqlgen.GetParameters());
	}
	catch(VException &)
	{
		error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
		db->RollbackTransaction(trans);
		LOG_ERROR(error);
		return error;
	}
	sql.str("");

	// подтверждение транзакции
	try
	{
		db->CommitTransaction(trans);
	}
	catch(...)
	{
		error = string_to_wstring(db->GetErrorDescription());
	}

	msg.str(_T(""));
	if (error.empty())
	{
		msg << _T("Обновление файла ") << fn.GetShortName() << _T(" успешно завершено");
		LOG_INFO(msg.str());
	}
	else
	{
		msg << _T("Обновление файла ") << fn.GetShortName() << _T(" завершено с ошибками");
		LOG_WARNING(msg.str());
	}

	// voila
	return error;
}

