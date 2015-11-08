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

///������
class VXMLStaticGraph
{
public:
	///����������� ������ VXMLStaticGraph
	VXMLStaticGraph(wstring name)
	{
		Name=name;
		SetDefaults();
	}

	///����������� �� ��������� ������ VXMLStaticGraph ������
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
	///��� �������
	wstring Name;
	///������� ������� (������)
	wstring Legend;
	///���� �������, ���� ������� ��������� �������� ����� ����� � �������, �� ������ ��������� �������� ������������ ��� 
	///���������� ������������ �����
	wstring Color;
	///"�������" ������ FixMe!!! ������������ ������ wstring Color;
	VColorQueue Colors;
	///���� �������, ������������ ��� ������� ���� BARCHART
	wstring FillColor;
	///"�������" ������ ������� FixMe!!! ������������ ������ wstring FillColor;
	VColorQueue FillColors;
	///������� �����
	double Width;
	///����� ��������� �������� "LINE", "LINE", "BARCHART", "POINT".
	wstring Type;
	///- �������� ������, ����� ��������� ��������: "PROFILE()", "PROF_MARKS()", "PARAMETER(<��� ��������� PRM>)", 
	///	"MEAN(<��� ��������� PRM>)", "DEVIATION(<��� ��������� PRM>)"
	///	"QUERY(<����� ������� select>)" � �.�.
	wstring DataType;	
	///����� - �������� ������ �� ��������� �������
	int Clip;	
	///����� - ���������� ��������
	bool ShowLabels;	
	///����� - ���������� �������
	bool ShowLegend;
	///����� - ����������� ��������� ����
	bool TrackMouse;

	///���������� �������� �� ���������
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

///������� ����
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
	///��� �������
	wstring Name;
	///���� �������, ���� ������� ��������� �������� ����� ����� � �������, �� ������ ��������� �������� ������������ ��� 	
	///���������� ������������ �����
	wstring Color;	
	///���� �������
	wstring FillColor;	
	///������� �����
	double Width;
	///- �������� ������, ����� ��������� ��������: "SLOPE()", "BRIDGES()" � �.�.	
	wstring DataType;
	///����� - ������� ������ ������ ���� ���, ������� �� ������ �� ���� ��������
	int Clip;

	///���������� �������� �� ���������
	void SetDefaults()
	{
		Color=L"0x0000ffff";
		FillColor=L"0x000000ff";
		Width=default_line_width;
		DataType=L"OP_MARKS()";
		Clip=CLIP_HORIZONTAL;
	}
};

/// ������� ���������
class VXMLAxes
{
public:
	/// ����������� ������ VXMLAxes
	VXMLAxes(wstring name)
	{
		Name=name;
		SetDefaults();
	}

	/// ����������� �� ��������� ������ VXMLAxes ������
	VXMLAxes()
	{	
		SetDefaults();
	}

	///�������� ������
	void AddStaticGraph(VXMLStaticGraph staticGraph)
	{
		static_graphs.push_back(staticGraph);
	}

	///������� ������
	void RemoveStaticGraph(size_t i)
	{
		if (i>=0 && i<=static_graphs.size())
		{
			vector<VXMLStaticGraph>::iterator p=static_graphs.begin();
			p+=i;
			static_graphs.erase(p);
		}
	}

	///���������� ���������� ��������
	size_t GetStaticGraphCount() {return static_graphs.size();}

	///���������� ������
	VXMLStaticGraph & GetStaticGraph(size_t i) {return static_graphs.at(i);}

	///�������� ����������
	void AddTool(VXMLTool tool)
	{
		tools.push_back(tool);
	}

	///������� ����������
	void RemoveTool(size_t i)
	{
		if (i>=0 && i<=tools.size())
		{
			vector<VXMLTool>::iterator p=tools.begin();
			p+=i;
			tools.erase(p);
		}
	}

	///���������� ���������� ������������
	size_t GetToolCount() {return tools.size();}

	///���������� ����������
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
	///�������
	vector<VXMLStaticGraph> static_graphs;
	//�����������
	vector<VXMLTool> tools;	
	///�������� ������� ���������
	wstring Name;
	///����� - ���������� ����� �� ��� X
	bool ShowXLines;
	///����� - ���������� ��������� �� ��� X
	bool ShowXCoord;
	///����� - ���������� ����� �� ��� Y
	bool ShowYLines;
	///����� - ���������� ��������� �� ��� Y
	bool ShowYCoord;
	///����� - ���������� ������� �� ��� X
	bool ShowXScale;
	///����� - ���������� ������� �� ��� Y
	bool ShowYScale;
	///����� - ���������� ������� ��� Y
	bool ShowLegendY;
	///������� ��� Y
	wstring LegendY;
	//����� ��������������� - "���������� ��������" ��� "�������"
	wstring ScaleMethod;
	///������������ �������� �� ��� Y, ����� ����� �������� "auto", ����� ������������ �������� ����� ����������� 
	///�������������, �� ������ ������ ��������.
	wstring MaxY;
	///����������� �������� �� ��� Y, ����� ����� �������� "auto".
	wstring MinY;
	///������� �� ��� Y.
	wstring ScaleY;	
	///������ ��������� ���������.
	wstring AvailScaleY;
	///����� - ��� ������ ������� �� ������� ������� - ������� ������� ���������.
	bool Relative;	

