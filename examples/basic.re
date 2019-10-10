open Sdl2;
open Sdl2.Gl;
open Reglm;

let isNative =
  switch (Sys.backend_type) {
  | Native => true
  | Bytecode => true
  | _ => false
  };

let getExecutingDirectory = () =>
  isNative ? Filename.dirname(Sys.argv[0]) ++ Filename.dir_sep : "";

let loadShader = (shaderType, source) => {
  let shader = glCreateShader(shaderType);
  let () = glShaderSource(shader, source);
  let result = glCompileShader(shader);
  switch (result) {
  | CompilationSuccess => print_endline("Shader compiled successfully.")
  | CompilationFailure(v) => print_endline("Failed to compile shader: " ++ v)
  };
  shader;
};

let initShaderProgram = (vsSource, fsSource) => {
  let vsShader = loadShader(GL_VERTEX_SHADER, vsSource);
  let fsShader = loadShader(GL_FRAGMENT_SHADER, fsSource);
  let shaderProgram = glCreateProgram();
  let () = glAttachShader(shaderProgram, vsShader);
  let _ = glAttachShader(shaderProgram, fsShader);
  let result = glLinkProgram(shaderProgram);
  switch (result) {
  | LinkSuccess => print_endline("Shader linked successfully.")
  | LinkFailure(v) => print_endline("Failed to link shader: " ++ v)
  };
  shaderProgram;
};

