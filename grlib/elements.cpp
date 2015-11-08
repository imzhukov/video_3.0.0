#include "cairo_ex.h"
#include "elements.h"
#include "glyphs.h"
#include "shiny.h"

/// Выводит текст
void CairoShowText(cairo_t * cr, VAxesPtr axes, double x, double y,
	const std::wstring& in_text, TEXT_ORIENTATION to, TEXT_POSITION tp, double in_text_space)
{
	cairo_text_extents_t te;		
	cairo_text_extents(cr, in_text, &te);				
	if (to==TO_VERTICAL)
		{				
			switch (tp)
			{				
				case TP_OVER:
					cairo_move_to(cr, x+te.height/2, y-axes->DrawingToDeviceY(in_text_space));
					break;
				case TP_UNDER:
					cairo_move_to(cr, x+te.height/2, y+te.width+axes->DrawingToDeviceY(in_text_space));
					break;
				case TP_LEFT:
					cairo_move_to(cr, x+-axes->DrawingToDeviceX(in_text_space), y+te.width/2);
					break;
				case TP_RIGHT:
					cairo_move_to(cr, x+te.height+axes->DrawingToDeviceX(in_text_space), y+te.width/2);
					break;
				case TP_AREA_BOTTOM_OVER:					
					cairo_move_to(cr, x+te.height/2, axes->GetArea()->GetHeight()-axes->DrawingToDeviceY(in_text_space));
					break;
				case TP_AREA_BOTTOM_UNDER:					
					cairo_move_to(cr, x+te.height/2, axes->GetArea()->GetHeight()+te.width+axes->DrawingToDeviceY(in_text_space));
					break;
				case TP_AREA_TOP_OVER:				
					cairo_move_to(cr, x+te.height/2, -axes->DrawingToDeviceY(in_text_space));
					break;
				case TP_AREA_TOP_UNDER:					
					cairo_move_to(cr, x+te.height/2, te.width+axes->DrawingToDeviceY(in_text_space));
					break;
			}
			cairo_rotate(cr, -M_PI/2);
		}								
		else
		{			
			switch (tp)
			{
				case TP_OVER:
					cairo_move_to(cr, x-te.width/2, y-axes->DrawingToDeviceY(in_text_space));
					break;
				case TP_UNDER:
					cairo_move_to(cr, x-te.width/2, y+te.height+axes->DrawingToDeviceY(in_text_space));
					break;
				case TP_LEFT:
					cairo_move_to(cr, x-te.width-axes->DrawingToDeviceX(in_text_space), y+te.height/2);
					break;
				case TP_RIGHT:
					cairo_move_to(cr, x+axes->DrawingToDeviceX(in_text_space), y+te.height/2);
					break;
				case TP_AREA_BOTTOM_OVER:
					cairo_move_to(cr, x-te.width/2, axes->GetArea()->GetHeight()-axes->DrawingToDeviceY(in_text_space));
					break;
				case TP_AREA_BOTTOM_UNDER:
					cairo_move_to(cr, x-te.width/2, axes->GetArea()->GetHeight()+te.height+axes->DrawingToDeviceY(in_text_space));					
					break;
				case TP_AREA_TOP_OVER:
					cairo_move_to(cr, x-te.width/2, -axes->DrawingToDeviceY(in_text_space));
					break;
				case TP_AREA_TOP_UNDER:
					cairo_move_to(cr, x-te.width/2, te.height+axes->DrawingToDeviceY(in_text_space));
					break;
			}
		}
	cairo_show_text(cr, in_text);
}

//------------------------ Методы класса VBackgroundElement ----------------------------------------

/** Конструктор*/
VBackgroundElement::VBackgroundElement(const VColor& in_fill_color)
{
	fill_color=in_fill_color;
}

/** Отрисовка фона*/
void VBackgroundElement::Paint(cairo_t * cr)
{
	cairo_set_antialias(cr, antialias);
	//Фон
	CairoSetColor(cr, fill_color);
	cairo_paint(cr);
}

//---------------------------------- Методы класса VTextGraph --------------------------------------

/// Конструктор
VTextGraph::VTextGraph(VDataProviderPtr in_data, VAxesPtr in_axes, TEXT_ORIENTATION in_to, TEXT_POSITION in_tp)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;						
		to=in_to;
		tp=in_tp;		
	}
	else
	{
		throw NULL_POINTER;
	}
}



/// Рисование
void VTextGraph::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, CLIP_HORIZONTAL);
	cairo_set_antialias(cr,antialias);
	CairoSetColor(cr, VColor(0,0,0,255));								
	CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceX(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));		
	double x_prev=(0-DBL_MAX);
	//Параметр												
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{					
		cairo_save(cr);		
		cairo_text_extents_t te;		
		cairo_text_extents(cr, data->GetLabel(i), &te);				
		double x=axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)/2);
		double y=axes->ParamToAreaY(data->GetY(i));	

		if (to==TO_VERTICAL && (tp==TP_AREA_BOTTOM_OVER || tp==TP_AREA_BOTTOM_UNDER || tp==TP_AREA_TOP_OVER || tp==TP_AREA_TOP_UNDER))
		{
			x=std::max<double>(x, x_prev+te.height+axes->DrawingToDeviceX(text_space));
			x_prev=x;
		}
		
		CairoShowText(cr, axes, x, y, data->GetLabel(i), to, tp);
			
		cairo_restore(cr);		
	}
	//Восстанавливаем состояние	
	axes->GetArea()->Restore(cr);
}


//---------------------- Методы класса VLineGraph ----------------------------------------

/** Находит точки, в которой нужно сделать "скачок" (FixMe!!! Неизящное решение)*/
std::vector<point_t> VLineGraph::GetRelativeXY(double in_x1, double in_y1, double in_x2, double in_y2) const
{
	std::vector<point_t> result;
	
	if (axes->GetHeight()>0)
	{		
		double y1=axes->GetOriginY()*axes->GetHeight()*0.01-axes->DrawingToDeviceY(axes->ParamToDrawingY(in_y1-axes->GetStartY()));
		double y2=axes->GetOriginY()*axes->GetHeight()*0.01-axes->DrawingToDeviceY(axes->ParamToDrawingY(in_y2-axes->GetStartY()));		
		double x1=axes->ParamToAreaX(in_x1);
		double x2=axes->ParamToAreaX(in_x2);

		//Смещаем первую и вторую точки так, чтобы первая попала в область.
		if (y1>axes->GetHeight())
		{
			while (y1>axes->GetHeight())
			{
				y1-=axes->GetHeight();
				y2-=axes->GetHeight();
			}
		}
		else if (y1<0)
		{
			while (y1<0)
			{
				y1+=axes->GetHeight();
				y2+=axes->GetHeight();
			}
		}

		//Уравнение прямой отрезка графика
		line_t l(point_t(x1, y1), point_t(x2, y2));

		//Если вторая точка вне области
		//Ищем точки пересечения прямых y=GetHeight(), y=GetHeight()*2 и т.д.
		if (y2>axes->GetHeight())
		{		
			double mult=1.0;
			while (true)
			{					
				point_t p=calc_cross_point(l, line_t(0,1,-axes->GetHeight()*mult));										
				if (p.x>x1 && p.x<x2)
				{
					result.push_back(point_t(p.x, axes->GetHeight()));
					result.push_back(point_t(p.x, 0));						
				}
				else
					break;
				mult+=1.0;
			}			
		}
		else if (y2<0)
		{		
			double mult=0.0;
			while (true)
			{					
				point_t p=calc_cross_point(l, line_t(0,1,-axes->GetHeight()*mult));	
				if (p.x>x1 && p.x<x2)
				{
					result.push_back(point_t(p.x, 0));		
					result.push_back(point_t(p.x, axes->GetHeight()));						
				}
				else
					break;
				mult+=1.0;
			}			
		}		
	}
	return result;
}

/** Конструктор*/
VLineGraph::VLineGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, const VColor& in_fill_color, double in_width, int in_clip)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;
		fill_color=in_fill_color;
		width=in_width;		
		clip=in_clip;			
	}
	else
	{
		throw NULL_POINTER;
	}
};

/** Отрисовка графика*/
void VLineGraph::Paint(cairo_t * cr)
{		
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	
	std::vector<point_group> groups;

	size_t GROUP_SIZE=750;

	point_group group;
	group.points.reserve(GROUP_SIZE);		
		
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{							
		if (group.points.size()==0)
		{
			point_t point(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaY(data->GetY(i)));		
			group.points.push_back(point);
			group.color=data->GetFlags(i)==0 ? color : fill_color;
		}
		else
		{	
			point_t point(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaY(data->GetY(i)));		
			if(point!=group.points.back())
			{
				if (axes->GetRelative())
				{											
					std::vector<point_t> r=GetRelativeXY(data->GetX(i-1), data->GetY(i-1), data->GetX(i), data->GetY(i));
					group.points.insert(group.points.end(), r.begin(), r.end());
				}
				group.points.push_back(point);
			}
		}
		
		if  (color!=fill_color && i!=0 && data->GetFlags(i)!=data->GetFlags(i-1) || 
			group.points.size()>=GROUP_SIZE || 
			i==visible.back())
		{				
			groups.push_back(group);
			group.points.clear();
			group.points.reserve(GROUP_SIZE);
		}
	}
	axes->GetArea()->Apply(cr, clip);
	cairo_set_antialias(cr,antialias);		
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));
	cairo_new_path(cr);
	for (size_t i=0; i<groups.size(); ++i)
	{		
		for (size_t j=0; j<groups[i].points.size(); ++j)
		{
			cairo_line_to(cr, groups[i].points[j].x, groups[i].points[j].y);	
		}
		CairoSetColor(cr, groups[i].color);		
		cairo_stroke(cr);
		cairo_move_to(cr, groups[i].points.back().x, groups[i].points.back().y);
	}		
	axes->GetArea()->Restore(cr);

}

//---------------------- Методы класса VBarGraph ----------------------------------------

/** Конструктор*/
VBarGraph::VBarGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, 
	const VColor& in_fill_color, double in_width, int in_clip, bool in_show_labels)
{
	if (in_data && in_axes)
	{
		data=in_data;		
		axes=in_axes;				
		color=in_color;
		fill_color=in_fill_color;		
		width=in_width;
		clip=in_clip;		
		show_labels=in_show_labels;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VBarGraph::Paint(cairo_t * cr)
{		
	if (data->GetCount()<=0)
		return;
	axes->GetArea()->Apply(cr, clip);
	cairo_set_antialias(cr,antialias);	
	//Определяем диапазон индексов кадров, которые будут отбражены		
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	//Параметр	
	cairo_new_path(cr);	
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));		
	double zero=axes->ParamToAreaY(0);	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{					
		double x1=axes->ParamToAreaX(data->GetX(i));
		double y1=axes->ParamToAreaY(data->GetY(i));
		double x2=axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i));
		double y2=axes->ParamToAreaY(data->GetY(i));						
		if (x1!=x2)
		{
			cairo_rectangle(cr, x1, y1, x2-x1, zero-y1);
			CairoSetColor(cr, fill_color);
			cairo_fill_preserve(cr);
			CairoSetColor(cr, color);
			extents_t e;
			cairo_path_extents(cr, &e.x1, &e.y1, &e.x2, &e.y2);	
			cairo_stroke(cr);
			if (show_labels)
			{
				//Текст			
				cairo_save(cr);
				CairoSetColor(cr, COLOR_HTML_BLACK);
				CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
				std::wstring text=data->GetLabel(i);
				cairo_text_extents_t te;
				cairo_text_extents(cr, text, &te);				
				cairo_move_to(cr, (e.x1+e.x2)/2+te.height/2, (e.y1+e.y2)/2+te.width/2);
				cairo_rotate(cr, -M_PI/2);
				cairo_show_text(cr, text);
				cairo_restore(cr);
			}
		}	
	}
	
	//Восстанавливаем состояние	
	axes->GetArea()->Restore(cr);		
}

//---------------------- Методы класса VSquareGraph ----------------------------------------

/** Конструктор*/
VSquareGraph::VSquareGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, 
	const VColor& in_fill_color, double in_width, int in_clip, bool in_show_labels)
{
	if (in_data && in_axes)
	{
		data=in_data;		
		axes=in_axes;				
		color=in_color;
		fill_color=in_fill_color;		
		width=in_width;
		clip=in_clip;		
		show_labels=in_show_labels;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VSquareGraph::Paint(cairo_t * cr)
{		
	if (data->GetCount()<=0)
		return;
	axes->GetArea()->Apply(cr, clip);
	cairo_set_antialias(cr,antialias);	
	//Определяем диапазон индексов кадров, которые будут отбражены		
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	//Параметр	
	cairo_new_path(cr);	
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));		
	double zero=axes->ParamToAreaY(0);	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{			
		double parameter_x1=data->GetX(i);
		double parameter_y1=data->GetY(i);
		double parameter_x2=data->GetX(i)+data->GetSizeX(i);
		double parameter_y2=data->GetY(i)+data->GetSizeY(i);
		double x1=axes->ParamToAreaX(parameter_x1);
		double y1=axes->ParamToAreaY(parameter_y1);
		double x2=axes->ParamToAreaX(parameter_x2);
		double y2=axes->ParamToAreaY(parameter_y2);								
		cairo_rectangle(cr, x1, y1, (x2-x1)!=0 ? (x2-x1) : 1.0, (y2-y1)!=0 ? (y2-y1) : 1.0);
		CairoSetColor(cr, fill_color);
		cairo_fill_preserve(cr);
		CairoSetColor(cr, color);	
		extents_t e;
		cairo_path_extents(cr, &e.x1, &e.y1, &e.x2, &e.y2);	
		cairo_stroke(cr);		
		if (show_labels)
		{
			//Текст			
			cairo_save(cr);
			CairoSetColor(cr, COLOR_HTML_BLACK);
			CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
			std::wstring text=data->GetLabel(i);
			cairo_text_extents_t te;
			cairo_text_extents(cr, text, &te);							
			cairo_move_to(cr, (e.x1+e.x2)/2+te.height/2, e.y1-axes->DrawingToDeviceY(text_space));
			cairo_rotate(cr, -M_PI/2);
			cairo_show_text(cr, text);
			cairo_restore(cr);
		}
	}	
	//Восстанавливаем состояние	
	axes->GetArea()->Restore(cr);		
}


//---------------------- Методы класса VAreaGraph ----------------------------------------

/** Конструктор*/
VAreaGraph::VAreaGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, 
	const VColor& in_fill_color, double in_width, int in_clip, bool in_show_labels)
{
	if (in_data && in_axes)
	{
		data=in_data;		
		axes=in_axes;				
		color=in_color;
		fill_color=in_fill_color;		
		width=in_width;
		clip=in_clip;		
		show_labels=in_show_labels;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VAreaGraph::Paint(cairo_t * cr)
{		
	axes->GetArea()->Apply(cr, clip);
	cairo_set_antialias(cr,antialias);	
	//Определяем диапазон индексов кадров, которые будут отбражены		
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	//Параметр
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));		
	double zero=axes->ParamToAreaY(0);
	
	cairo_move_to(cr, axes->ParamToAreaX(data->GetX(visible.front())), axes->ParamToAreaY(data->GetY(visible.front())));
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{			
		cairo_line_to(cr, axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaY(data->GetY(i)));
	}
	cairo_line_to(cr, axes->ParamToAreaX(data->GetX(visible.front())), axes->ParamToAreaY(data->GetY(visible.front())));
	CairoSetColor(cr, fill_color);
	cairo_fill_preserve(cr);
	CairoSetColor(cr, color);
	cairo_stroke(cr);
	//Восстанавливаем состояние	
	axes->GetArea()->Restore(cr);		
}


//---------------------- Методы класса VStickGraph ----------------------------------------

/** Конструктор*/
VStickGraph::VStickGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, const VColor& in_fill_color, double in_width, int in_clip, bool in_show_labels)
{
	if (in_data && in_axes)
	{		
		data=in_data;		
		axes=in_axes;				
		color=in_color;
		fill_color=in_fill_color;		
		width=in_width;
		clip=in_clip;
		show_labels=in_show_labels;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VStickGraph::Paint(cairo_t * cr)
{		
	if (data->GetCount()<=0)
		return;
	axes->GetArea()->Apply(cr, clip);
	cairo_set_antialias(cr,antialias);
	//Определяем диапазон индексов кадров, которые будут отображены		
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	//Параметр	
	cairo_new_path(cr);				
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{
		double x=axes->ParamToAreaX(data->GetX(i));		
		double y=axes->ParamToAreaY(data->GetY(i));
		CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));
		data->GetFlags(i)==0 ? CairoSetColor(cr, color) : CairoSetColor(cr, fill_color);
		double zero=axes->ParamToAreaY(0);
		cairo_move_to(cr, x, zero);		
		cairo_line_to(cr, x, y);		
		cairo_stroke(cr);		
		if (show_labels)
		{
			std::wstring text = data->GetLabel(i);
			CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
			cairo_text_extents_t te;
			cairo_text_extents(cr, text, &te);
			if(data->GetY(i)>0)
				cairo_move_to(cr, x+te.height/2, y-axes->DrawingToDeviceX(text_space));
			else
				cairo_move_to(cr, x+te.height/2, y-axes->DrawingToDeviceX(-text_space-0.15));			
			CairoSetColor(cr, COLOR_HTML_BLACK);
			cairo_save(cr);
			cairo_rotate(cr, -M_PI/2);			
			cairo_show_text(cr, text);
			cairo_restore(cr);
		}		
	}
	
	//Восстанавливаем состояние	
	axes->GetArea()->Restore(cr);
}

//--------------------------------------- Методы класса VCoordPointGraph ----------------------------------

/** Конструктор*/
VCoordPointGraph::VCoordPointGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, const VColor& in_fill_color, double in_width, int in_clip)
{
	if (in_data && in_axes)
	{
		data=in_data;		
		axes=in_axes;				
		color=in_color;
		fill_color=in_fill_color;
		width=in_width;		
		clip=in_clip;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VCoordPointGraph::Paint(cairo_t * cr)
{		
	if (data->GetCount()<=0)
		return;
	axes->GetArea()->Apply(cr, clip);
	cairo_set_antialias(cr,antialias);
	//Определяем диапазон индексов кадров, которые будут отображены		
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	//Параметр	
	cairo_new_path(cr);						
	double x;	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{
		x=axes->ParamToAreaX(data->GetX(i));
		double y=axes->ParamToAreaY(data->GetY(i));
		double zero=axes->ParamToAreaY(0);
		std::wstring text = data->GetLabel(i);
		CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));		
		CairoSetColor(cr, color);
		double point_size=axes->DrawingToDeviceX(1.0/10.0);
		cairo_arc(cr, x, y, point_size, 0.0, 2.0*M_PI);
		if(i==data->GetSelectedPoint())
		{
			CairoSetColor(cr, fill_color);							
			cairo_fill_preserve(cr);
			cairo_fill(cr);							
		}
		else
		{
			if(text==L"в")
			{
				CairoSetColor(cr, color);							
				cairo_fill_preserve(cr);
				cairo_fill(cr);
			}
		}
		cairo_stroke(cr);
		CairoSetColor(cr, color);
		cairo_move_to(cr, x, zero);
		if(data->GetY(i)>0)
			cairo_line_to(cr, x, y+point_size);
		if(data->GetY(i)<0)
			cairo_line_to(cr, x, y-point_size);
		cairo_stroke(cr);
	}
	
	//Восстанавливаем состояние	
	axes->GetArea()->Restore(cr);	
}

/// Обработчик событий
bool VCoordPointGraph::ProcessEvent(const VEvent& in_event)
{
	bool result=false;
	switch (in_event.GetCode())
	{
		case EVENT_MOUSE_MOTION:			
		{
			data->DeselectPoint();
			double x=((VMouseEvent&)in_event).GetX();
			double x_mouse=axes->GetArea()->ScreenToAreaX(x);
			for (size_t i=0; i < data->GetCount(); ++i)
			{
				double x_i = axes->ParamToAreaX(data->GetX(i));
				double y_i = axes->ParamToAreaY(data->GetY(i));
				if(x_i-4 < x_mouse && x_i+4 > x_mouse)
				{
					data->SelectPoint(i);
					result=true;
				}
			}
			break;		
		}
		case EVENT_MOUSE_LEFT_UP:
		{
			if(data->GetSelectedPoint()!=BAD_INDEX)
			{
				data->RefreshData();
				result=true;
				break;
			}
		}
		case EVENT_MOUSE_RIGHT_UP:
		{
			result=true;
			break;
		}

	}
	return result;
}

//----------------------------------- Методы класса VPointElement -----------------------------------

/** Конструктор*/
VPointElement::VPointElement(VDataProviderPtr in_data, VAxesPtr in_axes, size_t in_index, 
	const VColor& in_color,  const VColor& in_fill_color, int in_clip, bool in_show_label, 
	point_style_t in_style, TEXT_ORIENTATION in_to, TEXT_POSITION in_tp)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		index=in_index;
		clip=in_clip;
		ext_left=0;	
		ext_top=0;	
		ext_right=0;	
		ext_bottom=0;
		hit=false;	
		show_label=in_show_label;
		to=in_to;
		tp=in_tp;
		style=in_style;
		drag=false;
		color=in_color;
		fill_color=in_fill_color;
		width=default_line_width;
	}
	else
	{
		throw NULL_POINTER;
	}
};

