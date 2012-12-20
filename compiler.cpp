#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "scanner.h"
#include <stdio.h>

using namespace std;

int main(int argc, char* argv[])
{ 
	cout <<__rdtsc();
//	int a=isdigit(('я'));									// istype не работает с signed char!
//	ifstream input( "c:/compiler/Debug/lex_tests/6.txt");
//	try{
//		scanner scan(input);
//			token *current;
//			while (current=scan.get_token(),current->get_type()!=EOF_TYPE) current->display();
////		parser pars(&scan);
////		pars.output();
//		}
//
//	catch(excep error) {error.display();}
//return 0;
//}

	if (argc != 3 || argv[1][0]!= 'l' &&  argv[1][0]!='p' ) {
		cout << "usage: filename p(for parser)/l(for lexer)" << endl
			<<"Kim Anton C8303"<< endl;
		return 1;
	}
	ifstream input(argv[2]);
	if (!input) {cout <<"cannot open file" << argv[2] << endl; return 1;}
	try{
		scanner scan(input);

		if( argv[1][0] == 'p')	
		{
	//		parser pars(&scan);
	//		pars.output();
			return 0;
		}
		if( argv[1][0] == 'l')
		{
			token *current;
			while (current=scan.get_token(),current->get_type()!=EOF_TYPE) current->display();
			return 0;
		}
	}
	catch(excep error) {error.display();}
	return 0;
}

