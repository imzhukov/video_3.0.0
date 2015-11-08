#include "rtparam.h"
#include "inm.h"
#include "VCorrupt.h"

#include <float.h>

#include <iomanip>

void VRtImportedParameter::Param<float>::CorrectWears()
{
	const float low_range = 0;

	if (this->value < low_range)	
	{
		this->value = low_range;
		this->corrupt = false;
	}
}

void VRtImportedParameter::Param<float>::CorrectZazor()
{
	if (this->value > 0) 
	{
		this->value = round(this->value);
	}
}

template <class T> void VRtImportedParameter::Param<T>::Import(const T & src, 
					 T minValue, T maxValue, T defaultValue,
					 uint32_t error, uint32_t errorCode)
{
    if (errorCode > 0)
    {
        this->corrupt = (error & errorCode) == errorCode;
    }
    else
    {
        this->corrupt = false;
    }

    if (_isnan(src) || src <= minValue || src >= maxValue)
    {
        this->value = defaultValue;        
        this->corrupt = true;
    }
    else
    {
        this->value = src;
    }
}

template <class T> VRtImportedParameter::Param<T> 
VRtImportedParameter::Import(const T & src, 
					 T minValue, T maxValue, T defaultValue,
					 uint32_t errorCode)
{
	Param<T> prm;
	
	prm.Import(src, minValue, maxValue, defaultValue, this->corrupt, errorCode);

/*	if (errorCode > 0)
	{
		prm.corrupt = (this->corrupt & errorCode) == errorCode;
	}
	else
	{
		prm.corrupt = false;
	}

	if (_isnan(src) || src < minValue || src > maxValue)
	{
		prm.value = defaultValue;
		prm.corrupt = true;
	}
	else
	{
		prm.value = src;
	}*/

	return prm;
}

VRtImportedParameter::VRtImportedParameter() : isOk(false), wayPassed(0), glzAbsCoord(0), 
corrupt(0), buttons(0), speed(0), dpp(0), kadres(0), time(0), rfid(0)
{
	this->width.value = 1520;
	this->widthFront.value = 1520;
	this->widthBack.value = 1520;
	this->zazorLeft.value = -1;
	this->zazorRight.value = -1;
	this->mzLeft.value = -1;
	this->mzRight.value = -1;
}

