#include <sstream>

#include "VAprinfo.h"
#include "VHtml.h"

#include "mathlib.h"

// -----------------------------------------------------------------------------
// VAprinfoParameter
// -----------------------------------------------------------------------------

// ������� ������ (�������)
template <class T> std::vector<T> VAprinfoParameter<T>::ExportAreas(double from, double to, 
																	bool (*Fits)(const T &),
																	bool cut_ranges_to_section) const
{
	std::vector<T> result;

	for (size_t i = 0; i < this->GetCount(); ++i)
	{
		if (this->GetItem(i).base >= to || this->GetItem(i).end <= from) continue;
		if (!Fits(this->GetItem(i))) continue;

		result.push_back(this->GetItem(i));

		if (cut_ranges_to_section)
		{
			if (result.back().end > to) result.back().end = to;
			if (result.back().base < from) result.back().base = from;
		}
	}

	return result;
}

// ������� ������ (�����)
template <class T> std::vector<T> VAprinfoParameter<T>::ExportLabels(double from, double to) const
{
	std::vector<T> result;

	for (size_t i = 0; i < this->GetCount(); ++i)
	{
		if (this->GetItem(i).coord >= from && this->GetItem(i).coord <= to)
		{
			result.push_back(this->GetItem(i));
		}
	}

	return result;
}

template <class T> size_t VAprinfoParameter<T>::FindItem(double coord) const
{
	class PredicateFits
	{
		const double coord;

	public:

		PredicateFits(double & c) : coord(c) {}

		bool operator() (const T & area) const
		{
			return this->coord >= area.base && this->coord < area.end;
		}
	};

	std::vector<T>::const_iterator found_item = std::find_if(
		this->data.begin(), this->data.end(), PredicateFits(coord));

	if (found_item == this->data.end()) return BAD_INDEX;
	else return found_item - this->data.begin();
}

template <class T> size_t VAprinfoParameter<T>::FindKm(long km) const
{
	class PredicateFits
	{
		const long km;

	public:

		PredicateFits(long __km) : km(__km) {}

		bool operator() (const T & kmr) const
		{
			return this->km == kmr.km;
		}
	};

	std::vector<VAprinfoKmRecord>::const_iterator found_item = std::find_if(
		this->data.begin(), this->data.end(), PredicateFits(km));

	if (found_item == this->data.end()) return BAD_INDEX;
	else return found_item - this->data.begin();
}

// -----------------------------------------------------------------------------
// VAprinfo: �������� � ��������
// -----------------------------------------------------------------------------

// �������� ������
void VAprinfo::Unload()
{
	this->SetLoaded(false);
	this->message = ERRMSG_DATA_NOT_LOADED;
	this->warning = false;

	this->dirCode = 0;
	this->dirName.clear();
	this->wayName.clear();
	ZeroMemory(&this->section, sizeof(this->section));
	this->fromDB = false;
	this->absCoordBase = 0;
	this->absCoordEnd = 0;

	this->kmList.Unload();
	this->pchList.Unload();
	this->wayCatList.Unload();
	this->pntList.Unload();
	this->bridgeList.Unload();
	this->tonnelList.Unload();
	this->switchList.Unload();

	this->speedList.Unload();
	this->speedTmpList.Unload();
	this->speedProjList.Unload();

	this->nwidthStraightList.Unload();
	this->nlevelStraightList.Unload();
	this->spalaList.Unload();
	this->holeList.Unload();
	this->threadStraightList.Unload();

	this->routeList.Unload();
	this->wobLeftList.Unload();
	this->wobRightList.Unload();
	this->profList.Unload();
	this->coordPointList.Unload();
	this->controlAreaList.Unload();
	this->repairList.Unload();
	this->crossingList.Unload();
	this->chamJointList.Unload();

	this->crvFact.Unload();
	this->crvPassport.Unload();

	this->chrouteList.Unload();
	this->railsType.Unload();
	this->railsBrace.Unload();
}

// ------------------------------------------------------------------
// �����
// ------------------------------------------------------------------

// ��������� ����� �������������� � ������
void VAprinfo::SetChrouteWarningPoint(double coord)
{
	VAprinfoIndexRecord chroute;

	this->chrouteList.Unload();
	if (coord > this->GetAbsCoordBase() && coord < this->GetAbsCoordEnd())
	{
		chroute.base = this->GetAbsCoordBase();
		chroute.end = coord;
		chroute.value = -1;
		this->chrouteList.Add(chroute);

		chroute.base = coord;
		chroute.end = this->GetAbsCoordEnd();
		chroute.value = 1;
		this->chrouteList.Add(chroute);
	}
	else
	{
		chroute.base = this->GetAbsCoordBase();
		chroute.end = this->GetAbsCoordEnd();
		chroute.value = 0;
		this->chrouteList.Add(chroute);
	}
}

// ������ ������� ������
// �� ������ ������� �� ����� warning ����� ������ ��������
long VAprinfo::GetChrouteIndexValue(double coord) const
{
	size_t index = this->chrouteList.FindItem(coord);
	if (index == BAD_INDEX)	return 0;
	else return this->chrouteList.GetItem(index).value;
}

// -----------------------------------------------------------------------------
// VAprinfo: ��������� � ����������
// -----------------------------------------------------------------------------

// ����� ���������
size_t VAprinfo::FindKm(long km) const
{
	return this->kmList.FindKm(km);
}

// ������� ������ � ��������� - ������� ������� �� ��������� �� �����
VAprinfoKmRecord VAprinfo::GetKm(long km) const
{
	size_t kmIndex = this->FindKm(km);
	if (kmIndex < this->kmList.GetCount())
	{
		return this->kmList.GetItem(kmIndex);
	}

	else
	{
		VAprinfoKmRecord kmRecord;
		long kmDir = this->GetKmDirection();

		// ������
		if ((kmDir > 0 && km > this->kmList.GetLastItem().km) ||
			(kmDir < 0 && km < this->kmList.GetItem(0).km))
		{
//			kmRecord.base = this->GetAbsCoordEnd();
			kmRecord.base = this->kmList.GetLastItem().end;
			kmRecord.end = kmRecord.base + 1000;
			kmRecord.km = this->kmList.GetLastItem().km + kmDir;
			kmRecord.len = kmRecord.km * kmDir >= 0 ? 1000 : -1000;

			while (true)
			{
				if (kmRecord.km == km)
				{
					return kmRecord;
				}

				kmRecord.base += 1000;
				kmRecord.end += 1000;
				//kmRecord.end = kmRecord.base + 1000;
				kmRecord.km += kmDir;
				kmRecord.len = kmRecord.km * kmDir >= 0 ? 1000 : -1000;
			}
		}

		// �����
		else
		{
//			kmRecord.end = this->GetAbsCoordBase();
			kmRecord.end = this->kmList.GetItem(0).base;
			kmRecord.base = kmRecord.end - 1000;
			kmRecord.km = this->kmList.GetItem(0).km - kmDir;
			kmRecord.len = kmRecord.km * kmDir >= 0 ? 1000 : -1000;

			while (true)
			{
				if (kmRecord.km == km)
				{
					return kmRecord;
				}

				kmRecord.end -= 1000;
				kmRecord.base -= 1000;
				kmRecord.km -= kmDir;
				kmRecord.len = kmRecord.km * kmDir >= 0 ? 1000 : -1000;
			}
		}
	}
}

// ����������� ��������� ������������ �����
long VAprinfo::GetKmDirection() const
{
	// ������ ������
	if (this->kmList.GetCount() == 0)
	{
		return 0;
	}
	// ����������� �����
	else if (this->kmList.GetLastItem().km > this->kmList.GetItem(0).km)
	{
		return 1;
	}
	// ��������� �����
	else if (this->kmList.GetLastItem().km < this->kmList.GetItem(0).km)
	{
		return -1;
	}
	// ������������ ��������, �������� ������� �� �����
	else if (this->kmList.GetItem(0).km >= 0)
	{
		if (this->kmList.GetItem(0).len > 0)	return 1;
		else									return -1;
	}
	// ������������ ��������, ��������� ������� �� �����
	else if (this->kmList.GetItem(0).km < 0)
	{
		if (this->kmList.GetItem(0).len < 0)	return 1;
		else									return -1;
	}
	// ���� ��������� �����
	else
	{
		return 0;
	}
}

// �������� ������� ��������� �� ������� ����
bool VAprinfo::CheckKm(long km) const
{
	long kmDir = this->GetKmDirection();
	if (kmDir == 0) return false;

	long firstKm = this->kmList.GetItem(0).km;
	long lastKm = this->kmList.GetLastItem().km;
	return km * kmDir >= firstKm * kmDir && km * kmDir <= lastKm * kmDir;
}

// ����� ���������
float VAprinfo::GetKmLength(long km) const
{
	if (this->CheckKm(km))
		return this->kmList.GetItem(this->FindKm(km)).len;
	else 
		return 1000.0f * this->GetKmDirection();
}

/*
void VAprinfo::BuildTestReverseNet()
{
	this->Unload();

	VAprinfoKmRecord km;
	km.len = -1000;

	km.base = 5000;
	km.end = 6000;
	km.km = 17;
	this->kmList.Add(km);

	km.base = 6000;
	km.end = 7000;
	km.km = 16;
	this->kmList.Add(km);

	km.base = 7000;
	km.end = 8000;
	km.km = 15;
	this->kmList.Add(km);

	km.base = 8000;
	km.end = 9000;
	km.km = 14;
	this->kmList.Add(km);

	km.base = 9000;
	km.end = 10000;
	km.km = 13;
	this->kmList.Add(km);

	km.base = 10000;
	km.end = 11000;
	km.km = 12;
	this->kmList.Add(km);

	km.base = 11000;
	km.end = 12000;
	km.km = 11;
	this->kmList.Add(km);

	km.base = 12000;
	km.end = 13000;
	km.km = 10;
	this->kmList.Add(km);

	this->SetDefaults();
}

void VAprinfo::BuildTestDirectNet()
{
	this->Unload();

	VAprinfoKmRecord km;
	km.len = 1000;

	km.base = 5000;
	km.end = 6000;
	km.km = 10;
	this->kmList.Add(km);

	km.base = 6000;
	km.end = 7000;
	km.km = 11;
	this->kmList.Add(km);

	km.base = 7000;
	km.end = 8000;
	km.km = 12;
	this->kmList.Add(km);

	km.base = 8000;
	km.end = 9000;
	km.km = 13;
	this->kmList.Add(km);

	km.base = 9000;
	km.end = 10000;
	km.km = 14;
	this->kmList.Add(km);

	km.base = 10000;
	km.end = 11000;
	km.km = 15;
	this->kmList.Add(km);

	km.base = 11000;
	km.end = 12000;
	km.km = 16;
	this->kmList.Add(km);

	km.base = 12000;
	km.end = 13000;
	km.km = 17;
	this->kmList.Add(km);
}
*/

// �������������� ������� �-�� � ����������
double VAprinfo::GetAbsCoord(const VWayCoord & in_coord) const
{
	return this->GetKm(in_coord.km).GetAbsCoord(in_coord.m);
}

// �������������� ���������� �-�� � �������
VWayCoord VAprinfo::GetRealCoord(double in_abs) const
{
	size_t kmIndex = this->kmList.FindItem(in_abs);
	if (kmIndex < this->kmList.GetCount())
	{
		return this->kmList.GetItem(kmIndex).GetRealCoord(in_abs);
	}

	else
	{				

		VAprinfoKmRecord kmRecord;
		long kmDir = this->GetKmDirection();

        // ������ �� �����
        if (this->kmList.GetCount() == 0)
        {
            VWayCoord wc;
            wc.km = short(in_abs / 1000.0);
            wc.m = in_abs - (wc.km * 1000.0);
            return wc;
        }

		// ������ �� �� �����
		else if (in_abs >= this->GetAbsCoordEnd())
		{
			kmRecord.base = this->GetAbsCoordEnd();
			kmRecord.end = kmRecord.base + 1000;
			kmRecord.km = this->kmList.GetLastItem().km + kmDir;
			kmRecord.len = kmRecord.km * kmDir >= 0 ? 1000 : -1000;

			while (true)
			{
				if (in_abs >= kmRecord.base && in_abs < kmRecord.end)
				{
					return kmRecord.GetRealCoord(in_abs);
				}

				kmRecord.base += 1000;;
				kmRecord.end += 1000;
				kmRecord.km += kmDir;
				kmRecord.len = kmRecord.km * kmDir >= 0 ? 1000 : -1000;
			}
		}

		// ����� �� �� �����
		else
		{
			kmRecord.end = this->GetAbsCoordBase();
			kmRecord.base = kmRecord.end - 1000;
			kmRecord.km = this->kmList.GetItem(0).km - kmDir;
			kmRecord.len = kmRecord.km * kmDir >= 0 ? 1000 : -1000;

			while (true)
			{
				if (in_abs >= kmRecord.base && in_abs < kmRecord.end)
				{
					return kmRecord.GetRealCoord(in_abs);
				}

				kmRecord.end -= 1000;
				kmRecord.base -= 1000;
				kmRecord.km -= kmDir;
				kmRecord.len = kmRecord.km * kmDir >= 0 ? 1000 : -1000;
			}
		}
	}
}

// ������ ������� �� ���������
std::vector<VPicket> VAprinfo::GetPkList(long km) const
{
	return this->GetKm(km).GetPkList();
}

// ����� �� �� ����������
VPicket VAprinfo::GetPk(double coord) const
{
	VPicket empty_pk;

	long km = this->GetRealCoord(coord).km;
	std::vector<VPicket> pks = this->GetPkList(km);
	for (size_t i = 0; i < pks.size(); ++i)
	{
		const VPicket & cur_pk = pks[i];
		if (coord >= cur_pk.absStart && coord < cur_pk.absEnd)
		{
			return cur_pk;
		}
	}

	return empty_pk;
}

// ������ ������� �� �������
std::vector<VPicket> VAprinfo::GetPkList(double from, double to,
										 bool cut_ranges_to_section) const
{
	std::vector<VPicket> pks;
	long kmDir = this->GetKmDirection();

	VWayCoord wayFrom = this->GetRealCoord(from), wayTo = this->GetRealCoord(to);

	long km = wayFrom.km;
	while (true)
	{
		std::vector<VPicket> curpks = this->GetPkList(km);
		for (size_t i = 0; i < curpks.size(); ++i)
		{
			if (!(curpks[i].absEnd <= from || curpks[i].absStart >= to))
			{
				pks.push_back(curpks[i]);
			}
		}

		if (km == wayTo.km) break;
		km += kmDir;
	}

	if (cut_ranges_to_section && !pks.empty())
	{
		if (this->GetAbsCoord(pks.front().section.start) < from)
		{
			pks.front().absStart = from;
			pks.front().section.start = this->GetRealCoord(from);
		}
	
		if (this->GetAbsCoord(pks.back().section.end) > to)
		{
			pks.back().absEnd = to;
			pks.back().section.end = this->GetRealCoord(to);
		}
	}

	return pks;
}

// -----------------------------------------------------------------------------
// ������� ������� 
// -----------------------------------------------------------------------------

// ������ ���������� �� �������
std::vector<VAprinfoKmRecord> VAprinfo::GetKmList(double from, double to, 
												  bool cut_ranges_to_section) const
{
	return this->kmList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoKmRecord>::FitsNonZeroKm, cut_ranges_to_section);
}

// ������ ���������� �� ����
std::vector<VAprinfoKmRecord> VAprinfo::GetKmList() const
{
	return this->kmList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoKmRecord>::FitsNonZeroKm, false);
}

/// ������ ���� �� �� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetStraightNormaWidthList(double from, double to, 
																	 bool cut_ranges_to_section) const
{
	return this->nwidthStraightList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ ���� �� �� ����
std::vector<VAprinfoIndexRecord> VAprinfo::GetStraightNormaWidthList() const
{
	return this->nwidthStraightList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, false);
}

// ������ ���� ���������� �� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetStraightNormaLevelList(double from, double to, 
																	 bool cut_ranges_to_section) const
{
	return this->nlevelStraightList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ ���� ���������� �� ����
std::vector<VAprinfoIndexRecord> VAprinfo::GetStraightNormaLevelList() const
{
	return this->nlevelStraightList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, false);
}

/// ������ ������ ������������ ���� �� ������� (�����)
std::vector<VAprinfoWobRecord> VAprinfo::GetWobLeftList(double from, double to) const
{
	return this->wobLeftList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoWobRecord>::FitsObject, false);
}

