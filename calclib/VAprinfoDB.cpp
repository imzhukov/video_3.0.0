#include "VAprinfo.h"
#include "dblib.h"
#include "strlib.h"
#include <sstream>

// -----------------------------------------------------------------------------
// VAprinfoParameter
// -----------------------------------------------------------------------------

// выровнять границы записей по указанным рамкам
template <class T> void VAprinfoParameter<T>::SyncRanges(double begin, double end)
{
	//FixMe!!! Вектор может быть пустым S.Markin
	if (this->GetCount()==0)
		return;

	this->Sort();
	this->GetItem(0).base = begin;		
	for (size_t i = 1; i < this->GetCount(); ++i)
	{
		double value = 0.5 * (this->GetItem(i-1).end + this->GetItem(i).base);
		this->GetItem(i-1).end = value;
		this->GetItem(i).base = value;
		T cur = this->GetItem(i), prev = this->GetItem(i-1);
	}
	this->GetLastItem().end = end;
}

template <class T> void VAprinfoParameter<T>::SyncRanges(double begin, double end, 
														 long def_value)
{
	//FixMe!!! Вектор может быть пустым S.Markin
	if (this->GetCount()==0)
		return;

	const double min_filling_area = 10.0;
	size_t recCount = this->GetCount();
	this->Sort();

	if (this->GetItem(0).base - begin < min_filling_area)	
	{
		this->GetItem(0).base = begin;
	}
	else
	{
		T t;
		t.base = begin;
		t.end = this->GetItem(0).base;
		t.value = def_value;
		this->Add(t);
	}

	for (size_t i = 1; i < recCount; ++i)
	{
		if (this->GetItem(i).base - this->GetItem(i-1).end < min_filling_area)
		{
			double cur_value = 0.5 * (this->GetItem(i).base + this->GetItem(i-1).end);
			this->GetItem(i).base = cur_value;
			this->GetItem(i-1).end = cur_value;
		}
		else
		{
			T t;
			t.base = this->GetItem(i-1).end;
			t.end = this->GetItem(i).base;
			t.value = def_value;
			this->Add(t);
		}
	}

	if (end - this->GetItem(recCount-1).end < min_filling_area)
	{
		this->GetItem(recCount-1).end = end;
	}
	else
	{
		T t;
		t.base = this->GetItem(recCount-1).end;
		t.end = end;
		t.value = def_value;
		this->Add(t);
	}

	this->Sort();
}

template <class T> void VAprinfoParameter<T>::SyncRanges(double begin, double end, 
														 std::string def_value)
{
	//FixMe!!! Вектор может быть пустым S.Markin
	if (this->GetCount()==0)
		return;

	const double min_filling_area = 10.0;
	size_t recCount = this->GetCount();
	this->Sort();

	if (this->GetItem(0).base - begin < min_filling_area)	
	{
		this->GetItem(0).base = begin;
	}
	else
	{
		T t;
		t.base = begin;
		t.end = this->GetItem(0).base;
		t.descr = def_value;
		this->Add(t);
	}

	for (size_t i = 1; i < recCount; ++i)
	{
		if (this->GetItem(i).base - this->GetItem(i-1).end < min_filling_area)
		{
			double cur_value = 0.5 * (this->GetItem(i).base + this->GetItem(i-1).end);
			this->GetItem(i).base = cur_value;
			this->GetItem(i-1).end = cur_value;
		}
		else
		{
			T t;
			t.base = this->GetItem(i-1).end;
			t.end = this->GetItem(i).base;
			t.descr = def_value;
			this->Add(t);
		}
	}

	if (end - this->GetItem(recCount-1).end < min_filling_area)
	{
		this->GetItem(recCount-1).end = end;
	}
	else
	{
		T t;
		t.base = this->GetItem(recCount-1).end;
		t.end = end;
		t.descr = def_value;
		this->Add(t);
	}

	this->Sort();
}


template <class T> void VAprinfoParameter<T>::SyncSpdRanges(double begin, double end, const VSpeeds & def_spd)
{
	//FixMe!!! Вектор может быть пустым S.Markin
	if (this->GetCount()==0)
		return;

	const double min_filling_area = 10.0;
	size_t recCount = this->GetCount();
	this->Sort();

	if (this->GetItem(0).base - begin < min_filling_area)	
	{
		this->GetItem(0).base = begin;
	}
	else
	{
		T t;
		t.base = begin;
		t.end = this->GetItem(0).base;
		t.spd = def_spd;
		this->Add(t);
	}

	for (size_t i = 1; i < recCount; ++i)
	{
		if (this->GetItem(i).base - this->GetItem(i-1).end < min_filling_area)
		{
			double cur_value = 0.5 * (this->GetItem(i).base + this->GetItem(i-1).end);
			this->GetItem(i).base = cur_value;
			this->GetItem(i-1).end = cur_value;
		}
		else
		{
			T t;
			t.base = this->GetItem(i-1).end;
			t.end = this->GetItem(i).base;
			t.spd = def_spd;
			this->Add(t);
		}
	}

	if (end - this->GetItem(recCount-1).end < min_filling_area)
	{
		this->GetItem(recCount-1).end = end;
	}
	else
	{
		T t;
		t.base = this->GetItem(recCount-1).end;
		t.end = end;
		t.spd = def_spd;
		this->Add(t);
	}

	this->Sort();
}

template <class T> void VAprinfoParameter<T>::SyncRanges(double begin, double end, long def_width, long def_level)
{
	//FixMe!!! Вектор может быть пустым S.Markin
	if (this->GetCount()==0)
		return;

	const double min_filling_area = 1.0;
	size_t recCount = this->GetCount();
	this->Sort();

	if (this->GetItem(0).base - begin < min_filling_area)	
	{
		this->GetItem(0).base = begin;
	}
	else
	{
		T t;
		t.base = begin;
		t.end = this->GetItem(0).base;
		t.mo_width = def_width;
		t.mo_level = def_level;
		this->Add(t);
	}

	for (size_t i = 1; i < recCount; ++i)
	{
		if (this->GetItem(i).base - this->GetItem(i-1).end < min_filling_area)
		{
			double cur_value = 0.5 * (this->GetItem(i).base + this->GetItem(i-1).end);
			this->GetItem(i).base = cur_value;
			this->GetItem(i-1).end = cur_value;
		}
		else
		{
			T t;
			t.base = this->GetItem(i-1).end;
			t.end = this->GetItem(i).base;
			t.mo_width = def_width;
			t.mo_level = def_level;
			this->Add(t);
		}
	}

	if (end - this->GetItem(recCount-1).end < min_filling_area)
	{
		this->GetItem(recCount-1).end = end;
	}
	else
	{
		T t;
		t.base = this->GetItem(recCount-1).end;
		t.end = end;
		t.mo_width = def_width;
		t.mo_level = def_level;
		this->Add(t);
	}

	this->Sort();
}

bool operator > (const VSpeeds & s1, const VSpeeds & s2)
{
	return s1.Valid() && s2.Valid() &&
		(s1.GetPassSpeed() > s2.GetPassSpeed() ||
		(s1.GetPassSpeed() == s2.GetPassSpeed() && s1.GetTruckSpeed() > s2.GetTruckSpeed()) ||
		(s1.GetPassSpeed() == s2.GetPassSpeed() && s1.GetTruckSpeed() == s2.GetTruckSpeed() &&
		 s1.GetEmptySpeed() > s2.GetEmptySpeed()));
}

// выровнять границы скоростей, вернуть true для предупреждения о дырках
template <class T> bool VAprinfoParameter<T>::SyncSpdRanges(double begin, double end)
{
	bool warning = false;
	const double min_filling_area = 10.0;

	if (this->GetCount() == 0)
	{
		warning = true;
	}
	else
	{
		if (this->GetItem(0).base > begin)
		{
			if (this->GetItem(0).base - begin > min_filling_area)
			{
				warning = true;
			}
			this->GetItem(0).base = begin;
		}

		if (this->GetLastItem().end < end)
		{
			if (end - this->GetLastItem().end > min_filling_area)
			{
				warning = true;
			}
			this->GetLastItem().end = end;
		}
		for (size_t i = 1; i < this->GetCount(); ++i)
		{
			if (this->GetItem(i).base > this->GetItem(i-1).end)
			{
				if (this->GetItem(i).base - this->GetItem(i-1).end > min_filling_area)
				{
					warning = true;
				}

				if (this->GetItem(i).spd > this->GetItem(i-1).spd)
				{
					this->GetItem(i).base = this->GetItem(i-1).end;
				}
				else
				{
					this->GetItem(i-1).end = this->GetItem(i).base;
				}
			}
		}
	}

	return warning;
}

// выровнять границы выровнять границы стрелок
template <class T> void VAprinfoParameter<T>::SyncSwitchRanges(double begin, double end)
{
	size_t swCount = this->GetCount();
	if (swCount == 0) return;

	VAprinfoSwitchRecord sw;
	sw.base = begin;
	sw.end = this->GetItem(0).base;
	this->Add(sw);

	for (size_t i = 1; i < swCount; ++i)
	{
		double a = this->GetItem(i-1).end;
		double b = this->GetItem(i).base;

		if (a < b)
		{
			sw.base = a;
			sw.end = b;
			this->Add(sw);
		}
	}

	sw.base = this->GetLastItem().end;
	sw.end = end;
	this->Add(sw);
	this->Sort();
}

// выровнять границы выровнять границы плетей
template <class T> void VAprinfoParameter<T>::SyncWobRanges(double begin, double end)
{
	size_t swCount = this->GetCount();
	if (swCount == 0) return;

	VAprinfoWobRecord wob;
	wob.base = begin;
	wob.end = this->GetItem(0).base;
	this->Add(wob);

	for (size_t i = 1; i < swCount; ++i)
	{
		double a = this->GetItem(i-1).end;
		double b = this->GetItem(i).base;

		if (a < b)
		{
			wob.base = a;
			wob.end = b;
			this->Add(wob);
		}
	}

	wob.base = this->GetLastItem().end;
	wob.end = end;
	this->Add(wob);
	this->Sort();
}

static double SafeZoneLength(double length)
{
	if (length < 25.0)			return 0.0;
	else if (length < 100.0)	return 200.0;
	else						return 500.0;
}

static std::vector<VAprinfoIndexRecord> BuildSafeZone(double areaBegin, double areaEnd, 
													  double prevLen, double nextLen)
{
	std::vector<VAprinfoIndexRecord> result;
	VAprinfoIndexRecord bridge;

	double prevZone = SafeZoneLength(prevLen);
	double nextZone = SafeZoneLength(nextLen);

	// empty area
	if (areaEnd <= areaBegin)
	{
	}

	// area is shorter than both zones
	if ((areaEnd - areaBegin) <= (prevZone + nextZone))
	{
		bridge.base = areaBegin;
		bridge.end = areaEnd;
		bridge.value = SZO_SZONE;
		result.push_back(bridge);
	}

	// area is longer than both zones
	else
	{
		if (prevZone > 0.0)
		{
			bridge.base = areaBegin;
			bridge.end = areaBegin + prevZone;
			bridge.value = SZO_SZONE;
			result.push_back(bridge);
		}

		bridge.base = areaBegin + prevZone;
		bridge.end = areaEnd - nextZone;
		bridge.value = SZO_NONE;
		result.push_back(bridge);

		if (nextZone > 0.0)
		{
			bridge.base = areaEnd - nextZone;
			bridge.end = areaEnd;
			bridge.value = SZO_SZONE;
			result.push_back(bridge);
		}
	}

	return result;
}

