#include "V4fViewerDialog.h"

//Диалог просмотра кадров из файла v4f
BEGIN_EVENT_TABLE(V4fViewerDialog, wxDialog)
	EVT_UPDATE_UI(ID_V4F_VIEWER_SLIDER,V4fViewerDialog::OnUpdateUI)
	EVT_TEXT(ID_V4F_VIEWER_KMCTRL, V4fViewerDialog::OnWayCoordUpdate)
	EVT_TEXT(ID_V4F_VIEWER_MCTRL, V4fViewerDialog::OnWayCoordUpdate)
	EVT_BUTTON(ID_V4F_VIEWER_LEFT, V4fViewerDialog::OnLeftButton)
	EVT_BUTTON(ID_V4F_VIEWER_RIGHT, V4fViewerDialog::OnRightButton)
	EVT_BUTTON(ID_V4F_VIEWER_EXPCURIMG, V4fViewerDialog::OnExportCurrentImg)
	EVT_BUTTON(ID_V4F_VIEWER_EXPAVI, V4fViewerDialog::OnExportToAvi)
	EVT_PAINT(V4fViewerDialog::OnPaint)
END_EVENT_TABLE()

V4fViewerDialog::V4fViewerDialog(wxWindow * parent, std::string fileName, VRegisteredDatabase * RegisteredDatabase) : 
	wxDialog(parent, wxID_ANY, L"Просмотр кадров", wxDefaultPosition, wxSize(510, 510), wxDEFAULT_DIALOG_STYLE), info(), mainBitmap()
{
	subtitleMemoryDC = new wxMemoryDC();

	//Загрузка информации из файла (грузим только номер кадра и позицию в файле)
	bufData = new unsigned char [921600];
	bufDataLen = 921600;

	subtitleBitmap = 0;
	dataSet = new V4fDataSet(fileName, "");
	if(dataSet)
	{
		dataSet->OpenReadFile();
		dataSet->ReadHeader();
		dataSet->LoadFrames();
	}

	try
	{
		//Загружаем априорную инофрмацию
		//Подключение к БД
		if (RegisteredDatabase->GetConnectString()!=L"")
		{
			db = VIBDatabasePtr(new VIBDatabase);
			try
			{
				ConnectDB(this->db, *RegisteredDatabase);
			}
			catch(VException &e)
			{
				wchar_t msg [1000] = L"";
				_snwprintf(msg, 999, L"Не удалось подключиться к БД: %s", RegisteredDatabase);
				LOG_ERROR(msg);
				_snwprintf(msg, 999, L"Details: %s", e.GetDetails());
				LOG_ERROR(msg);
			}
		}
		info.Makeinfo(*db.get(), dataSet->GetDir(), std::string(dataSet->GetWay()), VWaySection(0, 0.0f, 99999, 0.0f), VTime(dataSet->GetTime()), false, false);
	}
	catch(Exception)
	{
		LOG_ERROR(L"Ошибка при загрузке aprinfo");
	}

	currentPosSlider = -1;

	wxBoxSizer * mainSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer * topSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBoxSizer * screenSizer = new wxStaticBoxSizer(wxHORIZONTAL, this);
	screen = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(640, 480));
	screenSizer->Add(screen, 0, wxALL, 0);
	topSizer->Add(screen, 1, wxALL | wxEXPAND, 10);

	wxBoxSizer * buttonSizer = new wxBoxSizer(wxVERTICAL);
	exportToAVI = new wxButton(this, ID_V4F_VIEWER_EXPALLIMG, L"Экспорт в .avi", wxDefaultPosition, wxSize(140, 25));
	buttonSizer->Add(exportToAVI, 0, wxALL, 10);
	exportCurrentImg = new wxButton(this, ID_V4F_VIEWER_EXPCURIMG, L"Экспорт текущего кадра", wxDefaultPosition, wxSize(140, 25));
	buttonSizer->Add(exportCurrentImg, 0, wxALL, 10);
	topSizer->Add(buttonSizer, 0, wxALL, 10);

	wxBoxSizer * bottomSizer = new wxBoxSizer(wxHORIZONTAL);
	slider = new wxSlider(this, ID_V4F_VIEWER_SLIDER, 0, 0, dataSet->GetFrames().size()-1, wxDefaultPosition, wxSize(screen->GetSize().GetX(), 30));
	bottomSizer->Add(slider, 0, wxALL, 10);

	wxBoxSizer * textCtrlSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer * kmmSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText * kmText = new wxStaticText(this, wxID_ANY, L"км");
	kmmSizer->Add(kmText, 0, wxALL, 5);

	kmCtrl = new wxTextCtrl(this, ID_V4F_VIEWER_KMCTRL, L"", wxDefaultPosition, wxSize(30, 20));
	kmmSizer->Add(kmCtrl, 0, wxALL, 5);
	wxStaticText * mText = new wxStaticText(this, wxID_ANY, L"м");
	kmmSizer->Add(mText, 0, wxALL, 5);
	mCtrl = new wxTextCtrl(this, ID_V4F_VIEWER_MCTRL, L"", wxDefaultPosition, wxSize(30, 20));
	kmmSizer->Add(mCtrl, 0, wxALL, 5);
	textCtrlSizer->Add(kmmSizer);
	wxBoxSizer * shiftSizer = new wxBoxSizer(wxHORIZONTAL);
	leftCadre = new wxButton(this, ID_V4F_VIEWER_LEFT, L"<", wxDefaultPosition, wxSize(30, 20));
	shiftSizer->Add(leftCadre, 0, wxALL, 5);
	rightCadre = new wxButton(this, ID_V4F_VIEWER_RIGHT, L">", wxDefaultPosition, wxSize(30, 20));
	shiftSizer->Add(rightCadre, 0, wxALL, 5);
	textCtrlSizer->Add(shiftSizer);

	bottomSizer->Add(textCtrlSizer, 0, wxALL, 10);
	mainSizer->Add(topSizer, 0, wxALL, 0);
	mainSizer->Add(bottomSizer, 1, wxALL | wxEXPAND, 0);

	this->SetSizer(mainSizer);

	this->Centre();
	this->Center();
	this->Fit();
}

