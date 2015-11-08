#pragma once
  
/*
Файл VParameter.h
Измеренный параметр в файле поездки
*/

//boost
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include "VBasicParameter.h"
#include "VRemoteFile.h"
#include "mathlib.h"
#include "error_messages.h"

#include "Shiny.h"
#include "parameters.h"

#include "VCorrupt.h"
#include "fft.h"

class VProgressBox;

// ---------------------------------------------------------------------------
/// Не найдено
// ---------------------------------------------------------------------------
const uint32_t NOT_FOUND = static_cast<uint32_t>(-1);

// ---------------------------------------------------------------------------
/// Базовый интерфейсный класс -- прототип для доступа к параметрам
// ---------------------------------------------------------------------------
template <class T> class VParameterProto
{
private:

	/// Данные загружены
	bool isLoaded;

protected:

	/// Сообщение об ошибке
	std::wstring message;

public:

	/// Конструктор
	VParameterProto() : message(ERRMSG_DATA_NOT_LOADED), isLoaded(false)
	{}

	/// Выгрузка списка
	void Unload()
	{
		this->message = ERRMSG_DATA_NOT_LOADED;
		this->SetLoaded(false);
	}

	/// Проверка загруженности параметра
	bool IsLoaded() const
	{
		return this->isLoaded;
	}

	/// Установить параметру флаг "загружен"
	void SetLoaded(bool value)
	{
		this->isLoaded = value;
	}

	/// Сообщение об ошибке
	const std::wstring & GetErrorMessage() const
	{
		return this->message;
	}

	/// Число записей
	virtual size_t GetCount() const = 0;

	/// Вернуть констатную ссылку на i-й элемент
	virtual const T & GetItem(size_t index) const = 0;

	/// Вернуть ссылку на i-й элемент
	virtual T & GetItem(size_t index) = 0;

	/// Вернуть ссылку на последний элемент
	T & GetLastItem()
	{
		return this->GetItem(this->GetCount()-1);
	}

	/// Вернуть константную ссылку на последний элемент
	const T & GetLastItem() const
	{
		return this->GetItem(this->GetCount()-1);
	}
};

// ---------------------------------------------------------------------------
/// Шаблонный класс, трактующий блок данных как вектор элементов T
// ---------------------------------------------------------------------------

template <class T> class VTemplateParameter : public VParameterProto<T>
{
protected:

	std::vector<T> data;	///< блок данных в файле
	uint32_t version;		///< версия параметра	

	/// блок отбраковок
	VParameterCorruptPtr corrupt;

private:

	/// Копирущий конструктор закрыт
	VTemplateParameter(const VTemplateParameter<T> &);

	/// Оператор присваивания закрыт
	VTemplateParameter<T> & operator = (const VTemplateParameter<T> &);

public:

	/// Конструктор
	VTemplateParameter() : VParameterProto<T>(), corrupt(new VParameterCorrupt)
	{
		this->version = 0;
	}

	~VTemplateParameter()
	{
		this->Unload();
	}

	/// Загрузка
	template <class F> bool Load(F & file, uint32_t type);

	/// Загрузка с цеплянием внешнего канала отбраковок
	template <class F> bool Load(F & file, uint32_t type, VParameterCorruptPtr corr);

	/// Сохранить в файл
	bool Save(VMainFile & file, uint32_t type);

	/// Версия записи в файле
	size_t GetVersion() const
	{
		return this->version;
	}

	/// Создать "пустой" параметр
	bool Create(uint32_t type, uint32_t recCount, uint32_t version, const T & defValue);

	/// Выгрузка
	void Unload()
	{
		this->data.clear();
		this->version = 0;
		VParameterProto<T>::Unload();
	}

	/// Параметр загружен в память
	bool IsAlloc() const
	{
		return true;
	}

	/// Число записей
	size_t GetCount() const
	{
		return this->data.size();
	}

	/// Вернуть константную ссылку на i-й элемент
	const T & GetItem(size_t i) const
	{
		return this->data.at(i);
	}

	/// Вернуть ссылку на i-й элемент
	T & GetItem(size_t i) 
	{
		return this->data.at(i);
	}

	/// Вернуть признак сбоя на i-м элементе
	bool IsCorrupt(size_t i) const
	{
		return this->corrupt->IsCorrupt(i);
	}

	/// Установить признак сбоя на i-м элементе
	void SetCorrupt(size_t i, bool c)
	{
		this->corrupt->SetCorrupt(i, c);
	}

	/// Сдвиг отбраковки на shift кадров
	void ShiftCorrupt(int count)
	{
		this->corrupt->Shift(count);
	}
};

