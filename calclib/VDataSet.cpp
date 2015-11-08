#include "VDataSet.h"
#include "strlib.h"
#include "VProgressInterface.h"

#include "inm.h"

#include "VFunctionParser.h"
#include "dblib.h"
#include "VLog.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <locale>

#include <float.h>

#include "Shiny.h"

// загружаемые из файла параметры
static PARAMETER_TYPE file_parameters[] = {
	PRM_KURS, PRM_KURS_BACKUP, PRM_GRADIENT, PRM_LEVEL, 
	PRM_WIDTH, PRM_WIDTH_FRONT, PRM_WIDTH_BACK, PRM_PIT_LEFT, PRM_PIT_RIGHT, 
	PRM_CURV, PRM_DIST_SHORT, PRM_DIST_LONG, PRM_RICH_LEFT, PRM_RICH_RIGHT, 
	PRM_TEMP_LEFT, PRM_TEMP_RIGHT, PRM_WEAR_LEFT, PRM_WEAR_RIGHT, PRM_UNI_WEAR_SENSOR, 
	PRM_USK_VERT, PRM_USK_HOR, PRM_LATITUDE, PRM_LONGITUDE, 
	PRM_PPR_WIDTH, PRM_PPR_NAK_LEFT, PRM_PPR_NAK_RIGHT, PRM_PPR_POD_LEFT, PRM_PPR_POD_RIGHT,  
	PRM_PPR_IZN_LEFT, PRM_PPR_IZN_RIGHT, PRM_PPR_IZN_INT_LEFT, PRM_PPR_IZN_INT_RIGHT, PRM_PPR_IZN_EXT_LEFT, PRM_PPR_IZN_EXT_RIGHT,
	PRM_TEST_4, PRM_TEST_7, PRM_TEST_8, 
	PRM_NONE
};

// вычисляемые параметры
static PARAMETER_TYPE calc_parameters[] = {
	PRM_SPEED, PRM_MID_WIDTH, PRM_MID_RICH_LEFT, PRM_MID_RICH_RIGHT, 
	PRM_MID_LEVEL, PRM_MID_CURV, PRM_UNI_WEAR, PRM_PROFILE, PRM_DIFF_PROFILE, 
	PRM_APROX_PROFILE, PRM_JAGGY_PROFILE, PRM_PLAN_OFFSET, PRM_PLAN_OFFSET_FICTIONAL,
	PRM_MODEL_PLAN_OFFSET, PRM_ANP_REAL_HIGH, PRM_ANP_REAL_HIGH_BT, PRM_ANP_REAL_PASS, PRM_ANP_REAL_TRUCK, 
	PRM_JAGGY_PLAN, PRM_JAGGY_PLAN_ON_LINES, PRM_PSI_REAL_HIGH, PRM_PSI_REAL_HIGH_BT, 
	PRM_PSI_REAL_PASS, PRM_PSI_REAL_TRUCK, PRM_LEVEL_ZLINE, 
	PRM_CURV_FROM_KURS, PRM_DIFF_LEVEL, PRM_DIFF_LEVEL_MODEL, 
	PRM_DIFF_WIDTH_FRONT, PRM_DIFF_WIDTH_BACK,
	PRM_NONE 
};

// параметры типа double
static PARAMETER_TYPE double_parameters[] = {
	PRM_GPS_TIME, PRM_GPS_LATITUDE, PRM_GPS_LONGITUDE, PRM_GPS_ALTITUDE, 
	PRM_NONE 
};

static PARAMETER_TYPE zazor_parameters[] = 
{
	PRM_ZAZOR_LEFT, PRM_ZAZOR_RIGHT, PRM_MAG_ZAZOR_LEFT, 
	PRM_MAG_ZAZOR_RIGHT, PRM_NONE
};

// параметр возвращается как VParameter
bool VDataSet::IsParameter(PARAMETER_TYPE prm)
{
	return VDataSet::IsFileParameter(prm) || VDataSet::IsCalcParameter(prm);
}

// параметр загружается из файла
bool VDataSet::IsFileParameter(PARAMETER_TYPE prm)
{
	for (size_t i = 0; file_parameters[i] != PRM_NONE; ++i)
		if (file_parameters[i] == prm) return true;

	return false;
}

// параметр вычисляется на лету
bool VDataSet::IsCalcParameter(PARAMETER_TYPE prm)
{
	for (size_t i = 0; calc_parameters[i] != PRM_NONE; ++i)
		if (calc_parameters[i] == prm) return true;

	return false;
}

// параметр возвращается как VDoubleParameter
bool VDataSet::IsDoubleParameter(PARAMETER_TYPE prm)
{

	for (size_t i = 0; double_parameters[i] != PRM_NONE; ++i)
		if (double_parameters[i] == prm) return true;

	return false;
}

// параметр возвращается как VZazorList
bool VDataSet::IsZazor(PARAMETER_TYPE param)
{
	for (size_t i = 0; zazor_parameters[i] != PRM_NONE; ++i)
		if (zazor_parameters[i] == param) return true;

	return false;
}

// Сообщение об ошибке при загрузке параметра
std::wstring VDataSet::FormatErrorMessage(PARAMETER_TYPE param, const std::wstring & msg)
{
	std::wstring message = L"Не загружен параметр: ";
	message += GetUserParamName(param);

	if (!msg.empty())
		message += L" (" + msg + L"";

	return message;
}

/// Конструктор
VDataSet::VDataSet() : open_mode(0), message(ERRMSG_DATA_NOT_LOADED), canting_left_overstep(SD_LEFT), canting_right_overstep(SD_RIGHT),
	ext_wear_left_overstep(SD_LEFT), ext_wear_right_overstep(SD_RIGHT)
{
	::ZeroMemory(&this->header, sizeof(this->header));

	for (int i = 0; file_parameters[i] != PRM_NONE; ++i)
		this->params[file_parameters[i]] = VParameterPtr(new VParameter);

	int Z = 0;
}

/// Конструктор - с открытием локального файла
VDataSet::VDataSet(const VFileName & fn) : open_mode(0), message(ERRMSG_DATA_NOT_LOADED), canting_left_overstep(SD_LEFT), canting_right_overstep(SD_RIGHT),
	ext_wear_left_overstep(SD_LEFT), ext_wear_right_overstep(SD_RIGHT)
{
	::ZeroMemory(&this->header, sizeof(this->header));

	for (int i = 0; file_parameters[i] != PRM_NONE; ++i)
		this->params[file_parameters[i]] = VParameterPtr(new VParameter);

	this->OpenFile(fn);

	int Z = 0;
}

/// Конструктор - с открытием удаленного файла
VDataSet::VDataSet(VIBDatabase & db, const VFileName & fn) : open_mode(0), message(ERRMSG_DATA_NOT_LOADED), canting_left_overstep(SD_LEFT), canting_right_overstep(SD_RIGHT),
	ext_wear_left_overstep(SD_LEFT), ext_wear_right_overstep(SD_RIGHT)
{
	::ZeroMemory(&this->header, sizeof(this->header));

	for (int i = 0; file_parameters[i] != PRM_NONE; ++i)
		this->params[file_parameters[i]] = VParameterPtr(new VParameter);

	this->OpenFile(db, fn);

	int Z = 0;
}

// Вернуть имя пути
std::string VDataSet::GetWay() const
{
	if (strncmp(this->header.Way, this->header.WayLong, 3) == 0)
	{
		return std::string(this->header.WayLong);
	}
	else
	{
		return std::string(this->header.Way);
	}
}

// Информация о магистральном проезде
bool VDataSet::SetMagistralInfo(uint32_t dirCode, 
								const std::string & way, 
								const std::string & dirName, 
								const std::string & peregon)
{
	if (dirCode == 0 || way.empty()) return false;

	this->header.StCode = 0;
	this->header.Dir = dirCode;

	memset(this->header.Way, 0, 4);
	strncpy(this->header.Way, way.c_str(), 3);

	memset(this->header.WayLong, 0, 32);
	strncpy(this->header.WayLong, way.c_str(), 31);

	if (!dirName.empty()) 
	{
		memset(this->header.DirName, 0, 256);
		strncpy(this->header.DirName, dirName.c_str(), 255);
	}

	if (!peregon.empty()) 
	{
		memset(this->header.Peregon, 0, 256);
		strncpy(this->header.Peregon, peregon.c_str(), 255);
	}

	return true;
}

// Информация о станционном проезде
bool VDataSet::SetStationInfo(uint32_t stCode, 
							  const std::string & stName, 
							  const std::string & park, 
							  const std::string & way)
{
	if (stCode == 0 || park.empty() || way.empty()) return false;

	this->header.Dir = 0;
	this->header.StCode = stCode;

	memset(this->header.DirName, 0, 256);
	if (!stName.empty()) strncpy(this->header.DirName, stName.c_str(), 255);

	memset(this->header.Peregon, 0, 256);
	strncpy(this->header.Peregon, park.c_str(), 255);

	memset(this->header.Way, 0, 4);
	strncpy(this->header.Way, way.c_str(), 3);

	memset(this->header.WayLong, 0, 32);
	strncpy(this->header.WayLong, way.c_str(), 31);

	return true;
}

// Открыть удаленный файл для чтения
bool VDataSet::OpenFile(VIBDatabase & db, const VFileName & fileName)
{
	if (this->file.Loaded())
	{
		this->message = L"Файл уже открыт как локальный";
		return false;
	}
	else if (this->remote.Loaded())
	{
		return true;
	}
	else
	{
		if (this->remote.Open(db, fileName))
		{
			this->header = *(this->remote.GetUserHeader());
			this->open_mode = OPEN_MODE_READ;
			this->fn = fileName;

			if (!this->fileAprinfo.Load(this->remote, DHT_APRINFO))
			{
				this->message = L"Не загружен параметр: ";
				this->message += GetParamName(PRM_APRINFO);
				this->message += L" (" + this->fileAprinfo.GetErrorMessage() + L"";
				return false;
			}
			else
			{
				this->message = ERRMSG_OK;
				return true;
			}

			this->message = ERRMSG_OK;
			return true;
		}
		else
		{
			this->message = L"Не удалось открыть файл для чтения ";
			this->message += L"(";
			this->message += fileName.GetPath();
			this->message += L"";
			return false;
		}
	}
}

// открыть локальный файл на чтение, если еще не.
bool VDataSet::OpenFile(const VFileName & fileName)						
{
	if (this->remote.Loaded())
	{
		this->message = L"Файл уже открыт как удаленный";
		return false;
	}
	else if (this->file.Loaded()) 
	{
		return true;
	}
	else 
	{
		this->file.Open(fileName.GetPath().c_str(), OPEN_MODE_READ);
		this->fn = fileName;
		if (this->file.Loaded())
		{
			this->header = *(this->file.GetUserHeader());
			this->open_mode = OPEN_MODE_READ;

			if (!this->fileAprinfo.Load(this->file, DHT_APRINFO))
			{
				this->message = L"Не загружен параметр: ";
				this->message += GetParamName(PRM_APRINFO);
				this->message += L" (" + this->fileAprinfo.GetErrorMessage() + L"";
				return false;
			}
			else
			{
				this->message = ERRMSG_OK;
				return true;
			}
		}
		else
		{
			this->message = L"Не удалось открыть файл для чтения ";
			this->message += L"(";
			this->message += fileName.GetPath();
			this->message += L"";
			return false;
		}
	}
}

// Создать новый файл
bool VDataSet::CreateFile(const VFileName & fileName)
{
	this->file.Open(fileName.GetPath().c_str(), OPEN_MODE_CREATE);
	if (this->file.Loaded())
	{
		this->header.CreateTime = VTime(time(0));
		this->open_mode = OPEN_MODE_CREATE;
		this->message = ERRMSG_OK;
		return true;
	}
	else 
	{
		this->message = L"Не удалось создать файл ";
		this->message += L"(";
		this->message += fileName.GetPath();
		this->message += L"";
		return false;
	}
}

// Проверка наличия данных в файле
bool VDataSet::DataExists(PARAMETER_TYPE param)
{
	if (!this->FileIsLoaded()) return false;
	unsigned long code = GetFileCodeByParam(param);
	if (code == 0) return false;
	else return this->file.GetBlockIndex(code) != BAD_SIZE;
}

// Таймштамп блока данных в файле
VTime VDataSet::GetDataBlockTime(PARAMETER_TYPE param) 
{
	VBasicParameter bprm;
    unsigned long code = GetFileCodeByParam(param);
	if (code == 0) return VTime::ZeroTime();	
	if (!bprm.Load(this->file, code, false)) return VTime::ZeroTime();
	
	VTime tt = bprm.GetUpdateTime();
	if (tt.IsZero()) tt = bprm.GetCreateTime();
	if (tt.IsZero()) tt = this->GetCreateTime();
	return tt;
}

// Количество записей в блоке данных в файле
size_t VDataSet::GetDataBlockRecCount(PARAMETER_TYPE param) 
{
	VBasicParameter bprm;
    unsigned long code = GetFileCodeByParam(param);
	if (code == 0) return BAD_INDEX;	
	if (bprm.Load(this->file, code, false)) return bprm.GetRecCount();
	else return BAD_INDEX;
}

// Проверка наличия блока
bool VDataSet::CheckDataBlock(PARAMETER_TYPE param)
{
	VBasicParameter bprm;
	unsigned long code = GetFileCodeByParam(param);
	if (code == 0) return false;	
	return bprm.Load(this->file, code, false);
}

