#include "kuz_calc.h"
#include <algorithm>

static void GOST_Kuz_S(const uint8_t *in_data, uint8_t *out_data)
{
    int i;
    for (i = 0; i < BLCK_SIZE; i++)
        out_data[i] = Pi[in_data[i]];
}

static void GOST_Kuz_X(const uint8_t *a, const uint8_t *b, uint8_t *c)
{
    int i;
    for (i = 0; i < BLCK_SIZE; i++)
        c[i] = a[i] ^ b[i];
}

static uint8_t GOST_Kuz_GF_mul(uint8_t a, uint8_t b)
{
    uint8_t c = 0;
    uint8_t hi_bit;
    int i;
    for (i = 0; i < 8; i++)
    {
        if (b & 1)
            c ^= a;
        hi_bit = a & 0x80;
        a <<= 1;
        if (hi_bit)
            a ^= 0xc3; //полином x^8+x^7+x^6+x+1
        b >>= 1;
    }
    return c;
}

static void GOST_Kuz_R(uint8_t *state)
{
    int i;
    uint8_t a_15 = 0;
    vect internal;
    for (i = 15; i >= 0; i--)
    {
        internal[i - 1] = state[i];
        a_15 ^= GOST_Kuz_GF_mul(state[i], l_vec[i]);
    }
    internal[15] = a_15;
    memcpy(state, internal, BLCK_SIZE);
}


static void GOST_Kuz_L(const uint8_t *in_data, uint8_t *out_data)
{
    int i;
    vect internal;
    memcpy(internal, in_data, BLCK_SIZE);
    for (i = 0; i < 16; i++)
        GOST_Kuz_R(internal);
    memcpy(out_data, internal, BLCK_SIZE);
}


static void GOST_Kuz_Get_C()
{
    int i;
    vect iter_num[32];
    for (i = 0; i < 32; i++)
    {
        memset(iter_num[i], 0, BLCK_SIZE);
        iter_num[i][0] = i+1;
    }
    for (i = 0; i < 32; i++)
         GOST_Kuz_L(iter_num[i], iter_C[i]);
}

static void GOST_Kuz_F(const uint8_t *in_key_1, const uint8_t *in_key_2,
           uint8_t *out_key_1, uint8_t *out_key_2,
           uint8_t *iter_const)
{
    vect internal;
    memcpy(out_key_2, in_key_1, BLCK_SIZE);
    GOST_Kuz_X(in_key_1, iter_const, internal);
    GOST_Kuz_S(internal, internal);
    GOST_Kuz_L(internal, internal);
    GOST_Kuz_X(internal, in_key_2, out_key_1);
}

void GOST_Kuz_Expand_Key(const uint8_t *key)
{
    int i;
    uint8_t key_1[KEY_SIZE/2];
    uint8_t key_2[KEY_SIZE/2];
    uint8_t iter_1[KEY_SIZE/2];
    uint8_t iter_2[KEY_SIZE/2];
    uint8_t iter_3[KEY_SIZE/2];
    uint8_t iter_4[KEY_SIZE/2];
    memcpy(key_1, key + KEY_SIZE/2, KEY_SIZE/2);
    memcpy(key_2, key, KEY_SIZE/2);
    GOST_Kuz_Get_C();
    memcpy(iter_key[0], key_1, KEY_SIZE/2);
    memcpy(iter_key[1], key_2, KEY_SIZE/2);
    memcpy(iter_1, key_1, KEY_SIZE/2);
    memcpy(iter_2, key_2, KEY_SIZE/2);
    for (i = 0; i < 4; i++)
    {
        GOST_Kuz_F(iter_1, iter_2, iter_3, iter_4, iter_C[0 + 8 * i]);
        GOST_Kuz_F(iter_3, iter_4, iter_1, iter_2, iter_C[1 + 8 * i]);
        GOST_Kuz_F(iter_1, iter_2, iter_3, iter_4, iter_C[2 + 8 * i]);
        GOST_Kuz_F(iter_3, iter_4, iter_1, iter_2, iter_C[3 + 8 * i]);
        GOST_Kuz_F(iter_1, iter_2, iter_3, iter_4, iter_C[4 + 8 * i]);
        GOST_Kuz_F(iter_3, iter_4, iter_1, iter_2, iter_C[5 + 8 * i]);
        GOST_Kuz_F(iter_1, iter_2, iter_3, iter_4, iter_C[6 + 8 * i]);
        GOST_Kuz_F(iter_3, iter_4, iter_1, iter_2, iter_C[7 + 8 * i]);
        memcpy(iter_key[2 * i + 2], iter_1, KEY_SIZE/2);
        memcpy(iter_key[2 * i + 3], iter_2, KEY_SIZE/2);
    }
}

