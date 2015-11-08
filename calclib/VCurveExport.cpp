#include "VCurveExport.h"
#include "VCurveCard.h"
#include "revision.h"

int GetWearDegree(double wear)
{
	int degree = 0;
	if (round(wear, 1) > 18.0)		degree = 4;
	else if (round(wear, 1) > 16.0)	degree = 3; 
	else if (round(wear, 1) > 14.0)	degree = 2;
	return degree;
}

std::string VCurveExport::GetTable(TABLE table) const
{
	switch(this->crvType)
	{
	case CURVE_MAGISTRAL:
		switch(table)
		{
		case TABLE_TRAIL:					return "msr_trail";
		case TABLE_PLAN_ELEM:				return "msr_plan_elem";
		case TABLE_CURVE:					return "msr_curve";
		case TABLE_MARK:					return "msr_curve_mark";
		case TABLE_SPDAREA:					return "msr_curve_spdarea_prk";
		case TABLE_SPDAREA_HS:				return "msr_curve_spdarea_hs";
		case TABLE_SPDAREA_HS_BT:			return "msr_curve_spdarea_hs_bt";
		case TABLE_RADIUS:					return "msr_elcurve";
		case TABLE_RADIUS_SPDAREA:			return "msr_elcurve_spdarea_prk";
		case TABLE_RADIUS_SPDAREA_HS:		return "msr_elcurve_spdarea_hs";
		case TABLE_RADIUS_SPDAREA_HS_BT:	return "msr_elcurve_spdarea_hs_bt";
		case TABLE_PICKET_SPDLIM:			return "msr_curve_picket_spd_prk";
		case TABLE_PICKET_WEAR:				return "msr_curve_picket_wear";
		case TABLE_LONG_RAILS:				return "msr_rails_length";
		case TABLE_WEAR_DENSITY:			return "msr_wear_density";
		default: return "(BAD TABLE FOR MAGISTRAL CURVES)";
		}

	case CURVE_STATION:
		switch(table)
		{
		case TABLE_TRAIL:			return "stw_trail";
		case TABLE_PLAN_ELEM:		return "stw_plan_elem";
		case TABLE_CURVE:			return "stw_curve";
		case TABLE_MARK:			return "stw_curve_mark";
		case TABLE_SPDAREA:			return "stw_curve_spdarea_prk";
		case TABLE_RADIUS:			return "stw_elcurve";
		case TABLE_RADIUS_SPDAREA:	return "stw_elcurve_spdarea_prk";
		case TABLE_PICKET_SPDLIM:	return "stw_curve_picket_spd_prk";
		default: return "(BAD TABLE FOR STATION CURVES)";
		}

	case CURVE_MODEL:
		switch(table)
		{
		case TABLE_TRAIL:					return "msr_trail";
		case TABLE_PLAN_ELEM:				return "msr_plan_elem_model";
		case TABLE_CURVE:					return "msr_curve_model";
		case TABLE_SPDAREA_HS:				return "msr_curve_model_spdarea_hs";
		case TABLE_SPDAREA_HS_BT:			return "msr_curve_model_spdarea_hs_bt";
		case TABLE_MARK:					return "msr_curve_mark_model";
		case TABLE_SPDAREA:					return "msr_curve_model_spdarea";
		case TABLE_RADIUS:					return "msr_elcurve_model";
		case TABLE_RADIUS_SPDAREA:			return "msr_elcurve_model_spdarea";
		case TABLE_RADIUS_SPDAREA_HS:		return "msr_elcurve_model_spdarea_hs";
		case TABLE_RADIUS_SPDAREA_HS_BT:	return "msr_elcurve_model_spdarea_hs_bt";
		default: return "(BAD TABLE FOR MODEL CURVES)";
		}

	case CURVE_REPAIR:
		switch(table)
		{
		case TABLE_TRAIL:			return "msr_trail";
		case TABLE_PLAN_ELEM:		return "msr_plan_elem";
		case TABLE_CURVE:			return "repair_curve_estim";
		case TABLE_MARK:			return "repair_curve_mark";
		case TABLE_SPDAREA:			return "repair_curve_spdarea";
		case TABLE_RADIUS:			return "repair_elcurve_estim";
		case TABLE_RADIUS_SPDAREA:	return "repair_elcurve_spdarea";
		case TABLE_PICKET_SPDLIM:	return "repair_curve_picket_spd";
		default: return "(BAD TABLE FOR REPAIR CURVES)";
		}

	case CURVE_SPEED:
		switch(table)
		{
		case TABLE_TRAIL:			return "speed_sections";
		case TABLE_CURVE:			return "speed_curve_estim";
		case TABLE_MARK:			return "speed_curve_mark";
		case TABLE_SPDAREA:			return "speed_curve_spdarea";
		case TABLE_RADIUS:			return "speed_elcurve_estim";
		case TABLE_RADIUS_SPDAREA:	return "speed_elcurve_spdarea";
		case TABLE_PICKET_SPDLIM:	return "speed_curve_picket_spd";
		default: return "(BAD TABLE FOR SPEED CURVES)";
		}

	default: return "(BAD CURVE TYPE)";
	}
}

// выполнить sql-запрос
bool VCurveExport::ExecuteStatement(const std::string & sql)
{
	try
	{
		this->dataBase->ExecuteStatement(this->transaction, sql.c_str(), this->handler);
	}
	catch(VException & e)
	{
		this->message = e.GetDescription() + L": ";
		this->message += string_to_wstring(this->dataBase->GetErrorDescription());
		this->message += L"\n\n" + string_to_wstring(sql);
		return false;
	}
	return true;
}

// выполнить sql-запрос
bool VCurveExport::ExecuteStatement(VSQLGenerator & sqlgen)
{
	std::string sql = sqlgen.GetInsertSQL();
	try
	{
		this->dataBase->ExecuteStatementImmediate(this->transaction, sql.c_str(), sqlgen.GetParameters());
	}
	catch(VException & e)
	{
		this->message = e.GetDescription() + L": ";
		this->message += string_to_wstring(this->dataBase->GetErrorDescription());
		this->message += L"\n\n" + string_to_wstring(sql);
		return false;
	}
	return true;
}

