#include <boost/asio.hpp>
#include "VBaseWriter.h"
#include "VFileName.h"

//Boost
#include <boost/format.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/split.hpp>

#include "VCoordinatesRecord.h"
#include "VVideoOptions.h"
#include "VLog.h"

#include <fstream>

///����� ��� ������ ������ ����� (.jpg �� ����) ����� ������ intervalRecording ������
class VJpgWriter : public VBaseWriter
{
	int frame_rate;
	int width;
	int height;
	std::wstring cameraName;
	int intervalRecording;
	///������ ���������� ����������� �����
	double lastPosition;
	///���� �� ������ ������
	bool Opened;
	///�������� �� ����������� ����
	bool isWayStarted;
	///����������, ���� ������������ �����
	std::wstring jpgDir;
	std::ofstream fout;

	int count;
public:
	VJpgWriter(/*std::wstring f, */ std::wstring newCameraName, 
		int newFrame_rate, int newWidth, int newHeight, int newIntervalRecording);
	~VJpgWriter();
	bool IsOpened();
	void AddFrame(Mat frame);	
	bool Open();
	bool Release();
};