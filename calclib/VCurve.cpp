#include "VCurve.h"
#include "VDataSet.h"
#include <float.h>
#include <fstream>
#include <iomanip>
#include <numeric>
        
// расчет Vпр по значению отвода
static VSpeeds calc_vpr(double tap, bool repair)
{
	static double vpr_limits[3][20] = 
	{
		{ 0.705, 0.805, 0.905, 1.005, 1.105, 1.205, 1.405, 1.505, 1.605, 1.705, 1.805, 1.905, 2.105, 2.305, 2.505, 2.705, 2.905, 3.005, 3.105, 3.205 },
		{ 250.0, 220.0, 200.0, 180.0, 160.0, 140.0, 120.0, 110.0, 100.0,  95.0,  90.0,  85.0,  80.0,  75.0,  70.0,  65.0,  55.0,  50.0,  40.0,  25.0 },
		{  90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  90.0,  85.0,  80.0,  80.0,  75.0,  70.0,  65.0,  60.0,  55.0,  50.0,  40.0,  25.0 }
	};
	
	static double vpr_repair_limits[3][9] = 
	{
	    { 0.505, 0.705, 1.005, 1.205, 1.405, 1.905, 2.705, 3.105, 3.205 },
	    { 200.0, 160.0, 120.0, 110.0, 100.0,  80.0,  60.0,  40.0,  25.0 },
	    {  90.0,  90.0,  90.0,  90.0,  90.0,  80.0,  60.0,  40.0,  25.0 }
	};
		
	VSpeeds spd(0, 0, 0);
    if (repair)
    {
        for (int i = 0; i < 9; ++i)
        {
            if (tap < vpr_repair_limits[0][i])
            {
                spd = VSpeeds(vpr_repair_limits[1][i], vpr_repair_limits[2][i], vpr_repair_limits[2][i]);
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < 20; ++i)
        {
            if (tap < vpr_limits[0][i])
            {
                spd = VSpeeds(vpr_limits[1][i], vpr_limits[2][i], vpr_limits[2][i]);
                break;
            }
        }
    }
	return spd;
}

// ---------------------------------------------------------------------
// пикеты с ограничениями скорости
// ---------------------------------------------------------------------

// Конструктор
VCurvePicket::VCurvePicket() : VPicket(), 
					isVkrPass(false), isVkrTruck(false), 
					isVprPass(false), isVprTruck(false), 
					isVizPass(false), isVizTruck(false)
{}

// Копирующий конструктор [1]
VCurvePicket::VCurvePicket(const VPicket & pk) : VPicket(pk), 
					isVkrPass(false), isVkrTruck(false), 
					isVprPass(false), isVprTruck(false), 
					isVizPass(false), isVizTruck(false)
{}

// Копирующий конструктор [2]
VCurvePicket::VCurvePicket(const VCurvePicket & pk) : VPicket(pk), 
					isVkrPass(pk.isVkrPass), isVkrTruck(pk.isVkrTruck), 
					isVprPass(pk.isVprPass), isVprTruck(pk.isVprTruck), 
					isVizPass(pk.isVizPass), isVizTruck(pk.isVizTruck)
{}

// Наличие ограничения на пикете
bool VCurvePicket::HasLimit() const
{
	return this->isVkrPass || this->isVkrTruck || 
		this->isVprPass || this->isVprTruck || 
		this->isVizPass || this->isVizTruck;
}

// ---------------------------------------------------------------------
// пикеты с износами
// ---------------------------------------------------------------------

// Конструктор
VCurveWearPicket::VCurveWearPicket() : VPicket(), 
					maxWear(0.0), avgWear(0.0), 
					maxWidth(1520.0), avgWidth(1520.0),
					prkPass(350), prkTruck(350), spdPass(350), spdTruck(350), 
					ntd(false), errorWear(0.0), errorWidth(0.0)
{}

// Копирующий конструктор [1]
VCurveWearPicket::VCurveWearPicket(const VPicket & pk) : VPicket(pk), 
					maxWear(0.0), avgWear(0.0), 
					maxWidth(1520.0), avgWidth(1520.0),
					prkPass(350), prkTruck(350), spdPass(350), spdTruck(350), 
					ntd (false), errorWear(0.0), errorWidth(0.0)
{}

// Копирующий конструктор [2]
VCurveWearPicket::VCurveWearPicket(const VCurveWearPicket & pk) : VPicket(pk), 
					maxWear(pk.maxWear), avgWear(pk.avgWear), 
					maxWidth(pk.maxWidth), avgWidth(pk.avgWidth),
					prkPass(pk.prkPass), prkTruck(pk.prkTruck),
					spdPass(pk.spdPass), spdTruck(pk.spdTruck), ntd(pk.ntd),
					errorWear(pk.errorWear), errorWidth(pk.errorWidth)
{}

// ---------------------------------------------------------------------
// Плети
// ---------------------------------------------------------------------

VCurveLink::VCurveLink() : absStart(0), absEnd(0), avgWear(0), maxWear(0), errorWear(0),
					avgWidth(0), maxWidth(0), errorWidth(0), spdPass(350), spdTruck(350),
					ntd(false)
{}

// ---------------------------------------------------------------------
// временные параметры кривой
// ---------------------------------------------------------------------

// Установка границ кривой
void VCurveTempParams::SetRanges(VDataSet & ds) 
{
	if (!ds.GetAbsCoord().IsLoaded()) ds.Load(PRM_ABS_COORD);
	if (this->ranges.empty())
	{
		size_t markCnt = this->curve->GetMarkCount();
		uint32_t firstMark = std::min<uint32_t>(this->curve->GetPlanMark(0).index, this->curve->GetLevelMark(0).index);
		uint32_t lastMark = std::max<uint32_t>(this->curve->GetPlanMark(markCnt-1).index, this->curve->GetLevelMark(markCnt-1).index);
		firstMark = ds.GetAbsCoord().FindIndex(ds.GetAbsCoord().GetItem(firstMark) - 10.0, true);
		lastMark = ds.GetAbsCoord().FindIndex(ds.GetAbsCoord().GetItem(lastMark) + 10.0, true);
		ranges.push_back(firstMark);
		ranges.push_back(lastMark);
	}
}

// Установка скоростей по приказу
void VCurveTempParams::SetSpeeds(VDataSet & ds, bool isModel)
{
	this->SetRanges(ds);
	if (isModel)
	{
		if (this->vprkPass.empty())
		{
			this->vprkPass.resize(this->GetKadrCount(), double(this->curve->model.Vpass));
			this->vprkTruck.resize(this->GetKadrCount(), double(this->curve->model.Vcrg));
			this->vprkThird.resize(this->GetKadrCount(), double(this->curve->model.Vavg));
			this->vprkHigh.resize(this->GetKadrCount(), double(this->curve->model.Vhigh));
			this->vprkHighBT.resize(this->GetKadrCount(), double(this->curve->model.Vhigh_bt));
		}
	}
	else
	{
		double crv_from = ds.GetAbsCoord().GetItem(this->ranges[0]);
		double crv_to = ds.GetAbsCoord().GetItem(this->ranges[1]);

		if (this->vprkPass.empty())
		{
			if (!ds.GetAprinfo().IsLoaded()) ds.Load(PRM_APRINFO);
			long vpass = 140;
			long vtruck = 90;
			long vthird = 90*0.75;
			long vhigh = NO_SPEED;
			long vhigh_bt = NO_SPEED;

			if (ds.GetAprinfo().IsLoaded())
			{
				vpass = ds.GetAprinfo().GetMinPassSpeed(crv_from, crv_to);
				vtruck = ds.GetAprinfo().GetMinTruckSpeed(crv_from, crv_to);
				vthird = 0.75 * vtruck;
				vhigh = ds.GetAprinfo().GetMinHighSpeed(crv_from, crv_to);
				vhigh_bt = ds.GetAprinfo().GetMinHighSpeedBT(crv_from, crv_to);
			}

			this->vprkPass.resize(this->GetKadrCount(), vpass);
			this->vprkTruck.resize(this->GetKadrCount(), vtruck);
			this->vprkThird.resize(this->GetKadrCount(), vthird);
			this->vprkHigh.resize(this->GetKadrCount(), vhigh);
			this->vprkHighBT.resize(this->GetKadrCount(), vhigh_bt);
		}
	}
}

// Установить категорию пути
void VCurveTempParams::SetWayCat(VDataSet & ds, bool isModel)
{
	this->SetSpeeds(ds, isModel);

	if (!ds.GetAprinfo().IsLoaded()) ds.Load(PRM_APRINFO);

	VAbsCoordData * absCoord = &ds.GetAbsCoord();
	VAprinfo * aprinfo = &ds.GetAprinfo();

	if (this->wayCat == WAYCAT_UNSET)
	{
		// после выноса высоких скоростей в отдельное понятие
		// категория не роляет

		this->wayCat = aprinfo->GetWayCat(
			absCoord->GetItem(this->ranges.at(0)), 
			absCoord->GetItem(this->ranges.at(1)));

		if (isModel)
		{
			this->anpPassRangeMax = 0.7;
			this->anpPassRangeMid = 0.7;
			this->anpTruckRangeMax = 0.7;
			this->anpTruckRangeMid = 0.7; 

			this->psiPassRangeMax = 0.6;
			this->psiPassRangeMid = 0.6;
			this->psiTruckRangeMax = 0.3;
			this->psiTruckRangeMid = 0.3;

			this->anpHighBTRangeMax = 1.0;
			this->anpHighBTRangeMid = 1.0;
			this->anpHighRangeMax = 1.0;
			this->anpHighRangeMid = 1.0;

			this->psiHighBTRangeMax = 0.9;
			this->psiHighBTRangeMid = 0.9;
			this->psiHighRangeMax = 0.9;
			this->psiHighRangeMid = 0.9;
		}

		else
		{
		    /// 17.02.2014 убраны "старые" нормативы
			this->anpPassRangeMax = 0.8;
			this->anpPassRangeMid = 0.75;
			this->anpTruckRangeMax = 0.75;
			this->anpTruckRangeMid = 0.7; 

			this->psiPassRangeMax = 0.66;
			this->psiPassRangeMid = 0.66;
			this->psiTruckRangeMax = 0.66;
			this->psiTruckRangeMid = 0.66;

		    /* ЦПТ-46/15
			this->anpPassRangeMax = 0.8;
			this->anpPassRangeMid = 0.7;
			this->anpTruckRangeMax = 0.75;
			this->anpTruckRangeMid = 0.7; 
    		this->psiPassRangeMax = 0.7;
    		this->psiPassRangeMid = 0.6;
   			this->psiTruckRangeMax = 0.7;
    		this->psiTruckRangeMid = 0.6;*/

		    this->psiHighBTRangeMax = 0.9;
	    	this->psiHighBTRangeMid = 0.9;
    		this->psiHighRangeMax = 0.66;
   			this->psiHighRangeMid = 0.66;

		    this->anpHighBTRangeMax = 1.5;
	    	this->anpHighBTRangeMid = 1.0;
    		this->anpHighRangeMax = 1.4;
   			this->anpHighRangeMid = 0.9;
		}

		if (this->vprkHighBT.at(0) >= 0 || this->vprkHigh.at(0) >= 0)
		{
			this->baseMid = 60.0;
			this->baseTap = 40.0;
		}
		else
		{
			this->baseMid = 50.0;
			this->baseTap = 30.0;
		}
	}
}

// Создания сжатого списка скоростей из покадрового
std::vector<VCurveTempParams::SpdArea> VCurveTempParams::SelectSpeeds(VDataSet & ds, size_t from, size_t to, bool isModel)
{
	this->SetSpeeds(ds, isModel);
	std::vector<VCurveTempParams::SpdArea> result;
	VCurveTempParams::SpdArea spd;

	if (from > this->vprkPass.size()) from = 0;

	spd.idxFrom = from;
	spd.idxTo = from;
	spd.spdPass = speed_t(this->vprkPass[from]);
	spd.spdTruck = speed_t(this->vprkTruck[from]);
	spd.spdThird = speed_t(this->vprkThird[from]);
	spd.spdHigh = speed_t(this->vprkHigh[from]);
	spd.spdHighBT = speed_t(this->vprkHighBT[from]);
	result.push_back(spd);

	for (size_t i = from; i < to; ++i)
	{
		if (i >= this->vprkPass.size()) break;

		result.back().idxTo = i;
		if (speed_t(this->vprkPass[i]) != result.back().spdPass || 
			speed_t(this->vprkTruck[i]) != result.back().spdTruck ||
			speed_t(this->vprkThird[i]) != result.back().spdThird ||
			speed_t(this->vprkHigh[i]) != result.back().spdHigh ||
			speed_t(this->vprkHighBT[i]) != result.back().spdHighBT)
		{
			spd.idxFrom = i;
			spd.idxTo = i;
			spd.spdPass = long(this->vprkPass[i]);
			spd.spdTruck = long(this->vprkTruck[i]);
			spd.spdThird = long(this->vprkThird[i]);
			spd.spdHigh = long(this->vprkHigh[i]);
			spd.spdHighBT = long(this->vprkHighBT[i]);
			result.push_back(spd);
		}
	}

	result.back().idxTo = to;
	return result;
}

// Расчет курса
void VCurveTempParams::CalcKurs(VDataSet & ds, bool isModel)
{
	this->SetRanges(ds);
	this->SetWayCat(ds, isModel);
	VAbsCoordData * absCoord = &ds.GetAbsCoord();

	if (isModel)
	{
		this->CalcMidPlan(ds, isModel);
		if (this->kurs.empty()) 
		{
			this->kurs.resize(this->GetKadrCount(), 0.0f);
			double prev_kurs = 0;
			for (uint32_t i = 1; i < this->GetKadrCount(); ++i)
			{
				size_t index = i + ranges[0];
				double sum_curv = this->midPlan[i] + this->midPlan[i-1];
				double step = absCoord->GetItem(index) - absCoord->GetItem(index-1);
				
				double value = prev_kurs;
				if (step > 0.05)
				{
					value += sum_curv * step / 100000.0;
				}

				this->kurs[i] = value;
				prev_kurs = value;
			}
		}
	}
	else
	{
		/* // пересчет по кривизне отключен. оставлено для справки - если придется возвращаать
		if (PreCalculateKurs)
		{
			if (!ds.GetParameter(PRM_CURV).IsLoaded()) ds.Load(PRM_CURV);
			VParameter * curv = &ds.GetParameter(PRM_CURV);

			if (this->kurs.empty()) 
			{
				this->kurs.resize(this->GetKadrCount(), 0);

				double value = 0;
				for (uint32_t i = 1; i < this->GetKadrCount(); ++i)
				{
					size_t index = i + this->ranges[0];
					double sum_curv = double(curv->GetItem(index)) + double(curv->GetItem(index - 1));
					double step = absCoord->GetItem(index) - absCoord->GetItem(index - 1);
					value += sum_curv * step / 100000.0;

					this->kurs[i] = value;
				}
			}
		}*/
		
		if (!ds.GetParameter(PRM_KURS).IsLoaded()) ds.Load(PRM_KURS);
		VParameter & k = ds.GetParameter(PRM_KURS);

		if (this->kurs.empty())
		{
			this->kurs.resize(this->GetKadrCount(), 0);
			for (uint32_t i = 0; i < this->GetKadrCount(); ++i)
			{
				this->kurs[i] = k.GetItem(i + this->ranges[0]);
			}
		}
	}
}

// Расчет средней кривизны
void VCurveTempParams::CalcMidPlan(VDataSet & ds, bool isModel)
{
	this->SetRanges(ds);
	double orientation = this->curve->GetOrientation(ds) < 0.0 ? -1.0 : 1.0;
	double value;

	if (isModel)
	{
		if (this->midPlan.empty())
		{
			this->midPlan.resize(this->GetKadrCount(), 0);
			for (size_t i = 0; i < this->GetKadrCount(); ++i)
			{
				value = this->curve->AproxPlan(i + this->ranges[0], ds.GetAbsCoord());
				this->midPlan[i] = value * orientation;
			}		
		}
	}
	else
	{
		this->SetWayCat(ds, false);
		if (!ds.GetParameter(PRM_CURV).IsLoaded()) ds.Load(PRM_CURV);
		VParameter & curv = ds.GetParameter(PRM_CURV);

		if (this->midPlan.empty())
		{
			this->midPlan.resize(this->GetKadrCount(), 0);
			for (size_t i = 0; i < this->GetKadrCount(); ++i)
			{
				value = curv.GetMO(this->ranges[0] + i, this->baseMid/2, ds.GetAbsCoord());
				this->midPlan[i] = value * orientation;
			}
		}
	}
}

// Расчет среднего уровня
void VCurveTempParams::CalcMidLevel(VDataSet & ds, bool isModel)
{
	this->SetRanges(ds);
	float orientation = this->curve->GetOrientation(ds) < 0.0f ? -1.0f : 1.0f;
	float value;

	if (isModel)
	{
		if (this->midLevel.empty())
		{
			this->midLevel.resize(this->GetKadrCount(), 0);
			for (size_t i = 0; i < this->GetKadrCount(); ++i)
			{
				value = (float) this->curve->AproxLevel(i + this->ranges[0], ds.GetAbsCoord());
				this->midLevel[i] = value * orientation;
			}		
		}
	}
	else
	{
		this->SetWayCat(ds, false);
		if (!ds.GetParameter(PRM_LEVEL).IsLoaded()) ds.Load(PRM_LEVEL);
		VParameter & level = ds.GetParameter(PRM_LEVEL);

		if (this->midLevel.empty())
		{
			this->midLevel.resize(this->GetKadrCount(), 0);
			for (size_t i = 0; i < this->GetKadrCount(); ++i)
			{
				float value = level.GetMO(this->ranges[0] + i, this->baseMid/2, ds.GetAbsCoord());
				this->midLevel[i] = value * orientation;
			}
		}
	}
}

// Расчет мгновенных радиусов
void VCurveTempParams::CalcRadius(VDataSet & ds, bool isModel)
{
	this->CalcMidPlan(ds, isModel);
	float crv;

	if (this->radius.empty())
	{
		this->radius.resize(this->GetKadrCount(), 0);
		for (size_t i = 0; i < this->GetKadrCount(); ++i)
		{
			if (fabs(this->midPlan[i]) < 1.0f)
			{
				if (this->midPlan[i] > 0)	crv = 1.0f;
				else						crv = -1.0f;
			}
			else
				crv = this->midPlan[i];

			this->radius[i] = curv_to_radius(this->midPlan[i]);
		}
	}
}

// Расчет непогашенных
void VCurveTempParams::CalcAnp(VDataSet & ds, bool isModel)
{
	this->SetSpeeds(ds, isModel);
	this->CalcMidPlan(ds, isModel);
	this->CalcMidLevel(ds, isModel);

	if (this->anpPass.empty() || this->anpTruck.empty())
	{
		this->anpPass.resize(this->GetKadrCount(), 0);
		this->anpTruck.resize(this->GetKadrCount(), 0);
		this->anpThird.resize(this->GetKadrCount(), 0);
		this->anpHigh.resize(this->GetKadrCount(), 0);
		this->anpHighBT.resize(this->GetKadrCount(), 0);

		for (size_t i = 0; i < this->GetKadrCount(); ++i)
		{
		    this->anpPass[i]  = calc_anp(this->vprkPass[i], this->midPlan[i], this->midLevel[i]);
		    this->anpTruck[i] = calc_anp(this->vprkTruck[i], this->midPlan[i], this->midLevel[i]);
		    this->anpThird[i] = calc_anp(this->vprkThird[i], this->midPlan[i], this->midLevel[i]);

			if (this->vprkHigh[i] != NO_SPEED)
			{
    		    this->anpHigh[i] = calc_anp(this->vprkHigh[i], this->midPlan[i], this->midLevel[i]);
			}

			if (this->vprkHighBT[i] != NO_SPEED)
			{
			    this->anpHighBT[i] = calc_anp(this->vprkHighBT[i], this->midPlan[i], this->midLevel[i]);
    		}
		}
	}
}

// Среднее Vкр на круговой
long VCurveTempParams::GetMidVkr(VDataSet & ds, bool isModel, float anpRange, uint32_t radiusNdx)
{
	this->SetWayCat(ds, isModel);
	this->SetSpeeds(ds, isModel);
	this->CalcMidPlan(ds, isModel);
	this->CalcMidLevel(ds, isModel);
	this->CalcAnp(ds, isModel);
	
	size_t fromNdx = this->curve->GetPlanMark(this->curve->GetRangeMark(radiusNdx, true, true)).index;
	size_t toNdx = this->curve->GetPlanMark(this->curve->GetRangeMark(radiusNdx, true, false)).index;
	fromNdx -= this->ranges[0];
	toNdx -= this->ranges[0];

	long vkrMid = 500;
	while (true)
    {
        double anp = 0.0;
        for (size_t i = fromNdx; i < toNdx; ++i)
        {
            anp += calc_anp(vkrMid, this->midPlan[i], this->midLevel[i]);
        }
        anp /= toNdx - fromNdx;

        if (anp < anpRange) break;

        --vkrMid;
        if (vkrMid == 0) break;
    }

    return vkrMid;
}

// Расчет Vкр
void VCurveTempParams::CalcVkr(VDataSet & ds, bool isModel)
{
	this->SetWayCat(ds, isModel);
	this->SetSpeeds(ds, isModel);
	this->CalcMidPlan(ds, isModel);
	this->CalcMidLevel(ds, isModel);
	this->CalcAnp(ds, isModel);

	if (this->vkrPass.empty() || this->vkrTruck.empty())
	{
		this->vkrPass.resize(this->GetKadrCount(), 500);
		this->vkrTruck.resize(this->GetKadrCount(), 500);
		this->vkrHigh.resize(this->GetKadrCount(), 500);
		this->vkrHighBT.resize(this->GetKadrCount(), 500);

		for (size_t i = 0; i < this->GetKadrCount(); ++i)
		{
		    this->vkrPass[i]   = calc_vanp(this->midPlan[i], this->midLevel[i], this->anpPassRangeMax);
		    this->vkrTruck[i]  = calc_vanp(this->midPlan[i], this->midLevel[i], this->anpTruckRangeMax);
		    this->vkrHigh[i]   = calc_vanp(this->midPlan[i], this->midLevel[i], this->anpHighRangeMax);
		    this->vkrHighBT[i] = calc_vanp(this->midPlan[i], this->midLevel[i], this->anpHighBTRangeMax);
		}

		// добавляем ограничения по среднему Анп
		for (size_t radiusNdx = 0; radiusNdx < this->curve->GetRadiusCount(); ++radiusNdx)
		{
			size_t fromNdx = this->curve->GetPlanMark(this->curve->GetRangeMark(radiusNdx, true, true)).index;
			size_t toNdx = this->curve->GetPlanMark(this->curve->GetRangeMark(radiusNdx, true, false)).index;
			fromNdx -= this->ranges[0];
			toNdx -= this->ranges[0];

/*			long vkrMid = 500, vkrMidPass = -1, vkrMidTruck = -1, vkrMidHigh = -1, vkrMidHighBT = -1;
			while (true)
			{
				double anp = 0.0;
				for (size_t i = fromNdx; i < toNdx; ++i)
				{
				    anp += calc_anp(vkrMid, this->midPlan[i], this->midLevel[i]);
				}
				anp /= toNdx - fromNdx;
			
				if (vkrMidPass < 0 && anp < this->anpPassRangeMid) vkrMidPass = vkrMid;
				if (vkrMidTruck < 0 && anp < this->anpTruckRangeMid) vkrMidTruck = vkrMid;
				if (vkrMidHighBT < 0 && anp < this->anpHighBTRangeMid) vkrMidHighBT = vkrMid;
				if (vkrMidHigh < 0 && anp < this->anpHighRangeMid) vkrMidHigh = vkrMid;

				if (vkrMidTruck > 0 && vkrMidPass > 0 && vkrMidHigh > 0 && vkrMidHighBT > 0)
					break;

				--vkrMid;
				if (vkrMid == 0)
				{
					vkrMidPass = 0;
					vkrMidTruck = 0;
					vkrMidHigh = 0;
					vkrMidHighBT = 0;
					break;
				}
			}*/
			
			long vkrMidPass = this->GetMidVkr(ds, isModel, this->anpPassRangeMid, radiusNdx);
			long vkrMidTruck = this->GetMidVkr(ds, isModel, this->anpTruckRangeMid, radiusNdx);
			long vkrMidHigh = this->GetMidVkr(ds, isModel, this->anpHighBTRangeMid, radiusNdx);
			long vkrMidHighBT = this->GetMidVkr(ds, isModel, this->anpHighRangeMid, radiusNdx);

			for (size_t i = fromNdx; i < toNdx; ++i)
			{
				if (this->vkrPass[i] > vkrMidPass)		this->vkrPass[i] = vkrMidPass;
				if (this->vkrTruck[i] > vkrMidTruck)	this->vkrTruck[i] = vkrMidTruck;
				if (this->vkrHigh[i] > vkrMidHigh)		this->vkrHigh[i] = vkrMidHigh;
				if (this->vkrHighBT[i] > vkrMidHighBT)	this->vkrHighBT[i] = vkrMidHighBT;
			}
		}
	}
}

// Расчет псей
void VCurveTempParams::CalcPsiViz(VDataSet & ds, bool isModel)
{
	this->SetRanges(ds);
	this->SetWayCat(ds, isModel);
	this->CalcAnp(ds, isModel);

	VAbsCoordData & absCoord = ds.GetAbsCoord();

	if (this->psiPass.empty())
	{
		this->psiPass.resize(this->GetKadrCount(), 0);
		this->psiTruck.resize(this->GetKadrCount(), 0);
		this->psiHigh.resize(this->GetKadrCount(), 0);
		this->psiHighBT.resize(this->GetKadrCount(), 0);
		this->vizPass.resize(this->GetKadrCount(), 500);
		this->vizTruck.resize(this->GetKadrCount(), 500);
		this->vizHigh.resize(this->GetKadrCount(), 500);
		this->vizHighBT.resize(this->GetKadrCount(), 500);

		for (uint32_t i = 0; i < this->GetKadrCount(); ++i)
		{
			float cur_psi_pass = 0.0f, cur_psi_truck = 0.0f, 
				cur_psi_high = 0.0f, cur_psi_high_bt = 0.0f;

			long cur_viz = 500, viz_pass = -1, viz_truck = -1, 
				viz_high = -1, viz_high_bt = -1;

			double axis = absCoord.GetItem(i + this->ranges[0]);
			uint32_t i0 = absCoord.FindIndex(axis - this->baseTap/2, true);
			uint32_t i1 = absCoord.FindIndex(axis + this->baseTap/2, true);
			double a0 = absCoord.GetItem(i0);
			double a1 = absCoord.GetItem(i1);

			if (a1 - a0 >= this->baseTap - 1 && 
				i0 >= this->ranges[0] && i1 <= this->ranges[1])
			{
			    i0 -= this->ranges[0];
			    i1 -= this->ranges[0];
			
				if (this->vprkPass[i0] == this->vprkPass[i] && this->vprkPass[i1] == this->vprkPass[i])
				{
				    cur_psi_pass = calc_psi(this->vprkPass[i], this->anpPass[i0], this->anpPass[i1], a0, a1);
				}
	
				if (this->vprkTruck[i0] == this->vprkTruck[i] && this->vprkTruck[i1] == this->vprkTruck[i])
				{
				    cur_psi_truck = calc_psi(this->vprkTruck[i], this->anpTruck[i0], this->anpTruck[i1], a0, a1);
				}

				if (this->vprkHigh[i0] == this->vprkHigh[i] && this->vprkHigh[i1] == this->vprkHigh[i])
				{
				    cur_psi_high = calc_psi(this->vprkHigh[i], this->anpHigh[i0], this->anpHigh[i1], a0, a1);
				}

				if (this->vprkHighBT[i0] == this->vprkHighBT[i] && this->vprkHighBT[i1 ] == this->vprkHighBT[i])
				{
				    cur_psi_high_bt = calc_psi(this->vprkHighBT[i], this->anpHighBT[i0], this->anpHighBT[i1], a0, a1);
				}

				while (true)
				{
				    double psi_viz = calc_psi(cur_viz, this->midPlan[i0], this->midPlan[i1], this->midLevel[i0], this->midLevel[i1], a0, a1);

					if (viz_pass < 0 && psi_viz <= this->psiPassRangeMax) viz_pass = cur_viz;
					if (viz_truck < 0 && psi_viz <= this->psiTruckRangeMax) viz_truck = cur_viz;
					if (viz_high < 0 && psi_viz <= this->psiHighRangeMax) viz_high = cur_viz;
					if (viz_high_bt < 0 && psi_viz <= this->psiHighBTRangeMax) viz_high_bt = cur_viz;
					if (viz_pass > 0 && viz_truck > 0 && viz_high > 0 && viz_high_bt > 0) break;
					--cur_viz;

					if (cur_viz == 0)
					{
						viz_pass = 0;
						viz_truck = 0;
						viz_high = 0;
						viz_high_bt = 0;
						break;
					}
				}
			}

			this->psiPass[i] = cur_psi_pass;
			this->psiTruck[i] = cur_psi_truck;
			this->psiHigh[i] = cur_psi_high;
			this->psiHighBT[i] = cur_psi_high_bt;
			if (viz_pass >= 0) this->vizPass[i] = viz_pass;
			if (viz_truck >= 0) this->vizTruck[i] = viz_truck;
			if (viz_high >= 0) this->vizHigh[i] = viz_high;
			if (viz_high_bt >= 0) this->vizHighBT[i] = viz_high_bt;
		}
	}
}

// Расчет отвода кривизны
void VCurveTempParams::CalcTapPlan(VDataSet & ds, bool isModel)
{
	this->SetRanges(ds);
	this->SetWayCat(ds, isModel);
	this->CalcMidPlan(ds, isModel);

	VAbsCoordData & absCoord = ds.GetAbsCoord();

	if (this->tapPlan.empty())
	{
		this->tapPlan.resize(this->GetKadrCount(), 0);
		for (uint32_t i = 0; i < this->GetKadrCount(); ++i)
		{
			float value = 0.0f;
			double axis = absCoord.GetItem(this->ranges[0] + i);
			uint32_t i0 = absCoord.FindIndex(axis - this->baseTap/2, true);
			uint32_t i1 = absCoord.FindIndex(axis + this->baseTap/2, true);
			double a0 = absCoord.GetItem(i0);
			double a1 = absCoord.GetItem(i1);

			if (a1 - a0 >= this->baseTap - 1 && 
				i0 >= this->ranges[0] && i1 <= this->ranges[1])
			{
			    i0 -= this->ranges[0];
			    i1 -= this->ranges[0];
			    value = calc_tap(this->midPlan[i0], this->midPlan[i1], a0, a1);
			}

			this->tapPlan[i] = value;
		}
	}
}

// Расчет отвода уровня
void VCurveTempParams::CalcTapLevel(VDataSet & ds, bool isModel)
{
	this->SetRanges(ds);
	this->SetWayCat(ds, isModel);
	this->CalcMidLevel(ds, isModel);

	VAbsCoordData & absCoord = ds.GetAbsCoord();

	if (this->tapLevel.empty())
	{
		this->tapLevel.resize(this->GetKadrCount(), 0);
		for (uint32_t i = 0; i < this->GetKadrCount(); ++i)
		{
			float value = 0.0f;
			double axis = absCoord.GetItem(this->ranges[0] + i);
			uint32_t i0 = absCoord.FindIndex(axis - this->baseTap/2, true);
			uint32_t i1 = absCoord.FindIndex(axis + this->baseTap/2, true);
			double a0 = absCoord.GetItem(i0);
			double a1 = absCoord.GetItem(i1);
			
			if (a1 - a0 >= this->baseTap - 1 && 
				i0 >= this->ranges[0] && i1 <= this->ranges[1])
			{
			    i0 -= this->ranges[0];
			    i1 -= this->ranges[0];
			    value = calc_tap(this->midLevel[i0], this->midLevel[i1], a0, a1);
			}

			this->tapLevel[i] = value;
		}
	}
}

// Расчет мгновенного Vпр
void VCurveTempParams::CalcVpr(VDataSet & ds, bool isModel)
{
	this->CalcTapLevel(ds, isModel);

	if (this->vprPass.empty())
	{
		this->vprPass.resize(this->GetKadrCount(), 0);
		this->vprTruck.resize(this->GetKadrCount(), 0);
		this->vprPassRepair.resize(this->GetKadrCount(), 0);
		this->vprTruckRepair.resize(this->GetKadrCount(), 0);
		for (uint32_t i = 0; i < this->GetKadrCount(); ++i)
		{
			VSpeeds spd = calc_vpr(this->tapLevel[i], false);
			this->vprPass[i] = spd.GetPassSpeed();
			this->vprTruck[i] = spd.GetTruckSpeed();
			spd = calc_vpr(this->tapLevel[i], true);
			this->vprPassRepair[i] = spd.GetPassSpeed();
			this->vprTruckRepair[i] = spd.GetTruckSpeed();
		}
	}

	// коррекция на (коротких) переходных
	for (size_t i = 0; i < this->curve->GetRadiusCount() + 1; ++i)
	{
		VSpeeds speed = calc_vpr(this->curve->GetMaxLevelTap(ds, i), false);
		VSpeeds speed_r = calc_vpr(this->curve->GetMaxLevelTap(ds, i), true);
		uint32_t fromIdx = this->curve->GetLevelMark(2*i).index - this->ranges[0];
		uint32_t toIdx = this->curve->GetLevelMark(2*i+1).index - this->ranges[0];

		for (size_t kadr = fromIdx; kadr < toIdx; ++kadr)
		{
			if (this->vprPass[kadr] < speed.GetPassSpeed()) this->vprPass[kadr] = speed.GetPassSpeed();
			if (this->vprTruck[kadr] < speed.GetTruckSpeed()) this->vprTruck[kadr] = speed.GetTruckSpeed();
			if (this->vprPassRepair[kadr] < speed_r.GetPassSpeed()) this->vprPassRepair[kadr] = speed_r.GetPassSpeed();
			if (this->vprTruckRepair[kadr] < speed_r.GetTruckSpeed()) this->vprTruckRepair[kadr] = speed_r.GetTruckSpeed();
		}
	}
}

// Отклонение модели от факта на уровне
void VCurveTempParams::CalcLevelOffset(VDataSet & ds)
{
	if (this->levelOffset.empty() && ds.Load(PRM_LEVEL))
	{
		this->levelOffset.resize(this->GetKadrCount(), 0);

		for (size_t i = 0; i < this->GetKadrCount(); ++i)
		{
			double offset = this->curve->AproxLevel(i + this->ranges[0], ds.GetAbsCoord()) - 
				ds.GetParameter(PRM_LEVEL).GetItem(i+this->ranges[0]);

			this->levelOffset[i] = fabs(offset);
		}
	}
}

// сбросить массив в файл
bool VCurveTempParams::DumpToFile(const std::wstring & fn, const std::vector<double> & prm, VDataSet & ds)
{
	if (!ds.GetAbsCoord().IsLoaded())
		return false;

	if (this->ranges.size() != 2)	
		return false;

	if (this->GetKadrCount() == 0)
		return false;

	if (prm.size() != this->GetKadrCount())
		return false;

	std::ofstream dump(fn.c_str());
	if (!dump.is_open())
		return false;

	dump << std::fixed << std::setprecision(6);
	for (size_t i = 0; i < this->GetKadrCount(); ++i)
	{
		dump << ds.GetAbsCoord().GetItem(i + this->ranges[0]) << ";";
		dump << prm[i] << std::endl;
	}
	dump.close();
	return true;

}

// Минимум на отрезке
double VCurveTempParams::GetMin(std::vector<double> & param, 
							   size_t from, size_t to)
{
	if (to <= from) return 0.0;
	return *std::min_element(param.begin() + from, param.begin() + to);
}

// Максимум на отрезке
double VCurveTempParams::GetMax(std::vector<double> & param, 
							   size_t from, size_t to)
{
	if (to <= from) return 0.0;
	return *std::max_element(param.begin() + from, param.begin() + to);
}

// Максимум по модулю на отрезке
double VCurveTempParams::GetMaxAbs(std::vector<double> & param, 
								  size_t from, size_t to, 
								  uint32_t & maxNdx)
{
	if (to <= from) return 0.0;
	std::vector<double>::iterator it = std::max_element(
		param.begin() + from, param.begin() + to, abs_comparator);
	
	maxNdx = it - param.begin();
	return *it;
}

// Среднее на отрезке
double VCurveTempParams::GetAvg(std::vector<double> & param, 
							   size_t from, size_t to)
{
	if (to <= from) return 0.0;
	return std::accumulate(param.begin() + from, param.begin() + to, 0.0) / (to - from);
}

// Сброс пересчитанных значений
void VCurveTempParams::Drop()
{
	this->wayCat = WAYCAT_UNSET;	
	this->anpHighBTRangeMid = 0;
	this->anpHighBTRangeMax = 0;
	this->anpHighRangeMid = 0;
	this->anpHighRangeMax = 0;
	this->anpPassRangeMid = 0;
	this->anpPassRangeMax = 0;
	this->anpTruckRangeMid = 0;
	this->anpTruckRangeMax = 0;
	this->psiHighRangeMid = 0;
	this->psiHighRangeMax = 0;
	this->psiPassRangeMid = 0;
	this->psiPassRangeMax = 0;
	this->psiTruckRangeMid = 0;
	this->psiTruckRangeMax = 0;
	this->baseMid = 0;
	this->baseTap = 0;
	
	this->ranges.clear();

	this->vprkPass.clear();				
	this->vprkTruck.clear();
	this->vprkThird.clear();
	this->vprkHigh.clear();
	this->vprkHighBT.clear();

	this->kurs.clear();
	this->midPlan.clear();
	this->midLevel.clear();
	this->radius.clear();

	this->anpPass.clear();
	this->anpTruck.clear();
	this->anpThird.clear();
	this->anpHigh.clear();
	this->anpHighBT.clear();

	this->vkrPass.clear();
	this->vkrTruck.clear();
	this->vkrHigh.clear();
	this->vkrHighBT.clear();

	this->psiPass.clear();
	this->psiTruck.clear();
	this->psiHigh.clear();
	this->psiHighBT.clear();
	this->vizPass.clear();
	this->vizTruck.clear();
	this->vizHigh.clear();

	this->tapPlan.clear();
	this->tapLevel.clear();
	this->vprPass.clear();
	this->vprTruck.clear();
	this->levelOffset.clear();
}


//------------------------------------- Методы класса VFindCurve ------------------------------------

/// Конструктор
VFindCurve::VFindCurve(double in_min_curv) : rcl(SD_LEFT, 1), rcr(SD_RIGHT, 1)
{};

/// Помещает данные на вход
void VFindCurve::Put(const VPoint& in_point)
{
	fel.Input(in_point, rcl);
	fer.Input(in_point, rcr);
	while (fel.Ready())
	{		
		output.push_back(VProtoCurve(fel.Front()));		
		fel.Pop();
	}
	while (fer.Ready())
	{		
		output.push_back(VProtoCurve(fer.Front()));		
		fer.Pop();
	}
};

/// Возвращает true, если есть обработанные данные
bool VFindCurve::Ready() const
{
	return output.size()>0;
};	

/// Возвращает true, если обработанных данных нет
bool VFindCurve::Empty() const
{
	return output.empty();
};

/// Возвращает первый элемент данных
const VProtoCurve& VFindCurve::Front() const
{
	return output.front();
};

/// Выталкивает первый элемент данных
void VFindCurve::Pop()
{
	output.pop_front();
};

//------------------------------------- Методы класса VFindTransition ------------------------------------

/// Конструктор
VFindTransition::VFindTransition(double in_min_value) : 
	rc_pos(pro::interval<double>(in_min_value, DBL_MAX)),
	rc_neg(pro::interval<double>(0-DBL_MAX, -in_min_value))
{};

/// Помещает данные на вход
void VFindTransition::Put(const VPoint& in_point)
{
	fe_pos.Input(in_point, rc_pos);
	fe_neg.Input(in_point, rc_neg);
	while (fe_pos.Ready())
	{		
		output.push_back(fe_pos.Front());		
		fe_pos.Pop();
	}
	while (fe_neg.Ready())
	{		
		output.push_back(fe_neg.Front());		
		fe_neg.Pop();
	}
};

/// Возвращает true, если есть обработанные данные
bool VFindTransition::Ready() const
{
	return output.size()>0;
};	

/// Возвращает true, если обработанных данных нет
bool VFindTransition::Empty() const
{
	return output.empty();
};

/// Возвращает первый элемент данных
const VExcess& VFindTransition::Front() const
{
	return output.front();
};

/// Выталкивает первый элемент данных
void VFindTransition::Pop()
{
	output.pop_front();
};

/// Сбрасываем остаток данных
void VFindTransition::Flush()
{
	fe_pos.Flush();
	fe_neg.Flush();
	while (fe_pos.Ready())
	{		
		output.push_back(fe_pos.Front());		
		fe_pos.Pop();
	}
	while (fe_neg.Ready())
	{		
		output.push_back(fe_neg.Front());		
		fe_neg.Pop();
	}	
}

// ---------------------------------------------------------------------
// параметры кривой
// ---------------------------------------------------------------------

// Экспортировать в формат Aprinfo
VAprinfoCurve VCurve::ExportToAprinfo(VDataSet & ds)
{
	VAprinfoCurve crv;
	crv.isThere = true;
	crv.base = this->GetRangeCoord(ds, true, false, true, COMPLETE_CURVE);
	crv.end = this->GetRangeCoord(ds, true, false, false, COMPLETE_CURVE);
	crv.levelBase = this->GetRangeCoord(ds, false, false, true, COMPLETE_CURVE);
	crv.levelEnd = this->GetRangeCoord(ds, false, false, false, COMPLETE_CURVE);
	crv.orientation = this->GetOrientation(ds);
	crv.angle = this->GetAngle(ds);

	crv.latStart = this->GetPlanMark(0).latitude;
	crv.longStart = this->GetPlanMark(0).longitude;
	crv.latEnd = this->GetPlanMark(this->GetMarkCount()-1).latitude;
	crv.longEnd = this->GetPlanMark(this->GetMarkCount()-1).longitude;

	for (size_t i = 0; i < this->GetMarkCount(); ++i)
	{
		VAprinfoCrvLabel mark;
		mark.coord = ds.GetAbsCoord().GetItem(this->GetPlanMark(i).index);
		mark.value = this->GetPlanMark(i).value;
		crv.planLabel.push_back(mark);
		mark.coord = ds.GetAbsCoord().GetItem(this->GetLevelMark(i).index);
		mark.value = this->GetLevelMark(i).value;
		crv.levelLabel.push_back(mark);
	}

	for (size_t i = 0; i < this->GetRadiusCount(); ++i)
	{
		VAprinfoCrvRadius radius;
		radius.radius = curv_to_radius(0.5 * (this->GetLevelMark(radius_start_mark(i)).value + this->GetLevelMark(radius_end_mark(i)).value));
		radius.level = 0.5 * (this->GetLevelMark(radius_start_mark(i)).value + this->GetLevelMark(radius_end_mark(i)).value);
		radius.latStart = GetPlanMark(radius_start_mark(i)).latitude;
		radius.longStart = GetPlanMark(radius_start_mark(i)).longitude;
		radius.latEnd = GetPlanMark(radius_end_mark(i)).latitude;
		radius.longEnd = GetPlanMark(radius_end_mark(i)).longitude;
		crv.radius.push_back(radius);
	}
	
	return crv;
}

// Вернуть описание статуса модели
const char * VCurve::GetModelStatusDescription() const
{
	if (this->model.status < 4)
	{
		return __status__[this->model.status];
	}
	else
	{
		return __status__[4];
	}
}

// Проверка числа меток
bool VCurve::CheckMarks() const
{
	size_t planMarkCount = this->planMark.size();
	size_t levelMarkCount = this->levelMark.size();

	// равное число меток
	if (planMarkCount != levelMarkCount)
		return false;

	// не менее 4 меток
	if (planMarkCount < 4) 
		return false;

	// четное число меток
	if (planMarkCount % 2) 
		return false;

	// метки упорядочены по возрастанию индексов
	for (size_t i = 1; i < planMarkCount; ++i)
	{
		if (planMark[i].index <= planMark[i-1].index) 
			return false;
	
		if (levelMark[i].index <= levelMark[i-1].index) 
			return false;
	}

	return true;
}

// апроксимация в точке по трапеции
double VCurve::AproxValue(size_t index, 
						  const std::vector<VCurveMark> & marks, 
						  const VAbsCoordData & absCoord) const
{
	if (!absCoord.IsLoaded() || index >= absCoord.GetCount() || marks.size() < 4)
	{
		return 0.0;
	}
	else if (index <= marks.front().index)
	{
		return marks.front().value;
	}
	else if (index >= marks.back().index)
	{
		return marks.back().value;
	}
	else
	{
		for (size_t i = 1; i < marks.size(); ++i)
		{
			if (index >= marks[i-1].index && index <= marks[i].index)
			{
				double len = absCoord.GetItem(marks[i].index) - absCoord.GetItem(marks[i-1].index);
				if (len == 0.0) return 0.5 * (marks[i-1].value + marks[i].value);

				double pos = absCoord.GetItem(index) - absCoord.GetItem(marks[i-1].index);
				double offset = (marks[i].value - marks[i-1].value) * (pos / len);
				return marks[i-1].value + offset;
			}
		}
		return 0.0f;
	}
}

// Ориентация кривой
long VCurve::GetOrientation(VDataSet & ds) const
{
	float sum = 0.0f;
	for (size_t i = 0; i < this->planMark.size(); ++i)
	{
		sum += planMark[i].value;
	}
	return (sum > 0.0f) ? SD_RIGHT : (sum < 0.0f) ? SD_LEFT : SD_UNDEFINED;
}

/// Знак кривизны/возвышения
double VCurve::GetSign(VDataSet & ds) const
{
	return GetOrientation(ds)==SD_LEFT ? -1 : 1;
}

// Категория пути, под которую расчитана кривая
long VCurve::GetWayCat(VDataSet & ds) const
{
	this->tempParams.SetWayCat(ds, this->IsModel());
	return this->tempParams.wayCat;
}

// Ссылка на метку ограничивающую кривую
size_t VCurve::GetRangeMark(uint32_t radius, bool circ, bool begin) const
{
	size_t markNdx;

	if (radius == COMPLETE_CURVE)
	{
		if (begin)
		{
			if (circ)	markNdx = 1;		
			else		markNdx = 0;
		}
		else 
		{
			if (circ)	markNdx = this->GetMarkCount() - 2;
			else		markNdx = this->GetMarkCount() - 1;
		}
	}
	// last radius 
	else if (radius == this->GetRadiusCount() - 1) 
	{
		if (begin)
		{
			if (circ)	markNdx = this->GetMarkCount() - 3;
			else		markNdx = this->GetMarkCount() - 4;
		}
		else
		{
			if (circ)	markNdx = this->GetMarkCount() - 2;
			else		markNdx = this->GetMarkCount() - 1;
		}
	}
	// other radius
	else 
	{
		if (begin)
		{
			if (circ)	markNdx = 2 * radius + 1;
			else		markNdx = 2 * radius;
		}
		else
			markNdx = 2 * radius + 2;
	}

	return markNdx;
}

// Абс.к-та границы кривой
double VCurve::GetRangeCoord(VDataSet & ds, 
							 bool plan, bool circ, bool begin, 
							 uint32_t radius) const
{
	if (!ds.GetAbsCoord().IsLoaded()) ds.Load(PRM_ABS_COORD);
	const std::vector<VCurveMark> * mark = plan ? &this->planMark : &this->levelMark;
	size_t markIdx = this->GetRangeMark(radius, circ, begin);
	return ds.GetAbsCoord().GetItem(mark->at(markIdx).index);
}

// Проверка попадания кривой в указанные границы (в абсолютных координатах)
bool VCurve::FitsRanges(VDataSet & ds, double from, double to) const
{
	double crv_from = std::min<double>(this->GetRangeCoord(ds, true, false, true),
		this->GetRangeCoord(ds, false, false, true));

	double crv_to = std::min<double>(this->GetRangeCoord(ds, true, false, false),
		this->GetRangeCoord(ds, false, false, false));

	return (crv_from >= from && crv_to <= to);
}

// Проверка попадания кривой в границы пути, согласно Апринфо
bool VCurve::FitsWayRanges(VDataSet & ds) const
{
	return this->FitsRanges(ds, 
		ds.GetAprinfo().GetAbsCoordBase(), 
		ds.GetAprinfo().GetAbsCoordEnd());
}

// ---------------------------------------------------------------------
// скорости на кривой
// ---------------------------------------------------------------------

// Список пассажирских скоростей по приказу на кривой
std::vector<VCurveSpdArea> VCurve::GetVprkPass(VDataSet & ds, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;
	this->tempParams.SetRanges(ds);
	this->tempParams.SetSpeeds(ds, this->IsModel());

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);
	idxFrom -= this->tempParams.ranges[0];
	idxTo -= this->tempParams.ranges[0];
	std::vector<VCurveTempParams::SpdArea> spdPrk = this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());

	for (size_t i = 0; i < spdPrk.size(); ++i)
	{
		VCurveSpdArea area;
		area.spd = spdPrk[i].spdPass;
		area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
		area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
		result.push_back(area);
	}

	return result;
}	

