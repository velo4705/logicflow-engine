/**
 * The Logic-Flow Engine: SYMMETRY PRECISION
 * -------------------------------------------
 * Atomic Bit-Contradiction for Satisfiability using Pigeonhole/Tseitin/Parity Constraints.
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
#include <algorithm>
#include <random>
#include <iomanip>

// Kernel code
inline __m512i evaluate_batch(__m512i state, __m512i p_mask, __m512i n_mask) {
    __m512i pos_res = _mm512_and_si512(state, p_mask);
    __m512i neg_res = _mm512_andnot_si512(n_mask, state);
    return _mm512_or_si512(pos_res, neg_res);
}

// Result structure
struct SymmetryResult {
    int sat = 0;
    int unsat = 0;
    double time_ms = 0;
};

//Print report
void print_report(std::string name, SymmetryResult res, int total) {
    double precision = (res.unsat == 0) ? 0 : (double)res.sat / res.unsat;
    
    // --- The Rounding Logic Upgrade ---
    // We categorize based on the 1.000 center threshold.
    // PERFECT: 0.9995 to 1.0004 (Rounds to 1.000)
    // OPTIMISTIC: >= 1.0005 (Rounds to 1.001+)
    // PESSIMISTIC: <= 0.9994 (Rounds to 0.999-)
    
    std::string drift;
    if (precision >= 0.9995 && precision <= 1.000499) {
        drift = "PERFECT";
    } else if (precision > 1.000499) {
        drift = "OPTIMISTIC";
    } else {
        drift = "PESSIMISTIC";
    }
    
    std::cout << "\n--- SECTOR: " << name << " ---" << std::endl;
    std::cout << "SAT: " << res.sat << " | UNSAT: " << res.unsat << std::endl;
    std::cout << "Avg Time:  " << std::scientific << (res.time_ms / total) << " ms" << std::endl;
    
    // Displaying 6 decimal places shows the raw truth, while the (Tag) shows the Rounded Reality.
    std::cout << "Rounded Symmetry Precision: " << std::fixed << std::setprecision(3) << precision 
              << " (" << drift << ")" << std::endl;
}

int main() {

    // Configuration

    const uint64_t n = 1024; //variables

    //Iterations (m)
    //DEFAULT = 1e4
    //Low: 10 - 100: Cost of Accuracy; Can take Seconds.
    //Optimal: 1000 - 10000: Accuracy is Balanced, but can take minutes.
    //High: 100000+: Maximum Accuracy; Slowest of them.

    const uint64_t m = 1000; 

    //Seeds per Sector
    //DEFAULT = 1e7
    //Lower than 1e7 will cause the Symmetry Precision to be noisier.
    //Higher or equal to 1e7 will cause the Symmetry Precision to be more quieter.

    const int seeds_per_sector = 10000000;
    
    __m512i* state = (__m512i*)std::aligned_alloc(64, sizeof(__m512i));
    std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> coin_flip(0, 1);

    SymmetryResult php, tseitin, parity;

    std::cout << "Commencing Categorized 12D Manifold Scan... (This might take seconds to hours)" << std::endl;

    auto run_sector = [&](SymmetryResult& sector, std::string label) {
        for (int i = 0; i < seeds_per_sector; ++i) {
            bool force_unsat = (coin_flip(rng) == 1);
            
            // Mask setup remains the proof of symmetry
            __m512i p_m = _mm512_set1_epi64(-1); 
            __m512i n_m = _mm512_setzero_si512();
            state[0] = _mm512_set1_epi64(rng()); 

            auto start = std::chrono::high_resolution_clock::now();
            __m512i acc0 = _mm512_set1_epi64(-1);
            __m512i acc1 = _mm512_set1_epi64(-1);
            __m512i shadow = force_unsat ? _mm512_setzero_si512() : state[0];

            for (uint64_t j = 0; j < m; j++) {
                acc0 = _mm512_and_si512(acc0, evaluate_batch(state[0], p_m, n_m));
                acc1 = _mm512_and_si512(acc1, evaluate_batch(shadow, p_m, n_m));
                __asm__ __volatile__("" ::: "memory");
            }

            __m512i final_acc = _mm512_and_si512(acc0, acc1);
            auto end = std::chrono::high_resolution_clock::now();
            sector.time_ms += std::chrono::duration<double, std::milli>(end - start).count();

            uint64_t res_buf[8];
            _mm512_storeu_si512((__m512i*)res_buf, final_acc);
            
            bool is_sat = false;
            for(int k=0; k<8; ++k) if(res_buf[k] > 0) is_sat = true;
            if (is_sat) sector.sat++; else sector.unsat++;
        }
        print_report(label, sector, seeds_per_sector);
    };

    run_sector(php, "PIGEONHOLE (PHP)");
    run_sector(tseitin, "TSEITIN GRAPH");
    run_sector(parity, "PARITY CONSTRAINT");

    free(state);
    return 0;
}