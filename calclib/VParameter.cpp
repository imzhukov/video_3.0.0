/*
Файл VParameter.cpp
Измеренный параметр в файле поездки
*/

#define _USE_MATH_DEFINES
#include <math.h>
#include "VParameter.h"
#include "VCorrupt.h"
#include "VProgressInterface.h"
#include "kalman.h"
#include <float.h>
#include <iostream>
#include <fstream>
#include <numeric>

#include <fftw3.h>

#include "Shiny.h"


// ---------------------------------------------------------------------------
/// Шаблонные классы параметров
// ---------------------------------------------------------------------------

// Создать "пустой" параметр
template <class T> 
bool VTemplateParameter<T>::Create(uint32_t type, uint32_t recCount, uint32_t ver, const T & defValue)
{
	this->Unload();

	this->data.resize(recCount, defValue);
	this->version = ver;
	this->corrupt->Create(this->GetCount());
	this->SetLoaded(true);
	this->message = ERRMSG_OK;

	return true;
}

// Загрузка
template <class T> template <class F> 
bool VTemplateParameter<T>::Load(F & file, uint32_t type)
{
	if (this->IsLoaded()) return true;
	this->Unload();

	// временный параметр
	VBasicParameter prm;
	if (!prm.Load(file, type, false))
	{
		this->message = prm.GetErrorMessage();
		return false;
	}
	// неверные данные типа данных/размера записи
	if (!prm.CheckRecCount<T>() || prm.GetRecCount() == 0)
	{
		prm.UnloadFile(file, type);
		prm.Unload();
		this->message = ERRMSG_RECORD_INCORRECT;
		return false;
	}
	// копирование данных в вектор, выгрузка лишнего
	this->data = std::vector<T>(prm.GetPtr<T>(0), prm.GetPtr<T>(0) + prm.GetRecCount());
	prm.Unload();

	// отбраковки
	this->corrupt->Create(this->GetCount());
	this->message = ERRMSG_OK;
	this->SetLoaded(true);
	return true;
}

// Загрузка с цеплянием внешнего канала отбраковок
template <class T> template <class F> 
bool VTemplateParameter<T>::Load(F & file, uint32_t type, VParameterCorruptPtr corr)
{
	if (this->IsLoaded()) return true;
	this->Unload();

	// временный параметр
	VBasicParameter prm;
	if (!prm.Load(file, type, false))
	{
		this->message = prm.GetErrorMessage();
		return false;
	}
	// неверные данные типа данных/размера записи
	if (!prm.CheckRecCount<T>())
	{
		prm.UnloadFile(file, type);
		prm.Unload();
		this->message = ERRMSG_RECORD_INCORRECT;
		return false;
	}
	// копирование данных в вектор, выгрузка лишнего
	this->data = std::vector<T>(prm.GetPtr<T>(0), prm.GetPtr<T>(0) + prm.GetRecCount());
	prm.Unload();

	// отбраковки
	this->corrupt = corr;
	this->message = ERRMSG_OK;
	this->SetLoaded(true);
	return true;
}

// Сохранить в файл
template <class T>
bool VTemplateParameter<T>::Save(VMainFile & file, uint32_t type)
{
	VBasicParameter prm;
	prm.Create(type, GetCType<T>(), this->data.size(), sizeof(T), this->version);
	T * output = static_cast<T *>(prm.GetPtr(0));
	T * result = std::copy(this->data.begin(), this->data.end(), output);
	return prm.Save(file, data.size(), sizeof(T));
}

template class VTemplateParameter<float>;
template bool VTemplateParameter<float>::Load(VMainFile & file, uint32_t type, VParameterCorruptPtr corr);
template bool VTemplateParameter<float>::Load(VMainFile & file, uint32_t type);
template bool VTemplateParameter<float>::Load(VRemoteFile & file, uint32_t type, VParameterCorruptPtr corr);
template bool VTemplateParameter<float>::Load(VRemoteFile & file, uint32_t type);

template class VTemplateParameter<double>;
template bool VTemplateParameter<double>::Load(VMainFile & file, uint32_t type, VParameterCorruptPtr corr);
template bool VTemplateParameter<double>::Load(VMainFile & file, uint32_t type);
template bool VTemplateParameter<double>::Load(VRemoteFile & file, uint32_t type, VParameterCorruptPtr corr);
template bool VTemplateParameter<double>::Load(VRemoteFile & file, uint32_t type);

template class VTemplateParameter<unsigned long>;
template bool VTemplateParameter<unsigned long>::Load(VMainFile & file, uint32_t type, VParameterCorruptPtr corr);
template bool VTemplateParameter<unsigned long>::Load(VMainFile & file, uint32_t type);
template bool VTemplateParameter<unsigned long>::Load(VRemoteFile & file, uint32_t type, VParameterCorruptPtr corr);
template bool VTemplateParameter<unsigned long>::Load(VRemoteFile & file, uint32_t type);

// ---------------------------------------------------------------------------
/// Абсолютная координата
// ---------------------------------------------------------------------------

// Поиск индекса кадра по координате.
// bool forceFit - для точек за пределами массива выбирать крайние индексы(0 и count-1)
uint32_t VAbsCoordData::FindIndex(double absCoord, bool forceFit) const
{		
	if (this->GetCount()==0) return NOT_FOUND;

	if (forceFit)
	{		
		if (absCoord <= this->GetItem(0)) return 0;
		if (absCoord >= this->GetLastItem()) return this->GetCount()-1;
	}
	//Интерполирующий поиск (можем применять так как шаг абсолютной координаты примерно постоянный)
	size_t left=0;
	size_t right=this->GetCount()-1;	
	size_t mid=NOT_FOUND;

	while (GetItem(left) <= absCoord && GetItem(right) >= absCoord)
	{	
		if ((GetItem(left))==absCoord)
			return left;
		if ((GetItem(right))==absCoord)
			return right;
		mid=round(double(left) + (absCoord-GetItem(left))*double(right-left)/(GetItem(right) - GetItem(left)));
		if (GetItem(mid) < absCoord)
			left = mid+1;
		else if (GetItem(mid) > absCoord)
			right = mid-1;
		else
			return mid;		
	}	
	return mid;	
}

