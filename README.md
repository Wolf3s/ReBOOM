# ReBOOM
ReBOOM is a continuation of the BOOM source port, version 2.02.

# what is it
ReBOOM is a source port, directly ported from BOOM 2.02 with additions from the Pooch source port (64bit compatibility, SDL2).  ReBOOM aims to tastefully continue the development of BOOM in the
style of Team TNT and bring it into modern times.

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
