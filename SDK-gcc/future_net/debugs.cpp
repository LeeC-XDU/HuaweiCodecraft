#include "debugs.h"
#include <limits.h>
#include <stdlib.h>


void test_CreateALGraph_Function(const GraphAdjList *G)
{
	putchar('\n');
	printf("The vextex number is %d\n", G->numVertexes);
	printf("The edge number is %d\n", G->numEdges);
	puts("索引所对应的节点");
	for(int i=0; i<G->numVertexes; i++)
		printf("%-4d", i);
	putchar('\n');
	for(int i=0; i<G->numVertexes; i++)
		printf("%-4d", G->adjList[i].data);
	putchar('\n');
	putchar('\n');
}

void test_analyzeDemand_Function(const GraphAdjList *G, const int * srcDestNode, const int * demandNodeArray, const int demandNodeNum)
{
	int i = 0;
	printf("srcNodeNumber(Index) :%d(%d)\tdstNodeNumber(Index) :%d(%d)\n", G->adjList[srcDestNode[0]].data, srcDestNode[0], G->adjList[srcDestNode[1]].data, srcDestNode[1]);
	printf("The demand node number(Index)\n");
	for(i=0; i<demandNodeNum; i++)
		printf("%4d(%d)", G->adjList[demandNodeArray[i]].data, demandNodeArray[i]);
	putchar('\n');

	putchar('\n');
}

void test_findingSubWay_Funciton(const GraphAdjList *G, const int srcNode, const int dstNode, const int * passNodeArray, const int passNodeNum, const int weight)
{
	const char *outputFileName  = "debugsInfo.txt";
	FILE *fp = fopen(outputFileName, "a");
	if( !fp )
	{
		fputs("Open file error.\n", stdout);
		fputs("Call test_findingSubWay_Funciton function failure.\n", stdout);
		exit(EXIT_FAILURE);
	}

	if(weight >= INT_MAX/10)
	{
		fprintf(fp, "From %d(%d) to %d(%d) has no way\n", G->adjList[srcNode].data, srcNode, G->adjList[dstNode].data, dstNode);
		fprintf(fp, "The weight is %d\n", weight);
		return;
	}
	fprintf(fp, "From %d(%d) to %d(%d)\n", G->adjList[srcNode].data, srcNode, G->adjList[dstNode].data, dstNode);
	fprintf(fp, "The weight is %d\n", weight);
	fprintf(fp, "The pass node number is %d\n", passNodeNum);
	fputs("The pass node(index) is:\n", fp);
	for(int i=0; i<passNodeNum; i++)
		fprintf(fp, "%4d(%d)", G->adjList[passNodeArray[i]].data, passNodeArray[i]);
	putc('\n', fp);

	putc('\n', fp);
	
	fclose(fp);
}

void test_bubble_sort_Function(const GraphAdjList *G, const int *weightArray, const int *passNodeNum, int **passNodeArray, const int* demandNodeArray, const int demandNodeArrayNum)
{
	const char *outputFileName  = "~\\Huawei\\test-case\\debugInfo\\test_bubble_sortInfo.txt";
	FILE *fp = fopen(outputFileName, "a");
	if( !fp )
	{
		fputs("Open file error.\n", stdout);
		fputs("Call test_bubble_sort_Function failure.\n", stdout);
		exit(EXIT_FAILURE);
	}
	fputs("***After sort***", fp);
/*	for(int i=0; i<demandNodeArrayNum; i++)
	{
		printf("The weight is %d\n", weightArray[i]);
		printf("The pass node number is %d\n", passNodeNum[i]);
		printf("The pass node(index) is\n");
		for(int j=0; j<passNodeNum[i]; j++)
			printf("%4d(%d)", G->adjList[passNodeArray[i][j]].data, passNodeArray[i][j]);
		
		putchar('\n');
		
		putchar('\n');
	}*/
	
	fputs("The choice path`s is\n", fp);
	fprintf(fp, "From %d(%d) to %d(%d)\n", G->adjList[passNodeArray[0][0]].data, passNodeArray[0][0], G->adjList[demandNodeArray[0]].data, demandNodeArray[0]);
	fprintf(fp, "The choice path`s node number is %d\n", passNodeNum[0]);
	fprintf(fp, "The choice path`s weight is %d\n", weightArray[0]);
	fputs("The choice path is:", fp);
	for(int k=0; k<passNodeNum[0]; k++)
		fprintf(fp, "%4d(%d)", G->adjList[passNodeArray[0][k]].data, passNodeArray[0][k]);
	putc('\n', fp);
	
	fputs("The new demand node array is:\n", fp);
	for(int i=0; i<demandNodeArrayNum; i++)
		fprintf(fp, "%4d(%d)", G->adjList[demandNodeArray[i]].data, demandNodeArray[i]);

	putc('\n', fp);
	putc('\n', fp);
	
	fclose(fp);
}

