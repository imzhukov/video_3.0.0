#pragma once

#include "VDataSet.h"

class VIBDatabase;

/// ����� ���������
enum STATDEBUG_REPORT
{
	STATREP_KM,	/// �������������� ����������
	STATREP_PK	/// ���������� ����������
};

/// ���������� ��������� ������
std::wstring StatExportHTML(STATDEBUG_REPORT report,
							const VFileName & fileName, 
							const VFileName & reportFile, 
							VIBDatabasePtr dataBase);


