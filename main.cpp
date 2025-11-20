#include <iostream>
#include "deck.h"
#include <stack>
#include <unordered_map>
#include <vector>
#include <set>
#include <limits>
#include <thread>
#include <chrono>

using namespace std;

// global flag to control delays (set to false for faster game)
const bool ENABLE_DELAYS = true;
const int DELAY_MS = 800;  // milliseconds between messages
const int HAND_SIZE = 3;

// Custom delayed cout
void print_delayed(const string& message, bool newline = true) {
    if (ENABLE_DELAYS) {
        this_thread::sleep_for(chrono::milliseconds(DELAY_MS));
    }
    cout << message;
    if (newline) {
        cout << '\n';
    }
    cout.flush();
}

// for instant printing (such as in prompts, user input)
void print_instant(const string& message, bool newline = true) {
    cout << message;
    if (newline) {
        cout << '\n';
    }
    cout.flush();
}

// input validation
// for a lot of this game, the user must make a choice between two numbers
int get_valid_input(const string& prompt, int minVal, int maxVal) {
    int choice;
    while (true) {
        //print message and get user's choice
        print_instant(prompt, false);
        cin >> choice;
        
        // check if input was a non-number
        if (cin.fail()) {
            cin.clear();
            //ignore all non-numerical chars
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            print_delayed("Invalid input! Please enter a number.");
        }
        // check if choice is in valid range
        else if (choice < minVal || choice > maxVal) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            print_delayed("Out of range! Enter a number between " + to_string(minVal) + 
                         " and " + to_string(maxVal) + ".");
        }
        // else, input is valid
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        }
    }
}

void display_hand(const vector<Card>& hand) {
    cout << '[';
    for (const Card& c : hand) {
        cout << " " << c;
    }
    cout << "]\n";
}

/*
    A set in Gin Rummy is a list of 3+ cards with the same rank, but different suits
    eg. 3S, 3D, 3H
*/
vector<vector<Card>> find_sets(const vector<Card>& hand) {
    vector<vector<Card>> sets;
    
    // map with key: rank, value: vector of cards with that rank
    unordered_map<uint8, vector<Card>> rankMap;
    
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

    return sets;
}

/*
    A run in Gin Rummy is a list of 3+ cards with the same suit, but consecutive ranks
    eg. AS, 2S, 3S
*/
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
            sort(cards.begin(), cards.end(), [](Card a, Card b) {
                return a.rank < b.rank;
            });
            
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
            
            // Don't forget to check the lastrun
            if (currentRun.size() >= 3) {
                runs.push_back(currentRun);
            }
        }   
    }
    
    return runs;
}

int calculate_deadwood(const vector<Card>& hand, 
                      const vector<vector<Card>>& sets,
                      const vector<vector<Card>>& runs) {
    // collect all cards that are in melds
    set<pair<uint8, uint8>> meldedCards;
    
    // add all cards from sets & runs
    for (const auto& meld : sets) {
        for (const Card& c : meld) {
            meldedCards.insert({c.suit, c.rank});
        }
    }
    
    for (const auto& meld : runs) {
        for (const Card& c : meld) {
            meldedCards.insert({c.suit, c.rank});
        }
    }
    
    // calculate points for unmelded cards
    int deadwood = 0;
    for (const Card& c : hand) {
        // check if card is NOT in any meld
        if (meldedCards.find({c.suit, c.rank}) == meldedCards.end()) {
            if (c.rank == 1) {
                deadwood += 1;
            } else if (c.rank >= 10) {
                deadwood += 10;
            } else {
                deadwood += c.rank;
            }
        }
    }
    
    return deadwood;
}

/*
    Display the user's sets and runs to the
*/
void display_melds(const vector<vector<Card>>& sets, const vector<vector<Card>>& runs) {
    if (!sets.empty()) {
        print_instant("Sets found:");
        for (const auto& cardSet : sets) {
            print_instant("  ", false);
            display_hand(cardSet);
        }
    }
    
    if (!runs.empty()) {
        print_instant("Runs found:");
        for (const auto& cardRun : runs) {
            print_instant("  ", false);
            display_hand(cardRun);
        }
    }
    
    if (sets.empty() && runs.empty()) {
        print_instant("No melds yet.");
    }
}

