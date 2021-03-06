#include "parser.h"

#include <gsl/gsl>

Parser::Parser(Tokenizer &tokenizer, ErrorSys &errorsys) noexcept
	: tokenizer(tokenizer), errorsys(errorsys), parse(std::make_unique<::Parse>())
{ }

void Parser::Parse() noexcept
{
	while (true) {

		Token *tok = tokenizer.Next();
		tokenizer.Back();

		if (tok == nullptr) {
			errorsys.Exit(this);
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
			errorsys.Exit(this);
		}
	}
}

inline void EatUntilNext(TOK tok, Tokenizer &tokenizer) noexcept
{
	Token *token;
	while ((token = tokenizer.Next())->tok != (TOK)tok) {
	} // keep eating tokens until we hit whatever
}

inline void EatUntilFunctionEnd(Tokenizer &tokenizer) noexcept
{
	int scope = 1; // we already missed one, so compensate for that
	
	Token *token;
	while ((token = tokenizer.Next()) != nullptr) {
		if (scope == 0)
		{
			return;
		}
		if (token->tok == (TOK)'{')
			scope++;
		if (token->tok == (TOK)'}')
			scope--;
	}
}

void Parser::DoGlobal() noexcept
{
	Expects(tokenizer.Peek(tINT));

	Token *inttok = tokenizer.Next(); // eat int

	Token *ident;
	if ((ident = tokenizer.Match(tIDENT)) == nullptr)
	{
		errorsys.Error(1, inttok->line, "<identifier>"); // expected token <identifier>
		EatUntilNext((TOK)';', tokenizer); // recover
		return;
	}

#ifdef PARSER_DEBUG
	printf("Falling into global declaration.\n");
#endif

	Token *semi;
	if ((semi = tokenizer.Match((TOK) ';')) == nullptr) {
		errorsys.Error(1, ident->line, ";"); // expected token ;
		return;
	}
	
	parse->globals->add(std::make_unique<DeclarationStmt>(ident->line, ident->identifier));
}

void Parser::DoNative() noexcept
{
	Expects(tokenizer.Peek(tNATIVE));

	Native native;
	Token *nat = tokenizer.Next(); // eat native

	Token *type;
	if ((type = tokenizer.Match(tINT)) == nullptr) {
		errorsys.Error(1, nat->line, "<type>");
		EatUntilNext((TOK)';', tokenizer); // recover
		return;
	}
	native.type = type->tok;

	Token *ident;
	if ((ident = tokenizer.Match(tIDENT)) == nullptr) {
		errorsys.Error(1, nat->line, "<identifier>"); // expected token <identifier>
		EatUntilNext((TOK)';', tokenizer); // recover
		return;
	}
	Token *tok;
	if ((tok = tokenizer.Match((TOK)'(')) == nullptr) {
		errorsys.Error(1, nat->line, "(");
		EatUntilNext((TOK)')', tokenizer); // recover
		return;
	}

	ArgumentList args;
	DoArguments(&args);

	if ((tok = tokenizer.Match((TOK)')')) == nullptr) {
		errorsys.Error(1, nat->line, ")");
		return;
	}

	native.identifier = ident->identifier;
	native.arguments = args;

	this->parse->natives.push_back(native);
}

void Parser::DoArguments(ArgumentList *args) noexcept
{
#ifdef PARSER_DEBUG
	printf("Falling into argument parsing\n");
#endif

	Token *tok;
	if ((tok = tokenizer.Match((TOK)'{')) != nullptr) {
#ifdef PARSER_DEBUG
		printf("Bad token, erroring..\n");
#endif
		tokenizer.Back();
		errorsys.Error(2, tok->line, "{");
		return;
	}

	bool another = !tokenizer.Peek((TOK)')');
	while (another)
	{
#ifdef PARSER_DEBUG
		printf("Argument found\n");
#endif
		Argument arg;

		tok = tokenizer.Next(); // type
#ifdef PARSER_DEBUG
		printf("Grabbed type %c\n", tok->tok);
#endif


		if (!tokenizer.IsBuiltinType(tok->tok)) {
			errorsys.Error(3, tok->line, tok->identifier.c_str()); // expected token <identifier>

			EatUntilNext((TOK)';', tokenizer); // recover
			return;
		}

		Token *identifier = tokenizer.Next();
		if (identifier->tok != tIDENT) {
			errorsys.Error(1, identifier->line, "<identifier>"); // expected token <identifier>

			EatUntilNext((TOK)';', tokenizer); // recover
			return;
		}

		arg.type = tok->tok;
		arg.identifier = identifier->identifier;
		args->arguments.push_back(arg);

		another = tokenizer.Peek((TOK)',');
		if (another) {
			tokenizer.Next(); // eat ','
		}
	}

	TempToken token{tokenizer};
#ifdef PARSER_DEBUG
		printf("Argument end. Net token: %c/%i\n", token->tok, token->tok);
#endif
}

