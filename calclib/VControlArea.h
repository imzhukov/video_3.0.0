#pragma once

#include "VParameter.h"

/// Информация об одном параметре - структура в файле
#pragma pack (push,1)
struct VControlAreaParam
{
	/// норматив, мат.ожидание, ско
	float norma, mo, sko;

	/// конструтор
	VControlAreaParam() : norma(0.0f), mo(0.0f), sko(0.0f)
	{}
};
#pragma pack (pop)

/// Информация об участке - структура в файле
#pragma pack (push,1)
struct VFileControlArea
{
	/// версия записи
	int version;

	/// шк и уровень
	VControlAreaParam width, level;

	/// границы участка (в оригинале тут стоит старинная WAYSECTION)
	short kmn;
	float mn;
	short kmk;
	float mk;

	/// резерв
	unsigned char reserve[88];

	/// конструктор
	VFileControlArea() : version(1)
	{
		ZeroMemory(reserve, sizeof(reserve));
	}
};
#pragma pack (pop)

/// Информация об участке - структура в памяти
struct VControlArea
{
	/// версия записи
	int version;

	/// шк и уровень
	VControlAreaParam width, level;

	/// участок
	VWaySection section;

	/// Импорт из структуры на диске
	bool Import(const VFileControlArea & record);

	/// Экспорт
	VFileControlArea Export() const;

	/// отступление укладывается в участок
	bool MeetsSection(const VWaySection * section) const
	{
		return section == NULL || 
			!(this->section.end < section->start || this->section.start > section->end);
	}

	// Участок вписывается в границы проезда, по числу кадров
	bool Fits(size_t kadrCount) const { return true; }
};

/// Список контрольных участков
class VControlAreaList : public VIndexList<VControlArea>
{
public:

	/// конструктор
	VControlAreaList() : VIndexList<VControlArea>()
	{}

	/// загрузка из файла
	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		return this->DefaultLoad<FILETYPE, VFileControlArea>(file, type, kadrCount);
	}

	/// сохранение в файл
	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VFileControlArea>(file, type);
	}

	/// Копирование списка
	bool CopyFrom(const VControlAreaList & src, const VWaySection * section);
};