#pragma once

#include <vector>
#include <string>
#include <os.h>

//Xerces
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include <boost/format.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "VDrawing.h"

using namespace std;
using namespace xercesc;


const std::wstring DEFAULT_X_SCALES=L"1;2;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20;21;22;23;24;25;30;35;40;45;50;55;60;65;70;75;80;85;90;95;100;110;120;130;140;150;160;170;180;190;200;300;400;500;600;700;800;900;1000;1500;2000;2500;3000;3500;4000;4500;5000;6000;7000;8000;9000;10000";
const std::wstring DEFAULT_X_SCALE=L"100";

const std::wstring DEFAULT_Y_SCALES=L"0.1/100/0.1";
const std::wstring DEFAULT_Y_SCALE=L"1";

enum APRINFO_SOURCE {APRINFO_FROM_DB=0, APRINFO_FROM_FILE=1};
enum ABSCOORD_MARGINS {MARGINS_FROM_DATA_SET=0, MARGINS_FROM_SELECTED_SECTION=1};
enum ACTUAL_DATE {DATE_CURRENT=0, DATE_TRAIL=1};

///График
class VXMLStaticGraph
{
public:
	///Конструктор класса VXMLStaticGraph
	VXMLStaticGraph(wstring name)
	{
		Name=name;
		SetDefaults();
	}

	///Конструктор по умолчанию класса VXMLStaticGraph закрыт
	VXMLStaticGraph()
	{
		SetDefaults();
	}	

	void SetName(const wstring& name) {Name=name;}
	const wstring& GetName() const {return Name;}
	void SetLegend(const wstring& legend) {Legend=legend;}
	const wstring& GetLegend() const {return Legend;};
	void SetColor(wstring color) 
	{
		Color=color;
		Colors=VColorQueue(color);
	}
	wstring GetColor() {return Color;}
	VColorQueue& GetColors() {return Colors;}
	void SetFillColor(wstring fillColor) 
	{
		FillColor=fillColor;
		FillColors=VColorQueue(fillColor);
	}
	wstring GetFillColor() {return FillColor;}
	VColorQueue& GetFillColors() {return FillColors;}	
	void SetWidth(double width) {Width=width;}
	double GetWidth() {return Width;}
	void SetType(wstring type) {Type=type;}
	wstring GetType() {return Type;}
	void SetDataType(wstring dataType) {DataType=dataType;}
	wstring GetDataType() {return DataType;}	
	void SetClip(int clip) {Clip=clip;}
	int GetClip() {return Clip;}
	void SetShowLabels(bool show_labels) {ShowLabels=show_labels;}
	bool GetShowLabels() {return ShowLabels;}
	void SetShowLegend(bool show_legend) {ShowLegend=show_legend;}
	bool GetShowLegend() {return ShowLegend;}	

	void SetTrackMouse(bool track_mouse) {TrackMouse=track_mouse;}
	bool GetTrackMouse() {return TrackMouse;}		
private:	
	///имя графика
	wstring Name;
	///легенда графика (шаблон)
	wstring Legend;
	///цвет графика, если указаны несколько значений через точку с запятой, то каждое следующее значение используется для 
	///следующего загруженного файла
	wstring Color;
	///"Очередь" цветов FixMe!!! использовать вместо wstring Color;
	VColorQueue Colors;
	///цвет заливки, используется для графика типа BARCHART
	wstring FillColor;
	///"Очередь" цветов заливки FixMe!!! использовать вместо wstring FillColor;
	VColorQueue FillColors;
	///толщина линии
	double Width;
	///может принимать значения "LINE", "LINE", "BARCHART", "POINT".
	wstring Type;
	///- источник данных, может принимать значения: "PROFILE()", "PROF_MARKS()", "PARAMETER(<код параметра PRM>)", 
	///	"MEAN(<код параметра PRM>)", "DEVIATION(<код параметра PRM>)"
	///	"QUERY(<текст запроса select>)" и т.п.
	wstring DataType;	
	///опция - обрезать график за пределами области
	int Clip;	
	///Опция - показывать значения
	bool ShowLabels;	
	///Опция - показывать легенду
	bool ShowLegend;
	///Опция - отслеживать положение мыши
	bool TrackMouse;

	///Установить значения по умолчанию
	void SetDefaults()
	{
		Legend=L"$n [$f] $c";
		Color=GRAPH_COLOURS;
		Colors=VColorQueue(Color);
		FillColor=ALL_RED_COLOURS;
		FillColors=VColorQueue(FillColor);
		Width=default_line_width;
		Type=L"LINE";
		DataType=L"PARAMETER(WIDTH)";		
		Clip=CLIP_HORIZONTAL;
		ShowLabels=false;
		ShowLegend=true;		
		TrackMouse=true;
	}
};

