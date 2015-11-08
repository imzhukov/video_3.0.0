#include "VDataSet.h"

//------------------------------- Методы класса VJaggyOverstep ------------------------------------------------

VJaggyOverstep::VJaggyOverstep() : VBasicOverstep(){};

VJaggyOverstep::VJaggyOverstep(const VBasicOverstep& in_source) : VBasicOverstep(in_source){};

void VJaggyOverstep::SetDegree(JAGGY_OVERSTEP_DEGREE in_degree)
{
	degree=in_degree;
};

void VJaggyOverstep::SetOtvod(double in_otvod)
{
	otvod=in_otvod;
};	

std::wstring VJaggyOverstep::GetDescription() const
{
	return boost::str(boost::wformat(_T("%0.1f / %0.0f")) % GetValue() % GetLength());
};

void VJaggyOverstep::CalcOtvod(VDataSet& ds, PARAMETER_TYPE type, double in_scale)
{
	size_t start=ds.GetAbsCoord().FindIndex(GetStart(), true);
	size_t end=ds.GetAbsCoord().FindIndex(GetEnd(), true);
	double otvod=(0-DBL_MAX);
	int current_speed=0;
	for (size_t j=start; j<=end; ++j)
	{
		//В зависимости от скорости меняем условия
		int speed=ds.GetAprinfo().GetPassSpeed(ds.GetAbsCoord().GetItem(j));
		double base=30;
		if (speed!=current_speed)
		{
			current_speed=speed;
			base=OPTIONS.JAGGY_OTVOD_BASE(current_speed);
		}
		otvod=std::max<double>(otvod, in_scale*ds.GetParameter(type).GetOtvod(j, ds.GetAbsCoord(), base));
	}
	SetOtvod(otvod);
}

//-------------------------------- Методы класса VJaggyPlanOverstepList -----------------------------------------

/// Добавляет отступления
void VJaggyPlanOverstepList::AddOversteps(const VFindOverstep& in_oversteps, JAGGY_OVERSTEP_DEGREE in_degree)
{
	/*
	for (size_t i=0; i<in_oversteps.GetCount(); ++i)
	{		
		VJaggyOverstep new_overstep(in_oversteps.GetItem(i));
		new_overstep.SetDegree(in_degree);
		AddItem(new_overstep);
	}
	*/
}

