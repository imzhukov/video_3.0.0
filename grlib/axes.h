#pragma once

#include "VDrawing.h"




/** ������� ���������*/
class VAxes : public VActiveObject
{
private:
	/** ��������� �� �������*/
	VAreaPtr area;	
	/** ������� - ������� �� ����� (��� ��� �������) FixMe!!! ����� ������ ������*/
	bool relative;
	/** ��������� �� ����������*/
	VDrawingDevicePtr device;
	
	//������ ���������

	/** ������ ��������� �� ��� �*/
	double start_x;
	/** ������ ��������� �� ��� y*/
	double start_y;
	/** ��������� ������ ��������� ������������ ������� �� ��� X (� ���������)*/
	double origin_x;
	/** ��������� ������ ��������� ������������ ������� �� ��� Y (� ���������)*/
	double origin_y;	

	/** ������ ���������� ��������� �� ��� X*/	
	VScaleQueue scales_x;
	/** ������ ���������� ��������� �� ��� Y*/	
	VScaleQueue scales_y;

	/** ������� ��� X*/
	std::wstring legend_x;
	/** ������� ��� Y*/
	std::wstring legend_y;

	/** ����������� ������������ �������� �� X*/
	double min_visible_x;
	/** ������������ ������������ �������� �� X*/
	double max_visible_x;

	/** ����������� ������������ �������� �� Y*/
	double min_visible_y;
	/** ������������ ������������ �������� �� Y*/
	double max_visible_y;
	
	/** ������ ���������� ������*/
	std::vector<VDataProviderPtr> data;
	typedef std::vector<VDataProviderPtr>::const_iterator data_itr;

	/** �������� �� X*/	
	double sign_x;
	
	/** �������� �� Y*/	
	double sign_y;

public:

	/** �����������*/
	VAxes(VAreaPtr in_area, VDrawingDevicePtr in_device);	
	/** ���������� ��������� �� �������*/
	VAreaPtr GetArea() const;
	/** ���������� ������ �������*/
	double GetWidth() const;
	/** ���������� ������ �������*/
	double GetHeight() const;

	/** ���������� ������ ��������� �� ��� X*/
	void SetStartX(double in_start_x);	
	/** ���������� ������ ��������� �� ��� Y*/
	void SetStartY(double in_start_y);
	/** ���������� ������ �� ��� X*/
	double GetStartX();
	/** ���������� ������ �� ��� Y*/
	double GetStartY();
	/** ���������� ������� �� ��� X*/
	void SetScaleX(double in_scale_x);
	/** ���������� ������� �� ��� Y*/
	void SetScaleY(double in_scale_y);	
	/** ���������� ������� �� ��� X*/
	double GetScaleX() const;
	/** ���������� ������� �� ��� Y*/
	double GetScaleY() const;	
	/** ������������� ��������� ������ ��������� ������������ ������� �� ��� X (� ���������)*/
	void SetOriginX(double in_origin_x);
	/** ���������� ��������� ������ ��������� ������������ ������� �� ��� X (� ���������)*/
	double GetOriginX();
	/** ������������� ��������� ������ ��������� ������������ ������� �� ��� Y (� ���������)*/
	void SetOriginY(double in_origin_y);
	/** ���������� ��������� ������ ��������� ������������ ������� �� ��� Y (� ���������)*/
	double GetOriginY();
	/** ���������� ������� �� ��� X*/
	void ZoomInX();
	/** ��������� ������� �� ��� X*/
	void ZoomOutX();
	/** ���������� ������� �� ��� Y*/
	void ZoomInY();
	/** ��������� ������� �� ��� Y*/
	void ZoomOutY();	
	/** ���������� ������ ������� �� ��� X*/
	void SetBestScaleX(double in_min_x, double in_max_x);	
	/** ���������� �������������� ������� �� ��� X*/
	void SetAutoScaleX();
	/** ���������� �������������� ������� �� ��� Y*/
	void SetAutoScaleY();
	/// ���������� true ���� �������� ���������� �������������� �������
	bool AllowAutoScaleY() const;
	/** ���������� ������� �� ��������� �� ��� X*/
	void SetDefaultScaleX();
	/** ���������� ������� �� ��������� �� ��� Y*/
	void SetDefaultScaleY();
	/// ���������� true ���� �������� ���������� ������� �� ���������
	bool AllowDefaultScaleY() const;
	/** ���������� ������ ������� �� ��� Y*/
	void SetBestScaleY(double in_min_y, double in_max_y);
	/** �������� ������������ �������� �� ��� X*/
	double GetMaxX() const;
	/** �������� ����������� �������� �� ��� X*/
	double GetMinX() const;
	/** �������� ������������ �������� �� ��� Y*/
	double GetMaxY() const;
	/** �������� ����������� �������� �� ��� Y*/
	double GetMinY() const;	
		
