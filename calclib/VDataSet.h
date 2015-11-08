#pragma once

#include <set>
#include <map>
#include <ostream>

//boost includes
#include <boost/shared_ptr.hpp>

#include "parameters.h"
#include "VAprinfo.h"
#include "VParameter.h"
#include "VCorrupt.h"
#include "VCurveList.h"
#include "VOverstep.h"
#include "VProfile.h"
#include "VOpMark.h"
#include "VZazor.h"
#include "VSidewayList.h"
#include "VControlArea.h"
#include "VWear.h"
#include "VStatistics.h"
#include "VPlan.h"
#include "VPlanElemList.h"
#include "VHistory.h"
#include "VComment.h"
#include "VUklon.h"
#include "VPer.h"
#include "VPerLine.h"
#include "VCoordPoint.h"
#include "VFileName.h"
#include "VRemoteFile.h"
#include "VLineList.h"
#include "oversteps.h"
#include "VDiffWidth.h"
#include "canting.h"
#include "aui.h"
#include "VPrNgrAreas.h"
#include "rtparam.h"
#include "markers.h"

class VIBDatabase;
class VProgressBox;

/// ��������� ������� ����� ��� ���������� � CSV
enum SIDEWAYS_PROCESSING
{
	SIDEWAYS_KEEP,		///< ��������� ��� ���������
	SIDEWAYS_ZERO,		///< ��������
	SIDEWAYS_REMOVE		///< �������
};

/// ����������������� ����� ������ �� �����
class VDataSet
{
	VMainFile file;						///< ���� ����������
	VRemoteFile remote;					///< ��������� ���� ����������
	int open_mode;						///< �����, � ������� ������ ����
	VFileName fn;						///< ��� �����
	VMainFileHeader header;				///< ��������� �����

	VAbsCoordData absCoord;				///< ���.����������
	VCorruptData corrupt;				///< ���� ���������
	VSpeedData speed;					///< �������� ������
	VAprinfo aprInfo, fileAprinfo;		///< ������� � ����������� ��������� ����������
	VComment comment;					///< ����������� � �����

	VOvr2List ovrs2;					///< ����������� ������2
	VOvr3List ovrs3;					///< ����������� ������3
	VOpMarkList opMarks;				///< ����� ���������
	VMagZazorList magZazor;				///< ��������� ������
	VZazorList zazorLeft, zazorRight;	///< ���������� ������
	VZazorList magZazorLeft, magZazorRight; ///<��������� ������ � ���������
	VUgonList ugonLeft, ugonRight;		///< �����
	VSpalaMarkerList spalaMarker;		///< ����� ����
	VCoordPointAutoCorr coordPoint;		///< ������� ����

	// ���������� ���������, ������������ ��� VParameter
	std::map<PARAMETER_TYPE, VParameterPtr> params;

	VParameter midLevel;				///< ��. ������� 
	VParameter midCurv;					///< ��. �������� 
	VParameter midWidth;				///< ��. ��
	VParameter midRichLeft;				///< ��. ����� �������� 
	VParameter midRichRight;			///< ��. ������ �������� 

	VParameter curv_from_kurs;			///< �������� �� �����

	VDoubleParameter gps_time;			///< GPS: ����� 
	VDoubleParameter gps_latitude;		///< GPS: ������ 
	VDoubleParameter gps_longitude;		///< GPS: �������
	VDoubleParameter gps_height;		///< GPS: ����������

	VUlongParameter dpp;				///< ������� ��������� DPP

	VCurveList curves, modelCurves;		///< ������ � ��������� ������
	VLineList lines;					///< ������
	VLineMarkList lineMarks;			///< ����� ������
	VProfileMarkList profMarks;			///< ����� �������
	VSidewayList sideways;				///< ������� ����
	VControlAreaList ctrlAreas;			///< ����������� �������
	VHistory history;					///< ������� ��������
	
