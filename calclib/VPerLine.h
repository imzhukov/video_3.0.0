#pragma once

#include "VDataSet.h"
#include "VParameter.h"

class VDataSet;

struct VPerLine
{
public:
	float abscoord;
	float dLength;
	float speed1;
	float speed2;
	int dNorma;
	float dMaxValue;
	float dMidValue;


};

class VPerLineList : public VIndexList<VPerLine>
{
public:

	/// Загрузка
	bool Load(VDataSet & ds);
};