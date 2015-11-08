#pragma once

#include <string>
#include <ostream>

#include "types.h"

class VHtml
{
public:

	static std::string GetHeader(const std::string & header);
	static std::string GetFooter();
};

std::ostream & operator << (std::ostream & os, const VWayCoord & coord);