struct __tmpSpdArea
{
	double begin, end;
	speed_t value;
};

bool VAprinfo::Makeinfo(VIBDatabase & db,
						long dirCode, const std::string & wayName, const VWaySection & in_section, 
						const VTime & rideDate, bool repairs, bool mea_curves)
{
	this->Unload();

	this->message = ERRMSG_OK;

	if (!db.Connected())
	{
		this->message = ERRMSG_DB_NO_CONNECTION;
		return false;
	}

	// транзакция
	VIBTransaction trans = db.StartTransaction();

	// хэндлер
	VIBDataHandlerParameter handler;

	// поток в памяти для формирования запросов
	std::stringstream sql;

	// число возвращаемых базой рядов
	size_t rowCount;

	// ------------------------------------------------------------
	// направление и путь
	// ------------------------------------------------------------
	// вынимаем ВСЕ участки пути с нужным направлением/путём,
	// затем отбираем первый пересекающийся с указанной секцией

	sql << "select adm_direction.code, adm_direction.name, adm_way.code, ";
	sql << "adm_way.id, gwp.start_km, gwp.start_m, gwp.end_km, gwp.end_m " << std::endl;
	sql << "from adm_direction inner join adm_way on adm_way.adm_direction_id = adm_direction.id ";
	sql << "inner join get_way_coord_date_period  gwp on gwp.adm_way_id = adm_way.id " << std::endl;
	sql << "where adm_direction.code = '" << dirCode << "' and adm_way.code = '" << wayName << "';" << std::endl;

	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	size_t rowNdx = 0;
	bool found = false;
	VWayCoord way_start, way_end;
	for (; rowNdx < rowCount; ++rowNdx)
	{
		way_start = VWayCoord(handler.GetParameter(rowNdx, 4).GetInteger(),
			handler.GetParameter(rowNdx, 5).GetFloat());

		way_end = VWayCoord(handler.GetParameter(rowNdx, 6).GetInteger(),
			handler.GetParameter(rowNdx, 7).GetFloat());

		if (way_end <= in_section.start || way_start >= in_section.end)
			continue;

		found = true;
		break;
	}

	if (!found)
	{
		this->message = L"Направление/путь отсутствуют в базе";
		db.RollbackTransaction(trans);
		return false;
	}

	// id пути
	__int64 way_id;
	try
	{
		this->dirCode = _wtoi(handler.GetParameter(rowNdx, 0).GetString().c_str());
		this->dirName = wstring_to_string(handler.GetParameter(rowNdx, 1).GetString());
		this->wayName = wstring_to_string(handler.GetParameter(rowNdx, 2).GetString());
		way_id = handler.GetParameter(rowNdx, 3).GetInteger();
		this->section.dwSize = sizeof(VWaySection);
		this->section.start = way_start;
		this->section.end = way_end;
	}
	catch(...)
	{
		this->message = L"Ошибка при получении информации о пути";
		db.RollbackTransaction(trans);
		return false;
	}

	std::string datebuf = rideDate.DateToStringISO();

	// ------------------------------------------------------------
	// километровая сетка
	// ------------------------------------------------------------
	for (long km = this->section.start.km; km < this->section.end.km; ++km)
		// FixMe!!! Лучше так: for (long km = way_start.km; km < way_end.km; ++km)
	{
		VAprinfoKmRecord kmRecord;
		kmRecord.km = km;
		kmRecord.len = 1000;
		this->kmList.Add(kmRecord);
	}
	if (this->section.end.m > 0)
	{
		VAprinfoKmRecord kmRecord;
		kmRecord.km = this->section.end.km;
		kmRecord.len = this->section.end.m;
		this->kmList.Add(kmRecord);
	}

	// нестандартные километры
	sql << "select tpl_nst_km.km, tpl_nst_km.len " << std::endl;
	sql << "from tpl_nst_km inner join tpl_nst_km_period ";
	sql << "on tpl_nst_km.tpl_nst_km_period_id = tpl_nst_km_period.id " << std::endl;
	sql << "where tpl_nst_km_period.adm_way_id = " << way_id << " and ";
	sql << "tpl_nst_km_period.start_date < '" << datebuf << "' and ";
	sql << "tpl_nst_km_period.end_date > '" << datebuf << "'" << std::endl;
	sql << "order by tpl_nst_km.km;";

	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
	{
		__int64 km_num, km_len;

		try
		{
			km_num = handler.GetParameter(rowNdx, 0).GetInteger();
			km_len = handler.GetParameter(rowNdx, 1).GetInteger();
		}
		catch(...)
		{
			this->message = L"Ошибка при получении информации о нестандартных км";
			db.RollbackTransaction(trans);
			return false;
		}

		for (size_t i = 0; i < this->kmList.GetCount(); ++i)
		{
			if (this->kmList.GetItem(i).km == km_num)
			{
				this->kmList.GetItem(i).len = (float) km_len;
				break;
			}
		}
	}

	// завершение формирования километровой сетки
	for (size_t i = 0; i < this->kmList.GetCount(); ++i)
	{
		if (i == 0)	this->kmList.GetItem(i).base = this->kmList.GetItem(i).km * 1000.0;
		else		this->kmList.GetItem(i).base = this->kmList.GetItem(i-1).end;

		this->kmList.GetItem(i).end = kmList.GetItem(i).base + kmList.GetItem(i).len;
	}

	// границы пути
	this->absCoordBase = this->kmList.GetItem(0).base;
	this->absCoordEnd = this->kmList.GetLastItem().end;

	// ------------------------------------------------------------
	// Нормы ШК в прямых
	// ------------------------------------------------------------
	sql << "select enu_norma_width.norma, apr_norma_width.start_km, apr_norma_width.start_m, apr_norma_width.end_km, apr_norma_width.end_m " << std::endl;
	sql << "from apr_norma_width inner join apr_norma_width_period on apr_norma_width.apr_norma_width_period_id = apr_norma_width_period.id " << std::endl;
	sql << "inner join enu_norma_width on apr_norma_width.enu_norma_width_id = enu_norma_width.id " << std::endl;
	sql << "where apr_norma_width_period.adm_way_id = " << way_id << " and ";
	sql << "apr_norma_width_period.start_date < '" << datebuf << "' and ";
	sql << "apr_norma_width_period.end_date > '" << datebuf << "'" << std::endl;
	sql	<< "order by apr_norma_width.start_km, apr_norma_width.start_m;";

	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{	
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{	
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			VAprinfoIndexRecord norma;
			double begin, end;
			__int64 value;

			try
			{
				value = handler.GetParameter(rowNdx, 0).GetInteger();
				begin = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 1).GetInteger(),
					handler.GetParameter(rowNdx, 2).GetFloat()));
				end = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 3).GetInteger(),
					handler.GetParameter(rowNdx, 4).GetFloat()));
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации о нормах ШК";
				db.RollbackTransaction(trans);
				return false;
			}

			// пропускаем участок, лежащий за границами выбранного пути
			if (begin >= this->GetAbsCoordEnd()) continue;
			if (end <= this->GetAbsCoordBase()) continue;

			if (this->nwidthStraightList.GetCount() > 0 &&
				this->nwidthStraightList.GetLastItem().end == begin &&
				this->nwidthStraightList.GetLastItem().value == value)
			{
				this->nwidthStraightList.GetLastItem().end = end;
			}
			else
			{
				norma.value = value;
				norma.base = begin;
				norma.end = end;
				this->nwidthStraightList.Add(norma);
			}
		}
		this->nwidthStraightList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 1520);
	}

	// ------------------------------------------------------------
	// Нормы возвышения
	// ------------------------------------------------------------
	sql << "select enu_norma_level.norma, apr_norma_level.start_km, apr_norma_level.start_m, apr_norma_level.end_km, apr_norma_level.end_m " << std::endl;
	sql << "from apr_norma_level inner join apr_norma_level_period on apr_norma_level.apr_norma_level_period_id = apr_norma_level_period.id " << std::endl;
	sql << "inner join enu_norma_level on apr_norma_level.enu_norma_level_id = enu_norma_level.id " << std::endl;
	sql << "where apr_norma_level_period.adm_way_id = " << way_id << " and ";
	sql << "apr_norma_level_period.start_date < '" << datebuf << "' and ";
	sql << "apr_norma_level_period.end_date > '" << datebuf << "'" << std::endl;
	sql << "order by apr_norma_level.start_km, apr_norma_level.start_m;";

	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			VAprinfoIndexRecord norma;
			double begin, end;
			int64_t value;

			try
			{
				value = handler.GetParameter(rowNdx, 0).GetInteger();
				begin = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 1).GetInteger(),
					handler.GetParameter(rowNdx, 2).GetFloat()));
				end = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 3).GetInteger(),
					handler.GetParameter(rowNdx, 4).GetFloat()));
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации о нормах возвышения";
				db.RollbackTransaction(trans);
				return false;
			}

			// пропускаем участок, лежащий за границами выбранного пути
			if (begin >= this->GetAbsCoordEnd()) continue;
			if (end <= this->GetAbsCoordBase()) continue;

			norma.value = value;
			norma.base = begin;
			norma.end = end;

			if (this->nlevelStraightList.GetCount() > 0 &&
				norma.base == this->nlevelStraightList.GetLastItem().end &&
				norma.value == this->nlevelStraightList.GetLastItem().value)
			{
				this->nlevelStraightList.GetLastItem().end = norma.end;
			}
			else
			{
				this->nlevelStraightList.Add(norma);
			}
		}
		this->nlevelStraightList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 0);
	}

	// ------------------------------------------------------------
	// Контрольные участки
	// ------------------------------------------------------------
	sql << "select apr_check_sections.start_km, apr_check_sections.start_m, ";
	sql << "apr_check_sections.end_km, apr_check_sections.end_m, " << std::endl;
	sql << "apr_check_sections.avg_width, apr_check_sections.avg_level, " << std::endl;
	sql << "apr_check_sections.sko_width, apr_check_sections.sko_level " << std::endl;
	sql << "from apr_check_sections inner join apr_check_sections_period on apr_check_sections.apr_check_sections_period_id = apr_check_sections_period.id " << std::endl;
	sql << "where apr_check_sections_period.adm_way_id = " << way_id << " and ";
	sql << "apr_check_sections_period.start_date < '" << datebuf << "' and ";
	sql << "apr_check_sections_period.end_date > '" << datebuf << "'" << std::endl;
	sql << "order by apr_check_sections.start_km, apr_check_sections.start_m;";

	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			double mo_width, mo_level, sko_width, sko_level;
			double begin, end;
			VAprinfoControlAreaRecord record;

			try
			{	
				begin = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 0).GetInteger(),
					handler.GetParameter(rowNdx, 1).GetFloat()));
				end = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 2).GetInteger(),
					handler.GetParameter(rowNdx, 3).GetFloat()));

				mo_width = handler.GetParameter(rowNdx, 4).GetFloat();
				mo_level = handler.GetParameter(rowNdx, 5).GetFloat();
				sko_width = handler.GetParameter(rowNdx, 6).GetFloat();
				sko_level = handler.GetParameter(rowNdx, 7).GetFloat();
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации о контрольных участках";
				db.RollbackTransaction(trans);
				return false;
			}

			// пропускаем участок, лежащий за границами выбранного пути
			if (begin >= this->GetAbsCoordEnd()) continue;
			if (end <= this->GetAbsCoordBase()) continue;

			record.base = begin;
			record.end = end;
			record.mo_width = mo_width;
			record.sko_width = sko_width;
			record.mo_level = mo_level;
			record.sko_level = sko_level;
			record.isThere = true;
			this->controlAreaList.Add(record);
		}
	}
	this->controlAreaList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 0, 0);

	// ------------------------------------------------------------
	// Станции
	// ------------------------------------------------------------
	sql << "select adm_station.code, adm_station.name, ";
	sql << "tpl_station_section.start_km, tpl_station_section.start_m, ";
	sql << "tpl_station_section.end_km, tpl_station_section.end_m, ";
	sql << "tpl_station_section.axis_km, tpl_station_section.axis_m " << std::endl;
	sql << "from adm_station inner join tpl_station_section on adm_station.id = tpl_station_section.adm_station_id ";
	sql << "inner join tpl_station_section_period on ";
	sql << "tpl_station_section_period.id = tpl_station_section.tpl_station_section_period_id " << std::endl;
	sql << "where tpl_station_section_period.adm_way_id = " << way_id << " ";
	sql << "order by axis_km, axis_m;";

	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{	
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		double prev_end = this->GetAbsCoordBase();
		std::string prev_name = "????";
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			std::string code, name;
			double begin, end, axis;

			try
			{
				code = wstring_to_string(handler.GetParameter(rowNdx, 0).GetString());
				name = wstring_to_string(handler.GetParameter(rowNdx, 1).GetString());
				begin = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 2).GetInteger(),
					handler.GetParameter(rowNdx, 3).GetFloat()));
				end = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 4).GetInteger(),
					handler.GetParameter(rowNdx, 5).GetFloat()));
				axis = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 6).GetInteger(),
					handler.GetParameter(rowNdx, 7).GetFloat()));
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации о станции";
				db.RollbackTransaction(trans);
				return false;
			}

			// перегон перед текущей станцией
			if (begin > prev_end)
			{
				VAprinfoPntRecord pnt;
				pnt.base = prev_end;
				pnt.end = begin;
				pnt.axis = 0.5 * (pnt.base + pnt.end);
				pnt.isThere = false;
				pnt.code = 0;
				pnt.name = prev_name + " - " + name;
				pnt.nameReverse = name + " - " + prev_name;
				this->pntList.Add(pnt);
			}

			// текущая станция
			VAprinfoPntRecord pnt;
			pnt.base = begin;
			pnt.end = end;
			pnt.isThere = true;
			pnt.axis = axis;
			pnt.code = atoi(code.c_str());
			pnt.name = name;
			pnt.nameReverse = name;
			this->pntList.Add(pnt);

			prev_end = end;
			prev_name = name;
		}

		VAprinfoPntRecord pnt;
		pnt.base = prev_end;
		pnt.end = this->GetAbsCoordEnd();
		pnt.isThere = false;
		pnt.axis = 0.5 * (pnt.base + pnt.end);
		pnt.code = 0;
		pnt.name = prev_name + " - ????";
		pnt.nameReverse = "???? - " + prev_name;
		this->pntList.Add(pnt);
		this->pntList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
	}

	// ------------------------------------------------------------
	// ПЧ
	// ------------------------------------------------------------
	sql << "select adm_distantion.code, adm_nod.code, adm_road.code , ";
	sql << "adm_distantion.name, adm_nod.name, adm_road.name, ";
	sql << "tpl_dist_section.start_km, tpl_dist_section.start_m, ";
	sql << "tpl_dist_section.end_km, tpl_dist_section.end_m " << std::endl;
	sql << "from adm_distantion inner join tpl_dist_section ";
	sql << "on adm_distantion.id = tpl_dist_section.adm_distantion_id ";
	sql << "inner join tpl_dist_section_period ";
	sql << "on tpl_dist_section.tpl_dist_section_period_id = tpl_dist_section_period.id ";
	sql << "inner join adm_nod on adm_distantion.adm_nod_id = adm_nod.id " << std::endl;
	sql << "inner join adm_road on adm_nod.adm_road_id = adm_road.id " << std::endl;
	sql << "where tpl_dist_section_period.adm_way_id = " << way_id << " " << std::endl;
	sql << "order by start_km, start_m;";

	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			double begin, end;

			try
			{			
				begin = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 6).GetInteger(),
					handler.GetParameter(rowNdx, 7).GetFloat()));

				end = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 8).GetInteger(),
					handler.GetParameter(rowNdx, 9).GetFloat()));

				// пропускаем участок, лежащий за границами выбранного пути
				if (begin >= this->GetAbsCoordEnd()) continue;
				if (end <= this->GetAbsCoordBase()) continue;

				VAprinfoPchRecord pch;
				pch.pchCode = _wtoi(handler.GetParameter(rowNdx, 0).GetString().c_str());
				pch.pchName = wstring_to_string(handler.GetParameter(rowNdx, 3).GetString());
				pch.nodCode = _wtoi(handler.GetParameter(rowNdx, 1).GetString().c_str());
				pch.nodName = wstring_to_string(handler.GetParameter(rowNdx, 4).GetString());
				pch.roadCode = _wtoi(handler.GetParameter(rowNdx, 2).GetString().c_str());
				pch.roadName = wstring_to_string(handler.GetParameter(rowNdx, 5).GetString());
				pch.base = begin;
				pch.end = end;
				this->pchList.Add(pch);
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации об участках ПЧ";
				db.RollbackTransaction(trans);
				return false;
			}
		}
		this->pchList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
	}

	// ------------------------------------------------------------
	// Скорости по приказу
	// ------------------------------------------------------------
	sql << "select apr_speed.speed1, apr_speed.speed2, apr_speed.speed3, "; 
	sql << "apr_speed.start_km, apr_speed.start_m, apr_speed.end_km, apr_speed.end_m " << std::endl;
	sql	<< "from apr_speed inner join apr_speed_period on apr_speed.apr_speed_period_id = apr_speed_period.id " << std::endl;
	sql << "where apr_speed_period.adm_way_id = " << way_id << " ";
	sql << "and apr_speed_period.start_date < '" << datebuf << "' ";
	sql << "and apr_speed_period.end_date > '" << datebuf << "' " << std::endl;
	sql << "order by start_km, start_m;";

	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			try
			{
				double begin = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 3).GetInteger(),
					handler.GetParameter(rowNdx, 4).GetFloat()));

				double end = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 5).GetInteger(),
					handler.GetParameter(rowNdx, 6).GetFloat()));

				// пропускаем участок, лежащий за границами выбранного пути
				if (begin >= this->GetAbsCoordEnd()) continue;
				if (end <= this->GetAbsCoordBase()) continue;

				VAprinfoSpeedRecord spd;
				spd.spd = VSpeeds(handler.GetParameter(rowNdx, 0).GetInteger(),
					handler.GetParameter(rowNdx, 1).GetInteger(),
					handler.GetParameter(rowNdx, 2).GetInteger());
				spd.base = begin;
				spd.end = end;
				this->speedList.Add(spd);
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации о скоростях";
				db.RollbackTransaction(trans);
				return false;
			}
		}
		if (this->speedList.SyncSpdRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd()))
		{
			this->warning = true;
			this->message = ERRMSG_APRINFO_HOLE_IN_SPD;
		}
	}

	std::vector<__tmpSpdArea> hiSpdAreas, hiSpdBtAreas;

	// ------------------------------------------------------------
	// Высокоскоростные участки
	// ------------------------------------------------------------
	sql << "select apr_speed_hs.speed, apr_speed_hs.start_km, apr_speed_hs.start_m, ";
	sql << "apr_speed_hs.end_km, apr_speed_hs.end_m " << std::endl;
	sql	<< "from apr_speed_hs inner join apr_speed_hs_period ";
	sql << "on apr_speed_hs.apr_speed_hs_period_id = apr_speed_hs_period.id " << std::endl;
	sql << "where apr_speed_hs_period.adm_way_id = " << way_id << " ";
	sql << "and apr_speed_hs_period.start_date < '" << datebuf << "' ";
	sql << "and apr_speed_hs_period.end_date > '" << datebuf << "' " << std::endl;
	sql << "order by start_km, start_m;";

	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			try
			{
				double begin = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 1).GetInteger(),
					handler.GetParameter(rowNdx, 2).GetFloat()));

				double end = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 3).GetInteger(),
					handler.GetParameter(rowNdx, 4).GetFloat()));

				// пропускаем участок, лежащий за границами выбранного пути
				if (begin >= this->GetAbsCoordEnd()) continue;
				if (end <= this->GetAbsCoordBase()) continue;

				__tmpSpdArea tsa;
				tsa.value = handler.GetParameter(rowNdx, 0).GetInteger();
				tsa.begin = begin;
				tsa.end = end;
				hiSpdAreas.push_back(tsa);
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации об участках высокоскоростного движения";
				db.RollbackTransaction(trans);
				return false;
			}
		}
	}

	// ------------------------------------------------------------
	// Высокоскоростные участки с наклоном кузова
	// ------------------------------------------------------------
	sql << "select apr_speed_hs_bt.speed, apr_speed_hs_bt.start_km, apr_speed_hs_bt.start_m, ";
	sql << " apr_speed_hs_bt.end_km, apr_speed_hs_bt.end_m " << std::endl;
	sql	<< "from apr_speed_hs_bt inner join apr_speed_hs_bt_period on ";
	sql << "apr_speed_hs_bt.apr_speed_hs_bt_period_id = apr_speed_hs_bt_period.id " << std::endl;
	sql << "where apr_speed_hs_bt_period.adm_way_id = " << way_id << " ";
	sql << "and apr_speed_hs_bt_period.start_date < '" << datebuf << "' ";
	sql << "and apr_speed_hs_bt_period.end_date > '" << datebuf << "' " << std::endl;
	sql << "order by start_km, start_m;";

	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			try
			{
				double begin = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 1).GetInteger(),
					handler.GetParameter(rowNdx, 2).GetFloat()));

				double end = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 3).GetInteger(),
					handler.GetParameter(rowNdx, 4).GetFloat()));

				// пропускаем участок, лежащий за границами выбранного пути
				if (begin >= this->GetAbsCoordEnd()) continue;
				if (end <= this->GetAbsCoordBase()) continue;

				__tmpSpdArea tsa;
				tsa.value = handler.GetParameter(rowNdx, 0).GetInteger();
				tsa.begin = begin;
				tsa.end = end;
				hiSpdBtAreas.push_back(tsa);
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации об участках высокоскоростного движения с наклоном кузова";
				db.RollbackTransaction(trans);
				return false;
			}
		}
	}
	
	if (!hiSpdAreas.empty() || !hiSpdBtAreas.empty())
	{
    	std::set<double> selection_points;
	    selection_points.insert(this->speedList.GetItem(0).base);
    	for (size_t i = 0; i < this->speedList.GetCount(); ++i)
	    {
            selection_points.insert(this->speedList.GetItem(i).end);	    
	    }
	
	    if (!hiSpdAreas.empty())
	    {
        	selection_points.insert(hiSpdAreas[0].begin);
    	    for (size_t i = 0; i < hiSpdAreas.size(); ++i)
    	    {
	            selection_points.insert(hiSpdAreas[i].end);
	        }
	    }

	    if (!hiSpdBtAreas.empty())
	    {
        	selection_points.insert(hiSpdBtAreas[0].begin);
    	    for (size_t i = 0; i < hiSpdBtAreas.size(); ++i)
    	    {
	            selection_points.insert(hiSpdBtAreas[i].end);
	        }
	    }

        if (selection_points.size() >= 2) 
        {
            VAprinfoParameter<VAprinfoSpeedRecord> newSpd;
        
            std::set<double>::iterator prev = selection_points.begin();
            for (std::set<double>::iterator cur = selection_points.begin(); cur != selection_points.end(); cur++)
            {
                if (prev == cur) // first point
                {
                    continue;
                }
                else
                {
                    VAprinfoSpeedRecord s;
                    s.base = *prev;
                    s.end = *cur;
                    double axis = 0.5 * (s.base + s.end);
                    s.spd = this->GetSpeeds(axis);

                    for (size_t i = 0; i < hiSpdAreas.size(); ++i)
                    {
                        if (axis > hiSpdAreas[i].begin && axis < hiSpdAreas[i].end)
                        {
                            s.spd.SetHighSpeed(hiSpdAreas[i].value);
                        }
                    }
                    
                    for (size_t i = 0; i < hiSpdBtAreas.size(); ++i)
                    {
                        if (axis > hiSpdBtAreas[i].begin && axis < hiSpdBtAreas[i].end)
                        {
                            s.spd.SetHighBTSpeed(hiSpdBtAreas[i].value);
                        }
                    }
                    
                    newSpd.Add(s);
                    prev++;
                }
            }
            
            this->speedList = newSpd;
        	this->speedList.Sort();
        }
    }

	// ------------------------------------------------------------
	// Временные скорости
	// ------------------------------------------------------------
	sql << "select apr_tempspeed.speed1, apr_tempspeed.speed2, apr_tempspeed.speed3, "; 
	sql << "apr_tempspeed.start_km, apr_tempspeed.start_m, apr_tempspeed.end_km, apr_tempspeed.end_m " << std::endl;
	sql	<< "from apr_tempspeed inner join apr_tempspeed_period on apr_tempspeed.apr_tempspeed_period_id = apr_tempspeed_period.id " << std::endl;
	sql << "where apr_tempspeed_period.adm_way_id = " << way_id << " ";
	sql << "and apr_tempspeed_period.start_date < '" << datebuf << "' ";
	sql << "and apr_tempspeed_period.end_date > '" << datebuf << "' " << std::endl;
	sql << "order by start_km, start_m;";

	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			try
			{
				double begin = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 3).GetInteger(),
					handler.GetParameter(rowNdx, 4).GetFloat()));

				double end = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 5).GetInteger(),
					handler.GetParameter(rowNdx, 6).GetFloat()));

				// пропускаем участок, лежащий за границами выбранного пути
				if (begin >= this->GetAbsCoordEnd()) continue;
				if (end <= this->GetAbsCoordBase()) continue;

				VAprinfoSpeedRecord spd;
				spd.spd = VSpeeds(handler.GetParameter(rowNdx, 0).GetInteger(),
					handler.GetParameter(rowNdx, 1).GetInteger(),
					handler.GetParameter(rowNdx, 2).GetInteger());
				spd.base = begin;
				spd.end = end;
				this->speedTmpList.Add(spd);
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации о скоростях";
				db.RollbackTransaction(trans);
				return false;
			}
		}

		this->speedTmpList.SyncSpdRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), VSpeeds());
	}

	// ------------------------------------------------------------
	// Мосты
	// ------------------------------------------------------------
	sql << "select bld_bridge.name, bld_bridge_section.brlength, ";
	sql << "bld_bridge_section.axis_km, bld_bridge_section.axis_m " << std::endl;
	sql << "from bld_bridge_section inner join bld_bridge on bld_bridge_section.bld_bridge_id = bld_bridge.id ";
	sql << "inner join bld_bridge_section_period on ";
	sql << "bld_bridge_section.bld_bridge_section_period_id = bld_bridge_section_period.id " << std::endl;
	sql << "where bld_bridge_section_period.adm_way_id = " << way_id << " ";
	sql << "and bld_bridge_section_period.start_date < '" << datebuf << "' ";
	sql << "and bld_bridge_section_period.end_date > '" << datebuf << "' ";
	sql << "and bld_bridge.start_date < '" << datebuf << "' ";
	sql << "and bld_bridge.end_date > '" << datebuf << "' " << std::endl;
	sql << "order by axis_km, axis_m;";

	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		double prev_end = this->GetAbsCoordBase(), prev_len = 0.0;
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			try
			{
				std::string name = wstring_to_string(handler.GetParameter(rowNdx, 0).GetString());
				double len = handler.GetParameter(rowNdx, 1).GetFloat();
				double axis = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 2).GetInteger(),
					handler.GetParameter(rowNdx, 3).GetFloat()));

				double begin = axis - 0.5 * len;
				double end = axis + 0.5 * len;

				if (begin < this->GetAbsCoordBase() ||
					end > this->GetAbsCoordEnd())
				{
					continue;
				}

				// участок перед первым мостом, либо между мостами
				this->bridgeList.Add(BuildSafeZone(prev_end, begin, prev_len, len));

				VAprinfoIndexRecord bridge;
				bridge.base = begin;
				bridge.end = end;
				bridge.value = SZO_OBJECT;
				this->bridgeList.Add(bridge);

				prev_end = end;
				prev_len = len;
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации о мостах";
				db.RollbackTransaction(trans);
				return false;
			}
		}

		// участок после последнего моста
		this->bridgeList.Add(BuildSafeZone(prev_end, this->GetAbsCoordEnd(), prev_len, 0.0));
		this->bridgeList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
	}

	// ------------------------------------------------------------
	// Тоннели
	// ------------------------------------------------------------
	sql << "select bld_tunnel.name, bld_tunnel_section.start_km, bld_tunnel_section.start_m, ";
	sql << "bld_tunnel_section.end_km, bld_tunnel_section.end_m " << std::endl;
	sql << "from bld_tunnel_section inner join bld_tunnel on bld_tunnel_section.bld_tunnel_id = bld_tunnel.id ";
	sql << "inner join bld_tunnel_section_period on ";
	sql << "bld_tunnel_section.bld_tunnel_section_period_id = bld_tunnel_section_period.id " << std::endl;
	sql << "where bld_tunnel_section_period.adm_way_id = " << way_id << " ";
	sql << "and bld_tunnel_section_period.start_date < '" << datebuf << "' ";
	sql << "and bld_tunnel_section_period.end_date > '" << datebuf << "' ";
	sql << "and bld_tunnel.start_date < '" << datebuf << "' ";
	sql << "and bld_tunnel.end_date > '" << datebuf << "' " << std::endl;
	sql << "order by bld_tunnel_section.start_km, bld_tunnel_section.start_m;";

	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		double prev_end = this->GetAbsCoordBase(), prev_len = 0.0;
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			try
			{
				std::string name = wstring_to_string(handler.GetParameter(rowNdx, 0).GetString());
				double begin = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 1).GetInteger(),
					handler.GetParameter(rowNdx, 2).GetFloat()));
				double end = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 3).GetInteger(),
					handler.GetParameter(rowNdx, 4).GetFloat()));
				double len = end - begin;

				if (begin < this->GetAbsCoordBase() ||
					end > this->GetAbsCoordEnd())
				{
					continue;
				}

				// участок перед первым мостом, либо между мостами
				this->tonnelList.Add(BuildSafeZone(prev_end, begin, prev_len, len));

				VAprinfoIndexRecord tonnel;
				tonnel.base = begin;
				tonnel.end = end;
				tonnel.value = SZO_OBJECT;
				this->tonnelList.Add(tonnel);

				prev_end = end;
				prev_len = len;
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации о тоннелях";
				db.RollbackTransaction(trans);
				return false;
			}
		}

		// участок после последнего моста
		this->tonnelList.Add(BuildSafeZone(prev_end, this->GetAbsCoordEnd(), prev_len, 0.0));
		this->tonnelList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
	}

	// ------------------------------------------------------------
	// Категории пути
	// ------------------------------------------------------------
	sql << "select apr_waycat.enu_waycat_id, apr_waycat.start_km, apr_waycat.start_m, ";
	sql << "apr_waycat.end_km, apr_waycat.end_m " << std::endl;
	sql << "from apr_waycat inner join apr_waycat_period ";
	sql << "on apr_waycat.apr_waycat_period_id = apr_waycat_period.id " << std::endl;
	sql << "where apr_waycat_period.adm_way_id = " << way_id << " ";
	sql << "and apr_waycat_period.start_date < '" << datebuf << "' ";
	sql << "and apr_waycat_period.end_date > '" << datebuf << "' " << std::endl;
	sql << "order by apr_waycat.start_km, apr_waycat.start_m;";

	try
	{	
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			try
			{
				double begin = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 1).GetInteger(),
					handler.GetParameter(rowNdx, 2).GetFloat()));
				double end = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 3).GetInteger(),
					handler.GetParameter(rowNdx, 4).GetFloat()));

				// пропускаем участок, лежащий за границами выбранного пути
				if (begin >= this->GetAbsCoordEnd()) continue;
				if (end <= this->GetAbsCoordBase()) continue;

				VAprinfoIndexRecord wayCat;
				wayCat.value = handler.GetParameter(rowNdx, 0).GetInteger();
				wayCat.base = begin;
				wayCat.end = end;
				this->wayCatList.Add(wayCat);
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации о категориях пути";
				db.RollbackTransaction(trans);
				return false;
			}
		}
		this->wayCatList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
	}

	// ------------------------------------------------------------
	// Метки профиля
	// ------------------------------------------------------------
	sql << "select apr_profmarks.profil, apr_profmarks.km, apr_profmarks.m ";
	sql << "from apr_profmarks inner join apr_profmarks_period ";
	sql << "on apr_profmarks.apr_profmarks_period_id = apr_profmarks_period.id ";
	sql << "where apr_profmarks_period.adm_way_id = " << way_id << " ";
	sql << "and apr_profmarks_period.start_date < '" << datebuf << "' ";
	sql << "and apr_profmarks_period.end_date > '" << datebuf << "' " << std::endl;
	sql << "order by apr_profmarks.km, apr_profmarks.m;";

	try
	{	
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			try
			{
				VAprinfoProfLabel prof;
				prof.value = handler.GetParameter(rowNdx, 0).GetFloat();
				prof.coord = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 1).GetInteger(),
					handler.GetParameter(rowNdx, 2).GetFloat()));
				this->profList.Add(prof);
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации о проектных метках профиля";
				db.RollbackTransaction(trans);
				return false;
			}
		}
	}

	// ------------------------------------------------------------
	// переезды
	// ------------------------------------------------------------
	sql << "select apr_crossings.km, apr_crossings.m, apr_crossings.name, enu_side_id ";
	sql << "from apr_crossings inner join apr_crossings_period ";
	sql << "on apr_crossings.apr_crossings_period_id=apr_crossings_period.id ";
	sql << "where apr_crossings_period.adm_way_id = " << way_id << " ";
	sql << "and apr_crossings_period.start_date < '" << datebuf << "' ";
	sql << "and apr_crossings_period.end_date > '" << datebuf << "' ";
	sql << "order by apr_crossings.km, apr_crossings.m;";

	try
	{	
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			VAprinfoCrossing crossing;
			crossing.coord = this->GetAbsCoord(VWayCoord(
				handler.GetParameter(rowNdx, 0).GetInteger(),
				handler.GetParameter(rowNdx, 1).GetFloat()));
			crossing.name = handler.GetParameter(rowNdx, 2).GetString();
			crossing.side = handler.GetParameter(rowNdx, 3).GetInteger();
			this->crossingList.Add(crossing);
		}
	}

	// ------------------------------------------------------------
	// Стрелки
	// ------------------------------------------------------------
	sql << "select tpl_switch.km, tpl_switch.m, tpl_switch.enu_switch_dir_id, ";
	sql << "tpl_switch.enu_side_id, tpl_switch.num, enu_cross_mark.mark ";
	sql << "from tpl_switch inner join tpl_switch_period ";
	sql << "on tpl_switch.tpl_switch_period_id = tpl_switch_period.id ";
	sql << "inner join enu_cross_mark on tpl_switch.enu_cross_mark_id = enu_cross_mark.id ";
	sql << "where tpl_switch_period.adm_way_id = " << way_id << " ";
	sql << "and tpl_switch_period.start_date < '" << datebuf << "' ";
	sql << "and tpl_switch_period.end_date > '" << datebuf << "' " << std::endl;
	sql << "order by tpl_switch.km, tpl_switch.m;";

	try
	{	
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		VAprinfoSwitchRecord sw;
		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			try
			{
				// остряк
				double ooo = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 0).GetInteger(),
					handler.GetParameter(rowNdx, 1).GetFloat()));

				sw.isThere = true;
				sw.dir = static_cast<SWITCH_DIR>(handler.GetParameter(rowNdx, 2).GetInteger());
				sw.side = handler.GetParameter(rowNdx, 3).GetInteger();
				sw.num = wstring_to_string(handler.GetParameter(rowNdx, 4).GetString());
				sw.cross_mark = wstring_to_string(handler.GetParameter(rowNdx, 5).GetString());

				if (sw.dir > 0)
				{
					sw.o = ooo;
					sw.x = ooo + 30;
					sw.base = sw.o - 5.0;
					sw.end = sw.x + 5.0;
				}
				else if (sw.dir < 0)
				{
					sw.o = ooo;
					sw.x = ooo - 30;
					sw.base = sw.x - 5.0;
					sw.end = sw.o + 5.0;
				}
				else
				{
					sw.o = 0;
					sw.x = 0;
					sw.base = ooo - 20.0;
					sw.end = ooo + 20.0;
				}
				this->switchList.Add(sw);
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации о стрелках";
				db.RollbackTransaction(trans);
				return false;
			}
		}

		this->switchList.SyncSwitchRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
	}

	// ------------------------------------------------------------
	// Плети
	// ------------------------------------------------------------
	sql << "select apr_long_rails.id, apr_long_rails.enu_side_id, apr_long_rails.num, ";
	sql << "apr_long_rails.start_km, apr_long_rails.start_m, apr_long_rails.end_km, apr_long_rails.end_m ";
	sql << "from apr_long_rails inner join apr_long_rails_period ";
	sql << "on apr_long_rails.apr_long_rails_period_id = apr_long_rails_period.id ";
	sql << "where apr_long_rails_period.adm_way_id = " << way_id << " ";
	sql << "and apr_long_rails_period.start_date < '" << datebuf << "' ";
	sql << "and apr_long_rails_period.end_date > '" << datebuf << "' ";
	sql << "order by apr_long_rails.start_km, apr_long_rails.start_m;";

	try
	{	
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		VAprinfoWobRecord wobRecord;

		for (size_t rowNdx = 0; rowNdx < handler.GetRowCount(); ++rowNdx)
		{
			wobRecord.isThere = true;
			wobRecord.num = wstring_to_string(handler.GetParameter(rowNdx, 2).GetString());
			wobRecord.base = this->GetAbsCoord(VWayCoord(
				handler.GetParameter(rowNdx, 3).GetInteger(), handler.GetParameter(rowNdx, 4).GetFloat()));
			wobRecord.end = this->GetAbsCoord(VWayCoord(
				handler.GetParameter(rowNdx, 5).GetInteger(), handler.GetParameter(rowNdx, 6).GetFloat()));

			VIBDataHandlerParameter handler2;
			sql << "select km, m, fixtemp, maxtemp, mintemp, start_shift from apr_long_marks ";
			sql << "where apr_long_rails_id = " << handler.GetParameter(rowNdx, 0).GetInteger() << " ";
			sql << "order by km, m;";
			try
			{	
				db.ExecuteStatement(trans, sql.str().c_str(), handler2);
			}
			catch(...)
			{
				this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
				db.RollbackTransaction(trans);
				return false;
			}
			sql.str("");

			for (size_t j = 0; j < handler2.GetRowCount(); ++j)
			{
				VAprinfoWobLabel label;
				label.coord = this->GetAbsCoord(VWayCoord(
					handler2.GetParameter(j, 0).GetInteger(), 
					handler2.GetParameter(j, 1).GetFloat()));
				label.fixTemp = handler2.GetParameter(j, 2).GetInteger();
				label.minTemp = handler2.GetParameter(j, 3).GetInteger();
				label.maxTemp = handler2.GetParameter(j, 4).GetInteger();
				label.startShift = handler2.GetParameter(j, 5).GetInteger();

				wobRecord.labels.push_back(label);
			}

			long side = handler.GetParameter(rowNdx, 1).GetInteger();
			if (side < 0)		this->wobLeftList.Add(wobRecord);
			else if (side > 0)	this->wobRightList.Add(wobRecord);
		}
	}

	this->wobLeftList.SyncWobRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
	this->wobRightList.SyncWobRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
	
	// ------------------------------------------------------------
	// Типы шпал
	// ------------------------------------------------------------
	sql << "select apr_crosstie.enu_crosstie_type_id, ";
	sql << "apr_crosstie.start_km, apr_crosstie.start_m, ";
	sql << "apr_crosstie.end_km, apr_crosstie.end_m ";
	sql << "from apr_crosstie inner join apr_crosstie_period ";
	sql << "on apr_crosstie.apr_crosstie_period_id = apr_crosstie_period.id ";
	sql << "where apr_crosstie_period.adm_way_id = " << way_id << " ";
	sql << "and apr_crosstie_period.start_date < '" << datebuf << "' ";
	sql << "and apr_crosstie_period.end_date > '" << datebuf << "' " << std::endl;
	sql << "order by apr_crosstie.start_km, apr_crosstie.start_m;";

	try
	{	
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		VAprinfoIndexRecord spalaRecord;

		for (size_t rowNdx = 0; rowNdx < rowCount; ++rowNdx)
		{
			try
			{
				double begin = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 1).GetInteger(),
					handler.GetParameter(rowNdx, 2).GetFloat()));

				double end = this->GetAbsCoord(VWayCoord(
					handler.GetParameter(rowNdx, 3).GetInteger(),
					handler.GetParameter(rowNdx, 4).GetFloat()));

				// пропускаем участок, лежащий за границами выбранного пути
				if (begin >= this->GetAbsCoordEnd()) continue;
				if (end <= this->GetAbsCoordBase()) continue;

				VAprinfoIndexRecord spala;
				//long type_id = GetIntParam(handler, rowNdx, "enu_crosstie_type_id";
				__int64 type_id = handler.GetParameter(rowNdx, 0).GetInteger();
				switch(type_id)
				{
				case 1: spala.value = SPALA_GB96; break;
				case 3:
				case 4: spala.value = SPALA_WOOD; break;
				default: spala.value = SPALA_GB;
				}
				spala.base = begin;
				spala.end = end;
				this->spalaList.Add(spala);
			}
			catch(...)
			{
				this->message = L"Ошибка при получении информации о категориях пути";
				db.RollbackTransaction(trans);
				return false;
			}
		}
		this->spalaList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), SPALA_DEFAULT);
	}

	// ------------------------------------------------------------
	// Рихтовочные нити в прямых
	// ------------------------------------------------------------
	sql << "select ss.enu_side_id, ss.start_km, ss.start_m, ss.end_km, ss.end_m ";
	sql << "from apr_straight_side ss inner join apr_straight_side_period ssp ";
	sql << "on ss.apr_straight_side_period_id = ssp.id ";
	sql << "where ssp.adm_way_id = " << way_id << " ";
	sql << "and ssp.start_date < '" << datebuf << "' ";
	sql << "and ssp.end_date > '" << datebuf << "' ";
	sql << "order by ss.start_km, ss.start_m;";

	try
	{	
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	for (size_t i = 0; i < handler.GetRowCount(); ++i)
	{
		VAprinfoIndexRecord thread;
		thread.value = handler.GetParameter(i, 0).GetInteger();
		thread.base = this->GetAbsCoord(VWayCoord(
			handler.GetParameter(i, 1).GetInteger(),
			handler.GetParameter(i, 2).GetFloat()));
		thread.end = this->GetAbsCoord(VWayCoord(
			handler.GetParameter(i, 3).GetInteger(),
			handler.GetParameter(i, 4).GetFloat()));
		this->threadStraightList.Add(thread);
	}
	this->threadStraightList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		SD_RIGHT);

	// ------------------------------------------------------------
	// Отверстия
	// ------------------------------------------------------------
	sql << "select enu_norma_diameter.norma, ";
	sql << "apr_boltslot.start_km, apr_boltslot.start_m, ";
	sql << "apr_boltslot.end_km, apr_boltslot.end_m ";
	sql << "from apr_boltslot inner join apr_boltslot_period ";
	sql << "on apr_boltslot.apr_boltslot_period_id = apr_boltslot_period.id ";
	sql << "inner join enu_norma_diameter ";
	sql << "on enu_norma_diameter.id = apr_boltslot.enu_norma_diameter_id ";
	sql << "where apr_boltslot_period.adm_way_id = " << way_id << " ";
	sql << "and apr_boltslot_period.start_date < '" << datebuf << "' ";
	sql << "and apr_boltslot_period.end_date > '" << datebuf << "' ";
	sql << "order by start_km, start_m;";

	try
	{	
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");
	for (size_t i = 0; i < handler.GetRowCount(); ++i)
	{
		VAprinfoIndexRecord idxrec;
		idxrec.value = handler.GetParameter(i, 0).GetInteger();
		idxrec.base = this->GetAbsCoord(VWayCoord(
			handler.GetParameter(i, 1).GetInteger(),
			handler.GetParameter(i, 2).GetFloat()));
		idxrec.end = this->GetAbsCoord(VWayCoord(
			handler.GetParameter(i, 3).GetInteger(),
			handler.GetParameter(i, 4).GetFloat()));
		this->holeList.Add(idxrec);
	}
	this->holeList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 36);

	// ------------------------------------------------------------
	// Кривые - факт
	// ------------------------------------------------------------

    double prev_end, prev_end_level;

    if (mea_curves)
    {
        sql << "select c.id, c.enu_side_id, c.angle from msr_curve c ";
        sql << "inner join msr_plan_elem p on p.id = c.msr_plan_elem_id ";
        sql << "inner join msr_trail t on t.id = p.msr_trail_id ";
        sql << "where enu_elem_type_id = 3 and t.adm_way_id = " << way_id << " and not exists ";
        sql << "(select p1.id from msr_plan_elem p1, msr_trail t1 ";
        sql << "where enu_elem_type_id = 3 and p1.msr_trail_id = t1.id ";
        sql << "and t1.adm_way_id=t.adm_way_id and t1.date_trail > t.date_trail ";
        sql << "and not (p1.start_abs > p.end_abs or p1.end_abs < p.start_abs)) ";
        sql << "order by p.start_abs;";

        try
        {	
            db.ExecuteStatement(trans, sql.str().c_str(), handler);
        }
        catch(...)
        {
            this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
            db.RollbackTransaction(trans);
            return false;
        }
        sql.str("");

        prev_end = this->GetAbsCoordBase();
        prev_end_level = this->GetAbsCoordBase();
        rowCount = handler.GetRowCount();
        if (rowCount > 0)
        {
            VAprinfoCrvRadius radius;
            VAprinfoCurve curve, straight;
            VAprinfoCrvLabel label;
            VIBDataHandlerParameter handler2;

            for (size_t i = 0; i < rowCount; ++i)
            {
                curve.Clear();
                curve.id = handler.GetParameter(i, 0).GetInteger();
                curve.isThere = true;
                curve.orientation = handler.GetParameter(i, 1).GetInteger();
                curve.angle = handler.GetParameter(i, 2).GetFloat();

                sql << "select radius_avg_circ, lvl_avg_circ, wear_avg ";
                sql << "from msr_elcurve where msr_curve_id = " << curve.id << " ";
                sql << "order by radius_number;";
                try
                {	
                    db.ExecuteStatement(trans, sql.str().c_str(), handler2);
                }
                catch(...)
                {
                    this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
                    db.RollbackTransaction(trans);
                    return false;
                }
                sql.str("");

                if (handler2.GetRowCount() == 0) 
                {
                    continue;
                }

                for (size_t j = 0; j < handler2.GetRowCount(); ++j)
                {
                    radius.radius = handler2.GetParameter(j, 0).GetFloat();
                    radius.level = handler2.GetParameter(j, 1).GetFloat();
                    radius.wear = handler2.GetParameter(j, 2).GetFloat();
                    radius.width = 1520;
                    radius.latEnd = 0;
                    radius.latStart = 0;
                    radius.longEnd = 0;
                    radius.longStart = 0;
                    curve.radius.push_back(radius);
                }

                sql << "select crv_abs, crv_value, lvl_abs, lvl_value, latitude, longitude ";
                sql << "from msr_curve_mark where msr_curve_id = " << curve.id << " ";
                sql << "order by crv_abs;";
                try
                {	
                    db.ExecuteStatement(trans, sql.str().c_str(), handler2);
                }
                catch(...)
                {
                    this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
                    db.RollbackTransaction(trans);
                    return false;
                }
                sql.str("");

                if (handler2.GetRowCount() == 0) 
                {
                    continue;
                }

                for (size_t j = 0; j < handler2.GetRowCount(); ++j)
                {
                    label.coord = handler2.GetParameter(j, 0).GetFloat();
                    label.value = handler2.GetParameter(j, 1).GetFloat();
                    curve.planLabel.push_back(label);
                    label.coord = handler2.GetParameter(j, 2).GetFloat();
                    label.value = handler2.GetParameter(j, 3).GetFloat();
                    curve.levelLabel.push_back(label);

                    if (j == 0) // первая метка на кривой
                    {
                        curve.base = handler2.GetParameter(j, 0).GetFloat();
                        curve.levelBase = handler2.GetParameter(j, 2).GetFloat();
                        curve.latStart = handler2.GetParameter(j, 4).GetFloat();
                        curve.longStart = handler2.GetParameter(j, 5).GetFloat();
                    }
                    else if (j == handler2.GetRowCount() - 1) // последняя метка
                    {
                        curve.end = handler2.GetParameter(j, 0).GetFloat();
                        curve.levelEnd = handler2.GetParameter(j, 2).GetFloat();
                        curve.latEnd = handler2.GetParameter(j, 4).GetFloat();
                        curve.longEnd = handler2.GetParameter(j, 5).GetFloat();
                    }
                    else
                    {
                        VAprinfoCrvRadius & r = curve.radius[(j-1)/2];
                        if (((j-1)%2) == 0)
                        {
                            r.latStart = handler2.GetParameter(j, 4).GetFloat();
                            r.longStart = handler2.GetParameter(j, 5).GetFloat(); 
                        }
                        else
                        {
                            r.latEnd = handler2.GetParameter(j, 4).GetFloat();
                            r.longEnd = handler2.GetParameter(j, 5).GetFloat(); 
                        }
                    }
                }

                // валидация кривой
                std::wstring valid = curve.Validate();
                if (!valid.empty())
                {
                    std::wstringstream msg;
                    msg << L"Кривая (факт) " << this->GetRealCoord(curve.base).format_long();
                    msg << L": " << valid << L" (пропущена)";
                    LOG_ERROR(msg.str());
                    continue;
                }

                if (curve.base < prev_end || curve.levelBase < prev_end_level)
                {
                    std::wstringstream msg;
                    msg << L"Кривая (факт) " << this->GetRealCoord(curve.base).format_long();
                    msg << L" пересекается с предыдущей (пропущена)";
                    LOG_ERROR(msg.str());
                    continue;
                }

                // прямая перед кривой
                if (curve.base > prev_end)
                {
                    straight.base = prev_end;
                    straight.end = curve.base;
                    straight.isThere = false;
                    this->crvFact.curveList.Add(straight);
                }
                prev_end = curve.end;
                prev_end_level = curve.levelEnd;

                // кривая
                this->crvFact.curveList.Add(curve);
            }

            straight.base = prev_end;
            straight.end = this->GetAbsCoordEnd();
            straight.isThere = false;
            this->crvFact.curveList.Add(straight);
            this->crvFact.curveList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
            std::vector<std::wstring> zl_result = this->crvFact.BuildZL(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
            for (size_t i = 0; i < zl_result.size(); ++i)
            {
                LOG_WARNING(L"Кривые (факт): " + zl_result[i]);
            }
        }
    }
	// ------------------------------------------------------------
	// Кривые - паспорт
	// ------------------------------------------------------------

	sql << "select apr_curve.id, apr_curve.enu_side_id, apr_curve.angle, ";
	sql << "apr_curve.crv_start_km, apr_curve.crv_start_m, ";
	sql << "apr_curve.lvl_start_km, apr_curve.lvl_start_m, ";
	sql << "apr_curve.crv_end_km, apr_curve.crv_end_m, ";
	sql << "apr_curve.lvl_end_km, apr_curve.lvl_end_m, ";
	sql << "apr_curve.latitude_start, apr_curve.latitude_end, ";
	sql << "apr_curve.longitude_start, apr_curve.longitude_end ";
	sql << "from apr_curve inner join apr_curve_period ";
	sql << "on apr_curve.apr_curve_period_id = apr_curve_period.id ";
	sql << "where apr_curve_period.adm_way_id = " << way_id << " ";
	sql << "and apr_curve_period.start_date < '" << datebuf << "' ";
	sql << "and apr_curve_period.end_date > '" << datebuf << "' " << std::endl;
	sql << "order by apr_curve.crv_start_km, apr_curve.crv_start_m;";

	try
	{	
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	prev_end = this->GetAbsCoordBase();
	prev_end_level = this->GetAbsCoordBase();
	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		VAprinfoCrvRadius radius;
		VAprinfoCurve curve, straight;
		VAprinfoCrvLabel label;
		VIBDataHandlerParameter handler2;

		for (size_t i = 0; i < rowCount; ++i)
		{
			curve.Clear();
			curve.id = handler.GetParameter(i, 0).GetInteger();
			curve.isThere = true;
			curve.orientation = handler.GetParameter(i, 1).GetInteger();
			curve.angle = handler.GetParameter(i, 2).GetFloat();

			// начало кривой
			label.value = 0;
			label.coord = this->GetAbsCoord(VWayCoord(
				handler.GetParameter(i, 3).GetInteger(),
				handler.GetParameter(i, 4).GetFloat()));
			curve.planLabel.push_back(label);
			curve.base = label.coord;

			label.coord = this->GetAbsCoord(VWayCoord(
				handler.GetParameter(i, 5).GetInteger(),
				handler.GetParameter(i, 6).GetFloat()));
			curve.levelLabel.push_back(label);
			curve.levelBase = label.coord;

			curve.latStart = handler.GetParameter(i, 11).GetFloat();
			curve.latEnd = handler.GetParameter(i, 12).GetFloat();
			curve.longStart = handler.GetParameter(i, 13).GetFloat();
			curve.longEnd = handler.GetParameter(i, 14).GetFloat();

			// элементарные кривые
			sql << "select radius, lvl, width, wear, ";
			sql << "latitude_start, latitude_end, ";
			sql << "longitude_start, longitude_end, ";
			sql << "crv_start_km, crv_start_m, crv_len, ";
			sql << "lvl_start_km, lvl_start_m, lvl_len ";
			sql << "from apr_elcurve where apr_curve_id = " << curve.id << " ";
			sql << "order by crv_start_km, crv_start_m;";

			try
			{	
				db.ExecuteStatement(trans, sql.str().c_str(), handler2);
			}
			catch(...)
			{
				this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
				db.RollbackTransaction(trans);
				return false;
			}
			sql.str("");

			size_t row2Count = handler2.GetRowCount();
			for (size_t row2Ndx = 0; row2Ndx < row2Count; ++row2Ndx)
			{
				radius.radius = handler2.GetParameter(row2Ndx, 0).GetFloat();
				radius.level = handler2.GetParameter(row2Ndx, 1).GetFloat();
				radius.width = handler2.GetParameter(row2Ndx, 2).GetFloat();
				radius.wear = handler2.GetParameter(row2Ndx, 3).GetFloat();
				radius.latStart = handler2.GetParameter(row2Ndx, 4).GetFloat();
				radius.latEnd = handler2.GetParameter(row2Ndx, 5).GetFloat();
				radius.longStart = handler2.GetParameter(row2Ndx, 6).GetFloat();
				radius.longEnd = handler2.GetParameter(row2Ndx, 7).GetFloat();
				curve.radius.push_back(radius);

				if (radius.radius > 100)
				{
					label.value = (50000.0 / radius.radius) * curve.orientation;
				}
				label.coord = this->GetAbsCoord(VWayCoord(
					handler2.GetParameter(row2Ndx, 8).GetInteger(),
					handler2.GetParameter(row2Ndx, 9).GetFloat()));
				curve.planLabel.push_back(label);

				label.coord += handler2.GetParameter(row2Ndx, 10).GetFloat();
				curve.planLabel.push_back(label);

				label.value = radius.level * curve.orientation;
				label.coord = this->GetAbsCoord(VWayCoord(
				handler2.GetParameter(row2Ndx, 11).GetInteger(),
					handler2.GetParameter(row2Ndx, 12).GetFloat()));
				curve.levelLabel.push_back(label);

				label.coord += handler2.GetParameter(row2Ndx, 13).GetFloat();
				curve.levelLabel.push_back(label);
			}

			// конец кривой
			label.value = 0;
			label.coord = this->GetAbsCoord(VWayCoord(
				handler.GetParameter(i, 7).GetInteger(),
				handler.GetParameter(i, 8).GetFloat()));
			curve.planLabel.push_back(label);
			curve.end = label.coord;

			label.coord = this->GetAbsCoord(VWayCoord(
				handler.GetParameter(i, 9).GetInteger(),
				handler.GetParameter(i, 10).GetFloat()));
			curve.levelLabel.push_back(label);
			curve.levelEnd = label.coord;

			// валидация кривой
			std::wstring valid = curve.Validate();
			if (!valid.empty())
			{
				std::wstringstream msg;
				msg << L"Кривая (паспорт) " << this->GetRealCoord(curve.base).format_long();
				msg << L": " << valid << L" (пропущена)";
				LOG_WARNING(msg.str());
				continue;
			}

			if (curve.base < prev_end || curve.levelBase < prev_end_level)
			{
				std::wstringstream msg;
				msg << L"Кривая (паспорт) " << this->GetRealCoord(curve.base).format_long();
				msg << L" пересекается с предыдущей (пропущена)";
				LOG_WARNING(msg.str());
				continue;
			}

			// прямая перед кривой
			if (curve.base > prev_end)
			{
				straight.base = prev_end;
				straight.end = curve.base;
				straight.isThere = false;
				this->crvPassport.curveList.Add(straight);
			}
			prev_end = curve.end;
			prev_end_level = curve.levelEnd;

			// кривая
			this->crvPassport.curveList.Add(curve);
		}

		straight.base = prev_end;
		straight.end = this->GetAbsCoordEnd();
		straight.isThere = false;
		this->crvPassport.curveList.Add(straight);
		this->crvPassport.curveList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
		std::vector<std::wstring> zl_result = this->crvPassport.BuildZL(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
		for (size_t i = 0; i < zl_result.size(); ++i)
		{
			LOG_WARNING(L"Кривые (паспорт): " + zl_result[i]);
		}
	}

	// ------------------------------------------------------------
	// Ремонты
	// ------------------------------------------------------------
	if (repairs)
	{
		sql << "select start_date, end_date, start_km, start_m, end_km, end_m, enu_repair_type_id, name ";
//		sql << "(select name from enu_repair_type where id = enu_repair_type_id) ";
		sql << "from repair_project rp inner join enu_repair_type ert on enu_repair_type_id = ert.id ";
		sql << "where adm_way_id = " << way_id << " ";
		sql << "and enu_repair_type_id in (1, 2, 3) ";
		sql << "and not exists (";
		sql << "select start_date, start_km, start_m, end_km, end_m from repair_project rp1 ";
		sql << "where rp.adm_way_id = rp1.adm_way_id and rp.start_date < rp1.start_date ";
		sql << "and rp1.enu_repair_type_id in (1, 2, 3) and not (";
		sql << "(rp.end_km < rp1.start_km) or (rp.end_km = rp1.start_km and rp.end_m < rp1.end_m) ";
		sql << "or (rp.start_km > rp1.end_km) or (rp.start_km = rp1.end_km and rp.start_m > rp1.start_m))) ";
		sql << "order by start_km, start_m;";
		try
		{	
			db.ExecuteStatement(trans, sql.str().c_str(), handler);
		}
		catch(...)
		{
			this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
			db.RollbackTransaction(trans);
			return false;
		}
		sql.str("");

		double prev_end = this->GetAbsCoordBase();
		rowCount = handler.GetRowCount();
		if (rowCount > 0)
		{
			VAprinfoRepairRecord repair, between_repairs;

			for (size_t i = 0; i < rowCount; ++i)
			{
				repair.isThere = true;
				repair.start_date = handler.GetParameter(i, 0).GetTimestamp();
				repair.end_date = handler.GetParameter(i, 1).GetTimestamp();
				repair.base = this->GetAbsCoord(VWayCoord(handler.GetParameter(i, 2).GetInteger(), handler.GetParameter(i, 3).GetFloat()));
				repair.end = this->GetAbsCoord(VWayCoord(handler.GetParameter(i, 4).GetInteger(), handler.GetParameter(i, 5).GetFloat()));
				repair.type_id = handler.GetParameter(i, 6).GetInteger();
				repair.type_descr = handler.GetParameter(i, 7).GetString();

				// участок перед ремонтом
				if (prev_end < repair.base)
				{
					between_repairs.base = prev_end;
					between_repairs.end = repair.base;
					this->repairList.Add(between_repairs);
				}

				prev_end = repair.end;

				// ремонт
				this->repairList.Add(repair);
			}

			between_repairs.base = prev_end;
			between_repairs.end = this->GetAbsCoordEnd();
			this->repairList.Add(between_repairs);
			this->repairList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
		}
	}

	// ------------------------------------------------------------
	// Тип рельсов 
	// ------------------------------------------------------------
	sql << "select ars.start_km, ars.start_m, ars.end_km, ars.end_m, ars.rails_type_id ";
	sql << "from apr_rails_sections ars inner join apr_rails_sections_period arsp ";
	sql << "on ars.apr_rails_sections_period_id = arsp.id ";
	sql << "where arsp.adm_way_id = " << way_id << " and arsp.start_date < '" << datebuf << "' ";
	sql << "and arsp.end_date > '" << datebuf << "' " << std::endl;
	try
	{	
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	for (size_t i = 0; i < rowCount; ++i)
	{
		VAprinfoRailRecord ds;
		ds.base = this->GetAbsCoord(VWayCoord(
			handler.GetParameter(i, 0).GetInteger(),
			handler.GetParameter(i, 1).GetFloat()));
		ds.end = this->GetAbsCoord(VWayCoord(
			handler.GetParameter(i, 2).GetInteger(),
			handler.GetParameter(i, 3).GetFloat()));
		ds.value = handler.GetParameter(i, 4).GetInteger();
		this->railsType.Add(ds);
	}
	this->railsType.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), RAIL_DEFAULT);

	// ------------------------------------------------------------
	// Скрепления рельсов 
	// ------------------------------------------------------------
	sql << "select arb.start_km, arb.start_m, arb.end_km, arb.end_m, arb.enu_brace_type_id ";
	sql << "from apr_rails_brace arb inner join apr_rails_brace_period arbp ";
	sql << "on arb.apr_rails_brace_period_id = arbp.id ";
	sql << "where arbp.adm_way_id = " << way_id << " and arbp.start_date < '" << datebuf << "' ";
	sql << "and arbp.end_date > '" << datebuf << "' " << std::endl;
	try
	{	
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	for (size_t i = 0; i < rowCount; ++i)
	{
		VAprinfoBraceRecord ds;
		ds.base = this->GetAbsCoord(VWayCoord(
			handler.GetParameter(i, 0).GetInteger(),
			handler.GetParameter(i, 1).GetFloat()));
		ds.end = this->GetAbsCoord(VWayCoord(
			handler.GetParameter(i, 2).GetInteger(),
			handler.GetParameter(i, 3).GetFloat()));
		ds.value = handler.GetParameter(i, 4).GetInteger();
		this->railsBrace.Add(ds);
	}
	this->railsBrace.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), BRACE_DEFAULT);

	// ------------------------------------------------------------
	// Уравнительные приборы
	// ------------------------------------------------------------
	sql << "select acj.km, acj.m from apr_cham_joint acj ";
	sql << "inner join apr_cham_joint_period acjp ";
	sql << "on acj.apr_cham_joint_period_id = acjp.id ";
	sql << "where acjp.adm_way_id = " << way_id << " and acjp.start_date < '" << datebuf << "' ";
	sql << "and acjp.end_date > '" << datebuf << "' " << std::endl;
	try
	{	
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		VAprinfoObjectRecord cj;
		cj.base = this->GetAbsCoordBase();

		for (size_t i = 0; i < rowCount; ++i)
		{
			double coord = this->GetAbsCoord(VWayCoord(
				handler.GetParameter(i, 0).GetInteger(), 
				handler.GetParameter(i, 1).GetFloat()));

			double area_begin = coord - 5.0, area_end = coord + 16.0;

			if (area_begin >= cj.base && area_end <= this->GetAbsCoordEnd())
			{
				cj.end = area_begin;
				cj.isThere = false;
				this->chamJointList.Add(cj);

				cj.base = area_begin;
				cj.end = area_end;
				cj.isThere = true;
				this->chamJointList.Add(cj);

				cj.base = area_end;
				cj.end = 0;
				cj.isThere = false;
			}
		}

		cj.end = this->GetAbsCoordEnd();
		this->chamJointList.Add(cj);
		this->chamJointList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
	}

	// ------------------------------------------------------------
	// Участки обкатки
	// ------------------------------------------------------------
	sql << "select enu_machine_type_id, start_km, start_m, end_km, end_m, ";
	sql << "start_date, end_date, speed1 from apr_testrun ";
	sql << "where adm_way_id = " << way_id << " and ";
	sql << "start_date < '" << datebuf << "' and end_date > '" << datebuf << "';";
	try
	{	
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		this->message = L"Ошибка БД: " + string_to_wstring(sql.str() + "\n\n" + db.GetErrorDescription());
		db.RollbackTransaction(trans);
		return false;
	}
	sql.str("");

	rowCount = handler.GetRowCount();
	if (rowCount > 0)
	{
		VAprinfoTestrunRecord tr_default;
		tr_default.base = this->GetAbsCoordBase();

		for (size_t i = 0; i < rowCount; ++i)
		{
			double base = this->GetAbsCoord(VWayCoord(
				handler.GetParameter(i, 1).GetInteger(), handler.GetParameter(i, 2).GetFloat()));
			double end = this->GetAbsCoord(VWayCoord(
				handler.GetParameter(i, 3).GetInteger(), handler.GetParameter(i, 4).GetFloat()));

			tr_default.end = base;
			this->testrunList.Add(tr_default);

			VAprinfoTestrunRecord tr;
			tr.isThere = true;
			tr.base = base;
			tr.end = end;
			tr.machine = (MACHINE_TYPE) handler.GetParameter(i, 0).GetInteger();
			tr.date_start = handler.GetParameter(i, 5).GetTimestamp();
			tr.date_end = handler.GetParameter(i, 6).GetTimestamp();
			tr.spd = VSpeeds(handler.GetParameter(i, 7).GetInteger(), 
				handler.GetParameter(i, 7).GetInteger(), handler.GetParameter(i, 7).GetInteger());
			this->testrunList.Add(tr);

			tr_default.base = end;
		}

		tr_default.end = this->GetAbsCoordEnd();
		this->testrunList.Add(tr_default);

		this->testrunList.SyncRanges(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
	}

	// ------------------------------------------------------------
	// Маркеры
	// ------------------------------------------------------------
    this->markers.Unload();
    this->markers.Load(db, this->GetDirCode(), this->GetWayCode(), 
                VWaySection(this->GetRealCoord(this->GetAbsCoordBase()), this->GetRealCoord(this->GetAbsCoordEnd())));

	// ------------------------------------------------------------
	// Voila!
	// ------------------------------------------------------------
	db.CommitTransaction(trans);
	this->fromDB = true;
	this->SetDefaults();
	this->SetLoaded(true);
	return true;
}

