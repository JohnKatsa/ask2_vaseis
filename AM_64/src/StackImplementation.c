/* This is the file StackImplementation.c */

#include <stdio.h>
#include <stdlib.h>
#include "StackInterface.h"

void InitializeStack(Stack *S)
{
   S->ItemList=NULL;
}

int Empty(Stack *S)
{
   return (S->ItemList==NULL);
}

int Full(Stack *S)
{
  return 0;
}
/* We assume an already constructed stack is not full since it can potentially */
/* grow as a linked structure  */

void Push(ItemType X, Stack *S)
{
   StackNode *Temp=NULL;

   Temp=(StackNode *) malloc(sizeof(StackNode));

   if (Temp==NULL){
      printf("system storage is exhausted");
   } else {
      Temp->Link=S->ItemList;
      Temp->Item=X;
      S->ItemList=Temp;
   }
}


void Pop(Stack *S, ItemType *X)
{
   StackNode *Temp=NULL;

   if (S->ItemList==NULL){
	X=NULL;
     printf("attempt to pop the empty stack");
   } else {
      Temp=S->ItemList;
      *X=Temp->Item;
      S->ItemList=Temp->Link;
      free(Temp);
   }
}
