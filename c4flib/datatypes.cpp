#include "datatypes.h"

/* Возвращает название C типа по его коду*/
const wchar_t * GetCTypeName(int code)
{
	switch (code)
	{
		case VCT_FLOAT:		return L"float";
		case VCT_DOUBLE:	return L"double";
		case VCT_INT:		return L"int";
		case VCT_UINT:		return L"unsigned int";
		case VCT_LONG:		return L"long";
		case VCT_ULONG:		return L"unsigned long";
		case VCT_CHAR:		return L"char";
		case VCT_UCHAR:		return L"unsigned char";
		case VCT_SHORT:		return L"short";
		case VCT_USHORT:	return L"unsigned short";
		default:			return L"определяется пользователем";
	}
};
