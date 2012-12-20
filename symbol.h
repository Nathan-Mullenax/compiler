#ifndef SYMBOL_H
#define SYMBOL_H

#include <list>
#include <string>

using namespace std;

class symbol
{
protected:

	string name;
	list <symbol*> children;
public:
	symbol(string n): name(n){}
	void output(int depth=0)
	{
		for (int i=0; i<depth; i++)
			cout << '\t';
		cout << name << endl;
		for (list <symbol*>::iterator i=children.begin(); i!=children.end(); ++i)
			(*i)->output(depth+1);
	}
};

class expr : public symbol
{
protected:
	bool lvalue;

public: 
	expr(string n):symbol(n){}
	bool get_lvalue(){return lvalue;}

};

class var : public expr
{
public:
	var(token* tok): expr(tok->get_lexeme()) { lvalue = true; }
};


class  constant : public expr
{
public:
	constant(token* tok): expr(tok->get_lexeme()) { lvalue = false; }


};

class unary_expr : public expr
{
public:
	unary_expr(string op, expr * operand_1): expr(op)
	{
		lvalue=operand_1->get_lvalue();
		children.push_back(operand_1);
	}
};

class binary_expr : public expr
{
public:
	binary_expr(string op, expr * operand_1, expr * operand_2, bool l=false): expr(op) 
	{
		lvalue= l && operand_1->get_lvalue() && operand_2->get_lvalue();
		children.push_back(operand_1);
		children.push_back(operand_2); 
	}
};

class conditional_expr : public expr
{
public:
	conditional_expr(expr* cond, expr* expr_1,expr* expr_2):expr("conditional_expr")
	{
		children.push_back(cond);
		children.push_back(expr_1);
		children.push_back(expr_2); 
	}
};

class stmt : public symbol
{
public:
	stmt(string n): symbol(n){}
};

class expr_stmt : public stmt
{
public:
	expr_stmt(expr *e): stmt("expr_stmt")
	{
		children.push_back(e);
	}
	expr_stmt(): stmt("empty_stmt") {}
};

class list_stmt : public stmt
{
public:
	list_stmt(stmt *s1, stmt *s2): stmt("list_stmt")
	{
		children.push_back(s1);
		children.push_back(s2);
	}
};

class if_stmt : public stmt
{
public:
	if_stmt(expr *cond, stmt *s ): stmt("if")
	{
		children.push_back(cond);
		children.push_back(s); 
	}
	if_stmt(expr *cond, stmt *s_1, stmt *s_2 ): stmt("if_else")
	{
		children.push_back(cond);
		children.push_back(s_1);
		children.push_back(s_2);
	}

};

class switch_stmt : public stmt
{
public:
	switch_stmt(expr *cond, stmt *s ): stmt("switch")
	{
		children.push_back(cond);
		children.push_back(s); 
	}
};

class while_stmt : public stmt
{
public:
	while_stmt(expr *cond, stmt *s ): stmt("while")
	{
		children.push_back(cond);
		children.push_back(s); 
	}
	while_stmt(stmt *s,expr *cond  ): stmt("do_while")
	{
		children.push_back(s);
		children.push_back(cond); 
	}
};

class for_stmt : public stmt			//expr|stmt
{
public:
	for_stmt(expr_stmt *start, expr_stmt *cond, expr* action, stmt *s ): stmt("for")
	{
		children.push_back(start);
		children.push_back(cond);
		children.push_back(action); 
		children.push_back(s); 
	}
	for_stmt(expr_stmt *start,expr_stmt *cond, stmt *s   ): stmt("for")
	{
		children.push_back(s);
		children.push_back(cond); 
		children.push_back(s); 
	}
};


class jump_stmt : public stmt
{
public:
	jump_stmt(string s): stmt(s){}
	jump_stmt(expr *reterned): stmt("return")
	{
		children.push_back(reterned);
	}
};

class label_stmt : public stmt
{
public:
	label_stmt(expr *l, stmt *s): stmt("case")
	{
		children.push_back(l);
		children.push_back(s);
	}
	label_stmt(stmt *s): stmt("default")
	{
		children.push_back(s);
	}
};








#endif