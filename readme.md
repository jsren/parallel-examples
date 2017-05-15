## Introduction ##
This repository contains example C++ implementations of some of the algorithms and 
primitives described in the Parallel Programming Languages and Systems (PPLS) course.    

The examples are in the format of unit tests using the ostest testing framework. Options for controlling the tests can be found in `test-common.h`.    

All source code remains the property of the author.


### Files ###
 - **main.cpp** - Contains the entry point & test result printing code
 - **test-common.h** - Contains common utility functions & test configuration
 - **barrier-tests.cpp** - Contains example code for barrier synchronisation
 - **primitive-tests.cpp** - Contains example code for synchronisation primitives

## How to Run ##
```
make
./tests.exe
```
