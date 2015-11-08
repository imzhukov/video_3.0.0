#pragma once

//stl
#include <strstream>
#include <memory>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>

//boost
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

#include "VException.h"
#include "mathlib.h"
#include "strlib.h"
#include "singleton.h"

#include "Shiny.h"

/// ��/���
extern const std::wstring OPTION_YES;
extern const std::wstring OPTION_NO;

/// ���������� ����� ����������� ��-515 ������������ ��� � ������������
extern const std::wstring OPTION_515;
extern const std::wstring OPTION_EXPERIMENT;

/// ���������� ����� ���������� ������
extern const std::wstring OPTION_PASSPORT;
extern const std::wstring OPTION_FACT;

///������ ������
extern const std::wstring OPTION_PRINT_ALL;
extern const std::wstring OPTION_PRINT_OVERSTEPS;
extern const std::wstring OPTION_PRINT_RESTRICTIONS;

/// �������� ��������
extern const std::wstring OPTION_CLOSE;
extern const std::wstring OPTION_RESTRICT_25;

///�������� �����
extern const std::wstring OPTION_SCALE1;
extern const std::wstring OPTION_SCALE2;
extern const std::wstring OPTION_SCALE3;

/// ��������� ���
extern const std::wstring OPTION_PPR_PC1;
extern const std::wstring OPTION_PPR_PPR;

/// ��������� ������ ��� ������� � ������
extern const std::wstring OPTION_STD;
extern const std::wstring OPTION_PPR;
extern const std::wstring OPTION_FRONT;
extern const std::wstring OPTION_BACK;

/// ����������
class VOption
{
private:
	/// ��������
	std::wstring name;
	/// ��������
	std::wstring value;
	/// �������� �� ���������
	std::wstring default_value;
	/// ������
	std::wstring group;
	/// ��������
	std::wstring description;	
	/// ������ ���������� ��������
	std::vector<std::wstring> avail_list;
	/// ���
	uint32_t hash;
public:
	/// �����������
	VOption();
	/// �����������
	VOption(const std::wstring& in_name, const std::wstring& in_value, const std::wstring& in_group,
		const std::wstring& in_description);
	/// �����������
	VOption(const std::wstring& in_name, const std::wstring& in_value, const std::wstring& in_group, 
		const std::wstring& in_description, const std::vector<std::wstring>& in_avail_list);
	/// �����������
	VOption(const std::wstring& in_name, const std::wstring& in_value, const std::wstring& in_group, 
		const std::wstring& in_description, const std::wstring& in_avail_list);
	/// ���������� ��������
	const std::wstring& GetName() const;	
	/// ���������� ��������
	template<class T> T GetValue() const
	{
		try
		{
			return boost::lexical_cast<T>(value);
		}
		catch (std::exception&)
		{
			std::wstring msg=boost::str(boost::wformat(L"%s %s") % description % value);
			throw VSimpleException(L"������������ �������� ���������: ", msg);
		}
	}
	/// ������������ GetValue ��� std::wstring 
	template<> std::wstring GetValue<std::wstring>() const
	{
		return value;	
	};
	/// ������������� GetValue
	std::wstring GetValue() const
	{
		return value;
	};
	/// ���������� ������
	const std::wstring& GetGroup() const;
	/// ���������� ��������
	const std::wstring& GetDescription() const;
	/// ������������� ��������
	void SetValue(const std::wstring& in_value);
	/// ������������� �������� �� ���������		
	void ResetValue();
	/// ���������� true, ���� �������� ����������� ���������
	bool Compare(const std::wstring& in_value) const;
	/// ��������� ��������� ��������
	void AddAvail(const std::wstring& in_avail);
	/// ���������� ����� ��������� ��������
	size_t GetAvailCount() const;
	/// ���������� ��������� ��������
	const std::wstring& GetAvail(size_t in_index) const;
	/// ���������� ���
	uint32_t GetHash() const;

