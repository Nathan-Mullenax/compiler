#include "parser.h"

inline bool parser::is_type(t_type t)
{
	if ( lookahead->get_type() != t) return false;
	get_token(); return true;
}


static int prior_op(t_type op)
{
	if (op == STAR || op == DIV || op == MOD)						return 1;				//multi op
	if (op == ADD || op == SUB)								return 2;						//add op
	if (op == SHIFT_L || op == SHIFT_R)						return 3;						//shift_op
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
	case FLOAT:
	case INT:
	case CHAR:
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
		cur=new binary_expr("index", cur, parse_expr(), true);
		if (!is_type(R_BRACKET))	error("missing ']'");
		return true;
	case L_PARENTH:
		get_token();
		if (lookahead->get_type() == R_PARENTH)	
			cur=new unary_expr("call_func", cur);					// lvalue false!
		else
			cur=new binary_expr("call_func", cur, parse_expr(), false);			
		if (!is_type(R_PARENTH))	error("missing ')'");
		return true;
	case ARROW:
		get_token();
		cur=new binary_expr("structure_dereference ", cur, parse_primary_expr(), true);
		return true;
	case DOT:
		get_token();
		cur=new binary_expr("structure_reference ", cur, parse_primary_expr(), true);
		return true;
	case INC:
		get_token();
		cur=new unary_expr("postfix_inc", cur);
		return true;
	case DEC:
		get_token();
		cur=new unary_expr("postfix_dec", cur);
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
		if (temp->get_type() == L_PARENTH);	//		cast_expr!sizeof!
		if (temp->get_type() == INC || temp->get_type() == DEC)
			return new unary_expr("prefix" + temp->get_lexeme(), parse_prefix_expr());
		return new unary_expr(temp->get_lexeme(), parse_prefix_expr());
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

	while( priority == prior_op(lookahead->get_type()) )
	{
		string op=lookahead->get_lexeme();
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
		string op=lookahead->get_lexeme();
		if (!cur->get_lvalue())					error("expr before " + op + " must be left value");
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

stmt *parser::parse_stmt()
{
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
		if (!is_type(L_PARENTH)) error("missing '('");
		e = parse_expr();
		if (!is_type(R_PARENTH)) error("missing ')'");
		if (!is_type(SEMICOLON)) error("missing ';'");
		return new while_stmt(e, s);
	case FOR:
		get_token();
		e = 0;
		if (!is_type(L_PARENTH)) error("missing '('");
		es = parse_expr_stmt();
		es2 = parse_expr_stmt();
		if (!is_type(R_PARENTH)) e=parse_expr();
		if (!is_type(R_PARENTH)) error("missing ')'");
		s = parse_stmt();
		if (e) return new for_stmt(es, es2, e, s);
		return new for_stmt(es, es2, s);
	case CONTINUE:;
	case BREAK:
		s = new jump_stmt(lookahead->get_lexeme());
		get_token();
		if (!is_type(SEMICOLON)) error("missing ';'");
		return s;
	case RETURN:
		get_token();
		if (is_type(SEMICOLON)) return new jump_stmt(lookahead->get_lexeme());
		s = new jump_stmt(parse_expr());
		if (!is_type(SEMICOLON)) error("missing ';'");
		return s;
	case CASE:
		e = parse_conditional_expr();
		if (!is_type(COLON)) error("missing ':'");
		return new label_stmt(e, parse_stmt());
	case DEFAULT:
		if (!is_type(COLON)) error("missing ':'");
		return new label_stmt(parse_stmt());
	default:
		return parse_expr_stmt();
	}
}



stmt *parser::parse_list_stmt()
{
	stmt *cur = parse_stmt();
	while(lookahead->get_type() != EOF_TYPE)
		cur = new list_stmt(cur, parse_stmt());
	return cur;
}


	

parser::parser(scanner *sc): scan(sc)
{
	get_token();
	root=parse_list_stmt();
}