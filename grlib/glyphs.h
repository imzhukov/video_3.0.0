#pragma once

#include "VDrawing.h"
#include "axes.h"
#include <boost/shared_ptr.hpp>


//Функции рисования объектов
struct extents_t
{
	double x1;
	double y1;
	double x2;
	double y2;
};

//Константы, для типов значков в соответсвиис ЦПТ-54/26
/** Ось станции внеклассная*/
#define station_0 1
/** Ось станции 1 класс*/
#define station_1 2
/** Ось станции 2 класс*/
#define station_2 3
/** Ось станции 3 класс*/
#define station_3 4
/** Ось станции 4 класс*/
#define station_4 5
/** Ось станции 5 класс*/
#define station_5 6
/** Разъезд*/
#define station_6 7
/** Остановочный пункт*/
#define station_7 8
/** Стрелочный перевод симметричный чётный*/
#define switch_1 9
/** Стрелочный перевод симметричный нечётный*/
#define switch_2 10
/** Стрелочный перевод левый чётный*/
#define switch_3 11
/** Стрелочный перевод левый нечётный*/
#define switch_4 12
/** Стрелочный перевод правый чётный*/
#define switch_5 13
/** Стрелочный перевод правый нечётный*/
#define switch_6 14
/** Стрелочный перевод перекрёстный*/
#define switch_7 15
/** Сбрасыватель башмака*/
#define switch_8 16
/** Светофор однолинзовый*/
#define sign_1 17
/** Светофор двухлинзовый*/
#define sign_2 18
/** Светофор трёхлинзовый*/
#define sign_3 19
/** Светофор четырёхлинзовый*/
#define sign_4 20
/** Светофор пятилинзовый*/
#define sign_5 21
/** Будка*/
#define building_1 22
/** Блок-пост*/
#define building_2 23
/** Жилой дом*/
#define building_3 24
/** Казарма*/
#define building_4 25
/** Изолирующий стык*/
#define izostyk_1 26
/** Негабаритный изолирующий стык*/
#define izostyk_2 27
/** Предельный столбик*/
#define stolbik_1 28
/** Отсутствующий Предельный столбик*/
#define stolbik_2 29
/** Конец пути без упора*/
#define dead_end_1 30
/** Конец пути с упором*/
#define dead_end_2 31
/** Конец пути с упором и балластной призмой*/
#define dead_end_3 32
/** Мост деревянный*/
#define bridge_1 33
/** Мост железобетонный*/
#define bridge_2 34
/** Мост металлический (фермовый) с ездой понизу*/
#define bridge_3 35
/** Мост металлический (фермовый) с ездой поверху*/
#define bridge_4 36
/** Начало/конец тормозного устройства*/
#define brake 37
/** Переезд неохраняемый одинарный*/
#define crossing_1 38
/** Переезд охраняемый одинарный*/
#define crossing_2 39
/** Переезд неохраняемый двойной*/
#define crossing_3 40
/** Переезд охраняемый двойной*/
#define crossing_4 41
/** Трубопровод*/
#define tubing 42
/** Пешеходный мост*/
#define foot_bridge 43
/** Путепровод над ж/д*/
#define overpass 44
/** Путепровод под ж/д*/
#define underpass 45
/** Труба круглая ж/б*/
#define tube_1 46
/** Труба металлическая*/
#define tube_2 47
/** Труба овоидальная*/
#define tube_3 48
/** Труба прямоугольная*/
#define tube_4 49
/** Подкрановые пути*/
#define crane_way 50
/** Эстакада*/
#define viaduct 51
/** Линия электропередачи*/
#define ptl 52
/** Линия связи*/
#define communication_line 53
/** Конкорс*/
#define concourse 54
/** Галерея*/
#define gallery 55
/** Платформа высокая*/
#define platform_1 56
/** Платформа низкая*/
#define platform_2 57
/** Колесосбрасывающий башмак*/
#define stop_block_1 58
/** Упор тормозной стационарный*/
#define stop_block_2 59
/** Горка малой мощности (полугорка)*/
#define hump_1 60
/** Горка*/
#define hump_2 61
/** Габаритные ворота*/
#define gate 62
/** Граница станции*/
#define border_1 63
/** Весы вагонные*/
#define weightbridge_1 64
/** Весы путевые*/
#define weightbridge_2 65
/** Вагонный замедлитель*/
#define rail_brake 66
/** Маневровая колонка*/
#define instrument_column 67
/** Указатель*/
#define guide_1 68
/** Граница пути необщего пользования (подъездного пути)*/
#define border_2 69
/** Указатель роспуска вагонов*/
#define guide_2 70
/** Электронный счетчик осей вагонов*/
#define wheel_counter 71
/** Опора ж/б*/
#define pier_1 72
/** Опора ж/б двойная*/
#define pier_2 73
/** Опора металлическая фермовая, мачта освещения*/
#define pier_3 74


