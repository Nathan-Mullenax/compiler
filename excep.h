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
		excep(std::string mas, int col, int l): column(col), line(l), massage(mas) {}
		void display()
		{
			std::cout << massage << " at line:" << line << " column:" <<  column << std::endl;
			return;
		}


};

#endif