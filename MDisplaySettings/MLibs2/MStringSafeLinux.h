#pragma once

#ifndef MPLATFORM_WIN32_MSVC
inline int strcpy_s(char *strDestination, size_t numberOfElements, const char *strSource)
{
	return snprintf(strDestination, numberOfElements, "%s", strSource);
}
#endif

