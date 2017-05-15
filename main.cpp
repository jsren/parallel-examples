/* main.cpp - (c) 2017 James S Renwick */
#include "ostest/ostest.h"
#include <cstring>
#include <stdio.h>


using namespace ostest;

int main()
{
    // Print application header
    printf("\n");
    printf("PPLS Example Code\n");
    printf("--------------------------------------\n");
    printf("(c) 2017 James S Renwick\n");
    printf("\n");

    // Gets all available unit tests
    auto tests = ostest::getUnitTests();

    // Creates a test runner to run the test.
    // Results are reported in 'handleTestComplete.
    while (tests.next()) {
        auto result = TestRunner(tests.current()).run();
    }
    return 0;
}


// Called when a test has completed.
void ostest::handleTestComplete(const TestInfo& test, const TestResult& result)
{
    static const char* passStr = "PASS";
    static const char* failStr = "FAIL";

    // Print test result
    printf("[%s] [%s::%s] at %s:%i\n", result ? passStr : failStr, test.suiteName,
        test.testName, test.file, test.line);


    auto first = result.getFirstFailure();
    auto final = result.getFinalFailure();

    // Log test assertions & their results
    auto assertions = result.getAssertions();
    while (assertions.next())
    {
        const Assertion* assert = &assertions.current();

        if (assert == first && assert == final) printf("  ONLY: ");
        else if (assert == first) printf(" FIRST: ");
        else if (assert == final) printf(" FINAL: ");
        else printf("\t");

        printf("[%s] [%s:%i] \"%s\"\n", assert->passed() ? passStr : failStr, assert->file, 
            assert->line, assert->expression);

        if (ostest::ostest_std_exceptions) {
            if (std::strcmp(assert->expression, "<unhandled exception>") == 0) {
                printf("\t   %s\n", assert->getMessage());
            }
        }
    }

    // Newline separator
    printf("\n");
}
