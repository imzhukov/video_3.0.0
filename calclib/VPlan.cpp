#include <fstream>
#include "mathlib.h"
#include <Shiny.h>
#include "VDataSet.h"

using namespace std;

#ifdef _OPENMP
	#include "omp.h"
#endif

//--------------------------------------- Методы класса VPlan ---------------------------------------------------------

/** Выгрузка*/
void VPlan::Unload()
{
	data.clear();
	this->SetLoaded(false);
}

/** Загрузка и расчет*/
bool VPlan::Load(VDataSet& ds)
{		
	if (IsLoaded()) return true;	
	if (!ds.GetParameter(PRM_KURS).IsLoaded() || !ds.GetAbsCoord().IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}	
	//Резервируем память
	data.reserve(ds.GetAbsCoord().GetCount());
	data.resize(ds.GetAbsCoord().GetCount());	
	data.at(0)=point_t(0,0);
	//Расчет			
	double latitude=0;
	double longitude=0;
	if (ds.GetParameter(PRM_LATITUDE).IsLoaded() && ds.GetParameter(PRM_LONGITUDE).IsLoaded())
	{		
		latitude=ds.GetParameter(PRM_LATITUDE).GetItem(0);
		longitude=ds.GetParameter(PRM_LONGITUDE).GetItem(0);
	}
	else
	{
		LOG_WARNING(L"План рассчитан без учета схождения меридианов.");
	}
	for (size_t i=1; i<data.size(); ++i)
	{
		//Приращение путевой координаты
		double abscoord_delta=ds.GetAbsCoord().GetItem(i)-ds.GetAbsCoord().GetItem(i-1);
		//Курс
		double course=ds.GetParameter(PRM_KURS).GetItem(i-1);		
		//Определяем схождение меридианов
		if (ds.GetParameter(PRM_LATITUDE).IsLoaded() && ds.GetParameter(PRM_LONGITUDE).IsLoaded())
		{														
			//Привязываемся к точке из которой начали движение			
			double L0=(ds.GetStartLongitude()!=0) ? ds.GetStartLongitude() : ds.GetParameter(PRM_LONGITUDE).GetItem(0);			
			//FixMe!!! широта и долгота могут иметь сбои - резкие скачки на протяжении 10-15 кадров. Необходима более корректная защита от этого
			latitude=fabs(ds.GetParameter(PRM_LATITUDE).GetItem(i)-latitude) < 0.001 ? ds.GetParameter(PRM_LATITUDE).GetItem(i) : latitude;
			longitude=fabs(ds.GetParameter(PRM_LONGITUDE).GetItem(i)-longitude) < 0.001 ? ds.GetParameter(PRM_LONGITUDE).GetItem(i) : longitude;
			double convergence = (longitude-L0)*sin(latitude);
			course-=convergence;
		}		
		//Вычисляем план		
		data.at(i)=CalcNextPoint(data.at(i-1), course, abscoord_delta);
	}
	this->SetLoaded(true);
	return true;
}

//------------------------------------- Методы класса VIdealPlan ----------------------------------------------

/** Расчет плана на кривой*/
void VIdealPlan::CalcCurvePlan(const VCurve & curve, const VParameter & in_course, const VAbsCoordData & in_abscoord)
{
	if (curve.PlanMarksValid())
	{
		double ideal_course=in_course.GetItem(curve.GetPlanMark(0).index);		
		size_t start_index=curve.GetPlanMark(0).index;
		size_t end_index=curve.GetPlanMark(curve.GetMarkCount()-1).index;
		for (size_t k=start_index+1; k<=end_index; ++k)
		{
			double d=in_abscoord.GetItem(k)-in_abscoord.GetItem(k-1);
			double ideal_curv=curve.AproxPlan(k, in_abscoord);
			double ideal_curv_prev=curve.AproxPlan(k-1, in_abscoord);
			double delta_course=(ideal_curv+ideal_curv_prev)*(d)/100000;//FixMe!!! Magic number										
			data.at(k)=CalcNextPoint(data.at(k-1), ideal_course, d);
			ideal_course=ideal_course+delta_course;
		}
	}
}

