#include "V4fCacheThread.h"

wxDEFINE_EVENT(wxEVT_COMMAND_V4FCACHETHREAD_COMPLETED, wxThreadEvent);

V4fCacheThread::V4fCacheThread(wxFrame * in_pFrame, std::string in_video_server) : pFrame(in_pFrame), video_server(in_video_server),
	start_code_dpp(-1), end_code_dpp(-1), t(0), imageBuffer(0), imageBufferSize(0)
{
	tmp_short_name = new char [MAX_PATH + 1];
	ReadFromCache();
}

void V4fCacheThread::CreateFullPathCacheFile(char * cache_file_path)
{
	std::string _video_server;
	if(video_server.length() > 0 && (video_server.at(video_server.length() - 1) == '\\' ||
		video_server.at(video_server.length() - 1) == '\/'))
		_video_server = video_server.substr(0, video_server.length() - 1);
	else
		_video_server = video_server;
	_snprintf(cache_file_path, MAX_PATH - 1, "%s\\%s", _video_server.c_str(), "cache.tmp");
}

V4fCacheThread::~V4fCacheThread()
{
	delete [] tmp_short_name;
	if(imageBuffer)
		delete [] imageBuffer;
	wxThreadEvent e(wxEVT_COMMAND_V4FCACHETHREAD_COMPLETED);
	pFrame->GetEventHandler()->AddPendingEvent(e);
}

void * V4fCacheThread::Entry()
{
	while (!TestDestroy())
	{
		if(!t && start_code_dpp < 0 && end_code_dpp < 0)
			continue;
		current_frame = frames.end();
		//удаление лишних фреймов
		while(frames.size() > 0 && frames.begin()->dpp < start_code_dpp)
			frames.erase(frames.begin());
		while(frames.size() > 0 && (frames.begin() + frames.size() - 1)->dpp > end_code_dpp)
			frames.erase((frames.begin() + frames.size() - 1));
		for(int idxRecord = 0; idxRecord < records.size(); idxRecord++)
		{
			if(records[idxRecord].GetTime() == t 
				&& start_code_dpp <= records[idxRecord].GetStartCodeDpp()
				&& end_code_dpp >= records[idxRecord].GetLastCodeDpp())
			{
				records[idxRecord].OpenReadFile();
				records[idxRecord].ReadHeader();
				records[idxRecord].LoadFrames();
				for(int idxFrame = 0; idxFrame < records[idxRecord].GetFrames().size(); idxFrame++)
				{
					int _imageBufferSize = records[idxRecord].GetFrames()[idxFrame].height * records[idxRecord].GetFrames()[idxFrame].width;
					if(imageBuffer && _imageBufferSize != imageBufferSize)
					{
						delete [] imageBuffer;
						unsigned char * imageBuffer = new unsigned char[_imageBufferSize];
						imageBufferSize = _imageBufferSize;
					}

					records[idxRecord].LoadImage(idxFrame, imageBuffer, imageBufferSize);
					V4fFrame frm(records[idxRecord].GetFrames()[idxFrame].dpp, 
						records[idxRecord].GetFrames()[idxFrame].absCoord, 
						records[idxRecord].GetFrames()[idxFrame].width,
						records[idxRecord].GetFrames()[idxFrame].height,
						imageBuffer, imageBufferSize);
					frames.push_back(frm);
				}
			}
			else
			{
				//Проверяем, если файл открыт, закрываем файл и очищаем вектор
				records[idxRecord].CloseFileAndClear();
			}
		}		
	}

	return NULL;
}

void V4fCacheThread::AddRecord(V4fDataSet * data_set)
{
	if(data_set != NULL)
	{
		V4fDataSet new_ds(*data_set);
		records.push_back(new_ds);
	}
}

void V4fCacheThread::SetRangeFrames(int in_start_code_dpp, int in_end_code_dpp, int64_t in_t)
{
	t = in_t;
	start_code_dpp = in_start_code_dpp;
	end_code_dpp = in_end_code_dpp;
}

