#include "cairo_ex.h"
#include "glyphs.h"

//-------------------- Значки объектов в соответствии с ЦПТ-54/26 -------------------------------

/// Подпись к объекту
void show_object_text(cairo_t * cr, VAxesPtr in_axes, const std::wstring& in_label, bool in_show_name, bool in_show_coord)
{
	std::vector<std::wstring> labels;
	boost::algorithm::split(labels, in_label, boost::is_any_of(L";"));
	
	CairoSetColor(cr, COLOR_HTML_BLACK);
	CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, in_axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
	cairo_text_extents_t te;
	
	//Название объекта
	if (in_show_name)
	{
		cairo_save(cr);
		cairo_text_extents(cr, labels.front(), &te);	
		cairo_move_to(cr, -in_axes->DrawingToDeviceX(text_space), -in_axes->DrawingToDeviceY(text_space));
		cairo_rotate(cr, -M_PI/2);		
		cairo_show_text(cr, labels.front());
		cairo_restore(cr);		
	}
	//Пикетаж объекта
	if (in_show_coord)
	{
		cairo_save(cr);
		cairo_text_extents(cr, labels.back(), &te);
		cairo_move_to(cr, te.height+in_axes->DrawingToDeviceX(text_space), -in_axes->DrawingToDeviceY(text_space));
		cairo_rotate(cr, -M_PI/2);		
		cairo_show_text(cr, labels.back());
		cairo_restore(cr);
	}
}


/// Километровый столб
extents_t draw_km(cairo_t * cr, VAxesPtr in_axes, side_t in_side)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(20.0/10.0);
	double d=in_axes->DrawingToDeviceY(5.0/10.0);
	double c=h-d/2;

	extents_t result;

	cairo_move_to(cr, 0, h);
	cairo_arc(cr, 0, c, d/2, static_cast<int>(in_side)*(-M_PI/2), static_cast<int>(in_side)*M_PI/2);
	cairo_fill(cr);		
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, h);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, c, d/2, 0, 2*M_PI);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);	
	cairo_stroke(cr);
	return result;
}

///По умолчанию
extents_t draw_default(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(23.0/10.0);	
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);	
	cairo_stroke_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);	
	cairo_stroke(cr);
	return result;
}

///По умолчанию
extents_t draw_default_bottom(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(23.0/10.0);
	double w=in_axes->DrawingToDeviceX(3.0/10.0);
	extents_t result;
	cairo_move_to(cr, 0, 0);	
	cairo_line_to(cr, 0, h);	
	cairo_stroke_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);	
	cairo_stroke(cr);
	return result;
}

///1. Станция (внеклассная)
extents_t draw_station_0(cairo_t * cr, VAxesPtr in_axes, side_t in_side)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(42.0/10.0);
	double d1=in_axes->DrawingToDeviceY(12.0/10.0);	
	double d2=in_axes->DrawingToDeviceY(10.0/10.0);	

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+d1/2, d1/2, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+d1/2, d2/2, 0, 2*M_PI);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);	
	cairo_stroke(cr);

	cairo_move_to(cr, 0, h-d1/2+d2/2);
	cairo_line_to(cr, 0, h-d1/2-d2/2);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+d1/2, d2/2, static_cast<int>(in_side)*(-M_PI/2), static_cast<int>(in_side)*M_PI/2);
	cairo_fill(cr);

	return result;
}

///2. Станция (первый класс)
extents_t draw_station_1(cairo_t * cr, VAxesPtr in_axes, side_t in_side)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(40.0/10.0);
	double d1=in_axes->DrawingToDeviceY(10.0/10.0);	
	double d2=in_axes->DrawingToDeviceY(8.0/10.0);	
	double c=-h+d1/2;

	cairo_move_to(cr, 0, -h);
	cairo_arc(cr, 0, c, d1/2, static_cast<int>(in_side)*(-M_PI/2), static_cast<int>(in_side)*M_PI/2);
	cairo_line_to(cr, 0, c+d2/2);	
	cairo_arc_negative(cr, 0, c, d2/2, static_cast<int>(in_side)*(M_PI/2), static_cast<int>(in_side)*(-M_PI/2));
	cairo_line_to(cr, 0, -h);	
	cairo_fill(cr);

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+d1/2, d1/2, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+d1/2, d2/2, 0, 2*M_PI);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);	
	cairo_stroke(cr);
	return result;
}

///3. Станция (второй класс)
extents_t draw_station_2(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(40.0/10.0);
	double d=in_axes->DrawingToDeviceY(10.0/10.0);	

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+d/2, d/2, 0, 2*M_PI);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);	
	cairo_stroke(cr);

	return result;
}

///4. Станция (третий класс)
extents_t draw_station_3(cairo_t * cr, VAxesPtr in_axes, side_t in_side)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(40.0/10.0);
	double d=in_axes->DrawingToDeviceY(10.0/10.0);
	double c=-h+d/2;

	cairo_move_to(cr, 0, -h);
	cairo_arc(cr, 0, c, d/2, static_cast<int>(in_side)*(-M_PI/2), static_cast<int>(in_side)*M_PI/2);
	cairo_fill(cr);
	
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+d/2, d/2, 0, 2*M_PI);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);	
	cairo_stroke(cr);

	return result;
}

///5. Станция (четвертый класс)
extents_t draw_station_4(cairo_t * cr, VAxesPtr in_axes, side_t in_side)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(40.0/10.0);
	double d=in_axes->DrawingToDeviceY(10.0/10.0);
	double c=-h+d/2;	
	
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_arc(cr, 0, c, d/2, static_cast<int>(in_side)*(-M_PI/2), static_cast<int>(in_side)*M_PI/2);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);	
	cairo_stroke(cr);

	return result;
}

///6. Станция (пятый класс)
extents_t draw_station_5(cairo_t * cr, VAxesPtr in_axes, side_t in_side)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(40.0/10.0);
	double d=in_axes->DrawingToDeviceY(10.0/10.0);
	double c=-h+d/2;	
	
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_arc(cr, 0, c, d/2, static_cast<int>(in_side)*(-M_PI/2), static_cast<int>(in_side)*M_PI/2);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);	
	cairo_fill_preserve(cr);
	cairo_stroke(cr);

	return result;
}


///7. Станция (разъезд)
extents_t draw_station_6(cairo_t * cr, VAxesPtr in_axes, side_t in_side)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(40.0/10.0);
	double d1=in_axes->DrawingToDeviceY(10.0/10.0);	
	double d2=in_axes->DrawingToDeviceY(8.0/10.0);	
	double c=-h;
		
	if (in_side==SD_RIGHT)
	{
		cairo_move_to(cr, d2/2, -h);
		cairo_line_to(cr, d1/2, -h);
		cairo_arc(cr, 0, c, d1/2, 0, M_PI/2);
		cairo_line_to(cr, 0, c+d2/2);
		cairo_arc_negative(cr, 0, c, d2/2, M_PI/2, 0);	
		cairo_fill(cr);
	}
	else
	{
		cairo_move_to(cr, -d2/2, -h);
		cairo_line_to(cr, -d1/2, -h);
		cairo_arc_negative(cr, 0, c, d1/2, M_PI, M_PI/2);
		cairo_line_to(cr, 0, c+d2/2);
		cairo_arc(cr, 0, c, d2/2, M_PI/2, M_PI);	
		cairo_fill(cr);
	}
		
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_line_to(cr, d1/2, -h);	
	cairo_arc(cr, 0, c, d1/2, 0, M_PI);
	cairo_line_to(cr, 0, -h);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///8. Остановочный пункт
