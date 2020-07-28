/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/


#include"sym.h"

var_type get_var_type(char* varName , scopeNode* sn, int lineno);
variable_entry* get_var_ptr(char* varName , scopeNode* sn,int lineNo);
void setDefined(variable_entry* varPtr , scopeNode* sn,int lineNo);
void Asttype(tNode *Astptr);
void semantic_analysis(tNode *temp);
bool modOutput(tNode* temp, module *mod);
void modInput(tNode* temp, module *mod);
bool modulesEqual(module mod,module modTemp,int lineNo);
module checkModule(char *modname,scopeNode *sn, int line);
int getLineNo(tNode *temp);
void fill_while_lexemes(tNode*temp);
void initSemanticAnalysis(FILE* ptr);
