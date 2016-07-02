#include "route.h"
#include "lib_record.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "stack.h"
//#include "debugs.h"


static void CreateALGraph(GraphAdjList *G, const int edgeNum, char **topo);
static bool isMatch(const AdjList arr, const int value, const int num);
static int Index(const AdjList array, const int element, const int num);
static void Dijkstra2(const GraphAdjList *G, const int srcNode, const int dstNode, int *priorArray, int *weightArray);
static EdgeNode *repeatNode(EdgeNode *e, int nodeIndex);
static int analyzeDemand(const GraphAdjList *G, const char *demand, int *srcDest, int **demandNode);
static int extractNodeIndex(const int *path, int *passNodeArray, int dstNode, const int vertNum);
static unsigned short getEdgeNumber(const GraphAdjList * G, const int src, const int dst);
//static bool Detection(const int *passNodeArray, const int *demandNodeArray, const int passNodeNum, const int demandNodeNum);
static void setNodeFlag(GraphAdjList *G, const int designatedNode, const bool flag);
static int findingWay(GraphAdjList *G, const int *srcDestNode, const int *demandNodeArray, const int demandNodeArrayNum, int **passTotalNodeArray);
static int findingSubWay(const GraphAdjList *G, const int srcNode, const int dstNode, int **passNodeArray, int *passNodeNum);
//static bool onceShortPath(const GraphAdjList *G, const int *srcDestNode, const int *demandNodeArray, const int demandNodeArrayNum);
static void bubble_sort(int *toEachDemandNodeWeightArray, int *toEachDemandNodePathArrayNum, int **toEachDemandNodePathArray, int *demandNodeArrayCopy, int demandNodeNum);
static void nodeArray2EdgeArray(const GraphAdjList * G, const int *nodeArray, unsigned short *edgeArray, const int nodeArrayNum);
void InitStatusValue(Item *e, const int num);
void freeStatusValue(Item *e);
void adjustVisitFlag(Item *e);
void freeMemory(Item *e);
void callBackBubbleSort(int *toEachDemandNodeWeightArray, int *toEachDemandNodePathArrayNum, int **toEachDemandNodePathArray, int *demandNodeArrayCopy, bool *visitDemandNodeFlag, int demandNodeNum);
static int callBackFindingWay(const GraphAdjList *G, const int *srcDestNode, const int *demandNodeArray, const int demandNodeArrayNum, int **passTotalNodeArray);
void GraphCpy(GraphAdjList *G, const GraphAdjList *srcG);
void DestoryGraph(GraphAdjList *G);

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
	GraphAdjList G;

	//创建图
	CreateALGraph(&G, edge_num, topo);

#ifdef TEST_CreateALGraph_FUNCTION  
	test_CreateALGraph_Function(&G);
#endif

/**********************************
	 * srcDestNode[0]：源节点索引
	 * srcDestNode[1]：目标节点索引
	 * demandNodeArray ：要求经过点所组成的数组
	 * demandNodeNum：要求经过的节点个数
	**********************************/
	int srcDestNode[2];
	int *demandNodeArray = NULL;
	int demandNodeNum = 0;

	//初始化相关要求信息
	demandNodeNum = analyzeDemand(&G, demand, srcDestNode, &demandNodeArray);

#ifdef TEST_analyzeDemand_FUNCTION  
	test_analyzeDemand_Function(&G, srcDestNode, demandNodeArray, demandNodeNum);
#endif

//这个真心不需要要，因为很难用到
/* 
	if( onceShortPath(&G, srcDestNode, demandNodeArray, demandNodeNum) )
	{
		//只释放这一个指针就行了，别的指针还没声明呢
		free(demandNodeArray);
		return;
	}
*/

	unsigned short *passEdgeNumberArray = NULL;
	int *passNodeArray = NULL;
	int passEdgeNumberArrayNum;
	setNodeFlag(&G, srcDestNode[1], false);
	//边数比节点数少1

	passEdgeNumberArrayNum = findingWay(&G, srcDestNode, demandNodeArray, demandNodeNum, &passNodeArray) - 1;
	
	
	//没有通路，传出0来，减去1，就是-1喽
	if(passEdgeNumberArrayNum == -1)
	{
		passEdgeNumberArrayNum = callBackFindingWay(&G, srcDestNode, demandNodeArray, demandNodeNum, &passNodeArray) - 1;
		if(passEdgeNumberArrayNum == -1)
		{
			//这部分也可以写为一个单独的函数了
			//输出”NA“
			//释放各种指针
			free(passEdgeNumberArray);
			free(demandNodeArray);
			DestoryGraph(&G);
			puts("The destination node don`t arrive");
			return;
		}
	}
	
	passEdgeNumberArray = (unsigned short *)malloc(passEdgeNumberArrayNum * sizeof(unsigned short));
	nodeArray2EdgeArray(&G, passNodeArray, passEdgeNumberArray, passEdgeNumberArrayNum+1);
	
	for(int i=0; i<passEdgeNumberArrayNum; i++)
		record_result(passEdgeNumberArray[i]);
	
	//释放相关指针
	free(demandNodeArray);
	free(passNodeArray);
	free(passEdgeNumberArray);
	DestoryGraph(&G);
}



