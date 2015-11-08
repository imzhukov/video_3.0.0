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

/** Обозначение NULL*/
#define NULL_VALUE L""

#define ADMIN_USER "SYSDBA"
#define ADMIN_PASS "masterkey"
#define DEFAULT_USER "CNII4"
#define DEFAULT_PASS "CNII4"

#define MAX_STATEMENT_LENGTH 64*1024

/** Диалект БД*/
static const int dialect=3;

/** Возвращает версию клиента*/
std::wstring GetClientVersion();

/** Разбор статус-вектора*/
std::string ParseStatusVector(ISC_STATUS * status_vector);

/** Исключение в БД*/
class VIBException : public VException
{
private:
	/** Статус - вектор*/
	ISC_STATUS status_vector[20];	
public:
	/** Конструктор*/
	VIBException(const ISC_STATUS * in_status_vector)
	{
		memcpy(&status_vector, in_status_vector, sizeof(ISC_STATUS)*20);

		switch (status_vector[1])
		{
			case isc_check_constraint:
				description=L"Нарушение ограничения.";
				details=string_to_wstring((char*)(status_vector[3]));
				break;
			case isc_unique_key_violation:
				description=L"Нарушение уникальности.";
				details=string_to_wstring((char*)(status_vector[3]));
				break;
			case isc_except:				
				description=L"Исключение.";
				details=string_to_wstring((char*)(status_vector[11]));
				break;
			case isc_convert_error:
				description=L"Ошибка преобразования значения.";
				details=string_to_wstring((char*)(status_vector[3]));
				break;
			default:
				description=L"Ошибка Firebird.";
				break;
		}				
		debug=string_to_wstring(ParseStatusVector(&status_vector[0]));
	};		
};

/** Выводит в журнал время выполнения запроса*/
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

/** Класс обертка для сервиса*/
class VIBService
{
private:
	/** handle сервиса*/
	isc_svc_handle service_handle;
public:
	/** Конструктор*/
	VIBService(){};
	/** Деструктор*/
	~VIBService()
	{
		try 
		{
			Detach();
		}
		catch(...){}
	};
	/** Соединение*/
	void Attach(const std::string& in_server_name="localhost", 
		const std::string& in_user=ADMIN_USER, 
		const std::string& in_password=ADMIN_PASS);
	/** Отключение*/
	void Detach();
	/** Возвращает версию*/
	std::wstring GetVersion();
	/** Возвращает папку в которой установлен сервер*/
	std::wstring GetEnv();
	/** Создает резервную копию БД*/
	void Backup(std::string in_source, std::string in_target);
	/** Восстанавливает БД из резервной копии*/
	void Restore(std::string in_backup, std::string in_database);
	/** Вернуть информацию о выполняемых операциях на сервере*/
	void ServiceQuery(char * respbuf, unsigned short & len);
	/** Возвращает handle сервиса*/
	isc_svc_handle * GetHandle()
	{
		return &service_handle;
	};	
};


/** Проверяет запущен ли сервер*/
bool IsServerRunning(const std::string& in_server="localhost");
/** Возвращает версию сервера*/
std::wstring GetServerVersion(const std::string& in_server="localhost");
/** Возвращает папку в которой установлен сервер*/
std::wstring GetServerPath(const std::string& in_server="localhost");
/** Проверяет существует ли пользователь*/
bool IsUserExists(const std::wstring& in_user_name);
/** Создание пользователя*/
void CreateUser(const std::wstring& in_user_name, const std::wstring& in_user_password);


class VIBDatabase;

/** Класс - обертка для транзакции*/
class VIBTransaction
{
friend VIBDatabase;
private:
	/** handle транзакции*/
	isc_tr_handle transaction_handle;
	/** Признак произведенных в этой транзакции изменений*/
	bool has_changes;	
		
public: 	
	/** Конструктор*/
	VIBTransaction()
	{
		transaction_handle=NULL;
		has_changes=false;
	};	
	/** Возвращает признак активной транзакции*/
	bool Active()
	{
		return (transaction_handle!=NULL);
	}	
	/** Возвращает признак накличия изменений в транзакции*/
	bool HasChanges()
	{
		return has_changes;
	}			
};
typedef boost::shared_ptr<VIBTransaction> VIBTransactionPtr;

