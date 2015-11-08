#pragma once
  
/*
���� VParameter.h
���������� �������� � ����� �������
*/

//boost
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include "VBasicParameter.h"
#include "VRemoteFile.h"
#include "mathlib.h"
#include "error_messages.h"

#include "Shiny.h"
#include "parameters.h"

#include "VCorrupt.h"
#include "fft.h"

class VProgressBox;

// ---------------------------------------------------------------------------
/// �� �������
// ---------------------------------------------------------------------------
const uint32_t NOT_FOUND = static_cast<uint32_t>(-1);

// ---------------------------------------------------------------------------
/// ������� ������������ ����� -- �������� ��� ������� � ����������
// ---------------------------------------------------------------------------
template <class T> class VParameterProto
{
private:

	/// ������ ���������
	bool isLoaded;

protected:

	/// ��������� �� ������
	std::wstring message;

public:

	/// �����������
	VParameterProto() : message(ERRMSG_DATA_NOT_LOADED), isLoaded(false)
	{}

	/// �������� ������
	void Unload()
	{
		this->message = ERRMSG_DATA_NOT_LOADED;
		this->SetLoaded(false);
	}

	/// �������� ������������� ���������
	bool IsLoaded() const
	{
		return this->isLoaded;
	}

	/// ���������� ��������� ���� "��������"
	void SetLoaded(bool value)
	{
		this->isLoaded = value;
	}

	/// ��������� �� ������
	const std::wstring & GetErrorMessage() const
	{
		return this->message;
	}

	/// ����� �������
	virtual size_t GetCount() const = 0;

	/// ������� ���������� ������ �� i-� �������
	virtual const T & GetItem(size_t index) const = 0;

	/// ������� ������ �� i-� �������
	virtual T & GetItem(size_t index) = 0;

	/// ������� ������ �� ��������� �������
	T & GetLastItem()
	{
		return this->GetItem(this->GetCount()-1);
	}

	/// ������� ����������� ������ �� ��������� �������
	const T & GetLastItem() const
	{
		return this->GetItem(this->GetCount()-1);
	}
};

// ---------------------------------------------------------------------------
/// ��������� �����, ���������� ���� ������ ��� ������ ��������� T
// ---------------------------------------------------------------------------

template <class T> class VTemplateParameter : public VParameterProto<T>
{
protected:

	std::vector<T> data;	///< ���� ������ � �����
	uint32_t version;		///< ������ ���������	

	/// ���� ����������
	VParameterCorruptPtr corrupt;

private:

	/// ��������� ����������� ������
	VTemplateParameter(const VTemplateParameter<T> &);

	/// �������� ������������ ������
	VTemplateParameter<T> & operator = (const VTemplateParameter<T> &);

public:

	/// �����������
	VTemplateParameter() : VParameterProto<T>(), corrupt(new VParameterCorrupt)
	{
		this->version = 0;
	}

	~VTemplateParameter()
	{
		this->Unload();
	}

	/// ��������
	template <class F> bool Load(F & file, uint32_t type);

	/// �������� � ��������� �������� ������ ����������
	template <class F> bool Load(F & file, uint32_t type, VParameterCorruptPtr corr);

	/// ��������� � ����
	bool Save(VMainFile & file, uint32_t type);

	/// ������ ������ � �����
	size_t GetVersion() const
	{
		return this->version;
	}

	/// ������� "������" ��������
	bool Create(uint32_t type, uint32_t recCount, uint32_t version, const T & defValue);

	/// ��������
	void Unload()
	{
		this->data.clear();
		this->version = 0;
		VParameterProto<T>::Unload();
	}

	/// �������� �������� � ������
	bool IsAlloc() const
	{
		return true;
	}

	/// ����� �������
	size_t GetCount() const
	{
		return this->data.size();
	}

	/// ������� ����������� ������ �� i-� �������
	const T & GetItem(size_t i) const
	{
		return this->data.at(i);
	}

	/// ������� ������ �� i-� �������
	T & GetItem(size_t i) 
	{
		return this->data.at(i);
	}

	/// ������� ������� ���� �� i-� ��������
	bool IsCorrupt(size_t i) const
	{
		return this->corrupt->IsCorrupt(i);
	}

	/// ���������� ������� ���� �� i-� ��������
	void SetCorrupt(size_t i, bool c)
	{
		this->corrupt->SetCorrupt(i, c);
	}

	/// ����� ���������� �� shift ������
	void ShiftCorrupt(int count)
	{
		this->corrupt->Shift(count);
	}
};