void V4fViewerDialog::OnUpdateUI(wxUpdateUIEvent & event)
{
	switch (event.GetId())
	{
		case ID_V4F_VIEWER_SLIDER:
		{
			int pos = slider->GetValue();
			if(currentPosSlider == pos)
				break;
			currentPosSlider = pos;
			//Выставляем координату в textCtrl
			double abscoord = dataSet->GetFrames()[pos].absCoord;
			if(abscoord > 1.0f)
			{
				VWayCoord wc = info.GetRealCoord(abscoord);
				wchar_t tmp[10] = L"";
				_snwprintf_s(tmp, 15, L"%i", wc.km);
				currentKm = wc.km;
				this->kmCtrl->SetValue(tmp);
				_snwprintf_s(tmp, 15, L"%.0f", wc.m);
				currentm = wc.m;
				this->mCtrl->SetValue(tmp);
			}
			else
			{
				this->kmCtrl->SetValue("0");
				this->mCtrl->SetValue("0");
			}
			Refresh();
			break;
		}
	}
	event.Skip();
}

void V4fViewerDialog::OnWayCoordUpdate(wxCommandEvent & event)
{
	try
	{
		long km = _wtoi(kmCtrl->GetValue().c_str());
		float m = _wtof(mCtrl->GetValue().c_str());
		if(currentKm == km && currentm == m)
		{
			event.Skip();
			return;
		}
		currentKm = km;
		currentm = m;
	}
	catch(std::exception e)
	{
		LOG_ERROR(L"Введите число");
		return;
	}
	VWayCoord wc(currentKm, currentm);
	double ac = info.GetAbsCoord(wc);
	for(int i = 0; i < dataSet->GetFrames().size(); i++)
	{
		if(dataSet->GetFrames()[i].absCoord > 0.001 && dataSet->GetFrames()[i].absCoord > ac)
		{
			slider->SetValue(i);			
		}
	}
	Refresh();
	event.Skip();
}

