#include "parser.h"

inline bool parser::is_type(t_type t)
{
	if ( lookahead->get_type() != t) return false;
	else { get_token(); return true; }
}

bool parser::is_spec()
{
	return lookahead->get_type() == CHAR || lookahead->get_type() == INT || lookahead->get_type() == FLOAT || lookahead->get_type() == VOID;
}

static int prior_op(t_type op)
{
	if (op == STAR || op == DIV || op == MOD)			return 1;							//multi op
	if (op == ADD || op == SUB)							return 2;							//add op
	if (op == SHIFT_L || op == SHIFT_R)					return 3;							//shift_op
	if (op == GREATER || op == LESS || op == GREATER_EQ || op == LESS_EQ)	return 4;		//relat_op
	if (op == NOT_EQUAL || op == EQUAL)					return 5;							//eq_op
	if (op == BIT_AND)									return 6;							//and_op
	if (op == XOR)										return 7;							//exc_or_op
	if (op == BIT_OR)									return 8;							//inc_or_op
	if (op == AND)										return 9;							//logic_and_op
	if (op == OR)										return 10;							//logic_or_op
	if	( op == ASSIGN || op == DIV_ASSIGN || op == MUL_ASSIGN || op == MOD_ASSIGN 
		 || op == ADD_ASSIGN || op == SUB_ASSIGN || op ==XOR_ASSIGN  || op == SHIFT_L_ASSIGN
		 || op == SHIFT_R_ASSIGN || op == OR_ASSIGN || op == AND_ASSIGN)	return 11;		//assign_op
	if (op == COMMA)									return 12;							//consist_culc_op
	return 0;
}

/*--------------------------------------------expration----------------------------------------*/
var *parser::parse_var()
{
	var *cur=new var(lookahead);
	get_token();
	return cur;
}

constant *parser::parse_constant()			// реализовать нормально
{
	constant *cur=new constant(lookahead);
	get_token();
	return cur;
}

expr *parser::parse_primary_expr()
{
	expr *cur;
	switch (lookahead->get_type())
	{
	case IDENTIFIER:	return parse_var();
	case FLOAT_CONST:
	case INT_CONST:
	case CHAR_CONST:
	case STRING:		return parse_constant();
	case L_PARENTH:
		get_token();
		cur = parse_expr();
		if (!is_type(R_PARENTH)) error("missing ')'");
		return cur;
	default: error("unexpected token");
	}
}

bool parser::parse_more_postfix_expr(expr *& cur)
{
	switch (lookahead->get_type())
	{
	case L_BRACKET:
		get_token();
		cur = new index(cur, parse_expr());
		if (!is_type(R_BRACKET))	error("missing ']'");
		return true;
	case L_PARENTH:
		get_token();
		if (lookahead->get_type() == R_PARENTH)	
			cur = new func(cur);					
		else
			cur=new func(cur, parse_expr());			
		if (!is_type(R_PARENTH))	error("missing ')'");
		return true;
	case ARROW:
		get_token();
		cur=new ref(cur, parse_var(), true);
		return true;
	case DOT:
		get_token();
		cur=new ref(cur, parse_var(), false);
		return true;
	case INC:
	case DEC:
		cur=new unary_expr(lookahead->get_type(), cur, true);
		get_token();
		return true;
	default:
		return 0;
	}
}



expr *parser::parse_postfix_expr()
{	
	expr *cur=parse_primary_expr();
	while(parse_more_postfix_expr(cur));
	return cur;
}

expr *parser::parse_prefix_expr()
{
	token *temp=lookahead;
	while(lookahead->get_type() == INC || lookahead->get_type() == DEC || lookahead->get_type() == SUB 
		|| lookahead->get_type() == ADD || lookahead->get_type() == BIT_AND || lookahead->get_type() == BIT_OR 
		|| lookahead->get_type() == STAR || lookahead->get_type() == XOR || lookahead->get_type() == DIV 
		|| lookahead->get_type() == SIZEOF)
	{
		get_token();
		if (temp->get_type() == L_PARENTH);	//		cast_expr!sizeof! реализация отложена до сем. анализа
		return new unary_expr(temp->get_type(), parse_prefix_expr());
	}
	return parse_postfix_expr();
}


