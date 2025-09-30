#include "bigint.h"
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cstdint> // Required for __int128

BigInt::BigInt(unsigned int bits) : num_bits(bits) {
    if (bits == 0) {
        throw std::invalid_argument("Number of bits must be positive.");
    }
    limbs.resize((bits + 63) / 64, 0);
}

BigInt::BigInt(uint64_t value) : num_bits(64) {
    limbs.resize(1, value);
}

BigInt::BigInt(const std::string& hex_str) {
    std::string clean_hex = (hex_str.substr(0, 2) == "0x") ? hex_str.substr(2) : hex_str;
    size_t num_limbs = (clean_hex.length() + 15) / 16;
    limbs.resize(num_limbs, 0);
    num_bits = clean_hex.length() * 4;

    for (size_t i = 0; i < num_limbs; ++i) {
        size_t pos = clean_hex.length() - (i + 1) * 16;
        size_t len = 16;
        if (pos > clean_hex.length()) { // check for underflow
            len = 16 - (pos + len);
            pos = 0;
        }
        std::string limb_str = clean_hex.substr(pos, len);
        limbs[i] = std::stoull(limb_str, nullptr, 16);
    }
}

void BigInt::seed(uint64_t seed_val) {
    if (!limbs.empty()) {
        limbs[0] = seed_val;
        for (size_t i = 1; i < limbs.size(); ++i) {
            limbs[i] = 0;
        }
    }
}

BigInt& BigInt::operator^=(const BigInt& other) {
    size_t min_limbs = std::min(limbs.size(), other.limbs.size());
    for (size_t i = 0; i < min_limbs; ++i) {
        limbs[i] ^= other.limbs[i];
    }
    trim();
    return *this;
}

BigInt& BigInt::operator<<=(size_t shift) {
    if (shift == 0) return *this;
    
    size_t limb_shift = shift / 64;
    size_t bit_shift = shift % 64;

    if (limb_shift >= limbs.size()) {
        std::fill(limbs.begin(), limbs.end(), 0);
        return *this;
    }

    if (bit_shift == 0) {
        for (size_t i = limbs.size() - 1; i >= limb_shift; --i) {
            limbs[i] = limbs[i - limb_shift];
        }
    } else {
        for (size_t i = limbs.size() - 1; i > limb_shift; --i) {
            limbs[i] = (limbs[i - limb_shift] << bit_shift) | (limbs[i - limb_shift - 1] >> (64 - bit_shift));
        }
        limbs[limb_shift] = limbs[0] << bit_shift;
    }

    for (size_t i = 0; i < limb_shift; ++i) {
        limbs[i] = 0;
    }

    trim();
    return *this;
}

BigInt& BigInt::operator>>=(size_t shift) {
    if (shift == 0) return *this;

    size_t limb_shift = shift / 64;
    size_t bit_shift = shift % 64;

    if (limb_shift >= limbs.size()) {
        std::fill(limbs.begin(), limbs.end(), 0);
        return *this;
    }

    if (bit_shift == 0) {
        for (size_t i = 0; i < limbs.size() - limb_shift; ++i) {
            limbs[i] = limbs[i + limb_shift];
        }
    } else {
        for (size_t i = 0; i < limbs.size() - limb_shift - 1; ++i) {
            limbs[i] = (limbs[i + limb_shift] >> bit_shift) | (limbs[i + limb_shift + 1] << (64 - bit_shift));
        }
        limbs[limbs.size() - limb_shift - 1] = limbs.back() >> bit_shift;
    }

    for (size_t i = limbs.size() - limb_shift; i < limbs.size(); ++i) {
        limbs[i] = 0;
    }
    
    return *this;
}

BigInt BigInt::operator+(const BigInt& other) const {
    size_t max_limbs = std::max(limbs.size(), other.limbs.size());
    BigInt result(max_limbs * 64);
    uint64_t carry = 0;

    for (size_t i = 0; i < max_limbs; ++i) {
        uint64_t l1 = (i < limbs.size()) ? limbs[i] : 0;
        uint64_t l2 = (i < other.limbs.size()) ? other.limbs[i] : 0;
        uint64_t sum = l1 + l2 + carry;
        result.limbs[i] = sum;
        carry = (sum < l1) || (sum == l1 && carry);
    }
    if (carry > 0) {
        result.limbs.push_back(carry);
    }
    result.trim();
    return result;
}

BigInt BigInt::operator-(const BigInt& other) const {
    if (*this < other) {
        throw std::runtime_error("Subtraction would result in a negative number.");
    }
    BigInt result(num_bits);
    uint64_t borrow = 0;

    for (size_t i = 0; i < limbs.size(); ++i) {
        uint64_t l1 = limbs[i];
        uint64_t l2 = (i < other.limbs.size()) ? other.limbs[i] : 0;
        uint64_t diff = l1 - l2 - borrow;
        result.limbs[i] = diff;
        borrow = (l1 < l2) || (l1 == l2 && borrow);
    }
    result.trim();
    return result;
}

BigInt BigInt::operator*(const BigInt& other) const {
    BigInt result(num_bits + other.num_bits);
    for (size_t i = 0; i < limbs.size(); ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < other.limbs.size(); ++j) {
            __int128 p = (__int128)limbs[i] * other.limbs[j] + result.limbs[i + j] + carry;
            result.limbs[i + j] = (uint64_t)p;
            carry = p >> 64;
        }
        if (carry > 0) {
            result.limbs[i + other.limbs.size()] += carry;
        }
    }
    result.trim();
    return result;
}

