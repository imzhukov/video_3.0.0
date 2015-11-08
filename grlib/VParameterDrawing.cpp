
//boost
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/xpressive/xpressive.hpp>

#include "VParameterDrawing.h"
#include "strlib.h"
#include "Shiny.h"
#include "xpm.h"

#include "VParamCorrections.h"

using namespace boost::xpressive;

//---------------------------------- Методы класса VParameterDrawing ---------------------------------------

// [mg] NB! убрано из инлайна для сокращения кол-ва мусора в варнингах 
// при сборке boost::algorithm::split() 
VCommandParser::VCommandParser(std::wstring& in_command)
{
	//Парсим команду
	boost::iterator_range<std::wstring::iterator> f=boost::algorithm::find_first(in_command, L"(");
	boost::iterator_range<std::wstring::iterator> l=boost::algorithm::find_last(in_command, L"");
	command=std::wstring(in_command.begin(), f.begin());
	parameter=std::wstring(f.end(),l.begin());		
	if (parameter.size()>0)
	{
		boost::algorithm::split(parameters, parameter, boost::is_any_of(L","));				
		for_each(parameters.begin(), parameters.end(), trimmer());
	}
};

//---------------------------------- Методы класса VParameterDrawing ---------------------------------------

/** Возвращает корневой узел*/
VXMLGraphics& VParameterDrawing::GetGraphics()
{
	return drawing_template.GetGraphics();
};

/** Возвращает корневой узел*/
const VXMLGraphics& VParameterDrawing::GetGraphics() const
{
	return drawing_template.GetGraphics();
};


/** Создает заголовок*/
void VParameterDrawing::CreateHeader()
{		
	VGraphInfoPtr graph_info(new VGraphInfo(GetGraphics().GetName(), data_set, GetActiveDataSet(), GetActualDir(), 
		GetActualWay(), GetActualDate().ToWideStringLocale()));
	GetLayer(std::wstring(L"GridLines"))->AddElement(VGraphHeaderElementPtr(new VGraphHeaderElement(header_area,
		device, graph_info)));
	GetLayer(std::wstring(L"GridLines"))->AddElement(VDataSetsElementPtr(new VDataSetsElement(header_area,
		device, graph_info)));	
}

/** Создает слои*/
void VParameterDrawing::CreateLayers()
{
	AddLayer(std::wstring(L"Background"));
	AddLayer(std::wstring(L"PageNumbers"));
	AddLayer(std::wstring(L"GridLines"));
	AddLayer(std::wstring(L"Graph"));
	AddLayer(std::wstring(L"Active"));
	AddLayer(std::wstring(L"Frames"));	
	AddLayer(std::wstring(L"Legend"));
	AddLayer(std::wstring(L"Dynamic"));
	AddLayer(std::wstring(L"Selecting"));		
	GetLayer(std::wstring(L"PageNumbers"))->SetVisible(false);				
	GetLayer(std::wstring(L"Dynamic"))->SetVisible(false);
}

/** Создает область*/	
VAreaPtr VParameterDrawing::CreateArea(VXMLArea& in_xml_area)
{
	//Создаем область				
	VAreaPtr area=VAreaPtr(new VArea());
	areas.insert(area_pair(in_xml_area.GetName(), area));
	if (in_xml_area.GetShowFrame())
	{
		VElementPtr area_frame=VElementPtr(new VAreaFrameElement(area, device));
		GetLayer(std::wstring(L"Frames"))->AddElement(area_frame);
		Subscribe(EVENT_MOUSE_MOTION, area);
	}
	if (in_xml_area.GetShowName())
	{
		VElementPtr area_name=VElementPtr(new VAreaNameElement(area, device, in_xml_area.GetName()));
		GetLayer(std::wstring(L"Legend"))->AddElement(area_name);		
	}
	//Создаем сайзер
	VAreaSizerPtr area_sizer=VAreaSizerPtr(new VAreaSizer(area, 0, in_xml_area.GetRelPos(), 100, in_xml_area.GetRelSize()));
	area_sizers.push_back(area_sizer);	
	return area;
}

/** Создает оси*/
VAxesPtr VParameterDrawing::CreateAxes(VXMLAxes& in_xml_axes, VAreaPtr in_area)
{
	VAxesPtr axes=VAxesPtr(new VAxes(in_area, device));
	axes->SetRelative(in_xml_axes.GetRelative());	
	axes->SetLegendY(in_xml_axes.GetLegendY());
	axes->SetAvailScaleX(VScaleQueue(DEFAULT_X_SCALES));
	axes_areas.push_back(axes);
	std::vector<VActionPtr> axes_actions;		
	actions.insert(actions_pair(axes, axes_actions));


	VAutoScaleActionPtr auto_scale(new VAutoScaleAction(axes, L"Автоматический масштаб"));
	RegisterAction(auto_scale, axes);

	VDefaultScaleActionPtr default_scale(new VDefaultScaleAction(axes, L"Масштаб по умолчанию"));
	RegisterAction(default_scale, axes);
		
	VActionPtr zoom_in_y(new VZoomInYAction(axes, L"Уменьшить масштаб по оси Y (Z)"));
	zoom_in_y->SetHotKey(HK_Z);
	Subscribe(EVENT_KEY_DOWN, zoom_in_y);
	RegisterAction(zoom_in_y, axes);
	
	VActionPtr zoom_out_y(new VZoomOutYAction(axes, L"Увеличить масштаб по оси Y (X)"));
	zoom_out_y->SetHotKey(HK_X);
	Subscribe(EVENT_KEY_DOWN, zoom_out_y);
	RegisterAction(zoom_out_y, axes);
	
	VActionPtr move_origin_up(new VMoveOriginY(axes, 1.0, L"Сдвинуть график вверх"));
	move_origin_up->SetHotKey(HK_UP);
	Subscribe(EVENT_KEY_DOWN, move_origin_up);
	VActionPtr move_origin_down(new VMoveOriginY(axes, -1.0, L"Сдвинуть график вниз"));
	move_origin_down->SetHotKey(HK_DOWN);
	Subscribe(EVENT_KEY_DOWN, move_origin_down);

	return axes;
};

/** Добавляет действие в меню*/
void VParameterDrawing::AddMenuAction(VActionPtr in_action)
{
	standart_actions.push_back(in_action);
};

/** Связывает действие с системой координат*/
void VParameterDrawing::RegisterAction(VActionPtr in_action, VAxesPtr in_axes)
{
	actions.find(in_axes)->second.push_back(in_action);
};

/** Посмещает действие в тулбар*/
void VParameterDrawing::AddToolbarAction(VActionPtr in_action, xpm_t* in_enabled_xpm, xpm_t* in_disabled_xpm)
{
	in_action->SetEnabledXPM(in_enabled_xpm);
	in_action->SetDisabledXPM(in_disabled_xpm);		
	toolbar_actions.push_back(in_action);
};

/** Устанавливат способ масштабирования для осей*/
void VParameterDrawing::SetScaleMethod(VXMLAxes in_xml_axes, VAxesPtr in_axes)
{
	if (in_xml_axes.GetScaleMethod().compare(L"Масштаб")==0)
	{		
		in_axes->SetAvailScaleY(VScaleQueue(in_xml_axes.GetAvailScaleY()));		
		in_axes->SetScaleY(str_to_double(in_xml_axes.GetScaleY()));
		in_axes->SetStartY((in_axes->GetMinDataY()+in_axes->GetMaxDataY())/2);
		in_axes->SetOriginY(50);
	}					
	else
	{				
		double min_y;
		double max_y;
		if (in_xml_axes.GetMinY().compare(L"auto")==0 || in_xml_axes.GetMinY().compare(L"")==0)
			min_y=in_axes->GetMinDataY();
		else
			min_y=str_to_double(in_xml_axes.GetMinY());

		if (in_xml_axes.GetMaxY().compare(L"auto")==0 || in_xml_axes.GetMaxY().compare(L"")==0)
			max_y=in_axes->GetMaxDataY();
		else
			max_y=str_to_double(in_xml_axes.GetMaxY());				
		//Устанавливаем минимальное и максимальное видимые значения
		in_axes->SetMinVisibleY(min_y);
		in_axes->SetMaxVisibleY(max_y);		
	}	
};

/** Возвращает true если имеем дело со станционными путями*/
bool VParameterDrawing::IsStationWay() const
{	
	return route_section.GetCode()==0;
}

/** Возвращает актуальную дату графика*/
VTime VParameterDrawing::GetActualDate()
{
	switch (GetGraphics().GetDate())
	{
		default:
		case DATE_CURRENT:
			return VTime();
			break;
		case DATE_TRAIL:
			return GetActiveDataSet() ? GetActiveDataSet()->GetRailDate() : VTime();
			break;
	}	
}

/** Возвращает код направления*/
std::wstring VParameterDrawing::GetActualDir()
{
	if (data_set.empty())
		return boost::lexical_cast<std::wstring>(aprinfo->GetDirCode());
	else
		return boost::lexical_cast<std::wstring>(data_set.front()->GetDir());
}

/** Возвращает код пути*/
std::wstring VParameterDrawing::GetActualWay()
{
	if (data_set.empty())
		return string_to_wstring(aprinfo->GetWayCode());
	else
		return string_to_wstring(data_set.front()->GetWay());
}

/** Возвращает участок пути*/
VWaySection VParameterDrawing::GetActualSection()
{
	VWaySection actual_section;
	actual_section.start=GetRealCoord(database, wstring_to_string(GetActualWay()).c_str(), 
		wstring_to_string(GetActualDir()).c_str(), 
		GetMinAbs(), GetActualDate());
	actual_section.end=GetRealCoord(database, wstring_to_string(GetActualWay()).c_str(), 
		wstring_to_string(GetActualDir()).c_str(),
		GetMaxAbs(), GetActualDate());
	return actual_section;
}

/** Возвращает активный файл*/
VDataSetPtr VParameterDrawing::GetActiveDataSet()
{
	if (!active_data_set)
	{
		//Активный файл данных
		switch (data_set.size())
		{
			case 0:				
				break;
			case 1:
				active_data_set=data_set.back();
				break;
			default:
			{
				std::vector<std::wstring> list;
				for (size_t i=0; i<data_set.size(); ++i)
					list.push_back(boost::str(boost::wformat(L" %s %s" ) % 
						data_set.at(i)->GetFileShortName() % data_set.at(i)->GetRailDate().ToWideStringLocale()));
				select_dialog->SetList(list);					
				select_dialog->SetSelectedItem(list.front());
				//Вызываем диалог
				if (select_dialog->ShowModal(L"Выбор активного файла", L"Загруженные файлы"))
				{				
					active_data_set=data_set.at(select_dialog->GetSelectedPos());
				}
				else
				{
					throw VUserException(L"Построение графика прервано пользователем");
				}
				break;
			}
		}					
	}
	return active_data_set;
}