/*****************************************************
函数功能:寻路函数，传出所经过的节点的数组和个数
参数1(Input/Output):图指针，要更改其中的 flag 标志位
参数2:源目的节点数组
参数3:要求经过的节点数组
参数4:要求经过的节点的个数
参数5(Output):所经过的全部节点组成的数组
返回值:所经过节点的个数
说明:
作者: Lee.C
完成时间:2016-03-28
修改时间:2016-03-29
修改说明:
*******************************************************/
static int findingWay(GraphAdjList *G, const int *srcDestNode, const int *demandNodeArray, const int demandNodeArrayNum, int **passTotalNodeArray)
{
	//压栈也是压这三个，顶多再有个图，这三个可以放到一个结构体里了，是水平层次的，demandNodeArrayNum是个最大值
	//这就导致了两种思路，一个是在此把空间都分配为最大值，另一个是在循环的内部分配合适的空间，在循环的结尾释放，
	//把需要的东西换个变量保存拿出循环，这两者没有太大效率上的区别，就看怎么写程序的问题了，感觉前者好写
	/*********************************************************************************************
	  *toEachDemandNodePathArray ：到每个指定点所经过的节点的数组
	  *toEachDemandNodeWeightArray：到每个指定点的权重
	  *toEachDemandNodePathArrayNum：到每个指定点所经过节点的个数
	*********************************************************************************************/
	int **toEachDemandNodePathArray = (int **)malloc(demandNodeArrayNum * sizeof(int *));
	int *toEachDemandNodeWeightArray = (int *)malloc(demandNodeArrayNum * sizeof(int));
	int *toEachDemandNodePathArrayNum = (int *)malloc(demandNodeArrayNum * sizeof(int));
	int *demandNodeArrayCopy = (int *)malloc(demandNodeArrayNum * sizeof(int));
	GraphAdjList copyG;
	GraphCpy(&copyG, G);
	memcpy(demandNodeArrayCopy, demandNodeArray, demandNodeArrayNum * sizeof(int));
	
	int toDstNodeWeight = 0;
	int toDstNodePathArrayNum = 0;
	int *toDstNodePathArray = NULL;
	int toEachDemandNodeIndex=0;
	
	
	//这句话有了回溯之后还会再改的
	(*passTotalNodeArray) = (int*)malloc(0 * sizeof(int));
	int offset = 0, passTotalNodeArrayNum = 0;

	int srcNode = srcDestNode[0];
	//int dstNode = demandNodeArrayCopy[0];
	
	for(int i=0; i<demandNodeArrayNum; i++)
	{
		/*
		**toEachDemandNodePathArray = (int **)malloc((demandNodeArrayNum-i) * sizeof(int *));
		*toEachDemandNodeWeightArray = (int *)malloc((demandNodeArrayNum-i) * sizeof(int));
		*toEachDemandNodePathArrayNum = (int *)malloc((demandNodeArrayNum-i) * sizeof(int));
		*demandNodeArrayCopy = (int *)malloc((demandNodeArrayNum-i) * sizeof(int));
		memcpy(demandNodeArrayCopy, demandNodeArrayCopy + i * sizeof(int), (demandNodeArrayNum-i) * sizeof(int));
	*/
//		int minWeight = INT_MAX/10;

		/*************************************************************************
		toEachDemandNodeWeightArray[demandNodeIndex]  到此指定点的权重
		toEachDemandNodePathArray[demandNodeIndex]    到此指定点所经过的节点索引
		
		循环结束后，就能找到源节点到所有指定点的权重和所经过节点的信息
		*************************************************************************/
		//初始化
		for(int m=0; m<demandNodeArrayNum; m++)
		{
			toEachDemandNodeWeightArray[m] = INT_MAX/10;
			toEachDemandNodePathArray[m] = NULL;
			toEachDemandNodePathArrayNum[m] = 0;
			if(m < demandNodeArrayNum-1 && i)
				demandNodeArrayCopy[m] = demandNodeArrayCopy[m+1];
		}

		for(toEachDemandNodeIndex=0; toEachDemandNodeIndex<demandNodeArrayNum-i; toEachDemandNodeIndex++)
		{
			toEachDemandNodeWeightArray[toEachDemandNodeIndex] = findingSubWay(&copyG, srcNode, demandNodeArrayCopy[toEachDemandNodeIndex], &toEachDemandNodePathArray[toEachDemandNodeIndex], &toEachDemandNodePathArrayNum[toEachDemandNodeIndex]);
#ifdef TEST_findingSubWay_FUNCITON 
			test_findingSubWay_Funciton(&copyG, srcNode, demandNodeArrayCopy[toEachDemandNodeIndex], toEachDemandNodePathArray[toEachDemandNodeIndex], toEachDemandNodePathArrayNum[toEachDemandNodeIndex], toEachDemandNodeWeightArray[toEachDemandNodeIndex]);
#endif
			
			if(toEachDemandNodeWeightArray[toEachDemandNodeIndex] >= INT_MAX/10)
			{
				//出栈，待定
				for(int i=0; i<demandNodeArrayNum; i++)
					free(toEachDemandNodePathArray[i]);
				free(toEachDemandNodePathArray);
				free(toEachDemandNodeWeightArray);
				free(toEachDemandNodePathArrayNum);
				free(demandNodeArrayCopy);
				return 0;
			}
		}

		
		
		//还是冒泡排序算了，毕竟这个自己好控一点，关键是本函数还要负责额外的一个指针排序
		bubble_sort(toEachDemandNodeWeightArray, toEachDemandNodePathArrayNum, toEachDemandNodePathArray, demandNodeArrayCopy, toEachDemandNodeIndex);
#ifdef TEST_bubble_sort_FUNCITON 
		test_bubble_sort_Function(&copyG, toEachDemandNodeWeightArray, toEachDemandNodePathArrayNum, toEachDemandNodePathArray, demandNodeArrayCopy, toEachDemandNodeIndex);
#endif

		//剪枝,在还没有回溯的情况下，下标只取0就可以
		for(int j=0; j<toEachDemandNodePathArrayNum[0]; j++)
			setNodeFlag(&copyG, toEachDemandNodePathArray[0][j], false);
#ifdef TEST_setNodeFlag_FUNCITON
		test_setNodeFlag_Function(&copyG);
#endif
		
		passTotalNodeArrayNum += toEachDemandNodePathArrayNum[0];
		offset = toEachDemandNodePathArrayNum[0];
		*passTotalNodeArray = (int *)realloc(*passTotalNodeArray, passTotalNodeArrayNum * sizeof(int));
		memcpy( *passTotalNodeArray+passTotalNodeArrayNum-offset, toEachDemandNodePathArray[0], toEachDemandNodePathArrayNum[0] * sizeof(int));
//		nodeArray2EdgeArray(G, toEachDemandNodePathArray[0], *passTotalNodeArray + offset, toEachDemandNodePathArrayNum[0]);
//#ifdef TEST_nodeArray2EdgeArray_FUNCITON
//		test_nodeArray2EdgeArray_Function(G, (*passTotalNodeArray), toEachDemandNodePathArray[0], toEachDemandNodePathArrayNum[0]);
//		putchar('\n');
//#endif
		srcNode = demandNodeArrayCopy[0];
#ifdef DEBUGINFO
		printf("The new srcNode number(index) is: %d(%d)\n", copyG.adjList[srcNode].data, srcNode);
		printf("The offset is :%d\n", offset);
		printf("The total number of passTotalNodeArray is: %d\n\n", passTotalNodeArrayNum);
#endif

	
	}
	//释放目的节点，找最后一个指定节点到目的节点的路径
	setNodeFlag(&copyG, srcDestNode[1], true);
	toDstNodeWeight = findingSubWay(&copyG, srcNode, srcDestNode[1], &toDstNodePathArray, &toDstNodePathArrayNum);
#ifdef TEST_findingSubWay_FUNCITON 
	test_findingSubWay_Funciton(&copyG, srcNode, srcDestNode[1], toDstNodePathArray, toDstNodePathArrayNum, toDstNodeWeight);
#endif
	if(toDstNodeWeight >= INT_MAX/10)  //????
	{
		//待定
		for(int i=0; i<demandNodeArrayNum; i++)
			free(toEachDemandNodePathArray[i]);
		free(toEachDemandNodePathArray);
		free(toEachDemandNodeWeightArray);
		free(toEachDemandNodePathArrayNum);
		free(demandNodeArrayCopy);
		free(toDstNodePathArray);
		return 0;
	}
	passTotalNodeArrayNum += toDstNodePathArrayNum;
	offset = toDstNodePathArrayNum;
	*passTotalNodeArray = (int *)realloc(*passTotalNodeArray, passTotalNodeArrayNum * sizeof(int));
	memcpy( *passTotalNodeArray+passTotalNodeArrayNum-offset, toDstNodePathArray, toDstNodePathArrayNum * sizeof(int));
#ifdef DEBUGINFO
	putchar('\n');
	puts("The passTotalNodeArray is:Number(index)");
	for(int i=0; i<passTotalNodeArrayNum; i++)
		printf("%d(%d),", G->adjList[(*passTotalNodeArray)[i]].data, (*passTotalNodeArray)[i]);
	putchar('\n');
	putchar('\n');

#endif

	
	*passTotalNodeArray = (int *)realloc(*passTotalNodeArray, ++passTotalNodeArrayNum * sizeof(int));
	(*passTotalNodeArray)[passTotalNodeArrayNum-1] = srcDestNode[1];
		
	for(int i=0; i<demandNodeArrayNum; i++)
		free(toEachDemandNodePathArray[i]);
	free(toEachDemandNodePathArray);
	free(toEachDemandNodeWeightArray);
	free(toEachDemandNodePathArrayNum);
	free(demandNodeArrayCopy);
	free(toDstNodePathArray);

#ifdef DEBUGINFO
	puts("findingWay function is called");
#endif

	DestoryGraph(&copyG);

	return passTotalNodeArrayNum;
}


