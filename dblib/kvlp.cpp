#include "kvlp.h"

#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>

/// Инициализация Xerces
void VRSXMLTable::Init()
{
	try 
	{
		//Инициализируем Xercesc
		xercesc::XMLPlatformUtils::Initialize();
		//Получаем указатель на реализацию DOM			
		impl = DOMImplementationRegistry::getDOMImplementation(L"LS");				
		//Создаем парсер						
		parser = boost::shared_ptr<XercesDOMParser>(new XercesDOMParser);									
		parser->setValidationScheme(XercesDOMParser::Val_Always);						
		//Создаем обработчик ошибок						 
		error_handler = boost::shared_ptr<ErrorHandler>((ErrorHandler*) new HandlerBase());
		parser->setErrorHandler(error_handler.get());
	}
	catch (...) 
	{				
		throw VSimpleException(L"Не удалось инициализировать парсер XML.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

/// Освобождение ресурсов
void VRSXMLTable::Terminate()
{
	error_handler.reset();
	parser.reset();
	xercesc::XMLPlatformUtils::Terminate();	
}

/// Загрузка документа
void VRSXMLTable::Load(const std::wstring& in_xml_file_name)
{
	try
	{				
		//Парсим документ
		parser->reset();
		parser->parse(in_xml_file_name.c_str());
		source_document=parser->getDocument();		
	}
	catch (const DOMException& e)
	{			
		std::wstring message = boost::str(boost::wformat(L"%s : %s") % in_xml_file_name % e.getMessage());
		throw VSimpleException(L"Ошибка разбора XML", message.c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	catch (const SAXException& e)
	{				
		std::wstring message = boost::str(boost::wformat(L"%s : %s") % in_xml_file_name % e.getMessage());
		throw VSimpleException(L"Ошибка разбора XML", message.c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	catch (const XMLException& e)
	{				
		std::wstring message = boost::str(boost::wformat(L"%s : %s") % in_xml_file_name % e.getMessage());
		throw VSimpleException(L"Ошибка разбора XML", message.c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	catch (...)
	{		
		throw VSimpleException(L"Ошибка разбора XML", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}		
}

/// Конструктор
VRSXMLTable::VRSXMLTable(const std::wstring& in_xml_file_name)
{
	Init();
	Load(in_xml_file_name);

	//Получаем корневой узел
	DOMElement * root=source_document->getDocumentElement();
	DOMNodeList * data=root->getElementsByTagName(L"z:row");	
	for (size_t i=0; i<data->getLength(); ++i)
	{
		DOMNamedNodeMap * attributes=data->item(i)->getAttributes();
		if (attributes!=NULL)
		{
			ROW row;
			for (size_t j=0; j<attributes->getLength(); j++)
			{
				DOMAttr * attr=(DOMAttr*)attributes->item(j);
				row.insert(CELL(boost::algorithm::to_lower_copy(std::wstring(attr->getName())),attr->getValue()));
			}
			table.push_back(row);
		}
	}
}

/// Деструктор
VRSXMLTable::~VRSXMLTable()
{
	Terminate();
}

/// Возвращает число строк
size_t VRSXMLTable::GetRowCount() const
{
	return table.size();
}

/// Возвращает содержимое ячейки
const std::wstring& VRSXMLTable::GetCellValue(size_t in_row, const std::wstring& in_key) const
{
	return table.at(in_row).find(boost::algorithm::to_lower_copy(in_key))->second;
}

/// Сохранение в CSV
void VRSXMLTable::SaveAsCSV() const
{
	throw std::wstring(L"ToDo!!!");
}
