#include "VRepairEstimate.h"
#include "revision.h"
#include "VCurveExport.h"
#include "VCurveCard.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <wx/wx.h>

// конструктор
VRepairEstimate::VRepairEstimate(VIBDatabasePtr db, __int64 rep_id) : 
repair_id(rep_id), trail_id_before(0), trail_id_after(0), trail_id_after_year(0), 
waycat_after_repair(0), database(db), jaggy_plan_on_lines(true)
{
}

// выполнить запрос
bool VRepairEstimate::ExecuteStatement(const std::string & sql)
{
	try
	{
		this->database->ExecuteStatement(this->trans, sql.c_str(), this->handler);
	}
	catch(VException & e)
	{
		this->message = e.GetDescription() + L": ";
		this->message += string_to_wstring(this->database->GetErrorDescription());
		this->message += L"\n\n" + string_to_wstring(sql);
		this->database->RollbackTransaction(this->trans);
		return false;
	}
	return true;
}

// выполнить запрос
bool VRepairEstimate::ExecuteStatement(VSQLGenerator & sqlgen)
{
	std::string sql = sqlgen.GetInsertSQL();
	try
	{
		this->database->ExecuteStatementImmediate(this->trans, sql.c_str(), sqlgen.GetParameters());
	}
	catch(VException & e)
	{
		this->message = e.GetDescription() + L": ";
		this->message += string_to_wstring(this->database->GetErrorDescription());
		this->message += L"\n\n" + string_to_wstring(sql);
		this->database->RollbackTransaction(this->trans);
		return false;
	}
	return true;
}

// априорная информация
bool VRepairEstimate::Makeinfo(VDataSetPtr ds, const wchar_t * msg)
{
	if (!ds->Makeinfo(this->database, false, false))
	{
		this->database->RollbackTransaction(this->trans);
		this->message = msg + ds->GetErrorMessage();
		return false;
	}

	// список проектных скоростей
	std::vector<VAprinfoSpeedRecord> spdlist;
	for (size_t i = 0; i < this->prj_speeds.size(); ++i)
	{
		VAprinfoSpeedRecord spd;
		spd.base = ds->GetAprinfo().GetAbsCoord(this->prj_speeds[i].section.start);
		spd.end = ds->GetAprinfo().GetAbsCoord(this->prj_speeds[i].section.end);
		spd.spd = VSpeeds(this->prj_speeds[i].spd1, this->prj_speeds[i].spd2, this->prj_speeds[i].spd2);
		spdlist.push_back(spd);
	}

	// причёсывание границ
	spdlist.front().base = ds->GetAprinfo().GetAbsCoordBase();
	spdlist.back().end = ds->GetAprinfo().GetAbsCoordEnd();
	for (size_t i = 1; i < spdlist.size(); ++i)
	{
		double axis = 0.5 * (spdlist[i-1].end + spdlist[i].base);
		spdlist[i-1].end = axis;
		spdlist[i].base = axis;
	}

	// подмена скоростей на проектные
	if (!ds->GetAprinfo().ReplaceSpeedList(spdlist))
	{
		this->database->RollbackTransaction(this->trans);
		this->message = L"Список проектных скоростей на ремонте не соответствует существующей априорной информации";
		return false;
	}

	// подмена списка категорий пути на проектную
	std::vector<VAprinfoIndexRecord> wclist;
	VAprinfoIndexRecord wcrec;
	wcrec.value = this->waycat_after_repair;
	wcrec.base = ds->GetAprinfo().GetAbsCoordBase();
	wcrec.end = ds->GetAprinfo().GetAbsCoordEnd();
	wclist.push_back(wcrec);

	if (!ds->GetAprinfo().ReplaceWayCatList(wclist))
	{
		this->database->RollbackTransaction(this->trans);
		this->message = L"Список категорий пути на ремонте не соответствует существующей априорной информации";
		return false;
	}

	return true;
}

// инициализация оценки участка ремонта
VRepairEstimate::RepairArea::RepairArea() : absStart(0), absEnd(0), 
maxGrad(0), maxAngle(0), minLen(10000), maxJaggy(0), sssp(0)
{
}

static double GetCrossing(double repairStart, double repairEnd, const VCurve & curve)
{
}

