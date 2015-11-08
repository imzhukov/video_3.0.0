#include "dblib.h"
#include <algorithm>
#include <sstream>
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/scope_exit.hpp>
#include "Shiny.h"

using namespace boost::xpressive;

/** Соединение*/
// [mg] NB! убрано из инлайна для сокращения кол-ва мусора в варнингах 
// при strcpy()
void VIBService::Attach(const std::string& in_server_name, const std::string& in_user, const std::string& in_password)
{
	ISC_STATUS status_vector[20];		
	char spb_buffer[128], *spb=spb_buffer;
	char request[255], *p=request;
	unsigned short spb_length;
	service_handle=NULL;

	//Подключение к сервису
	*spb++ = isc_spb_version;
	*spb++ = isc_spb_current_version;

	*spb++ = isc_spb_user_name;
	*spb++ = strlen(in_user.c_str());
	strcpy(spb, in_user.c_str());
	spb+=strlen(in_user.c_str());

	*spb++ = isc_spb_password;
	*spb++ = strlen(in_password.c_str());
	strcpy(spb, in_password.c_str());
	spb+=strlen(in_password.c_str());

	spb_length = (short) (spb - spb_buffer);

	isc_service_attach(status_vector, 0, (char*)((in_server_name+":service_mgr").c_str()), &service_handle, spb_length, spb_buffer);
	if (status_vector[0]==1 && status_vector[1])
	{
		throw VSimpleException(L"Ошибка Firebird.", string_to_wstring(ParseStatusVector(status_vector)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
};

/** Отключение*/
void VIBService::Detach()
{
	//Отключение
	ISC_STATUS status_vector[20];
	isc_service_detach(status_vector,&service_handle);
	if (status_vector[0]==1 && status_vector[1])
	{
		throw VSimpleException(L"Ошибка Firebird.", string_to_wstring(ParseStatusVector(status_vector)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);		
	}
}

/** Возвращает версию*/
std::wstring VIBService::GetVersion()
{	
	ISC_STATUS status_vector[20];
	char request[255], *p=request;
	unsigned short spb_length;
	*p++ = isc_action_svc_display_user;	
	spb_length= (short) (p - request);
	isc_service_start(status_vector, GetHandle(), NULL, spb_length, request);	
	if (status_vector[0]==1 && status_vector[1])
	{
		throw VSimpleException(L"Ошибка Firebird.", string_to_wstring(ParseStatusVector(status_vector)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);		
	}

	//
	string result;
	char request2[255], *p2=request2;
	char buffer[1024], *p3=buffer;
	memset(buffer, 0, 1024);
	*p2++ = isc_info_svc_server_version;	
	isc_service_query(status_vector, GetHandle(), NULL, 0, 0, spb_length, request2, 1024, buffer);
	if (status_vector[0]==1 && status_vector[1])
	{
		throw VSimpleException(L"Ошибка Firebird.", string_to_wstring(ParseStatusVector(status_vector)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);		
	}
	p3+=3;
	while (*p3 !=isc_info_end)
	{
		result+=*p3;
		p3++;
	}		
	return string_to_wstring(result);
}

/** Возвращает папку в которой установлен сервер*/
std::wstring VIBService::GetEnv()
{
	ISC_STATUS status_vector[20];
	char request[255], *p=request;
	*p++ = isc_action_svc_display_user;	
	short spb_length= (short) (p - request);
	isc_service_start(status_vector, GetHandle(), NULL, spb_length, request);	
	if (status_vector[0]==1 && status_vector[1])
	{
		throw VSimpleException(L"Ошибка Firebird.", string_to_wstring(ParseStatusVector(status_vector)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);		
	}

	//
	string result;
	char request2[255], *p2=request2;
	char buffer[1024], *p3=buffer;
	memset(buffer, 0, 1024);	
	*p2++ = isc_info_svc_get_env;
	isc_service_query(status_vector, GetHandle(), NULL, 0, 0, spb_length, request2, 1024, buffer);
	if (status_vector[0]==1 && status_vector[1])
	{
		throw VSimpleException(L"Ошибка Firebird.", string_to_wstring(ParseStatusVector(status_vector)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);		
	}
	p3+=3;
	while (*p3 !=isc_info_end)
	{
		result+=*p3;
		p3++;
	}
			
	return string_to_wstring(result);
}

/** Создает резервную копию БД*/
void VIBService::Backup(std::string in_source, std::string in_target)
{		
	ISC_STATUS status_vector[20];
	char request[255], *p=request;				
	*p++ = isc_action_svc_backup;		
	*p++ = isc_spb_dbname;
	ADD_SPB_LENGTH(p, strlen(in_source.c_str()));
	for (char * x=(char *)in_source.c_str(); *x;) 
		*p++=*x++;

	*p++ = isc_spb_bkp_file;
	ADD_SPB_LENGTH(p, strlen(in_target.c_str()));
	for (char * x=(char *)in_target.c_str(); *x;) 
		*p++=*x++;

	*p++ = isc_spb_verbose;

	short spb_length= (short) (p - request);

	isc_service_start(status_vector, &service_handle, NULL, spb_length, request);
		
	if (status_vector[0]==1 && status_vector[1])
	{
		throw VSimpleException(L"Ошибка Firebird.", string_to_wstring(ParseStatusVector(status_vector)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	
	char *x;
	char respbuf[512];
	//VProgressBox prog(1, "Резервное копирование БД", false);
	char msg[256];
	do
	{
		char sendbuf[] = {isc_info_svc_line};
		ISC_STATUS loc_status[20], *stat = loc_status;

		isc_service_query(status_vector, &service_handle, NULL, 0, NULL, sizeof (sendbuf), sendbuf, 512, respbuf);
		if (status_vector[0]==1 && status_vector[1])
		{
			throw VSimpleException(L"Ошибка Firebird.", string_to_wstring(ParseStatusVector(status_vector)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
		
		x = p = respbuf;
		if (*p++ == isc_info_svc_line)
		{
			ISC_USHORT len = 0, chTmp = 0;
			len = (ISC_USHORT)isc_vax_integer(p, sizeof(ISC_USHORT));
			p += sizeof (ISC_USHORT);

			if (!len)
			{
				if (*p != isc_info_end)
				{
					sprintf (msg, "Ошибка ... <%d>", *p);
					LOG_DEBUG(string_to_wstring(msg));
					//prog.SetMessage(0, msg);
				}
				else
				{
					sprintf (msg, "Завершение операции");
					LOG_DEBUG(string_to_wstring(msg));
					//prog.SetMessage(0, msg);
				}
				break;
			}
			strcpy(msg, "");
			for (chTmp = 0; chTmp < len; chTmp++)
			{
				char ch [3];
				sprintf(ch, "%c", p[chTmp]);
				strcat(msg, ch);
			}
			LOG_DEBUG(string_to_wstring(msg));
			//prog.SetMessage(0, msg);
			//prog.SetPosition(0, 1);			

			p += len;
			if (*p != isc_info_truncated && *p != isc_info_end)
			{
				sprintf (msg, "Ошибка ... <%d>\n", *p);
				LOG_DEBUG(string_to_wstring(msg));
				//prog.SetMessage(0, msg);
				break;
			}
		}
	} while (*x == isc_info_svc_line);
}

/** Вернуть информацию о выполняемых операциях на сервере*/
void VIBService::ServiceQuery(char * respbuf, unsigned short & len)
{
//	char respbuf[512];
	char sendbuf[] = {isc_info_svc_line};
	ISC_STATUS status_vector[20], *stat = status_vector;

	isc_service_query(status_vector, &service_handle, NULL, 0, NULL, sizeof (sendbuf), sendbuf, 512, respbuf);
	
	if (status_vector[0]==1 && status_vector[1])
	{
		throw VSimpleException(L"Ошибка Firebird.", string_to_wstring(ParseStatusVector(status_vector)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	
	char *p = respbuf;
	if (*p++ == isc_info_svc_line)
		len = (ISC_USHORT)isc_vax_integer(p, sizeof(ISC_USHORT));
}

/** Восстанавливает БД из резервной копии*/
void VIBService::Restore(std::string in_backup, std::string in_database)
{
	ISC_STATUS status_vector[20];
	char request[255], *p=request;				
	*p++ = isc_action_svc_restore;
	*p++ = isc_spb_bkp_file;
	ADD_SPB_LENGTH(p, strlen(in_backup.c_str()));
	for (char * x=(char *)in_backup.c_str(); *x;) 
		*p++=*x++;	

	*p++ = isc_spb_dbname;
	ADD_SPB_LENGTH(p, strlen(in_database.c_str()));
	for (char * x=(char *)in_database.c_str(); *x;) 
		*p++=*x++;

	*p++ = isc_spb_options;
	unsigned long options=isc_spb_res_create;
	ADD_SPB_NUMERIC(p,options);
	
	*p++ = isc_spb_verbose;

	short spb_length= (short) (p - request);
		
	isc_service_start(status_vector, &service_handle, NULL, spb_length, request);
	if (status_vector[0]==1 && status_vector[1])
	{
		throw VSimpleException(L"Ошибка Firebird.", string_to_wstring(ParseStatusVector(status_vector)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	
	char *x;
	char respbuf[512];
	//VProgressBox prog(1, "Восстановление БД из файла", false);
	char msg[256];
	do
	{
		char sendbuf[] = {isc_info_svc_line};
		ISC_STATUS loc_status[20], *stat = loc_status;

		isc_service_query(status_vector, &service_handle, NULL, 0, NULL, sizeof (sendbuf), sendbuf, 512, respbuf);
		
		if (status_vector[0]==1 && status_vector[1])
		{
			throw VSimpleException(L"Ошибка Firebird.", string_to_wstring(ParseStatusVector(status_vector)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}

		x = p = respbuf;
		if (*p++ == isc_info_svc_line)
		{
			ISC_USHORT len = 0, chTmp = 0;
			len = (ISC_USHORT)isc_vax_integer(p, sizeof(ISC_USHORT));
			p += sizeof (ISC_USHORT);

			if (!len)
			{
				if (*p != isc_info_end)
				{
					sprintf (msg, "Ошибка ... <%d>", *p);
					LOG_DEBUG(string_to_wstring(msg));
					//prog.SetMessage(0, msg);
				}
				else
				{
					sprintf (msg, "Завершение операции");
					LOG_DEBUG(string_to_wstring(msg));
					//prog.SetMessage(0, msg);
				}
				break;
			}
			strcpy(msg, "");
			for (chTmp = 0; chTmp < len; chTmp++)
			{
				char ch [3];
				sprintf(ch, "%c", p[chTmp]);
				strcat(msg, ch);
			}
			LOG_DEBUG(string_to_wstring(msg));
			//prog.SetMessage(0, msg);
			//prog.SetPosition(0, 1);			

			p += len;
			if (*p != isc_info_truncated && *p != isc_info_end)
			{
				sprintf (msg, "Ошибка ... <%d>\n", *p);
				LOG_DEBUG(string_to_wstring(msg));
				//prog.SetMessage(0, msg);
				break;
			}
		}
	} while (*x == isc_info_svc_line);	
}

/** Возвращает версию клиента*/
std::wstring GetClientVersion()
{
	char version[255]; 
	isc_get_client_version(version);
	return string_to_wstring(version);
}

/** Разбор статус-вектора*/
std::string ParseStatusVector(ISC_STATUS * status_vector)
{	
	string result("");
	if(status_vector[0]==1 && status_vector[1])
	{		
		//Interbase error
		result+="IB error: ";
		char ib_msg[4096];
		ISC_STATUS *pvector = status_vector;					
		while (fb_interpret(ib_msg, 4096, (const ISC_STATUS**) &pvector))
		{					
			result+=ib_msg;
			result+=" ";
		}
		//SQL error		
		result+="SQL error: ";
		char sql_msg[512];
		ISC_LONG sqlcode = isc_sqlcode(status_vector);
		isc_sql_interprete(sqlcode, sql_msg, 512);		
		result+=sql_msg;		
	}
	else
		result="Success!";		
	
	return result;
}

/** Проверяет запущен ли сервер*/
bool IsServerRunning(const std::string& in_server)
{		
	VIBService service;
	try
	{
		service.Attach(in_server);
	}
	catch (...)
	{
		return false;
	}
	return true;
}

/** Возвращает версию сервера*/
std::wstring GetServerVersion(const std::string& in_server)
{	
	VIBService local_service;
	local_service.Attach(in_server);
	return local_service.GetVersion();		
}

/** Возвращает папку в которой установлен сервер*/
std::wstring GetServerPath(const std::string& in_server)
{	
	VIBService service;
	service.Attach(in_server);
	return service.GetEnv();
}

/** Проверяет существует ли пользователь*/
bool IsUserExists(const std::wstring& in_user_name)
{	
	VIBService service;
	service.Attach();
	
	//
	ISC_STATUS status_vector[20];
	char request[255], *p=request;
	*p++ = isc_action_svc_display_user;	
	short spb_length= (short) (p - request);
	isc_service_start(status_vector, service.GetHandle(), NULL, spb_length, request);	
	if (status_vector[0]==1 && status_vector[1])
	{
		throw VIBException(&status_vector[0]);
	}

	//
	char request2[255], *p2=request2;
	char buffer[1024], *p3=buffer;
	memset(buffer, 0, 1024);
	*p2++ = isc_info_svc_get_users;
	isc_service_query(status_vector, service.GetHandle(), NULL, 0, 0, spb_length, request2, 1024, buffer);
	if (status_vector[0]==1 && status_vector[1])
	{
		throw VIBException(&status_vector[0]);		
	}
	p3+=3;
	while (*p3 !=isc_info_end)
	{
		if (*p3 == isc_spb_sec_userid)
		{			
			p3 += 5;
		}
		else if (*p3 == isc_spb_sec_groupid)
		{			
			p3 += 5;
		}
		else
		{
			unsigned short len = isc_vax_integer(p3+1, 2);
			switch (*p3)
			{
				case isc_spb_sec_username :
					if (strncmp(p3+3, wstring_to_string(in_user_name).c_str(), len)==0)
						return true;
					break;			
				default:
					break;
			}
			p3 += (3 + len);
		}
	}	
	return false;
}

/** Создание пользователя*/
void CreateUser(const std::wstring& in_user_name, const std::wstring& in_user_password)
{
	std::string user_name=wstring_to_string(in_user_name);
	std::string user_password=wstring_to_string(in_user_password);
	ISC_STATUS status_vector[20];
	USER_SEC_DATA user_sec_data;	
	memset(&user_sec_data, 0, sizeof(USER_SEC_DATA));
	user_sec_data.server = "localhost";
	user_sec_data.dba_user_name = ADMIN_USER;
	user_sec_data.dba_password = ADMIN_PASS;
	user_sec_data.protocol = sec_protocol_tcpip;
	user_sec_data.user_name = (char*)user_name.c_str();
	user_sec_data.password = (char*)user_password.c_str();
	user_sec_data.sec_flags = sec_server_spec | sec_password_spec
	| sec_dba_user_name_spec | sec_dba_password_spec;
	
	isc_add_user(status_vector, &user_sec_data);

	//Проверяем результат
	if (status_vector[0]==1 && status_vector[1])
	{	
		throw VIBException(&status_vector[0]);		
	}	
}

/** Возвращает true если значение поля NULL*/
bool is_null(const XSQLVAR& sqlvar)
{
	if ((sqlvar.sqltype & 1) && (*sqlvar.sqlind==-1))
	
	//(out_sqlda->sqlvar[i].sqltype & 1) && (*out_sqlda->sqlvar[i].sqlind==-1)

		return true;
	else
		return false;
}

/** Возвращает значение поля*/
wstring field_to_wstring(const XSQLVAR& sqlvar)
{
	wstring value;
	if (is_null(sqlvar))//(sqlvar.sqltype & 1) && (*sqlvar.sqlind==-1)
	{						
		//value=L"NULL";
		value=NULL_VALUE;
	}
	else
	switch (sqlvar.sqltype & ~1)
	{								
		case SQL_TEXT:
		{			
			sqlvar.sqldata[sqlvar.sqllen]=0;
			string str=string(sqlvar.sqldata);
			boost::trim(str);
			value=string_to_wstring(str);			
			break;
		}
		case SQL_VARYING:
			throw VSimpleException(L"Все поля SQL_VARYING должны быть преобразованы в SQL_TEXT.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		case SQL_SHORT:				
		{
			int16_t data;
			memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);											
			value=boost::str(boost::wformat(wstring(L"%d")) % data);
			break;				
		}
		case SQL_LONG:
		{
			int32_t data;
			memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);								
			value=boost::str(boost::wformat(wstring(L"%d")) % data);
			break;
		}
		case SQL_FLOAT:
		{
			float data;
			memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);
			value=boost::str(boost::wformat(wstring(L"%f")) % data);
			break;
		}
		case SQL_DOUBLE:
		{
			double data;
			memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);								
			value=boost::str(boost::wformat(wstring(L"%f")) % data);
			break;
		}
		case SQL_D_FLOAT:
		{			
			value=L"SQL_D_FLOAT";				
			break;
		}
		case SQL_TIMESTAMP:
		{
			ISC_TIMESTAMP * data=(ISC_TIMESTAMP*)sqlvar.sqldata;
			tm t;
			isc_decode_timestamp(data, &t);								
			wchar_t text[128];
			wcsftime(text, 128, L"%d.%m.%Y %H:%M", &t);	
			value=std::wstring(text);			
			break;
		}
		case SQL_BLOB:
		{	
			int64_t data;
			memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);
			value=boost::str(boost::wformat(wstring(L"BLOB(id=%d)")) % data);
			break;
		}
		case SQL_ARRAY:
		{			
			value=L"SQL_ARRAY";				
			break;
		}
		case SQL_QUAD:
		{			
			value=L"SQL_QUAD";				
			break;
		}
		case SQL_TYPE_TIME:
		{
			ISC_TIME * data=(ISC_TIME*)sqlvar.sqldata;
			tm t;
			isc_decode_sql_time(data, &t);			
			wchar_t text[64];
			wcsftime(text, 64, L"%H:%M", &t);	
			value=std::wstring(text);			
			break;
		}
		case SQL_TYPE_DATE:
		{
			ISC_DATE * data=(ISC_DATE*)sqlvar.sqldata;
			tm t;
			isc_decode_sql_date(data, &t);
			wchar_t text[64];
			wcsftime(text, 64, L"%d.%m.%Y", &t);			
			value=std::wstring(text);
			break;
		}
		case SQL_INT64:
		{
			int64_t data;
			memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);												
			value=boost::str(boost::wformat(wstring(L"%d")) % data);				
			break;
		}			
		default:
			throw VSimpleException(L"Тип данных не поддерживается.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	return value;
}

/** Возвращает значение поля как std::string*/
string field_to_string(const XSQLVAR& sqlvar)
{	
	string result=wstring_to_string(field_to_wstring(sqlvar));
	return result;
}

/** Возвращает значение поля как integer */
int64_t field_to_integer(const XSQLVAR & sqlvar)
{
	int64_t value;
	if (is_null(sqlvar))//(sqlvar.sqltype & 1) && (*sqlvar.sqlind==-1)
	{						
		value = 0;
	}
	else
	{
		switch (sqlvar.sqltype & ~1)
		{								
			case SQL_SHORT:				
			{
				int16_t data;
				memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);											
				value = data;
				break;				
			}
			case SQL_LONG:
			{
				int32_t data;
				memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);								
				value = data;
				break;
			}
			case SQL_INT64:
			{
				int64_t data;
				memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);												
				value = data;
				break;
			}			
			case SQL_FLOAT:
			{
				float data;
				memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);
				value = (int64_t)data;
				break;
			}
			case SQL_DOUBLE:
			{
				double data;
				memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);								
				value = (int64_t)data;
				break;
			}
			case SQL_TEXT:
			{
				sqlvar.sqldata[sqlvar.sqllen]=0;
				string str=string(sqlvar.sqldata);
				int temp = atoi(str.c_str());
				if(temp)
					value = (int64_t)temp;
				else
					throw VSimpleException(L"Невозможно конверитровать в int", boost::str(boost::wformat(L"[%d]") % sqlvar.sqltype), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				break;
			}
			default:
				throw VSimpleException(L"Тип данных не поддерживается.", 
					boost::str(boost::wformat(L"[%d]") % sqlvar.sqltype), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}
	return value;
}

/** Возвращает значение поля как float */
double field_to_float(const XSQLVAR & sqlvar)
{
	double value = 0.0;
	if (is_null(sqlvar))//(sqlvar.sqltype & 1) && (*sqlvar.sqlind==-1)
	{						
		value=0.0;
	}
	else
	{
		switch (sqlvar.sqltype & ~1)
		{			
			case SQL_SHORT:				
			{
				int16_t data;
				memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);											
				value = data;
				break;				
			}
			case SQL_LONG:
			{
				int32_t data;
				memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);								
				value = data;
				break;
			}
			case SQL_INT64:
			{
				int64_t data;
				memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);												
				value = (double)data;
				break;
			}			
			case SQL_FLOAT:
			{
				float data;
				memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);
				value = data;
				break;
			}
			case SQL_DOUBLE:
			{
				double data;
				memcpy(&data, sqlvar.sqldata, sqlvar.sqllen);								
				value = data;
				break;
			}
			default:
				throw VSimpleException(L"Тип данных не поддерживается.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}
	return value;
}

/** Возвращает значение поля как timestamp */
tm field_to_timestamp(const XSQLVAR & sqlvar)
{
	tm value = {0};
	if (is_null(sqlvar))
	{						
		//	
	}
	else
	{
		switch (sqlvar.sqltype & ~1)
		{								
			case SQL_TYPE_TIME:
			{
				ISC_TIME * data=(ISC_TIME*)sqlvar.sqldata;
				isc_decode_sql_time(data, &value);			
				break;
			}
			case SQL_TYPE_DATE:
			{
				ISC_DATE * data=(ISC_DATE*)sqlvar.sqldata;
				isc_decode_sql_date(data, &value);
				break;
			}
			case SQL_TIMESTAMP:
			{
				ISC_TIMESTAMP * data=(ISC_TIMESTAMP*)sqlvar.sqldata;
				isc_decode_timestamp(data, &value);								
				break;
			}
			default:
				throw VSimpleException(L"Тип данных не поддерживается.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}
	return value;
}

//----------------------------------- Методы класса VIBTransaction --------------------------------------


//-------------------------------------- Методы класса VIBDataHandlerSTD ---------------------------------

/** Подготовка обработчика*/
void VIBDataHandlerSTD::Prepare(XSQLDA * out_sqlda)
{
	//ToDo!!! Вывести заголовок таблицы
}

void VIBDataHandlerSTD::HandleData(XSQLDA * out_sqlda)
{
	for (short i=0; i<out_sqlda->sqld; i++)
	{
		cout << field_to_string(out_sqlda->sqlvar[i]).c_str();		
		if (i<out_sqlda->sqld-1)
			cout << ", ";
	}
	cout << endl;
}

//-------------------------------------- Методы класса VIBDataHandlerCSV ---------------------------------

/** Конструктор*/
VIBDataHandlerCSV::VIBDataHandlerCSV() : VIBDataHandler(), show_header(true)
{
}

/** Подготовка обработчика*/
void VIBDataHandlerCSV::Prepare(XSQLDA * out_sqlda)
{
	if (show_header)
	{
		for (short i=0; i<out_sqlda->sqld; i++)
		{
			out << out_sqlda->sqlvar[i].aliasname;
			if (i<out_sqlda->sqld-1)
				out << ", ";
		}				
		out << endl;
	}
}

/** Функция обработчика*/
void VIBDataHandlerCSV::HandleData(XSQLDA * out_sqlda)
{	
	for (short i=0; i<out_sqlda->sqld; i++)
	{
		out << field_to_string(out_sqlda->sqlvar[i]).c_str();		
		if (i<out_sqlda->sqld-1)
			out << ", ";
	}
	out << endl;	
}

/** Открывает выходной поток*/
void VIBDataHandlerCSV::Open(const char * const in_file_name)
{			
	if (out.is_open())
		out.close();
	out.open(in_file_name, ios_base::trunc);
	if (!out.is_open())
		throw VSimpleException(L"Не удалось открыть файл.", string_to_wstring(in_file_name), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
}

/** Закрывает выходной поток*/	
void VIBDataHandlerCSV::Close()
{
	out.close();
}

/** Устанавливает признак показывать/не показывать заголовок*/
void VIBDataHandlerCSV::SetShowHeader(bool in_show_header)
{
	show_header=in_show_header;
}

/** Возвращает признак показывать/не показывать заголовок*/	
bool VIBDataHandlerCSV::GetShowHeader() const
{
	return show_header;
}

//-------------------------------------- Методы класса VIBDataHandlerXML ---------------------------------

/** Подготовка обработчика*/
void VIBDataHandlerXML::Prepare(XSQLDA * out_sqlda)
{
	out << "<header>";
	for (short i=0; i<out_sqlda->sqld; i++)
	{
		out << "<column value=\"" << out_sqlda->sqlvar[i].aliasname << "\"/>";
	}
	out << "</header>" << endl;
}

/** Функция обработчика*/
void VIBDataHandlerXML::HandleData(XSQLDA * out_sqlda)
{	
	out << "<row>";
	for (short i=0; i<out_sqlda->sqld; i++)
	{
		out << "<cell value=\""  << field_to_string(out_sqlda->sqlvar[i]).c_str() << "\" />";		
	}
	out << "</row>" << endl;	
}

/** Открывает выходной поток*/
void VIBDataHandlerXML::Open(const char * const in_file_name)
{	
	if (out.is_open())
		out.close();
	out.open(in_file_name, ios_base::trunc);
	if (!out.is_open())
		throw VSimpleException(L"Не удалось открыть файл.", string_to_wstring(in_file_name), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	out << "<?xml version=\"1.0\" encoding=\"windows-1251\"?>" << endl;
	out << "<report>" << endl;
}

/** Закрывает выходной поток*/	
void VIBDataHandlerXML::Close()
{
	out << "</report>" << endl;
	out.close();
}

//------------------------------------- Методы класса VBlob ----------------------------------------

/** Конструктор*/
VBlob::VBlob()
{
	size=0;
};

/** Конструктор с загрузкой из потока*/
VBlob::VBlob(std::istream& in_stream)
{
	in_stream.seekg(0, ios_base::end);
	size=in_stream.tellg();	
	data=boost::shared_array<char>(new char[size]);	
	in_stream.seekg(0, ios_base::beg);
	in_stream.read(data.get(), size);
}

/** Возвращает указатель на const данные*/
const char * VBlob::GetConstData() const
{
	return data.get();
};	

/** Возвращает указатель на данные*/
char * VBlob::GetData()
{
	return data.get();
};	

/** Возвращает данные закодированные в base64*/
void VBlob::Base64Encode(wstring& out_data)
{			
	const std::wstring alpha(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
	const wchar_t suffixe=L'=';	
	size_t string_length=(size*4)/3+3;
	out_data.reserve(string_length);
	out_data.resize(string_length);	
	for (size_t i=0; i<size; i+=3)
	{
		size_t j=(i/3)*4;
		uint32_t byte1=((uint32_t)data[i] & 0xff) << 24;
		uint32_t byte2=(i+1<size) ? ((uint32_t)data[i+1] & 0xff) << 16 : 0;
		uint32_t byte3=(i+2<size) ? ((uint32_t)data[i+2] & 0xff) << 8 : 0;		
		uint32_t buffer=byte1 | byte2 | byte3;				
		uint32_t index1=(buffer >> 26) & 0x3f;
		uint32_t index2=(buffer >> 20) & 0x3f;
		uint32_t index3=(buffer >> 14) & 0x3f;
		uint32_t index4=(buffer >> 8) & 0x3f;		
		out_data[j++]=alpha[index1];		
		out_data[j++]=alpha[index2];		
		(i+1<size) ? out_data[j++]=alpha[index3] : out_data[j++]=suffixe;		
		(i+2<size) ? out_data[j++]=alpha[index4] : out_data[j++]=suffixe;
	}	
}

/** Декодирует из base64*/
void VBlob::Base64Decode(const wstring& in_data)
{
	const std::wstring alpha(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");		
	const wchar_t suffixe=L'=';
	std::deque<unsigned char> _data;	
	for (size_t i=0; i<in_data.size(); i+=4)
	{
		uint32_t index1=alpha.find(in_data[i]) << 26;
		uint32_t index2=alpha.find(in_data[i+1]) << 20;
		uint32_t index3=(i+2<in_data.size()) && in_data[i+2]!=suffixe ? alpha.find(in_data[i+2]) << 14 : 0;
		uint32_t index4=(i+3<in_data.size()) && in_data[i+3]!=suffixe ? alpha.find(in_data[i+3]) << 8 : 0;
		uint32_t index=index1 | index2 | index3 | index4;
		uint32_t byte1=(index >> 24);
		uint32_t byte2=(index >> 16);
		uint32_t byte3=(index >> 8);
		_data.push_back((unsigned char)byte1);
		if ((i+2<in_data.size()) && in_data[i+2]!=suffixe)
			_data.push_back((unsigned char)byte2);
		if ((i+3<in_data.size()) && in_data[i+3]!=suffixe)
			_data.push_back((unsigned char)byte3);
	}	
	size=_data.size();
	data.reset(new char[size]);	
	for (size_t i=0;i<size;++i)
	{
		data[i]=_data.at(i);
	}
}

/** Возвращает размер данных*/
size_t VBlob::GetSize() const
{
	return size;
}

/** Записывает данные в файл*/
void VBlob::Save(const wchar_t * in_filename) const
{
	ofstream file(in_filename, ios::binary);
	if (file.is_open())
	{
		file.write(data.get(), size);
		file.close();
	}
	else
	{
		throw VSimpleException(L"Не удалось записать BLOB в файл.", in_filename, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

/** Резервирует память*/
void VBlob::Reserve(size_t in_size)
{
	size=in_size;
	data.reset(new char[size]);	
}

//-------------------------------------- Методы класса VIBParameter -------------------------------

/** Вычисляет размер по типу данных*/
short VIBParameter::AutoSize(short in_type, const char* in_data) const
{
	switch(in_type)
	{
		case SQL_TEXT:
		{
			return strlen(in_data);
			break;
		}
		case SQL_VARYING:			
			throw VSimpleException(L"Все поля SQL_VARYING должны быть преобразованы в SQL_TEXT.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		case SQL_SHORT:				
		{
			return sizeof(int16_t);					
			break;				
		}
		case SQL_LONG:
		{
			return sizeof(int32_t);					
			break;
		}
		case SQL_FLOAT:
		{
			return sizeof(float);
			break;
		}
		case SQL_DOUBLE:
		{
			return sizeof(double);					
			break;
		}
		case SQL_D_FLOAT:
		{			
			//ToDo!!!
			throw VSimpleException(L"Тип данных не поддерживается.", L"SQL_D_FLOAT", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			break;
		}
		case SQL_TIMESTAMP:
		{
			return sizeof(ISC_TIMESTAMP);;					
			break;
		}
		case SQL_BLOB:
		case SQL_BLOB+1:
		{	
			return sizeof(ISC_QUAD);
			break;
		}
		case SQL_ARRAY:
		{			
			//ToDo!!!
			throw VSimpleException(L"Тип данных не поддерживается.", L"SQL_ARRAY", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			break;
		}
		case SQL_QUAD:
		{			
			return sizeof(ISC_QUAD);
			break;
		}
		case SQL_TYPE_TIME:
		{
			return sizeof(ISC_TIME);				
			break;
		}
		case SQL_TYPE_DATE:
		{
			return sizeof(ISC_DATE);			
			break;
		}
		case SQL_INT64:
		{
			return sizeof(int64_t);
			break;
		}
		default:
			throw VSimpleException(L"Тип данных не поддерживается.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

/** Конструктор*/
VIBParameter::VIBParameter()
{
	memset(this, 0, sizeof(VIBParameter));
}

/** Конструктор*/
VIBParameter::VIBParameter(short in_type, const char* in_data, short in_size)
{
	memset(&sqlvar, 0, sizeof(XSQLVAR));	
	sqlvar.sqltype = in_type;
	if (in_size>0)
		sqlvar.sqllen = in_size;
	else//Вычисляем размер данных по типу
	{
		sqlvar.sqllen=AutoSize(in_type, in_data);
	}	

	//Выделяем память под данные
	if ((sqlvar.sqltype & ~1)==SQL_TEXT)
	{
		data=(char*)malloc(sqlvar.sqllen+2);
		memset(data, 0, sqlvar.sqllen+2);
		memcpy(data, in_data, sqlvar.sqllen+2);
	}
	else if ((sqlvar.sqltype & ~1)==SQL_TIMESTAMP)
	{
		data=(char*)malloc(sqlvar.sqllen);
		memset(data, 0, sqlvar.sqllen);
		isc_encode_timestamp((const void*)in_data,(ISC_TIMESTAMP*)data);
	}
	else
	{
		data=(char*)malloc(sqlvar.sqllen);
		memset(data, 0, sqlvar.sqllen);
		memcpy(data, in_data, sqlvar.sqllen);
	}
	

	sqlvar.sqldata = data;
	ind=0;
	sqlvar.sqlind = &ind;
}

/** Конструктор*/
VIBParameter::VIBParameter(XSQLVAR& in_sqlvar)
{
	memset(&sqlvar, 0, sizeof(XSQLVAR));	
	sqlvar=in_sqlvar;	
	//Выделяем память под данные
	if ((sqlvar.sqltype & ~1)==SQL_TEXT)
	{
		data=(char*)malloc(sqlvar.sqllen+2);
		memset(data, 0, sqlvar.sqllen+2);
		memcpy(data, in_sqlvar.sqldata, sqlvar.sqllen+2);
	}
	else
	{
		data=(char*)malloc(sqlvar.sqllen);
		memset(data, 0, sqlvar.sqllen);
		memcpy(data, in_sqlvar.sqldata, sqlvar.sqllen);
	}
	
	sqlvar.sqldata = data;	

	ind=in_sqlvar.sqlind!=NULL ? *in_sqlvar.sqlind : 0;
	sqlvar.sqlind = &ind;	
}

/** Конструктор копирования*/
VIBParameter::VIBParameter(const VIBParameter& in_parameter)
{
	data=NULL;	
	*this=in_parameter;
}

/** Деструктор*/
VIBParameter::~VIBParameter()
{
	free(data);
}

/** Возвращает XSQLVAR*/
const XSQLVAR * VIBParameter::SqlVar() const
{
	return &sqlvar;
}

/** Возвращает представление в виде строки*/
wstring VIBParameter::GetString() const
{
	return field_to_wstring(sqlvar);
}

/** Возвращает представление в виде целого */
int64_t VIBParameter::GetInteger() const
{
	return field_to_integer(sqlvar);
}

/** Возвращает представление в виде float */
double VIBParameter::GetFloat() const
{
	return field_to_float(sqlvar);
}

/** Возвращает представление в виде timestamp */
tm VIBParameter::GetTimestamp() const
{
	return field_to_timestamp(sqlvar);
}

/** Возвращает имя поля*/
wstring VIBParameter::GetFieldName() const
{
	return string_to_wstring(string(sqlvar.sqlname));
}

/** Возвращает имя таблицы*/
wstring VIBParameter::GetRelationName() const
{
	return string_to_wstring(string(sqlvar.relname));
}

/** Возвращает владельца*/
wstring VIBParameter::GetOwnerName() const
{
	return string_to_wstring(string(sqlvar.ownname));
}

/** Возвращает псевдоним*/
wstring VIBParameter::GetAliasName() const
{
	return string_to_wstring(string(sqlvar.aliasname));
}

/** Возвращает тип*/
short VIBParameter::GetType() const
{
	return sqlvar.sqltype;
}

/** Возвращает true, если значение параметра NULL*/
bool VIBParameter::IsNull() const
{
	return is_null(sqlvar);
}

/** Оператор присваивания*/
VIBParameter& VIBParameter::operator=(const VIBParameter& in_parameter)
{	
	free(data);
	memset(&sqlvar, 0, sizeof(XSQLVAR));	
	sqlvar = in_parameter.sqlvar;		
	//Выделяем память под данные
	if ((sqlvar.sqltype & ~1)==SQL_TEXT)
	{
		data=(char*)malloc(sqlvar.sqllen+2);
		memset(data, 0, sqlvar.sqllen+2);
		memcpy(data, in_parameter.data, sqlvar.sqllen+2);
	}
	else
	{
		data=(char*)malloc(sqlvar.sqllen);
		memset(data, 0, sqlvar.sqllen);
		memcpy(data, in_parameter.data, sqlvar.sqllen);
	}
	sqlvar.sqldata = data;	
	ind=in_parameter.ind;
	sqlvar.sqlind = &ind;
	return *this;
}

//-------------------------------------- Методы класса VIBDataHandlerParameter ---------------------

/// Подготовка обработчика
void VIBDataHandlerParameter::Prepare(XSQLDA * out_sqlda)
{	
	data.clear();
	aliases.clear();
	colcount=out_sqlda->sqld;
	for (short i=0; i<out_sqlda->sqld; i++)
	{				
		aliases.insert(boost::algorithm::to_lower_copy(string_to_wstring(out_sqlda->sqlvar[i].aliasname)));
	}
}

/// Функция обработчика
void VIBDataHandlerParameter::HandleData(XSQLDA * out_sqlda)
{
	vector<VIBParameter> row;
	row.reserve(out_sqlda->sqld);
	row.resize(out_sqlda->sqld);
	for (short i=0; i<out_sqlda->sqld; i++)
	{		
		row[i]=VIBParameter(out_sqlda->sqlvar[i]);				
	}
	data.push_back(row);
}
/// Возвращает параметр по индексам
const VIBParameter& VIBDataHandlerParameter::GetParameter(size_t row, size_t col) const
{
	return data.at(row).at(col);
}

/// Возвращает true если поле есть в запросе
bool VIBDataHandlerParameter::HasField(const wstring& in_field) const
{		
	return aliases.find(boost::algorithm::to_lower_copy(in_field))!=aliases.end();
}

/// Возвращает параметр по имени поля
const VIBParameter& VIBDataHandlerParameter::GetParameter(size_t in_row, const wstring& in_field) const
{
	for (size_t i=0; i<data.at(in_row).size(); ++i)
	{					
		wstring aliasname=data.at(in_row).at(i).GetAliasName();
		if (boost::algorithm::iequals(aliasname, in_field))
		{
			return data.at(in_row).at(i);
		}
	}
	throw VSimpleException(L"Не удалось получить значение поля.", in_field, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
}

/// Возвращает число строк
size_t VIBDataHandlerParameter::GetRowCount() const
{
	return data.size();
};

/// Возвращает число столбцов
size_t VIBDataHandlerParameter::GetColCount() const
{
	return colcount;
};

/// Выделить память
void AllocData(XSQLDA * out_sqlda)
{
	for (short i=0; i<out_sqlda->sqld; i++)
	{
		//В зависивости от типа
		switch (out_sqlda->sqlvar[i].sqltype & ~1)
		{
			case SQL_VARYING:
				//Пока не совсем понятно нужно ли такое преобразование, однако, иначе, 
				//не удалось получить значения поля типа SQL_VARYING с помощью isc_dsql_fetch
				out_sqlda->sqlvar[i].sqltype=SQL_TEXT;
				out_sqlda->sqlvar[i].sqldata=(char *)malloc(out_sqlda->sqlvar[i].sqllen+2);
				break;
			case SQL_TEXT:
				//UDF, которая возвращает CSTRING приводит к ошибке, если не добавить два байта
				out_sqlda->sqlvar[i].sqldata=(char *)malloc(out_sqlda->sqlvar[i].sqllen+2);
				break;
			default:
				out_sqlda->sqlvar[i].sqldata=(char *)malloc(out_sqlda->sqlvar[i].sqllen);
				break;			
		}
		if (out_sqlda->sqlvar[i].sqltype & 1)
		{
			out_sqlda->sqlvar[i].sqlind=(short*)malloc(sizeof(short));
		}
	}
}

/// Освободить память
void FreeData(XSQLDA * out_sqlda)
{
	if (out_sqlda!=NULL)
	for (short i=0; i<out_sqlda->sqld; i++)
	{		
		free(out_sqlda->sqlvar[i].sqldata);
		out_sqlda->sqlvar[i].sqldata=NULL;
		if (out_sqlda->sqlvar[i].sqltype & 1)
		{
			free(out_sqlda->sqlvar[i].sqlind);
		}
	}
}

//-------------------------------------- Методы класса VIBDatabase ---------------------------------

/** Конструктор*/
VIBDatabase::VIBDatabase()
{	
	user="";
	password="";	
	db_handle=0;	
	monitor_running=false;
}

/** Деструктор*/
VIBDatabase::~VIBDatabase()
{
	try
	{		
		DisconnectDB();
	}
	catch (...)
	{
	}
}

/** Соединение с БД*/
void VIBDatabase::Connect(const char * const in_server_name, const char * const in_database_name, const char * const in_user, const char * const in_password)
{		
	if (Connected())
	{
		DisconnectDB();			
	}
	if (in_server_name==NULL || in_database_name==NULL || in_user==NULL || in_password==NULL)
		throw NULL_POINTER;
	if (strlen(in_user)>128)
		throw VSimpleException(L"Длина имени пользователя превышает 128 символов.", string_to_wstring(in_user), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);	
	if (strlen(in_password)>32)
		throw VSimpleException(L"Длина пароля превышает 32 символов.", string_to_wstring(in_password), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);	
	server_name=string(in_server_name);
	database_name=string(in_database_name);
	user=string(in_user);
	password=string(in_password);
	db_handle=0;	
	char dpb_buffer[256];
	char *dpb;
	short dpb_length;
	// Формируем dbp структуру
	dpb = dpb_buffer;
	*dpb++ = isc_dpb_version1;
	*dpb++ = isc_dpb_user_name;
	// Размер имени пользователя
	*dpb++ = static_cast<unsigned char>(user.size());
	// Копируем имя пользователя
	for(size_t i = 0; i<user.size(); i++)
		*dpb++ = user[i];
	*dpb++ = isc_dpb_password;
	// Размер пароля
	*dpb++ = static_cast<unsigned char>(password.size());
	// Копируем пароль
	for(size_t i = 0; i<password.size(); i++)
		*dpb++ = password[i];
	// Вычисляем размер структуры
	dpb_length = (short)(dpb - dpb_buffer);		
	//Соединяемся с БД	
	isc_attach_database(status_vector, static_cast<short>(GetConnectString().size()), (char*)GetConnectString().c_str(), 
		&db_handle, dpb_length, dpb_buffer);
	//Проверяем результат
	if (status_vector[0]==1 && status_vector[1])
	{		
		throw VSimpleException(L"Не удалось соединиться с базой.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	LOG_INFO(boost::str(boost::wformat(L"Соединение с БД: %s Версия сервера: %s") 
		% string_to_wstring(GetConnectString())	% GetServerVersion(in_server_name)));
}

/** Создание БД*/
void VIBDatabase::Create(const char * const in_server_name, const char * const in_database_name, const char * const in_user, const char * const in_password)
{
	if (Connected())
	{
		DisconnectDB();			
	}
	if (in_server_name==NULL || in_database_name==NULL || in_user==NULL || in_password==NULL)
		throw NULL_POINTER;
	if (strlen(in_user)>128)
		throw VSimpleException(L"Длина имени пользователя превышает 128 символов.", string_to_wstring(in_user), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	if (strlen(in_password)>32)
		throw VSimpleException(L"Длина пароля превышает 32 символов.", string_to_wstring(in_password), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	server_name=string(in_server_name);
	database_name=string(in_database_name);
	user=string(in_user);
	password=string(in_password);
	db_handle=0;

	//Создаем БД
	string statement=string("create database '" + GetConnectString() +
		string("' user '" + user +
		string("' password '") + password + string("'")));
	isc_tr_handle transaction(NULL);	
	isc_dsql_execute_immediate(status_vector, &db_handle, &transaction, 0, (char*)statement.c_str(), dialect, NULL);
	//Проверяем результат
	if (status_vector[0]==1 && status_vector[1])
	{		
		throw VSimpleException(L"Не удалось создать БД.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);		
	}
}

/** Разрыв соединения*/
void VIBDatabase::DisconnectDB()
{
	if (!Connected())
		return;

	StopMonitor();

	user="";
	password="";	
	//Откатываем транзакцию по умолчанию
	if (default_transaction.Active())
		RollbackTransaction(default_transaction);
	//Откатываем незакрытые транзакции
	for (std::set<isc_tr_handle>::iterator i=transactions.begin(); i!=transactions.end(); ++i)
	{			
		isc_tr_handle hnd(*i);
		isc_rollback_transaction(status_vector, &hnd);
		//Проверяем результат
		if (status_vector[0]==1 && status_vector[1])
		{		
			throw VSimpleException(L"Не удалось отключиться от базы.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}
	transactions.clear();

	isc_detach_database(status_vector, &db_handle);
	//Проверяем результат
	if (status_vector[0]==1 && status_vector[1])
	{		
		throw VSimpleException(L"Не удалось отключиться от базы.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}		
}

/** Возвращает код ошибки*/
long VIBDatabase::GetErrorCode()
{
	return status_vector[1];
}

/** Возвращает имя ограничения, которое вызвало ошибку*/
string VIBDatabase::GetErrorConstraint()
{
	return std::string((char*)(status_vector[3]));
}

/** Возвращает описание ошибки*/
string VIBDatabase::GetErrorDescription()
{			
	return ParseStatusVector(status_vector);
}

/** Возвращает SQL код ошибки*/
long VIBDatabase::GetSQLErrorCode()
{
	return isc_sqlcode(status_vector);
}

/** Возвращает признак соединения с БД*/
bool VIBDatabase::Connected()
{
	return db_handle!=NULL;
}

/** Начало транзакции*/
VIBTransaction VIBDatabase::StartTransaction()
{	
	if (!Connected())
	{
		throw VSimpleException(L"Нет соединения с базой данных.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	VIBTransaction transaction;	
	// Начинаем транзакцию
	isc_start_transaction(status_vector, &(transaction.transaction_handle), 1, &db_handle, 0, NULL);	
	//Проверяем результат
	if (status_vector[0]==1 && status_vector[1])
	{		
		throw VSimpleException(L"Не удалось начать транзакцию.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
	//Помещаем транзакцию в список
	transactions.insert(transaction.transaction_handle);
	return transaction;
}


/** Откат транзакции*/
void VIBDatabase::RollbackTransaction(VIBTransaction& in_transaction)
{
	if (!Connected())
		throw VSimpleException(L"Нет соединения с базой данных.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);	
	if (in_transaction.transaction_handle==NULL)
		throw VSimpleException(L"Неверный дескриптор транзакции.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);	
	//Удаляем транзакцию из списка	
	transactions.erase(in_transaction.transaction_handle);
	isc_rollback_transaction(status_vector, &in_transaction.transaction_handle);
	//Проверяем результат
	if (status_vector[0]==1 && status_vector[1])
	{		
		throw VSimpleException(L"Не удалось откатить транзакцию.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
	in_transaction.transaction_handle=NULL;
	in_transaction.has_changes=false;
}

/** Откат транзакции по умолчанию*/
void VIBDatabase::RollbackTransaction()
{
	RollbackTransaction(default_transaction);
}

/** Подтверждение транзакции*/
void VIBDatabase::CommitTransaction(VIBTransaction& in_transaction)
{
	if (!Connected())	
		throw VSimpleException(L"Нет соединения с базой данных.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);	
	if (in_transaction.transaction_handle==NULL)
		throw VSimpleException(L"Неверный дескриптор транзакции.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	//Удаляем транзакцию из списка	
	transactions.erase(in_transaction.transaction_handle);
	isc_commit_transaction(status_vector, &in_transaction.transaction_handle);
	//Проверяем результат
	if (status_vector[0]==1 && status_vector[1])
	{		
		throw VSimpleException(L"Не удалось подтвердить транзакцию.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
	in_transaction.transaction_handle=NULL;
	in_transaction.has_changes=false;
}

/** Подтверждение транзакции по умолчанию*/
void VIBDatabase::CommitTransaction()
{
	CommitTransaction(default_transaction);
}

/** Немедленное выполнение SQL-запроса*/
void VIBDatabase::ExecuteStatementImmediate(VIBTransaction& in_transaction, const char * in_statement)
{
	QueryProfiler profiler(string_to_wstring(in_statement));
	//Проверяем корректность входных параметров
	if (in_statement==NULL)
	{
		throw NULL_POINTER;
	}
	if (strlen(in_statement)>MAX_STATEMENT_LENGTH)
	{
		throw STATEMENT_TOO_LONG;
	}
	if (!in_transaction.Active())
	{
		throw VSimpleException(L"Нет активной транзакции.", string_to_wstring(in_statement), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}		
    //Выполняем SQL-запрос
	isc_dsql_execute_immediate(status_vector, &db_handle, &(in_transaction.transaction_handle), 0, in_statement, dialect, NULL);    	
	//Проверяем результат
	if (status_vector[0]==1 && status_vector[1])
	{	
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
	in_transaction.has_changes=true;
}

/** Немедленное выполнение запроса с параметрами*/
void VIBDatabase::ExecuteStatementImmediate(VIBTransaction& in_transaction, const char * in_statement, const vector<VIBParameter>& in_parameters)
{
	QueryProfiler profiler(string_to_wstring(in_statement));
	//Проверяем корректность входных параметров
	if (in_statement==NULL)
	{
		throw NULL_POINTER;
	}
	if (strlen(in_statement)>MAX_STATEMENT_LENGTH)
	{
		throw STATEMENT_TOO_LONG;
	}
	if (!in_transaction.Active())
	{
		throw VSimpleException(L"Нет активной транзакции.", string_to_wstring(in_statement), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	

	boost::scoped_array<char> in_sqlda_buffer(new char[XSQLDA_LENGTH(in_parameters.size())]);
	XSQLDA * in_sqlda=(XSQLDA*)in_sqlda_buffer.get();
	memset(in_sqlda, 0, XSQLDA_LENGTH(in_parameters.size()));
	in_sqlda->version = SQLDA_VERSION1;
	in_sqlda->sqln = in_parameters.size();
	in_sqlda->sqld = in_parameters.size();
	
	const vector<VIBParameter> & parameters=in_parameters;
	for (size_t i=0;i<parameters.size();++i)
	{		
		XSQLVAR var=*(parameters.at(i).SqlVar());
		in_sqlda->sqlvar[i]=var;
	}	
	
	//Выполняем SQL
	isc_dsql_execute_immediate(status_vector, &db_handle, &in_transaction.transaction_handle, 0, in_statement, dialect, in_sqlda);	
	if (status_vector[0]==1 && status_vector[1])
	{					
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	//ToDo!!! Проверяем количество изменившихся записей с помощью isc_dsql_sql_info() и isc_info_sql_records (см. http://www.firebirdfaq.org/faq300/)
	in_transaction.has_changes=true;
}

/** Выполнение запроса с подготовкй*/
void VIBDatabase::ExecuteStatement(VIBTransaction& in_transaction, const char * in_statement)
{		
	QueryProfiler profiler(string_to_wstring(in_statement));
	//Проверяем корректность входных параметров
	if (in_statement==NULL)
	{
		throw NULL_POINTER;
	}
	if (strlen(in_statement)>MAX_STATEMENT_LENGTH)
	{
		throw STATEMENT_TOO_LONG;
	}
	if (!in_transaction.Active())
	{
		throw VSimpleException(L"Нет активной транзакции.", string_to_wstring(in_statement), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
				
	//Указатель на SQL-запрос    
	isc_stmt_handle stmt_handle = NULL;
	//Результат перебора данных    
	boost::shared_array<char> out_sqlda_buffer(new char[XSQLDA_LENGTH(1)]);
	XSQLDA* out_sqlda=(XSQLDA*)out_sqlda_buffer.get();	
	out_sqlda->version=SQLDA_VERSION1;
	out_sqlda->sqln=1;
	//Выделяем память под SQL запрос
	isc_dsql_allocate_statement(status_vector, &db_handle, &stmt_handle);	
	if (status_vector[0]==1 && status_vector[1])
	{				
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}		
	//Подготавливаем SQL запрос
	isc_dsql_prepare(status_vector, &in_transaction.transaction_handle, &stmt_handle, 0, in_statement, SQL_DIALECT_CURRENT, out_sqlda);
	if (status_vector[0]==1 && status_vector[1])
	{					
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	//Получаем информацию о запросе
	char type_item[]={isc_info_sql_stmt_type};
	char res_buffer[8];
	isc_dsql_sql_info(status_vector, &stmt_handle, sizeof(type_item), type_item, sizeof(res_buffer), res_buffer);
	if (status_vector[0]==1 && status_vector[1])
	{					
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	int statement_type;
	if (res_buffer[0]==isc_info_sql_stmt_type)
	{
		int length=isc_vax_integer(&(res_buffer[1]), 2);
		statement_type=isc_vax_integer(&(res_buffer[3]), length);		
	}
	//В зависимости от типа запроса
	switch (statement_type)
	{
		case isc_info_sql_stmt_select://Запрос SELECT
		{			
			//Не обрабатывается
			break;
		}
		case isc_info_sql_stmt_insert:
		case isc_info_sql_stmt_update:
		case isc_info_sql_stmt_delete:			
		default://Все прочие запросы
		{			
			//Выполняем SQL запрос
			isc_dsql_execute(status_vector, &in_transaction.transaction_handle, &stmt_handle, dialect, NULL);
			if (status_vector[0]==1 && status_vector[1])
			{															
				//throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				throw VIBException(&status_vector[0]);
			}			
			in_transaction.has_changes=true;
			//ToDo!!! Проверяем количество изменившихся записей с помощью isc_dsql_sql_info() и isc_info_sql_records (см. http://www.firebirdfaq.org/faq300/)			
			break;
		}
	}	
	//Освобождаем память из под SQL запроса
	isc_dsql_free_statement(status_vector, &stmt_handle, DSQL_drop);
	if (status_vector[0]==1 && status_vector[1])
	{															
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

/** Выполнение запроса с подготовкой*/
void VIBDatabase::ExecuteStatement(VIBTransaction& in_transaction, const char * in_statement, VIBDataHandler& in_data_handler)
{
	QueryProfiler profiler(string_to_wstring(in_statement));
	//Проверяем корректность входных параметров
	if (in_statement==NULL)
	{
		throw NULL_POINTER;
	}
	if (strlen(in_statement)>MAX_STATEMENT_LENGTH)
	{
		throw STATEMENT_TOO_LONG;
	}
	if (!in_transaction.Active())
	{
		throw VSimpleException(L"Нет активной транзакции.", string_to_wstring(in_statement), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
		
	//Указатель на SQL-запрос
	isc_stmt_handle stmt_handle = NULL;
	//Результат перебора данных

	boost::shared_array<char> out_sqlda_buffer(new char[XSQLDA_LENGTH(1)]);
	XSQLDA* out_sqlda=(XSQLDA*)out_sqlda_buffer.get();
	memset(out_sqlda, 0, XSQLDA_LENGTH(1));
	out_sqlda->version=SQLDA_VERSION1;
	out_sqlda->sqln=1;
	//Выделяем память под SQL запрос
	isc_dsql_allocate_statement(status_vector, &db_handle, &stmt_handle);
	if (status_vector[0]==1 && status_vector[1])
	{				
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}		
	//Подготавливаем SQL запрос
	isc_dsql_prepare(status_vector, &in_transaction.transaction_handle, &stmt_handle, 0, in_statement, SQL_DIALECT_CURRENT, out_sqlda);
	if (status_vector[0]==1 && status_vector[1])
	{					
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	//Получаем информацию о запросе
	char type_item[]={isc_info_sql_stmt_type};
	char res_buffer[8];
	isc_dsql_sql_info(status_vector, &stmt_handle, sizeof(type_item), type_item, sizeof(res_buffer), res_buffer);
	if (status_vector[0]==1 && status_vector[1])
	{					
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	int statement_type;
	if (res_buffer[0]==isc_info_sql_stmt_type)
	{
		int length=isc_vax_integer(&(res_buffer[1]), 2);
		statement_type=isc_vax_integer(&(res_buffer[3]), length);		
	}
	//В зависимости от типа запроса
	switch (statement_type)
	{
		case isc_info_sql_stmt_select://Запрос SELECT
		{
			//Запрашиваем информацию о столбцах
			isc_dsql_describe(status_vector, &stmt_handle, 1, out_sqlda);
			if (status_vector[0]==1 && status_vector[1])
			{								
				throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			//Проверяем число столбцов и корректируем XSQLDA
			if (out_sqlda->sqld>out_sqlda->sqln)
			{
				short column_count=out_sqlda->sqld;				
				out_sqlda_buffer.reset(new char[XSQLDA_LENGTH(column_count)]);
				out_sqlda=(XSQLDA*)out_sqlda_buffer.get();
				memset(out_sqlda, 0, XSQLDA_LENGTH(column_count));
				out_sqlda->version=SQLDA_VERSION1;
				out_sqlda->sqln=column_count;		
				// Запрашиваем информацию о столбцах
				isc_dsql_describe(status_vector, &stmt_handle, 1, out_sqlda);
				if (status_vector[0]==1 && status_vector[1])
				{													
					throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				}
			}						
			//Выполняем SQL запрос
			isc_dsql_execute(status_vector, &in_transaction.transaction_handle, &stmt_handle, dialect, NULL);
			if (status_vector[0]==1 && status_vector[1])
			{							
				throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			//Формируем XSQLDA - выделяем память под данные
			AllocData(out_sqlda);
BOOST_SCOPE_EXIT((out_sqlda))
{
	FreeData(out_sqlda);
}BOOST_SCOPE_EXIT_END
			//Обрабатываем данные
			long fetch_stat;
			if (out_sqlda->sqld>0)
			{				
				//Устанавливаем параметры обработчика
				in_data_handler.SetParameters(this, &in_transaction);
				//Вызываем обработчик запроса первый раз для того чтобы построить заголовок таблицы
				in_data_handler.Prepare(out_sqlda);
				//Перебираем полученные записи	
				while((fetch_stat = isc_dsql_fetch(status_vector, &stmt_handle, 1, out_sqlda)) == 0)
				{											
					//Вызываем обработчик запроса					
					in_data_handler.HandleData(out_sqlda);
				}    
				if(fetch_stat != 100L)
				{											
					throw VSimpleException(L"Не удалось получить результаты запроса.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				}
			}					
			break;
		}
		case isc_info_sql_stmt_insert:
		case isc_info_sql_stmt_update:
		case isc_info_sql_stmt_delete:			
		default://Все прочие запросы
		{			
			//Выполняем SQL запрос
			isc_dsql_execute(status_vector, &in_transaction.transaction_handle, &stmt_handle, dialect, NULL);
			if (status_vector[0]==1 && status_vector[1])
			{															
				throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			in_transaction.has_changes=true;
			break;
		}
	}	
	//Освобождаем память из под SQL запроса
	isc_dsql_free_statement(status_vector, &stmt_handle, DSQL_drop);
	if (status_vector[0]==1 && status_vector[1])
	{															
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

/** Выполнение запроса с подготовкй с параметрами*/
void VIBDatabase::ExecuteStatement(VIBTransaction& in_transaction, const char * in_statement, const vector<VIBParameter>& in_parameters)
{
	QueryProfiler profiler(string_to_wstring(in_statement));
	//Проверяем корректность входных параметров
	if (in_statement==NULL)
	{
		throw NULL_POINTER;
	}
	if (strlen(in_statement)>MAX_STATEMENT_LENGTH)
	{
		throw STATEMENT_TOO_LONG;
	}
	if (!in_transaction.Active())
	{
		throw VSimpleException(L"Нет активной транзакции.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
				
	//Указатель на SQL-запрос    
	isc_stmt_handle stmt_handle = NULL;	  
	//Входные параметры	
	boost::scoped_array<char> in_sqlda_buffer(new char[XSQLDA_LENGTH(in_parameters.size())]);
	XSQLDA * in_sqlda = (XSQLDA*)in_sqlda_buffer.get();
	memset(in_sqlda, 0, XSQLDA_LENGTH(in_parameters.size()));
	in_sqlda->version=SQLDA_VERSION1;
	in_sqlda->sqln=in_parameters.size();	

	//Выходные параметры
	boost::shared_array<char> in_out_sqlda(new char[XSQLDA_LENGTH(1)]);
	XSQLDA * out_sqlda=(XSQLDA*)in_out_sqlda.get();
	memset(out_sqlda, 0, XSQLDA_LENGTH(1));
	out_sqlda->version=SQLDA_VERSION1;
	out_sqlda->sqln=1;

	//Выделяем память под SQL запрос
	isc_dsql_allocate_statement(status_vector, &db_handle, &stmt_handle);	
	if (status_vector[0]==1 && status_vector[1])
	{				
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	

	//Подготавливаем SQL запрос
	isc_dsql_prepare(status_vector, &in_transaction.transaction_handle, &stmt_handle, 0, in_statement, SQL_DIALECT_CURRENT, out_sqlda);	
	if (status_vector[0]==1 && status_vector[1])
	{							
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
		
	isc_dsql_describe_bind(status_vector, &stmt_handle, 1, in_sqlda);
	if (status_vector[0]==1 && status_vector[1])
	{							
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (in_sqlda->sqld > in_sqlda->sqln)
	{
		//Здесь нужно выделить память заново, но для нас это уже бесполезно
		throw VSimpleException(L"Не выделена память под результаты запроса.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}

	const vector<VIBParameter>& parameters=in_parameters;
	for (size_t i=0;i<in_parameters.size();++i)
	{		
		XSQLVAR var=*(parameters.at(i).SqlVar());
		in_sqlda->sqlvar[i]=var;
	}

	//Получаем информацию о запросе
	char type_item[]={isc_info_sql_stmt_type};
	char res_buffer[8];
	isc_dsql_sql_info(status_vector, &stmt_handle, sizeof(type_item), type_item, sizeof(res_buffer), res_buffer);
	if (status_vector[0]==1 && status_vector[1])
	{					
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	int statement_type;
	if (res_buffer[0]==isc_info_sql_stmt_type)
	{
		int length=isc_vax_integer(&(res_buffer[1]), 2);
		statement_type=isc_vax_integer(&(res_buffer[3]), length);		
	}

	//В зависимости от типа запроса
	switch (statement_type)
	{
		case isc_info_sql_stmt_select://Запрос SELECT
		{	
			//Не обрабатывается
			break;
		}
		case isc_info_sql_stmt_insert:
		case isc_info_sql_stmt_update:
		case isc_info_sql_stmt_delete:			
		default://Все прочие запросы
		{
			//Выполняем SQL запрос
			isc_dsql_execute(status_vector, &in_transaction.transaction_handle, &stmt_handle, dialect, in_sqlda);
			if (status_vector[0]==1 && status_vector[1])
			{							
				throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}			
			in_transaction.has_changes=true;
			break;
		}
	}	
	//Освобождаем память из под SQL запроса
	isc_dsql_free_statement(status_vector, &stmt_handle, DSQL_drop);
	if (status_vector[0]==1 && status_vector[1])
	{															
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

/** Выполнение запроса с подготовкой с параметрами*/
void VIBDatabase::ExecuteStatement(VIBTransaction& in_transaction, const char * in_statement, const vector<VIBParameter>& in_parameters, VIBDataHandler& in_data_handler)
{
	QueryProfiler profiler(string_to_wstring(in_statement));
	//Проверяем корректность входных параметров
	if (in_statement==NULL)
	{
		throw NULL_POINTER;
	}
	if (strlen(in_statement)>MAX_STATEMENT_LENGTH)
	{
		throw STATEMENT_TOO_LONG;
	}
	if (!in_transaction.Active())
	{
		throw VSimpleException(L"Нет активной транзакции.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
				
	//Указатель на SQL-запрос    
	isc_stmt_handle stmt_handle = NULL;
	  
	//Входные параметры
	boost::scoped_array<char> in_sqlda_buffer(new char[XSQLDA_LENGTH(in_parameters.size())]);
	XSQLDA * in_sqlda = (XSQLDA*)in_sqlda_buffer.get();	
	memset(in_sqlda, 0, XSQLDA_LENGTH(in_parameters.size()));
	in_sqlda->version=SQLDA_VERSION1;
	in_sqlda->sqln=in_parameters.size();	

	//Выходные параметры
	boost::shared_array<char> out_sqlda_buffer(new char[XSQLDA_LENGTH(1)]);
	XSQLDA * out_sqlda = (XSQLDA*)out_sqlda_buffer.get();
	memset(out_sqlda, 0, XSQLDA_LENGTH(1));
	out_sqlda->version=SQLDA_VERSION1;
	out_sqlda->sqln=1;

	//Выделяем память под SQL запрос
	isc_dsql_allocate_statement(status_vector, &db_handle, &stmt_handle);	
	if (status_vector[0]==1 && status_vector[1])
	{				
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	

	//Подготавливаем SQL запрос
	isc_dsql_prepare(status_vector, &in_transaction.transaction_handle, &stmt_handle, 0, in_statement, SQL_DIALECT_CURRENT, out_sqlda);	
	if (status_vector[0]==1 && status_vector[1])
	{							
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
		
	isc_dsql_describe_bind(status_vector, &stmt_handle, 1, in_sqlda);
	if (status_vector[0]==1 && status_vector[1])
	{							
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (in_sqlda->sqld > in_sqlda->sqln)
	{
		//Здесь нужно выделить память заново, но для нас это уже бесполезно
		throw VSimpleException(L"Не выделена память под результаты запроса.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}

	const vector<VIBParameter>& parameters=in_parameters;
	for (size_t i=0;i<in_parameters.size();++i)
	{		
		XSQLVAR var=*(parameters.at(i).SqlVar());
		in_sqlda->sqlvar[i]=var;
	}

	//Получаем информацию о запросе
	char type_item[]={isc_info_sql_stmt_type};
	char res_buffer[8];
	isc_dsql_sql_info(status_vector, &stmt_handle, sizeof(type_item), type_item, sizeof(res_buffer), res_buffer);
	if (status_vector[0]==1 && status_vector[1])
	{					
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	int statement_type;
	if (res_buffer[0]==isc_info_sql_stmt_type)
	{
		int length=isc_vax_integer(&(res_buffer[1]), 2);
		statement_type=isc_vax_integer(&(res_buffer[3]), length);		
	}

	//В зависимости от типа запроса
	switch (statement_type)
	{
		case isc_info_sql_stmt_select://Запрос SELECT
		{						
			//Запрашиваем информацию о столбцах
			isc_dsql_describe(status_vector, &stmt_handle, 1, out_sqlda);		
			if (status_vector[0]==1 && status_vector[1])
			{												
				throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			//Проверяем число столбцов и корректируем XSQLDA
			if (out_sqlda->sqld>out_sqlda->sqln)
			{
				short column_count=out_sqlda->sqld;				
				out_sqlda_buffer.reset(new char[XSQLDA_LENGTH(column_count)]);
				out_sqlda = (XSQLDA*)out_sqlda_buffer.get();
				memset(out_sqlda, 0, XSQLDA_LENGTH(column_count));
				out_sqlda->version=SQLDA_VERSION1;
				out_sqlda->sqln=column_count;				

				// Запрашиваем информацию о столбцах
				isc_dsql_describe(status_vector, &stmt_handle, 1, out_sqlda);		
				if (status_vector[0]==1 && status_vector[1])
				{																		
					throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				}
			}				
			
			//Выполняем SQL запрос
			isc_dsql_execute2(status_vector, &in_transaction.transaction_handle, &stmt_handle, dialect, in_sqlda, NULL);
			if (status_vector[0]==1 && status_vector[1])
			{					
				throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}	

			//Формируем XSQLDA - выделяем память под данные
			AllocData(out_sqlda);
BOOST_SCOPE_EXIT((out_sqlda))
{
	FreeData(out_sqlda);
}BOOST_SCOPE_EXIT_END			
			//Обрабатываем данные
			long fetch_stat;
			if (out_sqlda->sqld>0)
			{				
				//Устанавливаем параметры обработчика
				in_data_handler.SetParameters(this, &in_transaction);
				//Вызываем обработчик запроса первый раз для того чтобы построить заголовок таблицы
				in_data_handler.Prepare(out_sqlda);
				//Перебираем полученные записи	
				while((fetch_stat = isc_dsql_fetch(status_vector, &stmt_handle, 1, out_sqlda)) == 0)
				{											
					//Вызываем обработчик запроса					
					in_data_handler.HandleData(out_sqlda);
				}    
				if(fetch_stat != 100L)
				{						
					throw VSimpleException(L"Не удалось получить результаты запроса.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				}
			}			
			break;
		}
		case isc_info_sql_stmt_insert:
		case isc_info_sql_stmt_update:
		case isc_info_sql_stmt_delete:			
		default://Все прочие запросы
		{
			//Выполняем SQL запрос
			isc_dsql_execute(status_vector, &in_transaction.transaction_handle, &stmt_handle, dialect, in_sqlda);
			if (status_vector[0]==1 && status_vector[1])
			{							
				throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}			
			in_transaction.has_changes=true;
			break;
		}
	}	
	//Освобождаем память из под SQL запроса
	isc_dsql_free_statement(status_vector, &stmt_handle, DSQL_drop);
	if (status_vector[0]==1 && status_vector[1])
	{															
		throw VSimpleException(L"Не удалось выполнить запрос.", string_to_wstring(GetErrorDescription()).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

/** Выполнение запросов*/
void VIBDatabase::ExecuteScript(VIBTransaction& in_transaction, const string& in_script, bool in_stop_on_errors)
{
	//Удалить все комментарии, то что начинается с -- и кончается переводом строки
	sregex re = "--" >> +~_ln >> _ln;
	string script = regex_replace(in_script, re, string(""));
	//Разбиваем скрипт на части
	string term(";");	
	string::iterator beg=script.begin();
	string::iterator end=script.begin();
	string::iterator dend=script.end();

	while (end!=script.end())
	{			
		end=search(beg, script.end(), term.begin(), term.end());
		if (end!=script.end())
		{
			string statement(beg, end);
			size_t term_length=term.length();
			std::string set_term = "SET TERM";
			if (!boost::algorithm::ifind_first(
					boost::iterator_range<std::string::const_iterator>(statement.begin(), statement.end()), 
					boost::iterator_range<std::string::const_iterator>(set_term.begin(), set_term.end())
					).empty())
			{					
				string space(" ");
				string::iterator new_term_beg=find_end(beg, end, space.begin(), space.end())+1;
				string::iterator new_term_end=end;
				term=string(new_term_beg, new_term_end);										
			}
			else
			{						
				try
				{
					ExecuteStatementImmediate(in_transaction, statement.c_str());					
				}
				catch (VSimpleException & e)
				{
					//Иногда при возникновении ошибки имеет смысл продолжить выполнение скрипта, сообщив об ошибке в журнале
					if (in_stop_on_errors)
					{
						throw e;
					}
					else
					{
						LOG_ERROR(string_to_wstring(GetErrorDescription()));						
					}											
				}
			}
			beg=end+term_length;
		}
	}
}


/** Выполнение запросов из файла*/
void VIBDatabase::ExecuteFromFile(VIBTransaction& in_transaction, const char * in_filename, bool in_stop_on_errors)
{	
	ifstream input;
	input.open(in_filename);
	if (input.is_open())
	{				
		string data=string(istreambuf_iterator<char>(input), istreambuf_iterator<char>());
		ExecuteScript(in_transaction, data, in_stop_on_errors);
	}
	else
	{
		throw VSimpleException(L"Не удалось открыть файл.", string_to_wstring(in_filename), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

//------------------------------------------ Экcпериментальные методы ---------------------------------------------

/** Мониторинг событий*/

static size_t event_flag=0;
static int first=1;

ISC_EVENT_CALLBACK event_function(char *result, short length, char *updated)
{	
	if (length==0)
		return(0);
	event_flag++;
	while (length--)
		*result++ = *updated++;
	return(0);
};

/** Запускает монитор*/
void VIBDatabase::StartMonitor()
{		
	if (Connected() && event_names.size()>0)
	{		
		first=1;
		length = (short)isc_event_block(&event_buffer, &result_buffer, event_names.size(), 
			GetEventName(0).c_str(), GetEventName(1).c_str(), GetEventName(2).c_str(), GetEventName(3).c_str(), GetEventName(4).c_str(),
			GetEventName(5).c_str(), GetEventName(6).c_str(), GetEventName(7).c_str(), GetEventName(8).c_str(), GetEventName(9).c_str(),
			GetEventName(10).c_str(), GetEventName(11).c_str(), GetEventName(12).c_str(), GetEventName(13).c_str(), GetEventName(14).c_str());
		isc_que_events(	status_vector, &db_handle, &event_id, length, event_buffer, 
			(ISC_EVENT_CALLBACK)event_function, result_buffer);
		if (status_vector[0] == 1 && status_vector[1])
		{
			throw VSimpleException(string_to_wstring(GetErrorDescription()).c_str(), L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		};		
	}
};

/** Возвращает число событий*/
size_t VIBDatabase::GetEventCount() const
{
	return event_names.size();
}

/** Возвращает имя события*/
std::string VIBDatabase::GetEventName(size_t in_index) const
{
	return in_index < event_names.size() ? event_names.at(in_index) : std::string("");
}

/** Возвращает статус события*/
size_t VIBDatabase::GetEventStatus(size_t in_index) const
{
	if (in_index<15)
		return count_array[in_index];
	else 
		throw VSimpleException(L"Поддерживается не более 15 событий.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
}

/** Проверяет событие*/
bool VIBDatabase::TestEvent()
{		
	bool result=false;
	if (Connected())
	{
		if (event_flag)
		{
			event_flag=0;
			if (first)
			{
				first=0;				
				result=false;
			}
			else
			{						
				result=true;
			}			

			memset(count_array, 0, 15*sizeof(ISC_ULONG));
			isc_event_counts(count_array, length, event_buffer, result_buffer);

			isc_que_events(status_vector, &db_handle, &event_id, length, event_buffer, 
				(ISC_EVENT_CALLBACK)event_function, result_buffer);
			if (status_vector[0] == 1 && status_vector[1])
			{
				throw VSimpleException(string_to_wstring(GetErrorDescription()).c_str(), L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			};			
		}
	}
	return result;
};

/** Останавливает монитор*/
void VIBDatabase::StopMonitor()
{
	monitor_running=false;
	if (Connected())
	{
		isc_cancel_events(status_vector, &db_handle, &event_id);
		if (status_vector[0] == 1 && status_vector[1])
		{
			throw VSimpleException(string_to_wstring(GetErrorDescription()).c_str(), L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		};
	}
};

/** Возвращает true если монитор запущен*/
bool VIBDatabase::IsMonitorRunning() const
{
	return monitor_running;
}

/** Устанавливает список отслеживаемых событий*/
void VIBDatabase::SetEventNames(const std::vector<std::string>& in_event_names)
{	
	event_names=in_event_names;
	if (event_names.size()>15)
		event_names.resize(15);
}

/** Создание BLOB*/
ISC_QUAD VIBDatabase::AddBlob(VIBTransaction& in_transaction, const VBlob& in_blob)
{
	//Проверяем входные параметры
	if (!in_transaction.Active())
	{
		throw VSimpleException(L"Нет активной транзакции.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}

	ISC_QUAD blob_id;
	isc_blob_handle blob_handle;
	blob_handle = NULL;

	//Создаем BLOB
	isc_create_blob2(status_vector, &db_handle, &in_transaction.transaction_handle, &blob_handle, &blob_id, 0, NULL);
	if (status_vector[0]==1 && status_vector[1])
	{					
		throw VIBException(&status_vector[0]);
	}

	//Записываем данные в BLOB
	size_t len=in_blob.GetSize();	
	boost::shared_array<char> data(new char[len]);
	memcpy(data.get(), in_blob.GetConstData(), len);
	char * source=data.get();
	size_t seg_size=4096;//FixMe!!! Либо должен быть размер страницы либо минимальный размер сегмента
	size_t remain_size=len;
	while (source<data.get()+len)
	{
		isc_put_segment(status_vector, &blob_handle, std::min<size_t>(seg_size, remain_size), source);
		remain_size-=seg_size;
		source+=seg_size;
		if (status_vector[0]==1 && status_vector[1])
		{					
			throw VIBException(&status_vector[0]);
		}
	}

	//Закрываем BLOB
	isc_close_blob(status_vector, &blob_handle);	
	if (status_vector[0]==1 && status_vector[1])
	{					
		throw VIBException(&status_vector[0]);
	}	
	return blob_id;
}

/** Возвращает размер о BLOB*/
size_t VIBDatabase::GetBLOBSize(VIBTransaction& in_transaction, ISC_QUAD blob_id)
{
	isc_blob_handle blob_handle;
	blob_handle = NULL;

	isc_open_blob2(status_vector, &db_handle, &in_transaction.transaction_handle, &blob_handle, &blob_id, 0, NULL);
	if (status_vector[0]==1 && status_vector[1])
	{					
		throw VIBException(&status_vector[0]);
	}

	char blob_items[] = {isc_info_blob_max_segment, isc_info_blob_num_segments, isc_info_blob_total_length, isc_info_blob_type};
	char res_buffer[40];
	isc_blob_info(status_vector, &blob_handle, sizeof(blob_items), blob_items, sizeof(res_buffer), res_buffer);	
	if (status_vector[0]==1 && status_vector[1])
	{					
		throw VIBException(&status_vector[0]);
	}

	long max_size=0;
	long num_segments=0;
	long total_length=0;
	long blob_type=0;

	for (char * p = res_buffer; *p != isc_info_end ;)
	{
		char item = *p++;
		short length = (short)isc_vax_integer(p, 2);
		p += 2;
		switch (item)
		{
			case isc_info_blob_max_segment:
				max_size = isc_vax_integer(p, length);
				break;
			case isc_info_blob_num_segments:
				num_segments = isc_vax_integer(p, length);
				break;
			case isc_info_blob_total_length:
				total_length = isc_vax_integer(p, length);
				break;
			case isc_info_blob_type:
				blob_type = isc_vax_integer(p, length);
				break;
			case isc_info_truncated:
				/* handle error */
				break;
			case isc_info_error:
				/* handle error */
				break;
			default:
				break;
		}
		p += length;
	}
	isc_close_blob(status_vector, &blob_handle);
	return total_length;
}

/** Чтение BLOB*/
void VIBDatabase::ReadBLOB(VIBTransaction& in_transaction, ISC_QUAD blob_id, VBlob * in_blob)
{
	isc_blob_handle blob_handle; /* Declare a Blob handle. */
	blob_handle = NULL;

	//Открываем BLOB
	isc_open_blob2(status_vector, &db_handle, &in_transaction.transaction_handle, &blob_handle, &blob_id, 0, NULL);	
	if (status_vector[0]==1 && status_vector[1])
	{					
		throw VIBException(&status_vector[0]);
	}

	//Получаем информацию о BLOB
	char blob_items[] = {isc_info_blob_max_segment, isc_info_blob_num_segments, isc_info_blob_total_length, isc_info_blob_type};
	char res_buffer[40];
	isc_blob_info(status_vector, &blob_handle, sizeof(blob_items), blob_items, sizeof(res_buffer), res_buffer);	
	if (status_vector[0]==1 && status_vector[1])
	{					
		throw VIBException(&status_vector[0]);
	}
	long max_size=0;
	long num_segments=0;
	long total_length=0;
	long blob_type=0;
	for (char * p = res_buffer; *p != isc_info_end ;)
	{
		char item = *p++;
		short length = (short)isc_vax_integer(p, 2);
		p += 2;
		switch (item)
		{
			case isc_info_blob_max_segment:
				max_size = isc_vax_integer(p, length);
				break;
			case isc_info_blob_num_segments:
				num_segments = isc_vax_integer(p, length);
				break;
			case isc_info_blob_total_length:
				total_length = isc_vax_integer(p, length);
				break;
			case isc_info_blob_type:
				blob_type = isc_vax_integer(p, length);
				break;
			case isc_info_truncated:
				isc_close_blob(status_vector, &blob_handle);
				throw VSimpleException(L"Не удалось получить информацию о BLOB целиком.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			case isc_info_error:
				isc_close_blob(status_vector, &blob_handle);
				throw VSimpleException(L"Не удалось получить информацию о BLOB.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			default:
				break;
		}
		p += length;
	}	

	in_blob->Reserve(total_length);

	//Загрузка данных
	char * segment=(char*)malloc(max_size);
	char * target=in_blob->GetData();
	unsigned short actual_seg_len;
	ISC_STATUS blob_stat;
	blob_stat = isc_get_segment(status_vector, &blob_handle, &actual_seg_len, (unsigned short)max_size, segment);

	while (blob_stat == 0 || status_vector[1] == isc_segment)
	{
		//FixMe!!! убрать небезопасные конструкции типа memcpy (указатель на void и размер буфера можно инкапсулировать в специальный класс)
		memcpy(target, segment, actual_seg_len);		
		target+=actual_seg_len;
		blob_stat = isc_get_segment(status_vector, &blob_handle, &actual_seg_len, (unsigned short)max_size, segment);
	}

	free(segment);
	isc_close_blob(status_vector, &blob_handle);		
}

/** Возвращает число открытых транзакций*/
size_t VIBDatabase::GetOpenTransactionCount()
{
	return transactions.size();
}