VRtImportedParameter::VRtImportedParameter(const inm_record & inm): isOk(true), wayPassed(inm.wayPassed), glzAbsCoord(inm.absCoord),
speed(inm.speed), dpp(inm.dpp), kadres(0), time(0), rfid(inm.spalaId)
{
	this->corrupt = uint32_t(inm.corrupt) + (uint32_t(inm.corrupt2) << 16);
	this->buttons = uint16_t(inm.buttons) + (uint16_t(inm.buttons2) << 8);

	this->pitLeft.Import    (inm.pitLeft,    -500,   500,    0, this->corrupt, ERROR_PIT_LEFT);
	this->pitRight.Import   (inm.pitRight,   -500,   500,    0, this->corrupt, ERROR_PIT_RIGHT);
	this->richLeft.Import   (inm.richLeft,   -500,   500,    0, this->corrupt, ERROR_RICH_LEFT);
	this->richRight.Import  (inm.richRight,  -500,   500,    0, this->corrupt, ERROR_RICH_RIGHT);
	this->distShort.Import  (inm.distShort,  -500,   500,    0, this->corrupt, ERROR_DIST_SHORT);
	this->distLong.Import   (inm.distLong,   -500,   500,    0, this->corrupt, ERROR_DIST_LONG);
	this->width.Import      (inm.width,      1480,  1560, 1520, this->corrupt, ERROR_WIDTH);
	this->wear.Import       (inm.wear,       -100,   100,    0, this->corrupt, ERROR_WEAR);
	this->widthFront.Import (inm.widthFront, 1480,  1560, 1520, this->corrupt, ERROR_WIDTH_FRONT);
	this->widthBack.Import  (inm.widthBack,  1480,  1560, 1520, this->corrupt, ERROR_WIDTH_BACK);
	this->wearLeft.Import   (inm.wearLeft,   -100,   100,    0, this->corrupt, ERROR_WEAR_LEFT); 
	this->wearRight.Import  (inm.wearRight,  -100,   100,    0, this->corrupt, ERROR_WEAR_RIGHT);
	this->tempLeft.Import   (inm.tempLeft,   -100,   100,    0, this->corrupt,  ERROR_NONE);
	this->tempRight.Import  (inm.tempRight,  -100,   100,    0, this->corrupt, ERROR_NONE);
	this->level.Import      (inm.level,      -500,   500,    0, this->corrupt, ERROR_LEVEL);
	this->gradient.Import   (inm.gradient,   -500,   500,    0, this->corrupt, ERROR_GRADIENT);
	this->uskVert.Import    (inm.uskVert,    -500,   500,    0, this->corrupt, ERROR_NONE);
	this->uskHor.Import     (inm.uskHor,     -500,   500,    0, this->corrupt, ERROR_NONE);
	this->gyroLat.Import    (inm.gyroLat,    -100,   100,    0, this->corrupt, ERROR_NONE);
	this->gyroLong.Import   (inm.gyroLong,   -100,   100,    0, this->corrupt, ERROR_NONE);
	this->curv.Import       (inm.curv,       -500,   500,    0, this->corrupt, ERROR_CURV);
	this->kurs.Import       (inm.kurs,     -15000, 15000,    0, this->corrupt, ERROR_KURS);
	this->kren.Import       (inm.gyroKren,   -100,   100,    0, this->corrupt, ERROR_NONE);
	this->tangage.Import    (inm.gyroTangage,-100,   100,    0, this->corrupt, ERROR_NONE);
	this->gpsTime.Import    (inm.gpsTime,    -100,   100,    0, this->corrupt, ERROR_NONE);
	this->gpsLat.Import     (inm.gpsLat,     -100,   100,    0, this->corrupt, ERROR_NONE);
	this->gpsLong.Import    (inm.gpsLong,    -100,   100,    0, this->corrupt, ERROR_NONE);
	this->gpsAlt.Import     (inm.gpsAlt,    -1000, 10000,    0, this->corrupt, ERROR_NONE);
	this->zazorLeft.Import  (inm.zazorLeft,   -10,   100,   -1, this->corrupt, ERROR_ZAZOR_LEFT);
	this->zazorRight.Import (inm.zazorRight,  -10,   100,   -1, this->corrupt, ERROR_ZAZOR_RIGHT);
	this->mzLeft.Import     (inm.mzLeft,      -10,   100,   -1, this->corrupt, ERROR_MAG_ZAZOR_LEFT);
	this->mzRight.Import    (inm.mzRight,     -10,   100,   -1, this->corrupt, ERROR_MAG_ZAZOR_RIGHT);

	this->zazorLeft.CorrectZazor();
	this->zazorRight.CorrectZazor();
	this->mzLeft.CorrectZazor();
	this->mzRight.CorrectZazor();

	this->wear.CorrectWears();
	this->wearLeft.CorrectWears();
	this->wearRight.CorrectWears();

	if (inm.wayStep != 0) this->kadres = static_cast<unsigned long>(inm.wayPassed / inm.wayStep);

    if (inm.version <= 3)
    {
        this->ppr = PPRData(inm.pprShablon, 
                    inm.pprNakLeft * 100.0, inm.pprNakRight * 100.0,
                    inm.pprPodLeft * 100.0, inm.pprPodRight * 100.0, 
                    inm.pprWearLeft, inm.pprWearRight);
    }
    else
    {
 	    this->ppr = PPRData(inm.pprShablon,
	                inm.pprNakLeft * 100.0, inm.pprNakRight * 100.0, 
	                inm.pprPodLeft * 100.0, inm.pprPodRight * 100.0, 
	                inm.pprWearLeft, inm.pprWearRight,
	                inm.pprWearIntLeft, inm.pprWearIntRight,
	                inm.pprWearExtLeft, inm.pprWearExtRight);    
	}

    if (inm.version > 3)
    {
        this->time = uint32_t(inm.timeLow) + (uint32_t(inm.timeHigh) << 16);
    }
}

