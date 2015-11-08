#pragma once

#include <string>

#include "c4flib.h"
#include "VParameter.h"
#include "VAprinfo.h"
#include "VPlanElemList.h"
#include "error_messages.h"

enum SAREA_STATUS
{
	SAS_OK = 0,			// ��
	SAS_NO_DATA = 1,	// �������� ���������
	SAS_EXCLUDED = 2	// ����������� ������� (������� ���� ��� ������������� ������)
};

struct VStatData
{
	double absBegin, absEnd;	// ������� � ���.�����������
	VWaySection section;		// ������� � ������� �����������
	short km, pk;				// ����� �� � ��; ��� ��-����� pk==0

	// ����� ����� (��������), ����������� �����, ����������� �����)
	size_t pntSum, pntCalc, pntEx;

	// ������ �������
	SAREA_STATUS GetStatus() const;

	float moWidth, sskoWidth; // ��
	float moLevel, sskoLevel; // �������
	float moLevelDiff, sskoLevelDiff; // ������� 
	float moPitLeft, sskoPitLeft; // ��.��
	float moPitRight, sskoPitRight; // ��.��
	float moRichLeft, sskoRichLeft; // �������� �����
	float moRichRight, sskoRichRight; // �������� ������

	bool havePPR; // �� ������� ��������� �������� ���
	float moPrLeft, sskoPrLeft; // ������� ������ �����
	float moPrRight, sskoPrRight; // ������� ������ ������
	float moNgrLeft, sskoNgrLeft; // ������ ������� ������ �����
	float moNgrRight, sskoNgrRight; // ������ ������� ������ ������

	float sigma; // �����
	float sssp; // ����

	float maxLevel, minLevel; // ���� � ��� ������
};

class VStatList : public VIndexList<VStatData>
{
public:

	VStatData Find(short km, short pk);
};

class VStatistics
{
	VStatList pkList, kmList;
	std::wstring message;

public:

	bool Load(const VAprinfo & aprInfo, const VAbsCoordData & absCoord, 
		const VParameter & level, const VParameter & level_zline, 
		const VParameter & pitLeft, const VParameter & pitRight, const VParameter & width, 
		const VParameter & richLeft, const VParameter & richRight,
		const VParameter & prLeft, const VParameter & prRight,
		const VParameter & ngrLeft, const VParameter & ngrRight,
		const VPlanElemList & planElemList);

	VStatList & GetPkList()
	{
		return this->pkList;
	}

	const VStatList & GetPkList() const
	{
		return this->pkList;
	}

	VStatList & GetKmList()
	{
		return this->kmList;
	}

	const VStatList & GetKmList() const
	{
		return this->kmList;
	}

	const std::wstring & GetErrorMessage() const 
	{
		return this->message;
	}

	bool IsLoaded() const
	{
		return this->pkList.GetCount() != 0 && this->kmList.GetCount() != 0;
	}

	void Unload()
	{
		this->kmList.Unload();
		this->pkList.Unload();
	}
};