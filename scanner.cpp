#include "scanner.h"

Scanner::Scanner(const char *filename) : in(fin)
{
	fin.open(filename, std::ios::in);

	if (!fin.good())
	{
		char error[255];
		strerror_s(error, sizeof(error), errno);
		
		printf("// Error: %s: \"%s\"\n", error, filename);
		exit(EXIT_FAILURE);
	}

#ifdef SCANNER_DEBUG
	printf("Creating scanner w/ buffer len of %d\n", bufferlen);
#endif
}

bool Scanner::Peek(char in)
{
	char temp = this->Next();
	this->Back();

	return temp == in;
}

char Scanner::Next()
{
	if (pos == chars.size() && !fin.eof()) // fetch more if we even can
	{
#ifdef SCANNER_DEBUG
		printf("Next() call resulted in requirement to grab more chars\n");
#endif

		chars.push_back(in.Get()); // grabs the next char from CharFetcher & pushes to next pos
	}
	
	if (pos != chars.size())
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