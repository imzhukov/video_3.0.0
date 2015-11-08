#pragma once

#include "fftw3.h"
#include "mathlib.h"
#include "processing.h"

/// ��������� ������ sinc-�������
class VFIRFilter
{
public:
	/// ���������� ������ ���������� ��������������
	virtual size_t GetIRSize() const = 0;
	/// ���������� ������ �������
	virtual size_t GetFilterSize() const = 0;
	/// ���������� �������� � �����
	virtual point_t operator[](size_t in_index) const = 0;
};
typedef boost::shared_ptr<VFIRFilter> VFIRFilterPtr;

/// ���
class VLowPassFIR : public VFIRFilter
{
private:
	/// ������ ���������� ��������������
	size_t ir_size;
	/// ������ �������
	size_t filter_size;
	/// ������
	std::vector<point_t> filter;
public:
	/// �����������
	VLowPassFIR(double in_cut_off_freq);
	/// ���������� ������ ���������� ��������������
	size_t GetIRSize() const;
	/// ���������� ������ �������
	size_t GetFilterSize() const;
	/// ���������� �������� � �����
	point_t operator[](size_t in_index) const;
};

/// ���
class VHighPassFIR : public VFIRFilter
{
private:
	/// ������ ���������� ��������������
	size_t ir_size;
	/// ������ �������
	size_t filter_size;
	/// ������
	std::vector<point_t> filter;
public:
	/// �����������
	VHighPassFIR(double in_cut_off_freq);
	/// ���������� ������ ���������� ��������������
	size_t GetIRSize() const;
	/// ���������� ������ �������
	size_t GetFilterSize() const;
	/// ���������� �������� � �����
	point_t operator[](size_t in_index) const;
};


/// ��������� ������
class VBandPassFIR : public VFIRFilter
{
private:
	/// ������ ���������� ��������������
	size_t ir_size;
	/// ������ �������
	size_t filter_size;
	/// ������
	std::vector<point_t> filter;
public:
	/// �����������
	VBandPassFIR(double in_low, double in_high);
	/// ���������� ������ ���������� ��������������
	size_t GetIRSize() const;
	/// ���������� ������ �������
	size_t GetFilterSize() const;
	/// ���������� �������� � �����
	point_t operator[](size_t in_index) const;
};

/// ����������� ������
class VBandStopFIR : public VFIRFilter
{
private:
	/// ������ ���������� ��������������
	size_t ir_size;
	/// ������ �������
	size_t filter_size;
	/// ������
	std::vector<point_t> filter;
public:
	/// �����������
	VBandStopFIR(double in_low, double in_high);
	/// ���������� ������ ���������� ��������������
	size_t GetIRSize() const;
	/// ���������� ������ �������
	size_t GetFilterSize() const;
	/// ���������� �������� � �����
	point_t operator[](size_t in_index) const;
};

/// ������� �������
class VWindowFunction
{
public:
	virtual double operator()(size_t in_index, size_t in_count) const = 0;	
};
typedef boost::shared_ptr<VWindowFunction> VWindowFunctionPtr;

/// ������������� ����
class VRectWindowFunction : public VWindowFunction
{
private:
public:
	double operator()(size_t in_index, size_t in_count) const
	{
		return 1.0;
	};
};

/// ���� �����
class VHunnWindowFunction : public VWindowFunction
{
private:
public:
	double operator()(size_t in_index, size_t in_count) const
	{
		return 0.5*(1.0-cos(2.0*M_PI*double(in_index)/(double(in_count)-1.0)));
	};	
};

/// ���� ��������
class VHammingWindowFunction : public VWindowFunction
{
private:
public:
	double operator()(size_t in_index, size_t in_count) const
	{
		return 0.54-0.46*cos(2.0*M_PI*double(in_index)/(double(in_count)-1.0));
	};	
};

/// ���� ���������
class VBartlettWindowFunction : public VWindowFunction
{
private:
public:
	double operator()(size_t in_index, size_t in_count) const
	{
		return 1.0-fabs(double(in_index)/((double(in_count)-1.0)/2)-1.0);
	};
};

/// ���� ��������
class VBlackmanWindowFunction : public VWindowFunction
{
private:
public:
	double operator()(size_t in_index, size_t in_count) const
	{
		return 0.42-0.5*cos(2.0*M_PI*double(in_index)/double(in_count-1))+0.08*cos(4.0*M_PI*double(in_index)/double(in_count-1));
	};
};

/// ��� ������
class VFFTFilter : public VDataProcessor<point_t, point_t>
{
private:

	/// ������
	VFIRFilterPtr filter;	
	/// ������ �� �����
	std::deque<point_t> input;
	/// ������ �� ������
	std::deque<point_t> output;
	/// ��������� ������
	fftw_complex *signal;
	/// ��������� ������� ���
	fftw_complex *spectrum;
	/// ��������� ��������� ���
	fftw_complex *result;
	/// ������������ FFTW
	fftw_plan forward, backward;	
	/// ����������
	void Calculate();

public:
	/// �����������
	VFFTFilter(const VFIRFilterPtr& in_filter);
	/// ����������
	~VFFTFilter();
	/// ���� ������
	void Put(const point_t& in_point);	
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const;		
	/// ���������� ������ ������� ������
	const point_t& Front() const;
	/// ����������� ������ ������� ������
	void Pop();
	/// ������� ������� ������
	void Flush();
};