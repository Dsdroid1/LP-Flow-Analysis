#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"graph.h"

//Local Optimisations Possible
//Algebraic Simplification
//Constant Folding
//Dead code elimination
//common-subexpression elimination
//copy propagation
//

//Read a file and convert it into a stmt table(separate by stmts)
char ** StmtTable(char *filename,int *TAB_LEN)
{
    FILE *fp;
    fp = fopen(filename,"r");
    char **TABLE = NULL;
    char DATA[20];
    int numLines=0,maxLineLen=0;
    if(fp != NULL)
    {
        while(fgets(DATA,20,fp) != NULL){
            numLines++;
            //maxLineLen = (maxLineLen<strlen(DATA))?strlen(DATA):maxLineLen;
        }
        //Metadata of file gathered,now create the table
        fclose(fp);
        TABLE = (char **)malloc(sizeof(char *)*(numLines+1));
        int tabIndex=0;
        fp = fopen(filename,"r");
        while(fgets(DATA,20,fp) != NULL){
            //Trim DATA,of whitespaces(\n mainly) ,here,before strdup()
            //Here while trimming,can also add functionality to 'ignore' comments
            int i=0;
            while(DATA[i]!='\0' && DATA[i]!='\n')
            {
                i++;
            }
            DATA[i]='\0';
            TABLE[tabIndex] = strdup(DATA);
            tabIndex++;
        }
        *TAB_LEN = numLines;
        fclose(fp);
    }
    return TABLE; 
}

void DisplayTABLE(char **TABLE,int TAB_LEN){
    if(TABLE != NULL)
    {
        for(int i=0;i<TAB_LEN;i++)
        {
            printf("\n%d - %s",i,TABLE[i]);
        }
    }
}

int* GetLeaders(char **TABLE,int TAB_LEN)
{
    //Start searching from searchINdex for a leader stmt
    /*
        The rules we use are of the following:
    a.The first statement is a leader.
    b.Any statement that is the target of a conditional or unconditional goto is a leader.
    c.Any statement that immediately follows a goto or conditional goto statementis a leader. 
    */
   int *isLeader = (int *)malloc(sizeof(int)*TAB_LEN);
   for(int i=0;i<TAB_LEN;i++)
   {
       isLeader[i]=0;
   }
   if(isLeader!=NULL)
   {
       isLeader[0]=1;//Rule a
       int index=0;
       char *searchptr;
        //Rule b,c
        while(index<TAB_LEN)
        {
            searchptr = strstr(TABLE[index],"goto");
            if(searchptr != NULL)
            {
                //Stmt has goto,make next stmt as a leader
                if(index+1<TAB_LEN)
                {
                    isLeader[index+1] = 1; //Rule c
                }
                //Also,have to search for where the target stmt is
                //Assume goto is followed by the index of the target stmt,for simplicity
                //This can be changed to accomodate labels
                //Validity checks to be added
                searchptr=searchptr+4;//Advannce the goto part
                int jump_to;
                sscanf(searchptr,"%d",&jump_to);
                if(jump_to < TAB_LEN)
                {
                    isLeader[jump_to]=1;
                }
            }
            index++;
        }
   }
   return isLeader;
}

void DisplayBBInfo(FlowGraph *G,char **TABLE)
{
    for(int i=0;i<G->N;i++)
    {
        //For every basic block
        if( G->Nodelist[i].leader!=-1 && G->Nodelist[i].end_stmt!=-1)//Not deadcode
        {
            printf("\n <BB%d> Start ",i);
            for(int j=G->Nodelist[i].leader;j<=G->Nodelist[i].end_stmt;j++)
            {
                printf("\nStmt %d - %s",j,TABLE[j]);
            }
            printf("\n <BB%d> end\n",i);
        }
    }
}

