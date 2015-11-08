#include "rtgraph.h"
#include "revision.h"
#include <boost/scoped_array.hpp>

void LogWin32Error()
{
	DWORD err=GetLastError();
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	  NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	  (LPTSTR)&lpMsgBuf, 0, NULL
	);
	std::wstring message((wchar_t*)lpMsgBuf);
	LocalFree(lpMsgBuf);
	LOG_ERROR(boost::str(boost::wformat(L"Ошибка Win32: %s") % message));
}

///Возвращает контекст принтера
HDC GetDefaultPrinterDC()
{	
	TCHAR	szDriver[16] = L"WINSPOOL";
    TCHAR	szPrinter[256];
    DWORD	cchBuffer = 255;    
        
    //Получаем имя принтера по умолчанию
    if (!GetDefaultPrinter(szPrinter, &cchBuffer))
	{		
		LogWin32Error();
		return NULL;
	}	

	//Открываем принтер
	HANDLE	hPrinter = NULL;    
	if (!OpenPrinter(szPrinter, &hPrinter, NULL))
	{	
		LogWin32Error();
		return NULL;
    }	

	//Получаем информацию о принтере	
	DWORD cbNeeded = 0;
	GetPrinter(hPrinter, 2, NULL, 0, &cbNeeded);			
	boost::scoped_array<char> pdBuffer(new char[cbNeeded]);
	DWORD cbBuf=cbNeeded;		
	PRINTER_INFO_2 * pPrinterData = (PRINTER_INFO_2*)(pdBuffer.get());
    if (!GetPrinter(hPrinter, 2, (LPBYTE)(pdBuffer.get()), cbBuf, &cbNeeded))
	{		
		LogWin32Error();
		ClosePrinter(hPrinter);
		return NULL;
	}
	else
	{		
		if (pPrinterData->pServerName) 
			LOG_INFO(boost::str(boost::wformat(L"Сервер печати: %s") % pPrinterData->pServerName));
		if (pPrinterData->pPrinterName)
			LOG_INFO(boost::str(boost::wformat(L"Принтер: %s") % pPrinterData->pPrinterName));
		if (pPrinterData->pPortName)
			LOG_INFO(boost::str(boost::wformat(L"Порт: %s") % pPrinterData->pPortName));
		if (pPrinterData->pDriverName)
			LOG_INFO(boost::str(boost::wformat(L"Драйвер: %s") % pPrinterData->pDriverName));
		if (pPrinterData->pComment)
			LOG_INFO(boost::str(boost::wformat(L"Комментарий: %s") % pPrinterData->pComment));
		if (pPrinterData->pLocation)
			LOG_INFO(boost::str(boost::wformat(L"Расположение: %s") % pPrinterData->pLocation));
		if (pPrinterData->pPrintProcessor)
			LOG_INFO(boost::str(boost::wformat(L"Процессор печати: %s") % pPrinterData->pPrintProcessor));
		if (pPrinterData->pDatatype)
			LOG_INFO(boost::str(boost::wformat(L"Тип данных: %s") % pPrinterData->pDatatype));
		if (pPrinterData->pParameters)
			LOG_INFO(boost::str(boost::wformat(L"Параметры: %s") % pPrinterData->pParameters));
		ClosePrinter(hPrinter);
	}
	
	//Получаем контекст устройства
    HDC hdcPrint = CreateDC(szDriver, szPrinter, pPrinterData->pPortName, NULL);
	if (hdcPrint==NULL)
	{		
		LogWin32Error();
		return NULL;
	}

	//Принудительно устанавливаем ладшафтную ориентацию
	LONG size=DocumentProperties(NULL, hPrinter, szPrinter, NULL, NULL, 0);
	if (size<0)
	{
		LogWin32Error();
	}
	else
	{
		boost::scoped_array<char> buffer(new char[size]);
		DEVMODE * pDevMode=(DEVMODE*)(buffer.get());
		DocumentProperties(NULL, hPrinter, szPrinter, pDevMode, NULL, DM_OUT_BUFFER);
		pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
		pDevMode->dmFields = DM_ORIENTATION;
		DocumentProperties(NULL, hPrinter, szPrinter, pDevMode, pDevMode, DM_IN_BUFFER);
		::ResetDC(hdcPrint, pDevMode);		
	}			
	return hdcPrint;
}

/// Рендеринг страницы в памяти
void VRTGraph::RenderPage(const VRTKmDataPtr& in_km)
{	
	if (GetPDFEnabled())
	{
		Paint(pdf_document->GetContext());
		pdf_document->AddPage();
	}		
	if (GetPrintingEnabled() && MustBePrinted(in_km))
	{
		print_queue.push_back(Render());		
	}
}

/// Формирует имя файла PDF
std::wstring VRTGraph::FormatPDFFileName(int in_km)
{	
	if (pdf_split_policy==SPLIT_KM)
		return (prefix + boost::str(boost::wformat(L"_%04d.pdf") % in_km));
	else
		return (prefix + boost::str(boost::wformat(L".pdf")));
}

/// Запись страницы в PDF
void VRTGraph::SaveCurrentKmPDF(cairo_surface_t * in_source_surface)
{		
	pdf_document->AddPage(in_source_surface);
}

/// Открывает PDF
void VRTGraph::OpenPDF(const std::wstring& in_filename)
{
	pdf_document.reset(new VPDFDocument(in_filename, GetVisibleWidth(), GetVisibleHeight()));
	pdf_ready=true;
	
}

/// Закрывает PDF
void VRTGraph::ClosePDF()
{
	pdf_ready=false;	
}

