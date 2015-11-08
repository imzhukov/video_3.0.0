#include "fftw3.h"

#include "oversteps.h"

//-------------------------------------------- Методы класса VRichPalet -----------------------------------------------------

/// Конструктор
VRichPalet::VRichPalet()
{	
	pro::piecewise::linear LINE_MAX;
	LINE_MAX.add(pro::piecewise::section(RICH_LENGTH_RANGE, line_t(DBL_MAX)));
	LINE_MAX.add(pro::piecewise::section(LONG_RICH_LENGTH_RANGE, line_t(DBL_MAX)));
			
	pro::interval<double> RANGE_0_5(0,10,false,true);
	pro::interval<double> RANGE_5_10(10,20,false,true);
	pro::interval<double> RANGE_10_20(20,40,false,true);
	
	//Для скорости 121-140
	pro::piecewise::linear LINE_140_II;
	LINE_140_II.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 7.8*2), point_t(10.0, 7.8*2))));
	LINE_140_II.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 7.8*2), point_t(20.0, 5.9*2))));
	LINE_140_II.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 5.9*2), point_t(40.0, 7.8*2))));
	
	pro::piecewise::linear LINE_140_III;
	LINE_140_III.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 10.7*2), point_t(10.0, 10.7*2))));
	LINE_140_III.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 10.7*2), point_t(20.0, 6.7*2))));
	LINE_140_III.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 6.7*2), point_t(40.0, 10.5*2))));
	
	pro::piecewise::linear LINE_140_IV;
	LINE_140_IV.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 14.5*2), point_t(10.0, 14.5*2))));
	LINE_140_IV.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 14.5*2), point_t(20.0, 9.1*2))));
	LINE_140_IV.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 9.1*2), point_t(40.0, 14.7*2))));
	
	//Для скорости 61-120
	pro::piecewise::linear LINE_120_II;
	LINE_120_II.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 7.8*2), point_t(10.0, 7.8*2))));
	LINE_120_II.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 7.8*2), point_t(20.0, 5.9*2))));
	LINE_120_II.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 5.8*2), point_t(40.0, 7.8*2))));
	
	pro::piecewise::linear LINE_120_III;
	LINE_120_III.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 14.5*2), point_t(10.0, 14.5*2))));
	LINE_120_III.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 14.5*2), point_t(20.0, 9.1*2))));
	LINE_120_III.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 9.1*2), point_t(40.0, 14.7*2))));
	
	pro::piecewise::linear LINE_120_IV;
	LINE_120_IV.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 20.3*2), point_t(10.0, 20.3*2))));
	LINE_120_IV.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 20.3*2), point_t(20.0, 12.7*2))));
	LINE_120_IV.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 12.7*2), point_t(40.0, 16.8*2))));
	
	//Для скорости 41-60
	pro::piecewise::linear LINE_60_II;
	LINE_60_II.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 10.7*2), point_t(10.0, 10.7*2))));
	LINE_60_II.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 10.7*2), point_t(20.0, 6.8*2))));
	LINE_60_II.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 6.8*2), point_t(40.0, 10.5*2))));
	
	pro::piecewise::linear LINE_60_III;
	LINE_60_III.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 20.3*2), point_t(10.0, 20.3*2))));
	LINE_60_III.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 20.3*2), point_t(20.0, 12.7*2))));
	LINE_60_III.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 12.7*2), point_t(40.0, 16.8*2))));
	
	pro::piecewise::linear LINE_60_IV;
	LINE_60_IV.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 23.2*2), point_t(10.0, 23.2*2))));
	LINE_60_IV.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 23.2*2), point_t(20.0, 14.5*2))));
	LINE_60_IV.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 14.5*2), point_t(40.0, 21.0*2))));
	
	//Для скорости 16-40
	pro::piecewise::linear LINE_40_II;
	LINE_40_II.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 13.6*2), point_t(10.0, 13.6*2))));
	LINE_40_II.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 13.6*2), point_t(20.0, 7.8*2))));
	LINE_40_II.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 7.8*2), point_t(40.0, 12.6*2))));
	
	pro::piecewise::linear LINE_40_III;
	LINE_40_III.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 23.2*2), point_t(10.0, 23.2*2))));
	LINE_40_III.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 23.2*2), point_t(20.0, 14.5*2))));
	LINE_40_III.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 14.5*2), point_t(40.0, 21.0*2))));
	
	pro::piecewise::linear LINE_40_IV;
	LINE_40_IV.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 29.0*2), point_t(10.0, 29.0*2))));
	LINE_40_IV.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 29.0*2), point_t(20.0, 18.2*2))));
	LINE_40_IV.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 18.2*2), point_t(40.0, 27.1*2))));
	
	//Для скорости 15
	pro::piecewise::linear LINE_15_II;
	LINE_15_II.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 14.5*2), point_t(10.0, 14.5*2))));
	LINE_15_II.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 14.5*2), point_t(20.0, 9.1*2))));
	LINE_15_II.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 9.1*2), point_t(40.0, 14.7*2))));
	
	pro::piecewise::linear LINE_15_III;
	LINE_15_III.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 29.0*2), point_t(10.0, 29.0*2))));
	LINE_15_III.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 29.0*2), point_t(20.0, 18.2*2))));
	LINE_15_III.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 18.2*2), point_t(40.0, 27.1*2))));
	
	pro::piecewise::linear LINE_15_IV;
	LINE_15_IV.add(pro::piecewise::section(RANGE_0_5, line_t(point_t(0, 37.7*2), point_t(10.0, 37.7*2))));
	LINE_15_IV.add(pro::piecewise::section(RANGE_5_10, line_t(point_t(10.0, 37.7*2), point_t(20.0, 23.5*2))));
	LINE_15_IV.add(pro::piecewise::section(RANGE_10_20, line_t(point_t(20.0, 23.5*2), point_t(40.0, 37.6*2))));
	
	
	table.push_back(VRichPaletRecord(DEGREE2, pro::interval<int>(121,140), LINE_140_II, LINE_140_III));
	table.push_back(VRichPaletRecord(DEGREE3, pro::interval<int>(121,140), LINE_140_III, LINE_140_IV));
	table.push_back(VRichPaletRecord(DEGREE4, pro::interval<int>(121,140), LINE_140_IV, LINE_MAX));
	
	table.push_back(VRichPaletRecord(DEGREE2, pro::interval<int>(61,120), LINE_120_II, LINE_120_III));
	table.push_back(VRichPaletRecord(DEGREE3, pro::interval<int>(61,120), LINE_120_III, LINE_120_IV));
	table.push_back(VRichPaletRecord(DEGREE4, pro::interval<int>(61,120), LINE_120_IV, LINE_MAX));
	
	table.push_back(VRichPaletRecord(DEGREE2, pro::interval<int>(41,60), LINE_60_II, LINE_60_III));
	table.push_back(VRichPaletRecord(DEGREE3, pro::interval<int>(41,60), LINE_60_III, LINE_60_IV));
	table.push_back(VRichPaletRecord(DEGREE4, pro::interval<int>(41,60), LINE_60_IV, LINE_MAX));
	
	table.push_back(VRichPaletRecord(DEGREE2, pro::interval<int>(16,40), LINE_40_II, LINE_40_III));
	table.push_back(VRichPaletRecord(DEGREE3, pro::interval<int>(16,40), LINE_40_III, LINE_40_IV));
	table.push_back(VRichPaletRecord(DEGREE4, pro::interval<int>(16,40), LINE_40_IV, LINE_MAX));
	
	table.push_back(VRichPaletRecord(DEGREE2, pro::interval<int>(0,15), LINE_15_II, LINE_15_III));
	table.push_back(VRichPaletRecord(DEGREE3, pro::interval<int>(0,15), LINE_15_III, LINE_15_IV));
	table.push_back(VRichPaletRecord(DEGREE4, pro::interval<int>(0,15), LINE_15_IV, LINE_MAX));
};	

/// Возвращает диапазон
pro::interval<double> VRichPalet::GetRange(DEGREE in_degree, int in_speed, double in_length) const
{
	std::vector<VRichPaletRecord>::const_iterator i=std::find_if(table.begin(), table.end(), VRichPaletRecord::VFindKey(in_degree, in_speed));
	if (i!=table.end())		
		return i->GetRange(fabs(in_length));		
	else
		return EMPTY_INTERVAL<double>();
};

/// Функция коррекции
double VRichPalet::Correct(DEGREE in_degree, int in_speed, double in_value, double in_length) const
{
	pro::interval<double> r1=DETECT_TABLES.GetRichTable().GetRange(in_degree, in_speed, in_length);
	pro::interval<double> r2=GetRange(in_degree, in_speed, in_length);
	double correction=r1.left_bound().value/r2.left_bound().value;
	return round(correction*in_value);
};


//-------------------------------------------- Методы класса VRichCorrectionTable -----------------------------------

/// Конструктор
VRichCorrectionTable::VRichCorrectionTable()
{		
	function.add(pro::piecewise::section(point_t(0.0, 1.0/1.2), point_t(4.0, 1.0/1.2), true, false));
	function.add(pro::piecewise::section(point_t(4.0, 1.0/1.2), point_t(8.0, 1.0/1.2), true, false));
	function.add(pro::piecewise::section(point_t(8.0, 1.0/1.2), point_t(15.0, 1.0/0.92), true, false));
	function.add(pro::piecewise::section(point_t(15.0, 1.0/0.92), point_t(20.0, 1.0/0.74), true, false));
	function.add(pro::piecewise::section(point_t(20.0, 1.0/0.74), point_t(25.0, 1.0/0.67), true, false));
	function.add(pro::piecewise::section(point_t(25.0, 1.0/0.67), point_t(35.0, 1.0/0.68), true, false));
	function.add(pro::piecewise::section(point_t(35.0, 1.0/0.68), point_t(650000.0, 1.0/0.65), true, true));//FixMe!!! лучше использовать bound::max_value	
};

/// Функция коррекции
double VRichCorrectionTable::Correct(DEGREE in_degree, int in_speed, double in_value, double in_length) const
{
	if (function.value_in_domain(fabs(in_length)))
		return round(in_value*function.y(fabs(in_length)));
	else
		throw VSimpleException(L"Значение вне области определения функции", boost::str(boost::wformat(L"Длина = %f") % in_length), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
};

//------------------------------------ Методы класса VPitExtractor --------------------------------

/// Конструктор
VPitExtractor::VPitExtractor(side_t in_side) : side(in_side) {};

/// Оператор()
VPoint VPitExtractor::operator()(const VRtParameter& in_record) const
{
	return side==SD_LEFT ? VPoint(in_record.absCoord, in_record.pitLeft.value, in_record.pitLeft.corrupt) : 
		VPoint(in_record.absCoord, in_record.pitRight.value, in_record.pitRight.corrupt);
};

//------------------------------ Методы класса VWidthExtractor ------------------------------------

VPoint VWidthExtractor::operator()(const VRtParameter& in_record) const
{
	if (DETECT_OPTIONS.GetOption(L"WIDTH_SOURCE").Compare(OPTION_FRONT))
	{
		return VPoint(in_record.absCoord, in_record.widthFront.value, in_record.widthFront.corrupt);
	}
	else if (DETECT_OPTIONS.GetOption(L"WIDTH_SOURCE").Compare(OPTION_BACK))
	{		
		return VPoint(in_record.absCoord, in_record.widthBack.value, in_record.widthBack.corrupt);
	}
	else if (DETECT_OPTIONS.GetOption(L"WIDTH_SOURCE").Compare(OPTION_PPR))
	{
		return VPoint(in_record.absCoord, in_record.ppr.width.value,in_record.ppr.width.corrupt);		
	}
	else // стандарт
	{
		return VPoint(in_record.absCoord, in_record.width.value, in_record.width.corrupt);
	}
};

//------------------------------------ Методы класса VLevelExtractor --------------------------------

/// Оператор()
VPoint VLevelExtractor::operator()(const VRtParameter& in_record) const
{
	return VPoint(in_record.absCoord, in_record.level.value, in_record.level.corrupt);
}

//------------------------------------ Методы класса VCurvExtractor --------------------------------

/// Оператор()
VPoint VCurvExtractor::operator()(const VRtParameter& in_record) const
{
	return VPoint(in_record.absCoord, in_record.curv.value, in_record.curv.corrupt);
}


//------------------------------------ Методы класса VRichExtractor --------------------------------

/// Конструктор
VRichExtractor::VRichExtractor(side_t in_side) : side(in_side) {};

/// Оператор()
VPoint VRichExtractor::operator()(const VRtParameter& in_record) const
{
	return side==SD_LEFT ? VPoint(in_record.absCoord, in_record.richLeft.value, in_record.richLeft.corrupt) : 
		VPoint(in_record.absCoord, in_record.richRight.value, in_record.richRight.corrupt);
};

//----------------------------------- Методы класса VWearExtractor ---------------------------------------

/// Конструктор
VWearExtractor::VWearExtractor(side_t in_side) : side(in_side) {};

/// Конструктор
VWearExtractor::VWearExtractor() : side(SD_UNDEFINED) {};

/// operartor()
VPoint VWearExtractor::operator()(const VRtParameter& in_record) const
{
	if (DETECT_OPTIONS.GetOption(L"WEAR_SOURCE").Compare(OPTION_PPR))
	{
		switch (side)
		{
			case SD_LEFT:
				return  VPoint(in_record.absCoord, in_record.ppr.wearLeft.value, in_record.ppr.wearLeft.corrupt);
			case SD_RIGHT:
				return VPoint(in_record.absCoord, in_record.ppr.wearRight.value, in_record.ppr.wearRight.corrupt);
			case SD_UNDEFINED:
			default:
				return VPoint(in_record.absCoord, in_record.wear.value, in_record.wear.corrupt);
		}
	}
	else // стандарт
	{
		switch (side)
		{
			case SD_LEFT:
				return  VPoint(in_record.absCoord, in_record.wearLeft.value, in_record.wearLeft.corrupt);
			case SD_RIGHT:
				return VPoint(in_record.absCoord, in_record.wearRight.value, in_record.wearRight.corrupt);
			case SD_UNDEFINED:
			default:
				return VPoint(in_record.absCoord, in_record.wear.value, in_record.wear.corrupt);
		}
	}
};

//----------------------------------- Методы класса VZazorExtractor ---------------------------------------

/// Конструктор
VZazorExtractor::VZazorExtractor(side_t in_side) : side(in_side) {};

/// operartor()
VPoint VZazorExtractor::operator()(const VRtParameter& in_record) const
{	
	return side==SD_LEFT ? VPoint(in_record.absCoord, in_record.zazorLeft.value, in_record.zazorLeft.corrupt) : 
		VPoint(in_record.absCoord, in_record.zazorRight.value, in_record.zazorRight.corrupt);
};


//----------------------------------- Методы класса VTemperatureExtractor ---------------------------------------

/// Конструктор
VTemperatureExtractor::VTemperatureExtractor(side_t in_side) : side(in_side) {};

/// operartor()
VPoint VTemperatureExtractor::operator()(const VRtParameter& in_record) const
{	
	return side==SD_LEFT ? VPoint(in_record.absCoord, in_record.tempLeft.value, in_record.tempLeft.corrupt) : 
		VPoint(in_record.absCoord, in_record.tempRight.value, in_record.tempRight.corrupt);
};

//------------------------------------------ Методы класса VLevelPassportVariate ---------------------------

/// Конструктор
VLevelPassportVariate::VLevelPassportVariate(){};
/// Деструктор
VLevelPassportVariate::~VLevelPassportVariate() 
{
	if (!Empty()) 
		LOG_ERROR(L"VLevelPassportVariate: Необработанные данные...");
};
/// Ввод данных
void VLevelPassportVariate::Put(const VPoint& in_point)
{		
	double norma=SITUATION.GetLevel(in_point.GetCoord());
	output.push(VPoint(in_point.GetCoord(), in_point.GetValue()-norma, in_point.GetCorrupt()));
};	
/// Возвращает true, если есть обработанные данные
bool VLevelPassportVariate::Ready() const
{
	return output.size()>0;
};	
/// Возвращает true, если обработанных данных нет
bool VLevelPassportVariate::Empty() const
{
	return output.empty();
};		
/// Возвращает первый элемент данных
const VPoint& VLevelPassportVariate::Front() const
{
	return output.front();
};
/// Выталкивает первый элемент данных
void VLevelPassportVariate::Pop()
{
	output.pop();
};

//------------------------------------ Методы класса VRichPassportVariate -------------------------------------

/// Конструктор
VRichPassportVariate::VRichPassportVariate(side_t in_side)  : side(in_side){};
/// Деструктор
VRichPassportVariate::~VRichPassportVariate() 
{
	if (!Empty()) 
		LOG_ERROR(L"VRichPassportVariate: Необработанные данные...");
};
/// Ввод данных
void VRichPassportVariate::Put(const VPoint& in_point)
{		
	double norma=side==SD_LEFT ? SITUATION.GetRichLeft(in_point.GetCoord()) : SITUATION.GetRichRight(in_point.GetCoord());
	output.push(VPoint(in_point.GetCoord(), in_point.GetValue()-norma, in_point.GetCorrupt()));
};	
/// Возвращает true, если есть обработанные данные
bool VRichPassportVariate::Ready() const
{
	return output.size()>0;
};	
/// Возвращает true, если обработанных данных нет
bool VRichPassportVariate::Empty() const
{
	return output.empty();
};		
/// Возвращает первый элемент данных
const VPoint& VRichPassportVariate::Front() const
{
	return output.front();
};
/// Выталкивает первый элемент данных
void VRichPassportVariate::Pop()
{
	output.pop();
};

//-------------------------------- Методы класса VBasicOverstep ---------------------------------------------

/// Конструктор
VBasicOverstep::VBasicOverstep()
{
	start=0;
	end=0;
	max_value=0;
	min_value=0;
	value=0;
	type=L"?";
	degree=DEGREE_NONE;
	penalty=0;
	close_to_upper_bound=false;
	comment=L"";
	is_excluded=false;
	exclusion_reason=L"";
};

/// Конструктор
VBasicOverstep::VBasicOverstep(double in_start, double in_end, double in_max_value, double in_min_value, double in_value, 
	double in_length, const std::wstring& in_type, DEGREE in_degree)
{
	start=in_start;
	end=in_end;
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	max_value=in_max_value;
	min_value=in_min_value;
	value=in_value;
	length=in_length;
	type=in_type;
	degree=in_degree;
	close_to_upper_bound=false;
	comment=L"";
	is_excluded=false;
	exclusion_reason=L"";
}

/// Возвращает координату начала
double VBasicOverstep::GetStart() const
{
	return start;
};

/// Возвращает координату конца
double VBasicOverstep::GetEnd() const
{
	return end;
};

/// Возвращает максимальное значение
double VBasicOverstep::GetMax() const
{
	return max_value;
};

/// Возвращает минимальное значение
double VBasicOverstep::GetMin() const
{
	return min_value;
};	

/// Возвращает среднее значение
double VBasicOverstep::GetAverage() const
{
	return 0.0;
};	

/// Возвращает координату
double VBasicOverstep::GetCoord() const
{
	return (start+end)/2;
}

/// Возвращает координату
const VWayCoord& VBasicOverstep::GetWayCoord() const
{
	return waycoord;
}

/// Возвращает абсолютное значение
double VBasicOverstep::GetValue() const
{	
	return value;
};

/// Возвращает длину
double VBasicOverstep::GetLength() const
{
	return length;
};	

/// Возвращает степень
DEGREE VBasicOverstep::GetDegree() const
{
	return degree;
};

/// Возвращает признак близости к следующей степени
bool VBasicOverstep::CloseToUpperBound() const
{
	return close_to_upper_bound;
};

/// Устанавливает степень
void VBasicOverstep::SetDegree(DEGREE in_degree)
{
	degree=in_degree;
};

/// Вовращает описание
std::wstring VBasicOverstep::GetDescription() const
{		
	return boost::str(boost::wformat(L"%d км %0.0f м %s %s %s %s %s %s") % waycoord.km % waycoord.m  % GetTypeString() % GetDegreeString() 
		% GetValueString() % GetLengthString() % GetAmountString() % GetRestrictionString());
};	

/// Добавляет комментарий
void VBasicOverstep::AddComment(const std::wstring& in_comment)
{
	comment+=in_comment;
}

/// Добавляет комментарий
//const std::wstring& VBasicOverstep::GetComment() const
std::wstring VBasicOverstep::GetComment() const
{
	return comment;
}

///Возвращает true, если границы отступлений перекрываются
bool VBasicOverstep::Intersection(const VBasicOverstep& in_overstep) const
{
	return !(end<in_overstep.start || start>in_overstep.end);
};

///Возвращает расстояние между двумя отступлениями
double VBasicOverstep::Distance(const VBasicOverstep& in_overstep) const
{
	return std::min<double>(fabs(end-in_overstep.start), fabs(start-in_overstep.end));
};

/// Возвращает true, если отступление указанного типа
bool VBasicOverstep::IsType(const std::wstring& in_type) const
{
	return type.compare(in_type)==0;
};

/// Вернуть тип отступления
const std::wstring& VBasicOverstep::GetType() const
{
	return type;
}

/// Возвращает группу
const OVERSTEP_GROUP VBasicOverstep::GetGroup() const
{
	return GROUP_NONE;
}

/// Вернуть скорости
const VSpeeds& VBasicOverstep::GetSpeeds() const
{
	return speeds;
}

/// Вернуть ограничение скорости
const VSpeeds& VBasicOverstep::GetRestriction() const
{
	return restriction;
}

/// Установить ограничение скорости
void VBasicOverstep::SetRestriction(const VSpeeds& in_speeds)
{
	restriction=in_speeds;
}

/// Отменить закрытие скорости
void VBasicOverstep::CorrectClosure()
{
	if (HasRestriction() && GetRestriction().GetMaxSpeed()==0)
	{
		if (DETECT_OPTIONS.GetOption(L"LINE_CLOSURE_POLICY").Compare(OPTION_RESTRICT_25))
		{
			restriction=speeds;
			restriction.RestrictMaxSpeed(CLOSURE_CORRECTION_SPEED);
			AddComment(CLOSURE_CORRECTION_SIGN);
		}
	}
}

/// Возвращает true если скорость ограничена
bool VBasicOverstep::HasRestriction() const
{
	return !is_excluded && (speeds!=restriction);
}

/// Возвращает число элементарных отступлений
size_t VBasicOverstep::GetAmount() const
{
	return 1;
}

/// Возвращает балловую оценку
int VBasicOverstep::GetPenalty() const
{
	return 0;
}

/// Исключает отступление из оценки
void VBasicOverstep::Exclude(const std::wstring& in_reason)
{
	is_excluded=true;
	exclusion_reason=in_reason;
}

/// Возвращает true если отступление должно быть исколючено из оценки
bool VBasicOverstep::IsExcluded() const
{
	return is_excluded;
}

/// Возвращает причину исключения из оценки
const std::wstring& VBasicOverstep::GetExclusionReason() const
{
	return exclusion_reason;
}

/// Возвращаее строку с описанием координаты
std::wstring VBasicOverstep::GetCoordString() const
{	
	return boost::str(boost::wformat(L"%0.0f") % waycoord.m);
}

/// Возвращаее строку с описанием типа
std::wstring VBasicOverstep::GetTypeString() const
{
	return GetType();
}

/// Возвращает строку с описанием степени
std::wstring VBasicOverstep::GetDegreeString() const
{
	//Для исключенных отступлений не выводим степени
	return GetDegree() >0 && !IsExcluded() ? boost::str(boost::wformat(L"%d") % GetDegree()) : L"";
}

/// Возвращает строку с описанием значения
std::wstring VBasicOverstep::GetValueString() const
{
	return boost::str(boost::wformat(L"%1%") % GetValue());
}

/// Возвращает строку с описанием длины
std::wstring VBasicOverstep::GetLengthString() const
{
	return boost::str(boost::wformat(L"%1%") % GetLength());
}

/// Возвращает строку с описанием количества элементарных отступлений
std::wstring VBasicOverstep::GetAmountString() const
{
	return L"";
}

/// Возвращает строку с балловой оценкой
std::wstring VBasicOverstep::GetPenaltyString() const
{
	if (GetPenalty()!=0)
		return boost::str(boost::wformat(L"%d") % GetPenalty());
	else
		return L"";
}

/// Возвращает описание ограничений скорости
std::wstring VBasicOverstep::GetRestrictionString() const
{
	std::wstring result=L"";
	if (HasRestriction())
	{
		result=VSpeeds::Restriction(speeds, restriction).format_short();
	}
	return result;
}


//----------------------------- Методы класса VControlSectionOverstep ------------------------------------

/// Конструктор
VControlSectionOverstep::VControlSectionOverstep(double in_start, double in_end, double in_value_real, 
		double in_value_apr, const std::wstring& in_type)
{
	start=in_start;
	end=in_end;
	waycoord=SITUATION.GetRealCoord(GetCoord());
	max_value=0;
	min_value=0;
	value=round(in_value_real, CONTROL_SECTION_PRECISION);
	length=ceil(fabs(in_start-in_end));
	type=in_type;
	degree=DEGREE_NONE;
		
	value_real=round(in_value_real, CONTROL_SECTION_PRECISION);
	value_apr=in_value_apr;		
}

/// Возвращает координату
double VControlSectionOverstep::GetCoord() const
{
	return std::max<double>(start, end);
}

/// Возвращает строку с описанием типа
std::wstring VControlSectionOverstep::GetTypeString() const
{
	return this->GetType();
};

/// Возвращает строку с описанием значения
std::wstring VControlSectionOverstep::GetValueString() const
{
	return boost::str(boost::wformat(L"%1%") % value_real);
}

/// Возвращает строку с описанием длины
std::wstring VControlSectionOverstep::GetLengthString() const
{
	return L"";
}

/// Возвращает строку с описанием количества элементарных отступлений
std::wstring VControlSectionOverstep::GetAmountString() const
{
	return L"";
}

/// Возвращает описание ограничений скорости
std::wstring VControlSectionOverstep::GetRestrictionString() const	
{
	return L"";
}

/// Возвращает комментарий
std::wstring VControlSectionOverstep::GetComment() const
{
	return boost::str(boost::wformat(L"%1%") % value_apr);
}

//--------------------------------- Методы класса VWidenOverstep -----------------------------------------

/// Конструктор
VWidenOverstep::VWidenOverstep(const VExcess& in_excess, DEGREE in_degree)
{
	start=in_excess.GetStart();
	end=in_excess.GetEnd();
	waycoord=SITUATION.GetRealCoord(GetCoord());
	max_value=in_excess.GetMax();
	min_value=in_excess.GetMin();
	value=round(max_value, DEFAULT_PRECISION);
	length=ceil(fabs(start-end));
	type=WIDEN;
	degree=in_degree;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	radius=SITUATION.GetCurveRadius(GetCoord());
	on_switch=SITUATION.OnSwitch(pro::interval<double>(std::min<double>(start, end), std::max<double>(start, end)));
	on_chamfered_joint=SITUATION.GetChamJoint(start) || SITUATION.GetChamJoint(end);
	norma=SITUATION.GetWidth(GetCoord());	
	restriction=speeds;	
	//Ограничения 4-й степени
	if (GetDegree()==DEGREE4)
	{		
		while (restriction.GetMaxSpeed()>0 && VWidenCondition(DEGREE4, restriction, norma, radius, 0).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);				
		}
	}		
	//Для третьей степени проверяем близость к четвертой
	if (GetDegree()==DEGREE3)
	{
		if (VWidenCondition(DEGREE4, restriction, norma, radius, 0).TestValue(GetValue()+CLOSE_TO_UPPER_BOUND_VALUE, GetLength()))
		{
			close_to_upper_bound=true;			
		}
	}
	//Для второй степени проверяем близость к третьей
	if (GetDegree()==DEGREE2)
	{
		if (VWidenCondition(DEGREE3, restriction, norma, radius, 0).TestValue(GetValue()+CLOSE_TO_UPPER_BOUND_VALUE, GetLength()))
		{
			close_to_upper_bound=true;			
		}
	}
	if (on_switch)
	{
		AddComment(SWITCH_SIGN);
		Exclude(EXCLUSION_REASON_SWITCH);
	}
	if (on_chamfered_joint)
	{
		AddComment(CHAMFERED_JOINT_SIGN);
		Exclude(EXCLUSION_REASON_SWITCH);
	}
}

/// Возвращает число элементарных отступлений
size_t VWidenOverstep::GetAmount() const
{
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		return ceil(GetLength()/WIDTH_ELEMENTARY_LENGTH);
	}
	else
	{
		double count=fmod(GetLength(), WIDTH_ELEMENTARY_LENGTH)>WIDTH_ELEMENTARY_LENGTH/2.0 ?
			ceil(GetLength()/WIDTH_ELEMENTARY_LENGTH) : floor(GetLength()/WIDTH_ELEMENTARY_LENGTH);
		return std::max<int>(1,static_cast<int>(count));	
	}	
}

/// Возвращает строку с описанием количества элементарных отступлений
std::wstring VWidenOverstep::GetAmountString() const
{
	return boost::str(boost::wformat(L"%d") % GetAmount());
}

/// Возвращает балловую оценку
int VWidenOverstep::GetPenalty() const
{
	// FIXME!!! последний параметр - радиус < 3000
	int penalty = DETECT_TABLES.GetWidenPenaltyTable().GetPenalty(
		RoundNormaWidth(this->norma), this->speeds.GetMaxSpeed(), this->GetValue(), 0);

	if (penalty < 10)	
	{
		if (this->GetAmount() > 10)		penalty += 11;
		else							penalty += (this->GetAmount() - 1);
	}
	else if (penalty < 50)
	{
		if (this->GetAmount() > 10)		penalty += 20;
		else							penalty += 2 * (this->GetAmount() - 1);
	}
	else
	{
		if (this->GetAmount() > 10)		penalty += 50;
		else							penalty += 5 * (this->GetAmount() - 1);
	}	

	return penalty;
}


//--------------------------------- Методы класса VWidenExpress -------------------------------------------

/// Конструктор
VWidenExpress::VWidenExpress(const VExcess& in_excess)
{
	start=in_excess.GetStart();
	end=in_excess.GetEnd();
	if (COORD_CORRECTION_FUNCTION().Value().Valid())
	{
		start=COORD_CORRECTION_FUNCTION().Value().Correct(start);
		end=COORD_CORRECTION_FUNCTION().Value().Correct(end);		
	}
	else
	{
		AddComment(INACCURATE_COORDINATE_SIGN);
	}
	waycoord=SITUATION.GetRealCoord(GetCoord());
	max_value=in_excess.GetMax();
	min_value=in_excess.GetMin();
	value=round(max_value, DEFAULT_PRECISION);
	length=ceil(fabs(start-end));
	type=WIDEN;
	degree=DEGREE_NONE;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	restriction=VSpeeds(0,0,0);	
}

//--------------------------------- Методы класса VNarrowOverstep -----------------------------------------

/// Конструктор
VNarrowOverstep::VNarrowOverstep(const VExcess& in_excess, DEGREE in_degree)
{
	start=in_excess.GetStart();
	end=in_excess.GetEnd();
	waycoord=SITUATION.GetRealCoord(GetCoord());
	max_value=in_excess.GetMax();
	min_value=in_excess.GetMin();
	value=round(min_value, DEFAULT_PRECISION);
	length=ceil(fabs(start-end));
	type=NARROW;
	degree=in_degree;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	norma=SITUATION.GetWidth(GetCoord());
	zbsh=SITUATION.GetSpala(GetCoord())==SPALA_GB96;		
	on_switch=SITUATION.OnSwitch(pro::interval<double>(std::min<double>(start, end), std::max<double>(start, end)));
	on_chamfered_joint=SITUATION.GetChamJoint(start) || SITUATION.GetChamJoint(end);
	restriction=speeds;	
	//Ограничения 4-й степени
	if (GetDegree()==DEGREE4)
	{				
		while (restriction.GetMaxSpeed()>0 && VNarrowCondition(DEGREE4, restriction, norma, zbsh).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);				
		}		
	}		
	//Для третьей степени проверяем близость к четвертой
	if (GetDegree()==DEGREE3)
	{
		if (VNarrowCondition(DEGREE4, restriction, norma, zbsh).TestValue(GetValue()-CLOSE_TO_UPPER_BOUND_VALUE, GetLength()))
		{
			close_to_upper_bound=true;			
		}
	}
	//Для второй степени проверяем близость к третьей
	if (GetDegree()==DEGREE2)
	{
		if (VNarrowCondition(DEGREE3, restriction, norma, zbsh).TestValue(GetValue()-CLOSE_TO_UPPER_BOUND_VALUE, GetLength()))
		{
			close_to_upper_bound=true;			
		}
	}
	if (on_switch)
	{
		AddComment(SWITCH_SIGN);
		Exclude(EXCLUSION_REASON_SWITCH);
	}
	if (on_chamfered_joint)
	{
		AddComment(CHAMFERED_JOINT_SIGN);
		Exclude(EXCLUSION_REASON_SWITCH);
	}
	if (zbsh && degree<DEGREE3)
	{
		AddComment(ZBSH_SIGN);
	}
}

/// Возвращает число элементарных отступлений
size_t VNarrowOverstep::GetAmount() const
{
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		return ceil(GetLength()/WIDTH_ELEMENTARY_LENGTH);
	}
	else
	{
		double count=fmod(GetLength(), WIDTH_ELEMENTARY_LENGTH)>WIDTH_ELEMENTARY_LENGTH/2.0 ?
				ceil(GetLength()/WIDTH_ELEMENTARY_LENGTH) : floor(GetLength()/WIDTH_ELEMENTARY_LENGTH);
		return std::max<int>(1,static_cast<int>(count));
	}
}

/// Возвращает строку с описанием количества элементарных отступлений
std::wstring VNarrowOverstep::GetAmountString() const
{
	return boost::str(boost::wformat(L"%d") % GetAmount());
}

/// Балловая оценка
int VNarrowOverstep::GetPenalty() const
{
	int penalty = DETECT_TABLES.GetNarrowPenaltyTable().GetPenalty(
		RoundNormaWidth(this->norma), this->speeds.GetMaxSpeed(), this->GetValue(), this->zbsh ? 1 : 0);
	if (penalty < 10)	
	{
		if (this->GetAmount() > 10)		penalty += 11;
		else							penalty += (this->GetAmount() - 1);
	}
	else if (penalty < 50)
	{
		if (this->GetAmount() > 10)		penalty += 20;
		else							penalty += 2 * (this->GetAmount() - 1);
	}
	else
	{
		if (this->GetAmount()	 > 10)	penalty += 50;
		else							penalty += 5 * (this->GetAmount() - 1);
	}	

	return penalty;
}

//--------------------------------- Методы класса VNarrowExpress -------------------------------------------

/// Конструктор
VNarrowExpress::VNarrowExpress(const VExcess& in_excess)
{
	start=in_excess.GetStart();
	end=in_excess.GetEnd();
	if (COORD_CORRECTION_FUNCTION().Value().Valid())
	{
		start=COORD_CORRECTION_FUNCTION().Value().Correct(start);
		end=COORD_CORRECTION_FUNCTION().Value().Correct(end);		
	}
	else
	{
		AddComment(INACCURATE_COORDINATE_SIGN);
	}
	waycoord=SITUATION.GetRealCoord(GetCoord());
	max_value=in_excess.GetMax();
	min_value=in_excess.GetMin();
	value=round(min_value, DEFAULT_PRECISION);
	length=ceil(fabs(start-end));
	type=NARROW;
	degree=DEGREE_NONE;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	restriction=VSpeeds(0,0,0);
}


//--------------------------------- Методы класса VW10Overstep ---------------------------------------

/// Конструктор
VW10Overstep::VW10Overstep(double in_start, double in_end, double in_amount)
{
	start=in_start;
	end=in_end;
	amount=in_amount;
	waycoord=SITUATION.GetRealCoord((start+end)/2);	
	type=W10;
}

/// Возвращает число элементарных отступлений
size_t VW10Overstep::GetAmount() const
{	
	return 0;
}

/// Возвращает балловую оценку
int VW10Overstep::GetPenalty() const
{
	return 0;
}
/// Возвращает группу
const OVERSTEP_GROUP VW10Overstep::GetGroup() const 
{
	return GROUP_GEOMETRY;
};

/// Возвращаее строку с описанием координаты
std::wstring VW10Overstep::GetCoordString() const
{
	return L"";
}

/// Возвращает строку с описанием степени
std::wstring VW10Overstep::GetDegreeString() const
{
	return L"";
}

/// Возвращает строку с описанием значения
std::wstring VW10Overstep::GetValueString() const
{
	return L"";
}

/// Возвращает строку с описанием длины
std::wstring VW10Overstep::GetLengthString() const
{
	return L"";
}

/// Возвращает строку с описанием количества элементарных отступлений
std::wstring VW10Overstep::GetAmountString() const
{
	return boost::str(boost::wformat(L"%0.0f") % amount);
}