/** Возвращает эталонный файл*/
VDataSetPtr VParameterDrawing::GetReferenceDataSet()
{
	if (!reference_data_set)
	{
		switch (data_set.size())
		{
			case 0:
				throw VSimpleException(L"Невозможно выбрать эталонный файл", L"Не загружен ни один файл", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			case 1:
				reference_data_set=data_set.front();
				break;
			case 2:
				reference_data_set=GetActiveDataSet()==data_set.front() ? data_set.back() : data_set.front();
				break;
			default:
			{
				std::vector<std::wstring> list;
				for (size_t i=0; i<data_set.size(); ++i)
				{
					if (data_set.at(i)!=GetActiveDataSet())
					{
						list.push_back(boost::str(boost::wformat(L" %s %s" ) % 
							data_set.at(i)->GetFileShortName() % data_set.at(i)->GetRailDate().ToWideStringLocale()));
					}
				}
				select_dialog->SetList(list);					
				select_dialog->SetSelectedItem(list.front());
				//Вызываем диалог
				if (select_dialog->ShowModal(L"Выбор эталонного файла", L"Загруженные файлы"))
				{				
					size_t index=BAD_INDEX;
					for (size_t i=0; i<data_set.size(); ++i)
					{
						if (select_dialog->GetSelectedItem().compare(boost::str(boost::wformat(L" %s %s" ) % 
							data_set.at(i)->GetFileShortName() % data_set.at(i)->GetRailDate().ToWideStringLocale()))==0)
							index=i;
					}
					if (index!=BAD_INDEX)
						reference_data_set=data_set.at(index);
					else
						throw ITEM_NOT_FOUND;
				}
				break;
			}
		}		
	}
	return reference_data_set;
}

/** Возвращает минимальную координату из файлов*/
double VParameterDrawing::GetMinAbs()
{	
	double result=DBL_MAX;
	for (size_t i=0; i<data_set.size(); ++i)
	{
		result = std::min<double>(result, data_set.at(i)->GetAbsCoord().GetItem(0));
	}
	return result;
}

/** Возвращает максимальную координату из файлов*/
double VParameterDrawing::GetMaxAbs()
{
	double result=(0-DBL_MAX);
	for (size_t i=0; i<data_set.size(); ++i)
	{
		result = std::max<double>(result, data_set.at(i)->GetAbsCoord().GetLastItem());
	}
	return result;	
}

/** Загрузка данных*/
void VParameterDrawing::LoadData(const std::vector<std::wstring>& in_data_file_names, VIBDatabasePtr in_database,
	const VRouteSection& in_route_section, bool in_from_db)
{
	route_section=in_route_section;

	//Файлы данных
	for (size_t n=0; n<in_data_file_names.size(); ++n)
	{			
		//Загружаем данные
		VDataSetPtr new_data_set = in_from_db ? VDataSetPtr(new VDataSet(*database, in_data_file_names[n].c_str())) : 
			VDataSetPtr(new VDataSet(in_data_file_names[n].c_str()));				

		if (!new_data_set->FileIsLoaded())
		{
			throw VSimpleException(new_data_set->GetErrorMessage().c_str(), in_data_file_names.at(n),boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}

		data_set.push_back(new_data_set);		
		new_data_set->Load(PRM_ABS_COORD);		
	}	
	//Сортируем источники данных по убыванию даты
	struct ds_comp
	{
		bool operator()(const VDataSetPtr& l, const VDataSetPtr& r)
		{
			return l->GetRailDate() > r->GetRailDate();
		};
	};
	std::sort(data_set.begin(), data_set.end(), ds_comp());
	//Проверяем что все файлы соответствуют одному пути
	for (size_t n=1; n<in_data_file_names.size(); ++n)
	{			
		if (!(data_set.at(n-1)->GetDir()==data_set.at(n)->GetDir() &&
			  data_set.at(n-1)->GetWay()==data_set.at(n)->GetWay()))
		{
			throw VSimpleException(L"Выделены файлы, которые относятся к разным путям", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}				
	}
	//Априорная информация
	database=in_database;
	*transaction=database->StartTransaction();
	switch (drawing_template.GetGraphics().GetAprinfoSource())
	{
		default://По умолчанию из БД
		case APRINFO_FROM_DB: //Из БД
		{
			//Априорная информация из БД			
			aprinfo=VAprinfoPtr(new VAprinfo());	
			if (!IsStationWay())//Если это главный путь
			{	
				aprinfo->Makeinfo(*in_database, route_section.GetCode(), route_section.GetWay().c_str(), 
					route_section.GetSection(), GetActualDate(), false, false);
			}
			else//Если это станционный путь
			{			
				if (!data_set.empty())
				{
					uint32_t station_code=GetActiveDataSet()->GetStationCode();				
					std::string park_name(GetActiveDataSet()->GetParkName());				
					std::string way_name(GetActiveDataSet()->GetWay());
					aprinfo->MakeStWayInfo(*in_database, station_code, park_name.c_str(), way_name.c_str(), data_set.front()->GetFileShortName());
				}
				else
				{
					throw VSimpleException(L"Для построения графика требуются файлы проездов", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				}
			}
			if (!aprinfo->IsLoaded())
			{
				throw VSimpleException(L"Не удалось загрузить априорную информацию", aprinfo->GetErrorMessage().c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			//Заменяем априорную информацию в датасетах
			for (size_t i=0; i<data_set.size(); ++i)
			{				
				data_set.at(i)->GetAprinfo()=*aprinfo;
			}
			break;
		}
		case APRINFO_FROM_FILE:
		{
			if (!data_set.empty())
			{
				GetActiveDataSet()->Load(PRM_APRINFO);
				if (!GetActiveDataSet()->GetAprinfo().IsLoaded())
				{
					throw VSimpleException(L"Не удалось загрузить априорную информацию", 
						GetActiveDataSet()->GetAprinfo().GetErrorMessage().c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				}
				aprinfo=VAprinfoPtr(new VAprinfo());
				*aprinfo=GetActiveDataSet()->GetAprinfo();
				//Заменяем априорную информацию в датасетах
				for (size_t i=0; i<data_set.size(); ++i)
				{			
					if (GetActiveDataSet()!=data_set.at(i))
						data_set.at(i)->GetAprinfo()=*aprinfo;
				}
			}
			else
			{
				throw VSimpleException(L"Необходимо выделить файл", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			break;
		}
	}
}

/** Возвращает легенду графика*/
std::wstring VParameterDrawing::FormatGraphLegend(VXMLStaticGraph& xml_graph, size_t in_index, const std::wstring& in_comment) const
{
	std::wstring graph_legend=xml_graph.GetLegend();
	//Имя
	boost::algorithm::replace_all(graph_legend, L"$n", xml_graph.GetName());
	//Файл
	boost::algorithm::replace_all(graph_legend, L"$f", data_set.at(in_index)->GetFileShortName());
	//Дата
	boost::algorithm::replace_all(graph_legend, L"$t", data_set.at(in_index)->GetRailDate().ToWideStringLocale());
	//Комментарий
	boost::algorithm::replace_all(graph_legend, L"$c", in_comment);
	return graph_legend;
}

/** Создает источник данных определенного типа и возвращает указатель на него*/
std::vector<VDataProviderPtr> VParameterDrawing::CreateDataProvider(VAxesPtr in_axes, VXMLStaticGraph& in_xml_graph)
{
	//Парсим команду
	VCommandParser command_parser(in_xml_graph.GetDataType());	
	std::vector<VDataProviderPtr> data_provider;	
	//Координаты
	if (boost::algorithm::iequals(command_parser.GetCommand(), L"COORDINATES"))
	{
		data_provider.push_back(VDataProviderPtr(new VCoordinatesData(*aprinfo, pro::interval<double>(0, DBL_MAX))));
	}
	//Константа
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"CONSTANT"))
	{				
		if (command_parser.GetParameterCount()!=1)
			throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType());
		float value=boost::lexical_cast<float>(command_parser.GetParameter(0));
		if (constant_set.insert(in_xml_graph.GetDataType()).second)
		{			
			data_provider.push_back(VDataProviderPtr(new VYConstantData(in_axes, value, in_xml_graph.GetName())));
			data_provider.back()->SetLegend(in_xml_graph.GetName());			
		}
	}	
	//Диапазон
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"RANGE"))
	{				
		if (command_parser.GetParameterCount()!=2)
			throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType()); 
		float min_value=boost::lexical_cast<float>(command_parser.GetParameter(0));
		float max_value=boost::lexical_cast<float>(command_parser.GetParameter(1));
		if (constant_set.insert(in_xml_graph.GetDataType()).second)
		{
			data_provider.push_back(VDataProviderPtr(new VYRangeData(in_axes, min_value, max_value)));
			data_provider.back()->SetLegend(in_xml_graph.GetName());			
		}
	}
	//Значение опции
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"OPTION"))
	{
		if (command_parser.GetParameterCount()!=1)
			throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType()); 
		if (constant_set.insert(in_xml_graph.GetDataType()).second)
		{			
			//Проверяем наличие опции
			if (OPTIONS.HasOption(command_parser.GetParameter(0)))
			{
				data_provider.push_back(VDataProviderPtr(new VYOptionData(in_axes, command_parser.GetParameter(0))));
				data_provider.back()->SetLegend(in_xml_graph.GetName());
			}
			else
				throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType()); 
		}
	}
	//Симметричные границы (по опции)
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"SYMMETRIC_RANGE_OPTION"))
	{		
		if (command_parser.GetParameterCount()!=1)
			throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType()); 
		if (constant_set.insert(in_xml_graph.GetDataType()).second)
		{			
			//Проверяем наличие опции
			if (OPTIONS.HasOption(command_parser.GetParameter(0)))
			{
				data_provider.push_back(VDataProviderPtr(new VSymmetricYRangeOptionData(in_axes, command_parser.GetParameter(0))));
				data_provider.back()->SetLegend(in_xml_graph.GetName());			
			}
			else
				throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType()); 
		}
	}
	//Параметр
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"PARAMETER"))
	{			
		if (command_parser.GetParameterCount()!=1)
			throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType());
		PARAMETER_TYPE data_type=GetParamCode(command_parser.GetParameter(0).c_str());
		std::wstring comment=L"";

		for (size_t i=0; i<data_set.size(); ++i)
		{
			switch (data_type)
			{
				case PRM_NONE:
				{
					throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType());
				}
				case PRM_SPEED:
					data_provider.push_back(VDataProviderPtr(new VCurrentSpeedData(data_set.at(i))));
					break;
				case PRM_ZAZOR_LEFT:
					data_provider.push_back(VDataProviderPtr(new VZazorLeftData(data_set.at(i))));
					break;
				case PRM_ZAZOR_RIGHT:
					data_provider.push_back(VDataProviderPtr(new VZazorRightData(data_set.at(i))));
					break;
				case PRM_MAG_ZAZOR_LEFT:
					data_provider.push_back(VDataProviderPtr(new VMagZazorLeftData(data_set.at(i))));
					break;
				case PRM_MAG_ZAZOR_RIGHT:
					data_provider.push_back(VDataProviderPtr(new VMagZazorRightData(data_set.at(i))));
					break;
				case PRM_UGON_LEFT:
					data_provider.push_back(VDataProviderPtr(new VUgonLeftData(data_set.at(i))));
					break;
				case PRM_UGON_RIGHT:
					data_provider.push_back(VDataProviderPtr(new VUgonRightData(data_set.at(i))));
					break;
				default:					
					data_provider.push_back(VDataProviderPtr(new VParameterData(data_set.at(i), data_type)));
					comment=data_set.at(i)->GetParameter(data_type).GetComment();
					break;
			}
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i, comment));
		}
	}	
	//Метки оператора
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"OPERATOR_MARK"))
	{			
		if (command_parser.GetParameterCount()>1)
		throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType()); 
		PARAMETER_TYPE data_type=command_parser.GetParameterCount()==0 ? PRM_NONE : GetParamCode(command_parser.GetParameter(0).c_str());
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VOperatorMarkData(data_set.at(i), data_type)));	
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}	
	//RFID метки
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"SPALA_MARKER"))
	{			
		if (command_parser.GetParameterCount()>1)
			throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType());
		PARAMETER_TYPE data_type=command_parser.GetParameterCount()==0 ? PRM_NONE : GetParamCode(command_parser.GetParameter(0).c_str());
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VSpalaMarkerData(data_set.at(i), data_type)));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Маркеры пути
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"WAY_MARKER"))
	{
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VWayMarkerData(data_set.at(i))));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Функция возвращающая функцию от параметра
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"FUNCTION"))
	{
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VParameterFunctionData(data_set.at(i), 
				command_parser.GetParameters().c_str())));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}	
	//Модельное непогашенное ускорение
	else if (boost::algorithm::iequals(command_parser.GetCommand(),L"ANP_MODEL"))
	{													
		if (command_parser.GetParameterCount()!=1)
			throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType()); 
		int speed=boost::lexical_cast<int>(command_parser.GetParameter(0));
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VAnpModelData(data_set.at(i), (speed_type)speed, false)));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Разметка кривых
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"CURVE_MARKS"))
	{			
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VCurveMarksData(data_set.at(i), PRM_CURV)));	
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Разметка уровня
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"LEVEL_MARKS"))
	{					
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VCurveMarksData(data_set.at(i), PRM_LEVEL)));	
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Боковые пути
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"SIDEWAYS"))
	{
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VSidewayDataPtr(new VSidewayData(data_set.at(i))));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Опорные отметки профиля
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"APR_PROFMARKS"))
	{				
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VProfileMarkData(data_set.at(i))));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}		
	//Элементы профиля
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"PROF_ELEMENTS"))
	{	
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VProfileElementsData(data_set.at(i))));	
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}	
	//Уклоны
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"SLOPE"))
	{
		if (command_parser.GetParameterCount()!=1)
			throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType());
		PARAMETER_TYPE data_type=GetParamCode(command_parser.GetParameter(0).c_str());
		if (data_type==PRM_NONE)
			throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType());
		//Бессмысленно показывать уклоны на двух файлах - они будут накладываться поэтому используем только активный файл
		if (GetActiveDataSet())
		{
			VSlopeDataPtr slope_data = VSlopeDataPtr(new VSlopeData(GetActiveDataSet(), data_type));				
			RegisterAction(VActionPtr(new VSetSlopeType(slope_data, PRM_PROFILE_SLOPE_ELEMENT, L"Показать уклоны на элементах")), in_axes);
			RegisterAction(VActionPtr(new VSetSlopeType(slope_data, PRM_PROFILE_SLOPE_KM, L"Показать уклоны на километрах")), in_axes);
			RegisterAction(VActionPtr(new VSetSlopeType(slope_data, PRM_PROFILE_SLOPE_PK, L"Показать уклоны на пикетах")), in_axes);
			RegisterAction(VActionPtr(new VSetSlopeType(slope_data, PRM_PROFILE_SLOPE_HPK, L"Показать уклоны на половинах пикетов")), in_axes);
			data_provider.push_back(slope_data);
			data_provider.back()->SetLegend(in_xml_graph.GetName());
		}
	}
	//Отметки профиля
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"HEIGHT"))
	{
		if (command_parser.GetParameterCount()!=1)
			throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType()); 
		PARAMETER_TYPE data_type=GetParamCode(command_parser.GetParameter(0).c_str());
		if (data_type==PRM_NONE)
			throw VSimpleException(L"Неверные параметры функции", in_xml_graph.GetDataType());
		//Бессмысленно показывать отметки на двух файлах - они будут накладываться поэтому используем только активный файл
		if (GetActiveDataSet())
		{
			VHeightDataPtr height_data = VHeightDataPtr(new VHeightData(GetActiveDataSet(), data_type));		
			RegisterAction(VActionPtr(new VSetHeightType(height_data, PRM_PROFILE_SLOPE_ELEMENT, L"Показать оцифровку на элементах")), in_axes);
			RegisterAction(VActionPtr(new VSetHeightType(height_data, PRM_PROFILE_SLOPE_KM, L"Показать оцифровку на километрах")), in_axes);
			RegisterAction(VActionPtr(new VSetHeightType(height_data, PRM_PROFILE_SLOPE_PK, L"Показать оцифровку на пикетах")), in_axes);
			RegisterAction(VActionPtr(new VSetHeightType(height_data, PRM_PROFILE_SLOPE_HPK, L"Показать оцифровку на половинах пикетов")), in_axes);
			data_provider.push_back(height_data);
			data_provider.back()->SetLegend(in_xml_graph.GetName());		
		}
	}		
	//Запрос
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"QUERY"))
	{			
		//Подставляем параметры
		std::wstring _query=PredefinedQueries.GetCommand(command_parser.GetParameters());
		if (_query.compare(L"")==0)
			_query=command_parser.GetParameters();

		std::wstring query=_query;
		//Идентификатор пути
		std::wstring get_way_id=
			boost::str(boost::wformat(L" (select adm_way.id from adm_way inner join adm_direction on adm_way.adm_direction_id=adm_direction.id where adm_direction.code='%s' and adm_way.code='%s') ") %GetActualDir() %GetActualWay());
		algorithm::ireplace_all(query, L"%WAY_ID%", get_way_id);							
		algorithm::ireplace_all(query, L"%TRAIL_DATE%", GetActualDate().DateToWideStringISO());
		algorithm::ireplace_all(query, L"%START_ABS%", boost::str(boost::wformat(L"%f") % GetMinAbs()));
		algorithm::ireplace_all(query, L"%END_ABS%", boost::str(boost::wformat(L"%f") % GetMaxAbs()));
		for (subst_itr i=subst.begin(); i!=subst.end(); ++i)
		{
			algorithm::ireplace_all(query, i->first, i->second);
		}
						
		data_provider.push_back(VDataProviderPtr(new VQueryData(database, transaction, query)));
		data_provider.back()->SetLegend(in_xml_graph.GetName());
				
	}
	//Запрос данных проезда
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"TRAIL_QUERY"))
	{
		//Подставляем параметры
		std::wstring _query=PredefinedQueries.GetCommand(command_parser.GetParameters());
		if (_query.compare(L"")==0)
			_query=command_parser.GetParameters();		
		for (size_t i=0; i<data_set.size(); ++i)
		{										
			std::wstring query=_query;
			//Идентификатор пути
			std::wstring get_way_id=
				boost::str(boost::wformat(L" (select adm_way.id from adm_way inner join adm_direction on adm_way.adm_direction_id=adm_direction.id where adm_direction.code='%s' and adm_way.code='%s') ") %GetActualDir() %GetActualWay());
			algorithm::ireplace_all(query, L"%WAY_ID%", get_way_id);				
			//Идентификатор проезда		
			std::wstring get_trail_id=(!IsStationWay()) ? 
				boost::str(boost::wformat(L" (select id from msr_trail where filename='%s') ") % data_set.at(i)->GetFileName().GetShortName()) :
				boost::str(boost::wformat(L" (select id from stw_trail where filename='%s') ") % data_set.at(i)->GetFileName().GetShortName()) ;
			algorithm::ireplace_all(query, L"%TRAIL_ID%", get_trail_id);				
			algorithm::ireplace_all(query, L"%TRAIL_DATE%", GetActualDate().DateToWideStringISO());					

			for (subst_itr j=subst.begin(); j!=subst.end(); ++j)
			{
				algorithm::ireplace_all(query, j->first, j->second);
			}

			data_provider.push_back(VDataProviderPtr(new VQueryData(database, transaction, query)));					
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));				
		}
	}
	//Объекты на главном пути
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"MAIN_WAY_OBJECTS"))
	{		
		for (size_t i=0; i<data_set.size(); ++i)
		{
			std::wstring query=PredefinedQueries.GetCommand(L"PROFILE_OBJECTS");
			//Идентификатор пути
			std::wstring get_way_id=
				boost::str(boost::wformat(L" (select adm_way.id from adm_way inner join adm_direction on adm_way.adm_direction_id=adm_direction.id where adm_direction.code='%s' and adm_way.code='%s') ") %GetActualDir() %GetActualWay());
			algorithm::ireplace_all(query, L"%WAY_ID%", get_way_id);							
			algorithm::ireplace_all(query, L"%TRAIL_DATE%", GetActualDate().DateToWideStringISO());
			for (subst_itr j=subst.begin(); j!=subst.end(); ++j)
			{
				algorithm::ireplace_all(query, j->first, j->second);
			}
			data_provider.push_back(VDataProviderPtr(new VMainWayObjectData(database, transaction, query,
				data_set.at(i))));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Профиль станционного пути
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"STATION_WAY_PROFILE"))
	{		
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VStationProfileData(database, transaction, 
				data_set.at(i))));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Объекты на станционном пути
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"STATION_WAY_OBJECTS"))
	{		
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VStationWayObjectData(database, transaction, 
				data_set.at(i), false)));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}	
	//Километры на станционном пути
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"STATION_WAY_KM"))
	{		
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VStationWayKmData(database, transaction, 
				data_set.at(i))));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}				
	//Сверхнормативные вертикальные неровности
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"JAGGY_PROFILE_OVERSTEPS"))
	{		
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VJaggyProfileOverstepData(data_set.at(i))));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Сверхнормативные горизонтальные неровности
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"JAGGY_PLAN_OVERSTEPS"))
	{		
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VJaggyPlanOverstepData(data_set.at(i))));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Отжатия
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"DIFF_WIDTH_FRONT_OVERSTEPS"))
	{		
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VDiffWidthFrontOverstepData(data_set.at(i))));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Отжатия
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"DIFF_WIDTH_BACK_OVERSTEPS"))
	{		
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VDiffWidthBackOverstepData(data_set.at(i))));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Подуклонка
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"CANTING_LEFT_OVERSTEPS"))
	{		
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VCantingOverstepData(data_set.at(i), SD_LEFT)));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"CANTING_RIGHT_OVERSTEPS"))
	{		
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VCantingOverstepData(data_set.at(i), SD_RIGHT)));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Внешний износ
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"EXT_WEAR_LEFT_OVERSTEPS"))
	{		
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VExtWearOverstepData(data_set.at(i), SD_LEFT)));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	else if (boost::algorithm::iequals(command_parser.GetCommand(), L"EXT_WEAR_RIGHT_OVERSTEPS"))
	{		
		for (size_t i=0; i<data_set.size(); ++i)
		{
			data_provider.push_back(VDataProviderPtr(new VExtWearOverstepData(data_set.at(i), SD_RIGHT)));
			data_provider.back()->SetLegend(FormatGraphLegend(in_xml_graph, i));
		}
	}
	//Неопределенная команда
	else
	{
		throw VSimpleException(L"Неопределенная команда", in_xml_graph.GetDataType()); 
	}
	
	return data_provider;
}

