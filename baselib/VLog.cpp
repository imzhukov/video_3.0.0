#include "VLog.h"

volatile long VGlobalLog::lock = 0;

//------------------------------------ Методы класса VLogRecord --------------------------------------------

/** Конструктор*/
VLogRecord::VLogRecord(const std::wstring& in_message, log_level_t in_level) :
	message(in_message), level(in_level)
{
	//reg_time автоматически инициализируется текущим временем
};
/** Конструктор*/
VLogRecord::VLogRecord(const VLogRecord& in_log_record)		
{
	*this=in_log_record;
};
/** Оператор =*/
VLogRecord& VLogRecord::operator=(const VLogRecord& in_log_record)
{		
	message=in_log_record.message;
	level=in_log_record.level;
	reg_time=in_log_record.reg_time;
	return *this;
};	
/** Возвращает сообщение*/
const std::wstring& VLogRecord::GetLogMessage() const
{
	return message;
};
/** Возвращает уровень сообщения*/
log_level_t VLogRecord::GetLevel() const
{
	return level;
}
/** Возвращает время регистрации*/
const VTime& VLogRecord::GetTime() const
{
	return reg_time;
};


//------------------------------------ Методы класса VGlobalLog --------------------------------------------


/** Добавляет сообщение*/
void VGlobalLog::AddRecord(const VLogRecord& in_log_record)
{
	::EnterCriticalSection(&this->CS);
	if (in_log_record.GetLevel() <= log_level)
	{
		records.push_back(in_log_record);		
	}
	cache.push_back(in_log_record);
	std::ofstream out(file_name.c_str(), std::ios_base::app);
	out << in_log_record;
	::LeaveCriticalSection(&this->CS);
};

/** Возвращает число сообщений*/
size_t VGlobalLog::GetRecordCount() const
{
	return records.size();
};	

/** Возвращает событие*/
const VLogRecord& VGlobalLog::GetRecord(size_t in_index) const
{
	return records.at(in_index);
};

/** Связывает журнал с файлом на диске*/
void VGlobalLog::Init(const std::wstring& in_file_name, log_level_t in_log_level)
{	
	file_name=in_file_name;
	log_level=in_log_level;
	std::ofstream out(file_name.c_str(), std::ios_base::ate);		
};

/** Устанавливает уровень журнала*/
void VGlobalLog::SetLogLevel(log_level_t in_log_level)
{
	log_level=in_log_level;
	::EnterCriticalSection(&this->CS);
	records.clear();
	for (size_t i=0;i<cache.size();++i)
	{
		if (cache.at(i).GetLevel()<=log_level)
		{
			records.push_back(cache.at(i));
		}
	}		
	::LeaveCriticalSection(&this->CS);
};

/** Возвращает уровень журнала*/
log_level_t VGlobalLog::GetLogLevel() const
{
	return log_level;
};