#pragma once

//std
#include <stack>

//boost
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

//V
#include "calclib.h"
#include "dbfunc.h"
#include "axes.h"
#include "elements.h"
#include "providers.h"
#include "actions.h"
#include "VXMLEditor.h"
#include "VProgressInterface.h"

using namespace xercesc;

/** ����� �� ����������*/
#define ABSCOORD_SPACE 100

/** ������ �������*/
class VCommandParser
{
private:

	/** ������� ������*/
	struct trimmer
	{	
		void operator() (std::wstring & in_out_string) { boost::trim(in_out_string); };
	};

	/** �������*/
	std::wstring command;
	/** ���������*/
	std::vector<std::wstring> parameters;
	/** ���������*/
	std::wstring parameter;
public:
	/** �����������*/
	VCommandParser(std::wstring& in_command);
	/** ���������� �������*/
	const std::wstring& GetCommand() const
	{
		return command;
	};
	/** ���������� ����� ����������*/
	size_t GetParameterCount() const
	{
		return parameters.size();
	};
	/** ���������� ��������*/
	const std::wstring& GetParameter(size_t in_index)
	{
		return parameters.at(in_index);
	};
	/** ���������� ��������� ����� �������*/
	const std::wstring& GetParameters()
	{
		return parameter;
	};
};

/** ������� ����� ��� ������� ����������*/
class VParameterDrawing : public VDrawing
{
protected:	

	/** ������� ����*/
	VRouteSection route_section;

	/** ������ ���������� SQL*/
	typedef std::map<std::wstring, std::wstring> subst_t;
	typedef std::map<std::wstring, std::wstring>::iterator subst_itr;
	typedef std::pair<std::wstring, std::wstring> subst_pair;
	subst_t subst;


	/** ������ ��������*/
	std::map<std::wstring, VAreaPtr> areas;
	/** ���������� ��������� ��� ������ ��������*/
	typedef std::map<std::wstring, VAreaPtr>::iterator area_itr;
	/** ���������� ���� ��� ������ ��������*/
	typedef std::pair<std::wstring, VAreaPtr> area_pair;

	/** ������ ������ ����� �� ���������*/
	VColorQueue default_colors;

	/** ������ ��������*/
	std::vector<VAreaSizerPtr> area_sizers;
	/** ���������� ��������� ��� ������ ��������*/
	typedef std::vector<VAreaSizerPtr>::iterator area_sizer_itr;

	/** ������ ���� ���������*/
	std::vector<VAxesPtr> axes_areas;	
	typedef std::vector<VAxesPtr>::iterator axes_itr;
	
	/** ������� ���������*/
	VAreaPtr header_area;	
	/** ������� ��� ��������*/
	VAreaPtr footer_area;	
		
	/** ������ �������� */	
	typedef std::map<VAxesPtr, std::vector<VActionPtr>> actions_t;
	typedef std::map<VAxesPtr, std::vector<VActionPtr>>::iterator actions_itr;
	typedef std::pair<VAxesPtr, std::vector<VActionPtr>> actions_pair;
	actions_t actions;
	
	/** ������ � ������� ��������*/
	VPageInfoPtr page_info;

	/** ������ � ���������� ������� ���������*/
	VSelectionInfoPtr selection_info;
						
	/** ������ ����������� SQL*/
	std::set<std::wstring> sql_set;
	/** ������ ��������*/
	std::set<std::wstring> constant_set;


	/** ���� � ����� ������� ���������*/
	wstring template_path;		
	/** ������ �������*/
	VXMLEditor drawing_template;	
	
	/** ����� ������*/
	std::vector<VDataSetPtr> data_set;
	/** ��������� �� �������� ���� ������*/
	VDataSetPtr active_data_set;
	/** ��������� �� ��������� ���� ������*/
	VDataSetPtr reference_data_set;
	/** ��������� �� ��*/
	VIBDatabasePtr database;
	/** ����������*/
	VIBTransactionPtr transaction;
	/** ��������� ����������*/
	VAprinfoPtr aprinfo;			
	/** ��������� �� ������ ���������� �����*/
	VAUISaveFileDialogPtr save_file_dialog;	
	/** ��������� �� ������ ������ �� ������*/
	VAUISelectItemDialogPtr select_dialog;	
	/** ��������� �� ������ ���������*/
	VAUIProgressDialogPtr progress_dialog;
	/** ��������� �� ������ �������������� �������*/
	VAUIPropertyDialogPtr property_dialog;
	/** ��������� �� ������ ����� ������*/
	VAUIInputDialogPtr input_dialog;
	/** ������������ �����*/
	VCoordinatesDataPtr coordinates;
	
	//������
		
	/** ���������� SVG*/
	virtual void SaveDocumentSVG(const char * const file_name);
	/** ���������� PDF*/
	virtual void SaveDocumentPDF(const char * const file_name);	
	/** �������� ������ ��������*/
	void ClearAreas();	
	/** ��������� ������ ���������� �������� ��� �����*/
	virtual void CreateContextMenu(double x, double y);	
	/** ���������� �������� ����*/
	VXMLGraphics& GetGraphics();
	/** ���������� �������� ����*/
	const VXMLGraphics& GetGraphics() const;	
	/** ������� ���������*/
	void CreateHeader();
	/** ������� ����*/
	void CreateLayers();
	/** ������� �������*/	
	VAreaPtr CreateArea(VXMLArea& in_xml_area);
	/** ������� ���*/
	VAxesPtr CreateAxes(VXMLAxes& in_xml_axes, VAreaPtr in_area);
	/** ��������� �������� � ����*/
	void AddMenuAction(VActionPtr in_action);
	/** ��������� �������� � �������� ���������*/
	void RegisterAction(VActionPtr in_action, VAxesPtr in_axes);
	/** ��������� �������� � ������*/	
	void AddToolbarAction(VActionPtr in_action, xpm_t* in_enabled_xpm, xpm_t* in_disabled_xpm);		
	/** ������������ ������ ��������������� ��� ����*/
	void SetScaleMethod(VXMLAxes in_xml_axes, VAxesPtr in_axes);
	/** ���������� true ���� ����� ���� �� ������������ ������*/
	bool IsStationWay() const;

