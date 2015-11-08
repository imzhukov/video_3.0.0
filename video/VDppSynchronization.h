#pragma once

#include <deque>
#include <utility>

const int MAIN_BUFFER_SIZE = 1000;
const int NOTSYNCHRONIZED_BUFFER_SIZE = 1000;

template <typename MainFrame, typename NotSynchronizedFrame> class VDppSynchronization
{
	/// ����� ������������� ������� � ����� ��� (������ �������, ����������)
	/// ��� �������. ���� ����� ����� ������ ���� ���������� � ����� ������, 
	/// �� ����� ����� ��������� �� �����
	std::deque<NotSynchronizedFrame> inputNotSynchronized [2];
	int currentIdInput;
	// ���� ������ �� ����
	std::deque<MainFrame> inputMain;
	// ����� �� ������
	//std::deque<std::pair<MainFrame, NotSynchronizedFrame>> output [2];
	// FixMe: ����� �� ������ 
	std::deque<NotSynchronizedFrame> output [2];
	// ���������� �������
	bool flagCriticalSection;
public:
	VDppSynchronization():flagCriticalSection(false),currentIdInput(0){}
	/// �������� ������ ������� �������������������� ������
	int CreateNewInput()
	{
		return currentIdInput++;
	}
	/// �������� �� ���� ���� ������
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
	/// �������� �� ���� ���� ������ �� ����
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
	/// �������������
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
			//����� �� ������!! 
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
	/// ���������� true, ���� ������ ������
	bool Ready(int idInput) const
	{
		return (output[idInput].size() > 0);
	}
	/// ���������� true, ���� ������������ ������ ���
	bool Empty(int idInput) const
	{
		return output[idInput].empty();
	}
	/// ���������� ������ ������� ������
	//const std::pair<MainFrame, NotSynchronizedFrame>& Front(int idInput) const
	const NotSynchronizedFrame Front(int idInput) const
	{
		return output[idInput].front();
	}
	/// ����������� ������ ������� ������
	void Pop(int idInput)
	{
		if(flagCriticalSection)
			return;
		output[idInput].pop_front();
	}
};