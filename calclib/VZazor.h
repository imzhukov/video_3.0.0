#pragma once

#include "VParameter.h"
#include "VCorrupt.h"

struct VZazor
{
	uint32_t index;
	float value;

	bool UpdateIndex(const VAbsCoordData & destAbs, const VAbsCoordData & srcAbs)
	{
		return destAbs.UpdateIndex(this->index, srcAbs);
	}
};

	
enum TYPE_FINDING
{
	NEXT_ZAZOR,			//ищем следующий за магнитным оптический зазор
	PREVIOUS_ZAZOR,		//ищем предыдущий магнитному оптический зазор
	NEXT_2_ZAZOR,		//ищем следующий 2ой за магнитным оптический зазор
	PREVIOUS_2_ZAZOR,	//ищем предыдущий 2ой магнитному оптический зазор
	CLOSE_ZAZOR			//ищем ближайший магнитному оптический в радиусе, указанном в функции поиска
};

class VZazorList : public VIndexList<VZazor>
{
	VParameterCorruptPtr corrupt;

public:

	/// Конструктор
	VZazorList() : VIndexList<VZazor>(), corrupt(new VParameterCorrupt)
	{}

	/// Выгрузка
	void Unload()
	{
		VIndexList<VZazor>::Unload();
		this->kadrCount = 0;
	}

	/// Загрузка из файла
	bool Load(VMainFile & file, uint32_t type, VParameterCorruptPtr corr);

	/// Загрузка из файла
	bool Load(VRemoteFile & remote, uint32_t type, VParameterCorruptPtr corr);

	/// Загрузка из параметра
	bool Load(const VParameter & prm, VParameterCorruptPtr corr);

	/// Сохранение в файл
	bool Save(VMainFile & file, uint32_t type);

	/// Копирование из другого списка, с пересчетом координат
	bool CopyFrom(const VZazorList & src, 
		const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs);

	/// Функция поиска зазоров в указанном диапазоне вокруг указываемого кадра
	VZazorList FindZazor(uint32_t index, double range, const VAbsCoordData & absCoordData);

	/// Функция поиска зазоров следующего за магнитным (магнитный в кадре index)
	VZazorList FindZazor(uint32_t index, TYPE_FINDING type);

	/// Вернуть сбойность указанного зазора (номер зазора, не кадра!!!!)
	bool IsCorrupt(size_t index) const;

	/// Установить сбойность указанного зазора (номер зазора, не кадра!!!!)
	void SetCorrupt(size_t index, bool value);
};