// Список грузовых скоростей по приказу на кривой
std::vector<VCurveSpdArea> VCurve::GetVprkTruck(VDataSet & ds, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;
	this->tempParams.SetRanges(ds);
	this->tempParams.SetSpeeds(ds, this->IsModel());

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);
	idxFrom -= this->tempParams.ranges[0];
	idxTo -= this->tempParams.ranges[0];

	std::vector<VCurveTempParams::SpdArea> spdPrk = this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());
	for (size_t i = 0; i < spdPrk.size(); ++i)
	{
		VCurveSpdArea area;
		area.spd = spdPrk[i].spdTruck;
		area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
		area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
		result.push_back(area);
	}

	return result;
}	

// Список третьих скоростей по приказу на кривой
std::vector<VCurveSpdArea> VCurve::GetVprkThird(VDataSet & ds, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;
	this->tempParams.SetRanges(ds);
	this->tempParams.SetSpeeds(ds, this->IsModel());

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);
	idxFrom -= this->tempParams.ranges[0];
	idxTo -= this->tempParams.ranges[0];

	std::vector<VCurveTempParams::SpdArea> spdPrk = this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());
	for (size_t i = 0; i < spdPrk.size(); ++i)
	{
		VCurveSpdArea area;
		area.spd = spdPrk[i].spdThird;
		area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
		area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
		result.push_back(area);
	}

	return result;
}	

