#pragma once

#include "os.h"
#include "types.h"
#include "VMemoryMappedFile.h"
#include "VException.h"
#include "VFileName.h"
#include <deque>
#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>

/*
ToDo!

1. Заменить wchar_t на std::wstring

*/

/** Текущая версия файла*/
#define	FILE_VERSION			3L
/** Текущая версия структуры заголовка файла*/
#define	DFH_VERSION				1L
/** Текущая структура заголовка блока данных*/
#define	BDH_VERSION				1L

/** Сигнатура файла данных*/
#define	DATAFILE_SIGN "C4F"
/** Сигнатура блока данных*/
#define	DATABLOCK_SIGN	"DATABLK"
/** Размер сигнатуры файла*/
#define SIGNATURE_SIZE 4
/** Размер заголовка файла*/
#define HEADER_SIZE 64
/** Размер, под пользовательский заголовок*/
#define USER_HEADER_SIZE 1020
/** Начало первого блока*/
#define FIRST_BLOCK_OFFSET SIGNATURE_SIZE+HEADER_SIZE+USER_HEADER_SIZE

#pragma pack (push, 1)
/** Заголовок файла данных*/
struct VDataFileHeader
{
	VDataFileHeader()
	{
		memset(this, 0, sizeof(VDataFileHeader));		
	}
	/** Версия формата файла*/
	uint32_t FileVer;
	/** Размер заголовка*/
	uint32_t HdrSize;
	/** Версия структуры заголовка файла*/
	uint32_t HdrVer;
	/** Версия заголовка блока данных*/
	uint32_t BlockDataVer;	
	/** Резерв*/
	uint8_t reserve1[8];
	/** Размер пользовательского заголовка*/
	uint32_t FileHdrSize;
	/** Резерв*/
	uint8_t reserve2[36];
};

/** Заголовок блока данных*/
struct VDataBlockHeader
{
	/** Размер заголовка блока данных*/
	uint32_t HdrSize;
	/** Размер блока данных*/
	uint32_t DataSize;
	/** Тип данных*/
	uint32_t Type;
	/** Флаг используемых полей*/
	uint32_t Flags;
	/** Количество записей*/
	uint32_t RecCount;
	/** Размер записи*/
	uint32_t RecSize;
	/** Делитель*/
	uint32_t Divider;
	/** Аналог С типа*/
	uint32_t CType;
	/** Смещение*/
	int32_t Offset;
	/** Версия структуры в массиве*/
	uint32_t RecVer;
	/** Дата создания блока*/
	VTime CreateTime;
	/** Дата изменения блока*/
	VTime UpdateTime;
	/** Флаги индивидуальные для каждого блока данных*/
	uint32_t BlockFlags;
	/** Контрольная сумма блoка данных*/
	uint32_t crc32;
	/** Индекс блока данного типа*/
	uint32_t Index;
	/** Резерв*/
	uint8_t reserve2[36];
	/** Сигнатура для поиска и проверки блоков*/
	char cSign[8];
};
#pragma pack (pop)

/** Блок данных*/
class VDataBlock
{
public:
	/** Заголовок*/
	VDataBlockHeader * Header;
	/** Данные*/
	unsigned char * Data;
	/** Проекция*/
	void * Map;
	/** Смещение в файле*/
	size_t Offset;
	/** Копия заголовка*/	
	VDataBlockHeader HeaderCopy;
};


/** Файл данных
 структура файла данных примерно такая:
 Общая часть
 - сигнатура
 - заголовок файла
 Пользовательская часть
 - пользовательский заголовок
 - блоки данных*/
class VBaseDataFile
{
private:

	//Копирущий конструктор закрыт!
	VBaseDataFile(const VBaseDataFile &);
	//Оператор присваивания закрыт!
	VBaseDataFile & operator=(const VBaseDataFile &);

protected:

	//Данные

	/** Файл, спроецированный в память*/
	VMemoryMappedFile mmfile;
	/** Список блоков*/	
	std::deque<VDataBlock> DataBlocks;

	/** Заголовочная часть*/
	unsigned char * lpHeaderData;
	//Данные в заголовочной части
	/** Сигнатура*/
	char * Signature;
	/** Заголовок файла*/
	VDataFileHeader * FileHeader;
	/** Указатель на пользовательский заголовок (в потомках должен быть приведен к определенному типу)*/
	void * UserHeader;
	/** Признак того что файл загружен*/
	bool IsLoaded;
	/** Имя открытого файла*/
	std::wstring file_name;

