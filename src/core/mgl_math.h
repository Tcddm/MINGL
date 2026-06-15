#ifndef MGL_MATH_H
#define MGL_MATH_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif

#define MGL_ABS_I16(n) ((n)>=0?(n):(int16_t)(-(int32_t)(n)))

//请注意！表格90度因为32768，因为int16所以改为32767，精度损失极低
static const uint16_t sin_table_90[] = {
        0,     572,   1144,  1715,  2286,  2856,  3425,  3993,  4560,  5126,  5690,  6252,  6813,  7371,  7927,  8481,
        9032,  9580,  10126, 10668, 11207, 11743, 12275, 12803, 13328, 13848, 14365, 14876, 15384, 15886, 16384, 16877,
        17364, 17847, 18324, 18795, 19261, 19720, 20174, 20622, 21063, 21498, 21926, 22348, 22763, 23170, 23571, 23965,
        24351, 24730, 25102, 25466, 25822, 26170, 26510, 26842, 27166, 27482, 27789, 28088, 28378, 28660, 28932, 29197,
        29452, 29698, 29935, 30163, 30382, 30592, 30792, 30983, 31164, 31336, 31499, 31651, 31795, 31928, 32052, 32166,
        32270, 32365, 32449, 32524, 32588, 32643, 32688, 32723, 32748, 32763, 32767
};

int16_t mgl_sin_i16(int16_t angle);

static inline int16_t mgl_cos_i16(int16_t angle) {
    return mgl_sin_i16((int16_t)(angle+90));
}

void mgl_sort(int16_t * arr,int16_t n);

void *binary_search(const void *base,size_t num,size_t size,const void *key,int (*cmp)(const void *,const void *));

#ifdef __cplusplus
}
#endif
#endif //MGL_MATH_H
