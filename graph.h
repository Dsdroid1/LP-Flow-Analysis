#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#ifndef GRAPH_H
#define GRAPH_H

#define MAX_NO_OF_VERTICES 10

//Will use an adjancency list to store the flow graph
typedef struct NodeLL{
    int BasicBlockIndex;
    struct NodeLL * next;
}LinkList;

struct BasicBlockNode{
    int leader; //This will contain the indedx of the leader stmt from the stmt array
    int end_stmt; //Index of the last stmt of this block
    LinkList *edges;
};

typedef struct BasicBlockNode BasicBlock;

typedef struct FlowGraph
{
    int N;
    BasicBlock Nodelist[MAX_NO_OF_VERTICES];
}FlowGraph;

typedef enum {FAILURE,SUCCESS} sc;

//---GRAPH USAGE FNS----------------------
void InitGraph(FlowGraph *G);
sc AddBasicBlock(FlowGraph *G,int leader,int end_stmt);
sc AddEdge(FlowGraph *G,int startBlock,int endBlock);
void PrintGraph(FlowGraph *G);
void DisplayLeaderFromGraph(FlowGraph *G);
void DeleteGraph(FlowGraph *G);

#endif