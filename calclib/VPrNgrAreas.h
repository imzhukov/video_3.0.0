#pragma once

#include "types.h"
#include "VFileName.h"
#include "VParameter.h"
#include "VAprinfo.h"

/// Описание участка
struct VPrNgrAreaDescr
{	
	double begin, end; // границы в абс.к-тах
	VWaySection section; // границы участка
	std::wstring railsType, railsBrace; // тип рельсов и скреплений

	// макс.пасс.скорость
	short maxSpeed1;

	// мин-среднее-макс 
	float prLeftMin, prLeftAvg, prLeftMax;
	float prRightMin, prRightAvg, prRightMax;
	float ngrLeftMin, ngrLeftAvg, ngrLeftMax;
	float ngrRightMin, ngrRightAvg, ngrRightMax;
	
	// к-во метров ниже 1/60 (1,66667) и выше 1/12 (8,33333)
	double lenPrLeftUnderEdge, lenPrLeftOverEdge, lenPrRightUnderEdge, lenPrRightOverEdge,
		lenNgrLeftUnderEdge, lenNgrLeftOverEdge, lenNgrRightUnderEdge, lenNgrRightOverEdge; 

	VPrNgrAreaDescr();
};

class VPrNgrAreas : public VIndexList<VPrNgrAreaDescr>
{
public:

	bool Load(const VAprinfo & aprInfo, const VAbsCoordData & absCoord,
		const VParameter & prLeft, const VParameter & prRight,
		const VParameter & ngrLeft, const VParameter & ngrRight);
};

