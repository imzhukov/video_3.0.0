#pragma once

#include "options.h"
#include "ExchangeCadre.h"

class VCoordinatesRecord
{
public:
	VCoordinatesRecord();
	VCoordinatesRecord(const VCoordinatesRecord& record)
	{
		stringData = record.stringData;
		dpp = record.dpp;
		km = record.km;
		m = record.m;
		start_time = 0;
		strcpy(way, record.way);
		strcpy(peregon, record.peregon);
	}
	std::wstring stringData;
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
	///���������� ���� ������ �� ���������
	void SetDefaultValues();
};

DECLARE_VARIABLE_MT(VCoordinatesRecord, CURRENT_POSITION, VCoordinatesRecord())