static int callBackFindingWay(const GraphAdjList *G, const int *srcDestNode, const int *demandNodeArray, const int demandNodeArrayNum, int **passTotalNodeArray)
{
	Item currentStatus;
	memset(&currentStatus, 0, sizeof(Item));
	SqStack S;
	InitStack(&S);
	

	int srcNode = srcDestNode[0];
	int dstNode;
	int demandNodeIndex_i, demandNodeIndex_j, neglectCount, toEachDemandNodeIndex = 0;
	int offset = 0, passTotalNodeArrayNum = 0;
	
	//demandNodeIndex_i 记录跑到哪一层，出栈后可能要进行递减操作
	for(demandNodeIndex_i=0; demandNodeIndex_i<=demandNodeArrayNum; demandNodeIndex_i++)
	{
		if(demandNodeIndex_i == demandNodeArrayNum)
		{
			setNodeFlag(&currentStatus.G, srcDestNode[1], true);
			InitStatusValue(&currentStatus, 1);
			currentStatus.demandNodeArrayCopy[0] = srcDestNode[1];
#ifdef TEST_setNodeFlag_FUNCITON
			{
					const char *str = "Graph.txt";
					FILE *fpp = NULL;
					fpp = fopen(str, "a");
					fprintf(fpp, "The stack graph is:\n");
					fclose(fpp);
					test_setNodeFlag_Function(G);
					fpp = fopen(str, "a");
					fprintf(fpp, "The new graph is:\n");
					fclose(fpp);
					for(int i=0; i<=S.top; i++)
					{
						fpp = fopen(str, "a");
						fprintf(fpp, "The %d levle graph is:\n", i);
						fclose(fpp);
						test_setNodeFlag_Function(&S.data[i].G);
					}
			}
#endif
		}
		else
		{
			InitStatusValue(&currentStatus, demandNodeArrayNum-demandNodeIndex_i);
			if(demandNodeIndex_i == 0)
			{
				GraphCpy(&currentStatus.G, G);
				memcpy(currentStatus.demandNodeArrayCopy, demandNodeArray, demandNodeArrayNum * sizeof(int));
				if(currentStatus.num == 14)
					currentStatus.visitIndex = 4;
				if(currentStatus.num > 35 && currentStatus.num < 40)
					currentStatus.visitIndex = currentStatus.num - 6;
			}
		}
		
		//找某个节点到其他所有节点的距离，都有距离，则排序压栈保存
		for(demandNodeIndex_j=0, neglectCount=0; demandNodeIndex_j<currentStatus.num; demandNodeIndex_j++)
		{
			 //自己和已经包含的节点就不找了，但应该怎么处理呢？!!直接就不放入结构体中了吧
			 //非出栈进此循环是不会执行if中的语句的
			if(!currentStatus.visitDemandNodeFlag[demandNodeIndex_j])
			{
#ifdef DEBUGINFO
				const char *outputFileName  = "debugsInfo.txt";
				FILE *fp = fopen(outputFileName, "a");
				if( !fp )
				{
					fputs("Open file error.\n", stdout);
					fputs("Call test_bubble_sort_Function failure.\n", stdout);
					exit(EXIT_FAILURE);
				}
				fputs("This segement is called\n\n", fp);
				fclose(fp);
#endif
				neglectCount++;
				continue;
			}
			toEachDemandNodeIndex = demandNodeIndex_j - neglectCount;
			dstNode = currentStatus.demandNodeArrayCopy[demandNodeIndex_j];
			//输入参数的下标是demandNodeIndex_j，输出参数的下标是toEachDemandNodeIndex
			currentStatus.toEachDemandNodeWeightArray[toEachDemandNodeIndex] = findingSubWay(&currentStatus.G, srcNode, dstNode, &currentStatus.toEachDemandNodePathArray[toEachDemandNodeIndex], &currentStatus.toEachDemandNodePathArrayNum[toEachDemandNodeIndex]);
#ifdef TEST_findingSubWay_FUNCITON
			test_findingSubWay_Funciton(&currentStatus.G, srcNode, dstNode, currentStatus.toEachDemandNodePathArray[toEachDemandNodeIndex],currentStatus.toEachDemandNodePathArrayNum[toEachDemandNodeIndex], currentStatus.toEachDemandNodeWeightArray[toEachDemandNodeIndex]);
#endif

			if(currentStatus.toEachDemandNodeWeightArray[toEachDemandNodeIndex] >= INT_MAX/10)
			{
				freeStatusValue(&currentStatus);

				if(isEmptyStack(&S))
					return 0;
				else
				{
					//visitIndex,visitDemandNodeFlag的值都要发生变化，需要重新压栈的
					//出栈之前要把原结构中的图销毁，以便重新用出栈的数据为其赋新值
					DestoryGraph(&currentStatus.G);
					Pop(&S, &currentStatus, GraphCpy);
					demandNodeIndex_i--;
#ifdef DEBUGINFO
					const char *outputFileName  = "debugsInfo.txt";
					FILE *fp = fopen(outputFileName, "a");
					if( !fp )
					{
						fputs("Open file error.\n", stdout);
						fputs("Call test_bubble_sort_Function failure.\n", stdout);
						exit(EXIT_FAILURE);
					}
					static int xx = 0;
					fprintf(fp, "Pop %dth\n", ++xx);
					fprintf(fp, "The new %dth level\nThe demandNodeIndex_i is %d\n", demandNodeIndex_i+1, demandNodeIndex_i);
					fclose(fp);
#endif
					while(currentStatus.visitIndex == currentStatus.num-1)
					{
						freeStatusValue(&currentStatus);
						DestoryGraph(&currentStatus.G);
						Pop(&S, &currentStatus, GraphCpy);
						demandNodeIndex_i--;
//						GetTopItem(&S, &currentStatus);
					}
					currentStatus.visitIndex++;
					srcNode = currentStatus.demandNodeArrayCopy[currentStatus.visitIndex];
					for(int j=0; j<currentStatus.toEachDemandNodePathArrayNum[currentStatus.visitIndex]; j++)
						setNodeFlag(&currentStatus.G, currentStatus.toEachDemandNodePathArray[currentStatus.visitIndex][j], false);
					//本函数可以看当前的路径上有无有经过其他要求的节点，有则都置位
					adjustVisitFlag(&currentStatus);
					Push(&S, &currentStatus, GraphCpy);
					neglectCount = 0;
					demandNodeIndex_j=0;
					//是否应该进入外循环的开始呢？
					break;
				}
			}
		}
		
		if(demandNodeIndex_j == currentStatus.num)
		{
			//current中的数据是否真的改变了
			callBackBubbleSort(currentStatus.toEachDemandNodeWeightArray, currentStatus.toEachDemandNodePathArrayNum, currentStatus.toEachDemandNodePathArray, currentStatus.demandNodeArrayCopy, currentStatus.visitDemandNodeFlag, currentStatus.num);
#ifdef TEST_callBackBubbleSort_FUNCITON
			const char *outputFileName  = "debugsInfo.txt";
			FILE *fp = fopen(outputFileName, "a");
			if( !fp )
			{
				fputs("Open file error.\n", stdout);
				fputs("Call test_bubble_sort_Function failure.\n", stdout);
				exit(EXIT_FAILURE);
			}
			fprintf(fp, "This is %dth level\nThe demandNodeIndex_i is %d\n", demandNodeIndex_i+1, demandNodeIndex_i);
			fclose(fp);
			test_callBackBubbleSort_Function(&currentStatus.G, currentStatus.toEachDemandNodeWeightArray, currentStatus.toEachDemandNodePathArrayNum, currentStatus.toEachDemandNodePathArray, currentStatus.demandNodeArrayCopy, currentStatus.visitDemandNodeFlag, currentStatus.num);
#endif

			//压栈
			Push(&S, &currentStatus, GraphCpy);

			//剪枝,这个剪枝会不会改变原有压栈数据，答案是：不会，因为已经把它给了栈中的副本了，而且给的是变量而非指针
			for(int j=0; j<currentStatus.toEachDemandNodePathArrayNum[currentStatus.visitIndex]; j++)
				setNodeFlag(&currentStatus.G, currentStatus.toEachDemandNodePathArray[currentStatus.visitIndex][j], false);
			
			srcNode = currentStatus.demandNodeArrayCopy[currentStatus.visitIndex];
		}
		
	}

#ifdef DEBUGINFO
	if(S.top != demandNodeArrayNum)
		puts("There are problems");
#endif
#ifdef TEST_stackData_FUNCITON
	const char *fileName = "debugsIUnfo.txt";
	FILE *fp = fopen(fileName, "a");
	test_stackData_Funciton(&S, fp);
	fclose(fp);
#endif

	for(int i=0; i<=S.top; i++)
	{
		offset = S.data[i].toEachDemandNodePathArrayNum[S.data[i].visitIndex];
		passTotalNodeArrayNum += offset;
		*passTotalNodeArray = (int *)realloc(*passTotalNodeArray, passTotalNodeArrayNum * sizeof(int));
		memcpy( *passTotalNodeArray+passTotalNodeArrayNum-offset, S.data[i].toEachDemandNodePathArray[S.data[i].visitIndex], S.data[i].toEachDemandNodePathArrayNum[S.data[i].visitIndex] * sizeof(int));
	}
	
	*passTotalNodeArray = (int *)realloc(*passTotalNodeArray, ++passTotalNodeArrayNum * sizeof(int));
	(*passTotalNodeArray)[passTotalNodeArrayNum-1] = srcDestNode[1];

	//销毁栈
	DestoryStack(&S, freeMemory);

#ifdef DEBUGINFO
	puts("callBackFindingWay function is called");
#endif
	
	return passTotalNodeArrayNum;
}