// загрузка параметров
bool VDataSet::Load(PARAMETER_TYPE param)
{
	if (!this->FileIsLoaded()) 
	{	
		this->message = L"Не загружен файл.";
		return false;
	}

	bool result = false;
	size_t kadrCount = 0;
	if (param != PRM_ABS_COORD)
	{
		if (!this->Load(PRM_ABS_COORD)) return false;
		kadrCount = this->GetAbsCoord().GetCount();
	}

	// перебираем параметры
	switch(param)
	{
	case PRM_APRINFO:
		if (this->FileIsRemote())
			result = this->aprInfo.Load(this->remote, DHT_APRINFO);
		else
			result = this->aprInfo.Load(this->file, DHT_APRINFO);

		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->aprInfo.GetErrorMessage());
		}
		break;

	case PRM_COMMENT:
		if (this->FileIsRemote())
			result = this->comment.Load(this->remote);
		else
			result = this->comment.Load(this->file);

		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->comment.GetErrorMessage());
		}
		break;

	case PRM_ABS_COORD:

		if (this->FileIsRemote())
			result = this->absCoord.Load(this->remote, DHT_ABS_COORD);
		else
			result = this->absCoord.Load(this->file, DHT_ABS_COORD);

		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->absCoord.GetErrorMessage());
		}
		break;

	case PRM_CORRUPT:
		if (this->FileIsRemote())
			result = this->corrupt.Load(this->remote);
		else
			result = this->corrupt.Load(this->file);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->corrupt.GetErrorMessage());
		}
		break;

	case PRM_DPP:
		if (this->FileIsRemote())
			result = this->dpp.Load(this->remote, DHT_DPP);
		else
			result = this->dpp.Load(this->file, DHT_DPP);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->dpp.GetErrorMessage());
		}
		break;

	case PRM_SPEED:
		if (this->FileIsRemote())
			result = this->speed.Load(this->remote, DHT_SPEED);
		else
			result = this->speed.Load(this->file, DHT_SPEED);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->speed.GetErrorMessage());
		}
		break;

	case PRM_OVRS2:
		if (this->FileIsRemote())
			result = this->ovrs2.Load(this->remote, DHT_OVRS2);
		else
			result = this->ovrs2.Load(this->file, DHT_OVRS2);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->ovrs2.GetErrorMessage());
		}
		break;

	case PRM_OVRS3:
		if (this->FileIsRemote())
			result = this->ovrs3.Load(this->remote, DHT_OVRS3, kadrCount);
		else
			result = this->ovrs3.Load(this->file, DHT_OVRS3, kadrCount);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->ovrs3.GetErrorMessage());
		}
		break;

	case PRM_OP_MARKS:
		if (this->FileIsRemote())
			result = this->opMarks.Load(this->remote, DHT_OP_MARKS, kadrCount);
		else
			result = this->opMarks.Load(this->file, DHT_OP_MARKS, kadrCount);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->opMarks.GetErrorMessage());
		}
		break;

	case PRM_MAG_ZAZOR:
		if (this->FileIsRemote())
			result = this->magZazor.Load(this->remote, DHT_MAG_ZAZOR, kadrCount);
		else
			result = this->magZazor.Load(this->file, DHT_MAG_ZAZOR, kadrCount);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->magZazor.GetErrorMessage());
		}
		break;

	case PRM_ZAZOR_LEFT:
		if (!this->Load(PRM_CORRUPT)) break;

		if (this->FileIsRemote())
			result = this->zazorLeft.Load(this->remote, DHT_ZAZOR_LEFT, this->GetCorrupt().GetCorruptChannel(PRM_ZAZOR_LEFT));
		else
			result = this->zazorLeft.Load(this->file, DHT_ZAZOR_LEFT, this->GetCorrupt().GetCorruptChannel(PRM_ZAZOR_LEFT));

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->zazorLeft.GetErrorMessage());
		}
		break;

	case PRM_ZAZOR_RIGHT:
		if (!this->Load(PRM_CORRUPT)) break;

		if (this->FileIsRemote())
			result = this->zazorRight.Load(this->remote, DHT_ZAZOR_RIGHT, this->GetCorrupt().GetCorruptChannel(PRM_ZAZOR_RIGHT));
		else
			result = this->zazorRight.Load(this->file, DHT_ZAZOR_RIGHT, this->GetCorrupt().GetCorruptChannel(PRM_ZAZOR_RIGHT));

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->zazorRight.GetErrorMessage());
		}
		break;

	case PRM_MAG_ZAZOR_LEFT:
		if (!this->Load(PRM_CORRUPT)) break;

		if (this->FileIsRemote())
			result = this->magZazorLeft.Load(this->remote, DHT_MAG_ZAZOR_LEFT, this->GetCorrupt().GetCorruptChannel(PRM_MAG_ZAZOR_LEFT));
		else
			result = this->magZazorLeft.Load(this->file, DHT_MAG_ZAZOR_LEFT, this->GetCorrupt().GetCorruptChannel(PRM_MAG_ZAZOR_LEFT));

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->magZazorLeft.GetErrorMessage());
		}
		break;

	case PRM_MAG_ZAZOR_RIGHT:
		if (!this->Load(PRM_CORRUPT)) break;

		if (this->FileIsRemote())
			result = this->magZazorRight.Load(this->remote, DHT_MAG_ZAZOR_RIGHT, this->GetCorrupt().GetCorruptChannel(PRM_MAG_ZAZOR_RIGHT));
		else
			result = this->magZazorRight.Load(this->file, DHT_MAG_ZAZOR_RIGHT, this->GetCorrupt().GetCorruptChannel(PRM_MAG_ZAZOR_RIGHT));

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->magZazorRight.GetErrorMessage());
		}
		break;

	case PRM_UGON_LEFT:
		if (this->FileIsRemote())
			result = this->ugonLeft.Load(this->remote, DHT_UGON_LEFT, kadrCount);
		else
			result = this->ugonLeft.Load(this->file, DHT_UGON_LEFT, kadrCount);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->ugonLeft.GetErrorMessage());
		}
		break;

	case PRM_UGON_RIGHT:
		if (this->FileIsRemote())
			result = this->ugonRight.Load(this->remote, DHT_UGON_RIGHT, kadrCount);
		else
			result = this->ugonRight.Load(this->file, DHT_UGON_RIGHT, kadrCount);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->ugonRight.GetErrorMessage());
		}
		break;

	case PRM_SPALA_MARKER:
		if (this->FileIsRemote())
			result = this->spalaMarker.Load(this->remote, DHT_SPALA_MARKER, kadrCount);
		else
			result = this->spalaMarker.Load(this->file, DHT_SPALA_MARKER, kadrCount);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->spalaMarker.GetErrorMessage());
		}
		break;

	case PRM_HISTORY:
		if (this->FileIsRemote())
			result = this->history.Load(this->remote);
		else
			result = this->history.Load(this->file);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->history.GetErrorMessage());
		}
		break;

	case PRM_CONTROL_AREA:
		if (this->FileIsRemote())
			result = this->ctrlAreas.Load(this->remote, DHT_CONTROL_AREA, kadrCount);
		else
			result = this->ctrlAreas.Load(this->file, DHT_CONTROL_AREA, kadrCount);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->ctrlAreas.GetErrorMessage());
		}
		break;

	case PRM_SIDEWAYS:
		if (this->FileIsRemote())
			return this->sideways.Load(this->remote, DHT_SIDEWAYS, kadrCount);
		else
			return this->sideways.Load(this->file, DHT_SIDEWAYS, kadrCount);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->sideways.GetErrorMessage());
		}
		break;

	case PRM_CURVE_MARKS:
		if (!this->Load(PRM_ABS_COORD)) 
		{
			break;
		}
			
		if (this->FileIsRemote())
			result = this->curves.Load(this->remote, DHT_CURVE_MARKS, this->absCoord);
		else
			result = this->curves.Load(this->file, DHT_CURVE_MARKS, this->absCoord);
		
		if (this->Load(PRM_GPS_LATITUDE) && this->Load(PRM_GPS_LONGITUDE))
		{
			VDoubleParameter & latitude = this->GetDoubleParameter(PRM_GPS_LATITUDE);
			VDoubleParameter & longitude = this->GetDoubleParameter(PRM_GPS_LONGITUDE);

			this->GetCurves(PRM_CURVE_MARKS).SetGeoCoord(latitude, longitude);

		}

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->curves.GetErrorMessage());
		}
		break;

	case PRM_MODEL_CURVES:
		if (!this->Load(PRM_ABS_COORD))
		{
			break;
		}

		if (this->FileIsRemote())
			result = this->modelCurves.Load(this->remote, DHT_MODEL_CURVES, this->absCoord);
		else
			result = this->modelCurves.Load(this->file, DHT_MODEL_CURVES, this->absCoord);

		if (this->Load(PRM_GPS_LATITUDE) && this->Load(PRM_GPS_LONGITUDE))
		{
			this->GetCurves(PRM_MODEL_CURVES).SetGeoCoord(
				this->GetDoubleParameter(PRM_GPS_LATITUDE),
				this->GetDoubleParameter(PRM_GPS_LONGITUDE));
		}

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->modelCurves.GetErrorMessage());
		}
		break;

	case PRM_LEVEL_ZLINE:
		if (!this->Load(PRM_ABS_COORD)) break;
		if (!this->Load(PRM_APRINFO)) break;

		if (!this->Load(PRM_CURVE_MARKS)) 
		{
			LOG_WARNING(L"В файле отсутствует разметка кривых");		
		}

		result = this->zl.Load(this->GetAbsCoord(), 
			this->GetCurves(PRM_CURVE_MARKS), 
			true, &this->aprInfo);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->zl.GetErrorMessage());
		}
		break;

	case PRM_LINES:
		if (!this->Load(PRM_ABS_COORD)) break;
		this->Load(PRM_CURVE_MARKS);
		this->Load(PRM_LINE_MARKS);
		this->Load(PRM_SIDEWAYS);

		result = this->lines.Load(this->GetCurves(), this->GetLineMarks(), this->GetSideways(), 
			this->fileAprinfo, this->absCoord);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->lines.GetErrorMessage());
		}
		break;

	case PRM_LINE_MARKS:
		if (!this->Load(PRM_ABS_COORD)) break;

		// загружаем метки
		if (this->FileIsRemote())
			result = this->lineMarks.Load(this->remote, DHT_LINE_MARKS, this->absCoord, this->fileAprinfo);
		else
			result = this->lineMarks.Load(this->file, DHT_LINE_MARKS, this->absCoord, this->fileAprinfo);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->lineMarks.GetErrorMessage());
		}
		break;

	case PRM_PROF_MARKS:
		if (!this->Load(PRM_ABS_COORD))
		{
			break;
		}

		if (this->FileIsRemote())
			result = this->profMarks.Load(this->remote, DHT_PROF_MARKS, this->absCoord, this->fileAprinfo);
		else
			result = this->profMarks.Load(this->file, DHT_PROF_MARKS, this->absCoord, this->fileAprinfo);

		if (!result)
		{
			this->message = FormatErrorMessage(param, this->profMarks.GetErrorMessage());
		}
		break;

	// параметры с каналом отбраковки
	case PRM_WIDTH:	
	case PRM_WIDTH_FRONT:
	case PRM_WIDTH_BACK:
	case PRM_DIST_SHORT:
	case PRM_DIST_LONG:
	case PRM_PIT_LEFT:
	case PRM_PIT_RIGHT:
	case PRM_RICH_LEFT:	
	case PRM_RICH_RIGHT:
	case PRM_GRADIENT:
	case PRM_LEVEL:	
	case PRM_CURV:	
	case PRM_KURS:
	case PRM_WEAR_LEFT:
	case PRM_WEAR_RIGHT:
	case PRM_UNI_WEAR_SENSOR:
	case PRM_PPR_WIDTH:
	case PRM_PPR_NAK_LEFT:
	case PRM_PPR_NAK_RIGHT:
	case PRM_PPR_POD_LEFT:
	case PRM_PPR_POD_RIGHT:
	case PRM_PPR_IZN_LEFT:
	case PRM_PPR_IZN_RIGHT:
	case PRM_PPR_IZN_EXT_LEFT:
	case PRM_PPR_IZN_EXT_RIGHT:
	case PRM_PPR_IZN_INT_LEFT:
	case PRM_PPR_IZN_INT_RIGHT:
		if (!this->Load(PRM_CORRUPT)) 
			break;

		if (this->FileIsRemote())	
			result = this->GetParameter(param).VTemplateParameter<float>::Load(this->remote, GetFileCodeByParam(param), this->GetCorrupt().GetCorruptChannel(param));
		else
			result = this->GetParameter(param).VTemplateParameter<float>::Load(this->file, GetFileCodeByParam(param), this->GetCorrupt().GetCorruptChannel(param));

		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->GetParameter(param).GetErrorMessage());
		}
		break;

	// параметры без канала отбраковки
	case PRM_KURS_BACKUP:
	case PRM_TEMP_LEFT:
	case PRM_TEMP_RIGHT:
	case PRM_USK_VERT:
	case PRM_USK_HOR:
	case PRM_LATITUDE:
	case PRM_LONGITUDE:
	case PRM_TEST_4:
	case PRM_TEST_7:
	case PRM_TEST_8:

		if (this->FileIsRemote())	
			result = this->GetParameter(param).VTemplateParameter<float>::Load(this->remote, GetFileCodeByParam(param));
		else
			result = this->GetParameter(param).VTemplateParameter<float>::Load(this->file, GetFileCodeByParam(param));

		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->GetParameter(param).GetErrorMessage());
		}
		break;

	// параметры double
	case PRM_GPS_TIME:
	case PRM_GPS_LATITUDE:
	case PRM_GPS_LONGITUDE:
	case PRM_GPS_ALTITUDE:

		if (this->FileIsRemote())
			result = this->GetDoubleParameter(param).Load(this->remote, GetFileCodeByParam(param));
		else
			result = this->GetDoubleParameter(param).Load(this->file, GetFileCodeByParam(param));

		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->GetDoubleParameter(param).GetErrorMessage());
		}
		break;

	// объединенный износ
	case PRM_UNI_WEAR:
		result = this->wear.Load(*this, false);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->wear.GetErrorMessage());
		}
		break;

	case PRM_MID_LEVEL:		
		if (this->midLevel.IsLoaded()) break;
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_LEVEL))
		{
			break;
		}
		this->midLevel.Create(0xFFFFFFFF, absCoord.GetCount(), 0, 0);
		ParameterMean(this->GetParameter(PRM_LEVEL), this->midLevel, this->absCoord, 
			OPTIONS.GetOption(L"LEVEL_MEAN_BASE").GetValue<double>());
		result = true;
		break;

	case PRM_MID_CURV:
		if (this->midCurv.IsLoaded()) break;
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_CURV))
		{
			break;
		}
		this->midCurv.Create(0xFFFFFFFF, absCoord.GetCount(), 0, 0);
		ParameterMean(this->GetParameter(PRM_CURV), this->midCurv, this->absCoord, 
			OPTIONS.GetOption(L"CURV_MEAN_BASE").GetValue<double>());
		result = true;
		break;

    case PRM_MID_RICH_LEFT:
        if (this->midRichLeft.IsLoaded()) break;
        if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_RICH_LEFT))
        {
            break;
        }
		this->midRichLeft.Create(0xFFFFFFFF, absCoord.GetCount(), 0, 0);
		ParameterMean(this->GetParameter(PRM_RICH_LEFT), this->midRichLeft, this->absCoord, 
			OPTIONS.GetOption(L"CURV_MEAN_BASE").GetValue<double>());
		result = true;
		break;

    case PRM_MID_RICH_RIGHT:
        if (this->midRichRight.IsLoaded()) break;
        if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_RICH_RIGHT))
        {
            break;
        }
		this->midRichRight.Create(0xFFFFFFFF, absCoord.GetCount(), 0, 0);
		ParameterMean(this->GetParameter(PRM_RICH_RIGHT), this->midRichRight, this->absCoord, 
			OPTIONS.GetOption(L"CURV_MEAN_BASE").GetValue<double>());
		result = true;
		break;
		
	case PRM_MID_WIDTH:
	    if (this->midWidth.IsLoaded()) break;
	    if(!this->Load(PRM_ABS_COORD) || !this->Load(PRM_WIDTH))
	    {
	        break;
	    }
		this->midWidth.Create(0xFFFFFFFF, absCoord.GetCount(), 0, 0);
		ParameterMean(this->GetParameter(PRM_WIDTH), this->midWidth, this->absCoord, 
			OPTIONS.GetOption(L"WIDTH_MEAN_BASE").GetValue<double>());
		result = true;
		break;

	case PRM_PROFILE:
		if ((!this->aprInfo.IsLoaded() && !this->Load(PRM_APRINFO)) ||
			!this->Load(PRM_ABS_COORD) || !this->Load(PRM_GRADIENT))
		{
			break;
		}
		result = this->profile.Load(this->GetParameter(PRM_GRADIENT), this->absCoord, this->aprInfo);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->profile.GetErrorMessage());
		}
		break;

	case PRM_DIFF_PROFILE:
		if (!this->Load(PRM_APROX_PROFILE))
		{
			break;
		}
		result = this->diff_profile.Load(this->profile, this->aprox_profile);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->diff_profile.GetErrorMessage());
		}
		break;

	case PRM_APROX_PROFILE:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_PROFILE) ||
			!this->Load(PRM_PROF_MARKS) || !this->Load(PRM_PROFILE_SLOPE_ELEMENT))
		{
			break;
		}
		result = this->aprox_profile.Load(this->profile, this->absCoord, this->profile_slope_element/*this->profMarks*/);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->aprox_profile.GetErrorMessage());
		}
		break;
	
	case PRM_JAGGY_PROFILE:
		if (!this->Load(PRM_DIFF_PROFILE) || !this->Load(PRM_ABS_COORD) || !this->Load(PRM_PROF_MARKS))
		{
			break;
		}
		result = this->jaggy_profile.Load(*this);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->jaggy_profile.GetErrorMessage());
		}
		break;

	case PRM_PROFILE_SLOPE_ELEMENT:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_PROFILE) ||
			!this->Load(PRM_APRINFO) || !this->Load(PRM_PROF_MARKS))
		{
			break;
		}
		result = this->profile_slope_element.Load(this->profile, this->profMarks, this->absCoord, this->aprInfo, PRM_PROFILE_SLOPE_ELEMENT);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->profile_slope_element.GetErrorMessage());
		}		
		break;

	case PRM_PROFILE_SLOPE_KM:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_PROFILE) || !this->Load(PRM_APRINFO))
		{
			break;
		}
		result = this->profile_slope_km.Load(this->profile, this->profMarks, this->absCoord, this->aprInfo, PRM_PROFILE_SLOPE_KM);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->profile_slope_km.GetErrorMessage());
		}		
		break;

	case PRM_PROFILE_SLOPE_PK:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_PROFILE) ||
			!this->Load(PRM_APRINFO))
		{
			break;
		}
		result = this->profile_slope_pk.Load(this->profile, this->profMarks, this->absCoord, this->aprInfo, PRM_PROFILE_SLOPE_PK);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->profile_slope_pk.GetErrorMessage());
		}		
		break;

	case PRM_PROFILE_SLOPE_HPK:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_PROFILE) ||
			!this->Load(PRM_APRINFO))
		{
			break;
		}
		result = this->profile_slope_hpk.Load(this->profile, this->profMarks, this->absCoord, this->aprInfo, PRM_PROFILE_SLOPE_HPK);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->profile_slope_hpk.GetErrorMessage());
		}		
		break;

	case PRM_STATISTICS:

		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_LEVEL) || !this->Load(PRM_LEVEL_ZLINE) || 
			!this->Load(PRM_WIDTH) || !this->Load(PRM_PIT_LEFT) || 	!this->Load(PRM_PIT_RIGHT) || 
			!this->Load(PRM_RICH_LEFT) || !this->Load(PRM_RICH_RIGHT) || !this->Load(PRM_APRINFO) ||
			!this->Load(PRM_PLAN_ELEM_LIST))
		{
			break;
		}

		this->Load(PRM_PPR_POD_LEFT);
		this->Load(PRM_PPR_POD_RIGHT);
		this->Load(PRM_PPR_NAK_LEFT);
		this->Load(PRM_PPR_NAK_RIGHT);

		result = this->statistic.Load(this->aprInfo, this->absCoord, this->GetParameter(PRM_LEVEL), 
			this->zl, this->GetParameter(PRM_PIT_LEFT), this->GetParameter(PRM_PIT_RIGHT), 
			this->GetParameter(PRM_WIDTH), this->GetParameter(PRM_RICH_LEFT), this->GetParameter(PRM_RICH_RIGHT),
			this->GetParameter(PRM_PPR_POD_LEFT), this->GetParameter(PRM_PPR_POD_RIGHT),
			this->GetParameter(PRM_PPR_NAK_LEFT), this->GetParameter(PRM_PPR_NAK_RIGHT),
			this->planElem);

		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->statistic.GetErrorMessage());
		}
		break;

	case PRM_PLAN:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_KURS))
		{
			break;
		}
		//Широта используется для учета схождения мередианов, но не обязательна, 
		//потому ее загрузка не проверяется
		this->Load(PRM_LATITUDE);
		this->Load(PRM_LONGITUDE);
		result = this->plan.Load(*this);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->plan.GetErrorMessage());
		}
		break;

	case PRM_IDEAL_PLAN:
		if (!this->Load(PRM_KURS) || !this->Load(PRM_ABS_COORD) ||
			!this->Load(PRM_PLAN_ELEM_LIST) ||
			!this->Load(PRM_PLAN))
		{
			break;
		}
		result = this->ideal_plan.Load(*this);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->ideal_plan.GetErrorMessage());
		}
		break;

	case PRM_MODEL_IDEAL_PLAN:
		if (!this->Load(PRM_KURS) || 
			!this->Load(PRM_ABS_COORD) ||
			!this->Load(PRM_MODEL_PLAN_ELEM_LIST) || 
			!this->Load(PRM_MODEL_CURVES) ||
			!this->Load(PRM_PLAN))
		{
			break;
		}
		result = this->model_ideal_plan.Load(*this);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->model_ideal_plan.GetErrorMessage());
		}
		break;

	case PRM_PLAN_OFFSET:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_PLAN) ||
			!this->Load(PRM_IDEAL_PLAN))
		{
			break;
		}
		result = this->plan_offset.Load(*this);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->plan_offset.GetErrorMessage());
		}
		break;
		
	case PRM_PLAN_OFFSET_FICTIONAL:
	{
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_PLAN))
		{
			break;
		}
		//Загружаем "фиктивный идеальный план" 
		VPlanElemList fiction_elem_list;
		fiction_elem_list.Load(*this, false, true, false);
		VIdealPlan fiction_plan(false);
		fiction_plan.Load(this->GetParameter(PRM_KURS), this->GetPlan(), this->GetAbsCoord(), fiction_elem_list, this->GetCurves());
		result = this->plan_offset_fictional.Load(this->GetPlan(), fiction_plan);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->plan_offset_fictional.GetErrorMessage());
		}
		break;
	}
	case PRM_MODEL_PLAN_OFFSET:
		if (!this->Load(PRM_ABS_COORD) || 
			!this->Load(PRM_PLAN) ||
			!this->Load(PRM_MODEL_IDEAL_PLAN))
		{
			break;
		}
		result = this->model_plan_offset.Load(*this);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->model_plan_offset.GetErrorMessage());
		}
		break;
		
	case PRM_ANP_REAL_HIGH:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_CURV) ||
			!this->Load(PRM_LEVEL))
		{
			break;
		}
		result = this->anp_real_high.Load(*this, high_prk);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->anp_real_high.GetErrorMessage());
		}
		break;
		
	case PRM_ANP_REAL_HIGH_BT:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_CURV) ||
			!this->Load(PRM_LEVEL))
		{
			break;
		}
		result = this->anp_real_high_bt.Load(*this, high_bt_prk);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->anp_real_high_bt.GetErrorMessage());
		}
		break;

	case PRM_ANP_REAL_PASS:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_CURV) ||
			!this->Load(PRM_LEVEL))
		{
			break;
		}
		result = this->anp_real_pass.Load(*this, pass_prk);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->anp_real_pass.GetErrorMessage());
		}
		break;

	case PRM_JAGGY_PLAN:
		if (!this->Load(PRM_PLAN_OFFSET) || !this->Load(PRM_ABS_COORD))
		{
			break;
		}
		result = this->jaggy_plan.Load(*this, false);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->jaggy_plan.GetErrorMessage());
		}
		break;

	case PRM_JAGGY_PLAN_ON_LINES:
		if (!this->Load(PRM_PLAN_OFFSET) || !this->Load(PRM_ABS_COORD))
		{
			break;
		}
		result = this->jaggy_plan_on_lines.Load(*this, true);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->jaggy_plan_on_lines.GetErrorMessage());
		}
		break;


	case PRM_ANP_REAL_TRUCK:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_CURV) || !this->Load(PRM_LEVEL))
		{
			break;
		}
		result = this->anp_real_truck.Load(*this, crg_prk);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->anp_real_truck.GetErrorMessage());
		}
		break;

	case PRM_ANP_MODEL_PSP_HIGH_BT:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_MODEL_CURVES))
		{
			break;
		}
		result = this->anp_model_high_bt.Load(this->absCoord, this->modelCurves, this->aprInfo, high_bt_prk);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->anp_model_high_bt.GetErrorMessage());
		}
		break;

	case PRM_ANP_MODEL_PSP_HIGH:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_MODEL_CURVES))
		{
			break;
		}
		result = this->anp_model_high.Load(this->absCoord, this->modelCurves, this->aprInfo, high_prk);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->anp_model_high.GetErrorMessage());
		}
		break;		

	case PRM_ANP_MODEL_PSP_PASS:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_MODEL_CURVES))
		{
			break;
		}
		result = this->anp_model_pass.Load(this->absCoord, this->modelCurves, this->aprInfo, pass_prk);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->anp_model_pass.GetErrorMessage());
		}
		break;

	case PRM_ANP_MODEL_PSP_TRUCK:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_MODEL_CURVES))
		{
			break;
		}
		result = this->anp_model_truck.Load(this->absCoord, this->modelCurves, this->aprInfo, crg_prk);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->anp_model_truck.GetErrorMessage());
		}
		break;

	case PRM_PSI_REAL_HIGH:
		if (!this->Load(PRM_ANP_REAL_HIGH))
		{
			break;
		}
		result = this->psi_real_high.Load(*this, high_prk);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->psi_real_high.GetErrorMessage());
		}
		break;

	case PRM_PSI_REAL_HIGH_BT:
		if (!this->Load(PRM_ANP_REAL_HIGH_BT))
		{
			break;
		}
		result = this->psi_real_high_bt.Load(*this, high_bt_prk);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->psi_real_high_bt.GetErrorMessage());
		}
		break;

	case PRM_PSI_REAL_PASS:
		if (!this->Load(PRM_ANP_REAL_PASS))
		{
			break;
		}
		result = this->psi_real_pass.Load(*this, pass_prk);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->psi_real_pass.GetErrorMessage());
		}
		break;

	case PRM_PSI_REAL_TRUCK:
		if (!this->Load(PRM_ANP_REAL_TRUCK))
		{
			break;
		}
		result = this->psi_real_truck.Load(*this, crg_prk);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->psi_real_truck.GetErrorMessage());
		}
		break;


	case PRM_PLAN_ELEM_LIST:
		if (!this->Load(PRM_ABS_COORD))
		{
			break;
		}
		this->Load(PRM_SIDEWAYS);
		this->Load(PRM_CURVE_MARKS);
		result =  this->planElem.Load(*this, false, false, true);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->planElem.GetErrorMessage());
		}
		break;

	case PRM_MODEL_PLAN_ELEM_LIST:
		if (!this->Load(PRM_ABS_COORD))
		{
			break;
		}
		this->Load(PRM_SIDEWAYS);
		this->Load(PRM_MODEL_CURVES);
		result = this->modelPlanElem.Load(*this, true, false, true);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->modelPlanElem.GetErrorMessage());
		}
		break;		

	case PRM_CURV_FROM_KURS:

		if (this->curv_from_kurs.IsLoaded()) 
		{
			result = true;
			break;
		}

		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_KURS)) break;

		this->curv_from_kurs.Create(0xFFFFFFFF, absCoord.GetCount(), 0, 0.0f);
		ParameterDerivate(this->GetParameter(PRM_KURS), this->curv_from_kurs, this->absCoord, 1);
		for(size_t i = 0; i < this->curv_from_kurs.GetCount(); i++)
		{
			this->curv_from_kurs.GetItem(i) /= 200000.0;
		}

		result = true;
		break;

	case PRM_DIFF_LEVEL:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_LEVEL) || !this->Load(PRM_CURVE_MARKS))
		{
			break;
		}
		result = this->diff_level.Load(this->curves, this->GetParameter(PRM_LEVEL), this->absCoord, this->GetAprinfo());
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->diff_level.GetErrorMessage());
		}
		break;

	case PRM_DIFF_LEVEL_MODEL:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_LEVEL) || !this->Load(PRM_MODEL_CURVES))
		{
			break;
		}
		result = this->diff_level_model.Load(this->modelCurves, this->GetParameter(PRM_LEVEL), this->absCoord, this->GetAprinfo());
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->diff_level_model.GetErrorMessage());
		}
		break;

	case PRM_DIFF_WIDTH_FRONT:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_WIDTH) || !this->Load(PRM_WIDTH_FRONT))
		{
			break;
		}
		result = this->width_diff_front.Load(this->GetParameter(PRM_WIDTH), this->GetParameter(PRM_WIDTH_FRONT), this->absCoord);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->width_diff_front.GetErrorMessage());
		}
		break;

	case PRM_DIFF_WIDTH_BACK:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_WIDTH) || !this->Load(PRM_WIDTH_BACK))
		{
			break;
		}
		result = this->width_diff_back.Load(this->GetParameter(PRM_WIDTH), this->GetParameter(PRM_WIDTH_BACK), this->absCoord);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->width_diff_back.GetErrorMessage());
		}
		break;

	case PRM_JAGGY_PROFILE_OVERSTEP:
		if (!this->Load(PRM_JAGGY_PROFILE))
		{
			break;
		}		
		result = this->jaggy_profile_overstep.Load(*this);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, L"");
		}
		break;

	case PRM_JAGGY_PLAN_OVERSTEP:
		if (!this->Load(PRM_JAGGY_PLAN))
		{
			break;
		}		
		result = this->jaggy_plan_overstep.Load(*this);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, L"");
		}
		break;

	case PRM_DIFF_WIDTH_FRONT_OVERSTEP:
		if (!this->Load(PRM_DIFF_WIDTH_FRONT))
		{
			break;
		}	
		result = this->diff_width_front_overstep.Load(*this, PRM_DIFF_WIDTH_FRONT);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, L"");
		}
		break;

	case PRM_DIFF_WIDTH_BACK_OVERSTEP:
		if (!this->Load(PRM_DIFF_WIDTH_BACK))
		{
			break;
		}	
		result = this->diff_width_back_overstep.Load(*this, PRM_DIFF_WIDTH_BACK);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, L"");
		}
		break;
		
	case PRM_CANTING_LEFT_OVERSTEP:
		if (!this->Load(PRM_PPR_POD_LEFT))
		{
			break;
		}	
		result = this->canting_left_overstep.Load(*this);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, L"");
		}
		break;
		
	case PRM_CANTING_RIGHT_OVERSTEP:
		if (!this->Load(PRM_PPR_POD_RIGHT))
		{
			break;
		}	
		result = this->canting_right_overstep.Load(*this);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, L"");
		}
		break;

	case PRM_EXT_WEAR_LEFT_OVERSTEP:
		if (!this->Load(PRM_PPR_IZN_EXT_LEFT))
		{
			break;
		}	
		result = this->ext_wear_left_overstep.Load(*this);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, L"");
		}
		break;
		
	case PRM_EXT_WEAR_RIGHT_OVERSTEP:
		if (!this->Load(PRM_PPR_IZN_EXT_RIGHT))
		{
			break;
		}	
		result = this->ext_wear_right_overstep.Load(*this);
		if (!result)
		{
			this->message = this->FormatErrorMessage(param, L"");
		}
		break;


	case PRM_PR_NGR_AREAS:
		if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_APRINFO) ||
			!this->Load(PRM_PPR_POD_LEFT) || !this->Load(PRM_PPR_POD_RIGHT) ||
			!this->Load(PRM_PPR_NAK_LEFT) || !this->Load(PRM_PPR_NAK_RIGHT))
		{
			break;
		}
		result = this->prNgrAreas.Load(this->aprInfo, this->absCoord,
			this->GetParameter(PRM_PPR_POD_LEFT), this->GetParameter(PRM_PPR_POD_RIGHT), 
			this->GetParameter(PRM_PPR_NAK_LEFT), this->GetParameter(PRM_PPR_NAK_RIGHT));

		if (!result)
		{
			this->message = this->FormatErrorMessage(param, this->prNgrAreas.GetErrorMessage());
		}
		break;
	
	case PRM_WAY_MARKERS:
	{
		if (this->Load(PRM_ABS_COORD) && 
			this->Load(PRM_CURV) && 
			this->Load(PRM_KURS) && 
			this->Load(PRM_OP_MARKS))
		{
			this->Load(PRM_GPS_LATITUDE);
			this->Load(PRM_GPS_LONGITUDE);
			this->Load(PRM_SPALA_MARKER);

			result = this->markers.Load(*this);
		}

		if (!result)
		{
			this->message = this->FormatErrorMessage(param, L"");
		}

		break;
	}

	default:

		this->message = this->FormatErrorMessage(param, L"Неизвестный параметр");
		result = false;
	}

	if (result)
	{
		this->piList.insert(std::pair<PARAMETER_TYPE, bool>(param, false));
	}

	return result;
}

