#include "error.h"

ErrorSys::ErrorSys()
{
	phrases.push_back("empty statement");
	phrases.push_back("expected token '%s'");

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

void ErrorSys::Spew()
{
	printf("// compiler exited with %d errors!\n", output.size());
	for (auto str : output) {
		printf("// %s\n", str.c_str());
	}
}

bool ErrorSys::Fatal()
{
	return fatals > 0;
}