#include "VHtml.h"
#include "DebugAprinfo.h"

std::wstring AprinfoExportHTML(const VFileName & fileName, 
                               const VFileName & reportFile, 
							   VIBDatabasePtr dataBase)
{
	VProgressBox prog(1, "Экспорт априорной информации в таблицу HTML", false);
	prog.SetMessage(0, "Подготовка данных");

   	VDataSet dataSet;
	if (!dataSet.OpenFile(fileName))
	{
		return L"Не доступен локальный файл " + fileName.GetPath();
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
	report << std::fixed;
	report.imbue(std::locale(""));
	report << dataSet.GetAprinfo().GetReport(true, false);
	report.close();
	return L"";
}