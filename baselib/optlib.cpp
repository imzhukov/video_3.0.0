#include "optlib.h"

using namespace std;

/// <summary>
/// Конструктор класса VFastDescentMethod
/// </summary>
/// <param name="function">
/// Функция, объект какого-либо класса, наследуемого от VFunction
/// </param>
/// </param>
/// <param name="minAlfa">
/// Минимальное значение параметра alfa метода, положительное число, константа
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

VFastDescentMethod::VFastDescentMethod(VFunction* function, double minAlfa, double maxAlfa, double step, double eps, double delta, int minOrMax)
{
	MinAlfa=minAlfa;
	MaxAlfa=maxAlfa;
	Step=step;
	Eps=eps;
	Delta=delta;
	MinOrMax=minOrMax;
	Function=function;
}
/// <summary>
/// Поиск параметра Alfa и точки минимума для метода наискорейшего спуска
/// </summary>
double findAlfa(VFunction* function, double minAlfa, double maxAlfa, double step, double delta, int minOrMax)
{
	size_t count=function->GetParamCount();
	vector<double> data(count);
	vector<double> proizv(count);
	vector<double> scan(count);
	vector<double> extrScan(count);
	double extremum=0;
	double extrValue=function->Value();
	for (size_t index=0; index<count; index++)
	{
		data[index]=function->GetParam(index);
		proizv[index]=ch_proizv(function,delta,index);
	}
	for (double alfa=minAlfa; alfa<=maxAlfa; alfa+=step)
	{
		for (size_t index=0; index<count; index++)
		{
			scan[index]=data[index]-minOrMax*alfa*proizv[index];
			function->SetParam(index, scan[index]);
		}
		double value=function->Value();
		if (minOrMax*value<minOrMax*extrValue)
		{
			extrScan=scan;
			extremum=alfa;
			extrValue=value;
		}
	}
	for (size_t index=0; index<count; index++)
	{
		function->SetParam(index,data[index]);
	}
	return extremum;
}

/// <summary>
/// Поиск экстремума функции методом наискорейшего спуска
/// </summary>
void VFastDescentMethod::RunMethod()
{		
	size_t count=Function->GetParamCount();
	vector<double> prev(count);
	vector<double> scan(count);
	double prevValue=Function->Value();
	double Alfa=findAlfa(Function,MinAlfa,MaxAlfa,Step,Delta,MinOrMax);
	for (size_t index=0; index<count; index++)	
	{
		prev[index]=Function->GetParam(index);		
		scan[index]=prev[index]-MinOrMax*Alfa*ch_proizv(Function,Delta,index);
		Function->SetParam(index,scan[index]);
	}
	while ( (MinOrMax*(prevValue-Function->Value())>MinOrMax*Eps)&&(gradSqr(Function,Delta)>Eps))
	{		
		prevValue=Function->Value();
		Alfa=findAlfa(Function,MinAlfa,MaxAlfa,Step,Delta,MinOrMax);
		for (size_t index=0; index<count;index++)
		{	
			prev[index]=scan[index];			
			scan[index]-=MinOrMax*Alfa*ch_proizv(Function,Delta,index);
			Function->SetParam(index,scan[index]);
		}
	}
}

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
VGaussSeidelMethod::VGaussSeidelMethod(VFunction* function, double eps, int minOrMax)
{
	Eps=eps;
	MinOrMax=minOrMax;
	Function=function;
}

