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


/** Маршрут*/
class VRoute
{
public:
	/** Конструктор по умолчанию*/
	VRoute();
	/** Конструктор*/
	VRoute(const wstring& in_name);
	/** Добавить участок*/
	void AddSection(const VRouteSection& in_route_section);
	/** Редактирует участок пути*/
	void EditSection(const VRouteSection& in_route_section, const VRouteSection& out_route_section);
	/** Удаляет участок пути*/	
	void DeleteSection(const VRouteSection& in_route_section);
	/** Удаляет все участки пути*/
	void Clear();
	/** Возвращает имя*/	
	const wstring& GetName() const;
	/** Изменяет имя*/
	void SetName(const wstring& in_name);	
	/** Возвращает число участков*/
	size_t GetSectionCount() const;
	/** Возвращает константную ссылку на участок*/
	const VRouteSection& GetSection(size_t in_index) const;
	/** Возвращает участок*/
	VRouteSection& GetSection(size_t in_index);
private:

	//Данные

	/** Название маршрута*/
	wstring name;
	/** Список участков*/
	vector<VRouteSection> route;	
};

/** Список маршрутов*/
class VXMLRouteList
{
public:
	/** Конструктор*/
	VXMLRouteList();
	/** Деструктор*/
	~VXMLRouteList();
	/** Добавляет маршруты из файла XML*/
	void LoadFromFile(const wstring& in_filename);	
	/** Сохраняет маршрут в файл XML*/
	void SaveToFile(const wstring& in_filesdir, VRoute route, wstring in_filename=L"");
	/** Очищает список*/
	void Clear();
	/** Возвращает число маршрутов*/
	size_t GetCount();
	/** Возвращает константную ссылку на маршрут*/
	const VRoute& GetRoute(size_t i) const;
	/** Возвращает ссылку на маршрут*/
	VRoute& GetRoute(size_t i);
	/** Возвращает позицию маршрута в списке или -1, если маршрута в списке нет*/
	int IsNameInList(const wstring& in_name);
	/** Добавлеяет маршрут в список*/
	void AddRoute(const wstring& in_name);
	/** Удаляет маршрут из списка*/
	void DeleteRoute(size_t in_index);
private:
	//Данные

	/** Маршруты*/
	vector<VRoute> routes;
	/** Реализация DOM*/
	DOMImplementation * impl;	
	/** Парсер*/
	boost::shared_ptr<XercesDOMParser> parser;
	/** Исходный документ*/
	xercesc::DOMDocument * source_document;	
	/** Обработчик ошибок*/
	boost::shared_ptr<ErrorHandler> error_handler;
};