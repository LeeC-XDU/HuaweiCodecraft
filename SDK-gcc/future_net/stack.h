#ifndef __STACK_
#define __STACK_

#include "route.h"

//��ʵ����ṹ������ʵ����ĺܺã����Զ�̬�����ڴ棬����͵�һ���������һ��״̬��
//�ýṹ��Ļ�ֻ����route.cpp��ΪpassNodeArray�����ڴ棡
/*
typedef struct item
{
	GraphAdjList G;						//ͼ
	int weightArray[MAXDEAMDNODE];		//��ÿ��ָ���ڵ������
	int *passNodeArray[MAXDEAMDNODE];	//һȺָ�룬ÿ��ָ��ָ��һ�����飬�����鱣�浽һ��ָ����Ҫ��ڵ��������Ľڵ�
	int passNodeNum[MAXDEAMDNODE];		//���浽ÿ��ָ���ڵ�������������ĸ���
	int demandNodeArray[MAXDEAMDNODE];	//��ǰ����õ�Ҫ�󾭹��Ľڵ�����
}Item;
*/

#define MAXSIZE MAXDEAMDNODE

typedef struct item
{
	GraphAdjList G;						//ͼ
	int *toEachDemandNodeWeightArray;	//��ÿ��ָ���ڵ��Ȩ������
	int **toEachDemandNodePathArray;	//һȺָ�룬ÿ��ָ��ָ��һ�����飬�����鱣�浽һ��ָ����Ҫ��ڵ��������Ľڵ�
	int *toEachDemandNodePathArrayNum;	//���浽ÿ��ָ���ڵ�������������ĸ���
	int *demandNodeArrayCopy;			//��ǰ����õ�Ҫ�󾭹��Ľڵ�����
	bool *visitDemandNodeFlag;			//�����¼��ǰ����Щ�ڵ��Ѿ��������Ҿ�����
	int visitIndex;						//���ʵ��ĸ��ڵ�ı�ʶ
	
	int num;							//Ϊ����ָ�������ռ䣬Ҳ�Ǳ���λ�ʣ���ٸ�Ҫ��ڵ�ĸ���(�����ڼ�¼��ʣ���ٸ�Ҫ��ڵ�)
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