// ---------------------------------------------------------------------------
// Параметр double (к примеру, GPS)
// ---------------------------------------------------------------------------
typedef VTemplateParameter<double> VDoubleParameter;
typedef boost::shared_ptr<VDoubleParameter> VDoubleParameterPtr;

// ---------------------------------------------------------------------------
// Параметр unsigned long (к примеру, счетчик ДПП)
// ---------------------------------------------------------------------------
typedef VTemplateParameter<unsigned long>VUlongParameter;

// ---------------------------------------------------------------------------
/// Абсолютная координата 
// ---------------------------------------------------------------------------
class VAbsCoordData : public VDoubleParameter
{
public:

	/// Поиск индекса кадра по координате.
	/// bool forceFit - для точек за пределами массива выбирать крайние индексы(0 и count-1)
	uint32_t FindIndex(double absCoord, bool forceFit) const;

	/// Пересчет индексов при перепривязке сетки абс.координат из srcAbs к нам
	/// Например, при отрезании куска файла
	bool UpdateIndex(uint32_t & index, const VAbsCoordData & srcAbs) const;

	/// Сдвиг координаты
	void Shift(double offset_m);

	/// Масштабирование координаты
	void Scale(double scale_m_per_km);

	/// Пересчет координаты
	void Correct(double offset_m, double expand_m_per_km);

	// Пересчёт коррдинаты с помощью коэффициентов апроксимации
	void Correct(line_t input);

	// Функция возвращающая длину кадра
	const double GetLengthStep();

	// Пересчёт координаты по отдельным участкам пути (Для нормализации файла)
	// void Correct(uint32_t start_index, uint32_t end_index, double start_coord, double end_coord);
};

// ---------------------------------------------------------------------------
/// Параметр (измерения)
// ---------------------------------------------------------------------------
class VParameter : public VTemplateParameter<float>
{
protected:

	/// Кэш суммы для расчета суммы
	mutable double cache_sum;
	/// Кэш индекса для расчета суммы
	mutable size_t cache_sum_index1;
	/// Кэш индекса для расчета суммы
	mutable size_t cache_sum_index2;
	
	/// Кэш для расчета вектора весов
	mutable std::vector<double> cache_weight;

	/// Кэш суммы для расчета числа сбойных кадров
	mutable size_t cache_corrupt;
	/// Кэш индекса для расчета числа сбойных кадров
	mutable size_t cache_corrupt_index1;
	/// Кэш индекса для расчета числа сбойных кадров
	mutable size_t cache_corrupt_index2;

	mutable size_t first_def;
	mutable size_t last_def;

	///Кэш буфера для расчета медианы
	mutable std::vector<float> median_buffer;	
	
public:

	///Сбрасывает кэш
	void ResetCache();

	/// Конструктор
	VParameter();

	/// Добавляет ко всем значениям постоянную величину
	void Shift(float shift);

	/// Смещает параметр на shift кадров
	void ShiftByKadr(short int shift, bool fill_end = true);

	// Загрузка из массива
	void LoadFromArray(float * param);

	/// Инвертирует параметр относительно оси
	void Invert(float axis);

	/// Возвращает индекс максимального по модулю значения на участке
	size_t GetMaxAbsIndex(size_t startNdx, size_t endNdx) const;

	/// Возвращает максимальное значение на участке
	float GetMax(size_t startNdx, size_t endNdx) const;

	/// Возврашает максимальное значение на всем массиве
	float GetMax() const;	

	/// Возврашает максимальное значение на участке
	float GetMaxAbs(size_t startNdx, size_t endNdx) const;

	/// Возврашает максимальное значение на участке со знаком
	float GetMaxAbsSigned(size_t startNdx, size_t endNdx) const;

	/// Возврашает минимальное значение на участке
	float GetMin(size_t startNdx, size_t endNdx) const;	

	/// Возврашает минимальное значение навсем массиве
	float GetMin() const;

	/// Возврашает минимальное значение на участке
	float GetMinAbs(size_t startNdx, size_t endNdx) const;

    /// Возвращает сумму значений на участке
	double GetSum(size_t startNdx, size_t endNdx) const;
	
	 /// Возвращает взвешенную сумму значений на участке
	double GetWeightedSum(size_t startNdx, size_t endNdx) const;

	/// Возвращает число сбоев на участке
	size_t GetCorruptCount(size_t startNdx, size_t endNdx) const;

	// Возвращает число сбоев на участке
	size_t GetCorruptCount(size_t axis, float halfBase, const VAbsCoordData & absCoord) const;

	/// Возвращает сумму квадратов значений на участке
	double GetSum2(size_t startNdx, size_t endNdx, double mo) const;

