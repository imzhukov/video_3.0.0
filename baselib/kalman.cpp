#include "kalman.h"

/// �����������
VKalman::VKalman(double in_state, double in_r, double in_q)
{
	state=in_state;
	covariance=0.1;
	R=in_r;
	Q=in_q;
}

/// �������� ������ �� ����
void VKalman::Put(const double& in_data)
{
	// ��������� �������� �������
	double F=1.0;
	// ����������� �����������
	double B=0.0;
	
	double H=1.0;
		
	double X0 = F*state;
	double P0 = F*covariance*F + Q;
	//�������� �������
	double K = H*P0/(H*P0*H + R);
	state = X0 + K*(in_data - H*X0);
	covariance = (1 - K*H)*P0;
	
	output.push_back(state);
}
/// ���������� true, ���� ���� ������������ ������
bool VKalman::Ready() const
{
	return output.size()>0;
}
/// ���������� true, ���� ������������ ������ ���
bool VKalman::Empty() const
{
	return output.empty();
}	
/// ���������� ������ ������� ������
const double& VKalman::Front() const
{
	return output.front();
}
/// ����������� ������ ������� ������
void VKalman::Pop()
{
	output.pop_front();
}