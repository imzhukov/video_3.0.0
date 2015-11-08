#pragma once

#include "vaprinfo.h"
#include "singleton.h"
#include "VCalcOptions.h"
#include "markers.h"
#include "rtparam.h"

/// ����� ���������� ������� (�)
#define OPTICAL_SWITCH_LENGTH 60.0
/// ������������ �������� ��������� ��������� ����������
#define MAX_TRANSITION_CORRECTION 50.0
/// ����������� ����� �����
#define MIN_BRIDGE_LENGTH 25.0
/// ����������� �������� ����� ���������
#define MIN_MARKER_INTERVAL 1500.0

/// ����� �������
class VRTMessage
{
private:
	/// ����������
	double coord;
	/// ���������
	std::wstring message;
public:
	/// �����������
	VRTMessage(double in_coord, const std::wstring& in_message);
	/// ���������� ����������
	double GetCoord() const;
	/// ���������� ���������
	const std::wstring& GetMsg() const;
};

/// ����������� ������������ �����
static const std::wstring BAD_PLAN_SIGN=L"[����]";
/// ����������� ������������ ������
static const std::wstring BAD_LEVEL_SIGN=L"[����.]";
/// ����������� ������������ ����������
static const std::wstring BAD_COORDINATES_SIGN=L"[�����.]";
/// ���������� ����������� ���������� �����
static const double PLAN_CORRELATION=0.8;
/// ���������� ����������� ���������� ������
static const double LEVEL_CORRELATION=0.8;
/// ������� ���������� ���������
static const double COORDINATES_MATCH_PERCENT=80.0;

/// ������� �������������� ��������
class VBadPassport
{
private:
	/// �������
	pro::interval<double> section;	
	/// ����������� ���������� ��������
	double plan_correlation;	
	/// ����������� ���������� ������
	double level_correlation;
	/// ������ �������� ��������
	double plan_range;	
	/// ������ �������� ������
	double level_range;
	/// ������� ���������� ��������� ������
	double coordinates_match_percent;
public:
	/// �����������
	VBadPassport(double in_start, double in_end, 
		double in_plan_correlation, double in_plan_range,		
		double in_level_correlation, double in_level_range,
		double in_coordinates_match_percent);
	/// ���������� �������
	const pro::interval<double>& GetSection() const;
	/// ���������� ��������
	const std::wstring GetDescription() const;
	/// ���������� true, ���� ������ �� ������������� �������� �� ����� ��������
	bool IsBadPlan() const;			
	/// ���������� true, ���� ������ �� ������������� �������� �� ������
	bool IsBadLevel() const;
	/// ���������� true, ���� ������ �� ������������� �������� �� �����������
	bool IsBadCoordinates() const;
	/// ���������� true, ���� ������ �� ������������� �������� ���� �� �� ������ �� ����������
	bool IsBad() const;
};

enum PWLF_TYPE {PWLF_PASSPORT=0, PWLF_ZERO=1};

/// ������� ����, ������������ � �������� �������
class VSituation
{
private:

	/// �������� ������ �������� �������������� ���������
	struct find_bad_passport
	{
		double coord;
		find_bad_passport(double in_coord) : coord(in_coord) {};
		bool operator()(const VBadPassport& in_bad_passport) {return in_bad_passport.GetSection().contains(coord);};
	};

	/// �������� ��������� ����������
	VAprinfo aprinfo;

	/// ������ ����������
	std::vector<VAprinfoKmRecord> km_list;
	/// ������ ������
	std::vector<VAprinfoCurve> curve_list;
	/// ������� ������������� ���������� ������
	std::deque<VBadPassport> bad_passport;
	/// �������
	std::deque<VAprinfoSwitchRecord> switches;
	/// ��������� �������
	std::vector<VAprinfoSwitchRecord> apr_switches;
	/// �������
	std::deque<VRTMessage> messages;

	/// ���������� ����� (�������)
	zline level_passport;
	/// ���������� ����� (��������)
	zline rich_passport;
	/// ���������� ����� (��)
	zline width_passport;
		
	/// ���������� ������� (�������)
	zline level_previous;
	/// ���������� ������� (��������)
	zline rich_previous;

	/// ������� ����� (�������)
	zline level_zero;
	/// ������� ����� (�������� �����)
	zline rich_left_zero;
	/// ������� ����� (�������� ������)
	zline rich_right_zero;
	/// ������� ����� (��)
	zline width_zero;
	
	/// �������
	VWayMarkerList markers;
	
	/// ������������ ����� ������� �����
	void AdjustLine(zline& out_zline, size_t in_index, const line_t& in_line);
	
public:

	/// �����������
	VSituation();
	/// �����������
	VSituation(const VAprinfo& in_aprinfo);
	/// �������������
	void Init(const VAprinfo& in_aprinfo);
	/// ���������� ������������ �������� �� �����������
	speed_t GetMaxSpeed() const;
	/// ���������� ����� �������� �� �����������
	size_t GetMarkerCount() const;
	/// ���������� ������������ ����� ������� ����� ��������� �� �����������
	double GetMaxMarkerInterval(const pro::interval<double>& in_coordinates) const;
	/// ���������� �������� ���������� ����������
	const VAprinfo& GetAprinfo() const;
	/// ���������� ��������� ��������
	VAprinfoKmRecord GetNearestKm(double in_coord) const;

