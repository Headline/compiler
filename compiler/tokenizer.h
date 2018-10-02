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
	tFUNC = -3,
	tVAL = -4,
	tNATIVE = -5,
};

/**
 * Represents a token which is a keyword, literal, or other symbol.
 */
struct Token
{
	TOK tok;
	std::string identifier;
	int line; 

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
	Tokenizer(Scanner &scanner);

	/**
	 * Advances the tokenizer forward, returning the current parse state.
	 * If you mean to step forward temporarily, consider using TempToken.
	 */
	Token *Next();

	/**
	 * Retreats the tokenizer backwards. The tokenizer must have been advanced
	 * previously or else this call will fail.
	 */
	void Back();

	/**
	 * Returns whether or not the next token is of the same
	 * type as the input parameter.
	 */
	bool Peek(TOK tok);

	/**
	* Returns whether or not the next token is of the same
	* type as the input parameter, advances the tokenizer if it does
	* and also gives back the Token pointer, it returns a nullptr
	* if match was unsuccessful.
	*/
	Token *Match(TOK tok);

	/**
	 * Returns the scanner pointer, which is helpful for grabbing
	 * the current line number for ErrorSys.
	 */
	Scanner const &GetScanner();

	/**
	 * Returns whether or not the supplied token is a built-in type
	 */
	bool IsBuiltinType(TOK tok);

private:
	Scanner &scanner;
	std::vector<std::unique_ptr<Token>> states;
	size_t pos;
};


/**
 * A very useful class which allows us to give a tokenizer
 * and this class will advance the tokenizer forward, allow
 * us to interact with the token directly, but will automatically
 * step the tokenizer backwards when this token goes out of scope.
 * It is essentially an RAII container for a Token, but instead of
 * freeing the memory when going out of scope, we just step the 
 * tokenizer backwards.
 *
 * see IsStatement in parser.cpp
 */
class TempToken {
public:
	TempToken(Tokenizer &tokenizer)
		: tokenizer(tokenizer), t(tokenizer.Next()) {
	}
	~TempToken() {
		tokenizer.Back();
	}
	Token *operator ->() const {
		return t;
	}
	Token *get() const {
		return t;
	}
private:
	Tokenizer &tokenizer;
	Token *t;
};

#endif // H_TOKENIZER