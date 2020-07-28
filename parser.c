/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/


#include <stdio.h>
#include "parserDef.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "lexer.h"

char* table[] = {"EPS", "$", "AND", "ARRAY", "ASSIGNOP", "BC", "BO", "BOOLEAN", "BREAK", "CASE", "COLON", "COMMA", "DECLARE", "DEF", "DEFAULT", "DIV", "DRIVER", "DRIVERDEF", "DRIVERENDDEF", "END", "ENDDEF", "EQ", "FALSE", "FOR", "GE", "GET_VALUE", "GT", "ID", "IN", "INPUT", "INTEGER", "LE", "LT", "MINUS", "MODULE", "MUL", "NE", "NUM", "OF", "OR", "PARAMETERS", "PLUS", "PRINT", "PROGRAM", "RANGEOP", "REAL", "RETURNS", "RNUM", "SEMICOL", "SQBC", "SQBO", "START", "SWITCH", "TAKES", "TRUE", "USE", "WHILE", "WITH", "AnyTerm", "arithmeticExpr", "arithmeticOrBooleanExpr", "assignmentStmt", "boolConstt", "caseStmts", "condionalStmt", "dataType", "declareStmt", "Default", "driverModule", "expression", "factor", "idList", "Index", "input_plist", "ioStmt", "iterativeStmt", "logicalOp", "lvalueIDStmt", "lvalueARRStmt", "module", "moduleDeclaration", "moduleDeclarations", "moduleDef", "moduleReuseStmt", "new_NT", "N1", "N2", "N3", "N4", "N5", "N7", "N8", "N9", "op1", "op2", "optional", "otherModules", "output_plist", "program", "range", "range_arrays", "relationalOp", "ret", "simpleStmt", "statement", "statements", "term", "type", "value", "var", "var_id_num", "whichId", "whichStmt", "unary_op", "U", "IDF", "minusExpr", "undec" };//after t_IDF, for convenience exceptions;
FILE* errorFile;
int tableEntry(char* str) {
    for(int i = 0; i <= tot_tnt; i++) {
        if(strcmp(table[i],str)==0){
            return i;
        }
    }
    fprintf(errorFile,"Error: %s not found\n",str);
    return -1;
}

tNode* initTreeNode(tnt tokenId){
	tNode* treenode = (tNode*)malloc(sizeof(tNode));
    if(tokenId == t_EPSILON){
        strcpy(treenode->lexeme, "epsilon");
    }
	treenode->leftMostChild = NULL;
	treenode->nextSibling = NULL;
	treenode->parent = NULL;
	treenode->visited = 0;
	treenode->ruleUsed = -1;
	treenode->astName = undec;
	treenode->tokenId = tokenId;
	return treenode;
}

node* push_stack(node* head, tNode* data){
    node* temp;
    temp = (node*) malloc(sizeof(node));
    temp->data = data;
    temp->next = head;
    return temp;
}

node* pop_stack(node* head){
    node* temp = head;
    if(head==NULL)
        return head;
    head = head->next;
    temp->next = NULL;
    free(temp);
    return head;
}

int indexNonTerm(tnt a){//Used to get the index corresponding to the parsetable for non-terminals
	return (int)(a-NUM_T);
}

int isTerm(tnt t){ //Checks if the symbol is a terminal. Hardcoded with last terminal
	return t<=t_WITH;				//may need to make better somehow
}

rule* initRule(tnt LHS,	tnt *RHS, int length_RHS){//Used for initializing the rules.
	int i;
	rule* r = (rule*)malloc(sizeof(rule));
	r->LHS = LHS;
	r->RHS = (tnt*)malloc(sizeof(tnt)*length_RHS);
	r->length_RHS = length_RHS;	
	for(i=0;i<length_RHS;i++){
		r->RHS[i]=RHS[i];
	}
	return r;
}


void populateRules(rule** rules, char* filename) {
    FILE *fptr = fopen(filename, "r");
    char buff[30];
    int len_rhs;
    tnt *rhsArr = (tnt*)malloc(15*sizeof(tnt)); //assuming rhs has no more than 15 terms
    for(int i = 0; i < no_rules; i++) {
        fscanf(fptr,"%s",buff);
        tnt lhs = (tnt)tableEntry(buff);
        len_rhs = 0;
        while(1){
            fscanf(fptr,"%s",buff);
            if(strcmp(buff,".")==0)
                break;
            if(tableEntry(buff) != -1){
                rhsArr[len_rhs] = (tnt)tableEntry(buff);
                len_rhs++;
            }    
        }
        rules[i] = initRule(lhs,rhsArr,len_rhs);
        //free(rhsArr);
    }
}


