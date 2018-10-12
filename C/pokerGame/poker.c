/*
| Source Code: 	poker.c
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
|		make
|	       	./poker.out cardsPerHand nbrOfHands
|
| Description: 	Program will creat a deck of cards, shuffle the deck, and
|		deal out hands determined by user input. The deck and hands
|		will be printed after each corresponding step.
|		
|  Required Features Not Included: N/A
|  Known Bugs: N/A
*/

#include "pokerGame.h"		 //Contains libraries, constants, and prototypes to be used

/*
 * Creates a deck of regular playing cards
 * Each index will contain a rank value and a suit value.
 * @param N/A
 * @return the pointer to the deck (array of struct card)
 */
struct card * makeDeckStruct()
{
	static struct card deck[TOTAL_CARDS];
	int cardIndex = 0;
	for (;cardIndex < TOTAL_CARDS; cardIndex++)
	{
		deck[cardIndex].rank = cardIndex % RANKS;
		if(cardIndex < HEART * RANKS)
		{
			deck[cardIndex].suit = 'H';
		}
		else if(cardIndex < CLUB * RANKS)
		{
			deck[cardIndex].suit = 'C';
		}
		else if(cardIndex < DIAMOND * RANKS)
		{
			deck[cardIndex].suit = 'D';
		}
		else
		{
			deck[cardIndex].suit = 'S';
		}
	}
	struct card * deckPtrS = deck;
	return deckPtrS;
}

/*
 * Prints a deck of cards.
 * @param *deckPtr pointer to the deck
 * @param totalCards is the number of cards in the deck to be printed
 * @return N/A
 */
void printDeckStruct(struct card *deckPtr, int totalCards)
{
	int cardIndex = 0;
	for(;cardIndex < totalCards; cardIndex++)
	{
		printRank(deckPtr[cardIndex]);
		printSuit(deckPtr[cardIndex]);
		if((cardIndex % RANKS) == (RANKS - 1)){printf("\n");}		
	}
}

/*
 * Prints the rank of a card.
 * @param the card to be printed
 * @return N/A
 */
void printRank(struct card currentCard)
{
	printf("%5c", RANK[currentCard.rank]);
}

/*
 * Prints the suit of a card
 * @param the card to be printed
 * return N/A
 */
void printSuit(struct card currentCard)
{
	printf("%c", currentCard.suit);
}

/*
 * Shuffles a deck of cards by swapping random indexes.
 * Shuffle source:
 * https://en.wikipedia.org/wiki/Fisher%E2%80%93%Yates_shuffle#The_modern_algorithm
 * Time and random number source:
 * Sections 5.10 and 5.11 in "C How to Program" by Paul Deitel and Harvey Deitel
 * @param *deckPtr the pointer of the deck
 * @return N/A
 */
void shuffleDeckStruct(struct card *deckPtr)
{
	int cardIndex = 0;
	int randomIndex = 0;
	struct card temp;

	for(;cardIndex < TOTAL_CARDS; cardIndex++)
	{
		randomIndex = rand() % TOTAL_CARDS;
		temp = deckPtr[cardIndex];
		deckPtr[cardIndex] = deckPtr[randomIndex];
		deckPtr[randomIndex] = temp;
	}		
}

/* 
 * Deals out the hands depending on how many cards per hand and number of hands.
 * @param *deckPtr the pointer to the deck
 * @param cardsPerHand the number of cards per hand
 * @param nbrHands the number of hands
 * @return pointer to the hands (array of struct hand)
 */
struct hand * dealHands(struct card *deckPtr, int cardsPerHand, int nbrHands)
{
	int handIndex = 0;
	int cardCounter = 0;
	int cardIndex = 0;
	static struct hand hands[MAX_VALUE];
	struct hand *handsPtr = NULL;
	for(; handIndex < nbrHands; handIndex++)
	{
		for(; cardCounter < cardsPerHand; cardCounter++)
		{
			hands[handIndex].hand[cardCounter] = deckPtr[cardIndex];
			cardIndex++;
		}
		cardCounter = 0;
		hands[handIndex].pokerRank = 0;
	}
	handsPtr = hands;
	return handsPtr;
}

/*
 * Sorts the hands from lowest to highest with their respective rank.
 * Sort source:
 * https://en.wikipedia.org/wiki/Insertion_sort
 * @param *handsPtr the pointer to the array of hands
 * @param cardsPerHand the number of cards per hand
 * @param nbrHands the number of hands
 * @return N/A
 */
