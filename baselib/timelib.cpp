#include <boost/asio.hpp>
#include "timelib.h"

#include "strlib.h"
#include "mathlib.h"
#include "windows.h"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>


#include "VException.h"

/** �����������*/
VTime::VTime()
{
	memset(this, 0, sizeof(VTime));

	//�� ��������� ������� ����� FixMe!!! ����� ����� ����������� �������?
	time_t t;
	tm ltm;
	time(&t);
	if (localtime_s(&ltm, &t) == 0)
	{
		wYear=ltm.tm_year+1900;
		wMonth=ltm.tm_mon+1;
		wDayOfWeek=ltm.tm_wday;
		wDay=ltm.tm_mday;
		wHour=ltm.tm_hour;
		wMinute=ltm.tm_min;
		wSecond=ltm.tm_sec;
		wMilliseconds=0;//�� �����������
	}
};

/** ����������� �� ������*/
VTime::VTime(const std::string & str)
{	
	std::vector<std::string> splitVec, dateVec, timeVec;
	split(splitVec, str, boost::is_any_of(" "));
	std::string date= splitVec.at(0);
	//FixMe!!! ����� ���������� ������ ���� ��������� ��������: ������,ISO
	if (splitVec.at(0).find(".")!=std::string::npos)
	{
		split(dateVec,splitVec.at(0), boost::is_any_of("."));
		this->wDay=atoi(dateVec.at(0).c_str());
		this->wMonth=atoi(dateVec.at(1).c_str());
		this->wYear=atoi(dateVec.at(2).c_str());
	}
	else if (splitVec.at(0).find("-")!=std::string::npos)
	{
		split(dateVec,splitVec.at(0), boost::is_any_of("-"));
		this->wYear=atoi(dateVec.at(0).c_str());		
		this->wMonth=atoi(dateVec.at(1).c_str());
		this->wDay=atoi(dateVec.at(2).c_str());		
	}
	else
	{
		throw VSimpleException(L"����������� ������ ����.", string_to_wstring(splitVec.at(0)), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (splitVec.size()>1)
	{
		std::string time= splitVec.at(1);
		split(timeVec,splitVec.at(1),boost::is_any_of(":"));
		if (timeVec.size()>0)
			this->wHour=atoi(timeVec.at(0).c_str());
		else
			this->wHour=0;
		if (timeVec.size()>1)
			this->wMinute=atoi(timeVec.at(1).c_str());
		else
			this->wMinute=0;
		if (timeVec.size()>2)
			this->wSecond=atoi(timeVec.at(2).c_str());
		else
			this->wSecond=0;
		if (timeVec.size()>3)
			this->wMilliseconds=atoi(timeVec.at(3).c_str());
		else
			this->wMilliseconds=0;
	}
};

/** �������� ������������ �� time_t*/
VTime & VTime::operator = (const time_t &t)
{
	tm ltm = {0};
	if (localtime_s(&ltm, &t) == 0)
	{
		*this = ltm;
	}
	else
	{
		memset(this, 0, sizeof(VTime));
	}
	return *this;
};

/** �������� ������������ �� tm */
VTime & VTime::operator = (const tm &t)
{
	this->wYear = t.tm_year + 1900;
	this->wMonth = t.tm_mon + 1;
	this->wDayOfWeek = t.tm_wday;
	this->wDay = t.tm_mday;
	this->wHour = t.tm_hour;
	this->wMinute = t.tm_min;
	this->wSecond = t.tm_sec;
	this->wMilliseconds = 0;
	return *this;
}

/** � ��������� tm */
tm VTime::ToTM() const
{
	struct tm __tm;
	__tm.tm_year = this->wYear - 1900;
	__tm.tm_mon = this->wMonth - 1;
	__tm.tm_wday = this->wDayOfWeek;
	__tm.tm_mday = this->wDay;
	__tm.tm_hour = this->wHour;
	__tm.tm_min = this->wMinute;
	__tm.tm_sec = this->wSecond;
	return __tm;
}

/** � time_t */
time_t VTime::ToTimeT() const
{
	struct tm t = this->ToTM();
	return mktime(&t);
}

/** � ������ ������� ISO */
std::string VTime::ToStringISO() const
{
	return boost::str(boost::format("%04i-%02i-%02i %02i:%02i:%02i") 
		% this->wYear % this->wMonth % this->wDay 
		% this->wHour % this->wMinute % this->wSecond);
}

/** � ������ ������� ISO */
std::wstring VTime::ToWideStringISO() const
{
	return boost::str(boost::wformat(L"%04i-%02i-%02i %02i:%02i:%02i") 
		% this->wYear % this->wMonth % this->wDay 
		% this->wHour % this->wMinute % this->wSecond);
}

/** ���� � ������ ������� ISO */
std::string VTime::DateToStringISO() const
{
	return boost::str(boost::format("%04i-%02i-%02i") 
		% this->wYear % this->wMonth % this->wDay);
}

/** ���� � ������ ������� ISO */
std::wstring VTime::DateToWideStringISO() const
{
	return boost::str(boost::wformat(L"%04i-%02i-%02i") 
		% this->wYear % this->wMonth % this->wDay);
}

/** � ������ Locale */
std::string VTime::ToStringLocale() const
{
	return boost::str(boost::format("%02i.%02i.%04i %02i:%02i:%02i") 
		% this->wDay % this->wMonth % this->wYear 
		% this->wHour % this->wMinute % this->wSecond);
}

/** � ������ Locale */
std::wstring VTime::ToWideStringLocale() const
{
	return string_to_wstring(this->ToStringLocale());
}

/** ���� � ������ Locale */
std::string VTime::DateToStringLocale() const
{
	return boost::str(boost::format("%02i.%02i.%04i") % this->wDay % this->wMonth % this->wYear);
}

/** ���� � ������ Locale */
std::wstring VTime::DateToWideStringLocale() const
{
	return string_to_wstring(this->DateToStringLocale());
}

bool VTime::IsZero() const
{
	// nb! ��� ���� - ��������� ��� ������������� ������� VTime::ZeroTime()
	// wYear == 1900 && wMonth == 1 - ��������� ��� ������������� �� struct tm
	// � �������� ��� ������ NULL-������ �� ����
	return ((this->wYear == 0 && this->wMonth == 0) || (this->wYear == 1900 && this->wMonth == 1)) && 
		this->wDayOfWeek == 0 && this->wDay == 0 && this->wHour == 0 && this->wMinute == 0 && 
		this->wSecond == 0 && this->wMilliseconds == 0;
}

bool operator==(const VTime &t1,const VTime &t2)
{
	if ((t1.wYear==t2.wYear)&&(t1.wMonth==t2.wMonth)&&(t1.wDay==t2.wDay)&&(t1.wHour==t2.wHour)&&(t1.wMinute==t2.wMinute))
		return true;
	else return false;
}

bool operator!=(const VTime &t1,const VTime &t2)
{
	return !(t1 == t2);
}

bool operator>(const VTime &t1,const VTime &t2)
{
	if (t1.wYear>t2.wYear) return true;
	if (t1.wYear<t2.wYear) return false;
	if (t1.wMonth>t2.wMonth) return true;
	if (t1.wMonth<t2.wMonth) return false;
	if (t1.wDay>t2.wDay) return true;
	if (t1.wDay<t2.wDay) return false;
	if (t1.wHour>t2.wHour) return true;
	if (t1.wHour<t2.wHour) return false;
	if (t1.wMinute>t2.wMinute) return true;
	if (t1.wMinute<t2.wMinute) return false;
	if (t1.wSecond>t2.wSecond) return true;
	if (t1.wSecond<t2.wSecond) return false;
	return false;
}

bool operator<(const VTime &t1,const VTime &t2)
{
	if (t1.wYear>t2.wYear) return false;
	if (t1.wYear<t2.wYear) return true;
	if (t1.wMonth>t2.wMonth) return false;
	if (t1.wMonth<t2.wMonth) return true;
	if (t1.wDay>t2.wDay) return false;
	if (t1.wDay<t2.wDay) return true;
	if (t1.wHour>t2.wHour) return false;
	if (t1.wHour<t2.wHour) return true;
	if (t1.wMinute>t2.wMinute) return false;
	if (t1.wMinute<t2.wMinute) return true;
	if (t1.wSecond>t2.wSecond) return false;
	if (t1.wSecond<t2.wSecond) return true;
	return false;
}

bool operator <= (const VTime &t1,const VTime &t2)
{
	return t1 < t2 || t1 == t2;
}