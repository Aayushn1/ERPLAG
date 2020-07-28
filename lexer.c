/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/

#include "lexer.h"

FILE* errorFile;

void removeComments(char * filename) {
	FILE * fptr;
	char ch1,ch2;
	fptr = fopen(filename, "r");
	int flag = 0;
	if(fptr == NULL)
		return;
	
	do {
		ch1 = getc(fptr);
		if(ch1 == EOF)
			break;
		if(ch1 != '*') {
			fprintf(errorFile,"%c", ch1);
		}
		else {
			ch2 = getc(fptr);
			if(ch2 == EOF)
				break;
			if(ch2 == '*') {
				while(1) {
					char ch3 = getc(fptr);
					if(ch3 == EOF) {
						flag = 1;
						break;
					}
					if(ch3 == '*') {
						char ch4 = getc(fptr);
						if(ch4 == EOF) {
							flag = 1;
							break;
						}
						if(ch4 == '*')
							break;
					}
				}
				if(flag == 1)
					break;
			}
			else {
				fprintf(errorFile,"%c", ch2);
			}
		}
	} while(1);

	fclose(fptr);
}

void initializeLexer(char *fileName,FILE* ptr) {
	errorFile = ptr;
	// fprintf(errorFile,"\n\nLexical Errors:\n");
	buff1 = (char*)malloc(sizeof(char)*(BUFFER_SIZE+1));
	buff2 = (char*)malloc(sizeof(char)*(BUFFER_SIZE+1));
	line = 1;
	fptr = fopen(fileName, "r");
	assert(fptr != NULL);
	int size1 = fread(buff1, sizeof(char), BUFFER_SIZE, fptr);
	buff1[size1] = EOF;
	lexemeBegin = forward = buff1;
	read1 = read2 = true;
	lexemeSize = 0;
	lexerErrors = 0;
}

void cleanup() {
	free(buff1);
	free(buff2);
}

// Get the next character from the input stream
char getNextChar() {
	char curr = *forward;
	if (forward == buff1 + BUFFER_SIZE - 1) {
		if (read2) {
			int size2 = fread(buff2, sizeof(char), BUFFER_SIZE, fptr);
			buff2[size2] = EOF;
		}
		forward = buff2;
		read2 = true; // Resetting the flag if it was false
	}
	else if (forward == buff2 + BUFFER_SIZE - 1) {
		if (read1) {
			int size1 = fread(buff1, sizeof(char), BUFFER_SIZE, fptr);
			buff1[size1] = EOF;
		}
		forward = buff1;
		read1 = true; // Resetting the flag if it was false
	}
	else {
		++forward;
	}
	++lexemeSize;
	return curr;
}

// int n: how many characters to retract
void retract(int n) {
	if (forward >= buff1 && forward < buff1 + BUFFER_SIZE) {
		if (forward - buff1 < n) {
			read1 = false; // Don't load buff1 the next time
			n -= forward - buff1;
			forward = buff2 + BUFFER_SIZE - n;
		}
		else {
			forward -= n;
		}
	}
	else {
		if (forward - buff2 < n) {
			read2 = false; // Don't load buff2 the next time
			n -= forward - buff2;
			forward = buff1 + BUFFER_SIZE - n;
		}
		else {
			forward -= n;
		}
	}
	lexemeSize -= n;
}

// Returns the lexeme just read (null-terminated)
void getLexeme(char *arr) {
	int i = 0;
	char *currptr = lexemeBegin;
	while (currptr != forward) {
		arr[i] = *currptr;
		++i;
		if (currptr == buff1 + BUFFER_SIZE - 1) {
			currptr = buff2;
		}
		else if (currptr == buff2 + BUFFER_SIZE - 1) {
			currptr = buff1;
		}
		else {
			++currptr;
		}
	}
	arr[i] = '\0';
}

// Reset to start reading a new lexeme
void reset() {
	lexemeBegin = forward;
	lexemeSize = 0;
}