// Выгрузка
std::set<PARAMETER_TYPE> VDataSet::Unload(PARAMETER_TYPE param)
{
	std::set<PARAMETER_TYPE> unloaded;

	if (param == PRM_ABS_COORD || param == PRM_EVERYTHING)
	{
		this->absCoord.Unload();
		this->piList.erase(PRM_ABS_COORD);
		unloaded.insert(PRM_ABS_COORD);
	}

	if (param == PRM_APRINFO || param == PRM_EVERYTHING)		
	{
		this->aprInfo.Unload();
		this->piList.erase(PRM_APRINFO);
		unloaded.insert(PRM_APRINFO);
	}

	if (param == PRM_CORRUPT || param == PRM_EVERYTHING)		
	{
		this->corrupt.Unload();
		this->piList.erase(PRM_CORRUPT);
		unloaded.insert(PRM_CORRUPT);
	}

	// ----------------------------------------------------------------------------------

	for (int i = 0; file_parameters[i] != PRM_NONE; ++i)
	{
		if (param == file_parameters[i] || param == PRM_EVERYTHING)
		{
			this->GetParameter(file_parameters[i]).Unload();
			this->piList.erase(file_parameters[i]);
			unloaded.insert(file_parameters[i]);
		}
	}

	for (int i = 0; calc_parameters[i] != PRM_NONE; ++i)
	{
		if (param == calc_parameters[i] || param == PRM_EVERYTHING)
		{
			this->GetParameter(calc_parameters[i]).Unload();
			this->piList.erase(calc_parameters[i]);
			unloaded.insert(calc_parameters[i]);
		}
	}

	for (int i = 0; double_parameters[i] != PRM_NONE; ++i)
	{
		if (param == double_parameters[i] || param == PRM_EVERYTHING)
		{
			this->GetDoubleParameter(double_parameters[i]).Unload();
			this->piList.erase(double_parameters[i]);
			unloaded.insert(double_parameters[i]);
		}
	}

	// ---------------------------------------------------------------------------

	if (param == PRM_OVRS2 || param == PRM_EVERYTHING)		
	{
		this->ovrs2.Unload();
		this->piList.erase(PRM_OVRS2);
		unloaded.insert(PRM_OVRS2);
	}

	if (param == PRM_OVRS3 || param == PRM_EVERYTHING)
	{
		this->ovrs3.Unload();
		this->piList.erase(PRM_OVRS3);
		unloaded.insert(PRM_OVRS3);
	}

	if (param == PRM_OP_MARKS || param == PRM_EVERYTHING)		
	{
		this->opMarks.Unload();
		this->piList.erase(PRM_OP_MARKS);
		unloaded.insert(PRM_OP_MARKS);
	}

	if (param == PRM_MAG_ZAZOR || param == PRM_EVERYTHING)		
	{
		this->magZazor.Unload();
		this->piList.erase(PRM_MAG_ZAZOR);
		unloaded.insert(PRM_MAG_ZAZOR);
	}

	if (param == PRM_ZAZOR_LEFT || param == PRM_EVERYTHING)
	{
		this->zazorLeft.Unload();
		this->piList.erase(PRM_ZAZOR_LEFT);
		unloaded.insert(PRM_ZAZOR_LEFT);
	}

	if (param == PRM_ZAZOR_RIGHT || param == PRM_EVERYTHING)
	{
		this->zazorRight.Unload();
		this->piList.erase(PRM_ZAZOR_RIGHT);
		unloaded.insert(PRM_ZAZOR_RIGHT);
	}

	if (param == PRM_MAG_ZAZOR_LEFT || param == PRM_EVERYTHING)
	{
		this->magZazorLeft.Unload();
		this->piList.erase(PRM_MAG_ZAZOR_LEFT);
		unloaded.insert(PRM_MAG_ZAZOR_LEFT);
	}

	if (param == PRM_MAG_ZAZOR_RIGHT || param == PRM_EVERYTHING)
	{
		this->magZazorRight.Unload();
		this->piList.erase(PRM_MAG_ZAZOR_RIGHT);
		unloaded.insert(PRM_MAG_ZAZOR_RIGHT);
	}

	if (param == PRM_UGON_LEFT || param == PRM_EVERYTHING)	
	{
		this->ugonLeft.Unload();
		this->piList.erase(PRM_UGON_LEFT);
		unloaded.insert(PRM_UGON_LEFT);
	}

	if (param == PRM_UGON_RIGHT || param == PRM_EVERYTHING)	
	{
		this->ugonRight.Unload();
		this->piList.erase(PRM_UGON_RIGHT);
		unloaded.insert(PRM_UGON_RIGHT);
	}

	if (param == PRM_SPALA_MARKER || param == PRM_EVERYTHING)
	{
		this->spalaMarker.Unload();
		this->piList.erase(PRM_SPALA_MARKER);
		unloaded.insert(PRM_SPALA_MARKER);
	}

	if (param == PRM_CURVE_MARKS || param == PRM_EVERYTHING)	
	{
		this->curves.Unload();
		this->piList.erase(PRM_CURVE_MARKS);
		unloaded.insert(PRM_CURVE_MARKS);
	}

	if (param == PRM_MODEL_CURVES || param == PRM_EVERYTHING)
	{
		this->modelCurves.Unload();
		this->piList.erase(PRM_MODEL_CURVES);
		unloaded.insert(PRM_MODEL_CURVES);
	}

	if (param == PRM_PROF_MARKS || param == PRM_EVERYTHING)	
	{
		this->profMarks.Unload();
		this->piList.erase(PRM_MODEL_CURVES);
		unloaded.insert(PRM_PROF_MARKS);
	}

	if (param == PRM_PROFILE_SLOPE_ELEMENT || param == PRM_EVERYTHING)		
	{
		this->profile_slope_element.Unload();
		this->piList.erase(PRM_PROFILE_SLOPE_ELEMENT);
		unloaded.insert(PRM_PROFILE_SLOPE_ELEMENT);
	}

	if (param == PRM_PROFILE_SLOPE_KM || param == PRM_EVERYTHING)		
	{
		this->profile_slope_km.Unload();
		this->piList.erase(PRM_PROFILE_SLOPE_KM);
		unloaded.insert(PRM_PROFILE_SLOPE_KM);
	}

	if (param == PRM_PROFILE_SLOPE_PK || param == PRM_EVERYTHING)		
	{
		this->profile_slope_pk.Unload();
		this->piList.erase(PRM_PROFILE_SLOPE_PK);
		unloaded.insert(PRM_PROFILE_SLOPE_PK);
	}

	if (param == PRM_PROFILE_SLOPE_HPK || param == PRM_EVERYTHING)		
	{
		this->profile_slope_hpk.Unload();
		this->piList.erase(PRM_PROFILE_SLOPE_HPK);
		unloaded.insert(PRM_PROFILE_SLOPE_HPK);
	}

	if (param == PRM_SIDEWAYS || param == PRM_EVERYTHING)		
	{
		this->sideways.Unload();
		this->piList.erase(PRM_SIDEWAYS);
		unloaded.insert(PRM_SIDEWAYS);
	}

	if (param == PRM_STATISTICS || param == PRM_EVERYTHING)
	{
		this->statistic.Unload();
		this->piList.erase(PRM_STATISTICS);
		unloaded.insert(PRM_STATISTICS);
	}

	if (param == PRM_PLAN || param == PRM_EVERYTHING)			
	{
		this->plan.Unload();
		this->piList.erase(PRM_PLAN);
		unloaded.insert(PRM_PLAN);
	}	

	if (param == PRM_IDEAL_PLAN || param == PRM_EVERYTHING)		
	{
		this->ideal_plan.Unload();
		this->piList.erase(PRM_IDEAL_PLAN);
		unloaded.insert(PRM_IDEAL_PLAN);
	}

	if (param == PRM_PLAN_ELEM_LIST || param == PRM_EVERYTHING)	
	{
		this->planElem.Unload();
		this->piList.erase(PRM_PLAN_ELEM_LIST);
		unloaded.insert(PRM_PLAN_ELEM_LIST);
	}

	if (param == PRM_MODEL_PLAN_ELEM_LIST || param == PRM_EVERYTHING)
	{
		this->modelPlanElem.Unload();
		this->piList.erase(PRM_MODEL_PLAN_ELEM_LIST);
		unloaded.insert(PRM_MODEL_PLAN_ELEM_LIST);
	}

	if (param == PRM_HISTORY || param == PRM_EVERYTHING)		
	{
		this->history.Unload();
		this->piList.erase(PRM_HISTORY);
		unloaded.insert(PRM_HISTORY);
	}

	if (param == PRM_COMMENT || param == PRM_EVERYTHING)
	{
		this->comment.Unload();
		this->piList.erase(PRM_COMMENT);
		unloaded.insert(PRM_COMMENT);
	}
	
	if (param == PRM_WAY_MARKERS || param == PRM_EVERYTHING)
	{
		this->markers.Unload();
		this->piList.erase(PRM_WAY_MARKERS);
		unloaded.insert(PRM_WAY_MARKERS);
	}
	
	return unloaded;
}

// Границы поездки
VWaySection VDataSet::GetSection()
{
	if (!this->Load(PRM_ABS_COORD))
	{
		return VWaySection(0, 0, 0, 0);
	}

	VAbsCoordData * absCoord = &this->GetAbsCoord();

	double absStart = absCoord->GetItem(0);
	double absEnd = absCoord->GetLastItem();

	VWaySection section;
	section.dwSize = sizeof(VWaySection);

	// заведомо бредовая координата
	if (_isnan(absStart) || absStart < -500000.0 || absStart > 30000000.0 ||
		_isnan(absEnd) || absEnd < -500000.0 || absEnd > 30000000.0)
	{
		section.start.km = 0;
		section.start.m = 0;
		section.end = section.start;
	}
	else
	{
		if (this->GetAprinfo().IsLoaded())
		{
			section.start = this->GetAprinfo().GetRealCoord(absCoord->GetItem(0));
			section.end = this->GetAprinfo().GetRealCoord(absCoord->GetLastItem());
		}
		else if (this->GetFileAprinfo().IsLoaded())
		{
			section.start = this->GetFileAprinfo().GetRealCoord(absCoord->GetItem(0));
			section.end = this->GetFileAprinfo().GetRealCoord(absCoord->GetLastItem());
		}
		else
		{
			section.start.km = absStart / 1000;
			section.start.m = absStart - 1000 * section.start.km;

			section.end.km = absEnd / 1000;
			section.end.m = absEnd - 1000 * section.end.km;
		}
	}
	return section;
}

// Количество кадров в поездке
size_t VDataSet::GetKadrCount()
{
	this->Load(PRM_ABS_COORD);
	return this->GetAbsCoord().GetCount();
}

// Загрузка апр.информации из базы
bool VDataSet::Makeinfo(VIBDatabasePtr base, bool repairs, bool mea_curves)
{
	return this->aprInfo.Makeinfo(*base, 
		this->GetDir(), this->GetWay(), this->GetSection(), this->GetRailDate(),
		repairs, mea_curves);
}

// Проверка загруженности параметра
bool VDataSet::IsLoaded(PARAMETER_TYPE param) const
{
	return this->piList.find(param) != this->piList.end();
}

// Параметр по индексу
VParameter & VDataSet::GetParameter(PARAMETER_TYPE param)
{
	if (VDataSet::IsFileParameter(param))
	{
		return *(this->params[param]);
	}

	else if (VDataSet::IsCalcParameter(param))
	{
		switch(param)
		{
		case PRM_SPEED:				return this->speed; 			// скорость вагона
		case PRM_MID_WIDTH:			return this->midWidth; 			// ср. шк
		case PRM_MID_RICH_LEFT:		return this->midRichLeft; 		// ср. рихт. лев.
		case PRM_MID_RICH_RIGHT:	return this->midRichRight;		// ср. рихт. лев.
		case PRM_MID_LEVEL:			return this->midLevel; 			// ср. возвышения
		case PRM_MID_CURV:			return this->midCurv; 			// ср. кривизны
		case PRM_UNI_WEAR:			return this->wear; 				// объединенный износ
		case PRM_PROFILE:			return this->profile; 			// профиль
		case PRM_DIFF_PROFILE:		return this->diff_profile;		// отклонения от идеального профиля
		case PRM_APROX_PROFILE:		return this->aprox_profile;		// спрямленный профиль
		case PRM_JAGGY_PROFILE:		return this->jaggy_profile;		// неровности профиля
		case PRM_JAGGY_PLAN:		return this->jaggy_plan;		// неровности плана
		case PRM_JAGGY_PLAN_ON_LINES:	return this->jaggy_plan_on_lines; // неровности плана на прямых
		case PRM_PLAN_OFFSET:		return this->plan_offset;		// отклонения от идеального плана
		case PRM_PLAN_OFFSET_FICTIONAL:		return this->plan_offset_fictional;		// отклонения от идеального плана
		case PRM_MODEL_PLAN_OFFSET:	return this->model_plan_offset;	// отклонения от идеального плана
		case PRM_ANP_REAL_HIGH:  	return this->anp_real_high;		// непогашенное ускорение фактическое для скоростных поездов
		case PRM_ANP_REAL_HIGH_BT: 	return this->anp_real_high_bt;	// непогашенное ускорение фактическое для скоростных поездов с наклоном кузова
		case PRM_ANP_REAL_PASS:  	return this->anp_real_pass;		// непогашенное ускорение фактическое для пассажирских поездов
		case PRM_ANP_REAL_TRUCK:  	return this->anp_real_truck;	// непогашенное ускорение фактическое для грузовых поездов
		case PRM_PSI_REAL_HIGH:  	return this->psi_real_high;		// скорость нарастания непогашенного ускорения (факт. скор.)
		case PRM_PSI_REAL_HIGH_BT:  return this->psi_real_high_bt;	// скорость нарастания непогашенного ускорения (факт. скор. с НК)
		case PRM_PSI_REAL_PASS:  	return this->psi_real_pass;		// скорость нарастания непогашенного ускорения (факт. пасс.)
		case PRM_PSI_REAL_TRUCK:  	return this->psi_real_truck;	// скорость нарастания непогашенного ускорения (факт. груз.)
		case PRM_LEVEL_ZLINE:		return this->zl; 				// нулевая линия по уровню
		case PRM_CURV_FROM_KURS:	return this->curv_from_kurs;	// кривизна, рассчитанная из курса
		case PRM_DIFF_LEVEL:		return this->diff_level; 		// отклонения уровеня от разметки
		case PRM_DIFF_LEVEL_MODEL:	return this->diff_level_model;	// отклонения уровеня от разметки модели
		case PRM_DIFF_WIDTH_FRONT:	return this->width_diff_front;	// разность ШК с нагрузкой и без
		case PRM_DIFF_WIDTH_BACK:	return this->width_diff_back;	// разность ШК с нагрузкой и без
		default: break;
		}
	}
	
	std::wstringstream msg;
	msg << L"Параметр " << GetParamName(param) << L" не может быть возвращен как VParameter";
	throw VSimpleException(msg.str(), L"");
}

VDoubleParameter & VDataSet::GetDoubleParameter(PARAMETER_TYPE param)
{
	switch(param)
	{
	case PRM_GPS_TIME:		return this->gps_time;		// GPS время
	case PRM_GPS_LATITUDE:	return this->gps_latitude;	// GPS широта
	case PRM_GPS_LONGITUDE:	return this->gps_longitude;	// GPS долгота
	case PRM_GPS_ALTITUDE:	return this->gps_height;	// GPS высота	
	default: break;
	}

	std::wstringstream msg;
	msg << L"Параметр " << GetParamName(param) << L" не может быть возвращен как VDoubleParameter";
	throw VSimpleException(msg.str(), L"");
}

// Зазоры - все в куче
VZazorList & VDataSet::GetZazor(PARAMETER_TYPE param)
{
	switch(param)
	{
	case PRM_ZAZOR_LEFT:		return this->zazorLeft;
	case PRM_ZAZOR_RIGHT:		return this->zazorRight;
	case PRM_MAG_ZAZOR_LEFT:	return this->magZazorLeft;
	case PRM_MAG_ZAZOR_RIGHT:	return this->magZazorRight;
	default:					break;
	}

	std::wstringstream msg;
	msg << L"Параметр " << GetParamName(param) << L" не может быть возвращен как VZazorList";
	throw VSimpleException(msg.str(), L"");
}

/// Параметр из строки с разбором
void VDataSet::ParseParameter(wstring in_command, VParameter & parameter)
{
	VFunctionParser parser(this);
	parser.Parse(in_command,parameter);
}

bool VDataSet::Save(const std::wstring & fileName)
{
	// имя файла
	std::wstring fn;

	// проверяем состояние - может быть, всё и так ок
	if (open_mode == OPEN_MODE_WRITE || open_mode == OPEN_MODE_CREATE) 
	{	
		fn = this->file.GetFileName();
	}
	else if (open_mode == OPEN_MODE_READ)
	{
		fn = this->file.GetFileName();

		// закрываем файл
		this->CloseFile();

		// открываем файл на запись
		bool reopen_result = true;
		try
		{
			this->file.Open(fn.c_str(), OPEN_MODE_WRITE);
		}
		catch(...)
		{
			reopen_result = false;
		}
	
		if (!this->file.Loaded()) 
		{
			reopen_result = false;
		}

		if (!reopen_result)
		{
			this->file.Open(fn.c_str(), OPEN_MODE_READ);

			this->message = L"Не удалось открыть файл для записи ";
			this->message += L"(";
			this->message += fn;
			this->message += L"";

			return false;
		}
	}
	else /// no file
	{
		fn = std::wstring(fileName);

		// открываем файл на создание
		bool open_result = true;
		try
		{
			this->file.Open(fn.c_str(), OPEN_MODE_CREATE);
		}
		catch(...)
		{
			open_result = false;
		}

		if (!this->file.Loaded()) 
		{
			open_result = false;
		}

		if (!open_result)
		{
			this->message = L"Не удалось создать файл";
			this->message += L"(";
			this->message += fn;
			this->message += L"";

			return false;
		}
	}

	// абс.координата
	if (!this->absCoord.IsLoaded()) 
	{
		this->absCoord.Load(this->file, DHT_ABS_COORD);
	}

	// заголовок файла
	this->header.WaySection = this->GetSection();
	this->header.ParamCount = this->GetAbsCoord().GetCount();
	this->header.UpdateTime = VTime(time(0));
	*this->file.GetUserHeader() = this->header;

	// апринфо
	if (this->IsChanged(PRM_APRINFO))
	{
		this->fileAprinfo = this->aprInfo;
	}
	else 
	{
		this->fileAprinfo.Load(this->file, DHT_APRINFO);
	}

	bool result = true;

	if (this->IsChanged(PRM_ABS_COORD))
	{
		if (this->absCoord.Save(this->file, DHT_ABS_COORD))
			this->UnsetChanged(PRM_ABS_COORD);
		else
		{
			result = false;
		}
	}

	if (this->IsChanged(PRM_CORRUPT))
	{
		if (this->corrupt.Save(this->file))
			this->UnsetChanged(PRM_CORRUPT);
		else
			result = false;
	}

	if (this->IsChanged(PRM_SPEED))
	{
		if (this->speed.Save(this->file, DHT_SPEED))
			this->UnsetChanged(PRM_SPEED);
		else
			result = false;
	}

	if (this->IsChanged(PRM_DPP))
	{
		if (this->dpp.Save(this->file, DHT_DPP))
			this->UnsetChanged(PRM_DPP);
		else
			result = false;
	}

	if (this->IsChanged(PRM_APRINFO))
	{
		if (this->aprInfo.Save(this->file, DHT_APRINFO))
			this->UnsetChanged(PRM_APRINFO);
		else
			result = false;
	}

	if (this->IsChanged(PRM_OVRS2))
	{
		if (this->ovrs2.Save(this->file, DHT_OVRS2))
			this->UnsetChanged(PRM_OVRS2);
		else
			result = false;
	}

	if (this->IsChanged(PRM_OVRS3))
	{
		if (this->ovrs3.Save(this->file, DHT_OVRS3))
			this->UnsetChanged(PRM_OVRS3);
		else
			result = false;
	}

	if (this->IsChanged(PRM_OP_MARKS))
	{
		if (this->opMarks.Save(this->file, DHT_OP_MARKS))
			this->UnsetChanged(PRM_OP_MARKS);
		else
			result = false;
	}

	if (this->IsChanged(PRM_MAG_ZAZOR))
	{
		if (this->magZazor.Save(this->file, DHT_MAG_ZAZOR))
			this->UnsetChanged(PRM_MAG_ZAZOR);
		else
			result = false;
	}

	if (this->IsChanged(PRM_UGON_LEFT))
	{
		if (this->ugonLeft.Save(this->file, DHT_UGON_LEFT))
			this->UnsetChanged(PRM_UGON_LEFT);
		else
			result = false;
	}

	if (this->IsChanged(PRM_UGON_RIGHT))
	{
		if (this->ugonRight.Save(this->file, DHT_UGON_RIGHT))
			this->UnsetChanged(PRM_UGON_RIGHT);
		else
			result = false;
	}

	if (this->IsChanged(PRM_SPALA_MARKER))
	{
		if (this->spalaMarker.Save(this->file, DHT_SPALA_MARKER))
			this->UnsetChanged(PRM_SPALA_MARKER);
		else
			result = false;
	}

	// ------------------------------------------------------------------------

	for (int i = 0; file_parameters[i] != PRM_NONE; ++i)
	{
		if (this->IsChanged(file_parameters[i]))
		{
			if (this->GetParameter(file_parameters[i]).Save(this->file, GetFileCodeByParam(file_parameters[i])))
				this->UnsetChanged(file_parameters[i]);
			else
				result = false;
		}
	}

	// ------------------------------------------------------------------------

	for (int i = 0; double_parameters[i] != PRM_NONE; ++i)
	{
		if (this->IsChanged(double_parameters[i]))
		{
			if (this->GetDoubleParameter(double_parameters[i]).Save(this->file, GetFileCodeByParam(double_parameters[i])))
				this->UnsetChanged(double_parameters[i]);
			else
				result = false;
		}
	}

	// ------------------------------------------------------------------------

	if (this->IsChanged(PRM_ZAZOR_LEFT))
	{
		if (this->zazorLeft.Save(this->file, DHT_ZAZOR_LEFT))
			this->UnsetChanged(PRM_ZAZOR_LEFT);
		else
			result = false;
	}

	if (this->IsChanged(PRM_ZAZOR_RIGHT))
	{
		if (this->zazorRight.Save(this->file, DHT_ZAZOR_RIGHT))
			this->UnsetChanged(PRM_ZAZOR_RIGHT);
		else
			result = false;
	}

	if (this->IsChanged(PRM_MAG_ZAZOR_LEFT))
	{
		if (this->magZazorLeft.Save(this->file, DHT_MAG_ZAZOR_LEFT))
			this->UnsetChanged(PRM_MAG_ZAZOR_LEFT);
		else
			result = false;
	}

	if (this->IsChanged(PRM_MAG_ZAZOR_RIGHT))
	{
		if (this->magZazorRight.Save(this->file, DHT_MAG_ZAZOR_RIGHT))
			this->UnsetChanged(PRM_MAG_ZAZOR_RIGHT);
		else
			result = false;
	}

	if (this->IsChanged(PRM_CURVE_MARKS))
	{
		if (this->curves.Save(this->file, DHT_CURVE_MARKS, this->absCoord))
			this->UnsetChanged(PRM_CURVE_MARKS);
		else
			result = false;
	}

	if (this->IsChanged(PRM_MODEL_CURVES))
	{
		if (this->modelCurves.Save(this->file, DHT_MODEL_CURVES, this->absCoord))
			this->UnsetChanged(PRM_MODEL_CURVES);
		else
			result = false;
	}

	if (this->IsChanged(PRM_PROF_MARKS))
	{
		if (this->profMarks.Save(this->file, DHT_PROF_MARKS, this->absCoord, this->fileAprinfo))
			this->UnsetChanged(PRM_PROF_MARKS);
		else
			result = false;
	}

	if (this->IsChanged(PRM_SIDEWAYS))
	{
		if (this->sideways.Save(this->file, DHT_SIDEWAYS))
			this->UnsetChanged(PRM_SIDEWAYS);
		else
			result = false;
	}

	if (this->IsChanged(PRM_CONTROL_AREA))
	{
		if (this->ctrlAreas.Save(this->file, DHT_CONTROL_AREA))
			this->UnsetChanged(PRM_CONTROL_AREA);
		else
			result = false;
	}

	if (this->IsChanged(PRM_LINES))
	{
		// сохранения прямых нет[у]
		this->UnsetChanged(PRM_LINES);
	}

	if (this->IsChanged(PRM_LINE_MARKS))
	{
		if (this->lineMarks.Save(this->file, DHT_LINE_MARKS, this->absCoord, this->aprInfo))
			this->UnsetChanged(PRM_LINE_MARKS);
		else
			result = false;
	}

	if (this->IsChanged(PRM_HISTORY))
	{
 		if (this->history.Save(this->file))
			this->UnsetChanged(PRM_HISTORY);
		else
			result = false;
	}

	if (this->IsChanged(PRM_COMMENT))
	{
		if (this->comment.Save(this->file))
			this->UnsetChanged(PRM_COMMENT);
		else
			result = false;
	}

	this->file.Close();
	this->file.Open(fn.c_str(), OPEN_MODE_READ);

	if (result)
	{
		this->message = ERRMSG_OK;
	}
	else
	{
		this->message = L"Следующие параметры не были записаны:";
		std::set<PARAMETER_TYPE>::iterator it;
		for (it = this->changed.begin(); it != this->changed.end(); ++it)
		{
			this->message += L"\n";
			this->message += GetUserParamName(*it);
		}
	}

	return result;
}

