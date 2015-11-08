#pragma once

#include "VDrawing.h"
#include "elements.h"
#include "oversteps.h"
#include "shiny.h"

#define ZERO_COLOR COLOR_HTML_BLUE
#define CONSTANT_COLOR COLOR_HTML_BLUE
#define ORIGINAL_NORMA_COLOR VColor(0,128,0)
#define CORRECTED_NORMA_COLOR VColor(0,0,128)
#define OVERSTEP_COLOR VColor(255,0,0,255)
#define OVERSTEP_FILL_COLOR VColor(255,0,0,80)
#define PARAMETER_COLOR COLOR_HTML_BLACK
#define CORRUPT_COLOR VColor(255,0,0,80)
#define Y_LINE_COLOR COLOR_HTML_BLACK
#define APR_SWITCH_COLOR VColor(0,0,255,128)
#define OPT_SWITCH_COLOR COLOR_HTML_BLACK
#define ZERO_DASH VDash(1.0, 1.0)
#define CONSTANT_DASH VDash(0.25, 1.0)
#define Y_LINE_DASH VDash(0.25, 1.0)

#define FRAME_WIDTH min_line_width
#define CONSTANT_LINE_WIDTH min_line_width
#define PARAMETER_LINE_WIDTH min_line_width
#define NORMA_LINE_WIDTH min_line_width


/// ����� ������ PDF
enum PDF_SPLIT_POLICY {NO_SPLIT=0, SPLIT_KM=1};

/// ����������������� �����
class VRTGraph : public VDrawing
{
protected:	

	//������

	/// �������� ���������� ��������
	HDC printer_hdc;	
	/// ����� ��������� ������ � PDF
	bool pdf_enabled;	
	/// ����� ��������� ������
	bool printing_enabled;
	/// ���������� 
	double dpi;
	/// ��� ����� PDF
	std::wstring pdf_file_name;
	/// ����� ��������
	size_t page_number;	
	/// ���������	
	VRTKmProcessor km;
	/// �����
	std::wstring vps;
	/// ���
	std::wstring person;
	/// ����� ������ �������
	VTime ride_time;
	/// ����� ��������� �������� �����
	VTime report_time;
	/// �������� ������
	std::wstring data_source;	

	
	/// ������ ��������
	std::vector<VAreaSizerPtr> area_sizers;	
	typedef std::vector<VAreaSizerPtr>::iterator area_sizer_itr;
	/// ������ ���� ���������
	std::vector<VAxesPtr> axes_areas;	
	typedef std::vector<VAxesPtr>::iterator axes_itr;

	/// ������� ���������
	VAreaPtr header_area;
	/// ������� ����������
	VAreaPtr summary_area;
	/// ������� ���������� ���.
	VAreaPtr summary_area_ext;
	/// ������� ������ ��������
	VAreaPtr page_area;
	/// ������� ��������� ������
	VAreaPtr aprinfo_area;
	/// ������� ���. ��������
	VAreaPtr restriction_area;
	/// ������� ���������� ������������ �����������
	VAreaPtr count_area;
	/// ������� ���� �����������
	VAreaPtr length_area;
	/// ������� ������� �����������
	VAreaPtr value_area;
	/// ������� �������� �����������
	VAreaPtr degree_area;
	/// ������� ���� �����������
	VAreaPtr type_area;
	/// ������� ��������� �����������
	VAreaPtr coord_area;
	/// ������� �������
	VAreaPtr event_area;	
	/// ������� ���� ��������
	VAreaPtr graph_area;
	

	/// ��� ��������� ����������
	VAxesPtr aprinfo_axes;
	/// ������� ���. ��������
	VAxesPtr restriction_axes;
	/// ������� ���������� ������������ �����������
	VAxesPtr count_axes;
	/// ������� ���� �����������
	VAxesPtr length_axes;
	/// ������� ������� �����������
	VAxesPtr value_axes;
	/// ������� �������� �����������
	VAxesPtr degree_axes;
	/// ������� ���� �����������
	VAxesPtr type_axes;
	/// ������� ��������� �����������
	VAxesPtr coord_axes;
	/// ������� �������
	VAxesPtr event_axes;
	/// ������� ��������
	VAxesPtr graph_axes;
	
	/// ����������
	VCoordinatesDataPtr coordinates;
	/// ���������
	VUserTextProviderPtr header;
	/// ������
	VUserTextProviderPtr summary;
	/// �������
	VUserTextProviderPtr peregon;
	/// �������
	VRTEventInfoDataPtr events;
	/// ������� ��������
	VDequeDataPtr borders;
	/// ���������� �������
	VRTEventInfoDataPtr debug_events;
	/// ��������� ��������
	VDequeDataPtr passport_status;

