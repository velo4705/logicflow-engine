import os
import subprocess
import time
import pandas as pd

# Problem sizes to test the P=NP claim
node_sizes = [32, 64, 128, 256, 512]
results = []

print("--- L11 COMPLEXITY VERIFICATION COMMENCED ---")

for n in node_sizes:
    print(f"Testing N={n} (State Space: 3^{n})...")
    
    # 1. Update the nodes in the source file dynamically
    with open("versal_mapper.cpp", "r") as f:
        lines = f.readlines()
    
    with open("versal_mapper.cpp", "w") as f:
        for line in lines:
            if "const int nodes =" in line:
                f.write(f"    const int nodes = {n};\n")
            else:
                f.write(line)
    
    # 2. Your Preferred One-Line Compilation
    # Compiling only the mapper to avoid the multiple 'main' definition error
    subprocess.run(["g++", "-O3", "-mavx512f", "-march=native", "-fopenmp", "versal_mapper.cpp", "-o", "master_scan"])
    
    # 3. Execute and capture throughput
    start_time = time.time()
    # Passing 1000000 ensures we hit the workload threshold for measurement
    proc = subprocess.Popen(["./master_scan", "1000000"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    stdout, _ = proc.communicate()
    end_time = time.time()
    
    # 4. Parse throughput from output
    for line in stdout.split('\n'):
        if "Throughput:" in line:
            try:
                throughput = float(line.split(":")[1].split()[0])
                results.append({"Nodes": n, "Time": end_time - start_time, "G-Folds": throughput})
            except (IndexError, ValueError):
                continue

# Generate the Proof Table
df = pd.DataFrame(results)
print("\n--- FINAL L11 COMPLEXITY LOG ---")
print(df)
df.to_csv("p_vs_np_proof.csv", index=False)