module Image = Image;
module Float32Array = Float32Array;
module Uint16Array = Uint16Array;

module Size = {
  type t = {
    width: int,
    height: int,
  };
};

module ScreenSaver = {
  external enable: unit => unit = "resdl_SDL_EnableScreenSaver";
  external disable: unit => unit = "resdl_SDL_DisableScreenSaver";
  external isEnabled: unit => bool = "resdl_SDL_IsScreenSaverEnabled";
};

module Surface = {
  type t;
  external createFromImagePath: string => result(t, string) =
    "resdl_SDL_CreateRGBSurfaceFromImage";
};

module Clipboard = {
  external getText: unit => option(string) = "resdl_SDL_GetClipboardText";
  external setText: string => unit = "resdl_SDL_SetClipboardText";
  external hasText: unit => bool = "resdl_SDL_HasClipboardText";
};

module Display = {
  type t;

  module Dpi = {
    type t = {
      ddpi: float,
      hdpi: float,
      vdpi: float,
    };

    let show = (v: t) => {
      Printf.sprintf("ddpi: %f hdpi: %f vdpi: %f", v.ddpi, v.hdpi, v.vdpi);
    };
  };

  module Mode = {
    type t = {
      width: int,
      height: int,
      refreshRate: int,
    };

    let show = (v: t) => {
      Printf.sprintf(
        "width: %d height: %d refreshRate: %d",
        v.width,
        v.height,
        v.refreshRate,
      );
    };
  };

  external getDPI: t => Dpi.t = "resdl_SDL_GetDisplayDPI";

  external getCurrentMode: t => Mode.t = "resdl_SDL_GetCurrentDisplayMode";

  external getDesktopMode: t => Mode.t = "resdl_SDL_GetDesktopDisplayMode";
};

module Window = {
  type t;

  type hitTestResult =
    | Normal
    | Draggable
    | ResizeTopLeft
    | ResizeTop
    | ResizeTopRight
    | ResizeRight
    | ResizeBottomRight
    | ResizeBottom
    | ResizeBottomLeft
    | ResizeLeft;

  type hitTestCallback = (t, int, int) => hitTestResult;

  external create: (int, int, string) => t = "resdl_SDL_CreateWindow";
  external getId: t => int = "resdl_SDL_GetWindowId";
  external getSize: t => Size.t = "resdl_SDL_GetWindowSize";
  external setBordered: (t, bool) => unit = "resdl_SDL_SetWindowBordered";
  external setIcon: (t, Surface.t) => unit = "resdl_SDL_SetWindowIcon";
  external setTransparency: (t, float) => unit =
    "resdl_SDL_SetWindowTransparency";
  external setPosition: (t, int, int) => unit = "resdl_SDL_SetWindowPosition";
  external center: t => unit = "resdl_SDL_WindowCenter";
  external setResizable: (t, bool) => unit = "resdl_SDL_SetWindowResizable";
  external setSize: (t, int, int) => unit = "resdl_SDL_SetWindowSize";
  external setTitle: (t, string) => unit = "resdl_SDL_SetWindowTitle";
  external setMinimumSize: (t, int, int) => unit =
    "resdl_SDL_SetWindowMinimumSize";

  external _enableHitTest: t => unit = "resdl_SDL_EnableHitTest";
  external _disableHitTest: t => unit = "resdl_SDL_EnableHitTest";

  let _idToHitTest: Hashtbl.t(int, hitTestCallback) = Hashtbl.create(16);

  let setHitTest = (win: t, cb: option(hitTestCallback)) => {
    switch (cb) {
    | None => _disableHitTest(win)
    | Some(v) =>
      _enableHitTest(win);
      Hashtbl.add(_idToHitTest, getId(win), v);
    };
  };

  let _hitTest = (win: t, x: int, y: int) => {
    let id = getId(win);
    switch (Hashtbl.find_opt(_idToHitTest, id)) {
    | Some(v) => v(win, x, y)
    | None => Normal
    };
  };

  Callback.register("__sdl2_caml_hittest__", _hitTest);

  external hide: t => unit = "resdl_SDL_HideWindow";
  external raise: t => unit = "resdl_SDL_RaiseWindow";
  external show: t => unit = "resdl_SDL_ShowWindow";

  external minimize: t => unit = "resdl_SDL_MinimizeWindow";
  external restore: t => unit = "resdl_SDL_RestoreWindow";
  external maximize: t => unit = "resdl_SDL_MaximizeWindow";

  external getDisplay: t => Display.t = "resdl_SDL_GetWindowDisplayIndex";

