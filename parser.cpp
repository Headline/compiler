#include "parser.h"

Parser::Parser(std::unique_ptr<Scanner> &scanner)
	: scanner(std::move(scanner))
{
	pos = 0;
}

ParseState *Parser::Next()
{
	if (pos != states.size())
	{
		return states[pos].get();
	}

	auto state = std::make_unique<ParseState>();
	
	char c;
	while (isspace((c = scanner->Next()))) {
#ifdef PARSER_DEBUG
		printf("Parser skipping a space...\n");
#endif
	} // eat spaces  

	if (c == EOF)
	{
#ifdef PARSER_DEBUG
		printf("Hit EOF after spaces skipped.. returning nullptr\n");
#endif
		return nullptr;
	}

	std::string tok;
	do
	{
		if (c == EOF)
			break; // leave this token spliced
		tok.append(1, c);
	} while (!isspace((c = scanner->Next()))); // keep eating until we get another space

#ifdef PARSER_DEBUG
	printf("Token \"%s\" built, pushing to states\n", tok.c_str());
#endif

	state->tok = tok;

	ParseState *ptr = state.get();
	states.push_back(std::move(state));

	pos++;
	return ptr;
}

void Parser::Back()
{
	pos--;
}