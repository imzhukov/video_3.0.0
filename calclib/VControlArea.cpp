#include "VControlArea.h"

bool VControlArea::Import(const VFileControlArea & record)
{
	this->version = record.version;
	this->width = record.width;
	this->level = record.level;
	this->section.dwSize = sizeof(VWaySection);
	this->section.start.km = record.kmn;
	this->section.start.m = record.mn;
	this->section.end.km = record.kmk;
	this->section.end.m = record.mk;
	return true;
}

VFileControlArea VControlArea::Export() const
{
	VFileControlArea ca;
	ca.version = this->version;
	ca.width = this->width;
	ca.level = this->level;
	ca.kmn = static_cast<short>(section.start.km);
	ca.mn = section.start.m;
	ca.kmk = static_cast<short>(section.end.km);
	ca.mk = section.end.m;
	return ca;
}

// Копирование списка
bool VControlAreaList::CopyFrom(const VControlAreaList & src, const VWaySection * section)
{
	this->Unload();
	for (size_t itemNdx = 0; itemNdx < src.GetCount(); ++itemNdx)
	{
		VControlArea area = src.GetItem(itemNdx);
		if (area.MeetsSection(section))
		{
			this->records.push_back(area);
		}
	}
	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
}
