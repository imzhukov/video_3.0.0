#include "fft.h"
#include <shiny.h>
#include <fstream>

#include "VException.h"
#include "VLog.h"

/// Формирует импульсную характеристику на основе sinc-функции
std::deque<double> sinc_impulse(size_t in_order, double in_freq)
{
	size_t length=in_order+1;
	if (!is_odd(length))
		throw VSimpleException(L"Длина импульсной характеристики фильтра должна быть нечетной", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	VBlackmanWindowFunction window;
	std::deque<double> impulse;
	double sum=0;
	for (size_t i=0;i<length;++i)
	{
		double h=2.0*in_freq*nsinc(2.0*in_freq*(double(i)-double(in_order)/2));
		impulse.push_back(h*window(i, in_order));
		sum+=impulse.back();
	}
	for (size_t i=0;i<length;++i)
	{
		impulse.at(i)/=sum;		
	}
	return impulse;
};

/// Быстрое проеобразование Фурье
std::vector<point_t> fft(const std::deque<double>& in_impulse)
{
	std::vector<point_t> filter;
	filter.reserve(in_impulse.size());
	filter.resize(in_impulse.size());

	fftw_complex *signal, *spectrum;
	fftw_plan forward;
	signal = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * filter.size());
	spectrum = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * filter.size());
	forward = fftw_plan_dft_1d(filter.size(), signal, spectrum, FFTW_FORWARD, FFTW_ESTIMATE);
	
	for (size_t i=0; i<filter.size(); ++i)
	{
		signal[i][0]=in_impulse.at(i);
		signal[i][1]=0;
	}	 
	fftw_execute(forward);
	
	for (size_t i=0; i<filter.size(); ++i)
	{
		filter.at(i).x=spectrum[i][0];
		filter.at(i).y=spectrum[i][1];
	}	 
	
	fftw_destroy_plan(forward);
	fftw_free(signal); 
	fftw_free(spectrum);	
	return filter;
};

//------------------------------------------ Методы класса VLowPassFIR -------------------------------------------

/// Конструктор
VLowPassFIR::VLowPassFIR(double in_cut_off_freq)
{
	ir_size=size_t(round_to(4.0/in_cut_off_freq,2));
	filter_size=ir_size+ir_size/4;
	
	std::deque<double> impulse=sinc_impulse(ir_size, in_cut_off_freq);
	
	while (impulse.size()<filter_size)
	{
		impulse.push_back(0);
		
	}
	
	filter=fft(impulse);
}

/// Возвращает размер импульсной характеристики
size_t VLowPassFIR::GetIRSize() const
{
	return ir_size;
}

/// Возвращает размер фильтра
size_t VLowPassFIR::GetFilterSize() const
{
	return filter_size;
}

/// Возвращает значение в точке
point_t VLowPassFIR::operator[](size_t in_index) const
{
	return filter.at(in_index);
}


//------------------------------------------ Методы класса VHighPassFIR -------------------------------------------

/// Конструктор
VHighPassFIR::VHighPassFIR(double in_cut_off_freq)
{
	ir_size=size_t(round_to(4.0/in_cut_off_freq,2));
	filter_size=ir_size+ir_size/4;
	
	std::deque<double> impulse=sinc_impulse(ir_size, in_cut_off_freq);
	
	for (size_t i=0;i<impulse.size();++i)
	{
		double x=double(i)-double(ir_size)/2.0;
		if (x==0)
			impulse.at(i)=1.0-2.0*in_cut_off_freq;
		else
			impulse.at(i)=-impulse.at(i);
	}
	
	while (impulse.size()<filter_size)
	{
		impulse.push_back(0);
	}
	
	filter=fft(impulse);
}

/// Возвращает размер импульсной характеристики
size_t VHighPassFIR::GetIRSize() const
{
	return ir_size;
}

/// Возвращает размер фильтра
size_t VHighPassFIR::GetFilterSize() const
{
	return filter_size;
}

/// Возвращает значение в точке
point_t VHighPassFIR::operator[](size_t in_index) const
{
	return filter.at(in_index);
}

//------------------------------------------ Методы класса VBandPassFIR --------------------------------------------

/// Конструктор
VBandPassFIR::VBandPassFIR(double in_low, double in_high)
{
	ir_size=size_t(round_to(4.0/std::min<double>(in_low, in_high),2));
	filter_size=ir_size+ir_size/4;
	
	std::deque<double> impulse_low=sinc_impulse(ir_size, in_low);
	std::deque<double> impulse_high=sinc_impulse(ir_size, in_high);
	std::deque<double> impulse;
	
	for (size_t i=0;i<filter_size;++i)
	{
		double x=double(i)-double(ir_size)/2.0;
		if (x==0)
			impulse.push_back(2.0*(in_high-in_low));
		else if (i<ir_size)
			impulse.push_back(impulse_high.at(i)-impulse_low.at(i));
		else
			impulse.push_back(0);
	}
	
	filter=fft(impulse);
}

