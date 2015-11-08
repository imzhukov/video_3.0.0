#include "providers.h"
#include "strlib.h"
#include "mathlib.h"
#include "dbfunc.h"
#include "Shiny.h"

//--------------------------------- ������ ������ VGraphInfo ---------------------------------------------

/** �����������*/
VGraphInfo::VGraphInfo(const std::wstring in_graph_name, const std::vector<VDataSetPtr>& in_datasets, 
		   const VDataSetPtr& active_data_set, const std::wstring& in_actual_dir, 
		   const std::wstring& in_actual_way, const std::wstring& in_actual_date)
{
	graph_name=in_graph_name;
	active_file_index=BAD_INDEX;
	for (size_t i=0; i<in_datasets.size(); ++i)
	{
		file_names.push_back(boost::str(boost::wformat(L" %s %s [%d]" ) % 
			in_datasets.at(i)->GetFileShortName() % 
			in_datasets.at(i)->GetRailDate().ToWideStringLocale() % (i+1)));
		if (active_data_set && in_datasets.at(i)->GetFileShortName().compare(active_data_set->GetFileShortName())==0)
			active_file_index=i;
	}
	way_section=boost::str(boost::wformat(L"�����������: %s ����: %s [�� %s]") % in_actual_dir % in_actual_way % in_actual_date);
}

/** ���������� ��� �������*/
const std::wstring& VGraphInfo::GetGraphName() const
{
	return graph_name;
}

/** ���������� ����� ������*/
const std::vector<std::wstring>& VGraphInfo::GetFileNames() const
{
	return file_names;
}
/** ���������� ������ �������� �����*/
size_t VGraphInfo::GetActiveFileIndex() const
{
	return active_file_index;
}

/** ���������� �������� �������*/
const std::wstring& VGraphInfo::GetWaySection() const
{
	return way_section;
}

//---------------------------- ������ ������ VCoordinatesData --------------------------------

/** �����������*/	
VCoordinatesData::VCoordinatesData(const VAprinfo& in_aprinfo, pro::interval<double> in_range) : VXOrderedDataProvider()
{
	//����������� ������ ����������
	std::vector<VAprinfoKmRecord> kms=in_aprinfo.GetKmList(in_range.left_bound().value, in_range.right_bound().value, false);	
	//���������
	for (size_t i=0; i<kms.size(); ++i)
	{		
		xdata.push_back(kms.at(i).base);
		xsizedata.push_back(kms.at(i).end-kms.at(i).base);
		if (fmod((float)kms.at(i).km, float(5.0))==0)
			flags.push_back(COORDINATE_KM_5);
		else
			flags.push_back(COORDINATE_KM);
		kms.at(i).km >=0 ? 
			label.push_back(boost::str(boost::wformat(L"%1%") % kms.at(i).km)) :
			label.push_back(boost::str(boost::wformat(L"(%0.0f)") % fabs((float)kms.at(i).km)));
		//������
		std::vector<VPicket> pickets=in_aprinfo.GetPkList(kms.at(i).km);
		for (size_t j=0; j<pickets.size(); j++)
		{	
			flags.push_back(COORDINATE_PK);		
			xdata.push_back(pickets.at(j).absStart);
			xsizedata.push_back(pickets.at(j).absEnd-pickets.at(j).absStart);
			if (pickets.at(j).pk >= 0)
				label.push_back(boost::str(boost::wformat(L"%0.0f") % fabs((float)pickets.at(j).pk)));
			else
				label.push_back(boost::str(boost::wformat(L"(%0.0f)") % fabs((float)pickets.at(j).pk)));				
		}		
	}	
}

/** ���������� ������ ������*/
size_t VCoordinatesData::GetCount() const
{
	return xdata.size();
}

/** ���������� ���������� �� ��� X*/
double VCoordinatesData::GetX(size_t in_index) const
{
	return xdata.at(in_index);
}

/** ���������� ���������� �� ��� X*/
double VCoordinatesData::GetSizeX(size_t in_index) const
{
	return xsizedata.at(in_index);
}

/** ���������� ���������� �� ��� Y*/
double VCoordinatesData::GetY(size_t in_index) const
{
	return 0;
}

/** ���������� ����� ��� �����, �������� �����*/
uint64_t VCoordinatesData::GetFlags(size_t in_index) const
{
	return flags.at(in_index);
}

/** ���������� ����������� �������� �� Y*/
double VCoordinatesData::GetMinY()
{
	return DBL_MAX;
}

/** ���������� ������������ �������� �� Y*/
double VCoordinatesData::GetMaxY()
{
	return 0-DBL_MAX;
}

/** ���������� ����������� �������� �� X*/
double VCoordinatesData::GetMinX()
{
	return xdata.front();
}

/** ���������� ������������ �������� �� X*/
double VCoordinatesData::GetMaxX()
{
	return xdata.back();
}

/** ���������� �����*/
std::wstring VCoordinatesData::GetLabel(size_t in_index) const
{
	return label.at(in_index);
}

//---------------------------------- ������ ������ VParameterData ----------------------------------

/** �����������*/
VParameterData::VParameterData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type) : VXOrderedDataProvider()
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_type=in_data_type;
		data_set->Load(PRM_ABS_COORD);
		data_set->Load(PRM_CORRUPT);
		data_set->Load(data_type);		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VParameterData::GetCount() const
{
	return data_set->GetParameter(data_type).GetCount();		
}

/** ���������� ���������� �� ��� X*/
double VParameterData::GetX(size_t in_index) const
{
	return data_set->GetAbsCoord().GetItem(in_index);	
}

/** ���������� ���������� �� ��� Y*/
double VParameterData::GetY(size_t in_index) const
{
	return data_set->GetParameter(data_type).GetItem(in_index) ;	
}

/** ���������� ����� ��� �����, �������� ����� */
uint64_t VParameterData::GetFlags(size_t in_index) const
{	
	if (data_set->GetParameter(data_type).IsCorrupt(in_index))
		return 1;
	return 0;
}

/** ������������� �����, �������� �����*/
void VParameterData::SetFlags(size_t in_index, uint64_t in_flags)
{
	data_set->GetParameter(data_type).SetCorrupt(in_index, in_flags != 0);
	data_set->SetChanged(PRM_CORRUPT);		
}

/** ���������� ����������� �������� �� Y*/
double VParameterData::GetMinY() 
{
	return data_set->GetParameter(data_type).GetMin();	
};

/** ���������� ������������ �������� �� Y*/
double VParameterData::GetMaxY() 
{
	return data_set->GetParameter(data_type).GetMax();	
};

/** ���������� ����������� �������� �� X*/
double VParameterData::GetMinX() 
{
	return data_set->GetAbsCoord().GetItem(0);	
};

/** ���������� ������������ �������� �� X*/
double VParameterData::GetMaxX() 
{
	return data_set->GetAbsCoord().GetItem(data_set->GetAbsCoord().GetCount()-1);	
};	

/** ���������� �������� Y �� X*/
double VParameterData::GetYbyX(double in_x)
{
	size_t index=data_set->GetAbsCoord().FindIndex(in_x, true);
	return (index!=BAD_INDEX) ? GetY(index) : 0;
}

/** ���������� ������� �������*/
std::vector<size_t> VParameterData::GetVisible(double in_left, double in_top, double in_right, double in_bottom)
{
	std::vector<size_t> result = VXOrderedDataProvider::GetVisible(in_left, in_top, in_right, in_bottom);
	if (!result.empty())
	{
		result.front()=std::max<size_t>(data_set->GetParameter(data_type).GetFirstDef(), result.front());
		result.back()=std::min<size_t>(data_set->GetParameter(data_type).GetLastDef(), result.back());
	}
	return result;
}


//------------------------------------------ ������ ������ VParameterFunctionData ---------------------------

/** �����������*/	
VParameterFunctionData::VParameterFunctionData(VDataSetPtr in_data_set, const std::wstring& in_function) : VXOrderedDataProvider()
{
	if (in_data_set)
	{		
		data_set=in_data_set;
		in_data_set->Load(PRM_ABS_COORD);
		parameter.Create(0, in_data_set->GetAbsCoord().GetCount(), 0, 0);
		in_data_set->ParseParameter(in_function, parameter);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VParameterFunctionData::GetCount() const
{
	return parameter.GetCount();
}

/** ���������� ���������� �� ��� X*/
double VParameterFunctionData::GetX(size_t in_index) const
{
	return data_set->GetAbsCoord().GetItem(in_index);
}

/** ���������� ���������� �� ��� Y*/
double VParameterFunctionData::GetY(size_t in_index) const
{
	return parameter.GetItem(in_index);
}

/** ���������� ����� ��� �����, �������� �����*/
uint64_t VParameterFunctionData::GetFlags(size_t in_index) const
{
	if (parameter.IsCorrupt(in_index))
		return 1;
	return 0;
}

/** ���������� ����������� �������� �� Y*/
double VParameterFunctionData::GetMinY()
{
	return parameter.GetMin();
}

/** ���������� ������������ �������� �� Y*/
double VParameterFunctionData::GetMaxY()
{
	return parameter.GetMax();
}

/** ���������� ����������� �������� �� X*/
double VParameterFunctionData::GetMinX()
{
	return data_set->GetAbsCoord().GetItem(0);
}

/** ���������� ������������ �������� �� X*/
double VParameterFunctionData::GetMaxX()
{
	return data_set->GetAbsCoord().GetLastItem();
}

/** ���������� �������� Y �� X*/
double VParameterFunctionData::GetYbyX(double in_x)
{
	size_t index=data_set->GetAbsCoord().FindIndex(in_x, true);
	return (index!=BAD_INDEX) ? GetY(index) : 0;
}

/** ���������� ������� �������*/
std::vector<size_t> VParameterFunctionData::GetVisible(double in_left, double in_top, double in_right, double in_bottom)
{
	std::vector<size_t> result = VXOrderedDataProvider::GetVisible(in_left, in_top, in_right, in_bottom);
	result.front()=std::max<size_t>(parameter.GetFirstDef(), result.front());
	result.back()=std::min<size_t>(parameter.GetLastDef(), result.back());
	return result;
}

//---------------------------------- ������ ������ VCurrentSpeedData ----------------------------------

/** �����������*/	
VCurrentSpeedData::VCurrentSpeedData(VDataSetPtr in_data_set) : VXOrderedDataProvider()
{
	if (in_data_set)
	{	
		data_set=in_data_set;
		data_set->Load(PRM_SPEED);
	}
	else
	{
		throw NULL_POINTER;
	}
}
/** ���������� ������ ������*/
size_t VCurrentSpeedData::GetCount() const
{
	return data_set->GetSpeed().GetCount();
}

/** ���������� ���������� �� ��� X*/
double VCurrentSpeedData::GetX(size_t in_index) const
{
	return data_set->GetAbsCoord().GetItem(in_index);
}

/** ���������� ���������� �� ��� Y*/
double VCurrentSpeedData::GetY(size_t in_index) const
{
	return data_set->GetSpeed().GetItem(in_index);
}

/** ���������� ����������� �������� �� Y*/
double VCurrentSpeedData::GetMinY()
{	
	return data_set->GetSpeed().GetMin();
}

/** ���������� ������������ �������� �� Y*/
double VCurrentSpeedData::GetMaxY()
{	
	return data_set->GetSpeed().GetMax();
}

/** ���������� ����������� �������� �� X*/
double VCurrentSpeedData::GetMinX()
{
	return data_set->GetAbsCoord().GetItem(0);
}

/** ���������� ������������ �������� �� X*/
double VCurrentSpeedData::GetMaxX()
{
	return data_set->GetAbsCoord().GetLastItem();
}

/** ���������� �������� Y �� X*/
double VCurrentSpeedData::GetYbyX(double in_x)
{
	size_t index=data_set->GetAbsCoord().FindIndex(in_x, true);	
	return (index!=BAD_INDEX) ? GetY(index) : 0;
}

//---------------------------------- ������ ������ VZazorLeftData ----------------------------------

/** �����������*/
VZazorLeftData::VZazorLeftData(VDataSetPtr in_data_set) : VXOrderedDataProvider()
{
	if (in_data_set)
	{	
		data_set=in_data_set;
		data_set->Load(PRM_ZAZOR_LEFT);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VZazorLeftData::GetCount() const
{	
	return data_set->GetZazorLeft().GetCount();
}

/** ���������� ���������� �� ��� X*/
double VZazorLeftData::GetX(size_t in_index) const
{	
	return data_set->GetAbsCoord().GetItem(data_set->GetZazorLeft().GetItem(in_index).index);
}

/** ���������� ���������� �� ��� Y*/
double VZazorLeftData::GetY(size_t in_index) const
{	
	return data_set->GetZazorLeft().GetItem(in_index).value;
}

/** ���������� ����� ��� �����, �������� �����*/
uint64_t VZazorLeftData::GetFlags(size_t in_index) const
{
	if (data_set->GetZazorLeft().IsCorrupt(in_index))
		return 1;
	return 0;
}

/** ���������� ����������� �������� �� Y*/
double VZazorLeftData::GetMinY() 
{	
	//���������� ��� ������, ��� ����� �� ����� ���� �������������		
	return 0;
};

/** ���������� ������������ �������� �� Y*/
double VZazorLeftData::GetMaxY() 
{	
	//���������� ��� ������, ��� ����� �� ����� ���� �������������
	double result=0;
	for (size_t i=0;i<GetCount();++i)
	{
		result=std::max<double>(result, GetY(i));
	}
	return result;
};

/** ���������� ����������� �������� �� X*/
double VZazorLeftData::GetMinX() 
{	
	return GetCount()>0 ? GetX(0) : 0;
};

/** ���������� ������������ �������� �� X*/
double VZazorLeftData::GetMaxX() 
{	
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
};	
	
/** ���������� �����*/
std::wstring VZazorLeftData::GetLabel(size_t in_index) const
{	
	return boost::str(boost::wformat(L"%0.1f") % GetY(in_index));
}


//-------------------------������ ������ VMagZazorLeftData-------------------------------------------
VMagZazorLeftData::VMagZazorLeftData(VDataSetPtr in_data_set) : VXOrderedDataProvider()
{
	if (in_data_set)
	{	
		data_set=in_data_set;
		data_set->Load(PRM_MAG_ZAZOR_LEFT);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VMagZazorLeftData::GetCount() const
{	
	return data_set->GetMagZazorLeft().GetCount();
}

/** ���������� ���������� �� ��� X*/
double VMagZazorLeftData::GetX(size_t in_index) const
{	
	return data_set->GetAbsCoord().GetItem(data_set->GetMagZazorLeft().GetItem(in_index).index);
}

/** ���������� ���������� �� ��� Y*/
double VMagZazorLeftData::GetY(size_t in_index) const
{	
	return data_set->GetMagZazorLeft().GetItem(in_index).value;
}

/** ���������� ����� ��� �����, �������� �����*/
uint64_t VMagZazorLeftData::GetFlags(size_t in_index) const
{
	if (data_set->GetMagZazorLeft().IsCorrupt(in_index))
		return 1;
	return 0;
}

/** ���������� ����������� �������� �� Y*/
double VMagZazorLeftData::GetMinY() 
{	
	//���������� ��� ������, ��� ����� �� ����� ���� �������������		
	return 0;
};

/** ���������� ������������ �������� �� Y*/
double VMagZazorLeftData::GetMaxY() 
{	
	//���������� ��� ������, ��� ����� �� ����� ���� �������������
	double result=0;
	for (size_t i=0;i<GetCount();++i)
	{
		result=std::max<double>(result, GetY(i));
	}
	return result;
};

/** ���������� ����������� �������� �� X*/
double VMagZazorLeftData::GetMinX() 
{	
	return GetCount()>0 ? GetX(0) : 0;
};

/** ���������� ������������ �������� �� X*/
double VMagZazorLeftData::GetMaxX() 
{	
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
};	
	
/** ���������� �����*/
std::wstring VMagZazorLeftData::GetLabel(size_t in_index) const
{	
	return boost::str(boost::wformat(L"%0.1f") % GetY(in_index));
}

//---------------------------------- ������ ������ VMagZazorRightData -------------------------------------------
VMagZazorRightData::VMagZazorRightData(VDataSetPtr in_data_set) : VXOrderedDataProvider()
{
	if (in_data_set)
	{	
		data_set=in_data_set;
		data_set->Load(PRM_MAG_ZAZOR_RIGHT);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VMagZazorRightData::GetCount() const
{	
	return data_set->GetMagZazorRight().GetCount();
}

/** ���������� ���������� �� ��� X*/
double VMagZazorRightData::GetX(size_t in_index) const
{	
	return data_set->GetAbsCoord().GetItem(data_set->GetMagZazorRight().GetItem(in_index).index);
}

/** ���������� ���������� �� ��� Y*/
double VMagZazorRightData::GetY(size_t in_index) const
{	
	return data_set->GetMagZazorRight().GetItem(in_index).value;
}

/** ���������� ����� ��� �����, �������� �����*/
uint64_t VMagZazorRightData::GetFlags(size_t in_index) const
{
	if (data_set->GetMagZazorRight().IsCorrupt(in_index))
		return 1;
	return 0;
}

/** ���������� ����������� �������� �� Y*/
double VMagZazorRightData::GetMinY() 
{	
	//���������� ��� ������, ��� ����� �� ����� ���� �������������	}		
	return 0;
};

/** ���������� ������������ �������� �� Y*/
double VMagZazorRightData::GetMaxY() 
{	
	//���������� ��� ������, ��� ����� �� ����� ���� �������������
	double result=0;
	for (size_t i=0;i<GetCount();++i)
	{
		result=std::max<double>(result, GetY(i));
	}
	return result;
};

/** ���������� ����������� �������� �� X*/
double VMagZazorRightData::GetMinX() 
{	
	return GetCount()>0 ? GetX(0) : 0;
};

/** ���������� ������������ �������� �� X*/
double VMagZazorRightData::GetMaxX() 
{	
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
};	
	
/** ���������� �����*/
std::wstring VMagZazorRightData::GetLabel(size_t in_index) const
{	
	return boost::str(boost::wformat(L"%0.1f") % GetY(in_index));
}

//---------------------------------- ������ ������ VZazorRightData ----------------------------------

/** �����������*/
VZazorRightData::VZazorRightData(VDataSetPtr in_data_set) : VXOrderedDataProvider()
{
	if (in_data_set)
	{	
		data_set=in_data_set;
		data_set->Load(PRM_ZAZOR_RIGHT);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VZazorRightData::GetCount() const
{	
	return data_set->GetZazorRight().GetCount();
}

/** ���������� ���������� �� ��� X*/
double VZazorRightData::GetX(size_t in_index) const
{	
	return data_set->GetAbsCoord().GetItem(data_set->GetZazorRight().GetItem(in_index).index);
}

/** ���������� ���������� �� ��� Y*/
double VZazorRightData::GetY(size_t in_index) const
{	
	return data_set->GetZazorRight().GetItem(in_index).value;
}

/** ���������� ����� ��� �����, �������� �����*/
uint64_t VZazorRightData::GetFlags(size_t in_index) const
{
	if (data_set->GetZazorRight().IsCorrupt(in_index))
		return 1;
	return 0;
}
	
/** ���������� ����������� �������� �� Y*/
double VZazorRightData::GetMinY() 
{	
	//���������� ��� ������, ��� ����� �� ����� ���� �������������	
	return 0;
};

/** ���������� ������������ �������� �� Y*/
double VZazorRightData::GetMaxY() 
{		
	double result=0;
	for (size_t i=0;i<GetCount();++i)
	{
		result=std::max<double>(result, GetY(i));
	}
	return result;
};

/** ���������� ����������� �������� �� X*/
double VZazorRightData::GetMinX() 
{	
	return GetCount()>0 ? GetX(0) : 0;
};

/** ���������� ������������ �������� �� X*/
double VZazorRightData::GetMaxX() 
{	
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
};	
	
/** ���������� �����*/
std::wstring VZazorRightData::GetLabel(size_t in_index) const
{	
	return boost::str(boost::wformat(L"%0.1f") % GetY(in_index));
}


//---------------------------------- ������ ������ VUgonLeftData ----------------------------------

/** �����������*/
VUgonLeftData::VUgonLeftData(VDataSetPtr in_data_set) : VXOrderedDataProvider()
{
	if (in_data_set)
	{	
		data_set=in_data_set;
		data_set->Load(PRM_UGON_LEFT);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VUgonLeftData::GetCount() const
{	
	return data_set->GetUgonLeft().GetCount();
}

/** ���������� ���������� �� ��� X*/
double VUgonLeftData::GetX(size_t in_index) const
{	
	return data_set->GetAbsCoord().GetItem(data_set->GetUgonLeft().GetItem(in_index).index);
}

/** ���������� ���������� �� ��� Y*/
double VUgonLeftData::GetY(size_t in_index) const
{	
	return data_set->GetUgonLeft().GetItem(in_index).value;
}

/** ���������� ����� ��� �����, �������� �����*/
uint64_t VUgonLeftData::GetFlags(size_t in_index) const
{
	if (data_set->GetUgonLeft().IsCorrupt(in_index))
		return 1;
	return 0;
}
	
/** ���������� ����������� �������� �� Y*/
double VUgonLeftData::GetMinY() 
{	
	double result=0;
	for (size_t i=0;i<GetCount();++i)
	{
		result=std::min<double>(result, GetY(i));
	}
	return result;
};

/** ���������� ������������ �������� �� Y*/
double VUgonLeftData::GetMaxY() 
{		
	double result=0;
	for (size_t i=0;i<GetCount();++i)
	{
		result=std::max<double>(result, GetY(i));
	}
	return result;
};

/** ���������� ����������� �������� �� X*/
double VUgonLeftData::GetMinX() 
{	
	return GetCount()>0 ? GetX(0) : 0;
};

/** ���������� ������������ �������� �� X*/
double VUgonLeftData::GetMaxX() 
{	
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
};	
	
/** ���������� �����*/
std::wstring VUgonLeftData::GetLabel(size_t in_index) const
{	
	return boost::str(boost::wformat(L"%0.1f") % GetY(in_index));
}


//---------------------------------- ������ ������ VUgonRightData ----------------------------------

/** �����������*/
VUgonRightData::VUgonRightData(VDataSetPtr in_data_set) : VXOrderedDataProvider()
{
	if (in_data_set)
	{	
		data_set=in_data_set;
		data_set->Load(PRM_UGON_RIGHT);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VUgonRightData::GetCount() const
{	
	return data_set->GetUgonLeft().GetCount();
}

/** ���������� ���������� �� ��� X*/
double VUgonRightData::GetX(size_t in_index) const
{	
	return data_set->GetAbsCoord().GetItem(data_set->GetUgonLeft().GetItem(in_index).index);
}

/** ���������� ���������� �� ��� Y*/
double VUgonRightData::GetY(size_t in_index) const
{	
	return data_set->GetUgonLeft().GetItem(in_index).value;
}

/** ���������� ����� ��� �����, �������� �����*/
uint64_t VUgonRightData::GetFlags(size_t in_index) const
{
	if (data_set->GetUgonLeft().IsCorrupt(in_index))
		return 1;
	return 0;
}
	
/** ���������� ����������� �������� �� Y*/
double VUgonRightData::GetMinY() 
{	
	double result=0;
	for (size_t i=0;i<GetCount();++i)
	{
		result=std::min<double>(result, GetY(i));
	}
	return result;
};

/** ���������� ������������ �������� �� Y*/
double VUgonRightData::GetMaxY() 
{		
	double result=0;
	for (size_t i=0;i<GetCount();++i)
	{
		result=std::max<double>(result, GetY(i));
	}
	return result;
};

/** ���������� ����������� �������� �� X*/
double VUgonRightData::GetMinX() 
{	
	return GetCount()>0 ? GetX(0) : 0;
};

/** ���������� ������������ �������� �� X*/
double VUgonRightData::GetMaxX() 
{	
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
};	
	
/** ���������� �����*/
std::wstring VUgonRightData::GetLabel(size_t in_index) const
{	
	return boost::str(boost::wformat(L"%0.1f") % GetY(in_index));
}

//------------------------------ ������ ������ VSpalaMarkerData---------------------------------------------

/** �����������*/	
VSpalaMarkerData::VSpalaMarkerData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type) : VXOrderedDataProvider()
{
	if (in_data_set)
	{	
		data_set=in_data_set;
		data_type=in_data_type;
		data_set->Load(PRM_ABS_COORD);
		data_set->Load(PRM_SPALA_MARKER);
		if (data_type!=PRM_NONE)
			data_set->Load(data_type);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VSpalaMarkerData::GetCount() const
{
	return data_set->GetSpalaMarker().GetCount();
}

/** ���������� ���������� �� ��� X*/
double VSpalaMarkerData::GetX(size_t in_index) const
{
	return data_set->GetAbsCoord().GetItem(data_set->GetSpalaMarker().GetItem(in_index).index);
}

/** ���������� ���������� �� ��� Y*/
double VSpalaMarkerData::GetY(size_t in_index) const
{
	if (data_type==PRM_NONE) 
		return 0;
	else
	{
		uint32_t index = data_set->GetSpalaMarker().GetItem(in_index).index;
		if (index<data_set->GetParameter(data_type).GetCount()) 
			return data_set->GetParameter(data_type).GetItem(index);
		else return 0;
	}
}

/** ���������� ����������� �������� �� Y*/
double VSpalaMarkerData::GetMinY()
{
	return DBL_MAX;
}

/** ���������� ������������ �������� �� Y*/
double VSpalaMarkerData::GetMaxY()
{
	return 0-DBL_MAX;
}

/** ���������� ����������� �������� �� X*/
double VSpalaMarkerData::GetMinX()
{
	return GetCount()>0 ? GetX(0) : 0;
}

/** ���������� ������������ �������� �� X*/
double VSpalaMarkerData::GetMaxX()
{
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
}

/** ���������� �����*/
std::wstring VSpalaMarkerData::GetLabel(size_t in_index) const
{	
	return L"�";
}

//---------------------------------- ������ ������ VOperatorMarkData -------------------------------------------

/** �����������*/
VOperatorMarkData::VOperatorMarkData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type) : VXOrderedDataProvider()
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_type=in_data_type;
		data_set->Load(PRM_ABS_COORD);
		data_set->Load(PRM_OP_MARKS);
		if (data_type!=PRM_NONE)
			data_set->Load(data_type);		
	}
	else
	{
		throw NULL_POINTER;
	}
};

/** ���������� ����� �����*/
size_t VOperatorMarkData::GetCount() const
{
	return data_set->GetOpMarks().GetCount();
};

/** ���������� ���������� �� ��� X*/
double VOperatorMarkData::GetX(size_t in_index) const
{	
	if (GetCount()>0)
	{
		uint32_t index = data_set->GetOpMarks().GetItem(in_index).index;
		return data_set->GetAbsCoord().GetItem(index);
	}
	else
		return 0;
};	

/** ���������� ���������� �� ��� Y*/
double VOperatorMarkData::GetY(size_t in_index) const
{		
	if (data_type==PRM_NONE) 
		return 0;
	else
	{
		uint32_t index = data_set->GetOpMarks().GetItem(in_index).index;
		if (index<data_set->GetParameter(data_type).GetCount())
			return data_set->GetParameter(data_type).GetItem(index) ;
		else 
			return 0;
	}
};

/** ���������� ����������� �������� �� Y*/
double VOperatorMarkData::GetMinY()
{
	return DBL_MAX;
};

/** ���������� ������������ �������� �� Y*/
double VOperatorMarkData::GetMaxY()
{
	return 0-DBL_MAX;
};

/** ���������� ����������� �������� �� X*/
double VOperatorMarkData::GetMinX()
{
	if (GetCount()>0)
	{
		uint32_t index = data_set->GetOpMarks().GetItem(0).index;
		return data_set->GetAbsCoord().GetItem(index);		
	}
	else
		return 0;
};

/** ���������� ������������ �������� �� X*/
double VOperatorMarkData::GetMaxX()
{
	if (GetCount()>0)
	{
		uint32_t index = data_set->GetOpMarks().GetItem(GetCount()-1).index;
		return data_set->GetAbsCoord().GetItem(index);
	}
	else
		return 0;
};

/** ���������� �����*/
std::wstring VOperatorMarkData::GetLabel(size_t in_index) const
{
	return data_set->GetOpMarks().GetItem(in_index).ShortText();
};

/** �������� �����*/
void VOperatorMarkData::SelectPoint(size_t in_index)
{
	selected_point=in_index;
}

/** ���������� ���������� �����*/
size_t VOperatorMarkData::GetSelectedPoint() const
{
	return selected_point;
}

/** ������� ���������*/
void VOperatorMarkData::DeselectPoint()
{	
	selected_point=BAD_INDEX;
}

//--------------------------------------------------- ������ ������ VWayMarkerData ---------------------------------------------
/** �����������*/	
VWayMarkerData::VWayMarkerData(VDataSetPtr in_data_set)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_set->Load(PRM_WAY_MARKERS);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VWayMarkerData::GetCount() const
{
	return data_set->GetMarkers().GetCount();
}

/** ���������� ���������� �� ��� X*/
double VWayMarkerData::GetX(size_t in_index) const
{
	size_t i = data_set->GetMarkers().GetItem(in_index).GetIndex();
	return i<data_set->GetAbsCoord().GetCount() ? data_set->GetAbsCoord().GetItem(i) : 0;
	//return data_set->GetMarkers().GetItem(in_index).GetCoord();
}

/** ���������� ���������� �� ��� Y*/
double VWayMarkerData::GetY(size_t in_index) const
{
	return 0;
}

/** ���������� ����������� �������� �� Y*/
double VWayMarkerData::GetMinY()
{
	return 0;
}

/** ���������� ������������ �������� �� Y*/
double VWayMarkerData::GetMaxY()
{
	return 0;
}

/** ���������� �����*/
std::wstring VWayMarkerData::GetLabel(size_t in_index) const
{
	return string_to_wstring(data_set->GetMarkers().GetItem(in_index).GetTypeString());
}

//---------------------------------- ������ ������ VProfileMarkData -------------------------------------

/** �����������*/
VProfileMarkData::VProfileMarkData(VDataSetPtr in_data_set) : VXOrderedDataProvider()
{
	if (in_data_set)
	{
		std::vector<VAprinfoProfLabel> marks;			
		marks=in_data_set->GetAprinfo().GetProfLabels();
		if (!marks.empty())
		{
			for (size_t i=1; i<marks.size(); ++i)
			{			
				if (marks.at(i).coord>=in_data_set->GetAbsCoord().GetItem(0) && 
					marks.at(i).coord<=in_data_set->GetAbsCoord().GetItem(in_data_set->GetAbsCoord().GetCount()-1))
				{
					xdata.push_back(marks.at(i).coord);
					ydata.push_back(marks.at(i).value);
				}
			}
		}
	}
	else
	{
		throw NULL_POINTER;
	}	
}

/** ���������� ������ ������*/
size_t VProfileMarkData::GetCount() const
{
	return xdata.size();
}

/** ���������� ���������� �� ��� X*/
double VProfileMarkData::GetX(size_t in_index) const
{
	return xdata.at(in_index);
}

/** ���������� ���������� �� ��� Y*/
double VProfileMarkData::GetY(size_t in_index) const
{
	return ydata.at(in_index);
}

/** ���������� �����*/
std::wstring VProfileMarkData::GetLabel(size_t in_index) const
{
	return boost::str(boost::wformat(L"%1% ��") % GetY(in_index));
}

/** ���������� ����������� �������� �� Y*/
double VProfileMarkData::GetMinY()
{
	if (!ydata.empty())
		return *(std::min_element(ydata.begin(), ydata.end()));
	else
		return 0;
}

/** ���������� ������������ �������� �� Y*/
double VProfileMarkData::GetMaxY()
{
	if (!ydata.empty())
		return *(std::max_element(ydata.begin(), ydata.end()));
	else
		return 0;
}

/** ���������� ����������� �������� �� X*/
double VProfileMarkData::GetMinX() 
{
	return xdata.at(0);
};

/** ���������� ������������ �������� �� X*/
double VProfileMarkData::GetMaxX()
{
	return xdata.at(xdata.size()-1);
};

//--------------------------------------- ������ ������ VProfileElementsData ---------------------------------

/** �����������*/
VProfileElementsData::VProfileElementsData(VDataSetPtr in_data_set) : VXOrderedDataProvider()
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_set->Load(PRM_PROF_MARKS);		
		drag=false;		
	}
	else
	{
		throw NULL_POINTER;
	}	
}

/** ���������� ������ ������*/
size_t VProfileElementsData::GetCount() const
{
	return data_set->GetProfMarks().GetCount();
}

/** ���������� ���������� �� ��� X*/
double VProfileElementsData::GetX(size_t in_index) const
{	
	return data_set->GetAbsCoord().GetItem(data_set->GetProfMarks().GetItem(in_index).index);
}

/** ���������� ���������� �� ��� Y*/
double VProfileElementsData::GetY(size_t in_index) const
{		
	return data_set->GetProfMarks().GetItem(in_index).value;
}

/** ������������� �������� X*/
void VProfileElementsData::SetXY(size_t in_index, double in_x, double in_y)
{
	if (GetActive())
	{
		size_t point_index=data_set->GetAbsCoord().FindIndex(in_x, true);
		if (data_set->AllowMarkHereProfile(point_index, in_index, 0.0))
		{
			data_set->GetProfMarks().GetItem(in_index).index=point_index;
			data_set->GetProfMarks().GetItem(in_index).absCoord=in_x;						
			data_set->GetProfMarks().GetItem(in_index).value=
				OPTIONS.GetOption(L"DROP_MARKS_ON_PROFILE").Compare(OPTION_YES) ?
				data_set->GetParameter(PRM_PROFILE).GetItem(point_index) : in_y;
			data_set->GetProfMarks().Sort();
			data_set->SetChanged(PRM_PROF_MARKS);
			data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Update(data_set->GetParameter(PRM_PROFILE), 
				data_set->GetProfMarks(), data_set->GetAbsCoord(), data_set->GetAprinfo(), PRM_PROFILE_SLOPE_ELEMENT);
			data_set->GetAproxProfile().Update(data_set->GetParameter(PRM_PROFILE), data_set->GetAbsCoord(), 
				data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT), in_index);		
			data_set->GetDiffProfile().Update(data_set->GetParameter(PRM_PROFILE), data_set->GetAproxProfile(), 
				data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT), in_index);					
		}
	}
}

/** ��������� �����*/
void VProfileElementsData::AddXY(size_t in_index, double in_x, double in_y, double in_size_x, double in_size_y, 
		const std::wstring& in_label, uint64_t in_flags)
{
	if (GetActive())
	{
		VProfileMark new_mark;
		new_mark.index=data_set->GetAbsCoord().FindIndex(in_x, true);
		new_mark.absCoord=in_x;
		new_mark.isCurve=false;
		if (data_set->AllowMarkHereProfile(new_mark.index, 0.0))
		{
			//��������� �����
			new_mark.value=OPTIONS.GetOption(L"DROP_MARKS_ON_PROFILE").Compare(OPTION_YES) ?
				data_set->GetParameter(PRM_PROFILE).GetItem(new_mark.index) : in_y;							
			data_set->GetProfMarks().AddItem(new_mark);
			data_set->GetProfMarks().Sort();
			data_set->SetChanged(PRM_PROF_MARKS);

			//���������� ����� ����� �����
			size_t i=data_set->GetProfMarks().FindMark(new_mark);

			if (i!=BAD_INDEX)
			{

				data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Update(data_set->GetParameter(PRM_PROFILE), 
					data_set->GetProfMarks(), data_set->GetAbsCoord(), data_set->GetAprinfo(), PRM_PROFILE_SLOPE_ELEMENT);

				if (data_set->GetAproxProfile().IsLoaded())
				{
					data_set->GetAproxProfile().Update(data_set->GetParameter(PRM_PROFILE), data_set->GetAbsCoord(), 
						data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT), i);
				}
				else
				{
					data_set->GetAproxProfile().Load(data_set->GetParameter(PRM_PROFILE), data_set->GetAbsCoord(), data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT));
				}
				if (data_set->GetDiffProfile().IsLoaded())
				{
					data_set->GetDiffProfile().Update(data_set->GetParameter(PRM_PROFILE), data_set->GetAproxProfile(), 
						data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT), i);
				}
				else
				{
					data_set->GetDiffProfile().Load(data_set->GetParameter(PRM_PROFILE), data_set->GetAproxProfile());
				}								
			}
		}
	}
}

/** ������� �����*/
void VProfileElementsData::DeleteItem(size_t in_index)
{
	if (GetActive())
	{		
		data_set->GetProfMarks().DeleteItem(in_index);
		data_set->SetChanged(PRM_PROF_MARKS);		
		data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Update(data_set->GetParameter(PRM_PROFILE), 
			data_set->GetProfMarks(), data_set->GetAbsCoord(), data_set->GetAprinfo(), PRM_PROFILE_SLOPE_ELEMENT);
		data_set->GetAproxProfile().Update(data_set->GetParameter(PRM_PROFILE), data_set->GetAbsCoord(), 
			data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT), in_index);
		data_set->GetDiffProfile().Update(data_set->GetParameter(PRM_PROFILE), data_set->GetAproxProfile(), 
			data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT), in_index);		
	}
}

/** ������� ��� �����*/
void VProfileElementsData::DeleteAll()
{	
	if (GetActive())
	{
		data_set->GetProfMarks().Unload();
		data_set->SetChanged(PRM_PROF_MARKS);

		data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Update(data_set->GetParameter(PRM_PROFILE), 
			data_set->GetProfMarks(), data_set->GetAbsCoord(), data_set->GetAprinfo(), PRM_PROFILE_SLOPE_ELEMENT);
		data_set->GetAproxProfile().Unload();
		data_set->GetAproxProfile().Load(data_set->GetParameter(PRM_PROFILE), data_set->GetAbsCoord(), data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT)/*data_set->GetProfMarks()*/);
		data_set->GetDiffProfile().Unload();
		data_set->GetDiffProfile().Load(data_set->GetParameter(PRM_PROFILE), data_set->GetAproxProfile());				
	}
};

/** ���������� �����*/
std::wstring VProfileElementsData::GetLabel(size_t in_index) const
{
	return boost::str(boost::wformat(L"%0.0f") % GetY(in_index));
}

/** ���������� ����������� �������� �� Y*/
double VProfileElementsData::GetMinY()
{		
	return data_set->GetParameter(PRM_PROFILE).GetMin();
}

/** ���������� ������������ �������� �� Y*/
double VProfileElementsData::GetMaxY()
{		
	return data_set->GetParameter(PRM_PROFILE).GetMax();
}

/** ���������� ����������� �������� �� X*/
double VProfileElementsData::GetMinX() 
{	
	return data_set->GetAbsCoord().GetItem(0);
};

/** ���������� ������������ �������� �� X*/
double VProfileElementsData::GetMaxX()
{	
	return data_set->GetAbsCoord().GetLastItem();
};

/** ������������� ����*/
void VProfileElementsData::SetFlags(size_t in_index, uint64_t in_flags)
{
	data_set->GetProfMarks().GetItem(in_index).isCurve = in_flags!=0;

	data_set->SetChanged(PRM_PROF_MARKS);

	data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Update(data_set->GetParameter(PRM_PROFILE), 
		data_set->GetProfMarks(), data_set->GetAbsCoord(), data_set->GetAprinfo(), PRM_PROFILE_SLOPE_ELEMENT);
	data_set->GetAproxProfile().Update(data_set->GetParameter(PRM_PROFILE), data_set->GetAbsCoord(), 
		data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT), in_index);		
	data_set->GetDiffProfile().Update(data_set->GetParameter(PRM_PROFILE), data_set->GetAproxProfile(), 
		data_set->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT), in_index);			
}