BigInt BigInt::operator/(const BigInt& other) const {
    if (other.is_zero()) {
        throw std::runtime_error("Division by zero.");
    }
    if (*this < other) {
        return BigInt(uint64_t(0));
    }

    BigInt quotient(num_bits);
    BigInt remainder(*this);

    int this_bits = this->bit_length();
    int other_bits = other.bit_length();

    BigInt temp_other = other;
    temp_other <<= (this_bits - other_bits);

    for (int i = this_bits - other_bits; i >= 0; --i) {
        if (remainder >= temp_other) {
            remainder = remainder - temp_other;
            quotient.set_bit(i, 1);
        }
        temp_other >>= 1;
    }

    quotient.trim();
    return quotient;
}

BigInt BigInt::operator%(const BigInt& other) const {
     if (other.is_zero()) {
        throw std::runtime_error("Division by zero.");
    }
    if (*this < other) {
        return *this;
    }

    BigInt remainder(*this);

    int this_bits = this->bit_length();
    int other_bits = other.bit_length();

    BigInt temp_other = other;
    temp_other <<= (this_bits - other_bits);

    for (int i = this_bits - other_bits; i >= 0; --i) {
        if (remainder >= temp_other) {
            remainder = remainder - temp_other;
        }
        temp_other >>= 1;
    }
    
    remainder.trim();
    return remainder;
}

bool BigInt::operator==(const BigInt& other) const {
    if (limbs.size() != other.limbs.size()) return false;
    for (size_t i = 0; i < limbs.size(); ++i) {
        if (limbs[i] != other.limbs[i]) return false;
    }
    return true;
}

bool BigInt::operator!=(const BigInt& other) const {
    return !(*this == other);
}

bool BigInt::operator<(const BigInt& other) const {
    if (limbs.size() != other.limbs.size()) {
        return limbs.size() < other.limbs.size();
    }
    for (int i = limbs.size() - 1; i >= 0; --i) {
        if (limbs[i] != other.limbs[i]) {
            return limbs[i] < other.limbs[i];
        }
    }
    return false;
}

bool BigInt::operator>(const BigInt& other) const {
    return other < *this;
}

bool BigInt::operator<=(const BigInt& other) const {
    return !(*this > other);
}

bool BigInt::operator>=(const BigInt& other) const {
    return !(*this < other);
}

bool BigInt::is_zero() const {
    for (uint64_t limb : limbs) {
        if (limb != 0) return false;
    }
    return true;
}

bool BigInt::is_even() const {
    return limbs.empty() || (limbs[0] & 1) == 0;
}

void BigInt::set_bit(size_t n, bool value) {
    size_t limb_idx = n / 64;
    size_t bit_idx = n % 64;
    if (limb_idx >= limbs.size()) {
        limbs.resize(limb_idx + 1, 0);
    }
    if (value) {
        limbs[limb_idx] |= (1ULL << bit_idx);
    } else {
        limbs[limb_idx] &= ~(1ULL << bit_idx);
    }
}

bool BigInt::get_bit(size_t n) const {
    size_t limb_idx = n / 64;
    size_t bit_idx = n % 64;
    if (limb_idx >= limbs.size()) {
        return false;
    }
    return (limbs[limb_idx] >> bit_idx) & 1;
}

size_t BigInt::bit_length() const {
    if (is_zero()) return 0;
    size_t last_limb_idx = limbs.size() - 1;
    uint64_t last_limb = limbs[last_limb_idx];
    size_t bits = last_limb_idx * 64;
    while (last_limb > 0) {
        last_limb >>= 1;
        bits++;
    }
    return bits;
}


BigInt BigInt::modular_pow(BigInt base, BigInt exponent, const BigInt& modulus) {
    BigInt result(uint64_t(1));
    base = base % modulus;
    while (!exponent.is_zero()) {
        if (!exponent.is_even()) {
            result = (result * base) % modulus;
        }
        exponent >>= 1;
        base = (base * base) % modulus;
    }
    return result;
}

std::string BigInt::to_hex_string() const {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    bool first = true;
    for (auto it = limbs.rbegin(); it != limbs.rend(); ++it) {
        if (first) {
            if (*it != 0) {
                ss << "0x" << *it;
                first = false;
            }
        } else {
            ss << std::setw(16) << *it;
        }
    }
    if (first) {
        return "0x0";
    }
    return ss.str();
}

std::string BigInt::to_binary_string() const {
    std::stringstream ss;
    bool first = true;
    for (auto it = limbs.rbegin(); it != limbs.rend(); ++it) {
        uint64_t limb = *it;
        for (int i = 63; i >= 0; --i) {
            bool bit = (limb >> i) & 1;
            if (!first || bit) {
                ss << (bit ? '1' : '0');
                first = false;
            }
        }
    }
    if (first) {
        return "0";
    }
    std::string bin_str = ss.str();
    
    // Pad with leading zeros to match num_bits if necessary.
    if (bin_str.length() < num_bits) {
        bin_str.insert(0, num_bits - bin_str.length(), '0');
    }
    
    return bin_str;
}


/**
 * @brief Sets the limbs of the BigInt from a vector of uint64_t.
 * 
 * @param new_limbs A vector of uint64_t representing the new limbs.
 */
void BigInt::set_limbs(const std::vector<uint64_t>& new_limbs) {
    limbs = new_limbs;
    limbs.resize((num_bits + 63) / 64, 0);
    trim();
}

/**
 * @brief Removes leading zero limbs from the BigInt representation.
 */
void BigInt::trim() {
    while (limbs.size() > 1 && limbs.back() == 0) {
        limbs.pop_back();
    }
}

