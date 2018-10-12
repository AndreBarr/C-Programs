/*
| Source Code: 	cardGame.c
| Author: 	Andre Barrientos
| Student ID: 	5443095
| Assignment: 	Program #3
|
| Course:  COP 4338
| Section: U03
| Instructor William Feild
| Due Date: 10/17/17, at the beginning of class
|		I herbey certify that this collective work is my own
|		and none of it is the work of any other person or entity
|
|		____________________________
| Language: 	C
| Compile/Run: 	gcc cardGame.c -o cardGame.out
|	       	./cardGame.out cardsPerHand nbrOfHands
|
| Description: 	Program will creat a deck of cards, shuffle the deck, and
|		deal out hands determined by user input. The deck and hands
|		will be printed after each corresponding step.
|		
| Input:   Two numbers both in the range [1 - 13] (command-line input)
| Output:  The deck of cards created, the shuffled deck, and the hands dealt out.
| Process:  1. Command line arguements are validated
|	    2. Deck is created and printed
|	    3. Deck is shuffled and printed
|	    4. Hands are dealt out 
|  Required Features Not Included: N/A
|  Known Bugs: N/A
*/

#include "cardGame.h"		 //Contains libraries, constants, and prototypes to be used

int  main(int argc, char *argv[])
{  
    int cardsPerHand = 0;
    int nbrOfHands = 0;
    if(validate(argc, argv)) 
    {
	cardsPerHand = atoi(argv[1]);
	nbrOfHands = atoi(argv[2]);

	int *deckPtr = makeDeck();
	printf("%s\n", "Initial Deck:");
	printDeck(deckPtr, TOTAL_CARDS);

	shuffleDeck(deckPtr);
	printf("\n%s\n", "Shuffled Deck:");
	printDeck(deckPtr, TOTAL_CARDS);

	printf("\n%s\n", "Dealing Hands...");
	//+ 1 needed because of printDeck implementation
	dealOutHands(deckPtr, cardsPerHand + 1, nbrOfHands);
    }  

    return NO_ERROR;
}

/**
 * Will validate command-line input. Both values must be in the given range [1 -   											13].
 * They must both be integers and their product cannot exceed 52 since there are 	52 cards
 * in a deck of cards.
 * @param argc is the number of command-line inputs
 * @param *argv[] is the actual values of the command-line input
 * @return if the input values are valid
 */
int validate(int argc, char *argv[])
{
    printf("%s\n", "WARNING: Floating point values will be truncated to the whole 											number.");
    int isValid = 0;
    int cardsPerHand = 0;
    int nbrOfHands = 0;
    int nbrDealtCards = 0;

    if(argc != NEEDED_ARGS)
    {
	printf("%s\n", "Proper usage: ./cardGame.out cardsPerHand nbrOfHands");
    }
    else
    {
	//Position 0 holds the name of the file which we do not need
	cardsPerHand = atoi(argv[1]);
	nbrOfHands = atoi(argv[2]);
	int inRangeCardsPerHand = (cardsPerHand >= LOW_VALUE && cardsPerHand <= 											MAX_VALUE);
	int inRangeNbrOfHands = (nbrOfHands >= LOW_VALUE && nbrOfHands <= 												MAX_VALUE);
	if((inRangeCardsPerHand == TRUE) && (inRangeNbrOfHands == TRUE))
	{
		nbrDealtCards = cardsPerHand * nbrOfHands;
		if(nbrDealtCards < TOTAL_CARDS)
		{
			isValid = TRUE;
		}
		else
		{
			printf("%s%d%s\n", "Cannot deal out ", TOTAL_CARDS," or more 											cards.");
			printf("(%s%d)\n", "The product of the two values must be 										< ", TOTAL_CARDS);
		}
	}
	else
	{
		printf("%s%d%s%d%s\n", "Values must be numbers in the range [", 								LOW_VALUE, " - ", MAX_VALUE, "]");
	}
    }
    return isValid;
}

/**
 * Creates a deck of cards. Index 0 is skipped to make manipulating the deck 	easier.
 * Each index is filled with the index value to have an array with values 1 - 52 	in order.
 * @param N/A
 * @return The pointer to the deck (an array of numbers)
 */
int * makeDeck(void)
{
	//All values initialized to 0
	//Static so that the deck is not destroyed after function execution ends
	static int deck[TOTAL_CARDS] = {0};
	//Initialized to 1 because index 0 is not going to be used
	int cardIndex  = 1;

	for(;cardIndex < TOTAL_CARDS; cardIndex++)
	{
		deck[cardIndex] = cardIndex;
	}

	int *deckPtr = deck;
	return deckPtr;    
}

