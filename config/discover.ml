
type os =
    | Windows
    | Mac
    | Linux
    | Unknown

let uname () =
    let ic = Unix.open_process_in "uname" in
    let uname = input_line ic in
    let () = close_in ic in
    uname;;

let get_os =
    match Sys.os_type with
    | "Win32" -> Windows
    | _ -> match uname () with
        | "Darwin" -> Mac
        | "Linux" -> Linux
        | _ -> Unknown

let root = Sys.getenv "cur__root"
let c_flags = ["-I"; (Sys.getenv "SDL2_INCLUDE_PATH"); "-I"; Filename.concat root "include"; "-I"; Filename.concat root "src"]

let c_flags = match get_os with
    | Windows -> c_flags @ ["-mwindows"]
    | Linux -> c_flags @ ["-fPIC"]
    | _ -> c_flags
;;

let libPath = "-L" ^ (Sys.getenv "SDL2_LIB_PATH")

let ccopt s = ["-ccopt"; s]
let cclib s = ["-cclib"; s]

let flags =
    match get_os with
    | Windows ->  []
        @ ccopt(libPath)
        @ cclib("-lSDL2")
        @ cclib("-lgdi32")
        @ cclib("-subsystem windows");
    | Linux -> []
        @ ccopt(libPath)
        @ cclib("-lGL")
        @ cclib("-lGLU")
        @ cclib("-lSDL2")
        @ cclib("-lX11")
        @ cclib("-lXxf86vm")
        @ cclib("-lXrandr")
        @ cclib("-lXinerama")
        @ cclib("-lXcursor")
        @ cclib("-lpthread")
        @ cclib("-lXi")
    | _ -> []
        @ ccopt(libPath)
        @ cclib("-lSDL2")
        @ ccopt("-framework AppKit")
        @ ccopt("-framework Foundation")
        @ ccopt("-framework OpenGL")
        @ ccopt("-framework Cocoa")
        @ ccopt("-framework IOKit")
        @ ccopt("-framework CoreVideo")
;;

let c_library_flags =
    match get_os with
    | Linux -> [
        libPath;
        "-lSDL2";
    ]
    | Mac -> [
        libPath;
        "-lSDL2";
    ]
    | _ -> []

let cxx_flags =
    match get_os with
    | Linux -> c_flags @ ["-std=c++11"]
    | Windows -> c_flags @ ["-fno-exceptions"; "-fno-rtti"; "-lstdc++"; "-mwindows"]
    | Mac -> c_flags @ ["-x"; "objective-c++"]
    | _ -> c_flags
;;

Configurator.V1.Flags.write_sexp "c_library_flags.sexp" c_library_flags;
Configurator.V1.Flags.write_sexp "c_flags.sexp" c_flags;
Configurator.V1.Flags.write_sexp "cxx_flags.sexp" cxx_flags;
Configurator.V1.Flags.write_sexp "flags.sexp" flags;
