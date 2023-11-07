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

uint32_t gen_xs32(struct xs32 *rng) {
    uint32_t x = rng->x;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return rng->x = x;
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

uint32_t gen_mt32(struct mt32 *rng) {
    uint32_t y;
    static uint32_t mag01[2] = { 0x0U, MT_A };

    if (rng->mti >= MT_N) {
        int kk;

        for (kk = 0; kk < MT_N - MT_M; kk++) {
            y = (rng->mt[kk] & MT_U) | (rng->mt[kk + 1] & MT_L);
            rng->mt[kk] = rng->mt[kk + MT_M] ^ (y >> 1) ^ mag01[y & 0x1U];
        }

        for (;kk < MT_N - 1; kk++) {
            y = (rng->mt[kk] & MT_U) | (rng->mt[kk + 1] & MT_L);
            rng->mt[kk] = rng->mt[kk + MT_M - MT_N] ^ (y >> 1) ^ mag01[y & 0x1U];
        }

        y = (rng->mt[MT_N - 1] & MT_U) | (rng->mt[0] & MT_L);
        rng->mt[MT_N - 1] = rng->mt[MT_M - 1] ^ (y >> 1) ^ mag01[y & 0x1U];

        rng->mti = 0;
    }

    y = rng->mt[rng->mti++];

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

uint32_t gen_sc32(struct sc32 *rng) {
    uint32_t s, r, y;
    
    if (rng->offset < SC_S) {
        s = rng->x[SC_R - (SC_S - rng->offset)];
    } else {
        s = rng->x[rng->offset - SC_S];
    }

    r = rng->x[rng->offset];
    if (s >= r + rng->c) {
        y = s - r - rng->c;
        rng->c = 0x0U;
    } else {
        y = (0x7fffffffU - r - rng->c) + s + 1;
        rng->c = 0x1U;
    }

    rng->x[rng->offset] = y;
    rng->offset++;
    if (rng->offset >= SC_R) {
        rng->offset = 0;
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

uint32_t gen_lm32_helper(struct lm32 *rng) {
    uint32_t hi = rng->x / LM_Q;
    uint32_t lo = rng->x % LM_Q;
    uint32_t y;
    int32_t t = (LM_A * lo) - (LM_R * hi);

    if (t > 0) {
        y = t;
    } else {
        y = t + LM_M;
    }

    return rng->x = y;
}

uint32_t gen_lm32(struct lm32 *rng) {
    uint16_t a = (gen_lm32_helper(rng) >> 15);
    uint16_t b = (gen_lm32_helper(rng) >> 15);
    return (((uint32_t)a) << 16) | b;
}



// PCG32 - Implementation from `https://www.pcg-random.org/download.html#minimal-c-implementation`

#define PCG32_INIT_STATE  0xda3e39cb94b95bdbULL
     
struct pcg32 {      // Internals are *Private*.
    uint64_t state; // RNG state.  All values are possible.
    uint64_t inc;   // Controls which RNG sequence (stream) is selected. Must *always* be odd.
};

// pcg32_random_r(rng)  => gen_pcg32(rng)
//     Generate a uniformly distributed 32-bit random number

uint32_t gen_pcg32(struct pcg32 *rng) {
    uint64_t oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

// pcg32_srandom_r(rng, initstate, initseq) => seed_pcg32(seed)
//     Seed the rng.  Specified in two parts, state initializer and a
//     sequence selection constant (a.k.a. stream id)

struct pcg32 seed_pcg32(unsigned seed) {
    struct pcg32 rng = { 0, 0 };

    rng.state = 0U;
    rng.inc = (((uint64_t)seed) << 1u) | 1u;
    gen_pcg32(&rng);
    rng.state += PCG32_INIT_STATE;
    gen_pcg32(&rng);

    return rng;
}



// ChaCha - Implementation based on `https://docs.rs/crate/rand/0.1.1/source/src/chacha.rs` with snippets from ``https://gist.github.com/orlp/32f5d1b631ab092608b1`

#define KEYS 8
#define STATES 16
#define ROUNDS 20

struct cha32 {
    uint32_t buffer[STATES];
    uint32_t state[STATES];
    unsigned index;
};


struct cha32 seed_cha32(unsigned seed) {
    struct cha32 rng;
    
    for (unsigned int i = 0; i < STATES; i++) {
        rng.buffer[i] = 0;
    }
    
    struct xs32 helper_rng = seed_xs32(seed);

    rng.state[0] = 0x61707865;
    rng.state[1] = 0x3320646E;
    rng.state[2] = 0x79622D32;
    rng.state[3] = 0x6B206574;

    for (unsigned int i = 0; i < KEYS; i++) {
        rng.state[4 + i] = gen_xs32(&helper_rng);
    }

    rng.state[12] = 0;
    rng.state[13] = 0;
    rng.state[14] = 0;
    rng.state[15] = 0;


    rng.index = STATES;

    return rng;
}

uint32_t gen_cha32(struct cha32 *rng) {
    #define CHACHA_ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

    #define CHACHA_QUARTERROUND(x, a, b, c, d) \
        x[a] = x[a] + x[b]; x[d] ^= x[a]; x[d] = CHACHA_ROTL32(x[d], 16); \
        x[c] = x[c] + x[d]; x[b] ^= x[c]; x[b] = CHACHA_ROTL32(x[b], 12); \
        x[a] = x[a] + x[b]; x[d] ^= x[a]; x[d] = CHACHA_ROTL32(x[d],  8); \
        x[c] = x[c] + x[d]; x[b] ^= x[c]; x[b] = CHACHA_ROTL32(x[b],  7)

    if (rng->index == STATES) {
        for (unsigned int i = 0; i < STATES; i++) {
            rng->buffer[i] = rng->state[i];
        }

        for (unsigned int i = 0; i < ROUNDS; i += 2) {
            // Column Round
            CHACHA_QUARTERROUND(rng->buffer, 0, 4,  8, 12);
            CHACHA_QUARTERROUND(rng->buffer, 1, 5,  9, 13);
            CHACHA_QUARTERROUND(rng->buffer, 2, 6, 10, 14);
            CHACHA_QUARTERROUND(rng->buffer, 3, 7, 11, 15);
            // Diagonal Round
            CHACHA_QUARTERROUND(rng->buffer, 0, 5, 10, 15);
            CHACHA_QUARTERROUND(rng->buffer, 1, 6, 11, 12);
            CHACHA_QUARTERROUND(rng->buffer, 2, 7,  8, 13);
            CHACHA_QUARTERROUND(rng->buffer, 3, 4,  9, 14);
        }

        for (unsigned int i = 0; i < STATES; i++) {
            rng->buffer[i] += rng->state[i];
        }

        do {
            rng->index = 0;
            rng->state[12] += 1;
            if (rng->state[12] != 0) {
                break;
            }
            rng->state[13] += 1;
            if (rng->state[13] != 0) {
                break;
            }
            rng->state[14] += 1;
            if (rng->state[14] != 0) {
                break;
            }
            rng->state[15] += 1;
        } while (0);
    }

    uint32_t val = rng->buffer[rng->index % STATES];
    rng->index++;

    return val;
}