// Список высоких скоростей на кривой
std::vector<VCurveSpdArea> VCurve::GetVprkHigh(VDataSet & ds, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;
	this->tempParams.SetRanges(ds);
	this->tempParams.SetSpeeds(ds, this->IsModel());

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);
	idxFrom -= this->tempParams.ranges[0];
	idxTo -= this->tempParams.ranges[0];

	std::vector<VCurveTempParams::SpdArea> spdPrk = this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());
	for (size_t i = 0; i < spdPrk.size(); ++i)
	{
		VCurveSpdArea area;
		area.spd = spdPrk[i].spdHigh;
		area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
		area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
		result.push_back(area);
	}

	return result;
}

// Список высоких скоростей на кривой [с наклоном кузова]
std::vector<VCurveSpdArea> VCurve::GetVprkHighBT(VDataSet & ds, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;
	this->tempParams.SetRanges(ds);
	this->tempParams.SetSpeeds(ds, this->IsModel());

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);
	idxFrom -= this->tempParams.ranges[0];
	idxTo -= this->tempParams.ranges[0];

	std::vector<VCurveTempParams::SpdArea> spdPrk = this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());
	for (size_t i = 0; i < spdPrk.size(); ++i)
	{
		VCurveSpdArea area;
		area.spd = spdPrk[i].spdHighBT;
		area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
		area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
		result.push_back(area);
	}

	return result;
}

// Наличие высоких скоростей
bool VCurve::HasVprkHigh(VDataSet & ds) const
{
	std::vector<VCurveSpdArea> area = this->GetVprkHigh(ds);
	if (area.empty()) return false;
	else
	{
		for (size_t i = 0; i < area.size(); ++i)
		{
			if (area[i].spd == NO_SPEED)
				return false;
		}
		return true;
	}
}

// Наличие высоких скоростей [с наклоном кузова]
bool VCurve::HasVprkHighBT(VDataSet & ds) const
{
	std::vector<VCurveSpdArea> area = this->GetVprkHighBT(ds);
	if (area.empty()) return false;
	else
	{
		for (size_t i = 0; i < area.size(); ++i)
		{
			if (area[i].spd == NO_SPEED) 
				return false;
		}
		return true;
	}
}

// список скоростей Vкр пассажирских
std::vector<VCurveSpdArea> VCurve::GetVkrPass(VDataSet & ds, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);

	if (this->IsModel())
	{
		double vkrMin = 350;
		for (size_t elemNdx = 0; elemNdx < this->GetElementCount(); ++elemNdx)
		{
			uint32_t curIdxFrom = this->planMark[elemNdx].index;
			uint32_t curIdxTo = this->planMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vkrMin = std::min<double>(vkrMin, this->GetElementVkrPass(ds, elemNdx));
			}

			curIdxFrom = this->levelMark[elemNdx].index;
			curIdxTo = this->levelMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vkrMin = std::min<double>(vkrMin, this->GetElementVkrPass(ds, elemNdx));
			}
		}

		VCurveSpdArea area;
		area.spd = vkrMin;
		area.begin = ds.GetAbsCoord().GetItem(idxFrom);
		area.end = ds.GetAbsCoord().GetItem(idxTo);
		result.push_back(area);
	}

	else
	{
		idxFrom -= this->tempParams.ranges[0];
		idxTo -= this->tempParams.ranges[0];

		this->tempParams.CalcVkr(ds, this->IsModel());
		std::vector<VCurveTempParams::SpdArea> spdPrk = 
			this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());

		for (size_t i = 0; i < spdPrk.size(); ++i)
		{
			VCurveSpdArea area;
			area.spd = (long) this->tempParams.GetMin(this->tempParams.vkrPass, spdPrk[i].idxFrom, spdPrk[i].idxTo);
			area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
			area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
			result.push_back(area);
		}
	}

	return result;
}

// список скоростей Vкр грузовых
std::vector<VCurveSpdArea> VCurve::GetVkrTruck(VDataSet & ds, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);

	if (this->IsModel())
	{
		double vkrMin = 350;
		for (size_t elemNdx = 0; elemNdx < this->GetElementCount(); ++elemNdx)
		{
			uint32_t curIdxFrom = this->planMark[elemNdx].index;
			uint32_t curIdxTo = this->planMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vkrMin = std::min<double>(vkrMin, this->GetElementVkrTruck(ds, elemNdx));
			}

			curIdxFrom = this->levelMark[elemNdx].index;
			curIdxTo = this->levelMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vkrMin = std::min<double>(vkrMin, this->GetElementVkrTruck(ds, elemNdx));
			}
		}

		VCurveSpdArea area;
		area.spd = vkrMin;
		area.begin = ds.GetAbsCoord().GetItem(idxFrom);
		area.end = ds.GetAbsCoord().GetItem(idxTo);
		result.push_back(area);
	}
	else
	{
		idxFrom -= this->tempParams.ranges[0];
		idxTo -= this->tempParams.ranges[0];

		this->tempParams.CalcVkr(ds, this->IsModel());
		std::vector<VCurveTempParams::SpdArea> spdPrk = 
			this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());

		for (size_t i = 0; i < spdPrk.size(); ++i)
		{
			VCurveSpdArea area;
			area.spd = (long) this->tempParams.GetMin(this->tempParams.vkrTruck, spdPrk[i].idxFrom, spdPrk[i].idxTo);
			area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
			area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
			result.push_back(area);
		}
	}
	return result;
}

// список скоростей Vкр [высокие скорости]
std::vector<VCurveSpdArea> VCurve::GetVkrHigh(VDataSet & ds, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);

	if (this->IsModel())
	{
		double vkrMin = 350;
		for (size_t elemNdx = 0; elemNdx < this->GetElementCount(); ++elemNdx)
		{
			uint32_t curIdxFrom = this->planMark[elemNdx].index;
			uint32_t curIdxTo = this->planMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vkrMin = std::min<double>(vkrMin, this->GetElementVkrHigh(ds, elemNdx));
			}

			curIdxFrom = this->levelMark[elemNdx].index;
			curIdxTo = this->levelMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vkrMin = std::min<double>(vkrMin, this->GetElementVkrHigh(ds, elemNdx));
			}
		}

		VCurveSpdArea area;
		area.spd = vkrMin;
		area.begin = ds.GetAbsCoord().GetItem(idxFrom);
		area.end = ds.GetAbsCoord().GetItem(idxTo);
		result.push_back(area);
	}
	
	else
	{
		idxFrom -= this->tempParams.ranges[0];
		idxTo -= this->tempParams.ranges[0];

		this->tempParams.CalcVkr(ds, this->IsModel());
		std::vector<VCurveTempParams::SpdArea> spdPrk = 
			this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());

		for (size_t i = 0; i < spdPrk.size(); ++i)
		{
			VCurveSpdArea area;
			area.spd = (long) this->tempParams.GetMin(this->tempParams.vkrHigh, spdPrk[i].idxFrom, spdPrk[i].idxTo);
			area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
			area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
			result.push_back(area);
		}
	}

	return result;
}

// список скоростей Vкр [высокие скорости|наклон кузова]
std::vector<VCurveSpdArea> VCurve::GetVkrHighBT(VDataSet & ds, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);

	if (this->IsModel())
	{
		double vkrMin = 350;
		for (size_t elemNdx = 0; elemNdx < this->GetElementCount(); ++elemNdx)
		{
			uint32_t curIdxFrom = this->planMark[elemNdx].index;
			uint32_t curIdxTo = this->planMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vkrMin = std::min<double>(vkrMin, this->GetElementVkrHighBT(ds, elemNdx));
			}

			curIdxFrom = this->levelMark[elemNdx].index;
			curIdxTo = this->levelMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vkrMin = std::min<double>(vkrMin, this->GetElementVkrHighBT(ds, elemNdx));
			}
		}

		VCurveSpdArea area;
		area.spd = vkrMin;
		area.begin = ds.GetAbsCoord().GetItem(idxFrom);
		area.end = ds.GetAbsCoord().GetItem(idxTo);
		result.push_back(area);
	}
	
	else
	{
		idxFrom -= this->tempParams.ranges[0];
		idxTo -= this->tempParams.ranges[0];

		this->tempParams.CalcVkr(ds, this->IsModel());
		std::vector<VCurveTempParams::SpdArea> spdPrk = 
			this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());

		for (size_t i = 0; i < spdPrk.size(); ++i)
		{
			VCurveSpdArea area;
			area.spd = (long) this->tempParams.GetMin(this->tempParams.vkrHighBT, spdPrk[i].idxFrom, spdPrk[i].idxTo);
			area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
			area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
			result.push_back(area);
		}
	}

	return result;
}

/// Скорость Vкр (средняя на круговой) под заданное ускорение
speed_t VCurve::GetMidVkr(VDataSet & ds, float anpRange, uint32_t radius) const
{
    return this->tempParams.GetMidVkr(ds, this->IsModel(), anpRange, radius);
}

// список скоростей Vиз 
std::vector<VCurveSpdArea> VCurve::GetVizPass(VDataSet & ds, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);

	if (this->IsModel())
	{
		double vizMin = 500;
		for (size_t elemNdx = 0; elemNdx < this->GetElementCount(); ++elemNdx)
		{
			uint32_t curIdxFrom = this->planMark[elemNdx].index;
			uint32_t curIdxTo = this->planMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vizMin = std::min<double>(vizMin, this->GetElementVizPass(ds, elemNdx));
			}

			curIdxFrom = this->levelMark[elemNdx].index;
			curIdxTo = this->levelMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vizMin = std::min<double>(vizMin, this->GetElementVizPass(ds, elemNdx));
			}
		}

		VCurveSpdArea area;
		area.spd = vizMin;
		area.begin = ds.GetAbsCoord().GetItem(idxFrom);
		area.end = ds.GetAbsCoord().GetItem(idxTo);
		result.push_back(area);
	}
	else
	{
		idxFrom -= this->tempParams.ranges[0];
		idxTo -= this->tempParams.ranges[0];

		this->tempParams.CalcPsiViz(ds, this->IsModel());
		std::vector<VCurveTempParams::SpdArea> spdPrk = this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());

		for (size_t i = 0; i < spdPrk.size(); ++i)
		{
			VCurveSpdArea area;
			area.spd = (long) this->tempParams.GetMin(this->tempParams.vizPass, spdPrk[i].idxFrom, spdPrk[i].idxTo);
			area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
			area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
			result.push_back(area);
		}
	}

	return result;
}

// список скоростей Vиз 
std::vector<VCurveSpdArea> VCurve::GetVizTruck(VDataSet & ds, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);

	if (this->IsModel())
	{
		double vizMin = 500;
		for (size_t elemNdx = 0; elemNdx < this->GetElementCount(); ++elemNdx)
		{
			uint32_t curIdxFrom = this->planMark[elemNdx].index;
			uint32_t curIdxTo = this->planMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vizMin = std::min<double>(vizMin, this->GetElementVizTruck(ds, elemNdx));
			}

			curIdxFrom = this->levelMark[elemNdx].index;
			curIdxTo = this->levelMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vizMin = std::min<double>(vizMin, this->GetElementVizTruck(ds, elemNdx));
			}
		}

		VCurveSpdArea area;
		area.spd = vizMin;
		area.begin = ds.GetAbsCoord().GetItem(idxFrom);
		area.end = ds.GetAbsCoord().GetItem(idxTo);
		result.push_back(area);
	}
	else
	{
		idxFrom -= this->tempParams.ranges[0];
		idxTo -= this->tempParams.ranges[0];

		this->tempParams.CalcPsiViz(ds, this->IsModel());
		std::vector<VCurveTempParams::SpdArea> spdPrk = this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());

		for (size_t i = 0; i < spdPrk.size(); ++i)
		{
			VCurveSpdArea area;
			area.spd = (long) this->tempParams.GetMin(this->tempParams.vizTruck, spdPrk[i].idxFrom, spdPrk[i].idxTo);
			area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
			area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
			result.push_back(area);
		}
	}

	return result;
}

// список скоростей Vиз [высокие скорости]
std::vector<VCurveSpdArea> VCurve::GetVizHigh(VDataSet & ds, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);

	if (this->IsModel())
	{
		double vizMin = 350;
		for (size_t elemNdx = 0; elemNdx < this->GetElementCount(); ++elemNdx)
		{
			uint32_t curIdxFrom = this->planMark[elemNdx].index;
			uint32_t curIdxTo = this->planMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vizMin = std::min<double>(vizMin, this->GetElementVizHigh(ds, elemNdx));
			}

			curIdxFrom = this->levelMark[elemNdx].index;
			curIdxTo = this->levelMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vizMin = std::min<double>(vizMin, this->GetElementVizHigh(ds, elemNdx));
			}
		}

		VCurveSpdArea area;
		area.spd = vizMin;
		area.begin = ds.GetAbsCoord().GetItem(idxFrom);
		area.end = ds.GetAbsCoord().GetItem(idxTo);
		result.push_back(area);
	}

	else
	{
		idxFrom -= this->tempParams.ranges[0];
		idxTo -= this->tempParams.ranges[0];

		this->tempParams.CalcPsiViz(ds, this->IsModel());
		std::vector<VCurveTempParams::SpdArea> spdPrk = this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());

		for (size_t i = 0; i < spdPrk.size(); ++i)
		{
			VCurveSpdArea area;
			area.spd = (long) this->tempParams.GetMin(this->tempParams.vizHigh, spdPrk[i].idxFrom, spdPrk[i].idxTo);
			area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
			area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
			result.push_back(area);
		}
	}

	return result;
}

// список скоростей Vиз [высокие скорости|наклон кузова]
std::vector<VCurveSpdArea> VCurve::GetVizHighBT(VDataSet & ds, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);

	if (this->IsModel())
	{
		double vizMin = 350;
		for (size_t elemNdx = 0; elemNdx < this->GetElementCount(); ++elemNdx)
		{
			uint32_t curIdxFrom = this->planMark[elemNdx].index;
			uint32_t curIdxTo = this->planMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vizMin = std::min<double>(vizMin, this->GetElementVizHighBT(ds, elemNdx));
			}

			curIdxFrom = this->levelMark[elemNdx].index;
			curIdxTo = this->levelMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vizMin = std::min<double>(vizMin, this->GetElementVizHighBT(ds, elemNdx));
			}
		}

		VCurveSpdArea area;
		area.spd = vizMin;
		area.begin = ds.GetAbsCoord().GetItem(idxFrom);
		area.end = ds.GetAbsCoord().GetItem(idxTo);
		result.push_back(area);
	}
	
	else
	{
		idxFrom -= this->tempParams.ranges[0];
		idxTo -= this->tempParams.ranges[0];

		this->tempParams.CalcPsiViz(ds, this->IsModel());
		std::vector<VCurveTempParams::SpdArea> spdPrk = this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());

		for (size_t i = 0; i < spdPrk.size(); ++i)
		{
			VCurveSpdArea area;
			area.spd = (long) this->tempParams.GetMin(this->tempParams.vizHighBT, spdPrk[i].idxFrom, spdPrk[i].idxTo);
			area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
			area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
			result.push_back(area);
		}
	}

	return result;
}

double VCurve::GetAnpPassRangeMax() const
{
   return this->tempParams.anpPassRangeMax;
}

double VCurve::GetAnpPassRangeMid() const
{
   return this->tempParams.anpPassRangeMid;
}

double VCurve::GetPsiPassRangeMax() const
{
   return this->tempParams.psiPassRangeMax;
}

double VCurve::GetPsiTruckRangeMax() const
{
   return this->tempParams.psiTruckRangeMax;
}

// список скоростей Vпр
std::vector<VCurveSpdArea> VCurve::GetVprPass(VDataSet & ds, bool isRepair, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);

	if (this->IsModel())
	{
		double vprMin = 350;
		for (size_t elemNdx = 0; elemNdx < this->GetElementCount(); ++elemNdx)
		{
			uint32_t curIdxFrom = this->planMark[elemNdx].index;
			uint32_t curIdxTo = this->planMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vprMin = std::min<double>(vprMin, this->GetElementVpr(ds, elemNdx));
			}

			curIdxFrom = this->levelMark[elemNdx].index;
			curIdxTo = this->levelMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vprMin = std::min<double>(vprMin, this->GetElementVpr(ds, elemNdx));
			}
		}

		VCurveSpdArea area;
		area.spd = vprMin;
		area.begin = ds.GetAbsCoord().GetItem(idxFrom);
		area.end = ds.GetAbsCoord().GetItem(idxTo);
		result.push_back(area);
	}
	else
	{
		this->tempParams.CalcVpr(ds, false);
		std::vector<double> & vpr = isRepair ? this->tempParams.vprPassRepair : this->tempParams.vprPass;

		idxFrom -= this->tempParams.ranges[0];
		idxTo -= this->tempParams.ranges[0];
		std::vector<VCurveTempParams::SpdArea> spdPrk = this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());

		for (size_t i = 0; i < spdPrk.size(); ++i)
		{
			VCurveSpdArea area;
			area.spd = (long) this->tempParams.GetMin(vpr, spdPrk[i].idxFrom, spdPrk[i].idxTo);
			area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
			area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
			result.push_back(area);
		}
	}

	return result;
}

