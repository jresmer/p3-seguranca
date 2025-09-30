#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <functional>
#include <cassert>

#include "bigint.h"
#include "xorshift.h"
#include "fermat.h"
#include "miller-rabin.h"

/**
 * @brief Finds the next prime number starting from a given number.
 *
 * @param n The starting number.
 * @param k The number of rounds for the primality test.
 * @param prime_test A function pointer to the primality test to use.
 * @return The first prime number found at or after n.
 */
BigInt find_next_prime(BigInt n, int k, const std::function<bool(const BigInt&, int)>& prime_test) {
    if (n.is_even() && n != BigInt(uint64_t(2))) {
        n = n + BigInt(uint64_t(1));
    }
    while (!prime_test(n, k)) {
        n = n + BigInt(uint64_t(2));
        std::cout << "n: " << n.to_hex_string() << std::endl;
    }
    return n;
}

/**
 * @brief Tests the Fermat and Miller-Rabin primality testers with known 40-bit primes and composites.
 */
void test_primality_testers() {
    std::cout << "--------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "| Running primality tester validation                                                      |" << std::endl;
    std::cout << "--------------------------------------------------------------------------------------------" << std::endl;

    BigInt two_pow_40(1ULL);
    two_pow_40 <<= 40;

    std::vector<BigInt> primes = {
        two_pow_40 - BigInt(87ULL),
        two_pow_40 - BigInt(207ULL)
    };

    std::vector<BigInt> composites = {
        two_pow_40 - BigInt(1ULL),
        two_pow_40 - BigInt(2ULL)
    };

    int k = 10; // Number of rounds for primality tests

    for (const auto& p : primes) {
        std::cout << "Testing prime: " << p.to_hex_string() << std::endl;
        assert(is_prime_fermat(p, k));
        assert(is_prime_miller_rabin(p, k));
        std::cout << "  - PASSED" << std::endl;
    }

    for (const auto& c : composites) {
        std::cout << "Testing composite: " << c.to_hex_string() << std::endl;
        assert(!is_prime_fermat(c, k));
        assert(!is_prime_miller_rabin(c, k));
        std::cout << "  - PASSED" << std::endl;
    }
    std::cout << "All primality tests passed!" << std::endl;
}

int main() {
    test_primality_testers();

    std::vector<int> bit_sizes = {40, 56, 80, 128, 168, 224, 256};
    int k = 5; // Number of rounds for primality tests
    // , 512, 1024, 2048, 4096

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "--------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "| Bit Size | Fermat Time (ms) | Miller-Rabin Time (ms) | Difference (ms) |" << std::endl;
    std::cout << "--------------------------------------------------------------------------------------------" << std::endl;

    for (int bits : bit_sizes) {
        std::chrono::duration<double, std::milli> generation_duration;
        BigInt random_number = generate_random(bits, generation_duration);

        // --- Fermat Test ---
        auto start_fermat = std::chrono::high_resolution_clock::now();
        BigInt fermat_prime = find_next_prime(random_number, k, is_prime_fermat);
        auto end_fermat = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> fermat_duration = end_fermat - start_fermat;

        // --- Miller-Rabin Test ---
        auto start_miller = std::chrono::high_resolution_clock::now();
        BigInt miller_rabin_prime = find_next_prime(random_number, k, is_prime_miller_rabin);
        auto end_miller = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> miller_duration = end_miller - start_miller;
        
        double time_diff = fermat_duration.count() - miller_duration.count();

        std::cout << "| " << std::setw(8) << bits
                  << " | " << std::setw(16) << fermat_duration.count()
                  << " | " << std::setw(22) << miller_duration.count()
                  << " | " << std::setw(15) << time_diff << " |" << std::endl;
        
        std::cout << "| Found Fermat Prime: " << fermat_prime.to_hex_string() << std::endl;
        std::cout << "| Found Miller-Rabin Prime: " << miller_rabin_prime.to_hex_string() << std::endl;
        std::cout << "--------------------------------------------------------------------------------------------" << std::endl;
    }

    return 0;
}