double VRepairEstimate::RepairArea::Crossing(const VCurve & curve, VDataSetPtr ds) const
{
	double curveStart = std::min<double>(
		curve.GetRangeCoord(*ds, true, false, true),
		curve.GetRangeCoord(*ds, false, false, true));

	double curveEnd = std::max<double>(
		curve.GetRangeCoord(*ds, true, false, false),
		curve.GetRangeCoord(*ds, false, false, false));
		
	if (curveEnd <= this->absStart || curveStart >= this->absEnd) 
	{
		return 0;
	}
	else if (curveStart < this->absStart && curveEnd > this->absStart)
	{
        return (curveEnd - this->absStart) / (curveEnd - curveStart);
	}
	else if (curveStart < this->absEnd && curveEnd > this->absEnd)
	{
		return (this->absEnd - curveStart) / (curveEnd - curveStart);
	}
	else 
	{
		return 1.0;
	}
}

// оценить участок ремонта
void VRepairEstimate::RepairArea::Estimate(const VWaySection & rs, VDataSetPtr ds, 
										   const std::wstring & repair_type,
										   const std::vector<OvrCount> & ovrs,
										   bool jaggy_plan_on_lines)
{
	this->absStart = ds->GetAprinfo().GetAbsCoord(rs.start);
	this->absEnd = ds->GetAprinfo().GetAbsCoord(rs.end);

	// среднее сссп
	size_t ssspCount = 0;	
	for (size_t i = 0; i < ds->GetStatistics().GetKmList().GetCount(); ++i)
	{
		if (ds->GetStatistics().GetKmList().GetItem(i).section.start > rs.end ||
			ds->GetStatistics().GetKmList().GetItem(i).section.end < rs.start)
		{
			continue;
		}

		this->sssp += ds->GetStatistics().GetKmList().GetItem(i).sssp;
		++ssspCount;
	}
	if (ssspCount > 0) this->sssp /= ssspCount;

	// элементы профиля - макс.уклон, мин.угол, мин.длина
	const VSlopeList & slope = ds->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT);
	for (size_t i = 0; i < slope.GetCount(); ++i)
	{
		if (slope.GetItem(i).GetStart().x < this->absStart) continue;
		if (slope.GetItem(i).GetEnd().x > this->absEnd) continue;

		this->maxGrad = std::max<double>(this->maxGrad, fabs(slope.GetItem(i).Slope()));

		this->maxAngle = std::max<double>(this->maxAngle, 
			(i == 0 ? 0 : slope.GetItem(i).Slope() - slope.GetItem(i-1).Slope()));

		this->minLen = std::min<double>(this->minLen, slope.GetItem(i).GetLength());
	}

	size_t idxStart = ds->GetAbsCoord().FindIndex(this->absStart, true);
	size_t idxEnd = ds->GetAbsCoord().FindIndex(this->absEnd, true);

	// max.неровность профиля
	for (size_t kadrNdx = idxStart; kadrNdx < idxEnd; ++kadrNdx)
	{
		this->maxJaggy = std::max<float>(this->maxJaggy, fabs(ds->GetParameter(PRM_JAGGY_PROFILE).GetItem(kadrNdx)));
	}

	// участки оценки фпо по кривым
	std::vector<VCurveFPOArea> fpoAreas;
	for (size_t crvNdx = 0; crvNdx < ds->GetCurves().GetCount(); ++crvNdx)
	{
		VCurve & curve = ds->GetCurves().GetItem(crvNdx);
        if (this->Crossing(curve, ds) < 0.5) continue;

		std::vector<VCurveFPOArea> loc_fpoareas_list = curve.GetFPOAreaList(*ds);
		for (size_t i = 0; i < loc_fpoareas_list.size(); ++i)
		{
			fpoAreas.push_back(loc_fpoareas_list[i]);
		}
	}

	// км оценка
	for (int km = rs.start.km; km <= rs.end.km; ++km)
	{
/*	
		// пропускаем первый километр, если от него нам осталось 100м или менее
		if (km == rs.start.km && rs.start.m >= ds->GetAprinfo().GetKmLength(km) - 100) 
		{
			continue;
		}

		// пропускаем последний километр, если от него нам осталось 100м или менее
		if (km == rs.end.km && rs.end.m <= 100) 
		{
			continue;
		}
*/
		
		double kmBegin = ds->GetAprinfo().GetAbsCoord(VWayCoord(km, 0));
		size_t idxBegin = ds->GetAbsCoord().FindIndex(kmBegin, true);

		double kmEnd = ds->GetAprinfo().GetAbsCoord(VWayCoord(km+1, 0));
		size_t idxEnd = ds->GetAbsCoord().FindIndex(kmEnd, true);

		/// FIX! пропускаем несуществующие километры
		if (idxBegin >= idxEnd) continue;

		KmEstimate cur_km;
		cur_km.km = km;

		// неровности в плане и профиле, шк
		for (size_t i = idxBegin; i < idxEnd; ++i)
		{
			cur_km.maxDiffProfile = std::max<float>(cur_km.maxDiffProfile, fabs(ds->GetParameter(PRM_JAGGY_PROFILE).GetItem(i)));
			cur_km.maxDiffPlan = std::max<float>(cur_km.maxDiffPlan, fabs(ds->GetParameter(jaggy_plan_on_lines ? PRM_JAGGY_PLAN_ON_LINES : PRM_JAGGY_PLAN).GetItem(i)));

			if (!ds->GetParameter(PRM_WIDTH).IsCorrupt(i))
			{
				double delta = 
					ds->GetParameter(PRM_WIDTH).GetItem(i) - 
					ds->GetAprinfo().GetPassportCurveNormaWidth(ds->GetAbsCoord().GetItem(i));

				if (delta > 0)
				{
					if (delta > cur_km.maxWidthPlus) cur_km.maxWidthPlus = delta;
				}
				else
				{
					if (delta < cur_km.maxWidthMinus) cur_km.maxWidthMinus = delta;
				}
			}
		}

		// сссп, отклонение на уровне
		const VStatData & cur_stat = ds->GetStatistics().GetKmList().Find(km, 0);
		if (cur_stat.km == km)
		{
			cur_km.sssp = cur_stat.sssp;
			if (fabs(cur_stat.maxLevel) > fabs(cur_stat.minLevel))
			{
				cur_km.maxLevel = cur_stat.maxLevel;
			}
			else
			{
				cur_km.maxLevel = cur_stat.minLevel;
			}
		}

		// доп. скорость и оценка километра
		speed_t prkSpd = ds->GetAprinfo().GetMaxPassSpeed(kmBegin, kmEnd);
		cur_km.dopSpd = prkSpd;
		
		for (size_t i = 0; i < fpoAreas.size(); ++i)
		{
			VCurveFPOArea & fpo = fpoAreas[i];
			if (fpo.section.start.km != km) continue;

			speed_t crv_dop = std::min<speed_t>(fpo.spdVpr, std::min<speed_t>(fpo.spdVkr, fpo.spdViz));
			if (crv_dop < fpo.spdVprkPass && crv_dop < cur_km.dopSpd)
			{
				cur_km.dopSpd = crv_dop;
			}

			if (fpo.maxAnp > fpo.maxAnpRange || fpo.maxPsi > fpo.maxPsiRange ||
				IsTapFailure(fpo.spdVprkPass, fpo.maxOh, true))
			{
				cur_km.grade = 2;
			}
		}

		if (cur_km.dopSpd < prkSpd) 
		{
			cur_km.grade = 2;
		}
		else
		{
			int sssp_zapas = cur_km.sssp - prkSpd;

			int ovr1count = 0, ovr2count = 0, ovr3count = 0;
			for (size_t i = 0; i < ovrs.size(); ++i)
			{
				if (ovrs[i].km == km)
				{
					ovr1count = ovrs[i].ovr1count;
					ovr2count = ovrs[i].ovr2count;
					ovr3count = ovrs[i].ovr3count;
					break;
				}
			}

			if (cur_km.grade != 2 && sssp_zapas >= 50 && ovr1count == 0 && ovr2count == 0 && ovr3count == 0)
			{
				cur_km.grade = 5;
			}

			else if (cur_km.grade != 2 && 
				(repair_type == L"С" || repair_type == L"РП" || repair_type == L"П") &&
				ovr2count == 0 && ovr3count == 0 && sssp_zapas >= 40)
			{
				cur_km.grade = 4;
			}
			else if (cur_km.grade != 2 && sssp_zapas >= 40 && ovr1count <= 5 && ovr2count == 0 && ovr3count == 0)
			{
				cur_km.grade = 4;
			}

			else if (cur_km.grade != 2 && 
				(repair_type == L"С" || repair_type == L"РП" || repair_type == L"П") &&
				ovr2count <= 5 && ovr3count == 0)
			{
				cur_km.grade = 3;
			}
			else if (cur_km.grade != 2 && ovr1count <= 10 && ovr2count <= 5 && ovr3count == 0)
			{
				cur_km.grade = 3;
			}

			else
			{
				cur_km.grade = 2;
			}
		}

		this->kms.push_back(cur_km);
	}
}

