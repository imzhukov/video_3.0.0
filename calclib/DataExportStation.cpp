#include "strlib.h"
#include "revision.h"
#include "dblib.h"
#include "dbfunc.h"
#include "VProgressInterface.h"

#include "DataExport.h"

#include <iomanip>
#include <limits>

#include <boost/lexical_cast.hpp>

std::wstring DeleteStTrail(VIBDatabasePtr db, const std::wstring & sh_name)
{
	std::wstring result;

	/// [new] удаление расчитанного профиля
	result = DeleteStationProfile(db, sh_name);
	if (!result.empty()) result += L"\n";

	VIBTransaction trans_delete;
	std::stringstream sql;
	sql << "delete from stw_trail where filename='" << sh_name << "';";

	trans_delete = db->StartTransaction();
	try
	{
		db->ExecuteStatementImmediate(trans_delete, sql.str().c_str());
	}
	catch (VException &)
	{
		std::string error = sql.str() + "\n\n" + db->GetErrorDescription();
		db->RollbackTransaction(trans_delete);
		return string_to_wstring(error);
	}

	db->CommitTransaction(trans_delete);
	return result;
}

///Занесение в БД файла станционных путей
std::wstring ExportSingleStationFileDB(VIBDatabasePtr db, const VFileName & fileName, VProgressBox & prog)
{

	//Проверка уникальности поездки
	prog.SetMessage(0,"Проверка уникальности поездки");

	std::stringstream sql;
	VIBTransaction trans = db->StartTransaction();
	VIBDataHandlerParameter data_handler;

	sql << "select * from stw_trail where filename='" << fileName.GetShortName() << "'";
	try
	{

		db->ExecuteStatement(trans ,sql.str().c_str(), data_handler);
	}
	catch (...)
	{
		std::string error = sql.str() + "\n\n" +db->GetErrorDescription();
		db->RollbackTransaction(trans);
		return string_to_wstring(error);
	}

	if (data_handler.GetRowCount() > 0)
	{
		db->RollbackTransaction(trans);
		return L"Запись о поездке уже существует";
	}

	VDataSet dataSet(fileName);

	//Проверка наличия в базе станции и парка
	sql.str("");
	sql << "select id from stw_park where name = '" << dataSet.GetPeregon() << "' ";
	sql << "and adm_station_id = (select id from adm_station where code = '" << dataSet.GetStationCode() << "')";

	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), data_handler);
	}
	catch (...)
	{
		std::string error = sql.str() + "\n\n" + db->GetErrorDescription();
		db->RollbackTransaction(trans);
		return string_to_wstring(error);
	}

	if (data_handler.GetRowCount() == 0)
	{
		db->RollbackTransaction(trans);
		std::stringstream msg;
		msg << "Информация о станции '" << dataSet.GetStationCode() << " - " << dataSet.GetStationName() << "' ";
		msg << "или о парке '" << dataSet.GetPeregon() << "' отсутствует в базе";
		return string_to_wstring(msg.str());
	}

	// Занесение информации о поездке
	prog.SetMessage(0,"Занесение информации о поездке");

	dataSet.Load(PRM_ABS_COORD);
	sql.str("");
	sql << "insert into stw_trail (nvps, filename, date_trail, ";
	sql << "start_abs, end_abs, prog_version, stw_way_id) values (";
	sql << "'" << dataSet.GetDivision() << "', ";
	sql << "'" << fileName.GetShortName() << "',";
	sql << "'"<<dataSet.GetRailDate().ToStringISO()<<"',";
	sql << dataSet.GetAbsCoord().GetItem(0) << ", " << dataSet.GetAbsCoord().GetLastItem() << ", ";
	sql << "'" << revision << "', ";
	sql << "(select id from stw_way where code = '" << dataSet.GetWay() << "' and stw_park_id = ";
	sql << "(select id from stw_park where name = '"<<dataSet.GetParkName() << "'and adm_station_id = ";
	sql << "(select id from adm_station where code = '"<<dataSet.GetStationCode() << "'))));";

	try
	{
		db->ExecuteStatementImmediate(trans ,sql.str().c_str());
	}
	catch (...)
	{
		std::string error = sql.str() + "\n\n" + db->GetErrorDescription();
		db->RollbackTransaction(trans);
		return string_to_wstring(error);
	}

	prog.SetMessage(0,"Загрузка меток оператора");
	dataSet.Load(PRM_APRINFO);
	dataSet.Load(PRM_OP_MARKS);

	VOpMarkList & markList = dataSet.GetOpMarks();
	prog.SetRange(0, markList.GetCount());
	for (size_t i = 0; i < markList.GetCount(); i++)
	{
		vector<string> split_vec;
		string marks=wstring_to_string(markList.GetItem(i).LongText());
		boost::algorithm::split(split_vec,marks,boost::is_any_of(","));

		try
		{
			for (size_t j = 0; j < split_vec.size(); j++)
			{
				sql.str("");
				sql << std::setprecision(std::numeric_limits<double>::digits10);
				sql << "insert into stw_opmark (enu_opmark_type_id, stw_trail_id, abscoord) ";
				sql << "values ((select id from enu_opmark_type where stype= '" << split_vec.at(j) << "'), ";
				sql << "(select id from stw_trail where filename = '" << fileName.GetShortName() << "'),";
				sql	<< dataSet.GetAbsCoord().GetItem(markList.GetItem(i).index) << ";";
				db->ExecuteStatementImmediate(trans, sql.str().c_str());
			}
		}
		catch (...)
		{
			std::string error = sql.str() + "\n\n" + db->GetErrorDescription();
			DeleteStTrail(db, fileName.GetShortName());
			db->RollbackTransaction(trans);
			return string_to_wstring(error);
		}
	}

	//Подтверждение транзакции с метками оператора
	db->CommitTransaction(trans);
	return L"";
}