expr *parser::parse_binary_expr(int priority)
{
	expr *cur;
	switch (priority)
	{
		case 1  : cur=parse_prefix_expr();break;
		case 12 : cur=parse_assign_expr();break;
		default : cur=parse_binary_expr(priority-1);
	}
	t_type op = lookahead->get_type();
	while( priority == prior_op(lookahead->get_type()) )
	{
		get_token();
		switch (priority)
		{
			case 1  : cur=new binary_expr(op, cur, parse_prefix_expr());break;
			case 12 : cur=new binary_expr(op, cur, parse_assign_expr());break;
			default : cur=new binary_expr(op, cur, parse_binary_expr(priority-1));
		}
	}
	return cur;
}

expr *parser::parse_conditional_expr()
{
	expr *cond=parse_binary_expr(10);
	if (is_type(QUESTION))	 
	{
		expr *expr_1=parse_expr();
		if (!is_type(COLON)) error ("missing ':' in conditional expration");
		expr *expr_2=parse_conditional_expr();
		cond=new conditional_expr(cond, expr_1, expr_2);
	}
	return cond;
}

expr *parser::parse_assign_expr()
{
	expr *cur=parse_conditional_expr();
	if	( prior_op(lookahead->get_type()) == 11)	
	{
		t_type op = lookahead->get_type();
		if (!cur->get_lvalue())	error("expr before " + lookahead->get_lexeme() + " must be left value");
		get_token();
		cur=new binary_expr(op, cur, parse_assign_expr());
	}
	return cur;
}

expr *parser::parse_expr()
{
	return parse_binary_expr(12);
}

expr_stmt *parser::parse_expr_stmt()
{
	if (is_type(SEMICOLON)) return new expr_stmt();
	expr_stmt *cur = new expr_stmt(parse_expr());
	if (!is_type(SEMICOLON)) error("missing ';'");
	return cur;
}
/*--------------------------------------------statement----------------------------------------*/
stmt *parser::parse_stmt()
{
	compound_stmt *block = 0;
	expr *e;
	stmt *s, *s2;
	expr_stmt *es, *es2;
	switch(lookahead->get_type())
	{
	case IF:
		get_token();
		if (!is_type(L_PARENTH)) error("missing '('");
		e = parse_expr();
		if (!is_type(R_PARENTH)) error("missing ')'");
		s = parse_stmt();
		if (lookahead->get_type() == ELSE)
		{
			get_token();
			s2 = parse_stmt();
			return new if_stmt(e, s, s2);
		}
		return new if_stmt(e, s);
	case SWITCH:
		get_token();
		if (!is_type(L_PARENTH)) error("missing '('");
		e = parse_expr();
		if (!is_type(R_PARENTH)) error("missing ')'");
		s = parse_stmt();
		return new switch_stmt(e, s);
	case WHILE:
		get_token();
		if (!is_type(L_PARENTH)) error("missing '('");
		e = parse_expr();
		if (!is_type(R_PARENTH)) error("missing ')'");
		s = parse_stmt();
		return new while_stmt(e, s);
	case DO:
		get_token();
		s = parse_stmt();
		//require(WHILE, "expected 'while'");
		if (!is_type(WHILE)) error("expected 'while'");
		if (!is_type(L_PARENTH)) error("missing '('");
		e = parse_expr();
		if (!is_type(R_PARENTH)) error("missing ')'");
		if (!is_type(SEMICOLON)) error("missing ';'");
		return new do_stmt(s, e);
	case FOR:
		get_token();
		e = 0;

		if (!is_type(L_PARENTH)) error("missing '('");
		es = parse_expr_stmt();
		es2 = parse_expr_stmt();
		if (!is_type(R_PARENTH)) {
			e=parse_expr();
			if (!is_type(R_PARENTH)) error("missing ')'");
		}
		s = parse_stmt();
		if (e) return new for_stmt(es, es2, e, s);
		return new for_stmt(es, es2, s);
	case CONTINUE:;
		get_token();
		s = new continue_stmt();
		if (!is_type(SEMICOLON)) error("missing ';'");
		return s;
	case BREAK:
		get_token();
		s = new break_stmt();
		if (!is_type(SEMICOLON)) error("missing ';'");
		return s;
	case RETURN:
		get_token();
		if (is_type(SEMICOLON)) return new return_stmt();
		s = new return_stmt(parse_expr());
		if (!is_type(SEMICOLON)) error("missing ';'");
		return s;
	case CASE:
		get_token();
		e = parse_conditional_expr();
		if (!is_type(COLON)) error("missing ':'");
		return new case_stmt(e, parse_stmt());
	case DEFAULT:
		get_token();
		if (!is_type(COLON)) error("missing ':'");
		return new default_stmt(parse_stmt());
	case L_BRACE:
		get_token();
		block = new compound_stmt("statement");
		while(is_type(R_BRACE))
		{
			block->add(parse_stmt());
		}		
		return block;
	default:
		return parse_expr_stmt();
	}
}

