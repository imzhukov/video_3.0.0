#include "VCoordinatesClient.h"

time_t VCoordinatesClient::last_connection(NULL);
bool VCoordinatesClient::good_connection(false);

VCoordinatesClient::VCoordinatesClient(boost::asio::io_service& io_service, tcp::resolver::iterator endpoint_iterator, boost::posix_time::time_duration timeout) : io_service_(io_service), deadline_(io_service_), socket_(io_service)
{
	try
	{
		boost::asio::async_connect(socket_, endpoint_iterator, boost::bind(&VCoordinatesClient::handle_connect, this,
			  boost::asio::placeholders::error));
	}
	catch(std::exception & e)
	{
		LOG_ERROR(string_to_wstring(e.what()).c_str());
	}
	//tdpp = 150000;
}

VCoordinatesClient::~VCoordinatesClient()
{
	do_close();
}

void VCoordinatesClient::write(int currentDpp, boost::posix_time::time_duration timeout)
{
	deadline_.expires_from_now(timeout);
	try
	{
		if (socket_.is_open())
		{
			//io_service_.post(boost::bind(&VCoordinatesClient::do_write, this, currentDpp));
			memcpy(write_msg_.signature, SIG_COORD_QUESTION, 8);
			write_msg_.dpp = currentDpp;
			boost::asio::async_write(socket_, boost::asio::buffer(&write_msg_, sizeof(write_msg_)),
				boost::bind(&VCoordinatesClient::handle_write, this, boost::asio::placeholders::error));
			boost::asio::async_read(socket_, boost::asio::buffer(&read_msg_, sizeof(read_msg_)), boost::bind(&VCoordinatesClient::handle_read_header, this, boost::asio::placeholders::error));
		}
	}
	catch (std::exception & e)
	{
		LOG_ERROR(string_to_wstring(e.what()).c_str());
	}
}

void VCoordinatesClient::close()
{
	try
	{
		io_service_.post(boost::bind(&VCoordinatesClient::do_close, this));
	}
	catch(std::exception & e)
	{
		LOG_ERROR(string_to_wstring(e.what()).c_str());
	}
}

void VCoordinatesClient::handle_connect(const boost::system::error_code& error)
{
	time(&VCoordinatesClient::last_connection);
	if (!error)
    {
		strncpy(read_msg_.signature, SIG_COORD_ANSWER, 8);
		write(CURRENT_DPP().Value(), boost::posix_time::millisec(200));
		if(!good_connection)
			LOG_INFO(L"Успешное подключение к серверу координаты");
		good_connection = true;
    }
	else
	{
		good_connection = false;
		wchar_t msg [512] = L"";
		_snwprintf(msg, 511, L"Ошибка при подключении к СК %i: %s", error.value(), string_to_wstring(error.message()).c_str());
		LOG_ERROR(msg);
	}
}

void VCoordinatesClient::handle_read_header(const boost::system::error_code& error)
{
	if (!error)
	{
		/*boost::asio::async_read(socket_,
			boost::asio::buffer(&read_msg_, sizeof(read_msg_)),
			boost::bind(&VCoordinatesClient::handle_read_body, this,
			  boost::asio::placeholders::error));*/
		CURRENT_POSITION().Lock();
		CURRENT_POSITION().Value().km = read_msg_.km;
		CURRENT_POSITION().Value().m = read_msg_.m;
		CURRENT_POSITION().Value().abs_coord = read_msg_.abs_coord;
		CURRENT_POSITION().Value().direction = read_msg_.direction;
		CURRENT_POSITION().Value().dpp = read_msg_.dpp;
		CURRENT_POSITION().Value().start_time = read_msg_.start_time;
		int check = read_msg_.way[0];
		if (check >= '1' && check <= '9')
		{
			for (int i = 0; i < 8; i++)
				CURRENT_POSITION().Value().way[i] = read_msg_.way[i];
		}
		else
			strcpy(CURRENT_POSITION().Value().way, " NO WAY");
		if (*(read_msg_.peregon))
			strncpy(CURRENT_POSITION().Value().peregon, read_msg_.peregon, 79);
		CURRENT_POSITION().Unlock();
		//write(CURRENT_DPP().Value(), boost::posix_time::millisec(200));
	}
	else
	{
		wchar_t msg[512] = L"";
		_snwprintf(msg, 511, L"Ошибка чтения заголовка %i: %s", error.value(), string_to_wstring(error.message()).c_str());
		LOG_ERROR(msg);
		do_close();
	}
}

