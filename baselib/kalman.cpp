#include "kalman.h"

/// Конструктор
VKalman::VKalman(double in_state, double in_r, double in_q)
{
	state=in_state;
	covariance=0.1;
	R=in_r;
	Q=in_q;
}

/// Помещает данные на вход
void VKalman::Put(const double& in_data)
{
	// Описывает динамику системы
	double F=1.0;
	// Управляющее воздействие
	double B=0.0;
	
	double H=1.0;
		
	double X0 = F*state;
	double P0 = F*covariance*F + Q;
	//Усиление Калмана
	double K = H*P0/(H*P0*H + R);
	state = X0 + K*(in_data - H*X0);
	covariance = (1 - K*H)*P0;
	
	output.push_back(state);
}
/// Возвращает true, если есть обработанные данные
bool VKalman::Ready() const
{
	return output.size()>0;
}
/// Возвращает true, если обработанных данных нет
bool VKalman::Empty() const
{
	return output.empty();
}	
/// Возвращает первый элемент данных
const double& VKalman::Front() const
{
	return output.front();
}
/// Выталкивает первый элемент данных
void VKalman::Pop()
{
	output.pop_front();
}