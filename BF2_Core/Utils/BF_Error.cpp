#include "BF_Error.h"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <cstdlib>
#include <cassert>
#include <cstdio>

void errorF(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	std::vfprintf(stderr, format, args);
	va_end(args);

	// Default newline and flush (like std::endl)
	std::fputc('\n', stderr);
	std::fflush(stderr);
}

void panicF(const char * format, ...)
{
	va_list args;
	char buffer[2048] = { '\0' };

	va_start(args, format);
	std::vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	MessageBoxA(nullptr, buffer, "Fatal Error", MB_OK);
	std::abort();
}

void debugF(const char * format, ...)
{
#ifdef _DEBUG
	va_list args;
	char buffer[2048] = { '\0' };

	va_start(args, format);
	std::vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	OutputDebugString(buffer);
#endif
}