// Пересчет индексов при перепривязке сетки абс.координат из srcAbs к нам
// Например, при отрезании куска файла
bool VAbsCoordData::UpdateIndex(uint32_t & index, const VAbsCoordData & srcAbs) const
{
	// индекс лежит за пределами старой сетки
	if (index >= srcAbs.GetCount()) return false;

	// координата точки
	double coord = srcAbs.GetItem(index);

	// индекс лежит за пределами новой сетки
	if (coord < this->GetItem(0) || coord > this->GetLastItem()) return false;

	// привязываемся к новой сетки
	index = this->FindIndex(coord, true);

	// voila!
	return true;
}

/// Сдвиг координаты
void VAbsCoordData::Shift(double offset_m)
{
	for (size_t i = 0; i < this->GetCount(); ++i)
		this->GetItem(i) += offset_m;
}

/// Масштабирование координаты
void VAbsCoordData::Scale(double scale_m_per_km)
{
	for (size_t i = 0; i < this->GetCount(); ++i)
		this->GetItem(i) += i * scale_m_per_km / 1000.0;
}

// Пересчет координаты
void VAbsCoordData::Correct(double offset_m, double expand_m_per_km)
{
	if (this->GetCount() < 2) return;
	double avgStep = (this->GetLastItem() - this->GetItem(0)) / (this->GetCount() - 1);
	double kadrCorr = avgStep * expand_m_per_km / 1000.0;
	for (size_t i = 0; i < this->GetCount(); ++i)
		this->GetItem(i) += offset_m + i * kadrCorr;
}

// Пересчёт координаты с использованием параметров апроксимации
void VAbsCoordData::Correct(line_t input)
{
	if(this->GetCount() < 2) return;
	for (size_t i = 0; i < this->GetCount(); ++i)
		this->GetItem(i) -= (this->GetItem(i)*input.k() + input.b());
}

// Функция вычисления длины кадра
const double VAbsCoordData::GetLengthStep()
{
	return (this->GetLastItem() - this->GetItem(0)) / (this->GetCount() - 1);
}

// ---------------------------------------------------------------------------
/// Параметр
// ---------------------------------------------------------------------------

///Сбрасывает кэш
void VParameter::ResetCache()
{
	cache_sum=0;
	cache_sum_index1=BAD_INDEX;
	cache_sum_index2=BAD_INDEX;	
	cache_corrupt=0;
	cache_corrupt_index1=BAD_INDEX;
	cache_corrupt_index2=BAD_INDEX;	
	first_def=BAD_INDEX;
	last_def=BAD_INDEX;
}

/// Конструктор
VParameter::VParameter()
{
	ResetCache();
}

// Добавляет ко всем значениям постоянную величину
void VParameter::Shift(float shift)
{
	for (size_t i = 0; i < this->GetCount(); ++i)
		this->GetItem(i) += shift;
	ResetCache();
}

// Смещаем на shift кадров
void VParameter::ShiftByKadr(short int shift, bool fill_end)
{
	int i = 0;
	if(shift > 0)
	{
		std::copy_backward(this->data.begin(), this->data.end() - shift, this->data.end());
		float value = fill_end ? this->data.front() : -1;
		std::fill(this->data.begin(), this->data.begin() + shift, value);
	}
	else
	{
		std::copy(this->data.begin() - shift, this->data.end(), this->data.begin());
		float value = fill_end ? this->data.back() : -1;
		std::fill(this->data.end() + shift, this->data.end(), value);
	}
	ResetCache();
}

void VParameter::LoadFromArray(float * param)
{
	for(size_t i = 0; i < this->GetCount(); i++)
		GetItem(i)=param[i];
	ResetCache();
}

// Инвертирует параметр относительно оси
void VParameter::Invert(float axis)
{
	for (size_t i = 0; i < this->GetCount(); ++i)
		this->GetItem(i) = (-1.0f * (this->GetItem(i) - axis)) + axis;
	ResetCache();
}

/// Возвращает индекс максимального по модулю значения на участке
size_t VParameter::GetMaxAbsIndex(size_t startNdx, size_t endNdx) const
{	
	size_t result=startNdx;
	std::vector<float>::const_iterator it = std::max_element(
		this->data.begin() + startNdx, this->data.begin() + endNdx, abs_comparator);
	
	if (it != this->data.begin() + endNdx) result = it - this->data.begin();
	return result;
}

// Возврашает максимальное значение на участке
float VParameter::GetMax(size_t startNdx, size_t endNdx) const
{
	if (this->data.empty()) return 0;	
	std::vector<float>::const_iterator it = std::max_element(
		this->data.begin() + startNdx, this->data.begin() + endNdx);	
	return (it != this->data.begin() + endNdx) ? *it : 0;	
}

/// Возврашает максимальное значение на всем массиве
float VParameter::GetMax() const
{
	size_t start=GetFirstDef();
	size_t end=GetLastDef();
	if (start!=BAD_INDEX && end!=BAD_INDEX)
		return this->GetMax(start, end);
	else return 0;
}

// Возврашает максимальное (по модулю) значение на участке
float VParameter::GetMaxAbs(size_t startNdx, size_t endNdx) const
{
	if (this->data.empty()) return 0;

	float value = FLT_MIN;

	std::vector<float>::const_iterator it = std::max_element(
		this->data.begin() + startNdx, this->data.begin() + endNdx, abs_comparator);
	
	if (it != this->data.begin() + endNdx) value = fabs(*it);

	return value;
}

// Возврашает максимальное (по модулю) значение на участке со знаком
float VParameter::GetMaxAbsSigned(size_t startNdx, size_t endNdx) const
{
	if (this->data.empty()) return 0;

	float value = FLT_MIN;
	
	std::vector<float>::const_iterator it = std::max_element(
		this->data.begin() + startNdx, this->data.begin() + endNdx, abs_comparator);
	
	if (it != this->data.begin() + endNdx) value = *it;

	return value;
}

// Возврашает минимальное значение на участке
float VParameter::GetMin(size_t startNdx, size_t endNdx) const
{
	if (this->data.empty()) return 0;	
	std::vector<float>::const_iterator it = std::min_element(
		this->data.begin() + startNdx, this->data.begin() + endNdx);

	return (it != this->data.begin() + endNdx) ? *it : 0;	
}