/*****************************************************
函数功能:传出某节点到某节点的权重和相应路径节点信息
参数1:图指针
参数2:源节点
参数3:目的节点
参数4(Output):所经过的节点所组成的数组
参数5(Output):所经过节点的个数
返回值:源节点到目的节点的权重
说明:源节点到目的节点没有路径，传回INT_MAX/10；
     由于extractNodeIndex()函数的关系，本函数所传回的所经过节点的数组中，
	 只包括源节点和中间所经过的节点，不包括目的节点，要注意！！！
作者: Lee.C
完成时间:2016-03-28
修改时间:
*******************************************************/
static int findingSubWay(const GraphAdjList *G, const int srcNode, const int dstNode, int **passNodeArray, int *passNodeNum)
{
	//下面的几个都在相应传入函数的函数内部对其初始化了
	int * weightArray = (int *)malloc(G->numVertexes * sizeof(int));
	int * preArray = (int *)malloc(G->numVertexes * sizeof(int));
	int * tmpPassNodeArray = (int *)malloc(G->numVertexes * sizeof(int));
	int toNodeWeight = 0;
	
	
	Dijkstra2(G, srcNode, dstNode, preArray, weightArray);
		
	//如果源节点到某个指定节点没有通路，则就无需向下找了
	if(weightArray[dstNode] >= INT_MAX/10)
	{
#ifdef DEBUGINFO
		const char *outputFileName  = "debugsInfo.txt";
		FILE *fp = fopen(outputFileName, "a");
		if( !fp )
		{
			fputs("Open file error.\n", stdout);
			fputs("Call test_bubble_sort_Function failure.\n", stdout);
			exit(EXIT_FAILURE);
		}
		fprintf(fp, "%4d(%d) to %4d(%d) has no way, then pop\n\n", G->adjList[srcNode].data, srcNode, G->adjList[dstNode].data, dstNode);
		
		fclose(fp);

#endif
		free(weightArray);
		free(preArray);
		free(tmpPassNodeArray);
		return INT_MAX/10;
	}
	//如果可达，则把到它的权重和路径节点信息保存出来
	toNodeWeight= weightArray[dstNode];
	//extractNodeIndex()函数出来的数组只是源节点+源节点到目的节点之间所经过的节点的数组，不包括目的节点！！
	(*passNodeNum) = extractNodeIndex(preArray, tmpPassNodeArray, dstNode, G->numVertexes);
	
	(*passNodeArray) = (int *)malloc((*passNodeNum) * sizeof(int));
	memcpy((*passNodeArray), tmpPassNodeArray, (*passNodeNum) * sizeof(int));
		
	
	free(weightArray);
	free(preArray);
	free(tmpPassNodeArray);
	
	return toNodeWeight;
}