/// Загрузка
bool VJaggyPlanOverstepList::Load(VDataSet& ds)
{
	/*
	if (this->IsLoaded())
		return true;
	
	if (!ds.GetParameter(PRM_JAGGY_PLAN).IsLoaded())
		return false;

	//Находим отступления	

	VFindOverstep plan_ovr_low_pos1;
	VFindOverstep plan_ovr_low_neg1;
	VFindOverstep plan_ovr_low_pos2;
	VFindOverstep plan_ovr_low_neg2;	

	VFindOverstep plan_ovr_high_pos1;
	VFindOverstep plan_ovr_high_neg1;
	VFindOverstep plan_ovr_high_pos2;
	VFindOverstep plan_ovr_high_neg2;	

	condition_t condition_low_pos1;
	condition_t condition_low_neg1;
	condition_t condition_low_pos2;
	condition_t condition_low_neg2;	

	condition_t condition_high_pos1;
	condition_t condition_high_neg1;
	condition_t condition_high_pos2;
	condition_t condition_high_neg2;	

	int current_speed=0;
	
	for (size_t i=0; i<ds.GetParameter(PRM_JAGGY_PLAN).GetCount(); ++i)
	{
		//В зависимости от скорости меняем условия
		int speed=ds.GetAprinfo().GetPassSpeed(ds.GetAbsCoord().GetItem(i));
		if (speed!=current_speed)
		{
			current_speed=speed;
			if (OPTIONS.SPEEDS_RANGE_1().Contains(speed))
			{
				condition_low_pos1=(condition_t(range_t<double>(35, DBL_MAX), range_t<double>(0, 100), _T("")));
				condition_low_neg1=(condition_t(range_t<double>(-DBL_MAX, -35), range_t<double>(0, 100), _T("")));
				condition_low_pos2=(NULL_CONDITION);
				condition_low_neg2=(NULL_CONDITION);

				condition_high_pos1=(condition_t(range_t<double>(60, DBL_MAX), range_t<double>(0, 100), _T("")));
				condition_high_neg1=(condition_t(range_t<double>(-DBL_MAX, -60), range_t<double>(0, 100), _T("")));
				condition_high_pos2=(NULL_CONDITION);
				condition_high_neg2=(NULL_CONDITION);
			}		
			else if (OPTIONS.SPEEDS_RANGE_2().Contains(speed))
			{
				condition_low_pos1=(condition_t(range_t<double>(25, DBL_MAX), range_t<double>(0, 100), _T("")));
				condition_low_neg1=(condition_t(range_t<double>(-DBL_MAX, -25), range_t<double>(0, 100), _T("")));
				condition_low_pos2=(condition_t(range_t<double>(30, DBL_MAX), range_t<double>(100, 150), _T("")));
				condition_low_neg2=(condition_t(range_t<double>(-DBL_MAX, -30), range_t<double>(100, 150), _T("")));

				condition_high_pos1=(condition_t(range_t<double>(50, DBL_MAX), range_t<double>(0, 100), _T("")));
				condition_high_neg1=(condition_t(range_t<double>(-DBL_MAX, -50), range_t<double>(0, 100), _T("")));
				condition_high_pos2=(condition_t(range_t<double>(80, DBL_MAX), range_t<double>(100, 150), _T("")));
				condition_high_neg2=(condition_t(range_t<double>(-DBL_MAX, -80), range_t<double>(100, 150), _T("")));
			}
			else if (OPTIONS.SPEEDS_RANGE_3().Contains(speed))
			{
				condition_low_pos1=(condition_t(range_t<double>(30, DBL_MAX), range_t<double>(0, 150), _T("")));
				condition_low_neg1=(condition_t(range_t<double>(-DBL_MAX, -30), range_t<double>(0, 150), _T("")));
				condition_low_pos2=(condition_t(range_t<double>(45, DBL_MAX), range_t<double>(150, 200), _T("")));
				condition_low_neg2=(condition_t(range_t<double>(-DBL_MAX, -45), range_t<double>(150, 200), _T("")));

				condition_high_pos1=(condition_t(range_t<double>(60, DBL_MAX), range_t<double>(0, 150), _T("")));
				condition_high_neg1=(condition_t(range_t<double>(-DBL_MAX, -60), range_t<double>(0, 150), _T("")));
				condition_high_pos2=(condition_t(range_t<double>(90, DBL_MAX), range_t<double>(150, 200), _T("")));
				condition_high_neg2=(condition_t(range_t<double>(-DBL_MAX, -90), range_t<double>(150, 200), _T("")));
			}
			else
			{
				//нет отступления
				condition_low_pos1=(NULL_CONDITION);
				condition_low_neg1=(NULL_CONDITION);			
				condition_low_pos2=(NULL_CONDITION);
				condition_low_neg2=(NULL_CONDITION);

				condition_high_pos1=(NULL_CONDITION);
				condition_high_neg1=(NULL_CONDITION);			
				condition_high_pos2=(NULL_CONDITION);
				condition_high_neg2=(NULL_CONDITION);
			}
		}
		plan_ovr_low_pos1.Input(ds.GetParameter(PRM_JAGGY_PLAN).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_low_pos1);
		plan_ovr_low_neg1.Input(ds.GetParameter(PRM_JAGGY_PLAN).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_low_neg1);
		plan_ovr_low_pos2.Input(ds.GetParameter(PRM_JAGGY_PLAN).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_low_pos2);
		plan_ovr_low_neg2.Input(ds.GetParameter(PRM_JAGGY_PLAN).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_low_neg2);

		plan_ovr_high_pos1.Input(ds.GetParameter(PRM_JAGGY_PLAN).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_high_pos1);
		plan_ovr_high_neg1.Input(ds.GetParameter(PRM_JAGGY_PLAN).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_high_neg1);
		plan_ovr_high_pos2.Input(ds.GetParameter(PRM_JAGGY_PLAN).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_high_pos2);
		plan_ovr_high_neg2.Input(ds.GetParameter(PRM_JAGGY_PLAN).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_high_neg2);
	}	
	AddOversteps(plan_ovr_low_pos1, DEGREE_LOW);
	AddOversteps(plan_ovr_low_neg1, DEGREE_LOW);
	AddOversteps(plan_ovr_low_pos2, DEGREE_LOW);
	AddOversteps(plan_ovr_low_neg2, DEGREE_LOW);

	AddOversteps(plan_ovr_high_pos1, DEGREE_HIGH);
	AddOversteps(plan_ovr_high_neg1, DEGREE_HIGH);
	AddOversteps(plan_ovr_high_pos2, DEGREE_HIGH);
	AddOversteps(plan_ovr_high_neg2, DEGREE_HIGH);	

	//Сортируем отступления
	std::sort(records.begin(), records.end());

	//Вычисляем уклоны отвода	
	for (size_t i=0; i<records.size(); ++i)
	{
		records.at(i).CalcOtvod(ds, PRM_JAGGY_PLAN, 1000.0);
	}
	*/
	return true;
}

