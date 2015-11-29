#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "VCoordinatesRecord.h"
#include "VLog.h"

using boost::asio::ip::tcp;

class VCoordinatesClient
{
public:
	//Время последней попытки подключения
	static time_t last_connection;
	static bool good_connection;

	//Конструктор
	VCoordinatesClient(boost::asio::io_service& io_service, tcp::resolver::iterator endpoint_iterator, boost::posix_time::time_duration timeout);

	//Деструктор
	~VCoordinatesClient();

	void write(int currentDpp, boost::posix_time::time_duration timeout);

	void close();

private:

	void handle_connect(const boost::system::error_code& error);

	void handle_read_header(const boost::system::error_code& error);

	void handle_read_body(const boost::system::error_code& error);

	void do_write(int currentDpp);

	void handle_write(const boost::system::error_code& error);

	void do_close();

	boost::asio::io_service& io_service_;
	tcp::socket socket_;
	CoordQuestion write_msg_;
	CoordAnswer read_msg_;
	boost::asio::deadline_timer deadline_;
};