extents_t draw_station_7(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(34.0/10.0);
	double a=in_axes->DrawingToDeviceY(10.0/10.0);	
	double b=in_axes->DrawingToDeviceY(4.0/10.0);	
		
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_rectangle(cr, 0-a/2, -h, a, b);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///9. Стрелочный перевод симметричный (четн.)
extents_t draw_switch_1(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(25.0/10.0);
	double a=in_axes->DrawingToDeviceY(10.0/10.0);	
	double b=in_axes->DrawingToDeviceY(3.0/10.0);		
		
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	
	cairo_move_to(cr, a/2, -h+b/2);
	cairo_line_to(cr, -a/2, -h);
	cairo_move_to(cr, a/2, -h+b/2);
	cairo_line_to(cr, -a/2, -h+b);

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///10. Стрелочный перевод симметричный (нечетн.)
extents_t draw_switch_2(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(25.0/10.0);
	double a=in_axes->DrawingToDeviceY(10.0/10.0);	
	double b=in_axes->DrawingToDeviceY(3.0/10.0);		
		
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	
	cairo_move_to(cr, -a/2, -h+b/2);
	cairo_line_to(cr, a/2, -h);
	cairo_move_to(cr, -a/2, -h+b/2);
	cairo_line_to(cr, a/2, -h+b);

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///11. Стрелочный перевод (лев./четн.)
extents_t draw_switch_3(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(25.0/10.0);
	double a=in_axes->DrawingToDeviceY(10.0/10.0);	
	double b=in_axes->DrawingToDeviceY(3.0/10.0);		
		
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	
	cairo_move_to(cr, a/2, -h+b/2);
	cairo_line_to(cr, -a/2, -h+b/2);
	cairo_move_to(cr, a/2, -h+b/2);
	cairo_line_to(cr, -a/2, -h+b);

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///12. Стрелочный перевод (лев/нечетн.)
extents_t draw_switch_4(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(25.0/10.0);
	double a=in_axes->DrawingToDeviceY(10.0/10.0);	
	double b=in_axes->DrawingToDeviceY(3.0/10.0);		
		
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	
	cairo_move_to(cr, -a/2, -h+b/2);
	cairo_line_to(cr, a/2, -h);
	cairo_move_to(cr, -a/2, -h+b/2);
	cairo_line_to(cr, a/2, -h+b/2);

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///13. Стрелочный перевод (прав./четн.)
extents_t draw_switch_5(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(25.0/10.0);
	double a=in_axes->DrawingToDeviceY(10.0/10.0);	
	double b=in_axes->DrawingToDeviceY(3.0/10.0);		
		
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	
	cairo_move_to(cr, a/2, -h+b/2);
	cairo_line_to(cr, -a/2, -h);
	cairo_move_to(cr, a/2, -h+b/2);
	cairo_line_to(cr, -a/2, -h+b/2);

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///14. Стрелочный перевод (прав./нечетн.)
extents_t draw_switch_6(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(25.0/10.0);
	double a=in_axes->DrawingToDeviceY(10.0/10.0);	
	double b=in_axes->DrawingToDeviceY(3.0/10.0);		
		
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	
	cairo_move_to(cr, -a/2, -h+b/2);
	cairo_line_to(cr, a/2, -h+b/2);
	cairo_move_to(cr, -a/2, -h+b/2);
	cairo_line_to(cr, a/2, -h+b);

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///15. Стрелочный перевод перекрестный
extents_t draw_switch_7(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(22.5/10.0);
	double a=in_axes->DrawingToDeviceY(10.0/10.0);	
	double b=in_axes->DrawingToDeviceY(3.0/10.0);		
		
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	
	cairo_move_to(cr, -a/2, -h);
	cairo_line_to(cr, a/2, -h+b);
	cairo_move_to(cr, -a/2, -h+b);
	cairo_line_to(cr, a/2, -h);

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}


///16. Сбрасыватель башмака
extents_t draw_switch_8(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(22.5/10.0);
	double a=in_axes->DrawingToDeviceY(10.0/10.0);	
	double b=in_axes->DrawingToDeviceY(5.0/10.0);		

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h+b);
	
	//FixMe!!! В ЦПТ-54/26 возможна неточность. Непонятно как изображать

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///Коэффициент масштабирования сигналов
static const double sign_scale=1.0;

///17. Светофор однолинзовый
extents_t draw_sign_1(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double h=in_axes->DrawingToDeviceY(30.0/10.0);
	double x=in_axes->DrawingToDeviceY(sign_scale*2.0/10.0);
	double y=in_axes->DrawingToDeviceY(sign_scale*3.0/10.0);	
	double d=in_axes->DrawingToDeviceY(sign_scale*0.5/10.0);
	//double d=sign_scale*1.5;
	
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h+y);
	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y-x/2);
	cairo_arc(cr, 0, -h+y-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y/2, d, 0, 2*M_PI);
	

	extents_t result;
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///18. Светофор двухлинзовый
extents_t draw_sign_2(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	//размеры увеличены в два раза
	double h=in_axes->DrawingToDeviceY(30.0/10.0);
	double x=in_axes->DrawingToDeviceY(sign_scale*2.0/10.0);
	double y=in_axes->DrawingToDeviceY(sign_scale*5.0/10.0);
	double d=in_axes->DrawingToDeviceY(sign_scale*0.5/10.0);//sign_scale*1.5;
	
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h+y);
	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y-x/2);
	cairo_arc(cr, 0, -h+y-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y/3, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+2*y/3, d, 0, 2*M_PI);
	

	extents_t result;
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///19. Светофор трехлинзовый
extents_t draw_sign_3(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	//размеры увеличены в два раза
	double h=in_axes->DrawingToDeviceY(30.0/10.0);
	double x=in_axes->DrawingToDeviceY(sign_scale*2.0/10.0);
	double y=in_axes->DrawingToDeviceY(sign_scale*5.0/10.0);
	double d=in_axes->DrawingToDeviceY(sign_scale*0.5/10.0);
	//double d=sign_scale*1.5;
	
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h+y);
	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y-x/2);
	cairo_arc(cr, 0, -h+y-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y/4, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+2*y/4, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+3*y/4, d, 0, 2*M_PI);
	

	extents_t result;
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///20. Светофор четырехлинзовый
extents_t draw_sign_4(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	//размеры увеличены в два раза
	double h=in_axes->DrawingToDeviceY(30.0/10.0);
	double x=in_axes->DrawingToDeviceY(sign_scale*2.0/10.0);
	double y=in_axes->DrawingToDeviceY(sign_scale*7.0/10.0);
	double d=in_axes->DrawingToDeviceY(sign_scale*0.5/10.0);
	//double d=sign_scale*1.5;
	
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h+y);
	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y-x/2);
	cairo_arc(cr, 0, -h+y-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y/5, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+2*y/5, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+3*y/5, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+4*y/5, d, 0, 2*M_PI);
	

	extents_t result;
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///21. Светофор пятилинзовый
extents_t draw_sign_5(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	//размеры увеличены в два раза
	double h=in_axes->DrawingToDeviceY(30.0/10.0);
	double x=in_axes->DrawingToDeviceY(sign_scale*2.0/10.0);
	double y1=in_axes->DrawingToDeviceY(sign_scale*5.0/10.0);
	double y2=in_axes->DrawingToDeviceY(sign_scale*5.0/10.0);
	double y3=in_axes->DrawingToDeviceY(sign_scale*3.0/10.0);	
	double d=in_axes->DrawingToDeviceY(sign_scale*0.5/10.0);
	//double d=sign_scale*1.5;
	
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h+y1+y2+y3);

	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y1-x/2);
	cairo_arc(cr, 0, -h+y1-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y1/3, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+2*y1/3, d, 0, 2*M_PI);	

	h-=y1;

	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y2-x/2);
	cairo_arc(cr, 0, -h+y2-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y2/3, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+2*y2/3, d, 0, 2*M_PI);

	h-=y2;

	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y3-x/2);
	cairo_arc(cr, 0, -h+y3-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y3/2, d, 0, 2*M_PI);
	

	extents_t result;
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///22. Будка
extents_t draw_building_1(cairo_t * cr, VAxesPtr in_axes, side_t in_side)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(25.0/10.0);
	double d=in_axes->DrawingToDeviceY(5.0/10.0);
	double c=-h+d/2;

	cairo_move_to(cr, 0, -h);
	cairo_arc(cr, 0, c, d/2, static_cast<int>(in_side)*(-M_PI/2), static_cast<int>(in_side)*M_PI/2);
	cairo_fill(cr);
	
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+d/2, d/2, 0, 2*M_PI);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);	
	cairo_stroke(cr);

	return result;
}

