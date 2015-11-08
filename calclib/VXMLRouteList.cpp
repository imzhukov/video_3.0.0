#include "strlib.h"
#include "VXMLRouteList.h"

//--------------------------------------- Методы класса VRoute ---------------------------------------

/** Сравнение участков*/
bool section_comp(const VRouteSection& r, const VRouteSection& l)
{
	return (r.GetCode()<l.GetCode()) ||
		(r.GetCode()==l.GetCode()) && (r.GetWay()<l.GetWay()) ||
		(r.GetCode()==l.GetCode()) && (r.GetWay()==l.GetWay()) && r.GetSection().start<l.GetSection().start;
}

/** Конструктор по умолчанию*/
VRoute::VRoute() {};

/** Конструктор*/
VRoute::VRoute(const wstring& in_name) : name(in_name){};

/** Добавить участок*/
void VRoute::AddSection(const VRouteSection& in_route_section)
{
	route.push_back(in_route_section);
	//Сортируем участки
	sort(route.begin(), route.end(), section_comp);
}

/** Редактирует участок пути*/
void VRoute::EditSection(const VRouteSection& in_route_section, const VRouteSection& out_route_section)
{
	for (size_t i=0; i<route.size(); i++)
		if (section_comp(route.at(i),in_route_section)==0) 
		{
			route.at(i)=out_route_section;
			return;
		}		
}

/** Удаляет участок пути*/
void VRoute::DeleteSection(const VRouteSection& in_route_section)
{
	vector<VRouteSection>::iterator p=route.begin();
	for (size_t i=0; i<route.size(); i++)
		if (section_comp(route.at(i),in_route_section)==0) 
		{
			route.erase(p);
			return;
		}
		else p++;
}

/** Удаляет все участки пути*/
void VRoute::Clear()
{
	route.clear();
}

/** Возвращает имя*/
const wstring& VRoute::GetName() const
{
	return name;
}

/** Изменяет имя*/
void VRoute::SetName(const wstring& in_name)
{
	name=in_name;
}

/** Возвращает число участков*/
size_t VRoute::GetSectionCount() const
{
	return route.size();
}

/** Возвращает константную ссылку на участок*/
const VRouteSection& VRoute::GetSection(size_t in_index) const
{
	return route.at(in_index);
}

/** Возвращает участок*/
VRouteSection& VRoute::GetSection(size_t in_index)
{
	return route.at(in_index);
}


//------------------------------------- Методы класса VXMLRouteList ----------------------------------------


/** Конструктор*/
VXMLRouteList::VXMLRouteList()
{
	//Инициализируем Xercesc
	xercesc::XMLPlatformUtils::Initialize();
	//Получаем указатель на реализацию DOM
	impl = DOMImplementationRegistry::getDOMImplementation(L"LS");
}

/** Деструктор*/
VXMLRouteList::~VXMLRouteList()
{	
	xercesc::XMLPlatformUtils::Terminate();	
}

/** Добавляет маршруты из файла XML*/
void VXMLRouteList::LoadFromFile(const wstring& in_filename)
{
	//Создаем парсер						
	parser = boost::shared_ptr<XercesDOMParser>(new XercesDOMParser);									
	parser->setValidationScheme(XercesDOMParser::Val_Always);						
	//Создаем обработчик ошибок						 
	error_handler = boost::shared_ptr<ErrorHandler>((ErrorHandler*) new HandlerBase());
	parser->setErrorHandler(error_handler.get());

	//Парсим документ
	parser->reset();
	//FixMe!!! Нужно корректно обработать ситуацию, когда файл не существует
	parser->parse(in_filename.c_str());
	xercesc::DOMDocument * source_document=parser->getDocument();
	//Получаем корневой узел
	DOMElement * root_node=source_document->getDocumentElement();

	//Определяем имя маршрута
	std::wstring name(L"Маршрут");
	DOMNamedNodeMap * attributes=root_node->getAttributes();
	if (attributes!=NULL)
	{
		DOMAttr * attr=(DOMAttr*)attributes->getNamedItem(L"name");
		if (attr!=NULL)
			name=wstring(attr->getValue());
	}
	//Создаем маршрут
	VRoute route(name);

	//Перебираем участки
	DOMNodeList * section_node=((DOMElement*)root_node)->getElementsByTagName(L"section");
	for (size_t j=0; j<section_node->getLength(); ++j)
	{
		VRouteSection section;
		DOMNamedNodeMap * attributes=section_node->item(j)->getAttributes();
		if (attributes!=NULL)
		{
			DOMAttr * attr=(DOMAttr*)attributes->getNamedItem(L"direction");
			if (attr!=NULL)
				section.GetCode()=_wtol(attr->getValue());
			attr=(DOMAttr*)attributes->getNamedItem(L"way");
			if (attr!=NULL)
				section.GetWay() = wstring_to_string(attr->getValue()).c_str();
			attr=(DOMAttr*)attributes->getNamedItem(L"start_km");
			if (attr!=NULL)
				section.GetSection().start.km=_wtol(attr->getValue());
			attr=(DOMAttr*)attributes->getNamedItem(L"start_m");
			if (attr!=NULL)
				section.GetSection().start.m=static_cast<float>(_wtof(attr->getValue()));
			attr=(DOMAttr*)attributes->getNamedItem(L"end_km");
			if (attr!=NULL)
				section.GetSection().end.km=_wtol(attr->getValue());
			attr=(DOMAttr*)attributes->getNamedItem(L"end_m");
			if (attr!=NULL)
				section.GetSection().end.m=static_cast<float>(_wtof(attr->getValue()));
			route.AddSection(section);
		}		
	}

	routes.push_back(route);
	error_handler.reset();
	parser.reset();	
}