VRtImportedParameter::VRtImportedParameter(const inmx_record & inm): isOk(true), wayPassed(inm.wayPassed), glzAbsCoord(inm.absCoord),
speed(inm.speed), dpp(inm.dpp), kadres(0), time(0), rfid(inm.spalaId)
{
	this->corrupt = uint32_t(inm.corrupt) + (uint32_t(inm.corrupt2) << 16);
	this->buttons = uint16_t(inm.buttons) + (uint16_t(inm.buttons2) << 8);

	this->pitLeft.Import    (inm.pitLeft,    -500,   500,    0, this->corrupt, ERROR_PIT_LEFT);
	this->pitRight.Import   (inm.pitRight,   -500,   500,    0, this->corrupt, ERROR_PIT_RIGHT);
	this->richLeft.Import   (inm.richLeft,   -500,   500,    0, this->corrupt, ERROR_RICH_LEFT);
	this->richRight.Import  (inm.richRight,  -500,   500,    0, this->corrupt, ERROR_RICH_RIGHT);
	this->distShort.Import  (inm.distShort,  -500,   500,    0, this->corrupt, ERROR_DIST_SHORT);
	this->distLong.Import   (inm.distLong,   -500,   500,    0, this->corrupt, ERROR_DIST_LONG);
	this->width.Import      (inm.width,      1480,  1560, 1520, this->corrupt, ERROR_WIDTH);
	this->wear.Import       (inm.wear,       -100,   100,    0, this->corrupt, ERROR_WEAR);
	this->widthFront.Import (inm.widthFront, 1480,  1560, 1520, this->corrupt, ERROR_WIDTH_FRONT);
	this->widthBack.Import  (inm.widthBack,  1480,  1560, 1520, this->corrupt, ERROR_WIDTH_BACK);
	this->wearLeft.Import   (inm.wearLeft,   -100,   100,    0, this->corrupt, ERROR_WEAR_LEFT); 
	this->wearRight.Import  (inm.wearRight,  -100,   100,    0, this->corrupt, ERROR_WEAR_RIGHT);
	this->tempLeft.Import   (inm.tempLeft,   -100,   100,    0, this->corrupt,  ERROR_NONE);
	this->tempRight.Import  (inm.tempRight,  -100,   100,    0, this->corrupt, ERROR_NONE);
	this->level.Import      (inm.level,      -500,   500,    0, this->corrupt, ERROR_LEVEL);
	this->gradient.Import   (inm.gradient,   -500,   500,    0, this->corrupt, ERROR_GRADIENT);
	this->uskVert.Import    (inm.uskVert,    -500,   500,    0, this->corrupt, ERROR_NONE);
	this->uskHor.Import     (inm.uskHor,     -500,   500,    0, this->corrupt, ERROR_NONE);
	this->gyroLat.Import    (inm.gyroLat,    -100,   100,    0, this->corrupt, ERROR_NONE);
	this->gyroLong.Import   (inm.gyroLong,   -100,   100,    0, this->corrupt, ERROR_NONE);
	this->curv.Import       (inm.curv,       -500,   500,    0, this->corrupt, ERROR_CURV);
	this->kurs.Import       (inm.kurs,     -15000, 15000,    0, this->corrupt, ERROR_KURS);
	this->kren.Import       (inm.gyroKren,   -100,   100,    0, this->corrupt, ERROR_NONE);
	this->tangage.Import    (inm.gyroTangage,-100,   100,    0, this->corrupt, ERROR_NONE);
	this->gpsTime.Import    (inm.gpsTime,    -100,   100,    0, this->corrupt, ERROR_NONE);
	this->gpsLat.Import     (inm.gpsLat,     -100,   100,    0, this->corrupt, ERROR_NONE);
	this->gpsLong.Import    (inm.gpsLong,    -100,   100,    0, this->corrupt, ERROR_NONE);
	this->gpsAlt.Import     (inm.gpsAlt,    -1000, 10000,    0, this->corrupt, ERROR_NONE);
	this->zazorLeft.Import  (inm.zazorLeft,   -10,   100,   -1, this->corrupt, ERROR_ZAZOR_LEFT);
	this->zazorRight.Import (inm.zazorRight,  -10,   100,   -1, this->corrupt, ERROR_ZAZOR_RIGHT);
	this->mzLeft.Import     (inm.mzLeft,      -10,   100,   -1, this->corrupt, ERROR_MAG_ZAZOR_LEFT);
	this->mzRight.Import    (inm.mzRight,     -10,   100,   -1, this->corrupt, ERROR_MAG_ZAZOR_RIGHT);

	this->zazorLeft.CorrectZazor();
	this->zazorRight.CorrectZazor();
	this->mzLeft.CorrectZazor();
	this->mzRight.CorrectZazor();

	this->wear.CorrectWears();
	this->wearLeft.CorrectWears();
	this->wearRight.CorrectWears();

	if (inm.wayStep != 0) this->kadres = static_cast<unsigned long>(inm.wayPassed / inm.wayStep);

    if (inm.version <= 3)
    {
        this->ppr.Import(inm.pprShablon, 
                    inm.pprNakLeft * 100.0, inm.pprNakRight * 100.0,
                    inm.pprPodLeft * 100.0, inm.pprPodRight * 100.0, 
                    inm.pprWearLeft, inm.pprWearRight);
    }
    else
    {
 	    this->ppr.Import(inm.pprShablon,
	                inm.pprNakLeft * 100.0, inm.pprNakRight * 100.0, 
	                inm.pprPodLeft * 100.0, inm.pprPodRight * 100.0, 
	                inm.pprWearLeft, inm.pprWearRight,
	                inm.pprWearIntLeft, inm.pprWearIntRight,
	                inm.pprWearExtLeft, inm.pprWearExtRight);    
	}

    if (inm.version > 3)
    {
        this->time = uint32_t(inm.timeLow) + (uint32_t(inm.timeHigh) << 16);
    }
}

