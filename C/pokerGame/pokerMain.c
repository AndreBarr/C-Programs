/*
| Source Code: 	pokerMain.c
| Author: 	Andre Barrientos
| Student ID: 	5443095
| Assignment: 	Program #4
|
| Course:  COP 4338
| Section: U03
| Instructor William Feild
| Due Date: 10/31/17, at the beginning of class
|		I herbey certify that this collective work is my own
|		and none of it is the work of any other person or entity
|
|		____________________________
| Language: 	C
| Compile/Run: 	Make file executes gcc poker.c pokerMain.c -o poker.out
		make
|	       	./poker.out cardsPerHand nbrOfHands
|
| Description: 	Program will creat a deck of cards, shuffle the deck, and
|		deal out hands  and sort hands determined by user input.
|		The deck and hands will be printed after each corresponding step.
|		
| Input:   Two numbers both in the range [1 - 7] (command-line input)
| Output:  The deck of cards created, the shuffled deck, and the hands dealt out.
| Process:  1. Command line arguements are validated
|	    2. Deck is created and printed
|	    3. Deck is shuffled and printed
|	    4. Hands are dealt out
|	    5. Hands are printed then sorted then printed again.
|	    6. Assign poker ranks to the cards and print
|	    7. Print the winners.
|  Required Features Not Included: N/A
|  Known Bugs: N/A
*/

#include "pokerGame.h"		 //Contains libraries, constants, and prototypes to be used

int  main(int argc, char *argv[])
{
    srand((unsigned)time(NULL));  
    if(validate(argc, argv)) 
    {
	const int NBR_HANDS = atoi(argv[2]);

	struct card *deckPtrS = makeDeckStruct();
	printf("%s\n", "Initial Deck:");
	printDeckStruct(deckPtrS, TOTAL_CARDS);

	shuffleDeckStruct(deckPtrS);
	printf("\n%s\n", "Shuffled Deck:");
	printDeckStruct(deckPtrS, TOTAL_CARDS);

	struct hand *handsPtr = dealHands(deckPtrS, CARDS_PER_HAND, NBR_HANDS);
	printf("\n%s\n", "Dealing Hands...");
	printHands(handsPtr, CARDS_PER_HAND, NBR_HANDS);
	
	sortHands(handsPtr, CARDS_PER_HAND, NBR_HANDS);
	printf("\n%s\n", "Sorted Hands:");
	printHands(handsPtr, CARDS_PER_HAND, NBR_HANDS);

	assignPokerRanks(handsPtr, NBR_HANDS);
	printf("\n%s\n", "Poker Ranks:");
	printPokerRanks(handsPtr, NBR_HANDS);
	printf("\n%s\n", "Winners: ");
	printWinners(handsPtr, NBR_HANDS);
	
	//test();
    }  

    return NO_ERROR;
}

/**
 * Will validate command-line input. Both values must be in the given range [1 - 13].
 * They must both be integers and their product cannot exceed 52 since there are 52 cards
 * in a deck of cards.
 * @param argc is the number of command-line inputs
 * @param *argv[] is the actual values of the command-line input
 * @return if the input values are valid
 */
int validate(int argc, char *argv[])
{
    printf("%s\n", "WARNING: Floating point values will be truncated to the whole number.");
    printf("%s\n", "Cards per hand will always be five for stud poker.");
    int isValid = 0;
    int nbrOfHands = 0;

    if(argc != NEEDED_ARGS)
    {
	printf("%s\n", "Proper usage: ./cardGame.out cardsPerHand nbrOfHands");
    }
    else
    {
	//Position 0 holds the name of the file which we do not need
	//Position 1 is irrelevant since we will always use five as the cards per hand
	nbrOfHands = atoi(argv[2]);
	int inRangeNbrOfHands = (nbrOfHands >= LOW_VALUE && nbrOfHands <= MAX_VALUE);
	if(inRangeNbrOfHands == TRUE)
	{
		isValid = TRUE;
	}
	else
	{
		printf("%s%d%s%d%s\n", "Values must be numbers in the range [", LOW_VALUE, " - ", MAX_VALUE, "]");
	}
    }
    return isValid;
}

/*
 * Tests the poker ranks.
 * @param N/A
 * @return N/A
 *
void test()
{
	struct hand hands[MAX_VALUE]; 
	struct hand *handsPtr = NULL;
	handsPtr = hands;
	assignPokerRanks(handsPtr, MAX_VALUE);
	printf("\n%s\n", "(TEST)Poker Ranks:");
	printPokerRanks(hands, MAX_VALUE);	
	printf("\n%s\n", "(TEST)Winners: ");
}*/
