#include <stdint.h>
#include <stdio.h>

// XorShift - Implementation from `https://en.wikipedia.org/wiki/Xorshift`

struct xs32 {
    uint32_t x;
};

struct xs32 seed_xs32(unsigned seed) {
    struct xs32 rng = { seed };
    return rng;
} 

uint32_t gen_xs32(struct xs32 *state) {
    uint32_t x = state->x;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return state->x = x;
}


// Mersenne Twister - Implementation from `http://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/MT2002/CODES/mt19937ar.c` 

#define MT_N 624            // Parameter `N`
#define MT_M 397            // Parameter `M`
#define MT_A 0x9908b0dfU    // Constant Vector `A`
#define MT_U 0x80000000U    // Mask for most significant `W - R` Bits
#define MT_L 0x7fffffffU    // Mask for least significant `R` Bits

struct mt32 {
    uint32_t mt[MT_N];
    int mti;
};

struct mt32 seed_mt32(unsigned seed) {
    struct mt32 rng;
    rng.mt[0] = seed & 0xffffffff;
    for (int i = 1; i < MT_N; i++) {
        rng.mt[i] = (1812433253U * (rng.mt[i-1] ^ (rng.mt[i-1] >> 30)) + i);
        rng.mt[i] &= 0xffffffffU;
    }
    rng.mti = MT_N;
    return rng;
}

uint32_t gen_mt32(struct mt32 *state) {
    uint32_t y;
    static uint32_t mag01[2] = { 0x0U, MT_A };

    if (state->mti >= MT_N) {
        int kk;

        for (kk = 0; kk < MT_N - MT_M; kk++) {
            y = (state->mt[kk] & MT_U) | (state->mt[kk + 1] & MT_L);
            state->mt[kk] = state->mt[kk + MT_M] ^ (y >> 1) ^ mag01[y & 0x1U];
        }

        for (;kk < MT_N - 1; kk++) {
            y = (state->mt[kk] & MT_U) | (state->mt[kk + 1] & MT_L);
            state->mt[kk] = state->mt[kk + MT_M - MT_N] ^ (y >> 1) ^ mag01[y & 0x1U];
        }

        y = (state->mt[MT_N - 1] & MT_U) | (state->mt[0] & MT_L);
        state->mt[MT_N - 1] = state->mt[MT_M - 1] ^ (y >> 1) ^ mag01[y & 0x1U];

        state->mti = 0;
    }

    y = state->mt[state->mti++];

    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680U;
    y ^= (y << 15) & 0xefc60000U;
    y ^= (y >> 18);
    
    return y;
}


// Subtract-With-Carry 

#define SC_S 8
#define SC_R 20

struct sc32 {
    uint32_t x[SC_R];
    unsigned c;
    int offset;
};

struct sc32 seed_sc32(unsigned seed) {
    struct sc32 rng;
    struct xs32 helper_rng = seed_xs32(seed);
    for (int i = 0; i < SC_R; i++) {
        rng.x[i] = gen_xs32(&helper_rng);
    }
    rng.offset = 0;
    if (rng.x[0] > rng.x[SC_R - SC_S]) {
        rng.c = 1;
    } else {
        rng.c = 0;
    }

    return rng;
}

uint32_t gen_sc32(struct sc32 *state) {
    uint32_t s, r, y;
    
    if (state->offset < SC_S) {
        s = state->x[SC_R - (SC_S - state->offset)];
    } else {
        s = state->x[state->offset - SC_S];
    }

    r = state->x[state->offset];
    if (s >= r + state->c) {
        y = s - r - state->c;
        state->c = 0x0U;
    } else {
        y = (0x7fffffffU - r - state->c) + s + 1;
        state->c = 0x1U;
    }

    state->x[state->offset] = y;
    state->offset++;
    if (state->offset >= SC_R) {
        state->offset = 0;
    }

    return y;
}


// Lehmer RNG (MINSTD) - Implementation from `https://github.com/RIOT-OS/RIOT/blob/master/sys/random/minstd.c`

#define LM_M 0x7fffffffU
#define LM_A 16807U
#define LM_Q (LM_M / LM_A)
#define LM_R (LM_M % LM_A)

struct lm32 {
    uint32_t x;
};

struct lm32 seed_lm32(unsigned seed) {
    struct lm32 rng = { seed };
    return rng;
}

uint32_t gen_lm32_helper(struct lm32 *state) {
    uint32_t hi = state->x / LM_Q;
    uint32_t lo = state->x % LM_Q;
    uint32_t y;
    int32_t t = (LM_A * lo) - (LM_R * hi);

    if (t > 0) {
        y = t;
    } else {
        y = t + LM_M;
    }

    return state->x = y;
}

uint32_t gen_lm32(struct lm32 *state) {
    uint16_t a = (gen_lm32_helper(state) >> 15);
    uint16_t b = (gen_lm32_helper(state) >> 15);
    return (((uint32_t)a) << 16) | b;
}

