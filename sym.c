/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/


#include "sym.h"
#include <stdio.h>
#include<string.h>


FILE* errorFile;
scopeNode* tableRoot;
scopeNode* curr = NULL;
int curr_scope,curr_offset;
char* type[] = {"REAL","INTEGER","BOOLEAN","ARRAY","UNDEFINED"};
char* scope[] = {"ROOT","MODULE","DRIVER","WHILE","FOR","SWITCH"};
char* table1[] = {"EPS", "$", "AND", "ARRAY", "ASSIGNOP", "BC", "BO", "BOOLEAN", "BREAK", "CASE", "COLON", "COMMA", "DECLARE", "DEF", "DEFAULT", "DIV", "DRIVER", "DRIVERDEF", "DRIVERENDDEF", "END", "ENDDEF", "EQ", "FALSE", "FOR", "GE", "GET_VALUE", "GT", "ID", "IN", "INPUT", "INTEGER", "LE", "LT", "MINUS", "MODULE", "MUL", "NE", "NUM", "OF", "OR", "PARAMETERS", "PLUS", "PRINT", "PROGRAM", "RANGEOP", "REAL", "RETURNS", "RNUM", "SEMICOL", "SQBC", "SQBO", "START", "SWITCH", "TAKES", "TRUE", "USE", "WHILE", "WITH", "AnyTerm", "arithmeticExpr", "arithmeticOrBooleanExpr", "assignmentStmt", "boolConstt", "caseStmts", "condionalStmt", "dataType", "declareStmt", "Default", "driverModule", "expression", "factor", "idList", "Index", "input_plist", "ioStmt", "iterativeStmt", "logicalOp", "lvalueIDStmt", "lvalueARRStmt", "module", "moduleDeclaration", "moduleDeclarations", "moduleDef", "moduleReuseStmt", "new_NT", "N1", "N2", "N3", "N4", "N5", "N7", "N8", "N9", "op1", "op2", "optional", "otherModules", "output_plist", "program", "range", "range_arrays", "relationalOp", "ret", "simpleStmt", "statement", "statements", "term", "type", "value", "var", "var_id_num", "whichId", "whichStmt", "unary_op", "U", "IDF", "minusExpr", "undec" };//after t_IDF, for convenience exceptions;
int size[] = {SIZE_OF_REAL,SIZE_OF_INT,SIZE_OF_BOOL};
scopeNode* initializeSymbolTable(FILE* ptr){
    tableRoot = (scopeNode*)malloc(sizeof(scopeNode));
    tableRoot->children = NULL;
    tableRoot->no_of_children = 0;
    tableRoot->parent = NULL;
    tableRoot->type = ROOT;
    tableRoot->scope_start = 0;
    tableRoot->scope_end = 0;
    for(int i = 0 ; i < HASH_TABLE_SIZE ; i++)
    {
       tableRoot->var_Hash[i] = NULL;
    }
    curr_scope = curr_offset = 0;
    curr = tableRoot;
    symbolTableErrors = 0;

    errorFile = ptr;

    return tableRoot;
}

int hashFunction(char str[]){
    int i = 0;
    int sum = 0;
    while(str[i]!='\0')
    {
        if(str[i]>='a' && str[i]<='z')
            sum += str[i] - 'a';
        else if(str[i]>='A' && str[i]<='Z')
            sum += str[i] - 'A';
        else if(str[i]>='0' && str[i]<='9')
            sum += str[i] - '0';
        
       i++;     
    }

    sum = sum % 26;
    return sum;
}