void checkTable(char lexeme[], char token[], tnt* tokenId){//Matches the lexeme with keywords. Currenty takes linear time
	if(strcmp("AND",lexeme)==0){							//To be improved in the future by using DFA or Hash Table
		strcpy(token,"AND");
		*tokenId = t_AND;
	}
	else if(strcmp(lexeme,"array")==0){
		strcpy(token,"ARRAY");
		*tokenId = t_ARRAY;
	}
	else if(strcmp(lexeme,"boolean")==0){
		strcpy(token,"BOOLEAN");
		*tokenId = t_BOOLEAN;
	}
	else if(strcmp(lexeme,"break")==0){
		strcpy(token,"BREAK");
		*tokenId = t_BREAK;
	}
	else if(strcmp(lexeme,"case")==0){
		strcpy(token,"CASE");
		*tokenId = t_CASE;
	}
	else if(strcmp(lexeme,"declare")==0){
		strcpy(token,"DECLARE");
		*tokenId = t_DECLARE;
	}
	else if(strcmp(lexeme,"default")==0){
		strcpy(token,"DEFAULT");
		*tokenId = t_DEFAULT;
	}
	else if(strcmp(lexeme,"driver")==0){
		strcpy(token,"DRIVER");
		*tokenId = t_DRIVER;
	}
	else if(strcmp(lexeme,"end")==0){
		strcpy(token,"END");
		*tokenId = t_END;
	}
	else if(strcmp(lexeme,"false")==0){
		strcpy(token,"FALSE");
		*tokenId = t_FALSE;
	}
	else if(strcmp(lexeme,"for")==0){
		strcpy(token,"FOR");
		*tokenId = t_FOR;
	}
	else if(strcmp(lexeme,"get_value")==0){
		strcpy(token,"GET_VALUE");
		*tokenId = t_GET_VALUE;
	}
	else if(strcmp(lexeme,"in")==0){
		strcpy(token,"IN");
		*tokenId = t_IN;
	}
	else if(strcmp(lexeme,"input")==0){
		strcpy(token,"INPUT");
		*tokenId = t_INPUT;
	}
	else if(strcmp(lexeme,"integer")==0){
		strcpy(token,"INTEGER");
		*tokenId = t_INTEGER;
	}
	else if(strcmp(lexeme,"module")==0){
		strcpy(token,"MODULE");
		*tokenId = t_MODULE;
	}
	else if(strcmp(lexeme,"of")==0){
		strcpy(token,"OF");
		*tokenId = t_OF;
	}
	else if(strcmp(lexeme,"OR")==0){
		strcpy(token,"OR");
		*tokenId = t_OR;
	}
	else if(strcmp(lexeme,"parameters")==0){
		strcpy(token,"PARAMETERS");
		*tokenId = t_PARAMETERS;
	}
	else if(strcmp(lexeme,"print")==0){
		strcpy(token,"PRINT");
		*tokenId = t_PRINT;
	}
	else if(strcmp(lexeme,"program")==0){
		strcpy(token,"PROGRAM");
		*tokenId = t_PROGRAM;
	}
	else if(strcmp(lexeme,"real")==0){
		strcpy(token,"REAL");
		*tokenId = t_REAL;
	}
	else if(strcmp(lexeme,"returns")==0){
		strcpy(token,"RETURNS");
		*tokenId = t_RETURNS;
	}
	else if(strcmp(lexeme,"start")==0){
		strcpy(token,"START");
		*tokenId = t_START;
	}
	else if(strcmp(lexeme,"switch")==0){
		strcpy(token,"SWITCH");
		*tokenId = t_SWITCH;
	}
	else if(strcmp(lexeme,"takes")==0){
		strcpy(token,"TAKES");
		*tokenId = t_TAKES;
	}
	else if(strcmp(lexeme,"true")==0){
		strcpy(token,"TRUE");
		*tokenId = t_TRUE;
	}
	else if(strcmp(lexeme,"use")==0){
		strcpy(token,"USE");
		*tokenId = t_USE;
	}
	else if(strcmp(lexeme,"while")==0){
		strcpy(token,"WHILE");
		*tokenId = t_WHILE;
	}
	else if(strcmp(lexeme,"with")==0){
		strcpy(token,"WITH");
		*tokenId = t_WITH;
	}
	else{
		strcpy(token,"ID");
		*tokenId = t_ID;
	}	
}

Token* checkToken() {
	Token* t = (Token*)malloc(sizeof(Token));
	getLexeme(t->lexeme);
	checkTable(t->lexeme, t->tokenName, &(t->tokenId));
	t->lineNo = line;
	return t;		
}

