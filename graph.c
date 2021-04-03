#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"graph.h"

void InitGraph(FlowGraph *G){
    G->N = 0;
    //Initially no node
}

sc AddBasicBlock(FlowGraph *G,int leader,int end_stmt){
    sc retval = SUCCESS;
    //Add data at Nth position
    if(G->N<MAX_NO_OF_VERTICES-1)
    {
        (G->Nodelist[G->N]).leader = leader;
        (G->Nodelist[G->N]).end_stmt = end_stmt;
        (G->Nodelist[G->N]).edges = NULL;
        G->N=G->N+1;

    }
    else
    {
        retval = FAILURE;
        //No space
    }
    return retval;
}

sc AddEdge(FlowGraph *G,int startBlock,int endBlock)
{
    sc retval = SUCCESS;
    //Some simple checks of validity
    int num_nodes=G->N;
    if(num_nodes>startBlock && num_nodes>endBlock)
    {
        //Valid node indices
        //Now update LinkList
        //If first node,then
        if(G->Nodelist[startBlock].edges == NULL)
        {
            G->Nodelist[startBlock].edges = (LinkList *)malloc(sizeof(LinkList));
            if (G->Nodelist[startBlock].edges == NULL)
            {
                //Failure in alloting memory
                retval = FAILURE;
            }
            else
            {
                G->Nodelist[startBlock].edges->BasicBlockIndex = endBlock;
                G->Nodelist[startBlock].edges->next = NULL;   
            }
        }
        //Else the LL exists
        else
        {
            LinkList *nodes = G->Nodelist[startBlock].edges;
            while(nodes->next != NULL)
            {
                nodes = nodes->next;
            }
            nodes->next = (LinkList *)malloc(sizeof(LinkList));
            if (nodes->next == NULL)
            {
                retval = FAILURE;
            }
            else
            {
                nodes = nodes->next;
                nodes->next = NULL;
                nodes->BasicBlockIndex = endBlock;
            }
        }
    }
    else
    {
        retval = FAILURE;
    }
    return retval;
}


void PrintGraph(FlowGraph *G)
{
    int num_nodes = G->N;
    LinkList *node;
    for(int i=0;i<num_nodes;i++)
    {
        printf("\nBasic Block %d starts at stmt %d and ends at stmt %d",i,G->Nodelist[i].leader,G->Nodelist[i].end_stmt);
        node=G->Nodelist[i].edges;
        while(node != NULL)
        {
            printf("\nBasic Block %d has an edge to Basic Block %d",i,node->BasicBlockIndex);
            node=node->next;
        }
    }
}

void DisplayLeaderFromGraph(FlowGraph *G)
{
    int num_nodes = G->N;
    for (int i=0;i<num_nodes;i++)
    {
        printf("\nLeader of Basic Block %d is stmt no. %d",i,G->Nodelist[i].leader);
    }
}

void DeleteGraph(FlowGraph *G)
{
    for(int i=0;i<G->N;i++)
    {
        LinkList *head = G->Nodelist[i].edges,*trav;
        G->Nodelist[i].edges = NULL;
        trav=head;
        while(trav != NULL)
        {
            head=trav;
            trav=trav->next;
            free(head);
        }
    }
}
