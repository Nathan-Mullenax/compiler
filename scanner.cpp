
#include "scanner.h"


int islatter(int latter){	return (isalnum(latter) || latter=='_');}
int isoct(int oct){	return ((char)oct>='0' && (char)oct<='7');}

void scanner::move_forward()
{
	if (iseof)											return;
	switch (*forward)
		{
			case ' ':	column++;						break;
			case '\n':	line++,column=1;				break;
			case '\t':	column=1+(column-1)/4*4;		break;
			default	:	column++;						break;
		}
	if ( ++forward == buffer+BUFFER_SIZE*2 )
	{
		char *temp=(char *)buffer;
		std::strncpy(temp,(char *)half_buf,BUFFER_SIZE);
		input.read((char *)half_buf, BUFFER_SIZE);
		if(input.gcount()!=BUFFER_SIZE)
		{
			half_buf[input.gcount()]=EOF;
			eof=input.gcount();
		}
		forward=half_buf,	begining-=BUFFER_SIZE;
	}
	if (!iseof)	iseof=(forward-half_buf)>=eof;
}



void scanner::back_forward()
{
	current.lexem="";
	forward=begining;
	line=current.line;
	column=current.column;
	iseof=forward-half_buf>=eof;
}

void scanner::read_forward()	{current.lexem+=*forward; move_forward();}


void scanner::read_single_type(int (*func)(int))
{
	do	read_forward();
	while(func(*forward));	
}

bool scanner::read_comment()
{

	begining=forward;
	current.line=line, current.column=column;
	move_forward();
	bool flag=false;
	if (*begining =='/' && *forward=='*' )	
	{
		
		flag=true;
		do
		{
			if (iseof)				throw excep("EOF in comment",column,line);
			move_forward();
		}while (*forward!='/' || *(forward-1)!='*');
		move_forward();
	}

	else if (*forward=='/' && *begining =='/' )	
	{
		flag=true;
		do
		{
			if (iseof)	return true;
			move_forward();
		}while (*forward!='\n');
		move_forward();
	}
	else back_forward();
	return flag;
}

void scanner::read_ident(){
		read_single_type(islatter);
		if (keywords.find(current.lexem)!=keywords.end()) current.type=current.lexem;
		else current.type="IDENTIFIER";
	}
void scanner::read_exp()
{
	if(*forward=='+' || *forward=='-') { read_forward();}
	if(isdigit(*forward))	while(isdigit(*forward))	{read_forward();}
	else throw excep("const",column,line);
}
void scanner::read_num()
{
	if (*forward=='0')
	{
		read_forward();
		if (*forward=='x') 
		{ 
			read_forward();
			if (!isxdigit(*forward))								throw excep("const number",column,line);
			current.type="HEX_LITERAL";
			read_single_type(isxdigit);						
			if (!isspace(*forward) && !ispunct(*forward) && !iseof)	throw excep("const number",column,line);
			return;
		}
		else if (isoct(*forward))
		{ 
			read_single_type(isoct);
			current.type="OCT_LITERAL";
			if (!isspace(*forward) && !ispunct(*forward) && !iseof)	throw excep("const number",column,line); 
			return;
		}
	}
	else if (isdigit(*forward)){ read_single_type(isdigit); current.type="DEC_LITERAL";}

	if (*forward=='.')	{;read_single_type(isdigit); current.type="FLOAT";}
	if (*forward=='e' || *forward=='E') {read_forward(); read_exp(); current.type="FLOAT";}
	if (!isspace(*forward) && !ispunct(*forward) && !iseof)			throw excep("const number",column,line);
		
}

void scanner::read_char()
{
	current.type="CHAR";
	read_forward();
	if (*forward=='\\')
	{
		read_forward();
		if(escape_sequences.find(*forward)==escape_sequences.end())		throw excep("unknown escape sequence",column,line);
		read_forward();					
	}
	else
	{
		if (*forward=='\'')		throw excep("empty char const",column,line);
		if (*forward=='\n')		throw excep("newline in const char",column,line);
		if (iseof)				throw excep("EOF in const char",column,line);
		read_forward();
	}
	if (*forward!='\'')			throw excep("missing \"'\" ",column,line);
	read_forward();
}


void scanner::read_string()
{
	current.type="STRING";
	read_forward();
	while (*forward!='"')
	{
		if (iseof)				throw excep("EOF in string",column,line);
		if (*forward=='\n')		throw excep("newline in string",column,line);
		if (*forward=='\\')
		{
			read_forward();
			if (escape_sequences.find(*forward) == escape_sequences.end())
								throw excep("unknow escape_sequences in string",column,line);
			read_forward();
		} 
		else read_forward();
	}
	read_forward();

}

void scanner::read_ops()
{
	do
		read_forward();
	while(ops.find(current.lexem+*(char *)forward)!=ops.end());
	current.type=current.lexem;
}

void scanner::read_nonprod()
{
	while (isspace(*forward)) move_forward();
	bool comment_reading;
	do
	{
		comment_reading=read_comment();
		while (isspace(*forward)) move_forward();
	}while ( comment_reading );
}

token scanner::get_token()
{
	read_nonprod();
	current.line=line,	current.column=column,	begining=forward;

	if (iseof)															{current.type="EOF";return current;}
	if (isalpha(*forward) || *forward=='_')								{read_ident();return current;}
	if (isdigit(*forward))												{read_num();return current;}
	if (*forward=='.' && isdigit((move_forward(),*forward)) )			{back_forward(); read_num();return current;}  
	back_forward();
	if (*forward=='\'')													{read_char();return current;}	
	if (*forward=='"')													{read_string();return current;}
	std::string a;a+=*forward; if (ops.find(a)!=ops.end())				{read_ops();return current;}

	throw excep("unexpected symbol",column,line);
}

scanner::scanner(std::ifstream &in):
input(in), forward(buffer + BUFFER_SIZE),line(1), column(1), half_buf(buffer + BUFFER_SIZE), eof(BUFFER_SIZE+1), iseof(false)

{
	input.read((char *)half_buf, BUFFER_SIZE);
		if(input.gcount()!=BUFFER_SIZE)
	{
		half_buf[input.gcount()]=EOF;
		eof=input.gcount();
	}
#include "reserved_word.h"

}