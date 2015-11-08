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

/// Неопределенное значение параметра
#define UNDEFINED_VALUE 0

/** Округление по базе*/
double round_to(double in_value, double in_base);

/** Сравнивает два double*/
bool equal(double l, double r);

/** Округление до ближайшего целого*/
double round(double in_value);

/** Округление с точностью до некоторого знака после запятой*/
double round(double in_value, unsigned int in_prec);

/** Возвращает знак*/
template<class T> T sign(const T & t)
{
	return t>=0 ? static_cast<T>(1) : static_cast<T>(-1);
}

/** Точка*/
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

/** Расстояние между двумя точками*/
inline double length(const point_t& in_p1, const point_t& in_p2)
{
	return sqrt(pow(in_p1.x-in_p2.x, 2) + pow(in_p1.y-in_p2.y, 2));
};

/** Уравнение прямой вида A*x + B*y + C = 0*/
class line_t
{
public:
	/** Конструктор*/
	line_t() : _A(0), _B(0), _C(0)
	{};	

	/** Конструктор*/
	line_t(double in_A, double in_B, double in_C) : _A(in_A), _B(in_B), _C(in_C)
	{};
	
	/** Конструктор*/
	line_t(const point_t& in_p1, const point_t& in_p2);

	/** Конструктор*/
	line_t(const point_t& in_p, double in_angle);
	
	/** Конструктор прямой y=const*/
	line_t(double in_y) : _A(0), _B(-1), _C(in_y)
	{};

	//Уравненние с угловым коэффициентом
	double k() const;
	double b() const;

	//Уравнение в общем виде
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

	//Возвращает значение Y ои X
	double Y(double x) const
	{
		return k()*x+b();
	};

	//Возвращает значение X от Y
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

	//Уравнение в общем виде
	double _A, _B, _C;
};

/** Расстояние от точки до прямой*/
static double length(const point_t& in_p, const line_t& in_l)
{
	return (in_l.A()*in_p.x + in_l.B()*in_p.y + in_l.C())/sqrt(pow(in_l.A(),2)+pow(in_l.B(),2));
};

/** Возвращает уравненение прямой параллельной данной, проходящей через заданную точку*/
static line_t parallel(const point_t& in_point, const line_t& in_line)
{
	return line_t(in_line.A(), in_line.B(), -in_line.A()*in_point.x-in_line.B()*in_point.y);
};

/** Уравнение окружности (x-a)^2 + (y-b)^2 = r^2*/
class circle_t
{
public:
	/** Конструктор*/
	circle_t(double in_a, double in_b, double in_r) : _a(in_a), _b(in_b), _r(in_r)
	{};

	/** Конструктор (по трем точкам) см. http://faqs.org.ru/science/geometry_faq.htm*/
	circle_t(point_t in_p1, point_t in_p2, point_t in_p3);

	/** */
	double X(){return _a;};
	double Y(){return _b;};
	double R(){return _r;};

private:

	double _a, _b, _r;
};

/** Угол*/
class angle_t
{
private:
	/** Значение в радианах*/
	double angle;	

public:
	/** Конструктор*/
	angle_t() : angle(0)
	{};

	angle_t(double in_angle) : angle(in_angle)
	{};

	angle_t(int in_grad, int in_min, int in_sec);

	/** Возвращает радианы*/
	double get_rad() const 
	{
		return angle;
	};

	/** Возвращает знак*/
	int get_sign() const 
	{
		return angle < 0 ? -1 : 1;
	};

	/** Возвращает градусы со знаком*/
	int get_grad() const
	{
		return get_sign()*static_cast<int>(floor(fabs(180.0*angle/M_PI)));
	}

	/** Возвращает минуты без знака*/
	int get_min() const;

	/** Возвращает секунды без знака*/
	int get_sec() const;

	/** Форматирует значение угла: градусы и минуты*/
	std::wstring format_degrees() const
	{
		return boost::str(boost::wformat(L"%d°%d'") % get_grad() % get_min());
	}

	/** Форматирует значение угла в ISO 6709*/
	double get_iso_6709() const
	{		
		return 100.0*fabs(double(get_grad())) + double(get_min()) + double(get_sec())/60.0;		
	}
};

/// Возвращает true, если прямые параллельны
bool parallel(const line_t& in_line1, const line_t& in_line2);

