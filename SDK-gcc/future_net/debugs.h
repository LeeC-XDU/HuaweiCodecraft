#ifndef DEBUGS_H__
#define DEBUGS_H__

#include "route.h"
#include "stack.h"
#include <stdio.h>

#define TEST_CreateALGraph_FUNCTION
#undef TEST_CreateALGraph_FUNCTION
#define TEST_analyzeDemand_FUNCTION
#undef TEST_analyzeDemand_FUNCTION 
#define TEST_findingSubWay_FUNCITON
#undef TEST_findingSubWay_FUNCITON 
#define TEST_bubble_sort_FUNCITON
#undef TEST_bubble_sort_FUNCITON
#define TEST_setNodeFlag_FUNCITON
#undef TEST_setNodeFlag_FUNCITON
#define TEST_nodeArray2EdgeArray_FUNCITON
#undef TEST_nodeArray2EdgeArray_FUNCITON
#define TEST_callBackBubbleSort_FUNCITON
#undef TEST_callBackBubbleSort_FUNCITON
#define TEST_stackData_FUNCITON
//#undef TEST_stackData_FUNCITON

#define DEBUGINFO
#undef EBUGINFO

void test_CreateALGraph_Function(const GraphAdjList *G);
void test_analyzeDemand_Function(const GraphAdjList *G, const int * srcDestNode, const int * demandNodeArray, const int demandNodeNum);
void test_findingSubWay_Funciton(const GraphAdjList *G, const int srcNode, const int dstNode, const int * passNodeArray, const int passNodeNum, const int weight);
void test_bubble_sort_Function(const GraphAdjList *G, const int *weightArray, const int *passNodeNum, int **passNodeArray, const int* demandNodeArray, const int demandNodeArrayNum);
void test_setNodeFlag_Function(const GraphAdjList *G, FILE *fp);
void test_nodeArray2EdgeArray_Function(const GraphAdjList *G, const unsigned short *edgeArray, const int *nodeArray, const int nodeArrayNum);
void test_callBackBubbleSort_Function(const GraphAdjList *G, const int *weightArray, const int *passNodeNum, int **passNodeArray, const int* demandNodeArray, const bool *visitDemandNodeFlag, const int demandNodeArrayNum);
void test_stackData_Funciton(const SqStack *S, FILE *fp);

#endif
