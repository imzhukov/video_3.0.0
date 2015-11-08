#pragma once

#include "types.h"
#include "dblib.h"

/** Возвращает эксплуатационную координату*/
VWayCoord GetRealCoord(VIBDatabasePtr in_db, const char * in_way_code, const char * in_direction_code, double in_abscoord, const VTime& in_date);
/** Возвращает абсолютную координату*/
VWayCoord GetRealCoord(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::wstring& in_way_id, double in_abscoord);

/** Возвращает абсолютную координату*/
double GetAbsCoord(VIBDatabasePtr in_db, const char * in_way_code, const char * in_direction_code, const VWayCoord& in_waycoord, const VTime& in_date);

/** Возвращает абсолютную координату*/
double GetAbsCoord(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::wstring& in_way_id, const VWayCoord& in_wc);

/** Добавляет длину к эксплуатационной координате*/
VWayCoord IncRealCoord(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::wstring& in_way_id, const VWayCoord& in_wc, double in_length);

/** Возвращает сообщение об ошибке*/
std::wstring GetErrorDescription(VIBDatabasePtr in_db, VIBTransaction & in_transaction);

/** Возвращает сообщение об ошибке*/
std::wstring GetConstraintMessage(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::wstring& in_constraint);

/** Возращает сообщения об ошибках*/
std::vector<std::wstring> GetErrorLog(VIBDatabasePtr in_db, VIBTransaction & in_transaction);

/** Очищает журнал ошибок*/
void ClearErrorLog(VIBDatabasePtr in_db, VIBTransaction & in_transaction);

/** Очищает таблицу проверок*/
void ClearChanges(VIBDatabasePtr in_db, VIBTransaction & in_transaction);

/** Возвращает ID записи*/
std::wstring GetRowID(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::string& in_query);

/** Возвращает результат запроса одного поля как вектор строк*/
void GetSingleField(VIBDatabasePtr in_db, VIBTransaction & in_transaction, const std::string& in_query, std::vector<std::wstring>& out_values);