  // Windows-Only: Set DPI Aware process flag
  // Other platforms: no-op
  external setWin32ProcessDPIAware: t => unit =
    "resdl_SDL_SetWin32ProcessDPIAware";

  // WINDOWS-ONLY: Get the monitor scale factor for the window
  // Other platforms: Always returns 1.0
  external getWin32ScaleFactor: t => float = "resdl_SDL_GetWin32ScaleFactor";

  type nativeWindow;
  external getNativeWindow: t => nativeWindow = "resdl_SDL_GetNativeWindow";

  // MacOS-Only
  external setMacTitlebarTransparent: t => unit =
    "resdl_SDL_SetMacTitlebarTransparent";
  external setMacBackgroundColor: (t, float, float, float, float) => unit =
    "resdl_SDL_SetMacBackgroundColor";
};

module OldGl = Gl;
module Gl = {
  type context;

  external setup: Window.t => context = "resdl_SDL_GL_Setup";
  external makeCurrent: (Window.t, context) => unit =
    "resdl_SDL_GL_MakeCurrent";
  external swapWindow: Window.t => unit = "resdl_SDL_GL_SwapWindow";
  external getDrawableSize: Window.t => Size.t =
    "resdl_SDL_GL_GetDrawableSize";
  external setSwapInterval: int => unit = "resdl_SDL_GL_SetSwapInterval";
  include OldGl;
};

external delay: int => unit = "resdl_SDL_Delay";
external init: unit => int = "resdl_SDL_Init";

module TextInput = {
  [@noalloc] external start: unit => unit = "resdl_SDL_StartTextInput";
  [@noalloc] external stop: unit => unit = "resdl_SDL_StopTextInput";
  [@noalloc]
  external setInputRect: (int, int, int, int) => unit =
    "resdl_SDL_SetTextInputRect";
  [@noalloc] external isActive: unit => bool = "resdl_SDL_IsTextInputActive";
};

module MouseButton = {
  type t =
    | Left
    | Middle
    | Right
    | X1
    | X2;

  let show = v =>
    switch (v) {
    | Left => "Left"
    | Middle => "Middle"
    | Right => "Right"
    | X1 => "X1"
    | X2 => "X2"
    };
};

module Scancode = {
  type t = int;

  external getName: t => string = "resdl_SDL_GetScancodeName";

  [@noalloc] external ofInt: int => t = "resdl_PassThrough";
  [@noalloc] external toInt: t => int = "resdl_PassThrough";

  // Incrementally add these as needed from:
  // https://wiki.libsdl.org/SDLScancodeLookup
  let unknown = 0;
};

module Keycode = {
  type t = int;

  external getName: t => string = "resdl_SDL_GetKeyName";

  [@noalloc]
  external ofScancode: Scancode.t => t = "resdl_SDL_GetKeyFromScancode";
  [@noalloc]
  external toScancode: t => Scancode.t = "resdl_SDL_GetScancodeFromKey";

  // Incrementally add these as needed from:
  // https://wiki.libsdl.org/SDLKeycodeLookup
  let unknown = 0;
  let backspace = 8;

  let escape = 27;

  let minus = 45;
  let period = 46;
  let slash = 47;

  let equals = 61;

  let digit0 = 48;
  let digit1 = 49;
  let digit2 = 50;
  let digit3 = 51;
  let digit4 = 52;
  let digit5 = 53;
  let digit6 = 54;
  let digit7 = 55;
  let digit8 = 56;
  let digit9 = 57;

  let c = 99;

  let delete = 127;

  let right = 1073741903;
  let left = 1073741904;
};

module Keymod = {
  type t = int;

  [@noalloc] external isLeftShiftDown: t => bool = "resdl_SDL_ModLeftShift";
  [@noalloc] external isRightShiftDown: t => bool = "resdl_SDL_ModRightShift";

  let isShiftDown = v => isLeftShiftDown(v) || isRightShiftDown(v);

  [@noalloc]
  external isLeftControlDown: t => bool = "resdl_SDL_ModLeftControl";
  [@noalloc]
  external isRightControlDown: t => bool = "resdl_SDL_ModRightControl";

  let isControlDown = v => isLeftControlDown(v) || isRightControlDown(v);

  [@noalloc] external isLeftAltDown: t => bool = "resdl_SDL_ModLeftAlt";
  [@noalloc] external isRightAltDown: t => bool = "resdl_SDL_ModRightAlt";

  let isAltDown = v => isLeftAltDown(v) || isRightAltDown(v);

  [@noalloc] external isLeftGuiDown: t => bool = "resdl_SDL_ModLeftGui";
  [@noalloc] external isRightGuiDown: t => bool = "resdl_SDL_ModRightGui";

