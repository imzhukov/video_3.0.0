#include "mathlib.h"

#include "os.h"
#include "vexception.h"
#include "VLog.h"

//boost
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/mean.hpp>

using namespace boost::accumulators ;

/** Округление по базе*/
double round_to(double in_value, double in_base)
{	
    return round(in_value/in_base)*in_base;
}

/** Сравнивает два double*/
bool equal(double l, double r)
{
	return fabs(l-r) <= 16.0*DBL_EPSILON*std::max<double>(fabs(l), fabs(r));
}

/** Округление до ближайшего целого*/
double round(double in_value)
{	
	return floor(in_value+0.5);
}

/** Округление с точностью до некоторого знака после запятой*/
double round(double in_value, unsigned int in_prec)
{
	return round(in_value*pow(10.0, double(in_prec)))/pow(10.0, double(in_prec));
}

//--------------------------------------------- Методы класса line_t --------------------------------------------------------------

/** Конструктор*/
line_t::line_t(const point_t& in_p1, const point_t& in_p2)
{
	_A=in_p1.y-in_p2.y;
	_B=in_p2.x-in_p1.x;
	_C=in_p1.x*in_p2.y-in_p2.x*in_p1.y;		
};

/** Конструктор*/
line_t::line_t(const point_t& in_p, double in_angle)
{
	_A=tan(M_PI/2-in_angle);
	_B=-1.0;
	_C=in_p.y-_A*in_p.x;
}

