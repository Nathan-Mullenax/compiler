#ifndef NODE_H
#define NODE_H

#include <list>
#include <string>

using namespace std;

class node
{
protected:

	string name;
	list <node*> children;
public:
	node(string n): name(n) {}
	void output(int depth=0)
	{
		for (int i=0; i<depth; i++)
			cout << '\t';
		cout << name << endl;
		for (list <node*>::iterator i=children.begin(); i!=children.end(); ++i)
			(*i)->output(depth+1);
	}

};



/*--------------------------------------------expration----------------------------------------*/
class expr : public node
{
protected:
	bool lvalue;

public: 
	expr(string n, bool l):node(n), lvalue(l){}
	bool get_lvalue(){return lvalue;}

};


class var : public expr
{
private:
	token *tok;
public:
	var(token* t): expr(t->get_lexeme(), true), tok(t) {}
};


class  constant : public expr
{
private:
	token *tok;
public:
	constant(token* t): expr(t->get_lexeme(), false), tok(t) {}
};

class func : public expr
{
public:
	func(expr *name, expr *arg = 0): expr("CALL_FUNC", false)
	{
		children.push_back(name);
		if (arg) children.push_back(arg);
	}
};

class index : public expr
{
public:
	index(expr *mass, expr *index): expr("INDEX", mass->get_lvalue() && true)
	{
		children.push_back(mass);
		children.push_back(index);
	}
};

class ref : public expr
{
private:
	bool ptr;
public:
	ref(expr *stucture, expr *member, bool p): expr("REF", true), ptr(p)
	{
		children.push_back(stucture);
		children.push_back(member);
	}
};

class unary_expr : public expr
{
private:
	bool postfix;
public:
	unary_expr(t_type op, expr *operand_1, bool p = false): postfix(p),  expr(type_str[op], true)
	{
		if(p) name = "postfix " + name;
		lvalue = operand_1->get_lvalue();
		children.push_back(operand_1);
	}
};

class binary_expr : public expr
{
public:
	binary_expr(t_type op, expr * operand_1, expr * operand_2): expr(type_str[op], false) 
	{
		children.push_back(operand_1);
		children.push_back(operand_2); 
	}
};

class conditional_expr : public expr
{
public:
	conditional_expr(expr* cond, expr* expr_1,expr* expr_2):expr("conditional_expr", false)
	{
		children.push_back(cond);
		children.push_back(expr_1);
		children.push_back(expr_2); 
	}
};
/*--------------------------------------------statement----------------------------------------*/
class stmt : public node
{
public:
	stmt(string n): node(n){}
};

class compound_stmt : public stmt
{
public:
	compound_stmt(string n): stmt("list_" + n) {}
	void add(node * child)
	{
		children.push_back(child);
	}
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

class for_stmt : public stmt			//expr|stmt
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
		children.push_back(s);
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


/*--------------------------------------------decloration----------------------------------------*/
//
//class declarator : public node
//{
//protected:
//	int pointer;
//	var *ident;
//public:
//	declarator(int p, node *more_decl ): pointer(p), node("decl")
//	{
//		if (!pointer)	name += "_var";
//		else for (int j=0; j<pointer; j++)
//				name += "_pointer_to";
//		children.push_back(more_decl);
//	}
//};
//
//class decl_func : public declarator
//{
//public:
//	decl_func(int p, node *more_decl, list_node *arg = 0): declarator(p, more_decl)
//	{
//		name += "_func";
//		if (arg) children.push_back(arg);
//	}
//};
//
//class decl_mass : public declarator
//{
//public:
//	decl_mass(int p, node *more_decl, expr *border = 0 ): declarator(p, more_decl )
//	{
//		name += "_mass";
//		if (border) children.push_back(border);
//	}
//};
//
//class init_decl : public node
//{
//public:
//	init_decl(declarator *d, node *value): node("=")
//	{
//		children.push_back(d);		
//		children.push_back(value);
//	}
//};
//
//class decl : public node
//{
//private:
//	t_type type;
//	bool const_q;
//public:
//	decl(t_type spec, bool c, node *init_decl): node(type_str[spec]), type(spec), const_q(c)
//	{
//		if (c) name += "const";
//		children.push_back(init_decl);
//	}
//};


#endif