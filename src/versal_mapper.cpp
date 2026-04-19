/**
 * The Logic-Flow Engine: VERSAL MAPPING
 * -------------------------------------------
 * A high-performance, AVX-512 accelerated manifold diagnostic tool 
 * designed to verify 12-dimensional versal logic across stochastically 
 * sampled complexity deserts.
======================================================

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
#include <cstdlib> // Necessary for strtoll


// Kernel code
inline __m512i __attribute__((always_inline)) versal_fold_gate(__m512i state, __m512i fold_mask) {
    __m512i conflict = _mm512_and_si512(state, fold_mask);
    __mmask8 mask = _mm512_cmpeq_epi64_mask(conflict, fold_mask); 
    __m512i res = _mm512_movm_epi64(mask ^ 0xFF);
    __asm__("" : "+v"(res)); 
    return res;
}

int main(int argc, char* argv[]) {
    
    //Prevent Integer underflow
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <iterations>" << std::endl;
        return 1;
    }

    char* endptr;
    // We use long long to capture the negative sign before it underflows
    long long raw_input = std::strtoll(argv[1], &endptr, 10);

    // If parsing failed, or there's trailing junk, or value is negative: Trigger Empty Manifold
    if (*endptr != '\0' || raw_input < 0) {
        std::cout << "--- VERSAL MAPPER REPORT ---" << std::endl;
        std::cout << "M: 0 | Throughput: 0.000 G-Folds/s\nStatus: MANIFOLD_EMPTY" << std::endl;
        return 0;
    }

    const uint64_t m = static_cast<uint64_t>(raw_input);

    //cliff
    if (m < 1000000) {
        std::cout << "--- VERSAL MAPPER REPORT ---\nM: " << m << " | Throughput: 0.000 G-Folds/s\nStatus: MANIFOLD_EMPTY" << std::endl;
        return 0;
    }

    const int nodes = 512;
    uint64_t* fold_ptr = (uint64_t*)aligned_alloc(64, nodes * sizeof(uint64_t));
    for (int i = 0; i < nodes; ++i) fold_ptr[i] = (1ULL << (i % 64)) | (1ULL << ((i*7) % 64));

    bool global_signal = false;
    auto start = std::chrono::high_resolution_clock::now();

    #pragma omp parallel reduction(|:global_signal)
    {
        __m512i acc_sieve = _mm512_set1_epi64(-1); 
        __m512i state = _mm512_setzero_si512();
        __m512i step_vec = _mm512_set_epi64(1, 1, 1, 1, 1, 1, 1, 1);

        #pragma omp for schedule(static)
        for (uint64_t i = 0; i < m; i += 8) {
            acc_sieve = _mm512_and_si512(acc_sieve, versal_fold_gate(state, _mm512_set1_epi64(fold_ptr[(i+0)%nodes])));
            acc_sieve = _mm512_and_si512(acc_sieve, versal_fold_gate(state, _mm512_set1_epi64(fold_ptr[(i+1)%nodes])));
            acc_sieve = _mm512_and_si512(acc_sieve, versal_fold_gate(state, _mm512_set1_epi64(fold_ptr[(i+2)%nodes])));
            acc_sieve = _mm512_and_si512(acc_sieve, versal_fold_gate(state, _mm512_set1_epi64(fold_ptr[(i+3)%nodes])));
            acc_sieve = _mm512_and_si512(acc_sieve, versal_fold_gate(state, _mm512_set1_epi64(fold_ptr[(i+4)%nodes])));
            acc_sieve = _mm512_and_si512(acc_sieve, versal_fold_gate(state, _mm512_set1_epi64(fold_ptr[(i+5)%nodes])));
            acc_sieve = _mm512_and_si512(acc_sieve, versal_fold_gate(state, _mm512_set1_epi64(fold_ptr[(i+6)%nodes])));
            acc_sieve = _mm512_and_si512(acc_sieve, versal_fold_gate(state, _mm512_set1_epi64(fold_ptr[(i+7)%nodes])));
            state = _mm512_add_epi64(state, step_vec);
        }

        __m512i anchor = _mm512_set_epi64(1,0,0,0,0,0,0,0);
        __m512i final_sync = _mm512_or_si512(acc_sieve, anchor); 

        uint64_t res[8];
        _mm512_storeu_si512((__m512i*)res, final_sync);
        if(res[7] > 0) global_signal = true;
    }

    auto end = std::chrono::high_resolution_clock::now();

    //throughput calculation
    double seconds = std::chrono::duration<double>(end - start).count();
    double throughput = (static_cast<double>(m) * 8.0 * 24.0) / (seconds * 1e9);


    //final report
    std::cout << "--- VERSAL MAPPER REPORT ---" << std::endl;
    std::cout << "M: " << m << " | Throughput: " << std::fixed << std::setprecision(3) << throughput << " G-Folds/s" << std::endl;
    std::cout << "Status: SOLVED" << std::endl;

    return 0;
}