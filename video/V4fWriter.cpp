#include "V4fWriter.h"
//-------------------------------------- Методы класса V4fWriter ----------------------------------
V4fWriter::V4fWriter(std::wstring newCameraName)
{
	dataSet = 0;
	MAIN_SYNCHRONIZATION().Lock();
	inputId = MAIN_SYNCHRONIZATION().Value().CreateNewInput();
	MAIN_SYNCHRONIZATION().Unlock();
	//folder = f;
	cameraName = newCameraName;
	Opened = false;
	counterFrames = -1;

	cache = new V4fCacheThread(NULL, wstring_to_string(VIDEO_OPTIONS().Value().video_server));
}

V4fWriter::~V4fWriter()
{
	if(dataSet)
		delete dataSet;
}

bool V4fWriter::IsOpened()
{
	return Opened;
}

void V4fWriter::AddFrame(cv::Mat frame)
{
	if (!frame.empty() && Opened && CURRENT_POSITION().Value().absolutePosition > 0)
	{
		counterFrames++;
		if(counterFrames != MISSED_FRAMES)
			return;
		counterFrames = -1;
		vector<int> p;
		p.push_back(CV_IMWRITE_JPEG_QUALITY);
		p.push_back(50);
		vector<unsigned char> buf;
		imencode(".jpg", frame, buf, p);
		//Сохраняем информацию о кадре
		if(dataSet)
		{
			if(currentDir!=CURRENT_POSITION().Value().dir || strcmp(currentWay, CURRENT_POSITION().Value().way)
				/*Нужно добавить сюда ещё сравнение по времени*/)
			{
				this->Open();
			}
			int bufLen = buf.size();
			V4fFrame new_frame(CURRENT_DPP().Value(), CURRENT_POSITION().Value().absolutePosition, frame.cols, frame.rows, buf);
			//MAIN_SYNCHRONIZATION().Lock();
			//MAIN_SYNCHRONIZATION().Value().Put(inputId, new_frame);
			//MAIN_SYNCHRONIZATION().Value().Synchronize(inputId);
			//while(MAIN_SYNCHRONIZATION().Value().Ready(inputId))
			//{
				//std::pair<VCoordinatesRecord, V4fFrame> _pair = MAIN_SYNCHRONIZATION().Value().Front(inputId);
				//V4fFrame cadre = MAIN_SYNCHRONIZATION().Value().Front(inputId);
				dataSet->WriteFrame(new_frame.dpp, 0.0f, ENCODE_CV_50, new_frame.height, new_frame.width, new_frame.img.data(), new_frame.img.size());
				//dataSet->WriteFrame(_pair.first.dpp, _pair.first.absolutePosition, ENCODE_CV_50, _pair.second.height, _pair.second.width, _pair.second.img.data(), _pair.second.img.size());
				//MAIN_SYNCHRONIZATION().Value().Pop(inputId);
			//}
			//MAIN_SYNCHRONIZATION().Unlock();
		}
	}
}

void V4fWriter::AddFrame(int width, int height, unsigned char * bufData, int bufDataLen)
{
	if (Opened && CURRENT_POSITION().Value().absolutePosition > 0)
	{
		counterFrames++;
		if(counterFrames != MISSED_FRAMES)
			return;
		counterFrames = -1;
		//Сохраняем информацию о кадре
		if(dataSet)
		{
			if(currentDir!=CURRENT_POSITION().Value().dir || strcmp(currentWay, CURRENT_POSITION().Value().way)
				/*Нужно добавить сюда ещё сравнение по времени*/)
			{
				LOG_INFO(L"Изменились направление или путь, начинаем новую запись");
				this->Release();
				this->Open();
			}
			//LOG_DEBUG(L"Пытаемся заблокировать подачу меток ДПП");
			V4fFrame new_frame(CURRENT_DPP().Value(), CURRENT_POSITION().Value().absolutePosition, width, height, bufData, bufDataLen);
			//LOG_DEBUG(L"Создали кадр");
			//MAIN_SYNCHRONIZATION().Lock();
			//LOG_DEBUG(L"Заблокировали поток синхронизации");
			//MAIN_SYNCHRONIZATION().Value().Put(inputId, new_frame);
			//LOG_DEBUG(L"Добавили кадр в поток синхронизации");
			//MAIN_SYNCHRONIZATION().Value().Synchronize(inputId);
			//LOG_DEBUG(L"Провели синхронизацию");
			//while(MAIN_SYNCHRONIZATION().Value().Ready(inputId))
			//{
				//std::pair<VCoordinatesRecord, V4fFrame> _pair = MAIN_SYNCHRONIZATION().Value().Front(inputId);
				//V4fFrame cadre = MAIN_SYNCHRONIZATION().Value().Front(inputId);
				dataSet->WriteFrame(new_frame.dpp, 0.0f, ENCODE_CV_50, new_frame.height, new_frame.width, new_frame.img.data(), new_frame.img.size());
				//dataSet->WriteFrame(_pair.first.dpp, _pair.first.absolutePosition, ENCODE_CV_50, _pair.second.height, _pair.second.width, _pair.second.img.data(), _pair.second.img.size());
				//MAIN_SYNCHRONIZATION().Value().Pop(inputId);
			//}
			//MAIN_SYNCHRONIZATION().Unlock();
			//LOG_DEBUG(L"Разблокировали поток синхронизации");
		}
	}
}