bool VRtImportedParameter::PPRData::IsCorrupt() const
{
	return this->width.corrupt || 
		this->wearLeft.corrupt || this->wearRight.corrupt ||
		this->podLeft.corrupt || this->podRight.corrupt ||
		this->nakLeft.corrupt || this->nakRight.corrupt;
}

bool VRtImportedParameter::PPRData::IsCorruptNew() const
{
    return
		this->wearIntLeft.corrupt || this->wearIntRight.corrupt ||
		this->wearExtLeft.corrupt || this->wearExtRight.corrupt;
}

VRtImportedParameter::PPRData::PPRData() 
{
    this->isDefault = true;
	this->width.value = 1520.0;
	this->podLeft.value = 5.0;
	this->podRight.value = 5.0;
	this->nakLeft.value = 5.0;
	this->nakRight.value = 5.0;
}

VRtImportedParameter::PPRData::PPRData(const VRtPPR & ppr)
{
    this->Import(ppr.thread_width, 
                ppr.nak_left, ppr.nak_right, 
                ppr.pod_left, ppr.pod_right, 
                ppr.wear_sum_left, ppr.wear_sum_right, 
                ppr.wear_int_left, ppr.wear_int_right,
                ppr.wear_ext_left, ppr.wear_ext_right);
}

VRtImportedParameter::PPRData::PPRData(float w, float nl, float nr, float pl, float pr, float wl, float wr)
{
    this->Import(w, nl, nr, pl, pr, wl, wr);
}

VRtImportedParameter::PPRData::PPRData(float w, float nl, float nr, float pl, float pr, 
                float wl, float wr, float wil, float wir, float wel, float wer)
{
    this->Import(w, nl, nr, pl, pr, wl, wr, wil, wir, wel, wer);
}

void VRtImportedParameter::PPRData::Import(float w, float nl, float nr, float pl, float pr, float wl, float wr)
{
    this->isDefault = false;
    
    this->width.Import      (w,  1000, 2000, 1520, 0, ERROR_NONE);
    this->nakLeft.Import    (nl, -100,  100,    5, 0, ERROR_NONE);
    this->nakRight.Import   (nr, -100,  100,    5, 0, ERROR_NONE);
    this->podLeft.Import    (pl, -100,  100,    5, 0, ERROR_NONE);
    this->podRight.Import   (pr, -100,  100,    5, 0, ERROR_NONE);
    this->wearLeft.Import   (wl, -100,  100,    0, 0, ERROR_NONE);
    this->wearRight.Import  (wr, -100,  100,    0, 0, ERROR_NONE);

    this->wearLeft.CorrectWears();
    this->wearRight.CorrectWears();
}

