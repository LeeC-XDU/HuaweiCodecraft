#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct topo
{
	int edgeNumber;
	int src;
	int dst;
}Topo;

bool detection(int *arr, int count);

int main()
{
	const char *topoFileName = "topo.csv";
	const char *resultFileName = "result.csv";

	FILE * topocsv = fopen(topoFileName, "r");
	FILE * resultcsv = fopen(resultFileName, "r");
	if( !topocsv || !resultcsv )
	{
		printf("open file error!(1)\n");
		exit(EXIT_FAILURE);
	}

	Topo topo[5000];
	int topoCount = 0, topoIndex = 0;
	
	//保存经过的所有节点的数组
	int node[1000];
	int nodeArrayCount = 0;

	//存储topo.csv文件的一行字符串
	//存储sample_result.csv文件的一整行字符串
	char routeEdgeString[5000] = {0};
	int routeEdgeStringIndex = 0;

	//临时变量声明区
	//临时存储从文件读取的一行字符串数据
	char tmpStrLine[20] = {0};
	int tmpmpStrLineIndex = 0;
	int tmpEdgeNumber = 0;

	//初始化
	memset(topo, 0, 5000*sizeof(Topo));
	memset(routeEdgeString, 0, 5000);

	for(int i=0; i<1000; i++)
		node[i] = -1;

	//初始化topo数组
	while( !feof(topocsv) )
	{
		fgets(tmpStrLine, 20, topocsv);
		topo[topoCount].edgeNumber = atoi(tmpStrLine);
		while( tmpStrLine[tmpmpStrLineIndex++] != ',' )
			continue;
		topo[topoCount].src = atoi(&tmpStrLine[tmpmpStrLineIndex]);
		while( tmpStrLine[tmpmpStrLineIndex++] != ',' )
			continue;
		topo[topoCount].dst = atoi(&tmpStrLine[tmpmpStrLineIndex]);

		topoCount++;
		tmpmpStrLineIndex = 0;
	}

	fgets(routeEdgeString, 10000, resultcsv);
	if( strcmp(routeEdgeString, "NA") == 0 )
	{
		printf("NA\n");
		return 1;
	}

	tmpEdgeNumber = atoi(routeEdgeString);
	while(topo[topoIndex].edgeNumber != tmpEdgeNumber)
			topoIndex++;
	if( topoIndex==topoCount )
	{
		printf("error!\n");
		exit(EXIT_FAILURE);
	}
	printf("%d-->%d", topo[topoIndex].src, topo[topoIndex].dst);
	node[nodeArrayCount++] = topo[topoIndex].src;
	node[nodeArrayCount++] = topo[topoIndex].dst;

	topoIndex = 0;

	while(1)
	{
		while(routeEdgeString[routeEdgeStringIndex++] != '|' && routeEdgeString[routeEdgeStringIndex])
			continue;

		if(routeEdgeString[routeEdgeStringIndex] == '\0')
			break;

		tmpEdgeNumber = atoi(&routeEdgeString[routeEdgeStringIndex]);

		while(topo[topoIndex].edgeNumber != tmpEdgeNumber)
			topoIndex++;
		if( topoIndex==topoCount )
		{
			printf("error!\n");
			exit(EXIT_FAILURE);
		}

		printf("-->%d", topo[topoIndex].dst);
		node[nodeArrayCount++] = topo[topoIndex].dst;

		topoIndex = 0;
	}


	printf("\n");

	if(detection(node, nodeArrayCount))
		printf("OK\n");
	else
		printf("Error\n");


	printf("\n");

	fclose(topocsv);
	fclose(resultcsv);
	
	return 0;
}

bool detection(int *array, int arrayCount)
{
	const char *demandFileName = "demand.csv";
	FILE *fp = fopen(demandFileName, "r");
	if( !fp )
	{
		printf("open file error!");
		exit(EXIT_FAILURE);
	}

	//保存demand中的必经节点的信息
	char demandString[5000];
	int demandStringIndex = 0, demandNodeNumber = 0;
	int arrayIndex = 0;

	memset(demandString, 0, 5000);

	fgets(demandString, 5000, fp);

	while(demandString[demandStringIndex++] != ',')
		continue;

	while(demandString[demandStringIndex++] != ',')
		continue;

	demandNodeNumber = atoi(&demandString[demandStringIndex]);

	//判断是否经过了demand.csv指定的节点
	while(1)
	{
		//为1的意思是不检测源节点，因为array中保存了源和目的节点
		arrayIndex = 1;
		//最后不加那个等号，加上之后就多了一次循环，-1的意思是不检测最后的目标节点
		while(array[arrayIndex++] != demandNodeNumber && arrayIndex-1 < arrayCount)
			continue;
		if(arrayIndex-1 == arrayCount)
		{
			printf("No pass the demand node: %d\n", demandNodeNumber);
			fclose(fp);
			return false;
		}


		while(demandString[demandStringIndex++] != '|' && demandString[demandStringIndex])
			continue;
		
		if( demandString[demandStringIndex] == '\0')
			break;

		demandNodeNumber = atoi(&demandString[demandStringIndex]);
	}

	//判断经过的节点中是否包含了重复节点
	for(int i=0; i<arrayCount; i++)
	{
		for(int j=i+1; j<arrayCount; j++)
		{
			if(array[i] == array[j])
			{
				printf("Duplicate nodes: %d\n", array[i]);
				fclose(fp);
				return false;
			}
		}
	}

	fclose(fp);
	return true;

}
