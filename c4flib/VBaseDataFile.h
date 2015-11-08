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

1. �������� wchar_t �� std::wstring

*/

/** ������� ������ �����*/
#define	FILE_VERSION			3L
/** ������� ������ ��������� ��������� �����*/
#define	DFH_VERSION				1L
/** ������� ��������� ��������� ����� ������*/
#define	BDH_VERSION				1L

/** ��������� ����� ������*/
#define	DATAFILE_SIGN "C4F"
/** ��������� ����� ������*/
#define	DATABLOCK_SIGN	"DATABLK"
/** ������ ��������� �����*/
#define SIGNATURE_SIZE 4
/** ������ ��������� �����*/
#define HEADER_SIZE 64
/** ������, ��� ���������������� ���������*/
#define USER_HEADER_SIZE 1020
/** ������ ������� �����*/
#define FIRST_BLOCK_OFFSET SIGNATURE_SIZE+HEADER_SIZE+USER_HEADER_SIZE

#pragma pack (push, 1)
/** ��������� ����� ������*/
struct VDataFileHeader
{
	VDataFileHeader()
	{
		memset(this, 0, sizeof(VDataFileHeader));		
	}
	/** ������ ������� �����*/
	uint32_t FileVer;
	/** ������ ���������*/
	uint32_t HdrSize;
	/** ������ ��������� ��������� �����*/
	uint32_t HdrVer;
	/** ������ ��������� ����� ������*/
	uint32_t BlockDataVer;	
	/** ������*/
	uint8_t reserve1[8];
	/** ������ ����������������� ���������*/
	uint32_t FileHdrSize;
	/** ������*/
	uint8_t reserve2[36];
};

/** ��������� ����� ������*/
struct VDataBlockHeader
{
	/** ������ ��������� ����� ������*/
	uint32_t HdrSize;
	/** ������ ����� ������*/
	uint32_t DataSize;
	/** ��� ������*/
	uint32_t Type;
	/** ���� ������������ �����*/
	uint32_t Flags;
	/** ���������� �������*/
	uint32_t RecCount;
	/** ������ ������*/
	uint32_t RecSize;
	/** ��������*/
	uint32_t Divider;
	/** ������ � ����*/
	uint32_t CType;
	/** ��������*/
	int32_t Offset;
	/** ������ ��������� � �������*/
	uint32_t RecVer;
	/** ���� �������� �����*/
	VTime CreateTime;
	/** ���� ��������� �����*/
	VTime UpdateTime;
	/** ����� �������������� ��� ������� ����� ������*/
	uint32_t BlockFlags;
	/** ����������� ����� ��o�� ������*/
	uint32_t crc32;
	/** ������ ����� ������� ����*/
	uint32_t Index;
	/** ������*/
	uint8_t reserve2[36];
	/** ��������� ��� ������ � �������� ������*/
	char cSign[8];
};
#pragma pack (pop)

/** ���� ������*/
class VDataBlock
{
public:
	/** ���������*/
	VDataBlockHeader * Header;
	/** ������*/
	unsigned char * Data;
	/** ��������*/
	void * Map;
	/** �������� � �����*/
	size_t Offset;
	/** ����� ���������*/	
	VDataBlockHeader HeaderCopy;
};


/** ���� ������
 ��������� ����� ������ �������� �����:
 ����� �����
 - ���������
 - ��������� �����
 ���������������� �����
 - ���������������� ���������
 - ����� ������*/
class VBaseDataFile
{
private:

	//��������� ����������� ������!
	VBaseDataFile(const VBaseDataFile &);
	//�������� ������������ ������!
	VBaseDataFile & operator=(const VBaseDataFile &);

protected:

	//������

	/** ����, ��������������� � ������*/
	VMemoryMappedFile mmfile;
	/** ������ ������*/	
	std::deque<VDataBlock> DataBlocks;

	/** ������������ �����*/
	unsigned char * lpHeaderData;
	//������ � ������������ �����
	/** ���������*/
	char * Signature;
	/** ��������� �����*/
	VDataFileHeader * FileHeader;
	/** ��������� �� ���������������� ��������� (� �������� ������ ���� �������� � ������������� ����)*/
	void * UserHeader;
	/** ������� ���� ��� ���� ��������*/
	bool IsLoaded;
	/** ��� ��������� �����*/
	std::wstring file_name;

