#ifndef H_PARSER // double include protection
#define H_PARSER

#include "debug.h"
#include "tokenizer.h"
#include "error.h"
/**
 * It is the parser's job to make sense of the syntax, and ensure that
 * the grammar of the source programmign language is correct. The parser
 * will also be given a pointer to the errorsys, which handles any syntax
 * errors that may occur.
 */
class Parser
{
public:
	Parser(std::unique_ptr<Tokenizer> &tokenizer, ErrorSys *errorsys);

	void Validate();
private:
	ErrorSys *errorsys;
	std::unique_ptr<Tokenizer> tokenizer;
};

#endif // H_PARSER