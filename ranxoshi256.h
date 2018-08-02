/*
ranxoshi256.h - Portable, single-file, PRNG library implementing the xoshiro256** algorithm

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring
rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software.
If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

/*
ranxoshi256 supports the following three configurations:
#define RANXOSHI256_EXTERN
    Default, should be used when using ranxoshi256 in multiple compilation units within the same project.
#define RANXOSHI256_IMPLEMENTATION
    Must be defined in exactly one source file within a project for ranxoshi256 to be found by the linker.
#define RANXOSHI256_STATIC
    Defines all ranxoshi256 functions as static, useful if ranxoshi256 is only used in a single compilation unit.
*/

//include only once
#ifndef RANXOSHI256_H
#define RANXOSHI256_H

//process configuration
#ifdef RANXOSHI256_STATIC
    #define RANXOSHI256_IMPLEMENTATION
    #define RSHI256DEF static
#else //RANXOSHI256_EXTERN
    #define RSHI256DEF extern
#endif

//includes
#include <stdint.h>

//structs
struct ranxoshi256 {
    uint64_t s[4]; //PRNG state
};

//function declarations
RSHI256DEF void ranxoshi256Seed(struct ranxoshi256*, const unsigned char[32]);
    //pastes the given seed of 32 bytes into the generator's state in an endian-proof way
    //this allows for consistent results across machines with differing architectures
RSHI256DEF float ranxoshi256FloatCO(struct ranxoshi256*);
    //returns a random float in the range [0.0, 1.0) (not including 1.0)
    //provides maximum uniformity but only 2^24 possible values
RSHI256DEF float ranxoshi256FloatCC(struct ranxoshi256*);
    //returns a random float in the range [0.0, 1.0] (including 0.0 and 1.0)
    //uses more of float's available precision at the cost of uniformity
RSHI256DEF double ranxoshi256DoubleCO(struct ranxoshi256*);
    //returns a random double in the range [0.0, 1.0) (not including 1.0)
    //provides maximum uniformity but only 2^53 possible values
RSHI256DEF double ranxoshi256DoubleCC(struct ranxoshi256*);
    //returns a random double in the range [0.0, 1.0] (including 0.0 and 1.0)
    //uses more of double's available precision at the cost of uniformity
RSHI256DEF uint64_t ranxoshi256Next(struct ranxoshi256*);
    //returns a random uint64 (raw output of the generator)
RSHI256DEF void ranxoshi256Jump(struct ranxoshi256*);
    //advances the given generator's internal state by 2^128 calls to ranxoshi256Next
    //useful for generating multiple non-overlapping subsequences from a single seed

//implementation section
#ifdef RANXOSHI256_IMPLEMENTATION

//function declarations
static inline uint64_t ranxoshi256Rotate(uint64_t, int);

//public functions
RSHI256DEF void ranxoshi256Seed (struct ranxoshi256* ctx, const unsigned char s[32]) {
    for (int i = 0; i < 4; i++)
        ctx->s[i] = ((uint64_t)s[i*8] << 56)|((uint64_t)s[i*8+1] << 48)|
            ((uint64_t)s[i*8+2] << 40)|((uint64_t)s[i*8+3] << 32)|
            ((uint64_t)s[i*8+4] << 24)|((uint64_t)s[i*8+5] << 16)|
            ((uint64_t)s[i*8+6] << 8)|((uint64_t)s[i*8+7]);
}
RSHI256DEF float ranxoshi256FloatCO (struct ranxoshi256* ctx) {
    return (float)(ranxoshi256Next(ctx) >> 40)/16777216.0f;
}
RSHI256DEF float ranxoshi256FloatCC (struct ranxoshi256* ctx) {
    return (float)(ranxoshi256Next(ctx) >> 32)/(float)UINT32_MAX;
}
RSHI256DEF double ranxoshi256DoubleCO (struct ranxoshi256* ctx) {
    return (double)(ranxoshi256Next(ctx) >> 11)/9007199254740992.0;
}
RSHI256DEF double ranxoshi256DoubleCC (struct ranxoshi256* ctx) {
    return (double)ranxoshi256Next(ctx)/(double)UINT64_MAX;
}
RSHI256DEF uint64_t ranxoshi256Next (struct ranxoshi256* ctx) {
	const uint64_t res = ranxoshi256Rotate(ctx->s[1] * 5, 7) * 9;
	const uint64_t t = ctx->s[1] << 17;
	ctx->s[2] ^= ctx->s[0];
	ctx->s[3] ^= ctx->s[1];
	ctx->s[1] ^= ctx->s[2];
	ctx->s[0] ^= ctx->s[3];
	ctx->s[2] ^= t;
	ctx->s[3] = ranxoshi256Rotate(ctx->s[3], 45);
	return res;
}
RSHI256DEF void ranxoshi256Jump (struct ranxoshi256* ctx) {
    static const uint64_t jump[] = {0x180EC6D33CFD0ABA, 0xD5A61266F0C9392C, 0xA9582618E03FC9AA, 0x39ABDC4529B1661C};
	uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
	for (int i = 0; i < sizeof(jump)/sizeof(jump[0]); i++)
		for (int b = 0; b < 64; b++) {
			if (jump[i] & (uint64_t)1 << b) {
				s0 ^= ctx->s[0];
				s1 ^= ctx->s[1];
				s2 ^= ctx->s[2];
				s3 ^= ctx->s[3];
			}
			ranxoshi256Next(ctx);	
		}
	ctx->s[0] = s0;
	ctx->s[1] = s1;
	ctx->s[2] = s2;
	ctx->s[3] = s3;
}

//internal functions
static inline uint64_t ranxoshi256Rotate (uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

#endif //RANXOSHI256_IMPLEMENTATION
#endif //RANXOSHI256_H