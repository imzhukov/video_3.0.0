#pragma once

#include "mathlib.h"
#include "processing.h"

/// Кадр данных
class VPoint : public point_t
{
private:
	/// Признак сбоя
	bool corrupt;	
public:
	/// Конструктор
	VPoint();
	/// Конструктор
	VPoint(double in_coord, double in_value);
	/// Конструктор
	VPoint(double in_coord, double in_value, bool in_corrupt);
	/// Возвращает координату
	double GetCoord() const;
	/// Возвращает значение
	double GetValue() const;	
	/// Возвращает признак сбоя
	bool GetCorrupt() const;	
	/// Возвращает расстояние до другой точки FixMe!!! лучше сделать свободной функцией
	double Distance(const VPoint& in_point) const;
};


/// Экстремум
class VExtremum
{
private:
	/// Экстремум
	VPoint extremum;	
	/// Доп. точки (точки между данным экстремумом и предыдущим)
	std::vector<VPoint> extra;
	/// Признак наличия сбоев
	int corrupt_count;
public:
	/// Конструктор
	VExtremum(const VPoint& in_extremum);	
	/// Конструктор
	VExtremum(const VPoint& in_extremum, const std::vector<VPoint>& in_extra);
	/// Возвращает координату
	double GetCoord() const;
	/// Возвращает значение параметра
	double GetValue() const;		
	/// Возвращает экстремум
	const VPoint& GetExtremum() const;	
	/// Возвращает доп. точки
	const std::vector<VPoint>& GetExtra() const;
	/// Возвращает признак сбоя
	bool GetCorrupt() const;
};

/// Пара экстремумов
class VWave
{
private:
	/// Начало
	VExtremum start;
	/// Конец
	VExtremum end;
public:
	/// Конструктор
	VWave(const VExtremum& in_start, const VExtremum& in_end);	
	/// Возвращает начало
	const VExtremum& GetStart() const;
	/// Возвращает конец
	const VExtremum& GetEnd() const;
	/// Возвращает величину
	const double GetValue() const;
	/// Возвращает знак
	const double GetSign() const;
	/// Возвращает длину
	const double GetLength() const;
	/// Возвращает доп. точки
	const std::vector<VPoint>& GetExtra() const;
	/// Возвращает максимальную величину (с учетом доп. точек)
	const double GetValue(double in_distance) const;	
	/// Возвращает признак сбоя
	bool GetCorrupt() const;
};

/// Превышение порога
class VExcess
{
protected:
	/// Точка начала
	VPoint start;
	/// Точка конца
	VPoint end;	
	/// Точка максимума
	VPoint max_value;
	/// Точка минимума
	VPoint min_value;
	/// Сумма значений
	double sum;
	/// Количество значений
	int count;
	/// Количество сбойных значений
	int corrupt_count;
public:
	/// Конструктор
	VExcess(const std::deque<VPoint>& in_data);
	/// Возвращает координату начала
	double GetStart() const;
	/// Возвращает координату конца
	double GetEnd() const;
	/// Возвращает координату
	double GetCoord() const;
	/// Возвращает максимальное значение
	double GetMax() const;
	/// Возвращает минимальное
	double GetMin() const;
	/// Возвращает координату максимального значения
	double GetMaxCoord() const;
	/// Возвращает координату минимального значения
	double GetMinCoord() const;
	/// Возвращает среднее
	double GetAverage() const;
	/// Возвращает длину
	double GetLength() const;
	/// Возвращает сумму значений
	double GetSum() const;
	/// Возвращает количество сбойных значений
	double GetCorruptCount() const;
	/// Возвращает признак сбоя
	bool GetCorrupt() const;
};

/// Вычисляет отвод
class VFindOtvod : public VDataProcessor<VPoint, VPoint>
{
private:
	/// База
	double base;	
	/// Очередь данных на входе
	std::deque<VPoint> input;
	/// Очередь данных на выходе
	std::queue<VPoint> output;
public:	
	/// Конструктор
	VFindOtvod(double in_base);
	/// Деструктор
	~VFindOtvod();
	/// Помещает данные на вход
	virtual void Put(const VPoint& in_point);
	/// Возвращает true, если есть обработанные данные
	virtual bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VPoint& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();
	/// Устанавливает новую базу
	void SetBase(double in_base);
};


/// Находит экстремумы
class VFindExtremum : public VDataProcessor<VPoint, VExtremum>
{
private:
	/// База
	double base;	
	/// Очередь данных на входе
	std::deque<VPoint> input;
	/// Очередь данных на выходе
	std::queue<VExtremum> output;
	/// Очередь дополнительных точек
	std::vector<VPoint> extra;

	/// Возвращает true, если середина входного буфера - является экстремумом
	bool IsExtremum() const;
	
public:
	/// Конструктор
	VFindExtremum(double in_base);
	/// Деструктор
	~VFindExtremum();
	/// Ввод данных
	void Put(const VPoint& in_point);	
	/// Возвращает true, если есть обработанные данные
	virtual bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VExtremum& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();
};

/// Задержка
class VDelay : public VDataProcessor<VPoint, VPoint>
{
private:
	/// База
	double base;	
	/// Очередь данных на входе
	std::deque<VPoint> input;
	/// Очередь данных на выходе
	std::queue<VPoint> output;
public:
	/// Конструктор
	VDelay(double in_base);
	/// Деструктор
	~VDelay();
	/// Ввод данных
	void Put(const VPoint& in_point);	
	/// Возвращает true, если есть обработанные данные
	virtual bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VPoint& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();
};

/// Вычисляет скользящее среднее
class VMean : public VDataProcessor<VPoint, VPoint>
{
private:
	/// База
	double base;
	/// Сумма
	double sum;
	/// Количество сбойных кадров
	int bad_count;
	/// Очередь данных на входе
	std::deque<VPoint> input;
	/// Очередь данных на выходе
	std::queue<VPoint> output;
public:
	/// Конструктор
	VMean(double in_base);	
	/// Деструктор
	~VMean();
	/// Ввод данных
	void Put(const VPoint& in_point);	
	/// Возвращает true, если есть обработанные данные
	virtual bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VPoint& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();
	/// Меняет базу
	void SetBase(double in_base);
};


/// Выделяет случайную составляющую (фильтр - скользящее среднее)
class VMeanVariate : public VDataProcessor<VPoint, VPoint>
{
private:
	/// База
	double base;	
	/// Среднее
	double sum;
	/// Очередь данных на входе
	std::deque<VPoint> input;
	/// Очередь данных на выходе
	std::queue<VPoint> output;
public:
	/// Конструктор
	VMeanVariate(double in_base);
	/// Деструктор
	~VMeanVariate();
	/// Ввод данных
	void Put(const VPoint& in_point);	
	/// Возвращает true, если есть обработанные данные
	virtual bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VPoint& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();
};

/// Выделяет случайную составляющую (фильтр - медиана)
class VMedianVariate : public VDataProcessor<VPoint, VPoint>
{
private:
	/// База
	double base;	
	/// Очередь данных на входе
	std::deque<VPoint> input;
	/// Очередь данных на выходе
	std::queue<VPoint> output;
	/// Данные для сортировки
	std::vector<double> sorted;
public:
	/// Конструктор
	VMedianVariate(double in_base);
	/// Деструктор
	~VMedianVariate();
	/// Ввод данных
	void Put(const VPoint& in_point);	
	/// Возвращает true, если есть обработанные данные
	virtual bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VPoint& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();
};

