/*
| Source Code: 	datesFormat.h
| Author: 	Andre Barrientos
| Panther ID: 	5443095  
| Due Date:	11/14/17, at the beginning of class
|
| Description: 	Header file containing all the needed libraries, constants, and prototypes.
*/

#include <stdio.h>	//Standard Input and Output

#define NO_ERROR   	 0	//No error value
#define MONTHS     	12	//Number of months
#define TYPES_OF_YEAR	 2	//The number of types of year (normal and leap)
#define TRUE     	 1	//True value
#define FALSE		 0	//False Value
#define NEEDED_ARGS	 2	//Number of needed arguments from command-line
#define SCAN_ARGS	 3	//Number of items to be scanned from user
#define ALL		 0	//Read all valid inputs
#define LONGEST_VALID	40	//Longest valid date should be 16, but 40 to deal with extra spaces and zeros
#define VALID		 0	//Command-line input must be >= 0

const char *FINISHED = "Finished\n";		//The signal string to terminate

//+ 1 since we will not be using index 0
const int MONTH_DAYS[TYPES_OF_YEAR][MONTHS + 1] = {
			{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
			{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
			};

//Abbreviated month names
const char * MONTH_NAMES[] = {
			"", "JAN", "FEB", "MAR", "APR", "MAY", "JUN"
			  , "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
			};

//Date structure holds month, day, year and if the date is valid
typedef struct
{
	int month;
	int day;
	int year;
	int valid;
} Date;

int validateCML(int argc, char *argv[]);	//Used to validate command-line input
Date validateDate(char *currentLine);		//Used to validate a date
int isLeapYear(int year);			//Used to check if its a leap year
void signalFinish(void);			//Prints string stored in FINISHED
int checkFinished(char *currentLine);		//Checks if the currentLine is the finished signal
Date getDate(char *currentLine);		//Gets the date
void reformatDate(Date date);			//Reformats date from mm/dd/yy to dd mon yy
void appendOriginalDates(void);			//Appends the original dates (dates.dat)
