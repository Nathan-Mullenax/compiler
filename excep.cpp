#include "excep.h"


excep::excep(std::string mas, int col, int l): column(col), line(l), massage(mas) 
{
}

void excep::display()
{
	std::cout << "error:" << massage << " at line:" << line << " column:" <<  column << std::endl;
	return;
}