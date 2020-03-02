let isNative =
  switch (Sys.backend_type) {
  | Native => true
  | Bytecode => true
  | _ => false
  };

let getExecutingDirectory = () =>
  isNative ? Filename.dirname(Sys.argv[0]) ++ Filename.dir_sep : "";

let run = () => {
  Sdl2.Log.setOutputFunction((_category, _priority, message) => {
    Console.log("SDL2: " ++ message)
  });

  let _ = Sdl2.init();
  let attachResult = Sdl2.Platform.win32AttachConsole();

  // If we were unable to attach a console, try allocating a new one
  let _code =
    if (attachResult == 0) {
      Sdl2.Platform.win32AllocConsole();
    } else {
      attachResult;
    };

  Console.log("Operating system: " ++ Sdl2.Platform.getName());
  Console.log("Operating system version: " ++ Sdl2.Platform.getVersion());
  let primaryWindow = Sdl2.Window.create(100, 100, "test");
  let context = Sdl2.Gl.setup(primaryWindow);
  let version = Sdl2.Gl.getString(Sdl2.Gl.Version);
  let vendor = Sdl2.Gl.getString(Sdl2.Gl.Vendor);
  let shadingLanguageVersion =
    Sdl2.Gl.getString(Sdl2.Gl.ShadingLanguageVersion);

  Console.log(
    Printf.sprintf(
      "OpenGL Info - version: %s vendor: %s shading language version: %s\n",
      version,
      vendor,
      shadingLanguageVersion,
    ),
  );

  Sdl2.Gl.setSwapInterval(1);
  //glfwMakeContextCurrent(primaryWindow);

  /*let monitor = glfwGetPrimaryMonitor();
    let vidMode = glfwGetVideoMode(monitor);*/

  /*Sdl2.(
      primaryWindow,
      (vidMode.width - 800) / 2,
      (vidMode.height - 600) / 2,
    );*/
  Sdl2.Window.setTitle(primaryWindow, "reason-sdl2 example");
  Sdl2.Window.setWin32ProcessDPIAware(primaryWindow);

  let scale = Sdl2.Window.getWin32ScaleFactor(primaryWindow);
  Console.log("Win32 scale factor: " ++ string_of_float(scale));

  let display = Sdl2.Window.getDisplay(primaryWindow);
  let dpi = Sdl2.Display.getDPI(display);
  Console.log("Display DPI: " ++ Sdl2.Display.Dpi.show(dpi));

  let mode = Sdl2.Display.getDesktopMode(display);
  Console.log("Display mode: " ++ Sdl2.Display.Mode.show(mode));

  Sdl2.Window.setSize(primaryWindow, 800, 600);
  Sdl2.Window.center(primaryWindow);

  //Sdl2.Window.setTransparency(primaryWindow, 0.8);

  Sdl2.Window.setMacBackgroundColor(primaryWindow, 0.0, 0.0, 0.0, 1.);
  Sdl2.Window.setMacTitlebarTransparent(primaryWindow);

  Sdl2.Window.show(primaryWindow);

  /*Sdl2.Window.setHitTest(
      primaryWindow,
      Some(
        (w, x, y) => {
          let size = Sdl2.Window.getSize(w);
          let id = Sdl2.Window.getId(w);
          Printf.printf(
            "hit test - window id: %d width: %d height: %d areaX: %d areaY: %d\n",
            id,
            size.width,
            size.height,
            x,
            y,
          );
          if (x < 10) {
            ResizeLeft;
          } else if (y < 40) {
            Draggable;
          } else {
            Normal;
          };
        },
      ),
    );*/
  //Sdl2.Window.setBordered(primaryWindow, false);
  // Sdl2.Window.setSize(primaryWindow, 800, 600);
  Sdl2.Window.setResizable(primaryWindow, true);
  Sdl2.Window.setMinimumSize(primaryWindow, 200, 100);

  // Start text input, to experiment with IME + events
  Sdl2.TextInput.setInputRect(25, 50, 100, 25);
  Sdl2.TextInput.start();

  //Sdl2.Window.maximize(primaryWindow);

  /*   let secondaryWindow = */
  /*     glfwCreateWindow(100, 50, ~sharedContext=primaryWindow, "secondWindow"); */
  /*   glfwSetWindowPos(secondaryWindow, vidMode.width / 2, vidMode.height / 2); */
  /*   glfwSetWindowSize(secondaryWindow, 800, 600); */
  /*   glfwSetWindowTitle(secondaryWindow, "second window"); */

  let cursors = [|
    Sdl2.Cursor.createSystem(Arrow),
    Sdl2.Cursor.createSystem(IBeam),
    Sdl2.Cursor.createSystem(Wait),
    Sdl2.Cursor.createSystem(No),
    Sdl2.Cursor.createSystem(Hand),
  |];
  Random.self_init();
  let cursor = Random.int(Array.length(cursors));
  Sdl2.Cursor.set(cursors[cursor]);

  let iconSurface =
    Sdl2.Surface.createFromImagePath(
      getExecutingDirectory() ++ "favicon01.png",
    );
  switch (iconSurface) {
  | Ok(v) => Sdl2.Window.setIcon(primaryWindow, v)
  | Error(msg) => prerr_endline("Error: " ++ msg)
  };

  /* glfwSwapInterval sets the 'swap interval' - also known as vsync / vertical synchronization */
  /* A setting of 0 disables vsync, unless it is overridden by a driver or a user setting */
  // TODO:
  //glfwSwapInterval(0);

  //let width = ref(800);
  //let height = ref(600);

  let frameBufferSize = Sdl2.Gl.getDrawableSize(primaryWindow);
  Console.log(
    "framebuffersize: "
    ++ string_of_int(frameBufferSize.width)
    ++ "x"
    ++ string_of_int(frameBufferSize.height),
  );

  let render = window => {
    //  glfwMakeContextCurrent(window);
    Sdl2.Gl.makeCurrent(window, context);
    Sdl2.Gl.swapWindow(window);
  };

  //let frame = ref(0);
  Sdl2.renderLoop(() => {
    switch (Sdl2.Event.poll()) {
    | None => ()
    | Some(evt) =>
      //Console.log(Sdl2.Event.show(evt));
      switch (evt) {
      | Sdl2.Event.Quit => exit(0)
      | _ => ()
      }
    };

    render(primaryWindow);
    /* render(secondaryWindow); */
    /*  if (frame^ == 60) {
          captureWindow(primaryWindow, Printf.sprintf("scrot%d.tga", frame^));
        };
        frame := frame^ + 1;*/

    //let s = Glfw.glfwGetClipboardString(primaryWindow);

    //let v = switch(s) {
    //| Some(v) => "Some(" ++ v ++ ")"
    //| None => "None"
    //}
    //Console.log ("Clipboard string before: " ++ v);

    //Glfw.glfwSetClipboardString(primaryWindow, "test clipboard: " ++ string_of_int(frame^));

    //let s = Glfw.glfwGetClipboardString(primaryWindow);
    //let v = switch(s) {
    //| Some(v) => "Some(" ++ v ++ ")"
    //| None => "None"
    //}
    //Console.log ("Clipboard string after: " ++ v);

    /* Run the GC so we can catch any GC-related crashes early! */
    Gc.full_major();
    //glfwPollEvents();
    //glfwWindowShouldClose(primaryWindow);
    false;
  });

  Console.log("Done!");
  //glfwTerminate();
};
