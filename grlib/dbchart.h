#pragma once

#include "VDrawing.h"
#include "elements.h"

/// ������ ����������� �� ���������
#define CHART_DEFAULT_WIDTH 800
/// ������ ����������� �� ���������
#define CHART_DEFAULT_HEIGHT 600

/// �������� �������
class VChartGraphDescription
{
private:
	/// ������
	std::wstring query;
	/// ��� �������
	std::wstring type;
	/// �������
	std::wstring legend;
	/// ���� �������
	VColor color;
	/// ���� �������
	VColor fill_color;
public:
	/// �����������
	VChartGraphDescription(const std::wstring& in_query, const std::wstring& in_type, const std::wstring& in_legend, 
		const VColor& in_color=VColor(0,0,255,255), const VColor& in_fill_color=VColor(0,0,255,80));
	/// ���������� ����� �������
	const std::wstring& GetQuery() const;
	/// ���������� ��� �������
	const std::wstring& GetType() const;
	/// ���������� ����� �������
	const std::wstring& GetLegend() const;
	/// ���������� ���� �������
	const VColor& GetColor() const;
	/// ���������� ���� �������
	const VColor& GetFillColor() const;
};

/// ���� 
class VDBChart : public VDrawing
{
private:
	/// ��������� �� ��
	VIBDatabasePtr database;
	/// ��������� �� ����������
	VIBTransactionPtr transaction;
	/// ������ �������� ��������
	std::vector<VChartGraphDescription> graph_descriptions;
	
	bool show_x_lines;
	bool show_x_coord;
	bool show_y_lines;
	bool show_y_coord;

public:
	/// �����������
	VDBChart(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction);
	/// ��������� ������
	void AddQuery(const VChartGraphDescription& in_description);	
	/// ���������� ������
	void Prepare(const std::wstring& in_title, double in_width, double in_height);
	void SetShowXLines(bool in_show_x_lines);
	void SetShowXCoord(bool in_show_x_coord);
	void SetShowYLines(bool in_show_y_lines);
	void SetShowYCoord(bool in_show_y_coord);
};