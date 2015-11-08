#pragma once

#include <string>
#include "VException.h"
#include <wchar.h>

/** Возвращает разделитель для элементов списка, установленный в локали*/
std::wstring getCSVSep();

/** Преобразование wstring в string*/
std::string wstring_to_string(const std::wstring& source);

/** Преобразование string в wstring*/
std::wstring string_to_wstring(const std::string& source);

/** Запись широкой строки в узкий поток */
std::ostream & operator << (std::ostream & os, const std::wstring & str);
std::ostream & operator << (std::ostream & os, const wchar_t * str);

double str_to_double(const std::wstring& in_string);
double str_to_double(const std::string& in_string);

/// Возвращает хэш-функцию строки (алгоритм FNV32)
unsigned int FNV32(const std::wstring& in_str);

