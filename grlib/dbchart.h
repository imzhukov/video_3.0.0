#pragma once

#include "VDrawing.h"
#include "elements.h"

/// Ширина изображения по умолчанию
#define CHART_DEFAULT_WIDTH 800
/// Высота изображения по умолчанию
#define CHART_DEFAULT_HEIGHT 600

/// Описание графика
class VChartGraphDescription
{
private:
	/// Запрос
	std::wstring query;
	/// Тип графика
	std::wstring type;
	/// Легенда
	std::wstring legend;
	/// Цвет графика
	VColor color;
	/// Цвет заливки
	VColor fill_color;
public:
	/// Конструктор
	VChartGraphDescription(const std::wstring& in_query, const std::wstring& in_type, const std::wstring& in_legend, 
		const VColor& in_color=VColor(0,0,255,255), const VColor& in_fill_color=VColor(0,0,255,80));
	/// Возвращает текст запроса
	const std::wstring& GetQuery() const;
	/// Возвращает тип графика
	const std::wstring& GetType() const;
	/// Возвращает текст легенды
	const std::wstring& GetLegend() const;
	/// Возвращает цвет графика
	const VColor& GetColor() const;
	/// Возвращает цвет заливки
	const VColor& GetFillColor() const;
};

/// Чарт 
class VDBChart : public VDrawing
{
private:
	/// Указатель на БД
	VIBDatabasePtr database;
	/// Указатель на транзакцию
	VIBTransactionPtr transaction;
	/// Список описаний графиков
	std::vector<VChartGraphDescription> graph_descriptions;
	
	bool show_x_lines;
	bool show_x_coord;
	bool show_y_lines;
	bool show_y_coord;

public:
	/// Конструктор
	VDBChart(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction);
	/// Добавляет запрос
	void AddQuery(const VChartGraphDescription& in_description);	
	/// Подготовка данных
	void Prepare(const std::wstring& in_title, double in_width, double in_height);
	void SetShowXLines(bool in_show_x_lines);
	void SetShowXCoord(bool in_show_x_coord);
	void SetShowYLines(bool in_show_y_lines);
	void SetShowYCoord(bool in_show_y_coord);
};