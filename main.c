/* 
This program is written by: Ricardo Carvalheira */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"

#define WRDSTRLEN 21

FILE *inFile, *outFile;

typedef struct wordStored
{
    char word[WRDSTRLEN];
    int frequency;
} wordStored;

typedef struct tree
{
    wordStored *info;
    int depth;
    struct tree *left;
    struct tree *right;
} BST;

BST *initOps(BST *treeRoot, int numLines);
BST *findTreeNode(BST *treeRoot, char searchVal[]);
wordStored *create_node(char wordVal[]);
BST *insertTreeNode(BST* treeRoot, BST* newTreeNode, int depth);
BST* createTreeNode(wordStored* wordtoBeStored);
void answerQuery(BST* treeRoot, char word[]);
int getTreeSize(BST* treeRoot);
void createArr(BST** arr, BST* treeRoot, int i);
void BSTtoArray(BST* treeNode, BST* arr, int* i);
void quickSortArr(BST* arr, int low, int high);
void swap(BST *a, BST *b);
int compareTo(BST *treeA, BST *treeB);
void printArr(BST* arr, int size);
void freeTree(BST *treeRoot);
void freeArr(BST* arr, int size);

int main()
{
    atexit(report_mem_leak);	//mem leak stuff
    inFile = fopen("in.txt", "r"); //opens the file name

    if (inFile == NULL)
    {
      printf("Input file is missing!\n");
      fprintf(outFile,"Input file is missing!\n");
      fclose(inFile); //Closes the files
      exit(-1);
    }
    outFile = fopen("out.txt", "w"); //opens the output file

    int numLines;//gets the number of lines of the inFile
    BST *ptrToBST = NULL;//initializes root to NULL

    fscanf(inFile, "%d", &numLines);//gets the number of lines
    if(numLines>200000 || numLines<0)
    {
      printf("Input file Maximum of lines is out of bounds!\n");
      fprintf(outFile,"Input file Maximum of lines is out of bounds!\n");
      fclose(inFile); //Closes the files
      exit(-1);

    }
    ptrToBST = initOps(ptrToBST, numLines); //gets the pointer to the BST
    int size = getTreeSize(ptrToBST); //get the array size
    BST* arr = (BST*) calloc(size,sizeof(BST)); //Declare PTR array
    int i=0;
    BSTtoArray(ptrToBST, arr, &i);//gets the array from BST
    quickSortArr(arr,0,size-1); //sorts de array

    printArr(arr, size); //prints the array

    freeArr(arr,size); //frees the array and its structures
    freeTree(ptrToBST); //frees the binary tree

    fclose(inFile);  //Closes the files
    fclose(outFile); //Closes the files

    return 0;
}

//compares tree A to tree B according to their frequency

int compareTo(BST *treeA, BST *treeB) 
{
	int result = treeA->info->frequency - treeB->info->frequency;
  //if A is more frequent than B, this result is a positive number
  //else it will return a negative number
  //if they are the same size, we check which one of them comes first alphabetically
	if(result == 0){
	  return strcmp(treeB->info->word, treeA->info->word);
	}
	return result; 
}
int partition(BST *arr, int low, int high){

  int temp;
  int i, lowpos;
  i=low+rand()%(high-low+1);
  swap(&arr[low], &arr[i]); 
  lowpos=low;
  low++;

  while(low<=high)
  {
    while(low<=high && compareTo(&arr[low], &arr[lowpos])>0)  low++;
    while(low<=high&&compareTo(&arr[high],&arr[lowpos])<0) high--;

    if(low<high)
      swap(&arr[low],&arr[high]);
    
    
  }
  swap(&arr[lowpos], &arr[high]); 
    return high;
}

void quickSortArr(BST *arr, int low, int high)
{	
	if (low < high)
  {
		int k = partition(arr, low, high);
		quickSortArr(arr, low, k - 1);	//Before k
		quickSortArr(arr, k + 1, high);	//After k
	}

}
//this function transforms my BST to an array
void BSTtoArray(BST* treeNode, BST* arr, int *i)
{
    if(treeNode != NULL)
    {    
        arr[*i] = *treeNode;//pass the treeNode to the array at the position i
        ++*i;//increases the i to avoid repetition
        BSTtoArray(treeNode->left, arr, i);//goes to the left
        BSTtoArray(treeNode->right, arr, i);//goes to the right
    }
     
}

BST* createTreeNode(wordStored* wordtoBeStored)
{
    BST* temp =(BST*) malloc(sizeof(BST));  //Creates temp node pointer to store tree's information and Allocates memory for it
	temp->info = wordtoBeStored;  //Copy the wordStoers structure to this node.
	temp->depth =0; //Initlializes its depth
    temp->left = NULL; //Initlializes its left pointer
	temp->right = NULL; //Initlializes its right pointer

    return temp; //returns the tree node
}

//This function is responsible for correctly creating the tree note and initializing it.

