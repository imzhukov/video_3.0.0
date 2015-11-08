#pragma once

#include "processing.h"

class VKalman : public VDataProcessor<double, double>
{
private:
	/// ������� ������ �� �����
	std::deque<double> input; 
	/// ������� ������ �� ������
	std::deque<double> output;
	//������ ���������
	double R;
	//��� ��������
	double Q;
	/// ���������
	double state;
	/// ����������
	double covariance;
public:
	/// �����������
	VKalman(double in_state, double in_r, double in_q);
	/// �������� ������ �� ����
	void Put(const double& in_data);
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const;		
	/// ���������� ������ ������� ������
	const double& Front() const;
	/// ����������� ������ ������� ������
	void Pop();
};