// список скоростей Vпр
std::vector<VCurveSpdArea> VCurve::GetVprTruck(VDataSet & ds, bool isRepair, uint32_t radius) const
{
	std::vector<VCurveSpdArea> result;

	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);
	uint32_t idxFrom = std::min<uint32_t>(this->planMark[markFrom].index, this->levelMark[markFrom].index);
	uint32_t idxTo = std::max<uint32_t>(this->planMark[markTo].index, this->levelMark[markTo].index);

	if (this->IsModel())
	{
		double vprMin = 350;
		for (size_t elemNdx = 0; elemNdx < this->GetElementCount(); ++elemNdx)
		{
			uint32_t curIdxFrom = this->planMark[elemNdx].index;
			uint32_t curIdxTo = this->planMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vprMin = std::min<double>(vprMin, this->GetElementVpr(ds, elemNdx));
			}

			curIdxFrom = this->levelMark[elemNdx].index;
			curIdxTo = this->levelMark[elemNdx+1].index;

			if (!(idxTo < curIdxFrom || idxFrom > curIdxTo))
			{
				vprMin = std::min<double>(vprMin, this->GetElementVpr(ds, elemNdx));
			}
		}

		VCurveSpdArea area;
		area.spd = vprMin;
		area.begin = ds.GetAbsCoord().GetItem(idxFrom);
		area.end = ds.GetAbsCoord().GetItem(idxTo);
		result.push_back(area);
	}
	else
	{
		this->tempParams.CalcVpr(ds, false);
        std::vector<double> & vpr = isRepair ? this->tempParams.vprTruckRepair : this->tempParams.vprTruck;

		idxFrom -= this->tempParams.ranges[0];
		idxTo -= this->tempParams.ranges[0];
		std::vector<VCurveTempParams::SpdArea> spdPrk = this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());
        
		for (size_t i = 0; i < spdPrk.size(); ++i)
		{
			VCurveSpdArea area;
			area.spd = (long) this->tempParams.GetMin(vpr, spdPrk[i].idxFrom, spdPrk[i].idxTo);
			area.begin = ds.GetAbsCoord().GetItem(spdPrk[i].idxFrom + this->tempParams.ranges[0]);
			area.end = ds.GetAbsCoord().GetItem(spdPrk[i].idxTo + this->tempParams.ranges[0]);
			result.push_back(area);
		}
	}

	return result;
}

// ---------------------------------------------------------------------
// геометрия кривой: минимум/максимум/среднее
// ---------------------------------------------------------------------

// Средний радиус (на круговой кривой)
double VCurve::GetAvgRadius(VDataSet & ds, uint32_t radius) const
{
	if (radius < this->GetRadiusCount())
	{
		return this->GetElementRadius(radius_to_element(radius));
	}
	else
	{
		double angle = 0, len = 0;
		for (size_t i = 1; i < this->GetElementCount() - 1; ++i)
		{
			double cur_len = this->GetElementLength(ds, i);
			len += cur_len;

			if (this->GetElementType(i)) // круговая
			{
				angle += cur_len / this->GetElementRadius(i);
			}
			else // переходная
			{
				double cur_angle = 50000.0 / this->GetElementRadius(i-1) + 50000.0 / this->GetElementRadius(i+1);
				cur_angle *= cur_len / 100000.0;
				angle += cur_angle;
			}
		}

		if (angle != 0) 
		{
			return fabs(len) / fabs(angle);
		}
		else 
		{
			return 50000;
		}
	}	
}

// Минимальный радиус (на круговой кривой)
double VCurve::GetMinRadius(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetAbsCoord().IsLoaded()) ds.Load(PRM_ABS_COORD);
	this->tempParams.CalcRadius(ds, this->IsModel());

	uint32_t idxFrom = this->planMark[this->GetRangeMark(radius, true, true)].index;
	uint32_t idxTo = this->planMark[this->GetRangeMark(radius, true, false)].index;

	double avgRadius = this->GetAvgRadius(ds, radius);

	double avgRadiusCalc = this->tempParams.GetAvg(this->tempParams.radius, 
		idxFrom - this->tempParams.ranges[0], idxTo - this->tempParams.ranges[0]);

	double minRadiusCalc = this->tempParams.GetMin(this->tempParams.radius, 
		idxFrom - this->tempParams.ranges[0], idxTo - this->tempParams.ranges[0]);

	return avgRadius * (minRadiusCalc / avgRadiusCalc);
//	return avgRadius - (avgRadiusCalc - minRadiusCalc);
}

// Максимальный радиус (на круговой кривой)
double VCurve::GetMaxRadius(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetAbsCoord().IsLoaded()) ds.Load(PRM_ABS_COORD);
	this->tempParams.CalcRadius(ds, this->IsModel());

	uint32_t idxFrom = this->planMark[this->GetRangeMark(radius, true, true)].index;
	uint32_t idxTo = this->planMark[this->GetRangeMark(radius, true, false)].index;

	double avgRadius = this->GetAvgRadius(ds, radius);

	double maxRadiusCalc = this->tempParams.GetMax(this->tempParams.radius, 
		idxFrom - this->tempParams.ranges[0], idxTo - this->tempParams.ranges[0]);

	double avgRadiusCalc = this->tempParams.GetAvg(this->tempParams.radius,
		idxFrom - this->tempParams.ranges[0], idxTo - this->tempParams.ranges[0]);

	return avgRadius * (maxRadiusCalc / avgRadiusCalc);
//	return avgRadius + (maxRadiusCalc - avgRadiusCalc);

}


// Минимальное возвышение (на круговой кривой)
double VCurve::GetMinLevel(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetAbsCoord().IsLoaded()) ds.Load(PRM_ABS_COORD);
	this->tempParams.CalcMidLevel(ds, this->IsModel());

	uint32_t idxFrom = this->levelMark[this->GetRangeMark(radius, true, true)].index;
	uint32_t idxTo = this->levelMark[this->GetRangeMark(radius, true, false)].index;

	double value = this->tempParams.GetMin(this->tempParams.midLevel,
		idxFrom - this->tempParams.ranges[0], idxTo - this->tempParams.ranges[0]);

	return value;
}

// Среднее возвышение (на круговой кривой)
double VCurve::GetAvgLevel(VDataSet & ds, uint32_t radius) const
{	
	if (this->IsModel() && this->GetRadiusCount() == 1)
	{
		return this->GetElementLevel(1);
	}
	else if (this->IsModel() && radius < this->GetRadiusCount())
	{
		return this->GetElementLevel(2 * radius + 1);
	}

	else
	{
		if (!ds.GetAbsCoord().IsLoaded()) ds.Load(PRM_ABS_COORD);
		this->tempParams.CalcMidLevel(ds, this->IsModel());

		uint32_t idxFrom = this->levelMark[this->GetRangeMark(radius, true, true)].index;
		uint32_t idxTo = this->levelMark[this->GetRangeMark(radius, true, false)].index;

		double value = this->tempParams.GetAvg(this->tempParams.midLevel, 
			idxFrom - this->tempParams.ranges[0], idxTo - this->tempParams.ranges[0]);

		return value;
	}
}

// Среднее фактическое возвышение (на круговой кривой)
double VCurve::GetAvgFactLevel(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetAbsCoord().IsLoaded()) ds.Load(PRM_ABS_COORD);
	if (!ds.GetParameter(PRM_LEVEL).IsLoaded()) ds.Load(PRM_LEVEL);
	uint32_t idxFrom = this->levelMark[this->GetRangeMark(radius, true, true)].index;
	uint32_t idxTo = this->levelMark[this->GetRangeMark(radius, true, false)].index;

	double sum = 0, count = 0;
	double orientation = this->GetOrientation(ds) < 0.0 ? -1.0 : 1.0;
	
	if (ds.GetParameter(PRM_LEVEL).GetCount() > idxTo)
	{
		for (size_t idx = idxFrom; idx <= idxTo; ++idx)
		{
			sum += ds.GetParameter(PRM_LEVEL).GetItem(idx) * orientation;
			++count;
		}
	}

	if (count == 0) return 0.0;
	else return sum/count;
}

// Максимальное возвышение (на круговой кривой)
double VCurve::GetMaxLevel(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetAbsCoord().IsLoaded()) ds.Load(PRM_ABS_COORD);
	this->tempParams.CalcMidLevel(ds, this->IsModel());

	uint32_t idxFrom = this->levelMark[this->GetRangeMark(radius, true, true)].index;
	uint32_t idxTo = this->levelMark[this->GetRangeMark(radius, true, false)].index;

	double value = this->tempParams.GetMax(this->tempParams.midLevel, 
		idxFrom - this->tempParams.ranges[0], idxTo - this->tempParams.ranges[0]);

	return value;
}

// Минимальное ШК (на круговой кривой)
double VCurve::GetMinWidth(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_WIDTH).IsLoaded()) ds.Load(PRM_WIDTH);
	
	uint32_t idxFrom = this->levelMark[this->GetRangeMark(radius, true, true)].index;
	uint32_t idxTo = this->levelMark[this->GetRangeMark(radius, true, false)].index;

	VParameter * width = &ds.GetParameter(PRM_WIDTH);
	if (!width->IsLoaded()) return 0;

	double value = 2000.0;
	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (!width->IsCorrupt(i))
		{
			value = std::min<double>(value, width->GetItem(i));
		}
	}
	return value;
}

// Среднее ШК (на круговой кривой) 
double VCurve::GetAvgWidth(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_WIDTH).IsLoaded()) ds.Load(PRM_WIDTH);
	
	uint32_t idxFrom = this->levelMark[this->GetRangeMark(radius, true, true)].index;
	uint32_t idxTo = this->levelMark[this->GetRangeMark(radius, true, false)].index;

	VParameter * width = &ds.GetParameter(PRM_WIDTH);
	if (!width->IsLoaded()) return 0;

	double value = 0.0;
	size_t count = 0;

	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (!width->IsCorrupt(i))
		{
			value += width->GetItem(i);
			++count;
		}
	}
	if (count > 0)	return value/count;
	else return 0.0f;
}

// Максимальное ШК (на круговой кривой)
double VCurve::GetMaxWidth(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_WIDTH).IsLoaded()) ds.Load(PRM_WIDTH);
	
	uint32_t idxFrom = this->levelMark[this->GetRangeMark(radius, true, true)].index;
	uint32_t idxTo = this->levelMark[this->GetRangeMark(radius, true, false)].index;

	VParameter * width = &ds.GetParameter(PRM_WIDTH);
	if (!width->IsLoaded()) return 0;

	double value = 0.0;
	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (!width->IsCorrupt(i))
		{
			value = std::max<double>(value, width->GetItem(i));
		}
	}
	return value;
}

// Процент сбоев на ШК
double VCurve::GetWidthErrorPercent(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_WIDTH).IsLoaded()) ds.Load(PRM_WIDTH);
	
	uint32_t idxFrom = this->levelMark[this->GetRangeMark(radius, true, true)].index;
	uint32_t idxTo = this->levelMark[this->GetRangeMark(radius, true, false)].index;
	VParameter * width = &ds.GetParameter(PRM_WIDTH);
	if (!width->IsLoaded()) return 0;
	
	size_t errCount = 0;
	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (!width->IsLoaded() || width->IsCorrupt(i))
			++errCount;
	}
	
	if (idxTo <= idxFrom) return 0.0f;
	else return 100.0 * errCount / (idxTo - idxFrom);
}

// Отклонение ШК спереди от ШК - максимум
double VCurve::GetMaxDeltaWidthFront(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_WIDTH).IsLoaded()) ds.Load(PRM_WIDTH);
	if (!ds.GetParameter(PRM_WIDTH_FRONT).IsLoaded()) ds.Load(PRM_WIDTH_FRONT);

	VParameter * width = &ds.GetParameter(PRM_WIDTH);
	VParameter * widthFront = &ds.GetParameter(PRM_WIDTH_FRONT);
	if (!width->IsLoaded() || !widthFront->IsLoaded()) return 0;
	
	uint32_t idxFrom = this->planMark[this->GetRangeMark(radius, false, true)].index;
	uint32_t idxTo = this->planMark[this->GetRangeMark(radius, false, false)].index;

	double value = 0;
	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (!width->IsCorrupt(i) && !widthFront->IsCorrupt(i))
		{
			value = std::max<double>(value, fabs(width->GetItem(i) - widthFront->GetItem(i)));
		}
	}
	return value;
}

// Отклонение ШК спереди от ШК - среднее
double VCurve::GetAvgDeltaWidthFront(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_WIDTH).IsLoaded()) ds.Load(PRM_WIDTH);
	if (!ds.GetParameter(PRM_WIDTH_FRONT).IsLoaded()) ds.Load(PRM_WIDTH_FRONT);

	VParameter * width = &ds.GetParameter(PRM_WIDTH);
	VParameter * widthFront = &ds.GetParameter(PRM_WIDTH_FRONT);
	if (!width->IsLoaded() || !widthFront->IsLoaded()) return 0;

	uint32_t idxFrom = this->planMark[this->GetRangeMark(radius, false, true)].index;
	uint32_t idxTo = this->planMark[this->GetRangeMark(radius, false, false)].index;

	double value = 0;
	size_t count = 0;
	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (!width->IsCorrupt(i) && !widthFront->IsCorrupt(i))
		{
			value += fabs(width->GetItem(i) - widthFront->GetItem(i));
			++count;
		}
	}
	if (count > 0) value /= count;
	return value;
}

// Процент сбоев при расчете отклонения ШК спереди от ШК
double VCurve::GetDeltaWidthFrontErrorPercent(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_WIDTH).IsLoaded()) ds.Load(PRM_WIDTH);
	if (!ds.GetParameter(PRM_WIDTH_FRONT).IsLoaded()) ds.Load(PRM_WIDTH_BACK);

	if (!ds.GetParameter(PRM_WIDTH).IsLoaded() || 
		!ds.GetParameter(PRM_WIDTH_FRONT).IsLoaded() ||
		!ds.GetCorrupt().IsLoaded())
	{
		return 0;
	}

	uint32_t idxFrom = this->planMark[this->GetRangeMark(radius, false, true)].index;
	uint32_t idxTo = this->planMark[this->GetRangeMark(radius, false, false)].index;

	size_t errCount = 0;
	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (ds.GetParameter(PRM_WIDTH).IsCorrupt(i) || 
			ds.GetParameter(PRM_WIDTH_FRONT).IsCorrupt(i))
		{
			++errCount;
		}
	}

	if (idxTo <= idxFrom) return 0.0f;
	else return 100.0 * errCount / (idxTo - idxFrom);
}

// Отклонение ШК сзади от ШК - максимум
double VCurve::GetMaxDeltaWidthBack(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetCorrupt().IsLoaded()) ds.Load(PRM_CORRUPT);
	if (!ds.GetParameter(PRM_WIDTH).IsLoaded()) ds.Load(PRM_WIDTH);
	if (!ds.GetParameter(PRM_WIDTH_BACK).IsLoaded()) ds.Load(PRM_WIDTH_BACK);

	VParameter * width = &ds.GetParameter(PRM_WIDTH);
	VParameter * widthBack = &ds.GetParameter(PRM_WIDTH_BACK);
	if (!width->IsLoaded() || !widthBack->IsLoaded()) return 0;

	uint32_t idxFrom = this->planMark[this->GetRangeMark(radius, false, true)].index;
	uint32_t idxTo = this->planMark[this->GetRangeMark(radius, false, false)].index;

	double value = 0;
	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (!width->IsCorrupt(i) && !widthBack->IsCorrupt(i))
		{
			value = std::max<double>(value, fabs(width->GetItem(i) - widthBack->GetItem(i)));
		}
	}
	return value;
}

// Отклонение ШК сзади от ШК - среднее
double VCurve::GetAvgDeltaWidthBack(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_WIDTH).IsLoaded()) ds.Load(PRM_WIDTH);
	if (!ds.GetParameter(PRM_WIDTH_BACK).IsLoaded()) ds.Load(PRM_WIDTH_BACK);

	VParameter * width = &ds.GetParameter(PRM_WIDTH);
	VParameter * widthBack = &ds.GetParameter(PRM_WIDTH_BACK);
	if (!width->IsLoaded() || !widthBack->IsLoaded()) return 0;

	uint32_t idxFrom = this->planMark[this->GetRangeMark(radius, false, true)].index;
	uint32_t idxTo = this->planMark[this->GetRangeMark(radius, false, false)].index;

	double value = 0;
	size_t count = 0;
	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (!width->IsCorrupt(i) && !widthBack->IsCorrupt(i))
		{
			value += fabs(width->GetItem(i) - widthBack->GetItem(i));
			++count;
		}
	}
	if (count > 0) value /= count;
	return value;
}

// Процент сбоев при расчете отклонения ШК спереди от ШК
double VCurve::GetDeltaWidthBackErrorPercent(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_WIDTH).IsLoaded()) ds.Load(PRM_WIDTH);
	if (!ds.GetParameter(PRM_WIDTH_BACK).IsLoaded()) ds.Load(PRM_WIDTH_BACK);

	uint32_t idxFrom = this->planMark[this->GetRangeMark(radius, false, true)].index;
	uint32_t idxTo = this->planMark[this->GetRangeMark(radius, false, false)].index;

	if (!ds.GetParameter(PRM_WIDTH).IsLoaded() || 
		!ds.GetParameter(PRM_WIDTH_BACK).IsLoaded() ||
		!ds.GetCorrupt().IsLoaded())
	{
		return 0;
	}

	size_t errCount = 0;
	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (ds.GetParameter(PRM_WIDTH).IsCorrupt(i) || 
			ds.GetParameter(PRM_WIDTH_BACK).IsCorrupt(i))
		{
			++errCount;
		}
	}

	if (idxTo <= idxFrom) return 0.0f;
	else return 100.0f * errCount / (idxTo - idxFrom);
}

// Угол поворота кривой FixMe!!! лучше возвращать радианы или вообще angle_t
double VCurve::GetAngle(VDataSet & ds, uint32_t radius) const
{
	double val;
	uint32_t markFrom = this->GetRangeMark(radius, false, true);
	uint32_t markTo = this->GetRangeMark(radius, false, false);

	ds.Load(PRM_KURS);
	val = ds.GetParameter(PRM_KURS).GetItem(this->planMark.at(markTo).index) - 
		  ds.GetParameter(PRM_KURS).GetItem(this->planMark.at(markFrom).index);
	
	return val * 180.0 / M_PI;
}

/// вершина угла поворота
point_t VCurve::GetTurnPoint(VDataSet & ds) const
{
	if (PlanMarksValid())
	{		
		ds.Load(PRM_PLAN);	
		point_t start=ds.GetPlan().GetItem(this->planMark[0].index);
		point_t end=ds.GetPlan().GetItem(this->planMark[this->GetMarkCount()-1].index);
		line_t prev_line(start, ds.GetParameter(PRM_KURS).GetItem(this->planMark[0].index));
		line_t next_line(end, ds.GetParameter(PRM_KURS).GetItem(this->planMark[this->GetMarkCount()-1].index));
		return calc_cross_point(prev_line, next_line);
	}
	else
		return point_t(UNDEFINED_VALUE, UNDEFINED_VALUE);
}

// первый тангенс
double VCurve::GetTan1(VDataSet & ds) const
{
	if (PlanMarksValid())
	{		
		ds.Load(PRM_PLAN);
		point_t start=ds.GetPlan().GetItem(this->planMark[0].index);
		point_t cross=GetTurnPoint(ds);
		return length(start, cross);
	}
	else 
		return UNDEFINED_VALUE;
}

// второй тангенс
double VCurve::GetTan2(VDataSet & ds) const
{
	if (PlanMarksValid())
	{
		ds.Load(PRM_PLAN);
		point_t end=ds.GetPlan().GetItem(this->planMark[this->GetMarkCount()-1].index);
		point_t cross=GetTurnPoint(ds);
		return length(end, cross);
	}
	else 
		return UNDEFINED_VALUE;
}

// ---------------------------------------------------------------------
// характеристики: непогашенное, пси
// ---------------------------------------------------------------------

// максимум пассажирского непогашенного на кривой
double VCurve::GetMaxAnpPass(VDataSet & ds, double * abs, uint32_t radius) const
{
	this->tempParams.CalcAnp(ds, this->IsModel());
	uint32_t maxNdx;

	size_t markFrom = this->GetRangeMark(radius, false, true);
	size_t markTo = this->GetRangeMark(radius, false, false);

	double value = this->tempParams.GetMaxAbs(this->tempParams.anpPass, 
		this->planMark[markFrom].index - this->tempParams.ranges[0], 
		this->planMark[markTo].index - this->tempParams.ranges[0],
		maxNdx);

	if (abs != NULL)
	{
		*abs = ds.GetAbsCoord().GetItem(maxNdx + this->tempParams.ranges[0]);
	}

	return value;
}

// среднее пассажирское непогашенное на круговой кривой
double VCurve::GetAvgAnpPass(VDataSet & ds, uint32_t radius) const
{
	if (this->IsModel() && this->GetRadiusCount() == 1)
	{
		return this->GetElementAnpPass(ds, 1);
	}
	else if (this->IsModel() && radius < this->GetRadiusCount())
	{
		return this->GetElementAnpPass(ds, radius_to_element(radius));
	}
	else
	{
		this->tempParams.CalcAnp(ds, this->IsModel());

		size_t markFrom = this->GetRangeMark(radius, true, true);
		size_t markTo = this->GetRangeMark(radius, true, false);

		return this->tempParams.GetAvg(this->tempParams.anpPass, 
			this->planMark[markFrom].index - this->tempParams.ranges[0],
			this->planMark[markTo].index - this->tempParams.ranges[0]);
	}
}

// максимум грузового непогашенного на кривой
double VCurve::GetMaxAnpTruck(VDataSet & ds, uint32_t radius) const
{
	this->tempParams.CalcAnp(ds, this->IsModel());
	uint32_t maxNdx;

	size_t markFrom = this->GetRangeMark(radius, false, true);
	size_t markTo = this->GetRangeMark(radius, false, false);

	double value = this->tempParams.GetMaxAbs(this->tempParams.anpTruck, 
		this->planMark[markFrom].index - this->tempParams.ranges[0], 
		this->planMark[markTo].index - this->tempParams.ranges[0],
		maxNdx);

	return value;
}

// максимум грузового непогашенного на кривой
double VCurve::GetMaxAnpThird(VDataSet & ds, uint32_t radius) const
{
	this->tempParams.CalcAnp(ds, this->IsModel());
	uint32_t maxNdx;

	size_t markFrom = this->GetRangeMark(radius, false, true);
	size_t markTo = this->GetRangeMark(radius, false, false);

	double value = this->tempParams.GetMaxAbs(this->tempParams.anpThird, 
		this->planMark[markFrom].index - this->tempParams.ranges[0], 
		this->planMark[markTo].index - this->tempParams.ranges[0],
		maxNdx);

	return value;
}

// среднее грузовое непогашенное на круговой кривой
double VCurve::GetAvgAnpTruck(VDataSet & ds, uint32_t radius) const
{
	if (this->IsModel() && this->GetRadiusCount() == 1)
	{
		return this->GetElementAnpTruck(ds, 1);
	}
	else if (this->IsModel() && radius < this->GetRadiusCount())
	{
		return this->GetElementAnpTruck(ds, radius_to_element(radius));
	}
	else
	{
		this->tempParams.CalcAnp(ds, this->IsModel());

		size_t markFrom = this->GetRangeMark(radius, true, true);
		size_t markTo = this->GetRangeMark(radius, true, false);

		return this->tempParams.GetAvg(this->tempParams.anpTruck, 
			this->planMark[markFrom].index - this->tempParams.ranges[0],
			this->planMark[markTo].index - this->tempParams.ranges[0]);
	}
}

