# src/Makefile - Kernel Modules
CXX      := g++
CXXFLAGS := -O3 -march=x86-64-v4 -std=c++17 -fno-math-errno -fno-trapping-math -fno-signed-zeros -fopenmp -flto -fuse-linker-plugin -mprefer-vector-width=512 -fmove-loop-invariants -DNDEBUG -fprefetch-loop-arrays -fno-omit-frame-pointer -falign-functions=64 -falign-loops=32 -Wl,-O1,--sort-common,--as-needed

all: master_scan hyperflow_engine manifold_mapper hyperflow_magnitude

master_scan: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o master_scan

hyperflow_engine: hyperflow.cpp
	$(CXX) $(CXXFLAGS) hyperflow.cpp -o hyperflow_engine

hyperflow_magnitude: hyperflow_mag.cpp
	$(CXX) $(CXXFLAGS) hyperflow_mag.cpp -o hyperflow_magnitude

manifold_mapper: versal_mapper.cpp
	$(CXX) $(CXXFLAGS) versal_mapper.cpp -o manifold_mapper

clean:
	rm -f master_scan hyperflow_engine manifold_mapper hyperflow_magnitude