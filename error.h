#ifndef H_ERROR // double include protection
#define H_ERROR

#include "debug.h"
#include "tokenizer.h"

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
	void Error(int error, int lines);

	/**
	 * Spits out all errors queued to the stdout
	 */
	void Spew();

private:
	int fatals;
	int warnings;
	std::vector<std::string> phrases;
	std::vector<std::string> output;
};

#endif // H_ERROR