/** Рисование*/
void VPointElement::Paint(cairo_t * cr)
{	
	if (index<data->GetCount() && data->GetX(index)>axes->GetMinX() && data->GetX(index)<axes->GetMaxX())
	{					
		axes->GetArea()->Apply(cr, clip);
		cairo_set_antialias(cr,antialias);		
		CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));
		double x=axes->ParamToAreaX(data->GetX(index));
		double y=axes->ParamToAreaY(data->GetY(index));
		//В зависимости от стиля
		switch (style)
		{
			case 0://Кружок
			{				
				double point_size=axes->DrawingToDeviceX(1.0/10.0);		
				cairo_arc(cr, x, y, point_size, 0.0, 2.0*M_PI);
				CairoSetColor(cr, fill_color);							
				cairo_fill_preserve(cr);				
				CairoSetColor(cr, color);						
				cairo_path_extents(cr, &ext_left, &ext_top, &ext_right, &ext_bottom);
				cairo_user_to_device(cr, &ext_left, &ext_top);
				cairo_user_to_device(cr, &ext_right, &ext_bottom);
				if (hit && data->GetActive()) //Когда мышь над точкой, подсвечиваем ее
					cairo_fill(cr);
				else
					cairo_stroke(cr);
				//Выводим метку точки
				if (show_label)
				{			
					cairo_save(cr);
					CairoSetColor(cr, COLOR_HTML_BLACK);
					CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
					CairoShowText(cr, axes, x, y, data->GetLabel(index), to, tp, 2*text_space);					
					cairo_restore(cr);
				}
				break;
			}
			case 1://Флажок
			{
				double flag_size=axes->DrawingToDeviceX(2.0/10.0);

				//Рисуем "флагшток"
				CairoSetColor(cr, color);
				cairo_move_to(cr, x, y-flag_size);
				cairo_line_to(cr, x, y);
				cairo_stroke(cr);

				//Рисуем флаг
				if (data->GetCount()>1 && index==0)//Флаг направлен вправо
				{					
					cairo_move_to(cr, x, y-flag_size);
					cairo_line_to(cr, x, y-flag_size*2);
					cairo_line_to(cr, x+flag_size, y-flag_size*2);
					cairo_close_path(cr);
				}
				else if (data->GetCount()>1 && index==data->GetCount()-1) //Флаг направлен влево
				{
					cairo_move_to(cr, x, y-flag_size);
					cairo_line_to(cr, x-flag_size, y-flag_size*2);
					cairo_line_to(cr, x, y-flag_size*2);
					cairo_close_path(cr);
				}
				else
				{
					cairo_move_to(cr, x, y-flag_size);
					cairo_line_to(cr, x-flag_size/2, y-flag_size*2);
					cairo_line_to(cr, x+flag_size/2, y-flag_size*2);
					cairo_close_path(cr);
				}

				cairo_path_extents(cr, &ext_left, &ext_top, &ext_right, &ext_bottom);
				cairo_user_to_device(cr, &ext_left, &ext_top);
				cairo_user_to_device(cr, &ext_right, &ext_bottom);
				if (hit && data->GetActive())
				{
					CairoSetColor(cr, color);
					cairo_fill_preserve(cr);
					CairoSetColor(cr, color);
					cairo_stroke(cr);					
				}
				else if (data->GetActive())
				{
					CairoSetColor(cr, fill_color);
					cairo_fill_preserve(cr);
					CairoSetColor(cr, color);
					cairo_stroke(cr);					
				}
				else
				{
					CairoSetColor(cr, color);
					cairo_stroke(cr);
				}
				break;
			}
		}		
		axes->GetArea()->Restore(cr);			

	}	
};

/** Обработчик HitTest*/
bool VPointElement::HitTest(double in_x, double in_y)
{
	return in_x>ext_left && in_x<ext_right && in_y>ext_top && in_y<ext_bottom;	
}

/** Обработка события*/
bool VPointElement::ProcessEvent(const VEvent& in_event)
{	
	bool result = false;
	if (data->GetActive() && GetActive())
	{
		switch (in_event.GetCode())
		{
			case EVENT_MOUSE_MOTION:
			{
				double x=((VMouseEvent&)in_event).GetX();
				double y=((VMouseEvent&)in_event).GetY();				
				hit=HitTest(x, y);
				if (hit_changed.put(hit))				
					hit ? data->SelectPoint(index) : data->DeselectPoint();				
				result = true;								
				if (drag)
				{
					data->SetXY(index, axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(x)) - drag_shift_x,
									   axes->AreaToParamY(axes->GetArea()->ScreenToAreaY(y)) - drag_shift_y);					
				}
				break;
			}
			case EVENT_MOUSE_LEFT_DOWN:			
			{				
				if (hit)
				{		
					double x=((VMouseEvent&)in_event).GetX();
					double y=((VMouseEvent&)in_event).GetY();
					drag_shift_x=axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(x)) - data->GetX(index);
					drag_shift_y=axes->AreaToParamY(axes->GetArea()->ScreenToAreaY(y)) - data->GetY(index);
					drag=true;
					result = true;
				}
				break;
			}
			case EVENT_MOUSE_LEFT_UP:			
			{				
				drag=false;
				result = true;
				break;
			}
			case EVENT_MOUSE_MIDDLE_DOWN:
			{
				if (hit)
				{
					data->DeleteItem(index);
					result = true;				
				}
				break;
			}
			default:
				break;
		}
	}
	return result;
}

//------------------------------------- Методы класса VPointGraph -----------------------------------

/** Конструктор*/
VPointGraph::VPointGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  
	const VColor& in_fill_color, int in_clip, bool in_show_labels, point_style_t in_style,
	TEXT_ORIENTATION in_to, TEXT_POSITION in_tp)
{	
	if (in_data && in_axes)
	{
		data=in_data;	
		axes=in_axes;
		color=in_color;
		fill_color=in_fill_color;
		style=in_style;
		clip=in_clip;		
		show_labels=in_show_labels;
		to=in_to;
		tp=in_tp;
	}
	else
	{
		throw NULL_POINTER;
	}
};

/** Рисование*/
void VPointGraph::Paint(cairo_t * cr)
{			
	//Определяем диапазон индексов кадров, которые будут отбражены		
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;	
	//Добавляем элемент для каждой точки массива данных
	if (data->GetCount()!=points.size())
	{
		points.clear();
		points.reserve(data->GetCount());
		points.resize(data->GetCount());
		for (size_t i=0; i<data->GetCount(); ++i)
		{				
			points.at(i)=VPointElementPtr(new VPointElement(data, axes, i, color, fill_color, clip, show_labels, style, to, tp));
		}
	}	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{		
		if (i<points.size())
			points.at(i)->Render(cr);
	}	
}

/** Обработка hit-test*/
bool VPointGraph::HitTest(double in_x, double in_y)
{
	for (size_t i=0; i<points.size(); ++i)
	{		
		if (points.at(i)->HitTest(in_x, in_y))
			return true;
	}	
	return false;
}

/** Обработка события*/
bool VPointGraph::ProcessEvent(const VEvent& in_event)
{			
	if (GetActive())
	{
		switch (in_event.GetCode())
		{
			case EVENT_MOUSE_LEFT_DOWN:		
			{			
				bool result=false;
				for (point_itr i=points.begin(); i!=points.end(); ++i)
				{
					bool changed=(*i)->ProcessEvent(in_event);
					result = result || changed;
					if (result)
						return result;
				}
				if (!result)
				{
					double x=axes->GetArea()->ScreenToAreaX(((VMouseEvent&)in_event).GetX());
					double y=axes->GetArea()->ScreenToAreaY(((VMouseEvent&)in_event).GetY());
					//Если точка в границах области
					if (x>0 && x<axes->GetArea()->GetWidth() && y>0 && y<axes->GetArea()->GetHeight())
					{
						data->AddXY(0, axes->AreaToParamX(x), axes->AreaToParamY(y));					
					}
				}
				return result;						
			}				
			default:
			{
				bool result=false;
				for (point_itr i=points.begin(); i!=points.end(); ++i)
				{
					bool changed=(*i)->ProcessEvent(in_event);
					result = result || changed;
				}
				return result;
				break;
			}
		}
	}
	return false;
}


//---------------------- Методы класса VPointStickGraph ----------------------------------------

/** Конструктор*/
VPointStickGraph::VPointStickGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, const VColor& in_fill_color, double in_width, int in_clip, bool in_show_labels)
{
	if (in_data && in_axes)
	{
		sticks = VStickGraphPtr(new VStickGraph(in_data, in_axes, in_color, in_fill_color, in_width, in_clip, false));
		points = VPointGraphPtr(new VPointGraph(in_data, in_axes, in_color, in_fill_color, in_clip, in_show_labels, 
			POINT_STYLE_CIRCLE, TO_VERTICAL, TP_OVER));
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VPointStickGraph::Paint(cairo_t * cr)
{		
	sticks->Paint(cr);
	points->Paint(cr);
}

//---------------------- Методы класса VPointLineGraph ----------------------------------------

/// Конструктор
VPointLineGraph::VPointLineGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, 
			const VColor& in_fill_color, double in_width, int in_clip, bool in_show_labels, 
			point_style_t in_point_style)
{
	if (in_data && in_axes)
	{
		lines = VLineGraphPtr(new VLineGraph(in_data, in_axes, in_color, in_color, in_width, in_clip));
		points = VPointGraphPtr(new VPointGraph(in_data, in_axes, in_color, in_fill_color, in_clip,
			in_show_labels, in_point_style, TO_VERTICAL, TP_UNDER));
	}
	else
	{
		throw NULL_POINTER;
	}
}

/// Рисование
void VPointLineGraph::Paint(cairo_t * cr)
{		
	lines->Paint(cr);
	points->Paint(cr);
}

/// Обработка hit-test
bool VPointLineGraph::HitTest(double in_x, double in_y)
{
	return points->HitTest(in_x, in_y);
}
/// Обработка события
bool VPointLineGraph::ProcessEvent(const VEvent& in_event)
{
	return points->ProcessEvent(in_event);
}

//---------------------------------- Методы класса VProfileObjectsElement --------------------------------------

/** Конструктор*/
VProfileObjectsElement::VProfileObjectsElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, int in_clip)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;
		fill_color=in_fill_color;		
		clip=in_clip;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VProfileObjectsElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, clip);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));	
	CairoSetColor(cr, color);
	//Области занятые значками	
	std::vector<double> x;
	std::vector<double> y;
	std::vector<double> x_positions;
	std::vector<double> y_positions;
	x.reserve(data->GetCount());
	x.resize(data->GetCount());
	y.reserve(data->GetCount());
	y.resize(data->GetCount());
	x_positions.reserve(data->GetCount());
	x_positions.resize(data->GetCount());
	y_positions.reserve(data->GetCount());
	y_positions.resize(data->GetCount());
	//Нужно сначала вычислить точки в которых будут отображаться значки, так чтобы они не накладывались	
	//минимальное расстояние между объектами (10 мм)
	double space=axes->DrawingToDeviceX(10.0/10.0);
	double prev_x_over=DBL_MAX;
	double prev_y_over=DBL_MAX;
	double prev_x_under=DBL_MAX;
	double prev_y_under=DBL_MAX;
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{			
		x.at(i)=axes->ParamToAreaX(data->GetX(i));
		y.at(i)=axes->ParamToAreaY(data->GetY(i));
		x_positions.at(i)=axes->ParamToAreaX(data->GetX(i));
		y_positions.at(i)=axes->ParamToAreaY(data->GetY(i));
		
		if (GetObjectGlyph(data, i)->IsObjectOverProfile())
		{			
			if (prev_x_over!=DBL_MAX && x_positions.at(i)-prev_x_over < space)
			{
				x_positions.at(i)=prev_x_over+space;
				//Необходимо пересчитать также и координату Y				
				y_positions.at(i)-=space;				
			}
			prev_x_over=x_positions.at(i);
			prev_y_over=y_positions.at(i);
		}		
	}
	//... и лишь после этого отрисовать их		
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{				
		cairo_move_to(cr, x.at(i), y.at(i));
		cairo_line_to(cr, x_positions.at(i), y_positions.at(i));
		cairo_stroke(cr);				
		cairo_save(cr);
		cairo_translate(cr, x_positions.at(i), y_positions.at(i));
		extents_t e=GetObjectGlyph(data, i)->DrawProfile(cr, axes);
		cairo_translate(cr, 0, e.y2);
		show_object_text(cr, axes, data->GetLabel(i), GetObjectGlyph(data,i)->ShowName(), GetObjectGlyph(data,i)->ShowCoord());
		cairo_restore(cr);		
	}
	axes->GetArea()->Restore(cr);
}

//---------------------------------- Методы класса VSituationObjectsElement --------------------------------------

/** Конструктор*/
VSituationObjectsElement::VSituationObjectsElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, int in_clip)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;
		fill_color=in_fill_color;		
		clip=in_clip;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VSituationObjectsElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, clip);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));	
	CairoSetColor(cr, color);	

	double min_x=axes->ParamToAreaX(data->GetMinX());
	double max_x=axes->ParamToAreaX(data->GetMaxX());

	cairo_move_to(cr, min_x, axes->GetArea()->GetHeight()/2);
	cairo_line_to(cr, max_x, axes->GetArea()->GetHeight()/2);
	cairo_stroke(cr);

	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{					
		double x=axes->ParamToAreaX(data->GetX(i));
		double y=axes->GetArea()->GetHeight()/2;				
		cairo_save(cr);		
		cairo_translate(cr, x, y);		
		GetObjectGlyph(data, i)->DrawSituation(cr, axes);	
		cairo_restore(cr);
	}
	axes->GetArea()->Restore(cr);
}


//---------------------- Методы класса VKVLPOverstepElement ----------------------------------------

/** Конструктор*/
VKVLPOverstepElement::VKVLPOverstepElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, 
	const VColor& in_fill_color, double in_width, int in_clip)
{
	if (in_data && in_axes)
	{
		data=in_data;		
		axes=in_axes;				
		color=in_color;
		fill_color=in_fill_color;		
		width=in_width;
		clip=in_clip;				
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VKVLPOverstepElement::Paint(cairo_t * cr)
{		
	if (data->GetCount()<=0)
		return;
	axes->GetArea()->Apply(cr, clip);
	cairo_set_antialias(cr,antialias);	
	//Определяем диапазон индексов кадров, которые будут отбражены		
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	//Параметр	
	cairo_new_path(cr);	
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));		
	double zero=axes->ParamToAreaY(0);	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{					
		double ysize=0.0;
		switch (int(data->GetSizeY(i)))
		{
			default:				
				break;
			case DEGREE2:
				ysize=0.5;
				break;
			case DEGREE3:
				ysize=1.0;
				break;
			case DEGREE4:
				ysize=2.5;
				break;
		}
		double x1=axes->ParamToAreaX(data->GetX(i));
		double y1=axes->ParamToAreaY(axes->GetStartY())-axes->DrawingToDeviceY(ysize/10.0);
		double x2=axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i));
		double y2=axes->ParamToAreaY(axes->GetStartY())+axes->DrawingToDeviceY(ysize/10.0);			
		cairo_rectangle(cr, x1, y1, x2-x1, y2-y1);
		if (data->GetFlags(i)!=0)
		{
			//Исключенные отступления отображаем на ленте без закрашивания
			CairoSetColor(cr, fill_color);			
			cairo_stroke(cr);
		}
		else
		{
			CairoSetColor(cr, fill_color);
			cairo_fill_preserve(cr);
			CairoSetColor(cr, color);			
			cairo_stroke(cr);
		}
	}	
	//Восстанавливаем состояние	
	axes->GetArea()->Restore(cr);	
}

//---------------------------------- Меторды класса VKVLPAnpElement --------------------------------------

/** Конструктор*/
VKVLPAnpElement::VKVLPAnpElement(VDataProviderPtr in_data, VAxesPtr in_axes)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
	}
	else
	{
		throw NULL_POINTER;
	}		
}

/** Риcование*/	
void VKVLPAnpElement::Paint(cairo_t * cr)
{
	if (data->GetCount()<=0)
		return;
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);	
	//Определяем диапазон индексов кадров, которые будут отбражены		
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	//Параметр	
	cairo_new_path(cr);			
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{			
		double x=axes->ParamToAreaX(data->GetX(i));
		double y=axes->GetArea()->GetHeight()-axes->DrawingToDeviceY(text_space);
		double h=axes->DrawingToDeviceY(2.0/10.0);
		cairo_move_to(cr, x, y);
		cairo_line_to(cr, x,y-h);
		cairo_line_to(cr, x+h/2,y-h/2);
		cairo_line_to(cr, x, y);
		CairoSetColor(cr, COLOR_HTML_BLACK);
		cairo_fill(cr);		
	}	
	//Восстанавливаем состояние	
	axes->GetArea()->Restore(cr);		
}

//---------------------------------- Меторды класса VKVLPPsiElement --------------------------------------

/** Конструктор*/
VKVLPPsiElement::VKVLPPsiElement(VDataProviderPtr in_data, VAxesPtr in_axes)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
	}
	else
	{
		throw NULL_POINTER;
	}		
}

/** Риcование*/	
void VKVLPPsiElement::Paint(cairo_t * cr)
{
	if (data->GetCount()<=0)
		return;
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);	
	//Определяем диапазон индексов кадров, которые будут отображены		
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	//Параметр	
	cairo_new_path(cr);			
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{			
		double x=axes->ParamToAreaX(data->GetX(i));		
		double h=axes->DrawingToDeviceY(1.0/10.0);
		double y=axes->GetArea()->GetHeight()-axes->DrawingToDeviceY(text_space)-h;

		cairo_move_to(cr, x, y-h);
		cairo_line_to(cr, x, y+h);

		cairo_move_to(cr, x-h, y-h);
		cairo_line_to(cr, x+h, y+h);

		cairo_move_to(cr, x+h, y-h);
		cairo_line_to(cr, x-h, y+h);

		cairo_move_to(cr, x-h, y);
		cairo_line_to(cr, x+h, y);
		
		CairoSetColor(cr, COLOR_HTML_BLACK);
		cairo_stroke(cr);		
	}	
	//Восстанавливаем состояние	
	axes->GetArea()->Restore(cr);		
}


//---------------------------------- Меторды класса VKVLPOtvodWidthElement --------------------------------------

/** Конструктор*/
VKVLPOtvodWidthElement::VKVLPOtvodWidthElement(VDataProviderPtr in_data, VAxesPtr in_axes)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
	}
	else
	{
		throw NULL_POINTER;
	}		
}

/** Риcование*/	
void VKVLPOtvodWidthElement::Paint(cairo_t * cr)
{
	if (data->GetCount()<=0)
		return;
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);	
	//Определяем диапазон индексов кадров, которые будут отбражены		
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	//Параметр	
	cairo_new_path(cr);			
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{			
		double x=axes->ParamToAreaX(data->GetX(i));		
		double h=axes->DrawingToDeviceY(1.0/10.0);
		double y=axes->GetArea()->GetHeight()-axes->DrawingToDeviceY(text_space);

		cairo_move_to(cr, x-h, y);
		cairo_line_to(cr, x, y-h);		
		cairo_line_to(cr, x+h, y);
		
		CairoSetColor(cr, COLOR_HTML_BLACK);
		cairo_stroke(cr);		
	}	
	//Восстанавливаем состояние	
	axes->GetArea()->Restore(cr);		
}


//----------------------------------- Методы класса VKVLPSwitchesElement --------------------------------

/** Конструктор*/
VKVLPSwitchesElement::VKVLPSwitchesElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, 
	const VColor& in_fill_color, double in_width, int in_clip)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;
		width=in_width;
		fill_color=in_fill_color;		
		clip=in_clip;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VKVLPSwitchesElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());	
	if (visible.size()<2)
		return;

	axes->GetArea()->Apply(cr, clip);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));	
	CairoSetColor(cr, color);		
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{	
		double x3=axes->ParamToAreaX(data->GetX(i));
		double r=axes->DrawingToDeviceX(0.5/10.0);
		double h=axes->DrawingToDeviceY(2.0/10.0);
		double y=4.0*axes->GetArea()->GetHeight()/10.0;	
		
		// Отображаем положение крестовины
		cairo_move_to(cr, x3, y+h);
		cairo_line_to(cr, x3, y-h);
		
		// Отображаем ориентацию если указана
		switch (data->GetFlags(i))
		{			
			case SW_LEFT_ALONG:
			{
				cairo_move_to(cr, x3, y+h);
				cairo_line_to(cr, x3+r, y+h/2);
				cairo_move_to(cr, x3, y+h);
				cairo_line_to(cr, x3-r, y+h/2);
				cairo_move_to(cr, x3, y-h);
				cairo_line_to(cr, x3-3*r, y-3*h/2);
				break;
			}
			case SW_LEFT_COUNTER:
			{
				cairo_move_to(cr, x3, y+h);
				cairo_line_to(cr, x3+r, y+h/2);
				cairo_move_to(cr, x3, y+h);
				cairo_line_to(cr, x3-r, y+h/2);
				cairo_move_to(cr, x3, y-h);
				cairo_line_to(cr, x3+3*r, y-3*h/2);
				break;
			}
			case SW_RIGHT_ALONG:
			{
				cairo_move_to(cr, x3, y-h);
				cairo_line_to(cr, x3+r, y-h/2);
				cairo_move_to(cr, x3, y-h);
				cairo_line_to(cr, x3-r, y-h/2);
				cairo_move_to(cr, x3, y+h);
				cairo_line_to(cr, x3-3*r, y+3*h/2);
				break;
			}
			case SW_RIGHT_COUNTER:
			{
				cairo_move_to(cr, x3, y-h);
				cairo_line_to(cr, x3+r, y-h/2);
				cairo_move_to(cr, x3, y-h);
				cairo_line_to(cr, x3-r, y-h/2);
				cairo_move_to(cr, x3, y+h);
				cairo_line_to(cr, x3+3*r, y+3*h/2);
				break;
			}
			default:
			{
				cairo_move_to(cr, x3, y+h);
				cairo_line_to(cr, x3+r, y+h/2);
				cairo_move_to(cr, x3, y+h);
				cairo_line_to(cr, x3-r, y+h/2);
				cairo_move_to(cr, x3, y-h);
				cairo_line_to(cr, x3+r, y-h/2);
				cairo_move_to(cr, x3, y-h);
				cairo_line_to(cr, x3-r, y-h/2);
				break;
			}
		}
		cairo_stroke(cr);		
	}
	axes->GetArea()->Restore(cr);
}

