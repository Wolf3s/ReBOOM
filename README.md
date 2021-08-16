# ReBOOM!
ReBOOM is an historically accurate modern day port of BOOM (the Doom source port)

# what is it
ReBOOM is a doom source port taken from the original BOOM source port with an initial base of Pooch (for SDL2, 64bit, portability etc..).  Its goal is to be historically accurate to BOOM while running on modern systems.

The tactic I am using is:
1. Port the original code to modern compilers (remove the 32bit DOS assembler too)
2. Remove and Regress any and all additions that are not in BOOM, add back all the bugs from BOOM
3. Plug in the ported code, build, test, repeat.
4. Compare each and every original BOOM file(s) to the ported file(s) and ensure everything is as authentic as possible, barring obvious things that need to be fixed and changed (memory, wad loading, deh/bex loading etc..)

# what works
It is currently a WIP - I am still porting functions over from the original code to the new base while still removing parts that are not from BOOM.  The first release that I do, will be the one that is essentially 'BOOM'.

# building
Use the appropriate makefile or project file(s) for your system (only 64bit is supported):

Makefile.macos (MacOS)

Makefile.unix (GNU/Linux) -> Other Unixes may work but are not tested

Windows Visual Studio project files for 2019 Community

# where is the data?

1. Transmaps are saved (for UNIX) in HOME as a hidden file.  On Windows, they are saved in the location of the binary as a non-hidden file.
2. Config files (.cfg) are saved (for UNIX) in HOME (.config/reboom.cfg).  MacOS places them in ~/Library/Application Support/reboom.cfg and Windows has them in the same location as the binary.
3. WAD files are to always be placed in the same location as the binary, for all platforms.
4. Save games (.dsg) are saved in HOME for both UNIX and MacOS as a hidden file (.reboom_savX.dsg).  For Windows they are saved in the same location as the binary as a non-hidden file.

Thanks!

Enjoy it :)
