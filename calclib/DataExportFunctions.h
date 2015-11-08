#pragma once

#include "revision.h"
#include "VProgressInterface.h"
#include "VDataSet.h"
#include "VFileName.h"
#include "dblib.h"
#include "dbfunc.h"

/// Операция прервана
extern const std::wstring OPERATION_CANCELED;

/// Обновление информации о поездке
std::wstring UpdateTrail(VIBDatabasePtr db, VIBTransaction & trans, VDataSetPtr ds);

/// ID поездки
__int64 GetTrailID(VIBDatabasePtr db, VIBTransaction & trans, const VFileName & file);

/// Занесение кривых
std::wstring AddCurves(VIBDatabasePtr db, VIBTransaction & trans, 
					   VDataSetPtr ds, __int64 id,
					   VProgressBox * prog, CURVE_TYPE crvType, bool optics);

/// Занесение модельных кривых
/*std::wstring AddModelCurves(VIBDatabasePtr db, VIBTransaction & trans,
							VDataSetPtr ds, __int64 id, VProgressBox * prog);*/

/// Занесение статистики
std::wstring AddStatistics(VIBDatabasePtr db, VIBTransaction & trans,
						   VDataSetPtr ds, __int64 id, VProgressBox * prog);

/// Занесение отступлений
std::wstring AddOvrs(VIBDatabasePtr db, VIBTransaction & trans,
					  VDataSetPtr ds, __int64 id, VProgressBox * prog);

/// Занесение меток оператора, меток шпал итп
std::wstring AddMarks(VIBDatabasePtr db, VIBTransaction & trans,
					  VDataSetPtr ds, __int64 id, VProgressBox * prog);

/// Занесение зазоров
std::wstring AddZaz(VIBDatabasePtr db, VIBTransaction & trans,
					VDataSetPtr ds, __int64 id, VProgressBox * prog);

// Занесение макс. изменений уклона
std::wstring AddUklon(VIBDatabasePtr db, VIBTransaction & trans,
					  VDataSetPtr ds, __int64 id, VProgressBox * prog);

// Занесение перекосов на базе кузова и тележки
std::wstring AddPerekos(VIBDatabasePtr db, VIBTransaction & trans,
					   VDataSetPtr ds, __int64 id, VProgressBox * prog);

// Занесение макс. ускорений
std::wstring AddUskor(VIBDatabasePtr db, VIBTransaction & trans, VDataSetPtr ds,
					  __int64 id, VProgressBox * prog);

// Занесение макс. возвышений в прямых
std::wstring AddPerLine(VIBDatabasePtr db, VIBTransaction & trans, 
						VDataSetPtr ds, __int64 id, VProgressBox * prog);

// Занесение разметки профиля
std::wstring AddProfile(VIBDatabasePtr db, VIBTransaction & trans, 
						VDataSetPtr ds, __int64 id, VProgressBox * prog);

// Сверхнормативные неровности
std::wstring AddJaggy(VIBDatabasePtr db, VIBTransaction & trans,
					  VDataSetPtr ds, __int64 id, VProgressBox * prog);

// Отжатия рельсов
std::wstring AddDiffWidth(VIBDatabasePtr db, VIBTransaction & trans,
						  VDataSetPtr ds, __int64 id, VProgressBox * prog);

// Оценка ППР
std::wstring AddPPR(VIBDatabasePtr db, VIBTransaction & trans,
					VDataSetPtr ds, __int64 id, VProgressBox * prog);

// Маркеры
std::wstring AddMarkers(VIBDatabasePtr db, VIBTransaction & trans,
					   VDataSetPtr ds, __int64 id, VProgressBox * prog);

// Оценка проектных скоростей
std::wstring AddPlanSpd(VIBDatabasePtr db, VIBTransaction & trans,
						VDataSetPtr ds, __int64 id, VProgressBox * prog);

