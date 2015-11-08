#pragma once

#include "os.h"
#include <string>

/** ������� ����� ����������
	��� ���������� ������ ���� ������������ �� ����.
*/
class VException
{
protected:	
	/** ��������*/
	std::wstring description;	
	/** �����������*/
	std::wstring details;
	/** ���������� ����������*/
	std::wstring debug;
public:			
	/** ����������*/
	virtual ~VException(){};	
	/** ���������� �������� ������. � ����������� ���������� ��������� ���� �����*/
	virtual const std::wstring& GetDescription() const {return description;};
	/** ���������� �������� ������*/
	virtual const std::wstring& GetDetails() const {return details;};	
	/** ���������� ���������� ����������*/
	virtual const std::wstring& GetDebug() const {return debug;};
	/** ����������� �������� ���������*/
	virtual std::wstring FormatShort();
	/** ����������� ������� ���������*/
	virtual std::wstring FormatLong();
};


/** ����������*/
class VSimpleException : public VException
{
public:

	/** �����������*/
	VSimpleException(const std::wstring& in_description, const std::wstring& in_details);

	/** �����������*/
	VSimpleException(const std::wstring& in_description, const std::wstring& in_details, 
		const std::wstring& in_file, uint32_t in_line);
		
	/** �����������*/
	VSimpleException(const std::wstring& in_description, const std::wstring& in_details, 
		const std::wstring& in_file, uint32_t in_line, const std::wstring& in_typename);
};

#define NOT_SUPPORTED VSimpleException(L"����������� �� ��������������", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__)
#define NULL_POINTER VSimpleException(L"�������������������� ���������", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__)
#define OUT_OF_BOUNDS VSimpleException(L"������ ������� �� ������� ���������", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__)
#define ITEM_NOT_FOUND VSimpleException(L"������ �� ������", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__)
#define STRING_PARSE_ERROR(in_str) VSimpleException(L"������ ������� ������", in_str, L"strlib.cpp", __LINE__)
#define DIVISION_BY_ZERO VSimpleException(L"������� �� ����", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__)
#define STATEMENT_TOO_LONG VSimpleException(L"��������� ����� �������", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__)

/** ���������� �������������� �������������*/
class VUserException
{
private:
	/** ��������*/
	std::wstring description;
public:
	/** �����������*/
	VUserException(const std::wstring& in_description) : description(in_description){}	
	/** ���������� �������� ������*/
	const std::wstring& GetDescription() const
	{
		return description;
	}	
};


