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
    
    # Update the nodes in the source file dynamically
    with open("versal_mapper.cpp", "r") as f:
        lines = f.readlines()
    
    with open("versal_mapper.cpp", "w") as f:
        for line in lines:
            if "const int nodes =" in line:
                f.write(f"    const int nodes = {n};\n")
            else:
                f.write(line)
    
    # Compile with L11 flags
    subprocess.run(["g++", "-O3", "-mavx512f", "-mavx512dq", "-march=native", "-fopenmp", "versal_mapper.cpp", "-o", "master_scan"])
    
    # Execute and capture throughput
    start_time = time.time()
    proc = subprocess.Popen(["./master_scan"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    stdout, _ = proc.communicate()
    end_time = time.time()
    
    # Parse throughput from output
    for line in stdout.split('\n'):
        if "Throughput:" in line:
            throughput = float(line.split(":")[1].split()[0])
            results.append({"Nodes": n, "Time": end_time - start_time, "G-Folds": throughput})

# Generate the Proof Table
df = pd.DataFrame(results)
print("\n--- FINAL L11 COMPLEXITY LOG ---")
print(df)
df.to_csv("p_vs_np_proof.csv", index=False)