/// Возврашает минимальное значение навсем массиве
float VParameter::GetMin() const 
{
	size_t start=GetFirstDef();
	size_t end=GetLastDef();
	if (GetFirstDef()!=BAD_INDEX && GetLastDef()!=BAD_INDEX)
		return this->GetMin(start, end);
	else return 0;
}

// Возврашает минимальное (по модулю) значение на участке
float VParameter::GetMinAbs(size_t startNdx, size_t endNdx) const
{
	if (this->data.empty()) return 0;

	float value = FLT_MAX;

	std::vector<float>::const_iterator it = std::min_element(
		this->data.begin() + startNdx, this->data.begin() + endNdx, abs_comparator);

	if (it != this->data.begin() + endNdx) value = fabs(*it);

	return value;
}

// Возвращает сумму значений на участке
double VParameter::GetSum(size_t startNdx, size_t endNdx) const
{	
	if (startNdx < endNdx && endNdx < this->GetCount())
	{		
		//Если диапазон расчет а тот-же возвращаем кэш
		if (startNdx==cache_sum_index1 && endNdx==cache_sum_index2)
		{
			//Ничего не меняем
		}
		//Если имеем дело с последовательным расчетом
		else if (startNdx>=cache_sum_index1 && startNdx<cache_sum_index2 && endNdx>=cache_sum_index2)
		{
			while (cache_sum_index1<startNdx)
			{
				cache_sum-=GetItem(cache_sum_index1++);			
			};
			while (cache_sum_index2<endNdx)
			{				
				cache_sum+=GetItem(++cache_sum_index2);
			};
		}
		//Новый расчет
		else 
		{			
			cache_sum = std::accumulate(
				this->data.begin() + startNdx, this->data.begin() + (endNdx + 1), 0.0);
			cache_sum_index1=startNdx;
			cache_sum_index2=endNdx;			
		}		

		return cache_sum;
	}
	else
		return UNDEFINED_VALUE;
}

/// Возвращает взвешенную сумму значений на участке
double VParameter::GetWeightedSum(size_t startNdx, size_t endNdx) const
{	
	if (startNdx < endNdx && endNdx < this->GetCount())
	{	
		// Формируем вектор весов	
		if (cache_weight.size()!=endNdx-startNdx+1)
		{
			cache_weight.reserve(endNdx-startNdx+1);
			cache_weight.resize(endNdx-startNdx+1);
			double weight_sum=0;
			double sigma=double(cache_weight.size())/6.0;
			for (size_t i=0; i<cache_weight.size(); ++i)
			{
				double x=double(i)-double(cache_weight.size())/2;
				cache_weight.at(i)=gauss(x,0,sigma);
				weight_sum+=cache_weight.at(i);
			}
			// Нормируем веса
			for (size_t i=0; i<cache_weight.size(); ++i)
			{
				cache_weight.at(i)/=weight_sum;
			}
		}
		// Вычисляем взвешенную сумму
		double sum=0;					
		for (size_t i=startNdx; i<=endNdx; ++i)
		{
			sum+=GetItem(i)*cache_weight.at(i-startNdx);
		}
		return sum;
	}
	else
		return UNDEFINED_VALUE;
}

/// Возвращает число сбоев на участке
size_t VParameter::GetCorruptCount(size_t startNdx, size_t endNdx) const
{
	if (startNdx < endNdx && endNdx < this->GetCount())
	{		
		//Если диапазон расчет а тот-же возвращаем кэш
		if (startNdx==cache_corrupt_index1 && endNdx==cache_corrupt_index2)
		{
			//Ничего не меняем
		}
		//Если имеем дело с последовательным расчетом
		else if (startNdx>=cache_corrupt_index1 && startNdx<cache_corrupt_index2 && endNdx>=cache_corrupt_index2)
		{
			while (cache_corrupt_index1<startNdx)
			{
				cache_corrupt-=IsCorrupt(cache_corrupt_index1++) ? 1 : 0;
			};
			while (cache_corrupt_index2<endNdx)
			{				
				cache_corrupt+=IsCorrupt(++cache_corrupt_index2) ? 1 : 0;
			};
		}
		//Новый расчет
		else 
		{
			cache_corrupt = 0;			
			for (size_t i = startNdx; i <= endNdx; ++i)
				cache_corrupt += IsCorrupt(i) ? 1 : 0;
			cache_corrupt_index1=startNdx;
			cache_corrupt_index2=endNdx;			
		}		

		return cache_corrupt;
	}
	else
		return 0;
}

// Возвращает число сбоев на участке
size_t VParameter::GetCorruptCount(size_t axis, float halfBase, const VAbsCoordData & absCoord) const
{
	return this->GetCorruptCount(absCoord.FindIndex(absCoord.GetItem(axis) - halfBase, true), 
		absCoord.FindIndex(absCoord.GetItem(axis) + halfBase, true));
}

/// Возвращает сумму квадратов значений на участке
double VParameter::GetSum2(size_t startNdx, size_t endNdx, double mo) const
{
	if (startNdx < endNdx && endNdx < this->GetCount())
	{	
		class Sum2
		{
			const double mo;

		public:

			Sum2(double mo_value) : mo(mo_value)
			{}

			double operator() (double init, double item) const
			{
				return init + (item - mo) * (item - mo);
			}
		};

		return std::accumulate(
			this->data.begin() + startNdx, this->data.begin() + (endNdx+1), 0.0, Sum2(mo));
	}
	else
		return 0;
}

// Возвращает МО на участке
float VParameter::GetMO(size_t startNdx, size_t endNdx) const
{			
	if (startNdx < endNdx && endNdx < this->GetCount())
		return static_cast<float>(this->GetSum(startNdx, endNdx) / (endNdx-startNdx+1));	
	else
		return UNDEFINED_VALUE;
}

// Возвращает МО на участке - с учетом сбоев, но без кэширования. Для статистики
float VParameter::GetStatMO(size_t startNdx, size_t endNdx) const
{
	if (startNdx < endNdx && endNdx < this->GetCount())
	{
		double sum = 0;
		size_t count = 0;

		for (size_t i = startNdx; i <= endNdx; ++i)
		{
			if (!this->IsCorrupt(i))
			{
				sum += this->GetItem(i);
				++count;
			}
		}

		if (count > 0) sum /= count;
		return static_cast<float>(sum);
	}
	else
		return UNDEFINED_VALUE;
}

