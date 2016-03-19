#include "VCodeDPPThread.h"

wxDEFINE_EVENT(wxEVT_COMMAND_DPPTHREAD_COMPLETED, wxThreadEvent);

boost::asio::io_service io_service_dpp;

VCodeDPPThread::VCodeDPPThread(wxFrame * m_pFrame, std::wstring _pcComPort) : pFrame(m_pFrame), 
	pcComPort(_pcComPort), startCodeDpp(0xFFFFFFFF)
{
	//Готовим к запуску потока
	bt1 = '\0';
	bt2 = '\0';
	memset(buf, 0, 5000);
	startReadData = false;
	indxCodeDppPtr = 0;
	checkSum = 0;
	constShift = 0;
	tmpCodeDpp = 0;
	
	/*
	FILE * output = fopen("D:\\output_old.txt", "w");
	FILE * session = fopen("C:\\20150903145353.log", "rb");
	if(session != NULL && output != NULL)
	{
		int qqq = 0;
		while(!feof(session))
		{
			//char ptr = '\0';
			fread(&buf, 1, 1, session);
			if(buf[0] == '\t' || buf[0] == '\n' || qqq < 0)
			{
				qqq++;
				continue;
			}
			if(!((buf[0]|0x3F)^0x3F))
				tmpCodeDpp |= buf[0];
			if(!((buf[0]|0x3F)^0x7F))
				tmpCodeDpp |= (int (buf[0] & 0x3F)) << 6;
			if(!((buf[0]|0x3F)^0xBF))
				tmpCodeDpp |= (int (buf[0] & 0x3F)) << 12;
			if(!((buf[0]|0x3F)^0xFF))
			{
				tmpCodeDpp |= (int (buf[0] & 0x3F)) << 18;
				fprintf(output, "%i\n", tmpCodeDpp);
				tmpCodeDpp = 0;
			}
		}
		fclose(session);
		fclose(output);
	}*/

	/*FILE * output = fopen("D:\\output.txt", "w");
	FILE * session = fopen("C:\\session.log", "rb");
	if(session != NULL && output != NULL)
	{
		while(!feof(session))
		{
			//char ptr = '\0';
			fread(&buf, 1, 1, session);
			//Проверка контрольной суммы и загрузка в файл
			if(indxCodeDppPtr == 3)
			{
				checkSum = codeDppPtr[0]+codeDppPtr[1]+codeDppPtr[2]+codeDppPtr[3]+83+4;
				if(checkSumPtr[3]==buf[0])
				{
					if(startCodeDpp > tmpCodeDpp)
						startCodeDpp = tmpCodeDpp;
					fprintf(output, "%i\n", tmpCodeDpp);
				}
				bt1='\0'; bt2='\0';
				tmpCodeDpp = 0;
				indxCodeDppPtr = 0;
				startReadData=false;
			}
			//Пишем информацию из com-порта
			if(startReadData)
			{
				codeDppPtr[indxCodeDppPtr] = buf[0];
				indxCodeDppPtr++;
				continue;
			}
			//Пусть влетели в поток в какой-то момент ищем 2 известных байта (83d и 4d)
			if(!startReadData && buf[0]=='S')
				bt1=buf[0];
			if(!startReadData && buf[0]==4)
				bt2=buf[0];
			if(bt1=='S' && bt2==4)
			{
				startReadData = true;
				indxCodeDppPtr = 0;
				//codeDppPtr[indxCodeDppPtr] = buf;
				//indxCodeDppPtr++;
			}
		}
		fclose(session);
		fclose(output);
	}*/
}

