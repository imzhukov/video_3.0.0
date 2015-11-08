#pragma once

#include <vector>
#include "VParameter.h"
#include "parameters.h"
#include "oversteps.h"

class VMainFile;
class VAprinfo;
class VDataSet;

/// Метка профиля старого формата (в файле)
struct VOldProfileMark
{
	VWayCoord wc;
	double value;				///< Значение профиля
};

/// Метка профиля
struct VProfileMark
{
	uint32_t index;				///< Номер кадра
	double absCoord;			///< Абс.координата 
	double value;				///< Значение профиля
	bool isCurve;				///< Признак типа элемента

	/// Пересчет индексов FixMe!!! убрать
	bool UpdateIndex(const VAbsCoordData & destAbs, const VAbsCoordData & srcAbs)
	{
		return destAbs.UpdateIndex(this->index, srcAbs);
	}
};

inline bool operator < (const VProfileMark & l, const VProfileMark & r)
{
	if (l.index!=BAD_INDEX && r.index!=BAD_INDEX)
		return l.index < r.index;
	else
		return l.absCoord < r.absCoord;
}

inline bool operator==(const VProfileMark & l, const VProfileMark & r)
{
	//Абс. координату не сравниваем
	return l.index==r.index && l.value==r.value && l.isCurve==r.isCurve;
}

inline bool operator!=(const VProfileMark & l, const VProfileMark & r)
{
	//Абс. координату не сравниваем
	return l.index!=r.index || l.value!=r.value || l.isCurve!=r.isCurve;
}

/// Список меток профиля
class VProfileMarkList : public VIndexList<VProfileMark>
{

	/// Загрузка
	bool Load(const VBasicParameter & data, const VAbsCoordData & absCoord, const VAprinfo & aprInfo);

public:

	/// Конструктор
	VProfileMarkList()
	{}

	/// Загрузка
	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, const VAbsCoordData & absCoord, const VAprinfo & aprInfo);

	/// Копирование из другого списка, с пересчетом координат
	bool CopyFrom(const VProfileMarkList & src, 
		const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs);

	/// Сохранение
	bool Save(VMainFile & file, uint32_t type, const VAbsCoordData & absCoord, const VAprinfo & aprInfo);

	/// Пересчет величин меток по указанному профилю
	void UpdateValues(const VParameter & profile)
	{
		for (size_t i = 0; i < this->GetCount(); ++i)
		{
			this->GetItem(i).value = profile.GetItem(this->GetItem(i).index);
		}
	}

	// -----------------------------------------------------------------------------------
	// Элементы профиля
	// -----------------------------------------------------------------------------------

	/// Количество элементов профиля
	size_t GetElementCount()
	{
		if (this->GetCount() > 1) return this->GetCount() - 1;
		else return 0;
	}

	/// Ссылка на метку в начале элемента
	const VProfileMark & GetElementBegin(size_t i)
	{
		return this->GetItem(i);
	}

	/// Ссылка на метку в конце начале элемента
	const VProfileMark & GetElementEnd(size_t i)
	{
		return this->GetItem(i+1);
	}

	/// Находит метку
	size_t FindMark(const VProfileMark& in_mark)
	{
		std::vector<VProfileMark>::iterator i=std::find(records.begin(), records.end(), in_mark);
		if (i!=records.end())
			return i-records.begin();
		else
			return BAD_INDEX;
	};
	
	/// Находит метку слева от указанной координаты
	size_t FindLess(double in_abscoord)
	{
		
		VProfileMark mark;
		mark.index=BAD_INDEX;
		mark.absCoord=in_abscoord;
		std::vector<VProfileMark>::iterator i=std::lower_bound(records.begin(), records.end(), mark);
		if (i!=records.end())
			return i-records.begin();
		else
			return BAD_INDEX;
	};

	/// Находит метку справа от указанной координаты
	size_t FindGreater(double in_abscoord)
	{
		VProfileMark mark;
		mark.index=BAD_INDEX;
		mark.absCoord=in_abscoord;
		std::vector<VProfileMark>::iterator i=std::upper_bound(records.begin(), records.end(), mark);
		if (i!=records.end())
			return i-records.begin();
		else
			return BAD_INDEX;
	}
};

