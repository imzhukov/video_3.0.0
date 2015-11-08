#pragma once

#include <string>
#include <locale>
#include <boost/asio.hpp>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#endif

/// Без защиты
class NoGuard
{
public:
	void Lock(){};
	void Unlock(){};
};

#ifdef _WIN32
/// Защита на основе критических секций Win32
class GuardWin32
{
private:
	/// Критическая секция
	CRITICAL_SECTION critical_section;
public:
	/// Конструктор
	GuardWin32() 
	{
		if (!::InitializeCriticalSectionAndSpinCount(&critical_section, 4000))
		{
			throw;
		}
	};
	/// Деструктор
	~GuardWin32()
	{
		::DeleteCriticalSection(&critical_section);
	};
	///  Вход в критическую секцию
	void Lock()
	{
		::EnterCriticalSection(&critical_section);
	};
	/// Выход из критической секции
	void Unlock()
	{
		::LeaveCriticalSection(&critical_section);
	};
};
#endif

/// Класс осуществлет хранение переменной в файле
template<typename ValueType> class DiskStorage
{
private:
	/// Значение переменной
	ValueType value;
	/// Имя переменной
	std::wstring name;
	/// Имя файла - хранилища
	std::wstring file;
public:

	/// Конструктор
	DiskStorage(const ValueType& in_value, const std::wstring& in_name, const std::wstring& in_file) : 
		value(in_value), name(in_name), file(in_file)
	{
		Load();
	};
	
	/// Деструктор
	~DiskStorage()
	{
		Save();
	};
	
	/// Возвращает ссылку на значение переменной
	ValueType& Value(){return value;};
	
	/// Возвращает константную ссылку на значение переменной
	const ValueType& Value() const {return value;};
	
	/// Возвращает има переменной
	const std::wstring& Name() const{return name;};
	
	/// Возвращает строку вида переменная=значение
	std::wstring Str()
	{
		std::wstringstream s;
		s << Name() << L"=" << Value();
		return s.str();
	};	
	
	/// Разбирает строку вида переменная=значение
	void Str(const std::wstring& in_line)
	{
		size_t i=in_line.find(L"=");
		std::wstringstream s;
		s << in_line.substr(i+1);
		s >> Value();
	};
	
	/// Записывает переменную в файл
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
	
	/// Загружает переменную из файла
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

/// Осуществляет хранение перменной в памяти без записи в файл
template<typename ValueType> class MemoryStorage
{
private:
	/// Значение переменной
	ValueType value;		
public:
	/// Конструктор
	MemoryStorage(const ValueType& in_value) : value(in_value){};
	/// Деструктор
	~MemoryStorage(){};	
	/// Возвращает ссылку на значение переменной
	ValueType& Value(){return value;};
	/// Возвращает константную ссылку на значение переменной
	const ValueType& Value() const {return value;};
};

/// Осуществляет доступ к переменной
template<typename ValueType, typename DerivedType, template<typename> class StorageType=Storage, typename GuardType=NoGuard> class Option
{		
private:

	/// Создает защитника и возвращает ссылку на него
	GuardType& Guard()
	{
		static GuardType guard;
		return guard;
	};

	/// Создает статическое хранилище и возвращает ссылку на него
	StorageType<ValueType>& Storage()
	{
		static StorageType<ValueType> storage=DerivedType::Create();
		return storage;
	};		
		
public:
	/// Тип значения
	typedef ValueType Type;
	/// Возвращает ссылку на значение переменной	
	ValueType& Value()
	{
		return Storage().Value();
	};
	/// Возвращает константную ссылку на значение переменной
	const ValueType& Value() const
	{
		return Storage().Value();
	};
	/// Возвращает имя переменной на основе имени класса доступа
	static std::wstring Name()
	{
		std::wstring class_name = string_to_wstring(typeid(DerivedType).name());
		size_t pos=class_name.find(L" ");
		return class_name.substr(pos+1);
	};
	/// Вход в критическую секцию
	void Lock()
	{
		Guard().Lock();
	};
	/// Выход из критической секции
	void Unlock()
	{
		Guard().Unlock();
	}
};

/// Объявляет опцию хранящуюся в файле
#define DECLARE_OPTION(ValueType, ClassName, DefaultValue, FileName) class ClassName : public Option<ValueType, ClassName, DiskStorage, NoGuard> \
{ \
public: \
	static DiskStorage<ValueType> Create() \
	{ \
		return DiskStorage<ValueType>(DefaultValue, Name(), FileName); \
	}; \
};

#ifdef _WIN32
/// Объявляет опцию хранящуюся в файле (потокобезопасную)
#define DECLARE_OPTION_MT(ValueType, ClassName, DefaultValue, FileName) class ClassName : public Option<ValueType, ClassName, DiskStorage, GuardWin32> \
{ \
public: \
	static DiskStorage<ValueType> Create() \
	{ \
		return DiskStorage<ValueType>(DefaultValue, Name(), FileName); \
	}; \
};
#endif


/// Объявляет несохраняемую переменную в памяти
#define DECLARE_VARIABLE(ValueType, ClassName, DefaultValue) class ClassName : public Option<ValueType, ClassName, MemoryStorage, NoGuard> \
{ \
public: \
	static MemoryStorage<ValueType> Create() \
	{ \
		return MemoryStorage<ValueType>(DefaultValue); \
	}; \
};

#ifdef _WIN32
/// Объявляет несохраняемую переменную в памяти (потокобезопасную)
#define DECLARE_VARIABLE_MT(ValueType, ClassName, DefaultValue) class ClassName : public Option<ValueType, ClassName, MemoryStorage, GuardWin32> \
{ \
public: \
	static MemoryStorage<ValueType> Create() \
	{ \
		return MemoryStorage<ValueType>(DefaultValue); \
	}; \
};
#endif
