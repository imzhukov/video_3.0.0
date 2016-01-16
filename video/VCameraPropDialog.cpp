#include "VCameraPropDialog.h"

BEGIN_EVENT_TABLE(BaseProp,wxPanel)	
	//EVT_BUTTON(ID_CHOOSE_DIRECTORY, BaseProp::OnChooseDirectory)
	EVT_TEXT(ID_NAME_PROP, BaseProp::UpdateNameProp)
	//EVT_TEXT(ID_DIRECTORY_PROP, BaseProp::UpdateDirectoryProp)
END_EVENT_TABLE()

///Конструктор класса базовых настроек
BaseProp::BaseProp(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(500, 500))
{	
	//directory_str = 0;
	name_str = 0;
	wxStaticBoxSizer * vbox =  new wxStaticBoxSizer(wxVERTICAL,this,L"Общие настройки камеры");

	wxBoxSizer * nameBox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText * nameLabel = new wxStaticText(this,wxID_ANY,L"Имя камеры:");
	nameBox->Add(nameLabel, 0, wxALL, 10);
	name = new wxTextCtrl(this, ID_NAME_PROP, L"", wxDefaultPosition, wxSize(100,22));
	nameBox->Add(name, 0, wxALL|wxALIGN_RIGHT, 10);
	vbox->Add(nameBox, 0, wxALL, 10);
	
	/*
	wxBoxSizer * directoryBox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText * directoryLabel = new wxStaticText(this,wxID_ANY,L"Папка для записей:");
	directoryBox->Add(directoryLabel, 0, wxALL, 10);
	directory = new wxTextCtrl(this,ID_DIRECTORY_PROP,L"",wxDefaultPosition,wxSize(140,22));
	directoryBox->Add(directory, 0, wxALL|wxALIGN_RIGHT, 10);
	chooseDir = new wxButton(this, ID_CHOOSE_DIRECTORY, L"Обзор");
	directoryBox->Add(chooseDir, 0, wxALL|wxALIGN_RIGHT, 10);
	vbox->Add(directoryBox, 0, wxALL, 10);*/

	this->SetSizer(vbox);
}
//Обработка кнопки выбора папки для записей
/*
void BaseProp::OnChooseDirectory(wxCommandEvent &event)
{
	wxDirDialog *dir = new wxDirDialog(this,L"Выберите папку для записей с камеры наблюдения",wxEmptyString);
	if (dir->ShowModal() == wxID_OK)
		directory->SetValue(dir->GetPath());
}

void BaseProp::UpdateDirectoryProp(wxCommandEvent &event)
{
	if(directory_str)
		*directory_str = directory->GetValue().c_str();
	event.Skip();
}*/

void BaseProp::UpdateNameProp(wxCommandEvent &event)
{
	if(name_str)
		*name_str = name->GetValue().c_str();
	event.Skip();
}

void BaseProp::setProp(VCameraBase * prop)
{
	//directory_str = prop->GetDirectoryPtr();
	name_str = prop->GetCameraNamePtr();
	name->SetValue(name_str->c_str());
	//directory->SetValue(directory_str->c_str());
	this->Update();
}

BEGIN_EVENT_TABLE(RTSPProp,wxPanel)	
	//EVT_BUTTON(ID_CHOOSE_DIRECTORY, RTSPProp::OnChooseDirectory)
	EVT_TEXT(ID_ADDRESS_PROP, RTSPProp::UpdateProp)
	EVT_TEXT(ID_NAME_PROP, RTSPProp::UpdateNameProp)
	//EVT_TEXT(ID_DIRECTORY_PROP, RTSPProp::UpdateDirectoryProp)
END_EVENT_TABLE()