template <class FILETYPE>
bool VProfileMarkList::Load(FILETYPE & file, uint32_t type, 
							const VAbsCoordData & absCoord, const VAprinfo & aprInfo)
{
	if (this->IsLoaded()) return true;
	if (!absCoord.IsLoaded() || !aprInfo.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	VBasicParameter data;
	if (!data.Load(file, type, false))
	{
		this->message = data.GetErrorMessage();
		return false;
	}

	bool result = this->Load(data, absCoord, aprInfo);
	data.UnloadFile(file, type);
	data.Unload();
	return result;
}

/// Профиль 
class VProfile : public VParameter
{
private:
	/// Признак того что использовались проектны отметки
	bool prof_marks_used;
	/// Масимальное расстояние между проектными метками
	double distance_btw_marks;
	/// Комментарий
	std::wstring comment;

public:
	/// Конструктор
	VProfile();
	/// Загрузка. Первый параметр - градиент
	bool Load(const VParameter & grad, const VAbsCoordData & absCoord, const VAprinfo & aprInfo);
	/// Возвращает true, если использовались проектные отметки
	bool ProfileMarksUsed();
	/// Возвращает комментарий
	std::wstring GetComment() const;
};

/** Уклон элемента профиля*/
class VSlope
{
private:
	
	/** Индекс начала элемента в файле*/
	size_t start_index;
	/** Индекс конца элемента в файле*/
	size_t end_index;
	/** Начало*/
	point_t start;
	/** Конец*/
	point_t end;	
	/** Признак кривая/прямая*/
	bool isCurve;	
	/** Координаты центра*/
	point_t center;
	
	/** Вычисляет параметры кривой*/
	void CalcVerticalCurve(const VParameter& in_profile)
	{
		if (isCurve)
		{
			double mid_x=(start.x+end.x)/2;
			double mid_y=in_profile.GetItem((start_index+end_index)/2);
			circle_t c(point_t(start.x, start.y/100), point_t(mid_x, mid_y/100), 
				point_t(end.x, end.y/100));//приходится переводить в метры					
			center.x=c.X();
			center.y=c.Y()*100;//переводим обратно в сантиметры
		}
		else
		{
			center.x=0;
			center.y=0;
		}
	}

public:
		
	/** Конструктор*/
	VSlope(const VProfileMark& in_start, const VProfileMark& in_end, const VParameter& in_profile)
	{
		start_index=in_start.index;
		end_index=in_end.index;
		//Значения профиля и координаты округляются до целых метров и сантиметров
		start=point_t(round(in_start.absCoord), round(in_start.value));
		end=point_t(round(in_end.absCoord), round(in_end.value));		
		isCurve=in_start.isCurve;
		CalcVerticalCurve(in_profile);
	}	

	/** Конструктор*/
	VSlope(size_t in_start_index, size_t in_end_index, const VParameter& in_profile, const VAbsCoordData& in_abscoord, bool in_isCurve=false)
	{
		start_index=in_start_index;
		end_index=in_end_index;
		//Значения профиля округляются до целых сантиметров
		start=point_t(in_abscoord.GetItem(start_index), round(in_profile.GetItem(start_index)));
		end=point_t(in_abscoord.GetItem(end_index), round(in_profile.GetItem(end_index)));
		isCurve=in_isCurve;
		CalcVerticalCurve(in_profile);
	}
	/** Индекс начала элемента в файле*/
	size_t GetStartIndex() const {return start_index;};
	/** Индекс конца элемента в файле*/
	size_t GetEndIndex() const {return end_index;};
	/** Возвращает точку начала*/
	const point_t& GetStart() const {return start;};
	/** Возвращает точку конца*/
	const point_t& GetEnd() const {return end;};
	/** Возвращает длину (с точностью до метра)*/
	double GetLength() const {return round(end.x-start.x);};
	/** Возвращает признак кривая/прямая*/
	bool IsCurve() const {return isCurve;};	
	/** Возвращает радиус (м)*/
	double R() const {return isCurve ? sqrt(pow(start.x-center.x,2)+pow(start.y/100-center.y/100,2)) : 0;};
	/** Возвращает уклон мм/м*/
	double Slope() const {return round(10.0*(end.y-start.y)/(end.x-start.x), 1);};
	/** Возвращает знак вертикальной кривой*/
	double Sign()const {return start.y>center.y ? 1 : -1;};
	/** Возвращает значение профиля в точке*/
	double GetProfile(double in_abscoord) const
	{
		if (HasPoint(in_abscoord))
		{
			if (isCurve)
			{				
				return 100.0*(center.y/100.0 + Sign() * sqrt(pow(R(),2) - pow(in_abscoord - center.x, 2)));
			}
			else
			{				
				return line_t(start, end).Y(in_abscoord);
			}
		}
		else
			return 0;
	};
	///Возвращает true если точка принадлежит элементу
	bool HasPoint(double in_abscoord) const
	{
		return start.x<=in_abscoord && end.x>=in_abscoord;
	}
};

/// Оператор < для VSlope
static bool operator < (const VSlope & l, const VSlope & r)
{
	return l.GetStart().x < r.GetStart().x;
}

/// Уклоны профиля FixMe!!! собственно индексы здесь не используются
class VSlopeList : public VIndexList<VSlope>
{
private:
	///Кэш
	mutable size_t cache_i;	
	///Сброс кэша
	void ResetCache()
	{
		cache_i=BAD_INDEX;
	};
public:
	/// Загрузка
	bool Load(const VParameter & profile, VProfileMarkList & profmarks, const VAbsCoordData & absCoord, const VAprinfo & aprInfo, PARAMETER_TYPE in_data_type);
	/// Обновление
	bool Update(const VParameter & profile, VProfileMarkList & profmarks, const VAbsCoordData & absCoord, const VAprinfo & aprInfo, PARAMETER_TYPE in_data_type);	
	/// Находит элемент на котором находится заданная точка
	size_t Find(double in_abscoord) const;
	///Возвращает индексы элементов которые меняются при изменении точки с заданным номером
	std::vector<size_t> GetAffectedSlopes(size_t in_index) const;
	///Возвращает индексы участка который меняется при изменении точки с заданным номером
	std::vector<size_t> GetAffectedSection(size_t in_max, size_t in_index) const;
	///Возвращает значение профиля
	double GetProfile(double in_abscoord) const;
	///Возвращает true если тока находится на кривой
	bool IsCurve(double in_abscoord) const;
};

/// Аппроксимированный профиль
class VAproxProfile : public VParameter
{
private:
	/// Сглаживание углов кривими Безье
	void Bezier(const VAbsCoordData & in_abscoord, const VSlopeList& in_slope_list, size_t in_first, size_t in_last);
public:
	/// Загрузка
	bool Load(const VParameter & in_profile, const VAbsCoordData & in_abscoord, 
		const VSlopeList& in_slope_list);
	/// Обновление
	bool Update(const VParameter & in_profile, const VAbsCoordData & in_abscoord, 
		const VSlopeList& in_slope_list, size_t in_index);
};

/// Отклонения профиля от идеального FixMe!!! подобрать термин
class VDiffProfile : public VParameter
{
private:	
public:

	/// Загрузка
	bool Load(const VParameter & in_profile, const VAproxProfile & in_aprox_profile);

	/// Обновление
	bool Update(const VParameter & in_profile,  const VAproxProfile & in_aprox_profile,
		const VSlopeList& in_slope_list, size_t in_index);
};

/// Неровности профиля (отклонение от огибающей)
class VJaggyProfile : public VParameter
{
public:

	/// Загрузка
	bool Load(VDataSet& ds);

};

/// Степень опасности
enum JAGGY_PROFILE_SIGNIFICANCE {SIGN_1=1, SIGN_2=2};

/// Сверхнормативная неровность
class VJaggyProfileOverstep : public VBasicOverstep
{
private:
	/// Степень опасности
	JAGGY_PROFILE_SIGNIFICANCE significance;
	/// Координата FixMe!!! в базовый класс
	double coord;
	/// Крутизна отвода
	double otvod;
	/// Признак моста
	bool on_bridge;
	/// Признак стрелки
	bool on_switch;
	/// Признак станции
	bool on_station;

public:
	/// Конструктор
	VJaggyProfileOverstep(double in_start, double in_end, double in_coord, double in_value, 
		double in_otvod, JAGGY_PROFILE_SIGNIFICANCE in_significance, const VAprinfo& in_aprinfo);
	/// Возвращает координату
	double GetCoord() const;
	/// Возвращает максимальный отвод
	double GetOtvod() const;
	/// Вовращает описание
	std::wstring GetDescription() const;
	/// Возвращает строку с описанием степени
	std::wstring GetDegreeString() const;
};
typedef boost::shared_ptr<VJaggyProfileOverstep> VJaggyProfileOverstepPtr;


/// Сверхнормативные неровности профиля
class VJaggyProfileOverstepList
{
private:
	/// Признак того что данные загружены
	bool loaded;	
			
	/// Список отступлений
	std::deque<VJaggyProfileOverstepPtr> oversteps;	
	
	/// Добавляет отступление в список
	void AddOverstep(double start, double end, double center, double value, double otvod, const VAprinfo& aprinfo);	
	/// Расчет
	void Calculate(VDataSet& in_ds);

public:
	/// Конструктор
	VJaggyProfileOverstepList();
	/// Загрузка
	bool Load(VDataSet& in_ds);
	/// Выгрузка
	void Unload();
	/// Возвращает число записей
	size_t GetCount() const;
	/// Возвращает запись по номеру
	const VJaggyProfileOverstepPtr& GetItem(size_t in_index) const;
};