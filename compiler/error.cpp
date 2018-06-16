#include "error.h"


ErrorSys::ErrorSys()
{
	this->fatals = 0;
	this->warnings = 0;
}

void ErrorSys::Error(int error, int line, ...)
{
	va_list ap;
	va_start(ap, line);
	
	fatals++;

	char errorstr[128];
	vsnprintf(errorstr, sizeof(errorstr), errors[error], ap);

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