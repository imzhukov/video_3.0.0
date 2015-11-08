
#include "VCalcOptions.h"
#include "tables.h"



//--------------------------------- Методы класса VNarrowRecord ----------------------------------------------

/// Конструктор
VNarrowRecord::VNarrowRecord(): degree(DEGREE1), speed(0,300), norma(1520), value(0,0) {};

/// Конструктор
VNarrowRecord::VNarrowRecord(DEGREE in_degree, const pro::interval<int>& in_speed, double in_norma, const pro::interval<double>& in_value) : 
	degree(in_degree), speed(in_speed), norma(in_norma), value(in_value){};	
/// Возвращает true, если входные данный удовлетворяют ключу
bool VNarrowRecord::IsKey(DEGREE in_degree, int in_speed, double in_norma) const
{
	return (degree==in_degree) && speed.contains(in_speed) && (norma==in_norma);
};

/// Возвращает диапазон значений
const pro::interval<double> VNarrowRecord::GetRange() const
{
	return value;
};

//--------------------------------- Методы класса VWidenRecord --------------------------------------

/// Конструктор
VWidenRecord::VWidenRecord(DEGREE in_degree, const pro::interval<int>& in_speed, double in_norma, const pro::interval<double>& in_radius, const pro::interval<double>& in_value) :
	degree(in_degree), speed(in_speed), norma(in_norma), radius(in_radius), value(in_value){};

/// Возвращает true, если входные данный удовлетворяют ключу
bool VWidenRecord::IsKey(DEGREE in_degree, int in_speed, double in_norma, double in_radius) const
{
	return (degree==in_degree) && speed.contains(in_speed) && norma==in_norma && radius.contains(in_radius);
}

/// Возвращает диапазон значений
const pro::interval<double> VWidenRecord::GetRange() const
{
	return value;
}

//---------------------------------- Методы класса VWidenTable -----------------------------------------------

