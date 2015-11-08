#pragma once

#include "VDrawing.h"




/** Система координат*/
class VAxes : public VActiveObject
{
private:
	/** Указатель на область*/
	VAreaPtr area;	
	/** Признак - смещать ли точки (как для профиля) FixMe!!! найти другой термин*/
	bool relative;
	/** Указатель на устройство*/
	VDrawingDevicePtr device;
	
	//Начало координат

	/** Начало координат по оси х*/
	double start_x;
	/** Начало координат по оси y*/
	double start_y;
	/** Положение начала координат относительно области по оси X (в процентах)*/
	double origin_x;
	/** Положение начала координат относительно области по оси Y (в процентах)*/
	double origin_y;	

	/** Список допустимых масштабов по оси X*/	
	VScaleQueue scales_x;
	/** Список допустимых масштабов по оси Y*/	
	VScaleQueue scales_y;

	/** Легенда оси X*/
	std::wstring legend_x;
	/** Легенда оси Y*/
	std::wstring legend_y;

	/** Минимальное отображаемое значение по X*/
	double min_visible_x;
	/** Максимальное отображаемое значение по X*/
	double max_visible_x;

	/** Минимальное отображаемое значение по Y*/
	double min_visible_y;
	/** Максимальное отображаемое значение по Y*/
	double max_visible_y;
	
	/** Список источников данных*/
	std::vector<VDataProviderPtr> data;
	typedef std::vector<VDataProviderPtr>::const_iterator data_itr;

	/** Инверсия по X*/	
	double sign_x;
	
	/** Инверсия по Y*/	
	double sign_y;

public:

	/** Конструктор*/
	VAxes(VAreaPtr in_area, VDrawingDevicePtr in_device);	
	/** Возвращает указатель на область*/
	VAreaPtr GetArea() const;
	/** Возвращает ширину области*/
	double GetWidth() const;
	/** Возвращает высоту области*/
	double GetHeight() const;

	/** Установить начало координат по оси X*/
	void SetStartX(double in_start_x);	
	/** Установить начало координат по оси Y*/
	void SetStartY(double in_start_y);
	/** Возвращает начало по оси X*/
	double GetStartX();
	/** Возвращает начало по оси Y*/
	double GetStartY();
	/** Установить масштаб по оси X*/
	void SetScaleX(double in_scale_x);
	/** Установить масштаб по оси Y*/
	void SetScaleY(double in_scale_y);	
	/** Возвращает масштаб по оси X*/
	double GetScaleX() const;
	/** Возвращает масштаб по оси Y*/
	double GetScaleY() const;	
	/** Устанавливает положение начала координат относительно области по оси X (в процентах)*/
	void SetOriginX(double in_origin_x);
	/** Возвращает положение начала координат относительно области по оси X (в процентах)*/
	double GetOriginX();
	/** Устанавливает положение начала координат относительно области по оси Y (в процентах)*/
	void SetOriginY(double in_origin_y);
	/** Возвращает положение начала координат относительно области по оси Y (в процентах)*/
	double GetOriginY();
	/** Предыдущий масштаб по оси X*/
	void ZoomInX();
	/** Следующий масштаб по оси X*/
	void ZoomOutX();
	/** Предыдущий масштаб по оси Y*/
	void ZoomInY();
	/** Следующий масштаб по оси Y*/
	void ZoomOutY();	
	/** Установить лучший масштаб по оси X*/
	void SetBestScaleX(double in_min_x, double in_max_x);	
	/** Установить автоматический масштаб по оси X*/
	void SetAutoScaleX();
	/** Установить автоматический масштаб по оси Y*/
	void SetAutoScaleY();
	/// Возвращает true если возможно установить автоматический масштаб
	bool AllowAutoScaleY() const;
	/** Установить масштаб по умолчанию по оси X*/
	void SetDefaultScaleX();
	/** Установить масштаб по умолчанию по оси Y*/
	void SetDefaultScaleY();
	/// Возвращает true если возможно установить масштаб по умолчанию
	bool AllowDefaultScaleY() const;
	/** Установить лучший масштаб по оси Y*/
	void SetBestScaleY(double in_min_y, double in_max_y);
	/** Получить максимальное значение по оси X*/
	double GetMaxX() const;
	/** Получить минимальное значение по оси X*/
	double GetMinX() const;
	/** Получить максимальное значение по оси Y*/
	double GetMaxY() const;
	/** Получить минимальное значение по оси Y*/
	double GetMinY() const;	
		
