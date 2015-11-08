#pragma once


#include "rtparam.h"

#include "processing.h"
#include "situation.h"
#include "options.h"
#include "tables.h"
#include "signal_processing.h"
#include "piecewise.h"

//boost
#include <new>
#include <boost/utility.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/covariance.hpp>
#include <boost/accumulators/statistics/variates/covariate.hpp>
#include <boost/accumulators/statistics/skewness.hpp>
#include <boost/accumulators/statistics/kurtosis.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>


//shiny
#include "Shiny.h"

using namespace boost::accumulators ;

// ��������������� ������� 

template< typename DEFAULT_INITIALIZABLE >
inline void clear( DEFAULT_INITIALIZABLE& object )
{
    object.DEFAULT_INITIALIZABLE::~DEFAULT_INITIALIZABLE() ;
    ::new ( boost::addressof(object) ) DEFAULT_INITIALIZABLE() ;
}

/// ��������� ��������
class VRichCorrector
{
public:
	virtual double Correct(DEGREE in_degree, int in_speed, double in_value, double in_length) const = 0;
};
typedef boost::shared_ptr<VRichCorrector> VRichCorrectorPtr;

/// ������ ������� �������
class VRichPaletRecord
{
private:
	/// �������
	DEGREE degree;
	/// �������� ���������
	pro::interval<int> speed;
	/// ������� ������� ��������� � ����������� �� ����� ����������
	pro::piecewise::linear top;
	/// ������ ������� ��������� � ����������� �� ����� ����������
	pro::piecewise::linear bottom;
public:
	/// �����������
	VRichPaletRecord(DEGREE in_degree, const pro::interval<int>& in_speed, 
		const pro::piecewise::linear& in_bottom, const pro::piecewise::linear& in_top) : 
		degree(in_degree), speed(in_speed), bottom(in_bottom), top(in_top) {};
	/// ������� ��� ������
	class VFindKey
	{
	private:
		DEGREE degree;
		int speed;
	public:
		VFindKey(DEGREE in_degree, int in_speed) : degree(in_degree), speed(in_speed){};
		bool operator()(const VRichPaletRecord& in_key) const
		{
			return in_key.IsKey(degree, speed);
		}
	};		
	/// ���������� true, ���� ������� ������ ������������� �����
	bool IsKey(DEGREE in_degree, int in_speed) const
	{
		return (degree==in_degree) && speed.contains(in_speed);
	};
	/// ���������� �������� ��������
	const pro::interval<double> GetRange(double in_length) const
	{
		if (bottom.value_in_domain(in_length) && top.value_in_domain(in_length))
			return pro::interval<double>(bottom.y(in_length), top.y(in_length), false, true);
		else
			return EMPTY_INTERVAL<double>();			
	};
};

/// �������
class VRichPalet : public VRichCorrector
{
private:
	/// �������
	std::vector<VRichPaletRecord> table;	
public:
	/// �����������
	VRichPalet();
	/// ���������� ��������
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed, double in_length) const;
	/// ������� ���������
	double Correct(DEGREE in_degree, int in_speed, double in_value, double in_length) const;
};

/// ������� ��������� ��������
class VRichCorrectionTable  : public VRichCorrector
{
private:
	/// ������� �������� �������
	pro::piecewise::linear function;
public:
	/// �����������
	VRichCorrectionTable();
	/// ������� ���������
	double Correct(DEGREE in_degree, int in_speed, double in_value, double in_length) const;
};

/// ��� ���������
class VRichCorrectionNone  : public VRichCorrector
{
private:	
public:
	/// �����������
	VRichCorrectionNone() {};
	/// ������� ���������
	double Correct(DEGREE in_degree, int in_speed, double in_value, double in_length) const {return in_value;};
};

/// ������ �����������
enum OVERSTEP_GROUP
{
	GROUP_NONE=0,
	GROUP_GEOMETRY=1,
	GROUP_EXTRA=2,
	GROUP_SERVICE_GEOMETRY=3,
	GROUP_SERVICE_EXTRA=4,
	GROUP_EXPRESS=5
};

/// ���������� ����� �� ��������� � ���������
static double RoundNormaWidth(double in_norma)
{
	double result=1520;
	std::vector<double> norma;
	norma.push_back(NORMA_WIDTH_1520);
	norma.push_back(NORMA_WIDTH_1524);
	norma.push_back(NORMA_WIDTH_1530);
	norma.push_back(NORMA_WIDTH_1535);
	norma.push_back(NORMA_WIDTH_1540);	
	for (size_t i=1; i<norma.size(); ++i)
	{
		if (in_norma>=norma.at(i-1) && in_norma<=norma.at(i))
		{
			return fabs(in_norma-norma.at(i-1)) < fabs(in_norma-norma.at(i)) ? norma.at(i-1) : norma.at(i);
		}
	}
	return result;
};

/// ����������� ����� �����������
#define WIDEN L"��"
#define NARROW L"���"
#define OTVOD_WIDTH L"���"
#define WEAR_LEFT L"���.��"
#define WEAR_RIGHT L"���.��"
#define WEAR L"���"
#define PIT_LEFT L"��.��"
#define PIT_RIGHT L"��.��"
#define PIT_LEFT_GROUP L"3��.��"
#define PIT_RIGHT_GROUP L"3��.��"
#define LEVEL L"�"
#define SHORT_DISTORTION L"��"
#define DISTORTION L"�"
#define DISTORTION_GROUP L"3�"
#define RICH_LEFT L"�.��"
#define RICH_RIGHT L"�.��"
#define RICH L"�"
#define RICH_NR L"���"
#define RICH_GROUP L"3�"
#define RICH_GROUP_NR L"3���"
#define RICH_LEFT_GROUP L"3�.��"
#define RICH_RIGHT_GROUP L"3�.��"
#define RICH_PIT_COMBINATION  L"�+��"
#define RICH_DISTORTION_COMBINATION  L"�+�"
#define ANP L"���"
#define PSI L"���_��"
#define ZAZOR_LEFT L"���.��"
#define ZAZOR_RIGHT L"���.��"
#define ZERO_ZAZOR_LEFT L"��.��"
#define ZERO_ZAZOR_RIGHT L"��.��"

#define WIDTH_CORRUPT L"���� ��"
#define PIT_LEFT_CORRUPT L"���� ��.��"
#define PIT_RIGHT_CORRUPT L"���� ��.��"
#define RICH_LEFT_CORRUPT L"���� ����.��"
#define RICH_RIGHT_CORRUPT L"���� ����.��"
#define WEAR_LEFT_CORRUPT L"���� ���.��"
#define WEAR_RIGHT_CORRUPT L"���� ���.��"
#define WEAR_CORRUPT L"���� ���."
#define LEVEL_CORRUPT L"���� ��"

#define WIDTH_CORRUPT_PK L"���� �� ��"
#define PIT_LEFT_CORRUPT_PK L"���� ��.�� ��"
#define PIT_RIGHT_CORRUPT_PK L"���� ��.�� ��"
#define RICH_LEFT_CORRUPT_PK L"���� �.�� ��"
#define RICH_RIGHT_CORRUPT_PK L"���� �.�� ��"
#define WEAR_CORRUPT_PK L"���� ���. ��"
#define LEVEL_CORRUPT_PK L"���� ��. ��"

#define CONTROL_WIDTH_MO L"���"
#define CONTROL_LEVEL_MO L"��"
#define CONTROL_WIDTH_SKO L"��� ���"
#define CONTROL_LEVEL_SKO L"�� ���"
#define W10 L"�10"
#define K100 L"�100"
#define K60 L"�60"

#define ANP_AVERAGE L"���"
#define ANP_HORIZONTAL L"��"
#define PSI_HORIZONTAL L"���"
#define PSI_TRANSITION L"��� ��"
#define OTVOD_LEVEL L"���"

#define FALLBACK L"����."

#define WIDTH_OFF L"����. ��"
#define PIT_LEFT_OFF L"����. ��.��"
#define PIT_RIGHT_OFF L"����. ��.��"
#define RICH_LEFT_OFF L"����. ����.��"
#define RICH_RIGHT_OFF L"����. ����.��"
#define LEVEL_OFF L"����. ��"
#define WEAR_LEFT_OFF L"����. ���.��"
#define WEAR_RIGHT_OFF L"����. ���.��"
#define ZAZOR_LEFT_OFF L"����. ���.��"
#define ZAZOR_RIGHT_OFF L"����. ���.��"
#define MAG_ZAZOR_LEFT_OFF L"����. ���.����.��"
#define MAG_ZAZOR_RIGHT_OFF L"����. ���.����.��"




/// ����������� �����
#define BRIDGE_SIGN L"� "
/// ����������� �������
#define SWITCH_SIGN L"���. "
/// ����������� �������������� �������
#define CHAMFERED_JOINT_SIGN L"��.��. "
/// ����������� ����� ��� 
#define NTD_SIGN L"��� "
/// ����������� ����� ������������ 283�
#define N283_SIGN L"283� "
/// ����������� ���
#define PRJ_SIGN L"��� "
/// ����������� �������
#define TEST_RUN_SIGN L"������� "
/// ����������� �� ���� �� 96 �.
#define ZBSH_SIGN L"��96"
/// ����������� ����������� �� �������� ��������
#define CLOSURE_CORRECTION_SIGN L"���. ��."
/// ����������� �������� ����������
#define INACCURATE_COORDINATE_SIGN L"�������� ����������"
/// ����������� �������������� ��������
#define BAD_PASSPORT_SIGN L"����."

/// ������������ ��������
#define MAX_SPEED 300
/// ��� ��������� ��������
#define SPEED_QUANTUM 5
/// ���������� �������� ������� (��������� ��� �� ����� ���������� �� ������� ����� �� ����� ���� ���������������� ����� �����������)
#define DECISION_DISTANCE 150.0
/// ������������ �������� �����������
#define MAX_OVERSTEP_LENGTH 100.0
/// ����� �������� 3-� ������� � 4-�
#define CLOSE_TO_UPPER_BOUND_VALUE 1.0

/// �������� ����������� �� �������� ��������
#define CLOSURE_CORRECTION_SPEED 25

/// �������� �������� �� ������� �50
#define P50_CORRECTION_VALUE 1.5


/// �������� �� ��������� (������ ����� �������)
#define DEFAULT_PRECISION 0
/// �������� ��� (������ ����� �������)
#define ANP_PRECISION 2
/// �������� ��� (������ ����� �������)
#define PSI_PRECISION 2
/// �������� ������ (������ ����� �������)
#define WEAR_PRECISION 0
/// �������� ����������� �������� (������ ����� �������)
#define CONTROL_SECTION_PRECISION 2
/// �������� ������ ��
#define OTVOD_WIDTH_PRECISION 2
/// �������� ������ ����������
#define OTVOD_LEVEL_PRECISION 2

//---------------------------- ���� ���������� -------------------------

/// ����� �������� ������� (�) FixMe!!! ���� �������� ����� ���� ������������� � ������ ��� ������ ����������
#define CORRUPT_LENGTH 10.0
/// ������� ����� �� ������
#define CORRUPT_PERCENT_PK 50.0

//---------------------------- ������ ����� ---------------------------

/// ����������� ����� ����������� ��
#define MIN_WIDTH_LENGTH 1.0
/// ������������ ����� ����������� �� ��
#define MAX_WIDTH_LENGTH 300.0
/// ���� ���������� ������ ��� ��������� �������� ������ �������
#define WEAR_CORRECTION_BASE 2.0
/// ������������ �������� ������ ��� ��������� �������� ������ �������
#define MAX_WEAR_CORRECTION 15.0
/// ������������ ������ ������ � ������� �������� ��������� 
#define MAX_RADIUS_CORRECTION 1200.0
/// ����� ������������� �����������
#define WIDTH_ELEMENTARY_LENGTH 4.0
/// ������������ ������ ������ ��� �������� ��������� �������� �� ���������� ������
#define MAX_CORRECTION_RADIUS_FOR_HIGHSPEED_LINE 3000

//----------------------------- ����� �� ---------------------------------

/// ���� ���������� ������ ��
#define OTVOD_WIDTH_BASE 2.0
/// ����������� ����� ������
#define OTVOD_WIDTH_QUANTUM 2.0

//----------------------------- ����� ���������� --------------------------------

/// ����������� ����� ������
#define OTVOD_LEVEL_QUANTUM 2.0

//----------------------------- ����� --------------------------------

/// ����������� ����� ����������� �� ������
#define WEAR_QUANTUM 1.0
/// ������������ ����� ����������� �� ������
#define MAX_WEAR_LENGTH 300.0


//----------------------------- �������� -------------------------------

/// ���� ������ ���������� ��������
#define PIT_EXTREMUM_BASE 5.0
/// ���������� �������� �������
#define PIT_DECISION_DISTANCE 50.0

//----------------------------------- ������� -------------------------------------

/// ����������� ����� ����������� �� ������
#define LEVEL_QUANTUM 10.0
/// ����� ���������� ����������� �� ������
#define LEVEL_ELEMENTARY_LENGTH 30.0
/// ����� ���������� ����������� �� ������ ��-515
#define LEVEL_ELEMENTARY_LENGTH_515 10.0
/// �������� ���� ����������� �� ������
#define MAX_LEVEL_LENGTH 300.0

//----------------------------- ����� ������ ---------------------------------

/// ���� ���������� ������ ������
#define SHORT_DISTORTION_BASE 2.5

//---------------------------------- �������� -------------------------------------

/// ���� ������ ���������� ��������
#define DISTORTION_EXTREMUM_BASE 5.0
/// ���������� �������� �������
#define DISTORTION_DECISION_DISTANCE 100.0

//--------------------------------- �������� ------------------------------------

/// ���� ������ ���������� ��������
#define RICH_EXTREMUM_BASE 5.0
/// ���������� �������� �������
#define RICH_DECISION_DISTANCE 100.0


//--------------------------------- ��������� ----------------------------------------

/// ����� ������������� �����������
#define PERIODICAL_COUNT 3

/// ����� �������� � ������
#define PIT_GROUP_LENGTH DBL_MAX
/// �������� �������� � ������
#define PIT_GROUP_VALUE 20.0
/// �������� �������� � ������
#define PIT_GROUP_VALUE_515 15.0
/// ����� ������ ��������
#define PIT_GROUP_DISTANCE 10.0

/// ����� ��������� � ������
#define DISTORTION_GROUP_LENGTH 10.0
/// �������� ��������� � ������
#define DISTORTION_GROUP_VALUE 16.0
/// ����� ������ ���������
#define DISTORTION_GROUP_DISTANCE 30.0

/// ����� �������� � ������
#define RICH_GROUP_LENGTH 20.0
/// �������� �������� � ������
#define RICH_GROUP_VALUE 25.0
/// ����� ������ ��������
#define RICH_GROUP_DISTANCE 30.0

/// ���������� ����� ������������� � ���������
#define COMBINATION_DISTANCE 10.0
/// �������� �������� � ���������
#define COMBINATION_PIT_VALUE 15.0
/// �������� �������� � ���������
#define COMBINATION_RICH_VALUE 15.0
/// ����������� ������������� �������� ��� ������� ����������� ���������
#define MIN_COMBINATION_SPEED 60

//------------------------------------ ��� -------------------------------------------------

// ����������� ����� �����������
#define ANP_QUANTUM 1.0
/// ����� ���
#define MAX_ANP 0.7

//-------------------------------- �������� ��������� ��� ---------------------------------

/// ����������� ����� �����������
#define PSI_QUANTUM 1.0
/// ����� ���
#define MAX_PSI 0.6
/// ����� ��� ��� �����������
#define MAX_PSI_RESTRICTION 0.66
/// ����� ��� ��� ������������ ������� � �������� ������
#define MAX_PSI_BT_RESTRICTION 0.9

//--------------------------------- ������ -----------------------------------------------

/// ����� ������ (� ������� 1 �)
#define RAIL_LENGTH 26.0

//--------------------------- ��������� ���������� ---------------------------------------

/// ������������ ����� ������� ���������
#define MAX_CORRECTION_BUFFER_LENGTH 1200.0

