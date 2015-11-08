#pragma once

#include "VParameter.h"

// ---------------------------------------------------------------------------
/// ����� ����������� � ������� ����� (����� ���������)
// ---------------------------------------------------------------------------

/// ���� "����� - ����� �����" � �����
struct VFileIndexRecord
{
	uint32_t index;			// ����� �����
	union
	{
		uint32_t flags;		// �������� ����� (����� ���������, ���.������)
		float value;		// �������� ��������� (�����)
	};	
};

/// ����� ���������
struct VOpMark
{
	uint32_t index;		// ����� �����
	bool isIgnore;		// ������������
	bool isBridge;		// ����
	bool isRadar;		// �����
	bool isStation;		// ����.�����
	bool isSwitch;		// �������
	bool isPole;		// �����
	bool isPicket;		// �����
	bool isOptSwLeft;	// ������� ����� (���.) - ������
	bool isOptSwRight;	// ������� ������ (���.) - ������
	bool isOptXLeft;	// ������� ����� (���.) - ����������
	bool isOptXRight;	// ������� ������ (���.) - ����������

	VOpMark() : index(0), isIgnore(false), isBridge(false), isRadar(false), isStation(false), 
		isSwitch(false), isPole(false), isPicket(false), isOptSwLeft(false),
		isOptSwRight(false), isOptXLeft(false), isOptXRight(false)
	{}

	///���������� ��������� �������� ReviseMe!!!
	std::wstring ShortText() const
	{
		std::wstring text=L"";
		if (isBridge)
			text+=L"�";
		if (isRadar)
			text+=L"�";
		if (isStation)
			text+=L"�";
		if (isSwitch)
			text+=L"�";
		if (isPole)
			text+=L"�";
		if (isPicket)
			text+=L"�";
		if (isOptSwLeft)
			text+=L"� ��.";
		if (isOptSwRight)
			text+=L"� ��.";
		if (isOptXLeft)
			text+=L"� ��.";
		if (isOptXRight)
			text+=L"� ��.";
		return text;
	}

	///���������� ��������� ��������
	std::wstring LongText() const
	{
		std::wstring text=L"";
		if (isBridge)
			text+=L"����,";
		if (isRadar)
			text+=L"�����,";
		if (isStation)
			text+=L"����. �����,";
		if (isSwitch)
			text+=L"�������,";
		if (isPole)
			text+=L"�� �����,";
		if (isPicket)
			text+=L"�����,";
		if (isOptSwLeft)
			text+=L"������ �����,";
		if (isOptSwRight)
			text+=L"������ ������,";
		if (isOptXLeft)
			text+=L"���������� �����,";
		if (isOptXRight)
			text+=L"���������� ������,";
		text.erase(--text.end());
		return text;
	}

	//���������� ������� �������� ����
	//��� ��������� � ���� ������
	int DataBaseIndex() const
	{
		if (isBridge)		return 1;
		if (isRadar)		return 2;
		if (isStation)		return 3;
		if (isSwitch)		return 4;
		if (isPole)			return 5;
		if (isPicket)		return 6;
		if (isOptSwLeft)	return 7;
		if (isOptSwRight)	return 8;
		if (isOptXLeft)		return 9;
		if (isOptXRight)	return 10;
		else			    return 11;
	}

	// ������ �� ������ � �����
	bool Import(const VFileIndexRecord & src);

	// ������ �� ������ � ����� INM
	bool Import(uint8_t kms, uint8_t kms2, size_t index);

	// ������ �� ������ ������ � ����� INM
	bool Import(uint16_t kms, size_t index);

	// ������� ��� ����� c4f
	VFileIndexRecord Export() const;

	// ������� ��� ����� inm
	uint16_t ExportToInm() const;

	bool UpdateIndex(const VAbsCoordData & destAbs, const VAbsCoordData & srcAbs)
	{
		return destAbs.UpdateIndex(this->index, srcAbs);
	}

	// ������� ����������� � ������� �������, �� ����� ������
	bool Fits(size_t kadrCount) const { return this->index < kadrCount; }
};

