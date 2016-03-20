#include "VMainPropDialog.h"

BEGIN_EVENT_TABLE(VMainPropDialog,wxDialog)
	EVT_TEXT(ID_VIDEO_SERVER, VMainPropDialog::UpdateVideoServer)
	EVT_SPINCTRL(ID_INTERVAL_RECORDING, VMainPropDialog::UpdateInetrvalRecording)
	EVT_TEXT(ID_TMPLT_SUBTITLE, VMainPropDialog::UpdateTmpltSubtitle)
	EVT_TEXT(ID_HEIGHT_SUBTITLE, VMainPropDialog::UpdateHeightSubtitle)
	EVT_TEXT(ID_SHIFT_SUBTITLE, VMainPropDialog::UpdateShiftSubtitle)
	EVT_TEXT(ID_COM, VMainPropDialog::UpdateCom)
	EVT_TEXT(ID_HOST, VMainPropDialog::UpdateHost)
	EVT_TEXT(ID_DBFILENAME, VMainPropDialog::UpdateFileDb)
	EVT_TEXT(ID_IPUDP, VMainPropDialog::UpdateIpUdp)
	EVT_TEXT(ID_PORTUDP, VMainPropDialog::UpdatePortUdp)
	EVT_TEXT(ID_SHIFT_COORDINATE_VIEWER, VMainPropDialog::UpdateShiftCoordinateViewer)
	EVT_TEXT(ID_SHIFT_COORDINATE_SPEED_POSITIVE, VMainPropDialog::UpdateShiftCoordinateSpeedPositive)
	EVT_TEXT(ID_SHIFT_COORDINATE_SPEED_NEGATIVE, VMainPropDialog::UpdateShiftCoordinateSpeedNegative)
	EVT_RADIOBUTTON(ID_LOCATION_BOTTOM, VMainPropDialog::UpdateLocationSubtitle)
	EVT_RADIOBUTTON(ID_LOCATION_TOP, VMainPropDialog::UpdateLocationSubtitle)
	EVT_BUTTON(wxID_OK, VMainPropDialog::OnOkButton)
	EVT_BUTTON(wxID_CANCEL, VMainPropDialog::OnCancelButton)
	EVT_UPDATE_UI(ID_SHOWSUBTITLE,VMainPropDialog::OnUpdateUI)
	EVT_RADIOBUTTON(ID_FORMAT_DPP_NEW, VMainPropDialog::UpdateFormatDpp)
	EVT_RADIOBUTTON(ID_FORMAT_DPP_OLD, VMainPropDialog::UpdateFormatDpp)
END_EVENT_TABLE()

