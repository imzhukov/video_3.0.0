#include <boost/asio.hpp>
#include <wx/wx.h>

#include "VCaptureRTSP.h"
#include "VCaptureJAI.h"

#include "V4fWriter.h"
#include "VAviWriter.h"
//#include "V4fSynchronization.h"

wxDECLARE_EVENT(wxEVT_COMMAND_CAPTURETHREAD_COMPLETED, wxThreadEvent);
/// ����� �������
class VCaptureThread : public wxThread
{
private:	
	/// ���������� �������
	VCapture * capture;
	/// ��������� �� ����� �����������
	wxWindow * screen;
	/// ��������� �� ������������ �����
	wxFrame * pFrame;
	/// ����������� ������
	wxCriticalSection CaptureGuard;
	/// ����� ������������ ����� � ����
	VBaseWriterPtr writer;
	unsigned char * bufData;
	int bufDataLen;
	int captureType;
public:
	/// �����������
	VCaptureThread(VCameraBase * camera_props, wxWindow * in_screen, wxFrame * m_pFrame, bool &isConnected);
	/// ������� ������
	void * Entry();
	/// ���������� ������ �����
	wxSize GetVideoSize() const;
	/// �������� ������ � ����
	bool Start(/*std::wstring folder, */ int modeWriter);
	/// ������������� ������ � ����
	void Stop();
	//���������� true, ���� �������� ������
	bool IsRecording() const;	
	/// ������ ������ � ����
	void StartRecording(/*std::wstring folder, */ int mode);
	/// ���������� ������ � ����
	void StopRecording();
	///����������
	~VCaptureThread();
};