/** 
	Интерфейс класса - обработчика данных
	от него нужно наследовать обработчики, которые будут вызываться при выполнении запросов SELECT
	например, для записи результатов в файл CSV или в XML нужны соответствующие обработчики	
	P.S. Возможно обработчик должен получать на вход не XSQLDA а XSQLVAR
*/
class VIBDataHandler
{
protected:
	/** Указатель на БД*/
	VIBDatabase * database;
	/** Указатель на транзакцию*/
	VIBTransaction * transaction;
	
public: 	
	/** Деструктор*/
	virtual ~VIBDataHandler(){};
	/** Функция подготовки обработчика. Может использоваться для того чтобы вывести заголовок таблицы*/
	virtual void Prepare(XSQLDA * out_sqlda){};
	/** Функция обработчика, должна быть реализована в наследнике*/	
	virtual void HandleData(XSQLDA * out_sqlda)=0;	
	/** Устанавливает указатели на БД и транзакцию*/
	void SetParameters(VIBDatabase * in_database, VIBTransaction * in_transaction)
	{
		database=in_database;
		transaction=in_transaction;
	};
};
typedef boost::shared_ptr<VIBDataHandler> VIBDataHandlerPtr;

/** Обработчик данных - вывод в stdout*/
class VIBDataHandlerSTD : public VIBDataHandler
{
private:
	/** Признак первого вызова*/
	bool is_first_time_call;
public:
	/** Подготовка обработчика*/
	void Prepare(XSQLDA * out_sqlda);
	/** Функция обработчика*/
	void HandleData(XSQLDA * out_sqlda);
};

/** Запись в файл CSV*/
class VIBDataHandlerCSV : public VIBDataHandler
{
private:
	/** Выходной файловый поток*/
	ofstream out;	
	/** Выовдить или не выводить заголовок*/
	bool show_header;
public:
	/** Конструктор*/
	VIBDataHandlerCSV();
	/** Подготовка обработчика*/
	void Prepare(XSQLDA * out_sqlda);
	/** Функция обработчика*/
	void HandleData(XSQLDA * out_sqlda);
	/** Открывает выходной поток*/
	void Open(const char * const in_file_name);
	/** Закрывает выходной поток*/	
	void Close();
	/** */
	void SetShowHeader(bool in_show_header);
	/** */
	bool GetShowHeader() const;
};

/** Запись в простейший файл XML*/
class VIBDataHandlerXML : public VIBDataHandler
{
private:
	/** Выходной файловый поток*/
	ofstream out;		
public:	
	/** Подготовка обработчика*/
	void Prepare(XSQLDA * out_sqlda);
	/** Функция обработчика*/
	void HandleData(XSQLDA * out_sqlda);
	/** Открывает выходной поток*/
	void Open(const char * const in_file_name);
	/** Закрывает выходной поток*/	
	void Close();		
};

/** BLOB (ToDo!!! Возможно этот класс будет базовым для конкретных типов данных, например для VBlobPicture или VBlobXml)*/
class VBlob
{
private:
	/// Размер данных
	size_t size;
	/// Данные
	boost::shared_array<char> data;
public:
	/** Конструктор*/
	VBlob();	
	/** Конструктор с загрузкой из потока*/
	VBlob(std::istream& in_stream);	
	/** Возвращает указатель на const данные*/
	const char * GetConstData() const;	
	/** Возвращает указатель на данные*/
	char * GetData();	
	/** Возвращает данные закодированные в base64*/
	void Base64Encode(wstring& out_data);
	/** Декодирует из base64*/
	void Base64Decode(const wstring& in_data);
	/** Возвращает размер данных*/
	size_t GetSize() const;
	/** Загружает данные из файла*/
	void Load(const wchar_t * in_filename);
	/** Записывает данные в файл*/
	void Save(const wchar_t * in_filename) const;
	/** Резервирует память*/
	void Reserve(size_t in_size);
};


/** Параметр запроса (обертка для XSQLVAR) FixMe!!! Возможно, лучше разделить на два класса для входного и выходного параметра*/
class VIBParameter
{
private:
	/** Параметр*/
	XSQLVAR sqlvar;
	/** Индикатор*/
	short ind;
	/** Массив данных*/
	char * data;
	
