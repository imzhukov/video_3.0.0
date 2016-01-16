#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <wx/wx.h>
#include "VLog.h"

#include "VCoordinatesClient.h"
#include "VCodeDPPThread.h"
#include "VVideoOptions.h"
//#include "V4fSynchronization.h"	

using boost::asio::ip::tcp;

wxDECLARE_EVENT(wxEVT_COMMAND_COORDTHREAD_COMPLETED, wxThreadEvent);

/// ����� ������ ������ � ������� ����������
class VCoordinatesThread : public wxThread
{
	wxFrame * pFrame;						//������������ ����
	std::string ip_tcp;
	int port_tcp;

	boost::asio::io_service askc_service;
	VCoordinatesClient * client;
public:
	/// �����������
	VCoordinatesThread(wxFrame * m_pFrame, std::string _ip_udp, int _port_tcp);
	/// ����������
	~VCoordinatesThread();
protected:
	/// ������� ������
	virtual ExitCode Entry();
};