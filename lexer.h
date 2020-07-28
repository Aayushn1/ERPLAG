/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/

#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

void initializeLexer(char* fileName,FILE* ptr);
void cleanup();
char getNextChar();
void retract();
void getLexeme();
void reset();
char* isKeyword(char* lexeme);
Token* checkToken();
Token* getNextToken();
void removeComments(char *filename);

#endif
