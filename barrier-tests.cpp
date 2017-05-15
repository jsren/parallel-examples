/* barrier-tests.cpp - (c) 2017 James S Renwick */
#include "test-common.h"
#include "ostest/ostest.h"

#include <atomic>
#include <iostream>


class BarrierSuite : public PPLSSuite { };


TEST(BarrierSuite, CounterBarrierTest)
{
    std::atomic<int> count(0);

    auto* threads = co<threadCount>([&]
    {
        if (verbose) std::cout << "[INFO] Thread started.\n";

        // <count = count + 1;>
        count++;
        // <await (count == n); >
        while (count != threadCount) { }

        if (verbose) std::cout << "[INFO] Thread released.\n";
    });

    // Check that threads complete in good time & without exceptions
    ASSERT(wait_for_all(threads));

    // Only delete threads if successful (this prevents blocking indefinitely)
    delete threads;
}


TEST(BarrierSuite, SenseReversingBarrierTest)
{
    int count = 0;
    bool sense = false;
    std::mutex mutex{}; // Mutex for atomic section

    auto* threads = co<threadCount>([&]
    {
        if (verbose) std::cout << "[INFO] Thread started.\n";

        bool mySense = !sense;
        for (std::size_t i = 0; i < iterations; i++)
        {
            // <
            mutex.lock();
            // count = count + 1;
            count++;
            // if (count == n) { count = 0; sense = mySense; }
            if (count == threadCount) { count = 0; sense = mySense; }
            // >
            mutex.unlock();

            while (sense != mySense) { }
            mySense = !mySense;
        }
        if (verbose) std::cout << "[INFO] Thread released.\n";
    });

    // Check that threads complete in good time & without exceptions
    ASSERT(wait_for_all(threads));

    // Only delete threads if successful (this prevents blocking indefinitely)
    delete threads;
}


TEST(BarrierSuite, SymmetricBarrierTest)
{
    std::atomic<int> nextID(0);
    std::atomic<int> arrive[threadCount * 2]{};

    auto* threads = co<threadCount * 2>([&]
    {

        // Get this thread's ID and the ID of its friend thread
        int myid = nextID++;
        int friend_ = (myid % 2 ? myid - 1 : myid + 1);

        if (verbose) std::cout << "[INFO] Thread (" + std::to_string(myid) + 
            "/" + std::to_string(friend_) +  ") started.\n";

        
        for (std::size_t i = 0; i < iterations; i++)
        {
            // <await (arrive[myid] == 0);> 
            while (arrive[myid] != 0) { }
            arrive[myid] = 1;

            // <await (arrive[friend] == 1);> 
            while (arrive[friend_] != 1) { }
            arrive[friend_] = 0;
        }

        if (verbose) std::cout << "[INFO] Thread released.\n";
    });

    // Check that threads complete in good time & without exceptions
    ASSERT(wait_for_all(threads));

    // Only delete threads if successful (this prevents blocking indefinitely)
    delete threads;
}