/// ����������� ����� ������� ���������
#define MIN_CORRECTION_BUFFER_LENGTH 100.0

//----------------------------------- ������� ���������� �� ������ -----------------------------------------

#define EXCLUSION_REASON_SWITCH L"������� ��� ������������� ������"
#define EXCLUSION_REASON_HARDWARE_FAIL L"���� ����������"
#define EXCLUSION_REASON_BAD_PASSPORT L"�������������� ���������� ������"
#define EXCLUSION_REASON_BAD_COORDINATES L"������ ��������"
#define EXCLUSION_REASON_PROGRAM_ERROR L"������ ���"
#define EXCLUSION_REASON_SIDEWAY L"����� �� ������� ����"

//--------------------------------------------- ��������� -----------------------------------------------------

/// ���������� �������� ������� � ���������� ���������
#define KM_READY_DISTANCE 300.0;

/// ��������� ��������
class VPitExtractor
{
private:
	/// �������
	side_t side;
public:
	/// �����������
	VPitExtractor(side_t in_side);
	/// ��������()
	VPoint operator()(const VRtParameter& in_record) const;
};

/// ��������� ��
class VWidthExtractor
{
public:
	VPoint operator()(const VRtParameter& in_record) const;
};

/// ��������� ��������
class VRichExtractor
{
private:
	/// �������
	side_t side;
public:
	/// �����������
	VRichExtractor(side_t in_side);
	/// ��������()
	VPoint operator()(const VRtParameter& in_record) const;
};

/// ��������� �������
class VLevelExtractor
{
public:
	/// ��������()
	VPoint operator()(const VRtParameter& in_record) const;
};

/// ��������� ��������
class VCurvExtractor
{
public:
	/// ��������()
	VPoint operator()(const VRtParameter& in_record) const;
};


/// ��������� �����
class VWearExtractor
{
private:
	/// �������
	side_t side;
public:
	/// �����������
	VWearExtractor(side_t in_side);
	/// �����������
	VWearExtractor();
	/// operartor()
	VPoint operator()(const VRtParameter& in_record) const;
};

/// ��������� �����
class VZazorExtractor
{
private:
	/// �������
	side_t side;
public:
	/// �����������
	VZazorExtractor(side_t in_side);
	/// operartor()
	VPoint operator()(const VRtParameter& in_record) const;
};

/// ��������� ����������� ������
class VTemperatureExtractor
{
private:
	/// �������
	side_t side;
public:
	/// �����������
	VTemperatureExtractor(side_t in_side);
	/// operartor()
	VPoint operator()(const VRtParameter& in_record) const;
};


/// ��������� ���������� �� ������� ����� ���������� �� ��������
class VLevelPassportVariate : public VDataProcessor<VPoint, VPoint>
{
private:	
	/// ������� ������ �� �����
	std::deque<VPoint> input;
	/// ������� ������ �� ������
	std::queue<VPoint> output;
public:
	/// �����������
	VLevelPassportVariate();
	/// ����������
	~VLevelPassportVariate();
	/// ���� ������
	void Put(const VPoint& in_point);	
	/// ���������� true, ���� ���� ������������ ������
	virtual bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VPoint& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();
};

/// ��������� ���������� �� ������� ����� ��������
class VRichPassportVariate : public VDataProcessor<VPoint, VPoint>
{
private:
	/// �������
	side_t side;
	/// ������� ������ �� �����
	std::deque<VPoint> input;
	/// ������� ������ �� ������
	std::queue<VPoint> output;
public:
	/// �����������
	VRichPassportVariate(side_t in_side);
	/// ����������
	~VRichPassportVariate();
	/// ���� ������
	void Put(const VPoint& in_point);	
	/// ���������� true, ���� ���� ������������ ������
	virtual bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VPoint& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();
};

typedef boost::shared_ptr< VDataProcessor<VPoint, VPoint> > VPointToPointProcessorPtr;
typedef boost::shared_ptr< VDataProcessor<VExtremum, VWave> > VExtremumToWaveProcessorPtr;

// ������� ����������
class VOverstepCondition
{
public:
	/// ��������� ���������� �������
	virtual bool TestValue(double in_value, double in_length) const = 0;	
};

/// �������� �����������
class VBasicOverstep
{
protected:
	/// ���������� ������
	double start;
	/// ���������� �����
	double end;	
	/// ������� ���������� �����������
	VWayCoord waycoord;
	/// ������������ ��������
	double max_value;
	/// ����������� ��������
	double min_value;		
	/// �������� �����������
	double value;	
	/// �����
	double length;
	/// �������� ���� �����������
	std::wstring type;
	/// ������� �����������
	DEGREE degree;
	/// �������� � ������� ������� (� ��������� �������)
	bool close_to_upper_bound;
	/// C������� �� �������
	VSpeeds speeds;
	/// ����������� ��������
	VSpeeds restriction;	
	/// �����������
	std::wstring comment;	
	/// �����
	int penalty;
	/// ������� ����, ��� ����������� ������ ���� ��������� �� ������
	bool is_excluded;
	/// ������� ���������� �� ������
	std::wstring exclusion_reason;
	
public:	
	/// �����������
	VBasicOverstep();
	/// �����������
	VBasicOverstep(double in_start, double in_end, double in_max_value, double in_min_value, 
		double in_value, double in_length, const std::wstring& in_type, DEGREE in_degree=DEGREE_NONE);		
	///����������
	virtual ~VBasicOverstep(){};
	/// ���������� ���������� ������
	virtual double GetStart() const;
	/// ���������� ���������� �����
	virtual double GetEnd() const;
	/// ���������� ������������ ��������
	virtual double GetMax() const;
	/// ���������� ����������� ��������
	virtual double GetMin() const;		
	/// ���������� ������� ��������
	virtual double GetAverage() const;
	/// ���������� ����������
	virtual double GetCoord() const;
	/// ���������� ����������
	virtual const VWayCoord& GetWayCoord() const;
	/// ���������� ���������� ��������
	virtual double GetValue() const;
	/// ���������� �����
	virtual double GetLength() const;
	/// ������������� �������
	virtual void SetDegree(DEGREE in_degree);	
	/// ���������� �������
	virtual DEGREE GetDegree() const;
	/// ���������� ������� �������� � ��������� �������
	virtual bool CloseToUpperBound() const;
	/// ��������� ��������
	virtual std::wstring GetDescription() const;	
	/// ��������� �����������
	virtual void AddComment(const std::wstring& in_comment);
	/// ���������� �����������
	virtual std::wstring GetComment() const;
	///���������� true, ���� ������� ����������� �������������
	virtual bool Intersection(const VBasicOverstep& in_overstep) const;
	///���������� ���������� ����� ����� �������������
	virtual double Distance(const VBasicOverstep& in_overstep) const;
	/// ���������� true, ���� ����������� ���������� ����
	virtual bool IsType(const std::wstring& in_type) const;		
	/// ������� ��� �����������
	virtual const std::wstring& GetType() const;
	/// ���������� ������
	virtual const OVERSTEP_GROUP GetGroup() const;
	/// ������� ��������
	virtual const VSpeeds& GetSpeeds() const;	
	/// ������� ����������� ��������
	virtual const VSpeeds& GetRestriction() const;
	/// ���������� ����������� ��������
	virtual void SetRestriction(const VSpeeds& in_speeds);
	/// �������� �������� ��������
	virtual void CorrectClosure();
	/// ���������� true ���� �������� ����������
	virtual bool HasRestriction() const;	
	/// ���������� ����� ������������ �����������
	virtual size_t GetAmount() const;
	/// ���������� �������� ������
	virtual int GetPenalty() const;	
	/// ��������� ����������� �� ������
	virtual void Exclude(const std::wstring& in_reason);
	/// ���������� true ���� ����������� ������ ���� ��������� �� ������
	virtual bool IsExcluded() const;
	/// ���������� ������� ���������� �� ������
	virtual const std::wstring& GetExclusionReason() const;

	// ����� ���������

	/// ���������� ������ � ��������� ����������
	virtual std::wstring GetCoordString() const;
	/// ���������� ������ � ��������� ����
	virtual std::wstring GetTypeString() const;
	/// ���������� ������ � ��������� �������
	virtual std::wstring GetDegreeString() const;
	/// ���������� ������ � ��������� ��������
	virtual std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	virtual std::wstring GetLengthString() const;
	/// ���������� ������ � ��������� ���������� ������������ �����������
	virtual std::wstring GetAmountString() const;
	/// ���������� ������ � �������� �������
	virtual std::wstring GetPenaltyString() const;
	/// ���������� �������� ����������� ��������
	virtual std::wstring GetRestrictionString() const;		
};
typedef boost::shared_ptr<VBasicOverstep> VBasicOverstepPtr;

static bool operator<(const VBasicOverstep& l, const VBasicOverstep& r)
{
	return l.GetCoord() < r.GetCoord();
}

static bool operator<(const VBasicOverstepPtr& l, const VBasicOverstepPtr& r)
{
	return l->GetCoord() < r->GetCoord();
}

static bool operator>(const VBasicOverstep& l, const VBasicOverstep& r)
{
	return l.GetCoord() > r.GetCoord();
}

static bool operator>(const VBasicOverstepPtr& l, const VBasicOverstepPtr& r)
{
	return l->GetCoord() > r->GetCoord();
}

/// �������� ��� ������ ������� �����������
struct worst_overstep
{
public:
	bool operator()(const VBasicOverstepPtr& l, const VBasicOverstepPtr& r)
	{
		return l->GetDegree()<r->GetDegree() || l->GetDegree()==r->GetDegree() && l->GetValue()<r->GetValue();
	};
};

/// ��������� �������� �� ����������� ����������� �� ��������
static bool is_rich(const VBasicOverstepPtr& in_overstep) {return in_overstep->IsType(RICH_LEFT) || in_overstep->IsType(RICH_RIGHT);};
/// ��������� �������� �� ����������� ���������
static bool is_pit(const VBasicOverstepPtr& in_overstep) {return in_overstep->IsType(PIT_LEFT) || in_overstep->IsType(PIT_RIGHT);};
/// ��������� �������� �� ����������� ���������
static bool is_distortion(const VBasicOverstepPtr& in_overstep) {return in_overstep->IsType(DISTORTION);};

/// ����������� - ���� ����������
class VFallbackOverstep : public VBasicOverstep
{
private:
	/// ���������
	std::wstring message;
	/// ������
	OVERSTEP_GROUP group;
public:
	
	/// �����������
	VFallbackOverstep(double in_coord, OVERSTEP_GROUP in_group, const std::wstring& in_type)
	{
		start=in_coord;
		end=in_coord;
		waycoord=SITUATION.GetRealCoord(in_coord);
		max_value=0;
		min_value=0;
		value=0;
		length=0;
		type=in_type;
		degree=DEGREE_NONE;
		group=in_group;
	}
	
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const
	{
		return L"";
	}
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const
	{
		return L"";
	}
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return group;};
};

/// ����������� - ���� ����������
template<OVERSTEP_GROUP group> class VCorruptOverstep : public VBasicOverstep
{	
public:
	/// �����������
	VCorruptOverstep(double in_start, double in_end, const std::wstring& in_type)
	{
		start=in_start;
		end=in_end;
		waycoord=SITUATION.GetRealCoord((start+end)/2);
		max_value=0;
		min_value=0;
		value=0;
		length=fabs(in_start-in_end);
		type=in_type;
		degree=DEGREE_NONE;
	}
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const
	{
		return L"";
	}
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return group;};
};

/// ����������� - ���� ���������� �� ������
template<OVERSTEP_GROUP group> class VPkCorruptOverstep : public VBasicOverstep
{	
public:
	/// �����������
	VPkCorruptOverstep(double in_start, double in_end, double in_value, const std::wstring& in_type)
	{
		start=in_start;
		end=in_end;
		waycoord=SITUATION.GetRealCoord((start+end)/2);
		max_value=0;
		min_value=0;
		value=in_value;
		length=fabs(in_start-in_end);
		type=in_type;
		degree=DEGREE_NONE;
	}
	/// ���������� ������ � ��������� ���������
	std::wstring GetCoordString() const
	{
		return boost::str(boost::wformat(L"��%d") % SITUATION.GetPk(GetCoord()).pk);
	}
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const
	{
		return L"";
	}
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const
	{
		return boost::str(boost::wformat(L"%0.0f%%") % GetValue());
	}
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return group;};
};


/// ����������� - ����������� �������
class VControlSectionOverstep : public VBasicOverstep
{
private:
	/// ������� �������� (����)
	double value_real;
	/// ������� �������� (����)
	double value_apr;	
	
public:
	/// �����������
	VControlSectionOverstep(double in_start, double in_end, double in_value_real, 
		double in_value_apr, const std::wstring& in_type);
	/// ���������� ����������
	double GetCoord() const;
	/// ���������� ������ � ��������� ����
	std::wstring GetTypeString() const;
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// ���������� ������ � ��������� ���������� ������������ �����������
	std::wstring GetAmountString() const;
	/// ���������� �������� ����������� ��������
	std::wstring GetRestrictionString() const;	
	/// ���������� �����������
	std::wstring GetComment() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_SERVICE_GEOMETRY;};
};

/// ����������� �� ��������
class VWidenOverstep : public VBasicOverstep
{
private:
	/// ������� ���������� �� �������
	bool on_switch;
	/// ������� ���������� � ���� �������������� �������
	bool on_chamfered_joint;
	/// �����
	double norma;
	/// ������ ������
	double radius;
public:	
	/// �����������
	VWidenOverstep(const VExcess& in_excess, DEGREE in_degree);	
	/// ���������� ����� ������������ �����������
	size_t GetAmount() const;
	/// ���������� ������ � ��������� ���������� ������������ �����������
	std::wstring GetAmountString() const;	
	/// ���������� �������� ������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VWidenOverstep> VWidenOverstepPtr;


/// ����������� ����������� �� ��������
class VWidenExpress : public VBasicOverstep
{
public:
	/// �����������
	VWidenExpress(const VExcess& in_excess);
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXPRESS;};
	/// ���������� �������� ����������� ��������
	std::wstring GetRestrictionString() const {return L"";};		
};
typedef boost::shared_ptr<VWidenExpress> VWidenExpressPtr;

/// ����������� �� �������
class VNarrowOverstep : public VBasicOverstep
{
private:
	/// ������� ���������� �� �������
	bool on_switch;
	/// ������� ���������� � ���� �������������� �������
	bool on_chamfered_joint;
	/// �����
	double norma;
	/// ������� ���
	bool zbsh;
public:
	/// �����������
	VNarrowOverstep(const VExcess& in_excess, DEGREE in_degree);	
	/// ���������� ����� ������������ �����������
	size_t GetAmount() const;
	/// ���������� ������ � ��������� ���������� ������������ �����������
	std::wstring GetAmountString() const;	
	/// �������� ������ �����������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VNarrowOverstep> VNarrowOverstepPtr;

/// �������� ������ �������
class VNarrowExpress : public VBasicOverstep
{
public:
	/// �����������
	VNarrowExpress(const VExcess& in_excess);
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXPRESS;};
	/// ���������� �������� ����������� ��������
	std::wstring GetRestrictionString() const {return L"";};
};
typedef boost::shared_ptr<VNarrowExpress> VNarrowExpressPtr;

/// ����������� �10
class VW10Overstep : public VBasicOverstep
{
private:
	/// ����� �����������
	double amount;
public:
	/// �����������
	VW10Overstep(double in_start, double in_end, double in_amount);	
	/// ���������� ����� ������������ �����������
	size_t GetAmount() const;	
	/// ���������� �������� ������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const;
	/// ���������� ������ � ��������� ����������
	std::wstring GetCoordString() const;	
	/// ���������� ������ � ��������� �������
	std::wstring GetDegreeString() const;
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// ���������� ������ � ��������� ���������� ������������ �����������
	std::wstring GetAmountString() const;
	/// ���������� �������� ����������� ��������
	std::wstring GetRestrictionString() const;		
};
typedef boost::shared_ptr<VW10Overstep> VW10OverstepPtr;

