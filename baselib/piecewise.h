#pragma once

#include "interval.h"
#include "mathlib.h"//FixMe!!! ����� ��� line_t, �� ����� line_t ��������� � ��������� ����


namespace pro
{
namespace piecewise
{

/// �������
class section
{
	public:
		/// ��������
		interval<double> domain;
		/// ��������� ������
		line_t line;
		
		/// �����������
		section(const interval<double>& in_domain, const line_t& in_line) : domain(in_domain), line(in_line){};

		/// �����������
		section(const point_t& in_left, const point_t& in_right, bool in_close_left=true, bool in_close_right=true) : 
			domain(in_left.x, in_right.x, in_close_left, in_close_right), line(in_left, in_right){};
		
		/// ���������� ����� �����
		point_t left() const
		{
			return point_t(domain.left_bound().value, line.Y(domain.left_bound().value));
		};
		/// ���������� ������ �����
		point_t right() const
		{
			return point_t(domain.right_bound().value, line.Y(domain.right_bound().value));
		};
		/// ���������� true, ���� �������� �������� �������� �����
		bool contains(double in_x) const
		{
			return domain.contains(in_x);
		};
		/// ���������� ��������
		double y(double in_x) const
		{
			if (contains(in_x))
			{
				return line.Y(in_x);
			}
			else
			{
				return 0;//FixMe!!! ����������???
			}
		};
		/// operator<
		bool operator<(const section& in_section) const
		{
			return domain.right_bound().value <= in_section.domain.left_bound().value;
		}
};


/// �������-�������� �������
class linear
{
private:
	/// ������ ������
	std::vector<section> _lines;
	/// ����������� �������� ������������ ��� ������ ������
	mutable std::vector<section>::const_iterator cached_search;
public:
	/// �����������
	linear();
	/// �����������
	linear(const linear& in_source);
	/// ����������� �� ������
	linear(const std::vector<point_t>& in_points);
	/// ��������� ������
	void add(const section& in_section);
	/// ��������� ������ �� ���� ������ 
	void add(const point_t& in_left, const point_t& in_right, bool in_close_left=true, bool in_close_right=true);
	/// ��������� ������������
	bool valid() const;
	/// ���������� ������� �� ������� ��������� �����
	const std::vector<section>::const_iterator& find_section(double in_x) const;
	/// ��������� ������� �����������
	bool value_in_domain(double in_x) const;
	/// ���������� �������� y
	double y(double in_x) const;
	/// operator=
	linear& operator=(const linear& in_source);
	/// ���������� ������ �����
	std::vector<point_t> points() const;
};

	
}//end of nampespace piecewise
}//end of namespace pro