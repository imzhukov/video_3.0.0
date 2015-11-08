#pragma once

#include "processing.h"

class VKalman : public VDataProcessor<double, double>
{
private:
	/// Очередь данных на входе
	std::deque<double> input; 
	/// Очередь данных на выходе
	std::deque<double> output;
	//Ошибка измерения
	double R;
	//Шум процесса
	double Q;
	/// Состояние
	double state;
	/// Ковариация
	double covariance;
public:
	/// Конструктор
	VKalman(double in_state, double in_r, double in_q);
	/// Помещает данные на вход
	void Put(const double& in_data);
	/// Возвращает true, если есть обработанные данные
	bool Ready() const;	
	/// Возвращает true, если обработанных данных нет
	bool Empty() const;		
	/// Возвращает первый элемент данных
	const double& Front() const;
	/// Выталкивает первый элемент данных
	void Pop();
};