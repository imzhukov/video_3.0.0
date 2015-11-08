#pragma once


#include "rtparam.h"

#include "processing.h"
#include "situation.h"
#include "options.h"
#include "tables.h"
#include "signal_processing.h"
#include "piecewise.h"

//boost
#include <new>
#include <boost/utility.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/covariance.hpp>
#include <boost/accumulators/statistics/variates/covariate.hpp>
#include <boost/accumulators/statistics/skewness.hpp>
#include <boost/accumulators/statistics/kurtosis.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>


//shiny
#include "Shiny.h"

using namespace boost::accumulators ;

// Вспомогательные функции 

template< typename DEFAULT_INITIALIZABLE >
inline void clear( DEFAULT_INITIALIZABLE& object )
{
    object.DEFAULT_INITIALIZABLE::~DEFAULT_INITIALIZABLE() ;
    ::new ( boost::addressof(object) ) DEFAULT_INITIALIZABLE() ;
}

/// Корректор рихтовки
class VRichCorrector
{
public:
	virtual double Correct(DEGREE in_degree, int in_speed, double in_value, double in_length) const = 0;
};
typedef boost::shared_ptr<VRichCorrector> VRichCorrectorPtr;

/// Строка таблицы палетки
class VRichPaletRecord
{
private:
	/// Степень
	DEGREE degree;
	/// Диапазон скоростей
	pro::interval<int> speed;
	/// Верхняя граница норматива в зависимости от длины отсупления
	pro::piecewise::linear top;
	/// Нижняя граница норматива в зависимости от длины отсупления
	pro::piecewise::linear bottom;
public:
	/// Конструктор
	VRichPaletRecord(DEGREE in_degree, const pro::interval<int>& in_speed, 
		const pro::piecewise::linear& in_bottom, const pro::piecewise::linear& in_top) : 
		degree(in_degree), speed(in_speed), bottom(in_bottom), top(in_top) {};
	/// Функтор для поиска
	class VFindKey
	{
	private:
		DEGREE degree;
		int speed;
	public:
		VFindKey(DEGREE in_degree, int in_speed) : degree(in_degree), speed(in_speed){};
		bool operator()(const VRichPaletRecord& in_key) const
		{
			return in_key.IsKey(degree, speed);
		}
	};		
	/// Возвращает true, если входные данный удовлетворяют ключу
	bool IsKey(DEGREE in_degree, int in_speed) const
	{
		return (degree==in_degree) && speed.contains(in_speed);
	};
	/// Возвращает диапазон значений
	const pro::interval<double> GetRange(double in_length) const
	{
		if (bottom.value_in_domain(in_length) && top.value_in_domain(in_length))
			return pro::interval<double>(bottom.y(in_length), top.y(in_length), false, true);
		else
			return EMPTY_INTERVAL<double>();			
	};
};

/// Палетка
class VRichPalet : public VRichCorrector
{
private:
	/// Таблица
	std::vector<VRichPaletRecord> table;	
public:
	/// Конструктор
	VRichPalet();
	/// Возвращает диапазон
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed, double in_length) const;
	/// Функция коррекции
	double Correct(DEGREE in_degree, int in_speed, double in_value, double in_length) const;
};

/// Таблица пересчета рихтовки
class VRichCorrectionTable  : public VRichCorrector
{
private:
	/// Кусочно линейная функция
	pro::piecewise::linear function;
public:
	/// Конструктор
	VRichCorrectionTable();
	/// Функция коррекции
	double Correct(DEGREE in_degree, int in_speed, double in_value, double in_length) const;
};

/// Без коррекции
class VRichCorrectionNone  : public VRichCorrector
{
private:	
public:
	/// Конструктор
	VRichCorrectionNone() {};
	/// Функция коррекции
	double Correct(DEGREE in_degree, int in_speed, double in_value, double in_length) const {return in_value;};
};

/// Группы отступлений
enum OVERSTEP_GROUP
{
	GROUP_NONE=0,
	GROUP_GEOMETRY=1,
	GROUP_EXTRA=2,
	GROUP_SERVICE_GEOMETRY=3,
	GROUP_SERVICE_EXTRA=4,
	GROUP_EXPRESS=5
};

/// Возвращает норму ШК ближайшую к указанной
static double RoundNormaWidth(double in_norma)
{
	double result=1520;
	std::vector<double> norma;
	norma.push_back(NORMA_WIDTH_1520);
	norma.push_back(NORMA_WIDTH_1524);
	norma.push_back(NORMA_WIDTH_1530);
	norma.push_back(NORMA_WIDTH_1535);
	norma.push_back(NORMA_WIDTH_1540);	
	for (size_t i=1; i<norma.size(); ++i)
	{
		if (in_norma>=norma.at(i-1) && in_norma<=norma.at(i))
		{
			return fabs(in_norma-norma.at(i-1)) < fabs(in_norma-norma.at(i)) ? norma.at(i-1) : norma.at(i);
		}
	}
	return result;
};

/// Определения типов отступлений
#define WIDEN L"Уш"
#define NARROW L"Суж"
#define OTVOD_WIDTH L"Ошк"
#define WEAR_LEFT L"Изн.лв"
#define WEAR_RIGHT L"Изн.пр"
#define WEAR L"Изн"
#define PIT_LEFT L"Пр.лв"
#define PIT_RIGHT L"Пр.пр"
#define PIT_LEFT_GROUP L"3Пр.лв"
#define PIT_RIGHT_GROUP L"3Пр.пр"
#define LEVEL L"У"
#define SHORT_DISTORTION L"УР"
#define DISTORTION L"П"
#define DISTORTION_GROUP L"3П"
#define RICH_LEFT L"Р.лв"
#define RICH_RIGHT L"Р.пр"
#define RICH L"Р"
#define RICH_NR L"Рнр"
#define RICH_GROUP L"3Р"
#define RICH_GROUP_NR L"3Рнр"
#define RICH_LEFT_GROUP L"3Р.лв"
#define RICH_RIGHT_GROUP L"3Р.пр"
#define RICH_PIT_COMBINATION  L"Р+Пр"
#define RICH_DISTORTION_COMBINATION  L"Р+П"
#define ANP L"Уск"
#define PSI L"Уск_ск"
#define ZAZOR_LEFT L"Заз.лв"
#define ZAZOR_RIGHT L"Заз.пр"
#define ZERO_ZAZOR_LEFT L"СЗ.лв"
#define ZERO_ZAZOR_RIGHT L"СЗ.пр"

#define WIDTH_CORRUPT L"Сбой ШК"
#define PIT_LEFT_CORRUPT L"Сбой пр.лв"
#define PIT_RIGHT_CORRUPT L"Сбой пр.пр"
#define RICH_LEFT_CORRUPT L"Сбой рихт.лв"
#define RICH_RIGHT_CORRUPT L"Сбой рихт.пр"
#define WEAR_LEFT_CORRUPT L"Сбой изн.лв"
#define WEAR_RIGHT_CORRUPT L"Сбой изн.пр"
#define WEAR_CORRUPT L"Сбой изн."
#define LEVEL_CORRUPT L"Сбой ур"

#define WIDTH_CORRUPT_PK L"Сбой ШК ПК"
#define PIT_LEFT_CORRUPT_PK L"Сбой пр.лв ПК"
#define PIT_RIGHT_CORRUPT_PK L"Сбой пр.пр ПК"
#define RICH_LEFT_CORRUPT_PK L"Сбой Р.лв ПК"
#define RICH_RIGHT_CORRUPT_PK L"Сбой Р.пр ПК"
#define WEAR_CORRUPT_PK L"Сбой изн. ПК"
#define LEVEL_CORRUPT_PK L"Сбой ур. ПК"

#define CONTROL_WIDTH_MO L"КШК"
#define CONTROL_LEVEL_MO L"КУ"
#define CONTROL_WIDTH_SKO L"КШК СКО"
#define CONTROL_LEVEL_SKO L"КУ СКО"
#define W10 L"Ш10"
#define K100 L"К100"
#define K60 L"К60"

#define ANP_AVERAGE L"Анп"
#define ANP_HORIZONTAL L"Аг"
#define PSI_HORIZONTAL L"Пси"
#define PSI_TRANSITION L"Пси пк"
#define OTVOD_LEVEL L"Укл"

#define FALLBACK L"Откл."

#define WIDTH_OFF L"Откл. ШК"
#define PIT_LEFT_OFF L"Откл. пр.лв"
#define PIT_RIGHT_OFF L"Откл. пр.пр"
#define RICH_LEFT_OFF L"Откл. рихт.лв"
#define RICH_RIGHT_OFF L"Откл. рихт.пр"
#define LEVEL_OFF L"Откл. ур"
#define WEAR_LEFT_OFF L"Откл. изн.лв"
#define WEAR_RIGHT_OFF L"Откл. изн.пр"
#define ZAZOR_LEFT_OFF L"Откл. заз.лв"
#define ZAZOR_RIGHT_OFF L"Откл. заз.пр"
#define MAG_ZAZOR_LEFT_OFF L"Откл. заз.магн.лв"
#define MAG_ZAZOR_RIGHT_OFF L"Откл. заз.магн.пр"




/// Обозначение моста
#define BRIDGE_SIGN L"м "
/// Обозначение стрелки
#define SWITCH_SIGN L"стр. "
/// Обозначение уравнительного прибора
#define CHAMFERED_JOINT_SIGN L"ур.пр. "
/// Обозначение учета НТД 
#define NTD_SIGN L"НТД "
/// Обозначение учета распоряжения 283р
#define N283_SIGN L"283р "
/// Обозначение ПРЖ
#define PRJ_SIGN L"прж "
/// Обозначение обкатки
#define TEST_RUN_SIGN L"обкатка "
/// Обозначение ЖБ шпал до 96 г.
#define ZBSH_SIGN L"ЖБ96"
/// Обозначение ограничения до натурной проверки
#define CLOSURE_CORRECTION_SIGN L"нат. пр."
/// Обозначение неточной координаты
#define INACCURATE_COORDINATE_SIGN L"неточная координата"
/// Обозначение недостоверного паспорта
#define BAD_PASSPORT_SIGN L"пасп."

/// Максимальная скорость
#define MAX_SPEED 300
/// Шаг изменения скорости
#define SPEED_QUANTUM 5
/// Расстояние принятие решения (считается что на таком расстоянии от текущей точки не может быть зарегистрировано новых отступлений)
#define DECISION_DISTANCE 150.0
/// Максимальная величина отступления
#define MAX_OVERSTEP_LENGTH 100.0
/// Порог близости 3-й степени к 4-й
#define CLOSE_TO_UPPER_BOUND_VALUE 1.0

/// Скорость ограничения до натурной проверки
#define CLOSURE_CORRECTION_SPEED 25

/// Величина поправки на рельсах р50
#define P50_CORRECTION_VALUE 1.5


/// Точность по умолчанию (знаков после запятой)
#define DEFAULT_PRECISION 0
/// Точность Анп (знаков после запятой)
#define ANP_PRECISION 2
/// Точность Пси (знаков после запятой)
#define PSI_PRECISION 2
/// Точность износа (знаков после запятой)
#define WEAR_PRECISION 0
/// Точность контрольных участков (знаков после запятой)
#define CONTROL_SECTION_PRECISION 2
/// Точность отвода ШК
#define OTVOD_WIDTH_PRECISION 2
/// Точность отвода возвышения
#define OTVOD_LEVEL_PRECISION 2

//---------------------------- Сбои параметров -------------------------

/// Длина сбойного участка (м) FixMe!!! этот параметр может быть настраиваемым и разным для разных параметров
#define CORRUPT_LENGTH 10.0
/// Процент сбоев на пикете
#define CORRUPT_PERCENT_PK 50.0

//---------------------------- Ширина колеи ---------------------------

/// Минимальная длина отступления ШК
#define MIN_WIDTH_LENGTH 1.0
/// Максимальная длина отступления по ШК
#define MAX_WIDTH_LENGTH 300.0
/// База усреднения износа для коррекции уширений второй степени
#define WEAR_CORRECTION_BASE 2.0
/// Максимальная величина износа для коррекции уширений второй степени
#define MAX_WEAR_CORRECTION 15.0
/// Максимальный радиус кривой в которых возможна коррекция 
#define MAX_RADIUS_CORRECTION 1200.0
/// Длина элементарного отступления
#define WIDTH_ELEMENTARY_LENGTH 4.0
/// Максимальный радиус кривой для которого действует поправка на скоростных линиях
#define MAX_CORRECTION_RADIUS_FOR_HIGHSPEED_LINE 3000

//----------------------------- Отвод ШК ---------------------------------

/// База вычисления отвода ШК
#define OTVOD_WIDTH_BASE 2.0
/// Минимальная длина отвода
#define OTVOD_WIDTH_QUANTUM 2.0

//----------------------------- Отвод возвышения --------------------------------

/// Минимальная длина отвода
#define OTVOD_LEVEL_QUANTUM 2.0

//----------------------------- Износ --------------------------------

/// Минимальная длина отступления по износу
#define WEAR_QUANTUM 1.0
/// Максимальная длина отступления по износу
#define MAX_WEAR_LENGTH 300.0


//----------------------------- Просадки -------------------------------

/// База поиска экстремума просадки
#define PIT_EXTREMUM_BASE 5.0
/// Расстояние принятия решения
#define PIT_DECISION_DISTANCE 50.0

//----------------------------------- Уровень -------------------------------------

/// Минимальная длина отступления по уровню
#define LEVEL_QUANTUM 10.0
/// Длина единичного отступления по уровню
#define LEVEL_ELEMENTARY_LENGTH 30.0
/// Длина единичного отступления по уровню ЦП-515
#define LEVEL_ELEMENTARY_LENGTH_515 10.0
/// Диапазон длин отступлений по уровню
#define MAX_LEVEL_LENGTH 300.0

//----------------------------- Отвод уровня ---------------------------------

/// База вычисления отвода уровня
#define SHORT_DISTORTION_BASE 2.5

//---------------------------------- Перекосы -------------------------------------

/// База поиска экстремума перекоса
#define DISTORTION_EXTREMUM_BASE 5.0
/// Расстояние принятия решения
#define DISTORTION_DECISION_DISTANCE 100.0

//--------------------------------- Рихтовка ------------------------------------

/// База поиска экстремума рихтовки
#define RICH_EXTREMUM_BASE 5.0
/// Расстояние принятия решения
#define RICH_DECISION_DISTANCE 100.0


//--------------------------------- Сочетания ----------------------------------------

/// Число повторяющихся отступлений
#define PERIODICAL_COUNT 3

/// Длина просадок в группе
#define PIT_GROUP_LENGTH DBL_MAX
/// Величина просадок в группе
#define PIT_GROUP_VALUE 20.0
/// Величина просадок в группе
#define PIT_GROUP_VALUE_515 15.0
/// Длина группы просадок
#define PIT_GROUP_DISTANCE 10.0

/// Длина перекосов в группе
#define DISTORTION_GROUP_LENGTH 10.0
/// Величина перекосов в группе
#define DISTORTION_GROUP_VALUE 16.0
/// Длина группы перекосов
#define DISTORTION_GROUP_DISTANCE 30.0

/// Длина рихтовки в группе
#define RICH_GROUP_LENGTH 20.0
/// Величина рихтовок в группе
#define RICH_GROUP_VALUE 25.0
/// Длина группы рихтовок
#define RICH_GROUP_DISTANCE 30.0

/// Расстояние между отступлениями в сочетании
#define COMBINATION_DISTANCE 10.0
/// Величина просадки в сочетании
#define COMBINATION_PIT_VALUE 15.0
/// Величина рихтовки в сочетании
#define COMBINATION_RICH_VALUE 15.0
/// Минимальная установленная скорость при которой оцениваются сочетания
#define MIN_COMBINATION_SPEED 60

//------------------------------------ Анп -------------------------------------------------

// Минимальная длина отступления
#define ANP_QUANTUM 1.0
/// Порог Анп
#define MAX_ANP 0.7

//-------------------------------- Скорость изменения Анп ---------------------------------

/// Минимальная длина отступления
#define PSI_QUANTUM 1.0
/// Порог пси
#define MAX_PSI 0.6
/// Порог пси для ограничения
#define MAX_PSI_RESTRICTION 0.66
/// Порог пси для пассажирских поездов с наклоном кузова
#define MAX_PSI_BT_RESTRICTION 0.9

//--------------------------------- Зазоры -----------------------------------------------

/// Длина рельса (с запасом 1 м)
#define RAIL_LENGTH 26.0

