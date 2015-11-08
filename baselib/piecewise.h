#pragma once

#include "interval.h"
#include "mathlib.h"//FixMe!!! нужно для line_t, но лучше line_t перенести в отдельный файл


namespace pro
{
namespace piecewise
{

/// Участок
class section
{
	public:
		/// Интервал
		interval<double> domain;
		/// Уравнение прямой
		line_t line;
		
		/// Конструктор
		section(const interval<double>& in_domain, const line_t& in_line) : domain(in_domain), line(in_line){};

		/// Конструктор
		section(const point_t& in_left, const point_t& in_right, bool in_close_left=true, bool in_close_right=true) : 
			domain(in_left.x, in_right.x, in_close_left, in_close_right), line(in_left, in_right){};
		
		/// Возвращает левую точку
		point_t left() const
		{
			return point_t(domain.left_bound().value, line.Y(domain.left_bound().value));
		};
		/// Возвращает правую точку
		point_t right() const
		{
			return point_t(domain.right_bound().value, line.Y(domain.right_bound().value));
		};
		/// Возвращает true, если интервал содержит заданное число
		bool contains(double in_x) const
		{
			return domain.contains(in_x);
		};
		/// Возвращает значение
		double y(double in_x) const
		{
			if (contains(in_x))
			{
				return line.Y(in_x);
			}
			else
			{
				return 0;//FixMe!!! исключение???
			}
		};
		/// operator<
		bool operator<(const section& in_section) const
		{
			return domain.right_bound().value <= in_section.domain.left_bound().value;
		}
};


/// Кусочно-линейная функция
class linear
{
private:
	/// Список секций
	std::vector<section> _lines;
	/// Кэшированый итератор используемый для поиска секции
	mutable std::vector<section>::const_iterator cached_search;
public:
	/// Конструктор
	linear();
	/// Конструктор
	linear(const linear& in_source);
	/// Конструктор по точкам
	linear(const std::vector<point_t>& in_points);
	/// Добавляет секцию
	void add(const section& in_section);
	/// Добавляет секцию по двум точкам 
	void add(const point_t& in_left, const point_t& in_right, bool in_close_left=true, bool in_close_right=true);
	/// Проверяет корректность
	bool valid() const;
	/// Возвращает участок на котором находится точка
	const std::vector<section>::const_iterator& find_section(double in_x) const;
	/// Проверяет область определения
	bool value_in_domain(double in_x) const;
	/// Возвращает значение y
	double y(double in_x) const;
	/// operator=
	linear& operator=(const linear& in_source);
	/// Возвращает список точек
	std::vector<point_t> points() const;
};

	
}//end of nampespace piecewise
}//end of namespace pro