#pragma once

#include "VDataSet.h"
#include "VParameter.h"

struct VPer
{
public:
	double length;
	double value;
	double absPer;
	int porog;
	long speed;
};

class VPerTelList : public VIndexList<VPer>
{
public:

	/// Загрузка
	bool Load(VDataSet & ds, VProgressBox * prog);
};

class VPerKuzList : public VIndexList<VPer>
{
public:

	/// Загрузка
	bool Load(VDataSet & ds, VProgressBox * prog);
	float GetPorog(unsigned int Vpz);
};