/// Возвращает описание ограничений скорости
std::wstring VW10Overstep::GetRestrictionString() const
{
	return L"";
}


//--------------------------------- Методы класса VK100Overstep ---------------------------------------

/// Конструктор
VK100Overstep::VK100Overstep(double in_start, double in_end, double in_amount)
{
	start=in_start;
	end=in_end;
	amount=in_amount;
	waycoord=SITUATION.GetRealCoord((start+end)/2);	
	type=K100;
}

/// Возвращает число элементарных отступлений
size_t VK100Overstep::GetAmount() const
{	
	return 0;
}

/// Возвращает балловую оценку
int VK100Overstep::GetPenalty() const
{
	return 0;
}
/// Возвращает группу
const OVERSTEP_GROUP VK100Overstep::GetGroup() const 
{
	return GROUP_GEOMETRY;
};

/// Возвращаее строку с описанием координаты
std::wstring VK100Overstep::GetCoordString() const
{
	return L"";
}

/// Возвращает строку с описанием степени
std::wstring VK100Overstep::GetDegreeString() const
{
	return L"";
}

/// Возвращает строку с описанием значения
std::wstring VK100Overstep::GetValueString() const
{
	return L"";
}

/// Возвращает строку с описанием длины
std::wstring VK100Overstep::GetLengthString() const
{
	return L"";
}

/// Возвращает строку с описанием количества элементарных отступлений
std::wstring VK100Overstep::GetAmountString() const
{
	return boost::str(boost::wformat(L"%0.0f") % amount);
}

/// Возвращает описание ограничений скорости
std::wstring VK100Overstep::GetRestrictionString() const
{
	return L"";
}

//--------------------------------- Методы класса VK60Overstep ---------------------------------------

/// Конструктор
VK60Overstep::VK60Overstep(double in_start, double in_end, double in_amount)
{
	start=in_start;
	end=in_end;
	amount=in_amount;
	waycoord=SITUATION.GetRealCoord((start+end)/2);	
	type=K60;
}

/// Возвращает число элементарных отступлений
size_t VK60Overstep::GetAmount() const
{	
	return 0;
}

/// Возвращает балловую оценку
int VK60Overstep::GetPenalty() const
{
	return 0;
}
/// Возвращает группу
const OVERSTEP_GROUP VK60Overstep::GetGroup() const 
{
	return GROUP_GEOMETRY;
};

/// Возвращаее строку с описанием координаты
std::wstring VK60Overstep::GetCoordString() const
{
	return L"";
}

/// Возвращает строку с описанием степени
std::wstring VK60Overstep::GetDegreeString() const
{
	return L"";
}

/// Возвращает строку с описанием значения
std::wstring VK60Overstep::GetValueString() const
{
	return L"";
}

/// Возвращает строку с описанием длины
std::wstring VK60Overstep::GetLengthString() const
{
	return L"";
}

/// Возвращает строку с описанием количества элементарных отступлений
std::wstring VK60Overstep::GetAmountString() const
{
	return boost::str(boost::wformat(L"%0.0f") % amount);
}

/// Возвращает описание ограничений скорости
std::wstring VK60Overstep::GetRestrictionString() const
{
	return L"";
}



//-------------------------------- Методы  класса VOtvodWidthOverstep -------------------------------------

/// Конструктор
VOtvodWidthOverstep::VOtvodWidthOverstep(const VExcess& in_excess)
{
	start=in_excess.GetStart();
	end=in_excess.GetEnd();
	waycoord=SITUATION.GetRealCoord(GetCoord());
	max_value=in_excess.GetMax();
	min_value=in_excess.GetMin();
	value=round(max_value, OTVOD_WIDTH_PRECISION);
	length=ceil(fabs(start-end));
	type=OTVOD_WIDTH;
	degree=DEGREE_NONE;
	on_switch=SITUATION.OnSwitch(pro::interval<double>(std::min<double>(start, end), std::max<double>(start, end)));
	on_chamfered_joint=SITUATION.GetChamJoint(start) || SITUATION.GetChamJoint(end);
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	restriction=speeds;	
	// Ограничение скорости
	if (GetLength()>=OTVOD_WIDTH_QUANTUM && !(on_switch || on_chamfered_joint))
	{		
		while (restriction.GetMaxSpeed()>0 && VOtvodWidthCondition(restriction).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);			
		}
	}
	// Отступление близкое к опасному
	if (!HasRestriction() && GetLength()>=OTVOD_WIDTH_QUANTUM && !(on_switch || on_chamfered_joint) && VOtvodWidthCondition(speeds).TestValue(GetValue()+CLOSE_TO_UPPER_BOUND_VALUE, GetLength()))
	{
		close_to_upper_bound=true;		
	}	
	if (on_switch)
	{
		AddComment(SWITCH_SIGN);
		Exclude(EXCLUSION_REASON_SWITCH);
	}
	if (on_chamfered_joint)
	{
		AddComment(CHAMFERED_JOINT_SIGN);
		Exclude(EXCLUSION_REASON_SWITCH);
	}
}

/// Возвращает строку с описанием значения
std::wstring VOtvodWidthOverstep::GetValueString() const
{
	return boost::str(boost::wformat(L"%1%") % GetValue());
}

/// штраф
int VOtvodWidthOverstep::GetPenalty() const
{
	return this->HasRestriction() ? 50 : 0;
}

//-------------------------------- Методы  класса VOtvodLevelOverstep -------------------------------------

/// Конструктор
VOtvodLevelOverstep::VOtvodLevelOverstep(const VExcess& in_excess)
{
	start=in_excess.GetStart();
	end=in_excess.GetEnd();
	waycoord=SITUATION.GetRealCoord(GetCoord());
	max_value=in_excess.GetMax();
	min_value=in_excess.GetMin();
	value=round(max_value, OTVOD_LEVEL_PRECISION);
	length=ceil(fabs(start-end));
	type=OTVOD_LEVEL;
	degree=DEGREE_NONE;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	restriction=speeds;	
	// Ограничение скорости
	if (GetLength()>=OTVOD_LEVEL_QUANTUM)
	{		
		while (restriction.GetMaxSpeed()>0 && VOtvodLevelPassCondition(restriction).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);			
		}
		while (restriction.GetTruckSpeed()>0 && VOtvodLevelTruckCondition(restriction).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseTruckSpeed(SPEED_QUANTUM);			
		}
	}
	// FixMe!!! Отступление близкое к опасному	
}

/// Возвращает строку с описанием значения
std::wstring VOtvodLevelOverstep::GetValueString() const
{
	return boost::str(boost::wformat(L"%1%") % GetValue());
}

/// штраф
int VOtvodLevelOverstep::GetPenalty() const
{
	return this->HasRestriction() ? 50 : 0;
}



//------------------------------------- Методы класса VWearOverstep ---------------------------------

/// Конструктор
VWearOverstep::VWearOverstep(const VExcess& in_excess, side_t in_side, DEGREE in_degree)
{
	start=in_excess.GetStart();
	end=in_excess.GetEnd();
	waycoord=SITUATION.GetRealCoord(GetCoord());
	max_value=in_excess.GetMax();
	min_value=in_excess.GetMin();
	value=round(max_value, WEAR_PRECISION);
	length=ceil(fabs(start-end));
	side=in_side;
	switch (side)
	{
		case SD_LEFT:
			type=WEAR_LEFT;
			break;
		case SD_RIGHT:
			type=WEAR_RIGHT;
			break;
		case SD_UNDEFINED:
		default:
			type=WEAR;
			break;
	}	
	degree=in_degree;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	test_run=SITUATION.GetTestRun(GetCoord());
	double radius=SITUATION.GetCurveRadius(GetCoord());
	AddComment(boost::str(boost::wformat(L"R=%0.0f ") % radius));
	restriction=speeds;
	on_switch=SITUATION.OnSwitch(pro::interval<double>(std::min<double>(start, end), std::max<double>(start, end)));
	on_chamfered_joint=SITUATION.GetChamJoint(start) || SITUATION.GetChamJoint(end);
	if (!(on_switch || on_chamfered_joint))
	{
		if (degree==DEGREE3)
		{
			restriction.RestrictMaxSpeed(70);		
		}
		if (degree==DEGREE4)
		{		
			restriction.RestrictMaxSpeed(50);
		}
		//Учет 283р
		if (DETECT_OPTIONS.GetOption(L"USE_283p").Compare(OPTION_YES))
		{
			if (pro::interval<double>(4, 6, false, true).contains(value) && restriction.RestrictMaxSpeed(200))
				AddComment(N283_SIGN);
			if (pro::interval<double>(6, DBL_MAX, false, false).contains(value) && restriction.RestrictMaxSpeed(140))
				AddComment(N283_SIGN);
		}
		//Учет НТД
		if (value>15 && DETECT_OPTIONS.GetOption(L"USE_NTD").Compare(OPTION_YES))
		{
			if (radius<=350)	
			{
				if (restriction.RestrictSpeed(VSpeeds(70,60,60)))
					AddComment(NTD_SIGN);			
			}
			else
			{
				if (restriction.RestrictSpeed(VSpeeds(80,70,70)))
					AddComment(NTD_SIGN);		
			}
		}
		//Ограничения в период обкатки после ремонта
		if (test_run!=MACHINE_NONE)
		{
			if (pro::interval<double>(18,20,false,true).contains(value) && (radius<=350) && restriction.RestrictSpeed(VSpeeds(80,70,70)))
			{
				AddComment(L"2790р");
			}
			else if (pro::interval<double>(18,20,false,true).contains(value) && (radius>350) && restriction.RestrictSpeed(VSpeeds(70,60,60)))
			{
				AddComment(L"2790р");
			}
			else if (pro::interval<double>(20,25,false,true).contains(value) && restriction.RestrictSpeed(VSpeeds(50,50,50)))
			{
				AddComment(L"2790р");
			}
			else if (pro::interval<double>(25,DBL_MAX,false,false).contains(value) && restriction.RestrictSpeed(VSpeeds(25,25,25)))
			{
				AddComment(L"2790р");
			}
		}
		
	}
	if (on_switch)
	{
		AddComment(SWITCH_SIGN);
		Exclude(EXCLUSION_REASON_SWITCH);
	}
	if (on_chamfered_joint)
	{
		AddComment(CHAMFERED_JOINT_SIGN);
		Exclude(EXCLUSION_REASON_SWITCH);
	}
}

//--------------------------------- Методы класса VWearExpress -------------------------------------------

/// Конструктор
VWearExpress::VWearExpress(const VExcess& in_excess, side_t in_side)
{
	start=in_excess.GetStart();
	end=in_excess.GetEnd();
	if (COORD_CORRECTION_FUNCTION().Value().Valid())
	{
		start=COORD_CORRECTION_FUNCTION().Value().Correct(start);
		end=COORD_CORRECTION_FUNCTION().Value().Correct(end);		
	}
	else
	{
		AddComment(INACCURATE_COORDINATE_SIGN);
	}
	waycoord=SITUATION.GetRealCoord(GetCoord());
	max_value=in_excess.GetMax();
	min_value=in_excess.GetMin();
	value=round(max_value, WEAR_PRECISION);
	length=ceil(fabs(start-end));
	side=in_side;
	switch (side)
	{
		case SD_LEFT:
			type=WEAR_LEFT;
			break;
		case SD_RIGHT:
			type=WEAR_RIGHT;
			break;
		case SD_UNDEFINED:
		default:
			type=WEAR;
			break;
	}
	degree=DEGREE_NONE;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	restriction=VSpeeds(0,0,0);	
}

//--------------------------------- Методы класса VLevelOverstep -----------------------------------------

/// Конструктор
VLevelOverstep::VLevelOverstep(const VExcess& in_excess, DEGREE in_degree)
{
	start=in_excess.GetStart();
	end=in_excess.GetEnd();
	waycoord=SITUATION.GetRealCoord(GetCoord());
	max_value=in_excess.GetMax();
	min_value=in_excess.GetMin();
	value=round(max_value, DEFAULT_PRECISION);
	length=ceil(fabs(start-end));
	type=LEVEL;
	degree=in_degree;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	bridge=SITUATION.GetBridge(GetCoord()) || SITUATION.GetTonnel(GetCoord());
	test_run=SITUATION.GetTestRun(GetCoord());
	restriction=speeds;	
	
	//Ограничения в период обкатки после ремонта
	if (test_run!=MACHINE_NONE)
	{
		while (restriction.GetMaxSpeed()>0 && VLevelConditionTestRun(restriction, test_run).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);			
		}
	}
	
	//Ограничения 4-й степени (в кривых скорость не ограничивается)
	if (GetDegree()==DEGREE4 && (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515) || 
		!(SITUATION.IsCurvePlan(start) || SITUATION.IsCurvePlan(end))))
	{
		while (restriction.GetMaxSpeed()>0 && VLevelCondition(DEGREE4, restriction).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);			
		}
	}
	//Ограничения 3-й степени на мосту и в тоннеле
	if (bridge && GetDegree()>=DEGREE3 && DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{						
		while (restriction.GetMaxSpeed()>0 && VLevelCondition(DEGREE3, restriction).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);				
		}
	}	
	//Для третьей степени проверяем близость к четвертой
	if (GetDegree()==DEGREE3)
	{
		if (VLevelCondition(DEGREE4, restriction).TestValue(GetValue()+CLOSE_TO_UPPER_BOUND_VALUE, GetLength()))
		{
			close_to_upper_bound=true;			
		}
	}
	//Для второй степени проверяем близость к третьей
	if (GetDegree()==DEGREE2)
	{
		if (VLevelCondition(DEGREE3, restriction).TestValue(GetValue()+CLOSE_TO_UPPER_BOUND_VALUE, GetLength()))
		{
			close_to_upper_bound=true;			
		}
	}
	//Признак моста
	if (bridge)
	{
		AddComment(BRIDGE_SIGN);
	}
	/// Если паспорт недостоверный, то исключаем отступление, с пометкой
	if (SITUATION.BadLevelPassport(start) || SITUATION.BadLevelPassport(end) || 
		SITUATION.BadCoordinatePassport(start) || SITUATION.BadCoordinatePassport(end))
	{
		AddComment(BAD_PASSPORT_SIGN);
		Exclude(EXCLUSION_REASON_BAD_PASSPORT);
	}
}

/// Возвращает число элементарных отступлений
size_t VLevelOverstep::GetAmount() const
{	
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		return ceil(GetLength()/LEVEL_ELEMENTARY_LENGTH_515);
	}
	else
	{
		double count=fmod(GetLength(), LEVEL_ELEMENTARY_LENGTH)>LEVEL_QUANTUM ?
			ceil(GetLength()/LEVEL_ELEMENTARY_LENGTH) : floor(GetLength()/LEVEL_ELEMENTARY_LENGTH);
		return std::max<int>(1,static_cast<int>(count));
	}
}

/// Возвращает строку с описанием количества элементарных отступлений
std::wstring VLevelOverstep::GetAmountString() const
{
	return boost::str(boost::wformat(L"%d") % GetAmount());
}

/// Балловая оценка
int VLevelOverstep::GetPenalty() const
{
	int penalty = DETECT_TABLES.GetLevelPenaltyTable().GetPenalty(0, this->speeds.GetMaxSpeed(), this->GetValue(), 0);

	// поправка на длину
	if (penalty < 10)
	{
		if (this->GetLength() > 50)	penalty += 5;
		else if (GetLength() > 40)	penalty += 4;
		else if (GetLength() > 30)	penalty += 3;
		else if (GetLength() > 20)	penalty += 2;
		else if (GetLength() > 10)	penalty += 1;
	}
	else if (penalty < 40)
	{
		if (GetLength() > 50)		penalty += 10;
		else if (GetLength() > 40)	penalty += 8;
		else if (GetLength() > 30)	penalty += 6;
		else if (GetLength() > 20)	penalty += 4;
		else if (GetLength() > 10)	penalty += 2;
	}
	else if (penalty < 80)
	{
		if (GetLength() > 50)		penalty += 20;
		else if (GetLength() > 40)	penalty += 16;
		else if (GetLength() > 30)	penalty += 12;
		else if (GetLength() > 20)	penalty += 8;
		else if (GetLength() > 10)	penalty += 4;
	}
	else
	{
		if (GetLength() > 50)		penalty += 50;
		else if (GetLength() > 40)	penalty += 40;
		else if (GetLength() > 30)	penalty += 30;
		else if (GetLength() > 20)	penalty += 20;
		else if (GetLength() > 10)	penalty += 10;
	}

	return penalty;
}

//-------------------------------- Методы  класса VShortDistortionOverstep -------------------------------------

/// Конструктор
VShortDistortionOverstep::VShortDistortionOverstep(const VExcess& in_excess)
{
	start=in_excess.GetStart();
	end=in_excess.GetEnd();
	waycoord=SITUATION.GetRealCoord(GetCoord());
	max_value=in_excess.GetMax();
	min_value=in_excess.GetMin();
	value=round(max_value, DEFAULT_PRECISION);
	length=ceil(fabs(start-end));
	type=SHORT_DISTORTION;
	degree=DEGREE_NONE;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	//Ограничение скорости
	restriction=speeds;		
	while (restriction.GetMaxSpeed()>0 && VShortDistortionCondition(restriction).TestValue(GetValue(), GetLength()))
	{
		restriction.DecreaseSpeed(SPEED_QUANTUM);	
	}	
}

/// штраф
int VShortDistortionOverstep::GetPenalty() const
{
	return this->HasRestriction() ? 50 : 0;
}

//------------------------------------ Методы класса VPitOverstep --------------------------------------------

/// Конструктор
VPitOverstep::VPitOverstep(const VWave& in_wave, double in_value, double in_length, side_t in_side, DEGREE in_degree)
{
	start=in_wave.GetStart().GetCoord();
	end=in_wave.GetEnd().GetCoord();
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	max_value=in_value;
	min_value=in_value;
	value=round(in_value, DEFAULT_PRECISION);
	length=ceil(in_length);
	type=in_side==SD_LEFT ? PIT_LEFT : PIT_RIGHT;
	degree=in_degree;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	bridge=SITUATION.GetBridge(GetCoord()) || SITUATION.GetTonnel(GetCoord());
	prj=SITUATION.IsPrj(GetCoord());
	test_run=SITUATION.GetTestRun(GetCoord());
	restriction=speeds;
	
	//Ограничения в период обкатки после ремонта
	if (test_run!=MACHINE_NONE)
	{
		while (restriction.GetMaxSpeed()>0 && VPitConditionTestRun().TestValue(GetValue(),GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);			
		}
		AddComment(TEST_RUN_SIGN);
	}
	
	//Ограничения 4-й степени
	if (GetDegree()==DEGREE4)
	{		
		while (restriction.GetMaxSpeed()>0 && VPitCondition(DEGREE4, restriction, bridge).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);			
		}
	}
	//Ограничения 3-й степени на мосту и в тоннеле
	if (bridge && GetDegree()>=DEGREE3 && DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{						
		while (restriction.GetMaxSpeed()>0 && VPitCondition(DEGREE3, restriction, bridge).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);				
		}
	}
	//ПРЖ	
	if (prj && restriction.GetEmptySpeed()>60 && GetValue()>20.0)
	{
		if (restriction.RestrictEmptySpeed(60))
			AddComment(PRJ_SIGN);
	}		
	//Для третьей степени проверяем близость к четвертой
	if (GetDegree()==DEGREE3)
	{
		if (VPitCondition(DEGREE4, restriction, bridge).TestValue(GetValue()+CLOSE_TO_UPPER_BOUND_VALUE, GetLength()))
		{
			close_to_upper_bound=true;			
		}
	}
	//Для второй степени проверяем близость к третьей
	if (GetDegree()==DEGREE2)
	{
		if (VPitCondition(DEGREE3, restriction, bridge).TestValue(GetValue()+CLOSE_TO_UPPER_BOUND_VALUE, GetLength()))
		{
			close_to_upper_bound=true;			
		}
	}
	//Признак моста
	if (bridge)
	{
		AddComment(BRIDGE_SIGN);
	}
}

/// Возвращает балловую оценку
int VPitOverstep::GetPenalty() const
{
	int penalty = DETECT_TABLES.GetPitPenaltyTable().GetPenalty(0, this->speeds.GetMaxSpeed(), 
		this->GetValue(), this->prj ? 1 : 0);

	if (penalty == 5)
	{
		if (this->GetLength() > 6)	penalty = 4;
	}

	else if (penalty == 7)
	{
		if (this->GetLength() > 6) penalty = 6;
	}

	else if (penalty == 20)
	{
		if (this->GetLength() > 8)		penalty = 18;
		else if (this->GetLength() > 6)	penalty = 19;
	}

	else if (penalty == 24)
	{
		if (this->GetLength() > 8)		penalty = 20;
		else if (this->GetLength() > 6)	penalty = 22;
		else if (this->GetLength() > 4)	penalty = 23;
	}

	else if (penalty == 32)
	{
		if (this->GetLength() > 8)		penalty = 24;
		else if (this->GetLength() > 6)	penalty = 26;
		else if (this->GetLength() > 4)	penalty = 28;
		else if (this->GetLength() > 2)	penalty = 30;
	}

	else if (penalty == 48)
	{
		if (this->GetLength() > 8)		penalty = 32;
		else if (this->GetLength() > 6)	penalty = 36;
		else if (this->GetLength() > 4)	penalty = 40;
		else if (this->GetLength() > 2)	penalty = 44;
	}

	else if (penalty == 70)
	{
		if (this->GetLength() > 8)		penalty = 50;
		else if (this->GetLength() > 6)	penalty = 55;
		else if (this->GetLength() > 4)	penalty = 60;
		else if (this->GetLength() > 2)	penalty = 65;
	}

	else if (penalty == 150)
	{
		if (this->GetLength() > 8)		penalty = 100;
		else if (this->GetLength() > 6)	penalty = 110;
		else if (this->GetLength() > 5)	penalty = 120;
		else if (this->GetLength() > 4)	penalty = 130;
		else if (this->GetLength() > 2)	penalty = 140;
	}

	return penalty;
}

//------------------------------------ Методы класса VPitGroupOverstep ---------------------------------------

/// Конструктор
VPitGroupOverstep::VPitGroupOverstep(const std::deque<VBasicOverstepPtr>& in_group, side_t in_side)
{
	group=in_group;
	start=group.front()->GetCoord();
	end=group.back()->GetCoord();
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	max_value=0;
	min_value=0;
	value=0;
	length=ceil(fabs(end-start));
	type=in_side==SD_LEFT ? PIT_LEFT_GROUP : PIT_RIGHT_GROUP;
	degree=DEGREE_NONE;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	restriction=speeds;	
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		//restriction.RestrictMaxSpeed(60);
		for (size_t i=0; i<in_group.size(); ++i)
		{
			bool bridge=SITUATION.GetBridge(in_group.at(i)->GetCoord()) || SITUATION.GetTonnel(in_group.at(i)->GetCoord());
			///Ограничиваем максимальную скорость			
			while (restriction.GetMaxSpeed()>15 && VPitCondition(DEGREE3, restriction, bridge).TestValue(in_group.at(i)->GetValue(), 
				in_group.at(i)->GetLength()))
			{
				restriction.DecreaseSpeed(SPEED_QUANTUM);				
			}			
		}
	}		
	else
	{
		for (size_t i=0; i<in_group.size(); ++i)
		{
			bool bridge=SITUATION.GetBridge(in_group.at(i)->GetCoord()) || SITUATION.GetTonnel(in_group.at(i)->GetCoord());
			///Ограничиваем максимальную скорость			
			while (restriction.GetMaxSpeed()>140 && VPitCondition(DEGREE3, restriction, bridge).TestValue(in_group.at(i)->GetValue(), 
				in_group.at(i)->GetLength()))
			{
				restriction.DecreaseSpeed(SPEED_QUANTUM);				
			}
			///Ограничиваем пассажирскую скорость
			while (restriction.GetPassSpeed()>60)
				restriction.DecreasePassSpeed(SPEED_QUANTUM);
		}
	}
	AddComment(boost::str(boost::wformat(L"[%0.0f-%0.0f]") % 
		SITUATION.GetRealCoord(group.front()->GetCoord()).m % SITUATION.GetRealCoord(group.back()->GetCoord()).m));
}

/// Возвращает строку с описанием значения
std::wstring VPitGroupOverstep::GetValueString() const
{
	return L"";
}

/// Возвращает строку с описанием длины
std::wstring VPitGroupOverstep::GetLengthString() const
{	
	return L"";
}

/// Балловая оценка отступления
int VPitGroupOverstep::GetPenalty() const
{
	if (group.empty()) return 0;
	else
	{
		double value_min = group[0]->GetValue();
		double value_max = group[0]->GetValue();
		for (size_t i = 1; i < group.size(); ++i)
		{
			value_min = std::min<double>(value_min, group[i]->GetValue());
			value_max = std::max<double>(value_max, group[i]->GetValue());
		}

		if (value_min < 20.0) return 0;
		else if (value_max >= 21.0) return 100;
		else return 10;
	}
}


//------------------------------------ Методы класса VRichGroupOverstep ---------------------------------------

/// Конструктор
VRichGroupOverstep::VRichGroupOverstep(const std::deque<VBasicOverstepPtr>& in_group, side_t in_side)
{
	group=in_group;
	start=group.front()->GetCoord();
	end=group.back()->GetCoord();
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	max_value=0;
	min_value=0;
	value=0;
	length=ceil(fabs(end-start));
	side=in_side;
	type=side==SD_LEFT ? RICH_LEFT_GROUP : RICH_RIGHT_GROUP;
	degree=DEGREE_NONE;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	restriction=speeds;
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		//restriction.RestrictMaxSpeed(60);
		///Ограничиваем максимальную скорость
		for (size_t i=0; i<in_group.size(); ++i)
		{
			///Ограничиваем максимальную скорость
			bool bridge=SITUATION.GetBridge(in_group.at(i)->GetCoord()) || SITUATION.GetTonnel(in_group.at(i)->GetCoord());
			bool thread_side=SITUATION.GetSummaryThreadSide(in_group.at(i)->GetCoord())==in_side;
			bool curve=SITUATION.IsCurvePlan(in_group.at(i)->GetCoord());
			while (restriction.GetMaxSpeed()>15 && VRichCondition(DEGREE3, restriction, bridge, thread_side, curve).TestValue(in_group.at(i)->GetValue(), 
				in_group.at(i)->GetLength()))
			{
				restriction.DecreaseSpeed(SPEED_QUANTUM);				
			}			
		}
	}
	else
	{
		for (size_t i=0; i<in_group.size(); ++i)
		{
			///Ограничиваем максимальную скорость
			bool bridge=SITUATION.GetBridge(in_group.at(i)->GetCoord()) || SITUATION.GetTonnel(in_group.at(i)->GetCoord());
			bool thread_side=SITUATION.GetSummaryThreadSide(in_group.at(i)->GetCoord())==in_side;
			bool curve=SITUATION.IsCurvePlan(in_group.at(i)->GetCoord());
			while (restriction.GetMaxSpeed()>140 && VRichCondition(DEGREE3, restriction, bridge, thread_side, curve).TestValue(in_group.at(i)->GetValue(), 
				in_group.at(i)->GetLength()))
			{
				restriction.DecreaseSpeed(SPEED_QUANTUM);				
			}
			///Ограничиваем пассажирскую скорость
			while (restriction.GetPassSpeed()>60)
				restriction.DecreasePassSpeed(SPEED_QUANTUM);
		}
	}
	AddComment(boost::str(boost::wformat(L"[%0.0f-%0.0f]") % 
		SITUATION.GetRealCoord(group.front()->GetCoord()).m % SITUATION.GetRealCoord(group.back()->GetCoord()).m));
}

/// Возвращаее строку с описанием типа
std::wstring VRichGroupOverstep::GetTypeString() const
{	
	bool thread_side=SITUATION.GetSummaryThreadSide(GetCoord())==side;
	return thread_side ? RICH_GROUP : RICH_GROUP_NR;
}

/// Возвращает строку с описанием значения
std::wstring VRichGroupOverstep::GetValueString() const
{
	return L"";
}

/// Возвращает строку с описанием длины
std::wstring VRichGroupOverstep::GetLengthString() const
{	
	return L"";
}

/// Балловая оценка отступления
int VRichGroupOverstep::GetPenalty() const
{
	if (group.empty()) return 0;
	else
	{
		double value_min = group[0]->GetValue();
		double value_max = group[0]->GetValue();
		for (size_t i = 1; i < group.size(); ++i)
		{
			value_min = std::min<double>(value_min, group[i]->GetValue());
			value_max = std::max<double>(value_max, group[i]->GetValue());
		}

		if (value_min < 25.0) return 0;
		else if (value_max >= 26.0) return 100;
		else return 10;
	}
}

//----------------------------------- Методы класса VDistiortionOverstep ------------------------------

/// Конструктор
VDistortionOverstep::VDistortionOverstep(const VWave& in_wave, double in_value, double in_length, DEGREE in_degree)
{
	start=in_wave.GetStart().GetCoord();
	end=in_wave.GetEnd().GetCoord();
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	max_value=in_value;
	min_value=in_value;
	value=in_value;
	length=ceil(in_length);
	type=DISTORTION;
	degree=in_degree;	
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	bridge=SITUATION.GetBridge(GetCoord()) || SITUATION.GetTonnel(GetCoord());
	prj=SITUATION.IsPrj(GetCoord());
	test_run=SITUATION.GetTestRun(GetCoord());
	restriction=speeds;	
	
	//Ограничения в период обкатки после ремонта
	if (test_run!=MACHINE_NONE)
	{
		while (restriction.GetMaxSpeed()>0 && VDistortionConditionTestRun(restriction, test_run).TestValue(GetValue(),GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);			
		}
		AddComment(TEST_RUN_SIGN);
	}
	
	//Ограничения 4-й степени
	if (GetDegree()==DEGREE4)
	{				
		while (restriction.GetMaxSpeed()>0 && VDistortionCondition(DEGREE4, restriction, bridge).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);			
		}
	}
	//Ограничения 3-й степени на мосту и в тоннеле
	if (bridge && GetDegree()>=DEGREE3 && DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{						
		while (restriction.GetMaxSpeed()>0 && VDistortionCondition(DEGREE3, restriction, bridge).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);				
		}
	}
	//ПРЖ	
	if (prj && restriction.GetEmptySpeed()>60 && GetLength()<10.0 && 
		(NEW_INSTRUCTION && GetValue()>13.0 || OLD_INSTRUCTION && GetDegree()==DEGREE3))
	{
		if (restriction.RestrictEmptySpeed(60))
			AddComment(PRJ_SIGN);
	}			
	//Для третьей степени проверяем близость к четвертой
	if (GetDegree()==DEGREE3)
	{
		if (VDistortionCondition(DEGREE4, restriction, bridge).TestValue(GetValue()+CLOSE_TO_UPPER_BOUND_VALUE, GetLength()))
		{
			close_to_upper_bound=true;			
		}
	}
	//Для второй степени проверяем близость к третьей
	if (GetDegree()==DEGREE2)
	{
		if (VDistortionCondition(DEGREE3, restriction, bridge).TestValue(GetValue()+CLOSE_TO_UPPER_BOUND_VALUE, GetLength()))
		{
			close_to_upper_bound=true;			
		}
	}	
	//Признак моста
	if (bridge)
	{
		AddComment(BRIDGE_SIGN);
	}
	/// Если паспорт недостоверный, то помечаем отступление
	if (SITUATION.BadLevelPassport(start) || SITUATION.BadLevelPassport(end) || 
		SITUATION.BadCoordinatePassport(start) || SITUATION.BadCoordinatePassport(end))
	{
		AddComment(BAD_PASSPORT_SIGN);
		if (BAD_PASSPORT_HANDLING_POLICY().Value()==1)
			Exclude(EXCLUSION_REASON_BAD_PASSPORT);
	}
}

int VDistortionOverstep::GetPenalty() const
{
	int add_key;
	if (this->GetLength() > 10)	
	{
		add_key = 0;
	}
	else  // len <= 10
	{
		add_key = this->prj ? 2 : 1;
	}

	int penalty = DETECT_TABLES.GetDistortionPenaltyTable().GetPenalty(
		0, this->speeds.GetMaxSpeed(), this->GetValue(), add_key);

	if (penalty == 7)
	{
		if (this->GetLength() > 10)	penalty = 6;
	}

	else if (penalty == 20)
	{
		if (this->GetLength() > 18)			penalty = 18;
		else if (this->GetLength() > 10)	penalty = 19;
	}

	else if (penalty == 24)
	{
		if (this->GetLength() > 18)			penalty = 20;
		else if (this->GetLength() > 10)	penalty = 22;
	}

	else if (penalty == 32)
	{
		if (this->GetLength() > 18)			penalty = 24;
		else if (this->GetLength() > 14)	penalty = 26;
		else if (this->GetLength() > 10)	penalty = 28;
		else if (this->GetLength() > 5)		penalty = 30;
	}

	else if (penalty == 48)
	{
		if (this->GetLength() > 18)			penalty = 32;
		else if (this->GetLength() > 14)	penalty = 36;
		else if (this->GetLength() > 10)	penalty = 40;
		else if (this->GetLength() > 5)		penalty = 44;
	}

	else if (penalty == 70)
	{
		if (this->GetLength() > 18)			penalty = 50;
		else if (this->GetLength() > 14)	penalty = 55;
		else if (this->GetLength() > 10)	penalty = 60;
		else if (this->GetLength() > 5)		penalty = 65;
	}

	else if (penalty == 150)
	{
		if (this->GetLength() > 18)			penalty = 100;
		else if (this->GetLength() > 16)	penalty = 110;
		else if (this->GetLength() > 14)	penalty = 120;
		else if (this->GetLength() > 10)	penalty = 130;
		else if (this->GetLength() > 5)		penalty = 140;
	}

	return penalty;
}

//------------------------------------ Методы класса VDistortionGroupOverstep ---------------------------------------

/// Конструктор
VDistortionGroupOverstep::VDistortionGroupOverstep(const std::deque<VBasicOverstepPtr>& in_group)
{
	group=in_group;
	start=group.front()->GetCoord();
	end=group.back()->GetCoord();
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	max_value=0;
	min_value=0;
	value=0;
	length=ceil(fabs(end-start));
	type=DISTORTION_GROUP;
	degree=DEGREE_NONE;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	restriction=speeds;
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		for (size_t i=0; i<in_group.size(); ++i)
		{
			///Ограничиваем максимальную скорость
			bool bridge=SITUATION.GetBridge(in_group.at(i)->GetCoord()) || SITUATION.GetTonnel(in_group.at(i)->GetCoord());
			while (restriction.GetMaxSpeed()>15 && VDistortionCondition(DEGREE3, restriction, bridge).TestValue(in_group.at(i)->GetValue(), 
				in_group.at(i)->GetLength()))
			{
				restriction.DecreaseSpeed(SPEED_QUANTUM);				
			}			
		}		
	}
	else
	{
		for (size_t i=0; i<in_group.size(); ++i)
		{
			///Ограничиваем максимальную скорость
			bool bridge=SITUATION.GetBridge(in_group.at(i)->GetCoord()) || SITUATION.GetTonnel(in_group.at(i)->GetCoord());
			while (restriction.GetMaxSpeed()>140 && VDistortionCondition(DEGREE3, restriction, bridge).TestValue(in_group.at(i)->GetValue(), 
				in_group.at(i)->GetLength()))
			{
				restriction.DecreaseSpeed(SPEED_QUANTUM);				
			}
			///Ограничиваем пассажирскую скорость
			while (restriction.GetPassSpeed()>60)
				restriction.DecreasePassSpeed(SPEED_QUANTUM);
		}
	}
	AddComment(boost::str(boost::wformat(L"[%0.0f-%0.0f]") % 
		SITUATION.GetRealCoord(group.front()->GetCoord()).m % SITUATION.GetRealCoord(group.back()->GetCoord()).m));	
}

/// Возвращает строку с описанием значения
std::wstring VDistortionGroupOverstep::GetValueString() const
{
	return L"";
}

/// Возвращает строку с описанием длины
std::wstring VDistortionGroupOverstep::GetLengthString() const
{	
	return L"";
}

/// Балловая оценка отступления
int VDistortionGroupOverstep::GetPenalty() const
{
	if (group.empty()) return 0;
	else
	{
		double value_min = group[0]->GetValue();
		double value_max = group[0]->GetValue();
		for (size_t i = 1; i < group.size(); ++i)
		{
			value_min = std::min<double>(value_min, group[i]->GetValue());
			value_max = std::max<double>(value_max, group[i]->GetValue());
		}

		if (value_min < 16.0) return 0;
		else if (value_max >= 17.0) return 100;
		else return 10;
	}
}

//----------------------------------- Методы класса VRichOverstep ------------------------------------

