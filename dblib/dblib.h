#pragma once

//STL
#include <string>
#include <vector>
#include <deque>
#include <set>
#include <iostream>
#include <fstream>

//Boost
#include <boost/format.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

//V
#include <os.h>
#include <ibase.h>
#include "strlib.h"
#include "VException.h"
#include "VLog.h"
#include "VProgressInterface.h"

using namespace std;

/** ����������� NULL*/
#define NULL_VALUE L""

#define ADMIN_USER "SYSDBA"
#define ADMIN_PASS "masterkey"
#define DEFAULT_USER "CNII4"
#define DEFAULT_PASS "CNII4"

#define MAX_STATEMENT_LENGTH 64*1024

/** ������� ��*/
static const int dialect=3;

/** ���������� ������ �������*/
std::wstring GetClientVersion();

/** ������ ������-�������*/
std::string ParseStatusVector(ISC_STATUS * status_vector);

/** ���������� � ��*/
class VIBException : public VException
{
private:
	/** ������ - ������*/
	ISC_STATUS status_vector[20];	
public:
	/** �����������*/
	VIBException(const ISC_STATUS * in_status_vector)
	{
		memcpy(&status_vector, in_status_vector, sizeof(ISC_STATUS)*20);

		switch (status_vector[1])
		{
			case isc_check_constraint:
				description=L"��������� �����������.";
				details=string_to_wstring((char*)(status_vector[3]));
				break;
			case isc_unique_key_violation:
				description=L"��������� ������������.";
				details=string_to_wstring((char*)(status_vector[3]));
				break;
			case isc_except:				
				description=L"����������.";
				details=string_to_wstring((char*)(status_vector[11]));
				break;
			case isc_convert_error:
				description=L"������ �������������� ��������.";
				details=string_to_wstring((char*)(status_vector[3]));
				break;
			default:
				description=L"������ Firebird.";
				break;
		}				
		debug=string_to_wstring(ParseStatusVector(&status_vector[0]));
	};		
};

/** ������� � ������ ����� ���������� �������*/
class QueryProfiler
{
private:
	std::wstring query;
	std::clock_t start;
	std::clock_t end;
public:
	QueryProfiler(const std::wstring& in_query)
	{
		query=in_query;
		start=std::clock();
		LOG_DEBUG(query);
	};
	~QueryProfiler()
	{
		end=std::clock();
		LOG_DEBUG(boost::str(boost::wformat(L"[%0.3f sec.]") % (double(end-start)/CLOCKS_PER_SEC)));
	};
};

/** ����� ������� ��� �������*/
class VIBService
{
private:
	/** handle �������*/
	isc_svc_handle service_handle;
public:
	/** �����������*/
	VIBService(){};
	/** ����������*/
	~VIBService()
	{
		try 
		{
			Detach();
		}
		catch(...){}
	};
	/** ����������*/
	void Attach(const std::string& in_server_name="localhost", 
		const std::string& in_user=ADMIN_USER, 
		const std::string& in_password=ADMIN_PASS);
	/** ����������*/
	void Detach();
	/** ���������� ������*/
	std::wstring GetVersion();
	/** ���������� ����� � ������� ���������� ������*/
	std::wstring GetEnv();
	/** ������� ��������� ����� ��*/
	void Backup(std::string in_source, std::string in_target);
	/** ��������������� �� �� ��������� �����*/
	void Restore(std::string in_backup, std::string in_database);
	/** ������� ���������� � ����������� ��������� �� �������*/
	void ServiceQuery(char * respbuf, unsigned short & len);
	/** ���������� handle �������*/
	isc_svc_handle * GetHandle()
	{
		return &service_handle;
	};	
};


/** ��������� ������� �� ������*/
bool IsServerRunning(const std::string& in_server="localhost");
/** ���������� ������ �������*/
std::wstring GetServerVersion(const std::string& in_server="localhost");
/** ���������� ����� � ������� ���������� ������*/
std::wstring GetServerPath(const std::string& in_server="localhost");
/** ��������� ���������� �� ������������*/
bool IsUserExists(const std::wstring& in_user_name);
/** �������� ������������*/
void CreateUser(const std::wstring& in_user_name, const std::wstring& in_user_password);


class VIBDatabase;

