#pragma once

#include "VAprinfo.h"
#include "types.h"
#include "inm.h"
#include "markers.h"

#include <valarray>
#include <queue>

// ��������� ���
class VRtPPR
{
public:

	unsigned long dpp;
	std::vector<point_t> points_left, points_right;

	float nak_left, nak_right;
	float pod_left, pod_right;
	float wear_vert_left, wear_vert_right;
	float wear_ext_left, wear_ext_right;
	float wear_int_left, wear_int_right;
	float wear_sum_left, wear_sum_right;
	float thread_width;
	
	bool patch_left, patch_right;
	bool ramp_left, ramp_right;
	bool cross_left, cross_right;

	VRtPPR();
	VRtPPR(const ppr_record & ppr);

	bool WriteToFolder(const std::wstring & folder, const VWayCoord & wc) const;
	/*bool WriteToBinary(std::ostream & os) const;
	bool ReadFromBinary(std::istream & is);*/

	bool WriteToBuffer(char * buffer) const;
	bool ReadFromBuffer(const char * buffer);
};

// �������� � ��������
class VRtPicture
{
public:

	unsigned long dpp;
	std::valarray<unsigned char> data;

	VRtPicture();
	VRtPicture(const zaz_header & zaz, unsigned char * rawdata);

	bool WriteToFolder(const std::wstring & folder, const VWayCoord & wc) const;
};

// �����
struct VRtUgon
{
	unsigned long dpp;
	float ugon_left, ugon_right;

	VRtUgon() : dpp(0), ugon_left(0), ugon_right(0)
	{}

	VRtUgon(const ugon_record & rec) : dpp(rec.dpp),
		ugon_left(rec.ugon_left), ugon_right(rec.ugon_right)
	{}
};

// ���������, ������������� � ����-1
class VRtImportedParameter
{
public:

	template <class T> struct Param
	{
		T value;
		bool corrupt;

		Param() : value(0), corrupt(true)
		{}

		void CorrectWears();
		void CorrectZazor();
        void Import(const T & src, T minValue, T maxValue, T defaultValue, uint32_t error, uint32_t errorCode);

	};

private:

	template <class T> Param<T> Import(const T & src, 
		T minValue, T maxValue, T defaultValue,
		uint32_t errorCode);

public:

	// ��������� ���������
	bool isOk;			// ������� ���������� ��������� ��������� �������
	double wayPassed;	// ���������� ����
	double glzAbsCoord; // ���.���������� �� ��������
	uint32_t corrupt;	// ���� ���������
	uint16_t buttons;	// ������ ���������
	float speed;		// �������� ������
	unsigned long time; // ����� �������

	// ������������� ���������
	Param<float> pitLeft, pitRight;
	Param<float> richLeft, richRight;
	Param<float> distShort, distLong;
	Param<float> width, wear;
	Param<float> widthFront, widthBack;
	Param<float> wearLeft, wearRight;
	Param<float> tempLeft, tempRight;
	Param<float> level, gradient;
	Param<float> uskVert, uskHor;
	Param<float> gyroLat, gyroLong;
	Param<float> curv, kurs, kren, tangage;
	Param<double> gpsTime, gpsLat, gpsLong, gpsAlt;
	Param<float> zazorLeft, zazorRight;
	Param<float> mzLeft, mzRight;
	unsigned long dpp, kadres;

	// ������ ������� ������
	struct PPRData
	{
    	bool isDefault;

		Param<float> width, wearLeft, wearRight;
		Param<float> podLeft, podRight;
		Param<float> nakLeft, nakRight;
		Param<float> wearIntLeft, wearIntRight, wearExtLeft, wearExtRight;

        void Import(float width, float nl, float nr, float pl, float pr, float wl, float wr);
        void Import(float width, float nl, float nr, float pl, float pr, float wl, float wr, float wil, float wir, float wel, float wer);
        
        bool IsCorrupt() const;
        bool IsCorruptNew() const;

        PPRData();
		PPRData(const VRtPPR & ppr);
		PPRData(float width, float nl, float nr, float pl, float pr, float wl, float wr);
		PPRData(float width, float nl, float nr, float pl, float pr, float wl, float wr, float wil, float wir, float wel, float wer);
	};

	// ������������� ������ �������
	PPRData ppr;
	
	// ������ ������� �� ������� (������ ����)
	std::queue<VRtPPR> pprFull;
	
	// ����� ����
	unsigned long rfid;

	// �������� � �������
	std::queue<VRtPicture> pict;

	// �����
	std::queue<VRtUgon> ugon;

	VRtImportedParameter();
	VRtImportedParameter(const inm_record & inm);
	VRtImportedParameter(const inmx_record & inm);
};

// ����������� ������ ����������
class VRtParameter : public VRtImportedParameter
{
public:

	// ����� �����
	size_t number;
	
	// ��������� ���������
	double absCoord;	// ���������� ����������
	VWayCoord wayCoord;	// ������� ����������
	bool accurate; // ������� ������������ ����������

	int orig_direction; // ����������/�������� ���������� (����-1) ���������� 
	int direction;		// ����������/�������� ������������� ���������� 

	// ����������� � ��������������� �������
	struct ExpandedButtons
	{
		bool swxLeft, swxRight, swoLeft, swoRight, 
			bridge, radar, station, sw, picket, kms;
		side_t curve;
		WAY_MARKER_TYPE marker;
		long marker_index;
		
		ExpandedButtons() : swxLeft(false), swxRight(false), 
			swoLeft(false), swoRight(false), bridge(false),
			radar(false), station(false), sw(false), 
			picket(false), kms(false),
			curve(SD_UNDEFINED), marker(MARKER_NONE), marker_index(0)
		{
		};
		
		bool IsSwitch() const
		{
			return swoLeft || swoRight || sw || swxLeft || swxRight;
		};
		
		void SetMarker(WAY_MARKER_TYPE in_marker_type, long in_marker_index)
		{
			if (AllowedMarkerType(in_marker_type))
			{
				marker=in_marker_type;
				marker_index=in_marker_index;
			};
		};

	} expandedButtons;

	// -----------------------------------------------------------------------------

	// ��������� ����������
	VSpeeds prkSpd, tmpSpd, testrunSpd; // �������� �� �������, ��������� ��������

	long threadSide;		// ���.���� �� ������
	long chrouteIndex;		// ��������� ������ - warning �� ��������������� �����

	// ����� ���������� � �� �� ������
	float nlevelStr, nwidthStr; 

	bool isCurveLevel;
	ZLINE_TYPE zltPlan;

	long spala;				// ��� ����
	long bridge, bridgeLen;	// ����� - ������ � �����
	long tonnel, tonnelLen;	// ������� - ������ � �����

	VRtParameter();
	VRtParameter(const VRtImportedParameter & p);

	void TurnAround();
};

