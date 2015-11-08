#pragma once

#include "c4flib.h"
#include "optlib.h"
#include "parameters.h"
#include "VAprinfo.h"
#include "VCalcOptions.h"
#include "oversteps.h"
#include <vector>
#include <algorithm>

// ���� ������
enum CURVE_TYPE
{
	CURVE_MAGISTRAL, CURVE_STATION, CURVE_MODEL, CURVE_REPAIR, CURVE_SPEED
};


/// ����������� ���������� ����������
static const double MAX_LEVEL_ALLOWED=145;
/// ����������� ���������� �������� �� ����
static const double MIN_LEVEL_ALLOWED=15;
// �������� ������� ������
static const char * __status__[]= {
	"�� �����������",
	"��������������",
	"�������� ��������������",
	"�� ������������� ������������� ��������",
	"����������� ������ ������"
};

// ����������� � ������
static const char * __comment__[]= {
	"���������� ����������",
	"�� �������������� ���������� �� ���",
	"������������� ����������",
	"������������ �������",
	"������ �������",
	"������� ������",
	"������"
};

/// ������������� ������ ������� � ������ �������� (����������/��������)
static size_t radius_to_element(size_t in_radius) {return in_radius*2+1;};
/// ���������  ������ ����� ������ ������� (��������)
static size_t radius_start_mark(size_t in_radius) {return in_radius*2+1;};
/// ���������  ������ ����� ����� ������� (��������)
static size_t radius_end_mark(size_t in_radius) {return in_radius*2+2;};

// ������� ��� ��� ������

/// ��� ���������������� (��������) ��������
class VModelAnpTruckCondition : public VOverstepCondition
{
public:
	bool TestValue(double in_value, double in_length=0) const {return fabs(in_value) > 0.3;};
};

/// ��� ������������ ��������
class VModelAnpPassCondition : public VOverstepCondition
{
public:
	bool TestValue(double in_value, double in_length=0) const {return fabs(in_value) > 0.7;};
};

/// ��� ���������� �������
class VModelAnpHighCondition : public VOverstepCondition
{
public:
	bool TestValue(double in_value, double in_length=0) const {return fabs(in_value) > 1.0;};
};

/// ��� ��� ������������ ��������
class VModelPsiPassCondition : public VOverstepCondition
{
public:
	bool TestValue(double in_value, double in_length=0) const {return fabs(in_value) > 0.6;};
};

/// ��� ��� �������� ��������
class VModelPsiTruckCondition : public VOverstepCondition
{
public:
	bool TestValue(double in_value, double in_length=0) const {return fabs(in_value) > 0.3;};
};

/// ��� ��� ���������� �������
class VModelPsiHighCondition : public VOverstepCondition
{
public:
	bool TestValue(double in_value, double in_length=0) const {return fabs(in_value) > 0.9;};
};

// ---------------------------------------------------------------------------
// �������� ������
// ---------------------------------------------------------------------------

/// ����� �� ������
struct VCurveMark
{
	uint32_t index;	///< ����� �����
	double value;	///< ������� ���������
	double latitude, longitude; ///< GPS

	/// ����������� �� ���������
	VCurveMark() : index(0), value(0), 
		latitude(0.0f), longitude(0.0f)
	{}

	/// ����������� �� ��������� ������� � ��������
	VCurveMark(uint32_t __index, double __val) : index(__index), value(__val),
		latitude(0.0f), longitude(0.0f)
	{}

	/// ��������� ������ � �������
	void SetGeoCoord(const VDoubleParameter & lat, const VDoubleParameter & lng)
	{
		this->latitude = lat.GetItem(this->index);
		this->longitude = lng.GetItem(this->index);
	}

	/// �������� ���������
	bool operator < (const VCurveMark & other)
	{
		return this->index < other.index;
	}
};

///������ �����������
#define COMMENT_SIZE 260

// ������ ��������� ������
/** �� ��������������*/
#define MODEL_STATUS_NOT_PASSPORT 0L
/** ��������������*/
#define MODEL_STATUS_PASSPORT 1L
/** �������� ��������������*/
#define MODEL_STATUS_TEMP_PASSPORT 2L
/** �� ������������ ������������� ��������*/
#define MODEL_STATUS_SPEED_OVR 3L

/// �������������� ������ ������
struct VCurveModelInfo
{
	long status; ///< ������ ������ ������
	char comment[COMMENT_SIZE]; ///< ����������� ������
	speed_t Vhigh_bt, Vhigh, Vpass, Vcrg, Vavg, Vmin; ///< ��������� ��������

	// �����������
	VCurveModelInfo() : status(0), Vhigh_bt(NO_SPEED), Vhigh(NO_SPEED), Vpass(100), Vcrg(80), Vavg(60), Vmin(40)
	{
		ZeroMemory(comment, sizeof(comment));
	}

	// ���������� �����������
	VCurveModelInfo(const VCurveModelInfo & src) : status(src.status),
		Vhigh_bt(src.Vhigh_bt), Vhigh(src.Vhigh), Vpass(src.Vpass), Vcrg(src.Vcrg), Vavg(src.Vavg), Vmin(src.Vmin)
	{
		memcpy(this->comment, src.comment, COMMENT_SIZE);
	}

	// �������� ������������
	VCurveModelInfo & operator = (const VCurveModelInfo & src)
	{
		this->status = src.status;
		this->Vhigh_bt = src.Vhigh_bt;
		this->Vhigh = src.Vhigh;
		this->Vpass = src.Vpass;
		this->Vcrg = src.Vcrg;
		this->Vavg = src.Vavg;
		this->Vmin = src.Vmin;
		memcpy(this->comment, src.comment, COMMENT_SIZE);
		return *this;
	}

};

///������������ ��������� � �����
struct VAnp
{
	VAnp() : index(0), value(0) {};
	VAnp(size_t in_index, float in_value) : index(in_index), value(in_value) {};
	size_t index;
	double value;
	bool operator < (const VAnp & right)
	{
		return index < right.index;
	};
};

///��� ��������
enum speed_type
{
	pass_prk=0,///����. �� �������
	crg_prk=1,///����. �� �������
	prj_prk=2,///���. �� �������
	pass_plan=3,///����. �����������
	crg_plan=4,///����. �����������
	prj_plan=5,///���. �����������
	
	high_prk=6,///����.
	high_bt_prk=7///����. � ����. ������
};


