#pragma once

#include "VDataSet.h"

class VIBDatabase;

/// Отладочная ведомость отступлений
std::wstring OvrHTML(const VFileName & fileName, 
					  const VFileName & reportFile);

/// ПУ-32
std::wstring PU32HTML(const VFileName & fileName, 
					  const VFileName & reportFile);