//--------------------------- Коррекция координаты ---------------------------------------

/// Максимальная длина участка коррекции
#define MAX_CORRECTION_BUFFER_LENGTH 1200.0

/// Минимальная длина участка коррекции
#define MIN_CORRECTION_BUFFER_LENGTH 100.0

//----------------------------------- Причины исключения из оценки -----------------------------------------

#define EXCLUSION_REASON_SWITCH L"Стрелка или уравнительный прибор"
#define EXCLUSION_REASON_HARDWARE_FAIL L"Сбой аппаратуры"
#define EXCLUSION_REASON_BAD_PASSPORT L"Несоответствие паспортных данных"
#define EXCLUSION_REASON_BAD_COORDINATES L"Ошибка привязки"
#define EXCLUSION_REASON_PROGRAM_ERROR L"Ошибка ПМО"
#define EXCLUSION_REASON_SIDEWAY L"Съезд на боковой путь"

//--------------------------------------------- Километры -----------------------------------------------------

/// Расстояние принятие решения о готовности километра
#define KM_READY_DISTANCE 300.0;

/// Извлекает просадку
class VPitExtractor
{
private:
	/// Сторона
	side_t side;
public:
	/// Конструктор
	VPitExtractor(side_t in_side);
	/// Оператор()
	VPoint operator()(const VRtParameter& in_record) const;
};

/// Извлекает ШК
class VWidthExtractor
{
public:
	VPoint operator()(const VRtParameter& in_record) const;
};

/// Извлекает рихтовку
class VRichExtractor
{
private:
	/// Сторона
	side_t side;
public:
	/// Конструктор
	VRichExtractor(side_t in_side);
	/// Оператор()
	VPoint operator()(const VRtParameter& in_record) const;
};

/// Извлекает уровень
class VLevelExtractor
{
public:
	/// Оператор()
	VPoint operator()(const VRtParameter& in_record) const;
};

/// Извлекает кривизну
class VCurvExtractor
{
public:
	/// Оператор()
	VPoint operator()(const VRtParameter& in_record) const;
};


/// Извлекает износ
class VWearExtractor
{
private:
	/// Сторона
	side_t side;
public:
	/// Конструктор
	VWearExtractor(side_t in_side);
	/// Конструктор
	VWearExtractor();
	/// operartor()
	VPoint operator()(const VRtParameter& in_record) const;
};

/// Извлекает зазор
class VZazorExtractor
{
private:
	/// Сторона
	side_t side;
public:
	/// Конструктор
	VZazorExtractor(side_t in_side);
	/// operartor()
	VPoint operator()(const VRtParameter& in_record) const;
};

/// Извлекает температуру рельса
class VTemperatureExtractor
{
private:
	/// Сторона
	side_t side;
public:
	/// Конструктор
	VTemperatureExtractor(side_t in_side);
	/// operartor()
	VPoint operator()(const VRtParameter& in_record) const;
};


/// Вычисляет отклонение от нулевой линии возвышения по паспорту
class VLevelPassportVariate : public VDataProcessor<VPoint, VPoint>
{
private:	
	/// Очередь данных на входе
	std::deque<VPoint> input;
	/// Очередь данных на выходе
	std::queue<VPoint> output;
public:
	/// Конструктор
	VLevelPassportVariate();
	/// Деструктор
	~VLevelPassportVariate();
	/// Ввод данных
	void Put(const VPoint& in_point);	
	/// Возвращает true, если есть обработанные данные
	virtual bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VPoint& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();
};

/// Вычисляет отклонение от нулевой линии рихтовки
class VRichPassportVariate : public VDataProcessor<VPoint, VPoint>
{
private:
	/// Сторона
	side_t side;
	/// Очередь данных на входе
	std::deque<VPoint> input;
	/// Очередь данных на выходе
	std::queue<VPoint> output;
public:
	/// Конструктор
	VRichPassportVariate(side_t in_side);
	/// Деструктор
	~VRichPassportVariate();
	/// Ввод данных
	void Put(const VPoint& in_point);	
	/// Возвращает true, если есть обработанные данные
	virtual bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VPoint& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();
};

typedef boost::shared_ptr< VDataProcessor<VPoint, VPoint> > VPointToPointProcessorPtr;
typedef boost::shared_ptr< VDataProcessor<VExtremum, VWave> > VExtremumToWaveProcessorPtr;

// Условия отсупления
class VOverstepCondition
{
public:
	/// Проверяет выполнение условий
	virtual bool TestValue(double in_value, double in_length) const = 0;	
};

/// Описание отступления
class VBasicOverstep
{
protected:
	/// Координата гачала
	double start;
	/// Координата конца
	double end;	
	/// Путевая координата отступления
	VWayCoord waycoord;
	/// Максимальное значение
	double max_value;
	/// Минимальное значение
	double min_value;		
	/// Величина отступления
	double value;	
	/// Длина
	double length;
	/// Описание типа отступления
	std::wstring type;
	/// Степень отступления
	DEGREE degree;
	/// Близость к верхней границе (к следующей степени)
	bool close_to_upper_bound;
	/// Cкорости по приказу
	VSpeeds speeds;
	/// Ограничение скорости
	VSpeeds restriction;	
	/// Комментарий
	std::wstring comment;	
	/// Баллы
	int penalty;
	/// Признак того, что отступление должно быть исключено из оценки
	bool is_excluded;
	/// Причина исключения из оценки
	std::wstring exclusion_reason;
	
public:	
	/// Конструктор
	VBasicOverstep();
	/// Конструктор
	VBasicOverstep(double in_start, double in_end, double in_max_value, double in_min_value, 
		double in_value, double in_length, const std::wstring& in_type, DEGREE in_degree=DEGREE_NONE);		
	///Деструктор
	virtual ~VBasicOverstep(){};
	/// Возвращает координату начала
	virtual double GetStart() const;
	/// Возвращает координату конца
	virtual double GetEnd() const;
	/// Возвращает максимальное значение
	virtual double GetMax() const;
	/// Возвращает минимальное значение
	virtual double GetMin() const;		
	/// Возвращает среднее значение
	virtual double GetAverage() const;
	/// Возвращает координату
	virtual double GetCoord() const;
	/// Возвращает координату
	virtual const VWayCoord& GetWayCoord() const;
	/// Возвращает абсолютное значение
	virtual double GetValue() const;
	/// Возвращает длину
	virtual double GetLength() const;
	/// Устанавливает степень
	virtual void SetDegree(DEGREE in_degree);	
	/// Возвращает степень
	virtual DEGREE GetDegree() const;
	/// Возвращает признак близости к следующей степени
	virtual bool CloseToUpperBound() const;
	/// Вовращает описание
	virtual std::wstring GetDescription() const;	
	/// Добавляет комментарий
	virtual void AddComment(const std::wstring& in_comment);
	/// Возвращает комментарий
	virtual std::wstring GetComment() const;
	///Возвращает true, если границы отступлений перекрываются
	virtual bool Intersection(const VBasicOverstep& in_overstep) const;
	///Возвращает расстояние между двумя отступлениями
	virtual double Distance(const VBasicOverstep& in_overstep) const;
	/// Возвращает true, если отступление указанного типа
	virtual bool IsType(const std::wstring& in_type) const;		
	/// Вернуть тип отступления
	virtual const std::wstring& GetType() const;
	/// Возвращает группу
	virtual const OVERSTEP_GROUP GetGroup() const;
	/// Вернуть скорости
	virtual const VSpeeds& GetSpeeds() const;	
	/// Вернуть ограничение скорости
	virtual const VSpeeds& GetRestriction() const;
	/// Установить ограничение скорости
	virtual void SetRestriction(const VSpeeds& in_speeds);
	/// Отменить закрытие скорости
	virtual void CorrectClosure();
	/// Возвращает true если скорость ограничена
	virtual bool HasRestriction() const;	
	/// Возвращает число элементарных отступлений
	virtual size_t GetAmount() const;
	/// Возвращает балловую оценку
	virtual int GetPenalty() const;	
	/// Исключает отступление из оценки
	virtual void Exclude(const std::wstring& in_reason);
	/// Возвращает true если отступление должно быть исключено из оценки
	virtual bool IsExcluded() const;
	/// Возвращает причину исключения из оценки
	virtual const std::wstring& GetExclusionReason() const;

	// Общий интерфейс

	/// Возвращаее строку с описанием координаты
	virtual std::wstring GetCoordString() const;
	/// Возвращаее строку с описанием типа
	virtual std::wstring GetTypeString() const;
	/// Возвращает строку с описанием степени
	virtual std::wstring GetDegreeString() const;
	/// Возвращает строку с описанием значения
	virtual std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	virtual std::wstring GetLengthString() const;
	/// Возвращает строку с описанием количества элементарных отступлений
	virtual std::wstring GetAmountString() const;
	/// Возвращает строку с балловой оценкой
	virtual std::wstring GetPenaltyString() const;
	/// Возвращает описание ограничений скорости
	virtual std::wstring GetRestrictionString() const;		
};
typedef boost::shared_ptr<VBasicOverstep> VBasicOverstepPtr;

static bool operator<(const VBasicOverstep& l, const VBasicOverstep& r)
{
	return l.GetCoord() < r.GetCoord();
}

static bool operator<(const VBasicOverstepPtr& l, const VBasicOverstepPtr& r)
{
	return l->GetCoord() < r->GetCoord();
}

static bool operator>(const VBasicOverstep& l, const VBasicOverstep& r)
{
	return l.GetCoord() > r.GetCoord();
}

static bool operator>(const VBasicOverstepPtr& l, const VBasicOverstepPtr& r)
{
	return l->GetCoord() > r->GetCoord();
}

/// Предикат для выбора худшего отступления
struct worst_overstep
{
public:
	bool operator()(const VBasicOverstepPtr& l, const VBasicOverstepPtr& r)
	{
		return l->GetDegree()<r->GetDegree() || l->GetDegree()==r->GetDegree() && l->GetValue()<r->GetValue();
	};
};

/// Проверяет является ли отступление отсуплением по рихтовке
static bool is_rich(const VBasicOverstepPtr& in_overstep) {return in_overstep->IsType(RICH_LEFT) || in_overstep->IsType(RICH_RIGHT);};
/// Проверяет является ли отступление просадкой
static bool is_pit(const VBasicOverstepPtr& in_overstep) {return in_overstep->IsType(PIT_LEFT) || in_overstep->IsType(PIT_RIGHT);};
/// Проверяет является ли отступление перекосом
static bool is_distortion(const VBasicOverstepPtr& in_overstep) {return in_overstep->IsType(DISTORTION);};

/// Отступление - сбой параметров
class VFallbackOverstep : public VBasicOverstep
{
private:
	/// Сообщение
	std::wstring message;
	/// Группа
	OVERSTEP_GROUP group;
public:
	
	/// Конструктор
	VFallbackOverstep(double in_coord, OVERSTEP_GROUP in_group, const std::wstring& in_type)
	{
		start=in_coord;
		end=in_coord;
		waycoord=SITUATION.GetRealCoord(in_coord);
		max_value=0;
		min_value=0;
		value=0;
		length=0;
		type=in_type;
		degree=DEGREE_NONE;
		group=in_group;
	}
	
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const
	{
		return L"";
	}
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const
	{
		return L"";
	}
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return group;};
};

/// Отступление - сбой параметров
template<OVERSTEP_GROUP group> class VCorruptOverstep : public VBasicOverstep
{	
public:
	/// Конструктор
	VCorruptOverstep(double in_start, double in_end, const std::wstring& in_type)
	{
		start=in_start;
		end=in_end;
		waycoord=SITUATION.GetRealCoord((start+end)/2);
		max_value=0;
		min_value=0;
		value=0;
		length=fabs(in_start-in_end);
		type=in_type;
		degree=DEGREE_NONE;
	}
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const
	{
		return L"";
	}
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return group;};
};

/// Отступление - сбой параметров на пикете
template<OVERSTEP_GROUP group> class VPkCorruptOverstep : public VBasicOverstep
{	
public:
	/// Конструктор
	VPkCorruptOverstep(double in_start, double in_end, double in_value, const std::wstring& in_type)
	{
		start=in_start;
		end=in_end;
		waycoord=SITUATION.GetRealCoord((start+end)/2);
		max_value=0;
		min_value=0;
		value=in_value;
		length=fabs(in_start-in_end);
		type=in_type;
		degree=DEGREE_NONE;
	}
	/// Возвращает строку с описанием положения
	std::wstring GetCoordString() const
	{
		return boost::str(boost::wformat(L"ПК%d") % SITUATION.GetPk(GetCoord()).pk);
	}
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const
	{
		return L"";
	}
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const
	{
		return boost::str(boost::wformat(L"%0.0f%%") % GetValue());
	}
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return group;};
};


/// Отступление - контрольный участок
class VControlSectionOverstep : public VBasicOverstep
{
private:
	/// Среднее значение (факт)
	double value_real;
	/// Среднее значение (база)
	double value_apr;	
	
public:
	/// Конструктор
	VControlSectionOverstep(double in_start, double in_end, double in_value_real, 
		double in_value_apr, const std::wstring& in_type);
	/// Возвращает координату
	double GetCoord() const;
	/// Возвращает строку с описанием типа
	std::wstring GetTypeString() const;
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Возвращает строку с описанием количества элементарных отступлений
	std::wstring GetAmountString() const;
	/// Возвращает описание ограничений скорости
	std::wstring GetRestrictionString() const;	
	/// Возвращает комментарий
	std::wstring GetComment() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_SERVICE_GEOMETRY;};
};

/// Отступление по уширению
class VWidenOverstep : public VBasicOverstep
{
private:
	/// Признак нахождения на стрелке
	bool on_switch;
	/// Признак нахождения в зоне уравнительного прибора
	bool on_chamfered_joint;
	/// Норма
	double norma;
	/// Радиус кривой
	double radius;
public:	
	/// Конструктор
	VWidenOverstep(const VExcess& in_excess, DEGREE in_degree);	
	/// Возвращает число элементарных отступлений
	size_t GetAmount() const;
	/// Возвращает строку с описанием количества элементарных отступлений
	std::wstring GetAmountString() const;	
	/// Возвращает балловую оценку
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VWidenOverstep> VWidenOverstepPtr;


/// Оперативное отступление по уширению
class VWidenExpress : public VBasicOverstep
{
public:
	/// Конструктор
	VWidenExpress(const VExcess& in_excess);
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXPRESS;};
	/// Возвращает описание ограничений скорости
	std::wstring GetRestrictionString() const {return L"";};		
};
typedef boost::shared_ptr<VWidenExpress> VWidenExpressPtr;

/// Отступление по сужению
class VNarrowOverstep : public VBasicOverstep
{
private:
	/// Признак нахождения на стрелке
	bool on_switch;
	/// Признак нахождения в зоне уравнительного прибора
	bool on_chamfered_joint;
	/// Норма
	double norma;
	/// Признак ЖбШ
	bool zbsh;
public:
	/// Конструктор
	VNarrowOverstep(const VExcess& in_excess, DEGREE in_degree);	
	/// Возвращает число элементарных отступлений
	size_t GetAmount() const;
	/// Возвращает строку с описанием количества элементарных отступлений
	std::wstring GetAmountString() const;	
	/// Балловая оценка отступления
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VNarrowOverstep> VNarrowOverstepPtr;

/// Экспресс оценка сужения
class VNarrowExpress : public VBasicOverstep
{
public:
	/// Конструктор
	VNarrowExpress(const VExcess& in_excess);
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXPRESS;};
	/// Возвращает описание ограничений скорости
	std::wstring GetRestrictionString() const {return L"";};
};
typedef boost::shared_ptr<VNarrowExpress> VNarrowExpressPtr;

/// Отступление Ш10
class VW10Overstep : public VBasicOverstep
{
private:
	/// Число отступлений
	double amount;
public:
	/// Конструктор
	VW10Overstep(double in_start, double in_end, double in_amount);	
	/// Возвращает число элементарных отступлений
	size_t GetAmount() const;	
	/// Возвращает балловую оценку
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const;
	/// Возвращаее строку с описанием координаты
	std::wstring GetCoordString() const;	
	/// Возвращает строку с описанием степени
	std::wstring GetDegreeString() const;
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Возвращает строку с описанием количества элементарных отступлений
	std::wstring GetAmountString() const;
	/// Возвращает описание ограничений скорости
	std::wstring GetRestrictionString() const;		
};
typedef boost::shared_ptr<VW10Overstep> VW10OverstepPtr;

