#include <ios>
#include <unistd.h>

#include <iostream>
#include <vector>

#include "shared_memory.h"
#include "utils.h"

class Guesser {
    private:
     WeakSharedMemory gameMemory;
     int conID = 0;
     bool connected = false;
    public:
     Guesser(WeakSharedMemory &gameMemory) : gameMemory(std::move(gameMemory)) {}
     void sendGuess(int guess) {
        auto *statusPtr = static_cast<int *>(gameMemory.getData());
        auto *gamePtr = reinterpret_cast<ConnectionSlot *>(statusPtr + 1);
        gameMemory.writeLock();
        while (!connected && gamePtr[conID].pid != 0) {
            ++conID;
        }
        connected = true;
        *statusPtr = conID;
        gamePtr[conID].pid = getpid();
        gamePtr[conID].guess = guess;
        gameMemory.readUnlock();
     }
     
};

int main() {
    WeakSharedMemory req(REQUEST_SLOT_NAME, sizeof(Request));
    WeakSharedMemory rep(RESPONSE_SLOT_NAME, sizeof(Response));
    auto *reqPtr = static_cast<Request *>(req.getData());
    auto *repPtr = static_cast<Response *>(rep.getData());
    int gameID;
    int maxSlots;
    std::string command;
    while (std::cin >> command) {
        if (command == "create") {
            std::cin >> maxSlots;
            req.writeLock();
            reqPtr->newGame = true;
            reqPtr->pid = getpid();
            reqPtr->maxSlots = maxSlots;
            req.readUnlock();
            
            if (!rep.readLock(true)) {
                exit(EXIT_SUCCESS);
            }
            gameID = repPtr->gameID;
            rep.writeUnlock();
        } else if (command == "connect") {
            req.writeLock();
            reqPtr->newGame = false;
            reqPtr->pid = getpid();
            req.readUnlock();
            if (!rep.readLock(true)) {
                exit(EXIT_SUCCESS);
            }
            gameID = repPtr->gameID;
            
            maxSlots = repPtr->maxSlots;
            std::cout << "????" << gameID << ' ' << maxSlots << '\n';
            rep.writeUnlock();
        } else if (command == "stop") {
            req.writeLock();
            reqPtr->pid = -1;
            req.readUnlock();
            exit(EXIT_SUCCESS);
        } else {
            std::cerr << "Unknown command\n";
            continue;
        }
        if (gameID != -1) {
            std::cout << "Connected to game " << gameID << '\n';
            break;
        }
        std::cerr << "No free games available. Try creating new\n";
    }

    WeakSharedMemory gameMemory(
        "BC" + std::to_string(gameID),
        sizeof(int) + maxSlots * sizeof(ConnectionSlot));
    auto *statusPtr = static_cast<int *>(gameMemory.getData());
    auto *gamePtr = reinterpret_cast<ConnectionSlot *>(statusPtr + 1);
    bool connect = false;
    int conID = 0;
    while (true) {
        int guess;
        std::cin >> guess;
        if (guess > 9999 || guess < -1) {
            std::cerr << "Incorrect format\n";
            continue;
        }
        std::cerr << std::boolalpha << !connect << ' ' << (gamePtr[conID].pid != 0);
        while (!connect && (gamePtr[conID].pid != 0)) {
            ++conID;
        }
        connect = true;
        gameMemory.writeLock();
        *statusPtr = conID;
        std::cerr << conID << "!!!!!!!!!\n";
        gamePtr[conID].pid = getpid();
        gamePtr[conID].guess = guess;
        gameMemory.readUnlock();
        std::cout << "gameptr " << maxSlots << ' ' << gameID << '\n';
        for ( int i = 0; i < maxSlots; ++i) {
            std::cout << gamePtr[i].pid << ' ' << gamePtr[i].guess << '\n';
        }
    }
}