//----------------------------------- Методы класса VKVLPSwitchSectionsElement --------------------------------

/** Конструктор*/
VKVLPSwitchSectionsElement::VKVLPSwitchSectionsElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, 
	const VColor& in_fill_color, double in_width, int in_clip)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;
		width=in_width;
		fill_color=in_fill_color;		
		clip=in_clip;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VKVLPSwitchSectionsElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());	
	if (visible.size()<2)
		return;

	axes->GetArea()->Apply(cr, clip);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));	
	CairoSetColor(cr, color);		
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{			
		double x1=std::min<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double x2=std::max<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));		
		double r=axes->DrawingToDeviceX(0.5/10.0);
		double h=axes->DrawingToDeviceY(2.0/10.0);
		double y=4.0*axes->GetArea()->GetHeight()/10.0;	
		
		// Отображаем участок стрелки
		cairo_move_to(cr, x1, y);
		cairo_line_to(cr, x2, y);	
		
		cairo_stroke(cr);	

		std::wstring label=data->GetLabel(i);
		CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
		cairo_text_extents_t te;
		cairo_text_extents(cr, label, &te);
		cairo_move_to(cr, (x1+x2)/2, te.height+axes->DrawingToDeviceX(text_space));
		cairo_show_text(cr, label);
	}
	axes->GetArea()->Restore(cr);
}


//---------------------------------------- Методы класса VKVLPPassportStatusElement ------------------------------------

/** Конструктор*/
VKVLPPassportStatusElement::VKVLPPassportStatusElement(VDataProviderPtr in_data, VAxesPtr in_axes, int in_clip)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;		
		clip=in_clip;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VKVLPPassportStatusElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());	
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, clip);
	cairo_set_antialias(cr,antialias);		
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{			
		double x=axes->ParamToAreaX(data->GetX(i));
		double h=axes->DrawingToDeviceY(0.6/10.0);
		if (data->GetFlags(i)==0)
		{
			cairo_move_to(cr, x, 0);
			cairo_line_to(cr, x, h);
		}
	}
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));
	CairoSetColor(cr, COLOR_HTML_BLACK);
	cairo_stroke(cr);
	axes->GetArea()->Restore(cr);
}

//------------------------------------ Методы класса VKVLPBridgeElement ------------------------------

/** Конструктор*/
VKVLPBridgeElement::VKVLPBridgeElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VKVLPBridgeElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));	
	CairoSetColor(cr, color);	
	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{					
		double x1=std::min<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double x2=std::max<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double y=4.0*axes->GetArea()->GetHeight()/10.0;
		double r=axes->DrawingToDeviceY(0.5/10.0);
		double h=axes->DrawingToDeviceY(1.5/10.0);
		cairo_move_to(cr, x1-r, y+h+r);
		cairo_line_to(cr, x1, y+h);
		cairo_line_to(cr, x2, y+h);
		cairo_line_to(cr, x2+r, y+h+r);
		
		cairo_move_to(cr, x1-r, y-h-r);
		cairo_line_to(cr, x1, y-h);
		cairo_line_to(cr, x2, y-h);
		cairo_line_to(cr, x2+r, y-h-r);

		cairo_stroke(cr);

	}
	axes->GetArea()->Restore(cr);
}

//------------------------------------ Методы класса VKVLPTunnelElement ------------------------------

/** Конструктор*/
VKVLPTunnelElement::VKVLPTunnelElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VKVLPTunnelElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));	
	CairoSetColor(cr, color);	
	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{					
		double x1=std::min<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double x2=std::max<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double y=4.0*axes->GetArea()->GetHeight()/10.0;
		double r=axes->DrawingToDeviceY(0.5/10.0);
		double h=axes->DrawingToDeviceY(1.5/10.0);
		cairo_move_to(cr, x1-r, y+h-r);
		cairo_line_to(cr, x1, y+h);
		cairo_line_to(cr, x2, y+h);
		cairo_line_to(cr, x2+r, y+h-r);
		
		cairo_move_to(cr, x1-r, y-h+r);
		cairo_line_to(cr, x1, y-h);
		cairo_line_to(cr, x2, y-h);
		cairo_line_to(cr, x2+r, y-h+r);

		cairo_stroke(cr);

	}
	axes->GetArea()->Restore(cr);
}




//------------------------------------ Методы класса VKVLPBridgeAreaElement ------------------------------

/** Конструктор*/
VKVLPBridgeAreaElement::VKVLPBridgeAreaElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VKVLPBridgeAreaElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));	
	CairoSetColor(cr, color);	
	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{					
		double x1=std::min<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double x2=std::max<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double y=4.0*axes->GetArea()->GetHeight()/10.0;
		double r=axes->DrawingToDeviceY(0.5/10.0);
		double h=axes->DrawingToDeviceY(1.5/10.0);
		cairo_move_to(cr, x1, y+h);
		cairo_line_to(cr, x2, y+h);		
				
		cairo_move_to(cr, x1, y-h);
		cairo_line_to(cr, x2, y-h);		

		cairo_stroke(cr);

	}
	axes->GetArea()->Restore(cr);
}

//------------------------------------ Методы класса VKVLPChamferedJointElement ------------------------------

/** Конструктор*/
VKVLPChamferedJointElement::VKVLPChamferedJointElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VKVLPChamferedJointElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));	
	CairoSetColor(cr, color);	
	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{					
		double x1=std::min<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double x2=std::max<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double y=4.0*axes->GetArea()->GetHeight()/10.0;
		double h=axes->DrawingToDeviceY(0.6/10.0);
		cairo_move_to(cr, x1, y+h);
		cairo_line_to(cr, x2, y+h);		
				
		cairo_move_to(cr, x1, y-h);
		cairo_line_to(cr, x2, y-h);		

		cairo_stroke(cr);

	}
	axes->GetArea()->Restore(cr);
}



//------------------------------------ Методы класса VKVLPCrossingElement ------------------------------

/** Конструктор*/
VKVLPCrossingElement::VKVLPCrossingElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VKVLPCrossingElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));	
	CairoSetColor(cr, color);	
	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{					
		double x1=std::min<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double x2=std::max<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double y=4.0*axes->GetArea()->GetHeight()/10.0;
		double r=axes->DrawingToDeviceY(0.5/10.0);
		double h=axes->DrawingToDeviceY(1.5/10.0);
		cairo_move_to(cr, x1-r, y+h+r);
		cairo_line_to(cr, x1, y+h);
		cairo_line_to(cr, x1, y-h);
		cairo_line_to(cr, x1-r, y-h-r);
		
		cairo_move_to(cr, x2+r, y+h+r);
		cairo_line_to(cr, x2, y+h);
		cairo_line_to(cr, x2, y-h);
		cairo_line_to(cr, x2+r, y-h-r);

		cairo_stroke(cr);

	}
	axes->GetArea()->Restore(cr);
}

//------------------------------------ Методы класса VKVLPSpalaElement ------------------------------

/** Конструктор*/
VKVLPSpalaElement::VKVLPSpalaElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  
		double in_width, int in_clip)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;
		width=in_width;
		clip=in_clip;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VKVLPSpalaElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, clip);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));	
	CairoSetColor(cr, color);	
	

	double y=4.0*axes->GetArea()->GetHeight()/10.0;
	double h=axes->DrawingToDeviceY(0.6/10.0);
	double step=axes->DrawingToDeviceX(3.0/10.0);	

	cairo_move_to(cr, 0, y+h);
	cairo_line_to(cr, axes->GetArea()->GetWidth(), y+h);
	cairo_move_to(cr, 0, y-h);
	cairo_line_to(cr, axes->GetArea()->GetWidth(), y-h);
	cairo_stroke(cr);
		

	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{			
		double x1=axes->ParamToAreaX(data->GetX(i));
		double x2=axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i));				
		if (x2<x1) 
			//std::swap<double>(x1,x2);
			std::swap(x1,x2);
		switch (data->GetFlags(i))
		{
			default:
			case SPALA_GB:
			{				
				double x=x1+axes->DrawingToDeviceX(0.02);				
				while (x<x2-axes->DrawingToDeviceX(0.02))
				{
					cairo_move_to(cr, x-axes->DrawingToDeviceX(0.02), y+h);
					cairo_line_to(cr, x-axes->DrawingToDeviceX(0.02), y-h);
					cairo_move_to(cr, x+axes->DrawingToDeviceX(0.02), y+h);
					cairo_line_to(cr, x+axes->DrawingToDeviceX(0.02), y-h);					
					x+=step;
				}
				cairo_stroke(cr);
				break;
			}
			case SPALA_GB96:
			{				
				double x=x1+axes->DrawingToDeviceX(0.04);
				while (x<x2-axes->DrawingToDeviceX(0.04))
				{
					cairo_move_to(cr, x-axes->DrawingToDeviceX(0.04), y+h);
					cairo_line_to(cr, x-axes->DrawingToDeviceX(0.04), y-h);
					cairo_move_to(cr, x, y+h);
					cairo_line_to(cr, x, y-h);
					cairo_move_to(cr, x+axes->DrawingToDeviceX(0.04), y+h);
					cairo_line_to(cr, x+axes->DrawingToDeviceX(0.04), y-h);
					x+=step;
				}
				cairo_stroke(cr);
				break;
			}
			case SPALA_WOOD:
			{				
				double x=x1;
				while (x<x2)
				{
					cairo_move_to(cr, x, y+h);
					cairo_line_to(cr, x, y-h);					
					x+=step;
				}
				cairo_stroke(cr);
				break;
			}
		}
	}
	axes->GetArea()->Restore(cr);
}

//---------------------------------- Методы класса VKVLPStraightSideElement ---------------------------------

/** Конструктор*/
VKVLPStraightSideElement::VKVLPStraightSideElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, double in_width)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;		
		width=in_width;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VKVLPStraightSideElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));	
	CairoSetColor(cr, color);	
	
	double y=4.0*axes->GetArea()->GetHeight()/10.0;
	double h=axes->DrawingToDeviceY(0.6/10.0);
	double step=axes->DrawingToDeviceX(1.0/10.0);
	
	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{			
		double x1=axes->ParamToAreaX(data->GetX(i));
		double x2=axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i));				
		if (x2<x1)
			std::swap(x1,x2);
			//std::swap<double>(x1,x2);
		switch (data->GetFlags(i))
		{
			default:
			case SD_LEFT:
			{
				
				double x=x1;				
				while (x<x2)
				{
					cairo_move_to(cr, x, y-h);
					cairo_line_to(cr, x, y-2*h);					
					x+=step;
				}
				cairo_stroke(cr);
				break;
			}
			case SD_RIGHT:
			{		
				double x=x1;
				while (x<x2)
				{					
					cairo_move_to(cr, x, y+h);
					cairo_line_to(cr, x, y+2*h);
					x+=step;
				}
				cairo_stroke(cr);
				break;
			}			
		}
	}
	axes->GetArea()->Restore(cr);
}


//------------------------------------ Методы класса VCrossingElement ------------------------------

/** Конструктор*/
VCrossingElement::VCrossingElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VCrossingElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));	
	CairoSetColor(cr, color);	
	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{					
		double x1=std::min<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double x2=std::max<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double y=axes->GetArea()->GetHeight();
		double r=y/5;//axes->DrawingToDeviceY(1/10.0);
		cairo_move_to(cr, x1-r, 0);
		cairo_line_to(cr, x1, r);
		cairo_line_to(cr, x1, y-r);
		cairo_line_to(cr, x1-r, y);
		
		cairo_move_to(cr, x2+r, 0);
		cairo_line_to(cr, x2, r);
		cairo_line_to(cr, x2, y-r);
		cairo_line_to(cr, x2+r, y);

		cairo_stroke(cr);

	}
	axes->GetArea()->Restore(cr);
}

//------------------------------------ Методы класса VBridgeElement ------------------------------

/** Конструктор*/
VBridgeElement::VBridgeElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VBridgeElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));	
	CairoSetColor(cr, color);	
	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{					
		double x1=std::min<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double x2=std::max<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double y=axes->GetArea()->GetHeight();
		double r=y/5;//axes->DrawingToDeviceY(1/10.0);
		cairo_move_to(cr, x1-r, 0);				
		cairo_line_to(cr, x1, r);
		cairo_line_to(cr, x2, r);
		cairo_line_to(cr, x2+r, 0);
		
		cairo_move_to(cr, x1-r, y);				
		cairo_line_to(cr, x1, y-r);
		cairo_line_to(cr, x2, y-r);
		cairo_line_to(cr, x2+r, y);
		

		cairo_stroke(cr);

	}
	axes->GetArea()->Restore(cr);
}

//------------------------------------ Методы класса VTunnelElement ------------------------------

/** Конструктор*/
VTunnelElement::VTunnelElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VTunnelElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));	
	CairoSetColor(cr, color);	
	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{					
		double x1=std::min<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double x2=std::max<double>(axes->ParamToAreaX(data->GetX(i)), axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i)));
		double y=axes->GetArea()->GetHeight();
		double r=y/5;//axes->DrawingToDeviceY(1/10.0);
		cairo_move_to(cr, x1-r, 2*r);				
		cairo_line_to(cr, x1, 0+r);
		cairo_line_to(cr, x2, 0+r);
		cairo_line_to(cr, x2+r, 2*r);
		
		cairo_move_to(cr, x1-r, y-2*r);				
		cairo_line_to(cr, x1, y-r);
		cairo_line_to(cr, x2, y-r);
		cairo_line_to(cr, x2+r, y-2*r);
		

		cairo_stroke(cr);

	}
	axes->GetArea()->Restore(cr);
}



//---------------------------------- Методы класса VSlopeGraph --------------------------------------

/** Конструктор*/
VSlopeGraph::VSlopeGraph(VDataProviderPtr in_data, VAxesPtr in_axes)
{
	if (in_data && in_axes)
	{		
		data=in_data;
		axes=in_axes;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VSlopeGraph::Paint(cairo_t * cr)
{
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;	
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));		
	CairoSetColor(cr, COLOR_HTML_BLACK);
	cairo_rectangle(cr, 0, 0, axes->GetArea()->GetWidth(), axes->GetArea()->GetHeight()+1);
	cairo_stroke(cr);	
	for (size_t i=visible.front(); i<=visible.back(); ++i)	
	{
		double start_x=axes->ParamToAreaX(data->GetX(i));
		double end_x=axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i));		
		double len=data->GetSizeX(i);
		double max_text_len=end_x-start_x-axes->DrawingToDeviceX(text_space);
		//Рисуем вертикальные линии
		cairo_move_to(cr, start_x, 0);
		cairo_line_to(cr, start_x, axes->GetArea()->GetHeight());	

		if (i==data->GetCount()-1)
		{
			cairo_move_to(cr, end_x, 0);
			cairo_line_to(cr, end_x, axes->GetArea()->GetHeight());									
		}		 
		
		//Рисуем наклон и выводим оцифровку
		switch (data->GetFlags(i))
		{
			default:
			case SLOPE_ZERO:
			{
				cairo_move_to(cr, start_x, axes->GetArea()->GetHeight()/2);
				cairo_line_to(cr, end_x, axes->GetArea()->GetHeight()/2);	
				cairo_text_extents_t te;
				CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
				//Наклон			
				std::wstring text=data->GetLabel(i);
				cairo_text_extents (cr, text, &te);
				cairo_move_to (cr, start_x+axes->DrawingToDeviceX(text_space), te.height+axes->DrawingToDeviceX(text_space));
				if (te.width<max_text_len)
					cairo_show_text(cr, text);
				//Длина элемента
				if (len!=100 && len!=1000)
				{
					text=boost::str(boost::wformat(L"%0.0f") % len);
					cairo_text_extents (cr, text, &te);
					cairo_move_to (cr, end_x-axes->DrawingToDeviceX(text_space)-te.width, axes->GetArea()->GetHeight()-axes->DrawingToDeviceX(text_space));				
					if (te.width<max_text_len)
						cairo_show_text(cr, text);
				}
				break;
			}
			case SLOPE_POSITIVE:
			{
				cairo_move_to(cr, start_x, axes->GetArea()->GetHeight());
				cairo_line_to(cr, end_x, 0);						
				cairo_text_extents_t te;
				CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
				//Наклон
				std::wstring text=data->GetLabel(i);
				cairo_text_extents (cr, text, &te);
				cairo_move_to (cr, start_x+axes->DrawingToDeviceX(text_space), te.height+axes->DrawingToDeviceX(text_space));
				if (te.width<max_text_len)
					cairo_show_text(cr, text);
				//Длина элемента
				if (len!=100 && len!=1000)
				{
					text=boost::str(boost::wformat(L"%0.0f") % len);
					cairo_text_extents (cr, text, &te);
					cairo_move_to (cr, end_x-axes->DrawingToDeviceX(text_space)-te.width, axes->GetArea()->GetHeight()-axes->DrawingToDeviceX(text_space));				
					if (te.width<max_text_len)
						cairo_show_text(cr, text);
				}
				break;
			}
			case SLOPE_NEGATIVE:
			{
				cairo_move_to(cr, start_x, 0);
				cairo_line_to(cr, end_x, axes->GetArea()->GetHeight());												
				CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
				cairo_text_extents_t te;
				//Наклон
				std::wstring text=data->GetLabel(i);
				cairo_text_extents(cr, text, &te);
				cairo_move_to(cr, end_x-(te.width+axes->DrawingToDeviceX(text_space)), te.height+axes->DrawingToDeviceX(text_space));				
				if (te.width<max_text_len)
					cairo_show_text(cr, text);
				//Длина элемента
				if (len!=100 && len!=1000)
				{
					text=boost::str(boost::wformat(L"%0.0f") % len);
					cairo_text_extents (cr, text, &te);
					cairo_move_to(cr, start_x+axes->DrawingToDeviceX(text_space), axes->GetArea()->GetHeight()-axes->DrawingToDeviceX(text_space));
					if (te.width<max_text_len)
						cairo_show_text(cr, text);
				}
				break;
			}
			case SLOPE_HILL:
			{								
				double l=end_x-start_x;
				double h=std::min<double>(axes->GetArea()->GetHeight(), l/2);
				
				circle_t c(point_t(start_x, axes->GetArea()->GetHeight()), point_t(start_x+l/2, axes->GetArea()->GetHeight()-h), point_t(end_x, axes->GetArea()->GetHeight()));
				cairo_new_sub_path(cr);
				cairo_arc(cr, c.X(), c.Y(), c.R(), 0, M_PI*2);
				
				CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
				cairo_text_extents_t te;
				//Радиус
				std::wstring text=data->GetLabel(i);
				cairo_text_extents (cr, text, &te);
				cairo_move_to (cr, start_x+l/2-te.width/2, axes->GetArea()->GetHeight()-axes->DrawingToDeviceX(text_space));
				if (te.width<max_text_len)
					cairo_show_text(cr, text);
				//Длина элемента
				text=boost::str(boost::wformat(L"%0.0f") % len);
				cairo_text_extents (cr, text, &te);
				cairo_move_to (cr, start_x+l/2-te.width/2, axes->GetArea()->GetHeight() - te.height - 2*axes->DrawingToDeviceX(text_space));
				if (te.width<max_text_len)
					cairo_show_text(cr, text);
				break;
			}
			case SLOPE_PIT:
			{		
				double l=end_x-start_x;
				double h=std::min<double>(axes->GetArea()->GetHeight(), l/2);

				circle_t c(point_t(start_x, 0), point_t(start_x+l/2, h), point_t(end_x, 0));
				cairo_new_sub_path(cr);
				cairo_arc(cr, c.X(), c.Y(), c.R(), 0, M_PI*2);
				
				CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
				cairo_text_extents_t te;
				//Радиус
				std::wstring text=data->GetLabel(i);
				cairo_text_extents (cr, text, &te);
				cairo_move_to (cr, start_x+l/2-te.width/2, axes->DrawingToDeviceX(text_space)+te.height);
				if (te.width<end_x-start_x)
					cairo_show_text(cr, text);
				//Длина элемента
				text=boost::str(boost::wformat(L"%0.0f") % len);
				cairo_text_extents (cr, text, &te);
				cairo_move_to (cr, start_x+l/2-te.width/2, 2.0*(axes->DrawingToDeviceX(text_space)+te.height));
				if (te.width<end_x-start_x)
					cairo_show_text(cr, text);
				break;
			}
		}
	}	
	cairo_stroke(cr);
	axes->GetArea()->Restore(cr);
}

//----------------------------------- Методы класса VXMarkGraph ------------------------------------

/** Конструктор*/
VXMarkGraph::VXMarkGraph(VDataProviderPtr in_data, VAxesPtr in_axes, VColor in_color, double in_width, double in_height,
	area_edge_t in_edge, bool in_show_labels)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;
		width=in_width;
		height=in_height;
		edge=in_edge;
		show_labels=in_show_labels;
	}
	else
	{
		throw NULL_POINTER;
	}
};

