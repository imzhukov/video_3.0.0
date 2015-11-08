#pragma once

#include <boost/asio.hpp>

#include "V4fFrame.h"

class V4fDataSet
{
	int versionFile;					//Версия файла
	long dir;							//Направление
	char way [8];						//Путь
	int64_t start_time;					//Время начала записи файла
	int codeEncoding;					//Код сжатия файла

	std::vector<V4fFrame> frames;
	std::string full_path;
	std::string short_name;
	FILE * handler;

	int start_code_dpp;
	int last_code_dpp;
public:
	V4fDataSet(std::string in_full_path, std::string in_short_name);
	V4fDataSet(std::string in_full_path, std::string in_short_name, long & _dir, char * _way, int64_t _start_time);
	V4fDataSet(const V4fDataSet & ds);

	~V4fDataSet();

	int OpenWriteFile();

	int OpenReadFile();

	int WriteHeader();

	int ReadHeader();

	void WriteFrame(int codeDpp, double absCoord, int codeEncoding, int height, int width, unsigned char * imgData, int imgDataLen);

	void LoadFrames();

	void CloseFileAndClear();

	long GetDir();

	char * GetWay();

	int64_t GetTime();

	std::vector<V4fFrame> & GetFrames();

	void LoadImageW(int pos, unsigned char * bufData, int bufDataLen);

	int GetStartCodeDpp();
	
	int GetLastCodeDpp();

	std::string GetShortName();

	void SetShortName(std::string in_short_name);

	std::string GetFullPath();

	void SetFullPath(std::string in_full_path);
};