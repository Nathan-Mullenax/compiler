#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "scanner.h"
#include <string>
#include "symbol.h"

using namespace std;

class parser
{
	private:
		token* lookahead;
		scanner* scan;
		void get_token()		{ lookahead = scan->get_token(); }
		void error(string msg)	{ scan->error(msg , "syntax error:"); }
		symbol *root;
		inline bool is_type(t_type t);

		var *parse_var();
		constant *parse_constant();
		expr *parse_primary_expr();
		expr *parse_prefix_expr();
		expr *parse_postfix_expr();
		bool parse_more_postfix_expr(expr* &cur);
		expr *parse_binary_expr(int pr);
		expr *parse_conditional_expr(); 
		expr *parse_assign_expr(); 
		expr *parse_expr(); 

		expr_stmt *parse_expr_stmt();
		label_stmt *parse_label_stmt();
		stmt *parse_stmt();
		stmt *parse_list_stmt();


	public:
		parser(scanner* sc);
		void output ()			{root->output();}
};

#endif