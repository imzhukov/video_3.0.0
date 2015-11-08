#include "VMemoryMappedFile.h"
#include "VMemoryMappedFile.h"

//-------------------------- ������ ������ VMemoryMappedFile ------------------------------------------------

/** ���������� WIN32*/
#ifdef _WIN32

/** �����������*/
VMemoryMappedFile::VMemoryMappedFile()
{
	File=NULL;
	MappedFile=NULL;
	FileSize=0;
	OpenMode=0;
	Loaded=false;
	//���������� ������������� ��������� ������
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	Granularity=si.dwAllocationGranularity;
}

/** ����������*/
VMemoryMappedFile::~VMemoryMappedFile()
{
	try
	{
		//��������� ����
		Close();
	}
	catch(...)
	{
		//
	}
}

/** ������� ��������*/
bool VMemoryMappedFile::CreateMapping(const size_t size)
{
	DWORD flProtect;
	switch (OpenMode)
	{
		//���� ��������� ������ ��� ������
		case OPEN_MODE_READ:
			flProtect=PAGE_READONLY;
			break;
		case OPEN_MODE_WRITE:
		case OPEN_MODE_CREATE:
			flProtect=PAGE_READWRITE;
			break;
		default:
			break;
	}
	MappedFile = CreateFileMapping(File, NULL,
		flProtect, 0, static_cast<DWORD>(size), NULL);
	if(MappedFile == INVALID_HANDLE_VALUE)
	{
		MappedFile = NULL;
		return false;
	}
	return true;
}

/** ���������� ��������� ������ ���� ��������, � ������ ������������� ��������� ������*/
size_t VMemoryMappedFile::AlignSizeToGranularity(const size_t req)
{
	size_t result=req;
	result=(static_cast<size_t>(req/Granularity))*Granularity;
	return result;
}