	/// �������� ()
	VOption& operator[](const std::wstring& in_value)
	{
		AddAvail(in_value);
		return * this;
	}
	
	/// �������� <
	bool operator<(const VOption& in_option) const
	{
		return name.compare(in_option.name)<0;
	}
	
	/// �������� ==
	bool operator==(const VOption& in_option) const
	{
		return in_option.GetHash()==GetHash();
	}
	
	/// �������� <<
	friend std::ostream& operator<<(std::ostream& out, VOption& in_option)
	{
		out << "// " << wstring_to_string(in_option.GetGroup().c_str()) << " : " << wstring_to_string(in_option.GetDescription()).c_str();
		if (in_option.GetAvailCount()>0)
		{
			out << L" [";
			for (size_t i=0; i<in_option.GetAvailCount(); ++i)
			{
				if (i>0) out << L";";
				out << in_option.GetAvail(i);
			}
			out << L"]";
		}
		out << std::endl;
		out << wstring_to_string(in_option.GetName().c_str()) << "=" << 
			wstring_to_string(in_option.GetValue<std::wstring>()).c_str() << std::endl;
		return out;
	};
};

/// ����� �����
class VOptions
{
protected:

	// ������

	/// ��� �����
	std::wstring filename;
	/// ������ ����������
	std::set<VOption> options;
	/// ������
	std::vector<std::wstring> groups;
	/// ����� ����� �� �����
	class find_option_by_name
	{
	private:
		std::wstring name;
	public:
		find_option_by_name(const std::wstring& in_name) : name(in_name){};
		bool operator()(const VOption& in_option) const
		{		
			return in_option.GetName().compare(name)==0;
		}
	};
	/// ����� �� �����
	class find_option_by_hash
	{
	private:
		uint32_t hash;
	public:
		find_option_by_hash(uint32_t in_hash) : hash(in_hash){};
		bool operator()(const VOption& in_option) const
		{		
			return in_option.GetHash()==hash;
		}
	};
	/// ����� �� ������ � ��������
	class find_option_by_description
	{
	private:
		std::wstring group;
		std::wstring description;
	public:
		find_option_by_description(const std::wstring& in_group, const std::wstring& in_description) : 
		  group(in_group), description(in_description){};
		bool operator()(const VOption& in_option) const
		{
			return in_option.GetGroup().compare(group)==0 && 
				in_option.GetDescription().compare(description)==0;
		}
	};

public:
	/// �������� ������
	void Load(const std::wstring& in_filename);
	/// ������ �����
	void Save();
	/// ���������� ����� ����������
	size_t GetOptionsCount() const;
	/// ���������� ���������� �� �������
	const VOption& GetOption(size_t in_index);	
	/// ���������� true ���� ����� ������������ � ������
	bool HasOption(const std::wstring& in_name) const;
	/// ���������� true ���� ����� ������������ � ������
	bool HasOption(const std::wstring& in_group, const std::wstring& in_description) const;
	/// ���������� ���������� �� �� �����
	const VOption& GetOption(const std::wstring& in_name);
	/// ���������� ���������� �� ������ � ��������
	const VOption& GetOption(const std::wstring& in_group, const std::wstring& in_description);
	/// ���������� ���������� �� �� �����
	const VOption& GetOption(const std::wstring& in_name) const;
	/// ���������� ���������� �� ������ � ��������
	const VOption& GetOption(const std::wstring& in_group, const std::wstring& in_description) const ;
	/// ������� ���������� �� ������
	void DeleteOption(const VOption & opt)
	{
		options.erase(opt);
	}
	void AddOption(const VOption & opt)
	{
		options.insert(opt);
	}
	/// ���������� ����� �����
	size_t GetGroupCount() const;
	/// ���������� ������
	const std::wstring& GetGroup(size_t in_index) const;
};


/** ����� �� ������� "��������", ���������� ���������� ���������� ������� �������*/
class VCalcOptions  : public VOptions
{
private:		

