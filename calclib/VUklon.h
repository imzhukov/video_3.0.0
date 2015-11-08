#pragma once

#include "VDataSet.h"
#include "VParameter.h"

struct VUklon
{
public:
	double length;
	double value;
	int name_len;
	double absCoord;
	int porog;
};

/** Уклон*/
struct ParUklon
{
      float fuklon;
	  double dabs;
};

class VUklonList : public VIndexList<VUklon>
{
public:

	/// Загрузка
	bool Load(VDataSet & ds, VProgressBox * prog);
};