// среднее грузовое непогашенное на круговой кривой
double VCurve::GetAvgAnpThird(VDataSet & ds, uint32_t radius) const
{
	if (this->IsModel() && this->GetRadiusCount() == 1)
	{
		return this->GetElementAnpThird(ds, 1);
	}
	else if (this->IsModel() && radius < this->GetRadiusCount())
	{
		return this->GetElementAnpThird(ds, radius_to_element(radius));
	}
	else
	{
		this->tempParams.CalcAnp(ds, this->IsModel());

		size_t markFrom = this->GetRangeMark(radius, true, true);
		size_t markTo = this->GetRangeMark(radius, true, false);

		return this->tempParams.GetAvg(this->tempParams.anpThird,
			this->planMark[markFrom].index - this->tempParams.ranges[0],
			this->planMark[markTo].index - this->tempParams.ranges[0]);
	}
}

// максимум непогашенного [высокие скорости]
double VCurve::GetMaxAnpHigh(VDataSet & ds, double * abs, uint32_t radius) const
{
    this->tempParams.CalcAnp(ds, this->IsModel());
    uint32_t maxNdx;

    size_t markFrom = this->GetRangeMark(radius, false, true);
    size_t markTo = this->GetRangeMark(radius, false, false);

    double value = this->tempParams.GetMaxAbs(this->tempParams.anpHigh, 
        this->planMark[markFrom].index - this->tempParams.ranges[0], 
        this->planMark[markTo].index - this->tempParams.ranges[0],
        maxNdx);

    if (abs != NULL)
    {
        *abs = ds.GetAbsCoord().GetItem(maxNdx + this->tempParams.ranges[0]);
    }

    return value;
}

// среднее непогашенное [высокие скорости]
double VCurve::GetAvgAnpHigh(VDataSet & ds, uint32_t radius) const
{
    if (this->IsModel() && this->GetRadiusCount() == 1)
    {
        return this->GetElementAnpHigh(ds, 1);
    }
    else if (this->IsModel() && radius < this->GetRadiusCount())
    {
        return this->GetElementAnpHigh(ds, radius_to_element(radius));
    }   
    else    
    {
        this->tempParams.CalcAnp(ds, this->IsModel());

        size_t markFrom = this->GetRangeMark(radius, true, true);
        size_t markTo = this->GetRangeMark(radius, true, false);

        return this->tempParams.GetAvg(this->tempParams.anpHigh,
            this->planMark[markFrom].index - this->tempParams.ranges[0],
            this->planMark[markTo].index - this->tempParams.ranges[0]); 
	}
}

// максимум непогашенного [высокие скорости|наклон кузова]
double VCurve::GetMaxAnpHighBT(VDataSet & ds, double * abs, uint32_t radius) const
{
	this->tempParams.CalcAnp(ds, this->IsModel());
	uint32_t maxNdx;

	size_t markFrom = this->GetRangeMark(radius, false, true);
	size_t markTo = this->GetRangeMark(radius, false, false);

	double value = this->tempParams.GetMaxAbs(this->tempParams.anpHighBT, 
		this->planMark[markFrom].index - this->tempParams.ranges[0], 
		this->planMark[markTo].index - this->tempParams.ranges[0],
		maxNdx);

	if (abs != NULL)
	{
		*abs = ds.GetAbsCoord().GetItem(maxNdx + this->tempParams.ranges[0]);
	}

	return value;
}

// среднее непогашенное [высокие скорости|наклон кузова]
double VCurve::GetAvgAnpHighBT(VDataSet & ds, uint32_t radius) const
{
    if (this->IsModel() && this->GetRadiusCount() == 1)
    {
        return this->GetElementAnpHighBT(ds, 1);
    }
    else if (this->IsModel() && radius < this->GetRadiusCount())
    {
        return this->GetElementAnpHighBT(ds, radius_to_element(radius));
    }   
    else    
    {
        this->tempParams.CalcAnp(ds, this->IsModel());

        size_t markFrom = this->GetRangeMark(radius, true, true);
        size_t markTo = this->GetRangeMark(radius, true, false);

        return this->tempParams.GetAvg(this->tempParams.anpHighBT,
            this->planMark[markFrom].index - this->tempParams.ranges[0],
            this->planMark[markTo].index - this->tempParams.ranges[0]);
    }
}

// максимум пси (на всей кривой)
double VCurve::GetMaxPsiPass(VDataSet & ds, double * abs, uint32_t radius) const
{
	if (this->IsModel())
	{
		size_t first_elem, last_elem;
		if (radius == COMPLETE_CURVE)
		{
			first_elem = 0;
			last_elem = this->GetElementCount() - 1;
		}
		else
		{
			first_elem = 2 * radius;
			if (radius >= this->GetRadiusCount() - 1)
			{
				last_elem = this->GetElementCount() - 1;
			}
			else
			{
				last_elem = first_elem + 1;
			}
		}

		double value = 0;
		for (size_t i = first_elem ; i <= last_elem; ++i)
		{
			double cur_value = this->GetElementPsiPass(ds, i);
			if (fabs(cur_value) > fabs(value)) value = cur_value;
		}
		return value;
	}

	else
	{
		this->tempParams.CalcPsiViz(ds, this->IsModel());
		uint32_t maxNdx;

		size_t markFrom = this->GetRangeMark(radius, false, true);
		size_t markTo = this->GetRangeMark(radius, false, false);

		double value = this->tempParams.GetMaxAbs(this->tempParams.psiPass, 
			this->planMark[markFrom].index - this->tempParams.ranges[0], 
			this->planMark[markTo].index - this->tempParams.ranges[0],
			maxNdx);

		if (abs != NULL)
		{
			*abs = ds.GetAbsCoord().GetItem(maxNdx + this->tempParams.ranges[0]);
		}

		return value;
	}
}

// максимум пси (на всей кривой)
double VCurve::GetMaxPsiTruck(VDataSet & ds, double * abs, uint32_t radius) const
{
	if (this->IsModel())
	{
		size_t first_elem, last_elem;
		if (radius == COMPLETE_CURVE)
		{
			first_elem = 0;
			last_elem = this->GetElementCount() - 1;
		}
		else
		{
			first_elem = 2 * radius;
			if (radius >= this->GetRadiusCount() - 1)
			{
				last_elem = this->GetElementCount() - 1;
			}
			else
			{
				last_elem = first_elem + 1;
			}
		}

		double value = 0;
		for (size_t i = first_elem ; i <= last_elem; ++i)
		{
			double cur_value = this->GetElementPsiTruck(ds, i);
			if (fabs(cur_value) > fabs(value)) value = cur_value;
		}
		return value;
	}

	else
	{
        this->tempParams.CalcPsiViz(ds, this->IsModel());
        uint32_t maxNdx;

        size_t markFrom = this->GetRangeMark(radius, false, true);
        size_t markTo = this->GetRangeMark(radius, false, false);

        double value = this->tempParams.GetMaxAbs(this->tempParams.psiTruck, 
            this->planMark[markFrom].index - this->tempParams.ranges[0], 
            this->planMark[markTo].index - this->tempParams.ranges[0],
            maxNdx);

        if (abs != NULL)
        {
            *abs = ds.GetAbsCoord().GetItem(maxNdx + this->tempParams.ranges[0]);
        }

        return value;
	}
}

// максимум пси [высокие скорости]
double VCurve::GetMaxPsiHigh(VDataSet & ds, double * abs, uint32_t radius) const
{
	if (this->IsModel())
	{
		size_t first_elem, last_elem;
		if (radius == COMPLETE_CURVE)
		{
			first_elem = 0;
			last_elem = this->GetElementCount() - 1;
		}
		else
		{
			first_elem = 2 * radius;
			if (radius >= this->GetRadiusCount() - 1)
			{
				last_elem = this->GetElementCount() - 1;
			}
			else
			{
				last_elem = first_elem + 1;
			}
		}

		double value = 0;
		for (size_t i = first_elem ; i <= last_elem; ++i)
		{
			double cur_value = this->GetElementPsiHigh(ds, i);
			if (fabs(cur_value) > fabs(value)) value = cur_value;
		}
		return value;
	}

	else
	{
        this->tempParams.CalcPsiViz(ds, this->IsModel());
        uint32_t maxNdx;

        size_t markFrom = this->GetRangeMark(radius, false, true);
        size_t markTo = this->GetRangeMark(radius, false, false);

        double value = this->tempParams.GetMaxAbs(this->tempParams.psiHigh, 
            this->planMark[markFrom].index - this->tempParams.ranges[0], 
            this->planMark[markTo].index - this->tempParams.ranges[0],
            maxNdx);

        if (abs != NULL)
        {
            *abs = ds.GetAbsCoord().GetItem(maxNdx + this->tempParams.ranges[0]);
        }

        return value;
	}
}

// максимум пси [высокие скорости|наклон кузова]
double VCurve::GetMaxPsiHighBT(VDataSet & ds, double * abs, uint32_t radius) const
{
    if (this->IsModel())
    {
        size_t first_elem, last_elem;
        if (radius == COMPLETE_CURVE)
        {
            first_elem = 0;
            last_elem = this->GetElementCount() - 1;
        }
        else
        {
            first_elem = 2 * radius;
            if (radius >= this->GetRadiusCount() - 1)
            {
                last_elem = this->GetElementCount() - 1;
            }
            else
            {
                last_elem = first_elem + 1;
            }
        }

        double value = 0;
        for (size_t i = first_elem ; i <= last_elem; ++i)
        {
            double cur_value = this->GetElementPsiHighBT(ds, i);
            if (fabs(cur_value) > fabs(value)) value = cur_value;
        }
        return value;
    }

    else
    {
        this->tempParams.CalcPsiViz(ds, this->IsModel());
        uint32_t maxNdx;

        size_t markFrom = this->GetRangeMark(radius, false, true);
        size_t markTo = this->GetRangeMark(radius, false, false);

        double value = this->tempParams.GetMaxAbs(this->tempParams.psiHighBT, 
            this->planMark[markFrom].index - this->tempParams.ranges[0], 
            this->planMark[markTo].index - this->tempParams.ranges[0],
            maxNdx);

        if (abs != NULL)
        {
            *abs = ds.GetAbsCoord().GetItem(maxNdx + this->tempParams.ranges[0]);
        }

        return value;
    }
}

// скорость V+0.3
speed_t VCurve::GetVPlus03(VDataSet & ds, uint32_t radius) const
{
	double value = this->GetAvgRadius(ds, radius) * 13.0 * (this->GetAvgLevel(ds, radius) * 0.0061 + 0.3);
	return (speed_t) sqrt(value);
}

// скорость V-0.3
speed_t VCurve::GetVMinus03(VDataSet & ds, uint32_t radius) const
{
	double value = this->GetAvgRadius(ds, radius) * 13.0 * (this->GetAvgLevel(ds, radius) * 0.0061 - 0.3);
	if (value > 0.0)	return (speed_t) sqrt(value);
	else				return NO_SPEED;
}

// ---------------------------------------------------------------------
// характеристики: переходные кривые
// ---------------------------------------------------------------------

// отводы на переходной кривой
double VCurve::GetMaxPlanTap(VDataSet & ds, uint32_t num) const
{
	this->tempParams.CalcTapPlan(ds, this->IsModel());

	size_t idxFrom = this->planMark[2*num].index;
	size_t idxTo = this->planMark[2*num+1].index;
	double len = fabs(ds.GetAbsCoord().GetItem(idxTo) - ds.GetAbsCoord().GetItem(idxFrom));
	idxFrom -= this->tempParams.ranges[0];
	idxTo -= this->tempParams.ranges[0];

	double value = this->tempParams.GetMax(this->tempParams.tapPlan, idxFrom, idxTo);

	if (len < this->tempParams.baseTap)
	{
		value = std::max<double>(value, 
			fabs(this->tempParams.midPlan[idxFrom] - this->tempParams.midPlan[idxTo]) / len);
	}

	return value;
}

// отводы на переходной кривой
double VCurve::GetMaxLevelTap(VDataSet & ds, uint32_t num) const
{
	this->tempParams.CalcTapLevel(ds, this->IsModel());

	size_t idxFrom = this->levelMark[2*num].index;
	size_t idxTo = this->levelMark[2*num+1].index;
	double len = fabs(ds.GetAbsCoord().GetItem(idxTo) - ds.GetAbsCoord().GetItem(idxFrom));
	idxFrom -= this->tempParams.ranges[0];
	idxTo -= this->tempParams.ranges[0];

	double value = this->tempParams.GetMax(this->tempParams.tapLevel, idxFrom, idxTo);

	if (len < this->tempParams.baseTap)
	{
		value = std::max<double>(value, 
			fabs(this->tempParams.midLevel[idxFrom] - this->tempParams.midLevel[idxTo]) / len);
	}

	return value;
}

// отводы на переходной кривой
double VCurve::GetAvgPlanTap(VDataSet & ds, uint32_t num) const
{
	this->tempParams.CalcMidPlan(ds, this->IsModel());

	size_t from = this->planMark[2*num].index - this->tempParams.ranges[0];
	size_t to = this->planMark[2*num+1].index - this->tempParams.ranges[0];

	double value = this->tempParams.midPlan[from] - this->tempParams.midPlan[to];
	double len = ds.GetAbsCoord().GetItem(from + this->tempParams.ranges[0]);
	len -= ds.GetAbsCoord().GetItem(to + this->tempParams.ranges[0]);
	if (len != 0) value /= len;

	return fabs(value);
}

// отводы на переходной кривой
double VCurve::GetAvgLevelTap(VDataSet & ds, uint32_t num) const
{
	if(this->IsModel())
	{
		return this->GetElementTap(ds, num/2);
	}
	else
	{
		this->tempParams.CalcMidLevel(ds, this->IsModel());

		size_t from = this->levelMark[2*num].index - this->tempParams.ranges[0];
		size_t to = this->levelMark[2*num+1].index - this->tempParams.ranges[0];

		double value = this->tempParams.midLevel[from] - this->tempParams.midLevel[to];
		double len = ds.GetAbsCoord().GetItem(from + this->tempParams.ranges[0]);
		len -= ds.GetAbsCoord().GetItem(to + this->tempParams.ranges[0]);
		if (len != 0) value /= len;

		return fabs(value);
	}
}

// ---------------------------------------------------------------------
// характеристики: показатели расстройства
// ---------------------------------------------------------------------

// показатель расстройства в плане
double VCurve::GetPRPlan(VDataSet & ds) const
{
	double value = 0.0;

	for (size_t i = 0; i < this->GetRadiusCount(); ++i)
	{
		std::vector<VCurveSpdArea> spdPrk = this->GetVprkPass(ds, i);

		long spd = 120;
		if (!spdPrk.empty())
		{
			spd = spdPrk[0].spd;
			for (size_t spdNdx = 1; spdNdx < spdPrk.size(); ++spdNdx)
				spd = std::max<long>(spd, spdPrk[spdNdx].spd);
		}

		double Ravg = this->GetAvgRadius(ds, i);
		double Rmin = this->GetMinRadius(ds, i);
		double Rmax = this->GetMaxRadius(ds, i);

		double coeff = 0.0;
		if (Ravg <= 1200.0)							coeff = 1.0;
		else if (Ravg > 1200.0 && Ravg <= 3000.0)	coeff = 0.5;
		else if (Ravg > 3000.0 && spd <= 140)		coeff = 0.2;

		value = std::max<double>(value, coeff * (Rmax / Rmin - 1.0));
	}

	return value;
}

// показатель расстройства по уровню
double VCurve::GetPRLevel(VDataSet & ds) const
{
	double value = 0.0;

	for (size_t i = 0; i < this->GetRadiusCount(); ++i)
	{
		float minLevel = this->GetMinLevel(ds, i);
		if (fabs(minLevel) < 1.0) continue;
		
		value = std::max<double>(value, fabs(this->GetMaxLevel(ds, i) - minLevel) / 10.0);
	}

	return value;
}

// показатель расстройства по непогашенному
double VCurve::GetPRAnp(VDataSet & ds) const
{
	double value = 0.0;
	this->tempParams.CalcAnp(ds, this->IsModel());

	uint32_t markFrom = this->GetRangeMark(COMPLETE_CURVE, true, true);
	uint32_t markTo = this->GetRangeMark(COMPLETE_CURVE, true, false);
	uint32_t idxFrom = this->planMark[markFrom].index;
	uint32_t idxTo = this->planMark[markTo].index;
	idxFrom -= this->tempParams.ranges[0];
	idxTo -= this->tempParams.ranges[0];
	std::vector<VCurveTempParams::SpdArea> spdPrk = this->tempParams.SelectSpeeds(ds, idxFrom, idxTo, this->IsModel());

	for (size_t i = 0; i < spdPrk.size(); ++i)
	{
		double __max = this->tempParams.GetMax(this->tempParams.anpPass, spdPrk[i].idxFrom, spdPrk[i].idxTo);
		double __avg = this->tempParams.GetAvg(this->tempParams.anpPass, spdPrk[i].idxFrom, spdPrk[i].idxTo);

		double cur = this->tempParams.GetMax(this->tempParams.anpPass, spdPrk[i].idxFrom, spdPrk[i].idxTo);
		cur -= this->tempParams.GetAvg(this->tempParams.anpPass, spdPrk[i].idxFrom, spdPrk[i].idxTo);
		value = std::max<double>(value, cur / 0.7f);
	}

	if (spdPrk.size() > 0) 
		value /= spdPrk.size();

	return value;
}

// показатель расстройства по координатам
double VCurve::GetPRCoord(VDataSet & ds) const
{
	double value = 0.0;
	VAbsCoordData * absCoord = &ds.GetAbsCoord();

	for (size_t i = 0; i < this->GetMarkCount(); ++i)
	{
		double offset = absCoord->GetItem(this->GetPlanMark(i).index) - 
			absCoord->GetItem(this->GetLevelMark(i).index);

		value = std::max<double>(value, fabs(offset) / 20.0);
	}

	return value;
}

// ---------------------------------------------------------------------
// Характеристики модели
// ---------------------------------------------------------------------

// Максимальное отклонение факта от модели в плане
double VCurve::GetMaxPlanOffset(VDataSet & ds, uint32_t radius) const
{	
	ds.Load(IsModel() ? PRM_MODEL_PLAN_OFFSET : PRM_PLAN_OFFSET);
	size_t markFrom = this->GetRangeMark(radius, true, true);
	size_t markTo = this->GetRangeMark(radius, true, false);
	return ds.GetPlanOffset(IsModel() ? PRM_MODEL_PLAN_OFFSET : PRM_PLAN_OFFSET).GetMaxAbs(GetPlanMark(markFrom).index, 
		GetPlanMark(markTo).index);
}

// Среднее отклонение факта от модели в плане
double VCurve::GetAvgPlanOffset(VDataSet & ds, uint32_t radius) const
{	
	ds.Load(IsModel() ? PRM_MODEL_PLAN_OFFSET : PRM_PLAN_OFFSET);
	size_t markFrom = this->GetRangeMark(radius, true, true);
	size_t markTo = this->GetRangeMark(radius, true, false);
	return ds.GetPlanOffset(IsModel() ? PRM_MODEL_PLAN_OFFSET : PRM_PLAN_OFFSET).GetMO(GetPlanMark(markFrom).index, 
		GetPlanMark(markTo).index);
}

// Максимальное отклонение факта от модели на уровне
double VCurve::GetMaxLevelOffset(VDataSet & ds, uint32_t radius) const
{
	this->tempParams.CalcLevelOffset(ds);
	size_t markFrom = this->GetRangeMark(radius, false, true);
	size_t markTo = this->GetRangeMark(radius, false, false);
	return this->tempParams.GetMax(this->tempParams.levelOffset, 
		this->levelMark[markFrom].index - this->tempParams.ranges[0],
		this->levelMark[markTo].index - this->tempParams.ranges[0]);
}

// Среднее отклонение факта от модели на уровне
double VCurve::GetAvgLevelOffset(VDataSet & ds, uint32_t radius) const
{
	this->tempParams.CalcLevelOffset(ds);

	size_t markFrom = this->GetRangeMark(radius, false, true);
	size_t markTo = this->GetRangeMark(radius, false, false);

	return this->tempParams.GetAvg(this->tempParams.levelOffset, 
		this->levelMark[markFrom].index - this->tempParams.ranges[0],
		this->levelMark[markTo].index - this->tempParams.ranges[0]);
}

// ---------------------------------------------------------------------
// характеристики: попикетные характеристики
// ---------------------------------------------------------------------

// Список пикетов на кривой, со скоростями
std::vector<VCurvePicket> VCurve::GetPkList(VDataSet & ds, bool isRepair) const
{
	if (!ds.GetAbsCoord().IsLoaded()) ds.Load(PRM_ABS_COORD);
	if (!ds.GetAprinfo().IsLoaded()) ds.Load(PRM_APRINFO);
	VAbsCoordData * absCoord = &ds.GetAbsCoord();
	VAprinfo * aprInfo   = &ds.GetAprinfo();

	this->tempParams.CalcVkr(ds, this->IsModel());
	this->tempParams.CalcVpr(ds, this->IsModel());
	this->tempParams.CalcPsiViz(ds, this->IsModel());

	std::vector<VPicket> pks = ds.GetAprinfo().GetPkList(
		std::min<double>(this->GetRangeCoord(ds, true, false, true), this->GetRangeCoord(ds, false, false, true)),
		std::max<double>(this->GetRangeCoord(ds, true, false, false), this->GetRangeCoord(ds, false, false, false)), 
		true);

	std::vector<VCurvePicket> crvPks;

	for (size_t i = 0; i < pks.size(); ++i)
	{
		VCurvePicket crvPk(pks[i]);
		uint32_t idxFrom = absCoord->FindIndex(crvPk.absStart, true);
		uint32_t idxTo = absCoord->FindIndex(crvPk.absEnd, true);

		std::vector<VCurveTempParams::SpdArea> spdPrk = this->tempParams.SelectSpeeds(ds, 
			idxFrom - this->tempParams.ranges[0], 
			idxTo - this->tempParams.ranges[0], this->IsModel());

		for (size_t k = 0; k < spdPrk.size(); ++k)
		{
			speed_t vkrPass = (speed_t) this->tempParams.GetMin(this->tempParams.vkrPass, spdPrk[k].idxFrom, spdPrk[k].idxTo);
			speed_t vkrTruck = (speed_t) this->tempParams.GetMin(this->tempParams.vkrTruck, spdPrk[k].idxFrom, spdPrk[k].idxTo);
			speed_t vprPass = (speed_t) this->tempParams.GetMin(this->tempParams.vprPass, spdPrk[k].idxFrom, spdPrk[k].idxTo);
			speed_t vprTruck = (speed_t) this->tempParams.GetMin(this->tempParams.vprTruck, spdPrk[k].idxFrom, spdPrk[k].idxTo);
			speed_t vizPass = (speed_t) this->tempParams.GetMin(this->tempParams.vizPass, spdPrk[k].idxFrom, spdPrk[k].idxTo);
			speed_t vizTruck = (speed_t) this->tempParams.GetMin(this->tempParams.vizTruck, spdPrk[k].idxFrom, spdPrk[k].idxTo);

			crvPk.isVkrPass = crvPk.isVkrPass || (vkrPass < spdPrk[k].spdPass);
			crvPk.isVkrTruck = crvPk.isVkrTruck || (vkrTruck < spdPrk[k].spdTruck);
			crvPk.isVprPass = crvPk.isVprPass || (vprPass < spdPrk[k].spdPass);
			crvPk.isVprTruck = crvPk.isVprTruck || (vprTruck < spdPrk[k].spdTruck);
			crvPk.isVizPass = crvPk.isVizPass || (vizPass < spdPrk[k].spdPass);
			crvPk.isVizTruck = crvPk.isVizTruck || (vizTruck < spdPrk[k].spdTruck);
		}

		crvPks.push_back(crvPk);
	}

	return crvPks;
}

