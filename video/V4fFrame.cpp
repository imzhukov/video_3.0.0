#include "V4fFrame.h"

int V4fFrame::tmpLen = 0;
unsigned char * V4fFrame::tmp = 0;

void CreateFullPath(char * full_path, long direction, char * way, int64_t in_t, 
	std::string short_path, std::string video_server)
{
	VTime t(in_t);
	std::string _video_server;
	if(video_server.length() > 0 && (video_server.at(video_server.length() - 1) == '\\' ||
		video_server.at(video_server.length() - 1) == '\/'))
		_video_server = video_server.substr(0, video_server.length() - 1);
	else
		_video_server = video_server;
	char sub_directory [MAX_PATH] = "";
	_snprintf(sub_directory, MAX_PATH, "%s\\%i_%s_%s", _video_server.c_str(), 
		direction, way, t.DateToStringISO().c_str());
	_snprintf(full_path, MAX_PATH, "%s\\%s", sub_directory, 
		short_path.c_str());
	SECURITY_ATTRIBUTES sa; 
	sa.nLength= sizeof(sa);
	sa.lpSecurityDescriptor= NULL;
	try
	{
		CreateDirectory(string_to_wstring(sub_directory).c_str(), &sa);
	}
	catch(std::exception e){}
}

V4fFrame::V4fFrame(unsigned long _codeDPP, double _absCoord, int _width, int _height, std::vector<unsigned char> & _img)
{
	imgLen = _img.size();
	img = _img;
	dpp = _codeDPP;
	absCoord = _absCoord;
	width = _width;
	height = _height;
}

V4fFrame::V4fFrame(unsigned long _codeDPP, double _absCoord, int _width, int _height, unsigned char * _img, int _imgLen) : img(_imgLen)
{
	std::copy(_img, _img + _imgLen, img.begin());
	imgLen = imgLen;
	dpp = _codeDPP;
	absCoord = _absCoord;
	width = _width;
	height = _height;
}

V4fFrame::V4fFrame(unsigned long _codeDPP, double _absCoord, fpos_t _pos)
{
	imgLen=0;
	dpp = _codeDPP;
	absCoord = _absCoord;
	pos = _pos;
}

V4fFrame::~V4fFrame()
{}

void V4fFrame::LoadCoordsWithoutImage(FILE * v4f)
{
	fread(&dpp, 4, 1, v4f);
	fread(&absCoord, 8, 1, v4f);
	//Этот блок должен оставаться неизменным!!!
	codeEncoding = 0;
	fread(&codeEncoding, 4, 1, v4f);
	width = 0;
	fread(&width, 4, 1, v4f);
	height = 0;
	fread(&height, 4, 1, v4f);
	imgLen = 0;
	fread(&imgLen, 4, 1, v4f);
	//Запоминаем указатель на изображение
	fgetpos(v4f, &pos);
	//Просто передвигаем указатель в файле без записи какой-либо информации
	if(tmpLen < imgLen)
	{
		if(tmp)
			delete [tmpLen] tmp;
		tmp = new unsigned char[imgLen];
		tmpLen = imgLen;
	}
	fread(tmp, imgLen, 1, v4f);
}

void V4fFrame::LoadV4fImage(FILE * v4f, unsigned char * bufData, int bufDataLen)
{
	fsetpos(v4f, &pos);
	switch(codeEncoding)
	{
		case ENCODE_CV_50:
		{
			std::vector<unsigned char> buf;
			for(int idx = 0; idx < imgLen; idx++)
			{
				unsigned char ch='\0';
				fread(&ch, 1, 1, v4f);
				buf.push_back(ch);
			}
			cv::Mat mat;
			if(buf.size())
				imdecode(buf, 1, &mat);
			if(bufDataLen >= (width * height * 3))
			{
				im = mat;
				memcpy(bufData, im.imageData, im.height * im.width * 3);
			}
			break;
		}
		case ENCODE_RAW:
		{
			if(imgLen <= bufDataLen)
			{
				if(imgLen <= (width * height))
				{
					//8-bit картинка
					if(tmpLen < imgLen)
					{
						if(tmp)
							delete [tmpLen] tmp;
						tmp = new unsigned char[imgLen];
						tmpLen = imgLen;
					}
					fread(tmp, imgLen, 1, v4f);
					unsigned char * pSrc = tmp;
					unsigned char * pSrcEnd = pSrc + ( width * height );
					while ( pSrc < pSrcEnd )
					{
						bufData[0] = bufData[1] = bufData[2] = *pSrc++;
						bufData += 3;
					}
					bufData -= bufDataLen;
				}
				else
					fread(bufData, imgLen, 1, v4f);
			}
			break;
		}
	}	
}