// экспорт данных об участке ремонта
void VRepairEstimate::RepairArea::Export(VSQLGenerator & sqlgen)
{
	sqlgen.AddFloat("start_abs", this->absStart);
	sqlgen.AddFloat("end_abs", this->absEnd);
	sqlgen.AddFloat("max_uklon", this->maxGrad);
	sqlgen.AddFloat("max_razn_ukl", this->maxAngle);
	sqlgen.AddFloat("min_len_elprof", this->minLen);
	sqlgen.AddFloat("mid_ukl_nerprof", this->maxJaggy);
	sqlgen.AddInteger("sssp", this->sssp);
	sqlgen.AddString("prog_version", wstring_to_string(revision));
}

// экспорт данных об километре
void VRepairEstimate::KmEstimate::Export(VSQLGenerator & sqlgen)
{
	sqlgen.AddInteger("num_km", this->km);
	sqlgen.AddFloat("ner_prof", this->maxDiffProfile);
	sqlgen.AddFloat("ner_plan", this->maxDiffPlan);
	sqlgen.AddFloat("otkl_level", this->maxLevel);
	sqlgen.AddFloat("otkl_shk_plus", this->maxWidthPlus);
	sqlgen.AddFloat("otkl_shk_minus", this->maxWidthMinus);
	sqlgen.AddFloat("sssp", this->sssp);
	sqlgen.AddInteger("spd_vdp_pass",this->dopSpd);
	sqlgen.AddInteger("enu_grade_id",this->grade);
	sqlgen.AddString("prog_version", wstring_to_string(revision));
}