	/** Преобразование координаты из системы параметра в систему области по оси X*/
	double ParamToAreaX(double in_x) const;
	/** Преобразование координаты из системы области в систему параметра по оси X*/
	double AreaToParamX(double in_x) const;
	/** Преобразование координаты из системы параметра в систему области по оси Y*/
	double ParamToAreaY(double in_y) const;	
	/** Преобразование координаты из системы области в систему параметра по оси Y*/
	double AreaToParamY(double in_y) const;		
	
	/** Пересчитывает координаты из параметра в изображение по оси X*/
	double ParamToDrawingX(double in_value) const;
	/** Пересчитывает координаты из изображения в параметр по оси X*/
	double DrawingToParamX(double in_value) const;
	/** Пересчитывает координаты из изображения на устройство по оси X*/
	double DrawingToDeviceX(double in_value) const;
	/** Пересчитывает координаты из устройства в изображение по оси X*/
	double DeviceToDrawingX(double in_value) const;

	/** Пересчитывает координаты из параметра в изображение по оси Y*/
	double ParamToDrawingY(double in_value) const;
	/** Пересчитывает координаты из изображения в параметр по оси Y*/
	double DrawingToParamY(double in_value) const;
	/** Пересчитывает координаты из изображения на устройство по оси Y*/
	double DrawingToDeviceY(double in_value) const;
	/** Пересчитывает координаты из устройства в изображение по оси Y*/
	double DeviceToDrawingY(double in_value) const;
	

	/** Установить признак смещения*/
	void SetRelative(bool in_relative);	
	/** Возвращает признак смещения*/
	bool GetRelative() const;				

	/** Возвращает список доступных масштабов по оси X*/	
	const VScaleQueue& GetAvailScaleX() const;
	/** Устанавливает список допустимых масштабов по оси X*/	
	void SetAvailScaleX(const VScaleQueue& in_scales_x);

	/** Возвращает список доступных масштабов по оси Y*/	
	const VScaleQueue& GetAvailScaleY() const;
	/** Устанавливает список допустимых масштабов по оси Y*/	
	void SetAvailScaleY(const VScaleQueue& in_scales_y);

	/** Установить легенду оси X*/
	void SetLegendX(const std::wstring& in_legend_x);
	/** Возвращает легенду оси X*/
	std::wstring GetLegendX();

	/** Установить легенду оси Y*/
	void SetLegendY(const std::wstring& in_legend_y);
	/** Возвращает легенду оси Y*/
	std::wstring GetLegendY();		

	/** Добавление источника данных*/
	void AddDataProvider(VDataProviderPtr in_data_provider);
	
	/** Возвращает минимум данных по X*/
	double GetMinDataX() const;
	/** Возвращает максимум данных по X*/
	double GetMaxDataX() const;
	/** Возвращает минимум данных по Y*/
	double GetMinDataY() const;
	/** Возвращает максимум данных по Y*/
	double GetMaxDataY() const;

	/** Устанавливает минимальное видимое значение по Y*/
	void SetMinVisibleY(double in_value);
	/** Возвращает минимальное видимое значение по Y*/
	double GetMinVisibleY() const;
	/** Устанавливает максимальное видимое значение по Y*/
	void SetMaxVisibleY(double in_value);
	/** Возвращает максимальное видимое значение по Y*/
	double GetMaxVisibleY() const;

	/** Устанавливает минимальное видимое значение по X*/
	void SetMinVisibleX(double in_value);
	/** Устанавливает максимальное видимое значение по X*/
	void SetMaxVisibleX(double in_value);


	/** Инверсия по X*/
	bool GetInverseX() const;
	/** Инверсия по X*/
	void SetInverseX(bool in_inverse_x);
	
	/** Инверсия по Y*/
	bool GetInverseY() const;
	/** Инверсия по X*/
	void SetInverseY(bool in_inverse_y);
	
	/** Возвращает признак активности*/
	bool GetActive() const {return GetArea()->GetActive();};
};
typedef boost::shared_ptr<VAxes> VAxesPtr;
typedef boost::weak_ptr<VAxes> VAxesWeakPtr;