/// ������� �������� �� ������
struct VCurveSpdArea
{
	speed_t spd;		///< �������� ��������
	double begin, end;	///< ������� �������
};

/// ����� �� ������
struct VCurvePicket : public VPicket
{
	/// ������� ����������� �� ������
	bool isVkrPass, isVkrTruck, isVprPass, isVprTruck, isVizPass, isVizTruck;

	/// �����������
	VCurvePicket();

	/// ���������� ����������� [1]
	VCurvePicket(const VPicket & pk);

	/// ���������� ����������� [2]
	VCurvePicket(const VCurvePicket & pk);

	/// ������� ����������� �� ������
	bool HasLimit() const;
};

/// ����� � �������� �� ������
struct VCurveWearPicket : public VPicket
{
	double maxWear;	///< ������������ ����� 
	double avgWear;	///< ������� ����� 
	double maxWidth;	///< ������������ �� 
	double avgWidth;	///< ������� �� 
	speed_t spdPass;	///< ���������� �������� �� ������ - ������������
	speed_t spdTruck;	///< ���������� �������� �� ������ - ��������
	speed_t prkPass;    ///< �������� �� ������� - ������������
	speed_t prkTruck;   ///< �������� �� ������� - ��������
	bool ntd;			///< ����������� �� ���
	double errorWidth; ///< ������� ����� �� ��
	double errorWear; ///< ������� ����� �� ������

	/// �����������
	VCurveWearPicket();

	/// ���������� ����������� [1]
	VCurveWearPicket(const VPicket & pk);

	/// ���������� ����������� [2]
	VCurveWearPicket(const VCurveWearPicket & pk);
};

/// ����� �� ������
struct VCurveLink
{
	double absStart, absEnd; ///< ������� - ���.�-��
	VWaySection section; ///< ������� - ������� �-��
	double avgWear, maxWear, errorWear; ///< �����: ��������, �������, % �����
	double avgWidth, maxWidth, errorWidth; ///< ��: ��������, �������, % �����
	speed_t spdPass, spdTruck;	///< ����������� �������� �� �����, ����. � ����.
	bool ntd;			///< ����������� �� ���

	/// �����������
	VCurveLink();
};

/// "������� ������ ������" ��� ���
struct VCurveFPOArea
{
	double absStart, absEnd; ///< ������� - ���.�-��
	VWaySection section; ///< ������� - ������� �-��

	speed_t spdVprkPass; ///< ��������� ��������
	speed_t spdVkr, spdVpr, spdViz; ///< ��������� ��������
	double maxAnp, maxAnpCoord; ///< �������� ������������� ��������� � ��� ���.�-��
	double maxPsi, maxPsiCoord; ///< �������� ��� ��������� � ��� ���.�-��
	double maxOh, maxOhCoord; ///< �������� ������ ���������� ��������� � ��� ���.�-��
	double maxAnpRange, maxPsiRange; ///< ������ ���, ���, ��
	/// ������� ����������
	VWayCoord maxAnpWayCoord, maxPsiWayCoord, maxOhWayCoord;
};

/// ��������������� ������
class VDataSet;
class VAbsCoordData;
class VCurve;

/// ��������� ���� �� �����������
const long WAYCAT_UNSET = 100;

/// ��������� ��������� ������� ������
struct VCurveTempParams
{
	/// ����������� ������
	const VCurve * curve;
	
	long wayCat;				///< ��������� ����	

	/// ������ �������������, ��� � ���� ��� �������
	double anpHighBTRangeMid;   ///< ����� ��� �������� ��� �� ������� �������� � �������� ������
	double anpHighBTRangeMax;	///< ����� ��� ��������� ��� �� ������� �������� � �������� ������
	double anpHighRangeMid;     ///< ����� ��� �������� ��� �� ������� �������� 
	double anpHighRangeMax;		///< ����� ��� ��������� ��� �� ������� �������� 
	double anpPassRangeMid;		///< ����� ��� �������� ��� �� ����.��������
	double anpPassRangeMax;		///< ����� ��� ��������� ��� �� ����.��������
	double anpTruckRangeMid;	///< ����� ��� �������� ��� �� ����.��������
	double anpTruckRangeMax;	///< ����� ��� ��������� ��� �� ����.��������
	double psiHighBTRangeMid;	///< ����� ��� �������� ��� �� ������� ��������� � �������� ������
	double psiHighBTRangeMax;	///< ����� ��� ��������� ��� �� ������� ��������� � �������� ������
	double psiHighRangeMid;		///< ����� ��� �������� ��� �� ������� ���������
	double psiHighRangeMax;		///< ����� ��� ��������� ��� �� ������� ���������
	double psiPassRangeMid;		///< ����� ��� �������� ��� �� ����.���������
	double psiPassRangeMax;		///< ����� ��� ��������� ��� �� ����.���������
	double psiTruckRangeMid;	///< ����� ��� �������� ��� �� ����.���������
	double psiTruckRangeMax;	///< ����� ��� ��������� ��� �� ����.���������
	double baseMid;				///< ���� ���������� 
	double baseTap;				///< ���� ��� ���������� ��� � �������
	void SetWayCat(VDataSet & ds, bool isModel);	///< ���������� ��������� ����

	std::vector<uint32_t> ranges;					///< ������� ������ ������
	void SetRanges(VDataSet & ds);					///< ��������� ������ ������

	/// ����� ������ �� �������
	uint32_t GetKadrCount() const
	{
		return this->ranges.at(1) - this->ranges.at(0) + 1;
	}

	std::vector<double> vprkPass;					///< ������ ��������� �� ������� [������������]
	std::vector<double> vprkTruck;					///< ������ ��������� �� ������� [��������]
	std::vector<double> vprkThird;					///< ������ ��������� �� ������� [60/����������������]
	std::vector<double> vprkHigh;					///< ������ ��������� �� ������� [�������]
	std::vector<double> vprkHighBT;					///< ������ ��������� �� ������� [�������|����.������]
	void SetSpeeds(VDataSet & ds , bool isModel);	///< ��������� ��������� �� �������

	/// ������� �������� �� ������
	struct SpdArea
	{
		uint32_t idxFrom;	///< ������ ����� ������ �������
		uint32_t idxTo;		///< ������ ����� ����� �������
		speed_t spdPass;	///< �������� ������������
		speed_t spdTruck;	///< �������� ��������
		speed_t spdThird;	///< ������ ��������
		speed_t spdHigh;	///< ������� ��������
		speed_t spdHighBT;	///< ������� �������� � �������� ������
	};

