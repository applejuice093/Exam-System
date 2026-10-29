CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2

exam_system: main.cpp exam_utils.h
	$(CXX) $(CXXFLAGS) -o $@ $<

test: exam_system
	./tests/smoke_test.sh

clean:
	rm -f exam_system exam_system.exe

.PHONY: test clean
