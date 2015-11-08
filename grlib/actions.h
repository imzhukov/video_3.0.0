#pragma once

#include "calclib.h"
#include "dbfunc.h"
#include "axes.h"
#include "elements.h"
#include "cairo_ex.h"

/** �������� / ������ ����*/
class VSetLayerVisible : public VAction
{
private : 	
	/** ����*/
	VLayerPtr layer;	
public:
	/** �����������*/
	VSetLayerVisible(VLayerPtr in_layer, const std::wstring& in_name)
	{		
		layer=in_layer;
		name=in_name;
	};
	/** ����������*/
	void Execute()
	{
		layer->SetVisible(!layer->GetVisible());		
	};		
};
typedef boost::shared_ptr<VSetLayerVisible> VSetLayerVisiblePtr;

/** �������� ������� � ������ �������� �� ����������*/
class VZoomInXAction : public VAction
{
private:
	/** ��������� �� ������� ���������*/
	std::vector<VAxesPtr> axes;	
public:
	/** �����������*/
	VZoomInXAction(const std::vector<VAxesPtr>& in_axes, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VZoomInXAction> VZoomInXActionPtr;

/** �������� ������� � ������ �������� �� ����������*/
class VZoomOutXAction : public VAction
{
private:
	/** ��������� �� ������� ���������*/
	std::vector<VAxesPtr> axes;	
public:
	/** �����������*/
	VZoomOutXAction(const std::vector<VAxesPtr>& in_axes, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VZoomOutXAction> VZoomOutXActionPtr;


/** �������� ������� � ������� �� ���������*/
class VZoomInYAction : public VAction
{
private:
	/** ��������� �� ������� ���������*/
	VAxesWeakPtr axes;	
public:
	/** �����������*/
	VZoomInYAction(VAxesWeakPtr in_axes, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VZoomInYAction> VZoomInYActionPtr;

/** �������� ������� � ������� �� ���������*/
class VZoomOutYAction : public VAction
{
private:
	/** ��������� �� ������� ���������*/
	VAxesWeakPtr axes;	
public:
	/** �����������*/
	VZoomOutYAction(VAxesWeakPtr in_axes, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VZoomOutYAction> VZoomOutYActionPtr;

/** ����������� ������ ���������*/
class VMoveOriginY : public VAction
{
private:
	/** ��������� �� ������� ���������*/
	VAxesWeakPtr axes;	
	/** ���*/
	double step;
public:
	/** �����������*/
	VMoveOriginY(VAxesWeakPtr in_axes, double in_step, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VMoveOriginY> VMoveOriginYPtr;

/** ���������� ������� �� ����������*/
class VSetScaleX : public VAction
{
private:
	/** ��������� �� ������� ���������*/
	std::vector<VAxesPtr> axes;
	/** ������ ��������� ���������*/
	VScaleQueue scales;
	/** ������ ������*/
	VAUISelectItemDialogPtr select_dialog;
public:
	/** �����������*/
	VSetScaleX(const std::vector<VAxesPtr>& in_axes, const VScaleQueue& in_scales, 
		VAUISelectItemDialogPtr in_select_dialog, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSetScaleX> VSetScaleXPtr;

/** �������� ������ ������*/
class VChangeFontSize : public VAction
{
private:
	/** ��� ��������� ������� ������*/
	double step;
public:
	/** �����������*/
	VChangeFontSize(double in_step, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VChangeFontSize> VChangeFontSizePtr;

/** ������������� �������������� ������*/
class VAutoScaleAction : public VAction
{
private:
	/** ��������� �� ������� ���������*/
	VAxesWeakPtr axes;	
public:
	/** �����������*/
	VAutoScaleAction(VAxesWeakPtr in_axes, const std::wstring& in_name);
	/** ����������*/
	void Execute();	
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VAutoScaleAction> VAutoScaleActionPtr;

/** ���������� ������� �� ���������*/
class VDefaultScaleAction : public VAction
{
private:
	/** ��������� �� ������� ���������*/
	VAxesWeakPtr axes;	
public:
	/** �����������*/
	VDefaultScaleAction(VAxesWeakPtr in_axes, const std::wstring& in_name);
	/** ����������*/
	void Execute();	
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VDefaultScaleAction> VDefaultScaleActionPtr;

/** �������� ���������� � �����*/
class VShiftCoordinatesAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** �������� ��������� ��������*/
	double step;
	/** ������ ������� ������*/
	mutable VAUIInputDialogPtr input_dialog;
public:
	/** �����������*/
	VShiftCoordinatesAction(VDataSetPtr in_data_set, double in_step, const std::wstring& in_name);
	/** �����������*/
	VShiftCoordinatesAction(VDataSetPtr in_data_set, double in_step, const std::wstring& in_name, VAUIInputDialogPtr in_input_dialog);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VShiftCoordinatesAction> VShiftCoordinatesActionPtr;

/** ��������� ��� ����� ���������� � �����*/
class VScaleCoordinatesAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** �������� ��������� ��������*/
	double scale;
public:
	/** �����������*/
	VScaleCoordinatesAction(VDataSetPtr in_data_set, double in_scale, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VScaleCoordinatesAction> VScaleCoordinatesActionPtr;


/** ���������� �������� ������ ��������� � ���������� �����*/
class VSetActiveFlagsAction : public VAction
{
private:	
	
	/** ������*/
	VDataProviderPtr data;
	/** ���������� ��� �����*/
	uint64_t set_flag;	
	
public:
	/** �����������*/
	VSetActiveFlagsAction(VDataProviderPtr in_data,
		uint64_t in_set_flag, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSetActiveFlagsAction> VSetActiveFlagsActionPtr;


/** �������� - ����������� ���������� � ����� ����*/
class VCopyParametersAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ���������*/
	VSelectionInfoPtr selection;
	/** ������*/
	VAUISaveFileDialogPtr save_file_dialog;
public:
	/** �����������*/
	VCopyParametersAction(VDataSetPtr in_data_set, VSelectionInfoPtr in_selection, VAUISaveFileDialogPtr in_save_file_dialog, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VCopyParametersAction> VCopyParametersActionPtr;

/** �������� ��������*/
class VSaveParameterAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ��� ������*/
	PARAMETER_TYPE data_type;
public:
	/** �����������*/
	VSaveParameterAction(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSaveParameterAction> VSaveParameterActionPtr;

/** ������������ ��������*/
class VReloadParameterAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ��� ������*/
	PARAMETER_TYPE data_type;
public:
	/** �����������*/
	VReloadParameterAction(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VReloadParameterAction> VReloadParameterActionPtr;

/** �������� ���������*/
class VSaveChangesAction : public VAction
{
private:
	/** �������� ������*/
	std::vector<VDataSetPtr> data_set;	
public:
	/** �����������*/
	VSaveChangesAction(const std::vector<VDataSetPtr>& in_data_set, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSaveChangesAction> VSaveChangesActionPtr;

/** ������� ������*/
class VDeleteAllAction : public VAction
{
private:
	/** ������*/
	VDataProviderPtr data;

public:
	/** �����������*/
	VDeleteAllAction(VDataProviderPtr in_data, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ���������� ������� ������������� ������� �������������*/
	virtual bool NeedAccept() const{return true;};
	/** ���������� ������ ������� �������������*/
	virtual std::wstring GetAcceptQuestion() const{return L"������ ����� �������. ����������?";};
};
typedef boost::shared_ptr<VDeleteAllAction> VDeleteAllActionPtr;


/** ������������� ��������� �������*/
class VAutomarkProfileAction : public VAction
{
private:
	/** ������*/
	VDataSetPtr data_set;
	/** ��������*/
	VAUIProgressDialogPtr progress_dialog;
	/** ������ ����� ����������*/
	VAUIPropertyDialogPtr property_dialog;
public:
	/** �����������*/
	VAutomarkProfileAction(VDataSetPtr in_data_set, VAUIProgressDialogPtr in_progress_dialog);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ���������� ������� ������������� ������� �������������*/
	virtual bool NeedAccept() const{return true;};
	/** ���������� ������ ������� �������������*/
	virtual std::wstring GetAcceptQuestion() const{return L"������������ ����� ����� �������. ����������?";};	
};
typedef boost::shared_ptr<VAutomarkProfileAction> VAutomarkProfileActionPtr;

/** ��������� �������� ������� �� ��*/
class VProfileFromDBAction : public VAction
{
private:
	/// ������
	VDataSetPtr data_set;
	/// ���� ������
	VIBDatabasePtr database;
	/// ��������
	VAUIProgressDialogPtr progress_dialog;

public:
	/** �����������*/
	VProfileFromDBAction(VDataSetPtr in_data_set, VIBDatabasePtr in_database, VAUIProgressDialogPtr in_progress_dialog);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ���������� ������� ������������� ������� �������������*/
	virtual bool NeedAccept() const{return true;};
	/** ���������� ������ ������� �������������*/
	virtual std::wstring GetAcceptQuestion() const{return L"������������ ����� ����� �������. ����������?";};	
};
typedef boost::shared_ptr<VProfileFromDBAction> VProfileFromDBActionPtr;


/** ������� �����*/
class VDeleteDataItemAction : public VAction
{
private:
	/** ������*/
	VDataProviderPtr data;
	
public:
	/** �����������*/
	VDeleteDataItemAction(VDataProviderPtr in_data, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VDeleteDataItemAction> VDeleteDataItemActionPtr;

/** ������� ������ ��������*/
class VActivateAction : public VAction
{
private:
	/** ������*/
	VActiveObjectPtr data;		
public:
	/** �����������*/
	VActivateAction(VActiveObjectPtr in_data, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ���������� ��� - �������� ��������*/
	virtual std::wstring& GetName();
};
typedef boost::shared_ptr<VActivateAction> VActivateActionPtr;

/** ������� ������ �����������*/
class VDeactivateAction : public VAction
{
private:
	/** ������*/
	VActiveObjectPtr data;		
public:
	/** �����������*/
	VDeactivateAction(VActiveObjectPtr in_data, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ���������� ��� - �������� ��������*/
	virtual std::wstring& GetName();	
};
typedef boost::shared_ptr<VDeactivateAction> VDeactivateActionPtr;

/** ���������� �������� �������*/
class VSetSlopeType : public VAction
{
private:
	/** ������*/
	VSlopeDataPtr data;		
	/** ����� ������*/
	PARAMETER_TYPE data_type;
public:
	/** �����������*/
	VSetSlopeType(VSlopeDataPtr in_data, PARAMETER_TYPE in_data_type, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VSetSlopeType> VSetSlopeTypePtr;

/** ���������� �������� ��������� �������*/
class VSetHeightType : public VAction
{
private:
	/** ������*/
	VHeightDataPtr data;		
	/** ����� ������*/
	PARAMETER_TYPE data_type;
public:
	/** �����������*/
	VSetHeightType(VHeightDataPtr in_data, PARAMETER_TYPE in_data_type, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VSetHeightType> VSetHeightTypePtr;

/** �������� ���������� ������� ������� �����*/
class VSelectionToSideway : public VAction
{
private:
	/** ������*/
	VDataProviderPtr data;
	/** ���������*/
	VSelectionInfoPtr selection;
	/** ���*/
	uint32_t reason;	
public:
	/** �����������*/
	VSelectionToSideway(VDataProviderPtr in_data, VSelectionInfoPtr in_selection, uint32_t in_reason, 
		const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VSelectionToSideway> VSelectionToSidewayPtr;

/** �������� ���������� ������� ����������� �����*/
class VExtractStationWay : public VAction
{
private:
	/** ������*/
	VDataSetPtr data_set;
	/** ���������*/
	VSelectionInfoPtr selection;
	/** ���*/
	uint32_t reason;
	/** ��������� �� ������ ������*/
	VAUISelectItemDialogPtr select_dialog;
	/** ��� �������*/
	uint32_t station_code;
	/** �������� �������*/
	std::string station_name;
	/** �������� �����*/
	std::string park_name;
	/** ��������� �� ��*/
	VIBDatabasePtr database;
	/** ��������� �� ����������*/
	VIBTransactionPtr transaction;
public:
	/** �����������*/
	VExtractStationWay(VDataSetPtr in_data_set, VIBDatabasePtr in_database, 
		VIBTransactionPtr in_transaction, VSelectionInfoPtr in_selection, 
		VAUISelectItemDialogPtr in_select_dialog, uint32_t in_station_code, const std::string& in_station_name, 
		const std::string& in_park_name, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VExtractStationWay> VExtractStationWayPtr;

/** �������� ������� ������������ ���� � �����*/
class VAttachStationObject : public VAction
{
private:
	/** �������� ������ � ������*/
	VDataProviderPtr mark_data;	
	/** ���� �������*/
	VIBDatabasePtr database; 
	/** ����������*/
	VIBTransactionPtr transaction; 
	/** ��������� �� ������ ������*/
	VAUISelectItemDialogPtr select_dialog;
	/** ��� �������*/
	uint32_t station_code; 
	/** ����*/
	std::string park_name;
	/** ����*/
	std::string way_code;
public:
	/** �����������*/
	VAttachStationObject(VDataProviderPtr in_mark_data, 
		VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, VAUISelectItemDialogPtr in_select_dialog,
		uint32_t in_station_code, const std::string& in_park_name, const std::string& in_way_code, 
		const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VAttachStationObject> VAttachStationObjectPtr;

/** �������� �������� ������� ������������ ���� � �����*/
class VDetachStationObject : public VAction
{
private:
	/** �������� ������ � ������*/
	VDataProviderPtr mark_data;	
	/** ���� �������*/
	VIBDatabasePtr database; 
	/** ����������*/
	VIBTransactionPtr transaction;
	/** ��� �������*/
	uint32_t station_code; 
	/** ����*/
	std::string park_name;
public:
	/** �����������*/
	VDetachStationObject(VDataProviderPtr in_mark_data,
		VIBDatabasePtr in_database, VIBTransactionPtr in_transaction,
		uint32_t in_station_code, const std::string& in_park_name, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VDetachStationObject> VDetachStationObjectPtr;

/** ������� ���� -> ���������*/
class VSidewayToSelection : public VAction
{
private:
	/** ������*/
	VDataProviderPtr data;
	/** ���������*/
	VSelectionInfoPtr selection;
	/** ���*/
	uint32_t reason;
public:
	/** �����������*/
	VSidewayToSelection(VDataProviderPtr in_data, VSelectionInfoPtr in_selection, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ��������� �������*/
	bool ProcessEvent(const VEvent& in_event);
};
typedef boost::shared_ptr<VSidewayToSelection> VSidewayToSelectionPtr;

/** �������� - ������� � ��������� ������*/
class VActivateNextCurve : public VAction
{
private:	
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ��������� �� �������*/ //FixMe!!! ��������� �� ������� ������
	VDrawing * drawing;
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VActivateNextCurve(VDataSetPtr in_data_set, VDrawing * in_drawing, 
		const std::wstring& in_name, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;		
};
typedef boost::shared_ptr<VActivateNextCurve> VActivateNextCurvePtr;


/** ������������ ������*/
class VNormalizeCurve : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;	
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VNormalizeCurve(VDataSetPtr in_data_set, const std::wstring& in_name, 
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VNormalizeCurve> VNormalizeCurvePtr;


/** ����������� ������*/
class VOptimizeCurve : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VOptimizeCurve(VDataSetPtr in_data_set, const std::wstring& in_name, 
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VOptimizeCurve> VOptimizeCurvePtr;

/// �������� ������� �����
class VIncreaseLevelAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VIncreaseLevelAction(VDataSetPtr in_data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VIncreaseLevelAction> VIncreaseLevelActionPtr;

/// �������� ������� ����
class VDecreaseLevelAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VDecreaseLevelAction(VDataSetPtr in_data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VDecreaseLevelAction> VDecreaseLevelActionPtr;

/** ���������� ���������� ����� �� ������ � ����������� ����� �� ��������*/
class VFixLevelCoordinates : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VFixLevelCoordinates(VDataSetPtr in_data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VFixLevelCoordinates> VFixLevelCoordinatesPtr;

/** ���������� ����������� ���������� ��� ���������� ������*/
class VCreateLevelOptimalAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
public:
	/** �����������*/
	VCreateLevelOptimalAction(VDataSetPtr in_data_set, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VCreateLevelOptimalAction> VCreateLevelOptimalActionPtr;

/** ���������� ������������ ���������� ��� ���������� ������*/
class VCreateLevelRationalAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
public:
	/** �����������*/
	VCreateLevelRationalAction(VDataSetPtr in_data_set, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VCreateLevelRationalAction> VCreateLevelRationalActionPtr;

/** ���������� ���������� ������� � �����*/
class VCreateLevelFactAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
public:
	/** �����������*/
	VCreateLevelFactAction(VDataSetPtr in_data_set, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VCreateLevelFactAction> VCreateLevelFactActionPtr;


/** ���������� ����������� ���������� (��� ���� ������)*/
class VCreateLevelOptimalAllCurves : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
public:
	/** �����������*/
	VCreateLevelOptimalAllCurves(VDataSetPtr in_data_set, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VCreateLevelOptimalAllCurves> VCreateLevelOptimalAllCurvesPtr;

/** ���������� ������������ ���������� (��� ���� ������)*/
class VCreateLevelRationalAllCurves : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
public:
	/** �����������*/
	VCreateLevelRationalAllCurves(VDataSetPtr in_data_set, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VCreateLevelRationalAllCurves> VCreateLevelRationalAllCurvesPtr;

/** ���������� ���������� ������� � ����� (��� ���� ������)*/
class VCreateLevelFactAllCurves : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
public:
	/** �����������*/
	VCreateLevelFactAllCurves(VDataSetPtr in_data_set, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VCreateLevelFactAllCurves> VCreateLevelFactAllCurvesPtr;


/** �������� ������ � ������*/
class VApplyActiveCurve : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VApplyActiveCurve(VDataSetPtr in_data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VApplyActiveCurve> VApplyActiveCurvePtr;

/** �������� - ������� ������� ������*/
class VDeleteActiveCurveAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VDeleteActiveCurveAction(VDataSetPtr data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ���������� ������� ������������� ������� �������������*/
	bool NeedAccept() const{return true;};
	/** ���������� ������ ������� �������������*/
	std::wstring GetAcceptQuestion() const{return L"�������� ������ ����� �������. ����������?";};
};
typedef boost::shared_ptr<VDeleteActiveCurveAction> VDeleteActiveCurveActionPtr;

/** �������� - ������� ��� �������� ������*/
class VDeleteAllCurvesAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VDeleteAllCurvesAction(VDataSetPtr in_data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ���������� ������� ������������� ������� �������������*/
	bool NeedAccept() const{return true;};
	/** ���������� ������ ������� �������������*/
	std::wstring GetAcceptQuestion() const{return L"�������� ���� ������ ����� �������. ����������?";};
};
typedef boost::shared_ptr<VDeleteAllCurvesAction> VDeleteAllCurvesActionPtr;


/** �������� - ������� ��� �������� ������*/
class VDeleteAllLinesAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VDeleteAllLinesAction(VDataSetPtr in_data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ���������� ������� ������������� ������� �������������*/
	bool NeedAccept() const{return true;};
	/** ���������� ������ ������� �������������*/
	std::wstring GetAcceptQuestion() const{return L"�������� ������ ����� �������. ����������?";};
};
typedef boost::shared_ptr<VDeleteAllLinesAction> VDeleteAllLinesActionPtr;

/** �������� - �������� ����������� �������� � ����������*/
class VCopyCurvesAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;		
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VCopyCurvesAction(VDataSetPtr in_data_set, const std::wstring& in_name, 
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ���������� ������� ������������� ������� �������������*/
	bool NeedAccept() const{return true;};
	/** ���������� ������ ������� �������������*/
	std::wstring GetAcceptQuestion() const{return L"������������ �������� ����� �������. ����������?";};
};
typedef boost::shared_ptr<VCopyCurvesAction> VCopyCurvesActionPtr;

/** �������� - �������������� �������� ������*/
class VAutomarkCurvesAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;	
	/** ��������*/
	VAUIProgressDialogPtr progress_dialog;
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VAutomarkCurvesAction(VDataSetPtr in_data_set,
		VAUIProgressDialogPtr in_progress_dialog, const std::wstring& in_name, 
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ���������� ������� ������������� ������� �������������*/
	bool NeedAccept() const{return true;};
	/** ���������� ������ ������� �������������*/
	std::wstring GetAcceptQuestion() const{return L"������������ �������� ����� �������. ����������?";};
};
typedef boost::shared_ptr<VAutomarkCurvesAction> VAutomarkCurvesActionPtr;

/** �������� - ������� ��������� ������ �� ��*/
class VTransferPassportCurvesAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ���� ������*/
	VIBDatabasePtr database;
	/** ��������*/
	VAUIProgressDialogPtr progress_dialog;
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VTransferPassportCurvesAction(VDataSetPtr in_data_set, VIBDatabasePtr in_database,
		VAUIProgressDialogPtr in_progress_dialog, const std::wstring& in_name, 
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ���������� ������� ������������� ������� �������������*/
	bool NeedAccept() const{return true;};
	/** ���������� ������ ������� �������������*/
	std::wstring GetAcceptQuestion() const{return L"������������ �������� ����� �������. ����������?";};
};
typedef boost::shared_ptr<VTransferPassportCurvesAction> VTransferPassportCurvesActionPtr;


/** �������� - ������� ���������� ������ �� ��*/
class VTransferMeasuredCurvesAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ���� ������*/
	VIBDatabasePtr database;
	/** ��������*/
	VAUIProgressDialogPtr progress_dialog;
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VTransferMeasuredCurvesAction(VDataSetPtr in_data_set, VIBDatabasePtr in_database,
		VAUIProgressDialogPtr in_progress_dialog, const std::wstring& in_name, 
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ���������� ������� ������������� ������� �������������*/
	bool NeedAccept() const{return true;};
	/** ���������� ������ ������� �������������*/
	std::wstring GetAcceptQuestion() const{return L"������������ �������� ����� �������. ����������?";};
};
typedef boost::shared_ptr<VTransferMeasuredCurvesAction> VTransferMeasuredCurvesActionPtr;

/** �������� - ����������� ������*/
class VOptimizeCurvesAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ��������-������*/
	VAUIProgressDialogPtr progress_dialog;
	/** ��� �����*/
	PARAMETER_TYPE mark_type;
public:
	/** �����������*/
	VOptimizeCurvesAction(VDataSetPtr in_data_set, VAUIProgressDialogPtr in_progress_dialog, 
		const std::wstring& in_name, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VOptimizeCurvesAction> VOptimizeCurvesActionPtr;

/** �������� - ���������� ������ �������� ������*/
class VSetCurveStatusAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ������ ������*/
	VAUISelectItemDialogPtr select_dialog;
public:
	/** �����������*/
	VSetCurveStatusAction(VDataSetPtr data_set, const std::wstring& in_name,
		VAUISelectItemDialogPtr in_select_dialog);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSetCurveStatusAction> VSetCurveStatusActionPtr;

/** �������� - ���������� ����������� �������� ������*/
class VSetCurveCommentAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ������ ������*/
	VAUISelectItemDialogPtr select_dialog;
	/** ������ �����*/
	VAUIInputDialogPtr input_dialog;
public:
	/** �����������*/
	VSetCurveCommentAction(VDataSetPtr data_set, const std::wstring& in_name,
		VAUISelectItemDialogPtr in_select_dialog, VAUIInputDialogPtr in_input_dialog);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSetCurveCommentAction> VSetCurveCommentActionPtr;

/** �������� - ������ �������� �� ������*/
class VSetCurveSpeedsAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ������ ������*/
	VAUIPropertyDialogPtr property_dialog;
public:
	/** �����������*/
	VSetCurveSpeedsAction(VDataSetPtr data_set, const std::wstring& in_name,
		VAUIPropertyDialogPtr in_property_dialog);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSetCurveSpeedsAction> VSetCurveSpeedsActionPtr;

/** �������� ���������� �������� �� ������ �� ����������*/
class VResetCurveSpeedsAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
public:
	/** �����������*/
	VResetCurveSpeedsAction(VDataSetPtr data_set, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VResetCurveSpeedsAction> VResetCurveSpeedsActionPtr;

/** ��������  - ������ ��������*/
class VGroupAction : public VAction
{
private:
	/** ������ ��������*/
	std::vector<VActionPtr> actions;
public:
	/** �����������*/
	VGroupAction(const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
	/** ��������� �������� � ������*/
	void AddAction(VActionPtr in_action);
};
typedef boost::shared_ptr<VGroupAction> VGroupActionPtr;

/** �������� ������*/
class VRefreshDataAction : public VAction
{
private:
	/** ������*/
	VDataProviderPtr data;
public:
	/** �����������*/
	VRefreshDataAction(VDataProviderPtr in_data, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VRefreshDataAction> VRefreshDataActionPtr;

/** �������� - ������������� �� ��. �������*/
class VCorrectCoordAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	VIBDatabasePtr database;
	POINT_TYPE type;
public:
	/** �����������*/
	VCorrectCoordAction(VDataSetPtr in_data_set, VIBDatabasePtr in_database, POINT_TYPE in_type);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VCorrectCoordAction> VCorrectCoordActionPtr;

/** �������� - ����� ��������� �������*/
class VShiftZazorAction : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ������ ������� ����������*/
	VAUIPropertyDialogPtr property_dialog;
public:
	/** �����������*/
	VShiftZazorAction(VDataSetPtr in_data_set, const std::wstring& in_name, VAUIPropertyDialogPtr in_property_dialog);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VShiftZazorAction> VShiftZazorActionPtr;

/** �������� - ��������� �������*/
class VSelectPointAction : public VAction
{
private:
	/** �������� ������ � �������*/
	VDataProviderPtr data;
public:
	/** �����������*/
	VSelectPointAction(VDataProviderPtr in_data);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSelectPointAction> VSelectPointActionPtr;

/// �������� - �������� ���� ������ ��������� ��� ������
class VAlignParameterMean : public VAction
{
private:
	/// ���������� �������
	VSelectionInfoPtr selection;
	/// �������� ������
	VDataSetPtr data_set;	
	/// ��� ��������� ���������
	PARAMETER_TYPE data_type_source;
	/// ��� ��������� ���������
	PARAMETER_TYPE data_type_target;

public:
	///�����������
	VAlignParameterMean(VSelectionInfoPtr in_selection, VDataSetPtr in_data_set, 
		PARAMETER_TYPE in_data_type_source, PARAMETER_TYPE in_data_type_target, 
		const std::wstring& in_name);
	/// ����������
	void Execute();
	/// ���������� true, ���� �������� ��������
	bool Allowed() const;
};
typedef boost::shared_ptr<VAlignParameterMean> VAlignParameterMeanPtr;

/** �������� - �������� ����*/
class VShiftParameterAction : public VAction
{
private:	
	/** ��������� �� ������� ���������*/
	VAxesWeakPtr axes;
	/** �������� ������*/
	VDataSetPtr data_set;	
	/** ��� ���������*/
	PARAMETER_TYPE data_type;
	/** �������� ��������*/
	double shift;
public:
	/** �����������*/
	VShiftParameterAction(VAxesWeakPtr in_axes, VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type, 
		double in_shift, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VShiftParameterAction> VShiftParameterActionPtr;


/** �������� - ���������� ���������*/
class VCorruptParameterAction : public VAction
{
private:
	/** ���������� �������*/
	VSelectionInfoPtr selection_info;
	/** �������� ������*/
	VDataSetPtr data_set;
	/** ������� ����*/
	bool corrupt;
	/** ��� ���������*/
	PARAMETER_TYPE data_type;
public:
	/** �����������*/
	VCorruptParameterAction(VSelectionInfoPtr in_selection_info, VDataSetPtr in_data_set, bool in_corrupt, 
		PARAMETER_TYPE in_data_type, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VCorruptParameterAction> VCorruptParameterActionPtr;

/** ��������� ����� �� ������� � �������*/
class VAdjustProfileMarkHeight  : public VAction
{
private:
	/** �������� ������*/
	VDataSetPtr data_set;
public:
	/** �����������*/
	VAdjustProfileMarkHeight(VDataSetPtr data_set, const std::wstring& in_name);
	/** ����������*/
	void Execute();
	/** ���������� true, ���� �������� ��������*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VAdjustProfileMarkHeight> VAdjustProfileMarkHeightPtr;