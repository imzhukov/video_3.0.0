#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <typeinfo>
#include "VDrawing.h"
#include "shiny.h"

//----------------------------------- ������ ������ VHotKey -----------------------------------

// [mg] NB! ������ �� ������� ��� ���������� ���-�� ������ � ��������� 
// ��� ������ boost::algorithm::split() 
VHotKey::VHotKey(const std::wstring& in_string)
{
	//�� ���� ������ ��������� ������ ���� "ctrl+81" ��� "ctrl+q"
	std::vector<std::wstring> parts;		
	boost::algorithm::split(parts, in_string, boost::is_any_of(L"+"));
	switch (parts.size())
	{
	case 1://��� ������������
		key=parts.at(0).size()>1 ? boost::lexical_cast<int>(parts.at(0)) : (int)parts.at(0).at(0);
		alt=false;
		ctrl=false;
		shift=false;
		break;
	case 2://� �������������
		key=parts.at(1).size()>1 ? boost::lexical_cast<int>(parts.at(1)) : (int)parts.at(1).at(0);
		alt=boost::iequals(parts.at(0), L"alt");
		ctrl=boost::iequals(parts.at(0), L"ctrl");
		shift=boost::iequals(parts.at(0), L"shift");
		break;
	default:
		throw VSimpleException(L"������ ������� ������", in_string, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
};

//----------------------------------- ������ ������ VDrawingDevice -----------------------------------


/** ���������� ������ �� X*/
double VDrawingDevice::GetSizeX() const {return size_x;};
/** ���������� ������ �� Y*/
double VDrawingDevice::GetSizeY() const {return size_y;};
/** ������������� ���������� �� ����������� �� ���������� �� ��� X*/
double VDrawingDevice::DrawingToDeviceX(double in_value) const {return in_value*aspect_x;};
/** ������������� ���������� �� ���������� � ����������� �� ��� X*/
double VDrawingDevice::DeviceToDrawingX(double in_value) const {return in_value/aspect_x;};
/** ������������� ���������� �� ����������� �� ���������� �� ��� Y*/
double VDrawingDevice::DrawingToDeviceY(double in_value) const {return in_value*aspect_y;};
/** ������������� ���������� �� ���������� � ����������� �� ��� Y*/
double VDrawingDevice::DeviceToDrawingY(double in_value) const {return in_value/aspect_y;};

//---------------------------------------- ������ ������ VDrawingDeviceWin32 -------------------------

/** �����������*/
VDrawingDeviceWin32::VDrawingDeviceWin32(HDC in_hdc)
{
	aspect_x=inch2cm(GetDeviceCaps(in_hdc, LOGPIXELSX));
	aspect_y=inch2cm(GetDeviceCaps(in_hdc, LOGPIXELSX));
	size_x=GetDeviceCaps(in_hdc, HORZRES);
	size_y=GetDeviceCaps(in_hdc, VERTRES);
}

//--------------------------------------- ������ ������ VDrawingDevicePaper ------------------------------

/** �����������*/
VDrawingDevicePaper::VDrawingDevicePaper(double in_ppi_x, double in_ppi_y, double in_size_mm_x, double in_size_mm_y)
{
	aspect_x=inch2cm(in_ppi_x);
	aspect_y=inch2cm(in_ppi_y);
	size_x=DrawingToDeviceX(in_size_mm_x);
	size_y=DrawingToDeviceY(in_size_mm_y);
}

//---------------------------------------- ������ ������ VDrawingDeviceDisplay ----------------------------

/** �����������*/
VDrawingDeviceDisplay::VDrawingDeviceDisplay(double in_ppi_x, double in_ppi_y, double in_size_x, double in_size_y)
{
	aspect_x=inch2cm(in_ppi_x);
	aspect_y=inch2cm(in_ppi_y);
	size_x=in_size_x;
	size_y=in_size_y;
}

//------------------------------------- ������ ������ VLayer ----------------------------------------

/** �����������*/
VLayer::VLayer(const std::wstring& in_name) : VActiveObject(), visible(true)
{
	name=in_name;	
};

/** ���������*/
void VLayer::Paint(cairo_t * cr)
{
	if (visible)
	{
		for (element_itr i=elements.begin();i!=elements.end();i++)
		{	
			(*i)->Render(cr);
		}
	}
}


/** ��������� ������� � ����*/
void VLayer::AddElement(VElementPtr element)
{		
	if (!element)
		throw NULL_POINTER;
	elements.insert(elements.end(), element);
}

/** ������� ��� ��������*/
void VLayer::ClearElements()
{	
	elements.clear();
}

/** ������ ���� �������*/
void VLayer::SetVisible(bool in_visible)
{
	visible=in_visible;
}

/** ���������� ������� ��������� ����*/
bool VLayer::GetVisible()
{
	return visible;
}

/** ���������� ��� ����*/
std::wstring& VLayer::GetName()
{
	return name;
}

/** ������� ������� ���� ��������� ����*/
bool VLayer::ProcessEvent(const VEvent& in_event)
{	
	bool result=false;
	if (GetActive())
	{
		for (element_itr i=elements.begin(); i!=elements.end(); ++i)
		{
			//bool changed=(*i).second->ProcessEvent(in_event);
			bool changed=(*i)->ProcessEvent(in_event);
			result = result || changed;
		}
	}
	return result;
}

//------------------------------------ ������ ������ VArea -----------------------------------------

/** ������������� �������*/
void VArea::SetSize(double in_width, double in_height)
{
	width=round(in_width);
	height=round(in_height);
}

/** ���������� ������*/
double VArea::GetWidth() const
{
	return width;
}

/** ���������� ������*/
double VArea::GetHeight() const
{
	return height;
}

/** ������������� ���������*/
void VArea::SetPosition(double in_left, double in_top)
{
	left=round(in_left);
	top=round(in_top);
}

/** ���������� ���������� �������� ������ ���� �� ��� X*/
double VArea::GetLeft()
{
	return left;
}

/** ���������� ���������� �������� ������ ���� �� ��� Y*/
double VArea::GetTop()
{
	return top;
}

/** ��������� �������*/
void VArea::Apply(cairo_t * cr, int clip)
{	
	cairo_save(cr);	
	double t,l,w,h;
	cairo_clip_extents(cr, &t, &l, &w, &h);			
	cairo_rectangle(cr, 
		(clip & CLIP_LEFT) == CLIP_LEFT ? left : l, 
		(clip & CLIP_TOP) == CLIP_TOP ? top : t, 
		(clip & CLIP_RIGHT) == CLIP_RIGHT ?  width : w, 
		(clip & CLIP_BOTTOM) == CLIP_BOTTOM ? height : h);
	cairo_clip(cr);
	cairo_translate(cr, left, top);
	
	cairo_status_t status=cairo_status(cr);
	
	if (status!=CAIRO_STATUS_SUCCESS)
	{
		throw VSimpleException(L"������ Cairo", string_to_wstring(cairo_status_to_string(status)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__, string_to_wstring(typeid(*this).name()));
	}
}

/** ���������� �������� ���������*/
void VArea::Restore(cairo_t * cr)
{
	cairo_restore(cr);
}

/** ���������� true, ���� ������ ������ �������*/
bool VArea::HitTest(double in_x, double in_y)
{
	return (in_x>left && in_x<left+width && in_y>top && in_y<top+height);
}

/** ������������� ���������� X �� �������� � �������*/
double VArea::ScreenToAreaX(double in_x)
{
	return in_x-left;
}

/** ������������� ���������� Y �� �������� � �������*/
double VArea::ScreenToAreaY(double in_y)
{
	return in_y-top;
}
/** ������������� ���������� X �� ������� � ��������*/
double VArea::AreaToScreenX(double in_x)
{
	return in_x+left;
}
/** ������������� ���������� Y �� ������� � ��������*/
double VArea::AreaToScreenY(double in_y)
{
	return in_y+top;
}

/** ��������� �������*/
bool VArea::ProcessEvent(const VEvent& in_event)
{
	switch (in_event.GetCode())
	{
		case EVENT_MOUSE_MOTION:			
		{
			double x=((VMouseEvent&)in_event).GetX();
			double y=((VMouseEvent&)in_event).GetY();
			SetActive(HitTest(x,y));
			break;
		}
		default:
			break;
	}
	return true;
}

//------------------------ ������ ������ VAreaSizer -----------------------------------------------

/** �����������*/
VAreaSizer::VAreaSizer(VAreaPtr in_area, double in_rel_left, double in_rel_top, double in_rel_width, double in_rel_height)
{
	area=in_area;
	SetRelLeft(in_rel_left);
	SetRelTop(in_rel_top);
	SetRelWidth(in_rel_width);
	SetRelHeight(in_rel_height);	
};

/** ���������� ������������� ��������� ������ �������� ���� �� ��� X*/
void VAreaSizer::SetRelLeft(const double in_rel_left)
{
	rel_left=in_rel_left;
};

/** �������� ������������� ��������� ������ �������� ���� �� ��� X*/
double VAreaSizer::GetRelLeft()
{
	return rel_left;
};

/** ���������� ������������� ��������� ������ �������� ���� �� ��� Y*/
void VAreaSizer::SetRelTop(const double in_rel_top)
{
	rel_top=in_rel_top;
};

/** �������� ������������� ��������� ������ �������� ���� �� ��� Y*/
double VAreaSizer::GetRelTop()
{
	return rel_top;
};

/** ���������� ������������� ������*/
void VAreaSizer::SetRelWidth(const double in_rel_width)
{
	rel_width=in_rel_width;
};

/** �������� ������������� ������*/
double VAreaSizer::GetRelWidth()
{
	return rel_width;
};

/** ���������� ������������� ������*/
void VAreaSizer::SetRelHeight(const double in_rel_height)
{
	rel_height=in_rel_height;
};

/** �������� ������������� ������*/
double VAreaSizer::GetRelHeight()
{
	return rel_height;
};
											
/** ��������� ����������*/
void VAreaSizer::CalcSizeAndPos(double top_margin, double left_margin, double bottom_margin, double right_margin, 
		double in_visible_width, double in_visible_height, VDrawingDevicePtr device)
{		
	if (device)
	{
		double ViewPortSizeY = device->DeviceToDrawingY(in_visible_height)-top_margin-bottom_margin;
		double ViewPortSizeX = device->DeviceToDrawingX(in_visible_width)-left_margin-right_margin;
		double top_pos=device->DrawingToDeviceY(top_margin+GetRelTop()*ViewPortSizeY/100.0);
		double area_height=device->DrawingToDeviceY(GetRelHeight()*ViewPortSizeY/100.0);		
		double left_pos=device->DrawingToDeviceX(left_margin+GetRelLeft()*ViewPortSizeX/100.0);
		double area_width=device->DrawingToDeviceX(GetRelWidth()*ViewPortSizeX/100.0);
		//����������� ������� (����� �� ������� �������� ����� ���������) // FixMe!!! ������
		double v=fmod(top_pos, 1);
		top_pos-=v;	
		area_height=floor(area_height+v);

		double h=fmod(left_pos, 1);
		left_pos-=h;	
		area_width=floor(area_width+h);			
		
		area->SetPosition(left_pos, top_pos);
		area->SetSize(area_width, area_height);
	}
	else
		throw NULL_POINTER;
}

//------------------------------------ ������ ������ VElement ---------------------------------------

/** ����������� ����� ����������*/
void VElement::PrePaint(cairo_t * cr)
{
	//�� ��������� ��������� ��������� ��������� ���������
	cairo_save(cr);
};

/** ����������� ����� ���������*/
void VElement::PostPaint(cairo_t * cr)
{
	//�� ��������� ��������� ��������� ��������� ���������
	cairo_restore(cr);
};

/** ���������*/
void VElement::Render(cairo_t * cr)
{	
	PrePaint(cr);
	Paint(cr);
	cairo_status_t status=cairo_status(cr);
	if (status!=CAIRO_STATUS_SUCCESS)
	{			
		throw VSimpleException(L"������ Cairo", string_to_wstring(cairo_status_to_string(status)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__, string_to_wstring(typeid(*this).name()));
	}
	PostPaint(cr);
};

/** ��������� ��������� �� ������ ���� ��� ��������� (�� ��������� ���������� false (���������� �������))*/
bool VElement::HitTest(double x, double y)
{
	return false;
}

/** ������� ���� ��� ���� ��������� ������� ��������*/
bool VElement::HitChanged(){return false;};

//------------------------------------- ������ ������ VRawData -------------------------------------


/// �������� ������ ��� ������
void VRawData::Allocate(size_t in_width, size_t in_height)
{
	width=in_width;
	height=in_height;
	size_t data_size=width*height*3;
	dataRGB=(unsigned char*)malloc(data_size);
}

/// ��������� ������ ������
void VRawData::Assign(unsigned char * in_dataBGRA)
{
	for (int i=0; i<width*height; ++i)
	{
		dataRGB[i*3+0]=in_dataBGRA[i*4+2];
		dataRGB[i*3+1]=in_dataBGRA[i*4+1];
		dataRGB[i*3+2]=in_dataBGRA[i*4+0];
	}
}

/// �����������
VRawData::VRawData(size_t in_width, size_t in_height, unsigned char * in_dataRGBA)
{
	Allocate(in_width, in_height);
	Assign(in_dataRGBA);
};

/// �����������
VRawData::VRawData(cairo_surface_t * in_surface)
{
	Allocate(cairo_image_surface_get_width(in_surface), cairo_image_surface_get_height(in_surface));
	Assign(cairo_image_surface_get_data(in_surface));
};

/// �����������
VRawData::VRawData(const VRawData& in_source)
{
	width=in_source.width;
	height=in_source.height;
	size_t data_size=width*height*3;
	dataRGB=(unsigned char*)malloc(data_size);
	memcpy(dataRGB,in_source.dataRGB,data_size);
}
/// ����������
VRawData::~VRawData()
{
	free(dataRGB);
};
/// ���������� ������ �����������
size_t VRawData::GetWidth() const {return width;};
/// ���������� ������ �����������
size_t VRawData::GetHeight() const {return height;};
/// ���������� ������
unsigned char * VRawData::GetData() const {return dataRGB;};

//------------------------------------ ������ ������ VDrawing ---------------------------------------


/** �����������*/
VDrawing::VDrawing() : cairo_format(CAIRO_FORMAT_RGB24), device(new VDrawingDevice()), changed(true){}

/** ����������*/
VDrawing::~VDrawing(){}

/** ���������*/
void VDrawing::Paint(cairo_t * cr)
{		
	for (layer_itr i=layers.begin();i!=layers.end();i++)
	{		
		(*i)->Paint(cr);		
	}
}

/** �������� ����*/
void VDrawing::AddLayer(const std::wstring& name)
{
	//���������� ���������� ������������ ���� �����
	if (!LayerExists(name))
	{
		VLayerPtr layer = VLayerPtr(new VLayer(name));
		layers.push_back(layer);
	}
	//���� ���� � ����� ������ ��� ����������, ������ �� ����������
}

/** ���������� ��������� �� ����*/
VLayerPtr VDrawing::GetLayer(const std::wstring& name)
{	
	for (layer_itr i=layers.begin();i!=layers.end();i++)
	{
		if ((*i)->GetName()==name)
		{
			return *i;
		}
	}			
	throw VSimpleException(L"���� �� ������", name, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
}

/** ���������� true, ���� ���� � ����� ������ ����������*/
bool VDrawing::LayerExists(const std::wstring& name)
{	
	for (layer_itr i=layers.begin();i!=layers.end();i++)
	{
		if ((*i)->GetName()==name)
		{
			return true;
		}
	}	
	return false;
}

/** ������� ����*/
void VDrawing::RemoveLayer(const std::wstring& name)
{	
	for (layer_itr i=layers.begin(); i!=layers.end(); i++)
	{
		if ((*i)->GetName()==name)
		{			
			layers.erase(i);			
			return;
		}
	}
}

/** ������� ��� ����*/
void VDrawing::ClearLayers()
{			
	layers.clear();
}

/** ���������� true, ���� ���������� �������� ������*/
bool VDrawing::IsDataChanged()
{
	return false;
};

/** ��������� ��������*/
void VDrawing::AddAction(VActionPtr in_action)
{
	standart_actions.push_back(in_action);
}


/** ������ ��� �������*/
void VDrawing::SetName(const std::wstring& in_name)
{
	name=in_name;
}

/** ���������� ��� �������*/
const std::wstring& VDrawing::GetName() const
{
	return name;
}

/** ������������� ����������*/
void VDrawing::SetDevice(VDrawingDevicePtr in_device)
{
	*device=*in_device;
	SetVisibleSize(device->GetSizeX(), device->GetSizeY());
	AdjustAreas();
}

/** ��������� � ������*/
VRawData VDrawing::Render()
{
	//������� ����������� ��� ��������� � ������
	VCairoSurface img_surface(cairo_image_surface_create(cairo_format, device->GetSizeX(), device->GetSizeY()));
	VCairoContext img_cr(cairo_create(img_surface()));
	//��������� ���������� ��������� � ������
	Paint(img_cr());
	//��������� ������
	if (!img_cr.Ok())
	{			
		throw VSimpleException(L"������ Cairo", img_cr.ErrorMessage(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (!img_surface.Ok())
	{		
		throw VSimpleException(L"������ Cairo", img_surface.ErrorMessage(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	changed=false;
	return VRawData(img_surface());
}

/** ���������� ��� ������ ���������*/
void VDrawing::OnShow()
{
	//����������� � ��������
}

/** ���������� � ����*/
void VDrawing::SaveVisible(const char * const file_name, FILE_FORMAT format)
{
	switch (format)
	{
		case SVG:
			SaveSVG(file_name);
			break;
		case PDF:
			SavePDF(file_name);
			break;
		case PNG:
			SavePNG(file_name);
			break;		
		default:
			//����������
			break;
	}
}

/** ���������� ��������� ������� - ����������� � ��������*/
void VDrawing::SaveDocument(const char * const file_name, FILE_FORMAT format){};

/** ���������� SVG*/
void VDrawing::SaveSVG(const char * const file_name)
{		
	cairo_surface_t * svg_surface;	
	cairo_t * svg_cr;		
	svg_surface = cairo_svg_surface_create(file_name, GetVisibleWidth(), GetVisibleHeight());
	svg_cr = cairo_create(svg_surface);		
	Paint(svg_cr);	
	cairo_destroy(svg_cr);
	cairo_surface_destroy(svg_surface);
}

/** ���������� PNG � ����*/
void VDrawing::SavePNG(const char * const file_name)
{			
	cairo_surface_t * img_surface;	
	cairo_t * img_cr;
	img_surface = cairo_image_surface_create(cairo_format, GetVisibleWidth(), GetVisibleHeight());
	img_cr = cairo_create(img_surface);		
	Paint(img_cr);					
	cairo_status_t st = cairo_surface_write_to_png(img_surface, file_name);	
	cairo_destroy(img_cr);
	cairo_surface_destroy(img_surface);	
}

/** Callback ������� ��� ���������� PNG � �����*/
cairo_status_t write_png_to_stream (void *closure, unsigned char *data, unsigned int length)
{
	std::ostream * out=(std::ostream *)closure;
	out->write((char*)data, length);
	if (out->bad())
		return CAIRO_STATUS_WRITE_ERROR;	
    return CAIRO_STATUS_SUCCESS;
};

/** ���������� PNG � �����*/
void VDrawing::SaveStreamPNG(std::ostream& in_outstream)
{
	cairo_surface_t * img_surface;	
	cairo_t * img_cr;
	img_surface = cairo_image_surface_create(cairo_format, GetVisibleWidth(), GetVisibleHeight());
	img_cr = cairo_create(img_surface);
	Paint(img_cr);			
	cairo_surface_write_to_png_stream(img_surface, (cairo_write_func_t)write_png_to_stream, &in_outstream);

	//��������� ������
	cairo_status_t status=cairo_status(img_cr);
	if (status!=CAIRO_STATUS_SUCCESS)
	{			
		throw VSimpleException(L"������ Cairo", string_to_wstring(cairo_status_to_string(status)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__, string_to_wstring(typeid(*this).name()));
	}	
	status=cairo_surface_status(img_surface);
	if (status!=CAIRO_STATUS_SUCCESS)
	{		
		throw VSimpleException(L"������ Cairo", string_to_wstring(cairo_status_to_string(status)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
	//����������� ������
	cairo_destroy(img_cr);
	cairo_surface_destroy(img_surface);
}

/** ���������� PDF*/
void VDrawing::SavePDF(const char * const file_name)
{	
	cairo_surface_t * pdf_surface;	
	cairo_t * pdf_cr;		
	pdf_surface = cairo_pdf_surface_create(file_name, GetVisibleWidth(), GetVisibleHeight());	
	pdf_cr = cairo_create(pdf_surface);				
	Paint(pdf_cr);			
	cairo_destroy(pdf_cr);
	cairo_surface_destroy(pdf_surface);
}

/** ������������� ������ ������� �����*/
void VDrawing::SetVisibleSize(const double width, const double height)
{
	visible_width=width;
	visible_height=height;
}

/** ���������� true ���� ������� ��������� � ������� �����������*/
bool VDrawing::Changed()
{	
	return changed;
}

/** ��������� ������ �� �������*/
void VDrawing::Subscribe(uint32_t in_event_code, VEventHandlerPtr in_event_handler)
{
	subscribers.insert(subscriber_t(in_event_code, in_event_handler));
};	

/** ���������� ������� ���� �����������*/
bool VDrawing::ProcessEvent(const VEvent& in_event)
{	
	std::pair<subscribers_itr, subscribers_itr> range;
	range=subscribers.equal_range(in_event.GetCode());
	for (subscribers_itr i=range.first; i!=range.second; ++i)
	{
		changed = changed | (*i).second->ProcessEvent(in_event);
		if (in_event.GetHandled())
			return changed;
	}
	return changed;
}

/** �������� ����*/
void VDrawing::OnClose(){};

/** ���������� � ������ �������*/
void VDrawing::ScrollToHorz(double in_value){};

//---------------------------------------- ������ ������ VPDFDocument ------------------------------------------

/// �����������
VPDFDocument::VPDFDocument(const std::wstring& in_filename, double in_width, double in_height) : 
	surface(cairo_pdf_surface_create(wstring_to_string(in_filename).c_str(), in_width, in_height)),
	context(cairo_create(surface()))
{
	//��������� ������
	if (!context.Ok())
	{			
		throw VSimpleException(L"������ Cairo", context.ErrorMessage(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (!surface.Ok())
	{			
		throw VSimpleException(L"������ Cairo", surface.ErrorMessage(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

/// ��������� ��������
void VPDFDocument::Paint(cairo_surface_t * in_surface)
{
	cairo_set_source_surface(context(), in_surface, 0, 0);
	cairo_paint(context());
}

/// ��������� ��������
void VPDFDocument::AddPage(cairo_surface_t * in_surface)
{
	Paint(in_surface);
	AddPage();
}

/// ���������� ��������
cairo_t * VPDFDocument::GetContext()
{
	return context();
}
/// ��������� ��������
void VPDFDocument::AddPage()
{
	cairo_surface_show_page(surface());
	//��������� ������
	if (!context.Ok())
	{			
		throw VSimpleException(L"������ Cairo", context.ErrorMessage(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (!surface.Ok())
	{			
		throw VSimpleException(L"������ Cairo", surface.ErrorMessage(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

//------------------------------------- ��������������� ������� ------------------------------------------------

/** ������������� ����*/
void CairoSetColor(cairo_t * in_cr, const VColor& in_color)
{
	cairo_set_source_rgba(in_cr, static_cast<double>(in_color.Red())/255, static_cast<double>(in_color.Green())/255,
		static_cast<double>(in_color.Blue())/255, static_cast<double>(in_color.Alpha())/255);
}

/** ������������� ������� �����*/
void CairoSetLineWidth(cairo_t * in_cr , double in_width)
{
	//������� ����� �� ����� �������
	cairo_set_line_width(in_cr, std::max<double>(1.0, in_width));
}

/** ������������� �����*/
void CairoSetFont(cairo_t * in_cr, const VFont& in_font)
{
	cairo_select_font_face (in_cr, in_font.GetFace().c_str(), in_font.GetSlant(), in_font.GetWeight());
	cairo_set_font_size (in_cr, in_font.GetSize());

	cairo_font_options_t *opt;
	opt = cairo_font_options_create ();
	cairo_get_font_options (in_cr, opt);
	cairo_font_options_set_antialias (opt, font_antialias);
	cairo_set_font_options (in_cr, opt);
	cairo_font_options_destroy (opt);

}

/** ������������� ����� ��������*/
void CairoSetDash(cairo_t * in_cr, double in_on, double in_off)
{
	
	double dashes[]={std::max<double>(in_on, 1.0), std::max<double>(in_off, 1.0)};
	if (in_on==0 && in_off==0)
	{
		cairo_set_dash(in_cr, dashes, 0, 0);
	}
	else
	{
		cairo_set_dash(in_cr, dashes, 2, 0);
	}
}

/** ������� �����*/
void CairoShowText(cairo_t * in_cr, const std::wstring& in_text)
{
	cairo_show_text(in_cr, in_text);
}

/** ������� ����� ��������� ������*/
void CairoShowText(cairo_t * in_cr, const std::wstring& in_text, const VColor& in_color)
{
	cairo_save(in_cr);
	CairoSetColor(in_cr, in_color);
	CairoShowText(in_cr, in_text);
	cairo_restore(in_cr);
}

//----------------------------------- ������ ������ VScaleQueue -----------------------------------

template <class T> void VScaleQueue::InitFromString(const T& in_string, const T& in_separator1, const T& in_separator2)
{
	default_scale=10;				
	std::vector<T> scales_vector;
	if (in_string.size()>0)//���� ��������� ������
	{
		//������� 1 ������� ����� 10/100/10
		if (in_string.find_first_of(in_separator2)!=T::npos)
		{
			boost::algorithm::split(scales_vector, in_string, boost::is_any_of(in_separator2));
			if (scales_vector.size()==3)
			{
				double from=str_to_double(scales_vector.at(0));
				double to=str_to_double(scales_vector.at(1));
				double step=str_to_double(scales_vector.at(2));
				for (double scale=from; scale<=to; scale+=step)
				{						
					AddScale(scale);
				}
			}
		}
		//������� 2 ������� ����� 1;2;5;10;15;20;30
		else
		{
			boost::algorithm::split(scales_vector, in_string, boost::is_any_of(in_separator1));
			for (size_t i=0; i<scales_vector.size(); ++i)
			{
				double scale=str_to_double(scales_vector.at(i));
				AddScale(scale);
			}				
		}			
	}
	else
	{
		InitDefaultScales();
	}		
	current=scales.begin();
}

template void VScaleQueue::InitFromString<std::string>(const std::string &, const std::string &, const std::string & );
template void VScaleQueue::InitFromString<std::wstring>(const std::wstring &, const std::wstring &, const std::wstring & );
