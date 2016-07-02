#ifndef __ROUTE_H__
#define __ROUTE_H__

#define MAXVEX 600
#define MAXDEAMDNODE 50

//typedef int VertexType;
//边表节点
typedef struct EdgeNode
{
	int adjvex;		//顶点在顶点表中的下标索引
	int weight;		//边权重
	unsigned short edgeNumber;	//边号
	bool flag;		//顶点是否访问标记
	struct EdgeNode * next;
}EdgeNode;

//顶点表
typedef struct VertexNode
{
	int data;     //顶点标号

	EdgeNode *firstEdge;
}VertexNode, AdjList[MAXVEX];

typedef struct
{
	AdjList adjList;
	int numVertexes, numEdges;
}GraphAdjList;

void search_route(char *graph[5000], int edge_num, char *condition);


#endif