void DeadCodeElimination(FlowGraph *G)
{
    //For any basic block except BB0,if it does not have an incoming edge,then it is dead code
    //Later,maybe also remove the deadcode BB from the graph 
    int change=0;
    int *incoming=(int *)malloc(sizeof(int)*(G->N));
    int *marked_dead=(int *)malloc(sizeof(int)*(G->N));
    for(int i=0;i<G->N;i++)
    {
        marked_dead[i] = 0;
    }
    //This will continue analysis until there is no change detected
    //This is required as some dead code detected in first iter will lead to some edge deletion
    //Now this could potentially create more dead code,hence we iterate till no change
    do
    {
        change=0;
        for(int i=0;i<G->N;i++)
        {
            incoming[i] = 0;
        }
        for(int i=0;i<G->N;i++)
        {
            if(marked_dead[i]==0)
            {
                LinkList *edgeList = G->Nodelist[i].edges;
                while(edgeList != NULL)
                {
                    incoming[edgeList->BasicBlockIndex]++;
                    edgeList=edgeList->next;
                }
            }
        }
        //printf("\n---Dead Code Analysis-------------------");
        int deadcode=0;
        for(int i=1;i<G->N;i++)
        {
            if(incoming[i] == 0 && marked_dead[i]==0)
            {
                deadcode = 1;
                marked_dead[i] = 1;
                change=1;
                printf("\n BB%d is dead code!",i);
                DeleteDeadBlock(G,i);
            }
        }
        if(deadcode == 0)
        {
            //printf("\nNo dead code found");
        }
    }while(change==1);
    free(incoming);
    free(marked_dead);
}

void DisplayNaturalLoops(FlowGraph *G)
{
    //Get the back edges
    //For every arc from i to j, where j dom i
    //Then for every such back edge, remove j from graph and see which nodes can reach i
    //All such nodes make up the natural loop

    //Step 1 : Get the dominator table
    int ** dominates;
    dominates = (int **)malloc(sizeof(int *)*G->N);
    for(int i=0;i<G->N;i++)
    {
        dominates[i]=(int *)malloc(sizeof(int)*G->N);
        for(int j=0;j<G->N;j++)
        {
            dominates[i][j]=0;
        }
    }
    bool *visited = (bool*)malloc(G->N*sizeof(bool));
    bool *visited_after = (bool*)malloc(G->N*sizeof(bool));
    for(int j=0;j<G->N;j++)
    {
        visited[j]=FALSE;
    }
    DFS(G,0,visited,-1);
    for(int i=0;i<G->N;i++)
    {
        //For every node
        for(int j=0;j<G->N;j++)
        {
            
            visited_after[j]=FALSE;
        }
        
        DFS(G,0,visited_after,i); //skip the block to see what it dominates
        for(int j=0;j<G->N;j++)
        {
            if(visited[j]==TRUE && visited_after[j]==FALSE)
            {
            // i dominates block j 
                dominates[i][j] = 1;
            }
        }
    }
    //dominates is ready
    //Now look for back edges

    for(int i=0;i<G->N;i++)
    {
        LinkList *edges = G->Nodelist[i].edges;
        while(edges!=NULL)
        {
            // Edge is from i to edges->BasicBlockIndex
            //Check if j dominates i
            int j=edges->BasicBlockIndex;
            if(dominates[j][i]==1)
            {
                //Back edge detected!
                //Natural loop represented as edge from i to j
                //Check what all others belong to this
                int *nodeList;
                nodeList=(int *)malloc(sizeof(int)*G->N);
                for(int idx;idx<G->N;idx++)
                {
                    nodeList[idx]=0;
                }
                nodeList[i]=1;
                nodeList[j]=1;

                //Check what nodes can reach i without going via j
                for(int idx=0;idx<G->N;idx++)
                {
                    bool *reachable = (bool*)malloc(G->N*sizeof(bool));
                    DFS(G,idx,reachable,j);
                    printf("Reachable:%d",reachable[i]);
                    if(reachable[i]==TRUE)
                    {
                        nodeList[idx]=1;
                    }
                    free(reachable);
                }
                printf("\nNatural Loop: %d -> %d consists of blocks ",i,j);
                for(int idx=0;idx<G->N;idx++)
                {
                    if(nodeList[idx]==1)
                    {
                        printf("%d ",idx);
                    }
                }

                free(nodeList);
            }
            edges=edges->next;
        }
        
    }

    free(visited);
    free(visited_after);
    for(int i=0;i<G->N;i++)
    {
        free(dominates[i]);
    }
    free(dominates);
}

