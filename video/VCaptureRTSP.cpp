#include "VCaptureRTSP.h"

/// �����������
VCaptureRTSP::VCaptureRTSP(VCameraBase * in_camera_props, bool & isConnected) : VCapture()
{
	camera_props = in_camera_props;
	try
	{
		camera.open(wstring_to_string(((VRTSPCameraProperties *) camera_props)->GetAddress()));
	}
	catch(Exception & excp)
	{
		LOG_INFO(string_to_wstring(excp.what()).c_str());
	}
	//������������ � ��������
	codec=CODEC_MJPG;
	frame_rate=10;	//frame_rate=10 ��� BRICKCOM � frame_rate=25 ��� HIKVISION
	if (camera.isOpened())
	{
		width=(int)camera.get(CV_CAP_PROP_FRAME_WIDTH);
		height=(int)camera.get(CV_CAP_PROP_FRAME_HEIGHT);
		///������ ������� ��� bitmap ���������
		subtitleBitmap = new wxBitmap(width, height);
		if (frame_rate == FPS_AUTO)
			frame_rate=(int)camera.get(CV_CAP_PROP_FPS);
		isConnected = true;
		wchar_t msg [512];
		swprintf(msg, 512, L"����� ���������������, ������ %s ����������", in_camera_props->GetCameraName().c_str());
		LOG_INFO(msg);
	}
	else
	{
		isConnected = false;
		LOG_INFO(L"�� ������� ���������� ������ " + in_camera_props->GetCameraName());
	}
}

/// ����������
VCaptureRTSP::~VCaptureRTSP()
{
	if (camera.isOpened())
		camera.release();

	wchar_t msg [512];
	swprintf(msg, 512, L"����������� ������ � ������� %s ", camera_props->GetCameraName().c_str());
	LOG_INFO(msg);
}

bool VCaptureRTSP::IsConnected() const
{
	return camera.isOpened();
}

int VCaptureRTSP::GetWidth() const
{
	if(camera.isOpened())
		return frame.cols ? frame.cols : width;
	else
		return 0;
}

int VCaptureRTSP::GetHeight() const
{
	if(camera.isOpened())
		return frame.rows ? frame.rows : height;
	else
		return 0;
}

void VCaptureRTSP::ProcessFrame()
{
	if(camera.isOpened())
	{
		try
		{
			cv::Mat _frame;
			if(camera.read(_frame))
			{
				frame = _frame;
				img = frame;
			}
		}
		catch(std::exception e)
		{
			LOG_ERROR(L"Error while process frame in capture rtsp");
		}
	}
}

cv::Mat VCaptureRTSP::GetCvFrame()
{
	return frame;
}

void VCaptureRTSP::GetRawImage(unsigned char * image, int & imageLen)
{
	if(camera.isOpened())
	{
		try
		{
			memcpy(image, img.imageData, img.imageSize);
			imageLen = img.imageSize;
		}
		catch(std::exception e)
		{
			LOG_ERROR(L"Error while get raw image in capture rtsp");
		}
	}
}