void test_callBackBubbleSort_Function(const GraphAdjList *G, const int *weightArray, const int *passNodeNum, int **passNodeArray, const int* demandNodeArray, const bool *visitDemandNodeFlag, const int demandNodeArrayNum)
{
	const char *outputFileName  = "debugsInfo.txt";
	FILE *fp = fopen(outputFileName, "a");
	if( !fp )
	{
		fputs("Open file error.\n", stdout);
		fputs("Call test_bubble_sort_Function failure.\n", stdout);
		exit(EXIT_FAILURE);
	}
	fputs("***After sort***\n", fp);
/*	for(int i=0; i<demandNodeArrayNum; i++)
	{
		printf("The weight is %d\n", weightArray[i]);
		printf("The pass node number is %d\n", passNodeNum[i]);
		printf("The pass node(index) is\n");
		for(int j=0; j<passNodeNum[i]; j++)
			printf("%4d(%d)", G->adjList[passNodeArray[i][j]].data, passNodeArray[i][j]);
		
		putchar('\n');
		
		putchar('\n');
	}*/
	
	fputs("The choice path`s is\n", fp);
	fprintf(fp, "From %d(%d) to %d(%d)\n", G->adjList[passNodeArray[0][0]].data, passNodeArray[0][0], G->adjList[demandNodeArray[0]].data, demandNodeArray[0]);
	fprintf(fp, "The choice path`s node number is %d\n", passNodeNum[0]);
	fprintf(fp, "The choice path`s weight is %d\n", weightArray[0]);
	fputs("The choice path is:", fp);
	for(int k=0; k<passNodeNum[0]; k++)
		fprintf(fp, "%4d(%d)", G->adjList[passNodeArray[0][k]].data, passNodeArray[0][k]);
	putc('\n', fp);
	
	fputs("The new demand node array is:\n", fp);
	for(int i=0; i<demandNodeArrayNum; i++)
		fprintf(fp, "%4d(%d)", G->adjList[demandNodeArray[i]].data, demandNodeArray[i]);
	putc('\n', fp);
	
	fputs("The corresponds flag of the demand nodes is:\n", fp);
	for(int i=0; i<demandNodeArrayNum; i++)
		fprintf(fp, "%3d", visitDemandNodeFlag[i]);

	putc('\n', fp);
	putc('\n', fp);
	
	fclose(fp);
}

void test_setNodeFlag_Function(const GraphAdjList *G, FILE *fp)
{
	if( !fp )
	{
		fputs("File pointer is null.\n", stdout);
		fputs("Call test_setNodeFlag_Function failure.\n", stdout);
		exit(EXIT_FAILURE);
	}
	EdgeNode *e = NULL;
	bool printfAdjflag = false;		//假表示没有进去过
	
	fputs("Those nodes are deleted\n", fp);
	for(int i=0; i<G->numVertexes; i++)
	{
		e = G->adjList[i].firstEdge;
		while(e)
		{
			if(!e->flag)
			{
				if(!printfAdjflag)
				{
					fprintf(fp, "%d-->", G->adjList[i].data);
					printfAdjflag = true;
				}
				fprintf(fp, "%d,", G->adjList[e->adjvex].data);
			}
			e = e->next;
		}
		if(printfAdjflag)
			putc('\n', fp);
		printfAdjflag = false;
	}
	
	putc('\n', fp);
}


void test_nodeArray2EdgeArray_Function(const GraphAdjList *G, const unsigned short *edgeArray, const int *nodeArray, const int nodeArrayNum)
{
	for(int i=0; i<nodeArrayNum; i++)
		printf("%d-(%hu)-", G->adjList[nodeArray[i]].data, edgeArray[i]);

//	putchar('\n');
}

void test_stackData_Funciton(const SqStack *S, FILE *fp)
{
	for(int i=0; i<=S->top; i++)
	{
		if( !fp )
		{
			fputs("File pointer is null.\n", stdout);
			fputs("Call test_stackData_Funciton failure.\n", stdout);
			exit(EXIT_FAILURE);
		}
		fputs("****************\n", fp);
		fprintf(fp, "From %3d to %3d\n\n", S->data[i].G.adjList[S->data[i].toEachDemandNodePathArray[S->data[i].visitIndex][0]].data, S->data[i].G.adjList[S->data[i].demandNodeArrayCopy[S->data[i].visitIndex]].data);

		fputs("The current Graph is:\n", fp);
		test_setNodeFlag_Function(&S->data[i].G ,fp);

		fputs("The pass node is:\n", fp);
		for(int j=0; j<S->data[i].toEachDemandNodePathArrayNum[S->data[i].visitIndex]; j++)
		{
			fprintf(fp, "%4d",  S->data[i].G.adjList[S->data[i].toEachDemandNodePathArray[S->data[i].visitIndex][j]].data);
		}
		putc('\n', fp);
		putc('\n', fp);
	}
}



/*****************************************************
函数功能:单元测试函数，输出以某个顶点为弧尾的全部节点
参数1:图指针
参数2:顶边标号
返回值:无
说明:传入的是顶点编号，而非索引下标
作者: Lee.C
完成时间:2016-03-09
修改时间:
*******************************************************/
/*
static void printOutNode(const GraphAdjList *G, const int value)
{
	int index = Index(G->adjList, value, G->numVertexes);
	printf("Index is %d\n", index);
	if(G->adjList[index].firstEdge)	
	{
		EdgeNode *e = G->adjList[index].firstEdge;
		printf("%d 出度的点:(编号，权重，边号，索引)\n", value);
		while( e )
		{
			printf("(%d,%d,%d,%d)",e->data, e->weight, e->edgeNumber, e->adjvex);
			e = e->next;
			if( e )
				printf("--->");
		}
		putchar('\n');
	}
	else
		printf("NO Node!\n");
}

*/


/*****************************************************
函数功能:单元测试函数，递归打印到指定顶点所经过的节点编号
参数1:图指针
参数2:前驱数组
参数3:顶点下标索引
返回值:无
说明:传入的是顶点索引，而非顶点标号
作者: Lee.C
完成时间:2016-03-13
修改时间:
*******************************************************/
/*
#ifdef DEBUG3
static void printNodeNumber(const GraphAdjList *G, const int * path, const int value)
{	
	int number = G->adjList[value].data; 

	if(path[value] != -1)
	{
		printNodeNumber(G, path, path[value]);
	}
//	else
//		return;

	printf("%d(%d)-->", number, value);
}
#endif




*/