// Данные о ремонте из базы
bool VRepairEstimate::Load()
{
	std::stringstream sql;

	this->prog = boost::shared_ptr<VProgressBox>(new VProgressBox(1, L"Оценка ремонта", false));
	this->trans = this->database->StartTransaction();

	// --------------------------------------------------------------
	// информация о ремонте
	// --------------------------------------------------------------

	this->prog->SetMessage(0, L"Загрузка данных :: информация о ремонте");

	// границы участка ремонта, категория после ремонта

	sql << "select rp.start_km, rp.start_m, rp.end_km, rp.end_m, rp.category_ar, ert.s_name ";
	sql << "from repair_project rp inner join enu_repair_type ert on rp.enu_repair_type_id=ert.id ";
	sql << "where rp.id = " << this->repair_id << ";";
	if (!ExecuteStatement(sql.str()))
	{
		return false;
	}
	if (this->handler.GetRowCount() != 1)
	{
		this->database->RollbackTransaction(this->trans);
		this->message = L"Некорректная информация о ремонте";
		return false;
	}
	sql.str("");

	this->repair_section = VWaySection(
		this->handler.GetParameter(0, 0).GetInteger(), this->handler.GetParameter(0, 1).GetFloat(),
		this->handler.GetParameter(0, 2).GetInteger(), this->handler.GetParameter(0, 3).GetFloat());
	this->waycat_after_repair = this->handler.GetParameter(0, 4).GetInteger();
	this->repair_type = this->handler.GetParameter(0, 5).GetString();

	// проектные скорости

	sql << "select start_km, start_m, end_km, end_m, speed1, speed2 ";
	sql << "from repair_speeds where repair_project_id = " << repair_id << " ";
	sql << "order by start_km, start_m;";
	if (!ExecuteStatement(sql.str()))
	{
		return false;
	}
	if (this->handler.GetRowCount() == 0)
	{
		this->database->RollbackTransaction(this->trans);
		this->message = L"Список проектных скоростей на ремонте пуст";
		return false;
	}
	sql.str("");

	for (size_t i = 0; i < this->handler.GetRowCount(); ++i)
	{
		PrjSpeedArea area;
		area.section = VWaySection(
			this->handler.GetParameter(i, 0).GetInteger(), this->handler.GetParameter(i, 1).GetFloat(),
			this->handler.GetParameter(i, 2).GetInteger(), this->handler.GetParameter(i, 3).GetFloat());
		area.spd1 = this->handler.GetParameter(i, 4).GetInteger();
		area.spd2 = this->handler.GetParameter(i, 5).GetInteger();
		this->prj_speeds.push_back(area);
	}

	// отступления

	sql << "select km, ove1, ove2, ove3 from repair_oversteps ";
	sql << "where repair_project_id=" << repair_id << " order by km;";
	if (!ExecuteStatement(sql.str()))
	{
		return false;
	}
	sql.str("");

	for (size_t i = 0; i < this->handler.GetRowCount(); ++i)
	{
		OvrCount o;
		o.km = this->handler.GetParameter(i, 0).GetInteger();
		o.ovr1count = this->handler.GetParameter(i, 1).GetInteger();;
		o.ovr2count = this->handler.GetParameter(i, 2).GetInteger();
		o.ovr3count = this->handler.GetParameter(i, 3).GetInteger();
		this->ovrs.push_back(o);
	}

	return true;
}

