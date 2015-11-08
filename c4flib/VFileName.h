#pragma once

#include <string>

// путь к файлу
class VFileName
{
	std::wstring folder;
	std::wstring shname;

	void SanitizeFolderName();
	void SanitizeShortName();

public:

	VFileName()
	{
		this->Clear();
	}

	VFileName(const wchar_t * fileName)
	{
		this->Clear();
		this->SetPath(std::wstring(fileName));
	}

	VFileName(const std::wstring & fileName)
	{
		this->Clear();
		this->SetPath(fileName);
	}

	VFileName(const std::wstring & __folder, const std::wstring & __shname)
	{
		this->SetFolder(__folder);
		this->SetShortName(__shname);
	}

	static std::wstring GetHomeDir();

	static std::wstring GetPostfix(time_t t);

	static std::wstring GetSpoolerUniqueDir(long dir, const std::wstring & way);

	// убедиться в наличии папки, при необходимости создать её
	static bool EnsureFolderExists(const std::wstring & folder);

	void Clear();

	// изменить путь к файлу
	void SetFolder(const std::wstring & __folder);

	// вернуть путь к файлу
	const std::wstring & GetFolder() const
	{
		return this->folder;
	}

	// изменить короткое имя файла
	void SetShortName(const std::wstring & __shname);

	// вернуть короткое имя файла
	const std::wstring & GetShortName() const
	{
		return this->shname;
	}

	// расширение
	std::wstring GetExt() const
	{
		return this->shname.substr(this->shname.rfind('.'));
	}

	// изменить расширение
	void SetExt(const std::wstring & ext)
	{
		this->shname.replace(this->shname.rfind('.'), this->shname.length(), ext);
	}

	// имя не установлено
	bool IsEmpty() const
	{
		return this->shname.empty();
	}

	// изменить полный путь к файлу
	void SetPath(const std::wstring & __path);

	// вернуть полный путь к файлу
	std::wstring GetPath() const
	{
		return this->folder + this->shname;
	}

	// информация о файле
	struct FileInfo
	{
		__int64 ctime, mtime, size;
	};

	static bool GetFileInfo(VFileName::FileInfo * fi, const VFileName & fn);

	bool GetFileInfo(VFileName::FileInfo * fi) const
	{
		return VFileName::GetFileInfo(fi, *this);
	}

	static bool SetFileTime(__int64 ctime, __int64 mtime, const VFileName & fn);
	
	bool SetFileTime(__int64 ctime, __int64 mtime) const
	{
		return VFileName::SetFileTime(ctime, mtime, *this);
	}

	__int64 GetFileSize() const
	{
		VFileName::FileInfo fi;
		if (this->GetFileInfo(&fi))	return fi.size;
		else return -1;
	}

	bool Exists() const
	{
		return this->GetFileInfo(NULL);
	}

	// удалить файл
	bool Remove() const
	{
		return _wremove(this->GetPath().c_str()) == 0;
	}


	// операторы
	bool operator < (const VFileName & other) const;
	bool operator == (const VFileName & other) const;
	bool operator != (const VFileName & other) const;
};
