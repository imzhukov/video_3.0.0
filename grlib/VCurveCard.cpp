#include "VCurveCard.h"


//----------------------------------------- Методы класса VCurveCard -------------------------------

/** Конструктор*/
VCurveCard::VCurveCard(VDataSetPtr in_data_set, PARAMETER_TYPE in_mark_type, size_t in_index)
{		
	data_set=in_data_set;	
	mark_type=in_mark_type;
	index=in_index;
	//Меняем формат - это несколько уменьшит размер картинок
	cairo_format=CAIRO_FORMAT_RGB16_565;
};

/** Подготовка данных*/
void VCurveCard::Prepare(double in_width, double in_height)
{
	//Устанавливаем размеры	
	SetDevice(VDrawingDevicePtr(new VDrawingDeviceDisplay(DPI_DEFAULT_SCREEN, DPI_DEFAULT_SCREEN, in_width, in_height)));

	//Определяем свойства кривой	
	std::vector<VCurveMark> curve_marks = data_set->GetCurves(mark_type).GetItem(index).GetPlanMarkList();
	std::vector<VCurveMark> level_marks = data_set->GetCurves(mark_type).GetItem(index).GetLevelMarkList();
	double min_curv_mark = FLT_MAX;
	double max_curv_mark = (0-FLT_MAX);
	double min_level_mark = FLT_MAX;
	double max_level_mark = (0-FLT_MAX);
	double max_anp=OPTIONS.THRESHOLD_ANP_PASS();
	double min_anp=-OPTIONS.THRESHOLD_ANP_TRUCK();	
	for (size_t i=0; i<curve_marks.size(); ++i)
	{
		min_curv_mark=std::min<double>(min_curv_mark, curve_marks.at(i).value);
		max_curv_mark=std::max<double>(max_curv_mark, curve_marks.at(i).value);
	}
	for (size_t i=0; i<level_marks.size(); ++i)
	{
		min_level_mark=std::min<double>(min_level_mark, level_marks.at(i).value);
		max_level_mark=std::max<double>(max_level_mark, level_marks.at(i).value);
	}	
	double abscoord_start=std::min<double>(
		data_set->GetCurves(mark_type).GetItem(index).GetRangeCoord(*data_set, true, false, true), 
		data_set->GetCurves(mark_type).GetItem(index).GetRangeCoord(*data_set, false, false, true))-10;
	double abscoord_end=std::max<double>(
		data_set->GetCurves(mark_type).GetItem(index).GetRangeCoord(*data_set, true, false, false), 
		data_set->GetCurves(mark_type).GetItem(index).GetRangeCoord(*data_set, false, false, false))+10;				
	size_t index_from = data_set->GetAbsCoord().FindIndex(abscoord_start, true);
	size_t index_to = data_set->GetAbsCoord().FindIndex(abscoord_end, true);	


	// Здесь должен быть некий компромисс. Большое число масштабов снижает производительность (при копировании 
	// VScaleQueue), но в то же время их число должно быть достаточным, чтобы корректно отобразить любую кривую.
	// Возможно, зная длину кривой, можно вычислить небольшой набор подходящих масштабов.	
	VScaleQueue scales_x(floor((abscoord_end-abscoord_start)/20), ceil((abscoord_end-abscoord_start)/10), 1);

	//Создаем слои
	AddLayer(std::wstring(L"Background"));
	AddLayer(std::wstring(L"Frames"));
	AddLayer(std::wstring(L"GridLines"));
	AddLayer(std::wstring(L"Graph"));
	AddLayer(std::wstring(L"Legend"));

	//Создаем объект - сетка путевых координат
	coordinates = VCoordinatesDataPtr(new VCoordinatesData(data_set->GetAprinfo(), pro::interval<double>(abscoord_start, abscoord_end)));

	//Фон
	GetLayer(std::wstring(L"Background"))->AddElement(VElementPtr(new VBackgroundElement(VColor(255,255,255,255))));

	//Кривизна
	plan_area=VAreaPtr(new VArea);
	plan_area->SetSize(550, 100);
	plan_area->SetPosition(45, 20);
	plan_axes=VAxesPtr(new VAxes(plan_area, device));
	curv_data = VParameterDataPtr(new VParameterData(data_set, PRM_CURV));
	md_curv_data = VParameterDataPtr(new VParameterData(data_set, PRM_MID_CURV));	
	curv_mark_data = VCurveMarksDataPtr(new VCurveMarksData(data_set, PRM_CURV, mark_type));	
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VAreaFrameElement(plan_area, device)));
	GetLayer(std::wstring(L"GridLines"))->AddElement(VElementPtr(new VCoordinatesGridElement(coordinates, plan_axes)));
	GetLayer(std::wstring(L"GridLines"))->AddElement(VElementPtr(new VGridLinesYElement(plan_axes, true)));
	VLegendElementPtr plan_area_legend=VLegendElementPtr(new VLegendElement(plan_axes));
	GetLayer(std::wstring(L"Legend"))->AddElement(plan_area_legend);	
	if (OPTIONS.GetOption(L"CURVE_CARD_SHOW_CURV_MEAN").Compare(OPTION_YES))
		GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VLineGraph(md_curv_data, plan_axes, VColor(0,0,128), VColor(0,0,128), 0.5, CLIP_HORIZONTAL)));
	if (OPTIONS.GetOption(L"CURVE_CARD_SHOW_CURV").Compare(OPTION_YES))
		GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VLineGraph(curv_data, plan_axes, VColor(0,128,0), VColor(0,0,128), 0.5, CLIP_HORIZONTAL)));
	plan_area_legend->AddGraph(L"Кривизна", VColor(0,0,128));
	GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VCurveMarksGraph(curv_mark_data, plan_axes, VColor(0,0,255), VColor(0,255,0), CLIP_HORIZONTAL)));	
	
	plan_axes->SetAvailScaleX(scales_x);
	plan_axes->SetBestScaleX(abscoord_start, abscoord_end);
	plan_axes->SetAvailScaleY(VScaleQueue(AVAIL_SCALES_Y_CURVE));	
	plan_axes->SetBestScaleY(
		std::min<double>(min_curv_mark, data_set->GetParameter(PRM_CURV).GetMin(index_from, index_to))-10,
		std::max<double>(max_curv_mark, data_set->GetParameter(PRM_CURV).GetMax(index_from, index_to))+10);	

	//Уровень
	level_area=VAreaPtr(new VArea);
	level_area->SetSize(550, 100);
	level_area->SetPosition(45, 120);
	level_axes=VAxesPtr(new VAxes(level_area, device));	
	level_data = VParameterDataPtr(new VParameterData(data_set, PRM_LEVEL));
	md_level_data = VParameterDataPtr(new VParameterData(data_set, PRM_MID_LEVEL));	
	level_mark_data = VCurveMarksDataPtr(new VCurveMarksData(data_set, PRM_LEVEL, mark_type));		
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VAreaFrameElement(level_area, device)));
	GetLayer(std::wstring(L"GridLines"))->AddElement(VElementPtr(new VCoordinatesGridElement(coordinates, level_axes)));
	GetLayer(std::wstring(L"GridLines"))->AddElement(VElementPtr(new VGridLinesYElement(level_axes, true)));
	VLegendElementPtr level_area_legend=VLegendElementPtr(new VLegendElement(level_axes));
	GetLayer(std::wstring(L"Legend"))->AddElement(level_area_legend);
	if (OPTIONS.GetOption(L"CURVE_CARD_SHOW_LEVEL_MEAN").Compare(OPTION_YES))
		GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VLineGraph(md_level_data, level_axes, VColor(0,0,128), VColor(0,0,128), 0.5, CLIP_HORIZONTAL)));
	if (OPTIONS.GetOption(L"CURVE_CARD_SHOW_LEVEL").Compare(OPTION_YES))
		GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VLineGraph(level_data, level_axes, VColor(0,128,0), VColor(0,0,128), 0.5, CLIP_HORIZONTAL)));
	level_area_legend->AddGraph(L"Возвышение[мм]", VColor(0,0,128));
	GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VCurveMarksGraph(level_mark_data, level_axes, VColor(0,0,255), VColor(0,255,0), CLIP_HORIZONTAL)));

	level_axes->SetAvailScaleX(scales_x);
	level_axes->SetBestScaleX(abscoord_start, abscoord_end);
	level_axes->SetAvailScaleY(VScaleQueue(AVAIL_SCALES_Y_LEVEL));
	level_axes->SetBestScaleY(
		std::min<double>(min_level_mark, data_set->GetParameter(PRM_LEVEL).GetMin(index_from, index_to))-10, 
		std::max<double>(max_level_mark, data_set->GetParameter(PRM_LEVEL).GetMax(index_from, index_to))+10);

	//Непогашенное ускорение
	anp_area=VAreaPtr(new VArea);
	anp_area->SetSize(550, 100);
	anp_area->SetPosition(45, 220);
	anp_axes=VAxesPtr(new VAxes(anp_area, device));
	anp_pass = (mark_type==PRM_CURVE_MARKS) ?
		VDataProviderPtr(new VCurveParameterData(data_set, index, ANP_PASS, mark_type)) :
		VDataProviderPtr(new VAnpModelData(data_set, pass_prk, min_curv_mark<0));
	anp_truck = (mark_type==PRM_CURVE_MARKS) ?
		VDataProviderPtr(new VCurveParameterData(data_set, index, ANP_TRUCK, mark_type)) :
		VDataProviderPtr(new VAnpModelData(data_set, crg_prk, min_curv_mark<0));
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VAreaFrameElement(anp_area, device)));
	GetLayer(std::wstring(L"GridLines"))->AddElement(VElementPtr(new VCoordinatesGridElement(coordinates, anp_axes)));
	
	VLegendElementPtr anp_area_legend=VLegendElementPtr(new VLegendElement(anp_axes));
	GetLayer(std::wstring(L"Legend"))->AddElement(anp_area_legend);
	GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VLineGraph(anp_pass, anp_axes, VColor(0,0,128), VColor(0,0,128), 0.5, CLIP_HORIZONTAL)));
	anp_area_legend->AddGraph(L"Aнп пасс.", VColor(0,0,128));
	GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VLineGraph(anp_truck, anp_axes, VColor(0,128,0), VColor(0,128,0), 0.5, CLIP_HORIZONTAL)));
	anp_area_legend->AddGraph(L"Aнп груз.", VColor(0,128,0));
	
	if (mark_type==PRM_CURVE_MARKS)
	{
		//Если есть высокие скорости, добавляем Анп для них
		if (data_set->GetCurves(mark_type).GetItem(index).HasVprkHigh(*data_set))
		{
			VDataProviderPtr anp_high = VDataProviderPtr(new VCurveParameterData(data_set, index, ANP_HIGH, mark_type));
			GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VLineGraph(anp_high, anp_axes, 
				COLOR_HTML_TEAL, COLOR_HTML_TEAL, 0.5, CLIP_HORIZONTAL)));
			anp_area_legend->AddGraph(L"Aнп ск", COLOR_HTML_TEAL);
		}
		
		if (data_set->GetCurves(mark_type).GetItem(index).HasVprkHighBT(*data_set))
		{
			VDataProviderPtr anp_high_bt = VDataProviderPtr(new VCurveParameterData(data_set, index, ANP_HIGH_BT, mark_type));
			GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VLineGraph(anp_high_bt, anp_axes, 
				COLOR_HTML_OLIVE, COLOR_HTML_OLIVE, 0.5, CLIP_HORIZONTAL)));
			anp_area_legend->AddGraph(L"Aнп снк", COLOR_HTML_OLIVE);
		}
	}
	else
	{
		//Если есть высокие скорости, добавляем Анп для них
		if (data_set->GetCurves(mark_type).GetItem(index).GetModelVhigh()!=NO_SPEED)
		{
			
			VDataProviderPtr anp_high = VDataProviderPtr(new VAnpModelData(data_set, high_prk, min_curv_mark<0));
			GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VLineGraph(anp_high, anp_axes, 
				COLOR_HTML_TEAL, COLOR_HTML_TEAL, 0.5, CLIP_HORIZONTAL)));
			anp_area_legend->AddGraph(L"Aнп ск", COLOR_HTML_TEAL);
		}
		if (data_set->GetCurves(mark_type).GetItem(index).GetModelVhigh_bt()!=NO_SPEED)
		{
			
			VDataProviderPtr anp_high_bt = VDataProviderPtr(new VAnpModelData(data_set, high_bt_prk, min_curv_mark<0));
			GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VLineGraph(anp_high_bt, anp_axes, 
				COLOR_HTML_TEAL, COLOR_HTML_TEAL, 0.5, CLIP_HORIZONTAL)));
			anp_area_legend->AddGraph(L"Aнп снк", COLOR_HTML_OLIVE);
		}
	}	
	
	
	if (data_set->GetCurves(mark_type).GetItem(index).HasVprkHigh(*data_set))
	{
		GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VConstantLineYElement(VDataProviderPtr(
			new VYConstantData(anp_axes, OPTIONS.THRESHOLD_ANP_HIGH(), boost::str(boost::wformat(L"%0.1f") % OPTIONS.THRESHOLD_ANP_HIGH()))), anp_axes, DASH3, VColor(0,0,0,100))));
		max_anp=OPTIONS.THRESHOLD_ANP_HIGH();
	}
	
	if (data_set->GetCurves(mark_type).GetItem(index).HasVprkHighBT(*data_set))
	{
		GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VConstantLineYElement(VDataProviderPtr(
			new VYConstantData(anp_axes, OPTIONS.THRESHOLD_ANP_HIGH_BT(), boost::str(boost::wformat(L"%0.1f") % OPTIONS.THRESHOLD_ANP_HIGH_BT()))), anp_axes, DASH3, VColor(0,0,0,100))));
		max_anp=OPTIONS.THRESHOLD_ANP_HIGH_BT();
	}
	
	GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VConstantLineYElement(VDataProviderPtr(
		new VYConstantData(anp_axes, OPTIONS.THRESHOLD_ANP_PASS(), 
		boost::str(boost::wformat(L"%0.1f") % OPTIONS.THRESHOLD_ANP_PASS()))), anp_axes, DASH3, VColor(0,0,0,100))));
		
	GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VConstantLineYElement(VDataProviderPtr(
		new VYConstantData(anp_axes, OPTIONS.THRESHOLD_ANP_TRUCK(), 
		boost::str(boost::wformat(L"%0.1f") % OPTIONS.THRESHOLD_ANP_TRUCK()))), anp_axes, DASH3, VColor(0,0,0,100))));
		
	GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VConstantLineYElement(VDataProviderPtr(
		new VYConstantData(anp_axes, -OPTIONS.THRESHOLD_ANP_TRUCK(), 
		boost::str(boost::wformat(L"%0.1f") % -OPTIONS.THRESHOLD_ANP_TRUCK()))), anp_axes, DASH3, VColor(0,0,0,100))));		
	
	anp_axes->SetAvailScaleX(scales_x);
	anp_axes->SetBestScaleX(abscoord_start, abscoord_end);
	anp_axes->SetMinVisibleY(min_anp);
	anp_axes->SetMaxVisibleY(max_anp);
	anp_axes->SetDefaultScaleY();	

	//Скорости

	VAreaPtr speed_area=VAreaPtr(new VArea);
	speed_area->SetSize(550, 20);
	speed_area->SetPosition(45, 320);	
	VAxesPtr speed_axes=VAxesPtr(new VAxes(speed_area, device));	

	VDataProviderPtr speed_data = VAprinfoSpeedSectionDataPtr(new VAprinfoSpeedSectionData(data_set->GetAprinfo(), 
		abscoord_start, abscoord_end));

	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VAreaFrameElement(speed_area, device)));	
	GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VWaySectionGraph(speed_data, speed_axes, VColor(0,0,0,255), VColor(0,0,0,40), default_line_width)));

	speed_axes->SetAvailScaleX(scales_x);
	speed_axes->SetBestScaleX(abscoord_start, abscoord_end);		
	
	//Отклонения/износ
	diff_area=VAreaPtr(new VArea);	
	diff_area->SetSize(550, 100);
	diff_area->SetPosition(45, 340);	
	diff_axes=VAxesPtr(new VAxes(diff_area, device));	
	GetLayer(std::wstring(L"Frames"))->AddElement(VElementPtr(new VAreaFrameElement(diff_area, device)));
	GetLayer(std::wstring(L"GridLines"))->AddElement(VElementPtr(new VCoordinatesGridElement(coordinates, diff_axes)));
	GetLayer(std::wstring(L"GridLines"))->AddElement(VElementPtr(new VCoordinatesTextElement(coordinates, diff_axes)));
	GetLayer(std::wstring(L"GridLines"))->AddElement(VElementPtr(new VPreviousKmElement(diff_axes, coordinates)));
	GetLayer(std::wstring(L"GridLines"))->AddElement(VElementPtr(new VGridLinesYElement(diff_axes, true)));	
	VLegendElementPtr diff_area_legend=VLegendElementPtr(new VLegendElement(diff_axes));
	GetLayer(std::wstring(L"Legend"))->AddElement(diff_area_legend);						
	if (mark_type==PRM_CURVE_MARKS)
	{
		VDataProviderPtr wear_data = VParameterDataPtr(new VParameterData(data_set, PRM_UNI_WEAR));
		GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VLineGraph(wear_data, diff_axes, COLOR_HTML_OLIVE, VColor(L"0x80800040"), 0.5, CLIP_HORIZONTAL)));
		diff_area_legend->AddGraph(L"Износ[мм]", COLOR_HTML_OLIVE);			
		diff_axes->SetBestScaleY(-2.0, std::max<double>(20.0, data_set->GetParameter(PRM_UNI_WEAR).GetMax(index_from, index_to)));
	}
	else
	{
		diff_data = VParameterDataPtr(new VParameterData(data_set, PRM_MODEL_PLAN_OFFSET));			
		GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VLineGraph(diff_data, diff_axes, COLOR_HTML_OLIVE, COLOR_HTML_OLIVE, 0.5, CLIP_HORIZONTAL)));
		diff_area_legend->AddGraph(L"Отклонения[см]", COLOR_HTML_OLIVE);	
		VDataProviderPtr range(new VSymmetricYRangeOptionData(diff_axes, L"MAX_PLAN_OFFSET"));
		GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VSquareGraph(range, diff_axes, VColor(255,0,0), VColor(255,0,0,30))));				
		diff_axes->SetBestScaleY(
			std::min<double>(-0.3, data_set->GetParameter(PRM_MODEL_PLAN_OFFSET).GetMin(index_from, index_to)),
			std::max<double>(0.3, data_set->GetParameter(PRM_MODEL_PLAN_OFFSET).GetMax(index_from, index_to)));
	}
	diff_axes->SetAvailScaleX(scales_x);
	diff_axes->SetBestScaleX(abscoord_start, abscoord_end);	
};