/** Расчет плана для прямой*/
void VIdealPlan::CalcLinePlan(const VPlanElement & in_plan_element, const VPlan & in_plan, 
		const VAbsCoordData & in_abscoord)
{	
	if (in_plan_element.idxStart==in_plan_element.idxEnd || 
		in_plan.GetItem(in_plan_element.idxStart).x==in_plan.GetItem(in_plan_element.idxEnd).x)
	{
	}
	else
	{				
		//Рассчитываем план на прямой		
		double ax=(in_plan.GetItem(in_plan_element.idxEnd).x - in_plan.GetItem(in_plan_element.idxStart).x)/
			double(in_plan_element.idxEnd-in_plan_element.idxStart);
		double ay=(in_plan.GetItem(in_plan_element.idxEnd).y - in_plan.GetItem(in_plan_element.idxStart).y)/
			double(in_plan_element.idxEnd-in_plan_element.idxStart);
		for (size_t j=in_plan_element.idxStart; j<=in_plan_element.idxEnd; ++j)
		{			
			data.at(j).x=in_plan.GetItem(in_plan_element.idxStart).x + double(j-in_plan_element.idxStart)*ax;
			data.at(j).y=in_plan.GetItem(in_plan_element.idxStart).y + double(j-in_plan_element.idxStart)*ay;
		}		
	}	
}


/** Подгонка плана на элементе*/
void VIdealPlan::CorrectPlan(const VPlanElement & in_plan_element, const VPlan & in_plan)
{
	double shift_x=data.at(in_plan_element.idxStart+1).x-in_plan.GetItem(in_plan_element.idxStart+1).x;
	double shift_y=data.at(in_plan_element.idxStart+1).y-in_plan.GetItem(in_plan_element.idxStart+1).y;
	for (size_t k=in_plan_element.idxStart+1; k<in_plan_element.idxEnd; ++k)
	{
		data.at(k).x=data.at(k).x - shift_x;
		data.at(k).y=data.at(k).y - shift_y;
	}
	double count=(in_plan_element.idxEnd-in_plan_element.idxStart-2);
	if (count>0)
	{
		double drift_x=(data.at(in_plan_element.idxEnd-1).x - in_plan.GetItem(in_plan_element.idxEnd-1).x)/count;
		double drift_y=(data.at(in_plan_element.idxEnd-1).y - in_plan.GetItem(in_plan_element.idxEnd-1).y)/count;
		for (size_t k=in_plan_element.idxStart+1; k<in_plan_element.idxEnd; ++k)
		{
			data.at(k).x=data.at(k).x - (k-in_plan_element.idxStart)*drift_x;
			data.at(k).y=data.at(k).y - (k-in_plan_element.idxStart)*drift_y;	
		}
	}
	data.at(in_plan_element.idxStart)=in_plan.GetItem(in_plan_element.idxStart);
	data.at(in_plan_element.idxEnd)=in_plan.GetItem(in_plan_element.idxEnd);
}

/** Сглаживание углов кривыми Безье*/
void VIdealPlan::Bezier(const VPlanElemList& in_plan_elem_list, size_t in_first, size_t in_last)
{
	for (size_t i=in_first+1; i<=in_last; ++i)
	{
		VPlanElement element1=in_plan_elem_list.GetItem(i-1);
		VPlanElement element2=in_plan_elem_list.GetItem(i);					
		//Задаем три опорные точки
		size_t i0=std::max<int>(int(element1.idxEnd)-100, (element1.idxEnd+element1.idxStart)/2);//FixMe!!! необходимо корректно учитывать случай когда element1.idxEnd<100
		size_t i1=element1.idxEnd;
		size_t i2=std::min<int>(int(element2.idxStart)+100, (element2.idxEnd+element2.idxStart)/2);
		
		point_t p0=GetItem(i0);
		point_t p1=GetItem(i1);
		point_t p2=GetItem(i2);
		for (size_t j=i0; j<=i2; ++j)
		{
			this->GetItem(j)=bezier2(p0,p1,p2, double(j-i0) / double(i2-i0));
		}
	}
}

/** Копирует измеренный план*/
void VIdealPlan::CopyMeasuredPlan(const VPlanElement & plan_element, const VPlan & in_plan)
{
	for (size_t j=plan_element.idxStart; j<=plan_element.idxEnd; ++j)
	{
		data.at(j).x=in_plan.GetItem(j).x;
		data.at(j).y=in_plan.GetItem(j).y;
	}
}

/** Выгрузка*/
void VIdealPlan::Unload()
{
	data.clear();
	this->SetLoaded(false);
}