VMainPropDialog::VMainPropDialog(wxWindow * parent)
	: wxDialog(parent, wxID_ANY, L"Настройка программы видеонаблюдения", wxDefaultPosition, wxSize(400, 800), wxDEFAULT_DIALOG_STYLE)
{
	prev_intervalRecording = VIDEO_OPTIONS().Value().intervalRecording;
	prev_showSubtitle = VIDEO_OPTIONS().Value().showSubtitle;
	prev_tmpltSubtitle = VIDEO_OPTIONS().Value().tmpltSubtitle;
	prev_locationSubtitle = VIDEO_OPTIONS().Value().locationSubtitle;
	prev_heightSubtitle = VIDEO_OPTIONS().Value().heightSubtitle;
	prev_shiftSubtitle = VIDEO_OPTIONS().Value().shiftSubtitle;
	prev_com = VIDEO_OPTIONS().Value().com;
	prev_in_server_db = VIDEO_OPTIONS().Value().in_server_db;
	prev_in_database = VIDEO_OPTIONS().Value().in_database;
	prev_ip_udp = VIDEO_OPTIONS().Value().ip_udp;
	prev_port_udp = VIDEO_OPTIONS().Value().port_udp;
	prev_shiftCoordinateViewer = VIDEO_OPTIONS().Value().shiftCoordinateViewer;
	prev_shiftCoordinateSpeedPositive = VIDEO_OPTIONS().Value().shiftCoordinateSpeedPositive;
	prev_shiftCoordinateSpeedNegative = VIDEO_OPTIONS().Value().shiftCoordinateSpeedNegative;
	prev_formatDpp = VIDEO_OPTIONS().Value().formatDpp;

	wxBoxSizer * mainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer * videoServerSizer = new wxStaticBoxSizer(wxHORIZONTAL, this , L"Директория видеосервера");
	textCtrlVideoServer = new wxTextCtrl(this, ID_VIDEO_SERVER, L"", wxDefaultPosition, wxSize(260,20));
	textCtrlVideoServer->SetValue(VIDEO_OPTIONS().Value().video_server.c_str());
	videoServerSizer->Add(textCtrlVideoServer, 0, wxALIGN_RIGHT, 5);
	mainSizer->Add(videoServerSizer, 0, wxALL, 5);

	wxStaticBoxSizer * intSizer = new wxStaticBoxSizer(wxHORIZONTAL, this , L"Интервал съёмки");
	wxStaticText * intText = new wxStaticText(this, wxID_ANY, L"Интервал [мин(avi)/метров(jpg)]");
	intSizer->Add(intText, 0, wxALL , 5);
	spinIntRecord = new wxSpinCtrl(this, ID_INTERVAL_RECORDING);
	spinIntRecord->SetValue(VIDEO_OPTIONS().Value().intervalRecording);
	intSizer->Add(spinIntRecord, 0, wxALL|wxALIGN_RIGHT, 5);
	mainSizer->Add(intSizer, 0, wxALL, 5);

	showSubtitleBox = new wxCheckBox(this, ID_SHOWSUBTITLE, L"Показывать субтитры");
	showSubtitleBox->SetValue(VIDEO_OPTIONS().Value().showSubtitle);
	mainSizer->Add(showSubtitleBox, 0, wxALL, 5);

	wxStaticBoxSizer * tmpltSizer = new wxStaticBoxSizer(wxHORIZONTAL, this , L"Шаблон субтитров");
	textCtrlTmplt = new wxTextCtrl(this, ID_TMPLT_SUBTITLE, L"", wxDefaultPosition, wxSize(260,20));
	textCtrlTmplt->SetValue(VIDEO_OPTIONS().Value().tmpltSubtitle.c_str());
	tmpltSizer->Add(textCtrlTmplt, 0, wxALIGN_RIGHT, 5);
	mainSizer->Add(tmpltSizer, 0, wxALL, 5);

	wxStaticBoxSizer * heightSizer = new wxStaticBoxSizer(wxHORIZONTAL, this , L"Высота субтитров");
	textCtrlHeight = new wxTextCtrl(this, ID_HEIGHT_SUBTITLE, L"", wxDefaultPosition, wxSize(120,20));
	wchar_t heightStr[5];
	swprintf(heightStr, 4, L"%i", VIDEO_OPTIONS().Value().heightSubtitle);
	textCtrlHeight->SetValue(heightStr);
	heightSizer->Add(textCtrlHeight, 0, wxALIGN_RIGHT, 5);
	mainSizer->Add(heightSizer, 0, wxALL, 5);

	wxStaticBoxSizer * shiftSubtitleSizer = new wxStaticBoxSizer(wxHORIZONTAL, this , L"Смещение субтитров");
	textCtrlShiftSubtitle = new wxTextCtrl(this, ID_SHIFT_SUBTITLE, L"", wxDefaultPosition, wxSize(120,20));
	wchar_t shiftStr[10] = L"";
	swprintf(shiftStr, 4, L"%i", VIDEO_OPTIONS().Value().shiftSubtitle);
	textCtrlShiftSubtitle->SetValue(shiftStr);
	shiftSubtitleSizer->Add(textCtrlShiftSubtitle, 0, wxALIGN_RIGHT, 5);
	mainSizer->Add(shiftSubtitleSizer, 0, wxALL, 5);

	wxStaticBoxSizer * comSizer = new wxStaticBoxSizer(wxHORIZONTAL, this , L"COM-порт для ДПП");
	textCtrlCom = new wxTextCtrl(this, ID_COM, L"", wxDefaultPosition, wxSize(120,20));
	textCtrlCom->SetValue(VIDEO_OPTIONS().Value().com.c_str());
	comSizer->Add(textCtrlCom, 0, wxALIGN_RIGHT, 5);
	mainSizer->Add(comSizer, 0, wxALL, 5);

	wxStaticBoxSizer * dbSizer = new wxStaticBoxSizer(wxHORIZONTAL, this , L"База данных");
	textCtrlDbHost = new wxTextCtrl(this, ID_HOST, L"", wxDefaultPosition, wxSize(120,20));
	textCtrlDbHost->SetValue(VIDEO_OPTIONS().Value().in_server_db.c_str());
	dbSizer->Add(textCtrlDbHost, 0, wxALIGN_RIGHT, 5);
	textCtrlDbFile = new wxTextCtrl(this, ID_DBFILENAME, L"", wxDefaultPosition, wxSize(200,20));
	textCtrlDbFile->SetValue(VIDEO_OPTIONS().Value().in_database.c_str());
	dbSizer->Add(textCtrlDbFile, 0, wxALIGN_RIGHT, 5);
	mainSizer->Add(dbSizer, 0, wxALL, 5);

	wxStaticBoxSizer * udpSizer = new wxStaticBoxSizer(wxHORIZONTAL, this , L"UDP");
	textCtrlIpUdp = new wxTextCtrl(this, ID_IPUDP, L"", wxDefaultPosition, wxSize(250,20));
	textCtrlIpUdp->SetValue(VIDEO_OPTIONS().Value().ip_udp.c_str());
	udpSizer->Add(textCtrlIpUdp, 0, wxALIGN_RIGHT, 5);
	textCtrlPortUdp = new wxTextCtrl(this, ID_PORTUDP, L"", wxDefaultPosition, wxSize(70,20));
	wchar_t port_udp_str[20] = L"";
	swprintf(port_udp_str, 19, L"%i", VIDEO_OPTIONS().Value().port_udp);
	textCtrlPortUdp->SetValue(port_udp_str);
	udpSizer->Add(textCtrlPortUdp, 0, wxALIGN_RIGHT, 5);
	mainSizer->Add(udpSizer, 0, wxALL, 5);

	wxStaticBoxSizer * locationSizer = new wxStaticBoxSizer(wxHORIZONTAL, this , L"Положение субтитров");
	locationRdBtnBottom = new wxRadioButton(this, ID_LOCATION_BOTTOM, L"Внизу");
	locationRdBtnTop = new wxRadioButton(this, ID_LOCATION_TOP, L"Вверху");
	if(VIDEO_OPTIONS().Value().locationSubtitle == wxTOP)
		locationRdBtnTop->SetValue(true);
	else
		locationRdBtnBottom->SetValue(true);
	locationSizer->Add(locationRdBtnBottom, 0, wxALL|wxALIGN_RIGHT, 5);
	locationSizer->Add(locationRdBtnTop, 0, wxALL|wxALIGN_RIGHT, 5);
	mainSizer->Add(locationSizer, 0, wxALL, 5);

	wxStaticBoxSizer * shiftCoordinateSizer = new wxStaticBoxSizer(wxHORIZONTAL, this , L"Смещение координаты [м]");	
	wxStaticText * shiftCoordinateViewerText = new wxStaticText(this, wxID_ANY, L"Просмотрщик");
	textCtrlShiftCoordinateViewer = new wxTextCtrl(this, ID_SHIFT_COORDINATE_VIEWER, L"", wxDefaultPosition, wxSize(50,20));
	wcsncpy(shiftStr, L"", 5);
	_snwprintf(shiftStr, 9, L"%.1f", VIDEO_OPTIONS().Value().shiftCoordinateViewer);
	textCtrlShiftCoordinateViewer->SetValue(shiftStr);	
	shiftCoordinateSizer->Add(shiftCoordinateViewerText, 0, wxALIGN_RIGHT, 5);
	shiftCoordinateSizer->Add(textCtrlShiftCoordinateViewer, 0, wxALIGN_RIGHT, 5);

	wxStaticText * shiftCoordinateSpeedPositiveText = new wxStaticText(this, wxID_ANY, L"Скорость+");
	textCtrlShiftCoordinateSpeedPositive = new wxTextCtrl(this, ID_SHIFT_COORDINATE_SPEED_POSITIVE, L"", wxDefaultPosition, wxSize(50, 20));
	wcsncpy(shiftStr, L"", 5);
	_snwprintf(shiftStr, 9, L"%.1f", VIDEO_OPTIONS().Value().shiftCoordinateSpeedPositive);
	textCtrlShiftCoordinateSpeedPositive->SetValue(shiftStr);
	shiftCoordinateSizer->Add(shiftCoordinateSpeedPositiveText, 0, wxALIGN_RIGHT, 5);
	shiftCoordinateSizer->Add(textCtrlShiftCoordinateSpeedPositive, 0, wxALIGN_RIGHT, 5);

	wxStaticText * shiftCoordinateSpeedNegativeText = new wxStaticText(this, wxID_ANY, L"Скорость-");
	textCtrlShiftCoordinateSpeedNegative = new wxTextCtrl(this, ID_SHIFT_COORDINATE_SPEED_NEGATIVE, L"", wxDefaultPosition, wxSize(50, 20));
	wcsncpy(shiftStr, L"", 5);
	_snwprintf(shiftStr, 9, L"%.1f", VIDEO_OPTIONS().Value().shiftCoordinateSpeedNegative);
	textCtrlShiftCoordinateSpeedNegative->SetValue(shiftStr);
	shiftCoordinateSizer->Add(shiftCoordinateSpeedNegativeText, 0, wxALIGN_RIGHT, 5);
	shiftCoordinateSizer->Add(textCtrlShiftCoordinateSpeedNegative, 0, wxALIGN_RIGHT, 5);
	mainSizer->Add(shiftCoordinateSizer, 0, wxALL, 5);

	wxStaticBoxSizer * formatDppSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, L"Протокол кода ДПП");
	formatDppRdBtnNew = new wxRadioButton(this, ID_FORMAT_DPP_NEW, L"Новый");
	formatDppRdBtnOld = new wxRadioButton(this, ID_FORMAT_DPP_OLD, L"Старый");
	if (VIDEO_OPTIONS().Value().formatDpp == 0)
		formatDppRdBtnNew->SetValue(true);
	else
		formatDppRdBtnOld->SetValue(true);
	formatDppSizer->Add(formatDppRdBtnNew, 0, wxALL | wxALIGN_RIGHT, 5);
	formatDppSizer->Add(formatDppRdBtnOld, 0, wxALL | wxALIGN_RIGHT, 5);
	mainSizer->Add(formatDppSizer, 0, wxALL, 5);

	wxBoxSizer * buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	okButton = new wxButton(this, wxID_OK, L"OK");
	cancelButton = new wxButton(this, wxID_CANCEL, L"Отмена");
	buttonSizer->Add(okButton, 0, wxALL|wxALIGN_RIGHT, 10);
	buttonSizer->Add(cancelButton, 0, wxALL|wxALIGN_RIGHT, 10);
	mainSizer->Add(buttonSizer, 0, wxALL|wxALIGN_RIGHT, 10);
	SetSizer(mainSizer);
}

