#ifdef __APPLE__

@interface ReasonSDL2Window : NSWindow <NSDraggingDestination>

@property(nonatomic) SDL_Window *sdlWindow;
@property(nonatomic) int dragEventType;

+ (id)newWithSDLWindow:(SDL_Window *)sdlWindow dragEventType:(int)dragEventType;
- (id) initWithSDLWindow:(SDL_Window *)sdlWindow dragEventType:(int)dragEventType;
- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender;

@end

#endif