	/// �������� ������� ������ ��������� �� �����������
	std::vector<VCurveTempParams::SpdArea> SelectSpeeds(VDataSet & ds, size_t from, size_t to, bool isModel);

	std::vector<double> kurs;						///< ������ ��������� �����
	void CalcKurs(VDataSet & ds, bool isModel);		///< �������� �����

	std::vector<double> midPlan;					///< ������ ������� ��������
	void CalcMidPlan(VDataSet & ds, bool isModel);	///< �������� ������� ��������

	std::vector<double> midLevel;					///< ������ �������� ������
	void CalcMidLevel(VDataSet & ds, bool isModel);	///< �������� �������� ������	
	
	std::vector<double> radius;						///< ������ ���������� ��������
	void CalcRadius(VDataSet & ds, bool isModel);	///< �������� ���������� ��������
	
	std::vector<double> anpPass;					///< ������������ ��������� [������������]
	std::vector<double> anpTruck;					///< ������������ ��������� [��������]
	std::vector<double> anpThird;					///< ������������ ��������� [60|������� ��������]
	std::vector<double> anpHigh;					///< ������������ ��������� [������� ��������]
	std::vector<double> anpHighBT;					///< ������������ ��������� [������� ��������|����.������]
	void CalcAnp(VDataSet & ds, bool isModel);		///< �������� ������������

	std::vector<double> vkrPass;					///< ���������� V�� ������������
	std::vector<double> vkrTruck;					///< ���������� V�� ��������
	std::vector<double> vkrHigh;					///< ���������� V�� [������� ��������]
	std::vector<double> vkrHighBT;					///< ���������� V�� [������� ��������|����.������]
	void CalcVkr(VDataSet & ds, bool isModel);		///< �������� V��
    
    /// ������� V�� �� ��������
    long GetMidVkr(VDataSet & ds, bool isModel, float anpRange, uint32_t radiusNdx);

	std::vector<double> psiPass;					///< ��� - ����� ��� [������������]
	std::vector<double> psiTruck;					///< ��� - ����� ��� [��������]
	std::vector<double> psiHigh;					///< ��� - ����� ��� [������� ��������]
	std::vector<double> psiHighBT;					///< ��� - ����� ��� [������� ��������|����.������]
	std::vector<double> vizPass;					///< ���������� V�� [������������]
	std::vector<double> vizTruck;					///< ���������� V�� [��������]
	std::vector<double> vizHigh;					///< ���������� V�� [������� ��������]
	std::vector<double> vizHighBT;					///< ���������� V�� [������� ��������|����.������]
	void CalcPsiViz(VDataSet & ds, bool isModel);	///< �������� ���� � V��

	std::vector<double> tapPlan;					///< ������ ������� ��������
	void CalcTapPlan(VDataSet & ds, bool isModel);	///< ������ ������ ��������
	
	std::vector<double> tapLevel;					///< ������ ������� ������
	void CalcTapLevel(VDataSet & ds, bool isModel);	///< ������ ������ ������
	
	std::vector<double> vprPass;					///< ������ ���������� Vpr ������������
	std::vector<double> vprTruck;					///< ������ ���������� Vpr ��������
	std::vector<double> vprPassRepair;              ///< ������ ���������� Vpr ������������ �� ��������� ����������
	std::vector<double> vprTruckRepair;             ///< ������ ���������� Vpr �������� �� ��������� ����������
	void CalcVpr(VDataSet & ds, bool isModel);		///< ������ ���������� V��

	std::vector<double> levelOffset;
	void CalcLevelOffset(VDataSet & ds);			///< ���������� ������ �� ����� �� ������

	// �������� ������ � ����
	bool DumpToFile(const std::wstring & fn, const std::vector<double> & prm, VDataSet & ds);

	/// ������� �� �������
	double GetMin(std::vector<double> & param, size_t from, size_t to);

	/// �������� �� �������
	double GetMax(std::vector<double> & param, size_t from, size_t to);

	/// �������� �� ������ �� �������
	double GetMaxAbs(std::vector<double> & param, size_t from, size_t to, uint32_t & maxNdx);

	/// ������� �� �������
	double GetAvg(std::vector<double> & param, size_t from, size_t to);

	/// ����� ������������� ��������
	void Drop();
};

/// ��������� "����� ������" ��� ��������� ������������� ����� ������
const uint32_t COMPLETE_CURVE = static_cast<uint32_t>(-1);

/// ����������� ������ ������
class VCurveList;

/// "����" ������� �� ������ ���������� ��� ������ �������� �� �������
bool IsTapFailure(double prk_spd, double tap, bool is_repair);

/// ������� ������������� ������
class VProtoCurve 
{
private:
	VExcess excess;
public:
	/// �����������
	VProtoCurve(const VExcess& in_excess) : excess(in_excess) {};
	/// ���������� ���������� ������
	double GetStart() const {return excess.GetStart();};
	/// ���������� ���������� �����
	double GetEnd() const {return excess.GetEnd();};
	/// ���������� �����
	double GetLength() const {return excess.GetLength();};
	/// ���������� �������
	side_t GetSide() const {return excess.GetAverage()>0 ? SD_RIGHT : SD_LEFT;};
};

/// ����� �������� ������
class VFindCurve : public VDataProcessor<VPoint, VProtoCurve>
{
private:	
	/// ������ �� ������
	std::deque<VProtoCurve> output;
	/// ����� ������ (�������� <0)
	VFindExcess fel;
	/// ������� ��� ������ ����� ������
	VCurveCondition rcl;
	/// ������ ������ (�������� >0)
	VFindExcess fer;
	/// ������� ��� ������ ������ ������
	VCurveCondition rcr;
public:
	/// �����������
	VFindCurve(double in_min_curv=0);
	/// �������� ������ �� ����
	void Put(const VPoint& in_point);
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const;		
	/// ���������� ������ ������� ������
	const VProtoCurve& Front() const;
	/// ����������� ������ ������� ������
	void Pop();
};

