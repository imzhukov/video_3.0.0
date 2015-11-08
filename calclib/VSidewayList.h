#pragma once

#include "VParameter.h"

const uint32_t REASON_SIDEWAY = 1;		///< ������� ����
const uint32_t REASON_HALFCURVE = 2;	///< �������� ������
const uint32_t REASON_UNDEFINED = 3;	///< ������� ���� ��� ������������� ������
const uint32_t REASON_WAY = 4;			///< ����

/// ����� �� ������� ���� - ������ �� �����
#pragma pack(push,1)
struct VSidewayRecord
{
	uint32_t startNdx;	///< ������ ������ � ����� ������ ���������� ������� ������
	uint32_t endNdx;	///< ������ ������ � ����� ��� ������������� ������� ������
	uint32_t reason;	/// ������� �� ������� ������� ����������� �� ���������
};
#pragma pack(pop)

/// ����� �� ������� ���� - ������ � ������
class VSideway
{
	uint32_t startNdx;	///< ������ ������ � ����� ������ ���������� ������� ������
	uint32_t endNdx;	///< ������ ������ � ����� ��� ������������� ������� ������
	uint32_t reason;	///< ������� �� ������� ������� ����������� �� ���������

public:

	// �����������
	VSideway() : startNdx(0), endNdx(0), reason(0)
	{}

	// ������ ������
	bool Import(const VSidewayRecord & record)
	{
		this->startNdx = record.startNdx;
		this->endNdx = record.endNdx;
		this->reason = record.reason;
		return true;
	}

	// ������� ������
	VSidewayRecord Export() const
	{
		VSidewayRecord record;
		record.startNdx = this->startNdx;
		record.endNdx = this->endNdx;
		record.reason = this->reason;
		return record;
	}

	// �������� �������
	bool UpdateIndex(const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs);

	// ������ ����� ������ �������
	uint32_t GetStartNdx() const
	{
		return this->startNdx;
	}

	// ������ ����� ������ �������
	uint32_t GetEndNdx() const
	{
		return this->endNdx;
	}

	// ������ �������
	uint32_t GetReason() const
	{
		return this->reason;
	}

	// �������� ������� �������
	const wchar_t * GetReasonDescr() const;

	// ������� ����������� � ������� �������, �� ����� ������
	bool Fits(size_t kadrCount) const
	{
		return this->startNdx < kadrCount && 
			this->endNdx < kadrCount && 
			this->startNdx < this->endNdx;
	}
};

// ������ �������� ������� �����
class VSidewayList : public VIndexList<VSideway>
{
public:

	/// �����������
	VSidewayList() : VIndexList<VSideway>()
	{}

	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, size_t kadrCount)
	{
		return this->DefaultLoad<FILETYPE, VSidewayRecord>(file, type, kadrCount);
	}

	/// ���������� � ����
	bool Save(VMainFile & file, uint32_t type)
	{
		return this->DefaultSave<VSidewayRecord>(file, type);
	}

	/// ����� ������ ����������� � ���������� �����
	size_t FindItem(size_t kadrNdx) const;
};