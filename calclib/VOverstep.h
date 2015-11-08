#pragma once

#include "VParameter.h"
#include "oversteps.h"

/// Типы обнаруживаемых отступлений
const int ovrUnknown	=   0; ///< Неизвестное отступление
const int ovrWidthPl	=   1; ///< Уширение [2]
const int ovrWidthMn	=   2; ///< Сужение [2]
const int ovrLevel		=   3; ///< Плавное отступление по уровню [2]
const int ovrDistort	=   4; ///< Перекос по уровню [2]
const int ovrPitR		=   5; ///< Просадка правая [2]
const int ovrPitL		=   6; ///< Просадка левая [2]
const int ovrRichR		=   7; ///< Рихтовка правая [2]
const int ovrRichL		=   8; ///< Рихтовка левая [2]
const int ovrOtvod		=   9; ///< Отвод ширины колеи [4]
const int ovrDistShort	=  10; ///< Перекос на базе тележки [4]
const int ovrDistLong	=  11; ///< Перекос на базе вагона  [4]

const int ovrAnp		=  15; ///< Непогашенное ускорение [4]
const int ovrKsi		=  16; ///< Скорость нарастания непогашенного ускорения [4]
const int ovrOtvodH		=  17; ///< Отвод возвышения [4]
const int ovrZazor		=  18; ///< Зазор [4]
const int ovrIznos		=  19; ///< Износ [4]
const int ovrRich		=  20; ///< Рихтовка по рихтовочной нити [2]
const int ovrRichN		=  21; ///< Рихтовка по нерихтовочной нити [2]

const int ovrZazorR     =  22; ///< Зазор левый [4]
const int ovrZazorL     =  23; ///< Зазор правый [4]

const int ovrHoleSh		= 100; ///< Сбой измерений по ШК
const int ovrHoleLev	= 101; ///< Сбой измерений по уровню
const int ovrHolePrR	= 102; ///< Сбой измерений по прос.пр
const int ovrHolePrL	= 103; ///< Сбой измерений по прос.лев
const int ovrHoleRhR	= 104; ///< Сбой измерений по рих.пр
const int ovrHoleRhL	= 105; ///< Сбой измерений по рих.лев

const int ovrSpeed1		= 120; ///< изм. V прик
const int ovrSpeed2		= 121; ///< изм. V временное

const int ovrKmChange	= 140; ///< индикатор смены км.
const int ovrStrelka	= 150; ///< индикатор стрелки
const int ovrStrOptLt   = 151; ///< стрелка, остряк, левый опт.датчик
const int ovrStrOptRt   = 152; ///< стрелка, остряк, правый опт.датчик
const int ovrStrXOptLt  = 153; ///< стрелка, крестовина, левый опт.датчик
const int ovrStrXOptRt  = 154; ///< стрелка, крестовина, правый опт.датчик

/// имя отступления
std::string GetOverstepName(int type);

/// маски отступлений
const unsigned long ovmDefault = 0x00000000; ///< заранее определенная маска
const unsigned long ovmAll     = 0xFFFFFFFF; ///< грузить всё
const unsigned long ovmLimit   = 0x00000001; ///< ограничения скорости
const unsigned long ovmDeg4    = 0x00000002; ///< отступления 4 степени
const unsigned long ovmDeg3    = 0x00000004; ///< отступления 3 степени
const unsigned long ovmDeg2    = 0x00000008; ///< отступления 2 степени
const unsigned long ovmOverLen = 0x00000010; ///< превышения длины
const unsigned long ovmHoles   = 0x00000020; ///< сбои измерений
const unsigned long ovmSwitch  = 0x00000040; ///< стрелки
const unsigned long ovmTech    = 0x00000080; ///< странные "технологические" вещи: смена скоростей, границы км 

/// Дополнительный тип неисправности [старый вариант]
const int decCombine	= 0x00000001; ///< сочетание
const int decSequence	= 0x00000002; ///< последовательность

/// новая система обозначения объектов и доп.неисправностей
const int decNewObject	= 0x00000080; 

/// Сочетания 3 и 4 степеней, мосты и туннели
const int decC			= 0x00000100; ///< C 
const int decPP			= 0x00000200; ///< PP 
const int decThreePr	= 0x00000400; ///< 3Пр 
const int decBridge		= 0x00000800; ///< Мост 
const int decTonnel		= 0x00001000; ///< Туннель
const int decSwitch		= 0x00800000; ///< Стрелка

