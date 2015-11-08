#pragma once

#include "VParameter.h"

const uint32_t REASON_SIDEWAY = 1;		///< Боковой путь
const uint32_t REASON_HALFCURVE = 2;	///< Половина кривой
const uint32_t REASON_UNDEFINED = 3;	///< Боковой Путь или Неразмеченная Кривая
const uint32_t REASON_WAY = 4;			///< Путь

/// Съезд на боковой путь - запись на диске
#pragma pack(push,1)
struct VSidewayRecord
{
	uint32_t startNdx;	///< Индекс записи в файле откуда начинается участок съезда
	uint32_t endNdx;	///< Индекс записи в файле где заканчивается участок съезда
	uint32_t reason;	/// Причина по которой участок исключается из обработки
};
#pragma pack(pop)

/// Съезд на боковой путь - запись в памяти
class VSideway
{
	uint32_t startNdx;	///< Индекс записи в файле откуда начинается участок съезда
	uint32_t endNdx;	///< Индекс записи в файле где заканчивается участок съезда
	uint32_t reason;	///< Причина по которой участок исключается из обработки

public:

	// конструктор
	VSideway() : startNdx(0), endNdx(0), reason(0)
	{}

	// импорт данных
	bool Import(const VSidewayRecord & record)
	{
		this->startNdx = record.startNdx;
		this->endNdx = record.endNdx;
		this->reason = record.reason;
		return true;
	}

	// экспорт данных
	VSidewayRecord Export() const
	{
		VSidewayRecord record;
		record.startNdx = this->startNdx;
		record.endNdx = this->endNdx;
		record.reason = this->reason;
		return record;
	}

	// обновить индексы
	bool UpdateIndex(const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs);

	// индекс кадра начала участка
	uint32_t GetStartNdx() const
	{
		return this->startNdx;
	}

	// индекс кадра начала участка
	uint32_t GetEndNdx() const
	{
		return this->endNdx;
	}

	// статус участка
	uint32_t GetReason() const
	{
		return this->reason;
	}

	// описание статуса участка
	const wchar_t * GetReasonDescr() const;

	// Участок вписывается в границы проезда, по числу кадров
	bool Fits(size_t kadrCount) const
	{
		return this->startNdx < kadrCount && 
			this->endNdx < kadrCount && 
			this->startNdx < this->endNdx;
	}
};

// список участков боковых путей
class VSidewayList : public VIndexList<VSideway>
{
public:

	/// конструктор
	VSidewayList() : VIndexList<VSideway>()
	{}

	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		return this->DefaultLoad<FILETYPE, VSidewayRecord>(file, type, kadrCount);
	}

	/// Сохранение в файл
	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VSidewayRecord>(file, type);
	}

	/// Поиск записи привязанной к указанному кадру
	size_t FindItem(size_t kadrNdx) const;
};