scopeNode* varInsert(variable x,scopeNode* head){

    //get position in hash table
    int hash_position = hashFunction(x.name)%100;

    //if no collision, create new chain
    if(head->var_Hash[hash_position]==NULL)
    {
        variable_entry* temp = (variable_entry*)malloc(sizeof(variable_entry));
        temp->var = x;
        temp->next = NULL;
        head->var_Hash[hash_position] = temp;
    }
    
    //if collision, add to chain
    else{
        variable_entry* temp = (variable_entry*)malloc(sizeof(variable_entry));
        temp->var = x;
        temp->next = NULL;
        variable_entry* curr_entry = head->var_Hash[hash_position];

        //check till second last entry in chain
        while(curr_entry->next!=NULL)       
        {
            if((strcmp(curr_entry->var.name,x.name)==0) && (curr_entry->var.isInput == false))
            {
                symbolTableErrors++;
                fprintf(errorFile,"Line %d: Redeclaration of %s\n",x.line,x.name);
                return head;
            }

            else if((strcmp(curr_entry->var.name,x.name)==0) && (curr_entry->var.isInput == true)){
                if(x.type.print == false)
                    return head;
                curr_entry->next = temp;
                curr_entry->var.isInput = false;
                curr_entry->var.isOutput = true;
                return head;
            }
            curr_entry = curr_entry->next;
        }

        //check in last entry of chain
        if((strcmp(curr_entry->var.name,x.name)==0) && (curr_entry->var.isInput == false))
        {   
            symbolTableErrors++;
            fprintf(errorFile,"Line %d: Redeclaration of %s\n",x.line,x.name);
            return head;
        }
        else if((strcmp(curr_entry->var.name,x.name)==0) && (curr_entry->var.isInput == true)){
            if(x.type.print==false)
                return head;
            curr_entry->next = temp;
            curr_entry->var.isInput = false;
            curr_entry->var.isOutput = true;
            return head;
        }

        //add at the end of chain if not already inserted
        curr_entry->next = temp;

    }

    return head;

}




void insertModuleInputOutput(module mod,int start_line,int end_line){
    if(tableRoot->no_of_children > 0){
        for(int i = 0 ; i < tableRoot->no_of_children ; i++)
        {
            if(strcmp(tableRoot->children[i]->module.name,mod.name) == 0)
            {
                if(tableRoot->children[i]->module.defined == true)
                {
                    //symbolTableErrors++;
                    //fprintf(errorFile,"Line %d: Module %s already defined\n",mod.lineDec,mod.name);
                    return ;
                }
                else{
					int line = tableRoot->children[i]->module.lineDec;
					mod.lineDec = line;
                    tableRoot->children[i]->module = mod;
                    scopeNode* temp = tableRoot->children[i];
                    variable_entry* in = ((temp->module).inputs)->inputs;
                    variable_entry* out = (temp->module).outputs->outputs;
                    temp->scope_start = start_line;
                    temp->scope_end = end_line;

                     for(int i = 0 ; i < temp->module.inputs->count ; i++)
                    {
                        temp = varInsert(in->var,temp);
                        in = in->next;
                    }
                
                    for(int i=0;i< temp->module.outputs->count; i++){
                        temp = varInsert(out->var, temp);
                        out = out->next;
                    }
                    curr = temp;
					return ;
                }
            }
        }
    }    

    scopeNode* temp = (scopeNode*)malloc(sizeof(scopeNode));
    temp->type = MODULE;
    temp->module = mod;
    temp->children = NULL;
    temp->parent = tableRoot;
    temp->no_of_children = 0;
    temp->scope_start = start_line;
    temp->scope_end = end_line;
    for(int i = 0 ; i < HASH_TABLE_SIZE ; i++)
        temp->var_Hash[i] = NULL;
    
    variable_entry* in ,*out;
    if(temp->module.inputs!=NULL && temp->module.inputs->count != 0)    
    {
    	in = ((temp->module).inputs)->inputs;
    	for(int i = 0 ; i < temp->module.inputs->count ; i++)
    {
        temp = varInsert(in->var,temp);
        in = in->next;
    }
    }
    if(temp->module.outputs!=NULL && temp->module.outputs->count!=0)
    {
    	out = ((temp->module).outputs)->outputs;
      for(int i=0;i< temp->module.outputs->count; i++){
        temp = varInsert(out->var, temp);
        out = out->next;
    	}
    }

    tableRoot->children = realloc(tableRoot->children,(tableRoot->no_of_children+1)*sizeof(scopeNode*));
    tableRoot->children[tableRoot->no_of_children] = temp;
    tableRoot->no_of_children++;
    curr = temp;
}