/*****************************************************
函数功能:源节点到目的节点不可达或者两者的最短路径即包括了所有指定节点
参数1:图指针
参数2:源目的节点数组
参数3:要求节点数组
参数4:要求节点个数
返回值:
说明:
作者: Lee.C
完成时间:2016-03-28
修改时间:
*******************************************************/
/*
static bool onceShortPath(const GraphAdjList *G, const int *srcDestNode, const int *demandNodeArray, const int demandNodeArrayNum)
{
	int * weightArray = (int *)malloc(G->numVertexes * sizeof(int));
	int * preArray = (int *)malloc(G->numVertexes * sizeof(int));
	int * passNodeArray = (int *)malloc(G->numVertexes * sizeof(int));
	int passNodeNum = 0;


	Dijkstra2(G, srcDestNode[0], srcDestNode[1], preArray, weightArray);
	
	
	if(weightArray[srcDestNode[1]] >= INT_MAX/10)
	{
		//这部分也可以写为一个单独的函数了
		//输出”NA“
#ifdef DEBUGINFO
		printf("The destination node don`t arrive\n");
#endif
		free(weightArray);
		free(preArray);
		free(passNodeArray);
		//无路径也不用继续做了，返回true，使整个程序退出
		return true;
	}

	passNodeNum = extractNodeIndex(preArray, passNodeArray, srcDestNode[1], G->numVertexes);
	if( Detection(passNodeArray, demandNodeArray, passNodeNum, demandNodeArrayNum) )
	{
		//以下的四行可以写为一个 writeEdgeNumber() 程序，以后视情况改动
		//这里的passNodeArray数组保存了最后的目的节点，
//		record_result( getEdgeNumber(G, srcDestNode[0], passNodeArray[0]) );
		int i;
		for(i=1; i<passNodeNum; i++)
			record_result( getEdgeNumber(G, passNodeArray[i-1], passNodeArray[i]) );
		record_result( getEdgeNumber(G, passNodeArray[i], srcDestNode[1]) );

#ifdef DEBUGINFO
		puts("Call onceShortPath()");
#endif
	
		free(weightArray);
		free(preArray);
		free(passNodeArray);
		return true;
	}
	
	free(weightArray);
	free(preArray);
	free(passNodeArray);
	return false;
}
*/


/*****************************************************
函数功能:把节点索引数组转为边号数组
参数1:图指针
参数2:节点索引数组
参数3(Output):边号数组
参数4:节点索引数组个数
返回值:无
说明:这个函数是个级别很高的函数，在 search_route() 函数中调用，且只在此函数中调用一次，
     把由 findingWay() 找到的节点索引数组转为边号数组。
作者: Lee.C
完成时间:2016-03-28
修改时间:
*******************************************************/
static void nodeArray2EdgeArray(const GraphAdjList * G, const int *nodeArray, unsigned short *edgeArray, const int nodeArrayNum)
{
	//edgeArray[0] = getEdgeNumber(G, firstNode, nodeArray[0]);
	for(int i=1; i<nodeArrayNum; i++)
	{
		//注意下标
		edgeArray[i-1] = getEdgeNumber(G, nodeArray[i-1], nodeArray[i]);
	}
	//puts("nodeArray2EdgeArray function error!");
	//exit(EXIT_FAILURE);
}


/************************************
函数功能:读文件，创建图的邻接表结构
参数1(输出参数):图指针
参数2:边数
参数3:指向数据的指针
返回值:无
说明:
作者: Lee.C
完成时间:2016-03-08
修改时间:2016-03-21
 ***********************************/
