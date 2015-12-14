#include <boost/asio.hpp>
#include <wx/wx.h>

#include "VCaptureRTSP.h"
#include "VCaptureJAI.h"

#include "V4fWriter.h"
#include "VAviWriter.h"
//#include "V4fSynchronization.h"

wxDECLARE_EVENT(wxEVT_COMMAND_CAPTURETHREAD_COMPLETED, wxThreadEvent);
/// Поток захвата
class VCaptureThread : public wxThread
{
private:	
	/// Устройство захвата
	VCapture * capture;
	/// Указатель на экран перерисовки
	wxWindow * screen;
	/// Указатель на родительский фрейм
	wxFrame * pFrame;
	/// Критическая секция
	wxCriticalSection CaptureGuard;
	/// Класс записывающий видео в файл
	VBaseWriterPtr writer;
	unsigned char * bufData;
	int bufDataLen;
	int captureType;
public:
	/// Конструктор
	VCaptureThread(VCameraBase * camera_props, wxWindow * in_screen, wxFrame * m_pFrame, bool &isConnected);
	/// Функция потока
	void * Entry();
	/// Возвращает размер кадра
	wxSize GetVideoSize() const;
	/// Начинает запись в файл
	bool Start(/*std::wstring folder, */ int modeWriter);
	/// Останавливает запись в файл
	void Stop();
	//Возвращает true, если началась запись
	bool IsRecording() const;	
	/// Начать запись в файл
	void StartRecording(/*std::wstring folder, */ int mode);
	/// Остановить запись в файл
	void StopRecording();
	///Деструктор
	~VCaptureThread();
};