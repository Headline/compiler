#include <iostream>
#include <fstream>
#include <memory>

#include "scanner.h"
#include "tokenizer.h"
#include "parser.h"

inline bool endswith(const std::string &value, const std::string &ending)
{
	if (ending.size() > value.size())
		return false;

	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

int main(int argc, char *argv[])
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
			stderr = freopen(argv[1] + 8, "w+", stderr);
	}
	if (!endswith(argv[argc-1], ".x"))
	{
		fprintf(stderr, "Error: source file must end in .x");
		return 0;
	}

	std::unique_ptr<Scanner> scanner = std::make_unique<Scanner>(argv[argc-1]);
#else
	std::unique_ptr<Scanner> scanner = std::make_unique<Scanner>("test.x");
#endif
	std::unique_ptr<Tokenizer> tokenizer = std::make_unique<Tokenizer>(scanner);
	std::unique_ptr<ErrorSys> errsys = std::make_unique<ErrorSys>();
	std::unique_ptr<Parser> parser = std::make_unique <Parser>(tokenizer, errsys.get());

	parser->Parse();

	if (!errsys->Fatal())
		printf("Compilation ended successfully.\n");
    return 0;
}
