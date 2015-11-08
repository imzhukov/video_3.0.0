#pragma once

#include "VDrawing.h"
#include "axes.h"
#include <boost/shared_ptr.hpp>


//������� ��������� ��������
struct extents_t
{
	double x1;
	double y1;
	double x2;
	double y2;
};

//���������, ��� ����� ������� � ������������ ���-54/26
/** ��� ������� �����������*/
#define station_0 1
/** ��� ������� 1 �����*/
#define station_1 2
/** ��� ������� 2 �����*/
#define station_2 3
/** ��� ������� 3 �����*/
#define station_3 4
/** ��� ������� 4 �����*/
#define station_4 5
/** ��� ������� 5 �����*/
#define station_5 6
/** �������*/
#define station_6 7
/** ������������ �����*/
#define station_7 8
/** ���������� ������� ������������ ������*/
#define switch_1 9
/** ���������� ������� ������������ ��������*/
#define switch_2 10
/** ���������� ������� ����� ������*/
#define switch_3 11
/** ���������� ������� ����� ��������*/
#define switch_4 12
/** ���������� ������� ������ ������*/
#define switch_5 13
/** ���������� ������� ������ ��������*/
#define switch_6 14
/** ���������� ������� �����������*/
#define switch_7 15
/** ������������ �������*/
#define switch_8 16
/** �������� ������������*/
#define sign_1 17
/** �������� ������������*/
#define sign_2 18
/** �������� �����������*/
#define sign_3 19
/** �������� ��������������*/
#define sign_4 20
/** �������� ������������*/
#define sign_5 21
/** �����*/
#define building_1 22
/** ����-����*/
#define building_2 23
/** ����� ���*/
#define building_3 24
/** �������*/
#define building_4 25
/** ����������� ����*/
#define izostyk_1 26
/** ������������ ����������� ����*/
#define izostyk_2 27
/** ���������� �������*/
#define stolbik_1 28
/** ������������� ���������� �������*/
#define stolbik_2 29
/** ����� ���� ��� �����*/
#define dead_end_1 30
/** ����� ���� � ������*/
#define dead_end_2 31
/** ����� ���� � ������ � ���������� �������*/
#define dead_end_3 32
/** ���� ����������*/
#define bridge_1 33
/** ���� ��������������*/
#define bridge_2 34
/** ���� ������������� (��������) � ����� ������*/
#define bridge_3 35
/** ���� ������������� (��������) � ����� �������*/
#define bridge_4 36
/** ������/����� ���������� ����������*/
#define brake 37
/** ������� ������������ ���������*/
#define crossing_1 38
/** ������� ���������� ���������*/
#define crossing_2 39
/** ������� ������������ �������*/
#define crossing_3 40
/** ������� ���������� �������*/
#define crossing_4 41
/** �����������*/
#define tubing 42
/** ���������� ����*/
#define foot_bridge 43
/** ���������� ��� �/�*/
#define overpass 44
/** ���������� ��� �/�*/
#define underpass 45
/** ����� ������� �/�*/
#define tube_1 46
/** ����� �������������*/
#define tube_2 47
/** ����� �����������*/
#define tube_3 48
/** ����� �������������*/
#define tube_4 49
/** ����������� ����*/
#define crane_way 50
/** ��������*/
#define viaduct 51
/** ����� ���������������*/
#define ptl 52
/** ����� �����*/
#define communication_line 53
/** �������*/
#define concourse 54
/** �������*/
#define gallery 55
/** ��������� �������*/
#define platform_1 56
/** ��������� ������*/
#define platform_2 57
/** ������������������ ������*/
#define stop_block_1 58
/** ���� ��������� ������������*/
#define stop_block_2 59
/** ����� ����� �������� (���������)*/
#define hump_1 60
/** �����*/
#define hump_2 61
/** ���������� ������*/
#define gate 62
/** ������� �������*/
#define border_1 63
/** ���� ��������*/
#define weightbridge_1 64
/** ���� �������*/
#define weightbridge_2 65
/** �������� �����������*/
#define rail_brake 66
/** ���������� �������*/
#define instrument_column 67
/** ���������*/
#define guide_1 68
/** ������� ���� �������� ����������� (����������� ����)*/
#define border_2 69
/** ��������� �������� �������*/
#define guide_2 70
/** ����������� ������� ���� �������*/
#define wheel_counter 71
/** ����� �/�*/
#define pier_1 72
/** ����� �/� �������*/
#define pier_2 73
/** ����� ������������� ��������, ����� ���������*/
#define pier_3 74


