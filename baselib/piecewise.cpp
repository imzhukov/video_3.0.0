#include "piecewise.h"
#include "vexception.h"

using namespace pro;
using namespace piecewise;

/// �����������
linear::linear() : cached_search(_lines.end()) {};

/// �����������
linear::linear(const linear& in_source)
{
	*this=in_source;
}

/// ����������� �� ������
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
		throw VSimpleException(L"��������� �� ����� 2-� �����", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
};

/// ��������� ������
void linear::add(const section& in_section)
{
	_lines.push_back(in_section);	
	std::sort(_lines.begin(), _lines.end());
	cached_search=_lines.begin();
}

/// ��������� 
void linear::add(const point_t& in_left, const point_t& in_right, bool in_close_left, bool in_close_right)
{
	add(section(in_left, in_right, in_close_left, in_close_right));
}

/// ��������� ������������
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

/// ���������� ������� �� ������� ��������� �����
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

/// ��������� ������� �����������
bool linear::value_in_domain(double in_x) const
{
	return find_section(in_x) !=_lines.end();
}

/// ���������� �������� y
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

/// ���������� ������ �����
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