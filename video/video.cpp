#include "video.h"

//---------------------------- Методы класса VLogViewer --------------------------------

BEGIN_EVENT_TABLE(VLogViewer, wxListCtrl)
	EVT_LIST_ITEM_RIGHT_CLICK(wxID_ANY, VLogViewer::OnRightClick)
	EVT_LIST_COL_RIGHT_CLICK(wxID_ANY, VLogViewer::OnRightClick)
	EVT_UPDATE_UI(wxID_ANY, VLogViewer::OnUpdateUI)
END_EVENT_TABLE()

/** Обработчик правого клика*/
void VLogViewer::OnRightClick(wxListEvent& in_event)
{	
	popup_menu.Check(ID_SET_LOG_LEVEL, LOG->GetLogLevel() == LOG_LEVEL_DEBUG);		
	PopupMenu(&popup_menu, in_event.GetPoint());
	in_event.Skip();
}

/** Установить уровень журналирования*/
void VLogViewer::OnSetLogLevel(wxCommandEvent & in_event)
{
	if (LOG->GetLogLevel() == LOG_LEVEL_DEBUG)
		LOG->SetLogLevel(LOG_LEVEL_INFO);
	else
		LOG->SetLogLevel(LOG_LEVEL_DEBUG);
	Refresh();
}

/** Обновление UI*/
void VLogViewer::OnUpdateUI(wxUpdateUIEvent& evt)
{
	Refresh();
}

