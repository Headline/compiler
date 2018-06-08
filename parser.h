#ifndef H_PARSER // double include protection
#define H_PARSER

#include "debug.h"
#include "tokenizer.h"
#include "error.h"
#include "language-constructs.h"

/**
 * It is the parser's job to make sense of the syntax, and ensure that
 * the grammar of the source programmign language is correct. The parser
 * will also be given a pointer to the errorsys, which handles any syntax
 * errors that may occur.
 */
class Parser
{
public:
	/**
	 * Initializes the parser with the given Tokenizer and ErrorSys
	 * such that the incomming token stream can be read from and
	 * any syntax errors can be reported.
	 */
	Parser(std::unique_ptr<Tokenizer> &tokenizer, ErrorSys *errorsys);

	/**
	 * Starts the parsing process which validates whether or not
	 * the source language is meaningful and reports syntax errors
	 */
	void Parse();

	/**
	 * Parses a function.
	 */
	void DoFunction();

	/**
	 * Parses statements, and stores their representation in the StatementList.
	 */
	void DoStatements(StatementList *list);

	/**
	 * Parses a statement, and stores the representation in the statement provided.
	 * Returns true if the statement is syntax valid and the parameter was populated.
	 */
	bool DoStatement(Statement &statement);

private:
	std::unique_ptr<Tokenizer> tokenizer;
	ErrorSys *errorsys;
};

#endif // H_PARSER