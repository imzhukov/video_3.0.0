#pragma once

#include "parameters.h"
#include "VBasicParameter.h"

extern const uint32_t
ERROR_NONE,			// ��� ������
ERROR_ABS_COORD,	// ���p�� ���
ERROR_WIDTH,		// ���p�� �������
ERROR_LEVEL,		// ���p�� ������
ERROR_DIST_SHORT,	// ���p�� �������� ���������
ERROR_DIST_LONG,	// ���p�� �������� ��p������
ERROR_GRADIENT,		// ���p�� ������
ERROR_PIT_LEFT,		// ���p�� �������� �������� �����
ERROR_PIT_RIGHT,	// ���p�� �������� �������� ������		
ERROR_WIDTH_FRONT,	// ������ "�� �������"
ERROR_HEIGHT,		// ���p�� ������
ERROR_RICH_LEFT,	// ���p�� �������� �������� �����
ERROR_RICH_RIGHT,	// ���p�� �������� �������� ������
ERROR_CURV,			// ���p�� ��������
ERROR_KURS,			// ���p�� �����		
ERROR_WIDTH_BACK,	// ������ "�� �����"		
ERROR_WEAR,			// ������ ������������� ������ (�� ����-1)
ERROR_WEAR_LEFT,	// ������ ������ ������
ERROR_WEAR_RIGHT,	// ������ ������ �������
ERROR_PPR,			// ������ ������� �������
ERROR_ZAZOR_LEFT,	// ������ ������ ������
ERROR_ZAZOR_RIGHT,	// ������ ������� ������
ERROR_PPR_NEW,      // ����� ������ ������� ������� (4 ������)
ERROR_MAG_ZAZOR_LEFT,	// ������ ������ ���������� ������
ERROR_MAG_ZAZOR_RIGHT;	// ������ ������� ���������� ������

class VParameterCorrupt
{
	std::vector<bool> data;

public:

	void Create(size_t kadrCount)
	{
		this->data = std::vector<bool>(kadrCount, false);
	}

	void Unload()
	{
		this->data.clear();
	}

	size_t GetCount() const
	{
		return this->data.size();
	}

	bool IsLoaded() const
	{
		return !this->data.empty();
	}

	bool IsCorrupt(size_t index) const
	{
		return this->data.at(index);
	}

	void SetCorrupt(size_t index, bool value)
	{
		this->data.at(index) = value;
	}

	void CopyFrom(const VParameterCorrupt & src, size_t kadrFrom, size_t kadrTo)
	{
		this->data.resize(kadrTo - kadrFrom + 1);
		std::copy(src.data.begin() + kadrFrom, src.data.begin() + kadrTo, this->data.begin());
	}

	void Shift(int shift);
};

typedef boost::shared_ptr<VParameterCorrupt> VParameterCorruptPtr;

class VCorruptData
{
	// ������������ ���������� - ����� ������ ���
	VBasicParameter corrupt;

	VParameterCorruptPtr absCoord, width, widthFront, widthBack, 
		level, distShort, distLong, grad, pitLeft, pitRight, richLeft, richRight, 
		curv, kurs, wearSensor, wearLeft, wearRight, ppr, pprNew,
		zazLeft, zazRight, mzLeft, mzRight;

	void CreateBlocks(size_t kadrCount);

	bool isLoaded;
	std::wstring message;

public:

	/// �����������
	VCorruptData();

	/// �������� �� ���������� �����
	template <class FILETYPE> bool Load(FILETYPE & file);

	/// ������ ���������
	bool IsLoaded() const
	{
		return this->isLoaded;
	}

	/// ���������� ���� � �������� ������
	void SetLoaded(bool value)
	{
		this->isLoaded = value;
	}

	/// ��������� �� ������
	const std::wstring & GetErrorMessage() const
	{
		return this->message;
	}

	/// ������ ��������� � ������
	bool IsAlloc() const
	{
		return this->corrupt.IsAlloc();
	}

	/// ����������� �� ������� �����
	bool CopyFrom(const VCorruptData & src, size_t kadrFrom, size_t kadrTo);

	/// �������� ������
	void Unload();

	/// ���������� � ����
	bool Save(VMainFile & file);

	/// ������� ����� ����������
	VParameterCorruptPtr GetCorruptChannel(PARAMETER_TYPE param) const;

	/// �������
	void Create(uint32_t kadrCount, bool manualBlock);

	// ---- ������ �������� � ������ "����������� ����������"

	/// �������� �� 32-������� ����
	void SetCorruptValue(size_t kadrNdx, uint32_t value)
	{
		this->corrupt.GetItem<uint32_t>(kadrNdx) = value;
	}

	/// �������� �� ���� 16-������ �����
	void SetCorruptValue(size_t kadrNdx, uint16_t value1, uint16_t value2)
	{
		this->SetCorruptValue(kadrNdx, (uint32_t) value1 + 65536ul * (uint32_t) value2);
	}

	/// ����� �������
	size_t GetCount() const
	{
		return this->corrupt.GetRecCount();
	}

	//������ �� i-�� �������
	uint32_t & GetItem(size_t i)
	{
		return this->corrupt.GetItem<uint32_t>(i);
	}

};