//Конструктор класса настроек для IP-камеры
RTSPProp::RTSPProp(wxWindow *parent) : wxPanel(parent, wxID_ANY)
{
	address_str = 0;
	//directory_str = 0;
	name_str = 0;

	vbox =  new wxStaticBoxSizer(wxVERTICAL,this, L"Настройки IP-камеры");

	wxBoxSizer * nameBox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText * nameLabel = new wxStaticText(this,wxID_ANY,L"Имя камеры:");
	nameBox->Add(nameLabel, 0, wxALL, 10);
	name = new wxTextCtrl(this, ID_NAME_PROP, L"", wxDefaultPosition, wxSize(100,22));
	nameBox->Add(name, 0, wxALL|wxALIGN_RIGHT, 10);
	vbox->Add(nameBox, 0, wxALL, 10);
	
	/*
	wxBoxSizer * directoryBox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText * directoryLabel = new wxStaticText(this,wxID_ANY,L"Папка для записей:");
	directoryBox->Add(directoryLabel, 0, wxALL, 10);
	directory = new wxTextCtrl(this,ID_DIRECTORY_PROP,L"",wxDefaultPosition,wxSize(140,22));
	directoryBox->Add(directory, 0, wxALL|wxALIGN_RIGHT, 10);
	chooseDir = new wxButton(this, ID_CHOOSE_DIRECTORY, L"Обзор");
	directoryBox->Add(chooseDir, 0, wxALL|wxALIGN_RIGHT, 10);
	vbox->Add(directoryBox, 0, wxALL, 10);*/

	wxBoxSizer * addressBox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText * label = new wxStaticText(this,wxID_ANY,L"Адрес камеры:");
	addressBox->Add(label, 0, wxALL, 10);
	address = new wxTextCtrl(this, ID_ADDRESS_PROP, L"");
	addressBox->Add(address, 0, wxALL|wxALIGN_RIGHT, 10);
	vbox->Add(addressBox, 0, wxALL, 10);

	this->SetSizer(vbox);	
	this->Centre();
	this->Fit();
}

void RTSPProp::setProp(VRTSPCameraProperties * prop)
{
	//directory_str = prop->GetDirectoryPtr();
	//directory->SetValue(directory_str->c_str());
	name_str = prop->GetCameraNamePtr();
	name->SetValue(name_str->c_str());
	address_str = prop->GetAddressPtr();
	address->SetValue(address_str->c_str());
	this->Update();
}

/*
void RTSPProp::UpdateDirectoryProp(wxCommandEvent &event)
{
	if(directory_str)
		*directory_str = directory->GetValue().c_str();
	event.Skip();
}*/

void RTSPProp::UpdateNameProp(wxCommandEvent &event)
{
	if(name_str)
		*name_str = name->GetValue().c_str();
	event.Skip();
}

void RTSPProp::UpdateProp(wxCommandEvent & event)
{
	if(address_str)
		*address_str = address->GetValue().c_str();
	event.Skip();
}

//Обработка кнопки выбора папки для записей
/*
void RTSPProp::OnChooseDirectory(wxCommandEvent &event)
{
	wxDirDialog *dir = new wxDirDialog(this,L"Выберите папку для записей с камеры наблюдения",wxEmptyString);
	if (dir->ShowModal() == wxID_OK)
		directory->SetValue(dir->GetPath());
}*/

BEGIN_EVENT_TABLE(JAIProp,wxPanel)	
	//EVT_BUTTON(ID_CHOOSE_DIRECTORY, RTSPProp::OnChooseDirectory)
	EVT_TEXT(ID_NUM_PROP, JAIProp::UpdateProp)
	EVT_TEXT(ID_NAME_PROP, RTSPProp::UpdateNameProp)
	//EVT_TEXT(ID_DIRECTORY_PROP, RTSPProp::UpdateDirectoryProp)