	//Методы

	/** Вычисляет размер по типу данных*/
	short AutoSize(short in_type, const char* in_data) const;

public:
	/** Конструктор*/
	VIBParameter();
	/** Конструктор*/
	VIBParameter(short in_type, const char* in_data, short in_size=0);
	/** Конструктор*/
	VIBParameter(XSQLVAR &in_sqlvar);
	/** Конструктор копирования*/
	VIBParameter(const VIBParameter& in_parameter);
	/** Деструктор*/
	~VIBParameter();
	/** Возвращает XSQLVAR*/
	const XSQLVAR * SqlVar() const;
	/** Возвращает представление в виде строки*/
	wstring GetString() const;
	/** Возвращает представление в виде целого */
	int64_t GetInteger() const;
	/** Возвращает представление в виде float */
	double GetFloat() const;
	/** Возвращает представление в виде timestamp */
	tm GetTimestamp() const;
	/** Возвращает имя поля*/
	wstring GetFieldName() const;
	/** Возвращает имя таблицы*/
	wstring GetRelationName() const;
	/** Возвращает владельца*/
	wstring GetOwnerName() const;
	/** Возвращает псевдоним*/
	wstring GetAliasName() const;
	/** Возвращает тип*/
	short GetType() const;
	/** Возвращает true, если значение параметра NULL*/
	bool IsNull() const;

	//Операторы

	/** Оператор присваивания*/
	VIBParameter& operator=(const VIBParameter& in_parameter);
};


/// Обработчик результатов запроса (с сохранением во внутренний буфер)
class VIBDataHandlerParameter : public VIBDataHandler
{
private:
	/// Число столбцов
	size_t colcount;
	/// Данные
	deque<vector<VIBParameter>> data;
	/// Список имен полей
	std::set<std::wstring> aliases;
public:
	/// Подготовка обработчика
	void Prepare(XSQLDA * out_sqlda);
	/// Функция обработчика
	void HandleData(XSQLDA * out_sqlda);
	/// Возвращает параметр по индексам
	const VIBParameter& GetParameter(size_t row, size_t col) const;
	/// Возвращает true если поле есть в запросе
	bool HasField(const wstring& in_field) const;
	/// Возвращает параметр по имени поля
	const VIBParameter& GetParameter(size_t in_row, const wstring& in_field) const;	
	/// Возвращает число строк
	size_t GetRowCount() const;
	/// Возвращает число столбцов
	size_t GetColCount() const;

	//ToDo!!! Сохранение в CSV и т.п.
};

/** Выделить память*/
void AllocData(XSQLDA * out_sqlda);
/** Освободить данные*/
void FreeData(XSQLDA * out_sqlda);

/** Соединение с БД Interbase/Firebird*/
class VIBDatabase
{
private:

	//Данные

	/** Имя сервера*/
	string server_name;
	/** Путь к БД*/
	string database_name;
	/** Имя пользователя*/
	string user;
	/** Пароль*/
	string password;
	/** Handle базы*/
	isc_db_handle db_handle;	
	/** STATUS Вектор*/
	ISC_STATUS status_vector[20];
	/** Транзакция по умолчанию*/
	VIBTransaction default_transaction;			
	/** Опция журналирования*/
	bool log_enabled;
	/** Список транзакций*/
	std::set<isc_tr_handle> transactions;

	/** Мониторинг событий*/	
	ISC_UCHAR *event_buffer, *result_buffer;	
	short length;
	ISC_LONG event_id;		
	bool monitor_running;
	std::vector<std::string> event_names;
	ISC_ULONG count_array[15];


	//Методы

