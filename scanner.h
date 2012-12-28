#ifndef SCANNER_H
#define SCANNER_H


#include <fstream>
#include <string>
#include <ctype.h>
#include <map>
#include <set>
#include "excep.h"
#include "token.h"

static const int BUFFER_SIZE = 10;

class scanner
{
	private:
		std::map <char, char> escape_sequences;
		std::map <std::string, t_type> ops;
		std::map <std::string, t_type> keywords;

		std::ifstream &input;
		std::string lexeme;
		t_type type;
		char buffer[BUFFER_SIZE*2],*second_buf,*begining,*forward;
		int line,column,eof,begin_line,begin_column;
		bool iseof;

		inline void error(std::string msg);
		inline token *set_token();	
		void move();
		void read();
		char look_next();
		
		void skip_nonprod();
		bool skip_comment();
		void read_single_type(int (*type)(int));

		token* read_ident();
		token* read_num();
		token* read_float();
		token* read_char();
		token* read_string();	
		token* read_ops();

	public:
		scanner(std::ifstream &in);
		token *get_token();

};

#endif