/// Возвращает размер импульсной характеристики
size_t VBandPassFIR::GetIRSize() const
{
	return ir_size;
}

/// Возвращает размер фильтра
size_t VBandPassFIR::GetFilterSize() const
{
	return filter_size;
}

/// Возвращает значение в точке
point_t VBandPassFIR::operator[](size_t in_index) const
{
	return filter.at(in_index);
}
	

//------------------------------------------ Методы класса VBandStopFIR --------------------------------------------

/// Конструктор
VBandStopFIR::VBandStopFIR(double in_low, double in_high)
{
	ir_size=size_t(round_to(4.0/std::min<double>(in_low, in_high),2));
	filter_size=ir_size+round_to(ir_size/4,2)+1;
	
	std::deque<double> impulse_low=sinc_impulse(ir_size, in_low);
	std::deque<double> impulse_high=sinc_impulse(ir_size, in_high);
	std::deque<double> impulse;
	
	for (size_t i=0;i<filter_size;++i)
	{
		double x=double(i)-double(ir_size)/2.0;
		if (x==0)
			impulse.push_back(1.0-2.0*(in_high-in_low));
		else if (i<ir_size)
			impulse.push_back(impulse_low.at(i)-impulse_high.at(i));
		else
			impulse.push_back(0);
	}
	
	filter=fft(impulse);
}

/// Возвращает размер импульсной характеристики
size_t VBandStopFIR::GetIRSize() const
{
	return ir_size;
}

/// Возвращает размер фильтра
size_t VBandStopFIR::GetFilterSize() const
{
	return filter_size;
}

/// Возвращает значение в точке
point_t VBandStopFIR::operator[](size_t in_index) const
{
	return filter.at(in_index);
}	

//----------------------------------------- Методы класса VFFTFilter ----------------------------------------------

/// Вычисление
void VFFTFilter::Calculate()
{
	for (size_t i=0; i<filter->GetFilterSize(); ++i)
	{
		signal[i][0]=input.at(i).y;
		signal[i][1]=0;
	}	 
	fftw_execute(forward);	
	for (size_t i=0; i<filter->GetFilterSize(); ++i)
	{
		double Re=spectrum[i][0]*(*filter)[i].x + spectrum[i][1]*(*filter)[i].y;
		double Im=spectrum[i][1]*(*filter)[i].x - spectrum[i][0]*(*filter)[i].y;
		spectrum[i][0]=Re;
		spectrum[i][1]=Im;
	}
	fftw_execute(backward);	
	for (size_t i=0; i<filter->GetFilterSize()-filter->GetIRSize(); ++i)
	{
		output.push_back(point_t(input.at(i+filter->GetIRSize()/2).x, result[i][0]/input.size()));	
	}	
	while (input.size()>filter->GetIRSize())
	{
		input.pop_front();
	}
}

/// Конструктор
VFFTFilter::VFFTFilter(const VFIRFilterPtr& in_filter) : filter(in_filter) 
{
	signal = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * filter->GetFilterSize());
	spectrum = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * filter->GetFilterSize());
	result = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * filter->GetFilterSize());
	forward = fftw_plan_dft_1d(filter->GetFilterSize(), signal, spectrum, FFTW_FORWARD, FFTW_ESTIMATE);
	backward = fftw_plan_dft_1d(filter->GetFilterSize(), spectrum, result, FFTW_BACKWARD, FFTW_ESTIMATE);
}

/// Деструктор
VFFTFilter::~VFFTFilter()
{
	fftw_destroy_plan(forward);
	fftw_destroy_plan(backward);
	fftw_free(signal); 
	fftw_free(spectrum);
	fftw_free(result);
}

/// Ввод данных
void VFFTFilter::Put(const point_t& in_point)
{	
	input.push_back(in_point);
	if (input.size()==filter->GetFilterSize())
	{
		Calculate();
	}
}

/// Возвращает true, если есть обработанные данные
bool VFFTFilter::Ready() const
{
	return output.size()>0;
}

/// Возвращает true, если обработанных данных нет
bool VFFTFilter::Empty() const
{
	return output.empty();
}
	
/// Возвращает первый элемент данных
const point_t& VFFTFilter::Front() const
{
	return output.front();
}

/// Выталкивает первый элемент данных
void VFFTFilter::Pop()
{
	output.pop_front();
}

/// Сливает остаток данных
void VFFTFilter::Flush()
{
	while (!input.empty())
	{
		transfer<point_t>(input, output);
	}
}