	/// Возвращает МО на участке
	float GetMO(size_t startNdx, size_t endNdx) const;

	/// Возвращает МО на участке
	float GetWeightedMean(size_t startNdx, size_t endNdx) const;

	/// Возвращает МО в радиусе +/- halfBase от оси axis
	float GetMO(size_t axis, float halfBase, const VAbsCoordData & absCoord) const;
	
	/// Возвращает взвешенное среднее в радиусе +/- halfBase от оси axis
	float GetWeightedMean(size_t axis, float halfBase, const VAbsCoordData & absCoord) const;
	
	/// Возвращает неровность в точке
	float GetJaggy(size_t axis, float base, const VAbsCoordData & absCoord) const;

	/// Возвращает МО на всём файле
	float GetMO() const;

	/// Возвращает дисперсию на участке
	float GetDisp(size_t startNdx, size_t endNdx, double mo) const;	

	/// Возвращает дисперсию на всём файле
	float GetDisp() const;

	/// Возвращает СКО на участке
	float GetSKO(size_t startNdx, size_t endNdx, float mo) const;	

	/// Возвращает СКО на всём файле
	float GetSKO(float mo) const;

	/// Возвращает СТО на участке
	float GetSTO(size_t startNdx, size_t endNdx, float mo) const;	

	/// Возвращает СТО на всём файле
	float GetSTO(float mo) const;
	
	/// Возвращает распределение плотности вероятности на участке
	std::set<density_t> GetDensity(size_t startNdx, size_t endNdx, double in_size);

	/// Возвращает уравнение прямой аппроксимирующей участок по МНК (по индексам)
	line_t GetLine(size_t startNdx, size_t endNdx) const;

	/// Возвращает уравнение прямой аппроксимирующей участок по МНК (по абс. координате)
	line_t GetLine(size_t startNdx, size_t endNdx, const VAbsCoordData & absCoord) const;

	/// Возвращает уравнение прямой, образующей хорду по индексам
	line_t GetChord(size_t startNdx, size_t endNdx);

	/// Возвращает уравнение прямой, образующей хорду (по абс. координате)
	line_t GetChord(size_t startNdx, size_t endNdx, const VAbsCoordData & absCoord);

	/// Возвращает значение уклона параметра на базе 'base' метров
	float GetOtvod(size_t ndx, const VAbsCoordData & absCoord, double base) const;

	/// Возвращает значение уклона параметра на базе 'base' метров
	float GetOtvod(size_t in_start, size_t in_end, const VAbsCoordData & absCoord) const;

	/// Рассчитывает производную
	float CalcDerivate(size_t in_start, size_t in_end) const;
	
	/// Возвращает производную в точке (база - 3 кадра)
	float GetDerivate(size_t in_index) const;

	/// Возвращает вторую производную в точке (база - 3 кадра)
	float GetDerivate2(size_t in_index) const;

	/// Возвращает производную в точке (база - число кадров)
	float GetDerivate(size_t in_index, size_t in_base) const;

	/// Возвращает производную в точке (база - длина в метрах)
	float GetDerivate(size_t ndx, const VAbsCoordData & absCoord, double base) const;	
		
	/// Возвращает медиану на отрезке
	float GetMedian(size_t startNdx, size_t endNdx) const;

	/// Возвращает медиану
	float GetMedian(size_t ndx, const VAbsCoordData & absCoord, double base) const;

	/// Возвращает индекс первого определенного значения
	size_t GetFirstDef() const;

	/// Возвращает индекс последнего определенного значения
	size_t GetLastDef() const;

	/// Возвращает МО на участке - с учетом сбоев, но без кэширования. Для статистики + износов
	float GetStatMO(size_t startNdx, size_t endNdx) const;

	/// Возвращает СКО на участке - с учетом сбоев, но без кэширования. Для статистики
	float GetStatSKO(size_t startNdx, size_t endNdx, float mo) const;

    /// Возвращает максимум на участке - с учетом сбоев, но без кэширования. Для износов
    float GetStatMax(size_t startNdx, size_t endNdx) const;

	///Возвращает комментарий
	virtual std::wstring GetComment() const;
};

typedef boost::shared_ptr<VParameter> VParameterPtr;

// ---------------------------------------------------------------------------
/// Скорость ВПС
// ---------------------------------------------------------------------------

class VSpeedData : public VParameter
{
public:

	// загрузка из файла
	template <class F> bool Load(F & file, uint32_t type);

};