/** ���������� ���� (������� ���� ��������)*/
uint64_t VProfileElementsData::GetFlags(size_t in_index) const
{
	return static_cast<uint64_t>(data_set->GetProfMarks().GetItem(in_index).isCurve);
}

//----------------------------------- ������ ������ VSlopeData ----------------------------------------------

/** �����������*/
VSlopeData::VSlopeData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type) : VXOrderedDataProvider()
{
	if (in_data_set)
	{
		data_set=in_data_set;	
		data_type=in_data_type;
		data_set->Load(data_type);		
	}
	else
		throw NULL_POINTER;
}
/** ���������� ������ ������*/
size_t VSlopeData::GetCount() const
{
	return data_set->GetProfileSlope(data_type).GetCount();
}

/** ���������� ���������� �� ��� X*/
double VSlopeData::GetX(size_t in_index) const
{
	return data_set->GetProfileSlope(data_type).GetItem(in_index).GetStart().x;
}

/** ���������� ���������� �� ��� Y*/
double VSlopeData::GetY(size_t in_index) const		
{
	return data_set->GetProfileSlope(data_type).GetItem(in_index).GetStart().y;
}

/** ���������� ����������� �������� �� Y*/
double VSlopeData::GetMinY()
{
	return DBL_MAX;
}

/** ���������� ������������ �������� �� Y*/
double VSlopeData::GetMaxY()
{
	return 0-DBL_MAX;
}

/** ���������� ����������� �������� �� X*/
double VSlopeData::GetMinX()
{
	return GetCount()>0 ? GetX(0) : 0;
}

/** ���������� ������������ �������� �� X*/
double VSlopeData::GetMaxX()
{
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
}

/** ���������� �����*/
std::wstring VSlopeData::GetLabel(size_t in_index) const
{
	if (data_set->GetProfileSlope(data_type).GetItem(in_index).IsCurve())
	{
		return boost::str(boost::wformat(L"%0.1f ��") % (data_set->GetProfileSlope(data_type).GetItem(in_index).R() / 1000.0));
	}
	else
	{
		double slope=data_set->GetProfileSlope(data_type).GetItem(in_index).Slope();
		if (slope < 0.1 && slope > -0.1)
			return L"0.0";
		else
			return boost::str(boost::wformat(L"%0.1f") % slope);
	}
}

/** ���������� ����*/
uint64_t VSlopeData::GetFlags(size_t in_index) const
{	
	if (data_set->GetProfileSlope(data_type).GetItem(in_index).IsCurve())
	{
		return data_set->GetProfileSlope(data_type).GetItem(in_index).Sign()>0 ? SLOPE_HILL : SLOPE_PIT;
	}
	else
	{
		double slope=data_set->GetProfileSlope(data_type).GetItem(in_index).Slope();
		if (slope < 0.1 && slope > -0.1)	
			return SLOPE_ZERO;	
		else if (slope > 0)	
			return SLOPE_POSITIVE;	
		else
			return SLOPE_NEGATIVE;
	}
}

/** ���������� ������ �� ��� X*/
double VSlopeData::GetSizeX(size_t in_index) const
{
	return data_set->GetProfileSlope(data_type).GetItem(in_index).GetEnd().x-
		   data_set->GetProfileSlope(data_type).GetItem(in_index).GetStart().x;
}
/** ���������� ������ �� ��� Y*/
double VSlopeData::GetSizeY(size_t in_index) const
{
	return data_set->GetProfileSlope(data_type).GetItem(in_index).GetEnd().y-
		   data_set->GetProfileSlope(data_type).GetItem(in_index).GetStart().y;
}


/** ����������� �������� ������*/
void VSlopeData::SetType(PARAMETER_TYPE in_data_type)
{
	data_type=in_data_type;	
	data_set->Load(data_type);			
}

/** ���������� ��� ����������� ������*/
PARAMETER_TYPE VSlopeData::GetType()
{
	return data_type;
}

/** ���������� ��������� ������*/
const void * VSlopeData::GetUserData(size_t in_index) const
{
	return &data_set->GetProfileSlope(data_type).GetItem(in_index);
}

//------------------------------------- ������ ������ VSlopeAngleData ---------------------------------------

/** �����������*/
VSlopeAngleData::VSlopeAngleData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;	
		data_type=in_data_type;
		data_set->Load(data_type);		
	}
	else
		throw NULL_POINTER;
}
/** ���������� ������ ������*/
size_t VSlopeAngleData::GetCount() const
{	
	return data_set->GetProfileSlope(data_type).GetCount()>0 ? data_set->GetProfileSlope(data_type).GetCount()-1 : 0;
}

/** ���������� ���������� �� ��� X*/
double VSlopeAngleData::GetX(size_t in_index) const
{
	return data_set->GetProfileSlope(data_type).GetItem(in_index).GetEnd().x;
}

/** ���������� ���������� �� ��� Y*/
double VSlopeAngleData::GetY(size_t in_index) const
{
	return 0;
}


/** ���������� ����������� �������� �� Y*/
double VSlopeAngleData::GetMinY()
{
	return 0;
}

/** ���������� ������������ �������� �� Y*/
double VSlopeAngleData::GetMaxY()
{
	return 0;
}

/** ���������� ����������� �������� �� X*/
double VSlopeAngleData::GetMinX()
{
	return GetCount()>0 ? GetX(0) : 0;
}

/** ���������� ������������ �������� �� X*/
double VSlopeAngleData::GetMaxX()
{
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
}

/** ���������� �����*/
std::wstring VSlopeAngleData::GetLabel(size_t in_index) const
{
	if (in_index+1>=GetCount() ||
		data_set->GetProfileSlope(data_type).GetItem(in_index+1).IsCurve() ||
		data_set->GetProfileSlope(data_type).GetItem(in_index).IsCurve())
	{
		return L"";
	}
	else
	{		
		return boost::str(boost::wformat(L"%0.1f") % 
			(data_set->GetProfileSlope(data_type).GetItem(in_index+1).Slope()-
			data_set->GetProfileSlope(data_type).GetItem(in_index).Slope()));
	}	
}

/** ����������� �������� ������*/
void VSlopeAngleData::SetType(PARAMETER_TYPE in_data_type)
{
	data_type=in_data_type;
	data_set->Load(data_type);
}

/** ���������� ��� ����������� ������*/
PARAMETER_TYPE VSlopeAngleData::GetType()
{
	return data_type;
}

//----------------------------------- ������ ������ VHeightData ----------------------------------------------

/** �����������*/
VHeightData::VHeightData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type) : VXOrderedDataProvider()
{
	if (in_data_set)
	{
		data_set=in_data_set;	
		data_type=in_data_type;
		data_set->Load(data_type);		
	}
	else
		throw NULL_POINTER;
}
/** ���������� ������ ������*/
size_t VHeightData::GetCount() const
{
	
	return (data_set->GetProfileSlope(data_type).GetCount()>0) ? 
		data_set->GetProfileSlope(data_type).GetCount()+1 : 0;
}

/** ���������� ���������� �� ��� X*/
double VHeightData::GetX(size_t in_index) const
{
	if (in_index<data_set->GetProfileSlope(data_type).GetCount())
		return data_set->GetProfileSlope(data_type).GetItem(in_index).GetStart().x;
	else
		return data_set->GetProfileSlope(data_type).GetLastItem().GetEnd().x;
}

/** ���������� ���������� �� ��� Y*/
double VHeightData::GetY(size_t in_index) const		
{
	if (in_index<data_set->GetProfileSlope(data_type).GetCount())
		return data_set->GetProfileSlope(data_type).GetItem(in_index).GetStart().y;
	else
		return data_set->GetProfileSlope(data_type).GetLastItem().GetEnd().y;
}

/** ���������� ����������� �������� �� Y*/
double VHeightData::GetMinY()
{
	return DBL_MAX;
}

/** ���������� ������������ �������� �� Y*/
double VHeightData::GetMaxY()
{
	return 0-DBL_MAX;
}

/** ���������� ����������� �������� �� X*/
double VHeightData::GetMinX()
{
	return GetCount()>0 ? GetX(0) : 0;
}

/** ���������� ������������ �������� �� X*/
double VHeightData::GetMaxX()
{
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
}

/** ���������� �����*/
std::wstring VHeightData::GetLabel(size_t in_index) const
{
	// �������� ����������� � �����
	return boost::str(boost::wformat(L"%0.2f") % (GetY(in_index)/100.0));
}

/** ���������� ����*/
uint64_t VHeightData::GetFlags(size_t in_index) const
{
	double slope=data_set->GetProfileSlope(data_type).GetItem(in_index).Slope();
	if (slope < 0.1 && slope > -0.1)	
		return 0;	
	else if (slope > 0)	
		return 1;	
	else
		return 2;
}


/** ����������� �������� ������*/
void VHeightData::SetType(PARAMETER_TYPE in_data_type)
{
	data_type=in_data_type;	
	data_set->Load(data_type);			
}

/** ���������� ��� ����������� ������*/
PARAMETER_TYPE VHeightData::GetType()
{
	return data_type;
}

//------------------------------------ ������ ������ VAnpModelData -------------------------