/** Загрузка и расчет*/
bool VIdealPlan::Load(const VParameter& in_course, const VPlan& in_plan, 
	const VAbsCoordData& in_abscoord, const VPlanElemList& in_plan_elements, 
	const VCurveList& in_curves)
{
	if (this->IsLoaded()) 
		return true;	
	if (!in_course.IsLoaded() || !in_plan.IsLoaded() || 
		!in_abscoord.IsLoaded() || !in_plan_elements.IsLoaded() /*|| !in_curves.IsLoaded()*/)
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	if (!in_curves.IsLoaded())
	{
		LOG_WARNING(L"Идеальный план: Не загружена разметка кривых");
	}

	//Резервируем память
	data.reserve(in_abscoord.GetCount());
	data.resize(in_abscoord.GetCount());
	data.at(0)=in_plan.GetItem(0);	
	//Рассчитываем план
#pragma omp parallel for
	for (int i=0; i<in_plan_elements.GetCount(); ++i)
	{
		VPlanElement plan_element=in_plan_elements.GetItem(i);
		//FixMe!!! часть прямой рассматривается как целая прямая, что не совсем корректно
		if (plan_element.type==PLAN_ELEM_HALFLINE || plan_element.type==PLAN_ELEM_LINE)
		{
			CalcLinePlan(plan_element, in_plan, in_abscoord);
			CorrectPlan(in_plan_elements.GetItem(i), in_plan);
		}
		//Если это кривая
		else if (plan_element.type==PLAN_ELEM_CURVE)
		{						
			CalcCurvePlan(in_curves.GetItem(plan_element.curve_index), in_course, in_abscoord);
			CorrectPlan(in_plan_elements.GetItem(i), in_plan);
		}
		//Во всех остальных случаях план соответствует реальному (FixMe!!! временно)
		else
		{
			CopyMeasuredPlan(plan_element, in_plan);					
		}
	}	
	//Сглаживаем углы кривыми Безье
	if (OPTIONS.GetOption(L"BEZIER_ON_PLAN").Compare(OPTION_YES) && in_plan_elements.GetCount()>0)
		Bezier(in_plan_elements, 0, in_plan_elements.GetCount()-1);
	this->SetLoaded(true);
	return true;
}

/** Загрузка и расчет*/
bool VIdealPlan::Load(VDataSet& ds)
{	
	return Load(ds.GetParameter(PRM_KURS), ds.GetPlan(), 
		ds.GetAbsCoord(), ds.GetPlanElemList(is_model ? PRM_MODEL_PLAN_ELEM_LIST : PRM_PLAN_ELEM_LIST),
		ds.GetCurves(is_model ? PRM_MODEL_CURVES : PRM_CURVE_MARKS));
}

/** Обновление*/
void VIdealPlan::Update(VDataSet& ds, size_t in_first_element_index, size_t in_last_element_index)
{
	PARAMETER_TYPE plan_elem_source = is_model ? PRM_MODEL_PLAN_ELEM_LIST : PRM_PLAN_ELEM_LIST;
	PARAMETER_TYPE curves_source = is_model ? PRM_MODEL_CURVES : PRM_CURVE_MARKS;
	
	if (data.size()!=ds.GetKadrCount())
	{
		data.reserve(ds.GetKadrCount());
		data.resize(ds.GetKadrCount());
	}		
#pragma omp parallel for
	for (int i=in_first_element_index; i<=in_last_element_index; ++i)
	{
		VPlanElement plan_element=ds.GetPlanElemList(plan_elem_source).GetItem(i);
		switch (plan_element.type)
		{
			case PLAN_ELEM_CURVE:	
			{				
				CalcCurvePlan(ds.GetCurves(curves_source).GetItem(plan_element.curve_index), ds.GetParameter(PRM_KURS), ds.GetAbsCoord());				
				CorrectPlan(ds.GetPlanElemList(plan_elem_source).GetItem(i), ds.GetPlan());
				break;
			}
			case PLAN_ELEM_LINE:
			case PLAN_ELEM_HALFLINE:
			{
				CalcLinePlan(plan_element, ds.GetPlan(), ds.GetAbsCoord());
				CorrectPlan(ds.GetPlanElemList(plan_elem_source).GetItem(i), ds.GetPlan());
				break;
			}
			default:
				break;
		}
	}	
	//Сглаживаем углы кривыми Безье
	if (OPTIONS.GetOption(L"BEZIER_ON_PLAN").Compare(OPTION_YES) && ds.GetPlanElemList(plan_elem_source).GetCount()>0)
		Bezier(ds.GetPlanElemList(plan_elem_source), 0, ds.GetPlanElemList(plan_elem_source).GetCount()-1);
	this->SetLoaded(true);
}

