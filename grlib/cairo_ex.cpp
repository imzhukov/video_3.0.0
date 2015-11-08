#include "cairo_ex.h"

#ifdef _WIN32
	#include <windows.h>
#endif

/** ������������� ������� cairo_show_text ��� wchar_t*/
void cairo_show_text(cairo_t * cr, const wchar_t * text)
{	
	//������������� �� wchar_t � UTF-8	
#ifdef _WIN32
	//����� ������������ ������� WINAPI, ��� ��� setlocale �� ������������ UTF-8 � MSVC
	size_t req_size=WideCharToMultiByte(CP_UTF8, 0, text, -1, NULL, 0, NULL, NULL);//(wcslen(text)+1)*sizeof(wchar_t);
	char * target=static_cast<char*>(malloc(req_size));
	memset(target,0, req_size);
	WideCharToMultiByte(CP_UTF8, 0, text, -1, target, req_size, NULL, NULL); 
	cairo_show_text(cr, target);
	free(target);		
#else
	//����� ����� ������������ setlocale � mbstowcs
	cairo_show_text(cr, "Russian text supported only in windows.";
#endif
}

/** ������������� ������� cairo_text_extents ��� wchar_t*/
void cairo_text_extents(cairo_t * cr, const wchar_t * text, cairo_text_extents_t *extents)
{
	//������������� �� wchar_t � UTF-8	
#ifdef _WIN32
	//����� ������������ ������� WINAPI, ��� ��� setlocale �� ������������ UTF-8 � MSVC
	size_t req_size=WideCharToMultiByte(CP_UTF8, 0, text, -1, NULL, 0, NULL, NULL);//(wcslen(text)+1)*sizeof(wchar_t);
	char * target=static_cast<char*>(malloc(req_size));
	memset(target,0, req_size);
	WideCharToMultiByte(CP_UTF8, 0, text, -1, target, req_size, NULL, NULL); 
	cairo_text_extents(cr, target, extents);
	free(target);		
#else
	//����� ����� ������������ setlocale � mbstowcs
	cairo_show_text(cr, "Russian text supported only in windows.";
#endif
}

/** ������������� ������� cairo_show_text ��� std::string*/
void cairo_show_text(cairo_t * cr, const std::string& text)
{
#ifdef _WIN32
	//������� ������� ������ � UNICODE
	size_t req_size_wide=(text.size()+1)*sizeof(wchar_t);
	wchar_t * wide=static_cast<wchar_t*>(malloc(req_size_wide));
	memset(wide,0, req_size_wide);
	MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, wide, text.size());//req_size_wide

	cairo_show_text(cr, wide);		

	free(wide);
#else
	//����� ����� ������������ setlocale � mbstowcs
	cairo_show_text(cr, "Russian text supported only in windows.";
#endif
}

/** ������������� ������� cairo_show_text ��� std::wstring*/
void cairo_show_text(cairo_t * cr, const std::wstring& text)
{	
	cairo_show_text(cr, text.c_str());
}

/** ������������� ������� cairo_text_extents ��� std::wstring*/
void cairo_text_extents(cairo_t * cr, const std::wstring& text, cairo_text_extents_t *extents)
{
	cairo_text_extents(cr, text.c_str(), extents);
}

/** ��������� ������� �����. ��� ������� ����� ���������� � ��� ������, ���� ����������� 
    �������������� ����������� �� ������, ��� ������ �� ������� ��� �� �����*/
void cairo_set_line_width_ex(cairo_t * cr, double width)
{
	double ux=width, uy=width;
	cairo_device_to_user_distance (cr, &ux, &uy);
	if (ux < uy)
		ux = uy;	
	if (ux<1)
		ux=1;
	cairo_set_line_width(cr, ux);	
}