/** Рисование*/
void VXMarkGraph::Paint(cairo_t * cr)
{
	if (axes && data)
	{
		if (data->GetCount()>0)
		{
			//Определяем диапазон индексов кадров, которые будут отбражены				
			std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
			if (visible.size()<2)
				return;
			axes->GetArea()->Apply(cr, CLIP_HORIZONTAL);
			cairo_set_antialias(cr,antialias);
			//Параметр	
			cairo_new_path(cr);									
			double x;						
			for (size_t i=visible.front(); i<=visible.back(); ++i)
			{				

				x=axes->ParamToAreaX(data->GetX(i));
				CairoSetColor(cr, color);
				CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));				
				double zero=(edge==AREA_BOTTOM) ? axes->GetArea()->GetHeight() : axes->DrawingToDeviceX(height/10.0);
				double y=(edge==AREA_BOTTOM) ? zero-axes->DrawingToDeviceX(height/10.0) : 0;
				switch (edge)
				{
					case AREA_TOP:
						zero=axes->DrawingToDeviceX(height/10.0);
						y=0;
						break;
					default:
					case AREA_BOTTOM:
						zero=axes->GetArea()->GetHeight();
						y=zero-axes->DrawingToDeviceX(height/10.0);
						break;
					case AREA_OVER:
						zero=0;
						y=zero-axes->DrawingToDeviceX(height/10.0);
						break;
					case AREA_BELOW:
						zero=axes->GetArea()->GetHeight();
						y=zero+axes->DrawingToDeviceX(height/10.0);
						break;
				}
				cairo_move_to(cr, x, zero);
				cairo_line_to(cr, x, y);				
				cairo_stroke(cr);
				if (show_labels)
				{
					CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD));				
					(edge==AREA_BOTTOM) ? cairo_move_to(cr, x+axes->DrawingToDeviceX(text_space), zero-axes->DrawingToDeviceX(text_space)) : cairo_move_to(cr, x+axes->DrawingToDeviceX(text_space), zero+axes->DrawingToDeviceX(text_space));
					std::wstring text=data->GetLabel(i);
					cairo_show_text(cr, text);
				}
			}
			//Восстанавливаем состояние	
			axes->GetArea()->Restore(cr);
		}
	}
	else
	{
		throw NULL_POINTER;
	}
};


//----------------------------------- Методы класса VXLineGraph ------------------------------------

/** Конструктор*/
VXLineGraph::VXLineGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VDash& in_dash, const VColor& in_color, 
						 double in_width, area_edge_t in_edge)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		dash=in_dash;
		color=in_color;
		width=in_width;
		edge=in_edge;
	}
	else
	{
		throw NULL_POINTER;
	}
};

/** Рисование*/
void VXLineGraph::Paint(cairo_t * cr)
{
	if (axes && data)
	{
		if (data->GetCount()>0)
		{
			//Определяем диапазон индексов кадров, которые будут отбражены				
			std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
			if (visible.size()<2)
				return;
			axes->GetArea()->Apply(cr, CLIP_AREA);
			cairo_set_antialias(cr,antialias);
			//Параметр	
			cairo_new_path(cr);									
			double x;						
			for (size_t i=visible.front(); i<=visible.back(); ++i)
			{				
				x=axes->ParamToAreaX(data->GetX(i));
				CairoSetColor(cr, color);
				CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));	
				CairoSetDash(cr, axes->DrawingToDeviceX(dash.On()/10.0), axes->DrawingToDeviceX(dash.Off()/10.0));
				cairo_move_to(cr, x, 0);
				cairo_line_to(cr, x, axes->GetArea()->GetHeight());
				cairo_stroke(cr);
				
				cairo_save(cr);
				CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));				
				cairo_move_to(cr, x-axes->DrawingToDeviceX(text_space), axes->GetArea()->GetHeight()-axes->DrawingToDeviceX(text_space));
				cairo_rotate(cr, -M_PI/2);
				std::wstring text=data->GetLabel(i);
				cairo_show_text(cr, text);
				cairo_restore(cr);
			}
			//Восстанавливаем состояние	
			axes->GetArea()->Restore(cr);
		}
	}
	else
	{
		throw NULL_POINTER;
	}
};


//----------------------------------- Методы класса VActiveWaySectionElement -----------------------------------

/** Конструктор*/
VActiveWaySectionElement::VActiveWaySectionElement(VDataProviderPtr in_data, VAxesPtr in_axes, size_t in_index, VColor in_color, VColor in_fill_color, double in_width)
{
	if (in_data && in_axes)
	{
		data=in_data;		
		axes=in_axes;
		index=in_index;
		color=in_color;
		fill_color=in_fill_color;
		width=in_width;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VActiveWaySectionElement::Paint(cairo_t * cr)
{
	cairo_set_antialias(cr,antialias);
	axes->GetArea()->Apply(cr, CLIP_AREA);
	double arrow_size=axes->DrawingToDeviceX(2.0/10.0);
	double left_x=axes->ParamToAreaX(data->GetX(index));
	double right_x=axes->ParamToAreaX(data->GetX(index)+data->GetSizeX(index));
	if (!(left_x>axes->GetArea()->GetWidth() || right_x<0))
	{		
		//Заливка
		CairoSetColor(cr, fill_color);

		cairo_move_to(cr, left_x, 0);
		cairo_line_to(cr, left_x+arrow_size, 0);
		cairo_line_to(cr, left_x, arrow_size);
		cairo_line_to(cr, left_x, 0);

		cairo_move_to(cr, left_x, axes->GetArea()->GetHeight());
		cairo_line_to(cr, left_x+arrow_size, axes->GetArea()->GetHeight());
		cairo_line_to(cr, left_x, axes->GetArea()->GetHeight()-arrow_size);
		cairo_line_to(cr, left_x, axes->GetArea()->GetHeight());		

		cairo_move_to(cr, right_x, 0);
		cairo_line_to(cr, right_x-arrow_size, 0);
		cairo_line_to(cr, right_x, arrow_size);
		cairo_line_to(cr, right_x, 0);

		cairo_move_to(cr, right_x, axes->GetArea()->GetHeight());
		cairo_line_to(cr, right_x-arrow_size, axes->GetArea()->GetHeight());
		cairo_line_to(cr, right_x, axes->GetArea()->GetHeight()-arrow_size);
		cairo_line_to(cr, right_x, axes->GetArea()->GetHeight());
		cairo_fill(cr);

		//Контур

		CairoSetColor(cr, color);		
		data->GetSelectedPoint()==index ? CairoSetLineWidth(cr, axes->DrawingToDeviceX(width*2.0/10.0)) : 
				CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));

		cairo_move_to(cr, left_x, 0);
		cairo_line_to(cr, left_x, axes->GetArea()->GetHeight());

		cairo_move_to(cr, left_x, 0);
		cairo_line_to(cr, left_x+arrow_size, 0);
		cairo_line_to(cr, left_x, arrow_size);
		cairo_line_to(cr, left_x, 0);

		cairo_move_to(cr, left_x, axes->GetArea()->GetHeight());
		cairo_line_to(cr, left_x+arrow_size, axes->GetArea()->GetHeight());
		cairo_line_to(cr, left_x, axes->GetArea()->GetHeight()-arrow_size);
		cairo_line_to(cr, left_x, axes->GetArea()->GetHeight());

		cairo_move_to(cr, right_x, 0);
		cairo_line_to(cr, right_x, axes->GetArea()->GetHeight());

		cairo_move_to(cr, right_x, 0);
		cairo_line_to(cr, right_x-arrow_size, 0);
		cairo_line_to(cr, right_x, arrow_size);
		cairo_line_to(cr, right_x, 0);

		cairo_move_to(cr, right_x, axes->GetArea()->GetHeight());
		cairo_line_to(cr, right_x-arrow_size, axes->GetArea()->GetHeight());
		cairo_line_to(cr, right_x, axes->GetArea()->GetHeight()-arrow_size);
		cairo_line_to(cr, right_x, axes->GetArea()->GetHeight());

		cairo_stroke(cr);

		//Текст		
		CairoSetColor(cr, COLOR_HTML_BLACK);
		CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));			

		std::wstring text=data->GetLabel(index);// + std::wstring(L" (начало)");
		cairo_text_extents_t te;
		cairo_text_extents(cr, text, &te);
		cairo_move_to(cr, left_x+axes->DrawingToDeviceX(text_space)+te.height, axes->GetArea()->GetHeight()-arrow_size);
		cairo_save(cr);
		cairo_rotate(cr, -M_PI/2);
		cairo_show_text(cr, text);
		cairo_restore(cr);
	}	
	axes->GetArea()->Restore(cr);	
}

/** Обработка hit-test*/
bool VActiveWaySectionElement::HitTest(double in_x, double in_y)
{
	if (index<data->GetCount())
	{
		double hit_size=3;		
		double left_side_x=axes->GetArea()->AreaToScreenX(axes->ParamToAreaX(data->GetX(index)));
		double right_side_x=axes->GetArea()->AreaToScreenX(axes->ParamToAreaX(data->GetX(index)+data->GetSizeX(index)));
		if (in_x>left_side_x-hit_size && in_x<left_side_x+hit_size && 
			in_y>axes->GetArea()->GetTop() && in_y<axes->GetArea()->GetTop()+axes->GetArea()->GetHeight() ||
			in_x>right_side_x-hit_size && in_x<right_side_x+hit_size && 
			in_y>axes->GetArea()->GetTop() && in_y<axes->GetArea()->GetTop()+axes->GetArea()->GetHeight())	
			return true;	
		else	
			return false;	
		
	}
	else
		return false;
}

/** Обработка событий*/
bool VActiveWaySectionElement::ProcessEvent(const VEvent& in_event)
{
	bool result = false;
	if (GetActive() && data->GetActive())
	{
		switch (in_event.GetCode())
		{
			case EVENT_MOUSE_MOTION:
			{
				double x=((VMouseEvent&)in_event).GetX();
				double y=((VMouseEvent&)in_event).GetY();
				//Обрабатываем HitTest								
				if (HitTest(x,y))
					data->SelectPoint(index);				
				result = true;												
				break;
			}			
			default:
				break;
		}
	}
	return result;
}

//--------------------------------- Методы класса VActiveWaySectionGraph ------------------------------------

/** Конструктор*/
VActiveWaySectionGraph::VActiveWaySectionGraph(VDataProviderPtr in_data, VAxesPtr in_axes, VColor in_color, VColor in_fill_color, double in_width)
{
	if (in_data && in_axes)
	{
		data=in_data;		
		axes=in_axes;
		color=in_color;
		fill_color=in_fill_color;	
		width=in_width;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VActiveWaySectionGraph::Paint(cairo_t * cr)
{
	if (data->GetCount()==0)
		return;		
	//Добавляем элемент для каждой точки массива данных
	if (data->GetCount()!=points.size())
	{
		points.clear();
		points.reserve(data->GetCount());
		points.resize(data->GetCount());
		for (size_t i=0; i<data->GetCount(); ++i)
		{							
			points.at(i)=VActiveWaySectionElementPtr(new VActiveWaySectionElement(data, axes, i, color, fill_color, width));
		}
	}
	//Просто отрисовываем все точки	
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;		
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{		
		if (i<points.size())
			points.at(i)->Render(cr);
	}		
}


/** Обработка событий*/
bool VActiveWaySectionGraph::ProcessEvent(const VEvent& in_event)
{
	bool result=false;
	if (GetActive())
	{
		if (in_event.GetCode()==EVENT_MOUSE_MOTION)
			data->DeselectPoint();
		for (point_itr i=points.begin(); i!=points.end(); ++i)
		{
			bool changed=(*i)->ProcessEvent(in_event);
			result = result || changed;
		}
	}
	return result;	
}

//--------------------------------- Методы класса VWaySectionGraph ------------------------------------

/** Конструктор*/
VWaySectionGraph::VWaySectionGraph(VDataProviderPtr in_data, VAxesPtr in_axes, VColor in_color, VColor in_fill_color, double in_width, bool in_show_labels)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		color=in_color;
		fill_color=in_fill_color;	
		width=in_width;
		show_labels=in_show_labels;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VWaySectionGraph::Paint(cairo_t * cr)
{
	cairo_set_antialias(cr,antialias);
	axes->GetArea()->Apply(cr, CLIP_AREA);
	for (size_t i=0; i<data->GetCount(); ++i)
	{		
		double left_x=axes->ParamToAreaX(data->GetX(i));
		double right_x=axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i));
		if (!(left_x>axes->GetArea()->GetWidth() || right_x<0))
		{
			std::wstring label=data->GetLabel(i);			
			//Заливка и контур
			CairoSetColor(cr, fill_color);
			cairo_rectangle(cr, left_x, 0, right_x-left_x, axes->GetArea()->GetHeight());
			cairo_fill_preserve(cr);			
			CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));
			CairoSetColor(cr, color);
			cairo_stroke(cr);			

			//Текст
			if (show_labels)
			{
				cairo_save(cr);
				cairo_rectangle(cr, left_x, 0, right_x-left_x, axes->GetArea()->GetHeight());
				cairo_clip(cr);				
				CairoSetColor(cr, COLOR_HTML_BLACK);
				CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));			

				std::wstring text=data->GetLabel(i);
				cairo_text_extents_t te;
				cairo_text_extents(cr, text, &te);			
				double text_pos_x=(std::max<double>(left_x, 0) + std::min<double>(right_x, axes->GetArea()->GetWidth()))/2
					-te.width/2;
				double text_pos_y=axes->GetArea()->GetHeight()/2+te.height/2;
				cairo_move_to(cr, text_pos_x, text_pos_y);
				cairo_show_text(cr, text);
				cairo_restore(cr);				
			}
		}
	}
	axes->GetArea()->Restore(cr);	
}

//----------------------------------- Методы класса VHeaderElement ------------------------------------

/** Конструктор*/
VHeaderElement::VHeaderElement(VAreaPtr in_area, VDrawingDevicePtr in_device)
{
	if (in_area==NULL)
		throw NULL_POINTER;
	area=in_area;
	device=in_device;
}

/** Отрисовка*/
void VHeaderElement::Paint(cairo_t * cr)
{
	if (area)
	{
		area->Apply(cr, CLIP_AREA);		
		cairo_set_antialias(cr,antialias);			
		CairoSetColor(cr, COLOR_HTML_BLACK);
		CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, device->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD));		
		for (size_t i=0; i<lines.size(); ++i)
		{
			cairo_text_extents_t te;
			cairo_text_extents (cr, lines.at(i), &te);
			cairo_move_to (cr, area->GetWidth()/2-te.width/2, (i+1) * (te.height+te.height/2));
			cairo_show_text(cr, lines.at(i));
		}
		area->Restore(cr);
	}
}

/** Добавляет строку в заголовок*/
void VHeaderElement::AddLine(const std::wstring& in_line)
{
	lines.push_back(in_line);
}

//---------------------- Методы класса VGraphHeaderElement -------------------------------------

/** Конструктор*/
VGraphHeaderElement::VGraphHeaderElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VGraphInfoPtr in_graph_info)
{
	if (in_area && in_device && in_graph_info)
	{
		area=in_area;
		device=in_device;
		graph_info=in_graph_info;
	}
	else
	{
		throw NULL_POINTER;
	}
}
/** Рисование*/
void VGraphHeaderElement::Paint(cairo_t * cr)
{
	area->Apply(cr, CLIP_AREA);		
	cairo_set_antialias(cr,antialias);		
	CairoSetColor(cr, COLOR_HTML_BLACK);
	CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, device->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD));
	cairo_text_extents_t te;
	cairo_text_extents(cr, graph_info->GetGraphName(), &te);
	cairo_move_to(cr, device->DrawingToDeviceX(text_space), te.height+device->DrawingToDeviceY(text_space));
	cairo_show_text(cr, graph_info->GetGraphName());

	cairo_move_to(cr, device->DrawingToDeviceX(text_space), 2*(te.height+device->DrawingToDeviceY(text_space)));
	cairo_show_text(cr, graph_info->GetWaySection());
	
	area->Restore(cr);
}

//---------------------- Методы класса VDataSetsElement -------------------------------------

/** Конструктор*/
VDataSetsElement::VDataSetsElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VGraphInfoPtr in_graph_info)
{
	if (in_area && in_device && in_graph_info)
	{
		area=in_area;
		device=in_device;
		graph_info=in_graph_info;
	}
	else
	{
		throw NULL_POINTER;
	}
}
/** Рисование*/
void VDataSetsElement::Paint(cairo_t * cr)
{
	area->Apply(cr, CLIP_AREA);		
	cairo_set_antialias(cr,antialias);		
	CairoSetColor(cr, COLOR_HTML_BLACK);

	for (size_t i=0; i<graph_info->GetFileNames().size(); ++i)
	{
		if (i==graph_info->GetActiveFileIndex())
			CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, device->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD));
		else
			CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, device->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
		cairo_text_extents_t te;
		cairo_text_extents (cr, graph_info->GetFileNames().at(i), &te);
		cairo_move_to (cr, area->GetWidth()-te.width-device->DrawingToDeviceX(text_space), (i+1) * (te.height+te.height/2));		
		cairo_show_text(cr, graph_info->GetFileNames().at(i));
	}
	area->Restore(cr);
}

//--------------------- Методы класса VGridLinesXElement ------------------------------------

/** Конструктор*/
VGridLinesXElement::VGridLinesXElement(VAxesPtr in_axes, bool in_show_text)
{		
	if (in_axes==NULL)
		throw NULL_POINTER;
	axes=in_axes;	
	show_text=in_show_text;
}

/** Рисование*/
void VGridLinesXElement::Paint(cairo_t * cr)
{		
	if (axes)
	{	
		//Рисуется ноль и координатные линии слева и справа нуля с определенным шагом.
		double range_x=fabs(axes->GetMaxX()-axes->GetMinX());
		double screen_x=axes->GetArea()->GetWidth();		
		//Шаг линий сетки		
		double step_x = axes->ParamToDrawingX(range_x)>2.0 ? axes->GetScaleX() : axes->GetScaleX()/2;		
		axes->GetArea()->Apply(cr, CLIP_HORIZONTAL);		
		cairo_set_antialias(cr,antialias);
		CairoSetLineWidth(cr, axes->DrawingToDeviceY(default_line_width/10.0));		
		CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceX(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));		
		CairoSetDash(cr, axes->DrawingToDeviceX(2.5/10.0), axes->DrawingToDeviceX(2.5/10.0));
		double zero_x=axes->ParamToAreaX(0);
		//Ноль
		if (zero_x>0 && zero_x<axes->GetArea()->GetWidth())
		{
			CairoSetColor(cr, VColor(0,0,0,100));
			cairo_move_to(cr, zero_x, 1);
			cairo_line_to(cr, zero_x, axes->GetArea()->GetHeight()-2);
			cairo_stroke(cr);				
			if (show_text)
			{				
				CairoSetColor(cr, COLOR_HTML_BLACK);
				CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));				
				cairo_move_to(cr, zero_x, axes->GetArea()->GetHeight()+2);					
				std::wstring text = boost::str( boost::wformat(L"%0.0f") % 0.0);
				cairo_save(cr);
				cairo_rotate(cr, M_PI/2);
				cairo_show_text(cr, text);
				cairo_restore(cr);
			}
		}	
		//Справа от нуля		
		CairoSetDash(cr, axes->DrawingToDeviceX(5.0/10.0), axes->DrawingToDeviceX(1.25/10.0));
		for (double l=0+step_x; l<axes->GetMaxX(); l+=step_x)
		{
			double x=axes->ParamToAreaX(l);
			if (x>0 && x<axes->GetArea()->GetWidth())
			{				
				cairo_move_to(cr, x, 1);
				cairo_line_to(cr, x, axes->GetArea()->GetHeight()-2);				
				if (show_text)
				{																
					CairoSetColor(cr, COLOR_HTML_BLACK);
					CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));					
					cairo_move_to(cr, x, axes->GetArea()->GetHeight()+axes->DrawingToDeviceX(text_space));					
					std::wstring text = (range_x>10) ? boost::str( boost::wformat(L"%0.1f") % l) : boost::str( boost::wformat(L"%0.2f") % l);
					cairo_save(cr);
					cairo_rotate(cr, M_PI/2);
					cairo_show_text(cr, text);
					cairo_restore(cr);
				}
			}
		}
		CairoSetColor(cr, VColor(0,0,0,50));
		cairo_stroke(cr);
		//Слева от нуля
		for (double l=0-step_x; l>axes->GetMinX(); l-=step_x)
		{
			double x=axes->ParamToAreaX(l);
			if (x>0 && x<axes->GetArea()->GetWidth())
			{				
				cairo_move_to(cr, x, 1);
				cairo_line_to(cr, x, axes->GetArea()->GetHeight()-2);				
				if (show_text)
				{																
					CairoSetColor(cr, COLOR_HTML_BLACK);
					CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));					
					cairo_move_to(cr, x, axes->GetArea()->GetHeight()+axes->DrawingToDeviceX(text_space));					
					std::wstring text = (range_x>10) ? boost::str( boost::wformat(L"%0.1f") % l) : boost::str( boost::wformat(L"%0.2f") % l);
					cairo_save(cr);
					cairo_rotate(cr, M_PI/2);
					cairo_show_text(cr, text);
					cairo_restore(cr);
				}
			}
		}
		CairoSetColor(cr, VColor(0,0,0,50));
		cairo_stroke(cr);
		axes->GetArea()->Restore(cr);		
	}
}

//--------------------- Методы класса VGridLinesYElement ------------------------------------

/** Конструктор*/
VGridLinesYElement::VGridLinesYElement(VAxesPtr in_axes, bool in_show_text, side_t in_side)
{		
	if (in_axes==NULL)
		throw NULL_POINTER;
	axes=in_axes;	
	show_text=in_show_text;
	side=in_side;
}

