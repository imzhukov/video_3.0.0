#include "VPrNgrAreas.h"

VPrNgrAreaDescr::VPrNgrAreaDescr() : begin(0), maxSpeed1(0), prLeftMin(0), prLeftAvg(0), prLeftMax(0),
prRightMin(0), prRightAvg(0), prRightMax(0), ngrLeftMin(0), ngrLeftAvg(0), ngrLeftMax(0),
ngrRightMin(0), ngrRightAvg(0), ngrRightMax(0), lenPrLeftUnderEdge(0), lenPrLeftOverEdge(0), 
lenPrRightUnderEdge(0), lenPrRightOverEdge(0), lenNgrLeftUnderEdge(0), lenNgrLeftOverEdge(0), 
lenNgrRightUnderEdge(0), lenNgrRightOverEdge(0)
{}

bool VPrNgrAreas::Load(const VAprinfo & aprInfo, const VAbsCoordData & absCoord,
					   const VParameter & prLeft, const VParameter & prRight,
					   const VParameter & ngrLeft, const VParameter & ngrRight)
{

	if (!aprInfo.IsLoaded() || !absCoord.IsLoaded() || 
		!prLeft.IsLoaded() || !prRight.IsLoaded() ||
		!ngrLeft.IsLoaded() || !ngrRight.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	// точки, разделяющие участки
	std::set<double> rangePoints;

	std::vector<VAprinfoRailRecord> rails;
	rails = aprInfo.GetRailsTypeList(absCoord.GetItem(0), absCoord.GetLastItem());
	for (size_t i = 0; i < rails.size(); ++i)
	{
		if (i == 0)	rangePoints.insert(rails[i].base);
		rangePoints.insert(rails[i].end);
	}
	std::vector<VAprinfoBraceRecord> braces = aprInfo.GetRailsBraceList(absCoord.GetItem(0), absCoord.GetLastItem());
	for (size_t i = 0; i < braces.size(); ++i)
	{
		if (i == 0)	rangePoints.insert(braces[i].base);
		rangePoints.insert(braces[i].end);
	}

	std::set<double>::const_iterator it;
	for (it = rangePoints.begin(); it != rangePoints.end(); it++)
	{
		std::set<double>::const_iterator it_end = it;
		it_end++;
		if (it_end == rangePoints.end()) break;

		VPrNgrAreaDescr area;
		area.begin = *it;
		area.end = *it_end;
		area.section = VWaySection(aprInfo.GetRealCoord(area.begin), aprInfo.GetRealCoord(area.end));
		area.maxSpeed1 = aprInfo.GetMaxPassSpeed(area.begin, area.end);
		
		rails = aprInfo.GetRailsTypeList(area.begin, area.end);
		if (rails.size() == 1) area.railsType = string_to_wstring(rails[0].GetDescr());
		if (area.railsType.empty()) continue;
		
		braces = aprInfo.GetRailsBraceList(area.begin, area.end);
		if (braces.size() == 1) area.railsBrace = string_to_wstring(braces[0].GetDescr());
		if (area.railsBrace.empty()) continue;

		size_t idxFrom = absCoord.FindIndex(area.begin, true);
		size_t idxTo = absCoord.FindIndex(area.end, true);
		
		// к-во кадров выше верхнего и ниже нижнего порога
		int prLeftOverEdgeCounter = 0, prLeftUnderEdgeCounter = 0,
			prRightOverEdgeCounter = 0, prRightUnderEdgeCounter = 0,
			ngrLeftOverEdgeCounter = 0, ngrLeftUnderEdgeCounter = 0,
			ngrRightOverEdgeCounter = 0, ngrRightUnderEdgeCounter = 0;

		for (size_t i = idxFrom; i < idxTo; ++i)
		{
			if (prLeft.GetItem(i) > 8.33)	prLeftOverEdgeCounter++;
			if (prLeft.GetItem(i) < 1.67)	prLeftUnderEdgeCounter++;

			if (prRight.GetItem(i) > 8.33)	prRightOverEdgeCounter++;
			if (prRight.GetItem(i) < 1.67)	prRightUnderEdgeCounter++;

			if (ngrLeft.GetItem(i) > 8.33)	ngrLeftOverEdgeCounter++;
			if (ngrLeft.GetItem(i) < 1.67)	ngrLeftUnderEdgeCounter++;

			if (ngrRight.GetItem(i) > 8.33)	ngrRightOverEdgeCounter++;
			if (ngrRight.GetItem(i) < 1.67)	ngrRightUnderEdgeCounter++;
		}

		// приведенная длина шага
		double step = (area.end - area.begin) / double(idxTo - idxFrom);
		// длины участков
		area.lenPrLeftOverEdge = step * prLeftOverEdgeCounter;
		area.lenPrLeftUnderEdge = step * prLeftUnderEdgeCounter;
		area.lenPrRightOverEdge = step * prRightOverEdgeCounter;
		area.lenPrRightUnderEdge = step * prRightUnderEdgeCounter;
		area.lenNgrLeftOverEdge = step * ngrLeftOverEdgeCounter;
		area.lenNgrLeftUnderEdge = step * ngrLeftUnderEdgeCounter;
		area.lenNgrRightOverEdge = step * ngrRightOverEdgeCounter;
		area.lenNgrRightUnderEdge = step * ngrRightUnderEdgeCounter;

		// мин-макс-авг
		area.prLeftMin = prLeft.GetMin(idxFrom, idxTo);
		area.prLeftAvg = prLeft.GetMO(idxFrom, idxTo);
		area.prLeftMax = prLeft.GetMax(idxFrom, idxTo);
		area.prRightMin = prRight.GetMin(idxFrom, idxTo);
		area.prRightAvg = prRight.GetMO(idxFrom, idxTo);
		area.prRightMax = prRight.GetMax(idxFrom, idxTo);
		area.ngrLeftMin = ngrLeft.GetMin(idxFrom, idxTo);
		area.ngrLeftAvg = ngrLeft.GetMO(idxFrom, idxTo);
		area.ngrLeftMax = ngrLeft.GetMax(idxFrom, idxTo);
		area.ngrRightMin = ngrRight.GetMin(idxFrom, idxTo);
		area.ngrRightAvg = ngrRight.GetMO(idxFrom, idxTo);
		area.ngrRightMax = ngrRight.GetMax(idxFrom, idxTo);

		this->AddItem(area);
	}

	this->message = ERRMSG_OK;
	return true;
}

