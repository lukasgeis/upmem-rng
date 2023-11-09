#include "rng.c"
#include <stdint.h>

// OpenBSD - Code from `https://arxiv.org/pdf/1805.10941.pdf`

uint32_t openbsd(uint32_t s, uint32_t (*random32)(unsigned), unsigned id) {
    uint32_t t = (UINT32_MAX - s + 1) % s;
    uint32_t x;
    do {
        x = random32(id);
    } while (x < t);
    return x % s;
}


// Java - Code from `https://arxiv.org/pdf/1805.10941.pdf`

uint32_t java(uint32_t s, uint32_t (*random32)(unsigned), unsigned id) {
    uint32_t x = random32(id);
    uint32_t r = x % s;
    while (x - r > UINT32_MAX - s + 1) {
        x = random32(id);
        r = x % s;
    }
    return r;
}


// Lemire - Code from `https://arxiv.org/pdf/1805.10941.pdf` - originally "nearlydivisionless"

uint32_t lemire(uint32_t s, uint32_t (*random32)(unsigned), unsigned id) {
    uint32_t x = random32(id);
    uint64_t m = (uint64_t) x * (uint64_t) s;
    uint32_t l = (uint32_t) m;
    if (l < s) {
        uint32_t t = -s % s;
        while (l < t) {
            x = random32(id);
            m = (uint64_t) x * (uint64_t) s;
            l = (uint32_t) m;
        }
    }
    return m >> 32;
}


// Flips - From `https://arxiv.org/pdf/1304.1916.pdf` - originally "FastDiceRoller"

uint32_t flips(uint32_t s, uint32_t (*random32)(unsigned), unsigned id) {
    uint32_t v = 1;
    uint32_t d = 0;
    while (1) {
        d += d + (random32(id) & 1);
        v += v;

        if (v >= s) {
            if (d < s) {
                return d;
            }
            v -= s;
            d -= s;
        }
    }
}


// RoundReject - Round *s* to next highest power of 2 and use Rejection-Sampling

uint32_t rr(uint32_t s, uint32_t (*random32)(unsigned), unsigned id) {
    unsigned mask = (1 << (32 - __builtin_clz(s))) - 1;
    uint32_t x = random32(id) & mask;
    while (x >= s) {
        x = random32(id) & mask;
    }
    return x;
}
