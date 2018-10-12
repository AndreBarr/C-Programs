/*
| Source Code: 	cardGame.h
| Author: 	Andre Barrientos
| Panther ID: 	5443095  
| Due Date:	10/17/17, at the beginning of class
|
| Description: 	Header file containing all the needed libraries, constants, and prototypes.
*/

#include <stdio.h>	//Standard Input and Output
#include <time.h> 	//Used to randomize the index for the shuffle
#include <stdlib.h>	//Use to generate random numbers

#define NO_ERROR   	 0	//No error value
#define RANKS     	13	//Number of ranks in a deck of cards
#define TOTAL_CARDS	53   //Total number of cards in a deck 							(plus one to ignore index 0)
#define TRUE     	 1	//True value
#define FALSE		 0	//False Value
#define LOW_VALUE	 1	//Lowest valid value
#define MAX_VALUE	13	//Highest valid Value
#define NEEDED_ARGS	 3	//Number of needed arguments from command-line
#define HIGH_NBR_CARD	10	//The highest numbered card rank

enum suit {HEART = 1, CLUB, DIAMOND, SPADE};				//^The four suits in a deck of cards
enum letteredCards {ACE = 1, KING = 0, QUEEN = 12, JACK = 11};		//^Rank value obtained when value % RANKS

int validate(int argc,char *argv[]);						//^Used to validate command-line input
int * makeDeck(void);							
	//^Makes a ordered deck of playing cards
void printDeck(int *deckPtr, int totalCards);				//^Prints a deck of cards
void shuffleDeck(int *deckPtr);						
	//^Shuffles the deck
void dealOutHands(int *deckPtr, int cardsPerHand, int nbrHands);	//^Deals out the hands
