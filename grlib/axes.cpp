
#include "mathlib.h"
#include "axes.h"


//----------------------------- Методы класса VAxes ----------------------------------------

/** Конструктор*/
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

/** Возвращает указатель на область*/
VAreaPtr VAxes::GetArea() const
{
	return area;
}

/** Возвращает ширину области*/
double VAxes::GetWidth() const
{
	return area->GetWidth();
}

/** Возвращает высоту области*/
double VAxes::GetHeight() const
{
	return area->GetHeight();
}

/** Установить начало координат по оси X*/
void VAxes::SetStartX(double in_start_x)
{
	start_x=in_start_x;	
}

/** Установить масштаб по оси X*/
void VAxes::SetScaleX(double in_scale_x)
{
	scales_x.SetScale(in_scale_x);
}

/** Установить начало координат по оси Y*/
void VAxes::SetStartY(double in_start_y)
{
	start_y=in_start_y;	
}

/** Установить масштаб по оси Y*/
void VAxes::SetScaleY(double in_scale_y)
{
	scales_y.SetScale(in_scale_y);	
}

/** Возвращает масштаб по оси X*/
double VAxes::GetScaleX() const
{		
	return scales_x.GetScale();
}
/** Возвращает масштаб по оси Y*/
double VAxes::GetScaleY() const
{
	return scales_y.GetScale();
}

/** Возвращает начало по оси X*/
double VAxes::GetStartX()
{
	return start_x;
}

/** Возвращает начало по оси Y*/
double VAxes::GetStartY()
{
	return start_y;
}

/** Устанавливает положение начала координат относительно области по оси X (в процентах)*/
void VAxes::SetOriginX(double in_origin_x)
{	
	origin_x=in_origin_x;
}

/** Возвращает положение начала координат относительно области по оси X (в процентах)*/
double VAxes::GetOriginX()
{
	return origin_x;
}

/** Устанавливает положение начала координат относительно области по оси Y (в процентах)*/
void VAxes::SetOriginY(double in_origin_y)
{
	origin_y=in_origin_y;
}

/** Возвращает положение начала координат относительно области по оси Y (в процентах)*/
double VAxes::GetOriginY()
{
	return origin_y;
}

/** Предыдущий масштаб по оси X*/
void VAxes::ZoomInX()
{		
	scales_x.PrevScale();
	SetScaleX(scales_x.GetScale());
}

/** Следующий масштаб по оси X*/
void VAxes::ZoomOutX()
{			
	scales_x.NextScale();
	SetScaleX(scales_x.GetScale());
}


/** Предыдущий масштаб по оси Y*/
void VAxes::ZoomInY()
{	
	scales_y.PrevScale();
	SetScaleY(scales_y.GetScale());
}

/** Следующий масштаб по оси Y*/
void VAxes::ZoomOutY()
{		
	scales_y.NextScale();
	SetScaleY(scales_y.GetScale());
}

/** Установить лучший масштаб по оси X*/
void VAxes::SetBestScaleX(double in_min_x, double in_max_x)
{
	SetStartX((in_min_x+in_max_x)/2);
	SetOriginX(50);
	for (size_t i=0; i<scales_x.GetCount(); ++i)
	{		
		SetScaleX(scales_x.GetScale(i));
		//Если максимум и минимум в переделах области оставляем этот масштаб
		if (ParamToAreaX(in_min_x)>=0 && ParamToAreaX(in_max_x)<=GetWidth())
			break;			
	}	
}

/** Установить лучший масштаб*/
void VAxes::SetAutoScaleX()
{
	SetBestScaleX(GetMinDataX(), GetMaxDataX());
}

/** Установить лучший масштаб*/
void VAxes::SetAutoScaleY()
{
	SetBestScaleY(GetMinDataY(), GetMaxDataY());
}

/// Возвращает true если возможно установить автоматический масштаб
bool VAxes::AllowAutoScaleY() const
{
	//FixMe!!! доработать так чтобы GetRealtive не требовалоcь
	return !relative && GetMinDataY()<GetMaxDataY();
}

/** Установить масштаб по умолчанию по оси X*/
void VAxes::SetDefaultScaleX()
{
	SetBestScaleX(GetMinVisibleY(), GetMaxVisibleY());
}

