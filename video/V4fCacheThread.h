#pragma once
#include <boost/asio.hpp>

#include <wx/wx.h>
#include <fstream>
#include "V4fDataSet.h"

#define CACHE_V4F_VERSION	1L
#define MAX_COUNT_FRAMES	2000L

wxDECLARE_EVENT(wxEVT_COMMAND_V4FCACHETHREAD_COMPLETED, wxThreadEvent);
class V4fCacheThread : public wxThread
{
	/// ��������� �� ������������ �����
	wxFrame * pFrame;
	///������ ���� � ���������� ������������
	std::string video_server;
	///������ � ����
	std::vector<V4fDataSet> records;
	///����� ��� ������ ��������� ����� �����
	char * tmp_short_name;
	///������� ���� � ����� ����
	void CreateFullPathCacheFile(char * cache_file_path);
	///������ �������
	std::vector<V4fFrame> frames;
	std::vector<V4fFrame>::iterator current_frame;
	///������� ��������� ������ ������
	int start_code_dpp;
	int end_code_dpp;
	int64_t t;

	unsigned char * imageBuffer;
	int imageBufferSize;
public:
	///����������
	V4fCacheThread(wxFrame * in_pFrame, std::string in_video_server);
	///���������
	~V4fCacheThread();
	/// ������� ������
	void * Entry();
	///��������� ������ � ����� � ���
	void AddRecord(V4fDataSet * data_set);
	///������������� �������� ������ ������
	void SetRangeFrames(int in_start_code_dpp, int in_end_code_dpp, int64_t in_t);
	///���������� ����� �� ���� ���
	V4fFrame * GetFrame(int code_dpp);
	///��������� ��������� �����
	V4fFrame * NextFrame();
	///��������� ���������� �����
	V4fFrame * PreviousFrame();
	///������� ����� �� ����
	void ReadFromCache();
	///�������� ����� � ���
	void WriteToCache();
};