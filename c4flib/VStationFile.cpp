#include "VStationFile.h"
#include "datatypes.h"

#include <boost/format.hpp>

/** Проверяет тип файла*/
bool VStationFile::CheckFileType()
{
	//Проверяем тип файла по наличию в нем априорной информации
	if (GetBlockIndex(DHT_STATION_APRINFO)==BAD_INDEX)
		return false;
	else
		return true;
}

/** Возвращает указатель на пользовательский заголовок*/
VStationFileHeader * VStationFile::GetUserHeader()
{
	return static_cast<VStationFileHeader*>(UserHeader);
}


/** Функция генерации имени файла*/
const std::string GenFileName(uint32_t in_station_code, std::string in_way_code)
{
	long t = (long) time(0);
	std::string result=boost::str(boost::format("St%d_%s_%d.s4f") % in_station_code % in_way_code % t);
	return result;
}