///Объекты пути
class VXMLTool
{
public:
	VXMLTool(wstring name)
	{
		Name=name;
		SetDefaults();
	}

	VXMLTool()
	{
		SetDefaults();
	}

	void SetName(wstring name) {Name=name;}
	wstring GetName() {return Name;}
	void SetColor(wstring color) {Color=color;}
	wstring GetColor() {return Color;}
	void SetFillColor(wstring fillColor) {FillColor=fillColor;}
	wstring GetFillColor() {return FillColor;}
	void SetWidth(double width) {Width=width;}
	double GetWidth() {return Width;}
	void SetDataType(wstring dataType) {DataType=dataType;}
	wstring GetDataType() {return DataType;}
	void SetClip(int clip) {Clip=clip;}
	int GetClip() {return Clip;}
private:
	///имя графика
	wstring Name;
	///цвет графика, если указаны несколько значений через точку с запятой, то каждое следующее значение используется для 	
	///следующего загруженного файла
	wstring Color;	
	///цвет заливки
	wstring FillColor;	
	///толщина линии
	double Width;
	///- источник данных, может принимать значения: "SLOPE()", "BRIDGES()" и т.п.	
	wstring DataType;
	///опция - строить график только один раз, сколько бы файлов не было выделено
	int Clip;

	///Установить значения по умолчанию
	void SetDefaults()
	{
		Color=L"0x0000ffff";
		FillColor=L"0x000000ff";
		Width=default_line_width;
		DataType=L"OP_MARKS()";
		Clip=CLIP_HORIZONTAL;
	}
};

/// Система координат
class VXMLAxes
{
public:
	/// Конструктор класса VXMLAxes
	VXMLAxes(wstring name)
	{
		Name=name;
		SetDefaults();
	}

	/// Конструктор по умолчанию класса VXMLAxes закрыт
	VXMLAxes()
	{	
		SetDefaults();
	}

	///Добавить график
	void AddStaticGraph(VXMLStaticGraph staticGraph)
	{
		static_graphs.push_back(staticGraph);
	}

	///Удалить график
	void RemoveStaticGraph(size_t i)
	{
		if (i>=0 && i<=static_graphs.size())
		{
			vector<VXMLStaticGraph>::iterator p=static_graphs.begin();
			p+=i;
			static_graphs.erase(p);
		}
	}

	///Возвращает количество графиков
	size_t GetStaticGraphCount() {return static_graphs.size();}

	///Возвращает график
	VXMLStaticGraph & GetStaticGraph(size_t i) {return static_graphs.at(i);}

	///Добавить инструмент
	void AddTool(VXMLTool tool)
	{
		tools.push_back(tool);
	}

	///Удалить инструмент
	void RemoveTool(size_t i)
	{
		if (i>=0 && i<=tools.size())
		{
			vector<VXMLTool>::iterator p=tools.begin();
			p+=i;
			tools.erase(p);
		}
	}

	///Возвращает количество инструментов
	size_t GetToolCount() {return tools.size();}

	///Возвращает инструмент
	VXMLTool & GetTool(size_t i) {return tools.at(i);}