/// Печать страницы
void VRTGraph::PrintCurrentKm(cairo_surface_t * in_source_surface)
{
	/*Необходимо рендерить картинку под размеры и разрешение принтера, разрешение PDF задавать отдельно*/
	//print_queue.push_back(VRawData(device->GetSizeX(), device->GetSizeY(), cairo_image_surface_get_data(img_surface)));

#ifdef _WIN32
	//Печать
	printer_hdc=GetDefaultPrinterDC();
	if (printer_hdc!=NULL)
	{
		int lpx=GetDeviceCaps(printer_hdc, LOGPIXELSX);
		int lpy=GetDeviceCaps(printer_hdc, LOGPIXELSY);

		/// Холст cairo для принтера
		cairo_surface_t * printer_surface = cairo_win32_printing_surface_create(printer_hdc);	
		/// Контекст Cairo для принтера
		cairo_t * printer_cr = cairo_create(printer_surface);
		//Проверяем статус
		cairo_status_t status=cairo_status(printer_cr);
		if (status!=CAIRO_STATUS_SUCCESS)
		{			
			throw VSimpleException(L"Ошибка Cairo", string_to_wstring(cairo_status_to_string(status)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__, string_to_wstring(typeid(*this).name()));
		}
		status=cairo_surface_status(printer_surface);
		if (status!=CAIRO_STATUS_SUCCESS)
		{		
			throw VSimpleException(L"Ошибка Cairo", string_to_wstring(cairo_status_to_string(status)).c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}	
		
		DOCINFO doc_info;
		memset(&doc_info, 0, sizeof(DOCINFO));
		doc_info.cbSize=sizeof(DOCINFO);
		doc_info.lpszDocName=L"Путеизмерительная лента";//FixMe!!! желательно, чтобы здесь был номер километра.
		if (StartDoc(printer_hdc, &doc_info)<=0)
		{
			LogWin32Error();
			return;
		}	
		if (StartPage(printer_hdc)<=0)
		{
			LogWin32Error();
			return;
		}
		cairo_scale(printer_cr, lpx/dpi, lpy/dpi);
		cairo_set_source_surface(printer_cr, in_source_surface, 0, 0);		
		cairo_paint(printer_cr);
		cairo_surface_show_page(printer_surface);				
		cairo_destroy(printer_cr);
		cairo_surface_destroy(printer_surface);						
		if (EndPage(printer_hdc)<=0)
		{
			LogWin32Error();
			return;
		}	
		if (EndDoc(printer_hdc)<=0)
		{
			LogWin32Error();
			return;
		}
	}	
#endif
}

/// Возвращает количество страниц для печати
size_t VRTGraph::GetPageCount()
{
	return print_queue.size();
}
/// Возвращает страницу
VRawData VRTGraph::GetPage()
{
	return print_queue.front();
}
/// Удаляет страницу
void VRTGraph::PopPage()
{
	print_queue.pop_front();
}

/// Конструктор
VRTGraph::VRTGraph(const std::wstring& in_folder, const std::wstring& in_prefix) : VDrawing()
{
	page_number=0;			
	pdf_enabled=true;
	printing_enabled=false;
	pdf_split_policy=NO_SPLIT;
	dpi=DPI_DEFAULT_PRINTER;
	pdf_ready=false;

	folder=in_folder;
	prefix=in_prefix;

	SetDevice(VDrawingDevicePtr(new VDrawingDevicePaper(DPI_DEFAULT_PRINTER, DPI_DEFAULT_PRINTER, WIDTH_A4, HEIGHT_A4)));
}

/// Деструктор
VRTGraph::~VRTGraph()
{
	if (pdf_ready)
	{
		ClosePDF();	
	}
}

/// Устанавливает разрешение
void VRTGraph::SetDPI(double in_dpi)
{
	dpi=in_dpi;
}

/// Сохраняет страницу
void VRTGraph::ProcessKm(const VRTKmDataPtr& in_km)
{
	if (GetPDFEnabled() || GetPrintingEnabled())
	{
		if (!pdf_ready)
		{
			VFileName filename;
			filename.SetFolder(folder);
			filename.SetShortName(FormatPDFFileName(in_km->GetKm()));
			OpenPDF(filename.GetPath());
		}

		PrepareData(in_km);	
		VAprinfoKmRecord kmr=SITUATION.GetKm(in_km->GetKm());
		std::vector<VPicket> pk=SITUATION.GetPkList(in_km->GetKm());
		double startx=kmr.base;
		double last_shown=kmr.base;
		do
		{
			SetStartX(startx);
			page_info->SetPage(++page_number);			
			RenderPage(in_km);		
						
			struct find_pk
			{
				double coord;
				find_pk(double in_coord) : coord(in_coord) {};
				bool operator()(const VPicket& in_pk) {return in_pk.absStart<coord && in_pk.absEnd>=coord;};
			};

			std::vector<VPicket>::iterator i=std::find_if(pk.begin(), pk.end(), find_pk(aprinfo_axes->GetMaxX()));
			if (i!=pk.end())
				startx=i->absStart;
			else
				startx=aprinfo_axes->GetMaxX();

			last_shown=aprinfo_axes->GetMaxX();
			
			events->ClearDrawn();
			
		} while (last_shown-kmr.end<0);//FixMe!!! здесь должны быть дополнительные условия
		// длина участка, наличие отступлений на нем.
		ClearData();
		// Допечатываем события не поместившиеся на ленте
		while (events->GetCount()!=0)
		{			
			GetLayer(std::wstring(L"Frames"))->SetVisible(false);
			GetLayer(std::wstring(L"Graphs"))->SetVisible(false);
			SetStartX(events->GetX(0));
			page_info->SetPage(++page_number);
			RenderPage(in_km);		
			ClearData();
			GetLayer(std::wstring(L"Frames"))->SetVisible(true);
			GetLayer(std::wstring(L"Graphs"))->SetVisible(true);
		}
		ClearSummary();				

		if (pdf_split_policy==SPLIT_KM)
		{
			ClosePDF();
		}
	}		
}

/// Масштабирует области
void VRTGraph::AdjustAreas()
{
	double top_margin=4.0;
	double left_margin=DETECT_OPTIONS.GetOption(L"LEFT_MARGIN").GetValue<double>();
	double bottom_margin=1.0;
	double right_margin=15.0;

	//Устанавливаем размеры областей с графиками	
	for (area_sizer_itr i=area_sizers.begin(); i!=area_sizers.end(); ++i)
	{				
		(*i)->CalcSizeAndPos(top_margin/10.0, 
			left_margin/10.0, bottom_margin/10.0, 
			right_margin/10.0, GetVisibleWidth(), GetVisibleHeight(), device);
	}	

	if (header_area)
	{
		//Размеры области заголовка
		header_area->SetPosition(device->DrawingToDeviceX(left_margin/10.0),0);
		header_area->SetSize(GetVisibleWidth()
			-device->DrawingToDeviceX(left_margin/10.0)
			-device->DrawingToDeviceX(right_margin/10.0), 
			 device->DrawingToDeviceY(top_margin/10.0));
	}

	if (summary_area)
	{	
		double summary_space=device->DrawingToDeviceX(2.0/10.0);
		double summary_width=device->DrawingToDeviceX(7.0/10.0);
		double summary_height=device->DrawingToDeviceY(145.0/10.0);
		summary_area->SetPosition(device->DrawingToDeviceX(left_margin/10.0)
			-summary_width-summary_space, device->DrawingToDeviceY(top_margin/10.0));
		summary_area->SetSize(summary_width, summary_height);
	}
	
	if (summary_area_ext)
	{	
		double summary_space_ext=device->DrawingToDeviceX(2.0/10.0);
		double summary_width_ext=device->DrawingToDeviceX(7.0/10.0);
		double summary_height_ext=device->DrawingToDeviceY(185.0/10.0);
		summary_area_ext->SetPosition(device->DrawingToDeviceX(left_margin/10.0)-summary_width_ext-
			summary_space_ext, device->DrawingToDeviceY(top_margin/10.0));
		summary_area_ext->SetSize(summary_width_ext, summary_height_ext);
	}

	if (page_area)
	{
		//Размеры области с номером страницы
		page_area->SetPosition(GetVisibleWidth() - device->DrawingToDeviceX(right_margin/10.0),0);
		page_area->SetSize(
			device->DrawingToDeviceX(right_margin/10.0), 
			device->DrawingToDeviceY(top_margin/10.0));
	}
}

/** Устанавливает текущий масштаб*/
void VRTGraph::SetScaleX(double in_scale)
{		
	for (axes_itr i=axes_areas.begin(); i!=axes_areas.end(); ++i)
	{
		(*i)->SetScaleX(in_scale);
	}
}

/** Устанавливает начало координат по оси X*/
void VRTGraph::SetStartX(double in_start_x)
{
	for (axes_itr i=axes_areas.begin(); i!=axes_areas.end(); ++i)
	{
		(*i)->SetStartX(in_start_x);
	}
}

/// Устанавливает номер вагона
void VRTGraph::SetVPS(const std::wstring& in_vps)
{
	vps=in_vps;
}

/// Устанавливает ФИО
void VRTGraph::SetPerson(const std::wstring& in_person)
{
	person=in_person;
}

/// Устанавливает время проезда
void VRTGraph::SetTime(const VTime& in_time)
{
	ride_time=in_time;
}

/// Устанавливает источник данных
void VRTGraph::SetDataSource(const std::wstring& in_data_source)
{
	data_source=in_data_source;
}

/// Устанавливает опцию записи в PDF
void VRTGraph::SetPDFEnabled(bool in_pdf_enabled)
{
	pdf_enabled=in_pdf_enabled;
}

/// Возвращает true если включена запись в PDF
bool VRTGraph::GetPDFEnabled() const
{
	return pdf_enabled;
}

/// Устанавливает опцию печати на принере
void VRTGraph::SetPrintingEnabled(bool in_printing_enabled)
{
	printing_enabled=in_printing_enabled;
}

/// Возвращает true если включена печать на принтер
bool VRTGraph::GetPrintingEnabled() const
{
	return printing_enabled;
}

/// Устанавливает опцию режима записи PDF
void VRTGraph::SetPDFSplitPolicy(PDF_SPLIT_POLICY in_pdf_split_policy)
{
	pdf_split_policy=in_pdf_split_policy;
}

/// Возвращает опцию режима записи PDF
PDF_SPLIT_POLICY VRTGraph::GetPDFSplitPolicy()
{
	return pdf_split_policy;
}

//------------------------------------ Методы класса VRTGeometryGraph ----------------------------------

/// Подготовка очередной страницы
void VRTGeometryGraph::PrepareData(const VRTKmDataPtr& in_km)
{
	header->AddItem(boost::str(boost::wformat(L"ПИК ПРОГРЕСС: ПО версии %s (%s) ЦНИИ-4: %s (%s) %s Проезд: %s Источник данных: %s Масштаб: %s")
		% version_release
		% DETECT_OPTIONS.GetOption(L"INSTRUCTION").GetValue()
		% vps
		% person
		% string_to_wstring(VTime().ToStringLocale())
		% string_to_wstring(ride_time.DateToStringLocale())
		% data_source
		% DETECT_OPTIONS.GetOption(L"SCALE").GetValue()));

	peregon->AddItem(boost::str(boost::wformat(L"%s Км: %d") % in_km->GetPeregon() % in_km->GetKm()));
	
	if (DETECT_OPTIONS.UsePitLeft())
		pit_left->SetSize(in_km->GetDataCount());
	if (DETECT_OPTIONS.UsePitRight())
		pit_right->SetSize(in_km->GetDataCount());
	if (DETECT_OPTIONS.UseWidth())
		width->SetSize(in_km->GetDataCount());
	if (DETECT_OPTIONS.UseRichLeft())
		rich_left->SetSize(in_km->GetDataCount());
	if (DETECT_OPTIONS.UseRichRight())
		rich_right->SetSize(in_km->GetDataCount());
	if (DETECT_OPTIONS.UseLevel())
		level->SetSize(in_km->GetDataCount());	
	if (DETECT_OPTIONS.GetOption(L"SHOW_FACT_WEAR").Compare(OPTION_YES))	
		wear->SetSize(in_km->GetDataCount());
	if (DETECT_OPTIONS.GetOption(L"SHOW_PASSPORT_WEAR").Compare(OPTION_YES))
		wear_passport->SetSize(in_km->GetDataCount());
	
	///Данные	
	for (size_t i=0; i<in_km->GetDataCount(); ++i)
	{
		if (i<pit_left->GetCount())
			pit_left->Set(i, in_km->GetData(i).absCoord, in_km->GetData(i).pitLeft.value, in_km->GetData(i).pitLeft.corrupt);
		if (i<pit_right->GetCount())
			pit_right->Set(i, in_km->GetData(i).absCoord, in_km->GetData(i).pitRight.value, in_km->GetData(i).pitRight.corrupt);
		if (i<width->GetCount())
		{	
			VPoint _width=VWidthExtractor()(in_km->GetData(i));														
			width->Set(i, in_km->GetData(i).absCoord, _width.GetValue(), _width.GetCorrupt());										
		}
		if (i<rich_left->GetCount())
			rich_left->Set(i, in_km->GetData(i).absCoord, in_km->GetData(i).richLeft.value, in_km->GetData(i).richLeft.corrupt);	
		if (i<rich_right->GetCount())
			rich_right->Set(i, in_km->GetData(i).absCoord, in_km->GetData(i).richRight.value, in_km->GetData(i).richRight.corrupt);	
		if (i<level->GetCount())
			level->Set(i, in_km->GetData(i).absCoord, in_km->GetData(i).level.value, in_km->GetData(i).level.corrupt);	
		if (i<wear->GetCount())
			wear->Set(i, in_km->GetData(i).absCoord, in_km->GetData(i).wear.value, in_km->GetData(i).wear.corrupt);
		if (i<wear_passport->GetCount())
			wear_passport->Set(i, in_km->GetData(i).absCoord, SITUATION.GetNormaWear(in_km->GetData(i).absCoord), 0);
		if (in_km->GetData(i).rfid!=0)
		{
			rfid->Put(in_km->GetData(i).absCoord, 0, 0, 0, boost::str(boost::wformat(L"%d") % in_km->GetData(i).rfid), 0);
		}
	}
	
	//Отступления
	for (size_t i=0; i<in_km->GetOverstepCount(); ++i)
	{
		VBasicOverstepPtr overstep=in_km->GetOverstep(i);		

		if (overstep->GetDegree()!=DEGREE1)
		{

			if (overstep->GetGroup()==GROUP_GEOMETRY || overstep->GetGroup()==GROUP_SERVICE_GEOMETRY)
				events->Put(overstep);

			if (overstep->IsType(PIT_LEFT))
				pit_left_oversteps->Put(overstep);
			if (overstep->IsType(PIT_RIGHT))
				pit_right_oversteps->Put(overstep);
			if (overstep->IsType(WIDEN) || overstep->IsType(NARROW))
				width_oversteps->Put(overstep);
			if (overstep->IsType(RICH_LEFT))
				rich_left_oversteps->Put(overstep);
			if (overstep->IsType(RICH_RIGHT))
				rich_right_oversteps->Put(overstep);
			if (overstep->IsType(LEVEL) || overstep->IsType(DISTORTION))
				level_oversteps->Put(overstep);
			if (overstep->IsType(ANP) || overstep->IsType(ANP_HORIZONTAL))
				anp_oversteps->Put(overstep->GetCoord(), 0, 0);
			if (overstep->IsType(PSI) || overstep->IsType(PSI_HORIZONTAL))
				psi_oversteps->Put(overstep->GetCoord(), 0, 0);
			if (overstep->IsType(OTVOD_WIDTH))
				otvod_width_oversteps->Put(overstep->GetCoord(), 0 ,0);
		}
	}

	//Кривые
	VAprinfoKmRecord kmr=SITUATION.GetKm(in_km->GetKm());
	std::vector<VAprinfoCurve> curves=SITUATION.GetPassportCurveList(kmr.base, kmr.end);
	for (size_t i=0; i<curves.size(); ++i)
	{
		for (size_t j=0; j<curves.at(i).radius.size(); ++j)
		{
			double start_coord=curves.at(i).planLabel.at(radius_start_mark(j)).coord;
			double end_coord=curves.at(i).planLabel.at(radius_end_mark(j)).coord;
			VWayCoord start_wc=SITUATION.GetRealCoord(start_coord);
			VWayCoord end_wc=SITUATION.GetRealCoord(end_coord);
			double radius=curves.at(i).radius.at(j).radius;
			double level=curves.at(i).radius.at(j).level;
			double width=curves.at(i).radius.at(j).width;
			double wear=curves.at(i).radius.at(j).wear;
			
			if (DETECT_OPTIONS.GetOption(L"ROUND_RADIUS_AND_LEVEL").Compare(OPTION_YES))
			{
				radius=round_to(radius, 10);
				level=round_to(level, 5);
			}
			
			//Только если начало радиуса находится на километре
			if (start_coord>=kmr.base && start_coord<kmr.end)
			{
				events->Put(start_coord, boost::str(boost::wformat(L"%0.0f") % start_wc.m), 
				boost::str(boost::wformat(L"R:%0.0f H:%0.0f Ш:%0.0f И:%0.0f") % radius %
				level % width % wear), START_EVENT);				
			}
			else if (end_coord>=kmr.base && end_coord<kmr.end)
			{
				events->Put(end_coord, boost::str(boost::wformat(L"%0.0f") % end_wc.m), 
				boost::str(boost::wformat(L"R:%0.0f H:%0.0f Ш:%0.0f И:%0.0f") % radius %
				level % width % wear), END_EVENT);				
			}
		}
	}
	
	//Смена скорости по приказу
	std::vector<VAprinfoSpeedRecord> speeds=SITUATION.GetSpeedList(kmr.base, kmr.end);
	for (size_t i=0; i<speeds.size(); ++i)
	{
		double start=speeds.at(i).base;
		double end=speeds.at(i).end;
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
		{
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"Уст. %s") % speeds.at(i).format_short()), START_EVENT);
			borders->Put(start, 0, 0);
		}
		if (wc_end.km==in_km->GetKm())
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_end.m), 
			boost::str(boost::wformat(L"Уст. %s") % speeds.at(i).format_short()), END_EVENT);
	}
	
	//Смена временного ограничения скорости
	std::vector<VAprinfoSpeedRecord> tmp_speeds=SITUATION.GetTmpSpeedList(kmr.base, kmr.end);
	for (size_t i=0; i<tmp_speeds.size(); ++i)
	{	
		double start=tmp_speeds.at(i).base;
		double end=tmp_speeds.at(i).end;
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
		{
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"Врем. %s") % tmp_speeds.at(i).format_short()), START_EVENT);
			borders->Put(start, 0, 0);
		}
		if (wc_end.km==in_km->GetKm())
		{
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_end.m), 
			boost::str(boost::wformat(L"Врем. %s") % tmp_speeds.at(i).format_short()), END_EVENT);
			borders->Put(end, 0, 0);
		}
	}

	// Участки станций и перегоны
	std::vector<VAprinfoPntRecord> stations=SITUATION.GetPntList(kmr.base, kmr.end);
	for (size_t i=0; i<stations.size(); ++i)
	{
		double start=stations.at(i).base;
		double end=stations.at(i).end;
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
		{
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			string_to_wstring(stations.at(i).name), START_EVENT);
			borders->Put(start, 0, 0);
		}
		if (wc_end.km==in_km->GetKm())
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_end.m), 
			string_to_wstring(stations.at(i).name), END_EVENT);
	}

	// Участки дистанций
	std::vector<VAprinfoPchRecord> distantions=SITUATION.GetPchList(kmr.base, kmr.end);
	for (size_t i=0; i<distantions.size(); ++i)
	{
		double start=distantions.at(i).base;
		double end=distantions.at(i).end;
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
		{
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"ПЧ-%d (%s)") % distantions.at(i).pchCode % string_to_wstring(distantions.at(i).pchName)), START_EVENT);
			borders->Put(start, 0, 0);
		}
		if (wc_end.km==in_km->GetKm())
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_end.m), 
			boost::str(boost::wformat(L"ПЧ-%d (%s)") % distantions.at(i).pchCode % string_to_wstring(distantions.at(i).pchName)), END_EVENT);		
	}
	
	// Участки ремонтов
	std::vector<VAprinfoRepairRecord> repairs=SITUATION.GetRepairs(kmr.base, kmr.end);
	for (size_t i=0; i<repairs.size(); ++i)
	{
		//Показываем только капитальные ремонты
		if (repairs.at(i).type_id>=1 && repairs.at(i).type_id<=3)
		{
			//Приводим названия типов ремонтов к виду КВЛ-П
			std::wstring repair_type=(repairs.at(i).type_id==1) ? L"Усиленный капремонт" : L"Капремонт";			
			double start=std::max<double>(repairs.at(i).base, in_km->GetKmStart());
			double end=repairs.at(i).end;
			VWayCoord wc_start=SITUATION.GetRealCoord(start);
			VWayCoord wc_end=SITUATION.GetRealCoord(end);
			if (wc_start.km==in_km->GetKm())
			{
				events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
					boost::str(boost::wformat(L"%s %d.%d") % repair_type % 
					repairs.at(i).end_date.wMonth % repairs.at(i).end_date.wYear), START_EVENT);
				borders->Put(start, 0, 0);		
			}
			if (wc_end.km==in_km->GetKm())
			{
				events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_end.m), 
					boost::str(boost::wformat(L"%s %d.%d") % repair_type %
					repairs.at(i).end_date.wMonth % repairs.at(i).end_date.wYear), END_EVENT);
				borders->Put(end, 0, 0);
			}
		}
	}
	
	// Участки обкатки
	std::vector<VAprinfoTestrunRecord> test_run=SITUATION.GetTestrunList(kmr.base, kmr.end);
	for (size_t i=0; i<test_run.size(); ++i)
	{
		double start=test_run.at(i).base;
		double end=test_run.at(i).end;
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
		{
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"%s %s") % string_to_wstring(test_run.at(i).GetDescr()) % test_run.at(i).spd.format_short()), START_EVENT);
			borders->Put(start, 0, 0);
		}
		if (wc_end.km==in_km->GetKm())
		{
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"%s %s") % string_to_wstring(test_run.at(i).GetDescr()) % test_run.at(i).spd.format_short()), END_EVENT);
			borders->Put(end, 0, 0);
		}
	}
	 
	// Участки недостоверных паспортных данных
	for (size_t i=0; i<SITUATION.GetBadPassportSize(); ++i)
	{
		double start=SITUATION.GetBadPassport(i).GetSection().left_bound().value;
		double end=SITUATION.GetBadPassport(i).GetSection().right_bound().value;
						
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
		{
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
				SITUATION.GetBadPassport(i).GetDescription(), START_EVENT);
		}
		if (wc_end.km==in_km->GetKm())	
		{	
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_end.m), 
				SITUATION.GetBadPassport(i).GetDescription(), END_EVENT);
		}
	}
	// Состояние паспорта
	for (double x=in_km->GetKmStart(); x<in_km->GetKmEnd(); x+=5.0)
	{
		passport_status->Put(x, 0, 0, 0, L"", SITUATION.BadPassport(x));
	}
	
	// Сообщения
	for (size_t i=0; i<SITUATION.GetMessageCount(); ++i)
	{
		VWayCoord wc=SITUATION.GetRealCoord(SITUATION.GetMsg(i).GetCoord());		
		if (wc.km==in_km->GetKm())
			events->Put(SITUATION.GetMsg(i).GetCoord(), boost::str(boost::wformat(L"%0.0f") % wc.m), 
				SITUATION.GetMsg(i).GetMsg(), SING_EVENT);
	}

	//События должны быть отсортированы
	events->Sort();
	debug_events->Sort();		
		
	summary->AddItem(in_km->GetSectionString());
	summary->AddItem(in_km->GetSummaryString(GROUP_GEOMETRY));
	if (!DETECT_OPTIONS.HIDE_PASSPORT_LINES())
	{	
		for (size_t i=0; i<SITUATION.GetLevelCount(PWLF_PASSPORT); ++i)
		{
			level_passport_line_original->Put(SITUATION.GetLevelPoint(i, PWLF_PASSPORT).x, SITUATION.GetLevelPoint(i, PWLF_PASSPORT).y, 0);
		}	
		for (size_t i=0; i<SITUATION.GetRichCount(PWLF_PASSPORT, SD_UNDEFINED); ++i)
		{
			rich_passport_line_original->Put(SITUATION.GetRichPoint(i, PWLF_PASSPORT, SD_UNDEFINED).x, SITUATION.GetRichPoint(i, PWLF_PASSPORT, SD_UNDEFINED).y, 0);
		}	
		for (size_t i=0; i<SITUATION.GetWidthCount(PWLF_PASSPORT); ++i)
		{
			width_passport_line_original->Put(SITUATION.GetWidthPoint(i, PWLF_PASSPORT).x, SITUATION.GetWidthPoint(i, PWLF_PASSPORT).y, 0);
		}	
	}			
	for (size_t i=0; i<SITUATION.GetLevelCount(PWLF_ZERO); ++i)
	{
		level_passport_line_corrected->Put(SITUATION.GetLevelPoint(i, PWLF_ZERO).x, SITUATION.GetLevelPoint(i, PWLF_ZERO).y, 0);
	}	
	for (size_t i=0; i<SITUATION.GetRichCount(PWLF_ZERO, SD_LEFT); ++i)
	{
		rich_left_passport_line_corrected->Put(SITUATION.GetRichPoint(i, PWLF_ZERO, SD_LEFT).x, SITUATION.GetRichPoint(i, PWLF_ZERO, SD_LEFT).y, 0);
	}
	for (size_t i=0; i<SITUATION.GetRichCount(PWLF_ZERO, SD_RIGHT); ++i)
	{
		rich_right_passport_line_corrected->Put(SITUATION.GetRichPoint(i, PWLF_ZERO, SD_RIGHT).x, SITUATION.GetRichPoint(i, PWLF_ZERO, SD_RIGHT).y, 0);
	}	
	for (size_t i=0; i<SITUATION.GetWidthCount(PWLF_ZERO); ++i)
	{
		width_passport_line_corrected->Put(SITUATION.GetWidthPoint(i, PWLF_ZERO).x, SITUATION.GetWidthPoint(i, PWLF_ZERO).y, 0);
	}
	
	// Стрелки
	for (size_t i=0; i<SITUATION.GetSwitchCount(); ++i)
	{		
		opt_switches->Put(SITUATION.GetSwitch(i));
		opt_switch_sections->Put(SITUATION.GetSwitch(i));
	}
	for (size_t i=0; i<SITUATION.GetAprSwitchCount(); ++i)
	{		
		apr_switches->Put(SITUATION.GetAprSwitch(i));
		apr_switch_sections->Put(SITUATION.GetAprSwitch(i));
	}
}