// ---------------------------------------------------------------------------
/// Общий шаблон для "индексных" объектов - метки оператора, маг.зазоры, угоны
// ---------------------------------------------------------------------------
template <class MEMRECORD> class VIndexList : public VParameterProto<MEMRECORD>
{
private:

	/// оператор копирования закрыт
	/// пользуйтесь методом CopyFrom()
	VIndexList<MEMRECORD> & operator = (const VIndexList<MEMRECORD> &);

protected: 

	/// число кадров в оригинальном массиве
	/// для установления границ сдвига
	size_t kadrCount;

	/// записи в памяти
	std::vector<MEMRECORD> records;	

	/// загрузка
	template <class FILETYPE, class FILERECORD>
	bool DefaultLoad(FILETYPE & file, uint32_t type, size_t kadrCount);

	/// Сохранение в файл
	template <class FILERECORD> 
	bool DefaultSave(VMainFile & file, uint32_t type);

	/// Удаляем запись из файла
	void DeleteFromFile(VMainFile & file, uint32_t type)
	{
		size_t idx = file.GetBlockIndex(type);
		if (idx != BAD_SIZE)
		{
			file.DeleteBlock(idx);
		}
	}

public:

	/// Конструктор
	VIndexList() : kadrCount(0)
	{}

	/// Выгрузка
	void Unload()
	{
		this->Clear();
		this->SetLoaded(false);
		this->kadrCount = 0;
		this->message = ERRMSG_DATA_NOT_LOADED;
	}

	/// Число загруженных записей
	size_t GetCount() const
	{
		return this->records.size();
	}

	/// Константная ссылка на i-ю запись
	const MEMRECORD & GetItem(size_t i) const
	{
		return this->records.at(i);
	}

	/// Ссылка на i-ю запись
	MEMRECORD & GetItem(size_t i) 
	{
		return this->records.at(i);
	}

	/// Добавить запись
	void AddItem(const MEMRECORD & mark)
	{
		this->records.push_back(mark);
		this->SetLoaded(true);
	}

	/// Удаление записи
	void DeleteItem(size_t i)
	{
		this->records.erase(this->records.begin() + i);
	}

	/// Удаление всех записей
	void Clear()
	{
		this->records.clear();
	}

	///Сортировка по координате
	void Sort()
	{
		if (!this->records.empty())
			std::sort(this->records.begin(), this->records.end());
	}

	/// Копирование из другого списка, с пересчетом координат
	bool CopyFrom(const VIndexList<MEMRECORD> & src, 
		const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs);

	/// Поиск записи привязанной к указанному кадру
	size_t FindItem(size_t kadrNdx) const;

	/// Сдвиг на указанное число кадров
	void Shift(int value);
};

// Сдвиг на указанное число кадров
template <class MEMRECORD>
void VIndexList<MEMRECORD>::Shift(int value)
{
	std::vector<MEMRECORD> new_records;

	for (size_t i = 0; i < this->records.size(); ++i)
	{
		__int64 new_index = (__int64) this->records[i].index + (__int64) value;
		if (new_index > 0 && new_index < (__int64) this->kadrCount)
		{
			this->records[i].index = (size_t) new_index;
			new_records.push_back(this->records[i]);
		}
	}

	this->records = new_records;
}

// Поиск записи привязанной к указанному кадру
template <class MEMRECORD>
size_t VIndexList<MEMRECORD>::FindItem(size_t kadrNdx) const
{
	class PredicateFits
	{
		const size_t index;

	public:

		PredicateFits(size_t i) : index(i)
		{}

		bool operator () (const MEMRECORD & rec) const
		{
			return rec.index == this->index;
		}
	};

	std::vector<MEMRECORD>::const_iterator found_item = std::find_if(
		this->records.begin(), this->records.end(), PredicateFits(kadrNdx));

	if (found_item == this->records.end()) return BAD_INDEX;
	else return found_item - this->records.begin();
}

// Загрузка
template <class MEMRECORD>
template <class FILETYPE, class FILERECORD>
bool VIndexList<MEMRECORD>::DefaultLoad(FILETYPE & file, uint32_t type, size_t kCount)
{
	if (this->IsLoaded()) return true;
	this->Unload();

	VBasicParameter data;
	if (!data.Load(file, type, false))
	{
		this->message = data.GetErrorMessage();
		return false;
	}

	if (!data.CheckRecCount<FILERECORD>()) 
	{	
		data.UnloadFile(file, type);
		data.Unload();
		this->message = ERRMSG_VERSION_INCORRECT;
		return false;
	}

	MEMRECORD record;
	for (register size_t i = 0; i < data.GetRecCount(); ++i)
	{
		if (record.Import(data.GetItem<FILERECORD>(i)) &&
			record.Fits(kCount))
		{
			this->records.push_back(record);
		}
	}

	this->kadrCount = kCount;
	data.UnloadFile(file, type);
	data.Unload();
	this->message = ERRMSG_OK;
	this->SetLoaded(true);
	return true;
}

