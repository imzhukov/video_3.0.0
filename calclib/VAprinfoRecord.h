#pragma once

#include <types.h>
#include <vector>
#include <string>
#include <mathlib.h>
#include <interval.h>

/// ���� ����
const long SPALA_GB = 0;	/// ��� (�� �����, �� �����...)
const long SPALA_GB96 = 1;	/// ��� �� 1996
const long SPALA_WOOD = 2;	/// ������
const long SPALA_DEFAULT = SPALA_WOOD; /// �������� �� ���������

/// ������� � �������� �����
const long SZO_NONE = 0;	/// ������
const long SZO_OBJECT = 1;	/// ������
const long SZO_SZONE = 2;	/// �������� ����

/// ���� �������
const long RAIL_NONE = 0;	/// �� �������
const long RAIL_R75 = 1;	/// �75
const long RAIL_R65 = 2;	/// �65
const long RAIL_R65K = 3;	/// �65�
const long RAIL_R50 = 4;	/// �50
const long RAIL_R43 = 5;	/// �43
const long RAIL_DEFAULT = RAIL_R65;

/// ���� ����������
const long BRACE_NONE = 0;
const long BRACE_K2 = 1;
const long BRACE_BP = 2;
const long BRACE_ZHB = 3;
const long BRACE_ZHBR = 4;
const long BRACE_ZHBR65 = 5;
const long BRACE_ZHBR65SH = 6;
const long BRACE_ARS = 7;
const long BRACE_PANDROL_350 = 8;
const long BRACE_D2 = 9;
const long BRACE_D4 = 10;
const long BRACE_KD = 11;
const long BRACE_DEFAULT = BRACE_K2;

#pragma pack (push,1)

/// ��������� ��� ������ � ��������� ���������� - ������� �������
struct VAprinfoBasicRecord
{
	double base, end; ///< �������

	VAprinfoBasicRecord() : base(0.0), end(0.0) {}

	// ����� �������
	double GetLen() const
	{
		return this->end - this->base;
	}

	// ������ ��� ��������
	virtual bool Fits() const
	{
		return true;
	}
};

bool operator < (const VAprinfoBasicRecord & r1, const VAprinfoBasicRecord & r2);

/// ��������� ��� ����� - ����������
struct VAprinfoBasicLabel
{
	double coord; ///< ����������

	VAprinfoBasicLabel() : coord(0.0) {};
	VAprinfoBasicLabel(double in_coord) : coord(in_coord) {};

	// ������ ��� ��������
	virtual bool Fits() const
	{
		return true;
	}
};

/// ������, ���������� ��������� "��-���"
struct VAprinfoObjectRecord : public VAprinfoBasicRecord
{
	bool isThere; ///< �������

	VAprinfoObjectRecord() : VAprinfoBasicRecord(),
		isThere(false) {}

	// ������ ��� ��������
	virtual bool Fits() const
	{
		return this->isThere;
	}
};

/// ������ � ����� ��������� 
struct VAprinfoIndexRecord : public VAprinfoBasicRecord
{
	long value; ///< ��������

	VAprinfoIndexRecord() : VAprinfoBasicRecord(), 
		value(0) {}

	// ������ ��� ��������
	virtual bool Fits() const
	{
		return this->value == SZO_OBJECT;
	}
};

/*
/// ������ � ��������� �������
struct VAprinfoDescrRecord : public VAprinfoBasicRecord
{
	std::string descr;

	VAprinfoDescrRecord() : VAprinfoBasicRecord(), descr(""
	{}
};
*/

/// �������� ���������
inline bool operator == (const VAprinfoIndexRecord & rec1, const VAprinfoIndexRecord & rec2)
{
	return rec1.value == rec2.value;
}

/// ��� �����
struct VAprinfoRailRecord : public VAprinfoIndexRecord
{
	std::string GetDescr() const;

	VAprinfoRailRecord() : VAprinfoIndexRecord()
	{}
};

/// ��� ����������
struct VAprinfoBraceRecord : public VAprinfoIndexRecord
{
	std::string GetDescr() const;

	VAprinfoBraceRecord() : VAprinfoIndexRecord()
	{}
};

/// ��������
struct VAprinfoKmRecord : public VAprinfoBasicRecord
{
	long km; ///< �����
	float len; ///< �����

	VAprinfoKmRecord() : VAprinfoBasicRecord(), 
		km(0), len(0) {}