/// ����� ����������
class VFindTransition : public VDataProcessor<VPoint, VExcess>
{
private:	
	/// ������ �� ������
	std::deque<VExcess> output;
	/// ����� ������ (�������� <0)
	VFindExcess fe_pos;
	/// ������� ��� ������ ����� ������
	VRangeCondition rc_pos;
	/// ������ ������ (�������� >0)
	VFindExcess fe_neg;
	/// ������� ��� ������ ������ ������
	VRangeCondition rc_neg;
public:
	/// �����������
	VFindTransition(double in_min_curv);
	/// �������� ������ �� ����
	void Put(const VPoint& in_point);
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const;		
	/// ���������� ������ ������� ������
	const VExcess& Front() const;
	/// ����������� ������ ������� ������
	void Pop();	
	/// ���������� ������� ������
	void Flush();
};



/// ��������� ������
class VCurve
{
	friend VCurveList;
	friend VCurveTempParams;

	/// �������������� ������
	VCurveModelInfo model;	

	/// ������ ��������� 
	bool isModel;

	/// ����� �� ����� � ������
	std::vector<VCurveMark> planMark, levelMark;	

	/// ������������ � ����� �� ��������
	double AproxValue(size_t index, 
		const std::vector<VCurveMark> & marks, 
		const VAbsCoordData & absCoord) const;

	/// ��������� ��������� ������
	mutable VCurveTempParams tempParams;	

	/// ������� ������������ �� ����������
	int GetDegree(float p, const float table[]) const
	{
		return p > table[0] ? 3 : p > table[1] ? 2 : p > table[2] ? 1 : 0;
	}

	//������ ��� ��������� �������� � �������������

	///������������ ���������
	mutable std::vector<VAnp> modelAnp;

public:

	/// �����������
	VCurve()
	{
		this->isModel = false;
		this->tempParams.curve = this;
		this->tempParams.wayCat = WAYCAT_UNSET;
	}

	/// ���������� �����������
	VCurve(const VCurve & crv)
	{
		this->planMark = crv.planMark;
		this->levelMark = crv.levelMark;
		this->model = crv.model;
		this->isModel = crv.isModel;
		this->tempParams = crv.tempParams;
		this->tempParams.curve = this;
	}

	/// �������������� � ������ Aprinfo
	VAprinfoCurve ExportToAprinfo(VDataSet & ds);

	/// �������� ��������� ��������
	void DropCalculations()
	{
		this->tempParams.Drop();
	}

	/// ������� ����������� ������
	bool IsModel() const
	{
		return this->isModel;
	}

	/// ���������� "�����������" ������
	void SetModel(bool isModel)
	{
		this->isModel = isModel;
	}

	/// ������� ����������� ������
	const char * GetModelComment() const
	{
		return this->model.comment;
	}

	/// ���������� ����������� ������
	void SetModelComment(const char * comment)
	{
		strncpy_s(this->model.comment, COMMENT_SIZE, comment, COMMENT_SIZE-1);
		this->model.comment[COMMENT_SIZE-1] = 0;
	}

	/// ������� ������ ������
	int GetModelStatus() const
	{
		if (this->model.status >= 0 && this->model.status < 5)
			return this->model.status;
		else
			return 0;
	}

	/// ���������� ������ ������
	void SetModelStatus(int status)
	{
		this->model.status = status;
	}

	/// ������� �������� ������� ������
	const char * GetModelStatusDescription() const;


	/// ������� �������� (����. � �������� ������)
	speed_t GetModelVhigh_bt() const
	{
		return this->model.Vhigh_bt;
	};
	
	/// ������� �������� (����.)
	speed_t GetModelVhigh() const
	{
		return this->model.Vhigh;
	};

	/// ������� �������� (����.)
	speed_t GetModelVpass() const
	{
		return this->model.Vpass;
	}; 

	/// ������� �������� (����.)
	speed_t GetModelVcrg() const
	{
		return this->model.Vcrg;
	}; 

	/// ������� �������� (�����.)
	speed_t GetModelVavg() const
	{
		return this->model.Vavg;
	};

	/// ������� �������� (���.)
	speed_t GetModelVmin() const
	{
		return this->model.Vmin;
	};


	/// ���������� �������� (����. � ����.)
	void SetModelVhigh_bt(speed_t Vhigh_bt)
	{
		this->model.Vhigh_bt=Vhigh_bt;
	};
	
	
	/// ���������� �������� (����.)
	void SetModelVhigh(speed_t Vhigh)
	{
		this->model.Vhigh=Vhigh;
	};

	/// ���������� �������� (����.)
	void SetModelVpass(speed_t Vpass)
	{
		this->model.Vpass=Vpass;
	}; 

	/// ���������� �������� (����.)
	void SetModelVcrg(speed_t Vcrg)
	{
		this->model.Vcrg=Vcrg;
	}; 

	/// ���������� �������� (�����.)
	void SetModelVavg(speed_t Vavg)
	{
		this->model.Vavg=Vavg;
	};

	/// ���������� �������� (���.)
	void SetModelVmin(speed_t Vmin)
	{
		this->model.Vmin=Vmin;
	};

	/// ����� �����
	size_t GetMarkCount() const
	{
		return this->planMark.size();
	}

	/// ����� ����� �� ������
	size_t GetLevelMarkCount() const
	{
		return this->levelMark.size();
	}

	/// �������� ����� �� ����
	void AddPlanMark(uint32_t index, double value)
	{		
		for (size_t i=0; i<planMark.size(); ++i)
		{
			if (planMark.at(i).index==index)
				return;
		}
		this->planMark.push_back(VCurveMark(index, value));
		std::sort(this->planMark.begin(), this->planMark.end());
	}
	
	/// ������� ����� �� �����
	bool DeletePlanMark(size_t index)
	{
		if (index < this->planMark.size())
		{			
			this->planMark.erase(this->planMark.begin() + index);			
			return true;
		}
		else 
			return false;
	}

	/// ����� �� �����
	const VCurveMark & GetPlanMark(size_t index) const
	{
		return this->planMark.at(index);
	}

	/// ����� �� �����
	VCurveMark & GetPlanMark(size_t index)
	{
		return this->planMark.at(index);
	}

	/// �������� ����� �� �������
	void AddLevelMark(uint32_t index, double value)
	{
		for (size_t i=0; i<levelMark.size(); ++i)
		{
			if (levelMark.at(i).index==index)
				return;
		}
		this->levelMark.push_back(VCurveMark(index, value));
		std::sort(this->levelMark.begin(), this->levelMark.end());
	}

