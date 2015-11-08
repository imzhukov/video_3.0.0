#pragma once

#include "c4flib.h"
#include "VCurve.h"
#include "VParameter.h"

/// Список кривых
class VCurveList : public VIndexList<VCurve>
{
	/// оператор копирования закрыт
	/// пользуйтесь методом CopyFrom()
	VCurveList & operator = (const VCurveList &);

	//Данные

	/// Индекс активной кривой
	size_t active_curve_index;
	/// Резервная копия активной кривой
	VCurve active_curve_bak;

	mutable size_t find_crv_cache_curve_index;
	mutable size_t find_crv_cache_record_index;
	mutable size_t find_lvl_cache_curve_index;
	mutable size_t find_lvl_cache_record_index;

	bool Load(const VBasicParameter & data, const VAbsCoordData & absCoord);

public:

	/// Конструктор
	VCurveList()
	{
		active_curve_index=BAD_INDEX;
		find_crv_cache_curve_index=BAD_INDEX;
		find_crv_cache_record_index=BAD_INDEX;
		find_lvl_cache_curve_index=BAD_INDEX;
		find_lvl_cache_record_index=BAD_INDEX;
	}

	/// Загрузка из файла
	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, const VAbsCoordData & absCoord);

	/// Заполнение географических координат
	bool SetGeoCoord(const VDoubleParameter & latutude, const VDoubleParameter & longitude);

	/// Запись в файл
	bool Save(VMainFile & file, uint32_t type, const VAbsCoordData & absCoord);

	/// Копировать кривые из другого списка, с перепривязкой индексов к сетке абс.к-т
	bool CopyFrom(const VCurveList & src, const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs);

	// Функции поддержки разметки

	/// Находит кривую на которой находится точка с заданным индексом
	size_t FindCrv(size_t in_index) const;
	/// Находит кривую на которой находится точка с заданным индексом
	size_t FindLvl(size_t in_index) const;
	/// Устанавливает активную кривую
	void ActivateCurve(size_t in_active_curve_index);
	/// Удаляет активную кривую
	void DeleteActiveCurve();
	/// Сбрасывает активную кривую
	void DeactivateCurve();
	/// Резервная копия активной кривой
	void BackupActiveCurve();
	/// Восстановление активной кривой из резервной копии
	void RestoreActiveCurve();
	/// Возвращает индекс активной кривой
	size_t GetActiveCurve() const;
	/// Возвращает модельную кривизну
	float AproxPlan(size_t in_index, const VAbsCoordData & absCoord) const;
	/// Возвращает модельное возвышение
	float AproxLevel(size_t in_index, const VAbsCoordData & absCoord) const;	
};

template <class FILETYPE>
bool VCurveList::Load(FILETYPE & file, uint32_t type, const VAbsCoordData & absCoord)
{
	if (this->IsLoaded()) return true;
	if (!absCoord.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	if (!file.Loaded())
	{
		this->message = ERRMSG_FILE_NOT_OPEN;
		return false;
	}

	VBasicParameter data;
	if (!data.Load(file, type, false)) 
	{
		this->message = data.GetErrorMessage();
		return false;
	}

	bool result = this->Load(data, absCoord);
	data.UnloadFile(file, type);
	data.Unload();
	return result;
}

class VAprinfo;

/// Нулевая линия 
class VZLine : public VParameter
{

	struct Element
	{
		size_t idxBegin, idxEnd;
		double valueBegin, valueEnd;
	};

public:

	/// загрузка
	/// bool level - разметка на уровне (true) или в плане (false)
	/// при aprInfo != NULL используются нормы возвышения, иначе - нули
	bool Load(const VAbsCoordData & absCoord, const VCurveList & curves, 
		bool level, const VAprinfo * aprInfo);

};


/// Аппроксимированный уровень (FixMе!!! перенести)
class VDiffLevel : public VParameter
{
public:
	/// Загрузка
	bool Load(const VCurveList& in_curves, const VParameter& in_level, const VAbsCoordData& in_abscoord, 
		const VAprinfo& in_aprinfo);
	/// Обновление
	bool Update(const VCurveList& in_curves, const VParameter& in_level, const VAbsCoordData& in_abscoord, 
		const VAprinfo& in_aprinfo, size_t in_index);
};