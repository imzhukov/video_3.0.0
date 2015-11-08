#pragma once

#define PORT_SENDER 15997
#define PORT_RECEIVER 15998

/// Initialize WinSock
bool WinSockStart();

/// Stop and clean up WinSock
bool WinSockStop();

/// convert textual ip-string to numeric
unsigned long GetNumericIP(const char * str);

/// кадр для обмена
#pragma pack(push, 1)
struct ExchangeCadre
{
	char sign[8];       // сигнатура 'ExChAnGe'
	long km, m;         // координата
	long dir;           // направление
	char way[8];        // путь
	char peregon[80];   // перегон

	double absCoord;    // абс. к-та
	float floatm;       // метры, float
	float kurs;         // курсовой угол
	float vspeed;       // скорость вагона
	float width;        // шк
	float level;        // уровень
	float kren;         // крен
	float tangage;      // тангаж

	unsigned long dpp;  // счетчик тиков
	bool accurate;      // признак аккуратности координаты
};
#pragma pack(pop)


// ------------------------------------------------------
/// Datagram Sender
class VRtDgrSender
{
	/// socket
	int sock; 
	/// flags: WinSock ready, socket ready
	bool wsa_ready, sock_ready; 

public:

	/// constructor
	VRtDgrSender();
	/// destructor
	~VRtDgrSender();
	/// initialize socket
	bool Init(unsigned long local_ip, unsigned short port);
	/// ready?
	bool Ready() const;
    /// send buffer
    bool Send(unsigned long receiver_ip, unsigned short port, const void * data, int len);
	/// receive buffer
	int Recv(unsigned long sender_ip, unsigned short port, void * data, int len, bool block);
};