	void SetName(wstring name) {Name=name;}
	wstring GetName() {return Name;}
	void SetShowXLines(bool showXLines) {ShowXLines=showXLines;}
	bool GetShowXLines() {return ShowXLines;}
	void SetShowXCoord(bool showXCoord) {ShowXCoord=showXCoord;}
	bool GetShowXCoord() {return ShowXCoord;}
	void SetShowYLines(bool showYLines) {ShowYLines=showYLines;}
	bool GetShowYLines() {return ShowYLines;}
	void SetShowYCoord(bool showYCoord) {ShowYCoord=showYCoord;}
	bool GetShowYCoord() {return ShowYCoord;}
	void SetShowXScale(bool showXScale) {ShowXScale=showXScale;}
	bool GetShowXScale() {return ShowXScale;}
	void SetShowYScale(bool showYScale) {ShowYScale=showYScale;}
	bool GetShowYScale() {return ShowYScale;}
	void SetShowLegendY(bool showLegendY) {ShowLegendY=showLegendY;}
	bool GetShowLegendY() {return ShowLegendY;}
	void SetLegendY(wstring legendY) {LegendY=legendY;}
	wstring GetLegendY() {return LegendY;}
	void SetScaleMethod(wstring scaleMethod) {ScaleMethod=scaleMethod;}
	wstring GetScaleMethod() {return ScaleMethod;}
	void SetMaxY(wstring maxY) {MaxY=maxY;}
	wstring GetMaxY() {return MaxY;}
	void SetMinY(wstring minY) {MinY=minY;}
	wstring GetMinY() {return MinY;}
	void SetScaleY(wstring scaleY) {ScaleY=scaleY;}
	wstring GetScaleY() {return ScaleY;}	
	void SetAvailScaleY(wstring availScaleY) {AvailScaleY=availScaleY;}
	wstring GetAvailScaleY() {return AvailScaleY;}	
	void SetRelative(bool relative) {Relative=relative;}
	bool GetRelative() {return Relative;}	

private:
	///графики
	vector<VXMLStaticGraph> static_graphs;
	//инструменты
	vector<VXMLTool> tools;	
	///название системы координат
	wstring Name;
	///опция - показывать сетку по оси X
	bool ShowXLines;
	///опция - показывать оцифровку по оси X
	bool ShowXCoord;
	///опция - показывать сетку по оси Y
	bool ShowYLines;
	///опция - показывать оцифровку по оси Y
	bool ShowYCoord;
	///опция - показывать масштаб по оси X
	bool ShowXScale;
	///опция - показывать масштаб по оси Y
	bool ShowYScale;
	///опция - показывать легенду оси Y
	bool ShowLegendY;
	///легенда оси Y
	wstring LegendY;
	//Режим масштабирования - "Предельные значения" или "Масштаб"
	wstring ScaleMethod;
	///максимальное значение по оси Y, может иметь значение "auto", тогда максимальное значение будет вычисляться 
	///автоматически, на основе данных графиков.
	wstring MaxY;
	///минимальное значение по оси Y, может иметь значение "auto".
	wstring MinY;
	///масштаб по оси Y.
	wstring ScaleY;	
	///список доступных масштабов.
	wstring AvailScaleY;
	///опция - при выходе графика за пределы области - смещать систему координат.
	bool Relative;	

	///Установить значения по умолчанию
	void SetDefaults()
	{
		ShowXLines=true;
		ShowXCoord=true;
		ShowYLines=true;
		ShowYCoord=true;
		ShowXScale=false;
		ShowYScale=false;
		ShowLegendY=false;
		LegendY=L"";
		ScaleMethod=L"Предельные значения";
		MaxY=L"auto";
		MinY=L"auto";
		ScaleY=DEFAULT_Y_SCALE;		
		AvailScaleY=DEFAULT_Y_SCALES;
		Relative=false;				
	}
};

/// Область
class VXMLArea
{
public:
	/// Конструктор класса VXMLArea
	VXMLArea(wstring name)
	{
		Name=name;
		SetDefaults();
	}

	/// Конструктор класса VXMLArea по умолчанию закрыт
	VXMLArea()
	{
		SetDefaults();
	}

	void SetAxes(VXMLAxes axes) {Axes=axes;}
	VXMLAxes & GetAxes() {return Axes;}
	void SetName(wstring name) {Name=name;}
	wstring GetName() {return Name;}
	void SetRelSize(int relSize) {RelSize=relSize;}
	int GetRelSize() {return RelSize;}
	void SetRelPos(int relPos) {RelPos=relPos;}
	int GetRelPos() {return RelPos;}
	void SetShowFrame(bool showFrame) {ShowFrame=showFrame;}
	bool GetShowFrame() {return ShowFrame;}
	void SetShowName(bool showName) {ShowName=showName;}
	bool GetShowName() {return ShowName;}

private:
	///Система координат
	VXMLAxes Axes;
	///название области
	wstring Name;
	///относительный размер области по высоте в процентах от выделенной под области части изображения
	int RelSize;
	///относительное положение области в процентах от выделенной под области части изображения. 
	int RelPos;
	///показывать рамку вокруг области
	bool ShowFrame;
	///показывать название области
	bool ShowName;

	///Установить значения по умолчанию
	void SetDefaults()
	{		
		RelSize=100;	
		RelPos=0;
		ShowFrame=true;
		ShowName=false;
	}
};

///Описание графика
class VXMLGraphics
{
public:
	///Конструктор класса VXMLGraphics
	VXMLGraphics(wstring name)
	{
		Name=name;
		SetDefaults();
	}

	///Конструктор по умолчанию класса VXMLGraphics закрыт
	VXMLGraphics()
	{
		SetDefaults();
	}

	/// Добавление новой области в конец
	void AddArea(VXMLArea area) {areas.push_back(area);}
	/// Добавление новой области между существующими
	void AddArea(VXMLArea area, size_t index)
	{
		size_t count=0;
		std::vector<VXMLArea>::iterator p=areas.begin(); 
		
		while (p!=areas.end())
		{
			if (count==index+1)
			{
				p=areas.insert(p,area);
				return;
			}
			count++;
			p++;
		}
		areas.push_back(area);
	}

