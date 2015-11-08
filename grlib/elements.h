#pragma once

#include "providers.h"
#include "axes.h"

/// Расположение текста FixME!!! уточнить термины, добавить расположение по левому и правому краю области
enum TEXT_POSITION {TP_OVER=0, TP_UNDER=1, TP_LEFT=2, TP_RIGHT=3, 
	TP_AREA_BOTTOM_OVER=4, TP_AREA_BOTTOM_UNDER=5,
	TP_AREA_TOP_OVER=6, TP_AREA_TOP_UNDER=7};

/// Ориентация текста
enum TEXT_ORIENTATION {TO_HORIZONTAL=0, TO_VERTICAL=1};

/// Выводит текст
void CairoShowText(cairo_t * cr, VAxesPtr axes, double x, double y,
	const std::wstring& in_text, TEXT_ORIENTATION to=TO_HORIZONTAL, TEXT_POSITION tp=TP_LEFT, double in_text_space=text_space);

/** Элемент с фоном*/
class VBackgroundElement : public VElement
{		
private:
	/** Цвет фона*/
	VColor fill_color;
public:
	/** Конструктор*/
	VBackgroundElement(const VColor& in_fill_color);
	/** Рисование*/
	virtual void Paint(cairo_t * cr);
};


/// Элемент - текст
class VTextGraph : public VElement
{
private:
	/// Источник данных
	VDataProviderPtr data;
	/// Оси
	VAxesPtr axes;		
	/// Ориентация текста
	TEXT_ORIENTATION to;
	/// Размещение текста
	TEXT_POSITION tp;

public:
	/// Конструктор
	VTextGraph(VDataProviderPtr in_data, VAxesPtr in_axes, TEXT_ORIENTATION in_to, TEXT_POSITION in_tp);
	/// Рисование
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VTextGraph> VTextGraphPtr;


/** График типа line*/
class VLineGraph : public VElement
{			
	private:

		/** Структура для  VLineGraph */
		struct point_group
		{
			std::vector<point_t> points;
			VColor color;
		};

		/** Данные*/
		VDataProviderPtr data;		
		/** Оси*/
		VAxesPtr axes;		
		/** Цвет линии*/
		VColor color;		
		/** Цвет линии*/
		VColor fill_color;		
		/** Толщина линии*/
		double width;
		/** Признак обрезания по краям области*/
		int clip;		

		/** Находит точки, в которой нужно сделать "скачок"*/
		std::vector<point_t> GetRelativeXY(double in_x1, double in_y1, double in_x2, double in_y2) const;	

	public:
		/** Конструктор*/
		VLineGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, const VColor& in_fill_color, double in_width=default_line_width, int in_clip=CLIP_AREA);
		/** Риcование*/	
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VLineGraph> VLineGraphPtr;

/** График типа barchart*/
class VBarGraph : public VElement
{		
	private:
		/** Данные*/
		VDataProviderPtr data;		
		/** Оси*/
		VAxesPtr axes;		
		/** Цвет линии*/
		VColor color;
		/** Цвет заливки*/
		VColor fill_color;		
		/** Толщина линии*/
		double width;		
		/** Признак обрезания по краям области*/
		int clip;		
		/** Показывать метки*/
		bool show_labels;		
	public:
		/** Конструктор*/
		VBarGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  
			const VColor& in_fill_color, double in_width=default_line_width, int in_clip=CLIP_AREA, 
			bool in_show_labels=false);
		/** Риcование*/	
		virtual void Paint(cairo_t * cr);
};

/** График типа square*/
class VSquareGraph : public VElement
{		
	private:
		/** Данные*/
		VDataProviderPtr data;		
		/** Оси*/
		VAxesPtr axes;		
		/** Цвет линии*/
		VColor color;
		/** Цвет заливки*/
		VColor fill_color;		
		/** Толщина линии*/
		double width;		
		/** Признак обрезания по краям области*/
		int clip;		
		/** Показывать метки*/
		bool show_labels;		
	public:
		/** Конструктор*/
		VSquareGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  
			const VColor& in_fill_color, double in_width=default_line_width, int in_clip=CLIP_AREA, bool in_show_labels=false);
		/** Риcование*/	
		virtual void Paint(cairo_t * cr);
};

/** График типа area*/
class VAreaGraph : public VElement
{		
	private:
		/** Данные*/
		VDataProviderPtr data;		
		/** Оси*/
		VAxesPtr axes;		
		/** Цвет линии*/
		VColor color;
		/** Цвет заливки*/
		VColor fill_color;		
		/** Толщина линии*/
		double width;		
		/** Признак обрезания по краям области*/
		int clip;		
		/** Показывать метки*/
		bool show_labels;		
	public:
		/** Конструктор*/
		VAreaGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  
			const VColor& in_fill_color, double in_width=default_line_width, int in_clip=CLIP_AREA, bool in_show_labels=false);
		/** Риcование*/	
		virtual void Paint(cairo_t * cr);
};

/** График типа stick*/
class VStickGraph : public VElement
{		
	private:
		/** Данные*/
		VDataProviderPtr data;		
		/** Оси*/
		VAxesPtr axes;		
		/** Цвет линии*/
		VColor color;
		/** Цвет заливки*/
		VColor fill_color;		
		/** Толщина линии*/
		double width;		
		/** Признак обрезания по краям области*/
		int clip;		
		/** Показывать значения*/
		bool show_labels;
	public:
		/** Конструктор*/
		VStickGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, 
			double in_width=default_line_width, int in_clip=CLIP_AREA, bool in_show_labels=true);
		/** Риcование*/	
		virtual void Paint(cairo_t * cr);		
};
typedef boost::shared_ptr<VStickGraph> VStickGraphPtr;