/// ����������� �100
class VK100Overstep : public VBasicOverstep
{
private:
	/// ����� �����������
	double amount;
public:
	/// �����������
	VK100Overstep(double in_start, double in_end, double in_amount);	
	/// ���������� ����� ������������ �����������
	size_t GetAmount() const;	
	/// ���������� �������� ������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const;
	/// ���������� ������ � ��������� ����������
	std::wstring GetCoordString() const;	
	/// ���������� ������ � ��������� �������
	std::wstring GetDegreeString() const;
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// ���������� ������ � ��������� ���������� ������������ �����������
	std::wstring GetAmountString() const;
	/// ���������� �������� ����������� ��������
	std::wstring GetRestrictionString() const;		
};
typedef boost::shared_ptr<VK100Overstep> VK100OverstepPtr;


/// ����������� �60
class VK60Overstep : public VBasicOverstep
{
private:
	/// ����� �����������
	double amount;
public:
	/// �����������
	VK60Overstep(double in_start, double in_end, double in_amount);	
	/// ���������� ����� ������������ �����������
	size_t GetAmount() const;	
	/// ���������� �������� ������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const;
	/// ���������� ������ � ��������� ����������
	std::wstring GetCoordString() const;	
	/// ���������� ������ � ��������� �������
	std::wstring GetDegreeString() const;
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// ���������� ������ � ��������� ���������� ������������ �����������
	std::wstring GetAmountString() const;
	/// ���������� �������� ����������� ��������
	std::wstring GetRestrictionString() const;		
};
typedef boost::shared_ptr<VK60Overstep> VK60OverstepPtr;


/// ����������� �� ������ ��
class VOtvodWidthOverstep : public VBasicOverstep
{
private:
	/// ������� ���������� �� �������
	bool on_switch;
	/// ������� ���������� � ���� �������������� �������
	bool on_chamfered_joint;
public:
	/// �����������
	VOtvodWidthOverstep(const VExcess& in_excess);	
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
	/// ���������� �������� ������
	int GetPenalty() const;
};
typedef boost::shared_ptr<VOtvodWidthOverstep> VOtvodWidthOverstepPtr;

/// ���������� �� ������ ����������
class VOtvodLevelOverstep : public VBasicOverstep
{
private:
public:
	/// �����������
	VOtvodLevelOverstep(const VExcess& in_excess);	
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
	/// ���������� �������� ������
	int GetPenalty() const;
};

/// ����������� �� ������
class VWearOverstep : public VBasicOverstep
{
private:
	///�������
	side_t side;
	/// ������� ���������� �� �������
	bool on_switch;
	/// ������� ���������� � ���� �������������� �������
	bool on_chamfered_joint;
	/// ������� ������� �������
	MACHINE_TYPE test_run;
public:
	/// �����������
	VWearOverstep(const VExcess& in_excess, side_t in_side, DEGREE in_degree);	
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXTRA;};
};
typedef boost::shared_ptr<VWearOverstep> VWearOverstepPtr;


/// ����������� ����������� �� ������
class VWearExpress : public VBasicOverstep
{
private:
	///�������
	side_t side;
public:
	/// �����������
	VWearExpress(const VExcess& in_excess, side_t in_side);
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXPRESS;};			
	/// ���������� �������� ����������� ��������
	std::wstring GetRestrictionString() const {return L"";};		
};
typedef boost::shared_ptr<VWearExpress> VWearExpressPtr;


/// ����������� �� ������
class VLevelOverstep : public VBasicOverstep
{
private:
	/// ������� �����
	bool bridge;
	/// ������� ������� �������
	MACHINE_TYPE test_run;
public:
	/// �����������
	VLevelOverstep(const VExcess& in_excess, DEGREE in_degree);	
	/// ���������� ����� ������������ �����������
	size_t GetAmount() const;
	/// ���������� ������ � ��������� ���������� ������������ �����������
	std::wstring GetAmountString() const;
	/// �������� ������ �����������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VLevelOverstep> VLevelOverstepPtr;

/// ���������� �� ������ ������
class VShortDistortionOverstep : public VBasicOverstep
{
public:
	/// �����������
	VShortDistortionOverstep(const VExcess& in_excess);
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
	/// ���������� �������� ������
	int GetPenalty() const;
};
typedef boost::shared_ptr<VShortDistortionOverstep> VShortDistortionOverstepPtr;

/// ����������� �� ��������
class VPitOverstep : public VBasicOverstep
{
private:
	/// ������� �����
	bool bridge;
	/// ������� ������ (���)
	bool prj;
	/// ������� �������
	MACHINE_TYPE test_run;
public:
	/// �����������
	VPitOverstep(const VWave& in_wave, double in_value, double in_length, side_t in_side, DEGREE in_degree);
	/// �������� ������ �����������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};

/// ������������� ��������
class VPitGroupOverstep : public VBasicOverstep
{
private:
	/// ����������� �������� � ������
	std::deque<VBasicOverstepPtr> group;
public:
	/// �����������
	VPitGroupOverstep(const std::deque<VBasicOverstepPtr>& in_group, side_t in_side);	
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// �������� ������ �����������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VPitGroupOverstep> VPitGroupOverstepPtr;

/// ������������� ��������
class VRichGroupOverstep : public VBasicOverstep
{
private:
	/// ����������� �������� � ������
	std::deque<VBasicOverstepPtr> group;
	/// �������
	side_t side;
public:
	/// �����������
	VRichGroupOverstep(const std::deque<VBasicOverstepPtr>& in_group, side_t in_side);
	/// ���������� ������ � ��������� ����
	std::wstring GetTypeString() const;
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// �������� ������ �����������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VRichGroupOverstep> VRichGroupOverstepPtr;

/// ����������� �� ��������
class VDistortionOverstep : public VBasicOverstep
{
private:
	/// ������� �����
	bool bridge;
	/// ������� ������ (���)
	bool prj;
	/// ������� �������
	MACHINE_TYPE test_run;
public:
	/// �����������
	VDistortionOverstep(const VWave& in_wave, double in_value, double in_length, DEGREE in_degree);		
	/// �������� ������ �����������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VDistortionOverstep> VDistortionOverstepPtr;

/// ������������� ��������
class VDistortionGroupOverstep : public VBasicOverstep
{
private:
	/// ����������� �������� � ������
	std::deque<VBasicOverstepPtr> group;
public:
	/// �����������
	VDistortionGroupOverstep(const std::deque<VBasicOverstepPtr>& in_group);	
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// �������� ������ �����������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VDistortionGroupOverstep> VDistortionGroupOverstepPtr;


/// ����������� �� ��������
class VRichOverstep : public VBasicOverstep
{
private:	
	/// ������� �����
	bool bridge;
	/// ������� ������
	bool curve;
	/// ������� ���
	bool prj;
	/// ������� �������
	bool on_switch;
	/// ������� ���������� � ���� �������������� �������
	bool on_chamfered_joint;
	/// ������� ����������� ����
	bool thread_side;
	/// ������� ������� ����� �������
	MACHINE_TYPE test_run;
public:
	/// �����������
	VRichOverstep(const VWave& in_wave, double in_value, double in_length, side_t in_side, DEGREE in_degree);	
	/// ���������� ������ � ��������� ����
	std::wstring GetTypeString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// �������� ������ �����������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VRichOverstep> VRichOverstepPtr;


/// ��������� �������� - ��������
class VRichAndPitCombination : public VBasicOverstep
{
private:
	/// ��������
	VBasicOverstepPtr rich_overstep;
	/// ��������
	VBasicOverstepPtr pit_overstep;
	/// ������� ���
	bool prj;
public:
	/// �����������
	VRichAndPitCombination(VBasicOverstepPtr in_overstep1, VBasicOverstepPtr in_overstep2);	
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// �������� ������ �����������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VRichAndPitCombination> VRichAndPitCombinationPtr;

/// ��������� �������� - �������
class VRichAndDistortionCombination : public VBasicOverstep
{
private:
	/// ��������
	VBasicOverstepPtr rich_overstep;
	/// �������
	VBasicOverstepPtr distortion_overstep;
	/// ������� ���
	bool prj;
public:
	/// �����������
	VRichAndDistortionCombination(VBasicOverstepPtr in_overstep1, VBasicOverstepPtr in_overstep2);	
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// �������� ������ �����������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};
typedef boost::shared_ptr<VRichAndDistortionCombination> VRichAndDistortionCombinationPtr;

/// ������ ��� ������ ������
class VCurveCalc
{
private:
	struct DATA
	{
		VPoint curv;
		VPoint level;
		DATA(const VPoint& in_curv, const VPoint& in_level) : curv(in_curv), level(in_level) {};
		double coord() const {return curv.GetCoord();};
		double anp(int in_speed) const {return calc_anp(in_speed, curv.GetValue(), level.GetValue());};
		bool corrupt() const {return curv.GetCorrupt() || level.GetCorrupt();};
		
	};
	/// ������
	std::deque<DATA> data;
	/// ���� ��������/������ �� ������
	double curve_sign;
public:

	/// ��������� ������
	void Put(const VPoint& in_curv, const VPoint& in_level);
	/// ���������� ������� �������� ��� ��� �������� ��������
	double GetAverageAnp(int in_speed) const;
	/// ���������� ������������ �������� ��� ��� �������� ��������
	VPoint GetMaxAnp(int in_speed) const;
	/// ���������� ������������ �������� ��� ��� �������� ��������
	VPoint GetMaxPsi(int in_speed) const;
	/// ���������� ������ ������� �����������
	double GetStart() const;
	/// ���������� ����� �������
	double GetEnd() const;
	/// ������� ������ ������
	void Clear();
	/// ���������� ����� ������ ������
	size_t GetCount() const;
	/// ���������� ��� �� ���� �������
	double GetPsi(int in_speed) const;
};

/// ����������� �� �������� ��� �� ��������
class VAnpAverageOverstep : public VBasicOverstep
{
private:
	double anp_high_bt;
	double anp_high;
	double anp_pass;
	double anp_truck;
public:
	/// �����������
	VAnpAverageOverstep(const VCurveCalc& in_calc);
	/// ���������� ������ � ��������� �������
	std::wstring GetDegreeString() const;
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// ���������� ������ � ��������� ������
	std::wstring GetPenaltyString() const;
	/// ���������� �������� ������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};


/// ����������� �� ��� � �����
class VAnpHorizontalOverstep : public VBasicOverstep
{
private:
	double anp_high_bt;
	double anp_high;
	double anp_pass;
	double anp_truck;
public:
	/// �����������
	VAnpHorizontalOverstep(const VCurveCalc& in_calc);
	/// ���������� ������ � ��������� �������
	std::wstring GetDegreeString() const;
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// ���������� ������ � ��������� ������
	std::wstring GetPenaltyString() const;
	/// ���������� �������� ������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};

/// ����������� ��� � �����
class VPsiOverstep : public VBasicOverstep
{
private:
	double psi_high_bt;
	double psi_high;
	double psi_pass;
	double psi_truck;
public:
	/// �����������
	VPsiOverstep(const VCurveCalc& in_calc);
	/// ���������� ������ � ��������� �������
	std::wstring GetDegreeString() const;
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// ���������� ������ � ��������� ������
	std::wstring GetPenaltyString() const;
	/// ���������� �������� ������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};

/// ����������� ��� �� ����������
class VPsiTransitionOverstep : public VBasicOverstep
{
private:
	double psi_high_bt;
	double psi_high;
	double psi_pass;
	double psi_truck;
public:
	/// �����������
	VPsiTransitionOverstep(const VCurveCalc& in_calc);
	/// ���������� ������ � ��������� �������
	std::wstring GetDegreeString() const;
	/// ���������� ������ � ��������� ��������
	std::wstring GetValueString() const;
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// ���������� ������ � ��������� ������
	std::wstring GetPenaltyString() const;
	/// ���������� �������� ������
	int GetPenalty() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_GEOMETRY;};
};

/// ����������� ����������� �� ������
class VZazorExpress : public VBasicOverstep
{
private:
	///�������
	side_t side;
public:
	/// �����������
	VZazorExpress(const VPoint& in_point, side_t in_side);
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXPRESS;};			
	/// ���������� ������ � ��������� �����
	virtual std::wstring GetLengthString() const {return L"";};
	/// ���������� �������� ����������� ��������
	std::wstring GetRestrictionString() const {return L"";};		
};
typedef boost::shared_ptr<VZazorExpress> VZazorExpressPtr;

/// ���������������� �����
class VZazorOverstep : public VBasicOverstep
{
private:
	/// �������
	side_t side;
	/// ������� ��������� � �������
	double diameter;
	/// �����������
	double temp;
public:
	/// �����������
	VZazorOverstep(const VPoint& in_point, double in_temp, side_t in_side);	
	/// ���������� ������ � ��������� �����
	std::wstring GetLengthString() const;
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXTRA;};
};


/// ������� ������
class VZeroZazorOverstep : public VBasicOverstep
{
private:
	/// �������
	side_t side;	
public:
	/// �����������
	VZeroZazorOverstep(double in_start, double in_end, double in_value, side_t in_side);		
	/// ���������� ������
	const OVERSTEP_GROUP GetGroup() const {return GROUP_EXTRA;};
};

/// ����� ������ ���������� ��������� ��������� ������
class VFindExcess : public VDataOut<VExcess>
{
private:	
	/// ������ �� �����
	std::deque<VPoint> input;
	/// ������ �� ������
	std::deque<VExcess> output;
public:
	/// �����������
	VFindExcess();
	/// �����
	void Reset();	
	/// �������� �� ���� ��������
	void Input(const VPoint& in_point, const VOverstepCondition& in_condition);
	/// ���������� ������� ������� ������
	virtual bool Ready() const;
	/// ���������� ������� ������� ������
	virtual bool Empty() const;
	/// ���������� ������������ �����������
	virtual const VExcess& Front() const;	
	/// ������� ����������� �� �������
	virtual void Pop();
	/// ���������� ������� ���������� � �������� �����
	void Flush();
};

/// ����� ��� �����������
class VFindWave : public VDataProcessor<VExtremum, VWave>
{
private:
	/// ������� ����� �� �����
	std::deque<VExtremum> input;
	/// ������� ��� ����������� �� ������
	std::queue<VWave> output;
	/// ���������� �� ��� X
	double resolution_x;
	/// ���������� �� ��� Y
	double resolution_y;
public:
	/// �����������
	VFindWave();
	/// �����������
	VFindWave(double in_resolution_x, double in_resolution_y);
	/// ����������
	~VFindWave();
	/// ��������� �����������
	virtual void Put(const VExtremum& in_point);
	/// ���������� ������� ������� ������
	virtual bool Ready() const;
	/// ���������� ������� ������� ������
	virtual bool Empty() const;
	/// ���������� ������������ �����������
	virtual const VWave& Front() const;	
	/// ������� ����������� �� �������
	virtual void Pop();	
};

/// ���������� ��������
class VWaveFilter : public VDataProcessor<VWave,VWave>
{
private:
	/// ������ �� ������
	std::deque<VWave> output;
public:
	/// �����������
	VWaveFilter();
	/// ����������
	~VWaveFilter();
	/// �������� ������ �� ����
	virtual void Put(const VWave& in_wave);
	/// ���������� ������� ������� ������
	virtual bool Ready() const;
	/// ���������� ������� ������� ������
	virtual bool Empty() const;
	/// ���������� ������������ �����������
	virtual const VWave& Front() const;	
	/// ������� ������ �� �������
	virtual void Pop();
};

/// ������������ ��������
class VWaveUnion : public VDataProcessor<VWave,VWave>
{
private:
	/// ������ �� �����
	std::deque<VWave> input;
	/// ������ �� ������
	std::deque<VWave> output;
	/// ��������� �����������
	double union_distance;
public:
	/// �����������
	VWaveUnion(double in_union_distance);
	/// ����������
	~VWaveUnion();
	/// �������� ������ �� ����
	virtual void Put(const VWave& in_wave);
	/// ���������� ������� ������� ������
	virtual bool Ready() const;
	/// ���������� ������� ������� ������
	virtual bool Empty() const;
	/// ���������� ������������ �����������
	virtual const VWave& Front() const;	
	/// ������� ����������� �� �������
	virtual void Pop();
	/// ���� ������
	virtual void Flush();
};

/// ����� �������� �� ������
class VWaveSelector : public VDataProcessor<VBasicOverstepPtr,VBasicOverstepPtr>
{
private:
	/// ������ �� �����
	std::deque<VBasicOverstepPtr> input;
	/// ������ �� ������
	std::deque<VBasicOverstepPtr> output;
public:
	/// �����������
	VWaveSelector();
	/// ����������
	~VWaveSelector();
	/// �������� �� ���� ��������
	virtual void Put(const VBasicOverstepPtr& in_wave);
	/// ���������� ������� ������� ������
	virtual bool Ready() const;
	/// ���������� ������� ������� ������
	virtual bool Empty() const;
	/// ���������� ������������ �����������
	virtual const VBasicOverstepPtr& Front() const;	
	/// ������� ����������� �� �������
	virtual void Pop();	
	/// ���� ������
	virtual void Flush();
};


/// ������ ��������
class VCheckPit : public VDataProcessor<VWave, VBasicOverstepPtr>
{
private:	
	/// ������ ����������� �� ������
	std::deque<VBasicOverstepPtr> output;
	/// �������
	side_t side;
public:
	/// �����������
	VCheckPit(side_t in_side);
	/// ����������
	~VCheckPit();
	/// �������� �� ���� ��������
	void Put(const VWave& in_wave);
	/// ���������� ������� ������� ������
	virtual bool Ready() const;
	/// ���������� ������� ������� ������
	virtual bool Empty() const;
	/// ���������� ������������ �����������
	virtual const VBasicOverstepPtr& Front() const;	
	/// ������� ����������� �� �������
	virtual void Pop();	
};

/// ������� ������ ��������
class VPitGroupCondition
{
public:
	bool operator()(const VBasicOverstepPtr& in_overstep) const
	{
		if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
		{
			return in_overstep->GetValue()>PIT_GROUP_VALUE_515 && !in_overstep->IsExcluded();
		}
		else
		{
			return !in_overstep->IsExcluded() && 
				(in_overstep->GetRestriction().GetPassSpeed()>60 && in_overstep->GetValue()>PIT_GROUP_VALUE && in_overstep->GetLength()<=PIT_GROUP_LENGTH ||
				 in_overstep->GetRestriction().GetMaxSpeed()>140 && in_overstep->GetDegree()==DEGREE3);
		}
	};
};

/// ������� ������ ��������
class VRichGroupCondition
{
public:
	bool operator()(const VBasicOverstepPtr& in_overstep) const
	{
		if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
		{
			return in_overstep->GetDegree()==DEGREE3 && !in_overstep->IsExcluded();
		}
		else
		{
			return !in_overstep->IsExcluded() && 
				(in_overstep->GetRestriction().GetPassSpeed()>60 && in_overstep->GetValue()>RICH_GROUP_VALUE && in_overstep->GetLength()<=RICH_GROUP_LENGTH ||
				 in_overstep->GetRestriction().GetMaxSpeed()>140 && in_overstep->GetDegree()==DEGREE3);
		}
	};
};

/// ������� ������ ���������
class VDistortionGroupCondition
{
public:
	bool operator()(const VBasicOverstepPtr& in_overstep) const
	{
		if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
		{
			return false;
		}
		else
		{
			return !in_overstep->IsExcluded() && 
				(in_overstep->GetRestriction().GetPassSpeed()>60 && in_overstep->GetValue()>DISTORTION_GROUP_VALUE && in_overstep->GetLength()<=DISTORTION_GROUP_LENGTH ||
				 in_overstep->GetRestriction().GetMaxSpeed()>140 && in_overstep->GetDegree()==DEGREE3);
		}
	};
};

/// ������� ��������� �������� � ��������
class VRichAndPitCondition
{
private:
	/// ����� �����������
	VBasicOverstepPtr overstep;			
public:
	/// �����������
	VRichAndPitCondition(const VBasicOverstepPtr& in_overstep) : overstep(in_overstep){};
	/// ��������()
	bool operator()(const VBasicOverstepPtr& in_overstep) const
	{	
		if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
		{
			return in_overstep->GetDegree()>=DEGREE3 &&
				overstep->GetDegree()>=DEGREE3 && 
				overstep->Intersection(*in_overstep) && 
				(is_rich(in_overstep) && is_pit(overstep) || is_rich(overstep) && is_pit(in_overstep));
		}
		else
		{				
			return overstep->Distance(*in_overstep)<COMBINATION_DISTANCE && 
				(is_rich(in_overstep) && is_pit(overstep) || is_rich(overstep) && is_pit(in_overstep));
		}
	};
};

/// ������� ��������� �������� � ��������
class VRichAndDistortionCondition
{
private:
	/// ����� �����������
	VBasicOverstepPtr overstep;			
public:
	/// �����������
	VRichAndDistortionCondition(const VBasicOverstepPtr& in_overstep) : overstep(in_overstep){};
	/// ��������()
	bool operator()(const VBasicOverstepPtr& in_overstep) const
	{				
		if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
		{	
			return in_overstep->GetDegree()>=DEGREE3 &&
				overstep->GetDegree()>=DEGREE3 && 
				overstep->Intersection(*in_overstep) && 
				(is_rich(in_overstep) && is_distortion(overstep) || is_rich(overstep) && is_distortion(in_overstep));
		}
		else
		{		
			return overstep->Distance(*in_overstep)<COMBINATION_DISTANCE && 
				(is_rich(in_overstep) && is_distortion(overstep) || is_rich(overstep) && is_distortion(in_overstep));
		}
	};
};

typedef VDataProcessor<VBasicOverstepPtr, VBasicOverstepPtr> VGroupDetector;
typedef boost::shared_ptr<VGroupDetector> VGroupDetectorPtr;

/// ������ ����� ��������
class VPitGroupDetector : public VGroupDetector
{
private:
	/// ������� ������ �� �����
	std::deque<VBasicOverstepPtr> input;
	/// ������� ������ �� ������
	std::deque<VBasicOverstepPtr> output;
	/// �������
	side_t side;
	/// ������� ���������
	double position;
public:
	/// �����������
	VPitGroupDetector(side_t in_side);
	/// ����������
	~VPitGroupDetector();
	/// �������� ��������� �����������
	void Put(const VBasicOverstepPtr& in_overstep);
	/// �������� �������� ������� �������
	void Put(const double& in_position);
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();
	/// ����� ������
	void Flush();
};


/// ������ ����� ��������
class VRichGroupDetector : public VGroupDetector
{
private:
	/// ������� ������ �� �����
	std::deque<VBasicOverstepPtr> input;
	/// ������� ������ �� ������
	std::deque<VBasicOverstepPtr> output;
	/// �������
	side_t side;
	/// ������� ���������
	double position;
public:
	/// �����������
	VRichGroupDetector(side_t in_side);
	/// ����������
	~VRichGroupDetector();
	/// �������� ��������� �����������
	void Put(const VBasicOverstepPtr& in_overstep);
	/// �������� �������� ������� �������
	void Put(const double& in_position);
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();
	/// ����� ������
	void Flush();
};

/// ������ ��������
class VCheckDistortion : public VDataProcessor<VWave, VBasicOverstepPtr>
{
private:	
	/// ������ ����������� �� ������
	std::deque<VBasicOverstepPtr> output;
public:
	/// �����������
	VCheckDistortion();
	/// ����������
	~VCheckDistortion();
	/// �������� �� ���� ��������
	virtual void Put(const VWave& in_wave);
	/// ���������� ������� ������� ������
	virtual bool Ready() const;
	/// ���������� ������� ������� ������
	virtual bool Empty() const;
	/// ���������� ������������ �����������
	virtual const VBasicOverstepPtr& Front() const;	
	/// ������� ����������� �� �������
	virtual void Pop();	
};

/// ������ ����� ���������
class VDistortionGroupDetector : public VGroupDetector
{
private:
	/// ������� ������ �� �����
	std::deque<VBasicOverstepPtr> input;
	/// ������� ������ �� ������
	std::deque<VBasicOverstepPtr> output;
	/// �������
	side_t side;
	/// ������� ���������
	double position;
public:
	/// �����������
	VDistortionGroupDetector();
	/// ����������
	~VDistortionGroupDetector();
	/// �������� ��������� �����������
	void Put(const VBasicOverstepPtr& in_overstep);
	/// �������� �������� ������� �������
	void Put(const double& in_position);
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();
	/// ����� ������
	void Flush();
};


/// ������ ��������
class VCheckRich : public VDataProcessor<VWave, VBasicOverstepPtr>
{
private:	
	/// ������ ����������� �� ������
	std::deque<VBasicOverstepPtr> output;
	/// �������
	side_t side;
	/// ������� �������������
	//VRichCorrectionTable RichCorrectionTable;
	VRichCorrectorPtr RichCorrector;
public:
	/// �����������
	VCheckRich(side_t in_side);
	/// ����������
	~VCheckRich();
	/// �������� �� ���� ��������
	void Put(const VWave& in_wave);
	/// ���������� ������� ������� ������
	virtual bool Ready() const;
	/// ���������� ������� ������� ������
	virtual bool Empty() const;
	/// ���������� ������������ �����������
	virtual const VBasicOverstepPtr& Front() const;	
	/// ������� ����������� �� �������
	virtual void Pop();	
};

/// ����� ��������� �������� � ��������
class VRichAndDistortionDetector : public VGroupDetector
{
private:
	/// ������� ������ �� �����
	std::deque<VBasicOverstepPtr> input;
	/// ������� ������ �� ������
	std::queue<VBasicOverstepPtr> output;
public:
	/// �����������
	VRichAndDistortionDetector();
	/// ����������
	~VRichAndDistortionDetector();
	/// �������� ��������� �����������
	virtual void Put(const VBasicOverstepPtr& in_overstep);
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();
};


/// ����� ��������� �������� � ��������
class VRichAndPitDetector : public VGroupDetector
{
private:
	/// ������� ������ �� �����
	std::deque<VBasicOverstepPtr> input;
	/// ������� ������ �� ������
	std::queue<VBasicOverstepPtr> output;
public:
	/// �����������
	VRichAndPitDetector();
	/// ����������
	~VRichAndPitDetector();
	/// �������� ��������� �����������
	virtual void Put(const VBasicOverstepPtr& in_overstep);
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();
};

/// ����� �10
class VW10OverstepDetector : public VGroupDetector
{
private:
	/// ���������� ���������� �� ���������
	double w10_amount;
	/// ������� ��������
	VAprinfoKmRecord aprinfo_km;
	/// ������� ������ �� ������
	std::deque<VBasicOverstepPtr> output;
public:
	/// �����������
	VW10OverstepDetector();
	/// ����������
	~VW10OverstepDetector();
	/// �������� ��������� �����������
	virtual void Put(const VBasicOverstepPtr& in_overstep);
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();
	/// ���� ������
	virtual void Flush();
};

/// ����� �100
class VK100OverstepDetector : public VGroupDetector
{
private:
	/// ���������� ���������� �� ���������
	double k100_amount;
	/// ������� ��������
	VAprinfoKmRecord aprinfo_km;
	/// ������� ������ �� ������
	std::deque<VBasicOverstepPtr> output;
public:
	/// �����������
	VK100OverstepDetector();
	/// ����������
	~VK100OverstepDetector();
	/// �������� ��������� �����������
	virtual void Put(const VBasicOverstepPtr& in_overstep);
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();
	/// ���� ������
	virtual void Flush();
};

/// ����� �60
class VK60OverstepDetector : public VGroupDetector
{
private:
	/// ���������� ���������� �� ���������
	double k60_amount;
	/// ������� ��������
	VAprinfoKmRecord aprinfo_km;
	/// ������� ������ �� ������
	std::deque<VBasicOverstepPtr> output;
public:
	/// �����������
	VK60OverstepDetector();
	/// ����������
	~VK60OverstepDetector();
	/// �������� ��������� �����������
	virtual void Put(const VBasicOverstepPtr& in_overstep);
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();
	/// ���� ������
	virtual void Flush();
};


/// ������� ��������� � ��������
class VRangeCondition : public VOverstepCondition
{
private:
	/// �������� ��������
	pro::interval<double> value_range;
	/// �������� ����
	pro::interval<double> length_range;
public:
	/// �����������
	VRangeCondition(const pro::interval<double>& in_value_range, const pro::interval<double>& in_length_range=pro::interval<double>(0, DBL_MAX));
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;	
};

/// ������� ����� ������
class VCurveCondition : public VOverstepCondition
{
private:
	/// �������� ��������
	pro::interval<double> range;
public:
	/// ����������
	VCurveCondition(side_t in_side, double in_min_curv=1) : range(in_side==SD_LEFT ?
		pro::interval<double>(0-DBL_MAX, -in_min_curv, false, false) : pro::interval<double>(in_min_curv, DBL_MAX, false, false)){};
	/// ��������� �������
	bool TestValue(double in_value, double in_length=0) const
	{
		return range.contains(in_value);
	};
};

/// ������� ��������
class VWidenCondition : public VOverstepCondition
{
private:
	/// �������
	DEGREE degree;
	/// ��������
	VSpeeds speeds;
	/// ����� ��
	double norma;	
	/// ������
	double radius;
	/// �����
	double wear;
	
public:
	/// �����������
	VWidenCondition(DEGREE in_degree, VSpeeds in_speeds, double in_norma, double in_radius, double in_wear);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;	
};

/// ������� �������
class VNarrowCondition : public VOverstepCondition
{
private:
	/// �������
	DEGREE degree;
	/// ��������
	VSpeeds speeds;
	/// ����� ��
	double norma;
	/// ������� ������� ���
	bool zbsh;	
public:
	/// �����������
	VNarrowCondition(DEGREE in_degree, VSpeeds in_speeds, double in_norma, bool in_zbsh);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;	
};

/// ������� ������ �� (������ ����)
class VOtvodWidthCondition : public VOverstepCondition
{
private:
	/// ��������
	VSpeeds speeds;	
public:
	/// �����������
	VOtvodWidthCondition(VSpeeds in_speeds);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};


/// ������� ������ ������ ��� ������������ � ����������
class VOtvodLevelPassCondition : public VOverstepCondition
{
private:
	/// ��������
	VSpeeds speeds;	
public:
	/// �����������
	VOtvodLevelPassCondition(VSpeeds in_speeds);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};

/// ������� ������ ������ ��� ��������
class VOtvodLevelTruckCondition : public VOverstepCondition
{
private:
	/// ��������
	VSpeeds speeds;	
public:
	/// �����������
	VOtvodLevelTruckCondition(VSpeeds in_speeds);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};

/// ������� ������ ������
class VOtvodLevelCondition : public VOverstepCondition
{
private:
	/// ��������
	VSpeeds speeds;	
public:
	/// �����������
	VOtvodLevelCondition(VSpeeds in_speeds);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};

/// ������� ������
class VWearCondition : public VOverstepCondition
{
private:	
	/// �������
	DEGREE degree;	
public:
	/// �����������
	VWearCondition(DEGREE in_degree);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};

/// ������� ������
class VZazorCondition : public VOverstepCondition
{
private:
	/// ��������
	VSpeeds speeds;
	/// ������� ��������� � �������
	double diameter;
public:
	/// �����������
	VZazorCondition(const VSpeeds& in_speeds, double in_diameter=0);
	/// ��������� �������
	bool TestValue(double in_value, double in_length=0) const;
};

/// ������� ��������
class VPitCondition : public VOverstepCondition
{
private:
	/// �������
	DEGREE degree;
	/// ��������
	VSpeeds speeds;
	/// ������� ������� �����
	bool bridge;
public:
	/// �����������
	VPitCondition(DEGREE in_degree, VSpeeds in_speeds, bool in_bridge);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};

/// ������� �������� ��� ������ ���� � ������ �������
class VPitConditionTestRun : public VOverstepCondition
{
private:
public:
	/// �����������
	VPitConditionTestRun();
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};

/// ������� �������� ���������� �� ������
class VLevelCondition : public VOverstepCondition
{
private:
	/// �������
	DEGREE degree;
	/// ��������
	VSpeeds speeds;	
public:
	/// �����������
	VLevelCondition(DEGREE in_degree, VSpeeds in_speeds);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};

/// ������� �������� ���������� �� ������ � ������ �������
class VLevelConditionTestRun : public VOverstepCondition
{
private:
	/// ��������
	VSpeeds speeds;
	/// ������ ������������ �����
	MACHINE_TYPE type;
public:
	/// �����������
	VLevelConditionTestRun(VSpeeds in_speeds, MACHINE_TYPE in_type);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};


/// ������� ������ �� ������
class VShortDistortionCondition : public VOverstepCondition
{
private:	
	/// ��������
	VSpeeds speeds;
public:
	/// �����������
	VShortDistortionCondition(VSpeeds in_speeds);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};

/// ������� ��������
class VDistortionCondition : public VOverstepCondition
{
private:
	/// �������
	DEGREE degree;
	/// ��������
	VSpeeds speeds;
	/// ������� �����
	bool bridge;
	
public:
	/// �����������
	VDistortionCondition(DEGREE in_degree, VSpeeds in_speeds, bool in_bridge);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};

/// ������� �������� ��� ������� ����
class VDistortionConditionTestRun : public VOverstepCondition
{
private:
	
