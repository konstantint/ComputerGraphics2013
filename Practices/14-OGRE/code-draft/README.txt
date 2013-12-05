This project uses the Ogre library (http://www.ogre3d.org/)
Please read below the notes relevant for your operating system.


===== Linux =====
For Linux you need to install the library files and headers for OGRE. In Debian-based
systems like Ubuntu, this can be done as follows:

  $ sudo apt-get install libogre-1.8-dev libois-dev

In addition, you might need to edit the file bin-linux/plugins.cfg.
The PluginFolder line there must point to a directory containing the RenderSystem_GL.so and Plugin_ParticleFX.so
libraries. The original configuration assumes a 64-bit Ubuntu with OGRE installed as shown above. If you use
a different OS (or compiled the OGRE SDK manually), the plugin files might be located in a different place.


===== Windows =====
For Windows+MinGW, the precompiled OGRE library binaries and the necessary includes are provided with the
project files for your convenience. Note, however that *two* different sets of libraries are included. 
The choice of which set to use depends on the MinGW compiler you are using to build your project.

Namely, if you use CodeBlocks with the bundled MinGW compiler (i.e. you downloaded CodeBlocks together
with MinGW as a single installer, which most participants of the course did), then you should be using
the libraries with "SetJmp/LongJmp (SJLJ) exception handling". Those are in lib/mingw-sjlj and the 
corresponding DLL's are in bin/. You will be automatically using those if you open the project 
<xxx>.mingw-sjlj.cbp in CodeBlocks.

If, however, you installed MinGW separately, from the MinGW website (and then configured CodeBlocks 
to use it), you should most probably be using libraries with the "DWARF2 exception handling".
Those are in lib/mingw-dw2. The DLL's are in bin-dw2/. The CodeBlocks project file
<xxx>.mingw-dw2.cbp is configured to link with those libraries.

Finally, note that the bundled OGRE binaries make up the bare minimum of the OGRE library, needed to 
run the exercise session example. The full SDK has more plugins, examples, documentation, etc. In addition,
the SJLJ version here was compiled without support for threading. 
If at any point you will be planning to work with OGRE seriously, you should download the full Ogre SDK from the
OGRE website. In this case you should know that the official binaries of the OGRE SDK for MinGW are prepared 
using the "Dwarf2-convention" (at the moment, i.e. 2013, at least).
Thus, if you want to use the precomiled SDK, you should make sure that you won't be using the MinGW compiler 
bundled with CodeBlocks (if you do it, you'll get strange linking errors saying something about "__Unwind not
found" and the like). Instead, you will need to install MinGW from the official MinGW website, and configure 
CodeBlocks to use it in the Settings->Compiler->Toolchain executables dialog.


===== Mac =====
No precompiled binaries are provided for Mac, you can install them from the OGRE website if you need. 
Note that those are, however, compiled for use with XCode, so you wouldn't be able to use them 
with the CodeBlocks + MinGW set up assumed in this project. Just make a new XCode project, set it to use
the OGRE libraries, and add the two project files (main.cpp and input_util.h).


==== General ====
In general, if you plan to develop OGRE applications, you should use CMake as the base build system,
as this will ensure better portability between platforms and development environments. 
Check out the OGRE website for more details.
