#include "VComment.h"

// Выгрузить комментарий
void VComment::Unload()
{
	if (this->comment != NULL)
	{
		::GlobalFree(this->comment);
	}
	this->comment = NULL;
	this->message = ERRMSG_DATA_NOT_LOADED;
}

// Сохранить в файл
bool VComment::Save(VMainFile & file)
{
	size_t len = strlen(this->comment);

	VBasicParameter data;
	data.Create(DHT_COMMENT, VCT_CHAR, len, sizeof(char), 1);
	::CopyMemory(data.GetPtr(0), this->comment, len);
	if (data.Save(file, len, sizeof(char)))
	{
		this->message = ERRMSG_OK;
		return true;
	}
	else
	{
		this->message = ERRMSG_BLOCK_WRITE_FAILED;
		return false;
	}
}

// Установить комментарий
void VComment::SetComment(const char * src, size_t len)
{
	this->Unload();
	this->comment = (char*) ::GlobalAlloc(GPTR, len+1);
	::CopyMemory(this->comment, src, len);
}
