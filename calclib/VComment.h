#pragma once

#include "VParameter.h"
#include "VRemoteFile.h"

class VComment
{
private:

	char * comment;
	std::wstring message;

public:

	/// �����������
	VComment() : comment(NULL), message(ERRMSG_DATA_NOT_LOADED)
	{}

	/// ���������� �����������
	VComment(const VComment & src) : comment(NULL), message(ERRMSG_DATA_NOT_LOADED)
	{
		this->SetComment(src.GetComment());
		this->message = src.message;
	}

	/// �������� ������������
	VComment & operator = (const VComment & src) 
	{
		this->Unload();
		this->SetComment(src.GetComment());
		this->message = src.message;
		return *this;
	}

	/// ����������
	~VComment()
	{
		this->Unload();
	}

	/// ��������� �����������
	void Unload();

	/// ���������
	bool IsLoaded()
	{
		return this->comment != NULL;
	}

	/// ���������
	template <class FILETYPE>
	bool Load(FILETYPE & file);

	/// ��������� � ����
	bool Save(VMainFile & file);

	/// ���������� ����������� ��������� �����
	void SetComment(const char * src, size_t len);

	/// ���������� �����������
	void SetComment(const char * src)
	{
		return this->SetComment(src, strlen(src));
	}

	/// ������� �����������
	const char * GetComment() const
	{
		return this->comment;
	}

	/// ������� ��������� �� ������
	const std::wstring & GetErrorMessage() const
	{
		return this->message;
	}

};

template <class FILETYPE>
bool VComment::Load(FILETYPE & file)
{
	if (this->IsLoaded()) return true;

	VBasicParameter data;
	if (!data.Load(file, DHT_COMMENT, false))
	{
		this->message = data.GetErrorMessage();
		return false;
	}

	this->SetComment((const char*) data.GetPtr(0), data.GetDataSize());
	data.UnloadFile(file, DHT_COMMENT);
	data.Unload();
	this->message = ERRMSG_OK;
	return true;
}