void VRtImportedParameter::PPRData::Import(float w, float nl, float nr, float pl, float pr, 
                float wl, float wr, float wil, float wir, float wel, float wer)
{
    this->Import(w, nl, nr, pl, pr, wl, wr);

    if (wil == -100 || wir == -100 || wel == -100 || wer == -100)
    {
        int Z = 0;
    }

    this->wearIntLeft.Import   (wil, -100,  100,    0, 0, ERROR_NONE);
    this->wearIntRight.Import  (wir, -100,  100,    0, 0, ERROR_NONE);
    this->wearExtLeft.Import   (wel, -100,  100,    0, 0, ERROR_NONE);
    this->wearExtRight.Import  (wer, -100,  100,    0, 0, ERROR_NONE);

    this->wearIntLeft.CorrectWears();
    this->wearIntRight.CorrectWears();
    this->wearExtLeft.CorrectWears();
    this->wearExtRight.CorrectWears();
    
    if (this->IsCorruptNew())
    {
        int Z = 0;
    }
    
}

VRtParameter::VRtParameter() : VRtImportedParameter(), number(0), 
absCoord(0), orig_direction(0), direction(0), 
threadSide(0), nlevelStr(0), nwidthStr(1520),
spala(-1), isCurveLevel(false), zltPlan(ZLINE_UNDEFINED),
bridge(-1), bridgeLen(0), tonnel(-1), tonnelLen(0), 
chrouteIndex(0), accurate(false)
{}

VRtParameter::VRtParameter(const VRtImportedParameter & p) : 
VRtImportedParameter(), number(0), 
absCoord(0), orig_direction(0), direction(0), 
threadSide(0), nlevelStr(0), nwidthStr(1520), spala(-1), 
isCurveLevel(false), zltPlan(ZLINE_UNDEFINED),
bridge(-1), bridgeLen(0), tonnel(-1), tonnelLen(0), 
chrouteIndex(0), accurate(false)
{
	this->isOk = p.isOk;
	this->wayPassed = p.wayPassed;
	this->glzAbsCoord = p.glzAbsCoord;
	this->corrupt = p.corrupt;
	this->buttons = p.buttons;
	this->speed = p.speed;
	this->pitLeft = p.pitLeft;
	this->pitRight = p.pitRight;
	this->richLeft = p.richLeft;
	this->richRight = p.richRight;
	this->distShort = p.distShort;
	this->distLong = p.distLong;
	this->width = p.width;
	this->wear = p.wear;
	this->widthFront = p.widthFront;
	this->widthBack = p.widthBack;
	this->wearLeft = p.wearLeft;
	this->wearRight = p.wearRight;
	this->tempLeft = p.tempLeft;
	this->tempRight = p.tempRight;
	this->level = p.level;
	this->gradient = p.gradient;
	this->uskVert = p.uskVert;
	this->uskHor = p.uskHor;
	this->gyroLat = p.gyroLat;
	this->gyroLong = p.gyroLong;
	this->curv = p.curv;
	this->kurs = p.kurs;
	this->gpsTime = p.gpsTime;
	this->gpsLat = p.gpsLat;
	this->gpsLong = p.gpsLong;
	this->gpsAlt = p.gpsAlt;
	this->zazorLeft = p.zazorLeft;
	this->zazorRight = p.zazorRight;
	this->mzLeft = p.mzLeft;
	this->mzRight = p.mzRight;
	this->ppr = p.ppr;
	this->pprFull = p.pprFull;
    this->rfid = p.rfid;
	this->kren = p.kren;
	this->tangage = p.tangage;
	this->dpp = p.dpp;
	this->kadres = p.kadres;
	this->pict = p.pict;
	this->ugon = p.ugon;
	this->time = p.time;
}

void VRtParameter::TurnAround()
{
	this->level.value *= -1.0;
	this->curv.value *= -1.0;
	this->gradient.value *= -1.0;
	this->kurs.value += (float) M_PI;
	std::swap(this->pitLeft, this->pitRight);
	std::swap(this->richLeft, this->richRight);
	this->richLeft.value *= -1.0;
	this->richRight.value *= -1.0;
	std::swap(this->tempLeft, this->tempRight);
	std::swap(this->zazorLeft, this->zazorRight);
	std::swap(this->mzLeft, this->mzRight);
	std::swap(this->wearLeft, this->wearRight);
}

// ------------------------------------------------------------------------------------

