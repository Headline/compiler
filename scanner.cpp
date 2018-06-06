#include "scanner.h"

Scanner::Scanner(const char *filename, int bufferlen) : bufferlen(bufferlen)
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

	assert(bufferlen > 0);
	
	GrabChars();
}

void Scanner::GrabChars()
{
	int len = bufferlen;
	while (!!len)
	{
		char c;
		fin.get(c);
		if (fin.eof())
			return;

		if (c == '\n')
			continue;

		chars.push_back(c);
		len--;

#ifdef SCANNER_DEBUG
		printf("Retrieved char %c, pushed to end of buffer\n", c);
#endif
	}
}

char Scanner::Next()
{
	if (pos == chars.size() && !fin.eof()) // fetch more if we even can
	{
#ifdef SCANNER_DEBUG
		printf("Next() call resulted in requirement to grab more chars\n");
#endif
		GrabChars();
	}
	
	if (pos != chars.size())
	{
		char temp = chars[pos];
		if (temp == '\n') // hit a new line, lets inc 
			line++;

		pos++;
		return temp;
	}
	else
		return EOF;
}

void Scanner::Back()
{
	assert(pos > 0);


	if (chars[pos] == '\n') // we moved back, lets dec
		line--;

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
	return line;
}