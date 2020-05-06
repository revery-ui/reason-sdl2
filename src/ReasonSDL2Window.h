#ifdef __APPLE__

@interface ReasonSDL2Window : NSWindow <NSDraggingDestination>

@property (nonatomic) int sdlWindowID;
@property (nonatomic) int dragEventType;

+ (id) newWithSDLWindow:(int)sdlWindowID dragEventType:(int)dragEventType;
- (id) initWithSDLWindow:(int)sdlWindowID dragEventType:(int)dragEventType;
- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender;

@end

#endif