/// Поиск
size_t VJaggyPlanOverstepList::FindGreater(double in_abscoord)
{
	class find_greater
	{
	private:
		double abscoord;
	public:
		find_greater(double in_abscoord) : abscoord(in_abscoord) {};
		bool operator()(const VJaggyOverstep& in_overstep)
		{
			return in_overstep.GetEnd()>=abscoord;
		}
	};
	vector<VJaggyOverstep>::iterator i=find_if(records.begin(), records.end(), find_greater(in_abscoord));
	if (i!=records.end())
		return i-records.begin();
	else
		return BAD_INDEX;
}

/// Поиск
size_t VJaggyPlanOverstepList::FindLess(double in_abscoord)
{
	class find_less
	{
	private:
		double abscoord;
	public:
		find_less(double in_abscoord) : abscoord(in_abscoord) {};
		bool operator()(const VJaggyOverstep& in_overstep)
		{
			return in_overstep.GetEnd()<=abscoord;
		}
	};
	vector<VJaggyOverstep>::iterator i=find_if(records.begin(), records.end(), find_less(in_abscoord));
	if (i!=records.end())
		return i-records.begin();
	else
		return BAD_INDEX;
}

//-------------------------------- Методы класса VJaggyProfileOverstepList -----------------------------------------

/// Добавляет отступления
void VJaggyProfileOverstepList::AddOversteps(const VFindOverstep& in_oversteps)
{
	/*
	for (size_t i=0; i<in_oversteps.GetCount(); ++i)
	{		
		AddItem(in_oversteps.GetItem(i));
	}
	*/
}

