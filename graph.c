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
        if(G->Nodelist[i].leader!=-1 && G->Nodelist[i].end_stmt!=-1)//Not dead code
        {
            printf("\nBasic Block %d starts at stmt %d and ends at stmt %d",i,G->Nodelist[i].leader,G->Nodelist[i].end_stmt);
            node=G->Nodelist[i].edges;
            while(node != NULL)
            {
                printf("\nBasic Block %d has an edge to Basic Block %d",i,node->BasicBlockIndex);
                node=node->next;
            }
        }
        else
        {
            printf("\nBasic Block %d detected as Dead Code",i);   
        }
    }
}

void DisplayLeaderFromGraph(FlowGraph *G)
{
    int num_nodes = G->N;
    for (int i=0;i<num_nodes;i++)
    {
        if(G->Nodelist[i].leader!=-1 && G->Nodelist[i].end_stmt!=-1)//Noe deadcode
        {
            printf("\nLeader of Basic Block %d is stmt no. %d",i,G->Nodelist[i].leader);
        }
        
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

sc DeleteDeadBlock(FlowGraph *G,int deadBlockIndex)
{
    //Free all edges of the dead block
    sc retval = SUCCESS;
    if(deadBlockIndex<G->N && deadBlockIndex>0)//BB0 cant be dead
    {
        LinkList *edges = G->Nodelist[deadBlockIndex].edges;
        LinkList *trav=edges;
        while(trav != NULL)
        {
            edges=trav;
            trav=trav->next;
            free(edges);
        }
        G->Nodelist[deadBlockIndex].edges = NULL;
        G->Nodelist[deadBlockIndex].leader = -1;
        G->Nodelist[deadBlockIndex].end_stmt = -1;
    }
    else
    {
        retval = FAILURE;
    }
    return retval;
}



void DFS(FlowGraph *G,int v,bool* visited,int blockToSkip){ 
    // blockToSkip is the block to ignore while doing dfs, very application specific
    // to do normal dfs put blockToSkip as -1.
    if(v==blockToSkip)
    {
        return;
    } 
    if(G->Nodelist[v].leader!=-1 && G->Nodelist[v].end_stmt!=-1){
        visited[v] = TRUE;
        //visit all neighbours of v
        LinkList* temp = G->Nodelist[v].edges;
        while(temp!=NULL)
        {
            if(visited[temp->BasicBlockIndex] == FALSE)
            {
                DFS(G,temp->BasicBlockIndex,visited,blockToSkip);
            }
            temp=temp->next;
        }

    }
    
    
}






void Dominates(FlowGraph *G,int nodeIndex){
    /*
        Dominates(G,X) tells you what blocks are dominated by Block X
    */
   printf("Block %d dominates\n",nodeIndex);
   bool *visited = (bool*)malloc(MAX_NO_OF_VERTICES*sizeof(bool));
   bool *visited_after = (bool*)malloc(MAX_NO_OF_VERTICES*sizeof(bool));
   for(int i=0;i<MAX_NO_OF_VERTICES;i++)
   {
       visited[i]=FALSE;
       visited_after[i]=FALSE;
   }
   DFS(G,0,visited,-1);
   DFS(G,0,visited_after,nodeIndex); //skip the block to see what it dominates
   for(int i=0;i<MAX_NO_OF_VERTICES;i++)
   {
       if(visited[i]==TRUE && visited_after[i]==FALSE)
       {
           // nodeIndex dominates block i 
           printf("Block %d\n",i);
       }
   }
   //freeing resources
   free(visited);
   free(visited_after);
} 