/** �����������*/	
VAnpModelData::VAnpModelData(VDataSetPtr in_data_set, speed_type in_speed, bool in_inverse) : VXOrderedDataProvider()
{
	if (in_data_set)
	{
		data_set=in_data_set;				
		speed=in_speed;
		switch (speed)
		{
			default:
			case pass_prk: 
				data_set->Load(PRM_ANP_MODEL_PSP_PASS);		
				break;
			case crg_prk: 
				data_set->Load(PRM_ANP_MODEL_PSP_TRUCK);		
				break;
			case high_prk: 
				data_set->Load(PRM_ANP_MODEL_PSP_HIGH);		
				break;
			case high_bt_prk: 
				data_set->Load(PRM_ANP_MODEL_PSP_HIGH_BT);		
				break;
		}
		sign_y=in_inverse ? -1.0 : 1.0;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VAnpModelData::GetCount() const
{
	switch (speed)
	{
		default:
		case pass_prk: 
			return data_set->GetAnpModelPass().GetCount();
			break;
		case crg_prk:
			return data_set->GetAnpModelTruck().GetCount();
			break;
		case high_prk: 
			return data_set->GetAnpModelHigh().GetCount();
			break;
		case high_bt_prk: 
			return data_set->GetAnpModelHighBT().GetCount();
			break;
	}
}

/** ���������� ���������� �� ��� X*/
double VAnpModelData::GetX(size_t in_index) const
{	
	switch (speed)
	{
		default:
		case pass_prk: 
			return data_set->GetAbsCoord().GetItem(data_set->GetAnpModelPass().GetItem(in_index).index);
			break;
		case crg_prk:
			return data_set->GetAbsCoord().GetItem(data_set->GetAnpModelTruck().GetItem(in_index).index);
			break;
		case high_prk: 
			return data_set->GetAbsCoord().GetItem(data_set->GetAnpModelHigh().GetItem(in_index).index);
			break;
		case high_bt_prk: 
			return data_set->GetAbsCoord().GetItem(data_set->GetAnpModelHighBT().GetItem(in_index).index);
			break;
	}
}

/** ���������� ���������� �� ��� Y*/
double VAnpModelData::GetY(size_t in_index) const
{	
	switch (speed)
	{
		default:
		case pass_prk: 
			return sign_y*data_set->GetAnpModelPass().GetItem(in_index).value;
			break;
		case crg_prk:
			return sign_y*data_set->GetAnpModelTruck().GetItem(in_index).value;
			break;
		case high_prk: 
			return sign_y*data_set->GetAnpModelHigh().GetItem(in_index).value;
			break;
		case high_bt_prk: 
			return sign_y*data_set->GetAnpModelHighBT().GetItem(in_index).value;
			break;
	}
}

/** ���������� ����������� �������� �� Y*/
double VAnpModelData::GetMinY()
{	
	if (GetCount()==0)
		return 0;
	switch (speed)
	{
		default:
		case pass_prk: 
			return sign_y>0 ? data_set->GetAnpModelPass().GetMin().value :
				sign_y * data_set->GetAnpModelPass().GetMax().value;
			break;
		case crg_prk:
			return sign_y>0 ? data_set->GetAnpModelTruck().GetMin().value :
				sign_y * data_set->GetAnpModelTruck().GetMax().value;
			break;
		case high_prk: 
			return sign_y>0 ? data_set->GetAnpModelHigh().GetMin().value :
				sign_y * data_set->GetAnpModelHigh().GetMax().value;
			break;
		case high_bt_prk: 
			return sign_y>0 ? data_set->GetAnpModelHighBT().GetMin().value :
				sign_y * data_set->GetAnpModelHighBT().GetMax().value;
			break;
	}	
}

/** ���������� ������������ �������� �� Y*/
double VAnpModelData::GetMaxY()
{
	if (GetCount()==0)
		return 0;
	switch (speed)
	{
		default:
		case pass_prk: 
			return sign_y>0 ? data_set->GetAnpModelPass().GetMax().value : 
				sign_y*data_set->GetAnpModelPass().GetMin().value;
			break;
		case crg_prk:
			return sign_y>0 ? data_set->GetAnpModelTruck().GetMax().value : 
				sign_y*data_set->GetAnpModelTruck().GetMin().value;
			break;
		case high_prk: 
			return sign_y>0 ? data_set->GetAnpModelHigh().GetMax().value : 
				sign_y*data_set->GetAnpModelHigh().GetMin().value;
			break;
		case high_bt_prk: 
			return sign_y>0 ? data_set->GetAnpModelHighBT().GetMax().value : 
				sign_y*data_set->GetAnpModelHighBT().GetMin().value;
			break;
	}
}

/** ���������� ����������� �������� �� X*/
double VAnpModelData::GetMinX()
{
	return data_set->GetAbsCoord().GetItem(0);
}

/** ���������� ������������ �������� �� X*/
double VAnpModelData::GetMaxX()
{
	return data_set->GetAbsCoord().GetLastItem();
}

//--------------------------------------- ������ ������ VCurvesInfo ---------------------------------------------

/** �����������*/
VCurvesInfo::VCurvesInfo(VDataSetPtr in_data_set, bool in_model)
{
	if (in_data_set)
	{
		data_set=in_data_set;		
		is_model=in_model;
		data_set->Load(is_model ? PRM_MODEL_CURVES : PRM_CURVE_MARKS);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ����� ������*/
size_t VCurvesInfo::GetCount() const
{
	return data_set->GetCurves(is_model ? PRM_MODEL_CURVES : PRM_CURVE_MARKS).GetCount();
}

/** ���������� ������*/
const VCurve& VCurvesInfo::GetCurve(size_t in_index) const
{
	return data_set->GetCurves(is_model ? PRM_MODEL_CURVES : PRM_CURVE_MARKS).GetItem(in_index);
}

/** ���������� ��������� ������*/
bool VCurvesInfo::Valid(size_t in_index)
{
	return data_set->GetCurves(is_model ? PRM_MODEL_CURVES : PRM_CURVE_MARKS).GetItem(in_index).CheckMarks();
}

/** ���������� ������ ������*/
double VCurvesInfo::GetAbsStart(size_t in_index) const
{
	return GetCurve(in_index).GetRangeCoord(*data_set, true, false, true);
}

/** ���������� ����� ������*/
double VCurvesInfo::GetAbsEnd(size_t in_index) const
{
	return GetCurve(in_index).GetRangeCoord(*data_set, true, false, false);
}

/** ���������� ������ ������ (�������)*/
double VCurvesInfo::GetAbsLevelStart(size_t in_index) const
{
	return GetCurve(in_index).GetRangeCoord(*data_set, false, false, true);
}

/** ���������� ����� ������ (�������)*/
double VCurvesInfo::GetAbsLevelEnd(size_t in_index) const
{
	return GetCurve(in_index).GetRangeCoord(*data_set, false, false, false);

}

/** ���������� ����������*/
long VCurvesInfo::GetOrientation(size_t in_index) const
{
	return GetCurve(in_index).GetOrientation(*data_set);
}

/** ���������� ����� ��������*/
size_t VCurvesInfo::GetRadiusCount(size_t in_index) const
{
	return GetCurve(in_index).GetRadiusCount();
}

/** ���������� ������ �������*/
double VCurvesInfo::GetAbsStart(size_t in_index, size_t in_radius_index) const
{
	return GetCurve(in_index).GetRangeCoord(*data_set, true, false, true, in_radius_index);
}

/** ���������� ����� �������*/
double VCurvesInfo::GetAbsEnd(size_t in_index, size_t in_radius_index) const
{
	return GetCurve(in_index).GetRangeCoord(*data_set, true, false, false, in_radius_index);
}

/** ���������� ������*/
double VCurvesInfo::GetRadius(size_t in_index, size_t in_radius_index) const
{
	//return GetCurve(in_index).GetAvgRadius(*data_set, in_radius_index);
	return GetCurve(in_index).GetElementRadius(radius_to_element(in_radius_index));
}

/** ���������� ����������*/
double VCurvesInfo::GetLevel(size_t in_index, size_t in_radius_index) const
{
	return GetCurve(in_index).GetAvgLevel(*data_set, in_radius_index);
}

/** ���������� �����*/
double VCurvesInfo::GetLength(size_t in_index, size_t in_radius_index) const
{	
	return GetCurve(in_index).GetElementLength(*data_set, 2*in_radius_index+1);
}

/** ���������� ����� ������ ����������*/
double VCurvesInfo::GetLength1(size_t in_index, size_t in_radius_index) const
{
	return GetCurve(in_index).GetElementLength(*data_set, 2*in_radius_index);	
}

/** ���������� ����� ������ ����������*/
double VCurvesInfo::GetLength2(size_t in_index, size_t in_radius_index) const
{
	return GetCurve(in_index).GetElementLength(*data_set, 2*in_radius_index+2);	
}

/** ���������� ���� ��������*/
double VCurvesInfo::GetAngle(size_t in_index, size_t in_radius_index) const
{
	//FixMe!!! GetAngle ���������� ���� � �������� ���������� ���������� ������� � �������
	return M_PI*GetCurve(in_index).GetAngle(*data_set, in_radius_index)/180.0;
}

/** ���������� ����� ������� ���������� ������ �������*/
double VCurvesInfo::GetMStart(size_t in_index, size_t in_radius_index) const
{
	return data_set->GetAprinfo().GetRealCoord(GetAbsStart(in_index, in_radius_index)).m;
}

/** ���������� ����� ������� ���������� ����� �������*/
double VCurvesInfo::GetMEnd(size_t in_index, size_t in_radius_index) const
{
	return data_set->GetAprinfo().GetRealCoord(GetAbsEnd(in_index, in_radius_index)).m;
}

/** ���������� ������ ������*/
std::wstring VCurvesInfo::GetStatus(size_t in_index) const
{		
	std::wstring result=string_to_wstring(GetCurve(in_index).GetModelStatusDescription());
	if (strcmp(GetCurve(in_index).GetModelComment(), "") !=0)
	{
		result=result+L" ("+string_to_wstring(GetCurve(in_index).GetModelComment())+L"";
	}
	return result;
}

/** ���������� true ���� ������� ������ - ��������*/
bool VCurvesInfo::GetElementType(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementType(in_element_index);
}

/** ���������� ����� �������� ������*/
double VCurvesInfo::GetElementLength(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementLength(*data_set, in_element_index);
}

/** ���������� ������ �������� ������*/
double VCurvesInfo::GetElementRadius(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementRadius(in_element_index);
}

/** ���������� ����� �������� ������ (�������)*/
double VCurvesInfo::GetElementLengthLevel(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementLengthLevel(*data_set, in_element_index);
}

/** ���������� ���������� �������� ������*/
double VCurvesInfo::GetElementLevel(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementLevel(in_element_index);
}

/** ���������� V��*/
double VCurvesInfo::GetModelVkr_high_bt(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementVkrHighBT(*data_set, in_element_index);
}

/** ���������� V��*/
double VCurvesInfo::GetModelVkr_high(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementVkrHigh(*data_set, in_element_index);
}

/** ���������� V��*/
double VCurvesInfo::GetModelVkr_pass(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementVkrPass(*data_set, in_element_index);
}

/** ���������� V��*/
double VCurvesInfo::GetModelVkr_truck(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementVkrTruck(*data_set, in_element_index);
}

/** ���������� V��*/
double VCurvesInfo::GetModelViz_high_bt(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementVizHighBT(*data_set, in_element_index);
}

/** ���������� V��*/
double VCurvesInfo::GetModelViz_high(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementVizHigh(*data_set, in_element_index);
}

/** ���������� V��*/
double VCurvesInfo::GetModelViz_pass(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementVizPass(*data_set, in_element_index);
}

/** ���������� V��*/
double VCurvesInfo::GetModelVpr(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementVpr(*data_set, in_element_index);
}

/** ���������� ����� ����������*/
double VCurvesInfo::GetModelTap(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementTap(*data_set, in_element_index);
}

/** ���������� ���*/
double VCurvesInfo::GetModelPsi_high_bt(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementPsiHighBT(*data_set, in_element_index);
}

/** ���������� ���*/
double VCurvesInfo::GetModelPsi_high(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementPsiHigh(*data_set, in_element_index);
}

/** ���������� ���*/
double VCurvesInfo::GetModelPsi_pass(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementPsiPass(*data_set, in_element_index);
}

/** ���������� ���*/
double VCurvesInfo::GetModelPsi_truck(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementPsiTruck(*data_set, in_element_index);
}

/** ���������� ���*/
double VCurvesInfo::GetModelPsi_avg(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementPsiThird(*data_set, in_element_index);
}

/** ���������� A�� ����. ��*/
double VCurvesInfo::GetModelAnpHighBT(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementAnpHighBT(*data_set, in_element_index);
}

/** ���������� A�� ����.*/
double VCurvesInfo::GetModelAnpHigh(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementAnpHigh(*data_set, in_element_index);
}

/** ���������� A��*/
double VCurvesInfo::GetModelAnpPass(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementAnpPass(*data_set, in_element_index);
}

/** ���������� A�� ����.*/
double VCurvesInfo::GetModelAnpTruck(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementAnpTruck(*data_set, in_element_index);
}

/** ���������� A�� �����.*/
double VCurvesInfo::GetModelAnpAvg(size_t in_index, size_t in_element_index) const
{
	return GetCurve(in_index).GetElementAnpThird(*data_set, in_element_index);
}

/** ���������� Vhigh_bt*/
speed_t VCurvesInfo::GetModelVhigh_bt(size_t in_index) const
{
	return GetCurve(in_index).GetModelVhigh_bt();
}

/** ���������� Vhigh*/
speed_t VCurvesInfo::GetModelVhigh(size_t in_index) const
{
	return GetCurve(in_index).GetModelVhigh();
}

/** ���������� Vpass*/
speed_t VCurvesInfo::GetModelVpass(size_t in_index) const
{
	return GetCurve(in_index).GetModelVpass();
}

/** ���������� Vcrg*/
speed_t VCurvesInfo::GetModelVcrg(size_t in_index) const
{
	return GetCurve(in_index).GetModelVcrg();
}

/** ���������� Vavg*/
speed_t VCurvesInfo::GetModelVavg(size_t in_index) const
{
	return GetCurve(in_index).GetModelVavg();
}

/** ���������� ������������ ���������� � �����*/
double VCurvesInfo::GetMaxPlanOverstep(size_t in_index)
{
	return GetCurve(in_index).GetMaxPlanOffset(*data_set);
}

//------------------------------------- ������ ������ VLinesData -------------------------------------------------

/** �����������*/
VLinesData::VLinesData(VDataSetPtr in_data_set) : VXOrderedDataProvider()
{
	if (in_data_set)
	{
		data_set=in_data_set;		
		data_set->Load(PRM_LINES);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VLinesData::GetCount() const
{
	return data_set->GetLines().GetCount();
}

/** ���������� ���������� �� ��� X*/
double VLinesData::GetX(size_t in_index) const
{
	return data_set->GetAbsCoord().GetItem( data_set->GetLines().GetItem(in_index).GetStartIndex());
}

/** ���������� ���������� �� ��� Y*/
double VLinesData::GetY(size_t in_index) const
{	
	/*if (in_index>0)
		return data_set->GetLines().GetAngle(data_set->GetPlan(), in_index-1, in_index);
	else*/
	return 0;
}

/** ���������� ������ "�����" �� X*/
double VLinesData::GetSizeX(size_t in_index) const
{
	return data_set->GetAbsCoord().GetItem(data_set->GetLines().GetItem(in_index).GetEndIndex()) -
		data_set->GetAbsCoord().GetItem(data_set->GetLines().GetItem(in_index).GetStartIndex());
}

/** ���������� ����������� �������� �� Y*/
double VLinesData::GetMinY()
{
	return 0;
}

/** ���������� ������������ �������� �� Y*/
double VLinesData::GetMaxY()
{
	return 0;
}

/** ���������� ����������� �������� �� X*/
double VLinesData::GetMinX()
{
	return data_set->GetAbsCoord().GetItem(0);
}

/** ���������� ������������ �������� �� X*/
double VLinesData::GetMaxX()
{
	return data_set->GetAbsCoord().GetLastItem();
}

/** ���������� �����*/
std::wstring VLinesData::GetLabel(size_t in_index) const
{
	return boost::str(boost::wformat(L"%0.1f") % GetSizeX(in_index));
}

//------------------------------------- ������ ������ VLineAngleData --------------------------------

/** �����������*/
VLineAngleData::VLineAngleData(VDataSetPtr in_data_set)
{
	if (in_data_set)
	{
		data_set=in_data_set;		
		data_set->Load(PRM_LINES);
	}
	else
	{
		throw NULL_POINTER;
	}
}
/** ���������� ������ ������*/
size_t VLineAngleData::GetCount() const
{
	return data_set->GetLines().GetCount();
}
/** ���������� ���������� �� ��� X*/
double VLineAngleData::GetX(size_t in_index) const
{
	return data_set->GetAbsCoord().GetItem( data_set->GetLines().GetItem(in_index).GetStartIndex());
}
/** ���������� ���������� �� ��� Y*/
double VLineAngleData::GetY(size_t in_index) const
{
	return 0;
}
/** ���������� ����������� �������� �� Y*/
double VLineAngleData::GetMinY()
{
	return 0;
}
/** ���������� ������������ �������� �� Y*/
double VLineAngleData::GetMaxY()
{
	return 0;
}
/** ���������� ����������� �������� �� X*/
double VLineAngleData::GetMinX()
{
	return data_set->GetAbsCoord().GetItem(0);
}
/** ���������� ������������ �������� �� X*/
double VLineAngleData::GetMaxX()
{
	return data_set->GetAbsCoord().GetLastItem();
}
/** ���������� �����*/
std::wstring VLineAngleData::GetLabel(size_t in_index) const
{
	if (in_index>0)
	{
		double angle_rad=data_set->GetLines().GetAngle(data_set->GetPlan(), in_index-1, in_index);
		if (angle_rad!=0)
		{
			angle_t angle(angle_rad);
			return angle.format_degrees();
		}
		else
			return L"";
	}
	else
		return L"";
}

//------------------------------------- ������ ������ VSidewayData -----------------------------------------------

/** �����������*/
VSidewayData::VSidewayData(VDataSetPtr in_data_set) : VDataProvider()
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_set->Load(PRM_SIDEWAYS);
		selected_point=BAD_INDEX;
	}
	else
	{
		throw NULL_POINTER;
	}
};

/** ���������� ������ ������*/
size_t VSidewayData::GetCount() const
{
	return data_set->GetSideways().GetCount();
};	

/** ���������� ���������� �� ��� X*/
double VSidewayData::GetX(size_t in_index) const
{
	size_t start_index=data_set->GetSideways().GetItem(in_index).GetStartNdx();
	size_t end_index=data_set->GetSideways().GetItem(in_index).GetEndNdx();		
	return data_set->GetAbsCoord().GetItem(start_index);
};	

/** ���������� ���������� �� ��� Y*/
double VSidewayData::GetY(size_t in_index) const 
{
	return 0;
};	

/** ���������� ������ "�����" �� X*/
double VSidewayData::GetSizeX(size_t in_index) const
{
	size_t start_index=data_set->GetSideways().GetItem(in_index).GetStartNdx();
	size_t end_index=data_set->GetSideways().GetItem(in_index).GetEndNdx();
	return data_set->GetAbsCoord().GetItem(end_index) - data_set->GetAbsCoord().GetItem(start_index);
};

/** ���������� ����������� �������� �� Y*/
double VSidewayData::GetMinY() 
{
	return DBL_MAX;
};

/** ���������� ������������ �������� �� Y*/
double VSidewayData::GetMaxY() 
{
	return 0-DBL_MAX;
};

/** ���������� ����������� �������� �� X*/
double VSidewayData::GetMinX() 
{
	return GetCount()>0 ? data_set->GetAbsCoord().GetItem(data_set->GetSideways().GetItem(0).GetStartNdx()) : 0;
};

/** ���������� ������������ �������� �� X*/
double VSidewayData::GetMaxX()
{		
	return GetCount()>0 ? data_set->GetAbsCoord().GetItem(data_set->GetSideways().GetItem(GetCount()-1).GetEndNdx()) : 0;
};	

/** ���������� �����*/
std::wstring VSidewayData::GetLabel(size_t in_index) const
{
	std::wstring result;
	switch (data_set->GetSideways().GetItem(in_index).GetReason())
	{
		case REASON_SIDEWAY:
			result=L"�����";
			break;
		case REASON_HALFCURVE:
			result=L"����.";
			break;
		case REASON_WAY:
			result=L"����";
			break;
		case REASON_UNDEFINED:
			result=L"?";
		default:
			break;
	}
	return result;
};

/** ��������� ������� ����*/
void VSidewayData::AddXY(size_t in_index, double in_x, double in_y, double in_size_x, double in_size_y, 
		const std::wstring& in_label, uint64_t in_flags)
{	
	VSidewayRecord sideway;
	sideway.startNdx=data_set->GetAbsCoord().FindIndex(in_x, true);
	sideway.endNdx=data_set->GetAbsCoord().FindIndex(in_x+in_size_x, true);
	sideway.reason=static_cast<uint32_t>(in_flags);
	VSideway _sideway;
	_sideway.Import(sideway);
	if (data_set->AllowSidewayHere(sideway.startNdx, sideway.endNdx, PRM_CURVE_MARKS) && 
		data_set->AllowSidewayHere(sideway.startNdx, sideway.endNdx, PRM_MODEL_CURVES))
	{
		data_set->GetSideways().AddItem(_sideway);
		data_set->SetChanged(PRM_SIDEWAYS);	
	}
	else
	{
		throw VSimpleException(L"���������� �������� ������� ������� �� ������� ���� ��� �������� ������.",
			L"����������� � ������ ��� ������ �� ������.");
	}
}

/** ������� ������� ����*/
void VSidewayData::DeleteItem(size_t in_index)
{
	data_set->GetSideways().DeleteItem(in_index);
	data_set->SetChanged(PRM_SIDEWAYS);
}

/** ������������� ��� �������*/
void VSidewayData::SetFlags(size_t in_index, uint64_t in_flags)
{
	VSidewayRecord sideway=data_set->GetSideways().GetItem(in_index).Export();
	sideway.reason=static_cast<uint32_t>(in_flags);
	data_set->GetSideways().GetItem(in_index).Import(sideway);
	data_set->SetChanged(PRM_SIDEWAYS);
}

//---------------------------------------- ������ ������ VQueryData --------------------------------------------

/** �����������*/
VQueryData::VQueryData(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, const std::wstring& in_query) 
	 : VXOrderedDataProvider(), database(in_database), transaction(in_transaction), query(in_query)
{
	RefreshData();
}

/** ��������� ������*/
void VQueryData::RefreshData()
{
	VIBDataHandlerParameter handler;	
	database->ExecuteStatement(*transaction, wstring_to_string(query).c_str(), handler);
	if (handler.GetColCount()>=2)
	{		
		data.reserve(handler.GetRowCount());
		data.resize(handler.GetRowCount());			
	
		min_x=DBL_MAX;
		max_x=(0-DBL_MAX);
		min_y=DBL_MAX;
		max_y=(0-DBL_MAX);

		for (size_t i=0; i<handler.GetRowCount(); ++i)
		{	
			data[i].x=handler.HasField(L"x") ? handler.GetParameter(i,L"x").GetFloat() : 0;
			data[i].y=handler.HasField(L"y") ? handler.GetParameter(i,L"y").GetFloat() : 0;			
			data[i].xsize=handler.HasField(L"size_x") ? handler.GetParameter(i,L"size_x").GetFloat() : 0;
			data[i].ysize=handler.HasField(L"size_y") ? handler.GetParameter(i,L"size_y").GetFloat() : 0;
			data[i].label=handler.HasField(L"label") ? handler.GetParameter(i,L"label").GetString() : L"";
			data[i].label_x=handler.HasField(L"label_x") ? handler.GetParameter(i,L"label_x").GetString() : L"";
			data[i].label_y=handler.HasField(L"label_y") ? handler.GetParameter(i,L"label_y").GetString() : L"";
			data[i].flags=handler.HasField(L"flags") ? static_cast<uint64_t>(handler.GetParameter(i,L"flags").GetInteger()) : 0;
			
			min_x=std::min<double>(min_x, std::min<double>(data[i].x, data[i].x+data[i].xsize));
			max_x=std::max<double>(max_x, std::max<double>(data[i].x, data[i].x+data[i].xsize));
			min_y=std::min<double>(min_y, std::min<double>(data[i].y, data[i].y+data[i].ysize));
			max_y=std::max<double>(max_y, std::max<double>(data[i].y, data[i].y+data[i].ysize));
		}
	}
	else
	{
		throw VSimpleException(L"������ ������ ������� �� ����� ���� ���������", query, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
};

/** ���������� ������ ������*/
size_t VQueryData::GetCount() const
{
	return data.size();
}

/** ���������� ���������� �� ��� X*/
double VQueryData::GetX(size_t in_index) const
{
	return data.at(in_index).x;
}

/** ���������� ���������� �� ��� Y*/
double VQueryData::GetY(size_t in_index) const
{
	return data.at(in_index).y;
}

/** ���������� ������ "�����" �� X (����� ����� ��� ����������� ��������)*/
double VQueryData::GetSizeX(size_t in_index) const
{
	return data.at(in_index).xsize;
}

/** ���������� ������ "�����" �� Y (����� ����� ��� ����������� ��������)*/
double VQueryData::GetSizeY(size_t in_index) const
{
	return data.at(in_index).ysize;
}

/** ���������� �����*/
std::wstring VQueryData::GetLabel(size_t in_index) const
{
	return data.at(in_index).label;
}

/** ���������� ���� - ��� �����*/
uint64_t VQueryData::GetFlags(size_t in_index) const
{
	return data.at(in_index).flags;
}

bool less_y(const VPointData& l, const VPointData& r)
{
	return l.y < r.y;
}

/** ���������� ����������� �������� �� Y*/
double VQueryData::GetMinY()
{		
	return min_y;
}

/** ���������� ������������ �������� �� Y*/
double VQueryData::GetMaxY()
{		
	return max_y;
}

/** ���������� ����������� �������� �� X*/
double VQueryData::GetMinX()
{
	return min_x;
}

/** ���������� ������������ �������� �� X*/
double VQueryData::GetMaxX()
{	
	return max_x;
}

//------------------------------------------ ������ ������ VSelectionInfo -----------------------------------------

/** �����������*/
VSelectionInfo::VSelectionInfo()
{		
	selecting=false;
	active=false;
	hit=false;
	Reset();
}

/** ������ ���������*/
void VSelectionInfo::SetStart(double in_x, double in_y)
{				
	select_start_x=in_x;
	select_start_y=in_y;
};

/** ����� ���������*/
void VSelectionInfo::SetEnd(double in_x, double in_y)
{		
	select_end_x=in_x;
	select_end_y=in_y;
};

/** ���������� X ���������� ��������� �����*/
double VSelectionInfo::GetStartX() const 
{		
	return select_start_x;
};
/** ���������� Y ���������� ��������� �����*/
double VSelectionInfo::GetStartY() const 
{		
	return select_start_y;
};

/** ���������� X ���������� �������� �����*/
double VSelectionInfo::GetEndX() const 
{		
	return select_end_x;
};

/** ���������� Y ���������� �������� �����*/
double VSelectionInfo::GetEndY() const 
{		
	return select_end_y;
};

/** ����� �������� � 0*/
void VSelectionInfo::Reset()
{
	select_start_x=0;
	select_start_y=0;
	select_end_x=0;
	select_end_y=0;
	hit_left=false;
	hit_right=false;
};

/** ���������� true, ���� ������� ���������*/
bool VSelectionInfo::Valid() const 
{
	return fabs(select_start_x-select_end_x)>=1;
};


/** ������������� ������� ����, ��� ���������� ���������*/
void VSelectionInfo::SetSelecting(bool in_selecting) 
{
	selecting=in_selecting;
};

/** ���������� ������� ����, ��� ���������� ���������*/
bool VSelectionInfo::GetSelecting() const 
{
	return selecting;
};

/** ������������� ������� ����������*/
void VSelectionInfo::SetActive(bool in_active)
{	
	active=in_active;	
};


//--------------------------------------- ������ ������ VPageInfo -------------------------------------------

/** ���������� ������� ��������*/
size_t VPageInfo::GetPage() const 
{
	return page;
};

/** ������������� ������� ��������*/
void VPageInfo::SetPage(size_t in_page)
{
	page=in_page;
};

//----------------------------- ������ ������ VCurveMarksData --------------------------

/** ����������� �������� ������ � ���� - ������/�����*/
index_cache_t VCurveMarksData::IndexToPair(size_t in_index) const
{
	return cache.at(in_index);
};

/** ���������� ����� ����� ������*/
size_t VCurveMarksData::GetMarkCount(size_t in_index) const
{
	if (in_index<data_set->GetCurves(mark_type).GetCount())
	{
		switch (data_type)
		{
			default:
			case PRM_CURV:
				return data_set->GetCurves(mark_type).GetItem(in_index).GetMarkCount();
			case PRM_LEVEL:
				return data_set->GetCurves(mark_type).GetItem(in_index).GetLevelMarkCount();
		}
	}
	else
		return 0;
};

/** ���������� ������ �� �����*/
VCurveMark& VCurveMarksData::GetMark(size_t in_curve_index, size_t in_mark_index)
{
	switch (data_type)
	{
		default:
		case PRM_CURV:
			return data_set->GetCurves(mark_type).GetItem(in_curve_index).GetPlanMark(in_mark_index);
		case PRM_LEVEL:
			return data_set->GetCurves(mark_type).GetItem(in_curve_index).GetLevelMark(in_mark_index);
	}
}

/** ���������� ������ �� �����*/
const VCurveMark& VCurveMarksData::GetMark(size_t in_curve_index, size_t in_mark_index) const
{
	switch (data_type)
	{
		default:
		case PRM_CURV:
			return data_set->GetCurves(mark_type).GetItem(in_curve_index).GetPlanMark(in_mark_index);
		case PRM_LEVEL:
			return data_set->GetCurves(mark_type).GetItem(in_curve_index).GetLevelMark(in_mark_index);
	}
}

/** ��������� �����*/
bool VCurveMarksData::AddMark(VCurve& in_curve, size_t in_index, float in_value)
{
	switch (data_type)
	{
		default:
		case PRM_CURV:
			if (data_set->AllowMarkHerePlan(in_index, mark_type))
			{
				in_curve.AddPlanMark(in_index, in_value);
				data_set->SetChanged(mark_type);
				return true;
			}
			break;
		case PRM_LEVEL:
			if (data_set->AllowMarkHereLevel(in_index, mark_type))
			{
				in_curve.AddLevelMark(in_index, in_value);
				data_set->SetChanged(mark_type);	
				data_set->UpdateDiffLevel(mark_type==PRM_MODEL_CURVES);
				return true;
			}
			break;
	}	
	return false;
}

/** ������� �����*/
void VCurveMarksData::DeleteMark(size_t in_index)
{
	size_t active_index=data_set->GetCurves(mark_type).GetActiveCurve();
	if (active_index!=BAD_INDEX)
	{
		switch (data_type)
		{
			default:
			case PRM_CURV:
			{
				data_set->GetCurves(mark_type).GetItem(active_index).DeletePlanMark(in_index);
				data_set->SetChanged(mark_type);	
				break;
			}
			case PRM_LEVEL:
			{
				data_set->GetCurves(mark_type).GetItem(active_index).DeleteLevelMark(in_index);
				data_set->SetChanged(mark_type);
				data_set->UpdateDiffLevel(mark_type==PRM_MODEL_CURVES);
				break;
			}
		}		
	}	
}

/** ����������� �����*/
void VCurveMarksData::NormalizeMarks()
{
	size_t active_index=data_set->GetCurves(mark_type).GetActiveCurve();
	if (active_index!=BAD_INDEX)
	{
		switch (data_type)
		{
			default:
			case PRM_CURV:
				if (data_set->GetCurves(mark_type).GetItem(active_index).PlanMarksValid())
				{
					data_set->GetCurves(mark_type).GetItem(active_index).NormalizePlanMarks(*data_set);
				}
				break;
			case PRM_LEVEL:
				if (data_set->GetCurves(mark_type).GetItem(active_index).LevelMarksValid())
				{
					data_set->GetCurves(mark_type).GetItem(active_index).NormalizeLevelMarks(*data_set);
					data_set->UpdateDiffLevel(mark_type==PRM_MODEL_CURVES);
				}
				break;
		}
		data_set->SetChanged(mark_type);	
	}	
}

/** ��������� ���*/
void VCurveMarksData::BuildCache() const
{	
	cache.clear();
	curves_count=data_set->GetCurves(mark_type).GetCount();
	for (size_t i=0; i<data_set->GetCurves(mark_type).GetCount(); ++i)
	{
		for (size_t j=0; j<GetMarkCount(i); ++j)
		{			
			cache.push_back(index_cache_t(i,j,GetMark(i,j).index));
		}
	}
	std::sort(cache.begin(), cache.end());
	size_t active_index=data_set->GetCurves(mark_type).GetActiveCurve();
	active_mark_count=active_index!=BAD_INDEX ? GetMarkCount(active_index) : 0;	
}

/** �����������*/
VCurveMarksData::VCurveMarksData(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type, PARAMETER_TYPE in_mark_type) : VXOrderedDataProvider()
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_type=in_data_type;
		if (in_mark_type!=PRM_MODEL_CURVES && in_mark_type!=PRM_CURVE_MARKS)
			throw VSimpleException(L"�������� ��� ������", L"���������� ���� ������: CURVE_MARKS � MODEL_CURVES", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		mark_type=in_mark_type;
		data_set->Load(mark_type);				
		SetActive(true);		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VCurveMarksData::GetCount() const
{
	return cache.size();
}

/** ���������� ���������� �� ��� X*/
double VCurveMarksData::GetX(size_t in_index) const
{	
	index_cache_t pair_index=IndexToPair(in_index);
	VCurveMark mark = GetMark(pair_index.first, pair_index.second);
	return data_set->GetAbsCoord().GetItem(mark.index);
}

/** ���������� ���������� �� ��� Y*/
double VCurveMarksData::GetY(size_t in_index) const
{
	index_cache_t pair_index=IndexToPair(in_index);	
	VCurveMark mark = GetMark(pair_index.first, pair_index.second);
	return mark.value;	
}

/** ���������� ���� - ��� �����*/
uint64_t VCurveMarksData::GetFlags(size_t in_index) const
{	
	index_cache_t pair_index=IndexToPair(in_index);			
	uint32_t curve_orientation=data_set->GetCurves(mark_type).GetItem(pair_index.first).GetOrientation(*data_set);
	uint32_t mark_position=INNER_MARK;	
	if (pair_index.first!=data_set->GetCurves(mark_type).GetActiveCurve())
	{
		if (pair_index.second==0)			
			mark_position=FIRST_MARK;
		else if (pair_index.second<GetMarkCount(pair_index.first)-1)			
			mark_position=INNER_MARK;
		else			
			mark_position=LAST_MARK;
	}
	else
	{
		if (pair_index.second==0)			
			mark_position=ACTIVE_FIRST_MARK;
		else if (pair_index.second<GetMarkCount(pair_index.first)-1)			
			mark_position=ACTIVE_INNER_MARK;
		else			
			mark_position=ACTIVE_LAST_MARK;
	}		
	return compose_flags(curve_orientation, mark_position);
}

/** ������������� �������� X*/
void VCurveMarksData::SetXY(size_t in_index, double in_x, double in_y)
{
	index_cache_t pair_index=IndexToPair(in_index);
	if (GetActive())
	{		
		size_t index=data_set->GetAbsCoord().FindIndex(in_x, true);

		switch (data_type)
		{
			default:
			case PRM_CURV:
				if (index!=BAD_INDEX && data_set->AllowMarkHerePlan(index, mark_type))
				{			

					size_t curve_index=pair_index.first;
					//���� ����� �� ���� �� ��������
					if (index>data_set->GetCurves(mark_type).GetItem(curve_index).GetPlanMarkLeftMargin(*data_set, pair_index.second) && 
						index<data_set->GetCurves(mark_type).GetItem(curve_index).GetPlanMarkRightMargin(*data_set, pair_index.second))
					{
						GetMark(curve_index, pair_index.second).index=index;
						GetMark(curve_index, pair_index.second).value=static_cast<float>(in_y);
						NormalizeMarks();
						data_set->GetCurves(mark_type).GetItem(curve_index).Drop();
						//��������� ����������											
						data_set->UpdatePlanOffset(index, mark_type==PRM_MODEL_CURVES);
						//��������� ������������ ��������� � 
						if (mark_type==PRM_MODEL_CURVES)
						{
							data_set->UpdateAnpModel();					
						}						
						data_set->SetChanged(mark_type);
					}
				}
				break;
			case PRM_LEVEL:
				if (index!=BAD_INDEX && data_set->AllowMarkHereLevel(index, mark_type))
				{			
					size_t curve_index=pair_index.first;
					//���� ����� �� ���� �� ��������
					if (index>data_set->GetCurves(mark_type).GetItem(curve_index).GetLevelMarkLeftMargin(*data_set, pair_index.second) && 
						index<data_set->GetCurves(mark_type).GetItem(curve_index).GetLevelMarkRightMargin(*data_set, pair_index.second))
					{
						GetMark(curve_index, pair_index.second).index=index;
						GetMark(curve_index, pair_index.second).value=static_cast<float>(in_y);
						NormalizeMarks();
						data_set->GetCurves(mark_type).GetItem(curve_index).Drop();						
						//��������� ������������ ��������� � 
						if (mark_type==PRM_MODEL_CURVES)
						{
							data_set->UpdateAnpModel();					
						}
						//��������� ���������� ������ �� ��������
						data_set->UpdateDiffLevel(mark_type==PRM_MODEL_CURVES);
						data_set->SetChanged(mark_type);
					}
				}
				break;
		}
	}
}

/** ��������� �����*/
void VCurveMarksData::AddXY(size_t in_index, double in_x, double in_y, double in_size_x, double in_size_y, 
		const std::wstring& in_label, uint64_t in_flags)
{		
	size_t index=data_set->GetAbsCoord().FindIndex(in_x, true);
	if (index!=BAD_INDEX)
	{
		size_t active_index=data_set->GetCurves(mark_type).GetActiveCurve();
		//���� ���� �������� ������ ...
		if (active_index!=BAD_INDEX)
		{		
			AddMark(data_set->GetCurves(mark_type).GetItem(active_index), index, static_cast<float>(in_y));
		}	
		else
		{			
			VCurve curve;
			curve.SetModel(mark_type==PRM_MODEL_CURVES);
			if (AddMark(curve, index, static_cast<float>(in_y)))
			{
				data_set->GetCurves(mark_type).AddItem(curve);
				data_set->GetCurves(mark_type).Sort();
				for (size_t i=0; i<data_set->GetCurves(mark_type).GetCount(); ++i)
				{
					if (GetMarkCount(i)==1)
						data_set->GetCurves(mark_type).ActivateCurve(i);
				}
			}		
		}
		//���������� �������� ���
		BuildCache();		
	}
}

/** ������� �����*/
void VCurveMarksData::DeleteItem(size_t in_index)
{
	index_cache_t pair_index=IndexToPair(in_index);
	if (GetActive())
	{
		data_set->GetCurves(mark_type).ActivateCurve(pair_index.first);
		DeleteMark(pair_index.second);		
		//���� ����� ������ ��� ���������� ������� � �������������� ������
		if (data_set->GetCurves(mark_type).GetItem(pair_index.first).GetMarkCount()==0 &&
			data_set->GetCurves(mark_type).GetItem(pair_index.first).GetLevelMarkCount()==0)
		{
			data_set->GetCurves(mark_type).DeleteActiveCurve();
		}
		//���������� �������� ���
		BuildCache();
		data_set->SetChanged(mark_type);	
	}
}

/** ���������� �����*/
std::wstring VCurveMarksData::GetLabel(size_t in_index) const
{	
	return L"";
}

/** ���������� ����������� �������� �� Y*/
double VCurveMarksData::GetMinY()
{
	return DBL_MAX;
}

/** ���������� ������������ �������� �� Y*/
double VCurveMarksData::GetMaxY()
{
	return 0-DBL_MAX;
}

/** ���������� ����������� �������� �� X*/
double VCurveMarksData::GetMinX()
{
	return 0;
}

/** ���������� ������������ �������� �� X*/
double VCurveMarksData::GetMaxX()
{
	return 0;
}

/** �������� �����*/
void VCurveMarksData::SelectPoint(size_t in_index)
{	
	selected_point=in_index;
}

/** ���������� ���������� �����*/
size_t VCurveMarksData::GetSelectedPoint() const
{
	return selected_point;
}

/** ������� ���������*/
void VCurveMarksData::DeselectPoint()
{	
	selected_point=BAD_INDEX;
}

/** �������� ������ �����*/
void VCurveMarksData::MarkPoints(size_t in_from, size_t in_to) 
{
	index_cache_t pair_index=IndexToPair(in_from);
	data_set->GetCurves(mark_type).ActivateCurve(pair_index.first);
};

/** ������� �������*/
void VCurveMarksData::UnmarkPoints() 
{
	data_set->GetCurves(mark_type).DeactivateCurve();
};


/** ���������� ������� �������*/
std::vector<size_t> VCurveMarksData::GetVisible(double in_left, double in_top, double in_right, double in_bottom)
{
	BuildCache();
	std::vector<size_t> result;
	if (GetCount()>0)
	{		
		index_cache_t left(0,0, data_set->GetAbsCoord().FindIndex(std::max<double>(in_left, range_x.left_bound().value), true));
		index_cache_t right(0,0, data_set->GetAbsCoord().FindIndex(std::min<double>(in_right, range_x.right_bound().value), true));
		size_t min_i=std::lower_bound(cache.begin(), cache.end(), left)-cache.begin();
		size_t max_i=std::upper_bound(cache.begin(), cache.end(), right)-cache.begin();

		if (max_i>GetCount()-1)
			max_i=GetCount()-1;
		if (min_i>GetCount()-1)
			min_i=GetCount()-1;
		
		if (min_i<cache.size())
		{
			size_t curve_index=cache.at(min_i).first;
			while (min_i>0 && cache.at(min_i--).first==curve_index){};
		}
		
		if (max_i<cache.size())
		{
			size_t curve_index=cache.at(max_i).first;		
			while (max_i<cache.size()-1 && cache.at(max_i++).first==curve_index){};
		}		
		result.push_back(min_i);
		result.push_back(max_i);
	}
	return result;	
}

//---------------------------------- ������ ������ VCoordPointData ----------------------------------

/** �����������*/
VCoordPointData::VCoordPointData(VDataSetPtr in_data_set, VIBDatabasePtr in_database, POINT_TYPE in_type) : VXOrderedDataProvider()
{
	if (in_data_set)
	{
		data_set=in_data_set;
		point_type=in_type;
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VCoordPointData::GetCount() const
{
	if (data_set)	
		return (data_set->GetCoordPoint().GetCountPoint(point_type));
	else
		throw NULL_POINTER;
}

/** ���������� ���������� �� ��� X*/
double VCoordPointData::GetX(size_t in_index) const
{
	if (data_set)
		return data_set->GetCoordPoint().GetPoint(point_type, in_index).abscoord;
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ���������� �� ��� Y*/
double VCoordPointData::GetY(size_t in_index) const
{
	if (data_set)
		return (data_set->GetCoordPoint().GetPoint(point_type, in_index).abscoord - data_set->GetCoordPoint().GetPoint(point_type, in_index).apr_abscoord);
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ����������� �������� �� X*/
double VCoordPointData::GetMinX() 
{
	if (data_set)
		return data_set->GetAbsCoord().GetItem(0);
	else
	{
		throw NULL_POINTER;
	}
};

/** ���������� ������������ �������� �� X*/
double VCoordPointData::GetMaxX() 
{
	if (data_set)
		return data_set->GetAbsCoord().GetLastItem();
	else
	{
		throw NULL_POINTER;
	}
};	

/**���������� ������������ �������� �� Y*/
double VCoordPointData::GetMaxY()
{
	if(data_set)
		return data_set->GetCoordPoint().GetMax(point_type);
	else
	{
		throw NULL_POINTER;
	}
}

/**���������� ����������� �������� �� Y*/
double VCoordPointData::GetMinY()
{
	if(data_set)
		return data_set->GetCoordPoint().GetMin(point_type);
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ����� */
std::wstring VCoordPointData::GetLabel(std::size_t in_index) const
{
	if(point_type==POINT_SPALA_MARKER || point_type == POINT_POLE && !data_set->GetCoordPoint().GetPoint(POINT_POLE, in_index).isIgnore)
		return L"�";
	else //(point_type == POINT_POLE && data_set->GetCoordPoint().GetPoint(POINT_POLE, in_index).isIgnore)
		return L"�";
}

/** �������� �����*/
void VCoordPointData::SelectPoint(size_t in_index)
{	
	selected_point=in_index;
}

/** ���������� ���������� �����*/
size_t VCoordPointData::GetSelectedPoint() const
{
	return selected_point;
}

/** ������� ���������*/
void VCoordPointData::DeselectPoint()
{	
	selected_point=BAD_INDEX;
}

/* �������� �������
void VCoordPointData::DeleteItem(size_t in_index)
{
	data_set->GetCoordPoint().DeletePoint(in_index);
}

/* ��������� ������� � ������ */
void VCoordPointData::RefreshData()
{
	if(point_type==POINT_POLE)
	{
		data_set->GetCoordPoint().SetCorrupt(*data_set, point_type, selected_point);
		data_set->GetCoordPoint().CalcLine(point_type);
	}
}

//---------------------------------- ������ ������ VLine_tData ----------------------------------
VLine_tData::VLine_tData(VDataSetPtr in_data_set, POINT_TYPE in_type)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		point_type=in_type;
		data_set->GetCoordPoint().CalcLine(in_type);
	}
	else
	{
		throw NULL_POINTER;
	}
}

size_t VLine_tData::GetCount() const
{
	if (data_set)	
		return 2;
	else
		throw NULL_POINTER;
}

double VLine_tData::GetX(size_t in_index) const
{
	if(data_set->GetCoordPoint().GetCountNotCorrupt(point_type) > 1)
	{
		if(!in_index)
			return data_set->GetAbsCoord().GetItem(0);
		else
			return data_set->GetAbsCoord().GetLastItem();
	}
	else if(data_set->GetCoordPoint().GetCountPoint(point_type) > 1)
		return data_set->GetAbsCoord().GetItem(0);
	else
		throw NULL_POINTER;
}

double VLine_tData::GetY(size_t in_index) const
{
	line_t line=data_set->GetCoordPoint().GetLine(point_type);
	if(data_set->GetCoordPoint().GetCountNotCorrupt(point_type) > 1)
	{
		if(!in_index)
			return (line.b() + line.k()*data_set->GetAbsCoord().GetItem(0));
		else
			return (line.b() + line.k()*data_set->GetAbsCoord().GetLastItem());
	}
	else if(data_set->GetCoordPoint().GetCountPoint(point_type) > 1)
		data_set->GetCoordPoint().GetPoint(point_type, 0);
	else
		throw NULL_POINTER;
	return 0;
}

/** ���������� ����������� �������� �� X*/
double VLine_tData::GetMinX() 
{
	if (data_set)
		return data_set->GetAbsCoord().GetItem(0);
	else
	{
		throw NULL_POINTER;
	}
};

/** ���������� ������������ �������� �� X*/
double VLine_tData::GetMaxX() 
{
	if (data_set)
		return data_set->GetAbsCoord().GetLastItem();
	else
	{
		throw NULL_POINTER;
	}
};	

/**���������� ������������ �������� �� Y*/
double VLine_tData::GetMaxY()
{
	line_t line=data_set->GetCoordPoint().GetLine(point_type);
	if(data_set)
	{
		if(line.k() > 0)
			return (line.b() + line.k()*(data_set->GetAbsCoord().GetLastItem()));
		else
			return (line.b() + line.k()*(data_set->GetAbsCoord().GetItem(0)));
	}
	else
	{
		throw NULL_POINTER;
	}
}

/**���������� ����������� �������� �� Y*/
double VLine_tData::GetMinY()
{
	line_t line=data_set->GetCoordPoint().GetLine(point_type);
	if(data_set)
	{
		if(line.k() <= 0)
			return (line.b() + line.k()*(data_set->GetAbsCoord().GetLastItem()));
		else
			return (line.b() + line.k()*(data_set->GetAbsCoord().GetItem(0)));
	}
	else
	{
		throw NULL_POINTER;
	}
}

//--------------------------------------- ������ ������ VCurveParameterData ------------------------------------

/** �����������*/
VCurveParameterData::VCurveParameterData(VDataSetPtr in_data_set, size_t in_curve_index, 
	CURVE_PARAM_TYPE in_type, PARAMETER_TYPE in_mark_type) : VXOrderedDataProvider()
{
	if (in_data_set)
	{
		data_set=in_data_set;
		curve_index=in_curve_index;
		switch (in_type)
		{
			case ANP_HIGH:
				ydata=data_set->GetCurves(in_mark_type).GetItem(curve_index).GetArrayAnpHigh(*data_set);
				break;
			case ANP_HIGH_BT:
				ydata=data_set->GetCurves(in_mark_type).GetItem(curve_index).GetArrayAnpHighBT(*data_set);
				break;
			case ANP_PASS:
				ydata=data_set->GetCurves(in_mark_type).GetItem(curve_index).GetArrayAnpPass(*data_set);
				break;
			case ANP_TRUCK:
				ydata=data_set->GetCurves(in_mark_type).GetItem(curve_index).GetArrayAnpTruck(*data_set);
				break;										
			default:
				throw VSimpleException(L"�������� ������� ��������", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}				
		double abscoord_start=std::min<double>(data_set->GetCurves(in_mark_type).GetItem(curve_index).GetRangeCoord(*data_set, true, false, true), data_set->GetCurves(in_mark_type).GetItem(curve_index).GetRangeCoord(*data_set, false, false, true))-10;
		start_index=data_set->GetAbsCoord().FindIndex(abscoord_start, true);
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ���������� ������ ������*/
size_t VCurveParameterData::GetCount() const
{
	return ydata.size();
}

/** ���������� ���������� �� ��� X*/
double VCurveParameterData::GetX(size_t in_index) const
{
	size_t index=start_index+in_index;
	return data_set->GetAbsCoord().GetItem(index);
}

/** ���������� ���������� �� ��� Y*/
double VCurveParameterData::GetY(size_t in_index) const
{
	return ydata.at(in_index);
}

/** ���������� ����������� �������� �� Y*/
double VCurveParameterData::GetMinY()
{
	return *(std::min_element(ydata.begin(), ydata.end()));
}

/** ���������� ������������ �������� �� Y*/
double VCurveParameterData::GetMaxY()
{
	if (!ydata.empty())
		return *(std::max_element(ydata.begin(), ydata.end()));
	else
		return 0;
}

/** ���������� ����������� �������� �� X*/
double VCurveParameterData::GetMinX()
{
	return GetX(0);
}

/** ���������� ������������ �������� �� X*/
double VCurveParameterData::GetMaxX()
{
	return GetX(GetCount()-1);
}

//----------------------------------------- ������ ������ VMainWayObjectData -------------------------------------

/** �����������*/
VMainWayObjectData::VMainWayObjectData(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, 
	const std::wstring& in_query, VDataSetPtr in_data_set)  : VXOrderedDataProvider(), database(in_database), 
	transaction(in_transaction)
{
	if (in_data_set)
	{
		query=in_query;
		data_set=in_data_set;
		data_set->Load(PRM_PROFILE);
		RefreshData();
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ��������� ������*/
void VMainWayObjectData::RefreshData()
{		
	database->ExecuteStatement(*transaction, wstring_to_string(query).c_str(), handler);
}

/** ���������� ������ ������*/
size_t VMainWayObjectData::GetCount() const
{
	return handler.GetRowCount();
}

/** ���������� ���������� �� ��� X*/
double VMainWayObjectData::GetX(size_t in_index) const
{
	return handler.GetParameter(in_index, 0).GetFloat();
}

/** ���������� ������ �� ��� X*/
double VMainWayObjectData::GetSizeX(size_t in_index) const
{
	return handler.GetParameter(in_index, 7).GetFloat();
}

/** ���������� ���������� �� ��� Y*/
double VMainWayObjectData::GetY(size_t in_index) const
{
	size_t index=data_set->GetAbsCoord().FindIndex(GetX(in_index), true);
	return data_set->GetParameter(PRM_PROFILE).GetItem(index);
}

/** ���������� ����������� �������� �� Y*/
double VMainWayObjectData::GetMinY()
{
	return data_set->GetParameter(PRM_PROFILE).GetMin();
}

/** ���������� ������������ �������� �� Y*/
double VMainWayObjectData::GetMaxY()
{
	return data_set->GetParameter(PRM_PROFILE).GetMax();
}

/** ���������� ����������� �������� �� X*/
double VMainWayObjectData::GetMinX()
{
	return GetCount()>0 ? GetX(0) : 0;
}

/** ���������� ������������ �������� �� X*/
double VMainWayObjectData::GetMaxX()
{
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
}

/** ���������� �����*/
std::wstring VMainWayObjectData::GetLabel(size_t in_index) const
{	
	return handler.GetParameter(in_index,4).GetString();
}

/** ���������� ����� (ID �����)*/
uint64_t VMainWayObjectData::GetFlags(size_t in_index) const
{
	uint32_t type=handler.GetParameter(in_index,5).GetInteger();
	int32_t side=handler.GetParameter(in_index, 6).GetInteger();	
	return compose_flags(side, type);
}



//------------------------------------------ ��� ����������� ����� ----------------------------------------------

//---------------------------- ������ ������ VStationProfileData ------------------------------------------------



/** �����������*/
VStationProfileData::VStationProfileData(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, 
	VDataSetPtr in_data_set)
{
	in_data_set->Load(PRM_PROFILE);
	for (size_t i=0; i<in_data_set->GetParameter(PRM_PROFILE).GetCount(); ++i)
	{
		data.push_back(point_t(in_data_set->GetAbsCoord().GetItem(i), in_data_set->GetParameter(PRM_PROFILE).GetItem(i)));
	}
	//FixMe!!! ����� ����� ��������� �� �� ���. ����� � �������� �� � ������.
	std::wstring query=boost::str(boost::wformat(L"select c.ABSCOORD, a.HEIGHT, a.KM, a.M "
		L"from STW_POINT_TO_OBJECT a "
		L"inner join STW_WAY d on d.ID=a.STW_WAY_ID "
		L"inner join STW_TRAIL t on t.STW_WAY_ID=d.ID "
		L"left join GET_STW_ABSCOORD(a.KM, a.M) c on 1=1 "
		L"where t.FILENAME='%s' "
		L"and a.ENU_POINT_OBJECT_TYPE_ID=31 "
		L"and a.KM is not null "
		L"and a.M is not null "
		L"and a.height is not null") % in_data_set->GetFileName().GetShortName());
	VIBDataHandlerParameter handler;
	in_database->ExecuteStatement(*in_transaction, wstring_to_string(query).c_str(), handler);
	for (size_t i=0; i<handler.GetRowCount(); ++i)
	{
		double abs_coord=in_data_set->GetAprinfo().GetAbsCoord(VWayCoord(handler.GetParameter(0,2).GetFloat(), handler.GetParameter(0,3).GetFloat()));
		data.push_back(point_t(abs_coord, handler.GetParameter(0,1).GetFloat()));//handler.GetParameter(0,0).GetFloat()
	}
	std::sort(data.begin(), data.end(), point_t::less_x());
}

/** ���������� ������ ������*/
size_t VStationProfileData::GetCount() const
{
	return data.size();
}

/** ���������� ���������� �� ��� X*/
double VStationProfileData::GetX(size_t in_index) const
{
	return data.at(in_index).x;
}

/** ���������� ���������� �� ��� Y*/
double VStationProfileData::GetY(size_t in_index) const
{
	return data.at(in_index).y;
}

/** ���������� ����������� �������� �� Y*/
double VStationProfileData::GetMinY()
{
	return data.empty() ? 0 : std::max_element(data.begin(), data.end(), point_t::less_y())->y;
}

/** ���������� ������������ �������� �� Y*/
double VStationProfileData::GetMaxY()
{
	return data.empty() ? 0 : std::min_element(data.begin(), data.end(), point_t::less_y())->y;
}

/** ���������� ����������� �������� �� X*/
double VStationProfileData::GetMinX()
{
	return data.empty() ? 0 : data.front().x;
}

/** ���������� ������������ �������� �� X*/
double VStationProfileData::GetMaxX()
{
	return data.empty() ? 0 : data.back().x;
}

//--------------------------------------- ������ ������ VStationOperatorMarkData ----------------------------------

/** �����������*/
VStationOperatorMarkData::VStationOperatorMarkData(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction,
	VDataSetPtr in_data_set, const std::wstring& in_trail_id)  : VXOrderedDataProvider(), database(in_database), 
	transaction(in_transaction), trail_id(in_trail_id) 
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_set->Load(PRM_PROFILE);		
		RefreshData();
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ��������� ������*/
void VStationOperatorMarkData::RefreshData()
{
	std::wstring query(
		L"select m.id, t.number, abscoord as X, 0 as Y, (select small from enu_opmark_type where id=m.ENU_OPMARK_TYPE_ID) "		
		L"from STW_OPMARK m "
		L"inner join enu_opmark_type t on t.ID=m.ENU_OPMARK_TYPE_ID "
		L"where STW_TRAIL_ID=%TRAIL_ID% order by abscoord");
	algorithm::ireplace_all(query, L"%TRAIL_ID%", trail_id);
	database->ExecuteStatement(*transaction, wstring_to_string(query).c_str(), handler);
}

/** ���������� ������ ������*/
size_t VStationOperatorMarkData::GetCount() const
{
	return handler.GetRowCount();
}

/** ���������� ���������� �� ��� X*/
double VStationOperatorMarkData::GetX(size_t in_index) const
{
	return handler.GetParameter(in_index, 2).GetFloat();
}

/** ���������� ���������� �� ��� Y*/
double VStationOperatorMarkData::GetY(size_t in_index) const
{
	size_t index=data_set->GetAbsCoord().FindIndex(GetX(in_index), true);
	if (index!=BAD_INDEX)
		return data_set->GetParameter(PRM_PROFILE).GetItem(index);
	else
		return handler.GetParameter(in_index, 3).GetFloat();
}

/** ���������� ����������� �������� �� Y*/
double VStationOperatorMarkData::GetMinY()
{
	return DBL_MAX;
}

/** ���������� ������������ �������� �� Y*/
double VStationOperatorMarkData::GetMaxY()
{
	return 0-DBL_MAX;
}

/** ���������� ����������� �������� �� X*/
double VStationOperatorMarkData::GetMinX()
{
	return 0;
}

/** ���������� ������������ �������� �� X*/
double VStationOperatorMarkData::GetMaxX()
{
	return 0;
}

/** ���������� �����*/
std::wstring VStationOperatorMarkData::GetLabel(size_t in_index) const
{
	return handler.GetParameter(in_index, 4).GetString();
}

/** ���������� ����� (ID �����)*/
uint64_t VStationOperatorMarkData::GetFlags(size_t in_index) const
{
	return compose_flags(handler.GetParameter(in_index, 0).GetInteger(), 
		handler.GetParameter(in_index, 1).GetInteger());
}

/** ���������� true ���� ����� ����� ������. �� ��������� ����� ������ ����� �����*/
bool VStationOperatorMarkData::CanChange(size_t in_index) const 
{
	return hi_word(GetFlags(in_index))==100;
}

/** ��������� �����*/
void VStationOperatorMarkData::AddXY(size_t in_index, double in_x, double in_y, double in_size_x, double in_size_y,
	const std::wstring& in_label, uint64_t in_flags)
{	
	std::wstring query=boost::str(boost::wformat(
	L"insert into STW_OPMARK (ENU_OPMARK_TYPE_ID, STW_TRAIL_ID, ABSCOORD, HEIGHT) values ((select id from enu_opmark_type where number=100), %s, %f, 0)") % trail_id % in_x);	
	database->ExecuteStatement(*transaction, wstring_to_string(query).c_str(), handler);
	//��������� ������
	RefreshData();
}

/** �������� �����*/
void VStationOperatorMarkData::SetXY(size_t in_index, double in_x, double in_y)
{
	//����������� ������ ������ ���������������� �����
	if (CanChange(in_index))
	{
		std::wstring query=boost::str(boost::wformat(
		L"update STW_OPMARK set ABSCOORD=%g where id=%d") %in_x %low_word(GetFlags(in_index)));	
		database->ExecuteStatement(*transaction, wstring_to_string(query).c_str(), handler);
		//��������� ������
		RefreshData();
		//�������� ���������� ��������� ������
		if (on_data_changed && on_data_changed->Allowed())
			on_data_changed->Execute();
	}
}

/** ������� �����*/	
void VStationOperatorMarkData::DeleteItem(size_t in_index)
{
	//����������� ������ ������ ���������������� �����
	if (CanChange(in_index))
	{
		std::wstring query=boost::str(boost::wformat(
		L"delete from STW_OPMARK where id=%d") % low_word(GetFlags(in_index)));
		database->ExecuteStatement(*transaction, wstring_to_string(query).c_str(), handler);
		//��������� ������
		RefreshData();
		//�������� ���������� ��������� ������
		if (on_data_changed && on_data_changed->Allowed())
			on_data_changed->Execute();
	}
}

/** �������� �����*/
void VStationOperatorMarkData::SelectPoint(size_t in_index)
{
	selected_point=in_index;
}

/** ���������� ���������� �����*/
size_t VStationOperatorMarkData::GetSelectedPoint() const
{
	return selected_point;
}

/** ������� ���������*/
void VStationOperatorMarkData::DeselectPoint()
{	
	selected_point=BAD_INDEX;
}

//----------------------------------------- ������ ������ VStationWayObjectData -------------------------------------

/** �����������*/
VStationWayObjectData::VStationWayObjectData(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, 
	VDataSetPtr in_data_set, bool in_show_all)  : VXOrderedDataProvider(), database(in_database), transaction(in_transaction)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		show_all=in_show_all;
		data_set->Load(PRM_PROFILE);
		trail_id=boost::str(boost::wformat(L"select id from STW_TRAIL where filename='%s'") % 
			data_set->GetFileName().GetShortName());

		RefreshData();
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ��������� ������*/
void VStationWayObjectData::RefreshData()
{	
	std::wstring query;
	if (show_all)//���������� ��� �������, ������� ������������ ������ � ��������������� �������
		query=
		L"SELECT b.ABSCOORD, b.height, 0, 0, case when (e.number in (33,34,35,36)) then e.stype else d.name end, e.number, c.ENU_SIDE_ID, grc_p.pk, grc_p.m "//d.NAME
		L"FROM STW_OPMARK_TO_POINT a "
		L"inner join STW_OPMARK b on b.ID=a.STW_OPMARK_ID "
		L"inner join STW_POINT_TO_OBJECT c on c.ID=a.STW_POINT_TO_OBJECT_ID "
		L"inner join STW_OBJECT d on d.ID=c.STW_OBJECT_ID "
		L"inner join ENU_POINT_OBJECT_TYPE e on e.ID=c.ENU_POINT_OBJECT_TYPE_ID "
		L"inner join stw_way sw on c.stw_way_id = sw.id "		
		L"left join get_realpiket_piketazh(b.stw_trail_id, b.abscoord) grc_p on 1=1 "
		L"where b.STW_TRAIL_ID=(%TRAIL_ID%) "
		L"order by b.abscoord";
	else//���������� ������� � ����������� � ��� � ��������� ������� �� ��������� ����
		
		//FixMe!!! ��� ���� ����� ���������� ��������������� ������� ������ "e.number<100" �������� "NOT (e.number=100 OR e.number=101)"
		query=
		L"SELECT b.ABSCOORD, b.height, 0, 0, case when (e.number in (33,34,35,36)) then e.stype else d.name end, e.number, c.ENU_SIDE_ID, grc_p.pk, grc_p.m " //d.NAME
		L"FROM STW_OPMARK_TO_POINT a "
		L"inner join STW_OPMARK b on b.ID=a.STW_OPMARK_ID "
		L"inner join STW_POINT_TO_OBJECT c on c.ID=a.STW_POINT_TO_OBJECT_ID "
		L"inner join STW_OBJECT d on d.ID=c.STW_OBJECT_ID "
		L"inner join ENU_POINT_OBJECT_TYPE e on e.ID=c.ENU_POINT_OBJECT_TYPE_ID "
		L"inner join stw_way sw on c.stw_way_id = sw.id "		
		L"left join get_realpiket_piketazh(b.stw_trail_id, b.abscoord) grc_p on 1=1 "
		L"where b.STW_TRAIL_ID=(%TRAIL_ID%) and e.number<100 and "

		L"(b.abscoord >= coalesce((SELECT b1.ABSCOORD  "
		L"FROM STW_OPMARK_TO_POINT a1  "
		L"inner join STW_OPMARK b1 on b1.ID=a1.STW_OPMARK_ID  "
		L"inner join STW_POINT_TO_OBJECT c1 on c1.ID=a1.STW_POINT_TO_OBJECT_ID  "
		L"inner join STW_FUNCTION_TO_POINT f1 on f1.STW_POINT_TO_OBJECT_ID=c1.ID   "                
		L"where b1.STW_TRAIL_ID=b.STW_TRAIL_ID and f1.ENU_POINT_TYPE_ID = 1), b.abscoord)  and "

		L"b.abscoord <= coalesce((SELECT b1.ABSCOORD  "
		L"FROM STW_OPMARK_TO_POINT a1  "
		L"inner join STW_OPMARK b1 on b1.ID=a1.STW_OPMARK_ID  "
		L"inner join STW_POINT_TO_OBJECT c1 on c1.ID=a1.STW_POINT_TO_OBJECT_ID  "
		L"inner join STW_FUNCTION_TO_POINT f1 on f1.STW_POINT_TO_OBJECT_ID=c1.ID   "                
		L"where b1.STW_TRAIL_ID=b.STW_TRAIL_ID and f1.ENU_POINT_TYPE_ID = 2), b.abscoord) or "

		L"b.abscoord <= coalesce((SELECT b1.ABSCOORD  "
		L"FROM STW_OPMARK_TO_POINT a1  "
		L"inner join STW_OPMARK b1 on b1.ID=a1.STW_OPMARK_ID  "
		L"inner join STW_POINT_TO_OBJECT c1 on c1.ID=a1.STW_POINT_TO_OBJECT_ID  "
		L"inner join STW_FUNCTION_TO_POINT f1 on f1.STW_POINT_TO_OBJECT_ID=c1.ID   "                
		L"where b1.STW_TRAIL_ID=b.STW_TRAIL_ID and f1.ENU_POINT_TYPE_ID = 1), b.abscoord)  and "

		L"b.abscoord >= coalesce((SELECT b1.ABSCOORD  "
		L"FROM STW_OPMARK_TO_POINT a1  "
		L"inner join STW_OPMARK b1 on b1.ID=a1.STW_OPMARK_ID  "
		L"inner join STW_POINT_TO_OBJECT c1 on c1.ID=a1.STW_POINT_TO_OBJECT_ID  "
		L"inner join STW_FUNCTION_TO_POINT f1 on f1.STW_POINT_TO_OBJECT_ID=c1.ID   "                
		L"where b1.STW_TRAIL_ID=b.STW_TRAIL_ID and f1.ENU_POINT_TYPE_ID = 2), b.abscoord)) "

		L"order by b.abscoord";
	algorithm::ireplace_all(query, L"%TRAIL_ID%", trail_id);
	database->ExecuteStatement(*transaction, wstring_to_string(query).c_str(), handler);
}

/** ���������� ������ ������*/
size_t VStationWayObjectData::GetCount() const
{
	return handler.GetRowCount();
}
	
/** ���������� ���������� �� ��� X*/
double VStationWayObjectData::GetX(size_t in_index) const
{
	return handler.GetParameter(in_index, 0).GetFloat();
}

/** ���������� ������ �� ��� X*/
double VStationWayObjectData::GetSizeX(size_t in_index) const
{
	//FixMe!!! �������� ���� � ��
	return 0;
}

/** ���������� ���������� �� ��� Y*/
double VStationWayObjectData::GetY(size_t in_index) const
{
	size_t index=data_set->GetAbsCoord().FindIndex(GetX(in_index), true);
	if (index!=BAD_INDEX)
		return data_set->GetParameter(PRM_PROFILE).GetItem(index);
	else
		return handler.GetParameter(in_index, 2).GetFloat();
}

/** ���������� ����������� �������� �� Y*/
double VStationWayObjectData::GetMinY()
{
	return data_set->GetParameter(PRM_PROFILE).GetMin();
}

/** ���������� ������������ �������� �� Y*/
double VStationWayObjectData::GetMaxY()
{
	return data_set->GetParameter(PRM_PROFILE).GetMax();
}

/** ���������� ����������� �������� �� X*/
double VStationWayObjectData::GetMinX()
{
	return data_set->GetAbsCoord().GetItem(0);
}

/** ���������� ������������ �������� �� X*/
double VStationWayObjectData::GetMaxX()
{
	return data_set->GetAbsCoord().GetLastItem();
}

/** ���������� �����*/
std::wstring VStationWayObjectData::GetLabel(size_t in_index) const
{		
	double pk=handler.GetParameter(in_index,7).GetFloat();
	double m=handler.GetParameter(in_index,8).GetFloat();
	return boost::str(boost::wformat(L"%s;%0.0f+%0.2f") % handler.GetParameter(in_index,4).GetString() % pk % m);	
}

/** ���������� ����� (ID �����)*/
uint64_t VStationWayObjectData::GetFlags(size_t in_index) const
{
	uint32_t type=handler.GetParameter(in_index,5).GetInteger();
	int32_t side=handler.GetParameter(in_index, 6).GetInteger();	
	return compose_flags(side, type);
}


//----------------------------------------- ������ ������ VStationWayKmData -------------------------------------

/** �����������*/
VStationWayKmData::VStationWayKmData(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, 
	VDataSetPtr in_data_set)  : VXOrderedDataProvider(), database(in_database), transaction(in_transaction)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_set->Load(PRM_PROFILE);
		trail_id=boost::str(boost::wformat(L"select id from STW_TRAIL where filename='%s'") % 
			data_set->GetFileName().GetShortName());

		RefreshData();
	}
	else
	{
		throw NULL_POINTER;
	}
}

/** ��������� ������*/
void VStationWayKmData::RefreshData()
{	
	std::wstring query(L"SELECT b.ABSCOORD, b.height, 0, 0, d.NAME, e.number, c.ENU_SIDE_ID FROM STW_OPMARK_TO_POINT a inner join STW_OPMARK b on b.ID=a.STW_OPMARK_ID inner join STW_POINT_TO_OBJECT c on c.ID=a.STW_POINT_TO_OBJECT_ID inner join STW_OBJECT d on d.ID=c.STW_OBJECT_ID inner join ENU_POINT_OBJECT_TYPE e on e.ID=c.ENU_POINT_OBJECT_TYPE_ID where b.STW_TRAIL_ID=(%TRAIL_ID%) and e.number=101 order by b.abscoord");
	algorithm::ireplace_all(query, L"%TRAIL_ID%", trail_id);
	database->ExecuteStatement(*transaction, wstring_to_string(query).c_str(), handler);	
}

/** ���������� ������ ������*/
size_t VStationWayKmData::GetCount() const
{
	return handler.GetRowCount();
}

/** ���������� ���������� �� ��� X*/
double VStationWayKmData::GetX(size_t in_index) const
{
	return handler.GetParameter(in_index, 0).GetFloat();
}

/** ���������� ���������� �� ��� Y*/
double VStationWayKmData::GetY(size_t in_index) const
{
	size_t index=data_set->GetAbsCoord().FindIndex(GetX(in_index), true);
	if (index!=BAD_INDEX)
		return data_set->GetParameter(PRM_PROFILE).GetItem(index);
	else
		return handler.GetParameter(in_index, 2).GetFloat();
}

/** ���������� ����������� �������� �� Y*/
double VStationWayKmData::GetMinY()
{
	return data_set->GetParameter(PRM_PROFILE).GetMin();
}

/** ���������� ������������ �������� �� Y*/
double VStationWayKmData::GetMaxY()
{
	return data_set->GetParameter(PRM_PROFILE).GetMax();
}

/** ���������� ����������� �������� �� X*/
double VStationWayKmData::GetMinX()
{
	return 0;
}

/** ���������� ������������ �������� �� X*/
double VStationWayKmData::GetMaxX()
{
	return 0;
}

/** ���������� �����*/
std::wstring VStationWayKmData::GetLabel(size_t in_index) const
{		
	return handler.GetParameter(in_index,4).GetString();
}

/** ���������� ����� (ID �����)*/
uint64_t VStationWayKmData::GetFlags(size_t in_index) const
{
	uint32_t type=handler.GetParameter(in_index,5).GetInteger();
	int32_t side=handler.GetParameter(in_index, 6).GetInteger();	
	return compose_flags(side, type);
}

//-------------------------------------- ������ ������ VPredefinedQueries -----------------------------

///�����������
VPredefinedQueries::VPredefinedQueries()
{
	//�������������� ������
	zero =L"";
		
	//����������				
	queries.insert(query_t(L"COORDINATES",
		L"execute block returns (x float, y float, label varchar(32), flags integer) "
		L"as "
		L"	declare variable start_abs float; "
		L"	declare variable end_abs float; "
		L"	declare variable km integer; "
		L"	declare variable pk integer; "
		L"begin "
		L"	for select km, start_abs, end_abs from GET_KM_ABS(%WAY_ID%, '%TRAIL_DATE%') into :km, :start_abs, :end_abs do "
		L"	begin "
		L"		if (start_abs < end_abs) then "
		L"		begin "
		L"			x=start_abs; "
		L"			y=0; "
		L"			label=cast(km as varchar(32)); "
		L"			flags=1; "
		L"			suspend; "
		L"			pk=0; "
		L"			while (x < end_abs) do "
		L"			begin "
		L"				x=x+100; "
        L"				if (abs(x-end_abs)<50) then "
        L"				begin "
        L"					x=end_abs; "
        L"				end "
		L"				pk=pk+1; "
		L"				label=cast(pk as varchar(32)); "
		L"				flags=2; "
		L"				suspend; "
		L"			end "
		L"		end "
		L"	end "
		L"end"));
		

	//�������� �� ������� (����.)				
	queries.insert(query_t(L"SPEEDS_PASS",
		L"select ec.abscoord as x, speed1 as y, 0 as size_x, 0 as size_y, '' as label, 0 as flags, 1 "
		L"from APR_SPEED "
		L"inner join APR_SPEED_PERIOD on apr_speed.APR_SPEED_PERIOD_ID=APR_SPEED_PERIOD.id "			
		L"left join get_abscoord(%WAY_ID%, apr_speed.end_km, apr_speed.end_m, '%TRAIL_DATE%') ec on 1=1 "
		L"where APR_SPEED_PERIOD.ADM_WAY_ID=%WAY_ID% and APR_SPEED_PERIOD.start_date < '%TRAIL_DATE%' and APR_SPEED_PERIOD.end_date > '%TRAIL_DATE%' "						
		L"union "
		L"select sc.abscoord as x, speed1 as y, 0 as size_x, 0 as size_y, '' as label, 0 as flags, 2 "
		L"from APR_SPEED "
		L"inner join APR_SPEED_PERIOD on apr_speed.APR_SPEED_PERIOD_ID=APR_SPEED_PERIOD.id "
		L"left join get_abscoord(%WAY_ID%, apr_speed.start_km, apr_speed.start_m, '%TRAIL_DATE%') sc on 1=1 "			
		L"where APR_SPEED_PERIOD.ADM_WAY_ID=%WAY_ID% and APR_SPEED_PERIOD.start_date < '%TRAIL_DATE%' and APR_SPEED_PERIOD.end_date > '%TRAIL_DATE%' "
		L"order by 1, 7"
		));
	

	//�������� �� ������� (����.)		
	queries.insert(query_t(L"SPEEDS_TRUCK",
		L"select ec.abscoord as x, speed2 as y, 0 as size_x, 0 as size_y, '' as label, 0 as flags, 1 "
		L"from APR_SPEED "
		L"inner join APR_SPEED_PERIOD on apr_speed.APR_SPEED_PERIOD_ID=APR_SPEED_PERIOD.id "			
		L"left join get_abscoord(%WAY_ID%, apr_speed.end_km, apr_speed.end_m, '%TRAIL_DATE%') ec on 1=1 "
		L"where APR_SPEED_PERIOD.ADM_WAY_ID=%WAY_ID% and APR_SPEED_PERIOD.start_date < '%TRAIL_DATE%' and APR_SPEED_PERIOD.end_date > '%TRAIL_DATE%' "						
		L"union "
		L"select sc.abscoord as x, speed2 as y, 0 as size_x, 0 as size_y, '' as label, 0 as flags, 2 "
		L"from APR_SPEED "
		L"inner join APR_SPEED_PERIOD on apr_speed.APR_SPEED_PERIOD_ID=APR_SPEED_PERIOD.id "
		L"left join get_abscoord(%WAY_ID%, apr_speed.start_km, apr_speed.start_m, '%TRAIL_DATE%') sc on 1=1 "			
		L"where APR_SPEED_PERIOD.ADM_WAY_ID=%WAY_ID% and APR_SPEED_PERIOD.start_date < '%TRAIL_DATE%' and APR_SPEED_PERIOD.end_date > '%TRAIL_DATE%' "
		L"order by 1, 7"
		));

	//�������� �� ������� (���)
	queries.insert(query_t(L"SPEEDS_SIMPLE",
		L"select sc.abscoord as x, 0 as y, ec.abscoord-sc.abscoord as size_x, 0 as size_y, cast(speed1 as varchar(8)) || '/' || cast(speed2 as varchar(8)) as label, 0 as flags "
		L"from APR_SPEED "
		L"inner join APR_SPEED_PERIOD on apr_speed.APR_SPEED_PERIOD_ID=APR_SPEED_PERIOD.id "
		L"left join get_abscoord(%WAY_ID%, apr_speed.start_km, apr_speed.start_m, '%TRAIL_DATE%') sc on 1=1 "
		L"left join get_abscoord(%WAY_ID%, apr_speed.end_km, apr_speed.end_m, '%TRAIL_DATE%') ec on 1=1 "
		L"where APR_SPEED_PERIOD.ADM_WAY_ID=%WAY_ID% and APR_SPEED_PERIOD.start_date < '%TRAIL_DATE%' and APR_SPEED_PERIOD.end_date > '%TRAIL_DATE%' order by sc.abscoord"));
		
	//�������� �� ������� (���������� ��������)
	queries.insert(query_t(L"HIGH_SPEEDS_SIMPLE",
		L"select sc.abscoord as x, 0 as y, ec.abscoord-sc.abscoord as size_x, 0 as size_y, cast(speed as varchar(8)) as label, 0 as flags "
		L"from APR_SPEED_HS s "
		L"inner join APR_SPEED_HS_PERIOD p on s.APR_SPEED_HS_PERIOD_ID=p.id "
		L"left join get_abscoord(%WAY_ID%, s.start_km, s.start_m, '%TRAIL_DATE%') sc on 1=1 "
		L"left join get_abscoord(%WAY_ID%, s.end_km, s.end_m, '%TRAIL_DATE%') ec on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.start_date < '%TRAIL_DATE%' and p.end_date > '%TRAIL_DATE%' order by sc.abscoord"));
	
	//�������� �� ������� (���������� �������� � �������� ������)
	queries.insert(query_t(L"HIGH_SPEEDS_BT_SIMPLE",
		L"select sc.abscoord as x, 0 as y, ec.abscoord-sc.abscoord as size_x, 0 as size_y, cast(speed as varchar(8)) as label, 0 as flags "
		L"from APR_SPEED_HS_BT s "
		L"inner join APR_SPEED_HS_PERIOD p on s.APR_SPEED_HS_PERIOD_ID=p.id "
		L"left join get_abscoord(%WAY_ID%, s.start_km, s.start_m, '%TRAIL_DATE%') sc on 1=1 "
		L"left join get_abscoord(%WAY_ID%, s.end_km, s.end_m, '%TRAIL_DATE%') ec on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.start_date < '%TRAIL_DATE%' and p.end_date > '%TRAIL_DATE%' order by sc.abscoord"));

	
	//������� ���������
	queries.insert(query_t(L"DISTANTION_SECTIONS",
		L"select sc.abscoord as x, 0 as y, ec.abscoord-sc.abscoord as size_x, 0 as size_y, d.name as label, 0 as flags "
		L"from TPL_DIST_SECTION ds "
		L"inner join TPL_DIST_SECTION_PERIOD dsp on dsp.id=ds.TPL_DIST_SECTION_PERIOD_ID "
		L"left join GET_ABSCOORD(dsp.ADM_WAY_ID, ds.start_km, ds.start_m, '%TRAIL_DATE%') sc on 1=1 "
		L"left join GET_ABSCOORD(dsp.ADM_WAY_ID, ds.end_km, ds.end_m, '%TRAIL_DATE%') ec on 1=1 "
		L"inner join ADM_DISTANTION d on d.ID=ds.ADM_DISTANTION_ID where dsp.ADM_WAY_ID=%WAY_ID% "
		L"order by sc.abscoord"));

	//�������
	queries.insert(query_t(L"STATION_SECTIONS",
		L"select sc.abscoord as x, 0 as y, ec.abscoord-sc.abscoord as size_x, 0 as size_y, s.name as label, 0 as flags "
		L"from TPL_STATION_SECTION ss "
		L"inner join TPL_STATION_SECTION_PERIOD ssp on ssp.id=ss.TPL_STATION_SECTION_PERIOD_ID "
		L"left join GET_ABSCOORD(ssp.ADM_WAY_ID, ss.start_km, ss.start_m, '%TRAIL_DATE%') sc on 1=1 "
		L"left join GET_ABSCOORD(ssp.ADM_WAY_ID, ss.end_km, ss.end_m, '%TRAIL_DATE%') ec on 1=1 "
		L"inner join ADM_STATION s on s.ID=ss.ADM_STATION_ID where ssp.ADM_WAY_ID=%WAY_ID% "
		L"order by sc.abscoord"));		

	//�����
	queries.insert(query_t(L"BRIDGE_SECTIONS",
		L"select ac.abscoord-brlength/2 as x, 0 as y, brlength as size_x, 0 as size_y, '�' as label, 0 as flags "
		L"from BLD_BRIDGE_SECTION bs "
		L"inner join BLD_BRIDGE_SECTION_PERIOD bsp on bsp.id=bs.BLD_BRIDGE_SECTION_PERIOD_ID "
		L"left join GET_ABSCOORD(bsp.ADM_WAY_ID, bs.axis_km, bs.axis_m, '%TRAIL_DATE%') ac on 1=1 "
		L"where bsp.ADM_WAY_ID=%WAY_ID% "
		L"order by ac.abscoord"));
		
	//�������
	queries.insert(query_t(L"TUNNEL_SECTIONS",
		L"select s.abscoord as x, 0 as y, (e.abscoord-s.abscoord) as size_x, 0 as size_y, '�' as label, 0 as flags "
		L"from BLD_TUNNEL_SECTION ts "
		L"inner join BLD_TUNNEL_SECTION_PERIOD tsp on tsp.id=ts.BLD_TUNNEL_SECTION_PERIOD_ID "
		L"left join GET_ABSCOORD(tsp.ADM_WAY_ID, ts.start_km, ts.start_m, '%TRAIL_DATE%') s on 1=1 "
		L"left join GET_ABSCOORD(tsp.ADM_WAY_ID, ts.end_km, ts.end_m, '%TRAIL_DATE%') e on 1=1 "
		L"where tsp.ADM_WAY_ID=%WAY_ID% "
		L"order by s.abscoord"));			

	//�������
	queries.insert(query_t(L"SWITCHES",
		L"select ac.abscoord as x, 0 as y, 30 as size_x, 0 as size_y, s.num as label, 0 as flags "
		L"from TPL_SWITCH s "
		L"inner join TPL_SWITCH_PERIOD sp on sp.id=s.TPL_SWITCH_PERIOD_ID "
		L"left join GET_ABSCOORD(sp.ADM_WAY_ID, s.km, s.m, '%TRAIL_DATE%') ac on 1=1 "
		L"where sp.ADM_WAY_ID=%WAY_ID% "
		L"order by ac.abscoord"));
		
	//������������� �������
	queries.insert(query_t(L"CHAMFERED_JOINTS",
		L"select ac.abscoord as x, 0 as y, 0 as size_x, 0 as size_y, '��.�.' as label, 0 as flags "
		L"from APR_CHAM_JOINT s "
		L"inner join APR_CHAM_JOINT_PERIOD sp on sp.id=s.APR_CHAM_JOINT_PERIOD_ID "
		L"left join GET_ABSCOORD(sp.ADM_WAY_ID, s.km, s.m, '%TRAIL_DATE%') ac on 1=1 "
		L"where sp.ADM_WAY_ID=%WAY_ID% "
		L"order by ac.abscoord"));
		
	//��������
	queries.insert(query_t(L"CROSSINGS",
		L"select ac.abscoord as x, 0 as y, 30 as size_x, 0 as size_y, name as label, 0 as flags "
		L"from APR_CROSSINGS c "
		L"inner join APR_CROSSINGS_PERIOD cp on cp.id=c.APR_CROSSINGS_PERIOD_ID "
		L"left join GET_ABSCOORD(cp.ADM_WAY_ID, c.km, c.m, '%TRAIL_DATE%') ac on 1=1 "
		L"where cp.ADM_WAY_ID=%WAY_ID% "
		L"order by ac.abscoord"));
	

	//����� ��
	queries.insert(query_t(L"NORMA_WIDTH",
		L"select sc.abscoord as x, norma as y, ec.abscoord-sc.abscoord as size_x, -norma as size_y, cast(norma as varchar(8)) as label, 0 as flags "
		L"from APR_NORMA_WIDTH l "
		L"inner join ENU_NORMA_WIDTH n on n.id=l.ENU_NORMA_WIDTH_ID inner join APR_NORMA_WIDTH_PERIOD p on l.APR_NORMA_WIDTH_period_id=p.id "
		L"left join get_abscoord(%WAY_ID%, l.start_km, l.start_m, '%TRAIL_DATE%') sc on 1=1 "
		L"left join get_abscoord(%WAY_ID%, l.end_km, l.end_m, '%TRAIL_DATE%') ec on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.start_date < '%TRAIL_DATE%' and '%TRAIL_DATE%' < p.end_date order by sc.abscoord"));

	//����� ����������
	queries.insert(query_t(L"NORMA_LEVEL",
		L"select sc.abscoord as x, norma as y, ec.abscoord-sc.abscoord as size_x, -norma as size_y, cast(norma as varchar(8)) as label, 0 as flags "
		L"from APR_NORMA_LEVEL l "
		L"inner join ENU_NORMA_LEVEL n on n.id=l.ENU_NORMA_LEVEL_ID inner join APR_NORMA_LEVEL_PERIOD p on l.APR_NORMA_LEVEL_period_id=p.id "
		L"left join get_abscoord(%WAY_ID%, l.start_km, l.start_m, '%TRAIL_DATE%') sc on 1=1 "
		L"left join get_abscoord(%WAY_ID%, l.end_km, l.end_m, '%TRAIL_DATE%') ec on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.start_date < '%TRAIL_DATE%' and p.end_date > '%TRAIL_DATE%' order by sc.abscoord"));

	//��� ����
	queries.insert(query_t(L"CROSSTIE_TYPE",
		L"select sc.abscoord as x, 0 as y, ec.abscoord-sc.abscoord as size_x, 0 as size_y, name as label, 0 as flags "
		L"from APR_CROSSTIE l "
		L"inner join ENU_CROSSTIE_TYPE n on n.id=l.ENU_CROSSTIE_TYPE_ID inner join APR_CROSSTIE_PERIOD p on l.APR_CROSSTIE_PERIOD_ID=p.id "			
		L"left join get_abscoord(%WAY_ID%, l.start_km, l.start_m, '%TRAIL_DATE%') sc on 1=1 "
		L"left join get_abscoord(%WAY_ID%, l.end_km, l.end_m, '%TRAIL_DATE%') ec on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.start_date < '%TRAIL_DATE%' and '%TRAIL_DATE%' < p.end_date order by sc.abscoord"));
	
	//��� �������
	queries.insert(query_t(L"RAIL_TYPE",
		L"select sc.abscoord as x, 0 as y, ec.abscoord-sc.abscoord as size_x, 0 as size_y, type_of_rail as label, 0 as flags "
		L"from APR_RAILS_SECTIONS l "
		L"inner join ENU_RAILS_TYPE n on n.id=l.RAILS_TYPE_ID inner join APR_RAILS_SECTIONS_PERIOD p on l.APR_RAILS_SECTIONS_PERIOD_ID=p.id "			
		L"left join get_abscoord(%WAY_ID%, l.start_km, l.start_m, '%TRAIL_DATE%') sc on 1=1 "
		L"left join get_abscoord(%WAY_ID%, l.end_km, l.end_m, '%TRAIL_DATE%') ec on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.start_date < '%TRAIL_DATE%' and '%TRAIL_DATE%' < p.end_date order by sc.abscoord"));

	//��� ����������
	queries.insert(query_t(L"BRACE_TYPE",
		L"select sc.abscoord as x, 0 as y, ec.abscoord-sc.abscoord as size_x, 0 as size_y, type_of_brace as label, 0 as flags "
		L"from APR_RAILS_BRACE l "
		L"inner join ENU_BRACE_TYPE n on n.id=l.ENU_BRACE_TYPE_ID inner join APR_RAILS_BRACE_PERIOD p on l.APR_RAILS_BRACE_PERIOD_ID=p.id "			
		L"left join get_abscoord(%WAY_ID%, l.start_km, l.start_m, '%TRAIL_DATE%') sc on 1=1 "
		L"left join get_abscoord(%WAY_ID%, l.end_km, l.end_m, '%TRAIL_DATE%') ec on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.start_date < '%TRAIL_DATE%' and '%TRAIL_DATE%' < p.end_date order by sc.abscoord"));

	//����� ������������ ���� (�����)
	queries.insert(query_t(L"LONG_RAILS_LEFT",
		L"SELECT s.ABSCOORD as x, 0 as y, (e.ABSCOORD-s.ABSCOORD) as size_x, 0 as size_y, a.NUM as label "
		L"FROM APR_LONG_RAILS a "
		L"inner join APR_LONG_RAILS_PERIOD p on p.ID=a.APR_LONG_RAILS_PERIOD_ID "
		L"left join GET_ABSCOORD(p.ADM_WAY_ID, a.START_KM, a.START_M, '%TRAIL_DATE%') s on 1=1 "
		L"left join GET_ABSCOORD(p.ADM_WAY_ID, a.END_KM, a.END_M, '%TRAIL_DATE%') e on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and a.ENU_SIDE_ID=-1"));

	//����� ������������ ���� (������)
	queries.insert(query_t(L"LONG_RAILS_RIGHT",
		L"SELECT s.ABSCOORD as x, 0 as y, (e.ABSCOORD-s.ABSCOORD) as size_x, 0 as size_y, a.NUM as label "
		L"FROM APR_LONG_RAILS a "
		L"inner join APR_LONG_RAILS_PERIOD p on p.ID=a.APR_LONG_RAILS_PERIOD_ID "
		L"left join GET_ABSCOORD(p.ADM_WAY_ID, a.START_KM, a.START_M, '%TRAIL_DATE%') s on 1=1 "
		L"left join GET_ABSCOORD(p.ADM_WAY_ID, a.END_KM, a.END_M, '%TRAIL_DATE%') e on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and a.ENU_SIDE_ID=1"));

	//������� ��������� � �������

	//������� ��������
	queries.insert(query_t(L"REPAIRS",
		L"SELECT s.ABSCOORD as x, 0 as y, (e.ABSCOORD-s.ABSCOORD) as size_x, 0 as size_y, "
		L"t.NAME || ' � ' || CAST (a.START_DATE as VARCHAR(32)) || ' �� ' || CAST (a.END_DATE as VARCHAR(32)) as label "//, a.START_DATE, a.END_DATE, , e.ABSCOORD 
		L"FROM REPAIR_PROJECT a "
		L"inner join ENU_REPAIR_TYPE t on t.ID=a.ENU_REPAIR_TYPE_ID "
		L"inner join ADM_WAY w on w.ID=a.ADM_WAY_ID "
		L"inner join ADM_DIRECTION d on d.ID=w.ADM_DIRECTION_ID  "
		L"left join GET_ABSCOORD(w.ID, a.START_KM, a.START_M, '%TRAIL_DATE%') s on 1=1 "
		L"left join GET_ABSCOORD(w.ID, a.END_KM, a.END_M, '%TRAIL_DATE%') e on 1=1 "
		L"where a.ADM_WAY_ID=%WAY_ID% "
		L"AND NOT EXISTS (SELECT ID from REPAIR_PROJECT a1 "
		L"left join GET_ABSCOORD(a1.ADM_WAY_ID, a1.START_KM, a1.START_M, '%TRAIL_DATE%') s1 on 1=1 "
		L"left join GET_ABSCOORD(a1.ADM_WAY_ID, a1.END_KM, a1.END_M, '%TRAIL_DATE%') e1 on 1=1 "
		L"where (not a1.ID=a.ID) and a1.ADM_WAY_ID=a.ADM_WAY_ID AND a1.START_DATE > a.START_DATE AND  "
		L"NOT (e1.ABSCOORD<s.ABSCOORD OR s1.ABSCOORD>e.ABSCOORD))"));

	//���� �� ����������
	queries.insert(query_t(L"SSSP_KM",
		L"select start_abs as x, sssp as y, end_abs-start_abs as size_x, -sssp as size_y, 0 as flags, '' as label from msr_sssp_km where "
		L"msr_trail_id=%TRAIL_ID% order by start_abs"));
	
	//���� �� �������
	queries.insert(query_t(L"SSSP_PK",
		L"select start_abs as x, sssp as y, end_abs-start_abs as size_x, -sssp as size_y, 0 as flags, '' as label from msr_sssp_picket where "
		L"msr_trail_id=%TRAIL_ID% order by start_abs"));		

	//�������� �� ��������� ������
	queries.insert(query_t(L"APR_CURVE",
		L"select a.ABSCOORD as x, 0 as y, 0 as size_x, 0 as size_y, '' as label, 0 as flags from apr_curve c "
		L"inner join apr_curve_period p on p.ID=c.APR_CURVE_PERIOD_ID "			
		L"left join get_abscoord(p.ADM_WAY_ID, c.CRV_START_KM, c.CRV_START_M, '%TRAIL_DATE%') a on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.START_DATE<'%TRAIL_DATE%' and '%TRAIL_DATE%'<p.END_DATE "
		L"union all "
		L"select a.ABSCOORD as x, 0 as y, 0 as size_x, 0 as size_y, '' as label, 0 as flags from apr_curve c "
		L"inner join apr_curve_period p on p.ID=c.APR_CURVE_PERIOD_ID "			
		L"left join get_abscoord(p.ADM_WAY_ID, c.CRV_END_KM, c.CRV_END_M, '%TRAIL_DATE%') a on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.START_DATE<'%TRAIL_DATE%' and '%TRAIL_DATE%'<p.END_DATE "
		L"union all "
		L"select a.ABSCOORD as x, c.ENU_SIDE_ID*50000/e.RADIUS as y, 0 as size_x, 0 as size_y, '' as label, 0 as flags from apr_elcurve e "
		L"inner join apr_curve c on c.ID=e.APR_CURVE_ID "
		L"inner join apr_curve_period p on p.ID=c.APR_CURVE_PERIOD_ID	"
		L"left join get_abscoord(p.ADM_WAY_ID, e.CRV_START_KM, e.CRV_START_M, '%TRAIL_DATE%') a on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.START_DATE<'%TRAIL_DATE%' and '%TRAIL_DATE%'<p.END_DATE "
		L"union all "
		L"select a.ABSCOORD+e.CRV_LEN as x, c.ENU_SIDE_ID*50000/e.RADIUS as y, 0 as size_x, 0 as size_y, '' as label, 0 as flags from apr_elcurve e "
		L"inner join apr_curve c on c.ID=e.APR_CURVE_ID "
		L"inner join apr_curve_period p on p.ID=c.APR_CURVE_PERIOD_ID	"
		L"left join get_abscoord(p.ADM_WAY_ID, e.CRV_START_KM, e.CRV_START_M, '%TRAIL_DATE%') a on 1=1  "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.START_DATE<'%TRAIL_DATE%' and '%TRAIL_DATE%'<p.END_DATE "
		L"order by 1"));

	//�������� �� ���������� ������
	queries.insert(query_t(L"MEASURED_CURVE",
		L"select m.CRV_ABS as x, m.CRV_VALUE as y, t.filename as label from msr_curve c "
		L"inner join msr_plan_elem p on  p.ID=c.MSR_PLAN_ELEM_ID "
		L"inner join msr_trail t on t.ID=p.MSR_TRAIL_ID   "
		L"inner join MSR_CURVE_MARK m on c.ID=m.MSR_CURVE_ID "
		L"where p.enu_elem_type_id = 3 and t.ADM_WAY_ID=%WAY_ID% and p.START_ABS < %END_ABS% and p.END_ABS > %START_ABS% and "
		L"not exists  "
		L"(select p1.id from msr_plan_elem p1, msr_trail t1 "
		L"where p1.enu_elem_type_id = 3 and p1.msr_trail_id = t1.id "
		L"and t1.adm_way_id=t.adm_way_id "
		L"and t1.date_trail > t.date_trail " 
		L"and p1.START_ABS < %END_ABS% and p1.END_ABS > %START_ABS%"
		L"and not (p1.START_ABS > p.END_ABS or p.START_ABS > p1.END_ABS ))"
		L"order by m.CRV_ABS"));
	
	//������� �� ��������� ������
	queries.insert(query_t(L"APR_LEVEL",
		L"select a.ABSCOORD as x, 0 as y, 0 as size_x, 0 as size_y, '' as label, 0 as flags from apr_curve c "
		L"inner join apr_curve_period p on p.ID=c.APR_CURVE_PERIOD_ID "			
		L"left join get_abscoord(p.ADM_WAY_ID, c.LVL_START_KM, c.LVL_START_M, '%TRAIL_DATE%') a on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.START_DATE<'%TRAIL_DATE%' and '%TRAIL_DATE%'<p.END_DATE "
		L"union  "
		L"select a.ABSCOORD as x, 0 as y, 0 as size_x, 0 as size_y, '' as label, 0 as flags from apr_curve c "
		L"inner join apr_curve_period p on p.ID=c.APR_CURVE_PERIOD_ID "			
		L"left join get_abscoord(p.ADM_WAY_ID, c.LVL_END_KM, c.LVL_END_M, '%TRAIL_DATE%') a on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.START_DATE<'%TRAIL_DATE%' and '%TRAIL_DATE%'<p.END_DATE "
		L"union "
		L"select a.ABSCOORD as x, c.ENU_SIDE_ID*e.LVL as y, 0 as size_x, 0 as size_y, '' as label, 0 as flags from apr_elcurve e "
		L"inner join apr_curve c on c.ID=e.APR_CURVE_ID "
		L"inner join apr_curve_period p on p.ID=c.APR_CURVE_PERIOD_ID	"
		L"left join get_abscoord(p.ADM_WAY_ID, e.LVL_START_KM, e.LVL_START_M, '%TRAIL_DATE%') a on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.START_DATE<'%TRAIL_DATE%' and '%TRAIL_DATE%'<p.END_DATE "
		L"union "
		L"select a.ABSCOORD+e.LVL_LEN as x, c.ENU_SIDE_ID*e.LVL as y, 0 as size_x, 0 as size_y, '' as label, 0 as flags from apr_elcurve e "
		L"inner join apr_curve c on c.ID=e.APR_CURVE_ID "
		L"inner join apr_curve_period p on p.ID=c.APR_CURVE_PERIOD_ID	"
		L"left join get_abscoord(p.ADM_WAY_ID, e.LVL_START_KM, e.LVL_START_M,' %TRAIL_DATE%') a on 1=1  "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.START_DATE<'%TRAIL_DATE%' and '%TRAIL_DATE%'<p.END_DATE "
		L"order by 1"));


	//������� �� ���������� ������
	queries.insert(query_t(L"MEASURED_LEVEL",
		L"select m.LVL_ABS as x, m.LVL_VALUE as y, t.filename as label from msr_curve c "
		L"inner join msr_plan_elem p on  p.ID=c.MSR_PLAN_ELEM_ID "
		L"inner join msr_trail t on t.ID=p.MSR_TRAIL_ID   "
		L"inner join MSR_CURVE_MARK m on c.ID=m.MSR_CURVE_ID "
		L"where p.enu_elem_type_id = 3 and t.ADM_WAY_ID=%WAY_ID% and p.START_ABS < %END_ABS% and p.END_ABS > %START_ABS% and "
		L"not exists  "
		L"(select p1.id from msr_plan_elem p1, msr_trail t1 "
		L"where p1.enu_elem_type_id = 3 and p1.msr_trail_id = t1.id "
		L"and t1.adm_way_id=t.adm_way_id "
        L"and t1.date_trail > t.date_trail "
		L"and p1.START_ABS < %END_ABS% and p1.END_ABS > %START_ABS%"
		L"and not (p1.START_ABS > p.END_ABS or p.START_ABS > p1.END_ABS )) "
		L"order by m.CRV_ABS"));
			
	//�������
	queries.insert(query_t(L"MARKERS",
		L"SELECT a.ABSCOORD as x, 0 as y, m.S_NAME as label "
		L"FROM MSR_WAY_MARKER a "
		L"inner join ENU_MARKER_TYPE m on m.ID=a.ENU_MARKER_TYPE_ID "
	    L"inner join msr_trail t on t.id=a.MSR_TRAIL_ID "
		L"where t.ADM_WAY_ID=%WAY_ID% "
		L"order by t.DATE_TRAIL descending, abscoord"));

	//������� ������
	queries.insert(query_t(L"CURVES",
		L"select s.ABSCOORD as x, 0 as y, e.ABSCOORD-s.ABSCOORD as size_x, 0 as size_y, '' as label, 0 as flags "
		L"from apr_curve c "
		L"inner join apr_curve_period p on p.ID=c.APR_CURVE_PERIOD_ID "
		L"left join get_abscoord(p.ADM_WAY_ID, c.CRV_START_KM, c.CRV_START_M, c.DATE_COMP) s on 1=1 "
		L"left join get_abscoord(p.ADM_WAY_ID, c.CRV_END_KM, c.CRV_END_M, c.DATE_COMP) e on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.START_DATE <'%TRAIL_DATE%' and '%TRAIL_DATE%' < p.END_DATE "
		L"order by s.abscoord"));	

	//������� �������� ������
	queries.insert(query_t(L"RADIUS",
		L"select a.ABSCOORD as x, 0 as y, e.CRV_LEN as size_x, 0 as size_y, "
		L"'R=' || (cast (cast (e.RADIUS as integer) as varchar(16) )) as label, 0 as flags "
		L"from apr_elcurve e "
		L"inner join apr_curve c on c.ID=e.APR_CURVE_ID "
		L"inner join apr_curve_period p on p.ID=c.APR_CURVE_PERIOD_ID "
		L"left join get_abscoord(p.ADM_WAY_ID, e.CRV_START_KM, e.CRV_START_M, c.DATE_COMP) a on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.START_DATE <'%TRAIL_DATE%' and '%TRAIL_DATE%' < p.END_DATE "
		L"order by a.abscoord"));
		
		
	//������� ������ (�������)
	queries.insert(query_t(L"CURVES_LVL",
		L"select s.ABSCOORD as x, 0 as y, e.ABSCOORD-s.ABSCOORD as size_x, 0 as size_y, '' as label, 0 as flags "
		L"from apr_curve c "
		L"inner join apr_curve_period p on p.ID=c.APR_CURVE_PERIOD_ID "
		L"left join get_abscoord(p.ADM_WAY_ID, c.LVL_START_KM, c.LVL_START_M, c.DATE_COMP) s on 1=1 "
		L"left join get_abscoord(p.ADM_WAY_ID, c.LVL_END_KM, c.LVL_END_M, c.DATE_COMP) e on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.START_DATE <'%TRAIL_DATE%' and '%TRAIL_DATE%' < p.END_DATE "
		L"order by s.abscoord"));	

	//������� �������� ������ (�������)
	queries.insert(query_t(L"RADIUS_LVL",
		L"select a.ABSCOORD as x, 0 as y, e.CRV_LEN as size_x, 0 as size_y, "
		L"'H=' || (cast (cast (e.LVL as integer) as varchar(16) )) as label, 0 as flags "
		L"from apr_elcurve e "
		L"inner join apr_curve c on c.ID=e.APR_CURVE_ID "
		L"inner join apr_curve_period p on p.ID=c.APR_CURVE_PERIOD_ID "
		L"left join get_abscoord(p.ADM_WAY_ID, e.LVL_START_KM, e.LVL_START_M, c.DATE_COMP) a on 1=1 "
		L"where p.ADM_WAY_ID=%WAY_ID% and p.START_DATE <'%TRAIL_DATE%' and '%TRAIL_DATE%' < p.END_DATE "
		L"order by a.abscoord"));	

	//������� �� �������
	queries.insert(query_t(L"PROFILE_OBJECTS",
		L"select so.abscoord, 0, 0, 0, case when (t.number in (33,34,35,36)) then t.stype else o.name end || ';' || cast(o.km as varchar(8)) || ' �� �� ' || cast(pk.piket as varchar(8)) || '+'  || cast(format_float(pk.metre,1,0) as varchar(8)), t.number, o.enu_side_id, 0 "
		L"from APR_POINT_TO_OBJECT o "
		L"inner join APR_POINT_TO_OBJECT_PERIOD op on op.id=o.APR_POINT_TO_OBJECT_PERIOD_ID "
		L"left join GET_ABSCOORD(op.ADM_WAY_ID, o.km, o.m, '%TRAIL_DATE%') so on 1=1 "			
		L"left join get_piket(o.m) pk on 1=1 "			
		L"inner join ENU_POINT_OBJECT_TYPE t on t.id=o.ENU_POINT_OBJECT_TYPE_ID "			
		L"where op.ADM_WAY_ID=%WAY_ID% and op.START_DATE <'%TRAIL_DATE%' and '%TRAIL_DATE%' < op.END_DATE  and not t.number=100"
		L"order by 1"));

	//���������� �� ��� �������
	queries.insert(query_t(L"STATION_AXIS_DISTANCES", 
		L"SELECT b.ABSCOORD as x, 0 as y, CAST(coalesce( cast(b.ABSCOORD - (SELECT b.ABSCOORD "
        L"FROM STW_OPMARK_TO_POINT a "
        L"inner join STW_OPMARK b on b.ID=a.STW_OPMARK_ID "
		L"inner join STW_POINT_TO_OBJECT c on c.ID=a.STW_POINT_TO_OBJECT_ID "
		L"inner join STW_FUNCTION_TO_POINT f on f.STW_POINT_TO_OBJECT_ID=c.ID "
		L"inner join STW_WAY g on g.ID=c.STW_WAY_ID "
		L"where b.STW_TRAIL_ID=%TRAIL_ID%  and ENU_POINT_TYPE_ID=6) as numeric(6,2)), '') AS VARCHAR(8)) as label "
		L"FROM STW_OPMARK_TO_POINT a "
		L"inner join STW_OPMARK b on b.ID=a.STW_OPMARK_ID "
		L"inner join STW_POINT_TO_OBJECT c on c.ID=a.STW_POINT_TO_OBJECT_ID "
		L"inner join STW_OBJECT d on d.ID=c.STW_OBJECT_ID "
		L"inner join ENU_POINT_OBJECT_TYPE e on e.ID=c.ENU_POINT_OBJECT_TYPE_ID "
		L"where b.STW_TRAIL_ID=%TRAIL_ID% order by b.abscoord"));
		
	//��������� ������� ����
	queries.insert(query_t(L"STATION_PROFILE_HEADER",  
			L"select t.START_ABS as x, (t.END_ABS-t.START_ABS) as size_x, "
            L"case when (s.NAME = p.NAME) then "
            L"'��. ' || s.NAME || ' ���� ' || w.code || ' (' || cast (t.date_trail as date) || ' ��-' || t.nvps || ')'"
            L"else "
            L"'��. ' || s.NAME || ' ���� ' || p.NAME || ' ���� ' || w.code || ' (' || cast (t.date_trail as date) || ' ��-' ||  t.nvps || ')' "
            L"end as label "
            L"from stw_trail t "
            L"inner join stw_way w on w.ID=t.STW_WAY_ID "
            L"inner join stw_park p on p.ID=w.STW_PARK_ID "
            L"inner join ADM_STATION s on s.ID=p.ADM_STATION_ID where t.id=%TRAIL_ID% "));
    
    //���� ��
    queries.insert(query_t(L"SSKO_WIDTH", 
		L"select start_abs as x, (end_abs-start_abs) as size_x, sko_width as y, -sko_width as size_y from msr_sssp_picket where msr_trail_id=%TRAIL_ID%"));
	//���� �������� �����
    queries.insert(query_t(L"SSKO_PIT_LEFT", 
		L"select start_abs as x, (end_abs-start_abs) as size_x, sko_pit_l as y, -sko_pit_l as size_y from msr_sssp_picket where msr_trail_id=%TRAIL_ID%"));
	//���� �������� ������
    queries.insert(query_t(L"SSKO_PIT_RIGHT", 
		L"select start_abs as x, (end_abs-start_abs) as size_x, sko_pit_r as y, -sko_pit_r as size_y from msr_sssp_picket where msr_trail_id=%TRAIL_ID%"));
	//���� ������
    queries.insert(query_t(L"SSKO_LEVEL", 
		L"select start_abs as x, (end_abs-start_abs) as size_x, sko_lvl as y, -sko_lvl as size_y from msr_sssp_picket where msr_trail_id=%TRAIL_ID%"));
	//���� �������� �����
    queries.insert(query_t(L"SSKO_RICH_LEFT", 
		L"select start_abs as x, (end_abs-start_abs) as size_x, sko_rich_l as y, -sko_rich_l as size_y from msr_sssp_picket where msr_trail_id=%TRAIL_ID%"));
	//���� �������� ������
    queries.insert(query_t(L"SSKO_RICH_RIGHT", 
		L"select start_abs as x, (end_abs-start_abs) as size_x, sko_rich_r as y, -sko_rich_r as size_y from msr_sssp_picket where msr_trail_id=%TRAIL_ID%"));
	
	//����������� �������� �� ������������ ��� �� �������
	queries.insert(query_t(L"OVERSTEP_RESTRICTIONS_PK", 
		L"execute block returns (x float, size_x float, label varchar(32)) "
		L"as  "
		L"declare variable way_id integer;  "
		L"declare variable trail_date date;  "
		L"declare variable trail_start float;  "
		L"declare variable trail_end float;  "
		L"begin "
		L"	select adm_way_id, date_trail, start_abs, end_abs from msr_trail where id=%TRAIL_ID% into :way_id, :trail_date, :trail_start, :trail_end; "
		L"	for select start_abs, (end_abs-start_abs) from GET_PIKET_ABS(:way_id, :trail_date)   "
		L"		where (not (start_abs > :trail_end or end_abs < :trail_start)) "
		L"		into :x, :size_x do "
		L"	begin "
		L"		select cast(min(speedlim1) as varchar(8)) || '/' || cast(min(speedlim2) as varchar(8)) || '/' || cast(min(speedlim3) as varchar(8))  "
		L"			from msr_oversteps where msr_trail_id=%TRAIL_ID% and abscoord > :x and abscoord < :x+:size_x and has_restriction=1 into :label; "
		L"		if (:label is not null) then "
		L"		suspend; "
		L"	end "
		L"end "));
}

///���������� ������� �� ��������	
const std::wstring& VPredefinedQueries::GetCommand(const std::wstring& in_description) const
{		
	query_itr i=queries.find(in_description);
	if (i!=queries.end())
		return i->second;
	else
		return zero;
}

//--------------------------------------- ������ ������ VLineMarkData ---------------------------------

/** �����������*/
VLineMarkData::VLineMarkData(VDataSetPtr in_data_set) : VXOrderedDataProvider()
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_set->Load(PRM_LINE_MARKS);
		data_set->Load(PRM_PLAN_OFFSET);		
		drag=false;		
	}
	else
	{
		throw NULL_POINTER;
	}	
}

/** ���������� ������ ������*/
size_t VLineMarkData::GetCount() const
{
	return data_set->GetLineMarks().GetCount();
}

/** ���������� ���������� �� ��� X*/
double VLineMarkData::GetX(size_t in_index) const
{	
	return data_set->GetAbsCoord().GetItem(data_set->GetLineMarks().GetItem(in_index).index);
}

/** ���������� ���������� �� ��� Y*/
double VLineMarkData::GetY(size_t in_index) const
{		
	return in_index<data_set->GetParameter(PRM_PLAN_OFFSET).GetCount() ? 
		data_set->GetParameter(PRM_PLAN_OFFSET).GetItem(data_set->GetLineMarks().GetItem(in_index).index) : 0;
}

/** ������������� �������� X*/
void VLineMarkData::SetXY(size_t in_index, double in_x, double in_y)
{
	if (GetActive())
	{
		size_t point_index=data_set->GetAbsCoord().FindIndex(in_x, true);
		if (data_set->AllowMarkHereLine(point_index, 0.0))
		{
			data_set->GetLineMarks().GetItem(in_index).index=point_index;			
			data_set->GetLineMarks().Sort();
			data_set->SetChanged(PRM_LINE_MARKS);
			//�������� ���������� �� ������
			data_set->UpdatePlanOffset(point_index, false);
			data_set->UpdatePlanOffset(point_index, true);
		}
	}
}

/** ��������� �����*/
void VLineMarkData::AddXY(size_t in_index, double in_x, double in_y, double in_size_x, double in_size_y, 
	const std::wstring& in_label, uint64_t in_flags)
{
	if (GetActive())
	{
		VLineMark new_mark(data_set->GetAbsCoord().FindIndex(in_x, true));		
		if (data_set->AllowMarkHereLine(new_mark.index, 0.0))
		{			
			data_set->GetLineMarks().AddItem(new_mark);
			data_set->GetLineMarks().Sort();
			data_set->SetChanged(PRM_LINE_MARKS);
			//�������� ���������� �� ������
			data_set->UpdatePlanOffset(new_mark.index, false);
			data_set->UpdatePlanOffset(new_mark.index, true);
		}
	}
}

/** ������� �����*/
void VLineMarkData::DeleteItem(size_t in_index)
{
	if (GetActive())
	{	
		size_t point_index=data_set->GetLineMarks().GetItem(in_index).index;
		data_set->GetLineMarks().DeleteItem(in_index);
		data_set->SetChanged(PRM_LINE_MARKS);		
		//�������� ���������� �� ������
		data_set->UpdatePlanOffset(point_index, false);
		data_set->UpdatePlanOffset(point_index, true);
	}
}

/** ������� ��� �����*/
void VLineMarkData::DeleteAll()
{	
	if (GetActive())
	{
		data_set->GetLineMarks().Unload();
		data_set->SetChanged(PRM_LINE_MARKS);		
		data_set->UpdatePlanOffset(false);
		data_set->UpdatePlanOffset(true);
	}
};

/** �������������� ��������*/
void VLineMarkData::Automark()
{	
	//ToDo!!!
}

/** ���������� �����*/
std::wstring VLineMarkData::GetLabel(size_t in_index) const
{
	return L"";
}

/** ���������� ����������� �������� �� Y*/
double VLineMarkData::GetMinY()
{		
	return 0;
}

/** ���������� ������������ �������� �� Y*/
double VLineMarkData::GetMaxY()
{		
	return 0;
}

/** ���������� ����������� �������� �� X*/
double VLineMarkData::GetMinX() 
{	
	return data_set->GetAbsCoord().GetItem(0);
};

/** ���������� ������������ �������� �� X*/
double VLineMarkData::GetMaxX()
{	
	return data_set->GetAbsCoord().GetLastItem();
};

//---------------------------------- ������ ������ VRTOverstepData --------------------------------

/** �����������*/
VRTOverstepData::VRTOverstepData()
{}

/** ��������� ������*/
void VRTOverstepData::Put(const VBasicOverstepPtr in_overstep)
{
	oversteps.push_back(in_overstep);
}

/** ������� ������*/
void VRTOverstepData::Clear()
{
	oversteps.clear();
}

/** ���������� ������ ������*/
size_t VRTOverstepData::GetCount() const
{
	return oversteps.size();
}

/** ���������� ���������� �� ��� X*/
double VRTOverstepData::GetX(size_t in_index) const
{
	return std::min<double>(oversteps.at(in_index)->GetStart(), oversteps.at(in_index)->GetEnd());
}

/** ���������� ���������� �� ��� Y*/
double VRTOverstepData::GetY(size_t in_index) const
{
	return 0;
}

/** ���������� ������ �� ��� X*/
double VRTOverstepData::GetSizeX(size_t in_index) const
{
	return fabs(oversteps.at(in_index)->GetStart() - oversteps.at(in_index)->GetEnd());
}

/** ���������� ������ �� ��� Y*/
double VRTOverstepData::GetSizeY(size_t in_index) const
{
	return (oversteps.at(in_index)->HasRestriction() && oversteps.at(in_index)->GetDegree()==DEGREE_NONE) ? DEGREE4 : oversteps.at(in_index)->GetDegree();
}

/** ���������� �����*/
std::wstring VRTOverstepData::GetLabel(size_t in_index) const
{
	return oversteps.at(in_index)->GetDescription();
}

/** ���������� �����*/
uint64_t VRTOverstepData::GetFlags(size_t in_index) const
{
	return uint64_t(oversteps.at(in_index)->IsExcluded());
}

/** ���������� ����������� �������� �� Y*/
double VRTOverstepData::GetMinY()
{
	return 0;
}
/** ���������� ������������ �������� �� Y*/
double VRTOverstepData::GetMaxY()
{
	return 0;
}
/** ���������� ����������� �������� �� X*/
double VRTOverstepData::GetMinX()
{
	return GetCount()>0 ? GetX(0) : 0;
}

/** ���������� ������������ �������� �� X*/
double VRTOverstepData::GetMaxX()
{
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
}

//---------------------------------------- ������ ������ VRTEventInfoData -----------------------------------------

/** �����������*/
VRTEventInfoData::VRTEventInfoData(){};

/** ��������� �����������*/
void VRTEventInfoData::Put(const VBasicOverstepPtr in_overstep)
{		
	bool is_draw_frame=in_overstep->IsType(WIDTH_CORRUPT) ||
		in_overstep->IsType(PIT_LEFT_CORRUPT) ||
		in_overstep->IsType(PIT_RIGHT_CORRUPT) ||
		in_overstep->IsType(RICH_LEFT_CORRUPT) ||
		in_overstep->IsType(RICH_RIGHT_CORRUPT) ||
		in_overstep->IsType(LEVEL_CORRUPT) ||
		in_overstep->IsType(WEAR_LEFT_CORRUPT) ||
		in_overstep->IsType(WEAR_RIGHT_CORRUPT) ||
		in_overstep->IsType(WEAR_CORRUPT) ||
		in_overstep->IsType(WIDTH_OFF) ||
		in_overstep->IsType(PIT_LEFT_OFF) ||
		in_overstep->IsType(PIT_RIGHT_OFF) ||
		in_overstep->IsType(RICH_LEFT_OFF) ||
		in_overstep->IsType(RICH_RIGHT_OFF) ||
		in_overstep->IsType(LEVEL_OFF) ||
		in_overstep->IsType(WEAR_LEFT_OFF) ||
		in_overstep->IsType(WEAR_RIGHT_OFF) ||
		in_overstep->IsType(ZAZOR_LEFT_OFF) ||
		in_overstep->IsType(ZAZOR_RIGHT_OFF) ||
		in_overstep->IsType(MAG_ZAZOR_LEFT_OFF) ||
		in_overstep->IsType(MAG_ZAZOR_RIGHT_OFF) ||
		in_overstep->IsType(CONTROL_WIDTH_MO) ||
		in_overstep->IsType(CONTROL_LEVEL_MO) ||
		in_overstep->IsType(CONTROL_WIDTH_SKO) ||
		in_overstep->IsType(CONTROL_LEVEL_SKO) ||
		in_overstep->IsType(W10) ||
		in_overstep->IsType(K100) ||
		in_overstep->IsType(K60);
	data.push_back(VRTEvent(in_overstep, is_draw_frame));
}

/** ��������� �������*/
void VRTEventInfoData::Put(double in_x, const std::wstring& in_coord, const std::wstring& in_description, int in_flag)
{
	data.push_back(VRTEvent(in_x, in_coord, in_description, in_flag));
}

/** ��������� �������*/
void VRTEventInfoData::Sort()
{
	std::sort(data.begin(), data.end());
}

/** ������� ������*/
void VRTEventInfoData::Clear()
{
	data.clear();
}

/// �������� ������� ��� ������������
void VRTEventInfoData::SetDrawn(size_t in_index)
{
	data.at(in_index).drawn=true;
}

/** �������  ������������ �������*/
void VRTEventInfoData::ClearDrawn()
{
	struct if_drawn
	{
		bool operator()(const VRTEvent& in_event) const
		{
			return in_event.drawn;
		};
	};
	data.erase(std::remove_if(data.begin(), data.end(), if_drawn()), data.end());	
}

std::wstring VRTEventInfoData::GetCoord(size_t in_index) const
{
	return data.at(in_index).coord;
}

std::wstring VRTEventInfoData::GetType(size_t in_index) const
{
	return data.at(in_index).type;
}

std::wstring VRTEventInfoData::GetDegree(size_t in_index) const
{
	return data.at(in_index).degree;
}

std::wstring VRTEventInfoData::GetValue(size_t in_index) const
{
	return data.at(in_index).value;
}

std::wstring VRTEventInfoData::GetLength(size_t in_index) const
{
	return data.at(in_index).length;
}

std::wstring VRTEventInfoData::GetAmount(size_t in_index) const
{
	return data.at(in_index).count;
}

std::wstring VRTEventInfoData::GetRestriction(size_t in_index) const
{
	return data.at(in_index).restriction;
}

bool VRTEventInfoData::HasRestriction(size_t in_index) const
{
	return data.at(in_index).has_restriction;
}

bool VRTEventInfoData::IsDrawFrame(size_t in_index) const
{
	return data.at(in_index).is_draw_frame;
}

bool VRTEventInfoData::IsBoldFont(size_t in_index) const
{
	return data.at(in_index).is_bold_font;
}

/** ���������� ������ ������*/
size_t VRTEventInfoData::GetCount() const
{
	return data.size();
}

/** ���������� ���������� �� ��� X*/
double VRTEventInfoData::GetX(size_t in_index) const
{
	return data.empty() ? 0 : data.at(in_index).x;
}

/** ���������� ���������� �� ��� Y*/
double VRTEventInfoData::GetY(size_t in_index) const
{
	return 0;
}

/** ���������� �����*/
std::wstring VRTEventInfoData::GetLabel(size_t in_index) const
{
	return boost::str(boost::wformat(L"%s %s") % data.at(in_index).coord % data.at(in_index).type);
}

/** ���������� �����*/
uint64_t VRTEventInfoData::GetFlags(size_t in_index) const
{
	return data.at(in_index).flag;
}

/** ���������� ����������� �������� �� Y*/
double VRTEventInfoData::GetMinY()
{
	return 0;
}

/** ���������� ������������ �������� �� Y*/
double VRTEventInfoData::GetMaxY()
{
	return 0;
}

/** ���������� ����������� �������� �� X*/
double VRTEventInfoData::GetMinX()
{
	return data.empty() ? 0 : data.front().x;
}

/** ���������� ������������ �������� �� X*/
double VRTEventInfoData::GetMaxX()
{
	return data.empty() ? 0 : data.back().x;
}

//--------------------------------------- ������ ������ VAprinfoSpeedSectionData ---------------------------

/** ���c�������*/
VAprinfoSpeedSectionData::VAprinfoSpeedSectionData(const VAprinfo& in_aprinfo, double in_start, double in_end)
{
	std::vector<VAprinfoSpeedRecord> tmp;
	tmp=in_aprinfo.GetSpeedList(in_start, in_end);
	xdata.reserve(tmp.size());
	xdata.resize(tmp.size());
	xsize.reserve(tmp.size());
	xsize.resize(tmp.size());
	labels.reserve(tmp.size());
	labels.resize(tmp.size());
	for (size_t i=0; i<tmp.size(); ++i)
	{
		xdata.at(i)=tmp.at(i).base;
		xsize.at(i)=tmp.at(i).end-tmp.at(i).base;
		labels.at(i)=boost::str(boost::wformat(L"%s") % tmp.at(i).format_short());
	}
}

/** ���������� ������ ������*/
size_t VAprinfoSpeedSectionData::GetCount() const
{
	return xdata.size();
}

/** ���������� ���������� �� ��� X*/
double VAprinfoSpeedSectionData::GetX(size_t in_index) const
{
	return xdata.at(in_index);
}

/** ���������� ���������� �� ��� Y*/
double VAprinfoSpeedSectionData::GetY(size_t in_index) const
{
	return 0;
}

/** ���������� ������ �� ��� X*/
double VAprinfoSpeedSectionData::GetSizeX(size_t in_index) const
{
	return xsize.at(in_index);
}

/** ���������� ������ �� ��� Y*/
double VAprinfoSpeedSectionData::GetSizeY(size_t in_index) const
{
	return 0;
}

/** ���������� �����*/
std::wstring VAprinfoSpeedSectionData::GetLabel(size_t in_index) const
{
	return labels.at(in_index);
}

/** ���������� ����������� �������� �� Y*/
double VAprinfoSpeedSectionData::GetMinY()
{
	return 0;
}

/** ���������� ������������ �������� �� Y*/
double VAprinfoSpeedSectionData::GetMaxY()
{
	return 0;
}

/** ���������� ����������� �������� �� X*/
double VAprinfoSpeedSectionData::GetMinX()
{
	return xdata.front();
}

/** ���������� ������������ �������� �� X*/
double VAprinfoSpeedSectionData::GetMaxX()
{
	return xdata.back();
}

//---------------------------------- ������ ������ VSwitchData --------------------------------------

/** ���c�������*/
VSwitchData::VSwitchData(){}

/// ��������� �������
void VSwitchData::Put(const VAprinfoSwitchRecord& in_data)
{
	if (data.empty() || in_data.base>data.back().base)
		data.push_back(in_data);
	else
		data.push_front(in_data);	
}

/// ������� ������
void VSwitchData::Clear()
{
	data.clear();
}

/** ���������� ������ ������*/
size_t VSwitchData::GetCount() const
{
	return data.size();
}

/** ���������� ���������� �� ��� X*/
double VSwitchData::GetX(size_t in_index) const
{
	return data.at(in_index).GetCoordX();
}

/** ���������� ���������� �� ��� Y*/
double VSwitchData::GetY(size_t in_index) const
{
	return 0;
}

/** ���������� �����*/
std::wstring VSwitchData::GetLabel(size_t in_index) const
{
	return string_to_wstring(data.at(in_index).num);
}

/** ���������� �����*/
uint64_t VSwitchData::GetFlags(size_t in_index) const
{
	return SwitchGlyphType(side_t(data.at(in_index).side), data.at(in_index).dir);
}


//---------------------------------- ������ ������ VSwitchSectionsData --------------------------------------

/** ���c�������*/
VSwitchSectionsData::VSwitchSectionsData(){}

/// ��������� �������
void VSwitchSectionsData::Put(const VAprinfoSwitchRecord& in_data)
{
	if (data.empty() || in_data.base>data.back().base)
		data.push_back(in_data);
	else
		data.push_front(in_data);
}

/// ������� ������
void VSwitchSectionsData::Clear()
{
	data.clear();
}

/** ���������� ������ ������*/
size_t VSwitchSectionsData::GetCount() const
{
	return data.size();
}

/** ���������� ���������� �� ��� X*/
double VSwitchSectionsData::GetX(size_t in_index) const
{
	return data.at(in_index).GetRange().left_bound().value;
}

/** ���������� ������ �� ��� X*/
double VSwitchSectionsData::GetSizeX(size_t in_index) const
{
	return data.at(in_index).GetRange().length();
}

/** ���������� ���������� �� ��� Y*/
double VSwitchSectionsData::GetY(size_t in_index) const
{
	return 0;
}

/** ���������� �����*/
std::wstring VSwitchSectionsData::GetLabel(size_t in_index) const
{
	return string_to_wstring(data.at(in_index).num);
}

/** ���������� �����*/
uint64_t VSwitchSectionsData::GetFlags(size_t in_index) const
{
	return SwitchGlyphType(side_t(data.at(in_index).side), data.at(in_index).dir);
}

//----------------------------------- ������ ������ VAprinfoBridgeData ------------------------------

/** ���c�������*/
VAprinfoBridgeData::VAprinfoBridgeData(const VAprinfo& in_aprinfo, bool in_include_areas)
{
	data=in_aprinfo.GetBridgeList(in_include_areas);
}

/** ���������� ������ ������*/
size_t VAprinfoBridgeData::GetCount() const
{
	return data.size();
}

/** ���������� ���������� �� ��� X*/
double VAprinfoBridgeData::GetX(size_t in_index) const
{
	return data.at(in_index).base;
}

/** ���������� ���������� �� ��� Y*/
double VAprinfoBridgeData::GetY(size_t in_index) const
{
	return 0;
}

/** ���������� ������ �� ��� X*/
double VAprinfoBridgeData::GetSizeX(size_t in_index) const
{
	return data.at(in_index).end-data.at(in_index).base;
}
/** ���������� ������ �� ��� Y*/
double VAprinfoBridgeData::GetSizeY(size_t in_index) const
{
	return 0;
}

/** ���������� �����*/
std::wstring VAprinfoBridgeData::GetLabel(size_t in_index) const
{
	return L"����";
}

/** ���������� ����������� �������� �� Y*/
double VAprinfoBridgeData::GetMinY()
{
	return 0;
}

/** ���������� ������������ �������� �� Y*/
double VAprinfoBridgeData::GetMaxY()
{
	return 0;
}

/** ���������� ����������� �������� �� X*/
double VAprinfoBridgeData::GetMinX()
{
	return GetCount()>0 ? GetX(0) : 0;
}

/** ���������� ������������ �������� �� X*/
double VAprinfoBridgeData::GetMaxX()
{
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
}

//----------------------------------- ������ ������ VAprinfoChamferedJointData ------------------------------

/** ���c�������*/
VAprinfoChamferedJointData::VAprinfoChamferedJointData(const VAprinfo& in_aprinfo)
{
	data=in_aprinfo.GetChamJoints();
}

/** ���������� ������ ������*/
size_t VAprinfoChamferedJointData::GetCount() const
{
	return data.size();
}

/** ���������� ���������� �� ��� X*/
double VAprinfoChamferedJointData::GetX(size_t in_index) const
{
	return data.at(in_index).base;
}

/** ���������� ���������� �� ��� Y*/
double VAprinfoChamferedJointData::GetY(size_t in_index) const
{
	return 0;
}

/** ���������� ������ �� ��� X*/
double VAprinfoChamferedJointData::GetSizeX(size_t in_index) const
{
	return data.at(in_index).end-data.at(in_index).base;
}
/** ���������� ������ �� ��� Y*/
double VAprinfoChamferedJointData::GetSizeY(size_t in_index) const
{
	return 0;
}

/** ���������� �����*/
std::wstring VAprinfoChamferedJointData::GetLabel(size_t in_index) const
{
	return L"��.�";
}

/** ���������� ����������� �������� �� Y*/
double VAprinfoChamferedJointData::GetMinY()
{
	return 0;
}

/** ���������� ������������ �������� �� Y*/
double VAprinfoChamferedJointData::GetMaxY()
{
	return 0;
}

/** ���������� ����������� �������� �� X*/
double VAprinfoChamferedJointData::GetMinX()
{
	return GetCount()>0 ? GetX(0) : 0;
}

/** ���������� ������������ �������� �� X*/
double VAprinfoChamferedJointData::GetMaxX()
{
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
}


//----------------------------------- ������ ������ VAprinfoTunnelData ------------------------------

/** ���c�������*/
VAprinfoTunnelData::VAprinfoTunnelData(const VAprinfo& in_aprinfo, bool in_include_areas)
{
	data=in_aprinfo.GetTonnelList(in_include_areas);
}

/** ���������� ������ ������*/
size_t VAprinfoTunnelData::GetCount() const
{
	return data.size();
}

/** ���������� ���������� �� ��� X*/
double VAprinfoTunnelData::GetX(size_t in_index) const
{
	return data.at(in_index).base;
}

/** ���������� ���������� �� ��� Y*/
double VAprinfoTunnelData::GetY(size_t in_index) const
{
	return 0;
}

/** ���������� ������ �� ��� X*/
double VAprinfoTunnelData::GetSizeX(size_t in_index) const
{
	return data.at(in_index).end-data.at(in_index).base;
}
/** ���������� ������ �� ��� Y*/
double VAprinfoTunnelData::GetSizeY(size_t in_index) const
{
	return 0;
}

/** ���������� �����*/
std::wstring VAprinfoTunnelData::GetLabel(size_t in_index) const
{
	return L"����";
}

/** ���������� ����������� �������� �� Y*/
double VAprinfoTunnelData::GetMinY()
{
	return 0;
}

/** ���������� ������������ �������� �� Y*/
double VAprinfoTunnelData::GetMaxY()
{
	return 0;
}

/** ���������� ����������� �������� �� X*/
double VAprinfoTunnelData::GetMinX()
{
	return GetCount()>0 ? GetX(0) : 0;
}

/** ���������� ������������ �������� �� X*/
double VAprinfoTunnelData::GetMaxX()
{
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
}


//----------------------------------- ������ ������ VAprinfoCrossingData ------------------------------

/** ���c�������*/
VAprinfoCrossingData::VAprinfoCrossingData(const VAprinfo& in_aprinfo)
{
	data=in_aprinfo.GetCrossings();
}

/** ���������� ������ ������*/
size_t VAprinfoCrossingData::GetCount() const
{
	return data.size();
}

/** ���������� ���������� �� ��� X*/
double VAprinfoCrossingData::GetX(size_t in_index) const
{
	return data.at(in_index).coord;
}

/** ���������� ���������� �� ��� Y*/
double VAprinfoCrossingData::GetY(size_t in_index) const
{
	return 0;
}

/** ���������� ������ �� ��� X*/
double VAprinfoCrossingData::GetSizeX(size_t in_index) const
{
	return 10;
}
/** ���������� ������ �� ��� Y*/
double VAprinfoCrossingData::GetSizeY(size_t in_index) const
{
	return 0;
}

/** ���������� �����*/
std::wstring VAprinfoCrossingData::GetLabel(size_t in_index) const
{
	return L"�������";
}

/** ���������� ����������� �������� �� Y*/
double VAprinfoCrossingData::GetMinY()
{
	return 0;
}

/** ���������� ������������ �������� �� Y*/
double VAprinfoCrossingData::GetMaxY()
{
	return 0;
}

/** ���������� ����������� �������� �� X*/
double VAprinfoCrossingData::GetMinX()
{
	return GetCount()>0 ? GetX(0) : 0;
}

/** ���������� ������������ �������� �� X*/
double VAprinfoCrossingData::GetMaxX()
{
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
}


//--------------------------------- ������ ������ VAprinfoSpalaData --------------------------------------------

/** ���c�������*/
VAprinfoSpalaData::VAprinfoSpalaData(const VAprinfo& in_aprinfo)
{
	data=in_aprinfo.GetSpalaList();
}
/** ���������� ������ ������*/
size_t VAprinfoSpalaData::GetCount() const
{
	return data.size();
}
/** ���������� ���������� �� ��� X*/
double VAprinfoSpalaData::GetX(size_t in_index) const
{
	return data.at(in_index).base;
}
/** ���������� ���������� �� ��� Y*/
double VAprinfoSpalaData::GetY(size_t in_index) const
{
	return 0;
}
/** ���������� ������ �� ��� X*/
double VAprinfoSpalaData::GetSizeX(size_t in_index) const
{
	return fabs(data.at(in_index).end-data.at(in_index).base);
}
/** ���������� ������ �� ��� Y*/
double VAprinfoSpalaData::GetSizeY(size_t in_index) const
{
	return 0;
}
/** ���������� �����*/
std::wstring VAprinfoSpalaData::GetLabel(size_t in_index) const
{
	switch (GetFlags(in_index))
	{
		default:
		case SPALA_GB:
			return L"���";
		case SPALA_GB96:
			return L"��� �� 96";
		case SPALA_WOOD:
			return L"��";
	}
}
/** ���������� ����������� �������� �� Y*/
double VAprinfoSpalaData::GetMinY()
{
	return 0;
}
/** ���������� ������������ �������� �� Y*/
double VAprinfoSpalaData::GetMaxY()
{
	return 0;
}
/** ���������� ����������� �������� �� X*/
double VAprinfoSpalaData::GetMinX()
{
	return GetCount()>0 ? GetX(0) : 0;
}
/** ���������� ������������ �������� �� X*/
double VAprinfoSpalaData::GetMaxX()
{
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
}

/** ���������� ����� (ID �����)*/
uint64_t VAprinfoSpalaData::GetFlags(size_t in_index) const
{
	return data.at(in_index).value;
}


//--------------------------------- ������ ������ VAprinfoStraightSideData --------------------------------------------

/** ���c�������*/
VAprinfoStraightSideData::VAprinfoStraightSideData(const VAprinfo& in_aprinfo, bool is_passport)//FixMe!!! ���������� bool is_passport ���������
{		
	data=is_passport ? in_aprinfo.GetPassportCurveStraightList() : in_aprinfo.GetFactCurveStraightList();
}
/** ���������� ������ ������*/
size_t VAprinfoStraightSideData::GetCount() const
{
	return data.size();
}
/** ���������� ���������� �� ��� X*/
double VAprinfoStraightSideData::GetX(size_t in_index) const
{
	return data.at(in_index).base;
}
/** ���������� ���������� �� ��� Y*/
double VAprinfoStraightSideData::GetY(size_t in_index) const
{
	return 0;
}
/** ���������� ������ �� ��� X*/
double VAprinfoStraightSideData::GetSizeX(size_t in_index) const
{
	return fabs(data.at(in_index).end-data.at(in_index).base);
}
/** ���������� ������ �� ��� Y*/
double VAprinfoStraightSideData::GetSizeY(size_t in_index) const
{
	return 0;
}
/** ���������� �����*/
std::wstring VAprinfoStraightSideData::GetLabel(size_t in_index) const
{
	switch (GetFlags(in_index))
	{
		default:
		case SD_LEFT:
			return L"�";
		case SD_RIGHT:
			return L"�";		
	}
}
/** ���������� ����������� �������� �� Y*/
double VAprinfoStraightSideData::GetMinY()
{
	return 0;
}
/** ���������� ������������ �������� �� Y*/
double VAprinfoStraightSideData::GetMaxY()
{
	return 0;
}
/** ���������� ����������� �������� �� X*/
double VAprinfoStraightSideData::GetMinX()
{
	return GetCount()>0 ? GetX(0) : 0;
}
/** ���������� ������������ �������� �� X*/
double VAprinfoStraightSideData::GetMaxX()
{
	return GetCount()>0 ? GetX(GetCount()-1) : 0;
}

/** ���������� ����� (ID �����)*/
uint64_t VAprinfoStraightSideData::GetFlags(size_t in_index) const
{
	return data.at(in_index).value;
}

//---------------------------------- ������ ������ VAprinfoNormaLevelData -------------------------------

/** ���c�������*/
VAprinfoNormaLevelData::VAprinfoNormaLevelData(const VAprinfo& in_aprinfo)
{	
	std::vector<VAprinfoZLRecord> zl_list_level=in_aprinfo.GetPassportCurveLevelZLList();
	for (size_t i=0; i<zl_list_level.size(); ++i)
	{
		point_t start(zl_list_level.at(i).base, zl_list_level.at(i).value_base);
		point_t end(zl_list_level.at(i).end, zl_list_level.at(i).value_end);		
		xdata.push_back(start.x);
		xdata.push_back(end.x);
		ydata.push_back(start.y);
		ydata.push_back(end.y);
	}
}

/** ���������� ������ ������*/
size_t VAprinfoNormaLevelData::GetCount() const
{
	return xdata.size();
}

/** ���������� ���������� �� ��� X*/
double VAprinfoNormaLevelData::GetX(size_t in_index) const
{
	return xdata.at(in_index);
}

/** ���������� ���������� �� ��� Y*/
double VAprinfoNormaLevelData::GetY(size_t in_index) const
{
	return ydata.at(in_index);
}

/** ���������� ����������� �������� �� Y*/
double VAprinfoNormaLevelData::GetMinY()
{
	if (!ydata.empty())
		return *(std::min_element(ydata.begin(), ydata.end()));
	else
		return 0;
}

/** ���������� ������������ �������� �� Y*/
double VAprinfoNormaLevelData::GetMaxY()
{
	if (!ydata.empty())
		return *(std::max_element(ydata.begin(), ydata.end()));
	else
		return 0;
}

/** ���������� ����������� �������� �� X*/
double VAprinfoNormaLevelData::GetMinX() 
{
	return xdata.at(0);
};

/** ���������� ������������ �������� �� X*/
double VAprinfoNormaLevelData::GetMaxX()
{
	return xdata.at(xdata.size()-1);
};

//---------------------------------- ������ ������ VAprinfoNormaRichData -------------------------------

/** ���c�������*/
VAprinfoNormaRichData::VAprinfoNormaRichData(const VAprinfo& in_aprinfo)
{	
	std::vector<VAprinfoZLRecord> zl_list_rich=in_aprinfo.GetPassportCurveRichZLList();
	for (size_t i=0; i<zl_list_rich.size(); ++i)
	{
		point_t start(zl_list_rich.at(i).base, zl_list_rich.at(i).value_base);
		point_t end(zl_list_rich.at(i).end, zl_list_rich.at(i).value_end);		
		xdata.push_back(start.x);
		xdata.push_back(end.x);
		ydata.push_back(start.y);
		ydata.push_back(end.y);
	}
}

/** ���������� ������ ������*/
size_t VAprinfoNormaRichData::GetCount() const
{
	return xdata.size();
}

/** ���������� ���������� �� ��� X*/
double VAprinfoNormaRichData::GetX(size_t in_index) const
{
	return xdata.at(in_index);
}

/** ���������� ���������� �� ��� Y*/
double VAprinfoNormaRichData::GetY(size_t in_index) const
{
	return ydata.at(in_index);
}

/** ���������� ����������� �������� �� Y*/
double VAprinfoNormaRichData::GetMinY()
{
	if (!ydata.empty())
		return *(std::min_element(ydata.begin(), ydata.end()));
	else
		return 0;
}

/** ���������� ������������ �������� �� Y*/
double VAprinfoNormaRichData::GetMaxY()
{
	if (!ydata.empty())
		return *(std::max_element(ydata.begin(), ydata.end()));
	else
		return 0;
}

/** ���������� ����������� �������� �� X*/
double VAprinfoNormaRichData::GetMinX() 
{
	return xdata.at(0);
};

/** ���������� ������������ �������� �� X*/
double VAprinfoNormaRichData::GetMaxX()
{
	return xdata.at(xdata.size()-1);
};

//---------------------------------- ������ ������ VAprinfoNormaWidthData -------------------------------

/** ���c�������*/
VAprinfoNormaWidthData::VAprinfoNormaWidthData(const VAprinfo& in_aprinfo)
{	
	std::vector<VAprinfoWidthRecord> zl_list_width=in_aprinfo.GetPassportCurveWidthList();
	for (size_t i=0; i<zl_list_width.size(); ++i)
	{
		point_t start(zl_list_width.at(i).base, zl_list_width.at(i).width_begin);
		point_t end(zl_list_width.at(i).end, zl_list_width.at(i).width_end);		
		xdata.push_back(start.x);
		xdata.push_back(end.x);
		ydata.push_back(start.y);
		ydata.push_back(end.y);
	}
}

/** ���������� ������ ������*/
size_t VAprinfoNormaWidthData::GetCount() const
{
	return xdata.size();
}

/** ���������� ���������� �� ��� X*/
double VAprinfoNormaWidthData::GetX(size_t in_index) const
{
	return xdata.at(in_index);
}

/** ���������� ���������� �� ��� Y*/
double VAprinfoNormaWidthData::GetY(size_t in_index) const
{
	return ydata.at(in_index);
}

/** ���������� ����������� �������� �� Y*/
double VAprinfoNormaWidthData::GetMinY()
{
	if (!ydata.empty())
		return *(std::min_element(ydata.begin(), ydata.end()));
	else
		return 0;
}

/** ���������� ������������ �������� �� Y*/
double VAprinfoNormaWidthData::GetMaxY()
{
	if (!ydata.empty())
		return *(std::max_element(ydata.begin(), ydata.end()));
	else
		return 0;
}

/** ���������� ����������� �������� �� X*/
double VAprinfoNormaWidthData::GetMinX() 
{
	return xdata.at(0);
};

/** ���������� ������������ �������� �� X*/
double VAprinfoNormaWidthData::GetMaxX()
{
	return xdata.at(xdata.size()-1);
};

//--------------------------------------- ������ ������ VDequeData -----------------------------------

/** ���c�������*/
VDequeData::VDequeData() : min_y(DBL_MAX), max_y(0-DBL_MAX) {}

/** ���������� �����*/
void VDequeData::Put(double in_x, double in_y, uint64_t in_flags)
{
	data.push_back(VGraphPointData(in_x, in_y, 0, 0, L"", in_flags));
	min_y=std::min<double>(min_y, in_y);
	max_y=std::max<double>(max_y, in_y);
}

/** ���������� �����*/
void VDequeData::Put(double in_x, double in_size_x, double in_y, double in_size_y, const std::wstring& in_label, uint64_t in_flags)
{
	data.push_back(VGraphPointData(in_x, in_y, in_size_x, in_size_y, in_label, in_flags));
	min_y=std::min<double>(min_y, in_y);
	max_y=std::max<double>(max_y, in_y);
}

/** ��������� ������*/
void VDequeData::Clear()
{
	data.clear();
}

/** ���������� ������ ������*/
size_t VDequeData::GetCount() const
{	
	return data.size();
}

/** ���������� ���������� �� ��� X*/
double VDequeData::GetX(size_t in_index) const
{
	return data.at(in_index).x;
}

/** ���������� ���������� �� ��� Y*/
double VDequeData::GetY(size_t in_index) const
{
	return data.at(in_index).y;
}

/** ���������� ������ �� ��� X*/
double VDequeData::GetSizeX(size_t in_index) const
{
	return data.at(in_index).xsize;
}

/** ���������� ������ �� ��� Y*/
double VDequeData::GetSizeY(size_t in_index) const
{
	return data.at(in_index).ysize;
}

/** ���������� �����*/
std::wstring VDequeData::GetLabel(size_t in_index) const
{
	return data.at(in_index).label;
}

/** ���������� �����*/
uint64_t VDequeData::GetFlags(size_t in_index) const
{
	return data.at(in_index).flags;
}

/** ���������� ����������� �������� �� Y*/
double VDequeData::GetMinY()
{
	return min_y;	
}

/** ���������� ������������ �������� �� Y*/
double VDequeData::GetMaxY()
{
	return max_y;	
}

/** ���������� ����������� �������� �� X*/
double VDequeData::GetMinX() 
{	
	return data.front().x;
};

/** ���������� ������������ �������� �� X*/
double VDequeData::GetMaxX()
{
	return data.back().x;
};

//--------------------------------------- ������ ������ VVectorData -----------------------------------

/** ���c�������*/
VVectorData::VVectorData()
{
}

/** ������������� ������*/ 
void VVectorData::SetSize(size_t in_size)
{
	xdata.reserve(in_size);
	ydata.reserve(in_size);
	flags.reserve(in_size);

	xdata.resize(in_size);
	ydata.resize(in_size);
	flags.resize(in_size);
}

/** ���������� �����*/
void VVectorData::Set(size_t in_index, double in_x, double in_y, uint64_t in_flags)
{
	xdata.at(in_index)=in_x;
	ydata.at(in_index)=in_y;
	flags.at(in_index)=in_flags;
}

/** ��������� ������*/
void VVectorData::Clear()
{
	xdata.clear();
	ydata.clear();
	flags.clear();
}

/** ���������� ������ ������*/
size_t VVectorData::GetCount() const
{
	return xdata.size();
}

/** ���������� ���������� �� ��� X*/
double VVectorData::GetX(size_t in_index) const
{
	return xdata.at(in_index);
}

/** ���������� ���������� �� ��� Y*/
double VVectorData::GetY(size_t in_index) const
{
	return ydata.at(in_index);
}

/** ���������� �����*/
uint64_t VVectorData::GetFlags(size_t in_index) const
{
	return flags.at(in_index);
}

/** ���������� ����������� �������� �� Y*/
double VVectorData::GetMinY()
{
	if (!ydata.empty())
		return *(std::min_element(ydata.begin(), ydata.end()));
	else
		return 0;
}

/** ���������� ������������ �������� �� Y*/
double VVectorData::GetMaxY()
{
	if (!ydata.empty())
		return *(std::max_element(ydata.begin(), ydata.end()));
	else
		return 0;
}

/** ���������� ����������� �������� �� X*/
double VVectorData::GetMinX() 
{
	return xdata.front();
};

/** ���������� ������������ �������� �� X*/
double VVectorData::GetMaxX()
{
	return xdata.back();
};

//-------------------------------------- ������ ������ VUserTextProvider ----------------------------------------

/** ���������� ����� �����*/
size_t VUserTextProvider::GetCount() const
{
	return items.size();
}

/** ���������� ������*/
const std::wstring& VUserTextProvider::GetItem(size_t in_index) const
{
	return items.at(in_index);
}

/** ��������� ������*/
void VUserTextProvider::AddItem(const std::wstring& in_line)
{
	items.push_back(in_line);
}

/** ������� ��� ������*/ 
void VUserTextProvider::Clear()
{
	items.clear();
}

//------------------------------------ ������ ������ VJaggyProfileOverstepData ------------------------

/// �����������
VJaggyProfileOverstepData::VJaggyProfileOverstepData(VDataSetPtr in_data_set)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_set->Load(PRM_JAGGY_PROFILE_OVERSTEP);		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/// ���������� ������ ������
size_t VJaggyProfileOverstepData::GetCount() const
{
	return data_set->GetJaggyProfileOversteps().GetCount();
}

/// ���������� ���������� �� ��� X*/
double VJaggyProfileOverstepData::GetX(size_t in_index) const
{
	return data_set->GetJaggyProfileOversteps().GetItem(in_index)->GetStart();
}

/// ���������� ������ �� ��� X
double VJaggyProfileOverstepData::GetSizeX(size_t in_index) const
{
	return data_set->GetJaggyProfileOversteps().GetItem(in_index)->GetLength();
}

/// ���������� ���������� �� ��� Y
double VJaggyProfileOverstepData::GetY(size_t in_index) const
{
	return 0;
}

/// ���������� ������ �� ��� Y
double VJaggyProfileOverstepData::GetSizeY(size_t in_index) const
{
	return data_set->GetJaggyProfileOversteps().GetItem(in_index)->GetValue();
}

/// ���������� ����������� �������� �� Y
double VJaggyProfileOverstepData::GetMinY()
{
	return 0;
}

/// ���������� ������������ �������� �� Y
double VJaggyProfileOverstepData::GetMaxY()
{
	return 0;
}

/// ���������� ����������� �������� �� X
double VJaggyProfileOverstepData::GetMinX()
{
	return 0;
}

/// ���������� ������������ �������� �� X
double VJaggyProfileOverstepData::GetMaxX()
{	
	return 0;
}

/// ���������� �����
std::wstring VJaggyProfileOverstepData::GetLabel(size_t in_index) const
{
	return data_set->GetJaggyProfileOversteps().GetItem(in_index)->GetDescription();
}

//------------------------------------ ������ ������ VJaggyPlanOverstepData ------------------------

/// �����������
VJaggyPlanOverstepData::VJaggyPlanOverstepData(VDataSetPtr in_data_set)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_set->Load(PRM_JAGGY_PLAN_OVERSTEP);		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/// ���������� ������ ������
size_t VJaggyPlanOverstepData::GetCount() const
{
	return data_set->GetJaggyPlanOversteps().GetCount();
}

/// ���������� ���������� �� ��� X*/
double VJaggyPlanOverstepData::GetX(size_t in_index) const
{
	return data_set->GetJaggyPlanOversteps().GetItem(in_index)->GetStart();
}

/// ���������� ������ �� ��� X
double VJaggyPlanOverstepData::GetSizeX(size_t in_index) const
{
	return data_set->GetJaggyPlanOversteps().GetItem(in_index)->GetLength();
}

/// ���������� ���������� �� ��� Y
double VJaggyPlanOverstepData::GetY(size_t in_index) const
{
	return 0;
}

/// ���������� ������ �� ��� Y
double VJaggyPlanOverstepData::GetSizeY(size_t in_index) const
{
	return data_set->GetJaggyPlanOversteps().GetItem(in_index)->GetValue();
}

/// ���������� ����������� �������� �� Y
double VJaggyPlanOverstepData::GetMinY()
{
	return 0;
}

/// ���������� ������������ �������� �� Y
double VJaggyPlanOverstepData::GetMaxY()
{
	return 0;
}

/// ���������� ����������� �������� �� X
double VJaggyPlanOverstepData::GetMinX()
{
	return 0;
}

/// ���������� ������������ �������� �� X
double VJaggyPlanOverstepData::GetMaxX()
{	
	return 0;
}

/// ���������� �����
std::wstring VJaggyPlanOverstepData::GetLabel(size_t in_index) const
{
	return data_set->GetJaggyPlanOversteps().GetItem(in_index)->GetDescription();
}


//------------------------------------ ������ ������ VDiffWidthFrontOverstepData ------------------------

/// �����������
VDiffWidthFrontOverstepData::VDiffWidthFrontOverstepData(VDataSetPtr in_data_set)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_set->Load(PRM_DIFF_WIDTH_FRONT_OVERSTEP);		
	}
	else
	{
		throw NULL_POINTER;
	}
}

/// ���������� ������ ������
size_t VDiffWidthFrontOverstepData::GetCount() const
{
	return data_set->GetDiffWidthFrontOversteps().GetCount();
}

/// ���������� ���������� �� ��� X*/
double VDiffWidthFrontOverstepData::GetX(size_t in_index) const
{
	return data_set->GetDiffWidthFrontOversteps().GetItem(in_index)->GetStart();
}

/// ���������� ������ �� ��� X
double VDiffWidthFrontOverstepData::GetSizeX(size_t in_index) const
{
	return data_set->GetDiffWidthFrontOversteps().GetItem(in_index)->GetLength();
}

/// ���������� ���������� �� ��� Y
double VDiffWidthFrontOverstepData::GetY(size_t in_index) const
{
	return data_set->GetDiffWidthFrontOversteps().GetItem(in_index)->GetMin();
}

/// ���������� ������ �� ��� Y
double VDiffWidthFrontOverstepData::GetSizeY(size_t in_index) const
{
	return data_set->GetDiffWidthFrontOversteps().GetItem(in_index)->GetMax()-
		   data_set->GetDiffWidthFrontOversteps().GetItem(in_index)->GetMin();
}

/// ���������� ����������� �������� �� Y
double VDiffWidthFrontOverstepData::GetMinY()
{
	return 0;
}

/// ���������� ������������ �������� �� Y
double VDiffWidthFrontOverstepData::GetMaxY()
{
	return 0;
}

/// ���������� ����������� �������� �� X
double VDiffWidthFrontOverstepData::GetMinX()
{
	return 0;
}

/// ���������� ������������ �������� �� X
double VDiffWidthFrontOverstepData::GetMaxX()
{	
	return 0;
}

/// ���������� �����
std::wstring VDiffWidthFrontOverstepData::GetLabel(size_t in_index) const
{
	return data_set->GetDiffWidthFrontOversteps().GetItem(in_index)->GetDescription();
}


//------------------------------------ ������ ������ VDiffWidthBackOverstepData ------------------------

/// �����������
VDiffWidthBackOverstepData::VDiffWidthBackOverstepData(VDataSetPtr in_data_set)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		data_set->Load(PRM_DIFF_WIDTH_BACK_OVERSTEP);				
	}
	else
	{
		throw NULL_POINTER;
	}
}

/// ���������� ������ ������
size_t VDiffWidthBackOverstepData::GetCount() const
{
	return data_set->GetDiffWidthBackOversteps().GetCount();
}

/// ���������� ���������� �� ��� X*/
double VDiffWidthBackOverstepData::GetX(size_t in_index) const
{
	return data_set->GetDiffWidthBackOversteps().GetItem(in_index)->GetStart();
}

/// ���������� ������ �� ��� X
double VDiffWidthBackOverstepData::GetSizeX(size_t in_index) const
{
	return data_set->GetDiffWidthBackOversteps().GetItem(in_index)->GetLength();
}

/// ���������� ���������� �� ��� Y
double VDiffWidthBackOverstepData::GetY(size_t in_index) const
{
	return data_set->GetDiffWidthBackOversteps().GetItem(in_index)->GetMin();
}

/// ���������� ������ �� ��� Y
double VDiffWidthBackOverstepData::GetSizeY(size_t in_index) const
{
	return data_set->GetDiffWidthBackOversteps().GetItem(in_index)->GetMax()-
		   data_set->GetDiffWidthBackOversteps().GetItem(in_index)->GetMin();
}

/// ���������� ����������� �������� �� Y
double VDiffWidthBackOverstepData::GetMinY()
{
	return 0;
}

/// ���������� ������������ �������� �� Y
double VDiffWidthBackOverstepData::GetMaxY()
{
	return 0;
}

/// ���������� ����������� �������� �� X
double VDiffWidthBackOverstepData::GetMinX()
{
	return 0;
}

/// ���������� ������������ �������� �� X
double VDiffWidthBackOverstepData::GetMaxX()
{	
	return 0;
}

/// ���������� �����
std::wstring VDiffWidthBackOverstepData::GetLabel(size_t in_index) const
{
	return data_set->GetDiffWidthBackOversteps().GetItem(in_index)->GetDescription();
}


//------------------------------------ ������ ������ VCantingOverstepData ------------------------

/// �����������
VCantingOverstepData::VCantingOverstepData(VDataSetPtr in_data_set, side_t in_side)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		side=in_side;
		data_set->Load(side==SD_LEFT ? PRM_CANTING_LEFT_OVERSTEP : PRM_CANTING_RIGHT_OVERSTEP);				
	}
	else
	{
		throw NULL_POINTER;
	}
}

/// ���������� ������ ������
size_t VCantingOverstepData::GetCount() const
{
	return data_set->GetCantingOversteps(side).GetCount();
}

/// ���������� ���������� �� ��� X*/
double VCantingOverstepData::GetX(size_t in_index) const
{
	return data_set->GetCantingOversteps(side).GetItem(in_index)->GetStart();
}

/// ���������� ������ �� ��� X
double VCantingOverstepData::GetSizeX(size_t in_index) const
{
	return data_set->GetCantingOversteps(side).GetItem(in_index)->GetLength();
}

/// ���������� ���������� �� ��� Y
double VCantingOverstepData::GetY(size_t in_index) const
{
	return data_set->GetCantingOversteps(side).GetItem(in_index)->GetMin();
}

/// ���������� ������ �� ��� Y
double VCantingOverstepData::GetSizeY(size_t in_index) const
{
	return data_set->GetCantingOversteps(side).GetItem(in_index)->GetMax()-
		   data_set->GetCantingOversteps(side).GetItem(in_index)->GetMin();
}

/// ���������� ����������� �������� �� Y
double VCantingOverstepData::GetMinY()
{
	return 0;
}

/// ���������� ������������ �������� �� Y
double VCantingOverstepData::GetMaxY()
{
	return 0;
}

/// ���������� ����������� �������� �� X
double VCantingOverstepData::GetMinX()
{
	return 0;
}

/// ���������� ������������ �������� �� X
double VCantingOverstepData::GetMaxX()
{	
	return 0;
}

/// ���������� �����
std::wstring VCantingOverstepData::GetLabel(size_t in_index) const
{
	return data_set->GetCantingOversteps(side).GetItem(in_index)->GetDescription();
}


//------------------------------------ ������ ������ VExtWearOverstepData ------------------------

/// �����������
VExtWearOverstepData::VExtWearOverstepData(VDataSetPtr in_data_set, side_t in_side)
{
	if (in_data_set)
	{
		data_set=in_data_set;
		side=in_side;
		data_set->Load(side==SD_LEFT ? PRM_EXT_WEAR_LEFT_OVERSTEP : PRM_EXT_WEAR_RIGHT_OVERSTEP);				
	}
	else
	{
		throw NULL_POINTER;
	}
}

/// ���������� ������ ������
size_t VExtWearOverstepData::GetCount() const
{
	return data_set->GetExtWearOversteps(side).GetCount();
}

/// ���������� ���������� �� ��� X*/
double VExtWearOverstepData::GetX(size_t in_index) const
{
	return data_set->GetExtWearOversteps(side).GetItem(in_index)->GetStart();
}

/// ���������� ������ �� ��� X
double VExtWearOverstepData::GetSizeX(size_t in_index) const
{
	return data_set->GetExtWearOversteps(side).GetItem(in_index)->GetLength();
}

/// ���������� ���������� �� ��� Y
double VExtWearOverstepData::GetY(size_t in_index) const
{
	return data_set->GetExtWearOversteps(side).GetItem(in_index)->GetMin();
}

/// ���������� ������ �� ��� Y
double VExtWearOverstepData::GetSizeY(size_t in_index) const
{
	return data_set->GetExtWearOversteps(side).GetItem(in_index)->GetMax()-
		   data_set->GetExtWearOversteps(side).GetItem(in_index)->GetMin();
}

/// ���������� ����������� �������� �� Y
double VExtWearOverstepData::GetMinY()
{
	return 0;
}

/// ���������� ������������ �������� �� Y
double VExtWearOverstepData::GetMaxY()
{
	return 0;
}

/// ���������� ����������� �������� �� X
double VExtWearOverstepData::GetMinX()
{
	return 0;
}

/// ���������� ������������ �������� �� X
double VExtWearOverstepData::GetMaxX()
{	
	return 0;
}

/// ���������� �����
std::wstring VExtWearOverstepData::GetLabel(size_t in_index) const
{
	return data_set->GetExtWearOversteps(side).GetItem(in_index)->GetDescription();
}
