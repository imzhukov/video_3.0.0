#pragma once
#include <boost/asio.hpp>

#include <wx/wx.h>
#include "timelib.h"
#include "V4fWriter.h"
#include "VCameraProperties.h"
#include "VVideoOptions.h"

/// ����� �������
class VCapture
{
protected:
	wxMemoryDC finallyMemoryDC;
	wxMemoryDC *subtitleMemoryDC;
	wxBitmap *subtitleBitmap;
	VCameraBase * camera_props;
	unsigned char * bufData;
	int bufDataLen;
	unsigned char *bufDataForMonoConvert;
	int bufDataForMonoConvertLen;
public:
	VCapture() : REWRITE_FRAME(false)
	{
		bufData = 0;
		bufDataLen = 0;
		bufDataForMonoConvert = 0;
		bufDataForMonoConvertLen = 0;
		subtitleMemoryDC = new wxMemoryDC();
		subtitleBitmap = 0;
	};
	virtual ~VCapture()
	{
		if(bufDataLen)
			delete [] bufData;
		if(bufDataForMonoConvertLen)
			delete [] bufDataForMonoConvert;
		delete subtitleMemoryDC;
		if(subtitleBitmap)
			delete subtitleBitmap;
	}
	/// ������� � ���� ����������
	void ShowPosition(unsigned char * bufData, int bufDataLen);
	/// ���������� true, ���� ������� ������� ������������ � ������
	virtual bool IsConnected() const = 0;
	/// ���������� ������ �����
	virtual int GetWidth() const = 0;
	/// ���������� ������ �����
	virtual int GetHeight() const = 0;
	/// ��������� ����� � ������ � ���������� � ��������� �����
	virtual void ProcessFrame() = 0;
	/// ���������� ����� ������
	virtual void GetRawImage(unsigned char * image, int & imageLen) = 0;
	//���� ���������� �����
	bool REWRITE_FRAME;
};