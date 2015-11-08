#pragma once


#include "VParameter.h"
#include "oversteps.h"

class VDataSet;

//--------------------- Отступления по подуклонке --------------------------------

#define CANTING_LEVEL_THRESHOLD 85.0
#define CANTING_1_DIV_12 100.0/12.0
#define CANTING_1_DIV_30 100.0/30.0
#define CANTING_1_DIV_60 100.0/60.0

///Отступление
class VCantingOverstep : public VBasicOverstep
{
private:
	/// Среднее
	double average_value;
public:
	/// Конструктор
	VCantingOverstep(const std::wstring& in_type, double in_start, double in_end, double in_min_value, double in_max_value, double in_average_value);
	/// Вовращает описание
	std::wstring GetDescription() const;
	/// Возвращает среднее
	double GetAverage() const;
};

///Подуклонка
class VCantingOverstepList
{
private:	
	/// Признак того что данные загружены
	bool loaded;	
	/// Список отступлений
	std::deque<VBasicOverstepPtr> oversteps;		
	/// Сторона
	side_t side;
	/// Добавляет отступление в список
	void AddOverstep(double start, double end, double min_value, double max_value, double average_value);
	/// Расчет
	void Calculate(VDataSet& in_ds);	

public:
	/// Конструктор
	VCantingOverstepList(side_t in_side);
	/// Загрузка
	bool Load(VDataSet& in_ds);
	/// Выгрузка
	void Unload();
	/// Возвращает число записей
	size_t GetCount() const;
	/// Возвращает запись по номеру
	const VBasicOverstepPtr& GetItem(size_t in_index) const;
};

/// Внешний износ

///Отступление
class VExtWearOverstep : public VBasicOverstep
{
private:
	/// Среднее
	double average_value;
public:
	/// Конструктор
	VExtWearOverstep(const std::wstring& in_type, double in_start, double in_end, double in_min_value, double in_max_value, double in_average_value);
	/// Вовращает описание
	std::wstring GetDescription() const;
	/// Возвращает среднее
	double GetAverage() const;
};

///Внешний износ
class VExtWearOverstepList
{
private:	
	/// Признак того что данные загружены
	bool loaded;	
	/// Список отступлений
	std::deque<VBasicOverstepPtr> oversteps;		
	/// Сторона
	side_t side;
	/// Добавляет отступление в список
	void AddOverstep(double start, double end, double min_value, double max_value, double average_value);
	/// Расчет
	void Calculate(VDataSet& in_ds);	

public:
	/// Конструктор
	VExtWearOverstepList(side_t in_side);
	/// Загрузка
	bool Load(VDataSet& in_ds);
	/// Выгрузка
	void Unload();
	/// Возвращает число записей
	size_t GetCount() const;
	/// Возвращает запись по номеру
	const VBasicOverstepPtr& GetItem(size_t in_index) const;
};