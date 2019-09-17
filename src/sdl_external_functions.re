module Setup = {
  external resdl_internal_init: (
    bool,
    bool,
    bool,
    bool,
    bool,
    bool,
    bool,
    bool) => int = "resdl_init_bytecode"
                   "resdl_init_native";

  let resdl_init = (
    ~audio: bool=false,
    ~video: bool=true,
    ~events: bool=true,
    ~haptic: bool=false,
    ~timer: bool=false,
    ~gamecontroller: bool=false,
    ~joystick: bool=false,
    ~everything: bool=false,
    ()
  ) : int => {
    let ret:int = resdl_internal_init(audio, video, events, haptic, timer, gamecontroller, joystick, everything);

    ret
  }

  external resdl_setMainReady: unit => unit = "resdl_SDL_SetMainReady()";
}

module Teardown = {
  external resdl_quit: unit => unit = "resdl_quit";
}

module Status = {
  external resdl_wasinit: int => int = "resdl_wasinit";
}

module Windowing = {
  // TODO: figure out how we want to handle flags here.
  // note: doing a bitwise or on a [value] and then converting to
  // an int should be identical to the bitwise or on the C/++ uint32 type for this case
  external resdl_SDL_CreateWindow: (
    ~window_title: string,
    ~x_window_position: int,
    ~y_window_position: int,
    ~window_width: int,
    ~window_height: int,
    ~windowFlags: int
  ) => Sdl_types.Resdl_t_windowing.window = "resdl_SDL_CreateWindow_bytecode"
                                  "resdl_SDL_CreateWindow";

  external resdl_SDL_DestroyWindow: Sdl_types.Resdl_t_windowing.window => unit = "resdl_SDL_DestroyWindow";
}