/** Рисование*/
void VGridLinesYElement::Paint(cairo_t * cr)
{		
	if (axes)
	{				
		//Рисуется ноль и координатные линии выше и ниже нуля с опрделенным шагом. Шаг подбирается автоматически.	
		double range_y=fabs(axes->GetMaxY()-axes->GetMinY());
		double screen_y=axes->GetArea()->GetHeight();		
		//Шаг линий сетки		
		double step_y = axes->ParamToDrawingY(range_y)>2.0 ? axes->GetScaleY() : axes->GetScaleY()/2;
		axes->GetArea()->Apply(cr, CLIP_NONE);		
		cairo_set_antialias(cr,antialias);
		CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));		
		CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));				
		CairoSetDash(cr, axes->DrawingToDeviceY(2.5/10.0), axes->DrawingToDeviceY(2.5/10.0));
		double zero_y=axes->ParamToAreaY(0);
		//Ноль
		if (zero_y>0 && zero_y<axes->GetArea()->GetHeight())
		{
			CairoSetColor(cr, VColor(0,0,0,100));
			cairo_move_to(cr, 1, zero_y);
			cairo_line_to(cr, axes->GetArea()->GetWidth()-2, zero_y);
			cairo_stroke(cr);
			if (show_text)
			{					

				std::wstring text = boost::str( boost::wformat(L"%0.0f") % 0.0);
				cairo_text_extents_t te;				
				cairo_text_extents(cr, text, &te);	
				if (side==SD_LEFT)
					cairo_move_to(cr, - (te.width+axes->DrawingToDeviceX(text_space)), zero_y+te.height/2);
				else
					cairo_move_to(cr, axes->GetArea()->GetWidth()+axes->DrawingToDeviceX(text_space), zero_y+te.height/2);				
				CairoSetColor(cr, COLOR_HTML_BLACK);
				cairo_show_text(cr, text);
			}
		}	
		//Выше и ниже нуля		
		CairoSetDash(cr, axes->DrawingToDeviceY(5.0/10.0), axes->DrawingToDeviceY(1.25/10.0));
		double l1=round_to(axes->GetMinY(), step_y);
		double l2=round_to(axes->GetMaxY(), step_y);
		for (double l=l1; l<=l2; l+=step_y)
		{
			double y=axes->ParamToAreaY(l);
			if (!pro::interval<double>(-step_y/2, step_y/2).contains(l) && pro::interval<double>(0, axes->GetArea()->GetHeight()).contains(y))
			{				
				cairo_move_to(cr, 1, y);
				cairo_line_to(cr, axes->GetArea()->GetWidth()-2, y);					
				if (show_text)
				{					
					std::wstring text = (range_y>10) ? boost::str( boost::wformat(L"%0.1f") % l) : boost::str( boost::wformat(L"%0.2f") % l);
					cairo_text_extents_t te;
					cairo_text_extents(cr, text, &te);	
					if (side==SD_LEFT)
						cairo_move_to(cr, -(te.width+axes->DrawingToDeviceX(text_space)), y+te.height/2);
					else
						cairo_move_to(cr, axes->GetArea()->GetWidth()+axes->DrawingToDeviceX(text_space), y+te.height/2);					
					CairoSetColor(cr, COLOR_HTML_BLACK);
					cairo_show_text(cr, text);
				}
			}
		}		
		CairoSetColor(cr, VColor(0,0,0,50));
		cairo_stroke(cr);		
		axes->GetArea()->Restore(cr);		
	}
}

//--------------------------------- Методы класса VPreviousKmElement ----------------------------------

/** Конструктор*/
VPreviousKmElement::VPreviousKmElement(VAxesPtr in_axes, VCoordinatesDataPtr in_data)
{
	if (in_axes && in_data)
	{
		axes=in_axes;	
		data=in_data;
	}
	else
	{
		throw NULL_POINTER;
	}	
}

/** Рисование*/
void VPreviousKmElement::Paint(cairo_t * cr)
{
	if (axes)
	{		
		std::wstring label;
		size_t i=0;
		while (i<data->GetCount() && data->GetX(i)<axes->GetMinX())
		{
			if (data->GetFlags(i) == COORDINATE_KM || data->GetFlags(i) == COORDINATE_KM_5)
			{
				label=data->GetLabel(i);
			}
			i++;
		}
		axes->GetArea()->Apply(cr, CLIP_NONE);
		CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD));
		cairo_text_extents_t te;
		cairo_text_extents(cr, label, &te);
		cairo_move_to(cr, -te.width-axes->DrawingToDeviceX(text_space), axes->GetArea()->GetHeight()+te.height+axes->DrawingToDeviceY(text_space));
		cairo_show_text(cr, label);
		axes->GetArea()->Restore(cr);		
	}		
}

//--------------------------------- Методы класса VCoordinatesGridElement --------------------------------

/** Конструктор*/
VCoordinatesGridElement::VCoordinatesGridElement(VDataProviderPtr in_data, VAxesPtr in_axes)
{
	if (in_axes && in_data)
	{
		axes=in_axes;	
		data=in_data;
	}
	else
	{
		throw NULL_POINTER;
	}	
}

/** Рисование*/
void VCoordinatesGridElement::Paint(cairo_t * cr)
{
	if (axes)
	{
		//Минимальное расстояние между линиями координатной сетки (см)
		const double min_grid_space=0.5;		
		axes->GetArea()->Apply(cr, CLIP_HORIZONTAL);
		cairo_set_antialias(cr,antialias);
		CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));

		std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
		if (visible.size()<2)
			return;

		for (size_t i=visible.front(); i<visible.back(); ++i)
		{
			double x1=axes->ParamToAreaX(data->GetX(i));	
			double x2=axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i));
			switch (data->GetFlags(i))
			{
				case COORDINATE_KM_5:
				{															
					CairoSetDash(cr, axes->DrawingToDeviceY(2.5/10.0), axes->DrawingToDeviceY(2.5/10.0));					
					CairoSetColor(cr, COLOR_HTML_BLACK);
					cairo_move_to(cr, x1, 1);
					cairo_line_to(cr, x1, axes->GetArea()->GetHeight()-2);
					cairo_stroke(cr);
					break;					
				}
				case COORDINATE_KM:
				{
					if (axes->ParamToDrawingX(1000)>2*min_grid_space)//Если между линиями больше двух сантиметров
					{																				
						CairoSetDash(cr, axes->DrawingToDeviceY(2.5/10.0), axes->DrawingToDeviceY(2.5/10.0));						
						CairoSetColor(cr, COLOR_HTML_BLACK);
						cairo_move_to(cr, x1, 1);
						cairo_line_to(cr, x1, axes->GetArea()->GetHeight()-2);
						cairo_stroke(cr);												
						break;
					}
				}
				case COORDINATE_PK:
				{					
					if (axes->ParamToDrawingX(100)>min_grid_space)//Если между линиями больше одного сантиметра
					{													
						CairoSetDash(cr, axes->DrawingToDeviceY(1.25/10.0), axes->DrawingToDeviceY(1.25/10.0));						
						CairoSetColor(cr, COLOR_HTML_GRAY);
						cairo_move_to(cr, x2, 1);
						cairo_line_to(cr, x2, axes->GetArea()->GetHeight()-2);
						cairo_stroke(cr);
						break;
					}
				}				
			}
		}
	}		
}


//--------------------------------- Методы класса VCoordinatesTextElement --------------------------------

/** Конструктор*/
VCoordinatesTextElement::VCoordinatesTextElement(VDataProviderPtr in_data, VAxesPtr in_axes)
{
	if (in_axes && in_data)
	{
		axes=in_axes;	
		data=in_data;		
	}
	else
	{
		throw NULL_POINTER;
	}	
}

/** Рисование*/
void VCoordinatesTextElement::Paint(cairo_t * cr)
{
	if (axes)
	{
		//Минимальное расстояние между линиями координатной сетки (см)
		const double min_grid_space=0.5;		
		axes->GetArea()->Apply(cr, CLIP_HORIZONTAL);
		cairo_set_antialias(cr,antialias);
		CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));

		std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
		if (visible.size()<2)
			return;

		for (size_t i=visible.front(); i<visible.back(); ++i)
		{
			double x1=axes->ParamToAreaX(data->GetX(i));	
			double x2=axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i));
			switch (data->GetFlags(i))
			{
				case COORDINATE_KM_5:
				{															
																
					CairoSetColor(cr, COLOR_HTML_BLACK);
					cairo_move_to(cr, x1, axes->GetArea()->GetHeight());
					cairo_line_to(cr, x1, axes->GetArea()->GetHeight()+axes->DrawingToDeviceY(0.4));
					cairo_stroke(cr);												

					CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD));
					cairo_text_extents_t te;
					cairo_text_extents(cr, data->GetLabel(i), &te);
					cairo_move_to(cr, axes->GetInverseX() ? x1-te.width-axes->DrawingToDeviceX(text_space) :
						x1+axes->DrawingToDeviceX(text_space), axes->GetArea()->GetHeight()+te.height+axes->DrawingToDeviceX(text_space));
					cairo_show_text(cr, data->GetLabel(i));
					
					break;					
				}
				case COORDINATE_KM:
				{
					if (axes->ParamToDrawingX(1000)>2*min_grid_space)//Если между линиями больше двух сантиметров
					{																				
											
						CairoSetColor(cr, COLOR_HTML_BLACK);
						cairo_move_to(cr, x1, axes->GetArea()->GetHeight());
						cairo_line_to(cr, x1, axes->GetArea()->GetHeight()+axes->DrawingToDeviceY(0.4));
						cairo_stroke(cr);	

						CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD));
						cairo_text_extents_t te;
						cairo_text_extents(cr, data->GetLabel(i), &te);
						cairo_move_to(cr, axes->GetInverseX() ? x1-te.width-axes->DrawingToDeviceX(text_space) :
							x1+axes->DrawingToDeviceX(text_space), axes->GetArea()->GetHeight()+te.height+axes->DrawingToDeviceX(text_space));
						cairo_show_text(cr, data->GetLabel(i));
						
						break;
					}
				}
				case COORDINATE_PK:
				{					
					if (axes->ParamToDrawingX(100)>min_grid_space)//Если между линиями больше одного сантиметра
					{																			
						CairoSetColor(cr, COLOR_HTML_BLACK);
						cairo_move_to(cr, x2, axes->GetArea()->GetHeight());
						cairo_line_to(cr, x2, axes->GetArea()->GetHeight()+axes->DrawingToDeviceY(0.2));
						cairo_stroke(cr);

						CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
						cairo_text_extents_t te;
						cairo_text_extents(cr, data->GetLabel(i), &te);
						cairo_move_to(cr, axes->GetInverseX() ? x2+axes->DrawingToDeviceX(text_space) :
							x2-te.width-axes->DrawingToDeviceX(text_space), axes->GetArea()->GetHeight()+te.height+axes->DrawingToDeviceX(text_space));							
						CairoSetColor(cr, COLOR_HTML_BLACK);
						cairo_show_text(cr, data->GetLabel(i));						
						break;
					}
				}				
			}
		}
	}		
}


//--------------------------------- Методы класса VKmElement --------------------------------

/** Конструктор*/
VKmElement::VKmElement(VDataProviderPtr in_data, VAxesPtr in_axes, bool in_show_text)
{
	if (in_axes && in_data)
	{
		axes=in_axes;	
		data=in_data;
		show_text=in_show_text;
	}
	else
	{
		throw NULL_POINTER;
	}	
}

/** Рисование*/
void VKmElement::Paint(cairo_t * cr)
{
	if (axes)
	{		
		axes->GetArea()->Apply(cr, CLIP_HORIZONTAL);	
		cairo_set_antialias(cr,antialias);
		CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));		
		CairoSetColor(cr, COLOR_HTML_BLACK);

		std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());		
		if (visible.size()<2)
			return;

		for (size_t i=visible.front(); i<=visible.back(); ++i)
		{
			double x=axes->ParamToAreaX(data->GetX(i));

			//Если источник данных - список объектов
			//Расшифровываем флаги определяя тип объекта и его расположение
			uint32_t type=hi_word(data->GetFlags(i));
			side_t side=static_cast<side_t>(low_word(data->GetFlags(i)));
			if (type==kms_1)
			{
				cairo_save(cr);	
				cairo_translate(cr, x, 0);
				extents_t e=draw_km(cr, axes, side);
				cairo_restore(cr);
				if (show_text)
				{
					CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD));
					cairo_text_extents_t te;
					cairo_text_extents(cr, data->GetLabel(i), &te);
					cairo_move_to(cr, x-te.width/2, e.y2-e.y1+te.height+axes->DrawingToDeviceX(text_space));		
					cairo_show_text(cr, data->GetLabel(i));						
				}				
			}			
		}
	}		
}

//-------------------------------- Методы класса VPkElement --------------------------------------------

/** Конструктор*/
VPkElement::VPkElement(VDataProviderPtr in_data, VAxesPtr in_axes, bool in_show_text)
{
	if (in_axes && in_data)
	{
		axes=in_axes;	
		data=in_data;
		show_text=in_show_text;
	}
	else
	{
		throw NULL_POINTER;
	}	
}

/** Рисование*/
void VPkElement::Paint(cairo_t * cr)
{

	if (axes)
	{		
		axes->GetArea()->Apply(cr, CLIP_HORIZONTAL);		
		cairo_set_antialias(cr,antialias);
		CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));		
		CairoSetColor(cr, COLOR_HTML_BLACK);

		std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
		if (visible.size()<2)
			return;

		for (size_t i=visible.front(); i<visible.back(); ++i)
		{
			double x1=axes->ParamToAreaX(data->GetX(i));
			double x2=axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i));

			//Если источник данных - априорная информация
			switch (data->GetFlags(i))
			{
				case COORDINATE_KM_5://Километр
				case COORDINATE_KM://Километр
				{	
					if (show_text)
					{
						CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD));
						cairo_text_extents_t te;
						cairo_text_extents(cr, data->GetLabel(i), &te);
						cairo_move_to(cr, x1+axes->DrawingToDeviceX(text_space), te.height+axes->DrawingToDeviceX(text_space));
						cairo_show_text(cr, data->GetLabel(i));						
					}
					break;
				}
				case COORDINATE_PK://Пикет
				{					
					if (show_text)
					{
						CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
						cairo_text_extents_t te;
						cairo_text_extents(cr, data->GetLabel(i), &te);
						cairo_move_to(cr, x2-te.width-axes->DrawingToDeviceX(text_space), te.height+axes->DrawingToDeviceX(text_space));
						cairo_show_text(cr, data->GetLabel(i));						
					}					
					break;					
				}
				default:
				{					
					break;
				}
			}
		}
	}
}

//--------------------------------- Методы класса VConstantLineYElement -------------------------------

/** Конcтруктор*/
VConstantLineYElement::VConstantLineYElement(VDataProviderPtr in_data, VAxesPtr in_axes, VDash in_dash, 
											 VColor in_color, double in_width, int in_clip, bool in_show_labels)
{
	if (in_data && in_axes)
	{
		axes=in_axes;
		data=in_data;	
		dash=in_dash;
		color=in_color;
		width=in_width;
		clip=in_clip;
		show_labels=in_show_labels;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VConstantLineYElement::Paint(cairo_t * cr)
{
	if (axes)
	{			
		axes->GetArea()->Apply(cr, clip);
		cairo_set_antialias(cr,antialias);
		CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));
		CairoSetDash(cr, axes->DrawingToDeviceY(dash.On()/10.0), axes->DrawingToDeviceY(dash.Off()/10.0));
		CairoSetColor(cr, color);

		double y=axes->ParamToAreaY(data->GetY(0));
		
		cairo_move_to(cr, 0, y);
		cairo_line_to(cr, axes->GetArea()->GetWidth(), y);
		cairo_stroke(cr);

		if (show_labels)
		{
			CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
			cairo_text_extents_t te;
			cairo_text_extents (cr, data->GetLabel(0), &te);
			cairo_move_to(cr, axes->GetArea()->GetWidth()+axes->DrawingToDeviceX(text_space), y+te.height/2.0);		
			cairo_show_text(cr, data->GetLabel(0));
		}
		axes->GetArea()->Restore(cr);
	}
}


//----------------------------------- Методы класса VRangeYElement --------------------------------

/** Рисование*/
void VRangeYElement::Paint(cairo_t * cr)
{	
	axes->GetArea()->Apply(cr, CLIP_AREA);		
	cairo_set_antialias(cr,antialias);	
	double min_y=axes->ParamToAreaY(data->GetMinY());
	double max_y=axes->ParamToAreaY(data->GetMaxY());
	cairo_rectangle(cr, -1, min_y, axes->GetArea()->GetWidth()+2, max_y-min_y);
	CairoSetColor(cr, fill_color);
	cairo_fill_preserve(cr);
	CairoSetColor(cr, color);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));	
	cairo_stroke(cr);
	axes->GetArea()->Restore(cr);
}

/** Конструктор*/
VRangeYElement::VRangeYElement(VDataProviderPtr in_data, VAxesPtr in_axes, VColor in_color, VColor in_fill_color)
{
	if (in_data && in_axes)
	{
		axes=in_axes;
		data=in_data;		
		color=in_color;
		fill_color=in_fill_color;
	}
	else
	{
		throw NULL_POINTER;
	}
}

//---------------------------------- Методы класса VCurvesElement ----------------------------------

/** Конструктор*/
VCurvesElement::VCurvesElement(VCurvesInfoPtr in_data, VAxesPtr in_axes)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VCurvesElement::Paint(cairo_t * cr)
{
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));

	double zero=axes->GetArea()->GetHeight()/2;
	double curve_height=90*zero/100;
	double radius_height=curve_height-5;

	cairo_move_to(cr, -10, axes->GetArea()->GetHeight()/2);

	for (size_t i=0; i<data->GetCount(); ++i)
	{
		if (data->Valid(i))
		{
			//Определяем границы кривой
			double start=axes->ParamToAreaX(data->GetAbsStart(i));
			double end=axes->ParamToAreaX(data->GetAbsEnd(i));
			
			//Если хотя бы часть кривой попадает в экран
			if (!(start>axes->GetArea()->GetWidth() || end<0))
			{					
				//Определяем ориентацию кривой
				int orientation=data->GetOrientation(i);
				//Рисуем обозначение кривой					
				CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));
				cairo_line_to(cr, start, zero);
				cairo_line_to(cr, start, zero-orientation*curve_height);
				cairo_line_to(cr, end, zero-orientation*curve_height);
				cairo_line_to(cr, end, zero);						
				cairo_stroke(cr);
				//Рисуем каждый радиус
				for (size_t j=0; j<data->GetRadiusCount(i); ++j)
				{
					double radius_start=axes->ParamToAreaX(data->GetAbsStart(i,j));
					double radius_end=axes->ParamToAreaX(data->GetAbsEnd(i,j));
					double radius_length=radius_end-radius_start;
					double trans_length=std::min<double>(radius_length/5, radius_height);
					double radius_middle=(radius_end+radius_start)/2;

					CairoSetLineWidth(cr, 2);				
					cairo_move_to(cr, radius_start, zero);
					cairo_line_to(cr, radius_start+trans_length, zero-orientation*radius_height);
					cairo_line_to(cr, radius_end-trans_length, zero-orientation*radius_height);
					cairo_line_to(cr, radius_end, zero);
					cairo_stroke(cr);

					//Пересчитываем угол
					angle_t a(data->GetAngle(i,j));

					//Выводим параметры кривой
					CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));				
					std::wstring text1 = boost::str(boost::wformat(L"R=%0.0f H=%0.0f") %  data->GetRadius(i,j) % data->GetLevel(i,j));
					std::wstring text2 = boost::str(boost::wformat(L"K=%0.0f A=%s") %  data->GetLength(i,j) % a.format_degrees());
					std::wstring text3 = boost::str(boost::wformat(L"L1=%0.0f L2=%0.0f") %  data->GetLength1(i,j) % data->GetLength2(i,j));
					cairo_text_extents_t te1;
					cairo_text_extents_t te2;
					cairo_text_extents_t te3;
					cairo_text_extents (cr, text1, &te1);
					cairo_text_extents (cr, text2, &te2);
					cairo_text_extents (cr, text3, &te3);
					//Если текст помещается в границы кривой
					if (te1.width<radius_length-trans_length*2 && te2.width<radius_length-trans_length*2  && te3.width<radius_length-trans_length*2)
					{
						cairo_move_to(cr, radius_middle-te1.width/2, zero-te2.height/2);
						cairo_show_text(cr, text1);
						cairo_move_to(cr, radius_middle-te2.width/2, zero+te2.height/2);
						cairo_show_text(cr, text2);
						cairo_move_to(cr, radius_middle-te3.width/2, zero+3*te2.height/2);
						cairo_show_text(cr, text3);
					}
					else//Иначе пробуем вывести сокращенный текст
					{
						text1 = boost::str(boost::wformat(L"R=%0.0f") %  data->GetRadius(i,j));
						text2 = boost::str(boost::wformat(L"H=%0.0f") %  data->GetLevel(i,j));
						cairo_text_extents (cr, text1, &te1);
						cairo_text_extents (cr, text2, &te2);
						if (te1.width<radius_length-trans_length*2 && te2.width<radius_length-trans_length*2)
						{
							cairo_move_to(cr, radius_middle-te1.width/2, zero-axes->DrawingToDeviceX(text_space));
							cairo_show_text(cr, text1);
							cairo_move_to(cr, radius_middle-te2.width/2, zero+axes->DrawingToDeviceX(text_space)+te2.height);
							cairo_show_text(cr, text2);
						}
					}
					//Выводим границы радиусов
					std::wstring text4 = boost::str(boost::wformat(L"%0.0f") %  data->GetMStart(i,j));
					std::wstring text5 = boost::str(boost::wformat(L"%0.0f") %  data->GetMEnd(i,j));
					cairo_text_extents_t te4;
					cairo_text_extents_t te5;
					cairo_text_extents (cr, text4, &te4);
					cairo_text_extents (cr, text5, &te5);				
					cairo_save(cr);
					//Начало
					(orientation<0) ? cairo_move_to(cr, radius_start+te3.height+axes->DrawingToDeviceX(text_space), 
						zero) : cairo_move_to(cr, radius_start+axes->DrawingToDeviceX(text_space), zero);
					cairo_rotate(cr, orientation * M_PI/2);
					cairo_show_text(cr, text4);
					cairo_restore(cr);
					//Конец
					if (j==data->GetRadiusCount(i)-1)
					{
						cairo_save(cr);
						(orientation<0) ? cairo_move_to(cr, radius_end-axes->DrawingToDeviceX(text_space), zero) : 
							cairo_move_to(cr, radius_end-te4.height-axes->DrawingToDeviceX(text_space), zero);
						cairo_rotate(cr, orientation * M_PI/2);
						cairo_show_text(cr, text5);
						cairo_restore(cr);
					}								
				}			

				//Переходим в конец кривой
				cairo_move_to(cr, end, zero);						
			}
		}
	}
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));
	cairo_line_to(cr, axes->GetArea()->GetWidth()+10, axes->GetArea()->GetHeight()/2);
	cairo_stroke(cr);

	axes->GetArea()->Restore(cr);
}


