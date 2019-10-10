open Reglm;

type enableOptions =
  | GL_DEPTH_TEST
  | GL_BLEND
  | GL_SCISSOR_TEST;

let glClearColor: (float, float, float, float) => unit;
let glClearDepth: float => unit;
let glViewport: (int, int, int, int) => unit;
let glScissor: (int, int, int, int) => unit;

type shader;
type shaderType =
  | GL_VERTEX_SHADER
  | GL_FRAGMENT_SHADER;

type shaderCompilationResult =
  | CompilationSuccess
  | CompilationFailure(string);

let glCreateShader: shaderType => shader;
let glShaderSource: (shader, string) => unit;
let glCompileShader: shader => shaderCompilationResult;
let glDeleteShader: shader => unit;

let glEnable: enableOptions => unit;
let glDisable: enableOptions => unit;

type depthFunctions =
  | GL_LEQUAL;

let glDepthFunc: depthFunctions => unit;

type blendFunc =
  | GL_ZERO
  | GL_ONE
  | GL_SRC_ALPHA
  | GL_ONE_MINUS_SRC_ALPHA;

let glBlendFunc: (blendFunc, blendFunc) => unit;

type program;

type glString =
| Extensions
| Vendor
| Renderer
| Version
| ShadingLanguageVersion;

let glGetString: (glString) => string;

type shaderLinkResult =
  | LinkSuccess
  | LinkFailure(string);

let glCreateProgram: unit => program;
let glAttachShader: (program, shader) => unit;
let glLinkProgram: program => shaderLinkResult;
let glUseProgram: program => unit;

type attribLocation;
let glGetAttribLocation: (program, string) => attribLocation;

type uniformLocation;
let glGetUniformLocation: (program, string) => uniformLocation;

let glUniform1f: (uniformLocation, float) => unit;
let glUniform2f: (uniformLocation, float, float) => unit;
let glUniform3f: (uniformLocation, float, float, float) => unit;
let glUniform4f: (uniformLocation, float, float, float, float) => unit;

let glUniform1i: (uniformLocation, int) => unit;
let glUniform2i: (uniformLocation, int, int) => unit;
let glUniform3i: (uniformLocation, int, int, int) => unit;
let glUniform4i: (uniformLocation, int, int, int, int) => unit;

let glUniform2fv: (uniformLocation, Vec2.t) => unit;
let glUniform3fv: (uniformLocation, Vec3.t) => unit;
let glUniform4fv: (uniformLocation, Vec4.t) => unit;

let glUniformMatrix4fv: (uniformLocation, Mat4.t) => unit;

type pixelAlignmentParameter =
  | GL_PACK_ALIGNMENT
  | GL_UNPACK_ALIGNMENT;

let glPixelStorei: (pixelAlignmentParameter, int) => unit;

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

type texture;
let glCreateTexture: unit => texture;
let glBindTexture: (textureType, texture) => unit;
let glTexParameteri:
  (textureType, textureParameter, textureParameterValue) => unit;
let glTexImage2D:
  (
    textureType,
    int,
    format,
    format,
    glType,
    Bigarray.Array2.t(int, Bigarray.int8_unsigned_elt, Bigarray.c_layout)
  ) =>
  unit;
let glGenerateMipmap: textureType => unit;

type bufferType =
  | GL_ARRAY_BUFFER
  | GL_ELEMENT_ARRAY_BUFFER;

let glVertexAttribPointer: (attribLocation, int, glType, bool) => unit;
let glEnableVertexAttribArray: attribLocation => unit;

type buffer;
let glCreateBuffer: unit => buffer;
let glBindBuffer: (bufferType, buffer) => unit;
let glUnbindBuffer: bufferType => unit;

type drawType =
  | GL_STATIC_DRAW;

let glBufferData:
  (bufferType, Bigarray.Array1.t('a, 'b, Bigarray.c_layout), drawType) => unit;

type drawMode =
  | GL_TRIANGLES
  | GL_TRIANGLE_STRIP;

let glDrawArrays: (drawMode, int, int) => unit;
let glDrawElements: (drawMode, int, glType, int) => unit;

let glReadPixels:
  (
    int,
    int,
    format,
    glType,
    Bigarray.Array2.t(int, Bigarray.int8_unsigned_elt, Bigarray.c_layout)
  ) =>
  unit;
