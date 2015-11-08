#pragma once

#include "mathlib.h"
#include "singleton.h"
#include "interval.h"
#include "piecewise.h"

/// Степень отступления
enum DEGREE
{
	DEGREE_NONE=0,
	DEGREE1=1,
	DEGREE2=2,
	DEGREE3=3,
	DEGREE4=4
};

/// Определяем нормы ШК (FixMe!!! это должно быть, где то в более общем месте. Все должны пользоваться этими определениями)
#define NORMA_WIDTH_1520 1520.0
#define NORMA_WIDTH_1524 1524.0
#define NORMA_WIDTH_1530 1530.0
#define NORMA_WIDTH_1535 1535.0
#define NORMA_WIDTH_1540 1540.0


/// Длина просадки
#define PIT_LENGTH 6.0
/// Максимальная длина просадки
#define PIT_MAX_LENGTH 10.0
/// Диапазон длин просадок
#define PIT_LENGTH_RANGE pro::interval<double>(0, PIT_LENGTH, true, true)


/// Длина перекоса
#define DISTORTION_LENGTH 20.0
/// Диапазон длин перекосов 
#define DISTORTION_LENGTH_RANGE pro::interval<double>(0,DISTORTION_LENGTH, false, true)
/// Диапазон длин перекосов
#define SHORT_DISTORTION_LENGTH_RANGE pro::interval<double>(0,10, false, true)
/// Диапазон длин перекосов
#define LONG_DISTORTION_LENGTH_RANGE pro::interval<double>(10,DISTORTION_LENGTH, false, true)
/// Максимальная длина перекоса
#define DISTORTION_MAX_LENGTH 40.0

/// Максимальная длина рихтовки
#define RICH_MAX_LENGTH 40.0
/// Диапазон длин отступлений по рихтовке
#define RICH_LENGTH_RANGE pro::interval<double>(0.0,20.0, false, true)
/// Диапазон длин отступлений по рихтовке
#define LONG_RICH_LENGTH_RANGE pro::interval<double>(20.0, 40.0, false, true)
/// Диапазон длин отступлений по рихтовке
#define VERY_LONG_RICH_LENGTH_RANGE pro::interval<double>(40.0, 60.0, false, true)


/// Строка таблицы степений ШК
class VNarrowRecord
{
private:
	/// Степень
	DEGREE degree;
	/// Диапазон скоростей
	pro::interval<int> speed;
	/// Норма ШК
	double norma;
	/// Диапазон значений
	pro::interval<double> value;
public:

	/// Функтор для поиска
	class VFindKey
	{
	private:
		DEGREE degree;
		int speed;
		double norma;
	public:
		VFindKey(DEGREE in_degree, int in_speed, double in_norma) : degree(in_degree), speed(in_speed), norma(in_norma){};
		bool operator()(const VNarrowRecord& in_key) const
		{
			return in_key.IsKey(degree, speed, norma);
		}
	};

	/// Конструктор
	VNarrowRecord();	
	/// Конструктор
	VNarrowRecord(DEGREE in_degree, const pro::interval<int>& in_speed, double in_norma, const pro::interval<double>& in_value);	
	/// Возвращает true, если входные данный удовлетворяют ключу
	bool IsKey(DEGREE in_degree, int in_speed, double in_norma) const;
	/// Возвращает диапазон значений
	const pro::interval<double> GetRange() const;
};

/// Строка таблицы степений уширений
class VWidenRecord
{
private:
	/// Степень
	DEGREE degree;
	/// Диапазон скоростей
	pro::interval<int> speed;
	/// Норма ШК
	double norma;
	/// Диапазон радиусов кривых
	pro::interval<double> radius;
	/// Диапазон значений
	pro::interval<double> value;
public:

	/// Функтор для поиска
	class VFindKey
	{
	private:
		DEGREE degree;
		int speed;
		double norma;
		double radius;
	public:
		VFindKey(DEGREE in_degree, int in_speed, double in_norma, double in_radius) : degree(in_degree), speed(in_speed), 
			norma(in_norma), radius(in_radius){};
		bool operator()(const VWidenRecord& in_key) const
		{
			return in_key.IsKey(degree, speed, norma, radius);
		}
	};

	/// Конструктор
	VWidenRecord(DEGREE in_degree, const pro::interval<int>& in_speed, double norma, const pro::interval<double>& in_radius, const pro::interval<double>& in_value);	
	/// Возвращает true, если входные данный удовлетворяют ключу
	bool IsKey(DEGREE in_degree, int in_speed, double in_norma, double in_radius) const;
	/// Возвращает диапазон значений
	const pro::interval<double> GetRange() const;
};


/// Таблица степеней отступлений по уширению
class VWidenTable
{
private:
	/// Таблица для скоростей ниже 140
	std::vector<VWidenRecord> table;
	/// Таблица для скоростей выше 140
	std::vector<VWidenRecord> table_high_speed;
public:
	/// Конструктор
	VWidenTable();
	/// Возвращает диапазон
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed, double in_norma, double in_radius) const;
};

