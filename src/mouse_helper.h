#ifndef MOUSE_HELPER_H
#define MOUSE_HELPER_H

inline void getNonFocusedMousePosition(SDL_Window *window, int *localMouseX, int *localMouseY) {
  int globalMouseX, globalMouseY;
  SDL_GetGlobalMouseState(&globalMouseX, &globalMouseY);
  int windowX, windowY;
  SDL_GetWindowPosition(window, &windowX, &windowY);
  *localMouseX = globalMouseX - windowX;
  *localMouseY = globalMouseY - windowY;
}

#endif