/// Конструктор
VWidenTable::VWidenTable()
{
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		//1520				
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(101,140), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(8,14,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(101,140), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(14,16,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(101,140), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(16,DBL_MAX,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(61,100), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(8,18,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(61,100), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(18,22,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(61,100), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(22,DBL_MAX,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(26,50), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(10,20,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(51,60), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(8,20,false,true)));		
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(20,26,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(26,DBL_MAX,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(10,26,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(26,28,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(28,DBL_MAX,false,true)));
		//1524
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(101,140), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(8,14,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(101,140), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(14,16,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(101,140), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(16,DBL_MAX,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(61,100), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(8,18,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(61,100), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(18,20,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(61,100), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(20,DBL_MAX,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(26,50), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(10,16,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(51,60), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(8,16,false,true)));		
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(16,22,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(22,DBL_MAX,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(10,22,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(22,24,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(24,DBL_MAX,false,true)));		
		//1530
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(101,140), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(8,14,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(101,140), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(14,16,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(101,140), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(16,DBL_MAX,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(61,100), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(8,14,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(61,100), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(14,16,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(61,100), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(16,DBL_MAX,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(26,50), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(10,14,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(51,60), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(8,14,false,true)));		
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(14,16,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(16,DBL_MAX,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(10,16,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(16,18,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(18,DBL_MAX,false,true)));		
		//1535	
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(101,140), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(6,9,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(101,140), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(9,11,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(101,140), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(11,DBL_MAX,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(61,100), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(6,9,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(61,100), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(9,11,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(61,100), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(11,DBL_MAX,false,true)));		
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(26,50), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(10,11,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(51,60), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(8,11,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(11,13,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(13,DBL_MAX,false,true)));			
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(10,11,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(11,13,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(13,DBL_MAX,false,true)));
		//1540
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(101,140), NORMA_WIDTH_1540, INFINITE_INTERVAL<double>(), pro::interval<double>(6,DBL_MAX,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(61,100), NORMA_WIDTH_1540, INFINITE_INTERVAL<double>(), pro::interval<double>(6,8,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(61,100), NORMA_WIDTH_1540, INFINITE_INTERVAL<double>(), pro::interval<double>(8,DBL_MAX,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1540, INFINITE_INTERVAL<double>(), pro::interval<double>(6,8,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1540, INFINITE_INTERVAL<double>(), pro::interval<double>(8,DBL_MAX,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1540, INFINITE_INTERVAL<double>(), pro::interval<double>(8,DBL_MAX,false,true)));
	}
	else if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_EXPERIMENT))
	{
		//Высокоскоростные линии
	
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(201,250), NORMA_WIDTH_1520, pro::interval<double>(0,DBL_MAX,true,true), pro::interval<double>(3,4,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(201,250), NORMA_WIDTH_1520, pro::interval<double>(0,DBL_MAX,true,true), pro::interval<double>(4,8,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(201,250), NORMA_WIDTH_1520, pro::interval<double>(0,DBL_MAX,true,true), pro::interval<double>(8,14,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(201,250), NORMA_WIDTH_1520, pro::interval<double>(0,DBL_MAX,true,true), pro::interval<double>(14,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(161,200), NORMA_WIDTH_1520, pro::interval<double>(3000,DBL_MAX,true,true), pro::interval<double>(6,8,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(161,200), NORMA_WIDTH_1520, pro::interval<double>(3000,DBL_MAX,true,true), pro::interval<double>(8,12,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(161,200), NORMA_WIDTH_1520, pro::interval<double>(3000,DBL_MAX,true,true), pro::interval<double>(12,14,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(161,200), NORMA_WIDTH_1520, pro::interval<double>(3000,DBL_MAX,true,true), pro::interval<double>(14,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(141,160), NORMA_WIDTH_1520, pro::interval<double>(3000,DBL_MAX,true,true), pro::interval<double>(7,9,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(141,160), NORMA_WIDTH_1520, pro::interval<double>(3000,DBL_MAX,true,true), pro::interval<double>(9,14,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(141,160), NORMA_WIDTH_1520, pro::interval<double>(3000,DBL_MAX,true,true), pro::interval<double>(14,18,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(141,160), NORMA_WIDTH_1520, pro::interval<double>(3000,DBL_MAX,true,true), pro::interval<double>(18,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(161,200), NORMA_WIDTH_1520, pro::interval<double>(0,3000,true,false), pro::interval<double>(6,8,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(161,200), NORMA_WIDTH_1520, pro::interval<double>(0,3000,true,false), pro::interval<double>(8,14,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(161,200), NORMA_WIDTH_1520, pro::interval<double>(0,3000,true,false), pro::interval<double>(14,18,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(161,200), NORMA_WIDTH_1520, pro::interval<double>(0,3000,true,false), pro::interval<double>(18,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(141,160), NORMA_WIDTH_1520, pro::interval<double>(0,3000,true,false), pro::interval<double>(8,12,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(141,160), NORMA_WIDTH_1520, pro::interval<double>(0,3000,true,false), pro::interval<double>(12,18,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(141,160), NORMA_WIDTH_1520, pro::interval<double>(0,3000,true,false), pro::interval<double>(18,22,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(141,160), NORMA_WIDTH_1520, pro::interval<double>(0,3000,true,false), pro::interval<double>(22,DBL_MAX,false,true)));
		
		//1520		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(121,140), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(7,9,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(121,140), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(9,14,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(121,140), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(14,18,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(121,140), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(18,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(61,120), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(8,10,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(61,120), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(10,18,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(61,120), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(18,22,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(61,120), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(22,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(51,60), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(8,14,false,true)));
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(26,50), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(10,14,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(14,20,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(20,26,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(26,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(0,25), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(12,18,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(18,26,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(26,28,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1520, INFINITE_INTERVAL<double>(), pro::interval<double>(28,DBL_MAX,false,true)));
		
		//1524
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(121,140), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(7,9,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(121,140), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(8,10,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(121,140), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(10,12,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(121,140), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(12,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(61,120), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(8,10,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(61,120), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(10,14,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(61,120), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(14,18,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(61,120), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(18,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(51,60), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(8,14,false,true)));
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(26,50), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(10,14,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(12,16,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(16,22,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(22,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(0,25), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(10,16,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(16,22,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(22,24,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1524, INFINITE_INTERVAL<double>(), pro::interval<double>(24,DBL_MAX,false,true)));
		
		//1530
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(121,140), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(5,6,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(121,140), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(6,10,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(121,140), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(10,12,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(121,140), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(12,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(61,120), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(8,10,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(61,120), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(10,14,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(61,120), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(14,16,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(61,120), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(16,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(51,60), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(8,14,false,true)));
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(26,50), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(10,14,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(11,14,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(14,16,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(16,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(0,25), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(9,13,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(13,16,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(16,18,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1530, INFINITE_INTERVAL<double>(), pro::interval<double>(18,DBL_MAX,false,true)));
		
		//1535			
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(61,120), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(6,8,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(61,120), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(8,9,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(61,120), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(9,11,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(61,120), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(11,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(51,60), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(8,14,false,true)));
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(26,50), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(10,14,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(9,11,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(11,13,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(13,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(0,25), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(8,9,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(9,11,false,true)));
		table.push_back(VWidenRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(11,13,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1535, INFINITE_INTERVAL<double>(), pro::interval<double>(13,DBL_MAX,false,true)));
		
		//1540
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(26,60), NORMA_WIDTH_1540, INFINITE_INTERVAL<double>(), pro::interval<double>(6,7,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1540, INFINITE_INTERVAL<double>(), pro::interval<double>(7,8,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1540, INFINITE_INTERVAL<double>(), pro::interval<double>(8,DBL_MAX,false,true)));
		
		table.push_back(VWidenRecord(DEGREE1, pro::interval<int>(0,25), NORMA_WIDTH_1540, INFINITE_INTERVAL<double>(), pro::interval<double>(6,7,false,true)));
		table.push_back(VWidenRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1540, INFINITE_INTERVAL<double>(), pro::interval<double>(7,8,false,true)));
		table.push_back(VWidenRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1540, INFINITE_INTERVAL<double>(), pro::interval<double>(8,DBL_MAX,false,true)));
	}
};

/// Возвращает диапазон
pro::interval<double> VWidenTable::GetRange(DEGREE in_degree, int in_speed, double in_norma, double in_radius) const
{
	std::vector<VWidenRecord>::const_iterator i=std::find_if(table.begin(), table.end(), VWidenRecord::VFindKey(in_degree, 
		in_speed, in_norma, in_radius));
	if (i!=table.end())	
		return i->GetRange();	
	else
		return EMPTY_INTERVAL<double>();
};




//----------------------------------- Методы класса VNarrowTable --------------------------------------------

/// Конструктор
VNarrowTable::VNarrowTable()
{	
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		//1520
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(101,140), NORMA_WIDTH_1520, pro::interval<double>(4,6,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(101,140), NORMA_WIDTH_1520, pro::interval<double>(6,8,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(101,140), NORMA_WIDTH_1520, pro::interval<double>(8,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(61,100), NORMA_WIDTH_1520, pro::interval<double>(4,6,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(61,100), NORMA_WIDTH_1520, pro::interval<double>(6,8,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(61,100), NORMA_WIDTH_1520, pro::interval<double>(8,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1520, pro::interval<double>(4,6,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1520, pro::interval<double>(6,8,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1520, pro::interval<double>(8,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1520, pro::interval<double>(4,6,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1520, pro::interval<double>(6,8,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1520, pro::interval<double>(8,DBL_MAX,false,true)));
		//1524
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(101,140), NORMA_WIDTH_1524, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(101,140), NORMA_WIDTH_1524, pro::interval<double>(10,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(101,140), NORMA_WIDTH_1524, pro::interval<double>(12,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(61,100), NORMA_WIDTH_1524, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(61,100), NORMA_WIDTH_1524, pro::interval<double>(10,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(61,100), NORMA_WIDTH_1524, pro::interval<double>(12,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1524, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1524, pro::interval<double>(10,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1524, pro::interval<double>(12,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1524, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1524, pro::interval<double>(10,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1524, pro::interval<double>(12,DBL_MAX,false,true)));
		//1530
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(101,140), NORMA_WIDTH_1530, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(101,140), NORMA_WIDTH_1530, pro::interval<double>(10,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(101,140), NORMA_WIDTH_1530, pro::interval<double>(12,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(61,100), NORMA_WIDTH_1530, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(61,100), NORMA_WIDTH_1530, pro::interval<double>(10,15,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(61,100), NORMA_WIDTH_1530, pro::interval<double>(15,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1530, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1530, pro::interval<double>(10,15,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1530, pro::interval<double>(15,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1530, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1530, pro::interval<double>(10,15,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1530, pro::interval<double>(15,DBL_MAX,false,true)));
		//1535	
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(101,140), NORMA_WIDTH_1535, pro::interval<double>(4,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(101,140), NORMA_WIDTH_1535, pro::interval<double>(12,15,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(101,140), NORMA_WIDTH_1535, pro::interval<double>(15,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(61,100), NORMA_WIDTH_1535, pro::interval<double>(4,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(61,100), NORMA_WIDTH_1535, pro::interval<double>(12,18,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(61,100), NORMA_WIDTH_1535, pro::interval<double>(18,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1535, pro::interval<double>(4,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1535, pro::interval<double>(12,18,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1535, pro::interval<double>(18,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1535, pro::interval<double>(4,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1535, pro::interval<double>(12,18,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1535, pro::interval<double>(18,DBL_MAX,false,true)));
		//1540
		//Для скоростей 101-140 данных нет - это недопустимые входные данные
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(61,100), NORMA_WIDTH_1540, pro::interval<double>(4,15,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(61,100), NORMA_WIDTH_1540, pro::interval<double>(15,20,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(61,100), NORMA_WIDTH_1540, pro::interval<double>(20,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1540, pro::interval<double>(4,15,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1540, pro::interval<double>(15,20,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1540, pro::interval<double>(20,DBL_MAX,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1540, pro::interval<double>(4,15,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1540, pro::interval<double>(15,20,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1540, pro::interval<double>(20,DBL_MAX,false,true)));
	}
	else if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_EXPERIMENT))
	{
		//Высокоскоростные линии
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(201,250), NORMA_WIDTH_1520, pro::interval<double>(2,3,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(201,250), NORMA_WIDTH_1520, pro::interval<double>(3,4,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(201,250), NORMA_WIDTH_1520, pro::interval<double>(4,6,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(201,250), NORMA_WIDTH_1520, pro::interval<double>(6,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(161,200), NORMA_WIDTH_1520, pro::interval<double>(3,5,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(161,200), NORMA_WIDTH_1520, pro::interval<double>(5,6,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(161,200), NORMA_WIDTH_1520, pro::interval<double>(6,8,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(161,200), NORMA_WIDTH_1520, pro::interval<double>(8,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(141,160), NORMA_WIDTH_1520, pro::interval<double>(3,5,false,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(141,160), NORMA_WIDTH_1520, pro::interval<double>(5,6,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(141,160), NORMA_WIDTH_1520, pro::interval<double>(6,8,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(141,160), NORMA_WIDTH_1520, pro::interval<double>(8,DBL_MAX,false,true)));


		//1520
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(121,140), NORMA_WIDTH_1520, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(121,140), NORMA_WIDTH_1520, pro::interval<double>(4,6,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(121,140), NORMA_WIDTH_1520, pro::interval<double>(6,8,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(121,140), NORMA_WIDTH_1520, pro::interval<double>(8,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(61,120), NORMA_WIDTH_1520, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(61,120), NORMA_WIDTH_1520, pro::interval<double>(4,6,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(61,120), NORMA_WIDTH_1520, pro::interval<double>(6,8,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(61,120), NORMA_WIDTH_1520, pro::interval<double>(8,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(26,60), NORMA_WIDTH_1520, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1520, pro::interval<double>(4,6,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1520, pro::interval<double>(6,8,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1520, pro::interval<double>(8,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(0,25), NORMA_WIDTH_1520, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1520, pro::interval<double>(4,6,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1520, pro::interval<double>(6,8,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1520, pro::interval<double>(8,DBL_MAX,false,true)));	

		//1524
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(121,140), NORMA_WIDTH_1524, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(121,140), NORMA_WIDTH_1524, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(121,140), NORMA_WIDTH_1524, pro::interval<double>(10,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(121,140), NORMA_WIDTH_1524, pro::interval<double>(12,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(61,120), NORMA_WIDTH_1524, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(61,120), NORMA_WIDTH_1524, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(61,120), NORMA_WIDTH_1524, pro::interval<double>(10,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(61,120), NORMA_WIDTH_1524, pro::interval<double>(12,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(26,60), NORMA_WIDTH_1524, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1524, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1524, pro::interval<double>(10,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1524, pro::interval<double>(12,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(0,25), NORMA_WIDTH_1524, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1524, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1524, pro::interval<double>(10,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1524, pro::interval<double>(12,DBL_MAX,false,true)));
		
		//1530
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(121,140), NORMA_WIDTH_1530, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(121,140), NORMA_WIDTH_1530, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(121,140), NORMA_WIDTH_1530, pro::interval<double>(10,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(121,140), NORMA_WIDTH_1530, pro::interval<double>(12,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(61,120), NORMA_WIDTH_1530, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(61,120), NORMA_WIDTH_1530, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(61,120), NORMA_WIDTH_1530, pro::interval<double>(10,15,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(61,120), NORMA_WIDTH_1530, pro::interval<double>(15,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(26,60), NORMA_WIDTH_1530, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1530, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1530, pro::interval<double>(10,15,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1530, pro::interval<double>(15,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(0,25), NORMA_WIDTH_1530, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1530, pro::interval<double>(4,10,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1530, pro::interval<double>(10,15,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1530, pro::interval<double>(15,DBL_MAX,false,true)));

		//1535	
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(61,120), NORMA_WIDTH_1535, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(61,120), NORMA_WIDTH_1535, pro::interval<double>(4,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(61,120), NORMA_WIDTH_1535, pro::interval<double>(12,18,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(61,120), NORMA_WIDTH_1535, pro::interval<double>(18,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(26,60), NORMA_WIDTH_1535, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1535, pro::interval<double>(4,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1535, pro::interval<double>(12,18,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1535, pro::interval<double>(18,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(0,25), NORMA_WIDTH_1535, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1535, pro::interval<double>(4,12,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1535, pro::interval<double>(12,18,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1535, pro::interval<double>(18,DBL_MAX,false,true)));
		
		//1540
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(26,60), NORMA_WIDTH_1540, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(26,60), NORMA_WIDTH_1540, pro::interval<double>(4,15,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(26,60), NORMA_WIDTH_1540, pro::interval<double>(15,20,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(26,60), NORMA_WIDTH_1540, pro::interval<double>(20,DBL_MAX,false,true)));
		
		table.push_back(VNarrowRecord(DEGREE1, pro::interval<int>(0,25), NORMA_WIDTH_1540, pro::interval<double>(4,4,true,true)));
		table.push_back(VNarrowRecord(DEGREE2, pro::interval<int>(0,25), NORMA_WIDTH_1540, pro::interval<double>(4,15,false,true)));
		table.push_back(VNarrowRecord(DEGREE3, pro::interval<int>(0,25), NORMA_WIDTH_1540, pro::interval<double>(15,20,false,true)));
		table.push_back(VNarrowRecord(DEGREE4, pro::interval<int>(0,25), NORMA_WIDTH_1540, pro::interval<double>(20,DBL_MAX,false,true)));	
	}
};


/// Возвращает диапазон
pro::interval<double> VNarrowTable::GetRange(DEGREE in_degree, int in_speed, double in_norma) const
{
	std::vector<VNarrowRecord>::const_iterator i=std::find_if(table.begin(), table.end(), VNarrowRecord::VFindKey(in_degree, 
		in_speed, in_norma));
	if (i!=table.end())		
		return i->GetRange();		
	else
		return EMPTY_INTERVAL<double>();
};

//-------------------------------- Методы класса VWaveRecord ------------------------------------------------

/// Конструктор
VWaveRecord::VWaveRecord(): degree(DEGREE1), speed(0,300), length(0, DBL_MAX), value(0,0) {};

/// Конструктор
VWaveRecord::VWaveRecord(DEGREE in_degree, const pro::interval<int>& in_speed, const pro::interval<double>& in_length, const pro::interval<double>& in_value) : 
	degree(in_degree), speed(in_speed), length(in_length), value(in_value){};	

/// Возвращает true, если входные данный удовлетворяют ключу
bool VWaveRecord::IsKey(DEGREE in_degree, int in_speed, double in_length) const
{
	return (degree==in_degree) && speed.contains(in_speed) && length.contains(in_length);
};

/// Возвращает диапазон значений
const pro::interval<double> VWaveRecord::GetRange() const
{
	return value;
};

//-------------------------------- Методы класса VPitTable --------------------------------------------------

/// Конструктор
VPitTable::VPitTable()
{	
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{	
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(121,140), PIT_LENGTH_RANGE, pro::interval<double>(10, 15, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(121,140), PIT_LENGTH_RANGE, pro::interval<double>(15, 20, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(121,140), PIT_LENGTH_RANGE, pro::interval<double>(20, DBL_MAX, false, true)));	
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(61,120), PIT_LENGTH_RANGE, pro::interval<double>(10, 20, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(61,120), PIT_LENGTH_RANGE, pro::interval<double>(20, 25, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(61,120), PIT_LENGTH_RANGE, pro::interval<double>(25, DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(41,60), PIT_LENGTH_RANGE, pro::interval<double>(12, 25, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(41,60), PIT_LENGTH_RANGE, pro::interval<double>(25, 30, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(41,60), PIT_LENGTH_RANGE, pro::interval<double>(30, DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(16,40), PIT_LENGTH_RANGE, pro::interval<double>(15, 30, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(16,40), PIT_LENGTH_RANGE, pro::interval<double>(30, 35, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(16,40), PIT_LENGTH_RANGE, pro::interval<double>(35, DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(0,15), PIT_LENGTH_RANGE, pro::interval<double>(18, 35, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(0,15), PIT_LENGTH_RANGE, pro::interval<double>(35, 45, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(0,15), PIT_LENGTH_RANGE, pro::interval<double>(45, DBL_MAX, false, true)));
	}
	else if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_EXPERIMENT))
	{
		//Скорости выше 140
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(201,250), PIT_LENGTH_RANGE, pro::interval<double>(6,8,false,true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(201,250), PIT_LENGTH_RANGE, pro::interval<double>(8,13,false,true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(201,250), PIT_LENGTH_RANGE, pro::interval<double>(13,18,false,true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(201,250), PIT_LENGTH_RANGE, pro::interval<double>(18,DBL_MAX,false,true)));
			
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(161,200), PIT_LENGTH_RANGE, pro::interval<double>(7,9,false,true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(161,200), PIT_LENGTH_RANGE, pro::interval<double>(9,14,false,true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(161,200), PIT_LENGTH_RANGE, pro::interval<double>(14,18,false,true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(161,200), PIT_LENGTH_RANGE, pro::interval<double>(18,DBL_MAX,false,true)));	
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(141,160), PIT_LENGTH_RANGE, pro::interval<double>(9,11,false,true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(141,160), PIT_LENGTH_RANGE, pro::interval<double>(11,15,false,true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(141,160), PIT_LENGTH_RANGE, pro::interval<double>(15,18,false,true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(141,160), PIT_LENGTH_RANGE, pro::interval<double>(18,DBL_MAX,false,true)));	
		
		
		//Скорости ниже 140
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(121,140), PIT_LENGTH_RANGE, pro::interval<double>(9,11,false,true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(121,140), PIT_LENGTH_RANGE, pro::interval<double>(11,15,false,true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(121,140), PIT_LENGTH_RANGE, pro::interval<double>(15,18,false,true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(121,140), PIT_LENGTH_RANGE, pro::interval<double>(18,DBL_MAX,false,true)));
			
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(61,120), PIT_LENGTH_RANGE, pro::interval<double>(10,14,false,true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(61,120), PIT_LENGTH_RANGE, pro::interval<double>(14,20,false,true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(61,120), PIT_LENGTH_RANGE, pro::interval<double>(20,25,false,true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(61,120), PIT_LENGTH_RANGE, pro::interval<double>(25,DBL_MAX,false,true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(0,60), PIT_LENGTH_RANGE, pro::interval<double>(15,20,false,true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(0,60), PIT_LENGTH_RANGE, pro::interval<double>(20,25,false,true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(41,60), PIT_LENGTH_RANGE, pro::interval<double>(25,30,false,true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(16,40), PIT_LENGTH_RANGE, pro::interval<double>(25,35,false,true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(0,15), PIT_LENGTH_RANGE, pro::interval<double>(25,45,false,true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(41,60), PIT_LENGTH_RANGE, pro::interval<double>(30,DBL_MAX,false,true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(16,40), PIT_LENGTH_RANGE, pro::interval<double>(35,DBL_MAX,false,true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(0,15), PIT_LENGTH_RANGE, pro::interval<double>(45,DBL_MAX,false,true)));			
	}
};	

/// Возвращает диапазон
pro::interval<double> VPitTable::GetRange(DEGREE in_degree, int in_speed, double in_length) const
{		
	std::vector<VWaveRecord>::const_iterator i=std::find_if(table.begin(), table.end(), VWaveRecord::VFindKey(in_degree, in_speed, in_length));
	if (i!=table.end())		
		return i->GetRange();		
	else
		return EMPTY_INTERVAL<double>();
};


//---------------------------------- Методы класса VDistortionTable ------------------------------------------------

/// Конструктор
VDistortionTable::VDistortionTable()
{	
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(121,140), DISTORTION_LENGTH_RANGE, pro::interval<double>(8,12, false, true)));		
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(121,140), DISTORTION_LENGTH_RANGE, pro::interval<double>(12,16, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(121,140), DISTORTION_LENGTH_RANGE, pro::interval<double>(16,DBL_MAX, false, true)));			
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(61,120), DISTORTION_LENGTH_RANGE, pro::interval<double>(10,16, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(61,120), DISTORTION_LENGTH_RANGE, pro::interval<double>(16,20, false, true)));				
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(61,120), DISTORTION_LENGTH_RANGE, pro::interval<double>(20,DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(41,60), DISTORTION_LENGTH_RANGE, pro::interval<double>(12,20, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(41,60), DISTORTION_LENGTH_RANGE, pro::interval<double>(20,25, false, true)));				
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(41,60), DISTORTION_LENGTH_RANGE, pro::interval<double>(25,DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(16,40), DISTORTION_LENGTH_RANGE, pro::interval<double>(14,25, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(16,40), DISTORTION_LENGTH_RANGE, pro::interval<double>(25,30, false, true)));				
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(16,40), DISTORTION_LENGTH_RANGE, pro::interval<double>(30,DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(0,15), DISTORTION_LENGTH_RANGE, pro::interval<double>(16,30, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(0,15), DISTORTION_LENGTH_RANGE, pro::interval<double>(30,50, false, true)));				
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(0,15), DISTORTION_LENGTH_RANGE, pro::interval<double>(50,DBL_MAX, false, true)));
	}
	else if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_EXPERIMENT))
	{
		//Скорости выше 140
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(201,250), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(6,8, false, true)));		
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(201,250), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(8,10, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(201,250), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(10,12, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(201,250), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(12,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(161,200), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(6,8, false, true)));		
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(161,200), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(8,11, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(161,200), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(11,13, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(161,200), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(13,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(141,160), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(7,9, false, true)));		
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(141,160), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(9,13, false, true)));		
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(141,160), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(13,14, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(141,160), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(14,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(201,250), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(6,8, false, true)));		
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(201,250), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(8,12, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(201,250), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(12,14, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(201,250), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(14,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(161,200), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(6,8, false, true)));		
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(161,200), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(8,12, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(161,200), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(12,15, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(161,200), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(15,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(141,160), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(7,9, false, true)));		
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(141,160), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(9,13, false, true)));		
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(141,160), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(13,16, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(141,160), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(16,DBL_MAX, false, true)));		
		
		//Скорости ниже 140
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(121,140), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(7,9, false, true)));		
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(121,140), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(9,13, false, true)));		
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(121,140), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(13,14, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(121,140), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(14,DBL_MAX, false, true)));		
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(61,120), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(9,11, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(61,120), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(11,16, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(61,120), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(16,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(0,60), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(12,15, false, true)));		
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(0,60), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(15,20, false, true)));		
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(41,60), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(20,25, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(16,40), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(25,30, false, true)));		
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(0,15), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(30,50, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(41,60), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(25,DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(16,40), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(30,DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(0,15), SHORT_DISTORTION_LENGTH_RANGE, pro::interval<double>(50,DBL_MAX, false, true)));
			
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(121,140), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(7,9, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(121,140), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(9,13, false, true)));		
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(121,140), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(13,16, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(121,140), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(16,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(61,120), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(9,11, false, true)));		
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(61,120), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(11,16, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(61,120), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(16,20, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(61,120), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(20,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(0,60), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(12,15, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(0,60), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(15,20, false, true)));		
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(41,60), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(20,25, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(16,40), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(25,30, false, true)));		
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(0,15), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(30,50, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(41,60), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(25,DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(16,40), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(30,DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(0,15), LONG_DISTORTION_LENGTH_RANGE, pro::interval<double>(50,DBL_MAX, false, true)));
	}
};

/// Возвращает диапазон
pro::interval<double> VDistortionTable::GetRange(DEGREE in_degree, int in_speed, double in_length) const
{
	std::vector<VWaveRecord>::const_iterator i=std::find_if(table.begin(), table.end(), VWaveRecord::VFindKey(in_degree, in_speed, in_length));
	if (i!=table.end())		
		return i->GetRange();		
	else
		return EMPTY_INTERVAL<double>();
};

//----------------------------------- Методы класса VLevelRecord ------------------------------------------------

/// Конструктор
VLevelRecord::VLevelRecord(DEGREE in_degree, const pro::interval<int>& in_speed, const pro::interval<double>& in_value) :
	degree(in_degree), speed(in_speed), value(in_value) {}

/// Возвращает true, если входные данный удовлетворяют ключу
bool VLevelRecord::IsKey(DEGREE in_degree, int in_speed) const
{
	return (degree==in_degree) && speed.contains(in_speed);
}

/// Возвращает диапазон значений
const pro::interval<double> VLevelRecord::GetRange() const
{
	return value;
}

//--------------------------------- Методы класса VLevelTable ----------------------------------------------------

/// Конструктор
VLevelTable::VLevelTable()
{
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		table.push_back(VLevelRecord(DEGREE2, pro::interval<int>(121,140), pro::interval<double>(6,16, false, true)));		
		table.push_back(VLevelRecord(DEGREE3, pro::interval<int>(121,140), pro::interval<double>(16,20, false, true)));		
		table.push_back(VLevelRecord(DEGREE4, pro::interval<int>(121,140), pro::interval<double>(20,DBL_MAX, false, true)));
					
		table.push_back(VLevelRecord(DEGREE2, pro::interval<int>(61,120), pro::interval<double>(8,20, false, true)));
		table.push_back(VLevelRecord(DEGREE3, pro::interval<int>(61,120), pro::interval<double>(20,25, false, true)));				
		table.push_back(VLevelRecord(DEGREE4, pro::interval<int>(61,120), pro::interval<double>(25,DBL_MAX, false, true)));
		
		table.push_back(VLevelRecord(DEGREE2, pro::interval<int>(41,60), pro::interval<double>(10,25, false, true)));
		table.push_back(VLevelRecord(DEGREE3, pro::interval<int>(41,60), pro::interval<double>(25,30, false, true)));				
		table.push_back(VLevelRecord(DEGREE4, pro::interval<int>(41,60), pro::interval<double>(30,DBL_MAX, false, true)));
		
		table.push_back(VLevelRecord(DEGREE2, pro::interval<int>(16,40), pro::interval<double>(12,30, false, true)));
		table.push_back(VLevelRecord(DEGREE3, pro::interval<int>(16,40), pro::interval<double>(30,35, false, true)));				
		table.push_back(VLevelRecord(DEGREE4, pro::interval<int>(16,40), pro::interval<double>(35,DBL_MAX, false, true)));
		
		table.push_back(VLevelRecord(DEGREE2, pro::interval<int>(0,15), pro::interval<double>(15,35, false, true)));
		table.push_back(VLevelRecord(DEGREE3, pro::interval<int>(0,15), pro::interval<double>(35,50, false, true)));				
		table.push_back(VLevelRecord(DEGREE4, pro::interval<int>(0,15), pro::interval<double>(50,DBL_MAX, false, true)));
	}
	else if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_EXPERIMENT))
	{
		//Скорости выше 140
		table.push_back(VLevelRecord(DEGREE1, pro::interval<int>(201,250), pro::interval<double>(5,6, false, true)));		
		table.push_back(VLevelRecord(DEGREE2, pro::interval<int>(201,250), pro::interval<double>(6,13, false, true)));		
		table.push_back(VLevelRecord(DEGREE3, pro::interval<int>(201,250), pro::interval<double>(13,16, false, true)));		
		table.push_back(VLevelRecord(DEGREE4, pro::interval<int>(201,250), pro::interval<double>(16,DBL_MAX, false, true)));
				
		table.push_back(VLevelRecord(DEGREE1, pro::interval<int>(161,200), pro::interval<double>(5,6, false, true)));		
		table.push_back(VLevelRecord(DEGREE2, pro::interval<int>(161,200), pro::interval<double>(6,15, false, true)));		
		table.push_back(VLevelRecord(DEGREE3, pro::interval<int>(161,200), pro::interval<double>(15,20, false, true)));		
		table.push_back(VLevelRecord(DEGREE4, pro::interval<int>(161,200), pro::interval<double>(20,DBL_MAX, false, true)));
		
		table.push_back(VLevelRecord(DEGREE1, pro::interval<int>(141,160), pro::interval<double>(8,10, false, true)));		
		table.push_back(VLevelRecord(DEGREE2, pro::interval<int>(141,160), pro::interval<double>(10,16, false, true)));
		table.push_back(VLevelRecord(DEGREE3, pro::interval<int>(141,160), pro::interval<double>(16,20, false, true)));		
		table.push_back(VLevelRecord(DEGREE4, pro::interval<int>(141,160), pro::interval<double>(20,DBL_MAX, false, true)));
		
		//Скорости ниже 140
		table.push_back(VLevelRecord(DEGREE1, pro::interval<int>(121,140), pro::interval<double>(8,10, false, true)));		
		table.push_back(VLevelRecord(DEGREE2, pro::interval<int>(121,140), pro::interval<double>(10,16, false, true)));		
		table.push_back(VLevelRecord(DEGREE3, pro::interval<int>(121,140), pro::interval<double>(16,20, false, true)));		
		table.push_back(VLevelRecord(DEGREE4, pro::interval<int>(121,140), pro::interval<double>(20,DBL_MAX, false, true)));
		
		table.push_back(VLevelRecord(DEGREE1, pro::interval<int>(61,120), pro::interval<double>(8,11, false, true)));		
		table.push_back(VLevelRecord(DEGREE2, pro::interval<int>(61,120), pro::interval<double>(11,20, false, true)));		
		table.push_back(VLevelRecord(DEGREE3, pro::interval<int>(61,120), pro::interval<double>(20,25, false, true)));		
		table.push_back(VLevelRecord(DEGREE4, pro::interval<int>(61,120), pro::interval<double>(25,DBL_MAX, false, true)));
		
		table.push_back(VLevelRecord(DEGREE1, pro::interval<int>(0,60), pro::interval<double>(12,15, false, true)));		
		table.push_back(VLevelRecord(DEGREE2, pro::interval<int>(0,60), pro::interval<double>(15,25, false, true)));		
		table.push_back(VLevelRecord(DEGREE3, pro::interval<int>(41,60), pro::interval<double>(25,30, false, true)));
		table.push_back(VLevelRecord(DEGREE3, pro::interval<int>(16,40), pro::interval<double>(25,35, false, true)));
		table.push_back(VLevelRecord(DEGREE3, pro::interval<int>(0,15), pro::interval<double>(25,50, false, true)));
		table.push_back(VLevelRecord(DEGREE4, pro::interval<int>(41,60), pro::interval<double>(30,DBL_MAX, false, true)));
		table.push_back(VLevelRecord(DEGREE4, pro::interval<int>(16,40), pro::interval<double>(35,DBL_MAX, false, true)));
		table.push_back(VLevelRecord(DEGREE4, pro::interval<int>(0,15), pro::interval<double>(50,DBL_MAX, false, true)));
	}
}

/// Возвращает диапазон
pro::interval<double> VLevelTable::GetRange(DEGREE in_degree, int in_speed) const
{
	std::vector<VLevelRecord>::const_iterator i=std::find_if(table.begin(), table.end(), VLevelRecord::VFindKey(in_degree, in_speed));
	if (i!=table.end())		
		return i->GetRange();		
	else
		return EMPTY_INTERVAL<double>();
}

//-------------------------------- Методы класса VRichTable -------------------------------------------------------------

/// Конструктор
VRichTable::VRichTable()
{	
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(121,140), RICH_LENGTH_RANGE, pro::interval<double>(10,15, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(121,140), RICH_LENGTH_RANGE, pro::interval<double>(15,25, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(121,140), RICH_LENGTH_RANGE, pro::interval<double>(25,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(61,120), RICH_LENGTH_RANGE, pro::interval<double>(10,25, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(61,120), RICH_LENGTH_RANGE, pro::interval<double>(25,35, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(61,120), RICH_LENGTH_RANGE, pro::interval<double>(35,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(41,60), RICH_LENGTH_RANGE, pro::interval<double>(15,35, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(41,60), RICH_LENGTH_RANGE, pro::interval<double>(35,40, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(41,60), RICH_LENGTH_RANGE, pro::interval<double>(40,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(16,40), RICH_LENGTH_RANGE, pro::interval<double>(20,40, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(16,40), RICH_LENGTH_RANGE, pro::interval<double>(40,50, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(16,40), RICH_LENGTH_RANGE, pro::interval<double>(50,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(0,15), RICH_LENGTH_RANGE, pro::interval<double>(25,50, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(0,15), RICH_LENGTH_RANGE, pro::interval<double>(50,65, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(0,15), RICH_LENGTH_RANGE, pro::interval<double>(65,DBL_MAX, false, true)));
		
		
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(121,140), LONG_RICH_LENGTH_RANGE, pro::interval<double>(15,25, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(121,140), LONG_RICH_LENGTH_RANGE, pro::interval<double>(25,35, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(121,140), LONG_RICH_LENGTH_RANGE, pro::interval<double>(35,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(61,120), LONG_RICH_LENGTH_RANGE, pro::interval<double>(15,35, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(61,120), LONG_RICH_LENGTH_RANGE, pro::interval<double>(35,40, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(61,120), LONG_RICH_LENGTH_RANGE, pro::interval<double>(40,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(41,60), LONG_RICH_LENGTH_RANGE, pro::interval<double>(25,40, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(41,60), LONG_RICH_LENGTH_RANGE, pro::interval<double>(40,50, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(41,60), LONG_RICH_LENGTH_RANGE, pro::interval<double>(50,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(16,40), LONG_RICH_LENGTH_RANGE, pro::interval<double>(30,50, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(16,40), LONG_RICH_LENGTH_RANGE, pro::interval<double>(50,65, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(16,40), LONG_RICH_LENGTH_RANGE, pro::interval<double>(65,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(0,15), LONG_RICH_LENGTH_RANGE, pro::interval<double>(35,65, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(0,15), LONG_RICH_LENGTH_RANGE, pro::interval<double>(65,90, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(0,15), LONG_RICH_LENGTH_RANGE, pro::interval<double>(90,DBL_MAX, false, true)));
	}
	else if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_EXPERIMENT))
	{
		//Скорости выше 140
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(201,250), RICH_LENGTH_RANGE, pro::interval<double>(7,9, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(201,250), RICH_LENGTH_RANGE, pro::interval<double>(9,14, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(201,250), RICH_LENGTH_RANGE, pro::interval<double>(14,18, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(201,250), RICH_LENGTH_RANGE, pro::interval<double>(18,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(181,200), RICH_LENGTH_RANGE, pro::interval<double>(8,10, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(181,200), RICH_LENGTH_RANGE, pro::interval<double>(10,15, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(181,200), RICH_LENGTH_RANGE, pro::interval<double>(15,20, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(181,200), RICH_LENGTH_RANGE, pro::interval<double>(20,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(161,180), RICH_LENGTH_RANGE, pro::interval<double>(9,10, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(161,180), RICH_LENGTH_RANGE, pro::interval<double>(10,15, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(161,180), RICH_LENGTH_RANGE, pro::interval<double>(15,25, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(161,180), RICH_LENGTH_RANGE, pro::interval<double>(25,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(141,160), RICH_LENGTH_RANGE, pro::interval<double>(9,10, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(141,160), RICH_LENGTH_RANGE, pro::interval<double>(10,15, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(141,160), RICH_LENGTH_RANGE, pro::interval<double>(15,25, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(141,160), RICH_LENGTH_RANGE, pro::interval<double>(25,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(201,250), LONG_RICH_LENGTH_RANGE, pro::interval<double>(10,12, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(201,250), LONG_RICH_LENGTH_RANGE, pro::interval<double>(12,18, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(201,250), LONG_RICH_LENGTH_RANGE, pro::interval<double>(18,25, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(201,250), LONG_RICH_LENGTH_RANGE, pro::interval<double>(25,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(181,200), LONG_RICH_LENGTH_RANGE, pro::interval<double>(12,15, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(181,200), LONG_RICH_LENGTH_RANGE, pro::interval<double>(15,18, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(181,200), LONG_RICH_LENGTH_RANGE, pro::interval<double>(18,27, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(181,200), LONG_RICH_LENGTH_RANGE, pro::interval<double>(27,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(161,180), LONG_RICH_LENGTH_RANGE, pro::interval<double>(15,18, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(161,180), LONG_RICH_LENGTH_RANGE, pro::interval<double>(18,20, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(161,180), LONG_RICH_LENGTH_RANGE, pro::interval<double>(20,30, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(161,180), LONG_RICH_LENGTH_RANGE, pro::interval<double>(30,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(141,160), LONG_RICH_LENGTH_RANGE, pro::interval<double>(16,20, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(141,160), LONG_RICH_LENGTH_RANGE, pro::interval<double>(20,25, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(141,160), LONG_RICH_LENGTH_RANGE, pro::interval<double>(25,35, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(141,160), LONG_RICH_LENGTH_RANGE, pro::interval<double>(35,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(201,250), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(12,15, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(201,250), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(15,20, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(201,250), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(20,27, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(201,250), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(27,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(181,200), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(15,18, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(181,200), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(18,20, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(181,200), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(20,30, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(181,200), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(30,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(161,180), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(18,22, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(161,180), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(22,25, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(161,180), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(25,35, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(161,180), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(35,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(141,160), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(18,22, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(141,160), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(22,27, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(141,160), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(27,40, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(141,160), VERY_LONG_RICH_LENGTH_RANGE, pro::interval<double>(40,DBL_MAX, false, true)));
		
		//Скорости ниже 140

		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(121,140), RICH_LENGTH_RANGE, pro::interval<double>(9,10, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(121,140), RICH_LENGTH_RANGE, pro::interval<double>(10,15, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(121,140), RICH_LENGTH_RANGE, pro::interval<double>(15,25, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(121,140), RICH_LENGTH_RANGE, pro::interval<double>(25,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(61,120), RICH_LENGTH_RANGE, pro::interval<double>(15,20, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(61,120), RICH_LENGTH_RANGE, pro::interval<double>(20,25, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(61,120), RICH_LENGTH_RANGE, pro::interval<double>(25,35, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(61,120), RICH_LENGTH_RANGE, pro::interval<double>(35,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(0,60), RICH_LENGTH_RANGE, pro::interval<double>(25,25, true, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(0,60), RICH_LENGTH_RANGE, pro::interval<double>(25,35, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(41,60), RICH_LENGTH_RANGE, pro::interval<double>(35,40, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(16,40), RICH_LENGTH_RANGE, pro::interval<double>(35,50, false, true)));		
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(0,15), RICH_LENGTH_RANGE, pro::interval<double>(35,65, false, true)));		
		//FixMe!!! учесть другие нормы для кривых радиусом менее 850 м 
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(41,60), RICH_LENGTH_RANGE, pro::interval<double>(40,DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(16,40), RICH_LENGTH_RANGE, pro::interval<double>(50,DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(0,15), RICH_LENGTH_RANGE, pro::interval<double>(65,DBL_MAX, false, true)));

		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(121,140), LONG_RICH_LENGTH_RANGE, pro::interval<double>(16,20, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(121,140), LONG_RICH_LENGTH_RANGE, pro::interval<double>(20,25, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(121,140), LONG_RICH_LENGTH_RANGE, pro::interval<double>(25,35, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(121,140), LONG_RICH_LENGTH_RANGE, pro::interval<double>(35,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(61,120), LONG_RICH_LENGTH_RANGE, pro::interval<double>(21,25, false, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(61,120), LONG_RICH_LENGTH_RANGE, pro::interval<double>(25,35, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(61,120), LONG_RICH_LENGTH_RANGE, pro::interval<double>(35,40, false, true)));		
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(61,120), LONG_RICH_LENGTH_RANGE, pro::interval<double>(40,DBL_MAX, false, true)));
		
		table.push_back(VWaveRecord(DEGREE1, pro::interval<int>(0,60), LONG_RICH_LENGTH_RANGE, pro::interval<double>(30,30, true, true)));
		table.push_back(VWaveRecord(DEGREE2, pro::interval<int>(0,60), LONG_RICH_LENGTH_RANGE, pro::interval<double>(30,40, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(41,60), LONG_RICH_LENGTH_RANGE, pro::interval<double>(40,50, false, true)));
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(16,40), LONG_RICH_LENGTH_RANGE, pro::interval<double>(40,65, false, true)));		
		table.push_back(VWaveRecord(DEGREE3, pro::interval<int>(0,15), LONG_RICH_LENGTH_RANGE, pro::interval<double>(40,90, false, true)));				
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(41,60), LONG_RICH_LENGTH_RANGE, pro::interval<double>(50,DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(16,40), LONG_RICH_LENGTH_RANGE, pro::interval<double>(65,DBL_MAX, false, true)));
		table.push_back(VWaveRecord(DEGREE4, pro::interval<int>(0,15), LONG_RICH_LENGTH_RANGE, pro::interval<double>(90,DBL_MAX, false, true)));
	}
};	

/// Возвращает диапазон
pro::interval<double> VRichTable::GetRange(DEGREE in_degree, int in_speed, double in_length) const
{
	std::vector<VWaveRecord>::const_iterator i=std::find_if(table.begin(), table.end(), VWaveRecord::VFindKey(in_degree, in_speed, in_length));
	if (i!=table.end())		
		return i->GetRange();		
	else
		return EMPTY_INTERVAL<double>();
};

//------------------------------------ Бальная оценка отступления -------------------------------------

int VPenaltyTable::GetPenalty(int in_norma, int in_speed, double in_value, int in_add_key) const
{
	std::vector<VPenaltyRecord>::const_iterator it = 
		std::find_if(this->table.begin(), this->table.end(), 
						VPenaltyRecord::VFindKey(in_norma, in_speed, in_value, in_add_key));

	if (it == this->table.end())	return 0;
	else							return it->GetPenalty();
}

VWidenPenaltyTable::VWidenPenaltyTable()
{
	// 1520
	// -----------------------------------------------------------------------------------------------------------------------
	// >200
	this->table.push_back(VPenaltyRecord(1, 1520, pro::interval<int>(201, 250), pro::interval<double>(1525, 1526, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1520, pro::interval<int>(201, 250), pro::interval<double>(1526, 1527, true, false)));
	this->table.push_back(VPenaltyRecord(3, 1520, pro::interval<int>(201, 250), pro::interval<double>(1527, 1528, true, false)));
	this->table.push_back(VPenaltyRecord(5, 1520, pro::interval<int>(201, 250), pro::interval<double>(1528, 1529, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1520, pro::interval<int>(201, 250), pro::interval<double>(1529, 1531, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1520, pro::interval<int>(201, 250), pro::interval<double>(1531, 1533, true, false)));
	this->table.push_back(VPenaltyRecord(32, 1520, pro::interval<int>(201, 250), pro::interval<double>(1533, 1535, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1520, pro::interval<int>(201, 250), pro::interval<double>(1535, DBL_MAX, true, false)));
	// > 140, R > 3000 (add_key == false)
	this->table.push_back(VPenaltyRecord(1, 1520, pro::interval<int>(141, 200), pro::interval<double>(1529, 1530, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(2, 1520, pro::interval<int>(141, 200), pro::interval<double>(1530, 1531, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(3, 1520, pro::interval<int>(141, 200), pro::interval<double>(1531, 1532, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(5, 1520, pro::interval<int>(141, 200), pro::interval<double>(1532, 1533, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(18, 1520, pro::interval<int>(141, 200), pro::interval<double>(1533, 1534, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(24, 1520, pro::interval<int>(141, 200), pro::interval<double>(1534, 1535, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(100, 1520, pro::interval<int>(141, 200), pro::interval<double>(1535, DBL_MAX, true, false), pro::interval<int>(0, 0)));
	// > 140, R <= 3000 (add_key == false)
	this->table.push_back(VPenaltyRecord(1, 1520, pro::interval<int>(141, 200), pro::interval<double>(1529, 1531, true, false), pro::interval<int>(1, INT_MAX)));
	this->table.push_back(VPenaltyRecord(2, 1520, pro::interval<int>(141, 200), pro::interval<double>(1531, 1533, true, false), pro::interval<int>(1, INT_MAX)));
	this->table.push_back(VPenaltyRecord(3, 1520, pro::interval<int>(141, 200), pro::interval<double>(1533, 1534, true, false), pro::interval<int>(1, INT_MAX)));
	this->table.push_back(VPenaltyRecord(5, 1520, pro::interval<int>(141, 200), pro::interval<double>(1534, 1535, true, false), pro::interval<int>(1, INT_MAX)));
	this->table.push_back(VPenaltyRecord(18, 1520, pro::interval<int>(141, 200), pro::interval<double>(1535, 1536, true, false), pro::interval<int>(1, INT_MAX)));
	this->table.push_back(VPenaltyRecord(24, 1520, pro::interval<int>(141, 200), pro::interval<double>(1536, 1537, true, false), pro::interval<int>(1, INT_MAX)));
	this->table.push_back(VPenaltyRecord(100, 1520, pro::interval<int>(141, 200), pro::interval<double>(1537, DBL_MAX, true, false), pro::interval<int>(1, INT_MAX)));
	// >120
	this->table.push_back(VPenaltyRecord(1, 1520, pro::interval<int>(121, 140), pro::interval<double>(1529, 1531, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1520, pro::interval<int>(121, 140), pro::interval<double>(1531, 1533, true, false)));
	this->table.push_back(VPenaltyRecord(3, 1520, pro::interval<int>(121, 140), pro::interval<double>(1533, 1534, true, false)));
	this->table.push_back(VPenaltyRecord(5, 1520, pro::interval<int>(121, 140), pro::interval<double>(1534, 1535, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1520, pro::interval<int>(121, 140), pro::interval<double>(1535, 1536, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1520, pro::interval<int>(121, 140), pro::interval<double>(1536, 1537, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1520, pro::interval<int>(121, 140), pro::interval<double>(1537, DBL_MAX, true, false)));
	// >60
	this->table.push_back(VPenaltyRecord(1, 1520, pro::interval<int>(61, 120), pro::interval<double>(1529, 1533, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1520, pro::interval<int>(61, 120), pro::interval<double>(1533, 1536, true, false)));
	this->table.push_back(VPenaltyRecord(3, 1520, pro::interval<int>(61, 120), pro::interval<double>(1536, 1538, true, false)));
	this->table.push_back(VPenaltyRecord(5, 1520, pro::interval<int>(61, 120), pro::interval<double>(1538, 1539, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1520, pro::interval<int>(61, 120), pro::interval<double>(1539, 1541, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1520, pro::interval<int>(61, 120), pro::interval<double>(1541, 1542, true, false)));
	this->table.push_back(VPenaltyRecord(32, 1520, pro::interval<int>(61, 120), pro::interval<double>(1542, 1543, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1520, pro::interval<int>(61, 120), pro::interval<double>(1543, DBL_MAX, true, false)));
	// >25
	this->table.push_back(VPenaltyRecord(1, 1520, pro::interval<int>(26, 60), pro::interval<double>(1529, 1534, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1520, pro::interval<int>(26, 60), pro::interval<double>(1534, 1538, true, false)));
	this->table.push_back(VPenaltyRecord(3, 1520, pro::interval<int>(26, 60), pro::interval<double>(1538, 1540, true, false)));
	this->table.push_back(VPenaltyRecord(5, 1520, pro::interval<int>(26, 60), pro::interval<double>(1540, 1541, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1520, pro::interval<int>(26, 60), pro::interval<double>(1541, 1543, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1520, pro::interval<int>(26, 60), pro::interval<double>(1543, 1545, true, false)));
	this->table.push_back(VPenaltyRecord(32, 1520, pro::interval<int>(26, 60), pro::interval<double>(1545, 1547, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1520, pro::interval<int>(26, 60), pro::interval<double>(1547, DBL_MAX, true, false)));
	// <=25
	this->table.push_back(VPenaltyRecord(1, 1520, pro::interval<int>(0, 25), pro::interval<double>(1531, 1537, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1520, pro::interval<int>(0, 25), pro::interval<double>(1537, 1542, true, false)));
	this->table.push_back(VPenaltyRecord(3, 1520, pro::interval<int>(0, 25), pro::interval<double>(1542, 1546, true, false)));
	this->table.push_back(VPenaltyRecord(5, 1520, pro::interval<int>(0, 25), pro::interval<double>(1546, 1547, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1520, pro::interval<int>(0, 25), pro::interval<double>(1547, 1548, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1520, pro::interval<int>(0, 25), pro::interval<double>(1548, 1549, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1520, pro::interval<int>(0, 25), pro::interval<double>(1549, DBL_MAX, true, false)));

	// 1524
	// -----------------------------------------------------------------------------------------------------------------------
	// >120
	this->table.push_back(VPenaltyRecord(1, 1524, pro::interval<int>(121, 140), pro::interval<double>(1533, 1534, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1524, pro::interval<int>(121, 140), pro::interval<double>(1534, 1535, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1524, pro::interval<int>(121, 140), pro::interval<double>(1535, 1536, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1524, pro::interval<int>(121, 140), pro::interval<double>(1536, 1537, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1524, pro::interval<int>(121, 140), pro::interval<double>(1537, DBL_MAX, true, false)));
	// >60
	this->table.push_back(VPenaltyRecord(1, 1524, pro::interval<int>(61, 120), pro::interval<double>(1533, 1534, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1524, pro::interval<int>(61, 120), pro::interval<double>(1534, 1535, true, false)));
	this->table.push_back(VPenaltyRecord(3, 1524, pro::interval<int>(61, 120), pro::interval<double>(1535, 1536, true, false)));
	this->table.push_back(VPenaltyRecord(5, 1524, pro::interval<int>(61, 120), pro::interval<double>(1536, 1537, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1524, pro::interval<int>(61, 120), pro::interval<double>(1537, 1539, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1524, pro::interval<int>(61, 120), pro::interval<double>(1539, 1540, true, false)));
	this->table.push_back(VPenaltyRecord(32, 1524, pro::interval<int>(61, 120), pro::interval<double>(1540, 1541, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1524, pro::interval<int>(61, 120), pro::interval<double>(1541, DBL_MAX, true, false)));
	// >25
	this->table.push_back(VPenaltyRecord(1, 1524, pro::interval<int>(26, 60), pro::interval<double>(1533, 1536, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1524, pro::interval<int>(26, 60), pro::interval<double>(1536, 1538, true, false)));
	this->table.push_back(VPenaltyRecord(3, 1524, pro::interval<int>(26, 60), pro::interval<double>(1538, 1540, true, false)));
	this->table.push_back(VPenaltyRecord(5, 1524, pro::interval<int>(26, 60), pro::interval<double>(1540, 1541, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1524, pro::interval<int>(26, 60), pro::interval<double>(1541, 1543, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1524, pro::interval<int>(26, 60), pro::interval<double>(1543, 1544, true, false)));
	this->table.push_back(VPenaltyRecord(32, 1524, pro::interval<int>(26, 60), pro::interval<double>(1544, 1545, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1524, pro::interval<int>(26, 60), pro::interval<double>(1545, DBL_MAX, true, false)));
	// <=25
	this->table.push_back(VPenaltyRecord(1, 1524, pro::interval<int>(0, 25), pro::interval<double>(1535, 1540, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1524, pro::interval<int>(0, 25), pro::interval<double>(1540, 1543, true, false)));
	this->table.push_back(VPenaltyRecord(3, 1524, pro::interval<int>(0, 25), pro::interval<double>(1543, 1546, true, false)));
	this->table.push_back(VPenaltyRecord(5, 1524, pro::interval<int>(0, 25), pro::interval<double>(1546, 1547, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1524, pro::interval<int>(0, 25), pro::interval<double>(1547, 1548, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1524, pro::interval<int>(0, 25), pro::interval<double>(1548, 1549, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1524, pro::interval<int>(0, 25), pro::interval<double>(1549, DBL_MAX, true, false)));

	// 1530
	// -----------------------------------------------------------------------------------------------------------------------
	// >120
	this->table.push_back(VPenaltyRecord(1, 1530, pro::interval<int>(121, 140), pro::interval<double>(1537, 1538, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1530, pro::interval<int>(121, 140), pro::interval<double>(1538, 1539, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1530, pro::interval<int>(121, 140), pro::interval<double>(1539, 1540, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1530, pro::interval<int>(121, 140), pro::interval<double>(1540, 1541, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1530, pro::interval<int>(121, 140), pro::interval<double>(1541, DBL_MAX, true, false)));
	// >25
	this->table.push_back(VPenaltyRecord(1, 1530, pro::interval<int>(26, 120), pro::interval<double>(1539, 1542, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1530, pro::interval<int>(26, 120), pro::interval<double>(1542, 1543, true, false)));
	this->table.push_back(VPenaltyRecord(3, 1530, pro::interval<int>(26, 120), pro::interval<double>(1543, 1544, true, false)));
	this->table.push_back(VPenaltyRecord(5, 1530, pro::interval<int>(26, 120), pro::interval<double>(1544, 1545, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1530, pro::interval<int>(26, 120), pro::interval<double>(1545, 1546, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1530, pro::interval<int>(26, 120), pro::interval<double>(1546, 1547, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1530, pro::interval<int>(26, 120), pro::interval<double>(1547, DBL_MAX, true, false)));
	// <=25
	this->table.push_back(VPenaltyRecord(1, 1530, pro::interval<int>(0, 25), pro::interval<double>(1541, 1544, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1530, pro::interval<int>(0, 25), pro::interval<double>(1544, 1545, true, false)));
	this->table.push_back(VPenaltyRecord(3, 1530, pro::interval<int>(0, 25), pro::interval<double>(1545, 1546, true, false)));
	this->table.push_back(VPenaltyRecord(5, 1530, pro::interval<int>(0, 25), pro::interval<double>(1546, 1547, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1530, pro::interval<int>(0, 25), pro::interval<double>(1547, 1548, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1530, pro::interval<int>(0, 25), pro::interval<double>(1548, 1549, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1530, pro::interval<int>(0, 25), pro::interval<double>(1549, DBL_MAX, true, false)));

	// 1535
	// -----------------------------------------------------------------------------------------------------------------------
	// >60
	this->table.push_back(VPenaltyRecord(1, 1535, pro::interval<int>(61, 120), pro::interval<double>(1542, 1543, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1535, pro::interval<int>(61, 120), pro::interval<double>(1543, 1544, true, false)));
	this->table.push_back(VPenaltyRecord(3, 1535, pro::interval<int>(61, 120), pro::interval<double>(1544, 1545, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1535, pro::interval<int>(61, 120), pro::interval<double>(1545, 1546, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1535, pro::interval<int>(61, 120), pro::interval<double>(1546, 1547, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1535, pro::interval<int>(61, 120), pro::interval<double>(1547, DBL_MAX, true, false)));
	// >25
	this->table.push_back(VPenaltyRecord(1, 1535, pro::interval<int>(26, 60), pro::interval<double>(1544, 1545, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1535, pro::interval<int>(26, 60), pro::interval<double>(1545, 1546, true, false)));
	this->table.push_back(VPenaltyRecord(3, 1535, pro::interval<int>(26, 60), pro::interval<double>(1546, 1547, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1535, pro::interval<int>(26, 60), pro::interval<double>(1547, 1548, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1535, pro::interval<int>(26, 60), pro::interval<double>(1548, 1549, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1535, pro::interval<int>(26, 60), pro::interval<double>(1549, DBL_MAX, true, false)));
	// <=25
	this->table.push_back(VPenaltyRecord(1, 1535, pro::interval<int>(0, 25), pro::interval<double>(1545, 1546, true, false)));
	this->table.push_back(VPenaltyRecord(2, 1535, pro::interval<int>(0, 25), pro::interval<double>(1546, 1547, true, false)));
	this->table.push_back(VPenaltyRecord(18, 1535, pro::interval<int>(0, 25), pro::interval<double>(1547, 1548, true, false)));
	this->table.push_back(VPenaltyRecord(24, 1535, pro::interval<int>(0, 25), pro::interval<double>(1548, 1549, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1535, pro::interval<int>(0, 25), pro::interval<double>(1549, DBL_MAX, true, false)));

	// 1540
	// -----------------------------------------------------------------------------------------------------------------------
	// <=60
	this->table.push_back(VPenaltyRecord(1, 1535, pro::interval<int>(0, 60), pro::interval<double>(1547, 1548, true, false)));
	this->table.push_back(VPenaltyRecord(5, 1535, pro::interval<int>(0, 60), pro::interval<double>(1548, 1549, true, false)));
	this->table.push_back(VPenaltyRecord(100, 1535, pro::interval<int>(0, 60), pro::interval<double>(1549, DBL_MAX, true, false)));
}

VNarrowPenaltyTable::VNarrowPenaltyTable()
{
	// 1520
	// -----------------------------------------------------------------------------------------------------------------------
	// >200
	this->table.push_back(VPenaltyRecord(100, 1520, pro::interval<int>(201, 250), pro::interval<double>(0, 1513, false, true)));
	this->table.push_back(VPenaltyRecord(24, 1520, pro::interval<int>(201, 250), pro::interval<double>(1513, 1514, false, true)));
	this->table.push_back(VPenaltyRecord(18, 1520, pro::interval<int>(201, 250), pro::interval<double>(1514, 1515, false, true)));
	this->table.push_back(VPenaltyRecord(2, 1520, pro::interval<int>(201, 250), pro::interval<double>(1515, 1516, false, true)));
	this->table.push_back(VPenaltyRecord(1, 1520, pro::interval<int>(201, 250), pro::interval<double>(1516, 1517, false, true)));
	// >140
	this->table.push_back(VPenaltyRecord(100, 1520, pro::interval<int>(141, 250), pro::interval<double>(0, 1511, false, true)));
	this->table.push_back(VPenaltyRecord(24, 1520, pro::interval<int>(141, 250), pro::interval<double>(1511, 1512, false, true)));
	this->table.push_back(VPenaltyRecord(18, 1520, pro::interval<int>(141, 250), pro::interval<double>(1512, 1513, false, true)));
	this->table.push_back(VPenaltyRecord(2, 1520, pro::interval<int>(141, 250), pro::interval<double>(1513, 1514, false, true)));
	this->table.push_back(VPenaltyRecord(1, 1520, pro::interval<int>(141, 250), pro::interval<double>(1514, 1515, false, true)));
	// >120, без жбш до 96 г.
	this->table.push_back(VPenaltyRecord(100, 1520, pro::interval<int>(0, 120), pro::interval<double>(0, 1511, false, true), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(24, 1520, pro::interval<int>(0, 120), pro::interval<double>(1511, 1512, false, true), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(18, 1520, pro::interval<int>(0, 120), pro::interval<double>(1512, 1513, false, true), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(2, 1520, pro::interval<int>(0, 120), pro::interval<double>(1513, 1514, false, true), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(1, 1520, pro::interval<int>(0, 120), pro::interval<double>(1514, 1515, false, true), pro::interval<int>(0, 0)));
	// >120, жбш до 96 г.
	this->table.push_back(VPenaltyRecord(100, 1520, pro::interval<int>(0, 120), pro::interval<double>(0, 1509, false, true), pro::interval<int>(1, INT_MAX)));
	this->table.push_back(VPenaltyRecord(24, 1520, pro::interval<int>(0, 120), pro::interval<double>(1509, 1510, false, true), pro::interval<int>(1, INT_MAX)));
	this->table.push_back(VPenaltyRecord(18, 1520, pro::interval<int>(0, 120), pro::interval<double>(1510, 1511, false, true), pro::interval<int>(1, INT_MAX)));
	this->table.push_back(VPenaltyRecord(2, 1520, pro::interval<int>(0, 120), pro::interval<double>(1511, 1512, false, true), pro::interval<int>(1, INT_MAX)));
	this->table.push_back(VPenaltyRecord(1, 1520, pro::interval<int>(0, 120), pro::interval<double>(1512, 1513, false, true), pro::interval<int>(1, INT_MAX)));
}

VDistortionPenaltyTable::VDistortionPenaltyTable()
{
	// >200, >10m
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(201, 250), pro::interval<double>(9, 10, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(201, 250), pro::interval<double>(10, 11, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(201, 250), pro::interval<double>(11, 12, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(201, 250), pro::interval<double>(12, 13, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(201, 250), pro::interval<double>(13, 14, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(201, 250), pro::interval<double>(14, 15, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(201, 250), pro::interval<double>(15, DBL_MAX, true, false), pro::interval<int>(0, 0)));	
	// >200, <=10m
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(201, 250), pro::interval<double>(9, 10, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(201, 250), pro::interval<double>(10, 11, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(201, 250), pro::interval<double>(11, 12, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(28, 0, pro::interval<int>(201, 250), pro::interval<double>(12, 13, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(201, 250), pro::interval<double>(13, DBL_MAX, true, false), pro::interval<int>(1, 2)));	
	// >160, >10m
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(161, 200), pro::interval<double>(9, 10, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(161, 200), pro::interval<double>(10, 11, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(161, 200), pro::interval<double>(11, 12, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(161, 200), pro::interval<double>(12, 13, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(161, 200), pro::interval<double>(13, 14, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(161, 200), pro::interval<double>(14, 15, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(161, 200), pro::interval<double>(15, 16, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(161, 200), pro::interval<double>(16, DBL_MAX, true, false), pro::interval<int>(0, 0)));	
	// >160, <=10m
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(161, 200), pro::interval<double>(9, 10, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(161, 200), pro::interval<double>(10, 11, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(161, 200), pro::interval<double>(11, 12, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(161, 200), pro::interval<double>(12, 13, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(161, 200), pro::interval<double>(13, 14, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(161, 200), pro::interval<double>(14, DBL_MAX, true, false), pro::interval<int>(1, 2)));	
	// >140, >10m
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(141, 160), pro::interval<double>(9, 10, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(141, 160), pro::interval<double>(10, 11, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(141, 160), pro::interval<double>(11, 12, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(141, 160), pro::interval<double>(12, 13, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(141, 160), pro::interval<double>(13, 14, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(141, 160), pro::interval<double>(14, 15, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(141, 160), pro::interval<double>(15, 16, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(141, 160), pro::interval<double>(16, 17, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(141, 160), pro::interval<double>(17, DBL_MAX, true, false), pro::interval<int>(0, 0)));	
	// >140, <=10m
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(141, 160), pro::interval<double>(9, 10, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(141, 160), pro::interval<double>(10, 11, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(141, 160), pro::interval<double>(11, 12, true, false), pro::interval<int>(1, 3)));	
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(141, 160), pro::interval<double>(12, 13, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(141, 160), pro::interval<double>(13, 14, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(141, 160), pro::interval<double>(14, 15, true, false), pro::interval<int>(1, 2)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(141, 160), pro::interval<double>(15, DBL_MAX, true, false), pro::interval<int>(1, 2)));	
	// >120, >10m
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(121, 140), pro::interval<double>(10, 11, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(121, 140), pro::interval<double>(11, 12, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(121, 140), pro::interval<double>(12, 13, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(121, 140), pro::interval<double>(13, 14, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(121, 140), pro::interval<double>(14, 15, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(121, 140), pro::interval<double>(15, 16, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(121, 140), pro::interval<double>(16, 17, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(121, 140), pro::interval<double>(17, DBL_MAX, true, false), pro::interval<int>(0, 0)));	
	// >120, <10m
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(121, 140), pro::interval<double>(10, 11, true, false), pro::interval<int>(1, 1)));	
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(121, 140), pro::interval<double>(11, 12, true, false), pro::interval<int>(1, 1)));	
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(121, 140), pro::interval<double>(12, 13, true, false), pro::interval<int>(1, 1)));	
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(121, 140), pro::interval<double>(13, 14, true, false), pro::interval<int>(1, 1)));	
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(121, 140), pro::interval<double>(14, 15, true, false), pro::interval<int>(1, 1)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(121, 140), pro::interval<double>(14, DBL_MAX, true, false), pro::interval<int>(1, 1)));	
	// >120, <10m & R<850
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(121, 140), pro::interval<double>(10, 11, true, false), pro::interval<int>(2, 2)));	
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(121, 140), pro::interval<double>(11, 12, true, false), pro::interval<int>(2, 2)));	
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(121, 140), pro::interval<double>(12, 13, true, false), pro::interval<int>(2, 2)));	
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(121, 140), pro::interval<double>(13, 14, true, false), pro::interval<int>(2, 2)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(121, 140), pro::interval<double>(14, DBL_MAX, true, false), pro::interval<int>(2, 2)));	
	// >60, >10m
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(61, 120), pro::interval<double>(12, 13, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(61, 120), pro::interval<double>(13, 14, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(61, 120), pro::interval<double>(14, 15, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(61, 120), pro::interval<double>(15, 16, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(61, 120), pro::interval<double>(16, 17, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(61, 120), pro::interval<double>(17, 18, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(61, 120), pro::interval<double>(18, 19, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(61, 120), pro::interval<double>(19, 20, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(61, 120), pro::interval<double>(20, 21, true, false), pro::interval<int>(0, 0)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(61, 120), pro::interval<double>(21, DBL_MAX, true, false), pro::interval<int>(0, 0)));	
	// >60, <10m
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(61, 120), pro::interval<double>(12, 13, true, false), pro::interval<int>(1, 1)));	
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(61, 120), pro::interval<double>(13, 14, true, false), pro::interval<int>(1, 1)));	
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(61, 120), pro::interval<double>(14, 15, true, false), pro::interval<int>(1, 1)));	
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(61, 120), pro::interval<double>(15, 16, true, false), pro::interval<int>(1, 1)));	
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(61, 120), pro::interval<double>(16, 17, true, false), pro::interval<int>(1, 1)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(61, 120), pro::interval<double>(17, DBL_MAX, true, false), pro::interval<int>(1, 1)));	
	// >60, <10m && R<850
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(61, 120), pro::interval<double>(12, 13, true, false), pro::interval<int>(2, 2)));	
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(61, 120), pro::interval<double>(13, 14, true, false), pro::interval<int>(2, 2)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(61, 120), pro::interval<double>(14, DBL_MAX, true, false), pro::interval<int>(2, 2)));	
	// >40
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(41, 60), pro::interval<double>(16, 17, true, false)));	
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(41, 60), pro::interval<double>(17, 18, true, false)));	
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(41, 60), pro::interval<double>(18, 19, true, false)));	
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(41, 60), pro::interval<double>(19, 20, true, false)));	
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(41, 60), pro::interval<double>(20, 21, true, false)));	
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(41, 60), pro::interval<double>(21, 22, true, false)));	
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(41, 60), pro::interval<double>(22, 23, true, false)));	
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(41, 60), pro::interval<double>(23, 24, true, false)));	
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(41, 60), pro::interval<double>(24, 25, true, false)));	
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(41, 60), pro::interval<double>(25, 26, true, false)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(41, 60), pro::interval<double>(26, DBL_MAX, true, false)));	
	// >15
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(16, 40), pro::interval<double>(16, 17, true, false)));	
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(16, 40), pro::interval<double>(17, 18, true, false)));	
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(16, 40), pro::interval<double>(18, 19, true, false)));	
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(16, 40), pro::interval<double>(19, 20, true, false)));	
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(16, 40), pro::interval<double>(20, 21, true, false)));	
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(16, 40), pro::interval<double>(21, 23, true, false)));	
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(16, 40), pro::interval<double>(23, 25, true, false)));	
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(16, 40), pro::interval<double>(25, 27, true, false)));	
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(16, 40), pro::interval<double>(27, 29, true, false)));	
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(16, 40), pro::interval<double>(29, 31, true, false)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(16, 40), pro::interval<double>(31, DBL_MAX, true, false)));	
	// <=15
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(0, 15), pro::interval<double>(16, 17, true, false)));	
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(0, 15), pro::interval<double>(17, 18, true, false)));	
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(0, 15), pro::interval<double>(18, 19, true, false)));	
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(0, 15), pro::interval<double>(19, 20, true, false)));	
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(0, 15), pro::interval<double>(20, 21, true, false)));	
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(0, 15), pro::interval<double>(21, 25, true, false)));	
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(0, 15), pro::interval<double>(25, 32, true, false)));	
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(0, 15), pro::interval<double>(32, 39, true, false)));	
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(0, 15), pro::interval<double>(39, 46, true, false)));	
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(0, 15), pro::interval<double>(46, 51, true, false)));	
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(0, 15), pro::interval<double>(51, DBL_MAX, true, false)));	
}

VPitPenaltyTable::VPitPenaltyTable()
{
	// >200
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(201, 250), pro::interval<double>(9, 10, true, false)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(201, 250), pro::interval<double>(10, 11, true, false)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(201, 250), pro::interval<double>(11, 12, true, false)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(201, 250), pro::interval<double>(12, 13, true, false)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(201, 250), pro::interval<double>(13, 14, true, false)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(201, 250), pro::interval<double>(14, 15, true, false)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(201, 250), pro::interval<double>(15, 16, true, false)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(201, 250), pro::interval<double>(16, 17, true, false)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(201, 250), pro::interval<double>(17, DBL_MAX, true, false)));
	// >140
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(141, 200), pro::interval<double>(9, 11, true, false)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(141, 200), pro::interval<double>(11, 12, true, false)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(141, 200), pro::interval<double>(12, 13, true, false)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(141, 200), pro::interval<double>(13, 14, true, false)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(141, 200), pro::interval<double>(14, 15, true, false)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(141, 200), pro::interval<double>(15, 16, true, false)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(141, 200), pro::interval<double>(16, 17, true, false)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(141, 200), pro::interval<double>(17, 18, true, false)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(141, 200), pro::interval<double>(18, 19, true, false)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(141, 200), pro::interval<double>(19, DBL_MAX, true, false)));
	// >120
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(121, 140), pro::interval<double>(12, 13, true, false)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(121, 140), pro::interval<double>(13, 14, true, false)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(121, 140), pro::interval<double>(14, 15, true, false)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(121, 140), pro::interval<double>(15, 16, true, false)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(121, 140), pro::interval<double>(16, 17, true, false)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(121, 140), pro::interval<double>(17, 18, true, false)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(121, 140), pro::interval<double>(18, 19, true, false)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(121, 140), pro::interval<double>(19, DBL_MAX, true, false)));
	// >60
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(61, 120), pro::interval<double>(15, 17, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(61, 120), pro::interval<double>(17, 18, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(61, 120), pro::interval<double>(18, 19, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(61, 120), pro::interval<double>(19, 20, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(61, 120), pro::interval<double>(20, 21, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(61, 120), pro::interval<double>(21, 22, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(61, 120), pro::interval<double>(22, 23, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(61, 120), pro::interval<double>(23, 24, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(61, 120), pro::interval<double>(24, 25, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(61, 120), pro::interval<double>(25, 26, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(61, 120), pro::interval<double>(26, DBL_MAX, true, false), pro::interval<int>(0, 0)));
	// >60 & R<850
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(61, 120), pro::interval<double>(15, 17, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(61, 120), pro::interval<double>(17, 18, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(61, 120), pro::interval<double>(18, 19, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(61, 120), pro::interval<double>(19, 20, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(61, 120), pro::interval<double>(20, 21, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(61, 120), pro::interval<double>(21, DBL_MAX, true, false), pro::interval<int>(1, 1)));
	// >40
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(41, 60), pro::interval<double>(21, 22, true, false)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(41, 60), pro::interval<double>(22, 23, true, false)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(41, 60), pro::interval<double>(23, 24, true, false)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(41, 60), pro::interval<double>(24, 25, true, false)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(41, 60), pro::interval<double>(25, 26, true, false)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(41, 60), pro::interval<double>(26, 27, true, false)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(41, 60), pro::interval<double>(27, 28, true, false)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(41, 60), pro::interval<double>(28, 29, true, false)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(41, 60), pro::interval<double>(29, 30, true, false)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(41, 60), pro::interval<double>(30, 31, true, false)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(41, 60), pro::interval<double>(31, DBL_MAX, true, false)));
	// >15
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(16, 40), pro::interval<double>(21, 22, true, false)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(16, 40), pro::interval<double>(22, 23, true, false)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(16, 40), pro::interval<double>(23, 24, true, false)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(16, 40), pro::interval<double>(24, 25, true, false)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(16, 40), pro::interval<double>(25, 26, true, false)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(16, 40), pro::interval<double>(26, 28, true, false)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(16, 40), pro::interval<double>(28, 30, true, false)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(16, 40), pro::interval<double>(30, 32, true, false)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(16, 40), pro::interval<double>(32, 34, true, false)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(16, 40), pro::interval<double>(34, 36, true, false)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(16, 40), pro::interval<double>(36, DBL_MAX, true, false)));
	// <=15
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(0, 15), pro::interval<double>(21, 22, true, false)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(0, 15), pro::interval<double>(22, 23, true, false)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(0, 15), pro::interval<double>(23, 24, true, false)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(0, 15), pro::interval<double>(24, 25, true, false)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(0, 15), pro::interval<double>(25, 26, true, false)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(0, 15), pro::interval<double>(26, 29, true, false)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(0, 15), pro::interval<double>(29, 34, true, false)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(0, 15), pro::interval<double>(34, 39, true, false)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(0, 15), pro::interval<double>(39, 43, true, false)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(0, 15), pro::interval<double>(43, 46, true, false)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(0, 15), pro::interval<double>(46, DBL_MAX, true, false)));
}

VLevelPenaltyTable::VLevelPenaltyTable()
{
	// >200
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(201, 250), pro::interval<double>(7, 9, true, false)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(201, 250), pro::interval<double>(9, 11, true, false)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(201, 250), pro::interval<double>(11, 12, true, false)));
	this->table.push_back(VPenaltyRecord(4, 0, pro::interval<int>(201, 250), pro::interval<double>(12, 13, true, false)));
	this->table.push_back(VPenaltyRecord(6, 0, pro::interval<int>(201, 250), pro::interval<double>(13, 14, true, false)));
	this->table.push_back(VPenaltyRecord(18, 0, pro::interval<int>(201, 250), pro::interval<double>(14, 15, true, false)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(201, 250), pro::interval<double>(15, 16, true, false)));
	this->table.push_back(VPenaltyRecord(50, 0, pro::interval<int>(201, 250), pro::interval<double>(16, 17, true, false)));
	this->table.push_back(VPenaltyRecord(100, 0, pro::interval<int>(201, 250), pro::interval<double>(17, DBL_MAX, true, false)));
	// >140
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(141, 200), pro::interval<double>(7, 9, true, false)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(141, 200), pro::interval<double>(9, 11, true, false)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(141, 200), pro::interval<double>(11, 13, true, false)));
	this->table.push_back(VPenaltyRecord(4, 0, pro::interval<int>(141, 200), pro::interval<double>(13, 15, true, false)));
	this->table.push_back(VPenaltyRecord(6, 0, pro::interval<int>(141, 200), pro::interval<double>(15, 16, true, false)));
	this->table.push_back(VPenaltyRecord(18, 0, pro::interval<int>(141, 200), pro::interval<double>(16, 17, true, false)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(141, 200), pro::interval<double>(17, 18, true, false)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(141, 200), pro::interval<double>(18, 19, true, false)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(141, 200), pro::interval<double>(19, 20, true, false)));
	this->table.push_back(VPenaltyRecord(50, 0, pro::interval<int>(141, 200), pro::interval<double>(20, 21, true, false)));
	this->table.push_back(VPenaltyRecord(100, 0, pro::interval<int>(141, 200), pro::interval<double>(21, DBL_MAX, true, false)));
	// >120
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(121, 140), pro::interval<double>(11, 13, true, false)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(121, 140), pro::interval<double>(13, 14, true, false)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(121, 140), pro::interval<double>(14, 15, true, false)));
	this->table.push_back(VPenaltyRecord(4, 0, pro::interval<int>(121, 140), pro::interval<double>(15, 16, true, false)));
	this->table.push_back(VPenaltyRecord(6, 0, pro::interval<int>(121, 140), pro::interval<double>(16, 17, true, false)));
	this->table.push_back(VPenaltyRecord(18, 0, pro::interval<int>(121, 140), pro::interval<double>(17, 18, true, false)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(121, 140), pro::interval<double>(18, 19, true, false)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(121, 140), pro::interval<double>(19, 20, true, false)));
	this->table.push_back(VPenaltyRecord(50, 0, pro::interval<int>(121, 140), pro::interval<double>(20, 21, true, false)));
	this->table.push_back(VPenaltyRecord(100, 0, pro::interval<int>(121, 140), pro::interval<double>(21, DBL_MAX, true, false)));
	// >60
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(61, 120), pro::interval<double>(12, 14, true, false)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(61, 120), pro::interval<double>(14, 16, true, false)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(61, 120), pro::interval<double>(16, 18, true, false)));
	this->table.push_back(VPenaltyRecord(4, 0, pro::interval<int>(61, 120), pro::interval<double>(18, 20, true, false)));
	this->table.push_back(VPenaltyRecord(6, 0, pro::interval<int>(61, 120), pro::interval<double>(20, 21, true, false)));
	this->table.push_back(VPenaltyRecord(18, 0, pro::interval<int>(61, 120), pro::interval<double>(21, 22, true, false)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(61, 120), pro::interval<double>(22, 23, true, false)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(61, 120), pro::interval<double>(23, 24, true, false)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(61, 120), pro::interval<double>(24, 25, true, false)));
	this->table.push_back(VPenaltyRecord(50, 0, pro::interval<int>(61, 120), pro::interval<double>(25, 26, true, false)));
	this->table.push_back(VPenaltyRecord(100, 0, pro::interval<int>(61, 120), pro::interval<double>(26, DBL_MAX, true, false)));
	// >40
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(41, 60), pro::interval<double>(16, 19, true, false)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(41, 60), pro::interval<double>(19, 21, true, false)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(41, 60), pro::interval<double>(21, 23, true, false)));
	this->table.push_back(VPenaltyRecord(4, 0, pro::interval<int>(41, 60), pro::interval<double>(23, 25, true, false)));
	this->table.push_back(VPenaltyRecord(6, 0, pro::interval<int>(41, 60), pro::interval<double>(25, 26, true, false)));
	this->table.push_back(VPenaltyRecord(18, 0, pro::interval<int>(41, 60), pro::interval<double>(26, 27, true, false)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(41, 60), pro::interval<double>(27, 28, true, false)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(41, 60), pro::interval<double>(28, 29, true, false)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(41, 60), pro::interval<double>(29, 30, true, false)));
	this->table.push_back(VPenaltyRecord(50, 0, pro::interval<int>(41, 60), pro::interval<double>(30, 31, true, false)));
	this->table.push_back(VPenaltyRecord(100, 0, pro::interval<int>(41, 60), pro::interval<double>(31, DBL_MAX, true, false)));
	// >15
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(16, 40), pro::interval<double>(16, 19, true, false)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(16, 40), pro::interval<double>(19, 21, true, false)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(16, 40), pro::interval<double>(21, 23, true, false)));
	this->table.push_back(VPenaltyRecord(4, 0, pro::interval<int>(16, 40), pro::interval<double>(23, 25, true, false)));
	this->table.push_back(VPenaltyRecord(6, 0, pro::interval<int>(16, 40), pro::interval<double>(25, 26, true, false)));
	this->table.push_back(VPenaltyRecord(18, 0, pro::interval<int>(16, 40), pro::interval<double>(26, 28, true, false)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(16, 40), pro::interval<double>(28, 30, true, false)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(16, 40), pro::interval<double>(30, 32, true, false)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(16, 40), pro::interval<double>(32, 34, true, false)));
	this->table.push_back(VPenaltyRecord(50, 0, pro::interval<int>(16, 40), pro::interval<double>(34, 36, true, false)));
	this->table.push_back(VPenaltyRecord(100, 0, pro::interval<int>(16, 40), pro::interval<double>(36, DBL_MAX, true, false)));
	// >0
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(0, 15), pro::interval<double>(16, 19, true, false)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(0, 15), pro::interval<double>(19, 21, true, false)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(0, 15), pro::interval<double>(21, 23, true, false)));
	this->table.push_back(VPenaltyRecord(4, 0, pro::interval<int>(0, 15), pro::interval<double>(23, 25, true, false)));
	this->table.push_back(VPenaltyRecord(6, 0, pro::interval<int>(0, 15), pro::interval<double>(25, 26, true, false)));
	this->table.push_back(VPenaltyRecord(18, 0, pro::interval<int>(0, 15), pro::interval<double>(26, 31, true, false)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(0, 15), pro::interval<double>(31, 36, true, false)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(0, 15), pro::interval<double>(36, 41, true, false)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(0, 15), pro::interval<double>(41, 46, true, false)));
	this->table.push_back(VPenaltyRecord(50, 0, pro::interval<int>(0, 15), pro::interval<double>(46, 51, true, false)));
	this->table.push_back(VPenaltyRecord(100, 0, pro::interval<int>(0, 15), pro::interval<double>(51, DBL_MAX, true, false)));
}

VRichPenaltyTable::VRichPenaltyTable()
{
	//>200, L<=20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(201, 250), pro::interval<double>(10, 11, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(201, 250), pro::interval<double>(11, 12, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(201, 250), pro::interval<double>(12, 13, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(201, 250), pro::interval<double>(13, 14, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(201, 250), pro::interval<double>(14, 15, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(201, 250), pro::interval<double>(15, 16, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(201, 250), pro::interval<double>(16, 17, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(201, 250), pro::interval<double>(17, 18, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(201, 250), pro::interval<double>(18, 19, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(201, 250), pro::interval<double>(19, DBL_MAX, true, false), pro::interval<int>(0, 2)));
	//>200, L>20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(201, 250), pro::interval<double>(13, 15, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(201, 250), pro::interval<double>(15, 16, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(201, 250), pro::interval<double>(16, 17, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(201, 250), pro::interval<double>(17, 18, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(201, 250), pro::interval<double>(18, 19, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(201, 250), pro::interval<double>(19, 21, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(201, 250), pro::interval<double>(21, 23, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(201, 250), pro::interval<double>(23, 24, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(201, 250), pro::interval<double>(24, 25, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(201, 250), pro::interval<double>(25, 26, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(201, 250), pro::interval<double>(26, DBL_MAX, true, false), pro::interval<int>(3, 3)));
	//>200, L>40
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(201, 250), pro::interval<double>(16, 17, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(201, 250), pro::interval<double>(17, 18, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(201, 250), pro::interval<double>(18, 19, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(201, 250), pro::interval<double>(19, 20, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(201, 250), pro::interval<double>(20, 21, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(201, 250), pro::interval<double>(21, 23, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(201, 250), pro::interval<double>(23, 25, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(201, 250), pro::interval<double>(25, 26, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(201, 250), pro::interval<double>(26, 27, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(201, 250), pro::interval<double>(27, 28, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(201, 250), pro::interval<double>(28, DBL_MAX, true, false), pro::interval<int>(4, INT_MAX)));

	//>180, L<=20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(181, 200), pro::interval<double>(11, 12, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(181, 200), pro::interval<double>(12, 13, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(181, 200), pro::interval<double>(13, 14, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(181, 200), pro::interval<double>(14, 15, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(181, 200), pro::interval<double>(15, 16, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(181, 200), pro::interval<double>(16, 17, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(181, 200), pro::interval<double>(17, 18, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(181, 200), pro::interval<double>(18, 19, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(181, 200), pro::interval<double>(19, 20, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(181, 200), pro::interval<double>(20, 21, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(181, 200), pro::interval<double>(21, DBL_MAX, true, false), pro::interval<int>(0, 2)));
	//>180, L>20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(181, 200), pro::interval<double>(16, 17, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(181, 200), pro::interval<double>(17, 18, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(181, 200), pro::interval<double>(18, 19, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(181, 200), pro::interval<double>(19, 21, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(181, 200), pro::interval<double>(21, 23, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(181, 200), pro::interval<double>(23, 25, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(181, 200), pro::interval<double>(25, 27, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(181, 200), pro::interval<double>(27, 28, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(181, 200), pro::interval<double>(28, DBL_MAX, true, false), pro::interval<int>(3, 3)));
	//>180, L>40
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(181, 200), pro::interval<double>(19, 20, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(181, 200), pro::interval<double>(20, 21, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(181, 200), pro::interval<double>(21, 23, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(181, 200), pro::interval<double>(23, 25, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(181, 200), pro::interval<double>(25, 27, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(181, 200), pro::interval<double>(27, 29, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(181, 200), pro::interval<double>(29, 31, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(181, 200), pro::interval<double>(31, DBL_MAX, true, false), pro::interval<int>(4, INT_MAX)));

	//>160, L<=20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(161, 180), pro::interval<double>(11, 12, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(161, 180), pro::interval<double>(12, 13, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(161, 180), pro::interval<double>(13, 14, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(161, 180), pro::interval<double>(14, 15, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(161, 180), pro::interval<double>(15, 16, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(161, 180), pro::interval<double>(16, 18, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(161, 180), pro::interval<double>(18, 20, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(161, 180), pro::interval<double>(20, 22, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(161, 180), pro::interval<double>(22, 24, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(161, 180), pro::interval<double>(24, 26, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(161, 180), pro::interval<double>(26, DBL_MAX, true, false), pro::interval<int>(0, 2)));
	//>160, L>20
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(161, 180), pro::interval<double>(19, 20, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(161, 180), pro::interval<double>(20, 21, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(161, 180), pro::interval<double>(21, 23, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(161, 180), pro::interval<double>(23, 25, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(161, 180), pro::interval<double>(25, 27, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(161, 180), pro::interval<double>(27, 29, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(161, 180), pro::interval<double>(29, 31, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(161, 180), pro::interval<double>(31, DBL_MAX, true, false), pro::interval<int>(3, 3)));
	//>160, L>40
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(161, 180), pro::interval<double>(23, 24, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(161, 180), pro::interval<double>(24, 25, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(161, 180), pro::interval<double>(25, 26, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(161, 180), pro::interval<double>(26, 28, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(161, 180), pro::interval<double>(28, 30, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(161, 180), pro::interval<double>(30, 32, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(161, 180), pro::interval<double>(32, 34, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(161, 180), pro::interval<double>(34, 36, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(161, 180), pro::interval<double>(36, DBL_MAX, true, false), pro::interval<int>(4, INT_MAX)));

	//>140, L<=20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(141, 160), pro::interval<double>(11, 12, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(141, 160), pro::interval<double>(12, 13, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(141, 160), pro::interval<double>(13, 14, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(141, 160), pro::interval<double>(14, 15, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(141, 160), pro::interval<double>(15, 16, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(141, 160), pro::interval<double>(16, 18, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(141, 160), pro::interval<double>(18, 20, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(141, 160), pro::interval<double>(20, 22, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(141, 160), pro::interval<double>(22, 24, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(141, 160), pro::interval<double>(24, 26, true, false), pro::interval<int>(0, 2)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(141, 160), pro::interval<double>(26, DBL_MAX, true, false), pro::interval<int>(0, 2)));
	//>140, L>20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(141, 160), pro::interval<double>(21, 22, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(141, 160), pro::interval<double>(22, 23, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(141, 160), pro::interval<double>(23, 24, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(141, 160), pro::interval<double>(24, 25, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(141, 160), pro::interval<double>(25, 26, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(141, 160), pro::interval<double>(26, 28, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(141, 160), pro::interval<double>(28, 30, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(141, 160), pro::interval<double>(30, 32, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(141, 160), pro::interval<double>(32, 34, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(141, 160), pro::interval<double>(34, 36, true, false), pro::interval<int>(3, 3)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(141, 160), pro::interval<double>(36, DBL_MAX, true, false), pro::interval<int>(3, 3)));
	//>140, L>40
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(141, 160), pro::interval<double>(16, 17, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(141, 160), pro::interval<double>(17, 18, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(141, 160), pro::interval<double>(18, 19, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(141, 160), pro::interval<double>(19, 20, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(141, 160), pro::interval<double>(20, 21, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(141, 160), pro::interval<double>(21, 23, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(141, 160), pro::interval<double>(23, 25, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(141, 160), pro::interval<double>(25, 27, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(141, 160), pro::interval<double>(27, 29, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(141, 160), pro::interval<double>(29, 31, true, false), pro::interval<int>(4, INT_MAX)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(141, 160), pro::interval<double>(31, DBL_MAX, true, false), pro::interval<int>(4, INT_MAX)));


	//>120, L<=10, R<850
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(121, 140), pro::interval<double>(11, 12, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(121, 140), pro::interval<double>(12, 13, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(121, 140), pro::interval<double>(13, 14, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(121, 140), pro::interval<double>(14, 15, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(121, 140), pro::interval<double>(15, 16, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(121, 140), pro::interval<double>(16, 17, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(121, 140), pro::interval<double>(17, 18, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(121, 140), pro::interval<double>(18, 19, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(121, 140), pro::interval<double>(19, 20, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(121, 140), pro::interval<double>(20, 21, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(121, 140), pro::interval<double>(21, DBL_MAX, true, false), pro::interval<int>(0, 0)));
	//>120, L<=20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(121, 140), pro::interval<double>(11, 12, true, false), pro::interval<int>(1, 2)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(121, 140), pro::interval<double>(12, 13, true, false), pro::interval<int>(1, 2)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(121, 140), pro::interval<double>(13, 14, true, false), pro::interval<int>(1, 2)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(121, 140), pro::interval<double>(14, 15, true, false), pro::interval<int>(1, 2)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(121, 140), pro::interval<double>(15, 16, true, false), pro::interval<int>(1, 2)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(121, 140), pro::interval<double>(16, 18, true, false), pro::interval<int>(1, 2)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(121, 140), pro::interval<double>(18, 20, true, false), pro::interval<int>(1, 2)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(121, 140), pro::interval<double>(20, 22, true, false), pro::interval<int>(1, 2)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(121, 140), pro::interval<double>(22, 24, true, false), pro::interval<int>(1, 2)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(121, 140), pro::interval<double>(24, 26, true, false), pro::interval<int>(1, 2)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(121, 140), pro::interval<double>(26, DBL_MAX, true, false), pro::interval<int>(1, 2)));
	//>120, L>20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(121, 140), pro::interval<double>(21, 22, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(121, 140), pro::interval<double>(22, 23, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(121, 140), pro::interval<double>(23, 24, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(121, 140), pro::interval<double>(24, 25, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(121, 140), pro::interval<double>(25, 26, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(121, 140), pro::interval<double>(26, 28, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(121, 140), pro::interval<double>(28, 30, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(121, 140), pro::interval<double>(30, 32, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(121, 140), pro::interval<double>(32, 34, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(121, 140), pro::interval<double>(34, 36, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(121, 140), pro::interval<double>(36, DBL_MAX, true, false), pro::interval<int>(3, INT_MAX)));

	//>60, L<=10, R<850
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(61, 120), pro::interval<double>(21, DBL_MAX, true, false), pro::interval<int>(0, 0)));
	//>60, L<=20 & L>10, R<850
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(61, 120), pro::interval<double>(21, 22, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(61, 120), pro::interval<double>(22, 23, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(61, 120), pro::interval<double>(23, 24, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(61, 120), pro::interval<double>(24, 25, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(61, 120), pro::interval<double>(25, 26, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(61, 120), pro::interval<double>(26, DBL_MAX, true, false), pro::interval<int>(1, 1)));
	//>60, L<=20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(61, 120), pro::interval<double>(21, 22, true, false), pro::interval<int>(2, 2)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(61, 120), pro::interval<double>(22, 23, true, false), pro::interval<int>(2, 2)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(61, 120), pro::interval<double>(23, 24, true, false), pro::interval<int>(2, 2)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(61, 120), pro::interval<double>(24, 25, true, false), pro::interval<int>(2, 2)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(61, 120), pro::interval<double>(25, 26, true, false), pro::interval<int>(2, 2)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(61, 120), pro::interval<double>(26, 28, true, false), pro::interval<int>(2, 2)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(61, 120), pro::interval<double>(28, 30, true, false), pro::interval<int>(2, 2)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(61, 120), pro::interval<double>(30, 32, true, false), pro::interval<int>(2, 2)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(61, 120), pro::interval<double>(32, 34, true, false), pro::interval<int>(2, 2)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(61, 120), pro::interval<double>(34, 36, true, false), pro::interval<int>(2, 2)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(61, 120), pro::interval<double>(36, DBL_MAX, true, false), pro::interval<int>(2, 2)));
	//>60, L>20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(61, 120), pro::interval<double>(26, 28, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(61, 120), pro::interval<double>(28, 30, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(61, 120), pro::interval<double>(30, 32, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(61, 120), pro::interval<double>(32, 34, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(61, 120), pro::interval<double>(34, 36, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(61, 120), pro::interval<double>(36, 37, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(61, 120), pro::interval<double>(37, 38, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(61, 120), pro::interval<double>(38, 39, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(61, 120), pro::interval<double>(39, 40, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(61, 120), pro::interval<double>(40, 41, true, false), pro::interval<int>(3, INT_MAX)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(61, 120), pro::interval<double>(41, DBL_MAX, true, false), pro::interval<int>(3, INT_MAX)));

	//>40, L<=20, R<850
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(41, 60), pro::interval<double>(26, 28, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(41, 60), pro::interval<double>(28, 30, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(41, 60), pro::interval<double>(30, 32, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(41, 60), pro::interval<double>(32, 34, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(41, 60), pro::interval<double>(34, 36, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(41, 60), pro::interval<double>(36, DBL_MAX, true, false), pro::interval<int>(0, 0)));
	//>40, L<=20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(41, 60), pro::interval<double>(26, 28, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(41, 60), pro::interval<double>(28, 30, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(41, 60), pro::interval<double>(30, 32, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(41, 60), pro::interval<double>(32, 34, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(41, 60), pro::interval<double>(34, 36, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(41, 60), pro::interval<double>(36, 37, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(41, 60), pro::interval<double>(37, 38, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(41, 60), pro::interval<double>(38, 39, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(41, 60), pro::interval<double>(39, 40, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(41, 60), pro::interval<double>(40, 41, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(41, 60), pro::interval<double>(41, DBL_MAX, true, false), pro::interval<int>(1, 1)));
	//>40, L>20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(41, 60), pro::interval<double>(31, 33, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(41, 60), pro::interval<double>(33, 35, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(41, 60), pro::interval<double>(35, 37, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(41, 60), pro::interval<double>(37, 39, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(41, 60), pro::interval<double>(39, 41, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(41, 60), pro::interval<double>(41, 43, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(41, 60), pro::interval<double>(43, 45, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(41, 60), pro::interval<double>(45, 47, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(41, 60), pro::interval<double>(47, 49, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(41, 60), pro::interval<double>(49, 51, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(41, 60), pro::interval<double>(51, DBL_MAX, true, false), pro::interval<int>(2, INT_MAX)));

	//>15, L<=20, R<850
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(16, 40), pro::interval<double>(26, 28, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(16, 40), pro::interval<double>(28, 30, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(16, 40), pro::interval<double>(30, 32, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(16, 40), pro::interval<double>(32, 34, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(16, 40), pro::interval<double>(34, 36, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(16, 40), pro::interval<double>(36, 37, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(16, 40), pro::interval<double>(37, 38, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(16, 40), pro::interval<double>(38, 39, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(16, 40), pro::interval<double>(39, 40, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(16, 40), pro::interval<double>(40, 41, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(16, 40), pro::interval<double>(41, DBL_MAX, true, false), pro::interval<int>(0, 0)));
	//>15, L<=20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(16, 40), pro::interval<double>(26, 28, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(16, 40), pro::interval<double>(28, 30, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(16, 40), pro::interval<double>(30, 32, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(16, 40), pro::interval<double>(32, 34, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(16, 40), pro::interval<double>(34, 36, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(16, 40), pro::interval<double>(36, 39, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(16, 40), pro::interval<double>(39, 42, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(16, 40), pro::interval<double>(42, 45, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(16, 40), pro::interval<double>(45, 48, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(16, 40), pro::interval<double>(48, 51, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(16, 40), pro::interval<double>(51, DBL_MAX, true, false), pro::interval<int>(1, 1)));
	//>15, L>20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(16, 40), pro::interval<double>(31, 33, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(16, 40), pro::interval<double>(33, 35, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(16, 40), pro::interval<double>(35, 37, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(16, 40), pro::interval<double>(37, 39, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(16, 40), pro::interval<double>(39, 41, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(16, 40), pro::interval<double>(41, 46, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(16, 40), pro::interval<double>(46, 51, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(16, 40), pro::interval<double>(51, 55, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(16, 40), pro::interval<double>(55, 61, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(16, 40), pro::interval<double>(61, 66, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(16, 40), pro::interval<double>(66, DBL_MAX, true, false), pro::interval<int>(2, INT_MAX)));

	//>0, L<=20, R<=850
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(0, 15), pro::interval<double>(26, 28, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(0, 15), pro::interval<double>(28, 30, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(0, 15), pro::interval<double>(30, 32, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(0, 15), pro::interval<double>(32, 34, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(0, 15), pro::interval<double>(34, 36, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(0, 15), pro::interval<double>(36, 38, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(0, 15), pro::interval<double>(38, 40, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(0, 15), pro::interval<double>(40, 42, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(0, 15), pro::interval<double>(42, 44, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(0, 15), pro::interval<double>(44, 46, true, false), pro::interval<int>(0, 0)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(0, 15), pro::interval<double>(46, DBL_MAX, true, false), pro::interval<int>(0, 0)));
	//>0, L<=20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(0, 15), pro::interval<double>(26, 28, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(0, 15), pro::interval<double>(28, 30, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(0, 15), pro::interval<double>(30, 32, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(0, 15), pro::interval<double>(32, 34, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(0, 15), pro::interval<double>(34, 36, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(0, 15), pro::interval<double>(36, 42, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(0, 15), pro::interval<double>(42, 48, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(0, 15), pro::interval<double>(48, 54, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(0, 15), pro::interval<double>(54, 60, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(0, 15), pro::interval<double>(60, 66, true, false), pro::interval<int>(1, 1)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(0, 15), pro::interval<double>(66, DBL_MAX, true, false), pro::interval<int>(1, 1)));
	//>0, L>20
	this->table.push_back(VPenaltyRecord(1, 0, pro::interval<int>(0, 15), pro::interval<double>(31, 33, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(2, 0, pro::interval<int>(0, 15), pro::interval<double>(33, 35, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(3, 0, pro::interval<int>(0, 15), pro::interval<double>(35, 37, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(5, 0, pro::interval<int>(0, 15), pro::interval<double>(37, 39, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(7, 0, pro::interval<int>(0, 15), pro::interval<double>(39, 41, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(20, 0, pro::interval<int>(0, 15), pro::interval<double>(41, 51, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(24, 0, pro::interval<int>(0, 15), pro::interval<double>(51, 61, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(32, 0, pro::interval<int>(0, 15), pro::interval<double>(61, 71, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(48, 0, pro::interval<int>(0, 15), pro::interval<double>(71, 81, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(70, 0, pro::interval<int>(0, 15), pro::interval<double>(81, 91, true, false), pro::interval<int>(2, INT_MAX)));
	this->table.push_back(VPenaltyRecord(150, 0, pro::interval<int>(0, 15), pro::interval<double>(91, DBL_MAX, true, false), pro::interval<int>(2, INT_MAX)));
}


//----------------------------------------- Методы класса VDetectTables --------------------------------------


/** Конструктор*/
VDetectTables::VDetectTables()
{	
	
	widen_penalty_table = VPenaltyTablePtr(new VWidenPenaltyTable());
	narrow_penalty_table = VPenaltyTablePtr(new VNarrowPenaltyTable());
	distortion_penalty_table = VPenaltyTablePtr(new VDistortionPenaltyTable());
	pit_penalty_table = VPenaltyTablePtr(new VPitPenaltyTable());	
	level_penalty_table = VPenaltyTablePtr(new VLevelPenaltyTable());	
	rich_penalty_table = VPenaltyTablePtr(new VRichPenaltyTable());		
}

// Методы возвращающие нормативы
const VWidenTable& VDetectTables::GetWidenTable() const
{
	return widen_table;
}

/// Возвращает нормативы сужений
const VNarrowTable& VDetectTables::GetNarrowTable() const
{
	return narrow_table;
}

/// Возвращает нормативы просадок
const VPitTable& VDetectTables::GetPitTable() const
{
	return pit_table;
}

/// Возвращает нормативы перекосов
const VDistortionTable& VDetectTables::GetDistortionTable() const
{
	return distortion_table;
}

/// Возвращает нормы уровня
const VLevelTable& VDetectTables::GetLevelTable() const
{
	return level_table;
}

/// Возвращает нормативы рихтовок
const VRichTable& VDetectTables::GetRichTable() const
{
	return rich_table;
}

/// Бальная оценка уширений
const VPenaltyTable& VDetectTables::GetWidenPenaltyTable() const
{
	return *widen_penalty_table;
}

/// Бальная оценка сужений
const VPenaltyTable& VDetectTables::GetNarrowPenaltyTable() const
{
	return *narrow_penalty_table;
}

/// Бальная оценка перекосов
const VPenaltyTable& VDetectTables::GetDistortionPenaltyTable() const
{
	return *distortion_penalty_table;
}

/// Бальная оценка просадок
const VPenaltyTable& VDetectTables::GetPitPenaltyTable() const
{
	return *pit_penalty_table;
}

/// Бальная оценка уровня
const VPenaltyTable& VDetectTables::GetLevelPenaltyTable() const
{
	return *level_penalty_table;
}

/// Бальная оценка рихтовок
const VPenaltyTable& VDetectTables::GetRichPenaltyTable() const
{
	return *rich_penalty_table;
}

