#include "DebugCurves.h"
#include "strlib.h"
#include "dblib.h"
#include "revision.h"
#include "VCurveCard.h"
#include "VHtml.h"
#include "options.h"
#include "VProgressInterface.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

#include "Shiny.h"

std::ostream & operator << (std::ostream & os, const std::vector<VCurveSpdArea> & speeds)
{
	std::string str;
	for (size_t i = 0; i < speeds.size(); ++i)
	{
		if (i != 0) str += "\\";
		char buf[32] = {0};
		_snprintf(buf, 32, "%li", speeds[i].spd);
		str += buf;
	}
	os << str;
	return os;
}

// Отладочная ведомость кривых
std::wstring CurveExportHTML(CRVDEBUG_REPORT repType,
							 const VFileName & fileName, 
							 const VFileName & reportFile, 
							 VIBDatabasePtr dataBase)
{
    float experimental_anp = EXPERIMENTAL_ANP().Value();

	VProgressBox prog(1, 
		repType == CRVREP_MODEL ? "Экспорт модельных кривых в таблицу HTML" : "Экспорт кривых в таблицу HTML",
		false);

	prog.SetMessage(0, "Подготовка данных");


	VDataSet dataSet;

	if (!dataSet.OpenFile(fileName))
	{
		return L"Не доступен локальный файл " + fileName.GetPath();
	}

	//if (!dataSet.OpenFile(*dataBase, fileName.GetShortName()))
	//{
	//	return L"Не доступен удаленный файл " + fileName.GetShortName();
	//}

	if (!dataSet.Load(PRM_ABS_COORD))
	{
		return L"Не удалось загрузить абсолютную координату";
	}

	if (!dataSet.Load(repType == CRVREP_MODEL ? PRM_MODEL_CURVES : PRM_CURVE_MARKS))
	{
		return L"Не удалось загрузить разметку кривых";
	}

	std::wstring message = dataSet.GetCurves(repType == CRVREP_MODEL ? PRM_MODEL_CURVES : PRM_CURVE_MARKS).GetErrorMessage();

	for (size_t position = message.find(L"\n", 0); 
		position != std::wstring::npos;
		position = message.find(L"\n", position+1))
	{
		message.replace(position, 1, L"<br>");
	}

	bool useDb = dataBase != NULL && dataBase->Connected();

	// апринфо из базы
	if (useDb)
	{
		prog.SetMessage(0, "Получение априорной информации из базы");

		try
		{
			if (!dataSet.Makeinfo(dataBase, false, false))
			{
				return dataSet.GetAprinfo().GetErrorMessage();
			}
		}
		catch (std::exception& e)
		{
			return string_to_wstring(string(e.what()));
		}
	}

	// апринфо из файла
	else
	{
		prog.SetMessage(0, "Загрузка априорной информации из файла");
		try
		{
			if (!dataSet.AprinfoLoadFromFile())
			{
				return dataSet.GetAprinfo().GetErrorMessage();
			}
		}
		catch (std::exception& e)
		{
			return string_to_wstring(string(e.what()));
		}
	}
	VAprinfo * aprInfo = &dataSet.GetAprinfo();

	std::ofstream report(reportFile.GetPath().c_str());
	if (report.bad()) 
	{
		return L"Не удалось открыть файл отчета";
	}
	// фиксированное число знаков после запятой
	report << std::fixed;

	// локаль
	report.imbue(std::locale(""));

	std::string title;
	switch(repType)
	{
	case CRVREP_CURVES:		title = "Кривые"; break;
	case CRVREP_STCURVES:	title = "Кривые на станционных путях"; break;
	case CRVREP_MODEL:		title = "Модельные кривые"; break;
	case CRVREP_PK:			title = "Попикетный износ на кривых"; break;
	case CRVREP_FPO:		title = "Участки ФПО на кривых"; break;
	case CRVREP_WEARDGR:	title = "Распределение износа на кривых"; break;
	case CRVREP_LINKS:		title = "Позвеньевой износ на кривых"; break;
	}
	report << VHtml::GetHeader(title);

	report << "<h3>" << dataSet.GetFileName().GetPath() << "<br>" << title << "</h3>" << std::endl;
	report << "<p><strong>Дата поездки:</strong> " << dataSet.GetRailDate().ToStringLocale() << "<br>" << std::endl;
	report << "<strong>Границы файла:</strong> " << dataSet.GetSection().start << " - ";
	report << dataSet.GetSection().end;

    if (repType == CRVREP_PK || repType == CRVREP_LINKS || repType == CRVREP_WEARDGR)
    {
        report << "<br><strong>Источник износов:</strong> ";
        report << wstring_to_string(OPTIONS.GetOption(L"WEAR_SOURCE").GetValue());
    }
    
    if (repType == CRVREP_CURVES && experimental_anp > 0)
    {
        report << "<br><strong>Экспериментальная Vкр для порога Анп:</strong> ";
        report << experimental_anp;
    }

	report << "</p>" << std::endl << std::endl << "<table>" << std::endl;

	if (repType == CRVREP_CURVES || repType == CRVREP_STCURVES)
	{
		report << "<colgroup span=6></colgroup><!-- coord -->" << std::endl; 
		report << "<colgroup span=9></colgroup><!-- geometry -->" << std::endl;
		report << "<colgroup span=10></colgroup><!-- width -->" << std::endl;
		report << "<colgroup span=4></colgroup><!-- otvods -->" << std::endl;
		report << "<colgroup span=2></colgroup><!-- v prk -->" << std::endl;
		report << "<colgroup span=11></colgroup><!-- anp + psi -->" << std::endl;
		report << "<colgroup span=10></colgroup><!-- speeds -->" << std::endl;
		report << "<colgroup span=2></colgroup><!-- v prk high -->" << std::endl;
		report << "<colgroup span=10></colgroup><!-- anp + psi high -->" << std::endl;
		report << "<colgroup span=4></colgroup><!-- speeds high -->" << std::endl;
		report << "<colgroup span=7></colgroup><!-- wear -->" << std::endl;
		report << "<colgroup span=9></colgroup><!-- P -->" << std::endl;
		
		if (repType == CRVREP_CURVES && experimental_anp > 0)
		{
		    report << "<colgroup span=1></colgroup><!-- v rk exp -->" << std::endl;
		}

		report << "<thead><tr>" << std::endl;
		report << "<th rowspan=2>No.</th>" << std::endl;
		report << "<th rowspan=2>ориент</th>" << std::endl;
		report << "<th colspan=2>границы план</th>" << std::endl;
		report << "<th colspan=2>границы ур</th>" << std::endl;

		report << "<th rowspan=2>угол</th>" << std::endl;
		report << "<th rowspan=2>t<sub>1</sub></th>" << std::endl;
		report << "<th rowspan=2>t<sub>2</sub></th>" << std::endl;
		report << "<th colspan=3>радиус</th>" << std::endl;
		report << "<th colspan=3>возв</th>" << std::endl;

		report << "<th colspan=4>шк</th>" << std::endl;
		report << "<th colspan=6>расх шк</th>" << std::endl;

		report << "<th colspan=2>отвод план</th>" << std::endl;
		report << "<th colspan=2>отвод ур</th>" << std::endl;

		report << "<th colspan=2>V<sub>прк</sub></th>" << std::endl;

		report << "<th colspan=3>a<sub>нп</sub> пасс</th>" << std::endl;
		report << "<th colspan=2>psi пасс</th>" << std::endl;
		report << "<th colspan=2>a<sub>нп</sub> груз</th>" << std::endl;
		report << "<th colspan=2>psi груз</th>" << std::endl;
		report << "<th colspan=2>a<sub>нп</sub> груз [60]</th>" << std::endl;

		report << "<th rowspan=2>V<sub>+0.3</sub></th>" << std::endl;
		report << "<th rowspan=2>V<sub>-0.3</sub></th>" << std::endl;
		report << "<th colspan=2>V<sub>кр</sub></th>" << std::endl;
		report << "<th colspan=2>V<sub>из</sub></th>" << std::endl;
		report << "<th colspan=2>V<sub>пр</sub></th>" << std::endl;
		report << "<th colspan=2>V<sub>пр рем</sub></th>" << std::endl;

		report << "<th colspan=2>V<sub>прк</sub></th>" << std::endl;

		report << "<th colspan=3>a<sub>нп</sub> вс</th>" << std::endl;
		report << "<th colspan=3>a<sub>нп</sub> вс нак</th>" << std::endl;
		report << "<th colspan=2>psi вс</th>" << std::endl;
		report << "<th colspan=2>psi вс нак</th>" << std::endl;

		report << "<th colspan=2>V<sub>кр</sub></th>" << std::endl;
		report << "<th colspan=2>V<sub>из</sub></th>" << std::endl;

		report << "<th colspan=7>износ</th>" << std::endl;

		report << "<th colspan=2>план</th>" << std::endl;
		report << "<th colspan=2>ур</th>" << std::endl;
		report << "<th colspan=2>a<sub>нп</sub></th>" << std::endl;
		report << "<th colspan=2>коорд</th>" << std::endl;
		report << "<th rowspan=2>P<sub>сум</sub></th>";

		if (repType == CRVREP_CURVES && experimental_anp > 0)
		{
		    report << "<th rowspan=2>V<sub>кр</sub> эксп</th>";
        }

		report << "</tr>" << std::endl;

		report << "<tr><th>нач</th><th>кон</th>" << std::endl; // Ranges.Plan
		report << "<th>нач</th><th>кон</th>" << std::endl; // Ranges.Level
		report << "<th>мин</th><th>ср</th><th>макс</th>" << std::endl; // Radius
		report << "<th>мин</th><th>ср</th><th>макс</th>" << std::endl; // Height

		report << "<th>мин</th><th>ср</th><th>макс</th><th>% сбоев</th>" << std::endl; // Width
		report << "<th>перед ср</th><th>перед макс</th><th>% сбоев</th><th>задн ср</th><th>задн макс</th><th>% сбоев</th>" << std::endl; // Width.Offset
		
		report << "<th>ср</th><th>макс</th>" << std::endl; // O.Plan
		report << "<th>ср</th><th>макс</th>" << std::endl; // O.Level

		report << "<th>пасс</th><th>груз</th>" << std::endl; // V.Prk

		report << "<th>ср</th><th>макс</th><th>коорд</th>" << std::endl; // Anp.Pass
		report << "<th>макс</th><th>коорд</th>" << std::endl; // Psi
		report << "<th>ср</th><th>макс</th>" << std::endl; // Anp.Truck
		report << "<th>макс</th><th>коорд</th>" << std::endl; // Psi
		report << "<th>ср</th><th>макс</th>" << std::endl; // Anp.Truck [60]

		report << "<th>пасс</th><th>груз</th>" << std::endl; // V.Kr
		report << "<th>пасс</th><th>груз</th>" << std::endl; // V.Iz
		report << "<th>пасс</th><th>груз</th>" << std::endl; // V.Pr
		report << "<th>пасс</th><th>груз</th>" << std::endl; // V.Pr.Repair

		report << "<th>вс</th><th>вс нак</th>" << std::endl; // V.Prk.High

		report << "<th>ср</th><th>макс</th><th>коорд</th>" << std::endl; // Anp.High
		report << "<th>ср</th><th>макс</th><th>коорд</th>" << std::endl; // Anp.High.BT
		report << "<th>макс</th><th>коорд</th>" << std::endl; // Psi.High
		report << "<th>макс</th><th>коорд</th>" << std::endl; // Psi.High.BT

		report << "<th>вс</th><th>вс нак</th>" << std::endl; // V.Kr.High
		report << "<th>вс</th><th>вс нак</th>" << std::endl; // V.Iz.High

		report << "<th>&gt;6</th><th>&gt;10</th><th>&gt;15</th>";
		report << "<th>ср</th><th>макс</th><th>корр</th><th>% сбоев</th>" << std::endl; // Wear 

		report << "<th>P</th><th>D</th>" << std::endl; // Plan
		report << "<th>P</th><th>D</th>" << std::endl; // Level
		report << "<th>P</th><th>D</th>" << std::endl; // Anp
		report << "<th>P</th><th>D</th>" << std::endl; // Coord
	}

	else if (repType == CRVREP_MODEL)
	{
		report << "<colgroup span=6></colgroup>" << std::endl; 
		report << "<colgroup span=4></colgroup>" << std::endl;
		report << "<colgroup span=2></colgroup>" << std::endl;
		report << "<colgroup span=5></colgroup>" << std::endl;
		report << "<colgroup span=4></colgroup>" << std::endl;
		report << "<colgroup span=5></colgroup>" << std::endl;
		report << "<colgroup span=6></colgroup>" << std::endl;
		report << "<colgroup span=4></colgroup>" << std::endl;
		report << "<colgroup span=1></colgroup>" << std::endl;

		report << "<thead><tr>" << std::endl;
		report << "<th rowspan=2>No.</th>" << std::endl;
		report << "<th rowspan=2>ориент</th>" << std::endl;
		report << "<th colspan=2>границы план</th>" << std::endl;
		report << "<th colspan=2>границы ур</th>" << std::endl;
		report << "<th rowspan=2>угол</th>" << std::endl;
		report << "<th rowspan=2>радиус</th>" << std::endl;
		report << "<th colspan=2>возв</th>" << std::endl;
		report << "<th rowspan=2>отвод план</th>" << std::endl;
		report << "<th rowspan=2>отвод ур</th>" << std::endl;
		report << "<th colspan=5>a<sub>нп</sub></th>" << std::endl;
		report << "<th colspan=4>psi макс</th>" << std::endl;
		report << "<th colspan=5>V<sub>расч</sub></th>" << std::endl;
		report << "<th colspan=2>V<sub>кр</sub></th>" << std::endl;
		report << "<th colspan=3>V<sub>из</sub></th>" << std::endl;
		report << "<th rowspan=2>V<sub>пр</sub></th>" << std::endl;
		report << "<th colspan=2>откл план</th>" << std::endl; // отклонение в плане
		report << "<th colspan=2>откл ур</th>" << std::endl; // отклонение на уровне
		report << "<th rowspan=2>статус</th></tr>" << std::endl; // status

		report << "<tr><th>начало</th><th>конец</th>" << std::endl; // Ranges.Plan
		report << "<th>начало</th><th>конец</th>" << std::endl; // Ranges.Level
		report << "<th>пасп</th><th>факт</th>"; // height
		report << "<th>пасс</th><th>груз</th><th>расч</th><th>вс</th><th>вс нк</th>"; // anp
		report << "<th>пасс</th><th>груз</th><th>вс</th><th>вс нк</th>"; // psi
		report << "<th>пасс</th><th>груз</th><th>расч</th><th>вс</th><th>вс нк</th>" << std::endl; // V.Calc
		report << "<th>пасс</th><th>вс</th>" << std::endl; // Vkr
		report << "<th>пасс</th><th>груз</th><th>вс</th>" << std::endl; // Viz
		report << "<th>сред</th><th>макс</th><th>сред</th><th>макс</th>" << std::endl; // Отклонение в плане и на уровне
	}

	else if (repType == CRVREP_PK)
	{
		report << "<colgroup span=5></colgroup>" << std::endl; 
		report << "<colgroup span=3></colgroup>" << std::endl; 
		report << "<colgroup span=3></colgroup>" << std::endl; 
		report << "<colgroup span=2></colgroup>" << std::endl; 
		report << "<colgroup span=3></colgroup>" << std::endl; 

		report << "<thead><tr>" << std::endl;
		report << "<th rowspan=2>No.</th>" << std::endl;
		report << "<th rowspan=2>км</th><th rowspan=2>пк</th>";
		report << "<th rowspan=2>начало</th><th rowspan=2>конец</th>" << std::endl;
		report << "<th colspan=3>износ</th><th colspan=3>шк</th><th colspan=2>Vпрк</th><th colspan=3>Vогр</th></tr>" << std::endl;
		report << "<tr><th>ср</th><th>макс</th><th>% сбоев</th>" << std::endl;
		report << "<th>ср</th><th>макс</th><th>% сбоев</th>" << std::endl;
		report << "<th>пасс</th><th>груз</th>" << std::endl;
		report << "<th>пасс</th><th>груз</th><th>нтд</th>" << std::endl;
	}

	else if (repType == CRVREP_LINKS)
	{
		report << "<colgroup span=1></colgroup>" << std::endl; 
		report << "<colgroup span=3></colgroup>" << std::endl; 
		report << "<colgroup span=3></colgroup>" << std::endl; 
		report << "<colgroup span=3></colgroup>" << std::endl; 
		report << "<colgroup span=3></colgroup>" << std::endl; 

		report << "<thead><tr>" << std::endl;
		report << "<th rowspan=2>No.</th>" << std::endl;
		report << "<th rowspan=2>начало</th><th rowspan=2>конец</th><th rowspan=2>длина</th>" << std::endl;
		report << "<th colspan=3>износ</th><th colspan=3>шк</th><th colspan=3>V</th></tr>" << std::endl;
		report << "<tr><th>ср</th><th>макс</th><th>% сбоев</th>" << std::endl;
		report << "<th>ср</th><th>макс</th><th>% сбоев</th>" << std::endl;
		report << "<th>пасс</th><th>груз</th><th>нтд</th>" << std::endl;
	}

	else if (repType == CRVREP_FPO)
	{
		report << "<colgroup span=1></colgroup>" << std::endl; 
		report << "<colgroup span=2></colgroup>" << std::endl; 
		report << "<colgroup span=1></colgroup>" << std::endl; 
		report << "<colgroup span=3></colgroup>" << std::endl; 
		report << "<colgroup span=2></colgroup>" << std::endl; 
		report << "<colgroup span=2></colgroup>" << std::endl; 
		report << "<colgroup span=2></colgroup>" << std::endl; 

		report << "<thead><tr>" << std::endl;
		report << "<th rowspan=2>No.</th>" << std::endl;
		report << "<th rowspan=2>начало</th><th rowspan=2>конец</th>" << std::endl;
		report << "<th rowspan=2>V<sub>прк</sub></th>" << std::endl;
		report << "<th rowspan=2>V<sub>кр</sub></th><th rowspan=2>V<sub>пр</sub></th>";
		report << "<th rowspan=2>V<sub>из</sub></th>" << std::endl;
		report << "<th colspan=2>a<sub>нп</sub></th><th colspan=2>psi</th><th colspan=2>отв возв</th></tr>" << std::endl;
		report << "<tr><th>макс</th><th>коорд</th><th>макс</th><th>коорд</th>" << std::endl;
		report << "<th>макс</th><th>коорд</th>" << std::endl;
	}

	else if (repType ==  CRVREP_WEARDGR)
	{
		report << "<colgroup span=1></colgroup>" << std::endl; 
		report << "<colgroup span=2></colgroup>" << std::endl; 
		report << "<colgroup span=2></colgroup>" << std::endl; 
		report << "<colgroup span=3></colgroup>" << std::endl; 

		report << "<thead><tr>" << std::endl;
		report << "<th>No.</th>" << std::endl;
		report << "<th>начало</th><th>конец</th>" << std::endl;
		report << "<th>износ ср</th><th>износ макс</th>" << std::endl;
		report << "<th>от</th><th>до</th><th>метров</th>" << std::endl;
	}
	
	report << "</tr></thead>" << std::endl;

	VCurveList * curves = repType == CRVREP_MODEL ? &dataSet.GetCurves(PRM_MODEL_CURVES) : &dataSet.GetCurves(PRM_CURVE_MARKS);
	size_t crvCount = curves->GetCount();
	prog.SetRange(0, crvCount);
	prog.SetPosition(0, 0);
	for (size_t crvNdx = 0; crvNdx < crvCount; ++crvNdx)
	{
		std::wstringstream msg;
		msg << L"Экспорт кривой кривой " << crvNdx+1 << L" из " << crvCount;
		prog.SetMessage(0, msg.str().c_str());
		LOG_DEBUG(msg.str());
		prog.Step(0);

		const VCurve * curv = &curves->GetItem(crvNdx);
		bool hs = false, hs_bt = false;

		// проверка разметки на кривой
		if (!curv->CheckMarks())
		{
			continue;
		}

		// кривая вписывается в границы участка, согласно апринфо
		if (!curv->FitsWayRanges(dataSet))
		{
			continue;
		}

		if (repType == CRVREP_CURVES || repType == CRVREP_STCURVES || repType == CRVREP_MODEL)
		{
			size_t radiusCount = curv->GetRadiusCount();
			report << std::endl << "<tbody><tr class=curve><th>" << crvNdx+1 << "</th>";

			long orient = curv->GetOrientation(dataSet);
			report << "<td>" << (orient < 0 ? "левая" : orient > 0 ? "правая" : "неизвестно") << "</td>";

			report << "<td>" << aprInfo->GetRealCoord(curv->GetRangeCoord(dataSet, true, false, true)) << "</td>";
			report << "<td>" << aprInfo->GetRealCoord(curv->GetRangeCoord(dataSet, true, false, false)) << "</td>";
			report << "<td>" << aprInfo->GetRealCoord(curv->GetRangeCoord(dataSet, false, false, true)) << "</td>";
			report << "<td>" << aprInfo->GetRealCoord(curv->GetRangeCoord(dataSet, false, false, false)) << "</td>";
			report << std::endl;

			report << std::setprecision(2);
			report << "<td>" << curv->GetAngle(dataSet) << "</td>";

			if (repType == CRVREP_CURVES || repType == CRVREP_STCURVES)
			{
				report << "<td>" << curv->GetTan1(dataSet) << "</td>";
				report << "<td>" << curv->GetTan2(dataSet) << "</td>";

				report << std::setprecision(0);
				report << "<td>" << curv->GetMinRadius(dataSet) << "</td>";
				report << "<td>" << curv->GetAvgRadius(dataSet) << "</td>";
				report << "<td>" << curv->GetMaxRadius(dataSet) << "</td>" << std::endl;
				report << "<td>" << curv->GetMinLevel(dataSet) << "</td>";
				report << "<td>" << curv->GetAvgLevel(dataSet) << "</td>";
				report << "<td>" << curv->GetMaxLevel(dataSet) << "</td>" << std::endl;

				report << "<td>" << curv->GetMinWidth(dataSet) << "</td>";
				report << "<td>" << curv->GetAvgWidth(dataSet) << "</td>";
				report << "<td>" << curv->GetMaxWidth(dataSet) << "</td>";
				report << std::setprecision(2);
				report << "<td>" << curv->GetWidthErrorPercent(dataSet) << "</td>" << std::endl;
				report << "<td>" << curv->GetAvgDeltaWidthFront(dataSet) << "</td>";
				report << "<td>" << curv->GetMaxDeltaWidthFront(dataSet) << "</td>";
				report << "<td>" << curv->GetDeltaWidthFrontErrorPercent(dataSet) << "</td>";
				report << "<td>" << curv->GetAvgDeltaWidthBack(dataSet) << "</td>";
				report << "<td>" << curv->GetMaxDeltaWidthBack(dataSet) << "</td>";
				report << "<td>" << curv->GetDeltaWidthBackErrorPercent(dataSet) << "</td>";

				report << "<td>" << curv->GetAvgPlanTap(dataSet, 0) << "</td>";
				report << "<td>" << curv->GetMaxPlanTap(dataSet, 0) << "</td>";
				report << "<td>" << curv->GetAvgLevelTap(dataSet, 0) << "</td>";

				report << "<td>";
				bool is_l = curv->IsLimitTap(dataSet, false, 0);
				bool is_lr = curv->IsLimitTap(dataSet, true, 0);
				if (is_l || is_lr) report << "<b>";
				report << curv->GetMaxLevelTap(dataSet, 0);
				if (is_l) report << "<sup>N</sup>";
				if (is_lr) report << "<sup>R</sup>";
				if (is_l || is_lr) report << "</b>";
				report << "</td>" << std::endl;

				report << "<td>" << curv->GetVprkPass(dataSet) << "</td>";
				report << "<td>" << curv->GetVprkTruck(dataSet) << "</td>";

				double maxAbsAnp = 0;
				report << "<td>" << curv->GetAvgAnpPass(dataSet) << "</td>";
				report << "<td>" << curv->GetMaxAnpPass(dataSet, &maxAbsAnp) << "</td>";
				report << "<td>" << aprInfo->GetRealCoord(maxAbsAnp) << "</td>" << std::endl;
				double maxAbsPsi = 0;
				report << "<td>" << curv->GetMaxPsiPass(dataSet, &maxAbsPsi) << "</td>";
				report << "<td>" << aprInfo->GetRealCoord(maxAbsPsi) << "</td>" << std::endl;
				report << "<td>" << curv->GetAvgAnpTruck(dataSet) << "</td>";
				report << "<td>" << curv->GetMaxAnpTruck(dataSet) << "</td>";
				maxAbsPsi = 0;
				report << "<td>" << curv->GetMaxPsiTruck(dataSet, &maxAbsPsi) << "</td>";
				report << "<td>" << aprInfo->GetRealCoord(maxAbsPsi) << "</td>" << std::endl;
				report << "<td>" << curv->GetAvgAnpThird(dataSet) << "</td>";
				report << "<td>" << curv->GetMaxAnpThird(dataSet) << "</td>";

				report << "<td>" << curv->GetVPlus03(dataSet) << "</td>";
				report << "<td>" << curv->GetVMinus03(dataSet) << "</td>" << std::endl;
				report << "<td>" << curv->GetVkrPass(dataSet) << "</td>";
				report << "<td>" << curv->GetVkrTruck(dataSet) << "</td>";
				report << "<td>" << curv->GetVizPass(dataSet) << "</td>";
				report << "<td>" << curv->GetVizTruck(dataSet) << "</td>";
				    
				report << "<td>" << curv->GetVprPass(dataSet, false, COMPLETE_CURVE) << "</td>" << std::endl;
				report << "<td>" << curv->GetVprTruck(dataSet, false, COMPLETE_CURVE) << "</td>" << std::endl;
				report << "<td>" << curv->GetVprPass(dataSet, true, COMPLETE_CURVE) << "</td>" << std::endl;
				report << "<td>" << curv->GetVprTruck(dataSet, true, COMPLETE_CURVE) << "</td>" << std::endl;

				hs = curv->HasVprkHigh(dataSet);
				hs_bt = curv->HasVprkHighBT(dataSet);

				if (hs) report << "<td>" << curv->GetVprkHigh(dataSet) << "</td>";
				else report << "<td>-</td>";

				if (hs_bt) report << "<td>" << curv->GetVprkHighBT(dataSet) << "</td>";
				else report << "<td>-</td>";

				if (hs)
				{
					report << "<td>" << curv->GetAvgAnpHigh(dataSet) << "</td>";
					maxAbsAnp = 0;
					report << "<td>" << curv->GetMaxAnpHigh(dataSet, &maxAbsAnp) << "</td>";
					report << "<td>" << aprInfo->GetRealCoord(maxAbsAnp) << "</td>" << std::endl;
				}
				else
				{
					report << "<td>-</td><td>-</td><td>-</td>" << std::endl;
				}

				if (hs_bt)
				{
					report << "<td>" << curv->GetAvgAnpHighBT(dataSet) << "</td>";
					maxAbsAnp = 0;
					report << "<td>" << curv->GetMaxAnpHighBT(dataSet, &maxAbsAnp) << "</td>";
					report << "<td>" << aprInfo->GetRealCoord(maxAbsAnp) << "</td>" << std::endl;
				}
				else
				{
					report << "<td>-</td><td>-</td><td>-</td>" << std::endl;
				}
				
				if (hs)
				{
					maxAbsPsi = 0;
					report << "<td>" << curv->GetMaxPsiHigh(dataSet, &maxAbsPsi) << "</td>";
					report << "<td>" << aprInfo->GetRealCoord(maxAbsPsi) << "</td>" << std::endl;
				}
				else
				{
					report << "<td>-</td><td>-</td>" << std::endl;
				}

				if (hs_bt)
				{
					maxAbsPsi = 0;
					report << "<td>" << curv->GetMaxPsiHighBT(dataSet, &maxAbsPsi) << "</td>";
					report << "<td>" << aprInfo->GetRealCoord(maxAbsPsi) << "</td>" << std::endl;
				}
				else
				{
					report << "<td>-</td><td>-</td>" << std::endl;
				}

				report << "<td>" << curv->GetVkrHigh(dataSet) << "</td>";
				report << "<td>" << curv->GetVkrHighBT(dataSet) << "</td>";
				report << "<td>" << curv->GetVizHigh(dataSet) << "</td>";
				report << "<td>" << curv->GetVizHighBT(dataSet) << "</td>" << std::endl;

				report << std::setprecision(0);
				report << "<td>" << curv->GetWearLenOverLimit(dataSet, 6.0f) << "</td>";
				report << "<td>" << curv->GetWearLenOverLimit(dataSet, 10.0f) << "</td>";
				report << "<td>" << curv->GetWearLenOverLimit(dataSet, 15.0f) << "</td>" << std::endl;
				report << std::setprecision(2);
				report << "<td>" << curv->GetAvgWear(dataSet) << "</td>";
				report << "<td>" << curv->GetMaxWear(dataSet) << "</td>";
				report << "<td>" << curv->GetCorrWear(dataSet) << "</td>";
				report << "<td>" << curv->GetWearErrorPercent(dataSet) << "</td>" << std::endl;

				report << "<td>" << curv->GetPRPlan(dataSet) << "</td><td>" << curv->GetDegreePlan(dataSet) << "</td>";
				report << "<td>" << curv->GetPRLevel(dataSet) << "</td><td>" << curv->GetDegreeLevel(dataSet) << "</td>";
				report << "<td>" << curv->GetPRAnp(dataSet) << "</td><td>" << curv->GetDegreeAnp(dataSet) << "</td>";
				report << "<td>" << curv->GetPRCoord(dataSet) << "</td><td>" << curv->GetDegreeCoord(dataSet) << "</td>";
				report << "<td>" << curv->GetPRSum(dataSet) << "</td>" << std::endl;
				
				if (repType == CRVREP_CURVES && experimental_anp > 0)
				{
				    report << "<td>" << curv->GetMidVkr(dataSet, experimental_anp) << "</td>";
				}
				
				report << "</tr>" << std::endl;
			}

			else if (repType == CRVREP_MODEL)
			{
				report << std::setprecision(0);
				report << "<td>" << curv->GetAvgRadius(dataSet) << "</td>";
				report << "<td>" << curv->GetAvgLevel(dataSet) << "</td>";
				report << "<td>" << curv->GetAvgFactLevel(dataSet) << "</td>";
				report << std::setprecision(2);
				report << "<td>" << curv->GetAvgPlanTap(dataSet, 0) << "</td>";
				report << "<td>" << curv->GetAvgLevelTap(dataSet, 0) << "</td>";

				report << "<td>" << curv->GetAvgAnpPass(dataSet) << "</td>";
				report << "<td>" << curv->GetAvgAnpTruck(dataSet) << "</td>";
				report << "<td>" << curv->GetAvgAnpThird(dataSet) << "</td>";

				if (curv->HasVprkHigh(dataSet))
					report << "<td>" << curv->GetAvgAnpHigh(dataSet) << "</td>";
				else report << "<td>-</td>";

				if (curv->HasVprkHighBT(dataSet))
					report << "<td>" << curv->GetAvgAnpHighBT(dataSet) << "</td>";
				else report << "<td>-</td>";

				report << "<td>" << curv->GetMaxPsiPass(dataSet, NULL) << "</td>";
				report << "<td>" << curv->GetMaxPsiTruck(dataSet, NULL) << "</td>";

				if (curv->HasVprkHigh(dataSet))
					report << "<td>" << curv->GetMaxPsiHigh(dataSet, NULL) << "</td>";
				else report << "<td>-</td>";

				if (curv->HasVprkHighBT(dataSet))
					report << "<td>" << curv->GetMaxPsiHighBT(dataSet, NULL) << "</td>";
				else report << "<td>-</td>";

				report << "<td>" << curv->GetVprkPass(dataSet) << "</td>";
				report << "<td>" << curv->GetVprkTruck(dataSet) << "</td>";
				report << "<td>" << curv->GetVprkThird(dataSet) << "</td>";
				report << "<td>" << curv->GetVprkHigh(dataSet) << "</td>";
				report << "<td>" << curv->GetVprkHighBT(dataSet) << "</td>";

				report << "<td>" << curv->GetVkrPass(dataSet) << "</td>";
				report << "<td>" << curv->GetVkrHigh(dataSet) << "</td>";

				report << "<td>" << curv->GetVizPass(dataSet) << "</td>";
				report << "<td>" << curv->GetVizTruck(dataSet) << "</td>";
				report << "<td>" << curv->GetVizHigh(dataSet) << "</td>";

				report << "<td>" << curv->GetVprPass(dataSet, false, COMPLETE_CURVE) << "</td>" << std::endl;

				report << "<td>" << curv->GetAvgPlanOffset(dataSet) << "</td>";
				report << "<td>" << curv->GetMaxPlanOffset(dataSet) << "</td>";
				report << "<td>" << curv->GetAvgLevelOffset(dataSet) << "</td>";
				report << "<td>" << curv->GetMaxLevelOffset(dataSet) << "</td>";

				report << "<td rowspan=" << radiusCount+1 << ">";
				report << curv->GetModelStatusDescription();

				const char * comment = curv->GetModelComment();
				if (comment != NULL && comment[0] != '\0')
				{
					report << " [" << comment << "]</td></tr>";
				}
				report << std::endl;
			}

			for (size_t radiusNdx = 0; radiusNdx < radiusCount; ++radiusNdx)
			{
				report << "<tr class=radius><th><small>[" << radiusNdx+1 << "]</small></th>";
				report << "<td></td>";

				report << "<td>" << aprInfo->GetRealCoord(curv->GetRangeCoord(dataSet, true, true, true, radiusNdx)) << "</td>";
				report << "<td>" << aprInfo->GetRealCoord(curv->GetRangeCoord(dataSet, true, true, false, radiusNdx)) << "</td>";
				report << "<td>" << aprInfo->GetRealCoord(curv->GetRangeCoord(dataSet, false, true, true, radiusNdx)) << "</td>";
				report << "<td>" << aprInfo->GetRealCoord(curv->GetRangeCoord(dataSet, false, true, false, radiusNdx)) << "</td>" << std::endl;

				report << std::setprecision(2);
				report << "<td>" << curv->GetAngle(dataSet, radiusNdx) << "</td>";
				report << std::setprecision(0);

				if (repType == CRVREP_MODEL)
				{
					report << "<td>" << curv->GetAvgRadius(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetAvgLevel(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetAvgFactLevel(dataSet, radiusNdx) << "</td>";
					report << std::setprecision(2);
					report << "<td>" << curv->GetAvgPlanTap(dataSet, radiusNdx+1) << "</td>";
					report << "<td>" << curv->GetAvgLevelTap(dataSet, radiusNdx+1) << "</td>";
					report << "<td>" << curv->GetAvgAnpPass(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetAvgAnpTruck(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetAvgAnpThird(dataSet, radiusNdx) << "</td>";
					
					if (curv->HasVprkHigh(dataSet))
						report << "<td>" << curv->GetAvgAnpHigh(dataSet, radiusNdx) << "</td>";
					else report << "<td>-</td>";

					if (curv->HasVprkHighBT(dataSet))
						report << "<td>" << curv->GetAvgAnpHighBT(dataSet, radiusNdx) << "</td>";
					else report << "<td>-</td>";

					report << "<td>" << curv->GetMaxPsiPass(dataSet, NULL, radiusNdx) << "</td>";
					report << "<td>" << curv->GetMaxPsiTruck(dataSet, NULL, radiusNdx) << "</td>";

					if (curv->HasVprkHigh(dataSet))
						report << "<td>" << curv->GetMaxPsiHigh(dataSet, NULL, radiusNdx) << "</td>";
					else report << "<td>-</td>";

					if (curv->HasVprkHighBT(dataSet))
						report << "<td>" << curv->GetMaxPsiHighBT(dataSet, NULL, radiusNdx) << "</td>";
					else report << "<td>-</td>";

					report << "<td>" << curv->GetVprkPass(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetVprkTruck(dataSet, radiusNdx) << "</td>";
				}
				else
				{
					report << "<td></td><td></td>";

					report << "<td>" << curv->GetMinRadius(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetAvgRadius(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetMaxRadius(dataSet, radiusNdx) << "</td>" << std::endl;
					report << "<td>" << curv->GetMinLevel(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetAvgLevel(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetMaxLevel(dataSet, radiusNdx) << "</td>" << std::endl;

					report << "<td>" << curv->GetMinWidth(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetAvgWidth(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetMaxWidth(dataSet, radiusNdx) << "</td>";
					report << std::setprecision(2);
					report << "<td>" << curv->GetWidthErrorPercent(dataSet, radiusNdx) << "</td>" << std::endl;
					report << "<td>" << curv->GetAvgDeltaWidthFront(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetMaxDeltaWidthFront(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetDeltaWidthFrontErrorPercent(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetAvgDeltaWidthBack(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetMaxDeltaWidthBack(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetDeltaWidthBackErrorPercent(dataSet, radiusNdx) << "</td>";

					report << "<td>" << curv->GetAvgPlanTap(dataSet, radiusNdx+1) << "</td>";
					report << "<td>" << curv->GetMaxPlanTap(dataSet, radiusNdx+1) << "</td>";
					report << "<td>" << curv->GetAvgLevelTap(dataSet, radiusNdx+1) << "</td>";
					
        			report << "<td>";
    				bool is_l = curv->IsLimitTap(dataSet, false, radiusNdx+1);
	    			bool is_lr = curv->IsLimitTap(dataSet, true, radiusNdx+1);
		    		if (is_l || is_lr) report << "<b>";
			    	report << curv->GetMaxLevelTap(dataSet, radiusNdx+1);
		    		if (is_l) report << "<sup>N</sup>";
    				if (is_lr) report << "<sup>R</sup>";
	    			if (is_l || is_lr) report << "</b>";
		    		report << "</td>" << std::endl;
		    		
					report << "<td>" << curv->GetVprkPass(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetVprkTruck(dataSet, radiusNdx) << "</td>";

					double maxAbsR = 0;
					report << "<td>" << curv->GetAvgAnpPass(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetMaxAnpPass(dataSet, &maxAbsR, radiusNdx) << "</td>";
					report << "<td>" << aprInfo->GetRealCoord(maxAbsR) << "</td>" << std::endl;
					double maxAbsPsiR = 0;
					report << "<td>" << curv->GetMaxPsiPass(dataSet, &maxAbsPsiR, radiusNdx) << "</td>";
					report << "<td>" << aprInfo->GetRealCoord(maxAbsPsiR) << "</td>" << std::endl;
					report << "<td>" << curv->GetAvgAnpTruck(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetMaxAnpTruck(dataSet, radiusNdx) << "</td>";
					maxAbsPsiR = 0;
					report << "<td>" << curv->GetMaxPsiTruck(dataSet, &maxAbsPsiR, radiusNdx) << "</td>";
					report << "<td>" << aprInfo->GetRealCoord(maxAbsPsiR) << "</td>" << std::endl;
					report << "<td>" << curv->GetAvgAnpThird(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetMaxAnpThird(dataSet, radiusNdx) << "</td>";

					report << "<td>" << curv->GetVPlus03(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetVMinus03(dataSet, radiusNdx) << "</td>" << std::endl;
				}

				if (repType == CRVREP_MODEL)
				{
					report << "<td>" << curv->GetVprkThird(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetVprkHigh(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetVprkHighBT(dataSet, radiusNdx) << "</td>";
				}

				report << "<td>" << curv->GetVkrPass(dataSet, radiusNdx) << "</td>";

				if (repType == CRVREP_MODEL)
				{
					report << "<td>" << curv->GetVkrHigh(dataSet, radiusNdx) << "</td>";
//					report << "<td>" << curv->GetVkrHighBT(dataSet, radiusNdx) << "</td>";
				}
				else
				{
					report << "<td>" << curv->GetVkrTruck(dataSet, radiusNdx) << "</td>";
				}

				report << "<td>" << curv->GetVizPass(dataSet, radiusNdx) << "</td>";
				report << "<td>" << curv->GetVizTruck(dataSet, radiusNdx) << "</td>";
				if (repType == CRVREP_MODEL)
				{
					report << "<td>" << curv->GetVizHigh(dataSet, radiusNdx) << "</td>";
				}

				report << "<td>" << curv->GetVprPass(dataSet, false, radiusNdx) << "</td>" << std::endl;

				if (repType != CRVREP_MODEL)
				{
					report << "<td>" << curv->GetVprTruck(dataSet, false, radiusNdx) << "</td>" << std::endl;
    				report << "<td>" << curv->GetVprPass(dataSet, true, radiusNdx) << "</td>" << std::endl;
	    			report << "<td>" << curv->GetVprTruck(dataSet, true, radiusNdx) << "</td>" << std::endl;
				}

				if (repType == CRVREP_MODEL)
				{
					report << "<td>" << curv->GetAvgPlanOffset(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetMaxPlanOffset(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetAvgLevelOffset(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetMaxLevelOffset(dataSet, radiusNdx) << "</td>";
				}
				else
				{
					if (hs) report << "<td>" << curv->GetVprkHigh(dataSet, radiusNdx) << "</td>";
					else report << "<td>-</td>";

					if (hs_bt) report << "<td>" << curv->GetVprkHighBT(dataSet, radiusNdx) << "</td>";
					else report << "<td>-</td>";

					if (hs)
					{
						report << "<td>" << curv->GetAvgAnpHigh(dataSet, radiusNdx) << "</td>";
						double maxAbsR = 0;
						report << "<td>" << curv->GetMaxAnpHigh(dataSet, &maxAbsR, radiusNdx) << "</td>";
						report << "<td>" << aprInfo->GetRealCoord(maxAbsR) << "</td>" << std::endl;
					}
					else
					{
						report << "<td>-</td><td>-</td><td>-</td>" << std::endl;
					}

					if (hs_bt)
					{
						report << "<td>" << curv->GetAvgAnpHighBT(dataSet, radiusNdx) << "</td>";
						double maxAbsR = 0;
						report << "<td>" << curv->GetMaxAnpHighBT(dataSet, &maxAbsR, radiusNdx) << "</td>";
						report << "<td>" << aprInfo->GetRealCoord(maxAbsR) << "</td>" << std::endl;
					}
					else
					{
						report << "<td>-</td><td>-</td><td>-</td>" << std::endl;
					}

					if (hs)
					{
						double maxAbsPsiR = 0;
						report << "<td>" << curv->GetMaxPsiHigh(dataSet, &maxAbsPsiR, radiusNdx) << "</td>";
						report << "<td>" << aprInfo->GetRealCoord(maxAbsPsiR) << "</td>" << std::endl;
					}
					else
					{
						report << "<td>-</td><td>-</td>" << std::endl;
					}

					if (hs_bt)
					{
						double maxAbsPsiR = 0;
						report << "<td>" << curv->GetMaxPsiHighBT(dataSet, &maxAbsPsiR, radiusNdx) << "</td>";
						report << "<td>" << aprInfo->GetRealCoord(maxAbsPsiR) << "</td>" << std::endl;
					}
					else
					{
						report << "<td>-</td><td>-</td>" << std::endl;
					}

					report << "<td>" << curv->GetVkrHigh(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetVkrHighBT(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetVizHigh(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetVizHighBT(dataSet, radiusNdx) << "</td>" << std::endl;

					report << std::setprecision(0);
					report << "<td>" << curv->GetWearLenOverLimit(dataSet, 6.0f, radiusNdx) << "</td>";
					report << "<td>" << curv->GetWearLenOverLimit(dataSet, 10.0f, radiusNdx) << "</td>";
					report << "<td>" << curv->GetWearLenOverLimit(dataSet, 15.0f, radiusNdx) << "</td>" << std::endl;
					report << std::setprecision(2);
					report << "<td>" << curv->GetAvgWear(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetMaxWear(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetCorrWear(dataSet, radiusNdx) << "</td>";
					report << "<td>" << curv->GetWearErrorPercent(dataSet, radiusNdx) << "</td>" << std::endl;

					report << "<td></td><td></td>";
					report << "<td></td><td></td>";
					report << "<td></td><td></td>";
					report << "<td></td><td></td><td></td>";
					
			    	if (repType == CRVREP_CURVES && experimental_anp > 0)
		    		{
	    			    report << "<td>" << curv->GetMidVkr(dataSet, experimental_anp, radiusNdx) << "</td>";
    				}
				}

				report << "</tr>" << std::endl;
			}

		} // CRVREP_CURVES || CRVREP_MODEL

		else if (repType == CRVREP_PK)
		{
			std::vector<VCurveWearPicket> pks = curv->GetWearPkList(dataSet);
			report << "<tbody><tr><th rowspan=" << pks.size() << ">" << crvNdx+1 << "</th>";
			for (size_t pkNdx = 0; pkNdx < pks.size(); ++pkNdx)
			{
				if (pkNdx != 0) report << "<tr>";
				report << "<td>" << pks[pkNdx].km << "</td>";
				report << "<td>" << pks[pkNdx].pk << "</td>";
				report << "<td>" << pks[pkNdx].section.start << "</td>";
				report << "<td>" << pks[pkNdx].section.end << "</td>";

				report << std::setprecision(2);
				report << "<td>" << pks[pkNdx].avgWear << "</td>";
				report << "<td>" << pks[pkNdx].maxWear << "</td>";
				report << std::setprecision(0);
				report << "<td>" << pks[pkNdx].errorWear << "</td>";

				report << std::setprecision(0);
				report << "<td>" << pks[pkNdx].avgWidth << "</td>";
				report << "<td>" << pks[pkNdx].maxWidth << "</td>";
				report << "<td>" << pks[pkNdx].errorWidth << "</td>";

				report << "<td>" << pks[pkNdx].prkPass << "</td>";
				report << "<td>" << pks[pkNdx].prkTruck << "</td>";
				report << "<td>" << pks[pkNdx].spdPass << "</td>";
				report << "<td>" << pks[pkNdx].spdTruck << "</td>";
				report << "<td>" << (pks[pkNdx].ntd ? "нтд" : "") << "</td>";
				report << "</tr>";
			}
			report << "</tbody>";
		}

		else if (repType == CRVREP_LINKS)
		{
			std::vector<VCurveLink> links = curv->GetLinkList(dataSet);

			report << "<tbody><tr><th rowspan=" << links.size() << ">" << crvNdx+1 << "</th>";
			for (size_t linkNdx = 0; linkNdx < links.size(); ++linkNdx)
			{
				if (linkNdx != 0) report << "<tr>";
				report << "<td>" << links[linkNdx].section.start << "</td>";
				report << "<td>" << links[linkNdx].section.end << "</td>";
				report << std::setprecision(0);
				report << "<td>" << links[linkNdx].absEnd - links[linkNdx].absStart << "</td>";

				report << std::setprecision(2);
				report << "<td>" << links[linkNdx].avgWear << "</td>";
				report << "<td>" << links[linkNdx].maxWear << "</td>";
				report << std::setprecision(0);
				report << "<td>" << links[linkNdx].errorWear << "</td>";

				report << std::setprecision(0);
				report << "<td>" << links[linkNdx].avgWidth << "</td>";
				report << "<td>" << links[linkNdx].maxWidth << "</td>";
				report << "<td>" << links[linkNdx].errorWidth << "</td>";

				report << "<td>" << links[linkNdx].spdPass << "</td>";
				report << "<td>" << links[linkNdx].spdTruck << "</td>";
				report << "<td>" << (links[linkNdx].ntd ? "нтд" : "") << "</td>";
				report << "</tr>";
			}
			report << "</tbody>";
		}

		else if (repType == CRVREP_FPO)
		{
			std::vector<VCurveFPOArea> areas = curv->GetFPOAreaList(dataSet);
			report << "<tbody><tr><th rowspan=" << areas.size() << ">" << crvNdx+1 << "</th>";
			for (size_t areaNdx = 0; areaNdx < areas.size(); ++areaNdx)
			{
				if (areaNdx != 0) report << "<tr>";
				report << "<td>" << areas[areaNdx].section.start << "</td>";
				report << "<td>" << areas[areaNdx].section.end << "</td>";

				report << "<td>" << areas[areaNdx].spdVprkPass << "</td>";
				report << "<td>" << areas[areaNdx].spdVkr << "</td>";
				report << "<td>" << areas[areaNdx].spdVpr << "</td>";
				report << "<td>" << areas[areaNdx].spdViz << "</td>";

				report << std::setprecision(2);
				report << "<td>" << areas[areaNdx].maxAnp << "</td>";
				report << "<td>" << areas[areaNdx].maxAnpWayCoord << "</td>";
				report << "<td>" << areas[areaNdx].maxPsi << "</td>";
				report << "<td>" << areas[areaNdx].maxPsiWayCoord << "</td>";
				report << "<td>" << areas[areaNdx].maxOh << "</td>";
				report << "<td>" << areas[areaNdx].maxOhWayCoord << "</td></tr>";
			}
			report << "</tbody>";
		}

		else if (repType ==  CRVREP_WEARDGR)
		{
			std::set<density_t> wearDgr = curv->GetWearDiagram(dataSet);
			if (wearDgr.empty())
			{
				report << "<tbody><tr><td>" << crvNdx+1 << "</td>";
				report << "<td>" << aprInfo->GetRealCoord(curv->GetRangeCoord(dataSet, true, false, true)) << "</td>";
				report << "<td>" << aprInfo->GetRealCoord(curv->GetRangeCoord(dataSet, true, false, false)) << "</td>";
				report << std::setprecision(2);
				report << "<td>" << curv->GetAvgWear(dataSet) << "</td>";
				report << "<td>" << curv->GetMaxWear(dataSet) << "</td>";
				report << std::endl;
				report << "<td>-</td><td>-</td><td>-</td></tr>" << std::endl;
			}
			else
			{
				report << "<tbody><tr><td rowspan=" << wearDgr.size() << ">" << crvNdx+1 << "</td>";
				report << "<td rowspan=" << wearDgr.size() << ">" << aprInfo->GetRealCoord(curv->GetRangeCoord(dataSet, true, false, true)) << "</td>";
				report << "<td rowspan=" << wearDgr.size() << ">" << aprInfo->GetRealCoord(curv->GetRangeCoord(dataSet, true, false, false)) << "</td>";

				report << std::setprecision(2);
				report << "<td rowspan=" << wearDgr.size() << ">" << curv->GetMaxWear(dataSet) << "</td>";
				report << "<td rowspan=" << wearDgr.size() << ">" << curv->GetAvgWear(dataSet) << "</td>";

				std::set<density_t>::const_iterator it = wearDgr.begin();
				for (; it != wearDgr.end(); it++)
				{
					if (it != wearDgr.begin()) report << "<tr>";
					report << std::setprecision(0);
					report << "<td>" << it->range.left_bound().value << "</td>";
					report << "<td>" << it->range.right_bound().value << "</td>";
					report << std::setprecision(1);
					report << "<td>" << it->value << "</td></tr>" << std::endl;
				}
			}
			report << "</tbody>" << std::endl << std::flush;
		}
	}
	report << "</table>" << std::endl;

	if (!message.empty())
	{
		report << "<p>" << message << "</p>" << std::endl;
	}

	// на обычных кривых заодно пишем прямые
	if (repType == CRVREP_CURVES)
	{
		if (dataSet.Load(PRM_LINES))
		{
			const VAprinfo & aprInfo = dataSet.GetAprinfo();
			const VAbsCoordData & absCoord = dataSet.GetAbsCoord();
			const VLineList & lines = dataSet.GetLines();
			const VLineMarkList & lineMarks = dataSet.GetLineMarks();

			report << "<h3>Прямые</h3>" << std::endl;
			report << "<table><thead><tr>" << std::endl;
			report << "<th>No.</th>" << std::endl;
			report << "<th>нач</th>" << std::endl;
			report << "<th>кон</th>" << std::endl;
			report << "<th>длина</th>" << std::endl;
			report << "<th>метки</th>" << std::endl;
			report << "</tr></thead><tbody>" << std::endl << std::endl;

			for (size_t lineNdx = 0; lineNdx < lines.GetCount(); ++lineNdx)
			{
				report << "<tr><th>" << lineNdx + 1 << "</th>" << std::endl;
				const VLine & line = lines.GetItem(lineNdx);

				double abs_start = absCoord.GetItem(line.GetStartIndex());
				report << "<td>" << aprInfo.GetRealCoord(abs_start) << "</td>" << std::endl;

				double abs_end = absCoord.GetItem(line.GetEndIndex());
				report << "<td>" << aprInfo.GetRealCoord(abs_end) << "</td>" << std::endl;
				report << "<td>" << abs_end - abs_start << "</td>" << std::endl;

				report << "<td>";
				for (size_t markNdx = 0; markNdx < lineMarks.GetCount(); ++markNdx)
				{	
					const VLineMark & mark = lineMarks.GetItem(markNdx);
					if (line.Fits(mark))
					{
						report << aprInfo.GetRealCoord(absCoord.GetItem(mark.index));
						report << "<br>" << std::endl;
					}
				}
				report << "</td></tr>" << std::endl;
			}
			report << "</tbody></table>" << std::endl << std::endl;
		}
		else
		{
			report << "<p>Разметка прямых не загружена</p>";
		}
	}
	
	report << VHtml::GetFooter();
	report.close();
	return L"";
}

