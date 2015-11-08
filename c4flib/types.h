#pragma once

#include <stdlib.h>
#include <time.h>
#include "c4stdint.h"

#include <os.h>
#include "strlib.h"
#include "timelib.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>

using namespace boost;

/** Неверный размер*/
static const size_t BAD_SIZE=-1;
/** Неверный индекс*/
static const size_t BAD_INDEX=-1;

/** Направление движения*/
enum direction_t
{
	DR_UNDEFINED=0,//Не определена
	DR_BACKWARD=-1,//На нарастание пикетажа
	DR_FORWARD=1//На убывание пикетажа
};

/** Сторона*/
enum side_t
{
	SD_UNDEFINED=0,//Не определена
	SD_LEFT=-1,//Левая
	SD_RIGHT=1//Правая
};

/** Инвертирует сторону*/
static side_t other_side(side_t in_side)
{
	return static_cast<side_t>(-1*static_cast<int>(in_side));
};

/** Преобразует side_t в текст*/
static std::wstring format_side_short(side_t in_side)
{	
	switch (in_side)
	{
		case SD_LEFT:
			return L"лев.";
		case SD_RIGHT:
			return L"прав.";
		case SD_UNDEFINED:
		default:
			return L"неопр.";
			
	}
}

#pragma pack(push,1)


/** Координата*/
struct VWayCoord
{
	/** Километр*/
	int32_t km;
	/** Метр*/
	float m;

	/** Оператор >*/
	bool operator>(const VWayCoord &wc2) const throw()
	{
		return ((km > wc2.km) || ((km == wc2.km) && (m > wc2.m)));
	};
	/** Оператор !=*/
	bool operator!=(const VWayCoord &wc2) const throw()
	{
		return ((km != wc2.km) || (m != wc2.m));
	};
	/** Оператор ==*/
	bool operator==(const VWayCoord &wc2) const throw()
	{
		return ((km == wc2.km) && (m == wc2.m));
	};
	/** Оператор <=*/
	bool operator<=(const VWayCoord &wc2) const throw()
	{
		return ((km < wc2.km) || ((km == wc2.km) && (m <= wc2.m)));
	};
	/** Оператор >=*/
	bool operator>=(const VWayCoord &wc2) const throw()
	{
		return ((km > wc2.km) || ((km == wc2.km) && (m >= wc2.m)));
	};
	/** Оператор <*/
	bool operator < (const VWayCoord & wc2) const throw()
	{
		return ((km < wc2.km) || ((km == wc2.km) && (m < wc2.m)));
	}

	/** Конструктор без параметров*/
	VWayCoord() : km(0), m(0) {	};
	/** Конструктор с параметрами*/
	VWayCoord(long lKm, float fM) : km(lKm), m(fM) { };
	
	
	std::wstring format_short() const {return boost::str(boost::wformat(L"%d:%0.0f") % km % m);};
	std::wstring format_long() const {return boost::str(boost::wformat(L"%d км %0.0f м") % km % m);};

};

/** Участок пути*/
struct VWaySection
{
	/** Размер структуры*/
	uint32_t dwSize;
	/** Координата начала*/
	VWayCoord start;
	/** Координата конца*/
	VWayCoord end;
	/** Оператор !=*/
	bool operator!=(const VWaySection &ws) const throw()
	{
		return ((start != ws.start) || (end != ws.end));
	}
	/** Оператор ==*/
	bool operator==(const VWaySection &ws) const throw()
	{
		return ((start == ws.start) && (end == ws.end));
	}
	/** Проверяет валидность участка, возвращает true, если координата начала меньше либо равна координате конца участка*/
	bool isValid()
	{
		return (start<=end) && !(start.km==0 && start.m==0 && end.km==0 && end.m==0);
	}

	/** Конструктор*/
	VWaySection() : dwSize(sizeof(VWaySection)), start(0,0), end(0,0)
	{};

	/** Конструктор*/
	VWaySection(const VWayCoord& in_start, const VWayCoord& in_end) : dwSize(sizeof(VWaySection)), 
		start(in_start), end(in_end)
	{};

	/** Конструктор*/
	VWaySection(int32_t in_start_km, float in_start_m, 
		int32_t in_end_km, float in_end_m) : dwSize(sizeof(VWaySection)), 
		start(in_start_km, in_start_m), end(in_end_km, in_end_m)
	{};

	/// Секции пересекаются
	bool Crossing(const VWaySection & src)
	{
		if (src.end <= this->start || src.start >= this->end) return false;
		else return true;
	}

	std::wstring format_long() const 
	{ 
		return this->start.format_long() + L" - " + this->end.format_long();
	}
};

const int DEFAULT_SPD_PASS(140);
const int DEFAULT_SPD_TRUCK(90);
const int DEFAULT_SPD_EMPTY(90);
const int NO_SPEED(-1);

/// Одиночная скорость
class speed_t
{
	int spd; // скорость

public:

	speed_t();
	speed_t(int in_spd);
	speed_t(const char * in_str);
	speed_t(const std::string & in_str);
	speed_t(const wchar_t * in_str);
	speed_t(const std::wstring & in_str);

	speed_t & operator = (int in_spd);
	speed_t & operator = (const char * in_str);
	speed_t & operator = (const std::string & in_str);
	speed_t & operator = (const wchar_t * in_str);
	speed_t & operator = (const std::wstring & in_str);

	operator int() const;

	bool Valid() const;

	bool operator < (const speed_t & in_spd) const;
	bool operator < (int in_spd) const;