/// Таблица степеней отступлений по сужению
class VNarrowTable
{
private:
	/// Таблица
	std::vector<VNarrowRecord> table;
public:
	/// Конструктор
	VNarrowTable();
	/// Возвращает диапазон
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed, double in_norma) const;
};

/// Строка таблицы критерия отступления (волна)
class VWaveRecord
{
private:
	/// Степень
	DEGREE degree;
	/// Диапазон скоростей
	pro::interval<int> speed;
	/// Диапазон длин
	pro::interval<double> length;
	/// Диапазон значений
	pro::interval<double> value;
public:

	/// Функтор для поиска
	class VFindKey
	{
	private:
		DEGREE degree;
		int speed;
		double length;
	public:
		VFindKey(DEGREE in_degree, int in_speed, double in_length) : degree(in_degree), speed(in_speed), length(in_length){};
		bool operator()(const VWaveRecord& in_key) const
		{
			return in_key.IsKey(degree, speed, length);
		}
	};

	/// Конструктор
	VWaveRecord();
	/// Конструктор
	VWaveRecord(DEGREE in_degree, const pro::interval<int>& in_speed, const pro::interval<double>& in_length, const pro::interval<double>& in_value);	
	/// Возвращает true, если входные данный удовлетворяют ключу
	bool IsKey(DEGREE in_degree, int in_speed, double in_length) const;
	/// Возвращает диапазон значений
	const pro::interval<double> GetRange() const;
};

/// Таблица отступлений по просадкам
class VPitTable
{
private:
	/// Таблица
	std::vector<VWaveRecord> table;		
public:
	/// Конструктор
	VPitTable();	
	/// Возвращает диапазон
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed, double in_length) const;
};

/// Таблица отступлений по перекосам
class VDistortionTable
{
private:
	/// Таблица
	std::vector<VWaveRecord> table;	
public:
	/// Конструктор
	VDistortionTable();	
	/// Возвращает диапазон
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed, double in_length) const;
};


/// Строка таблицы степеней ПЛУ
class VLevelRecord
{
private:
	/// Степень
	DEGREE degree;
	/// Диапазон скоростей
	pro::interval<int> speed;
	/// Диапазон значений
	pro::interval<double> value;
public:

	/// Функтор для поиска
	class VFindKey
	{
	private:
		DEGREE degree;
		int speed;
	public:
		VFindKey(DEGREE in_degree, int in_speed) : degree(in_degree), speed(in_speed){};
		bool operator()(const VLevelRecord& in_key) const
		{
			return in_key.IsKey(degree, speed);
		}
	};
	/// Конструктор
	VLevelRecord(DEGREE in_degree, const pro::interval<int>& in_speed, const pro::interval<double>& in_value);	
	/// Возвращает true, если входные данный удовлетворяют ключу
	bool IsKey(DEGREE in_degree, int in_speed) const;
	/// Возвращает диапазон значений
	const pro::interval<double> GetRange() const;
};


/// Таблица отступлений по уровню
class VLevelTable
{
private:
	/// Таблица
	std::vector<VLevelRecord> table;
public:
	/// Конструктор
	VLevelTable();
	/// Возвращает диапазон
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed) const;
};

/// Таблица отступлений по рихтовкам
class VRichTable
{
private:
	/// Таблица
	std::vector<VWaveRecord> table;	
public:
	/// Конструктор
	VRichTable();
	/// Возвращает диапазон
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed, double in_length) const;
};

//// -------------------------- Балловая оценка отступлений ---------------------------

/// Строка таблицы балльной оценки
class VPenaltyRecord
{
private:
	
	int penalty;			///< Кол-во баллов
	int norma;				///< норма ШК
	pro::interval<int> speed;		///< Диапазон скоростей
	pro::interval<double> value;	///< Диапазон значений
	pro::interval<int> add_key;	///< Дополнительный ключ

public:

	/// Функтор для поиска
	class VFindKey
	{
	private:

		int norma;
		int speed;
		double value;
		int add_key;

	public:

		VFindKey(int in_norma, int in_speed, double in_value, int in_add_key) : 
		  norma(in_norma), speed(in_speed), value(in_value), add_key(in_add_key)
		{}

		bool operator() (const VPenaltyRecord & in_key) const
		{
			return in_key.IsKey(norma, speed, value, add_key);
		}
	};

	/// Конструктор
	VPenaltyRecord() : penalty(0), norma(0), speed(0, 300), 
		value(0, 0), add_key(0, INT_MAX)
	{}

	/// Конструктор
	VPenaltyRecord(int in_penalty, int in_norma, 
		const pro::interval<int> & in_speed, const pro::interval<double> & in_value) :
	penalty(in_penalty), norma(in_norma), speed(in_speed), value(in_value), add_key(0, INT_MAX)
	{}