/** Километровый столб*/
#define kms_1 101
/** Вспомогательный объект*/
#define default_1 102
/** Не отображать*/
#define do_not_draw 100

//-------------------- Значки объектов в соответствии с ЦПТ-54/26 -------------------------------

/// Подпись к объекту
void show_object_text(cairo_t * cr, VAxesPtr in_axes, const std::wstring& label, bool in_show_name, bool in_show_coord);


/// Километровый столб
extents_t draw_km(cairo_t * cr, VAxesPtr in_axes, side_t in_side);

///0. По умолчанию
extents_t draw_default(cairo_t * cr, VAxesPtr in_axes);
///1. Станция (внеклассная)
extents_t draw_station_0(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///2. Станция (первый класс)
extents_t draw_station_1(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///3. Станция (второй класс)
extents_t draw_station_2(cairo_t * cr, VAxesPtr in_axes);
///4. Станция (третий класс)
extents_t draw_station_3(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///5. Станция (четвертый класс)
extents_t draw_station_4(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///6. Станция (пятый класс)
extents_t draw_station_5(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///7. Станция (разъезд)
extents_t draw_station_6(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///8. Остановочный пункт
extents_t draw_station_7(cairo_t * cr, VAxesPtr in_axes);

///9. Стрелочный перевод симметричный (четн.)
extents_t draw_switch_1(cairo_t * cr, VAxesPtr in_axes);
///10. Стрелочный перевод симметричный (нечетн.)
extents_t draw_switch_2(cairo_t * cr, VAxesPtr in_axes);
///11. Стрелочный перевод (лев./четн.)
extents_t draw_switch_3(cairo_t * cr, VAxesPtr in_axes);
///12. Стрелочный перевод (лев/нечетн.)
extents_t draw_switch_4(cairo_t * cr, VAxesPtr in_axes);
///13. Стрелочный перевод (прав./четн.)
extents_t draw_switch_5(cairo_t * cr, VAxesPtr in_axes);
///14. Стрелочный перевод (прав./нечетн.)
extents_t draw_switch_6(cairo_t * cr, VAxesPtr in_axes);
///15. Стрелочный перевод перекрестный
extents_t draw_switch_7(cairo_t * cr, VAxesPtr in_axes);
///16. Сбрасыватель башмака
extents_t draw_switch_8(cairo_t * cr, VAxesPtr in_axes);

///17. Светофор однолинзовый
extents_t draw_sign_1(cairo_t * cr, VAxesPtr in_axes);
///18. Светофор двухлинзовый
extents_t draw_sign_2(cairo_t * cr, VAxesPtr in_axes);
///19. Светофор трехлинзовый
extents_t draw_sign_3(cairo_t * cr, VAxesPtr in_axes);
///20. Светофор четырехлинзовый
extents_t draw_sign_4(cairo_t * cr, VAxesPtr in_axes);
///21. Светофор пятилинзовый
extents_t draw_sign_5(cairo_t * cr, VAxesPtr in_axes);

///22. Будка
extents_t draw_building_1(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///23. Блок-пост
extents_t draw_building_2(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///24. Жилой дом
extents_t draw_building_3(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///25. Казарма
extents_t draw_building_4(cairo_t * cr, VAxesPtr in_axes, side_t in_side);

///26. Изостык
extents_t draw_izostyk_1(cairo_t * cr, VAxesPtr in_axes);
///27. Негабаритный изостык
extents_t draw_izostyk_2(cairo_t * cr, VAxesPtr in_axes);

///28. Предельный столбик
extents_t draw_stolbik_1(cairo_t * cr, VAxesPtr in_axes);
///29. Отсутствующий предельный столбик
extents_t draw_stolbik_2(cairo_t * cr, VAxesPtr in_axes);

///30. Конец пути без упора
extents_t draw_dead_end_1(cairo_t * cr, VAxesPtr in_axes);
///31. Конец пути с упором
extents_t draw_dead_end_2(cairo_t * cr, VAxesPtr in_axes);
///32. Конец пути с упором и балластной призмой
extents_t draw_dead_end_3(cairo_t * cr, VAxesPtr in_axes);

///33. Мост деревянный
extents_t draw_bridge_1(cairo_t * cr, VAxesPtr in_axes, double in_length=0);
///34. Мост железобетонный
extents_t draw_bridge_2(cairo_t * cr, VAxesPtr in_axes, double in_length=0);
///35. Мост металлический (фермовый) с ездой понизу
extents_t draw_bridge_3(cairo_t * cr, VAxesPtr in_axes, double in_length=0);
/// Мост металлический (фермовый) с ездой поверху
extents_t draw_bridge_4(cairo_t * cr, VAxesPtr in_axes, double in_length=0);

///37. Начало/конец тормозного устройства*/
extents_t draw_brake(cairo_t * cr, VAxesPtr in_axes);

///38. Переезд неохраняемый одинарный
extents_t draw_crossing_1(cairo_t * cr, VAxesPtr in_axes);
///39. Переезд охраняемый одиночный
extents_t draw_crossing_2(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///40. Переезд неохраняемый двойной
extents_t draw_crossing_3(cairo_t * cr, VAxesPtr in_axes);
///41. Переезд охраняемый двойной
extents_t draw_crossing_4(cairo_t * cr, VAxesPtr in_axes, side_t in_side);

///42. Трубопровод
extents_t draw_tubing(cairo_t * cr, VAxesPtr in_axes);
///43. Пешеходный мост
extents_t draw_foot_bridge(cairo_t * cr, VAxesPtr in_axes);
///44. Путепровод над ж/д
extents_t draw_overpass(cairo_t * cr, VAxesPtr in_axes);
///45. Путепровод под ж/д
extents_t draw_underpass(cairo_t * cr, VAxesPtr in_axes);

///46. Труба круглая ж/б
extents_t draw_tube_1(cairo_t * cr, VAxesPtr in_axes);
///47. Труба металлич. гофрир.
extents_t draw_tube_2(cairo_t * cr, VAxesPtr in_axes);
///48. Труба овоидальная
extents_t draw_tube_3(cairo_t * cr, VAxesPtr in_axes);
///49. Труба прямоугольная ж/б
extents_t draw_tube_4(cairo_t * cr, VAxesPtr in_axes);


// -------------------------------  Рисование объектов на ситуации --------------------------------------------

///1-8. Станция (любая)
void situation_draw_station_0(cairo_t * cr, VAxesPtr in_axes);

///9. Стрелочный перевод симметричный (четн.)
void situation_draw_switch_1(cairo_t * cr, VAxesPtr in_axes);
///10. Стрелочный перевод симметричный (нечетн.)
void situation_draw_switch_2(cairo_t * cr, VAxesPtr in_axes);
///11. Стрелочный перевод (лев./четн.)
void situation_draw_switch_3(cairo_t * cr, VAxesPtr in_axes);
///12. Стрелочный перевод (лев/нечетн.)
void situation_draw_switch_4(cairo_t * cr, VAxesPtr in_axes);
///13. Стрелочный перевод (прав./четн.)
void situation_draw_switch_5(cairo_t * cr, VAxesPtr in_axes);
///14. Стрелочный перевод (прав./нечетн.)
void situation_draw_switch_6(cairo_t * cr, VAxesPtr in_axes);
///15. Стрелочный перевод перекрестный
void situation_draw_switch_7(cairo_t * cr, VAxesPtr in_axes);
///16. Сбрасыватель башмака
void situation_draw_switch_8(cairo_t * cr, VAxesPtr in_axes);

///17. Светофор однолинзовый
void situation_draw_sign_1(cairo_t * cr, VAxesPtr in_axes);
///18. Светофор двухлинзовый
void situation_draw_sign_2(cairo_t * cr, VAxesPtr in_axes);
///19. Светофор трехлинзовый
void situation_draw_sign_3(cairo_t * cr, VAxesPtr in_axes);
///20. Светофор четырехлинзовый
void situation_draw_sign_4(cairo_t * cr, VAxesPtr in_axes);
///21. Светофор пятилинзовый
void situation_draw_sign_5(cairo_t * cr, VAxesPtr in_axes);

///22-25. Будка / Блок-пост / Жилой дом / Казарма
void situation_draw_building_1(cairo_t * cr, VAxesPtr in_axes);

///26. Изостык
void situation_draw_izostyk_1(cairo_t * cr, VAxesPtr in_axes);
///27. Негабаритный изостык
void situation_draw_izostyk_2(cairo_t * cr, VAxesPtr in_axes);

///28. Предельный столбик
void situation_draw_stolbik_1(cairo_t * cr, VAxesPtr in_axes);
///29. Отсутствующий предельный столбик
void situation_draw_stolbik_2(cairo_t * cr, VAxesPtr in_axes);

///30. Конец пути без упора
void situation_draw_dead_end_1(cairo_t * cr, VAxesPtr in_axes);
///31. Конец пути с упором
void situation_draw_dead_end_2(cairo_t * cr, VAxesPtr in_axes);
///32. Конец пути с упором и балластной призмой
void situation_draw_dead_end_3(cairo_t * cr, VAxesPtr in_axes);

///33. Мост деревянный
void situation_draw_bridge_1(cairo_t * cr, VAxesPtr in_axes);
///34. Мост железобетонный
void situation_draw_bridge_2(cairo_t * cr, VAxesPtr in_axes);
///35. Мост металлический (фермовый) с ездой понизу
void situation_draw_bridge_3(cairo_t * cr, VAxesPtr in_axes);
///36. Мост металлический (фермовый) с ездой поверху
void situation_draw_bridge_4(cairo_t * cr, VAxesPtr in_axes);

///37. Начало/конец тормозного устройства*/
void situation_draw_brake(cairo_t * cr, VAxesPtr in_axes);

///38. Переезд неохраняемый одинарный
void situation_draw_crossing_1(cairo_t * cr, VAxesPtr in_axes);
///39. Переезд охраняемый одиночный
void situation_draw_crossing_2(cairo_t * cr, VAxesPtr in_axes);
///40. Переезд неохраняемый двойной
void situation_draw_crossing_3(cairo_t * cr, VAxesPtr in_axes);
///41. Переезд охраняемый двойной
void situation_draw_crossing_4(cairo_t * cr, VAxesPtr in_axes);

///42. Трубопровод
void situation_draw_tubing(cairo_t * cr, VAxesPtr in_axes);
///43. Пешеходный мост
void situation_draw_foot_bridge(cairo_t * cr, VAxesPtr in_axes);
///44. Путепровод над ж/д
void situation_draw_overpass(cairo_t * cr, VAxesPtr in_axes);
///45. Путепровод под ж/д
void situation_draw_underpass(cairo_t * cr, VAxesPtr in_axes);

///46. Труба круглая ж/б
void situation_draw_tube_1(cairo_t * cr, VAxesPtr in_axes);
///47. Труба металлич. гофрир.
void situation_draw_tube_2(cairo_t * cr, VAxesPtr in_axes);
///48. Труба овоидальная
void situation_draw_tube_3(cairo_t * cr, VAxesPtr in_axes);
///49. Труба прямоугольная ж/б
void situation_draw_tube_4(cairo_t * cr, VAxesPtr in_axes);

//---------------------------------- Рисование объектов в стиле КВЛП -------------------------------

///Стрелочный перевод
void kvlp_draw_switch_0(cairo_t * cr, VAxesPtr in_axes, double in_length);
///Стрелочный перевод по ходу
void kvlp_draw_switch_1(cairo_t * cr, VAxesPtr in_axes, double in_length);
///Стрелочный перевод против хода
void kvlp_draw_switch_2(cairo_t * cr, VAxesPtr in_axes, double in_length);



//----------------------------- Класс отображение объекта ------------------------------------------


class VObjectGlyph
{
public:
	virtual extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)=0;
	virtual void DrawSituation(cairo_t * cr, VAxesPtr in_axes)=0;
	virtual bool IsObjectOverProfile() const {return true;};
	virtual bool ShowName() const {return true;};
	virtual bool ShowCoord() const {return true;};
};
typedef boost::shared_ptr<VObjectGlyph> VObjectGlyphPtr;

class VEmptyGlyph : public VObjectGlyph
{
public:
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes) {};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes) {};
};

class VDefaultGlyph : public VObjectGlyph
{
public:
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_default(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes){};
};

///1. Станция (внеклассная)
class VStation0 : public VObjectGlyph
{
private:	
	side_t side;
public:
	VStation0(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return side!=SD_UNDEFINED ? draw_station_0(cr, in_axes, side) : draw_default(cr, in_axes);
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{				
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);				
		cairo_translate(cr, 0, s);		
		situation_draw_station_0(cr, in_axes);		
	};
};

///2. Станция (первый класс)
class VStation1 : public VObjectGlyph
{
private:	
	side_t side;
public:
	VStation1(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return side!=SD_UNDEFINED ? draw_station_1(cr, in_axes, side) : draw_default(cr, in_axes);
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);
		cairo_translate(cr, 0, s);		
		situation_draw_station_0(cr, in_axes);	
	};
};

///3. Станция (второй класс)
class VStation2 : public VObjectGlyph
{
private:	
	side_t side;
public:
	VStation2(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_station_2(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{	
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);				
		cairo_translate(cr, 0, s);		
		situation_draw_station_0(cr, in_axes);	
	};
};

///4. Станция (третий класс)
class VStation3 : public VObjectGlyph
{
private:	
	side_t side;
public:
	VStation3(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return side!=SD_UNDEFINED ? draw_station_3(cr, in_axes, side) : draw_default(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{	
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);				
		cairo_translate(cr, 0, s);		
		situation_draw_station_0(cr, in_axes);		
	};
};

///5. Станция (четвертый класс)
class VStation4 : public VObjectGlyph
{
private:	
	side_t side;
public:
	VStation4(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return side!=SD_UNDEFINED ? draw_station_4(cr, in_axes, side) : draw_default(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{	
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);				
		cairo_translate(cr, 0, s);		
		situation_draw_station_0(cr, in_axes);		
	};
};

///6. Станция (пятый класс)
class VStation5 : public VObjectGlyph
{
private:	
	side_t side;
public:
	VStation5(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return side!=SD_UNDEFINED ? draw_station_5(cr, in_axes, side) : draw_default(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{	
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);				
		cairo_translate(cr, 0, s);		
		situation_draw_station_0(cr, in_axes);		
	};
};

///7. Станция (разъезд)
class VStation6 : public VObjectGlyph
{
private:	
	side_t side;
public:
	VStation6(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return side!=SD_UNDEFINED ? draw_station_6(cr, in_axes, side) : draw_default(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{	
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);				
		cairo_translate(cr, 0, s);		
		situation_draw_station_0(cr, in_axes);	
	};
};

///8. Остановочный пункт
class VStation7 : public VObjectGlyph
{
private:	
	side_t side;
public:
	VStation7(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{						
		return draw_station_7(cr, in_axes);
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{	
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);				
		cairo_translate(cr, 0, s);		
		situation_draw_station_0(cr, in_axes);	
	};
};

///9. Стрелочный перевод симметричный (четн.)
class VSwitch1 : public VObjectGlyph
{
public:
	VSwitch1() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_switch_1(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{				
		situation_draw_switch_1(cr, in_axes);		
	};
};

///10. Стрелочный перевод симметричный (нечетн.)
class VSwitch2 : public VObjectGlyph
{
public:
	VSwitch2() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_switch_2(cr, in_axes);	
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_switch_2(cr, in_axes);		
	};
};

///11. Стрелочный перевод (лев./четн.)
class VSwitch3 : public VObjectGlyph
{
public:
	VSwitch3() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_switch_3(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_switch_3(cr, in_axes);	
	};
};

///12. Стрелочный перевод (лев/нечетн.)
class VSwitch4 : public VObjectGlyph
{
public:
	VSwitch4() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_switch_4(cr, in_axes);	
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_switch_4(cr, in_axes);
	};
};

///13. Стрелочный перевод (прав./четн.)
class VSwitch5 : public VObjectGlyph
{
public:
	VSwitch5() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_switch_5(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_switch_5(cr, in_axes);	
	};
};

///14. Стрелочный перевод (прав./нечетн.)
class VSwitch6 : public VObjectGlyph
{
public:
	VSwitch6() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_switch_6(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{
		situation_draw_switch_6(cr, in_axes);
	};
};

///15. Стрелочный перевод перекрестный
class VSwitch7 : public VObjectGlyph
{
public:
	VSwitch7() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_switch_7(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{	
		situation_draw_switch_7(cr, in_axes);	
	};
};

///16. Сбрасыватель башмака
class VSwitch8 : public VObjectGlyph
{
public:
	VSwitch8() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_switch_8(cr, in_axes);			
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_switch_8(cr, in_axes);		
	};
};

///17. Светофор однолинзовый
class VSign1 : public VObjectGlyph
{
private:
	side_t side;
public:
	VSign1(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_sign_1(cr, in_axes);			
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{				
		double s;
		double a;
		switch (side)
		{
			case SD_LEFT:
				a=-M_PI;
				s=-in_axes->DrawingToDeviceY(3.0/10.0);			
				break;
			case SD_RIGHT:
				a=0;
				s=in_axes->DrawingToDeviceY(3.0/10.0);
				break;
			default:
				a=0;
				s=0;
				break;
		}
		cairo_translate(cr, 0, s);
		cairo_rotate(cr, a);

		situation_draw_sign_1(cr, in_axes);		
	};
};

///18. Светофор двухлинзовый
class VSign2 : public VObjectGlyph
{
private:
	side_t side;
public:
	VSign2(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_sign_2(cr, in_axes);			
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		double s;
		double a;
		switch (side)
		{
			case SD_LEFT:
				a=-M_PI;
				s=-in_axes->DrawingToDeviceY(3.0/10.0);			
				break;
			case SD_RIGHT:
				a=0;
				s=in_axes->DrawingToDeviceY(3.0/10.0);
				break;
			default:
				a=0;
				s=0;
				break;
		}
		cairo_translate(cr, 0, s);
		cairo_rotate(cr, a);

		situation_draw_sign_2(cr, in_axes);		
	};
};

///19. Светофор трехлинзовый
class VSign3 : public VObjectGlyph
{
private:
	side_t side;
public:
	VSign3(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_sign_3(cr, in_axes);			
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		double s;
		double a;
		switch (side)
		{
			case SD_LEFT:
				a=-M_PI;
				s=-in_axes->DrawingToDeviceY(3.0/10.0);			
				break;
			case SD_RIGHT:
				a=0;
				s=in_axes->DrawingToDeviceY(3.0/10.0);
				break;
			default:
				a=0;
				s=0;
				break;
		}
		cairo_translate(cr, 0, s);
		cairo_rotate(cr, a);

		situation_draw_sign_3(cr, in_axes);		
	};
};

///20. Светофор четырехлинзовый
class VSign4 : public VObjectGlyph
{
private:
	side_t side;
public:
	VSign4(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_sign_4(cr, in_axes);			
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		double s;
		double a;
		switch (side)
		{
			case SD_LEFT:
				a=-M_PI;
				s=-in_axes->DrawingToDeviceY(3.0/10.0);			
				break;
			case SD_RIGHT:
				a=0;
				s=in_axes->DrawingToDeviceY(3.0/10.0);
				break;
			default:
				a=0;
				s=0;
				break;
		}
		cairo_translate(cr, 0, s);
		cairo_rotate(cr, a);
		situation_draw_sign_4(cr, in_axes);		
	};
};

///21. Светофор пятилинзовый
class VSign5 : public VObjectGlyph
{
private:
	side_t side;
public:
	VSign5(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_sign_5(cr, in_axes);			
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		double s;
		double a;
		switch (side)
		{
			case SD_LEFT:
				a=-M_PI;
				s=-in_axes->DrawingToDeviceY(3.0/10.0);			
				break;
			case SD_RIGHT:
				a=0;
				s=in_axes->DrawingToDeviceY(3.0/10.0);
				break;
			default:
				a=0;
				s=0;
				break;
		}
		cairo_translate(cr, 0, s);
		cairo_rotate(cr, a);
		situation_draw_sign_5(cr, in_axes);		
	};
};

///22. Будка
class VBuilding1 : public VObjectGlyph
{
private:
	side_t side;
public:
	VBuilding1(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return side!=SD_UNDEFINED ? draw_building_1(cr, in_axes, side) : draw_default(cr, in_axes);			
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);
		cairo_translate(cr, 0, s);
		situation_draw_building_1(cr, in_axes);		
	};
};

///23. Блок-пост
class VBuilding2 : public VObjectGlyph
{
private:
	side_t side;
public:
	VBuilding2(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return side!=SD_UNDEFINED ? draw_building_2(cr, in_axes, side) : draw_default(cr, in_axes);			
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);
		cairo_translate(cr, 0, s);
		situation_draw_building_1(cr, in_axes);		
	};
};

///24. Жилой дом
class VBuilding3 : public VObjectGlyph
{
private:
	side_t side;
public:
	VBuilding3(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return side!=SD_UNDEFINED ? draw_building_3(cr, in_axes, side) : draw_default(cr, in_axes);
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);
		cairo_translate(cr, 0, s);
		situation_draw_building_1(cr, in_axes);		
	};
};

///25. Казарма
class VBuilding4 : public VObjectGlyph
{
private:
	side_t side;
public:
	VBuilding4(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return side!=SD_UNDEFINED ? draw_building_4(cr, in_axes, side) : draw_default(cr, in_axes);
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);
		cairo_translate(cr, 0, s);
		situation_draw_building_1(cr, in_axes);		
	};
};

/// Настройка "показывать имена изостыков"
DECLARE_OPTION(bool, STATION_PROFILE_SHOW_IZOSTYK_NAME, false, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())

///26. Изостык
class VIzostyk1 : public VObjectGlyph
{
public:
	VIzostyk1(){};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_izostyk_1(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{
		situation_draw_izostyk_1(cr, in_axes);
	};
	bool ShowName() const {return STATION_PROFILE_SHOW_IZOSTYK_NAME().Value();};
};

///27. Негабаритный изостык
class VIzostyk2 : public VObjectGlyph
{
public:
	VIzostyk2(){};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_izostyk_2(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{
		situation_draw_izostyk_2(cr, in_axes);
	};
	bool ShowName() const {return STATION_PROFILE_SHOW_IZOSTYK_NAME().Value();};
};

/// Настройка "показывать имена предельных столбиков"
DECLARE_OPTION(bool, STATION_PROFILE_SHOW_STOLBIK_NAME, false, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())

//28. Предельный столбик
class VStolbik1 : public VObjectGlyph
{
private:
	side_t side;
public:
	VStolbik1(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_stolbik_1(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);
		cairo_translate(cr, 0, s);
		situation_draw_stolbik_1(cr, in_axes);
	};
	bool ShowName() const {return STATION_PROFILE_SHOW_STOLBIK_NAME().Value();};
};

//29. Отсутствующий предельный столбик
class VStolbik2 : public VObjectGlyph
{
private:
	side_t side;
public:
	VStolbik2(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_stolbik_2(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{
		double s=(side==SD_LEFT) ? -in_axes->DrawingToDeviceY(3.0/10.0) : in_axes->DrawingToDeviceY(3.0/10.0);
		cairo_translate(cr, 0, s);
		situation_draw_stolbik_2(cr, in_axes);
	};
	bool IsObjectOverProfile() const
	{
		return false;
	};
	bool ShowName() const {return STATION_PROFILE_SHOW_STOLBIK_NAME().Value();};
};

///30. Конец пути без упора
class VDeadEnd1 : public VObjectGlyph
{
public:
	VDeadEnd1(){};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_dead_end_1(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{
		situation_draw_dead_end_1(cr, in_axes);
	};
	bool IsObjectOverProfile() const
	{
		return false;
	};
};

///31. Конец пути с упором
class VDeadEnd2 : public VObjectGlyph
{
public:
	VDeadEnd2(){};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_dead_end_2(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{
		situation_draw_dead_end_2(cr, in_axes);
	};
	bool IsObjectOverProfile() const
	{
		return false;
	};
};

///32. Конец пути с упором и балластной призмой
class VDeadEnd3 : public VObjectGlyph
{
public:
	VDeadEnd3(){};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_dead_end_3(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{
		situation_draw_dead_end_3(cr, in_axes);
	};
	bool IsObjectOverProfile() const
	{
		return false;
	};
};

///33. Мост деревянный
class VBridge1 : public VObjectGlyph
{
private:
	double length;
public:
	VBridge1(double in_length) : length(in_length) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_bridge_1(cr, in_axes, length);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{
		situation_draw_bridge_1(cr, in_axes);
	};
	bool IsObjectOverProfile() const
	{
		return false;
	};
};

///34. Мост железобетонный
class VBridge2 : public VObjectGlyph
{
private:
	double length;
public:
	VBridge2(double in_length) : length(in_length) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{		
		return draw_bridge_2(cr, in_axes, length);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_bridge_2(cr, in_axes);	
	};
	bool IsObjectOverProfile() const
	{
		return false;
	};
};

///35. Мост металлический (фермовый) с ездой понизу
class VBridge3 : public VObjectGlyph
{
private:
	double length;
public:
	VBridge3(double in_length) : length(in_length) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{		
		return draw_bridge_3(cr, in_axes, length);	
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{				
		situation_draw_bridge_3(cr, in_axes);	
	};
	bool IsObjectOverProfile() const
	{
		return false;
	};
};

///36. Мост металлический (фермовый) с ездой поверху
class VBridge4 : public VObjectGlyph
{	
private:
	double length;
public:
	VBridge4(double in_length) : length(in_length) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_bridge_4(cr, in_axes, length);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_bridge_4(cr, in_axes);	
	};
	bool IsObjectOverProfile() const
	{
		return false;
	};
};

///37. Начало/конец тормозного устройства
class VBrake : public VObjectGlyph
{
public:
	VBrake(){};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_brake(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_brake(cr, in_axes);	
	};
	bool IsObjectOverProfile() const
	{
		return false;
	};
};

///38. Переезд неохраняемый одинарный
class VCrossing1 : public VObjectGlyph
{
public:
	VCrossing1(){};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_crossing_1(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_crossing_1(cr, in_axes);	
	};
};

///39. Переезд охраняемый одиночный
class VCrossing2 : public VObjectGlyph
{
private:
	side_t side;
public:
	VCrossing2(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return side!=SD_UNDEFINED ? draw_crossing_2(cr, in_axes, side) : draw_default(cr, in_axes);
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_crossing_2(cr, in_axes);	
	};
};

///40. Переезд неохраняемый двойной
class VCrossing3 : public VObjectGlyph
{
public:
	VCrossing3() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_crossing_3(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_crossing_3(cr, in_axes);	
	};
};

///41. Переезд охраняемый двойной
class VCrossing4 : public VObjectGlyph
{
private:
	side_t side;
public:
	VCrossing4(side_t in_side) : side(in_side) {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return side!=SD_UNDEFINED ? draw_crossing_4(cr, in_axes, side) : draw_default(cr, in_axes);
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_crossing_4(cr, in_axes);	
	};
};

///42. Трубопровод
class VTubing : public VObjectGlyph
{	
public:
	VTubing() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_tubing(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_tubing(cr, in_axes);	
	};
};

///43. Пешеходный мост
class VFootBridge : public VObjectGlyph
{	
public:
	VFootBridge() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_foot_bridge(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_foot_bridge(cr, in_axes);	
	};
};

///44. Путепровод над ж/д
class VOverpass : public VObjectGlyph
{	
public:
	VOverpass() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_overpass(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_overpass(cr, in_axes);	
	};
};

///45. Путепровод под ж/д
class VUnderpass : public VObjectGlyph
{	
public:
	VUnderpass() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_underpass(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_underpass(cr, in_axes);
	};
	bool IsObjectOverProfile() const
	{
		return false;
	};
};

///46. Труба круглая ж/б
class VTube1 : public VObjectGlyph
{	
public:
	VTube1() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_tube_1(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_tube_1(cr, in_axes);
	};
};

///47. Труба металлич. гофрир.
class VTube2 : public VObjectGlyph
{	
public:
	VTube2() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_tube_2(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_tube_2(cr, in_axes);
	};
};

///48. Труба овоидальная
class VTube3 : public VObjectGlyph
{	
public:
	VTube3() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_tube_3(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_tube_3(cr, in_axes);
	};
};

///49. Труба прямоугольная ж/б
class VTube4 : public VObjectGlyph
{	
public:
	VTube4() {};
	extents_t DrawProfile(cairo_t * cr, VAxesPtr in_axes)
	{				
		return draw_tube_4(cr, in_axes);		
	};
	void DrawSituation(cairo_t * cr, VAxesPtr in_axes)
	{		
		situation_draw_tube_4(cr, in_axes);
	};
};

//Фабрика объектов
static VObjectGlyphPtr GetObjectGlyph(VDataProviderPtr in_data, size_t in_index)
{	
	uint64_t in_type=in_data->GetFlags(in_index);
	double in_length=in_data->GetSizeX(in_index);
	switch (hi_word(in_type))
	{		
		case station_0:
			return VObjectGlyphPtr(new VStation0(static_cast<side_t>(low_word(in_type))));
		case station_1:
			return VObjectGlyphPtr(new VStation1(static_cast<side_t>(low_word(in_type))));
		case station_2:
			return VObjectGlyphPtr(new VStation2(static_cast<side_t>(low_word(in_type))));
		case station_3:
			return VObjectGlyphPtr(new VStation3(static_cast<side_t>(low_word(in_type))));
		case station_4:
			return VObjectGlyphPtr(new VStation4(static_cast<side_t>(low_word(in_type))));
		case station_5:
			return VObjectGlyphPtr(new VStation5(static_cast<side_t>(low_word(in_type))));
		case station_6:
			return VObjectGlyphPtr(new VStation6(static_cast<side_t>(low_word(in_type))));
		case station_7:
			return VObjectGlyphPtr(new VStation7(static_cast<side_t>(low_word(in_type))));
		case switch_1:
			return VObjectGlyphPtr(new VSwitch1());
		case switch_2:
			return VObjectGlyphPtr(new VSwitch2());
		case switch_3:
			return VObjectGlyphPtr(new VSwitch3());
		case switch_4:
			return VObjectGlyphPtr(new VSwitch4());
		case switch_5:
			return VObjectGlyphPtr(new VSwitch5());
		case switch_6:
			return VObjectGlyphPtr(new VSwitch6());
		case switch_7:
			return VObjectGlyphPtr(new VSwitch7());
		case switch_8:
			return VObjectGlyphPtr(new VSwitch8());
		case sign_1:
			return VObjectGlyphPtr(new VSign1(static_cast<side_t>(low_word(in_type))));
		case sign_2:
			return VObjectGlyphPtr(new VSign2(static_cast<side_t>(low_word(in_type))));
		case sign_3:
			return VObjectGlyphPtr(new VSign3(static_cast<side_t>(low_word(in_type))));
		case sign_4:
			return VObjectGlyphPtr(new VSign4(static_cast<side_t>(low_word(in_type))));
		case sign_5:
			return VObjectGlyphPtr(new VSign5(static_cast<side_t>(low_word(in_type))));
		case building_1:
			return VObjectGlyphPtr(new VBuilding1(static_cast<side_t>(low_word(in_type))));
		case building_2:
			return VObjectGlyphPtr(new VBuilding2(static_cast<side_t>(low_word(in_type))));
		case building_3:
			return VObjectGlyphPtr(new VBuilding3(static_cast<side_t>(low_word(in_type))));
		case building_4:
			return VObjectGlyphPtr(new VBuilding4(static_cast<side_t>(low_word(in_type))));
		case izostyk_1:
			return VObjectGlyphPtr(new VIzostyk1());
		case izostyk_2:
			return VObjectGlyphPtr(new VIzostyk2());
		case stolbik_1:
			return VObjectGlyphPtr(new VStolbik1(static_cast<side_t>(low_word(in_type))));
		case stolbik_2:
			return VObjectGlyphPtr(new VStolbik2(static_cast<side_t>(low_word(in_type))));
		case dead_end_1:
			return VObjectGlyphPtr(new VDeadEnd1());
		case dead_end_2:
			return VObjectGlyphPtr(new VDeadEnd2());
		case dead_end_3:
			return VObjectGlyphPtr(new VDeadEnd3());
		case bridge_1:
			return VObjectGlyphPtr(new VBridge1(in_length));
		case bridge_2:
			return VObjectGlyphPtr(new VBridge2(in_length));
		case bridge_3:
			return VObjectGlyphPtr(new VBridge3(in_length));
		case bridge_4:
			return VObjectGlyphPtr(new VBridge4(in_length));
		case brake:
			return VObjectGlyphPtr(new VBrake());
		case crossing_1:
			return VObjectGlyphPtr(new VCrossing1());
		case crossing_2:
			return VObjectGlyphPtr(new VCrossing2(static_cast<side_t>(low_word(in_type))));
		case crossing_3:
			return VObjectGlyphPtr(new VCrossing3());
		case crossing_4:
			return VObjectGlyphPtr(new VCrossing4(static_cast<side_t>(low_word(in_type))));
		case tubing:
			return VObjectGlyphPtr(new VTubing());
		case foot_bridge:
			return VObjectGlyphPtr(new VFootBridge());
		case overpass:
			return VObjectGlyphPtr(new VOverpass());
		case underpass:
			return VObjectGlyphPtr(new VUnderpass());
		case tube_1:
			return VObjectGlyphPtr(new VTube1());
		case tube_2:
			return VObjectGlyphPtr(new VTube2());
		case tube_3:
			return VObjectGlyphPtr(new VTube3());
		case tube_4:
			return VObjectGlyphPtr(new VTube4());
		default:
			return VObjectGlyphPtr(new VDefaultGlyph());
	}
}


