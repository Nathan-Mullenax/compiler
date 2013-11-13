#ifndef SYM_H
#define SYM_H
#include <map>
#include <list>

class expr;
class compound_stmt;

class sym
{
protected:
	string name;
public:
	sym(string n): name(n) {}
	string get_name() { return name; }
	virtual void output(int depth = 0); 
};

//------------------------------TABLE-------------------------------//
class sym_var;
class type_struct;

class symtable
{
protected:
	map<string, sym_var *> var_table;
	map<string, type_struct *> struct_table;

public:
	symtable *prev;
	symtable(): prev(0) {}
	bool push_var(sym_var *new_sym);
	bool push_struct(type_struct *new_sym);
	void prev_table(symtable *external)		{prev = external;}
	sym_var *check_var(string ident)		{return var_table[ident];}
	type_struct *check_struct(string ident)	{return struct_table["struct " + ident];}
	void output(int depth = 0);
	void set_offsets(int &base);
	int set_offsets_struct();
};

class global_symtable: public symtable
{
public:
	void gen(map<std::string, int> *global_var, map<std::string, list<asm_cmd *> *> *func_code);
};

//---------------------------------TYPE--------------------------------//
enum type {VOID_TYPE = 1, CHAR_TYPE, INT_TYPE, FLOAT_TYPE, FUNC, ARRAY, STRUCTURE, POINTER};
class sym_type: public sym
{
protected:
	type t;
public:
	sym_type(string n): sym(n){}
	type get_type(){return t;}
	virtual bool operator !=(sym_type &s)	
	{
		return t != s.get_type();
	}
	bool is_der_type() {return t>FLOAT_TYPE;}
	virtual int get_size() {return 4;}
};
//------------------BASE--------------//
class type_int: public sym_type
{
public:
	type_int(): sym_type("int") {t = INT_TYPE;}
};

class type_char: public sym_type
{
public:
	type_char(): sym_type("char") {t = CHAR_TYPE;}
};

class type_float: public sym_type
{
public:
	type_float(): sym_type("float") {t =FLOAT_TYPE;}
};

class type_void: public sym_type
{
public:
	type_void(): sym_type("void") {t = VOID_TYPE;}
};

class type_struct: public sym_type
{
	symtable *cons;
	int size;
public:
	void output(int depth = 0);
	void output_def(int depth = 0);
	type_struct(string name, symtable *consist): sym_type("struct " + name), cons(consist), size(0){t = STRUCTURE;}
	type_struct(symtable *consist): sym_type("struct"), cons(consist), size(0) {t = STRUCTURE;}
	symtable *get_cons() {return cons;}
	int get_size() 
	{
		if(!size)	 size = cons->set_offsets_struct();
		return size;
	}
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
		if (t == ARRAY && base->get_type() == VOID_TYPE)   error("massive of void is unavailable");
		base_type = base;
	}

	sym_type * get_base_type()
	{
		return base_type;
	}
	void output(int depth = 0);
	bool operator !=(sym_type &s) 
	{
		sym_type *sec = &s;
		if (!sec->is_der_type()) return true;
		derivative_type *i = this, *j = dynamic_cast <derivative_type *>(sec);
		for (; i->get_base_type()->is_der_type()&& j->get_base_type()->is_der_type() ;
			i = dynamic_cast <derivative_type *>(i->get_base_type()), 
			j = dynamic_cast <derivative_type *>(j->get_base_type()))
			if (i->get_type() != j->get_type())
				if ( (i->get_type() != POINTER || j->get_type() != ARRAY)
				&& (i->get_type() != ARRAY || j->get_type() != POINTER) ) return true;
		if (i->get_base_type()->get_type() != j->get_base_type()->get_type()) return true;
		return false;
	}

};

class type_array: public derivative_type
{
private:
	int size;
public:
	type_array(int _size): derivative_type("array "), size(_size) 
	{
		char n[10];
		name += itoa(size, n, 10);
		t = ARRAY;
	}
	int get_size() {return base_type->get_size()*size;}
};

class type_pointer: public derivative_type
{

public:
	type_pointer(): derivative_type("pointer") {t = POINTER;}
	type_pointer(sym_type *base): derivative_type("pointer") {t = POINTER; base_type = base;}
};


class type_func: public derivative_type
{
	list <sym_var *> *arg;
	compound_stmt *definition;
public:
	type_func(): derivative_type("func"), definition(0), arg(0) {t = FUNC;}
	type_func(list <sym_var *> * arguments): derivative_type("func"), arg(arguments), definition(0) {t = FUNC;}
	list <sym_var *> * get_arg() { return arg; }
	bool def(compound_stmt *d) 
	{
		if (definition) return false;
		definition = d;
		return true;
	}
	void output(int depth = 0);
	void gen();

};
//-------------------------------VARAIBLE----------------------------------//

class sym_var: public sym
{
private:
	sym_type *type_var;
	int offset;
	bool global;
public:
	sym_var(sym_type *type, string n): sym(n), type_var(type), offset(0)
	{
		if (type->get_type() == VOID_TYPE) error("decloration of void variable is unavailable");
	}
	void output(int depth = 0);
	sym_type *get_type()
	{
		return type_var;
	}
	void set_offset(int _offset, bool _global = false)
	{
		global = _global;
		offset = _offset;
	}
	int get_offset()
	{
		return offset;
	}
	bool is_global()
	{
		return global;
	}

};


#endif


