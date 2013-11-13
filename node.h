#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <string>
#include <list>
#include "scanner.h"
#include "asm_cmd.h"

using namespace std;

static enum op_op {NULL_OP, MULTI_OP, ADD_OP,
	SHIFT_OP, RELAT_OP, EQ_OP, AND_OP,
	EXC_OR_OP, INC_OR_OP, LOGIC_AND_OP,
	LOGIC_OR_OP, ASSIGN_OP, COM_OP};

inline void error(string msg) { throw msg; }
#include "sym.h"

class node
{
protected:
	string name;
	list <node*> children;
public:

	node(string n): name(n) {}
	virtual void output(int depth = 0);
	virtual asm_operand *gen() {return 0;}
	virtual bool is_comp() {return false;}
};

/*--------------------------------------------expration----------------------------------------*/
class expr : public node
{
protected:
	sym_type *expr_type;
	bool lvalue;
	void mov_val2reg(asm_operand *source);
	asm_operand *reg();
	asm_operand *addr(asm_operand *base, asm_operand *offset);

public: 
	expr(string n, bool l = false):node(n), lvalue(l){}
	bool get_lvalue(){return lvalue;}
	sym_type *get_type() { return expr_type; }
};

/*----support-----*/
static bool is_expr_of_type(expr *e, type fst, ...)
{
	if (e->get_type()->get_type() == VOID_TYPE)
		error("univalable void using");
	for (type *i = &fst; *i; i++)
		if (e->get_type()->get_type() == *i)
			return true;
	return false;
}
static bool arg_comp(list <sym_var *> *fst, list <expr *> *sec)
{
	if (!fst && !sec) return true;
	list <sym_var*>::iterator i=fst->begin();list <expr*>::iterator j = sec->begin();
	if (fst && sec)
	{
		for (; i!=fst->end(); ++i, ++j)
		{
			if (*(*i)->get_type() != *(*j)->get_type()) 
				return false;
		}
		if (j == sec->end()) return true;
		return false;
	}
	return false;
}
static op_op prior_op(t_type op)
{
	if (op == STAR || op == DIV || op == MOD)			return MULTI_OP;						//multi op
	if (op == ADD_TOK || op == SUB_TOK)					return ADD_OP;							//add op
	if (op == SHIFT_L || op == SHIFT_R)					return SHIFT_OP;						//shift_op
	if (op == GREATER || op == LESS 
		|| op == GREATER_EQ || op == LESS_EQ)			return RELAT_OP;						//relat_op
	if (op == NOT_EQUAL || op == EQUAL)					return EQ_OP;							//eq_op
	if (op == BIT_AND)									return AND_OP;							//and_op
	if (op == XOR_TOK)										return EXC_OR_OP;						//exc_or_op
	if (op == BIT_OR)									return INC_OR_OP;						//inc_or_op
	if (op == AND_TOK)										return LOGIC_AND_OP;					//logic_and_op
	if (op == OR_TOK)										return LOGIC_OR_OP;						//logic_or_op
	if	( op == ASSIGN || op == DIV_ASSIGN 
		|| op == MUL_ASSIGN || op == MOD_ASSIGN 
		|| op == ADD_ASSIGN || op == SUB_ASSIGN 
		|| op ==XOR_ASSIGN  || op == SHIFT_L_ASSIGN
		|| op == SHIFT_R_ASSIGN || op == OR_ASSIGN 
		|| op == AND_ASSIGN)							return ASSIGN_OP;						//assign_op
	if (op == COMMA)									return COM_OP;							//consist_culc_op
	return NULL_OP;
}

/*----------------*/

class var : public expr
{
private:
	sym_var *variable;
public:
	var(string ident, symtable *table);
	asm_operand *gen();
	sym_var *get_sym_var() {return variable;}
};

class constant : public expr
{
protected:
	int value;
public :
	constant(string n, int _value): expr(n, false), value(_value){}
	asm_operand *gen();
};

class  const_int : public constant
{
public:
	const_int(token_v<int> *t): constant("",t->get_value())
	{ 
	expr_type = new type_int();
	char n[20];
	name += itoa(value, n, 10);
	}

const_int::const_int(int _value): constant("", _value)
	{ 
	expr_type = new type_int();
	char n[20];
	name += itoa(value, n, 10);
	}
};

class  const_char : public constant
{
public:
	const_char(token_v<char> *t): constant(t->get_lexeme(),t->get_value()) {expr_type = new type_char();}
};

class  const_float : public constant
{
public:
	const_float(token_v<float> *t): constant(t->get_lexeme(),0)
	{
		float v = t->get_value();
		value = *(int *)(void *)(&v);
		expr_type = new type_float();
	}
	asm_operand *gen();
};

class  const_string : public expr				// подсделать
{
private:
	string value;
public:
	const_string(token_v<string> *t): expr(t->get_lexeme()), value(t->get_value()) 
	{ 
		expr_type = new type_array(value.length());
		dynamic_cast<type_array *>(expr_type)->det_base_type(new type_char);
	}
};