	/** Закрытый конструктор копирования*/
	VIBDatabase(VIBDatabase&){};
	/** Закрытый оператор присваивания*/
	VIBDatabase& operator=(VIBDatabase&){return *this;};	
		
public:	
	/** Конструктор*/
	VIBDatabase();	
	/** Деструктор*/
	~VIBDatabase();		
	/** Соединение с БД*/
	void Connect(const char * const in_server_name, const char * const in_database_name, 
		const char * const in_user, const char * const in_password);
	/** Создание БД*/
	void Create(const char * const in_server_name, const char * const in_database_name, 
		const char * const in_user, const char * const in_password);
	/** Разрыв соединения*/
	void DisconnectDB();
	/** Возвращает описание ошибки*/
	string GetErrorDescription();
	/** Возвращает код ошибки*/
	long GetErrorCode();
	/** Возвращает имя ограничения, которое вызвало ошибку*/
	string GetErrorConstraint();
	/** Возвращает SQL код ошибки*/
	long GetSQLErrorCode();
	/** Возвращает признак соединения с БД*/
	bool Connected();
	/** Начало транзакции*/
	VIBTransaction StartTransaction();
	/** Откат транзакции*/
	void RollbackTransaction(VIBTransaction& in_transaction);
	/** Откат транзакции по умолчанию*/
	void RollbackTransaction();
	/** Подтверждение транзакции*/
	void CommitTransaction(VIBTransaction& in_transaction);
	/** Подтверждение транзакции по умолчанию*/
	void CommitTransaction();
	/** Немедленное выполнение SQL-запроса*/
	void ExecuteStatementImmediate(VIBTransaction& in_transaction, const char * in_statement);	
	/** Немедленное выполнение запроса с параметрами (для INSERT и UPDATE)*/
	void ExecuteStatementImmediate(VIBTransaction& in_transaction, const char * in_statement, const vector<VIBParameter>& in_parameters);	
	/** Выполнение запроса с подготовкой*/
	void ExecuteStatement(VIBTransaction& in_transaction, const char * in_statement);
	/** Выполнение запроса с подготовкой*/
	void ExecuteStatement(VIBTransaction& in_transaction, const char * in_statement, VIBDataHandler& in_data_handler);	
	/** Выполнение запроса с подготовкой с параметрами*/
	void ExecuteStatement(VIBTransaction& in_transaction, const char * in_statement, const vector<VIBParameter>& in_parameters);
	/** Выполнение запроса с подготовкой с параметрами*/
	void ExecuteStatement(VIBTransaction& in_transaction, const char * in_statement, const vector<VIBParameter>& in_parameters, VIBDataHandler& in_data_handler);	
	/** Выполнение запросов*/
	void ExecuteScript(VIBTransaction& in_transaction, const string& script, bool in_stop_on_errors);	
	/** Выполнение запросов из файла*/
	void ExecuteFromFile(VIBTransaction& in_transaction, const char * in_filename, bool in_stop_on_errors);				
	/** Возвращает число открытых транзакций*/
	size_t GetOpenTransactionCount();
	/** Возвращает имя сервера*/
	const string& GetServerName() const {return server_name;};
	/** Возвращает путь к файлу БД*/
	const string& GetDatabaseName() const {return database_name;};
	/** Возвращает строку соединения с БД*/
	const string GetConnectString() const {return server_name!="" ? server_name+":"+database_name : database_name;};	

	//Экспериментальные методы	

	/** Запускает мониторинг*/
	void StartMonitor();
	/** Проверяет событие*/
	bool TestEvent();
	/** Останавливает мониторинг*/
	void StopMonitor();	
	/** Возвращает true если мониторинг выполняется*/
	bool IsMonitorRunning() const;
	/** Устанавливает список отслеживаемых событий*/
	void SetEventNames(const std::vector<std::string>& in_event_names);
	/** Возвращает число событий*/
	size_t GetEventCount() const;
	/** Возвращает имя события*/
	std::string GetEventName(size_t in_index) const;
	/** Возвращает статус события*/
	size_t GetEventStatus(size_t in_index) const;
	

	/** Создание BLOB*/
	ISC_QUAD AddBlob(VIBTransaction& in_transaction, const VBlob& in_blob);		
	/** Возвращает размер BLOB*/
	size_t GetBLOBSize(VIBTransaction& in_transaction, ISC_QUAD blob_id);
	/** Чтение BLOB*/
	void ReadBLOB(VIBTransaction& in_transaction, ISC_QUAD blob_id, VBlob * in_blob);		
};
typedef boost::shared_ptr<VIBDatabase> VIBDatabasePtr;


/** Возвращает значение поля как std::wstring*/
wstring field_to_wstring(const XSQLVAR& sqlvar);

/** Возвращает значение поля как std::string*/
string field_to_string(const XSQLVAR& sqlvar);
