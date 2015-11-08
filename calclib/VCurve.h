#pragma once

#include "c4flib.h"
#include "optlib.h"
#include "parameters.h"
#include "VAprinfo.h"
#include "VCalcOptions.h"
#include "oversteps.h"
#include <vector>
#include <algorithm>

// Типы кривых
enum CURVE_TYPE
{
	CURVE_MAGISTRAL, CURVE_STATION, CURVE_MODEL, CURVE_REPAIR, CURVE_SPEED
};


/// Максимально допустимое возвышение
static const double MAX_LEVEL_ALLOWED=145;
/// Минимальное возвышение отличное от нуля
static const double MIN_LEVEL_ALLOWED=15;
// Описание статуса модели
static const char * __status__[]= {
	"Не оценивалась",
	"Паспортизована",
	"Временно паспортизована",
	"Не соответствует установленной скорости",
	"Неизвестный статус модели"
};

// Комментарии к модели
static const char * __comment__[]= {
	"Избыточное возвышение",
	"Не обеспечиваются требования по Анп",
	"Недостаточное возвышение",
	"Несовпадение отводов",
	"Особые условия",
	"Сложная кривая",
	"Другое"
};

/// Пересчитывает индекс радиуса в индекс элемента (переходная/круговая)
static size_t radius_to_element(size_t in_radius) {return in_radius*2+1;};
/// Вычисляет  индекс метки начала радиуса (круговой)
static size_t radius_start_mark(size_t in_radius) {return in_radius*2+1;};
/// Вычисляет  индекс метки конца радиуса (круговой)
static size_t radius_end_mark(size_t in_radius) {return in_radius*2+2;};

// Условия Анп для модели

/// Для средневзвешенной (грузовой) скорости
class VModelAnpTruckCondition : public VOverstepCondition
{
public:
	bool TestValue(double in_value, double in_length=0) const {return fabs(in_value) > 0.3;};
};

/// Для пассажирской скорости
class VModelAnpPassCondition : public VOverstepCondition
{
public:
	bool TestValue(double in_value, double in_length=0) const {return fabs(in_value) > 0.7;};
};

/// Для скоростных поездов
class VModelAnpHighCondition : public VOverstepCondition
{
public:
	bool TestValue(double in_value, double in_length=0) const {return fabs(in_value) > 1.0;};
};

/// Пси для пассажирской скорости
class VModelPsiPassCondition : public VOverstepCondition
{
public:
	bool TestValue(double in_value, double in_length=0) const {return fabs(in_value) > 0.6;};
};

/// Пси для грузовой скорости
class VModelPsiTruckCondition : public VOverstepCondition
{
public:
	bool TestValue(double in_value, double in_length=0) const {return fabs(in_value) > 0.3;};
};

/// Пси для скоростных поездов
class VModelPsiHighCondition : public VOverstepCondition
{
public:
	bool TestValue(double in_value, double in_length=0) const {return fabs(in_value) > 0.9;};
};

// ---------------------------------------------------------------------------
// Элементы кривой
// ---------------------------------------------------------------------------

/// Метка на кривой
struct VCurveMark
{
	uint32_t index;	///< номер кадра
	double value;	///< значние параметра
	double latitude, longitude; ///< GPS

	/// Конструктор по умолчанию
	VCurveMark() : index(0), value(0), 
		latitude(0.0f), longitude(0.0f)
	{}

	/// Конструктор из указанных индекса и значения
	VCurveMark(uint32_t __index, double __val) : index(__index), value(__val),
		latitude(0.0f), longitude(0.0f)
	{}

	/// Выставить широту и долготу
	void SetGeoCoord(const VDoubleParameter & lat, const VDoubleParameter & lng)
	{
		this->latitude = lat.GetItem(this->index);
		this->longitude = lng.GetItem(this->index);
	}

	/// Оператор сравнения
	bool operator < (const VCurveMark & other)
	{
		return this->index < other.index;
	}
};

///Размер комментария
#define COMMENT_SIZE 260

// Статус модельной кривой
/** Не паспортизована*/
#define MODEL_STATUS_NOT_PASSPORT 0L
/** Паспортизована*/
#define MODEL_STATUS_PASSPORT 1L
/** Временно паспортизована*/
#define MODEL_STATUS_TEMP_PASSPORT 2L
/** Не соответсвует установленной скорости*/
#define MODEL_STATUS_SPEED_OVR 3L

/// Характеристики модели кривой
struct VCurveModelInfo
{
	long status; ///< Статус модели кривой
	char comment[COMMENT_SIZE]; ///< Комментарий модели
	speed_t Vhigh_bt, Vhigh, Vpass, Vcrg, Vavg, Vmin; ///< Модельные скорости

	// Конструктор
	VCurveModelInfo() : status(0), Vhigh_bt(NO_SPEED), Vhigh(NO_SPEED), Vpass(100), Vcrg(80), Vavg(60), Vmin(40)
	{
		ZeroMemory(comment, sizeof(comment));
	}

	// копирующий конструктор
	VCurveModelInfo(const VCurveModelInfo & src) : status(src.status),
		Vhigh_bt(src.Vhigh_bt), Vhigh(src.Vhigh), Vpass(src.Vpass), Vcrg(src.Vcrg), Vavg(src.Vavg), Vmin(src.Vmin)
	{
		memcpy(this->comment, src.comment, COMMENT_SIZE);
	}

	// Оператор присваивания
	VCurveModelInfo & operator = (const VCurveModelInfo & src)
	{
		this->status = src.status;
		this->Vhigh_bt = src.Vhigh_bt;
		this->Vhigh = src.Vhigh;
		this->Vpass = src.Vpass;
		this->Vcrg = src.Vcrg;
		this->Vavg = src.Vavg;
		this->Vmin = src.Vmin;
		memcpy(this->comment, src.comment, COMMENT_SIZE);
		return *this;
	}

};

///Непогашенное ускорение в точке
struct VAnp
{
	VAnp() : index(0), value(0) {};
	VAnp(size_t in_index, float in_value) : index(in_index), value(in_value) {};
	size_t index;
	double value;
	bool operator < (const VAnp & right)
	{
		return index < right.index;
	};
};

///Тип скорости
enum speed_type
{
	pass_prk=0,///Пасс. по приказу
	crg_prk=1,///Груз. по приказу
	prj_prk=2,///Прж. по приказу
	pass_plan=3,///Пасс. планируемая
	crg_plan=4,///Груз. планируемая
	prj_plan=5,///Прж. планируемая
	
