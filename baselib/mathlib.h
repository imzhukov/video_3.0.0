#pragma once

#define _USE_MATH_DEFINES
#include <tchar.h>
#include <string>
#include <deque>
#include <map>
#include <set>
#include <fstream>
#include <math.h>
#include <boost/format.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

#include "interval.h"

/// �������������� �������� ���������
#define UNDEFINED_VALUE 0

/** ���������� �� ����*/
double round_to(double in_value, double in_base);

/** ���������� ��� double*/
bool equal(double l, double r);

/** ���������� �� ���������� ������*/
double round(double in_value);

/** ���������� � ��������� �� ���������� ����� ����� �������*/
double round(double in_value, unsigned int in_prec);

/** ���������� ����*/
template<class T> T sign(const T & t)
{
	return t>=0 ? static_cast<T>(1) : static_cast<T>(-1);
}

/** �����*/
class point_t
{
public:
	point_t() : x(0), y(0) 
	{};

	point_t(double in_x, double in_y) : x(in_x), y(in_y) 
	{};

	double x, y;

	bool operator == (const point_t & other) const
	{		
		return equal(x, other.x) && equal(y, other.y);
	}

	bool operator != (const point_t & other) const
	{
		return !(*this == other);
	}

	struct less_y
	{
		bool operator()(const point_t& l, const point_t& r) const {return l.y < r.y;};
	};
	struct less_x
	{
		bool operator()(const point_t& l, const point_t& r) const {return l.x < r.x;};
	};
};

/** ���������� ����� ����� �������*/
inline double length(const point_t& in_p1, const point_t& in_p2)
{
	return sqrt(pow(in_p1.x-in_p2.x, 2) + pow(in_p1.y-in_p2.y, 2));
};

/** ��������� ������ ���� A*x + B*y + C = 0*/
class line_t
{
public:
	/** �����������*/
	line_t() : _A(0), _B(0), _C(0)
	{};	

	/** �����������*/
	line_t(double in_A, double in_B, double in_C) : _A(in_A), _B(in_B), _C(in_C)
	{};
	
	/** �����������*/
	line_t(const point_t& in_p1, const point_t& in_p2);

	/** �����������*/
	line_t(const point_t& in_p, double in_angle);
	
	/** ����������� ������ y=const*/
	line_t(double in_y) : _A(0), _B(-1), _C(in_y)
	{};

	//���������� � ������� �������������
	double k() const;
	double b() const;

	//��������� � ����� ����
	double A() const 
	{
		return _A;
	};
	double B() const 
	{
		return _B;
	};
	double C() const 
	{
		return _C;
	};

	//���������� �������� Y �� X
	double Y(double x) const
	{
		return k()*x+b();
	};

	//���������� �������� X �� Y
	double X(double y) const
	{
		return -(B()*y+C())/A();
	}

	bool operator==(const line_t& in_line)
	{
		return A()==in_line.A() && B()==in_line.B() && C()==in_line.C();
	}

	bool operator!=(const line_t& in_line)
	{
		return !(*this==in_line);
		//return A()!=in_line.A() || B()!=in_line.B() || C()!=in_line.C();
	}

private:

	//��������� � ����� ����
	double _A, _B, _C;
};

/** ���������� �� ����� �� ������*/
static double length(const point_t& in_p, const line_t& in_l)
{
	return (in_l.A()*in_p.x + in_l.B()*in_p.y + in_l.C())/sqrt(pow(in_l.A(),2)+pow(in_l.B(),2));
};

/** ���������� ����������� ������ ������������ ������, ���������� ����� �������� �����*/
static line_t parallel(const point_t& in_point, const line_t& in_line)
{
	return line_t(in_line.A(), in_line.B(), -in_line.A()*in_point.x-in_line.B()*in_point.y);
};

/** ��������� ���������� (x-a)^2 + (y-b)^2 = r^2*/
class circle_t
{
public:
	/** �����������*/
	circle_t(double in_a, double in_b, double in_r) : _a(in_a), _b(in_b), _r(in_r)
	{};

