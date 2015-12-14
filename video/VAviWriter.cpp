#include "VAviWriter.h"

VAviWriter::VAviWriter(/*std::wstring f, */ std::wstring newCameraName,
		int newFrame_rate, int newCodec, int newWidth, int newHeight)
{
	///Создаем папку для записей
	SECURITY_ATTRIBUTES sa; 
	sa.nLength= sizeof(sa);
	sa.lpSecurityDescriptor= NULL;
	//folder = f;
	cameraName = newCameraName;
	frame_rate = newFrame_rate;
	codec = newCodec;
	width = newWidth;
	height = newHeight;
	writer=0;
}

VAviWriter::~VAviWriter()
{
	if (writer!=NULL && writer->isOpened())
		writer->release();
}

bool VAviWriter::IsOpened()
{
	if(writer!=NULL)
		return writer->isOpened();
	else
		return false;
}
void VAviWriter::AddFrame(cv::Mat frame)
{
	if (!frame.empty() && writer!=NULL && writer->isOpened())
	{
		//cv::Mat frame;

		*writer << frame;
	}
}
bool VAviWriter::Open()
{
	//if(CURRENT_POSITION().Value().dir <= 0)
		//return false;
	///Создаем папку для записи изображений
	SECURITY_ATTRIBUTES sa; 
	sa.nLength= sizeof(sa);
	sa.lpSecurityDescriptor= NULL;
	///Часть названия папки, содержащая дату и время
	time_t now = time(NULL);
	localtime(&now);
	char ymd[22]="", hms[32]="", dir_way[64]="";
	strftime(ymd, 21, "%Y%m%d", localtime(&now));
	strftime(hms, 31, "%Hh%Mm%Ss", localtime(&now));
	_snprintf(dir_way,63,"%i_%s",CURRENT_POSITION().Value().direction, CURRENT_POSITION().Value().way);
	std::wstring finalPath = string_to_wstring((const char *) ymd) + L"_" + string_to_wstring((const char *) dir_way);
	
	const wchar_t * ch = VIDEO_OPTIONS().Value().video_server.c_str();
	if(ch[wcslen(ch)-1] == L'\\')
		finalPath = VIDEO_OPTIONS().Value().video_server + finalPath;
	else
		finalPath = VIDEO_OPTIONS().Value().video_server + L"\\" + finalPath;
	CreateDirectory(finalPath.c_str(), &sa);

	VFileName filename;
	filename.SetFolder(finalPath);
	std::wstringstream short_filename;
	///Имя видеофайла строится по шаблону: Имя камеры + Время записи
	wchar_t shtFileName [1024];
	swprintf(shtFileName, 1023, L"%s_%s.avi", cameraName.c_str(), string_to_wstring(hms).c_str());
	filename.SetShortName(shtFileName);
	fullPath = filename.GetPath();
	if (frame_rate <= 0 || width <= 0 || height <= 0)
	{
		throw VSimpleException(L"Не удалось начать запись, проверьте подключение камеры. ", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		return false;
	}
	try
	{	
		writer = new VideoWriter(wstring_to_string(filename.GetPath().c_str()), codec, frame_rate, Size(width, height), true);
		//writer->open(wstring_to_string(filename.GetPath().c_str()), codec, frame_rate, Size(width, height), true);
	}
	catch (...)
	{
		LOG_ERROR(L"Не удалось начать запись в файл: " + filename.GetPath());
		return false;
	}
	
	if (!writer->isOpened())
	{
		throw VSimpleException(L"Не удалось открыть файл для записи.", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		return false;
	}
	else
		return true;
}
bool VAviWriter::Release()
{
	if (writer!=NULL && writer->isOpened())
	{
		writer->release();
		return true;
	}
	else
		return false;
}