///23. Блок-пост
extents_t draw_building_2(cairo_t * cr, VAxesPtr in_axes, side_t in_side)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(30.0/10.0);
	double x=in_axes->DrawingToDeviceY(5.0/10.0);
	double y=in_axes->DrawingToDeviceY(10.0/10.0);

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);

	cairo_move_to(cr, 0, -(h-x));
	cairo_line_to(cr, -y/2, -h);
	cairo_line_to(cr, y/2, -h);
	cairo_line_to(cr, 0, -(h-x));

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);	
	cairo_stroke(cr);

	cairo_move_to(cr, 0, -(h-x));
	cairo_line_to(cr, 0, -h);
	cairo_line_to(cr, static_cast<int>(in_side)*y/2, -h);
	cairo_line_to(cr, 0, -(h-x));
	cairo_fill(cr);

	return result;
}

///24. Жилой дом
extents_t draw_building_3(cairo_t * cr, VAxesPtr in_axes, side_t in_side)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(23.0/10.0);
	double y=in_axes->DrawingToDeviceY(3.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -(h-y));

	cairo_rectangle(cr, -x/2, -h, x, y);
	
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);	
	cairo_stroke(cr);

	cairo_move_to(cr, -x/2, -h);
	cairo_line_to(cr, x/2, -(h-y));
	(in_side==SD_RIGHT) ? cairo_line_to(cr, x/2, -h) : cairo_line_to(cr, -x/2, -(h-y));
	cairo_line_to(cr, -x/2, -h);	
	cairo_fill(cr);

	return result;
}

///25. Казарма
extents_t draw_building_4(cairo_t * cr, VAxesPtr in_axes, side_t in_side)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(23.0/10.0);
	double y=in_axes->DrawingToDeviceY(3.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -(h-y));

	cairo_rectangle(cr, -x/2, -h, x, y);

	cairo_move_to(cr, -x/2, -h);
	cairo_line_to(cr, x/2, -(h-y));

	cairo_move_to(cr, x/2, -h);
	cairo_line_to(cr, -x/2, -(h-y));
	
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);	
	cairo_stroke(cr);

	cairo_move_to(cr, 0, -(h-y/2));
	cairo_line_to(cr, static_cast<int>(in_side)*x/2, -(h-y));
	cairo_line_to(cr, static_cast<int>(in_side)*x/2, -h);
		
	cairo_fill(cr);

	return result;
}

///26. Изостык
extents_t draw_izostyk_1(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(23.0/10.0);
	double x=in_axes->DrawingToDeviceY(3.0/10.0);
	double y=in_axes->DrawingToDeviceY(1.0/10.0);

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_move_to(cr, -x/2, -h);
	cairo_line_to(cr, x/2, -h);

	cairo_move_to(cr, -x/2, -h+y/2);
	cairo_line_to(cr, -x/2, -h-y/2);

	cairo_move_to(cr, x/2, -h+y/2);
	cairo_line_to(cr, x/2, -h-y/2);

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///27. Негабаритный изостык
extents_t draw_izostyk_2(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(23.0/10.0);
	double x=in_axes->DrawingToDeviceY(3.0/10.0);
	double y=in_axes->DrawingToDeviceY(1.0/10.0);
	double d=in_axes->DrawingToDeviceY(4.0/10.0);

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_move_to(cr, -x/2, -h);
	cairo_line_to(cr, x/2, -h);

	cairo_move_to(cr, -x/2, -h+y/2);
	cairo_line_to(cr, -x/2, -h-y/2);

	cairo_move_to(cr, x/2, -h+y/2);
	cairo_line_to(cr, x/2, -h-y/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h, d/2, 0, 2*M_PI);

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

//28. Предельный столбик
extents_t draw_stolbik_1(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(23.0/10.0);	
	double d=in_axes->DrawingToDeviceY(1.0/10.0);

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);	

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h, d/2, 0, 2*M_PI);
	cairo_fill_preserve(cr);

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

//29. Отсутствующий предельный столбик
extents_t draw_stolbik_2(cairo_t * cr, VAxesPtr in_axes)
{	
	return draw_default_bottom(cr, in_axes);
}

///30. Конец пути без упора
extents_t draw_dead_end_1(cairo_t * cr, VAxesPtr in_axes)
{	
	return draw_default_bottom(cr, in_axes);
}

///31. Конец пути с упором
extents_t draw_dead_end_2(cairo_t * cr, VAxesPtr in_axes)
{
	return draw_default_bottom(cr, in_axes);
}

///32. Конец пути с упором и балластной призмой
extents_t draw_dead_end_3(cairo_t * cr, VAxesPtr in_axes)
{
	return draw_default_bottom(cr, in_axes);
}

///33. Мост деревянный
extents_t draw_bridge_1(cairo_t * cr, VAxesPtr in_axes, double in_length)
{
	//FixMe!!! в ЦПТ 54/26 никак не обозначается, здесь использовано произвольное отображение
	double h1=in_axes->DrawingToDeviceY(2.0/10.0);
	double h2=in_axes->DrawingToDeviceY(6.0/10.0);
	double h3=in_axes->DrawingToDeviceY(2.0/10.0);
	double h4=in_axes->DrawingToDeviceY(23.0/10.0);
	double w=in_axes->DrawingToDeviceX(3.0/10.0);
	double l=std::max<double>(w, round_to(in_axes->DrawingToDeviceX(in_axes->ParamToDrawingX(in_length)), w));

	extents_t result;
	
	cairo_rectangle(cr, -l/2, 0, l, h1);
	cairo_move_to(cr, -l/2, h1);
	cairo_line_to(cr, -l/2, h2);
	cairo_move_to(cr, l/2, h1);
	cairo_line_to(cr, l/2, h2);

	cairo_move_to(cr, 0, h1);
	cairo_line_to(cr, 0, h4);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);

	return result;
}

///34. Мост железобетонный
extents_t draw_bridge_2(cairo_t * cr, VAxesPtr in_axes, double in_length)
{
	//FixMe!!! в ЦПТ 54/26 никак не обозначается, здесь использовано отображение из имеющихся профилей
	double h1=in_axes->DrawingToDeviceY(2.0/10.0);
	double h2=in_axes->DrawingToDeviceY(6.0/10.0);
	double h3=in_axes->DrawingToDeviceY(2.0/10.0);
	double h4=in_axes->DrawingToDeviceY(23.0/10.0);
	double w=in_axes->DrawingToDeviceX(3.0/10.0);
	double w1=in_axes->DrawingToDeviceX(0.5/10.0);
	double l=std::max<double>(w, round_to(in_axes->DrawingToDeviceX(in_axes->ParamToDrawingX(in_length)), w));

	extents_t result;
	
	cairo_rectangle(cr, -l/2, 0, l, h1);
	cairo_fill_preserve(cr);

	cairo_move_to(cr, -l/2, h1);
	cairo_line_to(cr, -l/2, h2);

	cairo_rectangle(cr, -l/2-w1, h2-h3, w1, h3);
	cairo_fill_preserve(cr);

	cairo_rectangle(cr, l/2, h2-h3, w1, h3);
	cairo_fill_preserve(cr);

	cairo_move_to(cr, l/2, h1);
	cairo_line_to(cr, l/2, h2);

	cairo_move_to(cr, 0, h1);
	cairo_line_to(cr, 0, h4);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);

	return result;
}