/** Установить масштаб по умолчанию по оси Y*/
void VAxes::SetDefaultScaleY()
{
	SetBestScaleY(GetMinVisibleY(), GetMaxVisibleY());
}

/// Возвращает true если возможно установить масштаб по умолчанию
bool VAxes::AllowDefaultScaleY() const
{
	//FixMe!!! доработать так чтобы GetRealtive не требовалоcь
	return !relative && GetMinVisibleY()<GetMaxVisibleY();
}

/** Установить лучший масштаб*/
void VAxes::SetBestScaleY(double in_min_y, double in_max_y)
{
	SetStartY((in_min_y+in_max_y)/2);
	SetOriginY(50);
	for (size_t i=0; i<scales_y.GetCount(); ++i)
	{		
		SetScaleY(scales_y.GetScale(i));
		//Если максимум и минимум в переделах области оставляем этот масштаб
		if (pro::interval<double>(0, GetHeight()).contains(ParamToAreaY(in_max_y)) && pro::interval<double>(0, GetHeight()).contains(ParamToAreaY(in_min_y)))
			break;			
	}	
}

/** Получить максимальное значение по оси X*/
double VAxes::GetMaxX() const
{	
	if (sign_x>0)
		return AreaToParamX(GetWidth());
	else
		return AreaToParamX(0);
}

/** Получить минимальное значение по оси X*/
double VAxes::GetMinX() const
{	
	if (sign_x>0)
		return AreaToParamX(0);
	else
		return AreaToParamX(GetWidth());
}

/** Получить максимальное видимое значение по оси Y*/
double VAxes::GetMaxY() const
{	
	if (sign_y>0)
		return AreaToParamY(0);
	else
		return AreaToParamY(GetHeight());
}

/** Получить минимальное видимое значение по оси Y*/
double VAxes::GetMinY() const
{	
	if (sign_y>0)
		return AreaToParamY(GetHeight());
	else
		return AreaToParamY(0);
}

/** Преобразование координаты из системы параметра в систему области по оси X*/
double VAxes::ParamToAreaX(double in_x) const
{			
	if (sign_x>0)
		return round(origin_x*GetWidth()*0.01+DrawingToDeviceX(ParamToDrawingX(in_x-start_x)));	
	else
		return round((100.0-origin_x)*GetWidth()*0.01+DrawingToDeviceX(ParamToDrawingX(start_x-in_x)));	
}

/** Преобразование координаты из системы области в систему параметра по оси X*/
double VAxes::AreaToParamX(double in_x) const
{		
	if (sign_x>0)
		return DrawingToParamX(DeviceToDrawingX(-origin_x*GetWidth()*0.01+in_x))+start_x;
	else
		return DrawingToParamX(DeviceToDrawingX((100.0-origin_x)*GetWidth()*0.01-in_x))+start_x;
}

/** Преобразование координаты из системы параметра в систему области по оси Y*/
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

/** Преобразование координаты из системы области в систему параметра по оси Y*/
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

/** Пересчитывает координаты из параметра в изображение по оси X*/
double VAxes::ParamToDrawingX(double in_value) const
{
	return in_value/GetScaleX();
}
/** Пересчитывает координаты из изображения в параметр по оси X*/
double VAxes::DrawingToParamX(double in_value) const
{
	return in_value*GetScaleX();
}
/** Пересчитывает координаты из изображения на устройство по оси X*/
double VAxes::DrawingToDeviceX(double in_value) const
{
	return device->DrawingToDeviceX(in_value);
}
/** Пересчитывает координаты из устройства в изображение по оси X*/
double VAxes::DeviceToDrawingX(double in_value) const
{
	return device->DeviceToDrawingY(in_value);
}

/** Пересчитывает координаты из параметра в изображение по оси Y*/
double VAxes::ParamToDrawingY(double in_value) const
{
	return in_value/GetScaleY();
}

/** Пересчитывает координаты из изображения в параметр по оси Y*/
double VAxes::DrawingToParamY(double in_value) const
{
	return in_value*GetScaleY();
}

/** Пересчитывает координаты из изображения на устройство по оси Y*/
double VAxes::DrawingToDeviceY(double in_value) const
{
	return device->DrawingToDeviceY(in_value);
}

/** Пересчитывает координаты из устройства в изображение по оси Y*/
double VAxes::DeviceToDrawingY(double in_value) const
{
	return device->DeviceToDrawingY(in_value);
}