	//������

	/** ���������� ������������ �����*/
	bool MapHeaders();
	/** ������� �������� ������������ �����*/
	bool UnmapHeaders();	
	/** ��������� ������ ������*/
	bool InitBlockList();
	/** �������������� ���������*/
	bool InitHeaders();
	/** ������������� ���������.*/
	void SetSignature();
	/** ��������� ���������.*/
	bool CheckSignature();
	/** ��������� ��� �����*/
	virtual bool CheckFileType();

public:

	//������

	/** �����������*/
	VBaseDataFile();
	/** ����������*/
	virtual ~VBaseDataFile();

	/** ��������� ������ � �������� ������*/
	void Open(const wchar_t * const szFileName, uint32_t dwMode=OPEN_MODE_READ);

	/** �� �� �����, ����� VFileName*/
	void Open(const VFileName & fileName, uint32_t dwMode=OPEN_MODE_READ)
	{
		this->Open(fileName.GetPath().c_str(), dwMode);
	}

	/** �������� �����*/
	void Close();

	/** ���������� ��������� �� ���������*/
	VDataFileHeader * GetFileHeader();
	/** ���������� ��������� �� ���������������� ���������*/
	void * GetUserHeader();
	/** ���������� ��� �����, � ������� ������ ������*/
	const wchar_t * GetFileName();

	//�������� � �������

	/** ���������� ����� ������*/
	size_t GetBlockCount();
	/** ���������� ����� ������ ��������� ����*/
	size_t GetBlockCountByType(uint32_t type);
	/** ������� ���� ������ �� �������*/
	void DeleteBlock(size_t pos);	
	/** ��������� ���� ������ � ��������� �������*/
	virtual void AddBlock(const VDataBlockHeader * const header, const void * const data, size_t pos);	
	/** ��������� ���� ������ � ����� �����*/
	virtual void AddBlock(const VDataBlockHeader * const header, const void * const data);
	/** ���������� ���� ������ �� �������*/
	VDataBlock * GetBlock(size_t pos);
	/** ���������� ������ ����� �� ���� � ������*/
	virtual size_t GetBlockIndex(uint32_t in_type, size_t in_index=0);	
	/** ���������� ������� ����, ��� ���� ��������*/
	bool Loaded();
	/** ���������� ������� ����, ��� ���� ������������*/
	bool BlockMapped(size_t pos);
	/** ���������� ���� (���������� ��������������� ����)*/
	VDataBlock * MapBlock(size_t pos);
	/** ������� �������� �����*/
	void UnmapBlock(size_t pos);
	/** ������� �������� ���� ������*/
	void UnmapAllBlocks();
};
typedef boost::shared_ptr<VBaseDataFile> VBaseDataFilePtr;

/*

���, ���������� � ������:

size_t lastndx=Target.GetBlockCount()-1;
VDataBlock * lastblk=Target.GetBlock(lastndx);
Target.DeleteBlock(lastndx-1);
lastblk->Header->stUpdate.wMilliseconds=0;//������!!!

�� ���� �������� ����� �������� � ���������� ��������, ���������� �����. �� �� ����� ����� � � ��� ������, ���� �� ������ 
������� ����. �������� ����� ������ �� �������, ������ ���������� ���� �����-�� ������. ��������, ����� ������ ������
������ VDataBlock � ����������� ������ � ��� ����� �������, ������� �������� �� ���������� ��� ������� ��������� ��
�������� ������.

class VDataBlock
{
private:
	//������� ����������
	bool Valid;
	//���������
	VDataBlockHeader * Header;
	// ������
	unsigned char * Data;
	// ��������
	void * Map;
	// �������� � �����
	size_t Offset;
public:
	VDataBlockHeader * GetHeader()
	{
		if (!Valid)
			throw VSimpleException(L"shit");
		return Header;
	}
};

����, ������:
- ��������� ������� ��������������� � ������ ����� � �������� �������� � ���� ������������.
- ���������� ��������� ������ � ������ ������������ � ����������� �� ���� �������������.
- ������������� � ������ ����� ����������� �������� �������� � �������, ����� ��� ��������� �������,
  ����������� ������ �� ����� ����� ����� � ������.

*/