///35. Мост металлический (фермовый) с ездой понизу
extents_t draw_bridge_3(cairo_t * cr, VAxesPtr in_axes, double in_length)
{
	double h=in_axes->DrawingToDeviceY(4.0/10.0);
	double w=in_axes->DrawingToDeviceX(3.0/10.0);
	double h2=in_axes->DrawingToDeviceY(23.0/10.0);
	double l=std::max<double>(w, round_to(in_axes->DrawingToDeviceX(in_axes->ParamToDrawingX(in_length)), w));
	
	extents_t result;
	
	cairo_rectangle(cr, -l/2, 0, l, h);
	cairo_move_to(cr, 0, h);
	cairo_line_to(cr, 0, h2);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_save(cr);	
	cairo_stroke_preserve(cr);
	cairo_clip(cr);
	for (size_t i=0; i<=size_t(l/w); ++i)
	{
		double z=-l/2+i*w;
		cairo_move_to(cr, z, 0);
		cairo_line_to(cr, z+w, h);		
		cairo_move_to(cr, z+w, 0);
		cairo_line_to(cr, z, h);		
	}	
	cairo_stroke(cr);
	cairo_restore(cr);
	return result;	
}

///36. Мост металлический (фермовый) с ездой поверху
extents_t draw_bridge_4(cairo_t * cr, VAxesPtr in_axes, double in_length)
{
	double h=in_axes->DrawingToDeviceY(6.0/10.0);
	double w=in_axes->DrawingToDeviceX(6.0/10.0);
	double h2=in_axes->DrawingToDeviceY(23.0/10.0);
	double l=std::max<double>(2*w, round_to(in_axes->DrawingToDeviceX(in_axes->ParamToDrawingX(in_length)), 2*w));
		
	extents_t result;
	
	cairo_move_to(cr, -l/2, 0);
	cairo_line_to(cr, -l/2+w/2, -h);
	cairo_line_to(cr, l/2-w/2, -h);
	cairo_line_to(cr, l/2, 0);
	cairo_line_to(cr, -l/2, 0);	
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, h2);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_save(cr);	
	cairo_stroke_preserve(cr);
	cairo_clip(cr);
	for (size_t i=0; i<=size_t(l/w); ++i)
	{
		double z=-l/2+i*w;
		cairo_move_to(cr, z, 0);
		cairo_line_to(cr, z+w/2, -h);
		cairo_line_to(cr, z+w, 0);

		cairo_move_to(cr, z, 0);
		cairo_line_to(cr, z, -h);
		cairo_move_to(cr, z+w/2, 0);
		cairo_line_to(cr, z+w/2, -h);
	}
	
	cairo_stroke(cr);
	cairo_restore(cr);
	return result;
}

///37. Начало/конец тормозного устройства*/
extents_t draw_brake(cairo_t * cr, VAxesPtr in_axes)
{	
	return draw_default_bottom(cr, in_axes);
}

///38. Переезд неохраняемый одинарный
extents_t draw_crossing_1(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double h=in_axes->DrawingToDeviceY(27.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);
	double y=in_axes->DrawingToDeviceY(2.0/10.0);	

	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);	
	cairo_line_to(cr, x, -h);
	cairo_line_to(cr, x-y/2, -h+y/2);
	cairo_line_to(cr, 0, -h+y/2);

	extents_t result;
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);

	cairo_move_to(cr, 0, -h+y);
	cairo_line_to(cr, x, -h+y);
	cairo_line_to(cr, x-y/2, -h+y/2);
	cairo_line_to(cr, 0, -h+y/2);
	cairo_fill(cr);

	return result;
}

///39. Переезд охраняемый одиночный
extents_t draw_crossing_2(cairo_t * cr, VAxesPtr in_axes, side_t in_side)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double h=in_axes->DrawingToDeviceY(27.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);
	double y=in_axes->DrawingToDeviceY(2.0/10.0);	
	double h2=in_axes->DrawingToDeviceY(4.0/10.0); 
	double d=in_axes->DrawingToDeviceY(5.0/10.0);

	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);	
	cairo_line_to(cr, x, -h);
	cairo_line_to(cr, x-y/2, -h+y/2);
	cairo_line_to(cr, 0, -h+y/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+d/2+h2, d/2, 0, 2*M_PI);

	extents_t result;
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);

	cairo_move_to(cr, 0, -h+y);
	cairo_line_to(cr, x, -h+y);
	cairo_line_to(cr, x-y/2, -h+y/2);
	cairo_line_to(cr, 0, -h+y/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+d/2+h2, d/2, static_cast<int>(in_side)*(-M_PI/2), static_cast<int>(in_side)*M_PI/2);

	cairo_fill(cr);

	return result;
}

///40. Переезд неохраняемый двойной
extents_t draw_crossing_3(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double h=in_axes->DrawingToDeviceY(27.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);
	double y=in_axes->DrawingToDeviceY(2.0/10.0);	

	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);	
	cairo_line_to(cr, x, -h);
	cairo_line_to(cr, x-y/2, -h+y/2);
	cairo_line_to(cr, 0, -h+y/2);	

	extents_t result;
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);

	cairo_move_to(cr, 0, -h+y);
	cairo_line_to(cr, x, -h+y);
	cairo_line_to(cr, x-y/2, -h+y/2);
	cairo_line_to(cr, 0, -h+y/2);
	cairo_fill(cr);

	h-=y;

	cairo_line_to(cr, 0, -h);	
	cairo_line_to(cr, x, -h);
	cairo_line_to(cr, x-y/2, -h+y/2);
	cairo_line_to(cr, 0, -h+y/2);	
	
	cairo_stroke(cr);

	cairo_move_to(cr, 0, -h+y);
	cairo_line_to(cr, x, -h+y);
	cairo_line_to(cr, x-y/2, -h+y/2);
	cairo_line_to(cr, 0, -h+y/2);
	cairo_fill(cr);

	return result;
}

///41. Переезд охраняемый двойной
extents_t draw_crossing_4(cairo_t * cr, VAxesPtr in_axes, side_t in_side)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double h=in_axes->DrawingToDeviceY(27.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);
	double y=in_axes->DrawingToDeviceY(2.0/10.0);	
	double h2=in_axes->DrawingToDeviceY(4.0/10.0); 
	double d=in_axes->DrawingToDeviceY(5.0/10.0);

	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);	
	cairo_line_to(cr, x, -h);
	cairo_line_to(cr, x-y/2, -h+y/2);
	cairo_line_to(cr, 0, -h+y/2);	

	extents_t result;
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);

	cairo_move_to(cr, 0, -h+y);
	cairo_line_to(cr, x, -h+y);
	cairo_line_to(cr, x-y/2, -h+y/2);
	cairo_line_to(cr, 0, -h+y/2);
	cairo_fill(cr);

	h-=y;

	cairo_line_to(cr, 0, -h);	
	cairo_line_to(cr, x, -h);
	cairo_line_to(cr, x-y/2, -h+y/2);
	cairo_line_to(cr, 0, -h+y/2);	

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+d/2+h2, d/2, 0, 2*M_PI);
	
	cairo_stroke(cr);

	cairo_move_to(cr, 0, -h+y);
	cairo_line_to(cr, x, -h+y);
	cairo_line_to(cr, x-y/2, -h+y/2);
	cairo_line_to(cr, 0, -h+y/2);	

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+d/2+h2, d/2, static_cast<int>(in_side)*(-M_PI/2), static_cast<int>(in_side)*M_PI/2);

	cairo_fill(cr);

	return result;
}