// <type> => INTEGER
// <type> => REAL
// <type> => BOOLEAN
// root is type->leftMostChild
id_type extractType(tNode* root){
    
    if(root->tokenId == t_INTEGER)
        return INTEGER;
    else if(root->tokenId == t_REAL)
        return REAL;
    else if(root->tokenId == t_BOOLEAN)
        return BOOLEAN;
    else if(root->leftMostChild->tokenId == nt_range_arrays)
        return ARRAY;//extractType(root->leftMostChild->nextSibling);
    else 
        return UNDEFINED;            
}

//to extract the array start and end indexes
//<dataType> => ARRAY SQBO <range_arrays> SQBC OF <type>
//<range_arrays> => <Index> RANGEOP <Index>
//<Index> => NUM 
//<Index> => ID
//root->tokenId is nt_dataType
var_type extractArrayDimensions(tNode* root){
    var_type var;
    tNode* temp = root->leftMostChild->leftMostChild; //temp->tokenId is nt_index->child
    tNode* temp1 = temp->nextSibling;   //temp1->tokenId is nt_index->child

    var.type = extractType(root->leftMostChild->nextSibling);
    var.print = true;

    if(temp->tokenId == t_NUM) {
        var.arr = true;
        var.isStartId = false;
        strcpy(var.startId,temp->lexeme);
        var.arr_start = toInt(temp->lexeme);
    }

    else if(temp->tokenId == t_ID){
        var.arr = true;
        var.isStartId = true;
        strcpy(var.startId,temp->lexeme);        
        var.arr_start = DynARRAY;
    }
    
	if(temp1->tokenId == t_NUM) {
        var.arr = true;
        var.isEndId = false;
        strcpy(var.endId,temp1->lexeme);
        var.arr_end = toInt(temp1->lexeme);
    }

    else if(temp1->tokenId == t_ID) {
        var.arr = true;
        var.isEndId = true;
        strcpy(var.endId,temp1->lexeme);         //TO DO:make lookup to extract value of id in array
        var.arr_end = DynARRAY; 
    }            
    
	return var; //correction made
}

int toInt(char str[]){
    int i = strlen(str)-1,mul = 1;
    int val = 0;
    while(i >= 0)
    {
        val += (str[i]-'0')*mul;
        mul*=10;
        i--;
    }

    return val;
}

//returns linked list of input variables
//root is <input_plist>
input* getInputList(tNode* root){

    input* ans = (input*)malloc(sizeof(input));
    ans->count = 0;
    ans->inputs = NULL;
    variable_entry* curr_entry = ans->inputs;
    if(root->leftMostChild==NULL)
    {
        return ans;
    }    
    
    tNode* temp = root->leftMostChild;
    tNode* temp1;
    while(temp!=NULL)
    {
        if(temp->tokenId == t_ID)
        {   
        
            variable_entry* vari = (variable_entry*)malloc(sizeof(variable_entry));
            vari->var.type.arr = false;
            vari->var.isInput = true;
            vari->var.isOutput = false;
            if(temp->nextSibling->tokenId == t_REAL)
            {
                    vari->var.type.type = REAL;
                    vari->var.offset = curr_offset;
                    vari->var.type.print = true;
                    curr_offset += SIZE_OF_REAL;
            }
            else if(temp->nextSibling->tokenId == t_INTEGER)
            {
                    vari->var.type.type = INTEGER;
                    vari->var.offset = curr_offset;
                    vari->var.type.print = true;
                    curr_offset += SIZE_OF_INT;
            }
            else if(temp->nextSibling->tokenId == t_BOOLEAN)
            {
                    vari->var.type.type = BOOLEAN;
                    vari->var.type.print = true;
                    vari->var.offset = curr_offset;
                    curr_offset += SIZE_OF_BOOL;
            } 
            
            else if((temp1=temp->nextSibling->leftMostChild)->tokenId == nt_range_arrays)
            {
                //vari->var.type.type = ARRAY;
                vari->var.type.arr = true;
                vari->var.type = extractArrayDimensions(temp1->parent);
                vari->var.offset = curr_offset;
                curr_offset += 1+2*size[INTEGER];
                vari->var.type.print = true;

                if(vari->var.type.isStartId){
                    variable_entry* t = (variable_entry*)malloc(sizeof(variable_entry));
                    variable x;
                    x.isInput = true;
                    x.isOutput = false;
                    x.line = vari->var.line;
                    strcpy(x.name,vari->var.type.startId);
                    x.type.print = false;
                    x.type.arr = false;
                    x.type.type = INTEGER;
                    t->var = x;
                    //varInsert(x,curr);
                    t->next = NULL;
                    ans->count++;

                    if(ans->inputs == NULL)
                    {
                        ans->inputs = t;
                        curr_entry = t;
                    }

                    else{
                        curr_entry->next = t;
                        curr_entry = curr_entry->next;
                    }

                }

                if(vari->var.type.isEndId){
                    variable x;
                    x.isInput = true;
                    x.isOutput = false;
                    x.line = vari->var.line;
                    strcpy(x.name,vari->var.type.endId);
                    x.type.print = false;
                    x.type.arr = false;
                    x.type.type = INTEGER;
                    //varInsert(x,curr);
                    variable_entry* t = (variable_entry*)malloc(sizeof(variable_entry));
                    t->var = x;
                    t->next = NULL;
                    
                    ans->count++;

                    if(ans->inputs == NULL)
                    {
                        ans->inputs = t;
                        curr_entry = t;
                    }

                    else{
                        curr_entry->next = t;
                        curr_entry = curr_entry->next;
                    }

                }
                
                //vari->var.type.arr_start = toInt(temp1->nextSibling->nextSibling->leftMostChild->nextSibling->nextSibling->lexeme);                
            }
            
            strcpy(vari->var.name,temp->lexeme);
            vari->var.line = temp->lineNo;
            vari->var.scope = curr_scope;
            //vari->var.type.print = true;
            ans->count++;

            if(ans->inputs == NULL)
            {
                ans->inputs = vari;
                curr_entry = vari;
            }

            else{
                curr_entry->next = vari;
                curr_entry = curr_entry->next;
            } 

            temp = temp->nextSibling;    
        }
        
        else if(temp->tokenId == nt_N1)
        {
            temp = temp->leftMostChild;
        }
        else
            temp = temp->nextSibling;
        
    }

    return ans;

}

