#pragma once

#include <string>

#include "c4flib.h"
#include "VParameter.h"
#include "VAprinfo.h"
#include "VPlanElemList.h"
#include "error_messages.h"

enum SAREA_STATUS
{
	SAS_OK = 0,			// Ок
	SAS_NO_DATA = 1,	// Неполные измерения
	SAS_EXCLUDED = 2	// Исключенный участок (боковой путь или непромеренная кривая)
};

struct VStatData
{
	double absBegin, absEnd;	// границы в абс.координатах
	VWaySection section;		// границы в путевых координатах
	short km, pk;				// номер км и пк; для км-листа pk==0

	// Всего точек (априорно), посчитанных точек, исключенных точек)
	size_t pntSum, pntCalc, pntEx;

	// Статус области
	SAREA_STATUS GetStatus() const;

	float moWidth, sskoWidth; // ШК
	float moLevel, sskoLevel; // Уровень
	float moLevelDiff, sskoLevelDiff; // Перекос 
	float moPitLeft, sskoPitLeft; // Пр.лв
	float moPitRight, sskoPitRight; // Пр.пр
	float moRichLeft, sskoRichLeft; // Рихтовка левая
	float moRichRight, sskoRichRight; // Рихтовка правая

	bool havePPR; // на участке расчитаны величины ППР
	float moPrLeft, sskoPrLeft; // Перекос рельса левый
	float moPrRight, sskoPrRight; // Перекос рельса правый
	float moNgrLeft, sskoNgrLeft; // Наклон головки рельса левый
	float moNgrRight, sskoNgrRight; // Наклон головки рельса правый

	float sigma; // Сигма
	float sssp; // СССП

	float maxLevel, minLevel; // макс и мин уровня
};

class VStatList : public VIndexList<VStatData>
{
public:

	VStatData Find(short km, short pk);
};

class VStatistics
{
	VStatList pkList, kmList;
	std::wstring message;

public:

	bool Load(const VAprinfo & aprInfo, const VAbsCoordData & absCoord, 
		const VParameter & level, const VParameter & level_zline, 
		const VParameter & pitLeft, const VParameter & pitRight, const VParameter & width, 
		const VParameter & richLeft, const VParameter & richRight,
		const VParameter & prLeft, const VParameter & prRight,
		const VParameter & ngrLeft, const VParameter & ngrRight,
		const VPlanElemList & planElemList);

	VStatList & GetPkList()
	{
		return this->pkList;
	}

	const VStatList & GetPkList() const
	{
		return this->pkList;
	}

	VStatList & GetKmList()
	{
		return this->kmList;
	}

	const VStatList & GetKmList() const
	{
		return this->kmList;
	}

	const std::wstring & GetErrorMessage() const 
	{
		return this->message;
	}

	bool IsLoaded() const
	{
		return this->pkList.GetCount() != 0 && this->kmList.GetCount() != 0;
	}

	void Unload()
	{
		this->kmList.Unload();
		this->pkList.Unload();
	}
};