	/// ���������� ���������� ��������� �������
	size_t GetAprSwitchCount() const;
	/// ���������� ��������� �������
	const VAprinfoSwitchRecord& GetAprSwitch(size_t in_index) const;

	/// ��������� ������� �������������� ��������
	void AddBadPassport(const VBadPassport& in_bad_passport);
	/// ���������� ������� �������������� ��������
	const VBadPassport& GetBadPassport(size_t in_index) const;
	/// ���������� ���������� �������� �������������� ��������
	size_t GetBadPassportSize() const;
	/// ���������� ������� ������������� ���������� ������
	bool BadPassport(double in_coord) const;	
	/// ���������� ������� ������������� ���������� ������
	bool BadPlanPassport(double in_coord) const;	
	/// ���������� ������� ������������� ���������� ������
	bool BadLevelPassport(double in_coord) const;
	// ���������� ������� ������������� ���������� ������
	bool BadCoordinatePassport(double in_coord) const;
	/// ��������� �������
	void AddSwitch(double in_coord, side_t in_side);
	/// ���������� �������
	const VAprinfoSwitchRecord& GetSwitch(size_t in_index) const;
	/// ���������� ���������� �������
	size_t GetSwitchCount() const;
	/// ���������� ������� �������
	bool OnSwitch(const pro::interval<double>& in_range) const;
	/// ��������� ���������
	void AddMessage(const VRTMessage& in_message);
	/// ���������� ���������� ���������
	size_t GetMessageCount() const;
	/// ���������� ���������
	const VRTMessage& GetMsg(size_t in_index) const;
	/// ���������� �������� �� ���������� ����� (��������)
	double GetPassportRich(double in_coord) const;
	/// ���������� �������� �� ���������� ����� (�������)
	double GetPassportLevel(double in_coord) const;
	/// ���������� �������� �� ������� ����� (�������)
	double GetLevel(double in_coord) const;
	/// ���������� �������� �� ������� ����� (�������� �����)
	double GetRichLeft(double in_coord) const;
	/// ���������� �������� �� ������� ����� (�������� ������)
	double GetRichRight(double in_coord) const;
	/// ���������� �������� �� ������� ����� (��)
	double GetWidth(double in_coord) const;
	/// ������������ ������� ����� (�������)
	void CorrectLevelTransition(const std::deque<point_t>& in_points);
	/// ������������ ������� ����� (��������)
	void CorrectRichLeftTransition(const std::deque<point_t>& in_points);
	/// ������������ ������� ����� (�������� ������)
	void CorrectRichRightTransition(const std::deque<point_t>& in_points);
	/// ������������ ������� ����� (��)
	void CorrectWidthTransition(const std::deque<point_t>& in_points);
	/// ������������ ������� ����� (�������)
	void CorrectLevelRadius(const std::deque<point_t>& in_points);
	/// ������������ ������� ����� (�������� �����)
	void CorrectRichLeftRadius(const std::deque<point_t>& in_points);
	/// ������������ ������� ����� (�������� ������)
	void CorrectRichRightRadius(const std::deque<point_t>& in_points);
	/// ������������ ������� ����� (��)
	void CorrectWidthRadius(const std::deque<point_t>& in_points);	
	/// ���������� ����� ����� ������� ����� (�������)
	size_t GetLevelCount(PWLF_TYPE in_type);
	/// ���������� ����� ����� ������� ����� (��������)
	size_t GetRichCount(PWLF_TYPE in_type, side_t in_side);
	/// ���������� ����� ����� ������� ����� (��)
	size_t GetWidthCount(PWLF_TYPE in_type);
	/// ���������� ����� ������� ����� (�������)
	point_t GetLevelPoint(size_t in_index, PWLF_TYPE in_type) const;
	/// ���������� ����� ������� ����� (��������)
	point_t GetRichPoint(size_t in_index, PWLF_TYPE in_type, side_t in_side) const;
	/// ���������� ����� ������� ����� (��)
	point_t GetWidthPoint(size_t in_index, PWLF_TYPE in_type) const;
	/// ����� �������������� �������
	size_t FindMarker(const VWayMarker& in_marker) const;
	/// ���������� ������
	const VWayMarker& GetMarker(size_t in_index) const;
	/// ���������� ��������� ������� � ����
	void DumpMarkers(const std::wstring& in_filename) const;
	///���������� ������� ����������
	VWayCoord GetRealCoord(double in_abscoord) const;
	/// ���������� �������� �� �������
	VSpeeds GetCombinedSpeeds(double in_abscoord) const;
	/// ���������� �������� �� ������� � ������ ������ ������
	VSpeeds GetCurveSpeeds(double in_abscoord) const;
	/// ���������� �������� �� �������
	VSpeeds GetSpeeds(double in_abscoord) const;
	/// ��������� ����������� �������� �� �������
	VSpeeds GetMinSpeeds(double in_start, double in_end) const; 	
 	/// ���������� ������ ������
 	double GetCurveRadius(double in_abscoord) const;
 	/// ���������� ������� �������������� �������
	bool GetChamJoint(double in_abscoord) const;
	/// ��������� ��� ����
	long GetSpala(double in_abscoord) const;
	/// ���������� ������� ����� ��� �������� ����
	bool GetBridge(double in_abscoord) const;
	/// ���������� ������� ������� ��� �������� ����
	bool GetTonnel(double in_abscoord) const;
	/// ���������� ������� ������
	bool IsCurvePlan(double in_abscoord) const;
	/// ���������� ���������� ������
	long GetCurveOrientation(double in_abscoord) const;
	/// ���������� ������� ������ (�������)
	bool IsCurveLevel(double in_abscoord) const;
	/// ���������� ����������� ���� 
	long GetSummaryThreadSide(double in_abscoord) const;
	/// ���������� ������� ���������
	long GetHoleDiameter(double in_abscoord) const;
	/// ���������� ������� ������������ �������
	bool IsControlArea(double in_abscoord) const;
	/// ���������� �� �� �� ���������� �������
	float GetControlAreaWidth(double in_abscoord) const;
	/// ���������� ��� �� �� ���������� �������
	float GetControlAreaWidthSKO(double in_abscoord) const;
	/// ���������� �� ������ �� ����������� �������
	float GetControlAreaLevel(double in_abscoord) const;
	/// ���������� ��� ������ �� ����������� �������
	float GetControlAreaLevelSKO(double in_abscoord) const;
	/// ���������� ���������� �����
	double GetNormaWear(double in_abscoord) const;	
	/// ���������� ��� ������� �����
	ZLINE_TYPE GetCurveStatusPlan(double in_abscoord) const;
	/// ���������� ��� ������� ����� (�������)
	ZLINE_TYPE GetCurveStatusLevel(double in_abscoord) const;
	/// ���������� ����� ��
	long GetPchNum(double in_abscoord) const;
	/// ���������� ��������
	VAprinfoKmRecord GetKm(double in_abscoord) const;
	/// ���������� �������� �����������
	std::string GetDirName() const;
	/// ���������� ��� �����������
	long GetDirCode() const;
	/// ���������� ����� ����
	std::string GetWayCode() const;
	/// ���������� �������
	std::string GetPeregon(double in_abscoord) const;
	/// ���������� ������ ���������� ������
	std::vector<VAprinfoCurve> GetPassportCurveList(double in_start, double in_end) const;
	/// ���������� �����
	VPicket GetPk(double in_abscoord) const;
	/// ���������� ������ ���������
	std::vector<VAprinfoSpeedRecord> GetSpeedList(double in_start, double in_end) const;
	/// ���������� ������ ��������� �����������
	std::vector<VAprinfoSpeedRecord> GetTmpSpeedList(double in_start, double in_end) const;
	/// ���������� ������ ��������� � ���������� �������
	std::vector<VAprinfoPntRecord> GetPntList(double in_start, double in_end) const;
	/// ���������� ������ ��
	std::vector<VAprinfoPchRecord> GetPchList(double in_start, double in_end) const;
	/// ���������� ������ ��������
	std::vector<VAprinfoRepairRecord> GetRepairs(double in_start, double in_end) const;
	/// ���������� ������ ������
	std::vector<VAprinfoWobRecord> GetWobLeftList(double in_start, double in_end) const;
	/// ���������� ������ ������
	std::vector<VAprinfoWobRecord> GetWobRightList(double in_start, double in_end) const;
	/// ���������� ������ �������� ����� �������
	std::vector<VAprinfoRailRecord> GetRailsTypeList(double in_start, double in_end) const;
	/// ���������� ������ ������� �� ���������
	std::vector<VPicket> GetPkList(long in_km) const;
	/// ���������� ������ �������� �������
	std::vector<VAprinfoTestrunRecord> GetTestrunList(double in_start, double in_end) const;
	/// ���������� ������� �������
	bool IsStation(double in_abscoord) const;
	/// ���������� true ���� ��� ������� �50
	bool P50(double in_abscoord) const;
	/// ���������� ��� ������� �������
	MACHINE_TYPE GetTestRun(double in_abscoord) const;
	/// ���������� ������� �����
	bool GetWobLeft(double in_abscoord) const;
	/// ���������� ������� �����
	bool GetWobRight(double in_abscoord) const;
	/// ���������� ��������� ����� ���������
	double GetKmLength(long in_km) const ;
	
	/// ���������� true ���� � ����� ��������� ������� ��� ����������� ���
	bool IsPrj(double in_coord);

};
typedef VSingleton<VSituation> VCurrentSituation;
#define SITUATION_INIT(objects) VCurrentSituation::Instance().SetObject(objects)
#define SITUATION VCurrentSituation::Instance().GetObject()
#define GLOBAL_APRINFO VCurrentSituation::Instance().GetObject().GetAprinfo()