/** График типа stick*/
class VCoordPointGraph : public VElement
{		
	private:
		/** Данные*/
		VDataProviderPtr data;		
		/** Оси*/
		VAxesPtr axes;		
		/** Цвет линии*/
		VColor color;
		/** Цвет заливки*/
		VColor fill_color;		
		/** Толщина линии*/
		double width;		
		/** Признак обрезания по краям области*/
		int clip;		
	public:
		/** Конструктор*/
		VCoordPointGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, 
			double in_width=default_line_width, int in_clip=CLIP_AREA);
		/** Риcование*/	
		virtual void Paint(cairo_t * cr);
		/** Обработка события*/
		bool ProcessEvent(const VEvent& in_event);
};

/** Стиль точки*/
enum point_style_t
{
	/** Кружок*/
	POINT_STYLE_CIRCLE=0,
	/** Флажок*/
	POINT_STYLE_FLAG=1
};

/** Точка - в будущем можно сделать этот класс базовым, для точек разного вида*/
class VPointElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Система координат*/
	VAxesPtr axes;
	/** Индекс точки в массиве данных*/
	size_t index;
	/** Цвет точки*/
	VColor color;
	/** Цвет заливки точки*/
	VColor fill_color;
	/** Толщина линии*/
	double width;
	/** Выводить ли точку за пределами области*/
	int clip;	
	/** Границы элемента*/
	double ext_left;
	/** Границы элемента*/
	double ext_top;
	/** Границы элемента*/
	double ext_right;
	/** Границы элемента*/
	double ext_bottom;
	/** Мышь над элементом*/
	bool hit;	
	/** Признак того что мышь пересекла границу элемента*/	
	change_calculator<bool> hit_changed;
	/** Показывать метку*/
	bool show_label;
	/** Ориентация текста*/
	TEXT_ORIENTATION to;
	/** Положение текста*/
	TEXT_POSITION tp;
	/** Стиль 0 - кружок, 1 - флаг*/
	point_style_t style;
	/** Драг*/
	bool drag;
	/** Смещение по x при перетаскивании*/
	double drag_shift_x;
	/** Смещение по y при перетаскивании*/
	double drag_shift_y;
public:
	/** Конструктор*/
	VPointElement(VDataProviderPtr in_data, VAxesPtr in_axes, size_t in_index, const VColor& in_color,  
		const VColor& in_fill_color, int in_clip=CLIP_AREA, bool in_show_label=true, 
		point_style_t in_style=POINT_STYLE_CIRCLE, TEXT_ORIENTATION to=TO_HORIZONTAL, TEXT_POSITION tp=TP_LEFT);
	/** Рисование*/
	virtual void Paint(cairo_t * cr);	
	/** Обработчик HitTest*/
	bool HitTest(double in_x, double in_y);		
	/** Обработка события*/
	bool ProcessEvent(const VEvent& in_event);
};	
typedef boost::shared_ptr<VPointElement> VPointElementPtr;

/** График типа points*/
class VPointGraph : public VElement
{	
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет графика*/
	VColor color;
	/** Цвет заливки*/
	VColor fill_color;
	/** Выводить ли точку за пределами области*/
	int clip;	
	/** Стиль отображения точек*/
	point_style_t style;
	/** Список точек*/
	std::vector<VPointElementPtr> points;
	/** Итератор для списка точек*/
	typedef std::vector<VPointElementPtr>::iterator point_itr; 	
	/** Показывать метки*/
	bool show_labels;	
	/** Ориентация текста*/
	TEXT_ORIENTATION to;
	/** Положение текста*/
	TEXT_POSITION tp;
public:
	/** Конструктор*/
	VPointGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, 
		int in_clip=CLIP_AREA, bool in_show_labels=true, point_style_t in_style=POINT_STYLE_CIRCLE,
		TEXT_ORIENTATION in_to=TO_VERTICAL, TEXT_POSITION in_tp=TP_UNDER);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
	/** Обработка hit-test*/
	bool HitTest(double in_x, double in_y);
	/** Обработка события*/
	virtual bool ProcessEvent(const VEvent& in_event);	
};
typedef boost::shared_ptr<VPointGraph> VPointGraphPtr;

/** График типа POINT_STICK*/
class VPointStickGraph : public VElement
{		
	private:
		/// Точки
		VPointGraphPtr points;
		/// Палки
		VStickGraphPtr sticks;

	public:
		/// Конструктор
		VPointStickGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, 
			double in_width=default_line_width, int in_clip=CLIP_AREA, bool in_show_labels=true);
		/// Риcование	
		virtual void Paint(cairo_t * cr);		
};
typedef boost::shared_ptr<VPointStickGraph> VPointStickGraphPtr;


/// Точки соединенные линиями
class VPointLineGraph : public VElement
{		
	private:
		/// Точки
		VPointGraphPtr points;
		/// Линии
		VLineGraphPtr lines;
	public:
		/// Конструктор
		VPointLineGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, 
			double in_width=default_line_width, int in_clip=CLIP_AREA, bool in_show_labels=true, 
			point_style_t in_point_style=POINT_STYLE_CIRCLE);
		/// Риcование	
		virtual void Paint(cairo_t * cr);
		/// Обработка hit-test
		bool HitTest(double in_x, double in_y);
		/// Обработка события
		virtual bool ProcessEvent(const VEvent& in_event);	
};
typedef boost::shared_ptr<VPointLineGraph> VPointLineGraphPtr;

