#pragma once

#include <boost/asio.hpp>
#include <wx/wx.h>
#include "dblib.h"
//Boost
#include <boost/format.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/split.hpp>

/** �������� ������������������ ��*/
class VRegisteredDatabase
{
private:
	/** ���*/
	wstring name;
	/** ������*/
	wstring server;
	/** ���� � ����*/
	wstring database;
	/** ����� � �����������*/
	wstring folder;
	/** ���������� ���-�� ���� ��� �������������� ���������� �����������*/
	int count_days_backup;
public:
	/** �����������*/
	VRegisteredDatabase() : name(L""), server(L""), database(L""), folder(L""), count_days_backup(0x7FFFFFFF) {} ;
	/** �����������*/
	VRegisteredDatabase(const wstring& in_fullstring);
	/** �����������*/
	VRegisteredDatabase(const wstring& in_name, const wstring& in_server, const wstring& in_database, 
		const wstring& in_folder, const int& in_count_days_backup = 0x7FFFFFFF) : name(in_name), server(in_server), database(in_database), folder(in_folder), count_days_backup(in_count_days_backup) {};
	/** ���������� ������ ������*/
	const wstring GetFullString() const {return name+L";"+server+L";"+database+L";"+folder+L";"+boost::str(boost::wformat(L"%i") % count_days_backup);};
	/** ���������� ������ ����������*/
	const wstring GetConnectString() const {return server!=L"" ? server+L":"+database : database;}; 
	/** ���������� ���*/
	const wstring& GetName() const {return name;};
	/** ���������� ������*/
	const wstring& GetServer() const {return server;}; 
	/** ���������� ���� � ����� ��*/
	const wstring& GetDatabase() const {return database;}; 
	/** ���������� ���� � ����� � �����������*/
	const wstring& GetFolder() const {return folder;};
	/** ���������� ���������� ���-�� ���� ��� �������������� ���������� �����������*/
	const int& GetAllowDaysBackup() const {return count_days_backup;};
	/** ���������� ���������� ���-�� ���� ��� �������������� ���������� �����������*/
	void SetCountDaysBackup(int in_count_days_backup) {count_days_backup=in_count_days_backup;};
};
//����������� � ��
void ConnectDB(VIBDatabasePtr db, VRegisteredDatabase & RegisteredDatabase);