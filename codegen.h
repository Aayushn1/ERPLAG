/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/


#include"sym.h"

void gen_code_util(char* str,tNode* ast_root,scopeNode* tableRoot);
void gen_code(tNode* ast_root,scopeNode* tableRoot);
void get_var_name(char* name, tNode* a);
void left(tNode* a);
void right(tNode* a);
void get_switch_id(char* name,tNode* a);
int get_switch_end(tNode* a);
int getChildrenNumber(tNode* a);
void compose(tNode* a,id_type t);
void gen_data(scopeNode* tableRoot);
void gen_prog(tNode* aroot , scopeNode* tableRoot);
