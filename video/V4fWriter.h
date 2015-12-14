#pragma once

#include <boost/asio.hpp>
#include "VBaseWriter.h"

#include "VCoordinatesRecord.h"
#include "VCodeDPPThread.h"
//#include "V4fSynchronization.h"
#include "VVideoOptions.h"
#include "V4fDataSet.h"
#include "VLog.h"
#include "VFileName.h"
#include "V4fCacheThread.h"

#include <boost/format.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/split.hpp>

#include <queue>

const int MISSED_FRAMES = 10;

//Класс записи файла-контейнера, в который записываются кадры с камеры
class V4fWriter : public VBaseWriter
{
	V4fDataSet * dataSet;					//Объект для записи фреймов
	V4fCacheThread * cache;					//Кэш
	std::wstring cameraName;				//Имя камеры
	bool Opened;							//Флаг открытия файла

	int64_t current_start_time;				//Текущее время старта (идентификатор поездки)
	int codeEncoding;						//Код сжатия файла
	int inputId;
	int counterFrames;						//Счётчик кадров
public:
	//Коснструтор
	V4fWriter(std::wstring newCameraName);
	//Деструктор
	~V4fWriter();
	bool IsOpened();
	void AddFrame(cv::Mat frame);
	void AddFrame(int width, int height, unsigned char * bufData, int bufDataLen);
	bool Open();
	bool Release();
};