#include "parser.h"

inline bool parser::is_token(t_type t)
{
	if ( lookahead->get_type() != t) return false;
	else { get_token(); return true; }
}

bool parser::is_one_of_type(t_type fst, ...)
{
	for (t_type *i = &fst; *i; i++)
		if (lookahead->get_type() == *i)	return true;
	return false;
}

static bool arg_comp(list <sym_var *> *fst, list <sym_var *> *sec)
{
	if (!fst && !sec) return true;
	if (fst && sec)
	{
		list <sym_var*>::iterator i=fst->begin(), j = sec->begin();
		for ( ;i!=fst->end(); ++i, ++j)
			if (*(*i)->get_type() != *(*j)->get_type()) return false;
		if (j == sec->end()) return true;
		return false;
	}
	return false;
}

inline void parser::expect(t_type tok)
{
	string str_tok = type_str[tok];
	if (!is_token(tok)) error("expected " + str_tok);
}


/*--------------------------------------------expration----------------------------------------*/

expr *parser::parse_primary_expr(bool l_par)
{
	expr *cur;
	if (l_par)
	{
		cur = parse_expr();
		expect(R_PARENTH);
		return cur;
	}
	switch (lookahead->get_type())
	{
	case IDENTIFIER:	return new var(get_token()->get_lexeme(), cur_table);
	case FLOAT_CONST:	return new const_float(dynamic_cast<token_v<float> *>(get_token()));
	case INT_CONST:		return new const_int(dynamic_cast<token_v<int> *>(get_token()));
	case CHAR_CONST:	return new const_char(dynamic_cast<token_v<char> *>(get_token()));
	case STRING:		return new const_string(dynamic_cast<token_v<string> *>(get_token()));
	case L_PARENTH:
		get_token();
		cur = parse_expr();
		expect(R_PARENTH);
		return cur;
	default: error("expected expression");
	}
}

