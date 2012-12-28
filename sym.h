#ifndef SYM_H
#define SYM_H

using namespace std;

class sym
{
protected:
	string name;
public:
	sym(string n): name(n) {}
	string get_name() { return name; }
};

//------------------------------TABLE-------------------------------//

class symtable
{
private:
	map<string, sym *> table;
	symtable *prev;
public:
	symtable(){};
	void push(sym *new_sym)
	{
		table[new_sym->get_name()] = new_sym;
	}
	void prev_table(symtable *external)
	{
		prev = external;
	}
	sym *operator [](string ident)
	{
		if (table[ident]) 	return table[ident];
		else return 0;
	}
};


//---------------------------------TYPE--------------------------------//
class sym_type: public sym
{
public:
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
	type_struct(string name, symtable *consist): sym_type("struct" + name), cons(consist) {}
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
	void get_base_type(sym_type *base)
	{
		base_type = base;
	}
};

class type_array: public derivative_type
{
private:
	expr *_size;
public:
	type_array(): derivative_type("array") {}
	type_array(expr *size): derivative_type("array"), _size(size) {}
};

class type_pointer: public derivative_type
{

public:
	type_pointer(): derivative_type("pointer") {}

};

class type_func: public derivative_type
{
	symtable *arg;

public:
	type_func(): derivative_type("func") {}
	type_func(symtable *arguments): derivative_type("func"), arg(arguments) {}

};
//-------------------------------VARAIBLE----------------------------------//

class sym_var: public sym
{
private:
	sym_type *type_var;
public:
	sym_var(sym_type *type, string n): sym(n), type_var(type) {}
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