/** Вычисление точки пересечения двух прямых*/
point_t calc_cross_point(const line_t& in_line1, const line_t& in_line2);

/** Вычисление кривой Безье первого порядка*/
inline point_t bezier1(const point_t& in_p0, const point_t& in_p1, double in_t)
{
	return point_t((1-in_t)*in_p0.x + in_t*in_p1.x, (1-in_t)*in_p0.y + in_t*in_p1.y);
}

/** Вычисление кривой Безье второго порядка*/
inline point_t bezier2(const point_t& in_p0, const point_t& in_p1, const point_t& in_p2, double in_t)
{
	return bezier1(bezier1(in_p0,in_p1,in_t), bezier1(in_p1,in_p2,in_t), in_t);
}

/** Возвращает перпендикуляр*/
line_t calc_perpendicular(const line_t& in_line, const point_t& in_point);

/** Коэффициент пересчета рихтовки в кривизну*/
float rich_to_curv(double in_rich, double in_front_arm=17.4, double in_back_arm=4.1);

/** Коэффициент пересчета кривизны в рихтовку*/
float curv_to_rich(double in_curv, double in_front_arm=17.4, double in_back_arm=4.1);

/** Вычисление кривизны по радиусу*/
float radius_to_curv(float in_radius);

/** Вычисление радиуса по кривизне*/
float curv_to_radius(float in_curv);

/** Вычисление непогашенного ускорения*/
float calc_anp(float in_speed, float in_curv, float in_level);

/** Вычисление допустимой скорости по непогашенному ускрению*/
float calc_vanp(float in_curv, float in_level, float anp_range);

/** Вычисление скорости нарастания непогашенного ускорения*/
float calc_psi(double in_speed, double in_anp_start, double in_anp_end, double in_coord_start, double in_coord_end);

/** Вычисление скорости нарастания непогашенного ускорения*/
float calc_psi(double in_speed, double in_curv_start, double in_curv_end, double in_level_start, double in_level_end, double in_coord_start, double in_coord_end);

/** Отвод чего-нибудь*/
float calc_tap(double in_value_start, double in_value_end, double in_coord_start, double in_coord_end);

/** Возвращает СССП в точке */
float fget_sssp(float &sgm);

/** Возвращает СИГМУ в точке */
float fget_sigma(float &sko_pit_l, float &sko_pit_r, float &sko_lvl_diff, float &sko_lvl, int &i_pit_l, int &i_pit_r, int &i_lvl_diff, int &i_lvl);

/// Хранит пару значений: предыдущее и текущее и возвращает true, если они разные
template<class T> class change_calculator
{
private:
	/// Значения
	std::deque<T> data;
public:
	/// Конструктор
	change_calculator() {};
	/// Помещает значение
	bool put(const T& in_t) 
	{
		data.push_back(in_t);
		while (data.size()>2)
			data.pop_front();
		return get();
	};
	/// Возвращает true если значение изменилось
	bool get() const
	{
		if (data.size()==2)
			return data.front()!=data.back();
		else
			return false;
	}
	/// Возвращает предыдущее значение
	const T& prev() const
	{
		return data.front();
	}
	/// Возвращает текущее значение
	const T& curr() const
	{
		return data.back();
	}
};

