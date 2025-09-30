#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <algorithm>
#include "bigint.h"

class CMWC {
public:
    /**
     * @brief Constructs a CMWC generator and seeds it.
     * @param seed The seed value for initialization.
     */
    CMWC(uint64_t seed);

    /**
     * @brief Generates the next 64-bit pseudo-random number.
     * @return A 64-bit pseudo-random number.
     */
    uint64_t next();

private:
    static const uint32_t R = 256;
    static const uint64_t A = 1234567890123456789ULL;

    std::vector<uint64_t> Q;
    uint64_t c;
    uint32_t i;
};

/**
 * @brief Constructs a CMWC generator and seeds it.
 *
 * The state array Q is seeded using a simple Linear Congruential Generator (LCG).
 * The initial carry `c` is also generated and must be less than the multiplier `A`.
 *
 * @param seed The seed value for initialization.
 */
CMWC::CMWC(uint64_t seed) : Q(R), c(0), i(R - 1) {
    uint64_t x = seed;
    for (uint32_t j = 0; j < R; ++j) {
        x = 6364136223846793005ULL * x + 1;
        Q[j] = x;
    }
    x = 6364136223846793005ULL * x + 1;
    c = x % A;
}

/**
 * @brief Generates the next 64-bit pseudo-random number using the CMWC algorithm.
 *
 * This method updates the generator's state and returns a new pseudo-random number.
 * The lag `R` must be a power of two for the index update `(i + 1) & (R - 1)` to work correctly.
 *
 * @return A 64-bit pseudo-random number.
 */
uint64_t CMWC::next() {
    i = (i + 1) & (R - 1);
    unsigned __int128 t = A * Q[i] + c;
    c = t >> 64;
    uint64_t x = t;
    Q[i] = x;
    return 0xFFFFFFFFFFFFFFFF - x;
}

const std::vector<int> supported_bits = {40, 56, 80, 128, 168, 224, 256, 512, 1024, 2048, 4096};

/**
 * @brief Checks if a given bit size is supported.
 * @param bits The bit size to check.
 * @return True if the bit size is supported, false otherwise.
 */
bool is_supported(int bits) {
    return std::find(supported_bits.begin(), supported_bits.end(), bits) != supported_bits.end();
}

/**
 * @brief Generates a pseudo-random large integer using the CMWC algorithm.
 *
 * @param bits The desired number of bits for the random number. Must be one of the supported sizes.
 * @param duration Reference to a duration object to store the generation time.
 * @return A BigInt representing the generated pseudo-random number.
 */
BigInt generate_random_cmwc(int bits, std::chrono::duration<double, std::milli> &duration) {
    if (!is_supported(bits)) {
        throw std::invalid_argument("Invalid bit size selected.");
    }

    auto start = std::chrono::high_resolution_clock::now();
    
    CMWC cmwc(time(0));
    BigInt result(bits);
    size_t num_limbs = (bits + 63) / 64;
    std::vector<uint64_t> limbs(num_limbs);
    for (size_t i = 0; i < num_limbs; ++i) {
        limbs[i] = cmwc.next();
    }
    result.set_limbs(limbs);
    
    auto end = std::chrono::high_resolution_clock::now();
    duration = end - start;

    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <bits>" << std::endl;
        std::cerr << "Available bit sizes: 40, 56, 80, 128, 168, 224, 256, 512, 1024, 2048, 4096" << std::endl;
        return 1;
    }

    int bits = std::atoi(argv[1]);

    try {
        std::cout << "Generating 1000 " << bits << "-bit random numbers using CMWC and calculating average time..." << std::endl;
        
        double total_duration = 0.0;
        const int iterations = 1000;

        for (int i = 0; i < iterations; ++i) {
            std::chrono::duration<double, std::milli> duration;
            generate_random_cmwc(bits, duration);
            total_duration += duration.count();
        }

        std::cout << "Average time to generate: " << total_duration / iterations << " ms" << std::endl;

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Available bit sizes: 40, 56, 80, 128, 168, 224, 256, 512, 1024, 2048, 4096" << std::endl;
        return 1;
    }

    return 0;
}
