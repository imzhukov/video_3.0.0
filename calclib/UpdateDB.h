#pragma once

#include "dbfunc.h"

// Первая, базовая известная версия
const int basicVersion = 200;
// Версия, до которой будем обновляться
const int destVersion = 670;

class wxFileName;

/// Версия БД
int GetDBVersion(VIBDatabase & db3);

/// Локальная папка измерений

std::wstring GetDBLocalFolder(VIBDatabase & db3);

/// Описание БД	
std::wstring GetDBInfo(VIBDatabase & db3);

/// Проверка и обновление структуры БД
bool UpdateDB(VIBDatabase & db3, bool check_only=false);

/// Проверка на последнее резервное копирование
bool CheckBackupDB(VIBDatabase & db3, int allow_days_without_backup);

/// Вернуть кол-во дней с последнего backup
int CountDaysLastBackup(VIBDatabase & db3);