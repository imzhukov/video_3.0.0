#include <float.h>
#include "VProfile.h"
#include "VAprinfo.h"
#include "mathlib.h"
#include "VFileMark.h"
#include "VDataSet.h"

#include "Shiny.h"

bool VProfileMarkList::Load(const VBasicParameter & data,
							const VAbsCoordData & absCoord, 
							const VAprinfo & aprInfo)
{
	if ((data.GetVersion() == 0 || data.GetVersion() == 1) 
		&& data.GetRecSize() == sizeof(VOldProfileMark))
	{
		for (register size_t i = 0; i < data.GetRecCount(); ++i)
		{
			VProfileMark mark;
			mark.absCoord = aprInfo.GetAbsCoord(data.GetItem<VOldProfileMark>(i).wc);
			mark.index = absCoord.FindIndex(mark.absCoord, true);
			mark.value = data.GetItem<VOldProfileMark>(i).value;
			mark.isCurve=false;
			this->records.push_back(mark);
		}
	}
	else if (data.GetVersion() == 2 && data.GetRecSize() == sizeof(VFileMark))
	{
		for (register size_t i = 0; i < data.GetRecCount(); ++i)
		{
			const VFileMark & file_mark = data.GetItem<VFileMark>(i);

			if (file_mark.mark_type != FILE_LABEL_PROF_LINE &&
				file_mark.mark_type != FILE_LABEL_PROF_CRV)
			{
				this->message = ERRMSG_RECORD_INCORRECT;
				this->SetLoaded(false);
				return false;
			}

			VProfileMark mark;
			mark.index = file_mark.index;
			mark.absCoord = absCoord.GetItem(mark.index);
			mark.value = file_mark.value;
			mark.isCurve = (file_mark.mark_type == FILE_LABEL_PROF_CRV);
			this->records.push_back(mark);
		}
	}

	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
}

// Сохранение
bool VProfileMarkList::Save(VMainFile & file, uint32_t type, 
							const VAbsCoordData & absCoord, 
							const VAprinfo & aprInfo)
{
	// абс.к-та не загружена
	if (!absCoord.IsLoaded())
	{
		return false;
	}

	// апринфо не загружена
	if (!aprInfo.IsLoaded())
	{
		return false;
	}

	// при записи пустого блока нужно просто удалить старый
	if (this->GetCount() == 0) 
	{
		this->DeleteFromFile(file, type);
		this->message = ERRMSG_OK;
		return true;
	}

	VBasicParameter data;
	size_t position = 0;

	data.Create(type, VCT_USER, this->GetCount(), sizeof(VFileMark), 2);
	for (size_t i = 0; i < this->GetCount(); ++i)
	{
		VFileMark mark;
		mark.index = this->GetItem(i).index;
		mark.mark_type = this->GetItem(i).isCurve ? FILE_LABEL_PROF_CRV : FILE_LABEL_PROF_LINE;
		mark.value = this->GetItem(i).value;
		data.Write<VFileMark>(mark, position);
	}

	return data.Save(file, data.GetRecCount(), data.GetRecSize());
}

// Копирование из другого списка, с пересчетом координат
bool VProfileMarkList::CopyFrom(const VProfileMarkList & src, 
								const VAbsCoordData & srcAbs, 
								const VAbsCoordData & destAbs)
{
	this->Unload();
	for (size_t itemNdx = 0; itemNdx < src.GetCount(); ++itemNdx)
	{
		VProfileMark mark = src.GetItem(itemNdx);
		if (mark.UpdateIndex(destAbs, srcAbs))
		{
			this->records.push_back(mark);
		}
	}
	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
}

bool operator < (const VAprinfoProfLabel & a1, const VAprinfoProfLabel & a2)
{
	return a1.coord < a2.coord;
}

/// Конструктор
VProfile::VProfile() : VParameter(), prof_marks_used(false), distance_btw_marks(0), comment(L""){}