void VMainPropDialog::OnUpdateUI(wxUpdateUIEvent & event)
{
	switch(event.GetId())
	{
	case ID_SHOWSUBTITLE:
		if(showSubtitleBox->GetValue())
			VIDEO_OPTIONS().Value().showSubtitle = true;
		else
			VIDEO_OPTIONS().Value().showSubtitle = false;
		break;
	}
	event.Skip();
}

void VMainPropDialog::UpdateVideoServer(wxCommandEvent & event)
{
	VIDEO_OPTIONS().Value().video_server = std::wstring(textCtrlVideoServer->GetValue().c_str());
	event.Skip();
}

void VMainPropDialog::UpdateInetrvalRecording(wxSpinEvent & event)
{
	VIDEO_OPTIONS().Value().intervalRecording = event.GetPosition();
	event.Skip();
}

void VMainPropDialog::UpdateTmpltSubtitle(wxCommandEvent & event)
{
	VIDEO_OPTIONS().Value().tmpltSubtitle = std::wstring(textCtrlTmplt->GetValue().c_str());
	event.Skip();
}

void VMainPropDialog::UpdateHeightSubtitle(wxCommandEvent & event)
{
	try
	{
		VIDEO_OPTIONS().Value().heightSubtitle = _wtoi(textCtrlHeight->GetValue().c_str());
	}
	catch(...)
	{
		wxMessageBox(L"Введите число", L"Ошибка", wxOK|wxICON_ERROR);
		return;
	}
	event.Skip();
}

