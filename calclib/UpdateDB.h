#pragma once

#include "dbfunc.h"

// ������, ������� ��������� ������
const int basicVersion = 200;
// ������, �� ������� ����� �����������
const int destVersion = 670;

class wxFileName;

/// ������ ��
int GetDBVersion(VIBDatabase & db3);

/// ��������� ����� ���������

std::wstring GetDBLocalFolder(VIBDatabase & db3);

/// �������� ��	
std::wstring GetDBInfo(VIBDatabase & db3);

/// �������� � ���������� ��������� ��
bool UpdateDB(VIBDatabase & db3, bool check_only=false);

/// �������� �� ��������� ��������� �����������
bool CheckBackupDB(VIBDatabase & db3, int allow_days_without_backup);

/// ������� ���-�� ���� � ���������� backup
int CountDaysLastBackup(VIBDatabase & db3);