/// Формирует имя файла PDF
std::wstring VRTGeometryGraph::FormatPDFFileName(int in_km)
{
	if (pdf_split_policy==SPLIT_KM)
		return (prefix + boost::str(boost::wformat(L"_ГРК_%04d.pdf") % in_km));
	else
		return (prefix + boost::str(boost::wformat(L"_ГРК.pdf")));
}

///Возвращает true если километр нужно печатать
bool VRTGeometryGraph::MustBePrinted(const VRTKmDataPtr& in_km)
{
	return (DETECT_OPTIONS.GetOption(L"PRINT_POLICY_GEOMETRY").Compare(OPTION_PRINT_ALL) ||
	        DETECT_OPTIONS.GetOption(L"PRINT_POLICY_GEOMETRY").Compare(OPTION_PRINT_OVERSTEPS) && in_km->GetOverstepAmount(GROUP_GEOMETRY)!=0 ||
	        DETECT_OPTIONS.GetOption(L"PRINT_POLICY_GEOMETRY").Compare(OPTION_PRINT_RESTRICTIONS) && in_km->HasRestriction(GROUP_GEOMETRY));
}


/// Очистка страницы
void VRTGeometryGraph::ClearData()
{	
	pit_left->Clear();
	pit_right->Clear();
	width->Clear();
	wear->Clear();
	wear_passport->Clear();
	rich_left->Clear();
	rich_right->Clear();
	level->Clear();
	events->ClearDrawn();
	debug_events->Clear();
	

	pit_left_oversteps->Clear();
	pit_right_oversteps->Clear();
	width_oversteps->Clear();
	rich_left_oversteps->Clear();
	rich_right_oversteps->Clear();
	level_oversteps->Clear();
	anp_oversteps->Clear();
	psi_oversteps->Clear();
	otvod_width_oversteps->Clear();
	
	if (!DETECT_OPTIONS.HIDE_PASSPORT_LINES())
	{
		level_passport_line_original->Clear();
		rich_passport_line_original->Clear();
		width_passport_line_original->Clear();
	}
	level_passport_line_corrected->Clear();
	rich_left_passport_line_corrected->Clear();
	rich_right_passport_line_corrected->Clear();
	width_passport_line_corrected->Clear();
	
	opt_switches->Clear();
	apr_switches->Clear();
	opt_switch_sections->Clear();
	apr_switch_sections->Clear();	

	borders->Clear();
	
	rfid->Clear();
}

