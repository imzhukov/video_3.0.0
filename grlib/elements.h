#pragma once

#include "providers.h"
#include "axes.h"

/// ������������ ������ FixME!!! �������� �������, �������� ������������ �� ������ � ������� ���� �������
enum TEXT_POSITION {TP_OVER=0, TP_UNDER=1, TP_LEFT=2, TP_RIGHT=3, 
	TP_AREA_BOTTOM_OVER=4, TP_AREA_BOTTOM_UNDER=5,
	TP_AREA_TOP_OVER=6, TP_AREA_TOP_UNDER=7};

/// ���������� ������
enum TEXT_ORIENTATION {TO_HORIZONTAL=0, TO_VERTICAL=1};

/// ������� �����
void CairoShowText(cairo_t * cr, VAxesPtr axes, double x, double y,
	const std::wstring& in_text, TEXT_ORIENTATION to=TO_HORIZONTAL, TEXT_POSITION tp=TP_LEFT, double in_text_space=text_space);

/** ������� � �����*/
class VBackgroundElement : public VElement
{		
private:
	/** ���� ����*/
	VColor fill_color;
public:
	/** �����������*/
	VBackgroundElement(const VColor& in_fill_color);
	/** ���������*/
	virtual void Paint(cairo_t * cr);
};


/// ������� - �����
class VTextGraph : public VElement
{
private:
	/// �������� ������
	VDataProviderPtr data;
	/// ���
	VAxesPtr axes;		
	/// ���������� ������
	TEXT_ORIENTATION to;
	/// ���������� ������
	TEXT_POSITION tp;

public:
	/// �����������
	VTextGraph(VDataProviderPtr in_data, VAxesPtr in_axes, TEXT_ORIENTATION in_to, TEXT_POSITION in_tp);
	/// ���������
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VTextGraph> VTextGraphPtr;


/** ������ ���� line*/
class VLineGraph : public VElement
{			
	private:

		/** ��������� ���  VLineGraph */
		struct point_group
		{
			std::vector<point_t> points;
			VColor color;
		};

		/** ������*/
		VDataProviderPtr data;		
		/** ���*/
		VAxesPtr axes;		
		/** ���� �����*/
		VColor color;		
		/** ���� �����*/
		VColor fill_color;		
		/** ������� �����*/
		double width;
		/** ������� ��������� �� ����� �������*/
		int clip;		

		/** ������� �����, � ������� ����� ������� "������"*/
		std::vector<point_t> GetRelativeXY(double in_x1, double in_y1, double in_x2, double in_y2) const;	

	public:
		/** �����������*/
		VLineGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, const VColor& in_fill_color, double in_width=default_line_width, int in_clip=CLIP_AREA);
		/** ��c������*/	
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VLineGraph> VLineGraphPtr;

/** ������ ���� barchart*/
class VBarGraph : public VElement
{		
	private:
		/** ������*/
		VDataProviderPtr data;		
		/** ���*/
		VAxesPtr axes;		
		/** ���� �����*/
		VColor color;
		/** ���� �������*/
		VColor fill_color;		
		/** ������� �����*/
		double width;		
		/** ������� ��������� �� ����� �������*/
		int clip;		
		/** ���������� �����*/
		bool show_labels;		
	public:
		/** �����������*/
		VBarGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  
			const VColor& in_fill_color, double in_width=default_line_width, int in_clip=CLIP_AREA, 
			bool in_show_labels=false);
		/** ��c������*/	
		virtual void Paint(cairo_t * cr);
};

/** ������ ���� square*/
class VSquareGraph : public VElement
{		
	private:
		/** ������*/
		VDataProviderPtr data;		
		/** ���*/
		VAxesPtr axes;		
		/** ���� �����*/
		VColor color;
		/** ���� �������*/
		VColor fill_color;		
		/** ������� �����*/
		double width;		
		/** ������� ��������� �� ����� �������*/
		int clip;		
		/** ���������� �����*/
		bool show_labels;		
	public:
		/** �����������*/
		VSquareGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  
			const VColor& in_fill_color, double in_width=default_line_width, int in_clip=CLIP_AREA, bool in_show_labels=false);
		/** ��c������*/	
		virtual void Paint(cairo_t * cr);
};

/** ������ ���� area*/
class VAreaGraph : public VElement
{		
	private:
		/** ������*/
		VDataProviderPtr data;		
		/** ���*/
		VAxesPtr axes;		
		/** ���� �����*/
		VColor color;
		/** ���� �������*/
		VColor fill_color;		
		/** ������� �����*/
		double width;		
		/** ������� ��������� �� ����� �������*/
		int clip;		
		/** ���������� �����*/
		bool show_labels;		
	public:
		/** �����������*/
		VAreaGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  
			const VColor& in_fill_color, double in_width=default_line_width, int in_clip=CLIP_AREA, bool in_show_labels=false);
		/** ��c������*/	
		virtual void Paint(cairo_t * cr);
};

/** ������ ���� stick*/
class VStickGraph : public VElement
{		
	private:
		/** ������*/
		VDataProviderPtr data;		
		/** ���*/
		VAxesPtr axes;		
		/** ���� �����*/
		VColor color;
		/** ���� �������*/
		VColor fill_color;		
		/** ������� �����*/
		double width;		
		/** ������� ��������� �� ����� �������*/
		int clip;		
		/** ���������� ��������*/
		bool show_labels;
	public:
		/** �����������*/
		VStickGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, 
			double in_width=default_line_width, int in_clip=CLIP_AREA, bool in_show_labels=true);
		/** ��c������*/	
		virtual void Paint(cairo_t * cr);		
};
typedef boost::shared_ptr<VStickGraph> VStickGraphPtr;

