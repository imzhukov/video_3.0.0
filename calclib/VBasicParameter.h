#pragma once

#include <typeinfo.h>
#include <string>

#include "c4flib.h"
#include "error_messages.h"

/// Файл данных
class VMainFile;
class VRemoteFile;

/// Расчет контрольной суммы
uint32_t GetCrc32(const void *s, size_t len);

// ---------------------------------------------------------------------------
/// Базовый класс для работы с блоком данных в файле
// ---------------------------------------------------------------------------
class VBasicParameter
{
	bool isLoaded;		///< данные загружены
	uint8_t * dataPtr;	///< указатель на набор данных
	bool isAlloc;		///< память для блока выделена тут
	size_t dataSize;	///< размер блока данных
	uint32_t dataType;	///< тип данных в блоке
	uint32_t index;		///< индекс
	uint32_t version;	///< версия параметра
	VTime createTime;	///< время создания блока
	VTime updateTime;	///< время измерения блока
	size_t recCount;	///< количество записей
	size_t recSize;		///< размер записи
	int cType;			///< С-тип записи
	uint32_t flags;		///< флаги
	uint32_t crc;		///< контрольная сумма блока
	std::wstring message;///< сообщение об ошибке

	// Копирущий конструктор закрыт
	VBasicParameter(const VBasicParameter &);

	// Оператор присваивания закрыт
	VBasicParameter & operator = (const VBasicParameter &);

public:

	/// конструктор
	VBasicParameter() : isLoaded(false), dataPtr(NULL), isAlloc(false), 
		dataType(-1), index(0), version(0), createTime(), updateTime(), 
		recCount(0), recSize(0), cType(VCT_UNKNOWN), flags(0), crc(0),
		message(ERRMSG_DATA_NOT_LOADED)
	{}

	/// деструктор
	~VBasicParameter()
	{
		this->Unload();
	}

	/// Сообщение об ошибке
	const std::wstring & GetErrorMessage() const
	{
		return this->message;
	}

	/// загрузка из файла
	bool Load(VMainFile & file, uint32_t type, bool copyToMemory);

	/// загрузка из удаленного файла
	bool Load(VRemoteFile & remote, uint32_t type, bool copyToMemory);

	/// загрузка из блока данных
	bool LoadBlock(const VDataBlock * dataBlock, bool copyToMemory);

	/// Отцепиться от файла
	void UnloadFile(VMainFile & file, uint32_t type);

	/// Отцепиться от удаленного файла
	void UnloadFile(VRemoteFile & remote, uint32_t type);

	/// загрузка виртуального параметра (блок в памяти, не в файле)
	bool Create(uint32_t dataType, uint32_t cType, uint32_t recCount, uint32_t recSize, uint32_t version);

	/// запись блока в файл
	/// recCount переустанавливает значение в заголовке
	/// для ситуации с неопределенным recSize,
	/// т.е. когда блок создавался с recCount = 1
	/// если при создании блока recCount > 1, это значение игнорируется
	/// если __recCount остается равным 0, его значение игнорируется
	bool Save(VMainFile & file, uint32_t recCount, uint32_t recSize);

	/// Данные в блоке изменились
	bool IsChanged() const
	{
		return this->crc != GetCrc32(this->dataPtr, this->dataSize);
	}

	/// выгрузка параметра
	void Unload();

	/// параметр загружен?
	bool IsLoaded() const
	{
		return this->isLoaded;
	}

	/// Установить флаг загрузки параметра
	void SetLoaded(bool value)
	{
		this->isLoaded = value;
	}

	/// параметр находится в памяти
	bool IsAlloc() const
	{
		return this->isAlloc;
	}

	/// флаги
	uint32_t GetFlags() const
	{
		return this->flags;
	}

	/// Число записей
	size_t GetRecCount() const 
	{
		return this->recCount;
	}

	/// Размер записи, по информации файла
	size_t GetRecSize() const
	{
		return this->recSize;
	}

	/// Размер блока данных
	size_t GetDataSize() const
	{
		return this->dataSize;
	}

	/// Вернуть тип данных
	uint32_t GetDataType() const
	{
		return this->dataType;
	}

	/// Вернуть тип С
	int GetCType() const
	{
		return this->cType;
	}

	/// Вернуть индекс
	uint32_t GetIndex() const
	{
		return this->index;
	}

	/// Версия структуры
	size_t GetVersion() const
	{
		return this->version;
	}

	/// Обновить версию программы
//	void UpdateVersion(uint32_t version)
//	{
//		this->version = version;
//	}

	/// Вернуть время создания
	const VTime & GetCreateTime() const
	{
		return this->createTime;
	}

	/// Вернуть время обновления
	const VTime & GetUpdateTime() const
	{
		return this->updateTime;
	}

	/// Обновить время обновления блока
	void UpdateTime(const VTime & updateTime)
	{
		this->updateTime = updateTime;
	}

