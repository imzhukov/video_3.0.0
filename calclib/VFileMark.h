#pragma once

#include "calclib.h"

/// типы меток в файле
enum FILE_LABEL_TYPE
{
	FILE_LABEL_UNDEFINED	= 0,	/// неопределено, ошибка
	FILE_LABEL_PROF_LINE	= 1,	/// профиль, начало прямой
	FILE_LABEL_PROF_CRV		= 2,	/// профиль, начало кривой
	FILE_LABEL_LINE			= 3,	/// прямая
	FILE_LABEL_CRV_PLAN		= 4,	/// кривая, на плане
	FILE_LABEL_CRV_LEVEL	= 5		/// кривая, на уровне
};

/// метки в файле - общая структура
struct VFileMark
{
	uint32_t index;				/// номер кадра
	int32_t mark_type;			/// тип метки
	float value;				/// величина параметра
	uint8_t reserve[52];		/// выравнивание до 64 байт

	// конструктор
	VFileMark();

};