/// Отступление К100
class VK100Overstep : public VBasicOverstep
{
private:
	/// Число отступлений
	double amount;
public:
	/// Конструктор
	VK100Overstep(double in_start, double in_end, double in_amount);	
	/// Возвращает число элементарных отступлений
	size_t GetAmount() const;	
	/// Возвращает балловую оценку
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const;
	/// Возвращаее строку с описанием координаты
	std::wstring GetCoordString() const;	
	/// Возвращает строку с описанием степени
	std::wstring GetDegreeString() const;
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Возвращает строку с описанием количества элементарных отступлений
	std::wstring GetAmountString() const;
	/// Возвращает описание ограничений скорости
	std::wstring GetRestrictionString() const;		
};
typedef boost::shared_ptr<VK100Overstep> VK100OverstepPtr;


/// Отступление К60
class VK60Overstep : public VBasicOverstep
{
private:
	/// Число отступлений
	double amount;
public:
	/// Конструктор
	VK60Overstep(double in_start, double in_end, double in_amount);	
	/// Возвращает число элементарных отступлений
	size_t GetAmount() const;	
	/// Возвращает балловую оценку
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const;
	/// Возвращаее строку с описанием координаты
	std::wstring GetCoordString() const;	
	/// Возвращает строку с описанием степени
	std::wstring GetDegreeString() const;
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Возвращает строку с описанием количества элементарных отступлений
	std::wstring GetAmountString() const;
	/// Возвращает описание ограничений скорости
	std::wstring GetRestrictionString() const;		
};
typedef boost::shared_ptr<VK60Overstep> VK60OverstepPtr;


/// Отступление по отводу ШК
class VOtvodWidthOverstep : public VBasicOverstep
{
private:
	/// Признак нахождения на стрелке
	bool on_switch;
	/// Признак нахождения в зоне уравнительного прибора
	bool on_chamfered_joint;
public:
	/// Конструктор
	VOtvodWidthOverstep(const VExcess& in_excess);	
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
	/// Возвращает балловую оценку
	int GetPenalty() const;
};
typedef boost::shared_ptr<VOtvodWidthOverstep> VOtvodWidthOverstepPtr;

/// Отсупление по отводу возвышения
class VOtvodLevelOverstep : public VBasicOverstep
{
private:
public:
	/// Конструктор
	VOtvodLevelOverstep(const VExcess& in_excess);	
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
	/// Возвращает балловую оценку
	int GetPenalty() const;
};

/// Отступление по износу
class VWearOverstep : public VBasicOverstep
{
private:
	///Сторона
	side_t side;
	/// Признак нахождения на стрелке
	bool on_switch;
	/// Признак нахождения в зоне уравнительного прибора
	bool on_chamfered_joint;
	/// Признак участка обкатки
	MACHINE_TYPE test_run;
public:
	/// Конструктор
	VWearOverstep(const VExcess& in_excess, side_t in_side, DEGREE in_degree);	
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXTRA;};
};
typedef boost::shared_ptr<VWearOverstep> VWearOverstepPtr;


/// Оперативное отступление по износу
class VWearExpress : public VBasicOverstep
{
private:
	///Сторона
	side_t side;
public:
	/// Конструктор
	VWearExpress(const VExcess& in_excess, side_t in_side);
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXPRESS;};			
	/// Возвращает описание ограничений скорости
	std::wstring GetRestrictionString() const {return L"";};		
};
typedef boost::shared_ptr<VWearExpress> VWearExpressPtr;


/// Отступление по уровню
class VLevelOverstep : public VBasicOverstep
{
private:
	/// Признак моста
	bool bridge;
	/// Признак участка обкатки
	MACHINE_TYPE test_run;
public:
	/// Конструктор
	VLevelOverstep(const VExcess& in_excess, DEGREE in_degree);	
	/// Возвращает число элементарных отступлений
	size_t GetAmount() const;
	/// Возвращает строку с описанием количества элементарных отступлений
	std::wstring GetAmountString() const;
	/// Балловая оценка отступления
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VLevelOverstep> VLevelOverstepPtr;

/// Отсупление по отводу уровня
class VShortDistortionOverstep : public VBasicOverstep
{
public:
	/// Конструктор
	VShortDistortionOverstep(const VExcess& in_excess);
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
	/// Возвращает балловую оценку
	int GetPenalty() const;
};
typedef boost::shared_ptr<VShortDistortionOverstep> VShortDistortionOverstepPtr;

/// Отступление по просадке
class VPitOverstep : public VBasicOverstep
{
private:
	/// Признак моста
	bool bridge;
	/// Признак кривой (ПРЖ)
	bool prj;
	/// Признак обкатки
	MACHINE_TYPE test_run;
public:
	/// Конструктор
	VPitOverstep(const VWave& in_wave, double in_value, double in_length, side_t in_side, DEGREE in_degree);
	/// Балловая оценка отступления
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};

/// Повторяющиеся просадки
class VPitGroupOverstep : public VBasicOverstep
{
private:
	/// Отступления входящие в группу
	std::deque<VBasicOverstepPtr> group;
public:
	/// Конструктор
	VPitGroupOverstep(const std::deque<VBasicOverstepPtr>& in_group, side_t in_side);	
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Балловая оценка отступления
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VPitGroupOverstep> VPitGroupOverstepPtr;

/// Повторяющиеся рихтовки
class VRichGroupOverstep : public VBasicOverstep
{
private:
	/// Отступления входящие в группу
	std::deque<VBasicOverstepPtr> group;
	/// Сторона
	side_t side;
public:
	/// Конструктор
	VRichGroupOverstep(const std::deque<VBasicOverstepPtr>& in_group, side_t in_side);
	/// Возвращает строку с описанием типа
	std::wstring GetTypeString() const;
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Балловая оценка отступления
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VRichGroupOverstep> VRichGroupOverstepPtr;

/// Отступление по перекосу
class VDistortionOverstep : public VBasicOverstep
{
private:
	/// Признак моста
	bool bridge;
	/// Признак кривой (ПРЖ)
	bool prj;
	/// Признак обкатки
	MACHINE_TYPE test_run;
public:
	/// Конструктор
	VDistortionOverstep(const VWave& in_wave, double in_value, double in_length, DEGREE in_degree);		
	/// Балловая оценка отступления
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VDistortionOverstep> VDistortionOverstepPtr;

/// Повторяющиеся перекосы
class VDistortionGroupOverstep : public VBasicOverstep
{
private:
	/// Отступления входящие в группу
	std::deque<VBasicOverstepPtr> group;
public:
	/// Конструктор
	VDistortionGroupOverstep(const std::deque<VBasicOverstepPtr>& in_group);	
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Балловая оценка отступления
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VDistortionGroupOverstep> VDistortionGroupOverstepPtr;


/// Отступление по рихтовке
class VRichOverstep : public VBasicOverstep
{
private:	
	/// Признак моста
	bool bridge;
	/// Признак кривой
	bool curve;
	/// Признак ПРЖ
	bool prj;
	/// Признак стрелки
	bool on_switch;
	/// Признак нахождения в зоне уравнительного прибора
	bool on_chamfered_joint;
	/// Признак рихтовочной нити
	bool thread_side;
	/// Признак обкатки после ремонта
	MACHINE_TYPE test_run;
public:
	/// Конструктор
	VRichOverstep(const VWave& in_wave, double in_value, double in_length, side_t in_side, DEGREE in_degree);	
	/// Возвращаее строку с описанием типа
	std::wstring GetTypeString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Балловая оценка отступления
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VRichOverstep> VRichOverstepPtr;


/// Сочетание рихтовка - просадка
class VRichAndPitCombination : public VBasicOverstep
{
private:
	/// Рихтовка
	VBasicOverstepPtr rich_overstep;
	/// Просадка
	VBasicOverstepPtr pit_overstep;
	/// Признак ПРЖ
	bool prj;
public:
	/// Конструктор
	VRichAndPitCombination(VBasicOverstepPtr in_overstep1, VBasicOverstepPtr in_overstep2);	
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Балловая оценка отступления
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VRichAndPitCombination> VRichAndPitCombinationPtr;

/// Сочетание рихтовка - перекос
class VRichAndDistortionCombination : public VBasicOverstep
{
private:
	/// Рихтовка
	VBasicOverstepPtr rich_overstep;
	/// Перекос
	VBasicOverstepPtr distortion_overstep;
	/// Признак ПРЖ
	bool prj;
public:
	/// Конструктор
	VRichAndDistortionCombination(VBasicOverstepPtr in_overstep1, VBasicOverstepPtr in_overstep2);	
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Балловая оценка отступления
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VRichAndDistortionCombination> VRichAndDistortionCombinationPtr;

/// Данные для оценки кривой
class VCurveCalc
{
private:
	struct DATA
	{
		VPoint curv;
		VPoint level;
		DATA(const VPoint& in_curv, const VPoint& in_level) : curv(in_curv), level(in_level) {};
		double coord() const {return curv.GetCoord();};
		double anp(int in_speed) const {return calc_anp(in_speed, curv.GetValue(), level.GetValue());};
		bool corrupt() const {return curv.GetCorrupt() || level.GetCorrupt();};
		
	};
	/// Данные
	std::deque<DATA> data;
	/// Знак кривизны/уровня на кривой
	double curve_sign;
public:

