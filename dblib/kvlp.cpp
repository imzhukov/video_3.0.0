#include "kvlp.h"

#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>

/// ������������� Xerces
void VRSXMLTable::Init()
{
	try 
	{
		//�������������� Xercesc
		xercesc::XMLPlatformUtils::Initialize();
		//�������� ��������� �� ���������� DOM			
		impl = DOMImplementationRegistry::getDOMImplementation(L"LS");				
		//������� ������						
		parser = boost::shared_ptr<XercesDOMParser>(new XercesDOMParser);									
		parser->setValidationScheme(XercesDOMParser::Val_Always);						
		//������� ���������� ������						 
		error_handler = boost::shared_ptr<ErrorHandler>((ErrorHandler*) new HandlerBase());
		parser->setErrorHandler(error_handler.get());
	}
	catch (...) 
	{				
		throw VSimpleException(L"�� ������� ���������������� ������ XML.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

/// ������������ ��������
void VRSXMLTable::Terminate()
{
	error_handler.reset();
	parser.reset();
	xercesc::XMLPlatformUtils::Terminate();	
}

/// �������� ���������
void VRSXMLTable::Load(const std::wstring& in_xml_file_name)
{
	try
	{				
		//������ ��������
		parser->reset();
		parser->parse(in_xml_file_name.c_str());
		source_document=parser->getDocument();		
	}
	catch (const DOMException& e)
	{			
		std::wstring message = boost::str(boost::wformat(L"%s : %s") % in_xml_file_name % e.getMessage());
		throw VSimpleException(L"������ ������� XML", message.c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	catch (const SAXException& e)
	{				
		std::wstring message = boost::str(boost::wformat(L"%s : %s") % in_xml_file_name % e.getMessage());
		throw VSimpleException(L"������ ������� XML", message.c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	catch (const XMLException& e)
	{				
		std::wstring message = boost::str(boost::wformat(L"%s : %s") % in_xml_file_name % e.getMessage());
		throw VSimpleException(L"������ ������� XML", message.c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	catch (...)
	{		
		throw VSimpleException(L"������ ������� XML", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}		
}

/// �����������
VRSXMLTable::VRSXMLTable(const std::wstring& in_xml_file_name)
{
	Init();
	Load(in_xml_file_name);

	//�������� �������� ����
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

/// ����������
VRSXMLTable::~VRSXMLTable()
{
	Terminate();
}

/// ���������� ����� �����
size_t VRSXMLTable::GetRowCount() const
{
	return table.size();
}

/// ���������� ���������� ������
const std::wstring& VRSXMLTable::GetCellValue(size_t in_row, const std::wstring& in_key) const
{
	return table.at(in_row).find(boost::algorithm::to_lower_copy(in_key))->second;
}

/// ���������� � CSV
void VRSXMLTable::SaveAsCSV() const
{
	throw std::wstring(L"ToDo!!!");
}
