#pragma once

#include "VParameter.h"

// ---------------------------------------------------------------------------
/// Флаги привязанные к индексу кадра (метки оператора)
// ---------------------------------------------------------------------------

/// пара "флаги - номер кадра" в файле
struct VFileIndexRecord
{
	uint32_t index;			// номер кадра
	union
	{
		uint32_t flags;		// побитные флаги (метки оператора, маг.зазоры)
		float value;		// значение параметра (угоны)
	};	
};

/// метки оператора
struct VOpMark
{
	uint32_t index;		// номер кадра
	bool isIgnore;		// игнорировать
	bool isBridge;		// мост
	bool isRadar;		// радар
	bool isStation;		// разд.пункт
	bool isSwitch;		// стрелка
	bool isPole;		// столб
	bool isPicket;		// пикет
	bool isOptSwLeft;	// Стрелка левая (опт.) - остряк
	bool isOptSwRight;	// Стрелка правая (опт.) - остряк
	bool isOptXLeft;	// Стрелка левая (опт.) - крестовина
	bool isOptXRight;	// Стрелка правая (опт.) - крестовина

	VOpMark() : index(0), isIgnore(false), isBridge(false), isRadar(false), isStation(false), 
		isSwitch(false), isPole(false), isPicket(false), isOptSwLeft(false),
		isOptSwRight(false), isOptXLeft(false), isOptXRight(false)
	{}

	///Возвращает текстовое описание ReviseMe!!!
	std::wstring ShortText() const
	{
		std::wstring text=L"";
		if (isBridge)
			text+=L"м";
		if (isRadar)
			text+=L"р";
		if (isStation)
			text+=L"в";
		if (isSwitch)
			text+=L"с";
		if (isPole)
			text+=L"к";
		if (isPicket)
			text+=L"п";
		if (isOptSwLeft)
			text+=L"о лв.";
		if (isOptSwRight)
			text+=L"о пр.";
		if (isOptXLeft)
			text+=L"х лв.";
		if (isOptXRight)
			text+=L"х пр.";
		return text;
	}

	///Возвращает текстовое описание
	std::wstring LongText() const
	{
		std::wstring text=L"";
		if (isBridge)
			text+=L"мост,";
		if (isRadar)
			text+=L"радар,";
		if (isStation)
			text+=L"разд. пункт,";
		if (isSwitch)
			text+=L"стрелка,";
		if (isPole)
			text+=L"км столб,";
		if (isPicket)
			text+=L"пикет,";
		if (isOptSwLeft)
			text+=L"остряк левый,";
		if (isOptSwRight)
			text+=L"остряк правый,";
		if (isOptXLeft)
			text+=L"крестовина левая,";
		if (isOptXRight)
			text+=L"крестовина правая,";
		text.erase(--text.end());
		return text;
	}

	//Возвращает индексы объектов пути
	//Для занесения в базу данных
	int DataBaseIndex() const
	{
		if (isBridge)		return 1;
		if (isRadar)		return 2;
		if (isStation)		return 3;
		if (isSwitch)		return 4;
		if (isPole)			return 5;
		if (isPicket)		return 6;
		if (isOptSwLeft)	return 7;
		if (isOptSwRight)	return 8;
		if (isOptXLeft)		return 9;
		if (isOptXRight)	return 10;
		else			    return 11;
	}

	// импорт из записи в файле
	bool Import(const VFileIndexRecord & src);

	// импорт из флагов в файле INM
	bool Import(uint8_t kms, uint8_t kms2, size_t index);

	// импорт из слитых флагов в файле INM
	bool Import(uint16_t kms, size_t index);

	// экспорт для файла c4f
	VFileIndexRecord Export() const;

	// экспорт для файла inm
	uint16_t ExportToInm() const;

	bool UpdateIndex(const VAbsCoordData & destAbs, const VAbsCoordData & srcAbs)
	{
		return destAbs.UpdateIndex(this->index, srcAbs);
	}

	// Участок вписывается в границы проезда, по числу кадров
	bool Fits(size_t kadrCount) const { return this->index < kadrCount; }
};

