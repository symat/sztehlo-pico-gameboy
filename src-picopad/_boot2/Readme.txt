This folder contains boot2 loader (256 bytes long).

The boot2 loader is included at the beginning of every
UF2 program. In the PicoLibSDK library, the boot2 loader
is only included at the beginning of the boot3 loader.

You can recompile (d_if_you_must.bat + c_rp2040.bat), but 
we also store the compiled bootloader in git (it really shouldn't 
change frequently).