  let isGuiDown = v => isLeftGuiDown(v) || isRightGuiDown(v);

  [@noalloc] external isNumLockDown: t => bool = "resdl_SDL_ModNumLockDown";
  [@noalloc] external isCapsLockDown: t => bool = "resdl_SDL_ModCapsLockDown";

  [@noalloc] external isAltGrKeyDown: t => bool = "resdl_SDL_ModAltGrDown";

  let show = (v: t) => {
    let int_of_bool = b => b ? 1 : 0;

    Printf.sprintf(
      "Keymods - LSHIFT: %d RSHIFT: %d LCTRL: %d RCTRL: %d LALT: %d RALT: %d LGUI: %d RGUI: %d NUM: %d CAPS: %d ALTGR: %d",
      int_of_bool(isLeftShiftDown(v)),
      int_of_bool(isRightShiftDown(v)),
      int_of_bool(isLeftControlDown(v)),
      int_of_bool(isRightControlDown(v)),
      int_of_bool(isLeftAltDown(v)),
      int_of_bool(isRightAltDown(v)),
      int_of_bool(isLeftGuiDown(v)),
      int_of_bool(isRightGuiDown(v)),
      int_of_bool(isNumLockDown(v)),
      int_of_bool(isCapsLockDown(v)),
      int_of_bool(isAltGrKeyDown(v)),
    );
  };

  [@noalloc] external getState: unit => t = "resdl_SDL_GetModState";
};

module Event = {
  type mouseMotion = {
    windowID: int,
    x: int,
    y: int,
  };

  type mouseWheel = {
    windowID: int,
    deltaX: int,
    deltaY: int,
    isFlipped: bool,
  };

  type mouseButtonEvent = {
    windowID: int,
    button: MouseButton.t,
    clicks: int,
    x: int,
    y: int,
  };

  type keyboardEvent = {
    windowID: int,
    repeat: bool,
    keymod: Keymod.t,
    scancode: Scancode.t,
    keycode: Keycode.t,
  };

  type textInputEvent = {
    windowID: int,
    text: string,
  };

  type textEditingEvent = {
    windowID: int,
    text: string,
    start: int,
    length: int,
  };

  type windowEvent = {windowID: int};

  type windowMoveEvent = {
    windowID: int,
    x: int,
    y: int,
  };

  type windowSizeEvent = {
    windowID: int,
    width: int,
    height: int,
  };

  type t =
    | Quit
    | MouseMotion(mouseMotion) // 0
    | MouseWheel(mouseWheel) // 1
    | MouseButtonDown(mouseButtonEvent) // 2
    | MouseButtonUp(mouseButtonEvent) // 3
    | KeyDown(keyboardEvent) // 4
    | KeyUp(keyboardEvent) // 5
    | TextInput(textInputEvent) // 6
    | TextEditing(textEditingEvent) // 7
    | WindowShown(windowEvent) // 8
    | WindowHidden(windowEvent) // 9
    | WindowExposed(windowEvent) // 10
    | WindowMoved(windowMoveEvent) // 11
    | WindowResized(windowSizeEvent) // 12
    | WindowSizeChanged(windowSizeEvent) //12
    | WindowMinimized(windowEvent)
    | WindowMaximized(windowEvent)
    | WindowRestored(windowEvent)
    | WindowEnter(windowEvent)
    | WindowLeave(windowEvent)
    | WindowFocusGained(windowEvent)
    | WindowFocusLost(windowEvent)
    | WindowClosed(windowEvent)
    | WindowTakeFocus(windowEvent)
    | WindowHitTest(windowEvent)
    // An event that hasn't been implemented yet
    | Unknown;