void V4fViewerDialog::OnLeftButton(wxCommandEvent & event)
{
	int pos = slider->GetValue();
	if(pos - 1 > -1)
	{
		currentPosSlider = pos - 1;
		slider->SetValue(pos - 1);
		//Выставляем координату в textCtrl
		double abscoord = dataSet->GetFrames()[pos-1].absCoord;
		if(abscoord > 1.0f)
		{
			VWayCoord wc = info.GetRealCoord(abscoord);
			wchar_t tmp[10] = L"";
			_snwprintf_s(tmp, 15, L"%i", wc.km);
			this->kmCtrl->SetValue(tmp);
			_snwprintf_s(tmp, 15, L"%.0f", wc.m);
			this->mCtrl->SetValue(tmp);
		}
		Refresh();
	}
	event.Skip();
}

void V4fViewerDialog::OnRightButton(wxCommandEvent & event)
{
	int pos = slider->GetValue();
	if(pos + 1 < dataSet->GetFrames().size())
	{
		currentPosSlider = pos + 1;
		slider->SetValue(pos + 1);
		//Выставляем координату в textCtrl
		double abscoord = dataSet->GetFrames()[pos + 1].absCoord;
		if(abscoord > 1.0f)
		{
			VWayCoord wc = info.GetRealCoord(abscoord);
			wchar_t tmp[10] = L"";
			_snwprintf_s(tmp, 15, L"%i", wc.km);
			currentKm = wc.km;
			this->kmCtrl->SetValue(tmp);
			_snwprintf_s(tmp, 15, L"%.0f", wc.m);
			currentm = wc.m;
			this->mCtrl->SetValue(tmp);
		}
		Refresh();
	}
	event.Skip();
}

