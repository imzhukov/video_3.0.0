#pragma once

#include "fftw3.h"
#include "mathlib.h"
#include "processing.h"

/// Интерфейс класса sinc-фильтра
class VFIRFilter
{
public:
	/// Возвращает размер импульсной характеристики
	virtual size_t GetIRSize() const = 0;
	/// Возвращает размер фильтра
	virtual size_t GetFilterSize() const = 0;
	/// Возвращает значение в точке
	virtual point_t operator[](size_t in_index) const = 0;
};
typedef boost::shared_ptr<VFIRFilter> VFIRFilterPtr;

/// ФНЧ
class VLowPassFIR : public VFIRFilter
{
private:
	/// Размер импульсной характеристики
	size_t ir_size;
	/// Размер фильтра
	size_t filter_size;
	/// Фильтр
	std::vector<point_t> filter;
public:
	/// Конструктор
	VLowPassFIR(double in_cut_off_freq);
	/// Возвращает размер импульсной характеристики
	size_t GetIRSize() const;
	/// Возвращает размер фильтра
	size_t GetFilterSize() const;
	/// Возвращает значение в точке
	point_t operator[](size_t in_index) const;
};

/// ФВЧ
class VHighPassFIR : public VFIRFilter
{
private:
	/// Размер импульсной характеристики
	size_t ir_size;
	/// Размер фильтра
	size_t filter_size;
	/// Фильтр
	std::vector<point_t> filter;
public:
	/// Конструктор
	VHighPassFIR(double in_cut_off_freq);
	/// Возвращает размер импульсной характеристики
	size_t GetIRSize() const;
	/// Возвращает размер фильтра
	size_t GetFilterSize() const;
	/// Возвращает значение в точке
	point_t operator[](size_t in_index) const;
};


/// Полосовой фильтр
class VBandPassFIR : public VFIRFilter
{
private:
	/// Размер импульсной характеристики
	size_t ir_size;
	/// Размер фильтра
	size_t filter_size;
	/// Фильтр
	std::vector<point_t> filter;
public:
	/// Конструктор
	VBandPassFIR(double in_low, double in_high);
	/// Возвращает размер импульсной характеристики
	size_t GetIRSize() const;
	/// Возвращает размер фильтра
	size_t GetFilterSize() const;
	/// Возвращает значение в точке
	point_t operator[](size_t in_index) const;
};

/// Режекторный фильтр
class VBandStopFIR : public VFIRFilter
{
private:
	/// Размер импульсной характеристики
	size_t ir_size;
	/// Размер фильтра
	size_t filter_size;
	/// Фильтр
	std::vector<point_t> filter;
public:
	/// Конструктор
	VBandStopFIR(double in_low, double in_high);
	/// Возвращает размер импульсной характеристики
	size_t GetIRSize() const;
	/// Возвращает размер фильтра
	size_t GetFilterSize() const;
	/// Возвращает значение в точке
	point_t operator[](size_t in_index) const;
};

/// Оконная функция
class VWindowFunction
{
public:
	virtual double operator()(size_t in_index, size_t in_count) const = 0;	
};
typedef boost::shared_ptr<VWindowFunction> VWindowFunctionPtr;

/// Прямоугольное окно
class VRectWindowFunction : public VWindowFunction
{
private:
public:
	double operator()(size_t in_index, size_t in_count) const
	{
		return 1.0;
	};
};

/// Окно Ханна
class VHunnWindowFunction : public VWindowFunction
{
private:
public:
	double operator()(size_t in_index, size_t in_count) const
	{
		return 0.5*(1.0-cos(2.0*M_PI*double(in_index)/(double(in_count)-1.0)));
	};	
};

/// Окно Хэмминга
class VHammingWindowFunction : public VWindowFunction
{
private:
public:
	double operator()(size_t in_index, size_t in_count) const
	{
		return 0.54-0.46*cos(2.0*M_PI*double(in_index)/(double(in_count)-1.0));
	};	
};

/// Окно Бартлетта
class VBartlettWindowFunction : public VWindowFunction
{
private:
public:
	double operator()(size_t in_index, size_t in_count) const
	{
		return 1.0-fabs(double(in_index)/((double(in_count)-1.0)/2)-1.0);
	};
};

/// Окно Блэкмана
class VBlackmanWindowFunction : public VWindowFunction
{
private:
public:
	double operator()(size_t in_index, size_t in_count) const
	{
		return 0.42-0.5*cos(2.0*M_PI*double(in_index)/double(in_count-1))+0.08*cos(4.0*M_PI*double(in_index)/double(in_count-1));
	};
};

/// ФФТ Фильтр
class VFFTFilter : public VDataProcessor<point_t, point_t>
{
private:

	/// Фильтр
	VFIRFilterPtr filter;	
	/// Данные на входе
	std::deque<point_t> input;
	/// Данные на выходе
	std::deque<point_t> output;
	/// Исхождный сигнал
	fftw_complex *signal;
	/// Результат прямого БПФ
	fftw_complex *spectrum;
	/// Результат обратного БПФ
	fftw_complex *result;
	/// Планировщики FFTW
	fftw_plan forward, backward;	
	/// Вычисление
	void Calculate();

public:
	/// Конструктор
	VFFTFilter(const VFIRFilterPtr& in_filter);
	/// Деструктор
	~VFFTFilter();
	/// Ввод данных
	void Put(const point_t& in_point);	
	/// Возвращает true, если есть обработанные данные
	bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	bool Empty() const;		
	/// Возвращает первый элемент данных
	const point_t& Front() const;
	/// Выталкивает первый элемент данных
	void Pop();
	/// Сливает остаток данных
	void Flush();
};