void VMainPropDialog::UpdateShiftSubtitle(wxCommandEvent & event)
{
	try
	{
		VIDEO_OPTIONS().Value().shiftSubtitle = _wtoi(textCtrlShiftSubtitle->GetValue().c_str());
	}
	catch(...)
	{
		wxMessageBox(L"Введите число", L"Ошибка", wxOK|wxICON_ERROR);
		return;
	}
	event.Skip();
}

void VMainPropDialog::UpdateLocationSubtitle(wxCommandEvent & event)
{
	if(locationRdBtnBottom->GetValue())
	{
		VIDEO_OPTIONS().Value().locationSubtitle = wxBOTTOM;
		locationRdBtnTop->SetValue(false);
	}
	if(locationRdBtnTop->GetValue())
	{
		VIDEO_OPTIONS().Value().locationSubtitle = wxTOP;
		locationRdBtnBottom->SetValue(false);
	}
	event.Skip();
}

void VMainPropDialog::UpdateFormatDpp(wxCommandEvent & event)
{
	if (formatDppRdBtnNew->GetValue())
	{
		VIDEO_OPTIONS().Value().formatDpp = 0;
		formatDppRdBtnOld->SetValue(false);
	}
	if (formatDppRdBtnOld->GetValue())
	{
		VIDEO_OPTIONS().Value().formatDpp = 1;
		formatDppRdBtnNew->SetValue(false);
	}
	event.Skip();
}

