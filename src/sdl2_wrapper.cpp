#include <cstdint>
#include <cstdio>

#include <algorithm>
#include <array>

#include <caml/alloc.h>
#include <caml/bigarray.h>
#include <caml/callback.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/threads.h>

#include <glad/glad.h>

#include "stb_image.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#endif

#ifdef WIN32
#include <Windows.h>
#endif

#define Val_none Val_int(0)
static value Val_some(value v) {
  CAMLparam1(v);
  CAMLlocal1(some);
  some = caml_alloc(1, 0);
  Store_field(some, 0, v);
  CAMLreturn(some);
}

static value Val_ok(value v) {
  CAMLparam1(v);
  CAMLlocal1(some);
  some = caml_alloc(1, 0);
  Store_field(some, 0, v);
  CAMLreturn(some);
}

static value Val_error(value v) {
  CAMLparam1(v);
  CAMLlocal1(some);
  some = caml_alloc(1, 1);
  Store_field(some, 0, v);
  CAMLreturn(some);
}

extern "C" {
CAMLprim value resdl_SDL_EnableScreenSaver() {
  CAMLparam0();
  SDL_EnableScreenSaver();
  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_DisableScreenSaver() {
  CAMLparam0();
  SDL_DisableScreenSaver();
  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_IsScreenSaverEnabled() {
  CAMLparam0();
  int result = SDL_IsScreenSaverEnabled() == SDL_TRUE;
  CAMLreturn(Val_int(result));
}

CAMLprim value resdl_SDL_SetMainReady() {
  SDL_SetMainReady();

  return Val_unit;
}
CAMLprim value resdl_SDL_DestroyWindow(value vWin) {
  SDL_Window *win = (SDL_Window *)vWin;
  SDL_DestroyWindow(win);
  return Val_unit;
}

SDL_HitTestResult resdl_hit_test(SDL_Window *win, const SDL_Point *area,
                                 void *data) {

  static const value *hitTestCallback = NULL;
  if (hitTestCallback == NULL) {
    hitTestCallback = caml_named_value("__sdl2_caml_hittest__");
  }
  value vWin = (value)win;
  value vRet = caml_callback3(*hitTestCallback, vWin, Val_int(area->x),
                              Val_int(area->y));
  SDL_HitTestResult result;
  switch (Int_val(vRet)) {
  case 0:
    result = SDL_HITTEST_NORMAL;
    break;
  case 1:
    result = SDL_HITTEST_DRAGGABLE;
    break;
  case 2:
    result = SDL_HITTEST_RESIZE_TOPLEFT;
    break;
  case 3:
    result = SDL_HITTEST_RESIZE_TOP;
    break;
  case 4:
    result = SDL_HITTEST_RESIZE_TOPRIGHT;
    break;
  case 5:
    result = SDL_HITTEST_RESIZE_RIGHT;
    break;
  case 6:
    result = SDL_HITTEST_RESIZE_BOTTOMRIGHT;
    break;
  case 7:
    result = SDL_HITTEST_RESIZE_BOTTOM;
    break;
  case 8:
    result = SDL_HITTEST_RESIZE_BOTTOMLEFT;
    break;
  case 9:
    result = SDL_HITTEST_RESIZE_LEFT;
    break;
  default:
    result = SDL_HITTEST_NORMAL;
    break;
  }

  return result;
};

CAMLprim value resdl_SDL_EnableHitTest(value vWin) {
  SDL_Window *win = (SDL_Window *)vWin;
  SDL_SetWindowHitTest(win, resdl_hit_test, NULL);
  return Val_unit;
}

CAMLprim value resdl_SDL_DisableHitTest(value vWin) {
  SDL_Window *win = (SDL_Window *)vWin;
  SDL_SetWindowHitTest(win, NULL, NULL);
  return Val_unit;
}

CAMLprim value resdl_SDL_Delay(value delay) {
  CAMLparam1(delay);

  int d = Int_val(delay);
  SDL_Delay(d);
  CAMLreturn(Val_unit);
}

#ifdef WIN32

typedef enum PROCESS_DPI_AWARENESS {
  PROCESS_DPI_UNAWARE = 0,
  PROCESS_SYSTEM_DPI_AWARE = 1,
  PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;

HWND getHWNDFromSDLWindow(SDL_Window *win) {
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(win, &wmInfo);
  return wmInfo.info.win.window;
};

#endif

CAMLprim value resdl_SDL_GetPlatform() {
  CAMLparam0();
  CAMLlocal1(ret);

  const char *str = SDL_GetPlatform();
  ret = caml_copy_string(str);

  CAMLreturn(ret);
}

CAMLprim value resdl_SDL_GetVersion() {
  CAMLparam0();
  CAMLlocal1(ret);

  const char *str;
#ifdef __APPLE__
  NSProcessInfo *pInfo = [NSProcessInfo processInfo];
  NSString *version = [pInfo operatingSystemVersionString];
  str = [version UTF8String];
#else
  str = "??.??.??";
#endif
  ret = caml_copy_string(str);

  CAMLreturn(ret);
}

CAMLprim value resdl_SDL_GetNativeWindow(value vWin) {
  CAMLparam1(vWin);

  SDL_Window *win = (SDL_Window *)vWin;
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(win, &wmInfo);

  void *pNativeWindow = NULL;
  switch (wmInfo.subsystem) {
#ifdef WIN32
  case SDL_SYSWM_WINDOWS:
    pNativeWindow = (void *)wmInfo.info.win.window;
    break;
#elif __APPLE__
  case SDL_SYSWM_COCOA:
    pNativeWindow = (void *)wmInfo.info.cocoa.window;
    break;
#else
  case SDL_SYSWM_X11:
    pNativeWindow = (void *)wmInfo.info.x11.window;
    break;
    // TODO: Do we need a compilation flag to enable wayland support?
    /*
    case SDL_SYSWM_WAYLAND:
      pNativeWindow = (void *)wmInfo.info.wl.surface;
      break;
    */
#endif
  default:
    break;
  }

  CAMLreturn((value)pNativeWindow);
};

CAMLprim value resdl_SDL_WinAttachConsole() {
  CAMLparam0();
  int ret = 0;
#ifdef WIN32
  ret = AttachConsole(ATTACH_PARENT_PROCESS);
  FILE *fDummy;
  freopen_s(&fDummy, "CONIN$", "r", stdin);
  freopen_s(&fDummy, "CONOUT$", "w", stderr);
  freopen_s(&fDummy, "CONOUT$", "w", stdout);
#endif
  CAMLreturn(Val_int(ret));
}

CAMLprim value resdl_SDL_WinAllocConsole() {
  CAMLparam0();
  int ret = 0;
#ifdef WIN32
  ret = AllocConsole();
  FILE *fDummy;
  freopen_s(&fDummy, "CONIN$", "r", stdin);
  freopen_s(&fDummy, "CONOUT$", "w", stderr);
  freopen_s(&fDummy, "CONOUT$", "w", stdout);
#endif
  CAMLreturn(Val_int(ret));
}

CAMLprim value resdl_SDL_SetMacTitlebarTransparent(value vWin) {
  CAMLparam1(vWin);

#ifdef __APPLE__
  SDL_Window *win = (SDL_Window *)vWin;
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(win, &wmInfo);
  NSWindow *nWindow = wmInfo.info.cocoa.window;
  [nWindow
      setStyleMask:[nWindow styleMask] | NSWindowStyleMaskFullSizeContentView];
  [nWindow setTitlebarAppearsTransparent:YES];
#endif

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_SetMacBackgroundColor(value vWin, value r, value g,
                                               value b, value a) {
  CAMLparam5(vWin, r, g, b, a);

#ifdef __APPLE__
  SDL_Window *win = (SDL_Window *)vWin;
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(win, &wmInfo);
  double red = Double_val(r);
  double green = Double_val(g);
  double blue = Double_val(b);
  double alpha = Double_val(a);
  NSWindow *nWindow = wmInfo.info.cocoa.window;
  NSColor *rgb =
      [NSColor colorWithDeviceRed:red green:green blue:blue alpha:alpha];
  [nWindow setBackgroundColor:rgb];
#endif

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_SetWin32ProcessDPIAware(value vWin) {
  CAMLparam1(vWin);

#ifdef WIN32
  void *userDLL;
  BOOL(WINAPI * SetProcessDPIAware)(void); // Vista and later
  void *shcoreDLL;
  HRESULT(WINAPI * SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS dpiAwareness);

  userDLL = SDL_LoadObject("USER32.DLL");
  if (userDLL) {
    SetProcessDPIAware =
        (BOOL(WINAPI *)(void))SDL_LoadFunction(userDLL, "SetProcessDPIAware");
  }

  shcoreDLL = SDL_LoadObject("SHCORE.DLL");
  if (shcoreDLL) {
    SetProcessDpiAwareness =
        (HRESULT(WINAPI *)(PROCESS_DPI_AWARENESS))SDL_LoadFunction(
            shcoreDLL, "SetProcessDpiAwareness");
  }

  if (SetProcessDpiAwareness) {
    // Try Windows 8.1+ version
    HRESULT result = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
  } else if (SetProcessDPIAware) {
    // Try Vista - Windows 8 version.
    // This has a constant scale factor for all monitors.
    BOOL success = SetProcessDPIAware();
  }
#endif

  CAMLreturn(Val_unit);
};

CAMLprim value resdl_SDL_GetWin32ScaleFactor(value vWin) {
  CAMLparam1(vWin);

#ifdef WIN32
  SDL_Window *win = (SDL_Window *)vWin;
  HWND hwnd = getHWNDFromSDLWindow(win);
  HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);

  void *shcoreDLL;
  HRESULT(WINAPI * GetScaleFactorForMonitor)(HMONITOR hmon, int *pScale);

  shcoreDLL = SDL_LoadObject("SHCORE.DLL");
  if (shcoreDLL) {
    GetScaleFactorForMonitor =
        (HRESULT(WINAPI *)(HMONITOR, int *))SDL_LoadFunction(
            shcoreDLL, "GetScaleFactorForMonitor");
  }

  if (GetScaleFactorForMonitor) {
    int pScale;
    GetScaleFactorForMonitor(hmon, &pScale);
    CAMLreturn(caml_copy_double(pScale / 100.0));
  } else {
    CAMLreturn(caml_copy_double(1.0));
  }

#else
  CAMLreturn(caml_copy_double(1.0));
#endif
};

CAMLprim value resdl_SDL_GetDisplayDPI(value vDisplay) {
  CAMLparam1(vDisplay);
  CAMLlocal1(ret);
  int displayIndex = Int_val(vDisplay);

  float ddpi, hdpi, vdpi;
  SDL_GetDisplayDPI(displayIndex, &ddpi, &hdpi, &vdpi);

  ret = caml_alloc(3, 0);
  Store_double_field(ret, 0, ddpi);
  Store_double_field(ret, 1, hdpi);
  Store_double_field(ret, 2, vdpi);
  CAMLreturn(ret);
};

CAMLprim value resdl_SDL_GetCurrentDisplayMode(value vDisplay) {
  CAMLparam1(vDisplay);
  CAMLlocal1(ret);

  int displayIndex = Int_val(vDisplay);
  SDL_DisplayMode current;

  SDL_GetCurrentDisplayMode(displayIndex, &current);

  ret = caml_alloc(3, 0);
  Store_field(ret, 0, current.w);
  Store_field(ret, 1, current.h);
  Store_field(ret, 2, current.refresh_rate);
  CAMLreturn(ret);
};

CAMLprim value resdl_SDL_GetDesktopDisplayMode(value vDisplay) {
  CAMLparam1(vDisplay);
  CAMLlocal1(ret);

  int displayIndex = Int_val(vDisplay);
  SDL_DisplayMode current;

  SDL_GetDesktopDisplayMode(displayIndex, &current);

  ret = caml_alloc(3, 0);
  Store_field(ret, 0, current.w);
  Store_field(ret, 1, current.h);
  Store_field(ret, 2, current.refresh_rate);
  CAMLreturn(ret);
};

CAMLprim value resdl_SDL_GetWindowDisplayIndex(value w) {
  CAMLparam1(w);
  SDL_Window *win = (SDL_Window *)w;
  int idx = SDL_GetWindowDisplayIndex(win);
  CAMLreturn(Val_int(idx));
};

CAMLprim value resdl_SDL_GL_SetSwapInterval(value vInterval) {
  int interval = Int_val(vInterval);
  SDL_GL_SetSwapInterval(interval);
  return Val_unit;
};

CAMLprim value resdl_SDL_GL_Setup(value w) {
  SDL_Window *win = (SDL_Window *)w;
  SDL_GLContext ctx = SDL_GL_CreateContext(win);

  gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress);
  return (value)ctx;
}

CAMLprim value resdl_SDL_GL_MakeCurrent(value vWindow, value vContext) {
  CAMLparam2(vWindow, vContext);
  SDL_Window *win = (SDL_Window *)vWindow;
  SDL_GLContext ctx = (SDL_GLContext)vContext;

  SDL_GL_MakeCurrent(win, ctx);
  CAMLreturn(Val_unit);
};

CAMLprim value resdl_SDL_GetClipboardText(value vUnit) {
  CAMLparam0();
  CAMLlocal1(ret);

  char *clip = SDL_GetClipboardText();
  if (clip == NULL) {
    ret = Val_none;
  } else {
    ret = Val_some(caml_copy_string(clip));
  }

  CAMLreturn(ret);
}

CAMLprim value resdl_SDL_HasClipboardText(value vUnit) {
  CAMLparam0();
  SDL_bool res = SDL_HasClipboardText();
  CAMLreturn(res == SDL_TRUE ? Val_bool(1) : Val_bool(0));
}

CAMLprim value resdl_SDL_SetClipboardText(value vText) {
  CAMLparam1(vText);
  SDL_SetClipboardText(String_val(vText));
  CAMLreturn(Val_unit);
}

CAMLprim value Val_SDL_WindowEvent(int type, int windowID) {
  CAMLparam0();
  CAMLlocal2(ret, v);

  v = caml_alloc(1, 0);
  Store_field(v, 0, Val_int(windowID));

  ret = caml_alloc(1, type);
  Store_field(ret, 0, v);

  CAMLreturn(ret);
}

CAMLprim value Val_SDL_WindowEventWithData(int type, int windowID, int data1,
                                           int data2) {
  CAMLparam0();
  CAMLlocal2(ret, v);

  v = caml_alloc(3, 0);
  Store_field(v, 0, Val_int(windowID));
  Store_field(v, 1, Val_int(data1));
  Store_field(v, 2, Val_int(data2));

  ret = caml_alloc(1, type);
  Store_field(ret, 0, v);

  CAMLreturn(ret);
}

CAMLprim value Val_SDL_Event(SDL_Event *event) {
  CAMLparam0();
  CAMLlocal2(v, vInner);

  int tag, mouseButton;

  switch (event->type) {
  case SDL_QUIT:
    v = Val_int(0);
    break;
  case SDL_MOUSEMOTION:
    vInner = caml_alloc(3, 0);
    Store_field(vInner, 0, Val_int(event->motion.windowID));
    Store_field(vInner, 1, Val_int(event->motion.x));
    Store_field(vInner, 2, Val_int(event->motion.y));

    v = caml_alloc(1, 0);
    Store_field(v, 0, vInner);
    break;
  case SDL_MOUSEWHEEL:
    v = caml_alloc(1, 1);

    vInner = caml_alloc(4, 0);
    Store_field(vInner, 0, Val_int(event->wheel.windowID));
    Store_field(vInner, 1, Val_int(event->wheel.x));
    Store_field(vInner, 2, Val_int(event->wheel.y));
    Store_field(vInner, 3,
                Val_bool(event->wheel.direction == SDL_MOUSEWHEEL_FLIPPED));

    Store_field(v, 0, vInner);
    break;
  case SDL_MOUSEBUTTONUP:
  case SDL_MOUSEBUTTONDOWN:
    if (event->type == SDL_MOUSEBUTTONDOWN)
      v = caml_alloc(1, 2);
    else
      v = caml_alloc(1, 3);

    mouseButton = 0;
    switch (event->button.button) {
    case SDL_BUTTON_LEFT:
      mouseButton = 0;
      break;
    case SDL_BUTTON_MIDDLE:
      mouseButton = 1;
      break;
    case SDL_BUTTON_RIGHT:
      mouseButton = 2;
      break;
    case SDL_BUTTON_X1:
      mouseButton = 3;
      break;
    case SDL_BUTTON_X2:
      mouseButton = 4;
      break;
    default:
      mouseButton = 0;
      break;
    }

    vInner = caml_alloc(5, 0);
    Store_field(vInner, 0, Val_int(event->button.windowID));
    Store_field(vInner, 1, Val_int(mouseButton));
    Store_field(vInner, 2, Val_int(event->button.clicks));
    Store_field(vInner, 3, Val_int(event->button.x));
    Store_field(vInner, 4, Val_int(event->button.y));

    Store_field(v, 0, vInner);
    break;
  case SDL_KEYDOWN:
  case SDL_KEYUP:
    v = caml_alloc(1, event->type == SDL_KEYDOWN ? 4 : 5);

    vInner = caml_alloc(5, 0);
    Store_field(vInner, 0, Val_int(event->key.windowID));
    Store_field(vInner, 1, Val_bool(event->key.repeat));
    Store_field(vInner, 2, Val_int(event->key.keysym.mod));
    Store_field(vInner, 3, Val_int(event->key.keysym.scancode));
    Store_field(vInner, 4, Val_int(event->key.keysym.sym));

    Store_field(v, 0, vInner);
    break;
  case SDL_TEXTINPUT:
    v = caml_alloc(1, 6);

    vInner = caml_alloc(2, 0);
    Store_field(vInner, 0, Val_int(event->text.windowID));
    Store_field(vInner, 1, caml_copy_string(event->text.text));

    Store_field(v, 0, vInner);
    break;
  case SDL_TEXTEDITING:
    v = caml_alloc(1, 7);

    vInner = caml_alloc(4, 0);
    Store_field(vInner, 0, Val_int(event->edit.windowID));
    Store_field(vInner, 1, caml_copy_string(event->edit.text));
    Store_field(vInner, 2, Val_int(event->edit.start));
    Store_field(vInner, 3, Val_int(event->edit.length));

    Store_field(v, 0, vInner);
    break;
  case SDL_PANEVENT:
    v = caml_alloc(1, 24);

    vInner = caml_alloc(8, 0);
    Store_field(vInner, 0, Val_int(event->window.windowID));
    Store_field(vInner, 1, Val_int(event->pan.x));
    Store_field(vInner, 2, Val_int(event->pan.y));
    Store_field(vInner, 3, Val_bool(event->pan.contains_x));
    Store_field(vInner, 4, Val_bool(event->pan.contains_y));
    Store_field(vInner, 5, Val_bool(event->pan.fling));
    Store_field(vInner, 6, Val_bool(event->pan.interrupt));
    // verify this is the correct way of representing a ref to some WheelType.t
    Store_field(vInner, 7, Val_int(event->pan.source_type));

    Store_field(v, 0, vInner);
    break;
  case SDL_WINDOWEVENT:
    switch (event->window.event) {
    case SDL_WINDOWEVENT_SHOWN:
      v = Val_SDL_WindowEvent(8, event->window.windowID);
      break;
    case SDL_WINDOWEVENT_HIDDEN:
      v = Val_SDL_WindowEvent(9, event->window.windowID);
      break;
    case SDL_WINDOWEVENT_EXPOSED:
      v = Val_SDL_WindowEvent(10, event->window.windowID);
      break;
    case SDL_WINDOWEVENT_MOVED:
      v = Val_SDL_WindowEventWithData(11, event->window.windowID,
                                      event->window.data1, event->window.data2);
      break;
    case SDL_WINDOWEVENT_RESIZED:
      v = Val_SDL_WindowEventWithData(12, event->window.windowID,
                                      event->window.data1, event->window.data2);
      break;
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      v = Val_SDL_WindowEventWithData(13, event->window.windowID,
                                      event->window.data1, event->window.data2);
      break;
    case SDL_WINDOWEVENT_MINIMIZED:
      v = Val_SDL_WindowEvent(14, event->window.windowID);
      break;
    case SDL_WINDOWEVENT_MAXIMIZED:
      v = Val_SDL_WindowEvent(15, event->window.windowID);
      break;
    case SDL_WINDOWEVENT_RESTORED:
      v = Val_SDL_WindowEvent(16, event->window.windowID);
      break;
    case SDL_WINDOWEVENT_ENTER:
      v = Val_SDL_WindowEvent(17, event->window.windowID);
      break;
    case SDL_WINDOWEVENT_LEAVE:
      v = Val_SDL_WindowEvent(18, event->window.windowID);
      break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      v = Val_SDL_WindowEvent(19, event->window.windowID);
      break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
      v = Val_SDL_WindowEvent(20, event->window.windowID);
      break;
    case SDL_WINDOWEVENT_CLOSE:
      v = Val_SDL_WindowEvent(21, event->window.windowID);
      break;
    case SDL_WINDOWEVENT_TAKE_FOCUS:
      v = Val_SDL_WindowEvent(22, event->window.windowID);
      break;
    case SDL_WINDOWEVENT_HIT_TEST:
      v = Val_SDL_WindowEvent(23, event->window.windowID);
      break;
    default:
      v = Val_int(1);
    };

    break;
  default:
    v = Val_int(1);
  }

  CAMLreturn(v);
};

CAMLprim value resdl_SDL_PollEvent() {
  CAMLparam0();
  CAMLlocal2(ret, evt);
  SDL_Event e;

  caml_release_runtime_system();
  int result = SDL_PollEvent(&e);
  caml_acquire_runtime_system();

  if (result == 0) {
    ret = Val_none;
  } else {
    evt = Val_SDL_Event(&e);
    ret = Val_some(evt);
  }

  CAMLreturn(ret);
}

CAMLprim value resdl_SDL_WaitEvent() {
  CAMLparam0();
  CAMLlocal2(ret, evt);
  SDL_Event e;

  caml_release_runtime_system();
  int result = SDL_WaitEvent(&e);
  caml_acquire_runtime_system();

  if (result == 1) {
    evt = Val_SDL_Event(&e);
    ret = Val_ok(evt);
  } else {
    ret = Val_error(caml_copy_string(SDL_GetError()));
  }

  CAMLreturn(ret);
}

CAMLprim value resdl_SDL_PushEvent() {
  CAMLparam0();

  SDL_Event user_event;
  user_event.type = SDL_USEREVENT;
  SDL_PushEvent(&user_event);

  CAMLreturn(Val_unit);
};

CAMLprim value resdl_SDL_WaitTimeoutEvent(value vTimeout) {
  CAMLparam1(vTimeout);
  CAMLlocal2(ret, evt);
  int timeout = Int_val(vTimeout);
  SDL_Event e;

  caml_release_runtime_system();
  int result = SDL_WaitEventTimeout(&e, timeout);
  caml_acquire_runtime_system();

  if (result == 1) {
    evt = Val_SDL_Event(&e);
    ret = Val_some(evt);
  } else {
    ret = Val_none;
  }

  CAMLreturn(ret);
}

CAMLprim value resdl_SDL_GetWindowSize(value vWindow) {
  CAMLparam1(vWindow);
  CAMLlocal1(ret);
  SDL_Window *win = (SDL_Window *)vWindow;
  int width, height = 0;
  SDL_GetWindowSize(win, &width, &height);
  ret = caml_alloc(2, 0);
  Store_field(ret, 0, Val_int(width));
  Store_field(ret, 1, Val_int(height));
  CAMLreturn(ret);
}

CAMLprim value resdl_SDL_GL_GetDrawableSize(value vWindow) {
  CAMLparam1(vWindow);
  CAMLlocal1(ret);
  SDL_Window *win = (SDL_Window *)vWindow;
  int width, height = 0;
  SDL_GL_GetDrawableSize(win, &width, &height);
  ret = caml_alloc(2, 0);
  Store_field(ret, 0, Val_int(width));
  Store_field(ret, 1, Val_int(height));
  CAMLreturn(ret);
}

CAMLprim value resdl_SDL_SetWindowIcon(value vWindow, value vIcon) {
  CAMLparam2(vWindow, vIcon);

  SDL_Window *win = (SDL_Window *)vWindow;
  SDL_Surface *surface = (SDL_Surface *)vIcon;
  SDL_SetWindowIcon(win, surface);

  CAMLreturn(Val_unit);
};

CAMLprim value resdl_SDL_SetWindowTransparency(value vWindow,
                                               value vTransparency) {
  CAMLparam2(vWindow, vTransparency);

  SDL_Window *win = (SDL_Window *)vWindow;
  double transparency = Double_val(vTransparency);

  int result;
  result = SDL_SetWindowOpacity(win, transparency);

  if (result == -1) {
    printf("WARNING: Setting transparency not supported!");
  }

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_CreateSystemCursor(value vCursor) {
  CAMLparam1(vCursor);

  SDL_Cursor *cursor = NULL;

  SDL_SystemCursor id = SDL_SYSTEM_CURSOR_ARROW;
  switch (Int_val(vCursor)) {
  case 1:
    id = SDL_SYSTEM_CURSOR_IBEAM;
    break;
  case 2:
    id = SDL_SYSTEM_CURSOR_WAIT;
    break;
  case 3:
    id = SDL_SYSTEM_CURSOR_CROSSHAIR;
    break;
  case 4:
    id = SDL_SYSTEM_CURSOR_WAITARROW;
    break;
  case 5:
    id = SDL_SYSTEM_CURSOR_SIZENWSE;
    break;
  case 6:
    id = SDL_SYSTEM_CURSOR_SIZENESW;
    break;
  case 7:
    id = SDL_SYSTEM_CURSOR_SIZEWE;
    break;
  case 8:
    id = SDL_SYSTEM_CURSOR_SIZENS;
    break;
  case 9:
    id = SDL_SYSTEM_CURSOR_SIZEALL;
    break;
  case 10:
    id = SDL_SYSTEM_CURSOR_NO;
    break;
  case 11:
    id = SDL_SYSTEM_CURSOR_HAND;
    break;
  default:
    id = SDL_SYSTEM_CURSOR_ARROW;
    break;
  }

  cursor = SDL_CreateSystemCursor(id);
  CAMLreturn((value)cursor);
}

CAMLprim value resdl_SDL_StartTextInput() {
  SDL_StartTextInput();
  return Val_unit;
}

CAMLprim value resdl_SDL_StopTextInput() {
  SDL_StopTextInput();
  return Val_unit;
}

CAMLprim value resdl_SDL_SetTextInputRect(value vX, value vY, value vWidth,
                                          value vHeight) {
  int x = Int_val(vX);
  int y = Int_val(vY);
  int width = Int_val(vWidth);
  int height = Int_val(vHeight);

  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = width;
  rect.h = height;

  SDL_SetTextInputRect(&rect);
  return Val_unit;
}

CAMLprim value resdl_SDL_IsTextInputActive() {
  return Val_bool(SDL_IsTextInputActive());
}

CAMLprim value resdl_SDL_SetCursor(value vCursor) {
  CAMLparam1(vCursor);
  SDL_Cursor *cursor = (SDL_Cursor *)vCursor;
  SDL_SetCursor(cursor);
  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_CreateRGBSurfaceFromImage(value vPath) {
  CAMLparam1(vPath);
  CAMLlocal1(ret);
  // FROM:
  // https://wiki.libsdl.org/SDL_CreateRGBSurfaceFrom

  int req_format = STBI_rgb_alpha;
  int width, height, orig_format;
  unsigned char *data =
      stbi_load(String_val(vPath), &width, &height, &orig_format, req_format);
  if (data == NULL) {

    ret = Val_error(caml_copy_string(stbi_failure_reason()));
  } else {

    // Set up the pixel format color masks for RGB(A) byte arrays.
    // Only STBI_rgb (3) and STBI_rgb_alpha (4) are supported here!
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int shift = (req_format == STBI_rgb) ? 8 : 0;
    rmask = 0xff000000 >> shift;
    gmask = 0x00ff0000 >> shift;
    bmask = 0x0000ff00 >> shift;
    amask = 0x000000ff >> shift;
#else // little endian, like x86
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = (req_format == STBI_rgb) ? 0 : 0xff000000;
#endif

    int depth, pitch;
    if (req_format == STBI_rgb) {
      depth = 24;
      pitch = 3 * width; // 3 bytes per pixel * pixels per row
    } else {             // STBI_rgb_alpha (RGBA)
      depth = 32;
      pitch = 4 * width;
    }

    SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(
        (void *)data, width, height, depth, pitch, rmask, gmask, bmask, amask);
    if (surf == NULL) {
      ret = Val_error(caml_copy_string(SDL_GetError()));
      stbi_image_free(data);
    } else {
      ret = Val_ok((value)surf);
    }
  }

  CAMLreturn(ret);
};

CAMLprim value resdl_SDL_GL_SwapWindow(value w) {
  SDL_Window *win = (SDL_Window *)w;
  caml_release_runtime_system();
  SDL_GL_SwapWindow(win);
  caml_acquire_runtime_system();
  return Val_unit;
}

SDL_HitTestResult hittest(SDL_Window *win, const SDL_Point *area, void *data) {
  return SDL_HITTEST_DRAGGABLE;
}

CAMLprim value resdl_SDL_SetWindowSize(value vWin, value vW, value vH) {
  CAMLparam3(vWin, vW, vH);

  SDL_Window *win = (SDL_Window *)vWin;
  int w = Int_val(vW);
  int h = Int_val(vH);
  SDL_SetWindowSize(win, w, h);

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_SetWindowMinimumSize(value vWin, value vW, value vH) {
  CAMLparam3(vWin, vW, vH);

  SDL_Window *win = (SDL_Window *)vWin;
  int w = Int_val(vW);
  int h = Int_val(vH);
  SDL_SetWindowMinimumSize(win, w, h);

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_SetWindowTitle(value vWin, value vTitle) {
  CAMLparam2(vWin, vTitle);

  SDL_Window *win = (SDL_Window *)vWin;
  const char *title = (const char *)String_val(vTitle);
  SDL_SetWindowTitle(win, title);

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_SetWindowPosition(value vWin, value vX, value vY) {
  CAMLparam3(vWin, vX, vY);

  SDL_Window *win = (SDL_Window *)vWin;
  int x = Int_val(vX);
  int y = Int_val(vY);
  SDL_SetWindowPosition(win, x, y);

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_WindowCenter(value vWin) {
  CAMLparam1(vWin);

  SDL_Window *win = (SDL_Window *)vWin;
  SDL_SetWindowPosition(win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_CreateWindow(value vWidth, value vHeight,
                                      value vName) {
  CAMLparam3(vWidth, vHeight, vName);

  int width = Int_val(vWidth);
  int height = Int_val(vHeight);

  // According to the docs - `SDL_GL_SetAttribute` needs
  // to be called prior to creating the window.

  /* Turn on double buffering with a 24bit Z buffer.
   * You may need to change this to 16 or 32 for your system */
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL_Window *win = (SDL_CreateWindow(
      String_val(vName), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width,
      height,
      SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE));

  value vWindow = (value)win;
  CAMLreturn(vWindow);
}

CAMLprim value resdl_SDL_SetWindowBordered(value vWin, value vBordered) {
  CAMLparam2(vWin, vBordered);

  SDL_Window *win = (SDL_Window *)vWin;
  SDL_bool bordered = Int_val(vBordered) == 1 ? SDL_TRUE : SDL_FALSE;

  SDL_SetWindowBordered(win, bordered);
  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_SetWindowResizable(value vWin, value vResizable) {
  CAMLparam2(vWin, vResizable);

  SDL_Window *win = (SDL_Window *)vWin;
  SDL_bool resize = Int_val(vResizable) == 1 ? SDL_TRUE : SDL_FALSE;

  SDL_SetWindowResizable(win, resize);
  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_HideWindow(value vWin) {
  CAMLparam1(vWin);

  SDL_Window *win = (SDL_Window *)vWin;
  SDL_HideWindow(win);

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_MaximizeWindow(value vWin) {
  CAMLparam1(vWin);

  SDL_Window *win = (SDL_Window *)vWin;
  SDL_MaximizeWindow(win);

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_MinimizeWindow(value vWin) {
  CAMLparam1(vWin);

  SDL_Window *win = (SDL_Window *)vWin;
  SDL_MinimizeWindow(win);

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_RaiseWindow(value vWin) {
  CAMLparam1(vWin);

  SDL_Window *win = (SDL_Window *)vWin;
  SDL_RaiseWindow(win);

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_RestoreWindow(value vWin) {
  CAMLparam1(vWin);

  SDL_Window *win = (SDL_Window *)vWin;
  SDL_RestoreWindow(win);

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_ShowWindow(value vWin) {
  CAMLparam1(vWin);

  SDL_Window *win = (SDL_Window *)vWin;
  SDL_ShowWindow(win);

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_SDL_GetWindowId(value vWindow) {
  CAMLparam1(vWindow);
  SDL_Window *win = (SDL_Window *)vWindow;
  int id = SDL_GetWindowID(win);
  CAMLreturn(Val_int(id));
}

CAMLprim value resdl_SDL_Init() {
  CAMLparam0();
  int ret = SDL_Init(SDL_INIT_VIDEO);

  CAMLreturn(Val_int(ret));
}

CAMLprim value resdl_SDL_GetScancodeName(value vScancode) {
  CAMLparam1(vScancode);
  CAMLlocal1(ret);

  SDL_Scancode scancode = (SDL_Scancode)Int_val(vScancode);
  ret = caml_copy_string(SDL_GetScancodeName(scancode));
  CAMLreturn(ret);
}

CAMLprim value resdl_SDL_GetKeyName(value vKey) {
  CAMLparam1(vKey);
  CAMLlocal1(ret);

  SDL_Keycode key = (SDL_Keycode)Int_val(vKey);
  ret = caml_copy_string(SDL_GetKeyName(key));
  CAMLreturn(ret);
}

CAMLprim value resdl_SDL_GetKeyFromScancode(value vScancode) {
  SDL_Scancode scancode = (SDL_Scancode)Int_val(vScancode);
  return Val_int(SDL_GetKeyFromScancode(scancode));
}

CAMLprim value resdl_SDL_GetScancodeFromKey(value vKey) {
  SDL_Keycode key = (SDL_Keycode)Int_val(vKey);
  return Val_int(SDL_GetScancodeFromKey(key));
}

CAMLprim value resdl_SDL_ModLeftShift(value vMod) {
  return Val_bool((Int_val(vMod) & KMOD_LSHIFT) == KMOD_LSHIFT);
};

CAMLprim value resdl_SDL_ModRightShift(value vMod) {
  return Val_bool((Int_val(vMod) & KMOD_RSHIFT) == KMOD_RSHIFT);
};

CAMLprim value resdl_SDL_ModLeftControl(value vMod) {
  return Val_bool((Int_val(vMod) & KMOD_LCTRL) == KMOD_LCTRL);
};

CAMLprim value resdl_SDL_ModRightControl(value vMod) {
  return Val_bool((Int_val(vMod) & KMOD_RCTRL) == KMOD_RCTRL);
};

CAMLprim value resdl_SDL_ModLeftAlt(value vMod) {
  return Val_bool((Int_val(vMod) & KMOD_LALT) == KMOD_LALT);
};

CAMLprim value resdl_SDL_ModRightAlt(value vMod) {
  return Val_bool((Int_val(vMod) & KMOD_RALT) == KMOD_RALT);
};

CAMLprim value resdl_SDL_ModLeftGui(value vMod) {
  return Val_bool((Int_val(vMod) & KMOD_LGUI) == KMOD_LGUI);
};

CAMLprim value resdl_SDL_ModRightGui(value vMod) {
  return Val_bool((Int_val(vMod) & KMOD_RGUI) == KMOD_RGUI);
};

CAMLprim value resdl_SDL_ModNumLockDown(value vMod) {
  return Val_bool((Int_val(vMod) & KMOD_NUM) == KMOD_NUM);
};

CAMLprim value resdl_SDL_ModCapsLockDown(value vMod) {
  return Val_bool((Int_val(vMod) & KMOD_CAPS) == KMOD_CAPS);
};

CAMLprim value resdl_SDL_ModAltGrDown(value vMod) {
  return Val_bool((Int_val(vMod) & KMOD_MODE) == KMOD_MODE);
};

CAMLprim value resdl_SDL_GetModState(value vUnit) {
  return Val_int(SDL_GetModState());
};

CAMLprim value resdl_SDL_ShowSimpleMessageBox(value vFlags, value vTitle,
                                              value vMessage, value vWindow) {
  CAMLparam4(vFlags, vTitle, vMessage, vWindow);
  int flags = SDL_MESSAGEBOX_INFORMATION;

  switch (Int_val(vFlags)) {
  case 0:
    flags = SDL_MESSAGEBOX_ERROR;
    break;
  case 1:
    flags = SDL_MESSAGEBOX_WARNING;
    break;
  default:
    flags = SDL_MESSAGEBOX_INFORMATION;
    break;
  }

  const char *title = String_val(vTitle);
  const char *msg = String_val(vMessage);

  SDL_Window *win = NULL;

  if (Is_block(vWindow)) {
    win = (SDL_Window *)Field(vWindow, 0);
  }

  SDL_ShowSimpleMessageBox(flags, title, msg, win);

  CAMLreturn(Val_unit);
}

CAMLprim value resdl_PassThrough(value v) { return v; };

CAMLprim value resdl__javascript__renderloop() { return Val_unit; }
};
