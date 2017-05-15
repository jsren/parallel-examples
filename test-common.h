/* test-common.h - (c) 2017 James S Renwick */
#include "ostest/ostest.h"
#include <future>
#include <chrono>
#include <array>
#include <cstddef>


/* The base test suite shared by all tests. */
class PPLSSuite : public ::ostest::TestSuite
{
protected:
    // The default number of threads in the test
    static const constexpr std::size_t threadCount = 3;
    // Whether tests should print log messages
    static const constexpr bool verbose = true;
    // The number of iterations to run for applicable tests
    static const constexpr std::size_t iterations = 100000;
    // The default number of ms to wait for threads to finish
    static const constexpr std::size_t defaultTimeout = 10000;

    /* Function to start N number of threads executing Func.
       Returns a pointer to a std::array. User-delete.
    */
    template<std::size_t N, typename Func>
    static auto co(const Func& function)
    {
        auto threads = new std::array<std::future<decltype(function())>, N>();

        for (std::size_t i = 0; i < N; i++) {
            threads->operator[](i) = std::async(std::launch::async, function);
        }
        return threads;
    }

    /* Returns true if each thread executes within the timeout. 
       Re-throws any exceptions within the threads.
    */
    template<typename Auto>
    static bool wait_for_all(Auto& threads, std::size_t timeoutMs = defaultTimeout)
    {
        for (auto& future : *threads) 
        {
            if (future.wait_for(std::chrono::milliseconds(timeoutMs)) 
                == std::future_status::timeout) return false;
            future.get(); // Throw any exceptions
        }
        return true;
    }
};
