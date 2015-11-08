#pragma once

#include <string>
#include "VFileName.h"

class VIBDatabase;

/** Описание файла */
struct VStationFileDescription
{
	VFileName fileName; ///< имя выходного файла

	std::wstring roadCode;	///< номер дороги
	std::wstring stationCode; ///< код станции
	std::wstring stationName; ///< название станции
	std::wstring parkName; ///< название парка
	std::wstring wayName; ///< название пути

	VFileName inmFile;	///< имя импортируемого файла

//	bool reverse; ///< переворот при импорте disabled
//	bool import; ///< занесение в базу импортированного файла

	void Clear();
	void ClearINM();
	bool IsEmpty() const;

	VStationFileDescription() 
	{
		this->Clear();
		this->ClearINM();
	}

	bool GenerateFileName(const std::wstring & folder);
};

/// Импорт измерений из файла INM в файл 3 версии
std::wstring ImportStationINM(const VStationFileDescription & descr, const std::wstring & vps);


