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
	///Направление
	long dir;
	///Путь
	char way[8];
	///Перегон
	char peregon [80];
	///Время старта поездки
	int64_t start_time;
	///Установить поля класса по умолчанию
	void SetDefaultValues();
};

DECLARE_VARIABLE_MT(VCoordinatesRecord, CURRENT_POSITION, VCoordinatesRecord())