	/// Конструктор
	VPenaltyRecord(int in_penalty, int in_norma, 
		const pro::interval<int> & in_speed, const pro::interval<double> & in_value,
		const pro::interval<int> & in_add_key) :
	penalty(in_penalty), norma(in_norma), speed(in_speed), value(in_value), add_key(in_add_key)
	{}

	/// Возвращает true, если входные данный удовлетворяют ключу
	bool IsKey(int in_norma, int in_speed, double in_value, int in_add_key) const	
	{
		return this->norma == in_norma && this->speed.contains(in_speed) && 
			this->value.contains(in_value) && this->add_key.contains(in_add_key);
	}

	/// Возвращает штраф
	int GetPenalty() const
	{
		return this->penalty;
	}
};

/// Общая таблица бальной оценки
class VPenaltyTable 
{
protected:
	
	/// Таблица
	std::vector<VPenaltyRecord> table;

public:

	/// Возвращает баллы
	int GetPenalty(int in_norma, int in_speed, double in_value, int in_add_key) const;
};

typedef boost::shared_ptr<VPenaltyTable> VPenaltyTablePtr;

/// Бальная оценка уширений
class VWidenPenaltyTable : public VPenaltyTable
{
public:

	/// Конструктор
	VWidenPenaltyTable();
};

/// Бальная оценка сужений
class VNarrowPenaltyTable : public VPenaltyTable
{
public:

	/// Конструктор
	VNarrowPenaltyTable();
};

/// Бальная оценка сужений
class VDistortionPenaltyTable : public VPenaltyTable
{
public:

	/// Конструктор
	VDistortionPenaltyTable();
};

/// Бальная оценка просадок
class VPitPenaltyTable : public VPenaltyTable
{
public:

	/// Конструктор
	VPitPenaltyTable();
};

/// Бальная оценка плавных отступлений по уровню
class VLevelPenaltyTable : public VPenaltyTable
{
public:

	/// Конструктор
	VLevelPenaltyTable();
};

/// Бальная оценка отступлений в плане
class VRichPenaltyTable : public VPenaltyTable
{
public:

	/// Конструктор
	VRichPenaltyTable();
};


// -------------------------------------------------------------------------------------


/// Таблицы нормативов
class VDetectTables/* : public VOptions*/
{
private:		

	//Данные

	/// Нормативы уширений
	VWidenTable widen_table;
	/// Нормативы сужений
	VNarrowTable narrow_table;
	/// Нормативы просадок
	VPitTable pit_table;
	/// Нормативы перекосов
	VDistortionTable distortion_table;
	/// Нормативы уровня
	VLevelTable level_table;
	/// Нормативы рихтовок
	VRichTable rich_table;

	/// Бальная оценка уширений
	VPenaltyTablePtr widen_penalty_table;
	/// Бальная оценка сужений
	VPenaltyTablePtr narrow_penalty_table;
	/// Бальная оценка перекосов
	VPenaltyTablePtr distortion_penalty_table;
	/// Бальная оценка просадок
	VPenaltyTablePtr pit_penalty_table;
	/// Бальная оценка уровня
	VPenaltyTablePtr level_penalty_table;
	/// Бальная оценка рихтовки
	VPenaltyTablePtr rich_penalty_table;		
	
public:	
	
	/// Конструктор
	VDetectTables();		

	// Методы возвращающие нормативы

	/// Возвращает нормативы уширений
	const VWidenTable& GetWidenTable() const;
	/// Возвращает нормативы сужений
	const VNarrowTable& GetNarrowTable() const;
	/// Возвращает нормативы просадок
	const VPitTable& GetPitTable() const;
	/// Возвращает нормативы перекосов
	const VDistortionTable& GetDistortionTable() const;
	/// Возвращает нормы уровня
	const VLevelTable& GetLevelTable() const;
	/// Возвращает нормативы рихтовок
	const VRichTable& GetRichTable() const;

	// Методы возвращающие нормы бальной оценки

	/// Бальная оценка уширений
	const VPenaltyTable& GetWidenPenaltyTable() const;
	/// Бальная оценка сужений
	const VPenaltyTable& GetNarrowPenaltyTable() const;
	/// Бальная оценка перекосов
	const VPenaltyTable& GetDistortionPenaltyTable() const;
	/// Бальная оценка просадок
	const VPenaltyTable& GetPitPenaltyTable() const;
	/// Бальная оценка уровня
	const VPenaltyTable& GetLevelPenaltyTable() const;
	/// Бальная оценка рихтовки
	const VPenaltyTable& GetRichPenaltyTable() const;					
};
typedef VSingleton<VDetectTables> detect_tables_t;
#define DETECT_TABLES_INIT detect_tables_t::Instance().SetObject(VDetectTables())
#define DETECT_TABLES detect_tables_t::Instance().GetObject() 

