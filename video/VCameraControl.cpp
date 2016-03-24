#include "VCameraControl.h"

//int NotConnectedWidth = 400, NotConnectedHeight = 200;
///��������� ��������� ������
bool isConnected = false;

BEGIN_EVENT_TABLE(VCameraControl,wxFrame)
	EVT_TOOL(ID_START_RECORDING,VCameraControl::OnStartRecording)
	EVT_TOOL(ID_STOP_RECORDING,VCameraControl::OnStopRecording)
	EVT_UPDATE_UI(ID_START_RECORDING,VCameraControl::OnUpdateUI)
	EVT_UPDATE_UI(ID_STOP_RECORDING,VCameraControl::OnUpdateUI)
	EVT_UPDATE_UI(ID_TOOLBAR_CAMERACONTROL,VCameraControl::OnUpdateUI)
	EVT_PAINT(VCameraControl::OnPaint)
END_EVENT_TABLE()

void VCameraControl::OnPaint(wxPaintEvent &ev)
{
	wxSize size;
	///������� �������������� �������, ���� �� ������� ���������� ������
	if (!isConnected)
	{
		///������ ������ ���� �� ���������
		size.SetWidth(400);
		size.SetHeight(200);
		this->SetMinSize(size);
		screen->SetMinSize(size);
		wxPaintDC dc(screen);
		dc.DrawText(L"�� ������� ���������� ������!",50,60);
		dc.DrawText(L"��������� ��������� �����������.", 50, 80);
	}
	else
	{
		size = capture->GetVideoSize();
		size.SetWidth(size.GetWidth());
		size.SetHeight(size.GetHeight());
	}
	Update();
	Fit();
	ev.Skip();
}

/// �������� ������
void VCameraControl::OnStartRecording(wxCommandEvent & in_event)
{
	try
	{
		localtime(&now);
		capture->StartRecording(type_rec);
		isRecording = true;
		in_event.Skip();
	}
	catch(std::exception e)
	{
		wchar_t msg [256];
		_snwprintf(msg, 255, L"VCameraControl: %s", e.what());
		LOG_ERROR(msg);
	}
}

/// ������������� ������
void VCameraControl::OnStopRecording(wxCommandEvent & in_event)
{
	try
	{
		capture->StopRecording();
		isRecording = false;
		in_event.Skip();
	}
	catch(std::exception e)
	{
		wchar_t msg [256];
		_snwprintf(msg, 255, L"VCameraControl: %s", e.what());
		LOG_ERROR(msg);
	}
}