/** ������ ���� stick*/
class VCoordPointGraph : public VElement
{		
	private:
		/** ������*/
		VDataProviderPtr data;		
		/** ���*/
		VAxesPtr axes;		
		/** ���� �����*/
		VColor color;
		/** ���� �������*/
		VColor fill_color;		
		/** ������� �����*/
		double width;		
		/** ������� ��������� �� ����� �������*/
		int clip;		
	public:
		/** �����������*/
		VCoordPointGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, 
			double in_width=default_line_width, int in_clip=CLIP_AREA);
		/** ��c������*/	
		virtual void Paint(cairo_t * cr);
		/** ��������� �������*/
		bool ProcessEvent(const VEvent& in_event);
};

/** ����� �����*/
enum point_style_t
{
	/** ������*/
	POINT_STYLE_CIRCLE=0,
	/** ������*/
	POINT_STYLE_FLAG=1
};

/** ����� - � ������� ����� ������� ���� ����� �������, ��� ����� ������� ����*/
class VPointElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ������� ���������*/
	VAxesPtr axes;
	/** ������ ����� � ������� ������*/
	size_t index;
	/** ���� �����*/
	VColor color;
	/** ���� ������� �����*/
	VColor fill_color;
	/** ������� �����*/
	double width;
	/** �������� �� ����� �� ��������� �������*/
	int clip;	
	/** ������� ��������*/
	double ext_left;
	/** ������� ��������*/
	double ext_top;
	/** ������� ��������*/
	double ext_right;
	/** ������� ��������*/
	double ext_bottom;
	/** ���� ��� ���������*/
	bool hit;	
	/** ������� ���� ��� ���� ��������� ������� ��������*/	
	change_calculator<bool> hit_changed;
	/** ���������� �����*/
	bool show_label;
	/** ���������� ������*/
	TEXT_ORIENTATION to;
	/** ��������� ������*/
	TEXT_POSITION tp;
	/** ����� 0 - ������, 1 - ����*/
	point_style_t style;
	/** ����*/
	bool drag;
	/** �������� �� x ��� ��������������*/
	double drag_shift_x;
	/** �������� �� y ��� ��������������*/
	double drag_shift_y;
public:
	/** �����������*/
	VPointElement(VDataProviderPtr in_data, VAxesPtr in_axes, size_t in_index, const VColor& in_color,  
		const VColor& in_fill_color, int in_clip=CLIP_AREA, bool in_show_label=true, 
		point_style_t in_style=POINT_STYLE_CIRCLE, TEXT_ORIENTATION to=TO_HORIZONTAL, TEXT_POSITION tp=TP_LEFT);
	/** ���������*/
	virtual void Paint(cairo_t * cr);	
	/** ���������� HitTest*/
	bool HitTest(double in_x, double in_y);		
	/** ��������� �������*/
	bool ProcessEvent(const VEvent& in_event);
};	
typedef boost::shared_ptr<VPointElement> VPointElementPtr;

/** ������ ���� points*/
class VPointGraph : public VElement
{	
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ���� �������*/
	VColor color;
	/** ���� �������*/
	VColor fill_color;
	/** �������� �� ����� �� ��������� �������*/
	int clip;	
	/** ����� ����������� �����*/
	point_style_t style;
	/** ������ �����*/
	std::vector<VPointElementPtr> points;
	/** �������� ��� ������ �����*/
	typedef std::vector<VPointElementPtr>::iterator point_itr; 	
	/** ���������� �����*/
	bool show_labels;	
	/** ���������� ������*/
	TEXT_ORIENTATION to;
	/** ��������� ������*/
	TEXT_POSITION tp;
public:
	/** �����������*/
	VPointGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, 
		int in_clip=CLIP_AREA, bool in_show_labels=true, point_style_t in_style=POINT_STYLE_CIRCLE,
		TEXT_ORIENTATION in_to=TO_VERTICAL, TEXT_POSITION in_tp=TP_UNDER);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
	/** ��������� hit-test*/
	bool HitTest(double in_x, double in_y);
	/** ��������� �������*/
	virtual bool ProcessEvent(const VEvent& in_event);	
};
typedef boost::shared_ptr<VPointGraph> VPointGraphPtr;

/** ������ ���� POINT_STICK*/
class VPointStickGraph : public VElement
{		
	private:
		/// �����
		VPointGraphPtr points;
		/// �����
		VStickGraphPtr sticks;

	public:
		/// �����������
		VPointStickGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, 
			double in_width=default_line_width, int in_clip=CLIP_AREA, bool in_show_labels=true);
		/// ��c������	
		virtual void Paint(cairo_t * cr);		
};
typedef boost::shared_ptr<VPointStickGraph> VPointStickGraphPtr;


/// ����� ����������� �������
class VPointLineGraph : public VElement
{		
	private:
		/// �����
		VPointGraphPtr points;
		/// �����
		VLineGraphPtr lines;
	public:
		/// �����������
		VPointLineGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, 
			double in_width=default_line_width, int in_clip=CLIP_AREA, bool in_show_labels=true, 
			point_style_t in_point_style=POINT_STYLE_CIRCLE);
		/// ��c������	
		virtual void Paint(cairo_t * cr);
		/// ��������� hit-test
		bool HitTest(double in_x, double in_y);
		/// ��������� �������
		virtual bool ProcessEvent(const VEvent& in_event);	
};
typedef boost::shared_ptr<VPointLineGraph> VPointLineGraphPtr;

