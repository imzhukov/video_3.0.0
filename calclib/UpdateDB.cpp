#include "UpdateDB.h"

#include "VProgressInterface.h"

#include "strlib.h"
#include <sstream>
#include <wx/wx.h>
#include <wx/filename.h>

// ������ ��
int GetDBVersion(VIBDatabase & db3)
{
	if (!db3.Connected())
	{
		return -1;
	}

	// ����������
	VIBTransaction trans3 = db3.StartTransaction();

	// ���������� ������ �� ����
	VIBDataHandlerParameter handler3;

	std::stringstream sql;
	sql << "select max(version) from srv_version;";

	db3.ExecuteStatement(trans3, sql.str().c_str(), handler3);

	int version = 0;
	if (handler3.GetRowCount() == 1)
	{
		version = handler3.GetParameter(0, 0).GetInteger();
	}

	db3.RollbackTransaction(trans3);
	return version;
}

// ���-�� ���� � ���������� ���������� ����������� ��
int CountDaysLastBackup(VIBDatabase & db3)
{
	// ����������
	VIBTransaction trans3 = db3.StartTransaction();

	// ���������� ������ �� ����
	VIBDataHandlerParameter handler3;

	std::stringstream sql;
	int count_days = 0;	
	if (GetDBVersion(db3) < 606)
		sql << "select cast((cast(current_date as timestamp) - last_backup) as integer) from srv_version where version=(select max(version) from srv_version);";
	else
	{
		sql << "select cast((cast(current_date+0.5 as timestamp) - time_backup) as integer) from srv_backup where id=(select max(id) from srv_backup);";
		db3.ExecuteStatement(trans3, sql.str().c_str(), handler3);

		if (handler3.GetRowCount() == 1)
			count_days = handler3.GetParameter(0, 0).GetInteger();
		else
		{
			sql.str("");
			sql << "select cast((cast(current_date as timestamp) - date_comp) as integer) from srv_version where id=(select min(id) from srv_version);";
			db3.ExecuteStatement(trans3, sql.str().c_str(), handler3);
			if (handler3.GetRowCount() == 1)
				count_days = handler3.GetParameter(0, 0).GetInteger();
		}
	}

	db3.RollbackTransaction(trans3);
	return count_days;
}

std::wstring GetDBLocalFolder(VIBDatabase & db3)
{
	if (!db3.Connected())
	{
		return L"";
	}

	// ����������
	VIBTransaction trans3 = db3.StartTransaction();

	// ���������� ������ �� ����
	VIBDataHandlerParameter handler3;

	std::stringstream sql;
	sql << "select path from srv_local_folder;";
	db3.ExecuteStatement(trans3, sql.str().c_str(), handler3);

	std::wstring path;
	if (handler3.GetRowCount() >= 1)
	{
		path = handler3.GetParameter(handler3.GetRowCount() - 1, 0).GetString();
	}

	db3.RollbackTransaction(trans3);
	return path;
}

// ������ ��, � ���� ������
std::wstring GetDBInfo(VIBDatabase & db3)
{
	std::wstringstream msg;
	msg << L"���� � ���� ������: " << string_to_wstring(db3.GetDatabaseName()) << std::endl;

	int version = GetDBVersion(db3);
	msg << L"������ ���� ������: ";
	if (version < 0)
	{
		msg << L"���� ������ ����������";
	}
	else if (version == 0)
	{
		msg << L"���������� �����������";
	}
	else
	{
		msg << version;
	}
	
	//���������� � ������� � ���������� ���������� �����������
	msg << std::endl;	
	int days_last_backup = CountDaysLastBackup(db3);
		msg << L"����� � ���������� ���������� �����������: ";	
	if(days_last_backup < 0)
		msg << L"���������� �����������";
	else
		msg << days_last_backup << L" ����(�)";
	
	msg << std::endl;

	msg << L"���� � ������ ����� ��: " << GetDBLocalFolder(db3) << std::endl;


	return msg.str();
}

// �������� � ���������� ��������� ��
bool UpdateDB(VIBDatabase & db3, bool check_only)
{
	if (!db3.Connected())
	{
		wxMessageBox(L"����������� ���������� � ����� ������ 3", L"������");
		return false;
	}

	int version = GetDBVersion(db3);

	// ���� �� ��������� ������ � ������
	if (version < basicVersion)
	{
		wxMessageBox(
			L"���� ������ �������� � �� ����� ���� ������������",
			L"������", wxOK|wxICON_ERROR);
		return false;
	}

	// ��������� ��������
	else if (version > destVersion)
	{
		wxMessageBox(
			L"������������ ���� ����� �����, ��� ������������ ���������.\n"
			L"���������� �������� ���������.",
			L"��������� ���������� ���������", wxOK|wxICON_WARNING);
		return false;
	}

	// ���������� �� ���������
	if (version == destVersion)
	{
		return false;
	}

	std::wstringstream msg;
	msg << L"������� ���� ������ (������ " << version << L" ��������." << std::endl;

	//������ ��������
	if (check_only)
	{
		wxMessageBox(msg.str(), L"��������� ���������� ���� ������", wxOK|wxICON_ERROR);
		return false;
	}
	return true;
}

bool CheckBackupDB(VIBDatabase & db3, int allow_days_without_backup)
{
	//�������� �� ���������� ���������� �����������
	if(GetDBVersion(db3) >= 594)
	{
		int days_last_backup = CountDaysLastBackup(db3);
		if(days_last_backup > allow_days_without_backup)
			return true;
	}
	return false;
}