#pragma once

#include "VParameter.h"
#include "oversteps.h"

class VDataSet;

/// Разность ШК с нагрузкой и без 
class VDiffWidth : public VParameter
{
public:
	/// Загрузка
	bool Load(VParameter & in_width, VParameter & in_width_front, VAbsCoordData & in_abscoord);
};


/// Отжатие
class VDiffWidthOverstep : public VBasicOverstep
{
private:
	/// Координата
	double coord;
public:
	/// Конструктор
	VDiffWidthOverstep(const VExcess& in_excess, const VAprinfo& in_aprinfo);
	/// Вовращает описание
	std::wstring GetDescription() const;
	/// Возвращает координату	
	double GetCoord() const;
};


/// Отжатия
class VDiffWidthOverstepList
{
private:	
	/// Признак того что данные загружены
	bool loaded;	
	/// Список отступлений
	std::deque<VBasicOverstepPtr> oversteps;		
	
	/// Добавляет отступление в список
	void AddOverstep(const VExcess& in_excess, const VAprinfo& in_aprinfo);
	/// Расчет
	void Calculate(VDataSet& in_ds, PARAMETER_TYPE in_data_type);	

public:
	/// Конструктор
	VDiffWidthOverstepList();
	/// Загрузка
	bool Load(VDataSet& in_ds, PARAMETER_TYPE in_data_type);
	/// Выгрузка
	void Unload();
	/// Возвращает число записей
	size_t GetCount() const;
	/// Возвращает запись по номеру
	const VBasicOverstepPtr& GetItem(size_t in_index) const;
};