/** Установить признак смещения*/
void VAxes::SetRelative(bool in_relative)
{
	relative=in_relative;
}

/** Возвращает признак смещения*/
bool VAxes::GetRelative() const
{
	return relative;
}

/** Возвращает список доступных масштабов по оси X*/
const VScaleQueue& VAxes::GetAvailScaleX() const
{
	return scales_x;
}

/** Устанавливает список допустимых масштабов по оси X*/
void VAxes::SetAvailScaleX(const VScaleQueue& in_scales_x)
{
	scales_x=in_scales_x;	
}

/** Возвращает список доступных масштабов по оси Y*/
const VScaleQueue& VAxes::GetAvailScaleY() const
{
	return scales_y;
}

/** Устанавливает список допустимых масштабов по оси Y*/
void VAxes::SetAvailScaleY(const VScaleQueue& in_scales_y)
{
	scales_y=in_scales_y;	
}


/** Установить легенду оси X*/
void VAxes::SetLegendX(const std::wstring& in_legend_x)
{
	legend_x=in_legend_x;
}

/** Возвращает легенду оси X*/
std::wstring VAxes::GetLegendX()
{
	return legend_x;
}

/** Установить легенду оси Y*/
void VAxes::SetLegendY(const std::wstring& in_legend_y)
{
	legend_y=in_legend_y;
}

/** Возвращает легенду оси Y*/
std::wstring VAxes::GetLegendY()
{
	return legend_y;
}

/** Добавление источника данных*/
void VAxes::AddDataProvider(VDataProviderPtr in_data_provider)
{
	data.push_back(in_data_provider);
}

/** Возвращает минимум данных по X*/
double VAxes::GetMinDataX() const
{
	double min_x=DBL_MAX;
	for (data_itr i=data.begin(); i!=data.end(); ++i)
	{
		min_x=std::min<double>(min_x, (*i)->GetMinX());
	}
	return min_x;
}

/** Возвращает максимум данных по X*/
double VAxes::GetMaxDataX() const
{
	double max_x=(0-DBL_MAX);
	for (data_itr i=data.begin(); i!=data.end(); ++i)
	{
		max_x=std::max<double>(max_x, (*i)->GetMaxX());
	}
	return max_x;
}

/** Возвращает минимум данных по Y*/
double VAxes::GetMinDataY() const
{
	double min_y=DBL_MAX;
	for (data_itr i=data.begin(); i!=data.end(); ++i)
	{
		min_y=std::min<double>(min_y, (*i)->GetMinY());
	}
	return min_y;
}

/** Возвращает максимум данных по Y*/
double VAxes::GetMaxDataY() const
{
	double max_y=(0-DBL_MAX);
	for (data_itr i=data.begin(); i!=data.end(); ++i)
	{
		max_y=std::max<double>(max_y, (*i)->GetMaxY());
	}
	return max_y;
}

/** Устанавливает минимальное видимое значение по Y*/
void VAxes::SetMinVisibleY(double in_value)
{
	min_visible_y=in_value;
}

/** Возвращает минимальное видимое значение по Y*/
double VAxes::GetMinVisibleY() const
{
	return min_visible_y;
}

/** Устанавливает максимальное видимое значение по Y*/
void VAxes::SetMaxVisibleY(double in_value)
{
	max_visible_y=in_value;
}

/** Возвращает максимальное видимое значение по Y*/
double VAxes::GetMaxVisibleY() const
{
	return max_visible_y;
}

/** Устанавливает минимальное видимое значение по X*/
void VAxes::SetMinVisibleX(double in_value)
{
	min_visible_x=in_value;
}

/** Устанавливает максимальное видимое значение по X*/
void VAxes::SetMaxVisibleX(double in_value)
{
	max_visible_x=in_value;
}

/** Инверсия по X*/
bool VAxes::GetInverseX() const
{
	return sign_x<0;
}

/** Инверсия по X*/
void VAxes::SetInverseX(bool in_inverse_x)
{
	sign_x=in_inverse_x ? -1.0 : 1.0;
}

/** Инверсия по Y*/
bool VAxes::GetInverseY() const
{
	return sign_y<0;
}

/** Инверсия по X*/
void VAxes::SetInverseY(bool in_inverse_y)
{
	sign_y=in_inverse_y ? -1.0 : 1.0;
}