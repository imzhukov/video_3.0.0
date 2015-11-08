#pragma once

#include "mathlib.h"
#include "singleton.h"
#include "interval.h"
#include "piecewise.h"

/// ������� �����������
enum DEGREE
{
	DEGREE_NONE=0,
	DEGREE1=1,
	DEGREE2=2,
	DEGREE3=3,
	DEGREE4=4
};

/// ���������� ����� �� (FixMe!!! ��� ������ ����, ��� �� � ����� ����� �����. ��� ������ ������������ ����� �������������)
#define NORMA_WIDTH_1520 1520.0
#define NORMA_WIDTH_1524 1524.0
#define NORMA_WIDTH_1530 1530.0
#define NORMA_WIDTH_1535 1535.0
#define NORMA_WIDTH_1540 1540.0


/// ����� ��������
#define PIT_LENGTH 6.0
/// ������������ ����� ��������
#define PIT_MAX_LENGTH 10.0
/// �������� ���� ��������
#define PIT_LENGTH_RANGE pro::interval<double>(0, PIT_LENGTH, true, true)


/// ����� ��������
#define DISTORTION_LENGTH 20.0
/// �������� ���� ��������� 
#define DISTORTION_LENGTH_RANGE pro::interval<double>(0,DISTORTION_LENGTH, false, true)
/// �������� ���� ���������
#define SHORT_DISTORTION_LENGTH_RANGE pro::interval<double>(0,10, false, true)
/// �������� ���� ���������
#define LONG_DISTORTION_LENGTH_RANGE pro::interval<double>(10,DISTORTION_LENGTH, false, true)
/// ������������ ����� ��������
#define DISTORTION_MAX_LENGTH 40.0

/// ������������ ����� ��������
#define RICH_MAX_LENGTH 40.0
/// �������� ���� ����������� �� ��������
#define RICH_LENGTH_RANGE pro::interval<double>(0.0,20.0, false, true)
/// �������� ���� ����������� �� ��������
#define LONG_RICH_LENGTH_RANGE pro::interval<double>(20.0, 40.0, false, true)
/// �������� ���� ����������� �� ��������
#define VERY_LONG_RICH_LENGTH_RANGE pro::interval<double>(40.0, 60.0, false, true)


/// ������ ������� �������� ��
class VNarrowRecord
{
private:
	/// �������
	DEGREE degree;
	/// �������� ���������
	pro::interval<int> speed;
	/// ����� ��
	double norma;
	/// �������� ��������
	pro::interval<double> value;
public:

	/// ������� ��� ������
	class VFindKey
	{
	private:
		DEGREE degree;
		int speed;
		double norma;
	public:
		VFindKey(DEGREE in_degree, int in_speed, double in_norma) : degree(in_degree), speed(in_speed), norma(in_norma){};
		bool operator()(const VNarrowRecord& in_key) const
		{
			return in_key.IsKey(degree, speed, norma);
		}
	};

	/// �����������
	VNarrowRecord();	
	/// �����������
	VNarrowRecord(DEGREE in_degree, const pro::interval<int>& in_speed, double in_norma, const pro::interval<double>& in_value);	
	/// ���������� true, ���� ������� ������ ������������� �����
	bool IsKey(DEGREE in_degree, int in_speed, double in_norma) const;
	/// ���������� �������� ��������
	const pro::interval<double> GetRange() const;
};

/// ������ ������� �������� ��������
class VWidenRecord
{
private:
	/// �������
	DEGREE degree;
	/// �������� ���������
	pro::interval<int> speed;
	/// ����� ��
	double norma;
	/// �������� �������� ������
	pro::interval<double> radius;
	/// �������� ��������
	pro::interval<double> value;
public:

	/// ������� ��� ������
	class VFindKey
	{
	private:
		DEGREE degree;
		int speed;
		double norma;
		double radius;
	public:
		VFindKey(DEGREE in_degree, int in_speed, double in_norma, double in_radius) : degree(in_degree), speed(in_speed), 
			norma(in_norma), radius(in_radius){};
		bool operator()(const VWidenRecord& in_key) const
		{
			return in_key.IsKey(degree, speed, norma, radius);
		}
	};

	/// �����������
	VWidenRecord(DEGREE in_degree, const pro::interval<int>& in_speed, double norma, const pro::interval<double>& in_radius, const pro::interval<double>& in_value);	
	/// ���������� true, ���� ������� ������ ������������� �����
	bool IsKey(DEGREE in_degree, int in_speed, double in_norma, double in_radius) const;
	/// ���������� �������� ��������
	const pro::interval<double> GetRange() const;
};


