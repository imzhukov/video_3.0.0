#include <boost/asio.hpp>
#include "strlib.h"

#include <ctype.h>
#include <windows.h>

#include <boost/lexical_cast.hpp>


/** Возвращает разделитель для элементов списка, установленный в локали*/
std::wstring getCSVSep()
{
	wchar_t buf[2];
	::GetLocaleInfo(GetThreadLocale(),LOCALE_SLIST,buf,2);
	return std::wstring(buf);
}

std::wstring getDECIMALSep()
{
	wchar_t buf[2];
	::GetLocaleInfo(GetThreadLocale(),LOCALE_SDECIMAL,buf,2);
	return std::wstring(buf);
}

std::string wstring_to_string(const std::wstring& source)
{
	size_t len = ::WideCharToMultiByte(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), NULL, 0, NULL, NULL);
	char * dest = (char*) std::calloc(len+1, sizeof(char));
	::WideCharToMultiByte(CP_ACP, 0, source.c_str(), -1, dest, static_cast<int>(len+1), NULL, NULL);
	std::string dest_str(dest);
	std::free(dest);
	size_t len2 = dest_str.length();
	return dest_str;
}

/** Преобразование string в wstring*/
std::wstring string_to_wstring(const std::string& source)
{
	size_t len = ::MultiByteToWideChar(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), NULL, 0);
	wchar_t * dest = (wchar_t*) std::calloc(len+1, sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, source.c_str(), -1, dest, static_cast<int>(len+1));
	std::wstring dest_str(dest);
	std::free(dest);
	size_t len2 = dest_str.length();
	return dest_str;
}


/** Запись широкой строки в узкий поток */
std::ostream & operator << (std::ostream & os, const std::wstring & str)
{
	os << wstring_to_string(str);
	return os;
}

std::ostream & operator << (std::ostream & os, const wchar_t * str)
{
	os << wstring_to_string(str);
	return os;
}

double str_to_double(const std::wstring& in_string)
{
	std::wstring _in_string = in_string;
	if(_in_string.find(L",") != 0xFFFFFFFF)
		_in_string.replace(_in_string.find_first_of(L","), 1, L".", 1);
	try
	{
		return boost::lexical_cast<double>(_in_string);
	}
	catch (...) //bad_lexical_cast& e
	{
		throw STRING_PARSE_ERROR(in_string);
	}
}

double str_to_double(const std::string& in_string)
{
	try
	{
		return boost::lexical_cast<double>(in_string);
	}
	catch (...) //bad_lexical_cast& e
	{
		std::locale::global(std::locale(""));
		double result=boost::lexical_cast<double>(in_string);
		std::locale::global(std::locale("C"));
		return result;
	}
}


/// Возвращает хэш-функцию строки (алгоритм FNV32)
unsigned int FNV32(const std::wstring& in_str)
{
	static const unsigned int p=0x01000193;
	unsigned int h=0x811c9dc5;
	for (size_t i=0; i<in_str.size(); ++i)
	{
		h*=p;
		h^=static_cast<unsigned int>(in_str[i]);
	}
	return h;
}