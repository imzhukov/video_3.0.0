#pragma once

#include "dblib.h"
#include "VMainFile.h"

#include <map>

class VRemoteFile
{
	std::wstring fileName;
	std::wstring message;
	VIBDatabase * dataBase;
	VMainFileHeader header;

	class DataBlockInfrastructure
	{
	private:

		VDataBlock block;
		unsigned char * data;
		size_t size;

	public:

		DataBlockInfrastructure() : data(NULL)
		{
			this->Clear();
		}

		DataBlockInfrastructure(const DataBlockInfrastructure & src) : data(NULL)
		{
			this->Clear();
			this->SetData(src.data, src.size);
		}

		void SetData(const void * ptr, size_t s);

		const VDataBlock * GetBlock() const
		{
			return &this->block;
		}

		bool Loaded() const
		{
			return this->data != NULL;
		}

		void Clear();
	};

	std::map<uint32_t, DataBlockInfrastructure> blocks;
//	DataBlockInfrastructure block;

	VRemoteFile(const VRemoteFile & src);
	VRemoteFile & operator = (const VRemoteFile & src);

public:

	VRemoteFile()
	{
		this->Close();
	}

	~VRemoteFile()
	{
		this->Close();
	}

	void Close();
	bool Open(VIBDatabase & db, const VFileName & fn);
	bool Loaded() const;
	const VMainFileHeader * GetUserHeader() const;

	const std::wstring & GetErrorMessage() const
	{
		return this->message;
	}

	const wchar_t * GetFileName() const
	{
		return this->fileName.c_str();
	}

	const VDataBlock * GetBlock(uint32_t type);
	void FreeBlock(uint32_t type);
};
