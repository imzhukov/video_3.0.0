#pragma once

#include <vector>
#include "VParameter.h"
#include "parameters.h"
#include "oversteps.h"

class VMainFile;
class VAprinfo;
class VDataSet;

/// ����� ������� ������� ������� (� �����)
struct VOldProfileMark
{
	VWayCoord wc;
	double value;				///< �������� �������
};

/// ����� �������
struct VProfileMark
{
	uint32_t index;				///< ����� �����
	double absCoord;			///< ���.���������� 
	double value;				///< �������� �������
	bool isCurve;				///< ������� ���� ��������

	/// �������� �������� FixMe!!! ������
	bool UpdateIndex(const VAbsCoordData & destAbs, const VAbsCoordData & srcAbs)
	{
		return destAbs.UpdateIndex(this->index, srcAbs);
	}
};

inline bool operator < (const VProfileMark & l, const VProfileMark & r)
{
	if (l.index!=BAD_INDEX && r.index!=BAD_INDEX)
		return l.index < r.index;
	else
		return l.absCoord < r.absCoord;
}

inline bool operator==(const VProfileMark & l, const VProfileMark & r)
{
	//���. ���������� �� ����������
	return l.index==r.index && l.value==r.value && l.isCurve==r.isCurve;
}

inline bool operator!=(const VProfileMark & l, const VProfileMark & r)
{
	//���. ���������� �� ����������
	return l.index!=r.index || l.value!=r.value || l.isCurve!=r.isCurve;
}

/// ������ ����� �������
class VProfileMarkList : public VIndexList<VProfileMark>
{

	/// ��������
	bool Load(const VBasicParameter & data, const VAbsCoordData & absCoord, const VAprinfo & aprInfo);

public:

	/// �����������
	VProfileMarkList()
	{}

	/// ��������
	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type, const VAbsCoordData & absCoord, const VAprinfo & aprInfo);

	/// ����������� �� ������� ������, � ���������� ���������
	bool CopyFrom(const VProfileMarkList & src, 
		const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs);

	/// ����������
	bool Save(VMainFile & file, uint32_t type, const VAbsCoordData & absCoord, const VAprinfo & aprInfo);

	/// �������� ������� ����� �� ���������� �������
	void UpdateValues(const VParameter & profile)
	{
		for (size_t i = 0; i < this->GetCount(); ++i)
		{
			this->GetItem(i).value = profile.GetItem(this->GetItem(i).index);
		}
	}

	// -----------------------------------------------------------------------------------
	// �������� �������
	// -----------------------------------------------------------------------------------

	/// ���������� ��������� �������
	size_t GetElementCount()
	{
		if (this->GetCount() > 1) return this->GetCount() - 1;
		else return 0;
	}

	/// ������ �� ����� � ������ ��������
	const VProfileMark & GetElementBegin(size_t i)
	{
		return this->GetItem(i);
	}

	/// ������ �� ����� � ����� ������ ��������
	const VProfileMark & GetElementEnd(size_t i)
	{
		return this->GetItem(i+1);
	}

	/// ������� �����
	size_t FindMark(const VProfileMark& in_mark)
	{
		std::vector<VProfileMark>::iterator i=std::find(records.begin(), records.end(), in_mark);
		if (i!=records.end())
			return i-records.begin();
		else
			return BAD_INDEX;
	};
	
	/// ������� ����� ����� �� ��������� ����������
	size_t FindLess(double in_abscoord)
	{
		
		VProfileMark mark;
		mark.index=BAD_INDEX;
		mark.absCoord=in_abscoord;
		std::vector<VProfileMark>::iterator i=std::lower_bound(records.begin(), records.end(), mark);
		if (i!=records.end())
			return i-records.begin();
		else
			return BAD_INDEX;
	};

	/// ������� ����� ������ �� ��������� ����������
	size_t FindGreater(double in_abscoord)
	{
		VProfileMark mark;
		mark.index=BAD_INDEX;
		mark.absCoord=in_abscoord;
		std::vector<VProfileMark>::iterator i=std::upper_bound(records.begin(), records.end(), mark);
		if (i!=records.end())
			return i-records.begin();
		else
			return BAD_INDEX;
	}
};

