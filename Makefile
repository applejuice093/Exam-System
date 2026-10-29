CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2

exam_system: main.cpp exam_utils.h
	$(CXX) $(CXXFLAGS) -o $@ main.cpp

tests/unit_tests: tests/unit_tests.cpp exam_utils.h
	$(CXX) $(CXXFLAGS) -o $@ tests/unit_tests.cpp

unit: tests/unit_tests
	./tests/unit_tests

test: unit exam_system
	./tests/smoke_test.sh

clean:
	rm -f exam_system exam_system.exe tests/unit_tests tests/unit_tests.exe

.PHONY: unit test clean