/// Конструктор
VRichOverstep::VRichOverstep(const VWave& in_wave, double in_value, double in_length, side_t in_side, DEGREE in_degree)
{
	start=in_wave.GetStart().GetCoord();
	end=in_wave.GetEnd().GetCoord();
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	max_value=in_value;
	min_value=in_value;
	value=in_value;
	length=ceil(in_length);
	type=in_side==SD_LEFT ? RICH_LEFT : RICH_RIGHT;
	degree=in_degree;	
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	bridge=SITUATION.GetBridge(GetCoord()) || SITUATION.GetTonnel(GetCoord());
	test_run=SITUATION.GetTestRun(GetCoord());
	on_switch=SITUATION.OnSwitch(pro::interval<double>(std::min<double>(start, end), std::max<double>(start, end)));
	on_chamfered_joint=SITUATION.GetChamJoint(start) || SITUATION.GetChamJoint(end);
	prj=SITUATION.IsPrj(GetCoord());
	thread_side=SITUATION.GetSummaryThreadSide(GetCoord())==in_side;
	curve=SITUATION.IsCurvePlan(GetCoord());
	restriction=speeds;	
	
	//Ограничения в период обкатки после ремонта
	if (test_run!=MACHINE_NONE)
	{
		while (restriction.GetMaxSpeed()>0 && VRichConditionTestRun(restriction, test_run).TestValue(GetValue(),GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);			
		}
		AddComment(TEST_RUN_SIGN);
	}
	
	//Ограничения 4-й степени
	if (GetDegree()==DEGREE4)
	{						
		while (restriction.GetMaxSpeed()>0 && VRichCondition(DEGREE4, restriction, bridge, thread_side, curve).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);				
		}
	}
	//Ограничения 3-й степени на мосту и в тоннеле при оценке по ЦП-515
	if (bridge && GetDegree()>=DEGREE3 && DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{						
		while (restriction.GetMaxSpeed()>0 && VRichCondition(DEGREE3, restriction, bridge, thread_side, curve).TestValue(GetValue(), GetLength()))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);				
		}
	}
	//ПРЖ	
	if (prj && restriction.GetEmptySpeed()>60 && (pro::interval<double>(0,10).contains(GetLength()) && GetValue()>20.0 ||
		pro::interval<double>(10,20).contains(GetLength()) && GetValue()>25.0))
	{
		if (restriction.RestrictEmptySpeed(60))
			AddComment(PRJ_SIGN);
	}			
	//Для третьей степени проверяем близость к четвертой
	if (GetDegree()==DEGREE3)
	{
		if (VRichCondition(DEGREE4, restriction, bridge, thread_side, curve).TestValue(GetValue()+CLOSE_TO_UPPER_BOUND_VALUE, GetLength()))
		{
			close_to_upper_bound=true;			
		}
	}
	//Для второй степени проверяем близость к третьей
	if (GetDegree()==DEGREE2)
	{
		if (VRichCondition(DEGREE3, restriction, bridge, thread_side, curve).TestValue(GetValue()+CLOSE_TO_UPPER_BOUND_VALUE, GetLength()))
		{
			close_to_upper_bound=true;			
		}
	}
	//Проверяем наличие стрелки
	if (on_switch)
	{
		AddComment(SWITCH_SIGN);
		Exclude(EXCLUSION_REASON_SWITCH);
	}	
	if (on_chamfered_joint)
	{
		AddComment(CHAMFERED_JOINT_SIGN);
		Exclude(EXCLUSION_REASON_SWITCH);
	}
	//Признак моста
	if (bridge)
	{
		AddComment(BRIDGE_SIGN);
	}
	/// Если паспорт недостоверный, то помечаем отступление
	if (SITUATION.BadPlanPassport(start) || SITUATION.BadPlanPassport(end) ||
		SITUATION.BadCoordinatePassport(start) || SITUATION.BadCoordinatePassport(end))
	{
		AddComment(BAD_PASSPORT_SIGN);
		if (BAD_PASSPORT_HANDLING_POLICY().Value()==1)
			Exclude(EXCLUSION_REASON_BAD_PASSPORT);
	}
}

/// Возвращаее строку с описанием типа
std::wstring VRichOverstep::GetTypeString() const
{	
	return thread_side ? RICH : RICH_NR;
}

/// Возвращает строку с описанием длины
std::wstring VRichOverstep::GetLengthString() const
{
	//Отображается удвоенная длина рихтовки
	return boost::str(boost::wformat(L"%0.0f") % (GetLength()));
}

int VRichOverstep::GetPenalty() const
{
	int add_key;
	if (this->GetLength() <= 10.0 && this->curve)
	{
		add_key = 0; 
	}
	else if (this->GetLength() <= 20.0 && this->curve)
	{
		add_key = 1; 
	}
	else if (this->GetLength() <= 20.0)
	{
		add_key = 2; 
	}
	else if (this->GetLength() <= 40.0)
	{
		add_key = 3; 
	}
	else 
	{
		add_key = 4; 
	}

	int penalty = DETECT_TABLES.GetRichPenaltyTable().GetPenalty(0, this->speeds.GetMaxSpeed(), this->GetValue(), add_key);

	// поправка на длину
	if (penalty == 5 && this->GetLength() > 18.0)			penalty = 4;
	else if (penalty == 7 && this->GetLength() > 18.0)		penalty = 6;
	else if (penalty == 20)
	{
		if (this->GetLength() > 35.0)		penalty = 18;
		else if (this->GetLength() > 18.0)	penalty = 19;
	}
	else if (penalty == 24)
	{
		if (this->GetLength() >= 35.0)		penalty = 20;
		else if (this->GetLength() > 18.0)	penalty = 22;
	}
	else if (penalty == 32)
	{
		if (this->GetLength() > 35.0)		penalty = 24;
		else if (this->GetLength() > 26.0)	penalty = 26;
		else if (this->GetLength() > 18.0)	penalty = 28;
		else if (this->GetLength() > 8.0)	penalty = 30;
	}
	else if (penalty == 48)
	{
		if (this->GetLength() > 35.0)		penalty = 32;
		else if (this->GetLength() > 26.0)	penalty = 36;
		else if (this->GetLength() > 10.0)	penalty = 40;
		else if (this->GetLength() > 8.0)	penalty = 44;
	}
	else if (penalty == 70)
	{
		if (this->GetLength() > 35.0)		penalty = 50;
		else if (this->GetLength() > 26.0)	penalty = 55;
		else if (this->GetLength() > 18.0)	penalty = 60;
		else if (this->GetLength() > 8.0)	penalty = 65;
	}
	else if (penalty == 150)
	{
		if (this->GetLength() > 35.0)		penalty = 100;
		else if (this->GetLength() > 30.0)	penalty = 110;
		else if (this->GetLength() > 26.0)	penalty = 120;
		else if (this->GetLength() > 18.0)	penalty = 130;
		else if (this->GetLength() > 8.0)	penalty = 140;
	}

	return penalty;
}

//----------------------------------- Методы класса VRichAndPitCombination ---------------------------------------

/// Конструктор
VRichAndPitCombination::VRichAndPitCombination(VBasicOverstepPtr in_overstep1, VBasicOverstepPtr in_overstep2)
{
	rich_overstep=is_rich(in_overstep1) ? in_overstep1 : in_overstep2;
	pit_overstep=is_pit(in_overstep1) ? in_overstep1 : in_overstep2;

	start=rich_overstep->GetCoord();
	end=pit_overstep->GetCoord();
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	max_value=0;
	min_value=0;
	value=0;
	length=0;
	type=RICH_PIT_COMBINATION;
	degree=DEGREE_NONE;	
	prj=SITUATION.IsPrj(GetCoord());
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	restriction=speeds;
	bool bridge=SITUATION.GetBridge(GetCoord()) || SITUATION.GetTonnel(GetCoord());
	bool thread_side=true;
	bool curve=SITUATION.IsCurvePlan(GetCoord());
	//Ограничение скорости
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{		
		//Уменьшаем скорость до тех пор пока оба отступления в сочетании не станут отступлениями второй степени 
		while (restriction.GetMaxSpeed()>15 && 
			(VRichCondition(DEGREE3, restriction, bridge, thread_side, curve).TestValue(rich_overstep->GetValue(), rich_overstep->GetLength()) || 
			 VRichCondition(DEGREE4, restriction, bridge, thread_side, curve).TestValue(rich_overstep->GetValue(), rich_overstep->GetLength())) ||
			(VPitCondition(DEGREE3, restriction, bridge).TestValue(pit_overstep->GetValue(), pit_overstep->GetLength()) ||
			 VPitCondition(DEGREE4, restriction, bridge).TestValue(pit_overstep->GetValue(), pit_overstep->GetLength())))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);				
		}
	}
	else
	{
		//Скоростные
		//Уменьшаем скорость до тех пор пока одно из отступлений в сочетании не станет отступлением второй степени, т.е. сочетание исчезнет
		while (restriction.GetMaxSpeed()>140 && 
				(VRichCondition(DEGREE3, restriction, bridge, thread_side, curve).TestValue(rich_overstep->GetValue(), rich_overstep->GetLength()) ||
				 VRichCondition(DEGREE4, restriction, bridge, thread_side, curve).TestValue(rich_overstep->GetValue(), rich_overstep->GetLength())) &&
				(VPitCondition(DEGREE3, restriction, bridge).TestValue(pit_overstep->GetValue(), pit_overstep->GetLength()) ||
				 VPitCondition(DEGREE4, restriction, bridge).TestValue(pit_overstep->GetValue(), pit_overstep->GetLength())))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);				
		}
		if (rich_overstep->GetLength()<20.0)
		{
			if (pit_overstep->GetValue()>15.0 && rich_overstep->GetValue()>15.0)
			{
				while (restriction.GetPassSpeed()>120)
					restriction.DecreasePassSpeed(SPEED_QUANTUM);			
			}
			if (pit_overstep->GetValue()>20.0 && rich_overstep->GetValue()>25.0)
			{
				while (restriction.GetPassSpeed()>120)
					restriction.DecreasePassSpeed(SPEED_QUANTUM);
			}	
			//ПРЖ	
			if (prj && pit_overstep->GetValue()>15.0 && rich_overstep->GetValue()>15.0)
			{
				if (restriction.RestrictEmptySpeed(60))
					AddComment(PRJ_SIGN);
			}		
		}
	}
	AddComment(boost::str(boost::wformat(L"[%0.0f-%0.0f]") % rich_overstep->GetWayCoord().m % pit_overstep->GetWayCoord().m));
}

/// Возвращает строку с описанием значения
std::wstring VRichAndPitCombination::GetValueString() const
{	
	return L"";
}

/// Возвращает строку с описанием длины
std::wstring VRichAndPitCombination::GetLengthString() const
{	
	return L"";
}

/// Балловая оценка отступления
int VRichAndPitCombination::GetPenalty() const
{
	double value_rich = this->rich_overstep->GetValue();
	double value_pit = this->pit_overstep->GetValue();
	int spd = this->speeds.GetMaxSpeed();
	int penalty = 0;
	if (spd > 60 && spd <= 120 && this->rich_overstep->GetLength() < 20.0)
	{
		if (this->prj)
		{
			if (value_pit >= 14.0 && value_pit < 15.0 && value_rich >= 16.0)		penalty = 5;
			else if (value_pit >= 15.0 && value_pit < 16.0 && value_rich >= 16.0)	penalty = 20;
			else if (value_pit >= 16.0 && value_rich >= 14.0 && value_rich < 15.0)	penalty = 5;
			else if (value_pit >= 16.0 && value_rich >= 15.0 && value_rich < 16.0)	penalty = 20;
			else if (value_pit >= 16.0 && value_rich >= 16.0)						penalty = 100;
		}
		else
		{
			if (value_pit >= 19.0 && value_pit < 20.0 && value_rich >= 26.0)		penalty = 5;
			else if (value_pit >= 20.0 && value_pit < 21.0 && value_rich >= 26.0)	penalty = 20;
			else if (value_pit >= 21.0 && value_rich >= 24.0 && value_rich < 25.0)	penalty = 5;
			else if (value_pit >= 21.0 && value_rich >= 25.0 && value_rich < 26.0)	penalty = 20;
			else if (value_pit >= 21.0 && value_rich >= 26.0)						penalty = 100;
		}
	}
	else if (spd > 120 && spd <= 140 && this->rich_overstep->GetLength() < 20.0)
	{
		if (value_pit >= 14.0 && value_pit < 15.0 && value_rich >= 16.0)		penalty = 5;
		else if (value_pit >= 15.0 && value_pit < 16.0 && value_rich >= 16.0)	penalty = 20;
		else if (value_pit >= 16.0 && value_rich >= 14.0 && value_rich < 15.0)	penalty = 5;
		else if (value_pit >= 16.0 && value_rich >= 15.0 && value_rich < 16.0)	penalty = 20;
		else if (value_pit >= 16.0 && value_rich >= 16.0)						penalty = 100;
	}
	return penalty;
}
	
//----------------------------------- Методы класса VRichAndDistortionCombination ---------------------------------------

/// Конструктор
VRichAndDistortionCombination::VRichAndDistortionCombination(VBasicOverstepPtr in_overstep1, 
	VBasicOverstepPtr in_overstep2)
{
	rich_overstep=is_rich(in_overstep1) ? in_overstep1 : in_overstep2;
	distortion_overstep=is_distortion(in_overstep1) ? in_overstep1 : in_overstep2;

	start=rich_overstep->GetCoord();
	end=distortion_overstep->GetCoord();
	waycoord=SITUATION.GetRealCoord((start+end)/2);	
	max_value=0;
	min_value=0;
	value=0;
	length=0;
	type=RICH_DISTORTION_COMBINATION;
	degree=DEGREE_NONE;	
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	prj=SITUATION.IsPrj(GetCoord());
	restriction=speeds;
	bool bridge=SITUATION.GetBridge(GetCoord()) || SITUATION.GetTonnel(GetCoord());
	bool thread_side=true;
	bool curve=SITUATION.IsCurvePlan(GetCoord());
	//Ограничение скорости
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{		
		//Уменьшаем скорость до тех пор пока оба отсупления в сочетании не станут отступлениями второй степени
		while (restriction.GetMaxSpeed()>15 && 
			(VRichCondition(DEGREE3, restriction, bridge, thread_side, curve).TestValue(rich_overstep->GetValue(), rich_overstep->GetLength()) || 
			 VRichCondition(DEGREE4, restriction, bridge, thread_side, curve).TestValue(rich_overstep->GetValue(), rich_overstep->GetLength())) ||
			(VDistortionCondition(DEGREE3, restriction, bridge).TestValue(distortion_overstep->GetValue(), distortion_overstep->GetLength()) ||
			 VDistortionCondition(DEGREE4, restriction, bridge).TestValue(distortion_overstep->GetValue(), distortion_overstep->GetLength())))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);				
		}
	}
	else 
	{
		//Скоростные
		//Уменьшаем скорость до тех пор пока одно из отступлений в сочетании не станет отступлением второй степени, т.е. сочетание исчезнет
		while (restriction.GetMaxSpeed()>140 && 
				(VRichCondition(DEGREE3, restriction, bridge, thread_side, curve).TestValue(rich_overstep->GetValue(), rich_overstep->GetLength()) ||
				 VRichCondition(DEGREE4, restriction, bridge, thread_side, curve).TestValue(rich_overstep->GetValue(), rich_overstep->GetLength())) &&
				(VDistortionCondition(DEGREE3, restriction, bridge).TestValue(distortion_overstep->GetValue(), distortion_overstep->GetLength()) ||
				 VDistortionCondition(DEGREE4, restriction, bridge).TestValue(distortion_overstep->GetValue(), distortion_overstep->GetLength())))
		{
			restriction.DecreaseSpeed(SPEED_QUANTUM);				
		}
		//Пассажирские
		if (distortion_overstep->GetLength()<10.0 && rich_overstep->GetLength()<20.0)
		{
			if (distortion_overstep->GetValue()>12.0 && rich_overstep->GetValue()>15.0)
			{
				restriction.RestrictMaxSpeed(120);			
			}
			if (distortion_overstep->GetValue()>16.0 && rich_overstep->GetValue()>25.0)
			{
				restriction.RestrictMaxSpeed(60);
			}
			//ПРЖ	
			if (prj && distortion_overstep->GetValue()>11.0 && rich_overstep->GetValue()>15.0)
			{
				if (restriction.RestrictEmptySpeed(60))
					AddComment(PRJ_SIGN);
			}		
		}
	}
	AddComment(boost::str(boost::wformat(L"[%0.0f-%0.0f]") % rich_overstep->GetWayCoord().m % distortion_overstep->GetWayCoord().m));
}

/// Возвращает строку с описанием значения
std::wstring VRichAndDistortionCombination::GetValueString() const
{	
	return L"";
}

/// Возвращает строку с описанием длины
std::wstring VRichAndDistortionCombination::GetLengthString() const
{	
	return L"";
}

/// Балловая оценка отступления
int VRichAndDistortionCombination::GetPenalty() const
{
	double value_rich = this->rich_overstep->GetValue();
	double value_dist = this->distortion_overstep->GetValue();
	int spd = this->speeds.GetMaxSpeed();
	int penalty = 0;
	if (spd > 120 && spd <= 140 && 
		this->distortion_overstep->GetLength() < 10.0 &&
		this->rich_overstep->GetLength() < 20.0)
	{
		if (value_dist >= 11.0 && value_dist < 12.0 && value_rich >= 16.0)		penalty = 5;
		else if (value_dist >= 12.0 && value_dist < 13.0 && value_rich >= 16.0)	penalty = 20;
		else if (value_dist >= 13.0 && value_rich >= 14.0 && value_rich < 15.0)	penalty = 5;
		else if (value_dist >= 13.0 && value_rich >= 15.0 && value_rich < 16.0)	penalty = 20;
		else if (value_dist >= 13.0 && value_rich >= 16.0)						penalty = 100;
	}
	else if (spd > 60 && spd <= 120 && 
		this->distortion_overstep->GetLength() < 10.0 &&
		this->rich_overstep->GetLength() < 20.0)
	{
		if (this->prj)
		{
			if (value_dist >= 10.0 && value_dist < 11.0 && value_rich >= 16.0)		penalty = 5;
			else if (value_dist >= 11.0 && value_dist < 12.0 && value_rich >= 16.0)	penalty = 20;
			else if (value_dist >= 12.0 && value_rich >= 14.0 && value_rich < 15.0)	penalty = 5;
			else if (value_dist >= 12.0 && value_rich >= 15.0 && value_rich < 16.0)	penalty = 20;
			else if (value_dist >= 12.0 && value_rich >= 16.0)						penalty = 100;
		}
		else
		{
			if (value_dist >= 15.0 && value_dist < 16.0 && value_rich >= 26.0)		penalty = 5;
			else if (value_dist >= 16.0 && value_dist < 17.0 && value_rich >= 26.0)	penalty = 20;
			else if (value_dist >= 17.0 && value_rich >= 24.0 && value_rich < 25.0)	penalty = 5;
			else if (value_dist >= 17.0 && value_rich >= 25.0 && value_rich < 26.0)	penalty = 20;
			else if (value_dist >= 17.0 && value_rich >= 26.0)						penalty = 100;
		}
	}

	return penalty;
}


//--------------------------------- Методы класса VCurveCalc -----------------------------------------------

/// Добавляет данные
void VCurveCalc::Put(const VPoint& in_curv, const VPoint& in_level)
{
	if (data.empty())
	{
		curve_sign=double(SITUATION.GetCurveOrientation(in_curv.GetCoord()));
	}
	//Сбойные кадры игнорируются FixMe!!! возможно лучше вычислять количество сбоев по аналогии с рихтовками и т.п.
	if (!(in_curv.GetCorrupt() || in_level.GetCorrupt()))
		data.push_back(DATA(in_curv, in_level));
};

/// Возвращает максимальное значение Анп для заданной скорости
double VCurveCalc::GetAverageAnp(int in_speed) const
{
	if (data.empty() || in_speed==NO_SPEED)
		return 0;
	accumulator_set< double, stats<tag::count, tag::mean > > acc;
	for (size_t i=0;i<data.size();++i)
	{
		acc(curve_sign*data.at(i).anp(in_speed));
	};
	return boost::accumulators::extract::count(acc)>0 ? mean(acc) : 0;
};

/// Возвращает максимальное значение Анп для заданной скорости
VPoint VCurveCalc::GetMaxAnp(int in_speed) const
{
	VPoint anp_max(0,0);
	if (in_speed!=NO_SPEED)
	{
		for (size_t i=0; i<data.size(); ++i)
		{
			
			VPoint anp(data.at(i).coord(), curve_sign*data.at(i).anp(in_speed));
			if (anp_max.GetValue() < anp.GetValue())
				anp_max=anp;
			
		}
	}
	return anp_max;
};

/// Возвращает максимальное значение Пси для заданной скорости
VPoint VCurveCalc::GetMaxPsi(int in_speed) const
{	
	std::deque<DATA> psi_base;
	VPoint psi_max(0,0);
	if (in_speed!=NO_SPEED)
	{
		for (size_t i=0; i<data.size(); ++i)
		{
			psi_base.push_back(data.at(i));
			if (fabs(psi_base.front().coord()-psi_base.back().coord())>OPTIONS.PSI_BASE(in_speed))
			{
				VPoint psi((psi_base.front().coord() + psi_base.back().coord())/2, 
					fabs(calc_psi(in_speed, psi_base.front().anp(in_speed), psi_base.back().anp(in_speed), 0, OPTIONS.PSI_BASE(in_speed))));
				psi_base.pop_front();
				if (psi_max.GetValue() < psi.GetValue())
					psi_max=psi;
			}
		}
	}
	return psi_max;
};

/// Возвращает начало участка отступления
double VCurveCalc::GetStart() const
{
	return data.front().coord();
}

/// Возвращает конец участка
double VCurveCalc::GetEnd() const
{
	return data.back().coord();
}

/// Очищает массив данных
void VCurveCalc::Clear()
{
	data.clear();
}

/// Возвращает число кадров данных
size_t VCurveCalc::GetCount() const
{
	return data.size();
}

/// Возвращает Пси на всем участке
double VCurveCalc::GetPsi(int in_speed) const
{
	return in_speed!=NO_SPEED ? fabs(calc_psi(in_speed, data.front().anp(in_speed), data.back().anp(in_speed), data.front().coord(), data.back().coord())) : 0.0;
}

//-------------------------------- Методы класса VAnpAverageOverstep ------------------------------------

/// Конструктор
VAnpAverageOverstep::VAnpAverageOverstep(const VCurveCalc& in_calc)
{
	start=in_calc.GetStart();
	end=in_calc.GetEnd();
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	speeds=SITUATION.GetCurveSpeeds(GetCoord());
	max_value=0;
	min_value=0;
	value=0;
	length=ceil(fabs(end-start));
	type=ANP_AVERAGE;
	degree=DEGREE_NONE;
	restriction=speeds;
	
	while (restriction.GetHighBTSpeed()>0 && VAnpAverageHighBTCondition(true).TestValue(fabs(in_calc.GetAverageAnp(restriction.GetHighBTSpeed()))))
	{
		restriction.DecreaseHighBTSpeed(SPEED_QUANTUM);
	}				
	while (restriction.GetHighSpeed()>0 && VAnpAverageHighCondition(true).TestValue(fabs(in_calc.GetAverageAnp(restriction.GetHighSpeed()))))
	{
		restriction.DecreaseHighSpeed(SPEED_QUANTUM);
	}
	while (restriction.GetPassSpeed()>0 && VAnpAveragePassCondition(true).TestValue(fabs(in_calc.GetAverageAnp(restriction.GetPassSpeed()))))
	{
		restriction.DecreasePassSpeed(SPEED_QUANTUM);
	}				
	while (restriction.GetTruckSpeed()>0 && VAnpAverageTruckCondition(true).TestValue(fabs(in_calc.GetAverageAnp(restriction.GetTruckSpeed()))))
	{
		restriction.DecreaseTruckSpeed(SPEED_QUANTUM);
	}
	
	anp_high_bt=in_calc.GetAverageAnp(speeds.GetHighBTSpeed());
	anp_high=in_calc.GetAverageAnp(speeds.GetHighSpeed());
	anp_pass=in_calc.GetAverageAnp(speeds.GetPassSpeed());
	anp_truck=in_calc.GetAverageAnp(speeds.GetTruckSpeed());
}

/// Возвращает строку с описанием степени
std::wstring VAnpAverageOverstep::GetDegreeString() const
{	
	return L"";
}

/// Возвращает строку с описанием значения
std::wstring VAnpAverageOverstep::GetValueString() const
{	
	std::wstringstream msg;
	msg.precision(ANP_PRECISION);
	msg << fixed;
	if (speeds.GetHighBTSpeed().Valid() || speeds.GetHighSpeed().Valid())
	{
		if (VAnpAverageHighBTCondition(false).TestValue(anp_high_bt))
			msg << anp_high_bt << L"/";
		else
			msg << L"-/";
		if (VAnpAverageHighCondition(false).TestValue(anp_high))
			msg << anp_high << L"/";
		else
			msg << L"-/";
	}
	if (VAnpAveragePassCondition(false).TestValue(anp_pass))
		msg << anp_pass << L"/";
	else
		msg << L"-/";
	if (VAnpAverageTruckCondition(false).TestValue(anp_truck))
		msg << anp_truck;
	else
		msg << L"-";
	
	return msg.str();
}

/// Возвращает строку с описанием длины
std::wstring VAnpAverageOverstep::GetLengthString() const
{
	return L"";
}

/// Возвращает строку с описанием баллов
std::wstring VAnpAverageOverstep::GetPenaltyString() const
{
	return L"";
}


/// штраф
int VAnpAverageOverstep::GetPenalty() const
{
	return this->HasRestriction() ? 50 : 0;
}


//-------------------------------- Методы класса VAnpHorizontalOverstep ------------------------------------

/// Конструктор
VAnpHorizontalOverstep::VAnpHorizontalOverstep(const VCurveCalc& in_calc)
{
	start=in_calc.GetStart();
	end=in_calc.GetEnd();
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	speeds=SITUATION.GetCurveSpeeds(GetCoord());
	max_value=0;
	min_value=0;
	value=0;
	length=ceil(fabs(end-start));
	type=ANP_HORIZONTAL;
	degree=DEGREE_NONE;
	restriction=speeds;
	
	while (restriction.GetHighBTSpeed()>0 && VAnpHighBTCondition(true).TestValue(fabs(in_calc.GetMaxAnp(restriction.GetHighBTSpeed()).GetValue())))
	{
		restriction.DecreaseHighBTSpeed(SPEED_QUANTUM);
	}				
	while (restriction.GetHighSpeed()>0 && VAnpHighCondition(true).TestValue(fabs(in_calc.GetMaxAnp(restriction.GetHighSpeed()).GetValue())))
	{
		restriction.DecreaseHighSpeed(SPEED_QUANTUM);
	}
	while (restriction.GetPassSpeed()>0 && VAnpPassCondition(true).TestValue(fabs(in_calc.GetMaxAnp(restriction.GetPassSpeed()).GetValue())))
	{
		restriction.DecreasePassSpeed(SPEED_QUANTUM);
	}				
	while (restriction.GetTruckSpeed()>0 && VAnpTruckCondition(true).TestValue(fabs(in_calc.GetMaxAnp(restriction.GetTruckSpeed()).GetValue())))
	{
		restriction.DecreaseTruckSpeed(SPEED_QUANTUM);
	}
	anp_high_bt=fabs(in_calc.GetMaxAnp(speeds.GetHighBTSpeed()).GetValue());
	anp_high=fabs(in_calc.GetMaxAnp(speeds.GetHighSpeed()).GetValue());
	anp_pass=fabs(in_calc.GetMaxAnp(speeds.GetPassSpeed()).GetValue());
	anp_truck=fabs(in_calc.GetMaxAnp(speeds.GetTruckSpeed()).GetValue());
};

/// Возвращает строку с описанием степени
std::wstring VAnpHorizontalOverstep::GetDegreeString() const
{	
	return L"";
}

/// Возвращает строку с описанием значения
std::wstring VAnpHorizontalOverstep::GetValueString() const
{	
	std::wstringstream msg;
	msg.precision(ANP_PRECISION);
	msg << fixed;
	if (speeds.GetHighBTSpeed().Valid() || speeds.GetHighSpeed().Valid())
	{
		if (VAnpHighBTCondition(false).TestValue(anp_high_bt))
			msg << anp_high_bt << L"/";
		else
			msg << L"-/";
		if (VAnpHighCondition(false).TestValue(anp_high))
			msg << anp_high << L"/";
		else
			msg << L"-/";
	}
	if (VAnpPassCondition(false).TestValue(anp_pass))
		msg << anp_pass << L"/";
	else
		msg << L"-/";
	if (VAnpTruckCondition(false).TestValue(anp_truck))
		msg << anp_truck;
	else
		msg << L"-";
	
	return msg.str();
}

/// Возвращает строку с описанием длины
std::wstring VAnpHorizontalOverstep::GetLengthString() const
{
	return L"";
}

/// Возвращает строку с описанием баллов
std::wstring VAnpHorizontalOverstep::GetPenaltyString() const
{
	return L"";
}


/// штраф
int VAnpHorizontalOverstep::GetPenalty() const
{
	return this->HasRestriction() ? 50 : 0;
}


//---------------------------------- Методы класса VPsiOverstep ---------------------------------------------------

/// Конструктор
VPsiOverstep::VPsiOverstep(const VCurveCalc& in_calc)
{
	start=in_calc.GetStart();
	end=in_calc.GetEnd();
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	speeds=SITUATION.GetCurveSpeeds(GetCoord());
	max_value=0;
	min_value=0;
	value=0;
	length=ceil(fabs(end-start));
	type=PSI_HORIZONTAL;
	degree=DEGREE_NONE;
	restriction=speeds;
	
	while (restriction.GetHighBTSpeed()>0 && VPsiBTCondition(true).TestValue(fabs(in_calc.GetMaxPsi(restriction.GetHighBTSpeed()).GetValue())))
	{
		restriction.DecreaseHighBTSpeed(SPEED_QUANTUM);
	}				
	while (restriction.GetHighSpeed()>0 && VPsiCondition(true).TestValue(fabs(in_calc.GetMaxPsi(restriction.GetHighSpeed()).GetValue())))
	{
		restriction.DecreaseHighSpeed(SPEED_QUANTUM);
	}
	while (restriction.GetPassSpeed()>0 && VPsiCondition(true).TestValue(fabs(in_calc.GetMaxPsi(restriction.GetPassSpeed()).GetValue())))
	{
		restriction.DecreasePassSpeed(SPEED_QUANTUM);
	}				
	while (restriction.GetTruckSpeed()>0 && VPsiCondition(true).TestValue(fabs(in_calc.GetMaxPsi(restriction.GetTruckSpeed()).GetValue())))
	{
		restriction.DecreaseTruckSpeed(SPEED_QUANTUM);
	}
	psi_high_bt=fabs(in_calc.GetMaxPsi(speeds.GetHighBTSpeed()).GetValue());
	psi_high=fabs(in_calc.GetMaxPsi(speeds.GetHighSpeed()).GetValue());
	psi_pass=fabs(in_calc.GetMaxPsi(speeds.GetPassSpeed()).GetValue());
	psi_truck=fabs(in_calc.GetMaxPsi(speeds.GetTruckSpeed()).GetValue());
}

/// Возвращает строку с описанием степени
std::wstring VPsiOverstep::GetDegreeString() const
{
	return L"";
}

/// Возвращает строку с описанием значения
std::wstring VPsiOverstep::GetValueString() const
{
	std::wstringstream msg;
	msg.precision(PSI_PRECISION);
	msg << fixed;
	if (speeds.GetHighBTSpeed().Valid() || speeds.GetHighSpeed().Valid())
	{
		if (VPsiBTCondition(false).TestValue(psi_high_bt))
			msg << psi_high_bt << L"/";
		else
			msg << L"-/";
		if (VPsiCondition(false).TestValue(psi_high))
			msg << psi_high << L"/";
		else
			msg << L"-/";
	}
	if (VPsiCondition(false).TestValue(psi_pass))
		msg << psi_pass << L"/";
	else
		msg << L"-/";
	if (VPsiCondition(false).TestValue(psi_truck))
		msg << psi_truck;
	else
		msg << L"-";
	
	return msg.str();
}

/// Возвращает строку с описанием длины
std::wstring VPsiOverstep::GetLengthString() const
{
	return L"";
}

/// Возвращает строку с описанием баллов
std::wstring VPsiOverstep::GetPenaltyString() const
{
	return L"";
}

/// Возвращает балловую оценку
int VPsiOverstep::GetPenalty() const
{
	return this->HasRestriction() ? 50 : 0;
}

//---------------------------------- Методы класса VPsiTransitionOverstep ---------------------------------------------------

/// Конструктор
VPsiTransitionOverstep::VPsiTransitionOverstep(const VCurveCalc& in_calc)
{
	start=in_calc.GetStart();
	end=in_calc.GetEnd();
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	speeds=SITUATION.GetCurveSpeeds(GetCoord());
	max_value=0;
	min_value=0;
	value=0;
	length=ceil(fabs(end-start));
	type=PSI_TRANSITION;
	degree=DEGREE_NONE;
	restriction=speeds;
	
	while (restriction.GetHighBTSpeed()>0 && VPsiBTCondition(true).TestValue(fabs(in_calc.GetPsi(restriction.GetHighBTSpeed()))))
	{
		restriction.DecreaseHighBTSpeed(SPEED_QUANTUM);
	}				
	while (restriction.GetHighSpeed()>0 && VPsiCondition(true).TestValue(fabs(in_calc.GetPsi(restriction.GetHighSpeed()))))
	{
		restriction.DecreaseHighSpeed(SPEED_QUANTUM);
	}
	while (restriction.GetPassSpeed()>0 && VPsiCondition(true).TestValue(fabs(in_calc.GetPsi(restriction.GetPassSpeed()))))
	{
		restriction.DecreasePassSpeed(SPEED_QUANTUM);
	}				
	while (restriction.GetTruckSpeed()>0 && VPsiCondition(true).TestValue(fabs(in_calc.GetPsi(restriction.GetTruckSpeed()))))
	{
		restriction.DecreaseTruckSpeed(SPEED_QUANTUM);
	}
	psi_high_bt=fabs(in_calc.GetPsi(speeds.GetHighBTSpeed()));
	psi_high=fabs(in_calc.GetPsi(speeds.GetHighSpeed()));
	psi_pass=fabs(in_calc.GetPsi(speeds.GetPassSpeed()));
	psi_truck=fabs(in_calc.GetPsi(speeds.GetTruckSpeed()));
}

/// Возвращает строку с описанием степени
std::wstring VPsiTransitionOverstep::GetDegreeString() const
{
	return L"";
}

/// Возвращает строку с описанием значения
std::wstring VPsiTransitionOverstep::GetValueString() const
{
	std::wstringstream msg;
	msg.precision(PSI_PRECISION);
	msg << fixed;
	if (speeds.GetHighBTSpeed().Valid() || speeds.GetHighSpeed().Valid())
	{
		if (VPsiBTCondition(false).TestValue(psi_high_bt))
			msg << psi_high_bt << L"/";
		else
			msg << L"-/";
		if (VPsiCondition(false).TestValue(psi_high))
			msg << psi_high << L"/";
		else
			msg << L"-/";
	}
	if (VPsiCondition(false).TestValue(psi_pass))
		msg << psi_pass << L"/";
	else
		msg << L"-/";
	if (VPsiCondition(false).TestValue(psi_truck))
		msg << psi_truck;
	else
		msg << L"-";
	
	return msg.str();
}

/// Возвращает строку с описанием длины
std::wstring VPsiTransitionOverstep::GetLengthString() const
{
	return L"";
}

/// Возвращает строку с описанием баллов
std::wstring VPsiTransitionOverstep::GetPenaltyString() const
{
	return L"";
}

/// Возвращает балловую оценку
int VPsiTransitionOverstep::GetPenalty() const
{
	return this->HasRestriction() ? 50 : 0;
}

//--------------------------------- Методы класса VZazorExpress -------------------------------------------

/// Конструктор
VZazorExpress::VZazorExpress(const VPoint& in_point, side_t in_side)
{
	start=in_point.GetCoord();
	end=in_point.GetCoord();
	if (COORD_CORRECTION_FUNCTION().Value().Valid())
	{
		start=COORD_CORRECTION_FUNCTION().Value().Correct(start);
		end=COORD_CORRECTION_FUNCTION().Value().Correct(end);		
	}
	else
	{
		AddComment(INACCURATE_COORDINATE_SIGN);
	}
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	max_value=round(in_point.GetValue());
	min_value=round(in_point.GetValue());
	value=round(in_point.GetValue());
	length=0;
	side=in_side;
	type=(side==SD_LEFT) ? ZAZOR_LEFT : ZAZOR_RIGHT;
	degree=DEGREE_NONE;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	restriction=VSpeeds(0,0,0);	
}


