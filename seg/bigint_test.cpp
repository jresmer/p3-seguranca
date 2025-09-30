#include <iostream>
#include <cassert>
#include "bigint.h"

void test_arithmetic_operators() {
    std::cout << "Running arithmetic operator tests..." << std::endl;
    BigInt a(static_cast<uint64_t>(100));
    BigInt b(static_cast<uint64_t>(50));

    // Test addition
    BigInt sum = a + b;
    assert(sum == BigInt(static_cast<uint64_t>(150)));

    // Test subtraction
    BigInt diff = a - b;
    assert(diff == BigInt(static_cast<uint64_t>(50)));

    // Test multiplication
    BigInt prod = a * b;
    assert(prod == BigInt(static_cast<uint64_t>(5000)));

    // Test division
    BigInt quot = a / b;
    assert(quot == BigInt(static_cast<uint64_t>(2)));

    // Test modulus
    BigInt rem = a % b;
    assert(rem == BigInt(static_cast<uint64_t>(0)));

    BigInt c(static_cast<uint64_t>(57));
    BigInt d(static_cast<uint64_t>(10));
    BigInt quot2 = c / d;
    BigInt rem2 = c % d;
    assert(quot2 == BigInt(static_cast<uint64_t>(5)));
    assert(rem2 == BigInt(static_cast<uint64_t>(7)));

    std::cout << "Arithmetic operator tests passed!" << std::endl;
}

void test_comparison_operators() {
    std::cout << "Running comparison operator tests..." << std::endl;
    BigInt a(static_cast<uint64_t>(100));
    BigInt b(static_cast<uint64_t>(50));
    BigInt c(static_cast<uint64_t>(100));

    // Test equality
    assert(a == c);
    assert(!(a == b));

    // Test inequality
    assert(a != b);
    assert(!(a != c));

    // Test less than
    assert(b < a);
    assert(!(a < b));

    // Test greater than
    assert(a > b);
    assert(!(b > a));

    // Test less than or equal to
    assert(b <= a);
    assert(a <= c);
    assert(!(a <= b));

    // Test greater than or equal to
    assert(a >= b);
    assert(a >= c);
    assert(!(b >= a));

    std::cout << "Comparison operator tests passed!" << std::endl;
}

void test_bitwise_operators() {
    std::cout << "Running bitwise operator tests..." << std::endl;
    
    // Test XOR assignment
    BigInt a(static_cast<uint64_t>(0b1100));
    BigInt b(static_cast<uint64_t>(0b1010));
    a ^= b;
    assert(a == BigInt(static_cast<uint64_t>(0b0110)));

    // Test left shift assignment
    BigInt c(static_cast<uint64_t>(5)); // 0101
    c <<= 2;
    assert(c == BigInt(static_cast<uint64_t>(20))); // 10100

    // Test right shift assignment
    BigInt d(static_cast<uint64_t>(20)); // 10100
    d >>= 2;
    assert(d == BigInt(static_cast<uint64_t>(5))); // 0101

    std::cout << "Bitwise operator tests passed!" << std::endl;
}

int main() {
    test_arithmetic_operators();
    test_comparison_operators();
    test_bitwise_operators();

    std::cout << "All BigInt tests passed!" << std::endl;

    return 0;
}
