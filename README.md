# Gin Rummy - A Terminal-Based Card Game

A two-player implementation of the classic card game Gin Rummy, built in C++ with a focus on clean code architecture and robust input validation.

## Table of Contents
- [Motivation](#motivation)
- [Game Rules](#game-rules)
- [Features](#features)
- [Technical Implementation](#technical-implementation)
- [How to Compile and Run](#how-to-compile-and-run)
- [How to Play](#how-to-play)
- [Notable Design Decisions](#notable-design-decisions)
- [Future (Potential) Improvements](#future-potential-improvements)

---

## Motivation

As someone with Asian descent, one of the most popular passtimes in families like ours is card games. In particular, variations of Gin Rummy! Given that and the fact that I had started developing a card system during my C++ labs at uni, this Hawkeye task was perfect for me!

---

## Game Rules

Gin Rummy is a two-player card game where the objective is to form melds (sets and runs) and minimize deadwood.

### Basic Rules
- **Players**: 2
- **Deck**: Standard 52-card deck
- **Deal**: 10 cards per player
- **Objective**: Form melds to reduce deadwood to ≤10 points, then knock

### Melds
- **Set**: 3 or 4 cards of the same rank with different suits
  - Example: 7♣ 7♦ 7♥
- **Run**: 3+ consecutive cards of the same suit
  - Example: 5♠ 6♠ 7♠ 8♠
  - Note: Ace is LOW only (A-2-3 valid, Q-K-A invalid)

### Turn Structure
1. **Draw**: Take a card from either the stock pile or discard pile
2. **Discard**: Place one card face-up on the discard pile
3. **Knock** (optional): If deadwood ≤10 points, you may knock to end the round

### Scoring
- **Deadwood Values**:
  - Ace = 1 point
  - 2-9 = face value
  - 10, J, Q, K = 10 points each

- **Knock Outcomes**:
  - **Normal Knock**: Score = Opponent's deadwood - Your deadwood
  - **Gin** (0 deadwood): Score = Opponent's deadwood + 25 bonus
  - **Undercut** (opponent has less deadwood): Opponent scores the difference + 25 bonus

- **Winning**: First player to reach 100 points wins the game

---

## Features

### Core Gameplay
- Full Gin Rummy rules implementation
- Two-player hot-seat gameplay
- Multiple rounds with persistent scoring
- Automatic meld detection (sets and runs)
- Deadwood calculation
- Knock, Gin, and Undercut mechanics

### User Experience
- Timed message printing for polished feel
- Clear visual separation between turns
- Meld display during gameplay
- Screen clearing between players (privacy)
- Comprehensive input validation

### Technical Features
- Efficient card shuffling algorithm (Fisher-Yates)
- Hash map-based meld detection
- Stack-based discard pile
- Robust error handling
- Consistent naming conventions (camelCase variables, snake_case functions)

---

## Technical Implementation

### Card Representation
Cards are represented using a compact bit-field struct. This made sense to me during my lab, given the way we classify cards (eg 10 of spades).
```cpp
typedef struct {
    uint8 suit : 4;  // 1-4 (Clubs, Diamonds, Hearts, Spades)
    uint8 rank : 4;  // 1-13 (Ace through King)
} Card;
```

### Deck Management
The `Deck` class implements:
- **Creation**: Generates all 52 cards (4 suits × 13 ranks)
- **Shuffling**: Fisher-Yates algorithm for unbiased randomization
- **Dealing**: Efficient card distribution with remaining count tracking

### Meld Detection Algorithm

#### Sets (Using Hash Maps)
```cpp
// Group cards by rank using unordered_map
unordered_map<uint8, vector<Card>> rankMap;

// For each rank with 3+ cards, verify different suits
set<uint8> suits;
if (suits.size() >= 3) {
    // Valid set found
}
```
**Time Complexity**: O(n) where n = hand size

#### Runs (Using Hash Maps + Sorting)
```cpp
// Group cards by suit using unordered_map
unordered_map<uint8, vector<Card>> suitMap;

// Sort each suit's cards by rank
sort(cards.begin(), cards.end(), [](Card a, Card b) {
    return a.rank < b.rank;
});

// Find consecutive sequences
if (cards[i].rank == currentRun.back().rank + 1) {
    currentRun.push_back(cards[i]);
}
```
**Time Complexity**: O(n log n) for sorting within each suit

### Deadwood Calculation
1. Collect all cards in melds using `set<pair<suit, rank>>` for O(1) lookup
2. Iterate through hand, sum values of cards NOT in melds
3. Apply point values: Ace=1, 2-9=face value, 10/J/Q/K=10

### Input Validation
Robust input handling with `get_valid_input()`:
- Detects non-numeric input (`cin.fail()`)
- Validates range
- Loops until valid input received

---

## How to Play

1. **Start the game**: Enter names for both players
2. **Each turn**:
   - View your hand and current melds
   - Choose to draw from stock or discard pile
   - View updated hand with new card
   - Select a card to discard (1-11)
   - See your deadwood score
3. **Knocking**: If deadwood ≤10, you'll be prompted to knock
4. **Scoring**: Round ends when someone knocks; points are awarded
5. **Winning**: First to 100 points wins the game

---

## Notable Design Decisions

### 1. **Multiplayer Instead of AI**
**Decision**: Implemented two-player human vs. human gameplay with screen clearing between turns.

**Rationale**: 
- An effective AI opponent would require 2-3 hours of development alone
- Focus on core data structures (hash maps) and algorithms (meld detection)
- More time available for polish and edge case handling

---

### 2. **Hash Maps for Meld Detection**
**Decision**: Used `unordered_map<uint8, vector<Card>>` to group cards by rank/suit.

**Rationale**:
- O(1) average-case grouping operation
- It was a natural way to organize cards by shared properties
- Makes meld detection algorithm clear and maintainable

---

### 3. **Stack for Discard Pile**
**Decision**: Used `std::stack<Card>` for the discard pile instead of `std::vector`.

**Rationale**:
- Discard pile is just like a stack (LIFO behavior) - we only need access to top card
- Prevents accidental access to middle of pile

---

### 4. **Separation of Meld Display from Detection**
**Decision**: `find_sets()` and `find_runs()` return data without printing; separate `display_melds()` handles output.

**Rationale**:
- Separation of concerns (logic vs. presentation) - functions are reusable in different contexts
- Easier to test (can verify melds without capturing output)
- Could add GUI later without changing core logic

---


### 5. **Timed Printing for Polish**
**Decision**: Added optional delays between game messages using `<thread>` and `<chrono>`.

**Rationale**:
- Makes game feel more polished and professional
- Game was difficult to follow along otherwise
- Easy to disable for testing (`ENABLE_DELAYS = false`)

**Trade-off**: Requires C++11 threading support, but adds minimal complexity.

---

### 6. **Input Validation with Loops**
**Decision**: Comprehensive input validation that loops until valid input received.

**Rationale**:
- Prevents crashes from invalid input
- Better user experience than program termination
- Shows defensive programming practices
- Handles edge cases (non-numeric input, out of range, empty input)

**Implementation**: Separate `get_valid_input()` function for reusability across all prompts.

---

## Future (Potential) Improvements

### Gameplay Enhancements
- **Undo last move**: Allow players to take back their most recent discard
- **Game statistics**: Track wins, average deadwood, fastest gin, etc.
- **Save/load game**: Serialize game state to file for resuming later

### AI Implementation
- **Difficulty levels**: Easy/Medium/Hard selectable AI

### User Interface
- **ASCII card art**: Visual card representations with borders
- **Color-coded suits**: Red for hearts/diamonds, black for clubs/spades (ANSI colors)
- **GUI version**: Some graphical interface

### Technical Improvements
- **Unit tests**: Test suite for meld detection, scoring, edge cases
- **Configuration file**: JSON/YAML for game settings (delays, scoring rules)
- **Refactor to classes**: `Player` class, `GinRummyGame` class for better encapsulation. Had I had more time I would have done this for sure, by splitting code into something like `card_utils.h`, `game_io.h`, `gin_rummy.h`

### Analytics
- **Move suggestion system**: Hint feature showing optimal discard
- **Card probability display**: Show odds of drawing needed cards
- **ELO rating system**: Track player skill over multiple games