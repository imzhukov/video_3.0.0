#include "rtsockets.h"

#include <winsock2.h>

bool WinSockStart()
{
	WSADATA wsa;
	WORD version = WINSOCK_VERSION;

	if (WSAStartup(version, &wsa) != 0) return false;
	if (wsa.wVersion != version) return false;

	return true;
}

bool WinSockStop()
{
	WSACleanup();
	return true;
}

// convert textual ip-string to numeric
unsigned long GetNumericIP(const char * str)
{
	if (str == NULL || str[0] == 0)	return 0;
	else return ::inet_addr(str);
}

// --------------------------------------------------------------------------

// create structure for given pair address:port
static struct sockaddr_in FillSockAddr(unsigned long address, unsigned short port)
{
	struct sockaddr_in sockstr;
	memset(&sockstr, 0, sizeof(sockstr));
	sockstr.sin_family = AF_INET;
	sockstr.sin_addr.s_addr = address;
	sockstr.sin_port = htons(port);
	return sockstr;
}

// constructor
VRtDgrSender::VRtDgrSender() : sock(0), wsa_ready(true), sock_ready(false) 
{
} 

// destructor
VRtDgrSender::~VRtDgrSender()
{
	if (this->sock_ready)
	{
		::closesocket(this->sock);
	}
}

// ready?
bool VRtDgrSender::Ready() const
{
	return this->wsa_ready && this->sock_ready;
}

// initialize socket
bool VRtDgrSender::Init(unsigned long local_ip, unsigned short port)
{

	this->sock_ready = false;
	if (!this->wsa_ready)
		return false;

	this->sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (this->sock < 0)
		return false;

	struct sockaddr_in us = ::FillSockAddr(local_ip, port);
	if (::bind(this->sock, (struct sockaddr *) & us, sizeof(struct sockaddr)) != 0)
		return false;

	this->sock_ready = true;
	return true;
}

// send buffer
bool VRtDgrSender::Send(unsigned long receiver_ip, unsigned short port,
						const void * data, int len)
{
	if (!this->sock_ready) return false;

	struct sockaddr_in them = ::FillSockAddr(receiver_ip, port);

	return (len == ::sendto(this->sock,
		reinterpret_cast<const char *>(data), len, 0,
		(struct sockaddr *) &them, sizeof(struct sockaddr)));
}

// receive buffer
int VRtDgrSender::Recv(unsigned long sender_ip, unsigned short port, 
					   void * data, int len, bool block)
{
	if (!this->sock_ready) return 0;

	struct sockaddr_in them = ::FillSockAddr(sender_ip, port);
	int slen = sizeof(struct sockaddr);

	if (!block)
	{
		fd_set rfds;
		struct timeval tv;

		FD_ZERO(&rfds);
		FD_SET(this->sock, &rfds);

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int res = ::select(this->sock+1, &rfds, NULL, NULL, &tv);

		if (res <= 0) return false;
	}

	int rcvlen = ::recvfrom(this->sock, reinterpret_cast<char *>(data), len,
		0, (struct sockaddr *) &them, &slen);

	if (rcvlen == SOCKET_ERROR)
	{
		int errorcode = ::WSAGetLastError();
		int Z;
		switch(errorcode)
		{
		case WSANOTINITIALISED:	Z = 0; break;
		case WSAENETDOWN:		Z = 1; break;
		case WSAEFAULT:			Z = 2; break;
		case WSAEINTR:			Z = 3; break;
		case WSAEINPROGRESS:	Z = 4; break;
		case WSAEINVAL:			Z = 5; break;
		case WSAEISCONN:		Z = 6; break;
		case WSAENETRESET:		Z = 7; break;
		case WSAENOTSOCK:		Z = 8; break;
		case WSAEOPNOTSUPP:		Z = 9; break;
		case WSAESHUTDOWN:	    Z = 10; break;
		case WSAEWOULDBLOCK:	Z = 11; break;
		case WSAEMSGSIZE:		Z = 12; break;
		case WSAETIMEDOUT:		Z = 13; break;
		case WSAECONNRESET:		Z = 14; break;
		default:				Z = 100;
		}
	}

	return rcvlen;
}

