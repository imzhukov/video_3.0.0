#pragma once

//std
#include <queue>
#include <list>

//boost
#include <boost/shared_ptr.hpp>

/// �������� ������� �� ������ ����� ������� � ����� ������
template<typename T, template<typename ELEM, typename = std::allocator<ELEM> > class ContainerType=std::deque > class transfer
{
public:
	transfer(ContainerType<T>& source, ContainerType<T>& target)
	{
		target.push_back(source.front());
		source.pop_front();
	}
};

//------------------------------------------- ���������� ------------------------------------------------

/// ����
template<class T> class VDataIn
{
public:
	/// ����������
	virtual ~VDataIn(){};
	/// �������� ������ �� ����
	virtual void Put(const T& in_t) = 0;	
};

/// �����
template<class T> class VDataOut
{
public:
	/// ����������
	virtual ~VDataOut(){};
	/// ���������� true, ���� ���� ������������ ������
	virtual bool Ready() const = 0;	
	/// ���������� true, ���� ������������ ������ ���
	virtual bool Empty() const = 0;		
	/// ���������� ������ ������� ������
	virtual const T& Front() const = 0;
	/// ����������� ������ ������� ������
	virtual void Pop() = 0;		
	/// ������������ ������� ������ �� �����
	virtual void Flush() {};
};

/// ������ ������
template<class T> class VMultipleIn : public VDataIn<T>
{
private:
	/// ��������� ������ ��������� �� �����
	std::list<VDataIn<T>*> data;
public:
	/// �����������
	VMultipleIn(){};	
	/// �����������
	VMultipleIn(VDataIn<T>& in_data)
	{
		data.push_back(&in_data);
	};
	/// ����������
	~VMultipleIn(){};
	/// �������� ������ �� ��� �����
	void Put(const T& in_t)
	{		
		for (std::list<VDataIn<T>*>::iterator i=data.begin(); i!=data.end(); ++i )
		{
			if (*i)
				(*i)->Put(in_t);
			else
				throw NULL_POINTER;
		};	
	};
	/// ��������� ���� � ������
	VMultipleIn<T>& operator& (VDataIn<T>& in_data)
	{
		data.push_back(&in_data);
		return *this;
	};
};

/// ���������� ��� �����
template<class T> VMultipleIn<T> operator& (VDataIn<T>& in_data1, VDataIn<T>& in_data2)
{
	return VMultipleIn<T>(in_data1) & in_data2;
}


/// ���������� ������
template<class T1, class T2> class VDataProcessor : public VDataIn<T1>, public VDataOut<T2>
{
public:
	///����������
	virtual ~VDataProcessor(){};
	/// �������� ������ �� ����
	virtual void Put(const T1& in_t1){};	
};



/// �������� ������� ������ �� ������ ����������� � ������� � ������� �� ���������
template<class T> void operator >> (VDataOut<T>& in_source, VDataIn<T>& in_target)
{
	while (in_source.Ready())
	{
		in_target.Put(in_source.Front());
		in_source.Pop();
	}		
};

/// �������� ��� ������ �� ������ ����������� � ������� � ������� �� ���������
template<class T> void operator >= (VDataOut<T>& in_source, VDataIn<T>& in_target)
{
	in_source.Flush();
	while (!in_source.Empty())
	{
		in_target.Put(in_source.Front());
		in_source.Pop();
	}
};

/// �������� ������� ������ �� ������ ����������� � ������� � ������� �� ���������
template<class T1, class T2> VDataProcessor<T1, T2>& operator >> (VDataOut<T1>& in_source, VDataProcessor<T1, T2>& in_target)
{
	while (in_source.Ready())
	{
		in_target.Put(in_source.Front());
		in_source.Pop();
	}	
	return in_target;
};

/// �������� ��� ������ �� ������ ����������� � ������� � ������� �� ���������
template<class T1, class T2> VDataProcessor<T1, T2>& operator >= (VDataOut<T1>& in_source, VDataProcessor<T1, T2>& in_target)
{
	in_source.Flush();
	while (!in_source.Empty())
	{
		in_target.Put(in_source.Front());
		in_source.Pop();
	}	
	return in_target;
};

/// �������� ������� ������ �� ������ ����������� � ������� � ������� �� ���������
template<class T1, class T2> void TransferReady(VDataOut<T1>& in_source, VDataIn<T2>& in_target)
{
	while (in_source.Ready())
	{
		in_target.Put(in_source.Front());		
		in_source.Pop();
	}
};

