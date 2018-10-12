/*
| Source Code: eCalcSQRT.c
| Author: Andre Barrientos
| Student ID: 5443095
| Assignment: Program #2
|
| Course: COP 4338
| Section: U03
| Instructor William Feild
| Due Date: 10/03/17, at the beginning of class
|	I herbey certify that this collective work is my own
|	and none of it is the work of any other person or entity
|
|	____________________________
| Language: C
| Compile/Run: gcc eCalcSQRT.c -o eCalcSQRT.out -lm
|	       ./eCalcSQRT.out
| Description: Program will compute the value of e and the square root of
|		any given number, greater than or equal to zero.
|		
| Input: A real number greater than or equal to zero.
| Output: Computed square root and value of e (sixteen decimal-place accuracy) 
|	   along with the expected value of square root and e. 
|	   The number of iterations taken to reach each 
|	   value as well as the time taken in ns.
|  Process: 1. User is prompted
|	    2. Value for e is computed and printed
|	    3. Value for the square root is computed and printed
|	    4. Program is finished executing 
|  Required Features Not Included: N/A
|  Known Bugs: Sixteenth decimal for computed value of e
|  		does not match expected value.
*/

#include <stdio.h>   //Standard Input and Output
#include <math.h>    //To retrieve expected values
#include <time.h>    //To calculate how long each process took

#define NO_ERROR 0           //No error value
#define TRUE     1	     //True value
#define ACCURACY pow(10,-16) //Used to get 16 decimal point accuracy
#define NANO pow(10,-9)      //Used to convert to nano

double validate(void);  	 //Used to validate input
double computeE(void);		 //Used to compute e
double squareRoot(double value); //Used to compute the square root

int  main(void)
{
    double input = validate();
    
    clock_t startTime = clock();
    double e = computeE();
    clock_t endTime = clock();
    double totalTime = ((double)(endTime - startTime)/CLOCKS_PER_SEC) / NANO;

    printf("%20s%20.16f\n", "Expected value of e: ", M_E);
    printf("%20s%5.16f\n", "Calculated value of e: ", e);
    printf("%20s%5.16f%s\n\n", "Time taken to compute: ", totalTime, " ns");

    startTime = clock();
    double sqRoot = squareRoot(input);
    endTime = clock();
    totalTime = ((double)(endTime - startTime)/CLOCKS_PER_SEC) / NANO;

    printf("%20s%.4f: %20.16f\n", "Expected square root of ", input, sqrt(input));
    printf("%20s%.4f: %5.16f\n", "Calculated square root of ", input, sqRoot);
    printf("%20s%5.16f%s\n", "Time taken to compute: ", totalTime, " ns");
    
    return NO_ERROR;
}

/**
 * Validates input to be a real number greater than or equal to zero.
 * @param N/A
 * @return the input value 
 */
double validate(void)
{
    double value = 0;
    int valid = 0;
    int notValid = 0;
    char clear;
    do
    {
	printf("%s\n", "Enter a value (>= 0) to compute the square root: ");
	int valid = scanf("%lf", &value);
	notValid = valid != TRUE;
        if (notValid)
	{
	   while (clear = getchar() != '\n')
	   {
	       putchar(clear);
	   }
	   printf("%s\n", "Not a floating point value.");
	}//end if
	else if (value < 0)
	{
	   printf("%s\n", "Value must be greater than or equal to zero.");
	   notValid = TRUE;
	}//end else if
    }//end do
    while(notValid);//end while
    return value;
}

/**
 * The value of e is derived from the sum of an infinite series
 * of 1/n!, for n starting at zero and ending at infinity.
 * Source: https://en.wikipedia.org/wiki/E_(mathematical_constant)
 * @param N/A
 * @return the value of e at fifteen decimal pointa accuracy
 */
double computeE(void)
{
    int counter = 0;
    double e = 0;
    //-1 used to ensure prevE != e to enter while loop
    double prevE = -1;
    //0! is first factorial and has a value of 1
    long int factorial = 1;
    while(fabs(e - prevE) > ACCURACY)
    {
	counter++;
	prevE = e;
	e = e + (1.0/factorial);
	factorial = factorial * (counter);
    }//end while
    printf("%s%d\n", "Iterations for e: ", counter);
    return e;
}

/**
 * The square root is derived from the formula:
 * X_1 = (1/2) * (X_0 + (value/X_0)). X_1 is then
 * plugged into the same equation replacing X_0, until
 * the desired accuracy is reached (Babylonian Method).
 * Source: https://en.wikipedia.org/wiki/Methods_of_computing_square_roots
 * @param the value to be taken the square root of
 * @return the square root of the input value
 */
double squareRoot(double value)
{
    int counter = 0;
    //-1 used to ensure prevSQRT != sqRoot to enter while loop
    double prevSQRT = -1;
    //Value is divided by two to get a decent starting value for the equation
    double sqRoot = value / 2.0;
    if (value != 0)
    {
    	while(fabs(sqRoot - prevSQRT) > ACCURACY)
    	{
	    counter++;
	    prevSQRT = sqRoot;
	    //(1.0)/(2.0) is gotten from the formula
	    sqRoot = (1.0/2.0)*(sqRoot + (value/sqRoot));
    	}//end while
    }//end if
    printf("%s%.4f: %d\n", "Iterations for the square root of ", value, counter);
    return sqRoot;
}

