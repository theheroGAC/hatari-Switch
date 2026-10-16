#ifndef HATARI_VKBD_H
#define HATARI_VKBD_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>

void VKBD_Init(void);
void VKBD_Quit(void);
void VKBD_Toggle(void);
bool VKBD_IsOpen(void);
void VKBD_Close(void);
bool VKBD_NeedsRedraw(void);
bool VKBD_HandleInput(uint64_t down, uint64_t held, int stickLX, int stickLY, int stickRX, int stickRY);
bool VKBD_HandleTouch(int touchX, int touchY, bool touchDown);
void VKBD_Render(SDL_Renderer *renderer);

#endif