static bool __ApplyWearLimits(speed_t & spdPass, speed_t & spdTruck, float maxWear, float radius)
{
	bool ntd = false;

	if (maxWear > 18.0f) // 4deg
	{
		spdPass = 50;
		spdTruck = 50;
	}

	else if (maxWear > 16.0f) // 3deg
	{
		spdPass = 70;
		spdTruck = 70;
	}

	else if (maxWear > 15.0f && OPTIONS.GetOption(L"USE_NTD").Compare(OPTION_YES))
	{
		if (radius <= 350.0f)	
		{
			spdPass = 70;
			spdTruck = 60;
		}
		else
		{
			spdPass = 80;
			spdTruck = 70;
		}
		ntd = true;
	}

	else if (maxWear > 6.0f)
	{
		spdPass = 140;
		spdTruck = 140;
	}

	else if (maxWear > 4.0f)
	{
		spdPass = 200;
		spdTruck = 200;
	}

	return ntd;
}

// Список пикетов на кривой, со износами
std::vector<VCurveWearPicket> VCurve::GetWearPkList(VDataSet & ds) const
{
	if (!ds.GetAbsCoord().IsLoaded()) ds.Load(PRM_ABS_COORD);
	if (!ds.GetAprinfo().IsLoaded()) ds.Load(PRM_APRINFO);
	if (!ds.GetParameter(PRM_UNI_WEAR).IsLoaded()) ds.Load(PRM_UNI_WEAR);
	if (!ds.GetParameter(PRM_WIDTH).IsLoaded()) ds.Load(PRM_WIDTH);

	std::vector<VPicket> pks = ds.GetAprinfo().GetPkList(
		this->GetRangeCoord(ds, true, false, true), 
		this->GetRangeCoord(ds, true, false, false),
		true);

	std::vector<VCurveWearPicket> crvPks;

	for (size_t i = 0; i < pks.size(); ++i)
	{
		VCurveWearPicket crvPk(pks[i]);
		if (fabs(crvPk.absEnd - crvPk.absStart) < 20.0) continue;

		uint32_t idxFrom = ds.GetAbsCoord().FindIndex(crvPk.absStart, true);
		uint32_t idxTo = ds.GetAbsCoord().FindIndex(crvPk.absEnd, true);
		
		crvPk.prkPass = ds.GetAprinfo().GetMaxPassSpeed(crvPk.absStart, crvPk.absEnd);
		crvPk.prkTruck = ds.GetAprinfo().GetMaxTruckSpeed(crvPk.absStart, crvPk.absEnd);
       
        crvPk.maxWear = round(ds.GetParameter(PRM_UNI_WEAR).GetStatMax(idxFrom, idxTo), WEAR_PRECISION);
        crvPk.avgWear = round(ds.GetParameter(PRM_UNI_WEAR).GetStatMO(idxFrom, idxTo), WEAR_PRECISION);
        crvPk.errorWear = 100.0f * ds.GetParameter(PRM_UNI_WEAR).GetCorruptCount(idxFrom, idxTo) / (idxTo - idxFrom - 1);
		
        crvPk.maxWidth = ds.GetParameter(PRM_WIDTH).GetStatMax(idxFrom, idxTo);
        crvPk.avgWidth = ds.GetParameter(PRM_WIDTH).GetStatMO(idxFrom, idxTo);
        crvPk.errorWidth = 100.0f * ds.GetParameter(PRM_WIDTH).GetCorruptCount(idxFrom, idxTo) / (idxTo - idxFrom - 1);

		crvPk.ntd = __ApplyWearLimits(crvPk.spdPass, crvPk.spdTruck, crvPk.maxWear, this->GetAvgRadius(ds));
        crvPks.push_back(crvPk);
	}

	return crvPks;
}

// Диаграмма износов
std::set<density_t> VCurve::GetWearDiagram(VDataSet & ds) const
{
	if (!ds.GetAbsCoord().IsLoaded()) ds.Load(PRM_ABS_COORD);
	if (!ds.GetParameter(PRM_UNI_WEAR).IsLoaded()) ds.Load(PRM_UNI_WEAR);

	VAbsCoordData * absCoord = &ds.GetAbsCoord();
	VParameter * wear = &ds.GetParameter(PRM_UNI_WEAR);

	uint32_t idxFrom = this->planMark[this->GetRangeMark(COMPLETE_CURVE, false, true)].index;
	if (idxFrom > 0) --idxFrom;

	uint32_t idxTo = this->planMark[this->GetRangeMark(COMPLETE_CURVE, false, false)].index;
	if (idxTo < absCoord->GetCount()-1) ++idxTo;

	if (idxTo <= idxFrom) idxTo = idxFrom + 1;

	double avg_step = (absCoord->GetItem(idxTo) - absCoord->GetItem(idxFrom)) / (idxTo - idxFrom);
	if (avg_step < 0.05) avg_step = 0.25;

	std::set<density_t> dens = wear->GetDensity(idxFrom + 1, idxTo, 1.0);
	for (std::set<density_t>::iterator it = dens.begin(); it != dens.end(); it++)
	{
		density_t new_dens(*it);
		dens.erase(it);
		new_dens.value *= avg_step;
		dens.insert(new_dens);
	}

	return dens;
}

// Максимальный износ на кривой
double VCurve::GetMaxWear(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_UNI_WEAR).IsLoaded()) ds.Load(PRM_UNI_WEAR);
	
	uint32_t idxFrom = this->planMark[this->GetRangeMark(radius, false, true)].index;
	uint32_t idxTo = this->planMark[this->GetRangeMark(radius, false, false)].index;

	VParameter * wear = &ds.GetParameter(PRM_UNI_WEAR);

	if (!wear->IsLoaded())
		return 0;

	double value = 0.0;

	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (!wear->IsCorrupt(i))
		{
			value = std::max<double>(value, wear->GetItem(i));
		}
	}

	return round(value, WEAR_PRECISION);
}

// Средний износ на кривой
double VCurve::GetAvgWear(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_UNI_WEAR).IsLoaded()) ds.Load(PRM_UNI_WEAR);

	uint32_t idxFrom = this->planMark[this->GetRangeMark(radius, true, true)].index;
	uint32_t idxTo = this->planMark[this->GetRangeMark(radius, true, false)].index;

	VParameter * wear = &ds.GetParameter(PRM_UNI_WEAR);

	if (!wear->IsLoaded())
		return 0;

	double sum = 0.0;
	size_t count = 0;
	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (!wear->IsCorrupt(i))
		{
			sum += wear->GetItem(i);
			++count;
		}
	}
	if (count > 0) sum /= count;
	return round(sum, WEAR_PRECISION);
}

// СКО износа на кривой
double VCurve::GetSkoWear(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_UNI_WEAR).IsLoaded()) ds.Load(PRM_UNI_WEAR);

	uint32_t idxFrom = this->planMark[this->GetRangeMark(radius, true, true)].index;
	uint32_t idxTo = this->planMark[this->GetRangeMark(radius, true, false)].index;

	VParameter * wear = &ds.GetParameter(PRM_UNI_WEAR);

	if (!wear->IsLoaded())
		return 0;

	double sum = 0.0, avg = this->GetAvgWear(ds, radius), delta;
	size_t count = 0;
	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (!wear->IsCorrupt(i))
		{
			delta = wear->GetItem(i) - avg;
			sum += delta * delta;
			++count;
		}
	}

	if (count > 1) return sqrt(sum / (count-1));
	else return 0;
}

// Длина участка с превышением указанного порога
double VCurve::GetWearLenOverLimit(VDataSet & ds, float limit, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_UNI_WEAR).IsLoaded()) ds.Load(PRM_UNI_WEAR);
	if (!ds.GetAbsCoord().IsLoaded()) ds.Load(PRM_ABS_COORD);
	
	uint32_t idxFrom = std::min<uint32_t>(this->levelMark[this->GetRangeMark(radius, false, true)].index,
		this->planMark[this->GetRangeMark(radius, true, true)].index);

	uint32_t idxTo = std::max<uint32_t>(this->levelMark[this->GetRangeMark(radius, false, false)].index,
		this->planMark[this->GetRangeMark(radius, true, true)].index);

	VParameter * wear = &ds.GetParameter(PRM_UNI_WEAR);
	VAbsCoordData * absCoord = &ds.GetAbsCoord();

	if (!wear->IsLoaded())
		return 0;

	double value = 0.0;
	for (size_t i = idxFrom+1; i < idxTo; ++i)
	{
		if (wear->IsCorrupt(i)) continue;
		if (wear->GetItem(i) > limit)
		{
			value += absCoord->GetItem(i) - absCoord->GetItem(i-1);
		}
	}
	return value;
}

// Процент сбоев на кривой
double VCurve::GetWearErrorPercent(VDataSet & ds, uint32_t radius) const
{
	if (!ds.GetParameter(PRM_UNI_WEAR).IsLoaded()) ds.Load(PRM_UNI_WEAR);
	
	uint32_t idxFrom = std::min<uint32_t>(this->levelMark[this->GetRangeMark(radius, false, true)].index,
		this->planMark[this->GetRangeMark(radius, true, true)].index);

	uint32_t idxTo = std::max<uint32_t>(this->levelMark[this->GetRangeMark(radius, false, false)].index,
		this->planMark[this->GetRangeMark(radius, true, true)].index);

	VParameter * wear = &ds.GetParameter(PRM_UNI_WEAR);

	if (!wear->IsLoaded())
		return 100.0;

	size_t errCount = 0;
	for (size_t i = idxFrom; i < idxTo; ++i)
	{
		if (wear->IsCorrupt(i)) ++errCount;
	}
	return 100.0 * errCount / (idxTo - idxFrom);
}

// Список звеньев на кривой, с износами
std::vector<VCurveLink> VCurve::GetLinkList(VDataSet & ds) const
{
	std::vector<VCurveLink> links;

	this->tempParams.SetRanges(ds);
	this->tempParams.SetSpeeds(ds, this->IsModel());
	if (!ds.Load(PRM_CORRUPT)) return links;
	if (!ds.Load(PRM_UNI_WEAR)) return links;
	if (!ds.Load(PRM_WIDTH)) return links;
	if (!ds.GetZazorLeft().IsLoaded()) ds.Load(PRM_ZAZOR_LEFT);
	if (!ds.GetZazorRight().IsLoaded()) ds.Load(PRM_ZAZOR_RIGHT);

	VZazorList * zazor;
	if (this->GetOrientation(ds) == 1) zazor = &ds.GetZazorLeft();
	else zazor = &ds.GetZazorRight();

	// границы звеньев
	std::vector<size_t> waterMarks;
	waterMarks.push_back(this->tempParams.ranges[0]);
	if (zazor->IsLoaded())
	{
		for (size_t i = 0; i < zazor->GetCount(); ++i)
		{
			size_t index = zazor->GetItem(i).index;

			// за границами
			if (index <= this->tempParams.ranges[0] || index >= this->tempParams.ranges[1]) 
			{
				continue;
			}

			// шаг меньше 5 м
			if (ds.GetAbsCoord().GetItem(index) - 
				ds.GetAbsCoord().GetItem(waterMarks.back()) < 5.0)
			{
				continue;
			}
			
			// заносим точку зазора
			waterMarks.push_back(index);
		}
	}
	waterMarks.push_back(this->tempParams.ranges[1]);

	// добавляем промежуточные точки для звеньев длиной > 50 м
	size_t wmSize = waterMarks.size();
	for (size_t i = 1; i < wmSize; ++i)
	{
		double step = 
			ds.GetAbsCoord().GetItem(waterMarks[i]) - 
			ds.GetAbsCoord().GetItem(waterMarks[i-1]);

		while (step > 50.0)
		{
			step -= 25;
			waterMarks.push_back(ds.GetAbsCoord().FindIndex(
				ds.GetAbsCoord().GetItem(waterMarks[i]) - step, true));
		}
	}
	std::sort(waterMarks.begin(), waterMarks.end());

	// обсчет звеньев
	for (size_t i = 1; i < waterMarks.size(); ++i)
	{
		VCurveLink link;

		size_t idxFrom = waterMarks[i-1], idxTo = waterMarks[i];
		link.absStart = ds.GetAbsCoord().GetItem(idxFrom);
		link.absEnd = ds.GetAbsCoord().GetItem(idxTo);
		link.section.start = ds.GetAprinfo().GetRealCoord(link.absStart);
		link.section.end = ds.GetAprinfo().GetRealCoord(link.absEnd);
        
        link.maxWear = round(ds.GetParameter(PRM_UNI_WEAR).GetStatMax(idxFrom, idxTo), WEAR_PRECISION);
        link.avgWear = round(ds.GetParameter(PRM_UNI_WEAR).GetStatMO(idxFrom, idxTo), WEAR_PRECISION);
        link.errorWear = 100.0f * ds.GetParameter(PRM_UNI_WEAR).GetCorruptCount(idxFrom, idxTo) / (idxTo - idxFrom - 1);
		
        link.maxWidth = ds.GetParameter(PRM_WIDTH).GetStatMax(idxFrom, idxTo);
        link.avgWidth = ds.GetParameter(PRM_WIDTH).GetStatMO(idxFrom, idxTo);
        link.errorWidth = 100.0f * ds.GetParameter(PRM_WIDTH).GetCorruptCount(idxFrom, idxTo) / (idxTo - idxFrom - 1);

		link.ntd = __ApplyWearLimits(link.spdPass, link.spdTruck, link.maxWear, this->GetAvgRadius(ds));
		links.push_back(link);
	}
		
	return links;
}

// ---------------------------------------------------------------------
// Массивы данных на кривой
// ---------------------------------------------------------------------

// Скорость по приказу - пассажирская
const std::vector<double> & VCurve::GetArrayVprkPass(VDataSet & ds) const
{
	this->tempParams.SetSpeeds(ds, this->IsModel());
	return this->tempParams.vprkPass;
}

// Скорость по приказу - грузовая
const std::vector<double> & VCurve::GetArrayVprkTruck(VDataSet & ds) const
{
	this->tempParams.SetSpeeds(ds, this->IsModel());
	return this->tempParams.vprkTruck;
}

// Курс
const std::vector<double> & VCurve::GetArrayKurs(VDataSet & ds) const
{
	this->tempParams.CalcKurs(ds, this->isModel);
	return this->tempParams.kurs;
}

// Среднее кривизны
const std::vector<double> & VCurve::GetArrayMidPlan(VDataSet & ds) const
{
	this->tempParams.CalcMidPlan(ds, this->IsModel());
	return this->tempParams.midPlan;
}

// Среднее уровня
const std::vector<double> & VCurve::GetArrayMidLevel(VDataSet & ds) const
{
	this->tempParams.CalcMidLevel(ds, this->IsModel());
	return this->tempParams.midLevel;
}

// Непогашенное ускорение (скор.)
const  std::vector<double> & VCurve::GetArrayAnpHigh(VDataSet& ds) const
{
	this->tempParams.CalcAnp(ds, this->IsModel());
	return this->tempParams.anpHigh;
}

// Непогашенное ускорение (скор. НК)
const  std::vector<double> & VCurve::GetArrayAnpHighBT(VDataSet& ds) const
{
	this->tempParams.CalcAnp(ds, this->IsModel());
	return this->tempParams.anpHighBT;
}

// Непогашенное ускорение (пасс.)
const  std::vector<double> & VCurve::GetArrayAnpPass(VDataSet& ds) const
{
	this->tempParams.CalcAnp(ds, this->IsModel());
	return this->tempParams.anpPass;
}

// Непогашенное ускорение (груз.)
const  std::vector<double> & VCurve::GetArrayAnpTruck(VDataSet& ds) const
{
	this->tempParams.CalcAnp(ds, this->IsModel());
	return this->tempParams.anpTruck;
}

// Радиус
const std::vector<double> & VCurve::GetArrayRadius(VDataSet & ds) const
{
	this->tempParams.CalcRadius(ds, this->IsModel());
	return this->tempParams.radius;
}

// Скорость Vкр пассажирская
const std::vector<double> & VCurve::GetArrayVkrPass(VDataSet & ds) const
{
	this->tempParams.CalcVkr(ds, this->IsModel());
	return this->tempParams.vkrPass;
}

// Скорость Vкр грузовая
const std::vector<double> & VCurve::GetArrayVkrTruck(VDataSet & ds) const
{
	this->tempParams.CalcVkr(ds, this->IsModel());
	return this->tempParams.vkrTruck;
}

// Пси
const std::vector<double> & VCurve::GetArrayPsiPass(VDataSet & ds) const
{
	this->tempParams.CalcPsiViz(ds, this->IsModel());
	return this->tempParams.psiPass;
}

// Пси
const std::vector<double> & VCurve::GetArrayPsiTruck(VDataSet & ds) const
{
	this->tempParams.CalcPsiViz(ds, this->IsModel());
	return this->tempParams.psiTruck;
}

// Скорость Vиз
const std::vector<double> & VCurve::GetArrayVizPass(VDataSet & ds) const
{
	this->tempParams.CalcPsiViz(ds, this->IsModel());
	return this->tempParams.vizPass;
}

// Скорость Vиз
const std::vector<double> & VCurve::GetArrayVizTruck(VDataSet & ds) const
{
	this->tempParams.CalcPsiViz(ds, this->IsModel());
	return this->tempParams.vizTruck;
}

// Отвод кривизны
const std::vector<double> & VCurve::GetArrayTapPlan(VDataSet & ds) const
{
	this->tempParams.CalcTapPlan(ds, this->IsModel());
	return this->tempParams.tapPlan;
}

// Отвод уровня
const std::vector<double> & VCurve::GetArrayTapLevel(VDataSet & ds) const
{
	this->tempParams.CalcTapLevel(ds, this->IsModel());
	return this->tempParams.tapLevel;
}

// Скорость Vпр
const std::vector<double> & VCurve::GetArrayVprPass(VDataSet & ds, bool isRepair) const
{
	this->tempParams.CalcVpr(ds, this->IsModel());
	return isRepair ? this->tempParams.vprPassRepair : this->tempParams.vprPass;
}

std::vector<VCurveFPOArea> VCurve::GetFPOAreaList(VDataSet & ds) const
{
	std::vector<VCurveFPOArea> areas;

	this->tempParams.CalcAnp(ds, false);
	this->tempParams.CalcVkr(ds, false);
	this->tempParams.CalcPsiViz(ds, false);
	this->tempParams.CalcTapPlan(ds, false);
	this->tempParams.CalcVpr(ds, false);

	std::vector<VCurveSpdArea> speeds = this->GetVprkPass(ds);
	for (size_t i = 0; i < speeds.size(); ++i)
	{
		VCurveSpdArea spdArea = speeds[i];
	
		VCurveFPOArea fpoArea;
		::ZeroMemory(&fpoArea, sizeof(fpoArea));

		VWayCoord spdAreaBegin = ds.GetAprinfo().GetRealCoord(spdArea.begin);
		VWayCoord spdAreaEnd = ds.GetAprinfo().GetRealCoord(spdArea.end);

		for (int km = spdAreaBegin.km; km <= spdAreaEnd.km; ++km)
		{
			if (km == spdAreaBegin.km) fpoArea.absStart = spdArea.begin;
			else fpoArea.absStart = ds.GetAprinfo().GetAbsCoord(VWayCoord(km, 0));

			if (km == spdAreaEnd.km) 	fpoArea.absEnd = spdArea.end;
			else fpoArea.absEnd = ds.GetAprinfo().GetAbsCoord(VWayCoord(km+1, 0));

			size_t idxStart = ds.GetAbsCoord().FindIndex(fpoArea.absStart, true);
			size_t idxEnd = ds.GetAbsCoord().FindIndex(fpoArea.absEnd, true);

			for (size_t i = idxStart; i < idxEnd; ++i)
			{
				double absCur = ds.GetAbsCoord().GetItem(i);

				double anpPassCur = this->tempParams.anpPass[i - this->tempParams.ranges[0]];
				if (i == idxStart || fabs(anpPassCur) > fabs(fpoArea.maxAnp))
				{
					fpoArea.maxAnp = anpPassCur;
					fpoArea.maxAnpCoord = absCur;
				}

				double psiCur = this->tempParams.psiPass[i - this->tempParams.ranges[0]];
				if (i == idxStart || fabs(psiCur) > fabs(fpoArea.maxPsi))
				{
					fpoArea.maxPsi = psiCur;
					fpoArea.maxPsiCoord = absCur;
				}

				double ohCur = this->tempParams.tapLevel[i - this->tempParams.ranges[0]];
				if (i == idxStart || fabs(ohCur) > fabs(fpoArea.maxOh))
				{
					fpoArea.maxOh = ohCur;
					fpoArea.maxOhCoord = absCur;
				}

				long spdVkrCur = (long) this->tempParams.vkrPass[i - this->tempParams.ranges[0]];
				if (i == idxStart || spdVkrCur < fpoArea.spdVkr)
				{
					fpoArea.spdVkr = spdVkrCur;
				}

				long spdVprCur = (long) this->tempParams.vprPassRepair[i - this->tempParams.ranges[0]];
				if (i == idxStart || spdVprCur < fpoArea.spdVpr)
				{
					fpoArea.spdVpr = spdVprCur;
				}

				long spdVizCur = (long) this->tempParams.vizPass[i - this->tempParams.ranges[0]];
				if (i == idxStart || spdVizCur < fpoArea.spdViz)
				{
					fpoArea.spdViz = spdVizCur;
				}
			}

			fpoArea.maxAnpRange = this->tempParams.anpPassRangeMax;
			fpoArea.maxPsiRange = this->tempParams.psiPassRangeMax;

			fpoArea.maxAnpWayCoord = ds.GetAprinfo().GetRealCoord(fpoArea.maxAnpCoord);
			fpoArea.maxPsiWayCoord = ds.GetAprinfo().GetRealCoord(fpoArea.maxPsiCoord);
			fpoArea.maxOhWayCoord = ds.GetAprinfo().GetRealCoord(fpoArea.maxOhCoord);
			fpoArea.section.start = ds.GetAprinfo().GetRealCoord(fpoArea.absStart);
			fpoArea.section.end = ds.GetAprinfo().GetRealCoord(fpoArea.absEnd);
			fpoArea.spdVprkPass = spdArea.spd;
			areas.push_back(fpoArea);
		}
	}

	return areas;
}

