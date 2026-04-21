#!/bin/bash


# --- CONFIGURATION ---
BINARY="./src/master_scan"
DURATION_MINUTES=30
END_TIME=$(( $(date +%s) + (DURATION_MINUTES * 60) ))

# --- METRICS ---
TOTAL_RUNS=0
TOTAL_PESSIMISTIC=0
TOTAL_PERFECT=0
SUM_LATENCY=0  # In milliseconds

echo "--- BORG-MANIFOLD AUDIT v2.0: START ---"
echo "Config: Seeds=2^27 | M=128 | Target: GOLD (>99.99%)"
echo "------------------------------------------------------"

while [ $(date +%s) -lt $END_TIME ]; do
    ((TOTAL_RUNS++))
    
    # Capture start time in nanoseconds
    START_NS=$(date +%s%N)
    
    # Execute
    OUTPUT=$($BINARY 2>&1)
    
    # Capture end time
    END_NS=$(date +%s%N)
    
    # Calculate Latency for this run (ms)
    RUN_LATENCY=$(( (END_NS - START_NS) / 1000000 ))
    SUM_LATENCY=$(( SUM_LATENCY + RUN_LATENCY ))
    
    # Process Results
    RUN_PESSIMISTIC=$(echo "$OUTPUT" | grep -o "PESSIMISTIC" | wc -l)
    RUN_PERFECT=$(echo "$OUTPUT" | grep -o "PERFECT" | wc -l)
    ((TOTAL_PESSIMISTIC += RUN_PESSIMISTIC))
    ((TOTAL_PERFECT += RUN_PERFECT))

    # Real-time Telemetry
    STATUS="[✓] OK"
    if [ "$RUN_PESSIMISTIC" -gt 0 ]; then STATUS="[!] FAIL ($RUN_PESSIMISTIC Errors)"; fi
    
    printf "Run #%-4d | %-18s | Latency: %-6d ms\n" "$TOTAL_RUNS" "$STATUS" "$RUN_LATENCY"
done

# --- FINAL CALCULATIONS ---
TOTAL_SECTORS=$((TOTAL_PERFECT + TOTAL_PESSIMISTIC))
UPTIME_PCT=$(echo "scale=4; (($TOTAL_SECTORS - $TOTAL_PESSIMISTIC) / $TOTAL_SECTORS) * 100" | bc)
AVG_LATENCY=$(echo "scale=2; $SUM_LATENCY / $TOTAL_RUNS" | bc)

echo ""
echo "------------------------------------------------------"
echo "FINAL PERFORMANCE REPORT"
echo "------------------------------------------------------"
echo "Total Execution Cycles:  $TOTAL_RUNS"
echo "Total Sectors Scanned:   $TOTAL_SECTORS"
echo "Average Run Time:        $AVG_LATENCY ms"
echo "Symmetry Uptime:         $UPTIME_PCT %"
echo "------------------------------------------------------"

# --- L11 CLASSIFICATION ---
if (( $(echo "$UPTIME_PCT >= 99.99" | bc -l) )); then
    echo -e "RANK: \e[32mGOLD / PLATINUM\e[0m"
else
    echo -e "RANK: \e[31mFAILURE - ESCALATE SEEDS\e[0m"
fi
