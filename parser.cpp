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
		errorsys->Error(1, func->line, "<identifier>"); // expected token <identifier>
		return;
	}

	// TODO: args
	Token *tok;
	if ((tok = tokenizer->Match((TOK)'(')) == nullptr) {
		errorsys->Error(1, func->line, "(");
		return;
	}
	if ((tok = tokenizer->Match((TOK)')')) == nullptr) {
		errorsys->Error(1, func->line, ")");
		return;
	}

	if ((tok = tokenizer->Match((TOK)'{')) == nullptr) { // look for '{'
		// we didn't find it, so in order to produce a sensible
		// error, we'll match the next token and point our error there.
		tok = tokenizer->Next();
		tokenizer->Back(); // back off the token to report the error
		errorsys->Error(1, tok->line, "{"); // expected token '{'
		return;
	}

	// we matched '{' above, but a statement can include the brackets so
	// we'll give the '{' back to the tokenizer and fall into ParseStatement
	tokenizer->Back();

	std::unique_ptr<StatementList> list = std::make_unique<StatementList>();
	DoStatements(list.get());
}

inline bool IsStatement(Tokenizer *tokenizer)
{
	TempToken tok = tokenizer;
	if (tok.get() == nullptr) //eof
	{
		return false;
	}
	else if (tok->tok == (TOK)';') // this is an empty statement to be errored later
	{
		return true;
	}
	else if (tok->tok == tINT)
	{
		TempToken ident = tokenizer;
		if (ident.get() == nullptr) // eof
		{
			return false;
		}

		return (ident->tok == tIDENT && tokenizer->Peek((TOK)';'));
	}
	else if (tok->tok == tIDENT)
	{
		TempToken equals = tokenizer;
		if (equals.get() == nullptr) // eof
		{
			return false;
		}

		if (equals->tok == (TOK)'=')
		{
			TempToken value = tokenizer;
			return (value->tok == tVAL && tokenizer->Peek((TOK)';'));
		}
	}
	return false;
}

void Parser::DoStatements(StatementList *list)
{
	Token *tok;
	if ((tok = tokenizer->Match((TOK)'{')) == nullptr) {
		// we didn't find it, so in order to produce a sensible
		// error, we'll match the next token and point our error there.
		tok = tokenizer->Next();
		tokenizer->Back(); // back off the token to report the error
		errorsys->Error(1, tok->line, "{"); // expected token '{'
		return;
	}

	Statement statement;
	bool valid = false;
	while (IsStatement(tokenizer.get())) {

		valid = DoStatement(statement);
		if (valid) {
#ifdef PARSER_DEBUG
			printf("Statement found, assign: %d\n", statement.assignment);
#endif
			list->list.push_back(statement);
		}
	}

#ifdef PARSER_DEBUG
	printf("Statements parsed... %d found\n", list->list.size());
#endif
	if ((tok = tokenizer->Match((TOK)'}')) == nullptr) {
		errorsys->Error(1, tokenizer->GetScanner()->GetLineNumber(), "}"); // expected token '}'
		return;
	}
}

inline void EatUntilNext(TOK tok, Tokenizer *tokenizer)
{
	Token *token;
	while ((token = tokenizer->Next())->tok != (TOK)tok) {
	} // keep eating tokens until we hit whatever
}

bool Parser::DoStatement(Statement &statement)
{
	if (tokenizer->Peek((TOK)';')) // empty statement
	{
		errorsys->Error(0, tokenizer->Next()->line); // we'll step forward, ';', and ignore it.
		return false; // should be effective error recovery
	}

	statement = Statement(); // we'll wipe whatever else was there

	Token *first = tokenizer->Next();
	if (first->tok == tIDENT)
	{
		Token *equals = tokenizer->Next();
		if (equals->tok == (TOK)'=') {
			statement.lvalue = first->identifier;
			statement.assignment = true;
			Token *value = tokenizer->Next();
			if (value->tok == tVAL) {
				statement.rvalue = value->identifier;
				Token *semicolon = tokenizer->Next();
				if (semicolon->tok != (TOK)';') {

					EatUntilNext((TOK)';', tokenizer.get()); // recover
					errorsys->Error(1, semicolon->line, ";");
					return false;
				}
			}
			else {
				EatUntilNext((TOK)';', tokenizer.get()); // recover
				errorsys->Error(1, value->line, "<value>"); // we'll step forward, grab the line, and ignore it.
			}
		}
	}
	else if (first->tok == tINT)
	{
		Token *identifier = tokenizer->Next();
		if (identifier->tok != tIDENT)
		{
			errorsys->Error(1, identifier->line, "<identifier>"); // expected token <identifier>
			
			EatUntilNext((TOK)';', tokenizer.get()); // recover
			return false;
		}
		Token *semicolon = tokenizer->Next();
		if (semicolon->tok != (TOK)';') {

			EatUntilNext((TOK)';', tokenizer.get()); // recover
			errorsys->Error(1, semicolon->line, ";");
			return false;
		}

		statement.declaration = true;
		statement.var = identifier->identifier;
	}
	else
	{
		assert(true);
	}
	return true;
}