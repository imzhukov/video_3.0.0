#pragma once
#include <boost/asio.hpp>

#include "stdlib.h"
#include <string>

#ifdef _WIN32
	#include "windows.h"
#else //Linux
	#include "wchar.h"
	#include "unistd.h"
	#include "sys/mman.h"
	#include "sys/types.h"
	#include "sys/stat.h"
	#include "fcntl.h"
	#include "stdio.h"	
	#include "string.h"
	#include "errno.h"
	extern int errno;
#endif //_WIN32


/** Открыть только для чтения*/
#define OPEN_MODE_READ 1
/** Открыть существующий файл для записи и чтения*/
#define OPEN_MODE_WRITE 2
/** Создать новый файл*/
#define OPEN_MODE_CREATE 3

/** Интерфейс для работы с файлом, проецируемым на память*/
class IMemoryMappedFile
{
public:	
	/** Открывает файл*/
	virtual bool Open(const wchar_t * const szFileName, unsigned long OpenMode=OPEN_MODE_READ)=0;
	/** Закрывает файл*/
	virtual bool Close()=0;
	/** Устанавливает размер файла*/
	virtual bool SetFileSize(size_t size)=0;
	/** Возвращает размер файла*/
	virtual size_t GetFileSize()=0;
	/** Возвращает гранулярность*/
	virtual size_t GetGranularity()=0;
	/** Удаляет кусок файла*/
	virtual bool DeletePart(size_t StartOffset, size_t EndOffset)=0;
	/** Вставляет кусок в файл*/
	virtual bool InsertPart(size_t StartOffset, size_t PartSize)=0;
	/** Проецирует часть файла*/
	virtual void * MapView(size_t FileOffsetHigh, size_t FileOffsetLow, size_t NumberOfBytesToMap)=0;
	/** Удаляет проекцию*/
	virtual bool UnmapView(void * address)=0;	
	/** Возвращает признак загруженности файла*/
	virtual bool IsLoaded()=0;	
	/** Возвращает режим открытия файла*/
	virtual unsigned long GetOpenMode()=0;
	/** Возвращает имя загруженного файла*/
	virtual const wchar_t * const GetFileName()=0;
	/** Возвращает требуемый размер окна проекции, с учетом гранулярности выделения памяти*/
	virtual size_t AlignSizeToGranularity(size_t req)=0;
};


#ifdef _WIN32 //Реализация WIN32

/** Класс для работы с файлом, проецируемым на память версия для WIN32*/
class VMemoryMappedFile : public IMemoryMappedFile
{
private:

	//Копирующий конструктор закрыт!*/
	VMemoryMappedFile(VMemoryMappedFile &){};
	//Опреатор присваивания закрыт!
	VMemoryMappedFile & operator=(VMemoryMappedFile &){};

protected:
	/** HANDLE файла*/
	HANDLE File;
	/** HANDLE проекции*/
	HANDLE MappedFile;	
	/** Размер файла*/
	size_t FileSize;
	/** Имя загруженного файла*/
	std::wstring FileName;
	/** Режим работы (чтение/запись)*/
	unsigned long OpenMode;
	/** Признак загруженности файла*/
	bool Loaded;
	/** Гранулярность выделения памяти*/
	size_t Granularity;	

	//Методы
	
	/** Создает проекцию*/
	bool CreateMapping(size_t dwSize=0);		

public:

	/** Конструктор*/
	VMemoryMappedFile();
	/** Деструктор*/
	virtual ~VMemoryMappedFile();

	//Методы
	
	/** Открывает файл*/
	bool Open(const wchar_t * const filename, unsigned long mode=OPEN_MODE_READ);
	/** Закрывает файл*/
	bool Close();
	/** Устанавливает размер файла*/
	bool SetFileSize(size_t size);
	/** Возвращает размер файла*/
	size_t GetFileSize();
	/** Возвращает гранулярность*/
	size_t GetGranularity();
	/** Удаляет кусок файла*/
	bool DeletePart(size_t StartOffset, size_t EndOffset);
	/** Вставляет кусок в файл*/
	bool InsertPart(size_t StartOffset, size_t PartSize);
	/** Проецирует часть файла*/
	void * MapView(size_t FileOffsetHigh, size_t FileOffsetLow, size_t NumberOfBytesToMap);
	/** Удаляет проекцию*/
	bool UnmapView(void * address);	
	/** Возвращает признак загруженности файла*/
	bool IsLoaded();	
	/** Возвращает режим открытия файла*/
	unsigned long GetOpenMode();
	/** Возвращает имя загруженного файла*/
	const wchar_t * const GetFileName();
	/** Возвращает требуемый размер окна проекции, с учетом гранулярности выделения памяти*/
	size_t AlignSizeToGranularity(size_t req);
};

#else //Реализация Linux

/** Класс для работы с файлом, проецируемым на память версия для WIN32*/
class VMemoryMappedFile : public IMemoryMappedFile
{
protected:
	/** Дескриптор файла*/
	int File;	
	/** Размер файла*/
	size_t FileSize;
	/** Имя загруженного файла*/
	wchar_t * FileName;
	/** Режим работы (чтение/запись)*/
	unsigned long OpenMode;
	/** Признак загруженности файла*/
	bool Loaded;
	/** Гранулярность выделения памяти*/
	size_t Granularity;	

	//Методы
		
public:

	/** Конструктор*/
	VMemoryMappedFile();
	/** Деструктор*/
	virtual ~VMemoryMappedFile();

	//Методы
	
	/** Открывает файл*/
	bool Open(const wchar_t * const filename, unsigned long mode);
	/** Закрывает файл*/
	bool Close();
	/** Устанавливает размер файла*/
	bool SetFileSize(size_t size);
	/** Возвращает размер файла*/
	size_t GetFileSize();
	/** Возвращает гранулярность*/
	size_t GetGranularity();
	/** Удаляет кусок файла*/
	bool DeletePart(size_t StartOffset, size_t EndOffset);
	/** Вставляет кусок в файл*/
	bool InsertPart(size_t StartOffset, size_t PartSize);
	/** Проецирует часть файла*/
	void * MapView(size_t FileOffsetHigh, size_t FileOffsetLow, size_t NumberOfBytesToMap);
	/** Удаляет проекцию*/
	bool UnmapView(void * address);	
	/** Возвращает признак загруженности файла*/
	bool IsLoaded();	
	/** Возвращает режим открытия файла*/
	unsigned long GetOpenMode();
	/** Возвращает имя загруженного файла*/
	const wchar_t * const GetFileName();
	/** Возвращает требуемый размер окна проекции, с учетом гранулярности выделения памяти*/
	size_t AlignSizeToGranularity(size_t req);
};

#endif //_WIN32