	/// Добавляет данные
	void Put(const VPoint& in_curv, const VPoint& in_level);
	/// Возвращает среднее значение Анп для заданной скорости
	double GetAverageAnp(int in_speed) const;
	/// Возвращает максимальное значение Анп для заданной скорости
	VPoint GetMaxAnp(int in_speed) const;
	/// Возвращает максимальное значение Пси для заданной скорости
	VPoint GetMaxPsi(int in_speed) const;
	/// Возвращает начало участка отступления
	double GetStart() const;
	/// Возвращает конец участка
	double GetEnd() const;
	/// Очищает массив данных
	void Clear();
	/// Возвращает число кадров данных
	size_t GetCount() const;
	/// Возвращает Пси на всем участке
	double GetPsi(int in_speed) const;
};

/// Отступление по среднему Анп на круговой
class VAnpAverageOverstep : public VBasicOverstep
{
private:
	double anp_high_bt;
	double anp_high;
	double anp_pass;
	double anp_truck;
public:
	/// Конструктор
	VAnpAverageOverstep(const VCurveCalc& in_calc);
	/// Возвращает строку с описанием степени
	std::wstring GetDegreeString() const;
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Возвращает строку с описанием баллов
	std::wstring GetPenaltyString() const;
	/// Возвращает балловую оценку
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};


/// Отступление по Анп в точке
class VAnpHorizontalOverstep : public VBasicOverstep
{
private:
	double anp_high_bt;
	double anp_high;
	double anp_pass;
	double anp_truck;
public:
	/// Конструктор
	VAnpHorizontalOverstep(const VCurveCalc& in_calc);
	/// Возвращает строку с описанием степени
	std::wstring GetDegreeString() const;
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Возвращает строку с описанием баллов
	std::wstring GetPenaltyString() const;
	/// Возвращает балловую оценку
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};

/// Отступление Пси в точке
class VPsiOverstep : public VBasicOverstep
{
private:
	double psi_high_bt;
	double psi_high;
	double psi_pass;
	double psi_truck;
public:
	/// Конструктор
	VPsiOverstep(const VCurveCalc& in_calc);
	/// Возвращает строку с описанием степени
	std::wstring GetDegreeString() const;
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Возвращает строку с описанием баллов
	std::wstring GetPenaltyString() const;
	/// Возвращает балловую оценку
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};

/// Отступление Пси на переходной
class VPsiTransitionOverstep : public VBasicOverstep
{
private:
	double psi_high_bt;
	double psi_high;
	double psi_pass;
	double psi_truck;
public:
	/// Конструктор
	VPsiTransitionOverstep(const VCurveCalc& in_calc);
	/// Возвращает строку с описанием степени
	std::wstring GetDegreeString() const;
	/// Возвращает строку с описанием значения
	std::wstring GetValueString() const;
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Возвращает строку с описанием баллов
	std::wstring GetPenaltyString() const;
	/// Возвращает балловую оценку
	int GetPenalty() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};

/// Оперативное отступление по зазору
class VZazorExpress : public VBasicOverstep
{
private:
	///Сторона
	side_t side;
public:
	/// Конструктор
	VZazorExpress(const VPoint& in_point, side_t in_side);
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXPRESS;};			
	/// Возвращает строку с описанием длины
	virtual std::wstring GetLengthString() const {return L"";};
	/// Возвращает описание ограничений скорости
	std::wstring GetRestrictionString() const {return L"";};		
};
typedef boost::shared_ptr<VZazorExpress> VZazorExpressPtr;

/// Сверхнормативный зазор
class VZazorOverstep : public VBasicOverstep
{
private:
	/// Сторона
	side_t side;
	/// Диаметр отверстий в рельсах
	double diameter;
	/// Температура
	double temp;
public:
	/// Конструктор
	VZazorOverstep(const VPoint& in_point, double in_temp, side_t in_side);	
	/// Возвращает строку с описанием длины
	std::wstring GetLengthString() const;
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXTRA;};
};


/// Нулевые зазоры
class VZeroZazorOverstep : public VBasicOverstep
{
private:
	/// Сторона
	side_t side;	
public:
	/// Конструктор
	VZeroZazorOverstep(double in_start, double in_end, double in_value, side_t in_side);		
	/// Возвращает группу
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXTRA;};
};

/// Класс поиска превышения некоторой величиной порога
class VFindExcess : public VDataOut<VExcess>
{
private:	
	/// Данные на входе
	std::deque<VPoint> input;
	/// Данные на выходе
	std::deque<VExcess> output;
public:
	/// Конструктор
	VFindExcess();
	/// Сброс
	void Reset();	
	/// Помещает на вход значение
	void Input(const VPoint& in_point, const VOverstepCondition& in_condition);
	/// Возвращает признак наличия данных
	virtual bool Ready() const;
	/// Возвращает признак пустого списка
	virtual bool Empty() const;
	/// Возвращает обнаруженное отступление
	virtual const VExcess& Front() const;	
	/// Удаляет отступление из очереди
	virtual void Pop();
	/// Сбрасывает текущее отсупление в выходной буфер
	void Flush();
};

/// Поиск пар экстремумов
class VFindWave : public VDataProcessor<VExtremum, VWave>
{
private:
	/// Очередь точек на входе
	std::deque<VExtremum> input;
	/// Очередь пар экстремумов на выходе
	std::queue<VWave> output;
	/// Разрешение по оси X
	double resolution_x;
	/// Разрешение по оси Y
	double resolution_y;
public:
	/// Конструктор
	VFindWave();
	/// Конструктор
	VFindWave(double in_resolution_x, double in_resolution_y);
	/// Деструктор
	~VFindWave();
	/// Добавляет отступление
	virtual void Put(const VExtremum& in_point);
	/// Возвращает признак наличия данных
	virtual bool Ready() const;
	/// Возвращает признак пустого списка
	virtual bool Empty() const;
	/// Возвращает обнаруженное отступление
	virtual const VWave& Front() const;	
	/// Удаляет отступление из очереди
	virtual void Pop();	
};

/// Фильтрация полуволн
class VWaveFilter : public VDataProcessor<VWave,VWave>
{
private:
	/// Данные на выходе
	std::deque<VWave> output;
public:
	/// Конструктор
	VWaveFilter();
	/// Деструктор
	~VWaveFilter();
	/// Помещает данные на вход
	virtual void Put(const VWave& in_wave);
	/// Возвращает признак наличия данных
	virtual bool Ready() const;
	/// Возвращает признак пустого списка
	virtual bool Empty() const;
	/// Возвращает обнаруженное отступление
	virtual const VWave& Front() const;	
	/// Удаляет данные из очереди
	virtual void Pop();
};

/// Объедиенение полуволн
class VWaveUnion : public VDataProcessor<VWave,VWave>
{
private:
	/// Данные на входе
	std::deque<VWave> input;
	/// Данные на выходе
	std::deque<VWave> output;
	/// Дистанция объединения
	double union_distance;
public:
	/// Конструктор
	VWaveUnion(double in_union_distance);
	/// Деструктор
	~VWaveUnion();
	/// Помещает данные на вход
	virtual void Put(const VWave& in_wave);
	/// Возвращает признак наличия данных
	virtual bool Ready() const;
	/// Возвращает признак пустого списка
	virtual bool Empty() const;
	/// Возвращает обнаруженное отступление
	virtual const VWave& Front() const;	
	/// Удаляет отступление из очереди
	virtual void Pop();
	/// Слив данных
	virtual void Flush();
};

/// Выбор полуволн из группы
class VWaveSelector : public VDataProcessor<VBasicOverstepPtr,VBasicOverstepPtr>
{
private:
	/// Данные на входе
	std::deque<VBasicOverstepPtr> input;
	/// Данные на выходе
	std::deque<VBasicOverstepPtr> output;
public:
	/// Конструктор
	VWaveSelector();
	/// Деструктор
	~VWaveSelector();
	/// Помещает на вход значение
	virtual void Put(const VBasicOverstepPtr& in_wave);
	/// Возвращает признак наличия данных
	virtual bool Ready() const;
	/// Возвращает признак пустого списка
	virtual bool Empty() const;
	/// Возвращает обнаруженное отступление
	virtual const VBasicOverstepPtr& Front() const;	
	/// Удаляет отступление из очереди
	virtual void Pop();	
	/// Слив данных
	virtual void Flush();
};


/// Оценка просадки
class VCheckPit : public VDataProcessor<VWave, VBasicOverstepPtr>
{
private:	
	/// Список отступлений на выходе
	std::deque<VBasicOverstepPtr> output;
	/// Сторона
	side_t side;
public:
	/// Конструктор
	VCheckPit(side_t in_side);
	/// Деструктор
	~VCheckPit();
	/// Помещает на вход значение
	void Put(const VWave& in_wave);
	/// Возвращает признак наличия данных
	virtual bool Ready() const;
	/// Возвращает признак пустого списка
	virtual bool Empty() const;
	/// Возвращает обнаруженное отступление
	virtual const VBasicOverstepPtr& Front() const;	
	/// Удаляет отступление из очереди
	virtual void Pop();	
};

/// Условие группы просадок
class VPitGroupCondition
{
public:
	bool operator()(const VBasicOverstepPtr& in_overstep) const
	{
		if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
		{
			return in_overstep->GetValue()>PIT_GROUP_VALUE_515 && !in_overstep->IsExcluded();
		}
		else
		{
			return !in_overstep->IsExcluded() && 
				(in_overstep->GetRestriction().GetPassSpeed()>60 && in_overstep->GetValue()>PIT_GROUP_VALUE && in_overstep->GetLength()<=PIT_GROUP_LENGTH ||
				 in_overstep->GetRestriction().GetMaxSpeed()>140 && in_overstep->GetDegree()==DEGREE3);
		}
	};
};

/// Условие группы рихтовок
class VRichGroupCondition
{
public:
	bool operator()(const VBasicOverstepPtr& in_overstep) const
	{
		if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
		{
			return in_overstep->GetDegree()==DEGREE3 && !in_overstep->IsExcluded();
		}
		else
		{
			return !in_overstep->IsExcluded() && 
				(in_overstep->GetRestriction().GetPassSpeed()>60 && in_overstep->GetValue()>RICH_GROUP_VALUE && in_overstep->GetLength()<=RICH_GROUP_LENGTH ||
				 in_overstep->GetRestriction().GetMaxSpeed()>140 && in_overstep->GetDegree()==DEGREE3);
		}
	};
};

/// Условие группы перекосов
class VDistortionGroupCondition
{
public:
	bool operator()(const VBasicOverstepPtr& in_overstep) const
	{
		if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
		{
			return false;
		}
		else
		{
			return !in_overstep->IsExcluded() && 
				(in_overstep->GetRestriction().GetPassSpeed()>60 && in_overstep->GetValue()>DISTORTION_GROUP_VALUE && in_overstep->GetLength()<=DISTORTION_GROUP_LENGTH ||
				 in_overstep->GetRestriction().GetMaxSpeed()>140 && in_overstep->GetDegree()==DEGREE3);
		}
	};
};

/// Условие сочетания рихтовки и просадки
class VRichAndPitCondition
{
private:
	/// Новое отступление
	VBasicOverstepPtr overstep;			
public:
	/// Конструктор
	VRichAndPitCondition(const VBasicOverstepPtr& in_overstep) : overstep(in_overstep){};
	/// Оператор()
	bool operator()(const VBasicOverstepPtr& in_overstep) const
	{	
		if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
		{
			return in_overstep->GetDegree()>=DEGREE3 &&
				overstep->GetDegree()>=DEGREE3 && 
				overstep->Intersection(*in_overstep) && 
				(is_rich(in_overstep) && is_pit(overstep) || is_rich(overstep) && is_pit(in_overstep));
		}
		else
		{				
			return overstep->Distance(*in_overstep)<COMBINATION_DISTANCE && 
				(is_rich(in_overstep) && is_pit(overstep) || is_rich(overstep) && is_pit(in_overstep));
		}
	};
};

/// Условие сочетания рихтовки и перекоса
class VRichAndDistortionCondition
{
private:
	/// Новое отступление
	VBasicOverstepPtr overstep;			
public:
	/// Конструктор
	VRichAndDistortionCondition(const VBasicOverstepPtr& in_overstep) : overstep(in_overstep){};
	/// Оператор()
	bool operator()(const VBasicOverstepPtr& in_overstep) const
	{				
		if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
		{	
			return in_overstep->GetDegree()>=DEGREE3 &&
				overstep->GetDegree()>=DEGREE3 && 
				overstep->Intersection(*in_overstep) && 
				(is_rich(in_overstep) && is_distortion(overstep) || is_rich(overstep) && is_distortion(in_overstep));
		}
		else
		{		
			return overstep->Distance(*in_overstep)<COMBINATION_DISTANCE && 
				(is_rich(in_overstep) && is_distortion(overstep) || is_rich(overstep) && is_distortion(in_overstep));
		}
	};
};

typedef VDataProcessor<VBasicOverstepPtr, VBasicOverstepPtr> VGroupDetector;
typedef boost::shared_ptr<VGroupDetector> VGroupDetectorPtr;

/// Оценка групп просадок
class VPitGroupDetector : public VGroupDetector
{
private:
	/// Очередь данных на входе
	std::deque<VBasicOverstepPtr> input;
	/// Очередь данных на выходе
	std::deque<VBasicOverstepPtr> output;
	/// Сторона
	side_t side;
	/// Текущее положение
	double position;
public:
	/// Конструктор
	VPitGroupDetector(side_t in_side);
	/// Деструктор
	~VPitGroupDetector();
	/// Помещает очередное отступление
	void Put(const VBasicOverstepPtr& in_overstep);
	/// Помещает значение текущей позиции
	void Put(const double& in_position);
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();
	/// Сброс буфера
	void Flush();
};


/// Оценка групп рихтовок
class VRichGroupDetector : public VGroupDetector
{
private:
	/// Очередь данных на входе
	std::deque<VBasicOverstepPtr> input;
	/// Очередь данных на выходе
	std::deque<VBasicOverstepPtr> output;
	/// Сторона
	side_t side;
	/// Текущее положение
	double position;
public:
	/// Конструктор
	VRichGroupDetector(side_t in_side);
	/// Деструктор
	~VRichGroupDetector();
	/// Помещает очередное отступление
	void Put(const VBasicOverstepPtr& in_overstep);
	/// Помещает значение текущей позиции
	void Put(const double& in_position);
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();
	/// Сброс буфера
	void Flush();
};

/// Оценка перекоса
class VCheckDistortion : public VDataProcessor<VWave, VBasicOverstepPtr>
{
private:	
	/// Список отступлений на выходе
	std::deque<VBasicOverstepPtr> output;
public:
	/// Конструктор
	VCheckDistortion();
	/// Деструктор
	~VCheckDistortion();
	/// Помещает на вход значение
	virtual void Put(const VWave& in_wave);
	/// Возвращает признак наличия данных
	virtual bool Ready() const;
	/// Возвращает признак пустого списка
	virtual bool Empty() const;
	/// Возвращает обнаруженное отступление
	virtual const VBasicOverstepPtr& Front() const;	
	/// Удаляет отступление из очереди
	virtual void Pop();	
};

/// Оценка групп перекосов
class VDistortionGroupDetector : public VGroupDetector
{
private:
	/// Очередь данных на входе
	std::deque<VBasicOverstepPtr> input;
	/// Очередь данных на выходе
	std::deque<VBasicOverstepPtr> output;
	/// Сторона
	side_t side;
	/// Текущее положение
	double position;
public:
	/// Конструктор
	VDistortionGroupDetector();
	/// Деструктор
	~VDistortionGroupDetector();
	/// Помещает очередное отступление
	void Put(const VBasicOverstepPtr& in_overstep);
	/// Помещает значение текущей позиции
	void Put(const double& in_position);
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();
	/// Сброс буфера
	void Flush();
};


/// Оценка рихтовки
class VCheckRich : public VDataProcessor<VWave, VBasicOverstepPtr>
{
private:	
	/// Список отступлений на выходе
	std::deque<VBasicOverstepPtr> output;
	/// Сторона
	side_t side;
	/// Таблица корректировки
	//VRichCorrectionTable RichCorrectionTable;
	VRichCorrectorPtr RichCorrector;
public:
	/// Конструктор
	VCheckRich(side_t in_side);
	/// Деструктор
	~VCheckRich();
	/// Помещает на вход значение
	void Put(const VWave& in_wave);
	/// Возвращает признак наличия данных
	virtual bool Ready() const;
	/// Возвращает признак пустого списка
	virtual bool Empty() const;
	/// Возвращает обнаруженное отступление
	virtual const VBasicOverstepPtr& Front() const;	
	/// Удаляет отступление из очереди
	virtual void Pop();	
};

/// Поиск сочетаний рихтовки и перекоса
class VRichAndDistortionDetector : public VGroupDetector
{
private:
	/// Очередь данных на входе
	std::deque<VBasicOverstepPtr> input;
	/// Очередь данных на выходе
	std::queue<VBasicOverstepPtr> output;
public:
	/// Конструктор
	VRichAndDistortionDetector();
	/// Деструктор
	~VRichAndDistortionDetector();
	/// Помещает очередное отступление
	virtual void Put(const VBasicOverstepPtr& in_overstep);
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();
};


/// Поиск сочетаний рихтовки и просадки
class VRichAndPitDetector : public VGroupDetector
{
private:
	/// Очередь данных на входе
	std::deque<VBasicOverstepPtr> input;
	/// Очередь данных на выходе
	std::queue<VBasicOverstepPtr> output;
public:
	/// Конструктор
	VRichAndPitDetector();
	/// Деструктор
	~VRichAndPitDetector();
	/// Помещает очередное отступление
	virtual void Put(const VBasicOverstepPtr& in_overstep);
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();
};

/// Поиск Ш10
class VW10OverstepDetector : public VGroupDetector
{
private:
	/// Количество отсуплений на километре
	double w10_amount;
	/// Текущий километр
	VAprinfoKmRecord aprinfo_km;
	/// Очередь данных на выходе
	std::deque<VBasicOverstepPtr> output;
public:
	/// Конструктор
	VW10OverstepDetector();
	/// Деструктор
	~VW10OverstepDetector();
	/// Помещает очередное отступление
	virtual void Put(const VBasicOverstepPtr& in_overstep);
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();
	/// Слив данных
	virtual void Flush();
};

/// Поиск К100
class VK100OverstepDetector : public VGroupDetector
{
private:
	/// Количество отсуплений на километре
	double k100_amount;
	/// Текущий километр
	VAprinfoKmRecord aprinfo_km;
	/// Очередь данных на выходе
	std::deque<VBasicOverstepPtr> output;
public:
	/// Конструктор
	VK100OverstepDetector();
	/// Деструктор
	~VK100OverstepDetector();
	/// Помещает очередное отступление
	virtual void Put(const VBasicOverstepPtr& in_overstep);
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();
	/// Слив данных
	virtual void Flush();
};

/// Поиск К60
class VK60OverstepDetector : public VGroupDetector
{
private:
	/// Количество отсуплений на километре
	double k60_amount;
	/// Текущий километр
	VAprinfoKmRecord aprinfo_km;
	/// Очередь данных на выходе
	std::deque<VBasicOverstepPtr> output;
public:
	/// Конструктор
	VK60OverstepDetector();
	/// Деструктор
	~VK60OverstepDetector();
	/// Помещает очередное отступление
	virtual void Put(const VBasicOverstepPtr& in_overstep);
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();
	/// Слив данных
	virtual void Flush();
};


/// Условие попадания в диапазон
class VRangeCondition : public VOverstepCondition
{
private:
	/// Диапазон значений
	pro::interval<double> value_range;
	/// Диапазон длин
	pro::interval<double> length_range;
public:
	/// Конструктор
	VRangeCondition(const pro::interval<double>& in_value_range, const pro::interval<double>& in_length_range=pro::interval<double>(0, DBL_MAX));
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;	
};

/// Условие левой кривой
class VCurveCondition : public VOverstepCondition
{
private:
	/// Диапазон значений
	pro::interval<double> range;
public:
	/// Конструкор
	VCurveCondition(side_t in_side, double in_min_curv=1) : range(in_side==SD_LEFT ?
		pro::interval<double>(0-DBL_MAX, -in_min_curv, false, false) : pro::interval<double>(in_min_curv, DBL_MAX, false, false)){};
	/// Проверяет условие
	bool TestValue(double in_value, double in_length=0) const
	{
		return range.contains(in_value);
	};
};

/// Условия уширения
class VWidenCondition : public VOverstepCondition
{
private:
	/// Степень
	DEGREE degree;
	/// Скорости
	VSpeeds speeds;
	/// Норма ШК
	double norma;	
	/// Радиус
	double radius;
	/// Износ
	double wear;
	
public:
	/// Конструктор
	VWidenCondition(DEGREE in_degree, VSpeeds in_speeds, double in_norma, double in_radius, double in_wear);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;	
};

/// Условия сужения
class VNarrowCondition : public VOverstepCondition
{
private:
	/// Степень
	DEGREE degree;
	/// Скорость
	VSpeeds speeds;
	/// Норма ШК
	double norma;
	/// Признак участка ЖБШ
	bool zbsh;	
public:
	/// Конструктор
	VNarrowCondition(DEGREE in_degree, VSpeeds in_speeds, double in_norma, bool in_zbsh);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;	
};

/// Условия отвода ШК (больше нуля)
class VOtvodWidthCondition : public VOverstepCondition
{
private:
	/// Скорость
	VSpeeds speeds;	
public:
	/// Конструктор
	VOtvodWidthCondition(VSpeeds in_speeds);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};


/// Условия отвода уровня для пассажирский и скоростных
class VOtvodLevelPassCondition : public VOverstepCondition
{
private:
	/// Скорость
	VSpeeds speeds;	
public:
	/// Конструктор
	VOtvodLevelPassCondition(VSpeeds in_speeds);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};

/// Условия отвода уровня для грузовых
class VOtvodLevelTruckCondition : public VOverstepCondition
{
private:
	/// Скорость
	VSpeeds speeds;	
public:
	/// Конструктор
	VOtvodLevelTruckCondition(VSpeeds in_speeds);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};

/// Условия отвода уровня
class VOtvodLevelCondition : public VOverstepCondition
{
private:
	/// Скорость
	VSpeeds speeds;	
public:
	/// Конструктор
	VOtvodLevelCondition(VSpeeds in_speeds);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};

/// Условия износа
class VWearCondition : public VOverstepCondition
{
private:	
	/// Степень
	DEGREE degree;	
public:
	/// Конструктор
	VWearCondition(DEGREE in_degree);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};

/// Условия зазора
class VZazorCondition : public VOverstepCondition
{
private:
	/// Скорости
	VSpeeds speeds;
	/// Диаметр отверстий в рельсах
	double diameter;
public:
	/// Конструктор
	VZazorCondition(const VSpeeds& in_speeds, double in_diameter=0);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length=0) const;
};

/// Условие просадки
class VPitCondition : public VOverstepCondition
{
private:
	/// Степень
	DEGREE degree;
	/// Скорость
	VSpeeds speeds;
	/// Признак наличия моста
	bool bridge;
public:
	/// Конструктор
	VPitCondition(DEGREE in_degree, VSpeeds in_speeds, bool in_bridge);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};

/// Условие просадки при оценке пути в период обкатки
class VPitConditionTestRun : public VOverstepCondition
{
private:
public:
	/// Конструктор
	VPitConditionTestRun();
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};

/// Условие плавного отклонения по уровню
class VLevelCondition : public VOverstepCondition
{
private:
	/// Степень
	DEGREE degree;
	/// Скорость
	VSpeeds speeds;	
public:
	/// Конструктор
	VLevelCondition(DEGREE in_degree, VSpeeds in_speeds);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};

/// Условие плавного отклонения по уровню в период обкатки
class VLevelConditionTestRun : public VOverstepCondition
{
private:
	/// Скорость
	VSpeeds speeds;
	/// Способ производства работ
	MACHINE_TYPE type;
public:
	/// Конструктор
	VLevelConditionTestRun(VSpeeds in_speeds, MACHINE_TYPE in_type);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};


/// Условие отвода по уровню
class VShortDistortionCondition : public VOverstepCondition
{
private:	
	/// Скорость
	VSpeeds speeds;
public:
	/// Конструктор
	VShortDistortionCondition(VSpeeds in_speeds);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};

/// Условие перекоса
class VDistortionCondition : public VOverstepCondition
{
private:
	/// Степень
	DEGREE degree;
	/// Скорость
	VSpeeds speeds;
	/// Признак моста
	bool bridge;
	
public:
	/// Конструктор
	VDistortionCondition(DEGREE in_degree, VSpeeds in_speeds, bool in_bridge);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};

/// Условие перекоса при обкатке пути
class VDistortionConditionTestRun : public VOverstepCondition
{
private:
	
