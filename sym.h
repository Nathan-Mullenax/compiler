#ifndef SYM_H
#define SYM_H
#include <map>

class sym
{
protected:
	string name;
public:
	sym(string n): name(n) {}
	string get_name() { return name; }
	virtual void output() { cout << name << " "; }
};

//------------------------------TABLE-------------------------------//

class symtable
{
private:
	map<string, sym *> var_table;
	map<string, sym *> struct_table;
public:
	symtable *prev;
	symtable(){};
	void push_var(sym *new_sym)
	{
		var_table[new_sym->get_name()] = new_sym;
	}
	void push_struct(sym *new_sym)
	{
		struct_table[new_sym->get_name()] = new_sym;
	}
	void prev_table(symtable *external)
	{
		prev = external;
	}
	sym *check_var(string ident)
	{
		if (var_table[ident]) 	return var_table[ident];
		else return 0;
	}
	sym *check_struct(string ident)
	{
		if (struct_table[ident]) 	return struct_table[ident];
		else return 0;
	}
	void output()
	{
		map<string, sym *>::iterator i;
		for (i = var_table.begin(); i != var_table.end(); i++)
			 i->second->output();
		for (i = struct_table.begin(); i != struct_table.end(); i++)
			 i->second->output();
	}
};


//---------------------------------TYPE--------------------------------//
enum type {BASE, FUNC, ARRAY, STRUCTURE, POINTER};
class sym_type: public sym
{
public:
	virtual type get_type() {return BASE;}
	sym_type(string n): sym(n) {}
};
//------------------BASE--------------//
class type_int: public sym_type
{
public:
	type_int(): sym_type("int") {}
};

class type_char: public sym_type
{
public:
	type_char(): sym_type("char") {}
};

class type_float: public sym_type
{
public:
	type_float(): sym_type("float") {}
};

class type_void: public sym_type
{
public:
	type_void(): sym_type("void") {}
};

class type_struct: public sym_type
{
	symtable *cons;
public:
	type get_type(){return STRUCTURE;}
	type_struct(string name, symtable *consist): sym_type("struct_" + name), cons(consist) {}
	type_struct(symtable *consist): sym_type("struct"), cons(consist) {}
};
//-------------------DERIVATIVE--------------------//

class derivative_type: public sym_type
{
protected:
	sym_type *base_type;
public:
	derivative_type(string n): sym_type(n) {}
	void det_base_type(sym_type *base)
	{
		base_type = base;
	}

	void output()
	{
		sym::output();
		base_type->output();
	}
};

class type_array: public derivative_type
{
private:
	expr *_size;
public:
	type get_type(){return ARRAY;}
	type_array(): derivative_type("array") {}
	type_array(expr *size): derivative_type("array"), _size(size) {}
};

class type_pointer: public derivative_type
{

public:
	type get_type(){return POINTER;}
	type_pointer(): derivative_type("pointer") {}

};

class type_func: public derivative_type
{
	symtable *arg;
	symtable *local;
	compound_stmt *func_statements;
public:
	type_func(): derivative_type("func") {}
	type_func(symtable *arguments): derivative_type("func"), arg(arguments) {}
	type get_type() {return FUNC;}
	void def(compound_stmt *statements, symtable *local_var) { func_statements = statements; local = local_var; }
};
//-------------------------------VARAIBLE----------------------------------//

class sym_var: public sym
{
private:
	sym_type *type_var;
public:
	sym_var(sym_type *type, string n): sym(n), type_var(type) {}
	void output()
	{
		type_var->output();
		cout << name << endl;
	}
	type get_type()
	{
		return type_var->get_type();
	}
};

class var_const: public sym_var
{
public:
	var_const(sym_type *type, string n): sym_var(type, n) {}
};

class var_arg: public sym_var
{
public:
	var_arg(sym_type *type, string n): sym_var(type, n) {}
};

class var_local: public sym_var
{
public:
	var_local(sym_type *type, string n): sym_var(type, n) {}
};

class var_global: public sym_var
{
public:
	var_global(sym_type *type, string n): sym_var(type, n) {}
};

#endif


