#ifdef TOKEN

TOKEN(NULL_TYPE),

TOKEN(INT_CONST),TOKEN(FLOAT_CONST),TOKEN(CHAR_CONST),TOKEN(STRING),

TOKEN(INT),TOKEN(FLOAT),TOKEN(CHAR),TOKEN(IDENTIFIER),TOKEN(EOF_TYPE),
TOKEN(BREAK),TOKEN(CASE),TOKEN(CONST),TOKEN(CONTINUE),TOKEN(DEFAULT),TOKEN(DO),TOKEN(ELSE),
TOKEN(ENUM),TOKEN(EXTERN),TOKEN(FOR),TOKEN(GOTO),TOKEN(IF),TOKEN(REGISTER),TOKEN(RETURN),
TOKEN(SIGNED),TOKEN(SIZEOF),TOKEN(STATIC),TOKEN(STRUCT),TOKEN(SWITCH),
TOKEN(TYPEDEF),TOKEN(UNION),TOKEN(UNSIGNED),TOKEN(VOID),TOKEN(WHILE),
	
TOKEN(COMMA),TOKEN(DOT),TOKEN(TILDE),TOKEN(QUESTION),TOKEN(COLON),TOKEN(SEMICOLON),TOKEN(R_BRACKET),
TOKEN(L_BRACKET),TOKEN(L_PARENTH),TOKEN(R_PARENTH),TOKEN(R_BRACE),TOKEN(L_BRACE),
TOKEN(STAR),TOKEN(DIV),TOKEN(MOD),TOKEN(NEGATION),TOKEN(ASSIGN),TOKEN(XOR),TOKEN(BIT_OR),TOKEN(BIT_AND),
TOKEN(ADD),TOKEN(SUB),TOKEN(GREATER),TOKEN(LESS),TOKEN(SHIFT_L),TOKEN(SHIFT_R),TOKEN(ADD_ASSIGN),
TOKEN(SUB_ASSIGN),TOKEN(MUL_ASSIGN),TOKEN(DIV_ASSIGN),TOKEN(MOD_ASSIGN),TOKEN(OT_EQUAL),
TOKEN(EQUAL),TOKEN(GREATER_EQ),TOKEN(LESS_EQ),TOKEN(NOT_EQUAL),
TOKEN(XOR_ASSIGN),TOKEN(OR_ASSIGN),TOKEN(AND_ASSIGN),TOKEN(AND),TOKEN(OR),TOKEN(INC),TOKEN(DEC),
TOKEN(ARROW),TOKEN(SHIFT_L_ASSIGN),TOKEN(SHIFT_R_ASSIGN) 


#endif

#ifdef RESERVED_EXPR

escape_sequences['\''] = '\'';
escape_sequences['"'] = '\"';
escape_sequences['\\'] = '\\';
escape_sequences['?'] = '\?';
escape_sequences['a'] = '\a';
escape_sequences['b'] = '\b';
escape_sequences['f'] = '\f';
escape_sequences['n'] = '\n';
escape_sequences['r'] = '\r';
escape_sequences['t'] = '\t';
escape_sequences['v'] = '\v';

ops[","] = COMMA;
ops["."] = DOT;
ops["~"] = TILDE;
ops["?"] = QUESTION;
ops[":"] = COLON;
ops[";"] = SEMICOLON;
ops["]"] = R_BRACKET;
ops["["] = L_BRACKET;
ops["("] = L_PARENTH;
ops[")"] = R_PARENTH;
ops["}"] = R_BRACE;
ops["{"] = L_BRACE;
ops["*"] = STAR;
ops["/"] = DIV;
ops["%"] = MOD;
ops["!"] = NEGATION;
ops["="] = ASSIGN;
ops["^"] = XOR;
ops["|"] = BIT_OR;
ops["&"] = BIT_AND; 
ops["+"] = ADD;
ops["-"] = SUB;
ops[">"] = GREATER;
ops["<"] = LESS;
ops["<<"] = SHIFT_L;
ops[">>"] = SHIFT_R;
ops["+="] = ADD_ASSIGN;
ops["-="] = SUB_ASSIGN;
ops["*="] = MUL_ASSIGN;
ops["/="] = DIV_ASSIGN;
ops["%="] = MOD_ASSIGN;
ops["!="] = NOT_EQUAL;
ops[" == "] = EQUAL;
ops[">="] = GREATER_EQ;
ops["<="] = LESS_EQ;
ops["+="] = ADD_ASSIGN;
ops["-="] = SUB_ASSIGN;
ops["^="] = XOR_ASSIGN;
ops["|="] = OR_ASSIGN;
ops["&="] = AND_ASSIGN;
ops["&&"] = AND;
ops["||"] = OR;
ops["++"] = INC;
ops["--"] = DEC;
ops["->"] = ARROW;
ops["<<="] = SHIFT_L_ASSIGN;
ops[">>="] = SHIFT_R_ASSIGN;

keywords["break"] = BREAK;
keywords["case"] = CASE;
keywords["char"] = CHAR;
keywords["const"] = CONST;
keywords["continue"] = CONTINUE;
keywords["default"] = DEFAULT;
keywords["do"] = DO;
keywords["else"] = ELSE;
keywords["enum"] = ENUM; 
keywords["extern"] = EXTERN;
keywords["float"] = FLOAT;
keywords["for"] = FOR;
keywords["goto"] = GOTO;
keywords["if"] = IF;
keywords["int"] = INT;
keywords["register"] = REGISTER;
keywords["return"] = RETURN;
keywords["signed"] = SIGNED;
keywords["sizeof"] = SIZEOF;
keywords["static"] = STATIC;
keywords["struct"] = STRUCT;
keywords["switch"] = SWITCH;
keywords["typedef"] = TYPEDEF;
keywords["union"] = UNION;
keywords["unsigned"] = UNSIGNED;
keywords["void"] = VOID;
keywords["while"] = WHILE;
#endif