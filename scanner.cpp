#include "scanner.h"

static int isoct(int oct){ return oct >= '0' && oct <= '7'; }
static int isidentsym(int sym){ return isalnum((unsigned char)sym) || sym == '_'; };

inline void scanner::error(std::string msg, std::string type="error:") { throw excep(type+msg, column, line); }
inline token* scanner::set_token() {return new token (type,  lexeme, begin_line, begin_column);}

void scanner::move()
{
	if (iseof) return;
	switch (*forward)
		{
			case ' ' :	column++; break;
			case '\n' :	line++, column = 1; break;
			case '\t' :	column = 1 + (column-1)/4*4; break;
			default	:	column++; break;
		}
	if (++forward-buffer == BUFFER_SIZE * 2)
	{
		std::strncpy(buffer,second_buf, BUFFER_SIZE);
		input.read(second_buf, BUFFER_SIZE);
		if (input.gcount() != BUFFER_SIZE)
		{
			second_buf[input.gcount()] = EOF;
			eof = input.gcount();
		}
		forward = second_buf,	begining -= BUFFER_SIZE;
	}
	if (!iseof)	iseof = (forward - second_buf) >= eof;
}

char scanner::look_next()
{
	int l=line, c=column;
	move();
	char next = *forward;
	forward--, line=l, column=c;
	return next;
}

void scanner::read() {lexeme += *forward; move();}

void scanner::read_single_type(int (*istype)(int))
{
	do	read();
	while(istype((unsigned char)*forward));	
}

token* scanner::read_ident()
{
	read_single_type(isidentsym);
	if (keywords.find(lexeme) != keywords.end()) { type = keywords[lexeme]; }
	else { type = IDENTIFIER; }
	return set_token();
}

token* scanner::read_num()
{
	type = INT;
	int v;
	if (*forward == '0')
	{
		read();
		if (*forward == 'x') 
		{ 
			read();
			if (!isxdigit((unsigned char)*forward)) error("need at least one hex digit");
			read_single_type(isxdigit);						
			if (!isspace((unsigned char)*forward) && !ispunct((unsigned char)*forward) && !iseof)	error("invalid number");
			sscanf_s(lexeme.c_str(), "%x", &v);
			return new token_v <int> (v, type,  lexeme, begin_line, begin_column);
		}
		if (isoct(*forward))
		{ 
			read_single_type(isoct);
			sscanf_s(lexeme.c_str(), "%o", &v);
			if (!isspace((unsigned char)*forward) && !ispunct((unsigned char)*forward) && !iseof)	error("invalid number"); 
			return new token_v <int> (v, type,  lexeme, begin_line, begin_column);
		}
	}
	
	else if (isdigit((unsigned char)*forward)) read_single_type(isdigit);
	sscanf_s(lexeme.c_str(), "%d", &v);
	if (*forward == '.' || *forward == 'e' || *forward == 'E') return read_float();
	if (!isspace((unsigned char)*forward) && !ispunct((unsigned char)*forward) && !iseof) error("invalid number");
	return new token_v <int> (v, type,  lexeme, begin_line, begin_column);
}

token *scanner::read_float()
{
	type = FLOAT;
	float v;
	if (*forward == '.')	
	{
		read();
		if (isdigit((unsigned char)*forward)) 
			read_single_type(isdigit);
	}
	if (*forward == 'e' || *forward == 'E')
	{ 
		read();
		if (*forward == '+' || *forward == '-')  read();
		if (isxdigit((unsigned char)*forward))	read_single_type(isxdigit);
		else error("need at least one digit after e");
	}
	sscanf_s(lexeme.c_str(), "%f", &v);
	if (!isspace((unsigned char)*forward) && !ispunct((unsigned char)*forward) && !iseof) error("invalid number");
	return new token_v <float> (v, type,  lexeme, begin_line, begin_column);
}

token* scanner::read_char()
{
	int v;
	type=CHAR;
	read();
	if (*forward == '\\')
	{
		read();
		if(escape_sequences.find(*forward) == escape_sequences.end()) error("unknown escape sequence");
		v = escape_sequences[*forward];
		read();					
	}
	else
	{
		if (*forward == '\'')	error("empty char const");
		if (*forward == '\n')	error("newline in const char");
		if (iseof) error("EOF in const char");
		v = *forward;
		read();
	}
	if (*forward!='\'') error("missing \"'\" ");
	read();
	return new token_v <int>(v, type,  lexeme, begin_line, begin_column);
}


token* scanner::read_string()
{
	std::string v;
	type = STRING;
	read();
	while (*forward!='"')
	{
		if (iseof) error("EOF in string");
		if (*forward == '\n') error("newline in string");
		if (*forward == '\\')
		{
			read();
			if (escape_sequences.find(*forward) == escape_sequences.end())
				error("unknow escape_sequences in string");
			v += escape_sequences[*forward];
			read();
		} 
		else { v += *forward; read(); }
	}
	read();
	return new token_v<std::string>(v, type,  lexeme, begin_line, begin_column);
}

token* scanner::read_ops()
{
	do read();
	while (ops.find(lexeme+*(char *)forward)!=ops.end());
	type = ops[lexeme];
	return set_token();
}

void scanner::skip_nonprod()
{
	while (isspace((unsigned char)*forward)) move();
	while (skip_comment())
		while (isspace((unsigned char)*forward)) move();
}

bool scanner::skip_comment()
{
	begining=forward;
	begin_line=line, begin_column=column;
	if (  *forward ==  '/' && look_next() == '*')	
	{
		move(); move();
		do
		{
			if (iseof) error("EOF in comment");
			move();
		} while (*forward != '*' || look_next() != '/');
		move(); move();
		return true;
	}
	else if (*forward == '/' && look_next() == '/' )	
	{
		do
		{
			if (iseof)	return true;
			move();
		} while (*forward != '\n');
		move();
		return true;
	}
	else return false;
}

token* scanner::get_token()
{
	skip_nonprod();
	begin_line = line,	begin_column = column,	begining = forward, lexeme = "";

	if (iseof) return new token(EOF_TYPE, lexeme, begin_line, begin_column);
	if (isalpha( (unsigned char)*forward) || *forward == '_')	return read_ident();	
	if (isdigit( (unsigned char)*forward) || *forward == '.' && isdigit((unsigned char)look_next()) )	return read_num();
	if (*forward == '\'') return read_char();	
	if (*forward == '"') return read_string();				
	std::string a;a+=*forward; if (ops.find(a)!=ops.end()) return read_ops();				
	error("unexpected symbol");
}



scanner::scanner(std::ifstream &in):
input(in), forward(buffer + BUFFER_SIZE),line(1), column(1), 
second_buf(buffer + BUFFER_SIZE),eof(BUFFER_SIZE+1), iseof(false)

{

	input.read((char *)second_buf, BUFFER_SIZE);
		if(input.gcount()!=BUFFER_SIZE)
	{
		second_buf[input.gcount()]=EOF;
		eof=input.gcount();
	}

#define RESERVED_EXPR
#include "token_type.h"
#undef RESERVED_EXPR
}