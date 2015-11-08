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


/** ������� ������ ��� ������*/
#define OPEN_MODE_READ 1
/** ������� ������������ ���� ��� ������ � ������*/
#define OPEN_MODE_WRITE 2
/** ������� ����� ����*/
#define OPEN_MODE_CREATE 3

/** ��������� ��� ������ � ������, ������������ �� ������*/
class IMemoryMappedFile
{
public:	
	/** ��������� ����*/
	virtual bool Open(const wchar_t * const szFileName, unsigned long OpenMode=OPEN_MODE_READ)=0;
	/** ��������� ����*/
	virtual bool Close()=0;
	/** ������������� ������ �����*/
	virtual bool SetFileSize(size_t size)=0;
	/** ���������� ������ �����*/
	virtual size_t GetFileSize()=0;
	/** ���������� �������������*/
	virtual size_t GetGranularity()=0;
	/** ������� ����� �����*/
	virtual bool DeletePart(size_t StartOffset, size_t EndOffset)=0;
	/** ��������� ����� � ����*/
	virtual bool InsertPart(size_t StartOffset, size_t PartSize)=0;
	/** ���������� ����� �����*/
	virtual void * MapView(size_t FileOffsetHigh, size_t FileOffsetLow, size_t NumberOfBytesToMap)=0;
	/** ������� ��������*/
	virtual bool UnmapView(void * address)=0;	
	/** ���������� ������� ������������� �����*/
	virtual bool IsLoaded()=0;	
	/** ���������� ����� �������� �����*/
	virtual unsigned long GetOpenMode()=0;
	/** ���������� ��� ������������ �����*/
	virtual const wchar_t * const GetFileName()=0;
	/** ���������� ��������� ������ ���� ��������, � ������ ������������� ��������� ������*/
	virtual size_t AlignSizeToGranularity(size_t req)=0;
};


#ifdef _WIN32 //���������� WIN32

/** ����� ��� ������ � ������, ������������ �� ������ ������ ��� WIN32*/
class VMemoryMappedFile : public IMemoryMappedFile
{
private:

	//���������� ����������� ������!*/
	VMemoryMappedFile(VMemoryMappedFile &){};
	//�������� ������������ ������!
	VMemoryMappedFile & operator=(VMemoryMappedFile &){};

protected:
	/** HANDLE �����*/
	HANDLE File;
	/** HANDLE ��������*/
	HANDLE MappedFile;	
	/** ������ �����*/
	size_t FileSize;
	/** ��� ������������ �����*/
	std::wstring FileName;
	/** ����� ������ (������/������)*/
	unsigned long OpenMode;
	/** ������� ������������� �����*/
	bool Loaded;
	/** ������������� ��������� ������*/
	size_t Granularity;	

	//������
	
	/** ������� ��������*/
	bool CreateMapping(size_t dwSize=0);		

public:

	/** �����������*/
	VMemoryMappedFile();
	/** ����������*/
	virtual ~VMemoryMappedFile();

	//������
	
	/** ��������� ����*/
	bool Open(const wchar_t * const filename, unsigned long mode=OPEN_MODE_READ);
	/** ��������� ����*/
	bool Close();
	/** ������������� ������ �����*/
	bool SetFileSize(size_t size);
	/** ���������� ������ �����*/
	size_t GetFileSize();
	/** ���������� �������������*/
	size_t GetGranularity();
	/** ������� ����� �����*/
	bool DeletePart(size_t StartOffset, size_t EndOffset);
	/** ��������� ����� � ����*/
	bool InsertPart(size_t StartOffset, size_t PartSize);
	/** ���������� ����� �����*/
	void * MapView(size_t FileOffsetHigh, size_t FileOffsetLow, size_t NumberOfBytesToMap);
	/** ������� ��������*/
	bool UnmapView(void * address);	
	/** ���������� ������� ������������� �����*/
	bool IsLoaded();	
	/** ���������� ����� �������� �����*/
	unsigned long GetOpenMode();
	/** ���������� ��� ������������ �����*/
	const wchar_t * const GetFileName();
	/** ���������� ��������� ������ ���� ��������, � ������ ������������� ��������� ������*/
	size_t AlignSizeToGranularity(size_t req);
};

#else //���������� Linux

/** ����� ��� ������ � ������, ������������ �� ������ ������ ��� WIN32*/
class VMemoryMappedFile : public IMemoryMappedFile
{
protected:
	/** ���������� �����*/
	int File;	
	/** ������ �����*/
	size_t FileSize;
	/** ��� ������������ �����*/
	wchar_t * FileName;
	/** ����� ������ (������/������)*/
	unsigned long OpenMode;
	/** ������� ������������� �����*/
	bool Loaded;
	/** ������������� ��������� ������*/
	size_t Granularity;	

	//������
		
public:

	/** �����������*/
	VMemoryMappedFile();
	/** ����������*/
	virtual ~VMemoryMappedFile();

	//������
	
	/** ��������� ����*/
	bool Open(const wchar_t * const filename, unsigned long mode);
	/** ��������� ����*/
	bool Close();
	/** ������������� ������ �����*/
	bool SetFileSize(size_t size);
	/** ���������� ������ �����*/
	size_t GetFileSize();
	/** ���������� �������������*/
	size_t GetGranularity();
	/** ������� ����� �����*/
	bool DeletePart(size_t StartOffset, size_t EndOffset);
	/** ��������� ����� � ����*/
	bool InsertPart(size_t StartOffset, size_t PartSize);
	/** ���������� ����� �����*/
	void * MapView(size_t FileOffsetHigh, size_t FileOffsetLow, size_t NumberOfBytesToMap);
	/** ������� ��������*/
	bool UnmapView(void * address);	
	/** ���������� ������� ������������� �����*/
	bool IsLoaded();	
	/** ���������� ����� �������� �����*/
	unsigned long GetOpenMode();
	/** ���������� ��� ������������ �����*/
	const wchar_t * const GetFileName();
	/** ���������� ��������� ������ ���� ��������, � ������ ������������� ��������� ������*/
	size_t AlignSizeToGranularity(size_t req);
};

#endif //_WIN32
