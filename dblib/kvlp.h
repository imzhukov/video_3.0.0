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


/// Таблица в XML
class VRSXMLTable
{
private:
	/// Реализация DOM
	DOMImplementation * impl;	
	/// Парсер
	boost::shared_ptr<XercesDOMParser> parser;
	/// Исходный документ
	xercesc::DOMDocument * source_document;	
	/// Обработчик ошибок
	boost::shared_ptr<ErrorHandler> error_handler;


	typedef std::pair<std::wstring, std::wstring> CELL;
	typedef std::map<std::wstring, std::wstring> ROW;
	typedef std::deque<ROW> TABLE;

	/// Таблица
	TABLE table;

	/// Инициализация
	void Init();
	/// Освобождение ресурсов
	void Terminate();
	/// Загрузка документа
	void Load(const std::wstring& in_xml_file_name);

public:
	/// Конструктор
	VRSXMLTable(const std::wstring& in_xml_file_name);
	/// Деструктор
	~VRSXMLTable();		
	/// Возвращает число строк
	size_t GetRowCount() const;
	/// Возвращает содержимое ячейки
	const std::wstring& GetCellValue(size_t in_row, const std::wstring& in_key) const;
	/// Сохранение в CSV
	void SaveAsCSV() const;
};