/*--------------------------------------------declaration----------------------------------------*/
sym_type *parser::det_base_type()
{
	switch (get_token()->get_type())
	{
	case INT:		return new type_int();
	case CHAR:		return new type_char();
	case FLOAT:		return new type_float();
	case VOID:		return new type_void();
	case STRUCT:	if (!lookahead->get_type() == IDENTIFIER) return new type_struct(parse_consist());
					else return new type_struct(get_token()->get_lexeme(), parse_consist());
	default: error("expected type");
	}
}


void parser::parse_decl()
{
	sym_type *spec = det_base_type();	
	do	cur_table->push(parse_single_spec(spec));
	while(is_type(COMMA));
	if (!is_type(SEMICOLON)) error("missing ';'");
}


derivative_type *parser::parse_declarator(derivative_type *head, string *ident)
{
	derivative_type *last = 0, *temp = 0;
	int pointer = 0;
	while (is_type(STAR))
		pointer++;
	if (is_type(L_PARENTH))
	{
		last = parse_declarator(head, ident);
		if (!is_type(R_PARENTH)) error("missing ')'");
	}
	else if (lookahead->get_type() == IDENTIFIER)
		*ident = lookahead->get_lexeme();
	else error(lookahead->get_lexeme() + ", expected identifier");
	if (is_type(L_PARENTH))
	{
		if (is_type(R_PARENTH))
		{
			temp = new type_func();
			last->det_base_type(temp);
			last = temp;
		}
		temp = new type_func(parse_arg());
		last->det_base_type(temp);
		last = temp;
		if (!is_type(R_PARENTH)) error("missing ')'");
		if (!head) head = last;
		return last;
	}
	while (is_type(L_BRACKET))
	{
		if (is_type(R_BRACKET))
		{
			temp = new type_array();
			last->det_base_type(temp);
			last = temp;
		}
		temp = new type_array(parse_conditional_expr());
		last->det_base_type(temp);
		last = temp;
		if (!is_type(R_BRACKET)) error("missing ']'");
		if (!head) head = last;
	}
	while (pointer)
	{
		temp = new type_pointer();
		last->det_base_type(temp);
		last = temp;
		if (!head) head = last;
		pointer --;
	}
	return last;
}

sym_var *parser::parse_single_spec(sym_type *spec)
{
	derivative_type *head = 0, *last;
	string *ident = 0;
	last = parse_declarator(head, ident);
	if (head) { last->det_base_type(spec); return new sym_var(head, *ident); }
	else return new sym_var(spec, *ident);
}


symtable *parser::parse_arg()
{	
	symtable *ret = new symtable();
	sym_type *spec = 0;
	if (!is_spec()) error("expected arguments type");
	do	ret->push(parse_single_spec(det_base_type()));
	while(is_type(COMMA));
	return ret;
}

symtable *parser::parse_consist()
{
	symtable *ret = new symtable();
	if (!is_spec()) error("expected consist of structure");
	do 
		ret->push(new sym_var(det_base_type(), lookahead->get_lexeme()));
	while(is_type(COMMA));
	return ret;
}


parser::parser(scanner *sc): scan(sc)
{
	get_token();
	compound_stmt *all = new compound_stmt("programm");
	while (lookahead->get_type() != EOF_TYPE)
		all->add(parse_stmt());
	root = all;
}