	high_prk=6,///Скор.
	high_bt_prk=7///Скор. с накл. кузова
};


/// Участок скорости на кривой
struct VCurveSpdArea
{
	speed_t spd;		///< Значение скорости
	double begin, end;	///< Границы участка
};

/// Пикет на кривой
struct VCurvePicket : public VPicket
{
	/// Наличие ограничений на пикете
	bool isVkrPass, isVkrTruck, isVprPass, isVprTruck, isVizPass, isVizTruck;

	/// Конструктор
	VCurvePicket();

	/// Копирующий конструктор [1]
	VCurvePicket(const VPicket & pk);

	/// Копирующий конструктор [2]
	VCurvePicket(const VCurvePicket & pk);

	/// Наличие ограничения на пикете
	bool HasLimit() const;
};

/// Пикет с износами на кривой
struct VCurveWearPicket : public VPicket
{
	double maxWear;	///< Максимальный износ 
	double avgWear;	///< Средний износ 
	double maxWidth;	///< Максимальное ШК 
	double avgWidth;	///< Среднее ШК 
	speed_t spdPass;	///< Допустимая скорость по износу - пассажирская
	speed_t spdTruck;	///< Допустимая скорость по износу - грузовая
	speed_t prkPass;    ///< Скорость по приказу - пассажирская
	speed_t prkTruck;   ///< Скорость по приказу - грузовая
	bool ntd;			///< Ограничение по НТД
	double errorWidth; ///< Процент сбоев на ШК
	double errorWear; ///< Процент сбоев на износе

	/// Конструктор
	VCurveWearPicket();

	/// Копирующий конструктор [1]
	VCurveWearPicket(const VPicket & pk);

	/// Копирующий конструктор [2]
	VCurveWearPicket(const VCurveWearPicket & pk);
};

/// Звено на кривой
struct VCurveLink
{
	double absStart, absEnd; ///< границы - абс.к-та
	VWaySection section; ///< границы - путевая к-та
	double avgWear, maxWear, errorWear; ///< износ: максимум, среднее, % сбоев
	double avgWidth, maxWidth, errorWidth; ///< шк: максимум, среднее, % сбоев
	speed_t spdPass, spdTruck;	///< ограничение скорости на звене, пасс. и груз.
	bool ntd;			///< Ограничение по НТД

	/// Конструктор
	VCurveLink();
};

/// "Участок единой оценки" для ФПО
struct VCurveFPOArea
{
	double absStart, absEnd; ///< Границы - абс.к-та
	VWaySection section; ///< Границы - путевая к-та

	speed_t spdVprkPass; ///< Проектная скорость
	speed_t spdVkr, spdVpr, spdViz; ///< Оценочные скорости
	double maxAnp, maxAnpCoord; ///< Максимум непогашенного ускорения и его абс.к-та
	double maxPsi, maxPsiCoord; ///< Максимум пси ускорения и его абс.к-та
	double maxOh, maxOhCoord; ///< Максимум отвода возвышения ускорения и его абс.к-та
	double maxAnpRange, maxPsiRange; ///< Пороги Анп, Пси, ОВ
	/// Путевые координаты
	VWayCoord maxAnpWayCoord, maxPsiWayCoord, maxOhWayCoord;
};

/// Декларированные классы
class VDataSet;
class VAbsCoordData;
class VCurve;

/// Категория пути не установлена
const long WAYCAT_UNSET = 100;

/// Временные параметры расчета кривых
struct VCurveTempParams
{
	/// Изначальная кривая
	const VCurve * curve;
	
	long wayCat;				///< Категория пути	

	/// Пороги непогашенного, пси и базы для расчета
	double anpHighBTRangeMid;   ///< Порог для среднего Анп на высокой скорости с наклоном кузова
	double anpHighBTRangeMax;	///< Порог для максимума Анп на высокой скорости с наклоном кузова
	double anpHighRangeMid;     ///< Порог для среднего Анп на высокой скорости 
	double anpHighRangeMax;		///< Порог для максимума Анп на высокой скорости 
	double anpPassRangeMid;		///< Порог для среднего Анп на пасс.скорости
	double anpPassRangeMax;		///< Порог для максимума Анп на пасс.скорости
	double anpTruckRangeMid;	///< Порог для среднего Анп на груз.скорости
	double anpTruckRangeMax;	///< Порог для максимума Анп на груз.скорости
	double psiHighBTRangeMid;	///< Порог для среднего Пси на высоких скоростях с наклоном кузова
	double psiHighBTRangeMax;	///< Порог для максимума Пси на высоких скоростях с наклоном кузова
	double psiHighRangeMid;		///< Порог для среднего Пси на высоких скоростях
	double psiHighRangeMax;		///< Порог для максимума Пси на высоких скоростях
	double psiPassRangeMid;		///< Порог для среднего Пси на пасс.скоростях
	double psiPassRangeMax;		///< Порог для максимума Пси на пасс.скоростях
	double psiTruckRangeMid;	///< Порог для среднего Пси на груз.скоростях
	double psiTruckRangeMax;	///< Порог для максимума Пси на груз.скоростях
	double baseMid;				///< База усреднения 
	double baseTap;				///< База для вычисления Пси и отводов
	void SetWayCat(VDataSet & ds, bool isModel);	///< Установить категорию пути

	std::vector<uint32_t> ranges;					///< Индексы границ кривой
	void SetRanges(VDataSet & ds);					///< Установка границ кривой

	/// Число кадров на участке
	uint32_t GetKadrCount() const
	{
		return this->ranges.at(1) - this->ranges.at(0) + 1;
	}

	std::vector<double> vprkPass;					///< Массив скоростей по приказу [пассажирские]
	std::vector<double> vprkTruck;					///< Массив скоростей по приказу [грузовые]
	std::vector<double> vprkThird;					///< Массив скоростей по приказу [60/средневзвешенная]
	std::vector<double> vprkHigh;					///< Массив скоростей по приказу [высокие]
	std::vector<double> vprkHighBT;					///< Массив скоростей по приказу [высокие|накл.кузова]
	void SetSpeeds(VDataSet & ds , bool isModel);	///< Установка скоростей по приказу

	/// Участок скорости на кривой
	struct SpdArea
	{
		uint32_t idxFrom;	///< индекс кадра начала отрезка
		uint32_t idxTo;		///< индекс кадра конца отрезка
		speed_t spdPass;	///< скорость пассажирская
		speed_t spdTruck;	///< скорость грузовая
		speed_t spdThird;	///< третья скорость
		speed_t spdHigh;	///< высокая скорость
		speed_t spdHighBT;	///< высокая скорость с наклоном кузова
	};

