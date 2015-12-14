#include "VCoordinatesThread.h"

using boost::asio::ip::tcp;

const double DELAY_CONNECTION = 10.0;

wxDEFINE_EVENT(wxEVT_COMMAND_COORDTHREAD_COMPLETED, wxThreadEvent);
//----------------------------------------------- Методы класса VCoordinatesThread -------------------------------------
/// Конструктор
VCoordinatesThread::VCoordinatesThread(wxFrame * m_pFrame, std::string _ip_tcp, int _port_tcp): wxThread(wxTHREAD_DETACHED),
	pFrame(m_pFrame)
{
	ip_tcp = _ip_tcp;
	port_tcp = _port_tcp;
}

/// Функция потока
wxThread::ExitCode VCoordinatesThread::Entry()
{
	int tdpp = 100000;
	while (!TestDestroy())
	{
		time_t now = time_t(NULL);
		time(&now);
		double diff_c = 10.1;
		if(VCoordinatesClient::last_connection != NULL)
			diff_c = difftime(now, VCoordinatesClient::last_connection);
		if(diff_c > DELAY_CONNECTION || VCoordinatesClient::good_connection)
		{
			try
			{
				boost::asio::io_service io_service;

				tcp::resolver resolver(io_service);
				char port_tcp_str[10] = "";
				_snprintf(port_tcp_str, 9, "%i", port_tcp);
				tcp::resolver::query query(ip_tcp.c_str(), port_tcp_str);
				tcp::resolver::iterator iterator = resolver.resolve(query);
				VCoordinatesClient client(io_service, iterator, boost::posix_time::millisec(200));
				boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));
				Sleep(50);
				int shift_dpp = 0;
				//if(CURRENT_POSITION().Value().speed > 0)
					//shift_dpp = VIDEO_OPTIONS().Value().shiftCoordinateSpeedPositive / 0.24;
				//else
					//shift_dpp =  - VIDEO_OPTIONS().Value().shiftCoordinateSpeedNegative / 0.24;
				client.write(CURRENT_DPP().Value(), boost::posix_time::millisec(200));
				//client.write(tdpp, boost::posix_time::millisec(200));
				Sleep(200);
				client.close();
				t.join();
				tdpp += 5;
			}
			catch(std::exception e)
			{
				LOG_ERROR(L"Ошибка boost asio");
				continue;
			}
		}
	}
	return (wxThread::ExitCode)0;
}

VCoordinatesThread::~VCoordinatesThread()
{
	wxThreadEvent e(wxEVT_COMMAND_COORDTHREAD_COMPLETED);
	pFrame->GetEventHandler()->AddPendingEvent(e);
}
