#pragma once

#include <cfloat>
#include <climits>
#include <ostream>

namespace pro
{

/// Граница интервала
template<typename T> class bound
{
private:
public:
	/// Значение границы
	T value;
	/// Признак закрытой границы
	bool closed;
	/// Минимальное значение границы
	static const T min_value;
	/// Максимальное значение границы
	static const T max_value;
	/// Значение по умолчанию
	static const T default_value;
	
	/// Конструктор
	bound() : value(default_value), closed(false) {};
	/// Конструктор
	bound(const T& in_value) : value(in_value), closed(true) {};
	/// Конструктор
	bound(const T& in_value, bool in_closed) : value(in_value), closed(in_closed) {};
	
	/// Сдвигает границу вправо (вверх)
	bound<T>& operator+=(const T& in_value)
	{
		((max_value-in_value)>=value) ? value+=in_value : max_value;
		return *this;
	};
	/// Сдвигает границу влево (вниз)
	bound<T>& operator-=(const T& in_value)
	{
		((min_value+in_value)<=value) ? value-=in_value : min_value;
		return *this;
	};
	/// Масштабирует границу
	bound<T>& operator*=(const T& in_value)
	{
		value*=in_value;
		return *this;
	};
	/// Оператор сравнения
	bool operator==(const bound<T>& in_r) const
	{
		return value==in_r.value && closed==in_r.closed;
	};
};

/// Специализации констант для встроенных типов FixMe!!! дополнить 
template<> const float bound<float>::min_value=-FLT_MAX;
template<> const float bound<float>::max_value=FLT_MAX;
template<> const float bound<float>::default_value=0;
template<> const double bound<double>::min_value=-DBL_MAX;
template<> const double bound<double>::max_value=DBL_MAX;
template<> const double bound<double>::default_value=0;
template<> const long double bound<long double>::min_value=-LDBL_MAX;
template<> const long double bound<long double>::max_value=LDBL_MAX;
template<> const long double bound<long double>::default_value=0;

template<> const short bound<short>::min_value=SHRT_MIN;
template<> const short bound<short>::max_value=SHRT_MAX;
template<> const short bound<short>::default_value=0;
template<> const int bound<int>::min_value=INT_MIN;
template<> const int bound<int>::max_value=INT_MAX;
template<> const int bound<int>::default_value=0;
template<> const long bound<long>::min_value=LONG_MIN;
template<> const long bound<long>::max_value=LONG_MAX;
template<> const long bound<long>::default_value=0;

/// Сдвигает границу вправо (вверх)
template<typename T> bound<T> operator+(const bound<T>& in_bound, const T& in_value)
{
	bound<T> bound=in_bound;
	bound+=in_value;
	return bound;
};
/// Сдвигает границу влево (вниз)
template<typename T> bound<T> operator-(const bound<T>& in_bound, const T& in_value)
{
	bound<T> bound=in_bound;
	bound-=in_value;
	return bound;
};
/// Масштабирует границу
template<typename T> bound<T> operator*(const bound<T>& in_bound, const T& in_value)
{
	bound<T> bound=in_bound;
	bound*=in_value;
	return bound;
};

/// Описание диапазона
template<typename T> class interval
{
private:
	///Левая (нижняя) граница диапазона
	bound<T> left;
	///Правая (верхняя) граница диапазона
	bound<T> right;
	/// Функтор
	struct min_bound
	{
		bool operator()(const bound<T>& l, const bound<T>& r) 
		{	
			return l.value<r.value || l.value==r.value && l.closed && !r.closed;
		};
	};
	/// Функтор
	struct max_bound
	{
		bool operator()(const bound<T>& l, const bound<T>& r) 
		{	
			return l.value<r.value || l.value==r.value && !l.closed && r.closed;
		};
	};
public:
	/// Конструктор
	interval(){};
	/// Конструктор
	interval(T in_left, T in_right) : left(in_left), right(in_right) {};
	/// Конструктор
	interval(T in_left, T in_right, bool in_closed_left, bool in_closed_right) :  left(in_left,in_closed_left), right(in_right,in_closed_right){};
	/// Конструктор
	interval(const bound<T>& in_left, const bound<T>& in_right) : left(in_left), right(in_right){};
	/// Возвращает true, если интервал содержит значение
	bool contains(T in_value) const
	{
		return (left.closed && in_value>=left.value || in_value>left.value) && (right.closed && in_value<=right.value || in_value<right.value);		   
	};
	/// Возвращает true если два интервала перекрываются
	bool intersect(interval<T> in_interval) const
	{
		return contains(in_interval.left.value) || contains(in_interval.right.value) ||
			   in_interval.contains(left.value) || in_interval.contains(right.value);
	}
	/// Возвращает левую границу
	const bound<T>& left_bound() const
	{
		return left;
	}
	/// Возвращает левую границу
	bound<T>& left_bound()
	{
		return left;
	}
	/// Возвращает правую границу
	const bound<T>& right_bound() const
	{
		return right;
	}
	/// Возвращает правую границу
	bound<T>& right_bound()
	{
		return right;
	}
	/// Оператор |= объединение интервалов
	interval<T>& operator|=(const interval<T>& in_interval)
	{
		left=(std::min<bound<T> >(left, in_interval.left, min_bound()));
		right=(std::max<bound<T> >(right, in_interval.right, max_bound()));
		return *this;
	}
	/// Оператор &= пересечение интервалов
	interval<T>& operator&=(const interval<T>& in_interval)
	{
		left=(std::max<bound<T> >(left, in_interval.left, min_bound()));
		right=(std::min<bound<T> >(right, in_interval.right, max_bound()));
		return *this;
	}
	/// Возвращает длину интервала
	T length() const
	{
		return right.value-left.value;
	}
	/// Возвращает true, если интервал закрытый
	bool closed() const
	{
		return left.closed && right.closed;
	};
	/// Возвращает true, если интервал пустой
	bool empty() const
	{
		return (left.value>right.value) || (left.value==right.value) && !closed();
	};
	/// Возвращает true, если интервал бесконечный
	bool infinite() const
	{
		return left.value==bound<T>::min_value && right.value==bound<T>::max_value;
	};
	/// Сдвигает обе границы интервала вправо (вверх)
	interval<T>& operator+=(const T& in_value)
	{
		left+=in_value;
		right+=in_value;
		return *this;
	}
	/// Сдвигает обе границы интервала влево (вниз)
	interval<T>& operator-=(const T& in_value)
	{
		left-=in_value;
		right-=in_value;
		return *this;
	}
	/// Масштабирует границы интервала
	interval<T>& operator*=(const T& in_value)
	{
		left*=in_value;
		right*=in_value;
		return *this;
	}
	/// Выводит интервал в поток
	friend std::ostream& operator<<(std::ostream& in_out, interval<T>& in_interval)
	{
		in_out << (in_interval.left.closed ? '[' : '(') << in_interval.left.value << ',' << in_interval.right.value << (in_interval.right.closed ? ']' : ')');
		return in_out;
	}
	/// Оператор ==
	bool operator==(const interval<T>& in_r) const
	{
		return left==in_r.left && right==in_r.right;
	};
};
/// Сдвигает обе границы интервала вправо (вверх)
template<typename T> interval<T> operator+(const interval<T>& in_interval, const T& in_value)
{
	interval<T> interval=in_interval;
	interval+=in_value;
	return interval;
};
/// Сдвигает обе границы интервала влево (вниз)
template<typename T> interval<T> operator-(const interval<T>& in_interval, const T& in_value)
{
	interval<T> interval=in_interval;
	interval-=in_value;
	return interval;
};
/// Масштабирует границы интервала
template<typename T> interval<T> operator*(const interval<T>& in_interval, const T& in_value)
{
	interval<T> interval=in_interval;
	interval*=in_value;
	return interval;
};
/// Объединение интервалов
template<typename T> interval<T> operator|(const interval<T>& in_left, const interval<T>& in_right)
{
	interval<T> interval=in_left;
	interval|=in_right;
	return interval;
};

/// Пересечение интервалов
template<typename T> interval<T> operator&(const interval<T>& in_left, const interval<T>& in_right)
{
	interval<T> interval=in_left;
	interval&=in_right;
	return interval;
};

}//end of namespace pro

/// Возвращает пустой интервал
template<typename T> pro::interval<T> EMPTY_INTERVAL()
{
	return pro::interval<T>(pro::bound<T>::default_value, pro::bound<T>::default_value, false, false);
}

/// Возвращает "бесконечный" интервал (все доступные значения типа)
template<typename T> pro::interval<T> INFINITE_INTERVAL()
{
	return pro::interval<T>(pro::bound<T>::min_value, pro::bound<T>::max_value, true, true);
}
