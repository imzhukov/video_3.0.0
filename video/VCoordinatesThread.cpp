#include "VCoordinatesThread.h"

using boost::asio::ip::tcp;

const double DELAY_CONNECTION = 10.0;

wxDEFINE_EVENT(wxEVT_COMMAND_COORDTHREAD_COMPLETED, wxThreadEvent);
//----------------------------------------------- Методы класса VCoordinatesThread -------------------------------------
/// Конструктор
VCoordinatesThread::VCoordinatesThread(wxFrame * m_pFrame, std::string _ip_tcp, int _port_tcp): wxThread(wxTHREAD_DETACHED),
	pFrame(m_pFrame), askc_service()
{
	ip_tcp = _ip_tcp;
	port_tcp = _port_tcp;
	client = 0;
}

/// Функция потока
wxThread::ExitCode VCoordinatesThread::Entry()
{
	int tdpp = 100000;

	tcp::resolver resolver(askc_service);
				char port_tcp_str[10] = "";
				_snprintf(port_tcp_str, 9, "%i", port_tcp);
				tcp::resolver::query query(ip_tcp.c_str(), port_tcp_str);
				tcp::resolver::iterator iterator = resolver.resolve(query);
				client = new VCoordinatesClient(askc_service, iterator, boost::posix_time::millisec(200));
				//askc_service.run();
				boost::thread t(boost::bind(&boost::asio::io_service::run, &askc_service));
	while (!TestDestroy())
	{
		Sleep(200);
		client->write(CURRENT_DPP().Value(), boost::posix_time::millisec(200));
						//t.join();
		/*time_t now = time_t(NULL);
		time(&now);
		double diff_c = 10.1;
		if(VCoordinatesClient::last_connection != NULL)
			diff_c = difftime(now, VCoordinatesClient::last_connection);
		if(diff_c > DELAY_CONNECTION || VCoordinatesClient::good_connection)
		{
			try
			{
				if(client)
				{
					delete client;
					client = 0;
				}
				if(t)
				{
					t->join();
					delete t;
					t = 0;
				}
				client = new VCoordinatesClient(io_service, iterator, boost::posix_time::millisec(200));
				t = new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service));
				//CreateConnection();
				//Sleep(50);
				//int shift_dpp = 0;
				//if(CURRENT_POSITION().Value().speed > 0)
					//shift_dpp = VIDEO_OPTIONS().Value().shiftCoordinateSpeedPositive / 0.24;
				//else
					//shift_dpp =  - VIDEO_OPTIONS().Value().shiftCoordinateSpeedNegative / 0.24;
				//client.write(CURRENT_DPP().Value(), boost::posix_time::millisec(200));
				//client.write(tdpp, boost::posix_time::millisec(200));
				//Sleep(200);
				//tdpp += 5;
			}
			catch(std::exception e)
			{
				LOG_ERROR(L"Ошибка boost asio");
				continue;
			}
		}*/
	}
	t.join();
/*				if(client)
				{
					client->close();
					delete client;
					client = 0;
				}
				if(t)
				{
					t->join();
					delete t;
					t = 0;
				}*/
	return (wxThread::ExitCode)0;
}

VCoordinatesThread::~VCoordinatesThread()
{
	client->close();
	delete client;
	wxThreadEvent e(wxEVT_COMMAND_COORDTHREAD_COMPLETED);
	pFrame->GetEventHandler()->AddPendingEvent(e);
}
