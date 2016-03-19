#include "VCaptureThread.h"

wxDEFINE_EVENT(wxEVT_COMMAND_CAPTURETHREAD_COMPLETED, wxThreadEvent);
/// Конструктор
VCaptureThread::VCaptureThread(VCameraBase * camera_props, wxWindow * in_screen, wxFrame * m_pFrame, bool &isConnected)
			: screen(in_screen), pFrame(m_pFrame)
{
	bufData = 0;
	bufDataLen = 0;
	captureType = camera_props->GetType();
	capture = 0;

	//writer.reset(new V4fWriter(camera_props->GetDirectory(), camera_props->GetCameraName()));
	//writer.reset(new V4fWriter(camera_props->GetCameraName()));
	switch(camera_props->GetType())
	{
		case RTSP_CAMERA:
		{
			capture = new VCaptureRTSP(camera_props, isConnected);
			//writer.reset(new V4fWriter(camera_props->GetDirectory(), camera_props->GetCameraName()));
			//writer.reset(new VAviWriter(camera_props->GetCameraName(), 10, CODEC_MJPG, capture->GetWidth(), capture->GetHeight()));
			writer.reset(new V4fWriter(camera_props->GetCameraName()));
			break;
		}
		case JAI_CAMERA:
		{
			capture = new VCaptureJAI(camera_props, isConnected);
			//writer.reset(new V4fWriter(camera_props->GetDirectory(), camera_props->GetCameraName()));
			writer.reset(new V4fWriter(camera_props->GetCameraName()));
			break;
		}
	}
}

/// Функция потока
void * VCaptureThread::Entry()
{
	while (!TestDestroy())
	{
		capture->ProcessFrame();
		if(3 * capture->GetWidth() * capture->GetHeight() > bufDataLen)
		{
			if(bufDataLen)
				delete [] bufData;
			bufData = new unsigned char [3 * capture->GetWidth() * capture->GetHeight()];
			bufDataLen = 3 * capture->GetWidth() * capture->GetHeight();
		}
		int tmpBufData = 0;
		capture->GetRawImage(bufData, tmpBufData);
		if(tmpBufData > 0)
		{
			/*
			switch(captureType)
			{
				case RTSP_CAMERA:
				{
					cv::Mat mat = ((VCaptureRTSP *) capture)->GetCvFrame();
					capture->ShowPosition(mat.data, mat.rows*mat.cols*3);
					writer->AddFrame(mat);
					break;
				}
				case JAI_CAMERA:
					writer->AddFrame(capture->GetWidth(), capture->GetHeight(), bufData, tmpBufData);
					break;
			}*/
			writer->AddFrame(capture->GetWidth(), capture->GetHeight(), bufData, tmpBufData);
			if(VIDEO_OPTIONS().Value().showSubtitle)
				capture->ShowPosition(bufData, bufDataLen);
			try
			{
				if (capture->GetWidth() > 0 && capture->GetHeight() > 0 && !screen->IsBeingDeleted())
				{
					wxImage image(capture->GetWidth(), capture->GetHeight(), (unsigned char*) bufData, true);
					wxBitmap bitmap(image);
					wxClientDC dc(screen);
					wxMutexGuiEnter();
					dc.DrawBitmap(bitmap,0,0,false);
					wxMutexGuiLeave();
				}
			}
			catch(VException & excp)
			{
				wxClientDC dc(screen);
				wxMutexGuiEnter();
				dc.DrawText(L"Неправильный кадр!",50,60);
				wxMutexGuiLeave();
			}
			Sleep(1);
		}
	}
	//Закрываем файл записи, если закончили работу с камерой
	if(writer->IsOpened())
		StopRecording();
	return NULL;
}

/// Возвращает размер кадра
wxSize VCaptureThread::GetVideoSize() const
{
	return wxSize(capture->GetWidth(), capture->GetHeight()); 
}

/// Начать запись в файл
void VCaptureThread::StartRecording(/*std::wstring folder, */ int mode)
{
	try
	{
		wxCriticalSectionLocker lock(CaptureGuard);
		if(Start(/*folder, */ mode))
		{
			if(*this->writer->fullPath.c_str())
			{
				wchar_t msg [512];
				swprintf(msg, 511, L"Началась запись в файл: %s", this->writer->fullPath.c_str());
				LOG_INFO(msg);
			}
		}
	}
	catch (VException& e)
	{
		LOG_ERROR(L"Не удалось начать запись в файл");
	}
}

/// Остановить запись в файл
void VCaptureThread::StopRecording()
{
	if(writer->IsOpened())
	{
		//wxCriticalSectionLocker lock(CaptureGuard);
		Stop();
		if(*this->writer->fullPath.c_str())
		{
			wchar_t msg [512];
			swprintf(msg, 511, L"Запись в файл остановлена: %s", this->writer->fullPath.c_str());
			LOG_INFO(msg);
		}
	}
}

bool VCaptureThread::IsRecording() const
{
	return writer->IsOpened();
}

/// Начинает запись в файл
bool VCaptureThread::Start(/*std::wstring folder, */ int mode)
{
	if(!writer->IsOpened())
		return writer->Open();
	else
		return false;
}

/// Останавливает запись в файл
void VCaptureThread::Stop()
{
	if(writer->IsOpened())
	{
		try
		{
			writer->Release();
		}
		catch(std::exception e)
		{
			wchar_t msg [256];
			_snwprintf(msg, 255, L"VCaptureThread: %s", e.what());
			LOG_ERROR(msg);
		}
	}
}

VCaptureThread::~VCaptureThread()
{
	if(bufDataLen && bufData)
		delete [] bufData;
	if(capture)
		delete capture;
	wxThreadEvent e(wxEVT_COMMAND_CAPTURETHREAD_COMPLETED);
	pFrame->GetEventHandler()->AddPendingEvent(e);
}