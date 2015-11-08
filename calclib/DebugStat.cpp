#include "DebugStat.h"
#include "strlib.h"
#include "dblib.h"
#include "revision.h"
#include "VCurveCard.h"
#include "VHtml.h"

#include "VProgressInterface.h"

#include <fstream>
#include <sstream>
#include <iomanip>

#include "Shiny.h"

/// ���������� ��������� ������
std::wstring StatExportHTML(STATDEBUG_REPORT repType,
							const VFileName & fileName, 
							const VFileName & reportFile, 
							VIBDatabasePtr dataBase)
{
	VProgressBox prog(1, 
		repType == STATREP_KM ? "������� �������������� ���������� � ������� HTML" : "������� ���������� ���������� � ������� HTML",
		false);

	prog.SetMessage(0, "���������� ������");


	VDataSet dataSet;

	if (!dataSet.OpenFile(fileName))
	{
		return L"�� �������� ��������� ���� " + fileName.GetPath();
	}

	if (!dataSet.Load(PRM_ABS_COORD))
	{
		return L"�� ������� ��������� ���������� ����������";
	}

	bool useDb = dataBase != NULL && dataBase->Connected();

	// ������� �� ����
	if (useDb)
	{
		prog.SetMessage(0, "��������� ��������� ���������� �� ����");

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

	// ������� �� �����
	else
	{
		prog.SetMessage(0, "�������� ��������� ���������� �� �����");
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

	prog.SetMessage(0, "�������� ����������");
	if (!dataSet.Load(PRM_STATISTICS))
	{
		return L"�� ������� ��������� ����������";
	}

	prog.SetMessage(0, "������� ������");
	std::ofstream report(reportFile.GetPath().c_str());
	if (report.bad()) 
	{
		return L"�� ������� ������� ���� ������";
	}
	// ������������� ����� ������ ����� �������
	report << std::fixed;

	// ������
	report.imbue(std::locale(""));
	std::string title = repType == STATREP_KM ? "�������������� ����������" : "���������� ����������";
	report << VHtml::GetHeader(title);

	report << "<h3>" << dataSet.GetFileName().GetPath() << "<br>" << title << "<h3>" << std::endl;
	report << "<p><strong>���� �������:</strong> " << dataSet.GetRailDate().ToStringLocale() << "<br>" << std::endl;
	report << "<strong>������� �����:</strong> " << dataSet.GetSection().start << " - ";
	report << dataSet.GetSection().end << "</p>" << std::endl << std::endl;

	report << "<table>" << std::endl;
	report << "<colgroup span=" << (repType == STATREP_PK ? 2 : 1) << "></colgroup>" << std::endl;
	report << "<colgroup span=3></colgroup>" << std::endl;
	report << "<colgroup span=4></colgroup>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl;

	report << "<thead><tr>" << std::endl;
	report << "<th rowspan=2>��</th>";
	if (repType == STATREP_PK) report << "<th rowspan=2>��</th>";
	report << std::endl;
	report << "<th rowspan=2>������</th><th rowspan=2>�����</th><th rowspan=2>�����</th>" << std::endl;
	report << "<th colspan=3>����� ���� �����</th>" << std::endl;
	report << "<th rowspan=2>������</th>" << std::endl;
	report << "<th colspan=2>�������</th>" << std::endl;
	report << "<th colspan=2>�������</th>" << std::endl;
	report << "<th colspan=2>���� �� ��� �����</th>" << std::endl;
	report << "<th colspan=2>�� ���</th>" << std::endl;
	report << "<th colspan=2>�� ����</th>" << std::endl;
	report << "<th colspan=2>���� ���</th>" << std::endl;
	report << "<th colspan=2>���� ����</th>" << std::endl;
	report << "<th colspan=2>��</th>" << std::endl;
	report << "<th colspan=2>������ ���</th>" << std::endl;
	report << "<th colspan=2>������ ����</th>" << std::endl;
	report << "<th colspan=2>���� �� ���</th>" << std::endl;
	report << "<th colspan=2>���� �� ����</th>" << std::endl;
	report << "<th rowspan=2>�����</th>" << std::endl;
	report << "<th rowspan=2>����</th>" << std::endl;
	report << "</tr><tr>" << std::endl;
	report << "<th>���</th><th>����</th><th>������</th>" << std::endl;
	report << "<th>��</th><th>����</th>" << std::endl;	
	report << "<th>��</th><th>����</th>" << std::endl;	
	report << "<th>���</th><th>����</th>" << std::endl;
	report << "<th>��</th><th>����</th>" << std::endl;	
	report << "<th>��</th><th>����</th>" << std::endl;	
	report << "<th>��</th><th>����</th>" << std::endl;	
	report << "<th>��</th><th>����</th>" << std::endl;	
	report << "<th>��</th><th>����</th>" << std::endl;	
	report << "<th>��</th><th>����</th>" << std::endl;	
	report << "<th>��</th><th>����</th>" << std::endl;	
	report << "<th>��</th><th>����</th>" << std::endl;	
	report << "<th>��</th><th>����</th>" << std::endl;	
	report << "</tr></thead>" << std::endl << std::endl;

	const VStatList & data = repType == STATREP_PK ? 
		dataSet.GetStatistics().GetPkList() : dataSet.GetStatistics().GetKmList();

	report << "<tbody>" << std::endl;
	for (size_t i = 0; i < data.GetCount(); ++i)
	{
		if (repType == STATREP_PK && i > 0 && data.GetItem(i).km != data.GetItem(i-1).km)
		{
			report << "</tbody><tbody>" << std::endl;
		}

		const VStatData & item = data.GetItem(i);
		if (item.pntCalc == 0) continue;

		report << "<tr><th>" << item.km << "</th>" << std::endl;
		if (repType == STATREP_PK) report << "<th>" << item.pk << "</th>" << std::endl;
		report << "<td>" << item.section.start << "</td>" << std::endl;
		report << "<td>" << item.section.end << "</td>" << std::endl;
		report << std::setprecision(0);
		report << "<td>" << item.absEnd - item.absBegin << "</td>" << std::endl;
		report << "<td>" << item.pntSum << "</td>" << std::endl;
		report << "<td>" << item.pntCalc << "</td>" << std::endl;
		report << "<td>" << item.pntEx << "</td>" << std::endl;

		report << "<td>";
		switch(item.GetStatus())
		{
		case SAS_EXCLUDED:	report << "���������"; break;
		case SAS_NO_DATA:	report << "��� ���������"; break;
		case SAS_OK:		report << "&nbsp;"; break;
		}
		report << "</td>" << std::endl;

		report << std::setprecision(2);
		report << "<td>" << item.moLevel << "</td>" << std::endl;
		report << "<td>" << item.sskoLevel << "</td>" << std::endl;
		report << "<td>" << item.moLevelDiff << "</td>" << std::endl;
		report << "<td>" << item.sskoLevelDiff << "</td>" << std::endl;
		report << "<td>" << item.minLevel << "</td>" << std::endl;
		report << "<td>" << item.maxLevel << "</td>" << std::endl;
		report << "<td>" << item.moPitLeft << "</td>" << std::endl;
		report << "<td>" << item.sskoPitLeft << "</td>" << std::endl;
		report << "<td>" << item.moPitRight << "</td>" << std::endl;
		report << "<td>" << item.sskoPitRight << "</td>" << std::endl;
		report << "<td>" << item.moRichLeft << "</td>" << std::endl;
		report << "<td>" << item.sskoRichLeft << "</td>" << std::endl;
		report << "<td>" << item.moRichRight << "</td>" << std::endl;
		report << "<td>" << item.sskoRichRight << "</td>" << std::endl;
		report << "<td>" << item.moWidth << "</td>" << std::endl;
		report << "<td>" << item.sskoWidth << "</td>" << std::endl;
		
		if (item.havePPR)
		{
			report << "<td>" << item.moPrLeft << "</td>" << std::endl;
			report << "<td>" << item.sskoPrLeft << "</td>" << std::endl;
			report << "<td>" << item.moPrRight << "</td>" << std::endl;
			report << "<td>" << item.sskoPrRight << "</td>" << std::endl;
			report << "<td>" << item.moNgrLeft << "</td>" << std::endl;
			report << "<td>" << item.sskoNgrLeft << "</td>" << std::endl;
			report << "<td>" << item.moNgrRight << "</td>" << std::endl;
			report << "<td>" << item.sskoNgrRight << "</td>" << std::endl;
		}
		else
		{
			report << "<td> - </td>" << std::endl;
			report << "<td> - </td>" << std::endl;
			report << "<td> - </td>" << std::endl;
			report << "<td> - </td>" << std::endl;
			report << "<td> - </td>" << std::endl;
			report << "<td> - </td>" << std::endl;
			report << "<td> - </td>" << std::endl;
			report << "<td> - </td>" << std::endl;
		}

		report << "<td>" << item.sigma << "</td>" << std::endl;

		report << std::setprecision(0);
		report << "<td>" << item.sssp << "</td></tr>" << std::endl;
	}
	report << "</tbody></table>" << std::endl;
	report << VHtml::GetFooter();

	report.close();
	return L"";
}


