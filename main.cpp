#include <iostream>
#include "deck.h"
#include <stack>
using namespace std;

void display_hand(const vector<Card>& hand, const string& playerName) {
    cout << '\n' << playerName << "'s hand:\n";
    for (Card c: hand) {
        cout << " " << c;
    }
    cout << '\n';
}

void take_turn(Deck& deck, vector<Card>& hand, const string& playerName, stack<Card>& discardPile) {
    
    cout << "Cards remaining in stock: " << deck.remaining() << "\n";
    cout << "Top of discard pile: " << discardPile.top();
    
    display_hand(hand, playerName);
    
    // DRAW PHASE
    cout << "\nChoose an action:\n";
    cout << "1. Draw from stock pile\n";
    cout << "2. Draw from discard pile\n";
    cout << "Your choice: ";
    
    int choice;
    cin >> choice;
    
    Card drawn;
    if (choice == 1) {
        // Draw from stock
        drawn = deck.deal_card();
        cout << "\nYou drew from stock: " << drawn;
    } else {
        // Draw from discard
        drawn = discardPile.top();
        discardPile.pop();
        cout << "\nYou took from discard: " << drawn;
    }

    hand.emplace_back(drawn);
    display_hand(hand, playerName);

    // DISCARD PHASE
    cout << "\nWhich card do you want to discard from 1 to " << hand.size() << "? ";
    int discardChoice;
    cin >> discardChoice;

    Card discarded = hand[discardChoice-1];
    hand.erase(hand.begin() + discardChoice-1);
    discardPile.push(discarded);
}

int main(int argc, const char * argv[]) {
    Deck deck;
    // discardPile as a stack
    // dealHand(10) for each player
    // add 1 card to discard pile
    // player1 starts and can take from discard pile or the deck
    // then we have player2 turn

    // methods we need
    // something to dictate who's turn it is
        // maybe just a while (!deck.isEmpty())

    vector<Card> hand1 = deck.deal_hand(10);
    vector<Card> hand2 = deck.deal_hand(10);

    stack<Card> discardPile;
    discardPile.push(deck.deal_card()); // discard pile initially starts with one card

    // test with a few turns
    for (int turn = 0; turn < 3; turn++) {
        cout << "\n========== TURN " << turn + 1 << " ==========\n";
        take_turn(deck, hand1, "Player 1", discardPile);
        take_turn(deck, hand2, "Player 2", discardPile);
    }
}