/** ������� - ������� �� ������� �������*/
class VProfileObjectsElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;
	/** ���� �������*/
	VColor fill_color;
	/** �������� ������ �� ��������� �������*/
	int clip;		
public:
	/** �����������*/
	VProfileObjectsElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, int in_clip=CLIP_LEFT | CLIP_RIGHT);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VProfileObjectsElement> VProfileObjectsElementPtr;

/** ������� - ������� �� ��������*/
class VSituationObjectsElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;
	/** ���� �������*/
	VColor fill_color;
	/** �������� ������ �� ��������� �������*/
	int clip;		
public:
	/** �����������*/
	VSituationObjectsElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, int in_clip=CLIP_AREA);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VSituationObjectsElement> VSituationObjectsElementPtr;

/** ����������� �� ����� ����*/
class VKVLPOverstepElement : public VElement
{		
	private:
		/** ������*/
		VDataProviderPtr data;		
		/** ���*/
		VAxesPtr axes;		
		/** ���� �����*/
		VColor color;
		/** ���� �������*/
		VColor fill_color;		
		/** ������� �����*/
		double width;		
		/** ������� ��������� �� ����� �������*/
		int clip;				
	public:
		/** �����������*/
		VKVLPOverstepElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  
			const VColor& in_fill_color, double in_width=default_line_width, int in_clip=CLIP_HORIZONTAL);
		/** ��c������*/	
		virtual void Paint(cairo_t * cr);
};

/// ����������� ��� �� ����� ����
class VKVLPAnpElement : public VElement
{
private:
	/** ������*/
	VDataProviderPtr data;		
	/** ���*/
	VAxesPtr axes;			
public:
	/** �����������*/
	VKVLPAnpElement(VDataProviderPtr in_data, VAxesPtr in_axes);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};

/// ����������� ��� �� ����� ����
class VKVLPPsiElement : public VElement
{
private:
	/** ������*/
	VDataProviderPtr data;		
	/** ���*/
	VAxesPtr axes;			
public:
	/** �����������*/
	VKVLPPsiElement(VDataProviderPtr in_data, VAxesPtr in_axes);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};

/// ����������� ��� �� ����� ����
class VKVLPOtvodWidthElement : public VElement
{
private:
	/** ������*/
	VDataProviderPtr data;		
	/** ���*/
	VAxesPtr axes;			
public:
	/** �����������*/
	VKVLPOtvodWidthElement(VDataProviderPtr in_data, VAxesPtr in_axes);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};


/** ������� - ������� �� ����� ����*/
class VKVLPSwitchesElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;
	/** ���� �������*/
	VColor fill_color;
	/** ������� �����*/
	double width;		
	/** �������� ������ �� ��������� �������*/
	int clip;		
public:
	/** �����������*/
	VKVLPSwitchesElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, const VColor& in_fill_color, 
		double in_width=default_line_width, int in_clip=CLIP_AREA);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPSwitchesElement> VKVLPSwitchesElementPtr;

/** ������� - ������� ������� �� ����� ����*/
class VKVLPSwitchSectionsElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;
	/** ���� �������*/
	VColor fill_color;
	/** ������� �����*/
	double width;		
	/** �������� ������ �� ��������� �������*/
	int clip;		
public:
	/** �����������*/
	VKVLPSwitchSectionsElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, const VColor& in_fill_color, 
		double in_width=default_line_width, int in_clip=CLIP_AREA);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPSwitchSectionsElement> VKVLPSwitchSectionsElementPtr;

/** ������� - ������������� ���������� ������*/ 
class VKVLPPassportStatusElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;		
	/** �������� ������ �� ��������� �������*/
	int clip;	
public:
	/** �����������*/
	VKVLPPassportStatusElement(VDataProviderPtr in_data, VAxesPtr in_axes, int in_clip=CLIP_AREA);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPPassportStatusElement> VKVLPPassportStatusElementPtr;

/** ������� - ����� �� ����� ����*/
class VKVLPBridgeElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;			
public:
	/** �����������*/
	VKVLPBridgeElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPBridgeElement> VKVLPBridgeElementPtr;

/** ������� - ������� �� ����� ����*/
class VKVLPTunnelElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;			
public:
	/** �����������*/
	VKVLPTunnelElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPTunnelElement> VKVLPTunnelElementPtr;


/** ������� - �������� ���� ����� �� ����� ����*/
class VKVLPBridgeAreaElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;			
public:
	/** �����������*/
	VKVLPBridgeAreaElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPBridgeAreaElement> VKVLPBridgeAreaElementPtr;

/** ������� - ������������� ������ �� ����� ����*/
class VKVLPChamferedJointElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;			
public:
	/** �����������*/
	VKVLPChamferedJointElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPChamferedJointElement> VKVLPChamferedJointElementPtr;


/** ������� - �������� �� ����� ����*/
class VKVLPCrossingElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;			
public:
	/** �����������*/
	VKVLPCrossingElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPCrossingElement> VKVLPCrossingElementPtr;


/** ������� - ��� ���� �� ����� ����*/
class VKVLPSpalaElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;
	/** ������� �����*/
	double width;
	/** �������� ������ �� ��������� �������*/
	int clip;	
