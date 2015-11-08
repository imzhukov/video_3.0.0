#include "VFileName.h"

#include "timelib.h"

#include <tchar.h>
#include <userenv.h>

#include <boost/format.hpp>

static void __SanitizeChars(wchar_t & ch)
{
	if (ch == L'/')	
	{
		ch = L'\\';
	}

	else if (ch == L':' || ch == L'*' || ch == L'?' || 
		ch == L'\"' || ch == L'<' || ch == L'>' || ch == L'|')
	{
		ch = L'_';
	}
}

void VFileName::SanitizeShortName()
{
	if (this->shname.empty()) return;

	for (size_t i = 0; i < this->shname.length(); ++i)
	{
		__SanitizeChars(this->shname[i]);
	}
}

void VFileName::SanitizeFolderName()
{
	if (this->folder.empty()) return;

	size_t check_start = 0;

	// полный путь - начинается как D:/ или D:\ или d:\ или d:/
	if (this->folder.length() > 2 && 
		((this->folder[0] >= L'A' && this->folder[0] <= L'Z') || (this->folder[0] >= L'a' && this->folder[0] <= L'z')) &&
		this->folder[1] == L':' &&
		(this->folder[2] == L'/' || this->folder[2] == L'\\'))
	{
		// проверяем начиная с первого слэша
		check_start = 2;
	}

	for (size_t i = check_start; i < this->folder.length(); ++i)
	{
		__SanitizeChars(this->folder[i]);
	}

	if (this->folder[this->folder.length() - 1] != L'\\')
		this->folder.push_back(L'\\');
}

std::wstring VFileName::GetHomeDir()
{
	wchar_t homeDirBuf[MAX_PATH] = {0};
	HANDLE token = 0;
	OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token);
	DWORD bufSize = MAX_PATH;
    GetUserProfileDirectory(token, homeDirBuf, &bufSize);
	CloseHandle(token);
	return std::wstring(homeDirBuf);
}

std::wstring VFileName::GetPostfix(time_t t)
{
	static const wchar_t l[] = L"0123456789abcdefghijklmnopqrstuvwxyz";
	static const time_t n[] = {60466176,1679616,46656,1296,36};

	std::wstring result;
	result.push_back(l[t/n[0]]);
	result.push_back(l[t%n[0]/n[1]]);
	result.push_back(l[t%n[0]%n[1]/n[2]]);
	result.push_back(l[t%n[0]%n[1]%n[2]/n[3]]);
	result.push_back(l[t%n[0]%n[1]%n[2]%n[3]/n[4]]);
	result.push_back(l[t%n[0]%n[1]%n[2]%n[3]%n[4]]);

	return result;
}

std::wstring VFileName::GetSpoolerUniqueDir(long dir, const std::wstring & way)
{
	VTime tim(time(NULL));
	return boost::str(boost::wformat(L"%04i.%02i.%02i %02i.%02i.%02i %i-%s") 
		% tim.wYear % tim.wMonth % tim.wDay 
		% tim.wHour % tim.wMinute % tim.wSecond
		% dir % way.c_str());
}

bool VFileName::EnsureFolderExists(const std::wstring & folder) 
{
	return ::CreateDirectory(folder.c_str(), NULL) == TRUE;
}

void VFileName::Clear()
{
	this->folder = VFileName::GetHomeDir();
	this->SanitizeFolderName();
	this->shname = L"";
}

void VFileName::SetFolder(const std::wstring & __folder)
{
	this->folder = __folder;
	this->SanitizeFolderName();
}

void VFileName::SetShortName(const std::wstring & __shname)
{
	this->shname = __shname;
	this->SanitizeShortName();
}

void VFileName::SetPath(const std::wstring & __path)
{
	size_t lastSlashPos = __path.find_last_of(L'\\');

	if (lastSlashPos == std::wstring::npos)
	{
		this->SetShortName(__path);
		this->SetFolder(L"");
	}
	else
	{
		this->SetShortName(__path.substr(lastSlashPos+1));
		this->SetFolder(__path.substr(0, lastSlashPos+1));
	}
}	
 
bool VFileName::GetFileInfo(VFileName::FileInfo * fi, const VFileName & fn)
{
	_ULARGE_INTEGER tmp;
	_FILETIME ct = {0}, mt = {0};

	HANDLE h = ::CreateFile(fn.GetPath().c_str(), GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);

	if (h == INVALID_HANDLE_VALUE) 
	{
		if (fi != NULL)
		{
			fi->ctime = -1;
			fi->mtime = -1;
			fi->size = -1;
		}
		return false;
	}

	if (fi != NULL)
	{
		tmp.LowPart = ::GetFileSize(h, &tmp.HighPart);
		fi->size = tmp.QuadPart;

		::GetFileTime(h, &ct, NULL, &mt);		

		tmp.HighPart = ct.dwHighDateTime;
		tmp.LowPart = ct.dwLowDateTime;
		fi->ctime = (tmp.QuadPart / 10000000) - 0x2b6109100;

		tmp.HighPart = mt.dwHighDateTime;
		tmp.LowPart = mt.dwLowDateTime;
		fi->mtime = (tmp.QuadPart / 10000000) - 0x2b6109100;
	}
	::CloseHandle(h);
	return true;
}

bool VFileName::SetFileTime(__int64 ctime, __int64 mtime, const VFileName & fn)
{
	_ULARGE_INTEGER tmp;
	_FILETIME ct = {0}, mt = {0};

	HANDLE h = ::CreateFile(fn.GetPath().c_str(), GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);

	if (h == INVALID_HANDLE_VALUE) 
	{
		return false;
	}

	tmp.QuadPart = (ctime + 0x2b6109100) * 10000000;
	ct.dwHighDateTime = tmp.HighPart;
	ct.dwLowDateTime = tmp.LowPart;

	tmp.QuadPart = (mtime + 0x2b6109100) * 10000000;
	mt.dwHighDateTime = tmp.HighPart;
	mt.dwLowDateTime = tmp.LowPart;

	::SetFileTime(h, &ct, NULL, &mt);
	::CloseHandle(h);
	return true;
}

bool VFileName::operator < (const VFileName & other) const
{
	if (this->GetFolder() == other.GetFolder())
	{
		return this->GetShortName() < other.GetShortName();
	}
	else
	{
		return this->GetFolder() < other.GetFolder();
	}
}

bool VFileName::operator == (const VFileName & other) const
{
	return this->GetFolder() == other.GetFolder() && this->GetShortName() == other.GetShortName();
}

bool VFileName::operator != (const VFileName & other) const
{
	return !(*this == other);
}