//returns linked list of output variables
//root is <output_plist>
output* getOutputList(tNode* root){
    
    output* ans = (output*)malloc(sizeof(output));
    ans->count = 0;
    ans->outputs = NULL;
    variable_entry* curr_entry = NULL;
    if(root == NULL ||  root->leftMostChild==NULL)
    {
        return ans;
    }    
    
    tNode* temp = root->leftMostChild;  //temp->tokenId = t_ID
    tNode* temp1;
    while(temp!=NULL)
    {
        if(temp->tokenId == t_ID)
        {
            variable_entry* vari = (variable_entry*)malloc(sizeof(variable_entry));
            vari->var.type.arr = false;
            vari->var.isInput = false;
            vari->var.isOutput = true;
            if(temp->nextSibling->tokenId == t_REAL)
            {
                    vari->var.type.type = REAL;
                    vari->var.offset = curr_offset;
                    curr_offset += SIZE_OF_REAL;
            }
            
            else if(temp->nextSibling->tokenId == t_INTEGER)
            {
                    vari->var.type.type = INTEGER;
                    vari->var.offset = curr_offset;
                    curr_offset += SIZE_OF_INT;
            }
            
            else if(temp->nextSibling->tokenId == t_BOOLEAN)
            {
                    vari->var.type.type = BOOLEAN;
                    vari->var.offset = curr_offset;
                    curr_offset += SIZE_OF_BOOL;
            }

            strcpy(vari->var.name,temp->lexeme);
            ans->count++;
            vari->var.line = temp->lineNo;
            vari->var.scope = curr_scope;
            vari->var.type.print = true;
            if(ans->outputs == NULL)
            {
                ans->outputs = vari;
                curr_entry = vari;
            }

            else{
                curr_entry->next = vari;
                curr_entry = curr_entry->next;
            } 

            temp = temp->nextSibling;    
        }
        
        else if(temp->tokenId == nt_N2)
        {
            temp = temp->leftMostChild;
        }
        else
            temp = temp->nextSibling;
        
    }

    return ans;

}