// ---------------------------------------------------------------------------
// �������� double (� �������, GPS)
// ---------------------------------------------------------------------------
typedef VTemplateParameter<double> VDoubleParameter;
typedef boost::shared_ptr<VDoubleParameter> VDoubleParameterPtr;

// ---------------------------------------------------------------------------
// �������� unsigned long (� �������, ������� ���)
// ---------------------------------------------------------------------------
typedef VTemplateParameter<unsigned long>VUlongParameter;

// ---------------------------------------------------------------------------
/// ���������� ���������� 
// ---------------------------------------------------------------------------
class VAbsCoordData : public VDoubleParameter
{
public:

	/// ����� ������� ����� �� ����������.
	/// bool forceFit - ��� ����� �� ��������� ������� �������� ������� �������(0 � count-1)
	uint32_t FindIndex(double absCoord, bool forceFit) const;

	/// �������� �������� ��� ������������ ����� ���.��������� �� srcAbs � ���
	/// ��������, ��� ��������� ����� �����
	bool UpdateIndex(uint32_t & index, const VAbsCoordData & srcAbs) const;

	/// ����� ����������
	void Shift(double offset_m);

	/// ��������������� ����������
	void Scale(double scale_m_per_km);

	/// �������� ����������
	void Correct(double offset_m, double expand_m_per_km);

	// �������� ���������� � ������� ������������� ������������
	void Correct(line_t input);

	// ������� ������������ ����� �����
	const double GetLengthStep();

	// �������� ���������� �� ��������� �������� ���� (��� ������������ �����)
	// void Correct(uint32_t start_index, uint32_t end_index, double start_coord, double end_coord);
};

// ---------------------------------------------------------------------------
/// �������� (���������)
// ---------------------------------------------------------------------------
class VParameter : public VTemplateParameter<float>
{
protected:

	/// ��� ����� ��� ������� �����
	mutable double cache_sum;
	/// ��� ������� ��� ������� �����
	mutable size_t cache_sum_index1;
	/// ��� ������� ��� ������� �����
	mutable size_t cache_sum_index2;
	
	/// ��� ��� ������� ������� �����
	mutable std::vector<double> cache_weight;

	/// ��� ����� ��� ������� ����� ������� ������
	mutable size_t cache_corrupt;
	/// ��� ������� ��� ������� ����� ������� ������
	mutable size_t cache_corrupt_index1;
	/// ��� ������� ��� ������� ����� ������� ������
	mutable size_t cache_corrupt_index2;

	mutable size_t first_def;
	mutable size_t last_def;

	///��� ������ ��� ������� �������
	mutable std::vector<float> median_buffer;	
	
public:

	///���������� ���
	void ResetCache();

	/// �����������
	VParameter();

	/// ��������� �� ���� ��������� ���������� ��������
	void Shift(float shift);

	/// ������� �������� �� shift ������
	void ShiftByKadr(short int shift, bool fill_end = true);

	// �������� �� �������
	void LoadFromArray(float * param);

	/// ����������� �������� ������������ ���
	void Invert(float axis);

	/// ���������� ������ ������������� �� ������ �������� �� �������
	size_t GetMaxAbsIndex(size_t startNdx, size_t endNdx) const;

	/// ���������� ������������ �������� �� �������
	float GetMax(size_t startNdx, size_t endNdx) const;

	/// ���������� ������������ �������� �� ���� �������
	float GetMax() const;	

	/// ���������� ������������ �������� �� �������
	float GetMaxAbs(size_t startNdx, size_t endNdx) const;

	/// ���������� ������������ �������� �� ������� �� ������
	float GetMaxAbsSigned(size_t startNdx, size_t endNdx) const;

	/// ���������� ����������� �������� �� �������
	float GetMin(size_t startNdx, size_t endNdx) const;	

	/// ���������� ����������� �������� ������ �������
	float GetMin() const;

	/// ���������� ����������� �������� �� �������
	float GetMinAbs(size_t startNdx, size_t endNdx) const;

    /// ���������� ����� �������� �� �������
	double GetSum(size_t startNdx, size_t endNdx) const;
	
	 /// ���������� ���������� ����� �������� �� �������
	double GetWeightedSum(size_t startNdx, size_t endNdx) const;

	/// ���������� ����� ����� �� �������
	size_t GetCorruptCount(size_t startNdx, size_t endNdx) const;

	// ���������� ����� ����� �� �������
	size_t GetCorruptCount(size_t axis, float halfBase, const VAbsCoordData & absCoord) const;

	/// ���������� ����� ��������� �������� �� �������
	double GetSum2(size_t startNdx, size_t endNdx, double mo) const;

	/// ���������� �� �� �������
	float GetMO(size_t startNdx, size_t endNdx) const;

