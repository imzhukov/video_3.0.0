#pragma once

#include "calclib.h"

/// ���� ����� � �����
enum FILE_LABEL_TYPE
{
	FILE_LABEL_UNDEFINED	= 0,	/// ������������, ������
	FILE_LABEL_PROF_LINE	= 1,	/// �������, ������ ������
	FILE_LABEL_PROF_CRV		= 2,	/// �������, ������ ������
	FILE_LABEL_LINE			= 3,	/// ������
	FILE_LABEL_CRV_PLAN		= 4,	/// ������, �� �����
	FILE_LABEL_CRV_LEVEL	= 5		/// ������, �� ������
};

/// ����� � ����� - ����� ���������
struct VFileMark
{
	uint32_t index;				/// ����� �����
	int32_t mark_type;			/// ��� �����
	float value;				/// �������� ���������
	uint8_t reserve[52];		/// ������������ �� 64 ����

	// �����������
	VFileMark();

};