//---------------------------------- Методы класса VPlanOffset ------------------------------

/// Загрузка
bool VPlanOffset::Load(const VPlan& in_plan, const VIdealPlan& in_ideal_plan)
{
	if (this->IsLoaded()) 
		return true;
	if (!in_plan.IsLoaded() || !in_ideal_plan.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}
	// массив со значениями отклонений
	this->Create(0xFFFFFFFF, in_plan.GetCount(), 0, UNDEFINED_VALUE);	
#pragma omp parallel for
	for (int i=0; i<GetCount()-1; ++i)
	{			
		if (i>0)
		{
			//переводим в сантиметры
			this->GetItem(i)=100.0*length(in_plan.GetItem(i), 
				line_t(in_ideal_plan.GetItem(i-1), 
					   in_ideal_plan.GetItem(i+1)));
		}
	}

	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
}

/// Загрузка
bool VPlanOffset::Load(VDataSet& ds)
{
	return Load(ds.GetPlan(), ds.GetIdealPlan(is_model ? PRM_MODEL_IDEAL_PLAN : PRM_IDEAL_PLAN));	
}

/// Обновление
bool VPlanOffset::Update(VDataSet& ds, size_t in_start, size_t in_end)
{	
	if (this->GetCount()!=ds.GetKadrCount())
	{
		this->Create(0xFFFFFFFF, ds.GetKadrCount(), 0, UNDEFINED_VALUE);
	}
	if (in_start<in_end && in_end<this->GetCount())
	{
#pragma omp parallel for
		for (int i=in_start; i<=in_end; ++i)
		{						
			if (i>0 && i+1<GetCount())
			{
				//переводим в сантиметры
				this->GetItem(i)=100.0*length(ds.GetPlan().GetItem(i), 
					line_t(ds.GetIdealPlan(is_model ? PRM_MODEL_IDEAL_PLAN : PRM_IDEAL_PLAN).GetItem(i-1), 
						   ds.GetIdealPlan(is_model ? PRM_MODEL_IDEAL_PLAN : PRM_IDEAL_PLAN).GetItem(i+1)));
			}
			else
			{
				this->GetItem(i)=UNDEFINED_VALUE;
			}

		}
		this->SetLoaded(true);
	}
	return true;
}

//------------------------------- Методы класса VAnpReal -----------------------------------------------

/// Загрузка
bool VAnpReal::Load(VDataSet& ds, speed_type in_speed)
{
	if (this->IsLoaded()) 
		return true;
	if (!ds.GetParameter(PRM_CURV).IsLoaded() || !ds.GetParameter(PRM_LEVEL).IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}
	//Выделяем и инициализируем память
	this->Create(0xFFFFFFFF, ds.GetKadrCount(), 0, UNDEFINED_VALUE);	
	//Вычиcляем значения	
	for (size_t i=1; i<GetCount(); ++i)
	{			
		float speed=OPTIONS.DEFAULT_SPEED();
		switch (in_speed)
		{
			case high_prk:
				speed=static_cast<float>(ds.GetAprinfo().GetHighSpeed(ds.GetAbsCoord().GetItem(i)));
				break;
			case high_bt_prk:
				speed=static_cast<float>(ds.GetAprinfo().GetHighSpeedBT(ds.GetAbsCoord().GetItem(i)));
				break;
			case pass_prk:
				speed=static_cast<float>(ds.GetAprinfo().GetPassSpeed(ds.GetAbsCoord().GetItem(i)));
				break;
			case crg_prk:
				speed=static_cast<float>(ds.GetAprinfo().GetTruckSpeed(ds.GetAbsCoord().GetItem(i)));
				break;			
			default:
				speed=OPTIONS.DEFAULT_SPEED();
				break;
		}		
		double base=OPTIONS.ANP_BASE(speed);		
		float curv=ds.GetParameter(PRM_CURV).GetMO(i, base/2, ds.GetAbsCoord());
		float level=ds.GetParameter(PRM_LEVEL).GetMO(i, base/2, ds.GetAbsCoord());
		this->GetItem(i) = calc_anp(speed, curv, level);
	}
	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;		
}

//------------------------------- Методы класса VPsiReal ------------------------------------------

