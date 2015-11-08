#pragma once

#include "VDataSet.h"

enum DATA_EXPORT
{
	DE_OVERSTEPS, DE_CURVES, DE_MODEL_CURVES,
	DE_STATISTICS, DE_MARKS, DE_ZAZOR, DE_UKLON, DE_PEREKOS, 
	DE_PROFILE, DE_JAGGY, DE_DIFF_WIDTH, DE_PPR, DE_MARKERS, 
	DE_PLAN_SPD, DE_AUTO
};

/// Проверка наличия поездки в базе
bool CheckTrailInBase(VIBDatabasePtr db, const VFileName & file);

/// Проверка необходимости обновления данных в базе
std::set<DATA_EXPORT> CheckDataUpdates(VIBDatabasePtr db, const VFileName & file);

/// Удаление из базы информации о поездке
std::wstring DeleteTrail(VIBDatabasePtr db, const VFileName & file);

/// Удаление из базы информации о поездках
std::wstring DeleteTrails(VIBDatabasePtr db, const std::vector<VFileName> & files);

/// Удаление станционных кривых из базы
std::wstring DeleteStCurves(VIBDatabasePtr db, const VFileName & file);

/// Занесение данных в базу 
std::wstring DataMultiExportDB(VIBDatabasePtr db, 
							   const std::vector<VFileName> & files, 
							   bool stationFile, bool rewrite, 
							   const std::set<DATA_EXPORT> & exports,
							   std::vector<VFileName> & exported);

/// Частичное удаление данных из базы
std::wstring PartialDataDelete(VIBDatabasePtr db,
                               const std::vector<VFileName> & files,
                               const std::set<DATA_EXPORT> & deletion);

/// Список проездов, по которым оценен ремонт
std::vector<std::wstring> GetRepairTrails(VIBDatabasePtr db, int repair_id);

/// Оценка ремонтов
std::wstring EstimateRepair(VIBDatabasePtr db, int repair_id,
							const VFileName & before, const VFileName & after, 
							const VFileName & after_year);

/// СССП через год после ремонта
std::wstring RepairSSSPAfterYear(VIBDatabasePtr db, int repair_id, const VFileName & after);

/// Удаление оценки ремонта
std::wstring DeleteRepair(VIBDatabasePtr db, int repair_id);

/// Обновление информации о поездке на участке план.скорости
std::wstring PlanSpeedExport(VIBDatabasePtr db, const VFileName & file, VProgressBox & prog);

/// Занесение в БД файла станционных путей
std::wstring ExportSingleStationFileDB(VIBDatabasePtr db, const VFileName & fileName, VProgressBox & prog);

/// Занесение в БД нескольких файлов станционных путей
std::wstring ExportMultiStationFileDB(VIBDatabasePtr db, const std::vector<VFileName> & files);

/// Удаление файла ст. путей из базы
std::wstring DeleteStTrail(VIBDatabasePtr db, const std::wstring & sh_name);

/// Расчет профиля на файле
std::wstring CalcStationProfile(VIBDatabasePtr db, const VFileName & fn);

/// Удаление расчета профиля на файле
std::wstring DeleteStationProfile(VIBDatabasePtr db, const std::wstring & sh_name);

/// Число точек относящихся к файлу
int64_t CalcProfPointCount(VIBDatabasePtr db, const std::wstring & sh_name, bool apr);