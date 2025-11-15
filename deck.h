#ifndef deck_h
#define deck_h

//Add all your included libraries below this line
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
using std::cout;

// use typedef to define differnt types of integers
//memory saving measures
// Rename to save time while typing
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

// using const and constexpr for all variables that are not meant to vary...
const std::string suitstr = "_CDHS"; //Clubs, Diamond, Heart and Spade. Const as they will never change.
// first char is underscore as we refer to this suitstr and access it by index. We have no 0 rank.
const std::string facecards = "ATJQK"; //Ace, Ten, Jack, Queen, King. Const as will never change.
constexpr uint8 default_deck = 52; // Every deck of cards must include 52 cards
constexpr uint8 max_decks = 1; // on start, the programmer can initiate a maximum of 1 pack in a deck
constexpr uint8 suitcount = 4; //There are 4 possible suit
constexpr uint8 rankcount = 13; // there are 13 different ranking in a deck 2 to 10 +jack, Queen, king and Ace


// Add the struct
// A card is made up of a rank of a particular suite.
typedef struct {
    uint8 suit : 4;
    uint8 rank : 4;
} Card;


// add content to class Deck. the word DATATYPE is just a filler. 
// Change for any datatype that suits you based on the 
class Deck
{
private:
    // uint8 cardsUsed = 0;
    uint8 cardsPerDeck = 0;
    uint8 numDecks = 0;
    uint16 remainingCardCount = 0;
    std::vector<Card> deck;

public:
    //used unsigned casting as otherwise it'll assign the ascii value of 52, which is four
    Deck() : cardsPerDeck((unsigned) default_deck), numDecks(1)
    {
        init();
    }
    Deck(uint8 n) : cardsPerDeck((unsigned) default_deck), numDecks(n)
    {
        init();
    }

    void init() {
        // this is how to 
        srand((unsigned) time(nullptr));  // seed rand for later
        rand();     // this helps on some systems (MSVC)

        create_deck();
        shuffle_deck();
    }
    
    void create_deck()
    {
        if (numDecks > max_decks) {
            std::cout<<"\n Too many decks (max 10)";
        } else {
            if (remainingCardCount>0) {
                delete_deck();
            }

            remainingCardCount = numDecks * cardsPerDeck;

            // for each deck
                // for each suit
                    // for each rank
            for (uint8 n_deck = 0; n_deck<numDecks; ++n_deck) {
                for (uint8 n_suit = 1; n_suit <= suitcount; ++n_suit) {
                    for (uint8 n_rank = 1; n_rank<=rankcount; ++n_rank) {
                        Card temp = {n_suit, n_rank};
                        deck.push_back(temp);
                    }
                } 
            }
        }
    }
    
    void delete_deck()
    {
       if (deck.size() > 0) {
        deck.empty();
       }
    }
    
        
    void shuffle_deck() {
        // use random seed
        // shuffle each card by that many spaces?
        uint16 ncards = remaining();

        for (uint16 i = 0; i<ncards; ++i) {
            // Shift all elements by r
            uint16 r = i + (rand() % (ncards - i));
            if (r != i) {
                std::swap(deck[r], deck[i]);
            }
        }
    }

    // const at end is saying "promise I won't change any member variables"
    uint16 remaining() const {
        return remainingCardCount;
    }

    void new_deck() {
        create_deck();
        shuffle_deck();
    }

    void new_deck(uint8 n) {
        numDecks = n;
        create_deck();
        shuffle_deck();
    }

    // const at start and end to show that we cannot change the card index value.
    const Card & get_card(uint16 index) const {
        if (index >= remainingCardCount) {
            std::cout<<"Deck: card out of range \n";
        }
        return deck[index];
    }

    const Card deal_card() {
       if (remainingCardCount == 0) {
            std::cout<<" \n Deck: cannot deal from empty deck";
            Card temp={0,0};
            return temp;
        } else {
            Card returnCard = deck[deck.size()-1];
            deck.erase(deck.begin()+deck.size()-1);
            remainingCardCount = deck.size();
            return returnCard;
        }
    }

    // we could repeat deal_card n times but then we are running that remainingCardCount check n times
    const std::vector<Card> deal_hand(uint8 n)
    {
        std::vector<Card> temp;

        if (remainingCardCount < n) {
            std::cout<<" \n Deck: cannot deal from empty deck";
            return temp;
        } else {
            for (int i = 0; i<n; ++i) {
                Card returnCard = deck[deck.size()-1];
                deck.erase(deck.begin()+deck.size()-1);
                temp.push_back(returnCard);
            }

            remainingCardCount = deck.size();

            return temp;
        }
    }
    
};

// here you'll be looking to write your overloaded functions

// convert rank to a number/character from the const
// convert suit to a character from the const
// return type?
// make two strings and concatenate after?

const std::ostream & operator<<(std::ostream & o, const Card & a)
{
    // cout << "[DEBUG: rank=" << (int)a.rank << " suit=" << (int)a.suit << "] ";
    char rankchar = 'x';

    if (a.rank == 1) {
        rankchar = facecards[0];
    } else if (a.rank >= 10) {
        // if a.rank == 10 then we get the index 1 = 2nd char from facecards string
        rankchar = facecards[a.rank-9];
    } else {
        rankchar = (unsigned) a.rank+48;
    }

    o<<"\n"<<std::string()<<rankchar<<suitstr[a.suit];
    return o;
   
}

// check if the suit and the rank is the same
bool operator==(Card a, Card b)
{
    return a.rank == b.rank && a.suit == b.suit;
}

#endif /* deck_h */


