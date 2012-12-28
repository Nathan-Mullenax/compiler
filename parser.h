#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include <string>
#include "node.h"
#include "sym.h"


using namespace std;

class parser
{
	private:
		token* lookahead;
		scanner *scan;
		token *get_token()		{ token *ret = lookahead; 
		lookahead = scan->get_token(); return ret;}
		inline void error(std::string msg) { throw excep(msg, lookahead->get_column(), lookahead->get_line()); }
		node *root;
		inline bool is_type(t_type t);
		inline bool is_spec();

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

		symtable *cur_table;

		expr_stmt *parse_expr_stmt();
		stmt *parse_stmt();

		void parse_decl();
		sym_type *det_base_type();
		sym_var *parse_single_spec(sym_type *base);
		derivative_type *parse_declarator(derivative_type *head, string *a);
		symtable *parse_consist();		
		symtable *parse_arg();
	public:
		parser(scanner* sc);
		void output ()			{root->output();}
};

#endif