#include "V4fWriter.h"
//-------------------------------------- Методы класса V4fWriter ----------------------------------
V4fWriter::V4fWriter(std::wstring newCameraName)
{
	dataSet = 0;
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
		try
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
				if(current_start_time!=CURRENT_POSITION().Value().start_time)
				{
					this->Open();
				}
				int bufLen = buf.size();
				V4fFrame new_frame(CURRENT_DPP().Value(), CURRENT_POSITION().Value().absolutePosition, frame.cols, frame.rows, buf);
				dataSet->WriteFrame(new_frame.dpp, new_frame.absCoord, ENCODE_CV_50, new_frame.height, new_frame.width, new_frame.img.data(), new_frame.img.size());
			}
		}
		catch (std::exception e)
		{
			LOG_ERROR(L"Ошибка при записи кадра cv::Mat (Обзорное видеонаблюдение)");
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
			if(current_start_time!=CURRENT_POSITION().Value().start_time)
			{
				LOG_INFO(L"Изменился идентификатор поездки");
				this->Release();
				this->Open();
			}
			V4fFrame new_frame(CURRENT_DPP().Value(), CURRENT_POSITION().Value().absolutePosition, width, height, bufData, bufDataLen);
			dataSet->WriteFrame(new_frame.dpp, new_frame.absCoord, ENCODE_CV_50, new_frame.height, new_frame.width, new_frame.img.data(), new_frame.img.size());
		}
	}
}

bool V4fWriter::Open()
{
	if (Opened || CURRENT_POSITION().Value().dir <= 0) 
		return false;

	try
	{
		SECURITY_ATTRIBUTES sa; 
		sa.nLength= sizeof(sa);
		sa.lpSecurityDescriptor= NULL;			

		time_t now = time(NULL);
		localtime(&now);
		char hms[32]="";
		char full_path [MAX_PATH] = "", short_path [128] = "";
		strftime(hms, 31, "%Hh%Mm%Ss", localtime(&now));
		_snprintf(short_path, 127, "%s_%s.v4f", wstring_to_string(cameraName).c_str(), hms);
		CreateFullPath(full_path, CURRENT_POSITION().Value().dir, CURRENT_POSITION().Value().way,
			CURRENT_POSITION().Value().start_time, short_path, wstring_to_string(VIDEO_OPTIONS().Value().video_server));
		fullPath = string_to_wstring(full_path);

		dataSet = new V4fDataSet(wstring_to_string(fullPath), short_path, CURRENT_POSITION().Value().dir, CURRENT_POSITION().Value().way, CURRENT_POSITION().Value().start_time);
		if(dataSet)
		{
			dataSet->OpenWriteFile();
			dataSet->WriteHeader();
			Opened = true;
			current_start_time = CURRENT_POSITION().Value().start_time;
			return true;
		}
	}
	catch(std::exception e)
	{
		LOG_ERROR(L"Ошибка при открытии файла .v4f");
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