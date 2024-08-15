#pragma once
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <string>
#include <string_view>

constexpr std::string_view REQUEST_SLOT_NAME = "/BCRequest";
constexpr std::string_view RESPONSE_SLOT_NAME = "/BCResponse";

struct Request {
    bool newGame;
    pid_t pid;
    int maxSlots;
};

struct Response {
    int gameID;
    int maxSlots;
};

class SharedMemoryException : public std::exception {
   public:
    SharedMemoryException(const std::string& message) noexcept
        : _msg(message) {}
    ~SharedMemoryException() noexcept override = default;
    [[nodiscard]] const char* what() const noexcept override {
        return _msg.c_str();
    }

   private:
    std::string _msg;
};

class WeakSharedMemory {
   public:
    enum {
        C_READ_ONLY = O_RDONLY,
        C_READ_WRITE = O_RDWR,
    } CREATE_MODE;

    enum {
        A_READ = PROT_READ,
        A_WRITE = PROT_WRITE,
    } ATTACH_MODE;
    WeakSharedMemory() = default;
    WeakSharedMemory(std::string_view name, std::size_t size);
    ~WeakSharedMemory();
    WeakSharedMemory(const WeakSharedMemory& other) = delete;
    WeakSharedMemory(WeakSharedMemory&& other) = default;
    WeakSharedMemory& operator=(const WeakSharedMemory& other) = delete;
    WeakSharedMemory& operator=(WeakSharedMemory&& other) = default;
    bool writeLock(bool timed = false);
    void writeUnlock();
    bool readLock(bool timed = false);
    void readUnlock();
    int getFD() const { return _FD; }
    void* getData() { return _ptr; };
    const void* getData() const { return _ptr; }
    const std::string& getName() const { return _name; }

   private:
    std::string _name;
    int _FD;
    sem_t* _wSemPtr;
    sem_t* _rSemPtr;
    size_t _size;
    void* _ptr;
};

class SharedMemory : public WeakSharedMemory {
   public:
    using WeakSharedMemory::WeakSharedMemory;
    ~SharedMemory();
};