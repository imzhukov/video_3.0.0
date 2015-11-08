#pragma once

#include "stdio.h"

/** Boost*/
#include <boost/shared_ptr.hpp>

#include "VBaseDataFile.h"

/** ���������������� ��������� ����� ������� �����*/
#pragma pack(push,1)
struct VMainFileHeader
{
	/** ���������� ������ � �������� � �����������*/
	uint32_t ParamCount;
	/** ���� �������*/
	VTime RailDate;
	/** ����� ������*/
	uint32_t RoadNum;
	/** ����� �����������*/
	uint32_t Dir;
	/** ����� ����*/
	char Way[4];
	/** ����������� ��������*/
	int8_t MoveDirection;
	/** ������������� (����� ������)*/
	char Division[16];
	/** ������� ���������*/
	uint32_t BadChannels;
	/** ������ 1*/
	uint8_t reserv1[4];
	/** �������� ������*/
	char DataSource[256];
	/** ������ ��������� - ��������� ������*/
	uint32_t SourceVersion;
	/** �������� ��������/�����*/
	char Peregon[256];
	/** �������� �����������/�������*/
	char DirName[256];
	/** ���� �������� �����*/
	VTime CreateTime;
	/** ���� ��������� �����*/
	VTime UpdateTime;
	/** ���������� ������ � �����*/
	VWaySection WaySection;
	/** ��� �������*/
	uint32_t StCode;
	/** ������� ����*/
	char WayLong[32];
	/** ������*/
	float Latitude;
	/**�������*/
	float Longitude;
	/** ������ 2*/
	unsigned char reserv2[95];
};
#pragma pack(pop)

/** ���� ���������� ������� �����*/
class VMainFile : public VBaseDataFile
{
	/*
		���������� �� ��������:		
		- ����� ����������������� ��������� 
		- ���, ��� ��������� �� ����� ������ ����� ������ ������� ����
	*/
	private:		

		/** ��������� ����������� ������!*/
		VMainFile(VMainFile&){};
		/** �������� ������������ ������!*/
		VMainFile & operator=(VMainFile&){};
		/** ��������� ��� �����*/
		virtual bool CheckFileType();

	public:		
		/** ����������� �� ���������*/
		VMainFile(){};
		/** ���������� ��������� �� ���������������� ���������*/
		VMainFileHeader * GetUserHeader();		
		/** ��������� ���� ������ � ��������� �������*/
		void AddBlock(const VDataBlockHeader * const header, const void * const data, size_t pos);		
		/** ��������� ���� ������ � ����� �����*/
		void AddBlock(const VDataBlockHeader * const header, const void * const data);
		/** ���������� ������ ����� �� ����*/
		size_t GetBlockIndex(uint32_t type);		
};
typedef boost::shared_ptr<VMainFile> VMainFilePtr;

////
//// ����� ���� ������ ���������� � ������� ������ �������� � ���� ����. ����������:
//// -------------------------------------------------------------------------------------
//// ���� �������. ���� �������� �����. ���� ��������� �����.
//// ���������� ������. ����������� �������� (+/-). ���� �������� (+/-).
//// �������� ������ (����/����). ������ ��������� ������.
//// �����, �������� ������.
//// ������������� (����� ������). ��� ���������� ������ (�����)
//// �� ������������� �����: ��� � �������� �����������, ��� ����, �������.
//// �� ����������� �����: ��� � �������� �������, �������� �����, ��� ����.
//// ������� ����� � ������� �����������.
//// ������� ����� � �������������� �����������.
////
