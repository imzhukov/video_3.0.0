#pragma once
#include <map>
#include <string>
#include <vector>

#include "dblib.h"

// ��������� sql-�������� 'insert'
class VSQLGenerator
{
	// ��� �������
	std::string table;			

	// ����� ����������
	std::vector<std::string> prm_names;

	// ���������
	std::vector<VIBParameter> prm_bodies;

	// ������ ������� � ���
	std::map<std::string, std::string> insql;	

	// �������� ������� ��������� � ������ (prm ��� insql)
	bool NameExists(const std::string & name) const;

public:

	// ������������� ��� ���������� ������� t
	void Init(const std::string & t);

	// ���������� �������� ��������� (blob)
	bool AddParameter(const std::string & name, const VIBParameter & prm);

	// ���������� �������������� ��������
	bool AddInteger(const std::string & name, const __int64 & value);

	// ���������� �������� � ��������� ������
	bool AddFloat(const std::string & name, const double & value);

	// ���������� ���������� �������� (char)
	bool AddString(const std::string & name, const std::string & text);

	// ���������� ���������� �������� (wide-char)
	bool AddString(const std::string & name, const std::wstring & text);

	// ���������� ����������
	bool AddTimestamp(const std::string & name, const VTime & value);

	// ������� ���������� SQL-����������
	bool AddSQL(const std::string & name, const std::string & text);

	// ������� �������� 'insert into ...'
	std::string GetInsertSQL() const;

	// ������� �������� 'update ... set ...'
	std::string GetUpdateSQL(const std::string & condition) const;

	// ����� ����������
	const std::vector<VIBParameter> & GetParameters() const;
};
