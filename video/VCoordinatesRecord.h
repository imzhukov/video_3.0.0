#pragma once

#include "options.h"
#include "ExchangeCadre.h"

/*class VCoordinatesRecord
{
public:
	VCoordinatesRecord();
	VCoordinatesRecord(const VCoordinatesRecord& record)
	{
		dir = record.dir;
		dpp = record.dpp;
		km = record.km;
		m = record.m;
		start_time = 0;
		strcpy(way, record.way);
		strcpy(peregon, record.peregon);
	}
	unsigned long dpp;
	double absolutePosition;
	long km;
	long m;
	///�����������
	long dir;
	///����
	char way[8];
	///�������
	char peregon [80];
	///����� ������ �������
	int64_t start_time;
	///���������� �������� ��� ����������� ���������� (-1 - �������� ����������; 1 - ����������� ����������; 0 - ������������� ��������)
	short increaseAbscoord;
	///���������� ���� ������ �� ���������
	void SetDefaultValues();
};*/

DECLARE_VARIABLE_MT(CoordAnswer, CURRENT_POSITION, CoordAnswer())