// ������ ������ ������������ ���� �� ���� (�����)
std::vector<VAprinfoWobRecord> VAprinfo::GetWobLeftList() const
{
	return this->wobLeftList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoWobRecord>::FitsObject, false);
}

// ������ ������ ������������ ���� �� ������� (������)
std::vector<VAprinfoWobRecord> VAprinfo::GetWobRightList(double from, double to) const
{
	return this->wobRightList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoWobRecord>::FitsObject, false);
}

// ������ ������ ������������ ���� �� ���� (������)
std::vector<VAprinfoWobRecord> VAprinfo::GetWobRightList() const
{
	return this->wobRightList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoWobRecord>::FitsObject, false);
}

/// ������ ������� � ��������� �� �������
std::vector<VAprinfoPntRecord> VAprinfo::GetPntList(double from, double to) const
{
	return this->pntList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoPntRecord>::FitsAlways, false);
}

/// ������ ������� � ��������� �� ����
std::vector<VAprinfoPntRecord> VAprinfo::GetPntList() const
{
	return this->pntList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoPntRecord>::FitsAlways, false);
}

// ������ ������� �� �������
std::vector<VAprinfoPntRecord> VAprinfo::GetStationList(double from, double to) const
{
	return this->pntList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoPntRecord>::FitsObject, false);
}

// ������ ������� �� ����
std::vector<VAprinfoPntRecord> VAprinfo::GetStationList() const
{
	return this->pntList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoPntRecord>::FitsObject, false);
}

// ������ �������� �� �� �������
std::vector<VAprinfoPchRecord> VAprinfo::GetPchList(double from, double to, 
													bool cut_ranges_to_section) const
{
	return this->pchList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoPchRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ �������� �� �� ����
std::vector<VAprinfoPchRecord> VAprinfo::GetPchList() const
{
	return this->pchList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoPchRecord>::FitsAlways, true);
}

// ������ ��������� �� ������� �� �������
std::vector<VAprinfoSpeedRecord> VAprinfo::GetSpeedList(double from, double to, 
														bool cut_ranges_to_section) const
{
	return this->speedList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoSpeedRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ ��������� �� ������� �� ����
std::vector<VAprinfoSpeedRecord> VAprinfo::GetSpeedList() const
{
	return this->speedList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoSpeedRecord>::FitsAlways, false);
}

// ������ ��������� ��������� �� ������� �� �������
std::vector<VAprinfoSpeedRecord> VAprinfo::GetTmpSpeedList(double from, double to, 
														   bool cut_ranges_to_section) const
{
	return this->speedTmpList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoSpeedRecord>::FitsValidSpeed, cut_ranges_to_section);
}

// ������ ��������� ��������� �� ������� �� ����
std::vector<VAprinfoSpeedRecord> VAprinfo::GetTmpSpeedList() const
{
	return this->speedTmpList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoSpeedRecord>::FitsAlways, false);
}

/*
// ������ ��������� ����������� �������� �� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetHighSpeedList(double from, double to, 
															bool cut_ranges_to_section) const
{
	return this->speedHsList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ ��������� ����������� �������� �� ����
std::vector<VAprinfoIndexRecord> VAprinfo::GetHighSpeedList() const
{
	return this->speedHsList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, false);
}

// ������ ��������� ����������� �������� � �������� ������ �� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetHighSpeedBTList(double from, double to, 
															  bool cut_ranges_to_section) const
{
	return this->speedHsBtList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ ��������� ����������� �������� � �������� ������ �� ����
std::vector<VAprinfoIndexRecord> VAprinfo::GetHighSpeedBTList() const
{
	return this->speedHsBtList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, false);
}
*/

// ������ ����� ������� �� �������
std::vector<VAprinfoRailRecord> VAprinfo::GetRailsTypeList(double from, double to, bool cut_ranges_to_section) const
{
	return this->railsType.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoRailRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ ����� ������� �� ��� �������
std::vector<VAprinfoRailRecord> VAprinfo::GetRailsTypeList() const
{
	return this->railsType.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoRailRecord>::FitsAlways, false);

}

// ������ ���������� ������� �� �������
std::vector<VAprinfoBraceRecord> VAprinfo::GetRailsBraceList(double from, double to, bool cut_ranges_to_section) const
{
	return this->railsBrace.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoBraceRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ ���������� ������� �� ��� �������
std::vector<VAprinfoBraceRecord> VAprinfo::GetRailsBraceList() const
{
	return this->railsBrace.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoBraceRecord>::FitsAlways, false);
}

// -----------------------------------------------------------------------------------------------

// ������� ������ ��������� �� ����
bool VAprinfo::ReplaceSpeedList(const std::vector<VAprinfoSpeedRecord> & spd)
{
	// ������ ������� ����� ������� ����
	if (spd.front().base != this->GetAbsCoordBase()) return false;
	// ����� ������� ����� ������� ����
	if (spd.back().end != this->GetAbsCoordEnd()) return false;
	// ����� ������� ������ ������ �������
	for (size_t i = 0; i < spd.size(); ++i)
	{
		if (spd[i].end < spd[i].base) return false;
	}
	// ����� ������� ��������� � ������� ����������
	for (size_t i = 1; i < spd.size(); ++i)
	{
		if (spd[i-1].end != spd[i].base) return false;
	}
	// �� � �������, ��������� ������ ���������
	this->speedList.Unload();
	for (size_t i = 0; i < spd.size(); ++i)
	{
		this->speedList.Add(spd[i]);
	}
	return true;
}

bool VAprinfo::ReplaceWayCatList(const std::vector<VAprinfoIndexRecord> & wc)
{
	// ������ ������� ����� ������� ����
	if (wc.front().base != this->GetAbsCoordBase()) return false;
	// ����� ������� ����� ������� ����
	if (wc.back().end != this->GetAbsCoordEnd()) return false;
	// ����� ������� ������ ������ �������
	for (size_t i = 0; i < wc.size(); ++i)
	{
		if (wc[i].end < wc[i].base) return false;
	}
	// ����� ������� ��������� � ������� ����������
	for (size_t i = 1; i < wc.size(); ++i)
	{
		if (wc[i-1].end != wc[i].base) return false;
	}
	// �� � �������, ��������� ������ ���������
	this->wayCatList.Unload();
	for (size_t i = 0; i < wc.size(); ++i)
	{
		this->wayCatList.Add(wc[i]);
	}
	return true;
}

// ������ ����� ���� �� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetSpalaList(double from, double to, bool cut_ranges_to_section) const
{
	return this->spalaList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ ����� ���� �� ����
std::vector<VAprinfoIndexRecord> VAprinfo::GetSpalaList() const
{
	return this->spalaList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, false);
}

/// ������ ������ �� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetBridgeList(double from, double to, bool include_areas) const
{
	if (include_areas)
		return this->bridgeList.ExportAreas(from, to, 
			VAprinfoParameter<VAprinfoIndexRecord>::FitsIndexAndZone, false);
	else
		return this->bridgeList.ExportAreas(from, to, 
			VAprinfoParameter<VAprinfoIndexRecord>::FitsIndex, false);
}
	
/// ������ ������ �� ���� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetBridgeList(bool include_areas) const
{
	if (include_areas)
		return this->bridgeList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
			VAprinfoParameter<VAprinfoIndexRecord>::FitsIndexAndZone, false);
	else
		return this->bridgeList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
			VAprinfoParameter<VAprinfoIndexRecord>::FitsIndex, false);
}

// ������ �������� �� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetTonnelList(double from, double to, bool include_areas) const
{
	if (include_areas)
		return this->tonnelList.ExportAreas(from, to, 
			VAprinfoParameter<VAprinfoIndexRecord>::FitsIndexAndZone, false);
	else
		return this->tonnelList.ExportAreas(from, to, 
			VAprinfoParameter<VAprinfoIndexRecord>::FitsIndex, false);
}

// ������ �������� �� ���� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetTonnelList(bool include_areas) const
{
	if (include_areas)
		return this->tonnelList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
			VAprinfoParameter<VAprinfoIndexRecord>::FitsIndexAndZone, false);
	else
		return this->tonnelList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
			VAprinfoParameter<VAprinfoIndexRecord>::FitsIndex, false);
}

// ������� ��������� �� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetHoleDiameterList(double from, double to, bool cut_ranges_to_section) const
{
	return this->holeList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, cut_ranges_to_section);
}
	
// ������� ��������� �� ����
std::vector<VAprinfoIndexRecord> VAprinfo::GetHoleDiameterList() const
{
	return this->holeList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, false);
}

// ������ ������� �� �������
std::vector<VAprinfoSwitchRecord> VAprinfo::GetSwitchList(double from, double to) const
{
	return this->switchList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoSwitchRecord>::FitsObject, false);
}

// ������ ������� �� �������
std::vector<VAprinfoSwitchRecord> VAprinfo::GetSwitchList() const
{
	return this->switchList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoSwitchRecord>::FitsObject, false);
}

// ������ ����������� ����� �� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetStraightThreadSideList(double from, double to, bool cut_ranges_to_section) const
{
	return this->threadStraightList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ ����������� ����� �� ���� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetStraightThreadSideList() const
{
	return this->threadStraightList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, false);
}

// ������ ���������� ����� ������� �� �������
std::vector<VAprinfoProfLabel> VAprinfo::GetProfLabels(double from, double to) const
{
	return this->profList.ExportLabels(from, to);
}

// ������ ���������� ����� ������� �� ����
std::vector<VAprinfoProfLabel> VAprinfo::GetProfLabels() const
{
	return this->profList.ExportLabels(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
}

// ��������
std::vector<VAprinfoCrossing> VAprinfo::GetCrossings(double from, double to) const
{
	return this->crossingList.ExportLabels(from, to);
}

std::vector<VAprinfoCrossing> VAprinfo::GetCrossings() const
{
	return this->crossingList.ExportLabels(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
}

// ������ ����������� ��������
std::vector<VAprinfoControlAreaRecord> VAprinfo::GetControlArea() const
{
	return this->controlAreaList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoControlAreaRecord>::FitsObject, false);
}

// ������ �������� �� �������
std::vector<VAprinfoRepairRecord> VAprinfo::GetRepairs(double from, double to, bool cut_ranges_to_section) const
{
	return this->repairList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoRepairRecord>::FitsObject, cut_ranges_to_section);
}

// ������ �������� �� ���� �������
std::vector<VAprinfoRepairRecord> VAprinfo::GetRepairs() const
{
	return this->repairList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoRepairRecord>::FitsObject, false);
}

// ������ �������� �� �������
std::vector<VAprinfoTestrunRecord> VAprinfo::GetTestrunList(double from, double to) const
{
	return this->testrunList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoTestrunRecord>::FitsObject, false);
}

// ������ �������� �� ��� �������
std::vector<VAprinfoTestrunRecord> VAprinfo::GetTestrunList() const
{
	return this->testrunList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoTestrunRecord>::FitsObject, false);
}

// ������ ������������� �������� �� �������
std::vector<VAprinfoObjectRecord> VAprinfo::GetChamJoints(double from, double to) const
{
	return this->chamJointList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoObjectRecord>::FitsObject, false);
}

// ������ ������������� �������� �� ��� �������
std::vector<VAprinfoObjectRecord> VAprinfo::GetChamJoints() const
{
	return this->chamJointList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoObjectRecord>::FitsObject, false);
}

// -----------------------------------------------------------------------------
// �������� ���������� � ����� ��� �� �������
// -----------------------------------------------------------------------------

// ����������� ������������ �������� �� �������
VSpeeds VAprinfo::GetMinSpeeds(double from, double to) const
{
	return VSpeeds(
		this->GetMinPassSpeed(from, to), 
		this->GetMinTruckSpeed(from, to), 
		this->GetMinThirdSpeed(from, to), 
		this->GetMinHighSpeed(from, to), 
		this->GetMinHighSpeedBT(from, to));
}

// ����������� ������������ �������� �� �������
VSpeeds VAprinfo::GetMinTmpSpeeds(double from, double to) const
{
	return VSpeeds(
		this->GetMinPassTmpSpeed(from, to), 
		this->GetMinTruckTmpSpeed(from, to), 
		this->GetMinThirdTmpSpeed(from, to)); 
}

speed_t VAprinfo::GetMinHighSpeed(double from, double to) const
{
	speed_t spd;
	for (size_t i = 0; i < this->speedList.GetCount(); ++i)
	{
		if (this->speedList.GetItem(i).base > to) continue;
		if (this->speedList.GetItem(i).end < from) continue;
		spd = std::min<speed_t>(spd, this->speedList.GetItem(i).spd.GetHighSpeed());
	}
	return spd;
}

speed_t VAprinfo::GetMinHighSpeedBT(double from, double to) const
{
	speed_t spd;
	for (size_t i = 0; i < this->speedList.GetCount(); ++i)
	{
		if (this->speedList.GetItem(i).base > to) continue;
		if (this->speedList.GetItem(i).end < from) continue;
		spd = std::min<speed_t>(spd, this->speedList.GetItem(i).spd.GetHighBTSpeed());
	}
	return spd;
}

// ������������ �������� ��� ����������� ��������
speed_t VAprinfo::GetMaxHighSpeed(double from, double to) const
{
	speed_t spd;
	for (size_t i = 0; i < this->speedList.GetCount(); ++i)
	{
		if (this->speedList.GetItem(i).base > to) continue;
		if (this->speedList.GetItem(i).end < from) continue;
		spd = std::max<speed_t>(spd, this->speedList.GetItem(i).spd.GetHighSpeed());
	}
	return spd;
}

// ������������ �������� ��� ����������� �������� � �������� ������
speed_t VAprinfo::GetMaxHighSpeedBT(double from, double to) const
{
	speed_t spd;
	for (size_t i = 0; i < this->speedList.GetCount(); ++i)
	{
		if (this->speedList.GetItem(i).base > to) continue;
		if (this->speedList.GetItem(i).end < from) continue;
		spd = std::max<speed_t>(spd, this->speedList.GetItem(i).spd.GetHighBTSpeed());
	}
	return spd;
}

// ������������ ������������ �������� �� �������
speed_t VAprinfo::GetMaxPassSpeed(double from, double to) const
{
	speed_t spd;
	for (size_t i = 0; i < this->speedList.GetCount(); ++i)
	{
		if (this->speedList.GetItem(i).base > to) continue;
		if (this->speedList.GetItem(i).end < from) continue;
		spd = std::max<speed_t>(spd, this->speedList.GetItem(i).spd.GetPassSpeed());
	}
	return spd;
}

// ����������� ������������ �������� �� �������
speed_t VAprinfo::GetMinPassSpeed(double from, double to) const
{
	speed_t spd;
	for (size_t i = 0; i < this->speedList.GetCount(); ++i)
	{
		if (this->speedList.GetItem(i).base > to) continue;
		if (this->speedList.GetItem(i).end < from) continue;
		spd = std::min<speed_t>(spd, this->speedList.GetItem(i).spd.GetPassSpeed());
	}
	return spd;
}

// ������������ �������� �������� �� �������
speed_t VAprinfo::GetMaxTruckSpeed(double from, double to) const
{
	speed_t spd;
	for (size_t i = 0; i < this->speedList.GetCount(); ++i)
	{
		if (this->speedList.GetItem(i).base > to) continue;
		if (this->speedList.GetItem(i).end < from) continue;
		spd = std::max<speed_t>(spd, this->speedList.GetItem(i).spd.GetTruckSpeed());
	}
	return spd;
}

// ����������� �������� �������� �� �������
speed_t VAprinfo::GetMinTruckSpeed(double from, double to) const
{
	speed_t spd;
	for (size_t i = 0; i < this->speedList.GetCount(); ++i)
	{
		if (this->speedList.GetItem(i).base > to) continue;
		if (this->speedList.GetItem(i).end < from) continue;
		spd = std::min<speed_t>(spd, this->speedList.GetItem(i).spd.GetTruckSpeed());
	}
	return spd;
}

// ����������� ������ �������� �� �������
speed_t VAprinfo::GetMinThirdSpeed(double from, double to) const
{
	speed_t spd;
	for (size_t i = 0; i < this->speedList.GetCount(); ++i)
	{
		if (this->speedList.GetItem(i).base > to) continue;
		if (this->speedList.GetItem(i).end < from) continue;
		spd = std::min<speed_t>(spd, this->speedList.GetItem(i).spd.GetEmptySpeed());
	}
	return spd;
}

// ������������ ������ �������� �� �������
speed_t VAprinfo::GetMaxThirdSpeed(double from, double to) const
{
	speed_t spd;
	for (size_t i = 0; i < this->speedList.GetCount(); ++i)
	{
		if (this->speedList.GetItem(i).base > to) continue;
		if (this->speedList.GetItem(i).end < from) continue;
		spd = std::max<speed_t>(spd, this->speedList.GetItem(i).spd.GetEmptySpeed());
	}
	return spd;
}