VRtPPR::VRtPPR() : dpp(0), nak_left(0), nak_right(0), pod_left(0), pod_right(0),
wear_vert_left(0), wear_vert_right(0), wear_ext_left(0), wear_ext_right(0),
wear_int_left(0), wear_int_right(0), wear_sum_left(0), wear_sum_right(0),
patch_left(false), patch_right(false), ramp_left(false), ramp_right(false),
cross_left(false), cross_right(false)
{}

VRtPPR::VRtPPR(const ppr_record & ppr) : dpp(ppr.dpp), 
nak_left(ppr.nak[0] * 100), nak_right(ppr.nak[1] * 100),
pod_left(ppr.pod[0] * 100), pod_right(ppr.pod[1] * 100),
wear_vert_left(ppr.wear_vert[0]), wear_vert_right(ppr.wear_vert[1]),
wear_ext_left(ppr.wear_ext[0]), wear_ext_right(ppr.wear_ext[1]),
wear_int_left(ppr.wear_int[0]), wear_int_right(ppr.wear_int[1]),
wear_sum_left(ppr.wear_sum[0]), wear_sum_right(ppr.wear_sum[1]),
thread_width(ppr.thread_width),
patch_left(ppr.patch[0]), patch_right(ppr.patch[1]),
ramp_left(ppr.ramp[0]), ramp_right(ppr.ramp[1]),
cross_left(ppr.cross[0]), cross_right(ppr.cross[1])
{
	for (size_t i = 0; i < 240; ++i)
	{
		this->points_left.push_back(point_t(
			double(ppr.points_left[i].x) / 100.0, 
			double(ppr.points_left[i].y) / 100.0));

		this->points_right.push_back(point_t(
			double(ppr.points_right[i].x) / 100.0, 
			double(ppr.points_right[i].y) / 100.0));
	}
}

bool VRtPPR::WriteToFolder(const std::wstring & folder, 
						   const VWayCoord & wc) const
{
	std::wstringstream shname;
	shname << wc.km << L"km" << int(wc.m) << L"m";
	shname << this->dpp << L".dat";
	
	VFileName fname(folder, shname.str());
	std::ofstream s(fname.GetPath().c_str());
	if (!s.good()) return false;

	s << std::fixed << std::setprecision(2);
//	s.imbue(std::locale(""));
//	s << "XLeft\tYLeft\tXRight\tXRight" << std::endl;
	for (size_t i = 0; i < this->points_left.size(); ++i)
	{
		s << this->points_left[i].x << "\t" << this->points_left[i].y << "\t";
		s << this->points_right[i].x << "\t" << this->points_right[i].y << std::endl;
	}
	s.close();

	return true;
}

template <class T> static void Write(char * buf, const T & data, size_t & position)
{
	memcpy(buf + position, &data, sizeof(data));
	position += sizeof(data);
}

bool VRtPPR::WriteToBuffer(char * buffer) const
{
	memset(buffer, 0, 4096);
	memcpy(buffer, "PPR0", 4);
	size_t position = 4;

	Write<unsigned long>(buffer, this->dpp, position);

	Write<unsigned long>(buffer, (unsigned long) this->points_left.size(), position);
	for (size_t i = 0; i < this->points_left.size(); ++i)
	{
		Write<float>(buffer, (float) points_left[i].x, position);;
		Write<float>(buffer, (float) points_left[i].y, position);
	}

	Write<unsigned long>(buffer, (unsigned long) this->points_right.size(), position);
	for (size_t i = 0; i < this->points_right.size(); ++i)
	{
		Write<float>(buffer, (float) points_right[i].x, position);;
		Write<float>(buffer, (float) points_right[i].y, position);
	}

	Write<float>(buffer, this->nak_left, position);
	Write<float>(buffer, this->nak_right, position);
	Write<float>(buffer, this->pod_left, position);
	Write<float>(buffer, this->pod_right, position);
	Write<float>(buffer, this->wear_vert_left, position);
	Write<float>(buffer, this->wear_vert_right, position);
	Write<float>(buffer, this->wear_ext_left, position);
	Write<float>(buffer, this->wear_ext_right, position);
	Write<float>(buffer, this->wear_int_left, position);
	Write<float>(buffer, this->wear_int_right, position);
	Write<float>(buffer, this->wear_sum_left, position);
	Write<float>(buffer, this->wear_sum_right, position);
	Write<float>(buffer, this->thread_width, position);

	unsigned long flags = 0;
	if (this->patch_left)	flags |= 0x00000001;
	if (this->patch_right)	flags |= 0x00000002;
	if (this->ramp_left)	flags |= 0x00000004;
	if (this->ramp_right)	flags |= 0x00000008;
	if (this->cross_left)	flags |= 0x00000010;
	if (this->cross_right)	flags |= 0x00000020;
	Write<unsigned long>(buffer, flags, position);

	return true;
}