/// Загрузка
bool VJaggyProfileOverstepList::Load(VDataSet& ds)
{
	/*
	if (this->IsLoaded())
		return true;
	
	if (!ds.GetParameter(PRM_JAGGY_PROFILE).IsLoaded())
		return false;

	//Находим отступления	

	VFindOverstep profile_ovr_low_pos1;
	VFindOverstep profile_ovr_low_neg1;
	VFindOverstep profile_ovr_low_pos2;
	VFindOverstep profile_ovr_low_neg2;	

	VFindOverstep profile_ovr_high_pos1;
	VFindOverstep profile_ovr_high_neg1;
	VFindOverstep profile_ovr_high_pos2;
	VFindOverstep profile_ovr_high_neg2;	

	condition_t condition_low_pos1;
	condition_t condition_low_neg1;
	condition_t condition_low_pos2;
	condition_t condition_low_neg2;	

	condition_t condition_high_pos1;
	condition_t condition_high_neg1;
	condition_t condition_high_pos2;
	condition_t condition_high_neg2;	

	int current_speed=0;
	
	for (size_t i=0; i<ds.GetParameter(PRM_JAGGY_PROFILE).GetCount(); ++i)
	{
		//В зависимости от скорости меняем условия
		int speed=ds.GetAprinfo().GetPassSpeed(ds.GetAbsCoord().GetItem(i));
		if (speed!=current_speed)
		{
			current_speed=speed;
			if (OPTIONS.SPEEDS_RANGE_1().Contains(speed))
			{
				condition_low_pos1=(condition_t(range_t<double>(40, DBL_MAX), range_t<double>(0, 100), _T("")));
				condition_low_neg1=(condition_t(range_t<double>(-DBL_MAX, -40), range_t<double>(0, 100), _T("")));
				condition_low_pos2=(NULL_CONDITION);
				condition_low_neg2=(NULL_CONDITION);

				condition_high_pos1=(condition_t(range_t<double>(70, DBL_MAX), range_t<double>(0, 100), _T("")));
				condition_high_neg1=(condition_t(range_t<double>(-DBL_MAX, -70), range_t<double>(0, 100), _T("")));
				condition_high_pos2=(NULL_CONDITION);
				condition_high_neg2=(NULL_CONDITION);
			}		
			else if (OPTIONS.SPEEDS_RANGE_2().Contains(speed))
			{
				condition_low_pos1=(condition_t(range_t<double>(30, DBL_MAX), range_t<double>(0, 100), _T("")));
				condition_low_neg1=(condition_t(range_t<double>(-DBL_MAX, -30), range_t<double>(0, 100), _T("")));
				condition_low_pos2=(condition_t(range_t<double>(40, DBL_MAX), range_t<double>(100, 150), _T("")));
				condition_low_neg2=(condition_t(range_t<double>(-DBL_MAX, -40), range_t<double>(100, 150), _T("")));

				condition_high_pos1=(condition_t(range_t<double>(60, DBL_MAX), range_t<double>(0, 100), _T("")));
				condition_high_neg1=(condition_t(range_t<double>(-DBL_MAX, -60), range_t<double>(0, 100), _T("")));
				condition_high_pos2=(condition_t(range_t<double>(90, DBL_MAX), range_t<double>(100, 150), _T("")));
				condition_high_neg2=(condition_t(range_t<double>(-DBL_MAX, -90), range_t<double>(100, 150), _T("")));
			}
			else if (OPTIONS.SPEEDS_RANGE_3().Contains(speed))
			{
				condition_low_pos1=(condition_t(range_t<double>(35, DBL_MAX), range_t<double>(0, 150), _T("")));
				condition_low_neg1=(condition_t(range_t<double>(-DBL_MAX, -35), range_t<double>(0, 150), _T("")));
				condition_low_pos2=(condition_t(range_t<double>(50, DBL_MAX), range_t<double>(150, 200), _T("")));
				condition_low_neg2=(condition_t(range_t<double>(-DBL_MAX, -50), range_t<double>(150, 200), _T("")));

				condition_high_pos1=(condition_t(range_t<double>(70, DBL_MAX), range_t<double>(0, 150), _T("")));
				condition_high_neg1=(condition_t(range_t<double>(-DBL_MAX, -70), range_t<double>(0, 150), _T("")));
				condition_high_pos2=(condition_t(range_t<double>(100, DBL_MAX), range_t<double>(150, 200), _T("")));
				condition_high_neg2=(condition_t(range_t<double>(-DBL_MAX, -100), range_t<double>(150, 200), _T("")));
			}
			else
			{
				//нет отступления
				condition_low_pos1=(NULL_CONDITION);
				condition_low_neg1=(NULL_CONDITION);			
				condition_low_pos2=(NULL_CONDITION);
				condition_low_neg2=(NULL_CONDITION);

				condition_high_pos1=(NULL_CONDITION);
				condition_high_neg1=(NULL_CONDITION);			
				condition_high_pos2=(NULL_CONDITION);
				condition_high_neg2=(NULL_CONDITION);
			}
		}
		profile_ovr_low_pos1.Input(ds.GetParameter(PRM_JAGGY_PROFILE).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_low_pos1);
		profile_ovr_low_neg1.Input(ds.GetParameter(PRM_JAGGY_PROFILE).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_low_neg1);
		profile_ovr_low_pos2.Input(ds.GetParameter(PRM_JAGGY_PROFILE).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_low_pos2);
		profile_ovr_low_neg2.Input(ds.GetParameter(PRM_JAGGY_PROFILE).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_low_neg2);

		profile_ovr_high_pos1.Input(ds.GetParameter(PRM_JAGGY_PROFILE).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_high_pos1);
		profile_ovr_high_neg1.Input(ds.GetParameter(PRM_JAGGY_PROFILE).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_high_neg1);
		profile_ovr_high_pos2.Input(ds.GetParameter(PRM_JAGGY_PROFILE).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_high_pos2);
		profile_ovr_high_neg2.Input(ds.GetParameter(PRM_JAGGY_PROFILE).GetItem(i), ds.GetAbsCoord().GetItem(i), condition_high_neg2);
	}	
	AddOversteps(profile_ovr_low_pos1);
	AddOversteps(profile_ovr_low_neg1);
	AddOversteps(profile_ovr_low_pos2);
	AddOversteps(profile_ovr_low_neg2);


	AddOversteps(profile_ovr_high_pos1);
	AddOversteps(profile_ovr_high_neg1);
	AddOversteps(profile_ovr_high_pos2);
	AddOversteps(profile_ovr_high_neg2);

	//Сортируем отступления
	std::sort(records.begin(), records.end());

	
	//Вычисляем уклоны отвода	
	for (size_t i=0; i<records.size(); ++i)
	{
		records.at(i).CalcOtvod(ds, PRM_JAGGY_PROFILE, 10.0);
	}
	*/
	return true;
}

