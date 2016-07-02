#ifndef __STACK_
#define __STACK_

#include "route.h"

//其实这个结构体用类实现真的很好，可以动态管理内存，这个就的一个对象就是一种状态；
//用结构体的话只能在route.cpp中为passNodeArray分配内存！
/*
typedef struct item
{
	GraphAdjList G;						//图
	int weightArray[MAXDEAMDNODE];		//到每个指定节点的数组
	int *passNodeArray[MAXDEAMDNODE];	//一群指针，每个指针指向一个数组，此数组保存到一个指定的要求节点所经过的节点
	int passNodeNum[MAXDEAMDNODE];		//保存到每个指定节点所经过的数组的个数
	int demandNodeArray[MAXDEAMDNODE];	//当前排序好的要求经过的节点数组
}Item;
*/

#define MAXSIZE MAXDEAMDNODE

typedef struct item
{
	GraphAdjList G;						//图
	int *toEachDemandNodeWeightArray;	//到每个指定节点的权重数组
	int **toEachDemandNodePathArray;	//一群指针，每个指针指向一个数组，此数组保存到一个指定的要求节点所经过的节点
	int *toEachDemandNodePathArrayNum;	//保存到每个指定节点所经过的数组的个数
	int *demandNodeArrayCopy;			//当前排序好的要求经过的节点数组
	bool *visitDemandNodeFlag;			//保存记录当前的哪些节点已经不用再找距离了
	int visitIndex;						//访问到哪个节点的标识
	
	int num;							//为以上指针分配多大空间，也是本层次还剩多少个要求节点的个数(就是在记录还剩多少个要求节点)
}Item;


typedef struct stack
{
	Item data[MAXSIZE];
	int top;
}SqStack;


bool InitStack(SqStack *S);

bool isEmptyStack(const SqStack *S);
void ClearStack(SqStack *S);

bool GetTopItem(const SqStack *S, Item *e);

bool Push(SqStack *S, const Item *e, void (*assiged)(GraphAdjList *, const GraphAdjList *));
bool Pop(SqStack *S, Item *e, void (*assiged)(GraphAdjList *, const GraphAdjList *));

bool DestoryStack(SqStack *S, void (*freeMemory)(Item*));


#endif