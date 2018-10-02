#include <iostream>
#include <fstream>
#include <memory>

#include "debug.h"
#include "scanner.h"
#include "tokenizer.h"
#include "parser.h"

inline bool endswith(std::string const &value, std::string const &ending)
{
	if (ending.size() > value.size())
		return false;

	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

#ifndef DEFAULT_TEST_FILE
int main(int argc, char *argv[])
#else
int main()
#endif
{
	printf("Compiler Version %s\n", COMPILER_VERSION);
	printf("(c) Michael Flaherty 2018\n");
	printf("\n");

#ifndef DEFAULT_TEST_FILE
	if (argc < 2)
	{
		fprintf(stderr, "Usage: compiler.exe <filename>\n");
		return 0;
	}
	if (strncmp(argv[1], "-stderr=", 8) == 0)
	{
		if (strlen(argv[1]+8))
		{
#ifdef PLATFORM_WINDOWS
			FILE *junk = nullptr;
			freopen_s(&junk, argv[1] + 8, "w+", stderr);
#else
			freopen(argv[1] + 8, "w+", stderr);
#endif
		}
	}
	if (!endswith(argv[argc-1], ".x"))
	{
		fprintf(stderr, "Error: source file must end in .x");
		return 0;
	}

	Scanner scanner(argv[argc-1]);
#else
	Scanner scanner("test.x");
#endif
	Tokenizer tokenizer(scanner);
	ErrorSys errsys;
	Parser parser(tokenizer, errsys);

	parser.Parse();
}
