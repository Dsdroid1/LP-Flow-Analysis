#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"graph.h"

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
            maxLineLen = (maxLineLen<strlen(DATA))?strlen(DATA):maxLineLen;
        }
        //Metadata of file gathered,now create the table
        fclose(fp);
        TABLE = (char **)malloc(sizeof(char *)*(numLines+1));
        int tabIndex=0;
        fp = fopen(filename,"r");
        while(fgets(DATA,20,fp) != NULL){
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
        printf("\n <BB%d> Start ",i);
        for(int j=G->Nodelist[i].leader;j<=G->Nodelist[i].end_stmt;j++)
        {
            printf("\nStmt %d - %s",j,TABLE[j]);
        }
        printf("\n<BB%d> end",i);
    }
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
        
            AddEdge(&G,block_index,target_block);
        }
        
        block_index++;
    }
    printf("\nBasic Block Info----------------");
    PrintGraph(&G);
    printf("\nFormatted Code in BBs");
    DisplayBBInfo(&G,TABLE);
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
