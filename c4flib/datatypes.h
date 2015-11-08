#pragma once

/*
���� datatypes.h
���������� ���� ������ � ������ c4f
*/

#include "os.h"
#include "stdint.h"

/** ����� ������������� ����� ���������

/** ���������� �������*/
#define	PHO_REC_COUNT			0x0001		
/** ������ ������*/
#define	PHO_REC_SIZE			0x0002		
/** ��������*/
#define	PHO_DIVIDER				0x0004		
/** ���*/
#define	PHO_C_TYPE				0x0008		
/** ��������*/
#define	PHO_OFFSET				0x0010		
/** ������ ���������*/
#define	PHO_REC_VER				0x0020		
/** ���� �������� �����*/
#define	PHO_CREATE_TIME			0x0040		
/** ���� ���������� �����*/
#define	PHO_UPDATE_TIME			0x0080		
/** ����� ������ ��� �����*/
#define	PHO_BLOCK_FLAGS			0x0100
/** ������ ����� ������� ����*/
#define	PHO_INDEX				0x0200

/** ����������� ����� ������*/
#define PHO_DEFAULT PHO_REC_COUNT | PHO_REC_SIZE | PHO_DIVIDER | PHO_C_TYPE | PHO_REC_VER | PHO_CREATE_TIME | PHO_UPDATE_TIME | PHO_INDEX


/* ���� C*/

/** ����������� ���*/
#define VCT_UNKNOWN		-1
/** float*/
#define	VCT_FLOAT		0
/** double*/
#define	VCT_DOUBLE		1
/** int*/
#define	VCT_INT			2
/** unsigned int*/
#define	VCT_UINT		3
/** long*/
#define	VCT_LONG		4
/** unsigned long*/
#define	VCT_ULONG		5
/** char*/
#define	VCT_CHAR		6
/** unsigned char*/
#define	VCT_UCHAR		7
/** short*/
#define	VCT_SHORT		8
/** unsigned short*/
#define	VCT_USHORT		9
/** user*/
#define	VCT_USER		10

/** ���������� �������� C ���� �� ��� ����*/
const wchar_t * GetCTypeName(int code);

