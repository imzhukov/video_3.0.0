#pragma once

#include <string>
#include <locale>
#include <boost/asio.hpp>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#endif

/// ��� ������
class NoGuard
{
public:
	void Lock(){};
	void Unlock(){};
};

#ifdef _WIN32
/// ������ �� ������ ����������� ������ Win32
class GuardWin32
{
private:
	/// ����������� ������
	CRITICAL_SECTION critical_section;
public:
	/// �����������
	GuardWin32() 
	{
		if (!::InitializeCriticalSectionAndSpinCount(&critical_section, 4000))
		{
			throw;
		}
	};
	/// ����������
	~GuardWin32()
	{
		::DeleteCriticalSection(&critical_section);
	};
	///  ���� � ����������� ������
	void Lock()
	{
		::EnterCriticalSection(&critical_section);
	};
	/// ����� �� ����������� ������
	void Unlock()
	{
		::LeaveCriticalSection(&critical_section);
	};
};
#endif

/// ����� ����������� �������� ���������� � �����
template<typename ValueType> class DiskStorage
{
private:
	/// �������� ����������
	ValueType value;
	/// ��� ����������
	std::wstring name;
	/// ��� ����� - ���������
	std::wstring file;
public:

	/// �����������
	DiskStorage(const ValueType& in_value, const std::wstring& in_name, const std::wstring& in_file) : 
		value(in_value), name(in_name), file(in_file)
	{
		Load();
	};
	
	/// ����������
	~DiskStorage()
	{
		Save();
	};
	
	/// ���������� ������ �� �������� ����������
	ValueType& Value(){return value;};
	
	/// ���������� ����������� ������ �� �������� ����������
	const ValueType& Value() const {return value;};
	
	/// ���������� ��� ����������
	const std::wstring& Name() const{return name;};
	
	/// ���������� ������ ���� ����������=��������
	std::wstring Str()
	{
		std::wstringstream s;
		s << Name() << L"=" << Value();
		return s.str();
	};	
	
	/// ��������� ������ ���� ����������=��������
	void Str(const std::wstring& in_line)
	{
		size_t i=in_line.find(L"=");
		std::wstringstream s;
		s << in_line.substr(i+1);
		s >> Value();
	};
	
	/// ���������� ���������� � ����
	void Save()
	{
		std::vector<std::wstring> options;
		std::wifstream in(file.c_str());
		if (in.is_open())
		{		
			in.imbue(std::locale("rus_rus.1251"));
			while (!in.eof())
			{
				std::wstring line;
				in >> line;
				options.push_back(line);
			}
			struct pred
			{
				std::wstring name;
				pred(const std::wstring& in_name) : name(in_name){};
				bool operator()(const std::wstring& in_name)
				{				
					return in_name.compare(0, name.size(), name)==0;
				};
			};
			std::vector<std::wstring>::iterator i=std::find_if(options.begin(), options.end(), pred(Name()));
			if (i!=options.end())
			{
				*i=Str();
			}
			else
			{			
				options.push_back(Str());
			}
		}
		else
		{		
			options.push_back(Str());
		}
		
		std::wofstream out(file.c_str(), std::ios_base::trunc);
		if (out.is_open())
		{
			out.imbue(std::locale("rus_rus.1251"));		
			for (size_t i=0;i<options.size();++i)
			{			
				out << options.at(i) << std::endl;
			}
		}	
	};
	
	/// ��������� ���������� �� �����
	void Load()
	{
		std::vector<std::wstring> options;
		std::wifstream in(file.c_str());
		if (in.is_open())
		{		
			in.imbue(std::locale("rus_rus.1251"));
			while (!in.eof())
			{
				std::wstring line;
				in >> line;
				options.push_back(line);
			}
			struct pred
			{
				std::wstring name;
				pred(const std::wstring& in_name) : name(in_name){};
				bool operator()(const std::wstring& in_name)
				{				
					return in_name.compare(0, name.size(), name)==0;
				};
			};
			std::vector<std::wstring>::iterator i=std::find_if(options.begin(), options.end(), pred(Name()));
			if (i!=options.end())
			{
				Str(*i);
			}		
		}
	};
};

/// ������������ �������� ��������� � ������ ��� ������ � ����
template<typename ValueType> class MemoryStorage
{
private:
	/// �������� ����������
	ValueType value;		
public:
	/// �����������
	MemoryStorage(const ValueType& in_value) : value(in_value){};
	/// ����������
	~MemoryStorage(){};	
	/// ���������� ������ �� �������� ����������
	ValueType& Value(){return value;};
	/// ���������� ����������� ������ �� �������� ����������
	const ValueType& Value() const {return value;};
};

/// ������������ ������ � ����������
template<typename ValueType, typename DerivedType, template<typename> class StorageType=Storage, typename GuardType=NoGuard> class Option
{		
private:

	/// ������� ��������� � ���������� ������ �� ����
	GuardType& Guard()
	{
		static GuardType guard;
		return guard;
	};

	/// ������� ����������� ��������� � ���������� ������ �� ����
	StorageType<ValueType>& Storage()
	{
		static StorageType<ValueType> storage=DerivedType::Create();
		return storage;
	};		
		
public:
	/// ��� ��������
	typedef ValueType Type;
	/// ���������� ������ �� �������� ����������	
	ValueType& Value()
	{
		return Storage().Value();
	};
	/// ���������� ����������� ������ �� �������� ����������
	const ValueType& Value() const
	{
		return Storage().Value();
	};
	/// ���������� ��� ���������� �� ������ ����� ������ �������
	static std::wstring Name()
	{
		std::wstring class_name = string_to_wstring(typeid(DerivedType).name());
		size_t pos=class_name.find(L" ");
		return class_name.substr(pos+1);
	};
	/// ���� � ����������� ������
	void Lock()
	{
		Guard().Lock();
	};
	/// ����� �� ����������� ������
	void Unlock()
	{
		Guard().Unlock();
	}
};

/// ��������� ����� ���������� � �����
#define DECLARE_OPTION(ValueType, ClassName, DefaultValue, FileName) class ClassName : public Option<ValueType, ClassName, DiskStorage, NoGuard> \
{ \
public: \
	static DiskStorage<ValueType> Create() \
	{ \
		return DiskStorage<ValueType>(DefaultValue, Name(), FileName); \
	}; \
};

#ifdef _WIN32
/// ��������� ����� ���������� � ����� (����������������)
#define DECLARE_OPTION_MT(ValueType, ClassName, DefaultValue, FileName) class ClassName : public Option<ValueType, ClassName, DiskStorage, GuardWin32> \
{ \
public: \
	static DiskStorage<ValueType> Create() \
	{ \
		return DiskStorage<ValueType>(DefaultValue, Name(), FileName); \
	}; \
};
#endif


/// ��������� ������������� ���������� � ������
#define DECLARE_VARIABLE(ValueType, ClassName, DefaultValue) class ClassName : public Option<ValueType, ClassName, MemoryStorage, NoGuard> \
{ \
public: \
	static MemoryStorage<ValueType> Create() \
	{ \
		return MemoryStorage<ValueType>(DefaultValue); \
	}; \
};

#ifdef _WIN32
/// ��������� ������������� ���������� � ������ (����������������)
#define DECLARE_VARIABLE_MT(ValueType, ClassName, DefaultValue) class ClassName : public Option<ValueType, ClassName, MemoryStorage, GuardWin32> \
{ \
public: \
	static MemoryStorage<ValueType> Create() \
	{ \
		return MemoryStorage<ValueType>(DefaultValue); \
	}; \
};
#endif
