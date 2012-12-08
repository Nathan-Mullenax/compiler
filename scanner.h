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
		std::map <char, std::string> escape_sequences;
		std::set <std::string> ops;
		std::set <std::string> keywords;

		std::ifstream &input;
		std::string lexem,type;
		unsigned char buffer[BUFFER_SIZE*2],*half_buf,*begining,*forward;
		int line,column,eof;
		token current;
		bool iseof;

		void read_single_type(int (*func)(int));
		void move_forward();
		void read_forward();
		void back_forward();
		void read_exp();
		void read_space();
		void read_ident();
		void read_num();
		bool read_comment();
		void read_nonprod();
		void read_char();
		void read_string();	
		void read_ops();
		

	public:
		scanner(std::ifstream &in);
		token get_token();

};

#endif