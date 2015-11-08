#include "VCalcOptions.h"

/// ��/���
extern const std::wstring OPTION_YES(L"��");
extern const std::wstring OPTION_NO(L"���");

/// ���������� ����� ����������� ��-515 ������������ ��� � ������������
extern const std::wstring OPTION_515(L"��-515");
extern const std::wstring OPTION_EXPERIMENT(L"�����������");

/// ���������� ����� ���������� ������
extern const std::wstring OPTION_PASSPORT(L"�������");
extern const std::wstring OPTION_FACT(L"����");

///������ ������
extern const std::wstring OPTION_PRINT_ALL(L"���");
extern const std::wstring OPTION_PRINT_OVERSTEPS(L"������ � �������������");
extern const std::wstring OPTION_PRINT_RESTRICTIONS(L"������ � ������������� ��������");

/// �������� ��������
extern const std::wstring OPTION_CLOSE(L"��������");
extern const std::wstring OPTION_RESTRICT_25(L"25 ��/� �� �������� ��������");

///�������� �����
extern const std::wstring OPTION_SCALE1(L"��������");
extern const std::wstring OPTION_SCALE2(L"�������������� 1");
extern const std::wstring OPTION_SCALE3(L"�������������� 2");

/// ��������� ���
extern const std::wstring OPTION_PPR_PC1(L"����-1");
extern const std::wstring OPTION_PPR_PPR(L"����-���");

/// ��������� ������ ��� ������� � ������
extern const std::wstring OPTION_STD(L"��������");
extern const std::wstring OPTION_PPR(L"���");
extern const std::wstring OPTION_FRONT(L"�������");
extern const std::wstring OPTION_BACK(L"�����");

/// �����������
VOption::VOption(){};

/// �����������
VOption::VOption(const std::wstring& in_name, const std::wstring& in_value, const std::wstring& in_group, 
	const std::wstring& in_description) : 
	name(in_name), value(in_value), default_value(in_value), group(in_group), description(in_description) 
{
	hash=FNV32(name);
};

/// �����������
VOption::VOption(const std::wstring& in_name, const std::wstring& in_value, const std::wstring& in_group,
	const std::wstring& in_description, const std::vector<std::wstring>& in_avail_list) : 
	name(in_name), value(in_value), default_value(in_value), group(in_group), description(in_description), avail_list(in_avail_list) 
{
	hash=FNV32(name);
};

/// �����������
VOption::VOption(const std::wstring& in_name, const std::wstring& in_value, const std::wstring& in_group,
	const std::wstring& in_description, const std::wstring& in_avail_list) : 
	name(in_name), value(in_value), default_value(in_value), group(in_group), description(in_description)
{
	hash=FNV32(name);
	boost::algorithm::split(avail_list, in_avail_list, boost::is_any_of(L";"));
};

/// ���������� ��������
const std::wstring& VOption::GetName() const 
{
	return name;
};

/// ���������� ������
const std::wstring& VOption::GetGroup() const 
{
	return group;
};

/// ���������� ��������
const std::wstring& VOption::GetDescription() const 
{
	return description;
};

/// ������������� ��������
void VOption::SetValue(const std::wstring& in_value)
{
	if (GetAvailCount()==0)
		value=in_value;
	else
	{
		std::vector<std::wstring>::iterator i=std::find(avail_list.begin(), avail_list.end(), in_value);
		if (i==avail_list.end())
			value=avail_list.front();
		else
			value=in_value;
	}
};

/// ������������� �������� �� ���������		
void VOption::ResetValue()
{
	value=default_value;	
};

/// ���������� true, ���� �������� ����� ������������ ��������
bool VOption::Compare(const std::wstring& in_value) const 
{
	return value.compare(in_value)==0;
};

/// ��������� ��������� ��������
void VOption::AddAvail(const std::wstring& in_avail)
{
	avail_list.push_back(in_avail);
};

/// ���������� ����� ��������� ��������
size_t VOption::GetAvailCount() const 
{
	return avail_list.size();
};

/// ���������� ��������� ��������
const std::wstring& VOption::GetAvail(size_t in_index) const 
{
	return avail_list.at(in_index);
};

/// ���������� ���
uint32_t VOption::GetHash() const
{
	return hash;
}

