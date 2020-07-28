/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/

#ifndef AST_H
#define AST_H
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include "parserDef.h"
#include "lexer.h"
#include "parser.h"
void parseTreeToAST(tNode** rootPtr,rule** rules);
bool terminalIsAllowed(tnt term);
bool isNonTerm(tnt term);
bool isAllowed(tnt val);
int condenseSubTree(tNode* currNode);
void transmuteNode(tNode* parent);
void repairEpsilonNodes(tNode* parent);
#endif
