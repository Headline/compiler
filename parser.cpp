#include "parser.h"

Parser::Parser(std::unique_ptr<Tokenizer> &tokenizer, ErrorSys *errorsys)
	: tokenizer(std::move(tokenizer))
{
	this->errorsys = errorsys;
}

void Parser::Parse()
{
	Token *last = nullptr;
	Token *current;
	while ((current = tokenizer->Next()) != nullptr)
	{
#ifdef PARSER_DEBUG
		printf("// %s\n", current->ToString().c_str());
#endif
		if (last != nullptr)
		{
			if (current->tok == ';' && last->tok == ';')
				errorsys->Error(0, current->line);
		}

		last = current;
	}

	if (errorsys->Fatal())
		errorsys->Spew();
}