bool parser::parse_more_postfix_expr(expr *& cur)
{
	expr *sec = 0;
	symtable *temp = 0;
	list <expr*> *arg = new list <expr*>;
	switch (lookahead->get_type())
	{
	case L_BRACKET:
		get_token();
		sec =  parse_expr();
		cur = new index(cur, sec);
		expect(R_BRACKET);
		return true;
	case L_PARENTH:
		get_token();
		if (is_token(R_PARENTH)) { cur = new func(cur); return true; }
		arg->push_back(parse_assign_expr());
		while (is_token(COMMA))
			arg->push_back(parse_assign_expr());
		expect(R_PARENTH);
		cur = new func(cur, arg);
		return true;
	case ARROW:  
		cur = new inder(cur);
	case DOT:
		get_token();
		cur=new ref(cur, get_token(), false);
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


expr *parser::parse_postfix_expr(bool l_par)
{	
	expr *cur=parse_primary_expr(l_par);
	while(parse_more_postfix_expr(cur));
	return cur;
}

expr *parser::parse_prefix_expr()
{
	token *temp = lookahead;
	expr *operand = 0;
	if (is_token(L_PARENTH))
	{
		if(is_one_of_type(CHAR, INT, FLOAT, 0)) 
			{
				sym_type *type = det_spec();
				type_pointer *pointer = 0;
				while(is_token(STAR))
				{
					pointer = new type_pointer();
					pointer->det_base_type(type);
					type = pointer;
				}
				expect(R_PARENTH);
				operand = parse_prefix_expr();
				return new cast(operand, type);
			}
		else return parse_postfix_expr(true);
	}
	if(is_token(SIZEOF))
	{
		if (is_token(L_PARENTH))
		{
			if (!is_one_of_type(CHAR, INT, FLOAT, 0)) error ("expected specifier");
			temp = get_token();
			expect(R_PARENTH);
			return new size(temp->get_type());
		}
		else operand = parse_prefix_expr();
		return new size(operand);
	}
	if(is_token(STAR))	
	{
		operand = parse_prefix_expr();
		return new inder(operand);
	}
	if(is_token(BIT_AND))	
	{
		operand = parse_prefix_expr();
		return new addr(operand);
	}
	if(is_one_of_type(INC, DEC, SUB_TOK, ADD_TOK, NEGATION, TILDE, 0))
	{
		get_token();
		operand = parse_prefix_expr();
		return new unary_expr(temp->get_type(), operand);
	}
	return parse_postfix_expr();
}


expr *parser::parse_binary_expr(int priority)
{
	expr *fst, *sec;
	switch (priority)
	{
		case MULTI_OP	: fst = parse_prefix_expr(); break;
		default			: fst = parse_binary_expr(priority-1);
	}
	t_type op = lookahead->get_type();
	while( priority == prior_op(op) )
	{
		get_token();
		switch (priority)
		{
			case MULTI_OP	: sec = parse_prefix_expr(); break;
			default			: sec = parse_binary_expr(priority-1);
		}
		if (priority <= ADD_OP) fst = new arithm_expr(op, fst, sec);
		else fst = new logic_expr(op, fst, sec);
		op = lookahead->get_type();

	}
	return fst;
}

expr *parser::parse_conditional_expr()
{
	expr *cond=parse_binary_expr(LOGIC_OR_OP);
	if (is_token(QUESTION))	 
	{
		expr *expr_1=parse_expr();
		expect(COLON);
		expr *expr_2=parse_conditional_expr();
		cond=new conditional_expr(cond, expr_1, expr_2);
	}
	return cond;
}

expr *parser::parse_assign_expr()
{
	expr *left = parse_conditional_expr();
	if	( prior_op(lookahead->get_type()) == ASSIGN_OP )	
	{
		t_type op = get_token()->get_type();
		expr *right = parse_assign_expr();
		switch(op)
		{
		case DIV_ASSIGN:		right = new arithm_expr(DIV, left, right); break;
		case MUL_ASSIGN:		right = new arithm_expr(STAR, left, right); break;
		case MOD_ASSIGN:		right = new arithm_expr(MOD, left, right); break;
		case ADD_ASSIGN:		right = new arithm_expr(ADD_TOK, left, right); break;
		case SUB_ASSIGN:		right = new arithm_expr(SUB_TOK, left, right); break;
		case XOR_ASSIGN:		right = new logic_expr(XOR_TOK, left, right); break;
		case SHIFT_L_ASSIGN:	right = new logic_expr(SHIFT_L, left, right); break;
		case SHIFT_R_ASSIGN:	right = new logic_expr(SHIFT_R, left, right); break;
		case OR_ASSIGN:			right = new logic_expr(OR_TOK, left, right); break;
		case AND_ASSIGN:		right = new logic_expr(AND_TOK, left, right); break;
		case ASSIGN:			left=new assign_expr(left, op, right);
		}
	}
	return left;
}

expr *parser::parse_expr()
{
	expr *fst = parse_assign_expr();
	while (is_token(COMMA))
		fst = new comma_expr(fst, parse_assign_expr());
	return fst;
}

expr_stmt *parser::parse_expr_stmt()
{
	if (is_token(SEMICOLON)) return new expr_stmt();
	expr_stmt *cur = new expr_stmt(parse_expr());
/*--------------------------------------------statement----------------------------------------*/
	expect(SEMICOLON);
	return cur;
}

stmt *parser::parse_stmt(bool so, bool co)
{

	expr *e;
	stmt *s, *s2;
	expr_stmt *es, *es2;
	list <expr*> *arg = new list <expr*>;
	switch(lookahead->get_type())
	{
	case IF:
		get_token();
		expect(L_PARENTH);
		e = parse_expr();
		expect(R_PARENTH);
		s = parse_stmt(so, co);
		if (is_one_of_type(ELSE, 0))
		{
			get_token();
			s2 = parse_stmt(so, co);
			return new if_stmt(e, s, s2);
		}
		return new if_stmt(e, s);
	case SWITCH:
		get_token();
		expect(L_PARENTH);
		e = parse_expr();
		expect(R_PARENTH);
		s = parse_stmt(true, co);
		return new switch_stmt(e, s);
	case WHILE:
		get_token();
		expect(L_PARENTH);
		e = parse_expr();
		expect(R_PARENTH);
		s = parse_stmt(so, true);
		return new while_stmt(e, s);
	case DO:
		get_token();
		s = parse_stmt(so, true);
		expect(WHILE);
		expect(L_PARENTH);
		e = parse_expr();
		expect(R_PARENTH);
		expect(SEMICOLON);
		return new do_stmt(s, e);
	case FOR:
		get_token();
		e = 0;

		expect(L_PARENTH);
		es = parse_expr_stmt();
		es2 = parse_expr_stmt();
		expect(R_PARENTH); {
			e=parse_expr();
			expect(R_PARENTH);
		}
		s = parse_stmt(so, true);
		if (e) return new for_stmt(es, es2, e, s);
		return new for_stmt(es, es2, s);
	case CONTINUE:;
		get_token();
		if (!co) error("continue without cycle");
		s = new continue_stmt();
		expect(SEMICOLON);
		return s;
	case BREAK:
		get_token();
		if (!co && !so) error("break without cycle or switch");
		s = new break_stmt();
		expect(SEMICOLON);
		return s;
	case RETURN:
		get_token();
		if (is_token(SEMICOLON))
		{
			if (returned->get_type() != VOID_TYPE)	error("function must return value");
			return new return_stmt();
		}
		e = parse_expr();
		if (*e->get_type() != *returned) error("mismatch type of returned value");
		s = new return_stmt(e);
		was_ret = true;
		expect(SEMICOLON);
		return s;
	case CASE:
		if (!so) error("case without switch");
		get_token();
		e = parse_conditional_expr();
		expect(COLON);
		return new case_stmt(e, parse_stmt());
	case DEFAULT:
		if (!so) error("default without switch");
		get_token();
		expect(COLON);
		return new default_stmt(parse_stmt());
	case L_BRACE:
		get_token();
		return parse_block(so, co);
	case PRINTF:
	case SCANF:
		get_token();
		expect(L_PARENTH);
		e = parse_assign_expr();
		if (*(e->get_type()) != *new type_pointer(new type_char()))
			error("expected pattern for io_operation");
		arg->push_back(e);
		while (is_token(COMMA))
		{
			e = parse_assign_expr(); 
			if (!is_expr_of_type(e, POINTER)) error("expected pointer for io_operation");
			arg->push_back(e);
		}
		expect(R_PARENTH);
		expect(SEMICOLON);
		return new io_stmt(arg);
	default:
		return parse_expr_stmt();
	}
}

compound_stmt *parser::parse_block(bool so, bool co, list <sym_var *> *arg_func)
{
	symtable *new_table = new symtable();
	new_table->prev = cur_table;
	cur_table = new_table;
	compound_stmt *block = 0;
	sym_type *spec = 0;
	if (arg_func)
			for (list <sym_var*>::iterator i=arg_func->begin(); i!=arg_func->end(); ++i)
				if (!cur_table->push_var(*i)) error("redefinition");
	block = new compound_stmt("statement", cur_table);
	while(!is_token(R_BRACE))
	{
		spec = det_spec();
		if (spec) 
		{
			if (spec->get_type() == STRUCTURE && is_token(SEMICOLON))			// struct def without decl
				continue;
			sym_var *s;
			do
			{
				s = parse_single_spec(spec);
				if (!s) continue;
				if (!cur_table->push_var(s)) error("redefinition");
				if (is_token(ASSIGN)) 
				{
					expr *init;
					switch(s->get_type()->get_type())
					{
					case CHAR_TYPE:
					case FLOAT_TYPE:
					case INT_TYPE:
					case POINTER:
						init = parse_assign_expr();
						block->add(new expr_stmt(new assign_expr(new var(s->get_name(), cur_table), ASSIGN, init)));
						break;
					case ARRAY:
						if (is_token(STRING)) error("string initializer is unavailable for char array"); // string init
						expect(L_BRACE);
						for(int i = 0; is_token(COMMA) || !i; i++)
						{
							init =  parse_assign_expr();
							block->add(new expr_stmt(new assign_expr(new index(new var(s->get_name(), cur_table), new const_int(i)), ASSIGN, init)));
							if (i >= dynamic_cast<type_array *>(s->get_type())->get_size())
								error("too many initializer");
						}
						expect(R_BRACE);
						break;
					default: error("unavailable initializer");
					}
				}
			} while(is_token(COMMA));
			expect(SEMICOLON);
		}
		else block->add(parse_stmt(so, co));
	}
	cur_table = cur_table->prev;
	return block;
}


/*--------------------------------------------declaration----------------------------------------*/
sym_type *parser::det_spec()
{

	switch (lookahead->get_type())
	{
	case INT:		{ get_token(); return new type_int(); }
	case CHAR:		{ get_token(); return new type_char(); }
	case FLOAT:		{ get_token(); return new type_float(); }
	case VOID:		{ get_token(); return new type_void(); }
	case STRUCT:	{ get_token(); return parse_struct(); }

	default: return 0;
	}
}

type_struct *parser::parse_struct()					// struct prototype is unavailable
{
	string name_struct;
	if (!is_one_of_type(IDENTIFIER, 0))
	{
		expect(L_BRACE);
		return new type_struct(parse_consist());
	}
	name_struct = get_token()->get_lexeme();
	if(is_token(L_BRACE))
	{
		if (!cur_table->push_struct(new type_struct(name_struct, parse_consist()))) error("redefinition structure"); 
		return cur_table->check_struct(name_struct);
	}
	for (symtable *lookahead_table = cur_table; lookahead_table ; lookahead_table = lookahead_table->prev)
		if (lookahead_table->check_struct(name_struct)) 
			return lookahead_table->check_struct(name_struct);
}

symtable *parser::parse_consist()
{
	sym_type *spec = 0;
	sym_var *s = 0;
	symtable *ret = new symtable();
	while(!is_token(R_BRACE))
	{
		spec = det_spec();
		if (spec) 
		{
			if (spec->get_type() == STRUCTURE && is_token(SEMICOLON))
				continue;
			do 
			{
				s = parse_single_spec(spec);
				if (s->get_type()->get_type() == FUNC) error("function is unavailable member of structure");
				if (!ret->push_var(s)) error("redefinition");
			}
			while(is_token(COMMA));
			expect(SEMICOLON);
		}
		else error("expected specifier member of structure");
	}
	return ret;
}

void parser::parse_global()				// global init
{
	bool is_func_def = false;
	sym_type *spec = det_spec();
	if (!spec) error("expected type");
	if (spec->get_type() == STRUCTURE && is_token(SEMICOLON)) // struct def wihtout declaration
		return;
	sym_var *new_sym = parse_single_spec(spec, &is_func_def);
	if (is_func_def) return;
	if (new_sym) if (!cur_table->push_var(new_sym))  error("redefinition");
	while(is_token(COMMA)) 
	{
		new_sym = parse_single_spec(spec);
		if (is_func_def) error("expected ';'");
		if (!new_sym) continue;
		if (!cur_table->push_var(new_sym)) error("redefinition");
	}
	if (is_token(ASSIGN))	error("global var initializing is unavailable");
	expect(SEMICOLON);
}

sym_var *parser::parse_single_spec(sym_type *spec, bool *is_func_def )
{
	derivative_type *head = 0, *last;
	string ident;
	last = parse_declarator(head, ident);
	if (!head) return new sym_var(spec, ident);
	last->det_base_type(spec);
	if (head->get_type() == FUNC)
	{
		type_func *head_func = dynamic_cast <type_func *>(head);
		if (cur_table->check_var(ident))
		{
			if (cur_table->check_var(ident)->get_type()->get_type() != FUNC) error("redefinition");
				if (!arg_comp(dynamic_cast <type_func *>(cur_table->check_var(ident)->get_type())->get_arg(),
					head_func->get_arg()))
						error("overloading function is unavailable");
		}
		else cur_table->push_var(new sym_var(head, ident));
		if (is_token(L_BRACE))
		{
			if (!is_func_def) error("local function definition");
			returned = head_func->get_base_type(), was_ret = false;
			if (!dynamic_cast <type_func *>(cur_table->check_var(ident)->get_type())->def(parse_block(false, false, head_func->get_arg())))
				error ("redefinition");
			if (returned->get_type() != VOID_TYPE && !was_ret) error ("function must returned value");
			*is_func_def = true;
			if (ident == "main") main_detected = true;
		}
		return 0;
	}
	return new sym_var(head, ident); 
}

derivative_type *parser::parse_declarator(derivative_type *&head, string &ident)
{
	derivative_type *last = 0, *temp = 0;
	int pointer = 0;
	while (is_token(STAR))
		pointer++;
	if (is_token(L_PARENTH))
	{
		last = parse_declarator(head, ident);
		expect(R_PARENTH);
	}
	else if (is_one_of_type(IDENTIFIER, 0))
		ident = get_token()->get_lexeme();
	else error("expected identifier");
	if (is_token(L_PARENTH))
	{
		if (last && last->get_type() == FUNC) error ("function return function");
		if (last && last->get_type() == ARRAY) error ("massive of function");
		if (is_token(R_PARENTH))
		{

			temp = new type_func();
			if (head) last->det_base_type(temp);
			else head = temp;
			last = temp;

		}
		else 
		{
			temp = new type_func(parse_arg());
			if (head) last->det_base_type(temp);
			else head = temp;
			last = temp;
			expect(R_PARENTH);
		}
	}
	while (is_token(L_BRACKET))
	{
		if (last && last->get_type() == FUNC) error ("function return massive");
		if (!is_one_of_type(INT_CONST, 0)) error("expected size of massive(int const)");
		int size = dynamic_cast<token_v<int> *>(get_token())->get_value();
		temp = new type_array(size);
		if (head) last->det_base_type(temp);
		else head = temp;
		last = temp;
		expect(R_BRACKET);
	}
	while (pointer)
	{
		temp = new type_pointer();
		if (head) last->det_base_type(temp);
		else head = temp;
		last = temp;
		pointer --;
	}
	return last;
}


list <sym_var *> * parser::parse_arg()
{	
	list <sym_var *> * ret = new list <sym_var *>;
	sym_type *spec = 0;
	do
	{
		spec = det_spec();
		if (!spec) error("expected type of argument");
		ret->push_back(parse_single_spec(spec));
	}
	while(is_token(COMMA));
	return ret;
}



global_symtable *parser::get_global_table()
{
	return global_table;
}

parser::parser(scanner *sc): scan(sc), main_detected(false)
{
	get_token();
	if (is_token(EOF_TYPE)) error("some more code plz");
	cur_table = global_table = new global_symtable();
	try
	{
		while (!is_one_of_type(EOF_TYPE, 0))
			parse_global();
	} catch(string msg){ error(msg); }
	if (!main_detected) error("main function is undefenition");
}