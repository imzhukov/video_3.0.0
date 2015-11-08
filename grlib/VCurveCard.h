#pragma once

#include "VDrawing.h"
#include "elements.h"

#define AVAIL_SCALES_Y_CURVE L"20/200/10"
#define AVAIL_SCALES_Y_LEVEL L"20/200/10"

/** ������ ��� �������� ���������� ������*/
class VCurveCard : public VDrawing
{
private:
	/** ��������� �� �������� ������*/
	VDataSetPtr data_set;		
	/** ��� ��������*/
	PARAMETER_TYPE mark_type;
	/** ����� ������*/
	size_t index;		
	/** ������� � �������� �������� � ������*/
	VAreaPtr plan_area;
	/** ������� � �������� ����������*/
	VAreaPtr level_area;
	/** ������� � ��������� ������������� ���������*/
	VAreaPtr anp_area;
	/** ������� � ������������*/
	VAreaPtr diff_area;
	/** ��� ��� ������� ��������*/
	VAxesPtr plan_axes;	
	/** ��� ��� ������� ����������*/	
	VAxesPtr level_axes;	
	/** ��� ��� �������� ������������� ���������*/
	VAxesPtr anp_axes;	
	/** ��� ��� ����������*/
	VAxesPtr diff_axes;
	/** ����������*/
	VCoordinatesDataPtr coordinates;
	/** ��������*/
	VParameterDataPtr curv_data;
	/** ��. ��������*/
	VParameterDataPtr md_curv_data;	
	/** ����������*/
	VParameterDataPtr level_data;
	/** ��. ����������*/
	VParameterDataPtr md_level_data;
	/** ����� �� ��������*/
	VDataProviderPtr curv_mark_data;
	/** ����� �� ������*/
	VDataProviderPtr level_mark_data;		
	/** ������������ �������� ����.*/
	VDataProviderPtr anp_pass;
	/** ������������ �������� ����.*/
	VDataProviderPtr anp_truck;
	/** ���������� � �����*/
	VDataProviderPtr diff_data;

	
public:
	/** �����������*/
	VCurveCard(VDataSetPtr in_data_set, PARAMETER_TYPE in_mark_type, size_t in_index);
	/** ���������� ������*/
	void Prepare(double in_width=640, double in_height=460);
};

