#include "piecewise.h"
#include "vexception.h"

using namespace pro;
using namespace piecewise;

/// Конструктор
linear::linear() : cached_search(_lines.end()) {};

/// Конструктор
linear::linear(const linear& in_source)
{
	*this=in_source;
}

/// Конструктор по точкам
linear::linear(const std::vector<point_t>& in_points)
{
	if (in_points.size()>1)
	{
		for (size_t i=1; i<in_points.size(); ++i)
		{
			add(section(in_points.at(i-1),in_points.at(i)));
		}		
	}
	else
	{
		throw VSimpleException(L"Требуется не менее 2-х точек", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
};

/// Добавляет секцию
void linear::add(const section& in_section)
{
	_lines.push_back(in_section);	
	std::sort(_lines.begin(), _lines.end());
	cached_search=_lines.begin();
}

/// Добавляет 
void linear::add(const point_t& in_left, const point_t& in_right, bool in_close_left, bool in_close_right)
{
	add(section(in_left, in_right, in_close_left, in_close_right));
}

/// Проверяет корректность
bool linear::valid() const
{
	if (_lines.empty())
		return false;
	for (size_t i=1; i<_lines.size(); ++i)
	{
		if (!(_lines.at(i).domain & _lines.at(i-1).domain).empty())
		{
			return false;			
		}
	}
	return true;
}

/// Возвращает участок на котором находится точка
const std::vector<section>::const_iterator& linear::find_section(double in_x) const
{
	struct _predicate
	{
		double x;
		_predicate(double in_x) : x(in_x) {};
		bool operator()(const section& in_section) {return in_section.contains(x);};
	};
	if (cached_search==_lines.end() || !cached_search->contains(in_x))
		cached_search = std::find_if(_lines.begin(), _lines.end(), _predicate(in_x));
	return cached_search;
}

/// Проверяет область определения
bool linear::value_in_domain(double in_x) const
{
	return find_section(in_x) !=_lines.end();
}

/// Возвращает значение y
double linear::y(double in_x) const
{
	return find_section(in_x)->y(in_x);
}

/// operator=
linear& linear::operator=(const linear& in_source)
{
	_lines=in_source._lines;
	cached_search=_lines.begin();
	return *this;
}

/// Возвращает список точек
std::vector<point_t> linear::points() const
{
	std::vector<point_t> result;
	for (size_t i=0; i<_lines.size(); ++i)
	{
		result.push_back(_lines.at(i).left());
		result.push_back(_lines.at(i).right());
	}
	return result;
}