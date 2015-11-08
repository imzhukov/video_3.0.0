#pragma once

//std
#include <string>
#include <map>

//Xerces
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

//V
#include "VException.h"
#include "dblib.h"
#include "types.h"

using namespace xercesc;
using namespace std;


/** �������*/
class VRoute
{
public:
	/** ����������� �� ���������*/
	VRoute();
	/** �����������*/
	VRoute(const wstring& in_name);
	/** �������� �������*/
	void AddSection(const VRouteSection& in_route_section);
	/** ����������� ������� ����*/
	void EditSection(const VRouteSection& in_route_section, const VRouteSection& out_route_section);
	/** ������� ������� ����*/	
	void DeleteSection(const VRouteSection& in_route_section);
	/** ������� ��� ������� ����*/
	void Clear();
	/** ���������� ���*/	
	const wstring& GetName() const;
	/** �������� ���*/
	void SetName(const wstring& in_name);	
	/** ���������� ����� ��������*/
	size_t GetSectionCount() const;
	/** ���������� ����������� ������ �� �������*/
	const VRouteSection& GetSection(size_t in_index) const;
	/** ���������� �������*/
	VRouteSection& GetSection(size_t in_index);
private:

	//������

	/** �������� ��������*/
	wstring name;
	/** ������ ��������*/
	vector<VRouteSection> route;	
};

/** ������ ���������*/
class VXMLRouteList
{
public:
	/** �����������*/
	VXMLRouteList();
	/** ����������*/
	~VXMLRouteList();
	/** ��������� �������� �� ����� XML*/
	void LoadFromFile(const wstring& in_filename);	
	/** ��������� ������� � ���� XML*/
	void SaveToFile(const wstring& in_filesdir, VRoute route, wstring in_filename=L"");
	/** ������� ������*/
	void Clear();
	/** ���������� ����� ���������*/
	size_t GetCount();
	/** ���������� ����������� ������ �� �������*/
	const VRoute& GetRoute(size_t i) const;
	/** ���������� ������ �� �������*/
	VRoute& GetRoute(size_t i);
	/** ���������� ������� �������� � ������ ��� -1, ���� �������� � ������ ���*/
	int IsNameInList(const wstring& in_name);
	/** ���������� ������� � ������*/
	void AddRoute(const wstring& in_name);
	/** ������� ������� �� ������*/
	void DeleteRoute(size_t in_index);
private:
	//������

	/** ��������*/
	vector<VRoute> routes;
	/** ���������� DOM*/
	DOMImplementation * impl;	
	/** ������*/
	boost::shared_ptr<XercesDOMParser> parser;
	/** �������� ��������*/
	xercesc::DOMDocument * source_document;	
	/** ���������� ������*/
	boost::shared_ptr<ErrorHandler> error_handler;
};