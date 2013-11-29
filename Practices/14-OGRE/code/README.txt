This project uses the Ogre library (http://www.ogre3d.org/)

For Linux this means you need to install the library files and headers for OGRE
by doing something like 

  $ apt-get install libogre-dev

For Windows+MinGW, the precompiled library binaries and the necessary includes are provided. Note, however
that *two* different sets of libraries are included. The choice of which set to use depends on the
MinGW compiler you are using to build your project.

Namely, if you use CodeBlocks with the bundled MinGW compiler (i.e. you downloaded CodeBlocks together
with MinGW as a single installer, which most participants of the course did), then you should be using
the libraries with "Short-Jump-Long-Jump exception handling". Those are in lib/mingw-sjlj and the corresponding
DLL's are in bin/. You will be automatically using those if you open the project 
<xxx>.mingw-sjlj.cbp in CodeBlocks.

If, however, you installed MinGW separately, from the MinGW website (and then configured CodeBlocks 
to use it), you should most probably be using libraries with the "DWARF2 exception handling".
Those are in lib/mingw-dw2. The DLL's are in bin-dw2/. The CodeBlocks project file
<xxx>.mingw-dw2.cbp is configured to link with those libraries.

Finally, note that the bundled OGRE binaries are a stripped version of the OGRE library (this is the bare minimum
needed to run the exercise session examples, the full SDK has more plugins, examples, documentation, etc. In addition,
the SJLJ version here was compiled without support for threading).
If at any point you will be planning to work with OGRE seriously, you should download the full Ogre SDK from the
OGRE website. Note that the official binary version of the OGRE SDK compiled for MinGW is using the "Dwarf2-convention",
so if you want to use that one, you should make sure that you are not using the MinGW bundled with CodeBlocks.
Instead, download and install MinGW from the mingw site, and configure CodeBlocks to use it in the Compiler preferences dialog.

No precompiled binaries are provided for Mac, you can install them from the OGRE website. Those are, however, compiled
for use with XCode, so you wouldn't be able to use them with CodeBlocks + MinGW. Use XCode then (or compile the Ogre SDK 
manually using MinGW).