	/// ��������
	VSpeeds speeds;
	/// ������ ������������ �����
	MACHINE_TYPE type;
	
public:
	/// �����������
	VDistortionConditionTestRun(VSpeeds in_speeds, MACHINE_TYPE in_type);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};


/// ������� ��������
class VRichCondition : public VOverstepCondition
{
private:
	/// �������
	DEGREE degree;
	/// ��������
	VSpeeds speeds;	
	/// ������� ������� �����
	bool bridge;	
	/// ������� ������� ����������� ����
	bool thread_side;
	/// ������� ������
	bool curve;
public:
	/// �����������
	VRichCondition(DEGREE in_degree, VSpeeds in_speeds, bool in_bridge, bool in_thread_side, bool in_curve);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};


/// ������� �������� � ������ ������� ����� �������
class VRichConditionTestRun : public VOverstepCondition
{
private:
	/// ��������
	VSpeeds speeds;	
	/// ������ ������������ �����
	MACHINE_TYPE type;

public:
	/// �����������
	VRichConditionTestRun(VSpeeds in_speeds, MACHINE_TYPE in_type);
	/// ��������� �������
	bool TestValue(double in_value, double in_length) const;
};

/// ������� ������������� ���������
class VAnpCondition : public VOverstepCondition
{
private:
	/// �����
	double anp;
public:
	/// �����������
	VAnpCondition(double in_anp);
	/// ��������� �������
	bool TestValue(double in_value, double in_length=0) const;
};

/// ������� �������� ��������� ������������� ���������
class VPsiCondition : public VOverstepCondition
{
private:
	/// ������������ �� ��������
	bool restrict;
public:
	/// �����������
	VPsiCondition(bool in_restrict) : restrict(in_restrict) {};
	/// ��������� �������
	bool TestValue(double in_value, double in_length=0) const;
};

/// ������� �������� ��������� ������������� ���������
class VPsiBTCondition : public VOverstepCondition
{
private:
	/// ������������ �� ��������
	bool restrict;
public:
	/// �����������
	VPsiBTCondition(bool in_restrict) : restrict(in_restrict) {};
	/// ��������� �������
	bool TestValue(double in_value, double in_length=0) const;
};

/// ������� �������� ������������� ��������� (����.)
class VAnpAverageHighCondition : public VOverstepCondition
{
private:
	/// ��� ����������� ��������
	bool restrict;
public:
	/// �����������
	VAnpAverageHighCondition(bool in_restrict) : restrict(in_restrict) {};
	/// ��������� �������
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 0.9 : 0.7);
	};
};

/// ������� �������� ������������� ��������� (����.)
class VAnpAverageHighBTCondition : public VOverstepCondition
{
private:
	/// ��� ����������� ��������
	bool restrict;
public:
	/// �����������
	VAnpAverageHighBTCondition(bool in_restrict) : restrict(in_restrict) {};
	/// ��������� �������
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 1.4 : 0.7);
	};
};

/// ������� �������� ������������� ��������� (����.)
class VAnpAveragePassCondition : public VOverstepCondition
{
private:
	/// ��� ����������� ��������
	bool restrict;
public:
	/// �����������
	VAnpAveragePassCondition(bool in_restrict) : restrict(in_restrict) {};
	/// ��������� �������
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 0.75 : 0.7);
	};
};

/// ������� �������� ������������� ��������� (����.)
class VAnpAverageTruckCondition : public VOverstepCondition
{
private:
	/// ��� ����������� ��������
	bool restrict;
public:
	/// �����������
	VAnpAverageTruckCondition(bool in_restrict) : restrict(in_restrict) {};
	/// ��������� �������
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 0.7 : 0.7);
	};
};


/// ������� ������������� ��������� (����.)
class VAnpHighCondition : public VOverstepCondition
{
private:
	/// ��� ����������� ��������
	bool restrict;
public:
	/// �����������
	VAnpHighCondition(bool in_restrict) : restrict(in_restrict) {};
	/// ��������� �������
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 1.0 : 0.7);
	};
};

/// ������� ������������� ��������� (����.)
class VAnpHighBTCondition : public VOverstepCondition
{
private:
	/// ��� ����������� ��������
	bool restrict;
public:
	/// �����������
	VAnpHighBTCondition(bool in_restrict) : restrict(in_restrict) {};
	/// ��������� �������
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 1.5 : 0.7);
	};
};

/// ������� ������������� ��������� (����.)
class VAnpPassCondition : public VOverstepCondition
{
private:
	/// ��� ����������� ��������
	bool restrict;
public:
	/// �����������
	VAnpPassCondition(bool in_restrict) : restrict(in_restrict) {};
	/// ��������� �������
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 0.8 : 0.7);
	};
};

/// ������� ������������� ��������� (����.)
class VAnpTruckCondition : public VOverstepCondition
{
private:
	/// ��� ����������� ��������
	bool restrict;
public:
	/// �����������
	VAnpTruckCondition(bool in_restrict) : restrict(in_restrict) {};
	/// ��������� �������
	bool TestValue(double in_value, double in_length=0) const 
	{
		return round(fabs(in_value), ANP_PRECISION) > (restrict ? 0.75 : 0.7);
	};
};

typedef VDataProcessor<VRtParameter, VBasicOverstepPtr> VOverstepDetector;
typedef boost::shared_ptr<VOverstepDetector> VOverstepDetectorPtr;

/// ����� ������ ������
class VFallbackOverstepDetector : public VOverstepDetector
{
private:
	/// ������
	OVERSTEP_GROUP group;
	/// �������� ����� ����������
	change_calculator<long> km_change;
	/// ���������
	std::wstring type;
	/// ����������� �� ������
	std::deque<VBasicOverstepPtr> output;
	
public:
	///�����������
	VFallbackOverstepDetector(OVERSTEP_GROUP in_group, const std::wstring& in_type) : group(in_group), type(in_type) {};
	/// ����������
	~VFallbackOverstepDetector() {};
	/// �������� ������ �� ����
	void Put(const VRtParameter& in_data)
	{
		if (km_change.put(SITUATION.GetRealCoord(in_data.absCoord).km))
		{
			output.push_back(VBasicOverstepPtr(new VFallbackOverstep(in_data.absCoord, group, type))); 
		}
	};
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const
	{
		return output.size()>0;
	};	
	/// ���������� true, ���� ����������� ���
	bool Empty() const
	{
		return output.empty();
	};
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const
	{
		return output.front(); 
	};
	/// ����������� ������ �������
	void Pop()
	{
		output.pop_front();
	};
};

/// ����� ������ �����
template<typename T, OVERSTEP_GROUP group> class VCorruptDetector : public VOverstepDetector
{
private:
	T extractor;
	std::deque<VPoint> input;
	std::deque<VBasicOverstepPtr> output;
	std::wstring type;
	double length;
public:
	///�����������
	VCorruptDetector(const T& in_extractor, const std::wstring& in_type, double in_length) : extractor(in_extractor), type(in_type), 
		length(in_length){};
		
	/// ����������
	~VCorruptDetector() 
	{
		if (!Empty()) 
			LOG_ERROR(L"VCorruptDetector: �������������� ������...");
	};

	/// �������� ������ �� ����
	void Put(const VRtParameter& in_record)
	{
		VPoint point=extractor(in_record);
		if (point.GetCorrupt())
		{
			input.push_back(point);
		}
		else
		{
			Flush();
		}
	}

	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const
	{
		return output.size()>0;
	}

	/// ���������� true, ���� ����������� ���
	bool Empty() const
	{
		return output.empty();
	}

	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const
	{
		return output.front();
	}

	/// ����������� ������ �������
	void Pop()
	{
		output.pop_front();
	}

	/// ���� ������
	void Flush()
	{
		if (!input.empty() && fabs(input.back().GetCoord()-input.front().GetCoord()) > length)
		{
			output.push_back(VBasicOverstepPtr(new VCorruptOverstep<group>(input.front().GetCoord(), 
				input.back().GetCoord(), type)));
		}
		input.clear();	
	}
};

/// ����� ������ ����� �� ������
template<typename T, OVERSTEP_GROUP group> class VPkCorruptDetector : public VOverstepDetector
{
private:
	T extractor;
	std::deque<VPoint> input;
	std::deque<VBasicOverstepPtr> output;
	std::wstring type;
	change_calculator<long> pk_changed;
	double percent;
public:
	///�����������
	VPkCorruptDetector(const T& in_extractor, const std::wstring& in_type, double in_percent) : extractor(in_extractor), type(in_type), 
		percent(in_percent){};
		
	/// ����������
	~VPkCorruptDetector() 
	{
		if (!Empty()) 
			LOG_ERROR(L"VPkCorruptDetector: �������������� ������...");
	};

	/// �������� ������ �� ����
	void Put(const VRtParameter& in_record)
	{
		VPoint point=extractor(in_record);
		input.push_back(point);
		if (pk_changed.put(SITUATION.GetPk(in_record.absCoord).pk))
		{
			Flush();
		}
	}

	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const
	{
		return output.size()>0;
	}

	/// ���������� true, ���� ����������� ���
	bool Empty() const
	{
		return output.empty();
	}

	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const
	{
		return output.front();
	}

	/// ����������� ������ �������
	void Pop()
	{
		output.pop_front();
	}

	/// ���� ������
	void Flush()
	{
		if (!input.empty())
		{
			struct _pred
			{
				bool operator()(const VPoint& in_point) const {return in_point.GetCorrupt();};
			};
			size_t bad_count=std::count_if(input.begin(), input.end(), _pred());
			double bad_percent=100*bad_count/input.size();
			if (bad_percent>percent)
			{	
				output.push_back(VBasicOverstepPtr(new VPkCorruptOverstep<group>(input.front().GetCoord(), input.back().GetCoord(), bad_percent, type)));
			}
			input.clear();
		}
	}
};



/// ����� �������� ����������� ��������
class VControlSectionDetector : public VOverstepDetector
{
private:
	
	std::deque<VBasicOverstepPtr> output;	
	accumulator_set< double, features<tag::mean, tag::variance > > width;
	accumulator_set< double, features<tag::mean, tag::variance > > level;
	std::deque<double> coord;
			
public:
	///�����������
	VControlSectionDetector();
	/// ����������
	~VControlSectionDetector();
	/// �������� ������ �� ����
	virtual void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;		
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();	
};

/// ����� ��������-������ ��������
class VWidenExpressDetector : public VOverstepDetector
{
private:
	/// ������� �����������
	std::deque<VBasicOverstepPtr> output;
	/// ����� �����������
	VFindExcess find_excess;
	/// �����������
	VRangeCondition condition;
public:
	/// �����������
	VWidenExpressDetector();
	/// ����������
	~VWidenExpressDetector();
	/// ���� ������	
	void Put(const VRtParameter& in_record);
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;		
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();	
};

/// ����� ������ ��������
class VWidenOverstepDetector : public VOverstepDetector
{
private:
	
	/// ������� �����������
	std::deque<VBasicOverstepPtr> oversteps;
	/// ����� ������ �������
	VFindExcess find_excess1;
	/// ����� ������ �������
	VFindExcess find_excess2;		
	/// ����� ������� �������
	VFindExcess find_excess3;		
	/// ����� ��������� �������
	VFindExcess find_excess4;	
	/// ���������� ������
	VMean wear_mean;	
	/// �������� ��
	VDelay width_delay;		
			
public:
	/// �����������
	VWidenOverstepDetector();	
	/// ����������
	~VWidenOverstepDetector();
	/// ���� ������
	void Put(const VRtParameter& in_record);	
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;		
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();	
	/// ���� ������
	void Flush();
};

/// ����� ��������-������ �������
class VNarrowExpressDetector : public VOverstepDetector
{
private:
	/// ������� �����������
	std::deque<VBasicOverstepPtr> output;
	/// ����� �����������
	VFindExcess find_excess;
	/// �����������
	VRangeCondition condition;
public:
	/// �����������
	VNarrowExpressDetector();
	/// ����������
	~VNarrowExpressDetector();
	/// ���� ������	
	void Put(const VRtParameter& in_record);
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;		
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();	
};

/// ����� ������ �������
class VNarrowOverstepDetector : public VOverstepDetector
{
private:
	
	/// ������� �����������
	std::deque<VBasicOverstepPtr> oversteps;
	/// ����� ������ �������
	VFindExcess find_excess1;	
	/// ����� ������ �������
	VFindExcess find_excess2;	
	/// ����� ������� �������
	VFindExcess find_excess3;	
	/// ����� ��������� �������
	VFindExcess find_excess4;
	
public:
	/// �����������
	VNarrowOverstepDetector();
	/// ����������
	~VNarrowOverstepDetector();
	/// ���� ������	
	virtual void Put(const VRtParameter& in_record);
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;		
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();	
	/// ���� ������
	void Flush();
};


/// ����� ������ ������ ��
class VOtvodWidthOverstepDetector : public VOverstepDetector
{
private:

	// ������

	/// ������� �����������
	std::deque<VBasicOverstepPtr> oversteps;
	/// ���������� ������
	VFindOtvod otvod;
	/// ����� �����������
	VFindExcess find_excess;	

public:
	/// �����������
	VOtvodWidthOverstepDetector();
	/// ����������
	~VOtvodWidthOverstepDetector();
	/// ���� ������	
	virtual void Put(const VRtParameter& in_record);
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;		
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();	
};


/// ����� ������ ������ ����������
class VOtvodLevelOverstepDetector : public VOverstepDetector
{
private:

	// ������

	/// ������� �����������
	std::deque<VBasicOverstepPtr> oversteps;
	/// ���������� ��������
	VMean level_mean;
	/// ���������� ������
	VFindOtvod otvod;
	/// ����� �����������
	VFindExcess find_excess;	

public:
	/// �����������
	VOtvodLevelOverstepDetector();
	/// ����������
	~VOtvodLevelOverstepDetector();
	/// ���� ������	
	virtual void Put(const VRtParameter& in_record);
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;		
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();	
};


/// ����� ��������-������ ������
class VWearExpressDetector : public VOverstepDetector
{
private:
	/// �������
	side_t side;
	/// ������� �����������
	std::deque<VBasicOverstepPtr> output;
	/// ����� �����������
	VFindExcess find_excess;
	/// �����������
	VRangeCondition condition;
public:
	/// �����������
	VWearExpressDetector(side_t in_side);
	/// ����������
	~VWearExpressDetector();
	/// ���� ������	
	void Put(const VRtParameter& in_record);
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;		
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();	
};

/// ����� ������ ������
class VWearOverstepDetector : public VOverstepDetector
{
private:

	/// ������� �����������
	std::deque<VBasicOverstepPtr> oversteps;
	/// ����� �����������
	VFindExcess find_excess0;
	/// ����� �����������
	VFindExcess find_excess2;
	/// ����� �����������
	VFindExcess find_excess3;	
	/// ����� �����������
	VFindExcess find_excess4;	
	
public:

	/// �����������
	VWearOverstepDetector();
	/// ����������
	~VWearOverstepDetector();
	/// ���� ������
	virtual void Put(const VRtParameter& in_record);		
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;		
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();	
	/// ���� ������
	void Flush();
};

/// ����� ������ ��������
class VPitOverstepDetector : public VOverstepDetector
{
private:

	// ������

	/// �������
	side_t side;
	/// ������� ����������� �� ������
	std::deque<VBasicOverstepPtr> output;
	/// ����������
	VFindExtremum extremum;	
	/// ���� �����������
	VFindWave wave;	
	/// ������ ��������
	VCheckPit pit;
	/// ����� ��������
	VWaveSelector selector;
				
public:

	/// �����������
	VPitOverstepDetector(side_t in_side);
	/// ����������
	~VPitOverstepDetector();
	/// ���� ������
	void Put(const VRtParameter& in_record);	
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;		
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();	
	/// ���������� ������� ������
	void Flush();
};

/// ����� ����������� �������� ������ (���������� �� �����)
class VLevelVariate : public VDataProcessor<VPoint, VPoint>
{
private:	
	/// ���������� �������� ������ � ������� �����
	VPointToPointProcessorPtr level_variate;
	/// ��������� �������� ������������� ��������
	change_calculator<bool> bad_passport_changed;		
			
public:
	/// �����������
	VLevelVariate();
	/// ����������
	~VLevelVariate();
	/// ���� ������	
	void Put(const VPoint& in_point);
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;		
	/// ���������� ������ �������
	const VPoint& Front() const;
	/// ����������� ������ �������
	void Pop();
};


/// ����� ������ ����������� �� ������
class VLevelOverstepDetector : public VOverstepDetector
{
private:
	// ������

	/// ������� �����������
	std::deque<VBasicOverstepPtr> oversteps;
	/// ����� ������ �������
	VFindExcess find_excess1;
	/// ����� ������ �������
	VFindExcess find_excess2;
	/// ����� ������� �������
	VFindExcess find_excess3;
	/// ����� ��������� �������
	VFindExcess find_excess4;
	
public:
	/// �����������
	VLevelOverstepDetector();
	/// ����������
	~VLevelOverstepDetector();
	/// ���� ������	
	virtual void Put(const VRtParameter& in_record);
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;		
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();	
	/// ���� ������
	void Flush();
};

/// ����� ������ ����������� �� ������
class VShortDistortionOverstepDetector : public VOverstepDetector
{
private:
	/// ������� �����������
	std::deque<VBasicOverstepPtr> oversteps;	
	/// ���������� ������
	VFindOtvod otvod;
	/// ����� �����������
	VFindExcess find_excess;
	
public:
	/// �����������
	VShortDistortionOverstepDetector();
	/// ����������
	~VShortDistortionOverstepDetector();
	/// ���� ������	
	virtual void Put(const VRtParameter& in_record);
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;		
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();	
};

/// ����� ������ ���������
class VDistortionOverstepDetector : public VOverstepDetector
{
private:
	// ������

	/// ������� �� ������
	std::deque<VBasicOverstepPtr> output;
	/// ����������� �������� ������
	VLevelVariate level_variate;
	/// ����� �����������
	VFindExtremum extremum;
	/// ���� �����������
	VFindWave wave;
	/// ������ ��������
	VWaveFilter wave_filter;
	/// ����������� ��������
	VWaveUnion wave_union;
	/// ������ ���������
	VCheckDistortion distortion;
	/// ����� ���������
	VWaveSelector selector;

public:
	/// �����������
	VDistortionOverstepDetector();
	/// ����������
	~VDistortionOverstepDetector();
	/// ���� ������
	void Put(const VRtParameter& in_record);		
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;		
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();	
	/// ���������� ������� ������
	void Flush();
};

/// ����� ����������� �������� �������� (���������� �� �����)
class VRichVariate : public VDataProcessor<VPoint, VPoint>
{
private:	
	/// �������
	side_t side;
	/// ���������� �������� �������� � ������� �����
	VPointToPointProcessorPtr rich_variate;
	/// ��������� �������� ������������� ��������
	change_calculator<bool> bad_passport_changed;	
public:
	/// �����������
	VRichVariate(side_t in_side);
	/// ����������
	~VRichVariate();
	/// ���� ������	
	void Put(const VPoint& in_point);
	/// ���������� true, ���� ���� ������
	bool Ready() const;	
	/// ���������� true, ���� ������ ���
	bool Empty() const;		
	/// ���������� ������ �������
	const VPoint& Front() const;
	/// ����������� ������ �������
	void Pop();
};


/// ����� ������ ��������
class VRichOverstepDetector : public VOverstepDetector
{
private:
	// ������

	/// �������
	side_t side;
	/// ������� �����������
	std::deque<VBasicOverstepPtr> output;	
	/// ���������� �������� �������� � ������� �����	
	VRichVariate rich_variate;
	/// ����� �����������
	VFindExtremum extremum;
	/// ����������� ����������� � ����
	VFindWave wave;	
	/// ������ ��������
	VCheckRich rich;
	/// ����� ��������
	VWaveSelector selector;	

public:
	/// �����������
	VRichOverstepDetector(side_t in_side);
	/// ����������
	~VRichOverstepDetector();
	/// ���� ������
	void Put(const VRtParameter& in_record);
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;
	/// ���������� true, ���� ����������� ���
	bool Empty() const;
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();
	/// ���������� ������� ������
	void Flush();
};

/// ����� ������ ����������� �� �������� ������������� ��������� �� �������� ������
class VAnpAverageOverstepDetector : public VOverstepDetector
{
private:

	/// ������ �������� ������
	VMean level_mean;
	/// ������ ������� ��������
	VMean curv_mean;
	/// ������ ���������� ������
	VCurveCalc calc;
	/// ������� �����������
	std::deque<VBasicOverstepPtr> output;
	
public:
	/// �����������
	VAnpAverageOverstepDetector();	
	/// ����������
	~VAnpAverageOverstepDetector();
	/// ���� ������
	void Put(const VRtParameter& in_record);	
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();
	/// ���� ������
	void Flush();
};

/// ����� ������ ����������� �� ������������� ��������� � �����
class VAnpHorizontalOverstepDetector : public VOverstepDetector
{
private:

	/// ������ �������� ������
	VMean level_mean;
	/// ������ ������� ��������
	VMean curv_mean;
	/// ������ ���������� ������
	VCurveCalc calc;
	/// ������� �����������
	std::deque<VBasicOverstepPtr> output;
	
public:
	/// �����������
	VAnpHorizontalOverstepDetector();	
	/// ����������
	~VAnpHorizontalOverstepDetector();
	/// ���� ������
	void Put(const VRtParameter& in_record);	
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();
	/// ���� ������
	void Flush();
};

/// ����� ������ ����������� �� �������� ��������� ������������� ���������
class VPsiOverstepDetector : public VOverstepDetector
{
private:
	/// ������ �������� ������
	VMean level_mean;
	/// ������ ������� ��������
	VMean curv_mean;
	/// ������ ���������� ������
	VCurveCalc calc;
	/// ������� �����������
	std::deque<VBasicOverstepPtr> output;
public:
	/// �����������
	VPsiOverstepDetector();
	/// ����������
	~VPsiOverstepDetector();
	/// ���� ������
	void Put(const VRtParameter& in_record);	
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;
	/// ���������� true, ���� ����������� ���
	bool Empty() const;
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();
	/// ���� ������
	void Flush();
};

/// ����� ������ ����������� �� ����������
class VPsiTransitionOverstepDetector : public VOverstepDetector
{
private:
	/// ������ �������� ������
	VMean level_mean;
	/// ������ ������� ��������
	VMean curv_mean;
	/// ������ ���������� ������
	VCurveCalc calc;
	/// ������� �����������
	std::deque<VBasicOverstepPtr> output;
public:
	/// �����������
	VPsiTransitionOverstepDetector();
	/// ����������
	~VPsiTransitionOverstepDetector();
	/// ���� ������
	void Put(const VRtParameter& in_record);	
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;
	/// ���������� true, ���� ����������� ���
	bool Empty() const;
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();
	/// ���� ������
	void Flush();
};

/// ����� ��������-������ ������
class VZazorExpressDetector : public VOverstepDetector
{
private:
	/// �������
	side_t side;
	/// ������� �����������
	std::deque<VBasicOverstepPtr> output;	
public:
	/// �����������
	VZazorExpressDetector(side_t in_side);
	/// ����������
	~VZazorExpressDetector();
	/// ���� ������	
	void Put(const VRtParameter& in_record);
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;		
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();	
};

/// ����� ������ ���������������� �������
class VZazorOverstepDetector : public VOverstepDetector
{
private:
	/// ������� ������ �� ������
	std::deque<VBasicOverstepPtr> output;
	/// �������
	side_t side;
public:
	/// �����������
	VZazorOverstepDetector(side_t in_side);
	/// ����������
	~VZazorOverstepDetector();
	/// ���� ������	
	virtual void Put(const VRtParameter& in_record);
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();
};

/// ����� ������ ���� � ����� ������� ������� ������
class VZeroZazorOverstepDetector : public VOverstepDetector
{
private:
	/// ������� ������ �� �����
	std::deque<VPoint> zero_zazor;
	/// ������� ������ �� ������
	std::deque<VBasicOverstepPtr> output;
	/// �������
	side_t side;
public:
	/// �����������
	VZeroZazorOverstepDetector(side_t in_side);
	/// ����������
	~VZeroZazorOverstepDetector();
	/// ���� ������	
	virtual void Put(const VRtParameter& in_record);
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();
};

/// ����� ���������� ����������� � ���� ������
class VOverstepAggregator : public VDataProcessor<VBasicOverstepPtr, VBasicOverstepPtr>
{
private:
	/// �����������
	std::deque<VBasicOverstepPtr> oversteps;		
public:	

	/// ����������
	~VOverstepAggregator();
	/// �������� ��������� �����������
	void Put(const VBasicOverstepPtr& in_overstep);		
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();
};

/// ����� ������������� �������� ��������
class VOverstepRestrictionHandler
{
public:
	//�������� ()
	void operator()(const VBasicOverstepPtr& in_overstep)
	{
		in_overstep->CorrectClosure();
	}
};

/// ������ ������ - ����������� �����������
template<typename OverstepHandlerType> class VOverstepProcessor : public VDataProcessor<VBasicOverstepPtr, VBasicOverstepPtr>
{
private:
	/// �����������
	std::deque<VBasicOverstepPtr> oversteps;
	/// ����������
	OverstepHandlerType handler;
public:	
	/// �����������
	VOverstepProcessor() {};

	/// ����������
	~VOverstepProcessor() 
	{
		if (!Empty()) 
			LOG_ERROR(L"VOverstepProcessor: �������������� ������...");
	};

	/// �������� ��������� �����������
	void Put(const VBasicOverstepPtr& in_overstep)
	{						
		handler(in_overstep);
		oversteps.push_back(in_overstep);
	};		
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const
	{
		return oversteps.size()>0;
	};	
	/// ���������� true, ���� ����������� ���
	bool Empty() const
	{
		return oversteps.empty();
	}
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const
	{
		return oversteps.front();
	}
	/// ����������� ������ �������
	void Pop()
	{		
		oversteps.pop_front();
	};	
};
typedef boost::shared_ptr<VDataProcessor<VBasicOverstepPtr, VBasicOverstepPtr> > VOverstepProcessorPtr;

/// ����� ��������������� ����������� � ������� ����������� ��� �������� ����������
class VOverstepArranger : public VDataProcessor<VBasicOverstepPtr, VBasicOverstepPtr>, public VDataIn<double>
{
protected:
	/// ������� ���������
	double position;
public:		
	/// �������� �������� ������� �������
	void Put(const double& in_position)
	{
		position=in_position;
	};
};

/// ����� ��������������� ����������� � ������� ����������� ��� �������� ����������
template<class T> class VOverstepArrangerImpl : public VOverstepArranger
{
private:
	/// �����������
	std::priority_queue<VBasicOverstepPtr, std::vector<VBasicOverstepPtr>, T> oversteps;	
public:	
	
	/// ����������
	~VOverstepArrangerImpl() 
	{
		if (!Empty()) 
			LOG_ERROR(L"VOverstepArrangerImpl: �������������� ������...");
	};

	/// �������� ��������� �����������
	void Put(const VBasicOverstepPtr& in_overstep)
	{			
		oversteps.push(in_overstep);		
	};		
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const
	{
		return (!Empty()) && (fabs(oversteps.top()->GetCoord()-position)>DECISION_DISTANCE);
	};	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const
	{
		return oversteps.empty();
	}
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const
	{
		return oversteps.top();
	}
	/// ����������� ������ �������
	virtual void Pop()
	{		
		oversteps.pop();
	};	
};

typedef VOverstepArrangerImpl< std::less<VBasicOverstepPtr> > VOverstepArrangerDec;
typedef VOverstepArrangerImpl< std::greater<VBasicOverstepPtr> > VOverstepArrangerInc;
typedef boost::shared_ptr<VOverstepArranger> VOverstepArrangerPtr;


/// �������� � ������ ����� (����� 4.9.1 ����� ����������)
static void BridgeCorrection(pro::interval<double> in_range, DEGREE in_degree)
{
	switch (in_degree)
	{
		default:
			break;
		case DEGREE3:
		{
			in_range.right_bound().value=floor(in_range.right_bound().value*0.9);
			break;
		}
		case DEGREE4:
		{
			in_range.left_bound().value=floor(in_range.left_bound().value*0.9);
			in_range.right_bound().value=floor(in_range.right_bound().value*0.9);
			break;
		}
	}
};

/// ��������-����������� (������� 2)
class VRTExpressDetector : public VOverstepDetector
{
private:					
	
	///�������������
	VDataParallel<VRtParameter, VBasicOverstepPtr> overstep_detectors;	
	/// �������
	VOverstepAggregator aggregator;
public:
	/// �����������
	VRTExpressDetector();	
	/// ����������
	~VRTExpressDetector();
	/// ������ �������
	void Flush();
	/// �������� ��������� �����������
	void Put(const VRtParameter& in_record);	
	/// ���������� true, ���� ���� ������� �����������
	bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	bool Empty() const;
	/// ���������� ������ �������
	const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	void Pop();
};

/// ����������� ��� � �������� �������
class VGeometryOverstepDetector : public VOverstepDetector
{
private:
	
	///������������� �����
	VDataParallel<VRtParameter, VBasicOverstepPtr> corrupt_detectors;
	///������������� �����������
	VDataParallel<VRtParameter, VBasicOverstepPtr> overstep_detectors;
	///������������� ���������
	VDataParallel<VBasicOverstepPtr, VBasicOverstepPtr> group_detectors;
	
	/// ���������� �� ������ ����
	diff_calculator<double> coord_diff;
	/// ������������ ��������� ���������
	change_calculator<int> km_change;
	/// �����������
	VOverstepArrangerPtr arranger;
public:
	/// �����������
	VGeometryOverstepDetector();	
	/// ����������
	~VGeometryOverstepDetector();
	/// ������ �������
	void Flush();
	/// �������� ��������� �����������
	virtual void Put(const VRtParameter& in_record);	
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();
};
typedef boost::shared_ptr<VGeometryOverstepDetector> VGeometryOverstepDetectorPtr;

/// ����������� ���. ���������� � �������� �������
class VExtraOverstepDetector : public VOverstepDetector
{
private:	
	
	///������������� �����
	VDataParallel<VRtParameter, VBasicOverstepPtr> corrupt_detectors;
	///�������������
	VDataParallel<VRtParameter, VBasicOverstepPtr> overstep_detectors;	
	/// �����������
	VOverstepArrangerPtr arranger;	
	/// ���������� �� ������ ����
	diff_calculator<double> coord_diff;
	/// ������������ ��������� ���������
	change_calculator<int> km_change;
	
public:
	/// �����������
	VExtraOverstepDetector();	
	/// ����������
	~VExtraOverstepDetector();
	/// ������ �������
	void Flush();
	/// �������� ��������� �����������
	virtual void Put(const VRtParameter& in_record);	
	/// ���������� true, ���� ���� ������� �����������
	virtual bool Ready() const;	
	/// ���������� true, ���� ����������� ���
	virtual bool Empty() const;
	/// ���������� ������ �������
	virtual const VBasicOverstepPtr& Front() const;
	/// ����������� ������ �������
	virtual void Pop();
};
typedef boost::shared_ptr<VExtraOverstepDetector> VExtraOverstepDetectorPtr;

