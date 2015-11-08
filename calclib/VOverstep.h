#pragma once

#include "VParameter.h"
#include "oversteps.h"

/// ���� �������������� �����������
const int ovrUnknown	=   0; ///< ����������� �����������
const int ovrWidthPl	=   1; ///< �������� [2]
const int ovrWidthMn	=   2; ///< ������� [2]
const int ovrLevel		=   3; ///< ������� ����������� �� ������ [2]
const int ovrDistort	=   4; ///< ������� �� ������ [2]
const int ovrPitR		=   5; ///< �������� ������ [2]
const int ovrPitL		=   6; ///< �������� ����� [2]
const int ovrRichR		=   7; ///< �������� ������ [2]
const int ovrRichL		=   8; ///< �������� ����� [2]
const int ovrOtvod		=   9; ///< ����� ������ ����� [4]
const int ovrDistShort	=  10; ///< ������� �� ���� ������� [4]
const int ovrDistLong	=  11; ///< ������� �� ���� ������  [4]

const int ovrAnp		=  15; ///< ������������ ��������� [4]
const int ovrKsi		=  16; ///< �������� ���������� ������������� ��������� [4]
const int ovrOtvodH		=  17; ///< ����� ���������� [4]
const int ovrZazor		=  18; ///< ����� [4]
const int ovrIznos		=  19; ///< ����� [4]
const int ovrRich		=  20; ///< �������� �� ����������� ���� [2]
const int ovrRichN		=  21; ///< �������� �� ������������� ���� [2]

const int ovrZazorR     =  22; ///< ����� ����� [4]
const int ovrZazorL     =  23; ///< ����� ������ [4]

const int ovrHoleSh		= 100; ///< ���� ��������� �� ��
const int ovrHoleLev	= 101; ///< ���� ��������� �� ������
const int ovrHolePrR	= 102; ///< ���� ��������� �� ����.��
const int ovrHolePrL	= 103; ///< ���� ��������� �� ����.���
const int ovrHoleRhR	= 104; ///< ���� ��������� �� ���.��
const int ovrHoleRhL	= 105; ///< ���� ��������� �� ���.���

const int ovrSpeed1		= 120; ///< ���. V ����
const int ovrSpeed2		= 121; ///< ���. V ���������

const int ovrKmChange	= 140; ///< ��������� ����� ��.
const int ovrStrelka	= 150; ///< ��������� �������
const int ovrStrOptLt   = 151; ///< �������, ������, ����� ���.������
const int ovrStrOptRt   = 152; ///< �������, ������, ������ ���.������
const int ovrStrXOptLt  = 153; ///< �������, ����������, ����� ���.������
const int ovrStrXOptRt  = 154; ///< �������, ����������, ������ ���.������

/// ��� �����������
std::string GetOverstepName(int type);

/// ����� �����������
const unsigned long ovmDefault = 0x00000000; ///< ������� ������������ �����
const unsigned long ovmAll     = 0xFFFFFFFF; ///< ������� ��
const unsigned long ovmLimit   = 0x00000001; ///< ����������� ��������
const unsigned long ovmDeg4    = 0x00000002; ///< ����������� 4 �������
const unsigned long ovmDeg3    = 0x00000004; ///< ����������� 3 �������
const unsigned long ovmDeg2    = 0x00000008; ///< ����������� 2 �������
const unsigned long ovmOverLen = 0x00000010; ///< ���������� �����
const unsigned long ovmHoles   = 0x00000020; ///< ���� ���������
const unsigned long ovmSwitch  = 0x00000040; ///< �������
const unsigned long ovmTech    = 0x00000080; ///< �������� "���������������" ����: ����� ���������, ������� �� 

/// �������������� ��� ������������� [������ �������]
const int decCombine	= 0x00000001; ///< ���������
const int decSequence	= 0x00000002; ///< ������������������

/// ����� ������� ����������� �������� � ���.��������������
const int decNewObject	= 0x00000080; 

/// ��������� 3 � 4 ��������, ����� � �������
const int decC			= 0x00000100; ///< C 
const int decPP			= 0x00000200; ///< PP 
const int decThreePr	= 0x00000400; ///< 3�� 
const int decBridge		= 0x00000800; ///< ���� 
const int decTonnel		= 0x00001000; ///< �������
const int decSwitch		= 0x00800000; ///< �������

/// ������ ������� �� ������
const int decPrjRich	= 0x00010000; ///< ���� 
const int decPrjP		= 0x00020000; ///< ���� 
const int decPrjPitR	= 0x00040000; ///< �����.� 
const int decPrjPitL	= 0x00080000; ///< �����.� 

/// ��������� ������ ��������
const int decPrjRP		= 0x00100000; ///< ����+� 
const int decPrjRPitR	= 0x00200000; ///< ����+��.� 
const int decPrjRPitL	= 0x00400000; ///< ����+��.� 

// �������� �������������� �����
inline bool IsFlag(int value, int flag)
{
	return (value & flag) == flag;
}

// �������� ���� �����������: ����������� �� ������
inline bool IsTypeBridgeLimit(int type)
{
	return type == ovrLevel ||	// �������
		type == ovrDistort ||	// ������� �� ������
		type == ovrPitR ||		// �������� ������
		type == ovrPitL ||		// �������� �����
		type == ovrRich ||		// ��������, ����������� ����
		type == ovrRichN ||		// ��������, ������������� ����
		type == ovrRichR ||		// �������� ������
		type == ovrRichL;		// �������� �����
}

// �������� ���� �����������: ������
inline bool IsTypeOptics(int type)
{
	return type == ovrWidthPl ||	// ��������
		type == ovrWidthMn ||	// �������
		type == ovrRich ||		// ��������, ����������� ����
		type == ovrRichN ||		// ��������, ������������� ����
		type == ovrRichR ||		// �������� ������
		type == ovrRichL;		// �������� �����
}

/// ��������� ����������� �� ����� [������]
#pragma pack (push,1)
struct VOverstepOnDiskV0
{
	short km;		///< ������� ����������, ��
	float m;		///< ������� ����������, �
    float abs;		///< ���������� ����������
    long type;		///< ��� �������������
    float value;	///< �������� �����������
    float norma;	///< ����� ���������
    float len;		///< �����
    long degree;	///< �������
    long number;	///< ��p. ����p ����������� � ������������������
    long comb;		///< �������������� ��� �������������
    long speedSet1;	///< ������������� ���p����
    long speedSet2;	///< ������������� ���p����
    long speedLim1;	///< ���p���� ��p��������
    long speedLim2;	///< ���p���� ��p��������
    char objectApr;	///< ���� ������� ���.������ (�����, ������� � ��.)
    bool sboy;		///< �p����� ���� [����� �� ������������]
};
#pragma pack (pop)

/// ��������� ����������� �� ����� [�����]
#pragma pack (push,1)
struct VOverstepOnDiskV1
{
	// ������������ ����������
	short km;		///< ������� ����������, ��
	float m;		///< ������� ����������, �
	double abs;		///< ���������� ����������

	// ���������� ����������
	short rkm;		///< ������� ����������, ��
	float rm;		///< ������� ����������, �
	double rabs;	///< ���������� ����������

	long type;		///< ��� �������������
	long typeOrig;	///< ������������ ��� ������������� (������ ��� ����� �������� ��� ����� ����)
	long degree;		///< �������
	long number;		///< ��p. ����p ����������� � ������������������
	long comb;		///< �������������� ��� �������������
	float value;	///< �������� �����������
	float norma;	///< ����� ���������
	float len;		///< �����

	/// ������������� ���p����
	long speedSet1;	
	long speedSet2;
	long speedSet3;

	/// ���p���� ��p��������
	long speedLim1;	
	long speedLim2;
	long speedLim3;
};
#pragma pack (pop)

/// ���������, ����������� ���.���� �����������
struct VOvrAddCode
{
	bool C;			///< �
	bool PP;		///< ��
	bool ThreePr;	///< 3��
	bool PrjRich;	///< ����
	bool PrjP;		///< ����
	bool PrjPitR;	///< �����.�
	bool PrjPitL;	///< �����.�
	bool PrjRP;		///< ����+�
	bool PrjRPitR;	///< ����+��.�
	bool PrjRPitL;  ///< ����+��.�
	bool Bridge;	///< ����
	bool Tonnel;	///< �������
	bool Switch;    ///< �������

	/// �����������
	VOvrAddCode()
	{
		this->Init();
	}

	/// �������������
	void Init()
	{
		ZeroMemory(this, sizeof(*this));
	}

	/// ������ �� �������� v0 � v1
	void Import(long type, long comb, char objectApr);

	/// ������� � int
	int Export();
};