unsigned long long findFIRSTstring(tnt string_arr[], int size_string, term_sets** arr){//Returns the FIRST set of string_arr 
	int i;
	unsigned long long FIRST_string = 0;
	for(i=0;i<size_string;i++){
		FIRST_string = FIRST_string | arr[string_arr[i]]->FIRST;  //union
		if((arr[string_arr[i]]->FIRST & (1ULL << t_EPSILON)) != 0){ //epsilon present
			if(i!=(size_string-1)) 
                FIRST_string = FIRST_string & (~(1ULL << t_EPSILON));
			continue;
		}
		else{
			break;
		}
	}
	return FIRST_string;
}


void populateFIRST(rule** rules, term_sets** arr){//Populates the FIRST sets for all the symbols present in the grammar
	int i,flag=1;

	for(i=0;i<no_tnt;i++){
		if(isTerm(arr[i]->token)){
			arr[i]->FIRST = arr[i]->FIRST | (1ULL << arr[i]->token); //adding element
		}
	}

	int didFIRSTchange = 1;
	while(didFIRSTchange){//Loops until there is no change in any of the FIRST sets
		didFIRSTchange = 0;
		for(i=0;i<no_rules;i++){
			int j;
			tnt X = rules[i]->LHS;
			tnt* Y = rules[i]->RHS;
			int no_right = rules[i]->length_RHS;
			unsigned long long current_FIRST = arr[X]->FIRST;
			unsigned long long new_FIRST = current_FIRST;
			for(j=0;j<no_right;j++){
				new_FIRST = new_FIRST | arr[Y[j]]->FIRST;
				if((arr[Y[j]]->FIRST & (1ULL << t_EPSILON)) != 0){ //If first contains epsilon, check for the rest of the string
					if((current_FIRST & (1ULL << t_EPSILON)) == 0){
						new_FIRST = new_FIRST & (~(1ULL << t_EPSILON)); //In case Epsilon was not present but is added
					}
					continue;
				}
				else{
					break;
				}
			}

			if(Y[0] == t_EPSILON){ //Checking for rule X->epsilon and adding epsilon to FIRST(X)
				new_FIRST = current_FIRST | (1ULL << t_EPSILON); //adding epsilon
				arr[X]->FIRST = new_FIRST;
			}
            else{
				for(j=0;j<no_right;j++){
					if((arr[Y[j]]->FIRST & (1ULL << t_EPSILON)) == 0){
						flag = 0;
						break;
					}
				}
				if(flag){ 
					new_FIRST = current_FIRST | (1ULL << t_EPSILON); //adding epsilon
					arr[X]->FIRST = new_FIRST;
				}
			}

			arr[X]->FIRST = new_FIRST;
			if((current_FIRST ^ new_FIRST) != 0){
				didFIRSTchange = 1;
			}
		}
	}
}

void populateFOLLOW(rule** rules,term_sets** arr){//Populates the follow sets for the non-terminals in the grammar
	arr[nt_program]->FOLLOW = arr[nt_program]->FOLLOW | (1ULL << t_DOLLAR); //add element
	int didFOLLOWchange = 1,i;
	while(didFOLLOWchange){
		didFOLLOWchange=0;
		for(i=0;i<no_rules;i++){
			int j;
			tnt X = rules[i]->LHS;
			tnt* Y = rules[i]->RHS;
			int no_right = rules[i]->length_RHS;
			int remaining = no_right;
			for(j=0;j<no_right-1;j++){ //Handle last waala
				remaining--;
				if(isTerm(Y[j])){
					continue;
				}
				tnt Z = Y[j];
				unsigned long long newFOLLOW = arr[Z]->FOLLOW;
				unsigned long long FIRSTString = findFIRSTstring(&Y[j+1],remaining,arr);



				newFOLLOW = newFOLLOW | FIRSTString; //union
				if((FIRSTString & (1ULL << t_EPSILON)) != 0){ //epsilon present
					newFOLLOW = newFOLLOW | arr[X]->FOLLOW; //union
				}
				newFOLLOW = newFOLLOW & (~(t_EPSILON)); //remove epsilon
				if((newFOLLOW ^ arr[Z]->FOLLOW) != 0){
					didFOLLOWchange=1;
					arr[Z]->FOLLOW = newFOLLOW;
				}

			}
			tnt Z = Y[no_right-1];
			if(!(isTerm(Z))){
				unsigned long long newFOLLOW = arr[Z]->FOLLOW;
				newFOLLOW = newFOLLOW | arr[X]->FOLLOW; //union
				newFOLLOW = newFOLLOW & (~(t_EPSILON)); //remove epsilon
				if((newFOLLOW ^ arr[Z]->FOLLOW) != 0){
					didFOLLOWchange=1;
					arr[Z]->FOLLOW = newFOLLOW;
				}

			}


		}
	}
}


