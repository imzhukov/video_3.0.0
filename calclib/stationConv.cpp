#include "dblib.h"
#include "dbfunc.h"
#include "calclib.h"
#include "c4flib.h"
#include "strlib.h"
#include "inm.h"

void VStationFileDescription::Clear()
{
	this->roadCode.clear();
	this->fileName.Clear();
	this->stationName.clear();
	this->stationCode.clear();
	this->parkName.clear();
	this->wayName.clear();
///	this->inmFile.Clear();
//	this->reverse = false;
//	this->import = false;
}

void VStationFileDescription::ClearINM()
{
	this->inmFile.Clear();
}

bool VStationFileDescription::IsEmpty() const
{
	return roadCode.empty() || this->stationCode.empty() || this->wayName.empty();
}

bool VStationFileDescription::GenerateFileName(const std::wstring & folder)
{
	if (this->IsEmpty()) return false;

	this->fileName.SetFolder(folder);
	
	long t = (long) time(0);
	while (true)
	{
		std::wstringstream fn;
		fn << "St" << this->stationCode << "_";
		fn << this->wayName << "_" << VFileName::GetPostfix(t) << ".s4f";
		this->fileName.SetShortName(fn.str());

		if (!this->fileName.Exists())
		{
			return false;
		}

		++t;
	}
}

