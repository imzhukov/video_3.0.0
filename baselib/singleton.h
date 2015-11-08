#pragma once

#define BOOST_DATE_TIME_NO_LIB

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "os.h"

template<class T> class VSingleton
{
private:
	/// �������� �����������
	VSingleton() {};
	/// �������� ����������
	~VSingleton(){};
	/// �������� ���������� �����������
	VSingleton(VSingleton&);
	/// �������� �������� ������������
	VSingleton& operator=(VSingleton&);
	/// ���������� ������ �� ��������� ������
	static VSingleton<T>& GetInstance()
	{
		static VSingleton<T> instance;		
		return instance;
	}
		
	/// �������� ������
	T object;

	/// ������� ����, ��� ������ ���������������
	bool valid;
	
	/// Mutex
	static boost::interprocess::interprocess_mutex mutex;
		
public:	
	/// ���������� ������ �� ��������� ������
	static VSingleton<T>& Instance()
	{				
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex);
		return GetInstance();
	};
	/// ������������� ������
	void SetObject(const T& in_object)
	{	
		object=in_object;
		valid=true;
	};
	/// ���������� ������
	const T& GetObject() const
	{		
		if (valid)
			return object;
		else
			throw VSimpleException(L"������ �� ���������������.", string_to_wstring(typeid(T).name()), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	};
	/// ���������� ������
	T& GetObject()
	{	
		if (valid)
			return object;
		else
			throw VSimpleException(L"������ �� ���������������.", string_to_wstring(typeid(T).name()), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
	};
};
template<class T> boost::interprocess::interprocess_mutex VSingleton<T>::mutex;


/// ��������� singleton'�
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

/// ����������� singleton'�
#define DEFINE_SINGLETON(classname) \
	boost::interprocess::interprocess_mutex classname::mutex;