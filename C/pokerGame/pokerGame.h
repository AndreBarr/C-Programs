/*
| Source Code: 	pokerGame.h
| Author: 	Andre Barrientos
| Panther ID: 	5443095  
| Due Date:	10/31/17, at the beginning of class
|
| Description: 	Header file containing all the needed libraries, constants, and prototypes.
*/

#include <stdio.h>	//Standard Input and Output
#include <time.h> 	//Used to randomize the index for the shuffle
#include <stdlib.h>	//Use to generate random numbers

#define NO_ERROR   	 0	//No error value
#define RANKS     	13	//Number of ranks in a deck of cards
#define TOTAL_CARDS	52      //Total number of cards in a deck (plus one to ignore index 0)
#define TRUE     	 1	//True value
#define FALSE		 0	//False Value
#define LOW_VALUE	 1	//Lowest valid value
#define MAX_VALUE	 7	//Highest valid Value
#define CARDS_PER_HAND   5	//Number of cards per hand for a stud poker game
#define NEEDED_ARGS	 3	//Number of needed arguments from command-line
#define HAND_RANKS	 9	//Number of hand ranks

static const char RANK[RANKS] = {'2', '3', '4', '5',
				 '6', '7', '8', '9',
			 	 'T', 'J', 'Q', 'K', 'A'};

static const char *POKER_RANKS[HAND_RANKS] = {"High Card", "One Pair", "Two Pair",
					     "Three of a Kind", "Straight", "Flush",
					     "Full House", "Four of a Kind", "Straight Flush"};

enum suit {HEART = 1, CLUB, DIAMOND, SPADE};				//The four suits in a deck of cards
enum pokerRank {HC, OP, TP, TOK, S, F, FH, FOK, SF};			//Poker ranks (abbreviated)

struct card
{
	int rank;
	char suit;
};

struct hand
{
	struct card hand[CARDS_PER_HAND];
	int pokerRank;
};

struct card * makeDeckStruct();						//Makes an ordered deck of playing cards
void printDeckStruct(struct card *deckPtr, int totalCards);		//Prints the deck
void printRank(struct card currentCard);				//Prints the rank
void printSuit(struct card currentCard);				//Prints the suit
void shuffleDeckStruct(struct card *deckPtr);				//Shuffles the deck
struct hand * dealHands(struct card *deckPtr, int cardsPerHand, int nbrHands);	//Deals the hands
void sortHands(struct hand *handPtr, int cardsPerHand, int nbrHands);	//Sorts the hands
void printHand(struct hand *handPtr, int cardsPerHand, int nbrHands);	//Prints the hands
void checkPairs(struct hand *handPtr, int currentHand);			//Checks for pairs	
void checkSameKind(struct hand *handsPtr, int currentHand);		//Checks for the same rank value
void checkStraight(struct hand *handsPtr, int currentHand);		//Cheks for a straight
void checkFlush(struct hand *handsPtr, int currentHand);		//checks for a flush
void checkFullHouse(struct hand *handsPtr, int currentHand);		//Cheks for a full house	
void checkStraightFlush(struct hand *handsPtr, int currentHand);	//Checks for a Straight Flush
void assignPokerRanks(struct hand *handsPtr, int nbrHands);			
//void test();

int validate(int argc,char *argv[]);					//Used to validate command-line input

