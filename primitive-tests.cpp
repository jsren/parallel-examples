/* primitive-tests.cpp - (c) 2017 James S Renwick */
#include "test-common.h"
#include "ostest/ostest.h"

#include <atomic>
#include <iostream>


class PrimitiveSuite : public PPLSSuite { };


class sem
{
private:
    int s = 0;
    std::mutex mutex{};

public:
    sem() = default;
    sem(const sem&) = delete; // No copying or moving
    inline sem(int value) : s(value) { }

public:
    inline void up()
    {
        std::unique_lock<std::mutex> lock(mutex);
        s = s + 1;
    }

    inline bool down()
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (s > 0) { s -= 1; return true; }
        else return false;
    }
};



inline void P(sem& s) {
    while (!s.down()) { }
}

inline void V(sem& s) {
    s.up();
}


// ======================================================================

TEST(PrimitiveSuite, SemaphoreTest)
{
    sem mutex{1};
    std::atomic<int> testCounter{0};

    auto* threads = co<threadCount>([&]
    {
        if (verbose) std::cout << "[INFO] Thread started.\n";

        for (std::size_t i = 0; i < iterations; i++)
        {
            P(mutex);
            // Critical section

            testCounter++;
            ASSERT(testCounter == 1);
            testCounter--;     

            V(mutex);
            // Non-critical section
        }
        if (verbose) std::cout << "[INFO] Thread released.\n";
    });

    // Check that threads complete in good time & without exceptions
    ASSERT(wait_for_all(threads));

    // Only delete threads if successful (this prevents blocking indefinitely)
    delete threads;
}


TEST(PrimitiveSuite, BoundedBufferTest)
{
    static constexpr auto n = 3;

    int buf[n];
    int front = 0, rear = 0;
    sem empty{n}, full{0}, mutexP{1}, mutexC{1};

    auto* producers = co<threadCount>([&]
    {
        if (verbose) std::cout << "[INFO] Producer thread started.\n";

        for (std::size_t i = 0; i < iterations; i++)
        {
            int data = i % n; // Produce new data locally

            P(empty);
              P(mutexP); buf[rear] = data; rear = (rear + 1) % n; V(mutexP);
            V(full);
        }
        if (verbose) std::cout << "[INFO] Producer thread released.\n";
    });

    auto* consumers = co<threadCount>([&]
    {
        if (verbose) std::cout << "[INFO] Consumer thread started.\n";

        for (std::size_t i = 0; i < iterations; i++)
        {
            P(full);
              P(mutexC); volatile int result = buf[front]; front = (front + 1) % n; V(mutexC);
            V(empty);
            //std::cout << result << "\n";
        }
        if (verbose) std::cout << "[INFO] Consumer thread released.\n";
    });

    // Check that threads complete in good time & without exceptions
    ASSERT(wait_for_all(producers));
    ASSERT(wait_for_all(consumers));

    // Only delete threads if successful (this prevents blocking indefinitely)
    delete producers;
    delete consumers;
}


