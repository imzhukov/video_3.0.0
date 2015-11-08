#pragma once

#include <boost/asio.hpp>
#include <wx/wx.h>
#include "options.h"

#include "VLog.h"
#include "VCoordinatesRecord.h"

wxDECLARE_EVENT(wxEVT_COMMAND_DPPTHREAD_COMPLETED, wxThreadEvent);

/// ����� ������������� ��� ���
class VCodeDPPThread : public wxThread
{
private:
	std::wstring pcComPort;				//�������� com-�����
	HANDLE hCom;						//���������� com �����
	//boost::asio::serial_port * hCom;	//���������� com �����
	unsigned char buf[5000], bt1, bt2;					//������ � ������� ���������� ���������� �� com-�����
	DWORD bufLen;
	bool startReadData;
	int indxCodeDppPtr;
	unsigned long constShift;
	union
	{
		unsigned int checkSum;
		unsigned char checkSumPtr[4];
	};
	union
	{
		unsigned long tmpCodeDpp;
		unsigned char codeDppPtr [4];
	};
	wxFrame * pFrame;					//������������ �����
public:
	VCodeDPPThread(wxFrame * m_pFrame, std::wstring _pcComPort);
	~VCodeDPPThread();
	virtual ExitCode Entry();
	unsigned long startCodeDpp;	//������ ��� ���
};

DECLARE_VARIABLE_MT(unsigned long, CURRENT_DPP, unsigned long(0))