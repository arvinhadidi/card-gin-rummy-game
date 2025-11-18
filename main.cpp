#include <iostream>
#include "deck.h"
#include <stack>
#include <unordered_map>
#include <vector>
#include <set>
using namespace std;

void display_hand(const vector<Card>& hand) {
    cout << '[';
    for (Card c: hand) {
        cout << " " << c;
    }
    cout << ']';
    cout << '\n';
}

vector<vector<Card>> find_sets(const vector<Card>& hand) {
    vector<vector<Card>> sets;
    
    // Map: rank -> vector of cards with that rank
    unordered_map<uint8, vector<Card>> rankMap;
    
    // Populate the map
    for (const Card& c : hand) {
        rankMap[c.rank].push_back(c);
    }

    for (auto& [rank, cards] : rankMap) {
        if (cards.size() >= 3) {
            // verify different suits
            set<uint8> suits;
            for (const Card& c : cards) {
                suits.insert(c.suit);
            }
            
            // we must have at least 3 different suits for it to be a set
            // otherwise we don't care
            if (suits.size() >= 3) {
                // get the cards with different suits
                vector<Card> validSet;
                set<uint8> usedSuits;
                
                for (const Card& c : cards) {
                    if (usedSuits.find(c.suit) == usedSuits.end()) {
                        validSet.push_back(c);
                        usedSuits.insert(c.suit);
                    }
                }
                
                if (validSet.size() >= 3) {
                    sets.push_back(validSet);
                }
            }
        }
    }

    // display each set to terminal
    cout << "Sets:";
    for (auto& cardSet: sets) {
        display_hand(cardSet);
    }

    return sets;
}

vector<vector<Card>> find_runs(const vector<Card>& hand) {
    vector<vector<Card>> runs;
    
    // map with key: suit, value: cards of that suit
    unordered_map<uint8, vector<Card>> suitMap;
    
    // add all cards to relevant place in map
    for (const Card& c : hand) {
        suitMap[c.suit].push_back(c);
    }
    
    // for each suit, find consecutive runs
    for (auto& [suit, cards] : suitMap) {
        // we need at least 3 cards for a run
        if (cards.size() >= 3) {
            // sort cards by rank + find consecutive sequences
            sort(cards.begin(), cards.end());
            
            vector<Card> currentRun = {cards[0]};
            
            for (size_t i = 1; i < cards.size(); i++) {
                // compare current card's rank to the most-recently added one
                if (cards[i].rank == currentRun.back().rank + 1) {
                    // we have a consecutive card - add it to the run
                    currentRun.push_back(cards[i]);
                } else {
                    // we found a gap so this is the end of that run
                    // check if the run is valid
                    if (currentRun.size() >= 3) {
                        runs.push_back(currentRun);
                    }
                    // start new run
                    currentRun = {cards[i]};
                }
            }
            
            // Don't forget to check last run
            if (currentRun.size() >= 3) {
                runs.push_back(currentRun);
            }
        }   
    }

    // display each run to terminal
    cout << "Runs:";
    for (auto& cardSet: runs) {
        display_hand(cardSet);
    }
    
    return runs;
}

int calculate_deadwood(const vector<Card>& hand, 
                      const vector<vector<Card>>& sets,
                      const vector<vector<Card>>& runs) {
    // collect all cards that are in melds
    set<pair<uint8, uint8>> melded_cards;  // using (suit, rank) as unique identifier
    
    // add all cards from sets
    for (const auto& meld : sets) {
        for (const Card& c : meld) {
            melded_cards.insert({c.suit, c.rank});
        }
    }
    
    // add all cards from runs
    for (const auto& meld : runs) {
        for (const Card& c : meld) {
            melded_cards.insert({c.suit, c.rank});
        }
    }
    
    // calculate points for unmelded cards
    int deadwood = 0;
    for (const Card& c : hand) {
        // check if card is NOT in any meld
        if (melded_cards.find({c.suit, c.rank}) == melded_cards.end()) {
            if (c.rank == 1) {
                deadwood += 1;  // ace = 1 point
            } else if (c.rank >= 10) {
                deadwood += 10;  // 10, J, Q, K = 10 points
            } else {
                deadwood += c.rank;  // 2-9 = face value
            }
        }
    }
    
    return deadwood;
}

void take_turn(Deck& deck, vector<Card>& hand, const string& playerName, stack<Card>& discardPile, vector<vector<Card>>& playerSets, vector<vector<Card>>& playerRuns, bool& knocked) {
    
    cout << "Cards remaining in stock: " << deck.remaining() << "\n";
    cout << "Top of discard pile: " << discardPile.top();
    
    cout << '\n' << playerName << "'s hand:\n";
    display_hand(hand);
    
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

    cout << '\n' << playerName << "'s hand:\n";
    display_hand(hand);
    playerSets = find_sets(hand);
    playerRuns = find_runs(hand);

    // DISCARD PHASE
    cout << "\nWhich card do you want to discard from 1 to " << hand.size() << "? ";
    int discardChoice;
    cin >> discardChoice;

    Card discarded = hand[discardChoice-1];
    hand.erase(hand.begin() + discardChoice-1);
    discardPile.push(discarded);

    // we recalculate deadwood after discarding, to see if we can knock
    playerSets = find_sets(hand);
    playerRuns = find_runs(hand);
    int deadwood = calculate_deadwood(hand, playerSets, playerRuns);
    cout << "\nYour deadwood: " << deadwood << " points\n";

    if (deadwood == 0) {
        cout << "\n " << playerName << " has Gin! \n";
        knocked = true;
    } else if (deadwood <= 10) {
        cout << "\n" << playerName << ", you can knock (deadwood = " << deadwood << ")\n";
        cout << "Do you want to knock? (1=Yes, 2=No): ";
        int knock_choice;
        cin >> knock_choice;
        
        if (knock_choice == 1) {
            cout << "\n" << playerName << " knocks!\n";
            knocked = true;
        }
    }
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
    vector<vector<Card>> sets1;
    vector<vector<Card>> sets2;
    vector<vector<Card>> runs1;
    vector<vector<Card>> runs2;

    stack<Card> discardPile;
    discardPile.push(deck.deal_card()); // discard pile initially starts with one card

    // play until someone knocks or deck runs out
    bool gameOver = false;
    int turn = 0;

    while (!gameOver && deck.remaining() > 0) {
        turn++;
        cout << "\n========== TURN " << turn << " ==========\n";
        
        // player 1's turn
        take_turn(deck, hand1, "Player 1", discardPile, sets1, runs1, gameOver);

        if (gameOver) {
            break;
        }
        
        // player 2's turn
        take_turn(deck, hand2, "Player 2", discardPile, sets2, runs2, gameOver);
    }
    
    if (!gameOver) {
        cout << "\nDeck is empty! Game ends in a draw.\n";
    }
    
    return 0;
}