//------------------------------- ������ ������ VOptions -------------------------------------------

/** �������� ������*/
void VOptions::Load(const std::wstring& in_filename)
{
	filename=in_filename;
	std::ifstream file(filename.c_str());
	if (file.is_open())
	{
		do
		{			
			//��������
			std::strstreambuf _description;
			file.get(_description);							
			std::string description=std::string(_description.str(), file.gcount());
			file.get();
			//��������
			std::strstreambuf _value;
			file.get(_value);							
			std::string value=std::string(_value.str(), file.gcount());
			file.get();
			if (value.compare("")!=0)
			{
				std::vector<std::string> parts;
				boost::algorithm::split(parts, value, boost::is_any_of("="));
				if (HasOption(string_to_wstring(parts.front())))
				{
					const VOption oldopt = GetOption(string_to_wstring(parts.front()));
					VOption newopt(oldopt);
					newopt.SetValue(string_to_wstring(parts.back()));
					options.erase(oldopt);
					options.insert(newopt);
					//GetOption(string_to_wstring(parts.front())).SetValue(string_to_wstring(parts.back()));
				}
			}
		} while (!file.eof());
	}
};

/** ������ �����*/
void VOptions::Save()
{	
	std::ofstream file(filename.c_str());
	for (std::set<VOption>::iterator i=options.begin(); i!=options.end(); ++i)
	{
		VOption _opt(*i);
		file << _opt;
	}
};

/** ���������� ����� ����������*/
size_t VOptions::GetOptionsCount() const 
{
	return options.size();
};

/** ���������� ���������� �� �������*/
const VOption& VOptions::GetOption(size_t in_index)
{
	size_t j=0;
	std::set<VOption>::iterator i=options.begin();
	while (j++<in_index)
		++i;
	return *i;
};	

/** ���������� true ���� ����� ������������ � ������*/
bool VOptions::HasOption(const std::wstring& in_name) const
{
	uint32_t hash=FNV32(in_name);
	std::set<VOption>::const_iterator i=std::find_if(options.begin(), options.end(), find_option_by_hash(hash));
	return i!=options.end();			
};

/** ���������� true ���� ����� ������������ � ������*/
bool VOptions::HasOption(const std::wstring& in_group, const std::wstring& in_description) const
{
	std::set<VOption>::const_iterator i=std::find_if(options.begin(), options.end(), find_option_by_description(in_group, in_description));
	return i!=options.end();			
};

/** ���������� ���������� �� �� �����*/	
const VOption& VOptions::GetOption(const std::wstring& in_name)
{			
	uint32_t hash=FNV32(in_name);
	std::set<VOption>::iterator i=std::find_if(options.begin(), options.end(), find_option_by_hash(hash));
	if (i!=options.end())
		return *i;
	else
		throw VSimpleException(L"����������� ���������� ����������� � ������.", in_name);
};

/** ���������� ���������� �� ������ � ��������*/
const  VOption& VOptions::GetOption(const std::wstring& in_group, const std::wstring& in_description)
{
	std::set<VOption>::iterator i=std::find_if(options.begin(), options.end(), find_option_by_description(in_group, in_description));
	if (i!=options.end())
		return *i;
	else
		throw VSimpleException(L"����������� ���������� ����������� � ������.", in_description);
};

/** ���������� ���������� �� �� �����*/	
const VOption& VOptions::GetOption(const std::wstring& in_name) const
{	
	uint32_t hash=FNV32(in_name);
	std::set<VOption>::const_iterator i=std::find_if(options.begin(), options.end(), find_option_by_hash(hash));
	if (i!=options.end())
		return *i;
	else
		throw VSimpleException(L"����������� ���������� ����������� � ������.", in_name);
};

/** ���������� ���������� �� ������ � ��������*/
const VOption& VOptions::GetOption(const std::wstring& in_group, const std::wstring& in_description) const
{
	std::set<VOption>::const_iterator i=std::find_if(options.begin(), options.end(), find_option_by_description(in_group, in_description));
	if (i!=options.end())
		return *i;
	else
		throw VSimpleException(L"����������� ���������� ����������� � ������.", in_description);
};

/** ���������� ����� �����*/
size_t VOptions::GetGroupCount() const 
{
	return groups.size();
};