	bool operator > (const speed_t & in_spd) const;
	bool operator > (int in_spd) const;

	bool operator != (const speed_t & in_spd) const;
	bool operator != (int in_spd) const;

	bool Decrease(int in_step);

	std::wstring format() const;
};

namespace std
{
	template <> const speed_t & max<speed_t>(const speed_t & s1, const speed_t & s2);
};

/// Скорости
class VSpeeds
{
	speed_t spd_pass, spd_truck, spd_empty; ///< пассажирская, грузовая, порожняк
	speed_t spd_hi, spd_hi_bt; ///< высокая

public:

	VSpeeds();

	VSpeeds(
		const speed_t & in_spd_pass, 
		const speed_t & in_spd_truck, 
		const speed_t & in_spd_empty);

	VSpeeds(
		const speed_t & in_spd_pass, 
		const speed_t & in_spd_truck, 
		const speed_t & in_spd_empty, 
		const speed_t & in_spd_hi, 
		const speed_t & in_spd_hi_bt);

	VSpeeds(const char * in_spd);
	VSpeeds(const std::string & in_spd);
	VSpeeds(const wchar_t * in_spd);
	VSpeeds(const std::wstring & in_spd);

	VSpeeds & operator = (const char * in_spd);
	VSpeeds & operator = (const std::string & in_spd);
	VSpeeds & operator = (const wchar_t * in_spd);
	VSpeeds & operator = (const std::wstring & in_spd);

	/// Возвращает строковое представление
	std::wstring format_short() const;	

	///Возвращает true если скорости валидны
	bool Valid() const;

	/// Возвращает пасс. скорость
	const speed_t & GetPassSpeed() const;

	/// Возвращает груз. скорость
	const speed_t & GetTruckSpeed() const;

	/// Возвращает скорость ПРЖ
	const speed_t & GetEmptySpeed() const;

	/// Возвращает скорость HS
	const speed_t & GetHighSpeed() const;

	/// Возвращает скорость HS-BT
	const speed_t & GetHighBTSpeed() const;

	/// Возвращает максимальную скорость
	const speed_t & GetMaxSpeed() const;

	/// Устанавливает пасс. скорость
	void SetPassSpeed(const speed_t & spd);

	/// Устанавливает груз. скорость
	void SetTruckSpeed(const speed_t & spd);

	/// Устанавливает скорость ПРЖ
	void SetEmptySpeed(const speed_t & spd);

	/// Возвращает скорость HS
	void SetHighSpeed(const speed_t & spd);

	/// Возвращает скорость HS-BT
	void SetHighBTSpeed(const speed_t & spd);

	/// Ограничивает скорость
	bool RestrictSpeed(const VSpeeds & in_restriction);

	/// Ограничивает скорость
	bool RestrictMaxSpeed(const speed_t & in_speed);

	/// Ограничивает скорость
	bool RestrictEmptySpeed(const speed_t & in_speed);

	/// Ограничивает скорость
	void DecreaseSpeed(int in_step);

	/// Снижает высокую скорость (накл.кузова)
	void DecreaseHighBTSpeed(int in_step);

	/// Снижает высокую скорость
	void DecreaseHighSpeed(int in_step);

	/// Снижает пассажирскую скорость
	void DecreasePassSpeed(int in_step);

	/// Снижает грузовую скорость 
	void DecreaseTruckSpeed(int in_step);

	/// Снижает скорость порожняка
	void DecreaseEmptySpeed(int in_step);

	/// Оператор !=
	bool operator != (const VSpeeds & in_speeds) const;

	/// Оператор <
	bool operator < (const VSpeeds & in_speeds) const;

	/// Комбинирует скорости выбирая минимальную
	static VSpeeds CombineSpeeds(const VSpeeds & in_l, const VSpeeds & in_r);
	static VSpeeds CombineSpeeds(const VSpeeds & in_1, const VSpeeds & in_2, const VSpeeds & in_3);
	/// Возвращает только ограничение
	static VSpeeds Restriction(const VSpeeds & in_speed, const VSpeeds & in_restrinction);
};

/// Пикет
struct VPicket
{
	uint32_t size;			///< размер структуры
	long km;				///< номер км
	long pk;				///< номер пикета
	float len;				///< длина пикета
	VWaySection section;	///< путевые координаты границ пикета
	double absStart, absEnd;///< абсолютные координаты границ пикета
};

#pragma pack(pop)

/** FixMe!!! доработать с учетом станций*/
/** Участок маршрута*/
struct VRouteSection
{
private:
	/// Код направления
	long dirCode;
	/// Имя пути
	std::string wayName;
	/// Координаты
	VWaySection section;
public:
	/// Конструктор
	VRouteSection() : dirCode(0), wayName("") {};
	/// Конструктор
	VRouteSection(long in_code, const std::string& in_way, const VWaySection& in_section) :
	dirCode(in_code), wayName(in_way), section(in_section){};
	/// Конструктор
	VRouteSection(const VRouteSection& in_source)
	{
		*this=in_source;
	};
	/// Возвращает код направления
	long GetCode() const {return dirCode;};
	long& GetCode() {return dirCode;};
	/// Возвращает путь
	const std::string& GetWay() const {return wayName;};
	std::string& GetWay() {return wayName;};
	/// Возвращает координаты
	const VWaySection& GetSection() const {return section;};
	VWaySection& GetSection() {return section;};
	/// Оператор=
	VRouteSection& operator=(const VRouteSection& in_source);
	/// Оператор==
	bool operator==(const VRouteSection& in_right) const;
};