// <idList> => ID <N3>
// <N3> => COMMA ID <N3>
// <N3> => EPS
// root->tokenId = nt_idList
void insertIdList(tNode* root,var_type type,scopeNode* head){
    //variable_entry* var = (variable_entry*)malloc(sizeof(variable_entry));
    tNode* temp = root->leftMostChild;

    while(temp!=NULL){
        if(temp->tokenId == t_ID){
            variable var;
            strcpy(var.name,temp->lexeme);
            var.type = type;
            var.type.print = true;
            var.line = temp->lineNo;
            var.scope = curr_scope;
            var.offset = curr_offset;
            var.isInput = false;
            var.isOutput = false;
            if(type.arr){
                if(type.isEndId || type.isStartId){
                    curr_offset+=1;
                }
                else
            	    curr_offset += size[type.type]*(type.arr_end - type.arr_start +1) + 1;
            	
            }
            else if(type.type == INTEGER)
                curr_offset += SIZE_OF_INT;
            else if(type.type == REAL)
                curr_offset += SIZE_OF_REAL;
            else if(type.type == BOOLEAN)
                curr_offset += SIZE_OF_BOOL;         
            varInsert(var,head);
            temp = temp->nextSibling;
        }

        else if(temp->tokenId == nt_N3)
            temp = temp->leftMostChild;
    
    }
}
//<moduleDeclaration> => DECLARE MODULE ID SEMICOL
//root->tokenId is t_ID
void addModule(tNode* root,scopeNode* curr,scopeNode* tableRoot){
    module mod;
    strcpy(mod.name,root->lexeme);
    mod.defined = false;
    mod.inputs = NULL;
    mod.outputs = NULL;
	mod.lineDec = root->lineNo;
	mod.lineDef = -1;
    mod.width = 0;

    for(int i = 0 ; i < tableRoot->no_of_children ; i++)
    {
        if(strcmp(tableRoot->children[i]->module.name,mod.name) == 0)
        {
            symbolTableErrors++;
            fprintf(errorFile,"Line %d: Module %s already declared\n",mod.lineDec,mod.name);
            //printf("REDECLARATION NOT ALLOWED\n");
            return;
        }
    }

    scopeNode* temp = (scopeNode*)malloc(sizeof(scopeNode));
    temp->no_of_children = 0;
    temp->parent = tableRoot;
    temp->module = mod;
    temp->type = MODULE;
    temp->scope_start = temp->scope_end = -1;
    for(int i = 0 ; i < HASH_TABLE_SIZE ; i++)
    {
        temp->var_Hash[i] = NULL;
    }

    tableRoot->no_of_children++;
    tableRoot->children = realloc(tableRoot->children,tableRoot->no_of_children*sizeof(scopeNode*));
	tableRoot->children[tableRoot->no_of_children-1] = temp;

    
}