END_EVENT_TABLE()
///Конструктор класса настроек для камер старого типа
JAIProp::JAIProp(wxWindow* parent) : wxPanel(parent, wxID_ANY)
{
	//directory_str = 0;
	name_str = 0;
	numCameraPtr = 0;

	vbox =  new wxStaticBoxSizer(wxVERTICAL,this, L"Настройки JAI-камер");

	wxBoxSizer * nameBox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText * nameLabel = new wxStaticText(this,wxID_ANY,L"Имя камеры:");
	nameBox->Add(nameLabel, 0, wxALL, 10);
	name = new wxTextCtrl(this, ID_NAME_PROP, L"", wxDefaultPosition, wxSize(100,22));
	nameBox->Add(name, 0, wxALL|wxALIGN_RIGHT, 10);
	vbox->Add(nameBox, 0, wxALL, 10);
	
	/*
	wxBoxSizer * directoryBox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText * directoryLabel = new wxStaticText(this,wxID_ANY,L"Папка для записей:");
	directoryBox->Add(directoryLabel, 0, wxALL, 10);
	directory = new wxTextCtrl(this,ID_DIRECTORY_PROP,L"",wxDefaultPosition,wxSize(140,22));
	directoryBox->Add(directory, 0, wxALL|wxALIGN_RIGHT, 10);
	chooseDir = new wxButton(this, ID_CHOOSE_DIRECTORY, L"Обзор");
	directoryBox->Add(chooseDir, 0, wxALL|wxALIGN_RIGHT, 10);
	vbox->Add(directoryBox, 0, wxALL, 10);*/

	wxBoxSizer * paramBox = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText * paramLabel = new wxStaticText(this,wxID_ANY,L"Номер камеры");
	paramBox->Add(paramLabel, 0, wxALL, 10);
	numCamera = new wxTextCtrl(this, ID_NUM_PROP, L"");
	paramBox->Add(numCamera, 0, wxALL|wxALIGN_RIGHT, 10);
	vbox->Add(paramBox, 0, wxALL, 10);

	this->SetSizer(vbox);
	this->Centre();
	this->Fit();
}
//Установить указатель на строку с адресом камеры
void JAIProp::setProp(VJAICameraProperties * prop)
{
	//directory_str = prop->GetDirectoryPtr();
	//directory->SetValue(directory_str->c_str());
	name_str = prop->GetCameraNamePtr();
	name->SetValue(name_str->c_str());
	numCameraPtr = prop->GetNumCameraPtr();
	wchar_t val [32];
	swprintf(val, L"%i", *(prop->GetNumCameraPtr()));
	numCamera->SetValue(val);
	this->Update();
}

/*void JAIProp::UpdateDirectoryProp(wxCommandEvent &event)
{
	if(directory_str)
		*directory_str = directory->GetValue().c_str();
	event.Skip();
}*/

void JAIProp::UpdateNameProp(wxCommandEvent &event)
{
	if(name_str)
		*name_str = name->GetValue().c_str();
	event.Skip();
}

void JAIProp::UpdateProp(wxCommandEvent & event)
{
	if(numCameraPtr)
	{
		int val = _wtoi(numCamera->GetValue().c_str());
		if(val)
			*numCameraPtr = val;
	}
	event.Skip();
}
//Обработка кнопки выбора папки для записей
/*
void JAIProp::OnChooseDirectory(wxCommandEvent &event)
{
	wxDirDialog *dir = new wxDirDialog(this,L"Выберите папку для записей с камеры наблюдения",wxEmptyString);
	if (dir->ShowModal() == wxID_OK)
		directory->SetValue(dir->GetPath());
}*/

//----------------------------------------------- Методы класса VCameraPropDialog -------------------------------------------------
DECLARE_VARIABLE(std::wstring, video_folder, VFileName(VFileName::GetHomeDir(), L"Analysis3\\video\\").GetPath())
#define VIDEO_FOLDER video_folder().Value()

