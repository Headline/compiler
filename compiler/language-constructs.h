#ifndef H_LANGUAGE_CONSTRUCTS // double include protection
#define H_LANGUAGE_CONSTRUCTS

#include <vector>

class Statement
{
public:
	bool assignment;
	std::string lvalue;
	std::string rvalue;
	bool declaration;
	std::string var;
	bool funccall;
	std::string identifier;
	int line;
};

class StatementList
{
public:
	std::vector<Statement> list;
};

class Argument
{
public:
	TOK type;
	std::string identifier;
};

class ArgumentList
{
public:
	std::vector<Argument> arguments;
};

class Function
{
public:
	std::string identifier;
	StatementList statements;
	ArgumentList arguments;
};

class Native
{
public:
	TOK type;
	std::string identifier;
	ArgumentList arguments;
};
#endif // H_LANGUAGE_CONSTRUCTS