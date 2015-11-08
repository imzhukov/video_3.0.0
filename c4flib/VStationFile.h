#pragma once

#include "stdio.h"
#include <math.h>
#include "VBaseDataFile.h"


#pragma pack(push,1)
/** ���������������� ��������� ����������� �����*/
struct VStationFileHeader
{				
	/** �����������*/
	VStationFileHeader()
	{
		memset(this, 0, sizeof(VStationFileHeader));
	}
	/** ���� �������*/
	VTime stRailDate;		
	/** ����� ������*/
	uint32_t dwRoadNum;
	/** ������������� (����� ������)*/
	char cDivision[16];			
	/** ��� �������*/
	uint32_t dwStationCode;				
	/** �������� �������*/
	char cStationName[256];							
	/** �������� �����*/
	char cParkName[256];	
	/** ���� �������� �����*/
	VTime stCreate;		
	/** ���� ��������� �����*/
	VTime stUpdate;	
	/** ������*/
	char cReserve[436];
};
#pragma pack(pop)

/** ���� ���������� ����������� �����*/
class VStationFile : public VBaseDataFile
{
	/*
		���������� �� ��������:
		- ���������� 'C4S'
		- ����� ����������������� ��������� 
		- ����� ���� ��������� ������ ������ ������ ����, �� ���������� ������������ ����� ������ �������
		  ���� ����� ��������
	*/
	private:		
		/** ��������� ����������� ������!*/
		VStationFile(VStationFile&){};
		/** �������� ������������ ������!*/
		VStationFile & operator=(VStationFile&){};
		/** ��������� ��� �����*/
		virtual bool CheckFileType();
	public:
		/** ����������� �� ���������*/
		VStationFile(){};
		/** ���������� ��������� �� ���������������� ���������*/
		VStationFileHeader * GetUserHeader();
};

/** ������� ��������� ����� �����*/
const std::string GenFileName(uint32_t in_station_code, std::string in_way_code);