///42. Трубопровод
extents_t draw_tubing(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(25.0/10.0);	
	double d=in_axes->DrawingToDeviceY(2.0/10.0);

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h, d/2, 0, 2*M_PI);	

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///43. Пешеходный мост
extents_t draw_foot_bridge(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(32.0/10.0);	
	double d=in_axes->DrawingToDeviceY(2.0/10.0);
	double l=in_axes->DrawingToDeviceY(10.0/10.0);

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_move_to(cr, -l/2, -h-d);
	cairo_line_to(cr, -l/2, -h);
	cairo_line_to(cr, l/2, -h);
	cairo_line_to(cr, l/2, -h-d);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///44. Путепровод над ж/д
extents_t draw_overpass(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(32.0/10.0);	
	double d=in_axes->DrawingToDeviceY(2.0/10.0);
	double l=in_axes->DrawingToDeviceY(10.0/10.0);

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_move_to(cr, -l/2, -h+2*d);
	cairo_line_to(cr, -l/2, -h+d);
	cairo_line_to(cr, l/2, -h-d);
	cairo_line_to(cr, l/2, -h-2*d);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///45. Путепровод под ж/д
extents_t draw_underpass(cairo_t * cr, VAxesPtr in_axes)
{
	return draw_default_bottom(cr, in_axes);
}


///46. Труба круглая ж/б
extents_t draw_tube_1(cairo_t * cr, VAxesPtr in_axes)
{
	return draw_tubing(cr, in_axes);
}

///47. Труба металлич. гофрир.
extents_t draw_tube_2(cairo_t * cr, VAxesPtr in_axes)
{
	return draw_tubing(cr, in_axes);
}

///48. Труба овоидальная
extents_t draw_tube_3(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double h=in_axes->DrawingToDeviceY(25.0/10.0);	
	double d=in_axes->DrawingToDeviceY(2.0/10.0);

	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);	
	
	//cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h, d/2, -M_PI, 0);		
	cairo_line_to(cr, 0, -h);

	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_stroke(cr);
	return result;
}

///49. Труба прямоугольная ж/б
extents_t draw_tube_4(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(20.0/10.0);	
	double w=in_axes->DrawingToDeviceY(4.0/10.0);
	extents_t result;
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h);
	cairo_path_extents(cr, &result.x1, &result.y1, &result.x2, &result.y2);
	cairo_rectangle(cr, -w/2, -h-2*w, w, w);
	cairo_stroke(cr);
	cairo_rectangle(cr, -w/2, -h-w, w, w);
	cairo_fill(cr);
	return result;
}

//-------------------------------  Рисование объектов на ситуации --------------------------------------------

///1-8. Станция (любая)
void situation_draw_station_0(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	double w=in_axes->DrawingToDeviceY(6.0/10.0);		
	double h=in_axes->DrawingToDeviceY(3.0/10.0);			
	cairo_rectangle(cr, -w/2, -h/2, w, h);	
	cairo_stroke(cr);	
	cairo_text_extents_t te;
	CairoSetFont(cr, VFont(DEFAULT_FONT_FACE, in_axes->DrawingToDeviceY(BASE_FONT_SIZE().Value()), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD));				
	cairo_text_extents(cr, L"ПЗ", &te);
	cairo_move_to(cr, -te.width/2, te.height/2);	
	cairo_show_text(cr, L"ПЗ");		
}

///9. Стрелочный перевод симметричный (четн.)
void situation_draw_switch_1(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double h=in_axes->DrawingToDeviceY(3.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);	
	
	cairo_move_to(cr, 0, h/2);
	cairo_line_to(cr, 0, -h/2);	
	
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, -x, -h/2);
	cairo_line_to(cr, -x, h/2);
	cairo_line_to(cr, 0, 0);

	cairo_stroke(cr);
}

///10. Стрелочный перевод симметричный (нечетн.)
void situation_draw_switch_2(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double h=in_axes->DrawingToDeviceY(3.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);	
	
	cairo_move_to(cr, 0, h/2);
	cairo_line_to(cr, 0, -h/2);	
	
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, +x, -h/2);
	cairo_line_to(cr, +x, h/2);
	cairo_line_to(cr, 0, 0);

	cairo_stroke(cr);
}

///11. Стрелочный перевод (лев./четн.)
void situation_draw_switch_3(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double h=in_axes->DrawingToDeviceY(3.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);		
	cairo_move_to(cr, -x, 0);
	cairo_line_to(cr, 0, 0);
	cairo_line_to(cr, -x, h/2);			
	cairo_stroke(cr);
}

///12. Стрелочный перевод (лев/нечетн.)
void situation_draw_switch_4(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double h=in_axes->DrawingToDeviceY(3.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);		
	cairo_move_to(cr, x, 0);
	cairo_line_to(cr, 0, 0);
	cairo_line_to(cr, x, -h/2);			
	cairo_stroke(cr);
}

///13. Стрелочный перевод (прав./четн.)
void situation_draw_switch_5(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double h=in_axes->DrawingToDeviceY(3.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);		
	cairo_move_to(cr, -x, 0);
	cairo_line_to(cr, 0, 0);
	cairo_line_to(cr, -x, -h/2);			
	cairo_stroke(cr);
}

///14. Стрелочный перевод (прав./нечетн.)
void situation_draw_switch_6(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double h=in_axes->DrawingToDeviceY(3.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);		
	cairo_move_to(cr, x, 0);
	cairo_line_to(cr, 0, 0);
	cairo_line_to(cr, x, h/2);			
	cairo_stroke(cr);
}

///15. Стрелочный перевод перекрестный
void situation_draw_switch_7(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double h=in_axes->DrawingToDeviceY(3.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);		
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, -x/2, -h/2);
	cairo_line_to(cr, -x/2, h/2);
	cairo_line_to(cr, 0, 0);
	cairo_fill(cr);
	cairo_line_to(cr, x/2, -h/2);
	cairo_line_to(cr, x/2, h/2);
	cairo_line_to(cr, 0, 0);
	cairo_fill(cr);
	cairo_rectangle(cr, -x/2, -h/2, x, h);
	cairo_stroke(cr);
}

///16. Сбрасыватель башмака
void situation_draw_switch_8(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double h=in_axes->DrawingToDeviceY(5.0/10.0);
	double x=in_axes->DrawingToDeviceY(10.0/10.0);		
	cairo_move_to(cr, -x, 0);
	cairo_line_to(cr, 0, 0);
	cairo_line_to(cr, -x, -h);			
	cairo_stroke(cr);
}


///17. Светофор однолинзовый
void situation_draw_sign_1(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	//размеры увеличены в два раза		
	double x=in_axes->DrawingToDeviceY(sign_scale*2.0/10.0);
	double y=in_axes->DrawingToDeviceY(sign_scale*3.0/10.0);	
	double h=in_axes->DrawingToDeviceY(sign_scale*1.5/10.0)+y;
	double d=in_axes->DrawingToDeviceY(sign_scale*0.5/10.0);
	//double d=sign_scale*1.5;
	
	cairo_save(cr);
	cairo_rotate(cr, M_PI/2);	
	cairo_move_to(cr, -x/2, 0);
	cairo_line_to(cr, x/2, 0);

	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h+y);
	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y-x/2);
	cairo_arc(cr, 0, -h+y-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y/2, d, 0, 2*M_PI);
		
	cairo_stroke(cr);

	cairo_restore(cr);
	
}