	/// Возвращает область
	VXMLArea & GetArea(size_t i) {return areas.at(i);}
	/// Возвращает последнюю область
	VXMLArea & GetLastArea() {return areas.at(areas.size()-1);}
	/// Возвращает количество областей
	size_t GetAreaCount() {return areas.size();}
	/// Удаляет область
	void RemoveArea(size_t i)
	{
		if (i>=0 && i<=areas.size())
		{
			vector<VXMLArea>::iterator p=areas.begin();
			p+=i;
			areas.erase(p);
		}
	}

	///Удаляет все области
	void Clear() {areas.clear();}

	void SetName(const wstring& name) {Name=name;}
	const wstring& GetName() const {return Name;}
	void SetGroup(wstring group) {Group=group;}
	wstring GetGroup() {return Group;}
	void SetFilter(wstring filter) {Filter=filter;}
	wstring GetFilter() {return Filter;}	
	void SetLeftMargin(int leftMargin) {LeftMargin=leftMargin;}
	int GetLeftMargin() {return LeftMargin;}
	void SetTopMargin(int topMargin) {TopMargin=topMargin;}
	int GetTopMargin() {return TopMargin;}
	void SetRightMargin(int rightMargin) {RightMargin=rightMargin;}
	int GetRightMargin() {return RightMargin;}
	void SetBottomMargin(int bottomMargin) {BottomMargin=bottomMargin;}
	int GetBottomMargin() {return BottomMargin;}
	void SetAprinfoSource(APRINFO_SOURCE  aprinfoSource) {AprinfoSource=aprinfoSource;}
	APRINFO_SOURCE GetAprinfoSource() {return AprinfoSource;}	
	void SetScaleX(wstring scale_x) {ScaleX=scale_x;}
	wstring GetScaleX() {return ScaleX;}
	void SetAbscoordMargins(ABSCOORD_MARGINS abscoordMargins) {AbscoordMargins=abscoordMargins;}	
	ABSCOORD_MARGINS GetAbscoordMargins() {return AbscoordMargins;}
	void SetDate(ACTUAL_DATE date) {Date=date;}
	ACTUAL_DATE GetDate() {return Date;}

private:
	///список областей
	vector<VXMLArea> areas;
	///название варианта, под которым он появится в окне выбора графиков.
	wstring Name;
	///группа в окне выбора, в которую будет помещен вариант
	wstring Group;
	///фильтр управляющий тем когда показывать а когда не показывать графики
	wstring Filter;	
	///отступ слева
	int LeftMargin;
	///отступ сверху
	int TopMargin;
	///отступ справа
	int RightMargin;
	///отступ снизу
	int BottomMargin;
	///источник априорных данных
	APRINFO_SOURCE AprinfoSource;		
	///масштаб по оси х
	std::wstring ScaleX;
	///источник границ по абсолтной координате (1 - БД, 0 - измерения, по умолчанию)
	ABSCOORD_MARGINS AbscoordMargins;
	///Дата
	ACTUAL_DATE Date;

	///Установить значения по умолчанию
	void SetDefaults()
	{
		Group=L"";
		Filter=L"main";		
		LeftMargin=default_left_margin;
		TopMargin=default_top_margin;
		RightMargin=default_right_margin;
		BottomMargin=default_bottom_margin;
		AprinfoSource=APRINFO_FROM_DB;	
		ScaleX=DEFAULT_X_SCALE;
		AbscoordMargins=MARGINS_FROM_DATA_SET;
		Date=DATE_CURRENT;
	}
};

/** Класс, позволяющий редактировать XML-документ*/
class VXMLEditor
{
public:
	/** Конструктор класса VXMLEditor*/
	VXMLEditor()
	{		
		xercesc::XMLPlatformUtils::Initialize();
	}

	/** Деструктор класса VXMLEditor*/
	~VXMLEditor()
	{
		parser.reset();
		error_handler.reset();
		xercesc::XMLPlatformUtils::Terminate();
	}

	/** Загрузка данных из файла*/
	void Load(const wstring fileName);
	/** Сохранение данных в файл*/
	void Save(const wstring fileName);	

	/** Возвращает описание графика*/
	VXMLGraphics & GetGraphics() {return Graphics;}
	/** Возвращает описание графика*/
	const VXMLGraphics& GetGraphics() const {return Graphics;}	
private:
	VXMLGraphics Graphics;
	boost::shared_ptr<XercesDOMParser> parser;
	boost::shared_ptr<ErrorHandler> error_handler;
};