void sortHands(struct hand *handsPtr, int cardsPerHand, int nbrHands)
{
	int cardIndex = 1;
	int newIndex = 0;
	int playerIndex = 0;
	struct card temp;
	for(; playerIndex < nbrHands; playerIndex++)
	{ 
		while(cardIndex < cardsPerHand)
		{
			newIndex = cardIndex;
			int thePreviousRank = handsPtr[playerIndex].hand[newIndex - 1].rank;
			int theCardRank = handsPtr[playerIndex].hand[newIndex].rank;
			while((newIndex > 0) && (thePreviousRank > theCardRank))
			{
				temp = handsPtr[playerIndex].hand[newIndex];
				handsPtr[playerIndex].hand[newIndex] = handsPtr[playerIndex].hand[newIndex - 1];
				handsPtr[playerIndex].hand[newIndex - 1] = temp;
				newIndex--;
				thePreviousRank = handsPtr[playerIndex].hand[newIndex - 1].rank;
				theCardRank = handsPtr[playerIndex].hand[newIndex].rank;
			}
			cardIndex++;
		}
		cardIndex = 0;
	}
}

/*
 * Prints the hands.
 * @param *handsPtr the pointer to the array of hands
 * @param cardsPerHand the number of cards per hand
 * @param nbrHands the number of hands
 * @return N/A
 */
void printHands(struct hand *handsPtr, int cardsPerHand, int nbrHands)
{
	int handIndex = 0;
	int cardIndex = 0;
	for(; handIndex < nbrHands; handIndex++)
	{
		printf("%s%d%s", "Player ", handIndex + 1, ":");
		for(; cardIndex < cardsPerHand; cardIndex++)
		{
			printRank(handsPtr[handIndex].hand[cardIndex]);
			printSuit(handsPtr[handIndex].hand[cardIndex]);		
			if((cardIndex % cardsPerHand) == (cardsPerHand - 1)){printf("\n");}
		}
		cardIndex = 0;
	}
}

/*
 * Checks to see if there is one or two pairs in the hand
 * @param *handsPtr the pointer to the array of hands
 * @param currentHand the current hand
 * @return N/A
 */
void checkPairs(struct hand *handsPtr, int currentHand)
{
	int howManyPairs = 0;
	int cardIndex = 1;
	for(; cardIndex < CARDS_PER_HAND; cardIndex++)
	{
		struct card theCard = handsPtr[currentHand].hand[cardIndex - 1];
		struct card nextCard = handsPtr[currentHand].hand[cardIndex];
		if(theCard.rank == nextCard.rank)
		{
			howManyPairs++;
			cardIndex++;
		}
	}
	handsPtr[currentHand].pokerRank = howManyPairs;	 
}

/*
 * Checks for four of a kind and three of a kind
 * @param *handsPtr the pointer to the array of hands
 * @param currentHand the current hand
 * @return N/A
 */
void checkSameKind(struct hand *handsPtr, int currentHand)
{
	int sameKind = 1;
	int cardIndex = 1;
	for(; cardIndex < CARDS_PER_HAND; cardIndex++)
	{
		if(handsPtr[currentHand].hand[cardIndex - 1].rank == handsPtr[currentHand].hand[cardIndex].rank)
		{
			sameKind++;
		}
		else
		{
			sameKind = 1;
		}
	}
	if(sameKind == TOK)
	{
		handsPtr[currentHand].pokerRank = TOK;
	}
	else if(sameKind == FOK)
	{
		handsPtr[currentHand].pokerRank = FOK;
	}
}

/*
 * Checks for a straight, all five cards in a row by rank
 * @param *handsPtr the pointer to the array of hands
 * @param currentHand the current hand
 * @return N/A
 */
void checkStraight(struct hand *handsPtr, int currentHand)
{
	int cardsInRow = 1;
	int cardIndex = 1;
	for(; cardIndex < CARDS_PER_HAND; cardIndex++)
	{
		if((handsPtr[currentHand].hand[cardIndex - 1].rank + 1) == handsPtr[currentHand].hand[cardIndex].rank)
		{
			cardsInRow++;	
		}
	}
	if(cardsInRow == CARDS_PER_HAND)
	{
		handsPtr[currentHand].pokerRank = S;
	}
	else if ((cardsInRow == (CARDS_PER_HAND - 1)))
	{
		if((RANK[handsPtr[currentHand].hand[0].rank] ==  '2') &&(RANK[handsPtr[currentHand].hand[1].rank] == '3'))
		{
			if(RANK[handsPtr[currentHand].hand[CARDS_PER_HAND - 1].rank] == 'A')
			{
				handsPtr[currentHand].pokerRank = S;
			}
		}
	}
}

/*
 * Checks to see if all the cards have the same suit
 * @param *handsPtr the pointer to the array of hands
 * @param currentHand the current hand
 * @return N/A
 */
