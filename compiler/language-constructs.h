#ifndef H_LANGUAGE_CONSTRUCTS // double include protection
#define H_LANGUAGE_CONSTRUCTS

#include <vector>
#include <memory>


template <typename T> class Node;

class Evaluable {

};

class EvalDigit : public Evaluable
{
public:
	int value;
};

class EvalVar : public Evaluable
{
public:
	EvalVar(std::string var) : var(var){};
	std::string var;
};

class Statement
{
public:
	enum StatementType {
		None,
		Assignment,
		Declaration,
		FunctionCall
	};
	virtual ~Statement(){};
public:
	virtual StatementType Type() {
		return None;
	} 
};

class AssignmentStmt : public Statement
{
public:
	AssignmentStmt(int line, std::string lvalue, Node<Evaluable> *rvalue)
					: line(line), lvalue(lvalue), rvalue(rvalue) {};

	StatementType Type() {
		return Assignment;
	}

	int line;
	std::string lvalue;
	std::unique_ptr<Node<Evaluable>> rvalue;
};

class DeclarationStmt : public Statement
{
public:
	DeclarationStmt(int line, std::string var) : line(line), var(var) {};
	StatementType Type() {
		return Declaration;
	}
	int line;
	std::string var;
};

class FuncCallStmt : public Statement
{
public:
	FuncCallStmt(int line, std::string identifier) : line(line), identifier(identifier) {};
	StatementType Type() {
		return FunctionCall;
	}
	int line;
	std::string identifier;
};

template <typename T>
class Node
{
public:
	Node(T t) : _t(t), _lhs(nullptr), _rhs(nullptr) {
	}

	inline T get() {
		return _t;
	}
	inline void setlhs(Node *lhs) {
		_lhs = lhs;
	}
	inline void setrhs(Node *rhs) {
		_rhs = rhs;
	}
private:
	T _t;
	std::unique_ptr<Node> _lhs;
	std::unique_ptr<Node> _rhs;
};

class StatementList
{
public:
	StatementList() {

	};
	~StatementList() {
		for (auto *ptr : list)
			delete ptr;
	}
	std::vector<Statement*> list;
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
	int line;
};

class Native
{
public:
	TOK type;
	std::string identifier;
	ArgumentList arguments;
};

#endif // H_LANGUAGE_CONSTRUCTS