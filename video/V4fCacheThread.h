#pragma once
#include <boost/asio.hpp>

#include <wx/wx.h>
#include <fstream>
#include "V4fDataSet.h"

#define CACHE_V4F_VERSION	1L
#define MAX_COUNT_FRAMES	2000L

wxDECLARE_EVENT(wxEVT_COMMAND_V4FCACHETHREAD_COMPLETED, wxThreadEvent);
class V4fCacheThread : public wxThread
{
	/// Указатель на родительский фрейм
	wxFrame * pFrame;
	///Полный путь к директории видеосервера
	std::string video_server;
	///Запись в файл
	std::vector<V4fDataSet> records;
	///Буфер для записи короткого имени файла
	char * tmp_short_name;
	///Создать путь к файлу кэша
	void CreateFullPathCacheFile(char * cache_file_path);
	///Вектор фреймов
	std::vector<V4fFrame> frames;
	std::vector<V4fFrame>::iterator current_frame;
	///Текущие параметры поиска кадров
	int start_code_dpp;
	int end_code_dpp;
	int64_t t;

	unsigned char * imageBuffer;
	int imageBufferSize;
public:
	///Констуктор
	V4fCacheThread(wxFrame * in_pFrame, std::string in_video_server);
	///Деструтор
	~V4fCacheThread();
	/// Функция потока
	void * Entry();
	///Добавляет запись о файле в кэш
	void AddRecord(V4fDataSet * data_set);
	///Устанавливает диапазон поиска кадров
	void SetRangeFrames(int in_start_code_dpp, int in_end_code_dpp, int64_t in_t);
	///Возвращает фрейм по коду дпп
	V4fFrame * GetFrame(int code_dpp);
	///Возвращет следующий фрейм
	V4fFrame * NextFrame();
	///Возвращет предыдущий фрейм
	V4fFrame * PreviousFrame();
	///Считать файлы из кэша
	void ReadFromCache();
	///Записать файлы в кэш
	void WriteToCache();
};