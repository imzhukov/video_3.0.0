#include "VMainFile.h"
#include "datatypes.h"

/** ��������� ��� �����*/
bool VMainFile::CheckFileType()
{
	if (GetBlockIndex(DHT_ABS_COORD) == BAD_INDEX)
		return false;

	if (GetUserHeader()->Dir == 0 && GetUserHeader()->StCode != 0)
		return true;

	if (GetUserHeader()->Dir != 0 && GetUserHeader()->StCode == 0)
	{
		return GetBlockIndex(DHT_APRINFO) != BAD_INDEX;
	}

	return false;	
}

/** ���������� ��������� �� ���������������� ���������*/
VMainFileHeader * VMainFile::GetUserHeader()
{
	return static_cast<VMainFileHeader*>(UserHeader);
}

/** ��������� ���� ������ � ��������� �������*/
void VMainFile::AddBlock(const VDataBlockHeader * const header, const void * const data, size_t pos)
{
	//��������� ��������� ����������� ���������� ���� ������ ������ ����
	if (GetBlockIndex(header->Type)==BAD_INDEX)
		VBaseDataFile::AddBlock(header, data, pos);
}

/** ��������� ���� ������ � ����� �����*/
void VMainFile::AddBlock(const VDataBlockHeader * const header, const void * const data)
{
	//��������� ��������� ����������� ���������� ���� ������ ������ ����
	if (GetBlockIndex(header->Type)==BAD_INDEX)
		VBaseDataFile::AddBlock(header, data);
}

/** ���������� ������ ����� �� ���� � ������*/
size_t VMainFile::GetBlockIndex(uint32_t type)
{
	return VBaseDataFile::GetBlockIndex(type, 0);
}