//We add to symbol table only for rules that have DECLARE,DRIVERDEF,DEF,FOR,WHILE or SWITCH because on these lead to new scopes or variables.
scopeNode* createSymbolTable(tNode* root){

    //scopeNode* ans = NULL;	CORRECTION: ans had no role in this function
    //scopeNode* curr = ans;   CORRECTION: made curr global as createSymbolTable is recursive
    
    //<moduleDeclaration> => DECLARE MODULE ID SEMICOL
    if(root->tokenId == t_ID && root->parent->tokenId == nt_moduleDeclarations){//correction made
        addModule(root,curr,tableRoot);
    }

    //<driverModule> => DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>
    else if(root->tokenId == nt_moduleDef && root->parent->tokenId == nt_program){ 
        tNode* temp = root->leftMostChild;  //temp->tokenId = t_START
        module mod;
        mod.width = 0;
        mod.defined = true;
        strcpy(mod.name,"DRIVER");
        mod.inputs = NULL;
        mod.outputs = NULL;
		mod.lineDef = root->leftMostChild->lineNo;
        mod.lineDec = -1;
        curr_offset = 0;
        int start_line,end_line;
        start_line = temp->lineNo;
        if(temp->nextSibling->tokenId == t_END)
            end_line = temp->nextSibling->lineNo;
        else
            end_line = temp->nextSibling->nextSibling->lineNo;    

        insertModuleInputOutput(mod,start_line,end_line);
        curr_offset = 0;    
    }

    //<module> => DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
    else if(root->tokenId == nt_module){
        module mod;
        mod.width = 0;
        mod.defined = true; //corrrection made: changed false to true
        strcpy(mod.name,root->leftMostChild->lexeme);
        tNode* in = root->leftMostChild->nextSibling;
        tNode* out = in->nextSibling;
        if(out->tokenId!=nt_output_plist)
            out = NULL;
        curr_offset = 0;
        mod.inputs = getInputList(in);
        mod.outputs = getOutputList(out);
        int start,end;
        tNode* temp = root->leftMostChild;
        while(temp!=NULL && temp->tokenId!=nt_moduleDef) 
            temp = temp->nextSibling;
        start = temp->leftMostChild->lineNo;    
        if(temp->leftMostChild->nextSibling->tokenId == t_END)
            end = temp->leftMostChild->nextSibling->lineNo;
        else
            end = temp->leftMostChild->nextSibling->nextSibling->lineNo;
        mod.lineDef = root->leftMostChild->lineNo;
        mod.lineDec = -1;
		insertModuleInputOutput(mod,start,end); 
        curr_offset = 0;
    }

    //<declareStmt> => DECLARE <idList> COLON <dataType> SEMICOL
    else if(root->tokenId == nt_declareStmt){
        id_type id = extractType(root->leftMostChild->nextSibling);
        var_type var;
        if(id == INTEGER || id == BOOLEAN || id == REAL){
            var.arr = false;
            var.arr_end = var.arr_start = -1;
            var.type = id;
            var.print = true;
        }

        else if(id == ARRAY){
            var = extractArrayDimensions(root->leftMostChild->nextSibling);
            var.print = true;
        }
        curr_scope++;
        insertIdList(root->leftMostChild,var,curr); //correction: changed last argument from ans to curr
        curr_scope--;
    }

//     //<condionalStmt> => SWITCH BO ID BC START <caseStmts><Default> END
//     else if(root->tokenId == t_SWITCH){
//         scopeNode* temp = (scopeNode*)malloc(sizeof(scopeNode));
//         temp->type = SWITCH;
//         temp->no_of_children = 0;
//         temp->parent = curr;
//         curr->children = (scopeNode*)realloc(curr->children,(curr->no_of_children + 1)*sizeof(scopeNode));
//         curr->no_of_children += 1;
//         temp->no_of_children = 0;
//         temp->scope_start = root->nextSibling->nextSibling->nextSibling->nextSibling->lineNo;
//         temp->scope_end = root->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->lineNo;
//         curr = temp;
//         curr_scope++;
//     }

//     //<iterativeStmt> => FOR BO ID IN <range> BC START <statements> END
//     else if(root->tokenId == t_FOR){
//         scopeNode* temp = (scopeNode*)malloc(sizeof(scopeNode));
//         temp->type = FOR;
//         temp->no_of_children = 0;
//         temp->parent = curr;
//         curr->children = (scopeNode*)realloc(curr->children,(curr->no_of_children + 1)*sizeof(scopeNode));
//         curr->no_of_children += 1;
//         temp->no_of_children = 0;
//         temp->scope_start = root->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->lineNo;
//         temp->scope_end = root->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->lineNo;
//         curr = temp;
//         curr_scope++;
//     }

//     //<iterativeStmt> => WHILE BO <arithmeticOrBooleanExpr> BC START <statements> END
//     else if(root->tokenId == t_WHILE){
//         scopeNode* temp = (scopeNode*)malloc(sizeof(scopeNode));
//         temp->type = WHILE;
//         temp->no_of_children = 0;
//         temp->parent = curr;
//         curr->children = (scopeNode*)realloc(curr->children,(curr->no_of_children + 1)*sizeof(scopeNode));
//         curr->no_of_children += 1;
//         temp->no_of_children = 0;
//         temp->scope_start = root->nextSibling->nextSibling->nextSibling->nextSibling->lineNo;
//         temp->scope_end = root->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->lineNo;
//         curr = temp;
//         curr_scope++;
//     }
	
	//FOR and WHILE
	else if(root->tokenId == nt_iterativeStmt){
		
		scopeNode* temp = (scopeNode*)malloc(sizeof(scopeNode));
		//FOR statement
		if(root->leftMostChild->tokenId == t_ID){
			temp->type = FOR;
            temp->module = curr->module;
            temp->scope_start = root->leftMostChild->nextSibling->nextSibling->lineNo;
			temp->scope_end = root->leftMostChild->nextSibling->nextSibling->nextSibling->nextSibling->lineNo;
		}
		//WHILE statement
		else if(root->leftMostChild->tokenId == nt_arithmeticOrBooleanExpr){
			temp->type = WHILE;
            temp->module = curr->module;
            temp->scope_start = root->leftMostChild->nextSibling->lineNo;
			temp->scope_end = root->leftMostChild->nextSibling->nextSibling->nextSibling->lineNo;
		}
		
		else {
            symbolTableErrors++;
			printf("INCORRECT AST\nEXITING\n");
			return tableRoot;
		}
		
		for(int i = 0 ; i < HASH_TABLE_SIZE ; i++)
        	temp->var_Hash[i] = NULL;
		
		temp->no_of_children = 0;
        temp->parent = curr;
        if(curr->no_of_children == 0)
            curr->children = malloc(sizeof(scopeNode*));
        else    
            curr->children = realloc(curr->children,(curr->no_of_children + 1)*sizeof(scopeNode*));
        curr->children[curr->no_of_children] = temp;
        curr->no_of_children += 1;
        temp->no_of_children = 0;
        curr = temp;
        curr_scope++;
	}
	//SWITCH statement
    else if(root->tokenId == nt_condionalStmt){
        scopeNode* temp = (scopeNode*)malloc(sizeof(scopeNode));
        temp->type = SWITCH;
        temp->module = curr->module;
        temp->scope_start = root->leftMostChild->nextSibling->lineNo;
        
        if(root->leftMostChild->nextSibling->nextSibling->nextSibling->tokenId == t_END)
            temp->scope_end = root->leftMostChild->nextSibling->nextSibling->nextSibling->lineNo;
        else if(temp->scope_end = root->leftMostChild->nextSibling->nextSibling->nextSibling->nextSibling->tokenId == t_END)
            temp->scope_end = root->leftMostChild->nextSibling->nextSibling->nextSibling->nextSibling->lineNo;    
        for(int i = 0 ; i < HASH_TABLE_SIZE ; i++)
        	temp->var_Hash[i] = NULL;
		
		temp->no_of_children = 0;
        temp->parent = curr;
        if(curr->no_of_children == 0)
            curr->children = malloc(sizeof(scopeNode*));
        else    
            curr->children = realloc(curr->children,(curr->no_of_children + 1)*sizeof(scopeNode*));
        curr->children[curr->no_of_children] = temp;
        curr->no_of_children += 1;
        temp->no_of_children = 0;
        curr = temp;
        curr_scope++;
    }
    else if(root->tokenId == t_END  && (root->parent->tokenId != nt_moduleDef))
	{
		curr = curr->parent;
		curr_scope--;
	}
    
    root->scope = curr;
    tNode* temp = root->leftMostChild;
    while(temp!=NULL){
        createSymbolTable(temp);
        temp = temp->nextSibling;
    }
	
	return tableRoot;
}