void Parser::DoFunction() noexcept
{
	Expects(tokenizer.Peek(tFUNC));
	Token *func = tokenizer.Next(); // eat func

#ifdef PARSER_DEBUG
	printf("Falling into function parse\n");
#endif

	Token *ret;
	if ((ret = tokenizer.Match(tINT)) == nullptr) {
		errorsys.Error(1, func->line, "<return type>"); // 
		return;
	}

	Token *ident;
	if ((ident = tokenizer.Match(tIDENT)) == nullptr) {
		errorsys.Error(1, func->line, "<identifier>"); // expected token <identifier>
		return;
	}

	Token *tok;
	if ((tok = tokenizer.Match((TOK)'(')) == nullptr) {
		errorsys.Error(1, func->line, "(");
		return;
	}

	ArgumentList args;
	DoArguments(&args);

	if ((tok = tokenizer.Match((TOK)')')) == nullptr) {
		errorsys.Error(1, func->line, ")");
		return;
	}

	if ((tok = tokenizer.Match((TOK)'{')) == nullptr) { // look for '{'
		// we didn't find it, so in order to produce a sensible
		// error, we'll match the next token and point our error there.
		{
			TempToken next{tokenizer};
			errorsys.Error(1, next->line, "{"); // expected token '{'
		}
		EatUntilFunctionEnd(tokenizer);
		return;
	}

	// we matched '{' above, but a statement can include the brackets so
	// we'll give the '{' back to the tokenizer and fall into ParseStatement
	tokenizer.Back();

	std::unique_ptr<StatementList> statements = DoStatements();
	if (!statements)
	{
		errorsys.Exit(this);
	}

	std::unique_ptr<Function> function = std::make_unique<Function>();
	function->statements = std::move(statements);
	function->identifier = ident->identifier;
	function->line = ident->line;
	this->parse->functions.push_back(std::move(function));
}

inline bool IsStatement(Tokenizer &tokenizer) noexcept
{
	TempToken tok{tokenizer};
	if (tok.get() == nullptr) //eof
	{
		return false;
	}
	else if (tok->tok == (TOK)';') // this is an empty statement to be errored later
	{
		return true;
	}
	else if (tok->tok == (TOK)'}') // end of scope
	{
		return false;
	}
	else if (tok->tok == tINT)
	{
		TempToken ident{tokenizer};
		if (ident.get() == nullptr) // eof
		{
			return false;
		}

		return (ident->tok == tIDENT && tokenizer.Peek((TOK)';'));
	}
	else if (tok->tok == tIDENT)
	{
		TempToken next{tokenizer};
		if (next.get() == nullptr) // eof
		{
			return false;
		}

		if (next->tok == (TOK)'=')
		{
			TempToken value{tokenizer};
			return (value->tok == tVAL && tokenizer.Peek((TOK)';'));
		}
		else if (next->tok == (TOK)'(')
		{
			TempToken close{tokenizer};
			if (close.get() == nullptr)
			{
				return false;
			}

			return (close->tok == (TOK)')' && tokenizer.Peek((TOK)';'));
		}
	}
	return false;
}

std::unique_ptr<StatementList> Parser::DoStatements() noexcept
{
	Token *tok;
	if ((tok = tokenizer.Match((TOK)'{')) == nullptr) {
		// we didn't find it, so in order to produce a sensible
		// error, we'll match the next token and point our error there.
		TempToken next{tokenizer};
		errorsys.Error(1, next->line, "{"); // expected token '{'
		return nullptr;
	}

	std::unique_ptr<StatementList> list = std::make_unique<StatementList>();
	while (IsStatement(tokenizer)) {
		std::unique_ptr<Statement> stmt = DoStatement();
		if (stmt) {
#ifdef PARSER_DEBUG
			printf("Statement found");
#endif
			list->add(std::move(stmt));
		}
	}

#ifdef PARSER_DEBUG
	printf("Statements parsed... %d found\n", (int)list->list.size());
#endif

	if ((tok = tokenizer.Match((TOK)'}')) == nullptr) {
		errorsys.Error(1, tokenizer.GetScanner().GetLineNumber(), "}"); // expected token '}'
		return nullptr;
	}
	
	return list;
}

