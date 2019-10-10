open Reglm;

type enableOptions =
  | GL_DEPTH_TEST
  | GL_BLEND
  | GL_SCISSOR_TEST;

type shader;
type shaderType =
  | GL_VERTEX_SHADER
  | GL_FRAGMENT_SHADER;

type shaderCompilationResult =
  | CompilationSuccess
  | CompilationFailure(string);

external glClearColor: (float, float, float, float) => unit =
  "caml_glClearColor";
external glClearDepth: float => unit = "caml_glClearDepth";
external glCreateShader: shaderType => shader = "caml_glCreateShader";
external glShaderSource: (shader, string) => unit = "caml_glShaderSource";

[@noalloc]
external glViewport: (int, int, int, int) => unit = "caml_glViewport";
[@noalloc] external glScissor: (int, int, int, int) => unit = "caml_glScissor";

[@noalloc] external glEnable: enableOptions => unit = "caml_glEnable";
[@noalloc] external glDisable: enableOptions => unit = "caml_glDisable";

type depthFunctions =
  | GL_LEQUAL;

external glDepthFunc: depthFunctions => unit = "caml_glDepthFunc";

type blendFunc =
  | GL_ZERO
  | GL_ONE
  | GL_SRC_ALPHA
  | GL_ONE_MINUS_SRC_ALPHA;

[@noalloc]
external glBlendFunc: (blendFunc, blendFunc) => unit = "caml_glBlendFunc";

/* TODO: Add compile result return */
external glCompileShader: shader => shaderCompilationResult =
  "caml_glCompileShader";
external glDeleteShader: shader => unit = "caml_glDeleteShader";

type glString =
  | Extensions
  | Vendor
  | Renderer
  | Version
  | ShadingLanguageVersion;

external glGetString: glString => string = "caml_glGetString";

type program;

external glCreateProgram: unit => program = "caml_glCreateProgram";
external glAttachShader: (program, shader) => unit = "caml_glAttachShader";
external glUseProgram: program => unit = "caml_glUseProgram";

type shaderLinkResult =
  | LinkSuccess
  | LinkFailure(string);

external glLinkProgram: program => shaderLinkResult = "caml_glLinkProgram";

type attribLocation;
[@noalloc]
external glGetAttribLocation: (program, string) => attribLocation =
  "caml_glGetAttribLocation";

type uniformLocation;
[@noalloc]
external glGetUniformLocation: (program, string) => uniformLocation =
  "caml_glGetUniformLocation";

[@noalloc]
external glUniform1f: (uniformLocation, [@unboxed] float) => unit =
  "caml_glUniform1f_byte" "caml_glUniform1f";
[@noalloc]
external glUniform2f:
  (uniformLocation, [@unboxed] float, [@unboxed] float) => unit =
  "caml_glUniform2f_byte" "caml_glUniform2f";
[@noalloc]
external glUniform3f:
  (uniformLocation, [@unboxed] float, [@unboxed] float, [@unboxed] float) =>
  unit =
  "caml_glUniform3f_byte" "caml_glUniform3f";
[@noalloc]
external glUniform4f:
  (
    uniformLocation,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float
  ) =>
  unit =
  "caml_glUniform4f_byte" "caml_glUniform4f";

[@noalloc]
external glUniform1i: (uniformLocation, int) => unit = "caml_glUniform1i";
[@noalloc]
external glUniform2i: (uniformLocation, int, int) => unit = "caml_glUniform2i";
[@noalloc]
external glUniform3i: (uniformLocation, int, int, int) => unit =
  "caml_glUniform3i";
[@noalloc]
external glUniform4i: (uniformLocation, int, int, int, int) => unit =
  "caml_glUniform4i";

[@noalloc]
external glUniform2fv: (uniformLocation, Vec2.t) => unit = "caml_glUniform2fv";
[@noalloc]
external glUniform3fv: (uniformLocation, Vec3.t) => unit = "caml_glUniform3fv";
[@noalloc]
external glUniform4fv: (uniformLocation, Vec4.t) => unit = "caml_glUniform4fv";

