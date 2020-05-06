#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include "mouse_helper.h"
#include "RESDL_DragEvent.h"

#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>
#import "ReasonSDL2Window.h"


@implementation ReasonSDL2Window
- (id) initWithSDLWindow:(int)sdlWindowID dragEventType:(int)dragEventType {
  SDL_Window *sdlWindow = SDL_GetWindowFromID(sdlWindowID);
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(sdlWindow, &wmInfo);

  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wdeprecated-declarations"
  class_setSuperclass([ReasonSDL2Window class], [wmInfo.info.cocoa.window class]);
  #pragma clang diagnostic pop
  self = [super init];

  if (self) {
    self.sdlWindowID = sdlWindowID;
    self.dragEventType = dragEventType;
    object_setClass(wmInfo.info.cocoa.window, [ReasonSDL2Window class]);
  }

  return self;
}

+ (id) newWithSDLWindow:(int)sdlWindowID dragEventType:(int)dragEventType {
  return [[ReasonSDL2Window alloc] initWithSDLWindow:sdlWindowID dragEventType:dragEventType];
}

- (void)test {

}

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender {
  [super draggingEntered:sender];


  NSLog(@"Drag enter! SDLWindow ID: %d", self.sdlWindowID);

    /* SDL_Event event;
    SDL_UserEvent userEvent;

    userEvent.type = SDL_USEREVENT;
    userEvent.windowID = self.sdlWindowID;
    int mouseX, mouseY;
    getNonFocusedMousePosition(SDL_GetWindowFromID(self.sdlWindowID), &mouseX, &mouseY);

    RESDL_DragEvent_data *data = (RESDL_DragEvent_data *)malloc(sizeof(RESDL_DragEvent_data));
    data->mouseX = mouseX;
    data->mouseY = mouseY;

    userEvent.data1 = data;
    userEvent.data2 = NULL;
    userEvent.code = self.dragEventType;
    userEvent.timestamp = SDL_GetTicks();

    event.type = SDL_USEREVENT;
    event.user = userEvent;
    SDL_PushEvent(&event);
 */
  return NSDragOperationNone;
}

@end

#endif
