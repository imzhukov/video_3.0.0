#pragma once

#include <string>
#include <wx/string.h>

#include "parameters.h"
#include "VParameter.h"
#include "VAprinfoRecord.h"
#include "VRemoteFile.h"
#include "markers.h"

/// ��������� ��
class VIBDatabase;

/// ���������� ������ �� ��
class VIBDataHandlerParameter;

/// ������� � ����� ����������
template <class T> class VAprinfoParameter
{
	std::vector<T> data;	
	bool isDefault;

public:

	// ����������� ������� ��� �������� ��������� ������ � �������������� ������

	/// ����� ������
	static bool FitsAlways(const T &)
	{
		return true;
	}

	/// ObjectRecord � ������� - ��������� ���� isThere
	static bool FitsObject(const T & object)
	{
		return object.isThere;
	}

	/// IndexRecord - ������� � �������� �����
	static bool FitsIndex(const T & object)
	{
		return object.value == SZO_OBJECT;
	}

	/// IndexRecord - ������� � �������� �����
	static bool FitsIndexAndZone(const T & object)
	{
		return object.value == SZO_OBJECT || object.value == SZO_SZONE;
	}

	/// KmRecord - ��������� � ��������� ������
	static bool FitsNonZeroKm(const T & object)
	{
		return object.len != 0;
	}
	
	/// �������� ��������
	static bool FitsValidSpeed(const T & object)
	{
		return 
			object.spd.GetPassSpeed().Valid() &&
			object.spd.GetTruckSpeed().Valid() &&
			object.spd.GetEmptySpeed().Valid();
	}

	/// �����������
	VAprinfoParameter() : isDefault(false)
	{}

	/// ���������� ���� "��������� ���������� �� ���������
	void SetDefault(bool __def__)
	{
		this->isDefault = __def__;
	}

	/// ��������� ���� "��������� ���������� �� ���������
	bool GetDefault() const
	{
		return this->isDefault;
	}

	/// ��������������� ������
	void Reserve(size_t limit)
	{
		this->data.reserve(limit);
	}

	/// �������� �������
	void Add(const T & src)
	{
		this->data.push_back(src);
	}

	/// �������� ������ ���������
	void Add(const std::vector<T> & src)
	{
		for (size_t i = 0; i < src.size(); ++i)
			this->Add(src[i]);
	}

	/// ��������� ������� ������� �� ��������� ������
	void SyncRanges(double begin, double end);

	/// ��������� �������, �������� ���������� �����������
	void SyncRanges(double begin, double end, long def_value);

	/// ��������� �������, �������� ���������� ����������� (��� �������� ��������)
	/// FIXME! ��� ����� - ���� ������ ����� ������.
	void SyncRanges(double begin, double end, long def_width, long def_level);

	/// ��������� �������, �������� ���������� �����������
	void SyncRanges(double begin, double end, std::string def_value);

	/// ��������� �������, �������� ���������� ����������� (��� ���������)
	/// FIXME! ��� ����� - ���� ������ ����� ������.
	void SyncSpdRanges(double begin, double end, const VSpeeds & def_spd);

	/// ��������� ������� ���������, ������� true ��� ��������������
	/// � ������
	bool SyncSpdRanges(double begin, double end);

	/// ��������� ������� �������
	void SyncSwitchRanges(double begin, double end);

	/// ��������� ������� ������
	void SyncWobRanges(double begin, double end);

	/// ������� ����� ���������
	size_t GetCount() const
	{
		return this->data.size();
	}

	/// ����������� ������ �� index-��� �������
	const T & GetItem(size_t index) const
	{
		return this->data.at(index); 		
	}

	/// ����������� ������ �� ��������� �������
	const T & GetLastItem() const
	{
		if (data.empty())
			throw VSimpleException(L"������ ����", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		return this->data.back();
	}

	/// ������ �� index-��� �������
	T & GetItem(size_t index) 
	{
		return this->data.at(index);		
	}

	/// ������ �� ��������� �������
	T & GetLastItem() 
	{
		return this->data.back();
	}

	/// ������ ��������, � �������� �������� ����� ����� coord
	size_t FindItem(double coord) const;

	/// ������ ��������� 
	size_t FindKm(long km) const;

	/// ������� ������ (�������)
	std::vector<T> ExportAreas(double from, double to, 
		bool (*Fits)(const T &), bool cut_ranges_to_section) const;

	/// ������� ������ (�����)
	std::vector<T> ExportLabels(double from, double to) const;

	/// ����������
	void Sort()
	{
		std::sort(this->data.begin(), this->data.end());
	}

	/// ��������� ������
	void Unload()
	{
		this->data.clear();
	}
};

/// ��������� ����������
class VAprinfo
{
	bool isLoaded; ///< ������� ������������� ���������
	std::wstring message; ///< ��������� �� ������
	bool warning;

	long dirCode; ///< ��� �����������
	std::string dirName; ///< ��� �����������
	std::string wayName; ///< ��� ����
	VWaySection section; ///< ������� ����
	bool fromDB;	/// �������� ������
	double absCoordBase, absCoordEnd;	/// ������� � ���.�����������

	// ���.���������
	VAprinfoParameter<VAprinfoPchRecord> pchList; ///< ������� ���������
	VAprinfoParameter<VAprinfoKmRecord> kmList; ///< ������������ �����
	VAprinfoParameter<VAprinfoIndexRecord> wayCatList; ///< ��������� ����
	VAprinfoParameter<VAprinfoPntRecord> pntList; ///< ������� � ��������
	VAprinfoParameter<VAprinfoIndexRecord> bridgeList; ///< �����
	VAprinfoParameter<VAprinfoIndexRecord> tonnelList; ///< �������
	VAprinfoParameter<VAprinfoSwitchRecord> switchList; ///< �������

	// ��������
	VAprinfoParameter<VAprinfoSpeedRecord> speedList; ///< �������� �� �������
	VAprinfoParameter<VAprinfoSpeedRecord> speedTmpList; ///< ��������� ��������
	VAprinfoParameter<VAprinfoSpeedRecord> speedProjList; ///< ��������� ��������

	// ���������
	VAprinfoParameter<VAprinfoIndexRecord> nwidthStraightList; ///< ����� �� �� ������
	VAprinfoParameter<VAprinfoIndexRecord> nlevelStraightList; ///< ����� ���������� �� ������
	VAprinfoParameter<VAprinfoIndexRecord> spalaList; ///< ���� ����
	VAprinfoParameter<VAprinfoIndexRecord> holeList; ///< ��������� � �������
	VAprinfoParameter<VAprinfoIndexRecord> threadStraightList;	///< ����.���� �� ������

	// ������
	VAprinfoParameter<VAprinfoBasicRecord> routeList; ///< ������� ��������
	VAprinfoParameter<VAprinfoWobRecord> wobLeftList;	///< ����� ������������ ���� - �����
	VAprinfoParameter<VAprinfoWobRecord> wobRightList;	///< ����� ������������ ���� - ������
	VAprinfoParameter<VAprinfoProfLabel> profList; ///< ����� �������
	VAprinfoParameter<VAprinfoCoordPointRecord> coordPointList; //< �������� �����
	VAprinfoParameter<VAprinfoControlAreaRecord> controlAreaList; //< ����������� �������
	VAprinfoParameter<VAprinfoRepairRecord> repairList; //< �������
	VAprinfoParameter<VAprinfoCrossing> crossingList; ///< ��������
	VAprinfoParameter<VAprinfoObjectRecord> chamJointList; ///< ������������� �������
	VAprinfoParameter<VAprinfoTestrunRecord> testrunList; //< ������� ������� ����

	// ������ � ������� �����
	struct CurvePack
	{
		VAprinfoParameter<VAprinfoCurve> curveList; ///< ������
		VAprinfoParameter<VAprinfoZLRecord> zlLevelList; ///< ������� ����� �� ������
		VAprinfoParameter<VAprinfoZLRecord> zlPlanList; ///< ������� ����� �� ����� (��������)
		VAprinfoParameter<VAprinfoZLRecord> zlRichList; ///< ������� ����� �� ����� (��������)
		VAprinfoParameter<VAprinfoWidthRecord> widthList; ///< ����� ��+�����
		VAprinfoParameter<VAprinfoIndexRecord> threadList; ///< ����������� ����

		/// ���������� ������� �����
		std::vector<std::wstring> BuildZL(double absStart, double absEnd); 

		/// �������� ������ �� ������
		void Unload();

		// �������� �� ���������
		void SetDefaults(double absStart, double absEnd);

		VAprinfo * aprInfo;
	}
	crvFact, crvPassport;

	/// ����� �������������� � ������ - ������� �������
	VAprinfoParameter<VAprinfoIndexRecord> chrouteList; 

	/// ��� �������
	VAprinfoParameter<VAprinfoRailRecord> railsType;
	
	/// ��� ����������
	VAprinfoParameter<VAprinfoBraceRecord> railsBrace;
	
	/// ������� �������
	VWayMarkerList markers;

	/// ������ ����� ������, ������������ ��� ������ �������
	uint32_t CalcBlockSize() const;

	/// ������ �� �����
	bool Load(const VBasicParameter & data);

public:

	/// �����������
	VAprinfo() : isLoaded(false), warning(false), 
		message(ERRMSG_DATA_NOT_LOADED), dirCode(0), fromDB(false)
	{
		ZeroMemory(&this->section, sizeof(this->section));
		this->crvFact.aprInfo = this;
		this->crvPassport.aprInfo = this;
	}

	/// ��������� �� ������
	const std::wstring & GetErrorMessage() const
	{
		return this->message;
	}

	// ------------------------------------------------------------------
	// �������� � �������� ������
	// ------------------------------------------------------------------

	/// ��������
	template <class FILETYPE> bool Load(FILETYPE & file, uint32_t type);

	/// �������� �� �� ���������� ��� �����������
	bool Makeinfo(VIBDatabase & db, 
		long dirCode, const std::string & wayName, const VWaySection & section, 
		const VTime & rideDate, bool repairs, bool mea_curves);

	bool GetWarning() const
	{
		return this->warning;
	}

	/// ������� ���������� ������ �� �������
	bool ReplaceMeasuredCurves(const std::vector<VAprinfoCurve> & curves);

	/// ���������� �� �� ��������� ��� ������� - �������
	bool UpdateStTrailInfo(VIBDatabase & db, const std::wstring & fileName);

	/// �������� �� �� ���������� ��� ������� - �� ����
	bool MakeStWayInfo(VIBDatabase & db, 
		int stCode, const std::string & parkName, const std::string & wayName,
		const std::wstring & fileName);

	/// �������� �� �� ���������� ��� ������� - �� ���� - �� ������� ������������� �� 
	bool MakeStWayInfo(VIBDatabase & db, 
		int stCode, const std::string & parkName, const std::string & wayName, 
		const std::vector<VAprinfoKmRecord> & nstKm, ///< ���������� ������ ����� � �����
		const std::wstring & fileName);

	/// ����������� APRINFO "�� ���������"
	bool MakeStWayInfo(double begin, double end, int startKm = 0);

	/// ������ ���������
	bool IsLoaded() const
	{
		return this->isLoaded;
	}

	/// ���������� ���� ��������
	void SetLoaded(bool value)
	{
		this->isLoaded = value;
	}

	/// ���������� � VBasicParameter
	bool Save(VBasicParameter & prm, uint32_t type);

	/// ���������� � ����
	bool Save(VMainFile & file, uint32_t type);

	/// �������� ������
	void Unload();

	/// ���������� ������������� ������ �����������
	void SetDefaults();

	/// ����� � ���������� �������
	std::string GetReport(bool addHtmlTag, bool fullReport) const;

	/// �������� ������
	bool IsFromDB() const
	{
		return this->fromDB;
	}

	/// ������ �� ������������� ����� (�� c4f!!!!)
	bool DumpLoadFromFile(const VFileName & fn);

	/// ���������� � ������������ ���� (�� c4f!!!!)
	bool DumpSaveToFile(const VFileName & fn);

	//------------------------------------------------------------------
	// ����������� � ����
	//------------------------------------------------------------------

	///���������� ��� �����������
	long GetDirCode() const
	{
		return dirCode;
	};

	///���������� ����� ����
	std::string GetWayCode() const
	{
		return wayName;
	};

	///���������� ��� �����������
	std::string GetDirName() const
	{
		return this->dirName;
	};

	// ------------------------------------------------------------------
	// ���������
	// ------------------------------------------------------------------
	
	/// ���. �-�� ������ ������� ����
	double GetAbsCoordBase() const 
	{ 
		return this->absCoordBase;
	}

	/// ���. �-�� ����� ������� ����
	double GetAbsCoordEnd() const
	{ 
		return this->absCoordEnd;
	}

	/// ����� ����� �� ����
	bool FitsWay(double coord) const
	{
		return coord >= this->GetAbsCoordBase() && coord <= this->GetAbsCoordEnd();
	}

	/// ������ �������� �� ������� ����
	long GetFirstKm() const
	{
		return this->kmList.GetItem(0).km;
	}

	/// ����� ���������� �� ������� ����
	size_t GetKmCount() const
	{
		return this->kmList.GetCount();
	}

	/// ����������� ��������� ������������ �����
	long GetKmDirection() const;

	/// �������� ������� ��������� �� ������� ����
	bool CheckKm(long km) const;

	/// ����� ���������
	size_t FindKm(long km) const;

	/// ������� ������ � ��������� - ������� ������� �� ��������� �� �����
	VAprinfoKmRecord GetKm(long km) const;

	/// ����� ���������
	float GetKmLength(long km) const;

	/// ������ ���������� �� �������
	std::vector<VAprinfoKmRecord> GetKmList(double from, double to, bool cut_ranges_to_section) const;

	/// ������ ���������� �� ����
	std::vector<VAprinfoKmRecord> GetKmList() const;

	/// �������������� ������� �-�� � ����������
	double GetAbsCoord(const VWayCoord & in_coord) const;

	/// �������������� ���������� �-�� � �������
	VWayCoord GetRealCoord(double coord) const;

	/// ������ ������� �� ���������
	std::vector<VPicket> GetPkList(long km) const;

	/// ������ ������� �� �������
	std::vector<VPicket> GetPkList(double from, double to, bool cut_ranges_to_section) const;

	/// �� �� ����������
	VPicket GetPk(double coord) const;

	// ------------------------------------------------------------------
	// �����
	// ------------------------------------------------------------------

	/// ��������� ����� �������������� � ������
	void SetChrouteWarningPoint(double coord);

	/// ������ ������� ������
	/// �� ������ ������� �� ����� warning ����� ������ ��������
	long GetChrouteIndexValue(double coord) const;

	// ------------------------------------------------------------------
	// ����� �� �� ������
	// ------------------------------------------------------------------

	/// ������ ���� �� �� �������
	std::vector<VAprinfoIndexRecord> GetStraightNormaWidthList(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ ���� �� �� ����
	std::vector<VAprinfoIndexRecord> GetStraightNormaWidthList() const;

	/// ����� �� �� ������ � ����� 
	long GetStraightNormaWidth(double coord) const;

	// ------------------------------------------------------------------
	// ����� ����������
	// ------------------------------------------------------------------

	/// ������ ���� ���������� �� �������
	std::vector<VAprinfoIndexRecord> GetStraightNormaLevelList(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ ���� ���������� �� ����
	std::vector<VAprinfoIndexRecord> GetStraightNormaLevelList() const;

	/// ����� ���������� �� ������ � �����
	long GetStraightNormaLevel(double coord) const;

	// ------------------------------------------------------------------
	// �� � ���������� �� ����������� ��������
	// ------------------------------------------------------------------

	bool IsControlArea(double coord) const;
	float GetControlAreaWidth(double coord) const;
	float GetControlAreaWidthSKO(double coord) const;
	float GetControlAreaLevel(double coord) const;
	float GetControlAreaLevelSKO(double coord) const;

	/// �� � ���������� �� ����������� �������� �� �� ����
	std::vector<VAprinfoControlAreaRecord> GetControlArea() const;

	// ------------------------------------------------------------------
	// �����
	// ------------------------------------------------------------------

	/// ������ ������ ������������ ���� �� ������� (�����)
	std::vector<VAprinfoWobRecord> GetWobLeftList(double from, double to) const;

	/// ������ ������ ������������ ���� �� ���� (�����)
	std::vector<VAprinfoWobRecord> GetWobLeftList() const;

	/// ������ ������ ������������ ���� �� ������� (������)
	std::vector<VAprinfoWobRecord> GetWobRightList(double from, double to) const;

	/// ������ ������ ������������ ���� �� ���� (������)
	std::vector<VAprinfoWobRecord> GetWobRightList() const;

	/// ������� ����� �� ����� ���� � �����
	bool GetWobLeft(double coord) const;

	/// ����� ����� �� ����� ���� � �����
	std::string GetWobLeftNum(double coord) const;

	/// ������� ����� �� ������ ���� � �����
	bool GetWobRight(double coord) const;

	/// ����� ����� �� ������ ���� � �����
	std::string GetWobRightNum(double coord) const;


	// ------------------------------------------------------------------
	// ������� � ��������
	// ------------------------------------------------------------------

	/// ������ ������� � ��������� �� �������
	std::vector<VAprinfoPntRecord> GetPntList(double from, double to) const;

	/// ������ ������� � ��������� �� ����
	std::vector<VAprinfoPntRecord> GetPntList() const;

	/// ������ ������� �� �������
	std::vector<VAprinfoPntRecord> GetStationList(double from, double to) const;

	/// ������ ������� �� ����
	std::vector<VAprinfoPntRecord> GetStationList() const;

	/// ��� �� � �����
	std::string GetPchName(double coord) const;

	/// �������� ��������
	std::string GetPeregon(double coord, int direction = 1) const;

	/// �������� �������� �� �������
	std::string GetAreaPeregon(double begin, double end) const;

	/// ��� ������� (��� ��������� ����� 0)
	long GetStationCode(double coord) const;

	// ------------------------------------------------------------------
	// ���������� ������ � ����� � �� ������
	// ------------------------------------------------------------------

	/// ������ ������ �� �������
	std::vector<VAprinfoCurve> GetPassportCurveList(double from, double to) const;

	/// ������ ������ �� ����
	std::vector<VAprinfoCurve> GetPassportCurveList() const;

	/// ������� ������ (���� ������� ���, ����� exception)
	const VAprinfoCrvRadius & GetPassportCurveRadiusRecord(double coord) const;

	/// ���������� ������ � �����
	long GetPassportCurveOrientation(double coord) const;

	/// ������ �������� ������� - �� �����
	ZLINE_TYPE GetPassportCurveStatusPlan(double coord) const;

	/// ������� ������ � ������� ����� - �� �����
	bool IsPassportCurvePlan(double coord) const;

	/// ������ �������� ������� - �� ������
	ZLINE_TYPE GetPassportCurveStatusLevel(double coord) const;

	/// ������� ������ � ������� ����� - �� ������
	bool IsPassportCurveLevel(double coord) const;

	/// id ������ � �����
	__int64 GetPassportCurveId(double coord) const;

	/// ������
	float GetPassportCurveRadius(double coord) const;

	/// ���������� � ������ ������
	float GetPassportCurveNormaLevel(double coord) const;

	/// �������� � ������ ������
	float GetPassportCurveNormaCurve(double coord) const;

	/// �������� � ������ ������
	float GetPassportCurveNormaRich(double coord) const;

	/// ����� �� ��� ��������
	float GetPassportCurveNormaWidthPlus(double coord) const;

	/// ����� �� ��� �������
	float GetPassportCurveNormaWidthMinus(double coord) const;

	/// ����������� ����
	long GetPassportCurveThreadSide(double coord) const;

	/// ������ ������� ����� �� ����� (��������)
	std::vector<VAprinfoZLRecord> GetPassportCurvePlanZLList(double from, double to) const;

	/// ������ ������� ����� �� ����� (��������)
	std::vector<VAprinfoZLRecord> GetPassportCurvePlanZLList() const;

	/// ������ ������� ����� �� ����� (��������)
	std::vector<VAprinfoZLRecord> GetPassportCurveRichZLList(double from, double to) const;

	/// ������ ������� ����� �� ����� (��������)
	std::vector<VAprinfoZLRecord> GetPassportCurveRichZLList() const;

	/// ������ ������� ����� �� ������, �� �������
	std::vector<VAprinfoZLRecord> GetPassportCurveLevelZLList(double from, double to) const;

	/// ������ ������� ����� �� ������, �� ����
	std::vector<VAprinfoZLRecord> GetPassportCurveLevelZLList() const;

	/// ������ ���� �� � ������ �� �������
	std::vector<VAprinfoWidthRecord> GetPassportCurveWidthList(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ ���� �� � ������ �� ����
	std::vector<VAprinfoWidthRecord> GetPassportCurveWidthList() const;

	/// ������ ����������� ����� � ������ ������ �� �������
	std::vector<VAprinfoIndexRecord> GetPassportCurveStraightList(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ ����������� ����� � ������ ������ �� ����
	std::vector<VAprinfoIndexRecord> GetPassportCurveStraightList() const;

	/// ���������� ����������� �� ��������� � ������������� ��������� � ������ ������
	VSpeeds GetPassportCurveSpeeds(double coord) const;

	/// ����� �� � ����� 
	float GetPassportCurveNormaWidth(double coord) const;

	/// ����� ������ � �����
	float GetPassportCurveNormaWear(double coord) const;

	/// ��������� ����������� ���� � ������ ���������� ������
	long GetPassportCurveSummaryThreadSide(double coord) const;

	// ------------------------------------------------------------------
	// ����������� ������ � ����� � �� ������
	// ------------------------------------------------------------------

	/// ������ ������ �� �������
	std::vector<VAprinfoCurve> GetFactCurveList(double from, double to) const;

	/// ������ ������ �� ����
	std::vector<VAprinfoCurve> GetFactCurveList() const;

	/// ������� ������ (���� ������� ���, ����� exception)
	const VAprinfoCrvRadius & GetFactCurveRadiusRecord(double coord) const;

	/// ���������� ������ � �����
	long GetFactCurveOrientation(double coord) const;

	/// ������ �������� ������� - �� �����
	ZLINE_TYPE GetFactCurveStatusPlan(double coord) const;

	/// ������� ������ � ������� ����� - �� �����
	bool IsFactCurvePlan(double coord) const;

	/// ������ �������� ������� - �� ������
	ZLINE_TYPE GetFactCurveStatusLevel(double coord) const;

	/// ������� ������ � ������� ����� - �� ������
	bool IsFactCurveLevel(double coord) const;

	/// id ������ � �����
	__int64 GetFactCurveId(double coord) const;

	/// ������
	float GetFactCurveRadius(double coord) const;

	/// ���������� � ������ ������
	float GetFactCurveNormaLevel(double coord) const;

	/// �������� � ������ ������
	float GetFactCurveNormaCurve(double coord) const;

	/// �������� � ������ ������
	float GetFactCurveNormaRich(double coord) const;

	/// ����� �� ��� ��������
	float GetFactCurveNormaWidthPlus(double coord) const;

	/// ����� �� ��� �������
	float GetFactCurveNormaWidthMinus(double coord) const;

	/// ����������� ����
	long GetFactCurveThreadSide(double coord) const;

	/// ������ ������� ����� �� ����� (��������)
	std::vector<VAprinfoZLRecord> GetFactCurvePlanZLList(double from, double to) const;

	/// ������ ������� ����� �� ����� (��������)
	std::vector<VAprinfoZLRecord> GetFactCurvePlanZLList() const;

	/// ������ ������� ����� �� ����� (��������)
	std::vector<VAprinfoZLRecord> GetFactCurveRichZLList(double from, double to) const;

	/// ������ ������� ����� �� ����� (��������)
	std::vector<VAprinfoZLRecord> GetFactCurveRichZLList() const;

	/// ������ ������� ����� �� ������, �� �������
	std::vector<VAprinfoZLRecord> GetFactCurveLevelZLList(double from, double to) const;

	/// ������ ������� ����� �� ������, �� ����
	std::vector<VAprinfoZLRecord> GetFactCurveLevelZLList() const;

	/// ������ ���� �� � ������ �� �������
	std::vector<VAprinfoWidthRecord> GetFactCurveWidthList(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ ���� �� � ������ �� ����
	std::vector<VAprinfoWidthRecord> GetFactCurveWidthList() const;

	/// ������ ����������� ����� � ������ ������ �� �������
	std::vector<VAprinfoIndexRecord> GetFactCurveStraightList(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ ����������� ����� � ������ ������ �� ����
	std::vector<VAprinfoIndexRecord> GetFactCurveStraightList() const;

	/// ���������� ����������� �� ��������� � ������������� ��������� � ������ ������
	VSpeeds GetFactCurveSpeeds(double coord) const;

	/// ����� �� � ����� 
	float GetFactCurveNormaWidth(double coord) const;

	/// ����� ������ � �����
	float GetFactCurveNormaWear(double coord) const;

	/// ��������� ����������� ���� � ������ ���������� ������
	long GetFactCurveSummaryThreadSide(double coord) const;

	// ------------------------------------------------------------------
	// ������� ��
	// ------------------------------------------------------------------

	/// ������ �������� �� �� �������
	std::vector<VAprinfoPchRecord> GetPchList(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ �������� �� �� ����
	std::vector<VAprinfoPchRecord> GetPchList() const;

	/// ����� �� � �����
	long GetPchNum(double coord) const;

	// ------------------------------------------------------------------
	// �������� �� �������
	// ------------------------------------------------------------------

	/// ������ ��������� �� ������� �� �������
	std::vector<VAprinfoSpeedRecord> GetSpeedList(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ ��������� �� ������� �� ����
	std::vector<VAprinfoSpeedRecord> GetSpeedList() const;
	
	/// ������ ��������� ��������� �� ������� �� �������
	std::vector<VAprinfoSpeedRecord> GetTmpSpeedList(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ ��������� ��������� �� ������� �� ����
	std::vector<VAprinfoSpeedRecord> GetTmpSpeedList() const;

	/// ������� ������ ��������� �� ����
	bool ReplaceSpeedList(const std::vector<VAprinfoSpeedRecord> & spd); 

	/// �������� � �����
	VSpeeds GetSpeeds(double coord) const;
	
	/// ��������� �������� � �����
	VSpeeds GetTmpSpeeds(double coord) const;
	
	/// ���������� ����������� �� ��������� � ������������� ���������
	VSpeeds GetCombinedSpeeds(double coord) const;
	
	/// ������������ �������� � �����
	speed_t GetPassSpeed(double coord) const;

	/// �������� �������� � �����
	speed_t GetTruckSpeed(double coord) const;

	/// ������ �������� � �����
	speed_t GetThirdSpeed(double coord) const;

	/// �������� ����������� ��������
	speed_t GetHighSpeed(double coord) const;

	/// �������� ����������� �������� � �������� ������
	speed_t GetHighSpeedBT(double coord) const;

	/// ������������ �������� � �����
	speed_t GetTempPassSpeed(double coord) const;

	/// �������� �������� � �����
	speed_t GetTempTruckSpeed(double coord) const;

	/// ������ �������� � �����
	speed_t GetTempThirdSpeed(double coord) const;

	// ����������� ������������ �������� �� �������
	VSpeeds GetMinSpeeds(double from, double to) const;
	
	// ����������� ��������� ����������� �������� �� �������
	VSpeeds GetMinTmpSpeeds(double from, double to) const;

	/// ������������ ������������ �������� �� �������
	speed_t GetMaxPassSpeed(double from, double to) const;

	/// ����������� ������������ �������� �� �������
	speed_t GetMinPassSpeed(double from, double to) const;

	/// ������������ �������� �������� �� �������
	speed_t GetMaxTruckSpeed(double from, double to) const;

	/// ����������� �������� �������� �� �������
	speed_t GetMinTruckSpeed(double from, double to) const;

	/// ������������ ������ �������� �� �������
	speed_t GetMaxThirdSpeed(double from, double to) const;

	/// ����������� ������ �������� �� �������
	speed_t GetMinThirdSpeed(double from, double to) const;

	/// ����������� �������� ��� ����������� ��������
	speed_t GetMinHighSpeed(double from, double to) const;

	/// ����������� �������� ��� ����������� �������� � �������� ������
	speed_t GetMinHighSpeedBT(double from, double to) const;

	/// ������������ �������� ��� ����������� ��������
	speed_t GetMaxHighSpeed(double from, double to) const;

	/// ������������ �������� ��� ����������� �������� � �������� ������
	speed_t GetMaxHighSpeedBT(double from, double to) const;

	speed_t GetMinPassTmpSpeed(double from, double to) const;
	speed_t GetMinTruckTmpSpeed(double from, double to) const;
	speed_t GetMinThirdTmpSpeed(double from, double to) const;

	/// ������ ��������� �������� ���������� "�� ���������"
	bool DefaultSpeeds() const
	{
		return this->speedList.GetDefault();
	}

	// ------------------------------------------------------------------
	// ���� ����
	// ------------------------------------------------------------------

	/// ������ ����� ���� �� �������
	std::vector<VAprinfoIndexRecord> GetSpalaList(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ ����� ���� �� ����
	std::vector<VAprinfoIndexRecord> GetSpalaList() const;

	/// ��� ���� � �����
	long GetSpala(double coord) const;

	// ------------------------------------------------------------------
	// ��������� � ������
	// ------------------------------------------------------------------

	/// ������� ��������� �� �������
	std::vector<VAprinfoIndexRecord> GetHoleDiameterList(double from, double to, bool cut_ranges_to_section = true) const;
	
	/// ������� ��������� �� ����
	std::vector<VAprinfoIndexRecord> GetHoleDiameterList() const;

	/// ������� ��������� � �����
	long GetHoleDiameter(double coord) const;

	// ------------------------------------------------------------------
	// ��������� ����
	// ------------------------------------------------------------------

	/// ��������� ���� � �����
	long GetWayCat(double coord) const;

	/// ��������� ���� �� ������� (������� - ����� �������)
	long GetWayCat(double from, double to) const;

	/// ������� ��������� ����
	bool ReplaceWayCatList(const std::vector<VAprinfoIndexRecord> & wc); 

	// ------------------------------------------------------------------
	// �������
	// ------------------------------------------------------------------

	/// ������ ������� �� �������
	std::vector<VAprinfoSwitchRecord> GetSwitchList(double from, double to) const;

	/// ������ ������� �� ����
	std::vector<VAprinfoSwitchRecord> GetSwitchList() const;

	/// ���������� �� �������
	bool GetSwitch(double coord) const;

	/// ���������� �������
	SWITCH_DIR GetSwitchDir(double coord) const;

	/// �������� ���������� �������
	static std::wstring GetSwitchDirDescription(SWITCH_DIR dir);

	// ------------------------------------------------------------------
	// �����
	// ------------------------------------------------------------------

	/// ������ ������ �� �������
	std::vector<VAprinfoIndexRecord> GetBridgeList(double from, double to, bool include_areas) const;

	/// ������ ������ �� ���� �������
	std::vector<VAprinfoIndexRecord> GetBridgeList(bool include_areas) const;

	/// ��������� �� ����� (����/���.����/�� ����)
	long GetBridge(double coord) const;

	/// ����� �����
	double GetBridgeLength(double coord) const;

	// ------------------------------------------------------------------
	// �������
	// ------------------------------------------------------------------

	/// ������ �������� �� �������
	std::vector<VAprinfoIndexRecord> GetTonnelList(double from, double to, bool include_areas) const;

	/// ������ �������� �� ���� �������
	std::vector<VAprinfoIndexRecord> GetTonnelList(bool include_areas) const;

	/// ��������� � ������� (����/���.����/�� ����)
	long GetTonnel(double coord) const;

	/// ����� �������
	double GetTonnelLength(double coord) const;

	// ------------------------------------------------------------------
	// ����� �������
	// ------------------------------------------------------------------

	/// ������ ���������� ����� ������� �� �������
	std::vector<VAprinfoProfLabel> GetProfLabels(double from, double to) const;

	/// ������ ���������� ����� ������� �� ����
	std::vector<VAprinfoProfLabel> GetProfLabels() const;

	// ------------------------------------------------------------------
	// ����������� ���� �� ������
	// ------------------------------------------------------------------

	/// ������ ����������� ����� �� �������
	std::vector<VAprinfoIndexRecord> GetStraightThreadSideList(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ ����������� ����� �� ���� �������
	std::vector<VAprinfoIndexRecord> GetStraightThreadSideList() const;

	/// ����������� ���� � �����
	long GetStraightThreadSide(double coord) const;

	// ------------------------------------------------------------------
	//�������� �����
	// ------------------------------------------------------------------
	double GetCoordPoint(int type, wxString id);

	// ------------------------------------------------------------------
	// �������
	// ------------------------------------------------------------------

	/// ������ �������� �� �������
	std::vector<VAprinfoRepairRecord> GetRepairs(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ �������� �� ���� �������
	std::vector<VAprinfoRepairRecord> GetRepairs() const;

	// ------------------------------------------------------------------
	// ������� �������
	// ------------------------------------------------------------------

	/// ������ �������� �� �������
	std::vector<VAprinfoTestrunRecord> GetTestrunList(double from, double to) const;

	/// ������ �������� �� ��� �������
	std::vector<VAprinfoTestrunRecord> GetTestrunList() const;

	/// ������� � �����
	MACHINE_TYPE GetTestrunType(double coord) const;

	/// �������� ������� � �����
	VSpeeds GetTestrunSpeed(double coord) const;

	// ------------------------------------------------------------------
	// ���� ������� � ����������
	// ------------------------------------------------------------------

	/// ��� ������� � �����
	long GetRailsType(double coord) const;

	/// �������� ���� ������� � �����
	std::string GetRailsTypeDescr(double coord) const;

	/// ������ ����� ������� �� �������
	std::vector<VAprinfoRailRecord> GetRailsTypeList(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ ����� ������� �� ��� �������
	std::vector<VAprinfoRailRecord> GetRailsTypeList() const;

	/// ��� ���������� ������� � �����
	long GetRailsBrace(double coord) const;

	/// �������� ���� ���������� ������� � �����
	std::string GetRailsBraceDescr(double coord) const;

	/// ������ ���������� ������� �� �������
	std::vector<VAprinfoBraceRecord> GetRailsBraceList(double from, double to, bool cut_ranges_to_section = true) const;

	/// ������ ���������� ������� �� ��� �������
	std::vector<VAprinfoBraceRecord> GetRailsBraceList() const;

	// ------------------------------------------------------------------
	// ��������
	// ------------------------------------------------------------------

	/// ������ ��������� �� �������
	std::vector<VAprinfoCrossing> GetCrossings(double from, double to) const;

	/// ������ ��������� �� ��� �������
	std::vector<VAprinfoCrossing> GetCrossings() const;

	// ------------------------------------------------------------------
	// ������������� �������
	// ------------------------------------------------------------------

	/// ������� �������������� ������� � �����
	bool GetChamJoint(double coord) const;

	/// ������ ������������� �������� �� �������
	std::vector<VAprinfoObjectRecord> GetChamJoints(double from, double to) const;

	/// ������ ������������� �������� �� ��� �������
	std::vector<VAprinfoObjectRecord> GetChamJoints() const;
	
	// ------------------------------------------------------------------
	// ������� �� ����
	// ------------------------------------------------------------------

    /// ������� ������� 
    const VWayMarkerList & GetMarkers() const
    {
        return this->markers;
    }

	// ------------------------------------------------------------------
	// ��������� ������� �����
	// ------------------------------------------------------------------

	/// ����� � ����������� ������� ����� �� ����� �� ���������� � ���������� ������
	std::wstring CompareZL(int km) const;

	/// ��������� ����� � ����������� ������� ����� �� ����� �� ���������� � ���������� ������
	/// (� ���)
	void CompareZL() const;

};

typedef boost::shared_ptr<VAprinfo> VAprinfoPtr;

template <class FILETYPE>
bool VAprinfo::Load(FILETYPE & file, uint32_t type)
{
	if (this->IsLoaded()) return true;
	this->Unload();

	VBasicParameter data;
	if (!data.Load(file, type, false)) 
	{
		this->message = data.GetErrorMessage();
		return false;
	}

	bool result = this->Load(data);
	data.UnloadFile(file, type);
	data.Unload();
	return result;
}
