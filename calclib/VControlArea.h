#pragma once

#include "VParameter.h"

/// ���������� �� ����� ��������� - ��������� � �����
#pragma pack (push,1)
struct VControlAreaParam
{
	/// ��������, ���.��������, ���
	float norma, mo, sko;

	/// ����������
	VControlAreaParam() : norma(0.0f), mo(0.0f), sko(0.0f)
	{}
};
#pragma pack (pop)

/// ���������� �� ������� - ��������� � �����
#pragma pack (push,1)
struct VFileControlArea
{
	/// ������ ������
	int version;

	/// �� � �������
	VControlAreaParam width, level;

	/// ������� ������� (� ��������� ��� ����� ��������� WAYSECTION)
	short kmn;
	float mn;
	short kmk;
	float mk;

	/// ������
	unsigned char reserve[88];

	/// �����������
	VFileControlArea() : version(1)
	{
		ZeroMemory(reserve, sizeof(reserve));
	}
};
#pragma pack (pop)

/// ���������� �� ������� - ��������� � ������
struct VControlArea
{
	/// ������ ������
	int version;

	/// �� � �������
	VControlAreaParam width, level;

	/// �������
	VWaySection section;

	/// ������ �� ��������� �� �����
	bool Import(const VFileControlArea & record);

	/// �������
	VFileControlArea Export() const;

	/// ����������� ������������ � �������
	bool MeetsSection(const VWaySection * section) const
	{
		return section == NULL || 
			!(this->section.end < section->start || this->section.start > section->end);
	}

	// ������� ����������� � ������� �������, �� ����� ������
	bool Fits(size_t kadrCount) const { return true; }
};

/// ������ ����������� ��������
class VControlAreaList : public VIndexList<VControlArea>
{
public:

	/// �����������
	VControlAreaList() : VIndexList<VControlArea>()
	{}

	/// �������� �� �����
	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		return this->DefaultLoad<FILETYPE, VFileControlArea>(file, type, kadrCount);
	}

	/// ���������� � ����
	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VFileControlArea>(file, type);
	}

	/// ����������� ������
	bool CopyFrom(const VControlAreaList & src, const VWaySection * section);
};