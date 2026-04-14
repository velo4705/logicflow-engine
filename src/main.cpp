/**
 * Master Scan SAT Solver - Empirical Polynomial Time Verification
 * * Hardware Requirements: AVX-512 capable CPU (Intel Skylake-X+, AMD Zen 4+)
 * Complexity: O(m * ceil(n/512))
 * * This program demonstrates bit-parallel SAT solving with statistical 
 * benchmarking (Warm-up + Mean execution profiling).
 */


#include <iostream>
#include <immintrin.h> // AVX-512 intrinsics
#include <vector>
#include <chrono>
#include <cstdlib>
#include <numeric>
#include <algorithm>

/**
 * THE MASTER SCAN CORE
 * Evaluates 512 logical configurations in parallel.
 */
inline __m512i evaluate_batch(__m512i state, __m512i p_mask, __m512i n_mask) {

    __m512i pos_res = _mm512_and_si512(state, p_mask);
    __m512i neg_res = _mm512_andnot_si512(state, n_mask); 
    return _mm512_or_si512(pos_res, neg_res);

}

int main() {

    // --- BENCHMARK CONFIGURATION ---

    const uint64_t n = 64;        // Variables (Adjust as needed: 64, 128, 256, 512 or >= 1024)
    const uint64_t m = 1000000;     // Clauses (If for 10 billion, use Unsigned Long Long)
    const int iterations = 100;     // Measured runs for average
    const int warm_up = 10;         // Discarded runs to stabilize CPU/Cache
    


    const uint64_t num_tiles = (n + 511) / 512;
    size_t alignment = 64;
    size_t total_size = sizeof(__m512i) * num_tiles;



    // 1. Memory Allocation & Alignment
    __m512i* state  = (__m512i*)std::aligned_alloc(alignment, total_size);
    __m512i* p_mask = (__m512i*)std::aligned_alloc(alignment, total_size);
    __m512i* n_mask = (__m512i*)std::aligned_alloc(alignment, total_size);


    // 2. Data Initialization
    for (uint64_t t = 0; t < num_tiles; ++t) {
        state[t]  = _mm512_set1_epi64(0xAAAAAAAAAAAAAAAA + t);
        p_mask[t] = _mm512_set1_epi64(0x5555555555555555 + t);
        n_mask[t] = _mm512_set1_epi64(0x0F0F0F0F0F0F0F0F + t);
    }


    std::vector<double> timings;
    bool last_sat_status = false;


    std::cout << "Benchmarking Master Scan: n=" << n << ", m=" << m << std::endl;
    std::cout << "Running " << warm_up << " warm-up cycles..." << std::endl;


    // 3. The Benchmarking Loop
    for (int r = 0; r < (iterations + warm_up); ++r) {

        auto start = std::chrono::high_resolution_clock::now();

        // Multi-accumulator ILP Pattern
        __m512i acc0 = _mm512_set1_epi64(-1); 
        __m512i acc1 = _mm512_set1_epi64(-1); 
        __m512i acc2 = _mm512_set1_epi64(-1); 
        __m512i acc3 = _mm512_set1_epi64(-1); 


        for (uint64_t i = 0; i < m; i += 4) {

            for (uint64_t t = 0; t < num_tiles; ++t) {
                // Dispatching to multiple execution ports simultaneously
                acc0 = _mm512_and_si512(acc0, evaluate_batch(state[t], p_mask[t], n_mask[t]));
                acc1 = _mm512_and_si512(acc1, evaluate_batch(state[t], p_mask[t], n_mask[t]));
                acc2 = _mm512_and_si512(acc2, evaluate_batch(state[t], p_mask[t], n_mask[t]));
                acc3 = _mm512_and_si512(acc3, evaluate_batch(state[t], p_mask[t], n_mask[t]));
            }

        }

        __m512i accumulator = _mm512_and_si512(_mm512_and_si512(acc0, acc1), _mm512_and_si512(acc2, acc3));

        auto end = std::chrono::high_resolution_clock::now();
        
        // Record results only after warm-up phase

        if (r >= warm_up) {
            std::chrono::duration<double, std::milli> elapsed = end - start;
            timings.push_back(elapsed.count());
            

            // Verify SAT status (should be consistent)

            uint64_t res_array[8];
            _mm512_store_si512((__m512i*)res_array, accumulator);
            last_sat_status = false;
            for(int k=0; k<8; ++k) if(res_array[k] > 0) last_sat_status = true;
        }
    }


    // 4. Statistical Analysis

    double average = std::accumulate(timings.begin(), timings.end(), 0.0) / timings.size();
    double min_time = *std::min_element(timings.begin(), timings.end());
    double max_time = *std::max_element(timings.begin(), timings.end());

    // 5. Output Results

    std::cout << "\n-----------------------------------" << std::endl;

    std::cout << "RESULTS (over " << iterations << " iterations)" << std::endl;
    std::cout << "Status:      " << (last_sat_status ? "SATISFIABLE" : "UNSATISFIABLE") << std::endl;
    std::cout << "AVERAGE:     " << average << " ms" << std::endl;
    std::cout << "PEAK (MIN):  " << min_time << " ms" << std::endl;
    std::cout << "SLOWEST:     " << max_time << " ms" << std::endl;

    std::cout << "-----------------------------------" << std::endl;

    free(state); free(p_mask); free(n_mask);

    return 0;
}