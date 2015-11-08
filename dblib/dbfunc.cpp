
#include <boost/lexical_cast.hpp>
#include "strlib.h"
#include "dbfunc.h"

/** Возвращает эксплуатационную координату*/
VWayCoord GetRealCoord(VIBDatabasePtr in_db, const char * in_way_code, const char * in_direction_code, double in_abscoord, const VTime& in_date)
{
	VWayCoord result;
	//Форматируем дату
	string date=in_date.DateToStringISO();
	//Обработчик данных
	VIBDataHandlerParameter handler;	
	string sql=boost::str(boost::format("select km, m from GET_REALCOORD( (select id from ADM_WAY where code='%s' and ADM_DIRECTION_id=(select id from ADM_DIRECTION where code='%s')), %f, '%s')") 
		%in_way_code %in_direction_code %in_abscoord %date.c_str());
	VIBTransaction transaction=in_db->StartTransaction();
	in_db->ExecuteStatement(transaction, sql.c_str(), handler);	
	in_db->RollbackTransaction(transaction);		
	wstring start_km=handler.GetParameter(0, 0).GetString();
	wstring start_m=handler.GetParameter(0, 1).GetString();		
	result.km=boost::lexical_cast<int>(start_km.c_str());
	result.m=boost::lexical_cast<float>(start_m.c_str());			
	return result;
}

/** Возвращает абсолютную координату*/
VWayCoord GetRealCoord(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::wstring& in_way_id, double in_abscoord)
{	
	VWayCoord result;
	std::wstring req=boost::str(boost::wformat(
		L"select KM, M from get_realcoord(%s, %f, CURRENT_DATE)")
			% in_way_id % in_abscoord);					
	VIBDataHandlerParameter data_handler;				
	in_db->ExecuteStatement(in_transaction, wstring_to_string(req).c_str(), data_handler);	
	result.km=boost::lexical_cast<int>(data_handler.GetParameter(0, 0).GetString());	
	result.m=boost::lexical_cast<float>(data_handler.GetParameter(0, 1).GetString());	
	return result;
}

/** Возвращает абсолютную координату*/
double GetAbsCoord(VIBDatabasePtr in_db, const char * in_way_code, const char * in_direction_code, const VWayCoord& in_waycoord, const VTime& in_date)
{
	double result;	
	string date=in_date.DateToStringISO();
	//Обработчик данных
	VIBDataHandlerParameter handler;	
	string sql=boost::str(boost::format("select abscoord from GET_ABSCOORD( (select id from ADM_WAY where code='%s' and ADM_DIRECTION_id=(select id from ADM_DIRECTION where code='%s')), %d, %f, '%s')")
		%in_way_code %in_direction_code %in_waycoord.km %in_waycoord.m %date.c_str());	
	VIBTransaction transaction=in_db->StartTransaction();
	in_db->ExecuteStatement(transaction, sql.c_str(), handler);
	in_db->RollbackTransaction(transaction);	
	//wstring abscoord=handler.GetParameter(0, wstring(L"abscoord")).GetString();
	wstring abscoord=handler.GetParameter(0, 0).GetString();				
	result=str_to_double(abscoord.c_str());			
	return result;
}

/** Возвращает абсолютную координату*/
double GetAbsCoord(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::wstring& in_way_id, const VWayCoord& in_wc)
{
	double result;
	std::wstring req=boost::str(boost::wformat(
		L"select ABSCOORD from get_abscoord(%s, %d, %f, CURRENT_DATE)")
			% in_way_id % in_wc.km % in_wc.m);					
	VIBDataHandlerParameter data_handler;				
	in_db->ExecuteStatement(in_transaction, wstring_to_string(req).c_str(), data_handler);	
	result=boost::lexical_cast<double>(data_handler.GetParameter(0, 0).GetString());	
	return result;
}

