#pragma once

#include "VDataSet.h"

enum DATA_EXPORT
{
	DE_OVERSTEPS, DE_CURVES, DE_MODEL_CURVES,
	DE_STATISTICS, DE_MARKS, DE_ZAZOR, DE_UKLON, DE_PEREKOS, 
	DE_PROFILE, DE_JAGGY, DE_DIFF_WIDTH, DE_PPR, DE_MARKERS, 
	DE_PLAN_SPD, DE_AUTO
};

/// �������� ������� ������� � ����
bool CheckTrailInBase(VIBDatabasePtr db, const VFileName & file);

/// �������� ������������� ���������� ������ � ����
std::set<DATA_EXPORT> CheckDataUpdates(VIBDatabasePtr db, const VFileName & file);

/// �������� �� ���� ���������� � �������
std::wstring DeleteTrail(VIBDatabasePtr db, const VFileName & file);

/// �������� �� ���� ���������� � ��������
std::wstring DeleteTrails(VIBDatabasePtr db, const std::vector<VFileName> & files);

/// �������� ����������� ������ �� ����
std::wstring DeleteStCurves(VIBDatabasePtr db, const VFileName & file);

/// ��������� ������ � ���� 
std::wstring DataMultiExportDB(VIBDatabasePtr db, 
							   const std::vector<VFileName> & files, 
							   bool stationFile, bool rewrite, 
							   const std::set<DATA_EXPORT> & exports,
							   std::vector<VFileName> & exported);

/// ��������� �������� ������ �� ����
std::wstring PartialDataDelete(VIBDatabasePtr db,
                               const std::vector<VFileName> & files,
                               const std::set<DATA_EXPORT> & deletion);

/// ������ ��������, �� ������� ������ ������
std::vector<std::wstring> GetRepairTrails(VIBDatabasePtr db, int repair_id);

/// ������ ��������
std::wstring EstimateRepair(VIBDatabasePtr db, int repair_id,
							const VFileName & before, const VFileName & after, 
							const VFileName & after_year);

/// ���� ����� ��� ����� �������
std::wstring RepairSSSPAfterYear(VIBDatabasePtr db, int repair_id, const VFileName & after);

/// �������� ������ �������
std::wstring DeleteRepair(VIBDatabasePtr db, int repair_id);

/// ���������� ���������� � ������� �� ������� ����.��������
std::wstring PlanSpeedExport(VIBDatabasePtr db, const VFileName & file, VProgressBox & prog);

/// ��������� � �� ����� ����������� �����
std::wstring ExportSingleStationFileDB(VIBDatabasePtr db, const VFileName & fileName, VProgressBox & prog);

/// ��������� � �� ���������� ������ ����������� �����
std::wstring ExportMultiStationFileDB(VIBDatabasePtr db, const std::vector<VFileName> & files);

/// �������� ����� ��. ����� �� ����
std::wstring DeleteStTrail(VIBDatabasePtr db, const std::wstring & sh_name);

/// ������ ������� �� �����
std::wstring CalcStationProfile(VIBDatabasePtr db, const VFileName & fn);

/// �������� ������� ������� �� �����
std::wstring DeleteStationProfile(VIBDatabasePtr db, const std::wstring & sh_name);

/// ����� ����� ����������� � �����
int64_t CalcProfPointCount(VIBDatabasePtr db, const std::wstring & sh_name, bool apr);