	/// Скорость
	VSpeeds speeds;
	/// Способ производства работ
	MACHINE_TYPE type;
	
public:
	/// Конструктор
	VDistortionConditionTestRun(VSpeeds in_speeds, MACHINE_TYPE in_type);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};


/// Условие рихтовки
class VRichCondition : public VOverstepCondition
{
private:
	/// Степень
	DEGREE degree;
	/// Скорость
	VSpeeds speeds;	
	/// Признак наличия моста
	bool bridge;	
	/// Признак стороны рихтовочной нити
	bool thread_side;
	/// Признак кривой
	bool curve;
public:
	/// Конструктор
	VRichCondition(DEGREE in_degree, VSpeeds in_speeds, bool in_bridge, bool in_thread_side, bool in_curve);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};


/// Условие рихтовки в период обкатки после ремонта
class VRichConditionTestRun : public VOverstepCondition
{
private:
	/// Скорость
	VSpeeds speeds;	
	/// Способ производства работ
	MACHINE_TYPE type;

public:
	/// Конструктор
	VRichConditionTestRun(VSpeeds in_speeds, MACHINE_TYPE in_type);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length) const;
};

/// Условие непогашенного ускорения
class VAnpCondition : public VOverstepCondition
{
private:
	/// Порог
	double anp;
public:
	/// Конструктор
	VAnpCondition(double in_anp);
	/// Проверяет условие
	bool TestValue(double in_value, double in_length=0) const;
};

/// Условие скорости изменения непогашенного ускорения
class VPsiCondition : public VOverstepCondition
{
private:
	/// Ограничивать ли скорость
	bool restrict;
public:
	/// Конструктор
	VPsiCondition(bool in_restrict) : restrict(in_restrict) {};
	/// Проверяет условие
	bool TestValue(double in_value, double in_length=0) const;
};

/// Условие скорости изменения непогашенного ускорения
class VPsiBTCondition : public VOverstepCondition
{
private:
	/// Ограничивать ли скорость
	bool restrict;
public:
	/// Конструктор
	VPsiBTCondition(bool in_restrict) : restrict(in_restrict) {};
	/// Проверяет условие
	bool TestValue(double in_value, double in_length=0) const;
};

/// Условие среднего непогашенного ускорения (скор.)
class VAnpAverageHighCondition : public VOverstepCondition
{
private:
	/// Для ограничения скорости
	bool restrict;
public:
	/// Конструктор
	VAnpAverageHighCondition(bool in_restrict) : restrict(in_restrict) {};
	/// Проверяет условие
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 0.9 : 0.7);
	};
};

/// Условие среднего непогашенного ускорения (накл.)
class VAnpAverageHighBTCondition : public VOverstepCondition
{
private:
	/// Для ограничения скорости
	bool restrict;
public:
	/// Конструктор
	VAnpAverageHighBTCondition(bool in_restrict) : restrict(in_restrict) {};
	/// Проверяет условие
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 1.4 : 0.7);
	};
};

/// Условие среднего непогашенного ускорения (пасс.)
class VAnpAveragePassCondition : public VOverstepCondition
{
private:
	/// Для ограничения скорости
	bool restrict;
public:
	/// Конструктор
	VAnpAveragePassCondition(bool in_restrict) : restrict(in_restrict) {};
	/// Проверяет условие
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 0.75 : 0.7);
	};
};

/// Условие среднего непогашенного ускорения (груз.)
class VAnpAverageTruckCondition : public VOverstepCondition
{
private:
	/// Для ограничения скорости
	bool restrict;
public:
	/// Конструктор
	VAnpAverageTruckCondition(bool in_restrict) : restrict(in_restrict) {};
	/// Проверяет условие
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 0.7 : 0.7);
	};
};


/// Условие непогашенного ускорения (скор.)
class VAnpHighCondition : public VOverstepCondition
{
private:
	/// Для ограничения скорости
	bool restrict;
public:
	/// Конструктор
	VAnpHighCondition(bool in_restrict) : restrict(in_restrict) {};
	/// Проверяет условие
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 1.0 : 0.7);
	};
};

/// Условие непогашенного ускорения (накл.)
class VAnpHighBTCondition : public VOverstepCondition
{
private:
	/// Для ограничения скорости
	bool restrict;
public:
	/// Конструктор
	VAnpHighBTCondition(bool in_restrict) : restrict(in_restrict) {};
	/// Проверяет условие
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 1.5 : 0.7);
	};
};

/// Условие непогашенного ускорения (пасс.)
class VAnpPassCondition : public VOverstepCondition
{
private:
	/// Для ограничения скорости
	bool restrict;
public:
	/// Конструктор
	VAnpPassCondition(bool in_restrict) : restrict(in_restrict) {};
	/// Проверяет условие
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 0.8 : 0.7);
	};
};

/// Условие непогашенного ускорения (груз.)
class VAnpTruckCondition : public VOverstepCondition
{
private:
	/// Для ограничения скорости
	bool restrict;
public:
	/// Конструктор
	VAnpTruckCondition(bool in_restrict) : restrict(in_restrict) {};
	/// Проверяет условие
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 0.75 : 0.7);
	};
};

typedef VDataProcessor<VRtParameter, VBasicOverstepPtr> VOverstepDetector;
typedef boost::shared_ptr<VOverstepDetector> VOverstepDetectorPtr;

/// Класс оценки пустой
class VFallbackOverstepDetector : public VOverstepDetector
{
private:
	/// Группа
	OVERSTEP_GROUP group;
	/// Детектор смены километров
	change_calculator<long> km_change;
	/// Сообщение
	std::wstring type;
	/// Отступления на выходе
	std::deque<VBasicOverstepPtr> output;
	
public:
	///Конструктор
	VFallbackOverstepDetector(OVERSTEP_GROUP in_group, const std::wstring& in_type) : group(in_group), type(in_type) {};
	/// Деструктор
	~VFallbackOverstepDetector() {};
	/// Помещает данные на вход
	void Put(const VRtParameter& in_data)
	{
		if (km_change.put(SITUATION.GetRealCoord(in_data.absCoord).km))
		{
			output.push_back(VBasicOverstepPtr(new VFallbackOverstep(in_data.absCoord, group, type))); 
		}
	};
	/// Возвращает true, если есть готовые отступления
	bool Ready() const
	{
		return output.size()>0;
	};	
	/// Возвращает true, если отступлений нет
	bool Empty() const
	{
		return output.empty();
	};
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const
	{
		return output.front(); 
	};
	/// Выталкивает первый элемент
	void Pop()
	{
		output.pop_front();
	};
};

/// Класс поиска сбоев
template<typename T, OVERSTEP_GROUP group> class VCorruptDetector : public VOverstepDetector
{
private:
	T extractor;
	std::deque<VPoint> input;
	std::deque<VBasicOverstepPtr> output;
	std::wstring type;
	double length;
public:
	///Конструктор
	VCorruptDetector(const T& in_extractor, const std::wstring& in_type, double in_length) : extractor(in_extractor), type(in_type), 
		length(in_length){};
		
	/// Деструктор
	~VCorruptDetector() 
	{
		if (!Empty()) 
			LOG_ERROR(L"VCorruptDetector: Необработанные данные...");
	};

	/// Помещает данные на вход
	void Put(const VRtParameter& in_record)
	{
		VPoint point=extractor(in_record);
		if (point.GetCorrupt())
		{
			input.push_back(point);
		}
		else
		{
			Flush();
		}
	}

	/// Возвращает true, если есть готовые отступления
	bool Ready() const
	{
		return output.size()>0;
	}

	/// Возвращает true, если отступлений нет
	bool Empty() const
	{
		return output.empty();
	}

	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const
	{
		return output.front();
	}

	/// Выталкивает первый элемент
	void Pop()
	{
		output.pop_front();
	}

	/// Слив данных
	void Flush()
	{
		if (!input.empty() && fabs(input.back().GetCoord()-input.front().GetCoord()) > length)
		{
			output.push_back(VBasicOverstepPtr(new VCorruptOverstep<group>(input.front().GetCoord(), 
				input.back().GetCoord(), type)));
		}
		input.clear();	
	}
};

/// Класс оценки сбоев на пикете
template<typename T, OVERSTEP_GROUP group> class VPkCorruptDetector : public VOverstepDetector
{
private:
	T extractor;
	std::deque<VPoint> input;
	std::deque<VBasicOverstepPtr> output;
	std::wstring type;
	change_calculator<long> pk_changed;
	double percent;
public:
	///Конструктор
	VPkCorruptDetector(const T& in_extractor, const std::wstring& in_type, double in_percent) : extractor(in_extractor), type(in_type), 
		percent(in_percent){};
		
	/// Деструктор
	~VPkCorruptDetector() 
	{
		if (!Empty()) 
			LOG_ERROR(L"VPkCorruptDetector: Необработанные данные...");
	};

	/// Помещает данные на вход
	void Put(const VRtParameter& in_record)
	{
		VPoint point=extractor(in_record);
		input.push_back(point);
		if (pk_changed.put(SITUATION.GetPk(in_record.absCoord).pk))
		{
			Flush();
		}
	}

	/// Возвращает true, если есть готовые отступления
	bool Ready() const
	{
		return output.size()>0;
	}

	/// Возвращает true, если отступлений нет
	bool Empty() const
	{
		return output.empty();
	}

	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const
	{
		return output.front();
	}

	/// Выталкивает первый элемент
	void Pop()
	{
		output.pop_front();
	}

	/// Слив данных
	void Flush()
	{
		if (!input.empty())
		{
			struct _pred
			{
				bool operator()(const VPoint& in_point) const {return in_point.GetCorrupt();};
			};
			size_t bad_count=std::count_if(input.begin(), input.end(), _pred());
			double bad_percent=100*bad_count/input.size();
			if (bad_percent>percent)
			{	
				output.push_back(VBasicOverstepPtr(new VPkCorruptOverstep<group>(input.front().GetCoord(), input.back().GetCoord(), bad_percent, type)));
			}
			input.clear();
		}
	}
};



/// Класс проверки контрольных участков
class VControlSectionDetector : public VOverstepDetector
{
private:
	
	std::deque<VBasicOverstepPtr> output;	
	accumulator_set< double, features<tag::mean, tag::variance > > width;
	accumulator_set< double, features<tag::mean, tag::variance > > level;
	std::deque<double> coord;
			
public:
	///Конструктор
	VControlSectionDetector();
	/// Деструктор
	~VControlSectionDetector();
	/// Помещает данные на вход
	virtual void Put(const VRtParameter& in_data);
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();	
};

/// Класс экспресс-оценки уширений
class VWidenExpressDetector : public VOverstepDetector
{
private:
	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> output;
	/// Поиск отступлений
	VFindExcess find_excess;
	/// Ограничение
	VRangeCondition condition;
public:
	/// Конструктор
	VWidenExpressDetector();
	/// Деструктор
	~VWidenExpressDetector();
	/// Ввод данных	
	void Put(const VRtParameter& in_record);
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;		
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();	
};

/// Класс поиска уширений
class VWidenOverstepDetector : public VOverstepDetector
{
private:
	
	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> oversteps;
	/// Поиск первой степени
	VFindExcess find_excess1;
	/// Поиск второй степени
	VFindExcess find_excess2;		
	/// Поиск третьей степени
	VFindExcess find_excess3;		
	/// Поиск четвертой степени
	VFindExcess find_excess4;	
	/// Усреднение износа
	VMean wear_mean;	
	/// Задержка ШК
	VDelay width_delay;		
			
public:
	/// Конструктор
	VWidenOverstepDetector();	
	/// Деструктор
	~VWidenOverstepDetector();
	/// Ввод данных
	void Put(const VRtParameter& in_record);	
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;		
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();	
	/// Слив данных
	void Flush();
};

/// Класс экспресс-оценки сужений
class VNarrowExpressDetector : public VOverstepDetector
{
private:
	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> output;
	/// Поиск отступлений
	VFindExcess find_excess;
	/// Ограничение
	VRangeCondition condition;
public:
	/// Конструктор
	VNarrowExpressDetector();
	/// Деструктор
	~VNarrowExpressDetector();
	/// Ввод данных	
	void Put(const VRtParameter& in_record);
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;		
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();	
};

/// Класс поиска сужений
class VNarrowOverstepDetector : public VOverstepDetector
{
private:
	
	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> oversteps;
	/// Поиск первой степени
	VFindExcess find_excess1;	
	/// Поиск второй степени
	VFindExcess find_excess2;	
	/// Поиск третьей степени
	VFindExcess find_excess3;	
	/// Поиск четвертой степени
	VFindExcess find_excess4;
	
public:
	/// Конструктор
	VNarrowOverstepDetector();
	/// Деструктор
	~VNarrowOverstepDetector();
	/// Ввод данных	
	virtual void Put(const VRtParameter& in_record);
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();	
	/// Слив данных
	void Flush();
};


/// Класс поиска отвода ШК
class VOtvodWidthOverstepDetector : public VOverstepDetector
{
private:

	// Данные

	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> oversteps;
	/// Вычисление отвода
	VFindOtvod otvod;
	/// Поиск отступлений
	VFindExcess find_excess;	

public:
	/// Конструктор
	VOtvodWidthOverstepDetector();
	/// Деструктор
	~VOtvodWidthOverstepDetector();
	/// Ввод данных	
	virtual void Put(const VRtParameter& in_record);
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();	
};


/// Класс поиска отвода возвышения
class VOtvodLevelOverstepDetector : public VOverstepDetector
{
private:

	// Данные

	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> oversteps;
	/// Вычисление среднего
	VMean level_mean;
	/// Вычисление отвода
	VFindOtvod otvod;
	/// Поиск отступлений
	VFindExcess find_excess;	

public:
	/// Конструктор
	VOtvodLevelOverstepDetector();
	/// Деструктор
	~VOtvodLevelOverstepDetector();
	/// Ввод данных	
	virtual void Put(const VRtParameter& in_record);
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();	
};


/// Класс экспресс-оценки износа
class VWearExpressDetector : public VOverstepDetector
{
private:
	/// Сторона
	side_t side;
	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> output;
	/// Поиск отступлений
	VFindExcess find_excess;
	/// Ограничение
	VRangeCondition condition;
public:
	/// Конструктор
	VWearExpressDetector(side_t in_side);
	/// Деструктор
	~VWearExpressDetector();
	/// Ввод данных	
	void Put(const VRtParameter& in_record);
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;		
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();	
};

/// Класс поиска износа
class VWearOverstepDetector : public VOverstepDetector
{
private:

	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> oversteps;
	/// Поиск отступлений
	VFindExcess find_excess0;
	/// Поиск отступлений
	VFindExcess find_excess2;
	/// Поиск отступлений
	VFindExcess find_excess3;	
	/// Поиск отступлений
	VFindExcess find_excess4;	
	
public:

	/// Конструктор
	VWearOverstepDetector();
	/// Деструктор
	~VWearOverstepDetector();
	/// Ввод данных
	virtual void Put(const VRtParameter& in_record);		
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();	
	/// Слив данных
	void Flush();
};

/// Класс поиска просадок
class VPitOverstepDetector : public VOverstepDetector
{
private:

	// Данные

	/// Сторона
	side_t side;
	/// Очередь отступлений на выходе
	std::deque<VBasicOverstepPtr> output;
	/// Экстремумы
	VFindExtremum extremum;	
	/// Пары экстремумов
	VFindWave wave;	
	/// Оценка просадок
	VCheckPit pit;
	/// Выбор полуволн
	VWaveSelector selector;
				
public:

	/// Конструктор
	VPitOverstepDetector(side_t in_side);
	/// Деструктор
	~VPitOverstepDetector();
	/// Воод данных
	void Put(const VRtParameter& in_record);	
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;		
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();	
	/// Сбрасывает остаток данных
	void Flush();
};

/// Класс вычисляющий вариацию уровня (отклонение от нормы)
class VLevelVariate : public VDataProcessor<VPoint, VPoint>
{
private:	
	/// Вычисление разности уровня и нулевой линии
	VPointToPointProcessorPtr level_variate;
	/// Изменение признака использования паспорта
	change_calculator<bool> bad_passport_changed;		
			
public:
	/// Конструктор
	VLevelVariate();
	/// Деструктор
	~VLevelVariate();
	/// Воод данных	
	void Put(const VPoint& in_point);
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;		
	/// Возвращает первый элемент
	const VPoint& Front() const;
	/// Выталкивает первый элемент
	void Pop();
};


/// Класс поиска отступлений по уровню
class VLevelOverstepDetector : public VOverstepDetector
{
private:
	// Данные

	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> oversteps;
	/// Поиск первой степени
	VFindExcess find_excess1;
	/// Поиск второй степени
	VFindExcess find_excess2;
	/// Поиск третьей степени
	VFindExcess find_excess3;
	/// Поиск четвертой степени
	VFindExcess find_excess4;
	
public:
	/// Конструктор
	VLevelOverstepDetector();
	/// Деструктор
	~VLevelOverstepDetector();
	/// Воод данных	
	virtual void Put(const VRtParameter& in_record);
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();	
	/// Слив данных
	void Flush();
};

/// Класс поиска отступлений по уровню
class VShortDistortionOverstepDetector : public VOverstepDetector
{
private:
	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> oversteps;	
	/// Вычисление отвода
	VFindOtvod otvod;
	/// Поиск отступлений
	VFindExcess find_excess;
	
public:
	/// Конструктор
	VShortDistortionOverstepDetector();
	/// Деструктор
	~VShortDistortionOverstepDetector();
	/// Воод данных	
	virtual void Put(const VRtParameter& in_record);
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();	
};

/// Класс поиска перекосов
class VDistortionOverstepDetector : public VOverstepDetector
{
private:
	// Данные

	/// Очередь на выходе
	std::deque<VBasicOverstepPtr> output;
	/// Вычисленние вариации уровня
	VLevelVariate level_variate;
	/// Поиск экстремумов
	VFindExtremum extremum;
	/// Пары экстремумов
	VFindWave wave;
	/// Фильтр полуволн
	VWaveFilter wave_filter;
	/// Объединение полуволн
	VWaveUnion wave_union;
	/// Оценка перекосов
	VCheckDistortion distortion;
	/// Выбор перекосов
	VWaveSelector selector;

public:
	/// Конструктор
	VDistortionOverstepDetector();
	/// Деструктор
	~VDistortionOverstepDetector();
	/// Воод данных
	void Put(const VRtParameter& in_record);		
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;		
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();	
	/// Сбрасывает остаток данных
	void Flush();
};

/// Класс вычисляющий вариацию рихтовки (отклонение от нормы)
class VRichVariate : public VDataProcessor<VPoint, VPoint>
{
private:	
	/// Сторона
	side_t side;
	/// Вычисление разности рихтовки и нулевой линии
	VPointToPointProcessorPtr rich_variate;
	/// Изменение признака использования паспорта
	change_calculator<bool> bad_passport_changed;	
public:
	/// Конструктор
	VRichVariate(side_t in_side);
	/// Деструктор
	~VRichVariate();
	/// Воод данных	
	void Put(const VPoint& in_point);
	/// Возвращает true, если есть данные
	bool Ready() const;	
	/// Возвращает true, если данных нет
	bool Empty() const;		
	/// Возвращает первый элемент
	const VPoint& Front() const;
	/// Выталкивает первый элемент
	void Pop();
};


/// Класс поиска рихтовки
class VRichOverstepDetector : public VOverstepDetector
{
private:
	// Данные

	/// Сторона
	side_t side;
	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> output;	
	/// Вычисление разности рихтовки и нулевой линии	
	VRichVariate rich_variate;
	/// Поиск экстремумов
	VFindExtremum extremum;
	/// Объединение экстремумов в пары
	VFindWave wave;	
	/// Оценка рихтовки
	VCheckRich rich;
	/// Выбор полуволн
	VWaveSelector selector;	

public:
	/// Конструктор
	VRichOverstepDetector(side_t in_side);
	/// Деструктор
	~VRichOverstepDetector();
	/// Воод данных
	void Put(const VRtParameter& in_record);
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;
	/// Возвращает true, если отступлений нет
	bool Empty() const;
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();
	/// Сбрасывает остаток данных
	void Flush();
};

/// Класс поиска отступлений по среднему непогашенному ускорению на круговых кривых
class VAnpAverageOverstepDetector : public VOverstepDetector
{
private:

	/// Расчет среднего уровня
	VMean level_mean;
	/// Расчет средней кривизны
	VMean curv_mean;
	/// Расчет параметров кривой
	VCurveCalc calc;
	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> output;
	
public:
	/// Конструктор
	VAnpAverageOverstepDetector();	
	/// Деструктор
	~VAnpAverageOverstepDetector();
	/// Ввод данных
	void Put(const VRtParameter& in_record);	
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();
	/// Слив данных
	void Flush();
};

/// Класс поиска отступлений по непогашенному ускорению в точке
class VAnpHorizontalOverstepDetector : public VOverstepDetector
{
private:

	/// Расчет среднего уровня
	VMean level_mean;
	/// Расчет средней кривизны
	VMean curv_mean;
	/// Расчет параметров кривой
	VCurveCalc calc;
	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> output;
	
public:
	/// Конструктор
	VAnpHorizontalOverstepDetector();	
	/// Деструктор
	~VAnpHorizontalOverstepDetector();
	/// Ввод данных
	void Put(const VRtParameter& in_record);	
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();
	/// Слив данных
	void Flush();
};

/// Класс поиска отступлений по скорости изменения непогашенного ускорения
class VPsiOverstepDetector : public VOverstepDetector
{
private:
	/// Расчет среднего уровня
	VMean level_mean;
	/// Расчет средней кривизны
	VMean curv_mean;
	/// Расчет параметров кривой
	VCurveCalc calc;
	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> output;
public:
	/// Конструктор
	VPsiOverstepDetector();
	/// Деструктор
	~VPsiOverstepDetector();
	/// Ввод данных
	void Put(const VRtParameter& in_record);	
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;
	/// Возвращает true, если отступлений нет
	bool Empty() const;
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();
	/// Слив данных
	void Flush();
};

/// Класс поиска отступлений на переходных
class VPsiTransitionOverstepDetector : public VOverstepDetector
{
private:
	/// Расчет среднего уровня
	VMean level_mean;
	/// Расчет средней кривизны
	VMean curv_mean;
	/// Расчет параметров кривой
	VCurveCalc calc;
	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> output;
public:
	/// Конструктор
	VPsiTransitionOverstepDetector();
	/// Деструктор
	~VPsiTransitionOverstepDetector();
	/// Ввод данных
	void Put(const VRtParameter& in_record);	
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;
	/// Возвращает true, если отступлений нет
	bool Empty() const;
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();
	/// Слив данных
	void Flush();
};

/// Класс экспресс-оценки зазора
class VZazorExpressDetector : public VOverstepDetector
{
private:
	/// Сторона
	side_t side;
	/// Очередь отступлений
	std::deque<VBasicOverstepPtr> output;	
public:
	/// Конструктор
	VZazorExpressDetector(side_t in_side);
	/// Деструктор
	~VZazorExpressDetector();
	/// Ввод данных	
	void Put(const VRtParameter& in_record);
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;		
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();	
};

/// Класс поиска сверхнормативных зазоров
class VZazorOverstepDetector : public VOverstepDetector
{
private:
	/// Очередь данных на выходе
	std::deque<VBasicOverstepPtr> output;
	/// Сторона
	side_t side;
public:
	/// Конструктор
	VZazorOverstepDetector(side_t in_side);
	/// Деструктор
	~VZazorOverstepDetector();
	/// Ввод данных	
	virtual void Put(const VRtParameter& in_record);
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();
};

/// Класс поиска двух и более нулевых зазоров подряд
class VZeroZazorOverstepDetector : public VOverstepDetector
{
private:
	/// Очередь данных на входе
	std::deque<VPoint> zero_zazor;
	/// Очередь данных на выходе
	std::deque<VBasicOverstepPtr> output;
	/// Сторона
	side_t side;
public:
	/// Конструктор
	VZeroZazorOverstepDetector(side_t in_side);
	/// Деструктор
	~VZeroZazorOverstepDetector();
	/// Ввод данных	
	virtual void Put(const VRtParameter& in_record);
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();
};

/// Класс собирающий отступления в один список
class VOverstepAggregator : public VDataProcessor<VBasicOverstepPtr, VBasicOverstepPtr>
{
private:
	/// Отступления
	std::deque<VBasicOverstepPtr> oversteps;		
public:	

	/// Деструктор
	~VOverstepAggregator();
	/// Помещает очередное отступление
	void Put(const VBasicOverstepPtr& in_overstep);		
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();
};

/// Класс корректировки закрытий скорости
class VOverstepRestrictionHandler
{
public:
	//Оператор ()
	void operator()(const VBasicOverstepPtr& in_overstep)
	{
		in_overstep->CorrectClosure();
	}
};

/// Шаблон класса - обработчика отступления
template<typename OverstepHandlerType> class VOverstepProcessor : public VDataProcessor<VBasicOverstepPtr, VBasicOverstepPtr>
{
private:
	/// Отступления
	std::deque<VBasicOverstepPtr> oversteps;
	/// Обработчик
	OverstepHandlerType handler;
public:	
	/// Конструктор
	VOverstepProcessor() {};

	/// Деструктор
	~VOverstepProcessor() 
	{
		if (!Empty()) 
			LOG_ERROR(L"VOverstepProcessor: Необработанные данные...");
	};

	/// Помещает очередное отступление
	void Put(const VBasicOverstepPtr& in_overstep)
	{						
		handler(in_overstep);
		oversteps.push_back(in_overstep);
	};		
	/// Возвращает true, если есть готовые отступления
	bool Ready() const
	{
		return oversteps.size()>0;
	};	
	/// Возвращает true, если отступлений нет
	bool Empty() const
	{
		return oversteps.empty();
	}
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const
	{
		return oversteps.front();
	}
	/// Выталкивает первый элемент
	void Pop()
	{		
		oversteps.pop_front();
	};	
};
typedef boost::shared_ptr<VDataProcessor<VBasicOverstepPtr, VBasicOverstepPtr> > VOverstepProcessorPtr;

/// Класс упорядочивающий отступления в порядке возрастания или убывания координаты
class VOverstepArranger : public VDataProcessor<VBasicOverstepPtr, VBasicOverstepPtr>, public VDataIn<double>
{
protected:
	/// Текущее положение
	double position;
public:		
	/// Помещает значение текущей позиции
	void Put(const double& in_position)
	{
		position=in_position;
	};
};

/// Класс упорядочивающий отступления в порядке возрастания или убывания координаты
template<class T> class VOverstepArrangerImpl : public VOverstepArranger
{
private:
	/// Отступления
	std::priority_queue<VBasicOverstepPtr, std::vector<VBasicOverstepPtr>, T> oversteps;	
public:	
	
	/// Деструктор
	~VOverstepArrangerImpl() 
	{
		if (!Empty()) 
			LOG_ERROR(L"VOverstepArrangerImpl: Необработанные данные...");
	};

	/// Помещает очередное отступление
	void Put(const VBasicOverstepPtr& in_overstep)
	{			
		oversteps.push(in_overstep);		
	};		
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const
	{
		return (!Empty()) && (fabs(oversteps.top()->GetCoord()-position)>DECISION_DISTANCE);
	};	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const
	{
		return oversteps.empty();
	}
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const
	{
		return oversteps.top();
	}
	/// Выталкивает первый элемент
	virtual void Pop()
	{		
		oversteps.pop();
	};	
};

typedef VOverstepArrangerImpl< std::less<VBasicOverstepPtr> > VOverstepArrangerDec;
typedef VOverstepArrangerImpl< std::greater<VBasicOverstepPtr> > VOverstepArrangerInc;
typedef boost::shared_ptr<VOverstepArranger> VOverstepArrangerPtr;


/// Поправка с учетом моста (пункт 4.9.1 новой инструкции)
static void BridgeCorrection(pro::interval<double> in_range, DEGREE in_degree)
{
	switch (in_degree)
	{
		default:
			break;
		case DEGREE3:
		{
			in_range.right_bound().value=floor(in_range.right_bound().value*0.9);
			break;
		}
		case DEGREE4:
		{
			in_range.left_bound().value=floor(in_range.left_bound().value*0.9);
			in_range.right_bound().value=floor(in_range.right_bound().value*0.9);
			break;
		}
	}
};

/// Экспресс-расшифровка (вариант 2)
class VRTExpressDetector : public VOverstepDetector
{
private:					
	
	///Расшифровщики
	VDataParallel<VRtParameter, VBasicOverstepPtr> overstep_detectors;	
	/// Сборщик
	VOverstepAggregator aggregator;
public:
	/// Конструктор
	VRTExpressDetector();	
	/// Деструктор
	~VRTExpressDetector();
	/// Выдать остаток
	void Flush();
	/// Помещает очередное отступление
	void Put(const VRtParameter& in_record);	
	/// Возвращает true, если есть готовые отступления
	bool Ready() const;	
	/// Возвращает true, если отступлений нет
	bool Empty() const;
	/// Возвращает первый элемент
	const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	void Pop();
};

/// Расшифровка ГРК в реальном времени
class VGeometryOverstepDetector : public VOverstepDetector
{
private:
	
	///Расшифровщики сбоев
	VDataParallel<VRtParameter, VBasicOverstepPtr> corrupt_detectors;
	///Расшифровщики отступлений
	VDataParallel<VRtParameter, VBasicOverstepPtr> overstep_detectors;
	///Расшифровщики сочетаний
	VDataParallel<VBasicOverstepPtr, VBasicOverstepPtr> group_detectors;
	
	/// Приращение на первом шаге
	diff_calculator<double> coord_diff;
	/// Отслеживание изменений километра
	change_calculator<int> km_change;
	/// Сортировщик
	VOverstepArrangerPtr arranger;
public:
	/// Конструктор
	VGeometryOverstepDetector();	
	/// Деструктор
	~VGeometryOverstepDetector();
	/// Выдать остаток
	void Flush();
	/// Помещает очередное отступление
	virtual void Put(const VRtParameter& in_record);	
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();
};
typedef boost::shared_ptr<VGeometryOverstepDetector> VGeometryOverstepDetectorPtr;

/// Расшифровка доп. параметров в реальном времени
class VExtraOverstepDetector : public VOverstepDetector
{
private:	
	
	///Расшифровщики сбоев
	VDataParallel<VRtParameter, VBasicOverstepPtr> corrupt_detectors;
	///Расшифровщики
	VDataParallel<VRtParameter, VBasicOverstepPtr> overstep_detectors;	
	/// Сортировщик
	VOverstepArrangerPtr arranger;	
	/// Приращение на первом шаге
	diff_calculator<double> coord_diff;
	/// Отслеживание изменений километра
	change_calculator<int> km_change;
	
public:
	/// Конструктор
	VExtraOverstepDetector();	
	/// Деструктор
	~VExtraOverstepDetector();
	/// Выдать остаток
	void Flush();
	/// Помещает очередное отступление
	virtual void Put(const VRtParameter& in_record);	
	/// Возвращает true, если есть готовые отступления
	virtual bool Ready() const;	
	/// Возвращает true, если отступлений нет
	virtual bool Empty() const;
	/// Возвращает первый элемент
	virtual const VBasicOverstepPtr& Front() const;
	/// Выталкивает первый элемент
	virtual void Pop();
};
typedef boost::shared_ptr<VExtraOverstepDetector> VExtraOverstepDetectorPtr;

struct VOverstep3;

/// Вычисление балловой оценки км
class VPenaltyCalc
{
	// штрафные баллы
	long penalty;

	// штрафные баллы за кривые
	long crv_penalty;

	// коэффициент длины
	double len_coeff;

	// для оценки непогашенных - оценённые кривые
	std::vector<__int64> crv_id_anp, crv_id_psi, crv_id_oh;
	