void populateParseTable(int** parseTable, rule** rules, term_sets** arr){ //fix for epsilon at the end
	int i,j;
	for(i=0;i<no_rules;i++){
		tnt A = rules[i]->LHS;
		tnt* alpha = rules[i]->RHS;
		int sizeAlpha = rules[i]->length_RHS;
		unsigned long long FIRSTAlpha = findFIRSTstring(alpha,sizeAlpha,arr);
		for(j=0;j<NUM_T;j++){
			if((FIRSTAlpha & (1ULL << j)) != 0){ //terminal j present
                parseTable[indexNonTerm(A)][j] = i;
			}
		}
		if((FIRSTAlpha & (1ULL << t_EPSILON)) != 0){ //epsilon present
			parseTable[indexNonTerm(A)][t_EPSILON] = -1;
			unsigned long long FOLLOWA = arr[A]->FOLLOW;
			for(j=0;j<NUM_T;j++){
				if((FOLLOWA & (1ULL << j)) != 0){ //terminal j present
					parseTable[indexNonTerm(A)][j] = i;
				}	
			}
		} 
	}
}

int** initParseTable(int** parseTable){
    parseTable = (int**)malloc(sizeof(int*)*NUM_NT);
    for(int i=0;i<NUM_NT;i++){
        parseTable[i] = (int*)malloc(sizeof(int)*NUM_T);
    }
    for(int i=0;i<NUM_NT;i++){
        for(int j=0;j<NUM_T;j++){
            parseTable[i][j]=-1; //initializing parse table
        }
    }
    return parseTable;
}


tNode* parseInputSourceCode(char *testcaseFile, int** parseTable, rule** rules,term_sets** arr,FILE* ptr){//Used for parsing the input source code based on the parsetable
    parserErrors = 0;
    initializeLexer(testcaseFile,ptr);
	tNode* treeRoot = initTreeNode(nt_program);
	node* stackTop = NULL;
	stackTop = push_stack(stackTop, initTreeNode(t_DOLLAR));
	stackTop = push_stack(stackTop,treeRoot);
	Token* currentToken = getNextToken();
	tnt a = currentToken->tokenId;
	errorFile = ptr;
	// fprintf(errorFile,"\n\nParsing Errors:\n");
	
    while(stackTop!=NULL){	
		tnt X = stackTop->data->tokenId;

		if(X==a){
			strcpy(stackTop->data->lexeme,currentToken->lexeme);
			stackTop->data->lineNo = currentToken->lineNo;
			stackTop = pop_stack(stackTop);
			currentToken = getNextToken();
			a = currentToken->tokenId;
		}

		else if(isTerm(X)){
			parserErrors+=1;
			// printf("%s %s %lu\n",emp,currentToken->lexeme,lineNo);
			fprintf(errorFile,"Line %d: The token %s for lexeme %s does not match. The expected token here is %s\n",line,table[a],currentToken->lexeme,table[X]);
			stackTop = pop_stack(stackTop);
			continue;
		}

		else if(parseTable[indexNonTerm(X)][a]==-1){
			parserErrors+=1;
			// printf("%s %s %lu\n",emp,currentToken->lexeme,lineNo);
			fprintf(errorFile,"Line %d: The token %s for lexeme %s does not match. The expected token here is %s\n",line,table[a],currentToken->lexeme,table[X]);
			//printf("Entering Panic Mode \n");
			unsigned long long syncSet;
			syncSet = arr[X]->FOLLOW;
			while((syncSet & (1ULL << a)) == 0){  //terminal a not present
				currentToken = getNextToken();
				a = currentToken->tokenId;
			}
			stackTop = pop_stack(stackTop);
			//printf("Exiting Panic Mode\n");
			continue;

		}
		else{
		
			int rule_no = parseTable[indexNonTerm(X)][a];
			// printRule(rules[rule_no]);
			node* poppedNode = stackTop;
			tNode* poppedData = stackTop->data;
			stackTop = pop_stack(stackTop);
			tNode* curr=NULL; 
            tNode* prev=NULL;
			for(int i=((rules[rule_no]->length_RHS)-1);i>=0;i--){

				if(rules[rule_no]->RHS[i]!=t_EPSILON){
					//printf("parameter to initTreenode: %d\n",rules[rule_no]->RHS[i]);
					curr = initTreeNode(rules[rule_no]->RHS[i]);
					curr->parent = poppedData;
					curr->nextSibling = prev;
					stackTop = push_stack(stackTop,curr);
					prev = curr;

				}
				else{
					curr = initTreeNode(t_EPSILON);
					curr->parent = poppedData;
				}	
			}
			poppedData->leftMostChild = curr;
			poppedData->ruleUsed = rule_no;
			//poppedData->astName = getASTName(poppedData->tokenId,rule_no);
		}
	}
	if((stackTop==NULL)&&(parserErrors==0)){
		printf("Input source code is syntactically correct...........\n");
	}
	else{
		printf("Compilation terminated with %d syntactic error(s)\n",parserErrors);
	}

	fclose(fptr);
	return treeRoot;
}


