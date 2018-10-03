#include "scanner.h"

#include <gsl/gsl>

Scanner::Scanner(gsl::not_null<char const *> filename) : in(fin), pos(0)
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
		printf("// Error: %s: \"%s\"\n", error, filename.get());
		exit(EXIT_FAILURE);
	}
#ifdef SCANNER_DEBUG
	printf("Creating scanner\n");
#endif
}

bool Scanner::Peek(char input) noexcept
{
	char temp = this->Next();
	this->Back();

	return temp == input;
}

char Scanner::Next() noexcept
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

void Scanner::Back() noexcept
{
	Expects(pos > 0);
	pos--;

}

int Scanner::Position() const noexcept
{
	return pos;
}

size_t Scanner::BufferSize() const noexcept
{
	return chars.size();
}

int Scanner::GetLineNumber() const noexcept
{
	return in.LineCount();
}