	/** ���������� ������� �������*/
	std::wstring FormatGraphLegend(VXMLStaticGraph& xml_graph, size_t in_index, const std::wstring& in_comment=L"") const;
	/** ������� �������� ������ ������������� ���� � ���������� ��������� �� ����*/
	std::vector<VDataProviderPtr> CreateDataProvider(VAxesPtr in_axes, VXMLStaticGraph& in_xml_graph);
	/** ������� ������*/
	VElementPtr CreateGraph(const std::wstring& in_element_name, VDataProviderPtr in_data_provider, 
		VAxesPtr in_axes, VXMLStaticGraph& in_xml_static_graph);	

	/** ���������� ���������� ���� �������*/
	VTime GetActualDate();
	/** ���������� ��� �����������*/
	std::wstring GetActualDir();
	/** ���������� ��� ����*/
	std::wstring GetActualWay();
	/** ���������� ������� ����*/
	VWaySection GetActualSection();
	/** ���������� �������� ����*/
	VDataSetPtr GetActiveDataSet();
	/** ���������� ��������� ����*/
	VDataSetPtr GetReferenceDataSet();
	/** ���������� ����������� ���������� �� ������*/
	double GetMinAbs();
	/** ���������� ������������ ���������� �� ������*/
	double GetMaxAbs();
	/** �������� ������*/
	void LoadData(const std::vector<std::wstring>& in_data_file_names, VIBDatabasePtr in_database,
		const VRouteSection& in_route_section, bool in_from_db);	

public:
	/** �����������*/
	VParameterDrawing();
	/** ����������*/
	virtual ~VParameterDrawing();
	/** �������� �������*/
	void LoadTemplate(const wstring& in_template_path);	
	/** ���������� �������*/
	void SaveTemplate(const wstring& in_template_path);	
	/** ���������� �������� �������*/
	const std::wstring& GetName() const;
	/** ���������� �������� ������*/
	std::wstring GetGroup();
	/** ���������� ������*/
	std::wstring GetFilter();	
	/** ���������� ������*/
	void Prepare(const std::vector<std::wstring>& in_data_file_names, VIBDatabasePtr in_database, 
		const VRouteSection& in_route_section, bool in_from_db);
	/** �������� ����*/
	virtual void OnClose();
	/** ���������� true, ���� ���������� �������� ������*/
	virtual bool IsDataChanged();
	/** ������������� ��������� �� ������ ���������� �����*/
	void SetSaveFileDialog(VAUISaveFileDialogPtr in_save_file_dialog);
	/** ������������� ��������� �� ������ ������ �� ������*/
	void SetSelectDialog(VAUISelectItemDialogPtr in_select_dialog);	
	/** ������������� ��������� �� ������ ���������*/
	void SetProgressDialog(VAUIProgressDialogPtr in_progress_dialog);	
	/** ������������� ��������� �� ������ �������������� �������*/
	void SetPropertyDialog(VAUIPropertyDialogPtr in_property_dialog);
	/** ������������� ��������� �� ������ ����� ������*/
	void SetInputDialog(VAUIInputDialogPtr in_input_dialog);	
	/** ����������� �������� � SQL �������*/	
	void SubstParameter(const wstring& in_name, const wstring& in_value);
	/** ���������� � ���� ������ ������� �����*/
	virtual void SaveVisible(const char * const file_name, FILE_FORMAT format);	
	/** ���������� ��������� ������� - ����������� � ��������*/
	virtual void SaveDocument(const char * const file_name, FILE_FORMAT format);
	/** ���������� ����� ������� ���������*/
	virtual size_t GetPageCount();
	/** ��������� �� �������� � ��������� �������*/
	virtual void GoToPage(int in_page);
	/** ���������� � ������ �������*/
	virtual void ScrollToHorz(double in_value);	

	/** ���������� ������� �� �����������*/
	virtual double GetHorzPos();
	/** ���������� ���������� ��������� ������� �� �����������*/
	virtual double GetHorzMin();
	/** ���������� ����������� ��������� ������� �� �����������*/
	virtual double GetHorzMax();
	
	/** ������������� ������� ��������*/
	void AdjustAreas();
			
	/** ������������� ������� ������� �� X*/
	void SetScaleX(double in_scale);
	/** ���������� ������� �������*/
	double GetScaleX();	
	/** ������������� ������ ��������� �� ��� X*/
	void SetStartX(double in_start_x);
	/** ���������� ������ ��������� �� ��� X*/
	double GetStartX();		
	/** ���������� ������ �������� � ���. �����������*/
	double GetPageSize();
	
	/** ����������� ��� ������ ���������*/
	void OnShow();	
};
typedef boost::shared_ptr<VParameterDrawing> VParameterDrawingPtr;