	/// Создания сжатого списка скоростей из покадрового
	std::vector<VCurveTempParams::SpdArea> SelectSpeeds(VDataSet & ds, size_t from, size_t to, bool isModel);

	std::vector<double> kurs;						///< Массив измерений курса
	void CalcKurs(VDataSet & ds, bool isModel);		///< Пересчет курса

	std::vector<double> midPlan;					///< Массив средней кривизны
	void CalcMidPlan(VDataSet & ds, bool isModel);	///< Пересчет средней кривизны

	std::vector<double> midLevel;					///< Массив среднего уровня
	void CalcMidLevel(VDataSet & ds, bool isModel);	///< Пересчет среднего уровня	
	
	std::vector<double> radius;						///< Массив мгновенных радиусов
	void CalcRadius(VDataSet & ds, bool isModel);	///< Пересчет мгновенных радиусов
	
	std::vector<double> anpPass;					///< Непогашенное ускорение [пассажирское]
	std::vector<double> anpTruck;					///< Непогашенное ускорение [грузовое]
	std::vector<double> anpThird;					///< Непогашенное ускорение [60|средняя скорость]
	std::vector<double> anpHigh;					///< Непогашенное ускорение [высокие скорости]
	std::vector<double> anpHighBT;					///< Непогашенное ускорение [высокие скорости|накл.кузова]
	void CalcAnp(VDataSet & ds, bool isModel);		///< Пересчет непогашенных

	std::vector<double> vkrPass;					///< Мгновенные Vкр пассажирские
	std::vector<double> vkrTruck;					///< Мгновенные Vкр грузовые
	std::vector<double> vkrHigh;					///< Мгновенные Vкр [высокие скорости]
	std::vector<double> vkrHighBT;					///< Мгновенные Vкр [высокие скорости|накл.кузова]
	void CalcVkr(VDataSet & ds, bool isModel);		///< Пересчет Vкр
    
    /// Среднее Vкр на круговой
    long GetMidVkr(VDataSet & ds, bool isModel, float anpRange, uint32_t radiusNdx);

	std::vector<double> psiPass;					///< Пси - отвод Анп [пассажирское]
	std::vector<double> psiTruck;					///< Пси - отвод Анп [грузовое]
	std::vector<double> psiHigh;					///< Пси - отвод Анп [высокие скорости]
	std::vector<double> psiHighBT;					///< Пси - отвод Анп [высокие скорости|накл.кузова]
	std::vector<double> vizPass;					///< Мгновенные Vиз [пассажирское]
	std::vector<double> vizTruck;					///< Мгновенные Vиз [грузовое]
	std::vector<double> vizHigh;					///< Мгновенные Vиз [высокие скорости]
	std::vector<double> vizHighBT;					///< Мгновенные Vиз [высокие скорости|накл.кузова]
	void CalcPsiViz(VDataSet & ds, bool isModel);	///< Пересчет псей и Vиз

	std::vector<double> tapPlan;					///< Массив отводов кривизны
	void CalcTapPlan(VDataSet & ds, bool isModel);	///< Расчет отвода кривизны
	
	std::vector<double> tapLevel;					///< Массив отводов уровня
	void CalcTapLevel(VDataSet & ds, bool isModel);	///< Расчет отвода уровня
	
	std::vector<double> vprPass;					///< Массив мгновенных Vpr пассажирских
	std::vector<double> vprTruck;					///< Массив мгновенных Vpr грузовых
	std::vector<double> vprPassRepair;              ///< Массив мгновенных Vpr пассажирских по ремонтным нормативам
	std::vector<double> vprTruckRepair;             ///< Массив мгновенных Vpr грузовых по ремонтным нормативам
	void CalcVpr(VDataSet & ds, bool isModel);		///< Расчет мгновенных Vпр

	std::vector<double> levelOffset;
	void CalcLevelOffset(VDataSet & ds);			///< Отклонение модели от факта на уровне

	// сбросить массив в файл
	bool DumpToFile(const std::wstring & fn, const std::vector<double> & prm, VDataSet & ds);

	/// Минимум на отрезке
	double GetMin(std::vector<double> & param, size_t from, size_t to);

	/// Максимум на отрезке
	double GetMax(std::vector<double> & param, size_t from, size_t to);

	/// Максимум по модулю на отрезке
	double GetMaxAbs(std::vector<double> & param, size_t from, size_t to, uint32_t & maxNdx);

	/// Среднее на отрезке
	double GetAvg(std::vector<double> & param, size_t from, size_t to);

	/// Сброс пересчитанных значений
	void Drop();
};

/// Фиктивный "номер кривой" для получения характеристик целой кривой
const uint32_t COMPLETE_CURVE = static_cast<uint32_t>(-1);

/// Декларируем список кривых
class VCurveList;

/// "Неуд" ремонту по отводу возвышения при некоей скорости по приказу
bool IsTapFailure(double prk_spd, double tap, bool is_repair);

/// Участок потенциальной кривой
class VProtoCurve 
{
private:
	VExcess excess;
public:
	/// Конструктор
	VProtoCurve(const VExcess& in_excess) : excess(in_excess) {};
	/// Возвращает координату начала
	double GetStart() const {return excess.GetStart();};
	/// Возвращает координату конца
	double GetEnd() const {return excess.GetEnd();};
	/// Возвращает длину
	double GetLength() const {return excess.GetLength();};
	/// Возвращает сторону
	side_t GetSide() const {return excess.GetAverage()>0 ? SD_RIGHT : SD_LEFT;};
};

/// Поиск участков кривых
class VFindCurve : public VDataProcessor<VPoint, VProtoCurve>
{
private:	
	/// Данные на выходе
	std::deque<VProtoCurve> output;
	/// Левые кривые (кривизна <0)
	VFindExcess fel;
	/// Условие для поиска левых кривых
	VCurveCondition rcl;
	/// Правые кривые (кривизна >0)
	VFindExcess fer;
	/// Условие для поиска правых кривых
	VCurveCondition rcr;
public:
	/// Конструктор
	VFindCurve(double in_min_curv=0);
	/// Помещает данные на вход
	void Put(const VPoint& in_point);
	/// Возвращает true, если есть обработанные данные
	bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	bool Empty() const;		
	/// Возвращает первый элемент данных
	const VProtoCurve& Front() const;
	/// Выталкивает первый элемент данных
	void Pop();
};

