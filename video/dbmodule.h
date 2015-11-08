#pragma once

#include <boost/asio.hpp>
#include <wx/wx.h>
#include "dblib.h"
//Boost
#include <boost/format.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/split.hpp>

/** Описание зарегистрированной БД*/
class VRegisteredDatabase
{
private:
	/** Имя*/
	wstring name;
	/** Сервер*/
	wstring server;
	/** Путь к базе*/
	wstring database;
	/** Папка с измерениями*/
	wstring folder;
	/** Допустимое кол-во дней без невыполненного резервного копирования*/
	int count_days_backup;
public:
	/** Конструктор*/
	VRegisteredDatabase() : name(L""), server(L""), database(L""), folder(L""), count_days_backup(0x7FFFFFFF) {} ;
	/** Конструктор*/
	VRegisteredDatabase(const wstring& in_fullstring);
	/** Конструктор*/
	VRegisteredDatabase(const wstring& in_name, const wstring& in_server, const wstring& in_database, 
		const wstring& in_folder, const int& in_count_days_backup = 0x7FFFFFFF) : name(in_name), server(in_server), database(in_database), folder(in_folder), count_days_backup(in_count_days_backup) {};
	/** Возвращает полную строку*/
	const wstring GetFullString() const {return name+L";"+server+L";"+database+L";"+folder+L";"+boost::str(boost::wformat(L"%i") % count_days_backup);};
	/** Возвращает строку соединения*/
	const wstring GetConnectString() const {return server!=L"" ? server+L":"+database : database;}; 
	/** Возвращает имя*/
	const wstring& GetName() const {return name;};
	/** Возвращает сервер*/
	const wstring& GetServer() const {return server;}; 
	/** Возвращает путь к файлу БД*/
	const wstring& GetDatabase() const {return database;}; 
	/** Возвращает путь к папке с измерениями*/
	const wstring& GetFolder() const {return folder;};
	/** Возвращает допустимое кол-во дней без невыполненного резервного копирования*/
	const int& GetAllowDaysBackup() const {return count_days_backup;};
	/** Установить допустимое кол-во дней без невыполненного резервного копирования*/
	void SetCountDaysBackup(int in_count_days_backup) {count_days_backup=in_count_days_backup;};
};
//Подключение к БД
void ConnectDB(VIBDatabasePtr db, VRegisteredDatabase & RegisteredDatabase);