/** ������������ �����*/
#define kms_1 101
/** ��������������� ������*/
#define default_1 102
/** �� ����������*/
#define do_not_draw 100

//-------------------- ������ �������� � ������������ � ���-54/26 -------------------------------

/// ������� � �������
void show_object_text(cairo_t * cr, VAxesPtr in_axes, const std::wstring& label, bool in_show_name, bool in_show_coord);


/// ������������ �����
extents_t draw_km(cairo_t * cr, VAxesPtr in_axes, side_t in_side);

///0. �� ���������
extents_t draw_default(cairo_t * cr, VAxesPtr in_axes);
///1. ������� (�����������)
extents_t draw_station_0(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///2. ������� (������ �����)
extents_t draw_station_1(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///3. ������� (������ �����)
extents_t draw_station_2(cairo_t * cr, VAxesPtr in_axes);
///4. ������� (������ �����)
extents_t draw_station_3(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///5. ������� (��������� �����)
extents_t draw_station_4(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///6. ������� (����� �����)
extents_t draw_station_5(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///7. ������� (�������)
extents_t draw_station_6(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///8. ������������ �����
extents_t draw_station_7(cairo_t * cr, VAxesPtr in_axes);

///9. ���������� ������� ������������ (����.)
extents_t draw_switch_1(cairo_t * cr, VAxesPtr in_axes);
///10. ���������� ������� ������������ (������.)
extents_t draw_switch_2(cairo_t * cr, VAxesPtr in_axes);
///11. ���������� ������� (���./����.)
extents_t draw_switch_3(cairo_t * cr, VAxesPtr in_axes);
///12. ���������� ������� (���/������.)
extents_t draw_switch_4(cairo_t * cr, VAxesPtr in_axes);
///13. ���������� ������� (����./����.)
extents_t draw_switch_5(cairo_t * cr, VAxesPtr in_axes);
///14. ���������� ������� (����./������.)
extents_t draw_switch_6(cairo_t * cr, VAxesPtr in_axes);
///15. ���������� ������� ������������
extents_t draw_switch_7(cairo_t * cr, VAxesPtr in_axes);
///16. ������������ �������
extents_t draw_switch_8(cairo_t * cr, VAxesPtr in_axes);

///17. �������� ������������
extents_t draw_sign_1(cairo_t * cr, VAxesPtr in_axes);
///18. �������� ������������
extents_t draw_sign_2(cairo_t * cr, VAxesPtr in_axes);
///19. �������� ������������
extents_t draw_sign_3(cairo_t * cr, VAxesPtr in_axes);
///20. �������� ���������������
extents_t draw_sign_4(cairo_t * cr, VAxesPtr in_axes);
///21. �������� ������������
extents_t draw_sign_5(cairo_t * cr, VAxesPtr in_axes);

///22. �����
extents_t draw_building_1(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///23. ����-����
extents_t draw_building_2(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///24. ����� ���
extents_t draw_building_3(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///25. �������
extents_t draw_building_4(cairo_t * cr, VAxesPtr in_axes, side_t in_side);

///26. �������
extents_t draw_izostyk_1(cairo_t * cr, VAxesPtr in_axes);
///27. ������������ �������
extents_t draw_izostyk_2(cairo_t * cr, VAxesPtr in_axes);

///28. ���������� �������
extents_t draw_stolbik_1(cairo_t * cr, VAxesPtr in_axes);
///29. ������������� ���������� �������
extents_t draw_stolbik_2(cairo_t * cr, VAxesPtr in_axes);

///30. ����� ���� ��� �����
extents_t draw_dead_end_1(cairo_t * cr, VAxesPtr in_axes);
///31. ����� ���� � ������
extents_t draw_dead_end_2(cairo_t * cr, VAxesPtr in_axes);
///32. ����� ���� � ������ � ���������� �������
extents_t draw_dead_end_3(cairo_t * cr, VAxesPtr in_axes);

///33. ���� ����������
extents_t draw_bridge_1(cairo_t * cr, VAxesPtr in_axes, double in_length=0);
///34. ���� ��������������
extents_t draw_bridge_2(cairo_t * cr, VAxesPtr in_axes, double in_length=0);
///35. ���� ������������� (��������) � ����� ������
extents_t draw_bridge_3(cairo_t * cr, VAxesPtr in_axes, double in_length=0);
/// ���� ������������� (��������) � ����� �������
extents_t draw_bridge_4(cairo_t * cr, VAxesPtr in_axes, double in_length=0);

///37. ������/����� ���������� ����������*/
extents_t draw_brake(cairo_t * cr, VAxesPtr in_axes);

///38. ������� ������������ ���������
extents_t draw_crossing_1(cairo_t * cr, VAxesPtr in_axes);
///39. ������� ���������� ���������
extents_t draw_crossing_2(cairo_t * cr, VAxesPtr in_axes, side_t in_side);
///40. ������� ������������ �������
extents_t draw_crossing_3(cairo_t * cr, VAxesPtr in_axes);
///41. ������� ���������� �������
extents_t draw_crossing_4(cairo_t * cr, VAxesPtr in_axes, side_t in_side);

///42. �����������
extents_t draw_tubing(cairo_t * cr, VAxesPtr in_axes);
///43. ���������� ����
extents_t draw_foot_bridge(cairo_t * cr, VAxesPtr in_axes);
///44. ���������� ��� �/�
extents_t draw_overpass(cairo_t * cr, VAxesPtr in_axes);
///45. ���������� ��� �/�
extents_t draw_underpass(cairo_t * cr, VAxesPtr in_axes);

///46. ����� ������� �/�
extents_t draw_tube_1(cairo_t * cr, VAxesPtr in_axes);
///47. ����� ��������. ������.
extents_t draw_tube_2(cairo_t * cr, VAxesPtr in_axes);
///48. ����� �����������
extents_t draw_tube_3(cairo_t * cr, VAxesPtr in_axes);
///49. ����� ������������� �/�
extents_t draw_tube_4(cairo_t * cr, VAxesPtr in_axes);


// -------------------------------  ��������� �������� �� �������� --------------------------------------------

///1-8. ������� (�����)
void situation_draw_station_0(cairo_t * cr, VAxesPtr in_axes);

///9. ���������� ������� ������������ (����.)
void situation_draw_switch_1(cairo_t * cr, VAxesPtr in_axes);
///10. ���������� ������� ������������ (������.)
void situation_draw_switch_2(cairo_t * cr, VAxesPtr in_axes);
///11. ���������� ������� (���./����.)
void situation_draw_switch_3(cairo_t * cr, VAxesPtr in_axes);
///12. ���������� ������� (���/������.)
void situation_draw_switch_4(cairo_t * cr, VAxesPtr in_axes);
///13. ���������� ������� (����./����.)
void situation_draw_switch_5(cairo_t * cr, VAxesPtr in_axes);
///14. ���������� ������� (����./������.)
void situation_draw_switch_6(cairo_t * cr, VAxesPtr in_axes);
///15. ���������� ������� ������������
void situation_draw_switch_7(cairo_t * cr, VAxesPtr in_axes);
///16. ������������ �������
void situation_draw_switch_8(cairo_t * cr, VAxesPtr in_axes);

///17. �������� ������������
void situation_draw_sign_1(cairo_t * cr, VAxesPtr in_axes);
///18. �������� ������������
void situation_draw_sign_2(cairo_t * cr, VAxesPtr in_axes);
///19. �������� ������������
void situation_draw_sign_3(cairo_t * cr, VAxesPtr in_axes);
///20. �������� ���������������
void situation_draw_sign_4(cairo_t * cr, VAxesPtr in_axes);
///21. �������� ������������
void situation_draw_sign_5(cairo_t * cr, VAxesPtr in_axes);

///22-25. ����� / ����-���� / ����� ��� / �������
void situation_draw_building_1(cairo_t * cr, VAxesPtr in_axes);

///26. �������
void situation_draw_izostyk_1(cairo_t * cr, VAxesPtr in_axes);
///27. ������������ �������
void situation_draw_izostyk_2(cairo_t * cr, VAxesPtr in_axes);

///28. ���������� �������
void situation_draw_stolbik_1(cairo_t * cr, VAxesPtr in_axes);
///29. ������������� ���������� �������
void situation_draw_stolbik_2(cairo_t * cr, VAxesPtr in_axes);

///30. ����� ���� ��� �����
void situation_draw_dead_end_1(cairo_t * cr, VAxesPtr in_axes);
///31. ����� ���� � ������
void situation_draw_dead_end_2(cairo_t * cr, VAxesPtr in_axes);
///32. ����� ���� � ������ � ���������� �������
void situation_draw_dead_end_3(cairo_t * cr, VAxesPtr in_axes);

///33. ���� ����������
void situation_draw_bridge_1(cairo_t * cr, VAxesPtr in_axes);
///34. ���� ��������������
void situation_draw_bridge_2(cairo_t * cr, VAxesPtr in_axes);
///35. ���� ������������� (��������) � ����� ������
void situation_draw_bridge_3(cairo_t * cr, VAxesPtr in_axes);
///36. ���� ������������� (��������) � ����� �������
void situation_draw_bridge_4(cairo_t * cr, VAxesPtr in_axes);

///37. ������/����� ���������� ����������*/
void situation_draw_brake(cairo_t * cr, VAxesPtr in_axes);

///38. ������� ������������ ���������
void situation_draw_crossing_1(cairo_t * cr, VAxesPtr in_axes);
///39. ������� ���������� ���������
void situation_draw_crossing_2(cairo_t * cr, VAxesPtr in_axes);
///40. ������� ������������ �������
void situation_draw_crossing_3(cairo_t * cr, VAxesPtr in_axes);
///41. ������� ���������� �������
void situation_draw_crossing_4(cairo_t * cr, VAxesPtr in_axes);

///42. �����������
void situation_draw_tubing(cairo_t * cr, VAxesPtr in_axes);
///43. ���������� ����
void situation_draw_foot_bridge(cairo_t * cr, VAxesPtr in_axes);
///44. ���������� ��� �/�
void situation_draw_overpass(cairo_t * cr, VAxesPtr in_axes);
///45. ���������� ��� �/�
void situation_draw_underpass(cairo_t * cr, VAxesPtr in_axes);

///46. ����� ������� �/�
void situation_draw_tube_1(cairo_t * cr, VAxesPtr in_axes);
///47. ����� ��������. ������.
void situation_draw_tube_2(cairo_t * cr, VAxesPtr in_axes);
///48. ����� �����������
void situation_draw_tube_3(cairo_t * cr, VAxesPtr in_axes);
///49. ����� ������������� �/�
void situation_draw_tube_4(cairo_t * cr, VAxesPtr in_axes);

//---------------------------------- ��������� �������� � ����� ���� -------------------------------

///���������� �������
void kvlp_draw_switch_0(cairo_t * cr, VAxesPtr in_axes, double in_length);
///���������� ������� �� ����
void kvlp_draw_switch_1(cairo_t * cr, VAxesPtr in_axes, double in_length);
///���������� ������� ������ ����
void kvlp_draw_switch_2(cairo_t * cr, VAxesPtr in_axes, double in_length);



//----------------------------- ����� ����������� ������� ------------------------------------------


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

///1. ������� (�����������)
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

///2. ������� (������ �����)
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

///3. ������� (������ �����)
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

///4. ������� (������ �����)
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

///5. ������� (��������� �����)
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

///6. ������� (����� �����)
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

///7. ������� (�������)
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

///8. ������������ �����
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

///9. ���������� ������� ������������ (����.)
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

///10. ���������� ������� ������������ (������.)
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

///11. ���������� ������� (���./����.)
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

///12. ���������� ������� (���/������.)
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

///13. ���������� ������� (����./����.)
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

///14. ���������� ������� (����./������.)
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

///15. ���������� ������� ������������
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

///16. ������������ �������
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

///17. �������� ������������
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

///18. �������� ������������
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

///19. �������� ������������
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

///20. �������� ���������������
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

///21. �������� ������������
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

///22. �����
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

///23. ����-����
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

///24. ����� ���
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

///25. �������
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

/// ��������� "���������� ����� ���������"
DECLARE_OPTION(bool, STATION_PROFILE_SHOW_IZOSTYK_NAME, false, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())

///26. �������
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

///27. ������������ �������
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

/// ��������� "���������� ����� ���������� ���������"
DECLARE_OPTION(bool, STATION_PROFILE_SHOW_STOLBIK_NAME, false, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())

//28. ���������� �������
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

//29. ������������� ���������� �������
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

///30. ����� ���� ��� �����
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

///31. ����� ���� � ������
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

///32. ����� ���� � ������ � ���������� �������
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

///33. ���� ����������
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

///34. ���� ��������������
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

///35. ���� ������������� (��������) � ����� ������
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

///36. ���� ������������� (��������) � ����� �������
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

///37. ������/����� ���������� ����������
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

///38. ������� ������������ ���������
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

///39. ������� ���������� ���������
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

///40. ������� ������������ �������
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

///41. ������� ���������� �������
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

///42. �����������
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

///43. ���������� ����
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

///44. ���������� ��� �/�
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

///45. ���������� ��� �/�
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

///46. ����� ������� �/�
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

///47. ����� ��������. ������.
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

///48. ����� �����������
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

///49. ����� ������������� �/�
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

//������� ��������
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