	///���������� �������� �� ���������
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
		ScaleMethod=L"���������� ��������";
		MaxY=L"auto";
		MinY=L"auto";
		ScaleY=DEFAULT_Y_SCALE;		
		AvailScaleY=DEFAULT_Y_SCALES;
		Relative=false;				
	}
};

/// �������
class VXMLArea
{
public:
	/// ����������� ������ VXMLArea
	VXMLArea(wstring name)
	{
		Name=name;
		SetDefaults();
	}

	/// ����������� ������ VXMLArea �� ��������� ������
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
	///������� ���������
	VXMLAxes Axes;
	///�������� �������
	wstring Name;
	///������������� ������ ������� �� ������ � ��������� �� ���������� ��� ������� ����� �����������
	int RelSize;
	///������������� ��������� ������� � ��������� �� ���������� ��� ������� ����� �����������. 
	int RelPos;
	///���������� ����� ������ �������
	bool ShowFrame;
	///���������� �������� �������
	bool ShowName;

	///���������� �������� �� ���������
	void SetDefaults()
	{		
		RelSize=100;	
		RelPos=0;
		ShowFrame=true;
		ShowName=false;
	}
};

///�������� �������
class VXMLGraphics
{
public:
	///����������� ������ VXMLGraphics
	VXMLGraphics(wstring name)
	{
		Name=name;
		SetDefaults();
	}

	///����������� �� ��������� ������ VXMLGraphics ������
	VXMLGraphics()
	{
		SetDefaults();
	}

	/// ���������� ����� ������� � �����
	void AddArea(VXMLArea area) {areas.push_back(area);}
	/// ���������� ����� ������� ����� �������������
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

	/// ���������� �������
	VXMLArea & GetArea(size_t i) {return areas.at(i);}
	/// ���������� ��������� �������
	VXMLArea & GetLastArea() {return areas.at(areas.size()-1);}
	/// ���������� ���������� ��������
	size_t GetAreaCount() {return areas.size();}
	/// ������� �������
	void RemoveArea(size_t i)
	{
		if (i>=0 && i<=areas.size())
		{
			vector<VXMLArea>::iterator p=areas.begin();
			p+=i;
			areas.erase(p);
		}
	}

	///������� ��� �������
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
	///������ ��������
	vector<VXMLArea> areas;
	///�������� ��������, ��� ������� �� �������� � ���� ������ ��������.
	wstring Name;
	///������ � ���� ������, � ������� ����� ������� �������
	wstring Group;
	///������ ����������� ��� ����� ���������� � ����� �� ���������� �������
	wstring Filter;	
	///������ �����
	int LeftMargin;
	///������ ������
	int TopMargin;
	///������ ������
	int RightMargin;
	///������ �����
	int BottomMargin;
	///�������� ��������� ������
	APRINFO_SOURCE AprinfoSource;		
	///������� �� ��� �
	std::wstring ScaleX;
	///�������� ������ �� ��������� ���������� (1 - ��, 0 - ���������, �� ���������)
	ABSCOORD_MARGINS AbscoordMargins;
	///����
	ACTUAL_DATE Date;

	///���������� �������� �� ���������
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

/** �����, ����������� ������������� XML-��������*/
class VXMLEditor
{
public:
	/** ����������� ������ VXMLEditor*/
	VXMLEditor()
	{		
		xercesc::XMLPlatformUtils::Initialize();
	}

	/** ���������� ������ VXMLEditor*/
	~VXMLEditor()
	{
		parser.reset();
		error_handler.reset();
		xercesc::XMLPlatformUtils::Terminate();
	}

	/** �������� ������ �� �����*/
	void Load(const wstring fileName);
	/** ���������� ������ � ����*/
	void Save(const wstring fileName);	

	/** ���������� �������� �������*/
	VXMLGraphics & GetGraphics() {return Graphics;}
	/** ���������� �������� �������*/
	const VXMLGraphics& GetGraphics() const {return Graphics;}	
private:
	VXMLGraphics Graphics;
	boost::shared_ptr<XercesDOMParser> parser;
	boost::shared_ptr<ErrorHandler> error_handler;
};