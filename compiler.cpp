//
//


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "scanner.h"

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 2) {cout << "usage: [filename]" << endl; return 1;}
	ifstream input(argv[1]);
	if (!input) {cout << "cannot open file: " << argv[1] << endl; return 1;}
	scanner scan(input);
	token current;
	try	{while (current=scan.get_token(),current.get_type()!="EOF") current.display();}
	catch(excep error) {error.display();}
	return 0;
}