	/// ������ � ������� ��������
	VPageInfoPtr page_info;	

	/// �������� PDF
	VPDFDocumentPtr pdf_document;


	/// ����� ��� ������
	std::wstring folder;
	/// ������� ����� PDF
	std::wstring prefix;
	/// ����� ������ PDF
	PDF_SPLIT_POLICY pdf_split_policy;
	/// ������� ��������� PDF
	bool pdf_ready;
	/// ������� ������� ��� ������
	std::deque<VRawData> print_queue;

	// �������
	
	/// ���������� ��������� ��������
	virtual void PrepareData(const VRTKmDataPtr& in_km)=0;	
	/// ������� ������
	virtual void ClearData()=0;
	/// ������� ���������� � ���������
	virtual void ClearSummary()=0;
	/// ��������� �������� � ������
	void RenderPage(const VRTKmDataPtr& in_km);
	/// ��������� ��� ����� PDF
	virtual std::wstring FormatPDFFileName(int in_km);
	/// ������ �������� � PDF
	void SaveCurrentKmPDF(cairo_surface_t * in_source_surface);
	/// ��������� PDF
	void OpenPDF(const std::wstring& in_filename);
	/// ��������� PDF
	void ClosePDF(); 
	/// ������ ��������
	void PrintCurrentKm(cairo_surface_t * in_source_surface);
	///���������� true ���� �������� ����� ��������
	virtual bool MustBePrinted(const VRTKmDataPtr& in_km)=0;

public:

	/// �����������
	VRTGraph(const std::wstring& in_folder, const std::wstring& in_prefix);
	/// ����������
	~VRTGraph();
	/// ������������� ����������
	void SetDPI(double in_dpi);
	/// ����������
	virtual void Prepare()=0;
	/// ��������� ��������
	void ProcessKm(const VRTKmDataPtr& in_km);
	/// ������������ �������
	void AdjustAreas();
	/// ������������� ������� �������
	void SetScaleX(double in_scale);
	/// ������������� ������ ��������� �� ��� X
	void SetStartX(double in_start_x);

	/// ������������� ����� ������
	void SetVPS(const std::wstring& in_vps);
	/// ������������� ���
	void SetPerson(const std::wstring& in_person);
	/// ������������� ����� �������
	void SetTime(const VTime& in_time);
	/// ������������� �������� ������
	void SetDataSource(const std::wstring& in_data_source);

	/// ������������� ����� ������ � PDF
	void SetPDFEnabled(bool in_pdf_enabled);
	/// ���������� true ���� �������� ������ � PDF
	bool GetPDFEnabled() const;

	/// ������������� ����� ������ �� �������
	void SetPrintingEnabled(bool in_printing_enabled);
	/// ���������� true ���� �������� ������ �� �������
	bool GetPrintingEnabled() const;
	
	/// ������������� ����� ������ ������ PDF
	void SetPDFSplitPolicy(PDF_SPLIT_POLICY in_pdf_split_policy);
	/// ���������� ����� ������ ������ PDF
	PDF_SPLIT_POLICY GetPDFSplitPolicy();
	
	/// ���������� ���������� ������� ��� ������
	size_t GetPageCount();
	/// ���������� ��������
	VRawData GetPage();
	/// ������� ��������
	void PopPage();
};
typedef boost::shared_ptr<VRTGraph> VRTGraphPtr;

/// ����� ���
class VRTGeometryGraph : public VRTGraph
{
private:
	
	/// ������� �������� �����
	VAreaPtr pit_left_area;
	/// ������� �������� ������
	VAreaPtr pit_right_area;
	/// ������� ��
	VAreaPtr width_area;
	/// ������� �������� �����
	VAreaPtr rich_left_area;
	/// ������� �������� ������
	VAreaPtr rich_right_area;
	/// ������� ������
	VAreaPtr level_area;
	/// ������� ������� ��� ��� ���
	VAreaPtr anp_area;
		
	
	/// ������� �������� �����
	VAxesPtr pit_left_axes;
	/// ������� �������� ������
	VAxesPtr pit_right_axes;
	/// ������� ��
	VAxesPtr width_axes;
	/// �����
	VAxesPtr wear_axes;
	/// ������� �������� �����
	VAxesPtr rich_left_axes;
	/// ������� �������� ������
	VAxesPtr rich_right_axes;
	/// ������� ������
	VAxesPtr level_axes;
	/// ������� ������� ��� ��� � �.�.
	VAxesPtr anp_axes;				

