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
	tIDENT = -2
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
 * This allows us to move backwards in the parser indefinitely. Unlike the scanner,
 * we no longer buffer in advance, so every *new* step forward determines what the
 * parser stepped over, and stores it in a ParseState.
 */
class Tokenizer
{
public:
	/**
	 * Creates the parser, we give the parser our scanner.
	 * The parser is now the owner of the scanner (std::move)
	 */
	Tokenizer(std::unique_ptr<Scanner> &scanner);

	/**
	 * Advances the parser forward, returning the current parse state
	 */
	Token *Next();

	/**
	 * Retreats the parser backwards. The parser must have been advanced
	 * previously or else this call will fail.
	 */
	void Back();

private:
	std::unique_ptr<Scanner> scanner;
	std::vector<std::unique_ptr<Token>> states;
	int pos;
};

#endif // H_TOKENIZER