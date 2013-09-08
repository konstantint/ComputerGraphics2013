The provided code can be used in Windows, Mac or Linux. 

Note the following points regarding the 1_HelloAllegro project, however:

* In Windows, the executable must be invoked from within the bin/ folder.
This happens automatically, when you launch it from CodeBlocks or double-click on bin/1_HelloAllegro.exe.
If you want to run it from another place, make sure the DLL files "allegro-5.0.10-monolith-md.dll"
and "libstdc++-6.dll" are available in the same folder.

* To compile the project in Mac, you will need to:
    - Install allegro5 from MacPorts.
    - Launch the compiled executable manually from the command line.
      (So far CodeBlocks for Mac seems to have problems launching files).
Alternatively, you may use XCode, but this will require some additional set up.
See http://stefanhendriks.wordpress.com/2013/01/11/getting-started-with-allegro-5-1-on-mac-os-x-10-8-xcode-4-5-and-homebrew/

* The Linux version should, hopefully, work on most modern Debian-based Linuxes.
If it does not, you might need to compile Allegro for your system manually.
See http://wiki.allegro.cc/index.php?title=Ubuntu_and_Allegro_5, for example.