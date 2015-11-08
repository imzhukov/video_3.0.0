#pragma once

#include "datatypes.h"
#include "VFileName.h"

// ---------------------------------------------------------------------------------
/// ����������� ���������
// ---------------------------------------------------------------------------------

enum PARAMETER_TYPE
{
	PRM_NONE, // ��� ��������� (0)
	PRM_APRINFO, // ��������� ���������� �� ����� 
	PRM_ABS_COORD, // ���������� ���������� 
	PRM_WAY_COORD, // ������� ����������� (��������) 
	PRM_CORRUPT, // ���� ��������� 
	PRM_HAND_CORRUPT, // ������ ���������� ��������� 
	PRM_SPEED, // �������� ������ 
	PRM_COMMENT, // �����������
	PRM_DPP, // ������� ����� ���
	PRM_KADR_COUNT, // ������� ����� ������ (��������)
	PRM_OVRS2, // ����������� - ����������� ������ 2
	PRM_OVRS3, // ����������� - ����������� ������ 3
	PRM_OP_MARKS, // ����� ���������
	PRM_MAG_ZAZOR, // ������ ��������� ������� - ����� ��� ���������
	PRM_UGON_LEFT, // ���� ������, �����
	PRM_UGON_RIGHT, // ���� ������, ������
	PRM_SPALA_MARKER, // RF ����� ����
	PRM_LINE_MARKS, // �������� ������
	PRM_SIDEWAYS, // ������� ����
	PRM_HISTORY, // ������� ��������
	PRM_CURVE_MARKS, // �������� ������
	PRM_MODEL_CURVES, // �������� ������� ������
	PRM_PROF_MARKS, // �������� �������
	PRM_CONTROL_AREA, // ����������� �������
	PRM_KURS, // �������� ����
	PRM_KURS_BACKUP, // ��������� ����� �����
	PRM_GRADIENT, // �����
	PRM_LEVEL, // �������
	PRM_CURV, // ��������
	PRM_WIDTH, // ������ �����
	PRM_WIDTH_FRONT, // ������ ����� ������� ������
	PRM_WIDTH_BACK, // ������ ����� ����� ������
	PRM_PIT_LEFT, // �������� ������
	PRM_PIT_RIGHT, // �������� �����
	PRM_DIST_SHORT, // ������� �� ���� �������
	PRM_DIST_LONG, // ������� �� ���� ������
	PRM_RICH_LEFT, // �������� ����� 
	PRM_RICH_RIGHT, //  �������� ������
	PRM_TEMP_LEFT, // ����������� �����
	PRM_TEMP_RIGHT,  // ����������� ������
	PRM_ZAZOR_LEFT, // ����� �����
	PRM_ZAZOR_RIGHT, // ����� ������
	PRM_MAG_ZAZOR_LEFT, // ��������� ����� ����� - ���������
	PRM_MAG_ZAZOR_RIGHT, // ��������� ����� ������ - ���������
	PRM_WEAR_LEFT, // ����� ����� 
	PRM_WEAR_RIGHT, // ����� ������
	PRM_UNI_WEAR_SENSOR, // ������������ ����� �� ����-1
	PRM_USK_VERT, // ��������� ������������
	PRM_USK_HOR, // ��������� ��������������
	PRM_LATITUDE, // ������ �����������
	PRM_LONGITUDE, // ������� �����������
	PRM_SHORT_GRADIENT, // �������� �����
	PRM_KN_LEFT, // �������� ����������
	PRM_KN_RIGHT, // �������� ����������
	PRM_GPS_TIME, // ����� ������� GPS
	PRM_GPS_LATITUDE, // ������
	PRM_GPS_LONGITUDE,  // �������
	PRM_GPS_ALTITUDE, // ����������
	PRM_TEST_4, 
	PRM_TEST_7,
	PRM_TEST_8, 
	PRM_PPR_WAYPASSED, // ������� ����������� ���� 
	PRM_PPR_WIDTH, // �� 
	PRM_PPR_NAK_LEFT, // ������ �����
	PRM_PPR_NAK_RIGHT, // ������ ������
	PRM_PPR_POD_LEFT, // ���������� �����
	PRM_PPR_POD_RIGHT, // ���������� ������
	PRM_PPR_IZN_LEFT, // ����� ������������ �����
	PRM_PPR_IZN_RIGHT, // ����� ������������ ������
	PRM_PPR_IZN_EXT_LEFT, // ����� ������� �����
	PRM_PPR_IZN_EXT_RIGHT, // ����� ������� ������
	PRM_PPR_IZN_INT_LEFT, // ����� ���������� �����
	PRM_PPR_IZN_INT_RIGHT, // ����� ���������� ������
	PRM_MID_LEVEL, // ������� �������
	PRM_MID_CURV, // ������� ��������
	PRM_MID_WIDTH, // ������� ������ �����
	PRM_MID_RICH_LEFT, // ������� �������� �����
	PRM_MID_RICH_RIGHT, // ������� �������� ������
	PRM_PROFILE, // �������
	PRM_UNI_WEAR,  // ������������ �����
	PRM_STATISTICS, // ���������
	PRM_PLAN, // ����
	PRM_IDEAL_PLAN, // ��������� ����
	PRM_PLAN_ELEM_LIST, // ������ ��������� �����
	PRM_DIFF_PROFILE, // ���������� �� ���������� �������
	PRM_MODEL_IDEAL_PLAN, // ��������� ���� �� ��������� ��������
	PRM_PLAN_OFFSET,	// ���������� �� ���������� �����
	PRM_PLAN_OFFSET_FICTIONAL, // ���������� �� ���������� ����� ��� ����� �������� ������
	PRM_MODEL_PLAN_ELEM_LIST, // ������ ��������� ����� �� ��������� ��������
	PRM_ANP_REAL_PASS, 
	PRM_ANP_REAL_TRUCK,
	PRM_ANP_MODEL_PSP_PASS,
	PRM_ANP_MODEL_PSP_TRUCK,
	PRM_JAGGY_PROFILE, // ���������� �������
	PRM_LEVEL_ZLINE,   // ������� ����� �� ������
	PRM_PROFILE_SLOPE_ELEMENT, // ������������ ����� �������
	PRM_PROFILE_SLOPE_KM,  // ����� ������� �� ���������
	PRM_PROFILE_SLOPE_PK, // ����� ������� �� ������
	PRM_PROFILE_SLOPE_HPK, // ����� ������� �� �������� ������
	PRM_PSI_REAL_PASS, //�������� ���������� ������������� ��������� (����. ����.)
	PRM_PSI_REAL_TRUCK, //�������� ���������� ������������� ��������� (����. ����.)
	PRM_MODEL_PLAN_OFFSET,	// ���������� �� ���������� ����� (����.)
	PRM_LINES, // ������ (�� ������ � ������� �����)
	PRM_JAGGY_PLAN, // ���������� � �����	
	PRM_APROX_PROFILE, //��������� �������
	PRM_CURV_FROM_KURS, //��������, ����������� �� ��������� ����	
	PRM_DIFF_LEVEL, // ���������� ������ �� ��������
	PRM_DIFF_LEVEL_MODEL, // ���������� ������ �� �������� ������
	PRM_JAGGY_PROFILE_OVERSTEP, // ���������������� ���������� �������
	PRM_JAGGY_PLAN_OVERSTEP, // ���������������� ���������� �����
	PRM_DIFF_WIDTH_FRONT, // �������� �� � ��������� � ���
	PRM_DIFF_WIDTH_FRONT_OVERSTEP, //�������
	PRM_DIFF_WIDTH_BACK, // �������� �� � ��������� � ���
	PRM_DIFF_WIDTH_BACK_OVERSTEP, //�������
	PRM_CANTING_LEFT_OVERSTEP, //���������� �����
	PRM_CANTING_RIGHT_OVERSTEP, //���������� ������
	PRM_PR_NGR_AREAS, // �-�� ��-��� �� �������� ����
	PRM_ANP_REAL_HIGH_BT, // 
	PRM_ANP_REAL_HIGH, //
	PRM_PSI_REAL_HIGH_BT, //�������� ���������� ������������� ��������� (����. ����. � ��)
	PRM_PSI_REAL_HIGH, //�������� ���������� ������������� ��������� (����. ����.)
	PRM_ANP_MODEL_PSP_HIGH, //��� ��������� ��� ���������� �������
	PRM_ANP_MODEL_PSP_HIGH_BT, //��� ��������� ��� ���������� ������� � �������� ������
	PRM_WAY_MARKERS, // ����� ����
	PRM_JAGGY_PLAN_ON_LINES, // ���������� � ����� �� ������
	PRM_EXT_WEAR_LEFT_OVERSTEP, //���������� �����
	PRM_EXT_WEAR_RIGHT_OVERSTEP, //���������� ������
	PRM_EVERYTHING	// ��� ���������
};

/// ����������� ������������� ���� ����������
void VerifyParamNames();

// �������� ���������
const wchar_t * GetParamName(PARAMETER_TYPE param); /// ���������� ��� ���������
const wchar_t * GetUserParamName(PARAMETER_TYPE param); /// Human-readable ��� ���������
const wchar_t * GetParamMeasurements(PARAMETER_TYPE param); /// Human-readable ��� ���������
unsigned long GetFileCodeByParam(PARAMETER_TYPE param); /// ��� ��������� � �����

// ����� ���������
PARAMETER_TYPE GetParamCode(const wchar_t * param); /// ��� ��������� �� ����������� �����
PARAMETER_TYPE GetParamByUserName(const wchar_t * param); /// ��� ��������� �� human-readable �����
PARAMETER_TYPE GetParamByFileCode(unsigned long fc); /// ��� ��������� �� ���� ��������� � �����

/// ����� �� ������� ������ ��������� ����������
bool DumpParamList(const VFileName & path);