// Список доступных для записи в CSV параметров
std::set<PARAMETER_TYPE> VDataSet::GetCSVParameters()
{
	std::set<PARAMETER_TYPE> prms;
	if (!this->DataExists(PRM_ABS_COORD) || !this->DataExists(PRM_APRINFO))
	{
		return prms;
	}

	// д.б. первой
	if (this->DataExists(PRM_SPEED)) prms.insert(PRM_SPEED); 

	// параметры в файле
	for (int i = 0; file_parameters[i] != PRM_NONE; ++i)
	{
		if (this->DataExists(file_parameters[i]))
			prms.insert(file_parameters[i]);
	}

	// расчетные параметры
	for (int i = 0; calc_parameters[i] != PRM_NONE; ++i)
	{
		prms.insert(calc_parameters[i]);
	}

	// параметры как double
	for (int i = 0; double_parameters[i] != PRM_NONE; ++i)
	{
		if (this->DataExists(double_parameters[i]))
			prms.insert(double_parameters[i]);
	}

	// счетчик тиков ДПП
	if (this->DataExists(PRM_DPP))
		prms.insert(PRM_DPP);

	// параметры привязанные к кадру
	if (this->DataExists(PRM_ZAZOR_LEFT)) prms.insert(PRM_ZAZOR_LEFT);
	if (this->DataExists(PRM_ZAZOR_RIGHT)) prms.insert(PRM_ZAZOR_RIGHT);
	if (this->DataExists(PRM_MAG_ZAZOR_LEFT)) prms.insert(PRM_MAG_ZAZOR_LEFT);
	if (this->DataExists(PRM_MAG_ZAZOR_RIGHT)) prms.insert(PRM_MAG_ZAZOR_RIGHT);
	if (this->DataExists(PRM_MAG_ZAZOR)) prms.insert(PRM_MAG_ZAZOR);
	if (this->DataExists(PRM_UGON_LEFT)) prms.insert(PRM_UGON_LEFT);
	if (this->DataExists(PRM_UGON_RIGHT)) prms.insert(PRM_UGON_RIGHT);
	if (this->DataExists(PRM_SPALA_MARKER)) prms.insert(PRM_SPALA_MARKER);

	// план
	if (this->DataExists(PRM_KURS) && 
		this->DataExists(PRM_LATITUDE) && 
		this->DataExists(PRM_LONGITUDE))
	{
		prms.insert(PRM_PLAN);
	}	

	return prms;
}

// Список доступных для коррекции параметров
std::set<PARAMETER_TYPE> VDataSet::GetCorrectableParameters()
{
	std::set<PARAMETER_TYPE> prms;

	for (int i = 0; file_parameters[i] != PRM_NONE; ++i)
	{
		if (this->DataExists(file_parameters[i]))
			prms.insert(file_parameters[i]);
	}

	for (int i = 0; double_parameters[i] != PRM_NONE; ++i)
	{
		if (this->DataExists(double_parameters[i]))
			prms.insert(double_parameters[i]);
	}

	if (this->DataExists(PRM_SPALA_MARKER)) prms.insert(PRM_SPALA_MARKER);

	return prms;
}

// Записать в поток таблицу CSV
size_t VDataSet::GetCSV(ostream & output, 
						const std::set<PARAMETER_TYPE> exported_prms,
						size_t step_kadr, float step_m, 
						const VWaySection & section,
						SIDEWAYS_PROCESSING swp,
						VProgressBox * progress)
{
	// возвращаем число строк (исключая заголовок)
	size_t lines = 0;

	std::wstringstream msg;
	msg << L"Запись измерений файла " << this->GetFileName().GetShortName() << L" в CSV";
	LOG_INFO(msg.str());

	if (progress) progress->SetMessage(0, L"Загрузка параметров");

	// разделитель
//	const std::string divider = " " + wstring_to_string(getCSVSep()) + " ";
	const std::string divider = wstring_to_string(getCSVSep());

	// список загруженных параметров
	std::set<PARAMETER_TYPE> loaded;

	// подгружаем параметры
	this->Load(PRM_CORRUPT);
	this->Load(PRM_ABS_COORD);
	this->Load(PRM_OP_MARKS);
	this->Load(PRM_DPP);
	if (!this->GetAprinfo().IsLoaded()) this->Load(PRM_APRINFO);

	if (progress != NULL)
	{
		progress->SetRange(0, exported_prms.size());
	}

	for (std::set<PARAMETER_TYPE>::const_iterator it = exported_prms.begin(); 
		it != exported_prms.end(); ++it)
	{
		if (progress != NULL)
		{
			std::stringstream msg;
			msg << "Загрузка параметров: " << GetUserParamName(*it);
			progress->SetMessage(0, msg.str().c_str());
		}
					
		if (!this->Load(*it))
		{
			LOG_WARNING(GetUserParamName(*it) + std::wstring(L": ") + this->GetErrorMessage());
		}

		if (progress != NULL)
		{
			progress->Step(0);
		}
	} 

	VSidewayList * sw = NULL;
	if (swp != SIDEWAYS_KEEP)
	{
		this->Load(PRM_SIDEWAYS);

		if (this->GetSideways().IsLoaded())
			sw = &this->GetSideways();
	}

	// абс.координата
	if (this->GetAbsCoord().IsLoaded() && this->GetAprinfo().IsLoaded())
	{
		output << "Номер кадра" << divider;
		if (/*exported_prms.find(PRM_DPP) != exported_prms.end() && */
				this->GetDPP().IsLoaded())
		{
			output << GetUserParamName(PRM_DPP) << divider;
			loaded.insert(PRM_DPP);
		}
		output << GetUserParamName(PRM_ABS_COORD);
		output << divider << "км" << divider << "м";
	}
	else
	{
		return 0;
	}

	// метки оператора
	if (this->GetOpMarks().IsLoaded())
	{
		output << divider << GetUserParamName(PRM_OP_MARKS);
		loaded.insert(PRM_OP_MARKS);
	}

	// угон левый
	if (this->GetUgonLeft().IsLoaded())
	{
		output << divider << GetUserParamName(PRM_UGON_LEFT);
		loaded.insert(PRM_UGON_LEFT);
	}

	// угон правый
	if (this->GetUgonRight().IsLoaded())
	{
		output << divider << GetUserParamName(PRM_UGON_RIGHT);
		loaded.insert(PRM_UGON_RIGHT);
	}

	// прочие параметры
	for (std::set<PARAMETER_TYPE>::const_iterator it = exported_prms.begin(); 
		it != exported_prms.end(); ++it)
	{
		if ((this->IsParameter(*it) || this->IsDoubleParameter(*it)) && 
			this->IsLoaded(*it))
		{
			output << divider << GetUserParamName(*it);
			loaded.insert(*it);
		}
	} 

	// план 
	if (exported_prms.find(PRM_PLAN) != exported_prms.end() && 
		this->GetPlan().IsLoaded())
	{
		output << divider << GetUserParamName(PRM_PLAN) << " (X)";
		output << divider << GetUserParamName(PRM_PLAN) << " (Y)";
		loaded.insert(PRM_PLAN);
	}	

	// счетчик тиков ДПП
/*	if (exported_prms.find(PRM_DPP) != exported_prms.end() && 
		this->GetDPP().IsLoaded())
	{
		output << divider << GetUserParamName(PRM_DPP);
		loaded.insert(PRM_DPP);
	}*/

	// зазоры
	for (size_t z_index = 0; zazor_parameters[z_index] != PRM_NONE; ++z_index)
	{
		if (exported_prms.find(zazor_parameters[z_index]) != exported_prms.end() &&
			this->GetZazor(zazor_parameters[z_index]).IsLoaded())
		{
			output << divider << GetUserParamName(zazor_parameters[z_index]);
			loaded.insert(zazor_parameters[z_index]);
		}
	}

	// метки шпал
	if (exported_prms.find(PRM_SPALA_MARKER) != exported_prms.end() && 
		this->GetSpalaMarker().IsLoaded())
	{
		output << divider << GetUserParamName(PRM_SPALA_MARKER);
		loaded.insert(PRM_SPALA_MARKER);
	}

	output << std::endl;

	// временный буфер
	std::stringstream buffer;
	buffer << std::fixed << std::setprecision(6);
	buffer.imbue(std::locale(""));

	// запись по строке
	size_t kadrCount = this->GetKadrCount();
	size_t last_kadr = BAD_INDEX;
	
	size_t kadrFrom = this->GetAbsCoord().FindIndex(
		this->GetAprinfo().GetAbsCoord(section.start), true);

	size_t kadrTo = this->GetAbsCoord().FindIndex(
		this->GetAprinfo().GetAbsCoord(section.end), true);

	if (progress != NULL)
		progress->SetRange(0, kadrTo - kadrFrom + 1);

	time_t t0 = time(NULL);

	for (size_t kadrNdx = kadrFrom; kadrNdx <= kadrTo; ++kadrNdx)
	{
		double absCoord = this->GetAbsCoord().GetItem(kadrNdx);
		VWayCoord wayCoord = this->GetAprinfo().GetRealCoord(absCoord);
	
		// обрабатываем боковые пути
		bool isSideway = false;
		if (sw)
		{
			size_t found = sw->FindItem(kadrNdx);
			if (found != BAD_INDEX)
			{
				isSideway = true;
			}
		}

		if (isSideway && swp == SIDEWAYS_REMOVE)
		{
			continue;
		}

		// прореживание?
		if (last_kadr != BAD_INDEX && (
			(step_kadr > 0 && kadrNdx - last_kadr < step_kadr) ||
			(step_m > 0.0f && this->absCoord.GetItem(kadrNdx) - this->absCoord.GetItem(last_kadr) < step_m)))
		{
			;
		}
		else
		{
			// номер кадра и координата
			buffer << kadrNdx+1;
			if (/*loaded.find(PRM_DPP) != loaded.end()*/ this->GetDPP().IsLoaded())
			{
				buffer << divider << this->GetDPP().GetItem(kadrNdx);
			}			
			buffer << divider << absCoord;
			buffer << divider << wayCoord.km << divider << wayCoord.m; 

			// метки оператора
			if (loaded.find(PRM_OP_MARKS) != loaded.end())
			{
				buffer << divider;

				if (!isSideway)
				{
					size_t found = this->GetOpMarks().FindItem(kadrNdx);
					if (found != BAD_INDEX) 
					{
						buffer << this->GetOpMarks().GetItem(found).ShortText();
					}
				}
			}

			// угон левый
			if (loaded.find(PRM_UGON_LEFT) != loaded.end())
			{
				buffer << divider;

				if (!isSideway)
				{
					size_t found = this->GetUgonLeft().FindItem(kadrNdx);
					if (found != BAD_INDEX)
					{
						buffer << this->GetUgonLeft().GetItem(found).value;
					}
				}
			}

			// угон правый
			if (loaded.find(PRM_UGON_RIGHT) != loaded.end())
			{
				buffer << divider;

				if (!isSideway)
				{
					size_t found = this->GetUgonRight().FindItem(kadrNdx);
					if (found != BAD_INDEX)
					{
						buffer << this->GetUgonRight().GetItem(found).value;
					}
				}
			}

			// прочие параметры
			for (std::set<PARAMETER_TYPE>::const_iterator it = loaded.begin(); it != loaded.end(); ++it)
			{
				if (isSideway)
				{
					buffer << divider << 0.0f;
				}

				else if (this->IsDoubleParameter(*it))
				{
					buffer << divider << this->GetDoubleParameter(*it).GetItem(kadrNdx);
				}

				else if (this->IsParameter(*it))
				{
					buffer << divider << this->GetParameter(*it).GetItem(kadrNdx);
				}
			} 

			// план
			if (loaded.find(PRM_PLAN) != loaded.end())
			{
				const point_t & pl = this->GetPlan().GetItem(kadrNdx);
				buffer << divider << pl.x << divider << pl.y;
			}	

			// счетчик ДПП
			/*if (loaded.find(PRM_DPP) != loaded.end())
			{
				buffer << divider << this->GetDPP().GetItem(kadrNdx);
			}*/

			// зазоры
			for (size_t z_index = 0; zazor_parameters[z_index] != PRM_NONE; ++z_index)
			{
				PARAMETER_TYPE cur_zazor_type = zazor_parameters[z_index];
				const VZazorList & zazor_list = this->GetZazor(cur_zazor_type);

				if (loaded.find(cur_zazor_type) != loaded.end())
				{
					buffer << divider;

					if (!isSideway)
					{
						size_t found = zazor_list.FindItem(kadrNdx);
						if (found != BAD_INDEX) 
						{
							buffer << zazor_list.GetItem(found).value;
						}
					}
				}
			}

			// метки шпал
			if (loaded.find(PRM_SPALA_MARKER) != loaded.end())
			{
				buffer << divider;

				if (!isSideway)
				{
					size_t found = this->GetSpalaMarker().FindItem(kadrNdx);
					if (found != BAD_INDEX) 
					{
						buffer << this->GetSpalaMarker().GetItem(found).value;
					}
				}
			}

			// voila!
			buffer << std::endl;

			if ((lines % 1000) == 0)
			{
				output << buffer.str();
				buffer.str("");
			}

			last_kadr = kadrNdx;
			++lines;
		}

		if ((kadrNdx % 1000) == 0 && progress != NULL)
		{
			std::stringstream msg;
			msg << std::fixed << std::setprecision(1);
			msg << "Экспорт параметров в таблицу [" << 100.0 * (kadrNdx - kadrFrom) / (kadrTo - kadrFrom + 1) << "%]";
			progress->SetMessage(0, msg.str().c_str());
			progress->SetPosition(0, kadrNdx - kadrFrom);
		}
	}

	output << buffer.str();

	time_t t_res = time(NULL) - t0;
	msg.str(L"");
	msg << L"Запись измерений в файл CSV заняла " << t_res << L" сек";
	LOG_INFO(msg.str());

	return lines;
}

/// Записать в поток метки оператора
size_t VDataSet::GetOpMarkCSV(std::ostream & output)
{
	size_t total = 0;
	const std::string divider = " " + wstring_to_string(getCSVSep()) + " ";

	if (this->Load(PRM_OP_MARKS) && this->Load(PRM_ABS_COORD))
	{		
		for (size_t i=0; i<this->GetOpMarks().GetCount(); ++i)
		{
			output << this->GetAbsCoord().GetItem(this->GetOpMarks().GetItem(i).index) << divider << this->GetOpMarks().GetItem(i).ShortText() << endl; 
		}
		total=this->GetOpMarks().GetCount();		
	}
	return total;
}

// Сохранить разметку как таблицу
size_t VDataSet::GetCurveMarkCSV(std::ostream & output, bool model)
{
	size_t total = 0;
	const std::string divider = " " + wstring_to_string(getCSVSep()) + " ";

	if (this->Load(model ? PRM_MODEL_CURVES : PRM_CURVE_MARKS)  && this->Load(PRM_ABS_COORD) && this->Load(PRM_APRINFO))
	{		
		output <<divider<<divider<<"кривизна"<<divider<<divider<<divider<<divider<<"уровень"<<divider<<divider<<divider<< std::endl;
		output <<"№ кривой"<<divider<<"№ метки"<<divider<<"абс.к-та"<<divider<<"км"<<divider<<"м"<<divider<<"значение"<<divider<<"абс.к-та"<<divider<<"км"<<divider<<"м"<<divider<<"значение" << std::endl << std::endl;

		// настраиваем поток
		output << std::fixed << std::setprecision(6);
		output.imbue(std::locale(""));

		VCurveList & curves = model ? this->GetCurves(PRM_MODEL_CURVES) : this->GetCurves(PRM_CURVE_MARKS);
		VAbsCoordData & absCoord = this->GetAbsCoord();
		VAprinfo & aprInfo = this->GetAprinfo();
		
		for (size_t crvNdx = 0; crvNdx < curves.GetCount(); ++crvNdx)
		{
			output << crvNdx+1;
			VCurve & crv = curves.GetItem(crvNdx);

			for (size_t markNdx = 0; markNdx < crv.GetMarkCount(); ++markNdx)
			{
				output << divider << markNdx+1;

				VCurveMark & mark = crv.GetPlanMark(markNdx);
				double abs = absCoord.GetItem(mark.index);
				VWayCoord way = aprInfo.GetRealCoord(abs);
				output << divider << abs << divider << way.km << divider << way.m << divider << mark.value;

				mark = crv.GetLevelMark(markNdx);
				abs = absCoord.GetItem(mark.index);
				way = aprInfo.GetRealCoord(abs);
				output << divider << abs << divider << way.km << divider << way.m << divider << mark.value << std::endl;
				++total;
			}
			output << std::endl;
		}
	}

	return total;
}

// Записать в поток метки профиля 
size_t VDataSet::GetProfMarkCSV(std::ostream & output)
{
	size_t total = 0;
	const std::string divider = " " + wstring_to_string(getCSVSep()) + " ";

	if (this->Load(PRM_PROF_MARKS)  && this->Load(PRM_ABS_COORD) && this->Load(PRM_APRINFO))
	{		
		output << "№ метки"<<divider<<"абс.к-та"<<divider<<"км"<<divider<<"м"<<divider<<"величина"<<std::endl;

		// настраиваем поток
		output << std::fixed << std::setprecision(6);
		output.imbue(std::locale(""));

		VProfileMarkList & profMarks = this->GetProfMarks();
		VAbsCoordData & absCoord = this->GetAbsCoord();
		VAprinfo & aprInfo = this->GetAprinfo();

		for (size_t markNdx = 0; markNdx < profMarks.GetCount(); ++markNdx)
		{
			VProfileMark & mark = profMarks.GetItem(markNdx);
			double abs = absCoord.GetItem(mark.index);
			VWayCoord way = aprInfo.GetRealCoord(abs);

			output << markNdx + 1 << divider << abs << divider << way.km << divider << way.m;
			output << divider << mark.value << std::endl;
			++total;
		}
	}

	return total;
}