// СССП через год после ремонта (загрузка, оценка, экспорт)
bool VRepairEstimate::CalcSSSPAfterYear(const VFileName & fnay)
{
	std::stringstream sql;
	VSQLGenerator sqlgen;

	// --------------------------------------------------------------
	// данные из файла после ремонта
	// --------------------------------------------------------------

	this->prog->SetMessage(0, L"Загрузка данных :: файл через год после ремонта");
	this->prog->Step(0);

	this->fn_after_year = fnay;
	this->trail_id_after_year = GetTrailID(this->database, this->trans, this->fn_after_year);

	if (this->trail_id_after_year < 0)
	{
		this->message += L"Файл через год после ремонта должен быть занесен в базу";
		this->database->RollbackTransaction(this->trans);
		return false;
	}

	this->ds_after_year = VDataSetPtr(new VDataSet(this->fn_after_year));

	if (!this->ds_after_year->Load(PRM_ABS_COORD))
	{
		this->database->RollbackTransaction(this->trans);
		this->message = L"Файл через год после ремонта: " + this->ds_after_year->GetErrorMessage();
		return false;
	}

	// --------------------------------------------------------------
	// данные из файла после ремонта - апринфо
	// --------------------------------------------------------------

	this->prog->SetMessage(0, L"Загрузка данных :: файл через год после ремонта :: априорная информация");
	this->prog->Step(0);
	
	if (!this->Makeinfo(this->ds_after_year, L"Файл через год после ремонта: "))
	{
		return false;
	}

	// --------------------------------------------------------------
	// данные из файла после ремонта - статистика
	// --------------------------------------------------------------

	this->prog->SetMessage(0, L"Загрузка данных :: файл через год после ремонта :: статистика");
	this->prog->Step(0);

	if (!this->ds_after_year->Load(PRM_STATISTICS))
	{
		this->database->RollbackTransaction(this->trans);
		this->message = L"Файл через год после ремонта: " + this->ds_after_year->GetErrorMessage();
		return false;
	}

	// км оценка
	for (int km = this->repair_section.start.km; km <= this->repair_section.end.km; ++km)
	{
/*	
		// пропускаем первый километр, если от него нам осталось 100м или менее
		if (km == this->repair_section.start.km && 
			this->repair_section.start.m >= this->ds_after_year->GetAprinfo().GetKmLength(km) - 100) 
		{
			continue;
		}

		// пропускаем последний километр, если от него нам осталось 100м или менее
		if (km == this->repair_section.end.km && this->repair_section.end.m <= 100) 
		{
			continue;
		}
*/
		
		double kmBegin = this->ds_after_year->GetAprinfo().GetAbsCoord(VWayCoord(km, 0));
		size_t idxBegin = this->ds_after_year->GetAbsCoord().FindIndex(kmBegin, true);

		double kmEnd = this->ds_after_year->GetAprinfo().GetAbsCoord(VWayCoord(km+1, 0));
		size_t idxEnd = this->ds_after_year->GetAbsCoord().FindIndex(kmEnd, true);

		/// FIX! пропускаем несуществующие километры
		if (idxBegin >= idxEnd) continue;

		KmEstimate cur_km;
		cur_km.km = km;

		// сссп
		const VStatData & cur_stat = this->ds_after_year->GetStatistics().GetKmList().Find(km, 0);
		if (cur_stat.km == km)
		{
			cur_km.sssp = cur_stat.sssp;
		}

		this->kms_after_year.push_back(cur_km);
	}

	// --------------------------------------------------------------
	// экспорт статистики
	// --------------------------------------------------------------

	this->prog->SetMessage(0, L"Оценка ремонта :: экспорт данных");
	this->prog->Step(0);

	for (size_t i = 0; i < this->kms_after_year.size(); ++i)
	{
		sqlgen.Init("repair_sssp_in_year");

		sqlgen.AddInteger("msr_trail_id", this->trail_id_after_year);
		sql << "select id from repair_estim where repair_project_id = " << this->repair_id;
		sqlgen.AddSQL("repair_estim_id", sql.str());
		sql.str("");

		sqlgen.AddInteger("num_km", this->kms_after_year[i].km);
		sqlgen.AddInteger("sssp", this->kms_after_year[i].sssp);
		sqlgen.AddString("prog_version", wstring_to_string(revision));

		if (!this->ExecuteStatement(sqlgen))
		{
			return false;
		}
	}

	return true;
}

