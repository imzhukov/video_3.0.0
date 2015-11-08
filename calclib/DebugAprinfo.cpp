#include "VHtml.h"
#include "DebugAprinfo.h"

std::wstring AprinfoExportHTML(const VFileName & fileName, 
                               const VFileName & reportFile, 
							   VIBDatabasePtr dataBase)
{
	VProgressBox prog(1, "������� ��������� ���������� � ������� HTML", false);
	prog.SetMessage(0, "���������� ������");

   	VDataSet dataSet;
	if (!dataSet.OpenFile(fileName))
	{
		return L"�� �������� ��������� ���� " + fileName.GetPath();
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
	
    std::ofstream report(reportFile.GetPath().c_str());
	if (report.bad()) 
	{
		return L"�� ������� ������� ���� ������";
	}
	report << std::fixed;
	report.imbue(std::locale(""));
	report << dataSet.GetAprinfo().GetReport(true, false);
	report.close();
	return L"";
}