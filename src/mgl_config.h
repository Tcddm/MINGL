#ifndef MGL_CONFIG_H
#define MGL_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define MGL_VERSION_STRING "v1.0-beta.3"

#include "mgl_config_gen.h"

#define MGL_FRAMEBUFFER (MGL_FRAMEBUFFER_INTERNAL || MGL_FRAMEBUFFER_EXTERNAL)

#ifdef __cplusplus
}
#endif
#endif //MGL_CONFIG_H