void printSymbolTable(scopeNode* root){

	scopeNode* temp = root;
	
	if(temp == NULL)
		return ;
	
	
	
	/*if(temp->no_of_children == 0)
	{
		for(int i = 0 ; i < HASH_TABLE_SIZE ; i++)
		{
			if(temp->var_Hash[i]!=NULL)
			{
				variable_entry* curr = temp->var_Hash[i];
				while(curr!=NULL)
				{
					printf("%s\t%s\t%d to %d\t\n",curr->var.name,temp->module.name,temp->scope_start,temp->scope_end);
					curr = curr->next;
				}
			}
		}
	}*/
	

	for(int i = 0 ; i < temp->no_of_children ; i++)
	{
		if(temp->children[i]){
			printSymbolTable(temp->children[i]);
        }
	}
	
    for(int i = 0 ; i < HASH_TABLE_SIZE ; i++)
		{
			if(temp->var_Hash[i]!=NULL)
			{
				variable_entry* curr = temp->var_Hash[i];
				while(curr!=NULL)
				{
                    char str[100];
					if(curr->var.type.arr){
                        strcpy(str,curr->var.type.startId);
                        strcat(str," to ");
                        strcat(str,curr->var.type.endId);
                    }
                    else{
                        strcpy(str,"-----");
                    }

                    int width = 0;
                    if((curr->var.type.type == REAL || curr->var.type.type == BOOLEAN || curr->var.type.type == INTEGER) && curr->var.type.arr == false){
                        width = size[curr->var.type.type];
                    }
                    else if(curr->var.type.arr && curr->var.isInput){
                        width = 1 + 2*size[INTEGER];
                    }
                    else if(curr->var.type.arr && (curr->var.type.isStartId || curr->var.type.isEndId))
                        width = 1;
                    else if(curr->var.type.arr){
                        width = size[curr->var.type.type]*(curr->var.type.arr_end - curr->var.type.arr_start +1) + 1;
                    }
                    //printf("%s\t%s\t\t%s\t\t%s\t\t%-17s%s\t\t%-25s %d to %d\t\t%d\t\t%d\n",curr->var.name,type[curr->var.type.type],curr->var.type.arr==true ? "YES":"NO",(curr->var.type.arr==true && (curr->var.type.isStartId == true || curr->var.type.isEndId == true )) ? "Dynamic ": (curr->var.type.arr == true ? "Static" : "-----"),str,scope[temp->type],temp->module.name,temp->scope_start,temp->scope_end,curr->var.scope,curr->var.offset);
					if(curr->var.type.print){
                        if(curr->var.isInput!=true && curr->var.isOutput!=true)
                            temp->module.width += width;
                        printf("%-10s\t%-20s\t%d to %-11d %-5d\t%-10s\t%-10s\t%-10s\t%-10s\t%-10d\t%-10d\n",curr->var.name,temp->module.name,temp->scope_start,temp->scope_end,width,curr->var.type.arr==true ? "YES":"NO",(curr->var.type.arr==true && (curr->var.type.isStartId == true || curr->var.type.isEndId == true )) ? "Dynamic ": (curr->var.type.arr == true ? "Static" : "-----"),str,type[curr->var.type.type],curr->var.offset,curr->var.scope);
                    }
                    //printf("%s\t%d\t%d\t%d\n",temp->module.name,temp->module.lineDec,temp->module.lineDef,curr->var.line);
                    curr = curr->next;
				}
			}
		}
}