void score_round(const string& knockerName, const vector<Card>& knockerHand,
                const vector<vector<Card>>& knockerSets, const vector<vector<Card>>& knockerRuns,
                int& knockerScore,
                const string& opponentName, const vector<Card>& opponentHand,
                const vector<vector<Card>>& opponentSets, const vector<vector<Card>>& opponentRuns,
                int& opponentScore) {
    
    int knockerDeadwood = calculate_deadwood(knockerHand, knockerSets, knockerRuns);
    int opponentDeadwood = calculate_deadwood(opponentHand, opponentSets, opponentRuns);
    
    print_delayed("\n========== SCORING ==========");
    print_delayed(knockerName + "'s final hand:");
    display_hand(knockerHand);
    display_melds(knockerSets, knockerRuns);
    print_delayed(knockerName + " deadwood: " + to_string(knockerDeadwood) + " points");
    
    print_delayed("\n" + opponentName + "'s final hand:");
    display_hand(opponentHand);
    display_melds(opponentSets, opponentRuns);
    print_delayed(opponentName + " deadwood: " + to_string(opponentDeadwood) + " points");
    
    if (knockerDeadwood == 0) {
        // GIN
        int points = opponentDeadwood + 25;
        knockerScore += points;
        print_delayed("\n GIN! " + knockerName + " scores " + to_string(points) + " points!");
    } else if (opponentDeadwood < knockerDeadwood) {
        // UNDERCUT
        int points = (knockerDeadwood - opponentDeadwood) + 25;
        opponentScore += points;
        print_delayed("\n UNDERCUT! " + opponentName + " scores " + to_string(points) + " points!");
    } else {
        // Normal knock
        int points = opponentDeadwood - knockerDeadwood;
        knockerScore += points;
        print_delayed("\n✓ " + knockerName + " scores " + to_string(points) + " points.");
    }
    
    print_delayed("\n--- Current Scores ---");
    print_delayed(knockerName + ": " + to_string(knockerScore));
    print_delayed(opponentName + ": " + to_string(opponentScore));
}

/*
    This function represents one turn:
        Picking to take from stock or discard pile
        Seeing the melds
        Calculating deadwood
        Offering player to knock if applicable
*/
void take_turn(Deck& deck, vector<Card>& hand, const string& playerName, 
              stack<Card>& discardPile, vector<vector<Card>>& playerSets, 
              vector<vector<Card>>& playerRuns, bool& knocked) {
    
    print_delayed("\n========================================");
    print_delayed(playerName + "'s Turn");
    print_delayed("========================================");
    
    print_instant("\nCards remaining in stock: " + to_string(deck.remaining()));
    print_instant("Top of discard pile: ", false);
    cout << discardPile.top();
    
    print_instant("\n" + playerName + "'s hand:");
    display_hand(hand);
    
    // DRAW PHASE
    print_instant("\nChoose an action:");
    print_instant("1. Draw from stock pile");
    print_instant("2. Draw from discard pile");
    
    int choice = get_valid_input("Your choice: ", 1, 2);
    
    Card drawn;
    if (choice == 1) {
        if (deck.isEmpty()) {
            print_delayed("Stock pile is empty! Drawing from discard instead.");
            drawn = discardPile.top();
            discardPile.pop();
        } else {
            drawn = deck.deal_card();
            print_delayed("You drew from stock: ", false);
            cout << drawn;
        }
    } else {
        // in case the discard pile has no cards, but it should always have >=1
        if (discardPile.empty()) {
            print_delayed("Discard pile is empty! Drawing from stock instead.");
            drawn = deck.deal_card();
        } else {
            drawn = discardPile.top();
            discardPile.pop();
            print_delayed("You took from discard: ", false);
            cout << drawn;
        }
    }

    // pick up card
    hand.push_back(drawn);

    print_delayed("\nUpdated hand:");
    display_hand(hand);
    
    // Find melds before discard
    playerSets = find_sets(hand);
    playerRuns = find_runs(hand);
    display_melds(playerSets, playerRuns);

    // DISCARD PHASE
    int discardChoice = get_valid_input("\nWhich card to discard (1-" + 
                                        to_string(hand.size()) + ")? ", 1, hand.size());

    Card discarded = hand[discardChoice - 1];
    hand.erase(hand.begin() + discardChoice - 1);
    discardPile.push(discarded);
    
    print_delayed("You discarded: ", false);
    cout << discarded;

    // Recalculate melds after discard
    playerSets = find_sets(hand);
    playerRuns = find_runs(hand);
    
    int deadwood = calculate_deadwood(hand, playerSets, playerRuns);
    print_delayed("\nYour deadwood: " + to_string(deadwood) + " points");

    // KNOCK CHECK
    if (deadwood == 0) {
        print_delayed("\n" + playerName + " has GIN! ");
        knocked = true;
    } else if (deadwood <= 10) {
        print_delayed("\n" + playerName + ", you can knock (deadwood = " + 
                     to_string(deadwood) + ")");
        int knockChoice = get_valid_input("Do you want to knock? (1=Yes, 2=No): ", 1, 2);
        
        if (knockChoice == 1) {
            print_delayed("\n" + playerName + " knocks!");
            knocked = true;
        } else {
            print_delayed(playerName + " chooses to continue playing.");
        }
    }
}

