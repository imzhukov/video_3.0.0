#pragma once

#include "stdio.h"
#include <math.h>
#include "VBaseDataFile.h"


#pragma pack(push,1)
/** Пользовательский заголовок станционных путей*/
struct VStationFileHeader
{				
	/** Конструктор*/
	VStationFileHeader()
	{
		memset(this, 0, sizeof(VStationFileHeader));
	}
	/** Дата поездки*/
	VTime stRailDate;		
	/** Номер дороги*/
	uint32_t dwRoadNum;
	/** Подразделение (номер вагона)*/
	char cDivision[16];			
	/** Код станции*/
	uint32_t dwStationCode;				
	/** Название станции*/
	char cStationName[256];							
	/** Название парка*/
	char cParkName[256];	
	/** Дата создания файла*/
	VTime stCreate;		
	/** Дата изменения файла*/
	VTime stUpdate;	
	/** Резерв*/
	char cReserve[436];
};
#pragma pack(pop)

/** Файл параметров станционных путей*/
class VStationFile : public VBaseDataFile
{
	/*
		Отличается от базового:
		- сигнатурой 'C4S'
		- типом пользовательского заголовка 
		- может быть несколько блоков данных одного типа, но необходимо соответствие числа блоков каждого
		  типа числу проездов
	*/
	private:		
		/** Копирущий конструктор закрыт!*/
		VStationFile(VStationFile&){};
		/** Оператор присваивания закрыт!*/
		VStationFile & operator=(VStationFile&){};
		/** Проверяет тип файла*/
		virtual bool CheckFileType();
	public:
		/** Конструктор по умолчанию*/
		VStationFile(){};
		/** Возвращает указатель на пользовательский заголовок*/
		VStationFileHeader * GetUserHeader();
};

/** Функция генерации имени файла*/
const std::string GenFileName(uint32_t in_station_code, std::string in_way_code);