#pragma once

#include "mathlib.h"
#include "interval.h"

class VFunction
{
public:	
	/// <summary>
	/// Метод, устанавливающий значение параметра с номером index равным value
	/// </summary>
	virtual void SetParam(size_t index, double value)=0;
	
	/// <summary>
	/// Метод, возвращающий значение параметра с номером index
	/// </summary>
	virtual double GetParam(size_t index)=0;
	
	/// <summary>
	/// Метод, устанавливающий значение границы параметра с номером index равным value
	/// </summary>
	virtual void SetParamRange(size_t index, const pro::interval<double>& value)=0;
	
	/// <summary>
	/// Метод, возвращающий границы параметра с номером index
	/// </summary>
	virtual pro::interval<double> GetParamRange(size_t index)=0;

	/// <summary>
	/// Метод, пересчитвающий значение целевой функции
	/// </summary>
	virtual void Update()=0;

	/// <summary>
	/// Метод, возвращающий текущее значение функции
	/// </summary>
	virtual double Value()=0;
	
	/// <summary>
	/// Метод, возвращающий количество параметров
	/// </summary>
	virtual size_t GetParamCount()=0;
};

class VMethod
{
public:
	/// <summary>
	/// Метод, устанавливающий оптимизируемую функцию
	/// </summary>
	void SetFunction(VFunction* function) {Function=function;}
	
	/// <summary>
	/// Реализация метода оптимизации
	/// </summary>
	virtual void RunMethod()=0;
protected:
	VFunction* Function;
};


class VFastDescentMethod : public VMethod
{
public:
	
	/// <summary>
	/// Конструктор класса VFastDescentMethod
	/// </summary>
	/// <param name="function">
	/// Функция, объект какого-либо класса, наследуемого от VFunction
	/// </param>
	/// <param name="minAlfa">
	/// Минимальное значение параметра alfa метода, положительное число, константа
	/// </param>
	/// </param>
	/// <param name="maxAlfa">
	/// Максимальное значение параметра alfa метода, положительное число, константа
	/// </param>
	/// <param name="step">
	/// Точность (шаг) вычисления параметра alfa
	/// </param>
	/// <param name="eps">
	/// Точность вычисления
	/// </param>
	/// <param name="delta">
	/// Точность вычисления производной
	/// </param>
	/// <param name="minOrMax">
	/// Параметр равен 1, если поиск минимума функции, и -1, если максимума
	/// </param>
	VFastDescentMethod(VFunction* function, double minAlfa, double maxAlfa, double step, double eps, double delta, int minOrMax);
	
	/// <summary>
	/// Поиск экстремума функции методом наискорейшего спуска
	/// </summary>
	void RunMethod();

private:
	double MinAlfa;
	double MaxAlfa;
	double Step;
	double Eps;
	double Delta;
	int MinOrMax;
};

class VGaussSeidelMethod : VMethod
{
public:
	/// <summary>
	/// Конструктор класса VGaussSeidelMethod
	/// </summary>
	/// <param name="function">
	/// Функция, объект какого-либо класса, наследуемого от VFunction
	/// </param>
	/// <param name="eps">
	/// Точность вычисления
	/// </param>
	/// <param name="minOrMax">
	/// Параметр равен 1, если поиск минимума функции, и -1, если максимума
	/// </param>
	VGaussSeidelMethod(VFunction* function, double eps, int minOrMax);
	
	/// <summary>
	/// Поиск экстремума функции методом Гаусса-Зейделя
	/// </summary>
	void RunMethod();

private:
	double Eps;
	int MinOrMax;
};

class VGradientMethod : public VMethod
{
public:
	
	/// <summary>
	/// Конструктор класса VGradientMethod
	/// </summary>
	/// <param name="function">
	/// Функция, объект какого-либо класса, наследуемого от VFunction
	/// </param>
	/// <param name="alfa">
	/// Параметр метода, положительное число, константа
	/// </param>
	/// <param name="eps">
	/// Точность вычисления
	/// </param>
	/// <param name="delta">
	/// Точность вычисления производной
	/// </param>
	/// <param name="minOrMax">
	/// Параметр равен 1, если поиск минимума функции, и -1, если максимума
	/// </param>
	VGradientMethod(VFunction* function, double alfa, double eps, double delta, int minOrMax);
	
	/// <summary>
	/// Поиск экстремума функции простым градиентным методом
	/// </summary>
	void RunMethod();

private:
	double Alfa;
	double Eps;
	double Delta;
	double MinOrMax;	
};


/// <summary>
/// Вспомогательная функция - частная производная функции в заданной точке
/// </summary>
double ch_proizv(VFunction* function, double delta, size_t parNum);
/// <summary>
/// Вспомогательная функция - модуль вектора градиента
/// </summary>
double gradSqr(VFunction* function, double delta);
