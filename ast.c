/*
GROUP 41
2016B2A70803P 	MANNAT KHURANA
2017A7PS0137P	PRAKALP TIWARI
2017A7PS0215P	JHAVERI AYUSH RAJESH
2017A7PS0219P	AAYUSH NAGPAL
*/



#include "ast.h"

tnt t_allowed[] = {t_ID, t_INTEGER, t_REAL, t_BOOLEAN, t_START, t_END, t_EPSILON, t_TRUE, t_FALSE, t_NUM, t_RNUM, t_ASSIGNOP, t_PLUS, t_MINUS, t_MUL, t_DIV, t_AND, t_OR, t_LT, t_LE, t_GT, t_GE, t_EQ, t_NE, t_PRINT, t_GET_VALUE};
#define SIZE_T_ALLOWED 26


bool terminalIsAllowed(tnt term){
    for(int i=0; i<SIZE_T_ALLOWED; i++){
        if(term == t_allowed[i]){
            return true;
        }
    }
    return false;
}

bool isNonTerm(tnt term) {
    return (term >= nt_AnyTerm && term <= nt_U);
}

bool isAllowed(tnt val) {
    return (terminalIsAllowed(val) || isNonTerm(val));
}

int condenseSubTree(tNode* currNode){
	int cnt = 0;        //keeps count of number of useful nodes
    //currNode->astName = astName;
	tNode* currChild = currNode->leftMostChild;
	tNode* prevChild = NULL;
	tNode* head = NULL;

    while(currChild != NULL){
        if(currChild == NULL && cnt == 0){  //we have reached end and no node was kept as child
            printf("currNode: %d, %d\n", currNode->ruleUsed, currNode->tokenId);
            printf("Wrong Parse Tree1\n");
			return -1;
        }
        if(!isAllowed(currChild->tokenId)){
            prevChild = currChild;
            currChild = currChild->nextSibling;
            free(prevChild);
        }
        else {
            cnt++;
            if(head == NULL) {      //we have found first useful node
                if(currChild->parent == NULL){
                    printf("Wrong parse tree, No Parent!\n");
                    return -1;
                }
                currNode->leftMostChild = currChild;        // we are changing the leftmost child of currNode
                                                            // this is the first useful node for currNode 
            }
            else {      //keep appending other useful nodes
                head->nextSibling = currChild;
            }
            head = currChild;
            currChild = currChild->nextSibling;
        }
    }
    head->nextSibling = NULL;
    return cnt;
}    


// this function is to remove redundancy amongst non terminals
// for eg NT1 --> NT2 | T
// this is redundant
// we shift NT2 | T up the tree
void transmuteNode(tNode* parent){ 
    if( parent->leftMostChild!=NULL && parent->leftMostChild->nextSibling == NULL){  //parent has only single child
        tNode* child = parent->leftMostChild;
        parent->tokenId = child->tokenId;
        
        strcpy(parent->lexeme,child->lexeme);
        parent->lineNo = child->lineNo;
        if(child->astName != undec){
            parent->astName = child->astName;
        }
        parent->visited = -99;     //to handle epsilon repair function later
        parent->ruleUsed = child->ruleUsed;
        parent->leftMostChild = child->leftMostChild;
        tNode* temp = child->leftMostChild;
        while(temp!=NULL){
            temp->parent = parent;
            temp = temp->nextSibling;
        }
        free(child);
        child = NULL;
    }
    else{
        printf("Transmute Error\n");
    }
    return;
}


void repairEpsilonNodes(tNode* parent){
    tNode* temp;
    
    if(parent==NULL || parent->leftMostChild==NULL){
        printf("repair error\n");
        return;
    }

    if(parent->leftMostChild->tokenId==t_EPSILON){     //single child which is EPSILON
        temp = parent->leftMostChild;
        parent->leftMostChild = temp->nextSibling;
        free(temp);
        temp = NULL;
        return;
    }

    tNode* temp2;
    temp2 = parent->leftMostChild;
    while(temp2->nextSibling!=NULL && temp2->nextSibling->tokenId!=t_EPSILON){
        temp2 = temp2->nextSibling;
    }
    if(temp2->nextSibling==NULL){       //EPSILON not present as we have reached end
        printf("repair error: No epsilon\n");
        return;
    }
    else{
        temp = temp2->nextSibling;
        if(temp->tokenId!=t_EPSILON || temp->visited!=-99){     
            printf("repair error: ingenuine epsilon\n");
            return;
        }
        temp2->nextSibling = temp->nextSibling;      //skip the epsilon term and free the epsilon term
        free(temp);
        return;
    }
}


//NOTE : Only when we have called transmuteNode(*rootPtr), then only we need to call parseTreeToAST() on rootPtr again as the rootPtr has changed
void parseTreeToAST(tNode** rootPtr, rule** rules){
    int flag = 0; //to handle unit rule case
    if((*rootPtr)==NULL){
        return;
    }

    if((*rootPtr)->ruleUsed == -1){//terminals       //base case
        if(!isTerm((*rootPtr)->tokenId)){
        printf("Terminal error: %d\n", (*rootPtr)->tokenId);
        }
        return;
    }

    if((*rootPtr)->tokenId != rules[(*rootPtr)->ruleUsed]->LHS){
        printf("Error: Expected %d but received %d\n", rules[(*rootPtr)->ruleUsed]->LHS, (*rootPtr)->tokenId);
    }

    int num = condenseSubTree(*rootPtr);
    if(num == -1) {
        printf("Condense error\n");
    }

    if(num == 1) {      //single child
        if((*rootPtr)->leftMostChild->tokenId == t_EPSILON) {
            transmuteNode(*rootPtr);
            repairEpsilonNodes((*rootPtr)->parent);
            return;
        }
        else {
            transmuteNode(*rootPtr);
            parseTreeToAST(rootPtr,rules);
            return;
        }
    }
    
    tNode **toBeTraversed;
    int no_child=0;
    tNode* temp;
    temp = (*rootPtr)->leftMostChild;
    
    while(temp!=NULL){
        no_child++;
        temp = temp->nextSibling;
    }
    
    toBeTraversed = (tNode**)malloc(no_child*sizeof(tNode*));
    int i;
    temp = (*rootPtr)->leftMostChild;
    for(i=0;i<no_child;i++){
        toBeTraversed[i] = temp;
        temp = temp->nextSibling;
    }
    temp = (*rootPtr)->leftMostChild;   //why this
    for(i=0;i<no_child;i++){
        parseTreeToAST(&toBeTraversed[i],rules);
    }//not freed this
    return;
}    