//--------------------------------- Методы класса VZazorOverstep -------------------------------------

/// Конструктор
VZazorOverstep::VZazorOverstep(const VPoint& in_point, double in_temp, side_t in_side)
{
	start=in_point.GetCoord();
	end=in_point.GetCoord();
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	max_value=round(in_point.GetValue());
	min_value=round(in_point.GetValue());
	value=round(in_point.GetValue());
	length=0;
	type=in_side==SD_LEFT ? ZAZOR_LEFT : ZAZOR_RIGHT;
	degree=DEGREE_NONE;	
	temp=in_temp;
	side=in_side;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());
	diameter=SITUATION.GetHoleDiameter(GetCoord());
	restriction=speeds;			
	//Ограничение скорости
	while (restriction.GetMaxSpeed()>0 && VZazorCondition(restriction, diameter).TestValue(value))
	{
		restriction.DecreaseSpeed(SPEED_QUANTUM);
	}				
	AddComment(boost::str(boost::wformat(L"%0.0f°C ") % in_temp));		
	if (diameter==40)
		AddComment(boost::str(boost::wformat(L"[%0.0fмм]") % diameter));
}

/// Возвращает строку с описанием длины
std::wstring VZazorOverstep::GetLengthString() const
{
	return L"";
}

//------------------------------------ Методы класса VZeroZazorOverstep --------------------------------------

/// Конструктор
VZeroZazorOverstep::VZeroZazorOverstep(double in_start, double in_end, double in_count, side_t in_side)
{
	start=in_start;
	end=in_end;
	waycoord=SITUATION.GetRealCoord((start+end)/2);
	max_value=0;
	min_value=0;
	value=in_count;
	length=ceil(fabs(in_start-in_end));
	type=in_side==SD_LEFT ? ZERO_ZAZOR_LEFT : ZERO_ZAZOR_RIGHT;
	degree=DEGREE_NONE;	
	side=in_side;
	speeds=SITUATION.GetCombinedSpeeds(GetCoord());	
	restriction=speeds;	
}

//------------------------------ Методы класса VFindExcess ---------------------------------------------

/// Конструктор
VFindExcess::VFindExcess(){}

/// Помещает на вход значение
void VFindExcess::Input(const VPoint& in_point, const VOverstepCondition& in_condition)
{
	if (in_condition.TestValue(in_point.GetValue(), input.empty() ? 0 : fabs(input.front().GetCoord()-in_point.GetCoord())))
	{
		input.push_back(in_point);
	}
	else
	{
		Flush();
	}
}
/// Возвращает признак наличия данных
bool VFindExcess::Ready() const
{
	return output.size()>0;
}
/// Возвращает признак пустого списка
bool VFindExcess::Empty() const
{
	return output.empty();
}
/// Возвращает обнаруженное отступление
const VExcess& VFindExcess::Front() const
{
	return output.front();
}
/// Удаляет отступление из очереди
void VFindExcess::Pop()
{
	output.pop_front();
}
/// Сбрасывает текущее отсупление в выходной буфер
void VFindExcess::Flush()
{
	if (!input.empty())
	{
		VExcess excess(input);				
		output.push_back(excess);
		input.clear();
	}
}

//---------------------------------- Методы класса VFindWave -------------------------------------

/// Конструктор
VFindWave::VFindWave() : resolution_x(DBL_MAX), resolution_y(DBL_MAX) {}

/// Конструктор
VFindWave::VFindWave(double in_resolution_x, double in_resolution_y) : resolution_x(in_resolution_x), resolution_y(in_resolution_y) {}

/// Деструктор
VFindWave::~VFindWave() 
{
	if (!Empty()) 
		LOG_ERROR(L"VFindWave: Необработанные данные...");
};

/// Добавляет отступление
void VFindWave::Put(const VExtremum& in_point)
{
	input.push_back(in_point);
	if (input.size()==2)
	{
		VWave wave(input.front(),input.back());
		output.push(wave);
		input.pop_front();
	}
}

/// Возвращает признак наличия данных
bool VFindWave::Ready() const
{
	return output.size()>0;
}

/// Возвращает признак пустого списка
bool VFindWave::Empty() const
{
	return output.empty();
}

/// Возвращает обнаруженное отступление
const VWave& VFindWave::Front() const
{
	return output.front();
}

/// Удаляет отступление из очереди
void VFindWave::Pop()
{
	output.pop();
}

//--------------------------------------- Методы класса VWaveFilter ---------------------------------------

/// Конструктор
VWaveFilter::VWaveFilter(){};
/// Деструктор
VWaveFilter::~VWaveFilter(){};
/// Помещает данные на вход
void VWaveFilter::Put(const VWave& in_wave)
{
	if (!(fabs(in_wave.GetValue())<6.0 || fabs(in_wave.GetValue())<8.0 && pro::interval<double>(6.0,DISTORTION_MAX_LENGTH).contains(in_wave.GetLength())))
	{
		output.push_back(in_wave);
	}
};
/// Возвращает признак наличия данных
bool VWaveFilter::Ready() const
{
	return output.size()>0;
};
/// Возвращает признак пустого списка
bool VWaveFilter::Empty() const
{
	return output.empty();
};
/// Возвращает обнаруженное отступление
const VWave& VWaveFilter::Front() const
{
	return output.front();
};	
/// Удаляет данные из очереди
void VWaveFilter::Pop()
{
	output.pop_front();
};

//------------------------------------------------ Методы класса VWaveUnion --------------------------------------------------------

/// Конструктор
VWaveUnion::VWaveUnion(double in_union_distance) : union_distance(in_union_distance){}

/// Деструктор
VWaveUnion::~VWaveUnion(){}

/// Помещает данные на вход
void VWaveUnion::Put(const VWave& in_wave)
{
	input.push_back(in_wave);
	if (input.size()==2)
	{
		if (input.front().GetSign()==input.back().GetSign() &&
		    fabs(input.front().GetStart().GetCoord()-input.back().GetEnd().GetCoord())<union_distance)
		{
			output.push_back(VWave(input.front().GetStart(), input.back().GetEnd()));
			input.clear();
		}
		else
			transfer<VWave>(input,output);
	}
}

/// Возвращает признак наличия данных
bool VWaveUnion::Ready() const
{
	return output.size()>0;
}

/// Возвращает признак пустого списка
bool VWaveUnion::Empty() const
{
	return output.empty();
}

/// Возвращает обнаруженное отступление
const VWave& VWaveUnion::Front() const
{
	return output.front();
}
/// Удаляет отступление из очереди
void VWaveUnion::Pop()
{
	output.pop_front();
}

/// Слив данных
void VWaveUnion::Flush()
{
	while (!input.empty())
	{
		transfer<VWave>(input, output);
	}
}

//------------------------------------ Методы класса VWaveSelector ------------------------------------

/// Конструктор
VWaveSelector::VWaveSelector() {};

/// Деструктор
VWaveSelector::~VWaveSelector() {};

/// Помещает на вход значение
void VWaveSelector::Put(const VBasicOverstepPtr& in_overstep) 
{
	if (!input.empty() && input.back()->Distance(*in_overstep)>1.0)
	{
		Flush();
	}
	input.push_back(in_overstep);
};

/// Возвращает признак наличия данных
bool VWaveSelector::Ready() const
{
	return output.size()>0;
}

/// Возвращает признак пустого списка
bool VWaveSelector::Empty() const
{
	return output.empty();
}

/// Возвращает обнаруженное отступление
const VBasicOverstepPtr& VWaveSelector::Front() const
{
	return output.front();
}
	
/// Удаляет отступление из очереди
void VWaveSelector::Pop()
{
	output.pop_front();
}

/// Слив данных
void VWaveSelector::Flush()
{
	if (!input.empty())
	{
		std::deque<VBasicOverstepPtr>::iterator worst=std::max_element(input.begin(), input.end(), worst_overstep());
		for (std::deque<VBasicOverstepPtr>::iterator i=input.begin(); i!=input.end(); ++i)
		{
			if ((i-worst)%2 == 0)
				output.push_back(*i);
		}		
		input.clear();
	}
}

//--------------------------------------- Методы класса VCheckPit ------------------------------------------

/// Конструктор
VCheckPit::VCheckPit(side_t in_side) : side(in_side) {}

/// Деструктор
VCheckPit::~VCheckPit() 
{
	if (!Empty()) 
		LOG_ERROR(L"VCheckPit: Необработанные данные...");
};

/// Помещает на вход значение
void VCheckPit::Put(const VWave& in_wave)
{
	//Пропускаем отступления, на участке которых есть сбои
	if (in_wave.GetCorrupt())
		return;
	if (in_wave.GetLength()>PIT_MAX_LENGTH)
		return;	
	VSpeeds speeds=SITUATION.GetCombinedSpeeds(in_wave.GetStart().GetCoord());
	bool bridge=SITUATION.GetBridge(in_wave.GetStart().GetCoord()) || SITUATION.GetTonnel(in_wave.GetStart().GetCoord());
	

	double length=ceil(std::min<double>(PIT_LENGTH, in_wave.GetLength()));
	double value=in_wave.GetLength()>PIT_LENGTH ? round(in_wave.GetValue(PIT_LENGTH)) : round(in_wave.GetValue());
	
	
	if (VPitCondition(DEGREE4, speeds, bridge).TestValue(value, length))
	{
		output.push_back(VBasicOverstepPtr(new VPitOverstep(in_wave, value, length, side, DEGREE4)));		
	}
	else if (VPitCondition(DEGREE3, speeds, bridge).TestValue(value, length))
	{
		output.push_back(VBasicOverstepPtr(new VPitOverstep(in_wave, value, length, side, DEGREE3)));
	}
	else if (VPitCondition(DEGREE2, speeds, bridge).TestValue(value, length))
	{
		output.push_back(VBasicOverstepPtr(new VPitOverstep(in_wave, value, length, side, DEGREE2)));				
	}
	else if (VPitCondition(DEGREE1, speeds, bridge).TestValue(value, length))
	{
		output.push_back(VBasicOverstepPtr(new VPitOverstep(in_wave, value, length, side, DEGREE1)));				
	}		
}

/// Возвращает признак наличия данных
bool VCheckPit::Ready() const
{
	return output.size()>0;
}

/// Возвращает признак пустого списка
bool VCheckPit::Empty() const
{
	return output.empty();
}

/// Возвращает обнаруженное отступление
const VBasicOverstepPtr& VCheckPit::Front() const
{
	return output.front();
}

/// Удаляет отступление из очереди
void VCheckPit::Pop()
{
	output.pop_front();
}


//------------------------------------- Методы класса VPitGroupDetector ----------------------------------


/// Конструктор
VPitGroupDetector::VPitGroupDetector(side_t in_side) : side(in_side){}

/// Деструктор
VPitGroupDetector::~VPitGroupDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VPitGroupDetector: Необработанные данные...");
};

/// Помещает очередное отступление
void VPitGroupDetector::Put(const VBasicOverstepPtr& in_overstep)
{	
	if (!in_overstep->IsExcluded())
	{
		if ((side==SD_LEFT && in_overstep->IsType(PIT_LEFT) || side==SD_RIGHT && in_overstep->IsType(PIT_RIGHT)) && VPitGroupCondition()(in_overstep))
		{	
			if	(input.empty() || input.back()->Distance(*in_overstep)<PIT_GROUP_DISTANCE)
			{
				input.push_back(in_overstep);
			}
			else
			{			
				Flush();		
				input.push_back(in_overstep);
			}
		}
	}
}

/// Помещает значение текущей позиции
void VPitGroupDetector::Put(const double& in_position)
{
	if (!input.empty() && fabs(in_position-input.back()->GetCoord())>PIT_DECISION_DISTANCE)
	{
		Flush();
	}
}

/// Возвращает true, если есть готовые отступления
bool VPitGroupDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VPitGroupDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VPitGroupDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VPitGroupDetector::Pop()
{
	output.pop_front();
}

/// Сброс буфера
void VPitGroupDetector::Flush()
{
	if (input.size()>=PERIODICAL_COUNT)
	{			
		//FixMe!!! учесть, что отступления не должны отличаться по длине более чем на 20%
		VPitGroupOverstepPtr group(new VPitGroupOverstep(input, side));
		output.push_back(group);		
	}
	input.clear();
}


//------------------------------------- Методы класса VRichGroupDetector ----------------------------------

/// Конструктор
VRichGroupDetector::VRichGroupDetector(side_t in_side) : side(in_side){}

/// Помещает очередное отступление
void VRichGroupDetector::Put(const VBasicOverstepPtr& in_overstep)
{	
	if (!in_overstep->IsExcluded())
	{
		if ((side==SD_LEFT && in_overstep->IsType(RICH_LEFT) || side==SD_RIGHT && in_overstep->IsType(RICH_RIGHT)) && VRichGroupCondition()(in_overstep))
		{
			if (input.empty() || input.back()->Distance(*in_overstep)<RICH_GROUP_DISTANCE)
			{
				input.push_back(in_overstep);
			}
			else
			{			
				Flush();		
				input.push_back(in_overstep);
			}
		}
	}
}

/// Деструктор
VRichGroupDetector::~VRichGroupDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VRichGroupDetector: Необработанные данные...");
};

/// Помещает значение текущей позиции
void VRichGroupDetector::Put(const double& in_position)
{
	if (!input.empty() && fabs(in_position-input.back()->GetCoord())>RICH_DECISION_DISTANCE)
	{
		Flush();
	}
}

/// Возвращает true, если есть готовые отступления
bool VRichGroupDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VRichGroupDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VRichGroupDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VRichGroupDetector::Pop()
{
	output.pop_front();
}

/// Сброс буфера
void VRichGroupDetector::Flush()
{
	if (input.size()>=PERIODICAL_COUNT)
	{			
		//FixMe!!! учесть, что отступления не должны отличаться по длине более чем на 20%
		VRichGroupOverstepPtr group(new VRichGroupOverstep(input, side));
		output.push_back(group);		
	}
	input.clear();
}


//------------------------------------- Методы класса VCheckDistortion -------------------------------------

/// Конструктор
VCheckDistortion::VCheckDistortion(){}

/// Деструктор
VCheckDistortion::~VCheckDistortion() 
{
	if (!Empty()) LOG_ERROR(L"VCheckDistortion: Необработанные данные...");
};

/// Помещает на вход значение
void VCheckDistortion::Put(const VWave& in_wave)
{
	//Пропускаем отступления, на участке которых есть сбои
	if (in_wave.GetCorrupt())
		return;
	if (NEW_INSTRUCTION && in_wave.GetLength()>DISTORTION_MAX_LENGTH || in_wave.GetLength()>DISTORTION_LENGTH)
		return;
	VSpeeds speeds=SITUATION.GetCombinedSpeeds(in_wave.GetStart().GetCoord());
	bool bridge=SITUATION.GetBridge(in_wave.GetStart().GetCoord()) || SITUATION.GetTonnel(in_wave.GetStart().GetCoord());	

	double length=ceil(std::min<double>(DISTORTION_LENGTH, in_wave.GetLength()));
	double value=in_wave.GetLength()>DISTORTION_LENGTH ? round(in_wave.GetValue(DISTORTION_LENGTH)) : round(in_wave.GetValue());
	
	if (VDistortionCondition(DEGREE4, speeds, bridge).TestValue(value, length))
	{
		output.push_back(VBasicOverstepPtr(new VDistortionOverstep(in_wave, value, length, DEGREE4)));		
	}
	else if (VDistortionCondition(DEGREE3, speeds, bridge).TestValue(value, length))
	{
		output.push_back(VBasicOverstepPtr(new VDistortionOverstep(in_wave, value, length, DEGREE3)));
	}
	else if (VDistortionCondition(DEGREE2, speeds, bridge).TestValue(value, length))
	{
		output.push_back(VBasicOverstepPtr(new VDistortionOverstep(in_wave, value, length, DEGREE2)));				
	}
	else if (VDistortionCondition(DEGREE1, speeds, bridge).TestValue(value, length))
	{
		output.push_back(VBasicOverstepPtr(new VDistortionOverstep(in_wave, value, length, DEGREE1)));				
	}			
}

/// Возвращает признак наличия данных
bool VCheckDistortion::Ready() const
{
	return output.size()>0;
}

/// Возвращает признак пустого списка
bool VCheckDistortion::Empty() const
{
	return output.empty();
}

/// Возвращает обнаруженное отступление
const VBasicOverstepPtr& VCheckDistortion::Front() const
{
	return output.front();
}

/// Удаляет отступление из очереди
void VCheckDistortion::Pop()
{
	output.pop_front();
}

//------------------------------------- Методы класса VDistortionGroupDetector ----------------------------------

/// Конструктор
VDistortionGroupDetector::VDistortionGroupDetector(){}

/// Деструктор
VDistortionGroupDetector::~VDistortionGroupDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VDistortionGroupDetector: Необработанные данные...");
};

/// Помещает очередное отступление
void VDistortionGroupDetector::Put(const VBasicOverstepPtr& in_overstep)
{	
	if (!in_overstep->IsExcluded())
	{
		if (in_overstep->IsType(DISTORTION) && VDistortionGroupCondition()(in_overstep))
		{
			if (input.empty() || input.back()->Distance(*in_overstep)<DISTORTION_GROUP_DISTANCE)
			{
				input.push_back(in_overstep);
			}
			else
			{			
				Flush();		
				input.push_back(in_overstep);
			}
		}
	}
}

/// Помещает значение текущей позиции
void VDistortionGroupDetector::Put(const double& in_position)
{
	if (!input.empty() && fabs(in_position-input.back()->GetCoord())>DISTORTION_DECISION_DISTANCE)
	{
		Flush();
	}
}

/// Возвращает true, если есть готовые отступления
bool VDistortionGroupDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VDistortionGroupDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VDistortionGroupDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VDistortionGroupDetector::Pop()
{
	output.pop_front();
}

/// Сброс буфера
void VDistortionGroupDetector::Flush()
{
	if (input.size()>=PERIODICAL_COUNT)
	{			
		//FixMe!!! учесть, что отступления не должны отличаться по длине более чем на 20%
		VDistortionGroupOverstepPtr group(new VDistortionGroupOverstep(input));
		output.push_back(group);		
	}
	input.clear();
}


//------------------------------------- Методы класса VCheckRich -------------------------------------

/// Конструктор
VCheckRich::VCheckRich(side_t in_side) : side(in_side)
{
	if (OLD_INSTRUCTION)
	{
		RichCorrector=VRichCorrectorPtr(new VRichPalet());		
	}
	else
	{
		RichCorrector=VRichCorrectorPtr(new VRichCorrectionTable());
	}	
}

/// Деструктор
VCheckRich::~VCheckRich() 
{
	if (!Empty()) LOG_ERROR(L"VCheckRich: Необработанные данные...");
};

/// Помещает на вход значение
void VCheckRich::Put(const VWave& in_wave)
{
	//Пропускаем отступления, на участке которых есть сбои
	if (in_wave.GetCorrupt())
		return;
	if (in_wave.GetLength()>RICH_MAX_LENGTH)
		return;	
		
	VSpeeds speeds=SITUATION.GetCombinedSpeeds(in_wave.GetStart().GetCoord());
	bool bridge=SITUATION.GetBridge(in_wave.GetStart().GetCoord()) || SITUATION.GetTonnel(in_wave.GetStart().GetCoord());
	bool thread_side=SITUATION.GetSummaryThreadSide(in_wave.GetStart().GetCoord())==side;
	bool curve=SITUATION.IsCurvePlan(in_wave.GetStart().GetCoord());
	
	//FixMe!!! Алгоритм оценки рихтовки упрощен. Не вычисляются и не оцениваются доп. точки
	
	/// Длинна рихтовки - расстояние между пиками умноженное на 2
	double length=ceil(in_wave.GetLength()*2);
	
	double value=in_wave.GetValue();
	
	if (VRichCondition(DEGREE4, speeds, bridge, thread_side, curve).TestValue(RichCorrector->Correct(DEGREE4, speeds.GetMaxSpeed(), value, length), length))
	{
		output.push_back(VBasicOverstepPtr(new VRichOverstep(in_wave, RichCorrector->Correct(DEGREE4, speeds.GetMaxSpeed(), value, length), length, side, DEGREE4)));		
	}
	else if (VRichCondition(DEGREE3, speeds, bridge, thread_side, curve).TestValue(RichCorrector->Correct(DEGREE3, speeds.GetMaxSpeed(), value, length), length))
	{
		output.push_back(VBasicOverstepPtr(new VRichOverstep(in_wave, RichCorrector->Correct(DEGREE3, speeds.GetMaxSpeed(), value, length), length, side, DEGREE3)));		
	}
	else if (VRichCondition(DEGREE2, speeds, bridge, thread_side, curve).TestValue(RichCorrector->Correct(DEGREE2, speeds.GetMaxSpeed(), value, length), length))
	{
		output.push_back(VBasicOverstepPtr(new VRichOverstep(in_wave, RichCorrector->Correct(DEGREE2, speeds.GetMaxSpeed(), value, length), length, side, DEGREE2)));		
	}
	/*else if (VRichCondition(DEGREE1, speeds, bridge, thread_side, curve).TestValue(RichCorrector->Correct(DEGREE1, speeds, value), length))
	{
		output.push_back(VBasicOverstepPtr(new VRichOverstep(in_wave, RichCorrector->Correct(DEGREE1, speeds, value), length, side, DEGREE1)));		
	}*/
}

/// Возвращает признак наличия данных
bool VCheckRich::Ready() const
{
	return output.size()>0;
}

/// Возвращает признак пустого списка
bool VCheckRich::Empty() const
{
	return output.empty();
}

/// Возвращает обнаруженное отступление
const VBasicOverstepPtr& VCheckRich::Front() const
{
	return output.front();
}

/// Удаляет отступление из очереди
void VCheckRich::Pop()
{
	output.pop_front();
}

//----------------------------------- Методы класса VRichAndDistortionDetector ----------------------------------

/// Конструктор
VRichAndDistortionDetector::VRichAndDistortionDetector(){}

/// Деструктор
VRichAndDistortionDetector::~VRichAndDistortionDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VRichAndDistortionDetector: Необработанные данные...");
};

/// Помещает очередное отступление
void VRichAndDistortionDetector::Put(const VBasicOverstepPtr& in_overstep)
{
	if (!in_overstep->IsExcluded())
	{
		if (!input.empty())
		{
			std::deque<VBasicOverstepPtr>::iterator i = std::find_if(input.begin(), input.end(), VRichAndDistortionCondition(in_overstep));
			if (i!=input.end())
			{			
				VBasicOverstepPtr overstep(new VRichAndDistortionCombination(*i, in_overstep));
				if (overstep->HasRestriction())
				{				
					output.push(overstep);						
				}
				input.erase(i);
			}
			else
			{
				//Сочетания оцениваются только для путей с установленной скоростью более 60
				if (in_overstep->GetSpeeds().GetMaxSpeed()>MIN_COMBINATION_SPEED)
					input.push_back(in_overstep);
			}			
		}
		else
		{
			input.push_back(in_overstep);
		}
	}
}

/// Возвращает true, если есть готовые отступления
bool VRichAndDistortionDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VRichAndDistortionDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VRichAndDistortionDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VRichAndDistortionDetector::Pop()
{
	output.pop();
}

//----------------------------------- Методы класса VRichAndPitDetector ----------------------------------

/// Конструктор
VRichAndPitDetector::VRichAndPitDetector(){}

/// Деструктор
VRichAndPitDetector::~VRichAndPitDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VRichAndPitDetector: Необработанные данные...");
};

/// Помещает очередное отступление
void VRichAndPitDetector::Put(const VBasicOverstepPtr& in_overstep)
{
	if (!in_overstep->IsExcluded())
	{
		if (!input.empty())
		{
			std::deque<VBasicOverstepPtr>::iterator i = std::find_if(input.begin(), input.end(), VRichAndPitCondition(in_overstep));
			if (i!=input.end())
			{			
				VBasicOverstepPtr overstep(new VRichAndPitCombination(*i, in_overstep));
				if (overstep->HasRestriction())
				{				
					output.push(overstep);						
				}
				input.erase(i);
			}
			else
			{
				//Сочетания оцениваются только для путей с установленной скоростью более 60
				if (in_overstep->GetSpeeds().GetMaxSpeed()>MIN_COMBINATION_SPEED)
					input.push_back(in_overstep);
			}
			
		}
		else
		{
			input.push_back(in_overstep);
		}
	}
}

/// Возвращает true, если есть готовые отступления
bool VRichAndPitDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VRichAndPitDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VRichAndPitDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VRichAndPitDetector::Pop()
{
	output.pop();
}

//---------------------------- Методы класса VW10OverstepDetector -----------------------------------------

/// Конструктор
VW10OverstepDetector::VW10OverstepDetector()
{
	w10_amount=0;
}

/// Деструктор
VW10OverstepDetector::~VW10OverstepDetector()
{
	if (!Empty()) 
		LOG_ERROR(L"VW10OverstepDetector: Необработанные данные...");
}

/// Помещает очередное отступление
void VW10OverstepDetector::Put(const VBasicOverstepPtr& in_overstep)
{
	if ((in_overstep->IsType(WIDEN) || in_overstep->IsType(NARROW)) && in_overstep->GetDegree()==DEGREE3)	
		w10_amount+=in_overstep->GetAmount();
	aprinfo_km=SITUATION.GetKm(SITUATION.GetRealCoord(in_overstep->GetCoord()).km);
}

/// Возвращает true, если есть готовые отступления
bool VW10OverstepDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VW10OverstepDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VW10OverstepDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VW10OverstepDetector::Pop()
{
	output.pop_front();
}

/// Слив данных
void VW10OverstepDetector::Flush()
{
	if (w10_amount>10)
	{		
		output.push_back(VBasicOverstepPtr(new VW10Overstep(aprinfo_km.end, aprinfo_km.end, w10_amount)));
	}
	w10_amount=0;
}


//---------------------------- Методы класса VK100OverstepDetector -----------------------------------------

/// Конструктор
VK100OverstepDetector::VK100OverstepDetector()
{
	k100_amount=0;
}

/// Деструктор
VK100OverstepDetector::~VK100OverstepDetector()
{
	if (!Empty()) 
		LOG_ERROR(L"VK100OverstepDetector: Необработанные данные...");
}

/// Помещает очередное отступление
void VK100OverstepDetector::Put(const VBasicOverstepPtr& in_overstep)
{
	if (in_overstep->GetDegree()==DEGREE2)
		k100_amount+=in_overstep->GetAmount();
	aprinfo_km=SITUATION.GetKm(SITUATION.GetRealCoord(in_overstep->GetCoord()).km);
}

/// Возвращает true, если есть готовые отступления
bool VK100OverstepDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VK100OverstepDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VK100OverstepDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VK100OverstepDetector::Pop()
{
	output.pop_front();
}

/// Слив данных
void VK100OverstepDetector::Flush()
{
	if (k100_amount>100)
	{		
		output.push_back(VBasicOverstepPtr(new VK100Overstep(aprinfo_km.end, aprinfo_km.end, k100_amount)));
	}
	k100_amount=0;
}

//---------------------------- Методы класса VK60OverstepDetector -----------------------------------------

/// Конструктор
VK60OverstepDetector::VK60OverstepDetector()
{
	k60_amount=0;
}

/// Деструктор
VK60OverstepDetector::~VK60OverstepDetector()
{
	if (!Empty()) 
		LOG_ERROR(L"VK60OverstepDetector: Необработанные данные...");
}

/// Помещает очередное отступление
void VK60OverstepDetector::Put(const VBasicOverstepPtr& in_overstep)
{
	if (in_overstep->GetDegree()==DEGREE2 && (in_overstep->IsType(PIT_LEFT) || in_overstep->IsType(PIT_RIGHT) ||
		in_overstep->IsType(RICH) || in_overstep->IsType(DISTORTION) ))
		k60_amount+=in_overstep->GetAmount();
	aprinfo_km=SITUATION.GetKm(SITUATION.GetRealCoord(in_overstep->GetCoord()).km);
}

/// Возвращает true, если есть готовые отступления
bool VK60OverstepDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VK60OverstepDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VK60OverstepDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VK60OverstepDetector::Pop()
{
	output.pop_front();
}

/// Слив данных
void VK60OverstepDetector::Flush()
{
	if (k60_amount>60)
	{		
		output.push_back(VBasicOverstepPtr(new VK60Overstep(aprinfo_km.end, aprinfo_km.end, k60_amount)));
	}
	k60_amount=0;
}




//----------------------------- Методы класса VRangeCondition ---------------------------------------------

/// Конструктор
VRangeCondition::VRangeCondition(const pro::interval<double>& in_value_range, const pro::interval<double>& in_length_range) : 
	value_range(in_value_range),  length_range(in_length_range) {};

/// Проверяет условие
bool VRangeCondition::TestValue(double in_value, double in_length) const
{	
	return value_range.contains(in_value) && length_range.contains(in_length);
}

//------------------------------ Методы класса VWidenCondition -----------------------------------------

/// Конструктор
VWidenCondition::VWidenCondition(DEGREE in_degree,	VSpeeds in_speeds, double in_norma, double in_radius, double in_wear)
{
	degree=in_degree;
	speeds=in_speeds;	
	norma=RoundNormaWidth(in_norma);
	radius=in_radius;
	wear=std::min<double>(in_wear, MAX_WEAR_CORRECTION);
}

/// Проверяет условие
bool VWidenCondition::TestValue(double in_value, double in_length) const
{	
	double value=round(in_value);	
	pro::interval<double> widen_range=DETECT_TABLES.GetWidenTable().GetRange(degree, speeds.GetMaxSpeed(), norma, radius);
	//Границы влияния износа
	pro::interval<double> wear_range(DETECT_TABLES.GetWidenTable().GetRange(DEGREE2, speeds.GetMaxSpeed(), norma, radius).left_bound()+wear,
							   DETECT_TABLES.GetWidenTable().GetRange(DEGREE4, speeds.GetMaxSpeed(), norma, radius).right_bound());
	//Границы 2-й степени повышается на величину износа
	if ((degree==DEGREE2 || degree==DEGREE3) && radius<=MAX_RADIUS_CORRECTION)
	{
		if (widen_range.intersect(wear_range))
			widen_range&=wear_range;
		else
		{
			widen_range=EMPTY_INTERVAL<double>();
		}
	}
	return widen_range.contains(value-norma) && in_length<=MAX_WIDTH_LENGTH;
};	


//------------------------------ Методы класса VNarrowCondition -----------------------------------------

/// Конструктор
VNarrowCondition::VNarrowCondition(DEGREE in_degree, VSpeeds in_speeds, double in_norma, bool in_zbsh)
{
	degree=in_degree;
	speeds=in_speeds;
	norma=RoundNormaWidth(in_norma);
	zbsh=in_zbsh;	
}

/// Проверяет условие
bool VNarrowCondition::TestValue(double in_value, double in_length) const
{		
	double value=round(in_value);	
	pro::interval<double> r=DETECT_TABLES.GetNarrowTable().GetRange(degree, speeds.GetMaxSpeed(), norma);
	// Учитываем поправку ЖБШ
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		if (zbsh && norma==NORMA_WIDTH_1520)
		{
			r.left_bound()+=2.0;
			r.right_bound()+=2.0;		
		}
	}
	else
	{
		if (zbsh && norma==NORMA_WIDTH_1520 && speeds.GetPassSpeed()<121)
		{
			if (degree <= DEGREE3)
			{
				r.left_bound()+=2.0;
			}
			if (degree < DEGREE3)
			{
				r.right_bound()+=2.0;
			}
		}
	}
	return r.contains(norma-value) && in_length<MAX_WIDTH_LENGTH;
};	

//--------------------------------- Методы класса VOtvodWidthCondition ----------------------------------

/// Конструктор
VOtvodWidthCondition::VOtvodWidthCondition(VSpeeds in_speeds)
{		
	speeds=in_speeds;
}

/// Проверяет условие
bool VOtvodWidthCondition::TestValue(double in_value, double in_length) const
{	
	double value=in_value;	
	if (pro::interval<int>(141, INT_MAX).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(2, DBL_MAX).contains(value);
	}
	else if (pro::interval<int>(121, 140).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(2.5, DBL_MAX).contains(value);
	}
	else if (pro::interval<int>(101, 120).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(3.0, DBL_MAX).contains(value);
	}
	else if (pro::interval<int>(81, 100).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(3.5, DBL_MAX).contains(value);
	}
	else if (pro::interval<int>(61, 80).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(4.0, DBL_MAX).contains(value);
	}
	else if (pro::interval<int>(26, 60).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(4.5, DBL_MAX).contains(value);
	}
	else if (pro::interval<int>(1, 25).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(5.0, DBL_MAX).contains(value);
	}
	else 
		return false;
}


//--------------------------------- Методы класса VOtvodLevelPassCondition ----------------------------------

/// Конструктор
VOtvodLevelPassCondition::VOtvodLevelPassCondition(VSpeeds in_speeds)
{		
	speeds=in_speeds;
}