//---------------------------------- Методы класса VCurvesInfoElement ----------------------------------

/** Конструктор*/
VCurvesInfoElement::VCurvesInfoElement(VCurvesInfoPtr in_data, VAxesPtr in_axes)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VCurvesInfoElement::Paint(cairo_t * cr)
{
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	
	double zero=axes->ParamToAreaY(0);	
	for (size_t i=0; i<data->GetCount(); ++i)
	{
		if (data->Valid(i))
		{
			//Определяем границы кривой
			double start=axes->ParamToAreaX(data->GetAbsStart(i));
			double end=axes->ParamToAreaX(data->GetAbsEnd(i));			
			//Если хотя бы часть кривой попадает в экран
			if (!(start>axes->GetArea()->GetWidth() || end<0))
			{					
				//Определяем ориентацию кривой
				int orientation=data->GetOrientation(i);
				//Рисуем рамку
				CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));
				CairoSetColor(cr, VColor(0,0,0));
				double top = (orientation==SD_LEFT) ? 0 : zero;
				double bottom = (orientation==SD_LEFT) ? zero : axes->GetArea()->GetTop()+axes->GetArea()->GetHeight();
				cairo_rectangle(cr, start, top, end-start, bottom-top);								
				cairo_stroke(cr);

				//Выводим текст
				std::wstring number=boost::str(boost::wformat(L"№ %d")% (i+1));
				cairo_text_extents_t te;
				cairo_text_extents(cr, number, &te);
				orientation==SD_LEFT ?
					cairo_move_to(cr, (start+end)/2 - te.width/2, zero-axes->DrawingToDeviceY(text_space)) :
					cairo_move_to(cr, (start+end)/2 - te.width/2, zero+te.height+axes->DrawingToDeviceY(text_space));
				cairo_show_text(cr, number);				
			}
		}
	}	
	axes->GetArea()->Restore(cr);
}


//---------------------------------- Методы класса VCurvesPassportElement ----------------------------------

/** Конструктор*/
VCurvesPassportElement::VCurvesPassportElement(VCurvesInfoPtr in_data, VAxesPtr in_axes)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VCurvesPassportElement::Paint(cairo_t * cr)
{
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	
	CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));				
	

	double zero=axes->ParamToAreaY(0);	
	for (size_t i=0; i<data->GetCount(); ++i)
	{
		if (data->Valid(i))
		{
			//Определяем ориентацию кривой
			int orientation=data->GetOrientation(i);
			//Определяем границы кривой
			double left=axes->ParamToAreaX(data->GetAbsStart(i));
			double right=axes->ParamToAreaX(data->GetAbsEnd(i));
			double top=orientation==SD_LEFT ? 0 : zero;
			double bottom=orientation==SD_LEFT ? zero : axes->GetArea()->GetHeight();
			double width=right-left;
			double height=bottom-top;
			//Если хотя бы часть кривой попадает в экран
			if (!(left>axes->GetArea()->GetWidth() || right<0))
			{									
				//Рисуем общую информацию
				CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));
				CairoSetColor(cr, VColor(0,0,0));													
				cairo_rectangle(cr, left, top, width, 2*height/3);
				cairo_stroke_preserve(cr);

				cairo_save(cr);
				cairo_clip(cr);
				//Номер и статус
				std::wstring number=boost::str(boost::wformat(L"№ %d %s")% (i+1) % data->GetStatus(i));
				cairo_text_extents_t te;
				cairo_text_extents(cr, number, &te);
				cairo_move_to(cr, left + axes->DrawingToDeviceX(text_space), top + axes->DrawingToDeviceX(text_space) + te.height);
				cairo_show_text(cr, number);				
				//Скорости							
				std::wstring speeds=boost::str(boost::wformat(L"Скорости: %s/%s/%s/%s/%s")
					% data->GetModelVhigh_bt(i).format() % data->GetModelVhigh(i).format()
					% data->GetModelVpass(i).format() % data->GetModelVcrg(i).format() % data->GetModelVavg(i).format());
				cairo_move_to(cr, left + axes->DrawingToDeviceX(text_space), top + (axes->DrawingToDeviceX(text_space) + te.height)*2);
				cairo_show_text(cr, speeds);
				//Угол поворота
				std::wstring angle=boost::str(boost::wformat(L"Угол %s") % angle_t(data->GetAngle(i,COMPLETE_CURVE)).format_degrees());
				cairo_move_to(cr, left + axes->DrawingToDeviceX(text_space), top + (axes->DrawingToDeviceX(text_space) + te.height)*3);
				cairo_show_text(cr, angle);
				//Максимальное отклонение
				double max_overstep=data->GetMaxPlanOverstep(i);
				std::wstring overstep=boost::str(boost::wformat(L"Макс. откл. %0.2f") % max_overstep);
				cairo_move_to(cr, left + axes->DrawingToDeviceX(text_space), top + (axes->DrawingToDeviceX(text_space) + te.height)*4);								
				cairo_save(cr);
				if (fabs(max_overstep)>OPTIONS.GetOption(L"MAX_PLAN_OFFSET").GetValue<double>())
				{
					CairoSetColor(cr, VColor(255,0,0));
				}
				cairo_show_text(cr, overstep);
				cairo_restore(cr);

				cairo_restore(cr);


				//Рисуем таблицу - каждая ячейка - переходная или круговая
				for (size_t j=0; j<data->GetCurve(i).GetMarkCount()-1; ++j)
				{
					//Ширина ячейки					
					double cell_width=width/(data->GetCurve(i).GetMarkCount()-1);
					double cell_height=height/3;
					double cell_left=left+cell_width*j;
					double cell_top=top+2*cell_height;
					cairo_rectangle(cr, cell_left, cell_top, cell_width, cell_height);
					cairo_stroke_preserve(cr);
					cairo_save(cr);
					cairo_clip(cr);
					//Длина элемента
					std::wstring length=boost::str(boost::wformat(L"L=%0.1f") % data->GetElementLength(i,j));
					cairo_text_extents_t te;
					cairo_text_extents(cr, length, &te);
					cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + axes->DrawingToDeviceX(text_space) + te.height);
					cairo_show_text(cr, length);				
					//Радиус элемента
					if (data->GetElementType(i,j))
					{
						std::wstring radius=boost::str(boost::wformat(L"R=%0.0f") % data->GetElementRadius(i,j));					
						cairo_text_extents(cr, radius, &te);
						cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*2);
						cairo_show_text(cr, radius);
					}

					cairo_restore(cr);
				}								
			}
		}
	}	
	axes->GetArea()->Restore(cr);
}

//---------------------------------- Методы класса VLevelPassportElement ----------------------------------

/** Конструктор*/
VLevelPassportElement::VLevelPassportElement(VCurvesInfoPtr in_data, VAxesPtr in_axes)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VLevelPassportElement::Paint(cairo_t * cr)
{
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));				
	

	double zero=axes->ParamToAreaY(0);	
	for (size_t i=0; i<data->GetCount(); ++i)
	{
		if (data->Valid(i))
		{
			//Определяем ориентацию кривой
			int orientation=data->GetOrientation(i);
			//Определяем границы кривой
			double left=axes->ParamToAreaX(data->GetAbsLevelStart(i));
			double right=axes->ParamToAreaX(data->GetAbsLevelEnd(i));
			double top=orientation==SD_LEFT ? 0 : zero;
			double bottom=orientation==SD_LEFT ? zero : axes->GetArea()->GetHeight();
			double width=right-left;
			double height=bottom-top;
			//Если хотя бы часть кривой попадает в экран
			if (!(left>axes->GetArea()->GetWidth() || right<0))
			{			
				CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));
				CairoSetColor(cr, VColor(0,0,0));
				//Рисуем таблицу - каждая ячейка - переходная или круговая
				for (size_t j=0; j<data->GetCurve(i).GetMarkCount()-1; ++j)
				{
					//Ширина ячейки					
					double cell_width=width/(data->GetCurve(i).GetMarkCount()-1);
					double cell_height=height;
					double cell_left=left+cell_width*j;
					double cell_top=top;
					cairo_rectangle(cr, cell_left, cell_top, cell_width, cell_height);
					cairo_stroke_preserve(cr);
					cairo_save(cr);
					cairo_clip(cr);		
					
					cairo_text_extents_t te;
					//Длина и возвышение элемента
					if (data->GetElementType(i,j))
					{
						std::wstring text=boost::str(boost::wformat(L"L=%0.1f H=%0.0f") % data->GetElementLengthLevel(i,j) % data->GetElementLevel(i,j));
						cairo_text_extents(cr, text, &te);											
						cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + axes->DrawingToDeviceX(text_space) + te.height);
						cairo_show_text(cr, text);
					}
					else
					{
						std::wstring text=boost::str(boost::wformat(L"L=%0.1f") % data->GetElementLengthLevel(i,j));						
						cairo_text_extents(cr, text, &te);
						cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + axes->DrawingToDeviceX(text_space) + te.height);
						cairo_show_text(cr, text);				
					}
					//Если это круговая
					if (data->GetElementType(i,j))
					{
						double line_number=2;
						//Скор. НК
						if (data->GetModelVhigh_bt(i)!=NO_SPEED)
						{
							std::wstring anp=boost::str(boost::wformat(L"Aнп снк=%0.2f") % data->GetModelAnpHighBT(i,j));											
							cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);
							if (VModelAnpHighCondition().TestValue(data->GetModelAnpHighBT(i,j)))
								CairoShowText(cr, anp, VColor(255,0,0));
							else
								CairoShowText(cr, anp);
						}
						//Скор.
						if (data->GetModelVhigh(i)!=NO_SPEED)
						{
							std::wstring anp=boost::str(boost::wformat(L"Aнп ск=%0.2f") % data->GetModelAnpHigh(i,j));											
							cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);
							if (VModelAnpHighCondition().TestValue(data->GetModelAnpHigh(i,j)))
								CairoShowText(cr, anp, VColor(255,0,0));
							else
								CairoShowText(cr, anp);
						}
						//Пасс.
						if (data->GetModelVpass(i)!=NO_SPEED)
						{
							std::wstring anp=boost::str(boost::wformat(L"Aнп пасс=%0.2f") % data->GetModelAnpPass(i,j));											
							cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);
							//FixMe!!! true или false, Average или Max - вопрос БНЗ
							//FixMe!!! Лучше добавить в VCurve и провайдер методы возвращающие признаки
							if (VModelAnpPassCondition().TestValue(data->GetModelAnpPass(i,j)))
								CairoShowText(cr, anp, VColor(255,0,0));
							else
								CairoShowText(cr, anp);
						}
						//Ср.
						if (data->GetModelVavg(i)!=NO_SPEED)
						{
							std::wstring anp=boost::str(boost::wformat(L"Aнп ср=%0.2f") % data->GetModelAnpAvg(i,j));											
							cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);						
							if (VModelAnpTruckCondition().TestValue(data->GetModelAnpAvg(i,j)))
								CairoShowText(cr, anp, VColor(255,0,0));
							else
								CairoShowText(cr, anp);
						}
						//Vкр снк
						if (data->GetModelVhigh_bt(i)!=NO_SPEED)
						{
							std::wstring vkr=boost::str(boost::wformat(L"Vкр снк=%0.0f") % data->GetModelVkr_high_bt(i,j));											
							cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);
							if (data->GetModelVkr_high_bt(i,j) < data->GetModelVhigh_bt(i))
								CairoShowText(cr, vkr, VColor(255,0,0));
							else
								CairoShowText(cr, vkr);
						}
						//Vкр ск
						if (data->GetModelVhigh(i)!=NO_SPEED)
						{
							std::wstring vkr=boost::str(boost::wformat(L"Vкр ск=%0.0f") % data->GetModelVkr_high(i,j));											
							cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);
							if (data->GetModelVkr_high(i,j) < data->GetModelVhigh(i))
								CairoShowText(cr, vkr, VColor(255,0,0));
							else
								CairoShowText(cr, vkr);
						}
						//Vкр пасс
						if (data->GetModelVpass(i)!=NO_SPEED)
						{
							std::wstring vkr=boost::str(boost::wformat(L"Vкр пасс=%0.0f") % data->GetModelVkr_pass(i,j));											
							cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);
							if (data->GetModelVkr_pass(i,j) < data->GetModelVpass(i))
								CairoShowText(cr, vkr, VColor(255,0,0));
							else
								CairoShowText(cr, vkr);
						}
					}	
					//Если это переходная
					else
					{
						double line_number=2;
						//Vпр
						std::wstring vpr=boost::str(boost::wformat(L"Vпр=%0.0f") % data->GetModelVpr(i,j));											
						cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);						
						if (data->GetModelVpr(i,j) < data->GetModelVpass(i))
							CairoShowText(cr, vpr, VColor(255,0,0));
						else
							CairoShowText(cr, vpr);
						//Vиз снк
						if (data->GetModelVhigh_bt(i)!=NO_SPEED)
						{
							std::wstring viz=boost::str(boost::wformat(L"Vиз снк=%0.0f") % data->GetModelViz_high_bt(i,j));											
							cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);						
							if (data->GetModelViz_high_bt(i,j) < data->GetModelVhigh_bt(i))
								CairoShowText(cr, viz, VColor(255,0,0));
							else
								CairoShowText(cr, viz);
						}
						//Vиз ск
						if (data->GetModelVhigh(i)!=NO_SPEED)
						{
							std::wstring viz=boost::str(boost::wformat(L"Vиз ск=%0.0f") % data->GetModelViz_high(i,j));											
							cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);						
							if (data->GetModelViz_high(i,j) < data->GetModelVhigh(i))
								CairoShowText(cr, viz, VColor(255,0,0));
							else
								CairoShowText(cr, viz);
						}
						//Vиз пасс
						if (data->GetModelVpass(i)!=NO_SPEED)
						{
							std::wstring viz=boost::str(boost::wformat(L"Vиз пасс=%0.0f") % data->GetModelViz_pass(i,j));											
							cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);						
							if (data->GetModelViz_pass(i,j) < data->GetModelVpass(i))
								CairoShowText(cr, viz, VColor(255,0,0));
							else
								CairoShowText(cr, viz);
						}
						//Отвод возвышения
						std::wstring tap=boost::str(boost::wformat(L"Отв.=%0.2f") % data->GetModelTap(i,j));											
						cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);
						cairo_show_text(cr, tap);
						//Пси снк
						if (data->GetModelVhigh_bt(i)!=NO_SPEED)
						{
							std::wstring psi=boost::str(boost::wformat(L"Пси снк=%0.2f") % data->GetModelPsi_high_bt(i,j));					
							cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);
							if (VModelPsiHighCondition().TestValue(data->GetModelPsi_high_bt(i,j)))
								CairoShowText(cr, psi, VColor(255,0,0));
							else
								CairoShowText(cr, psi);
						}
						//Пси ск
						if (data->GetModelVhigh(i)!=NO_SPEED)
						{
							std::wstring psi=boost::str(boost::wformat(L"Пси ск=%0.2f") % data->GetModelPsi_high(i,j));					
							cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);
							if (VModelPsiHighCondition().TestValue(data->GetModelPsi_high(i,j)))
								CairoShowText(cr, psi, VColor(255,0,0));
							else
								CairoShowText(cr, psi);
						}
						//Пси пасс.
						if (data->GetModelVpass(i)!=NO_SPEED)
						{
							std::wstring psi=boost::str(boost::wformat(L"Пси пасс=%0.2f") % data->GetModelPsi_pass(i,j));					
							cairo_move_to(cr, cell_left + axes->DrawingToDeviceX(text_space), cell_top + (axes->DrawingToDeviceX(text_space) + te.height)*line_number++);
							if (VModelPsiPassCondition().TestValue(data->GetModelPsi_pass(i,j)))
								CairoShowText(cr, psi, VColor(255,0,0));
							else
								CairoShowText(cr, psi);
						}
					}
					cairo_restore(cr);
				}								
			}
		}
	}	
	axes->GetArea()->Restore(cr);
}

//---------------------------------- Методы класса VLinesElement ----------------------------------

/** Конструктор*/
VLinesElement::VLinesElement(VLinesDataPtr in_data, VAxesPtr in_axes)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VLinesElement::Paint(cairo_t * cr)
{
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));
	CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
	double zero=axes->GetArea()->GetHeight()/2;
		
	for (size_t i=0; i<data->GetCount(); ++i)
	{
		double left_x=axes->ParamToAreaX(data->GetX(i));
		double right_x=axes->ParamToAreaX(data->GetX(i)+data->GetSizeX(i));

		if (!(left_x>axes->GetArea()->GetWidth() || right_x<0))
		{

			cairo_move_to(cr, left_x, zero+axes->DrawingToDeviceY(text_space));
			cairo_line_to(cr, left_x, zero-axes->DrawingToDeviceY(text_space));

			cairo_move_to(cr, left_x, zero);
			cairo_line_to(cr, right_x, zero);

			cairo_move_to(cr, right_x, zero+axes->DrawingToDeviceY(text_space));
			cairo_line_to(cr, right_x, zero-axes->DrawingToDeviceY(text_space));

			cairo_stroke(cr);

			//Длина прямой
			std::wstring label(data->GetLabel(i));
			cairo_text_extents_t te;
			cairo_text_extents (cr, label, &te);			
			double text_pos=(std::max<double>(left_x, 0) + std::min<double>(right_x, axes->GetArea()->GetWidth()))/2
					-te.width/2;
			if (te.width<right_x-left_x)
			{
				cairo_move_to(cr, text_pos, zero-axes->DrawingToDeviceX(text_space));
				cairo_show_text(cr, label);
			}

			//Угол между смежными прямыми			
			/*double angle_rad=data->GetY(i);
			if (angle_rad!=0)
			{
				angle_t angle(angle_rad);
				label=boost::str(boost::wformat(L"%d°%d'") % angle.get_grad() % angle.get_min());				
				cairo_text_extents (cr, label, &te);			
				cairo_move_to(cr, left_x-te.width/2, zero-2*axes->DrawingToDeviceY(text_space));
				cairo_show_text(cr, label);				
			}*/
		}
	}	
}

//---------------------------------- Методы класса VLegendElement ----------------------------------

/** Конструктор*/
VLegendElement::VLegendElement(VAxesPtr in_axes)
{
	if (in_axes==NULL)
		throw NULL_POINTER;
	axes=in_axes;	
	hit=false;
}

/** Добавляет график в легенду*/
void VLegendElement::AddGraph(std::wstring in_name, VColor in_color)
{	
	names.push_back(in_name);
	colors.push_back(in_color);
}

/** Рисование*/
void VLegendElement::Paint(cairo_t * cr)
{
	if (axes)
	{		
		double font_space=axes->DrawingToDeviceY(text_space/10.0);
		axes->GetArea()->Apply(cr, CLIP_AREA);		
		CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));		
		for (size_t i=0; i<names.size(); i++)
		{
			VColor color=colors.at(i);
			CairoSetColor(cr, color);
			std::wstring name=names.at(i);
			cairo_move_to (cr, axes->DrawingToDeviceX(text_space), (axes->DrawingToDeviceY(BASE_FONT_SIZE().Value())+font_space)*(i+1));
			cairo_show_text(cr, name);
		}		
		axes->GetArea()->Restore(cr);
	}
}

//---------------------------------- Методы класса VAreaFrameElement ----------------------------------

/** Конструктор*/
VAreaFrameElement::VAreaFrameElement(VAreaPtr in_area, VDrawingDevicePtr in_device, double in_width, uint32_t in_borders)
{
	if (!in_area || !in_device)
		throw NULL_POINTER;
	area=in_area;
	device=in_device;
	width=in_width;
	borders=in_borders;
}

/** Рисование*/
void VAreaFrameElement::Paint(cairo_t * cr)
{
	if (area)
	{
		cairo_set_antialias(cr,antialias);				
		//Рамка		
		cairo_save(cr);		
		area->GetActive() ? CairoSetLineWidth(cr, device->DrawingToDeviceX(heavy_line_width/10.0)) : CairoSetLineWidth(cr, device->DrawingToDeviceX(width/10.0));		
		CairoSetColor(cr, COLOR_HTML_BLACK);
		area->Apply(cr, CLIP_NONE);
		if (borders & AREA_FRAME_TOP)
		{
			cairo_move_to(cr, 0, 0);
			cairo_line_to(cr, area->GetWidth(), 0);
		}
		if (borders & AREA_FRAME_LEFT)
		{
			cairo_move_to(cr, 0, 0);
			cairo_line_to(cr, 0, area->GetHeight());
		}
		if (borders & AREA_FRAME_BOTTOM)
		{
			cairo_move_to(cr, 0, area->GetHeight());
			cairo_line_to(cr, area->GetWidth(), area->GetHeight());
		}
		if (borders & AREA_FRAME_RIGHT)
		{
			cairo_move_to(cr, area->GetWidth(), 0);
			cairo_line_to(cr, area->GetWidth(), area->GetHeight());
		}
		cairo_stroke(cr);		
		cairo_restore(cr);
	}
}

//--------------------------------- Методы класса VAreaNameElement ---------------------------------