/** Создает график*/
VElementPtr VParameterDrawing::CreateGraph(const std::wstring& in_element_name, VDataProviderPtr in_data_provider, 
	VAxesPtr in_axes, VXMLStaticGraph& in_xml_static_graph)
{
	VElementPtr graph_ptr;
	VColor color=in_xml_static_graph.GetColors().GetColor();
	VColor fill_color=in_xml_static_graph.GetFillColors().GetColor();
	//Координаты
	if (in_xml_static_graph.GetType()==L"COORDINATES")
	{
		graph_ptr=VElementPtr(new VCoordinatesTextElement(in_data_provider, in_axes));
	}
	//Константа
	else if (in_xml_static_graph.GetType()==L"CONSTANT")
	{
		VConstantLineYElement * new_graph=new VConstantLineYElement(in_data_provider, in_axes, DASH3, color, in_xml_static_graph.GetWidth(),  in_xml_static_graph.GetClip(), in_xml_static_graph.GetShowLabels());
		graph_ptr=VElementPtr(new_graph);						
	}
	//Диапазон
	else if (in_xml_static_graph.GetType()==L"RANGE")
	{
		VRangeYElement * new_graph=new VRangeYElement(in_data_provider, in_axes, color, fill_color);
		graph_ptr=VElementPtr(new_graph);						
	}	
	//Линия
	else if (in_xml_static_graph.GetType()==L"LINE")
	{
		VLineGraph * new_graph=new VLineGraph(in_data_provider, in_axes, color, fill_color,
			in_xml_static_graph.GetWidth(), in_xml_static_graph.GetClip());
		graph_ptr=VElementPtr(new_graph);
	}		
	//Точки
	else if (in_xml_static_graph.GetType()==L"POINT")
	{
		VPointGraph * new_graph=new VPointGraph(in_data_provider, in_axes, color, fill_color, 
			in_xml_static_graph.GetClip(), in_xml_static_graph.GetShowLabels());
		graph_ptr=VElementPtr(new_graph);	
	}
	//Точки с линиями
	else if (in_xml_static_graph.GetType()==L"POINT_LINE")
	{
		VPointLineGraph * new_graph=new VPointLineGraph(in_data_provider, in_axes, color, fill_color, 
			in_xml_static_graph.GetWidth(), in_xml_static_graph.GetClip(), in_xml_static_graph.GetShowLabels());
		graph_ptr=VElementPtr(new_graph);	
	}	
	//"Барчарт"
	else if (in_xml_static_graph.GetType()==L"BARCHART")
	{
		VBarGraph * new_graph = new VBarGraph(in_data_provider, in_axes, color, fill_color, 
			in_xml_static_graph.GetWidth(), in_xml_static_graph.GetClip(), in_xml_static_graph.GetShowLabels());							
		graph_ptr=VElementPtr(new_graph);
	}
	//"Прямоугольная область"
	else if (in_xml_static_graph.GetType()==L"SQUARE")
	{
		VSquareGraph * new_graph = new VSquareGraph(in_data_provider, in_axes, color, fill_color, 
			in_xml_static_graph.GetWidth(), in_xml_static_graph.GetClip(), in_xml_static_graph.GetShowLabels());							
		graph_ptr=VElementPtr(new_graph);
	}
	//"Палка"
	else if (in_xml_static_graph.GetType()==L"STICK")
	{
		VStickGraph * new_graph = new VStickGraph(in_data_provider, in_axes, color, fill_color, in_xml_static_graph.GetWidth(), 
			in_xml_static_graph.GetClip() , in_xml_static_graph.GetShowLabels());
		graph_ptr=VElementPtr(new_graph);
	}
	//"Палка с точкой"
	else if (in_xml_static_graph.GetType()==L"POINT_STICK")
	{
		VPointStickGraph * new_graph = new VPointStickGraph(in_data_provider, in_axes, color, fill_color, in_xml_static_graph.GetWidth(), 
			in_xml_static_graph.GetClip(), in_xml_static_graph.GetShowLabels());
		graph_ptr=VElementPtr(new_graph);
	}	
	//Метки на оси X
	else if (in_xml_static_graph.GetType()==L"XMARK")
	{
		VXMarkGraph * new_graph=new VXMarkGraph(in_data_provider, in_axes, color, in_xml_static_graph.GetWidth(), 2.0,
			AREA_BOTTOM, in_xml_static_graph.GetShowLabels());
		graph_ptr=VElementPtr(new_graph);	
	}
	//Границы на оси X
	else if (in_xml_static_graph.GetType()==L"XLINE")
	{
		VXLineGraph * new_graph=new VXLineGraph(in_data_provider, in_axes, DASH1, color, in_xml_static_graph.GetWidth());
		graph_ptr=VElementPtr(new_graph);	
	}
	//Простой текст по оси X
	else if (in_xml_static_graph.GetType()==L"XTEXT")
	{		
		graph_ptr=VElementPtr(new VTextGraph(in_data_provider, in_axes, TO_VERTICAL, TP_AREA_BOTTOM_OVER));
	}	
	//Участок
	else if (in_xml_static_graph.GetType()==L"SECTION")
	{
		VWaySectionGraph * new_graph=new VWaySectionGraph(in_data_provider, in_axes, color, fill_color, 
			in_xml_static_graph.GetWidth(), in_xml_static_graph.GetShowLabels());
		graph_ptr=VElementPtr(new_graph);	
	}
	//Переезды
	else if (in_xml_static_graph.GetType()==L"CROSSINGS")
	{
		VCrossingElement * new_graph=new VCrossingElement(in_data_provider, in_axes, color);
		graph_ptr=VElementPtr(new_graph);
	}	
	//Мосты
	else if (in_xml_static_graph.GetType()==L"BRIDGES")
	{
		VBridgeElement * new_graph=new VBridgeElement(in_data_provider, in_axes, color);
		graph_ptr=VElementPtr(new_graph);
	}
	//Тоннели
	else if (in_xml_static_graph.GetType()==L"TUNNELS")
	{
		VTunnelElement * new_graph=new VTunnelElement(in_data_provider, in_axes, color);
		graph_ptr=VElementPtr(new_graph);
	}	
	//Объекты на профиле
	else if (in_xml_static_graph.GetType()==L"PROF_OBJECTS")
	{
		VProfileObjectsElement * new_graph=new VProfileObjectsElement(in_data_provider, in_axes, color, fill_color);
		graph_ptr=VElementPtr(new_graph);
	}	
	//Объекты в графе "ситуация"
	else if (in_xml_static_graph.GetType()==L"SITUATION_OBJECTS")
	{
		VSituationObjectsElement * new_graph=new VSituationObjectsElement(in_data_provider, in_axes, color, fill_color);
		graph_ptr=VElementPtr(new_graph);
	}	
	//Уклоны
	else if (in_xml_static_graph.GetType()==L"SLOPE")
	{
		VSlopeGraph * new_graph=new VSlopeGraph(in_data_provider, in_axes);
		graph_ptr=VElementPtr(new_graph);
	}
	//Значки километровых столбов
	else if (in_xml_static_graph.GetType()==L"KM")
	{
		VKmElement * new_graph=new VKmElement(in_data_provider, in_axes, in_xml_static_graph.GetShowLabels());
		graph_ptr=VElementPtr(new_graph);
	}
	//Пикеты
	else if (in_xml_static_graph.GetType()==L"ПК")
	{
		VPkElement * new_graph=new VPkElement(in_data_provider, in_axes, in_xml_static_graph.GetShowLabels());
		graph_ptr=VElementPtr(new_graph);
	}	
	else
	{					
		throw VSimpleException(L"Тип графика не поддерживается: ", in_xml_static_graph.GetType(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	return graph_ptr;
}

/** Конструктор*/
VParameterDrawing::VParameterDrawing() : VDrawing()
{	
	transaction = VIBTransactionPtr(new VIBTransaction);	
}

/** Деструктор*/
VParameterDrawing::~VParameterDrawing()
{		
	try
	{
		if (transaction->Active())
		{		
			database->RollbackTransaction(*transaction);
		}
	}
	catch (...)	{}
}

/** Загрузка шаблона*/
void VParameterDrawing::LoadTemplate(const wstring& in_template_path)
{
	template_path=in_template_path;
	drawing_template.Load(in_template_path);	
}	

/** Сохранение шаблона*/
void VParameterDrawing::SaveTemplate(const wstring& in_template_path)
{
	template_path=in_template_path;
	drawing_template.Save(template_path);
}

/** Возвращает название графика*/
const std::wstring& VParameterDrawing::GetName() const
{
	return drawing_template.GetGraphics().GetName();
}

/** Возвращает название группы*/
std::wstring VParameterDrawing::GetGroup()
{
	return drawing_template.GetGraphics().GetGroup();
}

/** Возвращает фильтр*/
std::wstring VParameterDrawing::GetFilter()
{
	return drawing_template.GetGraphics().GetFilter();
}

/** Подготовка данных*/
void VParameterDrawing::Prepare(const std::vector<std::wstring>& in_data_file_names, VIBDatabasePtr in_database, 
	const VRouteSection& in_route_section, bool in_from_db)
{		
	//Очищаем график
	ClearLayers();
	ClearAreas();	
	data_set.clear();

	//Создаем слои		
	CreateLayers();
	
	//Создаем обязательные объекты	
	page_info = VPageInfoPtr(new VPageInfo());	
	selection_info = VSelectionInfoPtr(new VSelectionInfo());

	selection_info->SetActive(false);	
	VActivateActionPtr activate_selection(new VActivateAction(selection_info, L""));
	VDeactivateActionPtr deactivate_selection(new VDeactivateAction(selection_info, L""));
	activate_selection->SetHotKey(HK_SHIFT_DOWN);
	Subscribe(EVENT_KEY_DOWN, activate_selection);
	deactivate_selection->SetHotKey(HK_SHIFT_UP);
	Subscribe(EVENT_KEY_UP, deactivate_selection);

	//Создаем обязательные области
	header_area=VAreaPtr(new VArea());
	footer_area=VAreaPtr(new VArea());			

	//Создаем обязательные элементы
	GetLayer(std::wstring(L"Background"))->AddElement(VElementPtr(new VBackgroundElement(VColor(255,255,255,255))));
	GetLayer(std::wstring(L"PageNumbers"))->AddElement(VElementPtr(new VPageNumberElement(footer_area, device, page_info)));	
	
		
	//Загрузка данных
	database=in_database;
	LoadData(in_data_file_names, database, in_route_section, in_from_db);				
	
	//Создаем объект - сетка путевых координат
	coordinates = VCoordinatesDataPtr(new VCoordinatesData(*aprinfo, pro::interval<double>(0,DBL_MAX)));

	//Создаем обязательные действия
	VActionPtr inc_font_size=VActionPtr(new VChangeFontSize(font_size_step, L"Увеличить размер шрифта"));
	VActionPtr dec_font_size=VActionPtr(new VChangeFontSize(-font_size_step, L"Уменьшить размер шрифта"));	

	inc_font_size->SetHotKey(HK_CTRL_PLUS);
	dec_font_size->SetHotKey(HK_CTRL_MINUS);
	inc_font_size->SetHotKey(HK_CTRL_NUM_PLUS);
	dec_font_size->SetHotKey(HK_CTRL_NUM_MINUS);
	Subscribe(EVENT_KEY_DOWN, inc_font_size);
	Subscribe(EVENT_KEY_DOWN, dec_font_size);				

	//Копирование в новый файл	
	for (size_t i=0; i<data_set.size(); ++i)
	{
		std::wstring action_name=boost::str(boost::wformat(L"Копировать параметры в новый файл [%s]") % data_set.at(i)->GetFileShortName());
		VCopyParametersActionPtr copy_parameters(
			new VCopyParametersAction(data_set.at(i), selection_info, save_file_dialog, action_name));
		AddAction(copy_parameters);
	}
	
	//Сохранить изменения 	
	VActionPtr save_changes = VActionPtr(new VSaveChangesAction(data_set, L"Сохранить изменения"));
	AddToolbarAction(save_changes, &(save_enabled_xpm[0]), &(save_disabled_xpm[0]));
	//Показать/скрыть визир
	VActionPtr show_hide_cross = VActionPtr(new VSetLayerVisible(GetLayer(std::wstring(L"Dynamic")), L"Показать/скрыть визир"));	
	AddToolbarAction(show_hide_cross, &(locator_enabled_xpm[0]), &(locator_disabled_xpm[0]));		

	//Цикл по областям					
	for (size_t i=0; i<GetGraphics().GetAreaCount(); ++i)
	{								
		//Очищаем список выполненных запросов
		sql_set.clear();
		constant_set.clear();

		//Создаем область
		VXMLArea xml_area = GetGraphics().GetArea(i);					
		VAreaPtr area=CreateArea(xml_area);		
		
		//Создаем систему координат	
		VXMLAxes xml_axes=xml_area.GetAxes();																								
		VAxesPtr axes=CreateAxes(xml_axes, area);

		double min_y=DBL_MAX;
		double max_y=(0-DBL_MAX);

		//Создаем легенду
		VLegendElement * area_legend=new VLegendElement(axes);
		GetLayer(std::wstring(L"Legend"))->AddElement(VElementPtr(area_legend));

		//Создаем элемент отображения легенды оси Y
		if (xml_axes.GetShowLegendY())
		{
			area_legend->AddGraph(xml_axes.GetLegendY(),VColor(0,0,0));			
		}
							
		//Добавляем сетку и оцифровку
		if (xml_axes.GetShowXLines())
		{				
			GetLayer(std::wstring(L"GridLines"))->AddElement(VElementPtr(new VCoordinatesGridElement(coordinates, axes)));
		}
		if (xml_axes.GetShowXCoord())
		{
			GetLayer(std::wstring(L"GridLines"))->AddElement(VElementPtr(new VCoordinatesTextElement(coordinates, axes)));
			GetLayer(std::wstring(L"GridLines"))->AddElement(VElementPtr(new VPreviousKmElement(axes, coordinates)));
		}
		if (xml_axes.GetShowYLines())	
		{			
			GetLayer(std::wstring(L"GridLines"))->AddElement(
			VElementPtr(new VGridLinesYElement(axes, xml_axes.GetShowYCoord())));
		}
		//Линейка масштаба
		if (xml_axes.GetShowXScale() || xml_axes.GetShowYScale())
			GetLayer(std::wstring(L"Legend"))->AddElement(
			VElementPtr(new VScaleElement(axes, xml_axes.GetShowXScale(), xml_axes.GetShowYScale())));
					
		//Локатор
		VMousePositionDataPtr mouse_position(new VMousePositionData());
		Subscribe(EVENT_MOUSE_MOTION, mouse_position);
		VMouseCrossElementPtr mouse_cross=VMouseCrossElementPtr(new VMouseCrossElement(axes, mouse_position));
		GetLayer(std::wstring(L"Dynamic"))->AddElement(mouse_cross);
		VTrackValueElementPtr track_value=VTrackValueElementPtr(new VTrackValueElement(axes, mouse_position));				
		GetLayer(std::wstring(L"Dynamic"))->AddElement(track_value);					
		if (xml_axes.GetShowXCoord())
		{
			VTrackCoordElementPtr track_coord=VTrackCoordElementPtr(new VTrackCoordElement(axes, 
				mouse_position,	aprinfo));
			GetLayer(std::wstring(L"Dynamic"))->AddElement(track_coord);	
			Subscribe(EVENT_MOUSE_MOTION, track_coord);
		}
					
		//Глобальная область выделения
		VElementPtr selection_element(new VSelectionElement(axes, selection_info));
		GetLayer(std::wstring(L"Selecting"))->AddElement(selection_element);	
		Subscribe(EVENT_MOUSE_LEFT_DOWN, selection_element);
		Subscribe(EVENT_MOUSE_LEFT_UP, selection_element);
		Subscribe(EVENT_MOUSE_MOTION, selection_element);

		//Статические
		for (size_t k=0; k<xml_axes.GetStaticGraphCount(); ++k)
		{													
			VXMLStaticGraph xml_graph = xml_axes.GetStaticGraph(k);			
			//Создаем источники данных				
			std::vector<VDataProviderPtr> data_providers=CreateDataProvider(axes, xml_graph);									
			for (size_t n=0; n<data_providers.size(); ++n)
			{	
				//Добавляем источники данных в систему координат
				axes->AddDataProvider(data_providers.at(n));
				//Устанавливаем ограничения по X
				data_providers.at(n)->SetRangeX(pro::interval<double>(GetHorzMin()+ABSCOORD_SPACE/2, GetHorzMax()-ABSCOORD_SPACE/2));

				//В зависимости от типа графика создаем тот или иной элемент											
				VElementPtr graph_ptr=CreateGraph(data_providers.at(n)->GetLegend(), data_providers.at(n), axes, xml_graph);
				GetLayer(std::wstring(L"Graph"))->AddElement(graph_ptr);
				//Добавляем график в легенду				
				if (xml_graph.GetShowLegend())
				{					
					area_legend->AddGraph(data_providers.at(n)->GetLegend(), xml_graph.GetColors().GetColor());
				}
				//Устанавливаем опцию "отслеживать положение мыши"
				if (xml_graph.GetTrackMouse())
				{
					track_value->AddGraph(data_providers.at(n), xml_graph.GetColors().GetColor());
				}
				//Выбираем следующий цвет из списка
				xml_graph.GetColors().NextColor();
				xml_graph.GetFillColors().NextColor();				
			}					
		}			
		//Инструменты
		for (size_t k=0; k<xml_axes.GetToolCount(); ++k)
		{																					
			VXMLTool xml_graph = xml_axes.GetTool(k);
			//Создаем графики только для активного файла
			if (GetActiveDataSet())
			{
				//Парсим команду
				VCommandParser command_parser(xml_graph.GetDataType());							
				//Параметры кривых FixMe!!! Это не инструмент, но в обычный график тоже не вписывается.
				if (command_parser.GetCommand()==L"CURVES")
				{
					VCurvesInfoPtr curves = VCurvesInfoPtr(new VCurvesInfo(GetActiveDataSet()));
					VCurvesElementPtr curves_element=VCurvesElementPtr(new VCurvesElement(curves, axes));
					GetLayer(std::wstring(L"Graph"))->AddElement(curves_element);

					//Параметры прямых FixMe!!! в паре с кривыми
					VLinesDataPtr lines = VLinesDataPtr(new VLinesData(GetActiveDataSet()));
					VElementPtr lines_element=VElementPtr(new VLinesElement(lines, axes));
					GetLayer(std::wstring(L"Graph"))->AddElement(lines_element);
				}
				//Параметры кривых FixMe!!! Это не инструмент, но в обычный график тоже не вписывается.
				else if (command_parser.GetCommand()==L"CURVES_INFO")
				{
					VCurvesInfoPtr curves = VCurvesInfoPtr(new VCurvesInfo(GetActiveDataSet()));
					VCurvesInfoElementPtr curves_info_element=VCurvesInfoElementPtr(new VCurvesInfoElement(curves, axes));
					GetLayer(std::wstring(L"Graph"))->AddElement(curves_info_element);						
				}
				//Паспортизация кривой
				else if (command_parser.GetCommand()==L"CURVES_PASSPORT")
				{
					VCurvesInfoPtr curves = VCurvesInfoPtr(new VCurvesInfo(GetActiveDataSet(), true));
					VCurvesPassportElementPtr curves_info_element=VCurvesPassportElementPtr(new VCurvesPassportElement(curves, axes));
					GetLayer(std::wstring(L"Graph"))->AddElement(curves_info_element);

					//Создаем действия
					//Присвоить кривой статус						
					VActionPtr set_status_action = VActionPtr(new VSetCurveStatusAction(							
						GetActiveDataSet(), L"Присвоить статус", select_dialog));
					RegisterAction(set_status_action, axes);
					
					//Присвоить кривой комментарий						
					VActionPtr set_comment_action = VActionPtr(new VSetCurveCommentAction(							
						GetActiveDataSet(), L"Комментарий", select_dialog, input_dialog));
					RegisterAction(set_comment_action, axes);

					//Установить скорости на кривой
					VActionPtr set_speeds_action = VActionPtr(new VSetCurveSpeedsAction(							
						GetActiveDataSet(), L"Установить скорости", property_dialog));
					RegisterAction(set_speeds_action, axes);

					//Установить скорости по приказу
					VActionPtr reset_speeds_action = VActionPtr(new VResetCurveSpeedsAction(							
						GetActiveDataSet(), L"Установить скорости по приказу"));
					RegisterAction(reset_speeds_action, axes);
				}
				//Паспортизация кривой (уровень)
				else if (command_parser.GetCommand()==L"LEVEL_PASSPORT")
				{
					VCurvesInfoPtr curves = VCurvesInfoPtr(new VCurvesInfo(GetActiveDataSet(), true));
					VLevelPassportElementPtr level_info_element=VLevelPassportElementPtr(new VLevelPassportElement(curves, axes));
					GetLayer(std::wstring(L"Graph"))->AddElement(level_info_element);						

					//Рассчитать возвышение
					VActionPtr create_level_optimal_action = VActionPtr(new VCreateLevelOptimalAction(
						GetActiveDataSet(), L"Рассчитать возвышение: оптимальное"));
					RegisterAction(create_level_optimal_action, axes);

					//Рассчитать возвышение
					VActionPtr create_level_rational_action = VActionPtr(new VCreateLevelRationalAction(
						GetActiveDataSet(), L"Рассчитать возвышение: рациональное"));
					RegisterAction(create_level_rational_action, axes);

					//Рассчитать возвышение
					VActionPtr create_level_fact_action = VActionPtr(new VCreateLevelFactAction(
						GetActiveDataSet(), L"Рассчитать возвышение: близкое к факту"));
					RegisterAction(create_level_fact_action, axes);
					
					//Рассчитать возвышение
					VActionPtr create_level_optimal_all_curves = VActionPtr(new VCreateLevelOptimalAllCurves(
						GetActiveDataSet(), L"Рассчитать возвышение для всех кривых: оптимальное"));
					RegisterAction(create_level_optimal_all_curves, axes);

					//Рассчитать возвышение
					VActionPtr create_level_rational_all_curves = VActionPtr(new VCreateLevelRationalAllCurves(
						GetActiveDataSet(), L"Рассчитать возвышение для всех кривых: рациональное"));
					RegisterAction(create_level_rational_all_curves, axes);

					//Рассчитать возвышение
					VActionPtr create_level_fact_all_curves = VActionPtr(new VCreateLevelFactAllCurves(
						GetActiveDataSet(), L"Рассчитать возвышение для всех кривых: близкое к факту"));
					RegisterAction(create_level_fact_all_curves, axes);
				}
				//Отбраковка параметра
				else if (command_parser.GetCommand()==L"CORRUPT")
				{							
					if (command_parser.GetParameterCount()!=1 && command_parser.GetParameterCount()!=3)
						throw VSimpleException(L"Неверные параметры функции", xml_graph.GetDataType(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
					PARAMETER_TYPE data_type=GetParamCode(command_parser.GetParameter(0).c_str());

										
					//Создаем действия
					//Отбраковать параметр
					std::wstring action_name=command_parser.GetParameterCount()==3 ? 
						boost::str(boost::wformat(L"Отбраковать параметр '%s' [%s]") % GetUserParamName(data_type) % VHotKey(command_parser.GetParameter(1).c_str()).GetDescription()) :
						boost::str(boost::wformat(L"Отбраковать параметр '%s'") % GetUserParamName(data_type));
					VActionPtr corrupt_action = VActionPtr(new VCorruptParameterAction(							
						selection_info, GetActiveDataSet(), true, data_type, action_name));
					RegisterAction(corrupt_action, axes);
					//Восстановить параметр
					action_name=command_parser.GetParameterCount()==3 ? 
						boost::str(boost::wformat(L"Восстановить параметр '%s' [%s]") % GetUserParamName(data_type) % VHotKey(command_parser.GetParameter(2).c_str()).GetDescription()) :
						boost::str(boost::wformat(L"Восстановить параметр '%s'") % GetUserParamName(data_type));
					VActionPtr restore_action = VActionPtr(new VCorruptParameterAction(
						selection_info, GetActiveDataSet(), false, data_type, action_name));
					RegisterAction(restore_action, axes);						
					//Восстановить отбраковку
					VActionPtr reload_action = VActionPtr(new VReloadParameterAction(
						GetActiveDataSet(), PRM_CORRUPT, L"Восстановить отбраковку"));
					RegisterAction(reload_action, axes);

					if (command_parser.GetParameterCount()==3)
					{
						corrupt_action->SetHotKey(VHotKey(command_parser.GetParameter(1).c_str()));
						Subscribe(EVENT_KEY_DOWN, corrupt_action);
						restore_action->SetHotKey(VHotKey(command_parser.GetParameter(2).c_str()));
						Subscribe(EVENT_KEY_DOWN, restore_action);
					}

				}
				//Автокоррекция координат
				else if (command_parser.GetCommand()==L"COORD_CORRECT")
				{
					VColor color_km(0,0,255,255);
					VColor fill_color(0,255,0,255);

					VAutoScaleActionPtr auto_scale(new VAutoScaleAction(axes, L"Автоматический масштаб"));
					VGroupActionPtr select_point_and_auto_scale_action = VGroupActionPtr(new VGroupAction(L"Браковать/Снять отбраковку"));
					if(GetActiveDataSet()->GetCoordPoint().Load(*GetActiveDataSet(),POINT_POLE,true))
					{
						VDataProviderPtr data_provider_km = VDataProviderPtr(new VCoordPointData(GetActiveDataSet(), in_database, POINT_POLE));
						VDataProviderPtr data_provider_line_km = VDataProviderPtr(new VLine_tData(GetActiveDataSet(), POINT_POLE));
						//Создаем графики
						VCoordPointGraph * km_graph = new VCoordPointGraph(data_provider_km, axes, color_km, fill_color, xml_graph.GetWidth(), xml_graph.GetClip());
						VLineGraph * km_line_graph = new VLineGraph(data_provider_line_km, axes, color_km, fill_color, default_line_width);
						VElementPtr km_graph_ptr=VElementPtr(km_graph);
						VElementPtr km_line_graph_ptr=VElementPtr(km_line_graph);
						Subscribe(EVENT_MOUSE_MOTION, km_graph_ptr);
						GetLayer(std::wstring(L"Graph"))->AddElement(km_line_graph_ptr);
						GetLayer(std::wstring(L"Graph"))->AddElement(km_graph_ptr);
						area_legend->AddGraph(L"Км. столбы", color_km);							
						axes->AddDataProvider(data_provider_km);
						axes->AddDataProvider(data_provider_line_km);

						//Автокоррекция по км. столбам
						VActionPtr corr_km_action = VActionPtr(new VCorrectCoordAction(GetActiveDataSet(), in_database, POINT_POLE));
						VGroupActionPtr corr_km_and_auto_scale_action = VGroupActionPtr(new VGroupAction(L"Автокоррекция по км столбам"));
						corr_km_and_auto_scale_action->AddAction(corr_km_action);
						corr_km_and_auto_scale_action->AddAction(auto_scale);
						AddToolbarAction(corr_km_and_auto_scale_action, &(lightning_yellow_enabled_xpm[0]), &(lightning_yellow_disabled_xpm[0]));

						//Активация участка
						VActivateActionPtr activate_km(new VActivateAction(km_graph_ptr, L""));						
						activate_km->SetHotKey(HK_SHIFT_UP);						
						Subscribe(EVENT_KEY_UP, activate_km);
						//Деактивация участка
						VDeactivateActionPtr deactivate_km(new VDeactivateAction(km_graph_ptr, L""));
						deactivate_km->SetHotKey(HK_SHIFT_DOWN);
						Subscribe(EVENT_KEY_DOWN, deactivate_km);

						VActionPtr select_point_action_km = VActionPtr(new VSelectPointAction(
							data_provider_km));
						select_point_and_auto_scale_action->AddAction(select_point_action_km);						
					}

					select_point_and_auto_scale_action->AddAction(auto_scale);
					RegisterAction(select_point_and_auto_scale_action, axes);
				}				
				//Корреляция параметра
				else if ((command_parser.GetCommand()==L"CORRELATION"))
				{
					if (command_parser.GetParameterCount()!=1)
						throw VSimpleException(L"Неверные параметры функции", xml_graph.GetDataType(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
					PARAMETER_TYPE data_type=GetParamCode(command_parser.GetParameter(0).c_str());

					if (data_set.size()>=2)
					{
						VCorrelationInfoPtr corr(new VCorrelationInfo(GetActiveDataSet(), GetReferenceDataSet(), data_type));
						GetLayer(std::wstring(L"Graph"))->AddElement(VTextElementPtr(new VTextElement(axes->GetArea(), device, corr)));
					}
					else
					{
						throw VSimpleException(L"Для расчета коэффициента корреляции требуется два проезда", L"");
					}

				}
				//Коррекция нуля параметра под другому параметру
				else if ((command_parser.GetCommand()==L"ALIGN_PARAMETER_MEAN"))
				{
					if (command_parser.GetParameterCount()!=2)
						throw VSimpleException(L"Неверные параметры функции", xml_graph.GetDataType(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
					PARAMETER_TYPE data_type_source=GetParamCode(command_parser.GetParameter(0).c_str());
					PARAMETER_TYPE data_type_target=GetParamCode(command_parser.GetParameter(1).c_str());
					
					std::wstring action_name=boost::str(boost::wformat(L"Корректировать параметр '%s' по параметру '%s'") % 
							GetUserParamName(data_type_target) % GetUserParamName(data_type_source));

					VAlignParameterMeanPtr correct_parameter(new VAlignParameterMean(selection_info, 
						GetActiveDataSet(), data_type_source, data_type_target, action_name));
					RegisterAction(correct_parameter, axes);
				}
				//Коррекция нуля параметра
				else if ((command_parser.GetCommand()==L"SHIFT_PARAMETER"))
				{
					if (command_parser.GetParameterCount()!=2)
						throw VSimpleException(L"Неверные параметры функции", xml_graph.GetDataType(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
					PARAMETER_TYPE data_type=GetParamCode(command_parser.GetParameter(0).c_str());
					double value=boost::lexical_cast<float>(command_parser.GetParameter(1));

					VShiftParameterActionPtr shift_up(new VShiftParameterAction(axes, GetActiveDataSet(), 
						data_type, value, L"Сдвинуть вверх"));
					shift_up->SetHotKey(HK_CTRL_UP_ARROW);
					Subscribe(EVENT_KEY_DOWN, shift_up);

					VShiftParameterActionPtr shift_down(new VShiftParameterAction(axes, GetActiveDataSet(), 
						data_type, -value, L"Сдвинуть вниз"));
					shift_down->SetHotKey(HK_CTRL_DOWN_ARROW);
					Subscribe(EVENT_KEY_DOWN, shift_down);
				}
				//Ручной сдвиг файла
				else if ((command_parser.GetCommand()==L"SHIFT_COORD"))
				{	
					if (command_parser.GetParameterCount()!=1)
						throw VSimpleException(L"Неверные параметры функции", xml_graph.GetDataType(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
					double value=boost::lexical_cast<float>(command_parser.GetParameter(0));

					VShiftCoordinatesActionPtr shift(new VShiftCoordinatesAction(GetActiveDataSet(), 0, L"Сдвинуть ...", input_dialog));
					RegisterAction(shift, axes);

					VShiftCoordinatesActionPtr shift_left(new VShiftCoordinatesAction(GetActiveDataSet(), -value, L"Сдвинуть влево"));
					shift_left->SetHotKey(HK_CTRL_LEFT_ARROW);
					Subscribe(EVENT_KEY_DOWN, shift_left);
					AddToolbarAction(shift_left, &(shift_left_enabled_xpm[0]), &(shift_left_disabled_xpm[0]));

					VShiftCoordinatesActionPtr shift_right(new VShiftCoordinatesAction(GetActiveDataSet(), value, L"Сдвинуть вправо"));
					shift_right->SetHotKey(HK_CTRL_RIGHT_ARROW);
					Subscribe(EVENT_KEY_DOWN, shift_right);
					AddToolbarAction(shift_right, &(shift_right_enabled_xpm[0]), &(shift_right_disabled_xpm[0]));
				}
				//Ручное сжатие/растяжение файла
				else if ((command_parser.GetCommand()==L"SCALE_COORD"))
				{
					if (command_parser.GetParameterCount()!=1)
						throw VSimpleException(L"Неверные параметры функции", xml_graph.GetDataType(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
					double value=boost::lexical_cast<float>(command_parser.GetParameter(0));

					VScaleCoordinatesActionPtr shrink(new VScaleCoordinatesAction(GetActiveDataSet(), -value, L"Сжать (Alt +)"));
					shrink->SetHotKey(HK_ALT_PLUS);
					Subscribe(EVENT_KEY_DOWN, shrink);
					AddToolbarAction(shrink, &(shrink_enabled_xpm[0]), &(shrink_disabled_xpm[0]));

					VScaleCoordinatesActionPtr stretсh(new VScaleCoordinatesAction(GetActiveDataSet(), value, L"Растянуть (Alt -)"));
					stretсh->SetHotKey(HK_ALT_MINUS);
					Subscribe(EVENT_KEY_DOWN, stretсh);
					AddToolbarAction(stretсh, &(stretch_enabled_xpm[0]), &(stretch_disabled_xpm[0]));
				}
				//Фазировка левых магнитных зазоров
				else if (command_parser.GetCommand()==L"LEFT_MAG_ZAZOR")
				{
					VColor color(0,0,0,255);
					VColor fill_color(0,255,0,255);
					//Создаем источник данных
					VDataProviderPtr data_provider = VDataProviderPtr(new VMagZazorLeftData(GetActiveDataSet()));
					//Создаем график
					VStickGraph * new_graph=new VStickGraph(data_provider, axes, color, fill_color, default_line_width);
					VElementPtr graph_ptr=VElementPtr(new_graph);
					Subscribe(EVENT_MOUSE_MOTION, graph_ptr);				
					GetLayer(std::wstring(L"Graph"))->AddElement(graph_ptr);
					area_legend->AddGraph(L"Зазор лев. магн.", color);
					//Добавляем действия							
					axes->AddDataProvider(data_provider);
					VActionPtr shift_zazor = VActionPtr(new VShiftZazorAction(GetActiveDataSet(), L"Сдвиг МЗ", property_dialog));
					RegisterAction(shift_zazor, axes);

					//Активация участка
					VActivateActionPtr activate(new VActivateAction(graph_ptr, L""));						
					activate->SetHotKey(HK_SHIFT_UP);						
					Subscribe(EVENT_KEY_UP, activate);
					//Деактивация участка
					VDeactivateActionPtr deactivate(new VDeactivateAction(graph_ptr, L""));
					deactivate->SetHotKey(HK_SHIFT_DOWN);
					Subscribe(EVENT_KEY_DOWN, deactivate);
					max_y=std::max<double>(max_y, data_provider->GetMaxY()+15);
				}

				//Фазировка правых магнитных зазоров
				else if (command_parser.GetCommand()==L"RIGHT_MAG_ZAZOR")
				{
					VColor color(0,0,0,255);
					VColor fill_color(0,255,0,255);
					//Создаем источник данных
					VDataProviderPtr data_provider = VDataProviderPtr(new VMagZazorRightData(GetActiveDataSet()));
					//Создаем график
					VStickGraph * new_graph=new VStickGraph(data_provider, axes, color, fill_color, default_line_width);
					VElementPtr graph_ptr=VElementPtr(new_graph);
					Subscribe(EVENT_MOUSE_MOTION, graph_ptr);				
					GetLayer(std::wstring(L"Graph"))->AddElement(graph_ptr);
					area_legend->AddGraph(L"Зазор прав. магн.", color);
					//Добавляем действия							
					axes->AddDataProvider(data_provider);
					//Активация разметки
					VActivateActionPtr activate(new VActivateAction(graph_ptr, L""));						
					activate->SetHotKey(HK_SHIFT_UP);						
					Subscribe(EVENT_KEY_UP, activate);

					//Деактивация разметки
					VDeactivateActionPtr deactivate(new VDeactivateAction(graph_ptr, L""));
					deactivate->SetHotKey(HK_SHIFT_DOWN);
					Subscribe(EVENT_KEY_DOWN, deactivate);

					VActionPtr shift_zazor = VActionPtr(new VShiftZazorAction(GetActiveDataSet(), L"Сдвиг МЗ", property_dialog));
					RegisterAction(shift_zazor, axes);
					max_y=std::max<double>(max_y, data_provider->GetMaxY()+15);
				}
				//Разметка кривых по кривизне
				else if (command_parser.GetCommand()==L"CURVE_MARKS")
				{

					if (command_parser.GetParameterCount()!=1)
						throw VSimpleException(L"Неверные параметры функции", xml_graph.GetDataType().c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__); 		

					PARAMETER_TYPE mark_type=GetParamCode(command_parser.GetParameter(0).c_str());

					VColor color(0,0,255,255);
					VColor fill_color(0,255,0,255);

					//Создаем источник данных
					VDataProviderPtr data_provider = VDataProviderPtr(new VCurveMarksData(GetActiveDataSet(), PRM_CURV, mark_type));

					//Создаем график
					VCurveMarksGraph * new_graph=new VCurveMarksGraph(data_provider, axes, color, fill_color, CLIP_AREA);
					VElementPtr graph_ptr=VElementPtr(new_graph);
					Subscribe(EVENT_MOUSE_LEFT_DOWN, graph_ptr);
					Subscribe(EVENT_MOUSE_LEFT_UP, graph_ptr);
					Subscribe(EVENT_MOUSE_MOTION, graph_ptr);				
					GetLayer(std::wstring(L"Graph"))->AddElement(graph_ptr);						

					//Добавляем действия		

					//Активация разметки
					VActivateActionPtr activate(new VActivateAction(graph_ptr, L""));						
					activate->SetHotKey(HK_SHIFT_UP);						
					Subscribe(EVENT_KEY_UP, activate);

					//Деактивация разметки
					VDeactivateActionPtr deactivate(new VDeactivateAction(graph_ptr, L""));
					deactivate->SetHotKey(HK_SHIFT_DOWN);
					Subscribe(EVENT_KEY_DOWN, deactivate);
											
					//Удаление всех кривых
					VActionPtr clear_all_action = VActionPtr(new VDeleteAllCurvesAction(
						GetActiveDataSet(), L"Удалить все кривые", mark_type));												
					AddToolbarAction(clear_all_action, &(cross_enabled_xpm[0]), &(cross_disabled_xpm[0]));

					//Автоматическая разметка кривых
					VActionPtr automark_action = VActionPtr(new VAutomarkCurvesAction(
						GetActiveDataSet(), progress_dialog, L"Автоматическая разметка", mark_type));												
					AddToolbarAction(automark_action, &(lightning_yellow_enabled_xpm[0]), &(lightning_yellow_disabled_xpm[0]));

					//Разметка по паспорту
					VActionPtr curves_from_db_action = VActionPtr(new VTransferPassportCurvesAction(GetActiveDataSet(), 
						database, progress_dialog, L"Разметка по паспорту", mark_type));
					AddToolbarAction(curves_from_db_action, &(lightning_red_enabled_xpm[0]), &(lightning_red_disabled_xpm[0]));

					//Разметка по предыдущим проездам
					VActionPtr curves_from_db_action2 = VActionPtr(new VTransferMeasuredCurvesAction(GetActiveDataSet(), 
						database, progress_dialog, L"Разметка по предыдущим проездам", mark_type));
					AddToolbarAction(curves_from_db_action2, &(lightning_green_enabled_xpm[0]), &(lightning_green_disabled_xpm[0]));

					//Копирование кривых из другой разметки
					VActionPtr copy_curves_action = VActionPtr(new VCopyCurvesAction(GetActiveDataSet(), 
						mark_type==PRM_CURVE_MARKS ? L"Копировать кривые из паспортной разметки" : 
						L"Копировать кривые из фактической разметки", mark_type));
					RegisterAction(copy_curves_action, axes);

					//Оптимизация кривых
					VActionPtr optimize_action = VActionPtr(new VOptimizeCurvesAction(
						GetActiveDataSet(), progress_dialog, L"Оптимизация кривых", mark_type));																	
					AddToolbarAction(optimize_action, &(lightning_blue_enabled_xpm[0]), &(lightning_blue_disabled_xpm[0]));
					
					//Удаление метки
					VActionPtr delete_mark_action = VActionPtr(new VDeleteDataItemAction(
						data_provider, L"Удалить метку"));
					RegisterAction(delete_mark_action, axes);		
					
					//Удаление активной кривой
					VActionPtr delete_curve_action = VActionPtr(new VDeleteActiveCurveAction(
						GetActiveDataSet(), L"Удалить кривую (Delete)", mark_type));						
					AddAction(delete_curve_action);
					delete_curve_action->SetHotKey(HK_DELETE);
					Subscribe(EVENT_KEY_DOWN, delete_curve_action);

					//Занесение кривой в список
					VActionPtr apply_active_curve = VActionPtr(new VApplyActiveCurve(
						GetActiveDataSet(), L"Применить разметку активной кривой (Space)", mark_type));
					apply_active_curve->SetHotKey(HK_SPACE);
					Subscribe(EVENT_KEY_DOWN, apply_active_curve);																		

					//Переход к следующей кривой
					VActionPtr activate_next_curve = VActionPtr(new VActivateNextCurve(
						GetActiveDataSet(), this, L"Следующая кривая (Tab)", mark_type));
					activate_next_curve->SetHotKey(HK_TAB);
					Subscribe(EVENT_KEY_DOWN, activate_next_curve);
					AddAction(activate_next_curve);
					
					//Нормализация кривой
					VActionPtr normalize_active_curve = VActionPtr(new VNormalizeCurve(
						GetActiveDataSet(), L"Нормализовать кривую (Ctrl+Q)", mark_type));						
					AddAction(normalize_active_curve);
					normalize_active_curve->SetHotKey(HK_CTRL_Q);																		
					Subscribe(EVENT_KEY_DOWN, normalize_active_curve);					

					//Оптимизация кривой
					VActionPtr optimize_active_curve = VActionPtr(new VOptimizeCurve(
						GetActiveDataSet(), L"Оптимизировать кривую (Ctrl+W)", mark_type));						
					AddAction(optimize_active_curve);
					optimize_active_curve->SetHotKey(HK_CTRL_W);																		
					Subscribe(EVENT_KEY_DOWN, optimize_active_curve);						

					//Восстановление разметки
					VActionPtr load_action = VActionPtr(new VReloadParameterAction(
						GetActiveDataSet(), mark_type, L"Восстановить разметку кривых"));															
					AddAction(load_action);
				}
				//Разметка кривых по уровню
				else if (command_parser.GetCommand()==L"LEVEL_MARKS")
				{
					if (command_parser.GetParameterCount()!=1)
						throw VSimpleException(L"Неверные параметры функции", xml_graph.GetDataType().c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__); 		

					PARAMETER_TYPE data_type=GetParamCode(command_parser.GetParameter(0).c_str());

					VColor color(0,0,255,255);
					VColor fill_color(0,255,0,255);

					//Создаем источник данных
					VDataProviderPtr data_provider = VDataProviderPtr(new VCurveMarksData(GetActiveDataSet(), PRM_LEVEL, data_type));		

					//Создаем график
					VCurveMarksGraph * new_graph=new VCurveMarksGraph(data_provider, axes, color, fill_color, CLIP_AREA);
					VElementPtr graph_ptr=VElementPtr(new_graph);
					Subscribe(EVENT_MOUSE_LEFT_DOWN, graph_ptr);
					Subscribe(EVENT_MOUSE_LEFT_UP, graph_ptr);
					Subscribe(EVENT_MOUSE_MOTION, graph_ptr);
					GetLayer(std::wstring(L"Graph"))->AddElement(graph_ptr);

					//Активация разметки
					VActivateActionPtr activate(new VActivateAction(graph_ptr, L""));						
					activate->SetHotKey(HK_SHIFT_UP);						
					Subscribe(EVENT_KEY_UP, activate);

					//Деактивация разметки
					VDeactivateActionPtr deactivate(new VDeactivateAction(graph_ptr, L""));
					deactivate->SetHotKey(HK_SHIFT_DOWN);
					Subscribe(EVENT_KEY_DOWN, deactivate);

					//Координаты на уровне
					VFixLevelCoordinatesPtr fix_level_coord(new VFixLevelCoordinates(GetActiveDataSet(), 
						L"Приравнять координаты меток на уровне к меткам на кривизне (Ctrl+V)", data_type));
					RegisterAction(fix_level_coord, axes);
					fix_level_coord->SetHotKey(HK_CTRL_V);
					Subscribe(EVENT_KEY_DOWN, fix_level_coord);
					
					//Увеличить уровень
					VIncreaseLevelActionPtr increase_level(new VIncreaseLevelAction(GetActiveDataSet(), 
						L"Увеличить уровень (Ctrl+Up)", data_type));
					RegisterAction(increase_level, axes);
					increase_level->SetHotKey(HK_CTRL_UP_ARROW);
					Subscribe(EVENT_KEY_DOWN, increase_level);
					
					//Уменьшить уровень
					VDecreaseLevelActionPtr decrease_level(new VDecreaseLevelAction(GetActiveDataSet(), 
						L"Уменьшить уровень (Ctrl+Down)", data_type));
					RegisterAction(decrease_level, axes);
					decrease_level->SetHotKey(HK_CTRL_DOWN_ARROW);
					Subscribe(EVENT_KEY_DOWN, decrease_level);

					//Удаление метки
					VActionPtr delete_mark_action = VActionPtr(new VDeleteDataItemAction(
						data_provider, L"Удалить метку"));
					RegisterAction(delete_mark_action, axes);
				}
				//Разметка прямых
				else if (command_parser.GetCommand()==L"LINE_MARKS")
				{
					if (command_parser.GetParameterCount()!=1)
						throw VSimpleException(L"Неверные параметры функции", xml_graph.GetDataType().c_str(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__); 		

					PARAMETER_TYPE mark_type=GetParamCode(command_parser.GetParameter(0).c_str());
				
					//Создаем источники данных
					VDataProviderPtr data_provider = VDataProviderPtr(new VLineMarkData(GetActiveDataSet()));

					VDataProviderPtr data_provider2 = VDataProviderPtr(new VLineAngleData(GetActiveDataSet()));		

					//Создаем график
					VColor color(0,0,255,255);
					VColor fill_color(0,255,0,255);
					VPointGraph * new_graph=new VPointGraph(data_provider, axes, color, fill_color, 
						CLIP_AREA, false, POINT_STYLE_FLAG);		
					VElementPtr graph_ptr=VElementPtr(new_graph);
					GetLayer(std::wstring(L"Graph"))->AddElement(graph_ptr);
					Subscribe(EVENT_MOUSE_LEFT_DOWN, graph_ptr);
					Subscribe(EVENT_MOUSE_LEFT_UP, graph_ptr);
					Subscribe(EVENT_MOUSE_MOTION, graph_ptr);	

					GetLayer(std::wstring(L"Graph"))->AddElement(VElementPtr(new VTextGraph(data_provider2, axes, TO_VERTICAL, TP_AREA_BOTTOM_OVER)));

					//Активация разметки
					VActivateActionPtr activate(new VActivateAction(graph_ptr, L""));						
					activate->SetHotKey(HK_SHIFT_UP);						
					Subscribe(EVENT_KEY_UP, activate);

					//Деактивация разметки
					VDeactivateActionPtr deactivate(new VDeactivateAction(graph_ptr, L""));
					deactivate->SetHotKey(HK_SHIFT_DOWN);
					Subscribe(EVENT_KEY_DOWN, deactivate);

					//Удаление метки
					VActionPtr delete_mark_action = VActionPtr(new VDeleteDataItemAction(
						data_provider, L"Удалить метку"));
					RegisterAction(delete_mark_action, axes);
					
					//Удаление всех прямых					
					VActionPtr clear_all_action = VActionPtr(new VDeleteAllLinesAction(
						GetActiveDataSet(), L"Удалить разметку прямых", mark_type));												
					RegisterAction(clear_all_action, axes);

				}
				//Разметка боковых путей
				else if (command_parser.GetCommand()==L"SIDEWAYS")
				{
					//Создаем источник данных
					VSidewayDataPtr sideways = VSidewayDataPtr(new VSidewayData(GetActiveDataSet()));
					//Создаем график
					VActiveWaySectionGraphPtr sideways_element = VActiveWaySectionGraphPtr(new VActiveWaySectionGraph(sideways, axes, VColor(0,0,0,255), VColor(0,0,255,128), default_line_width));
					GetLayer(std::wstring(L"Graph"))->AddElement(sideways_element);
					Subscribe(EVENT_MOUSE_MOTION, sideways_element);
					Subscribe(EVENT_MOUSE_LEFT_DOWN, sideways_element);
					Subscribe(EVENT_MOUSE_LEFT_UP, sideways_element);

					//Условия активации
					VActivateActionPtr activate_sideways(new VActivateAction(sideways, L""));
					VDeactivateActionPtr deactivate_sideways(new VDeactivateAction(sideways, L""));
					activate_sideways->SetHotKey(HK_SHIFT_DOWN);
					Subscribe(EVENT_KEY_DOWN, activate_sideways);
					deactivate_sideways->SetHotKey(HK_SHIFT_UP);
					Subscribe(EVENT_KEY_UP, deactivate_sideways);
												
					//Добавляем действия						

					//Съезд на боковой путь
					VActionPtr selection_to_sideway = VActionPtr(new VSelectionToSideway(sideways, selection_info/*axes_selection_info*/, 
						REASON_SIDEWAY, L"Объявить участок съездом на боковой путь"));
					RegisterAction(selection_to_sideway, axes);						

					//Неполная кривая
					VActionPtr selection_to_halfcurve = VActionPtr(new VSelectionToSideway(sideways, selection_info/*axes_selection_info*/, 
						REASON_HALFCURVE, L"Объявить участок неполной кривой"));
					RegisterAction(selection_to_halfcurve, axes);

					//Удаление бокового пути
					VActionPtr delete_sideway_action = VActionPtr(new VDeleteDataItemAction(
						sideways, L"Удалить участок"));
					RegisterAction(delete_sideway_action, axes);

					//Редактирование бокового пути	
					VActionPtr sideway_to_selection = VActionPtr(new VSidewayToSelection(sideways, selection_info/*axes_selection_info*/, 
						L"Изменить границы участка"));
					RegisterAction(sideway_to_selection, axes);
					Subscribe(EVENT_MOUSE_LEFT_DOWN, sideway_to_selection);						
				}
				//Разметка профиля
				else if (command_parser.GetCommand()==L"PROF_ELEMENTS")
				{	
					VColor color(0,0,255,255);
					VColor fill_color(0,255,0,255);

					//Создаем источник данных
					VDataProviderPtr data_provider = VDataProviderPtr(new VProfileElementsData(GetActiveDataSet()));
					data_provider->SetActive(true);	

					VDataProviderPtr aprox_profile = VDataProviderPtr(new VParameterData(GetActiveDataSet(), PRM_APROX_PROFILE));

					VDataProviderPtr slope_angle = VDataProviderPtr(new VSlopeAngleData(GetActiveDataSet(), PRM_PROFILE_SLOPE_ELEMENT));

					//Создаем график
					VPointLineGraph * new_graph=new VPointLineGraph(data_provider, axes, color, fill_color, 
						default_line_width, CLIP_AREA, false, POINT_STYLE_FLAG);//true, 
					VElementPtr graph_ptr=VElementPtr(new_graph);
					GetLayer(std::wstring(L"Graph"))->AddElement(graph_ptr);
					Subscribe(EVENT_MOUSE_LEFT_DOWN, graph_ptr);
					Subscribe(EVENT_MOUSE_LEFT_UP, graph_ptr);
					Subscribe(EVENT_MOUSE_MOTION, graph_ptr);	

					VLineGraph * aprox_profile_graph=new VLineGraph(aprox_profile, axes, VColor(255,0,0), VColor(255,0,0), default_line_width, CLIP_AREA);
					VElementPtr aprox_profile_graph_ptr=VElementPtr(aprox_profile_graph);
					GetLayer(std::wstring(L"Graph"))->AddElement(aprox_profile_graph_ptr);

					VTextGraph * slope_angle_graph = new VTextGraph(slope_angle, axes, TO_VERTICAL, TP_AREA_BOTTOM_OVER);
					VElementPtr slope_angle_graph_ptr=VElementPtr(slope_angle_graph);
					GetLayer(std::wstring(L"Graph"))->AddElement(slope_angle_graph_ptr);
					
					//Добавляем действия

					//Активация разметки
					VActivateActionPtr activate(new VActivateAction(graph_ptr, L""));						
					activate->SetHotKey(HK_SHIFT_UP);						
					Subscribe(EVENT_KEY_UP, activate);

					//Деактивация разметки
					VDeactivateActionPtr deactivate(new VDeactivateAction(graph_ptr, L""));
					deactivate->SetHotKey(HK_SHIFT_DOWN);
					Subscribe(EVENT_KEY_DOWN, deactivate);
									
					//Автоматическая разметка
					VActionPtr automark_action = VActionPtr(new VAutomarkProfileAction(GetActiveDataSet(), progress_dialog));
					AddToolbarAction(automark_action, &(lightning_yellow_enabled_xpm[0]), &(lightning_yellow_disabled_xpm[0]));

					//Перенос из БД
					VActionPtr profile_from_db_action = VActionPtr(new VProfileFromDBAction(GetActiveDataSet(), 
						database, progress_dialog));
					AddToolbarAction(profile_from_db_action, &(lightning_red_enabled_xpm[0]), &(lightning_red_disabled_xpm[0]));
					
					//Удаление всех меток
					VActionPtr clear_action = VActionPtr(new VDeleteAllAction(
						data_provider, L"Удалить разметку"));								
					AddToolbarAction(clear_action, &(cross_enabled_xpm[0]), &(cross_disabled_xpm[0]));

					//Удаление метки
					VActionPtr delete_mark_action = VActionPtr(new VDeleteDataItemAction(
						data_provider, L"Удалить метку"));
					RegisterAction(delete_mark_action, axes);						

					//Восстановление разметки профиля
					VActionPtr load_action = VActionPtr(new VReloadParameterAction(
						GetActiveDataSet(), PRM_PROF_MARKS, L"Восстановить разметку профиля"));									
					RegisterAction(load_action, axes);

					//Объявление элемента вертикальной кривой
					VActionPtr set_curve_action = VActionPtr(new VSetActiveFlagsAction(data_provider, 1, 
						L"Объявить вертикальной кривой"));
					RegisterAction(set_curve_action, axes);
					VActionPtr set_line_action = VActionPtr(new VSetActiveFlagsAction(data_provider, 0, 
						L"Объявить прямой"));
					RegisterAction(set_line_action, axes);

					//Привязка меток к профилю
					VActionPtr adjust_mark_height = VActionPtr(new VAdjustProfileMarkHeight(GetActiveDataSet(), 
						L"Привязать высоты меток к профилю"));
					RegisterAction(adjust_mark_height, axes);
				}
				else if (command_parser.GetCommand()==L"EXTRACT_STATION_WAY")//Выделение станционного пути
				{														
					//Получаем информацию о станции
					uint32_t station_code=GetActiveDataSet()->GetStationCode();
					std::string station_name(GetActiveDataSet()->GetStationName());
					std::string park_name(GetActiveDataSet()->GetParkName());							
					
					//Выделение бокового пути
					VActionPtr selection_to_section = VActionPtr(new VExtractStationWay(GetActiveDataSet(), 
						in_database, transaction, selection_info, select_dialog, station_code, station_name,
						park_name, L"Выделить путь в новый файл"));
					RegisterAction(selection_to_section, axes);
					selection_to_section->SetHotKey(HK_SPACE);
					Subscribe(EVENT_KEY_DOWN, selection_to_section);							
				}
				else if (command_parser.GetCommand()==L"STATION_WAY_MARKS")//Привязка объектов на станции
				{
					//Получаем информацию о станции
					uint32_t station_code=GetActiveDataSet()->GetStationCode();
					std::string station_name(GetActiveDataSet()->GetStationName());
					std::string park_name(GetActiveDataSet()->GetParkName());
					std::string way_code(GetActiveDataSet()->GetWay());									
					//Метки
					std::wstring get_trail_id=(!IsStationWay()) ? 
						boost::str(boost::wformat(L" (select id from msr_trail where filename='%s') ") % GetActiveDataSet()->GetFileName().GetShortName()) :
						boost::str(boost::wformat(L" (select id from stw_trail where filename='%s') ") % GetActiveDataSet()->GetFileName().GetShortName()) ;		
					VDataProviderPtr data_provider = VDataProviderPtr(new VStationOperatorMarkData(in_database, transaction, GetActiveDataSet(), get_trail_id));
					
					VPointGraph * new_graph=new VPointGraph(data_provider, axes, VColor(255,0,0), VColor(255,255,255), 
						CLIP_AREA, true, POINT_STYLE_CIRCLE);
					VElementPtr graph_ptr=VElementPtr(new_graph);
					Subscribe(EVENT_MOUSE_LEFT_DOWN, graph_ptr);
					Subscribe(EVENT_MOUSE_LEFT_UP, graph_ptr);
					Subscribe(EVENT_MOUSE_MOTION, graph_ptr);
					GetLayer(std::wstring(L"Graph"))->AddElement(graph_ptr);

					//Объекты														
					VDataProviderPtr data_provider2 = VDataProviderPtr(new VStationWayObjectData(in_database, transaction, GetActiveDataSet(), true));
					VProfileObjectsElement * new_graph2=new VProfileObjectsElement(data_provider2, axes, VColor(0,0,0), VColor(0,0,0));
					VElementPtr graph_ptr2=VElementPtr(new_graph2);							
					GetLayer(std::wstring(L"Graph"))->AddElement(graph_ptr2);						
												

					//Добавляем действия							

					//Активация разметки
					VActivateActionPtr activate(new VActivateAction(graph_ptr, L""));						
					activate->SetHotKey(HK_SHIFT_UP);						
					Subscribe(EVENT_KEY_UP, activate);

					//Деактивация разметки
					VDeactivateActionPtr deactivate(new VDeactivateAction(graph_ptr, L""));
					deactivate->SetHotKey(HK_SHIFT_DOWN);
					Subscribe(EVENT_KEY_DOWN, deactivate);

					//Обновление объектов
					VActionPtr refresh_objects_action = VActionPtr(new VRefreshDataAction(data_provider2, L"Обновить отображение объектов"));
					data_provider->SetOnDataChanged(refresh_objects_action);

					//Привязка
					VActionPtr attach_object_action = VActionPtr(new VAttachStationObject(
						data_provider, in_database, transaction, select_dialog, station_code, park_name, 
						way_code, L"Привязать метку к объекту"));
					VGroupActionPtr attach_and_refresh = VGroupActionPtr(new VGroupAction(L"Привязать метку к объекту"));
					attach_and_refresh->AddAction(attach_object_action);
					attach_and_refresh->AddAction(refresh_objects_action);
					RegisterAction(attach_and_refresh, axes);

					//Удаление привязки
					VActionPtr detach_object_action = VActionPtr(new VDetachStationObject(data_provider, 
						in_database, transaction, station_code, park_name, L"Удалить привязки метки к объектам"));
					VGroupActionPtr detach_and_refresh = VGroupActionPtr(new VGroupAction(L"Удалить привязки метки к объектам"));
					detach_and_refresh->AddAction(detach_object_action);
					detach_and_refresh->AddAction(refresh_objects_action);
					RegisterAction(detach_and_refresh, axes);

					//Удаление метки
					VActionPtr delete_mark_action = VActionPtr(new VDeleteDataItemAction(data_provider, 
						L"Удалить метку"));							
					RegisterAction(delete_mark_action, axes);
				}				
				else
				{
					throw VSimpleException(L"Инструмент не поддерживается", command_parser.GetCommand(), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				}
			}
		}
		//Режим масштабирования по Y	
		SetScaleMethod(xml_axes, axes);
	}	
		
	//Устанавливаем границы по координатам			
	SetScaleX(str_to_double(drawing_template.GetGraphics().GetScaleX()));
	SetStartX(GetHorzMin());

	//Дополнительные действия
	VActionPtr set_scale_x=VActionPtr(new VSetScaleX(axes_areas, VScaleQueue(DEFAULT_X_SCALES), select_dialog, L"Масштаб по оси X"));
	AddToolbarAction(set_scale_x, &scale_enabled_xpm[0], &scale_disabled_xpm[0]);
	
	VActionPtr zoom_in_x=VActionPtr(new VZoomInXAction(axes_areas, L"Уменьшить масштаб по оси X"));		
	zoom_in_x->SetHotKey(HK_PLUS);
	zoom_in_x->SetHotKey(HK_NUM_PLUS);	
	Subscribe(EVENT_KEY_DOWN, zoom_in_x);	
	
	VActionPtr zoom_out_x=VActionPtr(new VZoomOutXAction(axes_areas, L"Увеличить масштаб по оси X"));
	zoom_out_x->SetHotKey(HK_MINUS);
	zoom_out_x->SetHotKey(HK_NUM_MINUS);	
	Subscribe(EVENT_KEY_DOWN, zoom_out_x);	
	
	//Формируем заголовок графика
	CreateHeader();
}

/** Закрытие окна*/
void VParameterDrawing::OnClose()
{	
	//Подтверждаем транзакцию		
	database->CommitTransaction(*transaction);
	//Записываем файлы
	bool result=true;
	for (size_t n=0;n<data_set.size();++n)
	{		
		if (data_set.at(n)->IsChanged())
		{
			if (!data_set.at(n)->Save())
			{				
				throw VSimpleException(L"Не удалось записать данные", data_set.at(n)->GetErrorMessage().c_str(), 
					boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
		}
	}		
}

/** Возвращает true, если изменились исходные данные*/
bool VParameterDrawing::IsDataChanged()
{	
	for (size_t n=0;n<data_set.size();++n)
	{		
		if (data_set.at(n)->IsChanged())		
			return true;		
	}
	if (transaction->HasChanges())
	{
		return true;
	}
	return false;
}

/** Устанавливает указатель на диалог сохранения файла*/
void VParameterDrawing::SetSaveFileDialog(VAUISaveFileDialogPtr in_save_file_dialog)
{
	save_file_dialog=in_save_file_dialog;
}

/** Устанавливает указатель на диалог выбора из списка*/
void VParameterDrawing::SetSelectDialog(VAUISelectItemDialogPtr in_select_dialog)
{
	select_dialog=in_select_dialog;
}

/** Устанавливает указатель на диалог прогресса*/
void VParameterDrawing::SetProgressDialog(VAUIProgressDialogPtr in_progress_dialog)
{
	progress_dialog=in_progress_dialog;
}

/** Устанавливает указатель на диалог редактирования свойств*/
void VParameterDrawing::SetPropertyDialog(VAUIPropertyDialogPtr in_property_dialog)
{
	property_dialog=in_property_dialog;
}

/** Устанавливает указатель на диалог ввода текста*/
void VParameterDrawing::SetInputDialog(VAUIInputDialogPtr in_input_dialog)
{
	input_dialog=in_input_dialog;
}

/** Подставляет параметр в SQL запросы*/	
void VParameterDrawing::SubstParameter(const wstring& in_name, const wstring& in_value)
{
	subst.insert(subst_pair(in_name, in_value));
}

/** Удалить все области*/
void VParameterDrawing::ClearAreas()
{
	areas.clear();	
	axes_areas.clear();
}

/** Сохранение SVG*/
void VParameterDrawing::SaveDocumentSVG(const char * const file_name)
{	
	cairo_surface_t * svg_surface;	
	cairo_t * svg_cr;		
	//Временно увеличиваем область рисования так чтобы в нее поместился ВЕСЬ график.	
	double prev_visible_width=GetVisibleWidth();
	double prev_abscoord_start=GetStartX();	
	double svg_width=((GetHorzMax() - GetHorzMin())/GetPageSize())*GetVisibleWidth();
	SetVisibleSize(svg_width, GetVisibleHeight());		
	SetStartX(GetHorzMin());	
	svg_surface = cairo_svg_surface_create(file_name, GetVisibleWidth(), GetVisibleHeight());
	svg_cr = cairo_create(svg_surface);		
	Paint(svg_cr);				
	SetVisibleSize(prev_visible_width, GetVisibleHeight());				
	SetStartX(prev_abscoord_start);	
	cairo_destroy(svg_cr);
	cairo_surface_destroy(svg_surface);
}

/** Сохранение PDF*/
void VParameterDrawing::SaveDocumentPDF(const char * const file_name)
{	
	cairo_surface_t * pdf_surface;	
	cairo_t * pdf_cr;		
	pdf_surface = cairo_pdf_surface_create(file_name, GetVisibleWidth(), GetVisibleHeight());	
	pdf_cr = cairo_create(pdf_surface);					
	double abscoord_start_prev=GetStartX();
	double abscoord_end_prev=GetStartX()+GetPageSize();
	SetStartX(GetHorzMin());
	//Включаем номера страниц	
	GetLayer(std::wstring(L"PageNumbers"))->SetVisible(true);
	//PDF - многостраничный документ	
	for (size_t page=0; page<GetPageCount(); ++page)
	{
		GoToPage(page);		
		Paint(pdf_cr);	
		cairo_surface_show_page(pdf_surface);
	}
	//Выключаем номера страниц	
	GetLayer(std::wstring(L"PageNumbers"))->SetVisible(false);
	cairo_destroy(pdf_cr);
	cairo_surface_destroy(pdf_surface);			
	SetStartX(abscoord_start_prev);
}

/** Сохранение в файл только видимой части*/
void VParameterDrawing::SaveVisible(const char * const file_name, FILE_FORMAT format)
{
	bool cross_visible=GetLayer(std::wstring(L"Dynamic"))->GetVisible();	
	GetLayer(std::wstring(L"Dynamic"))->SetVisible(false);	
	VDrawing::SaveVisible(file_name, format);	
	GetLayer(std::wstring(L"Dynamic"))->SetVisible(cross_visible);
}

/** Сохранение документа целиком - реализуется в потомках*/
void VParameterDrawing::SaveDocument(const char * const file_name, FILE_FORMAT format)
{	
	bool cross_visible=GetLayer(std::wstring(L"Dynamic"))->GetVisible();	
	GetLayer(std::wstring(L"Dynamic"))->SetVisible(false);
	switch (format)
	{
		case SVG:
			SaveDocumentSVG(file_name);
			break;
		case PDF:
			SaveDocumentPDF(file_name);
			break;
		case PNG:
			break;		
		default:
			break;
	}	
	GetLayer(std::wstring(L"Dynamic"))->SetVisible(cross_visible);
}

/** Возвращает число страниц документа*/
size_t VParameterDrawing::GetPageCount()
{
	double abs_length=GetPageSize();
	double scr_length=GetHorzMax()-GetHorzMin();
	return static_cast<size_t>(ceil(scr_length/abs_length));
}

/** Переходит на страницу с указанным номером*/
void VParameterDrawing::GoToPage(int in_page)
{
	ScrollToHorz(GetHorzMin()+in_page*GetPageSize());
	page_info->SetPage(in_page+1);
}

/** Прокрутить к нужной позиции*/
void VParameterDrawing::ScrollToHorz(double in_value)
{
	if (in_value >= GetHorzMin() && in_value <= GetHorzMax())
	{		
		SetStartX(in_value);
		changed=true;
	}
	else
		changed=false;
}

/** Возвращает виртуальную позицию по горизонтали*/
double VParameterDrawing::GetHorzPos()
{	
	return GetStartX();
}

/** Возвращает минисально возможную позицию по горизонтали*/
double VParameterDrawing::GetHorzMin()
{		
	//FixMe!!! не учитывать ABSCOORD_SPACE здесь, учитывать только при позиционировании
	switch (GetGraphics().GetAbscoordMargins())
	{	
		//Границы по измерениям
		default:
		case MARGINS_FROM_DATA_SET:
		{
			if (data_set.empty())
				return (IsStationWay() ? aprinfo->GetAbsCoordBase() : aprinfo->GetAbsCoord(route_section.GetSection().start))-ABSCOORD_SPACE;				
			else
				return GetMinAbs()-ABSCOORD_SPACE;
		}
		//Если границы по априорным данным
		case MARGINS_FROM_SELECTED_SECTION:
		{
			return (IsStationWay() ? aprinfo->GetAbsCoordBase() : aprinfo->GetAbsCoord(route_section.GetSection().start))-ABSCOORD_SPACE;			
		}
	}	
}

/** Возвращает максимально возможную позицию по горизонтали*/
double VParameterDrawing::GetHorzMax()
{	
	//FixMe!!! не учитывать ABSCOORD_SPACE здесь, учитывать только при позиционировании
	switch (GetGraphics().GetAbscoordMargins())
	{	
		//Границы по измерениям
		default:
		case MARGINS_FROM_DATA_SET:
		{
			if (data_set.empty())
				return (IsStationWay() ? aprinfo->GetAbsCoordEnd() : aprinfo->GetAbsCoord(route_section.GetSection().end))+ABSCOORD_SPACE;				
			else
				return GetMaxAbs()+ABSCOORD_SPACE;
		}
		//Если границы по априорным данным
		case MARGINS_FROM_SELECTED_SECTION:
		{
			return (IsStationWay() ? aprinfo->GetAbsCoordEnd() : aprinfo->GetAbsCoord(route_section.GetSection().end))+ABSCOORD_SPACE;			
		}
	}	
}

/** Устанавливает размеры областей*/
void VParameterDrawing::AdjustAreas()
{
	//Устанавливаем размеры областей с графиками	
	for (area_sizer_itr i=area_sizers.begin(); i!=area_sizers.end(); ++i)
	{		
		(*i)->CalcSizeAndPos(drawing_template.GetGraphics().GetTopMargin()/10.0, 
			drawing_template.GetGraphics().GetLeftMargin()/10.0, drawing_template.GetGraphics().GetBottomMargin()/10.0, 
			drawing_template.GetGraphics().GetRightMargin()/10.0, GetVisibleWidth(), GetVisibleHeight(), device);
	}
	if (header_area)
	{
		//Размеры области заголовка
		header_area->SetPosition(device->DrawingToDeviceY(drawing_template.GetGraphics().GetLeftMargin()/10.0),0);
		header_area->SetSize(GetVisibleWidth()
			-device->DrawingToDeviceY(drawing_template.GetGraphics().GetLeftMargin()/10.0)
			-device->DrawingToDeviceY(drawing_template.GetGraphics().GetRightMargin()/10.0), 
			device->DrawingToDeviceY(drawing_template.GetGraphics().GetTopMargin()/10.0));
	}
	if (footer_area)
	{
		//Размеры области под графиком
		footer_area->SetPosition(device->DrawingToDeviceY(drawing_template.GetGraphics().GetLeftMargin()/10.0), 
			GetVisibleHeight()-device->DrawingToDeviceY(drawing_template.GetGraphics().GetBottomMargin()/10.0));
		footer_area->SetSize(GetVisibleWidth()
			-device->DrawingToDeviceY(drawing_template.GetGraphics().GetLeftMargin()/10.0)
			-device->DrawingToDeviceY(drawing_template.GetGraphics().GetRightMargin()/10.0), 
			device->DrawingToDeviceX(drawing_template.GetGraphics().GetBottomMargin()/10.0));
	}	
}

/** Устанавливает текущий масштаб*/
void VParameterDrawing::SetScaleX(double in_scale)
{		
	for (axes_itr i=axes_areas.begin(); i!=axes_areas.end(); ++i)
	{
		(*i)->SetScaleX(in_scale);
	}
}

/** Возвращает текущий масштаб*/
double VParameterDrawing::GetScaleX()
{	
	return axes_areas.empty() ? 1 : (*axes_areas.begin())->GetScaleX();
}

/** Устанавливает начало координат по оси X*/
void VParameterDrawing::SetStartX(double in_start_x)
{
	for (axes_itr i=axes_areas.begin(); i!=axes_areas.end(); ++i)
	{
		(*i)->SetStartX(in_start_x);
	}
}

/** Возвращает начало координат по оси X*/
double VParameterDrawing::GetStartX()
{
	return axes_areas.empty() ? 0 : (*axes_areas.begin())->GetStartX();
}

/** Возвращает размер страницы в абс. координатах*/
double VParameterDrawing::GetPageSize()
{
	double area_width=(*axes_areas.begin())->GetArea()->GetWidth();
	double left_coord=(*axes_areas.begin())->AreaToParamX(0);
	double right_coord=(*axes_areas.begin())->AreaToParamX(area_width);
	return right_coord-left_coord;
}

/** Формирует список допустимых действий для точки*/
void VParameterDrawing::CreateContextMenu(double x, double y)
{	
	current_actions.clear();
	current_actions=standart_actions;		
	//Добавляем действия активной системы координат
	for (size_t i=0; i<axes_areas.size(); ++i)
	{		
		if (axes_areas.at(i)->GetActive())
		{
			std::vector<VActionPtr> axes_actions = actions.find(axes_areas.at(i))->second;
			for (size_t k=0; k<axes_actions.size(); ++k)
			{
				current_actions.push_back(axes_actions.at(k));
			}
		}
	}
}

/** Выполняется при первой отрисовке*/
void VParameterDrawing::OnShow()
{
	//Устанавливаем масштабы осей по умолчанию
	for (axes_itr i=axes_areas.begin(); i!=axes_areas.end(); ++i)
	{
		if ((*i)->AllowDefaultScaleY())
			(*i)->SetDefaultScaleY();
	}
}