struct VOverstep3;

/// ���������� �������� ������ ��
class VPenaltyCalc
{
	// �������� �����
	long penalty;

	// �������� ����� �� ������
	long crv_penalty;

	// ����������� �����
	double len_coeff;

	// ��� ������ ������������ - �������� ������
	std::vector<__int64> crv_id_anp, crv_id_psi, crv_id_oh;
	
	// ���� ����������� ��� ������ �� ��-515
	const std::vector<std::wstring> cp515_restrictions, cp515_degree_2, cp515_degree_3;

	// ���� �����������
	void Apply(const VAprinfo & apr, bool is_excluded, 
		double coord, __int64 km, const std::wstring & type, long penalty);
		
	// �������� ������������ ����
	bool classic_penalty;

public:

	// �����������
	VPenaltyCalc();
	
	// ���������� �����������
	VPenaltyCalc(const VPenaltyCalc & src);
	
	// �������� ������������
	VPenaltyCalc & operator = (const VPenaltyCalc & src);

	// ���� �����������
	void Apply(const VAprinfo & apr, VBasicOverstepPtr ovr);
	void Apply(const VAprinfo & apr, const VOverstep3 & ovr);

	// ���������� � ������� ����� �������� "�������" ��-515
	void CalcOldStylePenalty(long minSpd1, size_t restrictionCount, 
		size_t deg2Count, size_t deg3Count);

	// ������� �����
	long GetPenalty() const 
	{ 
		return this->penalty * this->len_coeff; 
	}

	// ������� ����� �� ������
	long GetCrvPenalty() const
	{
		return this->crv_penalty * this->len_coeff;
	}
	
	// �������� ������������ ����
	bool GetClassicPenalty() const
	{
	    return this->classic_penalty;
	}
	
	// ������ ����� ����������� �������� ��� ��-515
	const std::vector<std::wstring> & GetCP515RestrictionList() const
	{
	    return this->cp515_restrictions;
	}
	
	// ����������� �������� � ������ ����������� �� ��-515
	bool IsCP515Restriction(const std::wstring & type) const
	{
	    return std::find(this->cp515_restrictions.begin(), this->cp515_restrictions.end(), type) != this->cp515_restrictions.end();
	}
	
	// ������ ����� ����������� 2 ������� ��� ��-515
	const std::vector<std::wstring> & GetCP515Degree2List() const
	{
	    return this->cp515_degree_2;
	}
	
	// ����������� �������� � ������ 2 �������� �� ��-515
	bool IsCP515Degree2(const std::wstring & type) const
	{
	    return std::find(this->cp515_degree_2.begin(), this->cp515_degree_2.end(), type) != this->cp515_degree_2.end();
	}
	
	// ������ ����� ����������� 3 ������� ��� ��-515
	const std::vector<std::wstring> & GetCP515Degree3List() const
	{
	    return this->cp515_degree_3;
	}
	
	// ����������� �������� � ������ 3 �������� �� ��-515
	bool IsCP515Degree3(const std::wstring & type) const
	{
	    return std::find(this->cp515_degree_3.begin(), this->cp515_degree_3.end(), type) != this->cp515_degree_3.end();
	}
	
};

/// ������ ������ ���������
class VRTKmData
{
private:

	/// �������� ��� ������ �����������
	struct find_degree_group
	{
		DEGREE degree;
		OVERSTEP_GROUP group;
		find_degree_group(DEGREE in_degree, OVERSTEP_GROUP in_group) : degree(in_degree), group(in_group) {};
		bool operator()(const VBasicOverstepPtr& in_data) const {return in_data->GetDegree()==degree && in_data->GetGroup()==group;};
	};
	struct find_group
	{
		OVERSTEP_GROUP group;
		find_group(OVERSTEP_GROUP in_group) : group(in_group) {};
		bool operator()(const VBasicOverstepPtr& in_data) const {return in_data->GetGroup()==group;};
	};
	struct find_restriction_group
	{
		OVERSTEP_GROUP group;
		find_restriction_group(OVERSTEP_GROUP in_group) : group(in_group) {};
		bool operator()(const VBasicOverstepPtr& in_data) const {return in_data->GetGroup()==group && in_data->HasRestriction();};
	};
	struct find_restriction
	{			
		bool operator()(const VBasicOverstepPtr& in_data) const {return in_data->HasRestriction();};
	};
	
	/// ��������� ������
	VAprinfoKmRecord aprinfo_km;	
	/// ������
	std::deque<VRtParameter> data;		
	/// �����������
	std::deque<VBasicOverstepPtr> oversteps;
	/// ��������
	VSpeeds speeds;
	VSpeeds min_speeds;	
	/// ������ ����������
	VPenaltyCalc penalty;

public:	
	/// �����������
	VRTKmData(int in_km);

	/// ���������� ���������� ����������� �� ������
	size_t GetOverstepCount(OVERSTEP_GROUP in_group) const;
	/// ���������� ���������� ����������� �� ������� � ������
	size_t GetOverstepCount(OVERSTEP_GROUP in_group, DEGREE in_degree) const;
	/// ���������� ���������� ������������ ����������� �� ������
	size_t GetOverstepAmount(OVERSTEP_GROUP in_group) const;
	/// ���������� ���������� ������������ ����������� �� ������� � ������
	size_t GetOverstepAmount(OVERSTEP_GROUP in_group, DEGREE in_degree) const;
	/// ���������� ����� �����������
	size_t GetRestrictionCount(OVERSTEP_GROUP in_group) const;	
	/// ���������� ���������� �����������
	size_t GetRestrictionCount() const;			
	/// ���������� true ���� �������� ����������
	bool HasRestriction() const;
	/// ���������� ����������� ��������
	VSpeeds GetRestriction() const;
	/// ���������� true ���� �������� ����������
	bool HasRestriction(OVERSTEP_GROUP in_group) const;
	/// ���������� ����������� ��������
	VSpeeds GetRestriction(OVERSTEP_GROUP in_group) const;
	/// ���������� �������� ����������� ��������
	std::wstring GetRestrictionString() const;
	/// ���������� �������� ����������� ��������
	std::wstring GetRestrictionString(OVERSTEP_GROUP in_group) const;	
	/// ���������� ������ �������� �������
	std::wstring GetSectionString() const;
	///���������� ������ �� ������������
	std::wstring GetSummaryString(OVERSTEP_GROUP in_group) const;	
	/// ���������� �������
	std::wstring GetPeregon() const;
	/// ���������� ����� ���������
	int GetKm() const;
	/// ���������� ���������� ������ ���������
	double GetKmStart() const;
	/// ���������� ���������� ����� ���������
	double GetKmEnd() const;
	/// ���������� ����� ���������
	double GetKmLength() const;
	/// ���������� ������� �������� �� ���������
	double GetAverageSpeed() const;
	/// ��������� ���� ������
	void PutData(const VRtParameter& in_data);	
	/// ���������� ���� ������
	const VRtParameter& GetData(size_t in_index) const; 
	/// ���������� ����� ������ ������
	size_t GetDataCount() const;	
	/// ���������� ����� ������
	double GetDataLength() const;
	///��������� �����������
	void PutOverstep(const VBasicOverstepPtr& in_overstep);
	/// ���������� �����������
	const VBasicOverstepPtr& GetOverstep(size_t in_index) const;
	/// ���������� ����� �����������
	size_t GetOverstepCount() const;
	/// ���������� ������� ����� ������
	pro::interval<double> GetCoordRange() const;	
	/// ��������� �������
	void CalcPenalty();
	/// ����������� �� ��������
	long GetPenalty() const;
	/// ����������� �� �������� �� ������
	long GetCrvPenalty() const;
};
typedef boost::shared_ptr<VRTKmData> VRTKmDataPtr;

/// ������ ����������
class VRTKmProcessor : public VDataIn<VRtParameter>, public VDataIn<VBasicOverstepPtr>, public VDataOut<VRTKmDataPtr>
{
private:
	/// ���������
	std::deque<VRTKmDataPtr> km;
	/// ����������� ��������� ��
	change_calculator<long>	pch_changed;
public:	
	/// �����������
	VRTKmProcessor();	
	/// �������� �� ���� ���� ������
	virtual void Put(const VRtParameter& in_data);
	/// �������� �� ���� �����������
	virtual void Put(const VBasicOverstepPtr& in_overstep);
	/// ���������� true, ���� ������ ������
	virtual bool Ready() const;
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VRTKmDataPtr& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();		
};
typedef boost::shared_ptr<VRTKmProcessor> VRTKmProcessorPtr;

/// ������� ����������
class VKmRater : public VDataProcessor<VRTKmDataPtr, VRTKmDataPtr>
{
private:
	/// ���������
	std::deque<VRTKmDataPtr> output;
public:	
	/// �����������
	VKmRater();	
	/// �������� �� ���� ���� ������
	virtual void Put(const VRTKmDataPtr& in_data);
	/// ���������� true, ���� ������ ������
	virtual bool Ready() const;
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VRTKmDataPtr& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();		
};
typedef boost::shared_ptr<VKmRater> VKmRaterPtr;

// -------------------------------- ����������� ������������� �������� �� ���������� �� --------------------------------------

/// ���� �������
#define WIDTH_CORRUPT_DETECTION_BASE 10.0
/// ����� �������
#define WIDTH_CORRUPT_DETECTION_LENGTH 10.0
/// �������� �������
#define WIDTH_CORRUPT_DETECTION_VALUE 15.0
/// ��������
#define WIDTH_CORRUPT_DETECTION_DELAY 60.0


/// ����� �������������� ���� �� � ���� ������� � ������������� ��������
class VWidthCorruptDetector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// ������ �� �����
	std::deque<VRtParameter> input;	
	/// ������ �� ������
	std::deque<VRtParameter> output;
	/// 
	VMeanVariate width_variate;
	/// ����� ��������
	VFindExcess width_excess_detector;
	
public:
	/// �����������
	VWidthCorruptDetector();		
	/// ����������
	~VWidthCorruptDetector();
	/// �������� �� ���� ���� ������
	virtual void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ������ ������
	virtual bool Ready() const;
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();
	/// ������������ ������� ������ �� �����
	virtual void Flush();
};

/// ����� �������������� �������
class VSwitchDetector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// ������ �� �����
	std::deque<VRtParameter> input;	
	/// ������ �� ������
	std::deque<VRtParameter> output;
	
	/// 
public:
	/// �����������
	VSwitchDetector();		
	/// ����������
	~VSwitchDetector();
	/// �������� �� ���� ���� ������
	virtual void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ������ ������
	virtual bool Ready() const;
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();
	/// ������������ ������� ������ �� �����
	virtual void Flush();	
};

/// ����� ����������� �����
class VWearCalculator : public VDataProcessor<VRtParameter, VRtParameter>
{
private:	
	/// ������ �� ������
	std::deque<VRtParameter> output;
public:
	/// �����������
	VWearCalculator();		
	/// ����������
	~VWearCalculator();
	/// �������� �� ���� ���� ������
	virtual void Put(const VRtParameter& in_record);
	/// ���������� true, ���� ������ ������
	virtual bool Ready() const;
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();	
};

/// ����� ����������� ������� ������
class VZazorCalculator : public VDataProcessor<VRtParameter, VRtParameter>
{
private:	
	
	/// ������ �� �����
	std::deque<VRtParameter> input;	
	/// ������ �� ������
	std::deque<VRtParameter> output;
	
	/// ��������� ���������� ����������
	accumulator_set< double, stats<tag::count, tag::mean, tag::median, tag::min, tag::max > > diff_stat_left;
	accumulator_set< double, stats<tag::count, tag::mean, tag::median, tag::min, tag::max > > diff_stat_right;
	
public:
	/// �����������
	VZazorCalculator();		
	/// ����������
	~VZazorCalculator();
	/// �������� �� ���� ���� ������
	virtual void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ������ ������
	virtual bool Ready() const;
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();	
	/// ���� ������
	virtual void Flush();
};

/// ����� ����������� ��������
class VMarkerDetector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// ������ �� ������
	std::deque<VRtParameter> output;
public:
	/// �����������
	VMarkerDetector();
	/// ����������
	~VMarkerDetector();
	/// �������� ������ �� ����
	void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const;		
	/// ���������� ������ ������� ������
	const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	void Pop();
};

/// ����� ������ ������
class VCurveDetector : public VDataProcessor<VRtParameter, VRtParameter>
{	
private:
	/// ������ �� �����
	std::deque<VRtParameter> input;
	/// ������ �� ������
	std::deque<VRtParameter> output;
	/// �������
	side_t side;
	/// �������
	VCurveCondition condition;
			
public:
	/// �����������
	VCurveDetector(side_t in_side, double in_min_curv=1);
	/// ����������
	~VCurveDetector();
	/// �������� ������ �� ����
	void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const;		
	/// ���������� ������ ������� ������
	const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	void Pop();
	/// ���� ������
	void Flush();	
};

/// ���������� ������ �� ���������
class VDefaultSidewaySwitchHandler
{
public:
	void operator()(const std::wstring& in_message)
	{
		LOG_WARNING(in_message);
	};
};

/// ����������� ������� �� ��������
template<typename HandlerType> class VSidewaySwitchDetector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// ������ �� �����
	std::deque<VRtParameter> input;
	/// ������ �� ������
	std::deque<VRtParameter> output;
	/// ����� ����������
	VFindExtremum extremum_left;
	/// ����� ����������
	VFindExtremum extremum_right;
	/// ����� ������� �� ������� �������
	int optical_switch_count;
public:
	/// �����������
	VSidewaySwitchDetector() : extremum_left(10), extremum_right(10), optical_switch_count(0) {};
	/// ����������
	~VSidewaySwitchDetector() {};
	/// �������� ������ �� ����
	void Put(const VRtParameter& in_data)
	{
		input.push_back(in_data);
		if (input.back().expandedButtons.IsSwitch())
		{
			optical_switch_count++;	
		}
		if (fabs(input.back().absCoord-input.front().absCoord)>SIDEWAY_SWITCH_LENGTH().Value())
		{
			if (optical_switch_count>0)
			{
				double abs_coord=(input.back().absCoord+input.front().absCoord)/2;
				double curv_conv_left=0;
				double curv_conv_right=0;
				for (size_t i=0;i<input.size();++i)
				{		
					double x=input.at(i).curv.value/SIDEWAY_SWITCH_SCALE().Value();
					double h_left=sin(M_PI+2.0*M_PI*double(i)/double(input.size()));
					double h_right=sin(2.0*M_PI*double(i)/double(input.size()));
					curv_conv_left+=h_left*x;
					curv_conv_right+=h_right*x;
				}
				extremum_left.Put(VPoint(abs_coord, curv_conv_left));
				extremum_right.Put(VPoint(abs_coord, curv_conv_right));
				
				while (extremum_left.Ready())
				{
					if (extremum_left.Front().GetValue()>SIDEWAY_SWITCH_RANGE().Value())
					{
						HandlerType()(boost::str(boost::wformat(L"����� �� ������� ���� (������): %f") % extremum_left.Front().GetCoord()));
					}
					extremum_left.Pop();
				}
				while (extremum_right.Ready())
				{
					if (extremum_right.Front().GetValue()>SIDEWAY_SWITCH_RANGE().Value())
					{
						HandlerType()(boost::str(boost::wformat(L"����� �� ������� ���� (�������): %f") % extremum_right.Front().GetCoord()));
					}
					extremum_right.Pop();
				}
			}
			if (input.front().expandedButtons.IsSwitch())
			{
				optical_switch_count--;	
			}
			transfer<VRtParameter>(input,output);
		}
	};
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const
	{
		return output.size()>0;
	}
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const
	{
		return output.empty();		
	}
	/// ���������� ������ ������� ������
	const VRtParameter& Front() const
	{
		return output.front();
	}
	/// ����������� ������ ������� ������
	void VSidewaySwitchDetector::Pop()
	{
		output.pop_front();
	}
	/// ���� ������
	void Flush()
	{
		while (!input.empty())
			transfer<VRtParameter>(input,output);
	}

};