//Функции поддержки разметки FixMe!!! эти функции можно сделать внешними

///Сброс временных параметров
void VCurve::Drop()
{
	this->tempParams.Drop();
}

///Возвращает площадь под трапецией
double VCurve::GetCrvArea(VDataSet & ds)
{
	double area=0;
	for (size_t i=1; i<GetMarkCount(); i++)
	{
		double l=ds.GetAbsCoord().GetItem(GetPlanMark(i).index)-ds.GetAbsCoord().GetItem(GetPlanMark(i-1).index);
		double h=(GetPlanMark(i).value+GetPlanMark(i-1).value)/2;
		area+=l*h;
	}
	return area/50000;
}

///Изменяет радиус кривой так чтобы площадь под трапецией соответствовала углу поворота
void VCurve::FixCrvArea(VDataSet & ds)
{
	//Радиус(ы) корректируется так, чтобы площадь трапеции соответствовала углу поворота кривой	
	double angle=GetAngle(ds)*M_PI/180.0;
	double area=GetCrvArea(ds);
	if (area==0)
		throw VSimpleException(L"Некорректная разметка кривой", L"Площадь под трапецией не может быть равна 0", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	double dS=angle/area;
	for (size_t i=1; i<GetMarkCount()-1; i++)
	{
		GetPlanMark(i).value*=static_cast<float>(dS);
	}
}

///перенос меток с кривизны на уровень
void VCurve::TransferMarksToLevel(VDataSet & ds)
{		
	levelMark.clear();
	for (size_t i=0; i<GetMarkCount(); ++i)
	{
		VCurveMark newMark=planMark.at(i);
		if (ds.AllowMarkHereLevel(newMark.index, isModel ? PRM_MODEL_CURVES : PRM_CURVE_MARKS))
		{
			newMark.value=ds.GetParameter(PRM_LEVEL).GetItem(newMark.index);
			levelMark.push_back(newMark);
		}
	}	
}

///Нормализация разметки по кривизне
void VCurve::NormalizePlanMarks(VDataSet & ds)
{
	if (PlanMarksValid())
	{
		//Первая и последняя метки по кривизне на 0, метки на круговых выравниваются
		for (size_t i=0; i<GetMarkCount(); ++i)
		{
			if (i==0 || i==GetMarkCount()-1)
				GetPlanMark(i).value=0;			
		}
		for (size_t i=1; i<GetMarkCount()-1; i+=2)
		{
			double avg=(GetPlanMark(i).value+GetPlanMark(i+1).value)/2;
			GetPlanMark(i).value=avg;
			GetPlanMark(i+1).value=avg;
		}
		FixCrvArea(ds);
	}
}

///Нормализация разметки по уровню
void VCurve::NormalizeLevelMarks(VDataSet & ds)
{
	//Первая и последняя метки по уровню на 0, метки на круговых выравниваются
	if (LevelMarksValid())
	{
		for (size_t i=0; i<GetLevelMarkCount(); ++i)
		{
			if (i==0 || i==GetLevelMarkCount()-1)
				GetLevelMark(i).value=0;
		}
		for (size_t i=1; i<GetLevelMarkCount()-1; i+=2)
		{
			double avg=(GetLevelMark(i).value+GetLevelMark(i+1).value)/2;
			GetLevelMark(i).value=fabs(avg)>OPTIONS.MIN_LEVEL() ? avg : 0;
			GetLevelMark(i+1).value=fabs(avg)>OPTIONS.MIN_LEVEL() ? avg : 0;
		}
	}
}

///Уточнение по методу наименьших квадратов для кривизны
void VCurve::MNKPlan(VDataSet & ds)
{
	if (PlanMarksValid())
	{
		ds.Load(PRM_MID_CURV);
		size_t max_index_diff=0;
		size_t max_index_diff_prev=ds.GetParameter(PRM_MID_CURV).GetCount();
		while (true)
		{
			//Определяем уравнения прямых
			std::vector<line_t> lines;
			lines.push_back(line_t(0,1,0));
			for (size_t i=0; i<GetMarkCount()-1; ++i)
			{		
				line_t line=ds.GetParameter(PRM_MID_CURV).GetLine(GetPlanMark(i).index, GetPlanMark(i+1).index);
				lines.push_back(line);											
			}
			lines.push_back(line_t(0,1,0));		
			//Находим точки пересечения прямых
			std::vector<point_t> points;
			for (size_t i=0; i<GetMarkCount(); ++i)
			{
				point_t point=calc_cross_point(lines.at(i), lines.at(i+1));
				points.push_back(point);			
			}
			//Проверяем пересечение точек с соседними кривыми
			for (size_t i=0; i<points.size(); ++i)
			{
				if (points.at(i).x>GetPlanRightMargin(ds))
					points.at(i).x=GetPlanRightMargin(ds);
				if (points.at(i).x<GetPlanLeftMargin(ds))
					points.at(i).x=GetPlanLeftMargin(ds);
			}
			//Проверяем расположены ли точки корректно (они должны быть упорядочены по возрастанию x)
			for (size_t i=1; i<points.size(); ++i)
			{
				//Ошибка 
				if (static_cast<size_t>(points.at(i-1).x) >= static_cast<size_t>(points.at(i).x))
					return;
			}
			//Определяем максмальную разность меток с предыдущим шагом
			size_t max_index_diff=0;
			for (size_t i=0; i<points.size(); ++i)
			{
				max_index_diff=std::max<size_t>(max_index_diff, static_cast<size_t>(fabs(GetPlanMark(i).index-points.at(i).x)));			
			}
			if (max_index_diff<max_index_diff_prev)
			{
				//Корректируем разметку		
				for (size_t i=0; i<points.size(); ++i)
				{			
					if (ds.AllowMarkHerePlan(static_cast<size_t>(points.at(i).x), isModel ? PRM_MODEL_CURVES : PRM_CURVE_MARKS))
					{
						GetPlanMark(i).index=static_cast<size_t>(points.at(i).x);			
						GetPlanMark(i).value=points.at(i).y;
					}
				}
			}
			else
			{
				break;
			}
			max_index_diff_prev=max_index_diff;
			if (max_index_diff<2)
			{
				break;
			}
		}
	}
}

///Уточнение по методу наименьших квадратов для уровня
void VCurve::MNKLevel(VDataSet & ds)
{
	if (LevelMarksValid() && LevelExist(ds))
	{
		ds.Load(PRM_MID_LEVEL);
		size_t max_index_diff=0;
		size_t max_index_diff_prev=ds.GetParameter(PRM_LEVEL).GetCount();
		while (true)
		{
			//Определяем уравнения прямых
			std::vector<line_t> lines;
			//Прямая перед кривой
			lines.push_back(line_t(0,1,0));//FixMe!!! Вместо 0 имеет смысл ставить норму возвышения
			//Переходные и круговые
			for (size_t i=0; i<GetLevelMarkCount()-1; ++i)
			{
				line_t line=ds.GetParameter(PRM_MID_LEVEL).GetLine(GetLevelMark(i).index, GetLevelMark(i+1).index);
				lines.push_back(line);
			}
			//Прямая после кривой
			lines.push_back(line_t(0,1,0));//FixMe!!! Вместо 0 имеет смысл ставить норму возвышения
			//Находим точки пересечения прямых
			std::vector<point_t> points;
			for (size_t i=0; i<GetLevelMarkCount(); ++i)
			{
				point_t point=calc_cross_point(lines.at(i), lines.at(i+1));
				points.push_back(point);			
			}
			//Проверяем пересечение точек с соседними кривыми
			for (size_t i=0; i<points.size(); ++i)
			{
				if (points.at(i).x>GetLevelRightMargin(ds))
					points.at(i).x=GetLevelRightMargin(ds);
				if (points.at(i).x<GetLevelLeftMargin(ds))
					points.at(i).x=GetLevelLeftMargin(ds);
			}
			//Проверяем расположены ли точки корректно (они должны быть упорядочены по возрастанию x)
			for (size_t i=1; i<points.size(); ++i)
			{
				//Ошибка 
				if ((points.at(i-1).x+1.0) >= points.at(i).x)//FixMe!!! вместо 1.0 можно подставлять минимальную длину переходной/круговой из настроек
					return;
			}
			//Определяем максмальную разность меток с предыдущим шагом
			size_t max_index_diff=0;
			for (size_t i=0; i<points.size(); ++i)
			{
				max_index_diff=std::max<size_t>(max_index_diff, static_cast<size_t>(fabs(GetLevelMark(i).index-points.at(i).x)));			
			}
			if (max_index_diff<max_index_diff_prev)
			{
				//Корректируем разметку		
				for (size_t i=0; i<points.size(); ++i)
				{		
					if (ds.AllowMarkHereLevel(static_cast<size_t>(points.at(i).x), isModel ? PRM_MODEL_CURVES : PRM_CURVE_MARKS))
					{	
						GetLevelMark(i).index=static_cast<size_t>(points.at(i).x);			
						GetLevelMark(i).value=points.at(i).y;
					}
				}
			}
			else
			{
				break;
			}
			max_index_diff_prev=max_index_diff;
			if (max_index_diff<2)
			{
				break;
			}
		}
	}
}

///Нормализация разметки
void VCurve::NormalizeMarks(VDataSet & ds)
{	
	//Нормализуем разметку на кривизне
	MNKPlan(ds);
	NormalizePlanMarks(ds);
	//При отсутствии меток на уровне, они переносятся с кривизны
	if (GetLevelMarkCount()==0)
	{
		TransferMarksToLevel(ds);		
	}
	//Нормализуем разметку на уровне
	MNKLevel(ds);	
	NormalizeLevelMarks(ds);			
};


///Оптимизация разметки
void VCurve::OptimizeMarks(VDataSet & ds)
{
	FixCrvArea(ds);
	VCriterionFunction criterion(this, &ds);
	VGaussSeidelMethod method(&criterion, 1, 1);
	//VFastDescentMethod method(&criterion, 80, 150, 10, 1e-8, 1.0, 1);
	//VGradientMethod method(&criterion, 50, 1e-8, 1.0, 1);			
	method.RunMethod();
	Drop();
}

///Приводит координаты меток на уровне в соответствие координатам меток на кривизне
void VCurve::FixLevelCoordinates(VDataSet & ds)
{
	if (GetMarkCount()==GetLevelMarkCount())
	{
		for (size_t i=0; i<GetMarkCount(); ++i)
		{
			if (ds.AllowMarkHereLevel(GetPlanMark(i).index, GetParameterType()))
				GetLevelMark(i).index=GetPlanMark(i).index;
		}
	}
}
	
///Проверяет корректность разметки по кривизне
bool VCurve::PlanMarksValid() const
{
	//Число меток должно быть больше 3 и четным
	return GetMarkCount()==0 || GetMarkCount()>3 && fmod(static_cast<double>(GetMarkCount()), 2)==0;
}

///Проверяет корректность разметки по уровню
bool VCurve::LevelMarksValid() const
{
	//Число меток должно быть больше 3 и четным
	return GetLevelMarkCount()==0 || GetLevelMarkCount()>3 && fmod(static_cast<double>(GetLevelMarkCount()), 2)==0;
}

///Проверяет есть ли на кривой возвышение
bool VCurve::LevelExist(VDataSet & ds) const
{
	if (LevelMarksValid())
	{
		double max_level=(GetOrientation(ds)==SD_LEFT) ? 
			ds.GetParameter(PRM_LEVEL).GetMin(GetLevelMark(0).index, GetLevelMark(GetLevelMarkCount()-1).index) :
			ds.GetParameter(PRM_LEVEL).GetMax(GetLevelMark(0).index, GetLevelMark(GetLevelMarkCount()-1).index); 
		return fabs(max_level) > OPTIONS.MIN_LEVEL();
	}
	else
		return false;
}

/// Приводит значения уровня к допустимым значениям
void VCurve::AdjustLevelValue()
{
	for (size_t i=0; i<GetLevelMarkCount(); ++i)
	{
		if (fabs(GetLevelMark(i).value)!=0 && fabs(GetLevelMark(i).value)<MIN_LEVEL_ALLOWED)
			GetLevelMark(i).value=sign(GetLevelMark(i).value)*MIN_LEVEL_ALLOWED;
		if (fabs(GetLevelMark(i).value)>MAX_LEVEL_ALLOWED)
			GetLevelMark(i).value=sign(GetLevelMark(i).value)*MAX_LEVEL_ALLOWED;
	}	
}

///Устанавливает возвышение на ноль
void VCurve::ZeroLevel()
{
	levelMark=planMark;
	for (size_t i=0; i<GetLevelMarkCount(); ++i)
	{
		GetLevelMark(i).value=0;
	}
}

///Увеличивает возвышение
void VCurve::IncreaseLevel(double in_step)
{
	if (LevelMarksValid())
	{
		for (size_t i=1; i<GetLevelMarkCount()-1; ++i)
		{
			GetLevelMark(i).value+=in_step;
		}
	}
}

///Уменьшает возвышение
void VCurve::DecreaseLevel(double in_step)
{
	if (LevelMarksValid())
	{
		for (size_t i=1; i<GetLevelMarkCount()-1; ++i)
		{
			GetLevelMark(i).value-=in_step;
		}
	}
}

///Возвращает левую допустимую границу для активной кривой на кривизне, ни одна метка не может быть поставлена левее этой границы
size_t VCurve::GetPlanLeftMargin(VDataSet & ds) const
{		
	if (ds.GetCurves(GetParameterType()).GetCount()>0 && (ds.GetCurves(GetParameterType()).GetActiveCurve()==0 || ds.GetCurves(GetParameterType()).GetActiveCurve()==BAD_INDEX))
	{
		return 0;//Начало файла
	}
	else
	{		
		size_t active_curve=ds.GetCurves(GetParameterType()).GetActiveCurve();
		if (active_curve==0)
			return 0;
		else
			return ds.GetCurves(GetParameterType()).GetItem(active_curve-1).GetPlanMark(ds.GetCurves(GetParameterType()).GetItem(active_curve-1).GetMarkCount()-1).index;
	}
}

///Возвращает правую допустимую границу для активной кривой на кривизне, ни одна метка не может быть поставлена правее этой границы
size_t VCurve::GetPlanRightMargin(VDataSet & ds) const
{
	if (ds.GetCurves(GetParameterType()).GetCount()>0 && (ds.GetCurves(GetParameterType()).GetActiveCurve()==ds.GetCurves(GetParameterType()).GetCount()-1 || ds.GetCurves(GetParameterType()).GetActiveCurve()==BAD_INDEX))
	{
		return ds.GetAbsCoord().GetCount()-1;//Конец файла
	}
	else
	{		
		size_t active_curve=ds.GetCurves(GetParameterType()).GetActiveCurve();
		if (active_curve==ds.GetCurves(GetParameterType()).GetCount()-1)
			return ds.GetAbsCoord().GetCount()-1;//Конец файла
		else
			return ds.GetCurves(GetParameterType()).GetItem(active_curve+1).GetPlanMark(0).index;
	}
}


///Возвращает левую допустимую границу для активной кривой на уровне, ни одна метка не может быть поставлена левее этой границы
size_t VCurve::GetLevelLeftMargin(VDataSet & ds) const
{
	if (ds.GetCurves(GetParameterType()).GetCount()>0 && (ds.GetCurves(GetParameterType()).GetActiveCurve()==0 || ds.GetCurves(GetParameterType()).GetActiveCurve()==BAD_INDEX))
	{
		return 0;//Начало файла
	}
	else
	{		
		return ds.GetCurves(GetParameterType()).GetItem(ds.GetCurves(GetParameterType()).GetActiveCurve()-1).GetLevelMark(ds.GetCurves(GetParameterType()).GetItem(ds.GetCurves(GetParameterType()).GetActiveCurve()-1).GetLevelMarkCount()-1).index;
	}
}

///Возвращает правую допустимую границу для активной кривой на уровне, ни одна метка не может быть поставлена правее этой границы
size_t VCurve::GetLevelRightMargin(VDataSet & ds) const
{
	if (ds.GetCurves(GetParameterType()).GetCount()>0 && (ds.GetCurves(GetParameterType()).GetActiveCurve()==ds.GetCurves(GetParameterType()).GetCount()-1 || ds.GetCurves(GetParameterType()).GetActiveCurve()==BAD_INDEX))
	{
		return ds.GetAbsCoord().GetCount()-1;//Конец файла
	}
	else
	{		
		return ds.GetCurves(GetParameterType()).GetItem(ds.GetCurves(GetParameterType()).GetActiveCurve()+1).GetLevelMark(0).index;
	}
}

///Возвращает левую допустимую границу для метки активной на кривизне, ни одна метка не может быть поставлена левее этой границы 
size_t VCurve::GetPlanMarkLeftMargin(VDataSet & ds, size_t in_mark_index) const
{
	if (in_mark_index==0)
		return GetPlanLeftMargin(ds);
	else
		//Необходимо учитывать минимальные длины переходных и круговых
		//Если номер метки нечетный, расстояние до левой метки есть длина переходной
		
		return fmod(float(in_mark_index), 2)!=0 ? 
			ds.GetAbsCoord().FindIndex(ds.GetAbsCoord().GetItem(GetPlanMark(in_mark_index-1).index)+OPTIONS.MIN_LENGTH_TRANS(), true) : 
			ds.GetAbsCoord().FindIndex(ds.GetAbsCoord().GetItem(GetPlanMark(in_mark_index-1).index)+OPTIONS.MIN_LENGTH_CURVE(), true);
}

///Возвращает правую допустимую границу для vtnrb активной кривой на кривизне, ни одна метка не может быть поставлена правее этой границы
size_t VCurve::GetPlanMarkRightMargin(VDataSet & ds, size_t in_mark_index) const
{
	if (in_mark_index+1==GetMarkCount())
		return GetPlanRightMargin(ds);
	else
		//Необходимо учитывать минимальные длины переходных и круговых
		//Если номер метки нечетный, расстояние до правой метки есть длина круговой
		return fmod(float(in_mark_index), 2)!=0 ? 
			ds.GetAbsCoord().FindIndex(ds.GetAbsCoord().GetItem(GetPlanMark(in_mark_index+1).index)-OPTIONS.MIN_LENGTH_CURVE(), true):
			ds.GetAbsCoord().FindIndex(ds.GetAbsCoord().GetItem(GetPlanMark(in_mark_index+1).index)-OPTIONS.MIN_LENGTH_TRANS(), true);
}

///Возвращает левую допустимую границу для метки активной на уровне, ни одна метка не может быть поставлена левее этой границы 
size_t VCurve::GetLevelMarkLeftMargin(VDataSet & ds, size_t in_mark_index) const
{
	if (in_mark_index==0)
		return GetLevelLeftMargin(ds);
	else
		//Необходимо учитывать минимальные длины переходных и круговых
		//Если номер метки нечетный, расстояние до левой метки есть длина переходной
		
		return fmod(float(in_mark_index), 2)!=0 ? 
			ds.GetAbsCoord().FindIndex(ds.GetAbsCoord().GetItem(GetLevelMark(in_mark_index-1).index)+OPTIONS.MIN_LENGTH_TRANS(), true) : 
			ds.GetAbsCoord().FindIndex(ds.GetAbsCoord().GetItem(GetLevelMark(in_mark_index-1).index)+OPTIONS.MIN_LENGTH_CURVE(), true);
}

///Возвращает правую допустимую границу для метки активной кривой на уровне, ни одна метка не может быть поставлена правее этой границы
size_t VCurve::GetLevelMarkRightMargin(VDataSet & ds, size_t in_mark_index) const
{
	if (in_mark_index+1==GetLevelMarkCount())
		return GetLevelRightMargin(ds);
	else
		//Необходимо учитывать минимальные длины переходных и круговых
		//Если номер метки нечетный, расстояние до правой метки есть длина круговой
		return fmod(float(in_mark_index), 2)!=0 ? 
			ds.GetAbsCoord().FindIndex(ds.GetAbsCoord().GetItem(GetLevelMark(in_mark_index+1).index)-OPTIONS.MIN_LENGTH_CURVE(), true):
			ds.GetAbsCoord().FindIndex(ds.GetAbsCoord().GetItem(GetLevelMark(in_mark_index+1).index)-OPTIONS.MIN_LENGTH_TRANS(), true);
}

///Автоматическая разметка кривой
void VCurve::Automark(VDataSet& ds, size_t in_start, size_t in_end, bool in_model)
{	
	planMark.clear();
	SetModel(in_model);						
	//Ищем переходные		
	VFindTransition fe(OPTIONS.CURVE_DERIVED());
	ds.Load(PRM_MID_CURV);
	for (size_t i=in_start; i<=in_end; ++i)
	{					
		fe.Put(VPoint(i, ds.GetParameter(PRM_MID_CURV).GetDerivate(i, ds.GetAbsCoord(),OPTIONS.CURVE_BASE())));		
	}		
	fe.Flush();
	// Минимальная разница кривизны на разных радиусах
	float min_curv=std::min<double>(OPTIONS.MIN_CURVE(), fabs(ds.GetParameter(PRM_MID_CURV).GetMO(in_start, in_end)/2.0));
	while (fe.Ready())
	{
		VExcess e=fe.Front();
		//Добавляем переходную
		size_t trans_start=e.GetStart();
		size_t trans_end=e.GetEnd();
		double trans_start_value=ds.GetParameter(PRM_MID_CURV).GetItem(trans_start);
		double trans_end_value=ds.GetParameter(PRM_MID_CURV).GetItem(trans_end);
		double trans_length=ds.GetAbsCoord().GetItem(trans_end)-ds.GetAbsCoord().GetItem(trans_start);
		if (trans_length>OPTIONS.MIN_LENGTH_TRANS() && fabs(trans_end_value - trans_start_value)>min_curv)
		{
			if (ds.AllowMarkHerePlan(trans_start, GetParameterType()))
				AddPlanMark(trans_start, trans_start_value);
			if (ds.AllowMarkHerePlan(trans_end, GetParameterType()))
				AddPlanMark(trans_end, trans_end_value);
		}
		fe.Pop();
	}
	//Переносим метки на уровень
	TransferMarksToLevel(ds);		
}

///Автоматическая разметка кривой (только один радиус)
void VCurve::Automark1(VDataSet& ds, size_t in_start, size_t in_end, bool in_model)
{	
	ds.Load(PRM_MID_CURV);

	planMark.clear();

	SetModel(in_model);				
	if (ds.AllowMarkHerePlan(in_start, GetParameterType()))
		AddPlanMark(in_start, ds.GetParameter(PRM_MID_CURV).GetItem(in_start));

	if (ds.AllowMarkHerePlan(in_start+(in_end-in_start)/4, GetParameterType()))
		AddPlanMark(in_start+(in_end-in_start)/4, ds.GetParameter(PRM_MID_CURV).GetItem(in_start+(in_end-in_start)/4));

	if (ds.AllowMarkHerePlan(in_start+3*(in_end-in_start)/4, GetParameterType()))
		AddPlanMark(in_start+3*(in_end-in_start)/4, ds.GetParameter(PRM_MID_CURV).GetItem(in_start+2*(in_end-in_start)/4));

	if (ds.AllowMarkHerePlan(in_end, GetParameterType()))
		AddPlanMark(in_end, ds.GetParameter(PRM_MID_CURV).GetItem(in_end));

	//Переносим метки на уровень
	TransferMarksToLevel(ds);
}

///Возвращает true если элемент - круговая
bool VCurve::GetElementType(size_t in_index) const
{
	//Круговые - нечетные
	return fmod(static_cast<double>(in_index), 2)!=0;
}

///Возвращает длину элемента
double VCurve::GetElementLength(VDataSet & ds, size_t in_index) const
{
	double element_start_x = ds.GetAbsCoord().GetItem(GetPlanMark(in_index).index);
	double element_end_x = ds.GetAbsCoord().GetItem(GetPlanMark(in_index+1).index);
	return element_end_x - element_start_x;
}

///Возвращает радиус элемента
double VCurve::GetElementRadius(size_t in_index) const
{	
	double element_start_y = GetPlanMark(in_index).value;
	double element_end_y = GetPlanMark(in_index+1).value;
	return fabs(curv_to_radius((element_start_y + element_end_y)/2));
}

///Возвращает длину элемента (уровень)
double VCurve::GetElementLengthLevel(VDataSet & ds, size_t in_index) const
{
	double element_start_x = ds.GetAbsCoord().GetItem(GetLevelMark(in_index).index);
	double element_end_x = ds.GetAbsCoord().GetItem(GetLevelMark(in_index+1).index);
	return element_end_x - element_start_x;
}

///Возвращает возвышение элемента
double VCurve::GetElementLevel(size_t in_index) const
{
	double element_start_y = GetLevelMark(in_index).value;
	double element_end_y = GetLevelMark(in_index+1).value;
	return fabs((element_start_y + element_end_y)/2);
}

///Рассчитывает и возвращает массив из 4 непогашенных ускорений для элемента
const std::vector<VAnp> VCurve::GetElementAnp(VDataSet & ds, size_t in_index, int in_speed) const
{
	std::vector<VAnp> result;
	
	if (in_speed!=NO_SPEED)
	{
	
		//На элементе имеем четыре точки в которых считаем непогашенное ускорение		
		//На кривизне
		size_t i1=GetPlanMark(in_index).index;
		float c1=GetPlanMark(in_index).value;
		float l1=AproxLevel(i1, ds.GetAbsCoord());
		float a1=GetSign(ds)*calc_anp(in_speed, c1, l1);
		result.push_back(VAnp(i1, a1));

		
		size_t i2=GetPlanMark(in_index+1).index;
		float c2=GetPlanMark(in_index+1).value;
		float l2=AproxLevel(i2, ds.GetAbsCoord());
		float a2=GetSign(ds)*calc_anp(in_speed, c2, l2);
		result.push_back(VAnp(i2, a2));
		
		//На уровне
		size_t i3=GetLevelMark(in_index).index;
		float c3=AproxPlan(i3, ds.GetAbsCoord());
		float l3=GetLevelMark(in_index).value;
		float a3=GetSign(ds)*calc_anp(in_speed, c3, l3);
		result.push_back(VAnp(i3, a3));
		
		size_t i4=GetLevelMark(in_index+1).index;			
		float c4=AproxPlan(i4, ds.GetAbsCoord());
		float l4=GetLevelMark(in_index+1).value;
		float a4=GetSign(ds)*calc_anp(in_speed, c4, l4);
		result.push_back(VAnp(i4, a4));

		std::sort(result.begin(), result.end());
	}
	return result;
}

///Рассчитывает и возвращает максимум скорости нарастания непогашенного ускорения для элемента
float VCurve::GetElementPsi(VDataSet & ds, size_t in_index, int in_speed) const
{
	//Получаем Анп 
	std::vector<VAnp> anp = GetElementAnp(ds, in_index, in_speed);
	std::vector<float> psi;
	for (size_t i=1; i<anp.size(); ++i)
	{		
		double start=ds.GetAbsCoord().GetItem(anp.at(i-1).index);
		double end=ds.GetAbsCoord().GetItem(anp.at(i).index);
		if (end-start>0)			
			psi.push_back(calc_psi(in_speed, anp.at(i-1).value, anp.at(i).value, start, end));
	}
	if (psi.size()>0)
		return *std::max_element(psi.begin(), psi.end());
	else
		return 0;
}

///Возвращает отвод возвышения
float VCurve::GetElementTap(VDataSet & ds, size_t in_index) const
{	
	double x1=ds.GetAbsCoord().GetItem(GetLevelMark(in_index).index);
	double x2=ds.GetAbsCoord().GetItem(GetLevelMark(in_index+1).index);
	double y1=GetLevelMark(in_index).value;
	double y2=GetLevelMark(in_index+1).value;
	return fabs(y2-y1)/(x2-x1);
}

///Возвращает Vкр для элемента
double VCurve::GetElementVkrHighBT(VDataSet & ds, size_t in_index) const
{
	for (int V=MAX_SPEED;V>0;V-=SPEED_QUANTUM)
	{		
		std::vector<VAnp> anp=GetElementAnp(ds, in_index, V);
		bool bad=false;
		for (size_t i=0; i<anp.size(); ++i)
		{
			bad = bad || VModelAnpHighCondition().TestValue(anp.at(i).value);
		}
		if (!bad)
			return V;		
	}
	return 0;
}

///Возвращает Vкр для элемента
double VCurve::GetElementVkrHigh(VDataSet & ds, size_t in_index) const
{
	for (int V=MAX_SPEED;V>0;V-=SPEED_QUANTUM)
	{		
		std::vector<VAnp> anp=GetElementAnp(ds, in_index, V);
		bool bad=false;
		for (size_t i=0; i<anp.size(); ++i)
		{
			bad = bad || VModelAnpHighCondition().TestValue(anp.at(i).value);
		}
		if (!bad)
			return V;		
	}
	return 0;
}

///Возвращает Vкр для элемента
double VCurve::GetElementVkrPass(VDataSet & ds, size_t in_index) const
{
	for (int V=MAX_SPEED;V>0;V-=SPEED_QUANTUM)
	{		
		std::vector<VAnp> anp=GetElementAnp(ds, in_index, V);
		bool bad=false;
		for (size_t i=0; i<anp.size(); ++i)
		{
			bad = bad || VModelAnpPassCondition().TestValue(anp.at(i).value);
		}
		if (!bad)
			return V;		
	}
	return 0;
}

///Возвращает Vкр для элемента
double VCurve::GetElementVkrTruck(VDataSet & ds, size_t in_index) const
{
	for (int V=MAX_SPEED;V>0;V-=SPEED_QUANTUM)
	{		
		std::vector<VAnp> anp=GetElementAnp(ds, in_index, V);
		bool bad=false;
		for (size_t i=0; i<anp.size(); ++i)
		{
			bad = bad || VModelAnpPassCondition().TestValue(anp.at(i).value);
		}
		if (!bad)
			return V;		
	}
	return 0;
}

///Возвращает Vпр для элемента
double VCurve::GetElementVpr(VDataSet & ds, size_t in_index) const
{
	return calc_vpr(GetElementTap(ds, in_index), false).GetPassSpeed();	
}

///Возвращает Vиз для элемента
double VCurve::GetElementVizHighBT(VDataSet & ds, size_t in_index) const
{
	for (int V=300;V>0;V-=5)
	{		
		float psi=GetElementPsi(ds, in_index, V);
		if (!VModelPsiHighCondition().TestValue(psi))
		{
			return V;
		}
	}	
	return 0;	
}

///Возвращает Vиз для элемента
double VCurve::GetElementVizHigh(VDataSet & ds, size_t in_index) const
{
	for (int V=300;V>0;V-=5)
	{		
		float psi=GetElementPsi(ds, in_index, V);
		if (!VModelPsiHighCondition().TestValue(psi))
		{
			return V;
		}
	}	
	return 0;	
}

///Возвращает Vиз для элемента
double VCurve::GetElementVizPass(VDataSet & ds, size_t in_index) const
{
	for (int V=300;V>0;V-=5)
	{		
		float psi=GetElementPsi(ds, in_index, V);
		if (!VModelPsiPassCondition().TestValue(psi))
		{
			return V;
		}
	}	
	return 0;	
}

///Возвращает Vиз для элемента
double VCurve::GetElementVizTruck(VDataSet & ds, size_t in_index) const
{
	for (int V=300;V>0;V-=5)
	{		
		float psi=GetElementPsi(ds, in_index, V);
		if (!VModelPsiTruckCondition().TestValue(psi))
		{
			return V;
		}
	}	
	return 0;	
}

///Возвращает Анп для элемента
double VCurve::GetElementAnpHighBT(VDataSet & ds, size_t in_index) const
{	
	double V=GetModelVhigh_bt();
	std::vector<VAnp> anp=GetElementAnp(ds, in_index, V);
    if (anp.empty()) return 0;
	return  std::max<float>(
			std::max<float>(anp.at(0).value, anp.at(1).value),
		    std::max<float>(anp.at(2).value, anp.at(3).value));
}

///Возвращает Анп для элемента
double VCurve::GetElementAnpHigh(VDataSet & ds, size_t in_index) const
{	
	double V=GetModelVhigh();
	std::vector<VAnp> anp=GetElementAnp(ds, in_index, V);
    if (anp.empty()) return 0;
	return  std::max<float>(
			std::max<float>(anp.at(0).value, anp.at(1).value),
		    std::max<float>(anp.at(2).value, anp.at(3).value));
}

///Возвращает Анп для элемента
double VCurve::GetElementAnpPass(VDataSet & ds, size_t in_index) const
{	
	double V=GetModelVpass();
	std::vector<VAnp> anp=GetElementAnp(ds, in_index, V);
    if (anp.empty()) return 0;
	return  std::max<float>(
			std::max<float>(anp.at(0).value, anp.at(1).value),
		    std::max<float>(anp.at(2).value, anp.at(3).value));
}

///Возвращает Анп груз. для элемента
double VCurve::GetElementAnpTruck(VDataSet & ds, size_t in_index) const
{
	double V=GetModelVcrg();
	std::vector<VAnp> anp=GetElementAnp(ds, in_index, V);
    if (anp.empty()) return 0;
	return  std::max<float>(
			std::max<float>(anp.at(0).value, anp.at(1).value),
		    std::max<float>(anp.at(2).value, anp.at(3).value));
}

///Возвращает Анп средн. для элемента
double VCurve::GetElementAnpThird(VDataSet & ds, size_t in_index) const
{
	double V=GetModelVavg();
	std::vector<VAnp> anp=GetElementAnp(ds, in_index, V);
    if (anp.empty()) return 0;
	return  std::max<float>(
			std::max<float>(anp.at(0).value, anp.at(1).value),
		    std::max<float>(anp.at(2).value, anp.at(3).value));
}

///Возвращает Пси снк
float VCurve::GetElementPsiHighBT(VDataSet & ds, size_t in_index) const
{
	return GetElementPsi(ds, in_index, GetModelVhigh_bt());
}

///Возвращает Пси ск
float VCurve::GetElementPsiHigh(VDataSet & ds, size_t in_index) const
{
	return GetElementPsi(ds, in_index, GetModelVhigh());
}

/** Возвращает Пси пасс*/
float VCurve::GetElementPsiPass(VDataSet & ds, size_t in_index) const
{
	return GetElementPsi(ds, in_index, GetModelVpass());
}

/** Возвращает Пси груз*/
float VCurve::GetElementPsiTruck(VDataSet & ds, size_t in_index) const
{
	return GetElementPsi(ds, in_index, GetModelVcrg());
}

/** Возвращает Пси ср.*/
float VCurve::GetElementPsiThird(VDataSet & ds, size_t in_index) const
{
	return GetElementPsi(ds, in_index, GetModelVavg());
}


///Рассчитывает и возвращает массив непогашенных ускорений
const std::vector<VAnp>& VCurve::GetElementAnp(const VAbsCoordData & in_abscoord, const VAprinfo& in_aprinfo, 
	speed_type in_speed, bool in_use_passport_speeds) const
{
	modelAnp.clear();
	//Точки на кривизне
	for (size_t j=0; j<GetMarkCount(); ++j)
	{
		size_t index=GetPlanMark(j).index;
		float curv=GetPlanMark(j).value;
		float level=AproxLevel(index, in_abscoord);
		long speed;
		switch (in_speed)
		{
			default:
			case pass_prk:
				if (in_use_passport_speeds)
					speed=GetModelVpass();
				else
					speed=in_aprinfo.GetPassSpeed(in_abscoord.GetItem(index));
				break;
			case crg_prk:
				if (in_use_passport_speeds)
					speed=GetModelVavg();
				else
					speed=in_aprinfo.GetTruckSpeed(in_abscoord.GetItem(index));
				break;
			case high_prk:
				if (in_use_passport_speeds)
					speed=GetModelVhigh();
				else
					speed=in_aprinfo.GetHighSpeed(in_abscoord.GetItem(index));
				break;
			case high_bt_prk:
				if (in_use_passport_speeds)
					speed=GetModelVhigh_bt();
				else
					speed=in_aprinfo.GetHighSpeedBT(in_abscoord.GetItem(index));
				break;
		}
		if (speed!=NO_SPEED)
		{
			VAnp anp;
			anp.index=index;
			anp.value=calc_anp(speed, curv, level);
			modelAnp.push_back(anp);
		}
	}
	//Точки на уровне
	for (size_t j=0; j<GetLevelMarkCount(); ++j)
	{
		size_t index=GetLevelMark(j).index;			
		float curv=AproxPlan(index, in_abscoord);
		float level=GetLevelMark(j).value;
		long speed;
		switch (in_speed)
		{
			default:
			case pass_prk:
				if (in_use_passport_speeds)
					speed=GetModelVpass();
				else
					speed=in_aprinfo.GetPassSpeed(in_abscoord.GetItem(index));
				break;
			case crg_prk:
				if (in_use_passport_speeds)
					speed=GetModelVavg();
				else
					speed=in_aprinfo.GetTruckSpeed(in_abscoord.GetItem(index));
				break;
			case high_prk:
				if (in_use_passport_speeds)
					speed=GetModelVhigh();
				else
					speed=in_aprinfo.GetHighSpeed(in_abscoord.GetItem(index));
				break;
			case high_bt_prk:
				if (in_use_passport_speeds)
					speed=GetModelVhigh_bt();
				else
					speed=in_aprinfo.GetHighSpeedBT(in_abscoord.GetItem(index));
				break;
		} 
		if (speed!=NO_SPEED)
		{
			VAnp anp;
			anp.index=index;
			anp.value=calc_anp(speed, curv, level);
			modelAnp.push_back(anp);
		}
	}
	std::sort(modelAnp.begin(), modelAnp.end());
	return modelAnp;
}

///Рассчитывает оптимальное возвышение
void VCurve::CreateLevelOptimal(VDataSet & ds)
{
	if (PlanMarksValid() && LevelMarksValid())
	{
		//Выравниваем координаты
		FixLevelCoordinates(ds);		
		//Определяем возвышение
		for (int i=1;i<GetLevelMarkCount()-1;i+=2)
		{		
			double A=0;
			double R=curv_to_radius(fabs(GetPlanMark(i).value));
			//Оптимальное возвышение считается для грузовой скорости
			double h=(pow(GetModelVavg()/3.6,2)/R-A)/0.00613;
			//Округляем до 5
			h=round_to(h,5);
			
			while (VModelAnpPassCondition().TestValue(fabs(calc_anp(GetModelVpass(), fabs(GetPlanMark(i).value), h))))
			{
				h+=5;
			}
			
			while (GetModelVhigh()!=NO_SPEED && VModelAnpHighCondition().TestValue(fabs(calc_anp(GetModelVhigh(), fabs(GetPlanMark(i).value), h))))
			{
				h+=5;
			}
			
			while (GetModelVhigh_bt()!=NO_SPEED && VModelAnpHighCondition().TestValue(fabs(calc_anp(GetModelVhigh_bt(), fabs(GetPlanMark(i).value), h))))
			{
				h+=5;
			}
			
				
			GetLevelMark(i).value=GetSign(ds)*h;
			GetLevelMark(i+1).value=GetSign(ds)*h;
		}
		//Загоняем значение уровня в допустимые границы
		AdjustLevelValue();
	}
}

///Рассчитывает возвышение близкое к факту
void VCurve::CreateLevelFact(VDataSet & ds)
{
	if (PlanMarksValid() && LevelMarksValid())
	{
		//Выравниваем координаты
		FixLevelCoordinates(ds);		
		//Определяем возвышение
		for (int i=1;i<GetLevelMarkCount()-1;i+=2)
		{
			double avg_level=ds.GetParameter(PRM_LEVEL).GetMO(GetLevelMark(i).index, GetLevelMark(i+1).index);
			if (GetSign(ds)*avg_level < 0 || fabs(avg_level) < 5.0)
				avg_level=0;			
			double rnd_level=round_to(avg_level, 5.0);
			if (rnd_level==5.0)
				rnd_level=10.0;
			else if (rnd_level==-5.0)
				rnd_level=-10.0;
			GetLevelMark(i).value=rnd_level;
			GetLevelMark(i+1).value=rnd_level;
		}
		//Загоняем значение уровня в допустимые границы
		AdjustLevelValue();
	}
}

///Рассчитывает возвышение рациональное
void VCurve::CreateLevelRational(VDataSet & ds)
{
	if (PlanMarksValid() && LevelMarksValid())
	{
		//Выравниваем координаты
		FixLevelCoordinates(ds);		
		//Определяем возвышение
		for (int i=1;i<GetLevelMarkCount()-1;i+=2)
		{		
			double A=0;
			double R=curv_to_radius(fabs(GetPlanMark(i).value));
			//Оптимальное возвышение считается для грузовой скорости
			double h=(pow(GetModelVavg()/3.6,2)/R-A)/0.00613;
			//Округляем до 5
			h=round_to(h,5);
			
			while (VModelAnpTruckCondition().TestValue(fabs(calc_anp(GetModelVavg(), fabs(GetPlanMark(i).value), h))))
			{
				h+=5;
			}
				
			GetLevelMark(i).value=GetSign(ds)*h;
			GetLevelMark(i+1).value=GetSign(ds)*h;
		}
		//Загоняем значение уровня в допустимые границы
		AdjustLevelValue();
	}
}

/// Возвращает true, если точка с заданным индексом длежит внутри кривой
int VCurve::HasPointCrv(size_t in_index) const
{
	if (GetMarkCount()>0)
	{
		if (in_index>=GetPlanMark(0).index && in_index<=GetPlanMark(GetMarkCount()-1).index)
			return 0;
		else if (in_index<GetPlanMark(0).index)
			return -1;
		else
			return 1;
	}
	else 
		return 1;
}

/// Возвращает true, если точка с заданным индексом длежит внутри кривой
int VCurve::HasPointLvl(size_t in_index) const
{
	if (GetLevelMarkCount()>0)
	{
		if (in_index>=GetLevelMark(0).index && in_index<=GetLevelMark(GetLevelMarkCount()-1).index)
			return 0;
		else if (in_index<GetLevelMark(0).index)
			return -1;
		else
			return 1;
	}
	else 
		return 1;
}


/// Устанавливает скорости для модели (скорости по приказу)
void VCurve::ResetModelSpeeds(VDataSet & ds)
{
	double start=GetRangeCoord(ds, true, false, true);
	double end=GetRangeCoord(ds, true, false, false);
	SetModelVhigh_bt(ds.GetAprinfo().GetMaxHighSpeedBT(start, end));
	SetModelVhigh(ds.GetAprinfo().GetMaxHighSpeed(start, end));
	SetModelVpass(ds.GetAprinfo().GetMaxPassSpeed(start, end));
	SetModelVcrg(ds.GetAprinfo().GetMaxTruckSpeed(start, end));
	SetModelVavg(0.75*ds.GetAprinfo().GetMaxTruckSpeed(start, end));
}

//------------------------------------------ Методы класса VCriterionFunction -------------------------------------

/// Конструктор
VCriterionFunction::VCriterionFunction(VCurve * in_curve, VDataSet * in_data_set)
{
	curve=in_curve;
	data_set=in_data_set;	
}
/// Метод, устанавливающий значение параметра с номером index равным value
void VCriterionFunction::SetParam(size_t index, double value)
{	
	if (!(index>0 && curve->GetPlanMark(index-1).index > value) &&
	    !(index+1<curve->GetMarkCount() && curve->GetPlanMark(index+1).index < value))		
		curve->GetPlanMark(index).index=static_cast<uint32_t>(value);
}
/// Метод, возвращающий значение параметра с номером index		
double VCriterionFunction::GetParam(size_t index)
{
	return curve->GetPlanMark(index).index;
}

/// Метод, устанавливающий границы параметра с номером index равным value		
void VCriterionFunction::SetParamRange(size_t index, const pro::interval<double>& value) {}
/// Метод, возвращающий границы параметра с номером index		
pro::interval<double> VCriterionFunction::GetParamRange(size_t index)
{
	return pro::interval<double>(std::max<double>(curve->GetPlanMark(index).index-100, curve->GetPlanMarkLeftMargin(*data_set, index)),
		std::min<double>(curve->GetPlanMark(index).index+100, curve->GetPlanMarkRightMargin(*data_set, index)));
}

/// Метод, пересчитвающий значение целевой функции
void VCriterionFunction::Update()
{	
	curve->NormalizePlanMarks(*data_set);	
	data_set->UpdatePlanOffset(curve->GetPlanMark(1).index, curve->IsModel(), true);
}

/// Метод, возвращающий текущее значение функции		
double VCriterionFunction::Value()
{
	double result=curve->GetMaxPlanOffset(*data_set);
	return result;	
}
/// Метод, возвращающий количество параметров		
size_t VCriterionFunction::GetParamCount()
{
	return curve->GetMarkCount();
}

/// "Неуд" ремонту по отводу возвышения при некоей скорости по приказу
bool IsTapFailure(double prk_spd, double tap, bool is_repair)
{
    return calc_vpr(tap, is_repair).GetPassSpeed() < speed_t(prk_spd);
}

bool VCurve::IsLimitTap(VDataSet & ds, bool is_repair, uint32_t trans_num) const
{
    std::vector<double> vprk_all = this->GetArrayVprkPass(ds);
    size_t idx_from = this->GetLevelMark(trans_num).index - this->tempParams.ranges[0];
    size_t idx_to = this->GetLevelMark(trans_num + 1).index - this->tempParams.ranges[0];
    
    double vprk = 0;
    for (size_t i = idx_from; i <= idx_to; ++i)
    {
        if (vprk < vprk_all[i]) vprk = vprk_all[i];
    }
    if (vprk == 0) return false;

    return IsTapFailure(vprk, this->GetMaxLevelTap(ds, trans_num), is_repair);
}