/// �����, ����������� �����������
class VOverstep2
{
	/// ������ �� ��������� ������ 0
	void Import(const VOverstepOnDiskV0 & src);

	/// ������ �� ��������� ������ 1
	void Import(const VOverstepOnDiskV1 & src);

public:

	int version;		///< ������ ������������ ���������

	VWayCoord wayCoord; ///< ������� ���������� ����������
	double absCoord;	///< ���������� ���������� ����������

	VWayCoord wayCoordOrig; ///< ������� ���������� ������������
	double absCoordOrig;	///< ���������� ���������� ������������

	bool isOrigCoord;   ///< ������������ ����������

	float value;		///< �������� �����������
	float norma;		///< �����, ���� ����
	float len;			///< ����� �����������
	int numElem;		///< �-�� ������������ �����������
	int degree;			///< �������
	int number;			///< ����� � ������������������

	int type;			///< ��� �����������
	int typeOrig;		///< ������������ ��� ������������� (������ ��� ����� �������� ��� ����� ����)
	bool isOrigType;    ///< ������������ ��� �����������

	VOvrAddCode code;	///< ���.��� �����������

	VSpeeds setSpd;		///< �������� �� �������
	VSpeeds limSpd;		///< ����������� ���������
	bool isThirdSpeed;	///< ������ ��������
	
	/// �����������
	VOverstep2()
	{
		this->Init();
	}

	VOverstep2(const VOverstepOnDiskV0 & ovr)
	{
		this->Import(ovr);
	}

	VOverstep2(const VOverstepOnDiskV1 & ovr)
	{
		this->Import(ovr);
	}

	/// �������������
	void Init()
	{
		ZeroMemory(this, sizeof(*this));
	}

	/// ������� ����������� ��������
	bool IsLimit() const
	{
		return this->limSpd.GetPassSpeed() < this->setSpd.GetPassSpeed() ||
			this->limSpd.GetTruckSpeed() < this->setSpd.GetTruckSpeed() ||
			this->limSpd.GetEmptySpeed() < this->setSpd.GetEmptySpeed();
	}

	/// ����������� ������������� �����
	bool MeetsMask(unsigned long __mask) const;

	/// ����������� ������������ � �������
	bool MeetsSection(const VWaySection * section) const
	{
		return section == NULL || 
			(this->wayCoord >= section->start && this->wayCoord <= section->end);
	}

	/// ����� ����������� �� �����������
	int GetPenalty() const;

	bool operator < (const VOverstep2 & src)
	{
		return this->wayCoord < src.wayCoord;
	}
};

/// ������ �����������
class VOvr2List : public VIndexList<VOverstep2>
{
private:

	std::vector<VOverstepOnDiskV0> full_v0; ///< ������ ������ ����������� ������ 0
	std::vector<VOverstepOnDiskV1> full_v1; ///< ������ ������ ����������� ������ 1

	/// �������� ������������ ������
	VOvr2List & operator = (const VOvr2List &);

	/// ������� - �������� �� ����� ������
	bool Load(VBasicParameter & data);

	/// ���������� ��� (���������� ���� �����)
	bool CheckType(int type) const
	{
		return (type >= 1 && type <= 9) || (type >= 15 && type <= 17) ||
			(type >= 19 && type <= 23) || (type >= 100 && type <= 105);
	}
	
public:

	/// ������ - �������� �� �����
	template <class FILETYPE>
	bool Load(FILETYPE & file, uint32_t type);

	/// ����������� ������ 
	bool CopyFrom(const VOvr2List & src, const VWaySection * section);

	/// ������ �� �������
	bool Import(const std::vector<VOverstepOnDiskV1> & ovrs);

	/// ���������� � ����
	bool Save(VMainFile & file, uint32_t type);

	/// ��������� ����� � ������������� ������ �����������
	bool BuildOvrList(unsigned long __mask, VWaySection * section);

	/// ��������
	void Unload()
	{
		this->full_v0.clear();
		this->full_v1.clear();
		VIndexList<VOverstep2>::Unload();
	}

	/// ������� ������ �����������
	const std::vector<VOverstep2> & ExportList() const
	{
		return this->records;
	}

	/// ������� ������ ����������� ��� ���������� ��
	std::vector<VOverstep2> ExportList(int32_t km) const;

	/// ��-32 ��� ���������� ���������
	std::string GetPU32String(int32_t km) const;

	/// ������ ��������� ����������, � ������� ������
	std::set<int32_t> GetKmList() const;
};
