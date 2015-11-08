#pragma once

#include <types.h>
#include <vector>
#include <string>
#include <mathlib.h>
#include <interval.h>

/// Типы шпал
const long SPALA_GB = 0;	/// ЖБШ (по факту, всё кроме...)
const long SPALA_GB96 = 1;	/// ЖБШ до 1996
const long SPALA_WOOD = 2;	/// Дерево
const long SPALA_DEFAULT = SPALA_WOOD; /// Величина по умолчанию

/// Объекты с охранной зоной
const long SZO_NONE = 0;	/// Ничего
const long SZO_OBJECT = 1;	/// Объект
const long SZO_SZONE = 2;	/// Охранная зона

/// Типы рельсов
const long RAIL_NONE = 0;	/// Не указано
const long RAIL_R75 = 1;	/// р75
const long RAIL_R65 = 2;	/// р65
const long RAIL_R65K = 3;	/// р65к
const long RAIL_R50 = 4;	/// р50
const long RAIL_R43 = 5;	/// р43
const long RAIL_DEFAULT = RAIL_R65;

/// Типы скреплений
const long BRACE_NONE = 0;
const long BRACE_K2 = 1;
const long BRACE_BP = 2;
const long BRACE_ZHB = 3;
const long BRACE_ZHBR = 4;
const long BRACE_ZHBR65 = 5;
const long BRACE_ZHBR65SH = 6;
const long BRACE_ARS = 7;
const long BRACE_PANDROL_350 = 8;
const long BRACE_D2 = 9;
const long BRACE_D4 = 10;
const long BRACE_KD = 11;
const long BRACE_DEFAULT = BRACE_K2;

#pragma pack (push,1)

/// Основание для записи в априорной информации - границы участка
struct VAprinfoBasicRecord
{
	double base, end; ///< границы

	VAprinfoBasicRecord() : base(0.0), end(0.0) {}

	// длина участка
	double GetLen() const
	{
		return this->end - this->base;
	}

	// фильтр для экспорта
	virtual bool Fits() const
	{
		return true;
	}
};

bool operator < (const VAprinfoBasicRecord & r1, const VAprinfoBasicRecord & r2);

/// Основания для метки - координата
struct VAprinfoBasicLabel
{
	double coord; ///< координата

	VAprinfoBasicLabel() : coord(0.0) {};
	VAprinfoBasicLabel(double in_coord) : coord(in_coord) {};

	// фильтр для экспорта
	virtual bool Fits() const
	{
		return true;
	}
};

/// Объект, обладающий признаком "да-нет"
struct VAprinfoObjectRecord : public VAprinfoBasicRecord
{
	bool isThere; ///< признак

	VAprinfoObjectRecord() : VAprinfoBasicRecord(),
		isThere(false) {}

	// фильтр для экспорта
	virtual bool Fits() const
	{
		return this->isThere;
	}
};

/// Запись с целым значением 
struct VAprinfoIndexRecord : public VAprinfoBasicRecord
{
	long value; ///< значение

	VAprinfoIndexRecord() : VAprinfoBasicRecord(), 
		value(0) {}

	// фильтр для экспорта
	virtual bool Fits() const
	{
		return this->value == SZO_OBJECT;
	}
};

/*
/// Запись с описанием участка
struct VAprinfoDescrRecord : public VAprinfoBasicRecord
{
	std::string descr;

	VAprinfoDescrRecord() : VAprinfoBasicRecord(), descr(""
	{}
};
*/

/// Оператор равенства
inline bool operator == (const VAprinfoIndexRecord & rec1, const VAprinfoIndexRecord & rec2)
{
	return rec1.value == rec2.value;
}

/// Тип рельс
struct VAprinfoRailRecord : public VAprinfoIndexRecord
{
	std::string GetDescr() const;

	VAprinfoRailRecord() : VAprinfoIndexRecord()
	{}
};

/// Тип скреплений
struct VAprinfoBraceRecord : public VAprinfoIndexRecord
{
	std::string GetDescr() const;

	VAprinfoBraceRecord() : VAprinfoIndexRecord()
	{}
};

/// Километр
struct VAprinfoKmRecord : public VAprinfoBasicRecord
{
	long km; ///< номер
	float len; ///< длина

	VAprinfoKmRecord() : VAprinfoBasicRecord(), 
		km(0), len(0) {}

	/// координата метра, по указанной абсолютной
	VWayCoord GetRealCoord(double absCoord) const;

