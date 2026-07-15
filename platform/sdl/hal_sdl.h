#ifndef HAL_SDL_H
#define HAL_SDL_H

#include "hal/mgl_hal.h"
#ifdef __cplusplus
extern "C" {
#endif

bool mgl_hal_sdl2_init(const char *title);
void mgl_hal_sdl2_cleanup(void);
void mgl_hal_sdl2_poll_events(bool *quit);

#ifdef __cplusplus
}
#endif

#endif /* HAL_SDL_H */