	VProfile profile;					///< �������
	VDiffProfile diff_profile;			///< ���������� �� ���������� �������
	VAproxProfile aprox_profile;		///< ����������� �������
	VJaggyProfile jaggy_profile;		///< ���������� �������
	VSlopeList profile_slope_element;   ///< ����� ������� �� ���������
	VSlopeList profile_slope_km;		///< ����� ������� �� ����������
	VSlopeList profile_slope_pk;		///< ����� ������� �� �������
	VSlopeList profile_slope_hpk;		///< ����� ������� �� �����������

	VWear wear;							///< ������������ �����
	VStatistics statistic;				///< ��������� ��� ������� �������������� ������
	VPlan plan;							///< ���� ����	
	VIdealPlanReal ideal_plan;			///< "���������" ����
	VIdealPlanModel model_ideal_plan;	///< ��������� "���������" ����
	VPlanElemList planElem;				///< ������ ��������� �����
	VPlanElemList  modelPlanElem;		///< ��������� ������ ��������� �����	

	VUklonList uklon;					///< ������������ ��������� ������
	VPerKuzList perkuz;					///< �������� �� ���� ������
	VPerTelList pertel;					///< �������� �� ���� �������	
	VPerLineList perline;               ///< ���������� � ������

	VPlanOffsetReal plan_offset_fictional;///< ���������� ����� �� ���������� ��� ����� �������� ������

	VPlanOffsetReal	plan_offset;		///< ���������� ����� �� ����������	
	VPlanOffsetModel model_plan_offset;	///< ���������� ����� �� ����������	(����.)	
	VJaggyPlan jaggy_plan;		        ///< ���������� �����
	VJaggyPlan jaggy_plan_on_lines;		///< ���������� ����� �� ������
	VZLine zl;							///< ������� ����� �� ������

	VAnpReal anp_real_high;				///< ������������ ��������� ����������� ��� ���������� �������
	VAnpReal anp_real_high_bt;			///< ������������ ��������� ����������� ��� ���������� ������� � �������� ������
	VAnpReal anp_real_pass;				///< ������������ ��������� ����������� ��� ������������ �������
	VAnpReal anp_real_truck;			///< ������������ ��������� ����������� ��� �������� �������
	
	VAnpModel anp_model_high;			///< ������������ ��������� ��������� ��� ���������� �������
	VAnpModel anp_model_high_bt;		///< ������������ ��������� ��������� ��� ���������� ������� � �������� ������
	VAnpModel anp_model_pass;			///< ������������ ��������� ��������� ��� ������������ �������
	VAnpModel anp_model_truck;			///< ������������ ��������� ��������� ��� �������� �������

	VPsiReal psi_real_high;				///< �������� ���������� ������������� ��������� (����. ����.)
	VPsiReal psi_real_high_bt;			///< �������� ���������� ������������� ��������� (����. ����. � ��)
	VPsiReal psi_real_pass;				///< �������� ���������� ������������� ��������� (����. ����.)
	VPsiReal psi_real_truck;			///< �������� ���������� ������������� ��������� (����. ����.)
	
	VDiffLevel diff_level;				///< ���������� ������� �� ��������
	VDiffLevel diff_level_model;		///< ���������� ������� �� �������� ������

	VDiffWidth width_diff_front;				///< �������� �� � ��������� � ���
	VDiffWidth width_diff_back;				///< �������� �� � ��������� � ���

	VJaggyProfileOverstepList jaggy_profile_overstep; ///< ���������������� ���������� �������
	VJaggyPlanOverstepList jaggy_plan_overstep; ///< ���������������� ���������� �����

	VDiffWidthOverstepList diff_width_front_overstep; ///< �������
	VDiffWidthOverstepList diff_width_back_overstep; ///< �������
	
	VCantingOverstepList canting_left_overstep; ///< ����� ����������
	VCantingOverstepList canting_right_overstep; ///< ������ ����������
	
	VExtWearOverstepList ext_wear_left_overstep; ///< ����� ����� ��������� �����
	VExtWearOverstepList ext_wear_right_overstep; ///< ������ ����� ��������� �����

	VPrNgrAreas prNgrAreas; ///< ������� ��/���
	
	VWayMarkerList markers; ///< �������

	std::map<PARAMETER_TYPE, bool> piList;	///< ������ ����������� ����������
	std::set<PARAMETER_TYPE> changed;	///< ���������� ���������
	std::wstring message;				///< ��������� �� ������

	std::map<std::wstring, std::wstring> options;///< �����

	/// �������� ������������ ��� VParameter
	static bool IsParameter(PARAMETER_TYPE param); 

	/// �������� ����������� �� �����
	static bool IsFileParameter(PARAMETER_TYPE param);

	/// �������� ����������� �� ����
	static bool IsCalcParameter(PARAMETER_TYPE param);

	/// �������� ������������ ��� VDoubleParameter
	static bool IsDoubleParameter(PARAMETER_TYPE param); 

	/// �������� ������������ ��� VZazorList
	static bool IsZazor(PARAMETER_TYPE param);

	/// ��������� �� ������ ��� �������� ���������
	std::wstring FormatErrorMessage(PARAMETER_TYPE param, const std::wstring & msg);

public:

	/// �����������
	VDataSet();

	/// ����������� - � ��������� ���������� �����
	VDataSet(const VFileName & fn);

	/// ����������� - � ��������� ���������� �����
	VDataSet(VIBDatabase & db, const VFileName & fn);

	/// ����������
	~VDataSet()
	{
		this->CloseFile();
	}

	/// ��������� �� ������
	const std::wstring & GetErrorMessage() const
	{
		return this->message;
	}

	// --------------------------------------------------------------------
	// �������� � ������
	// --------------------------------------------------------------------

	/// ������� ��������� ���� ��� ������
	bool OpenFile(const VFileName & fileName);

	/// ������� ����� ��������� ����
	bool CreateFile(const VFileName & fileName);

	/// ������� ��������� ���� ��� ������
	bool OpenFile(VIBDatabase & db, const VFileName & fileName);

	/// �������� �������� � ������ ����������
	void SetChanged(PARAMETER_TYPE param)
	{
		this->changed.insert(param);
		this->piList.insert(std::pair<PARAMETER_TYPE, bool>(param, true));
	}

	/// ������� �������� �� ������ ���������
	void UnsetChanged(PARAMETER_TYPE param)
	{
		this->changed.erase(param);
	}

	/// ��������� "������������" ���������
	bool IsChanged(PARAMETER_TYPE param) const
	{
		return this->changed.find(param) != this->changed.end();
	}

	/// ��������� ���� �� �������� �����-���� ������
	bool IsChanged()
	{
		return !this->changed.empty();
	}

	/// ����������� �� DataSet
	void CopyFrom(VDataSet & source, const VWaySection & section, 
		VProgressBox * progress);

	/// ����������� �� DataSet - ������� � ���� ���. ���������
	void CopyFrom(VDataSet & source, double absFrom, double absTo,
		VProgressBox * progress);

	/// ��������� ��������� � �����
	bool Save(const std::wstring & fileName = L"");

	/// ��������� ��� inm
	bool SaveToInm(const std::wstring & fileName, VProgressBox * progress);

	/// �������� ���� ��� �����������
	bool GetRtParameter(VRtParameter & kadr, size_t kadrNdx);

	/// ������� ����
	void CloseFile()
	{
		if (this->FileIsLoaded()) 
			this->file.Close();
	}

	/// ���� ������ ��� ������
	/// FIXME!!!! ��� ��������� ������ ����� ��������� �������� ����������� �����
	bool FileIsLoaded()
	{
		return this->file.Loaded() || this->remote.Loaded();
	}

	bool FileIsRemote() const
	{
		return this->remote.Loaded();
	}

	/// �������� ������� ������ � �����
	bool DataExists(PARAMETER_TYPE param);

	/// ��� ������������ ����� � ������ �����
	const VFileName & GetFileName() const
	{
		return this->fn;
	}

	/// ��� ������������ �����
	const std::wstring GetFileShortName() const
	{
		return this->GetFileName().GetShortName();
	}

	// --------------------------------------------------------------------
	// �������� � ���������� �����
	// --------------------------------------------------------------------

	const VMainFileHeader * GetFileHeader() const
	{
		return &this->header;
	}