public:
	/** �����������*/
	VKVLPSpalaElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  double in_width=default_line_width, int in_clip=CLIP_AREA);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPSpalaElement> VKVLPSpalaElementPtr;

/** ������� - ����������� ���� �� ����� ����*/
class VKVLPStraightSideElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;
	/** ������� �����*/
	double width;
public:
	/** �����������*/
	VKVLPStraightSideElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color, double in_width=default_line_width);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPStraightSideElement> VKVLPStraightSideElementPtr;


/** ������� - ��������*/
class VCrossingElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;			
public:
	/** �����������*/
	VCrossingElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VCrossingElement> VCrossingElementPtr;

/** ������� - �����*/
class VBridgeElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;			
public:
	/** �����������*/
	VBridgeElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VBridgeElement> VBridgeElementPtr;

/** ������� - �������*/
class VTunnelElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ����*/
	VColor color;			
public:
	/** �����������*/
	VTunnelElement(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VTunnelElement> VTunnelElementPtr;



/** ������� - ������*/
class VSlopeGraph : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;	
public:
	/** �����������*/
	VSlopeGraph(VDataProviderPtr in_data, VAxesPtr in_axes);
	/** ���������*/
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VSlopeGraph> VSlopeGraph2Ptr;

/** ������� �������*/
enum area_edge_t
{
	AREA_TOP=1,
	AREA_BOTTOM=2,
	AREA_OVER=3,
	AREA_BELOW=4
};

/** ������� ���� - ����� �� ��� X*/
class VXMarkGraph : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ������� ���������*/
	VAxesPtr axes;
	/** ����*/
	VColor color;
	/** ������� �����*/
	double width;
	/** ������ �����*/
	double height;
	/** ������� �������*/
	area_edge_t edge;
	/** ���������� �����*/
	bool show_labels;
public:
	/** �����������*/
	VXMarkGraph(VDataProviderPtr in_data, VAxesPtr in_axes, VColor in_color, double in_width=default_line_width, double in_height=2.0,
		area_edge_t in_edge=AREA_BOTTOM, bool in_show_labels=true);
	/** ���������*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VXMarkGraph> VXMarkGraphPtr;

/** ������� ���� - ������� �� ��� X*/
class VXLineGraph : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ������� ���������*/
	VAxesPtr axes;
	/** �������*/
	VDash dash;
	/** ����*/
	VColor color;
	/** ������� �����*/
	double width;
	/** ������� �������*/
	area_edge_t edge;
public:
	/** �����������*/
	VXLineGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VDash& in_dash=DASH1, const VColor& in_color=COLOR_HTML_GRAY, 
		double in_width=default_line_width, area_edge_t in_edge=AREA_BOTTOM);
	/** ���������*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VXLineGraph> VXLineGraphPtr;


/** ������� ���� - ������� ����*/
class VActiveWaySectionElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ������ ��������*/
	size_t index;	
	/** ������� ���������*/
	VAxesPtr axes;
	/** ����*/
	VColor color;
	/** ���� �������*/
	VColor fill_color;
	/** ������� �����*/
	double width;	
		
public:
	/** �����������*/
	VActiveWaySectionElement(VDataProviderPtr in_data, VAxesPtr in_axes, size_t in_index, VColor in_color, VColor in_fill_color, double in_width=default_line_width);
	/** ���������*/
	void Paint(cairo_t * cr);
	/** ��������� hit-test*/
	bool HitTest(double in_x, double in_y);
	/** ��������� �������*/
	bool ProcessEvent(const VEvent& in_event);
};
typedef boost::shared_ptr<VActiveWaySectionElement> VActiveWaySectionElementPtr;

/** ������� ���� - ������� �� ��� X*/
class VActiveWaySectionGraph : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;	
	/** ������� ���������*/
	VAxesPtr axes;
	/** ����*/
	VColor color;
	/** ���� �������*/
	VColor fill_color;
	/** ������� �����*/
	double width;

	/** ������ �����*/
	std::vector<VActiveWaySectionElementPtr> points;
	/** �������� ��� ������ �����*/
	typedef std::vector<VActiveWaySectionElementPtr>::iterator point_itr; 

	
public:
	/** �����������*/
	VActiveWaySectionGraph(VDataProviderPtr in_data, VAxesPtr in_axes, VColor in_color, VColor in_fill_color, double in_width=default_line_width);
	/** ���������*/
	void Paint(cairo_t * cr);
	/** ��������� �������*/
	bool ProcessEvent(const VEvent& in_event);
};
typedef boost::shared_ptr<VActiveWaySectionGraph> VActiveWaySectionGraphPtr;

/** ������� ���� - ������� �� ��� X*/
class VWaySectionGraph : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ������� ���������*/
	VAxesPtr axes;
	/** ����*/
	VColor color;
	/** ���� �������*/
	VColor fill_color;
	/** ������� �����*/
	double width;
	/** ������� - ���������� �����*/
	bool show_labels;