/// Очистка всех данных
void VRTGeometryGraph::ClearSummary()
{	
	header->Clear();
	peregon->Clear();
	summary->Clear();	
	passport_status->Clear();
}

/// Подготовка
void VRTGeometryGraph::Prepare()
{
	BASE_FONT_SIZE().Value()=0.25;

	//Создаем слои
	AddLayer(std::wstring(L"Background"));
	AddLayer(std::wstring(L"Summary"));
	AddLayer(std::wstring(L"Frames"));
	AddLayer(std::wstring(L"Graphs"));
	AddLayer(std::wstring(L"Events"));
	AddLayer(std::wstring(L"Debug"));

	//Создаем области		
	header_area=VAreaPtr(new VArea);	
	summary_area=VAreaPtr(new VArea);
	summary_area_ext=VAreaPtr(new VArea);
	page_area=VAreaPtr(new VArea);
	aprinfo_area=VAreaPtr(new VArea);
	pit_left_area=VAreaPtr(new VArea);
	pit_right_area=VAreaPtr(new VArea);
	width_area=VAreaPtr(new VArea);
	rich_left_area=VAreaPtr(new VArea);
	rich_right_area=VAreaPtr(new VArea);
	level_area=VAreaPtr(new VArea);
	anp_area=VAreaPtr(new VArea);
		
	event_area=VAreaPtr(new VArea);

	graph_area=VAreaPtr(new VArea);	


	//Создаем сайзеры	
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(aprinfo_area, 0, 0, 100, 5)));
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(pit_left_area, 0, 5, 100, 7)));
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(pit_right_area, 0, 12, 100, 7)));
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(width_area, 0, 19, 100, 12)));
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(rich_left_area, 0, 31, 100, 13)));
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(rich_right_area, 0, 44, 100, 13)));
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(level_area, 0, 57, 100, 17)));
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(anp_area, 0, 68, 100, 3)));		

	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(event_area, 0, 74, 100, 26)));

	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(graph_area, 0, 5, 100, 69)));
	

	//Создаем оси

	aprinfo_axes=VAxesPtr(new VAxes(aprinfo_area, device));
	aprinfo_axes->SetScaleY(1.0);	
	axes_areas.push_back(aprinfo_axes);

	pit_left_axes=VAxesPtr(new VAxes(pit_left_area, device));
	pit_left_axes->SetScaleY(DETECT_OPTIONS.GetScalePit());
	pit_left_axes->SetStartY(0);
	pit_left_axes->SetOriginY(50);
	axes_areas.push_back(pit_left_axes);

	pit_right_axes=VAxesPtr(new VAxes(pit_right_area, device));
	pit_right_axes->SetScaleY(DETECT_OPTIONS.GetScalePit());
	pit_right_axes->SetStartY(0);
	pit_right_axes->SetOriginY(50);
	axes_areas.push_back(pit_right_axes);

	width_axes=VAxesPtr(new VAxes(width_area, device));
	width_axes->SetScaleY(DETECT_OPTIONS.GetScaleWidth());
	width_axes->SetStartY(1520.0);
	width_axes->SetOriginY(75);
	axes_areas.push_back(width_axes);

	wear_axes=VAxesPtr(new VAxes(width_area, device));
	wear_axes->SetScaleY(DETECT_OPTIONS.GetScaleWidth());
	wear_axes->SetStartY(0.0);
	wear_axes->SetOriginY(100);
	axes_areas.push_back(wear_axes);

	rich_left_axes=VAxesPtr(new VAxes(rich_left_area, device));
	rich_left_axes->SetScaleY(DETECT_OPTIONS.GetScaleRich());
	rich_left_axes->SetStartY(0);
	rich_left_axes->SetOriginY(90);
	axes_areas.push_back(rich_left_axes);

	rich_right_axes=VAxesPtr(new VAxes(rich_right_area, device));
	rich_right_axes->SetScaleY(DETECT_OPTIONS.GetScaleCoord());
	rich_right_axes->SetStartY(0);
	rich_right_axes->SetOriginY(25);
	axes_areas.push_back(rich_right_axes);

	level_axes=VAxesPtr(new VAxes(level_area, device));
	level_axes->SetScaleY(DETECT_OPTIONS.GetScaleCoord());
	level_axes->SetStartY(0);
	level_axes->SetOriginY(25);
	axes_areas.push_back(level_axes);
	
	anp_axes=VAxesPtr(new VAxes(anp_area, device));
	anp_axes->SetScaleY(1.0);
	anp_axes->SetStartY(0);
	anp_axes->SetOriginY(0);
	axes_areas.push_back(anp_axes);
		
	event_axes=VAxesPtr(new VAxes(event_area, device));
	event_axes->SetScaleY(1.0);	
	axes_areas.push_back(event_axes);

	graph_axes=VAxesPtr(new VAxes(graph_area, device));
	graph_axes->SetScaleY(1.0);	
	axes_areas.push_back(graph_axes);
	

	//Создаем источники данных		
	header = VUserTextProviderPtr(new VUserTextProvider());	
	page_info = VPageInfoPtr(new VPageInfo());	
	coordinates = VCoordinatesDataPtr(new VCoordinatesData(GLOBAL_APRINFO, pro::interval<double>(0, DBL_MAX)));
	summary = VUserTextProviderPtr(new VUserTextProvider());
	peregon = VUserTextProviderPtr(new VUserTextProvider());

	pit_left = VVectorDataPtr(new VVectorData());
	pit_right = VVectorDataPtr(new VVectorData());
	width = VVectorDataPtr(new VVectorData());
	wear = VVectorDataPtr(new VVectorData());
	wear_passport = VVectorDataPtr(new VVectorData());
	rich_left = VVectorDataPtr(new VVectorData());
	rich_right = VVectorDataPtr(new VVectorData());
	level = VVectorDataPtr(new VVectorData());
	rfid = VDequeDataPtr(new VDequeData());

	if (!DETECT_OPTIONS.HIDE_PASSPORT_LINES())
	{
		level_passport_line_original = VDequeDataPtr(new VDequeData());	
		rich_passport_line_original = VDequeDataPtr(new VDequeData());	
		width_passport_line_original = VDequeDataPtr(new VDequeData());
	}
	level_passport_line_corrected = VDequeDataPtr(new VDequeData());	
	rich_left_passport_line_corrected = VDequeDataPtr(new VDequeData());
	rich_right_passport_line_corrected = VDequeDataPtr(new VDequeData());	
	width_passport_line_corrected = VDequeDataPtr(new VDequeData());
	
	opt_switches = VSwitchDataPtr(new VSwitchData());
	apr_switches = VSwitchDataPtr(new VSwitchData());
	opt_switch_sections = VSwitchSectionsDataPtr(new VSwitchSectionsData());
	apr_switch_sections = VSwitchSectionsDataPtr(new VSwitchSectionsData());
	aprinfo_spala=VDataProviderPtr(new VAprinfoSpalaData(GLOBAL_APRINFO));
	aprinfo_straight_side=VDataProviderPtr(new VAprinfoStraightSideData(GLOBAL_APRINFO, DETECT_OPTIONS.UsePassport()));
	
	pit_left_oversteps = VRTOverstepDataPtr(new VRTOverstepData());
	pit_right_oversteps = VRTOverstepDataPtr(new VRTOverstepData());
	width_oversteps = VRTOverstepDataPtr(new VRTOverstepData());
	rich_left_oversteps = VRTOverstepDataPtr(new VRTOverstepData());
	rich_right_oversteps = VRTOverstepDataPtr(new VRTOverstepData());
	level_oversteps = VRTOverstepDataPtr(new VRTOverstepData());
	anp_oversteps = VDequeDataPtr(new VDequeData());
	psi_oversteps = VDequeDataPtr(new VDequeData());
	otvod_width_oversteps = VDequeDataPtr(new VDequeData());

	events = VRTEventInfoDataPtr(new VRTEventInfoData());
	borders = VDequeDataPtr(new VDequeData());
	debug_events = VRTEventInfoDataPtr(new VRTEventInfoData());
	passport_status = VDequeDataPtr(new VDequeData());
					
	//Создаем элементы
	GetLayer(std::wstring(L"Background"))->AddElement(VElementPtr(new VBackgroundElement(VColor(255,255,255,255))));	
	GetLayer(std::wstring(L"Background"))->AddElement(VElementPtr(new VPageNumberElement(page_area, device, page_info)));
	
	GetLayer(std::wstring(L"Summary"))->AddElement(VElementPtr(new VTextElement(header_area, device, header)));
	GetLayer(std::wstring(L"Summary"))->AddElement(VElementPtr(new VKVLPSummaryElement(summary_area, device, summary)));
	GetLayer(std::wstring(L"Summary"))->AddElement(VElementPtr(new VAreaFrameElement(summary_area, device, FRAME_WIDTH, AREA_FRAME_TOP | AREA_FRAME_BOTTOM | AREA_FRAME_RIGHT)));
	GetLayer(std::wstring(L"Summary"))->AddElement(VElementPtr(new VAreaFrameElement(summary_area_ext, device, FRAME_WIDTH, AREA_FRAME_LEFT)));

	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VAreaFrameElement(aprinfo_area, device, FRAME_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VKVLPAreaNameElement(aprinfo_area, device, L"Км", FRAME_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VKVLPAreaNameElement(pit_left_area, device, L"Пр. л.", FRAME_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VKVLPAreaNameElement(pit_right_area, device, L"Пр. пр.", FRAME_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VKVLPAreaNameElement(width_area, device, DETECT_OPTIONS.GetOption(L"WIDTH_SOURCE").Compare(OPTION_STD) ? L"Шаблон" : 
		boost::str(boost::wformat(L"Шаблон [%s]") % DETECT_OPTIONS.GetOption(L"WIDTH_SOURCE").GetValue()), FRAME_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VKVLPAreaNameElement(rich_left_area, device, L"Рихтовка л.", FRAME_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VKVLPAreaNameElement(rich_right_area, device, L"Рихтовка пр.", FRAME_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VKVLPAreaNameElement(level_area, device, L"Уровень", FRAME_WIDTH)));

	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VAreaFrameElement(graph_area, device, FRAME_WIDTH, AREA_FRAME_RIGHT | AREA_FRAME_LEFT)));
	
	GetLayer(std::wstring(L"Events"))->AddElement(VElementPtr(new VKVLPPeregonElement(event_area, device, peregon)));
	GetLayer(std::wstring(L"Events"))->AddElement(VElementPtr(new VKVLPEventAreaName(event_area, device, FRAME_WIDTH)));

	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(pit_left_axes, 10.0, L"10")), pit_left_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(pit_left_axes, 0.0, L"0")), pit_left_axes, ZERO_DASH, ZERO_COLOR, CONSTANT_LINE_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(pit_left_axes, -10.0, L"-10")), pit_left_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));

	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(pit_right_axes, 10.0, L"10")), pit_right_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(pit_right_axes, 0.0, L"0")), pit_right_axes, ZERO_DASH, ZERO_COLOR, CONSTANT_LINE_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(pit_right_axes, -10.0, L"-10")), pit_right_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));

	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(width_axes, 1548.0, L"1548")), width_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(width_axes, 1542.0, L"1542")), width_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(width_axes, 1536.0, L"1536")), width_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(width_axes, 1528.0, L"1528")), width_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(width_axes, 1520.0, L"1520")), width_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(width_axes, 1512.0, L"1512")), width_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));

	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(rich_left_axes, 30.0, L"30")), rich_left_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(rich_left_axes, 0.0, L"0")), rich_left_axes, ZERO_DASH, ZERO_COLOR, CONSTANT_LINE_WIDTH)));
	
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(rich_right_axes, 0.0, L"0")), rich_right_axes, ZERO_DASH, ZERO_COLOR, CONSTANT_LINE_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(rich_right_axes, -30.0, L"-30")), rich_right_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));

	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(level_axes, 30.0, L"30")), level_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(level_axes, 0.0, L"0")), level_axes, ZERO_DASH, ZERO_COLOR, CONSTANT_LINE_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(level_axes, -30.0, L"-30")), level_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));		

	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPCoordinatesElement(coordinates, aprinfo_axes, true)));	
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPBridgeAreaElement(VDataProviderPtr(new VAprinfoBridgeData(GLOBAL_APRINFO, true)), aprinfo_axes, COLOR_HTML_GRAY)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPBridgeAreaElement(VDataProviderPtr(new VAprinfoTunnelData(GLOBAL_APRINFO, true)), aprinfo_axes, COLOR_HTML_GRAY)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPBridgeElement(VDataProviderPtr(new VAprinfoBridgeData(GLOBAL_APRINFO, false)), aprinfo_axes, COLOR_HTML_BLACK)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPTunnelElement(VDataProviderPtr(new VAprinfoTunnelData(GLOBAL_APRINFO, false)), aprinfo_axes, COLOR_HTML_BLACK)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPCrossingElement(VDataProviderPtr(new VAprinfoCrossingData(GLOBAL_APRINFO)), aprinfo_axes, COLOR_HTML_BLACK)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPChamferedJointElement(VDataProviderPtr(new VAprinfoChamferedJointData(GLOBAL_APRINFO)), aprinfo_axes, COLOR_HTML_BLACK)));
	
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPSpalaElement(aprinfo_spala, aprinfo_axes, COLOR_HTML_BLACK, min_line_width)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPStraightSideElement(aprinfo_straight_side, aprinfo_axes, COLOR_HTML_BLACK, min_line_width)));	
	
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VXMarkGraph(rfid, aprinfo_axes, COLOR_HTML_BLACK, 0.25, 1.0, AREA_TOP, false)));

	if (!DETECT_OPTIONS.HIDE_PASSPORT_LINES())
	{
		GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(level_passport_line_original, level_axes, ORIGINAL_NORMA_COLOR, ORIGINAL_NORMA_COLOR, NORMA_LINE_WIDTH, CLIP_HORIZONTAL)));
		GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(rich_passport_line_original, rich_left_axes, ORIGINAL_NORMA_COLOR, ORIGINAL_NORMA_COLOR, NORMA_LINE_WIDTH, CLIP_HORIZONTAL)));
		GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(rich_passport_line_original, rich_right_axes, ORIGINAL_NORMA_COLOR, ORIGINAL_NORMA_COLOR, NORMA_LINE_WIDTH, CLIP_HORIZONTAL)));
		GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(width_passport_line_original, width_axes, ORIGINAL_NORMA_COLOR, ORIGINAL_NORMA_COLOR, NORMA_LINE_WIDTH, CLIP_HORIZONTAL)));
	}
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(level_passport_line_corrected, level_axes, CORRECTED_NORMA_COLOR, CORRECTED_NORMA_COLOR, NORMA_LINE_WIDTH, CLIP_HORIZONTAL)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(rich_left_passport_line_corrected, rich_left_axes, CORRECTED_NORMA_COLOR, CORRECTED_NORMA_COLOR, NORMA_LINE_WIDTH, CLIP_HORIZONTAL)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(rich_right_passport_line_corrected, rich_right_axes, CORRECTED_NORMA_COLOR, CORRECTED_NORMA_COLOR, NORMA_LINE_WIDTH, CLIP_HORIZONTAL)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(width_passport_line_corrected, width_axes, CORRECTED_NORMA_COLOR, CORRECTED_NORMA_COLOR, NORMA_LINE_WIDTH, CLIP_HORIZONTAL)));
	
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPSwitchesElement(opt_switches, aprinfo_axes, OPT_SWITCH_COLOR, OPT_SWITCH_COLOR)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPSwitchesElement(apr_switches, aprinfo_axes, APR_SWITCH_COLOR, APR_SWITCH_COLOR)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPSwitchSectionsElement(opt_switch_sections, aprinfo_axes, OPT_SWITCH_COLOR, OPT_SWITCH_COLOR)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPSwitchSectionsElement(apr_switch_sections, aprinfo_axes, APR_SWITCH_COLOR, APR_SWITCH_COLOR)));
	
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(pit_left, pit_left_axes, PARAMETER_COLOR, CORRUPT_COLOR, PARAMETER_LINE_WIDTH, CLIP_HORIZONTAL)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(pit_right, pit_right_axes, PARAMETER_COLOR, CORRUPT_COLOR, PARAMETER_LINE_WIDTH, CLIP_HORIZONTAL)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(width, width_axes, PARAMETER_COLOR, CORRUPT_COLOR, PARAMETER_LINE_WIDTH, CLIP_HORIZONTAL)));
	
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(wear_axes, 0.0, L"")), wear_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH)));
	if (DETECT_OPTIONS.GetOption(L"SHOW_FACT_WEAR").Compare(OPTION_YES))
	{
		GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(wear, wear_axes, CORRECTED_NORMA_COLOR, CORRUPT_COLOR, NORMA_LINE_WIDTH, CLIP_HORIZONTAL)));
	}
	if (DETECT_OPTIONS.GetOption(L"SHOW_PASSPORT_WEAR").Compare(OPTION_YES))
	{		
		GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(wear_passport, wear_axes, ORIGINAL_NORMA_COLOR, CORRUPT_COLOR, NORMA_LINE_WIDTH, CLIP_HORIZONTAL)));		
	}
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(rich_left, rich_left_axes, PARAMETER_COLOR, CORRUPT_COLOR, PARAMETER_LINE_WIDTH, CLIP_HORIZONTAL)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(rich_right, rich_right_axes, PARAMETER_COLOR, CORRUPT_COLOR, PARAMETER_LINE_WIDTH, CLIP_HORIZONTAL)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(level, level_axes, PARAMETER_COLOR, CORRUPT_COLOR, PARAMETER_LINE_WIDTH, CLIP_HORIZONTAL)));	

	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPOverstepElement(pit_left_oversteps, pit_left_axes, OVERSTEP_COLOR, OVERSTEP_FILL_COLOR)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPOverstepElement(pit_right_oversteps, pit_right_axes, OVERSTEP_COLOR, OVERSTEP_FILL_COLOR)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPOverstepElement(width_oversteps, width_axes, OVERSTEP_COLOR, OVERSTEP_FILL_COLOR)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPOverstepElement(rich_left_oversteps, rich_left_axes, OVERSTEP_COLOR, OVERSTEP_FILL_COLOR)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPOverstepElement(rich_right_oversteps, rich_right_axes, OVERSTEP_COLOR, OVERSTEP_FILL_COLOR)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPOverstepElement(level_oversteps, level_axes, OVERSTEP_COLOR, OVERSTEP_FILL_COLOR)));

	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPAnpElement(anp_oversteps, anp_axes)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPPsiElement(psi_oversteps, anp_axes)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPOtvodWidthElement(otvod_width_oversteps, anp_axes)));
	
	GetLayer(std::wstring(L"Events"))->AddElement(VElementPtr(new VKVLPEventElement(events, event_axes)));

	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VXLineGraph(borders, graph_axes, Y_LINE_DASH, Y_LINE_COLOR, NORMA_LINE_WIDTH)));	

	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPPassportStatusElement(passport_status, graph_axes)));

	AdjustAreas();
	SetScaleX(DETECT_OPTIONS.GetScaleCoord());

	for (axes_itr i=axes_areas.begin(); i!=axes_areas.end(); ++i)
	{
		(*i)->SetInverseX(true);
	}
}



