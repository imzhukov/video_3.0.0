#include "VRemoteFile.h"
#include "strlib.h"
#include "VParameter.h"

void VRemoteFile::DataBlockInfrastructure::Clear()
{
	if (this->data != NULL)	
	{
		::GlobalFree(this->data);
		this->data = NULL;
	}
	::ZeroMemory(&this->block, sizeof(this->block));
	this->size = 0;
}

void VRemoteFile::DataBlockInfrastructure::SetData(const void * ptr, size_t s)
{
	this->Clear();
	if (ptr == NULL || s == 0) return;
	this->data = (uint8_t*) ::GlobalAlloc(GPTR, s);
	::CopyMemory(this->data, ptr, s);
	this->size = s;
	this->block.Header = (VDataBlockHeader*) this->data;
	this->block.Data = this->data + sizeof(VDataBlockHeader);
}

bool VRemoteFile::Open(VIBDatabase & db, const VFileName & fn)
{
	this->Close();

	this->fileName = fn.GetShortName();
	if (!db.Connected())
	{
		this->dataBase = NULL;
		this->message = ERRMSG_DB_NO_CONNECTION;
		return false;
	}

	VIBTransaction trans = db.StartTransaction();
	VIBDataHandlerParameter handler;
	std::stringstream sql;
	sql << "select file_validate(path || '\\" << fn.GetShortName() <<"') from srv_local_folder;";
	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch (VException& e)
	{
		std::wstringstream msg;
		msg << e.GetDescription() << L" " << e.GetDetails() << L" " << e.GetDebug();		
		this->message = msg.str();
		db.RollbackTransaction(trans);
		return false;
	}
	if (handler.GetRowCount() != 1)
	{
		this->message = L"Пустой набор данных";
		db.RollbackTransaction(trans);
		return false;
	}
	if (handler.GetParameter(0, 0).GetInteger() == 0)
	{
		this->message = L"Файл не доступен";
		db.RollbackTransaction(trans);
		return false;
	}

	sql.str("");
	sql << "select get_user_header(path || '\\" << fn.GetShortName() <<"') from srv_local_folder;";
	try
	{
		db.ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch (VException& e)
	{
		std::wstringstream msg;
		msg << e.GetDescription() << L" " << e.GetDetails() << L" " << e.GetDebug();		
		this->message = msg.str();
		db.RollbackTransaction(trans);
		return false;
	}
	if (handler.GetRowCount() != 1)
	{
		this->message = L"Пустой набор данных";
		db.RollbackTransaction(trans);
		return false;
	}
	ISC_QUAD blob_id;
	memcpy(&blob_id, handler.GetParameter(0, 0).SqlVar()->sqldata, handler.GetParameter(0, 0).SqlVar()->sqllen);
	VBlob blob;
	db.ReadBLOB(trans, blob_id, &blob);
	this->header = *((VMainFileHeader*) blob.GetData());

	db.CommitTransaction(trans);
	this->dataBase = &db;
	this->message = ERRMSG_OK;
	return true;
}

void VRemoteFile::Close()
{
	this->fileName = L"";
	this->message = ERRMSG_DATA_NOT_LOADED;
	this->dataBase = NULL;
	::ZeroMemory(&this->header, sizeof(this->header));
}

bool VRemoteFile::Loaded() const
{
	return this->dataBase != NULL;
}

const VMainFileHeader * VRemoteFile::GetUserHeader() const
{
	return &this->header;
}

const VDataBlock * VRemoteFile::GetBlock(uint32_t type)
{
	if (this->blocks[type].Loaded())
	{
		return this->blocks[type].GetBlock();
	}

	if (!this->Loaded()) return NULL;

	VIBTransaction trans = this->dataBase->StartTransaction();
	VIBDataHandlerParameter handler;
	std::stringstream sql;
	sql << "select get_data_block(path || '\\" << this->fileName <<"', " << type << ", 0) ";
	sql << "from srv_local_folder;";
	try
	{
		this->dataBase->ExecuteStatement(trans, sql.str().c_str(), handler);
	}
	catch (VException& e)
	{
		std::wstringstream msg;
		msg << e.GetDescription() << L" " << e.GetDetails() << L" " << e.GetDebug();
		this->message = msg.str();
		this->dataBase->RollbackTransaction(trans);
		return NULL;
	}
	if (handler.GetRowCount() != 1)
	{
		this->message = L"Пустой набор данных";
		this->dataBase->RollbackTransaction(trans);
		return NULL;
	}
	ISC_QUAD blob_id;
	memcpy(&blob_id, handler.GetParameter(0, 0).SqlVar()->sqldata, handler.GetParameter(0, 0).SqlVar()->sqllen);
	VBlob blob;
	this->dataBase->ReadBLOB(trans, blob_id, &blob);

	if (blob.GetData() == NULL || blob.GetSize() == 0)
	{
		this->message = L"Блок отсутствует";
		this->dataBase->RollbackTransaction(trans);
		return NULL;
	}

	this->blocks[type].SetData(blob.GetData(), blob.GetSize());
	this->dataBase->RollbackTransaction(trans);
	return this->blocks[type].GetBlock();	
}

void VRemoteFile::FreeBlock(uint32_t type)
{
	if (this->blocks[type].Loaded())
	{
		return this->blocks[type].Clear();
	}
}
