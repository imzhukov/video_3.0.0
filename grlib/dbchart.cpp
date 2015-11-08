#include "dbchart.h"

//----------------------------------------- ������ ������ VChartGraphDescription ----------------------------------

/// �����������
VChartGraphDescription::VChartGraphDescription(const std::wstring& in_query, const std::wstring& in_type, 
	const std::wstring& in_legend, const VColor& in_color, const VColor& in_fill_color) : 
	query(in_query), type(in_type), legend(in_legend), color(in_color), fill_color(in_fill_color) {};

/// ���������� ����� �������
const std::wstring& VChartGraphDescription::GetQuery() const
{
	return query;
}

/// ���������� ��� �������
const std::wstring& VChartGraphDescription::GetType() const
{
	return type;
}

/// ���������� ����� �������
const std::wstring& VChartGraphDescription::GetLegend() const
{
	return legend;
}

/// ���������� ���� �������
const VColor& VChartGraphDescription::GetColor() const
{
	return color;
}

/// ���������� ���� �������
const VColor& VChartGraphDescription::GetFillColor() const
{
	return fill_color;
}


//-------------------------------------------- ������ ������ VDBChart ----------------------------------------------

/** �����������*/
VDBChart::VDBChart(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction) : 
	database(in_database), transaction(in_transaction) {};

/// ��������� ������
void VDBChart::AddQuery(const VChartGraphDescription& in_description)
{
	graph_descriptions.push_back(in_description);
}

/** ���������� ������*/
void VDBChart::Prepare(const std::wstring& in_title, double in_width, double in_height)
{	
	//������������� �������	
	SetDevice(VDrawingDevicePtr(new VDrawingDeviceDisplay(DPI_DEFAULT_SCREEN, DPI_DEFAULT_SCREEN, in_width, in_height)));

	//������� ����
	AddLayer(std::wstring(L"Background"));
	AddLayer(std::wstring(L"Frames"));
	AddLayer(std::wstring(L"GridLines"));
	AddLayer(std::wstring(L"Graph"));
	AddLayer(std::wstring(L"Legend"));

	//���
	GetLayer(std::wstring(L"Background"))->AddElement(VElementPtr(new VBackgroundElement(VColor(255,255,255,255))));

	//������� ���������	
	VAreaPtr header_area(new VArea());
	header_area->SetSize(in_width,50);
	header_area->SetPosition(0,0);
	VHeaderElementPtr header(new VHeaderElement(header_area, device));
	header->AddLine(in_title);
	GetLayer(std::wstring(L"Frames"))->AddElement(header);
	
	//������� �������
	VAreaPtr area(new VArea());
	area->SetSize(in_width-50, in_height-2*50);
	area->SetPosition(50,50);	
	VAxesPtr axes(new VAxes(area, device));
	
	//�����
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VAreaFrameElement(area, device)));

	//������� �������
	VLegendElementPtr area_legend(new VLegendElement(axes));
	GetLayer(std::wstring(L"Legend"))->AddElement(area_legend);
	//��������� ����
	if (show_y_lines)
	{
		GetLayer(std::wstring(L"GridLines"))->AddElement(
			VElementPtr(new VGridLinesYElement(axes, show_y_coord)));
	}
	if (show_x_lines)
	{
		GetLayer(std::wstring(L"GridLines"))->AddElement(
			VElementPtr(new VGridLinesXElement(axes, show_x_coord)));
	}		
	for (size_t i=0; i<graph_descriptions.size(); ++i)
	{
		//������� �������� ������
		VDataProviderPtr data(new VQueryData(database, transaction, graph_descriptions.at(i).GetQuery()));				
		if (boost::algorithm::iequals(graph_descriptions.at(i).GetType(), L"BARCHART"))
		{
			GetLayer(std::wstring(L"Graph"))->AddElement(
				VElementPtr(new VBarGraph(data, axes, graph_descriptions.at(i).GetColor(), 
				graph_descriptions.at(i).GetFillColor(), default_line_width, CLIP_NONE, true)));							
		}
		else if (boost::algorithm::iequals(graph_descriptions.at(i).GetType(), L"SQUARE"))
		{
			GetLayer(std::wstring(L"Graph"))->AddElement(
				VElementPtr(new VSquareGraph(data, axes, graph_descriptions.at(i).GetColor(), 
				graph_descriptions.at(i).GetFillColor(), default_line_width, CLIP_AREA, true)));						
		}
		else if (boost::algorithm::iequals(graph_descriptions.at(i).GetType(), L"XTEXT"))
		{
			GetLayer(std::wstring(L"Graph"))->AddElement(
				VElementPtr(new VTextGraph(data, axes, TO_VERTICAL, TP_AREA_BOTTOM_UNDER)));
		}
		else if (boost::algorithm::iequals(graph_descriptions.at(i).GetType(), L"LINE"))
		{
			GetLayer(std::wstring(L"Graph"))->AddElement(
				VElementPtr(new VLineGraph(data, axes, graph_descriptions.at(i).GetColor(), 
				graph_descriptions.at(i).GetFillColor(), default_line_width, CLIP_AREA)));
		}
		else if (boost::algorithm::iequals(graph_descriptions.at(i).GetType(), L"STICK"))
		{
			GetLayer(std::wstring(L"Graph"))->AddElement(
				VElementPtr(new VStickGraph(data, axes, graph_descriptions.at(i).GetColor(), 
				graph_descriptions.at(i).GetFillColor(), default_line_width, CLIP_AREA)));
		}
		else
		{
			throw VSimpleException(L"��� ������� �� ��������������", graph_descriptions.at(i).GetType(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}	

		if (graph_descriptions.at(i).GetLegend().compare(L"")!=0)
			area_legend->AddGraph(graph_descriptions.at(i).GetLegend(), graph_descriptions.at(i).GetColor());
		
		axes->AddDataProvider(data);
	}		
	axes->SetAutoScaleX();
	axes->SetAutoScaleY();
};


void VDBChart::SetShowXLines(bool in_show_x_lines)
{
	show_x_lines=in_show_x_lines;
}

void VDBChart::SetShowXCoord(bool in_show_x_coord)
{
	show_x_coord=in_show_x_coord;
}

void VDBChart::SetShowYLines(bool in_show_y_lines)
{
	show_y_lines=in_show_y_lines;
}

void VDBChart::SetShowYCoord(bool in_show_y_coord)
{
	show_y_coord=in_show_y_coord;
}