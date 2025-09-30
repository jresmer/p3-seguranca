#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <chrono>
#include <map>
#include <cstdlib> // For std::atoi
#include "bigint.h"
#include "xorshift.h"

void xorshift(BigInt& state, int a, int b, int c) {
    BigInt temp = state;
    temp >>= a;
    state ^= temp;

    temp = state;
    temp <<= b;
    state ^= temp;

    temp = state;
    temp >>= c;
    state ^= temp;
}

/**
 * @brief Generates a pseudo-random large integer using the Xorshift algorithm.
 * 
 * @param bits The desired number of bits for the random number. Must be one of the supported sizes.
 * @return A BigInt representing the generated pseudo-random number.
 */
BigInt generate_random(int bits, std::chrono::duration<double, std::milli> &duration) {
    std::map<int, std::vector<int>> shift_params = {
        {40, {13, 7, 17}},
        {56, {23, 18, 5}},
        {80, {21, 35, 4}},
        {128, {23, 17, 26}},
        {168, {41, 23, 19}},
        {224, {31, 45, 110}},
        {256, {55, 119, 67}},
        {512, {127, 251, 111}},
        {1024, {257, 503, 127}},
        {2048, {513, 1021, 255}},
        {4096, {1025, 2011, 511}}
    };

    if (shift_params.find(bits) == shift_params.end()) {
        throw std::invalid_argument("Invalid bit size selected.");
    }

    BigInt state(static_cast<unsigned int>(bits));
    state.seed(time(0));

    std::cout << "Generating a " << bits << "-bit random number..." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    
    xorshift(state, shift_params[bits][0], shift_params[bits][1], shift_params[bits][2]);

    auto end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    
    std::cout << "Time to generate: " << duration.count() << " ms" << std::endl;

    return state;
}

/*
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <bits>" << std::endl;
        std::cerr << "Available bit sizes: 40, 56, 80, 128, 168, 224, 256, 512, 1024, 2048, 4096" << std::endl;
        return 1;
    }

    int bits = std::atoi(argv[1]);

    try {
        float total_duration = 0;
        for (int i = 0; i < 1000; i++) {
            std::chrono::duration<double, std::milli> duration;
            BigInt random_number = generate_random(bits, duration);
            total_duration += duration.count();
        }
        total_duration = total_duration / 1000;

        std::cout << "Total duration: " << total_duration << " ms" << std::endl;
        // std::cout << "Generated number (binary): " << random_number.to_binary_string() << std::endl;
        // std::cout << "Generated number (hex):    " << random_number.to_hex_string() << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
*/
