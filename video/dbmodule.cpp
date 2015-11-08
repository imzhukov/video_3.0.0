#include "dbmodule.h"

VRegisteredDatabase::VRegisteredDatabase(const wstring& in_fullstring)
{
	if (in_fullstring.compare(L"")!=0)
	{
		std::vector<wstring> split_vec;
		boost::algorithm::split(split_vec, in_fullstring, boost::is_any_of(L";"));
		if (split_vec.size()==4)
		{
			name = split_vec.at(0);
			server = split_vec.at(1);			
			database = split_vec.at(2);
			folder = split_vec.at(3);
			count_days_backup = 7;
		}
		else if(split_vec.size()>=5)
		{
			//есть информация о кол-ве дней, допустимых без резервного копирования
			name = split_vec.at(0);
			server = split_vec.at(1);
			database = split_vec.at(2);
			folder = split_vec.at(3);
			count_days_backup = atoi(wstring_to_string(split_vec.at(4).c_str()).c_str());
		}
		else
		{
			throw VSimpleException(L"Ошибка разбора строки", in_fullstring, boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}
};

void ConnectDB(VIBDatabasePtr db, VRegisteredDatabase & RegisteredDatabase)
{
	db->Connect(wxString(RegisteredDatabase.GetServer().c_str()).char_str(wxConvLocal), 
		wxString(RegisteredDatabase.GetDatabase().c_str()).char_str(wxConvLocal), DEFAULT_USER, DEFAULT_PASS);	
}