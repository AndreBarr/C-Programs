/*
| Source Code: 	validDates.c
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
| Compile/Run: 	Make file executes gcc validDates.c -o validDates.out
|		make
|	       	./validDates.out nbrValidDates
|		(0 will get them all)
|		To execute using a file as input
|		./validDates.out < inFileName nbrValidDates
|		To send output to formatDates.out (makefile compiles both programs)
|		./validDates.out < inFileName nbrValidDates | ./formatDates.out > outFileName
|
| Description: 	Program will determine whether a date is valid and print
|		only the valid dates.
|
| Input:	A list of dates
| Output:	The valid dates in form of mm/dd/yy
| Process:	1. Checks how many valid dates to be gotten
|		2. Checks if EOF is reached or desired number of dates is reached
|		3. If Step 2 is false then validates the current line
|		4. If Step 3 is true then prints the valid date
|		5. Repeat Steps 2 - 4 as necessary
|		6. Signal done by printing FINISHED
|
|  Required Features Not Included: N/A
|  Known Bugs: N/A
*/

#include "datesFormat.h"		 //Contains libraries, constants, and prototypes to be used

int main(int argc, char *argv[])
{
	if(validateCML(argc, argv))
	{
		//Index 0 holds the name of the file which we do not need
		const unsigned int NBR_VALID_DATES = atoi(argv[1]);
		Date date;
		int validDates = 0;
		char currentLine[LONGEST_VALID];
		if(NBR_VALID_DATES == ALL)
		{
			//validDates will never equal NBR_VALID_DATES
			validDates = 1;
		}

		while((fgets(currentLine, LONGEST_VALID, stdin) != NULL) && (validDates != NBR_VALID_DATES))
		{	
			date = validateDate(currentLine);
			
			if(date.valid == TRUE)
			{
				printf("%d/%d/%d\n", date.month, date.day, date.year);
				validDates++;
			}
		}
		
		signalFinish();
	}
	
	return NO_ERROR;
}

/*
 * Validates the command line input to be >= 0
 * @param argc is the number of command-line arguments
 * @param *argv[] is the actual values of the command-line input
 * @return if the input value is valid
 */
int validateCML(int argc, char *argv[])
{
	int isValid = 0;
	int nbrValidDates = 0;

	if(argc != NEEDED_ARGS)
	{
		printf("%s\n", "Proper usage: ./validDates.out nbrValidDates");
	}
	else
	{
		//Index 0 holds the name of the file which we do not need
		nbrValidDates = atoi(argv[1]);
		if(nbrValidDates >= VALID)
		{
			isValid = TRUE;
		}
		else
		{
			signalFinish();
			printf("%s%d\n", "Value must be >= ", VALID);
		}
	}

	return isValid;
}

/*
 * Checks a string for a date in the form of mm/dd/yyyy
 * @param the pointer to the string
 * @return the valid date in the form of a struct date
 */
Date validateDate(char *currentLine)
{
	Date currentDate;
	unsigned int month = 0;
	unsigned int day = 0;
	long int testYear = 0;
	int year = 0;
	int leapYear = FALSE;
	//Garbage is used to check if any other non-whitespace characters are entered after the year
	char garbage = '\0';
	int passedArgs = sscanf(currentLine, "%d /%d /%ld %c\n", &month, &day, &testYear, &garbage);
	year = testYear;
	leapYear = checkLeapYear(year);

	if(passedArgs == SCAN_ARGS)
	{
		//Year != testYear makes sure there isnt overflow in int year
		//Month and day cannot be 0
		if(year != testYear || month > MONTHS || month == 0 || day == 0 || day > MONTH_DAYS[leapYear][month])
		{
			currentDate.valid = FALSE;
		}
		else
		{
			currentDate.valid = TRUE;
			currentDate.month = month;
			currentDate.day = day;
			currentDate.year = year;
		}	
	}
	else
	{
		currentDate.valid = FALSE;
	}
	return currentDate;
}	

/*
 * Checks if the year is a leap year.
 * Leap Year Source:
 * C Programming Language (Kernighan and Ritchie)
 */
int checkLeapYear(int year)
{
	int isLeapYear = FALSE;

	//If a year divides evenly into 4 and is not a century year it is a leap year
	//If the year is a century year and divides evenly by 400 then it is a leap year
	isLeapYear = ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);

	return isLeapYear;
}

/*
 * Signals finished by printing string stored in FINISHED
 * @param N/A
 * @return N/A
 */
void signalFinish(void)
{
	printf("%s\n", FINISHED);
}
