#pragma once

#include "VParameter.h"
#include "VCorrupt.h"

struct VZazor
{
	uint32_t index;
	float value;

	bool UpdateIndex(const VAbsCoordData & destAbs, const VAbsCoordData & srcAbs)
	{
		return destAbs.UpdateIndex(this->index, srcAbs);
	}
};

	
enum TYPE_FINDING
{
	NEXT_ZAZOR,			//���� ��������� �� ��������� ���������� �����
	PREVIOUS_ZAZOR,		//���� ���������� ���������� ���������� �����
	NEXT_2_ZAZOR,		//���� ��������� 2�� �� ��������� ���������� �����
	PREVIOUS_2_ZAZOR,	//���� ���������� 2�� ���������� ���������� �����
	CLOSE_ZAZOR			//���� ��������� ���������� ���������� � �������, ��������� � ������� ������
};

class VZazorList : public VIndexList<VZazor>
{
	VParameterCorruptPtr corrupt;

public:

	/// �����������
	VZazorList() : VIndexList<VZazor>(), corrupt(new VParameterCorrupt)
	{}

	/// ��������
	void Unload()
	{
		VIndexList<VZazor>::Unload();
		this->kadrCount = 0;
	}

	/// �������� �� �����
	bool Load(VMainFile & file, uint32_t type, VParameterCorruptPtr corr);

	/// �������� �� �����
	bool Load(VRemoteFile & remote, uint32_t type, VParameterCorruptPtr corr);

	/// �������� �� ���������
	bool Load(const VParameter & prm, VParameterCorruptPtr corr);

	/// ���������� � ����
	bool Save(VMainFile & file, uint32_t type);

	/// ����������� �� ������� ������, � ���������� ���������
	bool CopyFrom(const VZazorList & src, 
		const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs);

	/// ������� ������ ������� � ��������� ��������� ������ ������������ �����
	VZazorList FindZazor(uint32_t index, double range, const VAbsCoordData & absCoordData);

	/// ������� ������ ������� ���������� �� ��������� (��������� � ����� index)
	VZazorList FindZazor(uint32_t index, TYPE_FINDING type);

	/// ������� ��������� ���������� ������ (����� ������, �� �����!!!!)
	bool IsCorrupt(size_t index) const;

	/// ���������� ��������� ���������� ������ (����� ������, �� �����!!!!)
	void SetCorrupt(size_t index, bool value);
};