/** Конструктор*/
VAreaNameElement::VAreaNameElement(VAreaPtr in_area, VDrawingDevicePtr in_device, const std::wstring& in_name)
{
	if (!in_area)
		throw NULL_POINTER;
	area=in_area;
	device=in_device;
	name=in_name;
}

/** Рисование*/
void VAreaNameElement::Paint(cairo_t * cr)
{
	if (area)
	{
		cairo_save(cr);
		cairo_set_antialias(cr,antialias);										
		CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, device->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
		cairo_text_extents_t te;
		cairo_text_extents(cr, name, &te);		
		CairoSetLineWidth(cr, device->DrawingToDeviceX(default_line_width/10.0));
		cairo_rectangle(cr, 0, area->GetTop(), area->GetLeft(), area->GetHeight());
		cairo_stroke(cr);
		cairo_move_to(cr, area->GetLeft()/2 - te.width/2, area->GetTop() + area->GetHeight()/2 + te.height/2);
		cairo_show_text(cr, name);		
		cairo_restore(cr);
	}
}

//--------------------------------- Методы класса VKVLPAreaNameElement ---------------------------------

/** Конструктор*/
VKVLPAreaNameElement::VKVLPAreaNameElement(VAreaPtr in_area, VDrawingDevicePtr in_device, const std::wstring& in_name, double in_width)
{
	if (!in_area)
		throw NULL_POINTER;
	area=in_area;
	device=in_device;
	name=in_name;
	width=in_width;
}

/** Рисование*/
void VKVLPAreaNameElement::Paint(cairo_t * cr)
{
	if (area)
	{
		cairo_save(cr);
		cairo_set_antialias(cr,antialias);
		CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, device->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
		cairo_text_extents_t te;
		cairo_text_extents(cr, name, &te);
		CairoSetLineWidth(cr, device->DrawingToDeviceX(width/10.0));
		double area_space=0.8;
		double frame_space=0.05;
		double frame_size=0.4;

		cairo_rectangle(cr, area->GetLeft()+area->GetWidth()+device->DrawingToDeviceX(area_space), 
			area->GetTop()+device->DrawingToDeviceY(frame_space), 
			device->DrawingToDeviceX(frame_size), area->GetHeight()-2*device->DrawingToDeviceY(frame_space));
		cairo_stroke(cr);		

		cairo_move_to(cr, area->GetLeft()+area->GetWidth()+device->DrawingToDeviceX(area_space+frame_size-text_space), 
			area->GetTop() + area->GetHeight()/2 + te.width/2);
		cairo_rotate(cr, -M_PI/2);
		cairo_show_text(cr, name);		
		
		cairo_restore(cr);
	}
}

//---------------------------------- Методы класса VTrackValueElement ----------------------------------

/** Конструктор*/
VTrackValueElement::VTrackValueElement(VAxesPtr in_axes, VDataProviderPtr in_data)
{	
	if (in_axes && in_data)
	{
		axes=in_axes;
		data=in_data;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Добавляет график в легенду*/
void VTrackValueElement::AddGraph(VDataProviderPtr in_data_provider, const VColor& in_color)
{	
	//Помещаем источник в список только в том случае, если GetYbyX реализован в провайдере				
	if (in_data_provider->GetCount()>0 && in_data_provider->GetYbyX(0)!=DBL_MAX)
	{		
		data_providers.push_back(in_data_provider);	
		colors.push_back(in_color);
	}
}

/** Рисование*/
void VTrackValueElement::Paint(cairo_t * cr)
{
	if (axes)
	{		
		axes->GetArea()->Apply(cr, CLIP_AREA);
		cairo_text_extents_t te;				
		CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));							
		for (size_t i=0; i<data_providers.size(); i++)
		{				
			CairoSetColor(cr, colors.at(i));			
			std::wstring text = boost::str( boost::wformat(L"%0.2f") % data_providers.at(i)->GetYbyX(axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(data->GetX(0)))));
			cairo_text_extents (cr, text, &te);
			cairo_move_to(cr, axes->GetArea()->GetWidth()-te.width-axes->DrawingToDeviceX(text_space), 
				(te.height+axes->DrawingToDeviceX(text_space))*(i+1));
			cairo_show_text(cr, text);			
		}
		axes->GetArea()->Restore(cr);
	}
}


//------------------------------------- Методы класса VTrackCoordElement ----------------------------------

/** Конструктор*/
VTrackCoordElement::VTrackCoordElement(VAxesPtr in_axes, VDataProviderPtr in_data, VAprinfoPtr in_aprinfo)
{
	if (in_axes && in_data && in_aprinfo)
	{
		axes=in_axes;
		data=in_data;
		aprinfo=in_aprinfo;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Рисование*/
void VTrackCoordElement::Paint(cairo_t * cr)
{
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_text_extents_t te;				
	CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));		
	
	VWayCoord wc=aprinfo->GetRealCoord(axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(data->GetX(0))));
	std::wstring text=boost::str(boost::wformat(L"%d км %0.0f м") % wc.km % wc.m);			

	cairo_text_extents(cr, text, &te);
	cairo_move_to(cr, axes->GetArea()->ScreenToAreaX(data->GetX(0)) + axes->DrawingToDeviceX(text_space), 
		axes->GetArea()->GetHeight() - axes->DrawingToDeviceY(text_space));
	cairo_show_text(cr, text);			
	axes->GetArea()->Restore(cr);	
}

//----------------------------------- Методы класса VSelectionElement ----------------------------------

/** Конструктор*/
VSelectionElement::VSelectionElement(VAxesPtr in_axes, VSelectionInfoPtr in_selection_info) : 	
	axes(in_axes), selection_info(in_selection_info), hit_left(false), hit_right(false), 
	left_side_x(false), right_side_x(false), shift_pressed(false) {}

/** Отрисовка области выделения*/
void VSelectionElement::Paint(cairo_t * cr)
{		
	if (selection_info->Valid())
	{
		axes->GetArea()->Apply(cr, CLIP_AREA);
		cairo_set_antialias(cr,antialias);
		double _start_x=axes->ParamToAreaX(selection_info->GetStartX());		
		double _end_x=axes->ParamToAreaX(selection_info->GetEndX());								
		CairoSetColor(cr, VColor(0,0,255,127));
		/*
		Обычно нужна область выделения во всю высоту области с графиком, однако
		для некоторых целей может потребоваться обычная область выделения. В этом случае можно реализовать 
		дополнительный режим, или другой конкретный класс.		
		*/
		cairo_rectangle(cr, std::min<double>(_start_x, _end_x), 0, fabs(_end_x-_start_x), axes->GetArea()->GetHeight());
		cairo_fill(cr);		
		if (selection_info->GetHit())
		{			
			CairoSetColor(cr, COLOR_HTML_BLACK);
			CairoSetLineWidth(cr, 2);
			cairo_move_to(cr, _start_x, 0);
			cairo_line_to(cr, _start_x, axes->GetArea()->GetHeight());								
			cairo_move_to(cr, _end_x, 0);
			cairo_line_to(cr, _end_x, axes->GetArea()->GetHeight());
			cairo_stroke(cr);
		}
		axes->GetArea()->Restore(cr);
	}
}

/** Обработка hit-test*/
bool VSelectionElement::HitTest(double in_x, double in_y)
{
	double hit_size=3;
	bool prev_hit_left=hit_left;
	bool prev_hit_right=hit_right;
	left_side_x=axes->GetArea()->AreaToScreenX(axes->ParamToAreaX(selection_info->GetStartX()));
	right_side_x=axes->GetArea()->AreaToScreenX(axes->ParamToAreaX(selection_info->GetEndX()));
	if (in_x>left_side_x-hit_size && in_x<left_side_x+hit_size && in_y>axes->GetArea()->GetTop() && in_y<axes->GetArea()->GetTop()+axes->GetArea()/*area*/->GetHeight())	
		hit_left=true;	
	else	
		hit_left=false;	
	if (in_x>right_side_x-hit_size && in_x<right_side_x+hit_size && in_y>axes->GetArea()/*area*/->GetTop() && in_y<axes->GetArea()->GetTop()+axes->GetArea()->GetHeight())	
		hit_right=true;	
	else	
		hit_right=false;		
	return hit_left!=prev_hit_left || hit_right!=prev_hit_right;
}

/** Обработка события*/
bool VSelectionElement::ProcessEvent(const VEvent& in_event)
{
	double x=((VMouseEvent&)in_event).GetX();
	double y=((VMouseEvent&)in_event).GetY();	
	bool changed=false;
	if (selection_info->GetActive() && axes->GetArea()->HitTest(x,y))
	{
		switch (in_event.GetCode())
		{			
			case EVENT_MOUSE_LEFT_DOWN:
			{			

				HitTest(x,y);			
				
				left_side_x=axes->GetArea()->AreaToScreenX(axes->ParamToAreaX(selection_info->GetStartX()));
				right_side_x=axes->GetArea()->AreaToScreenX(axes->ParamToAreaX(selection_info->GetEndX()));

				if (hit_left)
				{				
					selection_info->SetSelecting(true);			
					left_side_x=axes->GetArea()->AreaToScreenX(axes->ParamToAreaX(selection_info->GetStartX()));
					right_side_x=axes->GetArea()->AreaToScreenX(axes->ParamToAreaX(selection_info->GetEndX()));
					selection_info->SetStart(axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(right_side_x)), 0);
					selection_info->SetEnd(axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(left_side_x)), 0);
				}
				else if (hit_right)
				{														
					selection_info->SetSelecting(true);			
					selection_info->SetStart(axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(left_side_x)), y);
					selection_info->SetEnd(axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(right_side_x)), y);
				}			
				else
				{
					selection_info->Reset();
					selection_info->SetSelecting(true);			
					selection_info->SetStart(axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(x)),0);
					selection_info->SetEnd(axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(x)),0);
				}
				break;
			}
			case EVENT_MOUSE_LEFT_UP:
			{
				if (selection_info->GetSelecting())
				{
					selection_info->SetSelecting(false);
					if (!selection_info->Valid())
					{
						selection_info->Reset();
					}
				}
				changed=changed || true;
				break;
			}
			case EVENT_MOUSE_MOTION:
			{
				double x=((VMouseEvent&)in_event).GetX();
				double y=((VMouseEvent&)in_event).GetY();
				if (selection_info->GetSelecting())
				{							
					selection_info->SetEnd(axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(x)), 0);
					changed=changed || true;
				}
				else
				{
					changed=HitTest(x,y);
					selection_info->SetHit(hit_left || hit_right);
				}
				break;
			}
		}
	}
	return changed;
}

//---------------------------------- Методы класса VPageNumberElement ----------------------------------

/** Конструктор*/
VPageNumberElement::VPageNumberElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VPageInfoPtr in_page_info)
{	
	if (!in_area || !in_device)
		throw NULL_POINTER;
	area=in_area;
	device=in_device;
	page_info=in_page_info;
}

/** Рисование*/
void VPageNumberElement::Paint(cairo_t * cr)
{	
	if (area)
	{
		std::wstring text = boost::str(boost::wformat(L"%1%") % page_info->GetPage());
		cairo_text_extents_t te;
		cairo_set_antialias(cr,antialias);		
		CairoSetColor(cr, COLOR_HTML_BLACK);
		CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, device->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD));		
		cairo_text_extents (cr, text, &te);
		area->Apply(cr, CLIP_AREA);		
		cairo_move_to (cr, area->GetWidth()-te.width-device->DrawingToDeviceX(text_space), area->GetHeight()-device->DrawingToDeviceY(text_space));
		cairo_show_text(cr, text);
		area->Restore(cr);
	}
}


//----------------------------------- Методы класса VMouseCrossElement ---------------------------------------
/** Конструктор*/
VMouseCrossElement::VMouseCrossElement(VAxesPtr in_axes, VDataProviderPtr in_data)
{
	if (in_axes && in_data)
	{		
		axes=in_axes;
		data=in_data;
	}
	else
		throw NULL_POINTER;
};

/** Рисование*/
void VMouseCrossElement::Paint(cairo_t * cr)
{		
	cairo_set_antialias(cr, antialias);
	axes->GetArea()->Apply(cr, CLIP_AREA);
	
	x=axes->GetArea()->ScreenToAreaX(data->GetX(0));
	y=axes->GetArea()->ScreenToAreaY(data->GetY(0));
	value_x=axes->AreaToParamX(x);		
	value_y=axes->AreaToParamY(y);

	//Рисуем перекрестье	
	CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));	
	CairoSetColor(cr, COLOR_HTML_GRAY);

	cairo_move_to(cr, 0, y);
	cairo_line_to(cr, axes->GetArea()->GetWidth(), y);
	cairo_move_to(cr, x, 0);
	cairo_line_to(cr, x, axes->GetArea()->GetHeight());

	cairo_stroke(cr);
	//Выводим текст				
	cairo_text_extents_t te;	
	CairoSetColor(cr, COLOR_HTML_BLACK);
	CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));		
	std::wstring text = boost::str( boost::wformat(L"%0.2f:%0.2f") % value_x % value_y);
	cairo_text_extents(cr, text, &te);		
	cairo_move_to(cr, x+axes->DrawingToDeviceX(text_space), y-axes->DrawingToDeviceY(text_space));
	cairo_show_text(cr, text);

	axes->GetArea()->Restore(cr);
};

//-------------------------------- Методы класса VScaleElement ---------------------------------

/** Конструктор*/
VScaleElement::VScaleElement(VAxesPtr in_axes, bool in_show_scale_x, bool in_show_scale_y)
{
	if (in_axes)
	{
		axes=in_axes;			
		show_scale_x=in_show_scale_x;
		show_scale_y=in_show_scale_y;
	}
	else
		throw NULL_POINTER;
}

/** Рисование*/
void VScaleElement::Paint(cairo_t * cr)
{
	if (axes)
	{		
		axes->GetArea()->Apply(cr, CLIP_AREA);		
		cairo_set_antialias(cr,antialias);
		if (show_scale_x)
		{			
			//Рисуем линейку
			CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));
			double width=axes->DrawingToDeviceX(10.0/10.0);
			double height=axes->DrawingToDeviceY(1.5/10.0);
			double hspace=axes->DrawingToDeviceX(3.0/10.0);
			double vspace=axes->DrawingToDeviceY(3.0/10.0);
			double tspace=axes->DrawingToDeviceY(1.0/10.0);			
			cairo_rectangle(cr, hspace, axes->GetArea()->GetHeight()-vspace, width, height);
			cairo_stroke_preserve(cr);			
			cairo_fill(cr);
			cairo_rectangle(cr, hspace+width, axes->GetArea()->GetHeight()-vspace, width, height);
			cairo_stroke(cr);
			//Выводим текст						
			std::wstring text = boost::str(boost::wformat(L"1 см :%0.0f %s") % (axes->GetScaleX()) % axes->GetLegendX());
			cairo_text_extents_t te;					
			CairoSetColor(cr, COLOR_HTML_BLACK);
			CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));			
			cairo_text_extents (cr, text, &te);
			cairo_move_to (cr, hspace+2*width-te.width, axes->GetArea()->GetHeight()-tspace-vspace);
			cairo_show_text(cr, text);
		}
		if (show_scale_y)
		{						
			//Рисуем линейку			
			CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));
			double width=axes->DrawingToDeviceX(1.5/10.0);			
			double height=axes->DrawingToDeviceY(10.0/10.0);
			double hspace=axes->DrawingToDeviceX(1.0/10.0);
			double vspace=axes->DrawingToDeviceY(3.5/10.0);
			double tspace=axes->DrawingToDeviceX(1.0/10.0);			
			cairo_rectangle(cr, hspace, axes->GetArea()->GetHeight()-vspace-height, width, height);
			cairo_stroke_preserve(cr);			
			cairo_fill(cr);
			cairo_rectangle(cr, hspace, axes->GetArea()->GetHeight()-vspace-2*height, width, height);
			cairo_stroke(cr);
			//Выводим текст			
			double scale_y=axes->GetScaleY();
			std::wstring text = boost::str(boost::wformat(L"1 см :%0.0f %s") % scale_y % axes->GetLegendY());
			cairo_text_extents_t te;					
			CairoSetColor(cr, COLOR_HTML_BLACK);
			CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));			
			cairo_text_extents (cr, text, &te);
			cairo_move_to (cr, hspace+width+tspace, axes->GetArea()->GetHeight()-vspace-2*height);
			cairo_save(cr);
			cairo_rotate(cr, M_PI/2);
			cairo_show_text(cr, text);
			cairo_restore(cr);
		}
		axes->GetArea()->Restore(cr);
	}
}	


//----------------------------------- Методы класса VCurveMarkElement -----------------------------------

/** Конструктор*/
VCurveMarkElement::VCurveMarkElement(VDataProviderPtr in_data, VAxesPtr in_axes, size_t in_index, 
	const VColor& in_color,  const VColor& in_fill_color, int in_clip, bool in_show_label, 
	point_style_t in_style)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;
		index=in_index;
		clip=in_clip;
		ext_left=0;	
		ext_top=0;	
		ext_right=0;	
		ext_bottom=0;		
		show_label=in_show_label;
		style=in_style;
		drag=false;
		color=in_color;
		fill_color=in_fill_color;
		width=default_line_width;
	}
	else
	{
		throw NULL_POINTER;
	}
};

/** Рисование*/
void VCurveMarkElement::Paint(cairo_t * cr)
{	
	if (data->GetX(index)>axes->GetMinX() && data->GetX(index)<axes->GetMaxX())
	{					
		axes->GetArea()->Apply(cr, clip);
		cairo_set_antialias(cr,antialias);		
		CairoSetLineWidth(cr, axes->DrawingToDeviceX(width/10.0));
		double x=axes->ParamToAreaX(data->GetX(index));
		double y=axes->ParamToAreaY(data->GetY(index));
				
		double flag_size=axes->DrawingToDeviceX(BASE_FONT_SIZE().Value());
		VColor _color=color;
		VColor _fill_color=fill_color;				

		//Устанавливаем свойства элемента
		if (hi_word(data->GetFlags(index))==ACTIVE_FIRST_MARK || hi_word(data->GetFlags(index))==ACTIVE_LAST_MARK || hi_word(data->GetFlags(index))==ACTIVE_INNER_MARK)
		{			
			_fill_color=VColor(255,0,0);
		}
		if (data->GetSelectedPoint() == index)
		{
			_fill_color=color;
		}

		double sign=low_word(data->GetFlags(index))==SD_LEFT ? -1.0 : 1.0;
		
		//Рисуем "флагшток"
		CairoSetColor(cr, color);
		cairo_move_to(cr, x, y-sign*flag_size);
		cairo_line_to(cr, x, y);		
		cairo_stroke(cr);		

		//Рисуем флаг
		if (hi_word(data->GetFlags(index))==FIRST_MARK || hi_word(data->GetFlags(index))==ACTIVE_FIRST_MARK)
		{
			cairo_move_to(cr, x, y-sign*flag_size);
			cairo_line_to(cr, x, y-sign*flag_size*2);
			cairo_line_to(cr, x+flag_size, y-sign*flag_size*2);
			cairo_close_path(cr);
		}
		else if (hi_word(data->GetFlags(index))==LAST_MARK || hi_word(data->GetFlags(index))==ACTIVE_LAST_MARK)
		{
			cairo_move_to(cr, x, y-sign*flag_size);
			cairo_line_to(cr, x-flag_size, y-sign*flag_size*2);
			cairo_line_to(cr, x, y-sign*flag_size*2);
			cairo_close_path(cr);
		}
		else
		{
			cairo_move_to(cr, x, y-sign*flag_size);
			cairo_line_to(cr, x-flag_size/2, y-sign*flag_size*2);
			cairo_line_to(cr, x+flag_size/2, y-sign*flag_size*2);
			cairo_close_path(cr);
		}
		
		cairo_path_extents(cr, &ext_left, &ext_top, &ext_right, &ext_bottom);
		cairo_user_to_device(cr, &ext_left, &ext_top);
		cairo_user_to_device(cr, &ext_right, &ext_bottom);
							
		CairoSetColor(cr, _fill_color);			
		cairo_fill_preserve(cr);
		CairoSetColor(cr, _color);
		cairo_stroke(cr);
		
		//Выводим метку точки
		if (show_label)
		{			
			CairoSetColor(cr, COLOR_HTML_BLACK);
			CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));			
			cairo_move_to (cr, x+5 ,y-5);			
			std::wstring text=data->GetLabel(index);
			cairo_show_text(cr, text);			
		}
		axes->GetArea()->Restore(cr);			

	}	
};

/** Обработчик HitTest*/
bool VCurveMarkElement::HitTest(double in_x, double in_y)
{
	return in_x>ext_left && in_x<ext_right && in_y>ext_top && in_y<ext_bottom;	
}

/** Обработка события*/
bool VCurveMarkElement::ProcessEvent(const VEvent& in_event)
{	
	bool result = false;
	if (data->GetActive() && GetActive())
	{
		switch (in_event.GetCode())
		{
			case EVENT_MOUSE_MOTION:
			{
				double x=((VMouseEvent&)in_event).GetX();
				double y=((VMouseEvent&)in_event).GetY();				
				if (HitTest(x, y))
				{
					data->SelectPoint(index);
				}				
				if (drag)
				{
					data->SetXY(index, axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(x)) - drag_shift_x,
					   axes->AreaToParamY(axes->GetArea()->ScreenToAreaY(y)) - drag_shift_y);
					result = true;
				}
				break;
			}
			case EVENT_MOUSE_LEFT_DOWN:			
			{			
				double x=((VMouseEvent&)in_event).GetX();
				double y=((VMouseEvent&)in_event).GetY();
				if (HitTest(x, y))
				{		
					data->MarkPoints(index, index);
					//Начинаем перетаскивание метки
					drag_shift_x=axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(x)) - data->GetX(index);
					drag_shift_y=axes->AreaToParamY(axes->GetArea()->ScreenToAreaY(y)) - data->GetY(index);
					drag=true;
					result = true;
				}				
				break;
			}
			case EVENT_MOUSE_LEFT_UP:			
			{				
				drag=false;
				result = true;
				break;
			}			
			default:
				break;
		}
	}
	return result;
}