/// Проверяет условие
bool VOtvodLevelPassCondition::TestValue(double in_value, double in_length) const
{	
	if (pro::interval<int>(220, 250, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(0.7, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(200, 220, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(0.8, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(180, 200, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(0.9, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(160, 180, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(1.0, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(140, 160, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(1.1, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(120, 140, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(1.2, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(110, 120, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(1.4, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(100, 110, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(1.5, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(95, 100, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(1.6, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(90, 95, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(1.7, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(85, 90, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(1.8, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(80, 85, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(1.9, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(75, 80, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(2.0, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(70, 75, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(2.3, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(60, 70, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(2.5, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(55, 60, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(2.7, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(50, 55, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(2.9, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(40, 50, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(3.0, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(25, 40, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(3.1, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(0, 25, false, true).contains(speeds.GetMaxSpeed()))
	{
		return pro::interval<double>(3.2, DBL_MAX).contains(in_value);
	}
	else 
		return false;
}


//--------------------------------- Методы класса VOtvodLevelTruckCondition ----------------------------------

/// Конструктор
VOtvodLevelTruckCondition::VOtvodLevelTruckCondition(VSpeeds in_speeds)
{		
	speeds=in_speeds;
}

/// Проверяет условие
bool VOtvodLevelTruckCondition::TestValue(double in_value, double in_length) const
{		
	if (pro::interval<int>(85, 90, false, true).contains(speeds.GetTruckSpeed()))
	{
		return pro::interval<double>(1.7, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(80, 85, false, true).contains(speeds.GetTruckSpeed()))
	{
		return pro::interval<double>(1.8, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(75, 80, false, true).contains(speeds.GetTruckSpeed()))
	{
		return pro::interval<double>(1.9, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(70, 75, false, true).contains(speeds.GetTruckSpeed()))
	{
		return pro::interval<double>(2.0, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(65, 70, false, true).contains(speeds.GetTruckSpeed()))
	{
		return pro::interval<double>(2.3, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(60, 65, false, true).contains(speeds.GetTruckSpeed()))
	{
		return pro::interval<double>(2.5, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(55, 60, false, true).contains(speeds.GetTruckSpeed()))
	{
		return pro::interval<double>(2.7, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(50, 55, false, true).contains(speeds.GetTruckSpeed()))
	{
		return pro::interval<double>(2.9, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(40, 50, false, true).contains(speeds.GetTruckSpeed()))
	{
		return pro::interval<double>(3.0, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(25, 40, false, true).contains(speeds.GetTruckSpeed()))
	{
		return pro::interval<double>(3.1, DBL_MAX).contains(in_value);
	}
	else if (pro::interval<int>(0, 25, false, true).contains(speeds.GetTruckSpeed()))
	{
		return pro::interval<double>(3.2, DBL_MAX).contains(in_value);
	}
	else 
		return false;
}

//--------------------------------- Методы класса VOtvodLevelCondition -------------------------------

/// Конструктор
VOtvodLevelCondition::VOtvodLevelCondition(VSpeeds in_speeds) : speeds(in_speeds) {}

/// Проверяет условие
bool VOtvodLevelCondition::TestValue(double in_value, double in_length) const
{
	return VOtvodLevelPassCondition(speeds).TestValue(in_value, in_length) || VOtvodLevelTruckCondition(speeds).TestValue(in_value, in_length);
}

//-------------------------------- Методы класса VWearCondition -----------------------------------------

///Конструктор
VWearCondition::VWearCondition(DEGREE in_degree) : degree(in_degree){};

/// Проверяет условие
bool VWearCondition::TestValue(double in_value, double in_length) const
{		
	if (in_length>MAX_WEAR_LENGTH)
		return false;
	double value=round(in_value, WEAR_PRECISION);
	switch (degree)
	{	
		case DEGREE_NONE:
			return pro::interval<double>(4, 14, false, true).contains(value);
		case DEGREE2:
			return pro::interval<double>(14, 16, false, true).contains(value);
		case DEGREE3:
			return pro::interval<double>(16, 18, false, true).contains(value);
		case DEGREE4:
			return pro::interval<double>(18, DBL_MAX, false, true).contains(value);
		default:
			return false;
	}	
}

//--------------------------------- Методы класса VZazorCondition ------------------------------------------

/// Конструктор
VZazorCondition::VZazorCondition(const VSpeeds& in_speeds, double in_diameter) : speeds(in_speeds), diameter(in_diameter){}

/// Проверяет условие
bool VZazorCondition::TestValue(double in_value, double in_length) const
{	
	//Нормы должны быть увеличены на 2 мм при диаметре отверстий в рельсах 40 мм.
	double correction = (diameter==40) ? 2.0 : 0;
	if (pro::interval<double>(24, 26, false, true).contains(in_value-correction))
	{
		return speeds.GetMaxSpeed()>100;
	}
	else if (pro::interval<double>(26, 30, false, true).contains(in_value-correction))
	{		
		return speeds.GetMaxSpeed()>60;
	}
	else if (pro::interval<double>(30, 35, false, true).contains(in_value-correction))
	{	
		return speeds.GetMaxSpeed()>25;
	}
	else if (pro::interval<double>(35, DBL_MAX, false, true).contains(in_value-correction))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//-------------------------------- Методы класса VPitCondition ----------------------------------------------

/// Конструктор
VPitCondition::VPitCondition(DEGREE in_degree, VSpeeds in_speeds, bool in_bridge)
{
	degree=in_degree;
	speeds=in_speeds;	
	bridge=in_bridge;
}

/// Проверяет условие
bool VPitCondition::TestValue(double in_value, double in_length) const
{
	double value=round(in_value);
	pro::interval<double> r = DETECT_TABLES.GetPitTable().GetRange(degree, speeds.GetMaxSpeed(), in_length);	
	if (bridge && speeds.GetMaxSpeed()<=120)
	{
		BridgeCorrection(r, degree);
	}	
	return r.contains(value);
}

//-------------------------------- Методы класса VPitConditionTestRun -----------------------------------

/// Конструктор
VPitConditionTestRun::VPitConditionTestRun()
{
}

/// Проверяет условие
bool VPitConditionTestRun::TestValue(double in_value, double in_length) const
{
	return in_value > 30.0;
}

//--------------------------------- Методы класса VLevelCondition ---------------------------------------

/// Конструктор
VLevelCondition::VLevelCondition(DEGREE in_degree, VSpeeds in_speeds)
{
	degree=in_degree;
	speeds=in_speeds;
}

/// Проверяет условие
bool VLevelCondition::TestValue(double in_value, double in_length) const
{			
	if (in_length>MAX_LEVEL_LENGTH)
		return false;
	double value=round(in_value);
	pro::interval<double> r=DETECT_TABLES.GetLevelTable().GetRange(degree, speeds.GetMaxSpeed());		
	return r.contains(value);	
}

//----------------------------------------- Методы класса VLevelConditionTestRun ------------------------------

/// Конструктор
VLevelConditionTestRun::VLevelConditionTestRun(VSpeeds in_speeds, MACHINE_TYPE in_type) : speeds(in_speeds), type(in_type) {}

/// Проверяет условие
bool VLevelConditionTestRun::TestValue(double in_value, double in_length) const
{
	switch (type)
	{
		default:
		case MACHINE_NONE:
			return false;
		case MACHINE_VPO:
			return speeds.GetMaxSpeed()>40 && in_value>20 || 
				   speeds.GetMaxSpeed()>25 && in_value>30 ||
			       in_value>40;	
		case MACHINE_VRO_OR_VPR:
			return speeds.GetMaxSpeed()>25 && in_value>20 || 
				   speeds.GetMaxSpeed()>15 && in_value>30 ||
			       in_value>40;	
		case MACHINE_VPO_PLUS_VPR:
			return speeds.GetMaxSpeed()>40 && in_value>20 || 
				   speeds.GetMaxSpeed()>25 && in_value>30 ||
			       in_value>40;	
		case MACHINE_VPO_PLUS_VPR_PLUS_DSP:
			return speeds.GetMaxSpeed()>50 && in_value>20 || 
				   speeds.GetMaxSpeed()>25 && in_value>30 ||
			       in_value>40;		
	}
}

//---------------------------------------- Методы класса VShortDistortionCondition -----------------------------

/// Конструктор
VShortDistortionCondition::VShortDistortionCondition(VSpeeds in_speeds)
{	
	speeds=in_speeds;
}

/// Проверяет условие
bool VShortDistortionCondition::TestValue(double in_value, double in_length) const
{
	if (in_length < 1.0)
		return false;
	double value=round(in_value);
	if (pro::interval<int>(201,250).contains(speeds.GetMaxSpeed()))
		return value > 7.0;
	else if (pro::interval<int>(161,200).contains(speeds.GetMaxSpeed()))
		return value > 9.0;
	else if (pro::interval<int>(141,160).contains(speeds.GetMaxSpeed()))
		return value > 10.0;
	else if (pro::interval<int>(121,140).contains(speeds.GetMaxSpeed()))
		return value > 12.0;
	else if (pro::interval<int>(101,120).contains(speeds.GetMaxSpeed()))
		return value > 14.0;
	else if (pro::interval<int>(61,100).contains(speeds.GetMaxSpeed()))
		return value > 18.0;	
	else
		return false;
}

//--------------------------------- Методы класса VDistortionCondition ---------------------------------------

/// Конструктор
VDistortionCondition::VDistortionCondition(DEGREE in_degree, VSpeeds in_speeds, bool in_bridge)
{
	degree=in_degree;
	speeds=in_speeds;
	bridge=in_bridge;
}

/// Проверяет условие
bool VDistortionCondition::TestValue(double in_value, double in_length) const
{
	double value=round(in_value);
	pro::interval<double> r=DETECT_TABLES.GetDistortionTable().GetRange(degree, speeds.GetMaxSpeed(), in_length);
	//Если мы находимся на мосту и оцениваем 3 и 4 степень нужно уменьшить границы на 10%
	//Верхнюю границу 3-й и обе границы 4-й степени. FixMе!!! нужно сделать исключение для прж
	if (bridge && speeds.GetMaxSpeed()<=120)
	{
		BridgeCorrection(r, degree);
	}	
	return r.contains(value);	
}

//------------------------------- Методы класса VDistortionConditionTestRun ----------------------------------

/// Конструктор
VDistortionConditionTestRun::VDistortionConditionTestRun(VSpeeds in_speeds, MACHINE_TYPE in_type) : speeds(in_speeds), type(in_type) {}

/// Проверяет условие
bool VDistortionConditionTestRun::TestValue(double in_value, double in_length) const
{
	switch (type)
	{
		default:
		case MACHINE_NONE:
			return false;
		case MACHINE_VPO:
			return speeds.GetMaxSpeed()>40 && in_value>20 || 
				   speeds.GetMaxSpeed()>25 && in_value>25 ||
			       in_value>30;	
		case MACHINE_VRO_OR_VPR:
			return speeds.GetMaxSpeed()>25 && in_value>20 || 
				   speeds.GetMaxSpeed()>15 && in_value>25 ||
			       in_value>30;	
		case MACHINE_VPO_PLUS_VPR:
			return speeds.GetMaxSpeed()>40 && in_value>20 || 
				   speeds.GetMaxSpeed()>25 && in_value>25 ||
			       in_value>30;	
		case MACHINE_VPO_PLUS_VPR_PLUS_DSP:
			return speeds.GetMaxSpeed()>50 && in_value>20 || 
				   speeds.GetMaxSpeed()>25 && in_value>25 ||
			       in_value>30;
	}
}

//-------------------------------- Методы класса VRichCondition ------------------------------------

/// Конструктор
VRichCondition::VRichCondition(DEGREE in_degree, VSpeeds in_speeds, bool in_bridge, bool in_thread_side, bool in_curve)
{
	degree=in_degree;
	speeds=in_speeds;
	bridge=in_bridge;	
	thread_side=in_thread_side;
	curve=in_curve;
}

/// Проверяет условие
bool VRichCondition::TestValue(double in_value, double in_length) const
{	
	//На нерихтовочной нити оцениваются только четвертые степени
	//На мостах и в тоннелях отступления в плане учитываются по обеим нитям (см. ЦП-515доп. п. 4.9.)
	if (thread_side || (NEW_INSTRUCTION && bridge && degree>DEGREE2 && !curve) || (degree==DEGREE4 && !curve))
	{
		pro::interval<double> r=DETECT_TABLES.GetRichTable().GetRange(degree, speeds.GetMaxSpeed(), in_length);
		
		//Если мы находимся на мосту и оцениваем 3 и 4 степень нужно уменьшить границы на 10%
		//Верхнюю границу 3-й и обе границы 4-й степени. FixMе!!! нужно сделать исключение для прж.
		if (bridge && speeds.GetMaxSpeed()<=120)
		{
			BridgeCorrection(r, degree);
		}	
		return r.contains(in_value);
	}
	else
		return false;
}

//-------------------------------------- Методы класса VRichConditionTestRun -------------------------------

/// Конструктор
VRichConditionTestRun::VRichConditionTestRun(VSpeeds in_speeds, MACHINE_TYPE in_type) :
	speeds(in_speeds), type(in_type) {}

/// Проверяет условие
bool VRichConditionTestRun::TestValue(double in_value, double in_length) const
{
	switch (type)
	{
		default:
		case MACHINE_NONE:
			return false;
		case MACHINE_VPO:
			return speeds.GetMaxSpeed()>40 && in_value>35 || 
				   speeds.GetMaxSpeed()>25 && in_value>50 ||
				   in_value>65;	
		case MACHINE_VRO_OR_VPR:
			return speeds.GetMaxSpeed()>25 && in_value>35 || 
				   speeds.GetMaxSpeed()>15 && in_value>50 ||
				   in_value>65;	
		case MACHINE_VPO_PLUS_VPR:
			return speeds.GetMaxSpeed()>40 && in_value>35 || 
				   speeds.GetMaxSpeed()>25 && in_value>50 ||
				   in_value>65;	
		case MACHINE_VPO_PLUS_VPR_PLUS_DSP:
			return speeds.GetMaxSpeed()>50 && in_value>35 || 
				   speeds.GetMaxSpeed()>25 && in_value>50 ||
				   in_value>65;
	}
}

//--------------------------------- Методы класса VAnpCondition ------------------------------------------

/// Конструктор
VAnpCondition::VAnpCondition(double in_anp) : anp(in_anp){}

/// Проверяет условие
bool VAnpCondition::TestValue(double in_value, double in_length) const
{
	return fabs(in_value) > anp;
}

//--------------------------------- Методы класса VPsiCondition ------------------------------------------

/// Проверяет условие
bool VPsiCondition::TestValue(double in_value, double in_length) const
{ 
	return fabs(in_value) > (restrict ? MAX_PSI_RESTRICTION : MAX_PSI);
}

//--------------------------------- Методы класса VPsiBTCondition ------------------------------------------

/// Проверяет условие
bool VPsiBTCondition::TestValue(double in_value, double in_length) const
{ 
	return fabs(in_value)  > (restrict ? MAX_PSI_BT_RESTRICTION : MAX_PSI);
}


//----------------------------------- Методы класса VControlSectionDetector -------------------------------------

///Конструктор
VControlSectionDetector::VControlSectionDetector(){};

/// Деструктор
VControlSectionDetector::~VControlSectionDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VControlSectionDetector: Необработанные данные...");
};

/// Помещает данные на вход
void VControlSectionDetector::Put(const VRtParameter& in_data)
{	
	bool is_control_section = SITUATION.IsControlArea(in_data.absCoord);
	if (is_control_section)
	{						
		width(VWidthExtractor()(in_data).GetValue());
		level(in_data.level.value);		
		coord.push_back(in_data.absCoord);
	}
	else
	{
		if (!coord.empty())
		{	
		
			double center=(coord.front()+coord.back())/2;
			double width_mo_apr=SITUATION.GetControlAreaWidth(center);			
			double width_sko_apr=SITUATION.GetControlAreaWidthSKO(center);			
			double level_mo_apr=SITUATION.GetControlAreaLevel(center);
			double level_sko_apr=SITUATION.GetControlAreaLevelSKO(center);
		
			output.push_back(VBasicOverstepPtr(new VControlSectionOverstep(
				coord.front(), coord.back(), 
				mean(width), width_mo_apr, CONTROL_WIDTH_MO)));
				
			output.push_back(VBasicOverstepPtr(new VControlSectionOverstep(
				coord.front(), coord.back(),
				mean(level), level_mo_apr, CONTROL_LEVEL_MO)));

			if (DETECT_OPTIONS.GetOption(L"SHOW_CONTROL_SECTION_SKO").Compare(OPTION_YES))
			{
				output.push_back(VBasicOverstepPtr(new VControlSectionOverstep(
					coord.front(), coord.back(), 
					sqrt(variance(width)), width_sko_apr, CONTROL_WIDTH_SKO)));
				output.push_back(VBasicOverstepPtr(new VControlSectionOverstep(
					coord.front(), coord.back(),
					sqrt(variance(level)), level_sko_apr, CONTROL_LEVEL_SKO)));
			}
			clear(width);
			clear(level);
			coord.clear();
		}					
	}	
}

/// Возвращает true, если есть готовые отступления
bool VControlSectionDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VControlSectionDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VControlSectionDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VControlSectionDetector::Pop()
{
	output.pop_front();
}

//---------------------------------- Методы класса VWidenExpressDetector ------------------------------------

/// Конструктор
VWidenExpressDetector::VWidenExpressDetector() : condition(pro::interval<double>(DETECT_OPTIONS.EXPRESS_WIDEN_THRESHOLD(), DBL_MAX), pro::interval<double>(0,DBL_MAX))
{
}

/// Деструктор
VWidenExpressDetector::~VWidenExpressDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VWidenExpressDetector: Необработанные данные...");
};

/// Ввод данных	
void VWidenExpressDetector::Put(const VRtParameter& in_record)
{
	VPoint point=VWidthExtractor()(in_record);
	if (point.GetCorrupt())
		return;
	find_excess.Input(point, condition);
	while (find_excess.Ready())
	{
		if (find_excess.Front().GetLength()>=MIN_WIDTH_LENGTH)
		{
			output.push_back(VBasicOverstepPtr(new VWidenExpress(find_excess.Front())));		
		}
		find_excess.Pop();
	}	
}

/// Возвращает true, если есть готовые отступления
bool VWidenExpressDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VWidenExpressDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VWidenExpressDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VWidenExpressDetector::Pop()
{
	output.pop_front();
}

//-------------------------------- Методы класса VWidenOverstepDetector ----------------------------------

/// Конструктор
VWidenOverstepDetector::VWidenOverstepDetector() : wear_mean(WEAR_CORRECTION_BASE), width_delay(WEAR_CORRECTION_BASE){}

/// Деструктор
VWidenOverstepDetector::~VWidenOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VWidenOverstepDetector: Необработанные данные...");
};

/// Ввод данных
void VWidenOverstepDetector::Put(const VRtParameter& in_record)
{
	VPoint point=VWidthExtractor()(in_record);
	VPoint wear=VPoint(in_record.absCoord, in_record.wear.value, in_record.wear.corrupt);
	width_delay.Put(point);
	wear_mean.Put(wear);		
	while (wear_mean.Ready() && width_delay.Ready())
	{
		VSpeeds speeds=SITUATION.GetCombinedSpeeds(width_delay.Front().GetCoord());
		double norma=SITUATION.GetWidth(width_delay.Front().GetCoord());
		double radius=SITUATION.GetCurveRadius(width_delay.Front().GetCoord());
		double wear = 0;
		//Если кривая малого радиуса, вычисляем поправку на боковой износ		
		if (radius <= MAX_RADIUS_CORRECTION)
		{
			if (DETECT_OPTIONS.UsePassportWear())
			{
				wear=SITUATION.GetNormaWear(width_delay.Front().GetCoord());
			}
			else
			{					
				if (!wear_mean.Front().GetCorrupt())
					wear=std::min<double>(wear_mean.Front().GetValue(), MAX_WEAR_CORRECTION);
				else
					wear=SITUATION.GetNormaWear(width_delay.Front().GetCoord());				
			}			
		}		
		
		find_excess4.Input(width_delay.Front(), VWidenCondition(DEGREE4, speeds, norma, radius, 0));								
		while (find_excess4.Ready())
		{
			if (find_excess4.Front().GetLength()>0 && !find_excess4.Front().GetCorrupt())
			{
				oversteps.push_back(VBasicOverstepPtr(new VWidenOverstep(find_excess4.Front(), DEGREE4)));
			}
			find_excess4.Pop();
		}				
		find_excess3.Input(width_delay.Front(), VWidenCondition(DEGREE3, speeds, norma, radius, wear));	
		while (find_excess3.Ready())
		{
			if (find_excess3.Front().GetLength()>=MIN_WIDTH_LENGTH && !find_excess3.Front().GetCorrupt())
			{		
				oversteps.push_back(VBasicOverstepPtr(new VWidenOverstep(find_excess3.Front(), DEGREE3)));
			}			
			find_excess3.Pop();
		}		
		find_excess2.Input(width_delay.Front(), VWidenCondition(DEGREE2, speeds, norma, radius, wear));			
		while (find_excess2.Ready())
		{
			if (find_excess2.Front().GetLength()>=MIN_WIDTH_LENGTH && !find_excess2.Front().GetCorrupt())
			{		
				oversteps.push_back(VBasicOverstepPtr(new VWidenOverstep(find_excess2.Front(), DEGREE2)));		
			}			
			find_excess2.Pop();
		}
		find_excess1.Input(width_delay.Front(), VWidenCondition(DEGREE1, speeds, norma, radius, wear));			
		while (find_excess1.Ready())
		{
			if (find_excess1.Front().GetLength()>=MIN_WIDTH_LENGTH && !find_excess1.Front().GetCorrupt())
			{		
				oversteps.push_back(VBasicOverstepPtr(new VWidenOverstep(find_excess1.Front(), DEGREE1)));		
			}			
			find_excess1.Pop();
		}				
		wear_mean.Pop();		
		width_delay.Pop();		
	}	
}

/// Возвращает true, если есть готовые отступления
bool VWidenOverstepDetector::Ready() const
{	
	return oversteps.size()>0;
}

/// Возвращает true, если отступлений нет
bool VWidenOverstepDetector::Empty() const
{
	return oversteps.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VWidenOverstepDetector::Front() const
{
	return oversteps.front();
}

/// Выталкивает первый элемент
void VWidenOverstepDetector::Pop()
{
	oversteps.pop_front();
}

/// Слив данных
void VWidenOverstepDetector::Flush()
{
	find_excess1.Flush();	
	find_excess2.Flush();
	find_excess3.Flush();	
	find_excess4.Flush();	
}

//---------------------------------- Методы класса VNarrowExpressDetector ------------------------------------

/// Конструктор
VNarrowExpressDetector::VNarrowExpressDetector() : condition(pro::interval<double>(0, DETECT_OPTIONS.EXPRESS_NARROW_THRESHOLD()), pro::interval<double>(0,DBL_MAX))
{
}

/// Деструктор
VNarrowExpressDetector::~VNarrowExpressDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VNarrowExpressDetector: Необработанные данные...");
};

/// Ввод данных	
void VNarrowExpressDetector::Put(const VRtParameter& in_record)
{
	VPoint point=VWidthExtractor()(in_record);
	if (point.GetCorrupt())
		return;
	find_excess.Input(point, condition);
	
	while (find_excess.Ready())
	{
		if (find_excess.Front().GetLength()>=MIN_WIDTH_LENGTH)
		{
			output.push_back(VBasicOverstepPtr(new VNarrowExpress(find_excess.Front())));		
		}
		find_excess.Pop();
	}	
}

/// Возвращает true, если есть готовые отступления
bool VNarrowExpressDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VNarrowExpressDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VNarrowExpressDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VNarrowExpressDetector::Pop()
{
	output.pop_front();
}

//-------------------------------- Методы класса VNarrowOverstepDetector ----------------------------------

/// Конструктор
VNarrowOverstepDetector::VNarrowOverstepDetector(){}

/// Деструктор
VNarrowOverstepDetector::~VNarrowOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VNarrowOverstepDetector: Необработанные данные...");
};

/// Ввод данных
void VNarrowOverstepDetector::Put(const VRtParameter& in_record)
{		
	VPoint point=VWidthExtractor()(in_record);
	VSpeeds speeds=SITUATION.GetCombinedSpeeds(in_record.absCoord);
	double norma = SITUATION.GetWidth(in_record.absCoord);
	bool zbsh = SITUATION.GetSpala(in_record.absCoord)==SPALA_GB96;	

	//Четвертая степень		
	find_excess4.Input(point, VNarrowCondition(DEGREE4, speeds, norma, zbsh));
	while (find_excess4.Ready())
	{
		if (find_excess4.Front().GetLength()>0 && !find_excess4.Front().GetCorrupt())
		{
			oversteps.push_back(VBasicOverstepPtr(new VNarrowOverstep(find_excess4.Front(), DEGREE4)));
		}
		find_excess4.Pop();
	}	
	//Третья степень		
	find_excess3.Input(point, VNarrowCondition(DEGREE3, speeds, norma, zbsh));
	while (find_excess3.Ready())
	{
		if (find_excess3.Front().GetLength()>=MIN_WIDTH_LENGTH && !find_excess3.Front().GetCorrupt())
		{
			oversteps.push_back(VBasicOverstepPtr(new VNarrowOverstep(find_excess3.Front(), DEGREE3)));
		}		
		find_excess3.Pop();
	}	
	//Вторая степень		
	find_excess2.Input(point, VNarrowCondition(DEGREE2, speeds, norma, zbsh));
	while (find_excess2.Ready())
	{
		if (find_excess2.Front().GetLength()>=MIN_WIDTH_LENGTH && !find_excess2.Front().GetCorrupt())
		{
			oversteps.push_back(VBasicOverstepPtr(new VNarrowOverstep(find_excess2.Front(), DEGREE2)));
		}		
		find_excess2.Pop();
	}
	//Первая степень		
	find_excess1.Input(point, VNarrowCondition(DEGREE1, speeds, norma, zbsh));
	while (find_excess1.Ready())
	{
		if (find_excess1.Front().GetLength()>=MIN_WIDTH_LENGTH && !find_excess1.Front().GetCorrupt())
		{
			oversteps.push_back(VBasicOverstepPtr(new VNarrowOverstep(find_excess1.Front(), DEGREE1)));
		}		
		find_excess1.Pop();
	}	
}

/// Возвращает true, если есть готовые отступления
bool VNarrowOverstepDetector::Ready() const
{
	return oversteps.size()>0;	
}

/// Возвращает true, если отступлений нет
bool VNarrowOverstepDetector::Empty() const
{
	return oversteps.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VNarrowOverstepDetector::Front() const
{
	return oversteps.front();
}

/// Выталкивает первый элемент
void VNarrowOverstepDetector::Pop()
{
	oversteps.pop_front();
}

/// Слив данных
void VNarrowOverstepDetector::Flush()
{
	find_excess4.Flush();
	find_excess3.Flush();
	find_excess2.Flush();
	find_excess1.Flush();
}

//----------------------------- Методы класса VOtvodWidthOverstepDetector ---------------------------------------

/// Конструктор
VOtvodWidthOverstepDetector::VOtvodWidthOverstepDetector() : otvod(OTVOD_WIDTH_BASE){}

/// Деструктор
VOtvodWidthOverstepDetector::~VOtvodWidthOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VOtvodWidthOverstepDetector: Необработанные данные...");
};

/// Ввод данных
void VOtvodWidthOverstepDetector::Put(const VRtParameter& in_record)
{	
	VPoint point=VWidthExtractor()(in_record);
	otvod.Put(point);
	while (otvod.Ready())
	{			
		VSpeeds speeds=SITUATION.GetCombinedSpeeds(otvod.Front().GetCoord());
		find_excess.Input(VPoint(otvod.Front().GetCoord(), fabs(otvod.Front().GetValue()), otvod.Front().GetCorrupt()), 
			VOtvodWidthCondition(speeds));
		while (find_excess.Ready())
		{
			if (find_excess.Front().GetLength()>=OTVOD_WIDTH_QUANTUM && !find_excess.Front().GetCorrupt())
			{
				oversteps.push_back(VBasicOverstepPtr(new VOtvodWidthOverstep(find_excess.Front())));	
			}			
			find_excess.Pop();
		}			
		otvod.Pop();			
	}	
}

/// Возвращает true, если есть готовые отступления
bool VOtvodWidthOverstepDetector::Ready() const
{
	return oversteps.size()>0;
}

/// Возвращает true, если отступлений нет
bool VOtvodWidthOverstepDetector::Empty() const
{
	return oversteps.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VOtvodWidthOverstepDetector::Front() const
{
	return oversteps.front();
}

/// Выталкивает первый элемент
void VOtvodWidthOverstepDetector::Pop()
{
	oversteps.pop_front();
}

//----------------------------- Методы класса VOtvodWidthOverstepDetector ---------------------------------------

/// Конструктор
VOtvodLevelOverstepDetector::VOtvodLevelOverstepDetector() : level_mean(50), otvod(40){}//FixMe!!! magic number

/// Деструктор
VOtvodLevelOverstepDetector::~VOtvodLevelOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VOtvodLevelOverstepDetector: Необработанные данные...");
};

/// Ввод данных
void VOtvodLevelOverstepDetector::Put(const VRtParameter& in_record)
{	
	VPoint point=VLevelExtractor()(in_record);
	VSpeeds speeds=SITUATION.GetCurveSpeeds(in_record.absCoord);
	level_mean.SetBase(OPTIONS.ANP_BASE(speeds.GetMaxSpeed()));
	otvod.SetBase(OPTIONS.LEVEL_OTVOD_BASE(speeds.GetMaxSpeed()));
	level_mean.Put(point);
	level_mean >> otvod;
	while (otvod.Ready())
	{			
		VSpeeds speeds=SITUATION.GetCurveSpeeds(otvod.Front().GetCoord());
		find_excess.Input(VPoint(otvod.Front().GetCoord(), fabs(otvod.Front().GetValue())), 
			VOtvodLevelPassCondition(speeds));
		while (find_excess.Ready())
		{
			if (find_excess.Front().GetLength()>=OTVOD_LEVEL_QUANTUM && !find_excess.Front().GetCorrupt())
			{
				oversteps.push_back(VBasicOverstepPtr(new VOtvodLevelOverstep(find_excess.Front())));	
			}			
			find_excess.Pop();
		}			
		otvod.Pop();			
	}	
}

/// Возвращает true, если есть готовые отступления
bool VOtvodLevelOverstepDetector::Ready() const
{
	return oversteps.size()>0;
}

/// Возвращает true, если отступлений нет
bool VOtvodLevelOverstepDetector::Empty() const
{
	return oversteps.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VOtvodLevelOverstepDetector::Front() const
{
	return oversteps.front();
}

/// Выталкивает первый элемент
void VOtvodLevelOverstepDetector::Pop()
{
	oversteps.pop_front();
}


//------------------------------------ Методы класса VWearExpressDetector ------------------------------

/// Конструктор
VWearExpressDetector::VWearExpressDetector(side_t in_side) : side(in_side), 
	condition(pro::interval<double>(DETECT_OPTIONS.EXPRESS_WEAR_THRESHOLD(), DBL_MAX), pro::interval<double>(0,DBL_MAX)){}
	
/// Деструктор
VWearExpressDetector::~VWearExpressDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VWearExpressDetector: Необработанные данные...");
};

/// Ввод данных	
void VWearExpressDetector::Put(const VRtParameter& in_record)
{
	VPoint point=VWearExtractor(side)(in_record);
	if (point.GetCorrupt())
		return;
	find_excess.Input(point, condition);
	while (find_excess.Ready())
	{
		if (find_excess.Front().GetLength()>WEAR_QUANTUM)
		{
			output.push_back(VBasicOverstepPtr(new VWearExpress(find_excess.Front(), side)));		
		}
		find_excess.Pop();
	}	
}

/// Возвращает true, если есть готовые отступления
bool VWearExpressDetector::Ready() const
{
	return output.size() > 0;
}

/// Возвращает true, если отступлений нет
bool VWearExpressDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VWearExpressDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VWearExpressDetector::Pop()
{
	output.pop_front();	
}

//----------------------------------- VWearOverstepDetector ---------------------------------------

/// Конструктор
VWearOverstepDetector::VWearOverstepDetector() {}

/// Деструктор
VWearOverstepDetector::~VWearOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VWearOverstepDetector: Необработанные данные...");
};

/// Ввод данных
void VWearOverstepDetector::Put(const VRtParameter& in_record)
{
	VPoint point=VWearExtractor()(in_record);
	double radius=static_cast<double>(SITUATION.GetCurveRadius(in_record.absCoord));
	if (radius<3000.0 || !DETECT_OPTIONS.GetOption(L"DISABLE_WEAR_DETECTOR_ON_RADIUS_3000").Compare(OPTION_YES))
	{	
		VSpeeds speeds=SITUATION.GetCurveSpeeds(in_record.absCoord);
		find_excess0.Input(point, VWearCondition(DEGREE_NONE));
		find_excess2.Input(point, VWearCondition(DEGREE2));
		find_excess3.Input(point, VWearCondition(DEGREE3));
		find_excess4.Input(point, VWearCondition(DEGREE4));
		while (find_excess0.Ready())
		{
			if (find_excess0.Front().GetLength()>=WEAR_QUANTUM && !find_excess0.Front().GetCorrupt())
			{
				VBasicOverstepPtr new_overstep(new VWearOverstep(find_excess0.Front(), SD_UNDEFINED, DEGREE_NONE));
				if (new_overstep->HasRestriction())
					oversteps.push_back(new_overstep);
			}		
			find_excess0.Pop();
		}
		while (find_excess2.Ready())
		{
			if (find_excess2.Front().GetLength()>=WEAR_QUANTUM && !find_excess2.Front().GetCorrupt())
			{
				oversteps.push_back(VBasicOverstepPtr(new VWearOverstep(find_excess2.Front(), SD_UNDEFINED, DEGREE2)));
			}		
			find_excess2.Pop();
		}

		while (find_excess3.Ready())
		{
			if (find_excess3.Front().GetLength()>=WEAR_QUANTUM && !find_excess3.Front().GetCorrupt())
			{
				oversteps.push_back(VBasicOverstepPtr(new VWearOverstep(find_excess3.Front(), SD_UNDEFINED, DEGREE3)));
			}			
			find_excess3.Pop();
		}

		while (find_excess4.Ready())
		{	
			if (find_excess4.Front().GetLength()>=WEAR_QUANTUM && !find_excess4.Front().GetCorrupt())
			{
				oversteps.push_back(VBasicOverstepPtr(new VWearOverstep(find_excess4.Front(), SD_UNDEFINED, DEGREE4)));
			}
			find_excess4.Pop();
		}
	}
}

/// Возвращает true, если есть готовые отступления
bool VWearOverstepDetector::Ready() const
{
	return oversteps.size()>0;
}

/// Возвращает true, если отступлений нет
bool VWearOverstepDetector::Empty() const
{
	return oversteps.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VWearOverstepDetector::Front() const
{
	return oversteps.front();
}

/// Выталкивает первый элемент
void VWearOverstepDetector::Pop()
{
	oversteps.pop_front();
}

/// Слив данных
void VWearOverstepDetector::Flush()
{
	find_excess2.Flush();
	find_excess3.Flush();
	find_excess4.Flush();
}

//---------------------------------- Методы класса VPitOverstepDetector ----------------------------------------

/// Конструктор
VPitOverstepDetector::VPitOverstepDetector(side_t in_side) : side(in_side), extremum(PIT_EXTREMUM_BASE), pit(in_side) {}

/// Деструктор
VPitOverstepDetector::~VPitOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VPitOverstepDetector: Необработанные данные...");
};

/// Ввод данных
void VPitOverstepDetector::Put(const VRtParameter& in_record)
{
	VPoint point=VPitExtractor(side)(in_record);
	extremum.Put(point);	
	extremum >> wave >> pit >> selector;
	while (selector.Ready())
	{
		output.push_back(selector.Front());
		selector.Pop();
	}
}

/// Возвращает true, если есть готовые отступления
bool VPitOverstepDetector::Ready() const
{	
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VPitOverstepDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VPitOverstepDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VPitOverstepDetector::Pop()
{
	output.pop_front();
}

/// Сбрасывает остаток данных
void VPitOverstepDetector::Flush()
{
	extremum >= wave >= pit >= selector;
	selector.Flush();
	while (!selector.Empty())
	{
		output.push_back(selector.Front());
		selector.Pop();
	}
}

//--------------------------------- Методы класса VLevelVariate ---------------------------------------------------

/// Конструктор
VLevelVariate::VLevelVariate()
{	
	level_variate=VPointToPointProcessorPtr(new VLevelPassportVariate());
	bad_passport_changed.put(false);	
}

/// Деструктор
VLevelVariate::~VLevelVariate() 
{
	if (!Empty()) 
		LOG_ERROR(L"VLevelVariate: Необработанные данные...");
};

/// Ввод данных	
void VLevelVariate::Put(const VPoint& in_point)
{		
	if (BAD_PASSPORT_HANDLING_POLICY().Value()==0 && (bad_passport_changed.put(SITUATION.BadLevelPassport(in_point.GetCoord()) || SITUATION.BadCoordinatePassport(in_point.GetCoord()))))
	{			
		if (bad_passport_changed.curr()==false)
		{
			level_variate.reset(new VLevelPassportVariate());
		}
		else
		{
			level_variate.reset(new VMeanVariate(MEAN_LEVEL_BASE().Value()));	
		}	
	}
	level_variate->Put(in_point);	
}

/// Возвращает true, если есть готовые отступления
bool VLevelVariate::Ready() const
{
	return level_variate->Ready();
}

/// Возвращает true, если отступлений нет
bool VLevelVariate::Empty() const
{
	return level_variate->Empty();
}

/// Возвращает первый элемент
const VPoint& VLevelVariate::Front() const
{
	return level_variate->Front();
}

/// Выталкивает первый элемент
void VLevelVariate::Pop()
{
	level_variate->Pop();
}



//----------------------------------- Методы класса VLevelOverstepDetector ----------------------------------------

/// Конструктор
VLevelOverstepDetector::VLevelOverstepDetector(){}

/// Деструктор
VLevelOverstepDetector::~VLevelOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VLevelOverstepDetector: Необработанные данные...");
};

/// Ввод данных
void VLevelOverstepDetector::Put(const VRtParameter& in_record)
{					
	VPoint point=VLevelExtractor()(in_record);
	VSpeeds speeds=SITUATION.GetCombinedSpeeds(point.GetCoord());		
	/// По ЦП-515 оценка от нулевой линии, по новой инструкции - от паспрорта
	double norma = NEW_INSTRUCTION ? SITUATION.GetPassportLevel(point.GetCoord()) : SITUATION.GetLevel(point.GetCoord());
	
	find_excess4.Input(VPoint(point.GetCoord(), fabs(point.GetValue()-norma)), VLevelCondition(DEGREE4, speeds));
	while (find_excess4.Ready())
	{
		if (find_excess4.Front().GetLength()>=LEVEL_QUANTUM && !find_excess4.Front().GetCorrupt())
		{
			oversteps.push_back(VBasicOverstepPtr(new VLevelOverstep(find_excess4.Front(), DEGREE4)));
		}			
		find_excess4.Pop();
	}	
	find_excess3.Input(VPoint(point.GetCoord(), fabs(point.GetValue()-norma)), VLevelCondition(DEGREE3, speeds));
	while (find_excess3.Ready())
	{
		if (find_excess3.Front().GetLength()>=LEVEL_QUANTUM && !find_excess3.Front().GetCorrupt())
		{
			oversteps.push_back(VBasicOverstepPtr(new VLevelOverstep(find_excess3.Front(), DEGREE3)));
		}			
		find_excess3.Pop();
	}
	find_excess2.Input(VPoint(point.GetCoord(), fabs(point.GetValue()-norma)), VLevelCondition(DEGREE2, speeds));
	while (find_excess2.Ready())
	{
		if (find_excess2.Front().GetLength()>=LEVEL_QUANTUM && !find_excess2.Front().GetCorrupt())
		{
			oversteps.push_back(VBasicOverstepPtr(new VLevelOverstep(find_excess2.Front(), DEGREE2)));		
		}			
		find_excess2.Pop();
	}
	find_excess1.Input(VPoint(point.GetCoord(), fabs(point.GetValue()-norma)), VLevelCondition(DEGREE1, speeds));
	while (find_excess1.Ready())
	{
		if (find_excess1.Front().GetLength()>=LEVEL_QUANTUM && !find_excess1.Front().GetCorrupt())
		{
			oversteps.push_back(VBasicOverstepPtr(new VLevelOverstep(find_excess1.Front(), DEGREE1)));		
		}			
		find_excess1.Pop();
	}	
}

/// Возвращает true, если есть готовые отступления
bool VLevelOverstepDetector::Ready() const
{
	return oversteps.size()>0;
}

/// Возвращает true, если отступлений нет
bool VLevelOverstepDetector::Empty() const
{
	return oversteps.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VLevelOverstepDetector::Front() const
{
	return oversteps.front();
}

/// Выталкивает первый элемент
void VLevelOverstepDetector::Pop()
{
	oversteps.pop_front();
}

/// Слив данных
void VLevelOverstepDetector::Flush()
{
	find_excess4.Flush();
	find_excess3.Flush();
	find_excess2.Flush();
	find_excess1.Flush();
}

//------------------------------------- Методы класса VShortDistortionOverstepDetector --------------------------------

/// Конструктор
VShortDistortionOverstepDetector::VShortDistortionOverstepDetector() : otvod(SHORT_DISTORTION_BASE) {}

/// Деструктор
VShortDistortionOverstepDetector::~VShortDistortionOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VShortDistortionOverstepDetector: Необработанные данные...");
};

/// Ввод данных
void VShortDistortionOverstepDetector::Put(const VRtParameter& in_record)
{
	VPoint point=VLevelExtractor()(in_record);
	otvod.Put(point);
	while (otvod.Ready())
	{			
		VSpeeds speeds=SITUATION.GetCombinedSpeeds(otvod.Front().GetCoord());
		find_excess.Input(VPoint(otvod.Front().GetCoord(), fabs(otvod.Front().GetValue()*SHORT_DISTORTION_BASE)), 
			VShortDistortionCondition(speeds));	
		while (find_excess.Ready())
		{
			if (!find_excess.Front().GetCorrupt())
			{
				oversteps.push_back(VBasicOverstepPtr(new VShortDistortionOverstep(find_excess.Front())));
			}
			find_excess.Pop();
		}		
		otvod.Pop();			
	}
}

/// Возвращает true, если есть готовые отступления
bool VShortDistortionOverstepDetector::Ready() const
{
	return oversteps.size()>0;
}

/// Возвращает true, если отступлений нет
bool VShortDistortionOverstepDetector::Empty() const
{
	return oversteps.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VShortDistortionOverstepDetector::Front() const
{
	return oversteps.front();
}

/// Выталкивает первый элемент
void VShortDistortionOverstepDetector::Pop()
{
	oversteps.pop_front();
}

//----------------------------------- Методы класса VDistortionOverstepDetector ----------------------------------------

/// Конструктор
VDistortionOverstepDetector::VDistortionOverstepDetector() : extremum(DISTORTION_EXTREMUM_BASE), wave_union(DISTORTION_LENGTH) {}

/// Деструктор
VDistortionOverstepDetector::~VDistortionOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VDistortionOverstepDetector: Необработанные данные...");
};

/// Ввод данных
void VDistortionOverstepDetector::Put(const VRtParameter& in_record)
{			
	VPoint point=VLevelExtractor()(in_record);
	level_variate.Put(point);
	level_variate >> extremum >> wave >> wave_filter >> wave_union >> distortion >> selector;
	while (selector.Ready())
	{
		output.push_back(selector.Front());
		selector.Pop();
	}
}

/// Возвращает true, если есть готовые отступления
bool VDistortionOverstepDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VDistortionOverstepDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VDistortionOverstepDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VDistortionOverstepDetector::Pop()
{
	output.pop_front();
}

/// Сбрасывает остаток данных
void VDistortionOverstepDetector::Flush()
{
	level_variate >= extremum >= wave >= wave_filter >= wave_union >= distortion >= selector;
	selector.Flush();
	while (!selector.Empty())
	{
		output.push_back(selector.Front());
		selector.Pop();
	}
}


//--------------------------------- Методы класса VRichVariate ---------------------------------------------------

/// Конструктор
VRichVariate::VRichVariate(side_t in_side) : side(in_side)
{	
	rich_variate=VPointToPointProcessorPtr(new VRichPassportVariate(side));
	bad_passport_changed.put(false);
}

/// Деструктор
VRichVariate::~VRichVariate() 
{
	if (!Empty()) 
		LOG_ERROR(L"VRichVariate: Необработанные данные...");
};

/// Ввод данных	
void VRichVariate::Put(const VPoint& in_point)
{		
	if (BAD_PASSPORT_HANDLING_POLICY().Value()==0 && (bad_passport_changed.put(SITUATION.BadPlanPassport(in_point.GetCoord()) || SITUATION.BadCoordinatePassport(in_point.GetCoord()))))
	{		
		if (bad_passport_changed.curr()==false)
		{
			rich_variate.reset(new VRichPassportVariate(side));
		}
		else
		{
			rich_variate.reset(new VMeanVariate(MEAN_RICH_BASE().Value()));	
		}	
	}	
	rich_variate->Put(in_point);	
}

/// Возвращает true, если есть готовые отступления
bool VRichVariate::Ready() const
{
	return rich_variate->Ready();
}

/// Возвращает true, если отступлений нет
bool VRichVariate::Empty() const
{
	return rich_variate->Empty();
}

/// Возвращает первый элемент
const VPoint& VRichVariate::Front() const
{
	return rich_variate->Front();
}

/// Выталкивает первый элемент
void VRichVariate::Pop()
{
	rich_variate->Pop();
}

//----------------------------------- Методы класса VRichOverstepDetector ----------------------------------------

/// Конструктор
VRichOverstepDetector::VRichOverstepDetector(side_t in_side) : side(in_side), extremum(RICH_EXTREMUM_BASE), rich_variate(in_side), rich(in_side){}

/// Деструктор
VRichOverstepDetector::~VRichOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VRichOverstepDetector: Необработанные данные...");
};

/// Ввод данных
void VRichOverstepDetector::Put(const VRtParameter& in_record)
{				
	VPoint point=VRichExtractor(side)(in_record);
	rich_variate.Put(point);		
	rich_variate >> extremum >> wave >> rich >> selector;
	while (selector.Ready())
	{	
		output.push_back(selector.Front());
		selector.Pop();
	}
}

/// Возвращает true, если есть готовые отступления
bool VRichOverstepDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VRichOverstepDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VRichOverstepDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VRichOverstepDetector::Pop()
{
	output.pop_front();
}

/// Сбрасывает остаток данных
void VRichOverstepDetector::Flush()
{
	rich_variate >= extremum >= wave >= rich >= selector;
	selector.Flush();
	while (!selector.Empty())
	{
		output.push_back(selector.Front());
		selector.Pop();
	}
}

//------------------------------------------ Методы класса VAnpAverageOverstepDetector -----------------------------------

/// Конструктор
VAnpAverageOverstepDetector::VAnpAverageOverstepDetector() : level_mean(50), curv_mean(50) {}
/// Деструктор
VAnpAverageOverstepDetector::~VAnpAverageOverstepDetector()
{
	if (!output.empty())
		LOG_ERROR(L"VAnpAverageOverstepDetector: Необработанные данные на выходе");
}
/// Ввод данных
void VAnpAverageOverstepDetector::Put(const VRtParameter& in_record)
{
	VSpeeds speeds=SITUATION.GetCurveSpeeds(in_record.absCoord);
	level_mean.SetBase(OPTIONS.ANP_BASE(speeds.GetMaxSpeed()));
	curv_mean.SetBase(OPTIONS.ANP_BASE(speeds.GetMaxSpeed()));
	level_mean.Put(VPoint(in_record.absCoord, in_record.level.value, in_record.level.corrupt));
	curv_mean.Put(VPoint(in_record.absCoord, in_record.curv.value, in_record.curv.corrupt));
	while (level_mean.Ready() && curv_mean.Ready())
	{
		if (SITUATION.GetCurveStatusPlan(curv_mean.Front().GetCoord())==ZLINE_RADIUS && !SITUATION.BadPassport(curv_mean.Front().GetCoord()))
		{
			calc.Put(curv_mean.Front(), level_mean.Front());
		}
		else
		{
			Flush();
		}
		curv_mean.Pop();
		level_mean.Pop();
	}	
}
/// Возвращает true, если есть готовые отступления
bool VAnpAverageOverstepDetector::Ready() const
{
	return output.size()>0;
}	
/// Возвращает true, если отступлений нет
bool VAnpAverageOverstepDetector::Empty() const
{
	return output.empty();
}
/// Возвращает первый элемент
const VBasicOverstepPtr& VAnpAverageOverstepDetector::Front() const
{
	return output.front();
}
/// Выталкивает первый элемент
void VAnpAverageOverstepDetector::Pop()
{
	output.pop_front();
}
/// Слив данных
void VAnpAverageOverstepDetector::Flush()
{
	if (calc.GetCount()>0)
	{
		VSpeeds speeds=SITUATION.GetCurveSpeeds((calc.GetStart()+calc.GetEnd())/2);
		if (VAnpAverageHighBTCondition(false).TestValue(fabs(calc.GetAverageAnp(speeds.GetHighBTSpeed()))) || 
			VAnpAverageHighCondition(false).TestValue(fabs(calc.GetAverageAnp(speeds.GetHighSpeed()))) || 
			VAnpAveragePassCondition(false).TestValue(fabs(calc.GetAverageAnp(speeds.GetPassSpeed()))) || 
			VAnpAverageTruckCondition(false).TestValue(fabs(calc.GetAverageAnp(speeds.GetTruckSpeed()))))
		{
			output.push_back(VBasicOverstepPtr(new VAnpAverageOverstep(calc)));
		}
		calc.Clear();
	}
}

//---------------------------------- Методы класса VAnpHorizontalOverstepDetector ------------------------------------

/// Конструктор
VAnpHorizontalOverstepDetector::VAnpHorizontalOverstepDetector()  : level_mean(50), curv_mean(50) {};	

/// Деструктор
VAnpHorizontalOverstepDetector::~VAnpHorizontalOverstepDetector() {};

/// Ввод данных
void VAnpHorizontalOverstepDetector::Put(const VRtParameter& in_record)
{
	VSpeeds speeds=SITUATION.GetCurveSpeeds(in_record.absCoord);
	level_mean.SetBase(OPTIONS.ANP_BASE(speeds.GetMaxSpeed()));
	curv_mean.SetBase(OPTIONS.ANP_BASE(speeds.GetMaxSpeed()));
	level_mean.Put(VPoint(in_record.absCoord, in_record.level.value, in_record.level.corrupt));
	curv_mean.Put(VPoint(in_record.absCoord, in_record.curv.value, in_record.curv.corrupt));
	while (level_mean.Ready() && curv_mean.Ready())
	{
		if (SITUATION.IsCurvePlan(curv_mean.Front().GetCoord()) && !SITUATION.BadPassport(curv_mean.Front().GetCoord()) &&
			(speeds.GetHighBTSpeed()!=NO_SPEED && VAnpHighBTCondition(false).TestValue(calc_anp(speeds.GetHighBTSpeed(), curv_mean.Front().GetValue(), level_mean.Front().GetValue())) ||
			 speeds.GetHighSpeed()!=NO_SPEED && VAnpHighCondition(false).TestValue(calc_anp(speeds.GetHighSpeed(), curv_mean.Front().GetValue(), level_mean.Front().GetValue())) ||
			 VAnpPassCondition(false).TestValue(calc_anp(speeds.GetPassSpeed(), curv_mean.Front().GetValue(), level_mean.Front().GetValue())) ||
			 VAnpTruckCondition(false).TestValue(calc_anp(speeds.GetTruckSpeed(), curv_mean.Front().GetValue(), level_mean.Front().GetValue()))
			))
		{			
			calc.Put(curv_mean.Front(), level_mean.Front());
		}
		else
		{
			Flush();
		}
		curv_mean.Pop();
		level_mean.Pop();
	}
};	

/// Возвращает true, если есть готовые отступления
bool VAnpHorizontalOverstepDetector::Ready() const
{
	return output.size()>0;
};

/// Возвращает true, если отступлений нет
bool VAnpHorizontalOverstepDetector::Empty() const
{
	return output.empty();
};

/// Возвращает первый элемент
const VBasicOverstepPtr& VAnpHorizontalOverstepDetector::Front() const
{
	return output.front();
};

/// Выталкивает первый элемент
void VAnpHorizontalOverstepDetector::Pop()
{
	output.pop_front();
};

/// Слив данных
void VAnpHorizontalOverstepDetector::Flush()
{
	if (calc.GetCount()>0 && fabs(calc.GetStart()-calc.GetEnd())>30.0)
	{	
		output.push_back(VBasicOverstepPtr(new VAnpHorizontalOverstep(calc)));		
	}
	calc.Clear();
};

//--------------------------------- Методы класса VPsiOverstepDetector --------------------------------------------------


/// Конструктор
VPsiOverstepDetector::VPsiOverstepDetector() : level_mean(50), curv_mean(50) {};
/// Деструктор
VPsiOverstepDetector::~VPsiOverstepDetector(){};
/// Ввод данных
void VPsiOverstepDetector::Put(const VRtParameter& in_record)
{

	VSpeeds speeds=SITUATION.GetCurveSpeeds(in_record.absCoord);
	level_mean.SetBase(OPTIONS.ANP_BASE(speeds.GetMaxSpeed()));
	curv_mean.SetBase(OPTIONS.ANP_BASE(speeds.GetMaxSpeed()));
	level_mean.Put(VPoint(in_record.absCoord, in_record.level.value, in_record.level.corrupt));
	curv_mean.Put(VPoint(in_record.absCoord, in_record.curv.value, in_record.curv.corrupt));
	while (level_mean.Ready() && curv_mean.Ready())
	{
		if (SITUATION.IsCurvePlan(curv_mean.Front().GetCoord()) && !SITUATION.BadPassport(curv_mean.Front().GetCoord()))
		{
			calc.Put(curv_mean.Front(), level_mean.Front());
		}
		else
		{
			Flush();
		}
		curv_mean.Pop();
		level_mean.Pop();
	}
};	
/// Возвращает true, если есть готовые отступления
bool VPsiOverstepDetector::Ready() const
{
	return output.size()>0;
};
/// Возвращает true, если отступлений нет
bool VPsiOverstepDetector::Empty() const
{
	return output.empty();
};
/// Возвращает первый элемент
const VBasicOverstepPtr& VPsiOverstepDetector::Front() const
{
	return output.front();
};
/// Выталкивает первый элемент
void VPsiOverstepDetector::Pop()
{
	output.pop_front();
};
/// Слив данных
void VPsiOverstepDetector::Flush()
{
	if (calc.GetCount()>0)
	{
		VSpeeds speeds=SITUATION.GetCurveSpeeds((calc.GetStart()+calc.GetEnd())/2);
		if (VPsiBTCondition(false).TestValue(calc.GetMaxPsi(speeds.GetHighBTSpeed()).GetValue()) || 
			VPsiCondition(false).TestValue(calc.GetMaxPsi(speeds.GetHighSpeed()).GetValue()) ||
			VPsiCondition(false).TestValue(calc.GetMaxPsi(speeds.GetPassSpeed()).GetValue()) ||
			VPsiCondition(false).TestValue(calc.GetMaxPsi(speeds.GetTruckSpeed()).GetValue()))
		{
			output.push_back(VBasicOverstepPtr(new VPsiOverstep(calc)));
		}
		calc.Clear();
	}
};


//--------------------------------- Методы класса VPsiTransitionOverstepDetector --------------------------------------------------


/// Конструктор
VPsiTransitionOverstepDetector::VPsiTransitionOverstepDetector() : level_mean(50), curv_mean(50) {};
/// Деструктор
VPsiTransitionOverstepDetector::~VPsiTransitionOverstepDetector(){};
/// Ввод данных
void VPsiTransitionOverstepDetector::Put(const VRtParameter& in_record)
{
	VSpeeds speeds=SITUATION.GetCurveSpeeds(in_record.absCoord);
	level_mean.SetBase(OPTIONS.ANP_BASE(speeds.GetMaxSpeed()));
	curv_mean.SetBase(OPTIONS.ANP_BASE(speeds.GetMaxSpeed()));
	level_mean.Put(VPoint(in_record.absCoord, in_record.level.value, in_record.level.corrupt));
	curv_mean.Put(VPoint(in_record.absCoord, in_record.curv.value, in_record.curv.corrupt));
	while (level_mean.Ready() && curv_mean.Ready())
	{
		ZLINE_TYPE zl=SITUATION.GetCurveStatusPlan(curv_mean.Front().GetCoord());
		if ((zl==ZLINE_OUTER_TRANSITION || zl==ZLINE_INNER_TRANSITION) && !SITUATION.BadPassport(curv_mean.Front().GetCoord()))
		{
			calc.Put(curv_mean.Front(), level_mean.Front());
		}
		else
		{	
			Flush();
		}
		curv_mean.Pop();
		level_mean.Pop();
	}	
};	
/// Возвращает true, если есть готовые отступления
bool VPsiTransitionOverstepDetector::Ready() const
{
	return output.size()>0;
};
/// Возвращает true, если отступлений нет
bool VPsiTransitionOverstepDetector::Empty() const
{
	return output.empty();
};
/// Возвращает первый элемент
const VBasicOverstepPtr& VPsiTransitionOverstepDetector::Front() const
{
	return output.front();
};
/// Выталкивает первый элемент
void VPsiTransitionOverstepDetector::Pop()
{
	output.pop_front();
};
/// Слив данных
void VPsiTransitionOverstepDetector::Flush()
{
	if (calc.GetCount()>1)
	{
		VSpeeds speeds=SITUATION.GetCurveSpeeds((calc.GetStart()+calc.GetEnd())/2);
		if (VPsiBTCondition(false).TestValue(calc.GetPsi(speeds.GetHighBTSpeed())) || 
			VPsiCondition(false).TestValue(calc.GetPsi(speeds.GetHighSpeed())) ||
			VPsiCondition(false).TestValue(calc.GetPsi(speeds.GetPassSpeed())) ||
			VPsiCondition(false).TestValue(calc.GetPsi(speeds.GetTruckSpeed())))
		{
			output.push_back(VBasicOverstepPtr(new VPsiTransitionOverstep(calc)));
		}
	}
	calc.Clear();
};


//--------------------------------- Методы класса VZazorExpressDetector -------------------------------------

/// Конструктор
VZazorExpressDetector::VZazorExpressDetector(side_t in_side) : side(in_side){}

/// Деструктор
VZazorExpressDetector::~VZazorExpressDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VZazorExpressDetector: Необработанные данные...");
};

/// Ввод данных	
void VZazorExpressDetector::Put(const VRtParameter& in_record)
{
	VPoint zazor=VZazorExtractor(side)(in_record);
	if (zazor.GetValue()>DETECT_OPTIONS.EXPRESS_ZAZOR_THRESHOLD())
	{
		output.push_back(VBasicOverstepPtr(new VZazorExpress(zazor, side)));
	}
}

/// Возвращает true, если есть готовые отступления
bool VZazorExpressDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VZazorExpressDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VZazorExpressDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VZazorExpressDetector::Pop()
{
	output.pop_front();
}

//---------------------------------- Методы класса VZazorOverstepDetector-----------------------------------

/// Конструктор
VZazorOverstepDetector::VZazorOverstepDetector(side_t in_side) : side(in_side) {}

/// Деструктор
VZazorOverstepDetector::~VZazorOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VZazorOverstepDetector: Необработанные данные...");
};

/// Ввод данных	
void VZazorOverstepDetector::Put(const VRtParameter& in_record)
{
	VPoint zazor=VZazorExtractor(side)(in_record);
	VPoint temp=VTemperatureExtractor(side)(in_record);
	VSpeeds speeds=SITUATION.GetCombinedSpeeds(zazor.GetCoord());	
	if (VZazorCondition(speeds).TestValue(zazor.GetValue()))
	{
		output.push_back(VBasicOverstepPtr(new VZazorOverstep(zazor, temp.GetValue(), side)));
	}
}

/// Возвращает true, если есть готовые отступления
bool VZazorOverstepDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VZazorOverstepDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VZazorOverstepDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VZazorOverstepDetector::Pop()
{
	output.pop_front();
}

//---------------------------------- Методы класса VZeroZazorOverstepDetector-----------------------------------

/// Конструктор
VZeroZazorOverstepDetector::VZeroZazorOverstepDetector(side_t in_side) : side(in_side) {}

/// Деструктор
VZeroZazorOverstepDetector::~VZeroZazorOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VZeroZazorOverstepDetector: Необработанные данные...");
};

/// Ввод данных	
void VZeroZazorOverstepDetector::Put(const VRtParameter& in_record)
{
	VPoint zazor=side==SD_LEFT ? VPoint(in_record.absCoord, in_record.zazorLeft.value, in_record.zazorLeft.corrupt) :
								 VPoint(in_record.absCoord, in_record.zazorRight.value, in_record.zazorRight.corrupt);
	if (zazor.GetValue()==0 && (zero_zazor.empty() || zero_zazor.back().Distance(zazor)<=RAIL_LENGTH))
	{
		zero_zazor.push_back(zazor);
	}
	if (!zero_zazor.empty() && zero_zazor.back().Distance(zazor)>RAIL_LENGTH)
	{
		if (zero_zazor.size()>=3)//Если количество зазоров 3 и более
		{
			output.push_back(VBasicOverstepPtr(new VZeroZazorOverstep(zero_zazor.front().GetCoord(), zero_zazor.back().GetCoord(), 
				zero_zazor.size(), side)));
		}
		zero_zazor.clear();
	}	
 }

/// Возвращает true, если есть готовые отступления
bool VZeroZazorOverstepDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если отступлений нет
bool VZeroZazorOverstepDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VZeroZazorOverstepDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент
void VZeroZazorOverstepDetector::Pop()
{
	output.pop_front();
}


//-------------------------------------- Методы класса VOverstepAggregator ----------------------------

/// Деструктор
VOverstepAggregator::~VOverstepAggregator() 
{
	if (!Empty()) 
		LOG_ERROR(L"VOverstepAggregator: Необработанные данные...");
};

/// Помещает очередное отступление
void VOverstepAggregator::Put(const VBasicOverstepPtr& in_overstep)
{				
	oversteps.push_back(in_overstep);
};		
/// Возвращает true, если есть готовые отступления
bool VOverstepAggregator::Ready() const
{
	return oversteps.size()>0;
};	
/// Возвращает true, если отступлений нет
bool VOverstepAggregator::Empty() const
{
	return oversteps.empty();
}
/// Возвращает первый элемент
const VBasicOverstepPtr& VOverstepAggregator::Front() const
{
	return oversteps.front();
}
/// Выталкивает первый элемент
void VOverstepAggregator::Pop()
{		
	oversteps.pop_front();
};	

//-------------------------------------- Методы класса VRTExpressDetector ----------------------------------

/// Конструктор
VRTExpressDetector::VRTExpressDetector()
{
	if (DETECT_OPTIONS.UseWidth())
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VWidenExpressDetector()));
	if (DETECT_OPTIONS.UseWidth())		
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VNarrowExpressDetector()));
	if (DETECT_OPTIONS.UseWearLeft())
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VWearExpressDetector(SD_LEFT)));
	if (DETECT_OPTIONS.UseWearRight())
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VWearExpressDetector(SD_RIGHT)));
	if (DETECT_OPTIONS.UseZazorLeft())
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VZazorExpressDetector(SD_LEFT)));
	if (DETECT_OPTIONS.UseZazorRight())
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VZazorExpressDetector(SD_RIGHT)));
}

/// Деструктор
VRTExpressDetector::~VRTExpressDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VRTExpressDetector: Необработанные данные...");
};
	
/// Выдать остаток
void VRTExpressDetector::Flush(){}

/// Помещает очередное отступление
void VRTExpressDetector::Put(const VRtParameter& in_record)
{
	/*VRtParameter record=in_record;
	if (COORD_CORRECTION_FUNCTION().Value().Valid())
	{
		record.absCoord=COORD_CORRECTION_FUNCTION().Value().Correct(record.absCoord);
	}*/
	overstep_detectors.Put(in_record);
	overstep_detectors >> aggregator;
}

/// Возвращает true, если есть готовые отступления
bool VRTExpressDetector::Ready() const
{
	return aggregator.Ready();
}

/// Возвращает true, если отступлений нет
bool VRTExpressDetector::Empty() const
{
	return aggregator.Empty();
}

/// Возвращает первый элемент
const VBasicOverstepPtr& VRTExpressDetector::Front() const
{
	return aggregator.Front();
}

/// Выталкивает первый элемент
void VRTExpressDetector::Pop()
{
	aggregator.Pop();
}

//------------------------------------- Методы класса VGeometryOverstepDetector ----------------------------------

/// Конструктор
VGeometryOverstepDetector::VGeometryOverstepDetector() : coord_diff(0)
{		
	// Инициализируем таблицы
	DETECT_TABLES_INIT;
	
	// Если отключена расшифровка
	if (DETECT_OPTIONS.GetOption(L"DISABLE_ALL_DETECTORS").Compare(OPTION_YES))
		return;
	
	//Сбои параметров
	if (CORRUPT_DETECTION_POLICY().Value()==1)
	{
		if (DETECT_OPTIONS.UseWidth())				
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VPkCorruptDetector<VWidthExtractor, GROUP_SERVICE_GEOMETRY>(VWidthExtractor(), WIDTH_CORRUPT_PK, CORRUPT_PERCENT().Value())));	
		if (DETECT_OPTIONS.UsePitLeft())		
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VPkCorruptDetector<VPitExtractor, GROUP_SERVICE_GEOMETRY>(VPitExtractor(SD_LEFT), PIT_LEFT_CORRUPT_PK, CORRUPT_PERCENT().Value())));	
		if (DETECT_OPTIONS.UsePitRight())
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VPkCorruptDetector<VPitExtractor, GROUP_SERVICE_GEOMETRY>(VPitExtractor(SD_RIGHT), PIT_RIGHT_CORRUPT_PK, CORRUPT_PERCENT().Value())));
		if (DETECT_OPTIONS.UseLevel())
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VPkCorruptDetector<VLevelExtractor, GROUP_SERVICE_GEOMETRY>(VLevelExtractor(), LEVEL_CORRUPT_PK, CORRUPT_PERCENT().Value())));
		if (DETECT_OPTIONS.UseRichLeft())
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VPkCorruptDetector<VRichExtractor, GROUP_SERVICE_GEOMETRY>(VRichExtractor(SD_LEFT), RICH_LEFT_CORRUPT_PK, CORRUPT_PERCENT().Value())));
		if (DETECT_OPTIONS.UseRichRight())
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VPkCorruptDetector<VRichExtractor, GROUP_SERVICE_GEOMETRY>(VRichExtractor(SD_RIGHT), RICH_RIGHT_CORRUPT_PK, CORRUPT_PERCENT().Value())));
	}
	else
	{
		if (DETECT_OPTIONS.UseWidth())				
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VCorruptDetector<VWidthExtractor, GROUP_SERVICE_GEOMETRY>(VWidthExtractor(), WIDTH_CORRUPT, CORRUPT_LENGTH)));	
		if (DETECT_OPTIONS.UsePitLeft())		
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VCorruptDetector<VPitExtractor, GROUP_SERVICE_GEOMETRY>(VPitExtractor(SD_LEFT), PIT_LEFT_CORRUPT, CORRUPT_LENGTH)));	
		if (DETECT_OPTIONS.UsePitRight())
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VCorruptDetector<VPitExtractor, GROUP_SERVICE_GEOMETRY>(VPitExtractor(SD_RIGHT), PIT_RIGHT_CORRUPT, CORRUPT_LENGTH)));
		if (DETECT_OPTIONS.UseLevel())
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VCorruptDetector<VLevelExtractor, GROUP_SERVICE_GEOMETRY>(VLevelExtractor(), LEVEL_CORRUPT, CORRUPT_LENGTH)));
		if (DETECT_OPTIONS.UseRichLeft())
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VCorruptDetector<VRichExtractor, GROUP_SERVICE_GEOMETRY>(VRichExtractor(SD_LEFT), RICH_LEFT_CORRUPT, CORRUPT_LENGTH)));	
		if (DETECT_OPTIONS.UseRichRight())
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VCorruptDetector<VRichExtractor, GROUP_SERVICE_GEOMETRY>(VRichExtractor(SD_RIGHT), RICH_RIGHT_CORRUPT, CORRUPT_LENGTH)));
	}
	
	// Информация об отключении каналов
	if (!DETECT_OPTIONS.UseWidth())				
		corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VFallbackOverstepDetector(GROUP_SERVICE_GEOMETRY, WIDTH_OFF)));
	if (!DETECT_OPTIONS.UsePitLeft())		
		corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VFallbackOverstepDetector(GROUP_SERVICE_GEOMETRY, PIT_LEFT_OFF)));
	if (!DETECT_OPTIONS.UsePitRight())
		corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VFallbackOverstepDetector(GROUP_SERVICE_GEOMETRY, PIT_RIGHT_OFF)));
	if (!DETECT_OPTIONS.UseLevel())
		corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VFallbackOverstepDetector(GROUP_SERVICE_GEOMETRY, LEVEL_OFF)));
	if (!DETECT_OPTIONS.UseRichLeft())		
		corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VFallbackOverstepDetector(GROUP_SERVICE_GEOMETRY, RICH_LEFT_OFF)));
	if (!DETECT_OPTIONS.UseRichRight())
		corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VFallbackOverstepDetector(GROUP_SERVICE_GEOMETRY, RICH_RIGHT_OFF)));
		
	//Расшифровщики отступлений	
	if (DETECT_OPTIONS.UseWidth())					
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VWidenOverstepDetector()));
	if (DETECT_OPTIONS.UseWidth())		
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VNarrowOverstepDetector()));
	if (DETECT_OPTIONS.UseWidth() && NEW_INSTRUCTION)//ОШК только в новой инструкции
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VOtvodWidthOverstepDetector()));											
	if (DETECT_OPTIONS.UsePitLeft())
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VPitOverstepDetector(SD_LEFT)));
	if (DETECT_OPTIONS.UsePitRight())
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VPitOverstepDetector(SD_RIGHT)));
	if (DETECT_OPTIONS.UseLevel())
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VLevelOverstepDetector()));
	if (DETECT_OPTIONS.UseLevel())		
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VDistortionOverstepDetector()));
	if (DETECT_OPTIONS.UseLevel() && NEW_INSTRUCTION)//Короткий перекос только в новой инструкции
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VShortDistortionOverstepDetector()));
	if (DETECT_OPTIONS.UseRichLeft())
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VRichOverstepDetector(SD_LEFT)));
	if (DETECT_OPTIONS.UseRichRight())
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VRichOverstepDetector(SD_RIGHT)));
		
	//Оценка кривых
	if (DETECT_OPTIONS.UseLevel())
	{
		if (DETECT_OPTIONS.GetOption(L"CURVE_INSTRUCTION").Compare(OPTION_YES))
		{
			overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VAnpAverageOverstepDetector()));
			overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VAnpHorizontalOverstepDetector()));
			overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VPsiOverstepDetector()));
			overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VPsiTransitionOverstepDetector()));
		}
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VOtvodLevelOverstepDetector()));
	}	

	//Контрольные участки
	if (DETECT_OPTIONS.UseLevel() && DETECT_OPTIONS.UseWidth())
		corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VControlSectionDetector()));	
	
	//Расшифровщики сочетаний
	if (DETECT_OPTIONS.UsePitLeft())
		group_detectors.AddProcessor(VGroupDetectorPtr(new VPitGroupDetector(SD_LEFT)));
	if (DETECT_OPTIONS.UsePitRight())
		group_detectors.AddProcessor(VGroupDetectorPtr(new VPitGroupDetector(SD_RIGHT)));
	if (DETECT_OPTIONS.UseRichLeft())
		group_detectors.AddProcessor(VGroupDetectorPtr(new VRichGroupDetector(SD_LEFT)));
	if (DETECT_OPTIONS.UseRichRight())
		group_detectors.AddProcessor(VGroupDetectorPtr(new VRichGroupDetector(SD_RIGHT)));
	if (DETECT_OPTIONS.UseLevel())
		group_detectors.AddProcessor(VGroupDetectorPtr(new VDistortionGroupDetector()));
	if (DETECT_OPTIONS.UseRichRight() && DETECT_OPTIONS.UsePitRight() || DETECT_OPTIONS.UseRichLeft() && DETECT_OPTIONS.UsePitLeft())
		group_detectors.AddProcessor(VGroupDetectorPtr(new VRichAndPitDetector()));
	if ((DETECT_OPTIONS.UseRichRight() || DETECT_OPTIONS.UseRichLeft()) && DETECT_OPTIONS.UseLevel())
		group_detectors.AddProcessor(VGroupDetectorPtr(new VRichAndDistortionDetector()));
	if (OLD_INSTRUCTION)//Ш10, К100 и К60 только по ЦП-515
	{
		group_detectors.AddProcessor(VGroupDetectorPtr(new VW10OverstepDetector()));
		group_detectors.AddProcessor(VGroupDetectorPtr(new VK100OverstepDetector()));
		group_detectors.AddProcessor(VGroupDetectorPtr(new VK60OverstepDetector()));
	}
};