void main()
{
    char filename[20]="file1.txt";
    char **TABLE=NULL;
    int TAB_LEN=0;
    printf("Starting program");
    TABLE = StmtTable(filename,&TAB_LEN);
    DisplayTABLE(TABLE,TAB_LEN);
    int *leaders = GetLeaders(TABLE,TAB_LEN);
    printf("\n Leaders----------------------");
    for (int i=0;i<TAB_LEN;i++)
    {
        if(leaders[i]==1)
        {
            printf("\nSTMT %d is a leader",i);
        }
    }
    //After getting leaders,make Graph
    FlowGraph G; 
    InitGraph(&G);
    int lead_index=0;
    int end=1,done=0;
    while(!done)
    {
        //Get next leader/block end
        while(end<TAB_LEN && leaders[end]==0)
        {
            end++;
        }
        if(end==TAB_LEN)
        {
            //Then whole program has been scanned,this is the last block
            done=1;
            
        }
        //Leader-end found!!
        end--;
        AddBasicBlock(&G,lead_index,end);
        end++;
        lead_index=end;
        end++;
    }
    //Now add edges
    done=0;
    int block_index=0,target_block=-1;
    while(block_index<G.N)
    {
        lead_index = G.Nodelist[block_index].leader;
        end = G.Nodelist[block_index].end_stmt;
        //If goto exists,it will be last stmt
        char *searchptr=strstr(TABLE[end],"goto");
        if(searchptr != NULL)
        {
            int jump_to;
            searchptr=searchptr+4;//Advannce the goto part
            sscanf(searchptr,"%d",&jump_to);
            //Search which block does this stmt. no. belongs
            //We know that target stmt will be a leader
            int found=0;
            for(int i=0;i<G.N && found==0;i++)
            {
                if(G.Nodelist[i].leader==jump_to)
                {
                    target_block=i;
                    found=0;
                }
            }
            AddEdge(&G,block_index,target_block);
        }
        //Also,add edge to the block which just follows this
        //This assumes that continuous blocks are connected,no space difference is currenly allowed 
        //i.e. code will/can fall through
        if(end != TAB_LEN-1)
        {
            int found=0;
            for(int i=0;i<G.N && found==0;i++)
            {
                if(G.Nodelist[i].leader==end+1)
                {
                    target_block=i;
                    found=1;
                }
            }
            //Maybe try to avoid fall through edge for unconditional goto
            //We could maybe check if 'if' exists along with goto,only then add edge
            //Else for unconditional goto,do not add fall through edge
            if(strstr(TABLE[G.Nodelist[block_index].end_stmt],"goto") != NULL)
            {
                //Conservatively checking for 'if'
                if(strstr(TABLE[G.Nodelist[block_index].end_stmt],"if") != NULL)
                {
                    //if is present with goto,then add fall through edge
                    AddEdge(&G,block_index,target_block);
                }
            } 
            else
            {
                //Add fall through edge(no goto)
                AddEdge(&G,block_index,target_block);
            }
        }
        
        block_index++;
    }
    printf("\n Basic Block Info----------------");
    PrintGraph(&G);
    printf("\n Formatted Code in BBs");
    DisplayBBInfo(&G,TABLE);
    DeadCodeElimination(&G);
    //PrintGraph(&G);
    printf("\nAfter Deadcode Elimination------------");
    DisplayBBInfo(&G,TABLE);
    Dominates(&G,3);
    DisplayNaturalLoops(&G);
    free(leaders);
    for(int i=0;i<TAB_LEN;i++)
    {
        free(TABLE[i]);
    }
    free(TABLE);
    DeleteGraph(&G);
    printf("\nEnd");
    //Remember to free the table
}

//Assume code will be 3 address,only have conditional/unconditional gotos with labels to corresponding stmts