	/// ���������� �� �� �������
	float GetWeightedMean(size_t startNdx, size_t endNdx) const;

	/// ���������� �� � ������� +/- halfBase �� ��� axis
	float GetMO(size_t axis, float halfBase, const VAbsCoordData & absCoord) const;
	
	/// ���������� ���������� ������� � ������� +/- halfBase �� ��� axis
	float GetWeightedMean(size_t axis, float halfBase, const VAbsCoordData & absCoord) const;
	
	/// ���������� ���������� � �����
	float GetJaggy(size_t axis, float base, const VAbsCoordData & absCoord) const;

	/// ���������� �� �� ��� �����
	float GetMO() const;

	/// ���������� ��������� �� �������
	float GetDisp(size_t startNdx, size_t endNdx, double mo) const;	

	/// ���������� ��������� �� ��� �����
	float GetDisp() const;

	/// ���������� ��� �� �������
	float GetSKO(size_t startNdx, size_t endNdx, float mo) const;	

	/// ���������� ��� �� ��� �����
	float GetSKO(float mo) const;

	/// ���������� ��� �� �������
	float GetSTO(size_t startNdx, size_t endNdx, float mo) const;	

	/// ���������� ��� �� ��� �����
	float GetSTO(float mo) const;
	
	/// ���������� ������������� ��������� ����������� �� �������
	std::set<density_t> GetDensity(size_t startNdx, size_t endNdx, double in_size);

	/// ���������� ��������� ������ ���������������� ������� �� ��� (�� ��������)
	line_t GetLine(size_t startNdx, size_t endNdx) const;

	/// ���������� ��������� ������ ���������������� ������� �� ��� (�� ���. ����������)
	line_t GetLine(size_t startNdx, size_t endNdx, const VAbsCoordData & absCoord) const;

	/// ���������� ��������� ������, ���������� ����� �� ��������
	line_t GetChord(size_t startNdx, size_t endNdx);

	/// ���������� ��������� ������, ���������� ����� (�� ���. ����������)
	line_t GetChord(size_t startNdx, size_t endNdx, const VAbsCoordData & absCoord);

	/// ���������� �������� ������ ��������� �� ���� 'base' ������
	float GetOtvod(size_t ndx, const VAbsCoordData & absCoord, double base) const;

	/// ���������� �������� ������ ��������� �� ���� 'base' ������
	float GetOtvod(size_t in_start, size_t in_end, const VAbsCoordData & absCoord) const;

	/// ������������ �����������
	float CalcDerivate(size_t in_start, size_t in_end) const;
	
	/// ���������� ����������� � ����� (���� - 3 �����)
	float GetDerivate(size_t in_index) const;

	/// ���������� ������ ����������� � ����� (���� - 3 �����)
	float GetDerivate2(size_t in_index) const;

	/// ���������� ����������� � ����� (���� - ����� ������)
	float GetDerivate(size_t in_index, size_t in_base) const;

	/// ���������� ����������� � ����� (���� - ����� � ������)
	float GetDerivate(size_t ndx, const VAbsCoordData & absCoord, double base) const;	
		
	/// ���������� ������� �� �������
	float GetMedian(size_t startNdx, size_t endNdx) const;

	/// ���������� �������
	float GetMedian(size_t ndx, const VAbsCoordData & absCoord, double base) const;

	/// ���������� ������ ������� ������������� ��������
	size_t GetFirstDef() const;

	/// ���������� ������ ���������� ������������� ��������
	size_t GetLastDef() const;

	/// ���������� �� �� ������� - � ������ �����, �� ��� �����������. ��� ���������� + �������
	float GetStatMO(size_t startNdx, size_t endNdx) const;

	/// ���������� ��� �� ������� - � ������ �����, �� ��� �����������. ��� ����������
	float GetStatSKO(size_t startNdx, size_t endNdx, float mo) const;

    /// ���������� �������� �� ������� - � ������ �����, �� ��� �����������. ��� �������
    float GetStatMax(size_t startNdx, size_t endNdx) const;

	///���������� �����������
	virtual std::wstring GetComment() const;
};

typedef boost::shared_ptr<VParameter> VParameterPtr;

// ---------------------------------------------------------------------------
/// �������� ���
// ---------------------------------------------------------------------------

class VSpeedData : public VParameter
{
public:

	// �������� �� �����
	template <class F> bool Load(F & file, uint32_t type);

};