void VMainPropDialog::UpdateCom(wxCommandEvent & event)
{
	VIDEO_OPTIONS().Value().com = std::wstring(textCtrlCom->GetValue().c_str());
	event.Skip();
}

void VMainPropDialog::UpdateHost(wxCommandEvent & event)
{
	VIDEO_OPTIONS().Value().in_server_db = std::wstring(textCtrlDbHost->GetValue().c_str());
	event.Skip();
}

void VMainPropDialog::UpdateFileDb(wxCommandEvent & event)
{
	VIDEO_OPTIONS().Value().in_database = std::wstring(textCtrlDbFile->GetValue().c_str());
	event.Skip();
}

void VMainPropDialog::UpdateIpUdp(wxCommandEvent & event)
{
	VIDEO_OPTIONS().Value().ip_udp = std::wstring(textCtrlIpUdp->GetValue().c_str());
	event.Skip();
}

void VMainPropDialog::UpdatePortUdp(wxCommandEvent & event)
{
	try
	{
		VIDEO_OPTIONS().Value().port_udp = _wtoi(textCtrlPortUdp->GetValue().c_str());
	}
	catch(std::exception e)
	{
		LOG_ERROR(string_to_wstring(e.what()));
	}
	event.Skip();
}

void VMainPropDialog::UpdateShiftCoordinateViewer(wxCommandEvent & event)
{
	try
	{
		VIDEO_OPTIONS().Value().shiftCoordinateViewer = _wtof(textCtrlShiftCoordinateViewer->GetValue().c_str());
	}
	catch(std::exception e)
	{
		LOG_ERROR(string_to_wstring(e.what()));
	}
	event.Skip();
}

