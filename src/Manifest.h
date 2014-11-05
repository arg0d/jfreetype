#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include "Vector2.h"
#include <stdarg.h>

void Log(const char *format, ...) 
{
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	
	printf(buffer, "");
}