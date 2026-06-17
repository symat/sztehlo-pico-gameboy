# VSCode compatible C project for developing software on the gameboy

The goal is to provide an optimized, easy to use application development environment on the gameboy hardware
.

## Architecture / Main Features

* There are three main components in the source-code:
    * lib: various libraries controlling different gameboy functions (display, sound, sd card, etc.)
    * os: the operating system is always running in the background, using one of the CPU cores. It controls the peripherials (refresh the screen, reads buttons, monitors the battery, etc) and executes the applications. All the compiled binary files contain the whole OS.
    * application: beside the OS, a single application can be executed on the second core. 
* Boot sequence:
    * TODO
* Debugging:
    * TODO
* Executing applications:
    * The GB can be flashed with new RP2040 UF2 files, using either the SWD interface or the BOOTSEL button
    * The OS can load new programs from the SD-card. You can also associate programs with extensions to start e.g. GBC emulator or MP3 player with a specific file.
    * The OS can support program load e.g. over USB