speed_t VAprinfo::GetMinPassTmpSpeed(double from, double to) const
{
	speed_t spd;
	for (size_t i = 0; i < this->speedTmpList.GetCount(); ++i)
	{
		if (this->speedTmpList.GetItem(i).base > to) continue;
		if (this->speedTmpList.GetItem(i).end < from) continue;
		spd = std::min<speed_t>(spd, this->speedTmpList.GetItem(i).spd.GetEmptySpeed());
	}
	return spd;
}

speed_t VAprinfo::GetMinTruckTmpSpeed(double from, double to) const
{
	speed_t spd;
	for (size_t i = 0; i < this->speedTmpList.GetCount(); ++i)
	{
		if (this->speedTmpList.GetItem(i).base > to) continue;
		if (this->speedTmpList.GetItem(i).end < from) continue;
		spd = std::min<speed_t>(spd, this->speedTmpList.GetItem(i).spd.GetEmptySpeed());
	}
	return spd;
}

speed_t VAprinfo::GetMinThirdTmpSpeed(double from, double to) const
{
	speed_t spd;
	for (size_t i = 0; i < this->speedList.GetCount(); ++i)
	{
		if (this->speedList.GetItem(i).base > to) continue;
		if (this->speedList.GetItem(i).end < from) continue;
		spd = std::min<speed_t>(spd, this->speedList.GetItem(i).spd.GetEmptySpeed());
	}
	return spd;
}

// ��������� ���� �� ������� (������� - ����� �������)
long VAprinfo::GetWayCat(double from, double to) const
{
	long result = 7;

	for (size_t i = 0; i < this->wayCatList.GetCount(); ++i)
	{
		if (this->wayCatList.GetItem(i).end < from) continue;
		if (this->wayCatList.GetItem(i).base > to) continue;
		result = std::min<long>(result, this->wayCatList.GetItem(i).value);
	}

	return result;
}

// ����� �� � �����
long VAprinfo::GetStraightNormaWidth(double coord) const
{
	size_t index = this->nwidthStraightList.FindItem(coord);
	if (index == NOT_FOUND)	return 1520;
	else return this->nwidthStraightList.GetItem(index).value;
}

// ����� �� � �����
float VAprinfo::GetPassportCurveNormaWidth(double coord) const
{
	size_t index = this->crvPassport.widthList.FindItem(coord);
	if (index == NOT_FOUND)	return 1520;
	else return this->crvPassport.widthList.GetItem(index).GetWidthCalc(coord);
}

// ����� ������ � �����
float VAprinfo::GetPassportCurveNormaWear(double coord) const
{
	size_t index = this->crvPassport.widthList.FindItem(coord);
	if (index == NOT_FOUND)	return 0;
	else return this->crvPassport.widthList.GetItem(index).GetWearCalc(coord);
}

// ����� ���������� � �����
long VAprinfo::GetStraightNormaLevel(double coord) const
{
	size_t index = this->nlevelStraightList.FindItem(coord);
	if (index == NOT_FOUND)	return 0;
	else return this->nlevelStraightList.GetItem(index).value;
}

// �������� � �����
VSpeeds VAprinfo::GetSpeeds(double coord) const
{
	VSpeeds result;

	size_t index = this->speedList.FindItem(coord);
	if (index == NOT_FOUND)	
		result = VSpeeds(DEFAULT_SPD_PASS, DEFAULT_SPD_TRUCK, DEFAULT_SPD_EMPTY);
	else 
		result = this->speedList.GetItem(index).spd;

	return result;
}

/// ��������� �������� � �����
VSpeeds VAprinfo::GetTmpSpeeds(double coord) const
{
	size_t index = this->speedTmpList.FindItem(coord);
	if (index == NOT_FOUND)	return VSpeeds();
	else return this->speedTmpList.GetItem(index).GetSpeed();
}

/// ���������� ����������� �� ��������� � ������������� ���������
VSpeeds VAprinfo::GetCombinedSpeeds(double coord) const
{
	return VSpeeds::CombineSpeeds(
		this->GetTestrunSpeed(coord),
		this->GetSpeeds(coord), 
		this->GetTmpSpeeds(coord));
}

// ������������ �������� � �����
speed_t VAprinfo::GetPassSpeed(double coord) const
{
	size_t index = this->speedList.FindItem(coord);
	if (index == NOT_FOUND)	return DEFAULT_SPD_PASS;
	else return this->speedList.GetItem(index).spd.GetPassSpeed();
}

// �������� �������� � �����
speed_t VAprinfo::GetTruckSpeed(double coord) const
{
	size_t index = this->speedList.FindItem(coord);
	if (index == NOT_FOUND)	return DEFAULT_SPD_TRUCK;
	else return this->speedList.GetItem(index).spd.GetTruckSpeed();
}

// �������� �������� � �����
speed_t VAprinfo::GetThirdSpeed(double coord) const
{
	size_t index = this->speedList.FindItem(coord);
	if (index == NOT_FOUND)	return DEFAULT_SPD_EMPTY;
	else return this->speedList.GetItem(index).spd.GetEmptySpeed();
}

// �������� ����������� ��������
speed_t VAprinfo::GetHighSpeed(double coord) const
{
	size_t index = this->speedList.FindItem(coord);
	if (index == NOT_FOUND)	return NO_SPEED;
	else return this->speedList.GetItem(index).spd.GetHighSpeed();
}

// �������� ����������� �������� � �������� ������
speed_t VAprinfo::GetHighSpeedBT(double coord) const
{
	size_t index = this->speedList.FindItem(coord);
	if (index == NOT_FOUND) return NO_SPEED;
	else return this->speedList.GetItem(index).spd.GetHighBTSpeed();
}

// �������� ������������ �������� � �����
speed_t VAprinfo::GetTempPassSpeed(double coord) const
{
	size_t index = this->speedTmpList.FindItem(coord);
	if (index == NOT_FOUND)	return NO_SPEED;
	else return this->speedTmpList.GetItem(index).spd.GetPassSpeed();
}

// �������� �������� �������� � �����
speed_t VAprinfo::GetTempTruckSpeed(double coord) const
{
	size_t index = this->speedTmpList.FindItem(coord);
	if (index == NOT_FOUND)	return NO_SPEED;
	else return this->speedTmpList.GetItem(index).spd.GetTruckSpeed();
}

// �������� �������� �������� � �����
speed_t VAprinfo::GetTempThirdSpeed(double coord) const
{
	size_t index = this->speedTmpList.FindItem(coord);
	if (index == NOT_FOUND)	return NO_SPEED;
	else return this->speedTmpList.GetItem(index).spd.GetEmptySpeed();
}

// ��� ���� � �����
long VAprinfo::GetSpala(double coord) const
{
	size_t index = this->spalaList.FindItem(coord);
	if (index == NOT_FOUND)	return SPALA_GB;
	else return this->spalaList.GetItem(index).value;
}

// ������� ��������� � �����
long VAprinfo::GetHoleDiameter(double coord) const
{
	size_t index = this->holeList.FindItem(coord);
	if (index == NOT_FOUND)	return 36;
	else return this->holeList.GetItem(index).value;
}

// ��������� ���� � �����
long VAprinfo::GetWayCat(double coord) const
{
	size_t index = this->wayCatList.FindItem(coord);
	if (index == NOT_FOUND)	return 1;
	else return this->wayCatList.GetItem(index).value;
}

// ��������� �� ����� (����/���.����/�� ����)
long VAprinfo::GetBridge(double coord) const
{
	size_t index = this->bridgeList.FindItem(coord);
	if (index == NOT_FOUND)	return SZO_NONE;
	else return this->bridgeList.GetItem(index).value;
}

// ����� �����
double VAprinfo::GetBridgeLength(double coord) const
{
	size_t index = this->bridgeList.FindItem(coord);
	if (index == NOT_FOUND)	return 0;
	else return this->bridgeList.GetItem(index).GetLen();
}

// ��������� � ������� (����/���.����/�� ����)
long VAprinfo::GetTonnel(double coord) const
{
	size_t index = this->tonnelList.FindItem(coord);
	if (index == NOT_FOUND)	return SZO_NONE;
	else return this->tonnelList.GetItem(index).value;
}

// ����� �������
double VAprinfo::GetTonnelLength(double coord) const
{
	size_t index = this->tonnelList.FindItem(coord);
	if (index == NOT_FOUND)	return 0;
	else return this->tonnelList.GetItem(index).GetLen();
}

// ����� �� � �����
long VAprinfo::GetPchNum(double coord) const
{
	size_t index = this->pchList.FindItem(coord);
	if (index == NOT_FOUND)	return SZO_NONE;
	else return this->pchList.GetItem(index).pchCode;
}

// ��� �� � �����
std::string VAprinfo::GetPchName(double coord) const
{
	size_t index = this->pchList.FindItem(coord);
	if (index == NOT_FOUND)	return "����� �� ������� �����������";
	else return this->pchList.GetItem(index).pchName;
}

// ������� ������� ��� �������
std::string VAprinfo::GetPeregon(double coord, int direction) const
{
	size_t index = this->pntList.FindItem(coord);
	if (index == NOT_FOUND)	return "����� �� ������� �����������";
	else 
	{
		const VAprinfoPntRecord & pnt = this->pntList.GetItem(index);

		if (pnt.isThere)		return "�������: " + pnt.name;
		else if (direction > 0)	return pnt.name;
		else					return pnt.nameReverse;
	}
}

// �������� �������� �� �������
std::string VAprinfo::GetAreaPeregon(double begin, double end) const
{
	if (pntList.GetCount() == 0) return " ";
	else if (pntList.GetCount() == 1) return this->pntList.GetItem(0).name;

	size_t indexBegin = this->pntList.FindItem(begin);
	if (indexBegin == BAD_INDEX) 
	{
		if (begin >= this->GetAbsCoordEnd())	
			indexBegin = this->pntList.GetCount()-1;
		else
			indexBegin = 0;
	}

	size_t indexEnd = this->pntList.FindItem(end);
	if (indexEnd == BAD_INDEX) 
	{
		if (end >= this->GetAbsCoordEnd())	
			indexEnd = this->pntList.GetCount()-1;
		else
			indexEnd = 0;
	}

	std::string result;

	// ����������
	if (begin < end)
	{
		if (pntList.GetItem(indexBegin).isThere)
		{
			result = pntList.GetItem(indexBegin).name;
		}
		else if (indexBegin == 0)	
		{
			result = pntList.GetItem(indexBegin+1).name;
		}
		else					
		{
			result = pntList.GetItem(indexBegin-1).name;
		}

		result += " - ";

		if (pntList.GetItem(indexEnd).isThere)
		{
			result += pntList.GetItem(indexEnd).name;
		}
		else if (indexEnd == pntList.GetCount()-1)
		{
			result += pntList.GetItem(indexEnd-1).name;
		}
		else
		{
			result += pntList.GetItem(indexEnd+1).name;
		}
	}

	// ��������
	else
	{
		if (pntList.GetItem(indexBegin).isThere)
		{
			result = pntList.GetItem(indexBegin).name;
		}
		else if (indexBegin == pntList.GetCount()-1)
		{
			result = pntList.GetItem(indexBegin-1).name;
		}
		else
		{
			result = pntList.GetItem(indexBegin+1).name;
		}

		result += " - ";

		if (pntList.GetItem(indexEnd).isThere)
		{
			result += pntList.GetItem(indexEnd).name;
		}
		else if (indexEnd == 0)	
		{
			result += pntList.GetItem(indexEnd+1).name;
		}
		else					
		{
			result += pntList.GetItem(indexEnd-1).name;
		}
	}

	return result;
}

// ��� ������� (��� ��������� ����� 0)
long VAprinfo::GetStationCode(double coord) const
{
	size_t index = this->pntList.FindItem(coord);
	if (index == NOT_FOUND)	return 0;
	else return this->pntList.GetItem(index).code;
}

// ��������� �� ����������� �������
bool VAprinfo::IsControlArea(double coord) const
{
	size_t index = this->controlAreaList.FindItem(coord);
	if (index == NOT_FOUND)	return false;
	else return this->controlAreaList.GetItem(index).isThere;
}

// �������� �� �� ������� �.�.
float VAprinfo::GetControlAreaWidth(double coord) const
{
	size_t index = this->controlAreaList.FindItem(coord);

	if (index == NOT_FOUND)	return 0;

	else if (controlAreaList.GetItem(index).isThere) 
		return this->controlAreaList.GetItem(index).mo_width;

	else return 0;
}

// �������� ��� �� �� ������� �.�.
float VAprinfo::GetControlAreaWidthSKO(double coord) const
{
	size_t index = this->controlAreaList.FindItem(coord);

	if (index == NOT_FOUND)	return 0;

	else if (controlAreaList.GetItem(index).isThere) 
		return this->controlAreaList.GetItem(index).sko_width;

	else return 0;
}

// �������� ���������� �� ������� �.�.
float VAprinfo::GetControlAreaLevel(double coord) const
{
	size_t index = this->controlAreaList.FindItem(coord);

	if (index == NOT_FOUND)	return 0;

	else if (controlAreaList.GetItem(index).isThere) 
		return this->controlAreaList.GetItem(index).mo_level;

	else return 0;
}

// �������� ��� ���������� �� ������� �.�.
float VAprinfo::GetControlAreaLevelSKO(double coord) const
{
	size_t index = this->controlAreaList.FindItem(coord);

	if (index == NOT_FOUND)	return 0;

	else if (controlAreaList.GetItem(index).isThere) 
		return this->controlAreaList.GetItem(index).sko_level;

	else return 0;
}

// ��������� �� �������
bool VAprinfo::GetSwitch(double coord) const
{
	size_t index = this->switchList.FindItem(coord);
	if (index == NOT_FOUND) return false;
	else return this->switchList.GetItem(index).isThere;
}

// ���������� �������
SWITCH_DIR VAprinfo::GetSwitchDir(double coord) const
{
	size_t index = this->switchList.FindItem(coord);
	if (index == NOT_FOUND) return SDIR_UNDEF;
	else return this->switchList.GetItem(index).dir;
}

// ����������� ����
long VAprinfo::GetStraightThreadSide(double coord) const
{
	size_t index = this->threadStraightList.FindItem(coord);
	if (index == NOT_FOUND) return SD_RIGHT;
	else return this->threadStraightList.GetItem(index).value;
}

// ������� ����� �� ����� ���� � �����
bool VAprinfo::GetWobLeft(double coord) const
{
	size_t index = this->wobLeftList.FindItem(coord);
	if (index == NOT_FOUND) return false;
	else return this->wobLeftList.GetItem(index).isThere;
}

// ����� ����� �� ����� ���� � �����
std::string VAprinfo::GetWobLeftNum(double coord) const
{
	size_t index = this->wobLeftList.FindItem(coord);
	if (index == NOT_FOUND) return "";
	else return this->wobLeftList.GetItem(index).num;
}

// ������� ����� �� ������ ���� � �����
bool VAprinfo::GetWobRight(double coord) const
{
	size_t index = this->wobRightList.FindItem(coord);
	if (index == NOT_FOUND) return false;
	else return this->wobRightList.GetItem(index).isThere;
}

// ����� ����� �� ������ ���� � �����
std::string VAprinfo::GetWobRightNum(double coord) const
{
	size_t index = this->wobRightList.FindItem(coord);
	if (index == NOT_FOUND) return "";
	else return this->wobRightList.GetItem(index).num;
}

// ������� �������������� ������� � �����
bool VAprinfo::GetChamJoint(double coord) const
{
	size_t index = this->chamJointList.FindItem(coord);
	if (index == NOT_FOUND) return false;
	else return this->chamJointList.GetItem(index).isThere;
}

// ��� ������� � �����
long VAprinfo::GetRailsType(double coord) const
{
	size_t index = this->railsType.FindItem(coord);
	if (index == NOT_FOUND) return 0;
	else return this->railsType.GetItem(index).value;
}

// �������� ���� ������� � �����
std::string VAprinfo::GetRailsTypeDescr(double coord) const
{
	size_t index = this->railsType.FindItem(coord);
	if (index == NOT_FOUND) return "";
	else return this->railsType.GetItem(index).GetDescr();
}

// ��� ���������� ������� � �����
long VAprinfo::GetRailsBrace(double coord) const
{
	size_t index = this->railsBrace.FindItem(coord);
	if (index == NOT_FOUND) return 0;
	else return this->railsBrace.GetItem(index).value;
}

