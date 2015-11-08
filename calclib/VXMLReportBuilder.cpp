#include "VXMLReportBuilder.h"
#include "strlib.h"
#include "Shiny.h"
#include "dbfunc.h"
#include "grlib.h"
#include "dbchart.h"

/** ��������� ����������� ������ ����: "����1=��������1;����2=��������2 ..."*/
KeyList ParseKeys(const wstring& in_key_string)
{
	KeyList result;
	//������, ����������� ������ � �������	
	typedef boost::tokenizer<boost::char_separator<wchar_t>, wstring::const_iterator, wstring> tokenizer;				
	boost::char_separator<wchar_t> separator(L";");
	tokenizer tok(in_key_string, separator);		
	for (tokenizer::iterator i=tok.begin(); i!=tok.end(); ++i)
	{
		//������ ������� - ������ ���� ��=km
		vector<wstring> pair;
		boost::algorithm::split(pair, *i, boost::is_any_of(L"="));
		if (pair.size()==2 )
		{
			result.insert(Key(pair.at(0),pair.at(1)));	
		}
		else
		{
			throw VSimpleException(L"������ ������� ������", in_key_string.c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}		
	return result;
}

//-------------------------------------- ������ ������ VXMLReportBuilder -----------------------------------------

/** �����������*/
VXMLReportBuilder::VXMLReportBuilder() : out_file_name(L"report.xml"), builded(false), parameter_changed(false),
	version(L"1"), embed_pictures(true), preview_results(false)
{
	try 
	{
		//�������������� Xercesc
		xercesc::XMLPlatformUtils::Initialize();
		//�������� ��������� �� ���������� DOM			
		impl = DOMImplementationRegistry::getDOMImplementation(L"LS");
		//������� �����������
		serializer = ((DOMImplementationLS*)impl)->createLSSerializer();
		//������� �������� ������
		output = ((DOMImplementationLS*)impl)->createLSOutput();
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

/** ����������*/
VXMLReportBuilder::~VXMLReportBuilder()
{	
	output->release();
	serializer->release();	
	error_handler.reset();
	parser.reset();
	xercesc::XMLPlatformUtils::Terminate();	
}

/** �������� �������*/
void VXMLReportBuilder::LoadTemplate(const wstring& in_template_path)
{
	template_path=in_template_path;
	try
	{		
		//������ ��������
		parser->reset();
		parser->parse(in_template_path.c_str());
		source_document=parser->getDocument();		
	}
	catch (const DOMException& e)
	{			
		std::wstring message = boost::str(boost::wformat(L"%s : %s") % in_template_path % e.getMessage());
		throw VSimpleException(L"������ ������� XML", message.c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	catch (const SAXException& e)
	{				
		std::wstring message = boost::str(boost::wformat(L"%s : %s") % in_template_path % e.getMessage());
		throw VSimpleException(L"������ ������� XML", message.c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	catch (const XMLException& e)
	{				
		std::wstring message = boost::str(boost::wformat(L"%s : %s") % in_template_path % e.getMessage());
		throw VSimpleException(L"������ ������� XML", message.c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	catch (...)
	{		
		throw VSimpleException(L"������ ������� XML", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}		
	//�������� �������� ����
	DOMElement * root=source_document->getDocumentElement();
	//������� � ������������ ��� <REPORT>
	name=in_template_path;
	group=wstring(L"���������");
	filter=wstring(L"main;station;repair;database");
	DOMNodeList * report=root->getElementsByTagName(L"report");	
	for (size_t i=0; i<report->getLength(); ++i)
	{		
		//���������� ��� ��������� (�� ���������  - ���� � �����) � ��� ��������� �����
		DOMNamedNodeMap * attributes=report->item(i)->getAttributes();
		if (attributes!=NULL)
		{
			DOMAttr * attr=(DOMAttr*)attributes->getNamedItem(L"name");										
			if (attr!=NULL)
			{
				name=wstring(attr->getValue());					
			}
			attr=(DOMAttr*)attributes->getNamedItem(L"group");										
			if (attr!=NULL)
			{
				group=wstring(attr->getValue());					
			}
			attr=(DOMAttr*)attributes->getNamedItem(L"filter");										
			if (attr!=NULL)
			{
				filter=wstring(attr->getValue());					
			}
			attr=(DOMAttr*)attributes->getNamedItem(L"out_file");										
			if (attr!=NULL)
			{
				out_file_name=wstring(attr->getValue());					
			}
			attr=(DOMAttr*)attributes->getNamedItem(L"version");										
			if (attr!=NULL)
			{
				version=wstring(attr->getValue());					
			}
		}						
		break;		
	}
	//���������� ������ ������� ����������
	parameters=root->getElementsByTagName(L"parameter");
	builded=false;
}

/** �������� �������*/
void VXMLReportBuilder::LoadTemplate()
{
	LoadTemplate(template_path);
}

/** ���������� �������*/
void VXMLReportBuilder::SaveTemplate(const wstring& in_template_path)
{
	//������ ��������� ������ ���� �� ��� ���������.
	if (builded)
	{
		throw VSimpleException(L"������ ��� ���������", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	//��������� ������ ������ ���� ���������� ���������
	if (parameter_changed)
	{
		boost::scoped_ptr<XMLFormatTarget> target(new LocalFileFormatTarget(in_template_path.c_str()));
		output->setByteStream(target.get());
		serializer->write(source_document, output);
	}
}

/** ���������� �������*/
void VXMLReportBuilder::SaveTemplate()
{
	SaveTemplate(template_path);
}

/** ���������� ����� ������� ����������*/
size_t VXMLReportBuilder::GetParameterCount()
{
	if (parameters!=NULL)
		return parameters->getLength();
	else
		return 0;
}

/** ���������� ��� ���������*/
wstring VXMLReportBuilder::GetParameterName(size_t in_index)
{
	if (in_index<GetParameterCount())
	{		
		DOMNamedNodeMap * attributes=parameters->item(in_index)->getAttributes();
		if (attributes!=NULL)
		{	
			DOMAttr * attr=(DOMAttr*)attributes->getNamedItem(L"name");										
			if (attr!=NULL)
			{
				return wstring(attr->getValue());
			}
			else
			{
				throw VSimpleException(L"����������� ��������� ������.", L"�� ������� ��� ���������", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__); 
			}
		}
		else
		{
			throw VSimpleException(L"����������� ��������� ������.", L"�� ������� ��������� ���������", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__); 
		}		
	}
	else
	{
		throw OUT_OF_BOUNDS; 
	}
}

/** ���������� �������� ��������� (�� ��������� ������������ �����)*/
wstring VXMLReportBuilder::GetParameterDescription(size_t in_index)
{
	if (in_index<GetParameterCount())
	{		
		DOMNamedNodeMap * attributes=parameters->item(in_index)->getAttributes();
		if (attributes!=NULL)
		{	
			DOMAttr * attr=(DOMAttr*)attributes->getNamedItem(L"description");										
			if (attr!=NULL)
			{
				return wstring(attr->getValue());
			}
			else
			{
				return GetParameterName(in_index);	
			}
		}
		else
		{
			throw VSimpleException(L"����������� ��������� ������.", L"�� ������� ��������� ���������", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__); 
		}		
	}
	else
	{
		throw OUT_OF_BOUNDS;
	}
}

/** ���������� �������� ���������*/
wstring VXMLReportBuilder::GetParameterValue(size_t in_index)
{
	if (in_index<GetParameterCount())
	{
		DOMNamedNodeMap * attributes=parameters->item(in_index)->getAttributes();
		if (attributes!=NULL)
		{
			DOMAttr * attr=(DOMAttr*)attributes->getNamedItem(L"value");										
			if (attr!=NULL)
			{
				return wstring(attr->getValue());
			}
			else
			{
				throw VSimpleException(L"����������� ��������� ������.", L"�� ������� �������� ���������", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__); 
			}
		}
		else
		{
			throw VSimpleException(L"����������� ��������� ������.", L"�� ������� ��������� ���������", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__); 
		}
	}
	else
	{
		throw OUT_OF_BOUNDS;
	}
}

/** ������������� �������� ���������*/
void VXMLReportBuilder::SetParameterValue(size_t in_index, const wstring& in_value)
{
	if (in_index<GetParameterCount())
	{
		static_cast<DOMElement*>(parameters->item(in_index))->setAttribute(L"value", in_value.c_str());
		parameter_changed=true;
	}
	else
	{
		throw OUT_OF_BOUNDS;
	}
}

/** ����������� ������� ��������*/
void VXMLReportBuilder::SubstParameter(const wstring& in_alias, const wstring& in_value)
{
	//�������� �������� ����
	DOMElement * root=source_document->getDocumentElement();
	SubstData(root, in_alias, in_value);
}

/** ���������� ������ ���������� �������� ��������� �� ����������� � ��*/
vector<wstring> VXMLReportBuilder::GetParameterAvail(size_t in_index, VIBDatabasePtr in_database)
{
	vector<wstring> result;
	if (in_index<GetParameterCount())
	{
		DOMNamedNodeMap * attributes=parameters->item(in_index)->getAttributes();
		if (attributes!=NULL)
		{
			//���� ���������� �������� ���������� �� ��
			DOMAttr * attr=(DOMAttr*)attributes->getNamedItem(L"avail_query");										
			if (attr!=NULL)
			{				
				wstring sql=wstring(attr->getValue());
				VIBTransaction transaction=in_database->StartTransaction();
				GetSingleField(in_database, transaction, wstring_to_string(sql), result);				
				in_database->RollbackTransaction(transaction);
				return result;
			}	
			//���� ���������� �������� ���������� �� ������
			attr=(DOMAttr*)attributes->getNamedItem(L"avail_list");
			if (attr!=NULL)
			{
				//������, ����������� ������ � �������
				wstring avail_list=wstring(attr->getValue());								
				boost::algorithm::split(result, avail_list, boost::is_any_of(L";"));												
				return result;
			}
		}		
	}
	else
	{
		throw OUT_OF_BOUNDS;
	}
	return result;
}

/** �������� �������*/
void VXMLReportBuilder::Build(const wstring& in_output_path, VIBDatabasePtr in_database, 
	VIBTransactionPtr in_transaction, VProgressInterface * in_progress)
{	
	try
	{	
		progress=in_progress;
		if (progress!=NULL)
		{
			progress->SetMessage(0, name.c_str());
			progress->Step(0);
			progress->Update();
		}
		builded=true;
		
		database=in_database;
		transaction=in_transaction;		

		//�������� �������� ����
		DOMElement * root=source_document->getDocumentElement();
		//������� �����������
		RemoveComments(root);
		//������������� ������ BLOB
		blob_index=0;
		blob_path=in_output_path;
		//������ ���������	
		ProcessNode(root);		
		//���������� ���������
		wstring output_file=VFileName(in_output_path, out_file_name).GetPath();
		//boost::scoped_ptr<XMLFormatTarget> target(new LocalFileFormatTarget(output_file.c_str()));		
		//output->setByteStream(target.get());		
		LocalFileFormatTarget target(output_file.c_str());
		output->setByteStream(&target);
		serializer->write(source_document, output);
	}	
	catch (const DOMException& e)
	{			
		throw VSimpleException(L"������ DOM", e.getMessage(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
	/*
		��������� ����������� ���������� , ������� ����� ���������� ��� �������� �������
		���� LocalFileFormatTarget � ���� �����. ���� ����� �� �������, ��������� ����� ������. 
		������� ��� ������� � ������������� ��������� ������ Xerces.
	*/
	catch (const XMLException& e)//���������� Xercesc
	{				
		throw VSimpleException(L"������ XML", e.getMessage(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
}

/** �������*/
void VXMLReportBuilder::Reset()
{
	parser.reset();
	builded=false;
}

/** ���������� ��������� �� ������ ����������� �����������*/
void VXMLReportBuilder::SetDisplayDialog(VPreviewPtr in_preview_dialog)
{
	preview_dialog=in_preview_dialog;
}

/** �������� ��������� �� ������ ����������� �����������*/
void VXMLReportBuilder::ResetDisplayDialog()
{	
	preview_dialog.reset();
}

/** ���������� ������*/
const wstring& VXMLReportBuilder::GetVersion()
{
	return version;
}

/** ���������� ��� ���������*/
const wstring& VXMLReportBuilder::GetName()
{
	return name;
}

/** ���������� ��� ������*/
const wstring& VXMLReportBuilder::GetGroup()
{
	return group;
}

/** ���������� ������*/
const wstring VXMLReportBuilder::GetFilter()
{
	/*
	if (boost::algorithm::equals(group, L"����������� ����"))
		return L"station_ways";
	else
		return L"main_ways";
	*/
	return filter;
}

/** ���������� ��� ��������� �����*/
const wstring& VXMLReportBuilder::GetOutFileName()
{
	return out_file_name;
}

/** ���������� ���� � ����� ������� ���������*/
const wstring& VXMLReportBuilder::GetTemplatePath()
{
	return template_path;
}

/// ��������� ���� PRAMETER
void VXMLReportBuilder::ProcessParameter(DOMNode * in_node)
{
	//�������� ������ �������
	DOMNodeList * siblings=in_node->getParentNode()->getChildNodes();
	//����������� �������� ���������
	for(XMLSize_t i=0; i<siblings->getLength(); ++i)
	{			
		DOMElement * sibling=(DOMElement*)siblings->item(i);
		if (sibling->getNodeType()==DOMNode::ELEMENT_NODE)
		{
			DOMNamedNodeMap * attributes=in_node->getAttributes();
			if (attributes!=NULL)
			{
				DOMAttr * name_attr=(DOMAttr*)attributes->getNamedItem(L"name");
				DOMAttr * value_attr=(DOMAttr*)attributes->getNamedItem(L"value");						
				if (name_attr!=NULL && value_attr!=NULL)
				{
					wstring name(name_attr->getValue());
					wstring value(value_attr->getValue());
					//���� ���� ��� subst_list, ����� ������ ���������� �������� �� subst_list FixMe!!! ���������?
					DOMAttr * subst_list_attr=(DOMAttr*)attributes->getNamedItem(L"subst_list");
					if (subst_list_attr!=NULL)
					{							
						KeyList keys = ParseKeys(subst_list_attr->getValue());
						value = (*keys.find(value)).second;
					}
					//����������� �������� ���������
					SubstData(sibling, name, value);
					//���� ��� ����������� ��������
					if (boost::algorithm::iequals(name, L"embed_pictures"))
					{
						if (boost::algorithm::equals(value, L"��"))							
							embed_pictures=true;
						else
							embed_pictures=false;
					}
					if (boost::algorithm::iequals(name, L"preview"))
					{
						if (boost::algorithm::equals(value, L"��"))
							preview_results=true;
						else
							preview_results=false;
					}
				}
				else
				{
					//��������� �����������
					throw VSimpleException(L"����������� ��������� ������.", L"�� ������� ���������", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				}
			}
			else
			{
				//��������� �����������
				throw VSimpleException(L"����������� ��������� ������.", L"�� ������� ���������", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
		}
	}
	//������� ����
	in_node->getParentNode()->removeChild(in_node);
};

/// ��������� ���� CHART
void VXMLReportBuilder::ProcessChart(DOMNode * in_node)
{
	VDBChart chart(database, transaction);
	//��������� ��� CHART
	std::vector<std::wstring> sql;
	wstring title=L"";
	bool show_x_lines=true;
	bool show_x_coord=true;
	bool show_y_lines=true;
	bool show_y_coord=true;
	size_t width=CHART_DEFAULT_WIDTH;
	size_t height=CHART_DEFAULT_HEIGHT;
	DOMNamedNodeMap * attributes=in_node->getAttributes();
	if (attributes!=NULL)
	{				
		DOMAttr * attr=(DOMAttr*)attributes->getNamedItem(L"width");
		if (attr!=NULL)
		{
			width=boost::lexical_cast<size_t>(std::wstring(attr->getValue()));
		}
		attr=(DOMAttr*)attributes->getNamedItem(L"height");
		if (attr!=NULL)
		{
			height=boost::lexical_cast<size_t>(std::wstring(attr->getValue()));
		}
		attr=(DOMAttr*)attributes->getNamedItem(L"title");
		if (attr!=NULL)
		{
			title=attr->getValue();
		}
		attr=(DOMAttr*)attributes->getNamedItem(L"show_x_lines");
		if (attr!=NULL)
		{
			chart.SetShowXLines(wcscmp(attr->getValue(), L"1")==0);
		}
		attr=(DOMAttr*)attributes->getNamedItem(L"show_x_coord");
		if (attr!=NULL)
		{
			chart.SetShowXCoord(wcscmp(attr->getValue(), L"1")==0);			
		}
		attr=(DOMAttr*)attributes->getNamedItem(L"show_y_lines");
		if (attr!=NULL)
		{
			chart.SetShowYLines(wcscmp(attr->getValue(), L"1")==0);			
		}
		attr=(DOMAttr*)attributes->getNamedItem(L"show_y_coord");
		if (attr!=NULL)
		{
			chart.SetShowYCoord(wcscmp(attr->getValue(), L"1")==0);
		}
	}
	else
	{
		//�������� sql ����������
		throw VSimpleException(L"����������� ��������� ������.", L"�� ������� ���������", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	
	//�������
	DOMNodeList * graph_nodes=in_node->getChildNodes();
	for (XMLSize_t n=0; n<graph_nodes->getLength(); ++n)
	{
		if (boost::algorithm::iequals(graph_nodes->item(n)->getNodeName(), L"graph"))
		{
			DOMNamedNodeMap * graph_attributes=graph_nodes->item(n)->getAttributes();
			DOMAttr * sql_attr=(DOMAttr*)graph_attributes->getNamedItem(L"sql");
			if (sql_attr!=NULL)
			{	
				std::wstring type=L"SQUARE";
				std::wstring legend=L"";
				VColor color=VColor(0,255,0,255);
				VColor fill_color=VColor(0,255,0,80);
				DOMAttr * type_attr=(DOMAttr*)graph_attributes->getNamedItem(L"type");
				if (type_attr!=NULL)
				{
					type=std::wstring(type_attr->getValue());
				}
				DOMAttr * legend_attr=(DOMAttr*)graph_attributes->getNamedItem(L"legend");
				if (legend_attr!=NULL)
				{
					legend=std::wstring(legend_attr->getValue());
				}
				DOMAttr * color_attr=(DOMAttr*)graph_attributes->getNamedItem(L"color");
				if (color_attr!=NULL)
				{
					color=VColor(color_attr->getValue());
				}
				DOMAttr * fill_color_attr=(DOMAttr*)graph_attributes->getNamedItem(L"fill_color");
				if (fill_color_attr!=NULL)
				{
					fill_color=VColor(fill_color_attr->getValue());
				}
				chart.AddQuery(VChartGraphDescription(sql_attr->getValue(), type, legend, color, fill_color));
			}			
			in_node->removeChild(graph_nodes->item(n));
		}		
	}
								
	chart.Prepare(title, width, height);
	std::stringstream static_graph;		
	chart.SaveStreamPNG(static_graph);

	//�������� ������� ����
	DOMNode * target_node=in_node->getParentNode();
	//�������� ����
	DOMNodeList * source_nodes=in_node->getChildNodes();			
	if (!static_graph.str().empty())
	{
		VBlob blob(static_graph);
		for (XMLSize_t n=0; n<source_nodes->getLength(); ++n)
		{					
			DOMNode * new_node=source_nodes->item(n)->cloneNode(true);
			BindPicture(new_node, L"chart", blob);
			target_node->appendChild(new_node);
			ProcessNode(new_node);
		}
	}	
	//������� ���� query
	target_node->removeChild(in_node);
}

/// ��������� ���� QUERY
void VXMLReportBuilder::ProcessQuery(DOMNode * in_node)
{
	//��������� ��� QUERY
	wstring sql;
	wstring preview=L"";
	wstring preview_title=L"��������������� ��������";
	DOMNamedNodeMap * attributes=in_node->getAttributes();
	if (attributes!=NULL)
	{
		DOMAttr * attr=(DOMAttr*)attributes->getNamedItem(L"preview");
		if (attr!=NULL)
		{
			preview=attr->getValue();
		}
		attr=(DOMAttr*)attributes->getNamedItem(L"preview_title");
		if (attr!=NULL)
		{
			preview_title=attr->getValue();
		}
		attr=(DOMAttr*)attributes->getNamedItem(L"sql");			
		if (attr!=NULL)
		{
			sql=attr->getValue();
		}
		else
		{
			//�������� sql ����������
			throw VSimpleException(L"����������� ��������� ������.", L"�� ������� ���������", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}
	else
	{
		//�������� sql ����������
		throw VSimpleException(L"����������� ��������� ������.", L"�� ������� ���������", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
	//�������� ������� ����
	DOMNode * target_node=in_node->getParentNode();
	//�������� ����
	DOMNodeList * source_nodes=in_node->getChildNodes();		
	
	VIBDataHandlerParameter handler;
	database->ExecuteStatement(*transaction, wstring_to_string(sql).c_str(), handler);
	if (target_node!=NULL)
	{
		vector<bool> selected;			
		//����� ����� ������ ������ ����� �� handler
		if (!boost::algorithm::iequals(preview, L"") && preview_dialog && preview_results && handler.GetRowCount()>0)
		{
			if (preview_dialog->ShowDialog(preview_title, handler, preview))
			{
				selected=preview_dialog->GetSelected();
			}
			else
			{
				throw VUserException(L"���������� ��������� �������� �������������");
			}
		}
		//���������� ������
		for (size_t r=0; r<handler.GetRowCount(); ++r)
		{
			if (selected.size()==0 || !preview_results || preview_results && selected.size()>0 && selected.at(r))//���� ������ �������� ��� ������������ ��������
			//���������� ����
			for (XMLSize_t n=0; n<source_nodes->getLength(); ++n)
			{											
				DOMNode * source_node=source_nodes->item(n);
				if (source_node->getNodeType()==DOMNode::ELEMENT_NODE)
				{
					//��������� ����
					DOMNode * new_node=source_node->cloneNode(true);
					//���������� ���������
					for (size_t c=0; c<handler.GetColCount(); ++c)
					{
						VIBParameter p=handler.GetParameter(r, c);						
						if ((( p.GetType() & ~1) == SQL_BLOB) || (( p.GetType() & ~1) == SQL_BLOB+1))
						{									
							ISC_QUAD blob_id;
							memcpy(&blob_id, p.SqlVar()->sqldata, p.SqlVar()->sqllen);								
							VBlob blob;
							database->ReadBLOB(*transaction, blob_id, &blob);
							BindPicture(new_node, p.GetAliasName(), blob);
						}
						else
						{										
							SubstData(new_node, p.GetAliasName(), p.GetString());
						}

					}
					target_node->appendChild(new_node);
					ProcessNode(new_node);
				}
			}
		}
	}			
		
	//������� ���� query
	target_node->removeChild(in_node);
}

/** ���������� ���������*/
void VXMLReportBuilder::ProcessNode(DOMNode * in_node)
{	
	if (progress!=NULL)	
	{
		if (progress->QueryCancel())
		{
			throw VUserException(L"���������� ��������� �������� �������������");
		}
		progress->Step(0);
	}				
	//���� ���� ���� - ��������, �� ���������� ���������� �������� � �������� ����, � ��� ���� �������
	if (boost::algorithm::iequals(in_node->getNodeName(), L"PARAMETER"))
	{	
		ProcessParameter(in_node);		
	}
	else if (boost::algorithm::iequals(in_node->getNodeName(), L"CHART"))
	{
		ProcessChart(in_node);		
	}
	//���� ���� ���� - ������, ��� ����� �������� � �������
	else if (boost::algorithm::iequals(in_node->getNodeName(), L"QUERY"))
	{
		ProcessQuery(in_node);
	}
	//��������� ���� ������������ � ��� ��� ������������ ��������	
	else
	{					
		//�������� ������ ��������
		DOMNodeList * children=in_node->getChildNodes();	
		//������ ������� ����� ���������� ���� - ���������
		for(XMLSize_t i=0; i<children->getLength(); ++i)
		{			
			DOMElement * child=(DOMElement*)children->item(i);
			if (child->getNodeType()==DOMNode::ELEMENT_NODE && boost::algorithm::iequals(child->getNodeName(), L"PARAMETER"))
			{
				ProcessNode(child);
			}
		}
		//� ����� ��� ���������
		for(XMLSize_t i=0; i<children->getLength(); ++i)
		{			
			DOMElement * child=(DOMElement*)children->item(i);
			if (child->getNodeType()==DOMNode::ELEMENT_NODE && !boost::algorithm::iequals(child->getNodeName(), L"PARAMETER"))
			{
				ProcessNode(child);
			}
		}
	}
}

///����������� ���������, ��������� �����������
void VXMLReportBuilder::RemoveComments(DOMNode * in_node)
{
	if (in_node->getNodeType()==DOMNode::COMMENT_NODE)
	{
		in_node->getParentNode()->removeChild(in_node);
		return;
	}
	//��������	
	for (XMLSize_t i=0; i<in_node->getChildNodes()->getLength(); ++i)
	{		
		RemoveComments(in_node->getChildNodes()->item(i));
	}
}

/// ����������� ��������� ���������� ����� ���������� �� ����������
void VXMLReportBuilder::SubstData(DOMNode * in_node, const wstring& in_alias, const wstring& in_value)
{	
	switch (in_node->getNodeType())
	{
		case DOMNode::TEXT_NODE:
		{
			wstring template_value(in_node->getNodeValue());
			boost::algorithm::ireplace_all(template_value, wstring(L"%")+in_alias+wstring(L"%"), in_value);
			in_node->setNodeValue(template_value.c_str());
			break;
		}
		case DOMNode::ELEMENT_NODE:
		{
			//����������� ���������� ������ �������� ����������		
			if (in_node->getAttributes()!=NULL)
			{		
				for (XMLSize_t i=0; i<in_node->getAttributes()->getLength(); ++i)
				{			
					DOMAttr * attr=(DOMAttr*)in_node->getAttributes()->item(i);
					wstring template_value(attr->getValue());
					boost::algorithm::ireplace_all(template_value, wstring(L"%")+in_alias+wstring(L"%"), in_value);
					attr->setValue(template_value.c_str());														
				}
			}		
			//��������	
			for (XMLSize_t i=0; i<in_node->getChildNodes()->getLength(); ++i)
			{		
				SubstData(in_node->getChildNodes()->item(i), in_alias, in_value);
			}
			break;
		}
		default:
			break;
	}
}

/// ��������� ��������
void VXMLReportBuilder::BindPicture(DOMNode * new_node, const std::wstring& in_alias, VBlob& blob)
{
	if (embed_pictures)
	{									
		//������� � ������� BLOB � xml
		wstring prefix(L"data:image/png;base64,");
		wstring base64;
		blob.Base64Encode(base64);									
		SubstData(new_node, in_alias, prefix+base64);
	}
	else
	{
		//������� � ������� ����� � ����
		VFileName blob_file(blob_path, boost::str(boost::wformat(L"blob%d.png") % blob_index));
		blob.Save(blob_file.GetPath().c_str());
		wstring subst_path=boost::str(boost::wformat(L"file:///%1%") % blob_file.GetPath());
		SubstData(new_node, in_alias, subst_path);	
		++blob_index;
	}
};

//-------------------------------------- ������ ������ VXMLCommandScript --------------------------------------

/// �����������
VXMLCommandScript::VXMLCommandScript()
{	
	//�������������� Xercesc
	xercesc::XMLPlatformUtils::Initialize();	
}

/// ����������
VXMLCommandScript::~VXMLCommandScript()
{			
	error_handler.reset();
	parser.reset();
	xercesc::XMLPlatformUtils::Terminate();	
}

/// �������� �������
void VXMLCommandScript::LoadScript(const wstring& in_file_path)
{
	//������� ���������� ������						 
	error_handler=boost::shared_ptr<ErrorHandler>((ErrorHandler*) new HandlerBase());	
	//������� ������						
	parser=boost::shared_ptr<XercesDOMParser>(new XercesDOMParser);									
	parser->setValidationScheme(XercesDOMParser::Val_Always);
	parser->setErrorHandler(error_handler.get());
	//������ ��������	
	parser->parse(in_file_path.c_str());	
}

/// ����������
void VXMLCommandScript::Execute(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, VProgressInterface * in_progress)
{	
	database=in_database;
	transaction=in_transaction;
	progress=in_progress;
	if (progress!=NULL)
	{		
		progress->Step(0);
		progress->Update();
	}
	//�������� �������� ����
	DOMElement * root=parser->getDocument()->getDocumentElement();
	ProcessNode(root);
}

/// ����������� ������� ��������� ����
void VXMLCommandScript::ProcessNode(DOMNode * in_node)
{
	if (progress!=NULL)	
	{
		if (progress->QueryCancel())
		{
			throw VUserException(L"���������� ��������� �������� �������������");
		}
		progress->Step(0);
	}
	try
	{
		if (boost::algorithm::iequals(in_node->getNodeName(), L"command"))
		{	
			DOMAttr * sql_attr=(DOMAttr*)in_node->getAttributes()->getNamedItem(L"sql");
			if (sql_attr==NULL)
				throw VSimpleException(L"����������� ��������� ������.", L"����������� �������� 'sql'", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__); 		
			//��������� �������
			wstring insert_sql(sql_attr->getValue());
			wstring sql=insert_sql;
			//��������� ������� BLOB
			DOMAttr * blob_attr=(DOMAttr*)in_node->getAttributes()->getNamedItem(L"blob");		
			std::vector<VIBParameter> param;
			if (blob_attr!=NULL)
			{
				VBlob blob;			
				wstring base64(blob_attr->getValue());
				boost::algorithm::ireplace_all(base64, L"data:image/png;base64,", L"");
				blob.Base64Decode(base64);
				ISC_QUAD blob_id = database->AddBlob(*transaction, blob);
				VIBParameter blob_id_var(SQL_BLOB+1, (char*)&blob_id);
				param.push_back(blob_id_var);
			}
			//��������� �������
			database->ExecuteStatementImmediate(*transaction, wstring_to_string(sql).c_str(), param);
		}
		else if (boost::algorithm::iequals(in_node->getNodeName(), L"command_part_1"))
		{
			DOMAttr * sql_attr=(DOMAttr*)in_node->getAttributes()->getNamedItem(L"sql");
			if (sql_attr==NULL)
				throw VSimpleException(L"����������� ��������� ������.", L"����������� �������� 'sql'", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__); 		
			//��������� �������
			wstring insert_sql(sql_attr->getValue());
			//�������� ������ ��������				
			for(XMLSize_t i=0; i<in_node->getChildNodes()->getLength(); ++i)
			{
				DOMElement * child=(DOMElement*)in_node->getChildNodes()->item(i);
				if (child->getNodeType()==DOMNode::ELEMENT_NODE && 
					boost::algorithm::iequals(child->getNodeName(), L"command_part_2"))
				{					
					DOMAttr * sql_attr=(DOMAttr*)child->getAttributes()->getNamedItem(L"sql");
					if (sql_attr==NULL)
						throw VSimpleException(L"����������� ��������� ������.", L"����������� �������� 'sql'", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__); 
					//��������� �������
					wstring values_sql(sql_attr->getValue());				
					wstring sql=insert_sql+values_sql;
					//��������� ������� BLOB
					DOMAttr * blob_attr=(DOMAttr*)child->getAttributes()->getNamedItem(L"blob");		
					std::vector<VIBParameter> param;
					if (blob_attr!=NULL)
					{
						VBlob blob;			
						wstring base64(blob_attr->getValue());
						boost::algorithm::ireplace_all(base64, L"data:image/png;base64,", L"");
						blob.Base64Decode(base64);
						ISC_QUAD blob_id = database->AddBlob(*transaction, blob);
						VIBParameter blob_id_var(SQL_BLOB+1, (char*)&blob_id);
						param.push_back(blob_id_var);
					}
					//��������� �������
					database->ExecuteStatementImmediate(*transaction, wstring_to_string(sql).c_str(), param);
				}
			}								
		}
	}
	catch (VException& e)
	{
		LOG_ERROR(e.FormatLong());
	}
	//��������
	DOMNodeList * children=in_node->getChildNodes();	
	for(XMLSize_t i=0; i<children->getLength(); ++i)
	{			
		DOMElement * child=(DOMElement*)children->item(i);
		if (child->getNodeType()==DOMNode::ELEMENT_NODE)
		{
			ProcessNode(child);
		}
	}
}
