CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2

all: exam_system

exam_system: main.cpp exam_utils.h
	$(CXX) $(CXXFLAGS) -o $@ main.cpp

tests/unit_tests: tests/unit_tests.cpp exam_utils.h
	$(CXX) $(CXXFLAGS) -o $@ tests/unit_tests.cpp

unit: tests/unit_tests
	./tests/unit_tests

test: unit exam_system
	./tests/smoke_test.sh

clean:
	rm -f exam_system exam_system.exe tests/unit_tests tests/unit_tests.exe *.tmp

help:
	@echo "make / make all  - build ./exam_system"
	@echo "make unit        - run the helper unit tests"
	@echo "make test        - unit tests, then the end-to-end smoke test"
	@echo "make clean       - remove build outputs and temp files"
	@echo "make help        - show this list"

.PHONY: all unit test clean help