	/** ������������� �����*/
	void InitOptions();	
		
public:	
	
	/** �����������*/
	VCalcOptions();
	/** �����������*/
	VCalcOptions(const std::wstring& in_filename);
	/** ����������*/
	~VCalcOptions();
		
	//������ ������������ ���������

	/** ����������� �������� ��� ������ ������*/
	double MIN_CURVE() const {return 5.0;};	
	/** ����������� ���� �������� ��� ������ ������ (40 �����)*/
	double MIN_ANGLE() const {return angle_t(0,40,0).get_rad();};	
	/** ���� ��� ������� ����������� ��� ������������ (�)*/
	double CURVE_BASE() const {return 32.0;};
	/** �������� ����������� ��� ������ ������*/
	double CURVE_DERIVED() const {return 0.01;};
	/** ����������� ����� ����������*/
	double MIN_LENGTH_TRANS() const {return std::max<double>(1.0, GetOption(L"MIN_LENGTH_TRANS").GetValue<double>());};
	/** ����������� ����� ��������*/
	double MIN_LENGTH_CURVE() const {return std::max<double>(1.0, GetOption(L"MIN_LENGTH_CURVE").GetValue<double>());};	
	/** ����������� �������� ���������� �� ������*/
	double MIN_LEVEL() const {return GetOption(L"MIN_LEVEL").GetValue<double>();};
	/** ������� ������������� ��������� ��� ��������������� �������*/
	double THRESHOLD_ANP_HIGH() const {return 1.0;};
	/** ������� ������������� ��������� ��� ���������������� ������� � �������� ������*/
	double THRESHOLD_ANP_HIGH_BT() const {return 1.5;};
	/** ������� ������������� ��������� ��� ������������ �������*/
	double THRESHOLD_ANP_PASS() const {return 0.7;};
	/** ������� ������������� ��������� ��� �������� �������*/
	double THRESHOLD_ANP_TRUCK() const {return 0.3;};
	/** �������� �� ���������*/
	double DEFAULT_SPEED() const {return 60;};

	pro::interval<size_t> SPEEDS_RANGE_1() const {return pro::interval<size_t>(141, 160);};
	pro::interval<size_t> SPEEDS_RANGE_2() const {return pro::interval<size_t>(161, 200);};
	pro::interval<size_t> SPEEDS_RANGE_3() const {return pro::interval<size_t>(201, 250);};

	// ������ - ��������

	/** ���������� ���� ���������� ��� ������� ������������� ��������� � ����������� �� ��������*/
	double ANP_BASE(int in_speed) const
	{
		if (in_speed>140)
		{
			return 60;
		}
		else 
		{
			return 50;
		}
	}
	
	/** ���� ������� �������� ���������� ������������� ��������� � ����������� �� ��������*/
	double PSI_BASE(int in_speed) const 
	{
		if (in_speed>140)
		{
			return 40;
		}
		else
		{
			return 30;
		}
	};
	
	/** ���� ������� ������ ������ ���������� � ����������� �� ��������*/
	double LEVEL_OTVOD_BASE(int in_speed) const 
	{
		if (in_speed>140)
		{
			return 40;
		}
		else
		{
			return 30;
		}
	};	

	/** ���������� ���� ���������� � ����������� �� ��������*/
	double JAGGY_BASE(int in_speed) const
	{
		if (SPEEDS_RANGE_1().contains(in_speed))
		{
			return 100;
		}
		else if (SPEEDS_RANGE_2().contains(in_speed))
		{
			return 120;
		}
		else if (SPEEDS_RANGE_3().contains(in_speed))
		{
			return 150;
		}
		else
		{
			return 80;
		}
	}
	/** ���������� ���� ������� �������� ������ ����������� � ����������� �� ��������*/
	double JAGGY_OTVOD_BASE(int in_speed)
	{
		if (SPEEDS_RANGE_1().contains(in_speed))
		{
			return 30;
		}
		else if (SPEEDS_RANGE_2().contains(in_speed))
		{
			return 40;
		}
		else if (SPEEDS_RANGE_3().contains(in_speed))
		{
			return 50;
		}
		else
		{
			return 30;
		}
	}		
};

