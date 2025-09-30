#include "bigint.h"
#include <random>
#include <iostream> // Added for main function
#include <cstdlib>  // Added for std::atoi
#include "fermat.h"

/**
 * @brief Performs the Fermat primality test on a BigInt.
 *
 * @param n The BigInt to test for primality. Must be greater than 2.
 * @param k The number of rounds of testing to perform. A higher value increases the accuracy.
 * @return true if n is likely prime, false otherwise.
 */
bool is_prime_fermat(const BigInt& n, int k) {
    if (n <= BigInt(uint64_t(1)) || n == BigInt(uint64_t(4))) return false;
    if (n <= BigInt(uint64_t(3))) return true;
    if (n.is_even()) return false;

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    for (int i = 0; i < k; i++) {
        BigInt a = BigInt(dis(gen)) % (n - BigInt(uint64_t(3))) + BigInt(uint64_t(2));
        if (BigInt::modular_pow(a, n - BigInt(uint64_t(1)), n) != BigInt(uint64_t(1))) {
            return false;
        }
    }

    return true;
}