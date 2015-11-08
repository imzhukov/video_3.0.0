#pragma once

// NB: класс VBasicParameter слабо пригоден для работы с данными.
// лучше использовать VTemplateParameter<T> - шаблонный класс, представляющий блок
// в виде массива элементов типа Т, либо VParameter - для работы с массивами измерений
//
// VAbsCoordData и VCorruptData - наследники соотв. VTemplateParameter<double> и
// VTemplateParameter<uint32_t>.

// Консолидированный набор параметров
#include "VDataSet.h"

// отдельные параметры
#include "VParameter.h"
#include "VAprinfo.h"
#include "VCurve.h"
#include "VCurveList.h"
#include "VProfile.h"
#include "VOverstep.h"
#include "VOpMark.h"
#include "VSidewayList.h"
#include "VControlArea.h"
// ------------------------------------
#include "VStatistics.h"
#include "VLineList.h"
// ------------------------------------
#include "DataExport.h"
#include "DebugCurves.h"
#include "DebugStat.h"
#include "DebugOvrs.h"
#include "DebugPrNgr.h"
#include "DebugAprinfo.h"
// ------------------------------------
#include "stationConv.h"