	/// ���������� �����, �� ��������� ����������
	VWayCoord GetRealCoord(double absCoord) const;

	/// ���. ���������� ���������� �����
	double GetAbsCoord(float realM) const;

	/// ������ ������� �� ���������
	std::vector<VPicket> GetPkList() const;

	/// ���� �����
	int GetKmDirection() const
	{
		return len > 0 ? 1 : -1;
	}
};

/// �������� �� �������
struct VAprinfoSpeedRecord : public VAprinfoBasicRecord
{
	VSpeeds spd; ///< ��������

	VAprinfoSpeedRecord() : VAprinfoBasicRecord()
	{}
		
	/// ���������� ��������� �������������
	std::wstring format_short() const
	{
		return this->spd.format_short();
	}

	VSpeeds GetSpeed() const
	{
		return this->spd;
	}
};

/// ��
struct VAprinfoPchRecord : public VAprinfoBasicRecord
{
	/// ��, ���, ������
	long pchCode, nodCode, roadCode;
	std::string pchName, nodName, roadName;
	VAprinfoPchRecord() : VAprinfoBasicRecord(), 
		pchCode(0), nodCode(0), roadCode(0) {}
};

/// �������� ���������
inline bool operator == (const VAprinfoPchRecord & rec1, const VAprinfoPchRecord & rec2)
{
	return rec1.pchCode == rec2.pchCode && 
		rec1.nodCode == rec2.nodCode && 
		rec1.roadName == rec2.roadName;
}

/// ����� �� ������� ������������ ����
struct VAprinfoWobLabel : VAprinfoBasicLabel
{
	long fixTemp, minTemp, maxTemp;	///< ����. ����������� � ����������
	long startShift; ///< ????

	VAprinfoWobLabel() : VAprinfoBasicLabel(), 
		fixTemp(0), minTemp(0), maxTemp(0), startShift(0) {}
};

/// �����
struct VAprinfoWobRecord : public VAprinfoObjectRecord
{
	std::string num;	///< �����
	std::vector<VAprinfoWobLabel> labels; ///< �����

	VAprinfoWobRecord() : VAprinfoObjectRecord()
	{}
};

/// ������� ���� - �������� � ���������� ������
struct VAprinfoPntRecord : public VAprinfoObjectRecord
{
	std::string name, nameReverse;
	double axis;
	long code;

	VAprinfoPntRecord() : VAprinfoObjectRecord(), axis(0.0), code(0) {}
};

/// ����������� �������
enum SWITCH_DIR
{
	SDIR_UNDEF = 0, SDIR_ALONG = 1, SDIR_COUNTER = -1
};

/// �������
struct VAprinfoSwitchRecord : public VAprinfoObjectRecord
{
	SWITCH_DIR dir;		
	long side;
	double o, x;
	std::string num, cross_mark;

	VAprinfoSwitchRecord() : VAprinfoObjectRecord(), dir(SDIR_UNDEF), side(0), o(0), x(0)
	{}

	/// ���������� ���������� �������
	double GetCoordO() const
	{
		return this->o;
	};
	/// ���������� ���������� ����������
	double GetCoordX() const
	{
		return this->x;
	};

	/// ������������ ���������� �������
	void CorrectCoord(double in_correction)
	{		
		this->o+=in_correction;
		this->x+=in_correction;
		this->base+=in_correction;
		this->end+=in_correction;
	};

	/// ���������� ���� �������� �������
	pro::interval<double> GetRange() const
	{
		// ���� �������� ����� ����������� "� �������"
		return pro::interval<double>(std::min<double>(this->base, this->end), std::max<double>(this->base, this->end));
	};
};

/// ����� �� ������
struct VAprinfoCrvLabel : public VAprinfoBasicLabel
{
	float value; ///< �������� ��������� � �����

	VAprinfoCrvLabel() : VAprinfoBasicLabel(), value(0) {};
	VAprinfoCrvLabel(double in_coord, float in_value) : VAprinfoBasicLabel(in_coord), value(in_value) {};	

	bool operator < (const VAprinfoCrvLabel & other)
	{
		return this->coord < other.coord;
	}
};

/// ������������ ������
struct VAprinfoCrvRadius
{
	double radius, level, width, wear; ///< ������, ����� ���������� � ��, �����
	double latStart, latEnd, longStart, longEnd; ///< ������ � �������

	VAprinfoCrvRadius() : width(1520), level(0), radius(0), wear(0),
		longStart(0), longEnd(0), latStart(0), latEnd(0)
	{}
};