/// Поиск переходных
class VFindTransition : public VDataProcessor<VPoint, VExcess>
{
private:	
	/// Данные на выходе
	std::deque<VExcess> output;
	/// Левые кривые (кривизна <0)
	VFindExcess fe_pos;
	/// Условие для поиска левых кривых
	VRangeCondition rc_pos;
	/// Правые кривые (кривизна >0)
	VFindExcess fe_neg;
	/// Условие для поиска правых кривых
	VRangeCondition rc_neg;
public:
	/// Конструктор
	VFindTransition(double in_min_curv);
	/// Помещает данные на вход
	void Put(const VPoint& in_point);
	/// Возвращает true, если есть обработанные данные
	bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	bool Empty() const;		
	/// Возвращает первый элемент данных
	const VExcess& Front() const;
	/// Выталкивает первый элемент данных
	void Pop();	
	/// Сбрасываем остаток данных
	void Flush();
};



/// Одиночная кривая
class VCurve
{
	friend VCurveList;
	friend VCurveTempParams;

	/// Характеристики модели
	VCurveModelInfo model;	

	/// Кривая модельная 
	bool isModel;

	/// Метки на плане и уровне
	std::vector<VCurveMark> planMark, levelMark;	

	/// апроксимация в точке по трапеции
	double AproxValue(size_t index, 
		const std::vector<VCurveMark> & marks, 
		const VAbsCoordData & absCoord) const;

	/// Временные параметры кривой
	mutable VCurveTempParams tempParams;	

	/// степень расстройства по показателю
	int GetDegree(float p, const float table[]) const
	{
		return p > table[0] ? 3 : p > table[1] ? 2 : p > table[2] ? 1 : 0;
	}

	//Данные для поддержки разметки и моделирования

	///Непогашенные ускорения
	mutable std::vector<VAnp> modelAnp;

public:

	/// Конструктор
	VCurve()
	{
		this->isModel = false;
		this->tempParams.curve = this;
		this->tempParams.wayCat = WAYCAT_UNSET;
	}

	/// Копирующий конструктор
	VCurve(const VCurve & crv)
	{
		this->planMark = crv.planMark;
		this->levelMark = crv.levelMark;
		this->model = crv.model;
		this->isModel = crv.isModel;
		this->tempParams = crv.tempParams;
		this->tempParams.curve = this;
	}

	/// Экспортировать в формат Aprinfo
	VAprinfoCurve ExportToAprinfo(VDataSet & ds);

	/// Сбросить расчетные величины
	void DropCalculations()
	{
		this->tempParams.Drop();
	}

	/// Вернуть модельность кривой
	bool IsModel() const
	{
		return this->isModel;
	}

	/// Установить "модельность" кривой
	void SetModel(bool isModel)
	{
		this->isModel = isModel;
	}

	/// Вернуть комментарий модели
	const char * GetModelComment() const
	{
		return this->model.comment;
	}

	/// Установить комментарий модели
	void SetModelComment(const char * comment)
	{
		strncpy_s(this->model.comment, COMMENT_SIZE, comment, COMMENT_SIZE-1);
		this->model.comment[COMMENT_SIZE-1] = 0;
	}

	/// Вернуть статус модели
	int GetModelStatus() const
	{
		if (this->model.status >= 0 && this->model.status < 5)
			return this->model.status;
		else
			return 0;
	}

	/// Установить статус модели
	void SetModelStatus(int status)
	{
		this->model.status = status;
	}

	/// Вернуть описание статуса модели
	const char * GetModelStatusDescription() const;


	/// Вернуть скорость (скор. с наклоном кузрва)
	speed_t GetModelVhigh_bt() const
	{
		return this->model.Vhigh_bt;
	};
	
	/// Вернуть скорость (скор.)
	speed_t GetModelVhigh() const
	{
		return this->model.Vhigh;
	};

	/// Вернуть скорость (пасс.)
	speed_t GetModelVpass() const
	{
		return this->model.Vpass;
	}; 

	/// Вернуть скорость (груз.)
	speed_t GetModelVcrg() const
	{
		return this->model.Vcrg;
	}; 

	/// Вернуть скорость (средн.)
	speed_t GetModelVavg() const
	{
		return this->model.Vavg;
	};

	/// Вернуть скорость (мин.)
	speed_t GetModelVmin() const
	{
		return this->model.Vmin;
	};


	/// Установить скорость (скор. с накл.)
	void SetModelVhigh_bt(speed_t Vhigh_bt)
	{
		this->model.Vhigh_bt=Vhigh_bt;
	};
	
	
	/// Установить скорость (скор.)
	void SetModelVhigh(speed_t Vhigh)
	{
		this->model.Vhigh=Vhigh;
	};

	/// Установить скорость (пасс.)
	void SetModelVpass(speed_t Vpass)
	{
		this->model.Vpass=Vpass;
	}; 

	/// Установить скорость (груз.)
	void SetModelVcrg(speed_t Vcrg)
	{
		this->model.Vcrg=Vcrg;
	}; 

	/// Установить скорость (средн.)
	void SetModelVavg(speed_t Vavg)
	{
		this->model.Vavg=Vavg;
	};

	/// Установить скорость (мин.)
	void SetModelVmin(speed_t Vmin)
	{
		this->model.Vmin=Vmin;
	};

	/// Число меток
	size_t GetMarkCount() const
	{
		return this->planMark.size();
	}

	/// Число меток на уровне
	size_t GetLevelMarkCount() const
	{
		return this->levelMark.size();
	}

	/// Добавить метку на план
	void AddPlanMark(uint32_t index, double value)
	{		
		for (size_t i=0; i<planMark.size(); ++i)
		{
			if (planMark.at(i).index==index)
				return;
		}
		this->planMark.push_back(VCurveMark(index, value));
		std::sort(this->planMark.begin(), this->planMark.end());
	}
	
	/// Удалить метку на плане
	bool DeletePlanMark(size_t index)
	{
		if (index < this->planMark.size())
		{			
			this->planMark.erase(this->planMark.begin() + index);			
			return true;
		}
		else 
			return false;
	}

	/// Метка на плане
	const VCurveMark & GetPlanMark(size_t index) const
	{
		return this->planMark.at(index);
	}

	/// Метка на плане
	VCurveMark & GetPlanMark(size_t index)
	{
		return this->planMark.at(index);
	}