// Сохранение в файл
template <class MEMRECORD>
template <class FILERECORD> 
bool VIndexList<MEMRECORD>::DefaultSave(VMainFile & file, uint32_t type)
{
	if (!this->IsLoaded())
	{
		this->message = ERRMSG_DATA_NOT_LOADED;
		return false;
	}

	// при записи пустого блока нужно просто удалить старый
	if (this->GetCount() == 0)
	{
		this->DeleteFromFile(file, type);
		this->message = ERRMSG_OK;
		return true;
	}

	VBasicParameter data;
	data.Create(type, VCT_USER, this->GetCount(), sizeof(FILERECORD), 1);

	size_t position = 0;
	for (size_t i = 0; i < this->GetCount(); ++i)
	{
		FILERECORD frec = this->GetItem(i).Export();
		data.Write<FILERECORD>(frec, position);
	}

	if (data.Save(file, this->GetCount(), sizeof(FILERECORD)))
	{
		this->message = ERRMSG_OK;
		return true;
	}
	else
	{
		this->message = ERRMSG_BLOCK_WRITE_FAILED;
		return false;
	}
}

// Копирование из другого списка, с пересчетом координат
template <class MEMRECORD>
bool VIndexList<MEMRECORD>::CopyFrom(const VIndexList<MEMRECORD> & src, 
	const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs)
{
	this->Unload();
	for (size_t itemNdx = 0; itemNdx < src.GetCount(); ++itemNdx)
	{
		MEMRECORD item = src.GetItem(itemNdx);
		if (item.UpdateIndex(destAbs, srcAbs))
		{
			this->records.push_back(item);
		}
	}
	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
}


///Функции обработки параметров FixMe!!! вынести  в отдельный файл

///Расчет скользящего среднего на базе (где база - абс. коорд.)
void ParameterMean(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base);

///Расчет скользящего среднего на базе (где база - абс. коорд.)
void ParameterWeightedMean(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base);

/// Спектр
void ParameterSpectrum(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord);

/// Фаза
void ParameterPhase(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord);

/// FIR
void ParameterFIRFilter(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, const VFIRFilterPtr& in_filter);

/// Фильтр Калмана
void ParameterKalman(VParameter& in_source, VParameter& out_target, double in_q, double in_r);

///Расчет скользящего СКО на базе (где база - абс. коорд.)
void ParameterSKO(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base);

///Расчет медианного фильтра
void ParameterMedian(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base);

///Расчет отвода на базе (где база - число кадров)
void ParameterOtvod(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base);

///Производная параметра на базе
void ParameterDerivate(VParameter& in_source, VParameter& out_target, size_t in_half_base);

///Производная параметра на базе (база - абс. коорд)
void ParameterDerivate(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base);

///"Резкость параметра"
void ParameterSharp(VParameter& in_source, VParameter& out_target, double in_amount);

///Разность двух параметров
void ParameterDiff(VParameter& in_source1, VParameter& in_source2, VParameter& out_target);

///Масштабирование параметра
void ParameterScale(VParameter& in_source, VParameter& out_target, double in_aspect);

///Смещение параметра
void ParameterShift(VParameter& in_source, VParameter& out_target, double in_value);

///Геометрическая сумма двух параметров
void ParameterGeoSum(VParameter& in_source1, VParameter& in_source2, VParameter& out_target);

/// Структура содеражащая СКО и смещение параметра
struct CorrAndOffset
{
	int offset;
	double oldCorr;
	double newCorr;
};

/// Синхронизация параметров - поиска максимума корреляции между параметрами
/// На выходе - смещение 2 параметра относительно первого.
double CorrParams(const VParameter & prm1, const VParameter & prm2,			// параметры
						PARAMETER_TYPE prm1_t, PARAMETER_TYPE prm2_t,		// название параметров
						size_t offset,	size_t maxOffset);					// смещение одного параметра относительно другого

//Служебные функции

///Расчет ковариации двух параметров
double cov(VParameter & in_param1, VParameter & in_param2);
///Расчет коэффициента корреляции (Пирсона)
double corr(VParameter & in_param1, VParameter & in_param2);
///Расчет коэффициента корреляции (Пирсона) по двум файлам
double corr(const VParameter& in_param1, const VAbsCoordData& in_abscoord1, 
			const VParameter & in_param2, const VAbsCoordData& in_abscoord2);
///Расчет коэффициента корреляции знаков (Фехнера)
double corr_sign(VParameter & in_param1, VParameter & in_param2);





