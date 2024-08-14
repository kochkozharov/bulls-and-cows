#pragma once
#include <string>

struct GuessResult {
    GuessResult(int bulls, int cows) : bulls(bulls), cows(cows) {}
    int bulls;
    int cows;
};

struct Game {
    Game(int gameID, int maxSlots, int freeSlots)
        : gameID(gameID), maxSlots(maxSlots), freeSlots(freeSlots) {}
    int gameID;
    int maxSlots;
    int freeSlots;
};

struct ConnectionSlot {
    int guess;
    int pid;
};

bool operator==(const GuessResult &a, const GuessResult &b);
bool operator<(const Game &a, const Game &b);

GuessResult MakeGuess(int secret, int guess);
int GenMysteryNumber();