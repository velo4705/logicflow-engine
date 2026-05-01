# Root Makefile - Logic-Flow Engine
CXX      = g++
# Explicitly defining flags for all targets
CXXFLAGS = -O3 -march=x86-64-v4 -std=c++17 -ffast-math -fopenmp -flto -fuse-linker-plugin -mprefer-vector-width=512 -funroll-loops -DNDEBUG -fno-plt -fprefetch-loop-arrays -fno-exceptions -fno-rtti -fomit-frame-pointer -falign-functions=64
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