void V4fCacheThread::WriteToCache()
{
		char cache_file_path [MAX_PATH];
		CreateFullPathCacheFile(cache_file_path);
		std::ofstream cache;
		cache.open(cache_file_path, std::ios::out|std::ios::binary);
		if (!cache.is_open())
		{
			return;
		}

		int version = CACHE_V4F_VERSION;
		cache.write((char*) &version, sizeof(int));
		cache.flush();

		for(int idxDataSet = 0; idxDataSet < records.size(); idxDataSet++)
		{
			long direction = records[idxDataSet].GetDir();
			cache.write((const char *) &direction, sizeof(long));
			char way [8];
			strncpy(way, records[idxDataSet].GetWay(), 8);
			cache.write((const char *) way, 8);
			int64_t t = records[idxDataSet].GetTime();
			cache.write((const char *) &t, 8);
			int start_code_dpp = records[idxDataSet].GetStartCodeDpp();
			cache.write((const char *) &start_code_dpp, 4);
			int last_code_dpp = records[idxDataSet].GetLastCodeDpp();
			cache.write((const char *) &last_code_dpp, 4);
			std::string short_name = records[idxDataSet].GetShortName();
			_snprintf(tmp_short_name, MAX_PATH, "%s", short_name.c_str());
			cache.write((const char *) tmp_short_name, MAX_PATH);
		}
		cache.close();
}

void V4fCacheThread::ReadFromCache()
{
	if(records.size())
		records.clear();
	///Основной поток
	char cache_file_path [MAX_PATH];
	CreateFullPathCacheFile(cache_file_path);	
	std::ifstream cache(cache_file_path, std::ios::in|std::ios::binary);
	if(cache.is_open())
	{
		cache.seekg(0, std::ios_base::end);
		int64_t fsize = cache.tellg();
		cache.seekg(0, std::ios_base::beg);

		// версия
		int version = -1;
		cache.read((char *) &version, sizeof(int));
		if (cache.gcount() != sizeof(int) || version != CACHE_V4F_VERSION)
			return;
		while (cache.tellg() < fsize)
		{
			long direction = 0;
			cache.read((char *) &direction, sizeof(long));
			char way [8] = "";
			cache.read((char *) way, 8);
			int64_t t = 0;
			cache.read((char *) &t, 8);
			int start_code_dpp = -1;
			cache.read((char *) &start_code_dpp, 4);
			int last_code_dpp = -1;
			cache.read((char *) &last_code_dpp, 4);
			cache.read((char *) tmp_short_name, MAX_PATH);
			std::string short_name(tmp_short_name);
			char * full_path = new char [MAX_PATH + 1];
			CreateFullPath(full_path, direction, way, t, short_name, video_server);
			V4fDataSet new_ds(full_path, short_name, direction, way, t);
			records.push_back(new_ds);
		}
		cache.close();
	}
}

V4fFrame * V4fCacheThread::GetFrame(int code_dpp)
{
	struct closer_to
	{
		unsigned long dpp;
		closer_to(unsigned long in_dpp) : dpp(in_dpp) {};
		bool operator()(const V4fFrame& l, const V4fFrame& r) const
		{
			return labs(l.dpp-dpp) < labs(r.dpp-dpp);
		};
	};
		
	current_frame = std::min_element(frames.begin(), frames.end(), closer_to(code_dpp));
	if(current_frame != frames.end())
		return current_frame._Ptr;
	else
		return NULL;
}

V4fFrame * V4fCacheThread::NextFrame()
{
	if(current_frame != frames.end())
	{
		current_frame++;
		return current_frame._Ptr;
	}
	else
	{
		return NULL;
	}
}

V4fFrame * V4fCacheThread::PreviousFrame()
{
	if(current_frame != frames.end() && current_frame != frames.begin())
	{
		current_frame--;
		return current_frame._Ptr;
	}
	else
	{
		return NULL;
	}
}