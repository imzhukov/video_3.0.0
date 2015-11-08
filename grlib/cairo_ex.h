#pragma once

#include <string>
#include <cairo.h>

#include "strlib.h"

/*
Расширения и дополнения библиотеки Cairo
*/

/** Перегруженная функция cairo_show_text*/
void cairo_show_text(cairo_t * cr, const wchar_t * text);
/** Перегруженная функция cairo_text_extents для wchar_t*/
void cairo_text_extents(cairo_t * cr, const wchar_t * text, cairo_text_extents_t *extents);
/** Перегруженная функция cairo_show_text*/
void cairo_show_text(cairo_t * cr, const std::string& text);
/** Перегруженная функция cairo_show_text для std::wstring*/
void cairo_show_text(cairo_t * cr, const std::wstring& text);
/** Перегруженная функция cairo_text_extents для std::wstring*/
void cairo_text_extents(cairo_t * cr, const std::wstring& text, cairo_text_extents_t *extents);
/** Установка толщины линии*/
void cairo_set_line_width_ex(cairo_t * cr, double width);


class VCairoSurface
{
private:
	cairo_surface_t * surface;
	VCairoSurface(const VCairoSurface&) {};
	VCairoSurface& operator=(const VCairoSurface&) {};
public:
	VCairoSurface(cairo_surface_t * in_surface) : surface(in_surface) {};
	~VCairoSurface() {cairo_surface_destroy(surface);};
	cairo_surface_t * operator()() {return surface;};
	bool Ok() const {return cairo_surface_status(surface)==CAIRO_STATUS_SUCCESS;};
	std::wstring ErrorMessage() const {return string_to_wstring(cairo_status_to_string(cairo_surface_status(surface)));};
};

class VCairoContext
{
private:
	cairo_t * cr;
	VCairoContext(const VCairoContext&) {};
	VCairoContext& operator=(const VCairoContext&) {};
public:	
	VCairoContext(cairo_t * in_cr) : cr(in_cr) {};
	~VCairoContext() {cairo_destroy(cr);};
	cairo_t * operator()() {return cr;};
	bool Ok() const {return cairo_status(cr)==CAIRO_STATUS_SUCCESS;};
	std::wstring ErrorMessage() const {return string_to_wstring(cairo_status_to_string(cairo_status(cr)));};
};