/// Третьи степени на кривых
const int decPrjRich	= 0x00010000; ///< пржР 
const int decPrjP		= 0x00020000; ///< пржП 
const int decPrjPitR	= 0x00040000; ///< пржПр.п 
const int decPrjPitL	= 0x00080000; ///< пржПр.л 

/// Сочетания вторых степеней
const int decPrjRP		= 0x00100000; ///< пржР+П 
const int decPrjRPitR	= 0x00200000; ///< пржР+Пр.п 
const int decPrjRPitL	= 0x00400000; ///< пржР+Пр.л 

// проверка выставленности флага
inline bool IsFlag(int value, int flag)
{
	return (value & flag) == flag;
}

// проверка типа отступления: ограничения на мостах
inline bool IsTypeBridgeLimit(int type)
{
	return type == ovrLevel ||	// уровень
		type == ovrDistort ||	// перекос по уровню
		type == ovrPitR ||		// просадка правая
		type == ovrPitL ||		// просадка левая
		type == ovrRich ||		// рихтовка, рихтовочная нить
		type == ovrRichN ||		// рихтовка, нерихтовочная нить
		type == ovrRichR ||		// рихтовка правая
		type == ovrRichL;		// рихтовка левая
}

// проверка типа отступления: оптика
inline bool IsTypeOptics(int type)
{
	return type == ovrWidthPl ||	// уширение
		type == ovrWidthMn ||	// сужение
		type == ovrRich ||		// рихтовка, рихтовочная нить
		type == ovrRichN ||		// рихтовка, нерихтовочная нить
		type == ovrRichR ||		// рихтовка правая
		type == ovrRichL;		// рихтовка левая
}

/// структура отступления на диске [старая]
#pragma pack (push,1)
struct VOverstepOnDiskV0
{
	short km;		///< Путевая координата, км
	float m;		///< Путевая координата, м
    float abs;		///< Абсолютная координата
    long type;		///< тип неисправности
    float value;	///< величина отступления
    float norma;	///< норма параметра
    float len;		///< длина
    long degree;	///< степень
    long number;	///< поp. номеp отступления в последовательности
    long comb;		///< дополнительный тип неисправности
    long speedSet1;	///< установленные скоpости
    long speedSet2;	///< установленные скоpости
    long speedLim1;	///< скоpости огpаничения
    long speedLim2;	///< скоpости огpаничения
    char objectApr;	///< флаг объекта апр.данных (мосты, станции и пр.)
    bool sboy;		///< пpизнак сбоя [нигде не используется]
};
#pragma pack (pop)

/// структура отступления на диске [новая]
#pragma pack (push,1)
struct VOverstepOnDiskV1
{
	// неподвинутые координаты
	short km;		///< Путевая координата, км
	float m;		///< Путевая координата, м
	double abs;		///< Абсолютная координата

	// подвинутые координаты
	short rkm;		///< Путевая координата, км
	float rm;		///< Путевая координата, м
	double rabs;	///< Абсолютная координата

	long type;		///< тип неисправности
	long typeOrig;	///< оригинальный тип неисправности (правая или левая рихтовка при учете нити)
	long degree;		///< степень
	long number;		///< поp. номеp отступления в последовательности
	long comb;		///< дополнительный тип неисправности
	float value;	///< величина отступления
	float norma;	///< норма параметра
	float len;		///< длина

	/// установленные скоpости
	long speedSet1;	
	long speedSet2;
	long speedSet3;

	/// скоpости огpаничения
	long speedLim1;	
	long speedLim2;
	long speedLim3;
};
#pragma pack (pop)

/// структура, описывающая доп.коды отступления
struct VOvrAddCode
{
	bool C;			///< С
	bool PP;		///< РР
	bool ThreePr;	///< 3Пр
	bool PrjRich;	///< пржР
	bool PrjP;		///< пржП
	bool PrjPitR;	///< пржПр.п
	bool PrjPitL;	///< пржПр.л
	bool PrjRP;		///< пржР+П
	bool PrjRPitR;	///< пржР+Пр.л
	bool PrjRPitL;  ///< пржР+Пр.п
	bool Bridge;	///< мост
	bool Tonnel;	///< тоннель
	bool Switch;    ///< стрелка

	/// конструктор
	VOvrAddCode()
	{
		this->Init();
	}

	/// инициализация
	void Init()
	{
		ZeroMemory(this, sizeof(*this));
	}

	/// импорт из структур v0 и v1
	void Import(long type, long comb, char objectApr);

