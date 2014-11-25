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
	
	printf("%s\n", buffer);
}

std::string Format(const char* format, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	
	return std::string(buffer);
}

void String_Split(std::vector<std::string> &output, const std::string &text, char regex)
{
	std::string current = "";
	
	for (unsigned int i = 0; i < text.length(); i++)
	{
		char ch = text.at(i);

		if (ch == regex)
		{
			output.push_back(current);
			current = "";
		}
		else
		{
			current += ch;
		}
	}	

	if (!current.empty())
	{
		output.push_back(current);
	}
}