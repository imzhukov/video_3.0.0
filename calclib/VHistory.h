/*
���� VHistory.h
������� �������� ������������� ��� ������
*/

#pragma once

#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "types.h"
#include "c4flib.h"
#include "VParameter.h"

using namespace std;

// ���� ��������

/** �������� ����� ��� ������� � RT*/
#define OC_CREATION 0xFFFFFFFF
/** �������������� ��������*/
#define OC_UNDEFINED 0L
/** �������������� �������� ���������*/
#define OC_AUTOCORRECTION 1L
/** ������ �������� ���������*/
#define OC_COORDCORRECTION 2L
/** ������ ��������� ����������*/
#define OC_CHANGEAPRINFO 3L
/** ���� ��*/
#define OC_SHIFTSHABLON 4L
/** ���� ������*/
#define OC_SHIFTLEVEL 5L
/** ���� ������*/
#define OC_SHIFTGRADIENT 6L
/** ���� ������ ������*/
#define OC_SHIFTIZNOSLEFT 7L
/** ���� ������� ������*/
#define OC_SHIFTIZNOSRIGHT 8L
/** ������ �������� ������*/
#define OC_FULLMARKCURVES 9L
/** ��������� �������� ������*/
#define OC_MARKCURVES 10L
/** ��������� �������� �������*/
#define OC_MARKPROFILE 11L
/** ��������� �������� ������*/
#define OC_MARKPLAN 12L
/** ��������� ����� ������*/
#define OC_CORRECTIZNOS 13L
/** ����� ����� ��������*/
#define OC_SHIFTRICHL 14L
/** ����� ������ ��������*/
#define OC_SHIFTRICHR 14L
/** ����� �� �������*/
#define OC_SHIFTWIDTHFRONT 15L
/** ����� �� �����*/
#define OC_SHIFTWIDTHBACK 16L
/** ��������� ��������� �� ���������� �����*/
#define OC_CORRECTIONBYFILE 17L
/** ��������� ��������� �� ������ �� ��*/
#define OC_CORRECTIONBYCURVES 18L
/** ������ ���� �������*/
#define OC_DATECORRECTION 19L
/** ������ ������ ������*/
#define OC_DIVISIONCORRECTION 20L
/** ������ ������ ������*/
#define OC_ROADNUMCORRECTION 21L
/** ��������� �����*/
#define OC_CORRECTCOURSE 22L
/** �������� ��. �������� �� ����� ����*/
#define OC_CALC_AVG_CRV 23L
/** �������� ��. ������ �� ����� ����*/
#define OC_CALC_AVG_LVL 24L
/** ��������� ���. ����������*/
#define OC_OPTICS_SYNCH 25L
/** ���������� ���. ����������*/
#define OC_OPTICS_CHKERRORS 26L
/** ����� ����� ����� ���������*/
#define OC_KURS_BACKUP 27L
/** �������� �����*/
#define OC_KURS_SHIFT 28L
/** �������������� ������������� �����*/
#define OC_KURS_SHIFT_UNDO 29L
/** ������������� ������*/
#define OC_FULLMODELCURVES 30L
/** �������� �� � �������� ��������*/
#define OC_WIDTH_FALSIFICATION 31L
/** ����� �����*/
#define OC_SHIFTCOURSE 34L
/** ����� ������ ����� (���)*/
#define OC_SHIFT_SHABLON_PPR 35L
/** ����� ������ ������� (���)*/
#define OC_SHIFT_NAKLON_LEFT_PPR 36L
/** ����� ������� ������� (���)*/
#define OC_SHIFT_NAKLON_RIGHT_PPR 37L
/** ����� ����� ����������*/
#define OC_SHIFT_INCLINE_LEFT_PPR 38L
/** ����� ������ ����������*/
#define OC_SHIFT_INCLINE_RIGHT_PPR 39L
/** ����� ������ ������ (���)*/
#define OC_SHIFT_IZNOS_LEFT_PPR 40L
/** ����� ������� ������ (���)*/
#define OC_SHIFT_IZNOS_RIGHT_PPR 41L
/** ������������ �����������*/
#define OC_RECALC 42L

class VHistory;

/** ������ �� ��������*/
class VOperationRecord
{
friend VHistory;
private:
	/** ���� � ����� ������*/
	VTime operation_time;
	/** ��� ��������*/
	uint32_t operation_code;
	/** ������ ���������� ��������*/
	uint32_t text_length;
	/** ��������� ��������*/
	char * text_description;
	/** ������ ����� ������������ ������*/
	uint32_t data_size;
	/** ���� ������������ ������, ������������ ����� ��������*/
	void * data;
public:	
	/** �����������*/
	VOperationRecord();
	/** ���������� �����������*/
	VOperationRecord(const VOperationRecord &source);
	/** ����������*/
	~VOperationRecord();

	//������

	/** ���������� ���� ������*/
	const VTime& GetDateTime() const;	
	/** ���������� ����� ������ ������*/
	size_t GetTotalSize() const;
	/** ��������� � ����� ������*/
	bool SaveToMemory(void * lpData);
	/** ��������� �� ����� ������*/
	bool LoadFromMemory(void * lpData);
	/** ���������� ��� ��������*/
	unsigned int GetCode() const;
	/** ������������� ��� ��������*/
	void SetCode(unsigned int uiCode);
	/** ���������� ����� ���������� ��������*/
	uint32_t GetTextLength() const;
	/** ���������� ��������� �� ��������� ��������*/
	const char * GetTextDescription() const;
	/** ������������� ��������� ��������*/
	void SetTextDescription(const std::string & sTextDescription);
	/** ������������� ��������� ��������*/
	void SetTextDescription(const char * cTextDescription, unsigned int uiTextLength);
	/** ���������� ������ ������������ ������*/
	size_t GetDataSize() const;
	/** ���������� ��������� �� ���� ������*/
	const void * GetData() const;
	/** ���������� ������������ ������*/
	void SetData(void * lpData, unsigned int uiDataSize);
	/** �������� ������������*/
	VOperationRecord& operator=(const VOperationRecord &source);	
};


/** ������ ������� � ����������� ���������*/
class VHistory : public VParameterProto<VOperationRecord>
{
private:

	//������

	/** ������ �������*/
	vector<VOperationRecord> m_Operations;

public:

	/** �����������*/
	VHistory();

	/** ����������*/
	~VHistory();	

	/** �������� ������ �� ��������� �����*/
	bool Load(VMainFile & file);	
	bool Load(VRemoteFile & file);	

	/** ���������� ������ � �������� �����*/
	bool Save(VMainFile & file);	

	/** ���������� ���������� ���������*/
	size_t GetCount() const;

	/** ���������� ������ ���� �������*/
	size_t GetTotalSize() const;

	/** ������� �������*/
	bool RemoveAt(size_t index);

	/** ���������� ��������� �� �������*/
	const VOperationRecord  & GetItem(size_t index) const;

	/** ���������� ��������� �� �������*/
	VOperationRecord  & GetItem(size_t index);

	/** ��������� �������*/
	void Add(const VOperationRecord &operationRecord);

	/** ��������� ���� �� ���� ���� ������ �� ������������ ��������*/
	bool OperationExists(unsigned int uiCode);

	/** ��������� ������*/
	void Unload()
	{
		this->m_Operations.clear();
		VParameterProto<VOperationRecord>::Unload();
	}

	/** ������� ������ ��������� ��������� ����*/
	std::string ExtractCreatorVersion() const;
};
