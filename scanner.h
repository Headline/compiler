#ifndef H_SCANNER // double include protection
#define H_SCANNER

#include "debug.h"

#include <stdio.h>
#include <vector>
#include <cassert>
#include <fstream>


/**
 * CharFetcher is our method for grabbing the input file one line at a time,
 * and then stepping through each character of the line. CharFetcher grabs the
 * data, Scanner distrubutes and requests more.
 *
 * It is more efficient to grab input line-by-line, and then step through the
 * input array by hand than consequent calls to getchar(). That is the purpose
 * of this class. It's also significantly easier to handle line number tracking.
 */
class CharFetcher
{
public:
	CharFetcher(std::ifstream &stream) : stream(stream), next(0), lines(0) {
	};

	char Get() {
		if (stream.eof())
			return EOF;

		if (next >= line.size() || line[next] == '\0') { // if no more chars left

			char buffer[1024]; // is this bad?
			stream.getline(buffer, sizeof(buffer)); //repopulate buffer with next
#ifdef SCANNER_DEBUG
			printf("No chars left, fetching line \"%s\"\n", buffer);
#endif
			line = buffer;

			lines++;
			next = 0; // reset pos indicator 
		}

		return line[next++];
	}

	int LineCount() const {
		return lines;
	}

private:
	std::ifstream &stream;
	std::string line;
	size_t next;
	int lines;
};
/**
 * The scanner is the most primative class in our small compiler, and
 * it's purpose is to fetch chars from the incomming token stream and
 * keep a buffer of them for backward steps. This buffer allows us to
 * infinitely step backwards and is fast.
 */
class Scanner
{
public:
	/**
	 * Initializes the scanner with the given input file name
	 */
	Scanner(const char *filename);

	/**
	 * Advances the scanner to the next position.
	 */
	char Next();

	/**
	 * Returns the current position of the scanner
     */
	int Position() const;

	/**
	 * Steps the scanner backwards. 
	 */
	void Back();

	/**
	 * Returns the length of the buffer. This value will change as Next() is called and
	 * more tokens are added to the buffer.
	 */
	size_t BufferSize() const;

	/**
	 * Returns the amount of newlines we've passed, this is useful for the parser to report
	 * to ErrorSys which line the error was on.
	 */
	int GetLineNumber() const;

	bool Peek(char in);
private:
	CharFetcher in;
	std::ifstream fin;
	std::vector<char> chars;
	int pos;
};

#endif // H_SCANNER