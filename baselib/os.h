#pragma once

#include <boost/asio.hpp>
#ifdef _WIN32	
	#include "windows.h"
#else //Linux	
	#include "wchar.h"
#endif

#ifdef _MSC_VER	
	#include "tchar.h"	
#else	
	#define Ls) L##s	
#endif

/// ����� ���������� ���� uint32_t � �.�.
#include "c4stdint.h"