// общий экспорт участков скоростей
bool VCurveExport::ExportSpdAreas(const VCurve & curve, TABLE table, TABLE parent_table, 
								  __int64 parent_id, size_t radiusNdx)
{
	std::vector<VCurveSpdArea> vprkPass  = curve.GetVprkPass(*this->dataSet, radiusNdx);
	std::vector<VCurveSpdArea> vprkTruck = curve.GetVprkTruck(*this->dataSet, radiusNdx);

	std::vector<VCurveSpdArea> vprkThird;
	if (this->crvType == CURVE_MODEL) 
	{
		vprkThird = curve.GetVprkThird(*this->dataSet, radiusNdx);
	}

	std::vector<VCurveSpdArea> vkrPass  = curve.GetVkrPass(*this->dataSet, radiusNdx);
	std::vector<VCurveSpdArea> vkrTruck = curve.GetVkrTruck(*this->dataSet, radiusNdx);
	std::vector<VCurveSpdArea> vprPass  = curve.GetVprPass(*this->dataSet, this->crvType == CURVE_REPAIR, radiusNdx);
	std::vector<VCurveSpdArea> vprTruck  = curve.GetVprTruck(*this->dataSet, this->crvType == CURVE_REPAIR, radiusNdx);
	std::vector<VCurveSpdArea> vizPass = curve.GetVizPass(*this->dataSet, radiusNdx);
	std::vector<VCurveSpdArea> vizTruck = curve.GetVizTruck(*this->dataSet, radiusNdx);

	for (size_t spdNdx = 0; spdNdx < vprkPass.size(); ++spdNdx)
	{
		this->sqlgen.Init(this->GetTable(table));
		this->sqlgen.AddInteger(this->GetTable(parent_table) + "_id", parent_id);
		this->sqlgen.AddFloat("start_abs", vprkPass[spdNdx].begin);
		this->sqlgen.AddFloat("end_abs", vprkPass[spdNdx].end);

		if (this->crvType == CURVE_MODEL)
		{
			this->sqlgen.AddInteger("spd_vmodel_pass", vprkPass[spdNdx].spd);
			this->sqlgen.AddInteger("spd_vmodel_truck", vprkTruck[spdNdx].spd);
			this->sqlgen.AddInteger("spd_vmodel_avg", vprkThird[spdNdx].spd);
		}
		else 
		{
			this->sqlgen.AddInteger("spd_vpz_pass", vprkPass[spdNdx].spd);
			this->sqlgen.AddInteger("spd_vpz_truck", vprkTruck[spdNdx].spd);
		}

		this->sqlgen.AddInteger("spd_vkr_pass", vkrPass[spdNdx].spd);
		this->sqlgen.AddInteger("spd_vkr_truck", vkrTruck[spdNdx].spd);
		this->sqlgen.AddInteger("spd_viz_pass", vizPass[spdNdx].spd);
		this->sqlgen.AddInteger("spd_viz_truck", vizTruck[spdNdx].spd);
		this->sqlgen.AddInteger("spd_vpr_pass", vprPass[spdNdx].spd);
		this->sqlgen.AddInteger("spd_vpr_truck", vprTruck[spdNdx].spd);

		this->sqlgen.AddString("prog_version", revision);

		if (!this->ExecuteStatement(this->sqlgen))
		{
			return false;
		}
	}
	return true;
}

// экспорт участков высоких скоростей
bool VCurveExport::ExportHiSpdAreas(const VCurve & curve, TABLE table, TABLE parent_table, bool bt,
									__int64 parent_id, size_t radiusNdx)
{
	if (bt)
	{
		if (!this->hs_bt) return true; // quietly exit
	}
	else
	{
		if (!this->hs) return true; // quietly exit
	}

	std::vector<VCurveSpdArea> vprk, vkr, viz, vpr;

	if (bt)
	{
		vprk = curve.GetVprkHighBT(*this->dataSet, radiusNdx);
		vkr = curve.GetVkrHighBT(*this->dataSet, radiusNdx);
		viz = curve.GetVizHighBT(*this->dataSet, radiusNdx);
	}
	else
	{
		vprk = curve.GetVprkHigh(*this->dataSet, radiusNdx);
		vkr = curve.GetVkrHigh(*this->dataSet, radiusNdx);
		viz = curve.GetVizHigh(*this->dataSet, radiusNdx);
	}

	vpr = curve.GetVprPass(*this->dataSet, this->crvType == CURVE_REPAIR, radiusNdx);

	for (size_t spdNdx = 0; spdNdx < vprk.size(); ++spdNdx)
	{
		this->sqlgen.Init(this->GetTable(table));
		this->sqlgen.AddInteger(this->GetTable(parent_table) + "_id", parent_id);
		this->sqlgen.AddFloat("start_abs", vprk[spdNdx].begin);
		this->sqlgen.AddFloat("end_abs", vprk[spdNdx].end);

		if (this->crvType == CURVE_MODEL)
		{
			this->sqlgen.AddInteger("spd_vmodel", vprk[spdNdx].spd);
		}
		else
		{
			this->sqlgen.AddInteger("spd_vpz", vprk[spdNdx].spd);
		}

		this->sqlgen.AddInteger("spd_vkr", vkr[spdNdx].spd);
		this->sqlgen.AddInteger("spd_viz", viz[spdNdx].spd);
		this->sqlgen.AddInteger("spd_vpr", vpr[spdNdx].spd);
		this->sqlgen.AddString("prog_version", revision);

		if (!this->ExecuteStatement(this->sqlgen))
		{
			return false;
		}
	}
	return true;
}

// конструктор
VCurveExport::VCurveExport(CURVE_TYPE ct, VDataSetPtr ds, 
						   VIBDatabasePtr db, VIBTransaction & trans,
						   __int64 trail, __int64 repair, bool opt) : 
crvType(ct), dataSet(ds), dataBase(db), transaction(trans), 
trailId(trail), repairId(repair), optics(opt), hs(false), hs_bt(false)
{
	this->sql << std::fixed << std::setprecision(6);
}

const std::wstring & VCurveExport::GetMessage() const
{
	return this->message;
}

// загрузка данных
bool VCurveExport::Load()
{
	PARAMETER_TYPE params[] = { 
		(this->crvType == CURVE_MODEL ? PRM_MODEL_PLAN_ELEM_LIST : PRM_PLAN_ELEM_LIST),
		(this->crvType == CURVE_MODEL ? PRM_MODEL_CURVES : PRM_CURVE_MARKS),
		PRM_CURV, PRM_LEVEL, PRM_KURS, PRM_NONE
	};
	
	for (size_t i = 0; params[i] != PRM_NONE; ++i)
	{
		if (!this->dataSet->Load(params[i]))
		{
			this->message = this->dataSet->GetErrorMessage();
			return false;
		}
	}

	if ((this->crvType == CURVE_MAGISTRAL || this->crvType == CURVE_REPAIR) && this->optics)
	{
		this->dataSet->Load(PRM_WIDTH);
		this->dataSet->Load(PRM_UNI_WEAR);
	}

	return true;
}

