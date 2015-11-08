#pragma once

#include "dblib.h"
#include "sqlgen.h"
#include "VProgressInterface.h"
#include "DataExport.h"
#include "DataExportFunctions.h"
#include "strlib.h"

class VRepairEstimate
{
	// ���������
	std::wstring message;
	boost::shared_ptr<VProgressBox> prog;

	// ������� �� � �����
	VFileName fn_before, fn_after, fn_after_year;
	VDataSetPtr ds_before, ds_after, ds_after_year;
	__int64 trail_id_before, trail_id_after, trail_id_after_year;

	// ���� ������
	VIBDatabasePtr database;
	VIBTransaction trans;
	VIBDataHandlerParameter handler;
	__int64 repair_id;

	// �������� ������� �� ����
	VWaySection repair_section;
	__int64 waycat_after_repair;
	std::wstring repair_type;
	bool jaggy_plan_on_lines;

	// ��������� ��������
	struct PrjSpeedArea
	{
		VWaySection section;
		__int64 spd1, spd2;
	};
	std::vector<PrjSpeedArea> prj_speeds;

	// �����������
	struct OvrCount
	{
		__int64 km;
		__int64 ovr1count;
		__int64 ovr2count;
		__int64 ovr3count;
	};
	std::vector<OvrCount> ovrs;

	// ������ ���������
	struct KmEstimate
	{
		int km;
		float maxDiffProfile;
		float maxDiffPlan;
		float maxLevel;
		float maxWidthPlus;
		float maxWidthMinus;
		float sssp;
		speed_t dopSpd;
		int grade;

		KmEstimate() : km(0), maxDiffProfile(0), maxDiffPlan(0), maxLevel(0),
			maxWidthPlus(0), maxWidthMinus(0), sssp(0), dopSpd(NO_SPEED), grade(0)
		{}

		// ������� ������
		void Export(VSQLGenerator & sqlgen);
	};

	// ����� ������ ������� �������
	struct RepairArea
	{
		double absStart, absEnd;
		double maxGrad, maxAngle, minLen, maxJaggy, sssp;
		std::vector<KmEstimate> kms;

		// �����������
		RepairArea();
		
		// ������� ����������� ������ � ��������
		double Crossing(const VCurve & curve, VDataSetPtr ds) const;

		// ������� ������� �������
		void Estimate(const VWaySection & rs, VDataSetPtr ds, const std::wstring & repair_type,
			const std::vector<OvrCount> & ovrs, bool jaggy_plan_on_lines);

		// ������� ������
		void Export(VSQLGenerator & sqlgen);

	} repair_area_before, repair_area_after;

	// ��������� ����� ��� ����� ������� (��� ����������)
	std::vector<KmEstimate> kms_after_year;

	// ��������� ������
	bool ExecuteStatement(const std::string & sql);

	// ��������� ������
	bool ExecuteStatement(VSQLGenerator & sqlgen);

	// ��������� ����������
	bool Makeinfo(VDataSetPtr ds, const wchar_t * msg);

	// ������� ������
	bool ExportCurves(__int64 trail_id, RepairArea & area, VDataSetPtr ds);

public:

	// �����������
	VRepairEstimate(VIBDatabasePtr db, __int64 rep_id);

	// ������ � ������� �� ����
	bool Load();

	// ������ �������
	bool Estimate(const VFileName & fnb, const VFileName & fna);

	// ���� ����� ��� ����� �������
	bool CalcSSSPAfterYear(const VFileName & fnay);

	// ��������� �� ������
	const std::wstring & GetMessage() const
	{
		return this->message;
	}

	// ��������
	void Finalize();
};