/** ���������� ������*/
const std::wstring& VOptions::GetGroup(size_t in_index) const 
{
	return groups.at(in_index);
};

//------------------------------ ������ ������ VCalcOptions -----------------------------------------

/** �����������*/
VCalcOptions::VCalcOptions()
{
	InitOptions();
};

/** �����������*/
VCalcOptions::VCalcOptions(const std::wstring& in_filename)
{
	InitOptions();
	Load(in_filename);
};

/** ����������*/
VCalcOptions::~VCalcOptions()
{
	Save();
};

/** ������������� �����*/
void VCalcOptions::InitOptions()
{		
	//������
	groups.push_back(L"����");
	
	options.insert(VOption(L"MIN_CURVE_ANGLE", L"40", 
		L"����", L"����������� ���� �������� ������ (���.)"));
	options.insert(VOption(L"MIN_CURVE_LENGTH", L"30", 
		L"����", L"����������� ����� ������ (�)"));
	options.insert(VOption(L"MAX_PLAN_OFFSET", L"15", 
		L"����", L"������������ ���������� �� �������� (��)"));
	options.insert(VOption(L"MIN_LENGTH_TRANS", L"5", 
		L"����", L"����������� ����� ���������� (�)"));
	options.insert(VOption(L"MIN_LENGTH_CURVE", L"5", 
		L"����", L"����������� ����� �������� (�)"));
	options.insert(VOption(L"BEZIER_ON_PLAN", OPTION_YES, 
		L"����", L"���������� �������� ������� �����")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"MIN_LEVEL", L"5", 
		L"����", L"����������� ���������� �� ������ (��)"));
	options.insert(VOption(L"JAGGY_LINES_ONLY", OPTION_NO, 
		L"����", L"��������� ���������� ������ �� ������")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"LEVEL_QUANTUM", L"5", 
		L"����", L"��� ��������� ���������� (��)"));
//	options.insert(VOption(L"CURVE_ANP_RANGES", OPTION_46_15,
//		L"����", L"������ ������������� ���������")[OPTION_46_15][OPTION_EXPERIMENT]);
	//�������
	groups.push_back(L"�������");
	options.insert(VOption(L"MIN_PROFILE_ELEMENT_LENGTH", L"200", 
		L"�������", L"����������� ����� �������� (�)"));
	options.insert(VOption(L"MAX_PROFILE_OFFSET", L"150", 
		L"�������", L"������������ ���������� �� �������� (��)"));	
	options.insert(VOption(L"DROP_MARKS_ON_PROFILE", OPTION_YES, 
		L"�������", L"����������� ����� � �������")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"BEZIER_ON_PROFILE", OPTION_YES, 
		L"�������", L"���������� �������� ������� �����")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"MAX_DISTANCE_BTW_MARKS", L"5000", 
		L"�������", L"������������ ���������� ����� �������� ��������� ������� (�)"));
	//����������
	groups.push_back(L"������ �������");
	options.insert(VOption(L"LEVEL_MEAN_BASE", L"48", 
		L"������ �������", L"���� ������� �������� ���������� (�)"));
	options.insert(VOption(L"CURV_MEAN_BASE", L"48", 
		L"������ �������", L"���� ������� ������� �������� (�)"));
	options.insert(VOption(L"WIDTH_MEAN_BASE", L"48", 
		L"������ �������", L"���� ������� ������� �� (�)"));
	//�������
	groups.push_back(L"�������");
	options.insert(VOption(L"MAX_DIFF_WIDTH", L"3", 
		L"�������", L"������������ �������� �� � ��������� � ��� (��)"));
	//�������� ������
	groups.push_back(L"�������� ������");
	options.insert(VOption(L"CURVE_CARD_SHOW_CURV_MEAN", OPTION_YES, 
		L"�������� ������", L"�������� ������� �������� �� �������")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"CURVE_CARD_SHOW_LEVEL_MEAN", OPTION_YES, 
		L"�������� ������", L"�������� ������� ������� �� �������")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"CURVE_CARD_SHOW_CURV", OPTION_NO, 
		L"�������� ������", L"�������� �������� �� �������")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"CURVE_CARD_SHOW_LEVEL", OPTION_NO, 
		L"�������� ������", L"�������� ������� �� �������")[OPTION_YES][OPTION_NO]);		
	//�����
	groups.push_back(L"�����");
	options.insert(VOption(L"USE_NTD", OPTION_YES, 
		L"�����", L"��������� ��� ��� ������ ������")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"EXT_WEAR_THRESHOLD", L"15", 
		L"�����", L"����� ������ ��������� ����� ������"));	
	options.insert(VOption(L"WEAR_SOURCE", OPTION_STD,
		L"�����", L"�������� �������")[OPTION_STD][OPTION_PPR]);
	//�������� �����			
	groups.push_back(L"�������� �����");	
	options.insert(VOption(L"PLAN_MARKS_COORD_CORRECTION", OPTION_NO, 
		L"�������� �����", L"�������������� �������� ������ ��� ��������� ����������")[OPTION_YES][OPTION_NO]);	
	options.insert(VOption(L"PROFILE_MARKS_COORD_CORRECTION", OPTION_NO,
		L"�������� �����", L"�������������� �������� ������� ��� ��������� ����������")[OPTION_YES][OPTION_NO]);
	//������������
	groups.push_back(L"������������");
	options.insert(VOption(L"ZEROLINE_ON_FILE_CURVE_MARKS", OPTION_YES,
		L"������������", L"������� ������� ����� �� ������ �� �����")[OPTION_YES][OPTION_NO]);
	//���������� ����-�������
	groups.push_back(L"���������� ����-�������");
	options.insert(VOption(L"COORD_SHIFT_VSPD_PLUS", L"0", L"���������� ����-�������",
	    L"�������� ���������� ��� ������������� �������� ������ (�)"));
	options.insert(VOption(L"COORD_SHIFT_VSPD_MINUS", L"0", L"���������� ����-�������",
	    L"�������� ���������� ��� ������������� �������� ������ (�)"));
}

