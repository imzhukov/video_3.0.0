#include "actions.h"
#include "VProgressInterface.h"

#include "VParamCorrections.h"

//--------------------------- ������ ������ VZoomInXAction -------------------------------------

struct is_null
{
	bool operator()(const VAxesPtr& in_axes){return in_axes;};
};

/** �����������*/
VZoomInXAction::VZoomInXAction(const std::vector<VAxesPtr>& in_axes, const std::wstring& in_name)
{	
	if (count_if(in_axes.begin(), in_axes.end(), is_null())==in_axes.size())
	{
		axes=in_axes;	
		name=in_name;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VZoomInXAction::Execute()
{	
	for (size_t i=0; i<axes.size(); ++i)
		axes.at(i)->ZoomInX();	
}

/** ���������� true, ���� �������� ��������*/
bool VZoomInXAction::Allowed() const
{
	return axes.size()>0;
}

//--------------------------- ������ ������ VZoomOutXAction -------------------------------------

/** �����������*/
VZoomOutXAction::VZoomOutXAction(const std::vector<VAxesPtr>& in_axes, const std::wstring& in_name)
{
	if (count_if(in_axes.begin(), in_axes.end(), is_null())==in_axes.size())
	{
		axes=in_axes;	
		name=in_name;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VZoomOutXAction::Execute()
{
	for (size_t i=0; i<axes.size(); ++i)
		axes.at(i)->ZoomOutX();	
}

/** ���������� true, ���� �������� ��������*/
bool VZoomOutXAction::Allowed() const
{
	return axes.size()>0;
}

//--------------------------- ������ ������ VZoomInYAction -------------------------------------

/** �����������*/
VZoomInYAction::VZoomInYAction(VAxesWeakPtr in_axes, const std::wstring& in_name)
{
	axes=in_axes;	
	name=in_name;	
}

/** ����������*/
void VZoomInYAction::Execute()
{
	if (VAxesPtr a=axes.lock())
	{
		a->ZoomInY();
	}	
}

/** ���������� true, ���� �������� ��������*/
bool VZoomInYAction::Allowed() const
{
	if (VAxesPtr a=axes.lock())
	{
		return a->GetActive();
	}
	else
		return false;
}


//--------------------------- ������ ������ VZoomOutYAction -------------------------------------

/** �����������*/
VZoomOutYAction::VZoomOutYAction(VAxesWeakPtr in_axes, const std::wstring& in_name)
{
	axes=in_axes;	
	name=in_name;	
}

/** ����������*/
void VZoomOutYAction::Execute()
{
	if (VAxesPtr a=axes.lock())
	{
		a->ZoomOutY();	
	}	
}

/** ���������� true, ���� �������� ��������*/
bool VZoomOutYAction::Allowed() const
{
	if (VAxesPtr a=axes.lock())
	{
		return a->GetActive();
	}
	else
		return false;
}

//----------------------------------- ������ ������ VMoveOriginY -------------------------------

/** �����������*/
VMoveOriginY::VMoveOriginY(VAxesWeakPtr in_axes, double in_step, const std::wstring& in_name)
{
	axes=in_axes;
	step=in_step;
	name=in_name;	
}

/** ����������*/
void VMoveOriginY::Execute()
{		
	if (VAxesPtr a=axes.lock())
	{
		a->SetOriginY(a->GetOriginY()+step);	
	}			
}

/** ���������� true, ���� �������� ��������*/
bool VMoveOriginY::Allowed() const
{
	if (VAxesPtr a=axes.lock())
	{
		return a->GetActive();
	}
	else
		return false;
}

//---------------------------------- ������ ������ VSetScaleX ---------------------------------------
/** �����������*/
VSetScaleX::VSetScaleX(const std::vector<VAxesPtr>& in_axes, const VScaleQueue& in_scales, 
	VAUISelectItemDialogPtr in_select_dialog, const std::wstring& in_name)
{
	axes=in_axes;
	scales=in_scales;
	select_dialog=in_select_dialog;
	name=in_name;
}

/** ����������*/
void VSetScaleX::Execute()
{
	std::vector<std::wstring> list;
	for (size_t i=0; i<scales.GetCount(); ++i)
		list.push_back(boost::lexical_cast<std::wstring>(scales.GetScale(i)));
	select_dialog->SetList(list);
	if (axes.size()>0)
	{	
		select_dialog->SetSelectedItem(boost::lexical_cast<std::wstring>(axes.at(0)->GetScaleX()));
	}
	//�������� ������
	if (select_dialog->ShowModal(L"����� ��������", L"������� (������ �� ���������)"))
	{
		for (size_t i=0; i<axes.size(); ++i)
		{
			axes.at(i)->SetScaleX(str_to_double(select_dialog->GetSelectedItem()));
		}	
	}	
}

/** ���������� true, ���� �������� ��������*/
bool VSetScaleX::Allowed() const
{
	return scales.GetCount()>0;
}

//--------------------------------- ������ ������ VChangeFontSize ------------------------------------

/** �����������*/
VChangeFontSize::VChangeFontSize(double in_step, const std::wstring& in_name)
{
	step=in_step;
	name=in_name;
}

/** ����������*/
void VChangeFontSize::Execute()
{	
	BASE_FONT_SIZE().Value()+=step;	
}

/** ���������� true, ���� �������� ��������*/
bool VChangeFontSize::Allowed() const
{
	//FixMe!!! ����� ����� ���������� ���������� ������� ������
	return true;
}

//----------------------------- ������ ������ VAutoScaleAction -------------------------------------

/** �����������*/
VAutoScaleAction::VAutoScaleAction(VAxesWeakPtr in_axes, const std::wstring& in_name)
{	
	axes=in_axes;
	name=in_name;	
}

/** ����������*/
void VAutoScaleAction::Execute()
{
	if (VAxesPtr a=axes.lock())	
		a->SetAutoScaleY();		
}

/** ���������� true, ���� �������� ��������*/
bool VAutoScaleAction::Allowed() const
{
	if (VAxesPtr a=axes.lock())			
		return a->AllowAutoScaleY();	
	else
		return false;
}

//----------------------------- ������ ������ VDefaultScaleAction -------------------------------------

/** �����������*/
VDefaultScaleAction::VDefaultScaleAction(VAxesWeakPtr in_axes, const std::wstring& in_name)
{
	axes=in_axes;
	name=in_name;	
}

/** ����������*/
void VDefaultScaleAction::Execute()
{
	if (VAxesPtr a=axes.lock())
		a->SetDefaultScaleY();
}

/** ���������� true, ���� �������� ��������*/
bool VDefaultScaleAction::Allowed() const
{
	if (VAxesPtr a=axes.lock())
		return a->AllowDefaultScaleY();	
	else	
		return false;	
}


//--------------------------- ������ ������ VShiftCoordinatesAction ----------------------

/** �����������*/
VShiftCoordinatesAction::VShiftCoordinatesAction(VDataSetPtr in_data_set, double in_step, const std::wstring& in_name)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		step=in_step;
		name=in_name;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** �����������*/
VShiftCoordinatesAction::VShiftCoordinatesAction(VDataSetPtr in_data_set, double in_step, const std::wstring& in_name, VAUIInputDialogPtr in_input_dialog)
{
	if (in_data_set && in_input_dialog)
	{
		data_set=in_data_set;
		step=in_step;
		name=in_name;
		input_dialog=in_input_dialog;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VShiftCoordinatesAction::Execute()
{	
	if (input_dialog && input_dialog->ShowModal(name, L"0"))
	{
		step=boost::lexical_cast<double>(input_dialog->GetValue());		
	}
	data_set->GetAbsCoord().Shift(step);
	data_set->SetChanged(PRM_ABS_COORD);
}

/** ���������� true, ���� �������� ��������*/
bool VShiftCoordinatesAction::Allowed() const
{
	return data_set->GetAbsCoord().IsLoaded();
}


//--------------------------- ������ ������ VShiftCoordinatesAction ----------------------

/** �����������*/
VScaleCoordinatesAction::VScaleCoordinatesAction(VDataSetPtr in_data_set, double in_scale, const std::wstring& in_name)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		scale=in_scale;
		name=in_name;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VScaleCoordinatesAction::Execute()
{
	data_set->GetAbsCoord().Scale(scale);
	data_set->SetChanged(PRM_ABS_COORD);
}

/** ���������� true, ���� �������� ��������*/
bool VScaleCoordinatesAction::Allowed() const
{
	return data_set->GetAbsCoord().IsLoaded();
}

//----------------------------------- ������ ������ VSetActiveFlagsAction ------------------------

/** �����������*/
VSetActiveFlagsAction::VSetActiveFlagsAction(VDataProviderPtr in_data, uint64_t in_set_flag, const std::wstring& in_name)
{
	if (in_data)
	{
		data=in_data;
		set_flag=in_set_flag;
		name=in_name;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VSetActiveFlagsAction::Execute()
{
	data->SetFlags(data->GetSelectedPoint(), set_flag);		
}

/** ���������� true, ���� �������� ��������*/
bool VSetActiveFlagsAction::Allowed() const
{
	return data->GetSelectedPoint()!=BAD_INDEX &&
		data->GetFlags(data->GetSelectedPoint())!=set_flag;
}

//--------------------------------������ ������ VCopyParametersAction ------------------------------

/** �����������*/
VCopyParametersAction::VCopyParametersAction(VDataSetPtr in_data_set, VSelectionInfoPtr in_selection, 
	VAUISaveFileDialogPtr in_save_file_dialog, const std::wstring& in_name)
{
	if (in_data_set && in_selection)
	{
		data_set=in_data_set;
		selection=in_selection;
		name=in_name;
		save_file_dialog=in_save_file_dialog;
	}
}

/** ����������*/
void VCopyParametersAction::Execute()
{
	VWaySection section;
	section.start=data_set->GetFileAprinfo().GetRealCoord(std::min<double>(selection->GetStartX(), selection->GetEndX()));
	section.end=data_set->GetFileAprinfo().GetRealCoord(std::max<double>(selection->GetStartX(), selection->GetEndX()));
	VDataSet target;
	VFileName fn = data_set->GetFileName();
	fn.SetShortName(L"copy_of_" + fn.GetShortName());
	fn.SetExt(L"");
	//���� ���� ������, �������� ���
	if (save_file_dialog)
	{
		save_file_dialog->SetWildcard(L"����� ������� ����� (*.c4f)|*.c4f|����� ����������� ����� (*.s4f)|*.s4f|��������� (*.inm)|*.inm|��� ����� (*.*)|*.*");
		save_file_dialog->SetPath(fn.GetPath());
		if (save_file_dialog->ShowModal())
		{
			VProgressBox prog(1, "����������� ���������� � ����� ����", false);
			VFileName new_file(save_file_dialog->GetPath());
			target.CopyFrom(*data_set, section, &prog);		

			if (_wcsicmp(new_file.GetExt().c_str(), L".inm") == 0)
			{
				target.SaveToInm(new_file.GetPath(), &prog);
			}
			else
			{
				target.CreateFile(save_file_dialog->GetPath().c_str());
				if (!target.Save())
				{
				    LOG_ERROR(target.GetErrorMessage());
				}
			}
		}
	}
	else
	{
		VProgressBox prog(1, "����������� ���������� � ����� ����", false);
		target.CreateFile(fn);
		target.CopyFrom(*data_set, section, &prog);		
		if (!target.Save())
		{
		    LOG_ERROR(target.GetErrorMessage());
		}
	}
}

/** ���������� true, ���� �������� ��������*/
bool VCopyParametersAction::Allowed() const
{
	return selection->Valid();
}

//------------------------------------ ������ ������ VSaveParameterAction -------------------------------------

/** �����������*/
VSaveParameterAction::VSaveParameterAction(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type, const std::wstring& in_name)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_type=in_data_type;
		SetName(in_name);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VSaveParameterAction::Execute()
{		
	data_set->SetChanged(data_type);
	if (!data_set->Save())
	{
		throw VSimpleException(L"�� ������� �������� ������", data_set->GetErrorMessage().c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
}

/** ���������� true, ���� �������� ��������*/
bool VSaveParameterAction::Allowed() const
{
	return data_set->IsChanged(data_type);
}

//------------------------------------ ������ ������ VReloadParameterAction -------------------------------------

/** �����������*/
VReloadParameterAction::VReloadParameterAction(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type, const std::wstring& in_name)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_type=in_data_type;
		SetName(in_name);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VReloadParameterAction::Execute()
{				
	data_set->Unload(data_type);
	data_set->Load(data_type);
	data_set->UnsetChanged(data_type);		
}

/** ���������� true, ���� �������� ��������*/
bool VReloadParameterAction::Allowed() const
{
	return data_set->IsChanged(data_type);
}

//----------------------------------- ������ ������ VSaveChangesAction -----------------------------------

/** �����������*/
VSaveChangesAction::VSaveChangesAction(const std::vector<VDataSetPtr>& in_data_set, const std::wstring& in_name)
{
	data_set=in_data_set;
	name=in_name;
}

/** ����������*/
void VSaveChangesAction::Execute()
{
	for (size_t i=0; i<data_set.size(); ++i)
	{
		if (data_set.at(i) && data_set.at(i)->IsChanged())
			data_set.at(i)->Save();
	}
}

/** ���������� true, ���� �������� ��������*/
bool VSaveChangesAction::Allowed() const
{
	for (size_t i=0; i<data_set.size(); ++i)
	{
		if (data_set.at(i) && data_set.at(i)->IsChanged())
			return true;
	}
	return false;
}

//-------------------------------- ������ ������ VDeleteAllAction --------------------------------

/** �����������*/
VDeleteAllAction::VDeleteAllAction(VDataProviderPtr in_data, const std::wstring& in_name)
{
	if (in_data)
	{
		data=in_data;		
		name=in_name;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VDeleteAllAction::Execute()
{
	data->DeleteAll();	
}

/** ���������� true, ���� �������� ��������*/
bool VDeleteAllAction::Allowed() const
{
	return data->GetActive() && data->GetCount()>0;	
}

//----------------------------------  ������ ������ VAutomarkProfileAction -----------------------------------------

/** �����������*/
VAutomarkProfileAction::VAutomarkProfileAction(VDataSetPtr in_data_set, VAUIProgressDialogPtr in_progress_dialog)
{
	if (in_data_set)
	{
		data_set=in_data_set;						
		progress_dialog=in_progress_dialog;
		SetName(L"�������������� �������� �������");
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VAutomarkProfileAction::Execute()
{	
	data_set->ProfileAutomark(progress_dialog);	
}

/** ���������� true, ���� �������� ��������*/
bool VAutomarkProfileAction::Allowed() const
{	
	return true;	
}


//-------------------------------- ������ ������ VProfileFromDBAction ------------------------------------

/** �����������*/
VProfileFromDBAction::VProfileFromDBAction(VDataSetPtr in_data_set, VIBDatabasePtr in_database,
	VAUIProgressDialogPtr in_progress_dialog)
{
	if (in_data_set && in_database && in_progress_dialog)
	{
		data_set=in_data_set;		
		database=in_database;
		progress_dialog=in_progress_dialog;
		name=L"��������� �������� ������� �� ��";				
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VProfileFromDBAction::Execute()
{	
	data_set->ProfileFromDB(database, progress_dialog);
}

/** ���������� true, ���� �������� ��������*/
bool VProfileFromDBAction::Allowed() const
{
	return true;
}

//------------------------------------ ������ ������ VDeleteDataItemAction ----------------------------------

/** �����������*/
VDeleteDataItemAction::VDeleteDataItemAction(VDataProviderPtr in_data, const std::wstring& in_name)
{
	if (in_data)
	{
		data=in_data;				
		SetName(in_name);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VDeleteDataItemAction::Execute()
{
	data->DeleteItem(data->GetSelectedPoint());
}

/** ���������� true, ���� �������� ��������*/
bool VDeleteDataItemAction::Allowed() const
{
	return data->GetActive() && data->GetSelectedPoint()!=BAD_INDEX && data->CanChange(data->GetSelectedPoint());
}

//------------------------------------ ������ ������ VActivateAction ----------------------------------


/** �����������*/
VActivateAction::VActivateAction(VActiveObjectPtr in_data, const std::wstring& in_name)
{
	if (in_data)
	{
		data=in_data;	
		name=in_name;	
	}
	else
	{
		throw NULL_POINTER;
	}
}
/** ����������*/
void VActivateAction::Execute()
{
	data->SetActive(true);	
}
/** ���������� true, ���� �������� ��������*/
bool VActivateAction::Allowed() const
{	
	return !data->GetActive();
}

/** ���������� ��� - �������� ��������*/
std::wstring& VActivateAction::GetName()
{
	return name;
}

//------------------------------------ ������ ������ VDeactivateAction ----------------------------------


/** �����������*/
VDeactivateAction::VDeactivateAction(VActiveObjectPtr in_data, const std::wstring& in_name)
{
	if (in_data)
	{
		data=in_data;	
		name=in_name;	
	}
	else
	{
		throw NULL_POINTER;
	}
}
/** ����������*/
void VDeactivateAction::Execute()
{
	data->SetActive(false);	
}
/** ���������� true, ���� �������� ��������*/
bool VDeactivateAction::Allowed() const
{	
	return data->GetActive();
}

/** ���������� ��� - �������� ��������*/
std::wstring& VDeactivateAction::GetName()
{
	return name;
}

//---------------------------------- ������ ������ VSetSlopeType ----------------------------------------

/** �����������*/
VSetSlopeType::VSetSlopeType(VSlopeDataPtr in_data, PARAMETER_TYPE in_data_type, const std::wstring& in_name)
{
	if (in_data)
	{
		data=in_data;
		data_type=in_data_type;
		name=in_name;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VSetSlopeType::Execute()
{
	data->SetType(data_type);	
}

/** ���������� true, ���� �������� ��������*/
bool VSetSlopeType::Allowed() const
{	
	return data->GetType()!=data_type;
}

//---------------------------------- ������ ������ VSetHeightType ----------------------------------------

/** �����������*/
VSetHeightType::VSetHeightType(VHeightDataPtr in_data, PARAMETER_TYPE in_data_type, const std::wstring& in_name)
{
	if (in_data)
	{
		data=in_data;
		data_type=in_data_type;
		name=in_name;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VSetHeightType::Execute()
{
	data->SetType(data_type);	
}

/** ���������� true, ���� �������� ��������*/
bool VSetHeightType::Allowed() const
{	
	return data->GetType()!=data_type;
}

//--------------------------------- ������ ������ VSelectionToSideway --------------------------------------

/** �����������*/
VSelectionToSideway::VSelectionToSideway(VDataProviderPtr in_data, VSelectionInfoPtr in_selection, 
	uint32_t in_reason, const std::wstring& in_name)
{
	if (in_data && in_selection)
	{
		data=in_data;
		selection=in_selection;
		reason=in_reason;
		name=in_name;				
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VSelectionToSideway::Execute()
{	
	double start=selection->GetStartX();
	double end=selection->GetEndX();
	data->AddXY(0, std::min<double>(start, end), 0, fabs(end-start), 0, L"", reason);
	selection->Reset();
}

/** ���������� true, ���� �������� ��������*/
bool VSelectionToSideway::Allowed() const
{
	return selection->Valid();
}

//------------------------------------------- ������ ������ VExtractStationWay ----------------------------------

/** �����������*/
VExtractStationWay::VExtractStationWay(VDataSetPtr in_data_set, VIBDatabasePtr in_database, 
	VIBTransactionPtr in_transaction, VSelectionInfoPtr in_selection, 
	VAUISelectItemDialogPtr in_select_dialog, uint32_t in_station_code, const std::string& in_station_name, 
	const std::string& in_park_name, const std::wstring& in_name)
{
	if (in_data_set && in_selection)
	{
		data_set=in_data_set;
		database=in_database;
		transaction=in_transaction;
		selection=in_selection;		
		name=in_name;		
		station_code=in_station_code;
		station_name=in_station_name;
		park_name=in_park_name;		
		select_dialog=in_select_dialog;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VExtractStationWay::Execute()
{
	//�������� ������ ����� � �����
	std::string query=boost::str(boost::format(
		"SELECT a.CODE FROM STW_WAY a inner join STW_PARK b on b.ID=a.STW_PARK_ID inner join ADM_STATION s on s.id=b.ADM_STATION_ID where s.code='%d' and b.NAME='%s'")
		% station_code % park_name);
	std::vector<std::wstring> list;			
	GetSingleField(database, *transaction, query, list);
	select_dialog->SetList(list);
	//�������� ������
	if (select_dialog->ShowModal(L"����� ������������ ����", L"����� ����"))
	{
		double start=selection->GetStartX();
		double end=selection->GetEndX();
		//���������� ����� ����
		query=boost::str(boost::format(
			"SELECT stw_way.code FROM STW_WAY a inner join STW_PARK b on b.ID=a.STW_PARK_ID inner join ADM_STATION s on s.id=b.ADM_STATION_ID where s.code='%d' and b.NAME='%s' and a.code='%s'")
		% station_code % park_name % wstring_to_string(select_dialog->GetSelectedItem()));

		list.clear();

		GetSingleField(database, *transaction, query, list);
		if (list.size()==1)
		{
			std::wstring way_code=list.at(0);
			
			//�������� ������� � ����� ����
			VWaySection section;
			section.start=data_set->GetFileAprinfo().GetRealCoord(selection->GetStartX());
			section.end=data_set->GetFileAprinfo().GetRealCoord(selection->GetEndX());
			VDataSet target;
			VFileName fn = data_set->GetFileName();
			fn.SetShortName(string_to_wstring(GenFileName(station_code, wstring_to_string(way_code))));
			target.CreateFile(fn);
			target.CopyFrom(*data_set, section, NULL);		
			target.SetStationInfo(station_code, station_name.c_str(), park_name.c_str(), wstring_to_string(way_code).c_str());
			target.Save();								
		}			
	}	
}

/** ���������� true, ���� �������� ��������*/
bool VExtractStationWay::Allowed() const
{
	return selection->Valid();
}

//------------------------------------------- ������ ������ VAttachStationObject ------------------------------

/** �����������*/
VAttachStationObject::VAttachStationObject(VDataProviderPtr in_mark_data, 
	VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, VAUISelectItemDialogPtr in_select_dialog, 
	uint32_t in_station_code, const std::string& in_park_name, const std::string& in_way_code, const std::wstring& in_name)
{
	if (in_mark_data)
	{
		mark_data=in_mark_data;		
		database=in_database;
		transaction=in_transaction;
		select_dialog=in_select_dialog;		
		station_code=in_station_code;
		park_name=in_park_name;
		way_code=in_way_code;
		name=in_name;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VAttachStationObject::Execute()
{
	//��������� ID �����. ��� ����� ������ �� ������ �������, ����� ��������� ���������.
	size_t selected_index=mark_data->GetSelectedPoint();
	uint32_t mark_id=low_word(mark_data->GetFlags(selected_index));

	//�������� ������ ����� � �����
	std::string query=boost::str(boost::format(
		"select b.STYPE || ' - ' || c.NAME "
		"from STW_POINT_TO_OBJECT a "
		"inner join ENU_POINT_OBJECT_TYPE b on b.ID=a.ENU_POINT_OBJECT_TYPE_ID "
		"inner join STW_OBJECT c on c.ID=a.STW_OBJECT_ID "
		"inner join STW_WAY d on d.ID=a.STW_WAY_ID "
		"inner join STW_PARK e on e.ID=d.STW_PARK_ID "
		"inner join ADM_STATION f on f.id=e.ADM_STATION_ID "
		"where f.code='%s' and e.NAME='%s' and d.CODE='%s' and a.ID not in " 
			"("
			"	SELECT c2.id FROM STW_OPMARK_TO_POINT a2 "
			"	inner join STW_OPMARK b2 on b2.ID=a2.STW_OPMARK_ID "
			"	inner join STW_POINT_TO_OBJECT c2 on c2.ID=a2.STW_POINT_TO_OBJECT_ID "
			"	inner join STW_OBJECT d2 on d2.ID=c2.STW_OBJECT_ID "
			"	inner join ENU_POINT_OBJECT_TYPE e2 on e2.ID=c2.ENU_POINT_OBJECT_TYPE_ID "
			"	where b2.stw_trail_id=(select stw_trail_id from STW_OPMARK where id=%d)"
			" "
		"order by d.CODE")
		% station_code % park_name % way_code % mark_id);
	std::vector<std::wstring> list;			
	GetSingleField(database, *transaction, query, list);

	if (!select_dialog)
		throw NULL_POINTER;

	select_dialog->SetList(list);
	//�������� ������
	if (select_dialog->ShowModal(L"����� ������� ��������", L"���� - ������ - �����"))
	{
		//���������� ID �����
		query=boost::str(boost::format(
			"select a.id from STW_POINT_TO_OBJECT a "
			"inner join ENU_POINT_OBJECT_TYPE b on b.ID=a.ENU_POINT_OBJECT_TYPE_ID "
			"inner join STW_OBJECT c on c.ID=a.STW_OBJECT_ID "
			"inner join STW_WAY d on d.ID=a.STW_WAY_ID "
			"inner join STW_PARK e on e.ID=d.STW_PARK_ID "
			"inner join ADM_STATION f on f.id=e.ADM_STATION_ID "
			"where f.code='%s' and e.NAME='%s' and d.CODE='%s' and (b.STYPE || ' - ' || c.NAME)='%s' order by d.CODE")
		% station_code % park_name % way_code % wstring_to_string(select_dialog->GetSelectedItem()));

		list.clear();
		GetSingleField(database, *transaction, query, list);		

		//�����������		
		query=boost::str(boost::format(
			"insert into stw_opmark_to_point(STW_POINT_TO_OBJECT_id, STW_OPMARK_id) values('%s', '%d')")
			% wstring_to_string(list.at(0)) % static_cast<uint32_t>(mark_id));
		database->ExecuteStatementImmediate(*transaction, query.c_str());					
	}	
}
/** ���������� true, ���� �������� ��������*/
bool VAttachStationObject::Allowed() const
{
	return mark_data->GetActive() && mark_data->GetSelectedPoint()!=BAD_INDEX;
}

//------------------------------------------- ������ ������ VDetachStationObject ------------------------------

/** �����������*/
VDetachStationObject::VDetachStationObject(VDataProviderPtr in_mark_data,
	VIBDatabasePtr in_database, VIBTransactionPtr in_transaction,
	uint32_t in_station_code, const std::string& in_park_name, const std::wstring& in_name)
{
	if (in_mark_data)
	{
		mark_data=in_mark_data;		
		database=in_database;
		transaction=in_transaction;
		station_code=in_station_code;
		park_name=in_park_name;
		name=in_name;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VDetachStationObject::Execute()
{	
	//���������� ID �����.
	size_t selected_index=mark_data->GetSelectedPoint();
	uint32_t mark_id=low_word(mark_data->GetFlags(selected_index));

	//������� ��� ��������		
	std::string query=boost::str(boost::format(
		"delete from stw_opmark_to_point where STW_OPMARK_id='%d'")
		 % static_cast<uint32_t>(mark_id));
	database->ExecuteStatementImmediate(*transaction, query.c_str());				
}

/** ���������� true, ���� �������� ��������*/
bool VDetachStationObject::Allowed() const
{
	return mark_data->GetActive() && mark_data->GetSelectedPoint()!=BAD_INDEX;
}

//------------------------------------------- ������ ������ VSidewayToSelection --------------------------------

/** �����������*/
VSidewayToSelection::VSidewayToSelection(VDataProviderPtr in_data, VSelectionInfoPtr in_selection, const std::wstring& in_name)
{
	if (in_data && in_selection)
	{
		data=in_data;
		selection=in_selection;
		name=in_name;
	}
}

/** ����������*/
void VSidewayToSelection::Execute()
{	
	selection->Reset();
	selection->SetStart(data->GetX(data->GetSelectedPoint()), 0);
	selection->SetEnd(data->GetX(data->GetSelectedPoint()) + data->GetSizeX(data->GetSelectedPoint()), 0);
	data->DeleteItem(data->GetSelectedPoint());
	data->DeselectPoint();		
}

/** ���������� true, ���� �������� ��������*/
bool VSidewayToSelection::Allowed() const
{
	return data->GetActive() && data->GetSelectedPoint()!=BAD_INDEX;
}

/** ��������� �������*/
bool VSidewayToSelection::ProcessEvent(const VEvent& in_event)
{
	if (Allowed())
	{
		if (in_event.GetCode()==EVENT_MOUSE_LEFT_DOWN)
		{	
			Execute();				
			return true;
		}		
	}
	return false;
};

//------------------------------------- ������ ������ VActivateNextCurve ---------------------------------------

/** �����������*/
VActivateNextCurve::VActivateNextCurve(VDataSetPtr in_data_set, VDrawing * in_drawing,	
	const std::wstring& in_name, PARAMETER_TYPE in_mark_type)
{
	if (in_data_set && in_drawing)
	{		
		data_set=in_data_set;		
		name=in_name;		
		drawing=in_drawing;
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VActivateNextCurve::Execute()
{
	//�������� ������ �������� ������
	size_t index=data_set->GetCurves(mark_type).GetActiveCurve();	
	data_set->GetCurves(mark_type).DeactivateCurve();
	//��������� � ��������� ������
	++index;
	if (index>=data_set->GetCurves(mark_type).GetCount())
		index=0;		
	data_set->GetCurves(mark_type).ActivateCurve(index);		
	double abscoord=(data_set->GetAbsCoord().GetItem(data_set->GetCurves(mark_type).GetItem(index).GetPlanMark(0).index)+
		data_set->GetAbsCoord().GetItem(data_set->GetCurves(mark_type).GetItem(index).GetPlanMark(data_set->GetCurves(mark_type).GetItem(index).GetMarkCount()-1).index))/2;
	drawing->ScrollToHorz(std::max<double>(abscoord-drawing->GetPageSize()/2, drawing->GetHorzMin()));
}

/** ���������� true, ���� �������� ��������*/
bool VActivateNextCurve::Allowed() const
{	
	size_t index=data_set->GetCurves(mark_type).GetActiveCurve();
	return index!=BAD_INDEX && data_set->GetCurves(mark_type).GetItem(index).CheckMarks();
}

//------------------------------------ ������ ������ VNormalizeCurve ------------------------------------------

/** �����������*/
VNormalizeCurve::VNormalizeCurve(VDataSetPtr in_data_set, const std::wstring& in_name, 
	PARAMETER_TYPE in_mark_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;		
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VNormalizeCurve::Execute()
{	
	size_t curve_index=data_set->GetCurves(mark_type).GetActiveCurve();
	data_set->GetCurves(mark_type).GetItem(curve_index).NormalizeMarks(*data_set);
	data_set->GetCurves(mark_type).GetItem(curve_index).Drop();
	data_set->SetChanged(mark_type);
	//��������� ���������� � �����		
	data_set->UpdatePlanOffset(data_set->GetCurves(mark_type).GetItem(curve_index).GetPlanMark(1).index, 
		mark_type==PRM_MODEL_CURVES);
	//��������� ������������ ���������
	if (mark_type==PRM_MODEL_CURVES)
		data_set->UpdateAnpModel();
	data_set->UpdateDiffLevel(mark_type==PRM_MODEL_CURVES);
}

/** ���������� true, ���� �������� ��������*/
bool VNormalizeCurve::Allowed() const
{
	size_t active_index=data_set->GetCurves(mark_type).GetActiveCurve();
	return active_index!=BAD_INDEX && data_set->GetCurves(mark_type).GetItem(active_index).PlanMarksValid();
}

//------------------------------------- ������ ������ VOptimizeCurve -----------------------------------------

/** �����������*/
VOptimizeCurve::VOptimizeCurve(VDataSetPtr in_data_set, const std::wstring& in_name,
	PARAMETER_TYPE in_mark_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VOptimizeCurve::Execute()
{
	size_t curve_index=data_set->GetCurves(mark_type).GetActiveCurve();
	data_set->GetCurves(mark_type).GetItem(curve_index).OptimizeMarks(*data_set);
	data_set->GetCurves(mark_type).GetItem(curve_index).Drop();				
	data_set->UpdatePlanOffset(data_set->GetCurves(mark_type).GetItem(curve_index).GetPlanMark(1).index, 
		mark_type==PRM_MODEL_CURVES);
	data_set->SetChanged(mark_type);
	//��������� ������������ ���������
	if (mark_type==PRM_MODEL_CURVES)
		data_set->UpdateAnpModel();
}

/** ���������� true, ���� �������� ��������*/
bool VOptimizeCurve::Allowed() const
{
	size_t active_index=data_set->GetCurves(mark_type).GetActiveCurve();
	return active_index!=BAD_INDEX && data_set->GetCurves(mark_type).GetItem(active_index).PlanMarksValid();
}

//------------------------------------ ������ ������ VIncreaseLevelAction ---------------------------------------

/** �����������*/
VIncreaseLevelAction::VIncreaseLevelAction(VDataSetPtr in_data_set, const std::wstring& in_name,
	PARAMETER_TYPE in_mark_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VIncreaseLevelAction::Execute()
{
	size_t curve_index=data_set->GetCurves(mark_type).GetActiveCurve();
	data_set->GetCurves(mark_type).GetItem(curve_index).IncreaseLevel(OPTIONS.GetOption(L"LEVEL_QUANTUM").GetValue<double>());
	data_set->GetCurves(mark_type).GetItem(curve_index).Drop();
	data_set->SetChanged(mark_type);
	//��������� ������������ ���������
	if (mark_type==PRM_MODEL_CURVES)
		data_set->UpdateAnpModel();
	data_set->UpdateDiffLevel(mark_type==PRM_MODEL_CURVES);
}

/** ���������� true, ���� �������� ��������*/
bool VIncreaseLevelAction::Allowed() const
{
	size_t curve_index=data_set->GetCurves(mark_type).GetActiveCurve();
	return curve_index!=BAD_INDEX && data_set->GetCurves(mark_type).GetItem(curve_index).LevelMarksValid();
}

//------------------------------------ ������ ������ VDecreaseLevelAction ---------------------------------------

/** �����������*/
VDecreaseLevelAction::VDecreaseLevelAction(VDataSetPtr in_data_set, const std::wstring& in_name,
	PARAMETER_TYPE in_mark_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VDecreaseLevelAction::Execute()
{
	size_t curve_index=data_set->GetCurves(mark_type).GetActiveCurve();
	data_set->GetCurves(mark_type).GetItem(curve_index).DecreaseLevel(OPTIONS.GetOption(L"LEVEL_QUANTUM").GetValue<double>());
	data_set->GetCurves(mark_type).GetItem(curve_index).Drop();
	data_set->SetChanged(mark_type);
	//��������� ������������ ���������
	if (mark_type==PRM_MODEL_CURVES)
		data_set->UpdateAnpModel();
	data_set->UpdateDiffLevel(mark_type==PRM_MODEL_CURVES);
}

/** ���������� true, ���� �������� ��������*/
bool VDecreaseLevelAction::Allowed() const
{
	size_t curve_index=data_set->GetCurves(mark_type).GetActiveCurve();
	return curve_index!=BAD_INDEX && data_set->GetCurves(mark_type).GetItem(curve_index).LevelMarksValid();
}

//-------------------------------------- ������ ������ VFixLevelCoordinates -------------------------------------

/** �����������*/
VFixLevelCoordinates::VFixLevelCoordinates(VDataSetPtr in_data_set, const std::wstring& in_name,
	PARAMETER_TYPE in_mark_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VFixLevelCoordinates::Execute()
{	
	size_t curve_index=data_set->GetCurves(mark_type).GetActiveCurve();
	if (data_set->GetCurves(mark_type).GetItem(curve_index).GetLevelMarkCount()!=data_set->GetCurves(mark_type).GetItem(curve_index).GetMarkCount())
		data_set->GetCurves(mark_type).GetItem(curve_index).TransferMarksToLevel(*data_set);
	data_set->GetCurves(mark_type).GetItem(curve_index).FixLevelCoordinates(*data_set);
	data_set->GetCurves(mark_type).GetItem(curve_index).Drop();
	data_set->SetChanged(mark_type);
	//��������� ������������ ���������
	if (mark_type==PRM_MODEL_CURVES)
		data_set->UpdateAnpModel();
	data_set->UpdateDiffLevel(mark_type==PRM_MODEL_CURVES);
}

/** ���������� true, ���� �������� ��������*/
bool VFixLevelCoordinates::Allowed() const
{
	return data_set->GetCurves(mark_type).GetActiveCurve()!=BAD_INDEX;
}

//-------------------------------------- ������ ������ VCreateLevelOptimalAction -------------------------------------

/** �����������*/
VCreateLevelOptimalAction::VCreateLevelOptimalAction(VDataSetPtr in_data_set, const std::wstring& in_name)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VCreateLevelOptimalAction::Execute()
{
	size_t curve_index=data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve();
	data_set->GetCurves(PRM_MODEL_CURVES).GetItem(curve_index).CreateLevelOptimal(*data_set);
	data_set->GetCurves(PRM_MODEL_CURVES).GetItem(curve_index).Drop();
	data_set->SetChanged(PRM_MODEL_CURVES);
	//��������� ������������ ���������
	data_set->UpdateAnpModel();
	data_set->UpdateDiffLevel(true);
}

/** ���������� true, ���� �������� ��������*/
bool VCreateLevelOptimalAction::Allowed() const
{
	return data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve()!=BAD_INDEX;
}


//-------------------------------------- ������ ������ VCreateLevelRationalAction -------------------------------------

/** �����������*/
VCreateLevelRationalAction::VCreateLevelRationalAction(VDataSetPtr in_data_set, const std::wstring& in_name)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VCreateLevelRationalAction::Execute()
{	
	size_t curve_index=data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve();
	data_set->GetCurves(PRM_MODEL_CURVES).GetItem(curve_index).CreateLevelRational(*data_set);
	data_set->GetCurves(PRM_MODEL_CURVES).GetItem(curve_index).Drop();
	data_set->SetChanged(PRM_MODEL_CURVES);
	//��������� ������������ ���������
	data_set->UpdateAnpModel();
	data_set->UpdateDiffLevel(true);	
}

/** ���������� true, ���� �������� ��������*/
bool VCreateLevelRationalAction::Allowed() const
{
	return data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve()!=BAD_INDEX;
}


//-------------------------------------- ������ ������ VCreateLevelFactAction -------------------------------------

/** �����������*/
VCreateLevelFactAction::VCreateLevelFactAction(VDataSetPtr in_data_set, const std::wstring& in_name)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VCreateLevelFactAction::Execute()
{
	size_t curve_index=data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve();
	data_set->GetCurves(PRM_MODEL_CURVES).GetItem(curve_index).CreateLevelFact(*data_set);
	data_set->GetCurves(PRM_MODEL_CURVES).GetItem(curve_index).Drop();
	data_set->SetChanged(PRM_MODEL_CURVES);
	//��������� ������������ ���������
	data_set->UpdateAnpModel();
	data_set->UpdateDiffLevel(true);
}

/** ���������� true, ���� �������� ��������*/
bool VCreateLevelFactAction::Allowed() const
{
	return data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve()!=BAD_INDEX;
}

//------------------------------------- ������ ������ VCreateLevelOptimalAllCurves ------------------------------------

/** �����������*/
VCreateLevelOptimalAllCurves::VCreateLevelOptimalAllCurves(VDataSetPtr in_data_set, const std::wstring& in_name)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VCreateLevelOptimalAllCurves::Execute()
{
	for (size_t i=0; i<data_set->GetCurves(PRM_MODEL_CURVES).GetCount(); ++i)
	{
		data_set->GetCurves(PRM_MODEL_CURVES).GetItem(i).CreateLevelOptimal(*data_set);
		data_set->GetCurves(PRM_MODEL_CURVES).GetItem(i).Drop();
	}
	data_set->SetChanged(PRM_MODEL_CURVES);
	//��������� ������������ ���������
	data_set->UpdateAnpModel();
	data_set->UpdateDiffLevel(true);
}

/** ���������� true, ���� �������� ��������*/
bool VCreateLevelOptimalAllCurves::Allowed() const
{
	return data_set->GetCurves(PRM_MODEL_CURVES).GetCount()>0 && 
		   data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve()==BAD_INDEX;
}

//------------------------------------- ������ ������ VCreateLevelRationalAllCurves ------------------------------------

/** �����������*/
VCreateLevelRationalAllCurves::VCreateLevelRationalAllCurves(VDataSetPtr in_data_set, const std::wstring& in_name)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VCreateLevelRationalAllCurves::Execute()
{
	for (size_t i=0; i<data_set->GetCurves(PRM_MODEL_CURVES).GetCount(); ++i)
	{
		data_set->GetCurves(PRM_MODEL_CURVES).GetItem(i).CreateLevelRational(*data_set);
		data_set->GetCurves(PRM_MODEL_CURVES).GetItem(i).Drop();
	}
	data_set->SetChanged(PRM_MODEL_CURVES);
	//��������� ������������ ���������
	data_set->UpdateAnpModel();
	data_set->UpdateDiffLevel(true);
}

/** ���������� true, ���� �������� ��������*/
bool VCreateLevelRationalAllCurves::Allowed() const
{
	return data_set->GetCurves(PRM_MODEL_CURVES).GetCount()>0 && 
		   data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve()==BAD_INDEX;
}

//------------------------------------- ������ ������ VCreateLevelFactAllCurves ------------------------------------

/** �����������*/
VCreateLevelFactAllCurves::VCreateLevelFactAllCurves(VDataSetPtr in_data_set, const std::wstring& in_name)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VCreateLevelFactAllCurves::Execute()
{
	for (size_t i=0; i<data_set->GetCurves(PRM_MODEL_CURVES).GetCount(); ++i)
	{
		data_set->GetCurves(PRM_MODEL_CURVES).GetItem(i).CreateLevelFact(*data_set);
		data_set->GetCurves(PRM_MODEL_CURVES).GetItem(i).Drop();
	}
	data_set->SetChanged(PRM_MODEL_CURVES);
	//��������� ������������ ���������
	data_set->UpdateAnpModel();
	data_set->UpdateDiffLevel(true);
}

/** ���������� true, ���� �������� ��������*/
bool VCreateLevelFactAllCurves::Allowed() const
{
	return data_set->GetCurves(PRM_MODEL_CURVES).GetCount()>0 && 
		   data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve()==BAD_INDEX;
}


//------------------------------------ ������ ������ VApplyActiveCurve -----------------------------------------

/** �����������*/
VApplyActiveCurve::VApplyActiveCurve(VDataSetPtr in_data_set, const std::wstring& in_name, 
	PARAMETER_TYPE in_mark_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;		
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VApplyActiveCurve::Execute()
{	
	data_set->GetCurves(mark_type).DeactivateCurve();			
}

/** ���������� true, ���� �������� ��������*/
bool VApplyActiveCurve::Allowed() const
{
	size_t active_index=data_set->GetCurves(mark_type).GetActiveCurve();
	return active_index!=BAD_INDEX && data_set->GetCurves(mark_type).GetItem(active_index).CheckMarks();
}

//--------------------------------- ������ ������ VDeleteActiveCurveAction --------------------------

/** �����������*/
VDeleteActiveCurveAction::VDeleteActiveCurveAction(VDataSetPtr in_data_set, const std::wstring& in_name,
	PARAMETER_TYPE in_mark_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VDeleteActiveCurveAction::Execute()
{	
	size_t curve_index=data_set->GetCurves(mark_type).GetActiveCurve();
	size_t point_index=data_set->GetCurves(mark_type).GetItem(curve_index).GetMarkCount()>0 ?
		data_set->GetCurves(mark_type).GetItem(curve_index).GetPlanMark(0).index : 
		data_set->GetCurves(mark_type).GetItem(curve_index).GetLevelMark(0).index;
	data_set->GetCurves(mark_type).DeleteActiveCurve();
	data_set->SetChanged(mark_type);		
	data_set->UpdatePlanOffset(point_index, 
		mark_type==PRM_MODEL_CURVES);
	//��������� ������������ ���������
	if (mark_type==PRM_MODEL_CURVES)
		data_set->UpdateAnpModel();
	//��������� ���������� ������ �� ��������
	data_set->UpdateDiffLevel(mark_type==PRM_MODEL_CURVES);
}

/** ���������� true, ���� �������� ��������*/
bool VDeleteActiveCurveAction::Allowed() const
{
	return data_set->GetCurves(mark_type).GetActiveCurve()!=BAD_INDEX;
}

//-------------------------------- ������ ������ VDeleteAllCurvesAction-----------------------------------
/** �����������*/
VDeleteAllCurvesAction::VDeleteAllCurvesAction(VDataSetPtr in_data_set, const std::wstring& in_name,
	PARAMETER_TYPE in_mark_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VDeleteAllCurvesAction::Execute()
{	
	data_set->GetCurves(mark_type).DeactivateCurve();
	data_set->DeleteCurves(mark_type);		
}

/** ���������� true, ���� �������� ��������*/
bool VDeleteAllCurvesAction::Allowed() const
{
	return data_set->GetCurves(mark_type).GetCount()>0;
}


//-------------------------------- ������ ������ VDeleteAllLinesAction-----------------------------------
/** �����������*/
VDeleteAllLinesAction::VDeleteAllLinesAction(VDataSetPtr in_data_set, const std::wstring& in_name, 
	PARAMETER_TYPE in_mark_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name=in_name;
		mark_type=in_mark_type;	
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VDeleteAllLinesAction::Execute()
{		
	data_set->DeleteLines(mark_type);		
}

/** ���������� true, ���� �������� ��������*/
bool VDeleteAllLinesAction::Allowed() const
{
	return data_set->GetLineMarks().GetCount()>0;
}


//------------------------------- ������ ������ VCopyCurvesAction --------------------------------

/** �����������*/
VCopyCurvesAction::VCopyCurvesAction(VDataSetPtr in_data_set, const std::wstring& in_name, 
	PARAMETER_TYPE in_mark_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;			
		name=in_name;		
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VCopyCurvesAction::Execute()
{	
	data_set->CopyCurves(mark_type);
}

/** ���������� true, ���� �������� ��������*/
bool VCopyCurvesAction::Allowed() const
{
	return true;
}

//------------------------------- ������ ������ VAutomarkCurvesAction --------------------------------

/** �����������*/
VAutomarkCurvesAction::VAutomarkCurvesAction(VDataSetPtr in_data_set,
	VAUIProgressDialogPtr in_progress_dialog, const std::wstring& in_name, PARAMETER_TYPE in_mark_type)
{
	if (in_data_set && in_progress_dialog)
	{
		data_set=in_data_set;		
		progress_dialog=in_progress_dialog;
		name=in_name;		
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VAutomarkCurvesAction::Execute()
{	
	data_set->GetCurves(mark_type).DeactivateCurve();
	data_set->SearchCurves(progress_dialog, mark_type);	
}

/** ���������� true, ���� �������� ��������*/
bool VAutomarkCurvesAction::Allowed() const
{
	return true;
}

//-------------------------------- ������ ������ VTransferPassportCurvesAction ------------------------------------

/** �����������*/
VTransferPassportCurvesAction::VTransferPassportCurvesAction(VDataSetPtr in_data_set, VIBDatabasePtr in_database,
	VAUIProgressDialogPtr in_progress_dialog, const std::wstring& in_name, PARAMETER_TYPE in_mark_type)
{
	if (in_data_set && in_database && in_progress_dialog)
	{
		data_set=in_data_set;		
		database=in_database;
		progress_dialog=in_progress_dialog;
		name=in_name;		
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VTransferPassportCurvesAction::Execute()
{
	data_set->GetCurves(mark_type).DeactivateCurve();
	data_set->TransferPassportCurves(database, progress_dialog, mark_type);
}

/** ���������� true, ���� �������� ��������*/
bool VTransferPassportCurvesAction::Allowed() const
{
	return true;
}


//-------------------------------- ������ ������ VTransferMeasuredCurvesAction ------------------------------------

/** �����������*/
VTransferMeasuredCurvesAction::VTransferMeasuredCurvesAction(VDataSetPtr in_data_set, VIBDatabasePtr in_database,
	VAUIProgressDialogPtr in_progress_dialog, const std::wstring& in_name, PARAMETER_TYPE in_mark_type)
{
	if (in_data_set && in_database && in_progress_dialog)
	{
		data_set=in_data_set;		
		database=in_database;
		progress_dialog=in_progress_dialog;
		name=in_name;		
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VTransferMeasuredCurvesAction::Execute()
{
	data_set->GetCurves(mark_type).DeactivateCurve();
	data_set->TransferMeasuredCurves(database, progress_dialog, mark_type);
}

/** ���������� true, ���� �������� ��������*/
bool VTransferMeasuredCurvesAction::Allowed() const
{
	return true;
}

//----------------------------------- ������ ������ VOptimizeCurvesAction -------------------------------

/** �����������*/
VOptimizeCurvesAction::VOptimizeCurvesAction(VDataSetPtr in_data_set, VAUIProgressDialogPtr in_progress_dialog, 
	const std::wstring& in_name, PARAMETER_TYPE in_mark_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		progress_dialog=in_progress_dialog;
		name=in_name;		
		mark_type=in_mark_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VOptimizeCurvesAction::Execute()
{	
	data_set->OptimizeCurves(progress_dialog, mark_type);
	data_set->SetChanged(mark_type);
}

/** ���������� true, ���� �������� ��������*/
bool VOptimizeCurvesAction::Allowed() const
{
	return data_set->GetCurves(mark_type).GetCount()>0;
}

//------------------------------------ ������ ������ VSetCurveStatusAction ----------------------------

/** �����������*/
VSetCurveStatusAction::VSetCurveStatusAction(VDataSetPtr in_data_set, const std::wstring& in_name, 
	VAUISelectItemDialogPtr in_select_dialog)
{
	if (in_data_set)
	{
		data_set=in_data_set;	
		name=in_name;			
		select_dialog=in_select_dialog;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VSetCurveStatusAction::Execute()
{
	std::vector<std::wstring> list;			
	list.push_back(string_to_wstring(__status__[0]));
	list.push_back(string_to_wstring(__status__[1]));
	list.push_back(string_to_wstring(__status__[2]));
	list.push_back(string_to_wstring(__status__[3]));
	select_dialog->SetList(list);
	//�������� ������
	if (select_dialog->ShowModal(L"���������� ������ ������", L"������ ������"))
	{
		size_t index=data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve();
		data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).SetModelStatus(select_dialog->GetSelectedPos());
		data_set->SetChanged(PRM_MODEL_CURVES);
	}	
}

/** ���������� true, ���� �������� ��������*/
bool VSetCurveStatusAction::Allowed() const
{
	return data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve()!=BAD_INDEX;
}

//------------------------------------ ������ ������ VSetCurveStatusAction ----------------------------

/** �����������*/
VSetCurveCommentAction::VSetCurveCommentAction(VDataSetPtr in_data_set, const std::wstring& in_name, 
	VAUISelectItemDialogPtr in_select_dialog, VAUIInputDialogPtr in_input_dialog)
{
	if (in_data_set)
	{
		data_set=in_data_set;	
		name=in_name;			
		select_dialog=in_select_dialog;
		input_dialog=in_input_dialog;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VSetCurveCommentAction::Execute()
{
	std::vector<std::wstring> list;			
	list.push_back(string_to_wstring(__comment__[0]));
	list.push_back(string_to_wstring(__comment__[1]));
	list.push_back(string_to_wstring(__comment__[2]));
	list.push_back(string_to_wstring(__comment__[3]));
	list.push_back(string_to_wstring(__comment__[4]));
	list.push_back(string_to_wstring(__comment__[5]));
	list.push_back(string_to_wstring(__comment__[6]));
	select_dialog->SetList(list);
	//�������� ������
	if (select_dialog->ShowModal(L"���������� �����������", L"�����������"))
	{
		size_t index=data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve();
		if (select_dialog->GetSelectedItem().compare(list.back())==0)
		{
			if (input_dialog->ShowModal(L"���������� �����������", L""))
			{
				data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).SetModelComment(wstring_to_string(input_dialog->GetValue()).c_str());
				data_set->SetChanged(PRM_MODEL_CURVES);
			}
		}
		else
		{
			data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).SetModelComment(wstring_to_string(select_dialog->GetSelectedItem()).c_str());
			data_set->SetChanged(PRM_MODEL_CURVES);
		}
	}	
}

/** ���������� true, ���� �������� ��������*/
bool VSetCurveCommentAction::Allowed() const
{
	return data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve()!=BAD_INDEX;
}

//-------------------------------------- ������ ������ VSetCurveSpeedsAction -------------------------------------

/** �����������*/
VSetCurveSpeedsAction::VSetCurveSpeedsAction(VDataSetPtr in_data_set, const std::wstring& in_name, VAUIPropertyDialogPtr in_property_dialog)
{
	if (in_data_set)
	{
		data_set=in_data_set;	
		name=in_name;			
		property_dialog=in_property_dialog;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VSetCurveSpeedsAction::Execute()
{	
	size_t index=data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve();
	std::wstring Vhigh_bt=data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).GetModelVhigh_bt().format();
	std::wstring Vhigh=data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).GetModelVhigh().format();
	std::wstring Vpass=data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).GetModelVpass().format();
	std::wstring Vcrg=data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).GetModelVcrg().format();
	std::wstring Vavg=data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).GetModelVavg().format();
	property_dialog->Clear();
	property_dialog->AddItemSimple(L"1. ���������� ������ � �������� ������", Vhigh_bt);
	property_dialog->AddItemSimple(L"2. ���������� ������", Vhigh);
	property_dialog->AddItemSimple(L"3. ������������", Vpass);
	property_dialog->AddItemSimple(L"4. ��������", Vcrg);
	property_dialog->AddItemSimple(L"5. ����������������", Vavg);
	if (property_dialog->ShowModal(L"��������", L""))
	{
		Vhigh_bt=property_dialog->GetValue(L"1. ���������� ������ � �������� ������");
		Vhigh=property_dialog->GetValue(L"2. ���������� ������");
		Vpass=property_dialog->GetValue(L"3. ������������");
		Vcrg=property_dialog->GetValue(L"4. ��������");
		Vavg=property_dialog->GetValue(L"5. ����������������");

		data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).SetModelVhigh_bt(speed_t(Vhigh_bt));
		data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).SetModelVhigh(speed_t(Vhigh));
		data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).SetModelVpass(speed_t(Vpass));
		data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).SetModelVcrg(speed_t(Vcrg));
		data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).SetModelVavg(speed_t(Vavg));

		data_set->SetChanged(PRM_MODEL_CURVES);

		//��������� ������������ ���������
		data_set->UpdateAnpModel();		
	}			
}

/** ���������� true, ���� �������� ��������*/
bool VSetCurveSpeedsAction::Allowed() const
{
	return data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve()!=BAD_INDEX;
}

//----------------------------------- ������ ������ VResetCurveSpeedsAction -------------------------------

/** �����������*/
VResetCurveSpeedsAction::VResetCurveSpeedsAction(VDataSetPtr in_data_set, const std::wstring& in_name)
{
	if (in_data_set)
	{
		data_set=in_data_set;	
		name=in_name;					
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VResetCurveSpeedsAction::Execute()
{
	size_t index=data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve();

	size_t start=data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).GetPlanMark(0).index;
	size_t end=data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).GetPlanMark(data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).GetMarkCount()-1).index;

	data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).SetModelVpass(data_set->GetAprinfo().GetMaxPassSpeed(data_set->GetAbsCoord().GetItem(start), data_set->GetAbsCoord().GetItem(end)));
	data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).SetModelVcrg(data_set->GetAprinfo().GetMaxTruckSpeed(data_set->GetAbsCoord().GetItem(start), data_set->GetAbsCoord().GetItem(end)));
	data_set->GetCurves(PRM_MODEL_CURVES).GetItem(index).SetModelVavg(0.75*data_set->GetAprinfo().GetMaxTruckSpeed(data_set->GetAbsCoord().GetItem(start), data_set->GetAbsCoord().GetItem(end)));

	data_set->SetChanged(PRM_MODEL_CURVES);

	//��������� ������������ ���������
	data_set->UpdateAnpModel();
}

/** ���������� true, ���� �������� ��������*/
bool VResetCurveSpeedsAction::Allowed() const
{
	return data_set->GetCurves(PRM_MODEL_CURVES).GetActiveCurve()!=BAD_INDEX;
}


//----------------------------------- ������ ������ VGroupAction ----------------------------------------

/** �����������*/
VGroupAction::VGroupAction(const std::wstring& in_name)
{
	name=in_name;
}

/** ����������*/
void VGroupAction::Execute()
{	
	for (size_t i=0; i<actions.size(); ++i)
	{
		if (actions.at(i)->Allowed())	
			actions.at(i)->Execute();
	}	
}


/** ���������� true, ���� �������� ��������*/
bool VGroupAction::Allowed() const
{
	bool allowed=true;
	for (size_t i=0; i<actions.size(); ++i)
	{
		bool a = actions.at(i)->Allowed();		
		allowed = allowed && a;
	}
	return allowed;
}

/** ��������� �������� � ������*/
void VGroupAction::AddAction(VActionPtr in_action)
{
	actions.push_back(in_action);
}

//------------------------------------ ������ ������ VRefreshDataAction ------------------------

/** �����������*/
VRefreshDataAction::VRefreshDataAction(VDataProviderPtr in_data, const std::wstring& in_name)
{
	if (in_data)
	{
		data=in_data;		
		name=in_name;		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VRefreshDataAction::Execute()
{
	data->RefreshData();	
}

/** ���������� true, ���� �������� ��������*/
bool VRefreshDataAction::Allowed() const
{
	return true;
}

//------------------------------- ������ ������ VCorrectCoordAction --------------------------------

/** �����������*/
VCorrectCoordAction::VCorrectCoordAction(VDataSetPtr in_data_set, VIBDatabasePtr in_database, POINT_TYPE in_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		database = in_database;
		type = in_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VCorrectCoordAction::Execute()
{
	data_set->Autocorrection(database.get(), type);
	data_set->GetCoordPoint().Unload(*data_set);
	data_set->GetCoordPoint().Load(*data_set, POINT_POLE, true);
	data_set->GetCoordPoint().Load(*data_set, POINT_SPALA_MARKER, true);
	data_set->GetCoordPoint().CalcLine(POINT_POLE);
	data_set->GetCoordPoint().CalcLine(POINT_SPALA_MARKER);		
}

/** ���������� true, ���� �������� ��������*/
bool VCorrectCoordAction::Allowed() const
{
	if(data_set->GetCoordPoint().GetCountNotCorrupt(type) > 1)
		return true;
	else
		return false;
}

//------------------------------- ������ ������ VSelectPointAction --------------------------------

/** �����������*/
VSelectPointAction::VSelectPointAction(VDataProviderPtr in_data)
{
	if (in_data)
		data = in_data;
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VSelectPointAction::Execute()
{
	data->RefreshData();
}

/** ���������� true, ���� �������� ��������*/
bool VSelectPointAction::Allowed() const
{
	return data->GetSelectedPoint()!=BAD_INDEX;
}

//------------------------------- ������ ������ VShiftZazorAction --------------------------------

/** �����������*/
VShiftZazorAction::VShiftZazorAction(VDataSetPtr in_data_set, const std::wstring& in_name, VAUIPropertyDialogPtr in_property_dialog)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		name = in_name;
		property_dialog = in_property_dialog;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VShiftZazorAction::Execute()
{
	property_dialog->Clear();
	property_dialog->AddItemSimple(L"����� (�)", L"5");	
	if (property_dialog->ShowModal(L"����� ��������� �������", L""))
	{		
		double len_of_kadr = (data_set->GetAbsCoord().GetLastItem() - data_set->GetAbsCoord().GetItem(0))/data_set->GetAbsCoord().GetCount();
		int value = floor(str_to_double(property_dialog->GetValue(L"����� (�)"))/len_of_kadr);
		data_set->ShiftMagZazor(value);		
	}	
}

/** ���������� true, ���� �������� ��������*/
bool VShiftZazorAction::Allowed() const
{
	return true;
}

//------------------------------------ ������ ������ VAlignParameterMean ---------------------------------

///�����������
VAlignParameterMean::VAlignParameterMean(VSelectionInfoPtr in_selection, VDataSetPtr in_data_set, 
	PARAMETER_TYPE in_data_type_source, PARAMETER_TYPE in_data_type_target, 
	const std::wstring& in_name)
{
	if (in_selection && in_data_set)
	{
		selection=in_selection;
		data_set=in_data_set;
		data_type_source=in_data_type_source;
		data_type_target=in_data_type_target;
		name=in_name;
	}
}

/// ����������
void VAlignParameterMean::Execute()
{
	size_t start_index=data_set->GetAbsCoord().FindIndex(selection->GetStartX(), true);
	size_t end_index=data_set->GetAbsCoord().FindIndex(selection->GetEndX(), true);
	if (start_index > end_index)
		std::swap(start_index,end_index);				
	double shift=data_set->GetParameter(data_type_source).GetMO(start_index, end_index) - 
		         data_set->GetParameter(data_type_target).GetMO(start_index, end_index);
	data_set->GetParameter(data_type_target).Shift(shift);
	data_set->SetChanged(data_type_target);			
}

/// ���������� true, ���� �������� ��������
bool VAlignParameterMean::Allowed() const
{
//	return selection->Valid() && data_set->GetParameter(data_type_source).IsLoaded() && data_set->GetParameter(data_type_target).IsLoaded();
	return selection->Valid() && data_set->IsLoaded(data_type_source) && data_set->IsLoaded(data_type_target);
}

//------------------------------------ ������ ������ VShiftParameter --------------------------------------

/** �����������*/
VShiftParameterAction::VShiftParameterAction(VAxesWeakPtr in_axes, VDataSetPtr in_data_set, 
	PARAMETER_TYPE in_data_type, double in_shift, const std::wstring& in_name)
{
	if (in_data_set)
	{			
		axes=in_axes;
		data_set=in_data_set;		
		data_type=in_data_type;
		shift=in_shift;
		name=in_name;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VShiftParameterAction::Execute()
{		
	data_set->GetParameter(data_type).Shift(shift);
	data_set->SetChanged(data_type);	
}

/** ���������� true, ���� �������� ��������*/
bool VShiftParameterAction::Allowed() const
{
	if (VAxesPtr a=axes.lock())
	{
		//return a->GetActive() && data_set->GetParameter(data_type).IsLoaded();
		return a->GetActive() && data_set->IsLoaded(data_type);
	}
	else
		return false;
}

//------------------------------------ ������ ������ VCorruptParameter --------------------------------------

/** �����������*/
VCorruptParameterAction::VCorruptParameterAction(VSelectionInfoPtr in_selection_info, VDataSetPtr in_data_set, bool in_corrupt,
	PARAMETER_TYPE in_data_type, const std::wstring& in_name)
{
	if (in_selection_info && in_data_set)
	{		
		selection_info=in_selection_info;
		data_set=in_data_set;
		corrupt=in_corrupt;
		data_type=in_data_type;
		name=in_name;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VCorruptParameterAction::Execute()
{	
	size_t start_index=data_set->GetAbsCoord().FindIndex(selection_info->GetStartX(), true);
	size_t end_index=data_set->GetAbsCoord().FindIndex(selection_info->GetEndX(), true);
	if (start_index > end_index)
	{
		size_t tmp=start_index;
		start_index=end_index;
		end_index=tmp;
	}
	if (data_type==PRM_ZAZOR_LEFT || data_type==PRM_ZAZOR_RIGHT || data_type==PRM_MAG_ZAZOR_LEFT || data_type==PRM_MAG_ZAZOR_RIGHT)
	{
		for (size_t i=0; i<data_set->GetZazor(data_type).GetCount(); ++i)
		{
			if (pro::interval<size_t>(start_index, end_index).contains(data_set->GetZazor(data_type).GetItem(i).index))
			{
				data_set->GetZazor(data_type).SetCorrupt(i, corrupt);
				data_set->SetChanged(PRM_CORRUPT);
			}
		}
	}
	else
	{
		for (size_t i=start_index; i<end_index; ++i)
		{
			data_set->GetParameter(data_type).SetCorrupt(i, corrupt);
			data_set->SetChanged(PRM_CORRUPT);		
		}
	}
}

/** ���������� true, ���� �������� ��������*/
bool VCorruptParameterAction::Allowed() const
{
	return selection_info->Valid();
}

//--------------------------------- ������ ������ VAdjustProfileMarkHeight ---------------------------------

VAdjustProfileMarkHeight::VAdjustProfileMarkHeight(VDataSetPtr in_data_set, const std::wstring& in_name)
{
	if (in_data_set)
	{				
		data_set=in_data_set;		
		name=in_name;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ����������*/
void VAdjustProfileMarkHeight::Execute()
{	
	data_set->AdjustProfileMarkHeight();	
}

/** ���������� true, ���� �������� ��������*/
bool VAdjustProfileMarkHeight::Allowed() const
{
	return data_set->GetProfMarks().GetCount()>0;
}