/*
template <class T> static void Write(std::ostream & os, const T & data, size_t & position)
{
	os.write((const char*) &data, sizeof(data));
	position += sizeof(data);
}

bool VRtPPR::WriteToBinary(std::ostream & os) const
{
	if (!os.good()) return false;
	size_t position = 0;

	const char * sync = "SYNC";
	os.write(sync, sizeof(sync));
	position +=4;

	Write<unsigned long>(os, this->dpp, position);

	Write<unsigned long>(os, (unsigned long) this->points_left.size(), position);
	for (size_t i = 0; i < this->points_left.size(); ++i)
	{
		Write<float>(os, (float) points_left[i].x, position);;
		Write<float>(os, (float) points_left[i].y, position);
	}

	Write<unsigned long>(os, (unsigned long) this->points_right.size(), position);
	for (size_t i = 0; i < this->points_right.size(); ++i)
	{
		Write<float>(os, (float) points_right[i].x, position);;
		Write<float>(os, (float) points_right[i].y, position);
	}

	Write<float>(os, this->nak_left, position);
	Write<float>(os, this->nak_right, position);
	Write<float>(os, this->pod_left, position);
	Write<float>(os, this->pod_right, position);
	Write<float>(os, this->wear_vert_left, position);
	Write<float>(os, this->wear_vert_right, position);
	Write<float>(os, this->wear_ext_left, position);
	Write<float>(os, this->wear_ext_right, position);
	Write<float>(os, this->wear_int_left, position);
	Write<float>(os, this->wear_int_right, position);
	Write<float>(os, this->wear_sum_left, position);
	Write<float>(os, this->wear_sum_right, position);
	Write<float>(os, this->thread_width, position);

	unsigned long flags = 0;
	if (this->patch_left)	flags |= 0x00000001;
	if (this->patch_right)	flags |= 0x00000002;
	if (this->ramp_left)	flags |= 0x00000004;
	if (this->ramp_right)	flags |= 0x00000008;
	if (this->cross_left)	flags |= 0x00000010;
	if (this->cross_right)	flags |= 0x00000020;
	Write<unsigned long>(os, flags, position);

	size_t bufLeft = 4096 - position;
	char * buf = new char[bufLeft];
	::memset(buf, 0, bufLeft);
	os.write(buf, bufLeft);
	delete[] buf;

	return true;
}
*/

template <class T> T Read(const char * buf, size_t position)
{
	T data;
	memcpy(&data, buf + position, sizeof(data));
	position += sizeof(data);
	return data;
}

bool VRtPPR::ReadFromBuffer(const char * buf)
{
	if (memcmp(buf, "PPR0", 4) == 0)
	{
		size_t position = 4;
		this->dpp = Read<unsigned long>(buf, position);

		unsigned long cnt = Read<unsigned long>(buf, position);
		this->points_left.clear();
		for (unsigned long i = 0; i < cnt; ++i)
		{
			float x = Read<float>(buf, position);
			float y = Read<float>(buf, position);
			this->points_left.push_back(point_t(x, y));
		}

		cnt = Read<unsigned long>(buf, position);
		this->points_right.clear();
		for (unsigned long i = 0; i < cnt; ++i)
		{
			float x = Read<float>(buf, position);
			float y = Read<float>(buf, position);
			this->points_right.push_back(point_t(x, y));
		}

		this->nak_left = Read<float>(buf, position);
		this->nak_right = Read<float>(buf, position);
		this->pod_left = Read<float>(buf, position);
		this->pod_right = Read<float>(buf, position);
		this->wear_vert_left = Read<float>(buf, position);
		this->wear_vert_right = Read<float>(buf, position);
		this->wear_ext_left = Read<float>(buf, position);
		this->wear_ext_right = Read<float>(buf, position);
		this->wear_int_left = Read<float>(buf, position);
		this->wear_int_right = Read<float>(buf, position);
		this->wear_sum_left = Read<float>(buf, position);
		this->wear_sum_right = Read<float>(buf, position);
		this->thread_width = Read<float>(buf, position);

		unsigned long flags = Read<unsigned long>(buf, position);
		this->patch_left = ((flags & 0x00000001) == 0x00000001);
		this->patch_right = ((flags & 0x00000002) == 0x00000002);
		this->ramp_left = ((flags & 0x00000004) == 0x00000004);
		this->ramp_right = ((flags & 0x00000008) == 0x00000008);
		this->cross_left = ((flags & 0x00000010) == 0x00000010);
		this->cross_right = ((flags & 0x00000020) == 0x00000020);

		return true;
	}

	else
	{
		return false;
	}
}