wxThread::ExitCode VCodeDPPThread::Entry()
{
/*	char comadr [100];
	std::string pcCom = wstring_to_string(pcComPort);
	_snprintf(comadr, 99, "\\\\.\\%s", pcCom.c_str());
	hCom = new boost::asio::serial_port(io_service_dpp);
	boost::system::error_code ec;
	hCom->open(comadr, ec);
	if(ec)
	{
		char msg [256];
		_snprintf(msg, 255, "Ошибка при открытии COM порта: %s", ec.message().c_str());
		LOG_ERROR(string_to_wstring((const char *) msg).c_str());
		return (wxThread::ExitCode) wxTHREAD_MISC_ERROR;
	}
	boost::asio::serial_port::baud_rate rate(115200);
	hCom->set_option(rate);
	boost::asio::serial_port::character_size size;
	hCom->set_option(size);
	boost::asio::serial_port::parity parity(boost::asio::serial_port::parity::type::none);
	hCom->set_option(parity);
	boost::asio::serial_port::stop_bits stop_bits(boost::asio::serial_port::stop_bits::type::one);
	hCom->set_option(stop_bits);*/
	try
	{
		DCB dcb;
		wchar_t comadr [100] = L"";
		_snwprintf(comadr, 99, L"\\\\.\\%s", pcComPort.c_str());
		hCom=CreateFile(comadr, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hCom==INVALID_HANDLE_VALUE)
		{
			wchar_t msg [512] = L"";
			_snwprintf(msg, 511, L"Ошибка открытия %s", pcComPort.c_str());
			LOG_ERROR(msg);
			return (wxThread::ExitCode) wxTHREAD_MISC_ERROR;
		}
		GetCommState(hCom, &dcb);
		dcb.BaudRate=CBR_115200;
		dcb.ByteSize=8;
		dcb.Parity=NOPARITY;
		dcb.StopBits=ONESTOPBIT;
		BOOL fSuccess=SetCommState(hCom, &dcb);	
		if (!fSuccess)
		{
			LOG_ERROR(L"Попытка вызвать SetCommState провалилась!");
			return (wxThread::ExitCode) wxTHREAD_MISC_ERROR;
		}
		wchar_t msg [512] = L"";
		_snwprintf(msg, 511, L"COM порт %s успешно сконфигурирован", pcComPort.c_str());
		LOG_INFO(msg);
		GetCommState(hCom, &dcb);
		_snwprintf(msg, 511, L"Скорость порта %s равна %i", pcComPort.c_str(), dcb.BaudRate);
		LOG_INFO(msg);
		COMMTIMEOUTS timeouts;

		timeouts.ReadIntervalTimeout = 1;
		timeouts.ReadTotalTimeoutMultiplier = 1;
		timeouts.ReadTotalTimeoutConstant = 1;
		if (!SetCommTimeouts(hCom, &timeouts))
			LOG_INFO(L"Не удалось установить timeout для com-порта");
		//Готовим к запуску потока
		bt1 = '\0';
		bt2 = '\0';
		buf[0] = '\0';
		startReadData = false;
		indxCodeDppPtr = 3;
		checkSum = 0;
		tmpCodeDpp = 0;
		CURRENT_DPP().Value() = 0;
	}
	catch (std::exception)
	{
		LOG_ERROR(L"Ошибка открытия  COM-порта");
	}
							//Тест
	FILE * f = fopen("E:/dpp", "w");
	while (!TestDestroy() && hCom != INVALID_HANDLE_VALUE)
	{
		try
		{
			bool isRead = ReadFile(hCom, buf, 1, &bufLen, NULL);
			if(!isRead)
				continue;
		/*	if(bufLen == 1)
			{
				if(!((buf[0]|0x3F)^0x3F))
				{
					tmpCodeDpp |= buf[0];
					startReadData = true;
				}
				if(!((buf[0]|0x3F)^0x7F))
					tmpCodeDpp |= (int (buf[0] & 0x3F)) << 6;
				if(!((buf[0]|0x3F)^0xBF))
					tmpCodeDpp |= (int (buf[0] & 0x3F)) << 12;
				if(!((buf[0]|0x3F)^0xFF))
				{
					tmpCodeDpp |= (int (buf[0] & 0x3F)) << 18;
					if(startReadData) {
						CURRENT_DPP().Value() = tmpCodeDpp;
						//Тест
						fprintf(f, "%i\n", tmpCodeDpp);
					}
					tmpCodeDpp = 0;
				}				
			}*/

/*		if(hCom->is_open())
		hCom->async_read_some(boost::asio::buffer(&buf, 1), 
			[this](boost::system::error_code ec, std::size_t bytes_recvd)*/
			//{
				//if (bytes_recvd>0)
			/*
				if(bufLen == 1)
				{
					//Проверка контрольной суммы и загрузка в файл
					if(indxCodeDppPtr == 3)
					{
						checkSum = codeDppPtr[0]+codeDppPtr[1]+codeDppPtr[2]+codeDppPtr[3]+83+4;
						wchar_t msg [256];
						_snwprintf(msg, 255, L"Контрольная сумма: %i | %i | %i | %i | %i | %i | %i | %i", buf[0], codeDppPtr[0], codeDppPtr[1], codeDppPtr[2], codeDppPtr[3], checkSumPtr[0], checkSumPtr[3]);
						//LOG_DEBUG(msg);
						if(checkSumPtr[3] == buf[0])
						{
							CURRENT_DPP().Lock();
							if(!constShift)
								constShift = tmpCodeDpp - CURRENT_POSITION().Value().dpp;
							CURRENT_DPP().Value() = tmpCodeDpp - constShift;
							wchar_t msg [256];
							_snwprintf(msg, 255, L"Код: %i", CURRENT_DPP().Value());
							LOG_DEBUG(msg);
							CURRENT_DPP().Unlock();
						}
						bt1='\0'; bt2='\0';
						tmpCodeDpp = 0;
						indxCodeDppPtr = 0;
						startReadData=false;
					}
					//Пишем информацию из com-порта
					if(startReadData)
					{
						wchar_t msg [256];
						_snwprintf(msg, 255, L"%c", buf[0]);
						//LOG_DEBUG(msg);
						codeDppPtr[indxCodeDppPtr] = buf[0];
						indxCodeDppPtr++;
						continue;
						//return;
					}
					//Пусть влетели в поток в какой-то момент ищем 2 известных байта (83d и 4d)
					if(!startReadData && buf[0]=='S')
						bt1=buf[0];
					if(!startReadData && buf[0]==4)
						bt2=buf[0];
					if(bt1=='S' && bt2==4)
					{
						startReadData = true;
						indxCodeDppPtr = 0;
						//LOG_DEBUG(L"Нашли стартовую метку");
						//codeDppPtr[indxCodeDppPtr] = buf[0];
						wchar_t msg [256];
						_snwprintf(msg, 255, L"%c", buf[0]);
						//LOG_DEBUG(msg);
						//indxCodeDppPtr++;
					}
				}
			//});
			bufLen = 0;*/
		}
		catch(std::exception e)
		{
			LOG_ERROR(L"Ошибка приёма кода ДПП");
		}
	}

	fclose(f);
	return (wxThread::ExitCode)0;
}

VCodeDPPThread::~VCodeDPPThread()
{
	try
	{
		if(hCom != INVALID_HANDLE_VALUE)
		{

			CloseHandle(hCom);
			//delete hCom;
		}
	}
	catch(std::exception e)
	{
		LOG_ERROR(L"Ошибка при закрытии потока кодов ДПП");
	}
	wxThreadEvent e(wxEVT_COMMAND_DPPTHREAD_COMPLETED);
	pFrame->GetEventHandler()->AddPendingEvent(e);
}