//----------------------------------------- ������ ������ VDetectOptions --------------------------------------

/** ������������� �����*/
void VDetectOptions::InitOptions()
{				
	groups.push_back(L"������ �����������");
		options.insert(VOption(L"INSTRUCTION", OPTION_515, 
			L"������ �����������", L"����������")[OPTION_515][OPTION_EXPERIMENT]);
		options.insert(VOption(L"CURVE_INSTRUCTION", OPTION_YES, 
			L"������ �����������", L"������ ������")[OPTION_YES][OPTION_NO]);			
		options.insert(VOption(L"PASSPORT", OPTION_PASSPORT, 
			L"������ �����������", L"������� �����")[OPTION_PASSPORT][OPTION_FACT]);
		options.insert(VOption(L"WEAR", OPTION_PASSPORT, 
			L"������ �����������", L"����� � ������")[OPTION_PASSPORT][OPTION_FACT]);
		options.insert(VOption(L"DISABLE_ALL_DETECTORS", OPTION_NO,
			L"������ �����������", L"��������� ������ ���� �����������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"ADJUST_ZERO_LINES", OPTION_YES,
			L"������ �����������", L"�������� ������� ����� �� �����")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"DISABLE_WEAR_DETECTOR_ON_RADIUS_3000", OPTION_YES,
			L"������ �����������", L"�� ��������� ����� �� ������ �������� ����� 3000 �")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"LINE_CLOSURE_POLICY", OPTION_CLOSE,
			L"������ �����������", L"�������� ��������")[OPTION_CLOSE][OPTION_RESTRICT_25]);
	groups.push_back(L"����������� ���������");			
		options.insert(VOption(L"HIDE_PASSPORT_LINE", OPTION_NO,
			L"����������� ���������", L"������ ���������� ����� ��� ������� �� �����")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"PRINT_POLICY_GEOMETRY", OPTION_PRINT_ALL,
			L"����������� ���������", L"�������� ����� ���")[OPTION_PRINT_ALL][OPTION_PRINT_OVERSTEPS][OPTION_PRINT_RESTRICTIONS]);
		options.insert(VOption(L"PRINT_POLICY_EXTRA", OPTION_PRINT_ALL,
			L"����������� ���������", L"�������� ����� ���. ����������")[OPTION_PRINT_ALL][OPTION_PRINT_OVERSTEPS][OPTION_PRINT_RESTRICTIONS]);
		options.insert(VOption(L"CUT_DISTANTIONS", OPTION_NO,
			L"����������� ���������", L"������ �������� ��� ����� ���������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"ROUND_RADIUS_AND_LEVEL", OPTION_NO,
			L"����������� ���������", L"��������� ������� � ����������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"SHOW_PASSPORT_WEAR", OPTION_NO,
			L"����������� ���������", L"���������� ������ ����������� ������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"SHOW_FACT_WEAR", OPTION_NO,
			L"����������� ���������", L"���������� ������ ������������ ������ �� ����� ���")[OPTION_YES][OPTION_NO]);			
		options.insert(VOption(L"SCALE", OPTION_SCALE1,
			L"����������� ���������", L"�������")[OPTION_SCALE1][OPTION_SCALE2][OPTION_SCALE3]);
		options.insert(VOption(L"SPLIT_DIAGRAM", OPTION_NO,
		    L"����������� ���������", L"��������� ������ �������� � ��������� ����")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"SAVE_GEOMETRY", OPTION_YES,
		    L"����������� ���������", L"��������� � ���� ��������� ���")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"SAVE_EXTRA", OPTION_YES,
		    L"����������� ���������", L"��������� � ���� ��������� ���. ����������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"LEFT_MARGIN", L"20",
			L"����������� ���������", L"������ ����� (��)")[L"20"][L"15"][L"10"]);		    
	groups.push_back(L"������ ���������");
		options.insert(VOption(L"USE_PIT_LEFT", OPTION_YES, 
			L"������ ���������", L"�������� �����")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_PIT_RIGHT", OPTION_YES, 
			L"������ ���������", L"�������� ������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_WIDTH", OPTION_YES, 
			L"������ ���������", L"��")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_RICH_LEFT", OPTION_YES, 
			L"������ ���������", L"�������� �����")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_RICH_RIGHT", OPTION_YES, 
			L"������ ���������", L"�������� ������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_LEVEL", OPTION_YES, 
			L"������ ���������", L"�������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_WEAR_LEFT", OPTION_YES, 
			L"������ ���������", L"����� �����")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_WEAR_RIGHT", OPTION_YES, 
			L"������ ���������", L"����� ������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_ZAZOR_LEFT", OPTION_YES, 
			L"������ ���������", L"����� �����")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_ZAZOR_RIGHT", OPTION_YES, 
			L"������ ���������", L"����� ������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_MAG_ZAZOR_LEFT", OPTION_YES, 
			L"������ ���������", L"����� ����� (���������)")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_MAG_ZAZOR_RIGHT", OPTION_YES, 
			L"������ ���������", L"����� ������ (���������)")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"WIDTH_SOURCE", OPTION_STD, 
			L"������ ���������", L"�������� ��")[OPTION_STD][OPTION_FRONT][OPTION_BACK][OPTION_PPR]);
		options.insert(VOption(L"PPR_SOURCE", OPTION_PPR_PC1,
			L"������ ���������", L"�������� ���")[OPTION_PPR_PC1][OPTION_PPR_PPR]);
		options.insert(VOption(L"WEAR_SOURCE", OPTION_STD,
			L"������ ���������", L"�������� ������� ��� �����������")[OPTION_STD][OPTION_PPR]);
		options.insert(VOption(L"COPY_PPR_TO_WEAR", OPTION_NO,
			L"������ ���������", L"���������� ������ �� ��� � �������� ������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_RFID", OPTION_YES, 
			L"������ ���������", L"RFID �����")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_PPR", OPTION_YES, 
			L"������ ���������", L"���")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"CALCULATE_ZERO_ZAZOR", OPTION_YES, 
			L"������ ���������", L"���������� ������� ������")[OPTION_YES][OPTION_NO]);
	groups.push_back(L"�������� ��������");
		options.insert(VOption(L"ENABLE_COORDINATES_CORRECTION", OPTION_YES, 
			L"�������� ��������", L"�������� �������� ��������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"COORDINATES_CORRECTION_KM", OPTION_YES, 
			L"�������� ��������", L"���������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"COORDINATES_CORRECTION_CURVE", OPTION_YES, 
			L"�������� ��������", L"������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"COORDINATES_CORRECTION_RFID", OPTION_NO, 
			L"�������� ��������", L"����� RFID")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"MAX_SWITCH_CORRECTION_DISTANCE", L"30", 
			L"�������� ��������", L"����� ������������� ������� (�)"));
		options.insert(VOption(L"MARKER_COUNT", L"18", 
			L"�������� ��������", L"����������� ����� �������� ��� ��������"));		
		options.insert(VOption(L"SAVE_CORRECTED_COORDINATE", OPTION_NO, 
			L"�������� ��������", L"��������� ����������������� ����������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"DETECT_SIDEWAYS", OPTION_NO, 
			L"�������� ��������", L"������������� ������� �� ������� ����")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"COORDINATES_CORRECTION_MARKERS", OPTION_PASSPORT, 
			L"�������� ��������", L"�������� �������� ��� ��������")[OPTION_FACT][OPTION_PASSPORT]);
		options.insert(VOption(L"USE_SAVED_CORRECTION_SLOPE", OPTION_YES, 
			L"�������� ��������", L"������������ �������� �� ����������� �������")[OPTION_YES][OPTION_NO]);			
	groups.push_back(L"�������� ������");					
		options.insert(VOption(L"EXPRESS_WIDEN_THRESHOLD", L"1548.0", 
			L"�������� ������", L"����� ��������"));
		options.insert(VOption(L"EXPRESS_NARROW_THRESHOLD", L"1510.0", 
			L"�������� ������", L"����� �������"));
		options.insert(VOption(L"EXPRESS_WEAR_THRESHOLD", L"18.0", 
			L"�������� ������", L"����� ������"));
		options.insert(VOption(L"EXPRESS_ZAZOR_THRESHOLD", L"26.0", 
			L"�������� ������", L"����� ������"));
	groups.push_back(L"������ ���. ����������");
		options.insert(VOption(L"DETECT_WEAR", OPTION_YES, 
			L"������ ���. ����������", L"������ ������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"DETECT_ZAZOR", OPTION_YES, 
			L"������ ���. ����������", L"������ �������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"DETECT_ZERO_ZAZOR", OPTION_YES, 
			L"������ ���. ����������", L"������ ������ �������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_NTD", OPTION_YES, 
			L"������ ���. ����������", L"��������� ��� ��� ������ ������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_283p", OPTION_YES, 
			L"������ ���. ����������", L"��������� ������������ 283� ��� ������ ������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_P50_CORRECTION", OPTION_YES, 
			L"������ ���. ����������", L"������� �������� � �������� ������ �� ������� �50")[OPTION_YES][OPTION_NO]);
	groups.push_back(L"����������� �������");
		options.insert(VOption(L"SHOW_CONTROL_SECTION_SKO", OPTION_NO, 
			L"����������� �������", L"��������� ��� �� ����������� ��������")[OPTION_YES][OPTION_NO]);
	groups.push_back(L"�������� ��������");
		options.insert(VOption(L"RICH_RECALCULATION", OPTION_NO, 
			L"�������� ��������", L"�������� �������� ��������")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"FRONT_ARM_REAL", L"11.48", 
			L"�������� ��������", L"�������� ����� ����� (�����) (�)"));
		options.insert(VOption(L"BACK_ARM_REAL", L"4.55", 
			L"�������� ��������", L"������ ����� ����� (�����) (�)"));
		options.insert(VOption(L"FRONT_ARM_VIRTUAL", L"17.4", 
			L"�������� ��������", L"�������� ����� ����� (��������) (�)"));
		options.insert(VOption(L"BACK_ARM_VIRTUAL", L"4.1", 
			L"�������� ��������", L"������ ����� ����� (��������) (�)"));
		options.insert(VOption(L"RICH_RECALCULATION_PRECISION", L"2.5", 
			L"�������� ��������", L"�������� ��������� (��)"));
		options.insert(VOption(L"RICH_RECALCULATION_BUFFER", L"100.0", 
			L"�������� ��������", L"����� ������ (�)"));		
	groups.push_back(L"���������� ����� ����������");
		options.insert(VOption(L"WIDTH_CORRUPT_DETECTION", OPTION_NO, 
			L"���������� ����� ����������", L"���������� ����� �� � ���� ������������� �������� � �������")[OPTION_YES][OPTION_NO]);
	groups.push_back(L"��������� ���");
    	options.insert(VOption(L"VPS", L"", L"��������� ���", L"�������� ���"));
	    options.insert(VOption(L"CHIEF", L"", L"��������� ���", L"��������� �����"));
	    options.insert(VOption(L"APPEND_VPS_TO_FILENAME", OPTION_YES, L"��������� ���",
	        L"��������� �������� ��� � ����� �����")[OPTION_YES][OPTION_NO]);
}

