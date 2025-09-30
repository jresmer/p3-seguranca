#ifndef BIGINT_H
#define BIGINT_H

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

class BigInt {
public:
    BigInt(unsigned int bits);
    BigInt(const std::string& hex_str);
    BigInt(uint64_t value);

    void seed(uint64_t seed_val);

    BigInt& operator^=(const BigInt& other);
    BigInt& operator<<=(size_t shift);
    BigInt& operator>>=(size_t shift);

    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator*(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator%(const BigInt& other) const;

    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;
    bool operator<(const BigInt& other) const;
    bool operator>(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator>=(const BigInt& other) const;

    bool is_zero() const;
    bool is_even() const;
    void set_bit(size_t n, bool value);
    bool get_bit(size_t n) const;
    size_t bit_length() const;

    static BigInt modular_pow(BigInt base, BigInt exponent, const BigInt& modulus);

    std::string to_hex_string() const;
    std::string to_binary_string() const;
    void set_limbs(const std::vector<uint64_t>& new_limbs);

private:
    std::vector<uint64_t> limbs;
    size_t num_bits;

    void trim();
};

#endif // BIGINT_H

