#pragma once

#include <typeinfo.h>
#include <string>

#include "c4flib.h"
#include "error_messages.h"

/// ���� ������
class VMainFile;
class VRemoteFile;

/// ������ ����������� �����
uint32_t GetCrc32(const void *s, size_t len);

// ---------------------------------------------------------------------------
/// ������� ����� ��� ������ � ������ ������ � �����
// ---------------------------------------------------------------------------
class VBasicParameter
{
	bool isLoaded;		///< ������ ���������
	uint8_t * dataPtr;	///< ��������� �� ����� ������
	bool isAlloc;		///< ������ ��� ����� �������� ���
	size_t dataSize;	///< ������ ����� ������
	uint32_t dataType;	///< ��� ������ � �����
	uint32_t index;		///< ������
	uint32_t version;	///< ������ ���������
	VTime createTime;	///< ����� �������� �����
	VTime updateTime;	///< ����� ��������� �����
	size_t recCount;	///< ���������� �������
	size_t recSize;		///< ������ ������
	int cType;			///< �-��� ������
	uint32_t flags;		///< �����
	uint32_t crc;		///< ����������� ����� �����
	std::wstring message;///< ��������� �� ������

	// ��������� ����������� ������
	VBasicParameter(const VBasicParameter &);

	// �������� ������������ ������
	VBasicParameter & operator = (const VBasicParameter &);

public:

	/// �����������
	VBasicParameter() : isLoaded(false), dataPtr(NULL), isAlloc(false), 
		dataType(-1), index(0), version(0), createTime(), updateTime(), 
		recCount(0), recSize(0), cType(VCT_UNKNOWN), flags(0), crc(0),
		message(ERRMSG_DATA_NOT_LOADED)
	{}

	/// ����������
	~VBasicParameter()
	{
		this->Unload();
	}

	/// ��������� �� ������
	const std::wstring & GetErrorMessage() const
	{
		return this->message;
	}

	/// �������� �� �����
	bool Load(VMainFile & file, uint32_t type, bool copyToMemory);

	/// �������� �� ���������� �����
	bool Load(VRemoteFile & remote, uint32_t type, bool copyToMemory);

	/// �������� �� ����� ������
	bool LoadBlock(const VDataBlock * dataBlock, bool copyToMemory);

	/// ���������� �� �����
	void UnloadFile(VMainFile & file, uint32_t type);

	/// ���������� �� ���������� �����
	void UnloadFile(VRemoteFile & remote, uint32_t type);

	/// �������� ������������ ��������� (���� � ������, �� � �����)
	bool Create(uint32_t dataType, uint32_t cType, uint32_t recCount, uint32_t recSize, uint32_t version);

	/// ������ ����� � ����
	/// recCount ����������������� �������� � ���������
	/// ��� �������� � �������������� recSize,
	/// �.�. ����� ���� ���������� � recCount = 1
	/// ���� ��� �������� ����� recCount > 1, ��� �������� ������������
	/// ���� __recCount �������� ������ 0, ��� �������� ������������
	bool Save(VMainFile & file, uint32_t recCount, uint32_t recSize);

	/// ������ � ����� ����������
	bool IsChanged() const
	{
		return this->crc != GetCrc32(this->dataPtr, this->dataSize);
	}

	/// �������� ���������
	void Unload();

	/// �������� ��������?
	bool IsLoaded() const
	{
		return this->isLoaded;
	}

	/// ���������� ���� �������� ���������
	void SetLoaded(bool value)
	{
		this->isLoaded = value;
	}

	/// �������� ��������� � ������
	bool IsAlloc() const
	{
		return this->isAlloc;
	}

	/// �����
	uint32_t GetFlags() const
	{
		return this->flags;
	}

	/// ����� �������
	size_t GetRecCount() const 
	{
		return this->recCount;
	}

	/// ������ ������, �� ���������� �����
	size_t GetRecSize() const
	{
		return this->recSize;
	}

	/// ������ ����� ������
	size_t GetDataSize() const
	{
		return this->dataSize;
	}

	/// ������� ��� ������
	uint32_t GetDataType() const
	{
		return this->dataType;
	}

	/// ������� ��� �
	int GetCType() const
	{
		return this->cType;
	}

	/// ������� ������
	uint32_t GetIndex() const
	{
		return this->index;
	}

	/// ������ ���������
	size_t GetVersion() const
	{
		return this->version;
	}

	/// �������� ������ ���������
//	void UpdateVersion(uint32_t version)
//	{
//		this->version = version;
//	}

	/// ������� ����� ��������
	const VTime & GetCreateTime() const
	{
		return this->createTime;
	}

	/// ������� ����� ����������
	const VTime & GetUpdateTime() const
	{
		return this->updateTime;
	}

	/// �������� ����� ���������� �����
	void UpdateTime(const VTime & updateTime)
	{
		this->updateTime = updateTime;
	}