/** Добавляет длину к эксплуатационной координате*/
VWayCoord IncRealCoord(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::wstring& in_way_id, const VWayCoord& in_wc, double in_length)
{
	VWayCoord result;
	std::wstring req=boost::str(boost::wformat(L"select KM, M "
		L"from get_realcoord(%s, (select ABSCOORD+%f from get_abscoord(%s, %d, %f, CURRENT_DATE)), CURRENT_DATE)")
		% in_way_id % in_length % in_way_id % in_wc.km % in_wc.m);					
	VIBDataHandlerParameter data_handler;				
	in_db->ExecuteStatement(in_transaction, wstring_to_string(req).c_str(), data_handler);	
	result.km=boost::lexical_cast<int>(data_handler.GetParameter(0, 0).GetString());
	result.m=boost::lexical_cast<float>(data_handler.GetParameter(0, 1).GetString());
	return result;
}

/** Возвращает сообщение об ошибке*/
std::wstring GetErrorDescription(VIBDatabasePtr in_db, VIBTransaction & in_transaction)
{
	std::wstring result;
	try
	{
		if (in_db->GetErrorCode()==isc_check_constraint ||
			in_db->GetErrorCode()==isc_unique_key_violation)
		{		
			VIBDataHandlerParameter handler;					
			std::string sql=boost::str(boost::format("select description from SRV_ERROR_TYPES where constr_name='%s'") % in_db->GetErrorConstraint());
			in_db->ExecuteStatement(in_transaction, sql.c_str(), handler);	
			result=handler.GetParameter(0, 0).GetString();			
		}
	}
	catch (VException& e)
	{
		return e.GetDescription();
	}
	return result;
}

/** Возвращает сообщение об ошибке*/
std::wstring GetConstraintMessage(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::wstring& in_constraint)
{
	if (in_constraint.find_first_not_of(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ_") != std::wstring::npos)
	{
		return L"";
	}

	std::string sql=boost::str(boost::format("select description from SRV_ERROR_TYPES where constr_name='%s'") % wstring_to_string(in_constraint));
	std::vector<std::wstring> msg;
	GetSingleField(in_db, in_transaction, sql, msg);
	if (!msg.empty())			
		return msg.front();
	else 
		return L"";
}

/** Возращает сообщения об ошибках*/
std::vector<std::wstring> GetErrorLog(VIBDatabasePtr in_db, VIBTransaction & in_transaction)
{
	std::vector<std::wstring> result;
	VIBDataHandlerParameter handler;	
	string sql="select err from SRV_ERROR_MESSAGES";			
	in_db->ExecuteStatement(in_transaction, sql.c_str(), handler);	
	for (size_t i=0; i<handler.GetRowCount(); ++i)
	{
		result.push_back(handler.GetParameter(i, 0).GetString());
	}
	return result;
}

/** Очищает журнал ошибок*/
void ClearErrorLog(VIBDatabasePtr in_db, VIBTransaction & in_transaction)
{
	in_db->ExecuteStatementImmediate(in_transaction, "delete from SRV_ERROR_MESSAGES");
}

/** Очищает таблицу проверок*/
void ClearChanges(VIBDatabasePtr in_db, VIBTransaction & in_transaction)
{
	in_db->ExecuteStatementImmediate(in_transaction, "delete from SRV_CHANGES");
}

/** Возвращает ID записи*/
std::wstring GetRowID(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::string& in_query)
{
	VIBDataHandlerParameter handler;	
	in_db->ExecuteStatement(in_transaction, in_query.c_str(), handler);
	if (handler.GetColCount()==1 && handler.GetRowCount()==1)
	{
		return handler.GetParameter(0,0).GetString();		
	}
	else
	{		
		throw VSimpleException(L"Запись не найдена", string_to_wstring(in_query), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
};

/** Возвращает результат запроса одного поля как вектор строк*/
void GetSingleField(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::string& in_query, std::vector<std::wstring>& out_values)
{
	VIBDataHandlerParameter handler;	
	in_db->ExecuteStatement(in_transaction, in_query.c_str(), handler);
	if (handler.GetColCount()!=1)
	{
		throw VSimpleException(L"Запрос не вернул результатов", string_to_wstring(in_query), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	else
	{
		for (size_t i=0;i<handler.GetRowCount();++i)
		{
			out_values.push_back(handler.GetParameter(i,0).GetString());
		}
	}	
}
