#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <iostream>
#include <string>


class excep 
{
	private:
		std::string massage;
		int line,column;
	public:
		void display();
		excep(std::string mas, int col, int l);
};

#endif