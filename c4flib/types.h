#pragma once

#include <stdlib.h>
#include <time.h>
#include "c4stdint.h"

#include <os.h>
#include "strlib.h"
#include "timelib.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>

using namespace boost;

/** �������� ������*/
static const size_t BAD_SIZE=-1;
/** �������� ������*/
static const size_t BAD_INDEX=-1;

/** ����������� ��������*/
enum direction_t
{
	DR_UNDEFINED=0,//�� ����������
	DR_BACKWARD=-1,//�� ���������� ��������
	DR_FORWARD=1//�� �������� ��������
};

/** �������*/
enum side_t
{
	SD_UNDEFINED=0,//�� ����������
	SD_LEFT=-1,//�����
	SD_RIGHT=1//������
};

/** ����������� �������*/
static side_t other_side(side_t in_side)
{
	return static_cast<side_t>(-1*static_cast<int>(in_side));
};

/** ����������� side_t � �����*/
static std::wstring format_side_short(side_t in_side)
{	
	switch (in_side)
	{
		case SD_LEFT:
			return L"���.";
		case SD_RIGHT:
			return L"����.";
		case SD_UNDEFINED:
		default:
			return L"�����.";
			
	}
}

#pragma pack(push,1)


/** ����������*/
struct VWayCoord
{
	/** ��������*/
	int32_t km;
	/** ����*/
	float m;

	/** �������� >*/
	bool operator>(const VWayCoord &wc2) const throw()
	{
		return ((km > wc2.km) || ((km == wc2.km) && (m > wc2.m)));
	};
	/** �������� !=*/
	bool operator!=(const VWayCoord &wc2) const throw()
	{
		return ((km != wc2.km) || (m != wc2.m));
	};
	/** �������� ==*/
	bool operator==(const VWayCoord &wc2) const throw()
	{
		return ((km == wc2.km) && (m == wc2.m));
	};
	/** �������� <=*/
	bool operator<=(const VWayCoord &wc2) const throw()
	{
		return ((km < wc2.km) || ((km == wc2.km) && (m <= wc2.m)));
	};
	/** �������� >=*/
	bool operator>=(const VWayCoord &wc2) const throw()
	{
		return ((km > wc2.km) || ((km == wc2.km) && (m >= wc2.m)));
	};
	/** �������� <*/
	bool operator < (const VWayCoord & wc2) const throw()
	{
		return ((km < wc2.km) || ((km == wc2.km) && (m < wc2.m)));
	}

	/** ����������� ��� ����������*/
	VWayCoord() : km(0), m(0) {	};
	/** ����������� � �����������*/
	VWayCoord(long lKm, float fM) : km(lKm), m(fM) { };
	
	
	std::wstring format_short() const {return boost::str(boost::wformat(L"%d:%0.0f") % km % m);};
	std::wstring format_long() const {return boost::str(boost::wformat(L"%d �� %0.0f �") % km % m);};

};

/** ������� ����*/
struct VWaySection
{
	/** ������ ���������*/
	uint32_t dwSize;
	/** ���������� ������*/
	VWayCoord start;
	/** ���������� �����*/
	VWayCoord end;
	/** �������� !=*/
	bool operator!=(const VWaySection &ws) const throw()
	{
		return ((start != ws.start) || (end != ws.end));
	}
	/** �������� ==*/
	bool operator==(const VWaySection &ws) const throw()
	{
		return ((start == ws.start) && (end == ws.end));
	}
	/** ��������� ���������� �������, ���������� true, ���� ���������� ������ ������ ���� ����� ���������� ����� �������*/
	bool isValid()
	{
		return (start<=end) && !(start.km==0 && start.m==0 && end.km==0 && end.m==0);
	}

	/** �����������*/
	VWaySection() : dwSize(sizeof(VWaySection)), start(0,0), end(0,0)
	{};

	/** �����������*/
	VWaySection(const VWayCoord& in_start, const VWayCoord& in_end) : dwSize(sizeof(VWaySection)), 
		start(in_start), end(in_end)
	{};

	/** �����������*/
	VWaySection(int32_t in_start_km, float in_start_m, 
		int32_t in_end_km, float in_end_m) : dwSize(sizeof(VWaySection)), 
		start(in_start_km, in_start_m), end(in_end_km, in_end_m)
	{};

	/// ������ ������������
	bool Crossing(const VWaySection & src)
	{
		if (src.end <= this->start || src.start >= this->end) return false;
		else return true;
	}

	std::wstring format_long() const 
	{ 
		return this->start.format_long() + L" - " + this->end.format_long();
	}
};

const int DEFAULT_SPD_PASS(140);
const int DEFAULT_SPD_TRUCK(90);
const int DEFAULT_SPD_EMPTY(90);
const int NO_SPEED(-1);

/// ��������� ��������
class speed_t
{
	int spd; // ��������

public:

	speed_t();
	speed_t(int in_spd);
	speed_t(const char * in_str);
	speed_t(const std::string & in_str);
	speed_t(const wchar_t * in_str);
	speed_t(const std::wstring & in_str);

	speed_t & operator = (int in_spd);
	speed_t & operator = (const char * in_str);
	speed_t & operator = (const std::string & in_str);
	speed_t & operator = (const wchar_t * in_str);
	speed_t & operator = (const std::wstring & in_str);

	operator int() const;

	bool Valid() const;

	bool operator < (const speed_t & in_spd) const;
	bool operator < (int in_spd) const;