bool VCurveExport::ClearTables()
{
	bool result = true;
	this->sql << "delete from " << this->GetTable(TABLE_PLAN_ELEM) << " where ";
	this->sql << this->GetTable(TABLE_TRAIL) << "_id = " << this->trailId << ";";
	result = this->ExecuteStatement(this->sql.str());
	this->sql.str("");

	if (!result) return false;

	if (this->crvType == CURVE_MAGISTRAL)
	{
		this->sql << "update " << this->GetTable(TABLE_TRAIL);
		this->sql << " set crv_count=0, crv_date=NULL, enu_iznos_type_id=NULL where id = " << this->trailId << ";";
	}
	else if (this->crvType == CURVE_MODEL)
	{
		this->sql << "update " << this->GetTable(TABLE_TRAIL);
		this->sql << " set pasp_count=0, pasp_date=NULL where id=" << this->trailId << ";";
	}
	else
	{
		return true;
	}

	result = this->ExecuteStatement(this->sql.str());
	this->sql.str("");
	return result;
}

// Запрос типа элемента плана
std::string VCurveExport::GetPlanElemTypeSQL(PLAN_ELEM_TYPE planElemType) const
{
	std::string descr;
	switch(planElemType)
	{
	case PLAN_ELEM_LINE:		descr = "Полная прямая"; break;
	case PLAN_ELEM_HALFLINE:	descr = "Неполная прямая"; break;
	case PLAN_ELEM_CURVE:		descr = "Полная кривая"; break;
	case PLAN_ELEM_HALFCURVE:	descr = "Неполная кривая"; break;
	case PLAN_ELEM_SIDEWAY:		descr = "Съезд на боковой путь"; break;
	default: return "";
	}
	return "select id from enu_elem_type where name = '" + descr + "'";
}

// экспорт элементов плана
bool VCurveExport::ExportPlanElem()
{
	VPlanElemList & peList = ((this->crvType == CURVE_MAGISTRAL || this->crvType == CURVE_STATION) ? 
		this->dataSet->GetPlanElemList() : 
		this->dataSet->GetModelPlanElemList());

	for (size_t i = 0; i < peList.GetCount(); ++i)
	{
		const VPlanElement & planElem = peList.GetItem(i);

		this->sqlgen.Init(this->GetTable(TABLE_PLAN_ELEM));
		this->sqlgen.AddInteger(this->GetTable(TABLE_TRAIL) + "_id", this->trailId);
		this->sqlgen.AddSQL("enu_elem_type_id", this->GetPlanElemTypeSQL(planElem.type));
		this->sqlgen.AddFloat("start_abs", planElem.absStart);
		this->sqlgen.AddFloat("end_abs", planElem.absEnd);
		this->sqlgen.AddString("prog_version", revision);

		if (!this->ExecuteStatement(this->sqlgen))
		{
			return false;
		}
	}
	return true;
}

