#include "sqlgen.h"

#include <sstream>

// ��������� ��������� ��� �����
static const bool InlineSQL = false; 

// �������� ������� ��������� � ������ (prm ��� insql)
bool VSQLGenerator::NameExists(const std::string & name) const
{
	if (std::find(this->prm_names.begin(), this->prm_names.end(), name) != this->prm_names.end())
	{
		return true;
	}
	
	if (this->insql.find(name) != this->insql.end())	
	{
		return true;
	}

	return false;
}

// ������������� ��� ���������� ������� t
void VSQLGenerator::Init(const std::string & t)
{
	this->table = t;
	this->prm_names.clear();
	this->prm_bodies.clear();
	this->insql.clear();
}

// ���������� �������� ��������� (blob)
bool VSQLGenerator::AddParameter(const std::string & name, const VIBParameter & prm)
{
	if (this->NameExists(name))	return false;
	this->prm_names.push_back(name);
	this->prm_bodies.push_back(prm);
	return true;
}

// ���������� �������������� ��������
bool VSQLGenerator::AddInteger(const std::string & name, const __int64 & value)
{
	if (this->NameExists(name))	return false;
//	this->prm_names.push_back(name);
//	this->prm_bodies.push_back(VIBParameter(SQL_INT64, (const char*)&value, sizeof(value)));

	if (InlineSQL)
	{
		std::stringstream s;
		s << value;
		this->AddSQL(name, s.str());
	}
	else
	{	
		this->AddParameter(name, VIBParameter(SQL_INT64, (const char*)&value, sizeof(value)));
	}

	return true;
}

// ���������� �������� � ��������� ������
bool VSQLGenerator::AddFloat(const std::string & name, const double & value)
{
	if (this->NameExists(name))	return false;
//	this->prm_names.push_back(name);
//	this->prm_bodies.push_back(VIBParameter(SQL_DOUBLE, (const char*)&value, sizeof(value)));

	if (InlineSQL)
	{
		std::stringstream s;
		s << value;
		this->AddSQL(name, s.str());
	}
	else
	{
		this->AddParameter(name, VIBParameter(SQL_DOUBLE, (const char*)&value, sizeof(value)));
	}

	return true;
}

// ���������� ���������� ��������
bool VSQLGenerator::AddString(const std::string & name, const std::string & text)
{
	if (this->NameExists(name))	return false;
//	this->prm_names.push_back(name);
//	this->prm_bodies.push_back(VIBParameter(SQL_TEXT, text.c_str(), text.length()));

	if (InlineSQL)
	{
		std::stringstream s;
		s << "'" << text << "'";
		this->AddSQL(name, s.str());
	}
	else
	{
		this->AddParameter(name, VIBParameter(SQL_TEXT, text.c_str(), text.length()));
	}

	return true;
}

bool VSQLGenerator::AddString(const std::string & name, const std::wstring & text)
{
	return this->AddString(name, wstring_to_string(text));
}

// ���������� ����������
bool VSQLGenerator::AddTimestamp(const std::string & name, const VTime & value)
{	
	if (this->NameExists(name)) return false;
//	this->prm_names.push_back(name);
//	struct tm __t = value.ToTM();
//	this->prm_bodies.push_back(VIBParameter(SQL_TIMESTAMP, (const char*)&__t));

	if (InlineSQL)
	{
		this->AddString(name, value.DateToStringISO());
	}
	else
	{
		struct tm __t = value.ToTM();
		this->AddParameter(name, VIBParameter(SQL_TIMESTAMP, (const char*)&__t));
	}

	return true;
}

// ������� ���������� SQL-����������
bool VSQLGenerator::AddSQL(const std::string & name, const std::string & text)
{
	if (this->NameExists(name))	return false;
	this->insql[name] = "(" + text + "";
	return true;
}

// ������� �������� 'insert into ...'
std::string VSQLGenerator::GetInsertSQL() const
{
	std::string sql = "insert into " + this->table + " (";

	int prmCnt = 0;
	for (size_t i = 0; i < this->prm_names.size(); ++i)
	{
		if (prmCnt > 0)	sql += ", ";
		sql += prm_names[i];
		++prmCnt;
	}

	std::map<std::string, std::string>::const_iterator isql;
	for (isql = this->insql.begin(); isql != this->insql.end(); isql++)
	{
		if (prmCnt > 0)	sql += ", ";
		sql += isql->first;
		++prmCnt;
	}

	sql += " values (";

	prmCnt = 0;
	for (size_t i = 0; i < this->prm_names.size(); ++i)
	{
		if (prmCnt > 0) sql += ", ";
		sql += "?";
		++prmCnt;
	}

	for (isql = this->insql.begin(); isql != this->insql.end(); isql++)
	{
		if (prmCnt > 0)	sql += ", ";
		sql += isql->second;
		++prmCnt;
	}

	sql += ";";

	return sql;
}

// ������� �������� 'update ... set ...'
std::string VSQLGenerator::GetUpdateSQL(const std::string & condition) const
{
	std::string sql = "update " + table + " set ";

	int prmCnt = 0;
	for (size_t i = 0; i < this->prm_names.size(); ++i)
	{
		if (prmCnt > 0)	sql += ", ";
		sql += prm_names[i] + " = ?";
		++prmCnt;
	}

	std::map<std::string, std::string>::const_iterator isql;
	for (isql = this->insql.begin(); isql != this->insql.end(); isql++)
	{
		if (prmCnt > 0)	sql += ", ";
		sql += isql->first + " = ";
		sql += isql->second;
		++prmCnt;
	}
	
	sql += " where " + condition + ";";
	return sql;
}

// ����� ����������
const std::vector<VIBParameter> & VSQLGenerator::GetParameters() const
{
	return this->prm_bodies;
}