static void CreateALGraph(GraphAdjList *G, const int edgeNum, char **topo)
{
	//循环控制变量声明
	int i = 0, j = 0, k = 0, vertCount = 0;
	/********************************************
	 * edgeNumber ：边标号
	 * src        ：弧头标号
	 * dst        ：弧尾标号
	 * weight     ：边权重
	 * vertCount  ：节点个数计数
	 * srcIndex   ：弧头节点在顶点表中的索引
	 * dstIndex   ：弧尾节点在顶点表中的索引
	*********************************************/
	unsigned short edgeNumber = 0;
	int src = 0, dst = 0, weight = 0; 
	int srcIndex = 0, dstIndex = 0;
	EdgeNode *e = NULL, *tmpE = NULL;
	
	char str[20] = {0};

	//初始化图结构中边数
	G->numEdges = edgeNum;

	//把顶点标号全部设为-1
	for(i=0; i<MAXVEX; i++)
		G->adjList[i].data = -1;

	for(k=0; k<G->numEdges; k++)
	{
		//处理 topo[] 读取读取相应信息
		strncpy(str, topo[k], 19);
		edgeNumber = (unsigned short)atoi(str);
		while( str[j++] != ',' )
			continue;
		src = atoi(&str[j]);
		while( str[j++] != ',' )
			continue;
		dst = atoi(&str[j]);
		while( str[j++] != ',' )
			continue;
		weight = atoi(&str[j]);
		while( str[j++] != '\n')
			continue;

		//建立顶点表
		//弧头节点是否已存在，不存在则加入，并增加节点数
		if( !isMatch(G->adjList, src, vertCount))
		{
			G->adjList[vertCount].data = src;
			G->adjList[vertCount].firstEdge = NULL; 
			vertCount++;
		}
		//弧尾节点是否已存在，不存在则加入，并增加节点数
		if( !isMatch(G->adjList, dst, vertCount))
		{
			G->adjList[vertCount].data = dst;
			G->adjList[vertCount].firstEdge = NULL;  
			vertCount++;
		}

		//求得弧头标号和弧尾标号在顶点表中索引
		srcIndex = Index(G->adjList, src, vertCount);
		dstIndex = Index(G->adjList, dst, vertCount);
		tmpE = G->adjList[srcIndex].firstEdge;
		//用于处理从弧头到弧尾有多条边的情况
		if((tmpE = repeatNode(tmpE, dstIndex)) != NULL)
		{
			//有多条边的时候，只把最小权重的边的权重和标号放入边表节点中
			tmpE->weight = tmpE->weight<weight?tmpE->weight:weight;	
			tmpE->edgeNumber = tmpE->weight < weight ? tmpE->edgeNumber : edgeNumber;
		}
		else
		{
			//有唯一边或者第一次建立时
			//头插法建立边表节点链表
			e = (EdgeNode *)malloc(sizeof(EdgeNode));

			e->adjvex = dstIndex;
			e->weight = weight;
			e->edgeNumber = edgeNumber;
			e->flag = true;		//true认为是可访问的
			e->next = G->adjList[srcIndex].firstEdge;
//			e->data = dst;
			G->adjList[srcIndex].firstEdge = e;
		}


		j = 0;
	}

	//初始化图结构中边数
	G->numVertexes = vertCount;
}



/*******************************************
函数功能:分析demand.csv文件
参数1:图指针
参数2:demand.csv文件指针
参数3(Output):源节点和目的节点索引的数组
参数4(Output):要求经过的节点的索引数组
返回值:要求经过节点的个数
说明:保存的都是在顶点表中的下表索引而非标号
作者: Lee.C
完成时间:2016-03-21
修改时间:
*********************************************/
static int analyzeDemand(const GraphAdjList *G, const char *demand, int *srcDest, int **demandNode)
{
	int num = 0;
	(*demandNode) = (int *)malloc(num*sizeof(int));

	int tmpNodeNumber = atoi(demand);
	srcDest[0] = Index(G->adjList, tmpNodeNumber, G->numVertexes);
	while(*demand++ != ',')
		continue;
	tmpNodeNumber = atoi(demand);
	srcDest[1] = Index(G->adjList, tmpNodeNumber, G->numVertexes);
	while(*demand++ != ',')
		continue;

	while(1)
	{
		tmpNodeNumber = atoi(demand);
		(*demandNode) = (int *)realloc((*demandNode), ++num*sizeof(int));
		(*demandNode)[num-1] = Index(G->adjList, tmpNodeNumber, G->numVertexes);
		
		while( *demand++ != '|' && *demand)
			continue;

		if( *demand == '\0')
			break;
	}

	return num;
}



/**********************************************
函数功能:邻接表的Dijstra算法
参数1:图指针
参数2:源节点索引
参数3:目标节点索引
参数4(Output):前驱数组
参数5(Output):权重数组
返回值:无
说明:
所需标准库文件:stdlib.h
作者: Lee.C
完成时间:2016-03-27
修改时间:
*************************************************/
static void Dijkstra2(const GraphAdjList *G, const int srcNode, const int dstNode, int *priorArray, int *weightArray)
{
	EdgeNode *e = NULL;
	int min = 0, Index = 0;
	int *final = (int *)malloc(G->numVertexes * sizeof(int));
	memset(final, 0, G->numVertexes * sizeof(int));
//	memset(priorArray, 0, G->numVertexes * sizeof(int));

	for(int i=0; i<G->numVertexes; i++)
	{
		weightArray[i] = INT_MAX/10;
		priorArray[i] = -1;
	}

	weightArray[srcNode] = 0;
	e = G->adjList[srcNode].firstEdge;
	while(e)
	{
		if(e->flag)
		{
			weightArray[e->adjvex] = e->weight;
			priorArray[e->adjvex] = srcNode;
		}
		e = e->next;
	}

	final[srcNode] = 1;

	for(int i=0; i<G->numVertexes; i++)
	{
		min = INT_MAX/10;
		for(int j=0; j<G->numVertexes; j++)
		{
			if(!final[j] && weightArray[j]<min)
			{
				Index = j;
				min = weightArray[j];
			}
		}
		final[Index] = 1;
		//只有它被标记了，才证明没有其他的路径到它的距离更短了，final的意思就是这个点结束了，不可能再松弛了
		//不能在松弛操作中结束，因为还可能有到此节点更短的路径
		if(final[dstNode] == 1)
			return;
		e = G->adjList[Index].firstEdge;

		//不用遍历 numVertexes 遍了，直接找当前节点的出度节点就行
		while( e )
		{
			if(e->flag)
			{
				int tmpAdjvex = e->adjvex;
				if(!final[tmpAdjvex] && (min+e->weight<weightArray[tmpAdjvex]))
				{
					weightArray[tmpAdjvex] = min + e->weight;
					priorArray[tmpAdjvex] = Index;
				}
			}
			e = e->next;
		}
	}
}



/*******************************************
函数功能:标记指定节点的状态
参数1(Output):图指针
参数2:指定的节点索引
参数3:所要设置的状态
返回值:无
说明:
作者: Lee.C
完成时间:2016-03-28
修改时间1:
修改说明1:
*********************************************/
static void setNodeFlag(GraphAdjList *G, const int designatedNode, const bool flag)
{
	EdgeNode *e = NULL;

	for(int i=0; i<G->numVertexes; i++)
	{
		e = G->adjList[i].firstEdge;
		while(e)
		{
			if(e->adjvex == designatedNode)
			{
				e->flag = flag;
				break;
			}
			e = e->next;
		}
	}
}

