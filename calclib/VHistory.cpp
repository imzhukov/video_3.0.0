/*
���� VHistory.cpp
������� �������� ������������� ��� ������
*/

#include "vhistory.h"
#include "VBasicParameter.h"

//---------------------------------------������ ������ VHistory-------------------------------------------------

/** �����������*/
VOperationRecord::VOperationRecord()
{
	//������������� �������� �� ���������
	memset(&operation_time, 0, sizeof(VTime));
	operation_code=0;
	text_length=0;
	text_description=NULL;
	data_size=0;
	data=NULL;
}

/** ���������� �����������*/
VOperationRecord::VOperationRecord(const VOperationRecord &source)
{
	//������������� �������� �� ���������
	memset(&operation_time, 0, sizeof(VTime));
	operation_code=0;
	text_length=0;
	text_description=NULL;
	data_size=0;
	data=NULL;
	//�������� ��������
	operation_time=source.operation_time;
	operation_code=source.operation_code;
	SetTextDescription(source.text_description, source.text_length);
	SetData(source.data, source.data_size);
}

/** ����������*/
VOperationRecord::~VOperationRecord()
{
	//����������� ������
	::GlobalFree(text_description);
	::GlobalFree(data);
}

/** ���������� ���� ������*/
const VTime& VOperationRecord::GetDateTime() const
{
	return operation_time;
}

/** ���������� ����� ������ ������*/
size_t VOperationRecord::GetTotalSize() const
{
	return sizeof(VTime)+
		   sizeof(operation_code)+
		   sizeof(text_length)+
		   text_length+
		   sizeof(data_size)+
		   data_size;
}
/** ��������� � ����� ������*/
bool VOperationRecord::SaveToMemory(void * lpMemory)
{	
	if (lpMemory==NULL)
		return false;
	//�������� ���
	unsigned char* ucMemory=(unsigned char*)lpMemory;
	try
	{
		//�����
		memcpy((void*)ucMemory, &operation_time, sizeof(VTime));
		ucMemory+=sizeof(VTime);
		//��� ��������
		memcpy((void*)ucMemory, &operation_code, sizeof(unsigned int));
		ucMemory+=sizeof(unsigned int);
		//����� ������
		memcpy((void*)ucMemory, &text_length, sizeof(unsigned int));
		ucMemory+=sizeof(unsigned int);
		//�����
		memcpy((void*)ucMemory, text_description, text_length);
		ucMemory+=text_length;
		//������ ����� ������������ ������
		memcpy((void*)ucMemory, &data_size, sizeof(unsigned int));
		ucMemory+=sizeof(unsigned int);
		//������������ ������
		memcpy((void*)ucMemory, data, data_size);
	}
	catch (...)
	{
		return false;
	}
	return true;
}
/** ��������� �� ����� ������*/
bool VOperationRecord::LoadFromMemory(void * lpMemory)
{
	if (lpMemory==NULL)
		return false;
	//�������� ���
	unsigned char* ucMemory=(unsigned char*)lpMemory;
	try
	{
		//�����
		memcpy(&operation_time, (void*)ucMemory, sizeof(VTime));
		ucMemory+=sizeof(VTime);
		//��� ��������
		memcpy(&operation_code, (void*)ucMemory, sizeof(unsigned int));
		ucMemory+=sizeof(unsigned int);
		//����� ������
		memcpy(&text_length, (void*)ucMemory, sizeof(unsigned int));
		ucMemory+=sizeof(unsigned int);
		//�����
		text_description=(char*)::GlobalAlloc(GPTR, text_length);
		::CopyMemory(text_description, (void*)ucMemory, text_length);
		ucMemory+=text_length;
		//������ ����� ������������ ������
		memcpy(&data_size, (void*)ucMemory, sizeof(unsigned int));
		ucMemory+=sizeof(unsigned int);
		//������������ ������
		data=::GlobalAlloc(GPTR, data_size);
		::CopyMemory(data, (void*)ucMemory, data_size);
	}
	catch (...)
	{
		return false;
	}
	return true;
}

