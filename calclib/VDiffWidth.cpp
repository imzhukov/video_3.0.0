#include "VDiffWidth.h"
#include "VCalcOptions.h"
#include "VDataSet.h"

//-------------------------------- Методы класса VWidthDiff ------------------------------------------

/// Загрузка
bool VDiffWidth::Load(VParameter & in_width, VParameter & in_width_front, VAbsCoordData & in_abscoord)
{
	if (this->IsLoaded()) 
		return true;	
	//Выделяем и инициализируем память
	this->Create(0xFFFFFFFF, in_abscoord.GetCount(), 0, UNDEFINED_VALUE);
	double base=OPTIONS.GetOption(L"WIDTH_MEAN_BASE").GetValue<double>();
	for (size_t i=0; i<this->GetCount(); ++i)
	{
		size_t stIndex=in_abscoord.FindIndex(in_abscoord.GetItem(i) - base/2, false);
		size_t eIndex=in_abscoord.FindIndex(in_abscoord.GetItem(i) + base/2, false);
		if (stIndex!=BAD_INDEX && eIndex!=BAD_INDEX)
		{
			this->GetItem(i)=in_width.GetMO(stIndex,eIndex) - in_width_front.GetMO(stIndex,eIndex);
			this->SetCorrupt(i, in_width.GetCorruptCount(stIndex, eIndex)>0 || in_width_front.GetCorruptCount(stIndex, eIndex)>0);
		}
		else
		{
			this->GetItem(i)=UNDEFINED_VALUE;
			this->SetCorrupt(i, false);
		}
	}
	return true;
}

//---------------------------- Методы класса VDiffWidthOverstep --------------------------------------

/// Конструктор
VDiffWidthOverstep::VDiffWidthOverstep(const VExcess& in_excess, const VAprinfo& in_aprinfo)
{
	start=in_excess.GetStart();
	end=in_excess.GetEnd();
	coord=in_excess.GetMaxCoord();	
	waycoord=in_aprinfo.GetRealCoord(coord);
	max_value=in_excess.GetMax();
	min_value=in_excess.GetMin();
	value=max_value;
	length=fabs(end-start);
	type=boost::str(boost::wformat(L"Отж.>%0.0f") % OPTIONS.GetOption(L"MAX_DIFF_WIDTH").GetValue<double>());
}

/// Вовращает описание
std::wstring VDiffWidthOverstep::GetDescription() const
{
	return boost::str(boost::wformat(L"%s / %0.1f" ) % type % value);
}

/// Возвращает координату	
double VDiffWidthOverstep::GetCoord() const
{
	return coord;
}

//-------------------------- Методы класса VDiffWidthOverstepList -------------------------

/// Конструктор
VDiffWidthOverstepList::VDiffWidthOverstepList() : loaded(false){};


/// Добавляет отступление в список
void VDiffWidthOverstepList::AddOverstep(const VExcess& in_excess, const VAprinfo& in_aprinfo)
{
	oversteps.push_back(VBasicOverstepPtr(new VDiffWidthOverstep(in_excess, in_aprinfo)));
}

/// Расчет
void VDiffWidthOverstepList::Calculate(VDataSet& in_ds, PARAMETER_TYPE in_data_type)
{
	oversteps.clear();
	VFindExcess find_excess;	
	VRangeCondition range_condition(pro::interval<double>(OPTIONS.GetOption(L"MAX_DIFF_WIDTH").GetValue<double>(), DBL_MAX, true, false));
	for (size_t i=0;i<in_ds.GetKadrCount();i++)
	{
		if (!in_ds.GetParameter(in_data_type).IsCorrupt(i))
		{
			VPoint point(in_ds.GetAbsCoord().GetItem(i), in_ds.GetParameter(in_data_type).GetItem(i));
			find_excess.Input(point, range_condition);
			while (find_excess.Ready())
			{
				AddOverstep(find_excess.Front(), in_ds.GetAprinfo());
				find_excess.Pop();
			}
		}
	}
}

/// Загрузка
bool VDiffWidthOverstepList::Load(VDataSet& in_ds, PARAMETER_TYPE in_data_type)
{
	if (loaded)
		return true;	
	Calculate(in_ds, in_data_type);	
	loaded=true;
	return true;
}

/// Выгрузка
void VDiffWidthOverstepList::Unload()
{
	oversteps.clear();
	loaded=false;
}

/// Возвращает число записей
size_t VDiffWidthOverstepList::GetCount() const
{		
	return oversteps.size();
}

/// Возвращает запись по номеру
const VBasicOverstepPtr& VDiffWidthOverstepList::GetItem(size_t in_index) const
{
	return oversteps.at(in_index);
}
