#include "VException.h"

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

/** Форматирует короткое сообщение*/
std::wstring VException::FormatShort()
{
	return boost::str(boost::wformat(L"%s %s") % GetDescription() % GetDetails());
};

/** Форматирует длинное сообщение*/
std::wstring VException::FormatLong()
{
	return boost::str(boost::wformat(L"%s %s %s") % GetDescription() % GetDetails() % GetDebug());
};

/** Конструктор*/
VSimpleException::VSimpleException(const std::wstring& in_description, 
								   const std::wstring& in_details)
{		
	description=in_description;
	details=in_details;
	debug=L"";

}

/** Конструктор*/
VSimpleException::VSimpleException(const std::wstring& in_description, 
								   const std::wstring& in_details, 
								   const std::wstring& in_file, 
								   uint32_t in_line)
{				
	description=in_description;
	details=in_details;
	std::vector<std::wstring> fn_parts;
	boost::algorithm::split(fn_parts, boost::iterator_range<std::wstring::const_iterator>(in_file.begin(), in_file.end()), boost::is_any_of(L"\\/"));
	debug=boost::str(boost::wformat(L"%s:%d") % fn_parts.back() % in_line);
}			

/** Конструктор*/
VSimpleException::VSimpleException(const std::wstring& in_description, const std::wstring& in_details, 
		const std::wstring& in_file, uint32_t in_line, const std::wstring& in_typename)
{
	description=in_description;
	details=in_details;
	std::vector<std::wstring> fn_parts;
	boost::algorithm::split(fn_parts, boost::iterator_range<std::wstring::const_iterator>(in_file.begin(), in_file.end()), boost::is_any_of(L"\\/"));
	debug=boost::str(boost::wformat(L"%s:%d(%s)") % fn_parts.back() % in_line % in_typename);
}