	/// Константный указатель на данные по смещению в байтах
	const void * GetPtr(size_t offset) const
	{
		return this->dataPtr + offset;
	}

	/// Указатель на данные по смещению в байтах
	void * GetPtr(size_t offset)
	{
		return this->dataPtr + offset;
	}

	/// Константный указатель на типизированные данные по смещению в элементах
	template <class T> const T * GetPtr(size_t offset) const 
	{
		return static_cast<const T*>(this->GetPtr(offset * sizeof(T)));
	}

	/// Указатель на типизированные данные по смещению в элементах
	template <class T> T * GetPtr(size_t offset)
	{
		return static_cast<T*>(this->GetPtr(offset * sizeof(T)));
	}

	/// проверка корректности типа "C"
	template <class T> bool CheckCType() const;

	/// проверка числа кадров
	template <class T> bool CheckRecCount() const
	{
		return this->dataSize == this->recCount * sizeof(T) && this->recSize == sizeof(T);
	}

	/// получить константную ссылку на  i-й элемент
	template <class T> const T & GetItem(size_t i) const
	{
		return *this->GetPtr<T>(i);
	}

	/// получить ссылку на  i-й элемент
	template <class T> T & GetItem(size_t i) 
	{
		return *this->GetPtr<T>(i);
	}

	/// псевдо-последовательное чтение из блока
	/// текущая позиция сохраняется во внешней переменной position
	void ReadBlock(void * buf, size_t len, size_t & position) const
	{
		memcpy(buf, this->GetPtr(position), len);
		position += len;
	}

	/// Шаблонная функция псевдо-последовательного чтения из блока
	template <class T> void Read(T & buf, size_t & position) const
	{
		this->ReadBlock(&buf, sizeof(buf), position);
	}

	/// псевдо-последовательная запись в блок
	/// текущая позиция сохраняется во внешней переменной position
	void WriteBlock(const void * buf, size_t len, size_t & position)
	{
		memcpy(this->GetPtr(position), buf, len);
		position += len;
	}

	/// Шаблонная функция псевдо-последовательной записи в блок
	template <class T> void Write(const T & buf, size_t & position)
	{
		this->WriteBlock(&buf, sizeof(buf), position);
	}
};

// Проверка корректности типа "C" при шаблонном доступе
template <class T> bool VBasicParameter::CheckCType() const
{
	int cType = this->GetCType();
	size_t recSize = this->GetRecSize();

	if (cType == VCT_UNKNOWN || cType == VCT_USER) 
		return false;

	else if (typeid(T) == typeid(float) && cType == VCT_FLOAT && recSize == sizeof(float)) 
		return true;

	else if (typeid(T) == typeid(double) && cType == VCT_DOUBLE && recSize == sizeof(double)) 
		return true;

	else if (typeid(T) == typeid(int) && cType == VCT_INT && recSize == sizeof(int)) 
		return true;	

	else if (typeid(T) == typeid(unsigned int) && cType == VCT_UINT && recSize == sizeof(int)) 
		return true;	

	else if (typeid(T) == typeid(long) && cType == VCT_LONG && recSize == sizeof(long)) 
		return true;	

	else if (typeid(T) == typeid(unsigned long) && cType == VCT_ULONG && recSize == sizeof(unsigned long)) 
			return true;	

	else if (typeid(T) == typeid(char) && cType == VCT_CHAR && recSize == sizeof(char)) 
		return true;	

	else if (typeid(T) == typeid(unsigned char) && cType == VCT_UCHAR && recSize == sizeof(unsigned char)) 
		return true;	

	else if (typeid(T) == typeid(short) && cType == VCT_SHORT && recSize == sizeof(short)) 
		return true;	

	else if (typeid(T) == typeid(unsigned short) && cType == VCT_USHORT && recSize == sizeof(unsigned short)) 
		return true;	

	else 
		return false;
}

/// Вернуть корректный C-тип для указанного типа T
template <class T> uint32_t GetCType() 
{
	if (typeid(T) == typeid(float))					return VCT_FLOAT;
	else if (typeid(T) == typeid(double))			return VCT_DOUBLE;
	else if (typeid(T) == typeid(int))				return VCT_INT;
	else if (typeid(T) == typeid(unsigned int))		return VCT_UINT;
	else if (typeid(T) == typeid(long))				return VCT_LONG;
	else if (typeid(T) == typeid(unsigned long))	return VCT_ULONG;
	else if (typeid(T) == typeid(char))				return VCT_CHAR;
	else if (typeid(T) == typeid(unsigned char))	return VCT_UCHAR;
	else if (typeid(T) == typeid(short))			return VCT_SHORT;
	else if (typeid(T) == typeid(unsigned short))	return VCT_USHORT;
	else											return VCT_USER;
}