void checkFlush(struct hand *handsPtr, int currentHand)
{
	int suitsInRow = 1;
	int cardIndex = 1;
	for(; cardIndex < CARDS_PER_HAND; cardIndex++)
	{
		if((handsPtr[currentHand].hand[cardIndex - 1].suit) == handsPtr[currentHand].hand[cardIndex].suit)
		{
			suitsInRow++;
		}
	}
	if(suitsInRow == F)
	{
		handsPtr[currentHand].pokerRank = F;
	}
}

/*
 * Checks to see if there is a full house (a three of a kind and a pair)
 * @param *handsPtr the pointer to the array of hands
 * @param currentHand the current hand
 * @return N/A
 */
void checkFullHouse(struct hand *handsPtr, int currentHand)
{
	checkSameKind(handsPtr, currentHand);
	if((handsPtr[currentHand].pokerRank) == TOK)
	{
		//Values used to get the first two cards and the last two cards from
		//a hand of five cards
		struct card firstCard = handsPtr[currentHand].hand[0];
		struct card secondCard = handsPtr[currentHand].hand[1];
		struct card fourthCard = handsPtr[currentHand].hand[3];
		struct card lastCard = handsPtr[currentHand].hand[4];
		if((firstCard.rank == secondCard.rank) && (fourthCard.rank == lastCard.rank))
		{
			handsPtr[currentHand].pokerRank = FH;
		}
	}
}

/*
 * Checks to see the hand is a straight and a flush
 * @param *handsPtr the pointer to the array of hands
 * @param currentHand the current hand
 * @return N/A
 */
void checkStraightFlush(struct hand *handsPtr, int currentHand)
{
	checkStraight(handsPtr, currentHand);
	if(handsPtr[currentHand].pokerRank == S)
	{
		checkFlush(handsPtr, currentHand);
		if(handsPtr[currentHand].pokerRank == F)
		{
			handsPtr[currentHand].pokerRank == SF;
		}
	}
}

/*
 * Assigns the appropriate poker rank to each hand.
 * @param *handsPtr the pointer to the array of hands
 * @param nbrHands the number of hands
 * @return N/A
 */
void assignPokerRanks(struct hand *handsPtr, int nbrHands)
{
	int handIndex = 0;
	for(; handIndex < nbrHands; handIndex++)
	{
		handsPtr[handIndex].pokerRank = 0;
		if(handsPtr[handIndex].pokerRank == 0)
			{checkStraightFlush(handsPtr, handIndex);}
		if(handsPtr[handIndex].pokerRank == 0)
		{
			checkSameKind(handsPtr, handIndex);
			if(handsPtr[handIndex].pokerRank == TOK)
			{handsPtr[handIndex].pokerRank = 0;}
		}
		if(handsPtr[handIndex].pokerRank == 0)
			{checkFullHouse(handsPtr, handIndex);}
		if(handsPtr[handIndex].pokerRank == 0)
			{checkFlush(handsPtr, handIndex);}
		if(handsPtr[handIndex].pokerRank == 0)
			{checkStraight(handsPtr, handIndex);}
		if(handsPtr[handIndex].pokerRank == 0)
			{checkSameKind(handsPtr, handIndex);}
		if(handsPtr[handIndex].pokerRank == 0)
			{checkPairs(handsPtr, handIndex);}
	}	
}

/*
 * Prints the poker ranks for each player.
 * @param *handsPtr the pointer to the array of hands
 * @param nbrHands the number of hand
 * @return N/A
 */
void printPokerRanks(struct hand *handsPtr, int nbrHands)
{
	int handIndex = 0;
	int pokerRank = 0;
	for(; handIndex < nbrHands; handIndex++)
	{
		pokerRank = handsPtr[handIndex].pokerRank;
		printf("%s%d%s%5s\n", "Player ", handIndex + 1, ": ", POKER_RANKS[pokerRank]);
	}
}

/*
 * Prints the winner(s)
 * @param *handsPtr the pointer to the array of hands
 * @param nbrHands the number of hands
 * @return N/A
 */
void printWinners(struct hand *handsPtr, int nbrHands)
{
	int handIndex = 0;
	int pokerRank = 0;
	int highestPokerRank = 0;
	for(; handIndex < nbrHands; handIndex++)
	{
		if(handsPtr[handIndex].pokerRank > highestPokerRank)
		{
			highestPokerRank = handsPtr[handIndex].pokerRank;
		}
	}
	
	handIndex = 0;
	for(; handIndex < nbrHands; handIndex++)
	{
		if(handsPtr[handIndex].pokerRank == highestPokerRank)
		{
			printf("%s%d%s\n", "Player ", handIndex + 1, ": Winner!");
		}
	}
}