BEGIN_EVENT_TABLE(VCameraPropDialog,wxDialog)	
	EVT_CLOSE(VCameraPropDialog::OnClose)
	EVT_LIST_ITEM_SELECTED(ID_LIST_CTRL,VCameraPropDialog::OnListClick)
	EVT_TOOL(ID_ADD_BUTTON, VCameraPropDialog::OnAddTool)
	EVT_TOOL(ID_REMOVE_BUTTON, VCameraPropDialog::OnRemoveTool)
	EVT_RADIOBUTTON(ID_RTSPRB, VCameraPropDialog::setTypeCamera)
	EVT_RADIOBUTTON(ID_JAIRB, VCameraPropDialog::setTypeCamera)
	EVT_BUTTON(wxID_OK, VCameraPropDialog::OnOkButton)
	EVT_BUTTON(wxID_CANCEL, VCameraPropDialog::OnCancelButton)
	EVT_BUTTON(wxID_APPLY, VCameraPropDialog::OnApplyButton)
	EVT_UPDATE_UI(ID_ADD_BUTTON, VCameraPropDialog::OnUpdateUI)
	EVT_UPDATE_UI(ID_REMOVE_BUTTON, VCameraPropDialog::OnUpdateUI)
	EVT_AUINOTEBOOK_PAGE_CHANGING(wxID_ANY, VCameraPropDialog::OnPageChanging)	
END_EVENT_TABLE()

///Конструктор
VCameraPropDialog::VCameraPropDialog(wxWindow * parent,	std::vector<VCameraBase *> & _prop)
	: wxDialog(parent, wxID_ANY, L"Настройка камер", wxDefaultPosition, wxSize(600, 450), wxDEFAULT_DIALOG_STYLE), prop()
{	
	for(size_t i = 0; i < _prop.size(); i++)
	{
		prop.push_back(_prop[i]);
		if(_prop[i]->GetType() == RTSP_CAMERA)
			temp_prop.push_back(new VRTSPCameraProperties(*(VRTSPCameraProperties *) _prop[i]));
		else if(_prop[i]->GetType() == JAI_CAMERA)
			temp_prop.push_back(new VJAICameraProperties(*(VJAICameraProperties *) _prop[i]));
	}

	this->SetMinSize(wxSize(600, 450));
	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
	
	///Создание тулбара с кнопками управления списком камер
	toolBar = new wxToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize);
	toolBar->SetToolBitmapSize(wxSize(22,22));
	toolBar->AddTool(ID_ADD_BUTTON,L"Добавить камеру",wxBitmap(add_cam_xpm,wxBITMAP_TYPE_PNG), L"Добавляет новую камеру в список");
	toolBar->AddTool(ID_REMOVE_BUTTON, L"Удалить камеру",wxBitmap(remove_cam_xpm,wxBITMAP_TYPE_PNG), L"Удаляет выбранную камеру из списка"); 
	toolBar->Realize();

	///Добавляем радио-кнопки в сайзер
	wxBoxSizer * radioButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	RtspRadioButton = new wxRadioButton(this,ID_RTSPRB,L"RTSP");
	JaiRadioButton = new wxRadioButton(this,ID_JAIRB,L"JAI");
	radioButtonSizer->Add(RtspRadioButton, 0, wxALIGN_LEFT|wxALL, 1);
	radioButtonSizer->Add(JaiRadioButton, 0, wxALIGN_LEFT|wxLEFT, 10);

    /// Создаем сплиттер
    m_SplitterWindow = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    /// Устанавливаем минимальный размер компонента на сплиттере
    m_SplitterWindow->SetMinimumPaneSize(50);

	///Создаем список камер
	camList = new wxListCtrl(m_SplitterWindow, ID_LIST_CTRL, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
	NoteBook = new wxAuiNotebook(m_SplitterWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP|wxAUI_NB_WINDOWLIST_BUTTON|wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_MOVE|wxAUI_NB_TAB_SPLIT);
        
	rtspPropPanel = new RTSPProp(NoteBook);
	jaiPropPanel = new JAIProp(NoteBook);
    
	///Устанавливаем сплиттер
	m_SplitterWindow->SplitVertically(camList, NoteBook, 140);
	    
    camList->InsertColumn(0, L"Список камер", 0, 130);
	///Добавляем камеры в список
	for (size_t i = 0; i < prop.size(); i++)
	{
		wxListItem *info = new wxListItem();
		info->SetText(prop[i]->GetCameraName());
		info->SetColumn(0);
		info->SetId(i);
		camList->InsertItem(*info);
	}

	///Добавляем кнопки подтверждения/отмены проведенных операцийs
	wxBoxSizer * ButtonBox = new wxBoxSizer(wxHORIZONTAL);
	okButton = new wxButton(this,wxID_OK,L"ОК");
	cancelButton = new wxButton(this,wxID_CANCEL,L"Отмена");
	applyButton = new wxButton(this,wxID_APPLY,L"Применить");

	///Добавляем кнопки в главный
	ButtonBox->Add(okButton,0, wxALIGN_RIGHT | wxALIGN_BOTTOM | wxBOTTOM, 10);
	ButtonBox->Add(cancelButton,0, wxALIGN_RIGHT | wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 10);
	ButtonBox->Add(applyButton,0, wxALIGN_RIGHT | wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 10);

	///Добавляем тулбар в сайзер
	sizer->Add(toolBar, 0, wxTOP, 0);
	sizer->Add(radioButtonSizer, 0, wxALIGN_RIGHT, 0);
    sizer->Add(m_SplitterWindow, 1, wxALL|wxEXPAND, 10);
	sizer->Add(ButtonBox, 0, wxALIGN_RIGHT | wxALIGN_BOTTOM | wxRIGHT, 10);
	this->SetSizer(sizer);
	
    /// Центрируем форму на экране
    Centre();

	if(!temp_prop.size())
		toolBar->EnableTool(ID_REMOVE_BUTTON, false);
	else
		camList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	selected_type_setup=0;
	rtspPropPanel->Hide();
	jaiPropPanel->Hide();
}

