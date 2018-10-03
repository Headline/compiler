#ifndef H_PARSER // double include protection
#define H_PARSER

#include "debug.h"
#include "error.h"
#include "tokenizer.h"
#include "language-constructs.h"

#include <memory>
#include <unordered_set>

using FuncSet = std::unordered_set<std::string>;

/**
 * The parse class is the result of a complete parse built by the parser.
 * It contains all native declarations, function declarations, etc that
 * the parser has detected.
 */
class Parse
{
public:
	Parse() noexcept : globals(std::make_unique<StatementList>()) {
	}
	std::unique_ptr<StatementList> globals;
	std::vector<std::unique_ptr<Function>> functions;
	std::vector<Native> natives;
};

class ErrorSys; // fwd decl

/**
 * It is the parser's job to make sense of the syntax, and ensure that
 * the grammar of the source programming language is correct. The parser
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
	Parser(Tokenizer &tokenizer, ErrorSys &errorsys) noexcept;

	/**
	 * Starts the parsing process which validates whether or not
	 * the source language is meaningful and reports syntax errors
	 */
	void Parse() noexcept;

	/**
	 * Parses a function.
	 */
	void DoFunction() noexcept;

	/**
	 * Parses statements, and stores their representation in the StatementList.
	 */
	std::unique_ptr<StatementList> DoStatements() noexcept;

	/**
	 * Parses a statement, and stores the representation in the statement provided.
	 * Returns true if the statement is syntax valid and the parameter was populated.
	 */
	std::unique_ptr<Statement> DoStatement() noexcept;

	/**
	 * Parses a native declaration.
	 */
	void DoNative() noexcept;

	/**
	 * Parses the arguments for a function or native declaration.
	 */
	void DoArguments(ArgumentList *args) noexcept;

	/**
	 * Parses a global variable 
	 */
	void DoGlobal() noexcept;

	/**
	 * Further parsing (semantic) analysis to ensure variables are used, functions
	 * are defined, etc.
	 */
	void Validate() const noexcept;

private:
	Tokenizer &tokenizer;
	ErrorSys &errorsys;
	std::unique_ptr<::Parse> parse;
	FuncSet counter;
};

#endif // H_PARSER