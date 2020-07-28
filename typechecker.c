/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/

#include"typechecker.h"

bool is_error;
bool in_for;
char for_lexeme[bufferSize];
bool in_switch;
bool in_while;
bool while_lexeme_changed;
char **while_lexeme;
int count_while_lexemes;
id_type switch_type;
FILE* errorFile;

void initSemanticAnalysis(FILE* ptr){
	is_error = false;
	in_for = false;
	in_switch = false;
	in_while = false;
	while_lexeme_changed=false;
	count_while_lexemes=0;
	errorFile = ptr;
}

//returns type of variable
var_type get_var_type(char* varName , scopeNode* sn, int lineno){
    
    var_type varType;

    //if the node is null, we didn't find the variable declaration
    if(sn == NULL)
    {
        varType.type = UNDEFINED;
        return varType;
    }

    int hash_position = hashFunction(varName)%100;

    //if the variable declaration not defined in current scope, check parent
    if(sn->var_Hash[hash_position] == NULL)
 	{
        return get_var_type(varName,sn->parent,lineno);
    }
    //variable declaration could be in current scope
    else{
		
		variable_entry *varPtr = sn->var_Hash[hash_position];
		var_type typeTemp;
		bool isType =false;

        while(varPtr != NULL)
		{
			if(strcmp(varPtr->var.name , varName) == 0 && (varPtr->var.line <= lineno))
			{
				typeTemp = (varPtr->var.type);
				isType=true;
			}
			varPtr = varPtr->next;
		}
		if(isType)return typeTemp;

		// varPtr = sn->module.inputs->inputs;
		// for(int i=0;i<sn->module.inputs->count;i++){
		// 	if(strcmp(varPtr->var.name , varName) == 0){
		// 		varType = (varPtr->var.type);
		// 		return varType;
		// 	}
		// 	varPtr = varPtr->next;
		// }
		// varPtr = sn->module.outputs->outputs;
		// for(int i=0;i<sn->module.outputs->count;i++){
		// 	if(strcmp(varPtr->var.name , varName) == 0){
		// 		varType = (varPtr->var.type);
		// 		return varType;
		// 	}
		// 	varPtr = varPtr->next;
		// }		


		//variable declaration not found in current scope, check parent scope
		return get_var_type(varName,sn->parent,lineno);

    }

}


//returns variable pointer to variable in the hashtable of a scope
variable_entry* get_var_ptr(char* varName , scopeNode* sn, int lineNo){
    
    //if the node is null, we didn't find the variable declaration
    if(sn == NULL) return NULL;

    int hash_position = hashFunction(varName)%100;

    //if the variable declaration not defined in current scope, check parent
    if(sn->var_Hash[hash_position] == NULL)
    {
        return get_var_ptr(varName,sn->parent,lineNo);
    }
    //variable declaration could be in current scope
    else{

        variable_entry *varPtr = sn->var_Hash[hash_position];
		variable_entry *varTemp;

        while(varPtr != NULL)
		{
			if(strcmp(varPtr->var.name , varName) == 0 && lineNo >= varPtr->var.line)
			{
				varTemp = varPtr;
			}
			varPtr = varPtr->next;
		}
		//printf("idhar gadbad hai\n");
		// if(varTemp)
		//printf("lineNo: %d %s\n",lineNo,varTemp->type);
		if(varTemp) return varTemp;
		//printf("idhar gadbad hai\n");

		// varPtr = sn->module.inputs->inputs;
		// while(varPtr != NULL){
		// 	if(strcmp(varPtr->var.name , varName) == 0){
		// 		return varPtr;
		// 	}
		// 	varPtr = varPtr->next;
		// }


		//variable declaration not found in current scope, check parent scope
		return get_var_ptr(varName,sn->parent,lineNo);

    }
}



//sets assigned = true of the variable in the hash table of its scope
void setDefined(variable_entry* varPtr , scopeNode* sn, int lineNo){

    //if the node is null, we didn't find the variable declaration
    if(sn == NULL)
    {
		//printf("yoyo2\n");
        return ;
    }
	//printf("adsf %s\n",varPtr->var.name);
	//printf("yoyo3\n");
    int hash_position = hashFunction(varPtr->var.name)%100;
	//printf("yoyo1\n");
    //if the variable declaration not defined in current scope, check parent
    if(sn->var_Hash[hash_position] == NULL)
    {
		//printf("hey vartemp\n");
        setDefined(varPtr,sn->parent,lineNo);
		//printf("hey vartemp2\n");
		return;
    }
    //variable declaration could be in current scope
    else{

        variable_entry *varTemp = sn->var_Hash[hash_position];
		variable_entry *varTempTemp;
	//printf("hey vartemptemp\n");
        while(varTemp != NULL)
		{
			if(strcmp(varTemp->var.name , varPtr->var.name) == 0 && lineNo >= varTemp->var.line)
			{
				varTempTemp = varTemp;
			}
			varTemp = varTemp->next;
		}
		//printf("hey vartemptemp\n");
		if(varTempTemp){
			varTempTemp->var.assigned = true;
			return;
		}
		//variable declaration not found in current scope, check parent scope
		 setDefined(varPtr,sn->parent,lineNo);
		 return;
    	
	}//no
}//I think setDefined mein I can't return like this


module checkModule(char* modname, scopeNode* sn, int line){
	scopeNode *sn1 = sn;
	
	//printf("Entering module reuse statement\n");
	module temp;
	temp.inputs = (input*)malloc(sizeof(input));
	temp.inputs->count=-1;
	//printf("yoeyo %s",sn->module.name);
	if(sn->parent !=NULL){
		
		while(sn1->parent->type != ROOT){
			sn1=sn1->parent;
		}
		
		//printf("%s and\n",sn1->module.name);
		//printf("%s\n",modname);
		//printf("yoyo %s yoyo %s",sn1->module.name,modname);
		if(!(strcmp(sn1->module.name,modname))){
			is_error=true;
			typeCheckErrors++;
			fprintf(errorFile,"Line %d: Recursion call to function %s not permitted\n",line,modname);
			return temp;
		}
	}
	
	scopeNode *sn2 = sn;
	while(sn2->parent != NULL){
		sn2 = sn2->parent;
	}
	bool flag=false;
	int j=0;
	
	while(j<sn2->no_of_children){
		if(!(strcmp(sn2->children[j]->module.name,modname))){
			flag=true;
			break;
		}
		j++;
	}
	//printf("lineNo: %d\n",line);
	if(!(flag)){
		is_error=true;
		typeCheckErrors++;
		fprintf(errorFile,"Line %d: Function %s called not declared\n",line,modname );
		return temp;
	}
	else{
		//printf("kfy kgy %d %d\n",line,sn2->children[j]->module.lineDef);

		if(sn2->children[j]->module.lineDec == -1){
			if(line>=sn2->children[j]->module.lineDef){
				sn2->children[j]->module.isCalled=true;
				return sn2->children[j]->module;
			}
			else{
				is_error=true;
				typeCheckErrors++;
				fprintf(errorFile,"Line %d: Function %s called not declared\n",line,modname );
			}
		}else{
			// if(line>=sn2->children[j]->module.lineDef && line>=sn2->children[j]->module.lineDec && !line>=sn2->children[j]->module.isCalled){
			// 	is_error=true;
			// 	typeCheckErrors++;fprintf(errorFile,"Semantic Error:Function %s called on line %d has been both declared and defined before\n",modname,line );
			// }else 
			if(line>=sn2->children[j]->module.lineDec){
				sn2->children[j]->module.isCalled=true;
				return sn2->children[j]->module;
			}
			else{
				is_error=true;
				typeCheckErrors++;
				fprintf(errorFile,"Line %d: Function %s called not declared\n",line,modname );
			}
		}
	}
	return temp;
}