///18. Светофор двухлинзовый
void situation_draw_sign_2(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	//размеры увеличены в два раза	
	double x=in_axes->DrawingToDeviceY(sign_scale*2.0/10.0);
	double y=in_axes->DrawingToDeviceY(sign_scale*5.0/10.0);
	double h=in_axes->DrawingToDeviceY(sign_scale*1.5/10.0)+y;
	double d=in_axes->DrawingToDeviceY(sign_scale*0.5/10.0);
	//double d=sign_scale*1.5;

	cairo_save(cr);
	cairo_rotate(cr, M_PI/2);	
	cairo_move_to(cr, -x/2, 0);
	cairo_line_to(cr, x/2, 0);
	
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h+y);
	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y-x/2);
	cairo_arc(cr, 0, -h+y-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y/3, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+2*y/3, d, 0, 2*M_PI);
		
	cairo_stroke(cr);	

	cairo_restore(cr);
}

///19. Светофор трехлинзовый
void situation_draw_sign_3(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	//размеры увеличены в два раза	
	double x=in_axes->DrawingToDeviceY(sign_scale*2.0/10.0);
	double y=in_axes->DrawingToDeviceY(sign_scale*5.0/10.0);
	double h=in_axes->DrawingToDeviceY(sign_scale*1.5/10.0)+y;
	double d=in_axes->DrawingToDeviceY(sign_scale*0.5/10.0);
	//double d=sign_scale*1.5;
	
	cairo_save(cr);
	cairo_rotate(cr, M_PI/2);	
	cairo_move_to(cr, -x/2, 0);
	cairo_line_to(cr, x/2, 0);

	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h+y);
	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y-x/2);
	cairo_arc(cr, 0, -h+y-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y/4, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+2*y/4, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+3*y/4, d, 0, 2*M_PI);
	
	cairo_stroke(cr);

	cairo_restore(cr);
}

///20. Светофор четырехлинзовый
void situation_draw_sign_4(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	//размеры увеличены в два раза	
	double x=in_axes->DrawingToDeviceY(sign_scale*2.0/10.0);
	double y=in_axes->DrawingToDeviceY(sign_scale*7.0/10.0);
	double h=in_axes->DrawingToDeviceY(sign_scale*1.5/10.0)+y;
	double d=in_axes->DrawingToDeviceY(sign_scale*0.5/10.0);
	//double d=1.5;

	cairo_save(cr);
	cairo_rotate(cr, M_PI/2);	
	cairo_move_to(cr, -x/2, 0);
	cairo_line_to(cr, x/2, 0);
	
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h+y);
	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y-x/2);
	cairo_arc(cr, 0, -h+y-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y/5, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+2*y/5, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+3*y/5, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+4*y/5, d, 0, 2*M_PI);
		
	cairo_stroke(cr);	

	cairo_restore(cr);
}

///21. Светофор пятилинзовый
void situation_draw_sign_5(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	//размеры увеличены в два раза	
	double x=in_axes->DrawingToDeviceY(sign_scale*2.0/10.0);
	double y1=in_axes->DrawingToDeviceY(sign_scale*5.0/10.0);
	double y2=in_axes->DrawingToDeviceY(sign_scale*5.0/10.0);
	double y3=in_axes->DrawingToDeviceY(sign_scale*3.0/10.0);
	double h=in_axes->DrawingToDeviceY(sign_scale*1.5/10.0)+y1+y2+y3;
	double d=in_axes->DrawingToDeviceY(sign_scale*0.5/10.0);
	//double d=sign_scale*1.5;

	cairo_save(cr);
	cairo_rotate(cr, M_PI/2);	
	cairo_move_to(cr, -x/2, 0);
	cairo_line_to(cr, x/2, 0);
	
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, -h+y1+y2+y3);

	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y1-x/2);
	cairo_arc(cr, 0, -h+y1-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y1/3, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+2*y1/3, d, 0, 2*M_PI);	

	h-=y1;

	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y2-x/2);
	cairo_arc(cr, 0, -h+y2-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y2/3, d, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+2*y2/3, d, 0, 2*M_PI);

	h-=y2;

	cairo_move_to(cr, -x/2, -h+x/2);
	cairo_arc(cr, 0, -h+x/2, x/2, M_PI, 0);
	cairo_line_to(cr, x/2, -h+y3-x/2);
	cairo_arc(cr, 0, -h+y3-x/2, x/2, 0, M_PI);
	cairo_line_to(cr, -x/2, -h+x/2);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, -h+y3/2, d, 0, 2*M_PI);
	
	cairo_stroke(cr);	

	cairo_restore(cr);
}

///22-25. Будка / Блок-пост / Жилой дом / Казарма
void situation_draw_building_1(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели
	//Для простоты не закрашиваем
	double w=in_axes->DrawingToDeviceY(6.0/10.0);		
	double h=in_axes->DrawingToDeviceY(3.0/10.0);			
	cairo_rectangle(cr, -w/2, -h/2, w, h);	
	cairo_stroke(cr);		
}

///26. Изостык
void situation_draw_izostyk_1(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double x=in_axes->DrawingToDeviceY(3.0/10.0);
	double y=in_axes->DrawingToDeviceY(1.0/10.0);
	
	cairo_save(cr);
	cairo_rotate(cr, M_PI/2);		

	cairo_move_to(cr, -x/2, 0);
	cairo_line_to(cr, x/2, 0);

	cairo_move_to(cr, -x/2, y/2);
	cairo_line_to(cr, -x/2, -y/2);

	cairo_move_to(cr, x/2, y/2);
	cairo_line_to(cr, x/2, -y/2);
	
	cairo_stroke(cr);

	cairo_restore(cr);
}

///27. Негабаритный изостык
void situation_draw_izostyk_2(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double x=in_axes->DrawingToDeviceY(3.0/10.0);
	double y=in_axes->DrawingToDeviceY(1.0/10.0);
	double d=in_axes->DrawingToDeviceY(4.0/10.0);

	cairo_save(cr);
	cairo_rotate(cr, M_PI/2);		
	
	cairo_move_to(cr, -x/2, 0);
	cairo_line_to(cr, x/2, 0);

	cairo_move_to(cr, -x/2, y/2);
	cairo_line_to(cr, -x/2, -y/2);

	cairo_move_to(cr, x/2, y/2);
	cairo_line_to(cr, x/2, -y/2);

	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, 0, d/2, 0, 2*M_PI);
	
	cairo_stroke(cr);	
	cairo_restore(cr);
}

///28. Предельный столбик
void situation_draw_stolbik_1(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double d=in_axes->DrawingToDeviceY(1.0/10.0);	
	cairo_move_to(cr, 0, 0);	
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, 0, d/2, 0, 2*M_PI);
	cairo_fill(cr);		
}

///29. Предельный столбик
void situation_draw_stolbik_2(cairo_t * cr, VAxesPtr in_axes)
{
	//Размеры в мм в соответствии с ЦПТ-54/26 пересчитываем в пиксели	
	double d=in_axes->DrawingToDeviceY(1.0/10.0);	
	cairo_move_to(cr, 0, 0);	
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, 0, d/2, 0, 2*M_PI);
	cairo_move_to(cr, -d, 0);
	cairo_line_to(cr, d, 0);
	cairo_move_to(cr, 0, -d);
	cairo_line_to(cr, 0, d);
	cairo_stroke(cr);
}

///30. Конец пути без упора
void situation_draw_dead_end_1(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(3.0/10.0);
	cairo_move_to(cr, 0, h/2);
	cairo_line_to(cr, 0, -h/2);
	cairo_stroke(cr);
}

///31. Конец пути с упором
void situation_draw_dead_end_2(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(3.0/10.0);
	double w=in_axes->DrawingToDeviceY(1.5/10.0);
	cairo_rectangle(cr, 0, -h/2, w/2, h);
	cairo_move_to(cr, 0, -h/2);
	cairo_line_to(cr, w, -h/2);
	cairo_move_to(cr, 0, h/2);
	cairo_line_to(cr, w, h/2);
	cairo_stroke(cr);	
}

