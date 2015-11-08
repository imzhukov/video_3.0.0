#pragma once

#include "calclib.h"
#include <stack>
#include <boost/lexical_cast.hpp>

#include "Shiny.h"

using namespace boost;
using namespace std;

class VDataSet;

///Операция над VParameter
struct VOper
{
	wstring name;
	VParameter * par1;
	VParameter * par2;
	VParameter * out;
	double par_add[2];
};

///Разбор строки функции над VParameter
class VFunctionParser
{
private:
	///Источник данных
	VDataSet * DataSet;
	///Результат
	VParameter * Result;
	///Вектор временных параметров
	vector<VParameter *> vect;

	///Функция, преобразующая строку с параметрами, разделёнными запятыми в вектор строк
	void SplitParam(wstring param, vector<wstring> & result)
	{
		while(param!=L"")
		{
			size_t comma=param.find_first_of(L",");
			if (comma!=std::string::npos)
			{
				//если перед следующей запятой есть скобка, значит параметр является функцией
				size_t brak=param.find_first_of(L"(");
				if (brak==string::npos || comma<brak)
				{
					wstring push=param.substr(0,comma);//-1 S.Markin
					if (push!=L"")
						result.push_back(push);
					param=param.substr(comma+1);
				}
				else
				{
					size_t rbrak=brak;
					int brak_count=1;
					while (brak_count!=0)
					{
						rbrak++;
						if (rbrak>=param.size())
						{
							throw VSimpleException(L"Отсутствует закрывающая скобка", param, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
						}
						if (param.substr(rbrak,1)==L"(")
							brak_count++;
						else if (param.substr(rbrak,1)==L"")
							brak_count--;
					}
					result.push_back(param.substr(0,rbrak+1));
					if (rbrak!=param.size()-1)
					{
						param=param.substr(rbrak+1);
						if (param.substr(0,1)==L",")
							param=param.substr(1);
					}
					else
						param=L"";
				}
			}
			else
			{
				result.push_back(param);
				break;
			}
		}
	}

	///Расчёт параметра
	void Calc(VOper & oper)
	{
		VAbsCoordData & absCoord=DataSet->GetAbsCoord();
		oper.out = new VParameter;
		vect.push_back(oper.out);
		oper.out->Create(0xFFFFFFFF,oper.par1->GetCount(),0,0);
		if (oper.name==L"MEAN")
		{
			ParameterMean(*(oper.par1),*(oper.out),absCoord,oper.par_add[0]);
		}
		else if (oper.name==L"WMA")
		{
			ParameterWeightedMean(*(oper.par1),*(oper.out),absCoord,oper.par_add[0]);
		}
		else if (oper.name==L"SPECTRUM")
		{
			ParameterSpectrum(*(oper.par1),*(oper.out),absCoord);
		}
		else if (oper.name==L"PHASE")
		{
			ParameterPhase(*(oper.par1),*(oper.out),absCoord);
		}
		else if (oper.name==L"LOW_PASS_FIR")
		{
			double cut_off_freq=absCoord.GetLengthStep()/(2*oper.par_add[0]);
			ParameterFIRFilter(*(oper.par1),*(oper.out),absCoord, VFIRFilterPtr(new VLowPassFIR(cut_off_freq)));
		}
		else if (oper.name==L"HIGH_PASS_FIR")
		{
			double cut_off_freq=absCoord.GetLengthStep()/(2*oper.par_add[0]);
			ParameterFIRFilter(*(oper.par1),*(oper.out),absCoord, VFIRFilterPtr(new VHighPassFIR(cut_off_freq)));
		}
		else if (oper.name==L"BAND_PASS_FIR")
		{
			double low=absCoord.GetLengthStep()/(2*oper.par_add[1]);
			double high=absCoord.GetLengthStep()/(2*oper.par_add[0]);
			ParameterFIRFilter(*(oper.par1),*(oper.out),absCoord, VFIRFilterPtr(new VBandPassFIR(low, high)));
		}
		else if (oper.name==L"BAND_STOP_FIR")
		{
			double low=absCoord.GetLengthStep()/(2*oper.par_add[1]);
			double high=absCoord.GetLengthStep()/(2*oper.par_add[0]);
			ParameterFIRFilter(*(oper.par1),*(oper.out),absCoord, VFIRFilterPtr(new VBandStopFIR(low, high)));
		}
		else if (oper.name==L"KALMAN")
		{
			double Q=absCoord.GetLengthStep()/(2*oper.par_add[1]);
			double R=absCoord.GetLengthStep()/(2*oper.par_add[0]);
			ParameterKalman(*(oper.par1),*(oper.out), Q, R);
		}
		else if (oper.name==L"SKO")
		{
			ParameterSKO(*(oper.par1),*(oper.out),absCoord,oper.par_add[0]);
		}
		else if (oper.name==L"MEDIAN")
		{
			ParameterMedian(*(oper.par1),*(oper.out),absCoord,oper.par_add[0]);
		}
		else if (oper.name==L"OTVOD")
		{
			ParameterOtvod(*(oper.par1),*(oper.out),absCoord,oper.par_add[0]);
		}
		else if (oper.name==L"DERIVATE")
		{
			ParameterDerivate(*(oper.par1),*(oper.out),absCoord,oper.par_add[0]);
		}
		else if (oper.name==L"SHARP")
		{
			ParameterSharp(*(oper.par1),*(oper.out),oper.par_add[0]);
		}		
		else if (oper.name==L"DIFF")
		{
			ParameterDiff(*(oper.par1),*(oper.par2),*(oper.out));
		}
		else if (oper.name==L"GEO_SUM")
		{
			ParameterGeoSum(*(oper.par1),*(oper.par2),*(oper.out));
		}
		else if (oper.name==L"SCALE")
		{
			ParameterScale(*(oper.par1),*(oper.out),oper.par_add[0]);
		}
		else if (oper.name==L"SHIFT")
		{
			ParameterShift(*(oper.par1),*(oper.out),oper.par_add[0]);
		}
	}

	///Разбор строки в стек
	void StackParse(wstring in_command, stack<VOper> & oper_stack)
	{
		boost::iterator_range<std::wstring::iterator> com=boost::algorithm::find_first(in_command, L"(");
		boost::iterator_range<std::wstring::iterator> param=boost::algorithm::find_last(in_command, L"");
		wstring command=std::wstring(in_command.begin(), com.begin());
		wstring parameters=wstring(com.end(),param.begin());

		boost::algorithm::trim(command);
		boost::algorithm::trim(parameters);

		std::vector<wstring> split_vec;
		VOper oper;

		oper.name=command;

		//Параметр из файла
		if (command==L"PARAMETER")
		{
			PARAMETER_TYPE data_type=GetParamCode(parameters.c_str());
			if (data_type==PRM_NONE)
			{
				throw VSimpleException(L"Неизвестный параметр", parameters, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			if (!DataSet->IsLoaded(data_type))
				DataSet->Load(data_type);
			oper.out=&(DataSet->GetParameter(data_type));
			oper_stack.push(oper);
		}
		else if (command==L"SKIP_CURVES")///FixMe!!! было нужно БНЗ. Убрать, когда надобность отпадет
		{
			SplitParam(parameters,split_vec);
			if (split_vec.size()!=1)
			{
				throw VSimpleException(L"Неверное количество параметров функции", command, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			boost::iterator_range<std::wstring::iterator> f=boost::algorithm::find_first(split_vec.at(0),L"(");
			if (f)
			{
				StackParse(split_vec.at(0),oper_stack);
				VOper oper1=oper_stack.top();
				oper_stack.pop();
				oper.par1=oper1.out;
			}
			else
			{
				PARAMETER_TYPE data_type=GetParamCode(split_vec.at(0).c_str());
				if (data_type==PRM_NONE)
				{
					throw VSimpleException(L"Неизвестный параметр", parameters, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				}
				if (!DataSet->IsLoaded(data_type))
					DataSet->Load(data_type);
				oper.par1=&(DataSet->GetParameter(data_type));
			}
			if (!DataSet->IsLoaded(PRM_CURVE_MARKS))
				DataSet->Load(PRM_CURVE_MARKS);
			oper.out = new VParameter;
			oper.out->Create(0xFFFFFFFF,oper.par1->GetCount(),0,0);
#pragma omp parallel for			
			for (int i=0; i<oper.out->GetCount(); ++i)
			{
				if (DataSet->GetCurves().FindCrv(i)!=BAD_INDEX)
				{
					oper.out->GetItem(i)=0;
				}
				else
					oper.out->GetItem(i)=oper.par1->GetItem(i);
			}
			oper_stack.push(oper);
			/*
			PARAMETER_TYPE data_type=GetParamCode(parameters.c_str());
			if (data_type==PRM_NONE)
			{
				throw VSimpleException(L"Неизвестный параметр", parameters, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			if (!DataSet->IsLoaded(data_type))
				DataSet->Load(data_type);
			oper.par1=&(DataSet->GetParameter(data_type));
			if (!DataSet->IsLoaded(PRM_CURVE_MARKS))
				DataSet->Load(PRM_CURVE_MARKS);
			oper.out = new VParameter;
			oper.out->Create(0xFFFFFFFF,oper.par1->GetCount(),0,0);
#pragma omp parallel for			
			for (int i=0; i<oper.out->GetCount(); ++i)
			{
				if (DataSet->GetCurves().FindCrv(i)!=BAD_INDEX)
				{
					oper.out->GetItem(i)=0;
				}
				else
					oper.out->GetItem(i)=oper.par1->GetItem(i);
			}
			
			oper_stack.push(oper);
			*/
		}
		else if (command==L"SPECTRUM" || command==L"PHASE")
		{
			SplitParam(parameters,split_vec);
			if (split_vec.size()!=1)
			{
				throw VSimpleException(L"Неверное количество параметров функции", command, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			boost::iterator_range<std::wstring::iterator> f=boost::algorithm::find_first(split_vec.at(0),L"(");
			if (f)
			{
				StackParse(split_vec.at(0),oper_stack);
				VOper oper1=oper_stack.top();
				oper_stack.pop();
				oper.par1=oper1.out;
			}
			else
			{
				PARAMETER_TYPE data_type=GetParamCode(split_vec.at(0).c_str());
				if (data_type==PRM_NONE)
				{
					throw VSimpleException(L"Неизвестный параметр", parameters, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				}
				if (!DataSet->IsLoaded(data_type))
					DataSet->Load(data_type);
				oper.par1=&(DataSet->GetParameter(data_type));
			}
			Calc(oper);
			oper_stack.push(oper);	
		}
		//функции с одним параметром и одной константой
		else if (command==L"MEAN" || command==L"WMA" || command==L"SKO" || command==L"MEDIAN" || command==L"OTVOD" || command==L"DERIVATE" || command==L"SHARP" 
			|| command==L"LOW_PASS_FIR" || command==L"HIGH_PASS_FIR")
		{
			SplitParam(parameters,split_vec);
			if (split_vec.size()!=2)
			{
				throw VSimpleException(L"Неверное количество параметров функции", command, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			boost::iterator_range<std::wstring::iterator> f=boost::algorithm::find_first(split_vec.at(0),L"(");
			if (f)
			{
				StackParse(split_vec.at(0),oper_stack);
				VOper oper1=oper_stack.top();
				oper_stack.pop();
				oper.par1=oper1.out;
			}
			else
			{
				PARAMETER_TYPE data_type=GetParamCode(split_vec.at(0).c_str());
				if (data_type==PRM_NONE)
				{
					throw VSimpleException(L"Неизвестный параметр", parameters, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				}
				//if (!DataSet->GetParameter(data_type).IsLoaded())
				if (!DataSet->IsLoaded(data_type))
					DataSet->Load(data_type);
				oper.par1=&(DataSet->GetParameter(data_type));
			}
			oper.par_add[0]=str_to_double(split_vec.at(1).c_str());
			Calc(oper);
			oper_stack.push(oper);
		}
		//Разность - функция с двумя параметрами
		else if (command==L"DIFF")
		{
			SplitParam(parameters,split_vec);
			if (split_vec.size()!=2)
			{
				throw VSimpleException(L"Неверное количество параметров функции", command, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			for (size_t i=0;i<2;i++)
			{
				boost::iterator_range<std::wstring::iterator> f=boost::algorithm::find_first(split_vec.at(i),L"(");
				if (f)
				{
					StackParse(split_vec.at(i),oper_stack);
					VOper oper1=oper_stack.top();
					oper_stack.pop();
					if (i==0)
						oper.par1=oper1.out;
					else
						oper.par2=oper1.out;
				}
				else
				{
					PARAMETER_TYPE data_type=GetParamCode(split_vec.at(i).c_str());
					if (data_type==PRM_NONE)
					{
						throw VSimpleException(L"Неизвестный параметр", parameters, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
					}
					//if (!DataSet->GetParameter(data_type).IsLoaded())
					if (!DataSet->IsLoaded(data_type))
						DataSet->Load(data_type);
					if (i==0)
						oper.par1=&(DataSet->GetParameter(data_type));
					else
						oper.par2=&(DataSet->GetParameter(data_type));
				}
			}
			Calc(oper);
			oper_stack.push(oper);
		}
		//Линейный фильтр - один параметр и две константы
		else if (command==L"LINEAR_FILTER" || command==L"BAND_STOP_FIR" || command==L"BAND_PASS_FIR" || command==L"KALMAN")
		{
			SplitParam(parameters,split_vec);
			if (split_vec.size()!=3)
			{
				throw VSimpleException(L"Неверное количество параметров функции", command, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			boost::iterator_range<std::wstring::iterator> f=boost::algorithm::find_first(split_vec.at(0),L"(");
			if (f)
			{
				StackParse(split_vec.at(0),oper_stack);
				VOper oper1=oper_stack.top();
				oper_stack.pop();
				oper.par1=oper1.out;
			}
			else
			{
				PARAMETER_TYPE data_type=GetParamCode(split_vec.at(0).c_str());
				if (data_type==PRM_NONE)
				{
					throw VSimpleException(L"Неизвестный параметр", parameters, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				}
				//if (!DataSet->GetParameter(data_type).IsLoaded())
				if (!DataSet->IsLoaded(data_type))
					DataSet->Load(data_type);
				oper.par1=&(DataSet->GetParameter(data_type));
			}
			oper.par_add[0]=str_to_double(split_vec.at(1).c_str());
			oper.par_add[1]=str_to_double(split_vec.at(2).c_str());
			Calc(oper);
			oper_stack.push(oper);
		}
		//Геометрическая сумма - функция с двумя параметрами
		else if (command==L"GEO_SUM")
		{
			SplitParam(parameters,split_vec);
			if (split_vec.size()!=2)
			{
				throw VSimpleException(L"Неверное количество параметров функции", command, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			for (size_t i=0;i<2;i++)
			{
				boost::iterator_range<std::wstring::iterator> f=boost::algorithm::find_first(split_vec.at(i),L"(");
				if (f)
				{
					StackParse(split_vec.at(i),oper_stack);
					VOper oper1=oper_stack.top();
					oper_stack.pop();
					if (i==0)
						oper.par1=oper1.out;
					else
						oper.par2=oper1.out;
				}
				else
				{
					PARAMETER_TYPE data_type=GetParamCode(split_vec.at(i).c_str());
					if (data_type==PRM_NONE)
					{
						throw VSimpleException(L"Неизвестный параметр", parameters, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
					}
					//if (!DataSet->GetParameter(data_type).IsLoaded())
					if (!DataSet->IsLoaded(data_type))
						DataSet->Load(data_type);
					if (i==0)
						oper.par1=&(DataSet->GetParameter(data_type));
					else
						oper.par2=&(DataSet->GetParameter(data_type));
				}
			}
			Calc(oper);
			oper_stack.push(oper);
		}
		//Масштабирование - один параметр и одна константа
		else if (command==L"SCALE" || command==L"SHIFT")
		{
			SplitParam(parameters,split_vec);
			if (split_vec.size()!=2)
			{
				throw VSimpleException(L"Неверное количество параметров функции", command, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
			}
			boost::iterator_range<std::wstring::iterator> f=boost::algorithm::find_first(split_vec.at(0),L"(");
			if (f)
			{
				StackParse(split_vec.at(0),oper_stack);
				VOper oper1=oper_stack.top();
				oper_stack.pop();
				oper.par1=oper1.out;
			}
			else
			{
				PARAMETER_TYPE data_type=GetParamCode(split_vec.at(0).c_str());
				if (data_type==PRM_NONE)
				{
					throw VSimpleException(L"Неизвестный параметр", parameters, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
				}
				//if (!DataSet->GetParameter(data_type).IsLoaded())
				if (!DataSet->IsLoaded(data_type))
					DataSet->Load(data_type);
				oper.par1=&(DataSet->GetParameter(data_type));
			}
			oper.par_add[0]=str_to_double(split_vec.at(1).c_str());			
			Calc(oper);
			oper_stack.push(oper);
		}
		else
		{
			throw VSimpleException(L"Неизвестная функция", command, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}


public:
	///Конструктор класса VFunctionParser
	VFunctionParser(VDataSet * dataSet)
	{
		DataSet=dataSet;
		if (!DataSet->GetAbsCoord().IsLoaded())
			DataSet->Load(PRM_ABS_COORD);
	}

	~VFunctionParser()
	{
		for (int i=vect.size()-1; i>=0; i--)
			delete vect.at(i);
	}

	///Разбор строки, возвращающий VParameter
	void Parse(wstring in_command, VParameter & param)
	{
		stack<VOper> oper_stack;
		StackParse(in_command, oper_stack);
		VOper oper=oper_stack.top();
		oper_stack.pop();
		param.Create(0xFFFFFFFF,oper.out->GetCount(),0,0);
		for (size_t i=0; i<oper.out->GetCount(); i++)
		{
			param.GetItem(i)=oper.out->GetItem(i);
			param.SetCorrupt(i, oper.out->IsCorrupt(i));
		}
	}
};