template <class FILETYPE>
bool VProfileMarkList::Load(FILETYPE & file, uint32_t type, 
							const VAbsCoordData & absCoord, const VAprinfo & aprInfo)
{
	if (this->IsLoaded()) return true;
	if (!absCoord.IsLoaded() || !aprInfo.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	VBasicParameter data;
	if (!data.Load(file, type, false))
	{
		this->message = data.GetErrorMessage();
		return false;
	}

	bool result = this->Load(data, absCoord, aprInfo);
	data.UnloadFile(file, type);
	data.Unload();
	return result;
}

/// ������� 
class VProfile : public VParameter
{
private:
	/// ������� ���� ��� �������������� �������� �������
	bool prof_marks_used;
	/// ����������� ���������� ����� ���������� �������
	double distance_btw_marks;
	/// �����������
	std::wstring comment;

public:
	/// �����������
	VProfile();
	/// ��������. ������ �������� - ��������
	bool Load(const VParameter & grad, const VAbsCoordData & absCoord, const VAprinfo & aprInfo);
	/// ���������� true, ���� �������������� ��������� �������
	bool ProfileMarksUsed();
	/// ���������� �����������
	std::wstring GetComment() const;
};

/** ����� �������� �������*/
class VSlope
{
private:
	
	/** ������ ������ �������� � �����*/
	size_t start_index;
	/** ������ ����� �������� � �����*/
	size_t end_index;
	/** ������*/
	point_t start;
	/** �����*/
	point_t end;	
	/** ������� ������/������*/
	bool isCurve;	
	/** ���������� ������*/
	point_t center;
	
	/** ��������� ��������� ������*/
	void CalcVerticalCurve(const VParameter& in_profile)
	{
		if (isCurve)
		{
			double mid_x=(start.x+end.x)/2;
			double mid_y=in_profile.GetItem((start_index+end_index)/2);
			circle_t c(point_t(start.x, start.y/100), point_t(mid_x, mid_y/100), 
				point_t(end.x, end.y/100));//���������� ���������� � �����					
			center.x=c.X();
			center.y=c.Y()*100;//��������� ������� � ����������
		}
		else
		{
			center.x=0;
			center.y=0;
		}
	}

public:
		
	/** �����������*/
	VSlope(const VProfileMark& in_start, const VProfileMark& in_end, const VParameter& in_profile)
	{
		start_index=in_start.index;
		end_index=in_end.index;
		//�������� ������� � ���������� ����������� �� ����� ������ � �����������
		start=point_t(round(in_start.absCoord), round(in_start.value));
		end=point_t(round(in_end.absCoord), round(in_end.value));		
		isCurve=in_start.isCurve;
		CalcVerticalCurve(in_profile);
	}	

	/** �����������*/
	VSlope(size_t in_start_index, size_t in_end_index, const VParameter& in_profile, const VAbsCoordData& in_abscoord, bool in_isCurve=false)
	{
		start_index=in_start_index;
		end_index=in_end_index;
		//�������� ������� ����������� �� ����� �����������
		start=point_t(in_abscoord.GetItem(start_index), round(in_profile.GetItem(start_index)));
		end=point_t(in_abscoord.GetItem(end_index), round(in_profile.GetItem(end_index)));
		isCurve=in_isCurve;
		CalcVerticalCurve(in_profile);
	}
	/** ������ ������ �������� � �����*/
	size_t GetStartIndex() const {return start_index;};
	/** ������ ����� �������� � �����*/
	size_t GetEndIndex() const {return end_index;};
	/** ���������� ����� ������*/
	const point_t& GetStart() const {return start;};
	/** ���������� ����� �����*/
	const point_t& GetEnd() const {return end;};
	/** ���������� ����� (� ��������� �� �����)*/
	double GetLength() const {return round(end.x-start.x);};
	/** ���������� ������� ������/������*/
	bool IsCurve() const {return isCurve;};	
	/** ���������� ������ (�)*/
	double R() const {return isCurve ? sqrt(pow(start.x-center.x,2)+pow(start.y/100-center.y/100,2)) : 0;};
	/** ���������� ����� ��/�*/
	double Slope() const {return round(10.0*(end.y-start.y)/(end.x-start.x), 1);};
	/** ���������� ���� ������������ ������*/
	double Sign()const {return start.y>center.y ? 1 : -1;};
	/** ���������� �������� ������� � �����*/
	double GetProfile(double in_abscoord) const
	{
		if (HasPoint(in_abscoord))
		{
			if (isCurve)
			{				
				return 100.0*(center.y/100.0 + Sign() * sqrt(pow(R(),2) - pow(in_abscoord - center.x, 2)));
			}
			else
			{				
				return line_t(start, end).Y(in_abscoord);
			}
		}
		else
			return 0;
	};
	///���������� true ���� ����� ����������� ��������
	bool HasPoint(double in_abscoord) const
	{
		return start.x<=in_abscoord && end.x>=in_abscoord;
	}
};

/// �������� < ��� VSlope
static bool operator < (const VSlope & l, const VSlope & r)
{
	return l.GetStart().x < r.GetStart().x;
}

/// ������ ������� FixMe!!! ���������� ������� ����� �� ������������
class VSlopeList : public VIndexList<VSlope>
{
private:
	///���
	mutable size_t cache_i;	
	///����� ����
	void ResetCache()
	{
		cache_i=BAD_INDEX;
	};
public:
	/// ��������
	bool Load(const VParameter & profile, VProfileMarkList & profmarks, const VAbsCoordData & absCoord, const VAprinfo & aprInfo, PARAMETER_TYPE in_data_type);
	/// ����������
	bool Update(const VParameter & profile, VProfileMarkList & profmarks, const VAbsCoordData & absCoord, const VAprinfo & aprInfo, PARAMETER_TYPE in_data_type);	
	/// ������� ������� �� ������� ��������� �������� �����
	size_t Find(double in_abscoord) const;
	///���������� ������� ��������� ������� �������� ��� ��������� ����� � �������� �������
	std::vector<size_t> GetAffectedSlopes(size_t in_index) const;
	///���������� ������� ������� ������� �������� ��� ��������� ����� � �������� �������
	std::vector<size_t> GetAffectedSection(size_t in_max, size_t in_index) const;
	///���������� �������� �������
	double GetProfile(double in_abscoord) const;
	///���������� true ���� ���� ��������� �� ������
	bool IsCurve(double in_abscoord) const;
};

/// ������������������ �������
class VAproxProfile : public VParameter
{
private:
	/// ����������� ����� ������� �����
	void Bezier(const VAbsCoordData & in_abscoord, const VSlopeList& in_slope_list, size_t in_first, size_t in_last);
public:
	/// ��������
	bool Load(const VParameter & in_profile, const VAbsCoordData & in_abscoord, 
		const VSlopeList& in_slope_list);
	/// ����������
	bool Update(const VParameter & in_profile, const VAbsCoordData & in_abscoord, 
		const VSlopeList& in_slope_list, size_t in_index);
};

/// ���������� ������� �� ���������� FixMe!!! ��������� ������
class VDiffProfile : public VParameter
{
private:	
public:

	/// ��������
	bool Load(const VParameter & in_profile, const VAproxProfile & in_aprox_profile);

	/// ����������
	bool Update(const VParameter & in_profile,  const VAproxProfile & in_aprox_profile,
		const VSlopeList& in_slope_list, size_t in_index);
};

/// ���������� ������� (���������� �� ���������)
class VJaggyProfile : public VParameter
{
public:

	/// ��������
	bool Load(VDataSet& ds);

};

/// ������� ���������
enum JAGGY_PROFILE_SIGNIFICANCE {SIGN_1=1, SIGN_2=2};

/// ���������������� ����������
class VJaggyProfileOverstep : public VBasicOverstep
{
private:
	/// ������� ���������
	JAGGY_PROFILE_SIGNIFICANCE significance;
	/// ���������� FixMe!!! � ������� �����
	double coord;
	/// �������� ������
	double otvod;
	/// ������� �����
	bool on_bridge;
	/// ������� �������
	bool on_switch;
	/// ������� �������
	bool on_station;

public:
	/// �����������
	VJaggyProfileOverstep(double in_start, double in_end, double in_coord, double in_value, 
		double in_otvod, JAGGY_PROFILE_SIGNIFICANCE in_significance, const VAprinfo& in_aprinfo);
	/// ���������� ����������
	double GetCoord() const;
	/// ���������� ������������ �����
	double GetOtvod() const;
	/// ��������� ��������
	std::wstring GetDescription() const;
	/// ���������� ������ � ��������� �������
	std::wstring GetDegreeString() const;
};
typedef boost::shared_ptr<VJaggyProfileOverstep> VJaggyProfileOverstepPtr;


/// ���������������� ���������� �������
class VJaggyProfileOverstepList
{
private:
	/// ������� ���� ��� ������ ���������
	bool loaded;	
			
	/// ������ �����������
	std::deque<VJaggyProfileOverstepPtr> oversteps;	
	
	/// ��������� ����������� � ������
	void AddOverstep(double start, double end, double center, double value, double otvod, const VAprinfo& aprinfo);	
	/// ������
	void Calculate(VDataSet& in_ds);

public:
	/// �����������
	VJaggyProfileOverstepList();
	/// ��������
	bool Load(VDataSet& in_ds);
	/// ��������
	void Unload();
	/// ���������� ����� �������
	size_t GetCount() const;
	/// ���������� ������ �� ������
	const VJaggyProfileOverstepPtr& GetItem(size_t in_index) const;
};