///32. Конец пути с упором и балластной призмой
void situation_draw_dead_end_3(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(3.0/10.0);
	double h2=in_axes->DrawingToDeviceY(2.5/10.0);
	double w=in_axes->DrawingToDeviceY(1.5/10.0);
	double l=in_axes->DrawingToDeviceY(4.0/10.0);

	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, w, h2/2);
	cairo_line_to(cr, l, h2/2);
	cairo_line_to(cr, l, -h2/2);
	cairo_line_to(cr, w, -h2/2);
	cairo_line_to(cr, 0, 0);
	cairo_rectangle(cr, l, -h/2, w/2, h);
	cairo_move_to(cr, l, -h/2);
	cairo_line_to(cr, l+w, -h/2);
	cairo_move_to(cr, l, h/2);
	cairo_line_to(cr, l+w, h/2);
	cairo_stroke(cr);	
}

///33. Мост деревянный
void situation_draw_bridge_1(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(6.0/10.0);	
	double w=in_axes->DrawingToDeviceY(2.0/10.0);
	cairo_rectangle(cr, -w/2, -h/2, w, h);
	cairo_stroke(cr);
}

///34. Мост железобетонный
void situation_draw_bridge_2(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(2.0/10.0);
	double w=in_axes->DrawingToDeviceY(1.5/10.0);
	double l=in_axes->DrawingToDeviceY(6.0/10.0);//FixMe!!! длина должна зависить от фактической длины моста
	cairo_move_to(cr, -l/2-w, -h-w);
	cairo_line_to(cr, -l/2, -h);
	cairo_line_to(cr, l/2, -h);
	cairo_line_to(cr, l/2+w, -h-w);

	cairo_move_to(cr, -l/2-w, h+w);
	cairo_line_to(cr, -l/2, h);
	cairo_line_to(cr, l/2, h);
	cairo_line_to(cr, l/2+w, h+w);

	cairo_stroke(cr);
}

///35. Мост металлический (фермовый) с ездой понизу
void situation_draw_bridge_3(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(2.0/10.0);
	double w=in_axes->DrawingToDeviceY(1.5/10.0);
	double l=in_axes->DrawingToDeviceY(6.0/10.0);//FixMe!!! длина должна зависить от фактической длины моста

	cairo_rectangle(cr, -l/2, -h/2, l, h);

	cairo_move_to(cr, -l/2-w, -h/2-w);
	cairo_line_to(cr, -l/2, -h/2);
	cairo_move_to(cr, l/2, -h/2);
	cairo_line_to(cr, l/2+w, -h/2-w);

	cairo_move_to(cr, -l/2-w, h/2+w);
	cairo_line_to(cr, -l/2, h/2);
	cairo_move_to(cr, l/2, h/2);
	cairo_line_to(cr, l/2+w, h/2+w);

	cairo_move_to(cr, -l/2, -h/2);
	cairo_line_to(cr, -l/2+w, h/2);
	cairo_line_to(cr, -l/2+2*w, -h/2);
	cairo_line_to(cr, -l/2+3*w, h/2);
	cairo_line_to(cr, -l/2+4*w, -h/2);	

	cairo_stroke(cr);
}

///36. Мост металлический (фермовый) с ездой поверху
void situation_draw_bridge_4(cairo_t * cr, VAxesPtr in_axes)
{
	situation_draw_bridge_3(cr, in_axes);
}

///37. Начало/конец тормозного устройства*/
void situation_draw_brake(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(3.0/10.0);
	double s=in_axes->DrawingToDeviceY(1.0/10.0);
	cairo_move_to(cr, 0, s/2);
	cairo_line_to(cr, 0, s/2+h);
	cairo_move_to(cr, 0, -s/2);
	cairo_line_to(cr, 0, -s/2-h);
	cairo_stroke(cr);
}

///38. Переезд неохраняемый одинарный
void situation_draw_crossing_1(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(3.0/10.0);
	double w=in_axes->DrawingToDeviceY(1.0/10.0);
	double l=in_axes->DrawingToDeviceY(5.0/10.0);

	cairo_rectangle(cr, -l/2, -h/2, l, h);

	cairo_move_to(cr, -l/2-w, -h/2-w);
	cairo_line_to(cr, -l/2, -h/2);
	cairo_move_to(cr, l/2, -h/2);
	cairo_line_to(cr, l/2+w, -h/2-w);

	cairo_move_to(cr, -l/2-w, h/2+w);
	cairo_line_to(cr, -l/2, h/2);
	cairo_move_to(cr, l/2, h/2);
	cairo_line_to(cr, l/2+w, h/2+w);	

	cairo_stroke(cr);
}

///39. Переезд охраняемый одиночный
void situation_draw_crossing_2(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(3.0/10.0);
	double w=in_axes->DrawingToDeviceY(1.0/10.0);
	double l=in_axes->DrawingToDeviceY(5.0/10.0);

	cairo_rectangle(cr, -l/2, -h/2, l, h);

	cairo_move_to(cr, -l/2-w, -h/2-w);
	cairo_line_to(cr, -l/2, -h/2);
	cairo_move_to(cr, l/2, -h/2);
	cairo_line_to(cr, l/2+w, -h/2-w);

	cairo_move_to(cr, -l/2-w, h/2+w);
	cairo_line_to(cr, -l/2, h/2);
	cairo_move_to(cr, l/2, h/2);
	cairo_line_to(cr, l/2+w, h/2+w);

	cairo_move_to(cr, -l/2, -h/2-w/2);
	cairo_line_to(cr, l/2, -h/2-w/2);

	cairo_move_to(cr, -l/2, h/2+w/2);
	cairo_line_to(cr, l/2, h/2+w/2);

	cairo_stroke(cr);

	cairo_new_sub_path(cr);
	cairo_arc(cr, -l/2+w/3, -h/2-w/2, w/3, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, l/2-w/3, -h/2-w/2, w/3, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, -l/2+w/3, h/2+w/2, w/3, 0, 2*M_PI);
	cairo_new_sub_path(cr);
	cairo_arc(cr, l/2-w/3, h/2+w/2, w/3, 0, 2*M_PI);

	cairo_fill(cr);	
}

///40. Переезд неохраняемый двойной
void situation_draw_crossing_3(cairo_t * cr, VAxesPtr in_axes)
{
	situation_draw_crossing_1(cr, in_axes);
}

///41. Переезд охраняемый двойной
void situation_draw_crossing_4(cairo_t * cr, VAxesPtr in_axes)
{
	situation_draw_crossing_2(cr, in_axes);
}

///42. Трубопровод
void situation_draw_tubing(cairo_t * cr, VAxesPtr in_axes)
{
	double l1=in_axes->DrawingToDeviceY(10.0/10.0);
	double l2=in_axes->DrawingToDeviceX(2.0/10.0);
	cairo_move_to(cr, 0, l1/2);
	cairo_line_to(cr, 0, -l1/2);
	cairo_move_to(cr, 0, l1/2+l2);
	cairo_line_to(cr, 0, l1/2+2*l2);
	cairo_move_to(cr, 0, -l1/2-l2);
	cairo_line_to(cr, 0, -l1/2-2*l2);

	cairo_move_to(cr, -l2/2, l1/2+l2/2);
	cairo_line_to(cr, l2/2, l1/2+l2/2);
	cairo_move_to(cr, -l2/2, l1/2);
	cairo_line_to(cr, -l2/2, l1/2+l2);

	cairo_move_to(cr, -l2/2, -l1/2-l2/2);
	cairo_line_to(cr, l2/2, -l1/2-l2/2);
	cairo_move_to(cr, -l2/2, -l1/2);
	cairo_line_to(cr, -l2/2, -l1/2-l2);
	
	cairo_stroke(cr);
}

