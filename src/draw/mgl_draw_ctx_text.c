#include "mgl_draw_ctx_text.h"

static void draw_glyph_run(mgl_draw_ctx_t *ctx,
                           mgl_coord_t base_x,mgl_coord_t y,
                           int run_start,int run_end,
                           const mgl_painter_t *painter){
    int w_run=run_end-run_start;
    if(run_start>=0){
        mgl_ctx_fill_rect(ctx,(mgl_coord_t)(base_x+run_start),y,
                          (mgl_coord_t)w_run,1,painter);
    }
}

static int scan_byte_runs(mgl_draw_ctx_t *ctx,
                          mgl_coord_t base_x,mgl_coord_t y,
                          uint8_t byte,int start_bit,int n_bits,int xx,
                          const mgl_painter_t *painter){
    int run_start=-1;
    int end=xx+n_bits;
    int bi=start_bit;
    while(xx<end){
        if((byte >> bi) & 1){
            if(run_start<0){
                run_start=xx;
            }
        }else{
            draw_glyph_run(ctx,base_x,y,run_start,xx,painter);
            run_start=-1;
        }
        xx++;
        bi--;
    }
    draw_glyph_run(ctx,base_x,y,run_start,xx,painter);
    return n_bits;
}

static void draw_glyph_row_1bpp(mgl_draw_ctx_t *ctx,
                                mgl_coord_t base_x,mgl_coord_t y,
                                const uint8_t *row_bmp,int w,
                                const mgl_painter_t *painter){
    int xx=0;
    while(xx<w){
        int byte_idx=xx >> 3;
        int start_bit=7-(xx & 7);
        uint8_t byte=row_bmp[byte_idx];
        int bits_left=w-xx;
        int in_this_byte=(start_bit+1<bits_left)
                           ? start_bit+1 : bits_left;

        if(in_this_byte==8&&byte==0){
            xx+=8;
            continue;
        }
        if(in_this_byte==8&&byte==0xFF){
            draw_glyph_run(ctx,base_x,y,xx,xx+8,painter);
            xx+=8;
            continue;
        }
        scan_byte_runs(ctx,base_x,y,byte,start_bit,in_this_byte,xx,painter);
        xx+=in_this_byte;
    }
}

static void mgl_draw_glyph(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,uint8_t flags,const mgl_glyph_t *glyph,const mgl_font_t *font,const mgl_painter_t *painter) {
    mgl_coord_t px=(mgl_coord_t)(x+glyph->ofs_x);
    mgl_coord_t py=(mgl_coord_t)(y+glyph->ofs_y);
    const uint8_t *bmp=font->bmp+glyph->ofs_bmp;
    uint8_t w=glyph->w;
    uint8_t h=glyph->h;
    uint8_t bpp=font->bpp;
    switch (bpp) {
        case 1:{
            int bytes_per_row=(w + 7) >> 3;
            for(int yy=0;yy<h;yy++){
                draw_glyph_row_1bpp(ctx,px,(mgl_coord_t)(py+yy),
                                    bmp+yy*bytes_per_row,w,painter);
            }
            break;
        }
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

        if(glyph!=NULL){
            mgl_draw_glyph(ctx,current_x,y,flags,glyph,font,painter);
            current_x=(mgl_coord_t)(current_x+glyph->adv);
        }else{
            mgl_coord_t sz=font->font_size;
            mgl_coord_t bx=current_x;
            mgl_coord_t by=(mgl_coord_t)(y-font->baseline);
            mgl_ctx_fill_rect(ctx,bx,by,
                                  sz,
                                  sz,
                                  painter);
            current_x=(mgl_coord_t)(current_x+sz+1);
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
        }else{
            width=(mgl_coord_t)(width+font->font_size);
        }
    }
    return width;
}