/** �����������*/
VDetectOptions::VDetectOptions()
{
	InitOptions();		
}

/** �����������*/
VDetectOptions::VDetectOptions(const std::wstring& in_filename)
{
	InitOptions();
	Load(in_filename);
}

/** ����������*/
VDetectOptions::~VDetectOptions()
{		
	Save();
}

/// ���������� true, ���� ��������� ����� ����������� �� ��������
bool VDetectOptions::UsePassport() const
{
	return GetOption(L"PASSPORT").Compare(OPTION_PASSPORT);
}

/// ���������� true, ���� ������������ ���������� �����
bool VDetectOptions::UsePassportWear() const
{
	return GetOption(L"WEAR").Compare(OPTION_PASSPORT);
}

/// ������������� ����� "������� ������� ����� �� ��������"
void VDetectOptions::SetUsePassport(bool use)
{
	const VOption oldopt = GetOption(L"PASSPORT");
	VOption newopt(oldopt);
	newopt.SetValue(use ? OPTION_PASSPORT : OPTION_FACT);
	options.erase(oldopt);
	options.insert(newopt);
	//GetOption(L"PASSPORT").SetValue(use ? OPTION_PASSPORT : OPTION_FACT);	
}

void VDetectOptions::SetUsePassportWear(bool use)
{
	const VOption oldopt = GetOption(L"WEAR");
	VOption newopt(oldopt);
	newopt.SetValue(use ? OPTION_PASSPORT : OPTION_FACT);
	options.erase(oldopt);
	options.insert(newopt);
	//GetOption(L"WEAR").SetValue(use ? OPTION_PASSPORT : OPTION_FACT);	
}