void GOST_Kuz_Destroy_Key()
{
    int i;
    for (i = 0; i < 10; i++)
        memset(iter_key[i], 0x00, BLCK_SIZE);
}

void GOST_Kuz_Encrypt(const uint8_t *blk, uint8_t *out_blk)
{
    int i;
    memcpy(out_blk, blk, BLCK_SIZE);
    for(i = 0; i < 9; i++)
    {
        GOST_Kuz_X(iter_key[i], out_blk, out_blk);

        GOST_Kuz_S(out_blk, out_blk);

        GOST_Kuz_L(out_blk, out_blk);
    }
    GOST_Kuz_X(out_blk, iter_key[9], out_blk);
}

static void inc_ctr(uint8_t *ctr)
{
    int i;
    unsigned int internal = 0;
    uint8_t bit[BLCK_SIZE];
    memset(bit, 0x00, BLCK_SIZE);
    bit[0] = 0x01;
    for (i = 0; i < BLCK_SIZE; i++)
    {
        internal = ctr[i] + bit[i] + (internal >> 8);
        ctr[i] = internal & 0xff;
    }
}

static void add_xor(const uint8_t *a, const uint8_t *b, uint8_t *c)
{
    int i;
    for (i = 0; i < BLCK_SIZE; i++)
        c[i] = a[i]^b[i];
}

void reverse_array(unsigned char *array, int size)
{
    int i;
    unsigned char temp;

    for(i=0; i<size/2; i++)
    {
        temp = array[i];
        array[i] = array[size-1-i];
        array[size-1-i] = temp;
    }
}

void CTR_Crypt(uint8_t *ctr, uint8_t *in_buf, uint8_t *out_buf, uint8_t *key, uint64_t size, bool ch)
{
    uint64_t num_blocks = size / BLCK_SIZE;
    uint8_t gamma[BLCK_SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    uint8_t internal[BLCK_SIZE];

    uint64_t i;
    GOST_Kuz_Expand_Key(key);
    for (i = 0; i < num_blocks; i++)
    {
        GOST_Kuz_Encrypt(ctr, gamma);
        if(ch)
            reverse_array(gamma, BLCK_SIZE);
        inc_ctr(ctr);
        memcpy(internal, in_buf + i*BLCK_SIZE, BLCK_SIZE);
        add_xor(internal, gamma, internal);
        memcpy(out_buf + i*BLCK_SIZE, internal, BLCK_SIZE);
        size = size - BLCK_SIZE;
    }
    GOST_Kuz_Destroy_Key();
}

size_t convert_hex(uint8_t *dest, size_t count, const char *src)
{
    char buf[3];
    size_t i;
    for (i = 0; i < count && *src; i++) {
        buf[0] = *src++;
        buf[1] = '\0';
        if (*src) {
            buf[1] = *src++;
            buf[2] = '\0';
        }
        if (sscanf(buf, "%hhx", &dest[i]) != 1)
            break;
    }
    return i;
}
std::string convert_to_string(const unsigned char* arr, size_t size)
{

    std::string result;
    for (int i = 0; i < size; i++) {
        char hex[3];
        sprintf(hex, "%02x", arr[i]);
        result += hex;
    }
    return result;
}

std::string reverse_hex(std::string str)
{
    std::reverse(str.begin(), str.end());
    std::string result;
    for (int i = 0; i < str.length(); i += 2) {
        result += str[i+1];
        result += str[i];
    }
    return result;
}
