
#include "mathlib.h"
#include "axes.h"


//----------------------------- ������ ������ VAxes ----------------------------------------

/** �����������*/
VAxes::VAxes(VAreaPtr in_area, VDrawingDevicePtr in_device)
{	
	if (in_area && in_device)
	{
		area=in_area;
		device=in_device;		
		relative=false;		
		legend_x=L"";
		legend_y=L"";
		origin_x=0;
		origin_y=50;
		min_visible_y=DBL_MAX;
		max_visible_y=(0-DBL_MAX);
		sign_x=1.0;
		sign_y=1.0;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ��������� �� �������*/
VAreaPtr VAxes::GetArea() const
{
	return area;
}

/** ���������� ������ �������*/
double VAxes::GetWidth() const
{
	return area->GetWidth();
}

/** ���������� ������ �������*/
double VAxes::GetHeight() const
{
	return area->GetHeight();
}

/** ���������� ������ ��������� �� ��� X*/
void VAxes::SetStartX(double in_start_x)
{
	start_x=in_start_x;	
}

/** ���������� ������� �� ��� X*/
void VAxes::SetScaleX(double in_scale_x)
{
	scales_x.SetScale(in_scale_x);
}

/** ���������� ������ ��������� �� ��� Y*/
void VAxes::SetStartY(double in_start_y)
{
	start_y=in_start_y;	
}

/** ���������� ������� �� ��� Y*/
void VAxes::SetScaleY(double in_scale_y)
{
	scales_y.SetScale(in_scale_y);	
}

/** ���������� ������� �� ��� X*/
double VAxes::GetScaleX() const
{		
	return scales_x.GetScale();
}
/** ���������� ������� �� ��� Y*/
double VAxes::GetScaleY() const
{
	return scales_y.GetScale();
}

/** ���������� ������ �� ��� X*/
double VAxes::GetStartX()
{
	return start_x;
}

/** ���������� ������ �� ��� Y*/
double VAxes::GetStartY()
{
	return start_y;
}

/** ������������� ��������� ������ ��������� ������������ ������� �� ��� X (� ���������)*/
void VAxes::SetOriginX(double in_origin_x)
{	
	origin_x=in_origin_x;
}

/** ���������� ��������� ������ ��������� ������������ ������� �� ��� X (� ���������)*/
double VAxes::GetOriginX()
{
	return origin_x;
}

/** ������������� ��������� ������ ��������� ������������ ������� �� ��� Y (� ���������)*/
void VAxes::SetOriginY(double in_origin_y)
{
	origin_y=in_origin_y;
}

/** ���������� ��������� ������ ��������� ������������ ������� �� ��� Y (� ���������)*/
double VAxes::GetOriginY()
{
	return origin_y;
}

/** ���������� ������� �� ��� X*/
void VAxes::ZoomInX()
{		
	scales_x.PrevScale();
	SetScaleX(scales_x.GetScale());
}

/** ��������� ������� �� ��� X*/
void VAxes::ZoomOutX()
{			
	scales_x.NextScale();
	SetScaleX(scales_x.GetScale());
}


/** ���������� ������� �� ��� Y*/
void VAxes::ZoomInY()
{	
	scales_y.PrevScale();
	SetScaleY(scales_y.GetScale());
}

/** ��������� ������� �� ��� Y*/
void VAxes::ZoomOutY()
{		
	scales_y.NextScale();
	SetScaleY(scales_y.GetScale());
}

/** ���������� ������ ������� �� ��� X*/
void VAxes::SetBestScaleX(double in_min_x, double in_max_x)
{
	SetStartX((in_min_x+in_max_x)/2);
	SetOriginX(50);
	for (size_t i=0; i<scales_x.GetCount(); ++i)
	{		
		SetScaleX(scales_x.GetScale(i));
		//���� �������� � ������� � ��������� ������� ��������� ���� �������
		if (ParamToAreaX(in_min_x)>=0 && ParamToAreaX(in_max_x)<=GetWidth())
			break;			
	}	
}

/** ���������� ������ �������*/
void VAxes::SetAutoScaleX()
{
	SetBestScaleX(GetMinDataX(), GetMaxDataX());
}

/** ���������� ������ �������*/
void VAxes::SetAutoScaleY()
{
	SetBestScaleY(GetMinDataY(), GetMaxDataY());
}

/// ���������� true ���� �������� ���������� �������������� �������
bool VAxes::AllowAutoScaleY() const
{
	//FixMe!!! ���������� ��� ����� GetRealtive �� ���������c�
	return !relative && GetMinDataY()<GetMaxDataY();
}

/** ���������� ������� �� ��������� �� ��� X*/
void VAxes::SetDefaultScaleX()
{
	SetBestScaleX(GetMinVisibleY(), GetMaxVisibleY());
}

/** ���������� ������� �� ��������� �� ��� Y*/
void VAxes::SetDefaultScaleY()
{
	SetBestScaleY(GetMinVisibleY(), GetMaxVisibleY());
}

/// ���������� true ���� �������� ���������� ������� �� ���������
bool VAxes::AllowDefaultScaleY() const
{
	//FixMe!!! ���������� ��� ����� GetRealtive �� ���������c�
	return !relative && GetMinVisibleY()<GetMaxVisibleY();
}

/** ���������� ������ �������*/
void VAxes::SetBestScaleY(double in_min_y, double in_max_y)
{
	SetStartY((in_min_y+in_max_y)/2);
	SetOriginY(50);
	for (size_t i=0; i<scales_y.GetCount(); ++i)
	{		
		SetScaleY(scales_y.GetScale(i));
		//���� �������� � ������� � ��������� ������� ��������� ���� �������
		if (pro::interval<double>(0, GetHeight()).contains(ParamToAreaY(in_max_y)) && pro::interval<double>(0, GetHeight()).contains(ParamToAreaY(in_min_y)))
			break;			
	}	
}

/** �������� ������������ �������� �� ��� X*/
double VAxes::GetMaxX() const
{	
	if (sign_x>0)
		return AreaToParamX(GetWidth());
	else
		return AreaToParamX(0);
}

/** �������� ����������� �������� �� ��� X*/
double VAxes::GetMinX() const
{	
	if (sign_x>0)
		return AreaToParamX(0);
	else
		return AreaToParamX(GetWidth());
}

/** �������� ������������ ������� �������� �� ��� Y*/
double VAxes::GetMaxY() const
{	
	if (sign_y>0)
		return AreaToParamY(0);
	else
		return AreaToParamY(GetHeight());
}

/** �������� ����������� ������� �������� �� ��� Y*/
double VAxes::GetMinY() const
{	
	if (sign_y>0)
		return AreaToParamY(GetHeight());
	else
		return AreaToParamY(0);
}

/** �������������� ���������� �� ������� ��������� � ������� ������� �� ��� X*/
double VAxes::ParamToAreaX(double in_x) const
{			
	if (sign_x>0)
		return round(origin_x*GetWidth()*0.01+DrawingToDeviceX(ParamToDrawingX(in_x-start_x)));	
	else
		return round((100.0-origin_x)*GetWidth()*0.01+DrawingToDeviceX(ParamToDrawingX(start_x-in_x)));	
}

/** �������������� ���������� �� ������� ������� � ������� ��������� �� ��� X*/
double VAxes::AreaToParamX(double in_x) const
{		
	if (sign_x>0)
		return DrawingToParamX(DeviceToDrawingX(-origin_x*GetWidth()*0.01+in_x))+start_x;
	else
		return DrawingToParamX(DeviceToDrawingX((100.0-origin_x)*GetWidth()*0.01-in_x))+start_x;
}

/** �������������� ���������� �� ������� ��������� � ������� ������� �� ��� Y*/
double VAxes::ParamToAreaY(double in_y) const
{		
	if (sign_y>0)
	{
		double y=origin_y*GetHeight()*0.01-DrawingToDeviceY(ParamToDrawingY(in_y-start_y));	
		if (relative && GetHeight()>0)
		{
			y=fmod(y,GetHeight());
			if (y<0) y+=GetHeight();
		}	
		return round(y);
	}
	else
	{
		double y=origin_y*GetHeight()*0.01+DrawingToDeviceY(ParamToDrawingY(in_y-start_y));
		return round(y); 
	}
}

/** �������������� ���������� �� ������� ������� � ������� ��������� �� ��� Y*/
double VAxes::AreaToParamY(double in_y) const
{			
	if (relative)
		return 0;
	else
	{
		if (sign_y>0)
			return DrawingToParamY(DeviceToDrawingY(origin_y*GetHeight()*0.01-in_y))+start_y;
		else
			return -DrawingToParamY(DeviceToDrawingY(origin_y*GetHeight()*0.01-in_y))+start_y;
	}
}

/** ������������� ���������� �� ��������� � ����������� �� ��� X*/
double VAxes::ParamToDrawingX(double in_value) const
{
	return in_value/GetScaleX();
}
/** ������������� ���������� �� ����������� � �������� �� ��� X*/
double VAxes::DrawingToParamX(double in_value) const
{
	return in_value*GetScaleX();
}
/** ������������� ���������� �� ����������� �� ���������� �� ��� X*/
double VAxes::DrawingToDeviceX(double in_value) const
{
	return device->DrawingToDeviceX(in_value);
}
/** ������������� ���������� �� ���������� � ����������� �� ��� X*/
double VAxes::DeviceToDrawingX(double in_value) const
{
	return device->DeviceToDrawingY(in_value);
}

/** ������������� ���������� �� ��������� � ����������� �� ��� Y*/
double VAxes::ParamToDrawingY(double in_value) const
{
	return in_value/GetScaleY();
}

/** ������������� ���������� �� ����������� � �������� �� ��� Y*/
double VAxes::DrawingToParamY(double in_value) const
{
	return in_value*GetScaleY();
}

/** ������������� ���������� �� ����������� �� ���������� �� ��� Y*/
double VAxes::DrawingToDeviceY(double in_value) const
{
	return device->DrawingToDeviceY(in_value);
}

/** ������������� ���������� �� ���������� � ����������� �� ��� Y*/
double VAxes::DeviceToDrawingY(double in_value) const
{
	return device->DeviceToDrawingY(in_value);
}

/** ���������� ������� ��������*/
void VAxes::SetRelative(bool in_relative)
{
	relative=in_relative;
}

/** ���������� ������� ��������*/
bool VAxes::GetRelative() const
{
	return relative;
}

/** ���������� ������ ��������� ��������� �� ��� X*/
const VScaleQueue& VAxes::GetAvailScaleX() const
{
	return scales_x;
}

/** ������������� ������ ���������� ��������� �� ��� X*/
void VAxes::SetAvailScaleX(const VScaleQueue& in_scales_x)
{
	scales_x=in_scales_x;	
}

/** ���������� ������ ��������� ��������� �� ��� Y*/
const VScaleQueue& VAxes::GetAvailScaleY() const
{
	return scales_y;
}

/** ������������� ������ ���������� ��������� �� ��� Y*/
void VAxes::SetAvailScaleY(const VScaleQueue& in_scales_y)
{
	scales_y=in_scales_y;	
}


/** ���������� ������� ��� X*/
void VAxes::SetLegendX(const std::wstring& in_legend_x)
{
	legend_x=in_legend_x;
}

/** ���������� ������� ��� X*/
std::wstring VAxes::GetLegendX()
{
	return legend_x;
}

/** ���������� ������� ��� Y*/
void VAxes::SetLegendY(const std::wstring& in_legend_y)
{
	legend_y=in_legend_y;
}

/** ���������� ������� ��� Y*/
std::wstring VAxes::GetLegendY()
{
	return legend_y;
}

/** ���������� ��������� ������*/
void VAxes::AddDataProvider(VDataProviderPtr in_data_provider)
{
	data.push_back(in_data_provider);
}

/** ���������� ������� ������ �� X*/
double VAxes::GetMinDataX() const
{
	double min_x=DBL_MAX;
	for (data_itr i=data.begin(); i!=data.end(); ++i)
	{
		min_x=std::min<double>(min_x, (*i)->GetMinX());
	}
	return min_x;
}

/** ���������� �������� ������ �� X*/
double VAxes::GetMaxDataX() const
{
	double max_x=(0-DBL_MAX);
	for (data_itr i=data.begin(); i!=data.end(); ++i)
	{
		max_x=std::max<double>(max_x, (*i)->GetMaxX());
	}
	return max_x;
}

/** ���������� ������� ������ �� Y*/
double VAxes::GetMinDataY() const
{
	double min_y=DBL_MAX;
	for (data_itr i=data.begin(); i!=data.end(); ++i)
	{
		min_y=std::min<double>(min_y, (*i)->GetMinY());
	}
	return min_y;
}

/** ���������� �������� ������ �� Y*/
double VAxes::GetMaxDataY() const
{
	double max_y=(0-DBL_MAX);
	for (data_itr i=data.begin(); i!=data.end(); ++i)
	{
		max_y=std::max<double>(max_y, (*i)->GetMaxY());
	}
	return max_y;
}

/** ������������� ����������� ������� �������� �� Y*/
void VAxes::SetMinVisibleY(double in_value)
{
	min_visible_y=in_value;
}

/** ���������� ����������� ������� �������� �� Y*/
double VAxes::GetMinVisibleY() const
{
	return min_visible_y;
}

/** ������������� ������������ ������� �������� �� Y*/
void VAxes::SetMaxVisibleY(double in_value)
{
	max_visible_y=in_value;
}

/** ���������� ������������ ������� �������� �� Y*/
double VAxes::GetMaxVisibleY() const
{
	return max_visible_y;
}

/** ������������� ����������� ������� �������� �� X*/
void VAxes::SetMinVisibleX(double in_value)
{
	min_visible_x=in_value;
}

/** ������������� ������������ ������� �������� �� X*/
void VAxes::SetMaxVisibleX(double in_value)
{
	max_visible_x=in_value;
}

/** �������� �� X*/
bool VAxes::GetInverseX() const
{
	return sign_x<0;
}

/** �������� �� X*/
void VAxes::SetInverseX(bool in_inverse_x)
{
	sign_x=in_inverse_x ? -1.0 : 1.0;
}

/** �������� �� Y*/
bool VAxes::GetInverseY() const
{
	return sign_y<0;
}

/** �������� �� X*/
void VAxes::SetInverseY(bool in_inverse_y)
{
	sign_y=in_inverse_y ? -1.0 : 1.0;
}