///Занесение в БД нескольких файлов станционных путей
std::wstring ExportMultiStationFileDB(VIBDatabasePtr db, const std::vector<VFileName> & files)
{
	VProgressBox prog(2, "Занесение измерительных данных в БД", true);
	prog.SetRange(1, files.size());

	if (!db->Connected())
	{
		std::string error = db->GetErrorDescription();
		return string_to_wstring(error);
	}
	std::wstring result;
	for (size_t i = 0; i < files.size(); i++)
	{
		try
		{
			std::wstringstream msg;
			msg << L"Файл " << i+1 << L" из " << files.size();
			msg << L" [" << files[i].GetShortName() << L"]";
			prog.SetMessage(1, msg.str().c_str());
			std::wstring cur_result = ExportSingleStationFileDB(db, files.at(i), prog);

			if (!cur_result.empty())
			{
				result += files[i].GetPath() + L"\n";
				result += cur_result;
				result += L"\n\n";
			}
		}
		catch(VException &)
		{
			if (result.empty())
				return L"Неизвестное исключение";
			else
				return result;
		}
		prog.SetPosition(1, i+1);
	}

	return result;
}

/// число точек (aprior - только точки с известной высотой)
int64_t CalcProfPointCount(VIBDatabasePtr db, const std::wstring & sh_name, bool apr)
{
	if (!db->Connected()) return -1;
	
	VIBTransaction trans = db->StartTransaction();
	VIBDataHandlerParameter handler;

	std::stringstream sql;
	sql << "select count(id) from stw_trail where filename = '" << sh_name << "';";
	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		db->RollbackTransaction(trans);
		return -1;
	}
	if (handler.GetParameter(0, 0).GetInteger() == 0)
	{
		db->RollbackTransaction(trans);
		return -1;
	}

	sql.str("");
	sql << "select count(stw_opmark.abscoord) from stw_opmark_to_point ";
	sql << "inner join stw_opmark on stw_opmark_to_point.stw_opmark_id = stw_opmark.id ";
	sql << "inner join stw_point_to_object on stw_opmark_to_point.stw_point_to_object_id = stw_point_to_object.id ";
	sql << "inner join stw_trail on stw_opmark.stw_trail_id = stw_trail.id ";
	sql << "where stw_trail.filename = '" << sh_name << "'";
	if (apr)
	{
		sql << " and stw_point_to_object.height is not null";
	}
	sql << ";";
	
	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		db->RollbackTransaction(trans);
		return -1;
	}

	int64_t result = handler.GetParameter(0, 0).GetInteger();
	db->RollbackTransaction(trans);
	return result;
}