	//Методы

	/** Проецирует заголовочную часть*/
	bool MapHeaders();
	/** Удаляет проекцию заголовочной части*/
	bool UnmapHeaders();	
	/** Загружает список блоков*/
	bool InitBlockList();
	/** Инициализирует заголовки*/
	bool InitHeaders();
	/** Устанавливает сигнатуру.*/
	void SetSignature();
	/** Проверяет сигнатуру.*/
	bool CheckSignature();
	/** Проверяет тип файла*/
	virtual bool CheckFileType();

public:

	//Методы

	/** Конструктор*/
	VBaseDataFile();
	/** Деструктор*/
	virtual ~VBaseDataFile();

	/** Связывает объект с дисковым файлом*/
	void Open(const wchar_t * const szFileName, uint32_t dwMode=OPEN_MODE_READ);

	/** То же самое, через VFileName*/
	void Open(const VFileName & fileName, uint32_t dwMode=OPEN_MODE_READ)
	{
		this->Open(fileName.GetPath().c_str(), dwMode);
	}

	/** Закрытие файла*/
	void Close();

	/** Возвращает указатель на заголовок*/
	VDataFileHeader * GetFileHeader();
	/** Возвращает указатель на пользовательский заголовок*/
	void * GetUserHeader();
	/** Возвращает имя файла, с которым связан объект*/
	const wchar_t * GetFileName();

	//Операции с блоками

	/** Возвращает число блоков*/
	size_t GetBlockCount();
	/** Возвращает число блоков заданного типа*/
	size_t GetBlockCountByType(uint32_t type);
	/** Удаляет блок данных по индексу*/
	void DeleteBlock(size_t pos);	
	/** Добавляет блок данных в указанную позицию*/
	virtual void AddBlock(const VDataBlockHeader * const header, const void * const data, size_t pos);	
	/** Добавляет блок данных в конец файла*/
	virtual void AddBlock(const VDataBlockHeader * const header, const void * const data);
	/** Возвращает блок данных по индексу*/
	VDataBlock * GetBlock(size_t pos);
	/** Возвращает индекс блока по типу и номеру*/
	virtual size_t GetBlockIndex(uint32_t in_type, size_t in_index=0);	
	/** Возвращает признак того, что файл загружен*/
	bool Loaded();
	/** Возвращает признак того, что блок спроецирован*/
	bool BlockMapped(size_t pos);
	/** Проецирует блок (возвращает спроецированный блок)*/
	VDataBlock * MapBlock(size_t pos);
	/** Удаляет проекцию блока*/
	void UnmapBlock(size_t pos);
	/** Удаляет проекции всех блоков*/
	void UnmapAllBlocks();
};
typedef boost::shared_ptr<VBaseDataFile> VBaseDataFilePtr;

/*

Код, приводящей к ошибке:

size_t lastndx=Target.GetBlockCount()-1;
VDataBlock * lastblk=Target.GetBlock(lastndx);
Target.DeleteBlock(lastndx-1);
lastblk->Header->stUpdate.wMilliseconds=0;//Падаем!!!

то есть удаление блока приводит к разрушению объектов, полученных ранее. То же самое будет и в том случае, если мы просто 
закроем файл. Избежать этого похоже не удастся, значит необходима хоть какая-то защита. Например, можно скрыть данные
класса VDataBlock и осуществить доступ к ним через функции, которые вызывали бы исключение при попытке запросить не
валидные данные.

class VDataBlock
{
private:
	//Признак валидности
	bool Valid;
	//Заголовок
	VDataBlockHeader * Header;
	// Данные
	unsigned char * Data;
	// Проекция
	void * Map;
	// Смещение в файле
	size_t Offset;
public:
	VDataBlockHeader * GetHeader()
	{
		if (!Valid)
			throw VSimpleException(L"shit");
		return Header;
	}
};

Итак, тезисы:
- Постоянно держать спроецированные в память файлы и работать напрямую с ними нежелательно.
- Необходимо загружать данные в память обрабатывать и освобождать по мере необходимости.
- Проецирование в память может значительно ускорить операции с файлами, такие как изменение размера,
  копирование данных из одной части файла в другую.

*/

