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

//class stmt : public symbol
//{
//public:
//	stmt(string n): symbol(n){}
//};
//
//class if_stmt : public stmt
//{
//public:
//	if_stmt(expr *cond, stmt *s ): stmt("if")
//	{
//		children.push_back(cond);
//		children.push_back(s); 
//	}
//	if_stmt(expr *cond, stmt *s_1, stmt *s_2 ): stmt("if_else")
//	{
//		children.push_back(cond);
//		children.push_back(s_1);
//		children.push_back(s_2);
//	}
//
//};
//
//class switch_stmt : public stmt
//{
//public:
//	switch_stmt(expr *cond, stmt *s ): stmt("switch")
//	{
//		children.push_back(cond);
//		children.push_back(s); 
//	}
//};
//
//class while_stmt : public stmt
//{
//public:
//	while_stmt(expr *cond, stmt *s ): stmt("while")
//	{
//		children.push_back(cond);
//		children.push_back(s); 
//	}
//	while_stmt(stmt *s,expr *cond  ): stmt("do_while")
//	{
//		children.push_back(s);
//		children.push_back(cond); 
//	}
//};
//class for_stmt : public stmt			//expr|stmt
//{
//public:
//	for_stmt(expr *start, expr *cond, expr* action ): stmt("for")
//	{
//		children.push_back(start);
//		children.push_back(cond);
//		children.push_back(action); 
//	}
//	for_stmt(expr *s,expr *cond  ): stmt("for")
//	{
//		children.push_back(s);
//		children.push_back(cond); 
//	}
//};
//class switch_stmt : public stmt
//{
//public:
//	switch_stmt(expr *cond, stmt *s ): stmt("switch")
//	{
//		children.push_back(cond);
//		children.push_back(s); 
//	}
//};


class terminal : public expr
{

public:
	terminal(token* tok):expr(tok->get_lexem()) {}
};

#endif