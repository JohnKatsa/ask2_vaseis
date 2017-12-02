/* This is the file StackInterface.h */
#include "StackTypes.h"

extern void InitializeStack(Stack *S);
extern int Empty(Stack *S);
extern int Full(Stack *S);
extern void Push(ItemType X, Stack *S);
extern void Pop(Stack *S, ItemType *X);
