#ifndef H_SCANNER // double include protection
#define H_SCANNER

#include "debug.h"

#include <stdio.h>
#include <vector>
#include <cassert>
#include <fstream>

/**
 * The scanner is the most primative class in our small compiler, and
 * it's purpose is to fetch chars from the incomming token stream and
 * keep a buffer of them for forward steps. This buffer allows us to
 * infinitely step backwards and is quite performant.
 */
class Scanner
{
public:
	/**
	 * We're keeping a vector of chars, and the bufferlen is how many chars we're caching
	 * ahead of the current position. If we hit the end of the vector (last element), then
	 * we'll fetch bufferlen more of them.
	 *
	 * bufferlen must be > 0
	 */
	Scanner(const char *filename, int bufferlen);

	/**
	 * Advances the scanner to the next position, returning the new char and
	 * buffering more characters if necessary.
	 */
	char Next();

	/**
	 * Returns the current position of the scanner
     */
	int Position() const;

	/**
	 * Steps the scanner backwards
     */
	void Back();

	/**
	 * Fills the scanner with bufferlen more characters
	 */
	void GrabChars();

	/**
	 * Returns the length of the buffer. This value can/will change as Next() is called and
	 * more tokens are fetched.
	 */
	size_t BufferSize() const;

private:
	std::ifstream fin;
	std::vector<char> chars;
	int pos;
	const int bufferlen;
};

#endif // H_SCANNER