#pragma once

#include "VPrNgrAreas.h"
#include "dblib.h"

// ������� � HTML
std::wstring PrNgrExportHTML(const VFileName & fileName, 
							 const VFileName & reportFile, 
							 VIBDatabasePtr dataBase);