	/// экспорт в int
	int Export();
};

/// класс, описывающий отступление
class VOverstep2
{
	/// импорт из структуры версии 0
	void Import(const VOverstepOnDiskV0 & src);

	/// импорт из структуры версии 1
	void Import(const VOverstepOnDiskV1 & src);

public:

	int version;		///< версия оригинальной структуры

	VWayCoord wayCoord; ///< путевая координата подвинутая
	double absCoord;	///< абсолютная координата подвинутая

	VWayCoord wayCoordOrig; ///< путевая координата оригинальная
	double absCoordOrig;	///< абсолютная координата оригинальная

	bool isOrigCoord;   ///< Оригинальная координата

	float value;		///< величина отступления
	float norma;		///< норма, если есть
	float len;			///< длина отступления
	int numElem;		///< к-во элементарных отступлений
	int degree;			///< степень
	int number;			///< номер в последовательности

	int type;			///< тип отступления
	int typeOrig;		///< оригинальный тип неисправности (правая или левая рихтовка при учете нити)
	bool isOrigType;    ///< оригинальный тип отступления

	VOvrAddCode code;	///< доп.код отступления

	VSpeeds setSpd;		///< скорости по приказу
	VSpeeds limSpd;		///< ограничения скоростей
	bool isThirdSpeed;	///< Третья скорость
	
	/// конструктор
	VOverstep2()
	{
		this->Init();
	}

	VOverstep2(const VOverstepOnDiskV0 & ovr)
	{
		this->Import(ovr);
	}

	VOverstep2(const VOverstepOnDiskV1 & ovr)
	{
		this->Import(ovr);
	}

	/// инициализация
	void Init()
	{
		ZeroMemory(this, sizeof(*this));
	}

	/// наличие ограничения скорости
	bool IsLimit() const
	{
		return this->limSpd.GetPassSpeed() < this->setSpd.GetPassSpeed() ||
			this->limSpd.GetTruckSpeed() < this->setSpd.GetTruckSpeed() ||
			this->limSpd.GetEmptySpeed() < this->setSpd.GetEmptySpeed();
	}

	/// отступление соответствует маске
	bool MeetsMask(unsigned long __mask) const;

	/// отступление укладывается в участок
	bool MeetsSection(const VWaySection * section) const
	{
		return section == NULL || 
			(this->wayCoord >= section->start && this->wayCoord <= section->end);
	}

	/// Баллы начисляемые за отступление
	int GetPenalty() const;

	bool operator < (const VOverstep2 & src)
	{
		return this->wayCoord < src.wayCoord;
	}
};

/// список отступлений
class VOvr2List : public VIndexList<VOverstep2>
{
private:

	std::vector<VOverstepOnDiskV0> full_v0; ///< Полный массив отступлений версии 0
	std::vector<VOverstepOnDiskV1> full_v1; ///< Полный массив отступлений версии 1

	/// Оператор присваивания закрыт
	VOvr2List & operator = (const VOvr2List &);

	/// Потроха - загрузка из блока данных
	bool Load(VBasicParameter & data);

	/// Правильный тип (пропускаем весь мусор)
	bool CheckType(int type) const
	{
		return (type >= 1 && type <= 9) || (type >= 15 && type <= 17) ||
			(type >= 19 && type <= 23) || (type >= 100 && type <= 105);
	}
	
public:

	/// Шаблон - загрузка из файла
	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type);

	/// Копирование списка 
	bool CopyFrom(const VOvr2List & src, const VWaySection * section);

	/// Импорт из вектора
	bool Import(const std::vector<VOverstepOnDiskV1> & ovrs);

	/// сохранение в файл
	bool Save(VMainFile & file, uint32_t type);

	/// установка маски и корректировка списка отступлений
	bool BuildOvrList(unsigned long __mask, VWaySection * section);

	/// Выгрузка
	void Unload()
	{
		this->full_v0.clear();
		this->full_v1.clear();
		VIndexList<VOverstep2>::Unload();
	}

	/// Экспорт списка отступлений
	const std::vector<VOverstep2> & ExportList() const
	{
		return this->records;
	}

	/// Экспорт списка отступлений для указанного км
	std::vector<VOverstep2> ExportList(int32_t km) const;

	/// ПУ-32 для указанного километра
	std::string GetPU32String(int32_t km) const;

	/// Список оцененных километров, в порядке оценки
	std::set<int32_t> GetKmList() const;
};
