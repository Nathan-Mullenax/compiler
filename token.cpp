#include "token.h"


token::token() {}

void token::display()
{
	std::cout << "token_type:" << type << " lexem:" << lexem << " line:"  << line << " column:" << column << std::endl;
	return ;
}