///Загрузка
bool VPsiReal::Load(VDataSet& ds, speed_type in_speed)
{	
	if (this->IsLoaded()) 
		return true;	
	//Выделяем и инициализируем память
	this->Create(0xFFFFFFFF, ds.GetKadrCount(), 0, UNDEFINED_VALUE);	
	//Вычиcляем значения	
#pragma omp parallel for
	for (int i=1; i<GetCount(); ++i)
	{								
		float speed=OPTIONS.DEFAULT_SPEED();
		switch (in_speed)
		{
			case high_prk:
				speed=static_cast<float>(ds.GetAprinfo().GetHighSpeed(ds.GetAbsCoord().GetItem(i)));
				break;
			case high_bt_prk:
				speed=static_cast<float>(ds.GetAprinfo().GetHighSpeedBT(ds.GetAbsCoord().GetItem(i)));
				break;
			case pass_prk:
				speed=static_cast<float>(ds.GetAprinfo().GetPassSpeed(ds.GetAbsCoord().GetItem(i)));
				break;
			case crg_prk:
				speed=static_cast<float>(ds.GetAprinfo().GetTruckSpeed(ds.GetAbsCoord().GetItem(i)));
				break;			
			default:
				speed=OPTIONS.DEFAULT_SPEED();
				break;
		}
		double base=OPTIONS.PSI_BASE(speed);		
		switch (in_speed)
		{
			case high_prk:
				this->GetItem(i)=static_cast<float>(speed)*ds.GetAnpHighPass().GetOtvod(i, ds.GetAbsCoord(), base)/3.6;
				break;
			case high_bt_prk:
				this->GetItem(i)=static_cast<float>(speed)*ds.GetAnpHighBTTruck().GetOtvod(i, ds.GetAbsCoord(), base)/3.6;
				break;
			case pass_prk:
				this->GetItem(i)=static_cast<float>(speed)*ds.GetAnpRealPass().GetOtvod(i, ds.GetAbsCoord(), base)/3.6;
				break;
			case crg_prk:
				this->GetItem(i)=static_cast<float>(speed)*ds.GetAnpRealTruck().GetOtvod(i, ds.GetAbsCoord(), base)/3.6;
				break;			
			default:
				this->GetItem(i)=UNDEFINED_VALUE;
				break;
		}
	}		
	return true;
}

//--------------------------------------- Методы класса VAnpModel -----------------------------------

/// Загрузка
bool VAnpModel::Load(const VAbsCoordData & in_abscoord, const VCurveList & in_curves, const VAprinfo& in_aprinfo, 
	speed_type in_speed, bool in_use_passport_speeds)
{
	if (this->IsLoaded()) 
		return true;
	if (!in_abscoord.IsLoaded() || !in_curves.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}
	//Добавляем точки с кривизны и уровня
	for (size_t i=0; i<in_curves.GetCount(); ++i)
	{		
		std::vector<VAnp> anp=in_curves.GetItem(i).GetElementAnp(in_abscoord, in_aprinfo, in_speed, in_use_passport_speeds);
		if (anp.size()>0)
			data.insert(data.end(), anp.begin(), anp.end());
	}	
	if (!in_use_passport_speeds)
	{
		//Добавляем точки изменения скорости по приказу	
		std::vector<VAprinfoSpeedRecord> speeds=in_aprinfo.GetSpeedList(in_abscoord.GetItem(0), in_abscoord.GetLastItem());
		for (size_t i=0; i<speeds.size(); ++i)
		{	
			long speed;
			switch (in_speed)
			{
				default:
				case pass_prk:
					speed=speeds.at(i).spd.GetPassSpeed();
					break;
				case crg_prk:
					speed=speeds.at(i).spd.GetTruckSpeed();
					break;
				case high_prk:
					speed=speeds.at(i).spd.GetHighSpeed();
					break;
				case high_bt_prk:
					speed=speeds.at(i).spd.GetHighBTSpeed();
					break;
			}
			if (speed!=NO_SPEED)
			{
				size_t start=in_abscoord.FindIndex(speeds.at(i).base, true);
				float curv_start=in_curves.AproxPlan(start, in_abscoord);
				float level_start=in_curves.AproxLevel(start, in_abscoord);

				VAnp anp;
				anp.index=start;
				anp.value=calc_anp(speed, curv_start, level_start);
				data.push_back(anp);

				size_t end=in_abscoord.FindIndex(speeds.at(i).end, true)-1;
				float curv_end=in_curves.AproxPlan(end, in_abscoord);
				float level_end=in_curves.AproxLevel(end, in_abscoord);

				anp.index=end;
				anp.value=calc_anp(speed, curv_end, level_end);
				data.push_back(anp);
			}
		}
	}
	//Сортируем точки
	std::sort(data.begin(), data.end());
	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
};	