	/// ������� ����� �� �����
	bool DeleteLevelMark(size_t index)
	{
		if (index < this->levelMark.size())
		{
			this->levelMark.erase(this->levelMark.begin() + index);
			return true;
		}
		else 
			return false;
	}

	/// ����� �� ������
	const VCurveMark & GetLevelMark(size_t index) const
	{
		return this->levelMark.at(index);
	}

	/// ����� �� ������
	VCurveMark & GetLevelMark(size_t index)
	{
		return this->levelMark.at(index);
	}

	/// ����� ��������
	size_t GetRadiusCount() const
	{
		return this->planMark.size() / 2 - 1;
	}
	
	/// ���������� ������ FixMe!!! �� �����������
	double GetRadius(size_t in_index) const
	{
		return curv_to_radius(this->planMark.at(radius_start_mark(in_index)).value);
	}
	
	/// ������������� ������ FixMe!!! �� �����������
	void SetRadius(size_t in_index, double in_radius)
	{
		this->planMark.at(radius_start_mark(in_index)).value = radius_to_curv(in_radius);
		this->planMark.at(radius_end_mark(in_index)).value = radius_to_curv(in_radius);
	}

	/// ������ ����� �� �����
	const std::vector<VCurveMark> & GetPlanMarkList() const
	{
		return this->planMark;
	}

	/// ������ ����� �� �����
	const std::vector<VCurveMark> & GetLevelMarkList() const
	{
		return this->levelMark;
	}

	/// �������� ����� �����
	bool CheckMarks() const;

	/// ������������ �������� �� ����� �� ��������
	double AproxPlan(size_t index, const VAbsCoordData & absCoord) const
	{
		return this->AproxValue(index, this->planMark, absCoord);
	}

	/// ������������ �������� �� ������ �� ��������
	double AproxLevel(size_t index, const VAbsCoordData & absCoord) const
	{
		return this->AproxValue(index, this->levelMark, absCoord);
	}

	/// ��������� ����, ��� ������� ��������� ������
	long GetWayCat(VDataSet & ds) const;

	// ---------------------------------------------------------------------
	// �������������� ����� � ������������ ������
	// ---------------------------------------------------------------------

	/// ������ �����, �������������� ������
	size_t GetRangeMark(uint32_t radius, bool circ, bool begin) const;

	/// ���.�-�� ������� ������
	double GetRangeCoord(VDataSet & ds, 
		bool plan, bool circ, bool begin, 
		uint32_t radius = COMPLETE_CURVE) const;

	/// �������� ��������� ������ � ��������� ������� (� ���������� �����������)
	bool FitsRanges(VDataSet & ds, double from, double to) const;

	/// �������� ��������� ������ � ������� ����, �������� �������
	bool FitsWayRanges(VDataSet & ds) const;