	/// ����������� ��������� �� ������ �� �������� � ������
	const void * GetPtr(size_t offset) const
	{
		return this->dataPtr + offset;
	}

	/// ��������� �� ������ �� �������� � ������
	void * GetPtr(size_t offset)
	{
		return this->dataPtr + offset;
	}

	/// ����������� ��������� �� �������������� ������ �� �������� � ���������
	template <class T> const T * GetPtr(size_t offset) const 
	{
		return static_cast<const T*>(this->GetPtr(offset * sizeof(T)));
	}

	/// ��������� �� �������������� ������ �� �������� � ���������
	template <class T> T * GetPtr(size_t offset)
	{
		return static_cast<T*>(this->GetPtr(offset * sizeof(T)));
	}

	/// �������� ������������ ���� "C"
	template <class T> bool CheckCType() const;

	/// �������� ����� ������
	template <class T> bool CheckRecCount() const
	{
		return this->dataSize == this->recCount * sizeof(T) && this->recSize == sizeof(T);
	}

	/// �������� ����������� ������ ��  i-� �������
	template <class T> const T & GetItem(size_t i) const
	{
		return *this->GetPtr<T>(i);
	}

	/// �������� ������ ��  i-� �������
	template <class T> T & GetItem(size_t i) 
	{
		return *this->GetPtr<T>(i);
	}

	/// ������-���������������� ������ �� �����
	/// ������� ������� ����������� �� ������� ���������� position
	void ReadBlock(void * buf, size_t len, size_t & position) const
	{
		memcpy(buf, this->GetPtr(position), len);
		position += len;
	}

	/// ��������� ������� ������-����������������� ������ �� �����
	template <class T> void Read(T & buf, size_t & position) const
	{
		this->ReadBlock(&buf, sizeof(buf), position);
	}

	/// ������-���������������� ������ � ����
	/// ������� ������� ����������� �� ������� ���������� position
	void WriteBlock(const void * buf, size_t len, size_t & position)
	{
		memcpy(this->GetPtr(position), buf, len);
		position += len;
	}

	/// ��������� ������� ������-���������������� ������ � ����
	template <class T> void Write(const T & buf, size_t & position)
	{
		this->WriteBlock(&buf, sizeof(buf), position);
	}
};

// �������� ������������ ���� "C" ��� ��������� �������
template <class T> bool VBasicParameter::CheckCType() const
{
	int cType = this->GetCType();
	size_t recSize = this->GetRecSize();

	if (cType == VCT_UNKNOWN || cType == VCT_USER) 
		return false;

	else if (typeid(T) == typeid(float) && cType == VCT_FLOAT && recSize == sizeof(float)) 
		return true;

	else if (typeid(T) == typeid(double) && cType == VCT_DOUBLE && recSize == sizeof(double)) 
		return true;

	else if (typeid(T) == typeid(int) && cType == VCT_INT && recSize == sizeof(int)) 
		return true;	

	else if (typeid(T) == typeid(unsigned int) && cType == VCT_UINT && recSize == sizeof(int)) 
		return true;	

	else if (typeid(T) == typeid(long) && cType == VCT_LONG && recSize == sizeof(long)) 
		return true;	

	else if (typeid(T) == typeid(unsigned long) && cType == VCT_ULONG && recSize == sizeof(unsigned long)) 
			return true;	

	else if (typeid(T) == typeid(char) && cType == VCT_CHAR && recSize == sizeof(char)) 
		return true;	

	else if (typeid(T) == typeid(unsigned char) && cType == VCT_UCHAR && recSize == sizeof(unsigned char)) 
		return true;	

	else if (typeid(T) == typeid(short) && cType == VCT_SHORT && recSize == sizeof(short)) 
		return true;	

	else if (typeid(T) == typeid(unsigned short) && cType == VCT_USHORT && recSize == sizeof(unsigned short)) 
		return true;	

	else 
		return false;
}

/// ������� ���������� C-��� ��� ���������� ���� T
template <class T> uint32_t GetCType() 
{
	if (typeid(T) == typeid(float))					return VCT_FLOAT;
	else if (typeid(T) == typeid(double))			return VCT_DOUBLE;
	else if (typeid(T) == typeid(int))				return VCT_INT;
	else if (typeid(T) == typeid(unsigned int))		return VCT_UINT;
	else if (typeid(T) == typeid(long))				return VCT_LONG;
	else if (typeid(T) == typeid(unsigned long))	return VCT_ULONG;
	else if (typeid(T) == typeid(char))				return VCT_CHAR;
	else if (typeid(T) == typeid(unsigned char))	return VCT_UCHAR;
	else if (typeid(T) == typeid(short))			return VCT_SHORT;
	else if (typeid(T) == typeid(unsigned short))	return VCT_USHORT;
	else											return VCT_USER;
}
