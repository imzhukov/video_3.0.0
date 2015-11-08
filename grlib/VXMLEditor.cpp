#include "VXMLEditor.h"
#include "strlib.h"

/** Вспомогательная функция - загружает атрибут, имеющий значения 0 и 1*/
bool LoadBoolAttr(DOMNamedNodeMap * attributes, const XMLCh * name, bool def_value)
{
	DOMAttr * attr = (DOMAttr *)(attributes->getNamedItem(name));
	if (attr!=NULL)
		if (_wtoi(attr->getValue())==1)
			return true;
		else if (_wtoi(attr->getValue())==0)
			return false;
	return def_value;
}

///Сохранить атрибут типа int
void SaveAttr(DOMElement * node, const XMLCh * name, int attr)
{	
	node->setAttribute(name, boost::lexical_cast<std::wstring>(attr).c_str());	
}

///Сохранить атрибут типа double
void SaveAttr(DOMElement * node, const XMLCh * name, double attr)
{		
	node->setAttribute(name, boost::lexical_cast<std::wstring>(attr).c_str());
}

/** Загрузка данных из файла*/
void VXMLEditor::Load(const std::wstring fileName)
{	
	parser = boost::shared_ptr<XercesDOMParser>(new XercesDOMParser);
	parser->setValidationScheme(XercesDOMParser::Val_Always);						
	//Создаем обработчик ошибок						 
	error_handler = boost::shared_ptr<ErrorHandler>((ErrorHandler*) new HandlerBase());
	parser->setErrorHandler(error_handler.get());

	try
	{
		parser->parse(fileName.c_str());
	}
	catch (const DOMException& e)
	{				
		std::wstring message=boost::str(boost::wformat(L"%s (%s)") % std::wstring(e.getMessage()) % fileName );
		throw VSimpleException(L"Некорректный XML документ", message.c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	catch (const SAXException& e)
	{				
		std::wstring message=boost::str(boost::wformat(L"%s (%s)") % std::wstring(e.getMessage()) % fileName );
		throw VSimpleException(L"Некорректный XML документ", message.c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	catch (const XMLException& e)
	{				
		std::wstring message=boost::str(boost::wformat(L"%s (%s)") % std::wstring(e.getMessage()) %fileName );
		throw VSimpleException(L"Некорректный XML документ", message.c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
	
	xercesc::DOMDocument * source_document=parser->getDocument();
	//Получаем корневой узел
	DOMElement * root_node=source_document->getDocumentElement();

	DOMNamedNodeMap * attributes = root_node->getAttributes();
	if (attributes!=NULL)
	{		
		DOMAttr * attr = (DOMAttr*)attributes->getNamedItem(L"name");
		if (attr!=NULL)
			Graphics.SetName(wstring(attr->getValue()));
		attr = (DOMAttr*)attributes->getNamedItem(L"group");
		if (attr!=NULL)
			Graphics.SetGroup(wstring(attr->getValue()));
		attr = (DOMAttr*)attributes->getNamedItem(L"filter");
		if (attr!=NULL)
			Graphics.SetFilter(wstring(attr->getValue()));		
		attr = (DOMAttr*)attributes->getNamedItem(L"left_margin");
		if (attr!=NULL)
			Graphics.SetLeftMargin(_wtoi(attr->getValue()));
		attr = (DOMAttr*)attributes->getNamedItem(L"top_margin");
		if (attr!=NULL)
			Graphics.SetTopMargin(_wtoi(attr->getValue()));
		attr = (DOMAttr*)attributes->getNamedItem(L"right_margin");
		if (attr!=NULL)
			Graphics.SetRightMargin(_wtoi(attr->getValue()));
		attr = (DOMAttr*)attributes->getNamedItem(L"bottom_margin");
		if (attr!=NULL)
			Graphics.SetBottomMargin(_wtoi(attr->getValue()));
		attr = (DOMAttr*)attributes->getNamedItem(L"aprinfo_source");
		if (attr!=NULL)
			Graphics.SetAprinfoSource(APRINFO_SOURCE(_wtoi(attr->getValue())));		
		attr = (DOMAttr*)attributes->getNamedItem(L"scale_x");
		if (attr!=NULL)
			Graphics.SetScaleX(attr->getValue());
		attr = (DOMAttr*)attributes->getNamedItem(L"abscoord_margins");
		if (attr!=NULL)
			Graphics.SetAbscoordMargins(ABSCOORD_MARGINS(_wtoi(attr->getValue())));
		attr = (DOMAttr*)attributes->getNamedItem(L"actual_date");
		if (attr!=NULL)
			Graphics.SetDate(ACTUAL_DATE(_wtoi(attr->getValue())));

		//получаем теги областей
		DOMNodeList * area_node = root_node->getElementsByTagName(L"area");
		for (size_t i=0; i < area_node->getLength(); i++)
		{
			VXMLArea area;
			DOMNamedNodeMap * areaAttributes = area_node->item(i)->getAttributes();
			if (areaAttributes!=NULL)
			{
				DOMAttr * areaAttr = (DOMAttr*)areaAttributes->getNamedItem(L"name");
				if (areaAttr!=NULL)
					area.SetName(wstring(areaAttr->getValue()));
				areaAttr = (DOMAttr*)areaAttributes->getNamedItem(L"rel_size");
				if (areaAttr!=NULL)
					area.SetRelSize(_wtoi(areaAttr->getValue()));
				areaAttr = (DOMAttr*)areaAttributes->getNamedItem(L"rel_pos");
				if (areaAttr!=NULL)
					area.SetRelPos(_wtoi(areaAttr->getValue()));
				area.SetShowFrame(LoadBoolAttr(areaAttributes,L"show_frame",true));
				area.SetShowName(LoadBoolAttr(areaAttributes,L"show_name",false));

				//получаем тег осей
				VXMLAxes axes;
				DOMNodeList * axes_node = ((DOMElement *)(area_node->item(i)))->getElementsByTagName(L"axes");
				DOMNamedNodeMap * axesAttributes = axes_node->item(0)->getAttributes();
				if (axesAttributes!=NULL)
				{
					DOMAttr * axesAttr = (DOMAttr*)axesAttributes->getNamedItem(L"name");
					if (axesAttr!=NULL)
						axes.SetName(wstring(axesAttr->getValue()));
					axes.SetShowXLines(LoadBoolAttr(axesAttributes,L"show_x_lines",true));
					axes.SetShowXCoord(LoadBoolAttr(axesAttributes,L"show_x_coord",true));
					axes.SetShowYLines(LoadBoolAttr(axesAttributes,L"show_y_lines",true));
					axes.SetShowYCoord(LoadBoolAttr(axesAttributes,L"show_y_coord",true));
					axes.SetShowXScale(LoadBoolAttr(axesAttributes,L"show_x_scale",false));
					axes.SetShowYScale(LoadBoolAttr(axesAttributes,L"show_y_scale",false));
					axes.SetShowLegendY(LoadBoolAttr(axesAttributes,L"show_y_legend",false));
					axesAttr = (DOMAttr*)axesAttributes->getNamedItem(L"legend_y");
					if (axesAttr!=NULL)
						axes.SetLegendY(wstring(axesAttr->getValue()));
					axesAttr = (DOMAttr*)axesAttributes->getNamedItem(L"scale_method");
					if (axesAttr!=NULL)
						axes.SetScaleMethod(wstring(axesAttr->getValue()));
					axesAttr = (DOMAttr*)axesAttributes->getNamedItem(L"max_y");
					if (axesAttr!=NULL)
						axes.SetMaxY(wstring(axesAttr->getValue()));
					axesAttr = (DOMAttr*)axesAttributes->getNamedItem(L"min_y");
					if (axesAttr!=NULL)
						axes.SetMinY(wstring(axesAttr->getValue()));
					axesAttr = (DOMAttr*)axesAttributes->getNamedItem(L"scale_y");
					if (axesAttr!=NULL)
						axes.SetScaleY(wstring(axesAttr->getValue()));					
					axesAttr = (DOMAttr*)axesAttributes->getNamedItem(L"avail_scale_y");
					if (axesAttr!=NULL)
						axes.SetAvailScaleY(wstring(axesAttr->getValue()));
					axes.SetRelative(LoadBoolAttr(axesAttributes,L"relative",false));					

					//получаем теги графиков
					DOMNodeList * static_graph_node = ((DOMElement *)(axes_node->item(0)))->getElementsByTagName(L"static_graph");
					for (size_t j=0; j < static_graph_node->getLength(); j++)
					{
						DOMNamedNodeMap * staticAttributes = static_graph_node->item(j)->getAttributes();
						if (staticAttributes!=NULL)
						{
							VXMLStaticGraph staticGraph;
							DOMAttr * staticAttr = (DOMAttr*)staticAttributes->getNamedItem(L"name");
							if (staticAttr!=NULL)
								staticGraph.SetName(wstring(staticAttr->getValue()));
							staticAttr = (DOMAttr*)staticAttributes->getNamedItem(L"legend");
							if (staticAttr!=NULL)
								staticGraph.SetLegend(wstring(staticAttr->getValue()));
							staticAttr = (DOMAttr*)staticAttributes->getNamedItem(L"color");
							if (staticAttr!=NULL)
								staticGraph.SetColor(wstring(staticAttr->getValue()));
							staticAttr = (DOMAttr*)staticAttributes->getNamedItem(L"fill_color");
							if (staticAttr!=NULL)
								staticGraph.SetFillColor(wstring(staticAttr->getValue()));
							staticAttr = (DOMAttr*)staticAttributes->getNamedItem(L"width");
							if (staticAttr!=NULL)
								staticGraph.SetWidth(str_to_double(staticAttr->getValue()));
							staticAttr = (DOMAttr*)staticAttributes->getNamedItem(L"type");
							if (staticAttr!=NULL)
								staticGraph.SetType(wstring(staticAttr->getValue()));
							staticAttr = (DOMAttr*)staticAttributes->getNamedItem(L"data_type");
							if (staticAttr!=NULL)
								staticGraph.SetDataType(wstring(staticAttr->getValue()));							
							staticGraph.SetClip(LoadBoolAttr(staticAttributes,L"clip", false) ? CLIP_AREA : CLIP_HORIZONTAL);			
							staticGraph.SetShowLabels(LoadBoolAttr(staticAttributes,L"show_labels",false));							
							staticGraph.SetShowLegend(LoadBoolAttr(staticAttributes,L"show_legend",true));
							staticGraph.SetTrackMouse(LoadBoolAttr(staticAttributes,L"track_mouse",true));
							
							axes.AddStaticGraph(staticGraph);
						}
					}

					//получаем теги инструментов
					DOMNodeList * tool_node = ((DOMElement *)(axes_node->item(0)))->getElementsByTagName(L"tool");
					for (size_t j=0; j < tool_node->getLength(); j++)
					{
						DOMNamedNodeMap * toolAttributes = tool_node->item(j)->getAttributes();
						if (toolAttributes!=NULL)
						{
							VXMLTool tool;
							DOMAttr * toolAttr = (DOMAttr*)toolAttributes->getNamedItem(L"name");
							if (toolAttr!=NULL)
								tool.SetName(wstring(toolAttr->getValue()));
							toolAttr = (DOMAttr*)toolAttributes->getNamedItem(L"color");
							if (toolAttr!=NULL)
								tool.SetColor(wstring(toolAttr->getValue()));
							toolAttr = (DOMAttr*)toolAttributes->getNamedItem(L"fill_color");
							if (toolAttr!=NULL)
								tool.SetFillColor(wstring(toolAttr->getValue()));
							toolAttr = (DOMAttr*)toolAttributes->getNamedItem(L"width");
							if (toolAttr!=NULL)
								tool.SetWidth(str_to_double(toolAttr->getValue()));
							toolAttr = (DOMAttr*)toolAttributes->getNamedItem(L"data_type");
							if (toolAttr!=NULL)
								tool.SetDataType(wstring(toolAttr->getValue()));
							tool.SetClip(LoadBoolAttr(toolAttributes,L"clip", true) ? CLIP_AREA : CLIP_HORIZONTAL);

							axes.AddTool(tool);
						}
					}								
				}
				area.SetAxes(axes);
				Graphics.AddArea(area);
			}
		}
	}
}

/** Сохранение данных в файл*/
void VXMLEditor::Save(const wstring fileName)
{
	DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(L"LS");

	parser.reset();
	error_handler.reset();

	xercesc::DOMDocument * target_document = impl->createDocument(NULL,L"graphics",NULL);

	DOMElement * root_node = target_document->getDocumentElement();
	root_node->setAttribute(L"name",Graphics.GetName().c_str());
	root_node->setAttribute(L"group",Graphics.GetGroup().c_str());	
	root_node->setAttribute(L"filter",Graphics.GetFilter().c_str());	
	SaveAttr(root_node,L"left_margin",Graphics.GetLeftMargin());
	SaveAttr(root_node,L"top_margin",Graphics.GetTopMargin());
	SaveAttr(root_node,L"right_margin",Graphics.GetRightMargin());
	SaveAttr(root_node,L"bottom_margin",Graphics.GetBottomMargin());
	SaveAttr(root_node,L"aprinfo_source",Graphics.GetAprinfoSource());		
	root_node->setAttribute(L"scale_x",Graphics.GetScaleX().c_str());		
	SaveAttr(root_node,L"abscoord_margins",Graphics.GetAbscoordMargins());
	SaveAttr(root_node,L"actual_date",Graphics.GetDate());

	for (size_t i=0; i<Graphics.GetAreaCount(); i++)
	{
		VXMLArea area=Graphics.GetArea(i);
		DOMElement * area_node = target_document->createElement(L"area");
		area_node->setAttribute(L"name",area.GetName().c_str());
		SaveAttr(area_node,L"rel_size",area.GetRelSize());		
		SaveAttr(area_node,L"rel_pos",area.GetRelPos());
		SaveAttr(area_node,L"show_frame",area.GetShowFrame());
		SaveAttr(area_node,L"show_name",area.GetShowName());

		VXMLAxes axes=area.GetAxes();
		DOMElement * axes_node = target_document->createElement(L"axes");

		axes_node->setAttribute(L"name",axes.GetName().c_str());
		SaveAttr(axes_node,L"show_x_lines",axes.GetShowXLines());
		SaveAttr(axes_node,L"show_x_coord",axes.GetShowXCoord());
		SaveAttr(axes_node,L"show_y_lines",axes.GetShowYLines());
		SaveAttr(axes_node,L"show_y_coord",axes.GetShowYCoord());
		SaveAttr(axes_node,L"show_x_scale",axes.GetShowXScale());
		SaveAttr(axes_node,L"show_y_scale",axes.GetShowYScale());
		SaveAttr(axes_node,L"show_y_legend",axes.GetShowLegendY());
		axes_node->setAttribute(L"legend_y",axes.GetLegendY().c_str());
		axes_node->setAttribute(L"scale_method",axes.GetScaleMethod().c_str());
		axes_node->setAttribute(L"max_y",axes.GetMaxY().c_str());
		axes_node->setAttribute(L"min_y",axes.GetMinY().c_str());
		axes_node->setAttribute(L"scale_y",axes.GetScaleY().c_str());		
		axes_node->setAttribute(L"avail_scale_y",axes.GetAvailScaleY().c_str());
		SaveAttr(axes_node,L"relative",axes.GetRelative());		

		for (size_t j=0; j<axes.GetStaticGraphCount(); j++)
		{
			VXMLStaticGraph static_graph = axes.GetStaticGraph(j);

			DOMElement * static_graph_node = target_document->createElement(L"static_graph");

			static_graph_node->setAttribute(L"name",static_graph.GetName().c_str());
			static_graph_node->setAttribute(L"legend",static_graph.GetLegend().c_str());
			static_graph_node->setAttribute(L"color",static_graph.GetColor().c_str());
			static_graph_node->setAttribute(L"fill_color",static_graph.GetFillColor().c_str());
			SaveAttr(static_graph_node,L"width",static_graph.GetWidth());				
			static_graph_node->setAttribute(L"type",static_graph.GetType().c_str());
			static_graph_node->setAttribute(L"data_type",static_graph.GetDataType().c_str());						
			SaveAttr(static_graph_node,L"clip", static_graph.GetClip()==CLIP_AREA);
			SaveAttr(static_graph_node,L"show_labels",static_graph.GetShowLabels());
			SaveAttr(static_graph_node,L"show_legend",static_graph.GetShowLegend());
			SaveAttr(static_graph_node,L"track_mouse",static_graph.GetTrackMouse());
			
			axes_node->appendChild(static_graph_node);
		}
		
		for (size_t j=0; j<axes.GetToolCount(); j++)
		{
			VXMLTool tool = axes.GetTool(j);

			DOMElement * tool_node = target_document->createElement(L"tool");

			tool_node->setAttribute(L"name",tool.GetName().c_str());
			tool_node->setAttribute(L"color",tool.GetColor().c_str());
			tool_node->setAttribute(L"fill_color",tool.GetFillColor().c_str());
			SaveAttr(tool_node,L"width",tool.GetWidth());				
			tool_node->setAttribute(L"data_type",tool.GetDataType().c_str());			
			SaveAttr(tool_node,L"clip", tool.GetClip()==CLIP_AREA);

			axes_node->appendChild(tool_node);
		}

		area_node->appendChild(axes_node);
		root_node->appendChild(area_node);
	}

	//Создаем сериалайзер
	DOMLSSerializer * serializer = ((DOMImplementationLS*)impl)->createLSSerializer();
	//Создаем выходной объект
	DOMLSOutput * output = ((DOMImplementationLS*)impl)->createLSOutput();

	boost::scoped_ptr<XMLFormatTarget> target(new LocalFileFormatTarget(fileName.c_str()));
	output->setByteStream(target.get());
	serializer->write(target_document, output);

	target_document->release();
	output->release();
	serializer->release();
}