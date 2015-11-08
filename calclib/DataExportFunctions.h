#pragma once

#include "revision.h"
#include "VProgressInterface.h"
#include "VDataSet.h"
#include "VFileName.h"
#include "dblib.h"
#include "dbfunc.h"

/// �������� ��������
extern const std::wstring OPERATION_CANCELED;

/// ���������� ���������� � �������
std::wstring UpdateTrail(VIBDatabasePtr db, VIBTransaction & trans, VDataSetPtr ds);

/// ID �������
__int64 GetTrailID(VIBDatabasePtr db, VIBTransaction & trans, const VFileName & file);

/// ��������� ������
std::wstring AddCurves(VIBDatabasePtr db, VIBTransaction & trans, 
					   VDataSetPtr ds, __int64 id,
					   VProgressBox * prog, CURVE_TYPE crvType, bool optics);

/// ��������� ��������� ������
/*std::wstring AddModelCurves(VIBDatabasePtr db, VIBTransaction & trans,
							VDataSetPtr ds, __int64 id, VProgressBox * prog);*/

/// ��������� ����������
std::wstring AddStatistics(VIBDatabasePtr db, VIBTransaction & trans,
						   VDataSetPtr ds, __int64 id, VProgressBox * prog);

/// ��������� �����������
std::wstring AddOvrs(VIBDatabasePtr db, VIBTransaction & trans,
					  VDataSetPtr ds, __int64 id, VProgressBox * prog);

/// ��������� ����� ���������, ����� ���� ���
std::wstring AddMarks(VIBDatabasePtr db, VIBTransaction & trans,
					  VDataSetPtr ds, __int64 id, VProgressBox * prog);

/// ��������� �������
std::wstring AddZaz(VIBDatabasePtr db, VIBTransaction & trans,
					VDataSetPtr ds, __int64 id, VProgressBox * prog);

// ��������� ����. ��������� ������
std::wstring AddUklon(VIBDatabasePtr db, VIBTransaction & trans,
					  VDataSetPtr ds, __int64 id, VProgressBox * prog);

// ��������� ��������� �� ���� ������ � �������
std::wstring AddPerekos(VIBDatabasePtr db, VIBTransaction & trans,
					   VDataSetPtr ds, __int64 id, VProgressBox * prog);

// ��������� ����. ���������
std::wstring AddUskor(VIBDatabasePtr db, VIBTransaction & trans, VDataSetPtr ds,
					  __int64 id, VProgressBox * prog);

// ��������� ����. ���������� � ������
std::wstring AddPerLine(VIBDatabasePtr db, VIBTransaction & trans, 
						VDataSetPtr ds, __int64 id, VProgressBox * prog);

// ��������� �������� �������
std::wstring AddProfile(VIBDatabasePtr db, VIBTransaction & trans, 
						VDataSetPtr ds, __int64 id, VProgressBox * prog);

// ���������������� ����������
std::wstring AddJaggy(VIBDatabasePtr db, VIBTransaction & trans,
					  VDataSetPtr ds, __int64 id, VProgressBox * prog);

// ������� �������
std::wstring AddDiffWidth(VIBDatabasePtr db, VIBTransaction & trans,
						  VDataSetPtr ds, __int64 id, VProgressBox * prog);

// ������ ���
std::wstring AddPPR(VIBDatabasePtr db, VIBTransaction & trans,
					VDataSetPtr ds, __int64 id, VProgressBox * prog);

// �������
std::wstring AddMarkers(VIBDatabasePtr db, VIBTransaction & trans,
					   VDataSetPtr ds, __int64 id, VProgressBox * prog);

// ������ ��������� ���������
std::wstring AddPlanSpd(VIBDatabasePtr db, VIBTransaction & trans,
						VDataSetPtr ds, __int64 id, VProgressBox * prog);

