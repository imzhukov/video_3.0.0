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
	///Направление
	long dir;
	///Путь
	char way[8];
	///Перегон
	char peregon [80];
	///Время старта поездки
	int64_t start_time;
	///Показатель убывания или возрастания координаты (-1 - убывание координаты; 1 - возрастание координаты; 0 - непределенное значение)
	short increaseAbscoord;
	///Установить поля класса по умолчанию
	void SetDefaultValues();
};*/

DECLARE_VARIABLE_MT(CoordAnswer, CURRENT_POSITION, CoordAnswer())