/// Деструктор
VGeometryOverstepDetector::~VGeometryOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VGeometryOverstepDetector: Необработанные данные...");
};

/// Помещает данные на вход
void VGeometryOverstepDetector::Put(const VRtParameter& in_record)
{
	//Проверяем выход за пределы направления
	bool has_apr=SITUATION.GetPchNum(in_record.absCoord)!=SZO_NONE;
	if (!has_apr)
		return;
	
	corrupt_detectors.Put(in_record);
	overstep_detectors.Put(in_record);
	
	
	/*
	FixMe!!! перенести эту логику в детекторы. Она может несколько отличаться для разных типов отступлений.
	Например, при оценке участков сбоев нужно рассматривать пикеты, а при оценке волн нельзя сливать данные
	на границе километра.
	*/
	if (km_change.put(SITUATION.GetRealCoord(in_record.absCoord).km))
	{
		corrupt_detectors.Flush();
		overstep_detectors.Flush();
		group_detectors.Flush();
	}

	if (coord_diff.put(in_record.absCoord)!=0)
	{
		///Сортировщик отступлений создается при получении второго кадра
		if (!arranger)
		{			
			arranger=(coord_diff.get()>0) ? VOverstepArrangerPtr(new VOverstepArrangerDec()) : VOverstepArrangerPtr(new VOverstepArrangerInc());
		}
		else
		{
			arranger->Put(in_record.absCoord);
			corrupt_detectors >> *arranger;
			overstep_detectors >> (*arranger & group_detectors);
			group_detectors >> *arranger;
		}
	}
}

/// Обрабатывает остаток
void VGeometryOverstepDetector::Flush()
{		
	if (arranger)
	{		
		corrupt_detectors >= *arranger;
		overstep_detectors >= (*arranger & group_detectors);
		group_detectors >= *arranger;
	}
}

/// Возвращает true, если есть готовые отступления
bool VGeometryOverstepDetector::Ready() const
{
	if (arranger)
		return arranger->Ready();
	else 
		return false;
};	
/// Возвращает true, если отступлений нет
bool VGeometryOverstepDetector::Empty() const
{
	if (arranger)
		return arranger->Empty();
	else
		return true;
};
/// Возвращает первый элемент
const VBasicOverstepPtr& VGeometryOverstepDetector::Front() const
{	
	return arranger->Front();
};
/// Выталкивает первый элемент
void VGeometryOverstepDetector::Pop()
{
	if (arranger)
		arranger->Pop();	
};

//------------------------------------- Методы класса VExtraOverstepDetector ----------------------------------