/// ���������� true ���� �������� ����� ��������
bool VDetectOptions::UsePitLeft() const
{
	return GetOption(L"USE_PIT_LEFT").Compare(OPTION_YES);
}

/// ���������� true ���� �������� ������ ��������
bool VDetectOptions::UsePitRight() const
{
	return GetOption(L"USE_PIT_RIGHT").Compare(OPTION_YES);
}

/// ���������� true ���� �������� ��
bool VDetectOptions::UseWidth() const
{
	return GetOption(L"USE_WIDTH").Compare(OPTION_YES);
}

/// ���������� true ���� �������� ����� ��������
bool VDetectOptions::UseRichLeft() const
{
	return GetOption(L"USE_RICH_LEFT").Compare(OPTION_YES);
}

/// ���������� true ���� �������� ������ ��������
bool VDetectOptions::UseRichRight() const
{
	return GetOption(L"USE_RICH_RIGHT").Compare(OPTION_YES);
}

/// ���������� true ���� ������� �������
bool VDetectOptions::UseLevel() const
{
	return GetOption(L"USE_LEVEL").Compare(OPTION_YES);	
}

/// ���������� true ���� ������� ����� �����
bool VDetectOptions::UseWearLeft() const
{
	return GetOption(L"USE_WEAR_LEFT").Compare(OPTION_YES);
}

