# Makefile for Red Teamers

CXX = g++
CXXFLAGS = -O3 -mavx512f -march=native -fopenmp
TARGETS = master_scan red_verify red_main red_hyperflow

all: $(TARGETS)

red_verify: tests/red_versal_mapper.cpp
	$(CXX) $(CXXFLAGS) tests/red_versal_mapper.cpp -o red_verify

red_main: tests/red_main.cpp
	$(CXX) $(CXXFLAGS) tests/red_main.cpp -o red_main

red_rsa: tests/red_hyperflow.cpp
	$(CXX) $(CXXFLAGS) tests/red_hyperflow.cpp -o red_hyperflow

clean:
	rm -f $(TARGETS)