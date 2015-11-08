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

	//int tdpp;
	/*
public:
	VCoordinatesClient(boost::asio::io_service& io_service, tcp::resolver::iterator endpoint_iterator) : io_service_(io_service), socket_(io_service)
	{
		boost::asio::async_connect(socket_, endpoint_iterator, boost::bind(&VCoordinatesClient::handle_connect, this,
			  boost::asio::placeholders::error));
	}

  void write(const chat_message& msg)
  {
    io_service_.post(boost::bind(&VCoordinatesClient::do_write, this, msg));
  }

  void close()
  {
    io_service_.post(boost::bind(&VCoordinatesClient::do_close, this));
  }

private:

  void handle_connect(const boost::system::error_code& error)
  {
    if (!error)
    {
      boost::asio::async_read(socket_,
          boost::asio::buffer(read_msg_.data(), chat_message::header_length),
          boost::bind(&VCoordinatesClient::handle_read_header, this,
            boost::asio::placeholders::error));
    }
  }

  void handle_read_header(const boost::system::error_code& error)
  {
    if (!error && read_msg_.decode_header())
    {
      boost::asio::async_read(socket_,
          boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
          boost::bind(&VCoordinatesClient::handle_read_body, this,
            boost::asio::placeholders::error));
    }
    else
    {
      do_close();
    }
  }

  void handle_read_body(const boost::system::error_code& error)
  {
    if (!error)
    {
      LOG_INFO(string_to_wstring(read_msg_.body()));
      boost::asio::async_read(socket_,
          boost::asio::buffer(read_msg_.data(), chat_message::header_length),
          boost::bind(&VCoordinatesClient::handle_read_header, this,
            boost::asio::placeholders::error));
    }
    else
    {
      do_close();
    }
  }

  void do_write(chat_message msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      boost::asio::async_write(socket_,
          boost::asio::buffer(write_msgs_.front().data(),
            write_msgs_.front().length()),
          boost::bind(&VCoordinatesClient::handle_write, this,
            boost::asio::placeholders::error));
    }
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      write_msgs_.pop_front();
      if (!write_msgs_.empty())
      {
        boost::asio::async_write(socket_,
            boost::asio::buffer(write_msgs_.front().data(),
              write_msgs_.front().length()),
            boost::bind(&VCoordinatesClient::handle_write, this,
              boost::asio::placeholders::error));
      }
    }
    else
    {
      do_close();
    }
  }

  void do_close()
  {
    socket_.close();
  }

private:
  boost::asio::io_service& io_service_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;*/
};