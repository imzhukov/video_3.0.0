#pragma once

#include <wx/wx.h>
#include "options.h"


class VVideoOptions
{
public:
	VVideoOptions()
	{
		video_server = L"C:\\";
		intervalRecording = 5;
		showSubtitle = true;
		tmpltSubtitle = L"%���� - %����: %�� �� - %� �";
		locationSubtitle = wxBOTTOM;
		shiftSubtitle = 40;
		heightSubtitle = 12;
		com = L"COM1";
		in_server_db = L"";
		in_database = L"";
		ip_udp = L"";
		port_udp = -1;
		shiftCoordinateViewer = 0.0f;
		shiftCoordinateSpeedPositive = 0.0f;
		shiftCoordinateSpeedNegative = 0.0f;
		formatDpp = 0;
	}
	std::wstring video_server;			//���������� ������������
	int intervalRecording;				//�������� ������
	bool showSubtitle;					//�������� ��������
	std::wstring tmpltSubtitle;			//������ ���������
	int locationSubtitle;				//��������� ���������
	int heightSubtitle;					//������ ���������
	int shiftSubtitle;					//����� ��������� �� ������� ������
	std::wstring com;					//��� com-����� ��� ����� ���
	std::wstring in_server_db;			//������ ��
	std::wstring in_database;			//���� ��
	std::wstring ip_udp;				//ip UDP
	int port_udp;						//port UDP
	float shiftCoordinateViewer;		//C������ ���������� ��� ������������ ������
	float shiftCoordinateSpeedPositive;	//C������ ���������� ��� ������������� ��������
	float shiftCoordinateSpeedNegative;	//C������ ���������� ��� ������������� ��������
	int formatDpp;						//������ ��������� ���� ��� (0 - �����, 1 - ������)
};

DECLARE_VARIABLE_MT(VVideoOptions, VIDEO_OPTIONS, VVideoOptions())