// загрузка данных
bool VRepairEstimate::Estimate(const VFileName & fnb, const VFileName & fna)
{
	std::stringstream sql;
	VSQLGenerator sqlgen;

	// --------------------------------------------------------------
	// данные из файла до ремонта
	// --------------------------------------------------------------

	this->prog->SetMessage(0, L"Загрузка данных :: файл до ремонта");
	this->prog->Step(0);

	this->fn_before = fnb;
	this->trail_id_before = GetTrailID(this->database, this->trans, this->fn_before);

	if (this->trail_id_before < 0)
	{
		this->message += L"Файл после ремонта должен быть занесен в базу";
		this->database->RollbackTransaction(this->trans);
		return false;
	}

	this->ds_before = VDataSetPtr(new VDataSet(this->fn_before));

	if (!this->ds_before->Load(PRM_ABS_COORD))
	{
		this->database->RollbackTransaction(this->trans);
		this->message = L"Файл до ремонта: " + this->ds_before->GetErrorMessage();
		return false;
	}

	// --------------------------------------------------------------
	// данные из файла до ремонта - апринфо
	// --------------------------------------------------------------

	this->prog->SetMessage(0, L"Загрузка данных :: файл до ремонта :: априорная информация");
	this->prog->Step(0);
	
	if (!this->Makeinfo(this->ds_before, L"Файл до ремонта: "))
	{
		return false;
	}

	// --------------------------------------------------------------
	// данные из файла до ремонта - параметры
	// --------------------------------------------------------------

	std::vector<PARAMETER_TYPE> param_list;
	param_list.push_back(PRM_WIDTH);
	param_list.push_back(PRM_PROFILE);
	param_list.push_back(PRM_PROF_MARKS);
	param_list.push_back(PRM_PROFILE_SLOPE_ELEMENT);
	param_list.push_back(PRM_JAGGY_PROFILE);
	param_list.push_back(PRM_PROFILE_SLOPE_ELEMENT);
	param_list.push_back(PRM_CURVE_MARKS);
	param_list.push_back(PRM_STATISTICS);
	param_list.push_back(this->jaggy_plan_on_lines ? PRM_JAGGY_PLAN_ON_LINES : PRM_JAGGY_PLAN);

	for (size_t i = 0; i < param_list.size(); ++i)
	{
		PARAMETER_TYPE param = param_list[i];

		std::wstring msg = L"Загрузка данных :: файл до ремонта :: ";
		msg += GetUserParamName(param);
		this->prog->SetMessage(0, msg.c_str());
		this->prog->Step(0);

		if (!this->ds_before->Load(param))
		{
			if (param == PRM_CURVE_MARKS)
			{
				LOG_WARNING(L"Файл до ремонта: Не загружена разметка кривых");
			}
			else
			{
				this->database->RollbackTransaction(this->trans);
				this->message = L"Файл до ремонта: " + this->ds_before->GetErrorMessage();
				return false;
			}
		}

		if (param == PRM_PROFILE)
		{
			this->ds_before->GetProfMarks().UpdateValues(this->ds_before->GetParameter(PRM_PROFILE));
		}
	}

	// --------------------------------------------------------------
	// данные из файла после ремонта
	// --------------------------------------------------------------

	this->prog->SetMessage(0, L"Загрузка данных :: файл после ремонта");
	this->prog->Step(0);

	this->fn_after = fna;
	this->trail_id_after = GetTrailID(this->database, this->trans, this->fn_after);

	if (this->trail_id_after < 0)
	{
		this->message += L"Файл до ремонта должен быть занесен в базу";
		this->database->RollbackTransaction(this->trans);
		return false;
	}

	this->ds_after = VDataSetPtr(new VDataSet(this->fn_after));

	if (!this->ds_after->Load(PRM_ABS_COORD))
	{
		this->database->RollbackTransaction(this->trans);
		this->message = L"Файл после ремонта: " + this->ds_after->GetErrorMessage();
		return false;
	}

	// --------------------------------------------------------------
	// данные из файла после ремонта - апринфо
	// --------------------------------------------------------------

	this->prog->SetMessage(0, L"Загрузка данных :: файл после ремонта :: априорная информация");
	this->prog->Step(0);
	
	if (!this->Makeinfo(this->ds_after, L"Файл после ремонта: "))
	{
		return false;
	}

	// --------------------------------------------------------------
	// данные из файла после ремонта - параметры
	// --------------------------------------------------------------

	for (size_t i = 0; i < param_list.size(); ++i)
	{
		PARAMETER_TYPE param = param_list[i];

		std::wstring msg = L"Загрузка данных :: файл после ремонта :: ";
		msg += GetUserParamName(param);
		this->prog->SetMessage(0, msg.c_str());
		this->prog->Step(0);

		if (!this->ds_after->Load(param))
		{
			if (param == PRM_CURVE_MARKS)
			{
				LOG_WARNING(L"Файл после ремонта: Не загружена разметка кривых");
			}
			else
			{
				this->database->RollbackTransaction(this->trans);
				this->message = L"Файл после ремонта: " + this->ds_after->GetErrorMessage();
				return false;
			}
		}

		if (param == PRM_PROFILE)
		{
			this->ds_before->GetProfMarks().UpdateValues(this->ds_before->GetParameter(PRM_PROFILE));
		}
	}

	// --------------------------------------------------------------
	// Оценка ремонта
	// --------------------------------------------------------------

	this->prog->SetMessage(0, L"Оценка ремонта :: файл до ремонта");
	this->prog->Step(0);

	this->repair_area_before.Estimate(this->repair_section, this->ds_before, this->repair_type, this->ovrs, this->jaggy_plan_on_lines);

	this->prog->SetMessage(0, L"Оценка ремонта :: файл после ремонта");
	this->prog->Step(0);

	this->repair_area_after.Estimate(this->repair_section, this->ds_after, this->repair_type, this->ovrs, this->jaggy_plan_on_lines);

	// --------------------------------------------------------------
	// Экспорт данных
	// --------------------------------------------------------------

	this->prog->SetMessage(0, L"Оценка ремонта :: экспорт данных");
	this->prog->Step(0);

	sql << "insert into repair_estim (repair_project_id) values (" << repair_id << ";";
	if (!this->ExecuteStatement(sql.str()))
	{
		return false;
	}
	sql.str("");

	// оценка участка до ремонта

	sqlgen.Init("repair_sections");
	sqlgen.AddInteger("msr_trail_id", this->trail_id_before);

	sql << "select id from repair_estim where repair_project_id = " << this->repair_id;
	sqlgen.AddSQL("repair_estim_id", sql.str());
	sql.str("");

	sqlgen.AddSQL("enu_repair_section_type_id", "select id from enu_repair_section_type where name='До ремонта'");

	this->repair_area_before.Export(sqlgen);
	
	if (!this->ExecuteStatement(sqlgen))
	{
		return false;
	}

	for (size_t i = 0; i < this->repair_area_before.kms.size(); ++i)
	{
		sqlgen.Init("repair_km_estim");

		sql << "select id from repair_sections where repair_estim_id = ";
		sql << "(select id from repair_estim where repair_project_id = " << this->repair_id << " and ";
		sql << "msr_trail_id = " << this->trail_id_before;
		sqlgen.AddSQL("repair_sections_id", sql.str());
		sql.str("");

		this->repair_area_before.kms[i].Export(sqlgen);

		if (!this->ExecuteStatement(sqlgen))
		{
			return false;
		}
	}

	// оценка участка после ремонта

	sqlgen.Init("repair_sections");
	sqlgen.AddInteger("msr_trail_id", this->trail_id_after);

	sql << "select id from repair_estim where repair_project_id = " << this->repair_id;
	sqlgen.AddSQL("repair_estim_id", sql.str());
	sql.str("");

	sqlgen.AddSQL("enu_repair_section_type_id", "select id from enu_repair_section_type where name='После ремонта'");

	this->repair_area_after.Export(sqlgen);
	
	if (!this->ExecuteStatement(sqlgen))
	{
		return false;
	}

	for (size_t i = 0; i < this->repair_area_after.kms.size(); ++i)
	{
		sqlgen.Init("repair_km_estim");

		sql << "select id from repair_sections where repair_estim_id = ";
		sql << "(select id from repair_estim where repair_project_id = " << this->repair_id << " and ";
		sql << "msr_trail_id = " << this->trail_id_after;
		sqlgen.AddSQL("repair_sections_id", sql.str());
		sql.str("");

		this->repair_area_after.kms[i].Export(sqlgen);

		if (!this->ExecuteStatement(sqlgen))
		{
			return false;
		}
	}

	// оценка кривых
	this->prog->SetMessage(0, L"Оценка ремонта :: экспорт кривых");
	this->prog->Step(0);

	if (!this->ExportCurves(this->trail_id_after, this->repair_area_after, this->ds_after))
	{
		return false;
	}

	return true;
}