/// ������� �������� ����������� �� ��������
class VWidenTable
{
private:
	/// ������� ��� ��������� ���� 140
	std::vector<VWidenRecord> table;
	/// ������� ��� ��������� ���� 140
	std::vector<VWidenRecord> table_high_speed;
public:
	/// �����������
	VWidenTable();
	/// ���������� ��������
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed, double in_norma, double in_radius) const;
};

/// ������� �������� ����������� �� �������
class VNarrowTable
{
private:
	/// �������
	std::vector<VNarrowRecord> table;
public:
	/// �����������
	VNarrowTable();
	/// ���������� ��������
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed, double in_norma) const;
};

/// ������ ������� �������� ����������� (�����)
class VWaveRecord
{
private:
	/// �������
	DEGREE degree;
	/// �������� ���������
	pro::interval<int> speed;
	/// �������� ����
	pro::interval<double> length;
	/// �������� ��������
	pro::interval<double> value;
public:

	/// ������� ��� ������
	class VFindKey
	{
	private:
		DEGREE degree;
		int speed;
		double length;
	public:
		VFindKey(DEGREE in_degree, int in_speed, double in_length) : degree(in_degree), speed(in_speed), length(in_length){};
		bool operator()(const VWaveRecord& in_key) const
		{
			return in_key.IsKey(degree, speed, length);
		}
	};

	/// �����������
	VWaveRecord();
	/// �����������
	VWaveRecord(DEGREE in_degree, const pro::interval<int>& in_speed, const pro::interval<double>& in_length, const pro::interval<double>& in_value);	
	/// ���������� true, ���� ������� ������ ������������� �����
	bool IsKey(DEGREE in_degree, int in_speed, double in_length) const;
	/// ���������� �������� ��������
	const pro::interval<double> GetRange() const;
};

/// ������� ����������� �� ���������
class VPitTable
{
private:
	/// �������
	std::vector<VWaveRecord> table;		
public:
	/// �����������
	VPitTable();	
	/// ���������� ��������
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed, double in_length) const;
};

/// ������� ����������� �� ���������
class VDistortionTable
{
private:
	/// �������
	std::vector<VWaveRecord> table;	
public:
	/// �����������
	VDistortionTable();	
	/// ���������� ��������
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed, double in_length) const;
};


/// ������ ������� �������� ���
class VLevelRecord
{
private:
	/// �������
	DEGREE degree;
	/// �������� ���������
	pro::interval<int> speed;
	/// �������� ��������
	pro::interval<double> value;
public:

	/// ������� ��� ������
	class VFindKey
	{
	private:
		DEGREE degree;
		int speed;
	public:
		VFindKey(DEGREE in_degree, int in_speed) : degree(in_degree), speed(in_speed){};
		bool operator()(const VLevelRecord& in_key) const
		{
			return in_key.IsKey(degree, speed);
		}
	};
	/// �����������
	VLevelRecord(DEGREE in_degree, const pro::interval<int>& in_speed, const pro::interval<double>& in_value);	
	/// ���������� true, ���� ������� ������ ������������� �����
	bool IsKey(DEGREE in_degree, int in_speed) const;
	/// ���������� �������� ��������
	const pro::interval<double> GetRange() const;
};


/// ������� ����������� �� ������
class VLevelTable
{
private:
	/// �������
	std::vector<VLevelRecord> table;
public:
	/// �����������
	VLevelTable();
	/// ���������� ��������
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed) const;
};

/// ������� ����������� �� ���������
class VRichTable
{
private:
	/// �������
	std::vector<VWaveRecord> table;	
public:
	/// �����������
	VRichTable();
	/// ���������� ��������
	pro::interval<double> GetRange(DEGREE in_degree, int in_speed, double in_length) const;
};

//// -------------------------- �������� ������ ����������� ---------------------------

/// ������ ������� �������� ������
class VPenaltyRecord
{
private:
	
	int penalty;			///< ���-�� ������
	int norma;				///< ����� ��
	pro::interval<int> speed;		///< �������� ���������
	pro::interval<double> value;	///< �������� ��������
	pro::interval<int> add_key;	///< �������������� ����

public:

	/// ������� ��� ������
	class VFindKey
	{
	private:

		int norma;
		int speed;
		double value;
		int add_key;

	public:

		VFindKey(int in_norma, int in_speed, double in_value, int in_add_key) : 
		  norma(in_norma), speed(in_speed), value(in_value), add_key(in_add_key)
		{}

		bool operator() (const VPenaltyRecord & in_key) const
		{
			return in_key.IsKey(norma, speed, value, add_key);
		}
	};

	/// �����������
	VPenaltyRecord() : penalty(0), norma(0), speed(0, 300), 
		value(0, 0), add_key(0, INT_MAX)
	{}