/// Записать в поток мат. ожидание и СКО
size_t VDataSet::GetMoSkoCSV(std::ostream & output, int filter)
{
	size_t total = 0;
	const std::string divider = " " + wstring_to_string(getCSVSep()) + " ";
			
	// настраиваем поток
	double moWidth, skoWidth;
	double moLevel, skoLevel;
	double moPitLeft, skoPitLeft;
	double moPitRight, skoPitRight;
	double moCurv, skoCurv;
	double moDistShort, skoDistShort;
	double moDistLong, skoDistLong;
	double moWearLeft, skoWearLeft;
	double moWearRight, skoWearRight;
	double moUskVert, skoUskVert;
	double moUskHor, skoUskHor;

	if (!this->Load(PRM_ABS_COORD) || !this->Load(PRM_APRINFO))
	{
		return 0;
	}
	VAprinfo aprinfo=this->GetAprinfo();


	// Расчёт параметров
	if (this->Load(PRM_WIDTH))
	{
		VParameter & width=this->GetParameter(PRM_WIDTH);
	
		double sum=0, sumkv=0;
		for (size_t i=0;i<width.GetCount();i++)
		{
			double abs_coord=this->GetAbsCoord().GetItem(i);
			sum+=(width.GetItem(i)-aprinfo.GetPassportCurveNormaWidth(abs_coord));
			sumkv+=pow(width.GetItem(i)-aprinfo.GetPassportCurveNormaWidth(abs_coord),2);
		}
		moWidth=sum/width.GetCount();
		skoWidth=sqrt((sumkv-pow(sum,2)/width.GetCount())/(width.GetCount()-1));
	}
	if (this->Load(PRM_LEVEL))
	{
		this->Load(PRM_CURVE_MARKS);
		VParameter & level=this->GetParameter(PRM_LEVEL);
		VAprinfo aprinfo=this->GetAprinfo();
		VCurveList & curveList=this->GetCurves();
		size_t curve_num=0;
		double sum=0, sumkv=0;
		for (size_t i=0;i<level.GetCount();i++)
		{
			double abs_coord=this->GetAbsCoord().GetItem(i);
			if (curve_num>=curveList.GetCount())
			{
				sum+=level.GetItem(i)-aprinfo.GetStraightNormaLevel(abs_coord);
				sumkv+=pow((double)(level.GetItem(i)-aprinfo.GetStraightNormaLevel(abs_coord)),2);
			}
			else
			{
				VAbsCoordData & absCoord = this->GetAbsCoord();
				VCurve & curve=curveList.GetItem(curve_num);
				VCurveMark& mark1=curve.GetLevelMark(0);
				VCurveMark& mark2=curve.GetLevelMark(curveList.GetItem(curve_num).GetLevelMarkCount()-1);
				if (abs_coord<absCoord.GetItem(mark1.index))
				{
					sum+=aprinfo.GetStraightNormaLevel(abs_coord);
					sumkv+=pow((double)aprinfo.GetStraightNormaLevel(abs_coord),2);
				}
				else if (absCoord.GetItem(mark2.index)<abs_coord)
				{
					sum+=aprinfo.GetStraightNormaLevel(abs_coord);
					sumkv+=pow((double)aprinfo.GetStraightNormaLevel(abs_coord),2);
					curve_num++;
				}
				else
				{
					double value=0;
					for (size_t j=0; j<curve.GetLevelMarkCount();j++)
						if (absCoord.GetItem(curve.GetLevelMark(j).index)==abs_coord)
						{
							value=curve.GetLevelMark(j).value;
							break;
						}
						else if (absCoord.GetItem(curve.GetLevelMark(j).index)>abs_coord)
						{
							VCurveMark& mark_before=curve.GetLevelMark(j-1);
							VCurveMark& mark_after=curve.GetLevelMark(j);
							value=mark_before.value+(mark_after.value-mark_before.value)*(abs_coord-absCoord.GetItem(mark_after.index))/(absCoord.GetItem(mark_after.index)-absCoord.GetItem(mark_before.index));
							break;
						}

					sum+=value;
					sumkv+=pow(value,2);
				}
			}
		}
		moLevel=sum/level.GetCount();
		skoLevel=sqrt((sumkv-pow(sum,2)/level.GetCount())/(level.GetCount()-1));
	}

	if (this->Load(PRM_PIT_LEFT))
	{
		moPitLeft=this->GetParameter(PRM_PIT_LEFT).GetMO();
		skoPitLeft=this->GetParameter(PRM_PIT_LEFT).GetSKO((float)moPitLeft);
	}
	if (this->Load(PRM_PIT_RIGHT))
	{
		moPitRight=this->GetParameter(PRM_PIT_RIGHT).GetMO();
		skoPitRight=this->GetParameter(PRM_PIT_RIGHT).GetSKO((float)moPitRight);
	}
	if (this->Load(PRM_CURV))
	{
		this->Load(PRM_CURVE_MARKS);
		VParameter & curv=this->GetParameter(PRM_CURV);
		VAprinfo aprinfo=this->GetAprinfo();
		VCurveList & curveList=this->GetCurves();
		size_t curve_num=0;
		double sum=0, sumkv=0, mo=0, sko=0;
		for (size_t i=0;i<curv.GetCount();i++)
		{
			double abs_coord=this->GetAbsCoord().GetItem(i);
			if (curve_num>=curveList.GetCount())
			{
			}
			else
			{
				VAbsCoordData & absCoord = this->GetAbsCoord();
				VCurve & curve=curveList.GetItem(curve_num);
				VCurveMark& mark1=curve.GetPlanMark(0);
				VCurveMark& mark2=curve.GetPlanMark(curveList.GetItem(curve_num).GetMarkCount()-1);
				if (abs_coord<absCoord.GetItem(mark1.index))
				{
				}
				else if (absCoord.GetItem(mark2.index)<abs_coord)
				{
					curve_num++;
				}
				else
				{
					double value=0;
					for (size_t j=0; j<curve.GetMarkCount();j++)
						if (absCoord.GetItem(curve.GetPlanMark(j).index)==abs_coord)
						{
							value=curve.GetPlanMark(j).value;
							break;
						}
						else if (absCoord.GetItem(curve.GetPlanMark(j).index)>abs_coord)
						{
							VCurveMark& mark_before=curve.GetPlanMark(j-1);
							VCurveMark& mark_after=curve.GetPlanMark(j);
							value=mark_before.value+(mark_after.value-mark_before.value)*(abs_coord-absCoord.GetItem(mark_after.index))/(absCoord.GetItem(mark_after.index)-absCoord.GetItem(mark_before.index));
							break;
						}

					sum+=value;
					sumkv+=pow(value,2);
				}
			}
		}
		moCurv=sum/curv.GetCount();
		skoCurv=sqrt((sumkv-pow(sum,2)/curv.GetCount())/(curv.GetCount()-1));
	}
	if (this->Load(PRM_DIST_SHORT))
	{
		moDistShort=this->GetParameter(PRM_DIST_SHORT).GetMO();
		skoDistShort=this->GetParameter(PRM_DIST_SHORT).GetSKO((float)moDistShort);
	}
	if (this->Load(PRM_DIST_LONG))
	{
		moDistLong=this->GetParameter(PRM_DIST_LONG).GetMO();
		skoDistLong=this->GetParameter(PRM_DIST_LONG).GetSKO((float)moDistLong);
	}
	if (this->Load(PRM_WEAR_LEFT))
	{
		moWearLeft=this->GetParameter(PRM_WEAR_LEFT).GetMO();
		skoWearLeft=this->GetParameter(PRM_WEAR_LEFT).GetSKO((float)moWearLeft);
	}
	if (this->Load(PRM_WEAR_RIGHT))
	{
		moWearRight=this->GetParameter(PRM_WEAR_RIGHT).GetMO();
		skoWearRight=this->GetParameter(PRM_WEAR_RIGHT).GetSKO((float)moWearRight);
	}
	if (this->Load(PRM_USK_VERT))
	{
		moUskVert=this->GetParameter(PRM_USK_VERT).GetMO();
		skoUskVert=this->GetParameter(PRM_USK_VERT).GetSKO((float)moUskVert);
	}
	if (this->Load(PRM_USK_HOR))
	{
		moUskHor=this->GetParameter(PRM_USK_HOR).GetMO();
		skoUskHor=this->GetParameter(PRM_USK_HOR).GetSKO((float)moUskHor);
	}

	//Cохранение в HTML
	if (filter==0)
	{
		VAbsCoordData & abs_coord=this->GetAbsCoord();
		std::string pch_list;
		std::vector<VAprinfoPchRecord> pchList=aprinfo.GetPchList(abs_coord.GetItem(0),abs_coord.GetLastItem());
		for (size_t i=0; i<pchList.size(); i++)
		{
			char buf[5];
			pch_list.append(_ltoa(pchList.at(i).pchCode,buf,10));
			pch_list.append(";");
		}
		VTime date(time(0));
		output<<"<html>\n<head><title>Ведомость СКО параметров</title></head>\n";
		output<<"<style type=\"text/css\">";
		output<<"table {border-collapse: collapse; }";
		output<<"tr { font-size:10pt; }";
        output<<"td { font-size:10pt; text-align: center; }";
		output<<"tr.a { text-align: center; font-weight: bold; background-color: #deb887;}";
		output<<"tr.b { text-align: center; font-weight: bold; background-color: #FFCC99; border-top: solid;}";
		output<<"tr.c { text-align: center; background-color: #FFE4E1;}";
		output<<"tr.d { text-align: center; background-color: #FFDAB9;}";
		output<<"td.e { text-align: left;}";
		output<<"</style>";
		output<<"<body><h3 align=\"center\">Ведомость СКО параметров <br/>";
		output<<"от "<<date.wDay<<"."<<date.wMonth<<"."<<date.wYear<<"</h3>";

		output << std::fixed << std::setprecision(2);
		output.imbue(std::locale(""));

		output<<"<table align=\"center\" border=\"1\" bordercolor=\"black\" cellspacing=\"0\" rules=\"all\" width=\"100%\" style=\"border-collapse: collapse\">";
		output<<"<tr class=\"a\"><td colspan=\"2\" class=\"e\">"<<"Направление: "<<this->GetDirName()<<"</td><td>"<<"Путь: "<<this->GetWay()<<"</td></tr>";
		output<<"<tr class=\"a\"><td class=\"e\">"<<"Участок: "<<this->GetPeregon()<<"</td><td>ПЧ: "<<pch_list<<"</td><td>Км: "<<this->GetSection().start.km<<"-"<<this->GetSection().end.km<<"</td></tr>";
		output<<"<tr class=\"b\"><th width=\"70%\">Имя параметра</th><th width=\"15%\">Среднее</th><th width=\"15%\">СКО</th><tr>";

		if (this->GetParameter(PRM_WIDTH).IsLoaded())
			output<<"<tr class=\"c\"><td class=\"e\">"<<wstring_to_string(GetUserParamName(PRM_WIDTH)).c_str()<<"(отклонения от нормы)</td><td>"<<moWidth<<"</td><td>"<<skoWidth<<"</td></tr>";
		if (this->GetParameter(PRM_LEVEL).IsLoaded())
			output<<"<tr class=\"d\"><td class=\"e\">"<<wstring_to_string(GetUserParamName(PRM_LEVEL)).c_str()<<"</td><td>"<<moLevel<<"</td><td>"<<skoLevel<<"</td></tr>";
		if (this->GetParameter(PRM_PIT_LEFT).IsLoaded())
			output<<"<tr class=\"c\"><td class=\"e\">"<<wstring_to_string(GetUserParamName(PRM_PIT_LEFT)).c_str()<<"</td><td>"<<moPitLeft<<"</td><td>"<<skoPitLeft<<"</td></tr>";
		if (this->GetParameter(PRM_PIT_RIGHT).IsLoaded())
			output<<"<tr class=\"d\"><td class=\"e\">"<<wstring_to_string(GetUserParamName(PRM_PIT_RIGHT)).c_str()<<"</td><td>"<<moPitRight<<"</td><td>"<<skoPitRight<<"</td></tr>";
		if (this->GetParameter(PRM_CURV).IsLoaded())
			output<<"<tr class=\"c\"><td class=\"e\">"<<wstring_to_string(GetUserParamName(PRM_CURV)).c_str()<<"</td><td>"<<moCurv<<"</td><td>"<<skoCurv<<"</td></tr>";
		if (this->GetParameter(PRM_DIST_SHORT).IsLoaded())
			output<<"<tr class=\"d\"><td class=\"e\">"<<wstring_to_string(GetUserParamName(PRM_DIST_SHORT)).c_str()<<"</td><td>"<<moDistShort<<"</td><td>"<<skoDistShort<<"</td></tr>";
		if (this->GetParameter(PRM_DIST_LONG).IsLoaded())
			output<<"<tr class=\"c\"><td class=\"e\">"<<wstring_to_string(GetUserParamName(PRM_DIST_LONG)).c_str()<<"</td><td>"<<moDistLong<<"</td><td>"<<skoDistLong<<"</td></tr>";
		if (this->GetParameter(PRM_WEAR_LEFT).IsLoaded())
			output<<"<tr class=\"d\"><td class=\"e\">"<<wstring_to_string(GetUserParamName(PRM_WEAR_LEFT)).c_str()<<"</td><td>"<<moWearLeft<<"</td><td>"<<skoWearLeft<<"</td></tr>";
		if (this->GetParameter(PRM_WEAR_RIGHT).IsLoaded())
			output<<"<tr class=\"c\"><td class=\"e\">"<<wstring_to_string(GetUserParamName(PRM_WEAR_RIGHT)).c_str()<<"</td><td>"<<moWearRight<<"</td><td>"<<skoWearRight<<"</td></tr>";
		if (this->GetParameter(PRM_USK_VERT).IsLoaded())
			output<<"<tr class=\"d\"><td class=\"e\">"<<wstring_to_string(GetUserParamName(PRM_USK_VERT)).c_str()<<"</td><td>"<<moUskVert<<"</td><td>"<<skoUskVert<<"</td></tr>";
		if (this->GetParameter(PRM_USK_HOR).IsLoaded())
			output<<"<tr class=\"c\"><td class=\"e\">"<<wstring_to_string(GetUserParamName(PRM_USK_HOR)).c_str()<<"</td><td>"<<moUskHor<<"</td><td>"<<skoUskHor<<"</td></tr>";
	}

	//Сохранение в CSV
	else if (filter==1)
	{
		output << std::fixed << std::setprecision(2);
		output.imbue(std::locale(""));
		output<<"Имя параметра"<<divider<<"Среднее"<<divider<<"СКО"<<std::endl;

		if (this->GetParameter(PRM_WIDTH).IsLoaded())
			output<<GetUserParamName(PRM_WIDTH)<<" (отклонения от нормы)"<<divider<<moWidth<<divider<<skoWidth<<std::endl;
		if (this->GetParameter(PRM_LEVEL).IsLoaded())
			output<<GetUserParamName(PRM_LEVEL)<<divider<<moLevel<<divider<<skoLevel<<std::endl;
		if (this->GetParameter(PRM_PIT_LEFT).IsLoaded())
			output<<GetUserParamName(PRM_PIT_LEFT)<<divider<<moPitLeft<<divider<<skoPitLeft<<std::endl;
		if (this->GetParameter(PRM_PIT_RIGHT).IsLoaded())
			output<<GetUserParamName(PRM_PIT_RIGHT)<<divider<<moPitRight<<divider<<skoPitRight<<std::endl;
		if (this->GetParameter(PRM_CURV).IsLoaded())
			output<<GetUserParamName(PRM_CURV)<<divider<<moCurv<<divider<<skoCurv<<std::endl;
		if (this->GetParameter(PRM_DIST_SHORT).IsLoaded())
			output<<GetUserParamName(PRM_DIST_SHORT)<<divider<<moDistShort<<divider<<skoDistShort<<std::endl;
		if (this->GetParameter(PRM_DIST_LONG).IsLoaded())
			output<<GetUserParamName(PRM_DIST_LONG)<<divider<<moDistLong<<divider<<skoDistLong<<std::endl;
		if (this->GetParameter(PRM_WEAR_LEFT).IsLoaded())
			output<<GetUserParamName(PRM_WEAR_LEFT)<<divider<<moWearLeft<<divider<<skoWearLeft<<std::endl;
		if (this->GetParameter(PRM_WEAR_RIGHT).IsLoaded())
			output<<GetUserParamName(PRM_WEAR_RIGHT)<<divider<<moWearRight<<divider<<skoWearRight<<std::endl;
		if (this->GetParameter(PRM_USK_VERT).IsLoaded())
			output<<GetUserParamName(PRM_USK_VERT)<<divider<<moUskVert<<divider<<skoUskVert<<std::endl;
		if (this->GetParameter(PRM_USK_HOR).IsLoaded())
			output<<GetUserParamName(PRM_USK_HOR)<<divider<<moUskHor<<divider<<skoUskHor<<std::endl;
	}

	return total;
}

// Коррекция абс.к-ты
void VDataSet::CorrectCoord(double offset_m, double expand_m_per_km)
{
	if (this->Load(PRM_ABS_COORD))	this->SetChanged(PRM_ABS_COORD);
	else return;

	if(OPTIONS.GetOption(L"PLAN_MARKS_COORD_CORRECTION").Compare(OPTION_YES))
	{
		if (this->Load(PRM_CURVE_MARKS))	this->SetChanged(PRM_CURVE_MARKS);
		if (this->Load(PRM_MODEL_CURVES))	this->SetChanged(PRM_MODEL_CURVES);
	}
	if(OPTIONS.GetOption(L"PROFILE_MARKS_COORD_CORRECTION").Compare(OPTION_YES))
		if (this->Load(PRM_PROF_MARKS))		this->SetChanged(PRM_PROF_MARKS);

	this->GetAbsCoord().Correct(offset_m, expand_m_per_km);
}

// Копирование из DataSet - границы в виде абс. координат
void VDataSet::CopyFrom(VDataSet & source, 
						double absFrom, double absTo,
						VProgressBox * progress)
{
	VWaySection section(
		source.GetFileAprinfo().GetRealCoord(absFrom), 
		source.GetFileAprinfo().GetRealCoord(absTo));

	return this->CopyFrom(source, section, progress);
}