/** ���������� ��� ��������*/
unsigned int VOperationRecord::GetCode() const
{
	return operation_code;
}

/** ������������� ��� ��������*/
void VOperationRecord::SetCode(unsigned int uiCode)
{
	operation_code=uiCode;
}

/** ���������� ����� ���������� ��������*/
uint32_t VOperationRecord::GetTextLength() const
{
	return text_length;
}

/** ���������� ��������� �� ��������� ��������*/
const char * VOperationRecord::GetTextDescription() const
{
	return text_description;
}

/** ������������� ��������� ��������*/
void VOperationRecord::SetTextDescription(const std::string & sTextDescription)
{
	this->SetTextDescription(sTextDescription.c_str(), sTextDescription.length());
}

/** ������������� ��������� ��������*/
void VOperationRecord::SetTextDescription(const char * cTextDescription, unsigned int uiTextLength)
{
	text_length=uiTextLength;
	::GlobalFree(text_description);
	text_description = (char*) ::GlobalAlloc(GPTR, text_length+1);
	::CopyMemory(text_description, cTextDescription, text_length);
}

/** ���������� ������ ������������ ������*/
size_t VOperationRecord::GetDataSize() const
{
	return data_size;
}

/** ���������� ��������� �� ���� ������*/
const void * VOperationRecord::GetData() const
{
	return data;
}

/** ���������� ������������ ������*/
void VOperationRecord::SetData(void * lpData, unsigned int uiDataSize)
{
	data_size = uiDataSize;
	::GlobalFree(data);
	data= :: GlobalAlloc(GPTR, data_size);
	::CopyMemory(data, lpData, data_size);
}

/** �������� ������������*/
VOperationRecord& VOperationRecord::operator=(const VOperationRecord &source)
{	
	operation_time=source.operation_time;
	operation_code=source.operation_code;
	SetTextDescription(source.text_description, source.text_length);
	SetData(source.data, source.data_size);
	return *this;
}

//------------------------------------------------------------------------------------------------------

/** �����������*/
VHistory::VHistory()
{
	//
}

/** ����������*/
VHistory::~VHistory()
{	
	this->Unload();
}

/** �������� ������ �� �����*/
bool VHistory::Load(VMainFile & file)
{
	if (this->IsLoaded()) return true;

	if (!file.Loaded())
		return false;

	size_t index = file.GetBlockIndex(DHT_HISTORY);
	if (index == BAD_SIZE) return false;

	const VDataBlock * data_block=file.MapBlock(index);
	if (data_block == NULL) return false;

	//�������� ���
	unsigned char * ucHistory=(unsigned char*)data_block->Data;
	//������
	for (size_t i=0; i<data_block->Header->RecCount; ++i)
	{
		VOperationRecord newOperationRecord;
		newOperationRecord.LoadFromMemory(ucHistory);
		ucHistory+=newOperationRecord.GetTotalSize();
		m_Operations.push_back(newOperationRecord);
	}	
	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	file.UnmapBlock(index);
	return true;	
}

/** �������� ������ �� �����*/
bool VHistory::Load(VRemoteFile & remote)
{
	if (this->IsLoaded()) return true;

	if (!remote.Loaded())
		return false;

	const VDataBlock * data_block=remote.GetBlock(DHT_HISTORY);
	if (data_block == NULL) return false;

	//�������� ���
	unsigned char * ucHistory=(unsigned char*)data_block->Data;
	//������
	for (size_t i=0; i<data_block->Header->RecCount; ++i)
	{
		VOperationRecord newOperationRecord;
		newOperationRecord.LoadFromMemory(ucHistory);
		ucHistory+=newOperationRecord.GetTotalSize();
		m_Operations.push_back(newOperationRecord);
	}	
	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	remote.FreeBlock(DHT_HISTORY);
	return true;	
}

