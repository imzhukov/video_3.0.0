#pragma once

#include <string>
#include "VException.h"
#include <wchar.h>

/** ���������� ����������� ��� ��������� ������, ������������� � ������*/
std::wstring getCSVSep();

/** �������������� wstring � string*/
std::string wstring_to_string(const std::wstring& source);

/** �������������� string � wstring*/
std::wstring string_to_wstring(const std::string& source);

/** ������ ������� ������ � ����� ����� */
std::ostream & operator << (std::ostream & os, const std::wstring & str);
std::ostream & operator << (std::ostream & os, const wchar_t * str);

double str_to_double(const std::wstring& in_string);
double str_to_double(const std::string& in_string);

/// ���������� ���-������� ������ (�������� FNV32)
unsigned int FNV32(const std::wstring& in_str);

