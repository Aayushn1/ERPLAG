/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/



#include "sym.h"
#include "typechecker.h"

int size1[] = {SIZE_OF_REAL, SIZE_OF_INT, SIZE_OF_BOOL};
FILE* fasm;
int label;
int number;


void gen_prog(tNode* aroot , scopeNode* tableRoot);

int get_int(char *arr){
    int num = 0;
    for(int i=0 ; arr[i]!='\0' ; i++) {
        num = num*10 + ((int)arr[i]-'0');
    }
    return num;
}

// generates a new temporary variable name
void get_var_name(char* name, tNode* a){
	strcpy(name , "l");
	variable_entry* vt = get_var_ptr(a->lexeme,a->scope,a->lineNo);
	strcat(name,a->lexeme);
	char line[50];
	sprintf(line , "%d" , vt->var.line);
	strcat(name, line);
}

void get_var_name1(char* name, char* idname, int lineNo){
	strcpy(name , "l");
	strcat(name,idname);
	char line[50];
	sprintf(line , "%d" , lineNo);
	strcat(name, line);
}


void left(tNode* a) {
	if(a->type == INTEGER){
		tNode * temp = a->parent->leftMostChild;
		if(temp != a) {
			while(temp && temp->nextSibling != a){
				temp = temp->nextSibling;
			}
		}

		if(temp->tokenId == t_MINUS)
			fprintf(fasm,"mov ax,dx\nneg ax\nmovsx rax,ax\npush rax\n");
		
		else if(temp->tokenId == t_DIV) {
			fprintf(fasm,"pop rax\nmov si,dx\ncwd\nidiv si\npush rax\n");	//sunn nagpal kal karte hai ab ok
		}

		else if(temp->tokenId == t_MUL){
			fprintf(fasm,"pop rax\nimul dx\npush rax\n");	//sunn nagpal kal karte hai ab ok
		}		//kya chal raha h? 
		else
			fprintf(fasm,"movsx rax,dx\npush rax\n");//nice
	}

	else if(a->type == BOOLEAN){
		tNode * temp = a->parent->leftMostChild;
		if(temp != a) {
			while(temp && temp->nextSibling != a){
				temp = temp->nextSibling;
			}
		}	
		
		if(temp->tokenId == t_AND) {
			fprintf(fasm,"pop rax\nand ax,dx\npush rax\n");
		}
		
		else if(temp->tokenId == t_OR) {
			fprintf(fasm,"pop rax\nor ax,dx\npush rax\n");
		}

		else
			fprintf(fasm,"mov al,dl\nmovsx rax,ax\npush rax\n");
	}
}



// <condionalStmt> => SWITCH BO ID BC START <caseStmts><Default> END
// <caseStmts> => CASE <value> COLON <statements> BREAK SEMICOL <N9>
// <N9> => CASE <value> COLON <statements> BREAK SEMICOL <N9>
// <N9> => EPS

void get_switch_id(char* name,tNode* a)
{
	while(a->tokenId != nt_condionalStmt)
		a = a->parent;
	get_var_name(name,a->leftMostChild);    // a->leftMostChild = ID
}

int get_switch_end(tNode* a)
{
	while(a->tokenId != nt_condionalStmt)
		a = a->parent;
	return a->scope->scope_end;
}

int getChildrenNumber(tNode* a){
    int ans = 0;
    tNode* temp = a->leftMostChild;
    while(temp!=NULL){
        ans++;
        temp = temp->nextSibling;
    }
    return ans;
}