	/// �������� �����
	VVectorDataPtr pit_left; 
	/// �������� ������
	VVectorDataPtr pit_right; 
	/// ��
	VVectorDataPtr width;
	/// ����� �����������
	VVectorDataPtr wear; 
	/// ����� ����������
	VVectorDataPtr wear_passport; 
	/// �������� �����
	VVectorDataPtr rich_left; 
	/// �������� ������
	VVectorDataPtr rich_right; 
	/// �������
	VVectorDataPtr level;
	/// ����� RFID
	VDequeDataPtr rfid;

	/// ���������� ����� �� ������
	VDequeDataPtr level_passport_line_original;
	/// ���������� ����� �� ��������
	VDequeDataPtr rich_passport_line_original;
	/// ���������� ����� �� ��
	VDequeDataPtr width_passport_line_original;
	
	/// ���������� ����� �� ������ (�����������������)
	VDequeDataPtr level_passport_line_corrected;
	/// ���������� ����� �� �������� (�����������������)
	VDequeDataPtr rich_left_passport_line_corrected;
	/// ���������� ����� �� �������� (�����������������)
	VDequeDataPtr rich_right_passport_line_corrected;
	/// ���������� ����� �� �� (�����������������)
	VDequeDataPtr width_passport_line_corrected;
	
	/// ������� ������������	
	VSwitchDataPtr opt_switches;
	/// ������� ���������
	VSwitchDataPtr apr_switches;
	/// ������� ������������	
	VSwitchSectionsDataPtr opt_switch_sections;
	/// ������� ���������
	VSwitchSectionsDataPtr apr_switch_sections;
	
	/// ��� ����
	VDataProviderPtr aprinfo_spala;
	/// ����������� ����
	VDataProviderPtr aprinfo_straight_side;	

	/// �������� �����
	VRTOverstepDataPtr pit_left_oversteps;
	/// �������� �����
	VRTOverstepDataPtr pit_right_oversteps;
	/// ��
	VRTOverstepDataPtr width_oversteps;
	/// �������� �����
	VRTOverstepDataPtr rich_left_oversteps;
	/// �������� �����
	VRTOverstepDataPtr rich_right_oversteps;
	/// �������
	VRTOverstepDataPtr level_oversteps;
	/// ������������ ���������
	VDequeDataPtr anp_oversteps;
	/// �������� ��������� ������������� ���������
	VDequeDataPtr psi_oversteps;
	/// ����� ��
	VDequeDataPtr otvod_width_oversteps;			

	/// ���������� ��������� ��������
	void PrepareData(const VRTKmDataPtr& in_km);
	/// ��������� ��� ����� PDF
	std::wstring FormatPDFFileName(int in_km);
	///���������� true ���� �������� ����� ��������
	bool MustBePrinted(const VRTKmDataPtr& in_km);

public:
	/// �����������
	VRTGeometryGraph(const std::wstring& in_folder, const std::wstring& in_prefix) : VRTGraph(in_folder, in_prefix) 
	{
		Prepare();
	};
	/// ����������
	void Prepare();	
	/// ������� ������
	void ClearData();
	/// ������� ���������� � ���������
	void ClearSummary();	
};


/// ����� ���. ����������
class VRTExtraGraph : public VRTGraph
{
private:
	
	/// ������� ������
	VAreaPtr wear_area;		
	/// ������� ������� �����
	VAreaPtr zazor_left_area;
	/// ������� ������� ������
	VAreaPtr zazor_right_area;			
	
	/// ������� ������
	VAxesPtr wear_axes;		
	/// ������� ������ ������
	VAxesPtr zazor_left_axes;
	/// ������� ������ �������
	VAxesPtr zazor_right_axes;
	
	/// �����
	VVectorDataPtr wear;
	/// ����� ����������
	VVectorDataPtr wear_passport;  	
	/// ����� ����� (������)
	VDequeDataPtr zazor_left_opt;
	/// ����� ������ (������)
	VDequeDataPtr zazor_right_opt;
	/// ����� ����� (���������)
	VDequeDataPtr zazor_left_mag;
	/// ����� ������ (���������)
	VDequeDataPtr zazor_right_mag;
	/// ����� RFID
	VDequeDataPtr rfid;
			
	/// �����
	VRTOverstepDataPtr wear_oversteps;	
	/// ����� �����
	VRTOverstepDataPtr zazor_left_oversteps;
	/// ����� ������
	VRTOverstepDataPtr zazor_right_oversteps;	