	/// Добавить метку на уровень
	void AddLevelMark(uint32_t index, double value)
	{
		for (size_t i=0; i<levelMark.size(); ++i)
		{
			if (levelMark.at(i).index==index)
				return;
		}
		this->levelMark.push_back(VCurveMark(index, value));
		std::sort(this->levelMark.begin(), this->levelMark.end());
	}

	/// Удалить метку на плане
	bool DeleteLevelMark(size_t index)
	{
		if (index < this->levelMark.size())
		{
			this->levelMark.erase(this->levelMark.begin() + index);
			return true;
		}
		else 
			return false;
	}

	/// Метка на уровне
	const VCurveMark & GetLevelMark(size_t index) const
	{
		return this->levelMark.at(index);
	}

	/// Метка на уровне
	VCurveMark & GetLevelMark(size_t index)
	{
		return this->levelMark.at(index);
	}

	/// Число радиусов
	size_t GetRadiusCount() const
	{
		return this->planMark.size() / 2 - 1;
	}
	
	/// Возвращает радиус FixMe!!! не ипользуется
	double GetRadius(size_t in_index) const
	{
		return curv_to_radius(this->planMark.at(radius_start_mark(in_index)).value);
	}
	
	/// Устанавливает радиус FixMe!!! не ипользуется
	void SetRadius(size_t in_index, double in_radius)
	{
		this->planMark.at(radius_start_mark(in_index)).value = radius_to_curv(in_radius);
		this->planMark.at(radius_end_mark(in_index)).value = radius_to_curv(in_radius);
	}

	/// Список меток на плане
	const std::vector<VCurveMark> & GetPlanMarkList() const
	{
		return this->planMark;
	}

	/// Список меток на плане
	const std::vector<VCurveMark> & GetLevelMarkList() const
	{
		return this->levelMark;
	}

	/// Проверка числа меток
	bool CheckMarks() const;

	/// апроксимация значения на плане по трапеции
	double AproxPlan(size_t index, const VAbsCoordData & absCoord) const
	{
		return this->AproxValue(index, this->planMark, absCoord);
	}

	/// апроксимация значения на уровне по трапеции
	double AproxLevel(size_t index, const VAbsCoordData & absCoord) const
	{
		return this->AproxValue(index, this->levelMark, absCoord);
	}

	/// Категория пути, под которую расчитана кривая
	long GetWayCat(VDataSet & ds) const;

	// ---------------------------------------------------------------------
	// характеристики целой и элементарных кривых
	// ---------------------------------------------------------------------

	/// Индекс метки, ограничивающей кривую
	size_t GetRangeMark(uint32_t radius, bool circ, bool begin) const;

	/// Абс.к-та границы кривой
	double GetRangeCoord(VDataSet & ds, 
		bool plan, bool circ, bool begin, 
		uint32_t radius = COMPLETE_CURVE) const;

	/// Проверка попадания кривой в указанные границы (в абсолютных координатах)
	bool FitsRanges(VDataSet & ds, double from, double to) const;

	/// Проверка попадания кривой в границы пути, согласно Апринфо
	bool FitsWayRanges(VDataSet & ds) const;