std::wstring CalcStationProfile(VIBDatabasePtr db, const VFileName & fn)
{
	VIBTransaction trans = db->StartTransaction();
	VIBDataHandlerParameter handler;

	// Проверка наличия поездки в базе
	std::stringstream sql;
	sql << "select count(id) from stw_trail where filename = '" << fn.GetShortName() << "';";
	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch (VException& e)
	{
		std::wstring message = string_to_wstring(sql.str() + "\n\n") + e.GetDescription();
		db->RollbackTransaction(trans);
		return message;
	}
	catch(...)
	{
		db->RollbackTransaction(trans);
		return L"Неизвестное исключение";
	}
	if (handler.GetParameter(0, 0).GetInteger() == 0)
	{
		db->RollbackTransaction(trans);
		return L"Запись о файле отсутствует в базе";
	}
	if (handler.GetParameter(0, 0).GetInteger() > 1)
	{
		db->RollbackTransaction(trans);
		return L"Запись о файле не уникальна в базе";
	}
	db->RollbackTransaction(trans);

	// Загрузка априорной информации
	VDataSet dataSet(fn);
	if (!dataSet.Load(PRM_APRINFO))
	{
		return dataSet.GetErrorMessage();
	}
	if (!dataSet.GetAprinfo().UpdateStTrailInfo(*db, fn.GetShortName()))
	{
		return dataSet.GetAprinfo().GetErrorMessage();
	}

	// Загрузка/расчет профиля
	if (!dataSet.Load(PRM_PROFILE))
	{
		return dataSet.GetErrorMessage();
	}

	// выбираем точки с неуказанной высотой
	trans = db->StartTransaction();
	sql.str("");
	sql << "select stw_opmark.abscoord, stw_point_to_object.id from stw_opmark_to_point ";
	sql << "inner join stw_opmark on stw_opmark_to_point.stw_opmark_id = stw_opmark.id ";
	sql << "inner join stw_point_to_object on stw_opmark_to_point.stw_point_to_object_id = stw_point_to_object.id ";
	sql << "inner join stw_trail on stw_opmark.stw_trail_id = stw_trail.id ";
	sql << "where stw_point_to_object.height is null ";
	sql << "and stw_trail.filename = '" << fn.GetShortName() << "';";
	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch (VException& e)
	{
		std::wstring message = string_to_wstring(sql.str() + "\n\n") + e.GetDetails();
		db->RollbackTransaction(trans);
		return message;
	}
	catch(...)
	{
		db->RollbackTransaction(trans);
		return L"Неизвестное исключение";
	}

	// обновляем записи
	for (size_t i = 0; i < handler.GetRowCount(); ++i)
	{
		int64_t point_id = handler.GetParameter(i, 1).GetInteger();
		double point_coord = handler.GetParameter(i, 0).GetFloat();
		size_t item_index = dataSet.GetAbsCoord().FindIndex(point_coord, true);
		float height = dataSet.GetParameter(PRM_PROFILE).GetItem(item_index);

		sql.str("");
		sql << "update stw_point_to_object set height = " << height << " where id = " << point_id << ";";
		try
		{
			db->ExecuteStatement(trans, sql.str().c_str());
		}
		catch (VException& e)
		{
			std::wstring message = string_to_wstring(sql.str() + "\n\n") + e.GetDetails();
			db->RollbackTransaction(trans);
			return message;
		}
		catch(...)
		{
			db->RollbackTransaction(trans);
			return L"Неизвестное исключение";
		}
	}

	db->CommitTransaction(trans);

	// Обновляем априорную информацию с учетом только что расчитанного
	if (!dataSet.GetAprinfo().UpdateStTrailInfo(*db, fn.GetShortName()))
	{
		return dataSet.GetAprinfo().GetErrorMessage();
	}

	dataSet.SetChanged(PRM_APRINFO);
	if (!dataSet.Save())
	{
		return dataSet.GetErrorMessage();
	}
	
	return L"";
}

/// Удаление расчета профиля на файле
std::wstring DeleteStationProfile(VIBDatabasePtr db, const std::wstring & sh_name)
{
	// выбираем точки с указанной высотой
	VIBTransaction trans = db->StartTransaction();
	VIBDataHandlerParameter handler;
	std::stringstream sql;

	sql << "execute procedure SET_NULL_HEIGHT(select stw_park_id from stw_way where id = ";
	sql << "(select stw_way_id from stw_trail where filename = '" << sh_name << "'));";
	try
	{
		db->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch (VException& e)
	{
		std::wstring message = string_to_wstring(sql.str() + "\n\n") + e.GetDetails();
		db->RollbackTransaction(trans);
		return message;
	}
	catch(...)
	{
		db->RollbackTransaction(trans);
		return L"Неизвестное исключение";
	}
	db->CommitTransaction(trans);
	return L"";

}