#pragma once

//std
#include <string>
#include <map>
#include <deque>
//Xerces
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
//Boost
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
//V
#include "VException.h"

using namespace xercesc;


/// ������� � XML
class VRSXMLTable
{
private:
	/// ���������� DOM
	DOMImplementation * impl;	
	/// ������
	boost::shared_ptr<XercesDOMParser> parser;
	/// �������� ��������
	xercesc::DOMDocument * source_document;	
	/// ���������� ������
	boost::shared_ptr<ErrorHandler> error_handler;


	typedef std::pair<std::wstring, std::wstring> CELL;
	typedef std::map<std::wstring, std::wstring> ROW;
	typedef std::deque<ROW> TABLE;

	/// �������
	TABLE table;

	/// �������������
	void Init();
	/// ������������ ��������
	void Terminate();
	/// �������� ���������
	void Load(const std::wstring& in_xml_file_name);

public:
	/// �����������
	VRSXMLTable(const std::wstring& in_xml_file_name);
	/// ����������
	~VRSXMLTable();		
	/// ���������� ����� �����
	size_t GetRowCount() const;
	/// ���������� ���������� ������
	const std::wstring& GetCellValue(size_t in_row, const std::wstring& in_key) const;
	/// ���������� � CSV
	void SaveAsCSV() const;
};