/// Хранит пару значений: предыдущее и текущее и возвращает их разность
template<class T> class diff_calculator
{
private:
	/// Значение по умолчанию
	T default_value;
	/// Значения
	std::deque<T> data;
public:
	/// Конструктор
	diff_calculator(T in_default_value) : default_value(in_default_value) {};
	/// Помещает значение
	T put(const T& in_t) 
	{
		data.push_back(in_t);
		while (data.size()>2)
			data.pop_front();
		return get();	
	};
	/// Возвращает значение разности
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

/// Вычисляет максимальное значение
template<class T> class max_calculator
{
private:
	/// Признак инициализированности
	bool initialized;
	/// Макисальное значение
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

/// Вычисляет минимальное значение
template<class T> class min_calculator
{
private:
	/// Признак инициализированности
	bool initialized;
	/// Макисальное значение
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

/// Вычисляет моду
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

/// Плотность вероятности на интервале
struct density_t
{
	/// Диапазон
	pro::interval<double> range;
	/// Значение
	double value;	
	/// Конструктор
	density_t(const pro::interval<double>& in_range) : range(in_range), value(0){};
	density_t(const density_t & _dens) : range(_dens.range), value(_dens.value){};
	/// Оператор <
	bool operator<(const density_t& in_density) const {return range.left_bound().value<in_density.range.left_bound().value;};
};

/// Вычисление плотности распределения
class density_calculator
{
private:
	double quantum;	
	std::set<density_t> data;
public:	
	/// Конструктор
	density_calculator(double in_quantum)
	{
		quantum=in_quantum;		
	};	
	/// Ввод данных
	void put(double in_value, double in_weight=1.0)
	{		
		/*data.insert(density_t(pro::interval<double>(round_to(in_value, quantum), 
			round_to(in_value, quantum)+quantum, true, false))).first->value+=in_weight;*/
	};
	/// Вывод данных
	const std::set<density_t>& get() const
	{
		return data;
	}
};


// Компаратор для GetMaxAbs|GetMinAbs вычисляемых через std::max_element|min_element
// Используется в VCurve и VParameter
inline bool abs_comparator(const double & value1, const double & value2)
{
	return fabs(value1) < fabs(value2);
}

//------------------------- Предикаты (в отдельный файл?) ------------------


//Меньше чем указанное значение
template<class T> class less_then
{
private:
	T value;
public:
	less_then(T in_value) : value(in_value){};
	bool operator()(T in_value) const {return in_value<value;};
};

//Больше чем указанное значение
template<class T> class greater_then
{
private:
	T value;
public:
	greater_then(T in_value) : value(in_value){};
	bool operator()(T in_value) const {return in_value>value;};
};

//Не меньше чем указанное значение
template<class T> class not_less_then
{
private:
	T value;
public:
	not_less_then(T in_value) : value(in_value){};
	bool operator()(T in_value) const {return !(in_value<value);};
};

//Не больше чем указанное значение
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

/// Метод наименьших квадратов
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
	/// Конструктор
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
	/// Данные
	std::deque<point_t> data;
	/// Число групп точек (имеет смысл 2 или 3 )
	static const int groups_count=3;
	/// Минимальное чило точек
	static const size_t min_data_size=9;	
public:
	/// Конструктор
	mml_calculator();
	/// Помещает данные на вход
	void put(const point_t& in_point);	
	/// Возвращает уравнение прямой по текущим данным
	line_t get() const;
	/// Возвращает true, если текущих данных достаточно для построения прямой
	bool valid() const;
	/// Сброс (удаление данных)
	void reset();
	/// Возвращает количество точек данных
	size_t size() const;
};

/// Класс вычисляющий коэффициент корреляции знаков Фехнера
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


/// Класс вычисляющий среднее на некотором скользящем интервале
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

/// Класс вычисляющий дисперсию на некотором скользящем интервале
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


/// Функция Гаусса
double gauss(double in_x, double in_mu, double in_sigma);

/// Функция sinc
double sinc(double in_x);

/// Нормализованная функция sinc
double nsinc(double in_x);

/// Корреляция двух наборов данных - доступ через итератор
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


/// Описание кусочно линейной функции (piecewise linear function) вариант 2
class zline
{
private:

	/// Ограничение диапазона
	pro::interval<double> range;
	/// Прямые задающие функцию
	std::deque<line_t> lines;
	/// Кэш индекса
	mutable size_t cached_index;
	
public:
	/// Конструктор
	zline();
	/// Конструктор
	zline(const pro::interval<double>& in_range);
	/// Конструктор
	zline(const std::vector<point_t>& in_points);
	/// Добавляет прямую
	void AddLine(const line_t in_line);
	/// Возвращает число точек
	size_t GetPointCount() const;
	/// Удаляет все точки
	void Clear();
	/// Возвращает точку
	point_t GetPoint(size_t in_index) const;
	/// Возвращает значение в точке
	double GetY(double in_x) const;
	/// Возвращает диапазон для линии
	pro::interval<double> GetRange(size_t in_index) const;
	/// Возвращает число прямых
	size_t GetLineCount() const;
	/// Возвращает уравнение прямой
	const line_t& GetLine(size_t in_index) const;
	/// Возвращает уравнение прямой
	line_t& GetLine(size_t in_index);
	/// Возвращает индекс прямой на которой находится точка
	size_t FindLine(double in_x) const;
};