///Выгрузка
void VAnpModel::Unload()
{
	data.clear();
	this->message = ERRMSG_DATA_NOT_LOADED;
	this->SetLoaded(false);
}

/// Число записей
size_t VAnpModel::GetCount() const
{
	return data.size();
};
/// Вернуть констатную ссылку на i-й элемент
const VAnp & VAnpModel::GetItem(size_t index) const
{
	return data.at(index);
};
/// Вернуть ссылку на i-й элемент
VAnp & VAnpModel::GetItem(size_t index)
{
	return data.at(index);
};

/// Возвращает минимальное Анп
VAnp & VAnpModel::GetMin()
{	
	size_t min_index=0;
	for (size_t i=1; i<GetCount(); ++i)
	{
		if (GetItem(i).value<GetItem(min_index).value)
		{			
			min_index=i;
		}
	}
	return GetItem(min_index);
};

/// Возвращает максимальное Анп
VAnp & VAnpModel::GetMax()
{
	size_t max_index=0;
	for (size_t i=1; i<GetCount(); ++i)
	{
		if (GetItem(i).value>GetItem(max_index).value)
		{			
			max_index=i;
		}
	}
	return GetItem(max_index);
};

//---------------------------------- Методы класса VJaggyPlan --------------------------------------------

/// Загрузка
bool VJaggyPlan::Load(VDataSet& ds, bool lines_only)
{
	if (!ds.GetParameter(PRM_PLAN_OFFSET).IsLoaded() || !ds.GetAbsCoord().IsLoaded()) return false;	

	this->Create(0xFFFFFFFF, ds.GetKadrCount(), 0, UNDEFINED_VALUE);

	//Загружаем "фиктивный идеальный план" 
	VPlanElemList fiction_elem_list;
	fiction_elem_list.Load(ds, false, true, false);
	VIdealPlan fiction_plan(false);
	fiction_plan.Load(ds.GetParameter(PRM_KURS), ds.GetPlan(), ds.GetAbsCoord(), fiction_elem_list, ds.GetCurves());
	VPlanOffset fiction_offset(false);
	fiction_offset.Load(ds.GetPlan(), fiction_plan);
	
	size_t first_def=fiction_offset.GetFirstDef();
	size_t last_def=fiction_offset.GetLastDef();

	if (lines_only || OPTIONS.GetOption(L"JAGGY_LINES_ONLY").Compare(OPTION_YES))
	{
		//Расчет только на прямых длиной более 200 м.
		for (size_t j=0; j<ds.GetPlanElemList().GetCount(); j++)
		{
			if (ds.GetPlanElemList().GetItem(j).type==PLAN_ELEM_LINE && ds.GetPlanElemList().GetItem(j).GetLength()>200.0)
			{
				int from=std::min<int>(last_def, ds.GetPlanElemList().GetItem(j).idxStart+100);
				int to=std::max<int>(first_def, ds.GetPlanElemList().GetItem(j).idxEnd-100);
				for (int i=from; i<to; ++i)
				{
					double base=OPTIONS.JAGGY_BASE(ds.GetAprinfo().GetPassSpeed(ds.GetAbsCoord().GetItem(i)));
					this->GetItem(i)=10.0*fiction_offset.GetJaggy(i, base, ds.GetAbsCoord());
				}
			}
		}
	}
	else
	{
		for (size_t i=first_def; i<last_def+1; ++i)
		{				
			double base=OPTIONS.JAGGY_BASE(ds.GetAprinfo().GetPassSpeed(ds.GetAbsCoord().GetItem(i)));
			//Пересчитываем в миллиметры
			this->GetItem(i)=10.0*fiction_offset.GetJaggy(i, base, ds.GetAbsCoord());
		}	
	}

	this->message = ERRMSG_OK;
	this->SetLoaded(true);
	return true;
}


//----------------------------------- Методы VJaggyPlanOverstep -----------------------------------


/// Конструктор
VJaggyPlanOverstep::VJaggyPlanOverstep(double in_start, double in_end, double in_coord, double in_value, 
		double in_otvod, JAGGY_PLAN_SIGNIFICANCE in_significance, const VAprinfo& in_aprinfo)

