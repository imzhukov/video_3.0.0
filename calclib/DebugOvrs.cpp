#include "DebugOvrs.h"
#include "VKmResult.h"
#include "strlib.h"
#include "dblib.h"
#include "revision.h"
#include "VHtml.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

std::ostream & operator << (std::ostream & os, const VSpeeds & spd)
{
	os << spd.format_short();
	return os;
}


// ���������� ��������� �����������
std::wstring OvrHTML(const VFileName & fileName, 
					  const VFileName & reportFile)
{
	VDataSet dataSet;

	if (!dataSet.OpenFile(fileName))
	{
		return L"�� �������� ��������� ���� " + fileName.GetPath();
	}

    /// ����������� ������ 3
    if (dataSet.Load(PRM_OVRS3))
    {
        const VOvr3List & overs = dataSet.GetOvrs3();
    	std::ofstream report(reportFile.GetPath().c_str());
	    if (report.bad()) 
    	{
	    	return L"�� ������� ������� ���� ������";
    	}

    	// ������������� ����� ������ ����� �������
	    report << std::fixed;
    	report << std::setprecision(0);

    	std::string title = "����������� (v.3)";
    	report << VHtml::GetHeader(title);

	    report << "<h3>" << dataSet.GetFileName().GetPath() << "<br>" << title << "</h3>" << std::endl;
    	report << "<p><strong>���� �������:</strong> " << dataSet.GetRailDate().ToStringLocale() << "<br>" << std::endl;
	    report << "<strong>������� �����:</strong> " << dataSet.GetSection().start << " - ";
    	report << dataSet.GetSection().end << "</p>" << std::endl << std::endl;
    	
        report << "<table><tr><th>�-��</th><th>���</th><th>��������</th>" << std::endl;
        report << "<th>�����/� ����</th><th>�������</th><!--<th>�����</th>-->" << std::endl;
        report << "<th>����.���</th><th>����.���</th><th>����������</th>" << std::endl;
        report << "</thead><tbody>" << std::endl;

        for (size_t i = 0; i < overs.GetCount(); ++i)
        {
            if (i > 0 && overs.GetItem(i).km != overs.GetItem(i-1).km)
            {
                report << "</tbody><tbody>" << std::endl;
            }

            report << "<tr><td>" << VWayCoord(overs.GetItem(i).km, overs.GetItem(i).m) << "</td>" << std::endl;
            report << "<td>" << wstring_to_string(overs.GetItem(i).type) << "</td>" << std::endl;
            report << "<td>" << overs.GetItem(i).value << "</td>" << std::endl;

            report << "<td>" << overs.GetItem(i).length;
            if (std::wstring(overs.GetItem(i).type) == WIDEN ||
                std::wstring(overs.GetItem(i).type) == NARROW ||
                std::wstring(overs.GetItem(i).type) == LEVEL)
            {
                report << " (" << overs.GetItem(i).amount << "";
            }
            report << "</td>" << std::endl;

            report << "<td>" << overs.GetItem(i).degree << "</td>" << std::endl;
            
            VSpeeds prk(overs.GetItem(i).spd1, overs.GetItem(i).spd2, overs.GetItem(i).spd3);
            report << "<td>" << prk  << "</td>" << std::endl;

            VSpeeds lim(overs.GetItem(i).rst1, overs.GetItem(i).rst2, overs.GetItem(i).rst3);
            if (lim < prk)  report << "<td>" << lim  << "</td>" << std::endl;
            else            report << "<td>" << VSpeeds()  << "</td>" << std::endl;
            report << "<td>" << wstring_to_string(overs.GetItem(i).comment) << "</td></tr>" << std::endl;
        }

        report << std::endl << "</tbody></table>" << std::endl;
        report << VHtml::GetFooter();
        report.close();
        return L"";
    }
    
    /// ����������� ������ 3
	else if (dataSet.Load(PRM_OVRS2))
	{
        const VOvr2List & overs = dataSet.GetOvrs2();

        std::ofstream report(reportFile.GetPath().c_str());
        if (report.bad()) 
        {
            return L"�� ������� ������� ���� ������";
        }
        // ������������� ����� ������ ����� �������
        report << std::fixed;
        report << std::setprecision(0);

        std::string title = "����������� (v.2)";
        report << VHtml::GetHeader(title);

        report << "<h3>" << dataSet.GetFileName().GetPath() << "<br>" << title << "</h3>" << std::endl;
        report << "<p><strong>���� �������:</strong> " << dataSet.GetRailDate().ToStringLocale() << "<br>" << std::endl;
        report << "<strong>������� �����:</strong> " << dataSet.GetSection().start << " - ";
        report << dataSet.GetSection().end << "</p>" << std::endl << std::endl;

        report << "<table><thead><tr>" << std::endl;
        report << "<th>�-��</th><th>���</th><th>��������</th>" << std::endl;
        report << "<th>�����/� ����</th><th>�������</th><!--<th>�����</th>-->" << std::endl;
        report << "<th>����.���</th><th>����.���</th><th>����������</th>" << std::endl;
        report << "</thead><tbody>" << std::endl;

        for (size_t i = 0; i < overs.GetCount(); ++i)
        {
            if (i > 0 && overs.GetItem(i).wayCoord.km != overs.GetItem(i-1).wayCoord.km)
            {
                report << "</tbody><tbody>" << std::endl;
            }

            if (GetOverstepName(overs.GetItem(i).type) == "" || 
                overs.GetItem(i).degree < 2 || overs.GetItem(i).degree > 4)
            {
                continue;
            }

            report << "<tr><td>" << overs.GetItem(i).wayCoord << "</td>" << std::endl;
            report << "<td>" << GetOverstepName(overs.GetItem(i).type) << "</td>" << std::endl;
            report << "<td>" << overs.GetItem(i).value << "</td>" << std::endl;

            report << "<td>" << overs.GetItem(i).len;
            if (overs.GetItem(i).type == ovrWidthPl || overs.GetItem(i).type == ovrWidthMn ||
                overs.GetItem(i).type == ovrLevel)
            {
                report << " (" << overs.GetItem(i).numElem << "";
            }
            report << "</td>" << std::endl;

            report << "<td>" << overs.GetItem(i).degree << "</td>" << std::endl;
            // report << "<td>" << overs.GetItem(i).GetPenalty() << "</td>" << std::endl;
            report << "<td>" << overs.GetItem(i).setSpd << "</td>" << std::endl;

            report << "<td>";
            if (overs.GetItem(i).IsLimit())
            {
                report << overs.GetItem(i).limSpd;
            }
            report << "</td>" << std::endl;

            report << "<td>";
            if (overs.GetItem(i).code.C)		report << "� ";
            if (overs.GetItem(i).code.PP)		report << "�� ";
            if (overs.GetItem(i).code.ThreePr)	report << "3�� ";
            if (overs.GetItem(i).code.PrjRich)	report << "���� ";
            if (overs.GetItem(i).code.PrjP)		report << "���� ";
            if (overs.GetItem(i).code.PrjPitR)	report << "�����.� ";
            if (overs.GetItem(i).code.PrjPitL)	report << "�����.� ";
            if (overs.GetItem(i).code.PrjRP)	report << "����+� ";
            if (overs.GetItem(i).code.PrjRPitR)	report << "����+��.� ";
            if (overs.GetItem(i).code.PrjRPitL)	report << "����+��.� ";
            if (overs.GetItem(i).code.Bridge)	report << "���� ";
            if (overs.GetItem(i).code.Tonnel)	report << "������� ";
            if (overs.GetItem(i).code.Switch)	report << "������� ";
            if (overs.GetItem(i).type == ovrWidthPl || overs.GetItem(i).type == ovrWidthMn)
            {
                report << "���=" << overs.GetItem(i).norma;
            }
            report << "</td></tr>" << std::endl;
        }

        report << std::endl << "</tbody></table>" << std::endl;
        report << VHtml::GetFooter();
        report.close();
        return L"";
	}
	
    else
    {
        return L"� ����� ����������� �����������";
    }
}

