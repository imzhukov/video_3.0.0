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


/** ������ ��������������*/
enum log_level_t {LOG_LEVEL_ERROR=1, LOG_LEVEL_WARNING=2, LOG_LEVEL_INFO=3, LOG_LEVEL_DEBUG=4};

/** ������ � �������*/
class VLogRecord
{
private:		
	/** ���������*/
	std::wstring message;
	/** ������� ���������*/
	log_level_t level;
	/** ����� �����������*/
	VTime reg_time;
public:
	/** �����������*/
	VLogRecord(const std::wstring& in_message, log_level_t in_level);
	/** �����������*/
	VLogRecord(const VLogRecord& in_log_record);
	/** �������� =*/
	VLogRecord& operator=(const VLogRecord& in_log_record);	
	/** ���������� ���������*/
	const std::wstring& GetLogMessage() const;
	/** ���������� ������� ���������*/
	log_level_t GetLevel() const;
	/** ���������� ����� �����������*/
	const VTime& GetTime() const;
	/** �������� ������ � �����*/
	friend std::ostream& operator<<(std::ostream& out, const VLogRecord& in_log_record)
	{		
		out << "[" << in_log_record.GetLevel() << "], ";
		out << in_log_record.GetTime().ToStringLocale() << ", ";		
		out << wstring_to_string(in_log_record.GetLogMessage()) << std::endl;
		return out;
	};

};

/** ���������� ������*/
class VGlobalLog
{
private:

	/** �������� �����������*/
	VGlobalLog()
	{
		::InitializeCriticalSection(&this->CS);
		lock=1;
	};
	/** �������� ����������*/
	~VGlobalLog()
	{
		::DeleteCriticalSection(&this->CS);
	};
	/** �������� ���������� �����������*/
	VGlobalLog(VGlobalLog&);
	/** �������� �������� ������������*/
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

	//������

	/** ����������� ������ ��� thread-������������*/
	CRITICAL_SECTION CS;
	/** ��� �����*/
	std::wstring file_name;
	/** ������*/
	std::deque<VLogRecord> records;
	/** ������*/
	std::deque<VLogRecord> cache;
	/** �������*/
	log_level_t log_level;	
	/** Lock variable*/
	static volatile long lock;

		
public:
	
	/** ���������� ��������� �� ��������� ������*/
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
	
	/** ��������� ���������*/
	void AddRecord(const VLogRecord& in_log_record);

	/** ���������� ����� ���������*/
	size_t GetRecordCount() const;	

	/** ���������� �������*/
	const VLogRecord& GetRecord(size_t in_index) const;

	/** ��������� ������ � ������ �� �����*/
	void Init(const std::wstring& in_file_name, log_level_t in_log_level=LOG_LEVEL_DEBUG);
	
	/** ������������� ������� �������*/
	void SetLogLevel(log_level_t in_log_level);

	/** ���������� ������� �������*/
	log_level_t GetLogLevel() const;
};

#define LOG VGlobalLog::Instance()
#define LOG_INIT(path, level) LOG->Init(path, level);
#define LOG_ERROR(message) LOG->AddRecord(VLogRecord(message, LOG_LEVEL_ERROR))
#define LOG_WARNING(message) LOG->AddRecord(VLogRecord(message, LOG_LEVEL_WARNING))
#define LOG_INFO(message) LOG->AddRecord(VLogRecord(message, LOG_LEVEL_INFO))
#define LOG_DEBUG(message) LOG->AddRecord(VLogRecord(message, LOG_LEVEL_DEBUG))
#define LOG_TMP(message) LOG->AddRecord(VLogRecord(message, LOG_LEVEL_DEBUG))