/// ������ ����� ���������
class VOpMarkList : public VIndexList<VOpMark>
{
public:

	/// �����������
//	VOpMarkList() : VIndexList<VOpMark>()
//	{}

	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		return this->DefaultLoad<FILETYPE, VFileIndexRecord>(file, type, kadrCount);
	}

	/// ���������� � ����
	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VFileIndexRecord>(file, type);
	}
};

/// ��������� �����
struct VMagZazor
{
	uint32_t index;
	bool isLeft, isRight;

	VMagZazor() : index(0), isLeft(false), isRight(false)
	{}

	bool Import(const VFileIndexRecord & src);

	bool Import(uint8_t km2, size_t index);

	bool Import(uint16_t km, size_t index);

	VFileIndexRecord Export() const;

	uint16_t ExportToInm() const;

	bool UpdateIndex(const VAbsCoordData & destAbs, const VAbsCoordData & srcAbs)
	{
		return destAbs.UpdateIndex(this->index, srcAbs);
	}

	// ������� ����������� � ������� �������, �� ����� ������
	bool Fits(size_t kadrCount) const { return this->index < kadrCount; }

};

/// ������ ��������� �������
class VMagZazorList : public VIndexList<VMagZazor>
{
public:

	/// �����������
	VMagZazorList() : VIndexList<VMagZazor>()
	{}

	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		return this->DefaultLoad<FILETYPE, VFileIndexRecord>(file, type, kadrCount);
	}

	/// ���������� � ����
	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VFileIndexRecord>(file, type);
	}

	void Shift(size_t index, bool side, int shift)
	{
		this->GetItem(index).index += shift;
	}
};

/// ����
struct VUgon
{
	uint32_t index;
	float value;

	bool Import(const VFileIndexRecord & src);

	VFileIndexRecord Export() const;

	/// �������� ��������
	bool UpdateIndex(const VAbsCoordData & destAbs, const VAbsCoordData & srcAbs)
	{
		return destAbs.UpdateIndex(this->index, srcAbs);
	}

	// ������� ����������� � ������� �������, �� ����� ������
	bool Fits(size_t kadrCount) const { return this->index < kadrCount; }
};

/// ������ ������
class VUgonList : public VIndexList<VUgon>
{
public:

	/// �����������
	VUgonList() : VIndexList<VUgon>()
	{}

	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		return this->DefaultLoad<FILETYPE, VFileIndexRecord>(file, type, kadrCount);
	}

	/// ���������� � ����
	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VFileIndexRecord>(file, type);
	}
	
	/// ������� ��������� ���������� ������ (����� ������, �� �����!!!!)
	bool IsCorrupt(size_t index) const
	{
		return true;/// FixMe!!!
	}

	/// ���������� ��������� ���������� ������ (����� ������, �� �����!!!!)
	void SetCorrupt(size_t index, bool value)
	{
		/// FixMe!!! ��������� ����� ����� �����
	}
};

/// ����� ����
struct VSpalaMarker
{
	uint32_t index;
	uint32_t value;

	bool Import(const VFileIndexRecord & src);

	VFileIndexRecord Export() const;

	/// �������� ��������
	bool UpdateIndex(const VAbsCoordData & destAbs, const VAbsCoordData & srcAbs)
	{
		return destAbs.UpdateIndex(this->index, srcAbs);
	}

	// ������� ����������� � ������� �������, �� ����� ������
	bool Fits(size_t kadrCount) const { return this->index < kadrCount; }
};

/// ������ ������
class VSpalaMarkerList : public VIndexList<VSpalaMarker>
{
public:

	/// �����������
	VSpalaMarkerList() : VIndexList<VSpalaMarker>()
	{}

	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		return this->DefaultLoad<FILETYPE, VFileIndexRecord>(file, type, kadrCount);
	}

	/// ���������� � ����
	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VFileIndexRecord>(file, type);
	}

	bool Shift(float value, VAbsCoordData  & abscoord);
};
