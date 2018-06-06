#ifndef H_TOKENIZER // double include protection
#define H_TOKENIZER

#include <memory>

#include "debug.h"
#include "scanner.h"

/**
  * The wrapper around a character, but with keywords being identified as neg values.
  * TOK tok = ';' is intended to be valid
  */
enum TOK
{
	tINT = -1,
	tIDENT = -2,
	tFUNC = -3
};

/**
 * Represents a token which is a keyword, literal, or other symbol.
 */
struct Token
{
	TOK tok;
	std::string identifier;

	/**
	 * Returns a string reperesentation of the thing, useful for
	 * debug stuff
	 */
	std::string ToString()
	{
		if (tok > 0)
			return std::string(1, (char)tok);
		else
			return identifier;
	}
};


/** 
 * Similar to how the scanner works, we cache everything we step past in a buffer. 
 * This allows us to move backwards in the tokenizer indefinitely. Unlike the scanner,
 * we no longer buffer in advance, so every *new* step forward determines what the
 * tokenizer stepped over, and stores it in a Token type.
 */
class Tokenizer
{
public:
	/**
	 * Creates the tokenizer, we give the tokenizer our scanner.
	 * The tokenizer is now the owner of the scanner (std::move)
	 */
	Tokenizer(std::unique_ptr<Scanner> &scanner);

	/**
	 * Advances the tokenizer forward, returning the current parse state
	 */
	Token *Next();

	/**
	 * Retreats the tokenizer backwards. The tokenizer must have been advanced
	 * previously or else this call will fail.
	 */
	void Back();

	/**
	 * Returns the scanner pointer, which is helpful for grabbing
	 * the current line number for ErrorSys.
	 */
	const Scanner *GetScanner();

private:
	std::unique_ptr<Scanner> scanner;
	std::vector<std::unique_ptr<Token>> states;
	int pos;
};

#endif // H_TOKENIZER