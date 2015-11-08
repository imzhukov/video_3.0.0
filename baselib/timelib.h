#pragma once

#include "c4stdint.h"
#include <time.h>
#include <string>


/** ����� ����������� � SYSTEMTIME*/
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

	/** �����������*/
	VTime();

	/** ����������� ����������� �� time_t*/
	VTime(const time_t &t)
	{
		*this=t;
	};

	/** ����������� ����������� �� tm*/
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

	/** ����������� �� ������*/
	VTime(const std::string & str);

	/** �������� ������������ �� time_t*/
	VTime & operator = (const time_t &t);

	/** �������� ������������ �� tm */
	VTime & operator = (const tm &t);

	/** � ��������� tm */
	tm ToTM() const;

	/** � time_t */
	time_t ToTimeT() const;

	/** � ������ ������� ISO */
	std::string ToStringISO() const;

	/** � ������ ������� ISO */
	std::wstring ToWideStringISO() const;

	/** ���� � ������ ������� ISO */
	std::string DateToStringISO() const;

	/** ���� � ������ ������� ISO */
	std::wstring DateToWideStringISO() const;

	/** � ������ Locale */
	std::string ToStringLocale() const;

	/** � ������ Locale */
	std::wstring ToWideStringLocale() const;

	/** ���� � ������ Locale */
	std::string DateToStringLocale() const;

	/** ���� � ������ Locale */
	std::wstring DateToWideStringLocale() const;

	bool IsZero() const;
};

bool operator==(const VTime &t1,const VTime &t2);
bool operator!=(const VTime &t1,const VTime &t2);
bool operator>(const VTime &t1,const VTime &t2);
bool operator<(const VTime &t1,const VTime &t2);
bool operator <= (const VTime &t1,const VTime &t2);