// ---------------------------------------------------------------------------
/// ����� ������ ��� "���������" �������� - ����� ���������, ���.������, �����
// ---------------------------------------------------------------------------
template <class MEMRECORD> class VIndexList : public VParameterProto<MEMRECORD>
{
private:

	/// �������� ����������� ������
	/// ����������� ������� CopyFrom()
	VIndexList<MEMRECORD> & operator = (const VIndexList<MEMRECORD> &);

protected: 

	/// ����� ������ � ������������ �������
	/// ��� ������������ ������ ������
	size_t kadrCount;

	/// ������ � ������
	std::vector<MEMRECORD> records;	

	/// ��������
	template <class FILETYPE, class FILERECORD>
	bool DefaultLoad(FILETYPE & file, uint32_t type, size_t kadrCount);

	/// ���������� � ����
	template <class FILERECORD> 
	bool DefaultSave(VMainFile & file, uint32_t type);

	/// ������� ������ �� �����
	void DeleteFromFile(VMainFile & file, uint32_t type)
	{
		size_t idx = file.GetBlockIndex(type);
		if (idx != BAD_SIZE)
		{
			file.DeleteBlock(idx);
		}
	}

public:

	/// �����������
	VIndexList() : kadrCount(0)
	{}

	/// ��������
	void Unload()
	{
		this->Clear();
		this->SetLoaded(false);
		this->kadrCount = 0;
		this->message = ERRMSG_DATA_NOT_LOADED;
	}

	/// ����� ����������� �������
	size_t GetCount() const
	{
		return this->records.size();
	}

	/// ����������� ������ �� i-� ������
	const MEMRECORD & GetItem(size_t i) const
	{
		return this->records.at(i);
	}

	/// ������ �� i-� ������
	MEMRECORD & GetItem(size_t i) 
	{
		return this->records.at(i);
	}

	/// �������� ������
	void AddItem(const MEMRECORD & mark)
	{
		this->records.push_back(mark);
		this->SetLoaded(true);
	}

	/// �������� ������
	void DeleteItem(size_t i)
	{
		this->records.erase(this->records.begin() + i);
	}

	/// �������� ���� �������
	void Clear()
	{
		this->records.clear();
	}

	///���������� �� ����������
	void Sort()
	{
		if (!this->records.empty())
			std::sort(this->records.begin(), this->records.end());
	}

	/// ����������� �� ������� ������, � ���������� ���������
	bool CopyFrom(const VIndexList<MEMRECORD> & src, 
		const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs);

	/// ����� ������ ����������� � ���������� �����
	size_t FindItem(size_t kadrNdx) const;

	/// ����� �� ��������� ����� ������
	void Shift(int value);
};

// ����� �� ��������� ����� ������
template <class MEMRECORD>
void VIndexList<MEMRECORD>::Shift(int value)
{
	std::vector<MEMRECORD> new_records;

	for (size_t i = 0; i < this->records.size(); ++i)
	{
		__int64 new_index = (__int64) this->records[i].index + (__int64) value;
		if (new_index > 0 && new_index < (__int64) this->kadrCount)
		{
			this->records[i].index = (size_t) new_index;
			new_records.push_back(this->records[i]);
		}
	}

	this->records = new_records;
}

// ����� ������ ����������� � ���������� �����
template <class MEMRECORD>
size_t VIndexList<MEMRECORD>::FindItem(size_t kadrNdx) const
{
	class PredicateFits
	{
		const size_t index;

	public:

		PredicateFits(size_t i) : index(i)
		{}

		bool operator () (const MEMRECORD & rec) const
		{
			return rec.index == this->index;
		}
	};

	std::vector<MEMRECORD>::const_iterator found_item = std::find_if(
		this->records.begin(), this->records.end(), PredicateFits(kadrNdx));

	if (found_item == this->records.end()) return BAD_INDEX;
	else return found_item - this->records.begin();
}

// ��������
template <class MEMRECORD>
template <class FILETYPE, class FILERECORD>
bool VIndexList<MEMRECORD>::DefaultLoad(FILETYPE & file, uint32_t type, size_t kCount)
{
	if (this->IsLoaded()) return true;
	this->Unload();

	VBasicParameter data;
	if (!data.Load(file, type, false))
	{
		this->message = data.GetErrorMessage();
		return false;
	}

	if (!data.CheckRecCount<FILERECORD>()) 
	{	
		data.UnloadFile(file, type);
		data.Unload();
		this->message = ERRMSG_VERSION_INCORRECT;
		return false;
	}

	MEMRECORD record;
	for (register size_t i = 0; i < data.GetRecCount(); ++i)
	{
		if (record.Import(data.GetItem<FILERECORD>(i)) &&
			record.Fits(kCount))
		{
			this->records.push_back(record);
		}
	}

	this->kadrCount = kCount;
	data.UnloadFile(file, type);
	data.Unload();
	this->message = ERRMSG_OK;
	this->SetLoaded(true);
	return true;
}

