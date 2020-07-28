/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/


#ifndef SYM_H
#define SYM_H


#include<stdio.h>
#include<stdbool.h>
#define HASH_TABLE_SIZE 100
#define bufferSize 100
#define SIZE_OF_BOOL 1
#define SIZE_OF_INT 2
#define SIZE_OF_REAL 4
#define DynARRAY -2
int symbolTableErrors;
int typeCheckErrors;
typedef enum {REAL,INTEGER,BOOLEAN,ARRAY,UNDEFINED}id_type;
typedef enum {ROOT,MODULE,DRIVER,WHILE,FOR,SWITCH}scope_type;

typedef struct var{
    id_type type;
    bool arr;
    bool isStartId,isEndId;
    char startId[20];
    char endId[20];
    int arr_start,arr_end;
    bool print;
}var_type;

typedef union{
    int integer;
    int float_value;
    bool boolean;
}value;

typedef struct var_def{
    char name[25];
    var_type type;
    value var_value;
    int line;
    int scope;
    int offset;
    bool isInput;
    bool isOutput;
    bool assigned;
}variable;

typedef struct entry{
    variable var;
    struct entry* next;                     //For implementing chaining in case of collision
}variable_entry;

typedef struct{
    int count;
    variable_entry* inputs;
}input;                         //Linked list of input variables - matlab? input output kya hai


typedef struct{
    int count;
    variable_entry* outputs;
}output;                        //linked list of output variables

typedef struct{
    char name[25];
    bool defined;
    input* inputs;
    output* outputs;         
	int lineDef; //new field, Nagpal take care of it
    int lineDec;
    bool isCalled;
    int width;
}module;                       

typedef struct scope_Node{
    scope_type type;                    
    variable_entry* var_Hash[HASH_TABLE_SIZE];   //For implementing hash table
    module module;
    struct scope_Node* parent;//uptil now yes, padh raha hu
    struct scope_Node** children; //okay haan
    int no_of_children;
    int scope_start;//ye line number hai?okay
    int scope_end;
}scopeNode;



/*
enum enum_tnt{t_DOLLAR, t_EPSILON, t_AND, t_ARRAY, t_ASSIGNOP, t_BC, t_BO, t_BOOLEAN, t_BREAK, t_CASE, t_COLON, t_COMMA, t_DECLARE, t_DEF, t_DEFAULT, t_DIV, t_DRIVER,t_DRIVERDEF,t_DRIVERENDDEF, t_END, t_ENDDEF, t_EQ, t_FALSE, t_FOR, t_GE, t_GET_VALUE, t_GT, t_ID, t_IN, t_INPUT, t_INTEGER, t_LE, t_LT, t_MINUS, t_MODULE, t_MUL, t_NE, t_NUM, t_OF, t_OR, t_PARAMETERS, t_PLUS, t_PRINT, t_PROGRAM, t_RANGEOP, t_REAL, t_RETURNS, t_RNUM, t_SEMICOL, t_SQBC, t_SQBO, t_START, t_SWITCH, t_TAKES, t_TRUE, t_USE, t_WHILE, t_WITH, nt_AnyTerm, nt_arithmeticExpr, nt_arithmeticOrBooleanExpr, nt_assignmentStmt, nt_caseStmts, nt_condionalStmt, nt_dataType, nt_declareStmt, nt_default, nt_driverModule, nt_expression, nt_factor, nt_idList, nt_index, nt_input_plist, nt_ioStmt, nt_iterativeStmt, nt_logicalOp, nt_lvalueIDStmt, nt_lvalueARRStmt, nt_module, nt_moduleDeclaration, nt_moduleDeclarations, nt_moduleDef, nt_moduleReuseStmt, nt_N1, nt_N2, nt_N3, nt_N4, nt_N5, nt_N7, nt_N8, nt_N9, nt_op1, nt_op2, nt_optional, nt_otherModules, nt_output_plist, nt_program, nt_range, nt_relationalOp, nt_ret, nt_simpleStmt, nt_statement, nt_statements, nt_term, nt_type, nt_value, nt_var, nt_whichId, nt_whichStmt, t_IDF, nt_minusExpr, undec };//after t_IDF, for convenience exceptions
typedef enum enum_tnt tnt;
typedef struct treeNod{
	tnt astName;
	tnt tokenId;
	char lexeme[bufferSize];
	unsigned long lineNo;
	int visited; //Used for inorder traversal
	int ruleUsed;
	struct treeNod* leftMostChild;
	struct treeNod* nextSibling;
	struct treeNod* parent;
	//scopeInfo scope;
	//st_data_type dataType;
	//enum_data_type exprType;
	//struct treeNod* parent;
}tNode;
*/

#endif