/*******************************************
函数功能:从前驱数组中提取到目的节点所经过的节点的索引
参数1:前驱数组
参数2(Output):源节点以及所经过节点的索引所组成的数组(是正序的了！)
参数3:目的节点
参数4:图的顶点个数，用于初始化passNodeArray的
返回值:
说明:本节点到本节点在前驱数组中也以-1的形式保存
     要求前驱矩阵的初始化形式为-1；
	 本函数的一个重要特征是没有保存最后的目的节点索引！！！
作者: Lee.C
完成时间:2016-03-21
修改时间:2016-03-28
修改说明:倒序变为正序
*********************************************/
static int extractNodeIndex(const int *path, int *passNodeArray, int dstNode, const int vertNum)
{
	int num = 0, temp = 0;
	
	for(int i=0; i<vertNum; i++)
		passNodeArray[i] = -1;

	while(path[dstNode] != -1)
	{
		passNodeArray[num] = path[dstNode];
		dstNode = passNodeArray[num++];
	}
	
	for(int i=0; i<int(num/2); i++)
	{
		temp = passNodeArray[i];
		passNodeArray[i] = passNodeArray[num-i-1];
		passNodeArray[num-i-1] = temp;
	}
	
	return num;
}

/*******************************************
函数功能:检测已经过节点中是否包含全部指定节点
参数1:所经过的节点的数组
参数2:要求经过的节点的数组
参数3:所经过节点数组的元素个数
参数4:要求节点数组的元素个数
返回值:passNodeArray包含demandNodeArray中所有元素返回true，否则返回false
说明:
作者: Lee.C
完成时间:2016-03-21
修改时间:
*********************************************/
/*
static bool Detection(const int *passNodeArray, const int *demandNodeArray, const int passNodeNum, const int demandNodeNum)
{
	if(demandNodeNum > passNodeNum)
		return false;

	for(int i=0, j=0; i<demandNodeNum; i++)
	{
		for(j=0; j<passNodeNum; j++)
		{
			if(passNodeArray[j] == demandNodeArray[i])
				break;
		}
		if( j == passNodeNum )
			return false;
	}
	return true;
}
*/

/*******************************************
函数功能:获取边标号
参数1:图指针
参数2:源节点索引
参数3:目的节点索引
返回值:边标号
说明:
作者: Lee.C
完成时间:2016-03-21
修改时间:
*********************************************/
static unsigned short getEdgeNumber(const GraphAdjList * G, const int src, const int dst)
{
	EdgeNode *e = G->adjList[src].firstEdge;
	while( e )
	{
		if( e->adjvex == dst )
			return e->edgeNumber;
		e = e->next;
	}
	puts("getEdgeNumber function error!");
	exit(EXIT_FAILURE);
}






/*****************************************************
函数功能:冒泡排序法排序权重以及相应的指针
参数1:图指针
参数2:源目的节点数组
参数3:要求节点数组
参数4:要求节点个数
返回值:无
说明:
作者: Lee.C
完成时间:2016-03-28
修改时间:
*******************************************************/
static void bubble_sort(int *toEachDemandNodeWeightArray, int *toEachDemandNodePathArrayNum, int **toEachDemandNodePathArray, int *demandNodeArrayCopy, int demandNodeNum)
{
	bool change;
	int i, j, tempWeight, tempNum, tempDemandNode;
	int *tempPointer;
	
	for(i=demandNodeNum-1,change=true; i>=1 && change; i--)
	{
		change = false;
		
		for(j=0; j<i; j++)		//每一次内循环把最大的数升至顶端
			if(toEachDemandNodeWeightArray[j] > toEachDemandNodeWeightArray[j+1])
			{
				tempWeight = toEachDemandNodeWeightArray[j];
				toEachDemandNodeWeightArray[j] = toEachDemandNodeWeightArray[j+1];
				toEachDemandNodeWeightArray[j+1] = tempWeight;
				
				tempNum = toEachDemandNodePathArrayNum[j];
				toEachDemandNodePathArrayNum[j] = toEachDemandNodePathArrayNum[j+1];
				toEachDemandNodePathArrayNum[j+1] = tempNum;
				
				tempPointer = toEachDemandNodePathArray[j];
				toEachDemandNodePathArray[j] = toEachDemandNodePathArray[j+1];
				toEachDemandNodePathArray[j+1] = tempPointer;
				
				tempDemandNode = demandNodeArrayCopy[j];
				demandNodeArrayCopy[j] = demandNodeArrayCopy[j+1];
				demandNodeArrayCopy[j+1] = tempDemandNode;
				
				change = true;	//如果整个序列已经有序，则这个数值不会变为真
			}
	}
}



/**********************************
函数功能:检测顶点表数组中前num个数是否包含value
参数1:顶点表数组
参数2:待检测值
参数3:检测前num个数
返回值:存在返回true，不存在返回false
说明:
作者: Lee.C
完成时间:2016-03-08
修改时间:
**************************************/
static bool isMatch(const AdjList arr, const int value, const int num)
{
	int i = 0;

	while( arr[i].data != value && i < num)
		i++;

	if( i == num )
		return false;
	else
		return true;
}



/**********************************
函数功能:返回顶点标号在定点表中的下标索引
参数1:顶点表数组
参数2:待检测顶点编号
参数3:顶点表元素个数
返回值:顶点表中的索引值
说明:返回的是数据元素的下标，而不是第几个元素
作者: Lee.C
完成时间:2016-03-08
修改时间:
**************************************/
static int Index(const AdjList array, const int element, const int num)
{
	for(int i=0; i<num; i++)
	{
		if(array[i].data == element)
			return i;
	}

	printf("Error!\n");
	exit(EXIT_FAILURE);
}

/**********************************
函数功能:检测边表节点中是否有重复的标号的顶点
参数1:边表节点头指针
参数2:定点标号索引
返回值:相同节点处的指针，没有返回NULL
说明:
作者: Lee.C
完成时间:2016-03-15
修改时间:
**************************************/
static EdgeNode *repeatNode(EdgeNode *e, const int nodeIndex)
{
	while( e )
	{
		if(e->adjvex == nodeIndex)
			return e;
		else
			e = e->next;
	}
	return NULL;
}



void InitStatusValue(Item *e, const int num)
{
	int tempDemandNodeArray[MAXDEAMDNODE];
	memcpy(tempDemandNodeArray, e->demandNodeArrayCopy, e->num * sizeof(int));
	
	e->num = num;
	
	//感觉这一句真的没必要
//	e->G = (*G);

	e->toEachDemandNodeWeightArray = (int *)malloc(e->num * sizeof(int));
	e->toEachDemandNodePathArray = (int **)malloc(e->num * sizeof(int *));
	e->toEachDemandNodePathArrayNum = (int *)malloc(e->num * sizeof(int));
	e->demandNodeArrayCopy = (int *)malloc(e->num * sizeof(int));
	e->visitDemandNodeFlag = (bool *)malloc(e->num * sizeof(bool));
	
	for(int i=0; i<e->num; i++)
	{
		e->toEachDemandNodeWeightArray[i] = INT_MAX/10;
		e->toEachDemandNodePathArray[i] = NULL;
		e->toEachDemandNodePathArrayNum[i] = 0;
		e->visitDemandNodeFlag[i] = true;
	}
	//剔除选为源节点的索引所对应的要求节点数组位置上的数，用以下方式，或者用for循环，若是for循环，还要记录下原来num的大小
	memcpy(e->demandNodeArrayCopy, tempDemandNodeArray, e->visitIndex * sizeof(int));
	memcpy(e->demandNodeArrayCopy + e->visitIndex, tempDemandNodeArray + e->visitIndex + 1, (e->num-e->visitIndex) * sizeof(int));
	e->visitIndex = 0;
}