/** ����� - ������� ��� ����������*/
class VIBTransaction
{
friend VIBDatabase;
private:
	/** handle ����������*/
	isc_tr_handle transaction_handle;
	/** ������� ������������� � ���� ���������� ���������*/
	bool has_changes;	
		
public: 	
	/** �����������*/
	VIBTransaction()
	{
		transaction_handle=NULL;
		has_changes=false;
	};	
	/** ���������� ������� �������� ����������*/
	bool Active()
	{
		return (transaction_handle!=NULL);
	}	
	/** ���������� ������� �������� ��������� � ����������*/
	bool HasChanges()
	{
		return has_changes;
	}			
};
typedef boost::shared_ptr<VIBTransaction> VIBTransactionPtr;

/** 
	��������� ������ - ����������� ������
	�� ���� ����� ����������� �����������, ������� ����� ���������� ��� ���������� �������� SELECT
	��������, ��� ������ ����������� � ���� CSV ��� � XML ����� ��������������� �����������	
	P.S. �������� ���������� ������ �������� �� ���� �� XSQLDA � XSQLVAR
*/
class VIBDataHandler
{
protected:
	/** ��������� �� ��*/
	VIBDatabase * database;
	/** ��������� �� ����������*/
	VIBTransaction * transaction;
	
public: 	
	/** ����������*/
	virtual ~VIBDataHandler(){};
	/** ������� ���������� �����������. ����� �������������� ��� ���� ����� ������� ��������� �������*/
	virtual void Prepare(XSQLDA * out_sqlda){};
	/** ������� �����������, ������ ���� ����������� � ����������*/	
	virtual void HandleData(XSQLDA * out_sqlda)=0;	
	/** ������������� ��������� �� �� � ����������*/
	void SetParameters(VIBDatabase * in_database, VIBTransaction * in_transaction)
	{
		database=in_database;
		transaction=in_transaction;
	};
};
typedef boost::shared_ptr<VIBDataHandler> VIBDataHandlerPtr;

/** ���������� ������ - ����� � stdout*/
class VIBDataHandlerSTD : public VIBDataHandler
{
private:
	/** ������� ������� ������*/
	bool is_first_time_call;
public:
	/** ���������� �����������*/
	void Prepare(XSQLDA * out_sqlda);
	/** ������� �����������*/
	void HandleData(XSQLDA * out_sqlda);
};

/** ������ � ���� CSV*/
class VIBDataHandlerCSV : public VIBDataHandler
{
private:
	/** �������� �������� �����*/
	ofstream out;	
	/** �������� ��� �� �������� ���������*/
	bool show_header;
public:
	/** �����������*/
	VIBDataHandlerCSV();
	/** ���������� �����������*/
	void Prepare(XSQLDA * out_sqlda);
	/** ������� �����������*/
	void HandleData(XSQLDA * out_sqlda);
	/** ��������� �������� �����*/
	void Open(const char * const in_file_name);
	/** ��������� �������� �����*/	
	void Close();
	/** */
	void SetShowHeader(bool in_show_header);
	/** */
	bool GetShowHeader() const;
};

/** ������ � ���������� ���� XML*/
class VIBDataHandlerXML : public VIBDataHandler
{
private:
	/** �������� �������� �����*/
	ofstream out;		
public:	
	/** ���������� �����������*/
	void Prepare(XSQLDA * out_sqlda);
	/** ������� �����������*/
	void HandleData(XSQLDA * out_sqlda);
	/** ��������� �������� �����*/
	void Open(const char * const in_file_name);
	/** ��������� �������� �����*/	
	void Close();		
};

/** BLOB (ToDo!!! �������� ���� ����� ����� ������� ��� ���������� ����� ������, �������� ��� VBlobPicture ��� VBlobXml)*/
class VBlob
{
private:
	/// ������ ������
	size_t size;
	/// ������
	boost::shared_array<char> data;
public:
	/** �����������*/
	VBlob();	
	/** ����������� � ��������� �� ������*/
	VBlob(std::istream& in_stream);	
	/** ���������� ��������� �� const ������*/
	const char * GetConstData() const;	
	/** ���������� ��������� �� ������*/
	char * GetData();	
	/** ���������� ������ �������������� � base64*/
	void Base64Encode(wstring& out_data);
	/** ���������� �� base64*/
	void Base64Decode(const wstring& in_data);
	/** ���������� ������ ������*/
	size_t GetSize() const;
	/** ��������� ������ �� �����*/
	void Load(const wchar_t * in_filename);
	/** ���������� ������ � ����*/
	void Save(const wchar_t * in_filename) const;
	/** ����������� ������*/
	void Reserve(size_t in_size);
};


