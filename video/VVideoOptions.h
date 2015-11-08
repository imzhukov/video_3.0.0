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
		tmpltSubtitle = L"%НАПР - %ПУТЬ: %КМ КМ - %М М";
		locationSubtitle = wxBOTTOM;
		shiftSubtitle = 40;
		heightSubtitle = 12;
		com = L"COM1";
		in_server_db = L"";
		in_database = L"";
		ip_udp = L"";
		port_udp = -1;
		shiftCoordinate = 0.0f;
	}
	std::wstring video_server;		//директория видеосервера
	int intervalRecording;			//Интервал съёмки
	bool showSubtitle;				//Выводить субтитры
	std::wstring tmpltSubtitle;		//Шаблон субтитров
	int locationSubtitle;			//Положение субтитров
	int heightSubtitle;				//Высота субтитров
	int shiftSubtitle;				//Сдвиг субтитров от границы экрана
	std::wstring com;				//Имя com-порта для кодов ДПП
	std::wstring in_server_db;		//Сервер БД
	std::wstring in_database;		//Файл БД
	std::wstring ip_udp;			//ip UDP
	int port_udp;					//port UDP
	float shiftCoordinate;			//Постоянная сдвижка координаты
};

DECLARE_VARIABLE_MT(VVideoOptions, VIDEO_OPTIONS, VVideoOptions())