	/// ����������� ������ (�� �������� ������)
	double GetMinRadius(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ������ (�� �������� ������)
	double GetAvgRadius(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������������ ������ (�� �������� ������)
	double GetMaxRadius(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ����������� ���������� (�� �������� ������)
	double GetMinLevel(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ���������� (�� �������� ������)
	double GetAvgLevel(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������������ ���������� (�� �������� ������)
	double GetMaxLevel(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ����������� ���������� (�� �������� ������)
	double GetAvgFactLevel(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ����������� �� (�� �������� ������)
	double GetMinWidth(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� �� (�� �������� ������) 
	double GetAvgWidth(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������������ �� (�� �������� ������)
	double GetMaxWidth(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ����� �� ��
	double GetWidthErrorPercent(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ���������� �� ������� �� �� - ��������
	double GetMaxDeltaWidthFront(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ���������� �� ������� �� �� - �������
	double GetAvgDeltaWidthFront(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ����� ��� ������� ���������� �� ������� �� ��
	double GetDeltaWidthFrontErrorPercent(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ���������� �� ����� �� �� - ��������
	double GetMaxDeltaWidthBack(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ���������� �� ����� �� �� - �������
	double GetAvgDeltaWidthBack(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ����� ��� ������� ���������� �� ������� �� ��
	double GetDeltaWidthBackErrorPercent(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;
	
	/// ���� �������� ������ (�� ����� ������)
	double GetAngle(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ���������� ������
	long GetOrientation(VDataSet & ds) const;

	/// ���� ��������/����������
	double GetSign(VDataSet & ds) const;

	/// �������� ������������� ������������� (�� ����� ������)
	double GetMaxAnpPass(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ������������ ������������ (�� �������� ������)
	double GetAvgAnpPass(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// �������� ��������� ������������� (�� ����� ������)
	double GetMaxAnpTruck(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� �������� ������������ (�� �������� ������)
	double GetAvgAnpTruck(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// �������� ������������� ��� 3-� �������� (�� ����� ������)
	double GetMaxAnpThird(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ������������ ��� 3-� �������� (�� �������� ������)
	double GetAvgAnpThird(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// �������� ������������� [������� ��������]
	double GetMaxAnpHigh(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ������������ [������� ��������]
	double GetAvgAnpHigh(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// �������� ������������� [������� ��������|������ ������]
	double GetMaxAnpHighBT(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ������������ [������� ��������|������ ������]
	double GetAvgAnpHighBT(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// �������� ������������� ��� (�� ���� ������)
	double GetMaxPsiPass(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// �������� ��������� ��� (�� ���� ������)
	double GetMaxPsiTruck(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// �������� ��� [������� ��������]
	double GetMaxPsiHigh(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// �������� ��� [������� ��������|������ ������]
	double GetMaxPsiHighBT(VDataSet & ds, double * abs, uint32_t radius = COMPLETE_CURVE) const;

	/// �������� V+0.3
	speed_t GetVPlus03(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// �������� V-0.3
	speed_t GetVMinus03(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ���� ��������
	point_t GetTurnPoint(VDataSet & ds) const;

	/// ��������
	double GetTan1(VDataSet & ds) const;
	double GetTan2(VDataSet & ds) const;

	// ---------------------------------------------------------------------
	// �������������� ���������� ������
	// ---------------------------------------------------------------------

	/// ������ � ����� �� ���������� ������
	double GetMaxPlanTap(VDataSet & ds, uint32_t num) const;

	/// ������ � ����� �� ���������� ������
	double GetAvgPlanTap(VDataSet & ds, uint32_t num) const;

	/// ������ ���������� �� ���������� ������
	double GetMaxLevelTap(VDataSet & ds, uint32_t num) const;

	/// ������ ���������� �� ���������� ������
	double GetAvgLevelTap(VDataSet & ds, uint32_t num) const;

    /// ������� ����������� �� ������ ���������� �� ���������� ������
    bool IsLimitTap(VDataSet & ds, bool is_repair, uint32_t trans_num) const;

	// ---------------------------------------------------------------------
	// ���������� ������������
	// ---------------------------------------------------------------------

	/// ���������� ������������ � �����
	double GetPRPlan(VDataSet & ds) const;

	/// ������� ������������ � �����
	int GetDegreePlan(VDataSet & ds) const
	{
		static const float table[] = { 0.5f, 0.3f, 0.2f };
		return this->GetDegree(this->GetPRPlan(ds), table);
	}

	/// ���������� ������������ �� ������
	double GetPRLevel(VDataSet & ds) const;

	/// ������� ������������ �� ������
	int GetDegreeLevel(VDataSet & ds) const
	{
		static const float table[] = { 3.0f, 2.0f, 1.0f };
		return this->GetDegree(this->GetPRLevel(ds), table);
	}

	/// ���������� ������������ �� �������������
	double GetPRAnp(VDataSet & ds) const;

	/// ������� ������������ �� �������������
	int GetDegreeAnp(VDataSet & ds) const
	{
		static const float table[] = { 0.6f, 0.4f, 0.3f };
		return this->GetDegree(this->GetPRAnp(ds), table);
	}

	/// ���������� ������������ �� �����������
	double GetPRCoord(VDataSet & ds) const;

	/// ������� ������������ �� �����������
	int GetDegreeCoord(VDataSet & ds) const
	{
		static const float table[] = { 3.0f, 1.5f, 1.0f };
		return this->GetDegree(this->GetPRCoord(ds), table);
	}

	/// ��������� ���������� ������������ 
	double GetPRSum(VDataSet & ds) const
	{
		return this->GetPRAnp(ds) + this->GetPRPlan(ds) * 0.5f + 
			this->GetPRLevel(ds) * 0.25f + this->GetPRCoord(ds) * 0.2f;
	}

	// ---------------------------------------------------------------------
	// �������� �� ������
	// ---------------------------------------------------------------------

	/// ������ ������������ ��������� �� ������
	std::vector<VCurveSpdArea> GetVprkPass(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������ ������������ ��������� �� ������
	std::vector<VCurveSpdArea> GetVprkTruck(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������ "�������" ��������� �� ������
	/// �� ����� - ���� ��������, ������� �� ������, 60 �� ������� ������
	std::vector<VCurveSpdArea> GetVprkThird(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������ ������� ��������� �� ������
	std::vector<VCurveSpdArea> GetVprkHigh(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������ ������� ��������� �� ������ [� �������� ������]
	std::vector<VCurveSpdArea> GetVprkHighBT(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ������� ���������
	bool HasVprkHigh(VDataSet & ds) const;

	/// ������� ������� ��������� [� �������� ������]
	bool HasVprkHighBT(VDataSet & ds) const;

	/// ������ ��������� V�� ������������
	std::vector<VCurveSpdArea> GetVkrPass(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������ ��������� V�� ��������
	std::vector<VCurveSpdArea> GetVkrTruck(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������ ��������� V�� [������� ��������]
	std::vector<VCurveSpdArea> GetVkrHigh(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������ ��������� V�� [������� ��������|������ ������]
	std::vector<VCurveSpdArea> GetVkrHighBT(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// �������� V�� (������� �� ��������) ��� �������� ���������
	speed_t GetMidVkr(VDataSet & ds, float anpRange, uint32_t radius = COMPLETE_CURVE) const; 

	/// ������ ��������� V�� ������������
	std::vector<VCurveSpdArea> GetVprPass(VDataSet & ds, bool isRepair, uint32_t radius) const;

	/// ������ ��������� V�� ��������
	std::vector<VCurveSpdArea> GetVprTruck(VDataSet & ds, bool isRepair, uint32_t radius) const;

	/// ������ ��������� V�� ������������ 
	std::vector<VCurveSpdArea> GetVizPass(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������ ��������� V�� ��������
	std::vector<VCurveSpdArea> GetVizTruck(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������ ��������� V�� [������� ��������]
	std::vector<VCurveSpdArea> GetVizHigh(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������ ��������� V�� [������� ��������|������ ������]
	std::vector<VCurveSpdArea> GetVizHighBT(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;
	
	// ---------------------------------------------------------------------
	// ������ ��� � ���
	// ---------------------------------------------------------------------
	double GetAnpPassRangeMax() const;
	double GetAnpPassRangeMid() const;
	double GetPsiPassRangeMax() const;
	double GetPsiTruckRangeMax() const;

	// ---------------------------------------------------------------------
	// ������
	// ---------------------------------------------------------------------

	/// ������������ ����� �� ������
	double GetMaxWear(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ����� �� ������
	double GetAvgWear(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ��� ������ �� ������
	double GetSkoWear(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ���������
	double GetCorrWear(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const
	{
		return this->GetAvgWear(ds, radius) + 1.5 * this->GetSkoWear(ds, radius);
	}

	/// ������� ����� �� ������
	double GetWearErrorPercent(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ����� ������� � ����������� ���������� ������
	double GetWearLenOverLimit(VDataSet & ds, float limit, uint32_t radius = COMPLETE_CURVE) const;

	// ---------------------------------------------------------------------
	// �������������� ������
	// ---------------------------------------------------------------------
	
	/// ������������ ���������� ����� �� ������ � �����
	double GetMaxPlanOffset(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ���������� ����� �� ������ � �����
	double GetAvgPlanOffset(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������������ ���������� ����� �� ������ �� ������
	double GetMaxLevelOffset(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	/// ������� ���������� ����� �� ������ �� ������
	double GetAvgLevelOffset(VDataSet & ds, uint32_t radius = COMPLETE_CURVE) const;

	// ---------------------------------------------------------------------
	// ���������� � ����������� ������ ������ � ������ �� �������� ��� ���
	// ---------------------------------------------------------------------
	
	/// ������ ������� �� ������, �� ����������
	std::vector<VCurvePicket> GetPkList(VDataSet & ds, bool isRepair) const;

	/// ������ ������� �� ������, �� ��������
	std::vector<VCurveWearPicket> GetWearPkList(VDataSet & ds) const;

	/// ������ ������� �� ������, � ��������
	std::vector<VCurveLink> GetLinkList(VDataSet & ds) const;

	/// ������ �������� ������ ��� ���
	std::vector<VCurveFPOArea> GetFPOAreaList(VDataSet & ds) const;
	
	///��������� �������
	std::set<density_t> GetWearDiagram(VDataSet & ds) const;

	// ---------------------------------------------------------------------
	// ������� ������ �� ������
	// ---------------------------------------------------------------------

	/// �������� �� ������� - ������������
	const std::vector<double> & GetArrayVprkPass(VDataSet & ds) const;

	/// �������� �� ������� - ��������
	const std::vector<double> & GetArrayVprkTruck(VDataSet & ds) const;

	/// ����
	const std::vector<double> & GetArrayKurs(VDataSet & ds) const;

	/// ������� ��������
	const std::vector<double> & GetArrayMidPlan(VDataSet & ds) const;

	/// ������� ������
	const std::vector<double> & GetArrayMidLevel(VDataSet & ds) const;
	
	// ������������ ��������� (����.)
	const  std::vector<double> & GetArrayAnpHigh(VDataSet& ds) const;
	
	// ������������ ��������� (����. ��)
	const  std::vector<double> & GetArrayAnpHighBT(VDataSet& ds) const;

	/// ������������ ��������� (����.)
	const  std::vector<double> & GetArrayAnpPass(VDataSet& ds) const;

	/// ������������ ��������� (����.)
	const  std::vector<double> & GetArrayAnpTruck(VDataSet& ds) const;

	/// ������
	const std::vector<double> & GetArrayRadius(VDataSet & ds) const;

	/// �������� V�� ������������
	const std::vector<double> & GetArrayVkrPass(VDataSet & ds) const;

	/// �������� V�� ��������
	const std::vector<double> & GetArrayVkrTruck(VDataSet & ds) const;

	/// ��� ������������
	const std::vector<double> & GetArrayPsiPass(VDataSet & ds) const;

	/// ��� ��������
	const std::vector<double> & GetArrayPsiTruck(VDataSet & ds) const;

	/// �������� V�� ������������
	const std::vector<double> & GetArrayVizPass(VDataSet & ds) const;

	/// �������� V�� ��������
	const std::vector<double> & GetArrayVizTruck(VDataSet & ds) const;

	/// ����� ��������
	const std::vector<double> & GetArrayTapPlan(VDataSet & ds) const;

	/// ����� ������
	const std::vector<double> & GetArrayTapLevel(VDataSet & ds) const;

	/// �������� V��
	const std::vector<double> & GetArrayVprPass(VDataSet & ds, bool isRepair) const;
	
	/// �������� ���������
	bool operator < (const VCurve & other)
	{
		if (this->GetMarkCount()>0 && other.GetMarkCount()>0)
			return this->GetPlanMark(0).index < other.GetPlanMark(0).index;
		else if (this->GetLevelMarkCount()>0 && other.GetLevelMarkCount()>0)
			return this->GetLevelMark(0).index < other.GetLevelMark(0).index;
		else
			return false;
	}

	///�������� ������������
	VCurve& operator=(const VCurve& in_curve)
	{		
		model=in_curve.model;
		isModel=in_curve.isModel;
		planMark=in_curve.planMark;
		levelMark=in_curve.levelMark;
		tempParams=in_curve.tempParams;
		tempParams.curve=this;
		return *this;
	}

	// ---------------------------------------------------------------------
	//������� ��������� �������� FixMe!!! ��� ������� ����� ������� ��������
	// ---------------------------------------------------------------------
	
	///����� ��������� ����������
	void Drop();
	///���������� ������� ��� ���������
	double GetCrvArea(VDataSet & ds);
	///�������� ������ ������ ��� ����� ������� ��� ��������� ��������������� ���� ��������
	void FixCrvArea(VDataSet & ds);
	///������� ����� � �������� �� �������
	void TransferMarksToLevel(VDataSet & ds);
	///������������ �������� �� ��������
	void NormalizePlanMarks(VDataSet & ds);
	///������������ �������� �� ������
	void NormalizeLevelMarks(VDataSet & ds);
	///��������� �� ������ ���������� ��������� ��� ��������
	void MNKPlan(VDataSet & ds);
	///��������� �� ������ ���������� ��������� ��� ������
	void MNKLevel(VDataSet & ds);
	///������������ ��������
	void NormalizeMarks(VDataSet & ds);
	///����������� ��������
	void OptimizeMarks(VDataSet & ds);	
	///�������� ���������� ����� �� ������ � ������������ ����������� ����� �� ��������
	void FixLevelCoordinates(VDataSet & ds);
	///��������� ������������ �������� �� ��������
	bool PlanMarksValid() const;
	///��������� ������������ �������� �� ������
	bool LevelMarksValid() const;
	///��������� ���� �� �� ������ ����������
	bool LevelExist(VDataSet & ds) const;
	/// �������� �������� ������ � ���������� ���������
	void AdjustLevelValue();
	///������������� ���������� �� ����
	void ZeroLevel();
	///����������� ����������
	void IncreaseLevel(double in_step=5.0);
	///��������� ����������
	void DecreaseLevel(double in_step=5.0);
	///���������� PRM ��� � ����������� �� �������� is_model
	PARAMETER_TYPE GetParameterType() const {return IsModel() ? PRM_MODEL_CURVES : PRM_CURVE_MARKS;};
	///���������� ����� ���������� ������� ��� �������� ������ �� ��������, �� ���� ����� �� ����� ���� ���������� ����� ���� ������� 
	size_t GetPlanLeftMargin(VDataSet & ds) const;
	///���������� ������ ���������� ������� ��� �������� ������ �� ��������, �� ���� ����� �� ����� ���� ���������� ������ ���� �������
	size_t GetPlanRightMargin(VDataSet & ds) const;
	///���������� ����� ���������� ������� ��� �������� ������ �� ������, �� ���� ����� �� ����� ���� ���������� ����� ���� ������� 
	size_t GetLevelLeftMargin(VDataSet & ds) const;
	///���������� ������ ���������� ������� ��� �������� ������ �� ������, �� ���� ����� �� ����� ���� ���������� ������ ���� �������
	size_t GetLevelRightMargin(VDataSet & ds) const;
	///���������� ����� ���������� ������� ��� ����� �������� �� ��������, �� ���� ����� �� ����� ���� ���������� ����� ���� ������� 
	size_t GetPlanMarkLeftMargin(VDataSet & ds, size_t in_mark_index) const;
	///���������� ������ ���������� ������� ��� ����� �������� ������ �� ��������, �� ���� ����� �� ����� ���� ���������� ������ ���� �������
	size_t GetPlanMarkRightMargin(VDataSet & ds, size_t in_mark_index) const;
	///���������� ����� ���������� ������� ��� ����� �������� �� ������, �� ���� ����� �� ����� ���� ���������� ����� ���� ������� 
	size_t GetLevelMarkLeftMargin(VDataSet & ds, size_t in_mark_index) const;
	///���������� ������ ���������� ������� ��� ����� �������� ������ �� ������, �� ���� ����� �� ����� ���� ���������� ������ ���� �������
	size_t GetLevelMarkRightMargin(VDataSet & ds, size_t in_mark_index) const;					
	///�������������� �������� ������
	void Automark(VDataSet& ds, size_t in_start, size_t in_end, bool in_model=false);	
	///�������������� �������� ������ (������ ���� ������)
	void Automark1(VDataSet& ds, size_t in_start, size_t in_end, bool in_model=false);		
	///����� ���������
	size_t GetElementCount() const
	{
		return this->GetMarkCount() - 1;
	}
	///���������� true ���� ������� - ��������
	bool GetElementType(size_t in_index) const;
	///���������� ����� ��������
	double GetElementLength(VDataSet & ds, size_t in_index) const;
	///���������� ������ ��������
	double GetElementRadius(size_t in_index) const;
	///���������� ����� �������� (�������)
	double GetElementLengthLevel(VDataSet & ds, size_t in_index) const;
	///���������� ���������� ��������
	double GetElementLevel(size_t in_index) const;
	///������������ � ���������� ������ �� 4 ������������ ��������� ��� ��������
	const std::vector<VAnp> GetElementAnp(VDataSet & ds, size_t in_index, int in_speed) const;
	///������������ � ���������� �������� �������� ���������� ������������� ��������� ��� ��������
	float GetElementPsi(VDataSet & ds, size_t in_index, int in_speed) const;
	///���������� ����� ����������
	float GetElementTap(VDataSet & ds, size_t in_index) const;
	///���������� V�� ��� ��������
	double GetElementVkrHighBT(VDataSet & ds, size_t in_index) const;
	///���������� V�� ��� ��������
	double GetElementVkrHigh(VDataSet & ds, size_t in_index) const;
	///���������� V�� ��� ��������
	double GetElementVkrPass(VDataSet & ds, size_t in_index) const;
	///���������� V�� ��� ��������
	double GetElementVkrTruck(VDataSet & ds, size_t in_index) const;
	///���������� V�� ��� ��������
	double GetElementVpr(VDataSet & ds, size_t in_index) const;
	///���������� V�� ��� ��������
	double GetElementVizHighBT(VDataSet & ds, size_t in_index) const;
	///���������� V�� ��� ��������
	double GetElementVizHigh(VDataSet & ds, size_t in_index) const;
	///���������� V�� ��� ��������
	double GetElementVizPass(VDataSet & ds, size_t in_index) const;
	///���������� V�� ��� ��������
	double GetElementVizTruck(VDataSet & ds, size_t in_index) const;
	///���������� ��� ����. �� ��� ��������
	double GetElementAnpHighBT(VDataSet & ds, size_t in_index) const;
	///���������� ��� ����. ��� ��������
	double GetElementAnpHigh(VDataSet & ds, size_t in_index) const;
	///���������� ��� ����. ��� ��������
	double GetElementAnpPass(VDataSet & ds, size_t in_index) const;
	///���������� ��� ����. ��� ��������
	double GetElementAnpTruck(VDataSet & ds, size_t in_index) const;
	///���������� ��� �����. ��� ��������
	double GetElementAnpThird(VDataSet & ds, size_t in_index) const;
	///���������� ��� ���
	float GetElementPsiHighBT(VDataSet & ds, size_t in_index) const;
	///���������� ��� ��
	float GetElementPsiHigh(VDataSet & ds, size_t in_index) const;
	///���������� ��� ����
	float GetElementPsiPass(VDataSet & ds, size_t in_index) const;	
	///���������� ��� ����
	float GetElementPsiTruck(VDataSet & ds, size_t in_index) const;
	///���������� ��� ��
	float GetElementPsiThird(VDataSet & ds, size_t in_index) const;	
	///������������ � ���������� ������ ������������ ���������
	const std::vector<VAnp>& GetElementAnp(const VAbsCoordData & in_abscoord, const VAprinfo& in_aprinfo, 
		speed_type in_speed, bool in_use_passport_speeds) const;			
	///������������ ����������� ����������
	void CreateLevelOptimal(VDataSet & ds);
	///������������ ���������� ������� � �����
	void CreateLevelFact(VDataSet & ds);
	///������������ ���������� ������������
	void CreateLevelRational(VDataSet & ds);
	/// ���������� true, ���� ����� � �������� �������� ������ ������ ������
	int HasPointCrv(size_t in_index) const;
	/// ���������� true, ���� ����� � �������� �������� ������ ������ ������
	int HasPointLvl(size_t in_index) const;
	/// ������������� �������� ��� ������ (�������� �� �������)
	void ResetModelSpeeds(VDataSet & ds);
};

///������� ������� ��� ��������� �����������
class VCriterionFunction : public VFunction
{
private:
	///��������� �� �������������� ������ FixMe!!! ����� shared_ptr
	VCurve * curve;
	///��������� �� ������������ DataSet FixMe!!! ����� shared_ptr
	VDataSet * data_set;	
public :
	/// �����������
	VCriterionFunction(VCurve * in_curve, VDataSet * in_data_set);
	/// �����, ��������������� �������� ��������� � ������� index ������ value
	void SetParam(size_t index, double value);
	/// �����, ������������ �������� ��������� � ������� index		
	double GetParam(size_t index);
	/// �����, ��������������� ������� ��������� � ������� index ������ value		
	void SetParamRange(size_t index, const pro::interval<double>& value);
	/// �����, ������������ ������� ��������� � ������� index		
	pro::interval<double> GetParamRange(size_t index);
	/// �����, ����������� ������� �������� �������		
	void Update();
	/// �����, ������������ ������� �������� �������		
	double Value();
	/// �����, ������������ ���������� ����������		
	size_t GetParamCount();
};
