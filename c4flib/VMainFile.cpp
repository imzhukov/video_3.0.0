#include "VMainFile.h"
#include "datatypes.h"

/** Проверяет тип файла*/
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

/** Возвращает указатель на пользовательский заголовок*/
VMainFileHeader * VMainFile::GetUserHeader()
{
	return static_cast<VMainFileHeader*>(UserHeader);
}

/** Добавляет блок данных в указанную позицию*/
void VMainFile::AddBlock(const VDataBlockHeader * const header, const void * const data, size_t pos)
{
	//Необходмо запретить возможность добавления двух блоков одного типа
	if (GetBlockIndex(header->Type)==BAD_INDEX)
		VBaseDataFile::AddBlock(header, data, pos);
}

/** Добавляет блок данных в конец файла*/
void VMainFile::AddBlock(const VDataBlockHeader * const header, const void * const data)
{
	//Необходмо запретить возможность добавления двух блоков одного типа
	if (GetBlockIndex(header->Type)==BAD_INDEX)
		VBaseDataFile::AddBlock(header, data);
}

/** Возвращает индекс блока по типу и номеру*/
size_t VMainFile::GetBlockIndex(uint32_t type)
{
	return VBaseDataFile::GetBlockIndex(type, 0);
}
