/*
| Source Code: 	parentChildCom.c
| Author: 	Andre Barrientos
| Student ID: 	5443095
| Assignment: 	Program #6
|
| Course:  COP 4338
| Section: U03
| Instructor William Feild
| Due Date: 12/7/17, at the beginning of class
|		I herbey certify that this collective work is my own
|		and none of it is the work of any other person or entity
|
|		____________________________
| Language: 	C
| Compile/Run: 	Make file executes gcc parentChildCom.c -o parentChildCom.out
|		make
|	       	./parentChildCom.out singleDigitNbrs
|		(Warning (cast from pointer to integer of different size) does come up on some systems)
|
| Description: 	Program will fork a number of children (number based off of nbr of 
|		cml inputs). Each child will be assigned a unique identifier and
|		a number from the input. Ex. Child 1 will be assigned the first value
|		from the cml. The children will then multiply their unqiue identifier with
|		the value assigned from the input and store that value in shared memory.
|		Print statements will be used to track the shared memory after each step and
|		which child is doing what. The parent process will print the inital and final
|		shared memory array.
|
|		******** Shared Memory, Fork, and Wait Source ******** 
|		Dr. C.-K. Shene at www.csl.mtu.edu/cs.ck/www/NOTES/process/process.html
|		
| Input:   One thru seven unique integer values between the values [0-9]. (command-line input)
| Output:  Trace statements keeping track of when and what each child process is doing. As well
|	   as each modification of the shared memory array.
| Process:  1. Command line arguements are validated
|	    2. Shared memory is requested and initialized
|	    3. Display initial shared memory array
|	    4. Fork off the number of children
|	    5. Child processes will manipulated their respective shared memory index
|	    6. Processes will display what they are doing at any given time
|	    7. Finial shared memory array is printed.
|  Required Features Not Included: N/A
|  Known Bugs: N/A
*/

#include "parentChildCom.h"		 //Contains libraries, constants, and prototypes to be used

int  main(int argc, char *argv[])
{
	if(validate(argc, argv))
	{
		//- 1 because argc counts the program name as a value
		int nbrChildren = argc - 1;
		int fid = 0;
		int childCounter = 0;
		int shmID = 0;
		int *shmPtr = NULL;	
		
		shmID = shmget(IPC_PRIVATE, nbrChildren * sizeof(int), IPC_CREAT | 0666);
		if (shmID < 0)
		{
			printf("%s", "shmget error");
			return ERROR;
		}
		printf("%s%d%s\n", "Parent: Received a shared memory of ", nbrChildren, " integer(s)...");

		shmPtr = (int *) shmat(shmID, NULL, 0);
		if((int) shmPtr == -1)
		{
			printf("%s", "shmat error");
			return ERROR;
		}
		printf("%s\n", "Parent: Attached the shared memory");

		setSharedMemory(nbrChildren, argv, shmPtr);

		printf("%s", "Parent: Initial Shared Memory: ");
		printSharedMemory(nbrChildren, shmPtr);
		
		do
		{
			fid = fork();
			childCounter++;
			if(fid > 0)
			{
				printf("%s%d%s%d\n", "Parent: Child ", childCounter, " has pid, ", fid);
			}
		}
		while(fid > 0 && childCounter != nbrChildren);	

		if (fid < 0)
		{
			printf("%s", "fork error");
			return ERROR;
		}
		else if (fid == 0)
		{
			childProcess(nbrChildren, childCounter, shmPtr);
		}
		else
		{
			printf("%s\n", "Parent: Waiting...");
			int childCounter = 0;
			int pid = 0;
			for (; childCounter < nbrChildren; childCounter++)
			{
				pid = wait();
				printf("%s%d%s\n", "Parent: Detected process ", pid, " has ended.");
			}
			printf("%s", "Parent: Final Shared Memory: ");
			printSharedMemory(nbrChildren, shmPtr);
			shmdt((void *) shmPtr);
			printf("%s\n", "Parent: Detached the shared memory");
			shmctl(shmID, IPC_RMID, NULL);
			printf("%s\n", "Parent: Removed the shared memory");
			printf("%s\n", "Parent: Done executing");
		}
	}

    return NO_ERROR;
}