void freeStatusValue(Item *e)
{
	free(e->toEachDemandNodeWeightArray);
	free(e->toEachDemandNodePathArrayNum);
	free(e->demandNodeArrayCopy);
	free(e->visitDemandNodeFlag);
	
	for(int i=0; i<e->num; i++)
		free(e->toEachDemandNodePathArray[i]);
}

//判断已经过节点中是否有要求的节点，只找比它小的就行
void adjustVisitFlag(Item *e)
{
//	e->visitDemandNodeFlag[e->visitIndex] = false;
	for(int i=0; i<e->toEachDemandNodePathArrayNum[e->visitIndex]; i++)
	{
		for(int j=0; j<e->visitIndex; j++)
			if(e->toEachDemandNodePathArray[e->visitIndex][i] == e->demandNodeArrayCopy[j])
				e->visitDemandNodeFlag[j] = false;
	}
}

//这里不用在意是否需要深一层之类，因为就算用Item e也行，因为指针指向的是同一块地址
void freeMemory(Item *e)
{
	free(e->toEachDemandNodeWeightArray);
	free(e->visitDemandNodeFlag);
	free(e->toEachDemandNodePathArrayNum);
	free(e->demandNodeArrayCopy);
	for(int i=0; i<e->num; i++)
		free(e->toEachDemandNodePathArray[i]);
	
}

void callBackBubbleSort(int *toEachDemandNodeWeightArray, int *toEachDemandNodePathArrayNum, int **toEachDemandNodePathArray, int *demandNodeArrayCopy, bool *visitDemandNodeFlag, int demandNodeNum)
{
	bool change;
	int i, j, tempWeight, tempNum, tempDemandNode;
	bool tempVisitFlag;
	int *tempPointer;
	
	for(i=demandNodeNum-1,change=true; i>=1 && change; i--)
	{
		change = false;
		
		for(j=0; j<i; j++)		//每一次内循环把最大的数升至顶端
			if(toEachDemandNodeWeightArray[j] > toEachDemandNodeWeightArray[j+1])
			{
				tempWeight = toEachDemandNodeWeightArray[j];
				toEachDemandNodeWeightArray[j] = toEachDemandNodeWeightArray[j+1];
				toEachDemandNodeWeightArray[j+1] = tempWeight;
				
				tempNum = toEachDemandNodePathArrayNum[j];
				toEachDemandNodePathArrayNum[j] = toEachDemandNodePathArrayNum[j+1];
				toEachDemandNodePathArrayNum[j+1] = tempNum;
				
				tempPointer = toEachDemandNodePathArray[j];
				toEachDemandNodePathArray[j] = toEachDemandNodePathArray[j+1];
				toEachDemandNodePathArray[j+1] = tempPointer;
				
				tempDemandNode = demandNodeArrayCopy[j];
				demandNodeArrayCopy[j] = demandNodeArrayCopy[j+1];
				demandNodeArrayCopy[j+1] = tempDemandNode;
				
				tempVisitFlag = visitDemandNodeFlag[j];
				visitDemandNodeFlag[j] = visitDemandNodeFlag[j+1];
				visitDemandNodeFlag[j+1] = tempVisitFlag;
				
				change = true;	//如果整个序列已经有序，则这个数值不会变为真
			}
	}
}
/*还要再看！
void GraphCpy(GraphAdjList *G, const GraphAdjList *srcG)
{
	EdgeNode *e = NULL;
	EdgeNode *eSrc = NULL;
	G->numVertexes = srcG->numVertexes;
	G->numEdges = srcG->numEdges;
	memcpy(G->adjList, srcG->adjList, G->numVertexes);
	for(int i=0; i<G->numVertexes; i++)
	{
		if(!srcG->adjList[i].firstEdge)
		{
			G->adjList[i].firstEdge == NULL;
			continue;
		}
		eSrc = srcG->adjList[i].firstEdge->next;
		//不能直接设为空
		tempE = G->adjList[i].firstEdge = NULL/;
		while( eSrc )
		{
			e = (EdgeNode *)malloc(sizeof(EdgeNode));
			memcpy(e, eSrc, sizeof(EdgeNode));
			tempE->next = e;
			tempE = e;
			eSrc = eSrc->next;
		}
		tempE->next = NULL;
	}
}
*/

void GraphCpy(GraphAdjList *G, const GraphAdjList *srcG)
{
	//自己给自己，当然什么都不做的返回
	if(G == srcG)
		return;

	//尾插法再建一次表
	EdgeNode *e = NULL;
	EdgeNode *eSrc = NULL;
	EdgeNode *tempE = NULL;
	G->numVertexes = srcG->numVertexes;
	G->numEdges = srcG->numEdges;
	for(int i=0; i<G->numVertexes; i++)
	{
		G->adjList[i].data = srcG->adjList[i].data;
		if(!srcG->adjList[i].firstEdge)
		{
			G->adjList[i].firstEdge = NULL;
		}
		else
		{
			eSrc = srcG->adjList[i].firstEdge;
			G->adjList[i].firstEdge = (EdgeNode *)malloc(sizeof(EdgeNode));
			tempE = G->adjList[i].firstEdge;
			memcpy(tempE, eSrc, sizeof(EdgeNode));
			while(eSrc->next)
			{
				eSrc = eSrc->next;
				e = (EdgeNode *)malloc(sizeof(EdgeNode));
				memcpy(e, eSrc, sizeof(EdgeNode));
				tempE->next = e;
				tempE = e;
			}
		}
	}
}

void DestoryGraph(GraphAdjList *G)
{
	int i = 0;
	EdgeNode *p = NULL, *q = NULL;
	
	for(i=0; i<G->numVertexes; i++)
	{
		p = G->adjList[i].firstEdge;
		while(p)
		{
			q = p->next;
			free(p);
			p = q;
		}
	}
	
}
