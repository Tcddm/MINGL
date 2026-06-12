#ifndef MGL_RECT_H
#define MGL_RECT_H

#include "mgl_types.h"

#ifdef __cplusplus
extern "C"{
#endif

// #region mgl_rect_intersect
/**
 * @brief 计算两个矩形的相交区域
 *
 * @param a 矩形A
 * @param b 矩形B
 * @param[out] out 相交结果矩形
 * @return  是否有交集
 */
bool mgl_rect_intersect(const mgl_rect_t *a,const mgl_rect_t *b,mgl_rect_t *out);
// #endregion

// #region mgl_rect_union
/**
 * @brief 计算两个矩形的并集（包围矩形）
 *
 * 并集定义为同时包含矩形A和矩形B的最小矩形。
 *
 * @param a 矩形A
 * @param b 矩形B
 * @param[out] out 并集结果矩形
 */
void mgl_rect_union(const mgl_rect_t *a,const mgl_rect_t *b,mgl_rect_t *out);
// #endregion

#ifdef __cplusplus
}
#endif
#endif //MGL_RECT_H
