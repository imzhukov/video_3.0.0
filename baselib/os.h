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

/// «десь определены типы uint32_t и т.п.
#include "c4stdint.h"