	/** ����������� (�� ���� ������) ��. http://faqs.org.ru/science/geometry_faq.htm*/
	circle_t(point_t in_p1, point_t in_p2, point_t in_p3);

	/** */
	double X(){return _a;};
	double Y(){return _b;};
	double R(){return _r;};

private:

	double _a, _b, _r;
};

/** ����*/
class angle_t
{
private:
	/** �������� � ��������*/
	double angle;	

public:
	/** �����������*/
	angle_t() : angle(0)
	{};

	angle_t(double in_angle) : angle(in_angle)
	{};

	angle_t(int in_grad, int in_min, int in_sec);

	/** ���������� �������*/
	double get_rad() const 
	{
		return angle;
	};

	/** ���������� ����*/
	int get_sign() const 
	{
		return angle < 0 ? -1 : 1;
	};

	/** ���������� ������� �� ������*/
	int get_grad() const
	{
		return get_sign()*static_cast<int>(floor(fabs(180.0*angle/M_PI)));
	}

	/** ���������� ������ ��� �����*/
	int get_min() const;

	/** ���������� ������� ��� �����*/
	int get_sec() const;

	/** ����������� �������� ����: ������� � ������*/
	std::wstring format_degrees() const
	{
		return boost::str(boost::wformat(L"%d�%d'") % get_grad() % get_min());
	}

	/** ����������� �������� ���� � ISO 6709*/
	double get_iso_6709() const
	{		
		return 100.0*fabs(double(get_grad())) + double(get_min()) + double(get_sec())/60.0;		
	}
};

/// ���������� true, ���� ������ �����������
bool parallel(const line_t& in_line1, const line_t& in_line2);

/** ���������� ����� ����������� ���� ������*/
point_t calc_cross_point(const line_t& in_line1, const line_t& in_line2);

/** ���������� ������ ����� ������� �������*/
inline point_t bezier1(const point_t& in_p0, const point_t& in_p1, double in_t)
{
	return point_t((1-in_t)*in_p0.x + in_t*in_p1.x, (1-in_t)*in_p0.y + in_t*in_p1.y);
}

/** ���������� ������ ����� ������� �������*/
inline point_t bezier2(const point_t& in_p0, const point_t& in_p1, const point_t& in_p2, double in_t)
{
	return bezier1(bezier1(in_p0,in_p1,in_t), bezier1(in_p1,in_p2,in_t), in_t);
}

/** ���������� �������������*/
line_t calc_perpendicular(const line_t& in_line, const point_t& in_point);

/** ����������� ��������� �������� � ��������*/
float rich_to_curv(double in_rich, double in_front_arm=17.4, double in_back_arm=4.1);

/** ����������� ��������� �������� � ��������*/
float curv_to_rich(double in_curv, double in_front_arm=17.4, double in_back_arm=4.1);

/** ���������� �������� �� �������*/
float radius_to_curv(float in_radius);

/** ���������� ������� �� ��������*/
float curv_to_radius(float in_curv);

/** ���������� ������������� ���������*/
float calc_anp(float in_speed, float in_curv, float in_level);

/** ���������� ���������� �������� �� ������������� ��������*/
float calc_vanp(float in_curv, float in_level, float anp_range);

/** ���������� �������� ���������� ������������� ���������*/
float calc_psi(double in_speed, double in_anp_start, double in_anp_end, double in_coord_start, double in_coord_end);

/** ���������� �������� ���������� ������������� ���������*/
float calc_psi(double in_speed, double in_curv_start, double in_curv_end, double in_level_start, double in_level_end, double in_coord_start, double in_coord_end);

/** ����� ����-������*/
float calc_tap(double in_value_start, double in_value_end, double in_coord_start, double in_coord_end);

/** ���������� ���� � ����� */
float fget_sssp(float &sgm);

/** ���������� ����� � ����� */
float fget_sigma(float &sko_pit_l, float &sko_pit_r, float &sko_lvl_diff, float &sko_lvl, int &i_pit_l, int &i_pit_r, int &i_lvl_diff, int &i_lvl);