// �������� ���� ���������� ������� � �����
std::string VAprinfo::GetRailsBraceDescr(double coord) const
{
	size_t index = this->railsBrace.FindItem(coord);
	if (index == NOT_FOUND) return "";
	else return this->railsBrace.GetItem(index).GetDescr();
}

// ������� � �����
MACHINE_TYPE VAprinfo::GetTestrunType(double coord) const
{
	size_t index = this->testrunList.FindItem(coord);
	if (index == NOT_FOUND) return MACHINE_NONE;
	else return this->testrunList.GetItem(index).machine;
}

// �������� ������� � �����
VSpeeds VAprinfo::GetTestrunSpeed(double coord) const
{
	size_t index = this->testrunList.FindItem(coord);
	if (index != NOT_FOUND) return this->testrunList.GetItem(index).spd;
	else return VSpeeds();
}

// -----------------------------------------------------------------------------
// ���������� ������������� ������ �����������
// -----------------------------------------------------------------------------
void VAprinfo::SetDefaults()
{
	// ��� ������ ����� ���������� ������ �� �����
	if (this->kmList.GetCount() == 0)
	{
		return;
	}

	// ����� �� �� ���������
	if (this->nwidthStraightList.GetCount() == 0)
	{
		VAprinfoIndexRecord norma;
		norma.value = 1520;
		norma.base = this->GetAbsCoordBase();
		norma.end = this->GetAbsCoordEnd();
		this->nwidthStraightList.Add(norma);
		this->nwidthStraightList.SetDefault(true);
	}

	// ����� ���������� �� ���������
	if (this->nlevelStraightList.GetCount() == 0)
	{
		VAprinfoIndexRecord norma;
		norma.value = 0;
		norma.base = this->GetAbsCoordBase();
		norma.end = this->GetAbsCoordEnd();
		this->nlevelStraightList.Add(norma);
		this->nlevelStraightList.SetDefault(true);
	}

	// ����� ������������ ���� - �����
	if (this->wobLeftList.GetCount() == 0)
	{
		VAprinfoWobRecord wob;
		wob.isThere = false;
		wob.base = this->GetAbsCoordBase();
		wob.end = this->GetAbsCoordEnd();
		this->wobLeftList.Add(wob);
		this->wobLeftList.SetDefault(true);
	}

	// ����� ������������ ���� - ������
	if (this->wobRightList.GetCount() == 0)
	{
		VAprinfoWobRecord wob;
		wob.isThere = false;
		wob.base = this->GetAbsCoordBase();
		wob.end = this->GetAbsCoordEnd();
		this->wobRightList.Add(wob);
		this->wobRightList.SetDefault(true);
	}

	// �������
	if (this->pntList.GetCount() == 0)
	{
		VAprinfoPntRecord pnt;
		pnt.isThere = false;
		pnt.code = 0;
		pnt.base = this->GetAbsCoordBase();
		pnt.end = this->GetAbsCoordEnd();
		pnt.axis = 0.5 * (pnt.base + pnt.end);
		this->pntList.Add(pnt);
		this->pntList.SetDefault(true);
	}

	// ��
	if (this->pchList.GetCount() == 0)
	{
		VAprinfoPchRecord pch;
		pch.pchCode = 0;
		pch.nodCode = 0;
		pch.roadCode = 0;
		pch.base = this->GetAbsCoordBase();
		pch.end = this->GetAbsCoordEnd();
		this->pchList.Add(pch);
		this->pchList.SetDefault(true);
	}

	// �������� �� �������
	if (this->speedList.GetCount() == 0)
	{
		VAprinfoSpeedRecord spd;
		spd.spd = VSpeeds(DEFAULT_SPD_PASS, DEFAULT_SPD_TRUCK, DEFAULT_SPD_EMPTY);
		spd.base = this->GetAbsCoordBase();
		spd.end = this->GetAbsCoordEnd();
		this->speedList.Add(spd);
		this->speedList.SetDefault(true);
	}

	// �������� �����������
	if (this->speedProjList.GetCount() == 0)
	{
		VAprinfoSpeedRecord spd;
		spd.spd = VSpeeds();
		spd.base = this->GetAbsCoordBase();
		spd.end = this->GetAbsCoordEnd();
		this->speedProjList.Add(spd);
		this->speedProjList.SetDefault(true);
	}

	// �������� ���������
	if (this->speedTmpList.GetCount() == 0)
	{
		VAprinfoSpeedRecord spd;
		spd.spd = VSpeeds();
		spd.base = this->GetAbsCoordBase();
		spd.end = this->GetAbsCoordEnd();
		this->speedTmpList.Add(spd);
		this->speedTmpList.SetDefault(true);
	}

	// ���� ����
	if (this->spalaList.GetCount() == 0)
	{
		VAprinfoIndexRecord spala;
		spala.value = SPALA_DEFAULT;
		spala.base = this->GetAbsCoordBase();
		spala.end = this->GetAbsCoordEnd();
		this->spalaList.Add(spala);
		this->spalaList.SetDefault(true);
	}

	// �����
	if (this->bridgeList.GetCount() == 0)
	{
		VAprinfoIndexRecord bridge;
		bridge.value = SZO_NONE;
		bridge.base = this->GetAbsCoordBase();
		bridge.end = this->GetAbsCoordEnd();
		this->bridgeList.Add(bridge);
		this->bridgeList.SetDefault(true);
	}

	// �������
	if (this->tonnelList.GetCount() == 0)
	{
		VAprinfoIndexRecord bridge;
		bridge.value = SZO_NONE;
		bridge.base = this->GetAbsCoordBase();
		bridge.end = this->GetAbsCoordEnd();
		this->tonnelList.Add(bridge);
		this->tonnelList.SetDefault(true);
	}

	// ��������� ����
	if (this->wayCatList.GetCount() == 0)
	{
		for (size_t pchNdx = 0; pchNdx < this->pchList.GetCount(); ++pchNdx)
		{
			VAprinfoIndexRecord wayCat;
			wayCat.base = this->pchList.GetItem(pchNdx).base;
			wayCat.end = this->pchList.GetItem(pchNdx).end;
			long maxSpd = this->GetMaxPassSpeed(wayCat.base, wayCat.end);
			if (maxSpd > 200)		wayCat.value = -1;
			else if (maxSpd > 140)	wayCat.value = 0;
			else if (maxSpd > 120)	wayCat.value = 1;
			else if (maxSpd > 100)	wayCat.value = 2;
			else if (maxSpd > 80)	wayCat.value = 3;
			else if (maxSpd > 60)	wayCat.value = 4;
			else if (maxSpd > 40)	wayCat.value = 5;
			else					wayCat.value = 6;
			this->wayCatList.Add(wayCat);
		}
		this->wayCatList.SetDefault(true);
	}

	// ������� ���������
	if (this->holeList.GetCount() == 0)
	{
		VAprinfoIndexRecord hole;
		hole.base = this->GetAbsCoordBase();
		hole.end = this->GetAbsCoordEnd();
		hole.value = 36;
		this->holeList.Add(hole);
		this->holeList.SetDefault(true);
	}

	// �������
	if (this->switchList.GetCount() == 0)
	{
		VAprinfoSwitchRecord sw;
		sw.base = this->GetAbsCoordBase();
		sw.end = this->GetAbsCoordEnd();
		sw.isThere = false;
		this->switchList.Add(sw);
		this->switchList.SetDefault(true);
	}

	// ��������
	if (this->routeList.GetCount() == 0)
	{
		VAprinfoBasicRecord route;
		route.base = this->GetAbsCoordBase();
		route.end = this->GetAbsCoordEnd();
		this->routeList.Add(route);
		this->routeList.SetDefault(true);
	}

	// ������ - �������
	if (this->crvPassport.curveList.GetCount() == 0)
	{
		this->crvPassport.SetDefaults(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
	}

	// ������ - ����
	if (this->crvFact.curveList.GetCount() == 0)
	{
		this->crvFact.SetDefaults(this->GetAbsCoordBase(), this->GetAbsCoordEnd());
	}

	// ����.���� � ������
	if (this->threadStraightList.GetCount() == 0)
	{
		VAprinfoIndexRecord thread;
		thread.base = this->GetAbsCoordBase();
		thread.end = this->GetAbsCoordEnd();
		thread.value = SD_RIGHT;
		this->threadStraightList.Add(thread);
		this->threadStraightList.SetDefault(true);
	}

	// �����
	if (this->chrouteList.GetCount() == 0)
	{
		VAprinfoIndexRecord chroute;
		chroute.base = this->GetAbsCoordBase();
		chroute.end = this->GetAbsCoordEnd();
		chroute.value = 0;
		this->chrouteList.Add(chroute);
		this->chrouteList.SetDefault(true);
	}

	// ����������� �������
	if (this->controlAreaList.GetCount() == 0)
	{
		VAprinfoControlAreaRecord ctrlArea;
		ctrlArea.base = this->GetAbsCoordBase();
		ctrlArea.end = this->GetAbsCoordEnd();
		this->controlAreaList.Add(ctrlArea);
		this->controlAreaList.SetDefault(true);
	}

	// �������
	if (this->repairList.GetCount() == 0)
	{
		VAprinfoRepairRecord repair;
		repair.base = this->GetAbsCoordBase();
		repair.end = this->GetAbsCoordEnd();
		this->repairList.Add(repair);
		this->repairList.SetDefault(true);
	}

	// ������� ������� ����
	if (this->testrunList.GetCount() == 0)
	{
		VAprinfoTestrunRecord tr;
		tr.base = this->GetAbsCoordBase();
		tr.end = this->GetAbsCoordEnd();
		this->testrunList.Add(tr);
		this->testrunList.SetDefault(true);
	}

	// ��� �������
	if (this->railsType.GetCount() == 0)
	{
		VAprinfoRailRecord rs;
		rs.base = this->GetAbsCoordBase();
		rs.end = this->GetAbsCoordEnd();
		rs.value = RAIL_DEFAULT;
		this->railsType.Add(rs);
		this->railsType.SetDefault(true);
	}

	// ���������� �������
	if (this->railsBrace.GetCount() == 0)
	{
		VAprinfoBraceRecord rb;
		rb.base = this->GetAbsCoordBase();
		rb.end = this->GetAbsCoordEnd();
		rb.value = BRACE_DEFAULT;
		this->railsBrace.Add(rb);
		this->railsBrace.SetDefault(true);
	}

	// ������������� �����
	if (this->chamJointList.GetCount() == 0)
	{
		VAprinfoObjectRecord cj;
		cj.base = this->GetAbsCoordBase();
		cj.end = this->GetAbsCoordEnd();
		this->chamJointList.Add(cj);
		this->chamJointList.SetDefault(true);
	}
}

bool VAprinfo::MakeStWayInfo(double begin, double end, int startKm)
{
	if (end < begin) return false;

	this->absCoordBase=begin;
	this->absCoordEnd=end;

	VAprinfoKmRecord kmRecord;
	while (end > begin)
	{
		kmRecord.km = startKm++;
		kmRecord.base = begin;
		kmRecord.base = begin;

		// ����������� ��
		if (end - begin > 1000.0)	
		{
			kmRecord.end = begin + 1000.0;
			kmRecord.len = 1000.0f;
			begin += 1000;
		}
		// ��������� - �������������
		else
		{
			kmRecord.end = end;
			kmRecord.len = end - begin;
			begin = end;
		}

		this->kmList.Add(kmRecord);
	}

	VAprinfoSpeedRecord speedRecord;
	speedRecord.spd = VSpeeds(40, 40, 40);
	speedRecord.base = this->GetAbsCoordBase();
	speedRecord.end = this->GetAbsCoordEnd();
	this->speedList.Add(speedRecord);

	this->SetDefaults();

	this->isLoaded = true;
	this->message = ERRMSG_OK;
	return true;
}

// -----------------------------------------------------------------------------
// �������� ����� ������
// -----------------------------------------------------------------------------

// �������� ���������� �������
std::wstring VAprinfo::GetSwitchDirDescription(SWITCH_DIR dir)
{
	switch (dir)
	{
	case SDIR_UNDEF:	return L"�� ���������";
	case SDIR_ALONG:	return L"�� ���� ��������";
	case SDIR_COUNTER:	return L"������ ���� ��������";
	default:			return L"�������� ���������� �������";
	}
}

// -----------------------------------------------------------------------------
// ����� � ���������� �������
// -----------------------------------------------------------------------------

std::string VAprinfo::GetReport(bool addHtmlTag, bool fullReport) const
{
	std::stringstream report;

	if (addHtmlTag)
	{
		report << VHtml::GetHeader("���������� �� ��������� ������");
	}

	report << "<h3>���������� �� ��������� ������</h3>" << std::endl;
	report << "<p><strong>�����������:</strong> " << this->dirCode;
	report << " [" << this->dirName << "]<br>" << std::endl;

	report << "<strong>����:</strong> " << this->wayName;
	report << " [" << this->section.start << " - " << this->section.end << "]</p>" << std::endl;

	if (this->kmList.GetCount() > 0)
	{
		report << "<h4>������������� ���������</h4>" << std::endl;
		report << "<table><thead><tr><th>����� ��</th><th>�����</th></tr></thead><tbody>" << std::endl;
		for	(size_t i = 0; i < this->kmList.GetCount(); ++i)
		{
			if (this->kmList.GetItem(i).len != 1000.0f)
			{
				report << "<tr><th>" << this->kmList.GetItem(i).km << "</th><td>";
				report << (long) this->kmList.GetItem(i).len << "</td></tr>" << std::endl;
			}
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->nwidthStraightList.GetDefault()) && this->nwidthStraightList.GetCount() > 0)
	{
		report << "<h4>����� �� � ������</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th><th>��������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->nwidthStraightList.GetCount(); ++i)
		{
			report << "<tr><th>" << this->GetRealCoord(this->nwidthStraightList.GetItem(i).base) << "</th>";
			report << "<th>" << this->GetRealCoord(this->nwidthStraightList.GetItem(i).end) << "</th><td>";
			report << this->nwidthStraightList.GetItem(i).value << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->nlevelStraightList.GetDefault()) && this->nlevelStraightList.GetCount() > 0)
	{
		report << "<h4>����� ���������� � ������</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th><th>��������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->nlevelStraightList.GetCount(); ++i)
		{
			report << "<tr><th>" << this->GetRealCoord(this->nlevelStraightList.GetItem(i).base) << "</th>";
			report << "<th>" << this->GetRealCoord(this->nlevelStraightList.GetItem(i).end) << "</th><td>";
			report << this->nlevelStraightList.GetItem(i).value << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->pntList.GetDefault()) && this->pntList.GetCount() > 0)
	{
		report << "<h4>�������</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>���</th><th>�����</th><th>�������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->pntList.GetCount(); ++i)
		{
			if (!this->pntList.GetItem(i).isThere) continue;

			report << "<tr><th>" << this->GetRealCoord(this->pntList.GetItem(i).base) << "</th>";
			report << "<th>" << this->GetRealCoord(this->pntList.GetItem(i).axis) << "</th>";
			report << "<th>" << this->GetRealCoord(this->pntList.GetItem(i).end) << "</th><td>";
			report << this->pntList.GetItem(i).name;
			if (this->pntList.GetItem(i).code > 0)
			{
				report << " (" << this->pntList.GetItem(i).code << "";
			}
			report << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->pchList.GetDefault()) && this->pchList.GetCount() > 0)
	{
		report << "<h4>���������������� �������</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th><th>��</th><th>���</th><th>������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->pchList.GetCount(); ++i)
		{
			report << "<tr><th>" << this->GetRealCoord(this->pchList.GetItem(i).base) << "</th>";
			report << "<th>" << this->GetRealCoord(this->pchList.GetItem(i).end) << "</th><td>";

			if (this->pchList.GetItem(i).pchName.empty())
				report << this->pchList.GetItem(i).pchCode;
			else
				report << this->pchList.GetItem(i).pchName << " (" << this->pchList.GetItem(i).pchCode << "";

			report << "</td><td>";

			if (this->pchList.GetItem(i).nodCode == 0)
				report << "-";
			else if (this->pchList.GetItem(i).nodName.empty())
				report << this->pchList.GetItem(i).nodCode;
			else
				report << this->pchList.GetItem(i).nodName << " (" << this->pchList.GetItem(i).nodCode << "";

			report << "</td><td>";

			if (this->pchList.GetItem(i).roadCode == 0)
				report << "-";
			else if (this->pchList.GetItem(i).roadName.empty())
				report << this->pchList.GetItem(i).roadCode;
			else
				report << this->pchList.GetItem(i).roadName << " (" << this->pchList.GetItem(i).roadCode << "";

			report << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->speedList.GetDefault()) && this->speedList.GetCount() > 0)
	{
		report << "<h4>�������� �� �������</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th><th>��������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->speedList.GetCount(); ++i)
		{
			report << "<tr><th>" << this->GetRealCoord(this->speedList.GetItem(i).base) << "</th>";
			report << "<th>" << this->GetRealCoord(this->speedList.GetItem(i).end) << "</th>";
			report << "<td>" << this->speedList.GetItem(i).spd.format_short() << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->wayCatList.GetDefault()) && this->wayCatList.GetCount() > 0)
	{
		report << "<h4>��������� ����</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th><th>���������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->wayCatList.GetCount(); ++i)
		{
			report << "<tr><th>" << this->GetRealCoord(this->wayCatList.GetItem(i).base) << "</th>";
			report << "<th>" << this->GetRealCoord(this->wayCatList.GetItem(i).end) << "</th>";
			report << "<td>" << this->wayCatList.GetItem(i).value << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->bridgeList.GetDefault()) && this->bridgeList.GetCount() > 0)
	{
		report << "<h4>�����</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th><th>������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->bridgeList.GetCount(); ++i)
		{
			if (this->bridgeList.GetItem(i).value == SZO_NONE) continue;

			report << "<tr><th>" << this->GetRealCoord(this->bridgeList.GetItem(i).base) << "</th>";
			report << "<th>" << this->GetRealCoord(this->bridgeList.GetItem(i).end) << "</th>";

			report << "<td>";
			if (this->bridgeList.GetItem(i).value == SZO_OBJECT)
				report << "���� (" << (long)(bridgeList.GetItem(i).GetLen()) << " �)";
			else if (this->bridgeList.GetItem(i).value == SZO_SZONE)
				report << "�������� ����";

			report << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->tonnelList.GetDefault()) && this->tonnelList.GetCount() > 0)
	{
		report << "<h4>�������</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th><th>������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->tonnelList.GetCount(); ++i)
		{
			if (this->tonnelList.GetItem(i).value == SZO_NONE) continue;

			report << "<tr><th>" << this->GetRealCoord(this->tonnelList.GetItem(i).base) << "</th>";
			report << "<th>" << this->GetRealCoord(this->tonnelList.GetItem(i).end) << "</th>";

			report << "<td>";
			if (this->tonnelList.GetItem(i).value == SZO_OBJECT)
				report << "������� (" << (long)(tonnelList.GetItem(i).GetLen()) << " �)";
			else if (this->tonnelList.GetItem(i).value == SZO_SZONE)
				report << "�������� ����";

			report << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->profList.GetDefault()) && this->profList.GetCount() > 0)
	{
		report << "<h4>������� ������� �������</h4>" << std::endl;
		report << "<table><thead><tr><th>����������</th><th>�������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->profList.GetCount(); ++i)
		{
			report << "<tr><th>" << this->GetRealCoord(this->profList.GetItem(i).coord) << "</th>";
			report << "<td>" << int(this->profList.GetItem(i).value) << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->crossingList.GetDefault()) && this->crossingList.GetCount() > 0)
	{
		report << "<h4>��������</h4>" << std::endl;
		report << "<table><thead><tr><th>����������</th><th>��������</th><th>�������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->crossingList.GetCount(); ++i)
		{
			report << "<tr><th>" << this->GetRealCoord(this->crossingList.GetItem(i).coord) << "</th>";
			report << "<td>" << this->crossingList.GetItem(i).name << "</td>";
			report << "<td>" << this->crossingList.GetItem(i).side << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->switchList.GetDefault()) && this->switchList.GetCount() > 0)
	{
		report << "<h4>�������</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th><th>�����</th>";
		report << "<th>�������</th><th>����������</th>";
		report << "<th>�-�� �������</th><th>�-�� ����������</th>";
		report << "<th>����� ����������</th><th>�����</th>";
		report << "</tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->switchList.GetCount(); ++i)
		{
			if (this->switchList.GetItem(i).isThere)
			{
				const VAprinfoSwitchRecord & sw = this->switchList.GetItem(i);
				report << "<tr><th>" << this->GetRealCoord(sw.base) << "</th>";
				report << "<th>" << this->GetRealCoord(sw.end) << "</th>";
				report << "<td>" << int(sw.GetLen()) << "</td>";
				report << "<td>" << sw.side > 0 ? "������" : (sw.side < 0 ? "�����" : "�� ����������");
				report << "<td>" << VAprinfo::GetSwitchDirDescription(sw.dir) << "</td>";

				if (sw.o > 0) report << "<td>" << this->GetRealCoord(sw.o) << "</td>";
				else report << "<td>-</td>";

				if (sw.o > 0) report << "<td>" << this->GetRealCoord(sw.x) << "</td>";
				else report << "<td>-</td>";

				report << "<td>" << sw.cross_mark << "</td>";
				report << "<td>" << sw.num << "</td>";
				report << "</tr>" << std::endl;
			}
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->threadStraightList.GetDefault()) && this->threadStraightList.GetCount() > 0)
	{
		report << "<h4>����������� ���� � ������</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th><th>�������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->threadStraightList.GetCount(); ++i)
		{
			const VAprinfoIndexRecord & thread = this->threadStraightList.GetItem(i);
			report << "<tr><th>" << this->GetRealCoord(thread.base) << "</th>";
			report << "<th>" << this->GetRealCoord(thread.end) << "</th>";

			report << "<td>";
			switch(thread.value)
			{
			case SD_RIGHT: report << "������"; break;
			case SD_LEFT: report << "�����"; break;
			default: report << "�� ����������";
			}
			report << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->wobLeftList.GetDefault()) && this->wobLeftList.GetCount() > 0)
	{
		report << "<h4>����� �� ����� ����</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th>";
		report << "<th>����� �����</th><th>����� �����</th></tr>" << std::endl;
		report << "</thead><tbody>" << std::endl;

		for (size_t i = 0; i < this->wobLeftList.GetCount(); ++i)
		{
			const VAprinfoWobRecord & wob = this->wobLeftList.GetItem(i);
			if (!wob.isThere) continue;

			report << "<tr><th>" << this->GetRealCoord(wob.base) << "</th>";
			report << "<th>" << this->GetRealCoord(wob.end) << "</th>";
			report << "<td>" << wob.num << "</td>";
			report << "<td>" << wob.labels.size() << "</td></tr>" << std::endl;
		}

		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->wobRightList.GetDefault()) && this->wobRightList.GetCount() > 0)
	{
		report << "<h4>����� �� ������ ����</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th>";
		report << "<th>����� �����</th><th>����� �����</th></tr>" << std::endl;
		report << "</thead><tbody>" << std::endl;

		for (size_t i = 0; i < this->wobRightList.GetCount(); ++i)
		{
			const VAprinfoWobRecord & wob = this->wobRightList.GetItem(i);
			if (!wob.isThere) continue;

			report << "<tr><th>" << this->GetRealCoord(wob.base) << "</th>";
			report << "<th>" << this->GetRealCoord(wob.end) << "</th>";
			report << "<td>" << wob.num << "</td>";
			report << "<td>" << wob.labels.size() << "</td></tr>" << std::endl;
		}

		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->holeList.GetDefault()) && this->holeList.GetCount() > 0)
	{
		report << "<h4>������� ��������� � �������</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th><th>��������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->holeList.GetCount(); ++i)
		{
			const VAprinfoIndexRecord & idxrec = this->holeList.GetItem(i);
			report << "<tr><th>" << this->GetRealCoord(idxrec.base) << "</th>";
			report << "<th>" << this->GetRealCoord(idxrec.end) << "</th>";
			report << "<td>" << idxrec.value << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}
	
	if ((fullReport || !this->spalaList.GetDefault()) && this->spalaList.GetCount() > 0)
	{
	    report << "<h4>��� ����</h4>" << std::endl;
	    report << "<table><thead><tr><th>������</th><th>�����</th><th>��� ����</th></tr></thead><tbody>" << std::endl;
	    for (size_t i = 0; i < this->spalaList.GetCount(); ++i)
	    {
	        const VAprinfoIndexRecord & idxrec = this->spalaList.GetItem(i);
	        report << "<tr><th>" << this->GetRealCoord(idxrec.base) << "</th>";
	        report << "<th>" << this->GetRealCoord(idxrec.end) << "</th>";
	        report << "<td>";
	        switch(idxrec.value)
	        {
	            case SPALA_GB:      report << "���"; break;
	            case SPALA_GB96:    report << "��� �� 96"; break;
	            case SPALA_WOOD:    report << "������"; break;
	            default:            report << "???"; 
	        }
	        report << "</td></tr>" << std::endl;
	    }
	    report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->crvPassport.curveList.GetDefault()) && 
		this->crvPassport.curveList.GetCount() > 0)
	{
		report << "<h4>��������� ������ - �������</h4>" << std::endl;
		report << "<table><thead><tr><th colspan=2>����</th><th colspan=2>�������</th>";
		report << "<th rowspan=2>����� ��</th><th rowspan=2>����� ������</th></tr>";
		report << "<tr><th>�-��</th><th>��������</th><th>�-��</th><th>��������</th></tr></thead>";
		report << std::endl;

		for (size_t i = 0; i < this->crvPassport.curveList.GetCount(); ++i)
		{
			if (!this->crvPassport.curveList.GetItem(i).isThere) continue;

			report << "<tbody>";
			for (size_t j = 0; j < this->crvPassport.curveList.GetItem(i).planLabel.size(); ++j)
			{
                const VAprinfoCurve & crv = this->crvPassport.curveList.GetItem(i);

				report << "<tr><td>";
				report << this->GetRealCoord(crv.planLabel.at(j).coord);
				report << "</td><td>";
				report << crv.planLabel.at(j).value;
				report << "</td><td>";
				report << this->GetRealCoord(crv.levelLabel.at(j).coord);
				report << "</td><td>";
				report << crv.levelLabel.at(j).value;
				report << "</td>";

                if (j == 0 || j == this->crvPassport.curveList.GetItem(i).planLabel.size() - 1)
                {
                    report << "<td>&nbsp;</td><td>&nbsp;</td>";
                }
                
                else if ( ((j-1) % 2) == 0)
                {
                    size_t radiusNdx = (j-1) / 2;
                    report << "<td rowspan=2>" << crv.radius.at(radiusNdx).width << "</td>";
                    report << "<td rowspan=2>" << crv.radius.at(radiusNdx).wear << "</td>";
                }
				
				report << "</tr>" << std::endl;
			}
			report << "</tbody>" << std::endl;
		}

		report << "</table>" << std::endl;
	}

	if ((fullReport || !this->crvPassport.widthList.GetDefault()) && this->crvPassport.widthList.GetCount() > 0)
	{
		report << "<h4>����� �� � ������ � ������ ���������� ������</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th><th>������ ��. �����</th><th>��1/2</th><th>�����1/2</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->crvPassport.widthList.GetCount(); ++i)
		{
			const VAprinfoWidthRecord & width = this->crvPassport.widthList.GetItem(i);

			report << "<tr><th>" << this->GetRealCoord(width.base) << "</th>";
			report << "<th>" << this->GetRealCoord(width.end) << "</th>";

			report << "<th>";
			switch(width.zlType)
			{
			case ZLINE_UNDEFINED: report << "�� ����������"; break;
			case ZLINE_STRAIGHT: report << "������"; break;
			case ZLINE_OUTER_TRANSITION: report << "������� ����������"; break;
			case ZLINE_RADIUS: 
				report << "�������� [R=" << (int) fabs(width.Radius); 
				report << (width.Radius < 0 ? ", �����]" : ", ������]");
				break;
			case ZLINE_INNER_TRANSITION: report << "���������� ����������"; break;
			}
			report << "</th>";
			
			report << "<td>" << width.width_begin;
			if (width.width_end != width.width_begin)
				report << "/" << width.width_end;
			report << "</td>";
		
			report << "<td>" << width.wear_begin;
			if (width.wear_end != width.wear_begin)
				report << "/" << width.wear_end;
			report << "</td></tr>";
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->crvFact.curveList.GetDefault()) && this->crvFact.curveList.GetCount() > 0)
	{
		report << "<h4>��������� ������ - ����</h4>" << std::endl;
		report << "<table><thead><tr><th colspan=2>����</th><th colspan=2>�������</th></tr>";
		report << "<tr><th>�-��</th><th>��������</th><th>�-��</th><th>��������</th></tr>";
		report << "</thead>" << std::endl;

		for (size_t i = 0; i < this->crvFact.curveList.GetCount(); ++i)
		{
			if (!this->crvFact.curveList.GetItem(i).isThere) continue;

			report << "<tbody>";
			for (size_t j = 0; j < this->crvFact.curveList.GetItem(i).planLabel.size(); ++j)
			{
				report << "<tr><td>";
				report << this->GetRealCoord(this->crvFact.curveList.GetItem(i).planLabel.at(j).coord);
				report << "</td><td>";
				report << this->crvFact.curveList.GetItem(i).planLabel.at(j).value;
				report << "</td><td>";
				report << this->GetRealCoord(this->crvFact.curveList.GetItem(i).levelLabel.at(j).coord);
				report << "</td><td>";
				report << this->crvFact.curveList.GetItem(i).levelLabel.at(j).value;
				report << "</td></tr>";
			}
			report << "</tbody>" << std::endl;
		}

		report << "</table>" << std::endl;
	}

	if ((fullReport || !this->crvFact.widthList.GetDefault()) && this->crvFact.widthList.GetCount() > 0)
	{
		report << "<h4>����� �� � ������ � ������ ����������� ������</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th><th>������ ��. �����</th><th>��1/2</th><th>�����1/2</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->crvFact.widthList.GetCount(); ++i)
		{
			const VAprinfoWidthRecord & width = this->crvFact.widthList.GetItem(i);

			report << "<tr><th>" << this->GetRealCoord(width.base) << "</th>";
			report << "<th>" << this->GetRealCoord(width.end) << "</th>";

			report << "<th>";
			switch(width.zlType)
			{
			case ZLINE_UNDEFINED: report << "�� ����������"; break;
			case ZLINE_STRAIGHT: report << "������"; break;
			case ZLINE_OUTER_TRANSITION: report << "������� ����������"; break;
			case ZLINE_RADIUS: 
				report << "�������� [R=" << (int) fabs(width.Radius); 
				report << (width.Radius < 0 ? ", �����]" : ", ������]");
				break;
			case ZLINE_INNER_TRANSITION: report << "���������� ����������"; break;
			}
			report << "</th>";
			
			report << "<td>" << width.width_begin;
			if (width.width_end != width.width_begin)
				report << "/" << width.width_end;
			report << "</td>";
		
			report << "<td>" << width.wear_begin;
			if (width.wear_end != width.wear_begin)
				report << "/" << width.wear_end;
			report << "</td></tr>";
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->repairList.GetDefault()) && this->repairList.GetCount() > 0)
	{
		report << "<h4>�������</h4>" << std::endl;
		report << "<table><thead><tr><th>���� ������</th><th>���� �����</th><th>������</th><th>�����</th><th>��� �������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->repairList.GetCount(); ++i)
		{
			const VAprinfoRepairRecord & repair = this->repairList.GetItem(i);
			if (!repair.isThere) continue;

			report << "<tr><td>" << repair.start_date.DateToStringLocale() << "</td>";
			report << "<td>" << repair.end_date.DateToStringLocale() << "</td>";
			report << "<td>" << this->GetRealCoord(repair.base) << "</td>";
			report << "<td>" << this->GetRealCoord(repair.end) << "</td>";
			report << "<td>" << repair.type_descr << "</td></tr>" << std::endl;
		}
		report <<"</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->testrunList.GetDefault()) && this->testrunList.GetCount() > 0)
	{
		report << "<h4>������� �������</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th>";
		report << "<th>���� ������</th><th>���� �����</th>";
		report << "<th>��� ������</th><th>��������</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->testrunList.GetCount(); ++i)
		{
			const VAprinfoTestrunRecord & tr = this->testrunList.GetItem(i);
			if (!tr.isThere) continue;

			report << "<tr><td>" << this->GetRealCoord(tr.base) << "</td>";
			report << "<td>" << this->GetRealCoord(tr.end) << "</td>";
			report << "<td>" << tr.date_start.DateToStringLocale() << "</td>";
			report << "<td>" << tr.date_end.DateToStringLocale() << "</td>";

			switch(tr.machine)
			{
			case MACHINE_NONE: report << "<td>&nbsp;</td>"; break;
			case MACHINE_VPO: report << "<td>���</td>"; break;
			case MACHINE_VRO_OR_VPR: report << "<td>��� ��� ���</td>"; break;
			case MACHINE_VPO_PLUS_VPR: report << "<td>���+���</td>"; break;
			case MACHINE_VPO_PLUS_VPR_PLUS_DSP: report << "<td>���+���+���</td>"; break;
			}

			report << "<td>" << tr.spd.format_short() << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}

	if ((fullReport || !this->chamJointList.GetDefault()) && this->chamJointList.GetCount() > 0)
	{
		report << "<h4>������������� �������</h4>" << std::endl;
		report << "<table><thead><tr><th>������</th><th>�����</th></tr></thead><tbody>" << std::endl;
		for (size_t i = 0; i < this->chamJointList.GetCount(); ++i)
		{
			const VAprinfoObjectRecord & cj = this->chamJointList.GetItem(i);
			if (!cj.isThere) continue;

			report << "<tr><td>" << this->GetRealCoord(cj.base) << "</td>";
			report << "<td>" << this->GetRealCoord(cj.end) << "</td></tr>" << std::endl;
		}
		report << "</tbody></table>" << std::endl;
	}

	report << "<p><strong>�������� ��������� ������:</strong> ";
	report << (this->IsFromDB() ? "���� ������" : "����") << "</p>" << std::endl;

	if (addHtmlTag)
	{
		report << VHtml::GetFooter();
	}

	return report.str();
}

double VAprinfo::GetCoordPoint (int type, wxString id)
{
	double apr_abscoord = 0;
	for(size_t i=0; i < coordPointList.GetCount(); i++)
	{
		if(type==coordPointList.GetItem(i).type && id==coordPointList.GetItem(i).id)
		{
			apr_abscoord = coordPointList.GetItem(i).apr_abscoord;
			return apr_abscoord;
		}
	}
	return apr_abscoord;
}

// ��������� ���������� �������
std::vector<VAprinfoIndexRecord> GlueIndexList(
	const std::vector<VAprinfoIndexRecord> & lst)
{
	std::vector<VAprinfoIndexRecord> result;
	for (size_t i = 0; i < lst.size(); ++i)
	{
		if (i > 0 && lst.at(i).value == lst.at(i-1).value)
		{
			result.back().end = lst.at(i).end;
		}
		else
		{
			result.push_back(lst.at(i));
		}
	}
	return result;
}

std::vector<point_t> BuildPWLF(const std::vector<VAprinfoIndexRecord> & nw)
{
	// ��������� ���������� �������
	std::vector<VAprinfoIndexRecord> nw2 = GlueIndexList(nw);

	// ������� �������� ������� �� �� ����� ��������
	std::vector<VAprinfoIndexRecord> nw3;
	for (size_t i = 0; i < nw2.size(); ++i)
	{
		double minlen = 0;
		if (i > 0) 
		{
			minlen += 5 * abs(nw2.at(i).value - nw2.at(i-1).value);
		}
		if (i < nw2.size()-1)
		{
			minlen += 5 * abs(nw2.at(i+1).value - nw2.at(i).value);
		}

		if (i == 0 || i == nw2.size()-1 || nw2.at(i).GetLen() > minlen)
		{
			nw3.push_back(nw2.at(i));
		}
	}

	// ���������� ��������� ����
	for (size_t i = 1; i < nw3.size(); ++i)
	{
		if (nw3.at(i).base > nw3.at(i-1).end)
		{
			double c = 0.5 * (nw3.at(i).base + nw3.at(i-1).end);
			nw3.at(i).base = c;
			nw3.at(i-1).end = c;
		}
	}

	// �������� ��������� ���������� �������
	nw3 = GlueIndexList(nw3);

	// ������ ������ �����
	std::vector<point_t> points;
	if (nw3.empty())
	{
		;
	}
	else
	{
		points.push_back(point_t(nw3.front().base, double(nw3.front().value)));
		for (size_t i = 1; i < nw3.size(); ++i)
		{
			double translen = 5 * abs(nw3[i].value - nw3[i-1].value);
			
			if (nw3.at(i-1).GetLen() > translen)
			{
				points.push_back(point_t(nw3.at(i-1).end - translen, double(nw3.at(i-1).value)));
			}

			if (nw3.at(i).GetLen() > translen)
			{
				points.push_back(point_t(nw3.at(i).base + translen, double(nw.at(i).value)));
			}
		}

		points.push_back(point_t(nw3.back().end, double(nw3.back().value)));
	}

	// voila!
	return points;
}

std::vector<VAprinfoZLRecord> __SmartTransitionLevel(
	const std::vector<VAprinfoIndexRecord> & nl, ZLINE_TYPE zl)
{
	std::vector<VAprinfoZLRecord> levels;
	std::vector<point_t> points = BuildPWLF(nl);
	VAprinfoZLRecord zlRecord;
	zlRecord.zlType = zl;
	for (size_t i = 1; i < points.size(); ++i)
	{
		zlRecord.base = points.at(i-1).x;
		zlRecord.end = points.at(i).x;
		zlRecord.value_base = points.at(i-1).y;
		zlRecord.value_end = points.at(i).y;
		levels.push_back(zlRecord);
	}
	return levels;
}

std::vector<VAprinfoWidthRecord> __SmartTransitionWidth(
	const std::vector<VAprinfoIndexRecord> & nw, ZLINE_TYPE zl)
{
	std::vector<VAprinfoWidthRecord> widths;
	std::vector<point_t> points = BuildPWLF(nw);
	VAprinfoWidthRecord widthRecord;
	widthRecord.Radius = 0;
	widthRecord.zlType = zl;
	widthRecord.wear_begin = 0;
	widthRecord.wear_end = 0;
	for (size_t i = 1; i < points.size(); ++i)
	{
		widthRecord.base = points.at(i-1).x;
		widthRecord.end = points.at(i).x;
		widthRecord.width_begin = points.at(i-1).y;
		widthRecord.width_end = points.at(i).y;
		widths.push_back(widthRecord);
	}
	return widths;
}

// �������� ������ �� ������
void VAprinfo::CurvePack::Unload()
{
	this->curveList.Unload();
	this->zlLevelList.Unload();
	this->zlPlanList.Unload();
	this->zlRichList.Unload();
	this->widthList.Unload();
	this->threadList.Unload();
}

// �������� �� ���������
void VAprinfo::CurvePack::SetDefaults(double absStart, double absEnd)
{
	VAprinfoCurve curve;
	curve.base = absStart;
	curve.end = absEnd;
	curve.isThere = false;
	curve.orientation = 0;
	this->curveList.Add(curve);
	this->curveList.SetDefault(true);
	this->BuildZL(absStart, absEnd);
	this->zlLevelList.SetDefault(true);
	this->zlPlanList.SetDefault(true);
	this->zlRichList.SetDefault(true);
}

// ���������� ������� �����
std::vector<std::wstring> VAprinfo::CurvePack::BuildZL(double absStart, double absEnd)
{
	std::vector<std::wstring> result;

	this->zlLevelList.Unload();
	this->zlPlanList.Unload();
	this->zlRichList.Unload();
	this->widthList.Unload();
	this->threadList.Unload();

	VAprinfoZLRecord zlRecord;
	VAprinfoWidthRecord widthRecord;
	VAprinfoIndexRecord threadRecord;

// ----------------------------------------------------------------
// ����
// ----------------------------------------------------------------

	for (size_t i = 0; i < this->curveList.GetCount(); ++i)
	{
		const VAprinfoCurve & curve = this->curveList.GetItem(i);

		if (!curve.isThere || 
			curve.planLabel.empty() || 
			curve.levelLabel.empty() || 
			curve.radius.empty()) continue;

		for (size_t k = 1; k < curve.planLabel.size(); ++k)
		{
			const VAprinfoCrvLabel & label0 = curve.planLabel[k-1];
			const VAprinfoCrvLabel & label1 = curve.planLabel[k];

			if (label0.coord > absEnd || 
				label1.coord < absStart)
			{
				continue;
			}

			zlRecord.base = label0.coord;
			zlRecord.value_base = label0.value;
			zlRecord.end = label1.coord;
			zlRecord.value_end = label1.value;

			if (zlRecord.base < absStart)
			{
				zlRecord.value_base += zlRecord.Shift(absStart, true);
				zlRecord.base = absStart;
			}

			if (zlRecord.end > absEnd)
			{
				zlRecord.value_end -= zlRecord.Shift(absEnd, false);
				zlRecord.end = absEnd;
			}

			widthRecord.base = zlRecord.base;
			widthRecord.end = zlRecord.end;

			// ������ ����������
			if (k == 1) 
			{
				zlRecord.zlType = ZLINE_OUTER_TRANSITION;
				widthRecord.zlType = ZLINE_OUTER_TRANSITION;
				widthRecord.Radius = 0;
				widthRecord.width_begin = this->aprInfo->GetStraightNormaWidth(widthRecord.base);
				widthRecord.width_end = curve.radius.front().width;
				widthRecord.wear_begin = 0;
				widthRecord.wear_end = curve.radius.front().wear;
			}
			// ��������� ����������
			else if (k == curve.planLabel.size() - 1) 
			{
				zlRecord.zlType = ZLINE_OUTER_TRANSITION;
				widthRecord.zlType = ZLINE_OUTER_TRANSITION;
				widthRecord.Radius = 0;
				widthRecord.width_begin = curve.radius.back().width;
				widthRecord.width_end = this->aprInfo->GetStraightNormaWidth(widthRecord.end);
				widthRecord.wear_begin = curve.radius.back().wear;
				widthRecord.wear_end = 0;
			}
			// ��������
			else if ((k % 2) == 0)
			{
				zlRecord.zlType = ZLINE_RADIUS;
				widthRecord.zlType = ZLINE_RADIUS;
				widthRecord.Radius = curve.radius.at(k/2 - 1).radius * curve.orientation;
				widthRecord.width_begin = curve.radius.at(k/2 - 1).width;
				widthRecord.width_end = widthRecord.width_begin;
				widthRecord.wear_begin = curve.radius.at(k/2 - 1).wear;
				widthRecord.wear_end = widthRecord.wear_begin;
			}
			// ���������� ����������
			else
			{
				zlRecord.zlType = ZLINE_INNER_TRANSITION;
				widthRecord.zlType = ZLINE_INNER_TRANSITION;
				widthRecord.Radius = 0;
				widthRecord.width_begin = curve.radius.at((k-1)/2 - 1).width;
				widthRecord.width_end = curve.radius.at((k-1)/2).width;
				widthRecord.wear_begin = curve.radius.at((k-1)/2 - 1).wear;
				widthRecord.wear_end = curve.radius.at((k-1)/2).wear;
			}

			this->zlPlanList.Add(zlRecord);
			//zlRecord.value_base *= 0.75;
			//zlRecord.value_end *= 0.75;
			zlRecord.value_base=curv_to_rich(zlRecord.value_base);
			zlRecord.value_end=curv_to_rich(zlRecord.value_end);
			this->zlRichList.Add(zlRecord);

			this->widthList.Add(widthRecord);
		}

		// ����������� ���� �� ������
		threadRecord.base = curve.base;
		threadRecord.end = curve.end;
		threadRecord.value = -1 * curve.orientation;
		this->threadList.Add(threadRecord);
	}

	this->zlPlanList.Sort();
	this->zlRichList.Sort();
	this->widthList.Sort();
	this->threadList.Sort();

	size_t planElemCount = this->zlPlanList.GetCount();
	size_t errorCount = 0;

	// ����� ��������� �� ������� ����� 
	for (size_t i = 1; i < this->zlPlanList.GetCount(); ++i)
	{
		VAprinfoZLRecord & prv = this->zlPlanList.GetItem(i-1);
		VAprinfoZLRecord & cur = this->zlPlanList.GetItem(i);

		if (cur.base < prv.end)
		{
			std::wstringstream msg;
			msg << L"��������������� �������� ������� ����� �� ����� � ������ ";
			msg << this->aprInfo->GetRealCoord(cur.base).format_long() << L" - ";
			msg << this->aprInfo->GetRealCoord(prv.end).format_long() << std::endl;
			result.push_back(msg.str());
			++errorCount;

			double c = 0.5 * (cur.base + prv.end);
			this->zlPlanList.GetItem(i).base = c;
			this->zlPlanList.GetItem(i-1).end = c;
		}
	}

	// � ������ ����� ������ ������ ����� 0
	widthRecord.wear_begin = 0;
	widthRecord.wear_end = 0;

	if (planElemCount == 0)
	{
		zlRecord.base = absStart;
		zlRecord.end = absEnd;
		zlRecord.value_base = 0;
		zlRecord.value_end = 0;
		zlRecord.zlType = ZLINE_UNDEFINED;
		widthRecord.zlType = ZLINE_UNDEFINED;
		widthRecord.Radius = 0;
		this->zlPlanList.Add(zlRecord);
		this->zlRichList.Add(zlRecord);

		std::vector<VAprinfoIndexRecord> nw = 
			this->aprInfo->GetStraightNormaWidthList(zlRecord.base, zlRecord.end);

		if (nw.empty())
		{
			widthRecord.base = zlRecord.base;
			widthRecord.end = zlRecord.end;
			widthRecord.width_begin = 1520;
			widthRecord.width_end = 1520;
			this->widthList.Add(widthRecord);
		}
		else
		{
			std::vector<VAprinfoWidthRecord> widths = 
				__SmartTransitionWidth(nw, ZLINE_UNDEFINED);

			for (size_t k = 0; k < widths.size(); ++k)
			{
				this->widthList.Add(widths[k]);
			}
		}

		std::vector<VAprinfoIndexRecord> sl = 
			this->aprInfo->GetStraightThreadSideList(zlRecord.base, zlRecord.end);

		if (sl.empty())
		{
			threadRecord.base = zlRecord.base;
			threadRecord.end = zlRecord.end;
			threadRecord.value = SD_RIGHT;
			this->threadList.Add(threadRecord);
		}
		else
		{
			for (size_t k = 0; k < sl.size(); ++k)
			{
				if (sl[k].base == sl[k].end) continue;
				threadRecord.base = sl[k].base;
				threadRecord.end = sl[k].end;
				threadRecord.value = sl[k].value;
				this->threadList.Add(threadRecord);
			}
		}
	}

	else
	{
		zlRecord.value_base = 0;
		zlRecord.value_end = 0;
		zlRecord.zlType = ZLINE_STRAIGHT;
		widthRecord.zlType = ZLINE_STRAIGHT;
		widthRecord.Radius = 0;

		for (size_t i = 0; i < planElemCount; ++i)
		{
			if (i == 0 && zlPlanList.GetItem(0).base > absStart)
			{
				zlRecord.base = absStart;
				zlRecord.end = zlPlanList.GetItem(0).base;
				this->zlPlanList.Add(zlRecord);
				this->zlRichList.Add(zlRecord);
			}
			else if (zlPlanList.GetItem(i).base > zlPlanList.GetItem(i-1).end)
			{
				zlRecord.base = zlPlanList.GetItem(i-1).end;
				zlRecord.end = zlPlanList.GetItem(i).base;
				this->zlPlanList.Add(zlRecord);
				this->zlRichList.Add(zlRecord);
			}
			else
			{
				continue;
			}

			std::vector<VAprinfoIndexRecord> nw = 
				this->aprInfo->GetStraightNormaWidthList(zlRecord.base, zlRecord.end);

			if (nw.empty())
			{
				widthRecord.base = zlRecord.base;
				widthRecord.end = zlRecord.end;
				widthRecord.width_begin = 1520;
				widthRecord.width_end = 1520;
				this->widthList.Add(widthRecord);
			}
			else
			{
				std::vector<VAprinfoWidthRecord> widths = 
					__SmartTransitionWidth(nw, ZLINE_STRAIGHT);

				for (size_t k = 0; k < widths.size(); ++k)
				{
					this->widthList.Add(widths[k]);
				}
			}

			std::vector<VAprinfoIndexRecord> sl = 
				this->aprInfo->GetStraightThreadSideList(zlRecord.base, zlRecord.end);

			if (sl.empty())
			{
				threadRecord.base = zlRecord.base;
				threadRecord.end = zlRecord.end;
				threadRecord.value = SD_RIGHT;
				this->threadList.Add(threadRecord);
			}
			else
			{
				for (size_t k = 0; k < sl.size(); ++k)
				{
					if (sl[k].base == sl[k].end) continue;
					threadRecord.base = sl[k].base;
					threadRecord.end = sl[k].end;
					threadRecord.value = sl[k].value;
					this->threadList.Add(threadRecord);
				}
			}
		}

		if (zlPlanList.GetItem(planElemCount-1).end < absEnd)
		{
			zlRecord.base = zlPlanList.GetItem(planElemCount-1).end;
			zlRecord.end = absEnd;
			this->zlPlanList.Add(zlRecord);
			this->zlRichList.Add(zlRecord);

			std::vector<VAprinfoIndexRecord> nw = 
				this->aprInfo->GetStraightNormaWidthList(zlRecord.base, zlRecord.end);

			if (nw.empty())
			{
				widthRecord.base = zlRecord.base;
				widthRecord.end = zlRecord.end;
				widthRecord.width_begin = 1520;
				widthRecord.width_end = 1520;
				this->widthList.Add(widthRecord);
			}
			else
			{
				std::vector<VAprinfoWidthRecord> widths = 
					__SmartTransitionWidth(nw, ZLINE_STRAIGHT);

				for (size_t k = 0; k < widths.size(); ++k)
				{
					this->widthList.Add(widths[k]);
				}
			}

			std::vector<VAprinfoIndexRecord> sl = 
				this->aprInfo->GetStraightThreadSideList(zlRecord.base, zlRecord.end);

			if (sl.empty())
			{
				threadRecord.base = zlRecord.base;
				threadRecord.end = zlRecord.end;
				threadRecord.value = SD_RIGHT;
				this->threadList.Add(threadRecord);
			}
			else
			{
				for (size_t k = 0; k < sl.size(); ++k)
				{
					if (sl[k].base == sl[k].end) continue;
					threadRecord.base = sl[k].base;
					threadRecord.end = sl[k].end;
					threadRecord.value = sl[k].value;
					this->threadList.Add(threadRecord);
				}
			}
		}
	}

	this->zlPlanList.Sort();
	this->zlRichList.Sort();
	this->widthList.Sort();
	this->threadList.Sort();

// -------------------------------------------------------------------
// �������
// -------------------------------------------------------------------

	for (size_t i = 0; i < this->curveList.GetCount(); ++i)
	{
		const VAprinfoCurve & curve = this->curveList.GetItem(i);
		if (!curve.isThere || 
			curve.planLabel.empty() || 
			curve.levelLabel.empty() || 
			curve.radius.empty()) continue;

		for (size_t k = 1; k < curve.levelLabel.size(); ++k)
		{
			const VAprinfoCrvLabel & label0 = curve.levelLabel[k-1];
			const VAprinfoCrvLabel & label1 = curve.levelLabel[k];

			if (label0.coord > absEnd || label1.coord < absStart)
			{
				continue;
			}

			zlRecord.base = label0.coord;
			if (k == 1)	
				zlRecord.value_base = this->aprInfo->GetStraightNormaLevel(zlRecord.base);
			else
				zlRecord.value_base = label0.value;

			zlRecord.end = label1.coord;
			if (k == curve.levelLabel.size()-1)
				zlRecord.value_end = this->aprInfo->GetStraightNormaLevel(zlRecord.end);
			else
				zlRecord.value_end = label1.value;

			if (zlRecord.base < absStart)
			{
				zlRecord.value_base += zlRecord.Shift(absStart, true);
				zlRecord.base = absStart;
			}

			if (zlRecord.end > absEnd)
			{
				zlRecord.value_end -= zlRecord.Shift(absEnd, false);
				zlRecord.end = absEnd;
			}

			if (k == 1 || k == curve.levelLabel.size() - 1)
			{
				zlRecord.zlType = ZLINE_OUTER_TRANSITION;
			}
			else if ((k % 2) == 0)
			{
				zlRecord.zlType = ZLINE_RADIUS;
			}
			else
			{
				zlRecord.zlType = ZLINE_INNER_TRANSITION;
			}

			this->zlLevelList.Add(zlRecord);
		}
	}

	this->zlLevelList.Sort();
	size_t levelElemCount = this->zlLevelList.GetCount();
	errorCount = 0;

	// ����� ��������� �� ������� ����� �� ������
	for (size_t i = 1; i < this->zlLevelList.GetCount(); ++i)
	{
		VAprinfoZLRecord & prv = this->zlLevelList.GetItem(i-1);
		VAprinfoZLRecord & cur = this->zlLevelList.GetItem(i);

		if (cur.base < prv.end)
		{
			std::wstringstream msg;
			msg << L"��������������� �������� ������� ����� �� ������ � ������ ";
			msg << this->aprInfo->GetRealCoord(cur.base).format_long() << L" - ";
			msg << this->aprInfo->GetRealCoord(prv.end).format_long() << std::endl;
			result.push_back(msg.str());
			++errorCount;

			double c = 0.5 * (cur.base + prv.end);
			this->zlLevelList.GetItem(i).base = c;
			this->zlLevelList.GetItem(i-1).end = c;

		}
	}

	// ������ ������
	if (levelElemCount == 0)
	{
		zlRecord.base = absStart;
		zlRecord.end = absEnd;
		zlRecord.zlType = ZLINE_UNDEFINED;

		std::vector<VAprinfoIndexRecord> nl = 
			this->aprInfo->GetStraightNormaLevelList(zlRecord.base, zlRecord.end);

		if (nl.empty())
		{
			zlRecord.value_base = 0;
			zlRecord.value_end = 0;
			this->zlLevelList.Add(zlRecord);
		}
		else
		{
			std::vector<VAprinfoZLRecord> levels = 
				__SmartTransitionLevel(nl, ZLINE_UNDEFINED);

			for (size_t k = 0; k < levels.size(); ++k)
			{
				this->zlLevelList.Add(levels[k]);
			}			
		}
	}

	// �������� ������
	else
	{
		zlRecord.zlType = ZLINE_STRAIGHT;
		for (size_t i = 0; i < levelElemCount; ++i)
		{
			if (i == 0 && this->zlLevelList.GetItem(0).base > absStart)
			{
				zlRecord.base = absStart;
				zlRecord.end = zlLevelList.GetItem(0).base;
			}
			else if (this->zlLevelList.GetItem(i).base > this->zlLevelList.GetItem(i-1).end)
			{
				zlRecord.base = zlLevelList.GetItem(i-1).end;
				zlRecord.end = zlLevelList.GetItem(i).base;
			}
			else
			{
				continue;
			}

			if (int(zlRecord.end) == 20526)
			{
				int Z = 0;
			}

			std::vector<VAprinfoIndexRecord> nl = 
				this->aprInfo->GetStraightNormaLevelList(zlRecord.base, zlRecord.end);

			if (nl.empty())
			{
				zlRecord.value_base = 0;
				zlRecord.value_end = 0;
				this->zlLevelList.Add(zlRecord);
			}
			else
			{
				std::vector<VAprinfoZLRecord> levels = 
					__SmartTransitionLevel(nl, ZLINE_STRAIGHT);

				for (size_t k = 0; k < levels.size(); ++k)
				{
					this->zlLevelList.Add(levels[k]);
				}			
			}
		}

		if (zlLevelList.GetItem(levelElemCount-1).end < absEnd)
		{
			zlRecord.base = zlLevelList.GetItem(levelElemCount-1).end;
			zlRecord.end = absEnd;

			std::vector<VAprinfoIndexRecord> nl = 
				this->aprInfo->GetStraightNormaLevelList(zlRecord.base, zlRecord.end);

			if (nl.empty())
			{
				zlRecord.value_base = 0;
				zlRecord.value_end = 0;
				this->zlLevelList.Add(zlRecord);
			}
			else
			{
				std::vector<VAprinfoZLRecord> levels = 
					__SmartTransitionLevel(nl, ZLINE_STRAIGHT);

				for (size_t k = 0; k < levels.size(); ++k)
				{
					this->zlLevelList.Add(levels[k]);
				}			
			}
		}
	}

	this->zlLevelList.Sort();
	
/// FixMe!!! ��� ��������� ������� ����� +-6 �� ������ ��������� "����". ����� �� �������. ����� ����� ���������� ��������.
std::deque<VAprinfoZLRecord> fix_list;
for (size_t i=1;i<zlLevelList.GetCount(); ++i)
{
	if (zlLevelList.GetItem(i-1).end<zlLevelList.GetItem(i).base)
	{
		VAprinfoZLRecord fix;
		fix.zlType=ZLINE_STRAIGHT;
		fix.base=zlLevelList.GetItem(i-1).end;
		fix.value_base=zlLevelList.GetItem(i-1).value_end;
		fix.end=zlLevelList.GetItem(i).base;
		fix.value_end=zlLevelList.GetItem(i).value_base;
		fix_list.push_back(fix);
	}
}
for (size_t i=0;i<fix_list.size();++i)
{
	zlLevelList.Add(fix_list.at(i));	
}
this->zlLevelList.Sort();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return result;
}

// --------------------------------------------------------------------------
// ���������� ������
// --------------------------------------------------------------------------

// ������ ������ �� �������
std::vector<VAprinfoCurve> VAprinfo::GetPassportCurveList(double from, double to) const
{
	return this->crvPassport.curveList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoCurve>::FitsObject, false);
}