/// ������
struct VAprinfoCurve : public VAprinfoObjectRecord
{
	long orientation;	///< ����������
	double angle; ///< ����
	double latStart, latEnd, longStart, longEnd; ///< ������ � �������
	__int64 id; ///< id �� ����
	double levelBase, levelEnd; ///< ������� ������ �� ������

	std::vector<VAprinfoCrvLabel> planLabel, levelLabel; ///< ����� �� ����� � ������
	std::vector<VAprinfoCrvRadius> radius; ///< ��������� ��. ������

	VAprinfoCurve() : VAprinfoObjectRecord(), orientation(0), angle(0),
		longStart(0), longEnd(0), latStart(0), latEnd(0), id(-1),
		levelBase(0), levelEnd(0)
	{}

	void Clear();

	std::wstring Validate() const;
};

/// ��� �������
enum ZLINE_TYPE
{
	ZLINE_UNDEFINED,			///< �� ����������
	ZLINE_STRAIGHT,				///< ������
	ZLINE_OUTER_TRANSITION,		///< ������� ����������
	ZLINE_RADIUS,				///< ��������
	ZLINE_INNER_TRANSITION		///< ���������� ����������
};

/// ������� �����
struct VAprinfoZLRecord : public VAprinfoBasicRecord
{
	/// ��� �������
	ZLINE_TYPE zlType;	

	/// �������� �������� ��������� �� �������� �������
	double value_base, value_end; 

	/// �������� ������ ��� "��������" �������
	double Shift(double coord, bool beginning) const;
};

/// ����������������� ����� ��+�����
struct VAprinfoWidthRecord : public VAprinfoBasicRecord
{
	/// ��� �������
	ZLINE_TYPE zlType;	

	/// ������
	float Radius;
		
	/// ����� �� � ���������� �� �������� �������
	float width_begin, width_end, wear_begin, wear_end;

	float GetWidthMax() const
	{
		return std::max<float>(this->width_begin, this->width_end);
	}

	float GetWidthMin() const
	{
		return std::min<float>(this->width_begin, this->width_end);
	}

	float GetWidthCalc(double coord) const
	{		
		return line_t(point_t(this->base, this->width_begin), point_t(this->end, this->width_end)).Y(coord);
	}

	float GetWearCalc(double coord) const
	{		
		return line_t(point_t(this->base, this->wear_begin), point_t(this->end, this->wear_end)).Y(coord);
	}
};

/// ����� �������
struct VAprinfoProfLabel : public VAprinfoBasicLabel
{
	float value; ///< ��������
	VAprinfoProfLabel() : VAprinfoBasicLabel(), value(0) {}
};

// �������� �����
struct VAprinfoCoordPointRecord : public VAprinfoBasicRecord
{
	std::wstring id;
	double apr_abscoord;
	short int type;
};

// ����������� �������
struct VAprinfoControlAreaRecord : public VAprinfoObjectRecord
{
	float mo_width, sko_width, mo_level, sko_level;
	VAprinfoControlAreaRecord() : VAprinfoObjectRecord(), mo_width(0), sko_width(0),
		mo_level(0), sko_level(0) {}
};

// �������
struct VAprinfoRepairRecord : public VAprinfoObjectRecord
{
	VTime start_date, end_date;
	long type_id;
	std::wstring type_descr;
	VAprinfoRepairRecord() : VAprinfoObjectRecord(), type_id(0) {}
};

// ��������
struct VAprinfoCrossing : public VAprinfoBasicLabel
{
	std::wstring name;
	long side;
	VAprinfoCrossing() : name(L""), side(0) {}
};

// ���� �����
enum MACHINE_TYPE
{
	MACHINE_NONE = 0,
	MACHINE_VPO = 1,
	MACHINE_VRO_OR_VPR = 2,
	MACHINE_VPO_PLUS_VPR = 3,
	MACHINE_VPO_PLUS_VPR_PLUS_DSP = 4
};

std::string GetMachineTypeDescription(MACHINE_TYPE mach);

// ������� ������� ����
struct VAprinfoTestrunRecord : public VAprinfoObjectRecord
{
	MACHINE_TYPE machine;
	VTime date_start, date_end;
	VSpeeds spd;

	std::string GetDescr() const;
	
	VAprinfoTestrunRecord() : VAprinfoObjectRecord(), machine(MACHINE_NONE)
	{}
};

#pragma pack (pop)