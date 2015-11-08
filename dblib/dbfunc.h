#pragma once

#include "types.h"
#include "dblib.h"

/** ���������� ���������������� ����������*/
VWayCoord GetRealCoord(VIBDatabasePtr in_db, const char * in_way_code, const char * in_direction_code, double in_abscoord, const VTime& in_date);
/** ���������� ���������� ����������*/
VWayCoord GetRealCoord(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::wstring& in_way_id, double in_abscoord);

/** ���������� ���������� ����������*/
double GetAbsCoord(VIBDatabasePtr in_db, const char * in_way_code, const char * in_direction_code, const VWayCoord& in_waycoord, const VTime& in_date);

/** ���������� ���������� ����������*/
double GetAbsCoord(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::wstring& in_way_id, const VWayCoord& in_wc);

/** ��������� ����� � ���������������� ����������*/
VWayCoord IncRealCoord(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::wstring& in_way_id, const VWayCoord& in_wc, double in_length);

/** ���������� ��������� �� ������*/
std::wstring GetErrorDescription(VIBDatabasePtr in_db, VIBTransaction & in_transaction);

/** ���������� ��������� �� ������*/
std::wstring GetConstraintMessage(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::wstring& in_constraint);

/** ��������� ��������� �� �������*/
std::vector<std::wstring> GetErrorLog(VIBDatabasePtr in_db, VIBTransaction & in_transaction);

/** ������� ������ ������*/
void ClearErrorLog(VIBDatabasePtr in_db, VIBTransaction & in_transaction);

/** ������� ������� ��������*/
void ClearChanges(VIBDatabasePtr in_db, VIBTransaction & in_transaction);

/** ���������� ID ������*/
std::wstring GetRowID(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::string& in_query);

/** ���������� ��������� ������� ������ ���� ��� ������ �����*/
void GetSingleField(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::string& in_query, std::vector<std::wstring>& out_values);