	// типы отступлений для оценки по ЦП-515
	const std::vector<std::wstring> cp515_restrictions, cp515_degree_2, cp515_degree_3;

	// учет отступления
	void Apply(const VAprinfo & apr, bool is_excluded, 
		double coord, __int64 km, const std::wstring & type, long penalty);
		
	// посчитан классический балл
	bool classic_penalty;

public:

	// конструктор
	VPenaltyCalc();
	
	// копирующий конструктор
	VPenaltyCalc(const VPenaltyCalc & src);
	
	// оператор присваивания
	VPenaltyCalc & operator = (const VPenaltyCalc & src);

	// учет отступления
	void Apply(const VAprinfo & apr, VBasicOverstepPtr ovr);
	void Apply(const VAprinfo & apr, const VOverstep3 & ovr);

	// рассчитать и вернуть штраф согласно "старому" ЦП-515
	void CalcOldStylePenalty(long minSpd1, size_t restrictionCount, 
		size_t deg2Count, size_t deg3Count);

	// вернуть штраф
	long GetPenalty() const 
	{ 
		return this->penalty * this->len_coeff; 
	}

	// вернуть штраф за кривые
	long GetCrvPenalty() const
	{
		return this->crv_penalty * this->len_coeff;
	}
	
	// посчитан классический балл
	bool GetClassicPenalty() const
	{
	    return this->classic_penalty;
	}
	
	// Список типов ограничений скорости для ЦП-515
	const std::vector<std::wstring> & GetCP515RestrictionList() const
	{
	    return this->cp515_restrictions;
	}
	
	// Отступление попадает в список ограничений по ЦП-515
	bool IsCP515Restriction(const std::wstring & type) const
	{
	    return std::find(this->cp515_restrictions.begin(), this->cp515_restrictions.end(), type) != this->cp515_restrictions.end();
	}
	
	// Список типов отступлений 2 степени для ЦП-515
	const std::vector<std::wstring> & GetCP515Degree2List() const
	{
	    return this->cp515_degree_2;
	}
	
	// Отступление попадает в список 2 степеней по ЦП-515
	bool IsCP515Degree2(const std::wstring & type) const
	{
	    return std::find(this->cp515_degree_2.begin(), this->cp515_degree_2.end(), type) != this->cp515_degree_2.end();
	}
	
	// Список типов отступлений 3 степени для ЦП-515
	const std::vector<std::wstring> & GetCP515Degree3List() const
	{
	    return this->cp515_degree_3;
	}
	
	// Отступление попадает в список 3 степеней по ЦП-515
	bool IsCP515Degree3(const std::wstring & type) const
	{
	    return std::find(this->cp515_degree_3.begin(), this->cp515_degree_3.end(), type) != this->cp515_degree_3.end();
	}
	
};

/// Данные одного километра
class VRTKmData
{
private:

	/// Предикат для поиска отступлений
	struct find_degree_group
	{
		DEGREE degree;
		OVERSTEP_GROUP group;
		find_degree_group(DEGREE in_degree, OVERSTEP_GROUP in_group) : degree(in_degree), group(in_group) {};
		bool operator()(const VBasicOverstepPtr& in_data) const {return in_data->GetDegree()==degree && in_data->GetGroup()==group;};
	};
	struct find_group
	{
		OVERSTEP_GROUP group;
		find_group(OVERSTEP_GROUP in_group) : group(in_group) {};
		bool operator()(const VBasicOverstepPtr& in_data) const {return in_data->GetGroup()==group;};
	};
	struct find_restriction_group
	{
		OVERSTEP_GROUP group;
		find_restriction_group(OVERSTEP_GROUP in_group) : group(in_group) {};
		bool operator()(const VBasicOverstepPtr& in_data) const {return in_data->GetGroup()==group && in_data->HasRestriction();};
	};
	struct find_restriction
	{			
		bool operator()(const VBasicOverstepPtr& in_data) const {return in_data->HasRestriction();};
	};
	
	/// Априорные данные
	VAprinfoKmRecord aprinfo_km;	
	/// Данные
	std::deque<VRtParameter> data;		
	/// Отступления
	std::deque<VBasicOverstepPtr> oversteps;
	/// Скорости
	VSpeeds speeds;
	VSpeeds min_speeds;	
	/// Оценка километров
	VPenaltyCalc penalty;

public:	
	/// Конструктор
	VRTKmData(int in_km);

	/// Возвращает количество отступлений по группе
	size_t GetOverstepCount(OVERSTEP_GROUP in_group) const;
	/// Возвращает количество отступлений по степени и группе
	size_t GetOverstepCount(OVERSTEP_GROUP in_group, DEGREE in_degree) const;
	/// Возвращает количество элементарных отступлений по группе
	size_t GetOverstepAmount(OVERSTEP_GROUP in_group) const;
	/// Возвращает количество элементарных отступлений по степени и группе
	size_t GetOverstepAmount(OVERSTEP_GROUP in_group, DEGREE in_degree) const;
	/// Возвращает число ограничений
	size_t GetRestrictionCount(OVERSTEP_GROUP in_group) const;	
	/// Возвращает количество ограничений
	size_t GetRestrictionCount() const;			
	/// Возвращает true если скорость ограничена
	bool HasRestriction() const;
	/// Возвращает ограничения скорости
	VSpeeds GetRestriction() const;
	/// Возвращает true если скорость ограничена
	bool HasRestriction(OVERSTEP_GROUP in_group) const;
	/// Возвращает ограничения скорости
	VSpeeds GetRestriction(OVERSTEP_GROUP in_group) const;
	/// Возвращает описание ограничений скорости
	std::wstring GetRestrictionString() const;
	/// Возвращает описание ограничений скорости
	std::wstring GetRestrictionString(OVERSTEP_GROUP in_group) const;	
	/// Возвращает строку описания участка
	std::wstring GetSectionString() const;
	///Возвращает сводку по отступлениям
	std::wstring GetSummaryString(OVERSTEP_GROUP in_group) const;	
	/// Возвращает перегон
	std::wstring GetPeregon() const;
	/// Возвращает номер километра
	int GetKm() const;
	/// Возвращает координату начала километра
	double GetKmStart() const;
	/// Возвращает координату конца километра
	double GetKmEnd() const;
	/// Возвращает длину километра
	double GetKmLength() const;
	/// Возвращает среднюю скорость на километре
	double GetAverageSpeed() const;
	/// Добавляет кадр данных
	void PutData(const VRtParameter& in_data);	
	/// Возвращает кадр данных
	const VRtParameter& GetData(size_t in_index) const; 
	/// Возвращает число кадров данных
	size_t GetDataCount() const;	
	/// Возвращает длину данных
	double GetDataLength() const;
	///Добавляет отступление
	void PutOverstep(const VBasicOverstepPtr& in_overstep);
	/// Возвращает отступление
	const VBasicOverstepPtr& GetOverstep(size_t in_index) const;
	/// Возвращает число отступлений
	size_t GetOverstepCount() const;
	/// Возвращает границы блока данных
	pro::interval<double> GetCoordRange() const;	
	/// Финальные расчеты
	void CalcPenalty();
	/// Ограничение на километр
	long GetPenalty() const;
	/// Ограничение на километр по кривым
	long GetCrvPenalty() const;
};
typedef boost::shared_ptr<VRTKmData> VRTKmDataPtr;

/// Список километров
class VRTKmProcessor : public VDataIn<VRtParameter>, public VDataIn<VBasicOverstepPtr>, public VDataOut<VRTKmDataPtr>
{
private:
	/// Километры
	std::deque<VRTKmDataPtr> km;
	/// Анаализатор изменения ПЧ
	change_calculator<long>	pch_changed;
public:	
	/// Конструктор
	VRTKmProcessor();	
	/// Помещает на вход кадр данных
	virtual void Put(const VRtParameter& in_data);
	/// Помещает на вход отступление
	virtual void Put(const VBasicOverstepPtr& in_overstep);
	/// Возвращает true, если данные готовы
	virtual bool Ready() const;
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VRTKmDataPtr& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();		
};
typedef boost::shared_ptr<VRTKmProcessor> VRTKmProcessorPtr;

/// Оценщик километров
class VKmRater : public VDataProcessor<VRTKmDataPtr, VRTKmDataPtr>
{
private:
	/// Километры
	std::deque<VRTKmDataPtr> output;
public:	
	/// Конструктор
	VKmRater();	
	/// Помещает на вход кадр данных
	virtual void Put(const VRTKmDataPtr& in_data);
	/// Возвращает true, если данные готовы
	virtual bool Ready() const;
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VRTKmDataPtr& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();		
};
typedef boost::shared_ptr<VKmRater> VKmRaterPtr;

// -------------------------------- Обнаружение уравнительных приборов по измерениям ШК --------------------------------------

/// База фильтра
#define WIDTH_CORRUPT_DETECTION_BASE 10.0
/// Длина выброса
#define WIDTH_CORRUPT_DETECTION_LENGTH 10.0
/// Величина выброса
#define WIDTH_CORRUPT_DETECTION_VALUE 15.0
/// Задержка
#define WIDTH_CORRUPT_DETECTION_DELAY 60.0


/// Класс обнаруживающий сбои ШК в зоне стрелок и уравнительных приборов
class VWidthCorruptDetector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// Данные на входе
	std::deque<VRtParameter> input;	
	/// Данные на выходе
	std::deque<VRtParameter> output;
	/// 
	VMeanVariate width_variate;
	/// Поиск выбросов
	VFindExcess width_excess_detector;
	
public:
	/// Конструктор
	VWidthCorruptDetector();		
	/// Деструктор
	~VWidthCorruptDetector();
	/// Помещает на вход кадр данных
	virtual void Put(const VRtParameter& in_data);
	/// Возвращает true, если данные готовы
	virtual bool Ready() const;
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();
	/// Обрабатывает остаток данных на входе
	virtual void Flush();
};

/// Класс обнаруживающий стрелки
class VSwitchDetector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// Данные на входе
	std::deque<VRtParameter> input;	
	/// Данные на выходе
	std::deque<VRtParameter> output;
	
	/// 
public:
	/// Конструктор
	VSwitchDetector();		
	/// Деструктор
	~VSwitchDetector();
	/// Помещает на вход кадр данных
	virtual void Put(const VRtParameter& in_data);
	/// Возвращает true, если данные готовы
	virtual bool Ready() const;
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();
	/// Обрабатывает остаток данных на входе
	virtual void Flush();	
};

/// Класс вычисляющий износ
class VWearCalculator : public VDataProcessor<VRtParameter, VRtParameter>
{
private:	
	/// Данные на выходе
	std::deque<VRtParameter> output;
public:
	/// Конструктор
	VWearCalculator();		
	/// Деструктор
	~VWearCalculator();
	/// Помещает на вход кадр данных
	virtual void Put(const VRtParameter& in_record);
	/// Возвращает true, если данные готовы
	virtual bool Ready() const;
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();	
};

/// Класс вычисляющий нулевые зазоры
class VZazorCalculator : public VDataProcessor<VRtParameter, VRtParameter>
{
private:	
	
	/// Данные на входе
	std::deque<VRtParameter> input;	
	/// Данные на выходе
	std::deque<VRtParameter> output;
	
	/// Вычисляет статистику отклонений
	accumulator_set< double, stats<tag::count, tag::mean, tag::median, tag::min, tag::max > > diff_stat_left;
	accumulator_set< double, stats<tag::count, tag::mean, tag::median, tag::min, tag::max > > diff_stat_right;
	
public:
	/// Конструктор
	VZazorCalculator();		
	/// Деструктор
	~VZazorCalculator();
	/// Помещает на вход кадр данных
	virtual void Put(const VRtParameter& in_data);
	/// Возвращает true, если данные готовы
	virtual bool Ready() const;
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();	
	/// Слив данных
	virtual void Flush();
};

/// Класс расстановки маркеров
class VMarkerDetector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// Данные на выходе
	std::deque<VRtParameter> output;
public:
	/// Конструктор
	VMarkerDetector();
	/// Деструктор
	~VMarkerDetector();
	/// Помещает данные на вход
	void Put(const VRtParameter& in_data);
	/// Возвращает true, если есть обработанные данные
	bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	bool Empty() const;		
	/// Возвращает первый элемент данных
	const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	void Pop();
};

/// Класс поиска кривых
class VCurveDetector : public VDataProcessor<VRtParameter, VRtParameter>
{	
private:
	/// Данные на входе
	std::deque<VRtParameter> input;
	/// Данные на выходе
	std::deque<VRtParameter> output;
	/// Сторона
	side_t side;
	/// Условие
	VCurveCondition condition;
			
public:
	/// Конструктор
	VCurveDetector(side_t in_side, double in_min_curv=1);
	/// Деструктор
	~VCurveDetector();
	/// Помещает данные на вход
	void Put(const VRtParameter& in_data);
	/// Возвращает true, если есть обработанные данные
	bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	bool Empty() const;		
	/// Возвращает первый элемент данных
	const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	void Pop();
	/// Слив данных
	void Flush();	
};

/// Обработчик съезда по умолчанию
class VDefaultSidewaySwitchHandler
{
public:
	void operator()(const std::wstring& in_message)
	{
		LOG_WARNING(in_message);
	};
};

/// Обнаружение съездов по стрелкам
template<typename HandlerType> class VSidewaySwitchDetector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// Данные на входе
	std::deque<VRtParameter> input;
	/// Данные на выходе
	std::deque<VRtParameter> output;
	/// Поиск экстремума
	VFindExtremum extremum_left;
	/// Поиск экстремума
	VFindExtremum extremum_right;
	/// Число стрелок во входном массиве
	int optical_switch_count;
public:
	/// Конструктор
	VSidewaySwitchDetector() : extremum_left(10), extremum_right(10), optical_switch_count(0) {};
	/// Деструктор
	~VSidewaySwitchDetector() {};
	/// Помещает данные на вход
	void Put(const VRtParameter& in_data)
	{
		input.push_back(in_data);
		if (input.back().expandedButtons.IsSwitch())
		{
			optical_switch_count++;	
		}
		if (fabs(input.back().absCoord-input.front().absCoord)>SIDEWAY_SWITCH_LENGTH().Value())
		{
			if (optical_switch_count>0)
			{
				double abs_coord=(input.back().absCoord+input.front().absCoord)/2;
				double curv_conv_left=0;
				double curv_conv_right=0;
				for (size_t i=0;i<input.size();++i)
				{		
					double x=input.at(i).curv.value/SIDEWAY_SWITCH_SCALE().Value();
					double h_left=sin(M_PI+2.0*M_PI*double(i)/double(input.size()));
					double h_right=sin(2.0*M_PI*double(i)/double(input.size()));
					curv_conv_left+=h_left*x;
					curv_conv_right+=h_right*x;
				}
				extremum_left.Put(VPoint(abs_coord, curv_conv_left));
				extremum_right.Put(VPoint(abs_coord, curv_conv_right));
				
				while (extremum_left.Ready())
				{
					if (extremum_left.Front().GetValue()>SIDEWAY_SWITCH_RANGE().Value())
					{
						HandlerType()(boost::str(boost::wformat(L"Съезд на боковой путь (налево): %f") % extremum_left.Front().GetCoord()));
					}
					extremum_left.Pop();
				}
				while (extremum_right.Ready())
				{
					if (extremum_right.Front().GetValue()>SIDEWAY_SWITCH_RANGE().Value())
					{
						HandlerType()(boost::str(boost::wformat(L"Съезд на боковой путь (направо): %f") % extremum_right.Front().GetCoord()));
					}
					extremum_right.Pop();
				}
			}
			if (input.front().expandedButtons.IsSwitch())
			{
				optical_switch_count--;	
			}
			transfer<VRtParameter>(input,output);
		}
	};
	/// Возвращает true, если есть обработанные данные
	bool Ready() const
	{
		return output.size()>0;
	}
	/// Возвращает true, если обработанных данных нет
	bool Empty() const
	{
		return output.empty();		
	}
	/// Возвращает первый элемент данных
	const VRtParameter& Front() const
	{
		return output.front();
	}
	/// Выталкивает первый элемент данных
	void VSidewaySwitchDetector::Pop()
	{
		output.pop_front();
	}
	/// Слив данных
	void Flush()
	{
		while (!input.empty())
			transfer<VRtParameter>(input,output);
	}

};

