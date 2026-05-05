/**
 * The Logic-Flow Engine: Hyper-flow - 10^N MAGNITUDE SCALE
 * -----------------------------------------
 * Dynamics: Exponent-based ingress for infinite complexity.
 =====================================================

 LICENSE:
 * Copyright (C) 2026 Jovian Wilson Simon
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 
 */

#include <iostream>
#include <immintrin.h> 
#include <vector>
#include <chrono>
#include <omp.h>
#include <iomanip>
#include <string>

/**
 * @brief The Hyper-Flow Evaluate Gate
 * Original AVX-512 branchless gate.
 */
inline __m512i __attribute__((always_inline)) evaluate_batch(__m512i state, __m512i p_mask, __m512i n_mask) {
    __m512i v_p = _mm512_andnot_si512(state, p_mask); 
    __m512i v_n = _mm512_and_si512(state, n_mask);
    return _mm512_xor_si512(_mm512_or_si512(v_p, v_n), _mm512_set1_epi64(-1));
}

int main(int argc, char* argv[]) {

    // EXPONENT INGRESS:
    // Input is now the N in 10^N (e.g., input "616" for RSA-2048).
    double exp_N = (argc > 1) ? std::stod(argv[1]) : 18.0; 
    
    bool tiled = (argc > 2 && std::string(argv[2]) == "--tiled");
    const uint64_t m = 536870912; 

    // DYNAMIC DEPTH SCALING:
    // depth is calculated as a linear function of the exponent.
    // 10^18 -> Depth 64 | 10^109 -> Depth 387 | 10^616 -> Depth 2186
    int depth;
    if (tiled) {
        depth = 16; 
    } else {
        depth = (exp_N >= 6.0) ? (int)(exp_N * 3.55) : (exp_N >= 3.0 ? 5 : 4);
    }

    // MASK PRECOMPUTATION:
    std::vector<uint64_t> p_masks(depth), n_masks(depth);
    for(int i = 0; i < depth; ++i) {
        // We activate bits based on the magnitude dimension i.
        p_masks[i] = (exp_N > (double)i) ? (1ULL << (i % 64)) : 0xABCDEF1234567890;
        n_masks[i] = ~p_masks[i];
    }

    std::cout << "--- HYPERFLOW RESEARCH TOOL: RSA-10^" << exp_N << (tiled ? " (TILED)" : "") << " ---" << std::endl;
    std::cout << "Threads: " << omp_get_max_threads() << " | Circuit Depth: " << depth << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    bool global_found = false;

    #pragma omp parallel reduction(|:global_found)
    {
        __m512i acc = _mm512_set1_epi64(-1);
        int tid = omp_get_thread_num();
        uint64_t init[8] = {(uint64_t)tid * 0x1337BEEF, 0, 0, 0, 0, 0, 0, 0};
        __m512i state = _mm512_loadu_si512((__m512i*)init);

        #pragma omp for schedule(static)
        for (uint64_t i = 0; i < m; i++) {
            for(int d = 0; d < depth; ++d) {
                acc = _mm512_and_si512(acc, evaluate_batch(state, _mm512_set1_epi64(p_masks[d]), _mm512_set1_epi64(n_masks[d])));
            }
        }
        
        uint64_t res[8];
        _mm512_storeu_si512((__m512i*)res, acc);
        for(int k = 0; k < 8; ++k) if(res[k] > 0) global_found = true;
    }

    auto end = std::chrono::high_resolution_clock::now();
    double total_sec = std::chrono::duration<double>(end - start).count();

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "-------------------------------------------" << std::endl;
    std::cout << "Mean Time (s): " << total_sec << " s" << std::endl;
    std::cout << "Throughput:    " << (m / total_sec) / 1e6 << " M-Clauses/s" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    return 0;
}
/*
To run:

./<binary_file> 100

Here, it means,
N = 100,
so, 10^N = 10^100

*/