//--------------------------------- Методы класса VCurveMarksGraph -------------------------------


/** Конструктор*/
VCurveMarksGraph::VCurveMarksGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  
	const VColor& in_fill_color, int in_clip)
{	
	if (in_data && in_axes)
	{
		data=in_data;	
		axes=in_axes;
		color=in_color;
		fill_color=in_fill_color;		
		clip=in_clip;		
	}
	else
	{
		throw NULL_POINTER;
	}
};

/** Рисование*/
void VCurveMarksGraph::Paint(cairo_t * cr)
{			
	//Определяем диапазон индексов кадров, которые будут отбражены		
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;	
	
	//Добавляем элемент для каждой точки массива данных
	if (data->GetCount()!=points.size())
	{
		points.clear();
		points.reserve(data->GetCount());
		points.resize(data->GetCount());
		for (size_t i=0; i<data->GetCount(); ++i)
		{				
			points.at(i)=VCurveMarkElementPtr(new VCurveMarkElement(data, axes, i, color, fill_color, clip, false, POINT_STYLE_FLAG));
		}
	}		

	//Линии
	axes->GetArea()->Apply(cr, CLIP_AREA);
	cairo_set_antialias(cr,antialias);	
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{		
		double x=axes->ParamToAreaX(data->GetX(i));
		double y=axes->ParamToAreaY(data->GetY(i));
		if (hi_word(data->GetFlags(i))==FIRST_MARK || hi_word(data->GetFlags(i))==ACTIVE_FIRST_MARK)
		{
			cairo_move_to(cr, x, y);			
		}		
		else if (hi_word(data->GetFlags(i))==LAST_MARK || hi_word(data->GetFlags(i))==ACTIVE_LAST_MARK)
		{
			cairo_line_to(cr, x, y);

			CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));
			CairoSetColor(cr, VColor(0,255,0, 50));
			cairo_fill_preserve(cr);
			CairoSetColor(cr, VColor(0,0,0));
			cairo_stroke(cr);
		}
		else
		{
			cairo_line_to(cr, x, y);
		}					
	}			
	axes->GetArea()->Restore(cr);


	//Точки
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{
		points.at(i)->Render(cr);
	}
}

/** Обработчик сообщений*/
bool VCurveMarksGraph::ProcessEvent(const VEvent& in_event)
{
	bool result=false;
	if (GetActive())
	{
		switch (in_event.GetCode())
		{
			case EVENT_MOUSE_MOTION:			
			{
				data->DeselectPoint();
				for (point_itr i=points.begin(); i!=points.end(); ++i)
				{
					bool changed=(*i)->ProcessEvent(in_event);
					result = result || changed;
				}
				break;		
			}
			case EVENT_MOUSE_LEFT_DOWN:	
			{
				for (point_itr i=points.begin(); i!=points.end(); ++i)
				{
					bool changed=(*i)->ProcessEvent(in_event);
					result = result || changed;
				}
				if (!result)
				{
					double x=((VMouseEvent&)in_event).GetX();
					double y=((VMouseEvent&)in_event).GetY();
					double _x=axes->GetArea()->ScreenToAreaX(x);
					double _y=axes->GetArea()->ScreenToAreaY(y);
					//Если точка в границах области
					if (_x>0 && _x<axes->GetArea()->GetWidth() && _y>0 && _y<axes->GetArea()->GetHeight())
					{
						data->AddXY(0, axes->AreaToParamX(axes->GetArea()->ScreenToAreaX(x)), 
							axes->AreaToParamY(axes->GetArea()->ScreenToAreaY(y)));
						result = true;
					}
				}
				break;		
			}
			case EVENT_MOUSE_LEFT_UP:
			{
				for (point_itr i=points.begin(); i!=points.end(); ++i)
				{
					bool changed=(*i)->ProcessEvent(in_event);
					result = result || changed;
				}
				break;
			}
		}
	}
	return result;
}

//----------------------------------------- Методы класса VAxesLegendYElement --------------------------------

/** Конструктор*/
VAxesLegendYElement::VAxesLegendYElement(VAxesPtr in_axes)
{
	if (in_axes)
	{
		axes=in_axes;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
 void VAxesLegendYElement::Paint(cairo_t * cr)
{	
	cairo_save(cr);	
	CairoSetColor(cr, COLOR_HTML_BLACK);
	CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));			
	std::wstring text=axes->GetLegendY();
	cairo_text_extents_t te;
	cairo_text_extents(cr, text, &te);
	cairo_move_to(cr, axes->DrawingToDeviceX(text_space)+te.height, 
		axes->GetArea()->GetTop()+axes->GetArea()->GetHeight()/2+te.width/2);
	cairo_rotate(cr, -M_PI/2);
	cairo_show_text(cr, text);
	cairo_restore(cr);
}

//-------------------------------------------- Методы класса VTextElement -----------------------------------

/** Конструктор*/
VTextElement::VTextElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VTextProviderPtr in_data)
{
	if (in_area && in_device && in_data)
	{
		area=in_area;
		device=in_device;
		data=in_data;		
	}
	else
		throw NULL_POINTER;
}

/** Риcование*/	
void VTextElement::Paint(cairo_t * cr)
{
	if (data->GetCount()<1)
		return;
	area->Apply(cr, CLIP_AREA);		
	CairoSetColor(cr, COLOR_HTML_BLACK);
	CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, device->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));				
	cairo_text_extents_t te;
	if (data->GetCount()>0)
		cairo_text_extents(cr, data->GetItem(0), &te);
	cairo_move_to(cr, area->GetWidth()-te.width-device->DrawingToDeviceX(text_space), 
		area->GetHeight()-device->DrawingToDeviceY(text_space));	
	if (data->GetCount()>0)
		cairo_show_text(cr, data->GetItem(0));
	area->Restore(cr);
}

//------------------------------ Методы класса VKVLPSummaryElement --------------------------------

/** Конструктор*/
VKVLPSummaryElement::VKVLPSummaryElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VTextProviderPtr in_data)
{
	if (in_area && in_device && in_data)
	{
		area=in_area;
		device=in_device;
		data=in_data;		
	}
	else
		throw NULL_POINTER;
}

/** Риcование*/	
void VKVLPSummaryElement::Paint(cairo_t * cr)
{
	if (data->GetCount()<2)
		return;
	area->Apply(cr, CLIP_AREA);				
	CairoSetColor(cr, COLOR_HTML_BLACK);
	CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, device->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));				
	cairo_text_extents_t te;	
	//Первая строка	
	if (data->GetCount()>0)
	{
		cairo_save(cr);
		cairo_text_extents(cr, data->GetItem(0), &te);
		cairo_move_to(cr, area->GetWidth()-2*device->DrawingToDeviceX(text_space)-te.height, 
			te.width+2*device->DrawingToDeviceY(text_space));
		cairo_rotate(cr, -M_PI/2);	
		cairo_show_text(cr, data->GetItem(0));
		cairo_restore(cr);
	}
	//Втроая строка		
	if (data->GetCount()>1)
	{
		cairo_save(cr);
		cairo_text_extents(cr, data->GetItem(1), &te);
		cairo_move_to(cr, area->GetWidth()-device->DrawingToDeviceX(text_space), 
			te.width+2*device->DrawingToDeviceY(text_space));
		cairo_rotate(cr, -M_PI/2);
		if (data->GetCount()>1)
			cairo_show_text(cr, data->GetItem(1));
		cairo_restore(cr);
	}
	area->Restore(cr);
}


//------------------------------ Методы класса VKVLPPeregonElement --------------------------------

/** Конструктор*/
VKVLPPeregonElement::VKVLPPeregonElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VTextProviderPtr in_data)
{
	if (in_area && in_device && in_data)
	{
		area=in_area;
		device=in_device;
		data=in_data;		
	}
	else
		throw NULL_POINTER;
}

/** Риcование*/	
void VKVLPPeregonElement::Paint(cairo_t * cr)
{		
	if (data->GetCount()<1)
		return;
	cairo_save(cr);
	cairo_set_antialias(cr,antialias);
	CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, device->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
	
	double area_space=0.6;

	cairo_text_extents_t te;
	cairo_text_extents(cr, data->GetItem(0), &te);

	double x=area->GetLeft()+area->GetWidth()+device->DrawingToDeviceX(text_space+area_space);
	double y=area->GetTop()+std::min<double>(area->GetHeight()-device->DrawingToDeviceY(text_space), te.width+device->DrawingToDeviceY(text_space));

	//Координата		
	cairo_move_to(cr, x, y);			
	cairo_save(cr);
	cairo_rotate(cr, -M_PI/2);			
	cairo_show_text(cr, data->GetItem(0));
	cairo_restore(cr);

	area->Restore(cr);
}


//-------------------------- Методы класса VKVLPEventElement -----------------------------------------------------

/** Конструктор*/
VKVLPEventElement::VKVLPEventElement(VRTEventInfoDataPtr in_data, VAxesPtr in_axes)
{
	if (in_data && in_axes)
	{
		data=in_data;
		axes=in_axes;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** Риcование*/	
void VKVLPEventElement::Paint(cairo_t * cr)
{
	//Определяем диапазон индексов кадров, которые будут отбражены				
	std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
	if (visible.size()<2)
		return;
	axes->GetArea()->Apply(cr, CLIP_NONE);
	cairo_set_antialias(cr,antialias);
	//Параметр										
	double x;
	double prev_x=axes->GetInverseX() ? axes->GetArea()->GetWidth() : 0;
	for (size_t i=visible.front(); i<=visible.back(); ++i)
	{				
		x = axes->GetInverseX() ?  std::min<double>(axes->ParamToAreaX(data->GetX(i)), prev_x) : std::max<double>(axes->ParamToAreaX(data->GetX(i)), prev_x);		
		if (x>=0 && x<= axes->GetArea()->GetWidth()) 
		{
			data->SetDrawn(i);

			CairoSetColor(cr, VColor(0,0,0,255));			
			
			prev_x = axes->GetInverseX() ? 
				x-axes->DrawingToDeviceX(BASE_FONT_SIZE().Value())-axes->DrawingToDeviceX(text_space) :
				x+axes->DrawingToDeviceX(BASE_FONT_SIZE().Value())+axes->DrawingToDeviceX(text_space) ;
			

			if (data->IsDrawFrame(i))
			{
				//Рамка
				cairo_rectangle(cr, 
					x-axes->DrawingToDeviceX(BASE_FONT_SIZE().Value())/2, 0, 
					axes->DrawingToDeviceX(BASE_FONT_SIZE().Value())+axes->DrawingToDeviceX(text_space	), axes->GetArea()->GetHeight());
				cairo_stroke(cr);

				//Направление действия
				double arrow_size=axes->DrawingToDeviceX(BASE_FONT_SIZE().Value())/2;
				if (data->GetFlags(i)==END_EVENT)
				{

					cairo_move_to(cr, x-arrow_size/2, axes->DrawingToDeviceY(text_space));
					cairo_line_to(cr, x+arrow_size/2, arrow_size/2+axes->DrawingToDeviceX(text_space));
					cairo_line_to(cr, x-arrow_size/2, arrow_size+axes->DrawingToDeviceX(text_space));
					cairo_close_path(cr);
					cairo_fill(cr);
				}
				else if (data->GetFlags(i)==START_EVENT)
				{
					cairo_move_to(cr, x+arrow_size/2, axes->DrawingToDeviceY(text_space));
					cairo_line_to(cr, x-arrow_size/2, arrow_size/2+axes->DrawingToDeviceX(text_space));
					cairo_line_to(cr, x+arrow_size/2, arrow_size+axes->DrawingToDeviceX(text_space));
					cairo_close_path(cr);
					cairo_fill(cr);
				}
			}

			double y=axes->GetArea()->GetHeight()-axes->DrawingToDeviceY(text_space);

			if (data->IsBoldFont(i))
				CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceX(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD));
			else
				CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceX(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
			
			
			cairo_text_extents_t te;

			//Координата			
			if (data->GetFlags(i)==START_EVENT || data->GetFlags(i)==END_EVENT || data->GetFlags(i)==SING_EVENT)
			{
				cairo_text_extents(cr, data->GetCoord(i), &te);
				cairo_move_to(cr, x+te.height/2, y);			
				cairo_save(cr);
				cairo_rotate(cr, -M_PI/2);			
				cairo_show_text(cr, data->GetCoord(i));
				cairo_restore(cr);
			}

			//Тип
			cairo_text_extents(cr, data->GetType(i), &te);
			cairo_move_to(cr, x+te.height/2, y-axes->DrawingToDeviceY(KVLP_EVENT_COL_SIZES[0]));
			cairo_save(cr);
			cairo_rotate(cr, -M_PI/2);			
			cairo_show_text(cr, data->GetType(i));
			cairo_restore(cr);

			//Степень
			cairo_text_extents(cr, data->GetDegree(i), &te);
			cairo_move_to(cr, x+te.height/2, y-axes->DrawingToDeviceY(KVLP_EVENT_COL_SIZES[1]));
			cairo_save(cr);
			cairo_rotate(cr, -M_PI/2);			
			cairo_show_text(cr, data->GetDegree(i));
			cairo_restore(cr);
			
			//Значение
			cairo_text_extents(cr, data->GetValue(i), &te);
			cairo_move_to(cr, x+te.height/2, y-axes->DrawingToDeviceY(KVLP_EVENT_COL_SIZES[2]));
			cairo_save(cr);
			cairo_rotate(cr, -M_PI/2);			
			cairo_show_text(cr, data->GetValue(i));
			cairo_restore(cr);

			//Длина
			cairo_text_extents(cr, data->GetLength(i), &te);
			cairo_move_to(cr, x+te.height/2, y-axes->DrawingToDeviceY(KVLP_EVENT_COL_SIZES[3]));
			cairo_save(cr);
			cairo_rotate(cr, -M_PI/2);			
			cairo_show_text(cr, data->GetLength(i));
			cairo_restore(cr);

			//Количество/балл
			cairo_text_extents(cr, data->GetAmount(i), &te);
			cairo_move_to(cr, x+te.height/2, y-axes->DrawingToDeviceY(KVLP_EVENT_COL_SIZES[4]));
			cairo_save(cr);
			cairo_rotate(cr, -M_PI/2);
			cairo_show_text(cr, data->GetAmount(i));
			cairo_restore(cr);

			//Ограничение
			cairo_text_extents(cr, data->GetRestriction(i), &te);
			cairo_move_to(cr, x+te.height/2, y-axes->DrawingToDeviceY(KVLP_EVENT_COL_SIZES[5]));
			cairo_save(cr);
			cairo_rotate(cr, -M_PI/2);			
			cairo_show_text(cr, data->GetRestriction(i));
			cairo_restore(cr);
		}
	}
	//Восстанавливаем состояние	
	axes->GetArea()->Restore(cr);
}


//--------------------------------------- Методы класса VKVLPEventAreaName --------------------------------------

/** Конструктор*/
VKVLPEventAreaName::VKVLPEventAreaName(VAreaPtr in_area, VDrawingDevicePtr in_device, double in_width)
{
	if (in_area && in_device)
	{
		area=in_area;
		device=in_device;
		width=in_width;
	}
	else
		throw NULL_POINTER;
}

/** Риcование*/	
void VKVLPEventAreaName::Paint(cairo_t * cr)
{
	if (area)
	{
		cairo_save(cr);
		cairo_set_antialias(cr,antialias);
		CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, device->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
		
		CairoSetLineWidth(cr, device->DrawingToDeviceX(width/10.0));
		double area_space=0.8;
		double frame_space=0.05;
		double frame_size=0.4;

		cairo_rectangle(cr, area->GetLeft()+area->GetWidth()+device->DrawingToDeviceX(area_space), 
			area->GetTop()+device->DrawingToDeviceY(frame_space), 
			device->DrawingToDeviceX(frame_size), area->GetHeight()-2*device->DrawingToDeviceY(frame_space));
		cairo_stroke(cr);				

		double x=area->GetLeft()+area->GetWidth()+device->DrawingToDeviceX(area_space+frame_size-text_space);
		double y=area->GetTop() + area->GetHeight() - device->DrawingToDeviceY(text_space);

		//Координата		
		cairo_move_to(cr, x, y);			
		cairo_save(cr);
		cairo_rotate(cr, -M_PI/2);			
		cairo_show_text(cr, L"м");
		cairo_restore(cr);

		//Тип		
		cairo_move_to(cr, x, y-device->DrawingToDeviceY(KVLP_EVENT_COL_SIZES[0]));
		cairo_save(cr);
		cairo_rotate(cr, -M_PI/2);			
		cairo_show_text(cr, L"Отст");
		cairo_restore(cr);

		//Степень		
		cairo_move_to(cr, x, y-device->DrawingToDeviceY(KVLP_EVENT_COL_SIZES[1]));
		cairo_save(cr);
		cairo_rotate(cr, -M_PI/2);			
		cairo_show_text(cr, L"Ст");
		cairo_restore(cr);

		//Значение		
		cairo_move_to(cr, x, y-device->DrawingToDeviceY(KVLP_EVENT_COL_SIZES[2]));
		cairo_save(cr);
		cairo_rotate(cr, -M_PI/2);			
		cairo_show_text(cr, L"Откл");
		cairo_restore(cr);

		//Длина		
		cairo_move_to(cr, x, y-device->DrawingToDeviceY(KVLP_EVENT_COL_SIZES[3]));
		cairo_save(cr);
		cairo_rotate(cr, -M_PI/2);			
		cairo_show_text(cr, L"Дл.");
		cairo_restore(cr);

		//Количество		
		cairo_move_to(cr, x, y-device->DrawingToDeviceY(KVLP_EVENT_COL_SIZES[4]));
		cairo_save(cr);
		cairo_rotate(cr, -M_PI/2);			
		DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515) ? cairo_show_text(cr, L"Кол") : cairo_show_text(cr, L"Балл");
		cairo_restore(cr);

		//Ограничение		
		cairo_move_to(cr, x, y-device->DrawingToDeviceY(KVLP_EVENT_COL_SIZES[5]));
		cairo_save(cr);
		cairo_rotate(cr, -M_PI/2);			
		cairo_show_text(cr, L"Огр.ск.");
		cairo_restore(cr);
	}
}


//--------------------------------- Методы класса VKVLPCoordinatesElement --------------------------------

/** Конструктор*/
VKVLPCoordinatesElement::VKVLPCoordinatesElement(VDataProviderPtr in_data, VAxesPtr in_axes, bool in_show_text)
{
	if (in_axes && in_data)
	{
		axes=in_axes;	
		data=in_data;
		show_text=in_show_text;
	}
	else
	{
		throw NULL_POINTER;
	}	
}

/** Рисование*/
void VKVLPCoordinatesElement::Paint(cairo_t * cr)
{
	if (axes)
	{
		//Минимальное расстояние между линиями координатной сетки (см)
		const double min_grid_space=0.5;
		double space1=axes->DrawingToDeviceY(2.5/10.0);
		double space2=axes->DrawingToDeviceY(1.25/10.0);
		
		axes->GetArea()->Apply(cr, CLIP_AREA);
		cairo_set_antialias(cr,antialias);
		CairoSetLineWidth(cr, axes->DrawingToDeviceX(default_line_width/10.0));

		std::vector<size_t> visible=data->GetVisible(axes->GetMinX(), axes->GetMaxY(), axes->GetMaxX(), axes->GetMinY());
		if (visible.size()<2)
			return;

		for (size_t i=visible.front(); i<visible.back(); ++i)
		{
			double x=axes->ParamToAreaX(data->GetX(i));			
			switch (data->GetFlags(i))
			{	
				case COORDINATE_KM_5:
				case COORDINATE_KM:
				{
					if (axes->ParamToDrawingX(1000)>2*min_grid_space)//Если между линиями больше двух сантиметров
					{												
						CairoSetColor(cr, COLOR_HTML_BLACK);
						cairo_move_to(cr, x, axes->GetArea()->GetHeight());
						cairo_line_to(cr, x, axes->GetArea()->GetHeight()-axes->DrawingToDeviceX(0.4));
						cairo_stroke(cr);												
						
						CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()+font_size_step), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD));
						cairo_text_extents_t te;
						cairo_text_extents(cr, data->GetLabel(i), &te);
						cairo_move_to(cr, axes->GetInverseX() ? x-te.width-axes->DrawingToDeviceX(text_space) :
							x+axes->DrawingToDeviceX(text_space), axes->GetArea()->GetHeight()-axes->DrawingToDeviceY(text_space));
						cairo_show_text(cr, data->GetLabel(i));
						
						break;
					}
				}
				case COORDINATE_PK:
				{					
					if (axes->ParamToDrawingX(100)>min_grid_space && data->GetLabel(i).compare(L"1")!=0)//Если между линиями больше одного сантиметра
					{							
						CairoSetColor(cr, COLOR_HTML_BLACK);
						cairo_move_to(cr, x, axes->GetArea()->GetHeight());
						cairo_line_to(cr, x, axes->GetArea()->GetHeight()-axes->DrawingToDeviceX(0.2));
						cairo_stroke(cr);												
						
						CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
						cairo_text_extents_t te;
						cairo_text_extents(cr, data->GetLabel(i), &te);
						cairo_move_to(cr, axes->GetInverseX() ? x-te.width-axes->DrawingToDeviceX(text_space) :
							x+axes->DrawingToDeviceX(text_space), axes->GetArea()->GetHeight()-axes->DrawingToDeviceY(text_space));
						cairo_show_text(cr, data->GetLabel(i));
						break;
					}
				}				
			}
		}
	}		
}