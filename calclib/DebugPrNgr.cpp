#include "DebugPrNgr.h"
#include "VProgressInterface.h"
#include "VDataSet.h"
#include "VHtml.h"

#include <iomanip>

std::wstring PrNgrExportHTML(const VFileName & fileName, 
							 const VFileName & reportFile, 
							 VIBDatabasePtr dataBase)
{

	VProgressBox prog(0, "������� �������� ��� �� �������� ����� �������/����������", false);
	prog.SetMessage(0, "���������� ������");

	// ����
	VDataSet dataSet;
	if (!dataSet.OpenFile(fileName))
	{
		return L"�� �������� ��������� ���� " + fileName.GetPath();
	}

	// ���.�-��
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

	prog.SetMessage(0, "�������� ��/���");
	if (!dataSet.Load(PRM_PR_NGR_AREAS))
	{
		return L"�� ������� ��������� ��/���";
	}

	prog.SetMessage(0, "������� ������");

	// �����
	std::ofstream report(reportFile.GetPath().c_str());
	if (report.bad()) 
	{
		return L"�� ������� ������� ���� ������";
	}
	
	// ������������� ����� ������ ����� �������
	report << std::fixed;

	// ������
	report.imbue(std::locale(""));

	// ���������
	report << VHtml::GetHeader("�������� ��/��� �� �������� ����");

	report << "<h3>" << dataSet.GetFileName().GetPath();
	report << "<br>���������� ��������� �������� ��� �� �������� ����� �������/����������</h3>" << std::endl;
	report << "<p><strong>���� �������:</strong> " << dataSet.GetRailDate().ToStringLocale() << "<br>" << std::endl;
	report << "<strong>������� �����:</strong> " << dataSet.GetSection().start << " - ";
	report << dataSet.GetSection().end << "</p>" << std::endl << std::endl;

	report << "<table>" << std::endl;
	report << "<colgroup span=2></colgroup>" << std::endl; 
	report << "<colgroup span=3></colgroup>" << std::endl; 
	report << "<colgroup span=5></colgroup>" << std::endl; 
	report << "<colgroup span=5></colgroup>" << std::endl; 
	report << "<colgroup span=5></colgroup>" << std::endl; 
	report << "<colgroup span=5></colgroup>" << std::endl; 

	report << "<thead><tr><th colspan=2>ranges</th><th colspan=3>area descr</th>";
	report << "<th colspan=5>pr.left</th><th colspan=5>pr.right</th>";
	report << "<th colspan=5>ngr.left</th><th colspan=5>ngr.right</th></tr>" << std::endl;
	report << "<tr><th>start</th><th>end</th><th>rails<br>type</th><th>rails<br>brace</th><th>max<br>spd1</th>";
	report << "<th>min</th><th>avg</th><th>max</th><th>&lt;1/60</th><th>&gt;1/12</th>";
	report << "<th>min</th><th>avg</th><th>max</th><th>&lt;1/60</th><th>&gt;1/12</th>";
	report << "<th>min</th><th>avg</th><th>max</th><th>&lt;1/60</th><th>&gt;1/12</th>";
	report << "<th>min</th><th>avg</th><th>max</th><th>&lt;1/60</th><th>&gt;1/12</th></tr></thead>";
	report << std::endl << "<tbody>" << std::endl;

	for (size_t i = 0; i < dataSet.GetPrNgrAreas().GetCount(); ++i)
	{
		const VPrNgrAreaDescr & area = dataSet.GetPrNgrAreas().GetItem(i);

		report << std::setprecision(0);	
		report << "<tr>" << std::endl;
		report << "<td>" << area.section.start << "</td>" << std::endl;
		report << "<td>" << area.section.end << "</td>" << std::endl;
		report << "<td>" << area.railsType << "</td>" << std::endl;
		report << "<td>" << area.railsBrace << "</td>" << std::endl;
		report << "<td>" << area.maxSpeed1 << "</td>" << std::endl;
		report << std::setprecision(2);	
		report << "<td>" << area.prLeftMin << "</td>";
		report << "<td>" << area.prLeftAvg << "</td>";
		report << "<td>" << area.prLeftMax << "</td>";
		report << "<td>" << area.lenPrLeftUnderEdge << "</td>";
		report << "<td>" << area.lenPrLeftOverEdge << "</td>";
		report << "<td>" << area.prRightMin << "</td>";
		report << "<td>" << area.prRightAvg << "</td>";
		report << "<td>" << area.prRightMax << "</td>";
		report << "<td>" << area.lenPrRightUnderEdge << "</td>";
		report << "<td>" << area.lenPrRightOverEdge << "</td>";
		report << "<td>" << area.ngrLeftMin << "</td>";
		report << "<td>" << area.ngrLeftAvg << "</td>";
		report << "<td>" << area.ngrLeftMax << "</td>";
		report << "<td>" << area.lenNgrLeftUnderEdge << "</td>";
		report << "<td>" << area.lenNgrLeftOverEdge << "</td>";
		report << "<td>" << area.ngrRightMin << "</td>";
		report << "<td>" << area.ngrRightAvg << "</td>";
		report << "<td>" << area.ngrRightMax << "</td>";
		report << "<td>" << area.lenNgrRightUnderEdge << "</td>";
		report << "<td>" << area.lenNgrRightOverEdge << "</td>";
		report << "</tr>" << std::endl;
	}
	report << "</tbody></table>";

	report << VHtml::GetFooter();
	report.close();

	return L"";
}