// ������ ������ �� ����
std::vector<VAprinfoCurve> VAprinfo::GetPassportCurveList() const
{
	return this->crvPassport.curveList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoCurve>::FitsObject, false);
}

// ������� ������ (���� ������� ���, ����� exception)
const VAprinfoCrvRadius & VAprinfo::GetPassportCurveRadiusRecord(double coord) const
{
	size_t index = this->crvPassport.curveList.FindItem(coord);
	if (index == BAD_INDEX)
	{
		throw VSimpleException(L"������ Aprinfo", L"����� �� ��������� �����������");
	}

	const VAprinfoCurve & curve = this->crvPassport.curveList.GetItem(index);
	if (!curve.isThere)
	{
		throw VSimpleException(L"������ Aprinfo", L"� ��������� ����� ����������� ������");
	}

	for (size_t i = 0; i < curve.radius.size(); ++i)
	{
		size_t markNdx0 = 2*(i+1) - 1, markNdx1 = 2*(i+1);

		if (markNdx0 < curve.planLabel.size() && coord >= curve.planLabel[markNdx0].coord &&
			markNdx1 < curve.planLabel.size() && coord <= curve.planLabel[markNdx1].coord)
		{
			return curve.radius[i];
		}
	}
	throw VSimpleException(L"������ Aprinfo", L"��������� ����� �� ����� �� �������� ������");
}