/// ���������� true ���� ������� ����� ������
bool VDetectOptions::UseWearRight() const
{
	return GetOption(L"USE_WEAR_RIGHT").Compare(OPTION_YES);
}

/// ���������� true ���� ������� ����� �����
bool VDetectOptions::UseZazorLeft() const
{
	return GetOption(L"USE_ZAZOR_LEFT").Compare(OPTION_YES);
}

/// ���������� true ���� ������� ����� ������
bool VDetectOptions::UseZazorRight() const
{
	return GetOption(L"USE_ZAZOR_RIGHT").Compare(OPTION_YES);
}

/// ���������� true ���� ������� ����� �����
bool VDetectOptions::UseMagZazorLeft() const
{
	return GetOption(L"USE_MAG_ZAZOR_LEFT").Compare(OPTION_YES);
}

/// ���������� true ���� ������� ����� ������
bool VDetectOptions::UseMagZazorRight() const
{
	return GetOption(L"USE_MAG_ZAZOR_RIGHT").Compare(OPTION_YES);
}

/// ���������� true ���� �������� ��������� �� ��
bool VDetectOptions::CoordinatesCorrectionKm() const
{
	return GetOption(L"COORDINATES_CORRECTION_KM").Compare(OPTION_YES);
}

/// ���������� true ���� �������� ��������� �� ������
bool VDetectOptions::CoordinatesCorrectionCurve() const
{
	return !GetOption(L"COORDINATES_CORRECTION_CURVE").Compare(OPTION_NO);
}

