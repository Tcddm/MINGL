#include "mgl_math.h"
int16_t mgl_sin_i16(int16_t angle){
    angle=(int16_t)(angle%360);
    if (angle<0) angle += 360;
    if (angle<=90) return (int16_t)sin_table_90[angle];
    if (angle<=180) return (int16_t)sin_table_90[180 - angle];
    if (angle<=270) return (int16_t)-sin_table_90[angle - 180];
    return (int16_t)-sin_table_90[360 - angle];
}
void mgl_sort(int16_t * arr,int16_t n){
    for (int16_t i=0; i < n-1; i++) {
        for (int16_t j=0; j < (int16_t)(n-i-1); j++) {
            if (arr[j] > arr[j+1]) {
                int16_t tmp=arr[j];
                arr[j]=arr[j+1];
                arr[j+1]=tmp;
            }
        }
    }
}


void *binary_search(const void *base,size_t num,size_t size,const void *key,int (*cmp)(const void *,const void *)){
    size_t left=0;
    size_t right=num;
    while (left<right) {
        size_t mid=left+((right-left)>>1);
        const void *p=(const uint8_t *)base+mid*size;
        int ret=cmp(key,p);
        if (ret==0) {
            return (void *)p;
        } else if(ret<0){
            right=mid;
        } else {
            left=mid+1;
        }
    }
    return NULL;
}