	bool operator > (const speed_t & in_spd) const;
	bool operator > (int in_spd) const;

	bool operator != (const speed_t & in_spd) const;
	bool operator != (int in_spd) const;

	bool Decrease(int in_step);

	std::wstring format() const;
};

namespace std
{
	template <> const speed_t & max<speed_t>(const speed_t & s1, const speed_t & s2);
};

/// ��������
class VSpeeds
{
	speed_t spd_pass, spd_truck, spd_empty; ///< ������������, ��������, ��������
	speed_t spd_hi, spd_hi_bt; ///< �������

public:

	VSpeeds();

	VSpeeds(
		const speed_t & in_spd_pass, 
		const speed_t & in_spd_truck, 
		const speed_t & in_spd_empty);

	VSpeeds(
		const speed_t & in_spd_pass, 
		const speed_t & in_spd_truck, 
		const speed_t & in_spd_empty, 
		const speed_t & in_spd_hi, 
		const speed_t & in_spd_hi_bt);

	VSpeeds(const char * in_spd);
	VSpeeds(const std::string & in_spd);
	VSpeeds(const wchar_t * in_spd);
	VSpeeds(const std::wstring & in_spd);

	VSpeeds & operator = (const char * in_spd);
	VSpeeds & operator = (const std::string & in_spd);
	VSpeeds & operator = (const wchar_t * in_spd);
	VSpeeds & operator = (const std::wstring & in_spd);

	/// ���������� ��������� �������������
	std::wstring format_short() const;	

	///���������� true ���� �������� �������
	bool Valid() const;

	/// ���������� ����. ��������
	const speed_t & GetPassSpeed() const;

	/// ���������� ����. ��������
	const speed_t & GetTruckSpeed() const;

	/// ���������� �������� ���
	const speed_t & GetEmptySpeed() const;

	/// ���������� �������� HS
	const speed_t & GetHighSpeed() const;

	/// ���������� �������� HS-BT
	const speed_t & GetHighBTSpeed() const;

	/// ���������� ������������ ��������
	const speed_t & GetMaxSpeed() const;

	/// ������������� ����. ��������
	void SetPassSpeed(const speed_t & spd);

	/// ������������� ����. ��������
	void SetTruckSpeed(const speed_t & spd);

	/// ������������� �������� ���
	void SetEmptySpeed(const speed_t & spd);

	/// ���������� �������� HS
	void SetHighSpeed(const speed_t & spd);

	/// ���������� �������� HS-BT
	void SetHighBTSpeed(const speed_t & spd);

	/// ������������ ��������
	bool RestrictSpeed(const VSpeeds & in_restriction);

	/// ������������ ��������
	bool RestrictMaxSpeed(const speed_t & in_speed);

	/// ������������ ��������
	bool RestrictEmptySpeed(const speed_t & in_speed);

	/// ������������ ��������
	void DecreaseSpeed(int in_step);

	/// ������� ������� �������� (����.������)
	void DecreaseHighBTSpeed(int in_step);

	/// ������� ������� ��������
	void DecreaseHighSpeed(int in_step);

	/// ������� ������������ ��������
	void DecreasePassSpeed(int in_step);

	/// ������� �������� �������� 
	void DecreaseTruckSpeed(int in_step);

	/// ������� �������� ���������
	void DecreaseEmptySpeed(int in_step);

	/// �������� !=
	bool operator != (const VSpeeds & in_speeds) const;

	/// �������� <
	bool operator < (const VSpeeds & in_speeds) const;

	/// ����������� �������� ������� �����������
	static VSpeeds CombineSpeeds(const VSpeeds & in_l, const VSpeeds & in_r);
	static VSpeeds CombineSpeeds(const VSpeeds & in_1, const VSpeeds & in_2, const VSpeeds & in_3);
	/// ���������� ������ �����������
	static VSpeeds Restriction(const VSpeeds & in_speed, const VSpeeds & in_restrinction);
};

/// �����
struct VPicket
{
	uint32_t size;			///< ������ ���������
	long km;				///< ����� ��
	long pk;				///< ����� ������
	float len;				///< ����� ������
	VWaySection section;	///< ������� ���������� ������ ������
	double absStart, absEnd;///< ���������� ���������� ������ ������
};

#pragma pack(pop)

/** FixMe!!! ���������� � ������ �������*/
/** ������� ��������*/
struct VRouteSection
{
private:
	/// ��� �����������
	long dirCode;
	/// ��� ����
	std::string wayName;
	/// ����������
	VWaySection section;
public:
	/// �����������
	VRouteSection() : dirCode(0), wayName("") {};
	/// �����������
	VRouteSection(long in_code, const std::string& in_way, const VWaySection& in_section) :
	dirCode(in_code), wayName(in_way), section(in_section){};
	/// �����������
	VRouteSection(const VRouteSection& in_source)
	{
		*this=in_source;
	};
	/// ���������� ��� �����������
	long GetCode() const {return dirCode;};
	long& GetCode() {return dirCode;};
	/// ���������� ����
	const std::string& GetWay() const {return wayName;};
	std::string& GetWay() {return wayName;};
	/// ���������� ����������
	const VWaySection& GetSection() const {return section;};
	VWaySection& GetSection() {return section;};
	/// ��������=
	VRouteSection& operator=(const VRouteSection& in_source);
	/// ��������==
	bool operator==(const VRouteSection& in_right) const;
};