/*
    Here, the main gameplay logic happens
*/
int main(int argc, const char * argv[]) {
    print_delayed("=== GIN RUMMY ===\n");
    
    string p1Name, p2Name;
    print_instant("Player 1 name: ", false);
    getline(cin, p1Name);
    print_instant("Player 2 name: ", false);
    getline(cin, p2Name);
    
    if (p1Name.empty()) p1Name = "Player 1";
    if (p2Name.empty()) p2Name = "Player 2";
    
    print_delayed("\nWelcome " + p1Name + " and " + p2Name + "!");
    print_delayed("First to 100 points wins the game.");
    print_delayed("Let's begin!\n");
    
    int p1Score = 0;
    int p2Score = 0;
    
    bool playAgain = true;
    
    while (playAgain) {
        // start new round
        print_delayed("\n\n========================================");
        print_delayed("        NEW ROUND");
        print_delayed("========================================");
        print_delayed("Current Scores: " + p1Name + " " + to_string(p1Score) + 
                     " - " + p2Name + " " + to_string(p2Score));
        
        // deal hands to each player etc
        Deck deck;
        vector<Card> p1Hand = deck.deal_hand(HAND_SIZE);
        vector<Card> p2Hand = deck.deal_hand(HAND_SIZE);
        vector<vector<Card>> p1Sets, p1Runs;
        vector<vector<Card>> p2Sets, p2Runs;
        
        if (p1Hand.empty() || p2Hand.empty()) {
            print_delayed("Error dealing cards. Exiting.");
            break;
        }
        
        stack<Card> discardPile;
        discardPile.push(deck.deal_card());
        
        print_delayed("\nStarting discard: ", false);
        cout << discardPile.top();
        
        bool knocked = false;
        bool p1Knocked = false;
        int turn = 0;
        
        // play until someone knocks or deck runs out
        while (!knocked && !deck.isEmpty()) {
            turn++;
            
            // player 1's turn
            take_turn(deck, p1Hand, p1Name, discardPile, p1Sets, p1Runs, knocked);
            if (knocked) {
                p1Knocked = true;
                
                // Update opponent's melds for scoring
                p2Sets = find_sets(p2Hand);
                p2Runs = find_runs(p2Hand);
                
                score_round(p1Name, p1Hand, p1Sets, p1Runs, p1Score,
                           p2Name, p2Hand, p2Sets, p2Runs, p2Score);
                break;
            }
            
            // player 2's turn
            take_turn(deck, p2Hand, p2Name, discardPile, p2Sets, p2Runs, knocked);
            if (knocked) {
                p1Knocked = false;
                
                // update opponent's melds for scoring
                p1Sets = find_sets(p1Hand);
                p1Runs = find_runs(p1Hand);
                
                score_round(p2Name, p2Hand, p2Sets, p2Runs, p2Score,
                           p1Name, p1Hand, p1Sets, p1Runs, p1Score);
                break;
            }
            
        }
        
        if (!knocked) {
            print_delayed("\n========== ROUND ENDS ==========");
            print_delayed("Deck is empty! Round ends in a draw (no points awarded).");
        }
        
        // Check if someone won the game (with 100 points)
        if (p1Score >= 100) {
            print_delayed("\n\n🏆🏆🏆 " + p1Name + " WINS THE GAME! 🏆🏆🏆");
            print_delayed("Final Score: " + p1Name + " " + to_string(p1Score) + 
                         " - " + p2Name + " " + to_string(p2Score));
            break;
        } else if (p2Score >= 100) {
            print_delayed("\n\n🏆🏆🏆 " + p2Name + " WINS THE GAME! 🏆🏆🏆");
            print_delayed("Final Score: " + p1Name + " " + to_string(p1Score) + 
                         " - " + p2Name + " " + to_string(p2Score));
            break;
        }
        
        // ask if players want to play another round
        int continueChoice = get_valid_input("\nPlay another round? (1=Yes, 2=No): ", 1, 2);
        
        if (continueChoice != 1) {
            playAgain = false;
            print_delayed("\n=== FINAL SCORES ===");
            print_delayed(p1Name + ": " + to_string(p1Score));
            print_delayed(p2Name + ": " + to_string(p2Score));
            
            if (p1Score > p2Score) {
                print_delayed("\n🏆 " + p1Name + " wins overall!");
            } else if (p2Score > p1Score) {
                print_delayed("\n🏆 " + p2Name + " wins overall!");
            } else {
                print_delayed("\n🤝 It's a tie!");
            }
        }
    }
    
    print_delayed("\nThanks for playing!");
    
    return 0;
}