void VCoordinatesClient::handle_read_body(const boost::system::error_code& error)
{
	if (!error && !strcmp(read_msg_.signature, SIG_COORD_ANSWER))
	{
		CURRENT_POSITION().Lock();
		CURRENT_POSITION().Value().SetCoordAnswer(read_msg_);
		/*if(!CURRENT_POSITION().Value().increaseAbscoord && CURRENT_POSITION().Value().dpp != 0xFFFFFFFF
			&& CURRENT_POSITION().Value().absolutePosition != read_msg_.abs_coord)
		{
			CURRENT_POSITION().Value().increaseAbscoord = (CURRENT_POSITION().Value().absolutePosition < read_msg_.abs_coord) ? 1 : -1;
		}
		CURRENT_POSITION().Value().km = read_msg_.km;
		CURRENT_POSITION().Value().m = read_msg_.m;
		CURRENT_POSITION().Value().absolutePosition = read_msg_.abs_coord;
		CURRENT_POSITION().Value().dir = read_msg_.direction;
		CURRENT_POSITION().Value().dpp = read_msg_.dpp;
		int check = read_msg_.way[0];
		if (check >= '1' && check <= '9')
		{
			for (int i = 0; i < 8; i++)
				CURRENT_POSITION().Value().way[i] = read_msg_.way[i];
		}
		else
			strcpy(CURRENT_POSITION().Value().way," NO WAY");
		if(*(read_msg_.peregon))
			strncpy(CURRENT_POSITION().Value().peregon, read_msg_.peregon, 79);*/
		CURRENT_POSITION().Unlock();
	}
	else
	{
		wchar_t msg[512] = L"";
		_snwprintf(msg, 511, L"Ошибка чтения тела протокола %i: %s", error.value(), string_to_wstring(error.message()).c_str());
		LOG_ERROR(msg);
		do_close();
	}
}

void VCoordinatesClient::do_write(int currentDpp)
{
	try
	{
		if(socket_.is_open())
		{
			memcpy(write_msg_.signature, SIG_COORD_QUESTION, 8);
			write_msg_.dpp = currentDpp;
			boost::asio::async_write(socket_, boost::asio::buffer(&write_msg_, sizeof(write_msg_)), 
				boost::bind(&VCoordinatesClient::handle_write, this, boost::asio::placeholders::error));
			boost::asio::async_read(socket_, boost::asio::buffer(&read_msg_, sizeof(read_msg_)), boost::bind(&VCoordinatesClient::handle_read_header, this, boost::asio::placeholders::error));
		}
	}
	catch(std::exception & e)
	{
		LOG_ERROR(string_to_wstring(e.what()).c_str());
	}
}

void VCoordinatesClient::handle_write(const boost::system::error_code& error)
{
	if (!error && socket_.is_open())
    {
		boost::asio::async_read(socket_, boost::asio::buffer(&read_msg_, sizeof(read_msg_)), boost::bind(&VCoordinatesClient::handle_read_header, this, boost::asio::placeholders::error));
    }
    else
    {
		wchar_t msg [512] = L"";
		_snwprintf(msg, 511, L"Ошибка записи %i: %s", error.value(), string_to_wstring(error.message()).c_str());
		LOG_ERROR(msg);
		do_close();
    }
}

void VCoordinatesClient::do_close()
{
	if(socket_.is_open())
		socket_.close();
}