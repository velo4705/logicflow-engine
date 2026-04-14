# Master Scan Makefile - P vs NP Empirical Verification
# Targeted for AVX-512 capable architectures

# Compiler
CXX = g++

# Compiler Flags:
# -O3: Maximum optimization for performance
# -mavx512f: Enables AVX-512 Foundation instructions
# -march=native: Optimizes specifically for the host CPU (CMI/Google Test Machine)
# -std=c++17: Modern C++ standard required for std::aligned_alloc
CXXFLAGS = -O3 -mavx512f -march=native -std=c++17 -Wall -Wextra

# Target binary name
TARGET = master_scan

# Source files
SRC = main.cpp

# Default build rule
all: $(TARGET)

# Link and compile
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Run standard solve mode
run: $(TARGET)
	./$(TARGET)

# Run formal benchmarking mode
bench: $(TARGET)
	./$(TARGET) --bench

# Clean build artifacts
clean:
	rm -f $(TARGET)

.PHONY: all run bench clean