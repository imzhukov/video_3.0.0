#pragma once

#include <boost/asio.hpp>
#include <wx/wx.h>
//OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <time.h>
#include <vector>
#include <cstdint>
#include "timelib.h"
#include "strlib.h"

const int V4F_VERSION = 4;

enum V4F_ENCODING
{
	ENCODE_RAW = 1,
	ENCODE_CV_50 = 2
};

void CreateFullPath(char * full_path, long direction, char * way, int64_t in_t,
	std::string short_path, std::string video_server);

class V4fFrame
{
public:
	//Вспомогательный буффер для чтения изображения
	static unsigned char * tmp;
	static int tmpLen;

	mutable IplImage im;
	unsigned long dpp;
	double absCoord;
	int codeEncoding;
	int width;
	int height;
	int imgLen;
	std::vector<unsigned char> img;
	fpos_t pos;							//Позиция в файле, где начинается информация об изображении
	V4fFrame(){};
	V4fFrame(unsigned long _codeDPP, double _absCoord, int _width, int _height, std::vector<unsigned char> & _img);
	V4fFrame(unsigned long _codeDPP, double _absCoord, int _width, int _height, unsigned char * _img, int _imgLen);
	V4fFrame(unsigned long _codeDPP, double _absCoord, fpos_t _pos);
	//Конструктор для вектора с фреймами
	~V4fFrame();
	//Загрузка только информации о координате (абс. коорд. и код ДПП) без картинки с запоминанием места откуда можно эту картинку прочесть
	void LoadCoordsWithoutImage(FILE * v4f);
	//Очистка буффера
	static void ClearTmp()
	{
		if(tmpLen)
		{
			delete [tmpLen] tmp;
			tmpLen=0;
		}
	}
	void LoadV4fImage(FILE * v4f, unsigned char * bufData, int bufDataLen);
};