// ���������� � ����
template <class MEMRECORD>
template <class FILERECORD> 
bool VIndexList<MEMRECORD>::DefaultSave(VMainFile & file, uint32_t type)
{
	if (!this->IsLoaded())
	{
		this->message = ERRMSG_DATA_NOT_LOADED;
		return false;
	}

	// ��� ������ ������� ����� ����� ������ ������� ������
	if (this->GetCount() == 0)
	{
		this->DeleteFromFile(file, type);
		this->message = ERRMSG_OK;
		return true;
	}

	VBasicParameter data;
	data.Create(type, VCT_USER, this->GetCount(), sizeof(FILERECORD), 1);

	size_t position = 0;
	for (size_t i = 0; i < this->GetCount(); ++i)
	{
		FILERECORD frec = this->GetItem(i).Export();
		data.Write<FILERECORD>(frec, position);
	}

	if (data.Save(file, this->GetCount(), sizeof(FILERECORD)))
	{
		this->message = ERRMSG_OK;
		return true;
	}
	else
	{
		this->message = ERRMSG_BLOCK_WRITE_FAILED;
		return false;
	}
}

// ����������� �� ������� ������, � ���������� ���������
template <class MEMRECORD>
bool VIndexList<MEMRECORD>::CopyFrom(const VIndexList<MEMRECORD> & src, 
	const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs)
{
	this->Unload();
	for (size_t itemNdx = 0; itemNdx < src.GetCount(); ++itemNdx)
	{
		MEMRECORD item = src.GetItem(itemNdx);
		if (item.UpdateIndex(destAbs, srcAbs))
		{
			this->records.push_back(item);
		}
	}
	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
}


///������� ��������� ���������� FixMe!!! �������  � ��������� ����

///������ ����������� �������� �� ���� (��� ���� - ���. �����.)
void ParameterMean(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base);

///������ ����������� �������� �� ���� (��� ���� - ���. �����.)
void ParameterWeightedMean(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base);

/// ������
void ParameterSpectrum(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord);

/// ����
void ParameterPhase(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord);

/// FIR
void ParameterFIRFilter(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, const VFIRFilterPtr& in_filter);

/// ������ �������
void ParameterKalman(VParameter& in_source, VParameter& out_target, double in_q, double in_r);

///������ ����������� ��� �� ���� (��� ���� - ���. �����.)
void ParameterSKO(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base);

///������ ���������� �������
void ParameterMedian(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base);

///������ ������ �� ���� (��� ���� - ����� ������)
void ParameterOtvod(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base);

///����������� ��������� �� ����
void ParameterDerivate(VParameter& in_source, VParameter& out_target, size_t in_half_base);

///����������� ��������� �� ���� (���� - ���. �����)
void ParameterDerivate(VParameter& in_source, VParameter& out_target, VAbsCoordData & in_abscoord, double in_base);

///"�������� ���������"
void ParameterSharp(VParameter& in_source, VParameter& out_target, double in_amount);

///�������� ���� ����������
void ParameterDiff(VParameter& in_source1, VParameter& in_source2, VParameter& out_target);

///��������������� ���������
void ParameterScale(VParameter& in_source, VParameter& out_target, double in_aspect);

///�������� ���������
void ParameterShift(VParameter& in_source, VParameter& out_target, double in_value);

///�������������� ����� ���� ����������
void ParameterGeoSum(VParameter& in_source1, VParameter& in_source2, VParameter& out_target);

/// ��������� ����������� ��� � �������� ���������
struct CorrAndOffset
{
	int offset;
	double oldCorr;
	double newCorr;
};

/// ������������� ���������� - ������ ��������� ���������� ����� �����������
/// �� ������ - �������� 2 ��������� ������������ �������.
double CorrParams(const VParameter & prm1, const VParameter & prm2,			// ���������
						PARAMETER_TYPE prm1_t, PARAMETER_TYPE prm2_t,		// �������� ����������
						size_t offset,	size_t maxOffset);					// �������� ������ ��������� ������������ �������

//��������� �������

///������ ���������� ���� ����������
double cov(VParameter & in_param1, VParameter & in_param2);
///������ ������������ ���������� (�������)
double corr(VParameter & in_param1, VParameter & in_param2);
///������ ������������ ���������� (�������) �� ���� ������
double corr(const VParameter& in_param1, const VAbsCoordData& in_abscoord1, 
			const VParameter & in_param2, const VAbsCoordData& in_abscoord2);
///������ ������������ ���������� ������ (�������)
double corr_sign(VParameter & in_param1, VParameter & in_param2);





