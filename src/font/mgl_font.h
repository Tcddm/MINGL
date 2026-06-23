#ifndef MGL_FONT_H
#define MGL_FONT_H

#include "stdint.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
    uint32_t code_point;      //Unicode码点
    uint8_t w;            //这个字符位图的实际宽度
    uint8_t h;           //位图高度
    int8_t ofs_x;          //绘制时X轴偏移
    int8_t ofs_y;          //绘制时Y轴偏移
    uint8_t adv;          //绘制后光标前进的像素数
    uint16_t ofs_bmp;    //位图偏移量
} mgl_glyph_t;

typedef struct {
    uint8_t font_size;        //字号
    uint8_t bpp;              //位图像素位数
    uint16_t glyph_count;     //包含的字符总数
    uint8_t baseline;
    const mgl_glyph_t *glyphs; //指向字符数组的指针（按码点排序）
    const uint8_t *bmp;       //位图数据
} mgl_font_t;

#define MGL_STR_SIZE(s,size) (s),.font=&mgl_font_##size##px
#define MGL_FMT_SIZE(fmt,size) (fmt)
#define MGL_FMT(fmt) (fmt)

#include "../generated/font/mgl_font_gen.h"

#ifdef __cplusplus
}
#endif
#endif //MGL_FONT_H
