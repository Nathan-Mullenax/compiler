#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <string>


class token {
	private:
		friend class scanner;
		std::string type,lexem;
		int column;
		int line;
	public:
		void display();
		token();
		std::string get_type(){return type;} 
};

#endif