/** ������ ��������*/
#define	DHT_OFFSET				0L
/** ��������� ����������*/
#define	DHT_APRINFO				10L
/** ���������� ����������*/
#define	DHT_ABS_COORD			11L
/** ������� ����������*/
#define	DHT_WAY_COORD			12L
/** �������� ���� ��������*/
#define	DHT_CORRUPT				13L
/** ������*/
#define	DHT_CURVE_MARKS			15L
/** �����������*/
#define	DHT_OVRS2			    16L
/** ����� �������*/
#define	DHT_PROF_MARKS			17L
/** ������� ������� �������*/
//#define	DHT_PR_MARKS		18L
/** ����� ����*/
#define	DHT_UGON_LEFT			19L
/** ������ ����*/
#define	DHT_UGON_RIGHT			20L
/** ����� ���������*/
#define	DHT_OP_MARKS			21L
/** ������*/
#define DHT_LINE_MARKS			22L
/** ����� ������*/
//#define DHT_LINE_MARKS		23L
/** �������� ������*/
#define	DHT_SPEED				100L
/** ����*/
#define	DHT_KURS				101L
/** �����*/
#define	DHT_GRADIENT			102L
/** �p�����*/
#define	DHT_LEVEL				103L
/** ��p���*/
#define	DHT_WIDTH				104L
/** �p������ �p����*/
#define	DHT_PIT_RIGHT			105L
/** �p������ �����*/
#define	DHT_PIT_LEFT			106L
/** �p������*/
#define	DHT_CURV				107L
/** ��p���� ��������*/
#define	DHT_DIST_SHORT			108L
/** ��p���� �������*/
#define	DHT_DIST_LONG			109L
/** �������� ������*/
#define	DHT_RICH_RIGHT			110L
/** �������� �����*/
#define	DHT_RICH_LEFT			111L
/** �����p���p� ������ p�����*/
#define	DHT_TEMP_LEFT			112L
/** �������� ����p ������ p�����*/
#define	DHT_ZAZOR_LEFT			113L
/** ����� ������ p�����*/
#define	DHT_WEAR_LEFT			114L
/** ��p����� ��p������� �� ����� p�����*/
#define	DHT_KN_LEFT				115L
/** �����p���p� ������� p�����*/
#define	DHT_TEMP_RIGHT			116L
/** �������� ����p ������� p�����*/
#define	DHT_ZAZOR_RIGHT			117L
/** ����� ������� p�����*/
#define	DHT_WEAR_RIGHT			118L
/** ��p����� ��p������� �� �p���� p�����*/
#define	DHT_KN_RIGHT			119L
/** ������� �������*/
#define	DHT_MID_LEVEL			120L
/** ��. ��������*/
#define	DHT_MID_CURV			121L
/** ��. p������� ������*/
#define	DHT_MID_RICH_RIGHT		122L
/** ��. p������� �����*/
#define	DHT_MID_RICH_LEFT		123L
/** �������� �����*/
#define	DHT_SHGRADIENT			127L
/** �������������� ���������*/
#define	DHT_USK_HOR				128L
/** ������������ ���������*/
#define	DHT_USK_VERT			129L
/** ��������� �����*/
#define DHT_MAG_ZAZOR			130L
/** ������� ������� �� ������� ����*/
#define DHT_SIDEWAYS            131L
/** ������� �������� ������������� ��� ������*/
#define DHT_HISTORY             132L
/** �����������*/
#define DHT_COMMENT             133L
/** ������� ��*/
#define DHT_MID_WIDTH			134L
/** �������������� �� (�������/�����)*/
#define DHT_WIDTH_FRONT			135L
/** �������������� �� (�����/������)*/
#define DHT_WIDTH_BACK			136L
/** ������������ �����*/
#define DHT_UNI_WEAR			137L
/** ��������� ���������� ��� ����������� �����*/
#define DHT_STATION_APRINFO		138L
/** ������ ����������� ����� (Obsolete)*/
#define DHT_STATION_WAYS		139L
/** �������� ����� �� ����������� ����� � ��������*/
#define DHT_ATTACHMENTS			140L
/** ����������� ����*/
#define DHT_WAY					141L
/** ������ �� �������*/
#define DHT_STATION_TRAILS		142L
/** ������� ���� ����������� ���������� (Obsolete. � ����� �������� ����� ������� ����� � ����) */
#define DHT_STATION_KMS         143L
/** ������������� ���.���������� (����������/������) */
#define DHT_ABS_COORD_SHIFTED   144L
/** ������ */
#define DHT_LATITUDE	        145L
/** ������� */
#define DHT_LONGITUDE	        146L
/** ����� GPS */
#define DHT_GPS_TIME			147L
/** ��������� ��������� */
#define DHT_GEORADAR			148L
/** ����������� - �-�� ������ ������ */
#define DHT_GABA_LEFTRAIL		149L
/** ����������� - �-�� ������� ������ */
#define DHT_GABA_RIGHTRAIL		150L
/** ����������� - �-�� ����������� ������� */
#define DHT_GABA_KP				151L
/** ����������� - �-�� ����� */
#define DHT_GABA_TNL			152L
/** ����������� - ���������� ������� */
#define DHT_GABA_RESULT			153L
/** ��������� ������ (������ ������)*/
#define DHT_MODEL_CURVES		154L
/** ��������� �������� � �������, ��� ������� ���������� ������*/
#define DHT_GABA_TPARAMS		155L
/** ��������� ����� ����� */
#define DHT_KURS_BACKUP			156L
/** ����������� ������� */
#define DHT_CONTROL_AREA		157L
/** �������� ���� �������� (������ ����������)*/
#define DHT_HAND_CORRUPT		158L
/** ����� ���� */
#define DHT_SPALA_MARKER		159L
/** ������ ������� ������ */
#define DHT_PPR_WAYPASSED		160L
#define DHT_PPR_WIDTH			161L
#define DHT_PPR_NAK_LEFT		162L
#define DHT_PPR_NAK_RIGHT		163L
#define DHT_PPR_POD_LEFT		164L
#define DHT_PPR_POD_RIGHT		165L
#define DHT_PPR_IZN_LEFT		166L
#define DHT_PPR_IZN_RIGHT		167L
// -> - - - - - - ���������� - - - - - - - - - - -
#define DHT_GPS_LONGITUDE		201L
#define DHT_GPS_LATITUDE		202L
#define DHT_GPS_ALTITUDE		203L
#define DHT_TEST_4				204L
#define DHT_MAG_ZAZOR_LEFT		205L
#define DHT_MAG_ZAZOR_RIGHT		206L
#define DHT_TEST_7				207L
#define DHT_TEST_8				208L

#define DHT_KADR_COUNT			209L
#define DHT_OVRS3				210L
#define DHT_DPP					211L

#define DHT_PPR_IZN_EXT_LEFT    212L
#define DHT_PPR_IZN_EXT_RIGHT   213L
#define DHT_PPR_IZN_INT_LEFT    214L
#define DHT_PPR_IZN_INT_RIGHT   215L
// <- - - - - - - ���������� - - - - - - - - - - -

/** ����� ������� ���������*/
#define	DHT_FIRST_PARAM			DHT_SPEED
/** ����� ���������� ���������*/
#define	DHT_LAST_PARAM			DHT_PPR_WEAR_INT_RIGHT