void V4fViewerDialog::OnExportCurrentImg(wxCommandEvent & event)
{
	if(!mainBitmap.GetWidth())
		return;
	int pos = slider->GetValue();
	if(dataSet->GetFrames()[pos].absCoord > 1.0f)
	{
		VWayCoord wc = info.GetRealCoord(dataSet->GetFrames()[pos].absCoord);
		wchar_t nameFile [256];
		_snwprintf_s(nameFile, 255, L"%i_%s(%i km %.0f m).jpg", info.GetDirCode(), info.GetWayCode().c_str(), wc.km, wc.m);

		wxFileDialog dlg(this, L"Выберите файл", L"", nameFile
			, L"Файлы jpg|*.jpg",wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
		if (dlg.ShowModal() == wxID_OK)
		{
			wxImage image = mainBitmap.ConvertToImage();
			image.SaveFile(dlg.GetPath(), wxBITMAP_TYPE_JPEG);
		}
	}
	event.Skip();
}

void V4fViewerDialog::OnExportToAvi(wxCommandEvent & event)
{
	event.Skip();
}

void V4fViewerDialog::OnPaint(wxPaintEvent & event)
{
	if(dataSet)
	{
		wxPaintDC finallyMemoryDC(screen);
		this->PrepareDC(finallyMemoryDC);
		wxMemoryDC bmpDC;
		int __width = dataSet->GetFrames()[currentPosSlider].width;
		int __height = dataSet->GetFrames()[currentPosSlider].height;
		if(subtitleBitmap)
			delete subtitleBitmap;
		subtitleBitmap = new wxBitmap(__width, __height);
		VWayCoord wc;
		if(dataSet->GetFrames()[currentPosSlider].absCoord > 1.0f)
			wc = info.GetRealCoord(dataSet->GetFrames()[currentPosSlider].absCoord);
		if(bufDataLen < (__width * __height * 3))
		{
			if(bufDataLen)
				delete [] bufData;
			bufData = new unsigned char [__width * __height * 3];
		}
		dataSet->LoadImage(currentPosSlider, bufData, bufDataLen);
		///Отрисовка кадра в wxMemoryDC
		//высота субтитров
		int heightSubtitle = VIDEO_OPTIONS().Value().heightSubtitle;
		wxImage image(__width, __height, (unsigned char*) bufData, true);
		wxBitmap bitmap(image);
		///Загружаем кадр в MemoryDC 
		bmpDC.SelectObjectAsSource(bitmap);
		///Добавляем в finallyMemoryDC subtitleBitmap для отрисовки субтитров
		subtitleMemoryDC->SelectObjectAsSource(*subtitleBitmap);

		///Рисуем субтитры
		int width = subtitleBitmap->GetWidth(), height = subtitleBitmap->GetHeight();
			
		//узнаём куда рисовать субтитры
		int yPos = __height - 40;

		//сдвиг субтитров
		int shiftSubtitle = VIDEO_OPTIONS().Value().shiftSubtitle;
		switch(VIDEO_OPTIONS().Value().locationSubtitle)
		{
			case wxTOP:
				yPos = shiftSubtitle;
				break;
			case wxBOTTOM:
				yPos = __height - shiftSubtitle;
				break;
			default:
				LOG_ERROR(L"Неправильное задание расположения субтитров");
				break;
		}
		
		subtitleMemoryDC->DrawRectangle(0, 0, __width, heightSubtitle);
		//Формируем строку субтитров
		std::wstring tmpTmplt = VIDEO_OPTIONS().Value().tmpltSubtitle;
		if(tmpTmplt.find(L"%НАПР")!=std::string::npos)
			{
				wchar_t dir_str [32] = L"";
				swprintf(dir_str, 31, L"%i", info.GetDirCode());
				tmpTmplt.replace(tmpTmplt.find(L"%НАПР"), 5, dir_str);
			}
			if(tmpTmplt.find(L"%ПУТЬ")!=std::string::npos)
				tmpTmplt.replace(tmpTmplt.find(L"%ПУТЬ"), 5, string_to_wstring(info.GetWayCode()).c_str());
			if(tmpTmplt.find(L"%КМ")!=std::string::npos)
			{
				wchar_t km_str [15] = L"";
				swprintf(km_str, 14, L"%i", wc.km);
				tmpTmplt.replace(tmpTmplt.find(L"%КМ"), 3, km_str);
			}
			if(tmpTmplt.find(L"%М")!=std::string::npos)
			{
				wchar_t m_str [8] = L"";
				swprintf(m_str, 7, L"%i", wc.m);
				tmpTmplt.replace(tmpTmplt.find(L"%М"), 2, m_str);
			}
			if(tmpTmplt.find(L"%ПЕРЕГОН")!=std::string::npos)
				tmpTmplt.replace(tmpTmplt.find(L"%ПЕРЕГОН"), 8, string_to_wstring(info.GetAreaPeregon(dataSet->GetFrames()[currentPosSlider].absCoord-1.0, dataSet->GetFrames()[currentPosSlider].absCoord+1.0)).c_str());

			subtitleMemoryDC->SetFont(wxFont((int)(heightSubtitle * 0.6), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
			subtitleMemoryDC->DrawLabel(tmpTmplt.c_str(), wxRect(0, (int)(-heightSubtitle * 0.1), width, heightSubtitle), wxALIGN_CENTER);

		bmpDC.Blit(0, 0, __width, heightSubtitle, subtitleMemoryDC, 0, 0);
		mainBitmap = bmpDC.GetAsBitmap();
		finallyMemoryDC.StretchBlit(0, 0, mainBitmap.GetWidth(), mainBitmap.GetHeight(), &bmpDC, 0, 0, mainBitmap.GetWidth(), mainBitmap.GetHeight());
		Update();
	}
	event.Skip();
}

V4fViewerDialog::~V4fViewerDialog()
{
	if(bufDataLen)
		delete [] bufData;
	if(subtitleBitmap)
		delete subtitleBitmap;
	delete subtitleMemoryDC;
	V4fFrame::ClearTmp();
	if(dataSet)
		delete dataSet;
}