bool modulesEqual(module mod,module modTemp,int lineNo){
	variable_entry *input_mod=mod.inputs->inputs;
	variable_entry *input_modTemp= modTemp.inputs->inputs;
	//printf(" modTemp input count=%d\n",modTemp.inputs->count);
	//printf(" mod input count=%d\n",mod.inputs->count);
	int j;
	// //printf("mod atman: %d", mod.inputs->count);

	//printf("modtemp atman: %d",modTemp.inputs->count);

	if(mod.inputs->count!= modTemp.inputs->count){
		is_error=true;
		typeCheckErrors++;
		fprintf(errorFile,"Line %d: Count of input variables passed to the call of function %s do not match\n",lineNo,mod.name);
		return false;
	}
	char *charType[]= {"REAL" , "INTEGER" , "BOOLEAN"};
	int i=1;
	int count = mod.inputs->count;
	while(count>0){
		if(input_modTemp->var.type.arr){
			// printf("asdf\n");
			if(input_mod->var.type.arr){
				// printf("asdf\n");
				if(input_mod->var.type.type != input_modTemp->var.type.type){
					is_error=true;
					typeCheckErrors++;
					fprintf(errorFile,"Line %d: Input argument %d of function %s : Expected type array of %s but found type array of %s\n", lineNo ,i,mod.name,charType[(int)input_modTemp->var.type.type],charType[(int)input_mod->var.type.type] );
					return false;
				}
			}else{
				is_error=true;
				typeCheckErrors++;
				fprintf(errorFile,"Line %d: Input argument %d of function %s : Expected type array of %s but found type %s\n", lineNo,i,mod.name,charType[(int)input_modTemp->var.type.type],charType[(int)input_mod->var.type.type] );
				return false;
			}
		}
		else{
			if(input_mod->var.type.arr){
				is_error=true;
				typeCheckErrors++;
				fprintf(errorFile,"Line %d: Input argument %d of function %s : Expected type %s but found type array of %s \n",lineNo,i,mod.name,charType[(int)input_modTemp->var.type.type],charType[(int)input_mod->var.type.type] );
				return false;
			}else{
				if(input_mod->var.type.type != input_modTemp->var.type.type){
					is_error=true;
					typeCheckErrors++;
					fprintf(errorFile,"Line %d: Input argument %d of function %s : Expected type %s but found type %s\n",lineNo,i,mod.name,charType[(int)input_modTemp->var.type.type],charType[(int)input_mod->var.type.type] );
					return false;
				}
			}
		}


		if(input_mod->var.type.type != input_modTemp->var.type.type){
			is_error=true;
			typeCheckErrors++;
			fprintf(errorFile,"Line %d: Input argument %d of function %s : Expected type %s but found type %s\n",lineNo,i,mod.name,charType[(int)input_modTemp->var.type.type],charType[(int)input_mod->var.type.type] );
			return false;
		}
		i++;
		if(input_mod->var.type.arr){

			if(!input_mod->var.type.isStartId){
					if(!input_modTemp->var.type.isStartId)
						if(input_mod->var.type.arr_start != input_modTemp->var.type.arr_start){
							is_error=true;
							typeCheckErrors++;
							fprintf(errorFile,"Line %d: Input argument %d of function %s : Start index mismatch\n",lineNo,i,mod.name);
						}
					else if (!input_mod->var.type.isEndId)
					{
						if(input_mod->var.type.arr_start > input_modTemp->var.type.arr_end){
							is_error=true;
							typeCheckErrors++;
							fprintf(errorFile,"Line %d: Input argument %d of function %s : Start index cannot be greater than end index\n",lineNo,i,mod.name);
						}
					}
					
			}

			if(!input_mod->var.type.isEndId){
				if(!input_modTemp->var.type.isEndId)
					if(input_mod->var.type.arr_end != input_modTemp->var.type.arr_end){
						is_error=true;
						typeCheckErrors++;
						fprintf(errorFile,"Line %d: Input argument %d of function %s : End index mismatch\n",lineNo,i,mod.name);
					}
					else if (!input_mod->var.type.isStartId)
					{
						if(input_mod->var.type.arr_end < input_modTemp->var.type.arr_start){
							is_error=true;
							typeCheckErrors++;
							fprintf(errorFile,"Line %d: Input argument %d of function %s : Start index cannot be lesser than end index\n",lineNo,i,mod.name);
						}
					}
			}
			int j = input_mod->var.type.arr_end - input_mod->var.type.arr_start;
			// printf("j is %d\n",j);
			for(;j>=0;j--){
				//printf("Type stuff:  type:%d, array:%d, arr_start:%d, arr_end:%d\n",input_mod->var.type.type,input_mod->var.type.arr,input_mod->var.type.arr_start,input_mod->var.type.arr_end);
				input_mod = input_mod->next;
			}
		}
		else input_mod = input_mod->next;
		input_modTemp = input_modTemp->next;
		count--;
		while(count>0  && input_modTemp->var.type.print == false){
			// printf("fdas %d %s\n",input_modTemp->var.type.print,input_modTemp->var.name);
			input_modTemp = input_modTemp->next;
			count--;
		}
		// printf("out of it\n");
	}
	// printf("outs\n");
	variable_entry *output_mod=mod.outputs->outputs;
	variable_entry *output_modTemp=modTemp.outputs->outputs;
	if(mod.outputs->count!= modTemp.outputs->count){
		is_error=true;
		typeCheckErrors++;
		fprintf(errorFile,"Line %d: Count of output variables passed to the call of function %s do not match\n",lineNo,mod.name);
		return false;
	}
	i=1;
	while(output_mod!=NULL){
		if(output_mod->var.type.type != output_modTemp->var.type.type){
			is_error=true;
			typeCheckErrors++;
			fprintf(errorFile,"Line %d: Output argument %d of function %s : Expected type %s but found type %s\n",lineNo,i,mod.name,charType[(int)output_modTemp->var.type.type],charType[(int)output_mod->var.type.type] );
			return false;
		}
		i++;
		output_mod = output_mod->next;
		output_modTemp = output_modTemp->next;
	}
	// printf("exit\n");
	return true;

}