/*
 * Validates the command line input. All values must be digits
 * (values must be whole number integers in range [0-9]). All values must
 * also be unique (no duplicate numbers). There may be no more than seven
 * input values and no less than one value.
 * Source for strtol usage:
 * C How to Program by Paul and Harvey Deitel (Section 8.4 pg. 344)
 * @param argc is the number of comman-line inputs
 * @param *argv[] contains the actual values of the command-line input
 * @return if the input values are valid
 */
int validate(int argc, char *argv[])
{
    int isValid = FALSE;
    int isUnique = TRUE;
    char *remainingStr = NULL;
    int inputNbr = 0;

    if((argc <  MIN_ARGC) || (argc > MAX_ARGC))
    {
	printf("Proper usage: %s digitNbrs (digitNbrs range [%d - %d])\n", argv[0], MIN_VALUE, MAX_VALUE);
    }
    else
    {
	//Position 0 holds the name of the file which we do not need
	int childID = 1;
	//Array used to check that numbers are unique
	int uniqueNbrs[DIGITS] = {0};
	do
	{
		inputNbr = (int) strtol(argv[childID], &remainingStr, BASE);

		if((inputNbr >= MIN_VALUE) && (inputNbr <= MAX_VALUE) && (argv[childID] != remainingStr) && (remainingStr[0] == '\0'))
		{
			if(uniqueNbrs[inputNbr] != 0)
			{
				isUnique = FALSE;
				isValid = FALSE;
				printf("%s\n", "All values must be unique.");
			}
			else
			{
				uniqueNbrs[inputNbr]++;
				childID++;
				isValid = TRUE;
			}
		}
		else
		{
			printf("%s%d%s%d%s\n", "Values must be integers in the range [", MIN_VALUE, " - ", MAX_VALUE, "]");
			childID = argc;
			isValid = FALSE;
		}
	}
	while (isUnique == TRUE && childID < argc);
    }
    return isValid;
}

/*
 * The child process displays the initial shared memory. It then multiplies
 * its child ID with its respective shared memory index. For example Child 1
 * will multiple its child ID (1) with the first shared memory index (index 0).
 * The modified shared memory is then displayed.
 * @param nbrChildren the number of child processes that the parent spawned
 * @param childID is the child unique identifier
 * @param sharedMemory is the pointer to the shared memory
 * @return N/A 
 */
void childProcess(int nbrChildren, int childID, int sharedMemory[])
{
	printf("%s%d%s\n", "Child ", childID, ": Starting...");

	printf("%s%d%s", "Child ", childID, ": Initial Shared Memory: ");
	printSharedMemory(nbrChildren, sharedMemory);

	//- 1 because index 0 being used (index 0  pertains to child 1, etc.)
	sharedMemory[childID - 1] = childID * sharedMemory[childID - 1];

	printf("%s%d%s", "Child ", childID, ": Final Shared Memory: ");
	printSharedMemory(nbrChildren, sharedMemory);

	printf("%s%d%s\n", "Child ", childID, ": Done executing");
}

/*
 * Prints the current state of the shared memory.
 * @param nbrchildren the number of child processes that the parent spawned
 * @param sharedMemory is the pointer to the shared memory
 * @return N/A
 */
void printSharedMemory(int nbrChildren, int sharedMemory[])
{
	int childIndex = 0;
	for (; childIndex < nbrChildren; childIndex++)
	{
		printf("%d ", sharedMemory[childIndex]);
	}
	printf("\n");
}

/*
 * Sets the shared memory array to the validated input values.
 * @param nbrChildren the number of child processes that the parent spawned
 * @param *argv[] contains the actual values of the command-line input
 * @param sharedMemory is the pointer to the shared memory
 * @return N/A
 */
void setSharedMemory (int nbrChildren, char *argv[], int sharedMemory[])
{
	int childIndex = 0;
	int childValue = 0;
	for (; childIndex < nbrChildren; childIndex++)
	{
		//+ 1 because argv[0] holds program name
		childValue = atoi(argv[childIndex + 1]);
		sharedMemory[childIndex] = childValue;
	}
}