class index : public expr
{
private:
	int mass_size;
public:
	index(expr *mass, expr *index);
	asm_operand *gen();
};

class inder : public expr
{
public:
	inder(expr *operand);
	asm_operand *gen();
};

class func : public expr
{
public:
	func(expr *func, list <expr *> *arg = 0);
	asm_operand *gen();
};

class ref : public expr
{
private:
	sym_var *member_sym;
public:
	ref(expr *structure, token *member, bool p);
	asm_operand *gen();
};

class addr : public expr
{
public:
	addr(expr *operand);
};

class size : public expr
{
public:
	size (expr *operand);
	size (t_type spec);
};

class cast : public expr
{
public:
	cast(expr *operand, sym_type *_type);
	void output(int depth = 0)
	{
		for (int i=0; i<depth; i++)
			cout << '\t';
			cout << name; 
			expr_type->output();
			cout << endl;
		for (list <node*>::iterator i=children.begin(); i!=children.end(); ++i)
			(*i)->output(depth+1);
	}
};

class unary_expr : public expr
{
private:
	bool postfix;
public:
	unary_expr(t_type op, expr *operand, bool p = false);
};

class arithm_expr : public expr
{
public:
	arithm_expr(t_type op, expr * operand_1, expr * operand_2);
};

class logic_expr : public expr
{
public:
	logic_expr(t_type op, expr * operand_1, expr * operand_2);
};

class conditional_expr : public expr
{
public:
	conditional_expr(expr* cond, expr *expr_1,expr *expr_2);
};

class assign_expr : public expr
{
public:
	assign_expr(expr *expr_1, t_type op, expr *expr_2);
};

class comma_expr : public expr
{
public:
	comma_expr(expr *expr_1, expr *expr_2);
};

/*--------------------------------------------statement----------------------------------------*/
class stmt : public node
{
public:
	stmt(string n): node(n){}
};

class symtable;
class compound_stmt : public stmt
{
private:
	symtable *local_var;
public:
	compound_stmt(string n, symtable *local): stmt("list_" + n), local_var(local) {}
	void add(node * child)
	{
		children.push_back(child);
	}
	void output(int depth = 0);
	virtual bool is_comp() {return true;}
	void gen(int offset);
};

class expr_stmt : public stmt
{
public:
	expr_stmt(expr *e): stmt("expr_stmt")
	{
		children.push_back(e);
	}
	expr_stmt(): stmt("empty_stmt") {}
	asm_operand * gen();
};



class if_stmt : public stmt
{
public:
	if_stmt(expr *cond, stmt *s ): stmt("IF")
	{
		children.push_back(cond);
		children.push_back(s); 
	}
	if_stmt(expr *cond, stmt *s_1, stmt *s_2 ): stmt("IF_ELSE")
	{
		children.push_back(cond);
		children.push_back(s_1);
		children.push_back(s_2);
	}

};

class switch_stmt : public stmt
{
public:
	switch_stmt(expr *cond, stmt *s ): stmt("SWITCH")
	{
		children.push_back(cond);
		children.push_back(s); 
	}
};

class while_stmt : public stmt
{
public:
	while_stmt(expr *cond, stmt *s ): stmt("WHILE")
	{
		children.push_back(cond);
		children.push_back(s); 
	}
};

class do_stmt : public stmt
{
public:
	do_stmt(stmt *s,expr *cond): stmt("DO_WHILE")
	{
		children.push_back(s);
		children.push_back(cond); 
	}
};

class for_stmt : public stmt
{
public:
	for_stmt(expr_stmt *start, expr_stmt *cond, expr* action, stmt *s ): stmt("FOR")
	{
		children.push_back(start);
		children.push_back(cond);
		children.push_back(action); 
		children.push_back(s); 
	}
	for_stmt(expr_stmt *start,expr_stmt *cond, stmt *s   ): stmt("FOR")
	{
		children.push_back(start);
		children.push_back(cond); 
		children.push_back(s); 
	}
};


class break_stmt : public stmt
{
public:
	break_stmt() : stmt("BREAK") {}
};

class continue_stmt : public stmt
{
public:
	continue_stmt() : stmt("CONTINUE") {}
};

class return_stmt : public stmt
{
public:
	return_stmt() : stmt("RETURN") {}
	return_stmt(expr *reterned): stmt("RETURN")
	{
		children.push_back(reterned);
	}
};
class case_stmt : public stmt
{
public:
	case_stmt(expr *l, stmt *s): stmt("CASE")
	{
		children.push_back(l);
		children.push_back(s);
	}
};

class default_stmt : public stmt
{
public:
	default_stmt(stmt *s): stmt("DEFAULT")
	{
		children.push_back(s);
	}
};

class io_stmt : public stmt
{
public:
	io_stmt(list<expr *> *arg): stmt("IO_STMT")
	{
		for (list <expr *>::iterator i=arg->begin(); i!=arg->end(); ++i)
			children.push_back(*i);
	}
};

#endif