/** Конструктор*/
VLogViewer::VLogViewer(wxWindow * in_parent, wxWindowID in_id) : wxListCtrl(in_parent,in_id,wxDefaultPosition,wxDefaultSize,wxLC_REPORT|wxLC_VIRTUAL)
{
	InsertColumn(0, _T("Время"));
	InsertColumn(1, _T("Сообщение"));			
	error_attr.SetTextColour(wxColour(128,0,0));		
	warning_attr.SetTextColour(wxColour(128,128,0));
	info_attr.SetTextColour(wxColour(0,0,128));
	
	popup_menu.AppendCheckItem(ID_SET_LOG_LEVEL, _T("Выводить в журнал отладочные сообщения"));	
	Connect(ID_SET_LOG_LEVEL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(VLogViewer::OnSetLogLevel));
}
/** Возвращает содержимое ячейки*/
wxString VLogViewer::OnGetItemText(long item, long column) const
{						
	switch (column)
	{
		case 0:
			return wxString(LOG->GetRecord(item).GetTime().ToWideStringLocale().c_str());
		case 1:
			return wxString(LOG->GetRecord(item).GetLogMessage().c_str());
		default:
			return _T("");
	}	
}
/** Возвращает аттрибуты ячейки*/
wxListItemAttr * VLogViewer::OnGetItemAttr(long item) const
{		
	switch (LOG->GetRecord(item).GetLevel())
	{			
		case LOG_LEVEL_ERROR:
			return const_cast<wxListItemAttr *>(&error_attr);
		case LOG_LEVEL_WARNING:
			return const_cast<wxListItemAttr *>(&warning_attr);
		case LOG_LEVEL_INFO:
			return const_cast<wxListItemAttr *>(&info_attr);
		case LOG_LEVEL_DEBUG:
			return const_cast<wxListItemAttr *>(&debug_attr);
		default:
			return NULL;
	}
}
/** Обновляет вид*/
void VLogViewer::Refresh()
{				
	if (GetItemCount()!=LOG->GetRecordCount())
	{
		wxWindowUpdateLocker(this);
		bool lastItemSelected = false;
		if (GetItemCount() == 0 || GetItemCount() > 0 && GetItemState(GetItemCount()-1, wxLIST_STATE_FOCUSED) == wxLIST_STATE_FOCUSED)
			lastItemSelected = true;		
		SetItemCount(LOG->GetRecordCount());
		SetColumnWidth(0, wxLIST_AUTOSIZE);
		SetColumnWidth(1, wxLIST_AUTOSIZE);
		
		if (GetItemCount()>0 && lastItemSelected)
		{
			EnsureVisible(GetItemCount()-1);
			this->SetItemState(GetItemCount()-1, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
		}
	}
}

//----------------------------------------------- Методы класса VCoordinatesThread ------------------------------------------
/// Конструктор
/*VCoordinatesThread::VCoordinatesThread(VMainFrame *handler, V4fSynchronization * _synchronization, std::string _ip_udp, int & _port_udp) : 
	wxThread(wxTHREAD_DETACHED), ip_udp(_ip_udp), port_udp(_port_udp)
{
	synchronization = _synchronization;
	ip_udp = _ip_udp;
	port_udp = _port_udp;
	m_pHandler = handler;
	subtitles_source = 0;
}

/// Функция потока
wxThread::ExitCode VCoordinatesThread::Entry()
{
	try
	{
		subtitles_source = new VNetworkSubtitles(ip_udp, port_udp);
	}
	catch(std::exception & e)
	{
		LOG_ERROR(L"Ошибка при создании объекта субтитров");
		return (wxThread::ExitCode) wxTHREAD_MISC_ERROR;
	}
	while (!TestDestroy())
	{
		CURRENT_POSITION().Lock();
		ExchangeCadre ex;
		{
			subtitles_source->GetData(ex);
		}
		if(ex.dir > 0)
		{
			CURRENT_POSITION().Value().km = ex.km;
			CURRENT_POSITION().Value().m = ex.m;
			CURRENT_POSITION().Value().absolutePosition = ex.absCoord;
			CURRENT_POSITION().Value().dir = ex.dir;
			CURRENT_POSITION().Value().dpp = ex.dpp;
			int check = ex.way[0];
			if (check >= '1' && check <= '9')
			{
				for (int i = 0; i < 8; i++)
					CURRENT_POSITION().Value().way[i] = ex.way[i];
			}
			else
				strcpy(CURRENT_POSITION().Value().way," NO WAY");
			if(*(ex.peregon))
				strncpy(CURRENT_POSITION().Value().peregon, ex.peregon, 79);
		}
		else
			CURRENT_POSITION().Value().SetDefaultValues();
		synchronization->PutMain(CURRENT_POSITION().Value());
		CURRENT_POSITION().Unlock();
		Sleep(1);
	}
	return (wxThread::ExitCode)0;
}

VCoordinatesThread::~VCoordinatesThread()
{
    wxCriticalSectionLocker enter(m_pHandler->m_pThreadCScoordinates);
	try
	{
		if(subtitles_source)
			delete subtitles_source;
	}
	catch(std::exception e)
	{
		LOG_ERROR(L"Ошибка при удалении объекта субтитров");
	}
    m_pHandler->coordinates = NULL;
}*/

/** Конструктор*/
VMainFrame::VMainFrame(wxWindow * parent, wxWindowID id, const wxString & title, const wxPoint & pos, const wxSize & size, long style)
	: wxFrame(parent, id, title, pos, size, style)
{
	VFileName log_file(LOG_FOLDER, L"video.log");
	LOG_INIT(log_file.GetPath(), LOG_LEVEL_DEBUG);

	wxFileConfig VideoConfig(wxEmptyString, wxEmptyString, VFileName(CONFIG_FOLDER, L".video").GetPath().c_str());
	//wxFileConfig VideoConfig(wxEmptyString, wxEmptyString, VFileName(L"E:\\.video").GetPath().c_str());

	wxString paramName, result;
	paramName = L"videoServer";
	if(VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().video_server = std::wstring(result.c_str());
	paramName = L"intervalRecording";
	if(VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().intervalRecording = atoi(result.c_str());
	paramName = L"showSubtitle";
	if(VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().showSubtitle = (bool) atoi(result.c_str());
	paramName = L"tmpltSubtitle";
	if(VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().tmpltSubtitle = std::wstring(result.c_str());
	paramName = L"locationSubtitle";
	if(VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().locationSubtitle = atoi(result.c_str());
	paramName = L"heightSubtitle";
	if(VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().heightSubtitle = atoi(result.c_str());
	paramName = L"shiftSubtitle";
	if(VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().shiftSubtitle = atoi(result.c_str());
	paramName = L"com";
	if(VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().com = wstring(result.c_str());
	paramName = L"server_db";
	if(VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().in_server_db = wstring(result.c_str());
	paramName = L"database";
	if(VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().in_database = wstring(result.c_str());
	paramName = L"ip_udp";
	if(VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().ip_udp = wstring(result.c_str());
	paramName = L"port_udp";
	if(VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().port_udp = _wtoi(result.c_str());
	paramName = L"shiftCoordinateViewer";
	if(VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().shiftCoordinateViewer = atoi(result.c_str());
	paramName = L"shiftCoordinateSpeedPositive";
	if (VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().shiftCoordinateSpeedPositive = atoi(result.c_str());
	paramName = L"shiftCoordinateSpeedNegative";
	if (VideoConfig.Read(paramName, &result))
		VIDEO_OPTIONS().Value().shiftCoordinateSpeedNegative = atoi(result.c_str());
	paramName = L"formatDpp";
	if (VideoConfig.Read(paramName, &result)) {
		bool new_format = (bool)atoi(result.c_str());
		if (new_format)
			VIDEO_OPTIONS().Value().formatDpp = 0;
		else
			VIDEO_OPTIONS().Value().formatDpp = 1;
	}
			
	if(VIDEO_OPTIONS().Value().in_server_db != L"" && VIDEO_OPTIONS().Value().in_database != L"")
		this->RegisteredDatabase = new VRegisteredDatabase(L"video_db", VIDEO_OPTIONS().Value().in_server_db, VIDEO_OPTIONS().Value().in_database, L"");
	else
		this->RegisteredDatabase = 0;

	//Создаём иконку в панели задач
	wxIcon video_icon(L"video.exe", wxBITMAP_TYPE_ICO);
	this->SetIcon(video_icon);

	wxFileConfig CameraConfig(wxEmptyString, wxEmptyString, VFileName(CONFIG_FOLDER, L".camera").GetPath().c_str());
	//Загрузить из конфига настройки камер
	int i = 0, version = -1;
	CameraConfig.Read(L"version", &version);
	if(version > -1)
	{
		paramName.Clear();
		paramName<<L"Camera"<<i;
		while(CameraConfig.Read(paramName, &result))
		{	
			i++;
			paramName.Clear();
			paramName<<L"Camera"<<i;
			std::vector<wstring> split_vec;
			boost::algorithm::split(split_vec, wstring(result.c_str()), boost::is_any_of(L";"));
			if(split_vec.size() > 0)
			{
				int typeId = atoi(wstring_to_string(split_vec[0]).c_str());		
				switch(typeId)
				{
					case RTSP_CAMERA:
					{
						VRTSPCameraProperties * camProps = new VRTSPCameraProperties(string_to_wstring((const char *) result.c_str()), version);
						cameraProperties.push_back(camProps);
						break;
					}
					case JAI_CAMERA:
					{
						VJAICameraProperties * camProps = new VJAICameraProperties(string_to_wstring((const char *) result.c_str()), version);
						cameraProperties.push_back(camProps);
						break;
					}
				}
			}
		}
	}
	cameraWndCounter = 1600;

	//CURRENT_POSITION().Value().SetDefaultValues();
	Connect(wxID_ANY, wxEVT_COMMAND_COORDTHREAD_COMPLETED, wxThreadEventHandler(VMainFrame::OnCompleteCoordThread), NULL, this);
	if(VIDEO_OPTIONS().Value().ip_udp != L"" && VIDEO_OPTIONS().Value().port_udp > -1)
	{
		coordinates = new VCoordinatesThread(this, wstring_to_string(VIDEO_OPTIONS().Value().ip_udp), VIDEO_OPTIONS().Value().port_udp);
		if(coordinates->Run() != wxTHREAD_NO_ERROR)
		{
			delete coordinates;
			coordinates = 0;
		}
	}
	else
		coordinates = 0;

	Connect(wxID_ANY, wxEVT_COMMAND_DPPTHREAD_COMPLETED, wxThreadEventHandler(VMainFrame::OnCompleteDppThread), NULL, this);
	if(VIDEO_OPTIONS().Value().com != L"")
	{
		dpp = new VCodeDPPThread(this, VIDEO_OPTIONS().Value().com);
		if(dpp->Run() != wxTHREAD_NO_ERROR)
		{
			delete dpp;
			dpp = 0;
		}
	}
	else
		dpp = 0;
	
	LogViewer=new VLogViewer(this, 1001);	
	SetupMenu();
}

/** Деструктор*/
VMainFrame::~VMainFrame()
{
	for(size_t i = 0; i < cameraProperties.size(); i++)
		delete cameraProperties[i];
	cameraProperties.clear();
	if(this->RegisteredDatabase)
		delete RegisteredDatabase;

	{
		//wxCriticalSectionLocker enter(m_pThreadCScoordinates);
		if (coordinates)
		{
			coordinates->TestDestroy();
			coordinates->Kill();
		}
	}
	/// Не доходим до этого места. Утечка и поломка порта
	{
		if (dpp)
		{
			//dpp->TestDestroy();
			dpp->Kill();
		}
	}
}

void VMainFrame::OnCompleteDppThread(wxThreadEvent & event)
{
	if (dpp)
	{
		dpp->Delete();
		dpp = 0;
	}
}

void VMainFrame::OnCompleteCoordThread(wxThreadEvent & event)
{
	if (coordinates) {
		coordinates->Delete();
		coordinates = 0; 
	}
}

/// Настраивает меню
void VMainFrame::SetupMenu()
{
	wxMenu * fileMenu = new wxMenu;
	fileMenu->Append(ID_PROPERTIES, L"Настройка программы");
	fileMenu->Append(ID_PROPERTIES_CAMS, L"Настройка камер");
	fileMenu->Append(ID_VIEW_V4F, L"Просмотр файла v4f");
	fileMenu->Append(ID_QUIT,L"Выход");	
	wxMenu * cams = new wxMenu;
	int ind = ID_CAM;
	for (size_t i = 0; i < cameraProperties.size(); i++)
	{
		cams->AppendCheckItem(ind + i,cameraProperties[i]->GetCameraName().c_str());
		Connect(ind + i, wxEVT_MENU, wxCommandEventHandler(VMainFrame::OnTurnCamera));
		Connect(ind + i, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(VMainFrame::OnUpdateUI));
		cameraControlsExisting.push_back(false);
		cameraWndId.push_back(-1);
	}
	wxMenu * helpMenu = new wxMenu();
	helpMenu->Append(ID_ABOUT, L"О программе");
	MenuBar = new wxMenuBar();
	MenuBar->Append(fileMenu,L"Файл");
	MenuBar->Append(cams,L"Камеры");
	MenuBar->Append(helpMenu,L"Справка");
	SetMenuBar(MenuBar);
}

void VMainFrame::UpdateList(wxCommandEvent &event)
{
	int countt = MenuBar->GetMenuCount();
	MenuBar->Remove(MenuBar->GetMenuCount() - 1);
	wxMenu * cams = new wxMenu;
	MenuBar->Append(cams, L"Камеры");
	Update();
	event.Skip();
}
/// Добавляет камеру
void VMainFrame::AddCamera(VCameraBase * in_camera_props, int cameraId)
{	
	try
	{
		cameraControlsExisting[cameraId] = false;
		VCameraControl * Camera=new VCameraControl(in_camera_props, this, cameraWndCounter, 
			(bool *) (cameraControlsExisting.begin() + cameraId)._Myptr);
		cameraWndId[cameraId] = cameraWndCounter;
		cameraWndCounter++;
		if (isConnected == true) 
			Camera->SetMinSize(Camera->GetBestSize());
		else
		{
			wxSize size(400,200);
			Camera->SetMinSize(size);
		}
		Camera->Show();
		Camera->Update();
	}
	catch (VException& e)
	{
		LOG_INFO(e.GetDescription().c_str());
	}
}


///Добавление камеры при выбирание из списка
void VMainFrame::OnTurnCamera(wxCommandEvent& event)
{	
	int ind = event.GetId() - ID_CAM;
	wstring msg = L"Выбрана камера: " + cameraProperties[ind]->GetCameraName();
	LOG_INFO(msg);
	int WindId = cameraWndId[ind];
	if (WindId < 0 || !cameraControlsExisting[ind])
		AddCamera(cameraProperties[ind], ind);
	else
		this->FindWindowById(WindId)->SetFocus();
	event.Skip();
}

void VMainFrame::OnProperties(wxCommandEvent& event)
{
	VMainPropDialog * dlg = new VMainPropDialog(this);
	if(dlg->ShowModal() == wxID_OK)
	{
		wxFileConfig VideoConfig(wxEmptyString, wxEmptyString, VFileName(CONFIG_FOLDER, L".video").GetPath().c_str());
		//wxFileConfig VideoConfig(wxEmptyString, wxEmptyString, VFileName(L".video").GetPath().c_str());
		VideoConfig.DeleteAll();
		VideoConfig.Write("videoServer", wstring_to_string(VIDEO_OPTIONS().Value().video_server).c_str());
		VideoConfig.Write("intervalRecording", VIDEO_OPTIONS().Value().intervalRecording);
		VideoConfig.Write("showSubtitle", VIDEO_OPTIONS().Value().showSubtitle);
		wxString tmpltSubtitle_str(wstring_to_string(VIDEO_OPTIONS().Value().tmpltSubtitle).c_str());
		VideoConfig.Write("tmpltSubtitle", tmpltSubtitle_str);
		VideoConfig.Write("locationSubtitle", VIDEO_OPTIONS().Value().locationSubtitle);
		VideoConfig.Write("heightSubtitle", VIDEO_OPTIONS().Value().heightSubtitle);
		VideoConfig.Write("shiftSubtitle", VIDEO_OPTIONS().Value().shiftSubtitle);
		wxString in_server_db_str(wstring_to_string(VIDEO_OPTIONS().Value().in_server_db).c_str());
		VideoConfig.Write("server_db", in_server_db_str);
		wxString in_database_str(wstring_to_string(VIDEO_OPTIONS().Value().in_database).c_str());
		VideoConfig.Write("database", in_database_str);
		wxString com_str(wstring_to_string(VIDEO_OPTIONS().Value().com).c_str());
		VideoConfig.Write("com", com_str);
		wxString ip_udp_str(wstring_to_string(VIDEO_OPTIONS().Value().ip_udp).c_str());
		VideoConfig.Write("ip_udp", ip_udp_str);
		VideoConfig.Write("port_udp", VIDEO_OPTIONS().Value().port_udp);
		VideoConfig.Write("shiftCoordinateViewer", VIDEO_OPTIONS().Value().shiftCoordinateViewer);
		VideoConfig.Write("shiftCoordinateSpeedPositive", VIDEO_OPTIONS().Value().shiftCoordinateSpeedPositive);
		VideoConfig.Write("shiftCoordinateSpeedNegative", VIDEO_OPTIONS().Value().shiftCoordinateSpeedNegative);
		VideoConfig.Write("formarDpp", (int) VIDEO_OPTIONS().Value().formatDpp);
		//Проверка подключения к БД
		if(this->RegisteredDatabase)
			delete RegisteredDatabase;
		if(VIDEO_OPTIONS().Value().in_server_db != L"" && VIDEO_OPTIONS().Value().in_database != L"")
			this->RegisteredDatabase = new VRegisteredDatabase(L"video_db", VIDEO_OPTIONS().Value().in_server_db, 
				VIDEO_OPTIONS().Value().in_database, L"");
		else
			this->RegisteredDatabase = 0;
		if(VIDEO_OPTIONS().Value().ip_udp != L"" && VIDEO_OPTIONS().Value().port_udp > -1)
		{
			if(coordinates)
			{
				//wxCriticalSectionLocker enter(m_pThreadCScoordinates);
				coordinates->Delete();
				coordinates = 0;
			}
			coordinates = new VCoordinatesThread(this, wstring_to_string(VIDEO_OPTIONS().Value().ip_udp), VIDEO_OPTIONS().Value().port_udp);
			if(coordinates->Run() != wxTHREAD_NO_ERROR)
			{
				delete coordinates;
				coordinates = 0;
			}
		}
		else
		{
			if(coordinates)
				coordinates->Delete();
			coordinates = 0;
		}
		//!!!Времянка, чтобы изменить настройки. Неправильно закрывается поток
		/*if(com != L"")
		{
			if(dpp)
			{
				//wxCriticalSectionLocker enter(m_pThreadCScoordinates);
				dpp->Delete();
				dpp = 0;
			}
			dpp = new VCodeDPPThread(this, com);
			if(dpp->Run() != wxTHREAD_NO_ERROR)
			{
				delete dpp;
				dpp = 0;
			}
		}
		else
		{
			if(dpp)
				dpp->Delete();
			dpp = 0;
		}*/
	}
	event.Skip();
}

/// Диалог настройки камер
void VMainFrame::OnPropertiesCams(wxCommandEvent& event)
{
	int oldCountProps = cameraProperties.size();
	VCameraPropDialog * prop = new VCameraPropDialog(this, cameraProperties);
	if(prop->ShowModal() == wxID_OK)
	{
		for(size_t i = 0; i < cameraProperties.size(); i++)
			delete cameraProperties[i];
		cameraProperties.clear();
		for(size_t i = 0; i < prop->GetCameraProperties().size(); i++)
		{
			cameraProperties.push_back(prop->GetCameraProperties()[i]);
		}

		int ind = ID_CAM;
		for (size_t i = 0; i < oldCountProps; i++)
		{
			Disconnect(ind + i, wxEVT_MENU, wxCommandEventHandler(VMainFrame::OnTurnCamera));
			Disconnect(ind + i, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(VMainFrame::OnUpdateUI));
		}
		
		MenuBar->Remove(1);
		cameraControlsExisting.clear();
		cameraWndId.clear();
		wxMenu * cams = new wxMenu;
		for (size_t i = 0; i < cameraProperties.size(); i++)
		{
			cams->AppendCheckItem(ind + i,cameraProperties[i]->GetCameraName().c_str());
			Connect(ind + i, wxEVT_MENU, wxCommandEventHandler(VMainFrame::OnTurnCamera));
			Connect(ind + i, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(VMainFrame::OnUpdateUI));
			cameraControlsExisting.push_back(false);
			cameraWndId.push_back(-1);
		}
		MenuBar->Insert(1, cams, L"Камеры");

		//Сохраняем в конфиг свойства камер
		wxFileConfig CameraConfig(wxEmptyString, wxEmptyString, VFileName(CONFIG_FOLDER, L".camera").GetPath().c_str());
		//wxFileConfig CameraConfig(wxEmptyString, wxEmptyString, VFileName(L"C:\\.camera").GetPath().c_str());
		if(cameraProperties.size() > 0)
		{
			CameraConfig.DeleteAll();
			CameraConfig.Write(L"version", CAMERA_CONFIG);
			wxString paramName;
			for(size_t i = 0; i < cameraProperties.size(); i++)
			{
				paramName.Clear();
				paramName<<"Camera"<<i;
				wxString conf_str(wstring_to_string(cameraProperties[i]->GetConfigStr().c_str()).c_str());
				CameraConfig.Write(paramName.c_str(), conf_str);
			}
		}
	}
}

void VMainFrame::OnViewC4v(wxCommandEvent& event)
{
	wxFileDialog dlg(this, L"Открыть файл...", L"", L"", L"Файлы V4F (*.v4f)|*.v4f");
	if (dlg.ShowModal()==wxID_OK)
	{
		//VCameraBase * camera_props = new VRTSPCameraProperties(L"", -1);
		V4fViewerDialog * viewer = new V4fViewerDialog(this, wstring_to_string((const wchar_t *) dlg.GetPath().c_str()), RegisteredDatabase);
		viewer->ShowModal();
	}
}

/** Пункт меню "Выход" */
void VMainFrame::OnQuit(wxCommandEvent& event)
{	
	Close(true);
}

/** Пункт меню "О программе" */
void VMainFrame::OnAbout(wxCommandEvent& event)
{

	wxString about = wxString::Format(L"Программа Видео\n"
#ifdef _DEBUG
		L"Ревизия %s [отладочная сборка]\n"
#else
		L"Ревизия %s\n"
#endif
		L"Сборка от %s - %s\n"
		L"wxWidgets %d.%d.%d\n"
		L"OpenCV %d.%d.%d", 
		revision, string_to_wstring(__DATE__).c_str(), string_to_wstring(__TIME__).c_str(), 
		wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER,
		CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);
	wxMessageBox(about, L"О программе", wxOK);
}

///Закрытие окна
void VMainFrame::OnClose(wxCloseEvent& event)
{
	Destroy();
}

/// Обновление UI
void VMainFrame::OnUpdateUI(wxUpdateUIEvent & event)
{
	event.Skip();
}

/*void VMainFrame::OnCoordinatesThreadCompletion(wxThreadEvent&)
{
	LOG_INFO(L"Завершилась работа с потоком UDP");
}*/

/** Метод, реализующий запуск приложения*/
bool VApp::OnInit()
{
	wxInitAllImageHandlers();
	//InitCriticalSection();
	//Устанавливаем локаль
	setlocale(LC_ALL, "C");
	std::locale::global(std::locale("C"));		
	wxHandleFatalExceptions(false);
	VMainFrame * frame = new VMainFrame(NULL, wxID_ANY, L"Видео");		

	SetTopWindow(frame);
	frame->Show();
	frame->Maximize();
	return true;	
}