// Копирование из DataSet
void VDataSet::CopyFrom(VDataSet & source, 
						const VWaySection & section, 
						VProgressBox * progress)
{
	source.Load(PRM_ABS_COORD);
	size_t kadrCount = source.GetAbsCoord().GetCount();
	size_t kadrFrom = source.GetAbsCoord().FindIndex(source.GetFileAprinfo().GetAbsCoord(section.start), true);
	size_t kadrTo = source.GetAbsCoord().FindIndex(source.GetFileAprinfo().GetAbsCoord(section.end), true);

	// Копируемые
	PARAMETER_TYPE params[] = 
	{
		// априорная информация
		PRM_APRINFO,
		
		// специальные параметры 
		PRM_ABS_COORD, PRM_CORRUPT, PRM_SPEED,

		// измеренные параметры, типа float
		PRM_KURS, PRM_KURS_BACKUP, PRM_GRADIENT, PRM_LEVEL, PRM_WIDTH, PRM_WIDTH_FRONT, PRM_WIDTH_BACK, 
		PRM_PIT_LEFT, PRM_PIT_RIGHT, PRM_CURV, PRM_DIST_SHORT, PRM_DIST_LONG, 
		PRM_RICH_LEFT, PRM_RICH_RIGHT, PRM_TEMP_LEFT, PRM_TEMP_RIGHT, 
		PRM_WEAR_LEFT, PRM_WEAR_RIGHT, PRM_UNI_WEAR_SENSOR, 
		PRM_USK_VERT, PRM_USK_HOR, PRM_LATITUDE, PRM_LONGITUDE, 
		PRM_PPR_WIDTH, PRM_PPR_NAK_LEFT, PRM_PPR_NAK_RIGHT,
		PRM_PPR_POD_LEFT, PRM_PPR_POD_RIGHT, PRM_PPR_IZN_LEFT, PRM_PPR_IZN_RIGHT, 
		PRM_PPR_IZN_INT_LEFT, PRM_PPR_IZN_INT_RIGHT, PRM_PPR_IZN_EXT_LEFT, PRM_PPR_IZN_EXT_RIGHT,
		PRM_TEST_4, PRM_TEST_7, PRM_TEST_8, 

		// измеренные параметры, типа double
		PRM_GPS_TIME, PRM_GPS_LATITUDE, PRM_GPS_LONGITUDE, PRM_GPS_ALTITUDE,
	
		// параметры, привязанные к числу кадров
		PRM_OP_MARKS, PRM_UGON_LEFT, PRM_UGON_RIGHT, PRM_PROF_MARKS, 
		PRM_SPALA_MARKER, PRM_LINE_MARKS, PRM_ZAZOR_LEFT, PRM_ZAZOR_RIGHT, 
		PRM_MAG_ZAZOR, PRM_MAG_ZAZOR_LEFT, PRM_MAG_ZAZOR_RIGHT,

		// параметры привязанные к кадру
		PRM_OVRS2, PRM_OVRS3, PRM_CURVE_MARKS, PRM_MODEL_CURVES, 
		PRM_CONTROL_AREA, PRM_SIDEWAYS, PRM_HISTORY, PRM_COMMENT, 

		// voila
		PRM_NONE
	};

	// количество параметров
	size_t paramCount = 0;
	for (size_t paramNdx = 0; params[paramNdx] != PRM_NONE; ++paramNdx)
	{
		if (source.DataExists(params[paramNdx]))
			++paramCount;
	}

	if (progress != NULL)
		progress->SetRange(0, paramCount);

	// Загрузка данных
	size_t okCount = 0; 
	std::vector<PARAMETER_TYPE> not_loaded;

	float latitude = 0, longitude = 0;
	for (size_t i = 0; params[i] != PRM_NONE; ++i)
	{
		if (progress != NULL)
		{
			std::wstringstream msg;
			msg << L"Загрузка параметра: " + std::wstring(GetUserParamName(params[i]));
			progress->SetMessage(0, msg.str().c_str());
			progress->Step(0);
			LOG_INFO(msg.str());
		}

		switch(params[i])
		{
		case PRM_APRINFO:
			if (source.Load(params[i]))
			{
				++okCount;
				this->GetAprinfo() = source.GetAprinfo();
				this->SetChanged(params[i]);
			}
			else
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_COMMENT:
			if (source.Load(params[i]))
			{
				++okCount;
				this->GetComment() = source.GetComment();
				this->SetChanged(params[i]);
			}
			else
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_ABS_COORD:
			if (source.Load(params[i])) 
			{
				++okCount;
				if (this->GetAbsCoord().Create(GetFileCodeByParam(params[i]), kadrTo - kadrFrom + 1, source.GetAbsCoord().GetVersion(), 0.0))
				{
					for (size_t k = kadrFrom; k <= kadrTo; ++k)
					{
						this->GetAbsCoord().GetItem(k - kadrFrom) = source.GetAbsCoord().GetItem(k);
					}
					this->SetChanged(params[i]);
				}
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_CORRUPT:
			if (source.Load(params[i])) 
			{
				++okCount;
				this->GetCorrupt().CopyFrom(source.GetCorrupt(), kadrFrom, kadrTo);
				this->SetChanged(params[i]);
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_GPS_TIME:
		case PRM_GPS_LATITUDE:
		case PRM_GPS_LONGITUDE:
		case PRM_GPS_ALTITUDE:
			if (source.Load(params[i]))
			{
				++okCount;
				if (this->GetDoubleParameter(params[i]).Create(GetFileCodeByParam(params[i]), kadrTo - kadrFrom + 1, source.GetDoubleParameter(params[i]).GetVersion(), 0.0))
				{
					for (size_t k = kadrFrom; k <= kadrTo; ++k)
					{
						this->GetDoubleParameter(params[i]).GetItem(k - kadrFrom) = source.GetDoubleParameter(params[i]).GetItem(k);
					}
					this->SetChanged(params[i]);
				}
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_CURVE_MARKS:
			if (source.Load(params[i])) 
			{	
				++okCount;
				this->GetCurves().CopyFrom(source.GetCurves(), source.GetAbsCoord(), this->GetAbsCoord());
				this->SetChanged(params[i]);
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_MODEL_CURVES:
			if (source.Load(params[i])) 
			{	
				++okCount;
				this->GetCurves(PRM_MODEL_CURVES).CopyFrom(source.GetCurves(PRM_MODEL_CURVES), source.GetAbsCoord(), this->GetAbsCoord());
				this->SetChanged(params[i]);
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_OP_MARKS:
			if (source.Load(params[i])) 
			{
				++okCount;
				this->GetOpMarks().CopyFrom(source.GetOpMarks(), source.GetAbsCoord(), this->GetAbsCoord());
				this->SetChanged(params[i]);
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_MAG_ZAZOR:
			if (source.Load(params[i])) 
			{
				++okCount;
				this->GetMagZazors().CopyFrom(source.GetMagZazors(), source.GetAbsCoord(), this->GetAbsCoord());
				this->SetChanged(params[i]);
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_ZAZOR_LEFT:
			if (source.Load(params[i]))
			{
				++okCount;
				this->GetZazorLeft().CopyFrom(source.GetZazorLeft(), source.GetAbsCoord(), this->GetAbsCoord());
				this->SetChanged(params[i]);
			}
			else
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_ZAZOR_RIGHT:
			if (source.Load(params[i]))
			{
				++okCount;
				this->GetZazorRight().CopyFrom(source.GetZazorRight(), source.GetAbsCoord(), this->GetAbsCoord());
				this->SetChanged(params[i]);
			}
			else
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_MAG_ZAZOR_LEFT:
			if (source.Load(params[i]))
			{
				++okCount;
				this->GetMagZazorLeft().CopyFrom(source.GetMagZazorLeft(), source.GetAbsCoord(), this->GetAbsCoord());
				this->SetChanged(params[i]);
			}
			else
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_MAG_ZAZOR_RIGHT:
			if (source.Load(params[i]))
			{
				++okCount;
				this->GetMagZazorRight().CopyFrom(source.GetMagZazorRight(), source.GetAbsCoord(), this->GetAbsCoord());
				this->SetChanged(params[i]);
			}
			else
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_UGON_LEFT:
			if (source.Load(params[i])) 
			{
				++okCount;
				this->GetUgonLeft().CopyFrom(source.GetUgonLeft(), source.GetAbsCoord(), this->GetAbsCoord());
				this->SetChanged(params[i]);
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_UGON_RIGHT:
			if (source.Load(params[i])) 
			{
				++okCount;
				this->GetUgonRight().CopyFrom(source.GetUgonRight(), source.GetAbsCoord(), this->GetAbsCoord());
				this->SetChanged(params[i]);
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_SIDEWAYS:
			if (source.Load(params[i])) 
			{
				++okCount;
				this->GetSideways().CopyFrom(source.GetSideways(), source.GetAbsCoord(), this->GetAbsCoord());
				this->SetChanged(params[i]);
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_PROF_MARKS:
			if (source.Load(params[i])) 
			{
				++okCount;
				this->GetProfMarks().CopyFrom(source.GetProfMarks(), source.GetAbsCoord(), this->GetAbsCoord());
				this->SetChanged(params[i]);
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_SPALA_MARKER:
			if (source.Load(params[i]))
			{
				++okCount;
				this->GetSpalaMarker().CopyFrom(source.GetSpalaMarker(), source.GetAbsCoord(), this->GetAbsCoord());
				this->SetChanged(params[i]);
			}
			else
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_OVRS2: 
			if (source.Load(params[i])) 
			{
				++okCount;
				this->GetOvrs2().CopyFrom(source.GetOvrs2(), &section);
				this->SetChanged(params[i]);
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_OVRS3:
			if (source.Load(params[i]))
			{
				++okCount;
				this->GetOvrs3().CopyFrom(source.GetOvrs3(), &section);
				this->SetChanged(params[i]);
			}
			else
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_HISTORY:
			if (source.Load(params[i])) 
			{
				++okCount;
				this->GetHistory() = source.GetHistory();
				this->SetChanged(params[i]);
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_CONTROL_AREA:
			if (source.Load(params[i])) 
			{
				++okCount;
				this->GetControlAreas().CopyFrom(source.GetControlAreas(), &section);
				this->SetChanged(params[i]);
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		case PRM_LINE_MARKS:
			if (source.Load(params[i])) 
			{
				++okCount;

				if (!this->GetLineMarks().CopyFrom(source.GetLineMarks(), source.absCoord,
					this->absCoord))
				{
					this->GetLineMarks().Unload();
					break;
				}

				this->SetChanged(params[i]);
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
			break;

		default:

			if (source.Load(params[i]))
			{
				++okCount; 
				VParameter & dest = this->GetParameter(params[i]);
				VParameter & src = source.GetParameter(params[i]);

				if (params[i] == PRM_WEAR_LEFT || params[i] == PRM_WEAR_RIGHT)
				{
					int Z = 0;
				}

				if (dest.Create(GetFileCodeByParam(params[i]), kadrTo - kadrFrom + 1, src.GetVersion(), 0.0f))
				{

					for (size_t k = kadrFrom; k <= kadrTo; ++k)
					{
						dest.GetItem(k - kadrFrom) = src.GetItem(k);
						dest.SetCorrupt(k - kadrFrom, src.IsCorrupt(k));
					}

					this->SetChanged(params[i]);

					// широта в заголовок
					if (params[i] == PRM_LATITUDE && this->header.Latitude == 0)
					{

						if (source.GetMoveDirection() < 0)
							latitude = source.GetParameter(params[i]).GetLastItem();
						else
							latitude = source.GetParameter(params[i]).GetItem(0);
					}

					// долгота в заголовок
					if (params[i] == PRM_LONGITUDE && this->header.Longitude == 0)
					{
						if (source.GetMoveDirection() < 0)
							longitude = source.GetParameter(params[i]).GetLastItem();
						else
							longitude = source.GetParameter(params[i]).GetItem(0);
					}
				}
			}
			else 
			{
				not_loaded.push_back(params[i]);
			}
		}

		if (progress != NULL)
		{
			progress->Step(0);
		}
	}

	// Заголовок нового файла
	this->header = source.header;
	if (latitude != 0) this->header.Latitude = latitude;
	if (longitude != 0) this->header.Longitude = longitude;

	VOperationRecord record;
	record.SetTextDescription("Измерения скопированы из файла " + 
		wstring_to_string(source.GetFileName().GetShortName()));
	this->GetHistory().Add(record);
	this->SetChanged(PRM_HISTORY);
}

///Рекурсивная процедура деления элемента в точке максимума отклонения
bool SplitElement(VAUIProgressDialogPtr in_progress, VDataSet& in_dataset, size_t in_index, double in_min_length, double in_overstep)
{
	if (in_progress)
	{
		in_progress->UpdateProgress(0, 0);
		if (in_progress->Cancel())
		{
			return false;
		}
	}

	double max_diff=in_overstep;
	size_t start_mark=in_index;
	size_t end_mark=in_index+1;

	//Если длина длемента меньше чем два порога, то разделить его нельзя
	if (in_dataset.GetAbsCoord().GetItem(in_dataset.GetProfMarks().GetItem(end_mark).index) -
		in_dataset.GetAbsCoord().GetItem(in_dataset.GetProfMarks().GetItem(start_mark).index) < 2*in_min_length)
		return true;

	//Находим точку на элементе в которой отклонение достигают макcимума
	size_t max_index=in_dataset.GetDiffProfile().GetMaxAbsIndex(
		in_dataset.GetProfMarks().GetItem(start_mark).index, 
		in_dataset.GetProfMarks().GetItem(end_mark).index);
	//Если отклонение в этой точке больше порога ...
	if (fabs(in_dataset.GetDiffProfile().GetItem(max_index))>max_diff)
	{		
		//... добавляем в этой точке метку разбивая элемент на два новых
		VProfileMark new_mark;
		new_mark.index=max_index;
		new_mark.value=in_dataset.GetParameter(PRM_PROFILE).GetItem(new_mark.index);
		new_mark.absCoord=in_dataset.GetAbsCoord().GetItem(new_mark.index);
		new_mark.isCurve=false;
		//Проверяем длины получившихся элементов		
		//Если первый элемент меньше увеличиваем его до 200 метров
		if (in_dataset.GetAbsCoord().GetItem(max_index) - 
			in_dataset.GetAbsCoord().GetItem(in_dataset.GetProfMarks().GetItem(start_mark).index) 
			< in_min_length)
		{
			new_mark.index=in_dataset.GetAbsCoord().FindIndex(in_dataset.GetAbsCoord().GetItem(
				in_dataset.GetProfMarks().GetItem(start_mark).index) + in_min_length, true);
			new_mark.value=in_dataset.GetParameter(PRM_PROFILE).GetItem(new_mark.index);
			new_mark.absCoord=in_dataset.GetAbsCoord().GetItem(new_mark.index);
		}
		//Если длина второго элемента меньше порога увеличиваем его
		else if (in_dataset.GetAbsCoord().GetItem(in_dataset.GetProfMarks().GetItem(end_mark).index) - in_dataset.GetAbsCoord().GetItem(max_index) < in_min_length)
		{
			new_mark.index=in_dataset.GetAbsCoord().FindIndex(in_dataset.GetAbsCoord().GetItem(
				in_dataset.GetProfMarks().GetItem(end_mark).index) - in_min_length, true);
			new_mark.value=in_dataset.GetParameter(PRM_PROFILE).GetItem(new_mark.index);
			new_mark.absCoord=in_dataset.GetAbsCoord().GetItem(new_mark.index);
		}
				
		in_dataset.GetProfMarks().AddItem(new_mark);
		in_dataset.GetProfMarks().Sort();

		//Пересчитываем уклоны
		in_dataset.GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Unload();
		in_dataset.GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Load(in_dataset.GetParameter(PRM_PROFILE), 
			in_dataset.GetProfMarks(), in_dataset.GetAbsCoord(), in_dataset.GetAprinfo(), PRM_PROFILE_SLOPE_ELEMENT);
		//Пересчитываем идеальный профиль
		in_dataset.GetAproxProfile().Update(in_dataset.GetParameter(PRM_PROFILE), in_dataset.GetAbsCoord(),
			in_dataset.GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT), start_mark+1);
		//Пересчитываем отклонения
		in_dataset.GetDiffProfile().Update(in_dataset.GetParameter(PRM_PROFILE), in_dataset.GetAproxProfile(),
			in_dataset.GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT), start_mark+1);
		//Рекурсия сначала для второго нового элемента, чтобы не сбивать индексы
		if (!SplitElement(in_progress, in_dataset, start_mark+1, in_min_length, in_overstep))
			return false;
		if (!SplitElement(in_progress, in_dataset, start_mark, in_min_length, in_overstep))
			return false;
	}
	return true;
}

///Автоматически размечает профиль FixMe!!! не факт, что этой функции место внутри класса
void VDataSet::ProfileAutomark(VAUIProgressDialogPtr in_progress)
{	
	double min_length=OPTIONS.GetOption(L"MIN_PROFILE_ELEMENT_LENGTH").GetValue<double>();
	double min_overstep=OPTIONS.GetOption(L"MAX_PROFILE_OFFSET").GetValue<double>();

	VAUIProgressAutoHider hider(in_progress);
	if (in_progress)
	{
		in_progress->Show(1, L"Автоматическая разметка профиля");
		in_progress->Reset(0, 0);
		in_progress->UpdateMessage(0, L"Разметка профиля");
	}
	//Удаляем предыдущую разметку
	this->GetProfMarks().Clear();
	/*
	Алгоритм автоматической разметки:	
	1. Ставим две метки, одну - в начале, другую - в конце файла. Таким образом, получаем единтвенный элемент профиля
	2. Находим точку максимального отклонения фактического профиля от спрямленного.
	3. Если отклонение меньше заданного порога - завершаем процедуру, иначе - ставим метку профиля в точке максимума 
	   отклонения, и делим тем самым элемент на два. Если длина одного из новых элементов меньше порога 
	   (in_min_length=200 м), откатываемся назад и завершаем процедуру.
	4. Для каждого из новых элементов рекурсивно повторяем пункты 2 и 3.
	*/	
	//Добавляем метки в начале ...
	VProfileMark start_mark;
	start_mark.index=this->GetAbsCoord().FindIndex(this->GetAbsCoord().GetItem(0), true);
	start_mark.value=this->GetParameter(PRM_PROFILE).GetItem(start_mark.index);
	start_mark.absCoord=this->GetAbsCoord().GetItem(start_mark.index);
	start_mark.isCurve=false;
	this->GetProfMarks().AddItem(start_mark);
	//... и в конце файла
	VProfileMark end_mark;
	end_mark.index=this->GetAbsCoord().FindIndex(this->GetAbsCoord().GetLastItem(), true);
	end_mark.value=this->GetParameter(PRM_PROFILE).GetItem(end_mark.index);
	end_mark.absCoord=this->GetAbsCoord().GetItem(end_mark.index);
	end_mark.isCurve=false;
	this->GetProfMarks().AddItem(end_mark);
	this->GetProfMarks().Sort();

	//Пересчитываем уклоны
	this->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Unload();
	this->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Load(this->GetParameter(PRM_PROFILE), 
		this->GetProfMarks(), this->GetAbsCoord(), this->GetAprinfo(), PRM_PROFILE_SLOPE_ELEMENT);
	//Пересчитываем профиль
	this->GetAproxProfile().Unload();
	this->GetAproxProfile().Load(this->GetParameter(PRM_PROFILE), this->GetAbsCoord(), this->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT)/*this->GetProfMarks()*/);	
	//Пересчитываем отклонения
	this->GetDiffProfile().Unload();
	this->GetDiffProfile().Load(this->GetParameter(PRM_PROFILE), this->GetAproxProfile());	
	//Таким образом мы получили первый элемент для которого можно использовать рекурсивную процедуру разметки
	if (!SplitElement(in_progress, *this, 0, min_length, min_overstep))
	{
		this->GetProfMarks().Clear();
		//Пересчитываем уклоны
		this->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Unload();
		this->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Load(this->GetParameter(PRM_PROFILE), 
			this->GetProfMarks(), this->GetAbsCoord(), this->GetAprinfo(), PRM_PROFILE_SLOPE_ELEMENT);
		//Пересчитываем профиль
		this->GetAproxProfile().Unload();
		this->GetAproxProfile().Load(this->GetParameter(PRM_PROFILE), this->GetAbsCoord(), this->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT)/*this->GetProfMarks()*/);	
		//Пересчитываем отклонения
		this->GetDiffProfile().Unload();
		this->GetDiffProfile().Load(this->GetParameter(PRM_PROFILE), this->GetAproxProfile());	
	}
	this->SetChanged(PRM_PROF_MARKS);
}

/// Перенос профиля из БД
void VDataSet::ProfileFromDB(VIBDatabasePtr in_database, VAUIProgressDialogPtr in_progress)
{

	VIBTransaction transaction=in_database->StartTransaction();
	std::wstring profile_query=boost::str(boost::wformat(
		L"select e1.start_abs, e1.END_ABS, e1.type_elem, t1.DATE_TRAIL from MSR_PROFILE_ELEM e1 "
		L"inner join MSR_TRAIL t1 on t1.id=e1.MSR_TRAIL_ID "
		L"inner join ADM_WAY w on w.ID=t1.ADM_WAY_ID "
		L"inner join ADM_DIRECTION d on d.ID=w.ADM_DIRECTION_ID "
		L"where d.code='%s' and w.code='%s' and "
		L"(not exists "
		L"("
		L"select id from msr_trail t2 where t2.START_ABS < e1.START_ABS and e1.start_abs < t2.END_ABS and t1.date_trail < t2.DATE_TRAIL and t1.ADM_WAY_ID=t2.ADM_WAY_ID"
		L""
		L" "
		L"order by e1.START_ABS")
		% GetDir() % string_to_wstring(GetWay()));
	VIBDataHandlerParameter profile_handler;
	in_database->ExecuteStatement(transaction, wstring_to_string(profile_query).c_str(), profile_handler);
	if (profile_handler.GetRowCount() > 0)
	{
		//Удаляем предыдущую разметку
		this->GetProfMarks().Clear();
		//Формируем новую разметку
		for (size_t i=0; i<profile_handler.GetRowCount(); ++i)
		{
			VProfileMark mark;
			mark.index=this->GetAbsCoord().FindIndex(profile_handler.GetParameter(i,0).GetFloat(), false);
			if (mark.index!=BAD_INDEX)
			{
				mark.value=this->GetParameter(PRM_PROFILE).GetItem(mark.index);
				mark.absCoord=this->GetAbsCoord().GetItem(mark.index);
				mark.isCurve=profile_handler.GetParameter(i,2).GetString().compare(L"кривая")==0;
				this->GetProfMarks().AddItem(mark);
			}
		}
		//Пересчитываем уклоны
		this->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Unload();
		this->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Load(this->GetParameter(PRM_PROFILE), 
			this->GetProfMarks(), this->GetAbsCoord(), this->GetAprinfo(), PRM_PROFILE_SLOPE_ELEMENT);
		//Пересчитываем профиль
		this->GetAproxProfile().Unload();
		this->GetAproxProfile().Load(this->GetParameter(PRM_PROFILE), this->GetAbsCoord(), this->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT)/*this->GetProfMarks()*/);	
		//Пересчитываем отклонения
		this->GetDiffProfile().Unload();
		this->GetDiffProfile().Load(this->GetParameter(PRM_PROFILE), this->GetAproxProfile());
		this->SetChanged(PRM_PROF_MARKS);
	}
	else
	{		
		throw VSimpleException(L"В базе отсутствуют метки профиля.", 
			boost::str(boost::wformat(L"Направление %d путь %s") % GetDir() % string_to_wstring(GetWay())));
	}	
}

///Привязывает метки к профилю
void VDataSet::AdjustProfileMarkHeight()
{
	for (size_t i=0; i<this->GetProfMarks().GetCount(); ++i)
	{		
		this->GetProfMarks().GetItem(i).value=
			this->GetParameter(PRM_PROFILE).GetItem(this->GetProfMarks().GetItem(i).index);
	}
	
	this->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT).Update(this->GetParameter(PRM_PROFILE), 
		this->GetProfMarks(), this->GetAbsCoord(), this->GetAprinfo(), PRM_PROFILE_SLOPE_ELEMENT);
	this->GetAproxProfile().Unload();
	this->GetAproxProfile().Load(this->GetParameter(PRM_PROFILE), this->GetAbsCoord(), this->GetProfileSlope(PRM_PROFILE_SLOPE_ELEMENT)/*this->GetProfMarks()*/);		
	this->GetDiffProfile().Unload();
	this->GetDiffProfile().Load(this->GetParameter(PRM_PROFILE), this->GetAproxProfile());		
	
	
	this->SetChanged(PRM_PROF_MARKS);
}

/// Возвращает true, если в кадре можно ставить метку профиля
bool VDataSet::AllowMarkHereProfile(size_t in_index, double in_min_length)
{
	//Метка должна стоять не ближе чем некое пороговое значение
	for (size_t i=0; i<this->GetProfMarks().GetCount(); ++i)
	{
		if (this->GetProfMarks().GetItem(i).index == in_index ||
			fabs(this->GetAbsCoord().GetItem(this->GetProfMarks().GetItem(i).index) - 
			this->GetAbsCoord().GetItem(in_index)) < in_min_length)
			return false;
	}
	return true;
}

/// Возвращает true, если в кадре можно передвинуть метку профиля
bool VDataSet::AllowMarkHereProfile(size_t in_index, size_t in_mark_index, double in_min_length)
{
	//Метка должна стоять не ближе чем некое пороговое значение
	for (size_t i=0; i<this->GetProfMarks().GetCount(); ++i)
	{
		if (i!=in_mark_index && (this->GetProfMarks().GetItem(i).index == in_index ||
			fabs(this->GetAbsCoord().GetItem(this->GetProfMarks().GetItem(i).index) - this->GetAbsCoord().GetItem(in_index)) < in_min_length))
			return false;
	}
	return true;
}

/// Обновление отклонений от идеального плана в окрестности точки
void VDataSet::UpdatePlanOffset(size_t in_index, bool in_model, bool in_fast)
{
	//Определяем типы необходимых данных
	PARAMETER_TYPE plan_offset_type=!in_model ? PRM_PLAN_OFFSET : PRM_MODEL_PLAN_OFFSET;
	PARAMETER_TYPE plan_elem_list_type=!in_model ? PRM_PLAN_ELEM_LIST : PRM_MODEL_PLAN_ELEM_LIST;
	PARAMETER_TYPE ideal_plan_type=!in_model ? PRM_IDEAL_PLAN : PRM_MODEL_IDEAL_PLAN;
	PARAMETER_TYPE curves_type=!in_model ? PRM_CURVE_MARKS : PRM_MODEL_CURVES;	
	//Перезагружаем список элементов плана
	this->GetPlanElemList(plan_elem_list_type).Unload();		
	this->Load(plan_elem_list_type);
	//Презагружаем список прямых
	this->GetLines().Unload();
	this->Load(PRM_LINES);
	if (!this->GetIdealPlan(ideal_plan_type).IsLoaded())
		this->Load(ideal_plan_type);
	if (!this->GetParameter(plan_offset_type).IsLoaded())
		this->Load(plan_offset_type);
	//перебираем все элементы и обновляем те к которым относится точка с указанным индексом
	std::vector<size_t> affected_elements=this->GetPlanElemList(plan_elem_list_type).GetAffectedElements(in_index);	
	if (affected_elements.size()>0)
	{
		size_t first=(affected_elements.front()==0 || in_fast) ? affected_elements.front() : affected_elements.front()-1;
		size_t last=in_fast ? affected_elements.back() : std::min<size_t>(affected_elements.back()+1, this->GetPlanElemList(plan_elem_list_type).GetCount()-1);
		//Обновляем идеальный план
		this->GetIdealPlan(ideal_plan_type).Update(*this, first, last);
		//Обновляем отклонения
		this->GetPlanOffset(plan_offset_type).Update(*this, 
			this->GetPlanElemList(plan_elem_list_type).GetItem(first).idxStart,
			this->GetPlanElemList(plan_elem_list_type).GetItem(last).idxEnd);
	}	
}

/// Обновление отклонений от идеального плана
void VDataSet::UpdatePlanOffset(bool in_model)
{	
	PARAMETER_TYPE plan_offset_type=!in_model ? PRM_PLAN_OFFSET : PRM_MODEL_PLAN_OFFSET;
	PARAMETER_TYPE plan_elem_list_type=!in_model ? PRM_PLAN_ELEM_LIST : PRM_MODEL_PLAN_ELEM_LIST;
	PARAMETER_TYPE ideal_plan_type=!in_model ? PRM_IDEAL_PLAN : PRM_MODEL_IDEAL_PLAN;
	PARAMETER_TYPE curves_type=!in_model ? PRM_CURVE_MARKS : PRM_MODEL_CURVES;

	this->GetPlanElemList(plan_elem_list_type).Unload();	
	this->Load(plan_elem_list_type);

	//Презагружаем список прямых
	this->GetLines().Unload();
	this->Load(PRM_LINES);

	this->GetIdealPlan(ideal_plan_type).Unload();
	this->GetIdealPlan(ideal_plan_type).Load(*this);
	this->GetPlanOffset(plan_offset_type).Unload();
	this->GetPlanOffset(plan_offset_type).Load(*this);
}

/// Обновление модельного непогашенного ускорения
void VDataSet::UpdateAnpModel()
{
	//if (this->GetAnpModelHigh().IsLoaded())//FixMe!!!
	{
		this->GetAnpModelHigh().Unload();
		this->GetAnpModelHigh().Load(this->GetAbsCoord(), this->GetCurves(PRM_MODEL_CURVES), this->GetAprinfo(), high_prk);
	}
	//if (this->GetAnpModelHighBT().IsLoaded())//FixMe!!!
	{
		this->GetAnpModelHighBT().Unload();
		this->GetAnpModelHighBT().Load(this->GetAbsCoord(), this->GetCurves(PRM_MODEL_CURVES), this->GetAprinfo(), high_bt_prk);
	}
	//if (this->GetAnpModelPass().IsLoaded())//FixMe!!!
	{
		this->GetAnpModelPass().Unload();
		this->GetAnpModelPass().Load(this->GetAbsCoord(), this->GetCurves(PRM_MODEL_CURVES), this->GetAprinfo(), pass_prk);
	}
	//if (this->GetAnpModelTruck().IsLoaded())//FixMe!!!
	{
		this->GetAnpModelTruck().Unload();
		this->GetAnpModelTruck().Load(this->GetAbsCoord(), this->GetCurves(PRM_MODEL_CURVES), this->GetAprinfo(), crg_prk);
	}
}

/// Удаление кривых
void VDataSet::DeleteCurves(PARAMETER_TYPE in_mark_type)
{
	GetCurves(in_mark_type).Clear();
	SetChanged(in_mark_type);
	//Обновляем отклонения в плане
	UpdatePlanOffset(in_mark_type==PRM_MODEL_CURVES);
	//Обновляем непогашенные ускорения
	if (in_mark_type==PRM_MODEL_CURVES)
		UpdateAnpModel();	
	UpdateDiffLevel(in_mark_type==PRM_MODEL_CURVES);
}

/// Удаление прямых
void VDataSet::DeleteLines(PARAMETER_TYPE in_mark_type)
{
	GetLineMarks().Clear();
	SetChanged(PRM_LINE_MARKS);
	//Обновляем отклонения в плане
	UpdatePlanOffset(in_mark_type==PRM_MODEL_CURVES);
}

/// Авторазметка кривой
void VDataSet::AutomarkCurve(size_t in_start, size_t in_end, PARAMETER_TYPE in_mark_type)
{
	VCurve curve;
	VWayCoord coord=GetAprinfo().GetRealCoord(GetAbsCoord().GetItem(in_start));			
	//Пробуем разметить кривую "сложным способом"
	curve.Automark(*this, in_start, in_end, in_mark_type==PRM_MODEL_CURVES);	
	if (curve.PlanMarksValid() && curve.GetMarkCount()>0 && CheckCurveVsLine(curve.GetPlanMark(0).index, curve, in_mark_type))
	{			
		if (in_mark_type==PRM_MODEL_CURVES)
		{
			curve.ResetModelSpeeds(*this);
		}			
		GetCurves(in_mark_type).AddItem(curve);
		SetChanged(in_mark_type);
		return;
	}	
	LOG_WARNING(boost::str(boost::wformat(L"Не удалось разметить кривую на %d км %0.0f м (первая попытка)") % coord.km % coord.m));	
	
	//Пробуем разметить кривую "простым способом"
	curve.Automark1(*this, in_start, in_end, in_mark_type==PRM_MODEL_CURVES);	
	if (curve.PlanMarksValid() && curve.GetMarkCount()>0 && CheckCurveVsLine(curve.GetPlanMark(0).index, curve, in_mark_type))
	{			
		if (in_mark_type==PRM_MODEL_CURVES)
		{
			curve.ResetModelSpeeds(*this);
		}			
		GetCurves(in_mark_type).AddItem(curve);
		SetChanged(in_mark_type);
		return;
	}	
	LOG_WARNING(boost::str(boost::wformat(L"Не удалось разметить кривую на %d км %0.0f м (вторая попытка)") % coord.km % coord.m));	
}

/// Возвращает маркеры (эксперимент)
const VWayMarkerList& VDataSet::GetMarkers()
{
	//if (!IsLoaded(PRM_WAY_MARKERS))
	//	Load(PRM_WAY_MARKERS);
	return markers;
}

/// Возвращает true, если в файле есть кривые
bool VDataSet::HasCurves()
{
	//Поиск кривых
	VFindCurve curve_detector(1);
	for (size_t i=0; i<GetAbsCoord().GetCount(); ++i)
	{
		curve_detector.Put(VPoint(i, GetParameter(PRM_CURV).GetItem(i)));
		while (curve_detector.Ready())
		{
			return true;
		}		
	}
	return false;
}

/// Поиск и первоначальная разметка кривых
void VDataSet::SearchCurves(VAUIProgressDialogPtr in_progress, PARAMETER_TYPE in_mark_type)
{	
	VAUIProgressAutoHider hider(in_progress);
	GetCurves(in_mark_type).Clear();		
	if (in_progress)
	{
		in_progress->Show(1, L"Автоматическая разметка кривых");
		in_progress->Reset(0, GetAbsCoord().GetCount());
		in_progress->UpdateMessage(0, L"Поиск кривых");
	}
	
	//Поиск кривых
	VFindCurve curve_detector(1);
	for (size_t i=0; i<GetAbsCoord().GetCount(); ++i)
	{
		curve_detector.Put(VPoint(GetAbsCoord().GetItem(i), GetParameter(PRM_CURV).GetItem(i)));
		while (curve_detector.Ready())
		{
			VProtoCurve e=curve_detector.Front();
			size_t start=GetAbsCoord().FindIndex(e.GetStart(), false);
			size_t end=GetAbsCoord().FindIndex(e.GetEnd(), false);
			if (start!=BAD_INDEX && end!=BAD_INDEX)
			{
				double angle=fabs(GetParameter(PRM_KURS).GetItem(end)-GetParameter(PRM_KURS).GetItem(start));
				double length=e.GetLength();
				if (angle > angle_t(0, OPTIONS.GetOption(L"MIN_CURVE_ANGLE").GetValue<double>(), 0).get_rad() &&				  
					length > OPTIONS.GetOption(L"MIN_CURVE_LENGTH").GetValue<double>())
				{
					AutomarkCurve(start, end, in_mark_type);				
				}
			}
			curve_detector.Pop();
		}		
		if (in_progress)
		{
			in_progress->UpdateProgress(0, i);
			if (in_progress->Cancel())
			{
				break;
			}
		}
	}			
	
	if (in_progress)
	{
		in_progress->Reset(0, GetCurves(in_mark_type).GetCount());
	}
	GetCurves(in_mark_type).Sort();

	//Нормализация кривых
	for (size_t i=0; i<GetCurves(in_mark_type).GetCount(); ++i)
	{		
		GetCurves(in_mark_type).ActivateCurve(i);
		GetCurves(in_mark_type).GetItem(i).NormalizeMarks(*this);		
		GetCurves(in_mark_type).DeactivateCurve();
		if (in_progress)
		{
			std::wstring msg=boost::str(boost::wformat(L"Кривая %d из %d") % (i+1) % GetCurves(in_mark_type).GetCount());
			in_progress->UpdateMessage(0, msg);
			in_progress->UpdateProgress(0, i);
			if (in_progress->Cancel())
			{
				break;
			}
		}
	}		

	//Обновляем отклонения в плане
	UpdatePlanOffset(in_mark_type==PRM_MODEL_CURVES);
	//Обновляем непогашенные ускорения
	if (in_mark_type==PRM_MODEL_CURVES)
		UpdateAnpModel();	
	//Обновляем отклонения уровня от разметки
	UpdateDiffLevel(in_mark_type==PRM_MODEL_CURVES);
}

/// Копирование кривых
void VDataSet::CopyCurves(PARAMETER_TYPE in_mark_type)
{
	PARAMETER_TYPE source_type=in_mark_type==PRM_CURVE_MARKS ? PRM_MODEL_CURVES : PRM_CURVE_MARKS;
	this->Load(source_type);
	if (this->GetCurves(source_type).GetCount()==0) 
	{
		LOG_WARNING(L"Исходная разметка отсутствует");
	}
	this->GetCurves(in_mark_type).DeactivateCurve();
	this->GetCurves(in_mark_type).CopyFrom(
		this->GetCurves(source_type),
		this->GetAbsCoord(), this->GetAbsCoord());
	for (size_t i=0; i<this->GetCurves(in_mark_type).GetCount(); ++i)
	{
		this->GetCurves(in_mark_type).GetItem(i).SetModel(in_mark_type==PRM_MODEL_CURVES);
		if (in_mark_type==PRM_MODEL_CURVES)
		{
			this->GetCurves(in_mark_type).GetItem(i).ResetModelSpeeds(*this);
		}
	}
	this->SetChanged(in_mark_type);

	this->UpdatePlanOffset(in_mark_type==PRM_MODEL_CURVES);
	//Обновляем непогашенные ускорения
	if (in_mark_type==PRM_MODEL_CURVES)
		this->UpdateAnpModel();
	//Обновляем отклонения уровня от разметки
	this->UpdateDiffLevel(in_mark_type==PRM_MODEL_CURVES);
}

/// Перенос кривых из БД
void VDataSet::TransferPassportCurves(VIBDatabasePtr in_database,  VAUIProgressDialogPtr in_progress, PARAMETER_TYPE in_mark_type)
{		
	VIBTransaction transaction=in_database->StartTransaction();
	std::wstring curves_query=boost::str(boost::wformat(
		L"select c.ID, cs.abscoord, ce.abscoord, ls.abscoord, le.abscoord, c.enu_side_id from apr_curve c "
		L"inner join apr_curve_period p on p.id=c.apr_curve_period_id "
		L"inner join adm_way w on w.id=p.adm_way_id "
		L"inner join adm_direction d on d.id=w.adm_direction_id "		
		L"left join get_abscoord(w.id, c.crv_start_km, c.crv_start_m, CURRENT_DATE) cs on 1=1 "
		L"left join get_abscoord(w.id, c.crv_end_km, c.crv_end_m, CURRENT_DATE) ce on 1=1 "
		L"left join get_abscoord(w.id, c.lvl_start_km, c.lvl_start_m, CURRENT_DATE) ls on 1=1 "
		L"left join get_abscoord(w.id, c.lvl_end_km, c.lvl_end_m, CURRENT_DATE) le on 1=1 "
		L"where d.code='%d' and w.code='%s' and p.start_date<CURRENT_DATE and CURRENT_DATE<p.end_date and "
		L"cs.abscoord < %f and ce.abscoord > %f")
		% GetDir() % string_to_wstring(GetWay()) % this->GetAbsCoord().GetLastItem() % this->GetAbsCoord().GetItem(0));
	VIBDataHandlerParameter curves_handler;
	in_database->ExecuteStatement(transaction, wstring_to_string(curves_query).c_str(), curves_handler);
	if (curves_handler.GetRowCount() > 0)
	{
		GetCurves(in_mark_type).Clear();
		for (size_t i=0; i<curves_handler.GetRowCount(); ++i)
		{
			VCurve curve;
			
			size_t index=this->GetAbsCoord().FindIndex(curves_handler.GetParameter(i,1).GetFloat(), false);
			if (index!=BAD_INDEX && AllowMarkHerePlan(index, in_mark_type))
				curve.AddPlanMark(index, 0);
			index=this->GetAbsCoord().FindIndex(curves_handler.GetParameter(i,2).GetFloat(), false);
			if (index!=BAD_INDEX && AllowMarkHerePlan(index, in_mark_type))
				curve.AddPlanMark(index, 0);
			index=this->GetAbsCoord().FindIndex(curves_handler.GetParameter(i,3).GetFloat(), false);
			if (index!=BAD_INDEX && AllowMarkHereLevel(index, in_mark_type))
				curve.AddLevelMark(index, 0);
			index=this->GetAbsCoord().FindIndex(curves_handler.GetParameter(i,4).GetFloat(), false);
			if (index!=BAD_INDEX && AllowMarkHereLevel(index, in_mark_type))
				curve.AddLevelMark(index, 0);
			int side=curves_handler.GetParameter(i,5).GetInteger();
			
			std::wstring elcurves_query=boost::str(boost::wformat(
				L"select cs.abscoord, e.radius, e.crv_len, ls.abscoord, e.lvl, e.lvl_len from apr_elcurve e "
				L"inner join apr_curve c on c.id=e.apr_curve_id "
				L"inner join apr_curve_period p on p.id=c.apr_curve_period_id "
				L"left join get_abscoord(p.adm_way_id, e.crv_start_km, e.crv_start_m, CURRENT_DATE) cs on 1=1 "
				L"left join get_abscoord(p.adm_way_id, e.crv_start_km, e.crv_start_m, CURRENT_DATE) ls on 1=1 "
				L"where apr_curve_id=%d ") % curves_handler.GetParameter(i,0).GetInteger());
			VIBDataHandlerParameter elcurves_handler;
			in_database->ExecuteStatement(transaction, wstring_to_string(elcurves_query).c_str(), elcurves_handler);
			for (size_t j=0; j<elcurves_handler.GetRowCount(); ++j)
			{
				double crv_start=elcurves_handler.GetParameter(j,0).GetFloat();
				double radius=radius_to_curv(elcurves_handler.GetParameter(j,1).GetFloat());
				double crv_len=elcurves_handler.GetParameter(j,2).GetFloat();
				double lvl_start=elcurves_handler.GetParameter(j,3).GetFloat();
				double lvl=elcurves_handler.GetParameter(j,4).GetFloat();
				double lvl_len=elcurves_handler.GetParameter(j,5).GetFloat();

				size_t index=this->GetAbsCoord().FindIndex(crv_start, false);
				if (index!=BAD_INDEX && AllowMarkHerePlan(index, in_mark_type))
					curve.AddPlanMark(index, side*radius);

				index=this->GetAbsCoord().FindIndex(crv_start+crv_len, false);
				if (index!=BAD_INDEX && AllowMarkHerePlan(index, in_mark_type))
					curve.AddPlanMark(index, side*radius);

				index=this->GetAbsCoord().FindIndex(lvl_start, false);
				if (index!=BAD_INDEX && AllowMarkHereLevel(index, in_mark_type))
					curve.AddLevelMark(index, side*lvl);

				index=this->GetAbsCoord().FindIndex(lvl_start+lvl_len, false);
				if (index!=BAD_INDEX && AllowMarkHereLevel(index, in_mark_type))
					curve.AddLevelMark(index, side*lvl);
			}
			if (curve.CheckMarks())
			{
				curve.FixCrvArea(*this);	
				curve.SetModel(in_mark_type==PRM_MODEL_CURVES);
				if (in_mark_type==PRM_MODEL_CURVES)
				{
					curve.ResetModelSpeeds(*this);
				}
				GetCurves(in_mark_type).AddItem(curve);
				SetChanged(in_mark_type);
			}
		}
		//Обновляем отклонения в плане
		UpdatePlanOffset(in_mark_type==PRM_MODEL_CURVES);
		//Обновляем непогашенные ускорения
		if (in_mark_type==PRM_MODEL_CURVES)
			UpdateAnpModel();
		//Обновляем отклонения уровня от разметки
		UpdateDiffLevel(in_mark_type==PRM_MODEL_CURVES);
	}
	else
	{		
		throw VSimpleException(L"В базе отсутствуют априорные кривые.", 
			boost::str(boost::wformat(L"Направление %d путь %s") % GetDir() % string_to_wstring(GetWay())));
	}	
}

/// Перенос кривых из БД
void VDataSet::TransferMeasuredCurves(VIBDatabasePtr in_database, VAUIProgressDialogPtr in_progress, PARAMETER_TYPE in_mark_type)
{		
	double abs_start=GetAbsCoord().GetItem(0);
	double abs_end=GetAbsCoord().GetLastItem();
	VIBTransaction transaction=in_database->StartTransaction();
	std::wstring curves_query=boost::str(boost::wformat(
		L"select c.ID, m.CRV_ABS, m.CRV_VALUE, m.LVL_ABS, m.LVL_VALUE from msr_curve c "
		L"inner join msr_plan_elem p on  p.ID=c.MSR_PLAN_ELEM_ID "
		L"inner join msr_trail t on t.ID=p.MSR_TRAIL_ID   "
		L"inner join MSR_CURVE_MARK m on c.ID=m.MSR_CURVE_ID "
		L"where t.ADM_WAY_ID=(select w.id from adm_way w inner join adm_direction d on d.id=w.adm_direction_id where d.code='%d' and w.code='%s') and "
		L"p.START_ABS>%f and p.END_ABS<%f and not exists  "
		L"(select t1.DATE_TRAIL, p1.START_ABS, p1.END_ABS from msr_curve c1 "
		L"inner join msr_plan_elem p1 on  p1.ID=c1.MSR_PLAN_ELEM_ID "
		L"inner join msr_trail t1 on t1.ID=p1.MSR_TRAIL_ID where not (p1.START_ABS > p.END_ABS or p.START_ABS > p1.END_ABS ) and t1.DATE_TRAIL > t.DATE_TRAIL  and p1.START_ABS>%f and p1.START_ABS<%f) "
		L"order by m.CRV_ABS") % GetDir() % string_to_wstring(GetWay()) % abs_start % abs_end % abs_start % abs_end);
	VIBDataHandlerParameter curves_handler;
	in_database->ExecuteStatement(transaction, wstring_to_string(curves_query).c_str(), curves_handler);
	if (curves_handler.GetRowCount() > 0)
	{
		GetCurves(in_mark_type).Clear();
		change_calculator<int64_t> id_change;
		VCurve curve;
		for (size_t i=0; i<curves_handler.GetRowCount(); ++i)
		{
			int64_t id = curves_handler.GetParameter(i, 0).GetInteger();
			double coord_plan = curves_handler.GetParameter(i, 1).GetInteger();
			double value_plan = curves_handler.GetParameter(i, 2).GetInteger();
			double coord_level = curves_handler.GetParameter(i, 3).GetInteger();
			double value_level = curves_handler.GetParameter(i, 4).GetInteger();
			if (id_change.put(id))
			{
				if (curve.CheckMarks())
				{
					curve.FixCrvArea(*this);	
					curve.SetModel(in_mark_type==PRM_MODEL_CURVES);
					if (in_mark_type==PRM_MODEL_CURVES)
					{
						curve.ResetModelSpeeds(*this);
					}
					GetCurves(in_mark_type).AddItem(curve);
					SetChanged(in_mark_type);
				}
				curve = VCurve();
			}
			size_t index_plan=this->GetAbsCoord().FindIndex(coord_plan, false);
			if (index_plan!=BAD_INDEX && AllowMarkHerePlan(index_plan, in_mark_type))
				curve.AddPlanMark(index_plan, value_plan);

			size_t index_level=this->GetAbsCoord().FindIndex(coord_level, false);
			if (index_level!=BAD_INDEX && AllowMarkHereLevel(index_level, in_mark_type))
				curve.AddLevelMark(index_level, value_level);
		}
		//Обновляем отклонения в плане
		UpdatePlanOffset(in_mark_type==PRM_MODEL_CURVES);
		//Обновляем непогашенные ускорения
		if (in_mark_type==PRM_MODEL_CURVES)
			UpdateAnpModel();
		//Обновляем отклонения уровня от разметки
		UpdateDiffLevel(in_mark_type==PRM_MODEL_CURVES);
	}
	else
	{		
		throw VSimpleException(L"В базе отсутствуют измеренные кривые.", 
			boost::str(boost::wformat(L"Направление %d путь %s") % GetDir() % string_to_wstring(GetWay())));
	}
}

/// Оптимизация кривых
void VDataSet::OptimizeCurves(VAUIProgressDialogPtr in_progress, PARAMETER_TYPE in_mark_type)
{	
	VAUIProgressAutoHider hider(in_progress);

	if (in_progress)
	{
		in_progress->Show(1, L"Оптимизация кривых");
		in_progress->Reset(0, GetCurves(in_mark_type).GetCount());
	}

	for (size_t i=0; i<GetCurves(in_mark_type).GetCount(); ++i)
	{										
		if (in_progress)
		{
			std::wstring msg=boost::str(boost::wformat(L"Кривая %d из %d") % (i+1) % GetCurves(in_mark_type).GetCount());			
			in_progress->UpdateMessage(0, msg);
			in_progress->UpdateProgress(0, i);
			if (in_progress->Cancel())
			{
				break;
			}
		}
		GetCurves(in_mark_type).ActivateCurve(i);
		GetCurves(in_mark_type).GetItem(i).OptimizeMarks(*this);				
		GetCurves(in_mark_type).DeactivateCurve();
	}
	//Обновляем отклонения в плане
	UpdatePlanOffset(in_mark_type==PRM_MODEL_CURVES);
	//Обновляем непогашенные ускорения
	if (in_mark_type==PRM_MODEL_CURVES)
		UpdateAnpModel();	
}

/// Возвращает true, если кривая не пересекает метку перелома плана
bool VDataSet::CheckCurveVsLine(size_t in_index, const VCurve& in_curve, PARAMETER_TYPE in_mark_type)
{
	if (in_curve.GetMarkCount()>0)
	for (size_t i=0; i<this->GetLineMarks().GetCount(); ++i)
	{
		size_t start=std::min<size_t>(in_curve.GetPlanMark(0).index, in_index);
		size_t end=std::max<size_t>(in_curve.GetPlanMark(in_curve.GetMarkCount()-1).index, in_index);
		if (this->GetLineMarks().GetItem(i).index >= start && 
			this->GetLineMarks().GetItem(i).index <= end)
			return false;
	}
	return true;
}

/// Возвращает true если допустимо объявить на участке съезд.
bool VDataSet::AllowSidewayHere(size_t in_index_start, size_t in_index_end, PARAMETER_TYPE in_mark_type)
{
	pro::interval<size_t> range(in_index_start, in_index_end);
	//Проверяем пересечение с кривыми
	for (size_t i=0; i<this->GetCurves(in_mark_type).GetCount(); ++i)
	{
		pro::interval<size_t> crv_range(this->GetCurves(in_mark_type).GetItem(i).GetPlanMark(0).index,
			                      this->GetCurves(in_mark_type).GetItem(i).GetPlanMark(
			                      this->GetCurves(in_mark_type).GetItem(i).GetMarkCount()-1).index);
		pro::interval<size_t> lvl_range(this->GetCurves(in_mark_type).GetItem(i).GetLevelMark(0).index,
			                      this->GetCurves(in_mark_type).GetItem(i).GetLevelMark(
			                      this->GetCurves(in_mark_type).GetItem(i).GetLevelMarkCount()-1).index);			                      
		if (crv_range.intersect(range) || lvl_range.intersect(range))
			return false;	
	}
	//Проверяем пересечение с меткой перелома плана
	for (size_t i=0; i<this->GetLineMarks().GetCount(); ++i)
	{
		if (range.contains(GetLineMarks().GetItem(i).index))
		{
			return false;
		}
	}
	return true;
}

/// Возвращает true, если в кадре можно ставить метку
bool VDataSet::AllowMarkHerePlan(size_t in_index, PARAMETER_TYPE in_mark_type)
{
	//Проверяем пересечение с кривыми
	for (size_t i=0; i<this->GetCurves(in_mark_type).GetCount(); ++i)
	{
		if (i!=this->GetCurves(in_mark_type).GetActiveCurve())
		{
			size_t start=this->GetCurves(in_mark_type).GetItem(i).GetPlanMark(0).index;
			size_t end=this->GetCurves(in_mark_type).GetItem(i).GetPlanMark(this->GetCurves(in_mark_type).GetItem(i).GetMarkCount()-1).index;
			if (start<in_index && in_index<end)
				return false;
		}
	}
	//Проверяем пересечение с боковыми путями	
	for (size_t j=0; j<this->GetSideways().GetCount(); ++j)
	{
		if (this->GetSideways().GetItem(j).GetStartNdx() < in_index &&
			this->GetSideways().GetItem(j).GetEndNdx() > in_index)
		{
			return false;
		}
	}
	//Проверяем пересечение активной кривой с соседнимми
	size_t active_index=this->GetCurves(in_mark_type).GetActiveCurve();
	if (active_index!=BAD_INDEX)
	{
		VCurve & active_curve=this->GetCurves(in_mark_type).GetItem(active_index);
		if (in_index < active_curve.GetPlanLeftMargin(*this) || in_index > active_curve.GetPlanRightMargin(*this))
			return false;
	}
	//Проверяем пересечение активной кривой с меткой перелома плана
	if (active_index!=BAD_INDEX)
	{
		VCurve & active_curve=this->GetCurves(in_mark_type).GetItem(active_index);
		return CheckCurveVsLine(in_index, active_curve, in_mark_type);
	}
	
	return true;
}

/// Возвращает true, если в кадре можно ставить метку
bool VDataSet::AllowMarkHereLevel(size_t in_index, PARAMETER_TYPE in_mark_type)
{
	//Проверяем пересечение с кривыми
	for (size_t i=0; i<this->GetCurves(in_mark_type).GetCount(); ++i)
	{
		if (i!=this->GetCurves(in_mark_type).GetActiveCurve())
		{
			size_t start=this->GetCurves(in_mark_type).GetItem(i).GetLevelMark(0).index;
			size_t end=this->GetCurves(in_mark_type).GetItem(i).GetLevelMark(this->GetCurves(in_mark_type).GetItem(i).GetMarkCount()-1).index;
			if (start<in_index && in_index<end)
				return false;
		}
	}
	//Проверяем пересечение с боковыми путями	
	for (size_t j=0; j<this->GetSideways().GetCount(); ++j)
	{
		if (this->GetSideways().GetItem(j).GetStartNdx() < in_index &&
			this->GetSideways().GetItem(j).GetEndNdx() > in_index)
		{
			return false;
		}
	}
	//Проверяем пересечение активной кривой с соседнимми
	size_t active_index=this->GetCurves(in_mark_type).GetActiveCurve();
	if (active_index!=BAD_INDEX)
	{
		VCurve & active_curve=this->GetCurves(in_mark_type).GetItem(active_index);
		if (in_index < active_curve.GetLevelLeftMargin(*this) || in_index > active_curve.GetLevelRightMargin(*this))
			return false;
	}
	return true;
}

void VDataSet::Autocorrection(VIBDatabase * database, POINT_TYPE type)
{
	if (this->Load(PRM_ABS_COORD))	this->SetChanged(PRM_ABS_COORD);
	else return;
	Load(PRM_HISTORY);
	coordPoint.CalcLine(type);
	line_t result = coordPoint.GetLine(type);
	
	///FixMe!!! ввести настройку в OPTIONS по аналогии с CorrectCoord
	if(OPTIONS.GetOption(L"PLAN_MARKS_COORD_CORRECTION").Compare(OPTION_YES))
	{
		if (this->Load(PRM_CURVE_MARKS))	this->SetChanged(PRM_CURVE_MARKS);
		if (this->Load(PRM_MODEL_CURVES))	this->SetChanged(PRM_MODEL_CURVES);
	}
	if(OPTIONS.GetOption(L"PROFILE_MARKS_COORD_CORRECTION").Compare(OPTION_YES))
		if (this->Load(PRM_PROF_MARKS))		this->SetChanged(PRM_PROF_MARKS);
	
	this->absCoord.Correct(result);
	VOperationRecord record;
	char descr[256];
	sprintf(descr,"Линейная коррекция координаты: a=%.1e, b=%.1f м", result.k(), result.b());
	record.SetTextDescription(descr,256);
	this->GetHistory().Add(record);
	this->SetChanged(PRM_HISTORY);
}

double VDataSet::CalcDelta(VDataSet & dataset, PARAMETER_TYPE param)
{
	double delta;
	switch(param)
	{
	case PRM_SPALA_MARKER:
		delta = GetCoordPoint().CalcDelta(POINT_SPALA_MARKER, dataset.GetCoordPoint());
		break;
	case PRM_OP_MARKS:
		delta = GetCoordPoint().CalcDelta(POINT_POLE, dataset.GetCoordPoint());
		break;
	}
	return delta;
}


///Коэффициент корреляции двух параметров (Пирсона)
double VDataSet::GetCorrelation(PARAMETER_TYPE in_param1, PARAMETER_TYPE in_param2)
{
	Load(in_param1);
	Load(in_param2);
	return corr(GetParameter(in_param1), GetParameter(in_param2));		
}

///Коэффициент корреляции знаков двух параметров (Фехнера)
double VDataSet::GetSignCorrelation(PARAMETER_TYPE in_param1, PARAMETER_TYPE in_param2)
{
	Load(in_param1);
	Load(in_param2);	
	return corr_sign(GetParameter(in_param1), GetParameter(in_param2));
}

///Возвращает true, если в кадре можно ставить метку перелома плана
bool VDataSet::AllowMarkHereLine(size_t in_index, double in_min_length)
{
	//Проверяем пересечение с кривыми
	for (size_t i=0; i<this->GetCurves(PRM_CURVE_MARKS).GetCount(); ++i)
	{
		size_t start=this->GetCurves(PRM_CURVE_MARKS).GetItem(i).GetPlanMark(0).index;
		size_t end=this->GetCurves(PRM_CURVE_MARKS).GetItem(i).GetPlanMark(this->GetCurves(PRM_CURVE_MARKS).GetItem(i).GetMarkCount()-1).index;
		if (start<in_index && in_index<end)
			return false;		
	}
	//Проверяем пересечение с паспортными кривыми
	for (size_t i=0; i<this->GetCurves(PRM_MODEL_CURVES).GetCount(); ++i)
	{		
		size_t start=this->GetCurves(PRM_MODEL_CURVES).GetItem(i).GetPlanMark(0).index;
		size_t end=this->GetCurves(PRM_MODEL_CURVES).GetItem(i).GetPlanMark(this->GetCurves(PRM_MODEL_CURVES).GetItem(i).GetMarkCount()-1).index;
		if (start<in_index && in_index<end)
			return false;		
	}
	//Проверяем пересечение с боковыми путями	
	for (size_t j=0; j<this->GetSideways().GetCount(); ++j)
	{
		if (this->GetSideways().GetItem(j).GetStartNdx() < in_index &&
			this->GetSideways().GetItem(j).GetEndNdx() > in_index)
		{
			return false;
		}
	}	
	return true;
}

///Обновляет отклонения уровня от разметки в окрестности точки
void VDataSet::UpdateDiffLevel(bool in_is_model)
{
	if (in_is_model)
	{
		this->diff_level_model.Update(this->modelCurves, this->GetParameter(PRM_LEVEL), this->absCoord, this->GetAprinfo(), this->modelCurves.GetActiveCurve());
	}
	else
	{		
		this->diff_level.Update(this->curves, this->GetParameter(PRM_LEVEL), this->absCoord, this->GetAprinfo(), this->curves.GetActiveCurve());
	}
}

// Функция подвижки магнтиных зазоров
void VDataSet::ShiftMagZazor(int value)
{
	this->magZazorLeft.Shift(value);
	this->magZazorRight.Shift(value);
}

// Сохранить как inm
bool VDataSet::SaveToInm(const std::wstring & fileName, VProgressBox * prog)
{
	std::ofstream inm(fileName.c_str(), std::ios::binary);
	if (!inm.is_open()) return false;

	int dir = this->GetMoveDirection();
	double absStart = dir > 0 ? this->GetAbsCoord().GetItem(0) : this->GetAbsCoord().GetLastItem();
	double wp = 0;

	if (prog)
	{
		prog->SetRange(0, this->GetKadrCount());
	}

	double abs_start = long(this->GetAbsCoord().GetItem(0));
	VWayCoord wc_start = this->GetAprinfo().GetRealCoord(abs_start);
	double abs_shifted = wc_start.km * 1000 + wc_start.m;
	double shift = abs_shifted - abs_start; // abs_shifted  = abs_start + shift

/*	VWayCoord wc_start;
	wc_start.km = abs_start / 1000;
	wc_start.m = abs_start - 1000*wc_start.km;
	double real_abs = this->GetAprinfo().GetAbsCoord(wc_start);
	double shift = real_abs - abs_start;*/

	size_t first_item = dir > 0 ? 0 : this->GetKadrCount() - 1;

	for (size_t kadrNdx = 0; kadrNdx < this->GetKadrCount(); ++kadrNdx)
	{
		if (prog && (kadrNdx % 100) == 0)
		{
			prog->SetPosition(0, kadrNdx);
			prog->Update();
		}

		inm_record kadr = {0};

		size_t i = dir > 0 ? kadrNdx : this->GetKadrCount() - 1 - kadrNdx;

		kadr.absCoord = this->GetAbsCoord().GetItem(i) + shift;
		kadr.wayPassed = fabs(kadr.absCoord - this->GetAbsCoord().GetItem(first_item) - shift);
		kadr.wayStep = kadr.wayPassed - wp;
		wp = kadr.wayPassed;

		unsigned long corrupt = {0};

		if (this->GetParameter(PRM_WIDTH).IsLoaded())	
		{
			kadr.width = this->GetParameter(PRM_WIDTH).GetItem(i);
			if (this->GetParameter(PRM_WIDTH).IsCorrupt(i)) 
			{
				corrupt |= ERROR_WIDTH;
			}
		}
		else kadr.width = 1520.0f;

		if (this->GetParameter(PRM_WIDTH_FRONT).IsLoaded())
		{
			kadr.widthFront = this->GetParameter(PRM_WIDTH_FRONT).GetItem(i);
			if (this->GetParameter(PRM_WIDTH_FRONT).IsCorrupt(i)) 
			{
				corrupt |= ERROR_WIDTH_FRONT;
			}
		}
		else kadr.widthFront = 1520.0f;

		if (this->GetParameter(PRM_WIDTH_BACK).IsLoaded())
		{
			kadr.widthBack = this->GetParameter(PRM_WIDTH_BACK).GetItem(i);
			if (this->GetParameter(PRM_WIDTH_BACK).IsCorrupt(i)) 
			{
				corrupt |= ERROR_WIDTH_BACK;
			}
		}
		else kadr.widthBack = 1520.0f;

		if (this->GetParameter(PRM_LEVEL).IsLoaded())
		{
			kadr.level = this->GetParameter(PRM_LEVEL).GetItem(i);
			if (this->GetParameter(PRM_LEVEL).IsCorrupt(i))
			{
				corrupt |= ERROR_LEVEL;
			}
		}

		if (this->GetParameter(PRM_DIST_SHORT).IsLoaded())
		{
			kadr.distShort = this->GetParameter(PRM_DIST_SHORT).GetItem(i);
			if (this->GetParameter(PRM_DIST_SHORT).IsCorrupt(i))
			{
				corrupt |= ERROR_DIST_SHORT;
			}
		}
		
		if (this->GetParameter(PRM_DIST_LONG).IsLoaded())
		{
			kadr.distLong = this->GetParameter(PRM_DIST_LONG).GetItem(i);
			if (this->GetParameter(PRM_DIST_LONG).IsCorrupt(i))
			{
				corrupt |= ERROR_DIST_LONG;
			}
		}

		if (this->GetParameter(PRM_GRADIENT).IsLoaded())
		{
			kadr.gradient = this->GetParameter(PRM_GRADIENT).GetItem(i);
			if (dir < 0) kadr.gradient *= -1.0f;
			if (this->GetParameter(PRM_GRADIENT).IsCorrupt(i))
			{
				corrupt |= ERROR_GRADIENT;		
			}
		}

		if (this->GetParameter(PRM_PIT_LEFT).IsLoaded())
		{
			kadr.pitLeft = this->GetParameter(PRM_PIT_LEFT).GetItem(i);
			if (this->GetParameter(PRM_PIT_LEFT).IsCorrupt(i)) 
			{
				corrupt |= ERROR_PIT_LEFT;		
			}
		}

		if (this->GetParameter(PRM_PIT_RIGHT).IsLoaded())
		{
			kadr.pitRight = this->GetParameter(PRM_PIT_RIGHT).GetItem(i);
			if (this->GetParameter(PRM_PIT_RIGHT).IsCorrupt(i))
			{
				corrupt |= ERROR_PIT_RIGHT;
			}
		}

		if (this->GetParameter(PRM_RICH_LEFT).IsLoaded())
		{
			kadr.richLeft = this->GetParameter(PRM_RICH_LEFT).GetItem(i);
			if (this->GetParameter(PRM_RICH_LEFT).IsCorrupt(i))
			{
				corrupt |= ERROR_RICH_LEFT;
			}
		}

		if (this->GetParameter(PRM_RICH_RIGHT).IsLoaded())
		{
			kadr.richRight = this->GetParameter(PRM_RICH_RIGHT).GetItem(i);
			if (this->GetParameter(PRM_RICH_RIGHT).IsCorrupt(i))
			{
				corrupt |= ERROR_RICH_RIGHT;
			}
		}

		if (this->GetParameter(PRM_CURV).IsLoaded())
		{
			kadr.curv = this->GetParameter(PRM_CURV).GetItem(i);
			if (this->GetParameter(PRM_CURV).IsCorrupt(i)) 
			{
				corrupt |= ERROR_CURV;
			}
		}

		if (this->GetParameter(PRM_KURS).IsLoaded())
		{
			kadr.kurs = this->GetParameter(PRM_KURS).GetItem(i);
			if (this->GetParameter(PRM_KURS).IsCorrupt(i)) 
			{
				corrupt |= ERROR_KURS;
			}
		}

		if (this->GetParameter(PRM_SPEED).IsLoaded())
			kadr.speed = this->GetParameter(PRM_SPEED).GetItem(i);

		if (this->GetParameter(PRM_TEMP_LEFT).IsLoaded())
			kadr.tempLeft = this->GetParameter(PRM_TEMP_LEFT).GetItem(i);

		if (this->GetParameter(PRM_TEMP_RIGHT).IsLoaded())
			kadr.tempRight = this->GetParameter(PRM_TEMP_RIGHT).GetItem(i);

		if (this->GetParameter(PRM_WEAR_LEFT).IsLoaded())
		{
			kadr.wearLeft = this->GetParameter(PRM_WEAR_LEFT).GetItem(i);
			if (this->GetParameter(PRM_WEAR_LEFT).IsCorrupt(i)) 
			{
				corrupt |= ERROR_WEAR_LEFT;
			}
		}

		if (this->GetParameter(PRM_WEAR_RIGHT).IsLoaded())
		{
			kadr.wearRight = this->GetParameter(PRM_WEAR_RIGHT).GetItem(i);
			if (this->GetParameter(PRM_WEAR_RIGHT).IsCorrupt(i)) 
			{
				corrupt |= ERROR_WEAR_RIGHT;
			}
		}

		if (this->GetParameter(PRM_UNI_WEAR_SENSOR).IsLoaded())
		{
			kadr.wear = this->GetParameter(PRM_UNI_WEAR_SENSOR).GetItem(i);
			if (this->GetParameter(PRM_UNI_WEAR_SENSOR).IsCorrupt(i)) 
			{
				corrupt |= ERROR_WEAR;
			}
		}

		if (this->GetParameter(PRM_USK_VERT).IsLoaded())
			kadr.uskVert = this->GetParameter(PRM_USK_VERT).GetItem(i);

		if (this->GetParameter(PRM_USK_HOR).IsLoaded())
			kadr.uskHor = this->GetParameter(PRM_USK_HOR).GetItem(i);

		if (this->GetParameter(PRM_LATITUDE).IsLoaded())
			kadr.gyroLat = this->GetParameter(PRM_LATITUDE).GetItem(i);

		if (this->GetParameter(PRM_LONGITUDE).IsLoaded())
			kadr.gyroLong = this->GetParameter(PRM_LONGITUDE).GetItem(i);

		if (this->GetDoubleParameter(PRM_GPS_LATITUDE).IsLoaded() &&
			this->GetDoubleParameter(PRM_GPS_LONGITUDE).IsLoaded())
		{
			kadr.gpsLat = this->GetDoubleParameter(PRM_GPS_LATITUDE).GetItem(i);
			kadr.gpsLong = this->GetDoubleParameter(PRM_GPS_LONGITUDE).GetItem(i);
		}

		if (this->GetParameter(PRM_PPR_WIDTH).IsLoaded() &&
			this->GetParameter(PRM_PPR_NAK_LEFT).IsLoaded() &&
			this->GetParameter(PRM_PPR_NAK_RIGHT).IsLoaded() &&
			this->GetParameter(PRM_PPR_POD_LEFT).IsLoaded() &&
			this->GetParameter(PRM_PPR_POD_RIGHT).IsLoaded() &&
			this->GetParameter(PRM_PPR_IZN_LEFT).IsLoaded() &&
			this->GetParameter(PRM_PPR_IZN_RIGHT).IsLoaded())
		{
			kadr.pprWayPassed = kadr.wayPassed;
			kadr.pprShablon = this->GetParameter(PRM_PPR_WIDTH).GetItem(i);
			kadr.pprNakLeft = this->GetParameter(PRM_PPR_NAK_LEFT).GetItem(i) / 100.0;
			kadr.pprNakRight = this->GetParameter(PRM_PPR_NAK_RIGHT).GetItem(i) / 100.0;
			kadr.pprPodLeft = this->GetParameter(PRM_PPR_POD_LEFT).GetItem(i) / 100.0;
			kadr.pprPodRight = this->GetParameter(PRM_PPR_POD_RIGHT).GetItem(i) / 100.0;
			kadr.pprWearLeft = this->GetParameter(PRM_PPR_IZN_LEFT).GetItem(i);
			kadr.pprWearRight = this->GetParameter(PRM_PPR_IZN_RIGHT).GetItem(i);
			
			bool have_add_wears = false;
			
			if (this->GetParameter(PRM_PPR_IZN_EXT_LEFT).IsLoaded() && 
			    this->GetParameter(PRM_PPR_IZN_EXT_RIGHT).IsLoaded() && 
			    this->GetParameter(PRM_PPR_IZN_INT_LEFT).IsLoaded() && 
			    this->GetParameter(PRM_PPR_IZN_INT_RIGHT).IsLoaded())
			{
			    have_add_wears = true;
			    kadr.pprWearExtLeft = this->GetParameter(PRM_PPR_IZN_EXT_LEFT).GetItem(i);
			    kadr.pprWearExtRight = this->GetParameter(PRM_PPR_IZN_EXT_RIGHT).GetItem(i);
			    kadr.pprWearIntLeft = this->GetParameter(PRM_PPR_IZN_INT_LEFT).GetItem(i);
			    kadr.pprWearIntRight = this->GetParameter(PRM_PPR_IZN_INT_RIGHT).GetItem(i);
			}

			if (this->GetParameter(PRM_PPR_WIDTH).IsCorrupt(i) ||
				this->GetParameter(PRM_PPR_NAK_LEFT).IsCorrupt(i) ||
				this->GetParameter(PRM_PPR_NAK_RIGHT).IsCorrupt(i) ||
				this->GetParameter(PRM_PPR_POD_LEFT).IsCorrupt(i) ||
				this->GetParameter(PRM_PPR_POD_RIGHT).IsCorrupt(i) ||
				this->GetParameter(PRM_PPR_IZN_LEFT).IsCorrupt(i) ||
				this->GetParameter(PRM_PPR_IZN_RIGHT).IsCorrupt(i) ||
   				(have_add_wears &&
		            (this->GetParameter(PRM_PPR_IZN_LEFT).IsCorrupt(i) ||
 			         this->GetParameter(PRM_PPR_IZN_RIGHT).IsCorrupt(i) ||
			         this->GetParameter(PRM_PPR_IZN_LEFT).IsCorrupt(i) ||
       				 this->GetParameter(PRM_PPR_IZN_RIGHT).IsCorrupt(i))))
			{
				corrupt |= ERROR_PPR;
			}
		}
		else
		{
			kadr.pprShablon = 1520.0;
		}
		

		if (this->GetZazorLeft().IsLoaded())
		{
			size_t found = this->GetZazorLeft().FindItem(i);
			if (found == BAD_INDEX) kadr.zazorLeft = -1;
			else
			{
				kadr.zazorLeft = this->GetZazorLeft().GetItem(found).value;
				if (this->GetZazorLeft().IsCorrupt(found)) kadr.corrupt |= ERROR_ZAZOR_LEFT;
			}
		}
		else
		{
			kadr.zazorLeft = -1;
		}

		if (this->GetZazorRight().IsLoaded())
		{
			size_t found = this->GetZazorRight().FindItem(i);
			if (found == BAD_INDEX) kadr.zazorRight = -1;
			else 
			{
				kadr.zazorRight = this->GetZazorRight().GetItem(found).value;
				if (this->GetZazorRight().IsCorrupt(found)) kadr.corrupt |= ERROR_ZAZOR_RIGHT;
			}
		}
		else
		{
			kadr.zazorRight = -1;
		}

		if (this->GetMagZazorLeft().IsLoaded())
		{
			size_t found = this->GetMagZazorLeft().FindItem(i);
			if (found == BAD_INDEX) kadr.mzLeft = -1;
			else
			{
				kadr.mzLeft = this->GetMagZazorLeft().GetItem(found).value;			
				if (this->GetMagZazorLeft().IsCorrupt(found)) kadr.corrupt |= ERROR_MAG_ZAZOR_LEFT;
			}
		}
		else
		{
			kadr.mzLeft = -1;
		}

		if (this->GetMagZazorRight().IsLoaded())
		{
			size_t found = this->GetMagZazorRight().FindItem(i);
			if (found == BAD_INDEX) kadr.mzRight = -1;
			else 
			{
				kadr.mzRight = this->GetMagZazorRight().GetItem(found).value;			
				if (this->GetMagZazorRight().IsCorrupt(found)) kadr.corrupt |= ERROR_MAG_ZAZOR_RIGHT;
			}
		}
		else
		{
			kadr.mzRight = -1;
		}

		if (corrupt > 0)
		{
			kadr.corrupt = (unsigned short) (corrupt & 0x0000FFFF);
			kadr.corrupt2 = (unsigned short) ((corrupt & 0xFFFF0000) >> 16);
		}

		if (this->GetUgonLeft().IsLoaded())
		{
			size_t found = this->GetUgonLeft().FindItem(i);
			if (found == BAD_INDEX) kadr.ugonLeft = -1;
			else kadr.ugonLeft = this->GetUgonLeft().GetItem(found).value;
		}

		if (this->GetUgonRight().IsLoaded())
		{
			size_t found = this->GetUgonRight().FindItem(i);
			if (found == BAD_INDEX) kadr.ugonRight = -1;
			else kadr.ugonRight = this->GetUgonRight().GetItem(found).value;
		}

		unsigned short buttons = {0};
		if (this->GetOpMarks().IsLoaded())
		{
			size_t found = this->GetOpMarks().FindItem(i);
			if (found != BAD_INDEX)
			{
				buttons = this->GetOpMarks().GetItem(found).ExportToInm();
			}
		}

		if (this->GetMagZazors().IsLoaded())
		{
			size_t found = this->GetMagZazors().FindItem(i);
			if (found != BAD_INDEX)
			{
				buttons |= this->GetMagZazors().GetItem(found).ExportToInm();
			}
		}

		if (buttons != 0)
		{
			kadr.buttons = (unsigned char) (buttons & 0x00FF);
			kadr.buttons2 = (unsigned char) ((buttons & 0xFF00) / 256);
		}

		if (this->GetSpalaMarker().IsLoaded())
		{
		    size_t found = this->GetSpalaMarker().FindItem(i);
		    if (found != BAD_INDEX)
		    {
		        kadr.spalaId = this->GetSpalaMarker().GetItem(found).value;
		        kadr.brdWayPassed = kadr.wayPassed;
		    }
		}

		inm.write(reinterpret_cast<const char*>(&kadr), sizeof(kadr));
	}

	if (prog)
	{
		prog->SetPosition(0, this->GetKadrCount());
		prog->Update();
	}

	inm.close();
	return true;
}

static void __Import(VRtImportedParameter::Param<float> & param, uint32_t & corrupt, 
					 VDataSet & ds, size_t kadrNdx, PARAMETER_TYPE prm_code, uint32_t error_code)
{
	if (ds.IsLoaded(prm_code))
	{
		param.value = ds.GetParameter(prm_code).GetItem(kadrNdx);
		param.corrupt = ds.GetParameter(prm_code).IsCorrupt(kadrNdx);
	}
	if (error_code > 0 && param.corrupt) corrupt |= error_code;
}

static void __Import(VRtImportedParameter::Param<double> & param, uint32_t & corrupt, 
					 VDataSet & ds, size_t kadrNdx, PARAMETER_TYPE prm_code, uint32_t error_code)
{
	if (ds.IsLoaded(prm_code))
	{
		param.value = ds.GetDoubleParameter(prm_code).GetItem(kadrNdx);
		param.corrupt = ds.GetDoubleParameter(prm_code).IsCorrupt(kadrNdx);
	}
	if (error_code > 0 && param.corrupt) corrupt |= error_code;
}

static void __ImportZaz(VRtImportedParameter::Param<float> & param, uint32_t & corrupt, 
						VDataSet & ds, size_t kadrNdx, PARAMETER_TYPE prm_code, uint32_t error_code)
{
	param.value = -1.0f;
	param.corrupt = false; 

	if (ds.IsLoaded(prm_code))
	{
		for (size_t i = 0; i < ds.GetZazor(prm_code).GetCount(); ++i)
		{
			if (ds.GetZazor(prm_code).GetItem(i).index == kadrNdx)
			{
				param.value = ds.GetZazor(prm_code).GetItem(i).value;
				param.corrupt = ds.GetZazor(prm_code).IsCorrupt(i);
				if (error_code > 0 && param.corrupt) corrupt |= error_code;
				break;
			}
		}
	}
}

// Получить кадр для расшифровки
bool VDataSet::GetRtParameter(VRtParameter & kadr, size_t kadrNdx)
{
	kadr.orig_direction = this->GetMoveDirection();
	kadr.direction = kadr.orig_direction;

	double absStart = kadr.direction > 0 ? this->GetAbsCoord().GetItem(0) : this->GetAbsCoord().GetLastItem();
	kadr.glzAbsCoord = this->GetAbsCoord().GetItem(kadrNdx);
	kadr.absCoord = kadr.glzAbsCoord;
	kadr.wayPassed = fabs(kadr.glzAbsCoord - absStart);
	kadr.speed = this->GetParameter(PRM_SPEED).GetItem(kadrNdx);

	__Import(kadr.pitLeft,    kadr.corrupt, *this, kadrNdx, PRM_PIT_LEFT,      ERROR_PIT_LEFT);
	__Import(kadr.pitRight,   kadr.corrupt, *this, kadrNdx, PRM_PIT_RIGHT,     ERROR_PIT_RIGHT);
	__Import(kadr.richLeft,   kadr.corrupt, *this, kadrNdx, PRM_RICH_LEFT,     ERROR_RICH_LEFT);
	__Import(kadr.richRight,  kadr.corrupt, *this, kadrNdx, PRM_RICH_RIGHT,    ERROR_RICH_RIGHT);
	__Import(kadr.distShort,  kadr.corrupt, *this, kadrNdx, PRM_DIST_SHORT,    ERROR_DIST_SHORT);
	__Import(kadr.distLong,   kadr.corrupt, *this, kadrNdx, PRM_DIST_LONG,     ERROR_DIST_LONG);
	__Import(kadr.width,      kadr.corrupt, *this, kadrNdx, PRM_WIDTH,         ERROR_WIDTH);
	// глазковский объединённый износ пропускаем
	__Import(kadr.widthFront, kadr.corrupt, *this, kadrNdx, PRM_WIDTH_FRONT,   ERROR_WIDTH_FRONT);
	__Import(kadr.widthBack,  kadr.corrupt, *this, kadrNdx, PRM_WIDTH_BACK,    ERROR_WIDTH_BACK);
	__Import(kadr.wearLeft,   kadr.corrupt, *this, kadrNdx, PRM_WEAR_LEFT,     ERROR_WEAR_LEFT);
	__Import(kadr.wearRight,  kadr.corrupt, *this, kadrNdx, PRM_WEAR_RIGHT,    ERROR_WEAR_RIGHT);
	__Import(kadr.tempLeft,   kadr.corrupt, *this, kadrNdx, PRM_TEMP_LEFT,     0);
	__Import(kadr.tempRight,  kadr.corrupt, *this, kadrNdx, PRM_TEMP_RIGHT,    0);
	__Import(kadr.level,      kadr.corrupt, *this, kadrNdx, PRM_LEVEL,         ERROR_LEVEL);
	__Import(kadr.gradient,   kadr.corrupt, *this, kadrNdx, PRM_GRADIENT,      ERROR_GRADIENT);
	if (kadr.direction < 0) kadr.gradient.value *= -1.0f;
	__Import(kadr.uskVert,    kadr.corrupt, *this, kadrNdx, PRM_USK_VERT,      0);
	__Import(kadr.uskHor,     kadr.corrupt, *this, kadrNdx, PRM_USK_HOR,       0);
	__Import(kadr.gyroLat,    kadr.corrupt, *this, kadrNdx, PRM_LATITUDE,      0);
	__Import(kadr.gyroLong,   kadr.corrupt, *this, kadrNdx, PRM_LONGITUDE,     0);
	__Import(kadr.curv,       kadr.corrupt, *this, kadrNdx, PRM_CURV,          ERROR_CURV);
	__Import(kadr.kurs,       kadr.corrupt, *this, kadrNdx, PRM_KURS,          ERROR_KURS);
	// крен и тангаж в c4f отсутствуют
	__Import(kadr.gpsTime,    kadr.corrupt, *this, kadrNdx, PRM_GPS_TIME,      0);
	__Import(kadr.gpsLat,     kadr.corrupt, *this, kadrNdx, PRM_GPS_LATITUDE,  0);
	__Import(kadr.gpsLong,    kadr.corrupt, *this, kadrNdx, PRM_GPS_LONGITUDE, 0);
	__Import(kadr.gpsAlt,     kadr.corrupt, *this, kadrNdx, PRM_GPS_ALTITUDE,  0);

	__Import(kadr.ppr.width,     kadr.corrupt, *this, kadrNdx, PRM_PPR_WIDTH,     ERROR_PPR);
	__Import(kadr.ppr.nakLeft,   kadr.corrupt, *this, kadrNdx, PRM_PPR_NAK_LEFT,  ERROR_PPR);
	__Import(kadr.ppr.nakRight,  kadr.corrupt, *this, kadrNdx, PRM_PPR_NAK_RIGHT, ERROR_PPR);
	__Import(kadr.ppr.podLeft,   kadr.corrupt, *this, kadrNdx, PRM_PPR_POD_LEFT,  ERROR_PPR);
	__Import(kadr.ppr.podRight,  kadr.corrupt, *this, kadrNdx, PRM_PPR_POD_RIGHT, ERROR_PPR);
	__Import(kadr.ppr.wearLeft,  kadr.corrupt, *this, kadrNdx, PRM_PPR_IZN_LEFT,  ERROR_PPR);
	__Import(kadr.ppr.wearRight, kadr.corrupt, *this, kadrNdx, PRM_PPR_IZN_RIGHT, ERROR_PPR);

	__ImportZaz(kadr.zazorLeft,  kadr.corrupt, *this, kadrNdx, PRM_ZAZOR_LEFT,      ERROR_ZAZOR_LEFT);
	__ImportZaz(kadr.zazorRight, kadr.corrupt, *this, kadrNdx, PRM_ZAZOR_RIGHT,     ERROR_ZAZOR_RIGHT);
	__ImportZaz(kadr.mzLeft,     kadr.corrupt, *this, kadrNdx, PRM_MAG_ZAZOR_LEFT,  ERROR_MAG_ZAZOR_LEFT);
	__ImportZaz(kadr.mzRight,    kadr.corrupt, *this, kadrNdx, PRM_MAG_ZAZOR_RIGHT, ERROR_ZAZOR_RIGHT);

    for (size_t i = 0; i < this->GetOpMarks().GetCount(); ++i)
        if (this->GetOpMarks().GetItem(i).index == kadrNdx)
        {
            const VOpMark & mark = this->GetOpMarks().GetItem(i);
            kadr.buttons = mark.ExportToInm();
            kadr.expandedButtons.swoLeft = mark.isOptSwLeft;
            kadr.expandedButtons.swoRight = mark.isOptSwRight;
            kadr.expandedButtons.bridge = mark.isBridge;
            kadr.expandedButtons.radar = mark.isRadar;
            kadr.expandedButtons.station = mark.isStation;
            kadr.expandedButtons.sw = mark.isSwitch;
            kadr.expandedButtons.kms = mark.isPole;
            kadr.expandedButtons.picket = mark.isPicket;
            kadr.expandedButtons.swxLeft = mark.isOptXLeft;
            kadr.expandedButtons.swxRight = mark.isOptXRight;
        }

	kadr.isOk = true;
	return true;
}

VWayCoord VDataSet::GetCoordByDPP(unsigned long in_dpp, float correction) 
{
	if (!this->GetDPP().IsLoaded())
	{
		LOG_WARNING(L"Счетчик ДПП не загружен");
	}

	if (!this->GetAbsCoord().IsLoaded())
	{
		LOG_WARNING(L"Абс.координата не загружена");
	}

	if (!this->GetAprinfo().IsLoaded())
	{
		LOG_WARNING(L"Апринфо не загружено");
	}

	size_t ndx0, ndx1;
	if (this->GetAbsCoord().GetCount() > 1000)
	{
		ndx0 = 250; 
		ndx1 = this->GetAbsCoord().GetCount() - 250;
	}

	else
	{
		ndx0 = this->GetAbsCoord().GetCount() * 0.25;
		ndx1 = this->GetAbsCoord().GetCount() * 0.75;
	}

	point_t p0(this->GetAbsCoord().GetItem(ndx0) + correction, double(this->GetDPP().GetItem(ndx0)));
	point_t p1(this->GetAbsCoord().GetItem(ndx1) + correction, double(this->GetDPP().GetItem(ndx1)));

	line_t dpp2abs(p0, p1);
	double pnt_abscoord = dpp2abs.X(double(in_dpp));
	return aprInfo.GetRealCoord(pnt_abscoord);
}

std::wstring GetPrefix(const VAprinfo & aprInfo, time_t t)
{
	std::wstringstream Prefix;
	if (DETECT_OPTIONS.GetOption(L"APPEND_VPS_TO_FILENAME").Compare(OPTION_YES) && 
	!DETECT_OPTIONS.GetOption(L"VPS").GetValue().empty())
	{
		Prefix << DETECT_OPTIONS.GetOption(L"VPS").GetValue() << L"_";
	}
	Prefix << aprInfo.GetDirCode() << L"_" << string_to_wstring(aprInfo.GetWayCode()) << L"_";
	Prefix << VFileName::GetPostfix(t);
	return Prefix.str();
}
