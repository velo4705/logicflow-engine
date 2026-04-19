# --- COMPILER SETTINGS ---
CXX      := g++
CXXFLAGS := -O3 -mavx512f -march=native -fopenmp
LDFLAGS  := -fopenmp

# --- TARGETS ---
TARGET   := master_scan

# --- INDIVIDUAL BUILD RULES ---

# 1. Symmetry Module (main.cpp)
symmetry: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o master_scan
	@echo ">> SYMMETRY CORE COMPILED"

# 2. Hyperflow Module (hyperflow.cpp)
hyperflow: hyperflow.cpp
	$(CXX) $(CXXFLAGS) -c hyperflow.cpp -o master_scan
	@echo ">> HYPERFLOW ENGINE COMPILED"

# 3. Manifold Module (versal_mapper.cpp)
manifold: versal_mapper.cpp
	$(CXX) $(CXXFLAGS) -c versal_mapper.cpp -o master_scan
	@echo ">> MANIFOLD MAPPER COMPILED"


# --- UTILITIES ---

# Cleanup targets and objects
clean:
	rm -f $(TARGET)
	@echo "--- CLEAN COMPLETE ---"

.PHONY: symmetry hyperflow manifold run clean