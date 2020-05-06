#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include "mouse_helper.h"
#include "RESDL_DragEvent.h"

#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>
#import "ReasonSDL2Window.h"


@implementation ReasonSDL2Window
- (id) initWithSDLWindow:(SDL_Window *)sdlWindow dragEventType:(int)dragEventType {
  self = [super init];
  if (self) {
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(sdlWindow, &wmInfo);

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    class_setSuperclass([ReasonSDL2Window class], [wmInfo.info.cocoa.window class]);
    #pragma clang diagnostic pop

    object_setClass(wmInfo.info.cocoa.window, [ReasonSDL2Window class]);
    _sdlWindow = sdlWindow;
    _dragEventType = dragEventType;
  }

  return self;
}

+ (id) newWithSDLWindow:(SDL_Window *)sdlWindow dragEventType:(int)dragEventType {
  return [[ReasonSDL2Window alloc] initWithSDLWindow:sdlWindow dragEventType:dragEventType];
}

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender {
  [super draggingEntered:sender];

    SDL_Event event;
    SDL_zero(event);
    event.type = _dragEventType;

    int mouseX, mouseY;
    getNonFocusedMousePosition(_sdlWindow, &mouseX, &mouseY);

    RESDL_DragEvent_data *data = (RESDL_DragEvent_data *)malloc(sizeof(RESDL_DragEvent_data));
    data->mouseX = mouseX;
    data->mouseY = mouseY;

    event.user.data1 = data;
    SDL_PushEvent(&event);



  return NSDragOperationNone;
}

@end

#endif
