#ifndef MGL_LOG_H
#define MGL_LOG_H

#ifdef __cplusplus
extern "C"{
#endif

// #region MGL_LOG_TAG
#define MGL_LOG_TAG_CORE "Core"
#define MGL_LOG_TAG_PAGE_POOL "Page Pool"
#define MGL_LOG_TAG_PAGE "Page"
#define MGL_LOG_TAG_DRAW "Draw"
#define MGL_LOG_TAG_WIDGET(name) "Widget - " #name
#define MGL_LOG_TAG_RENDER "Render"
#define MGL_LOG_TAG_EVENT "Event"
// #endregion

typedef enum{
    MGL_LOG_LEVEL_DBG,
    MGL_LOG_LEVEL_INFO,
    MGL_LOG_LEVEL_ERROR
} mgl_log_level_t;

typedef void (*mgl_log_output_t)(const char *msg);
// #region mgl_log_set_output
void mgl_log_set_output(mgl_log_output_t output);
// #endregion

// #region mgl_log_write
void mgl_log_write(const char *tag,mgl_log_level_t level,const char *fmt,...);
// #endregion

#if MGL_LOG_ENABLE_DBG
#define MGL_LOG_DBG(tag,fmt,...)  mgl_log_write(tag,MGL_LOG_LEVEL_DBG,fmt,##__VA_ARGS__)
#else
#define MGL_LOG_DBG(tag,fmt,...) ((void)0)
#endif

#if MGL_LOG_ENABLE_INFO
#define MGL_LOG_INFO(tag,fmt,...) mgl_log_write(tag,MGL_LOG_LEVEL_INFO,fmt,##__VA_ARGS__)
#else
#define MGL_LOG_INFO(tag,fmt,...) ((void)0)
#endif

#if MGL_LOG_ENABLE_ERROR
#define MGL_LOG_ERROR(tag,fmt,...) mgl_log_write(tag,MGL_LOG_LEVEL_ERROR,fmt,##__VA_ARGS__)
#else
#define MGL_LOG_ERROR(tag,fmt,...) ((void)0)
#endif

#ifdef __cplusplus
}
#endif
#endif //MGL_LOG_H