/** Элемент - объекты на графике профиля*/
class VProfileObjectsElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;
	/** Цвет заливки*/
	VColor fill_color;
	/** Выводить график за пределами области*/
	int clip;		
public:
	/** Конструктор*/
	VProfileObjectsElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, int in_clip=CLIP_LEFT | CLIP_RIGHT);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VProfileObjectsElement> VProfileObjectsElementPtr;

/** Элемент - объекты на ситуации*/
class VSituationObjectsElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;
	/** Цвет заливки*/
	VColor fill_color;
	/** Выводить график за пределами области*/
	int clip;		
public:
	/** Конструктор*/
	VSituationObjectsElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, int in_clip=CLIP_AREA);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VSituationObjectsElement> VSituationObjectsElementPtr;

/** Отступления на ленте КВЛП*/
class VKVLPOverstepElement : public VElement
{		
	private:
		/** Данные*/
		VDataProviderPtr data;		
		/** Оси*/
		VAxesPtr axes;		
		/** Цвет линии*/
		VColor color;
		/** Цвет заливки*/
		VColor fill_color;		
		/** Толщина линии*/
		double width;		
		/** Признак обрезания по краям области*/
		int clip;				
	public:
		/** Конструктор*/
		VKVLPOverstepElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  
			const VColor& in_fill_color, double in_width=default_line_width, int in_clip=CLIP_HORIZONTAL);
		/** Риcование*/	
		virtual void Paint(cairo_t * cr);
};

/// Отступления Анп на ленте КВЛП
class VKVLPAnpElement : public VElement
{
private:
	/** Данные*/
	VDataProviderPtr data;		
	/** Оси*/
	VAxesPtr axes;			
public:
	/** Конструктор*/
	VKVLPAnpElement(VDataProviderPtr in_data, VAxesPtr in_axes);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};

/// Отступления Пси на ленте КВЛП
class VKVLPPsiElement : public VElement
{
private:
	/** Данные*/
	VDataProviderPtr data;		
	/** Оси*/
	VAxesPtr axes;			
public:
	/** Конструктор*/
	VKVLPPsiElement(VDataProviderPtr in_data, VAxesPtr in_axes);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};

/// Отступления Ошк на ленте КВЛП
class VKVLPOtvodWidthElement : public VElement
{
private:
	/** Данные*/
	VDataProviderPtr data;		
	/** Оси*/
	VAxesPtr axes;			
public:
	/** Конструктор*/
	VKVLPOtvodWidthElement(VDataProviderPtr in_data, VAxesPtr in_axes);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};


/** Элемент - стрелки на ленте КВЛП*/
class VKVLPSwitchesElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;
	/** Цвет заливки*/
	VColor fill_color;
	/** Толщина линии*/
	double width;		
	/** Выводить график за пределами области*/
	int clip;		
public:
	/** Конструктор*/
	VKVLPSwitchesElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, const VColor& in_fill_color, 
		double in_width=default_line_width, int in_clip=CLIP_AREA);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPSwitchesElement> VKVLPSwitchesElementPtr;

/** Элемент - участки стрелок на ленте КВЛП*/
class VKVLPSwitchSectionsElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;
	/** Цвет заливки*/
	VColor fill_color;
	/** Толщина линии*/
	double width;		
	/** Выводить график за пределами области*/
	int clip;		
public:
	/** Конструктор*/
	VKVLPSwitchSectionsElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, const VColor& in_fill_color, 
		double in_width=default_line_width, int in_clip=CLIP_AREA);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPSwitchSectionsElement> VKVLPSwitchSectionsElementPtr;

/** Элемент - достоверность паспортных данных*/ 
class VKVLPPassportStatusElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;		
	/** Выводить график за пределами области*/
	int clip;	
public:
	/** Конструктор*/
	VKVLPPassportStatusElement(VDataProviderPtr in_data, VAxesPtr in_axes, int in_clip=CLIP_AREA);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPPassportStatusElement> VKVLPPassportStatusElementPtr;

/** Элемент - мосты на ленте КВЛП*/
class VKVLPBridgeElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;			
public:
	/** Конструктор*/
	VKVLPBridgeElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPBridgeElement> VKVLPBridgeElementPtr;

/** Элемент - тоннели на ленте КВЛП*/
class VKVLPTunnelElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;			
public:
	/** Конструктор*/
	VKVLPTunnelElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPTunnelElement> VKVLPTunnelElementPtr;


/** Элемент - охранная зона моста на ленте КВЛП*/
class VKVLPBridgeAreaElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;			
public:
	/** Конструктор*/
	VKVLPBridgeAreaElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPBridgeAreaElement> VKVLPBridgeAreaElementPtr;

/** Элемент - уравнительный прибор на ленте КВЛП*/
class VKVLPChamferedJointElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;			
public:
	/** Конструктор*/
	VKVLPChamferedJointElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPChamferedJointElement> VKVLPChamferedJointElementPtr;