void compose(tNode* a,id_type t){
	// printf("op symbol %d\n",a->tokenId);
	if(t == INTEGER){
		if(a->tokenId == t_PLUS){
			fprintf(fasm,"pop rax\nadd dx,ax\n"); 
		}

		else if(a->tokenId == t_MINUS) {
			if(! a->nextSibling->nextSibling){
				fprintf(fasm,"pop rax\nneg dx\nadd dx,ax\n");
			}
            else fprintf(fasm,"pop rax\nadd dx,ax\n");
        }

		else if(a->tokenId == t_MUL){
			if(!a->nextSibling->nextSibling)
				fprintf(fasm,"pop rax\nimul dx\nmov dx,ax\n");
		}

		else if(a->tokenId == t_DIV){
			if(!a->nextSibling->nextSibling) {
				fprintf(fasm,"pop rax\nmov si,dx\ncwd\nidiv si\nmov dx,ax\n");
			}
		}

		else if(a->tokenId == t_LT){
			fprintf(fasm,"pop rax\ncmp ax,dx\njl L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}

		else if(a->tokenId == t_LE){
			fprintf(fasm,"pop rax\ncmp ax,dx\njle L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}

		else if(a->tokenId == t_GT){
			fprintf(fasm,"pop rax\ncmp ax,dx\njg L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}

		else if(a->tokenId == t_GE){
			fprintf(fasm,"pop rax\ncmp ax,dx\njge L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}

		else if(a->tokenId == t_EQ){
			fprintf(fasm,"pop rax\ncmp ax,dx\nje L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}

		else if(a->tokenId == t_NE){
			fprintf(fasm,"pop rax\ncmp ax,dx\njne L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}
	}
	else if(t == BOOLEAN){
		if(a->tokenId == t_AND){
			fprintf(fasm,"pop rax\nand dl,al\n");
		}

		else if(a->tokenId == t_OR){
			fprintf(fasm,"pop rax\nor dl,al\n");
		}
	}
}


// this function is used to allocate memory for all the variables in the code
void gen_data(scopeNode* tableRoot) {
	if(tableRoot == NULL){
		// printf("Error: tableRoot is NULL\n");
		return;
	}
	
	for(int i = 0 ; i < HASH_TABLE_SIZE ; i++) {
		variable_entry* temp = tableRoot->var_Hash[i]; 
		while(temp != NULL) {
			variable v = temp->var;

            // if current variable is an array
            // find size and reserve space for it
			if(v.type.arr) {
				if(v.type.arr_start != DynARRAY && v.type.arr_end != DynARRAY) {
					int noe = v.type.arr_end - v.type.arr_start + 1;
					// printf("noe : %d\n", noe);
					char name[50];
					strcpy(name , v.name);
					sprintf(name , "%s%d" , name, v.line);

					// resb, resw, resd is used to reserve memory for uninitialised variables
					if(v.type.type == INTEGER)      // word 
						fprintf(fasm , "l%s: resd %d\n" , name , noe);
					else if(v.type.type == REAL)    // double word
						fprintf(fasm , "l%s: resd %d\n" , name , noe);
					else if(v.type.type == BOOLEAN) // byte
						fprintf(fasm , "l%s: resb %d\n" , name , noe);

				}
				else {
					char name[50];
					strcpy(name , v.name);
					sprintf(name , "%s%d" , name, v.line);

					// resb, resw, resd is used to reserve memory for uninitialised variables
					if(v.type.type == INTEGER)      // word 
						fprintf(fasm , "l%s: resd %d\n" , name , 100);
					else if(v.type.type == REAL)    // double word
						fprintf(fasm , "l%s: resd %d\n" , name , 100);
					else if(v.type.type == BOOLEAN) // byte
						fprintf(fasm , "l%s: resb %d\n" , name , 100);
				}
			}

			else {
				char name[50];
				strcpy(name , v.name);
				sprintf(name , "%s%d" , name, v.line);

				if(v.type.type == INTEGER)
					fprintf(fasm , "l%s: resd 1\n" , name);
				else if(v.type.type == REAL)
					fprintf(fasm , "l%s: resd 1\n" , name);
				else if(v.type.type == BOOLEAN)
					fprintf(fasm , "l%s: resb 1\n" , name);
			}
			temp = temp->next;
		}
	}

	for(int i = 0 ; i < tableRoot->no_of_children ; i++)
		gen_data(tableRoot->children[i]);
}



// this is for <var_id_num> 
// var_id_num => NUM | ID <whichId>
// <whichId> => ID | NUM | EPS 
// when <whichId> != EPS then we are accessing an array element
// ID[NUM] | ID[ID]
// we store the value of variable in dx or dl register based on size
			

void boolUtil(tNode* aroot, scopeNode* tableRoot) {
	if(aroot->tokenId == t_TRUE) {
		fprintf(fasm, "mov dl,1\n");
	}
	else if(aroot->tokenId == t_FALSE) {
		fprintf(fasm, "mov dl,0\n");
	}
}

// <term> => <var_id_num> <N5> | NUM <N5> | RNUM <N5>
void varUtil(tNode* aroot, scopeNode* tableRoot) {
	if(aroot->tokenId == t_NUM) {
		fprintf(fasm,"mov dx,%s\n",aroot->lexeme);
	}

	else if(aroot->tokenId == t_RNUM) {
		fprintf(fasm,"mov edx,%s\n",aroot->lexeme);
	}

	// <var_id_num>
	// <var_id_num> => ID 
	// <var_id_num> => ID NUM | ID ID  //arrays ID [NUM] | ID [ID]
	else {
		char name[50];
		get_var_name(name, aroot->leftMostChild);
		id_type t = aroot->leftMostChild->type;
		tNode* idnode = aroot->leftMostChild;

		// <whichId> => EPS
		if(aroot->leftMostChild->nextSibling == NULL) {
			if(t == INTEGER)
				fprintf(fasm,"mov dx,word[%s]\n",name);
			else if(t == BOOLEAN)
				fprintf(fasm,"mov dl,byte[%s]\n",name);
			else if(t == REAL)
				fprintf(fasm,"mov edx,dword[%s]\n",name);
		}

		// the ID corresponds to an ARRAY variable name 
		else {
			variable_entry* vt = get_var_ptr(aroot->leftMostChild->lexeme,tableRoot,aroot->leftMostChild->lineNo);
			int index_low = vt->var.type.arr_start;
			int index_high = vt->var.type.arr_end;
			int index_val;
			if(aroot->leftMostChild->nextSibling->tokenId == t_NUM) {
				index_val = get_int(aroot->leftMostChild->nextSibling->lexeme);
				if(index_low != DynARRAY) {
					int offset = (index_val - index_low)*size1[(int)t];
					if(t == INTEGER)
						fprintf(fasm,"mov dx,word[%s+%d]\n",name,offset);
					else if(t == BOOLEAN)
						fprintf(fasm,"mov dl,byte[%s+%d]\n",name,offset);
					else if(t == REAL) 
						printf("REAL value found 3\n");
				}

				else {
					fprintf(fasm,"push rax\npush rcx\n");
					fprintf(fasm,"mov rax,%d\n",index_val);
					char name1[50];
					variable_entry* vt1 = get_var_ptr(vt->var.type.startId,idnode->scope,idnode->lineNo);
					get_var_name1(name1,vt->var.type.startId,vt1->var.line);
					fprintf(fasm,"movzx rcx,word[%s]\n",name1);
					fprintf(fasm,"sub rax,rcx\n");	
					if(aroot->leftMostChild->type == INTEGER)
						fprintf(fasm,"mov dx,word[%s+rax*%d]\n",name,size1[(int)t]);
					
					else if(aroot->leftMostChild->type == REAL)
						fprintf(fasm,"mov edx,dword[%s+rax*%d]\n",name,size1[(int)t]);
					
					else if(aroot->leftMostChild->type == BOOLEAN)
						fprintf(fasm,"mov dl,byte[%s+rax*%d]\n",name,size1[(int)t]);
					fprintf(fasm,"pop rcx\npop rax\n");
				}		
			}

			// ID[ID]
			// extract value of ID in []
			else {
				char ind_name[50];
				get_var_name(ind_name, aroot->leftMostChild->nextSibling);
				if(t == INTEGER)
					fprintf(fasm,"movzx rsi,word[%s]\n",ind_name);
				else if(t == BOOLEAN)
					fprintf(fasm,"movzx rsi,byte[%s]\n",ind_name);
				int curr_label = label;
				label += 3;
				// bound checking
				
				if(index_low != DynARRAY)
					fprintf(fasm,"cmp rsi,%d\n",index_low);
				else {
					fprintf(fasm,"push rcx\n");
					char name1[50];
					variable_entry* vt1 = get_var_ptr(vt->var.type.startId,idnode->scope,idnode->lineNo);
					get_var_name1(name1,vt->var.type.startId,vt1->var.line);
					// printf("name1: %s\n", name1);
					fprintf(fasm,"movzx rcx,word[%s]\n",name1);		// rcx contains index_low
					fprintf(fasm,"cmp rsi,rcx\n");
				}	
				fprintf(fasm,"jge L%d\njmp L%d\n",curr_label,curr_label+1);
				
				if(index_high != DynARRAY)
					fprintf(fasm,"L%d:\ncmp rsi,%d\n",curr_label,index_high);
				else {
					char name1[50];
					variable_entry* vt1 = get_var_ptr(vt->var.type.endId,idnode->scope,idnode->lineNo);
					get_var_name1(name1,vt->var.type.endId,vt1->var.line);
					fprintf(fasm,"L%d:\npush rax\n",curr_label);
					fprintf(fasm,"movzx rax,word[%s]\n",name1);		// rax contains index_high
					fprintf(fasm,"cmp rsi,rax\n");
				}
				
				fprintf(fasm,"jle L%d\n", curr_label+2);
				fprintf(fptr,"L%d:\nmov rdi,errormsg\n",curr_label+1);
				fprintf(fptr,"mov al,0\ncall printf\n");	
				fprintf(fasm,"mov ebx,0\nmov eax,1\nint 0x80\nL%d:\n",curr_label+2);

				if(index_low != DynARRAY)
					fprintf(fasm,"sub rsi,%d\n",index_low);
				else
					fprintf(fasm,"sub rsi,rcx\n");

				if(index_high == DynARRAY)
					fprintf(fasm,"pop rax\n");
				if(index_low == DynARRAY)
					fprintf(fasm,"pop rcx\n");

				if(aroot->leftMostChild->type == INTEGER)
					fprintf(fasm,"mov dx,word[%s+rsi*%d]\n",name,size1[(int)t]);	
				else if(aroot->leftMostChild->type == REAL)
					fprintf(fasm,"mov edx,dword[%s+rsi*%d]\n",name,size1[(int)t]);
				else if(aroot->leftMostChild->type == BOOLEAN)
					fprintf(fasm,"mov dl,byte[%s+rsi*%d]\n",name,size1[(int)t]);
			}
		}
	}
}

void boolPrintUtil() {
	fprintf(fasm,"cmp rsi,1\n");
	fprintf(fasm,"jne L%d\n",label);
	fprintf(fasm,"mov rdi,msgtrue\njmp L%d\n",label+1);
	fprintf(fasm,"L%d:\nmov rdi,msgfalse\nL%d:\n",label,label+1);
	label += 2;
}

tNode* findModule(char* name, tNode* currNode) {
	while(currNode->tokenId != nt_program) {
		currNode = currNode->parent;
	}
	
	bool found = false;
	tNode* progNode = currNode;
	currNode = currNode->leftMostChild;
	if(currNode != NULL && currNode->tokenId == nt_moduleDeclarations)
		currNode = currNode->nextSibling;
	if(currNode != NULL && currNode->tokenId == nt_otherModules) {
		tNode* otherModNode = currNode;
		while(1) {
			if(strcmp(name,otherModNode->leftMostChild->leftMostChild->lexeme) == 0) {
				found = true;
				return otherModNode->leftMostChild;
			}
			else if(otherModNode->leftMostChild->nextSibling != NULL){
				otherModNode = otherModNode->leftMostChild->nextSibling;
			}
			else {
				// printf("Not found in othermodule 1\n");
				break;
			}
		}
		currNode = currNode->nextSibling;	
	}
	if(currNode != NULL && currNode->tokenId == nt_moduleDef)
		currNode = currNode->nextSibling;
	if(currNode != NULL && currNode->tokenId == nt_otherModules) {
		tNode* otherModNode = currNode;
		while(1) {
			if(strcmp(name,otherModNode->leftMostChild->leftMostChild->lexeme) == 0) {
				found = true;
				return otherModNode->leftMostChild;
			}
			else if(otherModNode->leftMostChild->nextSibling != NULL){
				otherModNode = otherModNode->leftMostChild->nextSibling;
			}
			else {
				// printf("Not found in othermodule 2\n");
				break;
			}
		}
	}
	// printf("Lode lag gye\n");		
}


// aroot is nt_module
// <module> => ID <input_plist> <ret> <moduleDef>
void moduleUtil(tNode* aroot, scopeNode* tableRoot) {
	char name[50];
	strcpy(name,aroot->leftMostChild->lexeme);
	fprintf(fasm,"global %s\n",name);
	fprintf(fasm,"%s:\npush rbp\n",name);
	tNode* temp = aroot->leftMostChild;
	while(temp->tokenId != nt_moduleDef) {
		temp = temp->nextSibling;
	}
	gen_prog(temp,tableRoot);
	fprintf(fasm,"pop rbp\nret\n");
}

void input_plistUtil(tNode* aroot,char** inputList,int inputSize,bool* boolArr, tNode** nodeInputArr) {
	tNode* idNode = aroot->leftMostChild;
	for(int i = 0; i < inputSize; i++) {
		char name[50];
		// printf("%s, Line 375\n", idNode->lexeme);
		get_var_name(name,idNode);
		if(boolArr[i]) {
			// printf("input_plist ARRAY\n");
			// printf("%s, Line 479\n", idNode->lexeme);
			variable_entry* vt2 = get_var_ptr(idNode->lexeme,idNode->scope,idNode->lineNo);   // original
			variable_entry* vt = get_var_ptr(nodeInputArr[i]->lexeme, nodeInputArr[i]->scope,nodeInputArr[i]->lineNo);	// new

			char name_start_vt[50];
			char name_end_vt[50];
			char name_start_vt2[50];
			char name_end_vt2[50];

			if(vt->var.type.arr_start == DynARRAY) {
				variable_entry* vt1 = get_var_ptr(vt->var.type.startId,nodeInputArr[i]->scope,nodeInputArr[i]->lineNo);
				get_var_name1(name_start_vt,vt->var.type.startId,vt1->var.line);
				variable_entry* vt3 = get_var_ptr(vt2->var.type.startId,idNode->scope,idNode->lineNo);
				get_var_name1(name_start_vt2,vt2->var.type.startId,vt3->var.line);
			}

			if(vt->var.type.arr_end == DynARRAY) {
				variable_entry* vt1 = get_var_ptr(vt->var.type.endId,nodeInputArr[i]->scope,nodeInputArr[i]->lineNo);
				get_var_name1(name_end_vt,vt->var.type.endId,vt1->var.line);
				variable_entry* vt3 = get_var_ptr(vt2->var.type.endId,idNode->scope,idNode->lineNo);
				get_var_name1(name_end_vt2,vt2->var.type.endId,vt3->var.line);
			}
			// copy the values of startId and endId
			// from the passed array to input_plist array

			// rdx will store index_low
			// rcx will store index_high
			fprintf(fasm,"push rdx\npush rcx\n"); 
			if(vt->var.type.arr_start != DynARRAY) {
				fprintf(fasm,"mov rdx,%d\n",vt->var.type.arr_start);
			}
			else {
				fprintf(fasm,"mov dx,word[%s]\n",name_start_vt);
				fprintf(fasm,"mov word[%s],dx\n",name_start_vt2);
			}

			if(vt->var.type.arr_end != DynARRAY) {
				fprintf(fasm,"mov rcx,%d\n",vt->var.type.arr_end);
			}
			else {
				fprintf(fasm,"mov cx,word[%s]\n",name_end_vt);
				fprintf(fasm,"mov word[%s],cx\n",name_end_vt2);
			}
			fprintf(fasm,"pop rcx\npop rdx\n");


			// printf("Line 455 nodeInputArr[i]->lexeme: %s",nodeInputArr[i]->lexeme);
			int curr_label = label;
			tNode* dataTypeNode = idNode->nextSibling;
			if(vt->var.type.arr_start != DynARRAY)
				fprintf(fasm,"mov bp,%d\nL%d:\n",vt->var.type.arr_start,label);
			else {
				fprintf(fasm,"mov bp,word[%s]\nL%d:\n",name_start_vt,label);
			}	
			label++;

			if(vt->var.type.arr_end != DynARRAY)
				fprintf(fasm,"cmp bp,%d\njg L%d\n",vt->var.type.arr_end,label);
			else {
				fprintf(fasm,"cmp bp,word[%s]\njg L%d\n",name_end_vt,label);
			}
			label++;

			int index_low = vt->var.type.arr_start;
			//int offset = (index_val - index_low)*size1[(int)t];
			
			fprintf(fasm,"movzx rsi,bp\n");

			if(vt->var.type.arr_start != DynARRAY)
				fprintf(fasm,"sub rsi,%d\n",index_low);
			else {
				fprintf(fasm,"push rdx\nmovsx rdx,word[%s]\n",name_start_vt);
				fprintf(fasm,"sub rsi,rdx\npop rdx\n");
			}

			fprintf(fasm,"push rsp\n");	// important line
			if(dataTypeNode->leftMostChild->nextSibling->tokenId == t_INTEGER) {
				id_type t = INTEGER;
				fprintf(fasm,"mov dx,word[%s+rsi*%d]\n",inputList[i],size1[(int)t]);
				fprintf(fasm,"mov word[%s+rsi*%d],dx\n",name,size1[(int)t]);
			}
			else if(dataTypeNode->leftMostChild->nextSibling->tokenId == t_BOOLEAN) {
				id_type t = BOOLEAN;
				fprintf(fasm,"mov dl,byte[%s+rsi*%d]\n",inputList[i],size1[(int)t]);
				fprintf(fasm,"mov byte[%s+rsi*%d],dl\n",name,size1[(int)t]);
			}
			fprintf(fasm,"pop rsp\n");	// important line
			fprintf(fasm,"inc bp\njmp L%d\nL%d:\n",curr_label,curr_label+1);
		}

		
		else if(idNode->type == REAL) {
			fprintf(fasm,"mov edx,dword[%s]\n",inputList[i]);
			fprintf(fasm,"mov dword[%s],edx\n",name);
		}
		else if(idNode->type == INTEGER) {
			fprintf(fasm,"mov dx,word[%s]\n",inputList[i]);
			fprintf(fasm,"mov word[%s],dx\n",name);
		}
		else if(idNode->type == BOOLEAN) {
			// printf("Bool type\n");
			fprintf(fasm,"mov dl,byte[%s]\n",inputList[i]);
			fprintf(fasm,"mov byte[%s],dl\n",name);
		}

		
		if(i != inputSize-1 && idNode->nextSibling->nextSibling == NULL) {
			printf("ERROR: Expected more ID\n");
		}
		else if(idNode->nextSibling->nextSibling != NULL) {
			idNode = idNode->nextSibling->nextSibling->leftMostChild;
		}
	}
}

void output_plistUtil(tNode* aroot, char** outputList, int outputSize) {
	tNode* idNode = aroot->leftMostChild;
	for(int i = 0; i < outputSize; i++) {
		char name[50];
		// printf("Line 402\n");
		get_var_name(name,idNode);
		if(idNode->type == REAL) {
			fprintf(fasm,"mov edx,dword[%s]\n",name);
			fprintf(fasm,"mov dword[%s],edx\n",outputList[i]);
		}
		else if(idNode->type == INTEGER) {
			fprintf(fasm,"mov dx,word[%s]\n",name);
			fprintf(fasm,"mov word[%s],dx\n",outputList[i]);
		}
		else if(idNode->type == BOOLEAN) {
			fprintf(fasm,"mov dl,byte[%s]\n",name);
			fprintf(fasm,"mov byte[%s],dl\n",outputList[i]);
		}
		if(i != outputSize-1 && idNode->nextSibling->nextSibling == NULL) {
			// printf("ERROR: Expected more ID\n");
		}
		else if(idNode->nextSibling->nextSibling != NULL){
			idNode = idNode->nextSibling->nextSibling->leftMostChild;
		}
	}
}

void gen_prog(tNode* aroot , scopeNode* tableRoot) {
	if(aroot == NULL) {
		// printf("In gen_prog: Root is NULL\n");
		return;
	}
	if(isNonTerm(aroot->tokenId)) {
		// printf("Inside gen_prog; aroot->leftMostChild == NULL\n");
		
        // <program> => <moduleDeclarations><otherModules><driverModule><otherModules>

        if(aroot->tokenId == nt_program) {
			bool found = false;
			tNode* currNode = aroot;
			currNode = currNode->leftMostChild;
			if(currNode != NULL && currNode->tokenId == nt_moduleDeclarations)
				currNode = currNode->nextSibling;
			if(currNode != NULL && currNode->tokenId == nt_otherModules) {
				tNode* otherModNode = currNode;
				while(1) {
					moduleUtil(otherModNode->leftMostChild,tableRoot);
					if(otherModNode->leftMostChild->nextSibling != NULL) 
						otherModNode =  otherModNode->leftMostChild->nextSibling;
					else
						break;
					
				}
				currNode = currNode->nextSibling;	
			}
			if(currNode != NULL && currNode->tokenId == nt_moduleDef)
				currNode = currNode->nextSibling;
			if(currNode != NULL && currNode->tokenId == nt_otherModules) {
				tNode* otherModNode = currNode;
				while(1) {
					moduleUtil(otherModNode->leftMostChild,tableRoot);
					if(otherModNode->leftMostChild->nextSibling != NULL) 
						otherModNode =  otherModNode->leftMostChild->nextSibling;
					else
						break;
					
				}
				currNode = currNode->nextSibling;	
			}

            // <driverModule>
			tNode *temp = aroot->leftMostChild;
			while(temp->tokenId != nt_moduleDef)
				temp = temp->nextSibling;
			fprintf(fasm,"main:\n");
			gen_prog(temp,tableRoot);
			fprintf(fasm , "mov ebx,0\nmov eax,1\nint 0x80\n");
		}

        // In AST START and END retained	
		else if(aroot->tokenId == nt_moduleDef) {
            // printf("If-else statement : 3\n");
            gen_prog(aroot->leftMostChild->nextSibling,tableRoot->children[0]);
        }

        else if(aroot->tokenId == nt_statements) {
			// below if statement was meant for the case statements => EPS
            // but we remove both EPS and <nt_statements> for the above rule
            // this will be handled by the first if in gen_prog which checks if root is NULL
            // if(aroot->leftMostChild->leftMostChild == NULL)
			//      return;
			// printf("If-else statement : 4\n");
            gen_prog(aroot->leftMostChild,tableRoot);   //<nt_statement>
			if(aroot->leftMostChild->nextSibling != NULL)
				gen_prog(aroot->leftMostChild->nextSibling,tableRoot);  //<nt_statements>
		}	
	
        else if(aroot->tokenId == nt_ioStmt) {
			// printf("If-else statement : 5\n");
			// scanning
			// <ioStmt> => GET_VALUE BO ID BC SEMICOL 
			if(aroot->leftMostChild->tokenId == t_GET_VALUE){
				// printf("If-else statement : 6\n");
				id_type t = aroot->leftMostChild->nextSibling->type;
				char name[50];
				get_var_name(name,aroot->leftMostChild->nextSibling);
				// printf("scanning 618:%s\n",name);
				variable_entry* vt = get_var_ptr(aroot->leftMostChild->nextSibling->lexeme,aroot->leftMostChild->nextSibling->scope,aroot->leftMostChild->nextSibling->lineNo);
				// printf("Line 620:%s type arr = %d\n",name,vt->var.type.arr);
				
				// array
				if(vt->var.type.arr) {
					fprintf(fasm,"push rdx\npush rcx\n");
					fprintf(fasm,"mov rdi, msg3\n");
					if(vt->var.type.arr_end != DynARRAY && vt->var.type.arr_start != DynARRAY) {
						int cnt = vt->var.type.arr_end - vt->var.type.arr_start + 1;
						fprintf(fasm,"mov rsi,%d\n",cnt);
					}
					else {
						if(vt->var.type.arr_end == DynARRAY) {
							char name1[50];
							variable_entry* vt1 = get_var_ptr(vt->var.type.endId,aroot->leftMostChild->nextSibling->scope,aroot->leftMostChild->nextSibling->lineNo);
							get_var_name1(name1,vt->var.type.endId,vt1->var.line);
							fprintf(fasm,"movsx rsi,word[%s]\n",name1);
						}
						else {
							fprintf(fasm,"mov rsi,%d\n",vt->var.type.arr_end);
						}
						if(vt->var.type.arr_start == DynARRAY) {
							char name1[50];
							variable_entry* vt1 = get_var_ptr(vt->var.type.startId,aroot->leftMostChild->nextSibling->scope,aroot->leftMostChild->nextSibling->lineNo);
							get_var_name1(name1,vt->var.type.startId,vt1->var.line);
							fprintf(fasm,"movsx rdx,word[%s]\n",name1);
							fprintf(fasm,"sub rsi,rdx\n");
						}
						else {
							fprintf(fasm,"sub rsi,%d\n",vt->var.type.arr_start);
						}
						fprintf(fasm,"add rsi,1\n");
					}
					
					if(vt->var.type.arr_start != DynARRAY)
						fprintf(fasm,"mov rdx,%d\n",vt->var.type.arr_start);
					else {
						char name1[50];
						variable_entry* vt1 = get_var_ptr(vt->var.type.startId,aroot->leftMostChild->nextSibling->scope,aroot->leftMostChild->nextSibling->lineNo);
						get_var_name1(name1,vt->var.type.startId,vt1->var.line);
						fprintf(fasm,"movsx rdx,word[%s]\n",name1);
					}
					if(vt->var.type.arr_end != DynARRAY)
						fprintf(fasm,"mov rcx,%d\n",vt->var.type.arr_end);
					else {
						char name1[50];
						variable_entry* vt1 = get_var_ptr(vt->var.type.endId,aroot->leftMostChild->nextSibling->scope,aroot->leftMostChild->nextSibling->lineNo);
						get_var_name1(name1,vt->var.type.endId,vt1->var.line);
						fprintf(fasm,"movsx rcx,word[%s]\n",name1);
					}
					fprintf(fasm,"mov al,0\ncall printf\n");
					fprintf(fasm,"pop rcx\npop rdx\n");
					int curr_label = label;
					if(vt->var.type.arr_start != DynARRAY)
						fprintf(fasm,"mov bp,%d\nL%d:\n",vt->var.type.arr_start,label);
					else {
						char name1[50];
						variable_entry* vt1 = get_var_ptr(vt->var.type.startId,aroot->leftMostChild->nextSibling->scope,aroot->leftMostChild->nextSibling->lineNo);
						get_var_name1(name1,vt->var.type.startId,vt1->var.line);
						fprintf(fasm,"mov bp,word[%s]\nL%d:\n",name1,label);
					}	
					label++;
					if(vt->var.type.arr_end != DynARRAY)
						fprintf(fasm,"cmp bp,%d\njg L%d\n",vt->var.type.arr_end,label);
					else {
						char name1[50];
						variable_entry* vt1 = get_var_ptr(vt->var.type.endId,aroot->leftMostChild->nextSibling->scope,aroot->leftMostChild->nextSibling->lineNo);
						get_var_name1(name1,vt->var.type.endId,vt1->var.line);
						fprintf(fasm,"cmp bp,word[%s]\njg L%d\n",name1,label);
					}	
					label++;
					int index_low = vt->var.type.arr_start;
					//int offset = (index_val - index_low)*size1[(int)t];
					
					fprintf(fasm,"movsx rsi,bp\n");
					if(vt->var.type.arr_start != DynARRAY)
						fprintf(fasm,"sub rsi,%d\n",index_low);
					else {
						char name1[50];
						variable_entry* vt1 = get_var_ptr(vt->var.type.startId,aroot->leftMostChild->nextSibling->scope,aroot->leftMostChild->nextSibling->lineNo);
						get_var_name1(name1,vt->var.type.startId,vt1->var.line);
						fprintf(fasm,"push rdx\nmovsx rdx,word[%s]\n",name1);
						fprintf(fasm,"sub rsi,rdx\npop rdx\n");
					}
					fprintf(fasm,"push rsp\n");	// important line
					fprintf(fasm,"mov rdi,formatin\n");
					fprintf(fasm,"lea rsi,[%s+rsi*%d]\n",name,size1[(int)t]);
					fprintf(fasm,"mov al,0\ncall scanf\n");
					fprintf(fasm,"pop rsp\n");	// important line
					fprintf(fasm,"inc bp\njmp L%d\nL%d:\n",curr_label,curr_label+1);
				}

				else {
					fprintf(fasm,"mov rdi,msg1\n");
					fprintf(fasm,"mov al,0\ncall printf\n");
					fprintf(fasm,"push rsp\n"); // very important line else you get segmentation fault
					fprintf(fasm,"mov rdi,formatin\n");
					fprintf(fasm,"lea rsi,[%s]\n",name);
					fprintf(fasm,"mov al,0\ncall scanf\n");
					fprintf(fasm,"pop rsp\n");
				}
			}

			// printing
			// <ioStmt> => PRINT BO <var> BC SEMICOL
			if(aroot->leftMostChild->tokenId == t_PRINT) {
				fprintf(fasm,"mov rdi,msg2\n");
				fprintf(fasm,"mov al,0\ncall printf\n");
				// <var> => <var_id_num>
				// <var_id_num> => ID <whichId>
				if(aroot->leftMostChild->nextSibling->leftMostChild->tokenId == t_ID){
					tNode* idnode = aroot->leftMostChild->nextSibling->leftMostChild; 
					id_type t = idnode->type;
				    char name[50];
				    get_var_name(name,idnode);

					// <whichId> => EPS
					if(idnode->nextSibling == NULL) {
						variable_entry* vt = get_var_ptr(idnode->lexeme,idnode->scope,idnode->lineNo);
						// printf("Array name line 757: %s\n",idnode->lexeme);
						// print whole array from start to end
						if(vt->var.type.arr) {
				    		int curr_label = label;
							if(vt->var.type.arr_start != DynARRAY)
				    			fprintf(fasm,"mov bp,%d\nL%d:\n",vt->var.type.arr_start,label);
				    		else {
								char name1[50];
								variable_entry* vt1 = get_var_ptr(vt->var.type.startId,idnode->scope,idnode->lineNo);
								get_var_name1(name1,vt->var.type.startId,vt1->var.line);
								fprintf(fasm,"mov bp,word[%s]\nL%d:\n",name1,label);
							}
							label++;
							if(vt->var.type.arr_end != DynARRAY)
				    			fprintf(fasm,"cmp bp,%d\njg L%d\n",vt->var.type.arr_end,label);
							else {
								char name1[50];
								variable_entry* vt1 = get_var_ptr(vt->var.type.endId,idnode->scope,idnode->lineNo);
								get_var_name1(name1,vt->var.type.endId,vt1->var.line);
								fprintf(fasm,"cmp bp,word[%s]\njg L%d\n",name1,label);
							}
				    		label++;
				    		int index_low = vt->var.type.arr_start;
							if(vt->var.type.arr_start == DynARRAY) {
								char name1[50];
								variable_entry* vt1 = get_var_ptr(vt->var.type.startId,idnode->scope,idnode->lineNo);
								get_var_name1(name1,vt->var.type.startId,vt1->var.line);
								fprintf(fasm,"movzx rdx,word[%s]\n",name1);		// rdx contains the lower index
							}
				    		//int offset = (index_val - index_low)*size1[(int)t];
				    		
				    		fprintf(fasm,"movzx rsi,bp\n");
							if(index_low != DynARRAY)
				    			fprintf(fasm,"sub rsi,%d\n",index_low);
							else
								fprintf(fasm,"sub rsi,rdx\n");
							
				    		fprintf(fasm,"mov rdi,formatout_arr\n");
				    		
							if(t == INTEGER)
								fprintf(fasm,"movsx rsi,word[%s+rsi*%d]\n",name,size1[(int)t]);
							
							// if(t == REAL)
							// 	fprintf(fasm,"movsx rsi,dword[%s+rsi*%d]\n",name,size1[(int)t]);
							
							else if(t == BOOLEAN) {
								fprintf(fasm,"movsx rsi,byte[%s+rsi*%d]\n",name,size1[(int)t]);
								boolPrintUtil();
							}
							fprintf(fasm,"mov al,0\ncall printf\n");
							fprintf(fasm,"inc bp\njmp L%d\nL%d:\n",curr_label,curr_label+1);
							fprintf(fasm,"mov rdi,newline\nmov al,0\ncall printf\n");
				    	}

						else {
				    		fprintf(fasm,"mov rdi,formatout\n");
					     	if(t == INTEGER)
								fprintf(fasm,"movsx rsi,word[%s]\n",name);
							// if(t == REAL)
							// 	fprintf(fasm,"movsx rsi,dword[%s]\n",name);
							else if(t == BOOLEAN) {
								fprintf(fasm,"movsx rsi,byte[%s]\n",name);
								boolPrintUtil();
							}
							fprintf(fasm,"mov al,0\ncall printf\n");
						}
					}

					//aroot->leftMostChild->nextSibling->leftMostChild->tokenId == t_ID
					// <whichId> => SQBO <Index> SQBC
					else {
						fprintf(fasm,"mov rdi,formatout\n");
						int index_val;
						variable_entry* vt = get_var_ptr(idnode->lexeme,tableRoot,idnode->lineNo);
						int index_low = vt->var.type.arr_start;

						// <Index> => NUM
						if(idnode->nextSibling->tokenId == t_NUM) {
							index_val = get_int(idnode->nextSibling->lexeme);
							if(index_low != DynARRAY) {
								int offset = (index_val - index_low)*size1[(int)t];
								if(t == INTEGER)
									fprintf(fasm,"movsx rsi,word[%s+%d]\n",name,offset);
								else if(t == BOOLEAN) {
									fprintf(fasm,"movsx rsi,byte[%s+%d]\n",name,offset);
									boolPrintUtil();
								}
							}
							else {
								fprintf(fasm,"push rax\npush rdx\n");
								fprintf(fasm,"mov rsi,%d\n",index_val);
								char name1[50];
								variable_entry* vt1 = get_var_ptr(vt->var.type.startId,idnode->scope,idnode->lineNo);
								get_var_name1(name1,vt->var.type.startId,vt1->var.line);
								fprintf(fasm,"movsx rdx,word[%s]\n",name1);		// rdx contains the lower index
								fprintf(fasm,"sub rsi,rdx\n");
								if(t == INTEGER) 	
									fprintf(fasm,"movsx rsi,word[%s+rsi*%d]\n",name,size1[(int)t]);
								else if(t == BOOLEAN)
									fprintf(fasm,"movsx rsi,byte[%s+rsi*%d]\n",name,size1[(int)t]);
								fprintf(fasm,"pop rdx\npop rax\n");
							}	
							fprintf(fasm,"mov al,0\ncall printf\n");
						}

						// <Index> => ID
						else {
							char ind_name[50];
							get_var_name(ind_name,idnode->nextSibling);
							// get the value stored in the variable
							if(t == INTEGER)
								fprintf(fasm,"movzx rsi,word[%s]\n",ind_name);
							else if(t == BOOLEAN)
								fprintf(fasm,"movzx rsi,byte[%s]\n",ind_name);
							
							if(index_low != DynARRAY)
								fprintf(fasm,"sub rsi,%d\n",index_low);
							else {		// dynamic array
								char name1[50];
								variable_entry* vt1 = get_var_ptr(vt->var.type.startId,idnode->scope,idnode->lineNo);
								get_var_name1(name1,vt->var.type.startId,vt1->var.line);
								fprintf(fasm,"push rdx\nmovsx rdx,word[%s]\n",name1);		// rdx contains the lower index
								fprintf(fasm,"sub rsi,rdx\npop rdx\n");
							}	
							fprintf(fasm,"mov rdi,formatout\n");
							if(t == INTEGER)
								fprintf(fasm,"movsx rsi,word[%s+rsi*%d]\n",name,size1[(int)t]);
							else if(t == BOOLEAN) {
								fprintf(fasm,"movsx rsi,byte[%s+rsi*%d]\n",name,size1[(int)t]);
								boolPrintUtil();
							}
							fprintf(fasm,"mov al,0\ncall printf\n");
						}
					}
				}

				// <var> => <var_id_num>
				// <var_id_num> => NUM
				// <var> => <boolConstt>
				// <boolConstt> => TRUE
				// <boolConstt> => FALSE
				else {
					tNode* temp = aroot->leftMostChild->nextSibling;
					fprintf(fasm,"mov rdi,formatout\n");
					if(temp->tokenId == t_NUM)
				    	fprintf(fasm,"mov rsi,%s\n",temp->lexeme);
					else if(temp->tokenId == t_TRUE) {
						fprintf(fasm,"mov rsi,1\n");
						boolPrintUtil();
					}
					else if(temp->tokenId == t_FALSE) {
						fprintf(fasm,"mov rsi,0\n");
						boolPrintUtil();
					}			
				    fprintf(fasm,"mov al,0\ncall printf\n");
				}
			}		
		}

		// <statement> => <simpleStmt>
		// <simpleStmt> => <assignmentStmt>
		// <assignmentStmt> => ID <whichStmt>

		// The final evaluated value of the expression on right of ASSIGNOP is there in rdx register
		else if(aroot->tokenId == nt_assignmentStmt) {
			// we get the required value to be assigned to ID 
			// inside dx or dl register by calling below gen_prog()
			gen_prog(aroot->leftMostChild->nextSibling,tableRoot);  //<lvalueIDStmt> or <lvalueARRStmt>
			char name[50];
			get_var_name(name,aroot->leftMostChild);
			
			// <whichStmt> => <lvalueIDStmt>
			// <lvalueIDStmt> => ASSIGNOP <expression> SEMICOL
			if(aroot->leftMostChild->nextSibling->tokenId == nt_lvalueIDStmt) {
				tNode* idNode = aroot->leftMostChild;
				variable_entry* vt = get_var_ptr(idNode->lexeme, idNode->scope,idNode->lineNo);
				//var_type vt=get_var_type(aroot->leftMostChild->lexeme,aroot->leftMostChild->scope,aroot->leftMostChild->lineNo);
				
				// when A:= B where both A and B are array
				if(vt->var.type.arr) {
					int curr_label = label;
					if(vt->var.type.arr_start != DynARRAY)
						fprintf(fasm,"mov bp,%d\nL%d:\n",vt->var.type.arr_start,label);
					else {
						char name1[50];
						variable_entry* vt1 = get_var_ptr(vt->var.type.startId,idNode->scope,idNode->lineNo);
						get_var_name1(name1,vt->var.type.startId,vt1->var.line);
						fprintf(fasm,"mov bp,word[%s]\nL%d:\n",name1,label);
					}	
					label++;
					
					if(vt->var.type.arr_end != DynARRAY)
						fprintf(fasm,"cmp bp,%d\njg L%d\n",vt->var.type.arr_end,label);
					else {
						char name1[50];
						variable_entry* vt1 = get_var_ptr(vt->var.type.endId,idNode->scope,idNode->lineNo);
						get_var_name1(name1,vt->var.type.endId,vt1->var.line);
						fprintf(fasm,"cmp bp,word[%s]\njg L%d\n",name1,label);
					}
					label++;
					
					int index_low = vt->var.type.arr_start;
					//int offset = (index_val - index_low)*size1[(int)t];
					
					fprintf(fasm,"movzx rsi,bp\n");

					if(vt->var.type.arr_start != DynARRAY)
						fprintf(fasm,"sub rsi,%d\n",index_low);
					else {
						char name1[50];
						variable_entry* vt1 = get_var_ptr(vt->var.type.startId,aroot->leftMostChild->nextSibling->scope,aroot->leftMostChild->nextSibling->lineNo);
						get_var_name1(name1,vt->var.type.startId,vt1->var.line);
						fprintf(fasm,"push rdx\nmovsx rdx,word[%s]\n",name1);
						fprintf(fasm,"sub rsi,rdx\npop rdx\n");
					}
					char name2[50];
					get_var_name(name2,aroot->leftMostChild->nextSibling->leftMostChild->nextSibling->leftMostChild->leftMostChild->leftMostChild->leftMostChild->leftMostChild);

					fprintf(fasm,"push rsp\n");	// important line
					id_type t = idNode->type;
					if(t == INTEGER) {
						fprintf(fasm,"mov dx,word[%s+rsi*%d]\n",name2,size1[(int)t]);
						fprintf(fasm,"mov word[%s+rsi*%d],dx\n",name,size1[(int)t]);
					}
					else if(t == REAL) {
						fprintf(fasm,"mov edx,dword[%s+rsi*%d]\n",name2,size1[(int)t]);
						fprintf(fasm,"mov dword[%s+rsi*%d],edx\n",name,size1[(int)t]);
					}				
					else if(t == BOOLEAN) {
						fprintf(fasm,"mov dl,byte[%s+rsi*%d]\n",name2,size1[(int)t]);
						fprintf(fasm,"mov byte[%s+rsi*%d],dl\n",name,size1[(int)t]);
					}
					fprintf(fasm,"pop rsp\n");	// important line
					fprintf(fasm,"inc bp\njmp L%d\nL%d:\n",curr_label,curr_label+1);
				}

				else {
					if(aroot->leftMostChild->type == INTEGER)
						fprintf(fasm,"mov word[%s],dx\n",name);
					
					else if(aroot->leftMostChild->type == REAL)
						fprintf(fasm,"mov dword[%s],edx\n",name);
				
					else if(aroot->leftMostChild->type == BOOLEAN)
						fprintf(fasm,"mov byte[%s],dl\n",name);
				}
			}	
				/*else if(!vt.isStartId && !vt.isEndId){//oye , mujhe ye bata right side ka id ka tnode cool
					int arrstart = vt.arr_start;//ok dekhta hu
					int arrend = vt.arr_end;
					printf(" start=%d end=%d line=%d\n",arrstart,arrend,aroot->lineNo);
					int offset=0;
					id_type t=vt.type;
					char name2[50];//main lunch karke aata hun :P ok, sun ye sahi lag raha? dikhne mein to acha lagg rahi h usse zyada main nhi bata paungalol bye
					// get_var_name(name2,aroot->leftMostChild->nextSibling->leftMostChild->nextSibling);//rhs array name: aroot->leftMostChild->nextSibling->leftMostChild->nextSibling->leftMostChild->leftMostChild->leftMostChild->leftMostChild->leftMostChild->lexeme
					get_var_name(name2,aroot->leftMostChild->nextSibling->leftMostChild->nextSibling->leftMostChild->leftMostChild->leftMostChild->leftMostChild->leftMostChild);
					if(aroot->leftMostChild->type == INTEGER)
						for(int i= arrstart ; i <= arrend ;i++){
							offset=(i - arrstart)*size1[(int)t];
							fprintf(fasm,"mov dx,word[%s+%d]\n",name2,offset);
							fprintf(fasm,"mov word[%s+%d],dx\n",name,offset);//vahi soch rha tha sochte hi reh jaaiyo
							
							// printf("mov word[%s+%d],dx\n",name,offset);
							// printf("mov dx,word[%s+%d]\n",name2,offset);
							}//karte hai chal thodi der mein jaunga
					else if(aroot->leftMostChild->type == REAL)//welcome dono ko
						for(int i= arrstart ; i <= arrend ;i++){
							offset=(i - arrstart)*size1[(int)t];
							fprintf(fasm,"mov edx,dword[%s+%d]\n",name2,offset);//fod diya thanks thanks
							fprintf(fasm,"mov dword[%s+%d],edx\n",name,offset);
							// printf("mov dword[%s+%d],edx\n",name,offset);
							// printf("mov edx,dword[%s+%d]\n",name2,offset);
							}
					else if(aroot->leftMostChild->type == BOOLEAN)
						for(int i= arrstart ; i <= arrend ;i++){
							offset=(i - arrstart)*size1[(int)t];
							fprintf(fasm,"mov dl,byte[%s+%d]\n",name2,offset);
							fprintf(fasm,"mov byte[%s+%d],dl\n",name,offset);
							}
					
				}*/
			

			// <whichStmt> => <lvalueARRStmt>
			// <lvalueARRStmt> => SQBO <Index> SQBC ASSIGNOP <expression> SEMICOL
			// ID [NUM] := <arithmeticOrBooleanExpr> | <U>
			// ID [ID] := <arithmeticOrBooleanExpr> | <U>
			else {
				tNode* index_node = aroot->leftMostChild->nextSibling->leftMostChild;
				int index_val;
				variable_entry* vt = get_var_ptr(aroot->leftMostChild->lexeme,tableRoot,aroot->leftMostChild->lineNo);
				id_type t = aroot->leftMostChild->type;
				int index_low = vt->var.type.arr_start;
				index_val = get_int(index_node->lexeme);
				int offset = (index_val - index_low)*size1[(int)t];
		    	tNode* idnode = aroot->leftMostChild;
				
				// <Index> => NUM
				
				if(index_node->tokenId == t_NUM) {
			    	if(index_low != DynARRAY) {
						if(aroot->leftMostChild->type == INTEGER)
							fprintf(fasm,"mov word[%s+%d],dx\n",name,offset);
						
						else if(aroot->leftMostChild->type == REAL)
							fprintf(fasm,"mov dword[%s+%d],edx\n",name,offset);
						
						else if(aroot->leftMostChild->type == BOOLEAN)
							fprintf(fasm,"mov byte[%s+%d],dl\n",name,offset);
					}	
					
					else {
						fprintf(fasm,"push rax\npush rcx\n");
						fprintf(fasm,"mov rax,%d\n",index_val);
						char name1[50];
						variable_entry* vt1 = get_var_ptr(vt->var.type.startId,idnode->scope,idnode->lineNo);
						get_var_name1(name1,vt->var.type.startId,vt1->var.line);
						fprintf(fasm,"movsx rcx,word[%s]\n",name1);
						fprintf(fasm,"sub rax,rcx\n");	
						if(aroot->leftMostChild->type == INTEGER)
							fprintf(fasm,"mov word[%s+rax*%d],dx\n",name,size1[(int)t]);
						
						else if(aroot->leftMostChild->type == REAL)
							fprintf(fasm,"mov dword[%s+rax*%d],edx\n",name,size1[(int)t]);
						
						else if(aroot->leftMostChild->type == BOOLEAN)
							fprintf(fasm,"mov byte[%s+rax*%d],dl\n",name,size1[(int)t]);
						fprintf(fasm,"pop rcx\npop rax\n");
					}
			    } 
				
				// <Index> => ID
				else
			    {
			    	// array index is a variable
			    	char ind_name[50];
					get_var_name(ind_name,index_node);
					if(t == INTEGER)
						fprintf(fasm,"movzx rsi,word[%s]\n",ind_name);
					else if(t == BOOLEAN)
						fprintf(fasm,"movzx rsi,byte[%s]\n",ind_name);

					if(index_low != DynARRAY) 
						fprintf(fasm,"sub rsi,%d\n",index_low);
					else {
						fprintf(fasm,"push rcx\n");
						char name1[50];
						variable_entry* vt1 = get_var_ptr(vt->var.type.startId,idnode->scope,idnode->lineNo);
						get_var_name1(name1,vt->var.type.startId,vt1->var.line);
						fprintf(fasm,"movsx rcx,word[%s]\n",name1);
						fprintf(fasm,"sub rsi,rcx\npop rcx\n");
					}
					
					if(aroot->leftMostChild->type == INTEGER)
						fprintf(fasm,"mov word[%s+rsi*%d],dx\n",name,size1[(int)t]);
					
					else if(aroot->leftMostChild->type == REAL)
						fprintf(fasm,"mov dword[%s+rsi*%d],edx\n",name,size1[(int)t]);
					
					else if(aroot->leftMostChild->type == BOOLEAN)
						fprintf(fasm,"mov byte[%s+rsi*%d],dl\n",name,size1[(int)t]);
			    }
			}
		}

		// <statement> => <iterativeStmt>
		else if(aroot->tokenId == nt_iterativeStmt) {
			// <iterativeStmt> => FOR BO ID IN <range> BC START <statements> END
			if(aroot->leftMostChild->tokenId == t_ID) {
				char name[50];
				get_var_name(name,aroot->leftMostChild);
				fprintf(fasm,"mov word[%s],%s\n",name,aroot->leftMostChild->nextSibling->leftMostChild->lexeme); // initialize the ID with first value
				int curr_label = label;
				fprintf(fasm,"cmp word[%s],%s\njg L%d\nL%d:\n",name,aroot->leftMostChild->nextSibling->leftMostChild->nextSibling->lexeme,curr_label+1,curr_label);
				label+=2;
				gen_prog(aroot->leftMostChild->nextSibling->nextSibling->nextSibling,tableRoot);
				fprintf(fasm,"inc word[%s]\n",name);
				fprintf(fasm,"cmp word[%s],%s\njle L%d\nL%d:\n",name,aroot->leftMostChild->nextSibling->leftMostChild->nextSibling->lexeme,curr_label,curr_label+1);
			}

			// <iterativeStmt> => WHILE BO <arithmeticOrBooleanExpr> BC START <statements> END
			else {
				gen_prog(aroot->leftMostChild,tableRoot);
				int curr_label = label;
				fprintf(fasm,"cmp dx,0\nje L%d\nL%d:\n",curr_label+1,curr_label);
				label+=2;
				gen_prog(aroot->leftMostChild->nextSibling->nextSibling,tableRoot);
				gen_prog(aroot->leftMostChild,tableRoot);	// after executing <statements> reevaulate the Expr inside while
				fprintf(fasm,"cmp dx,1\njge L%d\nL%d:\n",curr_label,curr_label+1);
			}
		}

		// <condionalStmt> => SWITCH BO ID BC START <caseStmts><Default> END
		else if(aroot->tokenId == nt_condionalStmt) {
			aroot->scope->scope_end = label;	// value stored for fprintf 
			label++;
			gen_prog(aroot->leftMostChild->nextSibling->nextSibling,tableRoot);
			if(aroot->leftMostChild->nextSibling->nextSibling->nextSibling != NULL)
				gen_prog(aroot->leftMostChild->nextSibling->nextSibling->nextSibling,tableRoot);
			fprintf(fasm,"L%d:\n",aroot->scope->scope_end);
		}

		// <caseStmts> => CASE <value> COLON <statements> BREAK SEMICOL <N9>
		else if(aroot->tokenId == nt_caseStmts) {
			char name[50];
			get_switch_id(name,aroot);		// get the switch ID name for the case
			id_type sw_type;
			if(aroot->leftMostChild->tokenId != t_NUM)
				sw_type = BOOLEAN;
			else
				sw_type = INTEGER;
		
			if(sw_type == INTEGER)
				fprintf(fasm,"cmp word[%s],%s\n",name,aroot->leftMostChild->lexeme);
			else
			{
				if(aroot->leftMostChild->tokenId == t_TRUE)
					fprintf(fasm,"cmp byte[%s],1\n",name);
				else
					fprintf(fasm,"cmp byte[%s],0\n",name);
			}
			int curr_label = label;
			fprintf(fasm,"jne L%d\n",curr_label);	// if cmp evaluates to not equal we will evaluate
													// remaining case statements ie the last gen_prog()	 
			label++;
			gen_prog(aroot->leftMostChild->nextSibling,tableRoot); // if cmp evaluates to equal run the statements inside case stmt
			fprintf(fasm,"jmp L%d\n",get_switch_end(aroot));	// after running the statements jump to the end of switch 
			fprintf(fasm,"L%d:\n",curr_label);
			if(aroot->leftMostChild->nextSibling->nextSibling != NULL)
				gen_prog(aroot->leftMostChild->nextSibling->nextSibling,tableRoot);
		}

		// <N9> => CASE <value> COLON <statements> BREAK SEMICOL <N9>
		// <N9> => EPS
		else if(aroot->tokenId == nt_N9) {
			char name[50];
			get_switch_id(name,aroot);
			id_type sw_type;
			if(aroot->leftMostChild->tokenId != t_NUM)
				sw_type = BOOLEAN;
			else
				sw_type = INTEGER;
		
			if(sw_type == INTEGER)
				fprintf(fasm,"cmp word[%s],%s\n",name,aroot->leftMostChild->lexeme);
			else
			{
				if(aroot->leftMostChild->tokenId == t_TRUE)
					fprintf(fasm,"cmp byte[%s],1\n",name);
				else
					fprintf(fasm,"cmp byte[%s],0\n",name);
			}
			int curr_label = label;
			fprintf(fasm,"jne L%d\n",curr_label);
			label++;
			gen_prog(aroot->leftMostChild->nextSibling,tableRoot);
			fprintf(fasm,"jmp L%d\n",get_switch_end(aroot));
			fprintf(fasm,"L%d:\n",curr_label);
			if(aroot->leftMostChild->nextSibling->nextSibling != NULL) {
				gen_prog(aroot->leftMostChild->nextSibling->nextSibling,tableRoot);
			}
		}


		// <lvalueIDStmt> => ASSIGNOP <expression> SEMICOL
		else if(aroot->tokenId == nt_lvalueIDStmt){
			gen_prog(aroot->leftMostChild->nextSibling, tableRoot);
		}
		
		// <lvalueARRStmt> => SQBO <Index> SQBC ASSIGNOP <expression> SEMICOL
		else if(aroot->tokenId == nt_lvalueARRStmt) {
			gen_prog(aroot->leftMostChild->nextSibling->nextSibling, tableRoot);
		}

		// <expression> => <arithmeticOrBooleanExpr>
		// <arithmeticOrBooleanExpr> => <AnyTerm> <N7>
		else if(aroot->tokenId == nt_arithmeticOrBooleanExpr){
			if(aroot->leftMostChild->tokenId == nt_AnyTerm){
				gen_prog(aroot->leftMostChild,tableRoot);
			}
			else {
				boolUtil(aroot->leftMostChild, tableRoot);
			}
			if(aroot->leftMostChild->nextSibling != NULL) {
				left(aroot->leftMostChild);
				gen_prog(aroot->leftMostChild->nextSibling,tableRoot);
				compose(aroot->leftMostChild->nextSibling->leftMostChild,aroot->leftMostChild->type);
			}
		}

		else if(aroot->tokenId == nt_AnyTerm)
		{	
			if(aroot->leftMostChild->tokenId == nt_arithmeticExpr){
				gen_prog(aroot->leftMostChild,tableRoot);
			}
			
			if(aroot->leftMostChild->nextSibling != NULL) {
				left(aroot->leftMostChild);
				gen_prog(aroot->leftMostChild->nextSibling,tableRoot);
				compose(aroot->leftMostChild->nextSibling->leftMostChild,aroot->leftMostChild->type);
			}
		}

		else if(aroot->tokenId == nt_N7) {
			if(aroot->leftMostChild->nextSibling->tokenId == nt_AnyTerm){
				gen_prog(aroot->leftMostChild->nextSibling,tableRoot);
			}
			
			if(aroot->leftMostChild->nextSibling->nextSibling != NULL) {
				left(aroot->leftMostChild->nextSibling);
				gen_prog(aroot->leftMostChild->nextSibling->nextSibling,tableRoot);
				compose(aroot->leftMostChild->nextSibling->nextSibling->leftMostChild,aroot->leftMostChild->nextSibling->type);
			}
		}

		else if(aroot->tokenId == nt_N8) {
			gen_prog(aroot->leftMostChild->nextSibling,tableRoot);
			// if(getChildrenNumber(aroot->leftMostChild->nextSibling->nextSibling ) {
			// 	left(aroot->leftMostChild->nextSibling);
			// 	gen_prog(aroot->leftMostChild->nextSibling->nextSibling,tableRoot);
			// 	compose(aroot->leftMostChild->nextSibling->nextSibling->leftMostChild->leftMostChild,aroot->leftMostChild->nextSibling->type);
			// }
		}

		else if(aroot->tokenId == nt_arithmeticExpr){
			gen_prog(aroot->leftMostChild,tableRoot);
			if(aroot->leftMostChild->nextSibling != NULL) {
				left(aroot->leftMostChild);
				gen_prog(aroot->leftMostChild->nextSibling,tableRoot);
				compose(aroot->leftMostChild->nextSibling->leftMostChild,aroot->leftMostChild->type);
			}
		}
		
		else if(aroot->tokenId == nt_term) {
			if(aroot->leftMostChild->tokenId == nt_arithmeticOrBooleanExpr)
				gen_prog(aroot->leftMostChild,tableRoot);
			else 
				varUtil(aroot->leftMostChild, tableRoot);
			if(aroot->leftMostChild->nextSibling != NULL) {
				left(aroot->leftMostChild);
				gen_prog(aroot->leftMostChild->nextSibling,tableRoot);
				// printf("id_type: %d\n", aroot->leftMostChild->type);
				compose(aroot->leftMostChild->nextSibling->leftMostChild,aroot->leftMostChild->type);
			}
		}

		else if(aroot->tokenId == nt_N4) {
			gen_prog(aroot->leftMostChild->nextSibling,tableRoot);
			if(aroot->leftMostChild->nextSibling->nextSibling != NULL) {
				left(aroot->leftMostChild->nextSibling);
				gen_prog(aroot->leftMostChild->nextSibling->nextSibling,tableRoot);
				compose(aroot->leftMostChild->nextSibling->nextSibling->leftMostChild,aroot->leftMostChild->nextSibling->type);
			}
		}

		
		else if(aroot->tokenId == nt_N5) {
			if(aroot->leftMostChild->nextSibling->tokenId == nt_arithmeticOrBooleanExpr)
				gen_prog(aroot->leftMostChild->nextSibling,tableRoot);
			else
				varUtil(aroot->leftMostChild->nextSibling,tableRoot);
			if(aroot->leftMostChild->nextSibling->nextSibling != NULL) {
				left(aroot->leftMostChild->nextSibling);
				gen_prog(aroot->leftMostChild->nextSibling->nextSibling,tableRoot);
				compose(aroot->leftMostChild->nextSibling->nextSibling->leftMostChild,aroot->leftMostChild->nextSibling->type);
			} 
		}


		else if(aroot->tokenId == nt_U) {
			if(aroot->leftMostChild->nextSibling->tokenId == nt_arithmeticExpr) 
				gen_prog(aroot->leftMostChild->nextSibling, tableRoot);
			else 
				varUtil(aroot->leftMostChild->nextSibling,tableRoot);
			if(aroot->leftMostChild->tokenId == t_MINUS) {
				if(aroot->leftMostChild->nextSibling->tokenId == t_NUM) {
					// printf("neg dx1\n");
					fprintf(fasm,"neg dx\n");
				}

				else if(aroot->leftMostChild->nextSibling->tokenId == t_RNUM) {
					printf("REAL value found in nt_U 1\n");
				}

				else {
					id_type t = aroot->leftMostChild->nextSibling->leftMostChild->type;
					// printf("neg dx2\n");
					if(t == INTEGER) 
						fprintf(fasm,"neg dx\n");
					else if(t == BOOLEAN)
						fprintf(fasm,"neg dl\n");
					else if(t == REAL)
						printf("REAL value found in nt_U 2\n");
				}
			} 
		}

		else if(aroot->tokenId == nt_moduleReuseStmt) {
			tNode* moduleId;
			int outputSize = 0, inputSize = 0;	
			char** inputList; 
			char** outputList;
			if(aroot->leftMostChild->tokenId == nt_optional) {
				// printf("YES\n");
				tNode* idNode = aroot->leftMostChild->leftMostChild->leftMostChild;
				tNode* temp = idNode;
				while(temp->tokenId == t_ID) {
					outputSize++;
					if(temp->nextSibling != NULL){
						temp = temp->nextSibling->leftMostChild;
					}
					else {
						break;
					}
				}
				outputList = (char**)malloc(outputSize*sizeof(char*));
				for(int i=0; i<outputSize; i++) {
					outputList[i] = (char*)malloc(sizeof(char)*50);
					get_var_name(outputList[i],idNode);
					if(idNode->nextSibling != NULL)
						idNode = idNode->nextSibling->leftMostChild;
				}
				moduleId = aroot->leftMostChild->nextSibling;
			}
			else {
				outputList = NULL;
				moduleId = aroot->leftMostChild;
			}
			
			tNode* idNode = moduleId->nextSibling->leftMostChild;
			tNode* temp = idNode;
			while(temp->tokenId == t_ID) {
				inputSize++;
				if(temp->nextSibling != NULL){
					temp = temp->nextSibling->leftMostChild;
				}
				else {
					break;
				}
			}
			inputList = (char**)malloc(inputSize*sizeof(char*));
			bool* boolArr = (bool*)malloc(inputSize*sizeof(bool));
			tNode** nodeInputArr = (tNode**)malloc(inputSize*sizeof(tNode*));		// stores the idNode of inputList
			for(int i=0; i<inputSize; i++) {
				nodeInputArr[i] = idNode;
				variable_entry* vt2 = get_var_ptr(idNode->lexeme,idNode->scope,idNode->lineNo);
				if(vt2->var.type.arr)
					boolArr[i] = true;
				else
					boolArr[i] = false;
					
				inputList[i] = (char*)malloc(sizeof(char)*50);
				get_var_name(inputList[i],idNode);
				if(idNode->nextSibling != NULL)
					idNode = idNode->nextSibling->leftMostChild;
			}
			// printf("OutputSize: %d\n", outputSize);
			tNode* module = findModule(moduleId->lexeme,aroot);
			input_plistUtil(module->leftMostChild->nextSibling,inputList,inputSize,boolArr,nodeInputArr);
			fprintf(fasm,"call %s\n",moduleId->lexeme);
			if(module->leftMostChild->nextSibling->nextSibling->tokenId == nt_output_plist)
				output_plistUtil(module->leftMostChild->nextSibling->nextSibling,outputList,outputSize);
		}
	}
}	

void gen_code(tNode* ast_root,scopeNode* tableRoot) {
	fprintf(fasm,"extern printf\n extern scanf\n");
	fprintf(fasm,"section .data\n");
	fprintf(fasm,"formatin : db \"%%d\",0\n");
	fprintf(fasm,"formatout : db \"%%d\",10,0\n");
	fprintf(fasm,"formatout_arr : db \"%%d \",0\n");
	fprintf(fasm,"newline: db \"\",10,0\n");
	fprintf(fasm,"msg1 : db \"Input: Enter an integer value\",10,0\n");
	fprintf(fasm,"msg2 : db \"Output: \",0\n");
	fprintf(fasm,"msg3 : db \"Enter %%d array elements of integer type for range %%d to %%d\",10,0\n");
	fprintf(fasm,"errormsg: db \"RUN TIME ERROR:  Array index out of bound\",10,0\n");
	fprintf(fasm,"msgtrue : db \"true\",10,0\n");
	fprintf(fasm,"msgfalse : db \"false\",10,0\n");
	// fprintf(fasm,"errormsg1: db \"RUN TIME ERROR:  Array index out of bound 1\",10,0\n");
	fprintf(fasm,"section .bss\n");
	label = 1;
	gen_data(tableRoot);
	fprintf(fasm,"section .text\nglobal main\n");
	gen_prog(ast_root,tableRoot);
	fclose(fasm);
}


void gen_code_util(char* str,tNode* ast_root,scopeNode* tableRoot) {
	fasm = fopen(str,"w");
	if(fasm == NULL)
		printf("No such file named %s\n",str);
	else
		gen_code(ast_root,tableRoot);
}