/// ������ ���� ��������: ���������� � ������� � ���������� true, ���� ��� ������
template<class T> class change_calculator
{
private:
	/// ��������
	std::deque<T> data;
public:
	/// �����������
	change_calculator() {};
	/// �������� ��������
	bool put(const T& in_t) 
	{
		data.push_back(in_t);
		while (data.size()>2)
			data.pop_front();
		return get();
	};
	/// ���������� true ���� �������� ����������
	bool get() const
	{
		if (data.size()==2)
			return data.front()!=data.back();
		else
			return false;
	}
	/// ���������� ���������� ��������
	const T& prev() const
	{
		return data.front();
	}
	/// ���������� ������� ��������
	const T& curr() const
	{
		return data.back();
	}
};

/// ������ ���� ��������: ���������� � ������� � ���������� �� ��������
template<class T> class diff_calculator
{
private:
	/// �������� �� ���������
	T default_value;
	/// ��������
	std::deque<T> data;
public:
	/// �����������
	diff_calculator(T in_default_value) : default_value(in_default_value) {};
	/// �������� ��������
	T put(const T& in_t) 
	{
		data.push_back(in_t);
		while (data.size()>2)
			data.pop_front();
		return get();	
	};
	/// ���������� �������� ��������
	T get() const
	{
		if (data.size()==2)
			return data.front()-data.back();
		else
			return default_value;
	};
	///
	T front() const
	{
		return data.front();
	};
	///
	T back() const
	{
		return data.back();
	};
};

/// ��������� ������������ ��������
template<class T> class max_calculator
{
private:
	/// ������� ��������������������
	bool initialized;
	/// ����������� ��������
	T max_value;
public:
	max_calculator(T default_value) : initialized(false), max_value(default_value) {}
	T put(const T& in_value)
	{
		if (initialized)
		{
			max_value=std::max<T>(max_value, in_value);
		}
		else
		{
			max_value=in_value;
			initialized=true;
		}
		return max_value;
	};
	T get() const
	{
		return max_value;
	};
	void reset()
	{
		initialized=false;
	};
};

/// ��������� ����������� ��������
template<class T> class min_calculator
{
private:
	/// ������� ��������������������
	bool initialized;
	/// ����������� ��������
	T min_value;
public:
	min_calculator(T default_value) : initialized(false), min_value(default_value) {}
	T put(const T& in_value)
	{
		if (initialized)
		{
			min_value=std::min<T>(min_value, in_value);
		}
		else
		{
			min_value=in_value;
			initialized=true;
		}
		return min_value;
	};
	T get() const
	{
		return min_value;
	};
	void reset()
	{
		initialized=false;
	};
};

/// ��������� ����
template<class T> class mode_calculator
{
private:
	std::map<T, size_t> data;	
	struct compare_first
	{
		bool operator()(const std::pair<T, size_t>& l, const std::pair<T, size_t>& r)
		{
			return l.first < r.first;
		}
	};
	struct compare_second
	{
		bool operator()(const std::pair<T, size_t>& l, const std::pair<T, size_t>& r)
		{
			return l.second < r.second;
		}
	};
public:	
	mode_calculator() {};
	void put(const T& value)
	{
		std::map<T, size_t>::iterator i=data.find(value);
		if (i!=data.end())
		{
			i->second++;
		}
		else
		{
			data.insert(std::pair<T, size_t>(value,1));			
		}
	};
	T get() const
	{		
		T result=std::max_element(data.begin(), data.end(), compare_second())->first;
		return result;
	};
	size_t count() const
	{		
		size_t result=std::max_element(data.begin(), data.end(), compare_second())->second;
		return result;
	};
	void pop()
	{
		data.erase(std::max_element(data.begin(), data.end(), compare_second()));
	};
	void reset()
	{
		data.clear();
	};
	bool empty() const
	{
		return data.empty();
	};
};

/// ��������� ����������� �� ���������
struct density_t
{
	/// ��������
	pro::interval<double> range;
	/// ��������
	double value;	
	/// �����������
	density_t(const pro::interval<double>& in_range) : range(in_range), value(0){};
	density_t(const density_t & _dens) : range(_dens.range), value(_dens.value){};
	/// �������� <
	bool operator<(const density_t& in_density) const {return range.left_bound().value<in_density.range.left_bound().value;};
};