let run = () => {
  let _ = Sdl2.init();
  let primaryWindow = Sdl2.Window.create(100, 100, "test");
  let context = Sdl2.Gl.setup(primaryWindow);
  let version = Sdl2.Gl.glGetString(Sdl2.Gl.Version);
  let vendor = Sdl2.Gl.glGetString(Sdl2.Gl.Vendor);
  let shadingLanguageVersion =
    Sdl2.Gl.glGetString(Sdl2.Gl.ShadingLanguageVersion);

  Printf.printf(
    "OpenGL Info - version: %s vendor: %s shading language version: %s\n",
    version,
    vendor,
    shadingLanguageVersion,
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
  print_endline("Win32 scale factor: " ++ string_of_float(scale));

  let display = Sdl2.Window.getDisplay(primaryWindow);
  let dpi = Sdl2.Display.getDPI(display);
  print_endline("Display DPI: " ++ Sdl2.Display.Dpi.show(dpi));

  let mode = Sdl2.Display.getDesktopMode(display);
  print_endline("Display mode: " ++ Sdl2.Display.Mode.show(mode));

  Sdl2.Window.setSize(primaryWindow, 800, 600);
  Sdl2.Window.center(primaryWindow);

  Sdl2.Window.show(primaryWindow);

  Sdl2.Window.setHitTest(primaryWindow, Some((w, x, y) => {
    let size = Sld2.Window.getSize(w);
    let id = Sdl2.Window.getId(w);
    Printf.printf("hit test - window id: %d width: %d height: %d areaX: %d areaY: %d\n", id, size.width, size.height, x, y);
    Draggable;
  });
  Sdl2.Window.setBordered(primaryWindow, false);
 // Sdl2.Window.setSize(primaryWindow, 800, 600);
  Sdl2.Window.setResizable(primaryWindow, true);

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

  glViewport(0, 0, 800, 600);

  /* glfwSwapInterval sets the 'swap interval' - also known as vsync / vertical synchronization */
  /* A setting of 0 disables vsync, unless it is overridden by a driver or a user setting */
  // TODO:
  //glfwSwapInterval(0);

  let width = ref(800);
  let height = ref(600);

  /* Update pack alignment to allow single-channel images */
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  let%lwt img =
    Image.load(getExecutingDirectory() ++ "UVCheckerMap02-512.png");

  /* let img = Image.fromColor(255, 0, 0, 255); */
  let dimensions = Image.getDimensions(img);
  let pixels = Image.getPixels(img);
  print_endline(
    "- width: "
    ++ string_of_int(dimensions.width)
    ++ " - height: "
    ++ string_of_int(dimensions.height),
  );

  let texture = glCreateTexture();
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  print_endline("Getting framebuffer size...");

  let frameBufferSize = Sdl2.Gl.getDrawableSize(primaryWindow);
  print_endline(
    "framebuffersize: "
    ++ string_of_int(frameBufferSize.width)
    ++ "x"
    ++ string_of_int(frameBufferSize.height),
  );

  let vsSource = {|
        #ifndef GL_ES
        #define lowp
        #else
        precision mediump float;
        #endif

        attribute vec3 aVertexPosition;
        attribute vec4 aVertexColor;

        uniform mat4 uProjectionMatrix;
        uniform mat4 uViewMatrix;
        uniform mat4 uWorldMatrix;

        varying lowp vec4 vColor;
        varying lowp vec3 vPos;

        void main() {
            gl_Position = uProjectionMatrix * uViewMatrix * uWorldMatrix * vec4(aVertexPosition, 1.0);
            vColor = aVertexColor;
            vPos = aVertexPosition;
        }
    |};
  print_endline(vsSource);
  let fsSource = {|
        #ifndef GL_ES
        #define lowp
        #else
        precision mediump float;
        #endif

        varying lowp vec4 vColor;
        varying lowp vec3 vPos;

        uniform sampler2D uSampler;

        void main() {
            vec4 t1 = texture2D(uSampler, vPos.yz);
            vec4 t2 = texture2D(uSampler, vPos.xz);
            vec4 t3 = texture2D(uSampler, vPos.xy);
            vec4 t = t1 + t2 + t3;
            gl_FragColor = vec4(vColor.r * t.r, vColor.g * t.g, vColor.b * t.b, 0.5);
            // gl_FragColor = vec4(vTexCoord, 0.0, 1.0);
            //gl_FragColor = texture2D(texture, vTexCoord);
        }
    |};
  print_endline(fsSource);

  /* Populate buffers for the cube geometry */
  let vArray = Float32Array.of_array(Cube.positions);
  let cArray = Float32Array.of_array(Cube.colors);
  let iArray = Uint16Array.of_array(Cube.indices);

  let shaderProgram = initShaderProgram(vsSource, fsSource);
  let vb = glCreateBuffer();
  let cb = glCreateBuffer();
  let ib = glCreateBuffer();
  glBindBuffer(GL_ARRAY_BUFFER, vb);
  glBufferData(GL_ARRAY_BUFFER, vArray, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, cb);
  glBufferData(GL_ARRAY_BUFFER, cArray, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, iArray, GL_STATIC_DRAW);

  let posAttribute = glGetAttribLocation(shaderProgram, "aVertexPosition");
  let colorAttribute = glGetAttribLocation(shaderProgram, "aVertexColor");
  let worldUniform = glGetUniformLocation(shaderProgram, "uWorldMatrix");
  let viewUniform = glGetUniformLocation(shaderProgram, "uViewMatrix");
  let projectionUniform =
    glGetUniformLocation(shaderProgram, "uProjectionMatrix");

  let prevTime = ref(Unix.gettimeofday());

  let delta = ref(0.);
  let render = window => {
    //  glfwMakeContextCurrent(window);
    Sdl2.Gl.makeCurrent(window, context);
    let time = Unix.gettimeofday();
    delta := delta^ +. time -. prevTime^;
    prevTime := time;
    /* type glfwMouseButtonCallback = (window, glfwMouseButton, glfwButtonState, array(glfwModifierKey)) => unit; */
    /* let glfwSetMouseButtonCallback = (window, glfwMouseButtonCallback) => unit; */

    /* let glfwGetMouseButton = (window, glfwMouseButton) => glfwButtonState; */

    /* let glfwScrollCallback = (window, float, float) => unit; */
    /* type glfwSetScrollCallback = (window, glfwScrollCallback) => unit; */

    glClearColor(0.0, 0., 0., 1.);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    /* Test scissor functionality */
    /* glEnable(GL_SCISSOR_TEST); */
    /* glScissor(200, 200, 200, 200); */

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    glBindTexture(GL_TEXTURE_2D, texture);

    glUseProgram(shaderProgram);
    let m = Mat4.create();
    let v = Vec3.create(0., 0., -10.);
    Mat4.fromTranslation(m, v);

    let rot = Mat4.create();
    Mat4.rotate(rot, Angle.from_radians(delta^), Vec3.create(0., 0., 1.));

    let yRot = Mat4.create();
    Mat4.rotate(
      rot,
      Angle.from_radians(delta^ *. 0.7),
      Vec3.create(0., 1., 0.),
    );

    Mat4.multiply(rot, m, rot);
    Mat4.multiply(rot, yRot, rot);

    glUniformMatrix4fv(worldUniform, rot);

    glUniformMatrix4fv(viewUniform, Mat4.create());

    let projectionMatrix = Mat4.create();
    Mat4.perspective(
      projectionMatrix,
      45. *. (4.0 *. atan(1.0)) /. 180.,
      float_of_int(width^) /. float_of_int(height^),
      0.1,
      100.,
    );

    glUniformMatrix4fv(projectionUniform, projectionMatrix);
    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glVertexAttribPointer(posAttribute, 3, GL_FLOAT, false);
    glEnableVertexAttribArray(posAttribute);

    glBindBuffer(GL_ARRAY_BUFFER, cb);
    glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, false);
    glEnableVertexAttribArray(colorAttribute);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

    Sdl2.Gl.swapWindow(window);
  };

  //let frame = ref(0);
  Sdl2.renderLoop(() => {
    switch (Sdl2.Event.poll()) {
    | None => ()
    | Some(evt) =>
      print_endline(Sdl2.Event.show(evt));
      switch (evt) {
      | Sdl2.Event.Quit => exit(0)
      | _ => ()
      };
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
    //print_endline ("Clipboard string before: " ++ v);

    //Glfw.glfwSetClipboardString(primaryWindow, "test clipboard: " ++ string_of_int(frame^));

    //let s = Glfw.glfwGetClipboardString(primaryWindow);
    //let v = switch(s) {
    //| Some(v) => "Some(" ++ v ++ ")"
    //| None => "None"
    //}
    //print_endline ("Clipboard string after: " ++ v);

    /* Run the GC so we can catch any GC-related crashes early! */
    // Gc.full_major();
    //glfwPollEvents();
    //glfwWindowShouldClose(primaryWindow);
    false;
  });

  print_endline("Done!");
  //glfwTerminate();
  Lwt.return();
};
