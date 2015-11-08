#include "canting.h"
#include "VCalcOptions.h"
#include "VDataSet.h"

//---------------------------- Методы класса VCantingOverstep --------------------------------------

/// Конструктор
VCantingOverstep::VCantingOverstep(const std::wstring& in_type, double in_start, double in_end, double in_min_value, double in_max_value, double in_average_value)
{
	start=in_start;
	end=in_end;
	max_value=in_max_value;
	min_value=in_min_value;
	value=in_max_value;
	length=fabs(in_end-in_start);
	type=in_type;
	average_value=in_average_value;
}

/// Вовращает описание
std::wstring VCantingOverstep::GetDescription() const
{
	return boost::str(boost::wformat(L"%s / %0.1f" ) % type % value);
}

/// Возвращает среднее
double VCantingOverstep::GetAverage() const
{
	return average_value;
}

//-------------------------- Методы класса VCantingOverstepList -------------------------

/// Конструктор
VCantingOverstepList::VCantingOverstepList(side_t in_side) : loaded(false), side(in_side){};


/// Добавляет отступление в список
void VCantingOverstepList::AddOverstep(double start, double end, double min_value, double max_value, double average_value)
{
	oversteps.push_back(VBasicOverstepPtr(new VCantingOverstep(side==SD_LEFT ? L"ПР лв." : L"ПР пр.", start, end, min_value, max_value, average_value)));
}

/// Расчет
void VCantingOverstepList::Calculate(VDataSet& in_ds)
{
	oversteps.clear();
	VFindExcess find_excess_high;	
	VFindExcess find_excess_low;
	for (size_t i=0;i<in_ds.GetKadrCount();i++)
	{
		double high_threshold=CANTING_1_DIV_12;
		double low_threshold=fabs(double(in_ds.GetAprinfo().GetPassportCurveNormaLevel(in_ds.GetAbsCoord().GetItem(i))))<CANTING_LEVEL_THRESHOLD ? 
			CANTING_1_DIV_60 : CANTING_1_DIV_30;
		VRangeCondition range_condition_high(pro::interval<double>(high_threshold, DBL_MAX, true, false));
		//Порог в зависимости от возвышения (если возвышение больше 85 то порог 1/30)	
		VRangeCondition range_condition_low(pro::interval<double>(-DBL_MAX, low_threshold, false, true));
		if (!in_ds.GetParameter(side==SD_LEFT ? PRM_PPR_POD_LEFT : PRM_PPR_POD_RIGHT).IsCorrupt(i))
		{
			VPoint point(in_ds.GetAbsCoord().GetItem(i), in_ds.GetParameter(side==SD_LEFT ? PRM_PPR_POD_LEFT : PRM_PPR_POD_RIGHT).GetItem(i));
			find_excess_high.Input(point, range_condition_high);
			while (find_excess_high.Ready())
			{
				AddOverstep(find_excess_high.Front().GetStart(), find_excess_high.Front().GetEnd(), high_threshold, find_excess_high.Front().GetMax(), find_excess_high.Front().GetAverage());
				find_excess_high.Pop();
			}
			find_excess_low.Input(point, range_condition_low);
			while (find_excess_low.Ready())
			{
				AddOverstep(find_excess_low.Front().GetStart(), find_excess_low.Front().GetEnd(), find_excess_low.Front().GetMin(), low_threshold, find_excess_low.Front().GetAverage());
				find_excess_low.Pop();
			}
		}
	}
}

/// Загрузка
bool VCantingOverstepList::Load(VDataSet& in_ds)
{
	if (loaded)
		return true;	
	Calculate(in_ds);	
	loaded=true;
	return true;
}

/// Выгрузка
void VCantingOverstepList::Unload()
{
	oversteps.clear();
	loaded=false;
}

/// Возвращает число записей
size_t VCantingOverstepList::GetCount() const
{		
	return oversteps.size();
}

/// Возвращает запись по номеру
const VBasicOverstepPtr& VCantingOverstepList::GetItem(size_t in_index) const
{
	return oversteps.at(in_index);
}


//---------------------------- Методы класса VExtWearOverstep --------------------------------------

/// Конструктор
VExtWearOverstep::VExtWearOverstep(const std::wstring& in_type, double in_start, double in_end, double in_min_value, double in_max_value, double in_average_value)
{
	start=in_start;
	end=in_end;
	max_value=in_max_value;
	min_value=in_min_value;
	value=in_max_value;
	length=fabs(in_end-in_start);
	type=in_type;
	average_value=in_average_value;
}

/// Вовращает описание
std::wstring VExtWearOverstep::GetDescription() const
{
	return boost::str(boost::wformat(L"%s / %0.1f" ) % type % value);
}

/// Возвращает среднее
double VExtWearOverstep::GetAverage() const
{
	return average_value;
}



//-------------------------- Методы класса VExtWearOverstepList -------------------------

/// Конструктор
VExtWearOverstepList::VExtWearOverstepList(side_t in_side) : loaded(false), side(in_side){};


/// Добавляет отступление в список
void VExtWearOverstepList::AddOverstep(double start, double end, double min_value, double max_value, double average_value)
{
	oversteps.push_back(VBasicOverstepPtr(new VExtWearOverstep(side==SD_LEFT ? L"Вн. изн лв." : L"Вн. изн. пр.", start, end, min_value, max_value, average_value)));
}

/// Расчет
void VExtWearOverstepList::Calculate(VDataSet& in_ds)
{
	oversteps.clear();
	VFindExcess find_excess;		
	for (size_t i=0;i<in_ds.GetKadrCount();i++)
	{
		double threshold=OPTIONS.GetOption(L"EXT_WEAR_THRESHOLD").GetValue<double>();

		VRangeCondition range_condition(pro::interval<double>(threshold, DBL_MAX, true, false));

		if (!in_ds.GetParameter(side==SD_LEFT ? PRM_PPR_IZN_EXT_LEFT : PRM_PPR_IZN_EXT_RIGHT).IsCorrupt(i))
		{
			VPoint point(in_ds.GetAbsCoord().GetItem(i), in_ds.GetParameter(side==SD_LEFT ? PRM_PPR_IZN_EXT_LEFT : PRM_PPR_IZN_EXT_RIGHT).GetItem(i));
			find_excess.Input(point, range_condition);
			while (find_excess.Ready())
			{
				AddOverstep(find_excess.Front().GetStart(), find_excess.Front().GetEnd(), threshold, find_excess.Front().GetMax(), find_excess.Front().GetAverage());
				find_excess.Pop();
			}
		}
	}
}

/// Загрузка
bool VExtWearOverstepList::Load(VDataSet& in_ds)
{
	if (loaded)
		return true;	
	Calculate(in_ds);	
	loaded=true;
	return true;
}

/// Выгрузка
void VExtWearOverstepList::Unload()
{
	oversteps.clear();
	loaded=false;
}

/// Возвращает число записей
size_t VExtWearOverstepList::GetCount() const
{		
	return oversteps.size();
}

/// Возвращает запись по номеру
const VBasicOverstepPtr& VExtWearOverstepList::GetItem(size_t in_index) const
{
	return oversteps.at(in_index);
}