[@noalloc]
external glUniformMatrix4fv: (uniformLocation, Mat4.t) => unit =
  "caml_glUniformMatrix4fv";

type pixelAlignmentParameter =
  | GL_PACK_ALIGNMENT
  | GL_UNPACK_ALIGNMENT;

[@noalloc]
external glPixelStorei: (pixelAlignmentParameter, int) => unit =
  "caml_glPixelStorei";

type texture;
type textureType =
  | GL_TEXTURE_2D;

type textureParameter =
  | GL_TEXTURE_WRAP_S
  | GL_TEXTURE_WRAP_T
  | GL_TEXTURE_MIN_FILTER
  | GL_TEXTURE_MAG_FILTER;

type textureParameterValue =
  | GL_REPEAT
  | GL_LINEAR
  | GL_CLAMP_TO_EDGE
  | GL_NEAREST
  | GL_NEAREST_MIPMAP_NEAREST
  | GL_LINEAR_MIPMAP_NEAREST
  | GL_NEAREST_MIPMAP_LINEAR
  | GL_LINEAR_MIPMAP_LINEAR;

type format =
  | GL_ALPHA
  | GL_RGB
  | GL_RGBA;

type glType =
  | GL_FLOAT
  | GL_UNSIGNED_BYTE
  | GL_UNSIGNED_SHORT
  | GL_UNSIGNED_SHORT_5_6_5
  | GL_UNSIGNED_SHORT_4_4_4_4
  | GL_UNSIGNED_SHORT_5_5_5_1;

[@noallooc] external glCreateTexture: unit => texture = "caml_glCreateTexture";
[@noalloc]
external glBindTexture: (textureType, texture) => unit = "caml_glBindTexture";
[@noalloc]
external glTexParameteri:
  (textureType, textureParameter, textureParameterValue) => unit =
  "caml_glTexParameteri";

external glTexImage2D:
  (
    textureType,
    int,
    format,
    format,
    glType,
    Bigarray.Array2.t(int, Bigarray.int8_unsigned_elt, Bigarray.c_layout)
  ) =>
  unit =
  "caml_glTexImage2D_bytecode" "caml_glTexImage2D_native";

[@noalloc]
external glGenerateMipmap: textureType => unit = "caml_glGenerateMipmap";

type bufferType =
  | GL_ARRAY_BUFFER
  | GL_ELEMENT_ARRAY_BUFFER;

type buffer;
[@noalloc] external glCreateBuffer: unit => buffer = "caml_glCreateBuffer";
[@noalloc]
external glBindBuffer: (bufferType, buffer) => unit = "caml_glBindBuffer";
[@noalloc] external glUnbindBuffer: bufferType => unit = "caml_glUnbindBuffer";

type drawType =
  | GL_STATIC_DRAW;

[@noalloc]
external glBufferData:
  (bufferType, Bigarray.Array1.t('a, 'b, Bigarray.c_layout), drawType) => unit =
  "caml_glBufferData";

[@noalloc]
external glVertexAttribPointer: (attribLocation, int, glType, bool) => unit =
  "caml_glVertexAttribPointer";
[@noalloc]
external glEnableVertexAttribArray: attribLocation => unit =
  "caml_glEnableVertexAttribArray";

type drawMode =
  | GL_TRIANGLES
  | GL_TRIANGLE_STRIP;

[@noalloc]
external glDrawArrays: (drawMode, int, int) => unit = "caml_glDrawArrays";
[@noalloc]
external glDrawElements: (drawMode, int, glType, int) => unit =
  "caml_glDrawElements";

external glReadPixels:
  (
    int,
    int,
    format,
    glType,
    Bigarray.Array2.t(int, Bigarray.int8_unsigned_elt, Bigarray.c_layout)
  ) =>
  unit =
  "caml_glReadPixels";
