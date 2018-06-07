#include "error.h"

ErrorSys::ErrorSys()
{
	phrases.push_back("empty statement");
	phrases.push_back("expected token <identifier>");
	phrases.push_back("expected token '('");
	phrases.push_back("expected token ')'");
}

void ErrorSys::Error(int error, int line)
{
	fatals++;

	char errorbuffer[128]; // should definitely be big enough, we're not a c++ compiler :^)
	snprintf(errorbuffer, sizeof(errorbuffer), "[Error] [Line %d]: %s", line, phrases[error].c_str());
	output.push_back(errorbuffer);
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