/// ���������� ������� UPDATE_UI
void VCameraControl::OnUpdateUI(wxUpdateUIEvent & in_event)
{
	if(isConnected)
	{
		if(capture->IsRecording())
		{
			time_t _now = time(NULL);
			localtime(&_now);
			double diff = difftime(_now, now);
			//!!!!�������� � ���� ������ ���������� �������� ������
			//!!!!� ����� ������� ��������� ������� ����� �� ����������, �� ���������...
			if(diff > intervalRecording * 60 /*|| CURRENT_POSITION().Value().dir <= 0*/)
			{
				now = _now;
				capture->StopRecording();
				//this->capture->StartRecording(directory,type_rec);
				this->capture->StartRecording(type_rec);
			}
		}
		else if(isRecording)
		{
			//������ ������, �� �� ������� ������ ������ �� �����-�� ��������
			time_t _now = time(NULL);
			localtime(&_now);
			double diff = difftime(_now, now);
			if(diff > 2)
			{
				now = _now;
				//this->capture->StartRecording(directory,type_rec);
				this->capture->StartRecording(type_rec);
			}
		}
	}
	switch (in_event.GetId())
	{
		case ID_START_RECORDING:
			in_event.Enable(!isRecording && isConnected);
			break;
		case ID_STOP_RECORDING:
			in_event.Enable(isRecording  && isConnected);
			break;
		default:
			break;
	}
}
/// �����������
VCameraControl::VCameraControl(VCameraBase * camera_props, wxWindow* parent, int WindowId, bool * _existing) :  wxFrame(parent, WindowId, camera_props->GetCameraName().c_str()),
	isRecording(false)
{	
	existing = _existing;
	//directory = camera_props->GetDirectory();
	now = time(NULL);
	intervalRecording = VIDEO_OPTIONS().Value().intervalRecording;
	//������ ������ � ������ �����
	wxIcon video_icon(L"video.exe", wxBITMAP_TYPE_ICO);
	this->SetIcon(video_icon);

	hbox = new wxBoxSizer(wxHORIZONTAL);
	wxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	SetSizer(hbox);
	sizer = new wxBoxSizer(wxVERTICAL);
	wxSizer *toolSizer = new wxBoxSizer(wxHORIZONTAL);
	
	///������� � ��������� ������ ���������� � �������� � ������� �����
	toolbar = new wxToolBar(this,ID_TOOLBAR_CAMERACONTROL,wxDefaultPosition, wxSize(28,28),wxTB_HORIZONTAL);	
	toolbar->SetToolBitmapSize(wxSize(22,22));
	toolbar->AddTool(ID_START_RECORDING, L"������", wxBitmap(media_record_xpm), 
			wxNullBitmap, wxITEM_NORMAL, L"������ ������ � ����");
	toolbar->AddTool(ID_STOP_RECORDING, L"����", wxBitmap(media_playback_stop_xpm), 
			wxNullBitmap, wxITEM_NORMAL, L"���������� ������ � ����");
	wchar_t txt_rec [32];
	switch(camera_props->GetType())
	{
		case RTSP_CAMERA:
		{
			wxStaticText *lbl = new wxStaticText(toolbar, wxID_ANY, L"����� ������ �����: .avi");
			lbl->SetBackgroundColour(this->GetBackgroundColour());
			toolbar->AddSeparator();
			toolbar->AddControl(lbl);
			break;
		}
		case JAI_CAMERA:
		{
			wxStaticText *lbl = new wxStaticText(toolbar, wxID_ANY, L"����� ������ �����: .v4f");
			toolbar->AddSeparator();
			toolbar->AddControl(lbl);
			break;
		}
	}

	toolbar->Realize();
	screen=new wxPanel(this,wxID_ANY,wxDefaultPosition,wxSize(1280,800));
	
	///�������������� ����� ������
	LOG_INFO(L"������������ ������ ������");
	capture = new VCaptureThread(camera_props, screen, this, isConnected);
	Connect(wxID_ANY, wxEVT_COMMAND_CAPTURETHREAD_COMPLETED, wxThreadEventHandler(VCameraControl::OnCompleteCaptureThread), NULL, this);
	capture->Create();
	capture->Run();
	sizer->Add(toolbar,0,wxEXPAND| wxTOP,0);
	wxSize size;
	///���� ����������� ��������� �������
	if (isConnected) 
	{	
		size = capture->GetVideoSize();
		size.SetWidth(size.GetWidth() + 20);
		size.SetHeight(size.GetHeight() + 40);
		this->SetMinSize(size);
		screen->SetMinSize(size);
	}
	///���� �� ������� ���������� ������
	else
	{
		///������ ������ ���� �� ���������
		size.SetWidth(400);
		size.SetHeight(200);
		screen->SetMinSize(size);		
	}
	sizer->Add(screen,1,wxALL|wxEXPAND,0);
	vbox->Add(sizer);
	hbox->Add(vbox);
	if (isConnected)
	{
		size = capture->GetVideoSize();
		size.SetWidth(size.GetWidth());
		size.SetHeight(size.GetHeight());
	}
	else
	{
		size.SetWidth(400);
		size.SetHeight(200);
	}
	screen->SetMinSize(size);
	this->SetMinSize(size);
	this->Centre();
	this->Center();
	this->Fit();
	*existing = true;
}

/// ����������
VCameraControl::~VCameraControl()
{
	*existing = false;
	//FixMe: ������� ���������� ���������� ������
	/*if(capture && capture->TestDestroy())
		capture->Kill();*/
}

/// ���������� ������
wxSize VCameraControl::GetBestSize() const
{
	return hbox->GetMinSize();
}

void VCameraControl::OnCompleteCaptureThread(wxThreadEvent & event)
{
	capture = 0;
	event.Skip();
}