	/// ������� ����� ������ ����������� � ���������
	uint32_t GetParamCount() const
	{
		return this->header.ParamCount;
	}

	// ����� ������
	void SetParamCount(uint32_t count) 
	{
		this->header.ParamCount = count;
	}

	/// ������� ���� �������
	const VTime & GetRailDate() const
	{
		return this->header.RailDate;
	}

	/// ���������� ���� �������
	void SetRailDate(const VTime & t)
	{
		this->header.RailDate = t;
	}

	/// ������� ����� ������
	uint32_t GetRoadNum() const
	{
		return this->header.RoadNum;
	}

	/// ���������� ����� ������
	void SetRoadNum(uint32_t rn)
	{
		this->header.RoadNum = rn;
	}

	/// ������� ����� �����������
	uint32_t GetDir() const
	{
		return this->header.Dir;
	}

	/// ������� ��� �������
	uint32_t GetStationCode() const
	{
		return this->header.StCode;
	}

	/// ������� ��� ����
	std::string GetWay() const;

	/// ������� ����������� ��������
	int8_t GetMoveDirection() const
	{
		return this->header.MoveDirection;
	}

	/// ���������� ����������� ��������
	void SetMoveDirection(int8_t md)
	{
		this->header.MoveDirection = md;
	}

	/// ������� �������������
	std::string GetDivision() const
	{
		return std::string(this->header.Division);
	}

	/// ���������� �������������
	void SetDivision(const std::string & div)
	{
		strncpy_s(this->header.Division, 16, div.c_str(), 15);
		this->header.Division[15] = 0;
	}

	/// ������� �������� ������
	std::string GetDataSource() const
	{
		return std::string(this->header.DataSource);
	}

	/// ��������� �������� ������
	void SetDataSource(const std::string & ds)
	{
		strncpy_s(this->header.DataSource, 256, ds.c_str(), 255);
		this->header.DataSource[255] = 0;
	}

	/// ������� ������ ��������� ������
	uint32_t GetSourceVersion() const
	{
		return this->header.SourceVersion;
	}

	/// ���������� ������ ��������� ������
	void SetSourceVersion(uint32_t sv)
	{
		this->header.SourceVersion = sv;
	}

	/// ������� �������� ��������
	std::string GetPeregon() const
	{
		return std::string(this->header.Peregon);
	}

	/// ������� �������� �����
	std::string GetParkName() const
	{
		return std::string(this->header.Peregon);
	}

	/// ������� ��� �����������
	std::string GetDirName() const
	{
		return std::string(this->header.DirName);
	}

	/// ������� ��� �������
	std::string GetStationName() const
	{
		return std::string(this->header.DirName);
	}

	/// ������� ����� �������� �����
	const VTime & GetCreateTime() const
	{
		return this->header.CreateTime;
	}

	/// ������� ����� ���������� �����
	const VTime & GetUpdateTime() const
	{
		return this->header.UpdateTime;
	}

	/// ������� ������� �����
	const VWaySection & GetWaySection() const
	{
		return this->header.WaySection;
	}

	/// ���������� ������� �����
	void SetWaySection(const VWaySection & ws)
	{
		this->header.WaySection = ws;
	}

	/// ������ �� ����������
	bool IsMagistral() const
	{
		return this->header.Dir != 0 && this->header.StCode == 0;
	}

	/// ������ �� ��������
	bool IsStation() const
	{
		return this->header.Dir == 0 && this->header.StCode != 0;
	}

	/// ������
	float GetStartLatitude() const
	{
		return this->header.Latitude;
	}

	/// �������
	float GetStartLongitude() const
	{
		return this->header.Longitude;
	}

	/// ���������� � ������������� �������
	bool SetMagistralInfo(uint32_t dirCode, const std::string & way, const std::string & dirName, const std::string & peregon);

	/// ���������� � ����������� �������
	bool SetStationInfo(uint32_t stCode, const std::string & stName, const std::string & park, const std::string & way);

	// --------------------------------------------------------------------
	// �������� � �������
	// --------------------------------------------------------------------

	/// �������� ����������
	bool Load(PARAMETER_TYPE param);

