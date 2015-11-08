#include "signal_processing.h"
#include "vlog.h"


//------------------------------------ ������ ������ VPoint ----------------------------------------------------

/// �����������
VPoint::VPoint() : point_t(0,0), corrupt(false) {};
/// �����������
VPoint::VPoint(double in_coord, double in_value) : point_t(in_coord, in_value), corrupt(false) {};
/// �����������
VPoint::VPoint(double in_coord, double in_value, bool in_corrupt) : point_t(in_coord, in_value), corrupt(in_corrupt){};
/// ���������� ����������
double VPoint::GetCoord() const 
{
	return x;
};
/// ���������� ��������
double VPoint::GetValue() const 
{
	return y;
};	
/// ���������� ������� ����
bool VPoint::GetCorrupt() const 
{
	return corrupt;
};	
/// ���������� ���������� �� ������ ����� FixMe!!! ����� ������� ��������� ��������
double VPoint::Distance(const VPoint& in_point) const 
{
	return fabs(GetCoord()-in_point.GetCoord());
};

//---------------------------------- ������ ������ VExtremum --------------------------------------------------

/// �����������
VExtremum::VExtremum(const VPoint& in_extremum) : extremum(in_extremum), corrupt_count(0){};	
/// �����������
VExtremum::VExtremum(const VPoint& in_extremum, const std::vector<VPoint>& in_extra) : 
	extremum(in_extremum), extra(in_extra) 
{
	struct point_corrupted
	{
		bool operator()(const VPoint& in_point) const
		{
			return in_point.GetCorrupt();
		};
	};
	corrupt_count = std::count_if(extra.begin(), extra.end(), point_corrupted());
};
/// ���������� ����������
double VExtremum::GetCoord() const 
{
	return extremum.GetCoord();
};
/// ���������� �������� ���������
double VExtremum::GetValue() const 
{
	return extremum.GetValue();
};
/// ���������� ���������
const VPoint& VExtremum::GetExtremum() const 
{
	return extremum;
};	
/// ���������� ���. �����
const std::vector<VPoint>& VExtremum::GetExtra() const 
{
	return extra;
};
/// ���������� ������� ����
bool VExtremum::GetCorrupt() const 
{
	return corrupt_count!=0;
};

//------------------------------- ������ ������ VWave --------------------------------------------------------

/// �����������
VWave::VWave(const VExtremum& in_start, const VExtremum& in_end) : start(in_start), end(in_end) {};	
/// ���������� ������
const VExtremum& VWave::GetStart() const 
{
	return start;
};
/// ���������� �����
const VExtremum& VWave::GetEnd() const 
{
	return end;
};
/// ���������� ��������
const double VWave::GetValue() const 
{
	return fabs(start.GetValue()-end.GetValue());
};
/// ���������� ����
const double VWave::GetSign() const 
{
	return start.GetValue()<end.GetValue() ? 1.0 : -1.0;
};
/// ���������� �����
const double VWave::GetLength() const 
{
	return fabs(start.GetCoord()-end.GetCoord());
};

/// ���������� ���. �����
const std::vector<VPoint>& VWave::GetExtra() const 
{
	return end.GetExtra();
};

/// ���������� ������������ �������� (� ������ ���. �����)
const double VWave::GetValue(double in_distance) const
{
	double distance=in_distance;
	VPoint start_point=GetStart().GetExtremum();
	VPoint end_point=GetEnd().GetExtremum();		

	// ��������� ��������� ��������� �� ���������� �� ������� ����������
	double value1=GetValue();		
	double value2=GetValue();								
	for (size_t i=1; i<GetExtra().size(); ++i)
	{			
		VPoint point1=GetExtra().at(i-1);
		VPoint point2=GetExtra().at(i);
		if (point1.Distance(start_point)<=distance && point2.Distance(start_point)>=distance  ||
			point1.Distance(start_point)>=distance && point2.Distance(start_point)<=distance )
		{
			value1=fabs(start_point.GetValue()-point1.GetValue());				
		}
		if (point1.Distance(end_point)<=distance && point2.Distance(end_point)>=distance  ||
			point1.Distance(end_point)>=distance && point2.Distance(end_point)<=distance )
		{
			value2=fabs(end_point.GetValue()-point2.GetValue());				
		}
	}					
	return std::max<double>(value1, value2);
}

/// ���������� ������� ����
bool VWave::GetCorrupt() const 
{
	return end.GetCorrupt();
};

//-------------------------------- ������ ������ VFindExcess ---------------------------------------------