	/// �����������
	VPenaltyRecord(int in_penalty, int in_norma, 
		const pro::interval<int> & in_speed, const pro::interval<double> & in_value) :
	penalty(in_penalty), norma(in_norma), speed(in_speed), value(in_value), add_key(0, INT_MAX)
	{}

	/// �����������
	VPenaltyRecord(int in_penalty, int in_norma, 
		const pro::interval<int> & in_speed, const pro::interval<double> & in_value,
		const pro::interval<int> & in_add_key) :
	penalty(in_penalty), norma(in_norma), speed(in_speed), value(in_value), add_key(in_add_key)
	{}

	/// ���������� true, ���� ������� ������ ������������� �����
	bool IsKey(int in_norma, int in_speed, double in_value, int in_add_key) const	
	{
		return this->norma == in_norma && this->speed.contains(in_speed) && 
			this->value.contains(in_value) && this->add_key.contains(in_add_key);
	}

	/// ���������� �����
	int GetPenalty() const
	{
		return this->penalty;
	}
};

/// ����� ������� ������� ������
class VPenaltyTable 
{
protected:
	
	/// �������
	std::vector<VPenaltyRecord> table;

public:

	/// ���������� �����
	int GetPenalty(int in_norma, int in_speed, double in_value, int in_add_key) const;
};

typedef boost::shared_ptr<VPenaltyTable> VPenaltyTablePtr;

/// ������� ������ ��������
class VWidenPenaltyTable : public VPenaltyTable
{
public:

	/// �����������
	VWidenPenaltyTable();
};

/// ������� ������ �������
class VNarrowPenaltyTable : public VPenaltyTable
{
public:

	/// �����������
	VNarrowPenaltyTable();
};

/// ������� ������ �������
class VDistortionPenaltyTable : public VPenaltyTable
{
public:

	/// �����������
	VDistortionPenaltyTable();
};

/// ������� ������ ��������
class VPitPenaltyTable : public VPenaltyTable
{
public:

	/// �����������
	VPitPenaltyTable();
};

/// ������� ������ ������� ����������� �� ������
class VLevelPenaltyTable : public VPenaltyTable
{
public:

	/// �����������
	VLevelPenaltyTable();
};

/// ������� ������ ����������� � �����
class VRichPenaltyTable : public VPenaltyTable
{
public:

	/// �����������
	VRichPenaltyTable();
};


// -------------------------------------------------------------------------------------


/// ������� ����������
class VDetectTables/* : public VOptions*/
{
private:		

	//������

	/// ��������� ��������
	VWidenTable widen_table;
	/// ��������� �������
	VNarrowTable narrow_table;
	/// ��������� ��������
	VPitTable pit_table;
	/// ��������� ���������
	VDistortionTable distortion_table;
	/// ��������� ������
	VLevelTable level_table;
	/// ��������� ��������
	VRichTable rich_table;

	/// ������� ������ ��������
	VPenaltyTablePtr widen_penalty_table;
	/// ������� ������ �������
	VPenaltyTablePtr narrow_penalty_table;
	/// ������� ������ ���������
	VPenaltyTablePtr distortion_penalty_table;
	/// ������� ������ ��������
	VPenaltyTablePtr pit_penalty_table;
	/// ������� ������ ������
	VPenaltyTablePtr level_penalty_table;
	/// ������� ������ ��������
	VPenaltyTablePtr rich_penalty_table;		
	
public:	
	
	/// �����������
	VDetectTables();		

	// ������ ������������ ���������

	/// ���������� ��������� ��������
	const VWidenTable& GetWidenTable() const;
	/// ���������� ��������� �������
	const VNarrowTable& GetNarrowTable() const;
	/// ���������� ��������� ��������
	const VPitTable& GetPitTable() const;
	/// ���������� ��������� ���������
	const VDistortionTable& GetDistortionTable() const;
	/// ���������� ����� ������
	const VLevelTable& GetLevelTable() const;
	/// ���������� ��������� ��������
	const VRichTable& GetRichTable() const;

	// ������ ������������ ����� ������� ������

	/// ������� ������ ��������
	const VPenaltyTable& GetWidenPenaltyTable() const;
	/// ������� ������ �������
	const VPenaltyTable& GetNarrowPenaltyTable() const;
	/// ������� ������ ���������
	const VPenaltyTable& GetDistortionPenaltyTable() const;
	/// ������� ������ ��������
	const VPenaltyTable& GetPitPenaltyTable() const;
	/// ������� ������ ������
	const VPenaltyTable& GetLevelPenaltyTable() const;
	/// ������� ������ ��������
	const VPenaltyTable& GetRichPenaltyTable() const;					
};
typedef VSingleton<VDetectTables> detect_tables_t;
#define DETECT_TABLES_INIT detect_tables_t::Instance().SetObject(VDetectTables())
#define DETECT_TABLES detect_tables_t::Instance().GetObject() 

