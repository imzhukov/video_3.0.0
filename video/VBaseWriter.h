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

///������� ����� ��� ������ �����
class VBaseWriter
{
public:
	VBaseWriter()
	{
		fullPath = L"";
	}
	///������ �� ����� ��� ������ �����
	virtual bool IsOpened() = 0;
	///����� ������ �����
	virtual void AddFrame(cv::Mat frame) = 0;
	virtual void AddFrame(int width, int height, unsigned char * bufData, int bufDataLen){};
	///����� ��� �������� ������ ������
	virtual bool Open() = 0;
	///������������ ������ ������
	virtual bool Release() = 0;
	///��� �����, ���� ������������ �����
	std::wstring fullPath;
	///����� ��� ������ �����
	///����������: ��� ������ ������������ ������������ ������ ���� � �����
	///����� ������ ������ � ���������� ������������ ������ ���� ������ � ��������� � ����������� �����-�����
	//std::wstring folder;
};
typedef boost::shared_ptr<VBaseWriter> VBaseWriterPtr;