/*
| Source Code: tempConv.c
| Author: Andre Barrientos
| Student ID: 5443095
| Assignment: Program #1
|
| Course: COP 4338
| Section: U03
| Instructor William Feild
| Due Date: 9/22/17, at the beginning of class
|	I herbey certify that this collective work is my own
|	and none of it is the work of any other person or entity/
|	____________________________
| Language: C
| Compile/Run: gcc tempConv.c -o tempConv.out
|	       ./tempConv.out
| Description: Display a temperature conversion chart starting with
|		a fixed low value and incrementing the value up by
|		the value inputted by the user.
| Input: An integer number between 1 - 9
| Output: A temperature conversion chart with temperatures increasing
|	by the input value
|  Process: Formula for conversion to celsius from fahrenheit:
|		(temperature - 32) * 5.0/9
|	    Formula for conversion to fahrenheit from celsius:
|		(temperature * 1.8) + 32
|  Required Features Not Included: Data Type Validation
|  Known Bugs: N/A
*/

#include <stdio.h>   //Standard Input and Output
#define NO_ERROR 0   //No error value
#define LOWTEMP -20  //Lowest temperature included in table
#define HIGHTEMP 280 //Sentinel value for table
#define BLANKLINE 8  //Number of enteries before blank line

int validate(void); //Used to validate input
float calculateCtoF(int temperature); //Conversion from celsius to fahrenheit
float calculateFtoC(int temperature); //Conversion from fahrenheit to celsius

int main(void)
{
    int stepValue = validate();
    printf("%s%d\n", "Step Value: ", stepValue);
    printf("%10s%10s%15s%13s\n", "Fahrenheit", "Celsius", "Celsius", "Fahrenheit");
    printf("%s\n", "-------------------------------------------------");
    int temp;
    int counter = 0;
    for(temp = LOWTEMP; temp <= HIGHTEMP; temp += stepValue)
    {
	float tempC = calculateFtoC(temp);
	float tempF = calculateCtoF(temp);
	printf("%10d%10.1f%15d%13.1f\n", temp, tempC, temp, tempF);
	counter++;
	if ((counter % BLANKLINE) == 0)
	{
	    printf("\n");
	} 
    }
    return NO_ERROR;
}

/**
 * Validates input to be within the required range (1 - 9).
 * @return the input value 
 */
int validate(void)
{
    int stepValue = 0;
    do
    {
        printf("%s\n","Please enter a step value:");
        scanf("%d", &stepValue);
    }
    while(stepValue > 9 || stepValue < 1);
    return stepValue;
}

/**
 * Converts from celsius to fahrenheit
 * Formula used: (temperature * 1.8) + 32
 * @param the temperature to be converted
 * @return the converted value
 */
float calculateCtoF(int temperature)
{
    float tempF = temperature * 1.8 + 32;
    return tempF;
}

/**
 * Converts from fahrenheit to celsius
 * Formula used: (temperature - 32) * (5.0/9) 
 * @param the temperature to be converted
 * @return the converted value
 */
float calculateFtoC(int temperature)
{
    float tempC = (temperature - 32) * (5.0/9);
    return tempC;
}

