#include "VHtml.h"

std::string VHtml::GetHeader(const std::string & header)
{
	std::string result = "<!DOCTYPE html>\n<head>\n"
		"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\n";

	if (!header.empty())
		result += "<title>" + header + "</title>\n";
	result += "\n";

	result += "<style>\n";
	result +=	"body { font-family: tahoma, sans-serif; font-size: 10pt; }\n"
				"table { border: solid medium; border-collapse: collapse; font-family: tahoma, sans-serif; font-size: 10pt; }\n"
				"colgroup, thead { border: solid medium; }\n"
				"th { border: solid thin; background: beige; }\n"
				"td { border: solid thin; border-bottom: dotted thin; text-align: right; }\n"
				"tbody { border-bottom: solid medium; }\n"
				"tr.curve td, tr.curve th, tr.radius td.pere { border-bottom: dashed thin; }\n"
				"tr.radius td, tr.radius th { border-top: none; border-bottom: none; }\n";

	result += "</style>\n</head>\n\n";
	result += "<body>\n";

	return result;
}

std::string VHtml::GetFooter()
{
	return "</body>\n</html>\n";
}

std::ostream & operator << (std::ostream & os, const VWayCoord & coord)
{
	char buf[32] = {0};
	_snprintf(buf, 32, "%li.%03.0f", coord.km, coord.m);
	os << static_cast<const char*>(buf);
	return os;
}