void printRules(rule** rules){
    printf("\n\nGRAMMAR\n");
    for(int i=0; i < no_rules; i++) {
        printf("%s : ", table[rules[i]->LHS]);
        for(int j=0; j<rules[i]->length_RHS;j++){
            printf("%s ", table[rules[i]->RHS[j]]);
        }
        printf("\n");
    }
}

void printSetNames(unsigned long long set1){ //Prints all elements that belong to the set set1
	int i;
	//unsigned long long j=1;
	for(i=0;i<NUM_T;i++){
		if((set1 & (1ULL << i)) != 0){
			printf("%s ",table[i]);
		}
	}
	printf("\n");
	return;
}

void printFirstAndFollow(term_sets** arr){
    printf("\n\n\n");
    printf("FIRST SET\n");
    for(int i = 0; i<no_tnt;i++){
        if(!isTerm(arr[i]->token)){
            printf("%s : ", table[arr[i]->token]);
            printSetNames(arr[i]->FIRST);
        }
    }
    printf("\n\n\n");
    printf("FOLLOW SET\n");
    for(int i = 0; i<no_tnt;i++){
        if(!isTerm(arr[i]->token)){
            printf("%s : ", table[arr[i]->token]);
            printSetNames(arr[i]->FOLLOW);
        }
    }
    printf("\n\n\n");
}


void printParseTree(tNode* head) {
    int first = 0;
    tNode *child;
    child = head->leftMostChild;

    // NOT a leaf node
    if (child != NULL)
    {
        while (child != NULL)
        {
            printParseTree(child);
            if (first == 0)
            {
                // printing a non terminal
                if (head->parent != NULL)
                    printf("---\t\t\t---\t\t---\t\t\t---\t%-20s\tNO\t\t\t\t%-20s\n", table[head->parent->tokenId], table[head->tokenId]);
                else
                    printf("---\t\t\t---\t\t---\t\t\t---\tROOT\t\t\tNO\t\t\t\t%-20s\n", table[head->tokenId]);
                first = 1;
            }
            child = child->nextSibling;
        }
    }
    // A leaf node
    else
    {
        if (isTerm(head->tokenId))
        {
            // with the token
            if (head->tokenId == t_NUM || head->tokenId == t_RNUM)
                printf("%-20s\t%-10d\t%-20s\t%-20s\t%-20s\tYES\t\t\t\t---\n", head->lexeme, head->lineNo, table[head->tokenId], head->lexeme, table[head->parent->tokenId]);
            else
                printf("%-20s\t%-10d\t%-20s\t---\t%-20s\tYES\t\t\t\t---\n", head->lexeme, head->lineNo, table[head->tokenId], table[head->parent->tokenId]);
        }
        else
            // printf("terminal node : %d\n", head->id);
            printf("---\t---\t%s\t---\t%s\tYES\t---\n", table[head->tokenId], table[head->parent->tokenId]);
    }
}   

long int countParseTree(tNode* root){
	if(root==NULL){
		return 0;
	}
	else{
		long int answer = 0;
		answer = 1;
		tNode* temp = root->leftMostChild;
		while(temp!=NULL){
			answer = answer + countParseTree(temp);
			temp = temp->nextSibling;
		}
		return answer;
	}
}
