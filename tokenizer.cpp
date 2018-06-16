#include "tokenizer.h"

Tokenizer::Tokenizer(std::unique_ptr<Scanner> &scanner)
	: scanner(std::move(scanner))
{
	pos = 0;
}

inline bool isspecialtoken(char tok)
{
	static const char constarr[] = { ';', '(', ')', '{', '}', '=', ',' };
	for (int i = 0; i < sizeof(constarr); ++i) {
		if (tok == constarr[i])
			return true;
	}

	return false;
}

Token *Tokenizer::Next()
{
	if (pos != states.size())
	{
		return states[pos++].get();
	}

	auto tok = std::make_unique<Token>();
	
	char c;
	while (isspace((c = scanner->Next()))) {
#ifdef TOKENIZER_DEBUG
		printf("Parser skipping a space...\n");
#endif
	} // eat spaces  

	if (c == EOF)
	{
#ifdef TOKENIZER_DEBUG
		printf("Hit EOF after spaces skipped.. returning nullptr\n");
#endif
		return nullptr;
	}

	std::string identifier;
	bool special = false;
	do
	{
		if (c == EOF)
			break; // leave this token spliced
		
		/* If we hit a special token off the bat, we need to
		 * just jump directly to the finalization
		 */
		if (isspecialtoken(c)) {
			tok->tok = (TOK)c;
			goto finalize_tok;
		}

		identifier.append(1, c);

	} while (!isspace((c = scanner->Next())) && !isspecialtoken(c)); // keep eating until we get another space or special token
	
	if (isspecialtoken(c)) // we hit a special on accident, lets remove it from the thing
	{
		scanner->Back(); // step scanner back
	}

finalize_tok:

	if (tok->tok == 0)
	{
		tok->tok = tIDENT;
	}
	
	if (identifier == "int")
	{
		tok->tok = tINT;
	}
	else if (identifier == "func")
	{
		tok->tok = tFUNC;
	}
	else if (identifier == "native")
	{
		tok->tok = tNATIVE;
	}
	else if (isdigit(identifier[0]))
	{
		tok->tok = tVAL;
	}

	tok->line = scanner->GetLineNumber();
	tok->identifier = identifier;


#ifdef TOKENIZER_DEBUG
	printf("Token \"%s\" built (line %d), pushing to states\n", tok->ToString().c_str(), tok->line);
#endif

	Token *ptr = tok.get();
	states.push_back(std::move(tok));

	pos++;
	return ptr;
}

void Tokenizer::Back()
{
	pos--;
}

bool Tokenizer::Peek(TOK tok)
{
	TempToken token = this;
	return token->tok == tok;
}

const Scanner *Tokenizer::GetScanner()
{
	return scanner.get();
}

Token *Tokenizer::Match(TOK token)
{
	if (this->Peek(token)) {
		return this->Next();
	}
	return nullptr;

}

bool Tokenizer::IsBuiltinType(TOK tok)
{
	if (tok == tINT)
		return true;

	return false;
}