/** �������� ������� (������� ��� XSQLVAR) FixMe!!! ��������, ����� ��������� �� ��� ������ ��� �������� � ��������� ���������*/
class VIBParameter
{
private:
	/** ��������*/
	XSQLVAR sqlvar;
	/** ���������*/
	short ind;
	/** ������ ������*/
	char * data;
	
	//������

	/** ��������� ������ �� ���� ������*/
	short AutoSize(short in_type, const char* in_data) const;

public:
	/** �����������*/
	VIBParameter();
	/** �����������*/
	VIBParameter(short in_type, const char* in_data, short in_size=0);
	/** �����������*/
	VIBParameter(XSQLVAR &in_sqlvar);
	/** ����������� �����������*/
	VIBParameter(const VIBParameter& in_parameter);
	/** ����������*/
	~VIBParameter();
	/** ���������� XSQLVAR*/
	const XSQLVAR * SqlVar() const;
	/** ���������� ������������� � ���� ������*/
	wstring GetString() const;
	/** ���������� ������������� � ���� ������ */
	int64_t GetInteger() const;
	/** ���������� ������������� � ���� float */
	double GetFloat() const;
	/** ���������� ������������� � ���� timestamp */
	tm GetTimestamp() const;
	/** ���������� ��� ����*/
	wstring GetFieldName() const;
	/** ���������� ��� �������*/
	wstring GetRelationName() const;
	/** ���������� ���������*/
	wstring GetOwnerName() const;
	/** ���������� ���������*/
	wstring GetAliasName() const;
	/** ���������� ���*/
	short GetType() const;
	/** ���������� true, ���� �������� ��������� NULL*/
	bool IsNull() const;

	//���������

	/** �������� ������������*/
	VIBParameter& operator=(const VIBParameter& in_parameter);
};


/// ���������� ����������� ������� (� ����������� �� ���������� �����)
class VIBDataHandlerParameter : public VIBDataHandler
{
private:
	/// ����� ��������
	size_t colcount;
	/// ������
	deque<vector<VIBParameter>> data;
	/// ������ ���� �����
	std::set<std::wstring> aliases;
public:
	/// ���������� �����������
	void Prepare(XSQLDA * out_sqlda);
	/// ������� �����������
	void HandleData(XSQLDA * out_sqlda);
	/// ���������� �������� �� ��������
	const VIBParameter& GetParameter(size_t row, size_t col) const;
	/// ���������� true ���� ���� ���� � �������
	bool HasField(const wstring& in_field) const;
	/// ���������� �������� �� ����� ����
	const VIBParameter& GetParameter(size_t in_row, const wstring& in_field) const;	
	/// ���������� ����� �����
	size_t GetRowCount() const;
	/// ���������� ����� ��������
	size_t GetColCount() const;

	//ToDo!!! ���������� � CSV � �.�.
};

/** �������� ������*/
void AllocData(XSQLDA * out_sqlda);
/** ���������� ������*/
void FreeData(XSQLDA * out_sqlda);

/** ���������� � �� Interbase/Firebird*/
class VIBDatabase
{
private:

	//������

	/** ��� �������*/
	string server_name;
	/** ���� � ��*/
	string database_name;
	/** ��� ������������*/
	string user;
	/** ������*/
	string password;
	/** Handle ����*/
	isc_db_handle db_handle;	
	/** STATUS ������*/
	ISC_STATUS status_vector[20];
	/** ���������� �� ���������*/
	VIBTransaction default_transaction;			
	/** ����� ��������������*/
	bool log_enabled;
	/** ������ ����������*/
	std::set<isc_tr_handle> transactions;

	/** ���������� �������*/	
	ISC_UCHAR *event_buffer, *result_buffer;	
	short length;
	ISC_LONG event_id;		
	bool monitor_running;
	std::vector<std::string> event_names;
	ISC_ULONG count_array[15];


	//������

	/** �������� ����������� �����������*/
	VIBDatabase(VIBDatabase&){};
	/** �������� �������� ������������*/
	VIBDatabase& operator=(VIBDatabase&){return *this;};	
		
public:	
	/** �����������*/
	VIBDatabase();	
	/** ����������*/
	~VIBDatabase();		
	/** ���������� � ��*/
	void Connect(const char * const in_server_name, const char * const in_database_name, 
		const char * const in_user, const char * const in_password);
	/** �������� ��*/
	void Create(const char * const in_server_name, const char * const in_database_name, 
		const char * const in_user, const char * const in_password);
	/** ������ ����������*/
	void DisconnectDB();
	/** ���������� �������� ������*/
	string GetErrorDescription();
	/** ���������� ��� ������*/
	long GetErrorCode();
	/** ���������� ��� �����������, ������� ������� ������*/
	string GetErrorConstraint();
	/** ���������� SQL ��� ������*/
	long GetSQLErrorCode();
	/** ���������� ������� ���������� � ��*/
	bool Connected();
	/** ������ ����������*/
	VIBTransaction StartTransaction();
	/** ����� ����������*/
	void RollbackTransaction(VIBTransaction& in_transaction);
	/** ����� ���������� �� ���������*/
	void RollbackTransaction();
	/** ������������� ����������*/
	void CommitTransaction(VIBTransaction& in_transaction);
	/** ������������� ���������� �� ���������*/
	void CommitTransaction();
	/** ����������� ���������� SQL-�������*/
	void ExecuteStatementImmediate(VIBTransaction& in_transaction, const char * in_statement);	
	/** ����������� ���������� ������� � ����������� (��� INSERT � UPDATE)*/
	void ExecuteStatementImmediate(VIBTransaction& in_transaction, const char * in_statement, const vector<VIBParameter>& in_parameters);	
	/** ���������� ������� � �����������*/
	void ExecuteStatement(VIBTransaction& in_transaction, const char * in_statement);
	/** ���������� ������� � �����������*/
	void ExecuteStatement(VIBTransaction& in_transaction, const char * in_statement, VIBDataHandler& in_data_handler);	
	/** ���������� ������� � ����������� � �����������*/
	void ExecuteStatement(VIBTransaction& in_transaction, const char * in_statement, const vector<VIBParameter>& in_parameters);
	/** ���������� ������� � ����������� � �����������*/
	void ExecuteStatement(VIBTransaction& in_transaction, const char * in_statement, const vector<VIBParameter>& in_parameters, VIBDataHandler& in_data_handler);	
	/** ���������� ��������*/
	void ExecuteScript(VIBTransaction& in_transaction, const string& script, bool in_stop_on_errors);	
	/** ���������� �������� �� �����*/
	void ExecuteFromFile(VIBTransaction& in_transaction, const char * in_filename, bool in_stop_on_errors);				
	/** ���������� ����� �������� ����������*/
	size_t GetOpenTransactionCount();
	/** ���������� ��� �������*/
	const string& GetServerName() const {return server_name;};
	/** ���������� ���� � ����� ��*/
	const string& GetDatabaseName() const {return database_name;};
	/** ���������� ������ ���������� � ��*/
	const string GetConnectString() const {return server_name!="" ? server_name+":"+database_name : database_name;};	

	//����������������� ������	

	/** ��������� ����������*/
	void StartMonitor();
	/** ��������� �������*/
	bool TestEvent();
	/** ������������� ����������*/
	void StopMonitor();	
	/** ���������� true ���� ���������� �����������*/
	bool IsMonitorRunning() const;
	/** ������������� ������ ������������� �������*/
	void SetEventNames(const std::vector<std::string>& in_event_names);
	/** ���������� ����� �������*/
	size_t GetEventCount() const;
	/** ���������� ��� �������*/
	std::string GetEventName(size_t in_index) const;
	/** ���������� ������ �������*/
	size_t GetEventStatus(size_t in_index) const;
	

	/** �������� BLOB*/
	ISC_QUAD AddBlob(VIBTransaction& in_transaction, const VBlob& in_blob);		
	/** ���������� ������ BLOB*/
	size_t GetBLOBSize(VIBTransaction& in_transaction, ISC_QUAD blob_id);
	/** ������ BLOB*/
	void ReadBLOB(VIBTransaction& in_transaction, ISC_QUAD blob_id, VBlob * in_blob);		
};
typedef boost::shared_ptr<VIBDatabase> VIBDatabasePtr;


/** ���������� �������� ���� ��� std::wstring*/
wstring field_to_wstring(const XSQLVAR& sqlvar);

/** ���������� �������� ���� ��� std::string*/
string field_to_string(const XSQLVAR& sqlvar);
