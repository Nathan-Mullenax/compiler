#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include <string>
using namespace std;
#include "node.h"

class parser
{
	private:
		token* lookahead;
		scanner *scan;
		node *root;
		symtable *cur_table;
		global_symtable *global_table;
		bool main_detected;

		sym_type *returned;
		bool was_ret;

		token *get_token() { token *ret = lookahead; lookahead = scan->get_token(); return ret; }
		inline void error(string msg) { throw excep(msg, lookahead->get_column(), lookahead->get_line()); }
		inline void expect(t_type tok);
		inline bool is_token(t_type t);
		inline bool is_one_of_type(t_type fst, ...);

		expr *parse_primary_expr(bool l_par = false);
		expr *parse_prefix_expr();
		expr *parse_postfix_expr(bool l_par = false);
		bool parse_more_postfix_expr(expr* &cur);
		expr *parse_binary_expr(int pr);
		expr *parse_conditional_expr(); 
		expr *parse_assign_expr(); 
		expr *parse_expr(); 

		expr_stmt *parse_expr_stmt();
		stmt *parse_stmt(bool so = false, bool co = false);

		void parse_global();
		compound_stmt *parse_block(bool so, bool co, list <sym_var *> *arg_func = 0);		
		sym_type *det_spec();
		sym_var *parse_single_spec(sym_type *base, bool *is_func_def = 0);
		derivative_type *parse_declarator(derivative_type *&head, string &ident);
		symtable *parse_consist();		
		list <sym_var *> * parse_arg();
		type_struct *parse_struct();
	public:
		parser(scanner* sc);
		global_symtable *get_global_table();
		void output () { global_table->output(); }
};

#endif