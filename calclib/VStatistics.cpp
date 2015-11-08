#include "c4flib.h"
#include "VCurve.h"
#include "VParameter.h"
#include "VStatistics.h"

SAREA_STATUS VStatData::GetStatus() const
{
	if (this->pntEx > 0)
	{
		return SAS_EXCLUDED;
	}
	else if (this->pntCalc > 0 && this->pntCalc == this->pntSum)
	{
		return SAS_OK;
	}
	else 
	{
		return SAS_NO_DATA;
	}
}

VStatData VStatList::Find(short km, short pk)
{
	VStatData data;
	::memset(&data, 0, sizeof(data));

	for (size_t i = 0; i < this->GetCount(); ++i)
	{
		if (this->GetItem(i).km == km && this->GetItem(i).pk == pk)
		{
			data = this->GetItem(i);
			break;
		}
	}

	return data;
}

inline double Square(const double & value)
{
	return value * value;
}

bool VStatistics::Load(const VAprinfo & aprInfo, const VAbsCoordData & absCoord, 
					   const VParameter & level, const VParameter & zline, 
					   const VParameter & pitLeft, const VParameter & pitRight, const VParameter & width, 
					   const VParameter & richLeft, const VParameter & richRight,
					   const VParameter & prLeft, const VParameter & prRight,
					   const VParameter & ngrLeft, const VParameter & ngrRight,
					   const VPlanElemList & planElemList)
{
	if (!aprInfo.IsLoaded() || !absCoord.IsLoaded() || 
		!level.IsLoaded() || !zline.IsLoaded() || 
		!pitLeft.IsLoaded() || !pitRight.IsLoaded() || !width.IsLoaded() || 
		!richLeft.IsLoaded() || !richRight.IsLoaded() ||
		!planElemList.IsLoaded())	
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	bool havePPR = false;
	if (prLeft.IsLoaded() && prRight.IsLoaded() && ngrLeft.IsLoaded() && ngrRight.IsLoaded())
		havePPR = true;

	VParameter midLevel;
	midLevel.Create(0xFFFFFFFF, absCoord.GetCount(), 1.0, 0.0f);
	for (size_t kadrNdx = 0; kadrNdx < midLevel.GetCount(); ++kadrNdx)
	{
		midLevel.GetItem(kadrNdx) = level.GetMO(kadrNdx, 24, absCoord);
	}

	std::vector<VPlanElement> excluded;
	for (size_t i = 0; i < planElemList.GetCount(); ++i)
	{
		if (planElemList.GetItem(i).type == PLAN_ELEM_HALFCURVE ||
			planElemList.GetItem(i).type == PLAN_ELEM_SIDEWAY)
		{
			excluded.push_back(planElemList.GetItem(i));
		}
	}

	VParameter tmp_level, tmp_diff, tmp_zldiff;
	tmp_level.Create(0xFFFFFFFF, absCoord.GetCount(), 1.0, 0.0f);	// уровень
	tmp_diff.Create(0xFFFFFFFF, absCoord.GetCount(), 1.0, 0.0f);	// перекос	
	tmp_zldiff.Create(0xFFFFFFFF, absCoord.GetCount(), 1.0, 0.0f);	// отклонение от нулевой
	for (size_t kadrNdx = 0; kadrNdx < tmp_level.GetCount(); ++kadrNdx)
	{
		float value_level = level.GetItem(kadrNdx);
		float value_mid_level = midLevel.GetItem(kadrNdx);
		float value_zl_level = zline.GetItem(kadrNdx);

		tmp_level.GetItem(kadrNdx) = value_level - value_zl_level;
		tmp_diff.GetItem(kadrNdx) = value_level - value_mid_level;
		tmp_zldiff.GetItem(kadrNdx) = value_mid_level - value_zl_level;
	}

	std::vector<VAprinfoKmRecord> kmList = 
		aprInfo.GetKmList(absCoord.GetItem(0), absCoord.GetLastItem(), false);

	for (size_t kmIdx = 0; kmIdx < kmList.size(); ++kmIdx)
	{
		VStatData kmRecord;
		::memset(&kmRecord, 0, sizeof(kmRecord));
		
		// заголовок записи о километре
		kmRecord.absBegin = kmList[kmIdx].base;
		kmRecord.absEnd = kmList[kmIdx].end;
		kmRecord.section.dwSize = sizeof(kmRecord.section);
		kmRecord.section.start = aprInfo.GetRealCoord(kmRecord.absBegin);
		kmRecord.section.end = aprInfo.GetRealCoord(kmRecord.absEnd);
		kmRecord.km = kmList[kmIdx].km;
		kmRecord.pk = 0;

		// временный список пикетов на км, он нам пригодится отдельно
		std::vector<VStatData> tmpPkList;

		// обходим пикеты
		std::vector<VPicket> pkList = aprInfo.GetPkList(kmRecord.absBegin, kmRecord.absEnd, false);
		for (size_t pkIdx = 0; pkIdx < pkList.size(); ++pkIdx)
		{
			VStatData pkRecord;
			::memset(&pkRecord, 0, sizeof(pkRecord));

			// заголовок пикета
			pkRecord.absBegin = pkList[pkIdx].absStart;
			pkRecord.absEnd = pkList[pkIdx].absEnd;
			pkRecord.section.dwSize = sizeof(pkRecord.section);
			pkRecord.section.start = aprInfo.GetRealCoord(pkRecord.absBegin);
			pkRecord.section.end = aprInfo.GetRealCoord(pkRecord.absEnd);
			pkRecord.km = pkList[pkIdx].km;
			pkRecord.pk = pkList[pkIdx].pk;

			// индексы 
			size_t ndxFrom, ndxTo;
			double absFrom, absTo;

			// мо и сско по точкам
			double pntAbsCoord = pkRecord.absBegin + 10;
			while (pntAbsCoord <= pkRecord.absEnd)
			{
				ndxFrom = absCoord.FindIndex(pntAbsCoord - 50.0, true);
				if (ndxFrom+1 < absCoord.GetCount()) ++ndxFrom;
				absFrom = absCoord.GetItem(ndxFrom);

				ndxTo = absCoord.FindIndex(pntAbsCoord + 50.0, true);
				absTo = absCoord.GetItem(ndxTo);

				// проверка на исключенность точки
				for (size_t i = 0; i < excluded.size(); ++i)
				{
					if (absTo <= excluded[i].absStart || absFrom >= excluded[i].absEnd)
						continue;

					++pkRecord.pntEx;
					break;
				}

				// расчет статистики
				if (pntAbsCoord - absFrom > 49 && absTo - pntAbsCoord > 49)
				{
					float mo, sko;

					// учитываем точки в пикетной записи
					mo = width.GetStatMO(ndxFrom, ndxTo);
					sko = width.GetStatSKO(ndxFrom, ndxTo, mo);
					pkRecord.moWidth += mo;
					pkRecord.sskoWidth += sko;

					mo = tmp_level.GetStatMO(ndxFrom, ndxTo);
					float level_sko = tmp_level.GetStatSKO(ndxFrom, ndxTo, mo);
					pkRecord.moLevel += mo;
					pkRecord.sskoLevel += level_sko;

					mo = tmp_diff.GetStatMO(ndxFrom, ndxTo);
					float diff_sko = tmp_diff.GetStatSKO(ndxFrom, ndxTo, mo);
					pkRecord.moLevelDiff += mo;
					pkRecord.sskoLevelDiff += diff_sko;

					mo = pitLeft.GetStatMO(ndxFrom, ndxTo);
					float pl_sko = pitLeft.GetStatSKO(ndxFrom, ndxTo, mo);
					pkRecord.moPitLeft += mo;
					pkRecord.sskoPitLeft += pl_sko;

					mo = pitRight.GetStatMO(ndxFrom, ndxTo);
					float pr_sko = pitRight.GetStatSKO(ndxFrom, ndxTo, mo);
					pkRecord.moPitRight += mo;
					pkRecord.sskoPitRight += pr_sko;

					mo = richLeft.GetStatMO(ndxFrom, ndxTo);
					sko = richLeft.GetStatSKO(ndxFrom, ndxTo, mo);
					pkRecord.moRichLeft += mo;
					pkRecord.sskoRichLeft += sko;

					mo = richRight.GetStatMO(ndxFrom, ndxTo);
					sko = richRight.GetStatSKO(ndxFrom, ndxTo, mo);
					pkRecord.moRichRight += mo;
					pkRecord.sskoRichRight += sko;

					if (havePPR)
					{
						pkRecord.havePPR = true;

						mo = prLeft.GetStatMO(ndxFrom, ndxTo);
						sko = prLeft.GetStatSKO(ndxFrom, ndxTo, mo);
						pkRecord.moPrLeft += mo;
						pkRecord.sskoPrLeft += sko;

						mo = prRight.GetStatMO(ndxFrom, ndxTo);
						sko = prRight.GetStatSKO(ndxFrom, ndxTo, mo);
						pkRecord.moPrRight += mo;
						pkRecord.sskoPrRight += sko;

						mo = ngrLeft.GetStatMO(ndxFrom, ndxTo);
						sko = ngrLeft.GetStatSKO(ndxFrom, ndxTo, mo);
						pkRecord.moNgrLeft += mo;
						pkRecord.sskoNgrLeft += sko;

						mo = ngrRight.GetStatMO(ndxFrom, ndxTo);
						sko = ngrRight.GetStatSKO(ndxFrom, ndxTo, mo);
						pkRecord.moNgrRight += mo;
						pkRecord.sskoNgrRight += sko;
					}

					int iPitLeft = 1, iPitRight = 1, iLevelDiff = 1, iLevel = 1;
					float sigma = fget_sigma(pl_sko, pr_sko, diff_sko, level_sko,
						iPitLeft, iPitRight, iLevelDiff, iLevel);

					pkRecord.sigma += sigma;
					pkRecord.sssp += fget_sssp(sigma);

					++pkRecord.pntCalc;
				}

				++pkRecord.pntSum;
				pntAbsCoord += 20.0;
			}

			// макс и мин отклонения от нулевой линии
			ndxFrom = absCoord.FindIndex(pkRecord.absBegin, true);
			ndxTo = absCoord.FindIndex(pkRecord.absEnd, true);
			for (size_t i = ndxFrom; i < ndxTo; ++i)
			{
				pkRecord.maxLevel = std::max<float>(pkRecord.maxLevel, tmp_zldiff.GetItem(i));
				pkRecord.minLevel = std::min<float>(pkRecord.minLevel, tmp_zldiff.GetItem(i));
			}
			
			if (pkRecord.pntCalc > 0)
			{
				// учитываем точки в километровой записи
				kmRecord.moWidth += pkRecord.moWidth;
				kmRecord.sskoWidth += pkRecord.sskoWidth;
				kmRecord.moLevel += pkRecord.moLevel;
				kmRecord.sskoLevel += pkRecord.sskoLevel;
				kmRecord.moLevelDiff += pkRecord.moLevelDiff;
				kmRecord.sskoLevelDiff += pkRecord.sskoLevelDiff;
				kmRecord.moPitLeft += pkRecord.moPitLeft;
				kmRecord.sskoPitLeft += pkRecord.sskoPitLeft;
				kmRecord.moPitRight += pkRecord.moPitRight;
				kmRecord.sskoPitRight += pkRecord.sskoPitRight;
				kmRecord.moRichLeft += pkRecord.moRichLeft;
				kmRecord.sskoRichLeft += pkRecord.sskoRichLeft;
				kmRecord.moRichRight += pkRecord.moRichRight;
				kmRecord.sskoRichRight += pkRecord.sskoRichRight;

				kmRecord.havePPR = pkRecord.havePPR;
				kmRecord.moPrLeft += pkRecord.moPrLeft;
				kmRecord.sskoPrLeft += pkRecord.sskoPrLeft;
				kmRecord.moPrRight += pkRecord.moPrRight;
				kmRecord.sskoPrRight += pkRecord.sskoPrRight;
				kmRecord.moNgrLeft += pkRecord.moNgrLeft;
				kmRecord.sskoNgrLeft += pkRecord.sskoNgrLeft;
				kmRecord.moNgrRight += pkRecord.moNgrRight;
				kmRecord.sskoNgrRight += pkRecord.sskoNgrRight;

				// мо и ско на пикете
				pkRecord.moWidth /= pkRecord.pntCalc;
				pkRecord.sskoWidth /= pkRecord.pntCalc;
				pkRecord.moLevel /= pkRecord.pntCalc;
				pkRecord.sskoLevel /= pkRecord.pntCalc;
				pkRecord.moLevelDiff /= pkRecord.pntCalc;
				pkRecord.sskoLevelDiff /= pkRecord.pntCalc;
				pkRecord.moPitLeft /= pkRecord.pntCalc;
				pkRecord.sskoPitLeft /= pkRecord.pntCalc;
				pkRecord.moPitRight /= pkRecord.pntCalc;
				pkRecord.sskoPitRight /= pkRecord.pntCalc;
				pkRecord.moRichLeft /= pkRecord.pntCalc;
				pkRecord.sskoRichLeft /= pkRecord.pntCalc;
				pkRecord.moRichRight /= pkRecord.pntCalc;
				pkRecord.sskoRichRight /= pkRecord.pntCalc;

				pkRecord.moPrLeft /= pkRecord.pntCalc;
				pkRecord.sskoPrLeft /= pkRecord.pntCalc;
				pkRecord.moPrRight /= pkRecord.pntCalc;
				pkRecord.sskoPrRight /= pkRecord.pntCalc;
				pkRecord.moNgrLeft /= pkRecord.pntCalc;
				pkRecord.sskoNgrLeft /= pkRecord.pntCalc;
				pkRecord.moNgrRight /= pkRecord.pntCalc;
				pkRecord.sskoNgrRight /= pkRecord.pntCalc;

				// сигма и сссп на пикете
				pkRecord.sigma /= pkRecord.pntCalc;
				pkRecord.sssp /= pkRecord.pntCalc;

				// сигма и сссп на километре
				kmRecord.sigma += pkRecord.sigma * pkRecord.pntCalc;
				kmRecord.sssp += pkRecord.sssp * pkRecord.pntCalc;
			}

			kmRecord.pntCalc += pkRecord.pntCalc;
			kmRecord.pntSum += pkRecord.pntSum;
			kmRecord.pntEx += pkRecord.pntEx;
			tmpPkList.push_back(pkRecord);
		}
		if (kmRecord.pntCalc > 0) 
		{
			// километровые мо, ско, сигма, сссп
			kmRecord.moWidth /= kmRecord.pntCalc;
			kmRecord.sskoWidth /= kmRecord.pntCalc;
			kmRecord.moLevel /= kmRecord.pntCalc;
			kmRecord.sskoLevel /= kmRecord.pntCalc;
			kmRecord.moLevelDiff /= kmRecord.pntCalc;
			kmRecord.sskoLevelDiff /= kmRecord.pntCalc;
			kmRecord.moPitLeft /= kmRecord.pntCalc;
			kmRecord.sskoPitLeft /= kmRecord.pntCalc;
			kmRecord.moPitRight /= kmRecord.pntCalc;
			kmRecord.sskoPitRight /= kmRecord.pntCalc;
			kmRecord.moRichLeft /= kmRecord.pntCalc;
			kmRecord.sskoRichLeft /= kmRecord.pntCalc;
			kmRecord.moRichRight /= kmRecord.pntCalc;
			kmRecord.sskoRichRight /= kmRecord.pntCalc;
			kmRecord.sigma /= kmRecord.pntCalc;
			kmRecord.sssp /= kmRecord.pntCalc;

			kmRecord.moPrLeft /= kmRecord.pntCalc;
			kmRecord.sskoPrLeft /= kmRecord.pntCalc;
			kmRecord.moPrRight /= kmRecord.pntCalc;
			kmRecord.sskoPrRight /= kmRecord.pntCalc;
			kmRecord.moNgrLeft /= kmRecord.pntCalc;
			kmRecord.sskoNgrLeft /= kmRecord.pntCalc;
			kmRecord.moNgrRight /= kmRecord.pntCalc;
			kmRecord.sskoNgrRight /= kmRecord.pntCalc;

			// поправка на "разбалтывание" попикетного сссп в приделах километра
			// заодно - макс и мин отклонений от уровня
			double sum = 0;
			size_t counter = 0;
			for (size_t pkNdx = 0; pkNdx < tmpPkList.size(); ++pkNdx)
			{
				if (tmpPkList[pkNdx].pntCalc == 0) continue;
				sum += Square(tmpPkList[pkNdx].sssp - kmRecord.sssp);
				++counter;

				if (pkNdx == 0)
				{
					kmRecord.maxLevel = tmpPkList[pkNdx].maxLevel;
					kmRecord.minLevel = tmpPkList[pkNdx].minLevel;
				}
				else
				{
					kmRecord.maxLevel = std::max<float>(kmRecord.maxLevel, tmpPkList[pkNdx].maxLevel);
					kmRecord.minLevel = std::min<float>(kmRecord.minLevel, tmpPkList[pkNdx].minLevel);
				}
			}
			if (counter > 1)
			{
				kmRecord.sssp -= static_cast<float>(sqrt(sum / (counter - 1)) / 2.0);
			}

			this->GetKmList().AddItem(kmRecord);
			for (size_t pkNdx = 0; pkNdx < tmpPkList.size(); ++pkNdx)
			{
				if (tmpPkList[pkNdx].pntCalc > 0)
					this->GetPkList().AddItem(tmpPkList[pkNdx]);
			}
		}
	}

	this->message = ERRMSG_OK;
	return true;
}
