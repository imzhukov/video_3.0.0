#include "strlib.h"
#include "VXMLRouteList.h"

//--------------------------------------- ������ ������ VRoute ---------------------------------------

/** ��������� ��������*/
bool section_comp(const VRouteSection& r, const VRouteSection& l)
{
	return (r.GetCode()<l.GetCode()) ||
		(r.GetCode()==l.GetCode()) && (r.GetWay()<l.GetWay()) ||
		(r.GetCode()==l.GetCode()) && (r.GetWay()==l.GetWay()) && r.GetSection().start<l.GetSection().start;
}

/** ����������� �� ���������*/
VRoute::VRoute() {};

/** �����������*/
VRoute::VRoute(const wstring& in_name) : name(in_name){};

/** �������� �������*/
void VRoute::AddSection(const VRouteSection& in_route_section)
{
	route.push_back(in_route_section);
	//��������� �������
	sort(route.begin(), route.end(), section_comp);
}

/** ����������� ������� ����*/
void VRoute::EditSection(const VRouteSection& in_route_section, const VRouteSection& out_route_section)
{
	for (size_t i=0; i<route.size(); i++)
		if (section_comp(route.at(i),in_route_section)==0) 
		{
			route.at(i)=out_route_section;
			return;
		}		
}

/** ������� ������� ����*/
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

/** ������� ��� ������� ����*/
void VRoute::Clear()
{
	route.clear();
}

/** ���������� ���*/
const wstring& VRoute::GetName() const
{
	return name;
}

/** �������� ���*/
void VRoute::SetName(const wstring& in_name)
{
	name=in_name;
}

/** ���������� ����� ��������*/
size_t VRoute::GetSectionCount() const
{
	return route.size();
}

/** ���������� ����������� ������ �� �������*/
const VRouteSection& VRoute::GetSection(size_t in_index) const
{
	return route.at(in_index);
}

/** ���������� �������*/
VRouteSection& VRoute::GetSection(size_t in_index)
{
	return route.at(in_index);
}


//------------------------------------- ������ ������ VXMLRouteList ----------------------------------------


/** �����������*/
VXMLRouteList::VXMLRouteList()
{
	//�������������� Xercesc
	xercesc::XMLPlatformUtils::Initialize();
	//�������� ��������� �� ���������� DOM
	impl = DOMImplementationRegistry::getDOMImplementation(L"LS");
}

/** ����������*/
VXMLRouteList::~VXMLRouteList()
{	
	xercesc::XMLPlatformUtils::Terminate();	
}

/** ��������� �������� �� ����� XML*/
void VXMLRouteList::LoadFromFile(const wstring& in_filename)
{
	//������� ������						
	parser = boost::shared_ptr<XercesDOMParser>(new XercesDOMParser);									
	parser->setValidationScheme(XercesDOMParser::Val_Always);						
	//������� ���������� ������						 
	error_handler = boost::shared_ptr<ErrorHandler>((ErrorHandler*) new HandlerBase());
	parser->setErrorHandler(error_handler.get());

	//������ ��������
	parser->reset();
	//FixMe!!! ����� ��������� ���������� ��������, ����� ���� �� ����������
	parser->parse(in_filename.c_str());
	xercesc::DOMDocument * source_document=parser->getDocument();
	//�������� �������� ����
	DOMElement * root_node=source_document->getDocumentElement();

	//���������� ��� ��������
	std::wstring name(L"�������");
	DOMNamedNodeMap * attributes=root_node->getAttributes();
	if (attributes!=NULL)
	{
		DOMAttr * attr=(DOMAttr*)attributes->getNamedItem(L"name");
		if (attr!=NULL)
			name=wstring(attr->getValue());
	}
	//������� �������
	VRoute route(name);

	//���������� �������
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

/** ��������� ������� � ���� XML*/
void VXMLRouteList::SaveToFile(const wstring& in_filesdir, VRoute route, wstring in_filename)
{	
	//������� ������						
	parser = boost::shared_ptr<XercesDOMParser>(new XercesDOMParser);									
	parser->setValidationScheme(XercesDOMParser::Val_Always);
	//������� ���������� ������						 
	error_handler = boost::shared_ptr<ErrorHandler>((ErrorHandler*) new HandlerBase());
	parser->setErrorHandler(error_handler.get());

	xercesc::DOMDocument * target_document=parser->getDocument();	
	target_document=impl->createDocument(NULL,L"route",NULL);

	//�������� �������� ����
	DOMElement * root_node=target_document->getDocumentElement();
	root_node->setAttribute(L"name", route.GetName().c_str());
	
	DOMNodeList * old_section_node=root_node->getChildNodes();
	if (old_section_node->getLength()>0)
	for (int i=old_section_node->getLength()-1; i>=0; --i)
	{
		if (old_section_node->item(i)->getNodeType()==DOMNode::ELEMENT_NODE)
			root_node->removeChild(old_section_node->item(i));
	}
	//������� � ������� ��� ���� <route>
	/*DOMNodeList * route_node=root_node->getElementsByTagName(L"route");
	while (route_node->getLength()>0)
	{
		root_node->removeChild(route_node->item(0));
	}*/	

	//��������� ����� ����
	
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
	
	//����������
		
	//������� �����������
	DOMLSSerializer * serializer = ((DOMImplementationLS*)impl)->createLSSerializer();
	//������� �������� ������
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


/** ������� ������*/
void VXMLRouteList::Clear()
{
	routes.clear();
}

/** ���������� ����� ���������*/
size_t VXMLRouteList::GetCount()
{
	return routes.size();
}

/** ���������� ������ �� �������*/
const VRoute& VXMLRouteList::GetRoute(size_t i) const
{
	return routes.at(i);
}

/** ���������� ����������� ������ �� �������*/
VRoute& VXMLRouteList::GetRoute(size_t i)
{
	return routes.at(i);
}

/** ���������� ������� �������� � ������ ��� -1, ���� �������� � ������ ���*/
int VXMLRouteList::IsNameInList(const wstring& in_name)
{	
	for (size_t i=0;i<routes.size();i++)
	{
		if (routes.at(i).GetName()==in_name) return i;
	}
	return -1;
}

/** ���������� ������� � ������*/
void VXMLRouteList::AddRoute(const wstring& in_name)
{
	VRoute route(in_name);
	routes.push_back(route);
}

/** ������� ������� �� ������*/
void VXMLRouteList::DeleteRoute(size_t in_index)
{
	std::vector<VRoute>::iterator p=routes.begin();	
	if (in_index>=0 && in_index<routes.size())
	{
		p+=in_index;
		routes.erase(p);
	}
}