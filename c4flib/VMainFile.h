#pragma once

#include "stdio.h"

/** Boost*/
#include <boost/shared_ptr.hpp>

#include "VBaseDataFile.h"

/** Пользовательский заголовок файла главных путей*/
#pragma pack(push,1)
struct VMainFileHeader
{
	/** Количество кадров в массивах с параметрами*/
	uint32_t ParamCount;
	/** Дата поездки*/
	VTime RailDate;
	/** Номер дороги*/
	uint32_t RoadNum;
	/** Номер направления*/
	uint32_t Dir;
	/** Номер пути*/
	char Way[4];
	/** Направление движения*/
	int8_t MoveDirection;
	/** Подразделение (номер вагона)*/
	char Division[16];
	/** Сбойные параметры*/
	uint32_t BadChannels;
	/** Резерв 1*/
	uint8_t reserv1[4];
	/** Источник данных*/
	char DataSource[256];
	/** Версия программы - источника данных*/
	uint32_t SourceVersion;
	/** Название перегона/парка*/
	char Peregon[256];
	/** Название направления/станции*/
	char DirName[256];
	/** Дата создания файла*/
	VTime CreateTime;
	/** Дата изменения файла*/
	VTime UpdateTime;
	/** Координаты начала и конца*/
	VWaySection WaySection;
	/** Код станции*/
	uint32_t StCode;
	/** Длинный путь*/
	char WayLong[32];
	/** Широта*/
	float Latitude;
	/**Долгота*/
	float Longitude;
	/** Резерв 2*/
	unsigned char reserv2[95];
};
#pragma pack(pop)

/** Файл параметров главных путей*/
class VMainFile : public VBaseDataFile
{
	/*
		Отличается от базового:		
		- типом пользовательского заголовка 
		- тем, что допускает не более одного блока данных каждого типа
	*/
	private:		

		/** Копирущий конструктор закрыт!*/
		VMainFile(VMainFile&){};
		/** Оператор присваивания закрыт!*/
		VMainFile & operator=(VMainFile&){};
		/** Проверяет тип файла*/
		virtual bool CheckFileType();

	public:		
		/** Конструктор по умолчанию*/
		VMainFile(){};
		/** Возвращает указатель на пользовательский заголовок*/
		VMainFileHeader * GetUserHeader();		
		/** Добавляет блок данных в указанную позицию*/
		void AddBlock(const VDataBlockHeader * const header, const void * const data, size_t pos);		
		/** Добавляет блок данных в конец файла*/
		void AddBlock(const VDataBlockHeader * const header, const void * const data);
		/** Возвращает индекс блока по типу*/
		size_t GetBlockIndex(uint32_t type);		
};
typedef boost::shared_ptr<VMainFile> VMainFilePtr;

////
//// Новый блок данных информации о поездке должен включать в себя след. информацию:
//// -------------------------------------------------------------------------------------
//// Дата поездки. Дата создания файла. Дата изменения файла.
//// Количество кадров. Направление движения (+/-). Знак скорости (+/-).
//// Источник данных (сеть/файл). Версия источника данных.
//// Номер, название дороги.
//// Подразделение (номер вагона). ФИО начальника вагона (смены)
//// На магистральных путях: код и название направления, код пути, перегон.
//// На станционных путях: код и название станции, название парка, код пути.
//// Границы файла в путевых координатах.
//// Границы файла в географических координатах.
////
