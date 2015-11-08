#pragma once

#include <boost/asio.hpp>
#include <wx/wx.h>
#include "options.h"

#include "VLog.h"
#include "VCoordinatesRecord.h"

wxDECLARE_EVENT(wxEVT_COMMAND_DPPTHREAD_COMPLETED, wxThreadEvent);

/// Поток отлавливающий код ДПП
class VCodeDPPThread : public wxThread
{
private:
	std::wstring pcComPort;				//Название com-порта
	HANDLE hCom;						//дескриптор com порта
	//boost::asio::serial_port * hCom;	//дескриптор com порта
	unsigned char buf[5000], bt1, bt2;					//буффер в который складываем информацию из com-порта
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
	wxFrame * pFrame;					//Родительская форма
public:
	VCodeDPPThread(wxFrame * m_pFrame, std::wstring _pcComPort);
	~VCodeDPPThread();
	virtual ExitCode Entry();
	unsigned long startCodeDpp;	//Первый код ДПП
};

DECLARE_VARIABLE_MT(unsigned long, CURRENT_DPP, unsigned long(0))