{
	start=in_start;
	end=in_end;
	coord=in_coord;
	max_value=in_value;
	min_value=in_value;
	value=in_value;
	length=fabs(in_end-in_start);
	type=L"Гориз.нер.";
	significance=in_significance;
	otvod=in_otvod;
	on_bridge=in_aprinfo.GetBridge(GetStart())!=SZO_NONE || 
			  in_aprinfo.GetBridge(GetCoord())!=SZO_NONE || 
			  in_aprinfo.GetBridge(GetEnd())!=SZO_NONE;
	on_switch=in_aprinfo.GetSwitch(GetStart()) ||
			  in_aprinfo.GetSwitch(GetCoord()) ||
			  in_aprinfo.GetSwitch(GetEnd());
	on_station=in_aprinfo.GetStationCode(GetStart())!=0 ||
		       in_aprinfo.GetStationCode(GetCoord())!=0 ||
			   in_aprinfo.GetStationCode(GetEnd())!=0;
	if (on_bridge)
		AddComment(L"[мост]");
	if (on_switch)
		AddComment(L"[стрелка]");
	if (on_station)
		AddComment(L"[станция]");
};

/// Возвращает координату
double VJaggyPlanOverstep::GetCoord() const
{
	return coord;
};

/// Возвращает максимальный отвод
double VJaggyPlanOverstep::GetOtvod() const
{
	return otvod;
};

/// Вовращает описание
std::wstring VJaggyPlanOverstep::GetDescription() const
{	
	return boost::str(boost::wformat(L"%s %s") % GetDegreeString() % GetComment());
};	

/// Возвращает строку с описанием степени
std::wstring VJaggyPlanOverstep::GetDegreeString() const
{
	switch (significance)
	{
		case PLAN_SIGN_2:
			return L"ПД";
		case PLAN_SIGN_1:
			return L"ТВ";
		default:
			return L"";
	}
};


//----------------------------------------- VJaggyPlanOverstepList -------------------------

/// Конструктор
VJaggyPlanOverstepList::VJaggyPlanOverstepList() : loaded(false) {};

/// Добавляет отступление в список
void VJaggyPlanOverstepList::AddOverstep(double start, double end, double center, double value, double otvod, 
	const VAprinfo& aprinfo)
{
	double length=end-start;
	VSpeeds speeds=aprinfo.GetSpeeds((start+end)/2);
	if (pro::interval<int>(141,160).contains(speeds.GetMaxSpeed()))
	{					
		if (pro::interval<double>(0,DBL_MAX,true,false).contains(fabs(length)))
		{	
			if (pro::interval<double>(35,60,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyPlanOverstepPtr(new VJaggyPlanOverstep(start, end, center, value, otvod, PLAN_SIGN_1, aprinfo)));
			}
			if (pro::interval<double>(60,DBL_MAX,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyPlanOverstepPtr(new VJaggyPlanOverstep(start, end, center, value, otvod, PLAN_SIGN_2, aprinfo)));								
			}
		}
	}
	else if (pro::interval<int>(161,200).contains(speeds.GetMaxSpeed()))
	{
		if (pro::interval<double>(0,100,true,false).contains(fabs(length)))
		{	
			if (pro::interval<double>(25,50,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyPlanOverstepPtr(new VJaggyPlanOverstep(start, end, center, value, otvod, PLAN_SIGN_1, aprinfo)));								
			}
			if (pro::interval<double>(50,DBL_MAX,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyPlanOverstepPtr(new VJaggyPlanOverstep(start, end, center, value, otvod, PLAN_SIGN_2, aprinfo)));								
			}
		}
		if (pro::interval<double>(100,DBL_MAX,true,false).contains(fabs(length)))
		{	
			if (pro::interval<double>(30,80,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyPlanOverstepPtr(new VJaggyPlanOverstep(start, end, center, value, otvod, PLAN_SIGN_1, aprinfo)));								
			}
			if (pro::interval<double>(80,DBL_MAX,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyPlanOverstepPtr(new VJaggyPlanOverstep(start, end, center, value, otvod, PLAN_SIGN_2, aprinfo)));								
			}
		}
	}
	else if (pro::interval<int>(201,250).contains(speeds.GetMaxSpeed()))
	{
		if (pro::interval<double>(0,150,true,false).contains(fabs(length)))
		{	
			if (pro::interval<double>(30,60,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyPlanOverstepPtr(new VJaggyPlanOverstep(start, end, center, value, otvod, PLAN_SIGN_1, aprinfo)));								
			}
			if (pro::interval<double>(60,DBL_MAX,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyPlanOverstepPtr(new VJaggyPlanOverstep(start, end, center, value, otvod, PLAN_SIGN_2, aprinfo)));
				
			}
		}
		if (pro::interval<double>(150,DBL_MAX,true,false).contains(fabs(length)))
		{	
			if (pro::interval<double>(45,90,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyPlanOverstepPtr(new VJaggyPlanOverstep(start, end, center, value, otvod, PLAN_SIGN_1, aprinfo)));								
			}
			if (pro::interval<double>(90,DBL_MAX,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyPlanOverstepPtr(new VJaggyPlanOverstep(start, end, center, value, otvod, PLAN_SIGN_2, aprinfo)));								
			}
		}
	}
}