/// Класс вычисления статистики для оценки паспорта
class VPassportStatistics
{
private:
	accumulator_set< double, stats<tag::covariance<double, tag::covariate1> > > covariate;
	accumulator_set< double, features<tag::variance, tag::max, tag::min, tag::mean, tag::median > > parameter;
	accumulator_set< double, features<tag::variance, tag::max, tag::min, tag::mean, tag::median > > passport;
public:
	/// Конструктор
	VPassportStatistics();
	/// Добавляетс данные
	void Put(double in_parameter, double in_passport);
	/// Возвращает коэффициент корреляции
	double GetCorrelation() const;
	/// Возвращает размах паспорта
	double GetPassportRange() const;	
};

/// Класс определения участков расхождения с паспортом.
class VBadPassportDetector : public VDataProcessor<VRtParameter, VRtParameter>
{	
private:
	/// Данные на входе
	std::deque<VRtParameter> input;
	/// Данные на выходе
	std::deque<VRtParameter> output;		
public:
	/// Конструктор
	VBadPassportDetector();
	/// Деструктор
	~VBadPassportDetector();
	/// Помещает данные на вход
	void Put(const VRtParameter& in_data);
	/// Возвращает true, если есть обработанные данные
	bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	bool Empty() const;		
	/// Возвращает первый элемент данных
	const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	void Pop();
	/// Слив данных
	void Flush();
};

/// Класс вычисления корреляции кривизны и паспорта (для проверки качества привязки координат)
class VCorrectionVerificator : public VDataProcessor<VRtParameter, VRtParameter>
{	
private:
	/// Данные на входе
	std::deque<VRtParameter> input;
	/// Данные на выходе
	std::deque<VRtParameter> output;
	/// Проверка монтонности координаты
	std::deque<double> monotone;
	/// Проверка максимального и минимального шага
	accumulator_set< double, stats<tag::max, tag::min, tag::mean, tag::median> > diff;
	/// Проверка корреляции с паспортом
	VPassportStatistics plan;		
public:
	/// Конструктор
	VCorrectionVerificator();
	/// Деструктор
	~VCorrectionVerificator();
	/// Помещает данные на вход
	void Put(const VRtParameter& in_data);
	/// Возвращает true, если есть обработанные данные
	bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	bool Empty() const;		
	/// Возвращает первый элемент данных
	const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	void Pop();
};

/// Класс определения длин нестандартных километров
class VNstKmDetector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// Данные на входе
	std::deque<VRtParameter> input;
	/// Данные на выходе
	std::deque<VRtParameter> output;
public:
	/// Конструктор
	VNstKmDetector();
	/// Деструктор
	~VNstKmDetector();
	/// Помещает данные на вход
	void Put(const VRtParameter& in_data);
	/// Возвращает true, если есть обработанные данные
	bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	bool Empty() const;		
	/// Возвращает первый элемент данных
	const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	void Pop();
	/// Слив данных
	void Flush();
};

/// Класс проверки паспортного износа на кривой
class VWearVerificator : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// Данные на входе
	std::deque<VRtParameter> input;
	/// Данные на выходе
	std::deque<VRtParameter> output;
public:
	/// Конструктор
	VWearVerificator();
	/// Деструктор
	~VWearVerificator();
	/// Помещает данные на вход
	void Put(const VRtParameter& in_data);
	/// Возвращает true, если есть обработанные данные
	bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	bool Empty() const;		
	/// Возвращает первый элемент данных
	const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	void Pop();	
};


/// Класс, инвертирующий стрелки, в зависимости от разворота вагона
class VSwitchesInvertor : public VDataProcessor<VRtParameter, VRtParameter>
{
private:	
	/// Данные на выходе
	std::deque<VRtParameter> output;		
public:
	/// Конструктор
	VSwitchesInvertor();		
	/// Деструктор
	~VSwitchesInvertor();
	/// Помещает на вход кадр данных
	virtual void Put(const VRtParameter& in_data);
	/// Возвращает true, если данные готовы
	virtual bool Ready() const;
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();	
};

/// Класс корректирующий кривые
class VCurveCorrector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// Данные на входе
	std::deque<VRtParameter> input;
	/// Уровень
	std::deque<point_t> level_points;
	/// Рихтовка левая
	std::deque<point_t> rich_left_points;
	/// Рихтовка правая
	std::deque<point_t> rich_right_points;
	/// ШК
	std::deque<point_t> width_points;
	/// Данные на выходе
	std::deque<VRtParameter> output;

	change_calculator<ZLINE_TYPE> level_changed;
	change_calculator<ZLINE_TYPE> plan_changed;
	
public:
	/// Конструктор
	VCurveCorrector();		
	/// Деструктор
	~VCurveCorrector();
	/// Помещает на вход кадр данных
	virtual void Put(const VRtParameter& in_data);
	/// Возвращает true, если данные готовы
	virtual bool Ready() const;
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();
	/// Обрабатывает остаток данных на входе
	virtual void Flush();
};

/// Класс коррекции ошибки старта
class VStartCorrector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// Данные на входе
	std::deque<VRtParameter> input;
	/// Данные на выходе
	std::deque<VRtParameter> output;
	/// Ошибка старта
	double shift;
	/// Признак того что коррекция произошла или невозможна
	bool skip;	

public:
	/// Конструктор
	VStartCorrector();
	/// Деструктор
	~VStartCorrector();
	/// Слив данных
	void Flush(double in_shift=0);
	/// Помещает на вход кадр данных
	virtual void Put(const VRtParameter& in_data);
	/// Возвращает true, если данные готовы
	virtual bool Ready() const;
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();	
};

/// Счетчик кадров
class VDataCounter : public VDataProcessor<VRtParameter, VRtParameter> 
{
private:
	/// Счетчик
	int count;
	/// Данные на выходе
	std::deque<VRtParameter> output;
public:
	/// Конструктор
	VDataCounter() : count(0) {};
	/// Деструктор
	~VDataCounter()
	{
		LOG_DEBUG(boost::str(boost::wformat(L"%d кадров") % count));
	};	
	/// Помещает на вход кадр данных
	virtual void Put(const VRtParameter& in_data) 
	{
		count++;
		output.push_back(in_data);
	};
	/// Возвращает true, если данные готовы
	virtual bool Ready() const {return output.size()>0;};
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const {return output.empty();};		
	/// Возвращает первый элемент данных
	virtual const VRtParameter& Front() const {return output.front();};
	/// Выталкивает первый элемент данных
	virtual void Pop() {output.pop_front();};
};

/// Функция коррекции координат
class VCoordCorrectionFunction
{
private:
	/// Список прямых
	std::deque<line_t> lines;
	
public:
	/// Добавляет линию
	void Update(const line_t& in_line);
	/// Добавляет линию
	void Update(double in_coordinate, const line_t& in_line);
	/// Добавляет линию
	void Update(point_t in_point, const line_t& in_line);
	/// Возвращает скорректированную координату
	double Correct(double in_coordinate);
	/// Возвращает true, если коррекция возможна
	bool Valid() const;
	/// Сбрасывает функцию
	void Reset();
	/// Возвращает значение фукнции в точке
	double Value(double in_coordinate)  const;
	/// Возвращает величину ошибки
	double GetError() const;
	/// Возвращает коэффициент поправки диаметра колеса
	double GetWheelDiameterCorrection() const;
};

/// Корректор координат
class VCoordCorrector : public VDataProcessor<VRtParameter, VRtParameter> 
{
private:

	//Данные

	/// Данные на входе
	std::deque<VRtParameter> input;
	/// Данные на выходе
	std::deque<VRtParameter> output;
	/// Медианная линия
	mml_calculator mml;	
	/// Признак неудачи корректировки
	bool fail;
	
public:
	/// Конструктор
	VCoordCorrector();
	/// Деструктор
	~VCoordCorrector();
	/// Слив данных
	void Flush();
	/// Помещает на вход кадр данных
	virtual void Put(const VRtParameter& in_data);
	/// Возвращает true, если данные готовы
	virtual bool Ready() const;
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const;		
	/// Возвращает первый элемент данных
	virtual const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	virtual void Pop();
};

struct VRichCalcHelper
{
	double K;
	double R;
	double r;
	double X;
	double Y;
	double result;
	double Rx() const {return R*sin(K);};
	double Ry() const {return R*cos(K);};
	double rx() const {return r*sin(K);};
	double ry() const {return r*cos(K);};
	double dr() const {return R-r;}
	double dx() const {return (Rx()-rx())/1000.0;};
	double dy() const {return (Ry()-ry())/1000.0;};
};

struct VRichCalcData
{
	VRichCalcData(const VRtParameter& in_data) : data(in_data)
	{
		absCoord=0;
		
		rich_left.K=0;
		rich_left.X=0;
		rich_left.Y=0;
		rich_left.R=data.richLeft.value;
		rich_left.r=0;
		rich_left.result=0;
		
		rich_right.K=0;
		rich_right.X=0;
		rich_right.Y=0;
		rich_right.R=data.richRight.value;
		rich_right.r=0;
		rich_right.result=0;
	};
	double absCoord;
	VRichCalcHelper rich_left;
	VRichCalcHelper rich_right;
	VRtParameter data;
};

/// Класс вычисляющий уточненный план
class VRichCalculator : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// Данные на входе
	std::deque<VRichCalcData> input;	
	/// Данные на выходе
	std::deque<VRichCalcData> output;
	/// Переднее плечо хорды
	double Lf;
	/// Заднее плечо хорды
	double Lb;
	/// Переднее плечо хорды
	double lf;
	/// Заднее плечо хорды
	double lb;
	/// Точность
	double precision;
	/// Размер буфера
	double buffer_size;
	/// Мертвая зона
	double deadzone;	
	/// Признак инициальзации
	bool initialized;
	/// Максимальное число итераций
	size_t max_iteration_count;
	
	/// Инициализация
	void Init(const VRtParameter& in_data);
	/// Предварительные вычисления
	void InputPreprocess();
	/// Уточнение плана
	void CalculatePlan();
	/// Вычисление рихтовок
	void CalculateRich();
	
public:
	/// Конструктор
	VRichCalculator();
	/// Деструктор
	~VRichCalculator();
	/// Помещает на вход кадр данных
	void Put(const VRtParameter& in_data);
	/// Возвращает true, если данные готовы
	bool Ready() const;
	/// Возвращает true, если обработанных данных нет
	bool Empty() const;		
	/// Возвращает первый элемент данных
	const VRtParameter& Front() const;
	/// Выталкивает первый элемент данных
	void Pop();
	/// Слив данных
	void Flush();
};

/// Отступление в файле
struct VOverstep3
{
	double absCoord;			// абс.к-та
	double begin, end;			// границы отступления
	__int64 km;					// км
	double m;					// м
	double value;				// величина
	double length;				// длина
	__int64 degree;				// степень
	wchar_t description[256];	// описание 
	wchar_t comment[256];		// комментарий
	wchar_t type[32];			// тип
	__int64 spd1, spd2, spd3;	// скорости
	__int64 rst1, rst2, rst3;	// ограничение
	__int64 hasRestriction;		// наличие описания
	__int64 amount;				// кол-во отступлений
	__int64 penalty;			// баллы
	__int64 closeToUpper;		// близость к высшей степени

#pragma pack(push, 1)
	bool is_excluded;			// исключено
	__int16 cp515_revision;		// ревизия ЦП-515: 0=нет км.оценки, 1=старый ЦП, 2=новый ЦП
	__int8 reserve0[13];
#pragma pack(pop)

	wchar_t excl_reason[256];	// причина исключения

	__int64 hi_speeds;			// 1 = true, 0 = false
	__int64 spd_hi, spd_hi_bt;  // приказ
	__int64 rst_hi, rst_hi_bt;  // ограничение

	//	__int64 reserve[36];		// резерв (до 2048 байт)
	__int64 reserve[31];		// резерв (до 2048 байт)

	// конструктор
	VOverstep3();
	
	// копирующий конструктор - из BasicOverstep
	VOverstep3(VBasicOverstepPtr ovr);

	// импорт из себя в себя - для совместимости с VIndexList
	bool Import(const VOverstep3 & src)
	{
		*this = src;
		return true;
	}

	// экспорт из себя в себя - для совместимости с VIndexList
	VOverstep3 Export() const
	{
		return *this;
	}

	// Попадание в секцию
	bool MeetsSection(const VWaySection * section) const
	{
		VWayCoord wc(this->km, this->m);
		return section == NULL || (wc >= section->start && wc <= section->end);
	}

	// проверка типа
	bool IsType(const wchar_t * t) const
	{
		return wcscmp(this->type, t) == 0;
	}

	// Установить тип
	void SetType(const std::wstring & type) 
	{
		wcsncpy(this->type, type.c_str(), 31);
		this->type[31] = 0;
	}

	// Установить описание
	void SetDescription(const std::wstring & descr)
	{
		wcsncpy(this->description, descr.c_str(), 255);
		this->description[255] = 0;
	}

	// Установить комментарий
	void SetComment(const std::wstring & comment)
	{
		wcsncpy(this->comment, comment.c_str(), 255); 
		this->comment[255] = 0;
	}

	// Установить причину исключенности
	void SetExclusionReason(const std::wstring & excl_reason)
	{
		wcsncpy(this->excl_reason, excl_reason.c_str(), 255); 
		this->excl_reason[255] = 0;
	}

	// приказ
	VSpeeds GetSpeed() const
	{
		if (this->hi_speeds)	
			return VSpeeds(this->spd1, this->spd2, this->spd3, this->spd_hi, this->spd_hi_bt);
		else
			return VSpeeds(this->spd1, this->spd2, this->spd3);

	}

	// Максимальная скорость по приказу
	const speed_t & GetMaxSpeed() const
	{
		return this->GetSpeed().GetMaxSpeed();
	}

	// ограничение
	VSpeeds GetRestriction() const
	{
		if (this->hi_speeds)	
			return VSpeeds(this->rst1, this->rst2, this->rst3, this->rst_hi, this->rst_hi_bt);
		else
			return VSpeeds(this->rst1, this->rst2, this->rst3);
	}

	// Участок вписывается в границы проезда, по числу кадров
	bool Fits(size_t kadrCount) const { return true; }
}; 

bool operator < (const VOverstep3 & o1, const VOverstep3 & o2);

class VOvr3List : public VIndexList<VOverstep3>
{
public:

	VOvr3List() : VIndexList<VOverstep3>()
	{}

	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		bool result = this->DefaultLoad<FILETYPE, VOverstep3>(file, type, kadrCount);
		this->Sort();
		return result;
	}

	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VOverstep3>(file, type);
	}

	bool CopyFrom(const VOvr3List & src, const VWaySection * section);
	bool CopyFrom(const std::vector<VBasicOverstepPtr> & src);
	
	long GetCP515Revision() const
	{
	    for (size_t i = 0; i < this->GetCount(); ++i)
	    {
	        return this->GetItem(i).cp515_revision;
	    }
	    return 0;
	}
};

/// Текущая координата
DECLARE_VARIABLE_MT(double, CURRENT_COORDINATE, 0);

/// База усреднения рихтовки
DECLARE_OPTION(double, MEAN_RICH_BASE, 48.0, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// База усреднения уровня
DECLARE_OPTION(double, MEAN_LEVEL_BASE, 48.0, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// Метод оценки сбоев
DECLARE_OPTION(int, CORRUPT_DETECTION_POLICY, 1, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// Допустимый процент сбоев на пикете
DECLARE_OPTION(double, CORRUPT_PERCENT, 50, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// Длина съезда на боковой путь по стрелке
DECLARE_OPTION(double, SIDEWAY_SWITCH_LENGTH, 80, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// Масштабный коэффициент кривизны на съезде на боковой путь по стрелке
DECLARE_OPTION(double, SIDEWAY_SWITCH_SCALE, 180, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// Порог для определения съезда на боковой путь по стрелке
DECLARE_OPTION(double, SIDEWAY_SWITCH_RANGE, 100, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// Правила обработки участков съезда на боковой путь (0 - переключаться на скользящее среднее, 1 - исключать с пометкой "пасп."
DECLARE_OPTION(int, BAD_PASSPORT_HANDLING_POLICY, 1, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// Ошибка диаметра колеса м/км (коэффициент k в уравнении y=kx+b, умноженный на 1000)
DECLARE_OPTION_MT(double, COORD_CORRECTION_SLOPE, 0, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// Уравнение прямой пересчета координат
DECLARE_VARIABLE_MT(VCoordCorrectionFunction, COORD_CORRECTION_FUNCTION, VCoordCorrectionFunction());

#define OLD_INSTRUCTION DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515)
#define NEW_INSTRUCTION DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_EXPERIMENT)