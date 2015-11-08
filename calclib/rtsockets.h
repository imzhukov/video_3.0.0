#pragma once

#define PORT_SENDER 15997
#define PORT_RECEIVER 15998

/// Initialize WinSock
bool WinSockStart();

/// Stop and clean up WinSock
bool WinSockStop();

/// convert textual ip-string to numeric
unsigned long GetNumericIP(const char * str);

/// ���� ��� ������
#pragma pack(push, 1)
struct ExchangeCadre
{
	char sign[8];       // ��������� 'ExChAnGe'
	long km, m;         // ����������
	long dir;           // �����������
	char way[8];        // ����
	char peregon[80];   // �������

	double absCoord;    // ���. �-��
	float floatm;       // �����, float
	float kurs;         // �������� ����
	float vspeed;       // �������� ������
	float width;        // ��
	float level;        // �������
	float kren;         // ����
	float tangage;      // ������

	unsigned long dpp;  // ������� �����
	bool accurate;      // ������� ������������ ����������
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