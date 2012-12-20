//#include "parser.h"
//
//static int prior_op(string op)
//{
//	if (op == "*" || op == "/" || op == "%")				return 1;		//multi op
//	if (op == "+" || op == "-")							return 2;		//add op
//	if (op == "<<" || op == ">>")						return 3;		//shift_op
//	if (op == "<" || op == ">" || op == "<=" || op == ">=")	return 4;		//relat_op
//	if (op == "!+" || op == " == ")						return 5;		//eq_op
//	if (op == "&")									return 6;		//and_op
//	if (op == "^")									return 7;		//exc_or_op
//	if (op == "|")									return 8;		//inc_or_op
//	if (op == "&&")									return 9;		//logic_and_op
//	if (op == "||")									return 10;		//logic_or_op
//	if	( op == "="	|| op == "*="	|| op == "/="	|| op == "+="				
//		|| op == "-=" ||op == "%=" 	|| op == ">>="|| op == "<<=" 
//		|| op == "^="	|| op == "|=" || op == "&=")		return 11;		//assign_op
//	if (op == ",")									return 12;		//consist_culc_op
//	return 0;
//}
//
//
//expr* parser::parse_primary_expr()
//{
//	expr* cur;
//	if (lookahead->get_type() == "DEC_INT" 
//		||lookahead->get_type() == "FLOAT"
//		||lookahead->get_type() == "HEX_INT" 
//		||lookahead->get_type() == "OCT_INT" 
//		||lookahead->get_type() == "CHAR" 
//		||lookahead->get_type() == "STRING" 
//		||lookahead->get_type() == "IDENTIFIER")
//	{
//		cur=new terminal(lookahead);
//		get_token();
//	}
//	else if (lookahead->get_type() == "(" )
//	{
//		get_token();
//		cur = parse_expr();
//		if (lookahead->get_type() != ")") error("missing ')'");
//		get_token();
//	}
//	else error(lookahead->get_type());
//	return cur;
//}
//
//bool parser::parse_more_postfix_expr(expr* & cur)
//{
//	if (lookahead->get_type() == "[")	
//	{
//		get_token();
//		cur=new binary_expr("index",cur,parse_expr(),true);
//		if (lookahead->get_type() != "]")	error("missing ']'");
//		get_token();
//		return 1;
//	}
//	if (lookahead->get_type() == "(")
//	{
//		get_token();
//		if (lookahead->get_type() == ")")	
//			cur=new unary_expr("call_func",cur);
//		else
//			cur=new binary_expr("call_func",cur,parse_expr(),true);			
//		if (lookahead->get_type() != ")")	error("missing ')'");
//		get_token();
//		return 1;
//	}
//	if (lookahead->get_type() == "." || lookahead->get_type() == "->")	
//	{
//		get_token();
//		cur=new binary_expr(lookahead->get_type(),cur,parse_primary_expr(),true);
//		get_token();
//		return 1;
//	}
//	if (lookahead->get_type() == "--" || lookahead->get_type() == "++")	
//	{
//		get_token();
//		cur=new unary_expr("postfix"+lookahead->get_type(),cur);
//		get_token();
//		return 1;
//	}
//	return 0;
//}
//
//
//
//expr* parser::parse_postfix_expr()
//{	
//	expr* cur=parse_primary_expr();
//	while(parse_more_postfix_expr(cur));
//	return cur;
//}
//
//expr* parser::parse_prefix_expr()
//{
//	token* temp=lookahead;
//	while(lookahead->get_type() == "++" 
//		|| lookahead->get_type() == "--" 
//		|| lookahead->get_type() == "-" 
//		|| lookahead->get_type() == "+" 
//		|| lookahead->get_type() == "&" 
//		|| lookahead->get_type() == "*" 
//		|| lookahead->get_type() == "^" 
//		|| lookahead->get_type() == "/" 
//		|| lookahead->get_type() == "sizeof" 
//		|| lookahead->get_type() == "|=") 
//	{
//		get_token();
//	//	if (temp->get_type() == "(")			cast_expr!sizeof!
//
//		if (temp->get_type() == "--" || temp->get_type() == "++")
//			return new unary_expr("prefix"+temp->get_type(),parse_prefix_expr());
//		return new unary_expr(temp->get_type(),parse_prefix_expr());
//	}
//	return parse_postfix_expr();
//}
//
//
//expr* parser::parse_binary_expr(int priority)
//{
//	expr* cur;
//	switch (priority)
//	{
//		case 1  : cur=parse_prefix_expr();break;
//		case 12 : cur=parse_assign_expr();break;
//		default : cur=parse_binary_expr(priority-1);
//	}
//
//	while( priority == prior_op(lookahead->get_type()) )
//	{
//		string op=lookahead->get_type();
//		get_token();
//		switch (priority)
//		{
//			case 1  : cur=new binary_expr(op,cur,parse_prefix_expr());break;
//			case 12 : cur=new binary_expr(op,cur,parse_assign_expr());break;
//			default : cur=new binary_expr(op,cur,parse_binary_expr(priority-1));
//		}
//	}
//	return cur;
//}
//
//expr* parser::parse_conditional_expr()
//{
//	expr* cond=parse_binary_expr(10);
//	if (lookahead->get_type() == "?")	 
//	{
//		get_token();
//		expr* expr_1=parse_expr();
//		if (lookahead->get_type() != ":") error ("missing ':' in conditional expration");
//		get_token();
//		expr* expr_2=parse_conditional_expr();
//		cond=new conditional_expr(cond,expr_1,expr_2);
//	}
//	return cond;
//}
//
//expr* parser::parse_assign_expr()
//{
//	expr* cur=parse_conditional_expr();
//	if	( prior_op(lookahead->get_type()) == 11)	
//	{
//		string op=lookahead->get_type();
//		if (!cur->get_lvalue())					error("expr before " + op + " must be left value");
//		get_token();
//		cur=new binary_expr(op,cur,parse_assign_expr());
//	}
//	return cur;
//}
//
//expr* parser::parse_expr()
//{
//	return parse_binary_expr(12);
//}
//
//parser::parser(scanner* sc): scan(sc)
//{
//
//	get_token();
//	root=parse_expr();
//}