/// �����������
VExcess::VExcess(const std::deque<VPoint>& in_data)
{
	start=in_data.front();
	end=in_data.back();
	max_value=start;
	min_value=start;
	sum=0;
	count=in_data.size();
	corrupt_count=0;
	for (size_t i=0; i<in_data.size(); ++i)
	{
		if (max_value.GetValue() < in_data.at(i).GetValue())
			max_value=in_data.at(i);
		if (min_value.GetValue() > in_data.at(i).GetValue())
			min_value=in_data.at(i);
		sum+=in_data.at(i).GetValue();
		if (in_data.at(i).GetCorrupt()) corrupt_count++;
	}
}
/// ���������� ���������� ������
double VExcess::GetStart() const 
{
	return start.GetCoord();
};
/// ���������� ���������� �����
double VExcess::GetEnd() const 
{
	return end.GetCoord();
};
/// ���������� ����������
double VExcess::GetCoord() const 
{
	return (GetStart()+GetEnd())/2;
};
/// ���������� ������������ ��������
double VExcess::GetMax() const 
{
	return max_value.GetValue();
};
/// ���������� �����������
double VExcess::GetMin() const 
{
	return min_value.GetValue();
};
/// ���������� ���������� ������������� ��������
double VExcess::GetMaxCoord() const
{
	return max_value.GetCoord();
};
/// ���������� ���������� ������������ ��������
double VExcess::GetMinCoord() const
{
	return min_value.GetCoord();
};
/// ���������� �������
double VExcess::GetAverage() const 
{
	return count>0 ? sum/count : 0;
};
/// ���������� �����
double VExcess::GetLength() const 
{
	return fabs(GetEnd()-GetStart());
};
/// ���������� ����� ��������
double VExcess::GetSum() const 
{
	return sum;
}
/// ���������� ���������� ������� ��������
double VExcess::GetCorruptCount() const 
{
	return corrupt_count;
};
/// ���������� ������� ����
bool VExcess::GetCorrupt() const 
{
	return corrupt_count!=0;
};

//------------------------------ ������ ������ VFindOtvod ------------------------------------------------

/// �����������
VFindOtvod::VFindOtvod(double in_base) : base(in_base){};
/// ����������
VFindOtvod::~VFindOtvod() 
{
	if (!Empty()) 
		LOG_ERROR(L"VFindOtvod: �������������� ������...");
};
/// �������� ������ �� ����
void VFindOtvod::Put(const VPoint& in_point)
{
	input.push_back(in_point);
	if (fabs(input.back().GetCoord()-input.front().GetCoord())>base)
	{
		double coord=input.at(input.size()/2).GetCoord();
		double value=(input.back().GetValue()-input.front().GetValue())/(input.back().GetCoord()-input.front().GetCoord());
		bool corrupt=input.back().GetCorrupt() || input.front().GetCorrupt();
		output.push(VPoint(coord, value, corrupt));
		input.pop_front();
	}
};

/// ���������� true, ���� ���� ������������ ������
bool VFindOtvod::Ready() const
{
	return output.size()>0;
};	

/// ���������� true, ���� ������������ ������ ���
bool VFindOtvod::Empty() const
{
	return output.empty();
};		

/// ���������� ������ ������� ������
const VPoint& VFindOtvod::Front() const
{
	return output.front();
};

/// ����������� ������ ������� ������
void VFindOtvod::Pop()
{
	output.pop();
};

/// ������������� ����� ����
void VFindOtvod::SetBase(double in_base)
{
	base=in_base;	
};

//-------------------------------- ������ ������ VFindExtremum ---------------------------------------------

/// ���������� true, ���� �������� �������� ������ - �������� �����������
bool VFindExtremum::IsExtremum() const
{
	VPoint center=input.at(input.size()/2);	
	size_t l=input.size()/2-1;
	size_t r=input.size()/2+1;
	while (l>=0 && r<input.size())
	{		
		double min_value=std::min<double>(input.at(l).GetValue(), input.at(r).GetValue());
		double max_value=std::max<double>(input.at(l).GetValue(), input.at(r).GetValue());		
		if (min_value==max_value && min_value==center.GetValue() || pro::interval<double>(min_value, max_value, false, false).contains(center.GetValue()))
			return false;
		l--;
		r++;
	}
	return true;
};

/// �����������
VFindExtremum::VFindExtremum(double in_base) : base(in_base){};

/// ����������
VFindExtremum::~VFindExtremum() 
{
	if (!Empty()) 
		LOG_ERROR(L"VFindExtremum: �������������� ������...");
};

/// ���� ������
void VFindExtremum::Put(const VPoint& in_point)
{
	input.push_back(in_point);	
	if (fabs(input.back().GetCoord()-input.front().GetCoord())>=base && input.size()>=3 && is_odd(input.size()))
	{			
		VPoint center=input.at(input.size()/2);
		extra.push_back(center);		
		if (IsExtremum())
		{
			output.push(VExtremum(center, extra));
			extra.clear();
		}		
		input.pop_front();
	}
};	
/// ���������� true, ���� ���� ������������ ������
bool VFindExtremum::Ready() const
{
	return output.size()>0;
};	
/// ���������� true, ���� ������������ ������ ���
bool VFindExtremum::Empty() const
{
	return output.empty();
};		
/// ���������� ������ ������� ������
const VExtremum& VFindExtremum::Front() const
{
	return output.front();
};
/// ����������� ������ ������� ������
void VFindExtremum::Pop()
{
	output.pop();
};


//-------------------------------- ������ ������ VDelay ----------------------------------------------------