BST *insertTreeNode(BST *treeRoot, BST* newTreeNode, int depth)
{
    newTreeNode->depth=depth;
    if (treeRoot == NULL)
    {
        return newTreeNode;                       //returns the tree node
    }
    else
    {
        if (strcmp(newTreeNode->info->word, treeRoot->info->word) > 0)
        {
            if(treeRoot->right!=NULL)
                treeRoot->right = insertTreeNode(treeRoot->right, newTreeNode, depth + 1); //traverse to the right to find the place to place the new node
            else
            {
              newTreeNode->depth+=1;//when the place is found, increase the depth
              treeRoot->right=newTreeNode;
            }
                
        }
        else if(strcmp(newTreeNode->info->word, treeRoot->info->word) < 0)
        {
            if(treeRoot->left!=NULL)
                treeRoot->left= insertTreeNode(treeRoot->left, newTreeNode, depth + 1);//traverse to the left to find the place to place the new node
            else
            {
              newTreeNode->depth+=1;//when the place is found, increase the depth
              treeRoot->left=newTreeNode;
            }     
        }
        return treeRoot;
    }    
}

//initializes all operations for the binary tree

BST *initOps(BST *treeRoot, int numLines)
{
    int i, operation;
    char word[WRDSTRLEN];

    for (i = 0; i < numLines; i++)
    {
        fscanf(inFile, "%d %s", &operation, word); //scans from file the operation and word
        int len = strlen(word);
        if(len>WRDSTRLEN)
        {
          printf("Invalid string size!\n");
          fprintf(outFile, "Invalid string size!\n");
          fclose(inFile);  //Closes the files
          fclose(outFile); //Closes the files
          exit(-1);
        }
        if (operation == 1)//write operation
        {
            BST *newTreeNode = findTreeNode(treeRoot, word);//search for trees with that word
            if (newTreeNode == NULL)
            {
                wordStored *tempWordStorageNode = create_node(word);         //Creates temporary node
                newTreeNode=createTreeNode(tempWordStorageNode);
                treeRoot = insertTreeNode(treeRoot, newTreeNode, 0); //inserts it to the tree
            }
            else
                newTreeNode->info->frequency += 1; //if the node found is not equals to null, increase its frequency
        }

        else if (operation == 2)//read operation
        {
           answerQuery(treeRoot, word); //calls function to answer the query of inFile
        }

        else //anything other than read or write is not valid
        {
            printf("Invalid Operation: %d.\nValid operations: 1 or 2", operation);
            fprintf(outFile,"Invalid Operation: %d.\nValid operations: 1 or 2", operation);
            fclose(inFile);  //Closes the files
            fclose(outFile); //Closes the files
            exit(-1);
        }
 
    }
    
    return treeRoot;
}

//Function returns null if the treeNode isn't found. If it finds it, it will return the treeRoot.

BST *findTreeNode(BST *treeRoot, char searchVal[])
{
    if (treeRoot != NULL)
    {
        if (strcmp(searchVal, treeRoot->info->word) == 0) //if searchVal is  in that node, return the treeRoot itself
            return treeRoot;
        if (strcmp(searchVal, treeRoot->info->word) < 0) //if searchVal is to the left of the root
            return findTreeNode(treeRoot->left, searchVal);
        if (strcmp(searchVal, treeRoot->info->word) > 0) //if searchVal is to the right of the root
            return findTreeNode(treeRoot->right, searchVal);
    }
    return NULL; //word not found
}

//This function is responsible for correctly creating the word note and initializing it.

wordStored *create_node(char wordVal[])
{
    wordStored *tempNode;                                //Creates temp node pointer to store words information. .
    tempNode = (wordStored *)malloc(sizeof(wordStored)); // Allocates memory for it
    strcpy(tempNode->word, wordVal);                     //Copy the new word to this node.
    tempNode->frequency = 1;                             //Initializes its frequency
    return tempNode;                                     //Returns new node
}

//this function responds to the querys
void answerQuery(BST* treeRoot, char word[])
{
  int notFoundInteger =-1; //Integer if we dont find the tree
  BST *newTreeNode = findTreeNode(treeRoot, word);
  if(newTreeNode==NULL)
  {
    printf("%d %d\n", notFoundInteger, notFoundInteger);
    fprintf(outFile, "%d %d\n", notFoundInteger, notFoundInteger);
  }
  else
  {//if its found, print its queried values
    printf("%d %d\n", newTreeNode->info->frequency, newTreeNode->depth);
    fprintf(outFile,"%d %d\n", newTreeNode->info->frequency, newTreeNode->depth);
  }
}
//this function gets the tree size
int getTreeSize(BST* treeRoot) //gets the tree size
{
    if (treeRoot==NULL)
      return 0;
      return 1+getTreeSize(treeRoot->left)+getTreeSize(treeRoot->right);
}
//this function frees the tree's memory
void freeTree(BST *treeRoot)
{
    if (treeRoot!= NULL)
    {   
        freeTree(treeRoot->left);       // Go Left.
        freeTree(treeRoot->right);      // Go Right.
        free(treeRoot);
    }
}

//This function prints the array
void printArr(BST* arr, int size) 
{ 
    int i;
    for( i=0;i<size;i++)
    {
      printf("%s %d\n", arr[i].info->word, arr[i].info->frequency);
      fprintf(outFile,"%s %d\n", arr[i].info->word, arr[i].info->frequency);
    }
}

//frees the array's memory

void freeArr(BST* arr, int size)
{
    int i;
    for( i=0;i<size;i++)
    {
      free(arr[i].info); //frees the structure containing word and frequency
    }  
    free(arr); //frees the array pointer to struct
}

//swaps trees

void swap(BST *wordA, BST *wordB) 
{
	BST temp = *wordA;
	*wordA = *wordB;
	*wordB = temp;
}