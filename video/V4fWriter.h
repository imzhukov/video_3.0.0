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

//����� ������ �����-����������, � ������� ������������ ����� � ������
class V4fWriter : public VBaseWriter
{
	V4fDataSet * dataSet;					//������ ��� ������ �������
	V4fCacheThread * cache;					//���
	std::wstring cameraName;				//��� ������
	bool Opened;							//���� �������� �����

	int64_t current_start_time;				//������� ����� ������ (������������� �������)
	int codeEncoding;						//��� ������ �����
	int inputId;
	int counterFrames;						//������� ������
public:
	//�����������
	V4fWriter(std::wstring newCameraName);
	//����������
	~V4fWriter();
	bool IsOpened();
	void AddFrame(cv::Mat frame);
	void AddFrame(int width, int height, unsigned char * bufData, int bufDataLen);
	bool Open();
	bool Release();
};