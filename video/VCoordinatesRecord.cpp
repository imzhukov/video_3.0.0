#include "VCoordinatesRecord.h"

VCoordinatesRecord::VCoordinatesRecord()
{
	dpp = 0xFFFFFFFF;
	absolutePosition = 0.0;
	dir = 0;
	km = 0;
	m = 0;
	strcpy_s(way," NO WAY");
	strcpy_s(peregon,"");
	increaseAbscoord = 0;
}

void VCoordinatesRecord::SetDefaultValues()
{
	dpp = 0xFFFFFFFF;
	absolutePosition = 0.0;
	dir = 0.0;
	km = 0.0;
	m = 0.0;
	strcpy_s(way," NO WAY");
	strcpy_s(peregon,"");
	increaseAbscoord = 0;
}