  let show = (v: t) => {
    switch (v) {
    | Quit => "Quit"
    | MouseMotion({windowID, x, y}) =>
      Printf.sprintf("MouseMotion windowId: %d x: %d y: %d", windowID, x, y)
    | MouseWheel({windowID, deltaX, deltaY, isFlipped}) =>
      Printf.sprintf(
        "MouseWheel windowId: %d x: %d y: %d isFlipped: %d",
        windowID,
        deltaX,
        deltaY,
        isFlipped ? 1 : 0,
      )
    | MouseButtonUp({windowID, button, _}) =>
      Printf.sprintf(
        "MouseButtonUp windowId: %d button: %s",
        windowID,
        MouseButton.show(button),
      )
    | MouseButtonDown({windowID, button, _}) =>
      Printf.sprintf(
        "MouseButtonDown windowId: %d button: %s",
        windowID,
        MouseButton.show(button),
      )
    | KeyDown({repeat, keymod, scancode, keycode, _}) =>
      Printf.sprintf(
        "KeyDown repeat %d:\n -- %s\n -- Scancode: %s\n -- Keycode: %s\n",
        repeat ? 1 : 0,
        Keymod.show(keymod),
        Scancode.getName(scancode),
        Keycode.getName(keycode),
      )
    | KeyUp({repeat, keymod, scancode, keycode, _}) =>
      Printf.sprintf(
        "KeyUp repeat %d:\n -- %s\n -- Scancode: %s\n -- Keycode: %s\n",
        repeat ? 1 : 0,
        Keymod.show(keymod),
        Scancode.getName(scancode),
        Keycode.getName(keycode),
      )
    | TextEditing({text, start, length, _}) =>
      Printf.sprintf(
        "TextEditing:\n -- start: %d length: %d text: %s\n",
        start,
        length,
        text,
      )
    | TextInput({text, _}) =>
      Printf.sprintf("TextInput:\n -- text: %s\n", text)
    | WindowShown({windowID}) =>
      Printf.sprintf("WindowShown: %d\n", windowID)
    | WindowHidden({windowID}) =>
      Printf.sprintf("WindowHidden: %d\n", windowID)
    | WindowExposed({windowID}) =>
      Printf.sprintf("WindowExposed: %d\n", windowID)
    | WindowMoved({windowID, x, y}) =>
      Printf.sprintf(
        "WindowMoved - windowID: %d x: %d y: %d\n",
        windowID,
        x,
        y,
      )
    | WindowResized({windowID, width, height}) =>
      Printf.sprintf(
        "WindowResized - windowID: %d x: %d y: %d\n",
        windowID,
        width,
        height,
      )
    | WindowSizeChanged({windowID, width, height}) =>
      Printf.sprintf(
        "WindowSizeChanged - windowID: %d x: %d y: %d\n",
        windowID,
        width,
        height,
      )
    | WindowMinimized({windowID}) =>
      Printf.sprintf("WindowMinimized: %d\n", windowID)
    | WindowMaximized({windowID}) =>
      Printf.sprintf("WindowMaximized: %d\n", windowID)
    | WindowRestored({windowID}) =>
      Printf.sprintf("WindowRestored: %d\n", windowID)
    | WindowEnter({windowID}) =>
      Printf.sprintf("WindowEnter: %d\n", windowID)
    | WindowLeave({windowID}) =>
      Printf.sprintf("WindowLeave: %d\n", windowID)
    | WindowFocusGained({windowID}) =>
      Printf.sprintf("WindowFocusGained: %d\n", windowID)
    | WindowFocusLost({windowID}) =>
      Printf.sprintf("WindowFocusLost: %d\n", windowID)
    | WindowClosed({windowID}) =>
      Printf.sprintf("WindowClosed: %d\n", windowID)
    | WindowTakeFocus({windowID}) =>
      Printf.sprintf("WindowTakeFocus: %d\n", windowID)
    | WindowHitTest({windowID}) =>
      Printf.sprintf("WindowHitTest: %d\n", windowID)
    | Unknown => "Unknown"
    };
  };

  external poll: unit => option(t) = "resdl_SDL_PollEvent";
  external push: unit => unit = "resdl_SDL_PushEvent";
  external wait: unit => result(t, string) = "resdl_SDL_WaitEvent";
  external waitTimeout: int => option(t) = "resdl_SDL_WaitTimeoutEvent";
};

module Cursor = {
  type systemCursor =
    | Arrow
    | IBeam
    | Wait
    | Crosshair
    | WaitArrow
    | SizeNWSE
    | SizeNESW
    | SizeWE
    | SizeNS
    | SizeAll
    | No
    | Hand;

  type t;

  external createSystem: systemCursor => t = "resdl_SDL_CreateSystemCursor";
  external set: t => unit = "resdl_SDL_SetCursor";
};

module MessageBox = {
  type flags =
    | Error
    | Warning
    | Information;

  external showSimple: (flags, string, string, option(Window.t)) => unit =
    "resdl_SDL_ShowSimpleMessageBox";
};

type renderFunction = unit => bool;
external _javaScriptRenderLoop: renderFunction => unit =
  "resdl__javascript__renderloop";

let _nativeLoop = renderFn => {
  while (!renderFn()) {
    ();
      /*Thread.yield();*/
  };
  ();
};

let renderLoop = (renderFunction: renderFunction) => {
  switch (Sys.backend_type) {
  | Native => _nativeLoop(renderFunction)
  | Bytecode => _nativeLoop(renderFunction)
  | _ => _javaScriptRenderLoop(renderFunction)
  };
};
