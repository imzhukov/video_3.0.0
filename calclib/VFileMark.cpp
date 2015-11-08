#include "VFileMark.h"

// конструктор
VFileMark::VFileMark() : index(NOT_FOUND), 
						 mark_type(FILE_LABEL_UNDEFINED), 
						 value(0.0f)
{
	::ZeroMemory(this->reserve, sizeof(reserve));
}
