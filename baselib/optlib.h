#pragma once

#include "mathlib.h"
#include "interval.h"

class VFunction
{
public:	
	/// <summary>
	/// �����, ��������������� �������� ��������� � ������� index ������ value
	/// </summary>
	virtual void SetParam(size_t index, double value)=0;
	
	/// <summary>
	/// �����, ������������ �������� ��������� � ������� index
	/// </summary>
	virtual double GetParam(size_t index)=0;
	
	/// <summary>
	/// �����, ��������������� �������� ������� ��������� � ������� index ������ value
	/// </summary>
	virtual void SetParamRange(size_t index, const pro::interval<double>& value)=0;
	
	/// <summary>
	/// �����, ������������ ������� ��������� � ������� index
	/// </summary>
	virtual pro::interval<double> GetParamRange(size_t index)=0;

	/// <summary>
	/// �����, �������������� �������� ������� �������
	/// </summary>
	virtual void Update()=0;

	/// <summary>
	/// �����, ������������ ������� �������� �������
	/// </summary>
	virtual double Value()=0;
	
	/// <summary>
	/// �����, ������������ ���������� ����������
	/// </summary>
	virtual size_t GetParamCount()=0;
};

class VMethod
{
public:
	/// <summary>
	/// �����, ��������������� �������������� �������
	/// </summary>
	void SetFunction(VFunction* function) {Function=function;}
	
	/// <summary>
	/// ���������� ������ �����������
	/// </summary>
	virtual void RunMethod()=0;
protected:
	VFunction* Function;
};


class VFastDescentMethod : public VMethod
{
public:
	
	/// <summary>
	/// ����������� ������ VFastDescentMethod
	/// </summary>
	/// <param name="function">
	/// �������, ������ ������-���� ������, ������������ �� VFunction
	/// </param>
	/// <param name="minAlfa">
	/// ����������� �������� ��������� alfa ������, ������������� �����, ���������
	/// </param>
	/// </param>
	/// <param name="maxAlfa">
	/// ������������ �������� ��������� alfa ������, ������������� �����, ���������
	/// </param>
	/// <param name="step">
	/// �������� (���) ���������� ��������� alfa
	/// </param>
	/// <param name="eps">
	/// �������� ����������
	/// </param>
	/// <param name="delta">
	/// �������� ���������� �����������
	/// </param>
	/// <param name="minOrMax">
	/// �������� ����� 1, ���� ����� �������� �������, � -1, ���� ���������
	/// </param>
	VFastDescentMethod(VFunction* function, double minAlfa, double maxAlfa, double step, double eps, double delta, int minOrMax);
	
	/// <summary>
	/// ����� ���������� ������� ������� ������������� ������
	/// </summary>
	void RunMethod();

private:
	double MinAlfa;
	double MaxAlfa;
	double Step;
	double Eps;
	double Delta;
	int MinOrMax;
};

class VGaussSeidelMethod : VMethod
{
public:
	/// <summary>
	/// ����������� ������ VGaussSeidelMethod
	/// </summary>
	/// <param name="function">
	/// �������, ������ ������-���� ������, ������������ �� VFunction
	/// </param>
	/// <param name="eps">
	/// �������� ����������
	/// </param>
	/// <param name="minOrMax">
	/// �������� ����� 1, ���� ����� �������� �������, � -1, ���� ���������
	/// </param>
	VGaussSeidelMethod(VFunction* function, double eps, int minOrMax);
	
	/// <summary>
	/// ����� ���������� ������� ������� ������-�������
	/// </summary>
	void RunMethod();

private:
	double Eps;
	int MinOrMax;
};

class VGradientMethod : public VMethod
{
public:
	
	/// <summary>
	/// ����������� ������ VGradientMethod
	/// </summary>
	/// <param name="function">
	/// �������, ������ ������-���� ������, ������������ �� VFunction
	/// </param>
	/// <param name="alfa">
	/// �������� ������, ������������� �����, ���������
	/// </param>
	/// <param name="eps">
	/// �������� ����������
	/// </param>
	/// <param name="delta">
	/// �������� ���������� �����������
	/// </param>
	/// <param name="minOrMax">
	/// �������� ����� 1, ���� ����� �������� �������, � -1, ���� ���������
	/// </param>
	VGradientMethod(VFunction* function, double alfa, double eps, double delta, int minOrMax);
	
	/// <summary>
	/// ����� ���������� ������� ������� ����������� �������
	/// </summary>
	void RunMethod();

private:
	double Alfa;
	double Eps;
	double Delta;
	double MinOrMax;	
};


/// <summary>
/// ��������������� ������� - ������� ����������� ������� � �������� �����
/// </summary>
double ch_proizv(VFunction* function, double delta, size_t parNum);
/// <summary>
/// ��������������� ������� - ������ ������� ���������
/// </summary>
double gradSqr(VFunction* function, double delta);