/// ���������� ��������� �������������
class density_calculator
{
private:
	double quantum;	
	std::set<density_t> data;
public:	
	/// �����������
	density_calculator(double in_quantum)
	{
		quantum=in_quantum;		
	};	
	/// ���� ������
	void put(double in_value, double in_weight=1.0)
	{		
		/*data.insert(density_t(pro::interval<double>(round_to(in_value, quantum), 
			round_to(in_value, quantum)+quantum, true, false))).first->value+=in_weight;*/
	};
	/// ����� ������
	const std::set<density_t>& get() const
	{
		return data;
	}
};


// ���������� ��� GetMaxAbs|GetMinAbs ����������� ����� std::max_element|min_element
// ������������ � VCurve � VParameter
inline bool abs_comparator(const double & value1, const double & value2)
{
	return fabs(value1) < fabs(value2);
}

//------------------------- ��������� (� ��������� ����?) ------------------


//������ ��� ��������� ��������
template<class T> class less_then
{
private:
	T value;
public:
	less_then(T in_value) : value(in_value){};
	bool operator()(T in_value) const {return in_value<value;};
};

//������ ��� ��������� ��������
template<class T> class greater_then
{
private:
	T value;
public:
	greater_then(T in_value) : value(in_value){};
	bool operator()(T in_value) const {return in_value>value;};
};

//�� ������ ��� ��������� ��������
template<class T> class not_less_then
{
private:
	T value;
public:
	not_less_then(T in_value) : value(in_value){};
	bool operator()(T in_value) const {return !(in_value<value);};
};

//�� ������ ��� ��������� ��������
template<class T> class not_greater_then
{
private:
	T value;
public:
	not_greater_then(T in_value) : value(in_value){};
	bool operator()(T in_value) const {return in_value>value;};
};


template<class T> bool is_odd(const T& in_value)
{
	return fmod(float(in_value), float(2.0))!=0;
}

template<class T> bool is_even(const T& in_value)
{
	return fmod(float(in_value), float(2.0))==0;
}

/// ����� ���������� ���������
class lsq_calculator
{
private:
	double SumDelta;
	double SumDelta2;
	double SumCoord;
	double SumCoord2;
	double SumCoordDelta;
	std::deque<point_t> data;
public:
	/// �����������
	lsq_calculator();
	void put(const point_t& in_point);
	void pop();
	line_t get() const;
	bool valid() const;
	void reset();
	size_t size() const;
};

/// Median-median line
class mml_calculator
{
private:
	/// ������
	std::deque<point_t> data;
	/// ����� ����� ����� (����� ����� 2 ��� 3 )
	static const int groups_count=3;
	/// ����������� ���� �����
	static const size_t min_data_size=9;	
public:
	/// �����������
	mml_calculator();
	/// �������� ������ �� ����
	void put(const point_t& in_point);	
	/// ���������� ��������� ������ �� ������� ������
	line_t get() const;
	/// ���������� true, ���� ������� ������ ���������� ��� ���������� ������
	bool valid() const;
	/// ����� (�������� ������)
	void reset();
	/// ���������� ���������� ����� ������
	size_t size() const;
};

/// ����� ����������� ����������� ���������� ������ �������
class fechner
{
private:
	double sum1;
	double sum2;
	double count;
	double C;
	double H;
public:
	fechner()
	{
		reset();
	}
	void reset()
	{
		sum1=0;
		sum2=0;
		count=0;
		C=0;
		H=0;
	}
	void put(double in_value1, double in_value2)
	{
		sum1+=in_value1;
		sum2+=in_value2;
		count+=1;
		double average1=sum1/count;
		double average2=sum2/count;
		((in_value1-average1)*(in_value2-average2) < 0) ? H+=1 : C+=1;
	}
	double get() const
	{
		return (C-H)/(C+H);
	};
};


