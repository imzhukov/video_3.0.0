#pragma once

#include "VDrawing.h"
#include "elements.h"

#define AVAIL_SCALES_Y_CURVE L"20/200/10"
#define AVAIL_SCALES_Y_LEVEL L"20/200/10"

/** График для карточки паспортной кривой*/
class VCurveCard : public VDrawing
{
private:
	/** Указатель на источник данных*/
	VDataSetPtr data_set;		
	/** Тип разметки*/
	PARAMETER_TYPE mark_type;
	/** Номер кривой*/
	size_t index;		
	/** Область с графиком кривизны и износа*/
	VAreaPtr plan_area;
	/** Область с графиком возвышения*/
	VAreaPtr level_area;
	/** Область с графиками непогашенного ускорения*/
	VAreaPtr anp_area;
	/** Область с отклонениями*/
	VAreaPtr diff_area;
	/** Ось для графика кривизны*/
	VAxesPtr plan_axes;	
	/** Ось для графика возвышения*/	
	VAxesPtr level_axes;	
	/** Ось для графиков непогашенного ускорения*/
	VAxesPtr anp_axes;	
	/** Ось для отклонений*/
	VAxesPtr diff_axes;
	/** Координаты*/
	VCoordinatesDataPtr coordinates;
	/** Кривизна*/
	VParameterDataPtr curv_data;
	/** Ср. кривизна*/
	VParameterDataPtr md_curv_data;	
	/** Возвышение*/
	VParameterDataPtr level_data;
	/** Ср. возвышение*/
	VParameterDataPtr md_level_data;
	/** Метки на кривизне*/
	VDataProviderPtr curv_mark_data;
	/** Метки на уровне*/
	VDataProviderPtr level_mark_data;		
	/** Непогашенное укорение пасс.*/
	VDataProviderPtr anp_pass;
	/** Непогашенное укорение груз.*/
	VDataProviderPtr anp_truck;
	/** Отклонения в плане*/
	VDataProviderPtr diff_data;

	
public:
	/** Конструктор*/
	VCurveCard(VDataSetPtr in_data_set, PARAMETER_TYPE in_mark_type, size_t in_index);
	/** Подготовка данных*/
	void Prepare(double in_width=640, double in_height=460);
};

