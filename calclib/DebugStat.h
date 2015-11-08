#pragma once

#include "VDataSet.h"

class VIBDatabase;

/// ¬ыбор ведомости
enum STATDEBUG_REPORT
{
	STATREP_KM,	/// покилометрова€ статистика
	STATREP_PK	/// попикетна€ статистика
};

/// ќтладочна€ ведомость кривых
std::wstring StatExportHTML(STATDEBUG_REPORT report,
							const VFileName & fileName, 
							const VFileName & reportFile, 
							VIBDatabasePtr dataBase);