// ���������� ������ � �����
long VAprinfo::GetPassportCurveOrientation(double coord) const
{
	size_t index = this->crvPassport.curveList.FindItem(coord);
	if (index == BAD_INDEX) return 0;
	else return this->crvPassport.curveList.GetItem(index).orientation;
}

// ������ �������� ������� - �� �����
ZLINE_TYPE VAprinfo::GetPassportCurveStatusPlan(double coord) const
{
	size_t index = this->crvPassport.zlPlanList.FindItem(coord);
	if (index == BAD_INDEX)	return ZLINE_UNDEFINED;
	else					return this->crvPassport.zlPlanList.GetItem(index).zlType;
}

// ������� ������ � ������� ����� - �� �����
bool VAprinfo::IsPassportCurvePlan(double coord) const
{
	ZLINE_TYPE type = this->GetPassportCurveStatusPlan(coord);
	return type == ZLINE_OUTER_TRANSITION || 
		type == ZLINE_RADIUS || 
		type == ZLINE_INNER_TRANSITION;
}

// ������ �������� ������� - �� ������
ZLINE_TYPE VAprinfo::GetPassportCurveStatusLevel(double coord) const
{
	size_t index = this->crvPassport.zlLevelList.FindItem(coord);
	if (index == BAD_INDEX)	return ZLINE_UNDEFINED;
	else					return this->crvPassport.zlLevelList.GetItem(index).zlType;
}

