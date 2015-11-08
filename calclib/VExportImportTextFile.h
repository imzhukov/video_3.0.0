#pragma once

#include "VDataSet.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include "strlib.h"
#include "VHtml.h"

enum FILE_TYPE {
	CSV = 1,
	HTML = 2
};

class VExportImportTextFile 
{
	std::fstream filestream;
	std::string divider;
	std::string GetDivider();
	std::string path;
	std::string title;
	FILE_TYPE file_type;
	std::map<std::string, std::vector<std::string>> array_string;
	std::vector<std::string> legend;

public:
	VExportImportTextFile(std::string in_path, FILE_TYPE in_file_type, std::string in_title);
	VExportImportTextFile();
	void SetProperties(std::string in_path, FILE_TYPE in_file_type, std::string in_title);
	void Export();
	bool Import();
	void SetSeparator(std::string in_separator);
	void SetLegend(std::vector<std::string> in_legend);
	void InsertRow(std::vector<std::string> row);
	void SetValue(int col, int row, std::string in_value);
	void DeleteRow(int row);
	std::string GetValue(int col, int row);
	int GetRows();
	int GetCols();
	void ExportString(const char * msg);
	std::fstream & operator << (const char * msg);
	void Close();
};