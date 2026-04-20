# Root Makefile - Logic-Flow Engine
CXX      = g++
# Explicitly defining flags for all targets
CXXFLAGS = -O3 -mavx512f -march=native -std=c++17 -fopenmp
TARGETS  = red_verify red_main red_hyperflow

all: build_src $(TARGETS)

# Triggers the src directory Makefile
build_src:
	$(MAKE) -C src

# IMPORTANT: You must include $(CXXFLAGS) in each rule
red_verify: tests/red_versal_mapper.cpp
	$(CXX) $(CXXFLAGS) tests/red_versal_mapper.cpp -o red_verify

red_main: tests/red_main.cpp
	$(CXX) $(CXXFLAGS) tests/red_main.cpp -o red_main

red_hyperflow: tests/red_hyperflow.cpp
	$(CXX) $(CXXFLAGS) tests/red_hyperflow.cpp -o red_hyperflow

clean:
	$(MAKE) -C src clean
	rm -f $(TARGETS)

.PHONY: all build_src clean