/// ����� ����������� ������� �� ��������� ���������� ���������
class mean_calculator
{
private:
	std::deque<double> data;
	double cached_sum;

public:
	mean_calculator()
	{
		reset();
	};
	void put(double in_value)
	{
		data.push_back(in_value);
		cached_sum+=data.back();
	};
	void pop()
	{
		cached_sum-=data.front();
		data.pop_front();
	};
	double get() const
	{
		if (data.size()>0)
		{
			return cached_sum/data.size();
		}
		else
		{
			return 0;
		}
	};
	size_t count() const
	{
		return data.size();
	};
	void reset()
	{
		cached_sum=0;
		data.clear();
	};
};

/// ����� ����������� ��������� �� ��������� ���������� ���������
class variance_calculator
{
private:
	size_t size;
	std::deque<double> data;
	double sum1;
	double sum2;
	void push(double in_value)
	{
		data.push_back(in_value);
		sum1+=data.back();
		sum2+=pow(data.back(), 2);
	}
	void pop()
	{
		sum1-=data.front();
		sum2-=pow(data.front(), 2);
		data.pop_front();
	}
public:
	variance_calculator(size_t in_size) : size(in_size)
	{
		reset();
	};
	void put(double in_value)
	{
		push(in_value);
		while (data.size()>size)
			pop();
	};
	double get() const
	{
		if (data.size()>0)
		{
			return sum2/data.size()-pow(sum1/data.size(),2);
		}
		else
		{
			return 0;
		}
	};
	size_t count() const
	{
		return data.size();
	};
	void reset()
	{
		sum1=0;
		sum2=0;
		data.clear();
	};
};


/// ������� ������
double gauss(double in_x, double in_mu, double in_sigma);

/// ������� sinc
double sinc(double in_x);

/// ��������������� ������� sinc
double nsinc(double in_x);

/// ���������� ���� ������� ������ - ������ ����� ��������
template <class Iterator> double correlate(Iterator it1_begin, Iterator it2_begin, size_t count)
{
	double mo1 = 0, mo2 = 0, v1, v2, d1 = 0, d2 = 0, s = 0;
	Iterator it1, it2;
	size_t i;

	for (i = 0, it1 = it1_begin, it2 = it2_begin; i < count; i++, it1++, it2++)
	{
		mo1 += *it1;
		mo2 += *it2; 
	}

	mo1 /= count;
	mo2 /= count;
	
	for (i = 0, it1 = it1_begin, it2 = it2_begin; i < count; i++, it1++, it2++)
	{
		v1 = *it1 - mo1;
		v2 = *it2 - mo2;
		d1 += v1 * v1; 
		d2 += v2 * v2;
		s += v1 * v2;
	}

	d1 /= count;
	d2 /= count;
	s /= count;

	if (d1 > 0 && d2 > 0)	return s / (sqrt(d1) * sqrt(d2));
	else					return -1;
}


/// �������� ������� �������� ������� (piecewise linear function) ������� 2
class zline
{
private:

	/// ����������� ���������
	pro::interval<double> range;
	/// ������ �������� �������
	std::deque<line_t> lines;
	/// ��� �������
	mutable size_t cached_index;
	
public:
	/// �����������
	zline();
	/// �����������
	zline(const pro::interval<double>& in_range);
	/// �����������
	zline(const std::vector<point_t>& in_points);
	/// ��������� ������
	void AddLine(const line_t in_line);
	/// ���������� ����� �����
	size_t GetPointCount() const;
	/// ������� ��� �����
	void Clear();
	/// ���������� �����
	point_t GetPoint(size_t in_index) const;
	/// ���������� �������� � �����
	double GetY(double in_x) const;
	/// ���������� �������� ��� �����
	pro::interval<double> GetRange(size_t in_index) const;
	/// ���������� ����� ������
	size_t GetLineCount() const;
	/// ���������� ��������� ������
	const line_t& GetLine(size_t in_index) const;
	/// ���������� ��������� ������
	line_t& GetLine(size_t in_index);
	/// ���������� ������ ������ �� ������� ��������� �����
	size_t FindLine(double in_x) const;
};

