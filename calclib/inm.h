#pragma once

#pragma pack(push,1)
struct inm_record
{
	double absCoord;
	float width;
	float level;
	float distShort;
	float distLong;
	float gradient;
	float pitLeft;
	float pitRight;
	unsigned short corrupt;
	unsigned short count;
	float widthFront;
	float height;
	float richLeft;
	float richRight;
	float curv;
	unsigned char buttons;
	float speed;
	float kurs;
	float widthBack;		/// PRR

	float tempLeft;
	float zazorLeft;
	float wear;
	float wearLeft;
	float knLeft;
	float tempRight;
	float zazorRight;
	float ugonRight;
	float wearRight;
	float knRight;
	float uskVert;
	float uskHor;
	float ugonLeft;
	unsigned short corrupt2;
	unsigned char buttons2;		/// PRX
	
	double wayPassed;
	float wayStep;
	float sensors[24];

	// Гироскоп
	float gyroKren, gyroTangage, gyroKurs, 
		gyroLat, gyroLong, gyroSpeed;

	unsigned char buffer[128];	/// мягкие отбраковки

	// GPS
	unsigned short gpsDataSize; 
	unsigned long gpsDp;
	unsigned long gpsReserve[2];
	double gpsLat, gpsLong, gpsAlt, gpsTime;

	float mzLeft;
	float mzRight;
	unsigned long spalaId;
	unsigned long brdTime;

	float pprWayPassed;
	float pprShablon;
	float pprNakLeft, pprNakRight;
	float pprPodLeft, pprPodRight;
	float pprWearLeft, pprWearRight;

	float brdWayPassed;
	unsigned long dpp;
	
	float pprWearIntLeft, pprWearIntRight;
	float pprWearExtLeft;
	unsigned short timeLow;
	unsigned short version;
	float pprWearExtRight;
	unsigned short timeHigh;
};

#define INMX_SIGNATURE "FRAME01_PROGRESS"

struct inmx_record
{
	char signature[16];
	double absCoord;
	float width;
	float level;
	float distShort;
	float distLong;
	float gradient;
	float pitLeft;
	float pitRight;
	unsigned short corrupt;
	unsigned short count;
	float widthFront;
	float height;
	float richLeft;
	float richRight;
	float curv;
	unsigned char buttons;
	float speed;
	float kurs;
	float widthBack;		/// PRR

	float tempLeft;
	float zazorLeft;
	float wear;
	float wearLeft;
	float knLeft;
	float tempRight;
	float zazorRight;
	float ugonRight;
	float wearRight;
	float knRight;
	float uskVert;
	float uskHor;
	float ugonLeft;
	unsigned short corrupt2;
	unsigned char buttons2;		/// PRX
	
	double wayPassed;
	float wayStep;
	float sensors[24];

	// Гироскоп
	float gyroKren, gyroTangage, gyroKurs, 
		gyroLat, gyroLong, gyroSpeed;

	unsigned char buffer[128];	/// мягкие отбраковки

	// GPS
	unsigned short gpsDataSize; 
	unsigned long gpsDp;
	unsigned long gpsReserve[2];
	double gpsLat, gpsLong, gpsAlt, gpsTime;

	float mzLeft;
	float mzRight;
	unsigned long spalaId;
	unsigned long brdTime;

	float pprWayPassed;
	float pprShablon;
	float pprNakLeft, pprNakRight;
	float pprPodLeft, pprPodRight;
	float pprWearLeft, pprWearRight;

	float brdWayPassed;
    unsigned long dpp;

	float pprWearIntLeft, pprWearIntRight;
	float pprWearExtLeft;

	unsigned short timeLow;
	unsigned short version;
	float pprWearExtRight;
	unsigned short timeHigh;
	char reserve2[496];
};

#define PPR_SIGNATURE "RAILSPROFILE"

struct ppr_point
{
	short x, y;
};

struct ppr_record
{
	char signature[12];
	unsigned long dpp;

	float pod[2], nak[2];
	float wear_vert[2], wear_ext[2], wear_int[2], wear_sum[2];
	float thread_width, ugon;
	float reserve_f[11];
	bool patch[2], ramp[2], cross[2];
	bool reserve_b[6];
	ppr_point points_left[240], points_right[240];
};

#define ZAZ_SIGNATURE "ZZRPICTR"

struct zaz_header
{
	char signature[8];
	unsigned long dpp;
	unsigned long datalen;
	unsigned long crc32;
};

#define UGON_SIGNATURE "UGON0001"

struct ugon_record
{
	char signature[8];
	unsigned long dpp;
	float ugon_left, ugon_right;
};

#pragma pack(pop)