///43. Пешеходный мост
void situation_draw_foot_bridge(cairo_t * cr, VAxesPtr in_axes)
{
	double l1=in_axes->DrawingToDeviceY(10.0/10.0);
	double l2=in_axes->DrawingToDeviceX(2.0/10.0);
	cairo_rectangle(cr, 0-l2/2, 0-l1/2, l2, l1);
	cairo_stroke(cr);
}

///44. Путепровод над ж/д
void situation_draw_overpass(cairo_t * cr, VAxesPtr in_axes)
{
	double l1=in_axes->DrawingToDeviceY(10.0/10.0);
	double l2=in_axes->DrawingToDeviceX(2.0/10.0);
	cairo_rectangle(cr, 0-l2/2, 0-l1/2, l2, l1);
	cairo_save(cr);
	cairo_set_source_rgba(cr, 1,1,1,1);
	cairo_fill(cr);
	cairo_restore(cr);
	cairo_move_to(cr, 0-l2, 0-l1/2);
	cairo_line_to(cr, 0-l2/2, 0-l1/2);
	cairo_line_to(cr, 0-l2/2, 0+l1/2);
	cairo_line_to(cr, 0-l2, 0+l1/2);
	cairo_move_to(cr, 0+l2, 0-l1/2);
	cairo_line_to(cr, 0+l2/2, 0-l1/2);
	cairo_line_to(cr, 0+l2/2, 0+l1/2);
	cairo_line_to(cr, 0+l2, 0+l1/2);
	cairo_stroke(cr);
}

///45. Путепровод под ж/д
void situation_draw_underpass(cairo_t * cr, VAxesPtr in_axes)
{
	double h=in_axes->DrawingToDeviceY(2.0/10.0);
	double h2=in_axes->DrawingToDeviceY(10.0/10.0);
	double w=in_axes->DrawingToDeviceY(1.5/10.0);
	double l=in_axes->DrawingToDeviceY(6.0/10.0);

	cairo_rectangle(cr, -l/2, -h/2, l, h);

	cairo_move_to(cr, -l/2-w, -h/2-w);
	cairo_line_to(cr, -l/2, -h/2);
	cairo_move_to(cr, l/2, -h/2);
	cairo_line_to(cr, l/2+w, -h/2-w);

	cairo_move_to(cr, -l/2-w, h/2+w);
	cairo_line_to(cr, -l/2, h/2);
	cairo_move_to(cr, l/2, h/2);
	cairo_line_to(cr, l/2+w, h/2+w);

	cairo_move_to(cr, -l/2, -h/2);
	cairo_line_to(cr, -l/2+w, h/2);
	cairo_line_to(cr, -l/2+2*w, -h/2);
	cairo_line_to(cr, -l/2+3*w, h/2);
	cairo_line_to(cr, -l/2+4*w, -h/2);	

	cairo_move_to(cr, -h/2, -h/2);
	cairo_line_to(cr, -h/2, -h2/2);

	cairo_move_to(cr, -h/2, h/2);
	cairo_line_to(cr, -h/2, h2/2);

	cairo_move_to(cr, h/2, -h/2);
	cairo_line_to(cr, h/2, -h2/2);

	cairo_move_to(cr, h/2, h/2);
	cairo_line_to(cr, h/2, h2/2);

	cairo_stroke(cr);
}

///46. Труба круглая ж/б
void situation_draw_tube_1(cairo_t * cr, VAxesPtr in_axes)
{
	double l1=in_axes->DrawingToDeviceY(10.0/10.0);
	double l2=in_axes->DrawingToDeviceX(2.0/10.0);	
	cairo_move_to(cr, 0-l2, 0-l1/2);
	cairo_line_to(cr, 0-l2/2, 0-l1/2);
	cairo_line_to(cr, 0-l2/2, 0+l1/2);
	cairo_line_to(cr, 0-l2, 0+l1/2);
	cairo_move_to(cr, 0+l2, 0-l1/2);
	cairo_line_to(cr, 0+l2/2, 0-l1/2);
	cairo_line_to(cr, 0+l2/2, 0+l1/2);
	cairo_line_to(cr, 0+l2, 0+l1/2);
	cairo_stroke(cr);
}

///47. Труба металлич. гофрир.
void situation_draw_tube_2(cairo_t * cr, VAxesPtr in_axes)
{
	situation_draw_tube_1(cr, in_axes);
}
///48. Труба овоидальная
void situation_draw_tube_3(cairo_t * cr, VAxesPtr in_axes)
{
	situation_draw_tube_1(cr, in_axes);
}

///49. Труба прямоугольная ж/б
void situation_draw_tube_4(cairo_t * cr, VAxesPtr in_axes)
{
	situation_draw_tube_1(cr, in_axes);
}

//---------------------------------- Рисование объектов в стиле КВЛП -------------------------------

///Стрелочный перевод
void kvlp_draw_switch_0(cairo_t * cr, VAxesPtr in_axes, double in_length)
{	
	double r=in_axes->DrawingToDeviceX(0.5/10.0);
	double h=in_axes->DrawingToDeviceY(1.0/10.0);
	cairo_move_to(cr, 0, 0);
	cairo_new_sub_path(cr);	
	cairo_move_to(cr, 0, h);
	cairo_line_to(cr, 0, -h);

	cairo_move_to(cr, 0, h);
	cairo_line_to(cr, r, h/2);
	cairo_move_to(cr, 0, h);
	cairo_line_to(cr, -r, h/2);

	cairo_move_to(cr, 0, -h);
	cairo_line_to(cr, r, -h/2);
	cairo_move_to(cr, 0, -h);
	cairo_line_to(cr, -r, -h/2);

	cairo_move_to(cr, -in_length/2, 0);
	cairo_line_to(cr, in_length/2, 0);
	
	cairo_stroke(cr);
};

///Стрелочный перевод по ходу
void kvlp_draw_switch_1(cairo_t * cr, VAxesPtr in_axes, double in_length)
{
	double r=in_axes->DrawingToDeviceX(0.5/10.0);
	double h=in_axes->DrawingToDeviceY(1.0/10.0);
	
	cairo_new_sub_path(cr);

	cairo_move_to(cr, in_length, h);
	cairo_line_to(cr, in_length, -h);

	cairo_move_to(cr, in_length, h);
	cairo_line_to(cr, in_length+r, h/2);
	cairo_move_to(cr, in_length, h);
	cairo_line_to(cr, in_length-r, h/2);

	cairo_move_to(cr, in_length, -h);
	cairo_line_to(cr, in_length+r, -h/2);
	cairo_move_to(cr, in_length, -h);
	cairo_line_to(cr, in_length-r, -h/2);

	cairo_move_to(cr, in_length, 0);
	cairo_line_to(cr, 0, 0);

	cairo_stroke(cr);
}

///Стрелочный перевод против хода
void kvlp_draw_switch_2(cairo_t * cr, VAxesPtr in_axes, double in_length)
{
	double r=in_axes->DrawingToDeviceX(0.5/10.0);
	double h=in_axes->DrawingToDeviceY(1.0/10.0);
	
	cairo_new_sub_path(cr);
	cairo_move_to(cr, -in_length, h);
	cairo_line_to(cr, -in_length, -h);

	cairo_move_to(cr, -in_length, h);
	cairo_line_to(cr, -in_length+r, h/2);
	cairo_move_to(cr, -in_length, h);
	cairo_line_to(cr, -in_length-r, h/2);

	cairo_move_to(cr, -in_length, -h);
	cairo_line_to(cr, -in_length+r, -h/2);
	cairo_move_to(cr, -in_length, -h);
	cairo_line_to(cr, -in_length-r, -h/2);

	cairo_move_to(cr, -in_length, 0);
	cairo_line_to(cr, 0, 0);
	cairo_stroke(cr);
}