/** Сохраняет маршрут в файл XML*/
void VXMLRouteList::SaveToFile(const wstring& in_filesdir, VRoute route, wstring in_filename)
{	
	//Создаем парсер						
	parser = boost::shared_ptr<XercesDOMParser>(new XercesDOMParser);									
	parser->setValidationScheme(XercesDOMParser::Val_Always);
	//Создаем обработчик ошибок						 
	error_handler = boost::shared_ptr<ErrorHandler>((ErrorHandler*) new HandlerBase());
	parser->setErrorHandler(error_handler.get());

	xercesc::DOMDocument * target_document=parser->getDocument();	
	target_document=impl->createDocument(NULL,L"route",NULL);

	//Получаем корневой узел
	DOMElement * root_node=target_document->getDocumentElement();
	root_node->setAttribute(L"name", route.GetName().c_str());
	
	DOMNodeList * old_section_node=root_node->getChildNodes();
	if (old_section_node->getLength()>0)
	for (int i=old_section_node->getLength()-1; i>=0; --i)
	{
		if (old_section_node->item(i)->getNodeType()==DOMNode::ELEMENT_NODE)
			root_node->removeChild(old_section_node->item(i));
	}
	//Находим и удаляем все теги <route>
	/*DOMNodeList * route_node=root_node->getElementsByTagName(L"route");
	while (route_node->getLength()>0)
	{
		root_node->removeChild(route_node->item(0));
	}*/	

	//Добавляем новые узлы
	
	for (size_t j=0; j<route.GetSectionCount(); ++j)
	{
		VRouteSection route_section = route.GetSection(j);
		DOMElement * section_node = target_document->createElement(L"section");
		wstring direction = boost::str(boost::wformat(L"%d") % route_section.GetCode());
		wstring way = boost::str(boost::wformat(L"%s") % string_to_wstring(route_section.GetWay()).c_str());
		wstring start_km = boost::str(boost::wformat(L"%d") % route_section.GetSection().start.km);
		wstring start_m = boost::str(boost::wformat(L"%f") % route_section.GetSection().start.m); 
		wstring end_km = boost::str(boost::wformat(L"%d") % route_section.GetSection().end.km);
		wstring end_m = boost::str(boost::wformat(L"%f") % route_section.GetSection().end.m); 
		section_node->setAttribute(L"direction", direction.c_str());
		section_node->setAttribute(L"way", way.c_str());
		section_node->setAttribute(L"start_km", start_km.c_str());
		section_node->setAttribute(L"start_m", start_m.c_str());
		section_node->setAttribute(L"end_km", end_km.c_str());
		section_node->setAttribute(L"end_m", end_m.c_str());
		root_node->appendChild(section_node);
	}
	
	//Записываем
		
	//Создаем сериалайзер
	DOMLSSerializer * serializer = ((DOMImplementationLS*)impl)->createLSSerializer();
	//Создаем выходной объект
	DOMLSOutput * output = ((DOMImplementationLS*)impl)->createLSOutput();

	wstring filename;
	if (in_filename==L"")
		filename=in_filesdir+L"\\"+route.GetName()+L".xml";
	else filename=in_filesdir+L"\\"+in_filename;
	boost::scoped_ptr<XMLFormatTarget> target(new LocalFileFormatTarget(filename.c_str()));
	output->setByteStream(target.get());
	serializer->write(target_document, output);

	target_document->release();
	output->release();
	serializer->release();
	error_handler.reset();
	parser.reset();	
}


/** Очищает список*/
void VXMLRouteList::Clear()
{
	routes.clear();
}

/** Возвращает число маршрутов*/
size_t VXMLRouteList::GetCount()
{
	return routes.size();
}

/** Возвращает ссылку на маршрут*/
const VRoute& VXMLRouteList::GetRoute(size_t i) const
{
	return routes.at(i);
}

/** Возвращает константную ссылку на маршрут*/
VRoute& VXMLRouteList::GetRoute(size_t i)
{
	return routes.at(i);
}

/** Возвращает позицию маршрута в списке или -1, если маршрута в списке нет*/
int VXMLRouteList::IsNameInList(const wstring& in_name)
{	
	for (size_t i=0;i<routes.size();i++)
	{
		if (routes.at(i).GetName()==in_name) return i;
	}
	return -1;
}

/** Добавлеяет маршрут в список*/
void VXMLRouteList::AddRoute(const wstring& in_name)
{
	VRoute route(in_name);
	routes.push_back(route);
}

/** Удаляет маршрут из списка*/
void VXMLRouteList::DeleteRoute(size_t in_index)
{
	std::vector<VRoute>::iterator p=routes.begin();	
	if (in_index>=0 && in_index<routes.size())
	{
		p+=in_index;
		routes.erase(p);
	}
}