// Экспорт кривых
bool VRepairEstimate::ExportCurves(__int64 trail_id, RepairArea & area, VDataSetPtr ds)
{
	VSQLGenerator sqlgen;
	std::stringstream sql;

	VCurveExport crvx(CURVE_REPAIR, ds, this->database, this->trans, trail_id, this->repair_id, false);

	VCurveList & curves = ds->GetCurves();

	for (size_t i = 0; i < curves.GetCount(); ++i)
	{
		VCurve & curve = curves.GetItem(i);

		// попадание на ремонт
		if (area.Crossing(curve, ds) < 0.5) continue;

		if (!crvx.Export(curve, i))
		{
			this->message = crvx.GetMessage();
			return false;
		}

		this->prog->Step(0);
	}

	return true;
}

void VRepairEstimate::Finalize()
{
	this->prog->SetMessage(0, L"Завершение работы");
	this->prog->Step(0);

	// Устанавливаем NULL не оценённые поля
	this->ExecuteStatement("update repair_sections set max_uklon = null where max_uklon = 0");
	this->ExecuteStatement("update repair_sections set max_razn_ukl = null where max_razn_ukl = 0");
	this->ExecuteStatement("update repair_sections set min_len_elprof = null where min_len_elprof = 10000");

	this->database->CommitTransaction(this->trans);
//	if (this->ds_before->IsLoaded() this->ds_before->Unload(PRM_EVERYTHING);
//	if (this->ds_after->IsLoaded()) this->ds_after->Unload(PRM_EVERYTHING);
//	if (this->ds_after_year->IsLoaded()) this->ds_after_year->Unload(PRM_EVERYTHING);
}