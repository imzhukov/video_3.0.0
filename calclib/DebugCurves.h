#pragma once

#include "VDataSet.h"

class VIBDatabase;

/// ����� ���������
enum CRVDEBUG_REPORT
{
	CRVREP_CURVES,	// ������
	CRVREP_STCURVES,// ������ �� ����������� �����
	CRVREP_PK,		// ������ (� �������)
	CRVREP_LINKS,	// ������ (� �������)
	CRVREP_FPO,		// ������� ��� (1 �������� / 1 ��)
	CRVREP_WEARDGR, // ��������� �������
	CRVREP_MODEL	// ������ ������
};

/// ���������� ��������� ������
std::wstring CurveExportHTML(CRVDEBUG_REPORT report,
							 const VFileName & fileName, 
							 const VFileName & reportFile, 
							 VIBDatabasePtr dataBase);

DECLARE_OPTION(double, EXPERIMENTAL_ANP, 0, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