void printArrays(scopeNode* root){
    scopeNode* temp = root;
	
	if(temp == NULL)
		return ;
	
	for(int i = 0 ; i < temp->no_of_children ; i++)
	{
		if(temp->children[i]){
			printArrays(temp->children[i]);
        }
	}
	
    for(int i = 0 ; i < HASH_TABLE_SIZE ; i++)
		{
			if(temp->var_Hash[i]!=NULL)
			{
				variable_entry* curr = temp->var_Hash[i];
				while(curr!=NULL)
				{
                    char str[100];
					if(curr->var.type.arr){
                        strcpy(str,curr->var.type.startId);
                        strcat(str," to ");
                        strcat(str,curr->var.type.endId);

                        printf("%-20s\t%d to %-10d %-10s\t%-10s\t%-10s\t%-10s\n",temp->module.name,temp->scope_start,temp->scope_end,curr->var.name,(curr->var.type.isStartId == true || curr->var.type.isEndId == true ) ? "Dynamic ":"Static",str,type[curr->var.type.type]);
                    }

                    curr = curr->next;
				}
			}
		}
}

void printActivationRecords(scopeNode* root){
    scopeNode* temp = root;

    if(temp==NULL)
        return ;

    for(int i = 0 ; i < temp->no_of_children ; i++){
        if(temp->children[i]){
            printActivationRecords(temp->children[i]);
        }
    }
    
    for(int i = 0 ; i < temp->no_of_children ; i++)
	{
		if(temp->children[i]){
            temp->module.width += temp->children[i]->module.width;
        }
	}

    

    if(temp->type == MODULE || temp->type == DRIVER)    
        printf("%-20s\t\t%d\n",temp->module.name,temp->module.width);    
}