Token* getNextToken(){
    int state = 0;
    int token_length = 0;
    int err = -1;
    char c;
    char lexeme[20];
	int id_length = 0;
    Token* t = (Token*)malloc(sizeof(Token));
    strcpy(t->lexeme,"");

    while(state >= 0)
    {
        switch(state)
        {
            case 0:{
                c = getNextChar();
				if(c == EOF)
				{
					strcpy(t->tokenName, "$");
					t->lineNo = line;
					t->tokenId = t_DOLLAR;
					return t;
				}			
				//newline character
				if(c == '\n') {
					++line;
					reset();
				}
				//whitespace characters
				else if(c == ' ' || c == '\t')
					reset();
				else if(c == '=')
					state = 1;
				else if(c == '!')
					state = 2;
				else if(c == ':')
					state = 3;	
				else if(c == '.')
					state = 4;
				else if(c == '(')
					state = 5;
				else if(c == ')')
					state = 6;
				else if(c == '[')
					state = 7;
				else if(c == ']')
					state = 8;
				else if(c == ';')
					state = 9;
				else if(c == ',')
					state = 10;
				else if(c == '+')
					state = 11;
				else if(c == '-')
					state = 12;
				else if(c == '<')
					state = 13;
				else if(c == '/')
					state = 14;
				else if(c == '*')
					state = 15;
				else if(c == '>')
					state = 16;
				else if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
					state = 17;
				else if(c >= '0' && c <= '9')
					state = 18;
				else {
					state = -1;
					err = 0;
				}
				
		    }
		    break;

            case 1:{
				c = getNextChar();
				if(c == '='){
					strcpy(t->tokenName, "EQ");
					t->lineNo = line;
					getLexeme(t->lexeme);
					t->tokenId = t_EQ;
					reset();
					return t;
				}
				else
				{
					state = -1;
					err = 1;
				}
				break;
			}

			case 2:{
				c = getNextChar();
				if(c == '='){
					strcpy(t->tokenName, "NE");
					t->lineNo = line;
					getLexeme(t->lexeme);
					t->tokenId = t_NE;
					reset();
					return t;
				}
				else
				{
					state = -1;
					err = 2;
				}
				break;
			}

			case 3:{
				c = getNextChar();
				if(c == '='){
					strcpy(t->tokenName, "ASSIGNOP");
					t->lineNo = line;
					getLexeme(t->lexeme);
					t->tokenId = t_ASSIGNOP;
					reset();
					return t;
				}				
				else
				{
					strcpy(t->tokenName, "COLON");
					t->lineNo = line;
					retract(1);
					getLexeme(t->lexeme);
					t->tokenId = t_COLON;
					reset();
					return t;
				}
				break;
			}

			case 4:{
				c = getNextChar();
				if(c == '.'){
					strcpy(t->tokenName, "RANGEOP");
					t->lineNo = line;
					getLexeme(t->lexeme);
					t->tokenId = t_RANGEOP;
					reset();
					return t;
				}
				else
				{
					state = -1;
					err = 4;
				}
				break;
			}

			case 5:{
				strcpy(t->tokenName, "BO");
				t->lineNo = line;
				getLexeme(t->lexeme);
				t->tokenId = t_BO;
				reset();
				return t;
				break;
			}

			case 6:{
				strcpy(t->tokenName, "BC");
				t->lineNo = line;
				getLexeme(t->lexeme);
				t->tokenId = t_BC;
				reset();
				return t;
				break;
			}

			case 7:{
				strcpy(t->tokenName, "SQBO");
				t->lineNo = line;
				getLexeme(t->lexeme);
				t->tokenId = t_SQBO;
				reset();
				return t;
				break;
			}

			case 8:{
				strcpy(t->tokenName, "SQBC");
				t->lineNo = line;
				getLexeme(t->lexeme);
				t->tokenId = t_SQBC;
				reset();
				return t;
				break;
			}

			case 9:{
				strcpy(t->tokenName, "SEMICOL");
				t->lineNo = line;
				getLexeme(t->lexeme);
				t->tokenId = t_SEMICOL;
				reset();
				return t;
				break;
			}

			case 10:{
				strcpy(t->tokenName, "COMMA");
				t->lineNo = line;
				getLexeme(t->lexeme);
				t->tokenId = t_COMMA;
				reset();
				return t;
				break;
			}

			case 11:{
				strcpy(t->tokenName, "PLUS");
				t->lineNo = line;
				getLexeme(t->lexeme);
				t->tokenId = t_PLUS;
				reset();
				return t;
				break;
			}

			case 12:{
				strcpy(t->tokenName, "MINUS");
				t->lineNo = line;
				getLexeme(t->lexeme);
				t->tokenId = t_MINUS;
				reset();
				return t;
				break;
			}

			case 13:{
				c = getNextChar();
				if(c == '=') {
					strcpy(t->tokenName, "LE");
					t->lineNo = line;
					getLexeme(t->lexeme);
					t->tokenId = t_LE;
					reset();
					return t;
				}

				else if(c == '<'){
					c = getNextChar();
					if(c == '<') {
						strcpy(t->tokenName, "DRIVERDEF");
						t->tokenId = t_DRIVERDEF;
					}
					else {
						strcpy(t->tokenName, "DEF");
						t->tokenId = t_DEF;
						retract(1);
					}
					t->lineNo = line;
					getLexeme(t->lexeme);
					reset();
					return t;
				}

				else{
					strcpy(t->tokenName, "LT");
					t->lineNo = line;
					retract(1);
					getLexeme(t->lexeme);
					t->tokenId = t_LT;
					reset();
					return t;
				}

				break;
			}

			case 14:{
				strcpy(t->tokenName, "DIV");
				t->lineNo = line;
				getLexeme(t->lexeme);
				t->tokenId = t_DIV;
				reset();
				return t;
				break;
			}

			case 15:{
				c = getNextChar();

				if(c == '*'){
					c = getNextChar();
				
					Loop:	
						while( c != '*'){
							if(c == '\n')
								line++;
							c = getNextChar();
						}
						
						if((c = getNextChar()) != '*'){
							if(c == '\n')
								line++;
							goto Loop;
						}
						else {
							state = 0;
							reset();
							//return getNextToken();
							break;
						}
						
				}

				else{
					strcpy(t->tokenName, "MUL");
					t->lineNo = line;
					retract(1);
					getLexeme(t->lexeme);
					t->tokenId = t_MUL;
					reset();
					return t;
				}
				break;
			}

			case 16:{
				c = getNextChar();
				if(c == '=') {
					strcpy(t->tokenName, "GE");
					t->lineNo = line;
					getLexeme(t->lexeme);
					t->tokenId = t_GE;
					reset();
					return t;
				}

				else if(c == '>'){
					c = getNextChar();
					if(c == '>') {
						strcpy(t->tokenName, "DRIVERENDDEF");
						t->tokenId = t_DRIVERENDDEF;
					}
					else {
						strcpy(t->tokenName, "ENDDEF");
						t->tokenId = t_ENDDEF;
						retract(1);
					}
					t->lineNo = line;
					getLexeme(t->lexeme);
					reset();
					return t;
				}

				else{
					strcpy(t->tokenName, "GT");
					t->lineNo = line;
					retract(1);
					getLexeme(t->lexeme);
					t->tokenId = t_GT;
					reset();
					return t;
				}
				break;
			}

			//TODO
			//For keywords and identifiers
			case 17:{		
				c = getNextChar();
				if((c >= 'a' && c <= 'z') 
					|| (c >= 'A' && c <= 'Z') 
					|| (c >= '0' && c <= '9') 
					|| c == '_'){
					state = 17;
				}
				else{
					if(lexemeSize > 30){
						lexerErrors++;
						fprintf(errorFile,"Line %d: LEXICAL_ERROR : Token length greater than 30\n",line);
					}
					else{
						retract(1);
						t = checkToken();
						reset();
						return t;
					}
					reset();
				}
				break;			
			}

			//Integer Numbers
			case 18:{		
				c = getNextChar();

				if(c >= '0' && c <= '9')
					state = 18;
				else if(c == '.')
					state = 19;
				else{
					t->lineNo = line;
					strcpy(t->tokenName, "NUM");
					retract(1);
					getLexeme(t->lexeme);
					t->tokenId = t_NUM;
					reset();
					return t;
				}
				break;		
			}
			
			
			//First Decimal input
			case 19:{		
				c = getNextChar();
				if(c >= '0' && c <= '9')
					state = 20;

				else if(c == '.'){
					retract(2);
					strcpy(t->tokenName, "NUM");
					getLexeme(t->lexeme);
					t->tokenId = t_NUM;
					t->lineNo = line;
					reset();
					return t;
				}
				else{
					state = -1;
					err = 19;
				}				
				break;				
			}

			//Input after first decimal
			case 20:{		
				c = getNextChar();
				if(c >= '0' && c <= '9')
					state = 20;
				else if(c == 'e' || c == 'E')
					state = 21;	
				else{
					t->lineNo = line;
					strcpy(t->tokenName, "RNUM");
					retract(1);
					getLexeme(t->lexeme);
					t->tokenId = t_RNUM;
					reset();
					return t;
				}
				break;
			}

			//After e|E
			case 21:{		
				c = getNextChar();
				if(c == '+' || c == '-')
					state = 22;
				else if(c <= '9' && c >= '0')
					state = 23;	
				else
				{
					state = -1;
					err = 21;
				}
				break;		
			}

			//For number after +|-
			case 22:{		
				c = getNextChar();
				if(c >= '0' && c <= '9')
					state = 23;
				else{
					state = -1;
					err = 22;
				}
				break;	
			}

			//For nos. after +|- or [0-9]
			case 23:{		
				c = getNextChar();
				if(c >= '0' && c <= '9')
					state = 23;
				else{
					strcpy(t->tokenName, "RNUM");
					t->lineNo = line;
					retract(1);
					getLexeme(t->lexeme);
					t->tokenId = t_RNUM;
					reset();
					return t;
				}
				break;	
			}        
        }
    }

	if(err != -1)
	{
		lexerErrors++;
		fprintf(errorFile,"Line %d: LEXICAL_ERROR",line);
		getLexeme(lexeme);
		fprintf(errorFile," in lexeme: %s :",lexeme);
		//fprintf(errorFile,"\tLexer state: %d\n",err);

		switch(err){
			case 0:
				fprintf(errorFile,"\tUnrecognised character: %c\n",c);
				break;
			case 1:{
				if(c == EOF){
					fprintf(errorFile,"\tUnexpected character: EOF,expected '='\n");
					break;
				}
				else
					fprintf(errorFile,"\tUnexpected character: '%c', expected '='\n",c);
				retract(1);
				break;
			}
			
			case 2:{
				if(c == EOF){
					fprintf(errorFile,"\tUnexpected character: EOF,expected '='\n");
					break;
				}
				else
					fprintf(errorFile,"\tUnexpected character: '%c', expected '='\n",c);
				retract(1);
				break;	
			}

			case 3:{
				if(c==EOF){
					fprintf(errorFile,"\tUnexpected character: EOF,expected '='\n");
					break;
				}
				else
					fprintf(errorFile,"\tUnexpected character: '%c', expected '='\n",c);
				retract(1);
				break;	
			}

			case 4:{
				if(c==EOF){
					fprintf(errorFile,"\tUnexpected character: EOF,expected '.'\n");
					break;
				}
				else
					fprintf(errorFile,"\tUnexpected character: '%c', expected '.'\n",c);
				retract(1);
				break;	
			}			
			case 19:{
				if(c==EOF){
					fprintf(errorFile,"\tUnexpected character: EOF,expected '[0-9]'\n");
					break;
				}
				else
					fprintf(errorFile,"\tUnexpected character: '%c', expected '[0-9]'\n",c);
				retract(1);
				break;	
			}

			case 21:{
				if(c==EOF){
					fprintf(errorFile,"\tUnexpected character: EOF,expected 'e|E' or '[0-9]'\n");
					break;
				}
				else
					fprintf(errorFile,"\tUnexpected character: '%c', expected 'e|E' or '[0-9]'\n",c);
				retract(1);
				break;
			}

			case 22:{
				if(c==EOF){
					fprintf(errorFile,"\tUnexpected character: EOF,expected '[0-9]'\n");
					break;
				}
				else
					fprintf(errorFile,"\tUnexpected character: '%c', expected [0-9]'\n",c);
				retract(1);
				break;
			}

			default: fprintf(errorFile,"%d",err);			
		}

		if(c==EOF)
			strcpy(t->tokenName, "$");
		else
			strcpy(t->tokenName, "ERROR");
		reset();
		return t;
	}

}
/*
int main(int argc, char ** argv) {
	initializeLexer(argv[1]);
	Token* t;
	t = getNextToken();
	while (strcmp(t->tokenName, "$") != 0)
	{
		if (strcmp(t->lexeme, "") != 0)
			fprintf(errorFile,"Line: %d\t\tLexeme: %s\t\tToken: %s\t\tTokenId: %d\n", t->lineNo, t->lexeme, t->tokenName, t->tokenId);
		t = getNextToken();
	}
}
*/