/**
 * Prints a deck of cards with the rank followed by the suit.
 * Any card with the number 0 in front is a 10. All others are shown
 * respective rank.
 * @param *deckPtr is the pointer to the deck
 * @param totalCards is the number of cards in the deck to be printed
 * @return N/A
 */
void printDeck(int *deckPtr, int totalCards)
{
	//Initialized to 1 because index 0 is not going to be used
	int cardIndex = 1;
	int currentCard = 0;
	int cardRank = 0;
	int nbrdCard = 0;
	int isNbrCard = FALSE;

	char rank = '\0';
	char suit = '\0';

	for(;cardIndex < totalCards; cardIndex++)
	{
		isNbrCard = FALSE;
		currentCard = deckPtr[cardIndex];
		cardRank = currentCard % RANKS;
		if(cardRank == ACE)	
		{
			rank = 'A';
		}
		else if(cardRank == KING)
		{
			rank = 'K';
		}
		else if(cardRank == QUEEN)
		{
			rank = 'Q';
		}
		else if(cardRank == JACK)
		{
			rank = 'J';
		}
		else
		{
			isNbrCard = TRUE;
			nbrdCard = (currentCard % RANKS) % HIGH_NBR_CARD;
		}
		
		if(currentCard <= (HEART * RANKS))
		{
			suit = 'H';
		}
		else if(currentCard <= (CLUB * RANKS))
		{
			suit = 'C';
		}
		else if(currentCard <= (DIAMOND * RANKS))
		{
			suit = 'D';
		}
		else if(currentCard <= (SPADE * RANKS))
		{
			suit = 'S';
		}
		
		if(isNbrCard)
		{
			printf("%5d%c", nbrdCard, suit);
		}
		else
		{
			printf("%5c%c", rank, suit);
		}

		if((cardIndex % RANKS) == 0){printf("\n");}
	}	    
}

/*
 * Shuffles a deck of cards by swapping random indexes.
 * Shuffle source:
 * https://en.wikipedia.org/wiki/Fisher%E2%80%93%	Yates_shuffle#The_modern_algorithm
 * Time and random number source:
 * Sections 5.10 and 5.11 in "C How to Program" by Paul Deitel and Harvey Deitel
 * @param *deckPtr the pointer to the deck
 * @return N/A
 */
void shuffleDeck(int *deckPtr)
{
	//Initialized to 1 because index 0 is not going to be used
	int cardIndex = 1;
	int randomIndex = 0;
	int tempValue = 0;
	//Seed is different everytime which further randomizes the numbers produced
	srand(time(NULL));

	for(;cardIndex <= TOTAL_CARDS; cardIndex++)
	{
		//+ 1 to avoid value zero and  - 1 to avoid index 53 which would be 			//outOfBounds
		randomIndex = 1 + (rand() % (TOTAL_CARDS - 1));
		tempValue = deckPtr[cardIndex];
		deckPtr[cardIndex] = deckPtr[randomIndex];
		deckPtr[randomIndex] = tempValue;
	}	
}

/*
 * Prints the hands for each player. Cards are dealt from the top of the deck.
 * @param *deckPtr the pointer to the deck
 * @param cardsPerHand the number of cards per hand
 * @param nbrHands the number of hands to be dealt out
 * @return N/A
 */
void dealOutHands(int *deckPtr, int cardsPerHand, int nbrHands)
{
	//Initialized to 1 because index 0 is not going to be used
	int cardIndex = 1;
	//Initialized to 1 because index 0 is not going to be used
	int deckIndex = 1;
	int handsIndex = 0;
	//All values initialized to 0
	int hand[MAX_VALUE] = {0};
	int *handPtr = NULL;

	for(; handsIndex < nbrHands; handsIndex++)
	{
		
		for(;cardIndex < cardsPerHand; cardIndex++)
		{
			hand[cardIndex] = deckPtr[deckIndex];
			deckIndex++;
		}
		//cardIndex being reinitialized to its starting value
		cardIndex = 1;
		handPtr = hand;
		//+ 1 because can't be a zero player
		printf("%s%d%s", "Player ", handsIndex + 1, ": ");
		printDeck(handPtr, cardsPerHand);
		printf("\n");
	}	
}