/// <summary>
/// Одномерный поиск методом дихотомии
/// </summary>
void dihotSeek(VFunction* function, size_t parNum, const pro::interval<double>& range, double eps, int minOrMax)
{	
	double first=range.left_bound().value;
	double last=range.right_bound().value;
	double point=(last+first)/2;
	double plus;
	double minus;
	
	while (last-first>eps)
	{			
		function->SetParam(parNum,point+eps);
		function->Update();		
		plus=function->Value();		
		function->SetParam(parNum,point-eps);
		function->Update();
		minus=function->Value();		
		if (minOrMax*plus<minOrMax*minus) 
			first=point;
		else 
			last=point;
		point=(last+first)/2;
	}	
	function->SetParam(parNum,last);
	function->Update();
	plus=function->Value();
	function->SetParam(parNum,first);
	function->Update();
	minus=function->Value();
	function->SetParam(parNum,point);
	function->Update();
	double temp=function->Value();
	
	if ((minOrMax*minus>minOrMax*temp)&&(minOrMax*plus>minOrMax*temp)) 
		function->SetParam(parNum,point);
	else if (minOrMax*plus<minOrMax*minus) 
		function->SetParam(parNum,last);
	else 
		function->SetParam(parNum,first);
	function->Update();	
}

/// <summary>
/// Поиск экстремума функции методом Гаусса-Зейделя
/// </summary>
void VGaussSeidelMethod::RunMethod()
{	
	const size_t count=VGaussSeidelMethod::Function->GetParamCount();
	vector<double> prev(count);
	vector<double> result(count);	
	for (size_t index=0; index<count; index++)
	{	
		result[index]=prev[index]=VGaussSeidelMethod::Function->GetParam(index);
	}	
	VGaussSeidelMethod::Function->Update();
	double value=VGaussSeidelMethod::Function->Value();
	double prev_value=value;
	while (true)
	{
		for (size_t index=0; index<count;index++)
		{			
			pro::interval<double> range=VGaussSeidelMethod::Function->GetParamRange(index);
			dihotSeek(VGaussSeidelMethod::Function, index, range, Eps, MinOrMax);
			result[index]=VGaussSeidelMethod::Function->GetParam(index);
		}
		value=VGaussSeidelMethod::Function->Value();
		if (result==prev || value>prev_value)
			break;
		prev=result;
		prev_value=value;
	}
	for (size_t i=0; i<count; i++)
	{
		VGaussSeidelMethod::Function->SetParam(i,result[i]);
	}
	VGaussSeidelMethod::Function->Update();
}

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
VGradientMethod::VGradientMethod(VFunction* function, double alfa, double eps, double delta, int minOrMax)
{
	Alfa=alfa;
	Eps=eps;
	Delta=delta;
	MinOrMax=minOrMax;
	Function=function;
}
/// <summary>
/// Поиск экстремума функции простым градиентным методом
/// </summary>
void VGradientMethod::RunMethod()
{		
	size_t count=Function->GetParamCount();
	vector<double> prev(count);
	vector<double> scan(count);
	double prevValue=Function->Value();
	
	for (size_t index=0; index<count; index++)	
	{
		prev[index]=Function->GetParam(index);		
		scan[index]=prev[index]-MinOrMax*Alfa*ch_proizv(Function,Delta,index);
		Function->SetParam(index,scan[index]);
	}
	while ( (MinOrMax*(prevValue-Function->Value())>MinOrMax*Eps)&&(gradSqr(Function,Delta)>Eps))
	{		
		prevValue=Function->Value();
		for (size_t index=0; index<count;index++)
		{	
			prev[index]=scan[index];			
			scan[index]-=MinOrMax*Alfa*ch_proizv(Function,Delta,index);
			Function->SetParam(index,scan[index]);
		}
	}
}

/// <summary>
/// Вспомогательная функция - частная производная функции в заданной точке
/// </summary>
double ch_proizv(VFunction* function, double delta, size_t parNum)
{
	double param = function->GetParam(parNum);
	double value=function->Value();
	function->SetParam(parNum, param+delta);
	double dvalue=function->Value();
	function->SetParam(parNum, param);	
	return (dvalue-value)/delta;
}

/// <summary>
/// Вспомогательная функция - модуль вектора градиента
/// </summary>
double gradSqr(VFunction* function, double delta)
{
	double sum=0;
	for(size_t index=0;index<function->GetParamCount();index++)
		sum+=pow(ch_proizv(function,delta,index),2);
	return sqrt(sum);
}
