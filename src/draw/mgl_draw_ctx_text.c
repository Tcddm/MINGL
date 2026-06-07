#include "mgl_draw_ctx_text.h"
static void mgl_draw_glyph(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,uint8_t flags,const mgl_glyph_t *glyph,const mgl_font_t *font,const mgl_painter_t *painter) {
    mgl_coord_t px=(mgl_coord_t)(x+glyph->ofs_x);
    mgl_coord_t py=(mgl_coord_t)(y+glyph->ofs_y);
    const uint8_t *bmp=font->bmp+glyph->ofs_bmp;
    uint8_t w=glyph->w;
    uint8_t h=glyph->h;
    uint8_t bpp=font->bpp;
    switch (bpp) {
        case 1:{
            int bytes_per_row =((w + 7)>>3);
            for (int yy=0;yy<h;yy++) {
                for (int xx=0;xx<w;xx++) {
                    int byte_idx=yy*bytes_per_row+(xx>>3);
                    int bit=7-(xx&7);
                    const uint8_t byte=bmp[byte_idx];
                    uint8_t set=(byte>>bit)&1;
                    if(set){
                        mgl_ctx_draw_pixel(ctx,(mgl_coord_t)(px+xx),(mgl_coord_t)(py+yy),painter);
                    }
                }
            }
        }
        break;
        case 8:
            //暂不支持
            break;
        default:
            return;
    }
}
static uint32_t mgl_utf8_decode(const char **str) {
    const uint8_t *s=(const uint8_t*)*str;
    uint32_t code=0;
    if (*s<0x80){
        code=*s++;
    }else if (*s<0xE0){
        code=((s[0]&0x1F)<<6)|(s[1]&0x3F);
        s+=2;
    }else if(*s<0xF0){
        code=((s[0]&0x0F)<<12)|((s[1]&0x3F)<<6)|(s[2]&0x3F);
        s+=3;
    }else{
        code=0xFFFD;
        s+=4;
    }
    *str=(const char*)s;
    return code;
}
static int mgl_glyph_cmp(const void *a,const void *b) {
    const mgl_glyph_t *ga=(const mgl_glyph_t*)a;
    const mgl_glyph_t *gb=(const mgl_glyph_t*)b;
    if (ga->code_point<gb->code_point){return -1;}
    if (ga->code_point>gb->code_point){return 1;}
    return 0;
}
void mgl_ctx_draw_text(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,uint8_t flags,const char *str,const mgl_font_t *font,const mgl_painter_t *painter){
    mgl_coord_t current_x = x;

    while (*str!='\0') {
        uint32_t code = mgl_utf8_decode(&str);
        mgl_glyph_t key={.code_point=code};
        mgl_glyph_t *glyph=binary_search(
                font->glyphs,
                font->glyph_count,
                sizeof(mgl_glyph_t),
                &key,
                mgl_glyph_cmp
        );

        if (glyph!=NULL) {
            mgl_draw_glyph(ctx,current_x,y,flags,glyph,font,painter);
            current_x=(mgl_coord_t)(current_x+glyph->adv);
        }
    }
}


mgl_coord_t mgl_font_get_text_width(const mgl_font_t *font,const char *text){
    mgl_coord_t width=0;
    while(*text){
        uint32_t code=mgl_utf8_decode(&text);
        mgl_glyph_t key={.code_point=code};
        mgl_glyph_t *glyph=binary_search(
                font->glyphs,
                font->glyph_count,
                sizeof(mgl_glyph_t),
                &key,
                mgl_glyph_cmp
        );
        if(glyph){
            width=(mgl_coord_t)(width+glyph->adv);
        }
    }
    return width;
}