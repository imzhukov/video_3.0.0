#pragma once

#include "VAprinfo.h"
#include "types.h"
#include "inm.h"
#include "markers.h"

#include <valarray>
#include <queue>

// Измерения ППР
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

// Картинки с зазорами
class VRtPicture
{
public:

	unsigned long dpp;
	std::valarray<unsigned char> data;

	VRtPicture();
	VRtPicture(const zaz_header & zaz, unsigned char * rawdata);

	bool WriteToFolder(const std::wstring & folder, const VWayCoord & wc) const;
};

// угоны
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

// параметры, импортируемые у ПЭВМ-1
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

	// Служебные параметры
	bool isOk;			// признак заполнения параметра реальными данными
	double wayPassed;	// пройденный путь
	double glzAbsCoord; // абс.координата от Глазкова
	uint32_t corrupt;	// сбои измерений
	uint16_t buttons;	// кнопки оператора
	float speed;		// скорость вагона
	unsigned long time; // время поездки

	// Измерительные параметры
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

	// полный профиль рельса
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

	// сфазированный полный профиль
	PPRData ppr;
	
	// полный профиль от егорова (полный кадр)
	std::queue<VRtPPR> pprFull;
	
	// метки шпал
	unsigned long rfid;

	// картинка с зазором
	std::queue<VRtPicture> pict;

	// угоны
	std::queue<VRtUgon> ugon;

	VRtImportedParameter();
	VRtImportedParameter(const inm_record & inm);
	VRtImportedParameter(const inmx_record & inm);
};

// расширенный список параметров
class VRtParameter : public VRtImportedParameter
{
public:

	// номер кадра
	size_t number;
	
	// Служебные параметры
	double absCoord;	// абсолютная координата
	VWayCoord wayCoord;	// путевая координата
	bool accurate; // признак корректности координаты

	int orig_direction; // нарастание/убывание измеренной (пэвм-1) координаты 
	int direction;		// нарастание/убывание пересчитанной координаты 

	// развернутые и отфильтрованные стрелки
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

	// Априорная информация
	VSpeeds prkSpd, tmpSpd, testrunSpd; // Скорости по приказу, временные скорости

	long threadSide;		// Рих.нить на прямой
	long chrouteIndex;		// Индикация съезда - warning по пересчелкиванию знака

	// нормы возвышения и шк на прямых
	float nlevelStr, nwidthStr; 

	bool isCurveLevel;
	ZLINE_TYPE zltPlan;

	long spala;				// Тип шпал
	long bridge, bridgeLen;	// Мосты - статус и длина
	long tonnel, tonnelLen;	// Тоннели - статус и длина

	VRtParameter();
	VRtParameter(const VRtImportedParameter & p);

	void TurnAround();
};

