//#include <iostream>
//#include <fstream>
//#include <string>
//#include <vector>
//#include "generator.h"
//#include <stdio.h>
//
//using namespace std;
//
//int main(int argc, char* argv[])
//{ 
//	if (argc != 3 || argv[1][0]!= 'l' &&  argv[1][0]!='p' ) {
//		cout << "usage: filename p(for parser)/l(for lexer)/g(for generator)" << endl
//			<<"Kim Anton C8303"<< endl;
//		return 1;
//	}
//	ifstream input(argv[2]);
//	if (!input) {cout <<"cannot open file :" << argv[2] << endl; return 1;}
//	try
//	{
//		scanner scan(input);
//		parser pars(&scan);
//		switch (argv[1][0])
//		{
//		case'p': 
//			pars.output();
//			return 0;
//		case'l': 
//			token *current;
//			while (current=scan.get_token(),current->get_type()!=EOF_TYPE) current->display();
//			return 0;
//		case'g':
//			generator gen(&pars);
//		}
//	}catch(excep error) {error.display();}
//	return 0;
//}



#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "generator.h"
#include <stdio.h>

using namespace std;

int main(int argc, char* argv[])
{ 
	ifstream input( "c:/compiler/Debug/gen_tests/01.txt");
	try{
		scanner scan(input);
	//		token *current;
	//		while (current=scan.get_token(),current->get_type()!=EOF_TYPE) current->display();
	parser pars(&scan);
	//pars.output();
	generator gen(&pars);
	gen.print();
		}
	catch(excep error) {error.display();}
return 0;
}