/// �����������
VDelay::VDelay(double in_base) : base(in_base){};
/// ����������
VDelay::~VDelay() 
{
	if (!Empty()) 
		LOG_ERROR(L"VDelay: �������������� ������...");
};
/// ���� ������
void VDelay::Put(const VPoint& in_point)
{
	input.push_back(in_point);
	if (fabs(input.back().GetCoord()-input.front().GetCoord())>base)
	{			
		VPoint center=input.at(input.size()/2);			
		output.push(center);
		input.pop_front();
	}
};	
/// ���������� true, ���� ���� ������������ ������
bool VDelay::Ready() const
{
	return output.size()>0;
};	
/// ���������� true, ���� ������������ ������ ���
bool VDelay::Empty() const
{
	return output.empty();
};		
/// ���������� ������ ������� ������
const VPoint& VDelay::Front() const
{
	return output.front();
};
/// ����������� ������ ������� ������
void VDelay::Pop()
{
	output.pop();
};

//----------------------------------- ������ ������ VMean ---------------------------------------------------

/// �����������
VMean::VMean(double in_base) : base(in_base), sum(0), bad_count(0){};	
/// ����������
VMean::~VMean() {if (!Empty()) LOG_ERROR(L"VMean: �������������� ������...");};
/// ���� ������
void VMean::Put(const VPoint& in_point)
{						
	input.push_back(in_point);				
	if (!input.empty())
	{
		sum+=input.back().GetValue();
		if (input.back().GetCorrupt())
			bad_count++;
		if (fabs(input.back().GetCoord()-input.front().GetCoord())>base)
		{			
			VPoint center=input.at(input.size()/2);			
			output.push(VPoint(center.GetCoord(), sum/input.size(), bad_count>0));
			sum-=input.front().GetValue();
			if (input.front().GetCorrupt())
				bad_count--;
			input.pop_front();
		}
	}		
};	
/// ���������� true, ���� ���� ������������ ������
bool VMean::Ready() const
{
	return output.size()>0;
};	
/// ���������� true, ���� ������������ ������ ���
bool VMean::Empty() const
{
	return output.empty();
};		
/// ���������� ������ ������� ������
const VPoint& VMean::Front() const
{
	return output.front();
};
/// ����������� ������ ������� ������
void VMean::Pop()
{
	output.pop();
};

/// ������ ����
void VMean::SetBase(double in_base)
{
	base=in_base;
};

//-------------------------------------- ������ ������ VMeanVariate -------------------------------------------

/// �����������
VMeanVariate::VMeanVariate(double in_base) : base(in_base), sum(0){};
/// ����������
VMeanVariate::~VMeanVariate() 
{
	if (!Empty()) 
		LOG_ERROR(L"VMeanVariate: �������������� ������...");
};
/// ���� ������
void VMeanVariate::Put(const VPoint& in_point)
{
	if (!in_point.GetCorrupt())
		input.push_back(in_point);
	else if (!input.empty())
	{
		input.push_back(input.back());			
	}
	if (!input.empty())
	{
		sum+=input.back().GetValue();
		if (fabs(input.back().GetCoord()-input.front().GetCoord())>base)
		{			
			VPoint center=input.at(input.size()/2);			
			output.push(VPoint(center.GetCoord(), center.GetValue()-(sum/input.size())));
			sum-=input.front().GetValue();
			input.pop_front();
		}
	}
};	
/// ���������� true, ���� ���� ������������ ������
bool VMeanVariate::Ready() const
{
	return output.size()>0;
};	
/// ���������� true, ���� ������������ ������ ���
bool VMeanVariate::Empty() const
{
	return output.empty();
};		
/// ���������� ������ ������� ������
const VPoint& VMeanVariate::Front() const
{
	return output.front();
};
/// ����������� ������ ������� ������
void VMeanVariate::Pop()
{
	output.pop();
};

//--------------------------------------- ������ ������ VMedianVariate -------------------------------

/// �����������
VMedianVariate::VMedianVariate(double in_base) : base(in_base){};
/// ����������
VMedianVariate::~VMedianVariate() 
{
	if (!Empty()) 
		LOG_ERROR(L"VMedianVariate: �������������� ������...");
};
/// ���� ������
void VMedianVariate::Put(const VPoint& in_point)
{
	input.push_back(in_point);
	if (fabs(input.back().GetCoord()-input.front().GetCoord())>base)
	{				
		sorted.reserve(input.size());
		sorted.resize(input.size());			
		for (size_t i=0; i<input.size(); ++i)
		{
			sorted[i]=input.at(i).GetValue();
		}			
		std::nth_element(sorted.begin(), sorted.begin()+(sorted.size()/2), sorted.end());
		output.push(VPoint(input.at(input.size()/2).GetCoord(), input.at(input.size()/2).GetValue() - sorted.at(sorted.size()/2)));
		input.pop_front();
	}
};	
/// ���������� true, ���� ���� ������������ ������
bool VMedianVariate::Ready() const
{
	return output.size()>0;
};	
/// ���������� true, ���� ������������ ������ ���
bool VMedianVariate::Empty() const
{
	return output.empty();
};		
/// ���������� ������ ������� ������
const VPoint& VMedianVariate::Front() const
{
	return output.front();
};
/// ����������� ������ ������� ������
void VMedianVariate::Pop()
{
	output.pop();
};