/** Элемент - переезды на ленте КВЛП*/
class VKVLPCrossingElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;			
public:
	/** Конструктор*/
	VKVLPCrossingElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPCrossingElement> VKVLPCrossingElementPtr;


/** Элемент - тип шпал на ленте КВЛП*/
class VKVLPSpalaElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;
	/** Толщина линии*/
	double width;
	/** Выводить график за пределами области*/
	int clip;	
public:
	/** Конструктор*/
	VKVLPSpalaElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  double in_width=default_line_width, int in_clip=CLIP_AREA);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPSpalaElement> VKVLPSpalaElementPtr;

/** Элемент - рихновочная нить на ленте КВЛП*/
class VKVLPStraightSideElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;
	/** Толщина линии*/
	double width;
public:
	/** Конструктор*/
	VKVLPStraightSideElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, double in_width=default_line_width);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPStraightSideElement> VKVLPStraightSideElementPtr;


/** Элемент - переезды*/
class VCrossingElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;			
public:
	/** Конструктор*/
	VCrossingElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VCrossingElement> VCrossingElementPtr;

/** Элемент - мосты*/
class VBridgeElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;			
public:
	/** Конструктор*/
	VBridgeElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VBridgeElement> VBridgeElementPtr;

/** Элемент - тоннели*/
class VTunnelElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;			
public:
	/** Конструктор*/
	VTunnelElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VTunnelElement> VTunnelElementPtr;



/** Элемент - уклоны*/
class VSlopeGraph : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;	
public:
	/** Конструктор*/
	VSlopeGraph(VDataProviderPtr in_data, VAxesPtr in_axes);
	/** Рисование*/
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VSlopeGraph> VSlopeGraph2Ptr;

/** Граница области*/
enum area_edge_t
{
	AREA_TOP=1,
	AREA_BOTTOM=2,
	AREA_OVER=3,
	AREA_BELOW=4
};

/** Элемент типа - метки на оси X*/
class VXMarkGraph : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Система координат*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;
	/** Толщина линии*/
	double width;
	/** Высота линии*/
	double height;
	/** Граница области*/
	area_edge_t edge;
	/** Показывать текст*/
	bool show_labels;
public:
	/** Конструктор*/
	VXMarkGraph(VDataProviderPtr in_data, VAxesPtr in_axes, VColor in_color, double in_width=default_line_width, double in_height=2.0,
		area_edge_t in_edge=AREA_BOTTOM, bool in_show_labels=true);
	/** Рисование*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VXMarkGraph> VXMarkGraphPtr;

/** Элемент типа - границы на оси X*/
class VXLineGraph : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Система координат*/
	VAxesPtr axes;
	/** Пунктир*/
	VDash dash;
	/** Цвет*/
	VColor color;
	/** Толщина линии*/
	double width;
	/** Граница области*/
	area_edge_t edge;
public:
	/** Конструктор*/
	VXLineGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VDash& in_dash=DASH1, const VColor& in_color=COLOR_HTML_GRAY, 
		double in_width=default_line_width, area_edge_t in_edge=AREA_BOTTOM);
	/** Рисование*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VXLineGraph> VXLineGraphPtr;


/** Элемент типа - участок пути*/
class VActiveWaySectionElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Индекс элемента*/
	size_t index;	
	/** Система координат*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;
	/** Цвет заливки*/
	VColor fill_color;
	/** Толщина линии*/
	double width;	
		
public:
	/** Конструктор*/
	VActiveWaySectionElement(VDataProviderPtr in_data, VAxesPtr in_axes, size_t in_index, VColor in_color, VColor in_fill_color, double in_width=default_line_width);
	/** Рисование*/
	void Paint(cairo_t * cr);
	/** Обработка hit-test*/
	bool HitTest(double in_x, double in_y);
	/** Обработка событий*/
	bool ProcessEvent(const VEvent& in_event);
};
typedef boost::shared_ptr<VActiveWaySectionElement> VActiveWaySectionElementPtr;

/** Элемент типа - участки на оси X*/
class VActiveWaySectionGraph : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;	
	/** Система координат*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;
	/** Цвет заливки*/
	VColor fill_color;
	/** Толщина линии*/
	double width;

	/** Список точек*/
	std::vector<VActiveWaySectionElementPtr> points;
	/** Итератор для списка точек*/
	typedef std::vector<VActiveWaySectionElementPtr>::iterator point_itr; 

	
public:
	/** Конструктор*/
	VActiveWaySectionGraph(VDataProviderPtr in_data, VAxesPtr in_axes, VColor in_color, VColor in_fill_color, double in_width=default_line_width);
	/** Рисование*/
	void Paint(cairo_t * cr);
	/** Обработка событий*/
	bool ProcessEvent(const VEvent& in_event);
};
typedef boost::shared_ptr<VActiveWaySectionGraph> VActiveWaySectionGraphPtr;

/** Элемент типа - участки на оси X*/
class VWaySectionGraph : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Система координат*/
	VAxesPtr axes;
	/** Цвет*/
	VColor color;
	/** Цвет заливки*/
	VColor fill_color;
	/** Толщина линии*/
	double width;
	/** Признак - показывать текст*/
	bool show_labels;