/// Список меток оператора
class VOpMarkList : public VIndexList<VOpMark>
{
public:

	/// Конструктор
//	VOpMarkList() : VIndexList<VOpMark>()
//	{}

	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		return this->DefaultLoad<FILETYPE, VFileIndexRecord>(file, type, kadrCount);
	}

	/// Сохранение в файл
	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VFileIndexRecord>(file, type);
	}
};

/// Магнитный зазор
struct VMagZazor
{
	uint32_t index;
	bool isLeft, isRight;

	VMagZazor() : index(0), isLeft(false), isRight(false)
	{}

	bool Import(const VFileIndexRecord & src);

	bool Import(uint8_t km2, size_t index);

	bool Import(uint16_t km, size_t index);

	VFileIndexRecord Export() const;

	uint16_t ExportToInm() const;

	bool UpdateIndex(const VAbsCoordData & destAbs, const VAbsCoordData & srcAbs)
	{
		return destAbs.UpdateIndex(this->index, srcAbs);
	}

	// Участок вписывается в границы проезда, по числу кадров
	bool Fits(size_t kadrCount) const { return this->index < kadrCount; }

};

/// Список магнитных зазоров
class VMagZazorList : public VIndexList<VMagZazor>
{
public:

	/// Конструктор
	VMagZazorList() : VIndexList<VMagZazor>()
	{}

	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		return this->DefaultLoad<FILETYPE, VFileIndexRecord>(file, type, kadrCount);
	}

	/// Сохранение в файл
	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VFileIndexRecord>(file, type);
	}

	void Shift(size_t index, bool side, int shift)
	{
		this->GetItem(index).index += shift;
	}
};

/// Угон
struct VUgon
{
	uint32_t index;
	float value;

	bool Import(const VFileIndexRecord & src);

	VFileIndexRecord Export() const;

	/// Пересчет индексов
	bool UpdateIndex(const VAbsCoordData & destAbs, const VAbsCoordData & srcAbs)
	{
		return destAbs.UpdateIndex(this->index, srcAbs);
	}

	// Участок вписывается в границы проезда, по числу кадров
	bool Fits(size_t kadrCount) const { return this->index < kadrCount; }
};

/// Список угонов
class VUgonList : public VIndexList<VUgon>
{
public:

	/// Конструктор
	VUgonList() : VIndexList<VUgon>()
	{}

	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		return this->DefaultLoad<FILETYPE, VFileIndexRecord>(file, type, kadrCount);
	}

	/// Сохранение в файл
	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VFileIndexRecord>(file, type);
	}
	
	/// Вернуть сбойность указанного зазора (номер зазора, не кадра!!!!)
	bool IsCorrupt(size_t index) const
	{
		return true;/// FixMe!!!
	}

	/// Установить сбойность указанного зазора (номер зазора, не кадра!!!!)
	void SetCorrupt(size_t index, bool value)
	{
		/// FixMe!!! формально нужен канал сбоев
	}
};

/// Метки шпал
struct VSpalaMarker
{
	uint32_t index;
	uint32_t value;

	bool Import(const VFileIndexRecord & src);

	VFileIndexRecord Export() const;

	/// Пересчет индексов
	bool UpdateIndex(const VAbsCoordData & destAbs, const VAbsCoordData & srcAbs)
	{
		return destAbs.UpdateIndex(this->index, srcAbs);
	}

	// Участок вписывается в границы проезда, по числу кадров
	bool Fits(size_t kadrCount) const { return this->index < kadrCount; }
};

/// Список угонов
class VSpalaMarkerList : public VIndexList<VSpalaMarker>
{
public:

	/// Конструктор
	VSpalaMarkerList() : VIndexList<VSpalaMarker>()
	{}

	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		return this->DefaultLoad<FILETYPE, VFileIndexRecord>(file, type, kadrCount);
	}

	/// Сохранение в файл
	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VFileIndexRecord>(file, type);
	}

	bool Shift(float value, VAbsCoordData  & abscoord);
};