	/** �������������� ���������� �� ������� ��������� � ������� ������� �� ��� X*/
	double ParamToAreaX(double in_x) const;
	/** �������������� ���������� �� ������� ������� � ������� ��������� �� ��� X*/
	double AreaToParamX(double in_x) const;
	/** �������������� ���������� �� ������� ��������� � ������� ������� �� ��� Y*/
	double ParamToAreaY(double in_y) const;	
	/** �������������� ���������� �� ������� ������� � ������� ��������� �� ��� Y*/
	double AreaToParamY(double in_y) const;		
	
	/** ������������� ���������� �� ��������� � ����������� �� ��� X*/
	double ParamToDrawingX(double in_value) const;
	/** ������������� ���������� �� ����������� � �������� �� ��� X*/
	double DrawingToParamX(double in_value) const;
	/** ������������� ���������� �� ����������� �� ���������� �� ��� X*/
	double DrawingToDeviceX(double in_value) const;
	/** ������������� ���������� �� ���������� � ����������� �� ��� X*/
	double DeviceToDrawingX(double in_value) const;

	/** ������������� ���������� �� ��������� � ����������� �� ��� Y*/
	double ParamToDrawingY(double in_value) const;
	/** ������������� ���������� �� ����������� � �������� �� ��� Y*/
	double DrawingToParamY(double in_value) const;
	/** ������������� ���������� �� ����������� �� ���������� �� ��� Y*/
	double DrawingToDeviceY(double in_value) const;
	/** ������������� ���������� �� ���������� � ����������� �� ��� Y*/
	double DeviceToDrawingY(double in_value) const;
	

	/** ���������� ������� ��������*/
	void SetRelative(bool in_relative);	
	/** ���������� ������� ��������*/
	bool GetRelative() const;				

	/** ���������� ������ ��������� ��������� �� ��� X*/	
	const VScaleQueue& GetAvailScaleX() const;
	/** ������������� ������ ���������� ��������� �� ��� X*/	
	void SetAvailScaleX(const VScaleQueue& in_scales_x);

	/** ���������� ������ ��������� ��������� �� ��� Y*/	
	const VScaleQueue& GetAvailScaleY() const;
	/** ������������� ������ ���������� ��������� �� ��� Y*/	
	void SetAvailScaleY(const VScaleQueue& in_scales_y);

	/** ���������� ������� ��� X*/
	void SetLegendX(const std::wstring& in_legend_x);
	/** ���������� ������� ��� X*/
	std::wstring GetLegendX();

	/** ���������� ������� ��� Y*/
	void SetLegendY(const std::wstring& in_legend_y);
	/** ���������� ������� ��� Y*/
	std::wstring GetLegendY();		

	/** ���������� ��������� ������*/
	void AddDataProvider(VDataProviderPtr in_data_provider);
	
	/** ���������� ������� ������ �� X*/
	double GetMinDataX() const;
	/** ���������� �������� ������ �� X*/
	double GetMaxDataX() const;
	/** ���������� ������� ������ �� Y*/
	double GetMinDataY() const;
	/** ���������� �������� ������ �� Y*/
	double GetMaxDataY() const;

	/** ������������� ����������� ������� �������� �� Y*/
	void SetMinVisibleY(double in_value);
	/** ���������� ����������� ������� �������� �� Y*/
	double GetMinVisibleY() const;
	/** ������������� ������������ ������� �������� �� Y*/
	void SetMaxVisibleY(double in_value);
	/** ���������� ������������ ������� �������� �� Y*/
	double GetMaxVisibleY() const;

	/** ������������� ����������� ������� �������� �� X*/
	void SetMinVisibleX(double in_value);
	/** ������������� ������������ ������� �������� �� X*/
	void SetMaxVisibleX(double in_value);


	/** �������� �� X*/
	bool GetInverseX() const;
	/** �������� �� X*/
	void SetInverseX(bool in_inverse_x);
	
	/** �������� �� Y*/
	bool GetInverseY() const;
	/** �������� �� X*/
	void SetInverseY(bool in_inverse_y);
	
	/** ���������� ������� ����������*/
	bool GetActive() const {return GetArea()->GetActive();};
};
typedef boost::shared_ptr<VAxes> VAxesPtr;
typedef boost::weak_ptr<VAxes> VAxesWeakPtr;
