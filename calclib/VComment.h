#pragma once

#include "VParameter.h"
#include "VRemoteFile.h"

class VComment
{
private:

	char * comment;
	std::wstring message;

public:

	/// Конструктор
	VComment() : comment(NULL), message(ERRMSG_DATA_NOT_LOADED)
	{}

	/// Копирующий конструктор
	VComment(const VComment & src) : comment(NULL), message(ERRMSG_DATA_NOT_LOADED)
	{
		this->SetComment(src.GetComment());
		this->message = src.message;
	}

	/// Оператор присваивания
	VComment & operator = (const VComment & src) 
	{
		this->Unload();
		this->SetComment(src.GetComment());
		this->message = src.message;
		return *this;
	}

	/// Деструктор
	~VComment()
	{
		this->Unload();
	}

	/// Выгрузить комментарий
	void Unload();

	/// Загружено
	bool IsLoaded()
	{
		return this->comment != NULL;
	}

	/// Загрузить
	template <class FILETYPE>
	bool Load(FILETYPE & file);

	/// Сохранить в файл
	bool Save(VMainFile & file);

	/// Установить комментарий указанной длины
	void SetComment(const char * src, size_t len);

	/// Установить комментарий
	void SetComment(const char * src)
	{
		return this->SetComment(src, strlen(src));
	}

	/// Вернуть комментарий
	const char * GetComment() const
	{
		return this->comment;
	}

	/// Вернуть сообщение об ошибке
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