// ���������� true ���� �������� ��������� �� RFID
bool VDetectOptions::CoordinatesCorrectionRFID() const
{
	return !GetOption(L"COORDINATES_CORRECTION_RFID").Compare(OPTION_NO);
}

/// ���������� True ���� �������� ��������� ������� ����� �� ����������
bool VDetectOptions::CorrectCurves() const
{
	return GetOption(L"ADJUST_ZERO_LINES").Compare(OPTION_YES);
}

/// ���������� ����� �������� ������ ��������
float VDetectOptions::EXPRESS_WIDEN_THRESHOLD() const
{
	return GetOption(L"EXPRESS_WIDEN_THRESHOLD").GetValue<float>();
}

/// ���������� ����� �������� ������ �������
float VDetectOptions::EXPRESS_NARROW_THRESHOLD() const
{
	return GetOption(L"EXPRESS_NARROW_THRESHOLD").GetValue<float>();
}

/// ���������� ����� �������� ������ ������
float VDetectOptions::EXPRESS_WEAR_THRESHOLD() const
{
	return GetOption(L"EXPRESS_WEAR_THRESHOLD").GetValue<float>();
}

/// ���������� ����� �������� ������ �������
float VDetectOptions::EXPRESS_ZAZOR_THRESHOLD() const
{
	return GetOption(L"EXPRESS_ZAZOR_THRESHOLD").GetValue<float>();
}

/// ���������� true ���� ��������� ������ ���������� �����()
bool VDetectOptions::HIDE_PASSPORT_LINES() const
{
	return !UsePassport() && GetOption(L"HIDE_PASSPORT_LINE").Compare(OPTION_YES);
}

/// ���������� ������� �� ����������
double VDetectOptions::GetScaleCoord() const
{
	VOption option=GetOption(L"SCALE");
	if (option.Compare(OPTION_SCALE3))
	{
		return 40;
	}
	else if (option.Compare(OPTION_SCALE2))
	{
		return 20;
	}
	else
	{
		return 40;
	}
}

/// ���������� ������� ��������
double VDetectOptions::GetScalePit() const
{
	VOption option=GetOption(L"SCALE");
	if (option.Compare(OPTION_SCALE3))
	{
		return 10;
	}
	else if (option.Compare(OPTION_SCALE2))
	{
		return 10;
	}
	else
	{
		return 20;
	}
}

/// ���������� ������� ��
double VDetectOptions::GetScaleWidth() const
{
	VOption option=GetOption(L"SCALE");
	if (option.Compare(OPTION_SCALE3))
	{
		return 10; 
	}
	else if (option.Compare(OPTION_SCALE2))
	{
		return 10;
	}
	else
	{
		return 20;
	}
}

/// ���������� ������� ��������
double VDetectOptions::GetScaleRich() const
{
	VOption option=GetOption(L"SCALE");
	if (option.Compare(OPTION_SCALE3))
	{
		return 20;
	}
	else if (option.Compare(OPTION_SCALE2))
	{
		return 20;
	}
	else
	{
		return 40;
	}
}

/// ���������� ������� ������
double VDetectOptions::GetScaleLevel() const
{
	VOption option=GetOption(L"SCALE");
	if (option.Compare(OPTION_SCALE3))
	{
		return 40;
	}
	else if (option.Compare(OPTION_SCALE2))
	{
		return 20;
	}
	else
	{
		return 40;
	}
}