/// Расчет
void VJaggyPlanOverstepList::Calculate(VDataSet& in_ds)
{
	change_calculator<double> sign_changed;	
	min_calculator<double> min_value(0);
	change_calculator<double> min_changed;
	max_calculator<double> max_value(0);
	change_calculator<double> max_changed;
	max_calculator<double> max_otvod(0);
	double start=in_ds.GetAbsCoord().GetItem(0);
	double end=in_ds.GetAbsCoord().GetItem(0);
	double max_coord=start;
	double min_coord=start;	
	
	for (size_t i=0; i<in_ds.GetParameter(PRM_JAGGY_PLAN).GetCount(); ++i)
	{
		VPoint point(in_ds.GetAbsCoord().GetItem(i), in_ds.GetParameter(PRM_JAGGY_PLAN).GetItem(i));
		int speed=in_ds.GetAprinfo().GetPassSpeed(in_ds.GetAbsCoord().GetItem(i));
		min_value.put(point.GetValue());
		max_value.put(point.GetValue());
		max_otvod.put(fabs(in_ds.GetParameter(PRM_JAGGY_PLAN).GetOtvod(i, in_ds.GetAbsCoord(), OPTIONS.JAGGY_OTVOD_BASE(speed))));
		end=point.GetCoord();		
		if (min_changed.put(min_value.get()))
			min_coord=in_ds.GetAbsCoord().GetItem(i);
		if (max_changed.put(max_value.get()))
			max_coord=in_ds.GetAbsCoord().GetItem(i);
		if (sign_changed.put(sign<double>(point.GetValue())))
		{						
			if (fabs(min_value.get()) > fabs(max_value.get()))
			{					
				AddOverstep(start, end, min_coord, min_value.get(), max_otvod.get(), in_ds.GetAprinfo());
			}
			else
			{								
				AddOverstep(start, end, max_coord, max_value.get(), max_otvod.get(), in_ds.GetAprinfo());				
			}
			min_value.reset();
			max_value.reset();
			max_otvod.reset();
			start=end;			
		}
	}
	/*
	change_calculator<double> sign_changed;
	min_calculator<double> min_value;
	max_calculator<double> max_value;
	double start=in_ds.GetAbsCoord().GetItem(0);
	double end=in_ds.GetAbsCoord().GetItem(0);
	for (size_t i=0; i<in_ds.GetParameter(PRM_JAGGY_PLAN).GetCount(); ++i)
	{
		VPoint point(in_ds.GetAbsCoord().GetItem(i), in_ds.GetParameter(PRM_JAGGY_PLAN).GetItem(i));
		min_value.put(point.GetValue());
		max_value.put(point.GetValue());
		end=point.GetCoord();
		if (sign_changed.put(sign<double>(point.GetValue())))
		{
			VSpeeds speeds=in_ds.GetAprinfo().GetSpeeds((start+end)/2);
			if (fabs(min_value.get()) > fabs(max_value.get()))
			{				
				AddOverstep(start, end, min_value.get(), speeds);
			}
			else
			{				
				AddOverstep(start, end, max_value.get(), speeds);				
			}
			min_value.reset();
			max_value.reset();
			start=end;
		}
	}
	*/
}

/// Загрузка
bool VJaggyPlanOverstepList::Load(VDataSet& in_ds)
{
	if (loaded)
		return true;
	Calculate(in_ds);
	loaded=true;
	return true;
}

/// Выгрузка
void VJaggyPlanOverstepList::Unload()
{
	oversteps.clear();
	loaded=false;
}

/// Возвращает число записей
size_t VJaggyPlanOverstepList::GetCount() const
{
	return oversteps.size();
}

/// Возвращает запись по номеру
const VJaggyPlanOverstepPtr& VJaggyPlanOverstepList::GetItem(size_t in_index) const
{
	return oversteps.at(in_index);
}