void Asttype(tNode *Astptr){
	char * charType[]= {"REAL" , "INTEGER" , "BOOLEAN"};
	//printf("mannat says we're checking %d\n",getLineNo(Astptr));
	//58:expression->arithmeticOrBooleanExpr
	if(Astptr->ruleUsed==58){
		Asttype(Astptr->leftMostChild);
		Astptr->type = Astptr->leftMostChild->type;
	}
	//59:expression->U
	else if(Astptr->ruleUsed==59){
		Asttype(Astptr->leftMostChild);
		Astptr->type = Astptr->leftMostChild->type;
	}
	//60:U->unary_op,new_NT
	else if(Astptr->ruleUsed==60){
		Asttype(Astptr->leftMostChild->nextSibling);
		Astptr->type = Astptr->leftMostChild->nextSibling->type;
	}
	//61:new_NT->arithmeticExpr
	else if(Astptr->ruleUsed==61){
		Asttype(Astptr->leftMostChild);
		Astptr->type = Astptr->leftMostChild->type;
	}
	//62:new_NT->var_id_num
	else if(Astptr->ruleUsed==62){
		Asttype(Astptr->leftMostChild);
		Astptr->type = Astptr->leftMostChild->type;
	}
	//65:arithmeticOrBooleanExpr->AnyTerm,N7
	else if(Astptr->ruleUsed==65){
		//printf("entered arbool\n");
		Asttype(Astptr->leftMostChild);
		if(Astptr->leftMostChild->type == UNDEFINED)
			Astptr->type = UNDEFINED;
		//67:N7->eps
		else if(Astptr->leftMostChild->nextSibling==NULL)
			Astptr->type = Astptr->leftMostChild->type;
		else if(Astptr->leftMostChild->type ==ARRAY){
			Astptr->type = UNDEFINED;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Operations on arrays are not allowed \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo);
			is_error = true;
		}
		else if(Astptr->leftMostChild->type != BOOLEAN)
		{
			Astptr->type = UNDEFINED;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Expected boolean expression not found on left side of operator %s\n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo,Astptr->leftMostChild->nextSibling->leftMostChild->lexeme );
			is_error = true;
		}
		else
		{
			Asttype(Astptr->leftMostChild->nextSibling);
			if(Astptr->leftMostChild->nextSibling->type == UNDEFINED)
				Astptr->type = UNDEFINED;
			else if(Astptr->leftMostChild->nextSibling->type ==ARRAY){
				Astptr->type = UNDEFINED;
				typeCheckErrors++;
				fprintf(errorFile, "Line %d: Operations on arrays are not allowed \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo );
				is_error = true;
			}
			else if(Astptr->leftMostChild->nextSibling->type == BOOLEAN)
				Astptr->type = BOOLEAN;
		}
	}
	//66:N7->logicalOp,AnyTerm,N7
	else if(Astptr->ruleUsed==66){
		Asttype(Astptr->leftMostChild->nextSibling);
		if(Astptr->leftMostChild->nextSibling->type == BOOLEAN)
		{
			//67:N7->eps
			if(Astptr->leftMostChild->nextSibling->nextSibling==NULL)
				Astptr->type = BOOLEAN;
			else
			{
				Asttype(Astptr->leftMostChild->nextSibling->nextSibling);
				if(Astptr->leftMostChild->nextSibling->nextSibling->type == UNDEFINED)
					Astptr->type = UNDEFINED;
				else if(Astptr->leftMostChild->nextSibling->nextSibling->type ==ARRAY){
					Astptr->type = UNDEFINED;
					typeCheckErrors++;
					fprintf(errorFile, "Line %d: Operations on arrays are not allowed \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo );
					is_error = true;
				}
				else if(Astptr->leftMostChild->nextSibling->nextSibling->type == BOOLEAN)
					Astptr->type = BOOLEAN;
			}
		}
		else
		{ 
			Astptr->type = UNDEFINED;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Expected boolean expression not found on right side of operator %s\n",Astptr->leftMostChild->lineNo,Astptr->leftMostChild->lexeme );
			is_error = true;
		}
	}
	//68:AnyTerm->arithmeticExpr,N8
	else if(Astptr->ruleUsed==68){
		//printf("entered anyterm\n");
		Asttype(Astptr->leftMostChild);
		if(Astptr->leftMostChild->type == UNDEFINED)
			Astptr->type = UNDEFINED;
		//71:N8->eps
		else if(Astptr->leftMostChild->nextSibling==NULL)
			Astptr->type = Astptr->leftMostChild->type;
		else if(Astptr->leftMostChild->type ==ARRAY){
			Astptr->type = UNDEFINED;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Operations on arrays are not allowed \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo);
			is_error = true;
		}
		else if((Astptr->leftMostChild->type != INTEGER) && (Astptr->leftMostChild->type != REAL))
		{
			Astptr->type = UNDEFINED;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Expected integer or real expression not found on left side of operator %s\n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo,Astptr->leftMostChild->nextSibling->leftMostChild->lexeme );
			is_error = true;

		}
		else
		{
			Asttype(Astptr->leftMostChild->nextSibling);
			if(Astptr->leftMostChild->nextSibling->type == UNDEFINED)
				Astptr->type = UNDEFINED;
			else if(Astptr->leftMostChild->nextSibling->type ==ARRAY){
				Astptr->type = UNDEFINED;
				typeCheckErrors++;
				fprintf(errorFile, "Line %d: Operations on arrays are not allowed \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo );
				is_error = true;
			}
			else if(Astptr->leftMostChild->type != Astptr->leftMostChild->nextSibling->type)
			{
				Astptr->type = UNDEFINED;
				typeCheckErrors++;
				fprintf(errorFile, "Line %d: Type mismatch between left side : %s and right side : %s of operator %s\n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo,charType[(int)Astptr->leftMostChild->type],charType[(int)Astptr->leftMostChild->nextSibling->type],Astptr->leftMostChild->nextSibling->leftMostChild->lexeme );
				is_error = true;
				
			} //ek hi errorkyun show kar raha? kaafi hai I corrected 5 to 6 already
			//ho gaya run? lol yes,is  i had cleared the terminal thats why seems so,it is showing only 1 , idk why acha try to write somethin in the terminal I'll get the option to giv e you permissions fir
			else 
				Astptr->type = BOOLEAN;
		}
	}
	//70:N8->relationalOp,arithmeticExpr
	else if(Astptr->ruleUsed==70){
		Asttype(Astptr->leftMostChild->nextSibling);
		if(Astptr->leftMostChild->nextSibling->type ==ARRAY){
				Astptr->type = UNDEFINED;
				typeCheckErrors++;
				fprintf(errorFile, "Line %d: Operations on arrays are not allowed \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo );
				is_error = true;
		}
		else if((Astptr->leftMostChild->nextSibling->type == BOOLEAN) || (Astptr->leftMostChild->nextSibling->type == UNDEFINED))
		{
			Astptr->type = UNDEFINED;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Expected integer or real expression on right side of operator %s \n",Astptr->leftMostChild->lineNo,Astptr->leftMostChild->lexeme );
			is_error = true;		
		}
		else
			Astptr->type=Astptr->leftMostChild->nextSibling->type;
	}
	//72:arithmeticExpr->term,N4
	else if(Astptr->ruleUsed==72){
		//printf("entered arexp\n");
		Asttype(Astptr->leftMostChild);
		
		if(Astptr->leftMostChild->type == UNDEFINED)
			Astptr->type = UNDEFINED;
		//74:N4->eps
		else if(Astptr->leftMostChild->nextSibling==NULL)
			Astptr->type = Astptr->leftMostChild->type;
		else if(Astptr->leftMostChild->type ==ARRAY){
			Astptr->type = UNDEFINED;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Operations on arrays are not allowed \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo );
			is_error = true;
		}
		else if((Astptr->leftMostChild->type != INTEGER) && (Astptr->leftMostChild->type != REAL))
		{
			Astptr->type = UNDEFINED;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Expected integer or real expression on left side of operator %s \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo,Astptr->leftMostChild->nextSibling->leftMostChild->lexeme );
			is_error = true;
		}
		else
		{
			//printf("axpr\n");
			Asttype(Astptr->leftMostChild->nextSibling);
			//printf("axpr\n");
			if(Astptr->leftMostChild->nextSibling->type == UNDEFINED) 
				Astptr->type = UNDEFINED;
			else if(Astptr->leftMostChild->nextSibling->type ==ARRAY){
				Astptr->type = UNDEFINED;
				typeCheckErrors++;
				fprintf(errorFile, "Line %d: Operations on arrays are not allowed \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo);
				is_error = true;
			}
			else if(Astptr->leftMostChild->type != Astptr->leftMostChild->nextSibling->type)
			{
				if(charType[(int)Astptr->leftMostChild->nextSibling->type])
				// //printf("axpr\n");
				Astptr->type = UNDEFINED;
				typeCheckErrors++;
				fprintf(errorFile, "Line %d: Type mismatch between left side : %s and right side : %s of operator %s \n",getLineNo(Astptr->leftMostChild->nextSibling),charType[(int)Astptr->leftMostChild->type],charType[(int)Astptr->leftMostChild->nextSibling->type],Astptr->leftMostChild->nextSibling->leftMostChild->lexeme );
				is_error = true;
				//printf("reached hereaen\n");
			}
			else
				Astptr->type = Astptr->leftMostChild->nextSibling->type;
			
		}
	}
	//73:N4->op1,term,N4
	else if(Astptr->ruleUsed==73){
		//printf("hey op1\n");
		Asttype(Astptr->leftMostChild->nextSibling);
		if(Astptr->leftMostChild->nextSibling->type == UNDEFINED)
			Astptr->type = UNDEFINED;
		else if(Astptr->leftMostChild->nextSibling->type == BOOLEAN)
		{
			Astptr->type = UNDEFINED;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Expected integer or real expression on right side of operator %s \n",Astptr->leftMostChild->lineNo,Astptr->leftMostChild->lexeme );
			is_error = true;
		}
		else
		{
			//74:N4->eps
			if(Astptr->leftMostChild->nextSibling->nextSibling==NULL)
				Astptr->type = Astptr->leftMostChild->nextSibling->type;
			else
			{
				Asttype(Astptr->leftMostChild->nextSibling->nextSibling);
				if(Astptr->leftMostChild->nextSibling->nextSibling->type == UNDEFINED)
					Astptr->type = UNDEFINED;
				else if(Astptr->leftMostChild->nextSibling->nextSibling->type ==ARRAY){
					Astptr->type = UNDEFINED;
					typeCheckErrors++;
					fprintf(errorFile, "Line %d: Operations on arrays are not allowed \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo);
					is_error = true;
				}
				else if(Astptr->leftMostChild->nextSibling->type != Astptr->leftMostChild->nextSibling->nextSibling->type)
				{
					Astptr->type = UNDEFINED;
					typeCheckErrors++;
					fprintf(errorFile,  "Line %d: Type mismatch between left side : %s and right side : %s of operator %s\n",Astptr->leftMostChild->lineNo,charType[(int)Astptr->leftMostChild->nextSibling->type],charType[(int)Astptr->leftMostChild->nextSibling->nextSibling->type],Astptr->leftMostChild->lexeme );
					is_error = true;
				}
				else
					Astptr->type = Astptr->leftMostChild->nextSibling->nextSibling->type;
			}
		}
	}
	//75:term->factor,N5
	else if(Astptr->ruleUsed==75){
		Asttype(Astptr->leftMostChild);
		if(Astptr->leftMostChild->type == UNDEFINED)
			Astptr->type = UNDEFINED;
		//77:N5->eps
		else if(Astptr->leftMostChild->nextSibling==NULL)
			Astptr->type = Astptr->leftMostChild->type;
		else if(Astptr->leftMostChild->type ==ARRAY){
			Astptr->type = UNDEFINED;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Operations on arrays are not allowed \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo);
			is_error = true;
		}
		else if((Astptr->leftMostChild->type != INTEGER) && (Astptr->leftMostChild->type != REAL))
		{
			Astptr->type = UNDEFINED;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Expected integer or real expression on left side of operator %s \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo,Astptr->leftMostChild->nextSibling->leftMostChild->lexeme );
			is_error = true;
		}
		else
		{
			Asttype(Astptr->leftMostChild->nextSibling);
			if(Astptr->leftMostChild->nextSibling->type == UNDEFINED)
				Astptr->type = UNDEFINED;
			else if(Astptr->leftMostChild->nextSibling->type ==ARRAY){
				Astptr->type = UNDEFINED;
				typeCheckErrors++;
				fprintf(errorFile, "Line %d: Operations on arrays are not allowed \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo );
				is_error = true;
			}
			else if(Astptr->leftMostChild->type != Astptr->leftMostChild->nextSibling->type)
		    {
				Astptr->type = UNDEFINED;
				typeCheckErrors++;
				fprintf(errorFile, "Line %d: Type mismatch between left side : %s and right side : %s of operator %s \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo,charType[(int)Astptr->leftMostChild->type],charType[(int)Astptr->leftMostChild->nextSibling->type],Astptr->leftMostChild->nextSibling->leftMostChild->lexeme );
				is_error = true;
			}
			else
				Astptr->type = Astptr->leftMostChild->nextSibling->type;
		}
	}
	//76:N5->op2,factor,N5
	else if(Astptr->ruleUsed==76){
		//printf("hey op2\n");
		Asttype(Astptr->leftMostChild->nextSibling);
		if(Astptr->leftMostChild->nextSibling->type == UNDEFINED)
			Astptr->type = UNDEFINED;
		else if(Astptr->leftMostChild->nextSibling->type == BOOLEAN)
		{
			Astptr->type = UNDEFINED;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Expected integer or real expression on right side of operator %s \n",Astptr->leftMostChild->lineNo,Astptr->leftMostChild->lexeme );
			is_error = true;
		}
		else
		{
			//77:N5->eps
			if(Astptr->leftMostChild->nextSibling->nextSibling==NULL)
				Astptr->type = Astptr->leftMostChild->nextSibling->type;
			else
			{
				Asttype(Astptr->leftMostChild->nextSibling->nextSibling);
				if(Astptr->leftMostChild->nextSibling->nextSibling->type == UNDEFINED)
					Astptr->type = UNDEFINED;
				else if(Astptr->leftMostChild->nextSibling->type ==ARRAY){
					Astptr->type = UNDEFINED;
					typeCheckErrors++;
					fprintf(errorFile, "Line %d: Operations on arrays are not allowed \n",Astptr->leftMostChild->nextSibling->leftMostChild->lineNo );
					is_error = true;
				}
				else if(Astptr->leftMostChild->nextSibling->type != Astptr->leftMostChild->nextSibling->nextSibling->type)
				{
					Astptr->type = UNDEFINED;
					typeCheckErrors++;
					fprintf(errorFile, "Line %d: Type mismatch between left side : %s and right side : %s of operator %s \n",Astptr->leftMostChild->lineNo,charType[(int)Astptr->leftMostChild->nextSibling->type],charType[(int)Astptr->leftMostChild->nextSibling->nextSibling->type],Astptr->leftMostChild->lexeme );
					is_error = true;
				}
				else
					Astptr->type = Astptr->leftMostChild->nextSibling->nextSibling->type;
			}
		}
	}
	//78:factor->arithmeticOrBooleanExpr
	else if(Astptr->ruleUsed==78){
		Asttype(Astptr->leftMostChild);
		Astptr->type = Astptr->leftMostChild->type;
	}
	//79:factor->var_id_num
	else if(Astptr->ruleUsed==79){
		Asttype(Astptr->leftMostChild);
		Astptr->type = Astptr->leftMostChild->type;
	}
	else if(Astptr->tokenId==nt_var){
		if(Astptr->leftMostChild->tokenId==nt_boolConstt)
			Astptr->type=BOOLEAN;
		else if(Astptr->leftMostChild->tokenId==nt_var_id_num){
			Asttype(Astptr->leftMostChild);
			Astptr->type=Astptr->leftMostChild->type;
		}
	}
	else if(Astptr->tokenId==nt_var_id_num){
		//printf("mannat says we're checking %d %d\n",getLineNo(Astptr),Astptr->leftMostChild->tokenId);
		if(Astptr->leftMostChild->tokenId==t_NUM)
			Astptr->type=INTEGER;
		else if(Astptr->leftMostChild->tokenId==t_RNUM)
			Astptr->type=REAL;
		else if(Astptr->leftMostChild->tokenId==t_ID){
			//printf("dhyaan se\n");
			var_type vt = get_var_type(Astptr->leftMostChild->lexeme,Astptr->leftMostChild->scope,Astptr->leftMostChild->lineNo);
			//printf("dhyaan se\n");
			if(vt.type == UNDEFINED)
			{
				Astptr->type = UNDEFINED;
				//printf( "Semantic Error : No such variable %s in line %d\n" , Astptr->leftMostChild->lexeme , getLineNo(Astptr));
				is_error = true;
			}
			else if(Astptr->leftMostChild->nextSibling==NULL && vt.arr)
			{
				Astptr->type = ARRAY;
				//printf("hey ");
				//printf("token id: %d\n", Astptr->tokenId);
				//printf( "Semantic Error : Array %s at line %d not allowed in expression\n",Astptr->leftMostChild->lexeme,getLineNo(Astptr));
				// is_error = true;
			}
			else if(vt.arr && Astptr->leftMostChild->nextSibling!=NULL){
				//printf("mannat says we're checking %d\n",getLineNo(Astptr));
				if(Astptr->leftMostChild->nextSibling->tokenId == t_NUM){
					int index = atoi(Astptr->leftMostChild->nextSibling->lexeme);
					//printf("mannat says we're checking %d\n",getLineNo(Astptr));
					//printf("sdf %d %d %d\n",index,vt.arr_start,vt.isStartId);
					if(!vt.isStartId && index < vt.arr_start){
						is_error=true;
						typeCheckErrors++;
						fprintf(errorFile, "Line %d: Index of Array %s out of bounds\n",getLineNo(Astptr),Astptr->leftMostChild->lexeme);
						Astptr->type=UNDEFINED;
					}
					else if(!vt.isEndId && index > vt.arr_end){
						//printf("ayush says yaha hona chahiye\n");
						is_error=true;
						typeCheckErrors++;
						fprintf(errorFile, "Line %d: Index of Array %s out of bounds\n",getLineNo(Astptr),Astptr->leftMostChild->lexeme);
						Astptr->type=UNDEFINED;
					}
					else
					Astptr->type = vt.type;		
				}
				else
					Astptr->type = vt.type;
			}
			else
				Astptr->type = vt.type;
		}
	}
	else if(Astptr->tokenId==t_TRUE)
		Astptr->type=BOOLEAN;
	else if(Astptr->tokenId==t_FALSE)
		Astptr->type=BOOLEAN;
	else if(Astptr->tokenId==t_NUM)
		Astptr->type=INTEGER;
	else if(Astptr->tokenId==t_RNUM)
		Astptr->type=REAL;
	else if(Astptr->tokenId==t_ID){
		var_type vt = get_var_type(Astptr->leftMostChild->lexeme,Astptr->leftMostChild->scope,Astptr->leftMostChild->lineNo);
			if(vt.type == UNDEFINED)
			{
				Astptr->type = UNDEFINED;
				//printf( "Semantic Error : No such variable %s in line %d\n" , Astptr->leftMostChild->lexeme , getLineNo(Astptr) );
				is_error = true;
			}
			else if(Astptr->leftMostChild->nextSibling==NULL && vt.arr)
			{
				Astptr->type = UNDEFINED;
				//printf("hello\n");
				typeCheckErrors++;
				fprintf(errorFile, "Line %d: Array %s not allowed in expression\n",getLineNo(Astptr),Astptr->lexeme);
				is_error = true;
			}
			else
				Astptr->type = vt.type;
	}
}



void semantic_analysis(tNode *temp){
    
    char * char_type[] = {"INTEGER" , "REAL" , "BOOLEAN"};

	/*if(temp->tokenId == t_START){
		typeCheckErrors++;fprintf(errorFile,"start parent: %d\n",temp->parent->tokenId);
	}*/
	//printf("lineNo: %d\n",getLineNo(temp));

	//if ID is a variable name (not function name)
    if(isTerm(temp->tokenId) && temp->tokenId == t_ID && temp->parent->tokenId != nt_module && temp->parent->tokenId != nt_moduleReuseStmt && temp->parent->tokenId != nt_moduleDeclarations)
    {
		//printf("",);
		//printf("%s with parent %d is getting inside this line no %d\n",temp->lexeme, temp->parent->tokenId, temp->lineNo);
        var_type varType = get_var_type(temp->lexeme , temp->scope, temp->lineNo);
		temp->type = varType.type;

		//if variable part of assignment statement
        if(temp->parent->tokenId == nt_assignmentStmt){

			variable_entry *varPtr = get_var_ptr(temp->lexeme, temp->scope,temp->lineNo);
			// if(!varPtr)typeCheckErrors++;fprintf(errorFile,"hey varptr\n");
			if(varPtr)setDefined(varPtr,temp->scope,temp->lineNo);
			//printf("hey setDefined\n");


			//printf("%d\n",temp->lineNo);

            if(varType.type == UNDEFINED){
                is_error = true;
                typeCheckErrors++;
				fprintf(errorFile,"Line %d: Variable %s doesn't exist\n",temp->lineNo, temp->lexeme);
				if(temp->nextSibling != NULL)
						return semantic_analysis(temp->nextSibling);
					return;
			}

			//ensuring that expression type and type of ID on left of AssignOP match
            if(temp->parent->leftMostChild->nextSibling->tokenId == nt_lvalueIDStmt)
			{

				if(in_for){
                    if(strcmp(temp->lexeme , for_lexeme) == 0)
					{
                        is_error = true;
						typeCheckErrors++;
						fprintf(errorFile, "Line %d: Redefinition of index variable %s\n",temp->lineNo,temp->lexeme);
                        if(temp->nextSibling != NULL)
							return semantic_analysis(temp->nextSibling);
						return;
                    }	
                }

				if(in_while){
					for(int i=0;i<count_while_lexemes;i++){
						if(strcmp(temp->lexeme , while_lexeme[i]) == 0)
						{
							while_lexeme_changed=true;
							break;
						}
					}	
                }
				
				
				
				//printf("%s %d %d\n",temp->parent->leftMostChild->nextSibling->leftMostChild->nextSibling->lexeme,temp->parent->leftMostChild->nextSibling->leftMostChild->nextSibling->lineNo,temp->parent->leftMostChild->nextSibling->leftMostChild->nextSibling->tokenId);
                // getting id type of expression

				//printf("entered\n");
				//printf("%d\n",temp->parent->leftMostChild->nextSibling->leftMostChild->nextSibling->tokenId);
				Asttype(temp->parent->leftMostChild->nextSibling->leftMostChild->nextSibling);
				//printf("Mannats code is right\n");
                //printf("entered3\n");
				id_type type_temp = temp->parent->leftMostChild->nextSibling->leftMostChild->nextSibling->type;
				//printf("entered2\n");
                if(type_temp == UNDEFINED)
				{
                    is_error=true;
                    //printf("Semantic Error: Expression on right side at line %d is not well formed\n",temp->lineNo);
					if(temp->nextSibling != NULL)
						return semantic_analysis(temp->nextSibling);
					return;
                }
				
				if(varType.arr ){
					if(type_temp == ARRAY){
						//printf("entering\n");
						tNode *tempNode = temp->parent->leftMostChild->nextSibling->leftMostChild->nextSibling;
						while(tempNode->leftMostChild!= NULL)
							tempNode = tempNode->leftMostChild;						
// 						->leftMostChild->leftMostChild
// ->leftMostChild->leftMostChild->leftMostChild;
//printf("temp: %s\n",tempNode->lexeme);
						var_type rhsType = get_var_type(tempNode->lexeme , tempNode->scope, tempNode->lineNo);
						if(rhsType.type != varType.type){
							is_error = true;
							typeCheckErrors++;
							fprintf(errorFile, "Line %d: Type mismatch for ID %s\n",temp->lineNo,temp->lexeme);		
						}else if(!varType.isStartId && !rhsType.isStartId && varType.arr_start!=rhsType.arr_start){
							is_error = true;
							typeCheckErrors++;
							fprintf(errorFile, "Line %d: Range mismatch for ID %s\n",temp->lineNo,temp->lexeme);		
						}else if(!varType.isEndId && !rhsType.isEndId && varType.arr_end!=rhsType.arr_end){
							is_error = true;
							typeCheckErrors++;
							fprintf(errorFile, "Line %d: Range mismatch for ID %s\n",temp->lineNo,temp->lexeme);		
						}
					}
					else{
						is_error = true;
						typeCheckErrors++;
						fprintf(errorFile, "Line %d: Type mismatch for ID %s\n",temp->lineNo,temp->lexeme);	
					}

					//printf("ending\n");

					if(temp->nextSibling != NULL)
						return semantic_analysis(temp->nextSibling);
					return;
				}
                
				if(varType.type != type_temp)
				{
					if(varType.type != UNDEFINED){
						is_error = true;
						typeCheckErrors++;
						fprintf(errorFile, "Line %d: Type mismatch for ID %s\n",temp->lineNo,temp->lexeme);
					}
					if(temp->nextSibling != NULL)
						return semantic_analysis(temp->nextSibling);
					return;	
                }
				//printf("did it reach\n");

                
				//printf("did it reach\n");
	
            }

			//ensuring that expression type and type of array ID on left of AssignOP match
            else
            {
				//probably redundant
				var_type varType = get_var_type(temp->lexeme , temp->scope, temp->lineNo);
            
				if(varType.type == UNDEFINED){
					is_error = true;
					typeCheckErrors++;
					fprintf(errorFile,"Line %d: Variable %s doesn't exist\n",temp->lineNo,temp->lexeme);
					if(temp->nextSibling != NULL)
						return semantic_analysis(temp->nextSibling);
					return;
				}

				if(!varType.arr){
					is_error = true;
                    typeCheckErrors++;
					fprintf(errorFile,"Line %d: Type mismatch for ID %s\n",temp->lineNo,temp->lexeme);
					if(temp->nextSibling != NULL)
						return semantic_analysis(temp->nextSibling);
					return;
				}


				if (temp->parent->leftMostChild->nextSibling->leftMostChild->tokenId == t_NUM){

					int index = atoi(temp->parent->leftMostChild->nextSibling->leftMostChild->lexeme);
							
					if(!varType.isStartId && index < varType.arr_start){
						is_error=true;
						typeCheckErrors++;
						fprintf(errorFile, "Line %d: Index of Array %s out of bounds\n",getLineNo(temp),temp->lexeme);
						if(temp->nextSibling != NULL)
							return semantic_analysis(temp->nextSibling);
					return;
					}
					else if(!varType.isEndId && index > varType.arr_end){
						is_error=true;
						typeCheckErrors++;
						fprintf(errorFile, "Line %d : Index of Array %s out of bounds\n",getLineNo(temp),temp->lexeme);
						if(temp->nextSibling != NULL)
							return semantic_analysis(temp->nextSibling);
					return;
					}
					
				//printf("toked id: %d\n",temp->parent->leftMostChild->nextSibling->leftMostChild->nextSibling->nextSibling->tokenId);
				}
				else if(temp->parent->leftMostChild->nextSibling->leftMostChild->tokenId == t_ID){
					
					var_type indexType = get_var_type(temp->parent->leftMostChild->nextSibling->leftMostChild->lexeme , temp->parent->leftMostChild->nextSibling->leftMostChild->scope, temp->parent->leftMostChild->nextSibling->leftMostChild->lineNo);
					if(indexType.type != INTEGER){
						is_error=true;	
						typeCheckErrors++;
						fprintf(errorFile, "Line %d: Index of Array %s should be of integer type\n",getLineNo(temp),temp->lexeme);
					}
					if(temp->nextSibling != NULL)
						return semantic_analysis(temp->nextSibling);
					return;
				}
				
				Asttype(temp->parent->leftMostChild->nextSibling->leftMostChild->nextSibling->nextSibling);
                id_type type_temp = temp->parent->leftMostChild->nextSibling->leftMostChild->nextSibling->nextSibling->type;

                // bool temp_arr = temp->parent->leftMostChild->nextSibling->leftMostChild->nextSibling->nextSibling->type;

				

				if(type_temp == UNDEFINED){
                    is_error=true;
                    //printf("Semantic Error: Expression on right side at line %d is not well formed\n",temp->lineNo);
                    if(temp->nextSibling != NULL)
						return semantic_analysis(temp->nextSibling);
					return;

                }

				if(varType.type != type_temp){
					//printf("varType.type: %d, type_temp: %d\n",varType.type, type_temp);
                    is_error = true;
                    typeCheckErrors++;
					fprintf(errorFile, "Line %d: Type mismatch for ID %s\n",temp->lineNo,temp->lexeme);
					if(temp->nextSibling != NULL)
						return semantic_analysis(temp->nextSibling);
					return;
                }
			}

			
        }
		else
		{
			//printf("lineses %d\n",getLineNo(temp));
			if(temp->parent && temp->parent->parent && (temp->parent->parent->tokenId == nt_optional || temp->parent->tokenId == nt_N3)){
				//printf("hey %d %s\n",temp->parent->tokenId,temp->lexeme);
				if(temp->parent->tokenId != nt_N3){
					if(in_while){
						for(int i=0;i<count_while_lexemes;i++){
							if(strcmp(temp->lexeme , while_lexeme[i]) == 0)
							{
								while_lexeme_changed=true;
								break;
							}
						}	
					}
				}else{
					tNode *temptemp = temp;

					//printf("idhar h kahi\n");
					while(temptemp->parent && temptemp->parent->tokenId != nt_idList){
						temptemp = temptemp->parent;
					}
					//printf("idhar h kahi2\n");
					if(temptemp->parent && temptemp->parent->parent && temptemp->parent->parent->tokenId == nt_optional){
						if(in_while){
							for(int i=0;i<count_while_lexemes;i++){
								if(strcmp(temp->lexeme , while_lexeme[i]) == 0)
								{
									while_lexeme_changed=true;
									break;
								}
							}	
                		}
						
					}

					//printf("idhar h kahi3\n");
				}
			}
			
			if(varType.type == UNDEFINED){
				is_error = true;
				//typeCheckErrors++;
				//fprintf(errorFile,"heehee\n");
				typeCheckErrors++;
				fprintf(errorFile,"Line %d: Variable %s doesn't exist\n", temp->lineNo,temp->lexeme);
				if(temp->nextSibling != NULL)
					return semantic_analysis(temp->nextSibling);
				return;
			}	
		}
		    
    }

	//checking if input and output parameters in module match with the module definition
	//also setting module outptut parameters as defined
	else if(isTerm(temp->tokenId) && temp->tokenId == t_ID && temp->parent->tokenId == nt_moduleReuseStmt)
	{
		//printf("adsfhd %d\n",temp->scope->scope_start);
		module modTemp = checkModule(temp->lexeme,temp->scope, temp->lineNo);
		
		//printf("Exited module reuse statement\n");
		if(modTemp.inputs->count == -1)
		{
			if(temp->nextSibling != NULL)
						return semantic_analysis(temp->nextSibling);
			return;
		}

		bool fill_out;
		module mod;
		mod.inputs = (input * )malloc(sizeof(input));
		mod.outputs = (output * )malloc(sizeof(output));
		mod.inputs->count=0;
		mod.outputs->count=0;
		mod.inputs->inputs=NULL;
		mod.outputs->outputs=NULL;
		strcpy(mod.name , temp->lexeme);

		//filling module output parameters
		//if <optional> == eps, i.e. no output parameters
		if(temp->parent->leftMostChild->tokenId==t_ID)
		{
			mod.outputs->count=0;
			mod.outputs->outputs=NULL;

			modInput(temp->parent->leftMostChild->nextSibling, &mod);
		}
		//if <optional> != eps, i.e. at least one output parameter
		else
		{
			fill_out = modOutput(temp->parent->leftMostChild->leftMostChild, &mod);
			modInput(temp->parent->leftMostChild->nextSibling->nextSibling,&mod);
		} 

		modulesEqual(mod,modTemp,getLineNo(temp));

		variable_entry *varOut = mod.outputs->outputs;

		while(varOut!=NULL)
		{
			//printf("yoyo\n");
			setDefined(varOut , temp->scope,temp->lineNo);
			//printf("yoyo3\n");
			if(in_for)
			{
				if(strcmp(varOut->var.name , for_lexeme) == 0)
				{
					is_error = true;
					typeCheckErrors++;
					fprintf(errorFile, "Line %d: Redefinition of index variable %s\n", temp->lineNo, for_lexeme );
				}	
			}
			varOut = varOut->next;
			//printf("yoy2\n");
		}

	}
	else if(isTerm(temp->tokenId) && temp->tokenId==t_ID && temp->parent->tokenId==nt_module)
	{
		module mod = temp->scope->module;
		//printf("linedef %d %s %d\n",mod.lineDef,mod.name,getLineNo(temp));
		if(mod.lineDef!=getLineNo(temp)){
			typeCheckErrors++;
			fprintf(errorFile,"Line %d: Function %s overloaded\n",getLineNo(temp),temp->lexeme);
			is_error=true;
			return;
		}
	}
	else if(isTerm(temp->tokenId) && temp->tokenId==t_START && temp->parent->tokenId==nt_moduleDef)
	{
		module mod = temp->scope->module;
		//printf("linedef %d %s %d\n",mod.lineDef,mod.name,getLineNo(temp));
		if(!mod.isCalled && mod.lineDec!=-1){
			typeCheckErrors++;
			fprintf(errorFile,"Line %d: Function %s called has been both declared and defined before\n",mod.lineDef,mod.name);
		}
	}


	//checking if module ouput parameters have been defined by the end of the module
	else if(isTerm(temp->tokenId) && temp->tokenId==t_END && temp->parent->tokenId==nt_moduleDef)
	{
		
		module mod = temp->scope->module;
		if(mod.outputs!= NULL){
			variable_entry *varOut = mod.outputs->outputs;
			//printf("end in moduledef\n");
			for(int i=0;i<mod.outputs->count;i++)
			{
				//printf("yo1\n");
				variable_entry *tempVar = get_var_ptr(varOut->var.name,temp->scope,temp->lineNo);
				//printf("yo2\n");
				if(tempVar != NULL && !tempVar->var.assigned)
				{
					is_error=true;
					typeCheckErrors++;
					fprintf(errorFile, "Line %d : Output parameter %s of module %s has not been initialized\n", getLineNo(temp),tempVar->var.name,mod.name);
				}
				varOut = varOut->next;
				// //printf("yo3\n");
			}
		}
		// //printf("end of end in moduledef\n");

	}

	//for and while loops
	else if(isTerm(temp->tokenId) && temp->tokenId == t_START && temp->parent->tokenId == nt_iterativeStmt)
	{
		//if its a for loop
		if(temp->parent->leftMostChild->tokenId == t_ID)
		{
			in_for = true;
			strcpy(for_lexeme , temp->parent->leftMostChild->lexeme);
		}
		//if its a while loop
		else
		{
			Asttype(temp->parent->leftMostChild);
			//while loop variable should be boolean
			if(temp->parent->leftMostChild->type != BOOLEAN)
			{
				is_error=true;
				typeCheckErrors++;
				fprintf(errorFile, "Line %d : Conditional expression of while loop must be of boolean type\n",getLineNo(temp->parent->leftMostChild));
				// if(temp->nextSibling != NULL)
				// 		return semantic_analysis(temp->nextSibling);
				return;
			}else{
				in_while=true;
				while_lexeme_changed=false;
				while_lexeme=NULL;
				count_while_lexemes=0;
				
				// while_lexeme = (char**)malloc(sizeof(char*));
				fill_while_lexemes(temp->parent->leftMostChild);
			}
		}		
	}
	//checking the type of the switch case variable
	else if(isTerm(temp->tokenId) && temp->tokenId == t_START && temp->parent->tokenId == nt_condionalStmt)
	{
		//printf("hey switch\n");
		in_switch = true;
		var_type varType = get_var_type(temp->parent->leftMostChild->lexeme,temp->parent->leftMostChild->scope,temp->parent->leftMostChild->lineNo);
		if(varType.type == UNDEFINED)
		{
			is_error = true;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: No such variable %s\n" , temp->parent->leftMostChild->lineNo, temp->parent->leftMostChild->lexeme );
			// if(temp->nextSibling != NULL)
			// 			return semantic_analysis(temp->nextSibling);
			return;
		}
		
		if(varType.type == REAL)
		{
			is_error = true;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Switch case variable cannot be real\n",temp->parent->leftMostChild->lineNo );
			return;
		}
		else if(varType.arr)
		{
			is_error = true;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Switch case variable %s cannot be of array type\n",getLineNo(temp->parent->leftMostChild),temp->parent->leftMostChild->lexeme);
			return;
		}
		else if(varType.type == INTEGER)
		{
			//if default is empty
			if(temp->parent->leftMostChild->nextSibling->nextSibling->nextSibling->tokenId == t_END)
			{
				is_error=true;
				typeCheckErrors++;
				fprintf(errorFile, "Line %d: Default case statement required in switch statement\n",getLineNo(temp->parent->leftMostChild->nextSibling->nextSibling->nextSibling));
			}	
		}
		else if(varType.type == BOOLEAN)
		{
			//if default is not empty
			if(temp->parent->leftMostChild->nextSibling->nextSibling->nextSibling->tokenId != t_END)
			{
				is_error = true;
				typeCheckErrors++;
				fprintf(errorFile, "Line %d: Default case statement not allowed in switch statement\n",getLineNo(temp->parent->leftMostChild->nextSibling->nextSibling->nextSibling));	
			}
		}
		switch_type = varType.type;
	}//sun default vale mai line no func ki starting ka hai is it? dekhta hu cool tu bhi dekhhaan lite haan samajh aa gaya smart
	//checking if the case value types match with the switch case type
	else if(!isTerm(temp->tokenId) && (temp->tokenId == nt_caseStmts||temp->tokenId==nt_N9))
	{
		if(switch_type == INTEGER && temp->leftMostChild->tokenId != t_NUM)
		{
			is_error = true;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Expected integer case label\n",getLineNo(temp->leftMostChild));
		}	
		else if(switch_type == BOOLEAN && (temp->leftMostChild->tokenId!=t_TRUE && temp->leftMostChild->tokenId!=t_FALSE))
		{
			is_error = true;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Expected boolean case label\n",getLineNo(temp->leftMostChild));
		}
	}

	//changing flags when exiting for and switch statements
	else if(isTerm(temp->tokenId) && temp->tokenId == t_END && temp->parent->tokenId == nt_iterativeStmt)
	{
		in_for = false;
		//printf("while count %d\n",count_while_lexemes);sunn follow kar
		if(in_while){
			in_while=false;
			if(!while_lexeme_changed){
				is_error=true;
				typeCheckErrors++;
				fprintf(errorFile,"Line %d: None of the while index variables altered in the while loop\n",getLineNo(temp));
			}
		}
		
	}
	else if(isTerm(temp->tokenId) && temp->tokenId == t_END && temp->parent->tokenId == nt_condionalStmt)
	{
		in_switch = false;
	}

	//assigning types to Number and real number variables
	else if(isTerm(temp->tokenId) && temp->tokenId == t_NUM)
		temp->type = INTEGER;
	else if(isTerm(temp->tokenId) && temp->tokenId == t_RNUM)
		temp->type = REAL;
	else if(isTerm(temp->tokenId) && temp->tokenId==t_GET_VALUE){
		if(in_for){
			if(!strcmp(temp->nextSibling->lexeme, for_lexeme)){
				is_error=true;
				typeCheckErrors++;
				fprintf(errorFile,"Line %d: For loop index variable redefined\n",getLineNo(temp->nextSibling));
			}
		}
		if(in_while){
			for(int i=0;i<count_while_lexemes;i++){
				//printf("We should reach here\n");
				if(strcmp(temp->nextSibling->lexeme , while_lexeme[i]) == 0)
				{
					while_lexeme_changed=true;
					break;
				}
			}	
		}
	}

	//traversing to first child, then sibling	
	if(temp->leftMostChild != NULL)
		semantic_analysis(temp->leftMostChild);

	if(temp->nextSibling != NULL)
		semantic_analysis(temp->nextSibling);

}

//fills the output parameters actually returned (not from the definition)
bool modOutput(tNode* temp, module *mod){

	if(isTerm(temp->tokenId) && temp->tokenId==t_ID)
	{
		variable_entry *varPtr = (variable_entry*)malloc(sizeof(variable_entry));
		varPtr->var.type = get_var_type(temp->lexeme , temp->scope, temp->lineNo); 
		mod->outputs->count++;
		
		if(varPtr->var.type.type == UNDEFINED)
		{
			is_error = true;
			//printf( "Semantic Error : No such variable %s in line %d\n" , temp->lexeme , temp->lineNo);
			return false;
		}
		if(varPtr->var.type.arr)
		{
			is_error = true;
			typeCheckErrors++;
			fprintf(errorFile, "Line %d: Array %s cannot be returned in call to function\n",varPtr->var.line,varPtr->var.name );
			return false;
		}

		strcpy(varPtr->var.name, temp->lexeme);
		varPtr->var.line = temp->lineNo;
		varPtr->next = NULL;

		if(mod->outputs->outputs == NULL)
		{
			mod->outputs->outputs = varPtr;
		}
		else
		{
			variable_entry * varTemp = mod->outputs->outputs;
			while(varTemp->next != NULL)
			{
				varTemp = varTemp->next;
			}
			varTemp->next = varPtr;
		}		
	}

	if(temp->leftMostChild != NULL)
		if(! modOutput(temp->leftMostChild,mod))
			return false;


	if(temp->nextSibling != NULL)
		if(! modOutput(temp->nextSibling,mod))
			return false;

	return true;
}


//fills the input parameters actually returned (not from the definition)
void modInput(tNode* temp, module * mod){

	if(isTerm(temp->tokenId) && temp->tokenId == t_ID)
	{
		var_type varType = get_var_type(temp->lexeme,temp->scope,temp->lineNo);

		if(varType.type == UNDEFINED)
		{
			is_error = true;
			//printf( "Semantic Error : No such variable %s in line %d\n" , temp->lexeme , temp->lineNo );
			return;
		}
		
		if(!varType.arr)
		{
			mod->inputs->count++;

			variable_entry* varPtr = (variable_entry*)malloc(sizeof(variable_entry));
			strcpy(varPtr->var.name, temp->lexeme);
			varPtr->var.line = temp->lineNo;
			varPtr->var.type = get_var_type(temp->lexeme , temp->scope, temp->lineNo);
			varPtr->var.scope = 0;
			varPtr->var.offset = 0; 
			varPtr->next = NULL;
			if(mod->inputs->inputs == NULL)
				mod->inputs->inputs = varPtr;
			else
			{
				variable_entry *varTemp = mod->inputs->inputs;
				while(varTemp->next != NULL)
				{
					varTemp = varTemp->next;
				}
				varTemp->next = varPtr;
			}
		}
		else
		{
			mod->inputs->count++;
			int j;
			if(varType.isStartId)mod->inputs->count++;
			if(varType.isEndId)mod->inputs->count++;
			for(j = varType.arr_start ; j <= varType.arr_end ; j++)
			{
				
				variable_entry * varPtr = (variable_entry*)malloc(sizeof(variable_entry));
				strcpy(varPtr->var.name, temp->lexeme);
				varPtr->var.line = temp->lineNo;
				varPtr->var.type = get_var_type(temp->lexeme , temp->scope, temp->lineNo);
				varPtr->var.scope = 0; 
				varPtr->var.offset = j;
				varPtr->next = NULL;

				if(mod->inputs->inputs == NULL)
					mod->inputs->inputs = varPtr;
				else
				{
					variable_entry* varTemp = mod->inputs->inputs;
					while(varTemp->next != NULL)
					{
						varTemp = varTemp->next;
					}
					varTemp->next = varPtr;
				}
			}
		}		
	}

	if(temp->leftMostChild != NULL)
		modInput(temp->leftMostChild,mod);


	if(temp->nextSibling != NULL)
		modInput(temp->nextSibling,mod);

}


int getLineNo(tNode *temp){
	if(temp->lineNo == 0)
		if(temp->leftMostChild != NULL)
			return getLineNo(temp->leftMostChild);
	return temp->lineNo;
}

void fill_while_lexemes(tNode*temp){
	if(temp->tokenId == t_ID){
		//printf("yoyo %d %s\n",count_while_lexemes,temp->lexeme);
		if(count_while_lexemes ==0){
			while_lexeme = (char**)malloc(sizeof(char*)*1);
			while_lexeme[0] = (char*)malloc(sizeof(char)*BUFFER_SIZE);
			while_lexeme[0] = temp->lexeme;
			count_while_lexemes++;
		}else{
			count_while_lexemes++;
			while_lexeme = (char**)realloc(while_lexeme,sizeof(char*)*count_while_lexemes);
			while_lexeme[count_while_lexemes-1] = (char*)malloc(sizeof(char)*BUFFER_SIZE);
			while_lexeme[count_while_lexemes-1] = temp->lexeme;
		}
	}else{
		if(temp->leftMostChild)
			fill_while_lexemes(temp->leftMostChild);
		if(temp->tokenId!= nt_arithmeticOrBooleanExpr && temp->nextSibling)
			fill_while_lexemes(temp->nextSibling);
	}
}
