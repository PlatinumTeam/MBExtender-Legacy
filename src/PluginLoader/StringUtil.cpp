#include <algorithm>
#include <ctype.h>
#include "StringUtil.h"

std::string strToUpper(const std::string &str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(), ::toupper);
	return result;
}