void VMainPropDialog::UpdateShiftCoordinateSpeedPositive(wxCommandEvent & event)
{
	try
	{
		VIDEO_OPTIONS().Value().shiftCoordinateSpeedPositive = _wtof(textCtrlShiftCoordinateSpeedPositive->GetValue().c_str());
	}
	catch (std::exception e)
	{
		LOG_ERROR(string_to_wstring(e.what()));
	}
	event.Skip();
}

void VMainPropDialog::UpdateShiftCoordinateSpeedNegative(wxCommandEvent & event)
{
	try
	{
		VIDEO_OPTIONS().Value().shiftCoordinateSpeedNegative = _wtof(textCtrlShiftCoordinateSpeedNegative->GetValue().c_str());
	}
	catch (std::exception e)
	{
		LOG_ERROR(string_to_wstring(e.what()));
	}
	event.Skip();
}
///Обработка кнопки ОК
void VMainPropDialog::OnOkButton(wxCommandEvent &event)
{
	if(VIDEO_OPTIONS().Value().video_server != prev_video_server || VIDEO_OPTIONS().Value().intervalRecording != prev_intervalRecording
		|| VIDEO_OPTIONS().Value().showSubtitle != prev_showSubtitle || VIDEO_OPTIONS().Value().tmpltSubtitle != prev_tmpltSubtitle 
		|| VIDEO_OPTIONS().Value().locationSubtitle != prev_locationSubtitle || VIDEO_OPTIONS().Value().heightSubtitle != prev_heightSubtitle 
		|| VIDEO_OPTIONS().Value().shiftSubtitle != prev_shiftSubtitle || VIDEO_OPTIONS().Value().com != prev_com || VIDEO_OPTIONS().Value().in_server_db != prev_in_server_db
		|| VIDEO_OPTIONS().Value().in_database != prev_in_database || VIDEO_OPTIONS().Value().ip_udp != prev_ip_udp || VIDEO_OPTIONS().Value().port_udp != prev_port_udp
		|| VIDEO_OPTIONS().Value().shiftCoordinateViewer != prev_shiftCoordinateViewer || VIDEO_OPTIONS().Value().shiftCoordinateSpeedPositive != prev_shiftCoordinateSpeedPositive
		|| VIDEO_OPTIONS().Value().shiftCoordinateSpeedNegative != prev_shiftCoordinateSpeedNegative
		|| VIDEO_OPTIONS().Value().formatDpp != prev_formatDpp)
	{
		LOG_INFO(L"Настройки видео изменились");
		EndModal(wxID_OK);
	}
	else
		EndModal(wxID_CANCEL);
}

///Обработка кнопки Отмена
void VMainPropDialog::OnCancelButton(wxCommandEvent &event)
{
	VIDEO_OPTIONS().Value().video_server = prev_video_server;
	VIDEO_OPTIONS().Value().intervalRecording = prev_intervalRecording;
	VIDEO_OPTIONS().Value().showSubtitle = prev_showSubtitle;
	VIDEO_OPTIONS().Value().tmpltSubtitle = prev_tmpltSubtitle;
	VIDEO_OPTIONS().Value().locationSubtitle = prev_locationSubtitle;
	VIDEO_OPTIONS().Value().heightSubtitle = prev_heightSubtitle;
	VIDEO_OPTIONS().Value().shiftSubtitle = prev_shiftSubtitle;
	VIDEO_OPTIONS().Value().com = prev_com;
	VIDEO_OPTIONS().Value().in_server_db = prev_in_server_db;
	VIDEO_OPTIONS().Value().in_database = prev_in_database;
	VIDEO_OPTIONS().Value().ip_udp = prev_ip_udp;
	VIDEO_OPTIONS().Value().port_udp = prev_port_udp;
	VIDEO_OPTIONS().Value().shiftCoordinateViewer = prev_shiftCoordinateViewer;
	VIDEO_OPTIONS().Value().shiftCoordinateSpeedNegative = prev_shiftCoordinateSpeedNegative;
	VIDEO_OPTIONS().Value().shiftCoordinateSpeedPositive = prev_shiftCoordinateSpeedPositive;
	VIDEO_OPTIONS().Value().formatDpp = prev_formatDpp;

	EndModal(wxID_CANCEL);
}