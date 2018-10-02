#include "error.h"

ErrorSys::ErrorSys()
{
	this->fatals = 0;
	this->warns = 0;
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

void ErrorSys::Warning(int warning, int line, ...)
{
	va_list ap;
	va_start(ap, line);

	warns++;

	char errorstr[128];
	vsnprintf(errorstr, sizeof(errorstr), warnings[warning], ap);

	char errorstring[256];
	snprintf(errorstring, sizeof(errorstring), "[Warning] [Line %d]: %s", line, errorstr);
	warningoutput.push_back(errorstring);
	va_end(ap);
}

void ErrorSys::Spew() const
{
	for (auto str : warningoutput) {
		fprintf(stderr, "%s\n", str.c_str());
	}
	for (auto str : output) {
		fprintf(stderr, "%s\n", str.c_str());
	}

	char warnmsg[64];

	if (warns > 0)
	{
		snprintf(warnmsg, sizeof(warnmsg), " and %d warnings", warns);
	}
	else
	{
		memset(warnmsg, 0, sizeof(warnmsg));
	}

	printf("Compiler exited with %d errors%s!\n", fatals, strlen(warnmsg) ? warnmsg : "");
	printf("\n");
}

bool ErrorSys::Fatal() const
{
	return fatals > 0;
}

void ErrorSys::Exit(Parser *parser) const
{
	if (parser)
		parser->Validate();

	this->Spew();

	if (!this->Fatal())
		printf("Compilation ended successfully.\n");
    
	exit(0);
}