/// Конструктор
VExtraOverstepDetector::VExtraOverstepDetector() : coord_diff(0)
{		

	// Если отключена расшифровка
	if (DETECT_OPTIONS.GetOption(L"DISABLE_ALL_DETECTORS").Compare(OPTION_YES))
		return;

	if (CORRUPT_DETECTION_POLICY().Value()==1)
	{
		if (DETECT_OPTIONS.UseWearLeft() || DETECT_OPTIONS.UseWearRight())
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VPkCorruptDetector<VWearExtractor, GROUP_SERVICE_EXTRA>(VWearExtractor(), WEAR_CORRUPT_PK, CORRUPT_PERCENT().Value())));
	}
	else
	{
		if (DETECT_OPTIONS.UseWearLeft() || DETECT_OPTIONS.UseWearRight())
			corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VCorruptDetector<VWearExtractor, GROUP_SERVICE_EXTRA>(VWearExtractor(), WEAR_CORRUPT, CORRUPT_LENGTH)));
	}
	
	if (!DETECT_OPTIONS.UseWearLeft())				
		corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VFallbackOverstepDetector(GROUP_SERVICE_EXTRA, WEAR_LEFT_OFF)));
	if (!DETECT_OPTIONS.UseWearRight())		
		corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VFallbackOverstepDetector(GROUP_SERVICE_EXTRA, WEAR_RIGHT_OFF)));
	if (!DETECT_OPTIONS.UseZazorLeft())
		corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VFallbackOverstepDetector(GROUP_SERVICE_EXTRA, ZAZOR_LEFT_OFF)));
	if (!DETECT_OPTIONS.UseZazorRight())
		corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VFallbackOverstepDetector(GROUP_SERVICE_EXTRA, ZAZOR_RIGHT_OFF)));
	if (!DETECT_OPTIONS.UseMagZazorLeft())
		corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VFallbackOverstepDetector(GROUP_SERVICE_EXTRA, MAG_ZAZOR_LEFT_OFF)));
	if (!DETECT_OPTIONS.UseMagZazorRight())
		corrupt_detectors.AddProcessor(VOverstepDetectorPtr(new VFallbackOverstepDetector(GROUP_SERVICE_EXTRA, MAG_ZAZOR_RIGHT_OFF)));
	
	if (DETECT_OPTIONS.UseZazorLeft() && DETECT_OPTIONS.GetOption(L"DETECT_ZAZOR").Compare(OPTION_YES))
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VZazorOverstepDetector(SD_LEFT)));
	if (DETECT_OPTIONS.UseZazorLeft() && DETECT_OPTIONS.UseMagZazorLeft() && DETECT_OPTIONS.GetOption(L"DETECT_ZERO_ZAZOR").Compare(OPTION_YES))
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VZeroZazorOverstepDetector(SD_LEFT)));
	if (DETECT_OPTIONS.UseZazorRight() && DETECT_OPTIONS.GetOption(L"DETECT_ZAZOR").Compare(OPTION_YES))
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VZazorOverstepDetector(SD_RIGHT)));
	if (DETECT_OPTIONS.UseZazorRight() && DETECT_OPTIONS.UseMagZazorRight() && DETECT_OPTIONS.GetOption(L"DETECT_ZERO_ZAZOR").Compare(OPTION_YES))
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VZeroZazorOverstepDetector(SD_RIGHT)));
	if ((DETECT_OPTIONS.UseWearLeft() || DETECT_OPTIONS.UseWearRight()) && DETECT_OPTIONS.GetOption(L"DETECT_WEAR").Compare(OPTION_YES))
		overstep_detectors.AddProcessor(VOverstepDetectorPtr(new VWearOverstepDetector()));
};

/// Деструктор
VExtraOverstepDetector::~VExtraOverstepDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VExtraOverstepDetector: Необработанные данные...");
};

/// Помещает данные на вход
void VExtraOverstepDetector::Put(const VRtParameter& in_record)
{	
	//Проверяем выход за пределы направления
	bool has_apr=SITUATION.GetPchNum(in_record.absCoord)!=SZO_NONE;
	if (!has_apr)
		return;
				
	//Вводим данные в расшифровщики
	bool km_changed=km_change.put(SITUATION.GetRealCoord(in_record.absCoord).km);
	double diff=coord_diff.put(in_record.absCoord);
	
	corrupt_detectors.Put(in_record);
	overstep_detectors.Put(in_record);
	
	if (km_changed)
	{
		corrupt_detectors.Flush();
		overstep_detectors.Flush();
	}
		
	if (diff!=0)
	{
		///Сортировщик отступлений создается при получении второго кадра
		if (!arranger)
		{			
			arranger=(diff>0) ? VOverstepArrangerPtr(new VOverstepArrangerDec()) :
							      VOverstepArrangerPtr(new VOverstepArrangerInc());
		}
		else
		{			
			arranger->Put(in_record.absCoord);
			corrupt_detectors >> *arranger;
			overstep_detectors >> *arranger;
		}
	}			
};

/// Обрабатывает остаток
void VExtraOverstepDetector::Flush()
{		
	if (arranger)
	{	
		corrupt_detectors >= *arranger;
		overstep_detectors >= *arranger;
	}
};

/// Возвращает true, если есть готовые отступления
bool VExtraOverstepDetector::Ready() const
{
	if (arranger)
		return arranger->Ready();
	else 
		return false;
};	
/// Возвращает true, если отступлений нет
bool VExtraOverstepDetector::Empty() const
{
	if (arranger)
		return arranger->Empty();
	else
		return true;
};
/// Возвращает первый элемент
const VBasicOverstepPtr& VExtraOverstepDetector::Front() const
{	
	return arranger->Front();
};
/// Выталкивает первый элемент
void VExtraOverstepDetector::Pop()
{
	if (arranger)
		arranger->Pop();	
};



//---------------------------------------- Методы класса VRTKmData ----------------------------------------------


/// Конструктор
VRTKmData::VRTKmData(int in_km)
{
	aprinfo_km = SITUATION.GetKm(in_km);		
	speeds=SITUATION.GetSpeeds(aprinfo_km.base);
	min_speeds=SITUATION.GetMinSpeeds(aprinfo_km.base, aprinfo_km.end);
}

/// Возвращает количество отступлений по группе
size_t VRTKmData::GetOverstepCount(OVERSTEP_GROUP in_group) const
{
	return std::count_if(oversteps.begin(), oversteps.end(), find_group(in_group));
}

/// Возвращает количество отступлений по группе и степени
size_t VRTKmData::GetOverstepCount(OVERSTEP_GROUP in_group, DEGREE in_degree) const
{
	return std::count_if(oversteps.begin(), oversteps.end(), find_degree_group(in_degree, in_group));
}

/// Возвращает количество элементарных отступлений по группе
size_t VRTKmData::GetOverstepAmount(OVERSTEP_GROUP in_group) const
{
	size_t amount=0;
	for (size_t i=0;i<oversteps.size();++i)
	{
		if (!oversteps.at(i)->IsExcluded() && oversteps.at(i)->GetGroup()==in_group)
		{
			amount+=oversteps.at(i)->GetAmount();
		}
	}
	return amount;
}

/// Возвращает количество элементарных отступлений по степени и группе
size_t VRTKmData::GetOverstepAmount(OVERSTEP_GROUP in_group, DEGREE in_degree) const
{
	size_t amount=0;
	for (size_t i=0;i<oversteps.size();++i)
	{
		if (!oversteps.at(i)->IsExcluded() && oversteps.at(i)->GetGroup()==in_group && oversteps.at(i)->GetDegree()==in_degree)
		{
			amount+=oversteps.at(i)->GetAmount();
		}
	}
	return amount;
}

/// Возвращает число ограничений
size_t VRTKmData::GetRestrictionCount(OVERSTEP_GROUP in_group) const
{
	return std::count_if(oversteps.begin(), oversteps.end(), find_restriction_group(in_group));
}

/// Возвращает количество ограничений
size_t VRTKmData::GetRestrictionCount() const
{
	return std::count_if(oversteps.begin(), oversteps.end(), find_restriction());
}

/// Возвращает строку описания участка
std::wstring VRTKmData::GetSectionString() const
{	
	std::wstring direction=string_to_wstring(SITUATION.GetDirName());
	long dir=SITUATION.GetDirCode();
	std::wstring way=string_to_wstring(SITUATION.GetWayCode());
	long pch=SITUATION.GetPchNum(aprinfo_km.base);
	std::wstring km = (aprinfo_km.len==1000) ? boost::str(boost::wformat(L"%d") % aprinfo_km.km) : boost::str(boost::wformat(L"%d (%0.0f м)") % aprinfo_km.km % aprinfo_km.len);
	if (pch!=SZO_NONE)
		return boost::str(boost::wformat(L"%s(%d) Путь:%s Км:%s ПЧ-%d Уст: %s") % direction %dir %way %km %pch
			%speeds.format_short());
	else
		return boost::str(boost::wformat(L"%s(%d) Путь:%s Выход за пределы направления") % direction %dir %way);
};

/// Возвращает true если скорость ограничена
bool VRTKmData::HasRestriction() const
{
	return GetRestrictionCount()>0;	
}

/// Возвращает описание ограничений скорости
VSpeeds VRTKmData::GetRestriction() const
{	
	VSpeeds restriction;
	for (size_t i=0; i<oversteps.size(); ++i)
	{	
		if (oversteps.at(i)->HasRestriction())
			restriction = VSpeeds::CombineSpeeds(restriction, oversteps.at(i)->GetRestriction());
	}
	return restriction;
}

/// Возвращает true если скорость ограничена
bool VRTKmData::HasRestriction(OVERSTEP_GROUP in_group) const
{
	return GetRestrictionCount(in_group)>0;	
}

/// Возвращает ограничения скорости
VSpeeds VRTKmData::GetRestriction(OVERSTEP_GROUP in_group) const
{
	VSpeeds restriction;
	for (size_t i=0; i<oversteps.size(); ++i)
	{
		if (oversteps.at(i)->GetGroup()==in_group && oversteps.at(i)->HasRestriction())
			restriction = VSpeeds::CombineSpeeds(restriction, oversteps.at(i)->GetRestriction());	
	}
	return restriction;
}

/// Возвращает описание ограничений скорости
std::wstring VRTKmData::GetRestrictionString() const
{		
	VSpeeds ovr_speeds;
	for (size_t i=0; i<oversteps.size(); ++i)
	{
		if (oversteps.at(i)->HasRestriction())
			ovr_speeds = VSpeeds::CombineSpeeds(ovr_speeds, oversteps.at(i)->GetSpeeds());	
	}				
	return VSpeeds::Restriction(ovr_speeds, GetRestriction()).format_short();
}

/// Возвращает описание ограничений скорости
std::wstring VRTKmData::GetRestrictionString(OVERSTEP_GROUP in_group) const
{		
	VSpeeds ovr_speeds;
	for (size_t i=0; i<oversteps.size(); ++i)
	{
		if (oversteps.at(i)->GetGroup()==in_group && oversteps.at(i)->HasRestriction())
			ovr_speeds = VSpeeds::CombineSpeeds(ovr_speeds, oversteps.at(i)->GetSpeeds());	
	}				
	return VSpeeds::Restriction(ovr_speeds, GetRestriction(in_group)).format_short();
}

///Возвращает сводку по отступлениям
std::wstring VRTKmData::GetSummaryString(OVERSTEP_GROUP in_group) const
{		
	std::wstringstream out;
    if (!NEW_INSTRUCTION && !penalty.GetClassicPenalty())
    {
        out << this->GetKm() << L"км: оценка километра не была произведена";
        LOG_ERROR(out.str());
        out.str(L"");
    }

	if (in_group==GROUP_GEOMETRY)//Балловая оценка выводится только для отступлений по ЦП-515
	{
		out << L"Балл - " << GetPenalty() << L" ";
		if (NEW_INSTRUCTION)
			out << L"(в т.ч. по кривым " << GetCrvPenalty() << L" ";
	}
	out << L"Кол.ст. - ";
	if (NEW_INSTRUCTION)
		out << L"1:" << GetOverstepAmount(in_group, DEGREE1) << L"; ";
	out << L"2:" << GetOverstepAmount(in_group, DEGREE2) << L"; ";
	out << L"3:" << GetOverstepAmount(in_group, DEGREE3) << L"; ";
	out << L"4:" << GetOverstepAmount(in_group, DEGREE4) << L"; ";
	out << L"Кол.огр. " << GetRestrictionCount(in_group) << L" ";
	out << L"Огр. " << GetRestrictionString(in_group) << L" ";
	out << L"Скор." << GetAverageSpeed() << L" ";
	out << L"[" << DETECT_OPTIONS.GetOption(L"PASSPORT").GetValue();
	out << L"/" << DETECT_OPTIONS.GetOption(L"WEAR").GetValue() << L"]";
	return out.str();
};

/// Возвращает перегон
std::wstring VRTKmData::GetPeregon() const
{
	return string_to_wstring(SITUATION.GetPeregon(aprinfo_km.base));
};

///Возвращает номер километра
int VRTKmData::GetKm() const
{
	return aprinfo_km.km;
}

/// Возвращает координату начала километра
double VRTKmData::GetKmStart() const
{
	return aprinfo_km.base;
}

/// Возвращает координату конца километра
double VRTKmData::GetKmEnd() const
{
	return aprinfo_km.end;
}

/// Возвращает длину километра
double VRTKmData::GetKmLength() const
{
	return aprinfo_km.end-aprinfo_km.base;
}

/// Возвращает среднюю скорость на километре
double VRTKmData::GetAverageSpeed() const
{
	if (data.empty())
		return 0;
	accumulator_set<double, features<tag::mean> > speed;	
	for (size_t i=0;i<data.size();++i)
	{
		speed(data.at(i).speed);
	}
	return round(mean(speed));
}

/// Добавляет кадр данных
void VRTKmData::PutData(const VRtParameter& in_data)
{	
	data.push_back(in_data);
}

/// Возвращает кадр данных
const VRtParameter& VRTKmData::GetData(size_t in_index) const
{
	return data.at(in_index);
}

/// Возвращает число кадров данных
size_t VRTKmData::GetDataCount() const
{
	return data.size();
}

/// Возвращает длину данных
double VRTKmData::GetDataLength() const
{	
	return data.empty() ? 0 : fabs(data.back().absCoord - data.front().absCoord);
}

///Добавляет отступление
void VRTKmData::PutOverstep(const VBasicOverstepPtr& in_overstep)
{		
	this->oversteps.push_back(in_overstep);
	this->penalty.Apply(SITUATION.GetAprinfo(), this->oversteps.back());
}

/// Возвращает отступление
const VBasicOverstepPtr& VRTKmData::GetOverstep(size_t in_index) const
{
	return this->oversteps.at(in_index);
}

/// Возвращает число отступлений
size_t VRTKmData::GetOverstepCount() const
{
	return this->oversteps.size();
}

/// Возвращает границы блока данных
pro::interval<double> VRTKmData::GetCoordRange() const
{
	/// Конец километра включается, так как к нему приписываются Ш10 и т.п.
	return pro::interval<double>(GetKmStart(), GetKmEnd(), false, true);
}

/// Финальные расчеты
void VRTKmData::CalcPenalty()
{	
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
	    size_t restriction_count = 0, degree_2_count = 0, degree_3_count = 0;
	    for (size_t i = 0; i < this->oversteps.size(); ++i)
	    {
	        VBasicOverstep & ovr = *this->oversteps.at(i);
	    
	        if (ovr.HasRestriction() && penalty.IsCP515Restriction(ovr.GetType()))
	        {
	            ++restriction_count;
	        }
	        
	        if (ovr.GetDegree() == DEGREE2 && penalty.IsCP515Degree2(ovr.GetType()))
	        {
	            ++degree_2_count;
	        }
	        
	        if (ovr.GetDegree() == DEGREE3 && penalty.IsCP515Degree3(ovr.GetType()))
	        {
	            ++degree_3_count;
	        }
	    }
    	
		this->penalty.CalcOldStylePenalty(this->speeds.GetMaxSpeed(), 
			restriction_count, degree_2_count, degree_3_count);
	}
}

// Ограничение на километр
long VRTKmData::GetPenalty() const
{
	return this->penalty.GetPenalty();
}

// Ограничение на километр
long VRTKmData::GetCrvPenalty() const
{
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))	
	{
		return 0;
	}
	else 
	{
		return this->penalty.GetCrvPenalty();
	}
}

//-------------------------------------------- Методы класса VRTKmProcessor --------------------------------------

/// Конструктор
VRTKmProcessor::VRTKmProcessor(){}

/// Помещает на вход кадр данных
void VRTKmProcessor::Put(const VRtParameter& in_data)
{	
	VWayCoord wc=SITUATION.GetRealCoord(in_data.absCoord);
	if (km.empty() || wc.km!=km.back()->GetKm() || 
		pch_changed.put(SITUATION.GetPchNum(in_data.absCoord)) && DETECT_OPTIONS.GetOption(L"CUT_DISTANTIONS").Compare(OPTION_YES))
	{			
		km.push_back(VRTKmDataPtr(new VRTKmData(wc.km)));
	}
	km.back()->PutData(in_data);
}

/// Помещает на вход отступление
void VRTKmProcessor::Put(const VBasicOverstepPtr& in_overstep)
{		
	//Отступления приходят с запаздыванием
	for (size_t i=0; i<km.size(); ++i)
	{		
		if (km.at(i)->GetDataLength()>0 && km.at(i)->GetCoordRange().contains(in_overstep->GetCoord()))
		{
			km.at(i)->PutOverstep(in_overstep);			
		}
	}
}

/// Возвращает true, если данные готовы
bool VRTKmProcessor::Ready() const
{		
	return km.size()>1 && km.back()->GetDataLength()>KM_READY_DISTANCE;
}

/// Возвращает true, если обработанных данных нет
bool VRTKmProcessor::Empty() const
{
	return km.empty();
}

/// Возвращает первый элемент данных
const VRTKmDataPtr& VRTKmProcessor::Front() const
{	
	return km.front();
}

/// Выталкивает первый элемент данных
void VRTKmProcessor::Pop()
{	
	km.pop_front();
}

//------------------------------------------ Методы класса VKmRater ------------------------------------------------

/// Конструктор
VKmRater::VKmRater(){}
/// Помещает на вход кадр данных
void VKmRater::Put(const VRTKmDataPtr& in_data)
{
	output.push_back(in_data);
	output.back()->CalcPenalty();
}
/// Возвращает true, если данные готовы
bool VKmRater::Ready() const
{
	return output.size()>0;
}
/// Возвращает true, если обработанных данных нет
bool VKmRater::Empty() const	
{
	return output.empty();
}
/// Возвращает первый элемент данных
const VRTKmDataPtr& VKmRater::Front() const
{
	return output.front();
}
/// Выталкивает первый элемент данных
void VKmRater::Pop()
{
	output.pop_front();
}

//----------------------------------------- Методы класса VWidthCorruptDetector ------------------------------------------

/// Конструктор
VWidthCorruptDetector::VWidthCorruptDetector() : width_variate(WIDTH_CORRUPT_DETECTION_BASE){}

/// Деструктор
VWidthCorruptDetector::~VWidthCorruptDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VWidthCorruptDetector: Необработанные данные...");
};
	
/// Помещает на вход кадр данных
void VWidthCorruptDetector::Put(const VRtParameter& in_data)
{	
	//ШК		
	width_variate.Put(VWidthExtractor()(in_data));
	while (width_variate.Ready())
	{
		width_excess_detector.Input(width_variate.Front(), VRangeCondition(pro::interval<double>(0, DBL_MAX), pro::interval<double>(0, DBL_MAX)));
		while (width_excess_detector.Ready())
		{
			VExcess e=width_excess_detector.Front();
			pro::interval<double> range(std::min<double>(e.GetStart(),e.GetEnd()),std::max<double>(e.GetStart(),e.GetEnd()));
			if (e.GetMax()>WIDTH_CORRUPT_DETECTION_VALUE && e.GetLength()<WIDTH_CORRUPT_DETECTION_LENGTH)
			{
				//Пока всегда отбраковываем ШК
				for (size_t i=0;i<input.size();++i)
				{						
					if (range.contains(input.at(i).absCoord))
						input.at(i).width.corrupt=true;
				}
			}
			width_excess_detector.Pop();
		}
		width_variate.Pop();
	}
	
	//Задержка
	input.push_back(in_data);
	if (fabs(input.front().absCoord-input.back().absCoord)>WIDTH_CORRUPT_DETECTION_DELAY)
	{
		transfer<VRtParameter>(input,output);
	}
}

/// Возвращает true, если данные готовы
bool VWidthCorruptDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если обработанных данных нет
bool VWidthCorruptDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент данных
const VRtParameter& VWidthCorruptDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент данных
void VWidthCorruptDetector::Pop()
{	
	output.pop_front();
}

/// Обрабатывает остаток данных на входе
void VWidthCorruptDetector::Flush()
{
	while (!input.empty())
	{
		transfer<VRtParameter>(input,output);
	}
}


//----------------------------------------- Методы класса VSwitchDetector ------------------------------------------

/// Конструктор
VSwitchDetector::VSwitchDetector(){}

/// Деструктор
VSwitchDetector::~VSwitchDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VSwitchDetector: Необработанные данные...");
};
	
/// Помещает на вход кадр данных
void VSwitchDetector::Put(const VRtParameter& in_data)
{
	/*
		Оптические стрелки привязаны к борту вагона. То есть одна и та же стрелка при движении с положительной скоростью
		будет распознана как левая, а при движении с отрицательной скоростью - как правая. Поэтому пр и движении с отрицательной
		скоростью стороны нужно инвертировать. Оптика обнаруживает остряк стрелки. Остряк на левой нити соответствует правой стрелке 
		и наоборот.
	*/
	if (in_data.expandedButtons.swoLeft)
	{
		SITUATION.AddSwitch(in_data.absCoord, in_data.speed<0 ? SD_RIGHT : SD_LEFT);
	}	
	else if (in_data.expandedButtons.swoRight)
	{
		SITUATION.AddSwitch(in_data.absCoord, in_data.speed<0 ? SD_LEFT : SD_RIGHT);
	}
			
	input.push_back(in_data);
	if (fabs(input.front().absCoord-input.back().absCoord)>OPTICAL_SWITCH_LENGTH)
	{
		transfer<VRtParameter>(input,output);
	}
}

/// Возвращает true, если данные готовы
bool VSwitchDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если обработанных данных нет
bool VSwitchDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент данных
const VRtParameter& VSwitchDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент данных
void VSwitchDetector::Pop()
{	
	output.pop_front();
}

/// Обрабатывает остаток данных на входе
void VSwitchDetector::Flush()
{
	while (!input.empty())
	{
		transfer<VRtParameter>(input,output);
	}
}

//----------------------------------------- Методы класса VWearCalculator ------------------------------------------

/// Конструктор
VWearCalculator::VWearCalculator(){}

/// Деструктор
VWearCalculator::~VWearCalculator() 
{
	if (!Empty()) 
		LOG_ERROR(L"VWearCalculator: Необработанные данные...");
};
	
/// Помещает на вход кадр данных
void VWearCalculator::Put(const VRtParameter& in_record)
{
	output.push_back(in_record);
	if (in_record.expandedButtons.curve==SD_RIGHT && DETECT_OPTIONS.UseWearLeft())	
	{	
		if (DETECT_OPTIONS.GetOption(L"WEAR_SOURCE").Compare(OPTION_PPR))
		{
			output.back().wear=in_record.ppr.wearLeft;		
		}
		else // стандарт
		{
			output.back().wear=in_record.wearLeft;
			if (SITUATION.P50(in_record.absCoord) && DETECT_OPTIONS.GetOption(L"USE_P50_CORRECTION").Compare(OPTION_YES))
				output.back().wear.value-=P50_CORRECTION_VALUE;
		}
	}
	else if (in_record.expandedButtons.curve==SD_LEFT && DETECT_OPTIONS.UseWearRight())
	{		
		if (DETECT_OPTIONS.GetOption(L"WEAR_SOURCE").Compare(OPTION_PPR))
		{
			output.back().wear=in_record.ppr.wearRight;		
		}
		else // стандарт
		{
			output.back().wear=in_record.wearRight;	
			if (SITUATION.P50(in_record.absCoord) && DETECT_OPTIONS.GetOption(L"USE_P50_CORRECTION").Compare(OPTION_YES))
				output.back().wear.value-=P50_CORRECTION_VALUE;
		}
	}
	else
	{
		output.back().wear.value=0;
		output.back().wear.corrupt=false;
	}
}

/// Возвращает true, если данные готовы
bool VWearCalculator::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если обработанных данных нет
bool VWearCalculator::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент данных
const VRtParameter& VWearCalculator::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент данных
void VWearCalculator::Pop()
{	
	output.pop_front();
}

//----------------------------------------- Методы класса VZazorCalculator ------------------------------------------

/// Конструктор
VZazorCalculator::VZazorCalculator(){}

/// Деструктор
VZazorCalculator::~VZazorCalculator() 
{
	if (!output.empty() || !input.empty())
		LOG_ERROR(L"VZazorCalculator: Необработанные данные...");
	if ( boost::accumulators::extract::count(diff_stat_left))
		LOG_DEBUG(boost::str(boost::wformat(L"Сдвиги магнитных зазоров (лв.): ср.=%f, мед.=%f, мин.=%f, макс.=%f")
		% boost::accumulators::extract::mean(diff_stat_left) % boost::accumulators::extract::median(diff_stat_left)
		% boost::accumulators::extract::min(diff_stat_left) % boost::accumulators::extract::max(diff_stat_left)));
	if (boost::accumulators::extract::median(diff_stat_right))	
		LOG_DEBUG(boost::str(boost::wformat(L"Сдвиги магнитных зазоров (пр.): ср.=%f, мед.=%f, мин.=%f, макс.=%f")
		% boost::accumulators::extract::mean(diff_stat_right) % boost::accumulators::extract::median(diff_stat_right)
		% boost::accumulators::extract::min(diff_stat_right) % boost::accumulators::extract::max(diff_stat_right)));		
};
	
/// Помещает на вход кадр данных
void VZazorCalculator::Put(const VRtParameter& in_data)
{			
	input.push_back(in_data);
	//В начале файла присутствуют нули их нужно заменить на -1
	if (input.back().zazorLeft.value==0) input.back().zazorLeft.value=-1;
	if (input.back().zazorRight.value==0) input.back().zazorRight.value=-1;
	if (fabs(input.back().absCoord-input.front().absCoord)>=10.0 && input.size()>=3 && is_odd(input.size()))
	{
		VRtParameter& center=input.at(input.size()/2);
		
		struct pred1
		{
			side_t side;	
			double absCoord;		
			pred1(side_t in_side, double in_coord) : side(in_side), absCoord(in_coord) {};
			bool operator()(const VRtParameter& in_data) const 
			{
				return (side==SD_LEFT ? in_data.zazorLeft.value>=0 : in_data.zazorRight.value>=0)
					&& fabs(in_data.absCoord-absCoord)<5.0;
			};
		};
		struct pred2
		{
			double absCoord;
			pred2(double in_coord) : absCoord(in_coord){};
			bool operator()(const VRtParameter& l, const VRtParameter& r)
			{
				return fabs(l.absCoord-absCoord) < fabs(r.absCoord-absCoord);
			};
		};
		// Если середина буфера - левый магнитный зазор и в БД нет плети...
		if (center.mzLeft.value>0 && !SITUATION.GetWobLeft(center.absCoord))
		{
			// ... ищем оптический					
			std::deque<VRtParameter>::iterator i=std::find_if(input.begin(), input.end(), pred1(SD_LEFT, center.absCoord));
			if (i!=input.end())
			{
				double diff=i->absCoord-center.absCoord;
				diff_stat_left(diff);
				//i->mzLeft.value=center.mzLeft.value;
				//center.mzLeft.value=-1;			
			}
			else
			{				
				double diff=median(diff_stat_left);				
				std::deque<VRtParameter>::iterator i=std::min_element(input.begin(), input.end(), pred2(center.absCoord+diff));				
				i->zazorLeft.value=0;
				//i->mzLeft.value=center.mzLeft.value;
				//center.mzLeft.value=-1;
			}			
		}
		// Если середина буфера - правый магнитный зазор и в БД нет плети...
		if (center.mzRight.value>0 && !SITUATION.GetWobRight(center.absCoord))
		{
			// ... ищем оптический					
			std::deque<VRtParameter>::iterator i=std::find_if(input.begin(), input.end(), pred1(SD_RIGHT, center.absCoord));
			if (i!=input.end())
			{
				double diff=i->absCoord-center.absCoord;
				diff_stat_right(diff);
				//i->mzRight.value=center.mzRight.value;
				//center.mzRight.value=-1;
			}
			else
			{				
				double diff=median(diff_stat_right);				
				std::deque<VRtParameter>::iterator i=std::min_element(input.begin(), input.end(), pred2(center.absCoord+diff));
				i->zazorRight.value=0;
				//i->mzRight.value=center.mzRight.value;
				//center.mzRight.value=-1;
			}			
		}
		transfer<VRtParameter>(input,output);
	}
}

/// Возвращает true, если данные готовы
bool VZazorCalculator::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если обработанных данных нет
bool VZazorCalculator::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент данных
const VRtParameter& VZazorCalculator::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент данных
void VZazorCalculator::Pop()
{	
	output.pop_front();
}

/// Слив данных
void VZazorCalculator::Flush()
{
	while (!input.empty())
	{
		transfer<VRtParameter>(input,output);
	}
}


//----------------------------------------- Методы класса 

/// Конструктор
VMarkerDetector::VMarkerDetector(){}

/// Деструктор
VMarkerDetector::~VMarkerDetector()
{
	if (!Empty()) 
		LOG_ERROR(L"VMarkerDetector: Необработанные данные...");
}

/// Помещает данные на вход
void VMarkerDetector::Put(const VRtParameter& in_data)
{
	output.push_back(in_data);
	if (output.back().expandedButtons.kms)
	{
		output.back().expandedButtons.SetMarker(MARKER_KM, SITUATION.GetNearestKm(output.back().absCoord).km);
		//output.back().expandedButtons.marker=MARKER_KM;
		//output.back().expandedButtons.marker_index=SITUATION.GetNearestKm(output.back().absCoord).km;
	}
	else if (output.back().rfid!=0)
	{
		output.back().expandedButtons.SetMarker(MARKER_RFID, output.back().rfid);
		//output.back().expandedButtons.marker=MARKER_RFID;
		//output.back().expandedButtons.marker_index=output.back().rfid;
	}
	else
		output.back().expandedButtons.SetMarker(MARKER_NONE, 0);
		//output.back().expandedButtons.marker=MARKER_NONE;
}

/// Возвращает true, если есть обработанные данные
bool VMarkerDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если обработанных данных нет
bool VMarkerDetector::Empty() const
{
	return output.empty();
}
	
/// Возвращает первый элемент данных
const VRtParameter& VMarkerDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент данных
void VMarkerDetector::Pop()
{
	output.pop_front();
}

//---------------------------------------- Методы класса VCurveDetector --------------------------------------

/// Конструктор
VCurveDetector::VCurveDetector(side_t in_side, double in_min_curv) : side(in_side), condition(in_side)
{
}

/// Деструктор
VCurveDetector::~VCurveDetector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VCurveDetector: Необработанные данные...");
};

/// Помещает данные на вход
void VCurveDetector::Put(const VRtParameter& in_data)
{	
	if (condition.TestValue(in_data.curv.value))
	{
		input.push_back(in_data);
	}
	else if (input.empty())
	{
		output.push_back(in_data);
	}
	else
	{
		double angle=input.front().kurs.value-input.back().kurs.value;
		double length=fabs(input.front().absCoord-input.back().absCoord);
		if (fabs(angle) > angle_t(0, OPTIONS.GetOption(L"MIN_CURVE_ANGLE").GetValue<double>(), 0).get_rad() &&
			length > OPTIONS.GetOption(L"MIN_CURVE_LENGTH").GetValue<double>())
		{
			if (input.front().absCoord<input.back().absCoord)
			{
				input.front().expandedButtons.SetMarker(MARKER_CURVE_BOUND_START, angle*10000);				
				input.back().expandedButtons.SetMarker(MARKER_CURVE_BOUND_END, angle*10000);				
			}
			else
			{
				input.front().expandedButtons.SetMarker(MARKER_CURVE_BOUND_END, angle*10000);				
				input.back().expandedButtons.SetMarker(MARKER_CURVE_BOUND_START, angle*10000);				
			}
			while (!input.empty())
			{
				input.front().expandedButtons.curve=side;
				transfer<VRtParameter>(input,output);
			}
		}
		else
		{
			Flush();
		}
		input.push_back(in_data);
	}
}

/// Возвращает true, если есть обработанные данные
bool VCurveDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если обработанных данных нет
bool VCurveDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент данных
const VRtParameter& VCurveDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент данных
void VCurveDetector::Pop()
{
	output.pop_front();
}

/// Слив данных
void VCurveDetector::Flush()
{
	while (!input.empty())
	{
		transfer<VRtParameter>(input,output);
	}
}

//----------------------------------- Методы класса VPassportStatistics -------------------------------

/// Конструктор
VPassportStatistics::VPassportStatistics()
{
	srand(time(NULL));
};	

/// Добавляет данные
void VPassportStatistics::Put(double in_parameter, double in_passport)
{
	double noise=double(rand() % 10)/100.0;
	covariate(in_parameter, covariate1=in_passport+noise);
	parameter(in_parameter);				
	passport(in_passport+noise);
};

/// Возвращает коэффициент корреляции
double VPassportStatistics::GetCorrelation() const
{
	return covariance(covariate)/(sqrt(variance(parameter))*sqrt(variance(passport)));
};

/// Возвращает размах
double VPassportStatistics::GetPassportRange() const
{
	return fabs(boost::accumulators::extract::max(passport) - boost::accumulators::extract::min(passport));
};

//------------------------------- Методы класса VBadPassportDetector ----------------------------------

/// Конструктор
VBadPassportDetector::VBadPassportDetector(){}

/// Деструктор
VBadPassportDetector::~VBadPassportDetector()
{
	if (!Empty()) 
		LOG_ERROR(L"VBadPassportDetector: Необработанные данные...");
}

/// Помещает данные на вход
void VBadPassportDetector::Put(const VRtParameter& in_data)
{	
	if (SITUATION.GetCurveOrientation(in_data.absCoord)!=SD_UNDEFINED || in_data.expandedButtons.curve!=SD_UNDEFINED)
	{
		input.push_back(in_data);
	}
	else
	{
		if (!input.empty())
		{			
			VPassportStatistics plan;			
			VPassportStatistics level;
			
			double coordinates_match_count=0;
									
			for (size_t i=0; i<input.size(); ++i)
			{								
				plan.Put(input.at(i).curv.value, rich_to_curv(SITUATION.GetPassportRich(input.at(i).absCoord)));				
				level.Put(input.at(i).level.value, SITUATION.GetLevel(input.at(i).absCoord));				
				if (SITUATION.GetCurveOrientation(input.at(i).absCoord)!=SD_UNDEFINED && input.at(i).expandedButtons.curve!=SD_UNDEFINED)
					coordinates_match_count++;
			}
			
			double coordinates_match_percent=coordinates_match_count*100.0/(input.size());
									
			VBadPassport bad_passport(input.front().absCoord, input.back().absCoord, 
				plan.GetCorrelation(), plan.GetPassportRange(), 				
				level.GetCorrelation(), level.GetPassportRange(), coordinates_match_percent);
									
			VWayCoord wc_start=SITUATION.GetRealCoord(input.front().absCoord);
			VWayCoord wc_end=SITUATION.GetRealCoord(input.back().absCoord);		
			if (bad_passport.IsBad())
			{								
				LOG_WARNING(boost::str(boost::wformat(L"%s - %s Расхождение с паспортом: План: %f Уровень: %f Координаты: %f)") 
					% wc_start.format_long() % wc_end.format_long() % plan.GetCorrelation() % level.GetCorrelation() % coordinates_match_percent));
				SITUATION.AddBadPassport(bad_passport);
			}														
			
			Flush();
		}
		output.push_back(in_data);
	}
}

/// Возвращает true, если есть обработанные данные
bool VBadPassportDetector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если обработанных данных нет
bool VBadPassportDetector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент данных
const VRtParameter& VBadPassportDetector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент данных
void VBadPassportDetector::Pop()
{
	output.pop_front();
}

/// Слив данных
void VBadPassportDetector::Flush()
{
	while (!input.empty())
	{
		transfer<VRtParameter>(input,output);
	}
}


//------------------------------- Методы класса VCorrectionVerificator ----------------------------------

/// Конструктор
VCorrectionVerificator::VCorrectionVerificator()
{
}

/// Деструктор
VCorrectionVerificator::~VCorrectionVerificator()
{			
	LOG_WARNING(L"Оценка качества привязки координат.");
	LOG_WARNING(boost::str(boost::wformat(L"Корреляция: %f)") % plan.GetCorrelation()));
	LOG_WARNING(boost::str(boost::wformat(L"Максимальный шаг координаты: %f)") % boost::accumulators::extract::max(diff)));
	LOG_WARNING(boost::str(boost::wformat(L"Минимальный шаг координаты: %f)") % boost::accumulators::extract::min(diff)));
	LOG_WARNING(boost::str(boost::wformat(L"Средний шаг координаты: %f)") % boost::accumulators::extract::mean(diff)));
			
	if (!Empty()) 
		LOG_ERROR(L"VCorrectionVerificator: Необработанные данные...");
}

/// Помещает данные на вход
void VCorrectionVerificator::Put(const VRtParameter& in_data)
{
	output.push_back(in_data);
	plan.Put(in_data.curv.value, rich_to_curv(SITUATION.GetPassportRich(in_data.absCoord)));		
	srand(time(NULL));		
	if (!monotone.empty())
	{
		diff(in_data.absCoord-monotone.back());
	}
	monotone.push_back(in_data.absCoord);
	if (monotone.size()==3)
	{
		if (!pro::interval<double>(std::min<double>(monotone.front(),monotone.back()), std::max<double>(monotone.front(),monotone.back()), false, false).contains(monotone.at(1)))
		{
			LOG_ERROR(boost::str(boost::wformat(L"Нарушен порядок кадров в точке с координатой %f м") % monotone.at(1)));
		}
		monotone.pop_front();
	}
}