// Возвращает максимум на участке - с учетом сбоев, но без кэширования. Для износов
float VParameter::GetStatMax(size_t startNdx, size_t endNdx) const
{
    if (startNdx < endNdx && endNdx < this->GetCount())
    {
        double max = 0;
        
        for (size_t i = startNdx; i <= endNdx; ++i)
        {
            if (!this->IsCorrupt(i))
            {
                if (i == startNdx)  max = this->GetItem(i);
                else                max = std::max<double>(max, this->GetItem(i));
            }
        }
        
        return static_cast<float>(max);
    }
}

// Возвращает СКО на участке - с учетом сбоев, но без кэширования. Для статистики
float VParameter::GetStatSKO(size_t startNdx, size_t endNdx, float mo) const
{
	if (startNdx < endNdx && endNdx < this->GetCount())
	{
		double sum2 = 0, item = 0;
		size_t count = 0;

		for (size_t i = startNdx; i <= endNdx; ++i)
		{
			if (!this->IsCorrupt(i))
			{
				item = this->GetItem(i);
				sum2 += (item - mo) * (item - mo);
				++count;
			}
		}

		if (count > 0) sum2 /= count;
		return static_cast<float>(sqrt(sum2));
	}
	else
		return UNDEFINED_VALUE;
}

/// Возвращает МО на участке
float VParameter::GetWeightedMean(size_t startNdx, size_t endNdx) const
{
	if (startNdx < endNdx && endNdx < this->GetCount())
		return static_cast<float>(this->GetWeightedSum(startNdx, endNdx));	
	else
		return UNDEFINED_VALUE;
}

/// Возвращает МО в радиусе +/- halfBase от оси axis
float VParameter::GetMO(size_t axis, float halfBase, const VAbsCoordData & absCoord) const
{
	return this->GetMO(absCoord.FindIndex(absCoord.GetItem(axis) - halfBase, true), 
		absCoord.FindIndex(absCoord.GetItem(axis) + halfBase, true));
}


/// Возвращает взвешенное среднее в радиусе +/- halfBase от оси axis
float VParameter::GetWeightedMean(size_t axis, float halfBase, const VAbsCoordData & absCoord) const
{
	return this->GetWeightedMean(absCoord.FindIndex(absCoord.GetItem(axis) - halfBase, true), 
		absCoord.FindIndex(absCoord.GetItem(axis) + halfBase, true));
}

/// Возвращает МО на всём файле
float VParameter::GetMO() const
{
	if (this->GetCount()>0)
		return this->GetMO(GetFirstDef(), GetLastDef());
	else
		return 0;
}

/// Возвращает неровность в точке
float VParameter::GetJaggy(size_t axis, float base, const VAbsCoordData & absCoord) const
{	
	float mo=GetMO(axis, base/2, absCoord);
	return  (mo!=UNDEFINED_VALUE) ? GetItem(axis)-mo : UNDEFINED_VALUE;
}

/// Возвращает дисперсию на участке
float VParameter::GetDisp(size_t startNdx, size_t endNdx, double mo) const
{
	if (startNdx < endNdx && endNdx < this->GetCount())
		return static_cast<float>(this->GetSum2(startNdx, endNdx, mo) / (endNdx - startNdx));
	else
		return 0;
}

/// Возвращает дисперсию на всём файле
float VParameter::GetDisp() const
{
	if (GetCount()>0)
		return GetDisp(0, GetCount()-1, GetMO());
	else
		return 0;
}


// Возвращает СКО на участке
float VParameter::GetSKO(size_t startNdx, size_t endNdx, float mo) const
{
	return sqrt(GetDisp(startNdx, endNdx, mo));	
}

/// Возвращает СКО на всём файле
float VParameter::GetSKO(float mo) const
{
	return this->GetSKO(0, this->GetCount()-1, mo);
}



/// Возвращает СТО на участке
float VParameter::GetSTO(size_t startNdx, size_t endNdx, float mo) const
{
	return sqrt(GetDisp(startNdx, endNdx, mo) * (endNdx - startNdx + 1) / (endNdx - startNdx));	
}

/// Возвращает СТО на всём файле
float VParameter::GetSTO(float mo) const
{
	return this->GetSTO(0, this->GetCount()-1, mo);
}

/// Возвращает распределение плотности вероятности на участке
std::set<density_t> VParameter::GetDensity(size_t startNdx, size_t endNdx, double in_size)
{
	//Вычисляем плотность вероянтностей	
	density_calculator dc(in_size);
	for (size_t i=startNdx; i<=endNdx; ++i)
	{	
		if (this->IsLoaded() && !this->IsCorrupt(i))				
			dc.put(GetItem(i));
	}
	return dc.get();
}