	/// ������� ������������	
	VSwitchDataPtr opt_switches;
	/// ������� ���������
	VSwitchDataPtr apr_switches;
	/// ������� ������������	
	VSwitchSectionsDataPtr opt_switch_sections;
	/// ������� ���������
	VSwitchSectionsDataPtr apr_switch_sections;

	/// ���������� ��������� ��������
	void PrepareData(const VRTKmDataPtr& in_km);
	/// ��������� ��� ����� PDF
	std::wstring FormatPDFFileName(int in_km);
	///���������� true ���� �������� ����� ��������
	bool MustBePrinted(const VRTKmDataPtr& in_km);

public:
	/// �����������
	VRTExtraGraph(const std::wstring& in_folder, const std::wstring& in_prefix) : VRTGraph(in_folder, in_prefix) 
	{
		Prepare();
	};
	/// ����������
	void Prepare();	
	/// ������� ������
	void ClearData();
	/// ������� ���������� � ���������
	void ClearSummary();
};


/// ����� ���������� ���������
class VRTGraphProcessor : public VDataIn<VRTKmDataPtr>, public VDataOut<VRawData>
{
private:
	VRTGeometryGraph GeometryGraph;
	VRTExtraGraph ExtraGraph;
	std::deque<VRawData> print_queue;
public:
	/// �����������
	VRTGraphProcessor(const std::wstring& in_folder, const std::wstring& in_prefix, const VTime& in_time, const std::wstring& in_source) : GeometryGraph(in_folder, in_prefix),
		ExtraGraph(in_folder, in_prefix) 
	{
		GeometryGraph.SetDPI(DPI_DEFAULT_PRINTER);	
		GeometryGraph.SetVPS(DETECT_OPTIONS.GetOption(L"VPS").GetValue());
		GeometryGraph.SetPerson(DETECT_OPTIONS.GetOption(L"CHIEF").GetValue());
		GeometryGraph.SetTime(in_time);
		GeometryGraph.SetDataSource(in_source);
		GeometryGraph.SetPDFSplitPolicy(DETECT_OPTIONS.GetOption(L"SPLIT_DIAGRAM").Compare(OPTION_YES) ? SPLIT_KM : NO_SPLIT);
		GeometryGraph.SetPDFEnabled(DETECT_OPTIONS.GetOption(L"SAVE_GEOMETRY").Compare(OPTION_YES));
		//������� ����� �������������	
		ExtraGraph.SetDPI(DPI_DEFAULT_PRINTER);	
		ExtraGraph.SetVPS(DETECT_OPTIONS.GetOption(L"VPS").GetValue());
		ExtraGraph.SetPerson(DETECT_OPTIONS.GetOption(L"CHIEF").GetValue());
		ExtraGraph.SetTime(in_time);
		ExtraGraph.SetDataSource(in_source);
		ExtraGraph.SetPDFSplitPolicy(DETECT_OPTIONS.GetOption(L"SPLIT_DIAGRAM").Compare(OPTION_YES) ? SPLIT_KM : NO_SPLIT);
		ExtraGraph.SetPDFEnabled(DETECT_OPTIONS.GetOption(L"SAVE_EXTRA").Compare(OPTION_YES));
	};
	/// ���� ������
	void Put(const VRTKmDataPtr& in_data)
	{
		GeometryGraph.ProcessKm(in_data);
		while (GeometryGraph.GetPageCount()>0)
		{
			print_queue.push_back(GeometryGraph.GetPage());
			GeometryGraph.PopPage();
		}
		ExtraGraph.ProcessKm(in_data);
		while (ExtraGraph.GetPageCount()>0)
		{
			print_queue.push_back(ExtraGraph.GetPage());
			ExtraGraph.PopPage();
		}
	};
	/// ����������� ������
	void SetPrintGeometry(bool in_enabled)///FixMe!!! ������������ OPTION
	{
		GeometryGraph.SetPrintingEnabled(in_enabled);
	};
	/// ����������� ������
	void SetPrintExtra(bool in_enabled)///FixMe!!! ������������ OPTION
	{
		ExtraGraph.SetPrintingEnabled(in_enabled);
	};
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const 
	{
		return print_queue.size()>0;
	};	
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const
	{
		return print_queue.empty();
	};
	/// ���������� ������ ������� ������
	const VRawData& Front() const
	{
		return print_queue.front();
	};
	/// ����������� ������ ������� ������
	void Pop()
	{
		print_queue.pop_front();
	};		
};

typedef boost::shared_ptr<VRTGraphProcessor> VRTGraphProcessorPtr;