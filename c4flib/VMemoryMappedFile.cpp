#include "VMemoryMappedFile.h"
#include "VMemoryMappedFile.h"

//-------------------------- Методы класса VMemoryMappedFile ------------------------------------------------

/** Реализация WIN32*/
#ifdef _WIN32

/** Конструктор*/
VMemoryMappedFile::VMemoryMappedFile()
{
	File=NULL;
	MappedFile=NULL;
	FileSize=0;
	OpenMode=0;
	Loaded=false;
	//Определяем гранулярность выделения памяти
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	Granularity=si.dwAllocationGranularity;
}

/** Деструктор*/
VMemoryMappedFile::~VMemoryMappedFile()
{
	try
	{
		//Закрываем файл
		Close();
	}
	catch(...)
	{
		//
	}
}

/** Создает проекцию*/
bool VMemoryMappedFile::CreateMapping(const size_t size)
{
	DWORD flProtect;
	switch (OpenMode)
	{
		//Если открываем только для чтения
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

/** Возвращает требуемый размер окна проекции, с учетом гранулярности выделения памяти*/
size_t VMemoryMappedFile::AlignSizeToGranularity(const size_t req)
{
	size_t result=req;
	result=(static_cast<size_t>(req/Granularity))*Granularity;
	return result;
}

/** Открывает файл*/
bool VMemoryMappedFile::Open(const wchar_t * const filename, unsigned long mode)
{
	Close();
	size_t req_size=(wcslen(filename)+1)*sizeof(wchar_t);	
	FileName=filename;
	OpenMode=mode;
	//Открываем файл
	DWORD dwAccess;
	DWORD dwShareMode;
	DWORD dwCreationDisposition;
	//В зависимость от режима, в котором открываем файл
	switch (OpenMode)
	{
		//Если открываем только для чтения
		case OPEN_MODE_READ:
			//Для нас файл доступен только для чтения
			dwAccess = GENERIC_READ;
			//Для всех остальных доступен для чтения
			dwShareMode = FILE_SHARE_READ;
			//Файл обязательно должен существовать
			dwCreationDisposition = OPEN_EXISTING;
			break;
		case OPEN_MODE_WRITE:
			//Для нас файл доступен для чтения и записи
			dwAccess = GENERIC_READ | GENERIC_WRITE;
			//Для всех остальных не доступен
			dwShareMode = 0;
			//Файл обязательно должен существовать
			dwCreationDisposition = OPEN_EXISTING;
			break;
		case OPEN_MODE_CREATE:
			//Для нас файл доступен для чтения и записи
			dwAccess = GENERIC_READ | GENERIC_WRITE;
			//Для всех остальных не доступен
			dwShareMode = 0;
			//Если файл существует он будет перезаписан
			dwCreationDisposition = CREATE_ALWAYS;
			break;
		default:
			return false; 
			break;
	}
	//Открываем файл
	File = CreateFile(FileName.c_str(), dwAccess, dwShareMode, NULL,
		dwCreationDisposition, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	//Проверяем открылся ли файл
	if(File == INVALID_HANDLE_VALUE)
	{
		File = NULL;
		return false;
	}
	//Определяем размер файла
	FileSize=::GetFileSize(File, NULL);
	//Создаем проекцию
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

/** Закрывает файл*/
bool VMemoryMappedFile::Close()
{
	bool result=true;
	if (Loaded)
	{		
		if (MappedFile!=NULL && !CloseHandle(MappedFile))
			result=false;
		if (File!=NULL && !CloseHandle(File))
			result=false;
		//Обнуляем поля
		File = NULL;
		MappedFile = NULL;
		Loaded=false;
	}
	return result;
}

/** Устанавливает размер файла*/
bool VMemoryMappedFile::SetFileSize(const size_t size)
{
	//Проверяем загружен ли файл
	if (!IsLoaded())
	{
		return false;
	}
	if (MappedFile!=NULL && !CloseHandle(MappedFile))
	{
		return false;
	}
	//Вычисляем новый размер файла
	size_t old_size=FileSize;
	FileSize=size;
	//Устанавливаем размер
	if (SetFilePointer(File, static_cast<LONG>(FileSize), NULL, FILE_BEGIN)==INVALID_SET_FILE_POINTER)
	{
		return false;
	}
	if (!SetEndOfFile(File))
	{
		return false;
	}
	//Создаем проекцию
	if (!CreateMapping(FileSize))
	{
		return false;
	}
	return true;
}

/** Возвращает размер файла*/
size_t VMemoryMappedFile::GetFileSize()
{
	//Проверяем загружен ли файл
	if (!IsLoaded())
	{
		return 0;
	}
	return FileSize;
}

/** Возвращает гранулярность*/
size_t VMemoryMappedFile::GetGranularity()
{
	return Granularity;
}

/** Удаляет кусок файла*/
bool VMemoryMappedFile::DeletePart(const size_t StartOffset, const size_t EndOffset)
{
	//Проверяем загружен ли файл
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
			//Определяем размер копируемого блока
			size_t dwSize=std::min<size_t>(Granularity, FileSize-dwSrcOffset);
			//Проецируем блоки
			LPVOID lpTrgAddress=MapView(0 ,dwTrgMap, (dwTrgOffset-dwTrgMap)+dwSize);
			LPVOID lpSrcAddress=MapView(0 ,dwSrcMap, (dwSrcOffset-dwSrcMap)+dwSize);
			if (lpTrgAddress==NULL || lpSrcAddress==NULL)
			{
				return false;
			}
			//Определяем фактические адреса блоков данных
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
			//Смещаемся в файле
			dwTrgOffset+=dwSize;
			dwSrcOffset+=dwSize;
		}
		while (dwSrcOffset<FileSize);
	}
	else
	{
		return false;
	}
	//Изменяем размер файла
	if (!SetFileSize(FileSize-(dwSrcOffset-dwTrgOffset)))
	{
		return false;
	}
	return true;
}

/** Вставляет кусок в файл*/
bool VMemoryMappedFile::InsertPart(const size_t StartOffset, const size_t PartSize)
{
	//Проверяем загружен ли файл
	if (!IsLoaded())
	{
		return false;
	}
	//Изменяем размер файла
	if (!SetFileSize(FileSize+PartSize))
	{
		return false;
	}
	//Определяем размер блока для копирования
	size_t dwSize=std::min<size_t>(Granularity, PartSize);
	size_t dwTrgOffset=FileSize;
	size_t dwSrcOffset=FileSize-PartSize;
	do
	{
		dwSize=std::min<size_t>(dwSize, dwSrcOffset-StartOffset);
		dwTrgOffset-=dwSize;
		dwSrcOffset-=dwSize;
		//Проецируем блоки
		size_t dwTrgMap=AlignSizeToGranularity(dwTrgOffset);
		size_t dwSrcMap=AlignSizeToGranularity(dwSrcOffset);
		LPVOID lpTrgAddress=MapView(0 ,dwTrgMap, (dwTrgOffset-dwTrgMap)+dwSize);
		LPVOID lpSrcAddress=MapView(0 ,dwSrcMap, (dwSrcOffset-dwSrcMap)+dwSize);
		if (lpTrgAddress==NULL || lpSrcAddress==NULL)
		{
			return false;
		}
		//Определяем фактические адреса блоков данных
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

/** Проецирует часть файла*/
LPVOID VMemoryMappedFile::MapView(const size_t FileOffsetHigh, const size_t FileOffsetLow, const size_t NumberOfBytesToMap)
{
	//Проверяем загружен ли файл
	if (!IsLoaded())
	{
		return NULL;
	}
	LPVOID lpData=NULL;
	DWORD dwDesiredAccess;
	switch (OpenMode)
	{
		//Если открываем только для чтения
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

/** Удаляет проекцию*/
bool VMemoryMappedFile::UnmapView(LPVOID address)
{
	return UnmapViewOfFile(address)==TRUE;
}

/** Возвращает признак загруженности файла*/
bool VMemoryMappedFile::IsLoaded()
{
	return Loaded;
}

/** Возвращает режим открытия файла*/
unsigned long VMemoryMappedFile::GetOpenMode()
{
	return OpenMode;
}

/** Возвращает имя загруженного файла*/
const wchar_t * const VMemoryMappedFile::GetFileName()
{
	return FileName.c_str();
}

#else //Реализация Linux

/** Конструктор*/
VMemoryMappedFile::VMemoryMappedFile()
{
	//Определяем гранулярность выделения памяти
	Granularity=getpagesize();
	Loaded=false;
}

/** Деструктор*/
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

/** Открывает файл*/
bool VMemoryMappedFile::Open(const wchar_t * const filename, unsigned long mode=OPEN_MODE_READ)
{
	Close();	
	FileName=filename;
	OpenMode=mode;
	int flags;
	switch (OpenMode)
	{
		//Если открываем только для чтения
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
	//Открываем файл
	File=open(wstring_to_string(FileName).c_str(), flags);
	free(fn);
	if (File>=0)
	{
	    //Получаем размер файла
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

/** Закрывает файл*/
bool VMemoryMappedFile::Close()
{
	bool result=true;
	if (Loaded)
	{
		//Освобождаем память		
		close(File);
		Loaded=false;
	}
	return result;
}

/** Устанавливает размер файла*/
bool VMemoryMappedFile::SetFileSize(const size_t size)
{
	//Проверяем загружен ли файл
	if (!IsLoaded())
	{
		return false;
	}
	//Вычисляем новый размер файла
	FileSize=size;
	//Устанавливаем размер
	if (ftruncate(File, size)==0)
        return true;
    else
        return false;
}

/** Возвращает размер файла*/
size_t VMemoryMappedFile::GetFileSize()
{
	//Проверяем загружен ли файл
	if (!IsLoaded())
	{
		return 0;
	}
	return FileSize;
}
/** Возвращает гранулярность*/
size_t VMemoryMappedFile::GetGranularity()
{
	return Granularity;
}
/** Удаляет кусок файла*/
bool VMemoryMappedFile::DeletePart(const size_t StartOffset, const size_t EndOffset)
{
	//Проверяем загружен ли файл
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
			//Определяем размер копируемого блока
			size_t dwSize=(Granularity<FileSize-dwSrcOffset) ? Granularity : FileSize-dwSrcOffset;
			//Проецируем блоки
			void * lpTrgAddress=MapView(0 ,dwTrgMap, (dwTrgOffset-dwTrgMap)+dwSize);
			void * lpSrcAddress=MapView(0 ,dwSrcMap, (dwSrcOffset-dwSrcMap)+dwSize);
			if (lpTrgAddress==NULL || lpSrcAddress==NULL)
			{
				return false;
			}
			//Определяем фактические адреса блоков данных
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
			//Смещаемся в файле
			dwTrgOffset+=dwSize;
			dwSrcOffset+=dwSize;
		}
		while (dwSrcOffset<FileSize);
	}
	else
	{
		return false;
	}
	//Изменяем размер файла
	if (!SetFileSize(FileSize-(dwSrcOffset-dwTrgOffset)))
	{
		return false;
	}
	return true;
}
/** Вставляет кусок в файл*/
bool VMemoryMappedFile::InsertPart(const size_t StartOffset, const size_t PartSize)
{
	//Проверяем загружен ли файл
	if (!IsLoaded())
	{
		return false;
	}
	//Изменяем размер файла
	if (!SetFileSize(FileSize+PartSize))
	{
		return false;
	}
	//Определяем размер блока для копирования
	size_t dwSize=(Granularity<PartSize) ? Granularity : PartSize;
	size_t dwTrgOffset=FileSize;
	size_t dwSrcOffset=FileSize-PartSize;
	do
	{
		dwSize=(dwSize<dwSrcOffset-StartOffset) ? dwSize : dwSrcOffset-StartOffset;
		dwTrgOffset-=dwSize;
		dwSrcOffset-=dwSize;
		//Проецируем блоки
		size_t dwTrgMap=AlignSizeToGranularity(dwTrgOffset);
		size_t dwSrcMap=AlignSizeToGranularity(dwSrcOffset);
		void * lpTrgAddress=MapView(0 ,dwTrgMap, (dwTrgOffset-dwTrgMap)+dwSize);
		void * lpSrcAddress=MapView(0 ,dwSrcMap, (dwSrcOffset-dwSrcMap)+dwSize);
		if (lpTrgAddress==NULL || lpSrcAddress==NULL)
		{
			return false;
		}
		//Определяем фактические адреса блоков данных
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
/** Проецирует часть файла*/
void * VMemoryMappedFile::MapView(const size_t FileOffsetHigh, const size_t FileOffsetLow, const size_t NumberOfBytesToMap)
{
	int prot;
	switch (OpenMode)
	{
		//Если открываем только для чтения
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
/** Удаляет проекцию*/
bool VMemoryMappedFile::UnmapView(void * address)
{
	munmap(address, 0);
	return true;
}
/** Возвращает признак загруженности файла*/
bool VMemoryMappedFile::IsLoaded()
{
	return Loaded;
}

/** Возвращает режим открытия файла*/
unsigned long VMemoryMappedFile::GetOpenMode()
{
	return OpenMode;
}

/** Возвращает имя загруженного файла*/
const wchar_t * const VMemoryMappedFile::GetFileName()
{
	return FileName.c_str();
}
/** Возвращает требуемый размер окна проекции, с учетом гранулярности выделения памяти*/
size_t VMemoryMappedFile::AlignSizeToGranularity(const size_t req)
{
	size_t result=req;
	result=(static_cast<size_t>(req/Granularity))*Granularity;
	return result;
}

#endif //_WIN32