//Уравненние с угловым коэффициентом
double line_t::k() const 
{
	if (_B!=0)
		return -_A/_B;
	else
		throw VSimpleException(L"Прямая вида x=const", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
};

double line_t::b() const 
{
	if (_B!=0)
		return -_C/_B;
	else
		throw VSimpleException(L"Прямая вида x=const", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
};

/** Конструктор (по трем точкам) см. http://faqs.org.ru/science/geometry_faq.htm*/
circle_t::circle_t(point_t in_p1, point_t in_p2, point_t in_p3)
{
	double A=in_p2.x-in_p1.x;
	double B=in_p2.y-in_p1.y;
	double C=in_p3.x-in_p1.x;
	double D=in_p3.y-in_p1.y;
	double E = A*(in_p1.x + in_p2.x) + B*(in_p1.y + in_p2.y);
	double F = C*(in_p1.x + in_p3.x) + D*(in_p1.y + in_p3.y);
	double G = 2.0*(A*(in_p3.y - in_p2.y)-B*(in_p3.x - in_p2.x));
	if (G==0)
		throw VSimpleException(L"Точки лежат на одной прямой", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	_a = (D*E - B*F) / G;
	_b = (A*F - C*E) / G;
	_r = sqrt( pow(in_p1.x - _a,2) + pow(in_p1.y - _b, 2));
};

/** Конструктор*/
angle_t::angle_t(int in_grad, int in_min, int in_sec)
{
	double angle_grad=static_cast<double>(in_grad)+
		static_cast<double>(in_min + static_cast<double>(in_sec) / 60.0) / 60.0;
	angle = M_PI*angle_grad/180.0;
};

/** Возвращает минуты без знака*/
int angle_t::get_min() const
{
	double angle_grad=fabs(180.0*angle/M_PI);
	double angle_min=60*(angle_grad-floor(angle_grad));
	return static_cast<int>(floor(angle_min));
}

/** Возвращает секунды без знака*/
int angle_t::get_sec() const
{
	double angle_grad=fabs(180.0*angle/M_PI);
	double angle_min=60*(angle_grad-floor(angle_grad));
	double angle_sec=60*(angle_min-floor(angle_min));
	return static_cast<int>(floor(angle_sec));
}

/// Возвращает true, если прямые параллельны
bool parallel(const line_t& in_line1, const line_t& in_line2)
{
	return in_line1.A()*in_line2.B() == in_line2.A()*in_line1.B();
}

/** Вычисление точки пересечения двух прямых*/
point_t calc_cross_point(const line_t& in_line1, const line_t& in_line2)
{
	point_t result;
	
	if (parallel(in_line1, in_line2))
		throw VSimpleException(L"Прямые параллельны или совпадают", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__); 
	result.x=-(in_line1.C()*in_line2.B()-in_line2.C()*in_line1.B())/
		(in_line1.A()*in_line2.B()-in_line2.A()*in_line1.B());
	result.y=-(in_line1.A()*in_line2.C()-in_line2.A()*in_line1.C())/
		(in_line1.A()*in_line2.B()-in_line2.A()*in_line1.B());
	
	return result;
}

/** Возвращает перпендикуляр*/
line_t calc_perpendicular(const line_t& in_line, const point_t& in_point)
{
	double k2=-1.0/in_line.k();
	double b2=in_point.y-k2*in_point.x;
	line_t result(k2, -1.0, b2);
	return result;
}


/** Коэффициент пересчета рихтовки в кривизну*/
float rich_to_curv(double in_rich, double in_front_arm, double in_back_arm)
{
	return in_rich*(100.0/(in_front_arm*in_back_arm));
}

/** Коэффициент пересчета кривизны в рихтовку*/
float curv_to_rich(double in_curv, double in_front_arm, double in_back_arm)
{
	return in_curv/(100.0/(in_front_arm*in_back_arm));
}

/** Вычисление кривизны по радиусу*/
float radius_to_curv(float in_radius)
{
	return (in_radius!=0) ? float(50000.0)/in_radius : FLT_MAX;
}

/** Вычисление радиуса по кривизне*/
float curv_to_radius(float in_curv)
{	
	return (in_curv!=0) ? float(50000.0)/in_curv : FLT_MAX;
}



/** Вычисление непогашенного ускорения*/
float calc_anp(float in_speed, float in_curv, float in_level)
{
	return pow(in_speed,float(2.0))*in_curv/float(50000.0*12.96) - float(0.0061)*in_level;
}

/** Вычисление допустимой скорости по непогашенному ускрению*/
float calc_vanp(float in_curv, float in_level, float anp_range)
{
	return sqrt(fabs((anp_range+0.0061*in_level)*float(50000.0*12.96)/in_curv));
}

/** Вычисление скорости нарастания непогашенного ускорения*/
float calc_psi(double in_speed, double in_anp_start, double in_anp_end, double in_coord_start, double in_coord_end)
{
	if (in_coord_end==in_coord_start)
		throw DIVISION_BY_ZERO;
	return fabs(in_speed*(in_anp_end-in_anp_start)/(3.6*(in_coord_end-in_coord_start)));
}

/** Вычисление скорости нарастания непогашенного ускорения*/
float calc_psi(double in_speed, double in_curv_start, double in_curv_end, double in_level_start, double in_level_end, double in_coord_start, double in_coord_end)
{
    return calc_psi(in_speed, calc_anp(in_speed, in_curv_start, in_level_start), calc_anp(in_speed, in_curv_end, in_level_end), in_coord_start, in_coord_end);
}

/** Отвод чего-нибудь*/
float calc_tap(double in_value_start, double in_value_end, double in_coord_start, double in_coord_end)
{
   	if (in_coord_end == in_coord_start)
		throw DIVISION_BY_ZERO;
	return fabs((in_value_start - in_value_end) / (in_coord_start - in_coord_end));
}

/** Возвращает СССП в точке */
float fget_sssp(float &sgm)
{
    // Таблицы для расчета СССП
	float sigma[] = { 0.00f,0.33f,0.40f,0.44f,0.51f,0.59f,0.68f,0.80f,1.00f,1.28f,1.55f,1.85f,2.25f,2.75f,3.05f };
	float sssp[]  = {   300,  300,  250,  220,  200,  180,  160,  140,  120,  100,  80,   60,   40,   15,  0 };

	if (sgm < sigma[0]) return sssp[0];
	else if (sgm > sigma[13]) return sssp[13];
	else
	{
		for (size_t j = 0; j < 13; ++j)
		{
			if (sgm >= sigma[j] && sgm < sigma[j+1])
				return sssp[j+1] + (sigma[j+1] - sgm)*(sssp[j] - sssp[j+1])/(sigma[j+1] - sigma[j]);
		}
	}
	// that doesn't have to be
	return 0;

}

/** Возвращает СИГМУ в точке */
float fget_sigma(float &sko_pit_l, float &sko_pit_r, float &sko_lvl_diff, float &sko_lvl, int &i_pit_l, int &i_pit_r, int &i_lvl_diff, int &i_lvl)
{
    if ((i_pit_l!=0)||(i_pit_r!=0)||(i_lvl_diff!=0)||(i_lvl!=0))
	return	(i_lvl_diff*sko_lvl_diff/2.0f+i_lvl*sko_lvl/3.0f+
            i_pit_l*sko_pit_l/2.1f+i_pit_r*sko_pit_r/2.1f)
            /(i_pit_l+i_pit_r+i_lvl_diff+i_lvl);

    return -1;
}

//-------------------------------------- Методы класса lsq_calculator ----------------------------------------------

///Конструктор
lsq_calculator::lsq_calculator() : SumDelta(0), SumDelta2(0), SumCoord(0), SumCoord2(0), SumCoordDelta(0) {};

/// Помещает данные на вход
void lsq_calculator::put(const point_t& in_point)
{
	data.push_back(in_point);
	
	SumDelta+=data.back().y;
	SumDelta2+=pow(data.back().y,2);
	SumCoord+=data.back().x;
	SumCoord2+=pow(data.back().x,2);
	SumCoordDelta+=data.back().y*data.back().x;
}
void lsq_calculator::pop()
{
	SumDelta-=data.front().y;
	SumDelta2-=pow(data.front().y,2);
	SumCoord-=data.front().x;
	SumCoord2-=pow(data.front().x,2);
	SumCoordDelta-=data.front().y*data.front().x;
	
	data.pop_front();
}
line_t lsq_calculator::get() const
{
	return line_t(double(data.size())*SumCoordDelta-SumDelta*SumCoord, 
		-(double(data.size())*SumCoord2-pow(SumCoord,2)), SumDelta*SumCoord2-SumCoordDelta*SumCoord);			
}
bool lsq_calculator::valid() const
{
	return data.size()>1;
}
void lsq_calculator::reset()
{
	SumDelta=0;
	SumDelta2=0;
	SumCoord=0;
	SumCoord2=0;
	SumCoordDelta=0;
	
	data.clear();
}

size_t lsq_calculator::size() const
{
	return data.size();
}

//--------------------------------- Методы класса mml_calculator -------------------------------
/// Конструктор
mml_calculator::mml_calculator(){}

/// Помещает данные на вход
void mml_calculator::put(const point_t& in_point)
{
	data.push_back(in_point);	
}

/// Возвращает уравнение прямой по текущим данным
line_t mml_calculator::get() const
{
	
	
	accumulator_set< double, stats<tag::median, tag::count> > left_x;
	accumulator_set< double, stats<tag::median, tag::count> > left_y;
	accumulator_set< double, stats<tag::median, tag::count> > right_x;
	accumulator_set< double, stats<tag::median, tag::count> > right_y;
	
	int count=0;
	while (int(data.size())-count*groups_count>1)
	{
		int left_index=count;
		int right_index=int(data.size())-count-1;
		left_x(data.at(left_index).x);
		left_y(data.at(left_index).y);
		right_x(data.at(right_index).x);
		right_y(data.at(right_index).y);
		count++;
	}
	point_t left_point(median(left_x), median(left_y));
	point_t right_point(median(right_x), median(right_y));
	return line_t(left_point, right_point);
}

/// Возвращает true, если текущих данных достаточно для построения прямой
bool mml_calculator::valid() const
{
	return data.size()>=min_data_size;
}

/// Сброс (удаление данных)
void mml_calculator::reset()
{
	data.clear();
}

/// Возвращает количество точек данных
size_t mml_calculator::size() const
{
	return data.size();
}

/// Функция Гаусса
double gauss(double in_x, double in_mu, double in_sigma)
{
	return exp(-pow(in_x-in_mu,2)/(2.0*pow(in_sigma,2))) / (in_sigma*sqrt(2.0*M_PI));
};

/// Функция sinc
double sinc(double in_x)
{
	return in_x==0 ? 1.0 : sin(in_x)/(in_x);
};

/// Нормализованная функция sinc
double nsinc(double in_x)
{
	return sinc(in_x*M_PI);
};

//---------------------------------------------- Методы класса zline ---------------------------------------------

/// Конструктор
zline::zline() : range(INFINITE_INTERVAL<double>()), cached_index(0) {};

/// Конструктор
zline::zline(const pro::interval<double>& in_range) : range(in_range) {};

/// Конструктор
zline::zline(const std::vector<point_t>& in_points)
{
	if (in_points.size()>1)
	{
		for (size_t i=1; i<in_points.size(); ++i)
		{
			AddLine(line_t(in_points.at(i-1),in_points.at(i)));
		}
		range=pro::interval<double>(std::min<double>(in_points.front().x, in_points.back().x), std::max<double>(in_points.front().x, in_points.back().x));
	}
	else
	{
		throw VSimpleException(L"Требуется не менее 2-х точек", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

/// Добавляет прямую
void zline::AddLine(const line_t in_line)
{	
	switch (lines.size())
	{
		case 0:
		{
			lines.push_back(in_line);
			break;
		}
		case 1:
		case 2:
		{
			if (!parallel(lines.back(),in_line))
				lines.push_back(in_line);
			break;					
		}
		default:
		{
			if (!parallel(lines.back(),in_line) && 
				calc_cross_point(lines.back(), in_line).x > calc_cross_point(lines.at(lines.size()-1), lines.at(lines.size()-2)).x)
				lines.push_back(in_line);
		}
	}
}

/// Возвращает число точек
size_t zline::GetPointCount() const
{
	return lines.size()>0 ? lines.size()+1 : 0;
}

/// Удаляет все точки
void zline::Clear()
{
	lines.clear();
}

/// Возвращает точку
point_t zline::GetPoint(size_t in_index) const
{
	if (in_index==0)
		return point_t(range.left_bound().value, lines.front().Y(range.left_bound().value));
	else if (in_index==GetPointCount()-1)
		return point_t(range.right_bound().value,lines.back().Y(range.right_bound().value));
	else
		return calc_cross_point(lines.at(in_index-1), lines.at(in_index));
}

/// Возвращает значение в точке
double zline::GetY(double in_x) const
{	
	return GetLine(FindLine(in_x)).Y(in_x);	
}

/// Возвращает диапазон для линии
pro::interval<double> zline::GetRange(size_t in_index) const
{
	switch (lines.size())
	{
		case 0:
			return EMPTY_INTERVAL<double>();
		case 1:
			return range;
		default:
		{
			return pro::interval<double>(GetPoint(in_index).x, GetPoint(in_index+1).x);
		}			
	}
}

/// Возвращает число прямых
size_t zline::GetLineCount() const
{
	return lines.size();
}

/// Возвращает уравнение прямой
const line_t& zline::GetLine(size_t in_index) const
{
	return lines.at(in_index);
}

/// Возвращает уравнение прямой
line_t& zline::GetLine(size_t in_index)
{
	return lines.at(in_index);
}

/// Возвращает индекс прямой на которой находится точка
size_t zline::FindLine(double in_x) const
{
	if (cached_index<lines.size() && GetRange(cached_index).contains(in_x))
		return cached_index;
	else
	{
		for (size_t i=0;i<lines.size();++i)
		{
			if (GetRange(i).contains(in_x))
			{
				cached_index=i;
				return i;
			}
		}	
	}
	return 0;
}