/** ���������� ������ � �������� �����*/
bool VHistory::Save(VMainFile & file)
{	
	VBasicParameter data;
	data.Create(DHT_HISTORY, VCT_USER, 1, this->GetTotalSize(), 1);

	size_t position = 0;
	for (size_t i = 0; i < this->GetCount(); ++i)
	{
		this->m_Operations[i].SaveToMemory(data.GetPtr(position));
		position += this->m_Operations[i].GetTotalSize();
	}

	return data.Save(file, this->GetCount(), 0);
}

/** ���������� ���������� ���������*/
size_t VHistory::GetCount() const
{
	return m_Operations.size();
}

/** ���������� ������ ���� �������*/
size_t VHistory::GetTotalSize() const
{
	size_t result = 0;
	for (unsigned int uiNdx=0; uiNdx < this->m_Operations.size(); uiNdx++)
	{
		result += this->m_Operations[uiNdx].GetTotalSize();
	}
	return result;
}

/** ������� �������*/
bool VHistory::RemoveAt(size_t index)
{
	try
	{
		m_Operations.erase(m_Operations.begin()+index);
	}
	catch (...)
	{
		return false;
	}
	return true;
}

/** ���������� ��������� �� �������*/
const VOperationRecord & VHistory::GetItem(size_t index) const
{
	return m_Operations.at(index);
}

/** ���������� ��������� �� �������*/
VOperationRecord & VHistory::GetItem(size_t index)
{
	return m_Operations.at(index);
}

/** ��������� �������*/
void VHistory::Add(const VOperationRecord & operationRecord)
{
	m_Operations.push_back(operationRecord);
	m_Operations.back().operation_time = VTime(time(NULL));
}

/** ��������� ���� �� ���� ���� ������ �� ������������ ��������*/
bool VHistory::OperationExists(unsigned int uiCode)
{
	for (unsigned int uiNdx=0; uiNdx<GetCount(); uiNdx++)
	{
		if (uiCode == this->GetItem(uiNdx).GetCode())
			return true;
	}
	return false;
}

std::string VHistory::ExtractCreatorVersion() const
{
	for (size_t i = 0; i < this->m_Operations.size(); ++i)
	{
		if (this->m_Operations[i].GetCode() == OC_CREATION)
		{
			std::string full_record = this->m_Operations[i].GetTextDescription();

			// ������ ������ - �� 2 ������
			if (strncmp(full_record.c_str(), "���� ������ � ", 14) == 0)
			{
				char time[256] = {0}, version[256] = {0}, db[1024] = {0};
				int fieldCount = _snscanf_s(full_record.c_str(), full_record.length(),
					"���� ������ � %s rt-%s ��: %s",
					time, 255, version, 255, db, 1024);

				if (fieldCount == 3) 
				{
					return version;
				}
				
				else if (fieldCount == 2)
				{
					int fieldCount = _snscanf_s(full_record.c_str(), full_record.length(),
						"���� ������ � %s rt-%s /debug/ ��: %s",
						time, 255, version, 255, db, 1024);

					if (fieldCount == 3) 
					{
						return std::string(version) + " /�������/";
					}

					else
					{
						int fieldCount = _snscanf_s(full_record.c_str(), full_record.length(),
							"���� ������ � %s rt-%s %s",
							time, 255, version, 255, db, 1024);

						if (fieldCount == 3 && std::string(db) == "/debug/")
						{
							return std::string(version) + " /�������/";
						}
						else if (fieldCount == 2)
						{
							return std::string(version);
						}

						else
						{
							LOG_ERROR(L"����������� ������ ������ � �������: " +
								string_to_wstring(full_record));
						}
					}
				}
			}

			// ����� ������ - �� 3 ������
			else if (strncmp(full_record.c_str(), "���� ������ ", 12) == 0)
			{
				char date[256] = {0}, time[256] = {0}, version[256] = {0};
				int fieldCount = _snscanf_s(full_record.c_str(), full_record.length(),
					"���� ������ %s %s ���������� �������� ����� ������ %s",
					date, 255, time, 255, version, 255);

				if (fieldCount == 3) return version;
			}
		}
	}
	return "";
}