//------------------------------------ Методы класса VRTExtraGraph ----------------------------------

/// Подготовка очередной страницы
void VRTExtraGraph::PrepareData(const VRTKmDataPtr& in_km)
{	
	header->AddItem(boost::str(boost::wformat(L"ПИК ПРОГРЕСС: ПО версии %s (%s) ЦНИИ-4: %s (%s) %s Проезд: %s Источник данных: %s")
		% version_release
		% L"доп. параметры"
		% vps
		% person
		% string_to_wstring(VTime().ToStringLocale())
		% string_to_wstring(ride_time.DateToStringLocale())
		% data_source));	

	peregon->AddItem(boost::str(boost::wformat(L"%s Км: %d") % in_km->GetPeregon() % in_km->GetKm()));
	
	if (DETECT_OPTIONS.UseWearLeft() || DETECT_OPTIONS.UseWearRight())
		wear->SetSize(in_km->GetDataCount());
	if (DETECT_OPTIONS.GetOption(L"SHOW_PASSPORT_WEAR").Compare(OPTION_YES))
		wear_passport->SetSize(in_km->GetDataCount());	

	for (size_t i=0; i<in_km->GetDataCount(); ++i)
	{									
		if (DETECT_OPTIONS.UseWearLeft() || DETECT_OPTIONS.UseWearRight())
			wear->Set(i, in_km->GetData(i).absCoord, in_km->GetData(i).wear.value, in_km->GetData(i).wear.corrupt);
		if (i<wear_passport->GetCount())
			wear_passport->Set(i, in_km->GetData(i).absCoord, SITUATION.GetNormaWear(in_km->GetData(i).absCoord), 0);
		if (DETECT_OPTIONS.UseZazorLeft() && in_km->GetData(i).zazorLeft.value>=0)
			zazor_left_opt->Put(in_km->GetData(i).absCoord, in_km->GetData(i).zazorLeft.value, in_km->GetData(i).zazorLeft.corrupt);		
		if (DETECT_OPTIONS.UseZazorRight() && in_km->GetData(i).zazorRight.value>=0)
			zazor_right_opt->Put(in_km->GetData(i).absCoord, in_km->GetData(i).zazorRight.value, in_km->GetData(i).zazorRight.corrupt);
		if (DETECT_OPTIONS.UseMagZazorLeft() && in_km->GetData(i).mzLeft.value>=0)
			zazor_left_mag->Put(in_km->GetData(i).absCoord, in_km->GetData(i).mzLeft.value, in_km->GetData(i).mzLeft.corrupt);		
		if (DETECT_OPTIONS.UseMagZazorRight() && in_km->GetData(i).mzRight.value>=0)
			zazor_right_mag->Put(in_km->GetData(i).absCoord, in_km->GetData(i).mzRight.value, in_km->GetData(i).mzRight.corrupt);	
		if (in_km->GetData(i).rfid!=0)
		{
			rfid->Put(in_km->GetData(i).absCoord, 0, 0, 0, boost::str(boost::wformat(L"%d") % in_km->GetData(i).rfid), 0);
		}			
	}

	//Отступления
	for (size_t i=0; i<in_km->GetOverstepCount(); ++i)
	{
		VBasicOverstepPtr overstep=in_km->GetOverstep(i);		

		if (overstep->GetGroup()==GROUP_EXTRA || overstep->GetGroup()==GROUP_SERVICE_EXTRA)
			events->Put(overstep);
		
		if (overstep->IsType(WEAR))
		{					
			wear_oversteps->Put(overstep);
		}
		if (overstep->IsType(ZAZOR_LEFT))
		{		
			zazor_left_oversteps->Put(overstep);
		}
		if (overstep->IsType(ZAZOR_RIGHT))
		{		
			zazor_right_oversteps->Put(overstep);
		}
		
	}

	//Кривые
	VAprinfoKmRecord kmr=SITUATION.GetKm(in_km->GetKm());
	std::vector<VAprinfoCurve> curves=SITUATION.GetPassportCurveList(kmr.base, kmr.end);
	for (size_t i=0; i<curves.size(); ++i)
	{
		for (size_t j=0; j<curves.at(i).radius.size(); ++j)
		{
			double start_coord=curves.at(i).planLabel.at(j*2+1).coord;
			double end_coord=curves.at(i).planLabel.at(j*2+2).coord;
			VWayCoord start_wc=SITUATION.GetRealCoord(start_coord);
			VWayCoord end_wc=SITUATION.GetRealCoord(end_coord);
			double radius=curves.at(i).radius.at(j).radius;
			double level=curves.at(i).radius.at(j).level;
			double width=curves.at(i).radius.at(j).width;
			double wear=curves.at(i).radius.at(j).wear;
			
			if (DETECT_OPTIONS.GetOption(L"ROUND_RADIUS_AND_LEVEL").Compare(OPTION_YES))
			{
				radius=round_to(radius, 10);
				level=round_to(level, 5);
			}
			
			//Только если начало радиуса находится на километре
			if (start_coord>=kmr.base && start_coord<kmr.end)
			{
				events->Put(start_coord, boost::str(boost::wformat(L"%0.0f") % start_wc.m), 
				boost::str(boost::wformat(L"R:%0.0f H:%0.0f Ш:%0.0f И:%0.0f") % radius %
				level % width % wear), START_EVENT);				
			}
			else if (end_coord>=kmr.base && end_coord<kmr.end)
			{
				events->Put(end_coord, boost::str(boost::wformat(L"%0.0f") % end_wc.m), 
				boost::str(boost::wformat(L"R:%0.0f H:%0.0f Ш:%0.0f И:%0.0f") % radius %
				level % width % wear), END_EVENT);				
			}
		}
	}
	
	//Смена скорости
	std::vector<VAprinfoSpeedRecord> speeds=SITUATION.GetSpeedList(kmr.base, kmr.end);
	for (size_t i=0; i<speeds.size(); ++i)
	{
		double start=speeds.at(i).base;
		double end=speeds.at(i).end;
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
		{
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"Уст. %s") % speeds.at(i).format_short()), START_EVENT);

			borders->Put(start, 0, 0);
		}
		if (wc_end.km==in_km->GetKm())
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_end.m), 
			boost::str(boost::wformat(L"Уст. %s") % speeds.at(i).format_short()), END_EVENT);
	}
	
	//Смена временного ограничения скорости
	std::vector<VAprinfoSpeedRecord> tmp_speeds=SITUATION.GetTmpSpeedList(kmr.base, kmr.end);
	for (size_t i=0; i<tmp_speeds.size(); ++i)
	{	
		double start=tmp_speeds.at(i).base;
		double end=tmp_speeds.at(i).end;
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
		{
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"Врем. %s") % tmp_speeds.at(i).format_short()), START_EVENT);
			borders->Put(start, 0, 0);
		}
		if (wc_end.km==in_km->GetKm())
		{
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_end.m), 
			boost::str(boost::wformat(L"Врем. %s") % tmp_speeds.at(i).format_short()), END_EVENT);
			borders->Put(end, 0, 0);
		}
	}

	// Участки станций и перегоны
	std::vector<VAprinfoPntRecord> stations=SITUATION.GetPntList(kmr.base, kmr.end);
	for (size_t i=0; i<stations.size(); ++i)
	{
		double start=stations.at(i).base;
		double end=stations.at(i).end;
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			string_to_wstring(stations.at(i).name), START_EVENT);
		if (wc_end.km==in_km->GetKm())
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_end.m), 
			string_to_wstring(stations.at(i).name), END_EVENT);
	}
	// Участки дистанций
	std::vector<VAprinfoPchRecord> distantions=SITUATION.GetPchList(kmr.base, kmr.end);
	for (size_t i=0; i<distantions.size(); ++i)
	{
		double start=distantions.at(i).base;
		double end=distantions.at(i).end;
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"ПЧ-%d (%s)") % distantions.at(i).pchCode % string_to_wstring(distantions.at(i).pchName)), START_EVENT);
		if (wc_end.km==in_km->GetKm())
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_end.m), 
			boost::str(boost::wformat(L"ПЧ-%d (%s)") % distantions.at(i).pchCode % string_to_wstring(distantions.at(i).pchName)), END_EVENT);		
	}
		
	// Участки ремонтов
	std::vector<VAprinfoRepairRecord> repairs=SITUATION.GetRepairs(kmr.base, kmr.end);
	for (size_t i=0; i<repairs.size(); ++i)
	{
		//Показываем только капитальные ремонты
		if (repairs.at(i).type_id>=1 && repairs.at(i).type_id<=3)
		{
			//Приводим названия типов ремонтов к виду КВЛ-П
			std::wstring repair_type=(repairs.at(i).type_id==1) ? L"Усиленный капремонт" : L"Капремонт";			
			double start=std::max<double>(repairs.at(i).base, in_km->GetKmStart());
			double end=repairs.at(i).end;
			VWayCoord wc_start=SITUATION.GetRealCoord(start);
			VWayCoord wc_end=SITUATION.GetRealCoord(end);
			if (wc_start.km==in_km->GetKm())
			{
				events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
					boost::str(boost::wformat(L"%s %d.%d") % repair_type % 
					repairs.at(i).end_date.wMonth % repairs.at(i).end_date.wYear), START_EVENT);			
			}
			if (wc_end.km==in_km->GetKm())
			{
				events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_end.m), 
					boost::str(boost::wformat(L"%s %d.%d") % repair_type %
					repairs.at(i).end_date.wMonth % repairs.at(i).end_date.wYear), END_EVENT);
			}
		}
	}
	
	// Участки обкатки
	std::vector<VAprinfoTestrunRecord> test_run=SITUATION.GetTestrunList(kmr.base, kmr.end);
	for (size_t i=0; i<test_run.size(); ++i)
	{
		double start=test_run.at(i).base;
		double end=test_run.at(i).end;
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
		{
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"%s %s") % string_to_wstring(test_run.at(i).GetDescr()) % test_run.at(i).spd.format_short()), START_EVENT);
			borders->Put(start, 0, 0);
		}
		if (wc_end.km==in_km->GetKm())
		{
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"%s %s") % string_to_wstring(test_run.at(i).GetDescr()) % test_run.at(i).spd.format_short()), END_EVENT);
			borders->Put(end, 0, 0);
		}
	}
	
	// Плети бесстыкового пути (левые.)
	std::vector<VAprinfoWobRecord> wob_left=SITUATION.GetWobLeftList(kmr.base, kmr.end);
	for (size_t i=0; i<wob_left.size(); ++i)
	{
		double start=wob_left.at(i).base;
		double end=wob_left.at(i).end;
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"Плеть лв. (%s)") % wob_left.at(i).num.c_str()), START_EVENT);
		if (wc_end.km==in_km->GetKm())
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"Плеть лв. (%s)") % wob_left.at(i).num.c_str()), END_EVENT);
	}
	
	// Плети бесстыкового пути (правые.)
	std::vector<VAprinfoWobRecord> wob_right=SITUATION.GetWobRightList(kmr.base, kmr.end);
	for (size_t i=0; i<wob_right.size(); ++i)
	{
		double start=wob_right.at(i).base;
		double end=wob_right.at(i).end;
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"Плеть пр. (%s)") % wob_right.at(i).num.c_str()), START_EVENT);
		if (wc_end.km==in_km->GetKm())
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"Плеть пр. (%s)") % wob_right.at(i).num.c_str()), END_EVENT);
	}
	
	// Тип рельсов
	std::vector<VAprinfoRailRecord> rails_type=SITUATION.GetRailsTypeList(kmr.base, kmr.end);
	for (size_t i=0; i<rails_type.size(); ++i)
	{
		double start=rails_type.at(i).base;
		double end=rails_type.at(i).end;
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"Тип рельсов: %s") % string_to_wstring(rails_type.at(i).GetDescr())), START_EVENT);
		if (wc_end.km==in_km->GetKm())
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
			boost::str(boost::wformat(L"Тип рельсов: %s") % string_to_wstring(rails_type.at(i).GetDescr())), END_EVENT);
	}
		
	// Участки недостоверных паспортных данных
	for (size_t i=0; i<SITUATION.GetBadPassportSize(); ++i)
	{
		double start=SITUATION.GetBadPassport(i).GetSection().left_bound().value;
		double end=SITUATION.GetBadPassport(i).GetSection().right_bound().value;
						
		VWayCoord wc_start=SITUATION.GetRealCoord(start);
		VWayCoord wc_end=SITUATION.GetRealCoord(end);
		if (wc_start.km==in_km->GetKm())
		{
			events->Put(start, boost::str(boost::wformat(L"%0.0f") % wc_start.m), 
				SITUATION.GetBadPassport(i).GetDescription(), START_EVENT);
		}
		if (wc_end.km==in_km->GetKm())	
		{	
			events->Put(end, boost::str(boost::wformat(L"%0.0f") % wc_end.m), 
				SITUATION.GetBadPassport(i).GetDescription(), END_EVENT);
		}
	}
	// Состояние паспорта
	for (double x=in_km->GetKmStart(); x<in_km->GetKmEnd(); x+=5.0)
	{
		passport_status->Put(x, 0, 0, 0, L"", SITUATION.BadPassport(x));
	}
	// Сообщения
	for (size_t i=0; i<SITUATION.GetMessageCount(); ++i)
	{
		VWayCoord wc=SITUATION.GetRealCoord(SITUATION.GetMsg(i).GetCoord());		
		if (wc.km==in_km->GetKm())
			events->Put(SITUATION.GetMsg(i).GetCoord(), boost::str(boost::wformat(L"%0.0f") % wc.m), 
				SITUATION.GetMsg(i).GetMsg(), SING_EVENT);
	}

	//События должны быть отсортированы
	events->Sort();
	debug_events->Sort();		
		

	summary->AddItem(in_km->GetSectionString());
	summary->AddItem(in_km->GetSummaryString(GROUP_EXTRA));
	
	// Стрелки
	for (size_t i=0; i<SITUATION.GetSwitchCount(); ++i)
	{		
		opt_switches->Put(SITUATION.GetSwitch(i));
		opt_switch_sections->Put(SITUATION.GetSwitch(i));
	}
	for (size_t i=0; i<SITUATION.GetAprSwitchCount(); ++i)
	{		
		apr_switches->Put(SITUATION.GetAprSwitch(i));
		apr_switch_sections->Put(SITUATION.GetAprSwitch(i));
	}
}

