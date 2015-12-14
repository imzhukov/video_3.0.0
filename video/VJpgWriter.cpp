#include "VJpgWriter.h"

/*--------------------------------------Методы классы VJpgWriter---------------------------------*/
VJpgWriter::VJpgWriter(/*std::wstring f, */ std::wstring newCameraName,
					 int newFrame_rate, int newWidth, int newHeight, int newIntervalRecording)
{
	
	///Устанавливаем значения по умолчанию для внутренних переменных
	lastPosition = 0.0;
	//folder = f;
	Opened = false;
	isWayStarted = false;
	frame_rate = newFrame_rate;
	width = newWidth;
	height = newHeight;
	cameraName = newCameraName;
	count = 0;
	intervalRecording = newIntervalRecording;
}

VJpgWriter::~VJpgWriter()
{
}

bool VJpgWriter::IsOpened()
{
	return Opened;
}

void VJpgWriter::AddFrame(cv::Mat frame)
{
	if (!frame.empty() && Opened && CURRENT_POSITION().Value().abs_coord > 0)
	{
		/*if (isWayStarted)
		{*/
		SECURITY_ATTRIBUTES sa; 
		sa.nLength= sizeof(sa);
		sa.lpSecurityDescriptor= NULL;			
		double distance = CURRENT_POSITION().Value().abs_coord - lastPosition;
		if (abs(distance) >= ((double)intervalRecording))
		{
			time_t now = time(NULL);
			char buff[22];
			strftime(buff, 22, "%Y%m%d", localtime(&now)); // %Hh%Mm%Ss
			char _jpgDir [256] = "";
			_snprintf(_jpgDir, 255, "%s_%i_%s", buff, CURRENT_POSITION().Value().direction, CURRENT_POSITION().Value().way);
			std::wstring new_jpgDir(string_to_wstring((const char *) _jpgDir));
			std::wstring fullPath;
			const wchar_t * ch = VIDEO_OPTIONS().Value().video_server.c_str();
			if(ch[wcslen(ch)-1] == L'\\')
				fullPath = VIDEO_OPTIONS().Value().video_server + new_jpgDir;
			else
				fullPath = VIDEO_OPTIONS().Value().video_server + L"\\" + new_jpgDir;
			if(new_jpgDir != jpgDir)
			{
				CreateDirectory(fullPath.c_str(), &sa);
			}
			jpgDir = new_jpgDir;
			///Сохраняем кадр в указанной ранее папке
			char fileName [256] = "";
			_snprintf(fileName, 255, "%s_%ikm%im.jpg", wstring_to_string(cameraName).c_str(), CURRENT_POSITION().Value().km, CURRENT_POSITION().Value().m);
			fullPath += (L"\\" + string_to_wstring((const char *) fileName));
			imwrite(wstring_to_string(fullPath),frame);
			lastPosition = CURRENT_POSITION().Value().abs_coord;
		}
	}
	else
		Opened = false;
}
bool VJpgWriter::Open()
{
	if (width <= 0 || height <= 0) 
	{
		Opened = false;
		throw VSimpleException(L"Не удалось начать запись, проверьте подключение камеры.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		return false;
	}
	if (Opened || CURRENT_POSITION().Value().direction <= 0) 
		return false;
	else
	{
		Opened = true;
		///Создаем папку для записи изображений
		SECURITY_ATTRIBUTES sa; 
		sa.nLength= sizeof(sa);
		sa.lpSecurityDescriptor= NULL;
		///Часть названия папки, содержащая дату и время
		time_t now = time(NULL);
		char buff[22];
		strftime(buff, 22, "%Y%m%d", localtime(&now)); // %Hh%Mm%Ss
		char _jpgDir [256] = "";
		_snprintf(_jpgDir, 255, "%s_%i_%s", buff, CURRENT_POSITION().Value().direction, CURRENT_POSITION().Value().way);
		jpgDir = std::wstring(string_to_wstring((const char *) _jpgDir));
		std::wstring fullPath;
		const wchar_t * ch = VIDEO_OPTIONS().Value().video_server.c_str();
		if(ch[wcslen(ch)-1] == L'\\')
			fullPath = VIDEO_OPTIONS().Value().video_server + jpgDir;
		else
			fullPath = VIDEO_OPTIONS().Value().video_server + L"\\" + jpgDir;
		CreateDirectory(fullPath.c_str(), &sa);
	}
	return true;
}
bool VJpgWriter::Release()
{
	if (Opened)
	{
		Opened = false;
		fout.close();
		return true;
	}
	else
		return false;
}