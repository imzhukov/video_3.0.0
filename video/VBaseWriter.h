#pragma once
#include <boost/asio.hpp>

#include <string> 
#include "strlib.h"

//OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//boost
#include <boost/shared_ptr.hpp>

using namespace cv;

///Базовый класс для записи видео
class VBaseWriter
{
public:
	VBaseWriter()
	{
		fullPath = L"";
	}
	///Открыт ли поток для записи видео
	virtual bool IsOpened() = 0;
	///Метод записи кадра
	virtual void AddFrame(cv::Mat frame) = 0;
	virtual void AddFrame(int width, int height, unsigned char * bufData, int bufDataLen){};
	///Метод для открытия потока записи
	virtual bool Open() = 0;
	///Освобождение потока записи
	virtual bool Release() = 0;
	///Имя файла, куда записывается видео
	std::wstring fullPath;
	///Папка для записи видео
	///Примечание: При вызове конструктора записывается только путь к файлу
	///После начала записи в переменную записывается полный путь вместе с названием и расширением видео-файла
	//std::wstring folder;
};
typedef boost::shared_ptr<VBaseWriter> VBaseWriterPtr;