/// Поиск
size_t VJaggyProfileOverstepList::FindGreater(double in_abscoord)
{
	class find_greater
	{
	private:
		double abscoord;
	public:
		find_greater(double in_abscoord) : abscoord(in_abscoord) {};
		bool operator()(const VJaggyOverstep& in_overstep)
		{
			return in_overstep.GetEnd()>=abscoord;
		}
	};
	vector<VJaggyOverstep>::iterator i=find_if(records.begin(), records.end(), find_greater(in_abscoord));
	if (i!=records.end())
		return i-records.begin();
	else
		return BAD_INDEX;
}

/// Поиск
size_t VJaggyProfileOverstepList::FindLess(double in_abscoord)
{
	class find_less
	{
	private:
		double abscoord;
	public:
		find_less(double in_abscoord) : abscoord(in_abscoord) {};
		bool operator()(const VJaggyOverstep& in_overstep)
		{
			return in_overstep.GetEnd()<=abscoord;
		}
	};
	vector<VJaggyOverstep>::iterator i=find_if(records.begin(), records.end(), find_less(in_abscoord));
	if (i!=records.end())
		return i-records.begin();
	else
		return BAD_INDEX;
}


//-------------------------------- Методы класса VOversteps -----------------------------------------

/// Загрузка
bool VOversteps::Load(VDataSet& ds)
{
	if (this->IsLoaded())
		return true;
	
	ds.Load(PRM_WIDTH);			
	ds.Load(PRM_UNI_WEAR);
	ds.Load(PRM_PIT_LEFT);
	ds.Load(PRM_PIT_RIGHT);
	ds.Load(PRM_LEVEL);
	ds.Load(PRM_RICH_LEFT);
	ds.Load(PRM_RICH_RIGHT);

	int direction=ds.GetMoveDirection();

	VRTOverstepDetector detector(ds.GetAprinfo());
	
	//Симулируем проезд
	int first=direction>0 ? 0 : ds.GetParameter(PRM_WIDTH).GetCount()-1;
	int last=direction>0 ? ds.GetParameter(PRM_WIDTH).GetCount()-1 : 0;
	int step=direction;
	for (size_t i=first; i!=last; i+=step)
	{
		VDataRecord record;

		//Получаем пакет измерений
		record.coord = ds.GetAbsCoord().GetItem(i);
		record.width = ds.GetParameter(PRM_WIDTH).GetItem(i);
		record.wear_left = ds.GetParameter(PRM_WEAR_LEFT).IsLoaded() ? ds.GetParameter(PRM_WEAR_LEFT).GetItem(i) : 0;
		record.wear_right = ds.GetParameter(PRM_WEAR_RIGHT).IsLoaded() ? ds.GetParameter(PRM_WEAR_RIGHT).GetItem(i) : 0;	
		record.pit_left = ds.GetParameter(PRM_PIT_LEFT).GetItem(i);
		record.pit_right = ds.GetParameter(PRM_PIT_RIGHT).GetItem(i);		
		record.level = ds.GetParameter(PRM_LEVEL).GetItem(i);
		record.rich_left = ds.GetParameter(PRM_RICH_LEFT).GetItem(i);
		record.rich_right = ds.GetParameter(PRM_RICH_RIGHT).GetItem(i);
		record.width_corrupt = ds.GetCorrupt().IsCorrupt(i, PRM_WIDTH);
		record.wear_left_corrupt = ds.GetCorrupt().IsCorrupt(i, PRM_WEAR_LEFT);
		record.wear_right_corrupt = ds.GetCorrupt().IsCorrupt(i, PRM_WEAR_RIGHT);
		record.pit_left_corrupt = ds.GetCorrupt().IsCorrupt(i, PRM_PIT_LEFT);
		record.pit_right_corrupt = ds.GetCorrupt().IsCorrupt(i, PRM_PIT_RIGHT);
		record.level_corrupt = ds.GetCorrupt().IsCorrupt(i, PRM_LEVEL);
		record.rich_left_corrupt = ds.GetCorrupt().IsCorrupt(i, PRM_RICH_LEFT);
		record.rich_right_corrupt = ds.GetCorrupt().IsCorrupt(i, PRM_RICH_RIGHT);

		detector.Put(record);
		while (detector.Ready())
		{			
			AddItem(detector.Front());
			detector.Pop();
		}
	}

	detector.Flush();
	while (!detector.Empty())
	{			
		AddItem(detector.Front());
		detector.Pop();
	}
	//Проверяем упорядоченны ли отступления
	for (size_t i=1; i<records.size(); ++i)
	{
		if (direction>0 && records.at(i-1)>records.at(i) || direction<0 && records.at(i-1)<records.at(i))
			LOG_ERROR(boost::str(boost::wformat(_T("Отступления не отсортированы: %f[%s] : %f[%s]")) %
			records.at(i-1).GetCoord() % records.at(i-1).GetDescription() % 
			records.at(i).GetCoord() % records.at(i).GetDescription()));
	}
	//Сортируем отступления
	std::sort(records.begin(), records.end());
	this->SetLoaded(true);
	return true;
}

/// Поиск
size_t VOversteps::FindGreater(double in_abscoord)
{	
	vector<VBasicOverstep>::iterator i=find_if(records.begin(), records.end(), find_greater(in_abscoord));
	if (i!=records.end())
		return i-records.begin();
	else
		return BAD_INDEX;
}

/// Поиск
size_t VOversteps::FindLess(double in_abscoord)
{	
	vector<VBasicOverstep>::iterator i=find_if(records.begin(), records.end(), find_less(in_abscoord));
	if (i!=records.end())
		return i-records.begin();
	else
		return BAD_INDEX;
}
