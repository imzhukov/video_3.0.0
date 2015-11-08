#pragma once

#include "c4stdint.h"
#include <time.h>
#include <string>


/** Время совместимое с SYSTEMTIME*/
struct VTime
{
    uint16_t wYear;
    uint16_t wMonth;
    uint16_t wDayOfWeek;
    uint16_t wDay;
    uint16_t wHour;
    uint16_t wMinute;
    uint16_t wSecond;
    uint16_t wMilliseconds;

	/** Конструктор*/
	VTime();

	/** Конструктор копирования из time_t*/
	VTime(const time_t &t)
	{
		*this=t;
	};

	/** Конструктор копирования из tm*/
	VTime(const tm &t)
	{
		*this=t;
	};

	static VTime ZeroTime()
	{
		VTime t; 
		memset(&t, 0, sizeof(VTime));		
		return t;
	}

	/** Конструктор из строки*/
	VTime(const std::string & str);

	/** Оператор присваивания из time_t*/
	VTime & operator = (const time_t &t);

	/** Оператор присваивания из tm */
	VTime & operator = (const tm &t);

	/** В структуру tm */
	tm ToTM() const;

	/** в time_t */
	time_t ToTimeT() const;

	/** В строку формата ISO */
	std::string ToStringISO() const;

	/** В строку формата ISO */
	std::wstring ToWideStringISO() const;

	/** Дата в строку формата ISO */
	std::string DateToStringISO() const;

	/** Дата в строку формата ISO */
	std::wstring DateToWideStringISO() const;

	/** В строку Locale */
	std::string ToStringLocale() const;

	/** В строку Locale */
	std::wstring ToWideStringLocale() const;

	/** дата в строку Locale */
	std::string DateToStringLocale() const;

	/** дата в строку Locale */
	std::wstring DateToWideStringLocale() const;

	bool IsZero() const;
};

bool operator==(const VTime &t1,const VTime &t2);
bool operator!=(const VTime &t1,const VTime &t2);
bool operator>(const VTime &t1,const VTime &t2);
bool operator<(const VTime &t1,const VTime &t2);
bool operator <= (const VTime &t1,const VTime &t2);



