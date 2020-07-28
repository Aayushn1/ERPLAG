/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/


#ifndef PARSERDEF
#define PARSERDEF

#include "symbolTableDef.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>
#include "lexer.h"


#define no_rules 105
#define no_tnt 115	//+1 not included here, i.e. t_IDF, function parameter.
#define tot_tnt 117
#define NUM_T 58
#define NUM_NT (no_tnt-NUM_T)

int parserErrors;

typedef struct treeNod{
	tnt astName;
	tnt tokenId;
	char lexeme[bufferSize];
	int lineNo;
	int visited; //Used for inorder traversal
	int ruleUsed;
	scopeNode* scope;
	struct treeNod* leftMostChild;
	struct treeNod* nextSibling;
	struct treeNod* parent;
	id_type type; //new field added, assigned in type checking
} tNode;

typedef struct Nod{
    tNode* data;
    struct Nod* next;
} node;

typedef struct{
	tnt LHS;
	tnt *RHS;
	int length_RHS;
} rule;

typedef struct{
	tnt token;
	unsigned long long FIRST;
	unsigned long long FOLLOW;
} term_sets;


#endif
