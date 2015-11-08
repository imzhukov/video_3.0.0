#pragma once
#include <boost/asio.hpp>
#include <windows.h>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <string>
#include <deque>
#include <set>
#include <fstream>
#include <strstream>
#include "strlib.h"
#include "timelib.h"


/** Уровни журналирования*/
enum log_level_t {LOG_LEVEL_ERROR=1, LOG_LEVEL_WARNING=2, LOG_LEVEL_INFO=3, LOG_LEVEL_DEBUG=4};

/** Запись в журнале*/
class VLogRecord
{
private:		
	/** Сообщение*/
	std::wstring message;
	/** Уровень сообщения*/
	log_level_t level;
	/** Время регистрации*/
	VTime reg_time;
public:
	/** Конструктор*/
	VLogRecord(const std::wstring& in_message, log_level_t in_level);
	/** Конструктор*/
	VLogRecord(const VLogRecord& in_log_record);
	/** Оператор =*/
	VLogRecord& operator=(const VLogRecord& in_log_record);	
	/** Возвращает сообщение*/
	const std::wstring& GetLogMessage() const;
	/** Возвращает уровень сообщения*/
	log_level_t GetLevel() const;
	/** Возвращает время регистрации*/
	const VTime& GetTime() const;
	/** Опреатор вывода в поток*/
	friend std::ostream& operator<<(std::ostream& out, const VLogRecord& in_log_record)
	{		
		out << "[" << in_log_record.GetLevel() << "], ";
		out << in_log_record.GetTime().ToStringLocale() << ", ";		
		out << wstring_to_string(in_log_record.GetLogMessage()) << std::endl;
		return out;
	};

};

/** Глобальный журнал*/
class VGlobalLog
{
private:

	/** Закрытый конструктор*/
	VGlobalLog()
	{
		::InitializeCriticalSection(&this->CS);
		lock=1;
	};
	/** Закрытый деструктор*/
	~VGlobalLog()
	{
		::DeleteCriticalSection(&this->CS);
	};
	/** Закрытый копирующий конструктор*/
	VGlobalLog(VGlobalLog&);
	/** Закрытый оператор присваивания*/
	VGlobalLog& operator=(VGlobalLog&);
	
	static VGlobalLog * GetPtr()
	{
		VGlobalLog * ptr=NULL;
		try
		{
			static VGlobalLog log;
			ptr=&log;
		}
		catch(...)
		{
			ptr=NULL;
		}
		return ptr;
	}

	//Данные

	/** Критическая секция для thread-безопасности*/
	CRITICAL_SECTION CS;
	/** Имя файла*/
	std::wstring file_name;
	/** Записи*/
	std::deque<VLogRecord> records;
	/** Записи*/
	std::deque<VLogRecord> cache;
	/** Уровень*/
	log_level_t log_level;	
	/** Lock variable*/
	static volatile long lock;

		
public:
	
	/** Возвращает указатель на экземпляр класса*/
	static VGlobalLog * Instance()
	{		
		while (::InterlockedExchange(&lock, 1)	!= 0)
		{
			Sleep(1);
		}
		VGlobalLog * ptr = GetPtr();
		::InterlockedExchange(&lock, 0);
		return ptr;		
	};
	
	/** Добавляет сообщение*/
	void AddRecord(const VLogRecord& in_log_record);

	/** Возвращает число сообщений*/
	size_t GetRecordCount() const;	

	/** Возвращает событие*/
	const VLogRecord& GetRecord(size_t in_index) const;

	/** Связывает журнал с файлом на диске*/
	void Init(const std::wstring& in_file_name, log_level_t in_log_level=LOG_LEVEL_DEBUG);
	
	/** Устанавливает уровень журнала*/
	void SetLogLevel(log_level_t in_log_level);

	/** Возвращает уровень журнала*/
	log_level_t GetLogLevel() const;
};

#define LOG VGlobalLog::Instance()
#define LOG_INIT(path, level) LOG->Init(path, level);
#define LOG_ERROR(message) LOG->AddRecord(VLogRecord(message, LOG_LEVEL_ERROR))
#define LOG_WARNING(message) LOG->AddRecord(VLogRecord(message, LOG_LEVEL_WARNING))
#define LOG_INFO(message) LOG->AddRecord(VLogRecord(message, LOG_LEVEL_INFO))
#define LOG_DEBUG(message) LOG->AddRecord(VLogRecord(message, LOG_LEVEL_DEBUG))
#define LOG_TMP(message) LOG->AddRecord(VLogRecord(message, LOG_LEVEL_DEBUG))