public:
	/** Конструктор*/
	VWaySectionGraph(VDataProviderPtr in_data, VAxesPtr in_axes, VColor in_color, VColor in_fill_color, double in_width=default_line_width, bool in_show_labels=true);
	/** Рисование*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VWaySectionGraph> VWaySectionGraphPtr;


/** Элемент с заголовком графика*/
class VHeaderElement : public VElement
{
	private:				
		/** Указатель на область*/
		VAreaPtr area;
		/** Указатель на устройство*/
		VDrawingDevicePtr device;
		/** Текст заголовка*/
		std::vector<std::wstring> lines;
	public:
		/** Конструктор*/
		VHeaderElement(VAreaPtr in_area, VDrawingDevicePtr in_device);
		/** Рисование*/
		virtual void Paint(cairo_t * cr);
		/** Добавляет строку в заголовок*/
		void AddLine(const std::wstring& in_line);
};
typedef boost::shared_ptr<VHeaderElement> VHeaderElementPtr;

/** Заголовок графика*/
class VGraphHeaderElement : public VElement
{
	private:
		/** Область*/
		VAreaPtr area;
		/** Устройство*/
		VDrawingDevicePtr device;
		/** Данные*/
		VGraphInfoPtr graph_info;
	public:
		/** Конструктор*/
		VGraphHeaderElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VGraphInfoPtr in_graph_info);
		/** Рисование*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VGraphHeaderElement> VGraphHeaderElementPtr;

/** Список файлов*/
class VDataSetsElement : public VElement
{
	private:
		/** Область*/
		VAreaPtr area;
		/** Устройство*/
		VDrawingDevicePtr device;
		/** Данные*/
		VGraphInfoPtr graph_info;
	public:
		/** Конструктор*/
		VDataSetsElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VGraphInfoPtr in_graph_info);
		/** Рисование*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VDataSetsElement> VDataSetsElementPtr;


/** Элемент с координатной сеткой по оси X*/
class VGridLinesXElement : public VElement
{
	private:		
		/** Указатель на систему координат*/
		VAxesPtr axes;
		/** Признак показывать или не показывать текст*/
		bool show_text;
	public:
		/** Конструктор*/
		VGridLinesXElement(VAxesPtr in_axes, bool in_show_text);
		/** Рисование*/
		virtual void Paint(cairo_t * cr);
};

/** Элемент с координатной сеткой по оси Y*/
class VGridLinesYElement : public VElement
{
	private:		
		/** Указатель на систему координат*/
		VAxesPtr axes;
		/** Признак показывать или не показывать текст*/
		bool show_text;
		/** Сторона*/
		side_t side;
	public:
		/** Конструктор*/
		VGridLinesYElement(VAxesPtr in_axes, bool in_show_text, side_t in_side=SD_LEFT);
		/** Рисование*/
		virtual void Paint(cairo_t * cr);
};

