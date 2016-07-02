#include "stack.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


bool InitStack(SqStack *S)
{
	S->top = -1;
	memset(S->data, 0, MAXSIZE * sizeof(Item));
	return true;
}


bool isEmptyStack(const SqStack *S)
{
	if(S->top == -1)
		return true;
	else
		return false;
}

void ClearStack(SqStack *S)
{
	S->top = -1;
	memset(S->data, 0, MAXSIZE * sizeof(Item));
}

/*
bool GetTopItem(const SqStack *S, Item *e)
{
	if(S->top == -1)
	{
		puts("The stack is empty!");
		return false;
	}

	(*e) = S->data[S->top];
	return true;
}
*/

bool Push(SqStack *S, const Item *e, void (*assiged)(GraphAdjList *, const GraphAdjList *))
{
	if(S->top == MAXSIZE-1)
	{
		puts("The stack is full!");
		return false;
	}
	S->top++;
	(*assiged)(&S->data[S->top].G, &e->G);
	S->data[S->top].toEachDemandNodeWeightArray = e->toEachDemandNodeWeightArray;
	S->data[S->top].toEachDemandNodePathArray = e->toEachDemandNodePathArray;
	S->data[S->top].toEachDemandNodePathArrayNum = e->toEachDemandNodePathArrayNum;
	S->data[S->top].demandNodeArrayCopy = e->demandNodeArrayCopy;
	S->data[S->top].visitDemandNodeFlag = e->visitDemandNodeFlag;
	S->data[S->top].visitIndex = e->visitIndex;
	S->data[S->top].num = e->num;
	
	return true;
}

bool Pop(SqStack *S, Item *e, void (*assiged)(GraphAdjList *, const GraphAdjList *))
{
	if(S->top == -1)
	{
		puts("The stack is empty!");
		return false;
	}
	
	(*assiged)(&e->G, &S->data[S->top].G);
	e->toEachDemandNodeWeightArray = S->data[S->top].toEachDemandNodeWeightArray;
	e->toEachDemandNodePathArray = S->data[S->top].toEachDemandNodePathArray;
	e->toEachDemandNodePathArrayNum = S->data[S->top].toEachDemandNodePathArrayNum;
	e->demandNodeArrayCopy = S->data[S->top].demandNodeArrayCopy;
	e->visitDemandNodeFlag = S->data[S->top].visitDemandNodeFlag;
	e->visitIndex = S->data[S->top].visitIndex;
	e->num = S->data[S->top].num;
	S->top--;

	return true;
}

bool DestoryStack(SqStack *S, void (*freeMemory)(Item*))
{
	int i = 0;
	for(i=0; i<=S->top; i++)
	{
		(*freeMemory)(&S->data[i]);
	}
	return true;
}
