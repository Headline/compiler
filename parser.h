#ifndef H_PARSER // double include protection
#define H_PARSER

#include <memory>

#include "debug.h"
#include "scanner.h"

/**
 * Represents a token, keyword, literal, or other symbol.
 * This will probably not survive past the devopment stage, but
 * it can be considered a snapshot of the current parser location.
 */
struct ParseState
{
	bool expr;
	bool symbol;
	bool lit;
	bool keyword;
	std::string tok;
};

/** 
 * Similar to how the scanner works, we cache everything we step past in a buffer. 
 * This allows us to move backwards in the parser indefinitely. Unlike the scanner,
 * we no longer buffer in advance, so every *new* step forward determines what the
 * parser stepped over, and stores it in a ParseState.
 */
class Parser
{
public:
	/**
	 * Creates the parser, we give the parser our scanner.
	 * The parser is now the owner of the scanner (std::move)
	 */
	Parser(std::unique_ptr<Scanner> &scanner);

	/**
	 * Advances the parser forward, returning the current parse state
	 */
	ParseState *Next();

	/**
	 * Retreats the parser backwards. The parser must have been advanced
	 * previously or else this call will fail.
	 */
	void Back();

private:
	std::unique_ptr<Scanner> scanner;
	std::vector<std::unique_ptr<ParseState>> states;
	int pos;
};

#endif // H_PARSER