	/// Минимальный радиус (на круговой кривой)
	double GetMinRadius(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Средний радиус (на круговой кривой)
	double GetAvgRadius(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Максимальный радиус (на круговой кривой)
	double GetMaxRadius(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Минимальное возвышение (на круговой кривой)
	double GetMinLevel(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Среднее возвышение (на круговой кривой)
	double GetAvgLevel(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Максимальное возвышение (на круговой кривой)
	double GetMaxLevel(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Среднее фактическое возвышение (на круговой кривой)
	double GetAvgFactLevel(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Минимальное ШК (на круговой кривой)
	double GetMinWidth(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Среднее ШК (на круговой кривой) 
	double GetAvgWidth(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Максимальное ШК (на круговой кривой)
	double GetMaxWidth(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Процент сбоев на ШК
	double GetWidthErrorPercent(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Отклонение ШК спереди от ШК - максимум
	double GetMaxDeltaWidthFront(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Отклонение ШК спереди от ШК - среднее
	double GetAvgDeltaWidthFront(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Процент сбоев при расчете отклонения ШК спереди от ШК
	double GetDeltaWidthFrontErrorPercent(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Отклонение ШК сзади от ШК - максимум
	double GetMaxDeltaWidthBack(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Отклонение ШК сзади от ШК - среднее
	double GetAvgDeltaWidthBack(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Процент сбоев при расчете отклонения ШК спереди от ШК
	double GetDeltaWidthBackErrorPercent(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;
	
	/// Угол поворота кривой (на целой кривой)
	double GetAngle(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Ориентация кривой
	long GetOrientation(VDataSet & ds) const;

	/// Знак кривизны/возвышения
	double GetSign(VDataSet & ds) const;

	/// максимум пассажирского непогашенного (на целой кривой)
	double GetMaxAnpPass(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// среднее пассажирское непогашенное (на круговой кривой)
	double GetAvgAnpPass(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// максимум грузового непогашенного (на целой кривой)
	double GetMaxAnpTruck(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// среднее грузовое непогашенное (на круговой кривой)
	double GetAvgAnpTruck(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// максимум непогашенного под 3-ю скорость (на целой кривой)
	double GetMaxAnpThird(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// среднее непогашенное под 3-ю скорость (на круговой кривой)
	double GetAvgAnpThird(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// максимум непогашенного [высокие скорости]
	double GetMaxAnpHigh(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// среднее непогашенное [высокие скорости]
	double GetAvgAnpHigh(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// максимум непогашенного [высокие скорости|наклон кузова]
	double GetMaxAnpHighBT(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// среднее непогашенное [высокие скорости|наклон кузова]
	double GetAvgAnpHighBT(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// максимум пассажирского пси (на всей кривой)
	double GetMaxPsiPass(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// максимум грузового пси (на всей кривой)
	double GetMaxPsiTruck(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// максимум пси [высокие скорости]
	double GetMaxPsiHigh(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// максимум пси [высокие скорости|наклон кузова]
	double GetMaxPsiHighBT(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// скорость V+0.3
	speed_t GetVPlus03(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// скорость V-0.3
	speed_t GetVMinus03(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// вершина угла поворота
	point_t GetTurnPoint(VDataSet & ds) const;

	/// тангенсы
	double GetTan1(VDataSet & ds) const;
	double GetTan2(VDataSet & ds) const;

	// ---------------------------------------------------------------------
	// характеристики переходных кривых
	// ---------------------------------------------------------------------

	/// отводы в плане на переходной кривой
	double GetMaxPlanTap(VDataSet & ds, uint32_t num) const;

	/// отводы в плане на переходной кривой
	double GetAvgPlanTap(VDataSet & ds, uint32_t num) const;

	/// отводы возвышения на переходной кривой
	double GetMaxLevelTap(VDataSet & ds, uint32_t num) const;

	/// отводы возвышения на переходной кривой
	double GetAvgLevelTap(VDataSet & ds, uint32_t num) const;

    /// наличие ограничения по отводу возвышения на переходной кривой
    bool IsLimitTap(VDataSet & ds, bool is_repair, uint32_t trans_num) const;

	// ---------------------------------------------------------------------
	// показатели расстройства
	// ---------------------------------------------------------------------

	/// показатель расстройства в плане
	double GetPRPlan(VDataSet & ds) const;

	/// степень расстройства в плане
	int GetDegreePlan(VDataSet & ds) const
	{
		static const float table[] = { 0.5f, 0.3f, 0.2f };
		return this->GetDegree(this->GetPRPlan(ds), table);
	}

	/// показатель расстройства по уровню
	double GetPRLevel(VDataSet & ds) const;

	/// степень расстройства по уровню
	int GetDegreeLevel(VDataSet & ds) const
	{
		static const float table[] = { 3.0f, 2.0f, 1.0f };
		return this->GetDegree(this->GetPRLevel(ds), table);
	}

	/// показатель расстройства по непогашенному
	double GetPRAnp(VDataSet & ds) const;

	/// степень расстройства по непогашенному
	int GetDegreeAnp(VDataSet & ds) const
	{
		static const float table[] = { 0.6f, 0.4f, 0.3f };
		return this->GetDegree(this->GetPRAnp(ds), table);
	}

	/// показатель расстройства по координатам
	double GetPRCoord(VDataSet & ds) const;

	/// степень расстройства по координатам
	int GetDegreeCoord(VDataSet & ds) const
	{
		static const float table[] = { 3.0f, 1.5f, 1.0f };
		return this->GetDegree(this->GetPRCoord(ds), table);
	}

	/// суммарный показатель расстройства 
	double GetPRSum(VDataSet & ds) const
	{
		return this->GetPRAnp(ds) + this->GetPRPlan(ds) * 0.5f + 
			this->GetPRLevel(ds) * 0.25f + this->GetPRCoord(ds) * 0.2f;
	}

	// ---------------------------------------------------------------------
	// Скорости на кривой
	// ---------------------------------------------------------------------

	/// Список пассажирских скоростей на кривой
	std::vector<VCurveSpdArea> GetVprkPass(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Список пассажирских скоростей на кривой
	std::vector<VCurveSpdArea> GetVprkTruck(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Список "третьих" скоростей на кривой
	/// По факту - одна скорость, средняя на модели, 60 на обычной кривой
	std::vector<VCurveSpdArea> GetVprkThird(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Список высоких скоростей на кривой
	std::vector<VCurveSpdArea> GetVprkHigh(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Список высоких скоростей на кривой [с наклоном кузова]
	std::vector<VCurveSpdArea> GetVprkHighBT(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Наличие высоких скоростей
	bool HasVprkHigh(VDataSet & ds) const;

	/// Наличие высоких скоростей [с наклоном кузова]
	bool HasVprkHighBT(VDataSet & ds) const;

	/// список скоростей Vкр пассажирских
	std::vector<VCurveSpdArea> GetVkrPass(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// список скоростей Vкр грузовых
	std::vector<VCurveSpdArea> GetVkrTruck(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// список скоростей Vкр [высокие скорости]
	std::vector<VCurveSpdArea> GetVkrHigh(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// список скоростей Vкр [высокие скорости|наклон кузова]
	std::vector<VCurveSpdArea> GetVkrHighBT(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Скорость Vкр (средняя на круговой) под заданное ускорение
	speed_t GetMidVkr(VDataSet & ds, float anpRange, uint32_t radius = COMPLETE_CURVE) const; 

	/// список скоростей Vпр пассажирских
	std::vector<VCurveSpdArea> GetVprPass(VDataSet & ds, bool isRepair, uint32_t radius) const;

	/// список скоростей Vпр грузовых
	std::vector<VCurveSpdArea> GetVprTruck(VDataSet & ds, bool isRepair, uint32_t radius) const;

	/// список скоростей Vиз пассажирских 
	std::vector<VCurveSpdArea> GetVizPass(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// список скоростей Vиз грузовых
	std::vector<VCurveSpdArea> GetVizTruck(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// список скоростей Vиз [высокие скорости]
	std::vector<VCurveSpdArea> GetVizHigh(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// список скоростей Vиз [высокие скорости|наклон кузова]
	std::vector<VCurveSpdArea> GetVizHighBT(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;
	
	// ---------------------------------------------------------------------
	// Пороги Анп и Пси
	// ---------------------------------------------------------------------
	double GetAnpPassRangeMax() const;
	double GetAnpPassRangeMid() const;
	double GetPsiPassRangeMax() const;
	double GetPsiTruckRangeMax() const;

	// ---------------------------------------------------------------------
	// Износы
	// ---------------------------------------------------------------------

	/// Максимальный износ на кривой
	double GetMaxWear(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Средний износ на кривой
	double GetAvgWear(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// СКО износа на кривой
	double GetSkoWear(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Коррекция
	double GetCorrWear(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const
	{
		return this->GetAvgWear(ds, radius) + 1.5 * this->GetSkoWear(ds, radius);
	}

	/// Процент сбоев на кривой
	double GetWearErrorPercent(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Длина участка с превышением указанного порога
	double GetWearLenOverLimit(VDataSet & ds, float limit, uint32_t radius = COMPLETE_CURVE) const;

	// ---------------------------------------------------------------------
	// Характеристики модели
	// ---------------------------------------------------------------------
	
	/// Максимальное отклонение факта от модели в плане
	double GetMaxPlanOffset(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Среднее отклонение факта от модели в плане
	double GetAvgPlanOffset(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Максимальное отклонение факта от модели на уровне
	double GetMaxLevelOffset(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// Среднее отклонение факта от модели на уровне
	double GetAvgLevelOffset(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	// ---------------------------------------------------------------------
	// Попикетная и позвеньевая оценка оценка и оценка по участкам для ФПО
	// ---------------------------------------------------------------------
	
	/// Список пикетов на кривой, со скоростями
	std::vector<VCurvePicket> GetPkList(VDataSet & ds, bool isRepair) const;

	/// Список пикетов на кривой, со износами
	std::vector<VCurveWearPicket> GetWearPkList(VDataSet & ds) const;

	/// Список звеньев на кривой, с износами
	std::vector<VCurveLink> GetLinkList(VDataSet & ds) const;

	/// Список участков оценки для ФПО
	std::vector<VCurveFPOArea> GetFPOAreaList(VDataSet & ds) const;
	
	///Диаграмма износов
	std::set<density_t> GetWearDiagram(VDataSet & ds) const;

	// ---------------------------------------------------------------------
	// Массивы данных на кривой
	// ---------------------------------------------------------------------

	/// Скорость по приказу - пассажирская
	const std::vector<double> & GetArrayVprkPass(VDataSet & ds) const;

	/// Скорость по приказу - грузовая
	const std::vector<double> & GetArrayVprkTruck(VDataSet & ds) const;

	/// Курс
	const std::vector<double> & GetArrayKurs(VDataSet & ds) const;

	/// Среднее кривизны
	const std::vector<double> & GetArrayMidPlan(VDataSet & ds) const;

	/// Среднее уровня
	const std::vector<double> & GetArrayMidLevel(VDataSet & ds) const;
	
	// Непогашенное ускорение (скор.)
	const  std::vector<double> & GetArrayAnpHigh(VDataSet& ds) const;
	
	// Непогашенное ускорение (скор. НК)
	const  std::vector<double> & GetArrayAnpHighBT(VDataSet& ds) const;

	/// Непогашенное ускорение (пасс.)
	const  std::vector<double> & GetArrayAnpPass(VDataSet& ds) const;

	/// Непогашенное ускорение (груз.)
	const  std::vector<double> & GetArrayAnpTruck(VDataSet& ds) const;

	/// Радиус
	const std::vector<double> & GetArrayRadius(VDataSet & ds) const;

	/// Скорость Vкр пассажирская
	const std::vector<double> & GetArrayVkrPass(VDataSet & ds) const;

	/// Скорость Vкр грузовая
	const std::vector<double> & GetArrayVkrTruck(VDataSet & ds) const;

	/// Пси пассажирская
	const std::vector<double> & GetArrayPsiPass(VDataSet & ds) const;

	/// Пси грузовая
	const std::vector<double> & GetArrayPsiTruck(VDataSet & ds) const;

	/// Скорость Vиз пассажирская
	const std::vector<double> & GetArrayVizPass(VDataSet & ds) const;

	/// Скорость Vиз грузовая
	const std::vector<double> & GetArrayVizTruck(VDataSet & ds) const;

	/// Отвод кривизны
	const std::vector<double> & GetArrayTapPlan(VDataSet & ds) const;

	/// Отвод уровня
	const std::vector<double> & GetArrayTapLevel(VDataSet & ds) const;

	/// Скорость Vпр
	const std::vector<double> & GetArrayVprPass(VDataSet & ds, bool isRepair) const;
	
	/// Оператор сравнения
	bool operator < (const VCurve & other)
	{
		if (this->GetMarkCount()>0 && other.GetMarkCount()>0)
			return this->GetPlanMark(0).index < other.GetPlanMark(0).index;
		else if (this->GetLevelMarkCount()>0 && other.GetLevelMarkCount()>0)
			return this->GetLevelMark(0).index < other.GetLevelMark(0).index;
		else
			return false;
	}

	///Оператор присваивания
	VCurve& operator=(const VCurve& in_curve)
	{		
		model=in_curve.model;
		isModel=in_curve.isModel;
		planMark=in_curve.planMark;
		levelMark=in_curve.levelMark;
		tempParams=in_curve.tempParams;
		tempParams.curve=this;
		return *this;
	}

	// ---------------------------------------------------------------------
	//Функции поддержки разметки FixMe!!! эти функции можно сделать внешними
	// ---------------------------------------------------------------------
	
	///Сброс временных параметров
	void Drop();
	///Возвращает площадь под трапецией
	double GetCrvArea(VDataSet & ds);
	///Изменяет радиус кривой так чтобы площадь под трапецией соответствовала углу поворота
	void FixCrvArea(VDataSet & ds);
	///перенос меток с кривизны на уровень
	void TransferMarksToLevel(VDataSet & ds);
	///Нормализация разметки по кривизне
	void NormalizePlanMarks(VDataSet & ds);
	///Нормализация разметки по уровню
	void NormalizeLevelMarks(VDataSet & ds);
	///Уточнение по методу наименьших квадратов для кривизны
	void MNKPlan(VDataSet & ds);
	///Уточнение по методу наименьших квадратов для уровня
	void MNKLevel(VDataSet & ds);
	///Нормализация разметки
	void NormalizeMarks(VDataSet & ds);
	///Оптимизация разметки
	void OptimizeMarks(VDataSet & ds);	
	///Приводит координаты меток на уровне в соответствие координатам меток на кривизне
	void FixLevelCoordinates(VDataSet & ds);
	///Проверяет корректность разметки по кривизне
	bool PlanMarksValid() const;
	///Проверяет корректность разметки по уровню
	bool LevelMarksValid() const;
	///Проверяет есть ли на кривой возвышение
	bool LevelExist(VDataSet & ds) const;
	/// Приводит значения уровня к допустимым значениям
	void AdjustLevelValue();
	///Устанавливает возвышение на ноль
	void ZeroLevel();
	///Увеличивает возвышение
	void IncreaseLevel(double in_step=5.0);
	///Уменьшает возвышение
	void DecreaseLevel(double in_step=5.0);
	///Возвращает PRM код в зависимости от признака is_model
	PARAMETER_TYPE GetParameterType() const {return IsModel() ? PRM_MODEL_CURVES : PRM_CURVE_MARKS;};
	///Возвращает левую допустимую границу для активной кривой на кривизне, ни одна метка не может быть поставлена левее этой границы 
	size_t GetPlanLeftMargin(VDataSet & ds) const;
	///Возвращает правую допустимую границу для активной кривой на кривизне, ни одна метка не может быть поставлена правее этой границы
	size_t GetPlanRightMargin(VDataSet & ds) const;
	///Возвращает левую допустимую границу для активной кривой на уровне, ни одна метка не может быть поставлена левее этой границы 
	size_t GetLevelLeftMargin(VDataSet & ds) const;
	///Возвращает правую допустимую границу для активной кривой на уровне, ни одна метка не может быть поставлена правее этой границы
	size_t GetLevelRightMargin(VDataSet & ds) const;
	///Возвращает левую допустимую границу для метки активной на кривизне, ни одна метка не может быть поставлена левее этой границы 
	size_t GetPlanMarkLeftMargin(VDataSet & ds, size_t in_mark_index) const;
	///Возвращает правую допустимую границу для метки активной кривой на кривизне, ни одна метка не может быть поставлена правее этой границы
	size_t GetPlanMarkRightMargin(VDataSet & ds, size_t in_mark_index) const;
	///Возвращает левую допустимую границу для метки активной на уровне, ни одна метка не может быть поставлена левее этой границы 
	size_t GetLevelMarkLeftMargin(VDataSet & ds, size_t in_mark_index) const;
	///Возвращает правую допустимую границу для метки активной кривой на уровне, ни одна метка не может быть поставлена правее этой границы
	size_t GetLevelMarkRightMargin(VDataSet & ds, size_t in_mark_index) const;					
	///Автоматическая разметка кривой
	void Automark(VDataSet& ds, size_t in_start, size_t in_end, bool in_model=false);	
	///Автоматическая разметка кривой (только один радиус)
	void Automark1(VDataSet& ds, size_t in_start, size_t in_end, bool in_model=false);		
	///Число элементов
	size_t GetElementCount() const
	{
		return this->GetMarkCount() - 1;
	}
	///Возвращает true если элемент - круговая
	bool GetElementType(size_t in_index) const;
	///Возвращает длину элемента
	double GetElementLength(VDataSet & ds, size_t in_index) const;
	///Возвращает радиус элемента
	double GetElementRadius(size_t in_index) const;
	///Возвращает длину элемента (уровень)
	double GetElementLengthLevel(VDataSet & ds, size_t in_index) const;
	///Возвращает возвышение элемента
	double GetElementLevel(size_t in_index) const;
	///Рассчитывает и возвращает массив из 4 непогашенных ускорений для элемента
	const std::vector<VAnp> GetElementAnp(VDataSet & ds, size_t in_index, int in_speed) const;
	///Рассчитывает и возвращает максимум скорости нарастания непогашенного ускорения для элемента
	float GetElementPsi(VDataSet & ds, size_t in_index, int in_speed) const;
	///Возвращает отвод возвышения
	float GetElementTap(VDataSet & ds, size_t in_index) const;
	///Возвращает Vкр для элемента
	double GetElementVkrHighBT(VDataSet & ds, size_t in_index) const;
	///Возвращает Vкр для элемента
	double GetElementVkrHigh(VDataSet & ds, size_t in_index) const;
	///Возвращает Vкр для элемента
	double GetElementVkrPass(VDataSet & ds, size_t in_index) const;
	///Возвращает Vкр для элемента
	double GetElementVkrTruck(VDataSet & ds, size_t in_index) const;
	///Возвращает Vпр для элемента
	double GetElementVpr(VDataSet & ds, size_t in_index) const;
	///Возвращает Vиз для элемента
	double GetElementVizHighBT(VDataSet & ds, size_t in_index) const;
	///Возвращает Vиз для элемента
	double GetElementVizHigh(VDataSet & ds, size_t in_index) const;
	///Возвращает Vиз для элемента
	double GetElementVizPass(VDataSet & ds, size_t in_index) const;
	///Возвращает Vиз для элемента
	double GetElementVizTruck(VDataSet & ds, size_t in_index) const;
	///Возвращает Анп скор. НК для элемента
	double GetElementAnpHighBT(VDataSet & ds, size_t in_index) const;
	///Возвращает Анп скор. для элемента
	double GetElementAnpHigh(VDataSet & ds, size_t in_index) const;
	///Возвращает Анп пасс. для элемента
	double GetElementAnpPass(VDataSet & ds, size_t in_index) const;
	///Возвращает Анп груз. для элемента
	double GetElementAnpTruck(VDataSet & ds, size_t in_index) const;
	///Возвращает Анп средн. для элемента
	double GetElementAnpThird(VDataSet & ds, size_t in_index) const;
	///Возвращает Пси снк
	float GetElementPsiHighBT(VDataSet & ds, size_t in_index) const;
	///Возвращает Пси ск
	float GetElementPsiHigh(VDataSet & ds, size_t in_index) const;
	///Возвращает Пси пасс
	float GetElementPsiPass(VDataSet & ds, size_t in_index) const;	
	///Возвращает Пси груз
	float GetElementPsiTruck(VDataSet & ds, size_t in_index) const;
	///Возвращает Пси ср
	float GetElementPsiThird(VDataSet & ds, size_t in_index) const;	
	///Рассчитывает и возвращает массив непогашенных ускорений
	const std::vector<VAnp>& GetElementAnp(const VAbsCoordData & in_abscoord, const VAprinfo& in_aprinfo, 
		speed_type in_speed, bool in_use_passport_speeds) const;			
	///Рассчитывает оптимальное возвышение
	void CreateLevelOptimal(VDataSet & ds);
	///Рассчитывает возвышение близкое к факту
	void CreateLevelFact(VDataSet & ds);
	///Рассчитывает возвышение рациональное
	void CreateLevelRational(VDataSet & ds);
	/// Возвращает true, если точка с заданным индексом длежит внутри кривой
	int HasPointCrv(size_t in_index) const;
	/// Возвращает true, если точка с заданным индексом длежит внутри кривой
	int HasPointLvl(size_t in_index) const;
	/// Устанавливает скорости для модели (скорости по приказу)
	void ResetModelSpeeds(VDataSet & ds);
};

///Целевая функция для агоритмов оптимизации
class VCriterionFunction : public VFunction
{
private:
	///Указатель на оптимизируемую кривую FixMe!!! лучше shared_ptr
	VCurve * curve;
	///Указатель на используемый DataSet FixMe!!! лучше shared_ptr
	VDataSet * data_set;	
public :
	/// Конструктор
	VCriterionFunction(VCurve * in_curve, VDataSet * in_data_set);
	/// Метод, устанавливающий значение параметра с номером index равным value
	void SetParam(size_t index, double value);
	/// Метод, возвращающий значение параметра с номером index		
	double GetParam(size_t index);
	/// Метод, устанавливающий границы параметра с номером index равным value		
	void SetParamRange(size_t index, const pro::interval<double>& value);
	/// Метод, возвращающий границы параметра с номером index		
	pro::interval<double> GetParamRange(size_t index);
	/// Метод, обновляющий текущее значение функции		
	void Update();
	/// Метод, возвращающий текущее значение функции		
	double Value();
	/// Метод, возвращающий количество параметров		
	size_t GetParamCount();
};
