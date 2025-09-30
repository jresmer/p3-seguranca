#ifndef XORSHIFT_H
#define XORSHIFT_H

#include "bigint.h"
#include <chrono>

BigInt generate_random(int bits, std::chrono::duration<double, std::milli>& duration);

#endif // XORSHIFT_H
