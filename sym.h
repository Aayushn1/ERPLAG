/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/


#include"parser.h"
#include"ast.h"

scopeNode* initializeSymbolTable(FILE* ptr);
int hashFunction(char str[]);
scopeNode* varInsert(variable x,scopeNode* head);
void insertModuleInputOutput(module mod,int start_line,int end_line);
var_type extractArrayDimensions(tNode* root);
int toInt(char str[]);
input* getInputList(tNode* root);
output* getOutputList(tNode* root);
void insertIdList(tNode* root,var_type type,scopeNode* head);
void addModule(tNode* root,scopeNode* curr,scopeNode* tableRoot);
scopeNode* createSymbolTable(tNode* root);
void printSymbolTable(scopeNode* root);
void printArrays(scopeNode* root);
void printActivationRecords(scopeNode* root);