/// �������� ��� ������ �� ������ ����������� � ������� � ������� �� ���������
template<class T1, class T2> void TransferRest(VDataOut<T1>& in_source, VDataIn<T2>& in_target)
{
	in_source.Flush();
	while (!in_source.Empty())
	{
		in_target.Put(in_source.Front());
		in_source.Pop();
	}
};

/// ������ ����������
template<class T> class VDummyProcessor : public VDataProcessor<T,T>
{
private:
	/// ������� ������
	std::deque<T> data;
public:
	/// �����������
	VDummyProcessor(){};
	/// ����������
	~VDummyProcessor(){};
	/// �������� ������ �� ����
	void Put(const T& in_data)
	{
		data.push_back(in_data);
	}
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const
	{
		return data.size()>0;
	}
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const
	{
		return data.empty();
	}
	/// ���������� ������ ������� ������
	const T& Front() const
	{
		return data.front();
	}
	/// ����������� ������ ������� ������
	void Pop()
	{
		data.pop_front();
	}
};

/// ������� ������������ ������
template<class T> class VDataChain : public VDataProcessor<T,T>
{
private:
	/// �����������
	mutable std::deque< boost::shared_ptr< VDataProcessor<T,T> > > processors;
	/// ��������� ������ ����������
	void Check() const
	{
		if (processors.empty())
			processors.push_back(boost::shared_ptr< VDataProcessor<T,T> >(new VDummyProcessor<T>()));
	};
public:
	/// �����������
	VDataChain(){};
	/// ����������
	~VDataChain(){};
	/// ��������� ���������� � ����� �������
	void AddProcessor(boost::shared_ptr< VDataProcessor<T,T> > in_processor)
	{
		processors.push_back(in_processor);
	};
	/// ������� �����������
	void Clear()
	{
		processors.clear();
	};
	/// �������� ������ �� ����
	void Put(const T& in_data)
	{
		Check();
		processors.front()->Put(in_data);
		if (processors.size()>=2)
		{
			for (size_t i=0; i+1<processors.size(); ++i)
			{
				*processors.at(i) >> *processors.at(i+1);
			}
		}
	};
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const
	{
		Check();
		return processors.back()->Ready();
	};
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const
	{
		Check();
		return processors.back()->Empty();
	};
	/// ���������� ������ ������� ������
	const T& Front() const
	{
		Check();
		return processors.back()->Front();
	};
	/// ����������� ������ ������� ������
	void Pop()
	{
		Check();
		processors.back()->Pop();
	};
	/// ���� ������
	void Flush()
	{
		Check();
		if (processors.size()>=2)
		{
			for (size_t i=0; i+1<processors.size(); ++i)
			{				
				*processors.at(i) >= *processors.at(i+1);
			}
		}
		processors.back()->Flush();
	};
};


/// ������������ �����������
template <typename T1, typename T2> class VDataParallel : public VDataProcessor<T1, T2>
{
private:
	/// �����������
	std::deque< boost::shared_ptr< VDataProcessor<T1,T2> > > processors;
	/// ������� ������ �� ������
	std::deque<T2> output;
public:
	/// �����������
	VDataParallel(){};
	/// ����������
	~VDataParallel(){};
	/// ��������� ���������� � ����� �������
	void AddProcessor(boost::shared_ptr< VDataProcessor<T1,T2> > in_processor)
	{
		processors.push_back(in_processor);
	}
	/// ������� �����������
	void Clear()
	{
		processors.clear();
	}
	/// �������� ������ �� ����
	void Put(const T1& in_data)
	{
		for (size_t i=0; i<processors.size(); ++i)
		{
			processors.at(i)->Put(in_data);
			while (processors.at(i)->Ready())
			{
				output.push_back(processors.at(i)->Front());
				processors.at(i)->Pop();
			}
		}
	}
	/// ���������� true, ���� ���� ������������ ������
	bool Ready() const
	{
		return output.size()>0;
	}
	/// ���������� true, ���� ������������ ������ ���
	bool Empty() const
	{
		return output.empty();
	}
	/// ���������� ������ ������� ������
	const T2& Front() const
	{
		return output.front();
	}
	/// ����������� ������ ������� ������
	void Pop()
	{
		output.pop_front();
	}
	/// ���� ������
	void Flush()
	{
		for (size_t i=0; i<processors.size(); ++i)
		{
			processors.at(i)->Flush();
			while (!processors.at(i)->Empty())
			{
				output.push_back(processors.at(i)->Front());
				processors.at(i)->Pop();
			}
		}
	};
};