CXX ?= g++

TARGET ?= tests.exe

default:
	$(CXX) -std=c++14 -DOSTEST_STD_EXCEPTIONS -pthread -Wall -Wextra ostest/ostest.cpp *.cpp -o $(TARGET)
