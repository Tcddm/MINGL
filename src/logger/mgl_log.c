#include "mgl_log.h"
#include <stdio.h>
#include <stdarg.h>

static mgl_log_output_t g_output=NULL;

void mgl_log_set_output(mgl_log_output_t output) {
    g_output=output;
}

void mgl_log_write(const char *tag,mgl_log_level_t level,const char *fmt,...) {

    char buf[128];
    va_list args;
    va_start(args,fmt);
    int len=vsnprintf(buf,sizeof(buf),fmt,args);
    va_end(args);
    if(len<=0){ return;}

    const char *level_str;
    switch (level) {
        case MGL_LOG_LEVEL_DBG:   level_str = "DBG";   break;
        case MGL_LOG_LEVEL_INFO:  level_str = "INFO";  break;
        case MGL_LOG_LEVEL_ERROR: level_str = "ERROR"; break;
        default:                  level_str = "?";     break;
    }


    char out[256];
    snprintf(out,sizeof(out),"[%s] [%s] %s\r\n",level_str,tag,buf);

    if (g_output) {
        g_output(out);
    }else{
        printf("%s", out);
    }
}