	/// абс. координата указанного метра
	double GetAbsCoord(float realM) const;

	/// список пикетов на километре
	std::vector<VPicket> GetPkList() const;

	/// знак длины
	int GetKmDirection() const
	{
		return len > 0 ? 1 : -1;
	}
};

/// Скорости по приказу
struct VAprinfoSpeedRecord : public VAprinfoBasicRecord
{
	VSpeeds spd; ///< скорости

	VAprinfoSpeedRecord() : VAprinfoBasicRecord()
	{}
		
	/// Возвращает строковое представление
	std::wstring format_short() const
	{
		return this->spd.format_short();
	}

	VSpeeds GetSpeed() const
	{
		return this->spd;
	}
};

/// ПЧ
struct VAprinfoPchRecord : public VAprinfoBasicRecord
{
	/// ПЧ, нод, дорога
	long pchCode, nodCode, roadCode;
	std::string pchName, nodName, roadName;
	VAprinfoPchRecord() : VAprinfoBasicRecord(), 
		pchCode(0), nodCode(0), roadCode(0) {}
};

/// Оператор равенства
inline bool operator == (const VAprinfoPchRecord & rec1, const VAprinfoPchRecord & rec2)
{
	return rec1.pchCode == rec2.pchCode && 
		rec1.nodCode == rec2.nodCode && 
		rec1.roadName == rec2.roadName;
}

/// метка на участке бесстыкового пути
struct VAprinfoWobLabel : VAprinfoBasicLabel
{
	long fixTemp, minTemp, maxTemp;	///< темп. закрепления и допустимые
	long startShift; ///< ????

	VAprinfoWobLabel() : VAprinfoBasicLabel(), 
		fixTemp(0), minTemp(0), maxTemp(0), startShift(0) {}
};

/// плети
struct VAprinfoWobRecord : public VAprinfoObjectRecord
{
	std::string num;	///< номер
	std::vector<VAprinfoWobLabel> labels; ///< метки

	VAprinfoWobRecord() : VAprinfoObjectRecord()
	{}
};

/// участки пути - перегоны и раздельные пункты
struct VAprinfoPntRecord : public VAprinfoObjectRecord
{
	std::string name, nameReverse;
	double axis;
	long code;

	VAprinfoPntRecord() : VAprinfoObjectRecord(), axis(0.0), code(0) {}
};

/// Направление стрелки
enum SWITCH_DIR
{
	SDIR_UNDEF = 0, SDIR_ALONG = 1, SDIR_COUNTER = -1
};

/// стрелки
struct VAprinfoSwitchRecord : public VAprinfoObjectRecord
{
	SWITCH_DIR dir;		
	long side;
	double o, x;
	std::string num, cross_mark;

	VAprinfoSwitchRecord() : VAprinfoObjectRecord(), dir(SDIR_UNDEF), side(0), o(0), x(0)
	{}

	/// Возвращает координату остряка
	double GetCoordO() const
	{
		return this->o;
	};
	/// Возвращает координату крестовины
	double GetCoordX() const
	{
		return this->x;
	};

	/// Корректирует координату стрелки
	void CorrectCoord(double in_correction)
	{		
		this->o+=in_correction;
		this->x+=in_correction;
		this->base+=in_correction;
		this->end+=in_correction;
	};

	/// Возвращает зону действия стрелки
	pro::interval<double> GetRange() const
	{
		// Зона действия может вычисляться "с запасом"
		return pro::interval<double>(std::min<double>(this->base, this->end), std::max<double>(this->base, this->end));
	};
};

/// метка на кривой
struct VAprinfoCrvLabel : public VAprinfoBasicLabel
{
	float value; ///< значение параметра в точке

	VAprinfoCrvLabel() : VAprinfoBasicLabel(), value(0) {};
	VAprinfoCrvLabel(double in_coord, float in_value) : VAprinfoBasicLabel(in_coord), value(in_value) {};	

	bool operator < (const VAprinfoCrvLabel & other)
	{
		return this->coord < other.coord;
	}
};

/// элементарная кривая
struct VAprinfoCrvRadius
{
	double radius, level, width, wear; ///< радиус, нормы возвышения и ШК, износ
	double latStart, latEnd, longStart, longEnd; ///< широта и долгота

	VAprinfoCrvRadius() : width(1520), level(0), radius(0), wear(0),
		longStart(0), longEnd(0), latStart(0), latEnd(0)
	{}
};

