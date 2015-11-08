#include "V4fDataSet.h"

V4fDataSet::V4fDataSet(std::string in_full_path, std::string in_short_name) : 
	full_path(in_full_path), short_name(in_short_name), start_code_dpp(-1), last_code_dpp(-1)
{}

V4fDataSet::V4fDataSet(std::string in_full_path, std::string in_short_name, long & _dir, 
			char * _way, int64_t _start_time)
		 : start_code_dpp(-1), last_code_dpp(-1)
{
	full_path = in_full_path;
	short_name = in_short_name;
	dir = _dir;
	strcpy(way, _way);
	start_time = _start_time;
	handler = 0;
}

V4fDataSet::V4fDataSet(const V4fDataSet & ds)
{
	this->full_path = ds.full_path;
	this->short_name = ds.short_name;
	this->dir = ds.dir;
	strcpy(this->way, ds.way);
	this->start_time = ds.start_time;
	handler = 0;
}

V4fDataSet::~V4fDataSet()
{
	if(handler)
		fclose(handler);
}

int V4fDataSet::OpenWriteFile()
{
	if(handler)
		fclose(handler);
	handler = fopen(full_path.c_str(), "wb");
	if(handler!= NULL)
		return 0;
	else
		return 1;
}

int V4fDataSet::OpenReadFile()
{
	handler = fopen(full_path.c_str(), "rb");
	if(handler!= NULL)
		return 0;
	else
		return 1;
}

int V4fDataSet::WriteHeader()
{
	if(handler)
	{
		try
		{
			fwrite(&V4F_VERSION, 4, 1, handler);
			//Пишем стандартный заголовок из с4f
			fwrite(&dir, 8, 1, handler);
			fwrite(way, 8, 1, handler);
			fwrite(&start_time, 8, 1, handler);
		}
		catch(std::exception & excp)
		{
			return 2;
		}
		return 0;
	}
	else
		return 1;
}

int V4fDataSet::ReadHeader()
{
	if(handler)
	{
		try
		{
			fread(&versionFile, 4, 1, handler);
			if(versionFile == V4F_VERSION)
			{
				fread(&dir, 8, 1, handler);
				fread(&way, 8, 1, handler);				
				fread(&start_time, 8, 1, handler);
			}
		}
		catch(std::exception & excp)
		{
			return 2;
		}
		return 0;
	}
	else
		return 1;
}

void V4fDataSet::WriteFrame(int codeDpp, double absCoord, int codeEncoding, int height, int width, unsigned char * imgData, int imgDataLen)
{
	if(handler)
	{
		fwrite(&codeDpp, 4, 1, handler);
		fwrite(&absCoord, 8, 1, handler);
		fwrite(&codeEncoding, 4, 1, handler);
		fwrite(&width, 4, 1, handler);
		fwrite(&height, 4, 1, handler);
		fwrite(&imgDataLen, 4, 1, handler);
		fwrite(imgData, imgDataLen, 1, handler);
	}
}

void V4fDataSet::LoadFrames()
{
	while(!feof(handler))
	{
		V4fFrame frm;
		frm.LoadCoordsWithoutImage(handler);
		if(frm.width > 0 && frm.height > 0)
			frames.push_back(frm);
	}
}

void V4fDataSet::CloseFileAndClear()
{
	if(handler)
	{
		fclose(handler);
		frames.clear();
	}
}

long V4fDataSet::GetDir()
{
	return dir;
}

char * V4fDataSet::GetWay()
{
	return way;
}

int64_t V4fDataSet::GetTime()
{
	return start_time;
}

std::vector<V4fFrame> & V4fDataSet::GetFrames()
{
	return frames;
}

void V4fDataSet::LoadImageW(int pos, unsigned char * bufData, int bufDataLen)
{
	frames[pos].LoadV4fImage(handler, bufData, bufDataLen);
}

int V4fDataSet::GetStartCodeDpp()
{
	return start_code_dpp;
}

int V4fDataSet::GetLastCodeDpp()
{
	return last_code_dpp;
}

std::string V4fDataSet::GetShortName()
{
	return short_name;
}

void V4fDataSet::SetShortName(std::string in_short_name)
{
	short_name = in_short_name;
}

std::string V4fDataSet::GetFullPath()
{
	return full_path;
}

void V4fDataSet::SetFullPath(std::string in_full_path)
{
	full_path = in_full_path;
}