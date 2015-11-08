#pragma once

#include "VDataSet.h"

class VIBDatabase;

/// Выбор ведомости
enum CRVDEBUG_REPORT
{
	CRVREP_CURVES,	// кривые
	CRVREP_STCURVES,// кривые на станционных путях
	CRVREP_PK,		// пикеты (с износом)
	CRVREP_LINKS,	// звенья (с износом)
	CRVREP_FPO,		// участки ФПО (1 скорость / 1 км)
	CRVREP_WEARDGR, // диаграмма износов
	CRVREP_MODEL	// модели кривых
};

/// Отладочная ведомость кривых
std::wstring CurveExportHTML(CRVDEBUG_REPORT report,
							 const VFileName & fileName, 
							 const VFileName & reportFile, 
							 VIBDatabasePtr dataBase);

DECLARE_OPTION(double, EXPERIMENTAL_ANP, 0, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
