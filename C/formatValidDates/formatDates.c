/*
| Source Code: 	formatDates.c
| Author: 	Andre Barrientos
| Student ID: 	5443095
| Assignment: 	Program #5
|
| Course:  COP 4338
| Section: U03
| Instructor William Feild
| Due Date: 11/14/17, at the beginning of class
|		I herbey certify that this collective work is my own
|		and none of it is the work of any other person or entity
|
|		____________________________
| Language: 	C
| Compile/Run: 	Make file executes gcc formatDates.c -o formatDates.out
|		make
|	       	./formatDates.out
|		To send output to a file
|		./formatDates.out > outFileName
|		To execute using validDates.out output as input
|		./validDates.out < inFileName nbrValidDates | ./formatDates.out > outFileName
|
| Description: 	Program will take valid dates in the form of mm/dd/yy
|		and change it to day (abbreviated month) year 
|		
| Input:	A valid date in the form of mm/dd/yy
| Output:	The date in format dd mon yy
| Process:	1. Checks if the sentinal value is typed (FINISHED)
|		2. If not it scans for the date and prints the reformatted version
|		3. Steps 1 and 2 and necessary
|		4. Appends a file to the end of the output
|	
|  Required Features Not Included: N/A
|  Known Bugs: N/A
*/

#include "datesFormat.h"		 //Contains libraries, constants, and prototypes to be used

int main(void)
{
	char currentLine[LONGEST_VALID];
	Date date;

	while(checkFinished(currentLine))
	{
		date = getDate(currentLine);
		reformatDate(date);
	}
	
	appendOriginalDates();

	return NO_ERROR;
}

/*
 * Checks if the FINISHED string is inputted, signally to terminate
 * Only returns 0 when the two lines are equal
 * @param the currentLine
 * @return if the currentLine is equal to the FINISHED line
 */
int checkFinished(char *currentLine)
{
	return strcmp(fgets(currentLine, LONGEST_VALID, stdin), FINISHED);
}

/*
 * Gets the date from the currentLine
 * @param the currentLine
 * @return the date from the currentLine
 */
Date getDate(char *currentLine)
{
	Date date;
	unsigned int month = 0;
	unsigned int day = 0;
	int year = 0;
	sscanf(currentLine, "%d/%d/%d\n", &month, &day, &year);
	date.month = month;
	date.day = day;
	date.year = year;
	return date;
}

/*
 * Prints the reformatted date, from mm/dd/yy to dd mon yy
 * @param the date
 * @return N/A
 */
void reformatDate(Date date)
{
	printf("%d %s %d\n", date.day, MONTH_NAMES[date.month], date.year);
}

/*
 * Appends dates.dat file
 * Source:
 * C: How to Program (Dietel & Dietel) Pages 448 - 449
 * @param N/A
 * @return N/A
 */
void appendOriginalDates(void)
{
	//Dates.dat file pointer
	FILE *dfPtr = NULL;
	char currentLine[LONGEST_VALID];

	if((dfPtr = fopen("dates.dat", "r")) == NULL)
	{
		printf("\n%s\n", "File cannot be appended");
	}
	else
	{
		printf("\n%s\n", "Dates.dat File:");
		fgets(currentLine, LONGEST_VALID, dfPtr);

		while(!feof(dfPtr))
		{
			printf("%s", currentLine);
			fgets(currentLine, LONGEST_VALID, dfPtr);
		}

		fclose(dfPtr);
	}	
}
