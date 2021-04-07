#include"sets.h"

int HashFunction(char *set_element,int HashSize)
{
    //Returns the bucket no. to which this element will belong
    int value=0;
    int len=strlen(set_element);
    int i=0;
    //Before this,it can be truncated ,folded or something
    for(i=0;i<len;i++)
    {
        value+=(set_element[i]-'0')*(i+1);
    }
    value=value%HashSize;
    if(value<0)
    {
        value=value%HashSize;
    }
    //printf("Hashed at %d",value);
    return value;
}

void InitializeEmptySet(Set *S)
{
    S->HashSize=DEFAULT_SET_SIZE;
    S->HashTablePtr=(HashElement **)malloc(sizeof(HashElement *)*S->HashSize);
    S->cardinality=0;
    int i=0;
    for(i=0;i<S->HashSize;i++)
    {
        (S->HashTablePtr)[i]=NULL;
    }
}

status_code add(Set *S,char *set_element)
{
    int index=-1;
    status_code sc=SUCCESS;
    index=HashFunction(set_element,S->HashSize);
    HashElement *ptr=NULL;
    ptr=(S->HashTablePtr)[index];
    if(ptr==NULL)
    {
        ptr=(HashElement *)malloc(sizeof(HashElement));
        if(ptr==NULL)
        {
            sc=FAILURE;
        }
        else
        {
            ptr->next=NULL;
            ptr->set_element=(char *)malloc(sizeof(char)*(strlen(set_element)+1));
            strcpy(ptr->set_element,set_element);
            S->cardinality++;
            (S->HashTablePtr)[index]=ptr;
        }
    }
    else
    {
        while(ptr->next!=NULL && sc==SUCCESS)
        {
            if(strcmp(ptr->set_element,set_element)==0)
            {
                sc=FAILURE;//Repetition not allowed
            }
            ptr=ptr->next;
        }
        if(sc==SUCCESS)
        {
            if(strcmp(ptr->set_element,set_element)==0)
            {
                sc=FAILURE;
                printf("\nThe element already exists!");
            }
            else
            {
                ptr->next=(HashElement *)malloc(sizeof(HashElement));
                if(ptr->next==NULL)
                {
                    sc=FAILURE;
                }
                else
                {
                    ptr=ptr->next;
                    ptr->next=NULL;
                    ptr->set_element=(char *)malloc(sizeof(char)*(strlen(set_element)+1));
                    strcpy(ptr->set_element,set_element);
                    S->cardinality++;
                }
            }
        }
        else
        {
            printf("\nThe element already exists!");
        }
    }
    return sc;
}

status_code delete(Set *S,char *set_element)
{
    status_code sc=SUCCESS;
    if(S->cardinality!=0)
    {
        int index=HashFunction(set_element,S->HashSize);
        HashElement *ptr=NULL,*trav=NULL,*prev=NULL;
        ptr=(S->HashTablePtr)[index];
        if(ptr!=NULL)
        {
            int found=0;
            trav=ptr;
            prev=NULL;
            while(trav!=NULL && found==0)
            {
                if(strcmp(trav->set_element,set_element)==0)
                {
                    //element found
                    found=1;
                }
                else
                {
                    prev=trav;
                    trav=trav->next;
                }
            }
            if(found==0)
            {
                sc=FAILURE;
                //Given value does not exist in set
            }
            else
            {
                //'trav' is the element to be deleted
                free(trav->set_element);
                S->cardinality--;
                if(prev==NULL)
                {
                    //It(Deleted value) was the first in this bucket
                    (S->HashTablePtr)[index]=trav->next;//Update to be made in the actual set
                    free(trav);
                }
                else
                {
                    prev->next=trav->next;
                    free(trav);
                }
            }
        }
        else
        {
            sc=FAILURE;
        }
    }
    else
    {
        sc=FAILURE;//No element exists to be deleted
    }
    return sc;
}


void printSet(Set S)
{
    HashElement *ptr;
    int i=0;
    printf("\n***********************************");
    if(S.cardinality!=0)
    {
        printf("\nThe element(s) in this set are:");
        printf("\n{");
        for(i=0;i<S.HashSize;i++)
        {
            ptr=(S.HashTablePtr)[i];
            //printf("\n%p",ptr);
            if(ptr!=NULL)
            {
                while(ptr!=NULL)
                {
                    printf("\n%s",ptr->set_element);
                    ptr=ptr->next;
                }
            }
        }
        printf("\n}");
    }
    else
    {
        printf("\nThe set is empty!");
    }
    printf("\n***********************************\n");
}

//----------Implementing required functions---------------
Bool isElementOf(char *x,Set S)
{
    int index,Found=0;
    Bool retval=TRUE;
    index=HashFunction(x,S.HashSize);
    HashElement *ptr=NULL;
    ptr=(S.HashTablePtr)[index];
    while(ptr!=NULL && Found==0)
    {
        if(strcmp(x,ptr->set_element)==0)
        {
            Found=1;
        }
        else
        {
            ptr=ptr->next;
        }
    }
    if(Found==0)
    {
        retval=FALSE;
    }
    return retval;
}

Bool isEmpty(Set S)
{   
    Bool retval=FALSE;
    if(S.cardinality==0)
    {
        retval=TRUE;
    }
}

int Size(Set S)
{
    return S.cardinality;
}

char** Enumerate(Set S,int* size)//Pass size via main\caller function...
{
    //It can be done so that a linked list is returned,changes need to be made though
    char **list=NULL;
    *size=Size(S);
    list=(char **)malloc(sizeof(char *)*(*size));
    int i=0,j=0;
    HashElement *ptr;
    for(i=0;i<S.HashSize;i++)
    {
        ptr=(S.HashTablePtr)[i];
        while(ptr!=NULL)
        {
            if(ptr!=NULL)
            {
                list[j]=(char *)malloc(sizeof(char)*(strlen(ptr->set_element)+1));
                strcpy(list[j],ptr->set_element);
                j++;
                ptr=ptr->next;
            }
            else
            {
                ptr=ptr->next;
            }
        }
    }
    return list;
}

status_code Build(char list[][MAX_SCAN],int size,Set *S)
{
    InitializeEmptySet(S);
    int i=0,flag=0;
    status_code sc;
    for(i=0;i<size && flag==0;i++)
    {
        sc=add(S,list[i]);
        if(sc==FAILURE)
        {
            flag=1;
            printf("\nSome erro occured!!");
        }
    }
    return sc;
}


//=---------For testing purpose---------------------------
void printStringArray(char **list,int size)
{
    int i=0;
    for(i=0;i<size;i++)
    {
        printf("\n%s",list[i]);
    }
}

void flushStringArray(char **list,int *size)
{
    int i=0;
    if(list!=NULL)
    {
        for(i=0;i<(*size);i++)
        {
            free(list[i]);
            list[i]=NULL;
        }
        free(list);
    }
    *size=0;
}



