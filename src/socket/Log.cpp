#include "Log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
using namespace libnetwork;

static const int MAXLEN = 1024;


void Log::info(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char buf[MAXLEN + 1] = { 0 };
	vsnprintf(buf, MAXLEN, fmt, ap);
	va_end(ap);
	
	printf("%s\n", buf);
}

void Log::error(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char buf[MAXLEN + 1] = { 0 };
	vsnprintf(buf, MAXLEN, fmt, ap);
	va_end(ap);

	const int n = strlen(buf);
	const int no = errno;
	snprintf(buf + n, MAXLEN - n, ", errno:%d:%s", no, strerror(no));
	printf("%s\n", buf);
}
