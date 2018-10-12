/*
| Source Code: 	parentChildCom.h
| Author: 	Andre Barrientos
| Panther ID: 	5443095  
| Due Date:	12/7/17, at the beginning of class
|
| Description: 	Header file containing all the needed libraries, constants, and prototypes.
*/

#include <stdio.h>	//Standard Input and Output
#include <sys/types.h>	//Used to manage shared memory
#include <sys/ipc.h>	//Used to manage shared memory
#include <sys/shm.h>	//Used to manage shared memory

#define NO_ERROR   	 0	//No error value
#define ERROR		 1	//Error value
#define TRUE     	 1	//True value
#define FALSE		 0	//False Value
#define MIN_VALUE	 0	//Lowest valid value
#define MAX_VALUE	 9	//Highest valid Value
#define MIN_ARGC	 2	//Minimum value of argc from command line
#define MAX_ARGC	 8	//Maximum value of argc from command line
#define DIGITS		10	//Number of digits [0 - 9]
#define BASE		10	//Numbers will be decimal values (base 10)

void childProcess(int nbrChildren, int childNbr, int sharedMemory[]);	//The child process
int validate(int argc,char *argv[]);									//Used to validate command-line input
void printSharedMemory(int nbrChildren, int sharedMemory[]);
void setSharedMemory(int nbrChildren, char *argv[], int sharedMemory[]);
