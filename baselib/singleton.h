#pragma once

#define BOOST_DATE_TIME_NO_LIB

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "os.h"

template<class T> class VSingleton
{
private:
	/// Закрытый конструктор
	VSingleton() {};
	/// Закрытый деструктор
	~VSingleton(){};
	/// Закрытый копирующий конструктор
	VSingleton(VSingleton&);
	/// Закрытый оператор присваивания
	VSingleton& operator=(VSingleton&);
	/// Возвращает ссылку на экземпляр класса
	static VSingleton<T>& GetInstance()
	{
		static VSingleton<T> instance;		
		return instance;
	}
		
	/// Хранимый объект
	T object;

	/// Признак того, что объект инициализирован
	bool valid;
	
	/// Mutex
	static boost::interprocess::interprocess_mutex mutex;
		
public:	
	/// Возвращает ссылку на экземпляр класса
	static VSingleton<T>& Instance()
	{				
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex);
		return GetInstance();
	};
	/// Устанавливает объект
	void SetObject(const T& in_object)
	{	
		object=in_object;
		valid=true;
	};
	/// Возвращает объект
	const T& GetObject() const
	{		
		if (valid)
			return object;
		else
			throw VSimpleException(L"Объект не инициализирован.", string_to_wstring(typeid(T).name()), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	};
	/// Возвращает объект
	T& GetObject()
	{	
		if (valid)
			return object;
		else
			throw VSimpleException(L"Объект не инициализирован.", string_to_wstring(typeid(T).name()), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	};
};
template<class T> boost::interprocess::interprocess_mutex VSingleton<T>::mutex;


/// Объявлние singleton'а
#define DECLARE_SINGLETON(classname) \
private: \
	static boost::interprocess::interprocess_mutex mutex; \
	classname(){}; \
	~classname(){}; \
	classname(classname&); \
	classname& operator=(classname&); \
	static classname& GetInstance() \
	{ \
		static classname instance; \
		return instance; \
	}; \
public: \
	static classname& Instance() \
	{ \
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex); \
		return GetInstance(); \
	};

/// Определение singleton'а
#define DEFINE_SINGLETON(classname) \
	boost::interprocess::interprocess_mutex classname::mutex;