void VCameraPropDialog::OnPageChanging(wxAuiNotebookEvent & evt)
{
	selected_type_setup = evt.GetSelection();
}

void VCameraPropDialog::setTypeCamera(wxCommandEvent& event)
{
	if(current_prop < 0)
		return;
	if(event.GetId() == ID_RTSPRB)
	{
		wchar_t camName [64];
		_snwprintf(camName, 63, L"%s", temp_prop[current_prop]->GetCameraName().c_str());
		delete temp_prop[current_prop];
		temp_prop.erase(temp_prop.begin() + current_prop);
		temp_prop.insert(temp_prop.begin() + current_prop, new VRTSPCameraProperties((const wchar_t *) camName, L"", VIDEO_FOLDER, V4F_RECORDING));
		rtspPropPanel->setProp((VRTSPCameraProperties *) temp_prop[current_prop]);

		rtspPropPanel->Show();
		jaiPropPanel->Hide();
		JaiRadioButton->SetValue(false);
	}
	else if(event.GetId() == ID_JAIRB)
	{
		wchar_t camName [64];
		_snwprintf(camName, 63, L"%s", temp_prop[current_prop]->GetCameraName().c_str());
		delete temp_prop[current_prop];
		temp_prop.erase(temp_prop.begin() + current_prop);
		temp_prop.insert(temp_prop.begin() + current_prop, new VJAICameraProperties((const wchar_t *) camName /*, VIDEO_FOLDER*/));
		jaiPropPanel->setProp((VJAICameraProperties *) temp_prop[current_prop]);

		rtspPropPanel->Hide();
		jaiPropPanel->Show();
		RtspRadioButton->SetValue(false);
	}
	NoteBook->SetSelection(selected_type_setup);
	event.Skip();
}

void VCameraPropDialog::OnClose(wxCloseEvent& event)
{
	for(size_t i = 0; i < temp_prop.size(); i++)
		delete temp_prop[i];
	event.Skip();
}

void VCameraPropDialog::OnUpdateUI(wxUpdateUIEvent& evt)
{
	if(!temp_prop.size())
		toolBar->EnableTool(ID_REMOVE_BUTTON, false);
	evt.Skip();
}