/** Элемент показывающий предыдущий километр*/
class VPreviousKmElement : public VElement
{
	private:		
		/** Указатель на систему координат*/
		VAxesPtr axes;		
		/** Указатель на данные*/
		VCoordinatesDataPtr data;		
	public:
		/** Конструктор*/
		VPreviousKmElement(VAxesPtr in_axes, VCoordinatesDataPtr in_data);		
		/** Рисование*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VPreviousKmElement> VPreviousKmElementPtr;

/** Элемент с координатной сеткой по путевой координате (только сетка)*/
class VCoordinatesGridElement : public VElement
{
	private:		
		/** Указатель на систему координат*/
		VAxesPtr axes;		
		/** Указатель на данные*/
		VDataProviderPtr data;		
	public:
		/** Конструктор*/
		VCoordinatesGridElement(VDataProviderPtr in_data, VAxesPtr in_axes);		
		/** Рисование*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VCoordinatesGridElement> VCoordinatesGridElementPtr;

/** Оцифровка путевой координаты*/
class VCoordinatesTextElement : public VElement
{
	private:		
		/** Указатель на систему координат*/
		VAxesPtr axes;		
		/** Указатель на данные*/
		VDataProviderPtr data;		
	public:
		/** Конструктор*/
		VCoordinatesTextElement(VDataProviderPtr in_data, VAxesPtr in_axes);		
		/** Рисование*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VCoordinatesTextElement> VCoordinatesTextElementPtr;

/** Элемент со значками километров*/
class VKmElement : public VElement
{
	private:		
		/** Указатель на систему координат*/
		VAxesPtr axes;		
		/** Указатель на данные*/
		VDataProviderPtr data;
		/** Признак показывать или не показывать текст*/
		bool show_text;
	public:
		/** Конструктор*/
		VKmElement(VDataProviderPtr in_data, VAxesPtr in_axes, bool in_show_text);		
		/** Рисование*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKmElement> VKmElementPtr;

/** Элемент с пикетами*/
class VPkElement : public VElement
{
	private:		
		/** Указатель на систему координат*/
		VAxesPtr axes;		
		/** Указатель на данные*/
		VDataProviderPtr data;
		/** Признак показывать или не показывать текст*/
		bool show_text;
	public:
		/** Конструктор*/
		VPkElement(VDataProviderPtr in_data, VAxesPtr in_axes, bool in_show_text);		
		/** Рисование*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VPkElement> VPkElementPtr;

/** Элемент - линия константа*/
class VConstantLineYElement : public VElement
{
private:
	/** Значение*/
	VDataProviderPtr data;
	/** Указатель на систему координат*/
	VAxesPtr axes;
	/** Пунктир*/
	VDash dash;
	/** Цвет линии*/
	VColor color;	
	/** Толщина линии*/
	double width;
	/** Признак обрезания по краям области*/
	int clip;		
	/** Признак - показывать оцифровку*/
	bool show_labels;
	/** Рисование*/
	virtual void Paint(cairo_t * cr);
public:
	/** Конструктор*/
	VConstantLineYElement(VDataProviderPtr in_data, VAxesPtr in_axes, VDash in_dash=DASH3, VColor in_color=COLOR_HTML_BLACK, 
		double in_width=default_line_width, int in_clip=CLIP_VERTICAL, bool in_show_labels=true);
};
typedef boost::shared_ptr<VConstantLineYElement> VConstantLineYElementPtr;

/** Элемент - коридор*/
class VRangeYElement : public VElement
{
private:
	/** Значение*/
	VDataProviderPtr data;
	/** Указатель на систему координат*/
	VAxesPtr axes;
	/** Текст*/
	std::wstring text;
	/** Цвет линии*/
	VColor color;
	/** Цвет фона*/
	VColor fill_color;
	/** Рисование*/
	virtual void Paint(cairo_t * cr);
public:
	/** Конструктор*/
	VRangeYElement(VDataProviderPtr in_data, VAxesPtr in_axes, VColor in_color, VColor in_fill_color);
};
typedef boost::shared_ptr<VRangeYElement> VRangeYElementPtr;

/** Элемент типа кривые (схематичное изображение)*/
class VCurvesElement : public VElement
{
private:
	/** Данные*/
	VCurvesInfoPtr data;
	/** Система координат*/
	VAxesPtr axes;	
public:
	/** Конструктор*/
	VCurvesElement(VCurvesInfoPtr in_data, VAxesPtr in_axes);	
	/** Рисование*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VCurvesElement> VCurvesElementPtr;

/** Элемент типа свойства кривой (информация под графиком разметки)*/
class VCurvesInfoElement : public VElement
{
private:
	/** Данные*/
	VCurvesInfoPtr data;
	/** Система координат*/
	VAxesPtr axes;	
public:
	/** Конструктор*/
	VCurvesInfoElement(VCurvesInfoPtr in_data, VAxesPtr in_axes);	
	/** Рисование*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VCurvesInfoElement> VCurvesInfoElementPtr;

/** Паспортизация кривой (информация под графиком разметки кривизны)*/
class VCurvesPassportElement : public VElement
{
private:
	/** Данные*/
	VCurvesInfoPtr data;
	/** Система координат*/
	VAxesPtr axes;	
public:
	/** Конструктор*/
	VCurvesPassportElement(VCurvesInfoPtr in_data, VAxesPtr in_axes);	
	/** Рисование*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VCurvesPassportElement> VCurvesPassportElementPtr;

/** Паспортизация кривой (информация под графиком разметки уровня)*/
class VLevelPassportElement : public VElement
{
private:
	/** Данные*/
	VCurvesInfoPtr data;
	/** Система координат*/
	VAxesPtr axes;	
public:
	/** Конструктор*/
	VLevelPassportElement(VCurvesInfoPtr in_data, VAxesPtr in_axes);	
	/** Рисование*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VLevelPassportElement> VLevelPassportElementPtr;

/** Элемент типа прямые (схематичное изображение)*/
class VLinesElement : public VElement
{
private:
	/** Данные*/
	VDataProviderPtr data;
	/** Система координат*/
	VAxesPtr axes;	
public:
	/** Конструктор*/
	VLinesElement(VLinesDataPtr in_data, VAxesPtr in_axes);	
	/** Рисование*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VLinesElement> VLinesElementPtr;


/** Легенда*/
class VLegendElement : public VElement
{
	private:		
		/** Указатель на область*/
		VAxesPtr axes;		
		/** Признак того, что курсор находится над элементом*/
		bool hit;						
		/** Названия*/
		std::vector<std::wstring> names;
		/** Цвета*/
		std::vector<VColor> colors;
	public:
		/** Конструктор*/
		VLegendElement(VAxesPtr in_axes);
		/** Добавляет график в легенду*/
		void AddGraph(std::wstring in_name, VColor in_color);
		/** Рисование*/
		virtual void Paint(cairo_t * cr);		
};
typedef boost::shared_ptr<VLegendElement> VLegendElementPtr;


#define AREA_FRAME_TOP		0x00000001
#define AREA_FRAME_LEFT		0x00000002
#define AREA_FRAME_BOTTOM	0x00000004
#define AREA_FRAME_RIGHT	0x00000008
#define AREA_FRAME_ALL		AREA_FRAME_TOP | AREA_FRAME_LEFT | AREA_FRAME_BOTTOM | AREA_FRAME_RIGHT

/** Рамка области*/
class VAreaFrameElement : public VElement
{
	private:		
		/** Указатель на область*/
		VAreaPtr area;		
		/** Признак того что курсор мыши в области*/
		//bool hit;
		/** Устройство*/
		VDrawingDevicePtr device;
		/** Толщина линии*/ 
		double width;
		/** Стороны*/
		uint32_t borders;
	public:
		/** Конструктор*/
		VAreaFrameElement(VAreaPtr in_area, VDrawingDevicePtr in_device, double in_width=default_line_width, uint32_t in_borders=AREA_FRAME_ALL);
		/** Рисование*/
		virtual void Paint(cairo_t * cr);		
};

/** Название области*/
class VAreaNameElement : public VElement
{
	private:		
		/** Указатель на область*/
		VAreaPtr area;
		/** Имя области*/
		std::wstring name;
		/** Устройство*/
		VDrawingDevicePtr device;
	public:
		/** Конструктор*/
		VAreaNameElement(VAreaPtr in_area, VDrawingDevicePtr in_device, const std::wstring& in_name);
		/** Рисование*/
		virtual void Paint(cairo_t * cr);				
};

/** Название области в стиле КВЛП*/
class VKVLPAreaNameElement : public VElement
{
	private:		
		/** Указатель на область*/
		VAreaPtr area;
		/** Имя области*/
		std::wstring name;
		/** Устройство*/
		VDrawingDevicePtr device;
		/** Толщина линии*/ 
		double width;
	public:
		/** Конструктор*/
		VKVLPAreaNameElement(VAreaPtr in_area, VDrawingDevicePtr in_device, const std::wstring& in_name, double in_width=default_line_width);
		/** Рисование*/
		virtual void Paint(cairo_t * cr);				
};

/** Значение параметра в текущей точке*/
class VTrackValueElement : public VElement
{
private:
	/** Указатель на систему координат*/
	VAxesPtr axes;	
	/** Положение мыши*/
	VDataProviderPtr data;
	/** Список источников данных*/
	std::vector<VDataProviderPtr> data_providers;
	/** Список цветов*/
	std::vector<VColor> colors;	
	
public:
	/** Конструктор*/
	VTrackValueElement(VAxesPtr in_axes, VDataProviderPtr in_data);
	/** Добавляет график в легенду*/
	void AddGraph(VDataProviderPtr in_data_provider, const VColor& in_color);
	/** Рисование*/
	void Paint(cairo_t * cr);	
};
typedef boost::shared_ptr<VTrackValueElement> VTrackValueElementPtr;

/** Значение путевой координаты в данной точке*/
class VTrackCoordElement : public VElement
{
private:
	/** Указатель на систему координат*/
	VAxesPtr axes;	
	/** Источник данных*/
	VAprinfoPtr aprinfo;
	VDataProviderPtr data;		
public:
	/** Конструктор*/
	VTrackCoordElement(VAxesPtr in_axes, VDataProviderPtr in_data, VAprinfoPtr in_aprinfo);	
	/** Рисование*/
	void Paint(cairo_t * cr);	
};
typedef boost::shared_ptr<VTrackCoordElement> VTrackCoordElementPtr;

/** Выделение*/
class VSelectionElement : public VElement
{
	private:				
		/** Область, в которой происходит выделение*/		
		VAxesPtr axes;
		/** Информация о выделенной области*/
		VSelectionInfoPtr selection_info;
		/** Признак того, что курсор над элементом*/
		bool hit_left;
		bool hit_right;
		double left_side_x;
		double right_side_x;
		/** Признак того, что нажат Shift*/
		bool shift_pressed;
	public:
		/** Конструктор*/
		VSelectionElement(VAxesPtr in_axes, VSelectionInfoPtr in_selection_info);
		/** Рисование*/
		virtual void Paint(cairo_t * cr);
		/** Обработка hit-test*/
		bool HitTest(double in_x, double in_y);
		/** Обработка события*/
		virtual bool ProcessEvent(const VEvent& in_event);
};
typedef boost::shared_ptr<VSelectionElement> VSelectionElementPtr;

/** Номер страницы*/
class VPageNumberElement : public VElement
{
private:	
	/** Информация о странице*/	
	VPageInfoPtr page_info;
	/** Область*/
	VAreaPtr area;
	/** Указатель на устройство*/
	VDrawingDevicePtr device;
public:
	/** Конструктор*/
	VPageNumberElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VPageInfoPtr in_page_info);	
	/** Рисование*/
	void Paint(cairo_t * cr);
};

/** Элемент, отображающий перекрестье*/
class VMouseCrossElement : public VElement
{
private:		
	/** Указатель на систему координат*/
	VAxesPtr axes;
	double x;
	double y;
	double value_x;
	double value_y;
	VDataProviderPtr data;

public:
	/** Конструктор*/
	VMouseCrossElement(VAxesPtr in_axes, VDataProviderPtr in_data);
	/** Рисование*/
	void Paint(cairo_t * cr);	
};
typedef boost::shared_ptr<VMouseCrossElement> VMouseCrossElementPtr;

/** Элемент отображающий масштаб по координате в виде 1 см : 500 м или 1:5000*/
class VScaleElement : public VElement
{
private:	
	/** Оси*/
	VAxesPtr axes;	
	/** Показывать масштаб по оси X*/
	bool show_scale_x;
	/** Показывать масштаб по оси Y*/
	bool show_scale_y;
public:
	/** Конструктор*/
	VScaleElement(VAxesPtr in_axes, bool in_show_scale_x=true, bool in_show_scale_y=false);
	/** Рисование*/
	void Paint(cairo_t * cr);
};

//------------------------------ Второй вариант разметки кривых -----------------------------------

/** Точка на разметке кривой*/
class VCurveMarkElement : public VElement
{
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Система координат*/
	VAxesPtr axes;
	/** Индекс точки в массиве данных*/
	size_t index;
	/** Цвет точки*/
	VColor color;
	/** Цвет заливки точки*/
	VColor fill_color;
	/** Толщина линии*/
	double width;
	/** Выводить ли точку за пределами области*/
	int clip;	
	/** Границы элемента*/
	double ext_left;
	/** Границы элемента*/
	double ext_top;
	/** Границы элемента*/
	double ext_right;
	/** Границы элемента*/
	double ext_bottom;	
	/** Показывать метку*/
	bool show_label;
	/** Стиль 0 - кружок, 1 - флаг*/
	point_style_t style;
	/** Драг*/
	bool drag;
	/** Смещение по x при перетаскивании*/
	double drag_shift_x;
	/** Смещение по y при перетаскивании*/
	double drag_shift_y;
public:
	/** Конструктор*/
	VCurveMarkElement(VDataProviderPtr in_data, VAxesPtr in_axes, size_t in_index, const VColor& in_color,  const VColor& in_fill_color, int in_clip=CLIP_AREA, bool in_show_label=true, point_style_t in_style=POINT_STYLE_CIRCLE);
	/** Рисование*/
	virtual void Paint(cairo_t * cr);	
	/** Обработчик HitTest*/
	bool HitTest(double in_x, double in_y);		
	/** Обработка события*/
	bool ProcessEvent(const VEvent& in_event);
};	
typedef boost::shared_ptr<VCurveMarkElement> VCurveMarkElementPtr;


/** График типа points*/
class VCurveMarksGraph : public VElement
{	
private:
	/** Источник данных*/
	VDataProviderPtr data;
	/** Оси*/
	VAxesPtr axes;
	/** Цвет графика*/
	VColor color;
	/** Цвет заливки*/
	VColor fill_color;
	/** Выводить ли точку за пределами области*/
	int clip;		
	/** Список точек*/
	std::vector<VCurveMarkElementPtr> points;
	/** Итератор для списка точек*/
	typedef std::vector<VCurveMarkElementPtr>::iterator point_itr; 
public:
	/** Конструктор*/
	VCurveMarksGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, int in_clip=CLIP_AREA);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
	/** Обработчик сообщений*/
	bool ProcessEvent(const VEvent& in_event);
};
typedef boost::shared_ptr<VCurveMarksGraph> VCurveMarksGraphPtr;


/** Отображение легенды оси Y*/
class VAxesLegendYElement : public VElement
{
private:
	/** Оси*/
	VAxesPtr axes;
public:
	/** Конструктор*/
	VAxesLegendYElement(VAxesPtr in_axes);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VAxesLegendYElement> VAxesLegendYElementPtr;

/** Отображение значения*/
//FixMe!!! добавить выравнивание, довести до ума ориентацию, шрифт, цвет. Отображение нескольких строк.
class VTextElement : public VElement
{
private:
	/** Область*/
	VAreaPtr area;
	/** Устройство*/
	 VDrawingDevicePtr device;
	/** Данные*/
	VTextProviderPtr data;	
public:
	/** Конструктор*/
	VTextElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VTextProviderPtr in_data);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VTextElement> VTextElementPtr;


/** Сводка на ленте КВЛП*/
class VKVLPSummaryElement : public VElement
{
private:
	/** Область*/
	VAreaPtr area;
	/** Устройство*/
	 VDrawingDevicePtr device;
	/** Данные*/
	VTextProviderPtr data;	
public:
	/** Конструктор*/
	VKVLPSummaryElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VTextProviderPtr in_data);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPSummaryElement> VKVLPSummaryElementPtr;


/** Информация о перегоне и километре на ленте КВЛП*/
class VKVLPPeregonElement : public VElement
{
private:
	/** Область*/
	VAreaPtr area;
	/** Устройство*/
	 VDrawingDevicePtr device;
	/** Данные*/
	VTextProviderPtr data;	
public:
	/** Конструктор*/
	VKVLPPeregonElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VTextProviderPtr in_data);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPPeregonElement> VKVLPPeregonElementPtr;


/// Смещения
static const double KVLP_EVENT_COL_SIZES[]={0.7, 1.5, 1.9, 2.6, 3.1, 3.8};

/** События на ленте КВЛП*/
class VKVLPEventElement : public VElement
{
private:
	/** Оси*/
	VAxesPtr axes;	
	/** Данные*/
	VRTEventInfoDataPtr data;	
public:
	/** Конструктор*/
	VKVLPEventElement(VRTEventInfoDataPtr in_data, VAxesPtr in_axes);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPEventElement> VKVLPEventElementPtr;

/** Заголовок области событий на ленте КВЛП*/
class VKVLPEventAreaName : public VElement
{
private:
	/** Область*/
	VAreaPtr area;
	/** Устройство*/
	 VDrawingDevicePtr device;	
	 /** Толщина линии*/ 
	double width;
public:
	/** Конструктор*/
	VKVLPEventAreaName(VAreaPtr in_area, VDrawingDevicePtr in_device, double in_width=default_line_width);
	/** Риcование*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPEventAreaName> VKVLPEventAreaNamePtr;

/** Элемент с координатной сеткой по путевой координате в стиле КВЛП*/
class VKVLPCoordinatesElement : public VElement
{
	private:		
		/** Указатель на систему координат*/
		VAxesPtr axes;		
		/** Указатель на данные*/
		VDataProviderPtr data;
		/** Признак показывать или не показывать текст*/
		bool show_text;
	public:
		/** Конструктор*/
		VKVLPCoordinatesElement(VDataProviderPtr in_data, VAxesPtr in_axes, bool in_show_text);		
		/** Рисование*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPCoordinatesElement> VKVLPCoordinatesElementPtr;