/// Конструктор FixME
bool VProfile::Load(const VParameter & grad, const VAbsCoordData & absCoord, const VAprinfo & aprInfo)
{
	if (this->IsLoaded()) return true;

	if (!grad.IsLoaded() || !absCoord.IsLoaded() || !aprInfo.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	// отметки профиля
	std::vector<VAprinfoProfLabel> aprLabels = aprInfo.GetProfLabels();
	std::sort(aprLabels.begin(), aprLabels.end());
	std::vector<float> values;
	std::vector<size_t> indices;

	if (!aprLabels.empty())
		distance_btw_marks=std::max<double>(aprLabels.front().coord-absCoord.GetItem(0), absCoord.GetLastItem()-aprLabels.back().coord);

	for (register size_t i = 0; i < aprLabels.size(); ++i)
	{		
		if (i>0)
			distance_btw_marks=std::max<double>(distance_btw_marks, aprLabels[i].coord-aprLabels[i-1].coord);
		size_t index = absCoord.FindIndex(aprLabels[i].coord, false);
		if (index != BAD_SIZE)
		{
			if (indices.empty() || index!=indices.back())
			{
				values.push_back(aprLabels[i].value);
				indices.push_back(index);
			}
		}
	}	
	if (indices.size() <= 1)
	{
		comment=L"Профиль рассчитан без учета опорных отметок.";
		LOG_WARNING(comment);
	}	
	else if (distance_btw_marks > OPTIONS.GetOption(L"MAX_DISTANCE_BTW_MARKS").GetValue<double>())
	{
		comment=boost::str(boost::wformat(L"Максимальная длина участка без опорных отметок профиля %g м.") % distance_btw_marks);
		LOG_WARNING(comment);
	}

	// массив со значениями профиля
	this->Create(0xFFFFFFFF, absCoord.GetCount(), 0, 0.0f);

	// расчет профиля без учета паспортных отметок
	double value = 0.0;
	this->GetItem(0) = (float) value;
	for (register size_t i = 1; i < this->GetCount(); ++i)
	{
		value += (absCoord.GetItem(i) - absCoord.GetItem(i-1))* 100 * tan(grad.GetItem(i) / 1000.0);//* 100.0 FixMe!!! без перевода в сантиметры
		this->GetItem(i) = (float) value;
	}

	// коррекция профиля по паспортным отметкам
	// -------------------------------------------------------------------------------------
	float offset, coeff;
	size_t labelCnt = indices.size();

	// одна отметка - подъем всего участка
	if (labelCnt == 1)
	{		
		offset = values[0] - this->GetItem(indices[0]);
		for (register size_t i = 0; i < this->GetCount(); ++i)
			this->GetItem(i) += offset;
	}
		
	// две отметки - подъем и разворот всего участка
	else if (labelCnt == 2)
	{		
		offset = values[0] - this->GetItem(indices[0]);
		coeff = ((values[1] - this->GetItem(indices[1])) - offset) / 
			float(absCoord.GetItem(indices[1]) - absCoord.GetItem(indices[0]));

		for (register size_t i = 0; i < this->GetCount(); ++i)
			this->GetItem(i) += 
				offset + coeff * float(absCoord.GetItem(i) - absCoord.GetItem(indices[0]));
	}
		
	// больше двух отметок - подъем и разворот по кускам
	else if (labelCnt >= 3)
	{		
		// первый кусок - от начала файла до второй (!!!) метки
		offset = values[0] - this->GetItem(indices[0]);
		coeff = ((values[1] - this->GetItem(indices[1])) - offset) /
			float(absCoord.GetItem(indices[1]) - absCoord.GetItem(indices[0]));

		for (register size_t i = 0; i < indices[1]; ++i)
			this->GetItem(i) += 
				offset + coeff * float(absCoord.GetItem(i) - absCoord.GetItem(indices[0]));

		// промежуточные куски
		for (register size_t labelNdx = 1; labelNdx < labelCnt-2; ++labelNdx)
		{
			offset = values[labelNdx] - this->GetItem(indices[labelNdx]);
			coeff = ((values[labelNdx+1] - this->GetItem(indices[labelNdx+1])) - offset) / 
				float(absCoord.GetItem(indices[labelNdx+1]) - absCoord.GetItem(indices[labelNdx]));

			for (register size_t i = indices[labelNdx]; i < indices[labelNdx+1]; ++i)
				this->GetItem(i) += 
					offset + coeff * float(absCoord.GetItem(i) - absCoord.GetItem(indices[labelNdx]));
		}

		// последний кусок - от предпоследней (!!!) метки до конца файла
		offset = values[labelCnt-2] - this->GetItem(indices[labelCnt-2]);
		coeff = ((values[labelCnt-1] - this->GetItem(indices[labelCnt-1])) - offset) / 
			float(absCoord.GetItem(indices[labelCnt-1]) - absCoord.GetItem(indices[labelCnt-2]));

		for (register size_t i = indices[labelCnt-2]; i < this->GetCount(); ++i)
			this->GetItem(i) += 
				offset + coeff * float(absCoord.GetItem(i) - absCoord.GetItem(indices[labelCnt-2]));
	}

	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;	
}

/// Возвращает true, если использовались проектные отметки
bool VProfile::ProfileMarksUsed()
{
	return prof_marks_used;
}

/// Возвращает комментарий
std::wstring VProfile::GetComment() const
{
	return comment;
}

//---------------------------- Методы класса VSlopeList ---------------------------------------------------

/// Загрузка
bool VSlopeList::Load(const VParameter & profile, VProfileMarkList & profmarks, const VAbsCoordData & absCoord, const VAprinfo & aprInfo, PARAMETER_TYPE in_data_type)
{
	if (this->IsLoaded()) return true;
	
	ResetCache();

	if (!profile.IsLoaded() || !absCoord.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}
	switch (in_data_type)
	{		
		default://Специальный способ расчета для вычисления неровностей
		{
			if (profmarks.GetCount()>1)
			{
				std::vector<VProfileMark> marks;				
				for (size_t i=0; i<profmarks.GetCount()-1; ++i)
				{
					VProfileMark start=profmarks.GetItem(i);
					VProfileMark end=profmarks.GetItem(i+1);
					if (start.isCurve)
					{
						if (marks.empty() || start!=marks.back())
							marks.push_back(start);
						marks.push_back(end);
					}
					else if (i==0)
					{
						marks.push_back(start);
					}
					else if (i+1==profmarks.GetCount()-1)
					{
						marks.push_back(end);
					}
				}
				if (marks.size()>1)
				{
					for (size_t i=0; i<marks.size()-1; ++i)
					{				
						VSlope slope(marks.at(i), marks.at(i+1), profile);
						AddItem(slope);
					}
				}
			}
			break;
		}
		case PRM_PROFILE_SLOPE_ELEMENT:
		{
			for (size_t i=0; i<profmarks.GetElementCount(); ++i)
			{				
				VSlope slope(profmarks.GetElementBegin(i), profmarks.GetElementEnd(i), profile);
				AddItem(slope);
			}			
			break;
		}
		case PRM_PROFILE_SLOPE_PK:
		{
			std::vector<VPicket> pkList=aprInfo.GetPkList(absCoord.GetItem(0), absCoord.GetLastItem(), true);
			for (size_t i=0; i<pkList.size(); ++i)
			{								
				VSlope slope(absCoord.FindIndex(pkList.at(i).absStart, true), 
					absCoord.FindIndex(pkList.at(i).absEnd, true), profile, absCoord);					
				AddItem(slope);				
			}
			break;
		}
		case PRM_PROFILE_SLOPE_KM:
		{
			std::vector<VAprinfoKmRecord> kmList=aprInfo.GetKmList(absCoord.GetItem(0), absCoord.GetLastItem(), true);
			for (size_t i=0; i<kmList.size(); ++i)
			{				
				VSlope slope(absCoord.FindIndex(kmList.at(i).base, true), absCoord.FindIndex(kmList.at(i).end, true),
					profile, absCoord);				
				AddItem(slope);
			}			
			break;
		}
		case PRM_PROFILE_SLOPE_HPK:
		{
			std::vector<VPicket> pkList=aprInfo.GetPkList(absCoord.GetItem(0), absCoord.GetLastItem(), true);
			for (size_t i=0; i<pkList.size(); ++i)
			{	
				if (pkList.at(i).absEnd-pkList.at(i).absStart > 50)
				{
					double border=(i==0) ? pkList.at(i).absEnd-50 : pkList.at(i).absStart+50;
					VSlope slope1(absCoord.FindIndex(pkList.at(i).absStart, true), 
						absCoord.FindIndex(border, true),
						profile, absCoord);
					AddItem(slope1);
					VSlope slope2(absCoord.FindIndex(border, true), 
						absCoord.FindIndex(pkList.at(i).absEnd, true),
						profile, absCoord);
					AddItem(slope2);
				}
				else
				{
					VSlope slope(absCoord.FindIndex(pkList.at(i).absStart, true), 
						absCoord.FindIndex(pkList.at(i).absEnd, true), profile, absCoord);					
					AddItem(slope);				
				}
			}			
			break;
		}
	}
	return true;
}

/// Обновление
bool VSlopeList::Update(const VParameter & profile, VProfileMarkList & profmarks, const VAbsCoordData & absCoord, const VAprinfo & aprInfo, PARAMETER_TYPE in_data_type)
{
	Unload();
	//FixMe!!! Пока используем неоптимальный способ
	return Load(profile, profmarks, absCoord, aprInfo, in_data_type);
}

/// Находит элемент на котором находится заданная точка
size_t VSlopeList::Find(double in_abscoord) const
{
	class has_point
	{
	private:
		double abscoord;
	public:
		has_point(double in_abscoord){abscoord=in_abscoord;};
		bool operator() (const VSlope& in_slope){return in_slope.HasPoint(abscoord);};
	};	
	std::vector<VSlope>::const_iterator i=std::find_if(records.begin(), records.end(), has_point(in_abscoord));
	return i!=records.end() ? i-records.begin() : BAD_INDEX;
}

///Возвращает индексы элементов которые меняются при изменении точки с заданным номером
std::vector<size_t> VSlopeList::GetAffectedSlopes(size_t in_index) const
{
	std::vector<size_t> result;
	result.push_back((in_index>=2) ? in_index-2 : 0);
	result.push_back((in_index+1<=GetCount()-1) ? in_index+1 : GetCount()-1);
	return result;
}

///Возвращает индексы участка который меняется при изменении точки с заданным номером
std::vector<size_t> VSlopeList::GetAffectedSection(size_t in_max, size_t in_index) const
{
	std::vector<size_t> result;		

	std::vector<size_t> affected_slopes=GetAffectedSlopes(in_index);

	if (affected_slopes.front()==0)
		result.push_back(0);
	else
		result.push_back(GetItem(affected_slopes.front()).GetStartIndex());
	if (affected_slopes.back()+1==GetCount())
		result.push_back(in_max);
	else
		result.push_back(GetItem(affected_slopes.back()).GetEndIndex());
		
	return result;
}

///Возвращает значение профиля
double VSlopeList::GetProfile(double in_abscoord) const
{
	if (GetCount()==0 || in_abscoord<GetItem(0).GetStart().x || in_abscoord>GetLastItem().GetEnd().x)
		return UNDEFINED_VALUE;
	if (cache_i==BAD_INDEX)
		cache_i=Find(in_abscoord);	
	else if (!GetItem(cache_i).HasPoint(in_abscoord))
		cache_i=Find(in_abscoord);
	else if (cache_i+1<GetCount() && GetItem(cache_i+1).HasPoint(in_abscoord))
		cache_i++;
	return cache_i!=BAD_INDEX ? records.at(cache_i).GetProfile(in_abscoord) : UNDEFINED_VALUE;	
}


///Возвращает true если тока находится на кривой
bool VSlopeList::IsCurve(double in_abscoord) const
{
	if (GetCount()==0 || in_abscoord<GetItem(0).GetStart().x || in_abscoord>GetLastItem().GetEnd().x)
		return UNDEFINED_VALUE;
	if (cache_i==BAD_INDEX)
		cache_i=Find(in_abscoord);	
	else if (!GetItem(cache_i).HasPoint(in_abscoord))
		cache_i=Find(in_abscoord);
	else if (cache_i+1<GetCount() && GetItem(cache_i+1).HasPoint(in_abscoord))
		cache_i++;
	return cache_i!=BAD_INDEX ? records.at(cache_i).IsCurve() : false;
}

//--------------------------------- Методы класса VAproxProfile ----------------------------


/// Сглаживание углов кривими Безье
void VAproxProfile::Bezier(const VAbsCoordData & in_abscoord, const VSlopeList& in_slope_list, 
	size_t in_first, size_t in_last)
{
	//Сглаживаем углы кривыми Безье (Эксперимент!!!)
	for (size_t i=in_first+1; i<=in_last; ++i)
	{
		VSlope slope1=in_slope_list.GetItem(i-1);
		VSlope slope2=in_slope_list.GetItem(i);
		//Задаем три опорные точки
		double i0=std::max<double>(slope1.GetEnd().x-35.0, slope1.GetStart().x+slope1.GetLength()/2);
		double i1=slope1.GetEnd().x;
		double i2=std::min<double>(slope2.GetStart().x+35.0, slope2.GetStart().x+slope2.GetLength()/2);
		point_t p0(i0, in_slope_list.GetProfile(i0));
		point_t p1(i1, in_slope_list.GetProfile(i1));
		point_t p2(i2, in_slope_list.GetProfile(i2));

		size_t k0=in_abscoord.FindIndex(i0, true);
		size_t k2=in_abscoord.FindIndex(i2, true);

		for (size_t j=k0; j<=k2; j++)
		{
			this->GetItem(j)=bezier2(p0,p1,p2,
				(in_abscoord.GetItem(j)-i0) / (i2-i0)).y;
		}
	}
}


/// Загрузка
bool VAproxProfile::Load(const VParameter & in_profile, const VAbsCoordData & in_abscoord, 
	const VSlopeList& in_slope_list)
{
	if (this->IsLoaded()) return true;
	
	ResetCache();
	if (!in_profile.IsLoaded() || !in_abscoord.IsLoaded() || !in_slope_list.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	// массив со значениями отклонений
	this->Create(0xFFFFFFFF, in_abscoord.GetCount(), 0, UNDEFINED_VALUE);

	//Заполняем значениями профиля
	for (size_t i=0; i<in_abscoord.GetCount(); ++i)
	{	
		this->GetItem(i)=in_slope_list.GetProfile(in_abscoord.GetItem(i));		
	}
	
	//Сглаживаем переломы
	if (OPTIONS.GetOption(L"BEZIER_ON_PROFILE").Compare(OPTION_YES) && in_slope_list.GetCount()>1)
		Bezier(in_abscoord, in_slope_list, 0, in_slope_list.GetCount()-1);

	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;	
}

/// Обновление
bool VAproxProfile::Update(const VParameter & in_profile, const VAbsCoordData & in_abscoord, 
	const VSlopeList& in_slope_list, size_t in_index)
{
	if (this->IsLoaded() && this->GetCount()==in_profile.GetCount() && this->GetCount()==in_abscoord.GetCount())
	{		
		if (in_slope_list.GetCount()==0)
		{
			this->Unload();
			this->Load(in_profile, in_abscoord, in_slope_list);
		}
		else
		{				
			std::vector<size_t> section=in_slope_list.GetAffectedSection(in_profile.GetCount()-1, in_index);
			for (size_t i=section.front(); i<=section.back(); ++i)
			{			
				this->GetItem(i)=in_slope_list.GetProfile(in_abscoord.GetItem(i));				
			}
						
			std::vector<size_t> slopes=in_slope_list.GetAffectedSlopes(in_index);
			//Сглаживаем переломы
			if (OPTIONS.GetOption(L"BEZIER_ON_PROFILE").Compare(OPTION_YES) && slopes.size()==2 && slopes.front()!=BAD_INDEX && slopes.back()!=BAD_INDEX)
				Bezier(in_abscoord, in_slope_list, (slopes.front()>0) ? slopes.front()-1 : slopes.front(), 
				(slopes.back()+1<in_slope_list.GetCount()) ? slopes.back()+1 : slopes.back());
		
		}
		ResetCache();
		return true;
	}
	else
	{
		return false;
	}
}


//---------------------------- Методы класса VDiffProfile -----------------------------------------

/// Загрузка
bool VDiffProfile::Load(const VParameter & in_profile, const VAproxProfile & in_aprox_profile)
{
	if (this->IsLoaded()) return true;
	
	ResetCache();
	if (!in_profile.IsLoaded() || !in_aprox_profile.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	// массив со значениями отклонений
	this->Create(0xFFFFFFFF, in_profile.GetCount(), 0, UNDEFINED_VALUE);

	size_t first_def=in_aprox_profile.GetFirstDef();
	size_t last_def=in_aprox_profile.GetLastDef();
	for (size_t i=first_def; i<last_def+1; ++i)
	{
		//Пересчитываем в миллиметры
		this->GetItem(i)=10.0*(in_profile.GetItem(i) - in_aprox_profile.GetItem(i));
	}

	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;	
}

/// Обновление
bool VDiffProfile::Update(const VParameter & in_profile,  const VAproxProfile & in_aprox_profile, 
	const VSlopeList& in_slope_list, size_t in_index)
{
	if (this->IsLoaded() && this->GetCount()==in_profile.GetCount() && this->GetCount()==in_aprox_profile.GetCount())
	{		
		if (in_slope_list.GetCount()==0)
		{
			this->Unload();
			this->Create(0xFFFFFFFF, in_profile.GetCount(), 0, UNDEFINED_VALUE);
			ResetCache();
		}
		else
		{			
			std::vector<size_t> section=in_slope_list.GetAffectedSection(in_profile.GetCount()-1, in_index);
			for (size_t i=section.front(); i<=section.back(); ++i)
			{				
				//Пересчитываем в миллиметры
				this->GetItem(i) = in_aprox_profile.GetItem(i)!=UNDEFINED_VALUE ?
					10.0*(in_profile.GetItem(i) - in_aprox_profile.GetItem(i)) : UNDEFINED_VALUE;
			}		
		}
		ResetCache();
		return true;
	}
	else
	{
		return false;
	}
}


//--------------------------------- Методы класса VJaggyProfile ----------------------------

// Загрузка
bool VJaggyProfile::Load(VDataSet& ds)
{
	if (this->IsLoaded()) return true;

	if (!ds.GetParameter(PRM_PROFILE).IsLoaded() || !ds.GetAbsCoord().IsLoaded() || 
		!ds.GetProfMarks().IsLoaded()) 
		return false;

	this->Create(0xFFFFFFFF, ds.GetAbsCoord().GetCount(), 0, UNDEFINED_VALUE);

	//Считаем "фиктивный идеальный профиль" 
	VSlopeList fiction_slopes;
	fiction_slopes.Load(ds.GetParameter(PRM_PROFILE), ds.GetProfMarks(), ds.GetAbsCoord(), ds.GetAprinfo(), PRM_NONE);
	VAproxProfile fiction_profile;
	fiction_profile.Load(ds.GetParameter(PRM_PROFILE), ds.GetAbsCoord(), fiction_slopes);	
	VDiffProfile fiction_diff;
	fiction_diff.Load(ds.GetParameter(PRM_PROFILE), fiction_profile);

	size_t first_def=fiction_diff.GetFirstDef();
	size_t last_def=fiction_diff.GetLastDef();
	int current_speed=100;
	double base=OPTIONS.JAGGY_BASE(current_speed);	
	for (size_t i=first_def; i<last_def+1; ++i)
	{		
		int speed=ds.GetAprinfo().GetPassSpeed(ds.GetAbsCoord().GetItem(i));		
		if (speed!=current_speed)
		{
			current_speed=speed;
			base=OPTIONS.JAGGY_BASE(current_speed);
		}		
		this->GetItem(i)=fiction_diff.GetJaggy(i, base, ds.GetAbsCoord());
	}

	this->message = ERRMSG_OK;
	this->SetLoaded(true);
	return true;
}


//----------------------------------- Методы VJaggyProfileOverstep -----------------------------------


/// Конструктор
VJaggyProfileOverstep::VJaggyProfileOverstep(double in_start, double in_end, double in_coord, double in_value, 
	double in_otvod, JAGGY_PROFILE_SIGNIFICANCE in_significance, const VAprinfo& in_aprinfo)

{
	start=in_start;
	end=in_end;
	coord=in_coord;
	max_value=in_value;
	min_value=in_value;
	value=in_value;
	length=fabs(in_end-in_start);
	type=L"Вертик.нер.";
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
double VJaggyProfileOverstep::GetCoord() const
{
	return coord;
};

/// Возвращает максимальный отвод
double VJaggyProfileOverstep::GetOtvod() const
{
	return otvod;
};

/// Вовращает описание
std::wstring VJaggyProfileOverstep::GetDescription() const
{	
	return boost::str(boost::wformat(L"%s %s") % GetDegreeString() % GetComment());
};	

/// Возвращает строку с описанием степени
std::wstring VJaggyProfileOverstep::GetDegreeString() const
{
	switch (significance)
	{
		case SIGN_2:
			return L"ПД";
		case SIGN_1:
			return L"ТВ";
		default:
			return L"";
	}
};


//----------------------------------------- VJaggyProfileOverstepList -------------------------

/// Конструктор
VJaggyProfileOverstepList::VJaggyProfileOverstepList() : loaded(false) {};


/// Добавляет отступление в список
void VJaggyProfileOverstepList::AddOverstep(double start, double end, double center, double value, double otvod, 
	const VAprinfo& aprinfo)
{	
	double length=end-start;
	VSpeeds speeds=aprinfo.GetSpeeds((start+end)/2);
	if (pro::interval<int>(141,160).contains(speeds.GetMaxSpeed()))
	{					
		if (pro::interval<double>(0,DBL_MAX,true,false).contains(fabs(length)))
		{	
			if (pro::interval<double>(40,70,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyProfileOverstepPtr(new VJaggyProfileOverstep(start, end, center, value, otvod, SIGN_1, aprinfo)));
			}
			if (pro::interval<double>(70,DBL_MAX,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyProfileOverstepPtr(new VJaggyProfileOverstep(start, end, center, value, otvod, SIGN_2, aprinfo)));								
			}
		}
	}
	else if (pro::interval<int>(161,200).contains(speeds.GetMaxSpeed()))
	{
		if (pro::interval<double>(0,100,true,false).contains(fabs(length)))
		{	
			if (pro::interval<double>(30,60,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyProfileOverstepPtr(new VJaggyProfileOverstep(start, end, center, value, otvod, SIGN_1, aprinfo)));								
			}
			if (pro::interval<double>(60,DBL_MAX,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyProfileOverstepPtr(new VJaggyProfileOverstep(start, end, center, value, otvod, SIGN_2, aprinfo)));								
			}
		}
		if (pro::interval<double>(100,DBL_MAX,true,false).contains(fabs(length)))
		{	
			if (pro::interval<double>(40,90,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyProfileOverstepPtr(new VJaggyProfileOverstep(start, end, center, value, otvod, SIGN_1, aprinfo)));								
			}
			if (pro::interval<double>(90,DBL_MAX,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyProfileOverstepPtr(new VJaggyProfileOverstep(start, end, center, value, otvod, SIGN_2, aprinfo)));								
			}
		}
	}
	else if (pro::interval<int>(201,250).contains(speeds.GetMaxSpeed()))
	{
		if (pro::interval<double>(0,150,true,false).contains(fabs(length)))
		{	
			if (pro::interval<double>(35,70,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyProfileOverstepPtr(new VJaggyProfileOverstep(start, end, center, value, otvod, SIGN_1, aprinfo)));								
			}
			if (pro::interval<double>(70,DBL_MAX,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyProfileOverstepPtr(new VJaggyProfileOverstep(start, end, center, value, otvod, SIGN_2, aprinfo)));
				
			}
		}
		if (pro::interval<double>(150,DBL_MAX,true,false).contains(fabs(length)))
		{	
			if (pro::interval<double>(50,100,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyProfileOverstepPtr(new VJaggyProfileOverstep(start, end, center, value, otvod, SIGN_1, aprinfo)));								
			}
			if (pro::interval<double>(100,DBL_MAX,true,false).contains(fabs(value)))
			{
				oversteps.push_back(VJaggyProfileOverstepPtr(new VJaggyProfileOverstep(start, end, center, value, otvod, SIGN_2, aprinfo)));								
			}
		}
	}
}

/// Расчет
void VJaggyProfileOverstepList::Calculate(VDataSet& in_ds)
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
	
	for (size_t i=0; i<in_ds.GetParameter(PRM_JAGGY_PROFILE).GetCount(); ++i)
	{
		VPoint point(in_ds.GetAbsCoord().GetItem(i), in_ds.GetParameter(PRM_JAGGY_PROFILE).GetItem(i));
		int speed=in_ds.GetAprinfo().GetPassSpeed(in_ds.GetAbsCoord().GetItem(i));
		min_value.put(point.GetValue());
		max_value.put(point.GetValue());
		max_otvod.put(fabs(in_ds.GetParameter(PRM_JAGGY_PROFILE).GetOtvod(i, in_ds.GetAbsCoord(), OPTIONS.JAGGY_OTVOD_BASE(speed))));
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
}

/// Загрузка
bool VJaggyProfileOverstepList::Load(VDataSet& in_ds)
{
	if (loaded)
		return true;		
	Calculate(in_ds);
	loaded=true;
	return true;
}

/// Выгрузка
void VJaggyProfileOverstepList::Unload()
{
	oversteps.clear();
	loaded=false;
}

/// Возвращает число записей
size_t VJaggyProfileOverstepList::GetCount() const
{
	return oversteps.size();
}

/// Возвращает запись по номеру
const VJaggyProfileOverstepPtr& VJaggyProfileOverstepList::GetItem(size_t in_index) const
{
	return oversteps.at(in_index);
}