bool VAprinfo::UpdateStTrailInfo(VIBDatabase & db, const std::wstring & fileName)
{
	if (!this->IsLoaded())
	{
		return false;
	}

	if (!db.Connected()) 
	{
		this->message = ERRMSG_DB_NO_CONNECTION;
		return false;
	}

	VIBTransaction trans = db.StartTransaction();
	VIBDataHandlerParameter handler;
	std::stringstream sql;
	sql << "select stw_opmark.abscoord, stw_point_to_object.height from stw_opmark_to_point ";
	sql << "inner join stw_opmark on stw_opmark_to_point.stw_opmark_id = stw_opmark.id ";
	sql << "inner join stw_point_to_object on stw_opmark_to_point.stw_point_to_object_id = stw_point_to_object.id ";
	sql << "inner join stw_trail on stw_opmark.stw_trail_id = stw_trail.id ";
	sql << "where stw_point_to_object.height is not null ";
	sql << "and stw_trail.filename = '" << fileName << "';";
	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		db.RollbackTransaction(trans);
		this->message = L"Ошибка при получении списка меток профиля для станционного пути";
		return false;
	}

	size_t rowCount = handler.GetRowCount();
	if (rowCount < 2)
	{
		db.RollbackTransaction(trans);
		this->message = L"Число меток профиля для станционного пути меньше двух";
		return false;
	}

	this->profList.Unload();
	for (size_t i = 0; i < rowCount; ++i)
	{
		VAprinfoProfLabel pl;
		pl.coord = handler.GetParameter(i, 0).GetFloat();
		pl.value = (float) handler.GetParameter(i, 1).GetFloat();
		this->profList.Add(pl);
	}

	this->message = ERRMSG_OK;
	return true;
}