/// Формирует имя файла PDF
std::wstring VRTExtraGraph::FormatPDFFileName(int in_km)
{
	if (pdf_split_policy==SPLIT_KM)
		return (prefix + boost::str(boost::wformat(L"_доп_%04d.pdf") % in_km));
	else
		return (prefix + boost::str(boost::wformat(L"_доп.pdf")));
}

///Возвращает true если километр нужно печатать
bool VRTExtraGraph::MustBePrinted(const VRTKmDataPtr& in_km)
{
	return (DETECT_OPTIONS.GetOption(L"PRINT_POLICY_EXTRA").Compare(OPTION_PRINT_ALL) ||
	        DETECT_OPTIONS.GetOption(L"PRINT_POLICY_EXTRA").Compare(OPTION_PRINT_OVERSTEPS) && in_km->GetOverstepAmount(GROUP_EXTRA)!=0 ||
	        DETECT_OPTIONS.GetOption(L"PRINT_POLICY_EXTRA").Compare(OPTION_PRINT_RESTRICTIONS) && in_km->HasRestriction(GROUP_EXTRA));
}

/// Очистка страницы
void VRTExtraGraph::ClearData()
{	
	wear->Clear();
	wear_passport->Clear();	
	zazor_left_opt->Clear();
	zazor_right_opt->Clear();
	zazor_left_mag->Clear();
	zazor_right_mag->Clear();	
	events->ClearDrawn();
	debug_events->Clear();
	
	wear_oversteps->Clear();	
	zazor_left_oversteps->Clear();
	zazor_right_oversteps->Clear();

	opt_switches->Clear();
	apr_switches->Clear();
	opt_switch_sections->Clear();
	apr_switch_sections->Clear();	

	borders->Clear();
	
	rfid->Clear();
}