// экспорт кривой
bool VCurveExport::Export(const VCurve & curve, size_t crvNdx)
{
	std::wstringstream msg;
	
	msg << L"Занесение кривой №" << crvNdx;
	LOG_DEBUG(msg.str());
	msg.str(L"");

	// определяемся с высокими скоростями
	this->hs = curve.HasVprkHigh(*this->dataSet);
	this->hs_bt = curve.HasVprkHighBT(*this->dataSet);

	// кривая
	this->sqlgen.Init(this->GetTable(TABLE_CURVE));

	double absAxis = 0.5 * (
		curve.GetRangeCoord(*this->dataSet, true, false, true) + 
		curve.GetRangeCoord(*this->dataSet, true, false, false));

	if (this->crvType == CURVE_SPEED)
	{
		this->sqlgen.AddInteger(this->GetTable(TABLE_TRAIL) + "_id", this->trailId);
	}
	else
	{
		this->sql << "select id from " << this->GetTable(TABLE_PLAN_ELEM) << " where ";
		this->sql << "start_abs < " << absAxis << " and end_abs > " << absAxis;
		this->sql << " and enu_elem_type_id = (" + this->GetPlanElemTypeSQL(PLAN_ELEM_CURVE) + " ";
		this->sql << " and " << this->GetTable(TABLE_TRAIL) << "_id = " << this->trailId;
		this->sqlgen.AddSQL(this->GetTable(TABLE_PLAN_ELEM) + "_id", this->sql.str());
		this->sql.str("");
	}

	if (this->crvType == CURVE_REPAIR)
	{
		this->sql << "select id from repair_sections where repair_estim_id = ";
		this->sql << "(select id from repair_estim where repair_project_id = ";
		this->sql << this->repairId << " ";
		this->sql << "and msr_trail_id = " << this->trailId;
		this->sqlgen.AddSQL("repair_sections_id", sql.str());
		this->sql.str("");
	}

	this->sqlgen.AddString("prog_version", revision);
	this->sqlgen.AddInteger("enu_side_id", curve.GetOrientation(*this->dataSet));
	this->sqlgen.AddInteger("enu_waycat_id", curve.GetWayCat(*this->dataSet));

	this->sqlgen.AddFloat("radius_avg_circ", curve.GetAvgRadius(*this->dataSet));
	this->sqlgen.AddFloat("radius_max_circ", curve.GetMaxRadius(*this->dataSet));
	this->sqlgen.AddFloat("radius_min_circ", curve.GetMinRadius(*this->dataSet));

	if (this->crvType == CURVE_MODEL)
	{
		this->sqlgen.AddInteger("num_pasp", crvNdx+1);
		this->sqlgen.AddFloat("angle_model", curve.GetAngle(*this->dataSet));
		this->sqlgen.AddInteger("enu_model_status_id", curve.GetModelStatus());

		const char * comment = curve.GetModelComment();
		if (comment != NULL && comment[0] != '\0')
		{
			this->sqlgen.AddString("description", comment);
		}

		this->sqlgen.AddFloat("level_pasp_avg_circ", curve.GetAvgLevel(*this->dataSet));
		this->sqlgen.AddFloat("level_pasp_max_circ", curve.GetMaxLevel(*this->dataSet));
		this->sqlgen.AddFloat("level_pasp_min_circ", curve.GetMinLevel(*this->dataSet));
		this->sqlgen.AddFloat("level_fact_avg_circ", curve.GetAvgFactLevel(*this->dataSet));

		this->sqlgen.AddFloat("anp_pass", curve.GetAvgAnpPass(*this->dataSet));
		this->sqlgen.AddFloat("anp_truck", curve.GetAvgAnpTruck(*this->dataSet));
		this->sqlgen.AddFloat("anp_avg", curve.GetAvgAnpThird(*this->dataSet));
		this->sqlgen.AddFloat("psi_pass", curve.GetMaxPsiPass(*this->dataSet, NULL));
		this->sqlgen.AddFloat("psi_truck", curve.GetMaxPsiTruck(*this->dataSet, NULL));

		this->sqlgen.AddFloat("plan_delta_avg", curve.GetAvgPlanOffset(*this->dataSet));
		this->sqlgen.AddFloat("plan_delta_max", curve.GetMaxPlanOffset(*this->dataSet));
		this->sqlgen.AddFloat("level_delta_avg", curve.GetAvgLevelOffset(*this->dataSet));
		this->sqlgen.AddFloat("level_delta_max", curve.GetMaxLevelOffset(*this->dataSet));

		this->sqlgen.AddFloat("olevel_max_1", curve.GetMaxLevelTap(*this->dataSet, 0));
		this->sqlgen.AddFloat("olevel_max_2", curve.GetMaxLevelTap(*this->dataSet, curve.GetRadiusCount()));
		this->sqlgen.AddFloat("olevel_avg_1", curve.GetAvgLevelTap(*this->dataSet, 0));
		this->sqlgen.AddFloat("olevel_avg_2", curve.GetAvgLevelTap(*this->dataSet, curve.GetRadiusCount()));

	}
	else // CURVE_MAGISTRAL || CURVE_STATION || CURVE_REPAIR
	{
		this->sqlgen.AddInteger("number_in_trail", crvNdx+1);
		this->sqlgen.AddFloat("angle", curve.GetAngle(*this->dataSet));

		this->sqlgen.AddFloat("lvl_avg_circ", curve.GetAvgLevel(*this->dataSet));
		this->sqlgen.AddFloat("lvl_max_circ", curve.GetMaxLevel(*this->dataSet));
		this->sqlgen.AddFloat("lvl_min_circ", curve.GetMinLevel(*this->dataSet));

		this->sqlgen.AddFloat("width_avg_circ", curve.GetAvgWidth(*this->dataSet));
		this->sqlgen.AddFloat("width_max_circ", curve.GetMaxWidth(*this->dataSet));
		this->sqlgen.AddFloat("width_min_circ", curve.GetMinWidth(*this->dataSet));
		this->sqlgen.AddFloat("incr_width_plus_max", curve.GetMaxDeltaWidthFront(*this->dataSet));
		this->sqlgen.AddFloat("incr_width_plus_avg", curve.GetAvgDeltaWidthFront(*this->dataSet));
		this->sqlgen.AddFloat("incr_width_minus_max", curve.GetMaxDeltaWidthBack(*this->dataSet));
		this->sqlgen.AddFloat("incr_width_minus_avg", curve.GetAvgDeltaWidthBack(*this->dataSet));

		this->sqlgen.AddFloat("wear_len_over6", curve.GetWearLenOverLimit(*this->dataSet, 6.0f));
		this->sqlgen.AddFloat("wear_len_over10", curve.GetWearLenOverLimit(*this->dataSet, 10.0f));
		this->sqlgen.AddFloat("wear_len_over15", curve.GetWearLenOverLimit(*this->dataSet, 15.0f));
		this->sqlgen.AddFloat("wear_bug_percent", curve.GetWearErrorPercent(*this->dataSet));
		this->sqlgen.AddFloat("width_bug_percent", curve.GetWidthErrorPercent(*this->dataSet));

		this->sqlgen.AddFloat("wear_avg", curve.GetAvgWear(*this->dataSet));

		double maxWear = curve.GetMaxWear(*this->dataSet);
		this->sqlgen.AddFloat("wear_max", maxWear);

		if (this->crvType == CURVE_MAGISTRAL)
		{
			this->sqlgen.AddInteger("degr_wear_max", GetWearDegree(maxWear));
		}
		else if (this->crvType == CURVE_STATION)
		{
			this->sqlgen.AddFloat("tangent_1", curve.GetTan1(*this->dataSet));
			this->sqlgen.AddFloat("tangent_2", curve.GetTan2(*this->dataSet));
		}

		double absCoord = 0.0;
		float value = curve.GetMaxAnpPass(*this->dataSet, &absCoord);
		this->sqlgen.AddFloat("anp_pass_max_abs", absCoord);
		this->sqlgen.AddFloat("anp_pass_max", value);
		this->sqlgen.AddFloat("anp_truck_max", curve.GetMaxAnpTruck(*this->dataSet));

		absCoord = 0.0;
		value = curve.GetMaxPsiPass(*this->dataSet, &absCoord);
		this->sqlgen.AddFloat("psi_pass_max_abs", absCoord);
		this->sqlgen.AddFloat("psi_pass_max", value);
		this->sqlgen.AddFloat("psi_truck_max", curve.GetMaxPsiTruck(*this->dataSet, &absCoord));

		this->sqlgen.AddFloat("anp_pass_avg", curve.GetAvgAnpPass(*this->dataSet));
		this->sqlgen.AddFloat("anp_truck_avg", curve.GetAvgAnpTruck(*this->dataSet));
		this->sqlgen.AddFloat("anp_truck_max_v60", curve.GetMaxAnpThird(*this->dataSet));
		this->sqlgen.AddFloat("anp_truck_avg_v60", curve.GetAvgAnpThird(*this->dataSet));
		this->sqlgen.AddFloat("spd_anp_plus_03", curve.GetVPlus03(*this->dataSet));
		this->sqlgen.AddFloat("spd_anp_minus_03", curve.GetVMinus03(*this->dataSet));

		this->sqlgen.AddFloat("p_crv", curve.GetPRPlan(*this->dataSet));
		this->sqlgen.AddFloat("p_lvl", curve.GetPRLevel(*this->dataSet));
		this->sqlgen.AddFloat("p_coord", curve.GetPRCoord(*this->dataSet));
		this->sqlgen.AddFloat("p_anp", curve.GetPRAnp(*this->dataSet));
		this->sqlgen.AddFloat("p_int", curve.GetPRSum(*this->dataSet));
		this->sqlgen.AddInteger("degr_crv", curve.GetDegreePlan(*this->dataSet));
		this->sqlgen.AddInteger("degr_lvl", curve.GetDegreeLevel(*this->dataSet));
		this->sqlgen.AddInteger("degr_coord", curve.GetDegreeCoord(*this->dataSet));
		this->sqlgen.AddInteger("degr_anp", curve.GetDegreeAnp(*this->dataSet));
	}

	if (this->crvType == CURVE_MAGISTRAL && this->hs)
	{
		this->sqlgen.AddFloat("anp_hs_max", curve.GetMaxAnpHigh(*this->dataSet, NULL));
		this->sqlgen.AddFloat("anp_hs_avg", curve.GetAvgAnpHigh(*this->dataSet));
		this->sqlgen.AddFloat("psi_hs_max", curve.GetMaxPsiHigh(*this->dataSet, NULL));
	}
	else if (this->crvType == CURVE_MODEL && this->hs)
	{
		this->sqlgen.AddFloat("anp_hs", curve.GetAvgAnpHigh(*this->dataSet));
		this->sqlgen.AddFloat("psi_hs", curve.GetMaxPsiHigh(*this->dataSet, NULL));
	}

	if (this->crvType == CURVE_MAGISTRAL && this->hs_bt)
	{
		this->sqlgen.AddFloat("anp_hs_bt_max", curve.GetMaxAnpHighBT(*this->dataSet, NULL));
		this->sqlgen.AddFloat("anp_hs_bt_avg", curve.GetAvgAnpHighBT(*this->dataSet));
		this->sqlgen.AddFloat("psi_hs_bt_max", curve.GetMaxPsiHighBT(*this->dataSet, NULL));
	}
	else if (this->crvType == CURVE_MODEL && this->hs_bt)
	{
		this->sqlgen.AddFloat("anp_hs_bt", curve.GetAvgAnpHighBT(*this->dataSet));
		this->sqlgen.AddFloat("psi_hs_bt", curve.GetMaxPsiHighBT(*this->dataSet, NULL));
	}

	LOG_DEBUG(L"График карточки кривой");

	// график для карточки
	ISC_QUAD graph_id;
	std::stringstream static_graph;
	VCurveCard card(this->dataSet, 
		this->crvType == CURVE_MODEL ? PRM_MODEL_CURVES : PRM_CURVE_MARKS, crvNdx);

	try
	{
		card.Prepare();
		card.SaveStreamPNG(static_graph);
		if (!static_graph.str().empty())
		{
			boost::scoped_ptr<VBlob> blob(new VBlob(static_graph));
			graph_id = this->dataBase->AddBlob(this->transaction, *blob);
			static_graph.str("");				
			this->sqlgen.AddParameter("picture", VIBParameter(SQL_BLOB+1, (char*)&graph_id));
		}
	}
	catch(VException & ex)
	{
		this->message = L"Сбой при создании картинки\n" + ex.GetDescription();
		this->message += L"\n\n";
		this->message += ex.GetDetails();
		return false;
	}
	catch(std::exception & ex)
	{
		this->message = L"Сбой при создании картинки\n" + string_to_wstring(ex.what());
		return false;
	}

	if (!this->ExecuteStatement(this->sqlgen))
	{
		return false;
	}

	LOG_DEBUG(L"Получение ID кривой");

	// id кривой
	if (this->crvType == CURVE_SPEED)
	{
		this->sql << "select id from " << this->GetTable(TABLE_CURVE) << " where ";
		this->sql << this->GetTable(TABLE_TRAIL) << "_id = " << this->trailId << " and ";
		this->sql << "number_in_trail = " << crvNdx+1;
	}
	else if (this->crvType == CURVE_REPAIR)
	{
		this->sql << "select id from " << this->GetTable(TABLE_CURVE) << " where ";
		this->sql << this->GetTable(TABLE_PLAN_ELEM) << "_id = ";
		this->sql << "(select id from " << this->GetTable(TABLE_PLAN_ELEM) << " where ";
		this->sql << "enu_elem_type_id = (" << this->GetPlanElemTypeSQL(PLAN_ELEM_CURVE) << " and ";
		this->sql << "start_abs < " << absAxis << " and end_abs > " << absAxis << " and ";
		this->sql << this->GetTable(TABLE_TRAIL) << "_id = " << this->trailId << " and ";
		this->sql << "repair_sections_id = (select id from repair_sections where ";
		this->sql << "repair_estim_id = (select id from repair_estim where ";
		this->sql << "repair_project_id = " << this->repairId << " and ";
		this->sql << "msr_trail_id = " << this->trailId << ";";
	}
	else
	{
		this->sql << "select id from " << this->GetTable(TABLE_CURVE) << " where ";
		this->sql << this->GetTable(TABLE_PLAN_ELEM) << "_id = ";
		this->sql << "(select id from " << this->GetTable(TABLE_PLAN_ELEM) << " where ";
		this->sql << "enu_elem_type_id = (" << this->GetPlanElemTypeSQL(PLAN_ELEM_CURVE) << " and ";
		this->sql << "start_abs < " << absAxis << " and end_abs > " << absAxis;
		this->sql << " and " << this->GetTable(TABLE_TRAIL) << "_id = " << this->trailId << ";";
	}

	if (!this->ExecuteStatement(this->sql.str()))
	{
		return false;
	}
	this->sql.str("");
	__int64 curveId = this->handler.GetParameter(0, 0).GetInteger();

	LOG_DEBUG(L"Разметка кривой");

	// метки кривой
	for (size_t markNdx = 0; markNdx < curve.GetMarkCount(); ++markNdx)
	{
		this->sqlgen.Init(this->GetTable(TABLE_MARK));
		this->sqlgen.AddInteger(this->GetTable(TABLE_CURVE) + "_id", curveId);
		this->sqlgen.AddFloat("crv_abs", this->dataSet->GetAbsCoord().GetItem(curve.GetPlanMark(markNdx).index));
		this->sqlgen.AddFloat("crv_value", curve.GetPlanMark(markNdx).value);
		this->sqlgen.AddFloat("lvl_abs", this->dataSet->GetAbsCoord().GetItem(curve.GetLevelMark(markNdx).index));
		this->sqlgen.AddFloat("lvl_value", curve.GetLevelMark(markNdx).value);
		
		if (this->crvType == CURVE_MODEL)
		{
			this->sqlgen.AddInteger("num_model_mark", markNdx);
		}
		else // CURVE_MAGISTRAL || CURVE_STATION
		{
			this->sqlgen.AddInteger("num_mark", markNdx);
		}
		this->sqlgen.AddFloat("latitude", curve.GetPlanMark(markNdx).latitude);
		this->sqlgen.AddFloat("longitude", curve.GetPlanMark(markNdx).longitude);
		this->sqlgen.AddString("prog_version", revision);

		if (!this->ExecuteStatement(this->sqlgen))
		{
			return false;
		}
	}

	LOG_DEBUG(L"Участки скоростей");

	// участки скоростей
	if (!this->ExportSpdAreas(curve, TABLE_SPDAREA, TABLE_CURVE, curveId, COMPLETE_CURVE))
	{
		return false;
	}

	if (this->crvType == CURVE_MAGISTRAL || this->crvType == CURVE_MODEL)
	{
		LOG_DEBUG(L"Участки высоких скоростей");

		// высокие скорости
		if (!this->ExportHiSpdAreas(curve, TABLE_SPDAREA_HS, TABLE_CURVE, false, curveId, COMPLETE_CURVE))
		{
			return false;
		}

		// высокие скорости [наклон кузова]
		if (!this->ExportHiSpdAreas(curve, TABLE_SPDAREA_HS_BT, TABLE_CURVE, true, curveId, COMPLETE_CURVE))
		{
			return false;
		}
	}

	// элементарные кривые
	size_t radiusCount = curve.GetRadiusCount();
	for (size_t radiusNdx = 0; radiusNdx < radiusCount; ++radiusNdx)
	{
		msg << L"Элементарная кривая №" << radiusNdx+1;
		LOG_DEBUG(msg.str());
		msg.str(L"");

		this->sqlgen.Init(this->GetTable(TABLE_RADIUS));
		this->sqlgen.AddInteger(this->GetTable(TABLE_CURVE) + "_id", curveId);
		this->sqlgen.AddString("prog_version", revision);

		this->sqlgen.AddFloat("radius_max_circ", curve.GetMaxRadius(*this->dataSet, radiusNdx));
		this->sqlgen.AddFloat("radius_min_circ", curve.GetMinRadius(*this->dataSet, radiusNdx));
		this->sqlgen.AddFloat("radius_avg_circ", curve.GetAvgRadius(*this->dataSet, radiusNdx));

		if (this->crvType == CURVE_MODEL)
		{
			this->sqlgen.AddInteger("radius_model_number", radiusNdx);
			this->sqlgen.AddFloat("angle_model", curve.GetAngle(*this->dataSet, radiusNdx));

			this->sqlgen.AddFloat("level_fact_avg_circ", curve.GetAvgFactLevel(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("level_pasp_avg_circ", curve.GetAvgLevel(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("level_pasp_max_circ", curve.GetMaxLevel(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("level_pasp_min_circ", curve.GetMinLevel(*this->dataSet, radiusNdx));

			this->sqlgen.AddFloat("olevel_max_1", curve.GetMaxLevelTap(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("olevel_avg_1", curve.GetAvgLevelTap(*this->dataSet, radiusNdx));
			if (radiusNdx == radiusCount-1)
			{
				this->sqlgen.AddFloat("olevel_max_2", curve.GetMaxLevelTap(*this->dataSet, radiusCount));
				this->sqlgen.AddFloat("olevel_avg_2", curve.GetAvgLevelTap(*this->dataSet, radiusCount));
			}
			else
			{
				this->sqlgen.AddFloat("olevel_max_2", 0);
				this->sqlgen.AddFloat("olevel_avg_2", 0);
			}

			this->sqlgen.AddFloat("anp_pass", curve.GetAvgAnpPass(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("anp_truck", curve.GetAvgAnpTruck(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("anp_avg", curve.GetAvgAnpThird(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("psi_pass", curve.GetMaxPsiPass(*this->dataSet, NULL, radiusNdx));
			this->sqlgen.AddFloat("psi_truck", curve.GetMaxPsiTruck(*this->dataSet, NULL, radiusNdx));

			this->sqlgen.AddFloat("plan_delta_avg", curve.GetAvgPlanOffset(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("plan_delta_max", curve.GetMaxPlanOffset(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("level_delta_avg", curve.GetAvgLevelOffset(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("level_delta_max", curve.GetMaxLevelOffset(*this->dataSet, radiusNdx));
		}

		else // CURVE_MAGISTRAL || CURVE_STATION || CURVE_REPAIR || CURVE_SPEED
		{
			this->sqlgen.AddInteger("radius_number", radiusNdx);
			this->sqlgen.AddFloat("angle", curve.GetAngle(*this->dataSet, radiusNdx));

			this->sqlgen.AddFloat("lvl_avg_circ", curve.GetAvgLevel(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("lvl_max_circ", curve.GetMaxLevel(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("lvl_min_circ", curve.GetMinLevel(*this->dataSet, radiusNdx));

			this->sqlgen.AddFloat("o_crv_max_1", curve.GetMaxPlanTap(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("o_crv_avg_1", curve.GetAvgPlanTap(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("o_lvl_max_1", curve.GetMaxLevelTap(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("o_lvl_avg_1", curve.GetAvgLevelTap(*this->dataSet, radiusNdx));
			if (radiusNdx == radiusCount-1)
			{
				this->sqlgen.AddFloat("o_crv_max_2", curve.GetMaxPlanTap(*this->dataSet, radiusCount));
				this->sqlgen.AddFloat("o_crv_avg_2", curve.GetAvgPlanTap(*this->dataSet, radiusCount));
				this->sqlgen.AddFloat("o_lvl_max_2", curve.GetMaxLevelTap(*this->dataSet, radiusCount));
				this->sqlgen.AddFloat("o_lvl_avg_2", curve.GetAvgLevelTap(*this->dataSet, radiusCount));
			}
			else
			{
				this->sqlgen.AddFloat("o_crv_max_2", 0);
				this->sqlgen.AddFloat("o_crv_avg_2", 0);
				this->sqlgen.AddFloat("o_lvl_max_2", 0);
				this->sqlgen.AddFloat("o_lvl_avg_2", 0);
			}

			this->sqlgen.AddFloat("wear_avg", curve.GetAvgWear(*this->dataSet, radiusNdx));
			double maxWear = curve.GetMaxWear(*this->dataSet, radiusNdx);
			this->sqlgen.AddFloat("wear_max", maxWear);
			if (this->crvType == CURVE_MAGISTRAL)
			{
				this->sqlgen.AddInteger("degr_wear_max", GetWearDegree(maxWear));
			}
			else if (this->crvType == CURVE_STATION)
			{
				sqlgen.AddFloat("tangent", 0);
			}

			this->sqlgen.AddFloat("width_avg_circ", curve.GetAvgWidth(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("width_max_circ", curve.GetMaxWidth(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("width_min_circ", curve.GetMinWidth(*this->dataSet, radiusNdx));

			this->sqlgen.AddFloat("corr_wear", curve.GetCorrWear(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("wear_bug_percent", curve.GetWearErrorPercent(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("width_bug_percent", curve.GetWidthErrorPercent(*this->dataSet, radiusNdx));

			double absCoord = 0;
			this->sqlgen.AddFloat("anp_pass_max", curve.GetMaxAnpPass(*this->dataSet, &absCoord, radiusNdx));
			this->sqlgen.AddFloat("anp_pass_max_abs", absCoord);
			this->sqlgen.AddFloat("anp_truck_max", curve.GetMaxAnpTruck(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("anp_pass_avg", curve.GetAvgAnpPass(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("anp_truck_avg", curve.GetAvgAnpTruck(*this->dataSet, radiusNdx));
			absCoord = 0;
			this->sqlgen.AddFloat("psi_pass_max", curve.GetMaxPsiPass(*this->dataSet,  &absCoord, radiusNdx));
			this->sqlgen.AddFloat("psi_pass_max_abs", absCoord);
			this->sqlgen.AddFloat("psi_truck_max", curve.GetMaxPsiTruck(*this->dataSet, NULL, radiusNdx));

			if (this->crvType == CURVE_REPAIR || this->crvType == CURVE_MAGISTRAL || this->crvType == CURVE_SPEED)
			{
				short iMarkCrv = 0;
				short isLimAnp = 0;
				short isLimPsi = 0;
				short isLimTap1 = 0;
				short isLimTap2 = 0;
				short isLimTap = 0;

				if (curve.GetMaxAnpPass(*this->dataSet, NULL, radiusNdx) > 
					curve.GetAnpPassRangeMax() + 0.005)
				{
					iMarkCrv = 2;
					isLimAnp = 1;
				}
	
				if (curve.GetAvgAnpPass(*this->dataSet, radiusNdx) > 
					curve.GetAnpPassRangeMid() + 0.005)
				{
					iMarkCrv = 2; 
					isLimAnp = 1;
				}

				if (curve.GetMaxPsiPass(*this->dataSet, NULL, radiusNdx) > 
					curve.GetPsiPassRangeMax() + 0.005)
				{
					iMarkCrv = 2; 
					isLimPsi = 1;
				}
				
				isLimTap1 = curve.IsLimitTap(*this->dataSet, this->crvType == CURVE_REPAIR, radiusNdx) ? 1 : 0;
				isLimTap2 = curve.IsLimitTap(*this->dataSet, this->crvType == CURVE_REPAIR, radiusNdx+1) ? 1 : 0;
				isLimTap = (isLimTap1 > 0 || isLimTap2 > 0) ? 1 : 0;
				if (isLimTap > 0) iMarkCrv = 2;

                if (this->crvType == CURVE_REPAIR)
    				this->sqlgen.AddInteger("enu_grade_id", iMarkCrv);
    				
				this->sqlgen.AddInteger("is_limit_anp", isLimAnp);
				this->sqlgen.AddInteger("is_limit_psi", isLimPsi);
				
				if (this->crvType == CURVE_REPAIR || this->crvType == CURVE_MAGISTRAL)
				{
    				this->sqlgen.AddInteger("is_limit_tap_1", isLimTap1);
	    			this->sqlgen.AddInteger("is_limit_tap_2", isLimTap2);
				}
				
				this->sqlgen.AddInteger("is_limit_tap", isLimTap);
			}
		}

		if (this->crvType == CURVE_MAGISTRAL && this->hs)
		{
			this->sqlgen.AddFloat("anp_hs_max", curve.GetMaxAnpHigh(*this->dataSet, NULL, radiusNdx));
			this->sqlgen.AddFloat("anp_hs_avg", curve.GetAvgAnpHigh(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("psi_hs_max", curve.GetMaxPsiHigh(*this->dataSet, NULL, radiusNdx));
		}
		else if (this->crvType == CURVE_MODEL && this->hs)
		{
			this->sqlgen.AddFloat("anp_hs", curve.GetAvgAnpHigh(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("psi_hs", curve.GetMaxPsiHigh(*this->dataSet, NULL, radiusNdx));
		}

		if (this->crvType == CURVE_MAGISTRAL && this->hs_bt)
		{
			this->sqlgen.AddFloat("anp_hs_bt_max", curve.GetMaxAnpHighBT(*this->dataSet, NULL, radiusNdx));
			this->sqlgen.AddFloat("anp_hs_bt_avg", curve.GetAvgAnpHighBT(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("psi_hs_bt_max", curve.GetMaxPsiHighBT(*this->dataSet, NULL, radiusNdx));
		}
		else if (this->crvType == CURVE_MODEL && this->hs_bt)
		{
			this->sqlgen.AddFloat("anp_hs_bt", curve.GetAvgAnpHighBT(*this->dataSet, radiusNdx));
			this->sqlgen.AddFloat("psi_hs_bt", curve.GetMaxPsiHighBT(*this->dataSet, NULL, radiusNdx));
		}

		if (!this->ExecuteStatement(this->sqlgen))
		{
			return false;
		}

		LOG_DEBUG(L"Получение ID элементарной кривой");

		// вынимаем id элементарной кривой
		this->sql << "select id from " << this->GetTable(TABLE_RADIUS) << " where ";
		this->sql << "" << this->GetTable(TABLE_CURVE) << "_id = " << curveId << " and ";
		this->sql << (this->crvType == CURVE_MODEL ? "radius_model_number" : "radius_number");
		this->sql << " = " << radiusNdx << ";";

		if (!this->ExecuteStatement(sql.str()))
		{
			return false;
		}
		int64_t radiusId = handler.GetParameter(0, 0).GetInteger();
		this->sql.str("");

		LOG_DEBUG(L"Участки скоростей на элементарной кривой");

		// участки скоростей
		if (!this->ExportSpdAreas(curve, TABLE_RADIUS_SPDAREA, TABLE_RADIUS, radiusId, radiusNdx))
		{
			return false;
		}

		if (this->crvType == CURVE_MAGISTRAL || this->crvType == CURVE_MODEL)
		{
			LOG_DEBUG(L"Участки высоких скоростей на элементарной кривой");

			// высокие скорости
			if (!this->ExportHiSpdAreas(curve, TABLE_RADIUS_SPDAREA_HS, TABLE_RADIUS, false, radiusId, radiusNdx))
			{
				return false;
			}

			// высокие скорости [наклон кузова]
			if (!this->ExportHiSpdAreas(curve, TABLE_RADIUS_SPDAREA_HS_BT, TABLE_RADIUS, true, radiusId, radiusNdx))
			{
				return false;
			}
		}
	}

	if (this->crvType == CURVE_MAGISTRAL || this->crvType == CURVE_STATION)
	{
		LOG_DEBUG(L"Пикеты с ограничениями скорости");

		// пикеты с ограничениями скоростей
		std::vector<VCurvePicket> crvPks = curve.GetPkList(*this->dataSet, false);
		for (size_t i = 0; i < crvPks.size(); ++i)
		{
			VCurvePicket & crvPk = crvPks[i];
			if (!crvPk.HasLimit()) continue;

			this->sqlgen.Init(this->GetTable(TABLE_PICKET_SPDLIM));
			this->sqlgen.AddInteger(this->GetTable(TABLE_CURVE) + "_id", curveId);
			this->sqlgen.AddInteger("km_piket", crvPk.km);
			this->sqlgen.AddInteger("num_piket", crvPk.pk);
			this->sqlgen.AddInteger("is_limit_vkr_pass", crvPk.isVkrPass);
			this->sqlgen.AddInteger("is_limit_vkr_truck", crvPk.isVkrTruck);
			this->sqlgen.AddInteger("is_limit_vpr_pass", crvPk.isVprPass);
			this->sqlgen.AddInteger("is_limit_vpr_truck", crvPk.isVprTruck);
			this->sqlgen.AddInteger("is_limit_viz_pass", crvPk.isVizPass);
			this->sqlgen.AddInteger("is_limit_viz_truck", crvPk.isVizTruck);
			this->sqlgen.AddString("prog_version", revision);

			if (!this->ExecuteStatement(this->sqlgen))
			{
				return false;
			}
		}

		if (this->crvType == CURVE_MAGISTRAL && 
			this->optics && 
			this->dataSet->IsLoaded(PRM_UNI_WEAR))
		{
			LOG_DEBUG(L"Пикеты с износом");

			// пикеты с износом
			std::vector<VCurveWearPicket> crvWPks = curve.GetWearPkList(*this->dataSet);
			for (size_t i = 0; i < crvWPks.size(); ++i)
			{
				VCurveWearPicket & pk = crvWPks[i];

				this->sqlgen.Init(this->GetTable(TABLE_PICKET_WEAR));
				this->sqlgen.AddInteger(this->GetTable(TABLE_CURVE) + "_id", curveId);
				this->sqlgen.AddInteger("km_piket", pk.km);
				this->sqlgen.AddInteger("num_piket", pk.pk);
				this->sqlgen.AddFloat("wear_max_piket", pk.maxWear);
				this->sqlgen.AddFloat("wear_avg_piket", pk.avgWear);
				this->sqlgen.AddFloat("width_max_piket", pk.maxWidth);
				this->sqlgen.AddFloat("width_avg_piket", pk.avgWidth);
				this->sqlgen.AddFloat("wear_bug_percent", pk.errorWear);
				this->sqlgen.AddFloat("width_bug_percent", pk.errorWidth);
				this->sqlgen.AddInteger("spd_wear_pass", pk.spdPass);
				this->sqlgen.AddInteger("spd_wear_truck", pk.spdTruck);
				sqlgen.AddInteger("degr_wear_max", GetWearDegree(pk.maxWear));
				if (pk.ntd)	sqlgen.AddString("comment", "нтд");
				sqlgen.AddString("prog_version", revision);

				if (!this->ExecuteStatement(this->sqlgen))
				{
					return false;
				}
			}

			LOG_DEBUG(L"Плети");

			// плети
			std::vector<VCurveLink> crvLinks = curve.GetLinkList(*this->dataSet);
			for (size_t i = 0; i < crvLinks.size(); ++i)
			{
				VCurveLink & link = crvLinks[i];

				this->sqlgen.Init(this->GetTable(TABLE_LONG_RAILS));
				this->sqlgen.AddInteger(this->GetTable(TABLE_CURVE) + "_id", curveId);
				this->sqlgen.AddFloat("start_abs", link.absStart);
				this->sqlgen.AddFloat("end_abs", link.absEnd);
				this->sqlgen.AddFloat("wear_avg", link.avgWear);
				this->sqlgen.AddFloat("wear_max", link.maxWear);
				this->sqlgen.AddFloat("wear_bug_percent", link.errorWear);
				this->sqlgen.AddFloat("width_avg", link.avgWidth);
				this->sqlgen.AddFloat("width_max", link.maxWidth);
				this->sqlgen.AddFloat("width_bug_percent", link.errorWidth);
				this->sqlgen.AddInteger("spd_wear_pass", link.spdPass);
				this->sqlgen.AddInteger("spd_wear_truck", link.spdTruck);
				//if (link.ntd)	sqlgen.AddString("comment", "нтд";
				if (!this->ExecuteStatement(this->sqlgen))
				{
					return false;
				}
			}

			LOG_DEBUG(L"Диаграмма износов");

			// диаграмма износов
			std::set<density_t> wearDiagram = curve.GetWearDiagram(*this->dataSet);
			std::set<density_t>::const_iterator it = wearDiagram.begin();
			for (; it != wearDiagram.end(); it++)
			{
				const density_t & dgrItem = *it;

				sqlgen.Init(this->GetTable(TABLE_WEAR_DENSITY));
				sqlgen.AddInteger(this->GetTable(TABLE_CURVE) + "_id", curveId);
				sqlgen.AddFloat("range_start", dgrItem.range.left_bound().value);
				sqlgen.AddFloat("range_end", dgrItem.range.right_bound().value);
				sqlgen.AddFloat("density", dgrItem.value);
				//sqlgen.AddInteger("ppr", OPTIONS.GetOption(L"WEAR_SOURCE").Compare(OPTION_PPR));
				sqlgen.AddString("prog_version", revision);

				if (!this->ExecuteStatement(this->sqlgen))
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool VCurveExport::CheckCurves()
{
    return this->dataSet->DataExists(this->crvType == CURVE_MODEL ? PRM_MODEL_CURVES : PRM_CURVE_MARKS);
}
