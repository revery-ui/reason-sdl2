let _ret = Sdl2.Platform.win32AttachConsole();

Console.log("Hello from Console.log!");
prerr_endline("Hello from prerr_endline!");
print_endline("Hello from print_endline!");

Unix.sleepf(5.0);

exit(0);