std::wstring ImportStationINM(const VStationFileDescription & descr, const std::wstring & vps)
{
	VMemoryMappedFile inm_file;
	if (!inm_file.Open(descr.inmFile.GetPath().c_str()))
	{
		return L"Файл не доступен";
	}

	size_t kadrCount = inm_file.GetFileSize() / sizeof(inm_record);
	if (kadrCount < 2)
	{
		return L"Число кадров в файле меньше двух";
	}
	const inm_record * inm = (const inm_record *) inm_file.MapView(0, 0, inm_file.GetFileSize());

	// средний шаг
	double avgStep = 0;
	for (size_t kadrNdx = 1; kadrNdx < kadrCount; ++kadrNdx)
	{
		avgStep += inm[kadrNdx].absCoord - inm[kadrNdx-1].absCoord;
	}
	avgStep /= kadrCount -1;

	// Список корректных кадров
	std::vector<size_t> kadrList;
	kadrList.reserve(kadrCount);

	int step_sign = 1;
	for (size_t kadrNdx = 0; kadrNdx < kadrCount; ++kadrNdx)
	{
		if (fabs(inm[kadrNdx].absCoord) < 1.0) continue;
		
		if (kadrNdx == 0)
		{
			kadrList.push_back(kadrNdx);
			continue;
		}

		double step = inm[kadrNdx].absCoord - inm[kadrNdx-1].absCoord;
		if (fabs(step) < 0.04) continue;

		if (kadrNdx == 1)
		{
			if (step < 0) step_sign = -1;
			kadrList.push_back(kadrNdx);
			continue;
		}

		if (step * step_sign <= 0) continue;

		kadrList.push_back(kadrNdx);
	}

	// переворачиваем список
	bool reverse = false;

//	if ((avgStep < 0 && !descr.reverse) || (avgStep > 0 && descr.reverse))
	if (avgStep < 0)
	{
		reverse = true;
		for (size_t kadrNdx = 0; kadrNdx < kadrList.size() / 2; ++kadrNdx)
		{
			std::swap(kadrList[kadrNdx], kadrList[kadrList.size() - 1 - kadrNdx]);
		}
	}

	// пересчитываем координату, если надо
	std::vector<double> absCoord;
	for (size_t kadrNdx = 0; kadrNdx < kadrList.size(); ++kadrNdx)
	{
		absCoord.push_back(inm[kadrList[kadrNdx]].absCoord);
	}

	if (absCoord.front() > absCoord.back())
	{
		double shift = inm[kadrList.front()].absCoord + inm[kadrList.back()].absCoord;
		for (size_t kadrNdx = 0; kadrNdx < absCoord.size(); ++kadrNdx)
		{
			absCoord[kadrNdx] *= -1.0;
			absCoord[kadrNdx] += shift;
		}
	}

	VDataSet ds;
	ds.CreateFile(descr.fileName.GetPath().c_str());
	ds.SetDataSource(wstring_to_string(descr.inmFile.GetShortName()));
	ds.SetStationInfo(boost::lexical_cast<uint32_t>(descr.stationCode), wstring_to_string(descr.stationName).c_str(),
		wstring_to_string(descr.parkName).c_str(), wstring_to_string(descr.wayName).c_str());
	ds.SetParamCount(kadrList.size());

	if (!vps.empty()) ds.SetDivision(wstring_to_string(vps));
	
	VFileName::FileInfo fi;
	descr.inmFile.GetFileInfo(&fi);
	ds.SetRailDate(VTime(fi.mtime));
	ds.SetMoveDirection(avgStep > 0 ? 1 : -1);

	/// апринфо
	ds.GetAprinfo().MakeStWayInfo(absCoord.front(), absCoord.back());
	ds.SetChanged(PRM_APRINFO);

	// абс.к-та и копируемые параметры
	ds.GetAbsCoord().Create(DHT_ABS_COORD, kadrList.size(), 0, 0.0);
	ds.GetCorrupt().Create(kadrList.size(), false);
	ds.GetSpeed().Create(DHT_SPEED, kadrList.size(), 0, 0);
	ds.GetParameter(PRM_WIDTH).Create(DHT_WIDTH, kadrList.size(), 0, 1520.0f);
	ds.GetParameter(PRM_WIDTH_FRONT).Create(DHT_WIDTH_FRONT, kadrList.size(), 0, 1520.0f);
	ds.GetParameter(PRM_WIDTH_BACK).Create(DHT_WIDTH_BACK, kadrList.size(), 0, 1520.0f);
	ds.GetParameter(PRM_LEVEL).Create(DHT_LEVEL, kadrList.size(), 0, 0.0f);
	ds.GetParameter(PRM_CURV).Create(DHT_CURV, kadrList.size(), 0, 0.0f);
	ds.GetParameter(PRM_GRADIENT).Create(DHT_GRADIENT, kadrList.size(), 0, 0.0f);
	ds.GetParameter(PRM_KURS).Create(DHT_KURS, kadrList.size(), 0, 0.0f);
	ds.GetParameter(PRM_PIT_LEFT).Create(DHT_PIT_LEFT, kadrList.size(), 0, 0.0f);
	ds.GetParameter(PRM_PIT_RIGHT).Create(DHT_PIT_RIGHT, kadrList.size(), 0, 0.0f);
	ds.GetParameter(PRM_RICH_LEFT).Create(DHT_RICH_LEFT, kadrList.size(), 0, 0.0f);
	ds.GetParameter(PRM_RICH_RIGHT).Create(DHT_RICH_RIGHT, kadrList.size(), 0, 0.0f);
	ds.GetParameter(PRM_WEAR_LEFT).Create(DHT_WEAR_LEFT, kadrList.size(), 0, 0.0f);
	ds.GetParameter(PRM_WEAR_RIGHT).Create(DHT_WEAR_RIGHT, kadrList.size(), 0, 0.0f);
	for (size_t i = 0; i < kadrList.size(); ++i)
	{
		size_t kadrNdx = kadrList[i];
		ds.GetAbsCoord().GetItem(i) = absCoord[i];
		ds.GetCorrupt().SetCorruptValue(i, inm[kadrNdx].corrupt, inm[kadrNdx].corrupt2);
		ds.GetSpeed().GetItem(i) = inm[kadrNdx].speed;
		ds.GetParameter(PRM_WIDTH).GetItem(i) = inm[kadrNdx].width;
		ds.GetParameter(PRM_WIDTH_FRONT).GetItem(i) = inm[kadrNdx].widthFront;
		ds.GetParameter(PRM_WIDTH_BACK).GetItem(i) = inm[kadrNdx].widthBack;
		ds.GetParameter(PRM_LEVEL).GetItem(i) = inm[kadrNdx].level;
		ds.GetParameter(PRM_CURV).GetItem(i) = inm[kadrNdx].curv;

		if (reverse)
		{
			ds.GetParameter(PRM_GRADIENT).GetItem(i) = -1.0 * inm[kadrNdx].gradient;
		}
		else
		{
			ds.GetParameter(PRM_GRADIENT).GetItem(i) = inm[kadrNdx].gradient;
		}

		ds.GetParameter(PRM_KURS).GetItem(i) = inm[kadrNdx].kurs;
		ds.GetParameter(PRM_PIT_LEFT).GetItem(i) = inm[kadrNdx].pitLeft;
		ds.GetParameter(PRM_PIT_RIGHT).GetItem(i) = inm[kadrNdx].pitRight;
		ds.GetParameter(PRM_RICH_LEFT).GetItem(i) = inm[kadrNdx].richLeft;
		ds.GetParameter(PRM_RICH_RIGHT).GetItem(i) = inm[kadrNdx].richRight;
		ds.GetParameter(PRM_WEAR_LEFT).GetItem(i) = inm[kadrNdx].wearLeft;
		ds.GetParameter(PRM_WEAR_RIGHT).GetItem(i) = inm[kadrNdx].wearRight;
	}
	ds.SetChanged(PRM_ABS_COORD);
	ds.SetChanged(PRM_CORRUPT);
	ds.SetChanged(PRM_SPEED);
	ds.SetChanged(PRM_WIDTH);
	ds.SetChanged(PRM_WIDTH_FRONT);
	ds.SetChanged(PRM_WIDTH_BACK);
	ds.SetChanged(PRM_LEVEL);
	ds.SetChanged(PRM_CURV);
	ds.SetChanged(PRM_GRADIENT);
	ds.SetChanged(PRM_KURS);
	ds.SetChanged(PRM_PIT_LEFT);
	ds.SetChanged(PRM_PIT_RIGHT);
	ds.SetChanged(PRM_RICH_LEFT);
	ds.SetChanged(PRM_RICH_RIGHT);
	ds.SetChanged(PRM_WEAR_LEFT);
	ds.SetChanged(PRM_WEAR_RIGHT);

	// расчетные параметры
	ds.GetParameter(PRM_MID_CURV).Create(DHT_MID_CURV, kadrList.size(), 0, 0.0f);
	ds.GetParameter(PRM_MID_LEVEL).Create(DHT_MID_LEVEL, kadrList.size(), 0, 0.0f);
	ds.GetParameter(PRM_MID_RICH_LEFT).Create(DHT_MID_RICH_LEFT, kadrList.size(), 0, 0.0f);
	ds.GetParameter(PRM_MID_RICH_RIGHT).Create(DHT_MID_RICH_RIGHT, kadrList.size(), 0, 0.0f);
	for (size_t i = 0; i < kadrList.size(); ++i)
	{
		ds.GetParameter(PRM_MID_CURV).GetItem(i) = ds.GetParameter(PRM_CURV).GetMO(i, 3.0f, ds.GetAbsCoord());
		ds.GetParameter(PRM_MID_LEVEL).GetItem(i) = ds.GetParameter(PRM_LEVEL).GetMO(i, 3.0f, ds.GetAbsCoord());
		ds.GetParameter(PRM_MID_RICH_LEFT).GetItem(i) = ds.GetParameter(PRM_RICH_LEFT).GetMO(i, 3.0f, ds.GetAbsCoord());
		ds.GetParameter(PRM_MID_RICH_RIGHT).GetItem(i) = ds.GetParameter(PRM_RICH_RIGHT).GetMO(i, 3.0f, ds.GetAbsCoord());
	}
	ds.SetChanged(PRM_MID_CURV);
	ds.SetChanged(PRM_MID_LEVEL);
	ds.SetChanged(PRM_MID_RICH_LEFT);
	ds.SetChanged(PRM_MID_RICH_RIGHT);

	// метки оператора, зазоры и маг.зазоры
	for (size_t i = 0; i < kadrList.size(); ++i)
	{
		size_t kadrNdx = kadrList[i];

		VOpMark opMark;
		if (opMark.Import(inm[kadrNdx].buttons, inm[kadrNdx].buttons2, i))
		{
			ds.GetOpMarks().AddItem(opMark);
		}

		VMagZazor magZazor;
		if (magZazor.Import(inm[kadrNdx].buttons2, i))
		{
			ds.GetMagZazors().AddItem(magZazor);
		}
	}
	if (ds.GetOpMarks().GetCount() > 0)		ds.SetChanged(PRM_OP_MARKS);
	if (ds.GetMagZazors().GetCount() > 0)	ds.SetChanged(PRM_MAG_ZAZOR);

	// зазоры
	VParameter zazLeft, zazRight, mzLeft, mzRight;
	zazLeft.Create(0xFFFFFFFF, kadrList.size(), 0, 0);
	zazRight.Create(0xFFFFFFFF, kadrList.size(), 0, 0);
	mzLeft.Create(0xFFFFFFFF, kadrList.size(), 0, 0);
	mzRight.Create(0xFFFFFFFF, kadrList.size(), 0, 0);
	for (size_t i = 0; i < kadrList.size(); ++i)
	{
		zazLeft.GetItem(i) = inm[kadrList[i]].zazorLeft;
		zazRight.GetItem(i) = inm[kadrList[i]].zazorRight;
		mzLeft.GetItem(i) = inm[kadrList[i]].mzLeft;
		mzRight.GetItem(i) = inm[kadrList[i]].mzRight;
	}

	if (ds.GetZazorLeft().Load(zazLeft, ds.GetCorrupt().GetCorruptChannel(PRM_ZAZOR_LEFT)))
		ds.SetChanged(PRM_ZAZOR_LEFT);

	if (ds.GetZazorRight().Load(zazRight, ds.GetCorrupt().GetCorruptChannel(PRM_ZAZOR_RIGHT)))
		ds.SetChanged(PRM_ZAZOR_RIGHT);

	if (ds.GetMagZazorLeft().Load(mzLeft, ds.GetCorrupt().GetCorruptChannel(PRM_MAG_ZAZOR_LEFT)))
		ds.SetChanged(PRM_MAG_ZAZOR_LEFT);

	if (ds.GetMagZazorRight().Load(mzRight, ds.GetCorrupt().GetCorruptChannel(PRM_MAG_ZAZOR_RIGHT)))
		ds.SetChanged(PRM_MAG_ZAZOR_RIGHT);

	if (!ds.Save())
	{
		return ds.GetErrorMessage();
	}

	return L"";
}