std::unique_ptr<Statement> Parser::DoStatement() noexcept
{
	if (tokenizer.Peek((TOK)';')) // empty statement
	{
		errorsys.Error(0, tokenizer.Next()->line); // we'll step forward, ';', and ignore it.
		return nullptr; // should be effective error recovery
	}

	std::unique_ptr<Statement> statement = nullptr;
	Token *first = tokenizer.Next();
	int line = first->line;
	if (first->tok == tIDENT)
	{
		Token *next = tokenizer.Next();
		if (next->tok == (TOK)'=') {
			Token *value = tokenizer.Next();
			if (value->tok == tVAL) {

				EvalVar var(value->identifier);
				statement = std::make_unique<AssignmentStmt>(line, first->identifier, std::make_unique<Node<Evaluable>>((EvalVar)var));

				Token *semicolon = tokenizer.Next();
				if (semicolon->tok != (TOK)';') {

					EatUntilNext((TOK)';', tokenizer); // recover
					errorsys.Error(1, semicolon->line, ";");
					return nullptr;
				}
				return statement;
			}
			else {
				EatUntilNext((TOK)';', tokenizer); // recover
				errorsys.Error(1, value->line, "<value>"); // we'll step forward, grab the line, and ignore it.
			}
		}
		else if (next->tok == (TOK)'(') { // function call
			// TODO params
			Token *close = tokenizer.Next();
			if (close->tok == (TOK)')') {
				Token *semicolon = tokenizer.Next(); // eat ;

				if (semicolon->tok == (TOK)';') {
#ifdef PARSER_DEBUG
					printf("Function call \"%s()\" found...\n", first->identifier.c_str());
#endif
					counter.insert(first->identifier);
					statement = std::make_unique<FuncCallStmt>(line, first->identifier);
					return statement;
				}
				return nullptr;
			}
		}
	}
	else if (first->tok == tINT)
	{
		Token *identifier = tokenizer.Next();
		if (identifier->tok != tIDENT)
		{
			errorsys.Error(1, identifier->line, "<identifier>"); // expected token <identifier>
			
			EatUntilNext((TOK)';', tokenizer); // recover
			return nullptr;
		}
		Token *semicolon = tokenizer.Next();
		if (semicolon->tok != (TOK)';') {

			EatUntilNext((TOK)';', tokenizer); // recover
			errorsys.Error(1, semicolon->line, ";");
			return nullptr;
		}

		statement = std::make_unique<DeclarationStmt>(line, identifier->identifier);
		return statement;
	}

	return nullptr;
}

inline bool IsValidFunction(std::string const &identifier, std::vector<std::unique_ptr<Function>> const &funcs) noexcept
{
	for (auto&& func : funcs)
	{
		if (func->identifier == identifier)
		{
			return true;
		}
	}
	return false;
}

inline bool IsValidNative(std::string const &identifier, std::vector<Native> const &natives) noexcept
{
	for (auto native : natives)
	{
		if (native.identifier == identifier)
		{
			return true;
		}
	}
	return false;
}

void Parser::Validate() const noexcept
{
	for (auto&& func : this->parse->functions)
	{
		// ensure all function calls are to things that are defined, or are forward decl'd
		// as a native.
		// TODO: Check parameters match as well
		for (auto && statement : func->statements->list)
		{
			Expects(statement);
			if (statement->Type() == Statement::StatementType::FunctionCall)
			{
				gsl::not_null<FuncCallStmt *> stmt = dynamic_cast<FuncCallStmt *>(statement.get());
				if (!IsValidFunction(stmt->identifier, this->parse->functions)
					&& !IsValidNative(stmt->identifier, this->parse->natives))
				{
					errorsys.Error(4, stmt->line, stmt->identifier.c_str());
				}
			}
		}

		// if the function was never used (it was never inserted into our set)
		// also, main is special :)
		if (func->identifier != "main" && counter.find(func->identifier) == counter.end())
		{
			errorsys.Warning(0, func->line, func->identifier.c_str());
		}
	}
}