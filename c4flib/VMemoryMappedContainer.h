#pragma once

#include "VFileName.h"
#include "VMemoryMappedFile.h"

template <class T>
class VMemoryMappedContainer
{

	VMemoryMappedFile file;
	const T * data;
	size_t count;

public:

	VMemoryMappedContainer() : data(NULL), count(0)
	{
	}

	bool Open(const VFileName & fn)
	{
		__int64 len = fn.GetFileSize();
		if (len < 0) return false;

		if (!this->file.Open(fn.GetPath().c_str())) return false;
		this->data = (const T*) this->file.MapView(0, 0, len);
		if (this->data == NULL) return false;
		this->count = len / sizeof(T);
		return true;
	}

	void Close()
	{
		this->file.UnmapView((void*) this->data);
		this->file.Close();
		this->data = NULL;
		this->count = 0;
	}

	bool IsOpen() const
	{
		return this->data != NULL;
	}

	size_t GetCount() const
	{
		return this->count;
	}

	const T & GetItem(size_t index) const
	{
		return this->data[index];
	}

};