/// Возвращает true, если есть обработанные данные
bool VCorrectionVerificator::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если обработанных данных нет
bool VCorrectionVerificator::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент данных
const VRtParameter& VCorrectionVerificator::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент данных
void VCorrectionVerificator::Pop()
{
	output.pop_front();
}



//---------------------------------- Методы класса VNstKmDetector -----------------------------------

/// Конструктор
VNstKmDetector::VNstKmDetector(){};

/// Деструктор
VNstKmDetector::~VNstKmDetector(){};

/// Помещает данные на вход
void VNstKmDetector::Put(const VRtParameter& in_data)
{
	output.push_back(in_data);
	if (in_data.expandedButtons.kms)
		input.push_back(in_data);
	if (input.size()==2)
	{
		VAprinfoKmRecord km1=SITUATION.GetNearestKm(input.front().absCoord);
		VAprinfoKmRecord km2=SITUATION.GetNearestKm(input.back().absCoord);
		
		if (km1.km-km2.km==1 || km2.km-km1.km==1)
		{
			long km=std::min<long>(km1.km, km2.km);
			double apr_length=SITUATION.GetKmLength(km);
			double real_length=round(fabs(input.front().absCoord-input.back().absCoord));
			double diff=fabs(apr_length-real_length);
			if (diff>10.0)
				LOG_WARNING(boost::str(boost::wformat(L"Километр %d: отличие от априорной длины %0.0f м [%0.0f м апр. %0.0f м факт.]") 
					%km %diff %real_length %apr_length));
		}
		
		input.pop_front();
	}
}

/// Возвращает true, если есть обработанные данные
bool VNstKmDetector::Ready() const
{
	return output.size()>0;
}
/// Возвращает true, если обработанных данных нет
bool VNstKmDetector::Empty() const
{
	return output.empty();
}
/// Возвращает первый элемент данных
const VRtParameter& VNstKmDetector::Front() const
{
	return output.front();
}
/// Выталкивает первый элемент данных
void VNstKmDetector::Pop()
{
	output.pop_front();
}
/// Слив данных
void VNstKmDetector::Flush(){};

//----------------------------------------- Методы класса VWearVerificator -----------------------------------------------

/// Конструктор
VWearVerificator::VWearVerificator(){};
/// Деструктор
VWearVerificator::~VWearVerificator()
{
	if (!Empty()) 
		LOG_ERROR(L"VWearVerificator: Необработанные данные...");
};
/// Помещает данные на вход
void VWearVerificator::Put(const VRtParameter& in_data)
{	
	output.push_back(in_data);
	if (SITUATION.GetCurveStatusPlan(in_data.absCoord)==ZLINE_RADIUS)
	{
		input.push_back(in_data);		
	}
	else
	{
		if (!input.empty())
		{
			accumulator_set< double, stats<tag::mean, tag::variance, tag::count> > wear;
			for (size_t i=0;i<input.size();++i)
			{
				VPoint wear_point=VWearExtractor(SD_UNDEFINED)(input.at(i));
				if (!wear_point.GetCorrupt() && wear_point.GetValue()>=0)
					wear(wear_point.GetValue());									
			}
			if (boost::accumulators::extract::count(wear)>0)
			{
				VWayCoord start=SITUATION.GetRealCoord(std::min<double>(input.front().absCoord, input.back().absCoord));
				VWayCoord end=SITUATION.GetRealCoord(std::max<double>(input.front().absCoord, input.back().absCoord));
				double fact_wear= mean(wear)+1.5*sqrt(variance(wear));
				double passport_wear=SITUATION.GetNormaWear(input.front().absCoord);
				if (fabs(fact_wear - passport_wear)>1.0)
					LOG_WARNING(boost::str(boost::wformat(L"%s - %s износ на круговой: %0.1f (паспорт: %0.1f)") % start.format_long() % end.format_long() % fact_wear % passport_wear));
			}					
			input.clear();
		}
	}
};
/// Возвращает true, если есть обработанные данные
bool VWearVerificator::Ready() const
{
	return output.size()>0;
};	
/// Возвращает true, если обработанных данных нет
bool VWearVerificator::Empty() const
{
	return output.empty();
};		
/// Возвращает первый элемент данных
const VRtParameter& VWearVerificator::Front() const
{
	return output.front();
};
/// Выталкивает первый элемент данных
void VWearVerificator::Pop()
{
	output.pop_front();
};	

//------------------------------------------- Методы класса VSwitchesInvertor --------------------------------------------

/// Конструктор
VSwitchesInvertor::VSwitchesInvertor(){}
/// Деструктор
VSwitchesInvertor::~VSwitchesInvertor(){}
/// Помещает на вход кадр данных
void VSwitchesInvertor::Put(const VRtParameter& in_data)
{
	output.push_back(in_data);
		
	if (output.back().expandedButtons.swoLeft && output.back().speed<0)
		output.back().expandedButtons.swoRight;
	if (output.back().expandedButtons.swoRight && output.back().speed<0)
		output.back().expandedButtons.swoLeft;		
}
/// Возвращает true, если данные готовы
bool VSwitchesInvertor::Ready() const
{
	return output.size()>0;
}
/// Возвращает true, если обработанных данных нет
bool VSwitchesInvertor::Empty() const
{
	return output.empty();
}
/// Возвращает первый элемент данных
const VRtParameter& VSwitchesInvertor::Front() const
{
	return output.front();
}
/// Выталкивает первый элемент данных
void VSwitchesInvertor::Pop()
{
	output.pop_front();
}

//----------------------------------------- Методы класса VCurveCorrector ------------------------------------------

/// Конструктор
VCurveCorrector::VCurveCorrector()
{
}

/// Деструктор
VCurveCorrector::~VCurveCorrector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VCurveCorrector: Необработанные данные...");
};

/// Помещает на вход кадр данных
void VCurveCorrector::Put(const VRtParameter& in_data)
{			
	input.push_back(in_data);		
	
	// Уровень
	ZLINE_TYPE zl_level=SITUATION.GetCurveStatusLevel(in_data.absCoord);
	//На переходной заполняем буфер
	if (zl_level==ZLINE_OUTER_TRANSITION || zl_level==ZLINE_INNER_TRANSITION || zl_level==ZLINE_RADIUS)
	{
		if (!in_data.level.corrupt)
			level_points.push_back(point_t(in_data.absCoord, in_data.level.value));
	}	
	if (level_changed.put(zl_level))
	{		
		if (!level_points.empty())
		{	
			if (level_changed.prev()==ZLINE_OUTER_TRANSITION || level_changed.prev()==ZLINE_INNER_TRANSITION)
				SITUATION.CorrectLevelTransition(level_points);
			else if (level_changed.prev()==ZLINE_RADIUS)
				SITUATION.CorrectLevelRadius(level_points);
			level_points.clear();
		}	
	}	
	
	// Рихтовка и ШК
	ZLINE_TYPE zl_plan=SITUATION.GetCurveStatusPlan(in_data.absCoord);
	//На переходной заполняем буфер
	if (zl_plan==ZLINE_OUTER_TRANSITION || zl_plan==ZLINE_INNER_TRANSITION || zl_plan==ZLINE_RADIUS)
	{
		if (!in_data.richLeft.corrupt)
			rich_left_points.push_back(point_t(in_data.absCoord, in_data.richLeft.value));
		if (!in_data.richRight.corrupt)
			rich_right_points.push_back(point_t(in_data.absCoord, in_data.richRight.value));
		if (!in_data.width.corrupt)
			width_points.push_back(point_t(in_data.absCoord, in_data.width.value));
	}	
	if (plan_changed.put(zl_plan))
	{		
		if (!rich_left_points.empty())
		{	
			if (plan_changed.prev()==ZLINE_OUTER_TRANSITION || plan_changed.prev()==ZLINE_INNER_TRANSITION)
				SITUATION.CorrectRichLeftTransition(rich_left_points);
			else if (plan_changed.prev()==ZLINE_RADIUS)
				SITUATION.CorrectRichLeftRadius(rich_left_points);
			rich_left_points.clear();
		}
		if (!rich_right_points.empty())
		{	
			if (plan_changed.prev()==ZLINE_OUTER_TRANSITION || plan_changed.prev()==ZLINE_INNER_TRANSITION)
				SITUATION.CorrectRichRightTransition(rich_right_points);
			else if (plan_changed.prev()==ZLINE_RADIUS)
				SITUATION.CorrectRichRightRadius(rich_right_points);
			rich_right_points.clear();
		}
		if (!width_points.empty())
		{	
			if (plan_changed.prev()==ZLINE_OUTER_TRANSITION || plan_changed.prev()==ZLINE_INNER_TRANSITION)
				SITUATION.CorrectWidthTransition(width_points);
			else if (plan_changed.prev()==ZLINE_RADIUS)
				SITUATION.CorrectWidthRadius(width_points);
			width_points.clear();
		}
	}	
	
	if (level_points.empty() && rich_left_points.empty() && rich_right_points.empty() && width_points.empty() && 
		fabs(input.front().absCoord-input.back().absCoord)>MAX_TRANSITION_CORRECTION)
	{
		Flush();
	}	
}

/// Возвращает true, если данные готовы
bool VCurveCorrector::Ready() const
{
	return output.size()>0;
};

/// Возвращает true, если обработанных данных нет
bool VCurveCorrector::Empty() const
{
	return output.empty();
};		

/// Возвращает первый элемент данных
const VRtParameter& VCurveCorrector::Front() const
{
	return output.front();
};

/// Выталкивает первый элемент данных
void VCurveCorrector::Pop()
{
	output.pop_front();
};

/// Обрабатывает остаток данных на входе
void VCurveCorrector::Flush()
{
	while (!input.empty())
	{
		transfer<VRtParameter>(input,output);
	}
};

//----------------------------------- Методы класса VStartCorrector -----------------------------

/// Конструктор
VStartCorrector::VStartCorrector() : shift(0), skip(false) {};

/// Деструктор
VStartCorrector::~VStartCorrector() 
{
	if (!Empty()) 
		LOG_ERROR(L"VStartCorrector: Необработанные данные...");
};

/// Слив данных
void VStartCorrector::Flush(double in_shift)
{
	while (!input.empty())
	{
		input.front().absCoord-=in_shift;
		transfer<VRtParameter>(input,output);
	}
}

/// Помещает на вход кадр данных
void VStartCorrector::Put(const VRtParameter& in_data)
{	
	input.push_back(in_data);
	input.back().absCoord-=shift;
	if (skip)
	{		
		Flush();
	}
	else
	{	
		if (input.back().expandedButtons.kms)
		{	
			VWayCoord wc=SITUATION.GetRealCoord(input.back().absCoord);					
			shift+=(input.back().absCoord - SITUATION.GetNearestKm(input.back().absCoord).base);	
			skip=true;						
			Flush(shift);						
			LOG_DEBUG(boost::str(boost::wformat(L"%s коррекция старта %0.1f м") % wc.format_long() % shift));
		}	
		if (!input.empty() && fabs(input.front().absCoord-input.back().absCoord)>MAX_CORRECTION_BUFFER_LENGTH)
		{
			VWayCoord wc=SITUATION.GetRealCoord(input.back().absCoord);
			skip=true;
			Flush();
			LOG_WARNING(boost::str(boost::wformat(L"%s накоплен слишком большой буфер на старте. Коррекция ошибки старта не произведена.") % wc.format_long()));
		}
	}
}

/// Возвращает true, если данные готовы
bool VStartCorrector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если обработанных данных нет
bool VStartCorrector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент данных
const VRtParameter& VStartCorrector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент данных
void VStartCorrector::Pop()
{
	output.pop_front();
}


//-------------------------------------- Методы класса VCoordCorrectionFunction -----------------------------------------

/// Добавляет линию
void VCoordCorrectionFunction::Update(const line_t& in_line)
{
	lines.push_back(in_line);
};

/// Добавляет линию
void VCoordCorrectionFunction::Update(double in_coordinate, const line_t& in_line)
{		
	if (lines.empty())
		lines.push_back(in_line);
	else
		lines.push_back(parallel(point_t(in_coordinate, lines.back().Y(in_coordinate)), in_line));
};

/// Добавляет линию
void VCoordCorrectionFunction::Update(point_t in_point, const line_t& in_line)
{
	if (lines.empty())
		lines.push_back(in_line);
	else
		lines.push_back(parallel(in_point, in_line));
};

/// Возвращает скорректированную координату
double VCoordCorrectionFunction::Correct(double in_coordinate)
{		
	if (Valid())
		return in_coordinate-lines.back().Y(in_coordinate);
	else			
		return in_coordinate;			
};

/// Возвращает true, если коррекция возможна
bool VCoordCorrectionFunction::Valid() const 
{
	return !lines.empty();
};

/// Сбрасывает функцию
void VCoordCorrectionFunction::Reset()
{
	lines.clear();
};

/// Возвращает значение функции в точке
double VCoordCorrectionFunction::Value(double in_coordinate)  const
{
	if (Valid())
		return lines.back().Y(in_coordinate);
	else
		return 0;
};

/// Возвращает величину ошибки
double VCoordCorrectionFunction::GetError() const
{
	return lines.back().k()*1000.0;
};

/// Возвращает коэффициент поправки диаметра колеса
double VCoordCorrectionFunction::GetWheelDiameterCorrection() const
{
	return double (1000.0-GetError())/1000.0;	
};

//--------------------------------------- Методы класса VCoordCorrector -----------------------------------

/// Конструктор
VCoordCorrector::VCoordCorrector() 
{
	fail=false;
}

/// Деструктор
VCoordCorrector::~VCoordCorrector() 
{	
	if (COORD_CORRECTION_FUNCTION().Value().Valid())
	{
		COORD_CORRECTION_SLOPE().Value()=COORD_CORRECTION_FUNCTION().Value().GetError();
		LOG_INFO(boost::str(boost::wformat(L"Ошибка диаметра колеса: %f м/км") % COORD_CORRECTION_FUNCTION().Value().GetError()));
		LOG_INFO(boost::str(boost::wformat(L"Ошибка старта: %f") % -COORD_CORRECTION_FUNCTION().Value().Correct(0)));		
		LOG_INFO(boost::str(boost::wformat(L"Коэффициент поправки диаметра колеса: %f") % COORD_CORRECTION_FUNCTION().Value().GetWheelDiameterCorrection()));
	}
	if (!Empty()) 
		LOG_ERROR(L"VCoordCorrector: Необработанные данные...");
	/// Необходимо сбросить функцию коррекции
	COORD_CORRECTION_FUNCTION().Value().Reset();
};

/// Слив данных
void VCoordCorrector::Flush()
{
	while (!input.empty())
	{		
		if (COORD_CORRECTION_FUNCTION().Value().Valid()) 
			input.front().absCoord=COORD_CORRECTION_FUNCTION().Value().Correct(input.front().absCoord);		
		transfer<VRtParameter>(input,output);
	}
}

/// Помещает на вход кадр данных
void VCoordCorrector::Put(const VRtParameter& in_data)
{
	input.push_back(in_data);
	if (input.back().expandedButtons.marker!=MARKER_NONE)
	{
		/*Вычисляем ожидаемую координату для поиска маркера в БД, как скорректированную, а не измеренную. Это поможет найти маркер км, если ошибка слишком большая*/
		VWayMarker expected_marker(BAD_INDEX, COORD_CORRECTION_FUNCTION().Value().Correct(input.back().absCoord), input.back().expandedButtons.marker_index, input.back().expandedButtons.marker, 0, 0, VTime());
		VWayMarker detected_marker(BAD_INDEX, input.back().absCoord, input.back().expandedButtons.marker_index, input.back().expandedButtons.marker, 0, 0, VTime());		
		size_t index=SITUATION.FindMarker(expected_marker);
		if (index!=BAD_INDEX)
		{
			VWayMarker aprinfo_marker = SITUATION.GetMarker(index);
			VWayMarkerDeviation deviation(detected_marker, aprinfo_marker);
			if (!COORD_CORRECTION_FUNCTION().Value().Valid() && DETECT_OPTIONS.GetOption(L"USE_SAVED_CORRECTION_SLOPE").Compare(OPTION_YES))
			{									
				line_t line=parallel(point_t(deviation.GetCoord(), deviation.GetDeviation()), line_t(COORD_CORRECTION_SLOPE().Value()/1000.0, -1, 0));
				COORD_CORRECTION_FUNCTION().Value().Update(line);				
			}
			
			mml.put(point_t(deviation.GetCoord(), deviation.GetDeviation()));
			if (mml.size()>=DETECT_OPTIONS.GetOption(L"MARKER_COUNT").GetValue<size_t>())
			{				
				COORD_CORRECTION_FUNCTION().Value().Update(deviation.GetCoord(), mml.get());
			}
			LOG_DEBUG(boost::str(boost::wformat(L"Привязка: [%s] к [%s]. Поправка: %0.2f м") % detected_marker.GetDescription() % aprinfo_marker.GetDescription() % deviation.GetDeviation()));			
		}
		else
		{
			LOG_DEBUG(boost::str(boost::wformat(L"Маркер: [%d]. Не найдено соответствие в базе.") % detected_marker.GetDescription()));			
		}
		LOG_DEBUG(boost::str(boost::wformat(L"Всего маркеров: %d; буфер: %0.2f км")  % mml.size() % (fabs(input.front().absCoord-input.back().absCoord)/1000.0)));
	}
	if (COORD_CORRECTION_FUNCTION().Value().Valid())
	{
		while (!input.empty())
		{						
			input.front().absCoord=COORD_CORRECTION_FUNCTION().Value().Correct(input.front().absCoord);			
			transfer<VRtParameter>(input,output);			
		}	
	}
	// Считаем что как минимум один раз в километр должен встретиться один маркер (километровый столб), иначе коррекция отключается
	if (!input.empty() && fabs(input.front().absCoord-input.back().absCoord)>DETECT_OPTIONS.GetOption(L"MARKER_COUNT").GetValue<double>()*1000.0)
	{
		LOG_ERROR(L"Недостаточно маркеров для коррекции координат. Коррекция координат будет отключена.");
		LOG_ERROR(boost::str(boost::wformat(L"Всего маркеров: %d; буфер: %0.2f км")  % mml.size() % (fabs(input.front().absCoord-input.back().absCoord)/1000.0)));
		fail=true;
	}
	if (fail)
	{
		Flush();
	}
}

/// Возвращает true, если данные готовы
bool VCoordCorrector::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если обработанных данных нет
bool VCoordCorrector::Empty() const
{
	return output.empty();
}

/// Возвращает первый элемент данных
const VRtParameter& VCoordCorrector::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент данных
void VCoordCorrector::Pop()
{
	output.pop_front();
}

//------------------------------------ Методы класса VRichCalculator -----------------------------

/// Инициализация
void VRichCalculator::Init(const VRtParameter& in_data)
{
	if (!initialized && in_data.speed<0)
	{
		std::swap(Lb,Lf);
		//std::swap<double>(Lb,Lf);
	}
	initialized=true;
}

/// Предварительные вычисления
void VRichCalculator::InputPreprocess()
{
	double step=fabs(input.front().data.absCoord-input.back().data.absCoord)/input.size();
	//Вычисляем курс и план
	for (size_t i=0; i<input.size(); ++i)
	{
		if (i>0)
		{
			input.at(i).absCoord=input.at(i-1).absCoord+step;
			
			input.at(i).rich_left.K=input.at(i-1).rich_left.K + step * (rich_to_curv(input.at(i-1).rich_left.R, Lf, Lb) + rich_to_curv(input.at(i).rich_left.R, Lf, Lb)) / 100000.0;
			input.at(i).rich_left.X=input.at(i-1).rich_left.X + step * cos(input.at(i-1).rich_left.K);
			input.at(i).rich_left.Y=input.at(i-1).rich_left.Y + step * sin(input.at(i-1).rich_left.K)*(-1);
			
			input.at(i).rich_right.K=input.at(i-1).rich_right.K + step * (rich_to_curv(input.at(i).rich_right.R, Lf, Lb) + rich_to_curv(input.at(i).rich_right.R, Lf, Lb)) / 100000.0;
			input.at(i).rich_right.X=input.at(i-1).rich_right.X + step * cos(input.at(i-1).rich_right.K);
			input.at(i).rich_right.Y=input.at(i-1).rich_right.Y + step * sin(input.at(i-1).rich_right.K)*(-1);
		}
	}
}

/// Уточнение плана
void VRichCalculator::CalculatePlan()
{
	double S=0.6;
	double dmax=DBL_MAX;
	
	//Определяем диапазон, в котором можем вычислить уточненный план
	size_t from=0;
	size_t to=input.size()-1;
	while (from<input.size() && fabs(input.at(from).absCoord-input.front().absCoord) < deadzone)//Lf
		from++;
	while (to>0 && fabs(input.back().absCoord-input.at(to).absCoord) < deadzone)//Lb
		to--;
	size_t iteration_count=0;
	while (dmax>precision && iteration_count<30)
	{
		iteration_count++;
		dmax=0;
		//Уточняем план в этом диапазоне
		for (size_t i=from; i<=to; ++i)
		{
			size_t Ib=i;
			size_t Ic=i;
			size_t If=i;
				
			while (If>0 && fabs(input.at(If).absCoord-input.at(Ic).absCoord) < Lf)
				If--;
			while (Ib<input.size()-1 && fabs(input.at(Ic).absCoord-input.at(Ib).absCoord) < Lb)
				Ib++;
					
			line_t chord_left(point_t(input.at(If).rich_left.X, input.at(If).rich_left.Y),point_t(input.at(Ib).rich_left.X, input.at(Ib).rich_left.Y));
			input.at(Ic).rich_left.r=1000.0*length(point_t(input.at(Ic).rich_left.X, input.at(Ic).rich_left.Y), chord_left);
			
			line_t chord_right(point_t(input.at(If).rich_right.X, input.at(If).rich_right.Y),point_t(input.at(Ib).rich_right.X, input.at(Ib).rich_right.Y));
			input.at(Ic).rich_right.r=1000.0*length(point_t(input.at(Ic).rich_right.X, input.at(Ic).rich_right.Y), chord_right);
			
			dmax=std::max<double>(dmax, fabs(input.at(Ic).rich_left.dx()));
			dmax=std::max<double>(dmax, fabs(input.at(Ic).rich_left.dy()));
			dmax=std::max<double>(dmax, fabs(input.at(Ic).rich_right.dx()));
			dmax=std::max<double>(dmax, fabs(input.at(Ic).rich_right.dy()));
		}		
		for (size_t i=from; i <=to; ++i)
		{	
			input.at(i).rich_left.X+=S*input.at(i).rich_left.dx();
			input.at(i).rich_left.Y+=S*input.at(i).rich_left.dy();			
			input.at(i).rich_right.X+=S*input.at(i).rich_right.dx();
			input.at(i).rich_right.Y+=S*input.at(i).rich_right.dy();
		}		
	}
	max_iteration_count=std::max<size_t>(max_iteration_count, iteration_count);
}

/// Вычисление рихтовок
void VRichCalculator::CalculateRich()
{
	//Определяем диапазон в котором можем вычислить рихтовки
	size_t from=0;
	size_t to=input.size()-1;		
	while (from<input.size() && fabs(input.at(from).absCoord-input.front().absCoord) < 2*deadzone+1)//lf+Lf
		from++;
	while (to>0 && fabs(input.back().absCoord-input.at(to).absCoord) < 2*deadzone+1)//lb+Lb
		to--;
	
	//Вычисляем рихтовки
	for (size_t i=from; i<=to; ++i)
	{
			size_t Ib=i;
			size_t Ic=i;
			size_t If=i;
				
			while (If>0 && fabs(input.at(If).absCoord-input.at(Ic).absCoord) < lf)
				If--;
			while (Ib<input.size()-1 && fabs(input.at(Ic).absCoord-input.at(Ib).absCoord) < lb)
				Ib++;
								
			line_t chord_left(point_t(input.at(If).rich_left.X, input.at(If).rich_left.Y),point_t(input.at(Ib).rich_left.X, input.at(Ib).rich_left.Y));
			input.at(Ic).rich_left.result=1000.0*length(point_t(input.at(Ic).rich_left.X, input.at(Ic).rich_left.Y), chord_left);
			
			line_t chord_right(point_t(input.at(If).rich_right.X, input.at(If).rich_right.Y),point_t(input.at(Ib).rich_right.X, input.at(Ib).rich_right.Y));
			input.at(Ic).rich_right.result=1000.0*length(point_t(input.at(Ic).rich_right.X, input.at(Ic).rich_right.Y), chord_right);
	}
}


/// Конструктор
VRichCalculator::VRichCalculator()
{
	Lf=DETECT_OPTIONS.GetOption(L"FRONT_ARM_REAL").GetValue<double>();
	Lb=DETECT_OPTIONS.GetOption(L"BACK_ARM_REAL").GetValue<double>();
	lf=DETECT_OPTIONS.GetOption(L"FRONT_ARM_VIRTUAL").GetValue<double>();
	lb=DETECT_OPTIONS.GetOption(L"BACK_ARM_VIRTUAL").GetValue<double>();
	precision=DETECT_OPTIONS.GetOption(L"RICH_RECALCULATION_PRECISION").GetValue<double>()/1000.0;
	buffer_size=DETECT_OPTIONS.GetOption(L"RICH_RECALCULATION_BUFFER").GetValue<double>();
	deadzone=std::max(Lb, Lf) + std::max(lb, lf);
	initialized=false;
	max_iteration_count=0;
};

/// Деструктор
VRichCalculator::~VRichCalculator()
{
	if (!input.empty()) 
		LOG_ERROR(L"VRichCalculator: Необработанные данные на входе...");
	if (!output.empty()) 
		LOG_ERROR(L"VRichCalculator: Необработанные данные на выходе...");
	LOG_DEBUG(boost::str(boost::wformat(L"Максимальное число итераций при пересчете рихтовок: %d") % max_iteration_count));
}

/// Помещает на вход кадр данных
void VRichCalculator::Put(const VRtParameter& in_data)
{	
	Init(in_data);
	
	input.push_back(VRichCalcData(in_data));
	if (fabs(input.front().data.absCoord-input.back().data.absCoord) > buffer_size+4*deadzone)
	{	
		InputPreprocess();
		CalculatePlan();
		CalculateRich();
	
		//Освобождаем буфер оставляя запас для следующего расчета
		while (fabs(input.front().absCoord-input.back().absCoord) > 4*deadzone+8)
		{
			input.front().data.richLeft.value=input.front().rich_left.result;
			input.front().data.richRight.value=input.front().rich_right.result;
			output.push_back(input.front().data);
			input.pop_front();
		}
	}
};

/// Возвращает true, если данные готовы
bool VRichCalculator::Ready() const
{
	return output.size()>0;	
};

/// Возвращает true, если обработанных данных нет
bool VRichCalculator::Empty() const
{
	return output.empty();
};
	
/// Возвращает первый элемент данных
const VRtParameter& VRichCalculator::Front() const
{
	return output.front().data;
};

/// Выталкивает первый элемент данных
void VRichCalculator::Pop()
{
	output.pop_front();
};

/// Слив данных
void VRichCalculator::Flush()
{
	if (fabs(input.front().data.absCoord-input.back().data.absCoord) > 2*deadzone)
	{	
		InputPreprocess();
		CalculatePlan();
		CalculateRich();
	}
	//Освобождаем буфер оставляя запас для следующего расчета
	while (!input.empty())
	{
		input.front().data.richLeft.value=input.front().rich_left.result;
		input.front().data.richRight.value=input.front().rich_right.result;
		output.push_back(input.front().data);
		input.pop_front();
	}
};

//------------------------------------ Методы класса VOverstep3 --------------------------------------------

VOverstep3::VOverstep3() 
{
	memset(this, 0, sizeof(*this));
}

VOverstep3::VOverstep3(VBasicOverstepPtr ovr) :
absCoord(ovr->GetCoord()), begin(ovr->GetStart()), end(ovr->GetEnd()),
km(ovr->GetWayCoord().km), m(ovr->GetWayCoord().m),
value(ovr->GetValue()), length(ovr->GetLength()), degree(ovr->GetDegree()), 
spd1(ovr->GetSpeeds().GetPassSpeed()), spd2(ovr->GetSpeeds().GetTruckSpeed()), 
spd3(ovr->GetSpeeds().GetEmptySpeed()), spd_hi(ovr->GetSpeeds().GetHighSpeed()),
spd_hi_bt(ovr->GetSpeeds().GetHighBTSpeed()), rst1(ovr->GetRestriction().GetPassSpeed()), 
rst2(ovr->GetRestriction().GetTruckSpeed()), rst3(ovr->GetRestriction().GetEmptySpeed()), 
rst_hi(ovr->GetRestriction().GetHighSpeed()), rst_hi_bt(ovr->GetRestriction().GetHighBTSpeed()),
hasRestriction(ovr->HasRestriction()), amount(ovr->GetAmount()), penalty(ovr->GetPenalty()), 
closeToUpper(ovr->CloseToUpperBound()), is_excluded(false), hi_speeds(true)
{
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
		this->cp515_revision = 1;
	else
		this->cp515_revision = 2;

	if (ovr->IsExcluded())
	{
		this->is_excluded = true;
		this->SetExclusionReason(ovr->GetExclusionReason());
	}
	else
	{
		memset(this->excl_reason, 0, sizeof(this->excl_reason));
	}
	memset(this->description, 0, sizeof(this->description));
	memset(this->comment, 0, sizeof(this->comment));
	memset(this->type, 0, sizeof(this->type));
	memset(this->reserve, 0, sizeof(this->reserve));
	memset(this->reserve0, 0, sizeof(this->reserve0));

	this->SetType(ovr->GetTypeString());
	this->SetComment(ovr->GetComment());
	this->SetDescription(ovr->GetDescription());

	if (this->cp515_revision < 2) this->penalty = 0;
}

bool operator < (const VOverstep3 & o1, const VOverstep3 & o2)
{
    return o1.absCoord < o2.absCoord;
}

// Список отступлений - копирования из блока
bool VOvr3List::CopyFrom(const VOvr3List & src, const VWaySection * section)
{
	this->Unload();
	for (size_t i = 0; i < src.GetCount(); ++i)
	{
		if (src.GetItem(i).MeetsSection(section))
			this->AddItem(src.GetItem(i));
	}
	this->Sort();
	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
}

// Список отступлений - копирования из блока
bool VOvr3List::CopyFrom(const std::vector<VBasicOverstepPtr> & src)
{
	this->Unload();
	for (size_t i = 0; i < src.size(); ++i)
	{
		this->AddItem(VOverstep3(src.at(i)));
	}
	this->Sort();
	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
}

//------------------------------------ Методы класса VPenaltyCalc --------------------------------------------

static const std::wstring static_cp515_restrictions[20] = 
{ 
    WIDEN, NARROW, PIT_LEFT, PIT_RIGHT, LEVEL, DISTORTION, RICH_LEFT, RICH_RIGHT, RICH, RICH_NR,
    RICH_PIT_COMBINATION, RICH_DISTORTION_COMBINATION, RICH_GROUP, RICH_GROUP_NR, RICH_LEFT_GROUP, RICH_RIGHT_GROUP,
    PIT_LEFT_GROUP, PIT_RIGHT_GROUP, ANP, OTVOD_LEVEL
};

static const std::wstring static_cp515_degree_3[8] = 
{
    PIT_LEFT, PIT_RIGHT, LEVEL, DISTORTION, RICH_LEFT, RICH_RIGHT, RICH, RICH_NR
};

static const std::wstring static_cp515_degree_2[10] = 
{
    PIT_LEFT, PIT_RIGHT, LEVEL, DISTORTION, RICH_LEFT, RICH_RIGHT, RICH, RICH_NR
};

// конструктор
VPenaltyCalc::VPenaltyCalc() : penalty(0), crv_penalty(0), len_coeff(1.0), classic_penalty(false),
cp515_restrictions(static_cp515_restrictions, static_cp515_restrictions + 20),
cp515_degree_3(static_cp515_degree_3, static_cp515_degree_3 + 8),
cp515_degree_2(static_cp515_degree_2, static_cp515_degree_2 + 10)
{}

// копирующий конструктор
VPenaltyCalc::VPenaltyCalc(const VPenaltyCalc & src) : penalty(src.penalty), crv_penalty(src.crv_penalty), 
len_coeff(src.len_coeff), classic_penalty(src.classic_penalty),
cp515_restrictions(static_cp515_restrictions, static_cp515_restrictions + 20),
cp515_degree_3(static_cp515_degree_3, static_cp515_degree_3 + 8),
cp515_degree_2(static_cp515_degree_2, static_cp515_degree_2 + 10)
{}

// оператор присваивания
VPenaltyCalc & VPenaltyCalc::operator = (const VPenaltyCalc & src)
{
    penalty = src.penalty;
    crv_penalty = src.crv_penalty;
    len_coeff = src.len_coeff;
    classic_penalty = src.classic_penalty;
    return (*this);
}

void VPenaltyCalc::Apply(const VAprinfo & apr, bool is_excluded, 
						 double coord, __int64 km, 
						 const std::wstring & type, long p)
{
	if (is_excluded) return;
	if (p == 0) return;

	double km_len = apr.GetKmLength(km);
	if (km_len == 0) return;
	//this->len_coeff = 1000.0 / km_len;

	// anp: ANP, ANP_HORIZONTAL
	if (type == ANP || type == ANP_HORIZONTAL || type == ANP_AVERAGE)
	{
		if (!apr.IsPassportCurvePlan(coord)) return;

		__int64 id = apr.GetPassportCurveId(coord);
		if (id <= 0) return;

		bool found = false;
		for (size_t i = 0; i < this->crv_id_anp.size(); ++i)
		{
			if (this->crv_id_anp[i] == id)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			this->penalty += p;
			this->crv_penalty += p;
			this->crv_id_anp.push_back(id);
		}
	}
	// psi: PSI, PSI_HORIZONTAL, PSI_TRANSITION
	else if (type == PSI || type == PSI_HORIZONTAL || type == PSI_TRANSITION)
	{
		if (!apr.IsPassportCurvePlan(coord)) return;

		__int64 id = apr.GetPassportCurveId(coord);
		if (id <= 0) return;

		bool found = false;
		for (size_t i = 0; i < this->crv_id_psi.size(); ++i)
		{
			if (this->crv_id_psi[i] == id)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			this->penalty += p;
			this->crv_penalty += p;
			this->crv_id_psi.push_back(id);
		}
	}
	// oh:  OTVOD_LEVEL
	else if (type == OTVOD_LEVEL)
	{
		if (!apr.IsPassportCurvePlan(coord)) return;

		__int64 id = apr.GetPassportCurveId(coord);
		if (id <= 0) return;

		bool found = false;
		for (size_t i = 0; i < this->crv_id_oh.size(); ++i)
		{
			if (this->crv_id_oh[i] == id)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			this->penalty += p;
			this->crv_penalty += p;
			this->crv_id_oh.push_back(id);
		}
	}
	// нормальные отступления
	else
	{
		this->penalty += p;
	}
}

void VPenaltyCalc::Apply(const VAprinfo & apr, VBasicOverstepPtr ovr)
{
	this->Apply(apr, ovr->IsExcluded(), ovr->GetCoord(), ovr->GetWayCoord().km, ovr->GetType(), ovr->GetPenalty());
}

void VPenaltyCalc::Apply(const VAprinfo & apr, const VOverstep3 & ovr)
{
	this->Apply(apr, ovr.is_excluded, ovr.absCoord, ovr.km, ovr.type, ovr.penalty);
}

// Классическая балловая оценка километра
void VPenaltyCalc::CalcOldStylePenalty(long min_spd1, size_t restriction_count, size_t deg2_count, size_t deg3_count)
{
	if (min_spd1 > 60)
	{
		if (restriction_count > 0)	this->penalty = 500;
		else if (deg3_count > 6)	this->penalty = 500;
		else if (deg2_count > 60)   this->penalty = 500;
		else if (deg3_count > 0)	this->penalty = 150;
		else if (deg2_count > 25)	this->penalty = 150;
		else if (deg2_count > 5)	this->penalty = 40;
		else						this->penalty = 10;
	}
	else
	{
		if (restriction_count > 0)	this->penalty = 500;
		else if (deg3_count > 3)	this->penalty = 500;
		else if (deg3_count > 0)	this->penalty = 150;
		else if (deg2_count > 12)	this->penalty = 150;
		else if (deg2_count > 3)	this->penalty = 40;
		else						this->penalty = 10;
	}
	this->crv_penalty = 0;
	this->classic_penalty = true;
}