/// ����� ���������� ���������� ��� ������ ��������
class VPassportStatistics
{
private:
	accumulator_set< double, stats<tag::covariance<double, tag::covariate1> > > covariate;
	accumulator_set< double, features<tag::variance, tag::max, tag::min, tag::mean, tag::median > > parameter;
	accumulator_set< double, features<tag::variance, tag::max, tag::min, tag::mean, tag::median > > passport;
public:
	/// �����������
	VPassportStatistics();
	/// ���������� ������
	void Put(double in_parameter, double in_passport);
	/// ���������� ����������� ����������
	double GetCorrelation() const;
	/// ���������� ������ ��������
	double GetPassportRange() const;	
};

/// ����� ����������� �������� ����������� � ���������.
class VBadPassportDetector : public VDataProcessor<VRtParameter, VRtParameter>
{	
private:
	/// ������ �� �����
	std::deque<VRtParameter> input;
	/// ������ �� ������
	std::deque<VRtParameter> output;		
public:
	/// �����������
	VBadPassportDetector();
	/// ����������
	~VBadPassportDetector();
	/// �������� ������ �� ����
	void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const;		
	/// ���������� ������ ������� ������
	const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	void Pop();
	/// ���� ������
	void Flush();
};

/// ����� ���������� ���������� �������� � �������� (��� �������� �������� �������� ���������)
class VCorrectionVerificator : public VDataProcessor<VRtParameter, VRtParameter>
{	
private:
	/// ������ �� �����
	std::deque<VRtParameter> input;
	/// ������ �� ������
	std::deque<VRtParameter> output;
	/// �������� ����������� ����������
	std::deque<double> monotone;
	/// �������� ������������� � ������������ ����
	accumulator_set< double, stats<tag::max, tag::min, tag::mean, tag::median> > diff;
	/// �������� ���������� � ���������
	VPassportStatistics plan;		
public:
	/// �����������
	VCorrectionVerificator();
	/// ����������
	~VCorrectionVerificator();
	/// �������� ������ �� ����
	void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const;		
	/// ���������� ������ ������� ������
	const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	void Pop();
};

/// ����� ����������� ���� ������������� ����������
class VNstKmDetector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// ������ �� �����
	std::deque<VRtParameter> input;
	/// ������ �� ������
	std::deque<VRtParameter> output;
public:
	/// �����������
	VNstKmDetector();
	/// ����������
	~VNstKmDetector();
	/// �������� ������ �� ����
	void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const;		
	/// ���������� ������ ������� ������
	const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	void Pop();
	/// ���� ������
	void Flush();
};

/// ����� �������� ����������� ������ �� ������
class VWearVerificator : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// ������ �� �����
	std::deque<VRtParameter> input;
	/// ������ �� ������
	std::deque<VRtParameter> output;
public:
	/// �����������
	VWearVerificator();
	/// ����������
	~VWearVerificator();
	/// �������� ������ �� ����
	void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const;		
	/// ���������� ������ ������� ������
	const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	void Pop();	
};


/// �����, ������������� �������, � ����������� �� ��������� ������
class VSwitchesInvertor : public VDataProcessor<VRtParameter, VRtParameter>
{
private:	
	/// ������ �� ������
	std::deque<VRtParameter> output;		
public:
	/// �����������
	VSwitchesInvertor();		
	/// ����������
	~VSwitchesInvertor();
	/// �������� �� ���� ���� ������
	virtual void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ������ ������
	virtual bool Ready() const;
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();	
};

/// ����� �������������� ������
class VCurveCorrector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// ������ �� �����
	std::deque<VRtParameter> input;
	/// �������
	std::deque<point_t> level_points;
	/// �������� �����
	std::deque<point_t> rich_left_points;
	/// �������� ������
	std::deque<point_t> rich_right_points;
	/// ��
	std::deque<point_t> width_points;
	/// ������ �� ������
	std::deque<VRtParameter> output;

	change_calculator<ZLINE_TYPE> level_changed;
	change_calculator<ZLINE_TYPE> plan_changed;
	
public:
	/// �����������
	VCurveCorrector();		
	/// ����������
	~VCurveCorrector();
	/// �������� �� ���� ���� ������
	virtual void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ������ ������
	virtual bool Ready() const;
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();
	/// ������������ ������� ������ �� �����
	virtual void Flush();
};

/// ����� ��������� ������ ������
class VStartCorrector : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// ������ �� �����
	std::deque<VRtParameter> input;
	/// ������ �� ������
	std::deque<VRtParameter> output;
	/// ������ ������
	double shift;
	/// ������� ���� ��� ��������� ��������� ��� ����������
	bool skip;	

public:
	/// �����������
	VStartCorrector();
	/// ����������
	~VStartCorrector();
	/// ���� ������
	void Flush(double in_shift=0);
	/// �������� �� ���� ���� ������
	virtual void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ������ ������
	virtual bool Ready() const;
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();	
};

/// ������� ������
class VDataCounter : public VDataProcessor<VRtParameter, VRtParameter> 
{
private:
	/// �������
	int count;
	/// ������ �� ������
	std::deque<VRtParameter> output;
public:
	/// �����������
	VDataCounter() : count(0) {};
	/// ����������
	~VDataCounter()
	{
		LOG_DEBUG(boost::str(boost::wformat(L"%d ������") % count));
	};	
	/// �������� �� ���� ���� ������
	virtual void Put(const VRtParameter& in_data) 
	{
		count++;
		output.push_back(in_data);
	};
	/// ���������� true, ���� ������ ������
	virtual bool Ready() const {return output.size()>0;};
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const {return output.empty();};		
	/// ���������� ������ ������� ������
	virtual const VRtParameter& Front() const {return output.front();};
	/// ����������� ������ ������� ������
	virtual void Pop() {output.pop_front();};
};

/// ������� ��������� ���������
class VCoordCorrectionFunction
{
private:
	/// ������ ������
	std::deque<line_t> lines;
	
public:
	/// ��������� �����
	void Update(const line_t& in_line);
	/// ��������� �����
	void Update(double in_coordinate, const line_t& in_line);
	/// ��������� �����
	void Update(point_t in_point, const line_t& in_line);
	/// ���������� ����������������� ����������
	double Correct(double in_coordinate);
	/// ���������� true, ���� ��������� ��������
	bool Valid() const;
	/// ���������� �������
	void Reset();
	/// ���������� �������� ������� � �����
	double Value(double in_coordinate)  const;
	/// ���������� �������� ������
	double GetError() const;
	/// ���������� ����������� �������� �������� ������
	double GetWheelDiameterCorrection() const;
};

/// ��������� ���������
class VCoordCorrector : public VDataProcessor<VRtParameter, VRtParameter> 
{
private:

	//������

	/// ������ �� �����
	std::deque<VRtParameter> input;
	/// ������ �� ������
	std::deque<VRtParameter> output;
	/// ��������� �����
	mml_calculator mml;	
	/// ������� ������� �������������
	bool fail;
	
public:
	/// �����������
	VCoordCorrector();
	/// ����������
	~VCoordCorrector();
	/// ���� ������
	void Flush();
	/// �������� �� ���� ���� ������
	virtual void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ������ ������
	virtual bool Ready() const;
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();
};

struct VRichCalcHelper
{
	double K;
	double R;
	double r;
	double X;
	double Y;
	double result;
	double Rx() const {return R*sin(K);};
	double Ry() const {return R*cos(K);};
	double rx() const {return r*sin(K);};
	double ry() const {return r*cos(K);};
	double dr() const {return R-r;}
	double dx() const {return (Rx()-rx())/1000.0;};
	double dy() const {return (Ry()-ry())/1000.0;};
};

struct VRichCalcData
{
	VRichCalcData(const VRtParameter& in_data) : data(in_data)
	{
		absCoord=0;
		
		rich_left.K=0;
		rich_left.X=0;
		rich_left.Y=0;
		rich_left.R=data.richLeft.value;
		rich_left.r=0;
		rich_left.result=0;
		
		rich_right.K=0;
		rich_right.X=0;
		rich_right.Y=0;
		rich_right.R=data.richRight.value;
		rich_right.r=0;
		rich_right.result=0;
	};
	double absCoord;
	VRichCalcHelper rich_left;
	VRichCalcHelper rich_right;
	VRtParameter data;
};

/// ����� ����������� ���������� ����
class VRichCalculator : public VDataProcessor<VRtParameter, VRtParameter>
{
private:
	/// ������ �� �����
	std::deque<VRichCalcData> input;	
	/// ������ �� ������
	std::deque<VRichCalcData> output;
	/// �������� ����� �����
	double Lf;
	/// ������ ����� �����
	double Lb;
	/// �������� ����� �����
	double lf;
	/// ������ ����� �����
	double lb;
	/// ��������
	double precision;
	/// ������ ������
	double buffer_size;
	/// ������� ����
	double deadzone;	
	/// ������� �������������
	bool initialized;
	/// ������������ ����� ��������
	size_t max_iteration_count;
	
	/// �������������
	void Init(const VRtParameter& in_data);
	/// ��������������� ����������
	void InputPreprocess();
	/// ��������� �����
	void CalculatePlan();
	/// ���������� ��������
	void CalculateRich();
	
public:
	/// �����������
	VRichCalculator();
	/// ����������
	~VRichCalculator();
	/// �������� �� ���� ���� ������
	void Put(const VRtParameter& in_data);
	/// ���������� true, ���� ������ ������
	bool Ready() const;
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const;		
	/// ���������� ������ ������� ������
	const VRtParameter& Front() const;
	/// ����������� ������ ������� ������
	void Pop();
	/// ���� ������
	void Flush();
};

/// ����������� � �����
struct VOverstep3
{
	double absCoord;			// ���.�-��
	double begin, end;			// ������� �����������
	__int64 km;					// ��
	double m;					// �
	double value;				// ��������
	double length;				// �����
	__int64 degree;				// �������
	wchar_t description[256];	// �������� 
	wchar_t comment[256];		// �����������
	wchar_t type[32];			// ���
	__int64 spd1, spd2, spd3;	// ��������
	__int64 rst1, rst2, rst3;	// �����������
	__int64 hasRestriction;		// ������� ��������
	__int64 amount;				// ���-�� �����������
	__int64 penalty;			// �����
	__int64 closeToUpper;		// �������� � ������ �������

#pragma pack(push, 1)
	bool is_excluded;			// ���������
	__int16 cp515_revision;		// ������� ��-515: 0=��� ��.������, 1=������ ��, 2=����� ��
	__int8 reserve0[13];
#pragma pack(pop)

	wchar_t excl_reason[256];	// ������� ����������

	__int64 hi_speeds;			// 1 = true, 0 = false
	__int64 spd_hi, spd_hi_bt;  // ������
	__int64 rst_hi, rst_hi_bt;  // �����������

	//	__int64 reserve[36];		// ������ (�� 2048 ����)
	__int64 reserve[31];		// ������ (�� 2048 ����)

	// �����������
	VOverstep3();
	
	// ���������� ����������� - �� BasicOverstep
	VOverstep3(VBasicOverstepPtr ovr);

	// ������ �� ���� � ���� - ��� ������������� � VIndexList
	bool Import(const VOverstep3 & src)
	{
		*this = src;
		return true;
	}

	// ������� �� ���� � ���� - ��� ������������� � VIndexList
	VOverstep3 Export() const
	{
		return *this;
	}

	// ��������� � ������
	bool MeetsSection(const VWaySection * section) const
	{
		VWayCoord wc(this->km, this->m);
		return section == NULL || (wc >= section->start && wc <= section->end);
	}

	// �������� ����
	bool IsType(const wchar_t * t) const
	{
		return wcscmp(this->type, t) == 0;
	}

	// ���������� ���
	void SetType(const std::wstring & type) 
	{
		wcsncpy(this->type, type.c_str(), 31);
		this->type[31] = 0;
	}

	// ���������� ��������
	void SetDescription(const std::wstring & descr)
	{
		wcsncpy(this->description, descr.c_str(), 255);
		this->description[255] = 0;
	}

	// ���������� �����������
	void SetComment(const std::wstring & comment)
	{
		wcsncpy(this->comment, comment.c_str(), 255); 
		this->comment[255] = 0;
	}

	// ���������� ������� �������������
	void SetExclusionReason(const std::wstring & excl_reason)
	{
		wcsncpy(this->excl_reason, excl_reason.c_str(), 255); 
		this->excl_reason[255] = 0;
	}

	// ������
	VSpeeds GetSpeed() const
	{
		if (this->hi_speeds)	
			return VSpeeds(this->spd1, this->spd2, this->spd3, this->spd_hi, this->spd_hi_bt);
		else
			return VSpeeds(this->spd1, this->spd2, this->spd3);

	}

	// ������������ �������� �� �������
	const speed_t & GetMaxSpeed() const
	{
		return this->GetSpeed().GetMaxSpeed();
	}

	// �����������
	VSpeeds GetRestriction() const
	{
		if (this->hi_speeds)	
			return VSpeeds(this->rst1, this->rst2, this->rst3, this->rst_hi, this->rst_hi_bt);
		else
			return VSpeeds(this->rst1, this->rst2, this->rst3);
	}

	// ������� ����������� � ������� �������, �� ����� ������
	bool Fits(size_t kadrCount) const { return true; }
}; 

bool operator < (const VOverstep3 & o1, const VOverstep3 & o2);

class VOvr3List : public VIndexList<VOverstep3>
{
public:

	VOvr3List() : VIndexList<VOverstep3>()
	{}

	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		bool result = this->DefaultLoad<FILETYPE, VOverstep3>(file, type, kadrCount);
		this->Sort();
		return result;
	}

	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VOverstep3>(file, type);
	}

	bool CopyFrom(const VOvr3List & src, const VWaySection * section);
	bool CopyFrom(const std::vector<VBasicOverstepPtr> & src);
	
	long GetCP515Revision() const
	{
	    for (size_t i = 0; i < this->GetCount(); ++i)
	    {
	        return this->GetItem(i).cp515_revision;
	    }
	    return 0;
	}
};

/// ������� ����������
DECLARE_VARIABLE_MT(double, CURRENT_COORDINATE, 0);

/// ���� ���������� ��������
DECLARE_OPTION(double, MEAN_RICH_BASE, 48.0, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// ���� ���������� ������
DECLARE_OPTION(double, MEAN_LEVEL_BASE, 48.0, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// ����� ������ �����
DECLARE_OPTION(int, CORRUPT_DETECTION_POLICY, 1, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// ���������� ������� ����� �� ������
DECLARE_OPTION(double, CORRUPT_PERCENT, 50, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// ����� ������ �� ������� ���� �� �������
DECLARE_OPTION(double, SIDEWAY_SWITCH_LENGTH, 80, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// ���������� ����������� �������� �� ������ �� ������� ���� �� �������
DECLARE_OPTION(double, SIDEWAY_SWITCH_SCALE, 180, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// ����� ��� ����������� ������ �� ������� ���� �� �������
DECLARE_OPTION(double, SIDEWAY_SWITCH_RANGE, 100, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// ������� ��������� �������� ������ �� ������� ���� (0 - ������������� �� ���������� �������, 1 - ��������� � �������� "����."
DECLARE_OPTION(int, BAD_PASSPORT_HANDLING_POLICY, 1, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// ������ �������� ������ �/�� (����������� k � ��������� y=kx+b, ���������� �� 1000)
DECLARE_OPTION_MT(double, COORD_CORRECTION_SLOPE, 0, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// ��������� ������ ��������� ���������
DECLARE_VARIABLE_MT(VCoordCorrectionFunction, COORD_CORRECTION_FUNCTION, VCoordCorrectionFunction());

#define OLD_INSTRUCTION DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515)
#define NEW_INSTRUCTION DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_EXPERIMENT)