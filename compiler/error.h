#ifndef H_ERROR // double include protection
#define H_ERROR

#include <cstdarg>
#include "debug.h"
#include "tokenizer.h"

/**
 * Static list of errors used by ErrorSys to report errors. The index in this
 * array corresponds with the desired error to be displayed by ErrorSys::Error
 */
static const char * const errors[] = {
	/* 00 */	"empty statement",
	/* 01 */	"expected token '%s'",
	/* 02 */	"unexpected token '%s'",
	/* 03 */	"expected built-in type, but got '%s'",
	/* 04 */	"function definition missing for '%s'"
};

/**
 * It is the parser's job to make sense of the syntax, and ensure that
 * the grammar of the source programmign language is correct. The parser
 * will also be given a pointer to the errorsys, which handles any syntax
 * errors that may occur.
 *
 * We store the errors and wait before they're outputted in an effort to 
 * recover from them. Spew() will output all errors to stdout.
 */
class ErrorSys
{
public:
	/**
	 * Creates the error system, which essentially counts errors, warnings, and
	 * outputs when parsing is completed.
	 *
	 * All phrases are build into the phrases vector when ErrorSys is constructed.
	 */
	ErrorSys();

	/**
	 * Queues up error of the given index. Each error number directly corresponds with
	 * an index in the phrases vector.
	 */
	void Error(int error, int lines, ...);

	/**
	 * Spits out all errors queued to the stdout
	 */
	void Spew() const;

	/**
	 * Returns if we've hit a fatal error 
	 */
	bool Fatal() const;

private:
	std::vector<std::string> output;
	int fatals;
	int warnings;
};

#endif // H_ERROR