bool V4fWriter::Open()
{
	if (Opened || CURRENT_POSITION().Value().dir <= 0) 
		return false;

	SECURITY_ATTRIBUTES sa; 
	sa.nLength= sizeof(sa);
	sa.lpSecurityDescriptor= NULL;			

	time_t now = time(NULL);
	localtime(&now);
	char hms[32]="";
	char full_path [MAX_PATH] = "", short_path [128] = "";
	strftime(hms, 31, "%Hh%Mm%Ss", localtime(&now));
	_snprintf(short_path, 127, "%s_%s.v4f", cameraName.c_str(), hms);
	CreateFullPath(full_path, CURRENT_POSITION().Value().dir, CURRENT_POSITION().Value().way,
		CURRENT_POSITION().Value().start_time, short_path, wstring_to_string(VIDEO_OPTIONS().Value().video_server));
	fullPath = string_to_wstring(full_path);
/*	char ymd[22]="", hms[32]="", dir_way[64]="";
	strftime(ymd, 21, "%Y%m%d", localtime(&now));
	strftime(hms, 31, "%Hh%Mm%Ss", localtime(&now));
	_snprintf(dir_way,63,"%i_%s",CURRENT_POSITION().Value().dir, CURRENT_POSITION().Value().way);
	std::wstring finalPath = string_to_wstring((const char *) ymd) + L"_" + string_to_wstring((const char *) dir_way);
	
	wchar_t shtFileName [1024] = L"";
	try
	{
		const wchar_t * ch = folder.c_str();
		if(ch[wcslen(ch)-1] == L'\\')
			finalPath = folder + finalPath;
		else
			finalPath = folder + L"\\" + finalPath;
		CreateDirectory(finalPath.c_str(), &sa);

		VFileName filename;
		filename.SetFolder(finalPath);
		std::wstringstream short_filename;
		///Имя видеофайла строится по шаблону: Имя камеры + Время записи
		swprintf(shtFileName, 1023, L"%s_%s.v4f", cameraName.c_str(), string_to_wstring(hms).c_str());
		filename.SetShortName(shtFileName);
		fullPath = filename.GetPath();
	}
	catch(std::exception e)
	{
		wchar_t msg [256];
		_snwprintf(msg, 255, L"Ошибка при открытии файла %s", e.what());
		LOG_ERROR(msg);
	}*/

	dataSet = new V4fDataSet(wstring_to_string(fullPath), short_path, CURRENT_POSITION().Value().dir, CURRENT_POSITION().Value().way, CURRENT_POSITION().Value().start_time);
	if(dataSet)
	{
		dataSet->OpenWriteFile();
		dataSet->WriteHeader();
		Opened = true;
		currentDir = CURRENT_POSITION().Value().dir;
		strcpy(currentWay, CURRENT_POSITION().Value().way);
		return true;
	}
	return false;
}

bool V4fWriter::Release()
{
	if (dataSet)
	{
		cache->AddRecord(dataSet);
		cache->WriteToCache();

		delete dataSet;
		dataSet = 0;
		Opened = false;
		return true;
	}
	else
		return false;
}