public:
	/** �����������*/
	VWaySectionGraph(VDataProviderPtr in_data, VAxesPtr in_axes, VColor in_color, VColor in_fill_color, double in_width=default_line_width, bool in_show_labels=true);
	/** ���������*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VWaySectionGraph> VWaySectionGraphPtr;


/** ������� � ���������� �������*/
class VHeaderElement : public VElement
{
	private:				
		/** ��������� �� �������*/
		VAreaPtr area;
		/** ��������� �� ����������*/
		VDrawingDevicePtr device;
		/** ����� ���������*/
		std::vector<std::wstring> lines;
	public:
		/** �����������*/
		VHeaderElement(VAreaPtr in_area, VDrawingDevicePtr in_device);
		/** ���������*/
		virtual void Paint(cairo_t * cr);
		/** ��������� ������ � ���������*/
		void AddLine(const std::wstring& in_line);
};
typedef boost::shared_ptr<VHeaderElement> VHeaderElementPtr;

/** ��������� �������*/
class VGraphHeaderElement : public VElement
{
	private:
		/** �������*/
		VAreaPtr area;
		/** ����������*/
		VDrawingDevicePtr device;
		/** ������*/
		VGraphInfoPtr graph_info;
	public:
		/** �����������*/
		VGraphHeaderElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VGraphInfoPtr in_graph_info);
		/** ���������*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VGraphHeaderElement> VGraphHeaderElementPtr;

/** ������ ������*/
class VDataSetsElement : public VElement
{
	private:
		/** �������*/
		VAreaPtr area;
		/** ����������*/
		VDrawingDevicePtr device;
		/** ������*/
		VGraphInfoPtr graph_info;
	public:
		/** �����������*/
		VDataSetsElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VGraphInfoPtr in_graph_info);
		/** ���������*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VDataSetsElement> VDataSetsElementPtr;


/** ������� � ������������ ������ �� ��� X*/
class VGridLinesXElement : public VElement
{
	private:		
		/** ��������� �� ������� ���������*/
		VAxesPtr axes;
		/** ������� ���������� ��� �� ���������� �����*/
		bool show_text;
	public:
		/** �����������*/
		VGridLinesXElement(VAxesPtr in_axes, bool in_show_text);
		/** ���������*/
		virtual void Paint(cairo_t * cr);
};

/** ������� � ������������ ������ �� ��� Y*/
class VGridLinesYElement : public VElement
{
	private:		
		/** ��������� �� ������� ���������*/
		VAxesPtr axes;
		/** ������� ���������� ��� �� ���������� �����*/
		bool show_text;
		/** �������*/
		side_t side;
	public:
		/** �����������*/
		VGridLinesYElement(VAxesPtr in_axes, bool in_show_text, side_t in_side=SD_LEFT);
		/** ���������*/
		virtual void Paint(cairo_t * cr);
};

