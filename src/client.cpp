#include <unistd.h>

#include <iostream>
#include <vector>

#include "shared_memory.h"
#include "utils.h"

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
    int conID = 0;
    bool connected = false;
    while (true) {
        int guess;
        std::cin >> guess;
        if (guess > 9999 || guess < -1) {
            std::cerr << "Incorrect format\n";
            continue;
        }
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
}