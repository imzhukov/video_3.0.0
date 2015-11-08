#pragma once

// NB: ����� VBasicParameter ����� �������� ��� ������ � �������.
// ����� ������������ VTemplateParameter<T> - ��������� �����, �������������� ����
// � ���� ������� ��������� ���� �, ���� VParameter - ��� ������ � ��������� ���������
//
// VAbsCoordData � VCorruptData - ���������� �����. VTemplateParameter<double> �
// VTemplateParameter<uint32_t>.

// ����������������� ����� ����������
#include "VDataSet.h"

// ��������� ���������
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