#include "aes.h"

static const uint8_t sbox[16][16] = {
    {0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76},
    {0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0},
    {0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15},
    {0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75},
    {0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84},
    {0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf},
    {0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8},
    {0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2},
    {0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73},
    {0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb},
    {0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79},
    {0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08},
    {0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a},
    {0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e},
    {0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf},
    {0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16},
};

// inverse Sbox
static const uint8_t inv_sbox[16][16] = {
    {0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb},
    {0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb},
    {0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e},
    {0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25},
    {0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92},
    {0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84},
    {0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06},
    {0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b},
    {0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73},
    {0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e},
    {0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b},
    {0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4},
    {0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f},
    {0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef},
    {0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61},
    {0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d}
};

static void subBytse(aes_ctx *ctx){
    uint8_t row = 0;
    uint8_t col = 0;
    for(int i = 0; i < AES_BLOCK_SIZE_BYTES; i++) {
        row = (ctx->state[i] & 0xF0) >> 4;
        col = ctx->state[i] & 0x0F;
        ctx->state[i] = sbox[row][col];
    }
}

// row 1 remains the same 
// row 2 cyclically shifted to the left by 1 
// row 3 cyclically shifted to the left by 2
// row 4 cyclically shifted to the left by 3, which is basically 
// cyclically shifted to the right by 1
/*
    The state matrix after shiftRows
    0  4  8   12                    0  4  8  12
    1  5  9   13        ===>        5  9  13  1
    2  6  10  14                    10 14 2   6
    3  7  11  15                    15 3  7  11
*/
static void shiftRows(aes_ctx *ctx){
    uint8_t tmp1, tmp2;
    tmp1 = ctx->state[1];
    ctx->state[1] = ctx->state[5];
    ctx->state[5] = ctx->state[9];
    ctx->state[9] = ctx->state[13];
    ctx->state[13] = tmp1;

    tmp1 = ctx->state[2];
    tmp2 = ctx->state[6];
    ctx->state[2] = ctx->state[10];
    ctx->state[6] = ctx->state[14];
    ctx->state[10] = tmp1;
    ctx->state[14] = tmp2;

    tmp1 = ctx->state[15];
    ctx->state[15] = ctx->state[11];
    ctx->state[11] = ctx->state[7];
    ctx->state[7] = ctx->state[3];
    ctx->state[3] = tmp1;
    
}

/*
    This function compute 2 * x in GF(2^8) as defined in AES
    if overflow occurs then we reduce the result by the irreducible polynomial
    as defined in NIST FIP 197
    https://nvlpubs.nist.gov/nistpubs/fips/nist.fips.197.pdf
    the polynomial is m(x) = x^8 + x^4 + x^3 + x + 1
    Multiplication is specified in section 4.2.1
*/
static uint8_t xtime(uint8_t x){
    // check if overflow will occur
    uint8_t res = x << 1;
    // check if there is an overflow, if the msb of x is set, then shifting it will cause an overflow 
    // if overflow occured, then (res & 0x80 >> 7) = 1, so we subtract 1 and flip all the bits to set the mask to 0xFF
    // if no overflow, then (res & 0x80 >> 7) = 0, subtracting 1 makes if 0xFF, so we flip all bits to turn it into 0x00
    // this masks will allow us to conditionally XOR with either 0x1B or 0x00
    uint8_t overflow_mask = ~(((x & 0x80) >> 7) - 1);
    return res ^ (0x1B & overflow_mask);
}

/*
    As specified in 4.2.1 of the AES standard, multiplication by any constant can be achieved by adding intermediate 
    results
*/
static uint8_t xtime_GF3(uint8_t x){
    return xtime(x) ^ x;
}

/*
    After shift row operation, each column is treated as a vector and 
    transformed by multiplying a matrix with fixed value
    c1     | 2 3 1 1 |    b1 
    c2  =  | 1 2 3 1 | x  b2
    c3     | 1 1 2 3 |    b3
    c4     | 3 1 1 2 |    b4
    All operations happen in GF(2^8)
*/
static void mixColumn(aes_ctx *ctx){
    for(int col = 0; col < 4; col++) {
        ctx->state[0 + (4*col)] = xtime(ctx->state[0 + (4*col)]) ^ xtime_GF3(ctx->state[1 + (4*col)])
                                    ^ ctx->state[2 + (4*col)] ^ ctx->state[3 + (4*col)];
        
        ctx->state[1 + (4*col)] = ctx->state[0 + (4*col)] ^ xtime(ctx->state[1 + (4*col)]) ^ xtime_GF3(ctx->state[2 + (4*col)]) ^
                                    ctx->state[3 + (4*col)];

        ctx->state[2 + (4*col)] = ctx->state[0 + (4*col)] ^ ctx->state[1 + (4*col)] ^ xtime(ctx->state[2 +(4*col)]) ^
                                    xtime_GF3(ctx->state[3 + (4*col)]);

        ctx->state[3 + (4*col)] = xtime_GF3(ctx->state[0 + (4*col)]) ^ ctx->state[1 + (4*col)] ^ ctx->state[2 + (4*col)] ^ 
                                    xtime(ctx->state[3 + (4*col)]);
    }
}

static void addRoundKey(aes_ctx *ctx, uint8_t *subkey){
    for(int i = 0; i < AES_BLOCK_SIZE_BYTES; i++){
        ctx->state[i] ^= subkey[i];
    }
}

static void key_expansion(uint8_t *key, uint8_t *keys){
    
}