typedef VSingleton<VCalcOptions> CALC_OPTIONS;
#define CALC_OPTIONS_INIT(options) CALC_OPTIONS::Instance().SetObject(options)
#define OPTIONS CALC_OPTIONS::Instance().GetObject()

/// ��������� � ���������
class VDetectOptions : public VOptions
{
private:	
	
	//������

	/// ������������� �����
	void InitOptions();		

public:	
	
	/// �����������
	VDetectOptions();
	/// �����������
	VDetectOptions(const std::wstring& in_filename);
	/// ����������
	~VDetectOptions();	

	
	/// ���������� true, ���� ��������� ����� ����������� �� ��������
	bool UsePassport() const;
	/// ���������� true, ���� ������������ ���������� �����
	bool UsePassportWear() const;

	/// ��������� ���������� �����������
	void SetUsePassport(bool use);
	void SetUsePassportWear(bool use);


	/// ���������� true ���� �������� ����� ��������
	bool UsePitLeft() const;
	/// ���������� true ���� �������� ������ ��������
	bool UsePitRight() const;
	/// ���������� true ���� �������� ��
	bool UseWidth() const;
	/// ���������� true ���� �������� ����� ��������
	bool UseRichLeft() const;
	/// ���������� true ���� �������� ������ ��������
	bool UseRichRight() const;
	/// ���������� true ���� ������� �������
	bool UseLevel() const;
	/// ���������� true ���� ������� ����� �����
	bool UseWearLeft() const;
	/// ���������� true ���� ������� ����� ������
	bool UseWearRight() const;
	/// ���������� true ���� ������� ����� �����
	bool UseZazorLeft() const;
	/// ���������� true ���� ������� ����� ������
	bool UseZazorRight() const;
	/// ���������� true ���� ������� ����� �����
	bool UseMagZazorLeft() const;
	/// ���������� true ���� ������� ����� ������
	bool UseMagZazorRight() const;

	/// ���������� true ���� �������� ��������� �� ��
	bool CoordinatesCorrectionKm() const;
	/// ���������� true ���� �������� ��������� �� ������
	bool CoordinatesCorrectionCurve() const;
	/// ���������� true ���� �������� ��������� �� RFID
	bool CoordinatesCorrectionRFID() const;
	
	/// ���������� True ���� �������� ��������� ������� ����� �� ����������
	bool CorrectCurves() const;
	
	/// ���������� ����� �������� ������ ��������
	float EXPRESS_WIDEN_THRESHOLD() const;
	/// ���������� ����� �������� ������ �������
	float EXPRESS_NARROW_THRESHOLD() const;
	/// ���������� ����� �������� ������ ������
	float EXPRESS_WEAR_THRESHOLD() const;
	/// ���������� ����� �������� ������ �������
	float EXPRESS_ZAZOR_THRESHOLD() const;
	
	/// ���������� true ���� ��������� ������ ���������� �����()
	bool HIDE_PASSPORT_LINES() const;
	
	/// ���������� ������� �� ����������
	double GetScaleCoord() const;
	/// ���������� ������� ��������
	double GetScalePit() const;
	/// ���������� ������� ��
	double GetScaleWidth() const;
	/// ���������� ������� ��������
	double GetScaleRich() const;
	/// ���������� ������� ������
	double GetScaleLevel() const;
};
typedef VSingleton<VDetectOptions> detect_options_t;
#define DETECT_OPTIONS_INIT(file_name) detect_options_t::Instance().SetObject(VDetectOptions(file_name))
#define DETECT_OPTIONS detect_options_t::Instance().GetObject() 

	