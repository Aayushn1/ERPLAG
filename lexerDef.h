/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/

#ifndef LEXERDEF
#define LEXERDEF

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>


int line;//line no. in file
FILE *fptr;
char* buff1,*buff2;
char *lexemeBegin, *forward;	// forward represents the lookahead pointer
bool read1, read2;	// Used to make sure we don't fread again after retracting past the edge of a buffer
int lexemeSize;	// Keeps track of the size of the current lexeme for efficiency
int lexerErrors;

enum enum_tnt{t_EPSILON, t_DOLLAR, t_AND, t_ARRAY, t_ASSIGNOP, t_BC, t_BO, t_BOOLEAN, t_BREAK, t_CASE, t_COLON, t_COMMA, t_DECLARE, t_DEF, t_DEFAULT, t_DIV, t_DRIVER,t_DRIVERDEF,t_DRIVERENDDEF, t_END, t_ENDDEF, t_EQ, t_FALSE, t_FOR, t_GE, t_GET_VALUE, t_GT, t_ID, t_IN, t_INPUT, t_INTEGER, t_LE, t_LT, t_MINUS, t_MODULE, t_MUL, t_NE, t_NUM, t_OF, t_OR, t_PARAMETERS, t_PLUS, t_PRINT, t_PROGRAM, t_RANGEOP, t_REAL, t_RETURNS, t_RNUM, t_SEMICOL, t_SQBC, t_SQBO, t_START, t_SWITCH, t_TAKES, t_TRUE, t_USE, t_WHILE, t_WITH, nt_AnyTerm, nt_arithmeticExpr, nt_arithmeticOrBooleanExpr, nt_assignmentStmt, nt_boolConstt, nt_caseStmts, nt_condionalStmt, nt_dataType, nt_declareStmt, nt_default, nt_driverModule, nt_expression, nt_factor, nt_idList, nt_index, nt_input_plist, nt_ioStmt, nt_iterativeStmt, nt_logicalOp, nt_lvalueIDStmt, nt_lvalueARRStmt, nt_module, nt_moduleDeclaration, nt_moduleDeclarations, nt_moduleDef, nt_moduleReuseStmt, nt_new_NT, nt_N1, nt_N2, nt_N3, nt_N4, nt_N5, nt_N7, nt_N8, nt_N9, nt_op1, nt_op2, nt_optional, nt_otherModules, nt_output_plist, nt_program, nt_range, nt_range_arrays, nt_relationalOp, nt_ret, nt_simpleStmt, nt_statement, nt_statements, nt_term, nt_type, nt_value, nt_var, nt_var_id_num, nt_whichId, nt_whichStmt, nt_unary_op, nt_U, t_IDF, nt_minusExpr, undec };//after t_IDF, for convenience exceptions
typedef enum enum_tnt tnt;
#define bufferSize 100
#define BUFFER_SIZE (1 << 5)

typedef struct _keyword {
	char lexeme[20];
	char tid[20];
}keyword;

typedef struct token{
	char lexeme[20];
	char tokenName[50];
	int lineNo;
	tnt tokenId;
} Token;

#endif