/** ��������� ����*/
bool VMemoryMappedFile::Open(const wchar_t * const filename, unsigned long mode)
{
	Close();
	size_t req_size=(wcslen(filename)+1)*sizeof(wchar_t);	
	FileName=filename;
	OpenMode=mode;
	//��������� ����
	DWORD dwAccess;
	DWORD dwShareMode;
	DWORD dwCreationDisposition;
	//� ����������� �� ������, � ������� ��������� ����
	switch (OpenMode)
	{
		//���� ��������� ������ ��� ������
		case OPEN_MODE_READ:
			//��� ��� ���� �������� ������ ��� ������
			dwAccess = GENERIC_READ;
			//��� ���� ��������� �������� ��� ������
			dwShareMode = FILE_SHARE_READ;
			//���� ����������� ������ ������������
			dwCreationDisposition = OPEN_EXISTING;
			break;
		case OPEN_MODE_WRITE:
			//��� ��� ���� �������� ��� ������ � ������
			dwAccess = GENERIC_READ | GENERIC_WRITE;
			//��� ���� ��������� �� ��������
			dwShareMode = 0;
			//���� ����������� ������ ������������
			dwCreationDisposition = OPEN_EXISTING;
			break;
		case OPEN_MODE_CREATE:
			//��� ��� ���� �������� ��� ������ � ������
			dwAccess = GENERIC_READ | GENERIC_WRITE;
			//��� ���� ��������� �� ��������
			dwShareMode = 0;
			//���� ���� ���������� �� ����� �����������
			dwCreationDisposition = CREATE_ALWAYS;
			break;
		default:
			return false; 
			break;
	}
	//��������� ����
	File = CreateFile(FileName.c_str(), dwAccess, dwShareMode, NULL,
		dwCreationDisposition, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	//��������� �������� �� ����
	if(File == INVALID_HANDLE_VALUE)
	{
		File = NULL;
		return false;
	}
	//���������� ������ �����
	FileSize=::GetFileSize(File, NULL);
	//������� ��������
	if (!CreateMapping(FileSize))
	{
		if (!CloseHandle(File))
		{
			return false;
		}
		Loaded=false;
		return false;
	}
	Loaded=true;
	return true;
}

/** ��������� ����*/
bool VMemoryMappedFile::Close()
{
	bool result=true;
	if (Loaded)
	{		
		if (MappedFile!=NULL && !CloseHandle(MappedFile))
			result=false;
		if (File!=NULL && !CloseHandle(File))
			result=false;
		//�������� ����
		File = NULL;
		MappedFile = NULL;
		Loaded=false;
	}
	return result;
}

/** ������������� ������ �����*/
bool VMemoryMappedFile::SetFileSize(const size_t size)
{
	//��������� �������� �� ����
	if (!IsLoaded())
	{
		return false;
	}
	if (MappedFile!=NULL && !CloseHandle(MappedFile))
	{
		return false;
	}
	//��������� ����� ������ �����
	size_t old_size=FileSize;
	FileSize=size;
	//������������� ������
	if (SetFilePointer(File, static_cast<LONG>(FileSize), NULL, FILE_BEGIN)==INVALID_SET_FILE_POINTER)
	{
		return false;
	}
	if (!SetEndOfFile(File))
	{
		return false;
	}
	//������� ��������
	if (!CreateMapping(FileSize))
	{
		return false;
	}
	return true;
}

/** ���������� ������ �����*/
size_t VMemoryMappedFile::GetFileSize()
{
	//��������� �������� �� ����
	if (!IsLoaded())
	{
		return 0;
	}
	return FileSize;
}

/** ���������� �������������*/
size_t VMemoryMappedFile::GetGranularity()
{
	return Granularity;
}

/** ������� ����� �����*/
bool VMemoryMappedFile::DeletePart(const size_t StartOffset, const size_t EndOffset)
{
	//��������� �������� �� ����
	if (!IsLoaded())
	{
		return false;
	}
	size_t dwTrgOffset=StartOffset;
	size_t dwSrcOffset=EndOffset;
	if (dwTrgOffset<dwSrcOffset && dwSrcOffset<=FileSize)
	{
		size_t dwTrgMap=AlignSizeToGranularity(dwTrgOffset);
		size_t dwSrcMap=AlignSizeToGranularity(dwSrcOffset);
		do
		{
			//���������� ������ ����������� �����
			size_t dwSize=std::min<size_t>(Granularity, FileSize-dwSrcOffset);
			//���������� �����
			LPVOID lpTrgAddress=MapView(0 ,dwTrgMap, (dwTrgOffset-dwTrgMap)+dwSize);
			LPVOID lpSrcAddress=MapView(0 ,dwSrcMap, (dwSrcOffset-dwSrcMap)+dwSize);
			if (lpTrgAddress==NULL || lpSrcAddress==NULL)
			{
				return false;
			}
			//���������� ����������� ������ ������ ������
			LPBYTE lpTrgData=(LPBYTE)lpTrgAddress+(dwTrgOffset-dwTrgMap);
			LPBYTE lpSrcData=(LPBYTE)lpSrcAddress+(dwSrcOffset-dwSrcMap);
			memmove(lpTrgData, lpSrcData, dwSize);
			if (!UnmapView(lpTrgAddress))
			{
				return false;
			}
			if (!UnmapView(lpSrcAddress))
			{
				return false;
			}
			//��������� � �����
			dwTrgOffset+=dwSize;
			dwSrcOffset+=dwSize;
		}
		while (dwSrcOffset<FileSize);
	}
	else
	{
		return false;
	}
	//�������� ������ �����
	if (!SetFileSize(FileSize-(dwSrcOffset-dwTrgOffset)))
	{
		return false;
	}
	return true;
}

/** ��������� ����� � ����*/
bool VMemoryMappedFile::InsertPart(const size_t StartOffset, const size_t PartSize)
{
	//��������� �������� �� ����
	if (!IsLoaded())
	{
		return false;
	}
	//�������� ������ �����
	if (!SetFileSize(FileSize+PartSize))
	{
		return false;
	}
	//���������� ������ ����� ��� �����������
	size_t dwSize=std::min<size_t>(Granularity, PartSize);
	size_t dwTrgOffset=FileSize;
	size_t dwSrcOffset=FileSize-PartSize;
	do
	{
		dwSize=std::min<size_t>(dwSize, dwSrcOffset-StartOffset);
		dwTrgOffset-=dwSize;
		dwSrcOffset-=dwSize;
		//���������� �����
		size_t dwTrgMap=AlignSizeToGranularity(dwTrgOffset);
		size_t dwSrcMap=AlignSizeToGranularity(dwSrcOffset);
		LPVOID lpTrgAddress=MapView(0 ,dwTrgMap, (dwTrgOffset-dwTrgMap)+dwSize);
		LPVOID lpSrcAddress=MapView(0 ,dwSrcMap, (dwSrcOffset-dwSrcMap)+dwSize);
		if (lpTrgAddress==NULL || lpSrcAddress==NULL)
		{
			return false;
		}
		//���������� ����������� ������ ������ ������
		LPBYTE lpTrgData=(LPBYTE)lpTrgAddress+(dwTrgOffset-dwTrgMap);
		LPBYTE lpSrcData=(LPBYTE)lpSrcAddress+(dwSrcOffset-dwSrcMap);
		memmove(lpTrgData, lpSrcData, dwSize);
		memset(lpSrcData, 0, dwSize);
		if (!UnmapView(lpTrgAddress))
		{
			return false;
		}
		if (!UnmapView(lpSrcAddress))
		{
			return false;
		}
	}
	while(dwSrcOffset>StartOffset);
	return true;
}

/** ���������� ����� �����*/
LPVOID VMemoryMappedFile::MapView(const size_t FileOffsetHigh, const size_t FileOffsetLow, const size_t NumberOfBytesToMap)
{
	//��������� �������� �� ����
	if (!IsLoaded())
	{
		return NULL;
	}
	LPVOID lpData=NULL;
	DWORD dwDesiredAccess;
	switch (OpenMode)
	{
		//���� ��������� ������ ��� ������
		case OPEN_MODE_READ:
			dwDesiredAccess=FILE_MAP_READ;
			break;
		case OPEN_MODE_WRITE:
		case OPEN_MODE_CREATE:
			dwDesiredAccess=FILE_MAP_WRITE;
			break;
		default:
			break;
	}
	lpData = MapViewOfFile(MappedFile,
		dwDesiredAccess, static_cast<DWORD>(FileOffsetHigh),
		static_cast<DWORD>(FileOffsetLow), static_cast<DWORD>(NumberOfBytesToMap));
	if(lpData == NULL)
	{
		return lpData;
	}
	return lpData;
}

/** ������� ��������*/
bool VMemoryMappedFile::UnmapView(LPVOID address)
{
	return UnmapViewOfFile(address)==TRUE;
}

/** ���������� ������� ������������� �����*/
bool VMemoryMappedFile::IsLoaded()
{
	return Loaded;
}

/** ���������� ����� �������� �����*/
unsigned long VMemoryMappedFile::GetOpenMode()
{
	return OpenMode;
}

/** ���������� ��� ������������ �����*/
const wchar_t * const VMemoryMappedFile::GetFileName()
{
	return FileName.c_str();
}

#else //���������� Linux

/** �����������*/
VMemoryMappedFile::VMemoryMappedFile()
{
	//���������� ������������� ��������� ������
	Granularity=getpagesize();
	Loaded=false;
}

/** ����������*/
VMemoryMappedFile::~VMemoryMappedFile()
{
	try
	{
		Close();
	}
	catch(...)
	{
	}
}

/** ��������� ����*/
bool VMemoryMappedFile::Open(const wchar_t * const filename, unsigned long mode=OPEN_MODE_READ)
{
	Close();	
	FileName=filename;
	OpenMode=mode;
	int flags;
	switch (OpenMode)
	{
		//���� ��������� ������ ��� ������
		case OPEN_MODE_READ:
			flags=O_RDONLY;
			break;
		case OPEN_MODE_WRITE:
			flags=O_RDWR;
			break;
		case OPEN_MODE_CREATE:
			flags=O_CREAT|O_RDWR;
			break;
		default:
			flags=O_RDONLY;
			break;
	}	
	//��������� ����
	File=open(wstring_to_string(FileName).c_str(), flags);
	free(fn);
	if (File>=0)
	{
	    //�������� ������ �����
        struct stat infobuf;
        if (fstat(File, &infobuf)==-1)
            return -1;
        FileSize=infobuf.st_size;
		Loaded=true;
		return true;
	}
	else
	{
		Loaded=false;
		return false;
	}
}

/** ��������� ����*/
bool VMemoryMappedFile::Close()
{
	bool result=true;
	if (Loaded)
	{
		//����������� ������		
		close(File);
		Loaded=false;
	}
	return result;
}

/** ������������� ������ �����*/
bool VMemoryMappedFile::SetFileSize(const size_t size)
{
	//��������� �������� �� ����
	if (!IsLoaded())
	{
		return false;
	}
	//��������� ����� ������ �����
	FileSize=size;
	//������������� ������
	if (ftruncate(File, size)==0)
        return true;
    else
        return false;
}

/** ���������� ������ �����*/
size_t VMemoryMappedFile::GetFileSize()
{
	//��������� �������� �� ����
	if (!IsLoaded())
	{
		return 0;
	}
	return FileSize;
}
/** ���������� �������������*/
size_t VMemoryMappedFile::GetGranularity()
{
	return Granularity;
}
/** ������� ����� �����*/
bool VMemoryMappedFile::DeletePart(const size_t StartOffset, const size_t EndOffset)
{
	//��������� �������� �� ����
	if (!IsLoaded())
	{
		return false;
	}
	size_t dwTrgOffset=StartOffset;
	size_t dwSrcOffset=EndOffset;
	if (dwTrgOffset<dwSrcOffset && dwSrcOffset<=FileSize)
	{
		size_t dwTrgMap=AlignSizeToGranularity(dwTrgOffset);
		size_t dwSrcMap=AlignSizeToGranularity(dwSrcOffset);
		do
		{
			//���������� ������ ����������� �����
			size_t dwSize=(Granularity<FileSize-dwSrcOffset) ? Granularity : FileSize-dwSrcOffset;
			//���������� �����
			void * lpTrgAddress=MapView(0 ,dwTrgMap, (dwTrgOffset-dwTrgMap)+dwSize);
			void * lpSrcAddress=MapView(0 ,dwSrcMap, (dwSrcOffset-dwSrcMap)+dwSize);
			if (lpTrgAddress==NULL || lpSrcAddress==NULL)
			{
				return false;
			}
			//���������� ����������� ������ ������ ������
			unsigned char * lpTrgData=(unsigned char*)lpTrgAddress+(dwTrgOffset-dwTrgMap);
			unsigned char * lpSrcData=(unsigned char*)lpSrcAddress+(dwSrcOffset-dwSrcMap);
			memmove(lpTrgData, lpSrcData, dwSize);
			if (!UnmapView(lpTrgAddress))
			{
				return false;
			}
			if (!UnmapView(lpSrcAddress))
			{
				return false;
			}
			//��������� � �����
			dwTrgOffset+=dwSize;
			dwSrcOffset+=dwSize;
		}
		while (dwSrcOffset<FileSize);
	}
	else
	{
		return false;
	}
	//�������� ������ �����
	if (!SetFileSize(FileSize-(dwSrcOffset-dwTrgOffset)))
	{
		return false;
	}
	return true;
}
/** ��������� ����� � ����*/
bool VMemoryMappedFile::InsertPart(const size_t StartOffset, const size_t PartSize)
{
	//��������� �������� �� ����
	if (!IsLoaded())
	{
		return false;
	}
	//�������� ������ �����
	if (!SetFileSize(FileSize+PartSize))
	{
		return false;
	}
	//���������� ������ ����� ��� �����������
	size_t dwSize=(Granularity<PartSize) ? Granularity : PartSize;
	size_t dwTrgOffset=FileSize;
	size_t dwSrcOffset=FileSize-PartSize;
	do
	{
		dwSize=(dwSize<dwSrcOffset-StartOffset) ? dwSize : dwSrcOffset-StartOffset;
		dwTrgOffset-=dwSize;
		dwSrcOffset-=dwSize;
		//���������� �����
		size_t dwTrgMap=AlignSizeToGranularity(dwTrgOffset);
		size_t dwSrcMap=AlignSizeToGranularity(dwSrcOffset);
		void * lpTrgAddress=MapView(0 ,dwTrgMap, (dwTrgOffset-dwTrgMap)+dwSize);
		void * lpSrcAddress=MapView(0 ,dwSrcMap, (dwSrcOffset-dwSrcMap)+dwSize);
		if (lpTrgAddress==NULL || lpSrcAddress==NULL)
		{
			return false;
		}
		//���������� ����������� ������ ������ ������
		unsigned char * lpTrgData=(unsigned char *)lpTrgAddress+(dwTrgOffset-dwTrgMap);
		unsigned char * lpSrcData=(unsigned char *)lpSrcAddress+(dwSrcOffset-dwSrcMap);
		memmove(lpTrgData, lpSrcData, dwSize);
		memset(lpSrcData, 0, dwSize);
		if (!UnmapView(lpTrgAddress))
		{
			return false;
		}
		if (!UnmapView(lpSrcAddress))
		{
			return false;
		}
	}
	while(dwSrcOffset>StartOffset);
	return true;
}
/** ���������� ����� �����*/
void * VMemoryMappedFile::MapView(const size_t FileOffsetHigh, const size_t FileOffsetLow, const size_t NumberOfBytesToMap)
{
	int prot;
	switch (OpenMode)
	{
		//���� ��������� ������ ��� ������
		case OPEN_MODE_READ:
			prot=PROT_READ;
			break;
		case OPEN_MODE_WRITE:
			prot=PROT_WRITE;
			break;
		case OPEN_MODE_CREATE:
			prot=PROT_WRITE;
			break;
		default:
			prot=PROT_READ;
			break;
	}
	return mmap(0, NumberOfBytesToMap, prot, MAP_SHARED, File, FileOffsetLow);
}
/** ������� ��������*/
bool VMemoryMappedFile::UnmapView(void * address)
{
	munmap(address, 0);
	return true;
}
/** ���������� ������� ������������� �����*/
bool VMemoryMappedFile::IsLoaded()
{
	return Loaded;
}

/** ���������� ����� �������� �����*/
unsigned long VMemoryMappedFile::GetOpenMode()
{
	return OpenMode;
}

/** ���������� ��� ������������ �����*/
const wchar_t * const VMemoryMappedFile::GetFileName()
{
	return FileName.c_str();
}
/** ���������� ��������� ������ ���� ��������, � ������ ������������� ��������� ������*/
size_t VMemoryMappedFile::AlignSizeToGranularity(const size_t req)
{
	size_t result=req;
	result=(static_cast<size_t>(req/Granularity))*Granularity;
	return result;
}

#endif //_WIN32
