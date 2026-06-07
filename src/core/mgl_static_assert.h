#ifndef MGL_STATIC_ASSERT_H
#define MGL_STATIC_ASSERT_H

#ifdef __cplusplus
extern "C"{
#endif

//编译期静态断言
#define MGL_STATIC_ASSERT(expr)  typedef char mgl_static_assert_##__LINE__[(expr) ? 1 : -1]

#define MGL_GUARD_CHECK(expr) do { if (!(expr)) return; } while(0)
#define MGL_GUARD_CHECK_RET(expr,r)  do{ if(!(expr)) { return (r); } }while(0)

#ifdef DEBUG
#define MGL_CHECK(expr)        do{ if(!(expr)) { return; } }while(0)
#define MGL_CHECK_RET(expr,r)  do{ if(!(expr)) { return (r); } }while(0)
#else
#define MGL_CHECK(expr)        (void)0
#define MGL_CHECK_RET(expr,r)  (void)0
#endif

#ifdef __cplusplus
}
#endif
#endif