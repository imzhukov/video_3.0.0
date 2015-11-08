#pragma once

#include "os.h"
#include <string>

/** Базовый класс исключения
	Все исключения должны быть унаследованы от него.
*/
class VException
{
protected:	
	/** Описание*/
	std::wstring description;	
	/** Подробности*/
	std::wstring details;
	/** Отладочная информация*/
	std::wstring debug;
public:			
	/** Деструктор*/
	virtual ~VException(){};	
	/** Возвращает описание ошибки. В наследниках необходимо перекрыть этот метод*/
	virtual const std::wstring& GetDescription() const {return description;};
	/** Возвращает описание ошибки*/
	virtual const std::wstring& GetDetails() const {return details;};	
	/** Возвращает отладочную информацию*/
	virtual const std::wstring& GetDebug() const {return debug;};
	/** Форматирует короткое сообщение*/
	virtual std::wstring FormatShort();
	/** Форматирует длинное сообщение*/
	virtual std::wstring FormatLong();
};


/** Исключение*/
class VSimpleException : public VException
{
public:

	/** Конструктор*/
	VSimpleException(const std::wstring& in_description, const std::wstring& in_details);

	/** Конструктор*/
	VSimpleException(const std::wstring& in_description, const std::wstring& in_details, 
		const std::wstring& in_file, uint32_t in_line);
		
	/** Конструктор*/
	VSimpleException(const std::wstring& in_description, const std::wstring& in_details, 
		const std::wstring& in_file, uint32_t in_line, const std::wstring& in_typename);
};

#define NOT_SUPPORTED VSimpleException(L"Возможность не поддерживается", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__)
#define NULL_POINTER VSimpleException(L"Неинициализированный указатель", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__)
#define OUT_OF_BOUNDS VSimpleException(L"Индекс выходит за границы диапазона", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__)
#define ITEM_NOT_FOUND VSimpleException(L"Объект не найден", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__)
#define STRING_PARSE_ERROR(in_str) VSimpleException(L"Ошибка разбора строки", in_str, L"strlib.cpp", __LINE__)
#define DIVISION_BY_ZERO VSimpleException(L"Деление на ноль", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__)
#define STATEMENT_TOO_LONG VSimpleException(L"Превышена длина запроса", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__)

/** Исключение инициированное пользователем*/
class VUserException
{
private:
	/** Описание*/
	std::wstring description;
public:
	/** Конструктор*/
	VUserException(const std::wstring& in_description) : description(in_description){}	
	/** Возвращает описание ошибки*/
	const std::wstring& GetDescription() const
	{
		return description;
	}	
};