// ������� ������ � ������� ����� - �� ������
bool VAprinfo::IsPassportCurveLevel(double coord) const
{
	ZLINE_TYPE type = this->GetPassportCurveStatusLevel(coord);
	return type == ZLINE_OUTER_TRANSITION || 
		type == ZLINE_RADIUS || 
		type == ZLINE_INNER_TRANSITION;
}

// id ������ � �����
__int64 VAprinfo::GetPassportCurveId(double coord) const
{
	size_t index = this->crvPassport.curveList.FindItem(coord);
	if (index == BAD_INDEX) return 0;
	else return this->crvPassport.curveList.GetItem(index).id;
}

// ���������� � ������ ������
float VAprinfo::GetPassportCurveNormaLevel(double coord) const
{
	size_t index = this->crvPassport.zlLevelList.FindItem(coord);
	if (index == BAD_INDEX)	return 0;
	else
	{
		const VAprinfoZLRecord & record = this->crvPassport.zlLevelList.GetItem(index);
		return record.value_base + record.Shift(coord, true);
	}
}

// �������� � ������ ������
float VAprinfo::GetPassportCurveNormaCurve(double coord) const
{
	size_t index = this->crvPassport.zlPlanList.FindItem(coord);
	if (index == BAD_INDEX)	return 0;
	else
	{
		const VAprinfoZLRecord & record = this->crvPassport.zlPlanList.GetItem(index);
		return record.value_base + record.Shift(coord, true);
	}
}

// �������� � ������ ������
float VAprinfo::GetPassportCurveNormaRich(double coord) const
{
	size_t index = this->crvPassport.zlRichList.FindItem(coord);
	if (index == BAD_INDEX)	return 0;
	else
	{
		const VAprinfoZLRecord & record = this->crvPassport.zlRichList.GetItem(index);
		return record.value_base + record.Shift(coord, true);
	}
}

// ������
float VAprinfo::GetPassportCurveRadius(double coord) const
{
	float value = fabs(this->GetPassportCurveNormaCurve(coord));
	if (value < 1.0) return 50000.0;
	else return 50000.0 / value;
}

// ����� �� ��� ��������
float VAprinfo::GetPassportCurveNormaWidthPlus(double coord) const
{
	size_t index = this->crvPassport.widthList.FindItem(coord);
	if (index == BAD_INDEX) return 1520.0f;
	else return this->crvPassport.widthList.GetItem(index).GetWidthMax();
}

// ����� �� ��� �������
float VAprinfo::GetPassportCurveNormaWidthMinus(double coord) const
{
	size_t index = this->crvPassport.widthList.FindItem(coord);
	if (index == BAD_INDEX) return 1520.0f;
	else return this->crvPassport.widthList.GetItem(index).GetWidthMin();
}

// ����������� ���� �� ������
long VAprinfo::GetPassportCurveThreadSide(double coord) const
{
	return -1 * this->GetPassportCurveOrientation(coord);
}

// ������ ������� ����� �� ����� (��������), �� �������
std::vector<VAprinfoZLRecord> VAprinfo::GetPassportCurvePlanZLList(double from, double to) const
{
	return this->crvPassport.zlPlanList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoZLRecord>::FitsAlways, false);
}

// ������ ������� ����� �� ����� (��������), �� ����
std::vector<VAprinfoZLRecord> VAprinfo::GetPassportCurvePlanZLList() const
{
	return this->crvPassport.zlPlanList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoZLRecord>::FitsAlways, false);
}

// ������ ������� ����� �� ����� (��������), �� �������
std::vector<VAprinfoZLRecord> VAprinfo::GetPassportCurveRichZLList(double from, double to) const
{
	return this->crvPassport.zlRichList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoZLRecord>::FitsAlways, false);
}

// ������ ������� ����� �� ����� (��������), �� ����
std::vector<VAprinfoZLRecord> VAprinfo::GetPassportCurveRichZLList() const
{
	return this->crvPassport.zlRichList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoZLRecord>::FitsAlways, false);
}

// ������ ������� ����� �� ������, �� �������
std::vector<VAprinfoZLRecord> VAprinfo::GetPassportCurveLevelZLList(double from, double to) const
{
	return this->crvPassport.zlLevelList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoZLRecord>::FitsAlways, false);
}

// ������ ������� ����� �� ������, �� ����
std::vector<VAprinfoZLRecord> VAprinfo::GetPassportCurveLevelZLList() const
{
	return this->crvPassport.zlLevelList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoZLRecord>::FitsAlways, false);
}

// ������ ���� �� � ������ �� �������
std::vector<VAprinfoWidthRecord> VAprinfo::GetPassportCurveWidthList(double from, double to, bool cut_ranges_to_section) const
{
	return this->crvPassport.widthList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoWidthRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ ���� �� � ������ �� ����
std::vector<VAprinfoWidthRecord> VAprinfo::GetPassportCurveWidthList() const
{
	return this->crvPassport.widthList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoWidthRecord>::FitsAlways, false);
}

// ������ ����������� ����� � ������ ������ �� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetPassportCurveStraightList(double from, double to, bool cut_ranges_to_section) const
{
	return this->crvPassport.threadList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ ����������� ����� � ������ ������ �� ����
std::vector<VAprinfoIndexRecord> VAprinfo::GetPassportCurveStraightList() const
{
	return this->crvPassport.threadList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, false);
}

/// ���������� ����������� �� ��������� � ������������� ��������� � ������ ������
VSpeeds VAprinfo::GetPassportCurveSpeeds(double coord) const
{
	size_t index = this->crvPassport.curveList.FindItem(coord);
	if (index == BAD_INDEX) 
	{
		return GetCombinedSpeeds(coord);
	}
	else
	{
		VSpeeds trspd = this->GetTestrunSpeed(coord);
		double start = this->crvPassport.curveList.GetItem(index).base;
		double end = this->crvPassport.curveList.GetItem(index).end;
		VSpeeds speeds = this->GetMinSpeeds(start, end);
		VSpeeds tmp_speeds = this->GetMinTmpSpeeds(start, end);
		if (tmp_speeds.Valid())
		{
			return VSpeeds::CombineSpeeds(trspd, speeds, tmp_speeds);
		}
		else
		{
			return speeds;
		}
	}
}

// ��������� ����������� ���� � ������ ������
long VAprinfo::GetPassportCurveSummaryThreadSide(double coord) const
{
	size_t index = this->crvPassport.threadList.FindItem(coord);
	if (index == BAD_INDEX)	return 0;
	else return this->crvPassport.threadList.GetItem(index).value;
}

// --------------------------------------------------------------------------
// ����������� ������
// --------------------------------------------------------------------------

// ������ ������ �� �������
std::vector<VAprinfoCurve> VAprinfo::GetFactCurveList(double from, double to) const
{
	return this->crvFact.curveList.ExportAreas(from, to, 
		VAprinfoParameter<VAprinfoCurve>::FitsObject, false);
}

// ������ ������ �� ����
std::vector<VAprinfoCurve> VAprinfo::GetFactCurveList() const
{
	return this->crvFact.curveList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(), 
		VAprinfoParameter<VAprinfoCurve>::FitsObject, false);
}

// ������� ������ (���� ������� ���, ����� exception)
const VAprinfoCrvRadius & VAprinfo::GetFactCurveRadiusRecord(double coord) const
{
	size_t index = this->crvFact.curveList.FindItem(coord);
	if (index == BAD_INDEX)
	{
		throw VSimpleException(L"������ Aprinfo", L"����� �� ��������� �����������");
	}

	const VAprinfoCurve & curve = this->crvFact.curveList.GetItem(index);
	if (!curve.isThere)
	{
		throw VSimpleException(L"������ Aprinfo", L"� ��������� ����� ����������� ������");
	}

	for (size_t i = 0; i < curve.radius.size(); ++i)
	{
		size_t markNdx0 = 2*(i+1) - 1, markNdx1 = 2*(i+1);

		if (markNdx0 < curve.planLabel.size() && coord >= curve.planLabel[markNdx0].coord &&
			markNdx1 < curve.planLabel.size() && coord <= curve.planLabel[markNdx1].coord)
		{
			return curve.radius[i];
		}
	}
	throw VSimpleException(L"������ Aprinfo", L"��������� ����� �� ����� �� �������� ������");
}

// ���������� ������ � �����
long VAprinfo::GetFactCurveOrientation(double coord) const
{
	size_t index = this->crvFact.curveList.FindItem(coord);
	if (index == BAD_INDEX) return 0;
	else return this->crvFact.curveList.GetItem(index).orientation;
}

// ������ �������� ������� - �� �����
ZLINE_TYPE VAprinfo::GetFactCurveStatusPlan(double coord) const
{
	size_t index = this->crvFact.zlPlanList.FindItem(coord);
	if (index == BAD_INDEX)	return ZLINE_UNDEFINED;
	else					return this->crvFact.zlPlanList.GetItem(index).zlType;
}

// ������� ������ � ������� ����� - �� �����
bool VAprinfo::IsFactCurvePlan(double coord) const
{
	ZLINE_TYPE type = this->GetFactCurveStatusPlan(coord);
	return 
		type == ZLINE_OUTER_TRANSITION || 
		type == ZLINE_RADIUS || 
		type == ZLINE_INNER_TRANSITION;
}

// ������ �������� ������� - �� ������
ZLINE_TYPE VAprinfo::GetFactCurveStatusLevel(double coord) const
{
	size_t index = this->crvFact.zlLevelList.FindItem(coord);
	if (index == BAD_INDEX)	return ZLINE_UNDEFINED;
	else					return this->crvFact.zlLevelList.GetItem(index).zlType;
}

// ������� ������ � ������� ����� - �� ������
bool VAprinfo::IsFactCurveLevel(double coord) const
{
	ZLINE_TYPE type = this->GetFactCurveStatusLevel(coord);
	return 
		type == ZLINE_OUTER_TRANSITION || 
		type == ZLINE_RADIUS || 
		type == ZLINE_INNER_TRANSITION;
}

// id ������ � �����
__int64 VAprinfo::GetFactCurveId(double coord) const
{
	size_t index = this->crvFact.curveList.FindItem(coord);
	if (index == BAD_INDEX) return 0;
	else return this->crvFact.curveList.GetItem(index).id;
}

// ���������� � ������ ������
float VAprinfo::GetFactCurveNormaLevel(double coord) const
{
	size_t index = this->crvFact.zlLevelList.FindItem(coord);
	if (index == BAD_INDEX)	return 0;
	else
	{
		const VAprinfoZLRecord & record = this->crvFact.zlLevelList.GetItem(index);
		return record.value_base + record.Shift(coord, true);
	}
}

// �������� � ������ ������
float VAprinfo::GetFactCurveNormaCurve(double coord) const
{
	size_t index = this->crvFact.zlPlanList.FindItem(coord);
	if (index == BAD_INDEX)	return 0;
	else
	{
		const VAprinfoZLRecord & record = this->crvFact.zlPlanList.GetItem(index);
		return record.value_base + record.Shift(coord, true);
	}
}

// �������� � ������ ������
float VAprinfo::GetFactCurveNormaRich(double coord) const
{
	size_t index = this->crvFact.zlRichList.FindItem(coord);
	if (index == BAD_INDEX)	return 0;
	else
	{
		const VAprinfoZLRecord & record = this->crvFact.zlRichList.GetItem(index);
		return record.value_base + record.Shift(coord, true);
	}
}

// ������
float VAprinfo::GetFactCurveRadius(double coord) const
{
	float value = fabs(this->GetFactCurveNormaCurve(coord));
	if (value < 1.0) return 50000.0;
	else return 50000.0 / value;
}

// ����� �� ��� ��������
float VAprinfo::GetFactCurveNormaWidthPlus(double coord) const
{
	size_t index = this->crvFact.widthList.FindItem(coord);
	if (index == BAD_INDEX) return 1520.0f;
	else return this->crvFact.widthList.GetItem(index).GetWidthMax();
}

// ����� �� ��� �������
float VAprinfo::GetFactCurveNormaWidthMinus(double coord) const
{
	size_t index = this->crvFact.widthList.FindItem(coord);
	if (index == BAD_INDEX) return 1520.0f;
	else return this->crvFact.widthList.GetItem(index).GetWidthMin();
}

// ����������� ���� �� ������
long VAprinfo::GetFactCurveThreadSide(double coord) const
{
	return -1 * this->GetFactCurveOrientation(coord);
}

// ������ ������� ����� �� ����� (��������), �� �������
std::vector<VAprinfoZLRecord> VAprinfo::GetFactCurvePlanZLList(double from, double to) const
{
	return this->crvFact.zlPlanList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoZLRecord>::FitsAlways, false);
}

// ������ ������� ����� �� ����� (��������), �� ����
std::vector<VAprinfoZLRecord> VAprinfo::GetFactCurvePlanZLList() const
{
	return this->crvFact.zlPlanList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoZLRecord>::FitsAlways, false);
}

// ������ ������� ����� �� ����� (��������), �� �������
std::vector<VAprinfoZLRecord> VAprinfo::GetFactCurveRichZLList(double from, double to) const
{
	return this->crvFact.zlRichList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoZLRecord>::FitsAlways, false);
}

// ������ ������� ����� �� ����� (��������), �� ����
std::vector<VAprinfoZLRecord> VAprinfo::GetFactCurveRichZLList() const
{
	return this->crvFact.zlRichList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoZLRecord>::FitsAlways, false);
}

// ������ ������� ����� �� ������, �� �������
std::vector<VAprinfoZLRecord> VAprinfo::GetFactCurveLevelZLList(double from, double to) const
{
	return this->crvFact.zlLevelList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoZLRecord>::FitsAlways, false);
}

// ������ ������� ����� �� ������, �� ����
std::vector<VAprinfoZLRecord> VAprinfo::GetFactCurveLevelZLList() const
{
	return this->crvFact.zlLevelList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoZLRecord>::FitsAlways, false);
}

// ������ ���� �� � ������ �� �������
std::vector<VAprinfoWidthRecord> VAprinfo::GetFactCurveWidthList(double from, double to, bool cut_ranges_to_section) const
{
	return this->crvFact.widthList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoWidthRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ ���� �� � ������ �� ����
std::vector<VAprinfoWidthRecord> VAprinfo::GetFactCurveWidthList() const
{
	return this->crvFact.widthList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoWidthRecord>::FitsAlways, false);
}

// ������ ����������� ����� � ������ ������ �� �������
std::vector<VAprinfoIndexRecord> VAprinfo::GetFactCurveStraightList(double from, double to, bool cut_ranges_to_section) const
{
	return this->crvFact.threadList.ExportAreas(from, to,
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, cut_ranges_to_section);
}

// ������ ����������� ����� � ������ ������ �� ����
std::vector<VAprinfoIndexRecord> VAprinfo::GetFactCurveStraightList() const
{
	return this->crvFact.threadList.ExportAreas(this->GetAbsCoordBase(), this->GetAbsCoordEnd(),
		VAprinfoParameter<VAprinfoIndexRecord>::FitsAlways, false);
}

/// ���������� ����������� �� ��������� � ������������� ��������� � ������ ������
VSpeeds VAprinfo::GetFactCurveSpeeds(double coord) const
{
	size_t index = this->crvPassport.curveList.FindItem(coord);
	if (index == BAD_INDEX) 
	{
		return GetCombinedSpeeds(coord);
	}
	else
	{
		VSpeeds trspd = this->GetTestrunSpeed(coord);
		double start = this->crvFact.curveList.GetItem(index).base;
		double end = this->crvFact.curveList.GetItem(index).end;
		VSpeeds speeds = this->GetMinSpeeds(start, end);
		VSpeeds tmp_speeds = this->GetMinTmpSpeeds(start, end);
		if (tmp_speeds.Valid())
		{
			return VSpeeds::CombineSpeeds(trspd, speeds, tmp_speeds);
		}
		else
		{
			return speeds;
		}
	}
}

// ��������� ����������� ���� � ������ ������
long VAprinfo::GetFactCurveSummaryThreadSide(double coord) const
{
	size_t index = this->crvFact.threadList.FindItem(coord);
	if (index == BAD_INDEX)	return 0;
	else return this->crvFact.threadList.GetItem(index).value;
}

// ��������� ������� ������
std::wstring VAprinfo::CompareZL(int km_num) const
{
	size_t index = this->FindKm(km_num);
	if (index == BAD_INDEX) return L"";

	const VAprinfoKmRecord & km = this->kmList.GetItem(index);

	// ������ ����� �� ��������

	std::vector<double> prm_passport;
	size_t count_passport_curve = 0;
	for (double pos = km.base; pos < km.end; pos += 1.0)
	{
		size_t index = this->crvPassport.zlPlanList.FindItem(pos);

		double value;
		if (index == BAD_INDEX) 
		{
			value = 0;
		}
		else
		{
			value = this->crvPassport.zlPlanList.GetItem(index).Shift(pos, true);
			if (this->crvPassport.zlPlanList.GetItem(index).zlType != ZLINE_STRAIGHT &&
				this->crvPassport.zlPlanList.GetItem(index).zlType != ZLINE_UNDEFINED) 
			{
				++count_passport_curve;
			}
		}

		prm_passport.push_back(value);
	}

	// ������ ����� �� �����, +/- 100 ������, ��� ������

	std::vector<double> prm_fact;
	size_t count_fact_curve = 0;
	for (double pos = km.base - 100; pos < km.end + 100; pos += 1.0)
	{
		size_t index = this->crvFact.zlPlanList.FindItem(pos);

		double value;
		if (index == BAD_INDEX) 
		{
			value = 0;
		}
		else
		{
			value = this->crvFact.zlPlanList.GetItem(index).Shift(pos, true);
			if (this->crvFact.zlPlanList.GetItem(index).zlType != ZLINE_STRAIGHT &&
				this->crvFact.zlPlanList.GetItem(index).zlType != ZLINE_UNDEFINED) 
			{
				++count_fact_curve;
			}
		}

		prm_fact.push_back(value);
	}

	// �������� - ��� ��������� �����
	if (prm_passport.empty() || prm_fact.empty())
	{
		return L"������ �������";
	}

	// ������ �� ����� �����
	if (count_passport_curve == 0 && count_fact_curve == 0)
	{
		return L"";
	}

	else if (count_passport_curve == 0)
	{
		return L"����������� ���������� ������";
	}

	else if (count_fact_curve == 0)
	{
		return L"����������� ���������� ������";
	}

	size_t max_offset = 0;
	double max_corr = correlate(prm_passport.begin(), prm_fact.begin(), prm_passport.size());

	for (size_t i = 1; i < prm_fact.size() - prm_passport.size(); ++i)
	{
		double corr = correlate(prm_passport.begin(), prm_fact.begin() + i, prm_passport.size());

		if (corr > max_corr)
		{
			max_corr = corr;
			max_offset = i;
		}
	}

	if (max_corr < 0.5)	return L"����������� ��������������� ���������� ������ ����������";
	else if (max_corr < 0.8) return L"��������������� ���������� ������ ����������";

	else if (abs(int(max_offset) - 100) > 10) 
	{
		std::wstringstream msg;
		msg << L"������������� �������� �������� �� " << int(max_offset) - 100 << L" ������";
		return msg.str();
	}

	return L"";
}

// ��������� ������� ������
void VAprinfo::CompareZL() const
{
	size_t crv_passport_count = 0;
	for (size_t i = 0; i < this->crvPassport.curveList.GetCount(); ++i)
	{
		if (this->crvPassport.curveList.GetItem(i).isThere) 
		{
			++crv_passport_count;
		}
	}

	if (crv_passport_count == 0)
	{
		LOG_ERROR(L"������ ���������� ������ ����");
		return;
	}

	size_t crv_fact_count = 0;
	for (size_t i = 0; i < this->crvFact.curveList.GetCount(); ++i)
	{
		if (this->crvFact.curveList.GetItem(i).isThere) 
		{
			++crv_fact_count;
		}
	}

	if (crv_fact_count == 0)
	{
		LOG_WARNING(L"������ ����������� ������ ����");
		return;
	}

	for (size_t kmNdx = 0; kmNdx < this->kmList.GetCount(); ++kmNdx)
	{
		std::wstring message = this->CompareZL(this->kmList.GetItem(kmNdx).km);
		if (!message.empty())
		{
			std::wstringstream msg; 
			msg << L"�������� " << this->kmList.GetItem(kmNdx).km << L": " << message;
			LOG_WARNING(msg.str());
		}
	}
}

// ������� ���������� ������ �� �������
bool VAprinfo::ReplaceMeasuredCurves(const std::vector<VAprinfoCurve> & curves)
{
	this->crvFact.Unload();

	VAprinfoCurve between;
	between.isThere = false;
	between.base = this->GetAbsCoordBase();

	for (size_t i = 0; i < curves.size(); ++i)
	{
		if (curves.at(i).base >= this->GetAbsCoordBase() &&
			curves.at(i).end <= this->GetAbsCoordEnd())
		{
			between.end = curves.at(i).base;
			this->crvFact.curveList.Add(between);
			this->crvFact.curveList.Add(curves.at(i));
			between.base = curves.at(i).end;
		}
	}

	between.end = this->GetAbsCoordEnd();
	this->crvFact.curveList.Add(between);
	this->crvFact.BuildZL(this->GetAbsCoordBase(), this->GetAbsCoordEnd());

	return true;
}