///Обработка кнопки "Добавить камеру в список"
void VCameraPropDialog::OnAddTool(wxCommandEvent &event)
{
	toolBar->EnableTool(ID_REMOVE_BUTTON, true);
	///Составляем название по умолчанию для новой камеры
	wchar_t camName [100];
	swprintf(camName, 100, L"Cam%i", temp_prop.size());
	///Добавляем камеру в список
	temp_prop.push_back(new VRTSPCameraProperties((const wchar_t *) camName, L"", VIDEO_FOLDER, V4F_RECORDING));
	///Добавляем камеру в список на форме
	long itemIndex = camList->InsertItem(temp_prop.size()-1, camName, temp_prop.size()-1);
	camList->SetItem(itemIndex, 0, camName);
	camList->SetItemState(temp_prop.size()-1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);	

	RtspRadioButton->SetValue(true);
	rtspPropPanel->Show();

	NoteBook->SetSelection(selected_type_setup);
	event.Skip();
}

///Обработка выбора камеры в списке
void VCameraPropDialog::OnListClick(wxListEvent &events)
{
	current_prop = -1;
	int ind = events.GetIndex();
	///заполняем поля
	int type = temp_prop[ind]->GetType();
	switch(type)
	{
		case RTSP_CAMERA:
		{
			RtspRadioButton->SetValue(true);
			JaiRadioButton->SetValue(false);
			rtspPropPanel->setProp((VRTSPCameraProperties *) temp_prop[ind]);
			rtspPropPanel->Show();
			jaiPropPanel->Hide();
			break;
		}
		case JAI_CAMERA:
		{
			RtspRadioButton->SetValue(false);
			JaiRadioButton->SetValue(true);
			jaiPropPanel->setProp((VJAICameraProperties *) temp_prop[ind]);
			rtspPropPanel->Hide();
			jaiPropPanel->Show();
			break;
		}
	}
	current_prop = ind;
	this->Update();
	events.Skip();
}

///Обработка кнопки "Удалить камеру"
void VCameraPropDialog::OnRemoveTool(wxCommandEvent &event)
{
	if (camList->GetItemCount() == 0)
	{
		wxMessageBox(L"Список камер пуст, невозможно удалить камеру!", L"Ошибка при удалении",wxICON_ERROR | wxOK); 
		return;
	}
	int id = camList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (id == -1)
	{
		wxMessageBox(L"Выберите камеру из списка!", L"Ошибка при удалении",wxICON_ERROR | wxOK); 
		return;
	}
	std::wstring msg =  L"Удалить камеру '" + temp_prop[id]->GetCameraName() + L"' из списка?";
	wxMessageDialog *box = new wxMessageDialog(this,msg.c_str(), 
		L"Удаление", wxYES_NO | wxNO_DEFAULT);
	if (box->ShowModal() == wxID_YES)
	{
		delete temp_prop[id];
		temp_prop.erase(temp_prop.begin()+id);
		camList->DeleteItem(id);
		if(camList->GetItemCount())
		{
			if(id)
				camList->SetItemState(id-1,wxLIST_STATE_SELECTED,wxLIST_STATE_SELECTED);
			else
				camList->SetItemState(0,wxLIST_STATE_SELECTED,wxLIST_STATE_SELECTED);
		}
	}
	current_prop = -1;
	
	event.Skip();
}

///Обработка кнопки ОК
void VCameraPropDialog::OnOkButton(wxCommandEvent &event)
{
	prop = temp_prop;
	temp_prop.clear();
	LOG_INFO(L"Настройки камер были изменены");
	EndModal(wxID_OK);
}

///Обработка кнопки Отмена
void VCameraPropDialog::OnCancelButton(wxCommandEvent &event)
{
	EndModal(wxID_CANCEL);
}
///Обработка кнопки Применить
void VCameraPropDialog::OnApplyButton(wxCommandEvent &event)
{
	prop = temp_prop;
	temp_prop.clear();
	for(size_t i = 0; i < prop.size(); i++)
	{
		switch(prop[i]->GetType())
		{
			case RTSP_CAMERA:
				temp_prop.push_back(new VRTSPCameraProperties(*((VRTSPCameraProperties *) prop[i])));
				break;
			case JAI_CAMERA:
				temp_prop.push_back(new VJAICameraProperties(*((VJAICameraProperties *) prop[i])));
				break;
		}
	}
	event.Skip();
}

std::vector<VCameraBase*> & VCameraPropDialog::GetCameraProperties()
{
	return prop;
}