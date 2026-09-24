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

static void mgl_draw_glyph(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,uint8_t flags,const mgl_glyph_t *glyph,const mgl_font_t *font,const mgl_painter_t *painter){
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
static uint32_t mgl_utf8_decode(const char **str){
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
static const mgl_glyph_t *mgl_font_find_glyph(const mgl_font_t *font,uint32_t code){
    mgl_glyph_t key={.code_point=code};
    return (const mgl_glyph_t *)binary_search(
            font->glyphs,
            font->glyph_count,
            sizeof(mgl_glyph_t),
            &key,
            mgl_glyph_cmp
    );
}
static mgl_coord_t mgl_text_line_width(const mgl_font_t *font,const char *str){
    mgl_coord_t w=0;
    while(*str&&*str!='\n'){
        uint32_t code=mgl_utf8_decode(&str);
        if(code=='\r'){continue;}
        const mgl_glyph_t *glyph=mgl_font_find_glyph(font,code);
        if(glyph){
            w=(mgl_coord_t)(w+glyph->adv);
        }else{
            w=(mgl_coord_t)(w+font->font_size+1);
        }
    }
    return w;
}
static void mgl_text_draw_line(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,uint8_t flags,const char *str,const mgl_font_t *font,const mgl_painter_t *painter){
    mgl_coord_t current_x=x;
    while(*str&&*str!='\n'){
        uint32_t code=mgl_utf8_decode(&str);
        if(code=='\r'){continue;}
        const mgl_glyph_t *glyph=mgl_font_find_glyph(font,code);
        if(glyph!=NULL){
            mgl_draw_glyph(ctx,current_x,y,flags,glyph,font,painter);
            current_x=(mgl_coord_t)(current_x+glyph->adv);
        }else{
            mgl_coord_t sz=font->font_size;
            mgl_ctx_fill_rect(ctx,current_x,(mgl_coord_t)(y-font->baseline),sz,sz,painter);
            current_x=(mgl_coord_t)(current_x+sz+1);
        }
    }
}
void mgl_ctx_draw_text(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,uint8_t flags,const char *str,const mgl_font_t *font,const mgl_painter_t *painter){
    if(!str||!font){return;}
    mgl_coord_t baseline=y;
    const char *p=str;
    for(;;){
        mgl_text_draw_line(ctx,x,baseline,flags,p,font,painter);
        while(*p&&*p!='\n'){p++;}
        if(!*p){break;}
        p++;
        baseline=(mgl_coord_t)(baseline+font->font_size);
    }
}
void mgl_ctx_draw_text_box(mgl_draw_ctx_t *ctx,const mgl_rect_t *box,mgl_text_align_t align_h,mgl_text_align_t align_v,mgl_coord_t line_spacing,uint8_t flags,const char *str,const mgl_font_t *font,const mgl_painter_t *painter){
    if(!box||!str||!font){return;}

    mgl_coord_t max_w=0;
    mgl_coord_t lines=0;
    const char *p=str;
    for(;;){
        mgl_coord_t w=mgl_text_line_width(font,p);
        if(w>max_w){max_w=w;}
        lines++;
        while(*p&&*p!='\n'){p++;}
        if(!*p){break;}
        p++;
    }

    mgl_coord_t line_advance=(mgl_coord_t)(font->font_size+line_spacing);
    mgl_coord_t total_h=(mgl_coord_t)(font->font_size+(lines-1)*line_advance);

    mgl_coord_t v_off=0;
    if(align_v==MGL_TEXT_ALIGN_CENTER){v_off=(mgl_coord_t)((box->h-total_h)/2);}
    else if(align_v==MGL_TEXT_ALIGN_END){v_off=(mgl_coord_t)(box->h-total_h);}
    if(v_off<0){v_off=0;}

    mgl_coord_t baseline=(mgl_coord_t)(box->y+v_off+font->baseline);
    p=str;
    for(mgl_coord_t i=0;i<lines;i++){
        mgl_coord_t w=mgl_text_line_width(font,p);
        mgl_coord_t h_off=0;
        if(align_h==MGL_TEXT_ALIGN_CENTER){h_off=(mgl_coord_t)((box->w-w)/2);}
        else if(align_h==MGL_TEXT_ALIGN_END){h_off=(mgl_coord_t)(box->w-w);}
        if(h_off<0){h_off=0;}
        mgl_text_draw_line(ctx,(mgl_coord_t)(box->x+h_off),baseline,flags,p,font,painter);
        while(*p&&*p!='\n'){p++;}
        if(!*p){break;}
        p++;
        baseline=(mgl_coord_t)(baseline+line_advance);
    }
}
void mgl_font_get_text_size(const mgl_font_t *font,const char *text,mgl_coord_t line_spacing,mgl_coord_t *out_w,mgl_coord_t *out_h){
    if(out_w){*out_w=0;}
    if(out_h){*out_h=0;}
    if(!font){return;}
    if(!text){
        if(out_h){*out_h=font->font_size;}
        return;
    }
    mgl_coord_t max_w=0,line_w=0;
    mgl_coord_t lines=1;
    while(*text){
        uint32_t code=mgl_utf8_decode(&text);
        if(code=='\n'){
            if(line_w>max_w){max_w=line_w;}
            line_w=0;
            lines++;
            continue;
        }
        if(code=='\r'){continue;}
        const mgl_glyph_t *glyph=mgl_font_find_glyph(font,code);
        if(glyph){
            line_w=(mgl_coord_t)(line_w+glyph->adv);
        }else{
            line_w=(mgl_coord_t)(line_w+font->font_size+1);
        }
    }
    if(line_w>max_w){max_w=line_w;}
    mgl_coord_t line_advance=(mgl_coord_t)(font->font_size+line_spacing);
    if(out_w){*out_w=max_w;}
    if(out_h){*out_h=(mgl_coord_t)(font->font_size+(lines-1)*line_advance);}
}