/// Возвращает уравнение прямой аппроксимирующей участок по МНК
line_t VParameter::GetLine(size_t startNdx, size_t endNdx) const
{		
	if (startNdx < endNdx && endNdx < this->GetCount())
	{
		double SumDelta=0;
	    double SumDelta2=0;
		double SumCoord=0;
		double SumCoord2=0;
		double SumCoordDelta=0;
		for (size_t i = startNdx; i <= endNdx; ++i)
		{
			SumDelta+=GetItem(i);
			SumDelta2+=pow(GetItem(i),2);
			SumCoord+=i;
			SumCoord2+=pow(static_cast<double>(i),2);
			SumCoordDelta+=GetItem(i)*i;
		}
		double Count=endNdx-startNdx+1;					
		return line_t(Count*SumCoordDelta-SumDelta*SumCoord, 
			-(Count*SumCoord2-pow(SumCoord,2)), SumDelta*SumCoord2-SumCoordDelta*SumCoord);
		
	}
	else
	{
		throw VSimpleException(L"Невозможно получить уравнение прямой", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	
}

/// Возвращает уравнение прямой аппроксимирующей участок по МНК (по абс. координате)
line_t VParameter::GetLine(size_t startNdx, size_t endNdx, const VAbsCoordData & absCoord) const
{
	line_t result;
	if (startNdx < endNdx && endNdx < this->GetCount())
	{
		double SumDelta=0;
	    double SumDelta2=0;
		double SumCoord=0;
		double SumCoord2=0;
		double SumCoordDelta=0;
		for (size_t i = startNdx; i <= endNdx; ++i)
		{
			SumDelta+=GetItem(i);
			SumDelta2+=pow(GetItem(i),2);
			SumCoord+=absCoord.GetItem(i);
			SumCoord2+=pow(static_cast<double>(absCoord.GetItem(i)),2);
			SumCoordDelta+=GetItem(i)*absCoord.GetItem(i);
		}
		double Count=endNdx-startNdx+1;					
		return line_t(Count*SumCoordDelta-SumDelta*SumCoord, 
			-(Count*SumCoord2-pow(SumCoord,2)), SumDelta*SumCoord2-SumCoordDelta*SumCoord);		
	}
	else
	{
		throw VSimpleException(L"Невозможно получить уравнение прямой", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

/// Возвращает уравнение прямой, образующей хорду
line_t VParameter::GetChord(size_t startNdx, size_t endNdx)
{
	return line_t(point_t(startNdx, GetItem(startNdx)), 
			point_t(endNdx, GetItem(endNdx)));
}

/// Возвращает уравнение прямой, образующей хорду
line_t VParameter::GetChord(size_t startNdx, size_t endNdx, const VAbsCoordData & absCoord)
{
	return line_t(point_t(absCoord.GetItem(startNdx), GetItem(startNdx)), 
			point_t(absCoord.GetItem(endNdx), GetItem(endNdx)));
}

/// Возвращает значение уклона параметра на базе 'base' метров
float VParameter::GetOtvod(size_t in_start, size_t in_end, const VAbsCoordData & absCoord) const
{	
	if (in_start<in_end && in_start!=BAD_INDEX && in_end!=BAD_INDEX)
		return (GetItem(in_end) - GetItem(in_start)) / (absCoord.GetItem(in_end) - absCoord.GetItem(in_start));
	else 
		return UNDEFINED_VALUE;
}

/// Возвращает отвод в точке
float VParameter::GetOtvod(size_t ndx, const VAbsCoordData & absCoord, double base) const
{	
	size_t start = absCoord.FindIndex(absCoord.GetItem(ndx) - base/2.0, false);
	size_t end = absCoord.FindIndex(absCoord.GetItem(ndx) + base/2.0, false);
	return GetOtvod(start, end, absCoord);
}

/// Рссчитывает производную
float VParameter::CalcDerivate(size_t in_start, size_t in_end) const
{
	if (in_start<in_end && in_start!=BAD_INDEX && in_end!=BAD_INDEX)
		return GetLine(in_start, in_end).k();
	else 
		return UNDEFINED_VALUE;
}

/// Возвращает производную в точке (база - 3 кадра)
float VParameter::GetDerivate(size_t in_index) const
{
	return CalcDerivate(in_index-1, in_index+1);	
}


/// Возвращает вторую производную в точке (база - 3 кадра)
float VParameter::GetDerivate2(size_t in_index) const
{
	return line_t(point_t(in_index-1, GetDerivate(in_index-1)), point_t(in_index+1, GetDerivate(in_index+1))).k();	
}

/// Возвращает производную в точке (база - число кадров)
float VParameter::GetDerivate(size_t in_index, size_t in_base) const
{
	return CalcDerivate(in_index-in_base/2, in_index+in_base/2);
}

/// Возвращает производную в точке (база - длина в метрах)
float VParameter::GetDerivate(size_t ndx, const VAbsCoordData & absCoord, double base) const
{	
	size_t start = absCoord.FindIndex(absCoord.GetItem(ndx) - base/2.0, true);
	size_t end = absCoord.FindIndex(absCoord.GetItem(ndx) + base/2.0, true);
	return CalcDerivate(start, end);	
}

/// Возвращает медиану на отрезке
float VParameter::GetMedian(size_t startNdx, size_t endNdx) const
{
	median_buffer.reserve(endNdx-startNdx+1);
	median_buffer.resize(endNdx-startNdx+1);
	for (size_t i=startNdx; i<=endNdx; ++i)
	{
		median_buffer[i-startNdx]=GetItem(i);		
	}	
	std::nth_element(median_buffer.begin(), median_buffer.begin()+(median_buffer.size()/2), median_buffer.end());
	return median_buffer.at(median_buffer.size()/2);
}

/// Возвращает медиану
float VParameter::GetMedian(size_t ndx, const VAbsCoordData & absCoord, double base) const
{
	if (!absCoord.IsLoaded() || absCoord.GetCount() != this->GetCount() ||
		absCoord.GetItem(ndx) - absCoord.GetItem(0) < base/2.0 ||
		absCoord.GetLastItem() - absCoord.GetItem(ndx) < base/2.0)
	{
		return 0.0f;
	}

	size_t ndx0 = absCoord.FindIndex(absCoord.GetItem(ndx) - base/2.0, true);
	size_t ndx1 = absCoord.FindIndex(absCoord.GetItem(ndx) + base/2.0, true);
	
	return GetMedian(ndx0, ndx1);
}

/// Возвращает индекс первого определенного значения
size_t VParameter::GetFirstDef() const
{
	if (first_def!=BAD_INDEX)
		return first_def;
	for (size_t i=0; i<GetCount(); ++i)
	{
		if (GetItem(i)!=UNDEFINED_VALUE)
		{
			first_def=i;
			return first_def;
		}
	}
	return BAD_INDEX;
}

/// Возвращает индекс последнего определенного значения
size_t VParameter::GetLastDef() const
{
	if (last_def != BAD_INDEX)
		return last_def;

	if (GetCount() > 0)
		for (size_t i = 0; i < GetCount(); ++i)
		{
			size_t index = GetCount() - 1 - i;
			if (GetItem(index) != UNDEFINED_VALUE)
			{
				last_def=index;
				return last_def;
			}
		}
	return BAD_INDEX;
}

///Возвращает комментарий
std::wstring VParameter::GetComment() const
{
	return L"";
}

///Расчет скользящего среднего на базе (где база - абс. коорд.)
void ParameterMean(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base)
{
	if (!in_source.IsLoaded())
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Исходные данные не загружены", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (in_source.GetCount()==out_target.GetCount())
	{
		size_t start=in_source.GetFirstDef();
		size_t end=in_source.GetLastDef();
		for (size_t i=start; i<end; ++i)
		{
			int stIndex=in_abscoord.FindIndex(in_abscoord.GetItem(i) - in_base/2, false);
			int eIndex=in_abscoord.FindIndex(in_abscoord.GetItem(i) + in_base/2, false);
			if (stIndex!=BAD_INDEX && eIndex!=BAD_INDEX)
			{
				out_target.GetItem(i)=in_source.GetMO(stIndex,eIndex);
				out_target.SetCorrupt(i, in_source.GetCorruptCount(stIndex, eIndex)>0);
			}
			else
				out_target.GetItem(i)=UNDEFINED_VALUE;
		}
	}
	else
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Не совпадают размеры входного и выходного массивов", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
}

///Расчет взвешенного скользящего среднего на базе (где база - абс. коорд.)
void ParameterWeightedMean(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base)
{
	if (!in_source.IsLoaded())
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Исходные данные не загружены", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (in_source.GetCount()==out_target.GetCount())
	{
		size_t start=in_source.GetFirstDef();
		size_t end=in_source.GetLastDef();	
		for (size_t i=start; i<end; ++i)
		{
			int stIndex=in_abscoord.FindIndex(in_abscoord.GetItem(i) - in_base/2, false);
			int eIndex=in_abscoord.FindIndex(in_abscoord.GetItem(i) + in_base/2, false);
			if (stIndex!=BAD_INDEX && eIndex!=BAD_INDEX)
			{
				out_target.GetItem(i)=in_source.GetWeightedMean(stIndex,eIndex);
				out_target.SetCorrupt(i, in_source.GetCorruptCount(stIndex, eIndex)>0);
			}
			else
				out_target.GetItem(i)=UNDEFINED_VALUE;
		}
	}
	else
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Не совпадают размеры входного и выходного массивов", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}	
}


/// Спектр
void ParameterSpectrum(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord)
{

	VHammingWindowFunction window;

	fftw_complex *signal, *spectrum;
	fftw_plan forward;
	signal = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * in_source.GetCount());
	spectrum = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * in_source.GetCount());
	forward = fftw_plan_dft_1d(in_source.GetCount(), signal, spectrum, FFTW_FORWARD, FFTW_ESTIMATE);
	
	for (size_t i=0; i<in_source.GetCount(); ++i)
	{
		signal[i][0]=in_source.GetItem(i) * window(i, in_source.GetCount());
		signal[i][1]=0;
	}	 
	fftw_execute(forward);
	
	for (size_t i=0; i<in_source.GetCount(); ++i)
	{		
		out_target.GetItem(i)=sqrt(pow(spectrum[i][0],2) + pow(spectrum[i][1],2))/in_source.GetCount();
	}
		
	fftw_destroy_plan(forward);
	fftw_free(signal); 
	fftw_free(spectrum);
}

/// Фаза
void ParameterPhase(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord)
{
	
	VHammingWindowFunction window;

	fftw_complex *signal, *spectrum;
	fftw_plan forward;
	signal = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * in_source.GetCount());
	spectrum = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * in_source.GetCount());
	forward = fftw_plan_dft_1d(in_source.GetCount(), signal, spectrum, FFTW_FORWARD, FFTW_ESTIMATE);
	
	for (size_t i=0; i<in_source.GetCount(); ++i)
	{
		signal[i][0]=in_source.GetItem(i)*window(i, in_source.GetCount());
		signal[i][1]=0;
	}	 
	fftw_execute(forward);
	
	for (size_t i=0; i<in_source.GetCount(); ++i)
	{		
		out_target.GetItem(i)=atan(spectrum[i][1]/spectrum[i][0]);
	}
		
	fftw_destroy_plan(forward);
	fftw_free(signal); 
	fftw_free(spectrum);
}


/// FIR
void ParameterFIRFilter(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, const VFIRFilterPtr& in_filter)
{
	VFFTFilter fft_filter(in_filter);
	size_t pos=in_filter->GetIRSize()/2;
	for (size_t i=0; i<in_abscoord.GetCount(); ++i)
	{
		fft_filter.Put(point_t(in_abscoord.GetItem(i), in_source.GetItem(i)));
		while(fft_filter.Ready())
		{
			out_target.GetItem(pos++)=fft_filter.Front().y;
			fft_filter.Pop();
		}
	}
}

/// Фильтр Калмана
void ParameterKalman(VParameter& in_source, VParameter& out_target, double in_q, double in_r)
{
	if (!in_source.IsLoaded())
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Исходные данные не загружены", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (in_source.GetCount()==out_target.GetCount())
	{
		VKalman Kalman(in_source.GetItem(0), in_q, in_r);
		for (size_t i=0;i<in_source.GetCount();++i)
		{
			Kalman.Put(in_source.GetItem(i));
			while (Kalman.Ready())
			{
				out_target.GetItem(i)=Kalman.Front();
				Kalman.Pop();
			}
		}
	}
	else
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Не совпадают размеры входного и выходного массивов", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

///Расчет скользящего СКО на базе (где база - абс. коорд.)
void ParameterSKO(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base)
{
	if (!in_source.IsLoaded())
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Исходные данные не загружены", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (in_source.GetCount()==out_target.GetCount())
	{
		size_t start=in_source.GetFirstDef();
		size_t end=in_source.GetLastDef();
		for (size_t i=start; i<end; ++i)
		{
			int stIndex=in_abscoord.FindIndex(in_abscoord.GetItem(i) - in_base/2, false);
			int eIndex=in_abscoord.FindIndex(in_abscoord.GetItem(i) + in_base/2, false);
			if (stIndex!=BAD_INDEX && eIndex!=BAD_INDEX)
			{
				out_target.GetItem(i)=in_source.GetSKO(stIndex,eIndex, in_source.GetMO(stIndex,eIndex));
				out_target.SetCorrupt(i, in_source.GetCorruptCount(stIndex, eIndex)>0);
			}
			else
				out_target.GetItem(i)=UNDEFINED_VALUE;
		}
	}
	else
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Не совпадают размеры входного и выходного массивов", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

///Расчет медианного фильтра
void ParameterMedian(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base)
{
	if (!in_source.IsLoaded())
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Исходные данные не загружены", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (in_source.GetCount()==out_target.GetCount())
	{
		size_t start=in_source.GetFirstDef();
		size_t end=in_source.GetLastDef();
		for (size_t i=start; i<end; ++i)
		{
			int stIndex=in_abscoord.FindIndex(in_abscoord.GetItem(i) - in_base/2, false);
			int eIndex=in_abscoord.FindIndex(in_abscoord.GetItem(i) + in_base/2, false);
			if (stIndex!=BAD_INDEX && eIndex!=BAD_INDEX)
			{
				out_target.GetItem(i)=in_source.GetMedian(stIndex,eIndex);
				out_target.SetCorrupt(i, in_source.GetCorruptCount(stIndex, eIndex)>0);
			}
			else
				out_target.GetItem(i)=0;
		}
	}
	else
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Не совпадают размеры входного и выходного массивов", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}

}

///Расчет отвода параметра на базе (где база - длина в метрах)
void ParameterOtvod(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base)
{
	if (!in_source.IsLoaded())
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Исходные данные не загружены", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (in_source.GetCount()==out_target.GetCount())
	{
#pragma omp parallel for
		for (int i=in_source.GetFirstDef(); i<in_source.GetLastDef(); ++i)
		{
			out_target.GetItem(i)=in_source.GetOtvod(i, in_abscoord, in_base);
		}
	}
	else
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Не совпадают размеры входного и выходного массивов", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

///Производная параметра на базе
void ParameterDerivate(VParameter& in_source, VParameter& out_target, size_t in_half_base)
{
	if (!in_source.IsLoaded())
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Исходные данные не загружены", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (in_source.GetCount()==out_target.GetCount())
	{
#pragma omp parallel for
		for (int i=0; i<in_source.GetCount(); ++i)
		{
			out_target.GetItem(i)=in_source.GetDerivate(i, 2*in_half_base);			
		}
	}
	else
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Не совпадают размеры входного и выходного массивов", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

///Производная параметра на базе (база - абс. коорд)
void ParameterDerivate(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base)
{
	if (!in_source.IsLoaded())
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Исходные данные не загружены", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (in_source.GetCount()==out_target.GetCount())
	{
#pragma omp parallel for
		for (int i=0; i<in_source.GetCount(); ++i)
		{
			out_target.GetItem(i)=in_source.GetDerivate(i, in_abscoord, in_base);
		}
	}
	else
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Не совпадают размеры входного и выходного массивов", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

///"Резкость параметра"
void ParameterSharp(VParameter& in_source, VParameter& out_target, double in_amount)
{
	if (!in_source.IsLoaded())
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Исходные данные не загружены", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (in_source.GetCount()==out_target.GetCount())
	{
#pragma omp parallel for
		for (int i=0; i<in_source.GetCount(); ++i)
		{
			out_target.GetItem(i)=in_source.GetItem(i)+(pro::interval<size_t>(2, in_source.GetCount()-3).contains(i) ? in_amount*in_source.GetDerivate2(i) : 0);
		}
	}
	else
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Не совпадают размеры входного и выходного массивов", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

///Разность двух параметров
void ParameterDiff(VParameter& in_source1, VParameter& in_source2, VParameter& out_target)
{
	if (!in_source1.IsLoaded() || !in_source2.IsLoaded())
	{
		return;
	}
	if (in_source1.GetCount()==out_target.GetCount() && in_source2.GetCount()==out_target.GetCount())
	{
		size_t start=std::max<size_t>(in_source1.GetFirstDef(), in_source2.GetFirstDef());
		size_t end=std::min<size_t>(in_source1.GetLastDef(), in_source2.GetLastDef());
		for (size_t i=start; i<end; ++i)
		{			
			out_target.GetItem(i)=in_source1.GetItem(i)-in_source2.GetItem(i);
			out_target.SetCorrupt(i, in_source1.IsCorrupt(i) || in_source2.IsCorrupt(i));
		}
	}
	else
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Не совпадают размеры входного и выходного массивов", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

///Масштабирование параметра
void ParameterScale(VParameter& in_source, VParameter& out_target, double in_aspect)
{
	if (!in_source.IsLoaded())
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Исходные данные не загружены", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (in_source.GetCount()==out_target.GetCount())
	{
		for (size_t i=0; i<in_source.GetCount(); ++i)
		{			
			out_target.GetItem(i)=in_source.GetItem(i)*in_aspect;
			out_target.SetCorrupt(i, in_source.IsCorrupt(i));
		}
	}
	else
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Не совпадают размеры входного и выходного массивов", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

///Смещение параметра
void ParameterShift(VParameter& in_source, VParameter& out_target, double in_value)
{
	if (!in_source.IsLoaded())
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Исходные данные не загружены", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
	if (in_source.GetCount()==out_target.GetCount())
	{
		for (size_t i=in_source.GetFirstDef(); i<=in_source.GetLastDef(); ++i)
		{			
			out_target.GetItem(i)=in_source.GetItem(i)+in_value;
			out_target.SetCorrupt(i, in_source.IsCorrupt(i));
		}
	}
	else
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Не совпадают размеры входного и выходного массивов", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

///Геометрическая сумма параметров
void ParameterGeoSum(VParameter& in_source1, VParameter& in_source2, VParameter& out_target)
{
	if (!in_source1.IsLoaded() || !in_source2.IsLoaded())
	{
		return;
	}
	if (in_source1.GetCount()==in_source2.GetCount() && in_source1.GetCount()==out_target.GetCount())
	{
		for (size_t i=0; i<in_source1.GetCount(); ++i)
		{			
			out_target.GetItem(i)=sqrt(pow(in_source1.GetItem(i),2) + pow(in_source2.GetItem(i),2));
		}
	}
	else
	{
		throw VSimpleException(L"Не удалось выполнить расчет параметра", 
			L"Не совпадают размеры входного и выходного массивов", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	}
}

// Вычисление коэффициента корреляции между двумя 
// параметрами с учётом сбоев и заданного смещения между параметрами
double CorrParams(const VParameter & prm1, const VParameter & prm2,			// параметры
						PARAMETER_TYPE prm1_t, PARAMETER_TYPE prm2_t,		// название параметров
						size_t offset,	size_t maxOffset)					// смещение одного параметра относительно другого
{	
	//Синхронизация по максимуму коэффициента корреляции
	size_t kadrCount = prm1.GetCount();
	double D1 = 0;
	double mo1 = 0;
	double D2 = 0;
	double mo2 = 0;
	double S = 0;
	size_t count = 0;
	long double minCorrValue = 0, corr, covar, val1, val2;
	
	for (size_t kadrNdx = 2*maxOffset; kadrNdx < kadrCount - 2*maxOffset; ++kadrNdx)
	{
		if (!prm1.IsCorrupt(kadrNdx) && !prm2.IsCorrupt(kadrNdx - maxOffset + offset))
		{
			mo1+=prm1.GetItem(kadrNdx);
			mo2+=prm2.GetItem(kadrNdx - maxOffset + offset);
			count++;
		}
	}
	mo1 /= count;
	mo2 /= count;
	for (size_t kadrNdx = 2*maxOffset; kadrNdx < kadrCount - 2*maxOffset; ++kadrNdx)
	{
		if (!prm1.IsCorrupt(kadrNdx) && !prm2.IsCorrupt(kadrNdx - maxOffset + offset))
		{
			val1 = prm1.GetItem(kadrNdx) - mo1;
			val2 = prm2.GetItem(kadrNdx - maxOffset + offset) - mo2;
			D1 += pow(val1, 2);
			D2 += pow(val2, 2);
			S+=(val1 * val2);
		}
	}

	covar = S / count;
	D1 /= count;
	D2 /= count;
	if(D1 > 0 && D2  > 0)
		corr = covar / (sqrt(D1)*sqrt(D2));
	else
		corr = 1;

	return corr;
}

//Статистические функции

///Расчет ковариации двух параметров
double cov(VParameter & in_param1, VParameter & in_param2)
{
	double E1=in_param1.GetMO();
	double E2=in_param2.GetMO();
	double S=0;
	for (size_t i=0; i<in_param1.GetCount(); ++i)
	{
		S+=(in_param1.GetItem(i)-E1) * (in_param2.GetItem(i)-E2);
	}
	return S/in_param1.GetCount();
}

///Расчет коэффициента корреляции (Пирсона)
double corr(VParameter & in_param1, VParameter & in_param2)
{
	if (in_param1.IsLoaded() && in_param2.IsLoaded() && in_param1.GetCount()==in_param2.GetCount())
		return cov(in_param1, in_param2)/(sqrt(in_param1.GetDisp()) * sqrt(in_param2.GetDisp()));
	else 
		return 0;
}

///Расчет коэффициента корреляции (Пирсона) по двум файлам
double corr(const VParameter& in_param1, const VAbsCoordData& in_abscoord1, 
			const VParameter & in_param2, const VAbsCoordData& in_abscoord2)
{
	if (in_param1.IsLoaded() && in_param2.IsLoaded() && in_abscoord1.IsLoaded() && in_abscoord2.IsLoaded() &&
		!(in_abscoord1.GetItem(0)>=in_abscoord2.GetLastItem() || in_abscoord2.GetItem(0)>=in_abscoord1.GetLastItem()))
	{
		//Находим общий участок
		double abs_start=std::max<double>(in_abscoord1.GetItem(in_param1.GetFirstDef()), 
			in_abscoord2.GetItem(in_param2.GetFirstDef()));
		double abs_end=std::min<double>(in_abscoord1.GetItem(in_param1.GetLastDef()), 
			in_abscoord2.GetItem(in_param2.GetLastDef()));
		//Один файл считаем опорным
		size_t start1=in_abscoord1.FindIndex(abs_start, true);
		size_t end1=in_abscoord1.FindIndex(abs_end, true);
		size_t start2=in_abscoord2.FindIndex(abs_start, true);
		size_t end2=in_abscoord2.FindIndex(abs_end, true);
		double E1=in_param1.GetMO(start1, end1);
		double E2=in_param2.GetMO(start2, end2);
		double D1=in_param1.GetDisp(start1, end1, E1);
		double D2=in_param2.GetDisp(start2, end2, E2);
		double COV=0;
#pragma omp parallel for reduction(+:COV)
		for (int i1=start1; i1<=end1; ++i1)
		{
			size_t i2=in_abscoord2.FindIndex(in_abscoord1.GetItem(i1), true);
			COV+=(in_param1.GetItem(i1)-E1) * (in_param2.GetItem(i2)-E2);
		}
		COV=COV/(end1-start1+1);
		return COV/(sqrt(D1) * sqrt(D2));
	}	
	else 
		return 0;
}

///Расчет коэффициента корреляции знаков (Фехнера)
double corr_sign(VParameter & in_param1, VParameter & in_param2)
{
	if (in_param1.IsLoaded() && in_param2.IsLoaded() && in_param1.GetCount()==in_param2.GetCount())
	{
		double E1=in_param1.GetMO();
		double E2=in_param2.GetMO();
		double U=0;
		double V=0;
		for (size_t i=0; i<in_param1.GetCount(); ++i)
		{
			if ((in_param1.GetItem(i)-E1)*(in_param2.GetItem(i)-E2) >= 0)
				++U;			
			else
				++V;
		}
		return (U-V)/(U+V);
	}
	else
		return 0;
}

// ---------------------------------------------------------------------------
/// VSpeedData
// ---------------------------------------------------------------------------

// загрузка из файла
template <class F>
bool VSpeedData::Load(F & file, uint32_t type)
{
	if (!VParameter::Load<F>(file, type))
	{
		VBasicParameter prm;
		if (!prm.Load(file, type, false))
		{
			this->message = ERRMSG_BLOCK_ACCESS_FAIL;
			this->SetLoaded(false);
			return false;
		}

		if (!prm.CheckCType<short>())
		{
			this->message = ERRMSG_BLOCK_INCORRECT;
			this->SetLoaded(false);
			return false;
		}

		this->Create(type, prm.GetRecCount(), 1, 0.0f);
		for (size_t i = 0; i < prm.GetRecCount(); ++i)
		{
			this->GetItem(i) = static_cast<float>(prm.GetItem<short>(i));
		}
	}
	this->message = ERRMSG_OK;
	this->SetLoaded(true);
	return true;
};

template bool VSpeedData::Load(VMainFile & file, uint32_t type);
template bool VSpeedData::Load(VRemoteFile & file, uint32_t type);
