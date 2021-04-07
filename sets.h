#ifndef SETS_H
#define SETS_H
//Headers Required--------------------------------------------
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
//Global Constants--------------------------------------------
#define DEFAULT_SET_SIZE 11 
#define MAX_SCAN 20
//Structures--------------------------------------------------
typedef struct HashElement
{
    char *set_element;
    struct HashElement *next;//For the same bucket
}HashElement;

typedef struct HashTable
{
    //require an array of HashElement pointers?
    int HashSize;
    int cardinality;
    HashElement **HashTablePtr;//As we require array of variable size at runtime
}Set;
//Misc. types------------------------------------------------
typedef enum {FAILURE,SUCCESS} status_code;
typedef enum {FALSE,TRUE} Bool;

//Functions Defined------------------------------------------
int HashFunction(char *set_element,int HashSize);
void InitializeEmptySet(Set *S);//Create()
status_code add(Set *S,char *set_element);
status_code delete(Set *S,char *set_element);//remove()
Bool isElementOf(char *x,Set S);
Bool isEmpty(Set S);
int Size(Set S);
char** Enumerate(Set S,int* size);//Pass size via main\caller function...
status_code Build(char list[][MAX_SCAN],int size,Set *S);//TO BE TESTED..................!!!!!!!!!!!!!!!!!!!!!!!!

//Misc. Functions--------------------------------------------
void printSet(Set S);
void printStringArray(char **list,int size);
void flushStringArray(char **list,int *size);//Use this before Enumerate for dynamically assigned list

#endif