/*
template <class T> T Read(std::istream & is, size_t & position)
{
	T data;
	is.read((char *) &data, sizeof(data));
	position += sizeof(data);
	return data;
}

bool VRtPPR::ReadFromBinary(std::istream & is)
{
	if (!is.good()) return false;

	char sync[5] = {0};
	is.read((char *)sync, 4);
	if (strncmp(sync, "SYNC", 4) != 0) return false;
	size_t position = 4;

	this->dpp = Read<unsigned long>(is, position);

	unsigned long cnt = Read<unsigned long>(is, position);
	this->points_left.clear();
	for (unsigned long i = 0; i < cnt; ++i)
	{
		float x = Read<float>(is, position);
		float y = Read<float>(is, position);
		this->points_left.push_back(point_t(x, y));
	}

	cnt = Read<unsigned long>(is, position);
	this->points_right.clear();
	for (unsigned long i = 0; i < cnt; ++i)
	{
		float x = Read<float>(is, position);
		float y = Read<float>(is, position);
		this->points_right.push_back(point_t(x, y));
	}

	this->nak_left = Read<float>(is, position);
	this->nak_right = Read<float>(is, position);
	this->pod_left = Read<float>(is, position);
	this->pod_right = Read<float>(is, position);
	this->wear_vert_left = Read<float>(is, position);
	this->wear_vert_right = Read<float>(is, position);
	this->wear_ext_left = Read<float>(is, position);
	this->wear_ext_right = Read<float>(is, position);
	this->wear_int_left = Read<float>(is, position);
	this->wear_int_right = Read<float>(is, position);
	this->wear_sum_left = Read<float>(is, position);
	this->wear_sum_right = Read<float>(is, position);
	this->thread_width = Read<float>(is, position);

	unsigned long flags = Read<unsigned long>(is, position);
	this->patch_left = ((flags & 0x00000001) == 0x00000001);
	this->patch_right = ((flags & 0x00000002) == 0x00000002);
	this->ramp_left = ((flags & 0x00000004) == 0x00000004);
	this->ramp_right = ((flags & 0x00000008) == 0x00000008);
	this->cross_left = ((flags & 0x00000010) == 0x00000010);
	this->cross_right = ((flags & 0x00000020) == 0x00000020);

	size_t bufLeft = 4096 - position;
	char * buf = new char[bufLeft];
	is.read(buf, bufLeft);

	return true;
}
*/

// ------------------------------------------------------------------------------------

VRtPicture::VRtPicture() : dpp(0)
{}

VRtPicture::VRtPicture(const zaz_header & zaz, unsigned char * rawdata) : dpp(zaz.dpp),
data(rawdata, zaz.datalen)
{}

bool VRtPicture::WriteToFolder(const std::wstring & folder,
							   const VWayCoord & wc) const
{
	std::wstringstream shname;
	shname << wc.km << L"km" << int(wc.m) << L"m";
	shname << this->dpp << L".jpg";

	VFileName fname(folder, shname.str());
	std::ofstream s(fname.GetPath().c_str());
	if (!s.good()) return false;

	unsigned char * buf = new unsigned char[this->data.size()];
	for (size_t i = 0; i < this->data.size(); ++i)
	{
		buf[i] = this->data[i];
	}

	s.write((char*) buf, this->data.size());
	s.close();
	delete[] buf;
	return true;
}
