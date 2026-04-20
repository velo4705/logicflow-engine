# src/Makefile - Kernel Modules
CXX      := g++
CXXFLAGS := -O3 -mavx512f -march=native -fopenmp -std=c++17

all: master_scan hyperflow_engine manifold_mapper

master_scan: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o master_scan

hyperflow_engine: hyperflow.cpp
	$(CXX) $(CXXFLAGS) hyperflow.cpp -o hyperflow_engine

manifold_mapper: versal_mapper.cpp
	$(CXX) $(CXXFLAGS) versal_mapper.cpp -o manifold_mapper

clean:
	rm -f master_scan hyperflow_engine manifold_mapper