/// Очистка всех данных
void VRTExtraGraph::ClearSummary()
{	
	header->Clear();
	peregon->Clear();
	summary->Clear();
	passport_status->Clear();
}

/// Подготовка
void VRTExtraGraph::Prepare()
{
	BASE_FONT_SIZE().Value()=0.25;

	//Создаем слои
	AddLayer(std::wstring(L"Background"));
	AddLayer(std::wstring(L"Summary"));
	AddLayer(std::wstring(L"Frames"));
	AddLayer(std::wstring(L"Graphs"));
	AddLayer(std::wstring(L"Events"));
	AddLayer(std::wstring(L"Debug"));

	//Создаем области		
	header_area=VAreaPtr(new VArea);	
	summary_area=VAreaPtr(new VArea);
	summary_area_ext=VAreaPtr(new VArea);
	page_area=VAreaPtr(new VArea);
	aprinfo_area=VAreaPtr(new VArea);
	wear_area=VAreaPtr(new VArea);		
	zazor_left_area=VAreaPtr(new VArea);
	zazor_right_area=VAreaPtr(new VArea);		
	event_area=VAreaPtr(new VArea);
	graph_area=VAreaPtr(new VArea);

	//Создаем сайзеры	
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(aprinfo_area, 0, 0, 100, 5)));
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(wear_area, 0, 5, 100, 25)));		
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(zazor_left_area, 0, 30, 100, 20)));
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(zazor_right_area, 0, 50, 100, 20)));
				
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(event_area, 0, 74, 100, 26)));
	area_sizers.push_back(VAreaSizerPtr(new VAreaSizer(graph_area, 0, 5, 100, 69)));

	//Создаем оси

	aprinfo_axes=VAxesPtr(new VAxes(aprinfo_area, device));
	aprinfo_axes->SetScaleY(1.0);	
	axes_areas.push_back(aprinfo_axes);

	wear_axes=VAxesPtr(new VAxes(wear_area, device));
	wear_axes->SetScaleY(10.0);
	wear_axes->SetStartY(0);
	wear_axes->SetOriginY(100);
	axes_areas.push_back(wear_axes);
				
	zazor_left_axes=VAxesPtr(new VAxes(zazor_left_area, device));
	zazor_left_axes->SetScaleY(20.0);
	zazor_left_axes->SetStartY(0);
	zazor_left_axes->SetOriginY(100);
	axes_areas.push_back(zazor_left_axes);

	zazor_right_axes=VAxesPtr(new VAxes(zazor_right_area, device));
	zazor_right_axes->SetScaleY(20.0);
	zazor_right_axes->SetStartY(0);
	zazor_right_axes->SetOriginY(100);
	axes_areas.push_back(zazor_right_axes);
			
	event_axes=VAxesPtr(new VAxes(event_area, device));
	event_axes->SetScaleY(1.0);	
	axes_areas.push_back(event_axes);

	graph_axes=VAxesPtr(new VAxes(graph_area, device));
	graph_axes->SetScaleY(1.0);	
	axes_areas.push_back(graph_axes);
	

	//Создаем источники данных		
	header = VUserTextProviderPtr(new VUserTextProvider());	
	page_info = VPageInfoPtr(new VPageInfo());	
	coordinates = VCoordinatesDataPtr(new VCoordinatesData(GLOBAL_APRINFO, pro::interval<double>(0, DBL_MAX)));
	summary = VUserTextProviderPtr(new VUserTextProvider());
	peregon = VUserTextProviderPtr(new VUserTextProvider());

	wear = VVectorDataPtr(new VVectorData());
	wear_passport = VVectorDataPtr(new VVectorData());
	zazor_left_opt = VDequeDataPtr(new VDequeData());
	zazor_right_opt = VDequeDataPtr(new VDequeData());
	zazor_left_mag = VDequeDataPtr(new VDequeData());
	zazor_right_mag = VDequeDataPtr(new VDequeData());
	rfid = VDequeDataPtr(new VDequeData());

	/// Стрелки	
	opt_switches = VSwitchDataPtr(new VSwitchData());
	apr_switches = VSwitchDataPtr(new VSwitchData());
	opt_switch_sections = VSwitchSectionsDataPtr(new VSwitchSectionsData());
	apr_switch_sections = VSwitchSectionsDataPtr(new VSwitchSectionsData());

	wear_oversteps = VRTOverstepDataPtr(new VRTOverstepData());	
	zazor_left_oversteps = VRTOverstepDataPtr(new VRTOverstepData());
	zazor_right_oversteps = VRTOverstepDataPtr(new VRTOverstepData());
	
	events = VRTEventInfoDataPtr(new VRTEventInfoData());
	borders = VDequeDataPtr(new VDequeData());
	debug_events = VRTEventInfoDataPtr(new VRTEventInfoData());
	passport_status = VDequeDataPtr(new VDequeData());
					
	//Создаем элементы
	GetLayer(std::wstring(L"Background"))->AddElement(VElementPtr(new VBackgroundElement(VColor(255,255,255,255))));	
	GetLayer(std::wstring(L"Background"))->AddElement(VElementPtr(new VPageNumberElement(page_area, device, page_info)));
	
	GetLayer(std::wstring(L"Summary"))->AddElement(VElementPtr(new VTextElement(header_area, device, header)));
	GetLayer(std::wstring(L"Summary"))->AddElement(VElementPtr(new VKVLPSummaryElement(summary_area, device, summary)));
	GetLayer(std::wstring(L"Summary"))->AddElement(VElementPtr(new VAreaFrameElement(summary_area, device, FRAME_WIDTH, AREA_FRAME_TOP | AREA_FRAME_BOTTOM | AREA_FRAME_RIGHT)));
	GetLayer(std::wstring(L"Summary"))->AddElement(VElementPtr(new VAreaFrameElement(summary_area_ext, device, FRAME_WIDTH, AREA_FRAME_LEFT)));

	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VAreaFrameElement(aprinfo_area, device, FRAME_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VKVLPAreaNameElement(aprinfo_area, device, L"Км", FRAME_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VKVLPAreaNameElement(wear_area, device, DETECT_OPTIONS.GetOption(L"WEAR_SOURCE").Compare(OPTION_STD) ? L"Износ" : 
		boost::str(boost::wformat(L"Износ [%s]") % DETECT_OPTIONS.GetOption(L"WEAR_SOURCE").GetValue()), FRAME_WIDTH)));		
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VKVLPAreaNameElement(zazor_left_area, device, L"Зазор [лв.]", FRAME_WIDTH)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VKVLPAreaNameElement(zazor_right_area, device, L"Зазор [пр.]", FRAME_WIDTH)));
	
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VAreaFrameElement(graph_area, device, FRAME_WIDTH, AREA_FRAME_RIGHT | AREA_FRAME_LEFT)));
	
	GetLayer(std::wstring(L"Events"))->AddElement(VElementPtr(new VKVLPPeregonElement(event_area, device, peregon)));
	GetLayer(std::wstring(L"Events"))->AddElement(VElementPtr(new VKVLPEventAreaName(event_area, device, FRAME_WIDTH)));
	
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPCoordinatesElement(coordinates, aprinfo_axes, true)));	
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPBridgeAreaElement(VDataProviderPtr(new VAprinfoBridgeData(GLOBAL_APRINFO, true)), aprinfo_axes, COLOR_HTML_GRAY)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPBridgeAreaElement(VDataProviderPtr(new VAprinfoTunnelData(GLOBAL_APRINFO, true)), aprinfo_axes, COLOR_HTML_GRAY)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPBridgeElement(VDataProviderPtr(new VAprinfoBridgeData(GLOBAL_APRINFO, false)), aprinfo_axes, COLOR_HTML_BLACK)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPTunnelElement(VDataProviderPtr(new VAprinfoTunnelData(GLOBAL_APRINFO, false)), aprinfo_axes, COLOR_HTML_BLACK)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPCrossingElement(VDataProviderPtr(new VAprinfoCrossingData(GLOBAL_APRINFO)), aprinfo_axes, COLOR_HTML_BLACK)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPSpalaElement(VDataProviderPtr(new VAprinfoSpalaData(GLOBAL_APRINFO)), aprinfo_axes, COLOR_HTML_BLACK, min_line_width)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPStraightSideElement(VDataProviderPtr(new VAprinfoStraightSideData(GLOBAL_APRINFO, DETECT_OPTIONS.UsePassport())), aprinfo_axes, COLOR_HTML_BLACK, min_line_width)));
	
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPSwitchesElement(opt_switches, aprinfo_axes, OPT_SWITCH_COLOR, OPT_SWITCH_COLOR)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPSwitchesElement(apr_switches, aprinfo_axes, APR_SWITCH_COLOR, APR_SWITCH_COLOR)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPSwitchSectionsElement(opt_switch_sections, aprinfo_axes, OPT_SWITCH_COLOR, OPT_SWITCH_COLOR)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPSwitchSectionsElement(apr_switch_sections, aprinfo_axes, APR_SWITCH_COLOR, APR_SWITCH_COLOR)));
	
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VXMarkGraph(rfid, aprinfo_axes, COLOR_HTML_BLACK, 0.25, 1.0, AREA_TOP, false)));

	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(wear_axes, 0.0, L"0")), wear_axes, ZERO_DASH, ZERO_COLOR, CONSTANT_LINE_WIDTH, CLIP_NONE)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(wear_axes, 14.0, L"14")), wear_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH, CLIP_NONE)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(wear_axes, 16.0, L"16")), wear_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH, CLIP_NONE)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(wear_axes, 18.0, L"18")), wear_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH, CLIP_NONE)));				
	
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(wear, wear_axes, PARAMETER_COLOR, CORRUPT_COLOR, PARAMETER_LINE_WIDTH, CLIP_HORIZONTAL)));
	
	if (DETECT_OPTIONS.GetOption(L"SHOW_PASSPORT_WEAR").Compare(OPTION_YES))
	{		
		GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VLineGraph(wear_passport, wear_axes, ORIGINAL_NORMA_COLOR, CORRUPT_COLOR, NORMA_LINE_WIDTH, CLIP_HORIZONTAL)));		
	}
		
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(zazor_left_axes, 0.0, L"0")), zazor_left_axes, ZERO_DASH, ZERO_COLOR, CONSTANT_LINE_WIDTH, CLIP_NONE)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(zazor_left_axes, 25.0, L"25")), zazor_left_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH, CLIP_NONE)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(zazor_left_axes, 30.0, L"30")), zazor_left_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH, CLIP_NONE)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(zazor_left_axes, 35.0, L"35")), zazor_left_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH, CLIP_NONE)));
		
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(zazor_right_axes, 0.0, L"0")), zazor_right_axes, ZERO_DASH, ZERO_COLOR, CONSTANT_LINE_WIDTH, CLIP_NONE)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(zazor_right_axes, 25.0, L"25")), zazor_right_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH, CLIP_NONE)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(zazor_right_axes, 30.0, L"30")), zazor_right_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH, CLIP_NONE)));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VConstantLineYElement(VYConstantDataPtr(new VYConstantData(zazor_right_axes, 35.0, L"35")), zazor_right_axes, CONSTANT_DASH, CONSTANT_COLOR, CONSTANT_LINE_WIDTH, CLIP_NONE)));
	
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VStickGraph(zazor_left_opt, zazor_left_axes, PARAMETER_COLOR, CORRUPT_COLOR, PARAMETER_LINE_WIDTH, CLIP_HORIZONTAL)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VStickGraph(zazor_right_opt, zazor_right_axes, PARAMETER_COLOR, CORRUPT_COLOR, PARAMETER_LINE_WIDTH, CLIP_HORIZONTAL)));
	
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VXMarkGraph(zazor_left_mag, zazor_left_axes, COLOR_HTML_GREEN, default_line_width, 2.0, AREA_BELOW)));
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VXMarkGraph(zazor_right_mag, zazor_right_axes, COLOR_HTML_GREEN, default_line_width, 2.0, AREA_BELOW)));
	
	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPOverstepElement(wear_oversteps, wear_axes, OVERSTEP_COLOR, OVERSTEP_FILL_COLOR)));
		
	GetLayer(std::wstring(L"Events"))->AddElement(VElementPtr(new VKVLPEventElement(events, event_axes)));

	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VXLineGraph(borders, graph_axes, Y_LINE_DASH, Y_LINE_COLOR, NORMA_LINE_WIDTH)));

	GetLayer(std::wstring(L"Graphs"))->AddElement(VElementPtr(new VKVLPPassportStatusElement(passport_status, graph_axes)));

	AdjustAreas();
	SetScaleX(DETECT_OPTIONS.GetScaleCoord());

	for (axes_itr i=axes_areas.begin(); i!=axes_areas.end(); ++i)
	{
		(*i)->SetInverseX(true);
	}
}