/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/

#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

tNode* initTreeNode(tnt tokenId);
int indexNonTerm(tnt a);
int isTerm(tnt t);
void populateRules(rule** rules, char* filename);
void populateFIRST(rule** rules, term_sets** arr);
void populateFOLLOW(rule** rules,term_sets** arr);
void populateParseTable(int** parseTable, rule** rules, term_sets** arr);
int** initParseTable(int** parseTable);
tNode* parseInputSourceCode(char *testcaseFile, int** parseTable, rule** rules,term_sets** arr,FILE* ptr);
void printRules(rule** rules);
void printFirstAndFollow(term_sets** arr);
void printParseTree(tNode* head);
long int countParseTree(tNode* root);

#endif