	/// �������� ������������� ���������
	bool IsLoaded(PARAMETER_TYPE param) const;

	/// �������� ������� �����
	bool CheckDataBlock(PARAMETER_TYPE param);

	/// ��������� ����� ������ � �����
	VTime GetDataBlockTime(PARAMETER_TYPE param);

	/// ���������� ������� � ����� ������ � �����
	size_t GetDataBlockRecCount(PARAMETER_TYPE param);

	/// ��������
	std::set<PARAMETER_TYPE> Unload(PARAMETER_TYPE param);

	/// ������ ��������� ��� ������ � CSV ����������
	std::set<PARAMETER_TYPE> GetCSVParameters();

	/// �������� � ����� ������� CSV
	size_t GetCSV(std::ostream & output, 
		const std::set<PARAMETER_TYPE> params,
		size_t step_kadr, float step_m, 
		const VWaySection & section,
		SIDEWAYS_PROCESSING swp,
		VProgressBox * progress);
		
	/// �������� � ����� ����� ���������
	size_t GetOpMarkCSV(std::ostream & output);		

	/// �������� � ����� ����� ������ 
	size_t GetCurveMarkCSV(std::ostream & output, bool model = false);

	/// �������� � ����� ����� ������� 
	size_t GetProfMarkCSV(std::ostream & output);

	/// �������� � ����� ���. �������� � ���
	size_t GetMoSkoCSV(std::ostream & output, int filter);

	/// ������ ��������� ��� ��������� ����������
	std::set<PARAMETER_TYPE> GetCorrectableParameters();

	/// ��������� ���.�-��
	void CorrectCoord(double offset_m, double expand_m_per_km);

	/// ������� �������
	VWaySection GetSection();

	/// ���������� ������ � �������
	size_t GetKadrCount();

	/// ���������� ���������� 
	VAbsCoordData & GetAbsCoord()
	{
		return this->absCoord;
	}

	/// ���� ��������� - �������
	VCorruptData & GetCorrupt()
	{
		return this->corrupt;
	}

	/// �������� ���
	VSpeedData & GetSpeed()
	{
		return this->speed;
	}

	/// ���. ���������� 
	VAprinfo & GetAprinfo()
	{
		return this->aprInfo;
	}

	/// ����������� ������� �� �����
	const VAprinfo & GetFileAprinfo() const
	{
		return this->fileAprinfo;
	}

	/// �������� ���.���������� �� �����
	bool AprinfoLoadFromFile()
	{
		return this->aprInfo.Load(this->file, DHT_APRINFO);
	}

	/// �������� ���.���������� �� ����
	bool Makeinfo(VIBDatabasePtr base, bool repairs, bool mea_curves);

	/// ������� ��������� DPP
	VUlongParameter & GetDPP()
	{
		return this->dpp;
	}

	/// �����������
	VOvr2List & GetOvrs2()
	{
		return this->ovrs2;
	}

	/// ����� �����������
	VOvr3List & GetOvrs3()
	{
		return this->ovrs3;
	}

	/// ����� ���������
	VOpMarkList & GetOpMarks()
	{
		return this->opMarks;
	}
	
	/// ��������� ������
	VMagZazorList & GetMagZazors()
	{
		return this->magZazor;
	}

	/// ����� �����
	VZazorList & GetZazorLeft()
	{
		return this->zazorLeft;
	}

	/// ����� ������
	VZazorList & GetZazorRight()
	{
		return this->zazorRight;
	}

	// ��������� ����� �����
	VZazorList & GetMagZazorLeft()
	{
		return this->magZazorLeft;
	}

	// ��������� ����� ������
	VZazorList & GetMagZazorRight()
	{
		return this->magZazorRight;
	}

	// ������ - ��� � ����
	VZazorList & GetZazor(PARAMETER_TYPE param);

	/// ��������� ������
	VUklonList & GetUklon()
	{
		return this->uklon;
	}

	/// ���� �����
	VUgonList & GetUgonLeft()
	{
		return this->ugonLeft;
	}

	/// ���� ������
	VUgonList & GetUgonRight()
	{
		return this->ugonRight;
	}