// Загрузка из БД информации для станций 
bool VAprinfo::MakeStWayInfo(VIBDatabase & db, 
							 int stCode, const std::string & parkName, const std::string & wayName,
							 const std::wstring & fileName)
{
	return this->MakeStWayInfo(db, stCode, parkName, wayName, 
		std::vector<VAprinfoKmRecord>(), fileName);
}

static float StaticGetKmLength(const std::vector<VAprinfoKmRecord> & nstKm, long km)
{
	for (size_t i = 0; i < nstKm.size(); ++i)
		if (nstKm[i].km == km)	return nstKm[i].len;

	return 1000.0f;
}

bool operator < (const VAprinfoKmRecord & km1, const VAprinfoKmRecord & km2)
{
	return km1.base < km2.base;
}

// Загрузка из БД информации для станций - координаты ст.пути
bool VAprinfo::MakeStWayInfo(VIBDatabase & db, int stCode, 
							 const std::string & parkName, const std::string & wayName,
							 const std::vector<VAprinfoKmRecord> & nstKm,
							 const std::wstring & fileName)
{
	if (this->IsLoaded())
	{
		this->Unload();
	}

	if (!db.Connected()) 
	{
		this->message = ERRMSG_DB_NO_CONNECTION;
		return false;
	}

	VIBTransaction trans = db.StartTransaction();
	VIBDataHandlerParameter handler;
	std::stringstream sql;

	/// границы поездки
	sql << "select start_abs, end_abs from stw_trail where filename = '" << fileName << "';";
	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		std::string addErrorMsg = db.GetErrorDescription();
		db.RollbackTransaction(trans);
		this->message = L"Ошибка при получении границ поездки";
		return false;
	}
	if (handler.GetRowCount() == 0)
	{
		db.RollbackTransaction(trans);
		this->message = L"Отсутствует информация о поездке";
		return false;
	}
	if (handler.GetRowCount() > 1)
	{
		db.RollbackTransaction(trans);
		this->message = L"Не уникальная запись о поездке";
		return false;
	}
	double trail_begin = handler.GetParameter(0, 0).GetFloat();
	double trail_end = handler.GetParameter(0, 1).GetFloat();

	double db_way_begin = 0, db_way_end = 0;
	bool is_db_way_begin = false, is_db_way_end = false;

	/// начало пути по базе
	sql.str("");
	sql << "select stw_opmark.abscoord from stw_opmark " << std::endl;
	sql << "inner join stw_opmark_to_point on stw_opmark_to_point.stw_opmark_id = stw_opmark.id " << std::endl;
	sql << "inner join stw_point_to_object on stw_opmark_to_point.stw_point_to_object_id = stw_point_to_object.id " << std::endl;
	sql << "inner join stw_function_to_point on stw_function_to_point.stw_point_to_object_id = stw_point_to_object.id " << std::endl;
	sql << "where stw_point_to_object.stw_way_id = (select id from stw_way " << std::endl;
	sql << "where code = '" << wayName << "' and stw_park_id = ";
	sql << "(select id from stw_park where name = '" << parkName << "' " << std::endl;
	sql << "and adm_station_id = (select id from adm_station where code = '" << stCode << "'))) " << std::endl;
	sql << "and stw_function_to_point.enu_point_type_id = (select id from enu_point_type where name = 'Начало пути') ";
	sql << "and stw_opmark.stw_trail_id = (select id from stw_trail where filename = '" << fileName << "');";
	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		std::string addErrorMsg = db.GetErrorDescription();
		db.RollbackTransaction(trans);
		this->message = L"Ошибка при получении точки начала пути";
		return false;
	}
	if (handler.GetRowCount() > 1)
	{
		db.RollbackTransaction(trans);
		this->message = L"Не уникальная точка начала пути";
		return false;
	}
	if (handler.GetRowCount() == 1)
	{
		db_way_begin = handler.GetParameter(0, 0).GetFloat();
		is_db_way_begin = true;

		/// Конец пути по базе
		sql.str("");
		sql << "select stw_opmark.abscoord from stw_opmark " << std::endl;
		sql << "inner join stw_opmark_to_point on stw_opmark_to_point.stw_opmark_id = stw_opmark.id " << std::endl;
		sql << "inner join stw_point_to_object on stw_opmark_to_point.stw_point_to_object_id = stw_point_to_object.id " << std::endl;
		sql << "inner join stw_function_to_point on stw_function_to_point.stw_point_to_object_id = stw_point_to_object.id " << std::endl;
		sql << "where stw_point_to_object.stw_way_id = (select id from stw_way " << std::endl;
		sql << "where code = '" << wayName << "' and stw_park_id = ";
		sql << "(select id from stw_park where name = '" << parkName << "' " << std::endl;
		sql << "and adm_station_id = (select id from adm_station where code = '" << stCode << "'))) " << std::endl;
		sql << "and stw_function_to_point.enu_point_type_id = (select id from enu_point_type where name = 'Конец пути') ";
		sql << "and stw_opmark.stw_trail_id = (select id from stw_trail where filename = '" << fileName << "');";
		try
		{
			db.ExecuteStatement(trans, sql.str().c_str(), handler);
		}
		catch(...)
		{
			db.RollbackTransaction(trans);
			this->message = L"Ошибка при получении точки конца пути";
			return false;
		}
		if (handler.GetRowCount() > 1)
		{
			db.RollbackTransaction(trans);
			this->message = L"Не уникальная точка конца пути";
			return false;
		}
		if (handler.GetRowCount() == 1)
		{
			db_way_end = handler.GetParameter(0, 0).GetFloat();
			is_db_way_end = true;
		}
	}

	/// реальные границы пути
	if (is_db_way_begin && is_db_way_end)
	{
		this->absCoordBase = db_way_begin;
		this->absCoordEnd = db_way_end;
	}
	else
	{
		this->absCoordBase = trail_begin;
		this->absCoordEnd = trail_end;
	}

	// ориентация пути
	int dir = 1;
	if (this->absCoordBase > this->absCoordEnd)	
	{
		dir = -1;
		std::swap(this->absCoordBase, this->absCoordEnd);
	}

	/// привязка пикетажа
	sql.str("");
	sql << "select stw_opmark.abscoord, stw_point_to_object.km, stw_point_to_object.m  from stw_opmark " << std::endl;
	sql << "inner join stw_opmark_to_point on stw_opmark_to_point.stw_opmark_id = stw_opmark.id " << std::endl;
	sql << "inner join stw_point_to_object on stw_opmark_to_point.stw_point_to_object_id = stw_point_to_object.id " << std::endl;
	sql << "inner join stw_function_to_point on stw_function_to_point.stw_point_to_object_id = stw_point_to_object.id " << std::endl;
	sql << "where stw_point_to_object.stw_way_id = (select id from stw_way " << std::endl;
	sql << "where code = '" << wayName << "' and stw_park_id = ";
	sql << "(select id from stw_park where name = '" << parkName << "' " << std::endl;
	sql << "and adm_station_id = (select id from adm_station where code = '" << stCode << "'))) " << std::endl;
	sql << "and stw_function_to_point.enu_point_type_id = (select id from enu_point_type ";
	sql << "where name = 'Точка привязки пикетажа') and ";
	sql << "stw_point_to_object.km is not null and stw_point_to_object.m is not null ";
	sql << "and stw_opmark.stw_trail_id = (select id from stw_trail where filename = '" << fileName << "');";
	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch(...)
	{
		db.RollbackTransaction(trans);
		this->message = L"Ошибка при получении точки привязки пикетажа";
		return false;
	}
	if (handler.GetRowCount() > 1)
	{
		db.RollbackTransaction(trans);
		this->message = L"Не уникальная точка привязки пикетажа";
		return false;
	}

	double absAxis;
	VWayCoord wayAxis;
	if (handler.GetRowCount() == 0)
	{
		absAxis = this->absCoordBase;
		wayAxis = VWayCoord(0, 0);
	}
	else
	{
		absAxis = handler.GetParameter(0, 0).GetFloat();
		wayAxis = VWayCoord(
			(long) handler.GetParameter(0, 1).GetInteger(), 
			(float) handler.GetParameter(0, 2).GetFloat());
	}

	db.RollbackTransaction(trans);	

	// ось за пределами границ?
	if (absAxis < this->absCoordBase)	this->absCoordBase = absAxis;
	if (absAxis > this->absCoordEnd)	this->absCoordEnd = absAxis;

	// начало (begin) километра указанного в оси
	VAprinfoKmRecord axisKm; 
	axisKm.km = wayAxis.km;
	axisKm.len = StaticGetKmLength(nstKm, axisKm.km) * dir;
	if (axisKm.km < 0) axisKm.len *= -1;
	if (axisKm.len > 0)	axisKm.base = absAxis - wayAxis.m;
	else				axisKm.base = absAxis - abs(axisKm.len) - wayAxis.m;
	axisKm.end = axisKm.base + fabs(axisKm.len);

	// ищем начало пути
	double abs = axisKm.base;
	long km = axisKm.km - dir;
	while (abs > this->absCoordBase)
	{
		VAprinfoKmRecord curKm;
		curKm.km = km;
		curKm.len = StaticGetKmLength(nstKm, km) * dir;
		if (curKm.km < 0) curKm.len *= -1;
		curKm.end = abs;
		abs -= fabs(curKm.len);
		curKm.base = abs;
		km -= dir;
		this->kmList.Add(curKm);
	}

	// ищем конец пути
	km = axisKm.km + dir;
	abs = axisKm.end;
	while (abs < this->absCoordEnd)
	{
		VAprinfoKmRecord curKm;
		curKm.km = km;
		curKm.len = StaticGetKmLength(nstKm, km) * dir;
		if (curKm.km < 0) curKm.len *= -1;
		curKm.base = abs;
		abs += fabs(curKm.len);
		curKm.end = abs;
		km += dir;
		this->kmList.Add(curKm);
	}

	this->kmList.Add(axisKm);
	this->kmList.Sort();

	// скорости
	VAprinfoSpeedRecord spd;
	spd.base = this->absCoordBase;
	spd.end = this->absCoordEnd;
	spd.spd = VSpeeds(40, 40, 40);
	this->speedList.Add(spd);

	// всё остальное
	this->SetDefaults();

	this->isLoaded = true;
	this->message = ERRMSG_OK;
	this->fromDB = true;

	// профиль
	this->UpdateStTrailInfo(db, fileName);
	return true;
}

