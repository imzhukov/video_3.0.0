#pragma once

#include "parameters.h"
#include "VBasicParameter.h"

extern const uint32_t
ERROR_NONE,			// нет ошибки
ERROR_ABS_COORD,	// аваpия ДПП
ERROR_WIDTH,		// аваpия шаблона
ERROR_LEVEL,		// аваpия уровня
ERROR_DIST_SHORT,	// аваpия перекоса короткого
ERROR_DIST_LONG,	// аваpия перекоса коpоткого
ERROR_GRADIENT,		// аваpия уклона
ERROR_PIT_LEFT,		// аваpия просадки короткой левой
ERROR_PIT_RIGHT,	// аваpия просадки короткой правая		
ERROR_WIDTH_FRONT,	// авария "шк спереди"
ERROR_HEIGHT,		// аваpия высоты
ERROR_RICH_LEFT,	// аваpия рихтовки короткой левой
ERROR_RICH_RIGHT,	// аваpия рихтовки короткой правой
ERROR_CURV,			// аваpия кривизны
ERROR_KURS,			// аваpия курса		
ERROR_WIDTH_BACK,	// авария "шк сзади"		
ERROR_WEAR,			// авария объединенного износа (от ПЭВМ-1)
ERROR_WEAR_LEFT,	// авария износа левого
ERROR_WEAR_RIGHT,	// авария износа правого
ERROR_PPR,			// авария полного профиля
ERROR_ZAZOR_LEFT,	// авария левого зазора
ERROR_ZAZOR_RIGHT,	// авария правого зазора
ERROR_PPR_NEW,      // новые каналы полного профиля (4 износа)
ERROR_MAG_ZAZOR_LEFT,	// авария левого магнитного зазора
ERROR_MAG_ZAZOR_RIGHT;	// авария правого магнитного зазора

class VParameterCorrupt
{
	std::vector<bool> data;

public:

	void Create(size_t kadrCount)
	{
		this->data = std::vector<bool>(kadrCount, false);
	}

	void Unload()
	{
		this->data.clear();
	}

	size_t GetCount() const
	{
		return this->data.size();
	}

	bool IsLoaded() const
	{
		return !this->data.empty();
	}

	bool IsCorrupt(size_t index) const
	{
		return this->data.at(index);
	}

	void SetCorrupt(size_t index, bool value)
	{
		this->data.at(index) = value;
	}

	void CopyFrom(const VParameterCorrupt & src, size_t kadrFrom, size_t kadrTo)
	{
		this->data.resize(kadrTo - kadrFrom + 1);
		std::copy(src.data.begin() + kadrFrom, src.data.begin() + kadrTo, this->data.begin());
	}

	void Shift(int shift);
};

typedef boost::shared_ptr<VParameterCorrupt> VParameterCorruptPtr;

class VCorruptData
{
	// классическая отбраковка - нужна только для
	VBasicParameter corrupt;

	VParameterCorruptPtr absCoord, width, widthFront, widthBack, 
		level, distShort, distLong, grad, pitLeft, pitRight, richLeft, richRight, 
		curv, kurs, wearSensor, wearLeft, wearRight, ppr, pprNew,
		zazLeft, zazRight, mzLeft, mzRight;

	void CreateBlocks(size_t kadrCount);

	bool isLoaded;
	std::wstring message;

public:

	/// Конструктор
	VCorruptData();

	/// Загрузка из шаблонного файла
	template <class FILETYPE> bool Load(FILETYPE & file);

	/// Данные загружены
	bool IsLoaded() const
	{
		return this->isLoaded;
	}

	/// Установить флаг о загрузке данных
	void SetLoaded(bool value)
	{
		this->isLoaded = value;
	}

	/// Сообщение об ошибке
	const std::wstring & GetErrorMessage() const
	{
		return this->message;
	}

	/// Данные загружены в память
	bool IsAlloc() const
	{
		return this->corrupt.IsAlloc();
	}

	/// Копирование из другого блока
	bool CopyFrom(const VCorruptData & src, size_t kadrFrom, size_t kadrTo);

	/// Выгрузка данных
	void Unload();

	/// Сохранение в файл
	bool Save(VMainFile & file);

	/// Вернуть канал отбраковок
	VParameterCorruptPtr GetCorruptChannel(PARAMETER_TYPE param) const;

	/// Создать
	void Create(uint32_t kadrCount, bool manualBlock);

	// ---- работа напрямую с блоком "стандартной отбраковки"

	/// Значение из 32-битного поля
	void SetCorruptValue(size_t kadrNdx, uint32_t value)
	{
		this->corrupt.GetItem<uint32_t>(kadrNdx) = value;
	}

	/// Значение из двух 16-битный полей
	void SetCorruptValue(size_t kadrNdx, uint16_t value1, uint16_t value2)
	{
		this->SetCorruptValue(kadrNdx, (uint32_t) value1 + 65536ul * (uint32_t) value2);
	}

	/// Число записей
	size_t GetCount() const
	{
		return this->corrupt.GetRecCount();
	}

	//Ссылка на i-ый элемент
	uint32_t & GetItem(size_t i)
	{
		return this->corrupt.GetItem<uint32_t>(i);
	}

};


