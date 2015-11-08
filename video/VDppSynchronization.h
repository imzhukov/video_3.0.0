#pragma once

#include <deque>
#include <utility>

const int MAIN_BUFFER_SIZE = 1000;
const int NOTSYNCHRONIZED_BUFFER_SIZE = 1000;

template <typename MainFrame, typename NotSynchronizedFrame> class VDppSynchronization
{
	/// Кадры произвольного формата с кодом ДПП (Полный профиль, видеокадры)
	/// Два вектора. Если камер будет больше двух подключено к одной машине, 
	/// то нужно будет увеличить их число
	std::deque<NotSynchronizedFrame> inputNotSynchronized [2];
	int currentIdInput;
	// Пара кадров из сети
	std::deque<MainFrame> inputMain;
	// Кадры на выходе
	//std::deque<std::pair<MainFrame, NotSynchronizedFrame>> output [2];
	// FixMe: пишем всё подряд 
	std::deque<NotSynchronizedFrame> output [2];
	// Простейший семофор
	bool flagCriticalSection;
public:
	VDppSynchronization():flagCriticalSection(false),currentIdInput(0){}
	/// Создание нового вектора несинхронизированных данных
	int CreateNewInput()
	{
		return currentIdInput++;
	}
	/// Помещает на вход кадр данных
	void Put(int idInput, const NotSynchronizedFrame & in_data)
	{
		if(flagCriticalSection)
			return;
		flagCriticalSection = true;
		if(inputNotSynchronized[idInput].size() == (int) NOTSYNCHRONIZED_BUFFER_SIZE)
			inputNotSynchronized[idInput].pop_front();
		inputNotSynchronized[idInput].push_back(in_data);
		flagCriticalSection = false;
	}
	/// Помещает на вход кадр данных по сети
	void PutMain(const MainFrame & in_data)
	{
		if(flagCriticalSection)
			return;
		flagCriticalSection = true;
		if(inputMain.size() == MAIN_BUFFER_SIZE)
			inputMain.pop_front();
		inputMain.push_back(in_data);
		flagCriticalSection = false;
	}
	/// Синхронизация
	void Synchronize(int idInput)
	{
		wchar_t msg [256];
		for(int q = 0; q < inputNotSynchronized[idInput].size(); q++)
		{
			_snwprintf(msg, 255, L"inputNotSynchronized: %i", inputNotSynchronized[idInput][q].dpp);
			//LOG_DEBUG(msg);
		}
		for(int q = 0; q < inputMain.size(); q++)
		{	
			_snwprintf(msg, 255, L"inputMain: %i", inputMain[q].dpp);
			//LOG_DEBUG(msg);
		}
		if(flagCriticalSection)
			return;
		flagCriticalSection = true;
		for(int idx = 0; idx < inputNotSynchronized[idInput].size() && idx >= 0; idx++)
		{
			//Пишем всё подряд!! 
			output[idInput].push_back(inputNotSynchronized[idInput][idx]);
			inputNotSynchronized[idInput].erase(inputNotSynchronized[idInput].begin() + idx);
			idx--;
			/*
			struct closer_to
			{
				unsigned long dpp;
				closer_to(unsigned long in_dpp) : dpp(in_dpp) {};
				bool operator()(const MainFrame& l, const MainFrame& r) const
				{
					return labs(l.dpp-dpp) < labs(r.dpp-dpp);
				};
			};
		
			std::deque<MainFrame>::iterator nearest=std::min_element(inputMain.begin(), inputMain.end(), closer_to(inputNotSynchronized[idInput][idx].dpp));
			if(labs(nearest->dpp - inputNotSynchronized[idInput][idx].dpp) > 20)
				continue;
			std::pair<MainFrame, NotSynchronizedFrame> new_pair = std::make_pair(*nearest, inputNotSynchronized[idInput][idx]);
			output[idInput].push_back(new_pair);
			inputNotSynchronized[idInput].erase(inputNotSynchronized[idInput].begin() + idx);
			idx--;*/
		}
		flagCriticalSection = false;
	}
	/// Возвращает true, если данные готовы
	bool Ready(int idInput) const
	{
		return (output[idInput].size() > 0);
	}
	/// Возвращает true, если обработанных данных нет
	bool Empty(int idInput) const
	{
		return output[idInput].empty();
	}
	/// Возвращает первый элемент данных
	//const std::pair<MainFrame, NotSynchronizedFrame>& Front(int idInput) const
	const NotSynchronizedFrame Front(int idInput) const
	{
		return output[idInput].front();
	}
	/// Выталкивает первый элемент данных
	void Pop(int idInput)
	{
		if(flagCriticalSection)
			return;
		output[idInput].pop_front();
	}
};