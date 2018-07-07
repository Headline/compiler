#include "parser.h"

Parser::Parser(std::unique_ptr<Tokenizer> &tokenizer, ErrorSys *errorsys)
	: tokenizer(std::move(tokenizer)), parse(std::make_unique<::Parse>())
{
	this->errorsys = errorsys;
}

void Parser::Parse()
{
	while (true) {

		Token *tok = tokenizer->Next();
		tokenizer->Back();

		if (tok == nullptr) {
			goto end;
		}

		switch (tok->tok) {
		case tINT:
			DoGlobal();
			break;
		case tFUNC:
			DoFunction();
			break;
		case tNATIVE:
			DoNative();
			break;
		default:
			goto end;
		}
	}

end:
	if (errorsys->Fatal()) {
		errorsys->Spew();
		exit(EXIT_FAILURE);
	}
}

inline void EatUntilNext(TOK tok, Tokenizer *tokenizer)
{
	Token *token;
	while ((token = tokenizer->Next())->tok != (TOK)tok) {
	} // keep eating tokens until we hit whatever
}

void Parser::DoGlobal()
{
	assert(tokenizer->Peek(tINT));

	Token *inttok = tokenizer->Next(); // eat int

	Token *ident;
	if ((ident = tokenizer->Match(tIDENT)) == nullptr)
	{
		errorsys->Error(1, inttok->line, "<identifier>"); // expected token <identifier>
		EatUntilNext((TOK)';', tokenizer.get()); // recover
		return;
	}

	Statement statement;
	statement.declaration = true;
	statement.lvalue = ident->identifier;

#ifdef PARSER_DEBUG
	printf("Falling into global declaration.\n");
#endif

	Token *semi;
	if ((semi = tokenizer->Match((TOK) ';')) == nullptr) {
		errorsys->Error(1, ident->line, ";"); // expected token ;
		return;
	}
	
	parse->globals.list.push_back(statement);
}

void Parser::DoNative()
{
	assert(tokenizer->Peek(tNATIVE));
	Native native;

	Token *func = tokenizer->Next(); // eat native

	Token *ident;
	if ((ident = tokenizer->Match(tIDENT)) == nullptr) {
		errorsys->Error(1, func->line, "<identifier>"); // expected token <identifier>
		EatUntilNext((TOK)';', tokenizer.get()); // recover
		return;
	}
	Token *tok;
	if ((tok = tokenizer->Match((TOK)'(')) == nullptr) {
		errorsys->Error(1, func->line, "(");
		EatUntilNext((TOK)')', tokenizer.get()); // recover
		return;
	}

	ArgumentList args;
	DoArguments(&args);

	if ((tok = tokenizer->Match((TOK)')')) == nullptr) {
		errorsys->Error(1, func->line, ")");
		return;
	}

	native.identifier = ident->identifier;
	native.arguments = args;

	this->parse->natives.push_back(native);
}

void Parser::DoArguments(ArgumentList *args)
{
#ifdef PARSER_DEBUG
	printf("Falling into argument parsing\n");
#endif

	Token *tok;
	if ((tok = tokenizer->Match((TOK)'{')) != nullptr) {
#ifdef PARSER_DEBUG
		printf("Bad token, erroring..\n");
#endif
		tokenizer->Back();
		errorsys->Error(2, tok->line, "{");
		return;
	}

	bool another = !tokenizer->Peek((TOK)')');
	while (another)
	{
#ifdef PARSER_DEBUG
		printf("Argument found\n");
#endif
		Argument arg;

		tok = tokenizer->Next(); // type
#ifdef PARSER_DEBUG
		printf("Grabbed type %c\n", tok->tok);
#endif


		if (!tokenizer->IsBuiltinType(tok->tok)) {
			errorsys->Error(3, tok->line, tok->identifier); // expected token <identifier>

			EatUntilNext((TOK)';', tokenizer.get()); // recover
			return;
		}

		Token *identifier = tokenizer->Next();
		if (identifier->tok != tIDENT) {
			errorsys->Error(1, identifier->line, "<identifier>"); // expected token <identifier>

			EatUntilNext((TOK)';', tokenizer.get()); // recover
			return;
		}

		arg.type = tok->tok;
		arg.identifier = identifier->identifier;
		args->arguments.push_back(arg);

		if (another = tokenizer->Peek((TOK)',')) {
			tokenizer->Next(); // eat ','
		}
	}

	TempToken token = tokenizer.get();
#ifdef PARSER_DEBUG
		printf("Argument end. Net token: %c/%i\n", token->tok, token->tok);
#endif

}

void Parser::DoFunction()
{
	assert(tokenizer->Peek(tFUNC));
	Token *func = tokenizer->Next(); // eat func

#ifdef PARSER_DEBUG
	printf("Falling into function parse\n");
#endif

	Token *ret;
	if ((ret = tokenizer->Match(tINT)) == nullptr) {
		errorsys->Error(1, func->line, "<return type>"); // 
		return;
	}

	Token *ident;
	if ((ident = tokenizer->Match(tIDENT)) == nullptr) {
		errorsys->Error(1, func->line, "<identifier>"); // expected token <identifier>
		return;
	}

	Token *tok;
	if ((tok = tokenizer->Match((TOK)'(')) == nullptr) {
		errorsys->Error(1, func->line, "(");
		return;
	}

	ArgumentList args;
	DoArguments(&args);

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

	StatementList list;
	DoStatements(&list);

	Function function;
	function.statements = list;
	function.identifier = ident->identifier;


	this->parse->functions.push_back(function);
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
		TempToken next = tokenizer;
		if (next.get() == nullptr) // eof
		{
			return false;
		}

		if (next->tok == (TOK)'=')
		{
			TempToken value = tokenizer;
			return (value->tok == tVAL && tokenizer->Peek((TOK)';'));
		}
		else if (next->tok == (TOK)'(')
		{
			TempToken close = tokenizer;
			if (close.get() == nullptr)
			{
				return false;
			}

			return (close->tok == (TOK)')' && tokenizer->Peek((TOK)';'));
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
	while (IsStatement(tokenizer.get())) {
		if (DoStatement(statement)) {
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
		Token *next = tokenizer->Next();
		if (next->tok == (TOK)'=') {
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
		else if (next->tok == (TOK)'(') { // function call
			Token *close = tokenizer->Next();
			if (close->tok == (TOK)')') {
				Token *semicolon = tokenizer->Next(); // eat ;
				if (semicolon->tok == (TOK)';') {
					statement.funccall = true;
					statement.identifier = first->identifier;
				}
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