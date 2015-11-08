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

///����� ��� ������ ����� � ������� AVI
class VAviWriter : public VBaseWriter
{
private:
	///����������, ����������� ��� ������ ������ � ���������
	VideoWriter *writer;
	///��� ������
	std::wstring cameraName;
	int frame_rate;
	int codec;
	int width;
	int height;
public:
	VAviWriter(/*std::wstring f,*/ std::wstring newCameraName,
		int newFrame_rate, int newCodec, int newWidth, int newHeight);
	~VAviWriter();
	bool IsOpened();
	void AddFrame(Mat frame);
	bool Open();
	bool Release();
};