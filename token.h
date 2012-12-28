#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <string>

enum t_type
{
#define TOKEN(name) name
#include "token_type.h"
#undef TOKEN
};

extern const char *type_str[];

using namespace std;

class token {
	protected:
		std::string lexeme;
		t_type type;
		int column;
		int line;
	public:
		virtual void display()
		{	
			cout.width(16);cout.setf(ios:: left);
			cout << "" << "type:";
			cout.width(15);cout.setf(ios:: left);
			cout << type_str[type] << " lexeme:";
			cout.width(10);cout.setf(ios:: left );
			cout << lexeme << " ("  << line << "," << column <<") " << endl;
			return ;
		}
		token(t_type t, std::string lex, int l, int c): type(t), lexeme(lex), line(l), column(c) {}
		std::string get_lexeme()		{return lexeme;} 
		t_type get_type() { return type; }
		int get_line() { return line; }
		int get_column() { return column; }
};

template <class v_type>
class token_v: public token 
{
private:
	v_type value;
public:
	token_v(v_type v, t_type t, std::string lex, int l, int c): token(t, lex, l, c), value(v) {}
	virtual void display()
		{
			cout << "value:";
			cout.width(10);cout.setf(ios:: left);
			cout << value << "type:";
			cout.width(15);cout.setf(ios:: left);
			cout << type_str[type] << " lexeme:";
			cout.width(10);cout.setf(ios:: left );
			cout << lexeme << " ("  << line << "," << column <<") " << endl;
			return ;
		}
	v_type get_value() { return value; }

};
#endif