#include "scanner.h"

Scanner::Scanner(const char *filename) : in(fin), pos(0)
{
	fin.open(filename, std::ios::in);

	if (!fin.good())
	{
		char error[255];
#if defined PLATFORM_APPLE || defined PLATFORM_LINUX
		strerror_r(errno, error, sizeof(error));
#else
		strerror_s(error, sizeof(error), errno);
#endif
		printf("// Error: %s: \"%s\"\n", error, filename);
		exit(EXIT_FAILURE);
	}

#ifdef SCANNER_DEBUG
	printf("Creating scanner\n");
#endif
}

bool Scanner::Peek(char input)
{
	char temp = this->Next();
	this->Back();

	return temp == input;
}

char Scanner::Next()
{
	if (pos == chars.size() && !fin.eof()) // fetch more if we even can
	{
		chars.push_back(in.Get()); // grabs the next char from CharFetcher & pushes to next pos
	}
	
	if (pos < chars.size())
		return chars[pos++];
	else
		return EOF;
}

void Scanner::Back()
{
	assert(pos > 0);
	pos--;

}

int Scanner::Position() const
{
	return pos;
}

size_t Scanner::BufferSize() const
{
	return chars.size();
}

int Scanner::GetLineNumber() const
{
	return in.LineCount();
}