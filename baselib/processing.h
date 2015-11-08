#pragma once

//std
#include <queue>
#include <list>

//boost
#include <boost/shared_ptr.hpp>

/// Передает элемент из начала одной очереди в конец другой
template<typename T, template<typename ELEM, typename = std::allocator<ELEM> > class ContainerType=std::deque > class transfer
{
public:
	transfer(ContainerType<T>& source, ContainerType<T>& target)
	{
		target.push_back(source.front());
		source.pop_front();
	}
};

//------------------------------------------- Абстракции ------------------------------------------------

/// Вход
template<class T> class VDataIn
{
public:
	/// Деструктор
	virtual ~VDataIn(){};
	/// Помещает данные на вход
	virtual void Put(const T& in_t) = 0;	
};

/// Выход
template<class T> class VDataOut
{
public:
	/// Деструктор
	virtual ~VDataOut(){};
	/// Возвращает true, если есть обработанные данные
	virtual bool Ready() const = 0;	
	/// Возвращает true, если обработанных данных нет
	virtual bool Empty() const = 0;		
	/// Возвращает первый элемент данных
	virtual const T& Front() const = 0;
	/// Выталкивает первый элемент данных
	virtual void Pop() = 0;		
	/// Обрабатывает остаток данных на входе
	virtual void Flush() {};
};

/// Группа входов
template<class T> class VMultipleIn : public VDataIn<T>
{
private:
	/// Контейнер хранит указатель на входы
	std::list<VDataIn<T>*> data;
public:
	/// Конструктор
	VMultipleIn(){};	
	/// Конструктор
	VMultipleIn(VDataIn<T>& in_data)
	{
		data.push_back(&in_data);
	};
	/// Деструктор
	~VMultipleIn(){};
	/// Помещает данные на все входы
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
	/// Добавляет вход к группе
	VMultipleIn<T>& operator& (VDataIn<T>& in_data)
	{
		data.push_back(&in_data);
		return *this;
	};
};

/// Объединяет два входа
template<class T> VMultipleIn<T> operator& (VDataIn<T>& in_data1, VDataIn<T>& in_data2)
{
	return VMultipleIn<T>(in_data1) & in_data2;
}


/// Обработчик данных
template<class T1, class T2> class VDataProcessor : public VDataIn<T1>, public VDataOut<T2>
{
public:
	///Деструктор
	virtual ~VDataProcessor(){};
	/// Помещает данные на вход
	virtual void Put(const T1& in_t1){};	
};



/// Передает готовые данные от одного обработчика к другому и удаляет из исходного
template<class T> void operator >> (VDataOut<T>& in_source, VDataIn<T>& in_target)
{
	while (in_source.Ready())
	{
		in_target.Put(in_source.Front());
		in_source.Pop();
	}		
};

/// Передает все данные от одного обработчика к другому и удаляет из исходного
template<class T> void operator >= (VDataOut<T>& in_source, VDataIn<T>& in_target)
{
	in_source.Flush();
	while (!in_source.Empty())
	{
		in_target.Put(in_source.Front());
		in_source.Pop();
	}
};

/// Передает готовые данные от одного обработчика к другому и удаляет из исходного
template<class T1, class T2> VDataProcessor<T1, T2>& operator >> (VDataOut<T1>& in_source, VDataProcessor<T1, T2>& in_target)
{
	while (in_source.Ready())
	{
		in_target.Put(in_source.Front());
		in_source.Pop();
	}	
	return in_target;
};

/// Передает все данные от одного обработчика к другому и удаляет из исходного
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

/// Передает готовые данные от одного обработчика к другому и удаляет из исходного
template<class T1, class T2> void TransferReady(VDataOut<T1>& in_source, VDataIn<T2>& in_target)
{
	while (in_source.Ready())
	{
		in_target.Put(in_source.Front());		
		in_source.Pop();
	}
};

/// Передает все данные от одного обработчика к другому и удаляет из исходного
template<class T1, class T2> void TransferRest(VDataOut<T1>& in_source, VDataIn<T2>& in_target)
{
	in_source.Flush();
	while (!in_source.Empty())
	{
		in_target.Put(in_source.Front());
		in_source.Pop();
	}
};

/// Пустой обработчик
template<class T> class VDummyProcessor : public VDataProcessor<T,T>
{
private:
	/// Очередь данных
	std::deque<T> data;
public:
	/// Конструктор
	VDummyProcessor(){};
	/// Деструктор
	~VDummyProcessor(){};
	/// Помещает данные на вход
	void Put(const T& in_data)
	{
		data.push_back(in_data);
	}
	/// Возвращает true, если есть обработанные данные
	bool Ready() const
	{
		return data.size()>0;
	}
	/// Возвращает true, если обработанных данных нет
	bool Empty() const
	{
		return data.empty();
	}
	/// Возвращает первый элемент данных
	const T& Front() const
	{
		return data.front();
	}
	/// Выталкивает первый элемент данных
	void Pop()
	{
		data.pop_front();
	}
};

/// Цепочка обработчиков данных
template<class T> class VDataChain : public VDataProcessor<T,T>
{
private:
	/// Обработчики
	mutable std::deque< boost::shared_ptr< VDataProcessor<T,T> > > processors;
	/// Добавляет пустой обработчик
	void Check() const
	{
		if (processors.empty())
			processors.push_back(boost::shared_ptr< VDataProcessor<T,T> >(new VDummyProcessor<T>()));
	};
public:
	/// Конструктор
	VDataChain(){};
	/// Деструктор
	~VDataChain(){};
	/// Добавляет обработчик в конец цепочки
	void AddProcessor(boost::shared_ptr< VDataProcessor<T,T> > in_processor)
	{
		processors.push_back(in_processor);
	};
	/// Удаляет обработчики
	void Clear()
	{
		processors.clear();
	};
	/// Помещает данные на вход
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
	/// Возвращает true, если есть обработанные данные
	bool Ready() const
	{
		Check();
		return processors.back()->Ready();
	};
	/// Возвращает true, если обработанных данных нет
	bool Empty() const
	{
		Check();
		return processors.back()->Empty();
	};
	/// Возвращает первый элемент данных
	const T& Front() const
	{
		Check();
		return processors.back()->Front();
	};
	/// Выталкивает первый элемент данных
	void Pop()
	{
		Check();
		processors.back()->Pop();
	};
	/// Слив данных
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


/// Параллельные обработчики
template <typename T1, typename T2> class VDataParallel : public VDataProcessor<T1, T2>
{
private:
	/// Обработчики
	std::deque< boost::shared_ptr< VDataProcessor<T1,T2> > > processors;
	/// Очередь данных на выходе
	std::deque<T2> output;
public:
	/// Конструктор
	VDataParallel(){};
	/// Деструктор
	~VDataParallel(){};
	/// Добавляет обработчик в конец цепочки
	void AddProcessor(boost::shared_ptr< VDataProcessor<T1,T2> > in_processor)
	{
		processors.push_back(in_processor);
	}
	/// Удаляет обработчики
	void Clear()
	{
		processors.clear();
	}
	/// Помещает данные на вход
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
	/// Возвращает true, если есть обработанные данные
	bool Ready() const
	{
		return output.size()>0;
	}
	/// Возвращает true, если обработанных данных нет
	bool Empty() const
	{
		return output.empty();
	}
	/// Возвращает первый элемент данных
	const T2& Front() const
	{
		return output.front();
	}
	/// Выталкивает первый элемент данных
	void Pop()
	{
		output.pop_front();
	}
	/// Слив данных
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