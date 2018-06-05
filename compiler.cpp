#include <iostream>
#include <fstream>
#include <memory>

#include "scanner.h"

inline bool endswith(const std::string &value, const std::string &ending)
{
	if (ending.size() > value.size())
		return false;

	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

int main(int argc, char *argv[])
{
	printf("// Experimental Compiler written by Michael Flaherty\n");
	printf("// (c) 2018\n");
	printf("//\n");

#ifndef DEFAULT_TEST_FILE
	if (argc != 2)
	{
		printf("// Usage: compiler.exe <filename>\n");
		return 0;
	}

	if (!endswith(argv[1], ".ds"))
	{
		printf("// Error: source file must end in .ds");
		return 0;
	}
#endif

#ifndef DEFAULT_TEST_FILE
	std::unique_ptr<Scanner> scanner = std::make_unique<Scanner>(argv[1], 30);
#else
	std::unique_ptr<Scanner> scanner = std::make_unique<Scanner>("test.ds", 30);
#endif



	std::string str;
	char c;
	while ((c = scanner->Next()) != EOF) {
		str.append(1, c);
	}

	printf("%s\n", str.c_str());
	getchar();
    return 0;
}

