#include "VExportImportTextFile.h"

VExportImportTextFile::VExportImportTextFile()
{}

void VExportImportTextFile::SetProperties(std::string in_path, FILE_TYPE in_file_type, std::string in_title)
{
	path = in_path;
	file_type = in_file_type;
	title = in_title;
}

VExportImportTextFile::VExportImportTextFile(std::string in_path, FILE_TYPE in_file_type, std::string in_title)
{
	path = in_path;
	file_type = in_file_type;
	title = in_title;
}

void VExportImportTextFile::Export()
{
	if(file_type == CSV)
	{
		divider = GetDivider();
		ofstream report(string_to_wstring(path).c_str());
		report << std::fixed;
		report.imbue(std::locale(""));
		for(size_t i = 0; i < legend.size(); i++)
		{
			report << legend[i].c_str();
				if(i != legend.size()-1)
					report << divider;
		}
		report << std::endl;
		for(size_t i = 0; i < array_string[legend[0]].size(); i++)
		{
			for(size_t j = 0; j < legend.size(); j++)
			{
				report << array_string[legend[j]][i].c_str();
					if(j != legend.size()-1)
						report << divider;
			}
			report << std::endl;
		}
		report.close();
	}
	if(file_type == HTML)
	{
		ofstream filestream(string_to_wstring(path).c_str());
		filestream << std::fixed;
		filestream << VHtml::GetHeader(title);
	}
}

bool VExportImportTextFile::Import()
{
	if(file_type == CSV)
	{
		ifstream _filestream(string_to_wstring(path).c_str());
		bool flag_legend = true;
		while(_filestream.good())
		{
			char str_c [512];
			std::vector<std::wstring> vector_string;
			_filestream.getline(str_c, 512);
			std::wstring str = string_to_wstring(str_c);
			boost::trim(str);
			boost::algorithm::split(vector_string, str, boost::is_any_of(string_to_wstring(GetDivider()).c_str()));
			if(flag_legend)
			{
				flag_legend = false;
				if(vector_string.size() != legend.size())
					return false;
				for(size_t i = 0; i < legend.size(); i++)
					if(legend[i] != wstring_to_string(vector_string[i]))
						return false;
			}
			else
				if(vector_string.size() == legend.size())
					for(size_t i = 0; i < vector_string.size(); i++)
						array_string[legend[i].c_str()].push_back(wstring_to_string(vector_string[i].c_str()).c_str());
		}
	}
	return true;
}

std::string VExportImportTextFile::GetDivider()
{
	return wstring_to_string(getCSVSep());
}

void VExportImportTextFile::SetSeparator(std::string in_separator)
{
	divider = in_separator;
}

void VExportImportTextFile::SetLegend(std::vector<std::string> in_legend)
{
	legend = in_legend;
}

void VExportImportTextFile::InsertRow(std::vector<std::string> row)
{
	for(size_t i = 0; i < row.size(); i++)
		array_string[legend[i]].push_back(row[i]);
}

void VExportImportTextFile::SetValue(int col, int row, std::string in_value)
{
	array_string[legend[col]][row] = in_value;
}

void VExportImportTextFile::DeleteRow(int row)
{
	for(size_t i = 0; i < legend.size(); i++)
		array_string[legend[i]].erase(array_string[legend[i]].begin() + row - 1);
}

std::string VExportImportTextFile::GetValue(int col, int row)
{
	return array_string[legend[col]][row];
}

int VExportImportTextFile::GetRows()
{
	return array_string[legend[0]].size();
}

int VExportImportTextFile::GetCols()
{
	return legend.size();
}

void VExportImportTextFile::ExportString(const char *msg)
{
	filestream << msg;
}

std::fstream & VExportImportTextFile::operator << (const char * msg)
{
	filestream << msg;
	return filestream;
}

void VExportImportTextFile::Close()
{
	filestream.close();
}