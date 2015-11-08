#pragma once

#include "mathlib.h"
#include "processing.h"

/// ���� ������
class VPoint : public point_t
{
private:
	/// ������� ����
	bool corrupt;	
public:
	/// �����������
	VPoint();
	/// �����������
	VPoint(double in_coord, double in_value);
	/// �����������
	VPoint(double in_coord, double in_value, bool in_corrupt);
	/// ���������� ����������
	double GetCoord() const;
	/// ���������� ��������
	double GetValue() const;	
	/// ���������� ������� ����
	bool GetCorrupt() const;	
	/// ���������� ���������� �� ������ ����� FixMe!!! ����� ������� ��������� ��������
	double Distance(const VPoint& in_point) const;
};


/// ���������
class VExtremum
{
private:
	/// ���������
	VPoint extremum;	
	/// ���. ����� (����� ����� ������ ����������� � ����������)
	std::vector<VPoint> extra;
	/// ������� ������� �����
	int corrupt_count;
public:
	/// �����������
	VExtremum(const VPoint& in_extremum);	
	/// �����������
	VExtremum(const VPoint& in_extremum, const std::vector<VPoint>& in_extra);
	/// ���������� ����������
	double GetCoord() const;
	/// ���������� �������� ���������
	double GetValue() const;		
	/// ���������� ���������
	const VPoint& GetExtremum() const;	
	/// ���������� ���. �����
	const std::vector<VPoint>& GetExtra() const;
	/// ���������� ������� ����
	bool GetCorrupt() const;
};

/// ���� �����������
class VWave
{
private:
	/// ������
	VExtremum start;
	/// �����
	VExtremum end;
public:
	/// �����������
	VWave(const VExtremum& in_start, const VExtremum& in_end);	
	/// ���������� ������
	const VExtremum& GetStart() const;
	/// ���������� �����
	const VExtremum& GetEnd() const;
	/// ���������� ��������
	const double GetValue() const;
	/// ���������� ����
	const double GetSign() const;
	/// ���������� �����
	const double GetLength() const;
	/// ���������� ���. �����
	const std::vector<VPoint>& GetExtra() const;
	/// ���������� ������������ �������� (� ������ ���. �����)
	const double GetValue(double in_distance) const;	
	/// ���������� ������� ����
	bool GetCorrupt() const;
};

/// ���������� ������
class VExcess
{
protected:
	/// ����� ������
	VPoint start;
	/// ����� �����
	VPoint end;	
	/// ����� ���������
	VPoint max_value;
	/// ����� ��������
	VPoint min_value;
	/// ����� ��������
	double sum;
	/// ���������� ��������
	int count;
	/// ���������� ������� ��������
	int corrupt_count;
public:
	/// �����������
	VExcess(const std::deque<VPoint>& in_data);
	/// ���������� ���������� ������
	double GetStart() const;
	/// ���������� ���������� �����
	double GetEnd() const;
	/// ���������� ����������
	double GetCoord() const;
	/// ���������� ������������ ��������
	double GetMax() const;
	/// ���������� �����������
	double GetMin() const;
	/// ���������� ���������� ������������� ��������
	double GetMaxCoord() const;
	/// ���������� ���������� ������������ ��������
	double GetMinCoord() const;
	/// ���������� �������
	double GetAverage() const;
	/// ���������� �����
	double GetLength() const;
	/// ���������� ����� ��������
	double GetSum() const;
	/// ���������� ���������� ������� ��������
	double GetCorruptCount() const;
	/// ���������� ������� ����
	bool GetCorrupt() const;
};

/// ��������� �����
class VFindOtvod : public VDataProcessor<VPoint, VPoint>
{
private:
	/// ����
	double base;	
	/// ������� ������ �� �����
	std::deque<VPoint> input;
	/// ������� ������ �� ������
	std::queue<VPoint> output;
public:	
	/// �����������
	VFindOtvod(double in_base);
	/// ����������
	~VFindOtvod();
	/// �������� ������ �� ����
	virtual void Put(const VPoint& in_point);
	/// ���������� true, ���� ���� ������������ ������
	virtual bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VPoint& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();
	/// ������������� ����� ����
	void SetBase(double in_base);
};


/// ������� ����������
class VFindExtremum : public VDataProcessor<VPoint, VExtremum>
{
private:
	/// ����
	double base;	
	/// ������� ������ �� �����
	std::deque<VPoint> input;
	/// ������� ������ �� ������
	std::queue<VExtremum> output;
	/// ������� �������������� �����
	std::vector<VPoint> extra;

	/// ���������� true, ���� �������� �������� ������ - �������� �����������
	bool IsExtremum() const;
	
public:
	/// �����������
	VFindExtremum(double in_base);
	/// ����������
	~VFindExtremum();
	/// ���� ������
	void Put(const VPoint& in_point);	
	/// ���������� true, ���� ���� ������������ ������
	virtual bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VExtremum& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();
};

/// ��������
class VDelay : public VDataProcessor<VPoint, VPoint>
{
private:
	/// ����
	double base;	
	/// ������� ������ �� �����
	std::deque<VPoint> input;
	/// ������� ������ �� ������
	std::queue<VPoint> output;
public:
	/// �����������
	VDelay(double in_base);
	/// ����������
	~VDelay();
	/// ���� ������
	void Put(const VPoint& in_point);	
	/// ���������� true, ���� ���� ������������ ������
	virtual bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VPoint& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();
};

/// ��������� ���������� �������
class VMean : public VDataProcessor<VPoint, VPoint>
{
private:
	/// ����
	double base;
	/// �����
	double sum;
	/// ���������� ������� ������
	int bad_count;
	/// ������� ������ �� �����
	std::deque<VPoint> input;
	/// ������� ������ �� ������
	std::queue<VPoint> output;
public:
	/// �����������
	VMean(double in_base);	
	/// ����������
	~VMean();
	/// ���� ������
	void Put(const VPoint& in_point);	
	/// ���������� true, ���� ���� ������������ ������
	virtual bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VPoint& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();
	/// ������ ����
	void SetBase(double in_base);
};


/// �������� ��������� ������������ (������ - ���������� �������)
class VMeanVariate : public VDataProcessor<VPoint, VPoint>
{
private:
	/// ����
	double base;	
	/// �������
	double sum;
	/// ������� ������ �� �����
	std::deque<VPoint> input;
	/// ������� ������ �� ������
	std::queue<VPoint> output;
public:
	/// �����������
	VMeanVariate(double in_base);
	/// ����������
	~VMeanVariate();
	/// ���� ������
	void Put(const VPoint& in_point);	
	/// ���������� true, ���� ���� ������������ ������
	virtual bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VPoint& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();
};

/// �������� ��������� ������������ (������ - �������)
class VMedianVariate : public VDataProcessor<VPoint, VPoint>
{
private:
	/// ����
	double base;	
	/// ������� ������ �� �����
	std::deque<VPoint> input;
	/// ������� ������ �� ������
	std::queue<VPoint> output;
	/// ������ ��� ����������
	std::vector<double> sorted;
public:
	/// �����������
	VMedianVariate(double in_base);
	/// ����������
	~VMedianVariate();
	/// ���� ������
	void Put(const VPoint& in_point);	
	/// ���������� true, ���� ���� ������������ ������
	virtual bool Ready() const;	
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const;		
	/// ���������� ������ ������� ������
	virtual const VPoint& Front() const;
	/// ����������� ������ ������� ������
	virtual void Pop();
};