	/// ����� ����
	VSpalaMarkerList & GetSpalaMarker()
	{
		return this->spalaMarker;
	}

	/// �������� ������
	VCurveList & GetCurves(PARAMETER_TYPE in_type=PRM_CURVE_MARKS)
	{
		switch (in_type)
		{
			case PRM_CURVE_MARKS:
				return this->curves;
			case PRM_MODEL_CURVES:
				return this->modelCurves;
			default:
				throw VSimpleException(L"������������ ��� ���������", boost::str(boost::wformat(L"(%d)") % static_cast<uint32_t>(in_type)), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}	

	/// ������� ����
	VSidewayList & GetSideways()
	{
		return this->sideways;
	}

	/// ������
	VLineList & GetLines()
	{
		return this->lines;
	}

	/// ����� ������
	VLineMarkList & GetLineMarks()
	{
		return this->lineMarks;
	}

	/// ����� �������
	VProfileMarkList & GetProfMarks()
	{
		return this->profMarks;
	}

	/// ���������� �� �������
	VDiffProfile & GetDiffProfile()
	{
		return this->diff_profile;
	}

	/// ��������� �������
	VAproxProfile & GetAproxProfile()
	{
		return this->aprox_profile;
	}

	/// ������
	VSlopeList & GetProfileSlope(PARAMETER_TYPE in_data_type)
	{
		switch (in_data_type)
		{
			default:
			case PRM_PROFILE_SLOPE_ELEMENT:			
				return this->profile_slope_element;
			case PRM_PROFILE_SLOPE_KM:
				return this->profile_slope_km;
			case PRM_PROFILE_SLOPE_PK:
				return this->profile_slope_pk;
			case PRM_PROFILE_SLOPE_HPK:
				return this->profile_slope_hpk;
		}
	}

	//�������� �� ���� ������
	VPerKuzList & GetPerKuz ()
	{
		return this->perkuz;
	}

	//�������� �� ���� �������
	VPerTelList & GetPerTel ()
	{
		return this->pertel;
	}

	VPerLineList & GetPerLine ()
	{
		return this->perline;
	}

	// TODO: ������, ������� ����, ������������ �����, �������

	/// �������� �� �������
	VParameter & GetParameter(PARAMETER_TYPE param);

	/// �������� �� ������ � ��������
	void ParseParameter(wstring in_command, VParameter & parameter);

	/// �������� double �� �������
	VDoubleParameter & GetDoubleParameter(PARAMETER_TYPE param);

	/// ���������� ������ �� ����������
	VStatistics & GetStatistics()
	{
		return this->statistic;
	}

	/// ���������� ������ �� ����
	VPlan & GetPlan()
	{
		return this->plan;
	}	

	/// ���������� ������ �� ��������� ����
	VIdealPlan & GetIdealPlan(PARAMETER_TYPE in_type=PRM_IDEAL_PLAN)
	{		
		switch (in_type)
		{
			case PRM_IDEAL_PLAN:
				return this->ideal_plan;
			case PRM_MODEL_IDEAL_PLAN:
				return this->model_ideal_plan;
			default:
				throw VSimpleException(L"������������ ��� ���������", boost::str(boost::wformat(L"(%d)") % static_cast<uint32_t>(in_type)), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}

	/// ���������� ����� �� ����������
	VPlanOffset & GetPlanOffset(PARAMETER_TYPE in_type=PRM_PLAN_OFFSET)
	{	
		switch (in_type)
		{
			case PRM_PLAN_OFFSET:
				return this->plan_offset;
			case PRM_MODEL_PLAN_OFFSET:
				return this->model_plan_offset;
			default:
				throw VSimpleException(L"������������ ��� ���������", boost::str(boost::wformat(L"(%d)") % static_cast<uint32_t>(in_type)), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}	
	
	/// ������������ ��������� ����������� ��� ���������� �������
	VAnpReal & GetAnpHighPass()
	{
		return this->anp_real_high;		
	}

	/// ������������ ��������� ����������� ��� ���������� ������� � �������� ������
	VAnpReal & GetAnpHighBTTruck()
	{
		return this->anp_real_high_bt;		
	}

	/// ������������ ��������� ����������� ��� ������������ �������
	VAnpReal & GetAnpRealPass()
	{
		return this->anp_real_pass;		
	}

	/// ������������ ��������� ����������� ��� �������� �������
	VAnpReal & GetAnpRealTruck()
	{
		return this->anp_real_truck;		
	}
	
	/// ������������ ��������� ��������� ��� ���������� �������
	VAnpModel & GetAnpModelHigh()
	{
		return this->anp_model_high;
	}

	/// ������������ ��������� ��������� ��� ���������� ������� � �������� ������
	VAnpModel & GetAnpModelHighBT()
	{
		return this->anp_model_high_bt;
	}

	/// ������������ ��������� ��������� ��� ������������ �������
	VAnpModel & GetAnpModelPass()
	{
		return this->anp_model_pass;
	}

	/// ������������ ��������� ��������� ��� �������� �������
	VAnpModel & GetAnpModelTruck()
	{
		return this->anp_model_truck;
	}

	/// �������� ���������� ������������� ��������� (����. ����.)
	VPsiReal & GetPsiRealPass()
	{
		return this->psi_real_pass;
	}

	/// �������� ���������� ������������� ��������� (����. ����.)
	VPsiReal & GetPsiRealTruck()
	{
		return this->psi_real_truck;
	}

	/// ������ ����������� ��������
	VControlAreaList & GetControlAreas()
	{
		return this->ctrlAreas;
	}

	/// ������� ��������
	VHistory & GetHistory()
	{
		return this->history;
	}

	/// ������ ��������� �����
	VPlanElemList & GetPlanElemList(PARAMETER_TYPE in_type=PRM_PLAN_ELEM_LIST)
	{		
		switch (in_type)
		{
			case PRM_PLAN_ELEM_LIST:
				return this->planElem;
			case PRM_MODEL_PLAN_ELEM_LIST:
				return this->modelPlanElem;
			default:
				throw VSimpleException(L"������������ ��� ���������", boost::str(boost::wformat(L"(%d)") % static_cast<uint32_t>(in_type)), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}

	/// ��������� ������ ��������� �����
	VPlanElemList & GetModelPlanElemList()
	{
		return this->modelPlanElem;
	}

	/// ����������� � �����
	VComment & GetComment()
	{
		return this->comment;
	}

	/// ������� ����
	VCoordPointAutoCorr & GetCoordPoint()
	{
		return this->coordPoint;
	}
	

	/// ���������������� ������������ ���������� 
	VJaggyProfileOverstepList& GetJaggyProfileOversteps()
	{
		return this->jaggy_profile_overstep;
	}

	/// ���������������� �������������� ���������� 
	VJaggyPlanOverstepList& GetJaggyPlanOversteps()
	{
		return this->jaggy_plan_overstep;
	}

	/// �������
	VDiffWidthOverstepList& GetDiffWidthFrontOversteps()
	{
		return this->diff_width_front_overstep;
	}

	/// �������
	VDiffWidthOverstepList& GetDiffWidthBackOversteps()
	{
		return this->diff_width_back_overstep;
	}
	
	/// ������ ����������
	VCantingOverstepList& GetCantingOversteps(side_t in_side)
	{
		return in_side==SD_LEFT ? this->canting_left_overstep : this->canting_right_overstep;
	}
	
	/// ������ ����������
	VExtWearOverstepList& GetExtWearOversteps(side_t in_side)
	{
		return in_side==SD_LEFT ? this->ext_wear_left_overstep : this->ext_wear_right_overstep;
	}

	/// ������� ��/���
	VPrNgrAreas & GetPrNgrAreas()
	{
		return this->prNgrAreas;
	}

	//FixMe!!! �� ����, ��� ���� �������� ����� ������ ������, �� ��� ������������ �� ������ �� ���� ��������
	//��������� �������� ������� ������� ���������� ���������� ������� � ������������ ������� (����� ��� ����� �������� � VDataSet)
	//��� ����� ��������� � ���������, ���� ����� VDataSet::Load �� ����� ��������� ������ ����������� ���, ��������, 
	//����������� �������, ��� ������� ���������� ����� ����������. �� ���� � ����� Load ����� �������� ��� ����
	//�������� bool in_requested_only, ���� �������� ���������� � true, �� ����������� ������
	//��������� ��������, ���� - false, �� ����������� ����� � ����������� ��� ���� ���������.	

	///������������� ��������� ������� 
	void ProfileAutomark(VAUIProgressDialogPtr in_progress);	
	/// ������� ������� �� ��
	void ProfileFromDB(VIBDatabasePtr in_database, VAUIProgressDialogPtr in_progress);
	///����������� ����� � �������
	void AdjustProfileMarkHeight();
	/// ���������� true, ���� � ����� ����� ������� ����� �������
	bool AllowMarkHereProfile(size_t in_index, double in_min_length=200);
	/// ���������� true, ���� � ����� ����� ����������� ����� �������
	bool AllowMarkHereProfile(size_t in_index, size_t in_mark_index, double in_min_length=200);	
	/// ���������� ���������� �� ���������� ����� � ����������� �����
	void UpdatePlanOffset(size_t in_index, bool in_model, bool in_fast=false);
	/// ���������� ���������� �� ���������� �����
	void UpdatePlanOffset(bool in_model=false);
	/// ���������� ���������� ������������� ���������
	void UpdateAnpModel();
	/// �������� ������
	void DeleteCurves(PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// �������� ������
	void DeleteLines(PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// ������������ ������
	void AutomarkCurve(size_t in_start, size_t in_end, PARAMETER_TYPE in_mark_type);
	/// ���������� ������� (�����������)
	const VWayMarkerList& GetMarkers();
	/// ���������� true, ���� � ����� ���� ������
	bool HasCurves();
	/// ����� � �������������� �������� ������
	void SearchCurves(VAUIProgressDialogPtr in_progress, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// ����������� ������
	void CopyCurves(PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// ������� ������ �� �� (��������� ������)
	void TransferPassportCurves(VIBDatabasePtr in_database, VAUIProgressDialogPtr in_progress, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// ������� ������ �� �� (������ �� ���������� ��������)
	void TransferMeasuredCurves(VIBDatabasePtr in_database, VAUIProgressDialogPtr in_progress, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// ����������� ������
	void OptimizeCurves(VAUIProgressDialogPtr in_progress, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// ���������� true, ���� ������ �� ���������� ����� �������� �����
	bool CheckCurveVsLine(size_t in_index, const VCurve& in_curve, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// ���������� true ���� ��������� �������� �� ������� �����.
	bool AllowSidewayHere(size_t in_index_start, size_t in_index_end, PARAMETER_TYPE in_mark_type);
	/// ���������� true, ���� � ����� ����� ������� �����
	bool AllowMarkHerePlan(size_t in_index, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// ���������� true, ���� � ����� ����� ������� �����
	bool AllowMarkHereLevel(size_t in_index, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	///������������� ���������� �� ������������ �������
	void Autocorrection(VIBDatabase * database, POINT_TYPE type);
	///����� ���������� ����� �������
	double CalcDelta(VDataSet & dataset, PARAMETER_TYPE param);
	///����������� ���������� ���� ���������� (�������)
	double GetCorrelation(PARAMETER_TYPE in_param1, PARAMETER_TYPE in_param2);
	///����������� ���������� ���� ���������� (�������)
	double GetSignCorrelation(PARAMETER_TYPE in_param1, PARAMETER_TYPE in_param2);
	///���������� true, ���� � ����� ����� ������� ����� �������� �����
	bool AllowMarkHereLine(size_t in_index, double in_min_length);
	///��������� ���������� ������ �� �������� � ����������� �����
	void UpdateDiffLevel(bool in_is_model);

	// ������� �������� ��������� �������
	void ShiftMagZazor(int value);	

	/// ���������� ������� ���������� �� ���� ���
	VWayCoord GetCoordByDPP(unsigned long in_dpp, float correction);

};
typedef boost::shared_ptr<VDataSet> VDataSetPtr;

std::wstring GetPrefix(const VAprinfo & aprInfo, time_t t);