/// кривая
struct VAprinfoCurve : public VAprinfoObjectRecord
{
	long orientation;	///< ориентация
	double angle; ///< угол
	double latStart, latEnd, longStart, longEnd; ///< широта и долгота
	__int64 id; ///< id из базы
	double levelBase, levelEnd; ///< границы кривой на уровне

	std::vector<VAprinfoCrvLabel> planLabel, levelLabel; ///< метки на плане и уровне
	std::vector<VAprinfoCrvRadius> radius; ///< параметры эл. кривых

	VAprinfoCurve() : VAprinfoObjectRecord(), orientation(0), angle(0),
		longStart(0), longEnd(0), latStart(0), latEnd(0), id(-1),
		levelBase(0), levelEnd(0)
	{}

	void Clear();

	std::wstring Validate() const;
};

/// Тип участка
enum ZLINE_TYPE
{
	ZLINE_UNDEFINED,			///< не определено
	ZLINE_STRAIGHT,				///< прямая
	ZLINE_OUTER_TRANSITION,		///< внешняя переходная
	ZLINE_RADIUS,				///< круговая
	ZLINE_INNER_TRANSITION		///< внутренняя переходная
};

/// Нулевые линии
struct VAprinfoZLRecord : public VAprinfoBasicRecord
{
	/// Тип участка
	ZLINE_TYPE zlType;	

	/// Значение величины параметра на границах участка
	double value_base, value_end; 

	/// Пересчет высоты при "подвижке" границы
	double Shift(double coord, bool beginning) const;
};

/// Консолидированные нормы ШК+Износ
struct VAprinfoWidthRecord : public VAprinfoBasicRecord
{
	/// Тип участка
	ZLINE_TYPE zlType;	

	/// Радиус
	float Radius;
		
	/// Нормы шк и возвышения на границах участка
	float width_begin, width_end, wear_begin, wear_end;

	float GetWidthMax() const
	{
		return std::max<float>(this->width_begin, this->width_end);
	}

	float GetWidthMin() const
	{
		return std::min<float>(this->width_begin, this->width_end);
	}

	float GetWidthCalc(double coord) const
	{		
		return line_t(point_t(this->base, this->width_begin), point_t(this->end, this->width_end)).Y(coord);
	}

	float GetWearCalc(double coord) const
	{		
		return line_t(point_t(this->base, this->wear_begin), point_t(this->end, this->wear_end)).Y(coord);
	}
};

/// метки профиля
struct VAprinfoProfLabel : public VAprinfoBasicLabel
{
	float value; ///< значение
	VAprinfoProfLabel() : VAprinfoBasicLabel(), value(0) {}
};

// Реперные точки
struct VAprinfoCoordPointRecord : public VAprinfoBasicRecord
{
	std::wstring id;
	double apr_abscoord;
	short int type;
};

// Контрольные участки
struct VAprinfoControlAreaRecord : public VAprinfoObjectRecord
{
	float mo_width, sko_width, mo_level, sko_level;
	VAprinfoControlAreaRecord() : VAprinfoObjectRecord(), mo_width(0), sko_width(0),
		mo_level(0), sko_level(0) {}
};

// Ремонты
struct VAprinfoRepairRecord : public VAprinfoObjectRecord
{
	VTime start_date, end_date;
	long type_id;
	std::wstring type_descr;
	VAprinfoRepairRecord() : VAprinfoObjectRecord(), type_id(0) {}
};

// Переезды
struct VAprinfoCrossing : public VAprinfoBasicLabel
{
	std::wstring name;
	long side;
	VAprinfoCrossing() : name(L""), side(0) {}
};

// Типы машин
enum MACHINE_TYPE
{
	MACHINE_NONE = 0,
	MACHINE_VPO = 1,
	MACHINE_VRO_OR_VPR = 2,
	MACHINE_VPO_PLUS_VPR = 3,
	MACHINE_VPO_PLUS_VPR_PLUS_DSP = 4
};

std::string GetMachineTypeDescription(MACHINE_TYPE mach);

// Участки обкатки пути
struct VAprinfoTestrunRecord : public VAprinfoObjectRecord
{
	MACHINE_TYPE machine;
	VTime date_start, date_end;
	VSpeeds spd;

	std::string GetDescr() const;
	
	VAprinfoTestrunRecord() : VAprinfoObjectRecord(), machine(MACHINE_NONE)
	{}
};

#pragma pack (pop)