#include "parser.h"

Parser::Parser(std::unique_ptr<Tokenizer> &tokenizer, ErrorSys *errorsys)
	: tokenizer(std::move(tokenizer))
{
	this->errorsys = errorsys;
}

void Parser::Parse()
{
	if (tokenizer->Peek(tFUNC)) {
		this->DoFunction();
	}

	if (errorsys->Fatal())
		errorsys->Spew();
}

void Parser::DoFunction()
{
	assert(tokenizer->Peek(tFUNC));
	Token *func = tokenizer->Next(); // eat func

#ifdef PARSER_DEBUG
	printf("Falling into function parse\n");
#endif

	Token *ident;
	if ((ident = tokenizer->Match(tIDENT)) == nullptr) {
		errorsys->Error(1, func->line); // expected token <identifier>
	}

	// TODO: args
	Token *paren;
	if ((paren = tokenizer->Match((TOK)'(')) == nullptr) {
		errorsys->Error(2, func->line); // expected token '('
	}
	if ((paren = tokenizer->Match((TOK)')')) == nullptr) {
		errorsys->Error(3, func->line); // expected token ')'
	}

	// TODO: parse statements and start creating structures to store stuff
	// into in order to build parse trees and the fun stuff.
}