// ��-32
std::wstring PU32HTML(const VFileName & fileName, 
					  const VFileName & reportFile)
{
	VDataSet dataSet;

	if (!dataSet.OpenFile(fileName))
	{
		return L"�� �������� ��������� ���� " + fileName.GetPath();
	}

	if (dataSet.Load(PRM_OVRS2))
	{
        const VOvr2List & overs = dataSet.GetOvrs2();

        std::ofstream report(reportFile.GetPath().c_str());
        if (report.bad()) 
        {
            return L"�� ������� ������� ���� ������";
        }
        // ������������� ����� ������ ����� �������
        report << std::fixed;

        // ������
        report.imbue(std::locale(""));

        std::string title = "�������� ������ ������� ������� (��-32)";
        report << VHtml::GetHeader(title);

        report << "<h3>" << dataSet.GetFileName().GetPath() << "<br>" << title << "<br>����������� (v.2)" << "</h3>" << std::endl;
        report << "<p><strong>���� �������:</strong> " << dataSet.GetRailDate().ToStringLocale() << "<br>" << std::endl;
        report << "<strong>������� �����:</strong> " << dataSet.GetSection().start << " - ";
        report << dataSet.GetSection().end << "</p>" << std::endl << std::endl;

        report << "<table><thead>" << std::endl;
        report << "<tr><th rowspan=2>� ��</th>" << std::endl;
        report << "<th colspan=7>���-�� �����������<br>II/III �������</th>" << std::endl;
        report << "<th rowspan=2>���-��<br>��������-<br>������</th>" << std::endl;
        report << "<th rowspan=2>��������<br>������<br>��/���</th>" << std::endl;
        report << "<th rowspan=2>������-<br>������<br>������<br>��/���</th>" << std::endl;
        report << "<th rowspan=2>����������</th>" << std::endl;
        report << "</tr><tr>" << std::endl;
        report << "<th>���</th><th>��</th><th>�</th>" << std::endl;
        report << "<th>�</th><th>��</th><th>�</th><th>�����</th>" << std::endl;
        report << "</thead><tbody>" << std::endl << std::endl;

        std::set<int32_t> kmList = overs.GetKmList();
        for (std::set<int32_t>::const_iterator it = kmList.begin(); it != kmList.end(); ++it)
        {
            report << overs.GetPU32String(*it);
        }

        report << std::endl << "</tbody></table>" << std::endl;
        report << VHtml::GetFooter();
        report.close();
        return L"";
    }
    
    else if (dataSet.Load(PRM_OVRS3))
    {
        if (!dataSet.Load(PRM_ABS_COORD))
        {
            return dataSet.GetErrorMessage();
        }
        
        if (!dataSet.Load(PRM_APRINFO))
        {
            return dataSet.GetErrorMessage();
        }
    
       	VKmResultList kms;
		kms.CreateKmList(dataSet.GetAbsCoord(), dataSet.GetAprinfo());
		for (size_t i = 0; i < dataSet.GetOvrs3().GetCount(); ++i)
		{
            kms.ApplyOverstep(dataSet.GetAprinfo(), dataSet.GetOvrs3().GetItem(i));		
        }    
        
        for (size_t i = 0; i < kms.GetCount(); ++i)
        {
            if (kms.GetItem(0).GetCP515Revision() == 1)
                kms.GetItem(i).ApplyOldStylePenalty();
        }

        std::ofstream report(reportFile.GetPath().c_str());
        if (report.bad()) 
        {
            return L"�� ������� ������� ���� ������";
        }
        // ������������� ����� ������ ����� �������
        report << std::fixed;

        // ������
        report.imbue(std::locale(""));

        std::string title = "�������� ������ ������� ������� (��-32)";
        report << VHtml::GetHeader(title);

        report << "<h3>" << dataSet.GetFileName().GetPath() << "<br>" << title << "<br>����������� (v.3)" << "</h3>" << std::endl;
        report << "<p><strong>���� �������:</strong> " << dataSet.GetRailDate().ToStringLocale() << "<br>" << std::endl;
        report << "<strong>������� �����:</strong> " << dataSet.GetSection().start << " - ";
        report << dataSet.GetSection().end << "</p>" << std::endl << std::endl;

        report << "<table><thead>" << std::endl;
        report << "<tr><th rowspan=2>� ��</th>" << std::endl;
        report << "<th colspan=7>���-�� �����������<br>II/III �������</th>" << std::endl;
        report << "<th rowspan=2>���-��<br>��������-<br>������</th>" << std::endl;
        report << "<th rowspan=2>��������<br>������</th>" << std::endl;
        report << "<th rowspan=2>������-<br>������<br>������</th>" << std::endl;
        report << "<th rowspan=2>����������</th>" << std::endl;
        report << "</tr><tr>" << std::endl;
        report << "<th>���</th><th>��</th><th>�</th>" << std::endl;
        report << "<th>�</th><th>��</th><th>�</th><th>�����</th>" << std::endl;
        report << "</thead><tbody>" << std::endl << std::endl;

        for (size_t i = 0; i < kms.GetCount(); ++i)
        {
            VKmResult & km = kms.GetItem(i);
        
            report << "<tr><th>" << km.GetKm() << "</th>";
            
            report << "<td>" <<km.GetOvrAmount(NARROW, 2) << "/" << km.GetOvrAmount(NARROW, 3) << "</td>";
            report << "<td>" << km.GetOvrAmount(WIDEN, 2) << "/" << km.GetOvrAmount(WIDEN, 3) << "</td>";
            report << "<td>" << km.GetOvrAmount(LEVEL, 2) << "/" << km.GetOvrAmount(LEVEL, 3) << "</td>";
            report << "<td>" << km.GetOvrAmount(DISTORTION, 2) << "/" << km.GetOvrAmount(DISTORTION, 3) << "</td>";
            
            report << "<td>" << km.GetOvrAmount(PIT_LEFT, 2) + km.GetOvrAmount(PIT_RIGHT, 2);
            report << "/" << km.GetOvrAmount(PIT_LEFT, 3) + km.GetOvrAmount(PIT_RIGHT, 3)  << "</td>";

            report << "<td>" << km.GetOvrAmount(RICH, 2) + km.GetOvrAmount(RICH_NR, 2) +
		            km.GetOvrAmount(RICH_LEFT, 2) + km.GetOvrAmount(RICH_RIGHT, 2);
		    
            report << "/" << km.GetOvrAmount(RICH, 3) + km.GetOvrAmount(RICH_NR, 3) +
		            km.GetOvrAmount(RICH_LEFT, 3) + km.GetOvrAmount(RICH_RIGHT, 3) << "</td>";

            report << "<td>" << km.GetCP515Sum2() << "/" << km.GetCP515Sum3() << "</td>";

            if (kms.GetItem(0).GetCP515Revision() == 1)
            {
                report << "<td>" << km.GetCP515Restrictions() << "</td>";
            }
            else
            {
                report << "<td>" << km.GetRestrictions() << "</td>";
            }
            
            report << "<td>" << km.GetPenalty() << "</td>";
            report << "<td>" << km.GetMark() << "</td>";
            report << "<td>" << wstring_to_string(km.GetComment()) << "</td></tr>";
        }
        
        report << std::endl << "</tbody></table>" << std::endl;
        report << VHtml::GetFooter();
        report.close();
        return L"";
    }
    
    else return L"� ����� ����������� �����������";
}

