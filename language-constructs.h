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
};

class StatementList
{
public:
	std::vector<Statement> list;
};
#endif // H_LANGUAGE_CONSTRUCTS