/** ������� ������������ ���������� ��������*/
class VPreviousKmElement : public VElement
{
	private:		
		/** ��������� �� ������� ���������*/
		VAxesPtr axes;		
		/** ��������� �� ������*/
		VCoordinatesDataPtr data;		
	public:
		/** �����������*/
		VPreviousKmElement(VAxesPtr in_axes, VCoordinatesDataPtr in_data);		
		/** ���������*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VPreviousKmElement> VPreviousKmElementPtr;

/** ������� � ������������ ������ �� ������� ���������� (������ �����)*/
class VCoordinatesGridElement : public VElement
{
	private:		
		/** ��������� �� ������� ���������*/
		VAxesPtr axes;		
		/** ��������� �� ������*/
		VDataProviderPtr data;		
	public:
		/** �����������*/
		VCoordinatesGridElement(VDataProviderPtr in_data, VAxesPtr in_axes);		
		/** ���������*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VCoordinatesGridElement> VCoordinatesGridElementPtr;

/** ��������� ������� ����������*/
class VCoordinatesTextElement : public VElement
{
	private:		
		/** ��������� �� ������� ���������*/
		VAxesPtr axes;		
		/** ��������� �� ������*/
		VDataProviderPtr data;		
	public:
		/** �����������*/
		VCoordinatesTextElement(VDataProviderPtr in_data, VAxesPtr in_axes);		
		/** ���������*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VCoordinatesTextElement> VCoordinatesTextElementPtr;

/** ������� �� �������� ����������*/
class VKmElement : public VElement
{
	private:		
		/** ��������� �� ������� ���������*/
		VAxesPtr axes;		
		/** ��������� �� ������*/
		VDataProviderPtr data;
		/** ������� ���������� ��� �� ���������� �����*/
		bool show_text;
	public:
		/** �����������*/
		VKmElement(VDataProviderPtr in_data, VAxesPtr in_axes, bool in_show_text);		
		/** ���������*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKmElement> VKmElementPtr;

/** ������� � ��������*/
class VPkElement : public VElement
{
	private:		
		/** ��������� �� ������� ���������*/
		VAxesPtr axes;		
		/** ��������� �� ������*/
		VDataProviderPtr data;
		/** ������� ���������� ��� �� ���������� �����*/
		bool show_text;
	public:
		/** �����������*/
		VPkElement(VDataProviderPtr in_data, VAxesPtr in_axes, bool in_show_text);		
		/** ���������*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VPkElement> VPkElementPtr;

/** ������� - ����� ���������*/
class VConstantLineYElement : public VElement
{
private:
	/** ��������*/
	VDataProviderPtr data;
	/** ��������� �� ������� ���������*/
	VAxesPtr axes;
	/** �������*/
	VDash dash;
	/** ���� �����*/
	VColor color;	
	/** ������� �����*/
	double width;
	/** ������� ��������� �� ����� �������*/
	int clip;		
	/** ������� - ���������� ���������*/
	bool show_labels;
	/** ���������*/
	virtual void Paint(cairo_t * cr);
public:
	/** �����������*/
	VConstantLineYElement(VDataProviderPtr in_data, VAxesPtr in_axes, VDash in_dash=DASH3, VColor in_color=COLOR_HTML_BLACK, 
		double in_width=default_line_width, int in_clip=CLIP_VERTICAL, bool in_show_labels=true);
};
typedef boost::shared_ptr<VConstantLineYElement> VConstantLineYElementPtr;

/** ������� - �������*/
class VRangeYElement : public VElement
{
private:
	/** ��������*/
	VDataProviderPtr data;
	/** ��������� �� ������� ���������*/
	VAxesPtr axes;
	/** �����*/
	std::wstring text;
	/** ���� �����*/
	VColor color;
	/** ���� ����*/
	VColor fill_color;
	/** ���������*/
	virtual void Paint(cairo_t * cr);
public:
	/** �����������*/
	VRangeYElement(VDataProviderPtr in_data, VAxesPtr in_axes, VColor in_color, VColor in_fill_color);
};
typedef boost::shared_ptr<VRangeYElement> VRangeYElementPtr;

/** ������� ���� ������ (����������� �����������)*/
class VCurvesElement : public VElement
{
private:
	/** ������*/
	VCurvesInfoPtr data;
	/** ������� ���������*/
	VAxesPtr axes;	
public:
	/** �����������*/
	VCurvesElement(VCurvesInfoPtr in_data, VAxesPtr in_axes);	
	/** ���������*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VCurvesElement> VCurvesElementPtr;

/** ������� ���� �������� ������ (���������� ��� �������� ��������)*/
class VCurvesInfoElement : public VElement
{
private:
	/** ������*/
	VCurvesInfoPtr data;
	/** ������� ���������*/
	VAxesPtr axes;	
public:
	/** �����������*/
	VCurvesInfoElement(VCurvesInfoPtr in_data, VAxesPtr in_axes);	
	/** ���������*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VCurvesInfoElement> VCurvesInfoElementPtr;

/** ������������� ������ (���������� ��� �������� �������� ��������)*/
class VCurvesPassportElement : public VElement
{
private:
	/** ������*/
	VCurvesInfoPtr data;
	/** ������� ���������*/
	VAxesPtr axes;	
public:
	/** �����������*/
	VCurvesPassportElement(VCurvesInfoPtr in_data, VAxesPtr in_axes);	
	/** ���������*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VCurvesPassportElement> VCurvesPassportElementPtr;

/** ������������� ������ (���������� ��� �������� �������� ������)*/
class VLevelPassportElement : public VElement
{
private:
	/** ������*/
	VCurvesInfoPtr data;
	/** ������� ���������*/
	VAxesPtr axes;	
public:
	/** �����������*/
	VLevelPassportElement(VCurvesInfoPtr in_data, VAxesPtr in_axes);	
	/** ���������*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VLevelPassportElement> VLevelPassportElementPtr;

/** ������� ���� ������ (����������� �����������)*/
class VLinesElement : public VElement
{
private:
	/** ������*/
	VDataProviderPtr data;
	/** ������� ���������*/
	VAxesPtr axes;	
public:
	/** �����������*/
	VLinesElement(VLinesDataPtr in_data, VAxesPtr in_axes);	
	/** ���������*/
	void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VLinesElement> VLinesElementPtr;


/** �������*/
class VLegendElement : public VElement
{
	private:		
		/** ��������� �� �������*/
		VAxesPtr axes;		
		/** ������� ����, ��� ������ ��������� ��� ���������*/
		bool hit;						
		/** ��������*/
		std::vector<std::wstring> names;
		/** �����*/
		std::vector<VColor> colors;
	public:
		/** �����������*/
		VLegendElement(VAxesPtr in_axes);
		/** ��������� ������ � �������*/
		void AddGraph(std::wstring in_name, VColor in_color);
		/** ���������*/
		virtual void Paint(cairo_t * cr);		
};
typedef boost::shared_ptr<VLegendElement> VLegendElementPtr;


#define AREA_FRAME_TOP		0x00000001
#define AREA_FRAME_LEFT		0x00000002
#define AREA_FRAME_BOTTOM	0x00000004
#define AREA_FRAME_RIGHT	0x00000008
#define AREA_FRAME_ALL		AREA_FRAME_TOP | AREA_FRAME_LEFT | AREA_FRAME_BOTTOM | AREA_FRAME_RIGHT

/** ����� �������*/
class VAreaFrameElement : public VElement
{
	private:		
		/** ��������� �� �������*/
		VAreaPtr area;		
		/** ������� ���� ��� ������ ���� � �������*/
		//bool hit;
		/** ����������*/
		VDrawingDevicePtr device;
		/** ������� �����*/ 
		double width;
		/** �������*/
		uint32_t borders;
	public:
		/** �����������*/
		VAreaFrameElement(VAreaPtr in_area, VDrawingDevicePtr in_device, double in_width=default_line_width, uint32_t in_borders=AREA_FRAME_ALL);
		/** ���������*/
		virtual void Paint(cairo_t * cr);		
};

/** �������� �������*/
class VAreaNameElement : public VElement
{
	private:		
		/** ��������� �� �������*/
		VAreaPtr area;
		/** ��� �������*/
		std::wstring name;
		/** ����������*/
		VDrawingDevicePtr device;
	public:
		/** �����������*/
		VAreaNameElement(VAreaPtr in_area, VDrawingDevicePtr in_device, const std::wstring& in_name);
		/** ���������*/
		virtual void Paint(cairo_t * cr);				
};

/** �������� ������� � ����� ����*/
class VKVLPAreaNameElement : public VElement
{
	private:		
		/** ��������� �� �������*/
		VAreaPtr area;
		/** ��� �������*/
		std::wstring name;
		/** ����������*/
		VDrawingDevicePtr device;
		/** ������� �����*/ 
		double width;
	public:
		/** �����������*/
		VKVLPAreaNameElement(VAreaPtr in_area, VDrawingDevicePtr in_device, const std::wstring& in_name, double in_width=default_line_width);
		/** ���������*/
		virtual void Paint(cairo_t * cr);				
};

/** �������� ��������� � ������� �����*/
class VTrackValueElement : public VElement
{
private:
	/** ��������� �� ������� ���������*/
	VAxesPtr axes;	
	/** ��������� ����*/
	VDataProviderPtr data;
	/** ������ ���������� ������*/
	std::vector<VDataProviderPtr> data_providers;
	/** ������ ������*/
	std::vector<VColor> colors;	
	
public:
	/** �����������*/
	VTrackValueElement(VAxesPtr in_axes, VDataProviderPtr in_data);
	/** ��������� ������ � �������*/
	void AddGraph(VDataProviderPtr in_data_provider, const VColor& in_color);
	/** ���������*/
	void Paint(cairo_t * cr);	
};
typedef boost::shared_ptr<VTrackValueElement> VTrackValueElementPtr;

/** �������� ������� ���������� � ������ �����*/
class VTrackCoordElement : public VElement
{
private:
	/** ��������� �� ������� ���������*/
	VAxesPtr axes;	
	/** �������� ������*/
	VAprinfoPtr aprinfo;
	VDataProviderPtr data;		
public:
	/** �����������*/
	VTrackCoordElement(VAxesPtr in_axes, VDataProviderPtr in_data, VAprinfoPtr in_aprinfo);	
	/** ���������*/
	void Paint(cairo_t * cr);	
};
typedef boost::shared_ptr<VTrackCoordElement> VTrackCoordElementPtr;

/** ���������*/
class VSelectionElement : public VElement
{
	private:				
		/** �������, � ������� ���������� ���������*/		
		VAxesPtr axes;
		/** ���������� � ���������� �������*/
		VSelectionInfoPtr selection_info;
		/** ������� ����, ��� ������ ��� ���������*/
		bool hit_left;
		bool hit_right;
		double left_side_x;
		double right_side_x;
		/** ������� ����, ��� ����� Shift*/
		bool shift_pressed;
	public:
		/** �����������*/
		VSelectionElement(VAxesPtr in_axes, VSelectionInfoPtr in_selection_info);
		/** ���������*/
		virtual void Paint(cairo_t * cr);
		/** ��������� hit-test*/
		bool HitTest(double in_x, double in_y);
		/** ��������� �������*/
		virtual bool ProcessEvent(const VEvent& in_event);
};
typedef boost::shared_ptr<VSelectionElement> VSelectionElementPtr;

/** ����� ��������*/
class VPageNumberElement : public VElement
{
private:	
	/** ���������� � ��������*/	
	VPageInfoPtr page_info;
	/** �������*/
	VAreaPtr area;
	/** ��������� �� ����������*/
	VDrawingDevicePtr device;
public:
	/** �����������*/
	VPageNumberElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VPageInfoPtr in_page_info);	
	/** ���������*/
	void Paint(cairo_t * cr);
};

/** �������, ������������ �����������*/
class VMouseCrossElement : public VElement
{
private:		
	/** ��������� �� ������� ���������*/
	VAxesPtr axes;
	double x;
	double y;
	double value_x;
	double value_y;
	VDataProviderPtr data;

public:
	/** �����������*/
	VMouseCrossElement(VAxesPtr in_axes, VDataProviderPtr in_data);
	/** ���������*/
	void Paint(cairo_t * cr);	
};
typedef boost::shared_ptr<VMouseCrossElement> VMouseCrossElementPtr;

/** ������� ������������ ������� �� ���������� � ���� 1 �� : 500 � ��� 1:5000*/
class VScaleElement : public VElement
{
private:	
	/** ���*/
	VAxesPtr axes;	
	/** ���������� ������� �� ��� X*/
	bool show_scale_x;
	/** ���������� ������� �� ��� Y*/
	bool show_scale_y;
public:
	/** �����������*/
	VScaleElement(VAxesPtr in_axes, bool in_show_scale_x=true, bool in_show_scale_y=false);
	/** ���������*/
	void Paint(cairo_t * cr);
};

//------------------------------ ������ ������� �������� ������ -----------------------------------

/** ����� �� �������� ������*/
class VCurveMarkElement : public VElement
{
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ������� ���������*/
	VAxesPtr axes;
	/** ������ ����� � ������� ������*/
	size_t index;
	/** ���� �����*/
	VColor color;
	/** ���� ������� �����*/
	VColor fill_color;
	/** ������� �����*/
	double width;
	/** �������� �� ����� �� ��������� �������*/
	int clip;	
	/** ������� ��������*/
	double ext_left;
	/** ������� ��������*/
	double ext_top;
	/** ������� ��������*/
	double ext_right;
	/** ������� ��������*/
	double ext_bottom;	
	/** ���������� �����*/
	bool show_label;
	/** ����� 0 - ������, 1 - ����*/
	point_style_t style;
	/** ����*/
	bool drag;
	/** �������� �� x ��� ��������������*/
	double drag_shift_x;
	/** �������� �� y ��� ��������������*/
	double drag_shift_y;
public:
	/** �����������*/
	VCurveMarkElement(VDataProviderPtr in_data, VAxesPtr in_axes, size_t in_index, const VColor& in_color,  const VColor& in_fill_color, int in_clip=CLIP_AREA, bool in_show_label=true, point_style_t in_style=POINT_STYLE_CIRCLE);
	/** ���������*/
	virtual void Paint(cairo_t * cr);	
	/** ���������� HitTest*/
	bool HitTest(double in_x, double in_y);		
	/** ��������� �������*/
	bool ProcessEvent(const VEvent& in_event);
};	
typedef boost::shared_ptr<VCurveMarkElement> VCurveMarkElementPtr;


/** ������ ���� points*/
class VCurveMarksGraph : public VElement
{	
private:
	/** �������� ������*/
	VDataProviderPtr data;
	/** ���*/
	VAxesPtr axes;
	/** ���� �������*/
	VColor color;
	/** ���� �������*/
	VColor fill_color;
	/** �������� �� ����� �� ��������� �������*/
	int clip;		
	/** ������ �����*/
	std::vector<VCurveMarkElementPtr> points;
	/** �������� ��� ������ �����*/
	typedef std::vector<VCurveMarkElementPtr>::iterator point_itr; 
public:
	/** �����������*/
	VCurveMarksGraph(VDataProviderPtr in_data, VAxesPtr in_axes, const VColor& in_color,  const VColor& in_fill_color, int in_clip=CLIP_AREA);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
	/** ���������� ���������*/
	bool ProcessEvent(const VEvent& in_event);
};
typedef boost::shared_ptr<VCurveMarksGraph> VCurveMarksGraphPtr;


/** ����������� ������� ��� Y*/
class VAxesLegendYElement : public VElement
{
private:
	/** ���*/
	VAxesPtr axes;
public:
	/** �����������*/
	VAxesLegendYElement(VAxesPtr in_axes);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VAxesLegendYElement> VAxesLegendYElementPtr;

/** ����������� ��������*/
//FixMe!!! �������� ������������, ������� �� ��� ����������, �����, ����. ����������� ���������� �����.
class VTextElement : public VElement
{
private:
	/** �������*/
	VAreaPtr area;
	/** ����������*/
	 VDrawingDevicePtr device;
	/** ������*/
	VTextProviderPtr data;	
public:
	/** �����������*/
	VTextElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VTextProviderPtr in_data);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VTextElement> VTextElementPtr;


/** ������ �� ����� ����*/
class VKVLPSummaryElement : public VElement
{
private:
	/** �������*/
	VAreaPtr area;
	/** ����������*/
	 VDrawingDevicePtr device;
	/** ������*/
	VTextProviderPtr data;	
public:
	/** �����������*/
	VKVLPSummaryElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VTextProviderPtr in_data);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPSummaryElement> VKVLPSummaryElementPtr;


/** ���������� � �������� � ��������� �� ����� ����*/
class VKVLPPeregonElement : public VElement
{
private:
	/** �������*/
	VAreaPtr area;
	/** ����������*/
	 VDrawingDevicePtr device;
	/** ������*/
	VTextProviderPtr data;	
public:
	/** �����������*/
	VKVLPPeregonElement(VAreaPtr in_area, VDrawingDevicePtr in_device, VTextProviderPtr in_data);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPPeregonElement> VKVLPPeregonElementPtr;


/// ��������
static const double KVLP_EVENT_COL_SIZES[]={0.7, 1.5, 1.9, 2.6, 3.1, 3.8};

/** ������� �� ����� ����*/
class VKVLPEventElement : public VElement
{
private:
	/** ���*/
	VAxesPtr axes;	
	/** ������*/
	VRTEventInfoDataPtr data;	
public:
	/** �����������*/
	VKVLPEventElement(VRTEventInfoDataPtr in_data, VAxesPtr in_axes);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPEventElement> VKVLPEventElementPtr;

/** ��������� ������� ������� �� ����� ����*/
class VKVLPEventAreaName : public VElement
{
private:
	/** �������*/
	VAreaPtr area;
	/** ����������*/
	 VDrawingDevicePtr device;	
	 /** ������� �����*/ 
	double width;
public:
	/** �����������*/
	VKVLPEventAreaName(VAreaPtr in_area, VDrawingDevicePtr in_device, double in_width=default_line_width);
	/** ��c������*/	
	virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPEventAreaName> VKVLPEventAreaNamePtr;

/** ������� � ������������ ������ �� ������� ���������� � ����� ����*/
class VKVLPCoordinatesElement : public VElement
{
	private:		
		/** ��������� �� ������� ���������*/
		VAxesPtr axes;		
		/** ��������� �� ������*/
		VDataProviderPtr data;
		/** ������� ���������� ��� �� ���������� �����*/
		bool show_text;
	public:
		/** �����������*/
		VKVLPCoordinatesElement(VDataProviderPtr in_data, VAxesPtr in_axes, bool in_show_text);		
		/** ���������*/
		virtual void Paint(cairo_t * cr);
};
typedef boost::shared_ptr<VKVLPCoordinatesElement> VKVLPCoordinatesElementPtr;

