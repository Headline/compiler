#include "error.h"

ErrorSys::ErrorSys()
{
	phrases.push_back("empty statement");							// 0
	phrases.push_back("expected token '%s'");						// 1
	phrases.push_back("unexpected token '%s'");						// 2
	phrases.push_back("expected built-in type but got '%s'");		// 3
}

void ErrorSys::Error(int error, int line, ...)
{
	va_list ap;
	va_start(ap, line);
	
	fatals++;

	char errorstr[128];
	vsnprintf(errorstr, sizeof(errorstr), phrases[error].c_str(), ap);

	char errorstring[256];
	snprintf(errorstring, sizeof(errorstring), "[Error] [Line %d]: %s", line, errorstr);
	output.push_back(errorstring);
	va_end(ap);

}

void ErrorSys::Spew() const
{
	for (auto str : output) {
		fprintf(stderr, "%s\n", str.c_str());
	}

	printf("Compiler exited with %d errors!\n", output.size());

	printf("\n");
}

bool ErrorSys::Fatal() const
{
	return fatals > 0;
}