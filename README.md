KMotionX
========

These notes pertain to the swig branch.  The master branch is basically the same as Par Hansson's repository.  The swig branch includes some bug fixes, plus a Python binding.

Linux/Unix (Mac OS X ) port of Dynomotions KMotion

KMotionX is more of a patch than a port of KMotion (Except for KMotionServer). The goal has been to make as small modifications as possible to the orginal source. This will make it easier to keep up with every new release.
    Builds with at least gcc-4.6 on linux and Visual Studio 2008 on windows.

The master branch requires gcc-4.7, but this requirement has been relaxed by removing some code constructs that necessitated 4.7.  gcc-4.6 is more typically installed on embedded systems like the Raspberry Pi, so it is nice not to have to upgrade the compiler.


See it in action on youtube
https://www.youtube.com/watch?v=oPTJwcre0hA


KMotionX also includes a JNI binding to use expose the GCodeInterpreter and KMotionDLL API to the Java world. See advanced.

The swig branch introduces a Python binding based on "swig".

####Who might be interested in this.
Anyone that would like to operate their KFlop controlled machine from their phone, tablets or anything with a web browser. I have ported the KMotionServer, KMotionDLL and GCodeInterpreter. I had plans to create a Java GUI but have now dropped that idea and created a HTML5 web application for that purpose. Drag an svg file into browser window and it will instantly be converted into executable GCode. Although I must admit it is still in alpha. There are lots of missing features to just use it out of the box. Some things might be hard wired for my current machine setup. Hence in this stage you might have to do some changes yourself. Or just wait until I have fixed it.


![Image of KMXWeb](KMotionX/doc/images/main.png)


#####Changes from original source
- Source code for KMotionDLL and GCodeInterpreter has been patched with "ifdef" directives.(I know it is a bit ugly)
- Changed include directives to build on case sensitive file systems #include "stdafx.h" to "StdAfx.h"
- Implemented thread handling to make use of Posix instead of Windows thread API
- Option to use libftdi instead of ftd2xx driver (libftdi works alot better on Linux)
- Added latest TCC compiler 0.9.26 patched with changes made in the original version TCC67 (0.9.16 or whatever version that was)

Note: the swig branch does not use TCC 0.9.26.  The TCC67 which is standard on the Windows versions of Dynomotion has been patched to work on 32-bit CPUs.  64-bit support is still broken.
TCC 0.9.26 does not yet completely work.  It is OK for simple code, but there are some known problems with more complex code.

#####Changes to be made
- (DONE) Remove dependency of CString (standard C functions)
- Implement callback pattern for MessageBox and AfxMessageBox


#####Features to be implemented
- (DONE) Add Compile and load coff program to init KFLOP
- (DONE) Execute on PC
- (DONE) Concatenation of error message in CheckSoftLimitsArc
- ListLocations KMotionDLL_Direct (should work but needs more testing)
- (DONE) LoadCoff


Dependencies
Linux at least GCC 4.6 toolchain
libudev-dev
cmake
boost
libusb
libftdi
udev rule


##Build and install
This guide contains four sections. How to install on Mac OS X, Ubuntu, Rasbian (Raspberry Pi) and a common section for KMotion libraries.

###Mac OS X
This is a really nice platform. Almost everything is installed already. Given that you are a developer and have the latest XCode installed.

Install macports from https://www.macports.org
Then install libftdi

Then install libfdti.
```
sudo port install libftdi1
```
Jump to section "Install KMotionX" or "Install Java" if you will use the Java binding.


###Ubuntu

This is only tested on 14.04.

######1. Install g++
Most platforms will have this already.  Try entering `gcc --version` which, if it exists, will show the version, which should be at least 4.6.  If it does not exist, or is back-level:
```
sudo apt-get install g++
```
######2. Install libftdi
Skip this step if you will be using ftd2xx driver. However libftdi works a lot better on linux libftdi is an open source ftdi driver that might be used as replacement when running on Linux or MacOSX
```
sudo apt-get install libftdi-dev
```

######3. Install python2.7-dev
Skip this step if you will not be using the Python binding
```
sudo apt-get install python2.7-dev
```

###Raspberry Pi

######1. Install Raspbian
Install latest Rasbian download zip image (current version 2014-01-07-wheezy-raspbian)
Follow installation guide README.md

Boot up your Pi.  A configuration wizard (raspi-config) will start automatically.  At least expand filesystem.

Update your installation executing following commands. Second and third command might take a while. This step is most probably not necessary, but it is how I did it.
```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get dist-upgrade
```


For a later model Raspberry Pi (R.Pi 2 model B etc.) only `sudo apt-get update` should be necessary.

######2. Install gcc
Skip this step if ”gcc --version” is 4.6 or later. The current latest version available is 4.8
Install gcc and g++ 4.8. This command installs gcc and g++ 4.8 but does not change to 4.8 as default. If you would like to do that here is a guide. (switching-gccg-versions.html) If you do you need to adjust make file to use default installed version.
```
sudo apt-get install gcc-4.8 g++-4.8
```
######3. Install libftdi
Skip this step if you will be using ftd2xx driver. However libftdi works a lot better on linux libftdi is an open source ftdi driver that might be used as replacement when running on Linux or MacOSX
```
sudo apt-get install libftdi-dev
```
Jump to section "Install KMotionX" or "Install Java" if you will use the Java binding.

######4. Install python2.7-dev
Skip this step if you will not be using the Python binding
```
sudo apt-get install python2.7-dev
```

##Install KMotionX

######1. Build KMotionX
Install git if not already installed
```
sudo apt-get install git
```

Clone repository
```
mkdir git
cd git
git clone https://github.com/meantaipan/KMotionX.git
```

```
cd KMotionX
```

Create a swig branch, check it out, and pull that branch
```
git branch swig
git checkout swig
git pull origin swig
```

Configure build for your platform and flavours
```
./configure
```
You can disable Java and/or Python using flags as follows:
```
./configure --disable-java --diable-python
```


Build project
```
make
```

Build Tiny C Compiler - but only if you want to do some development on it.  Otherwise, TCC67 will be built and used automatically.

(On Rasbian install texi2html and texinfo ```sudo apt-get install texi2html texinfo```)


```
cd KMotionX/tcc-0.9.26
./configure —enable-cross
make
cd ../..
```

######2. Install KFLOP device rules (Linux only)
This will install a rule that tell your system to grant read and write access to the kflop device for users in group "plugdev"
If your user is not in that group fix users groups or change the rule before pluging the device in.
```
sudo cp KMotionX/usb/etc/udev/rules.d/10.kflop.rules /etc/udev/rules.d/
```
If you already had the Kflop plugged in, unplug it and replug, otherwise the rule will not take effect.

######3. Execute examples
Plug in your KFlop to an available USB port.
KMotionServer will start automatically in background when needed.
When KMotionDLL is rebuilt make sure to kill running server 'killall KMotionServer'

First, try using the console:
```
cd bin
./kflopConsole
```
You should be able to enter commands like `version`.  If it fails to work, you can get some hints as to what is wrong by looking in syslog:
```
tail -40 /var/log/syslog
```

Start executeGCode example
```
./executeGCode
```
Or try the new web application. Start and surf to http://localhost:8080 in a modern web browser.
```
./kmxWeb -document_root ../KMotionX/KMotionXCNC/app
```

##SWIG branch
This branch provides several enhancements for the Linux port, plus bug fixes:

- fixes to the original TCC67 compiler from the official DynoMotion code base.

    This is not completely working on 64-bit systems because the code was not
    64-bit clean.
    
- changed client/server protocol so that it is possible to use TCP/IP as well
 as unix domain sockets.  This required each message to be prefixed with a
 2-byte length, so it is not compatible with the official Windows version.
 
 Basically, the KmotionServer program requires a direct USB connection to the
 Kflop, but clients can connect to the server using either TCP/IP or Unix
 (local pipe) sockets.

- added Python binding.  By default, the Python binding is enabled when running
 `configure`.  To disable, provide the --disable-python option.
 
 When enabled, `swig` will be used to generate the binding, so you will first
 need to install swig (Simple Wrapper Interface Generator).  Note that the
 git repository tracks the swig-generated file, so if you are just building
 straight from the repository, then the makefile will not trigger the swig
 step, so you can get away without installing swig.

##Python Binding
The `python` directory in the distribution contains some simple Python test code.
This is currently not documented much at all, so for details you will need to
read the source.  I hope to provide better documentation once everything is a
bit more stable.

To include the kmotion module, the first few lines of your Python script should
be something like:

```
import kmotion
from kmotion_extras import *
```

then you create an instance of class `KMotionX`.

For now, since there is no installer, you will need to issue the following
shell commands before you will be able to run any Python code:

```
export PYTHONPATH=path-to-KMmotionX-install/bin:path-to-KMmotionX-install/python
export KMOTION_BIN=path-to-KMmotionX-install/bin
```

Then, when the Kflop is connected to a USB port (and you have been able to 
run `bin/kflopConsole` successfully) you can try:

```
cd path-to-KMmotionX-install/python
python test_basic.py
```

although you will probably first need to edit those .py files and select the appropriate
Kflop location.  Find and edit the following lines:

```
# Select TCP or unix domain socket...
k = KMotionX(0, "192.168.7.2", with_console=True)
#k = KMotionX(0, with_console=True)

```

I'm assuming here that you know at least a modicum of Python.  One of these days I'll
define some command line parameters, but until then just edit the code.

####Python binding for KMotionDLL
This is the most basic level of control.  Documentation required.

####Python binding for GCodeInterpreter
The GCode interpreter class is also available, so that a complete CNC GUI may be constructed using Python and
a suitable GUI toolkit such as pyGTK.

In order to be as efficient as possible with (slow) interpreted languages like Python, there is some extra
functionality in swig_extras.cpp.  This module provides some derived classes for both the basic KMotionDLL and
the GCodeInterpreter.  In the latter case, there is an efficient polling method which automatically calls
back to Python code whenever any event of interest occurs.

All Python callbacks from the kmotion module are implemented via virtual methods in the relevant c++
class.  This is the most natural technique when using swig bindings.  It allows callbacks into the binding
language to be constructed by deriving a new subclass (in Python) of the c++ class.  Note that some of
the callbacks are not the same name as used in the original c++ class, since it was necessary to provide
some wrapper code to implement thread safety around the callbacks.  In this case, the Python GIL must
be acquired and released correctly.

Multi-thread handling for Python is supported, which is important since the kmotion module will often be
running in a separate thread, which will call back to Python code.  Although this works fine at the
Python level, you must be careful about calling non thread-aware code, such as some GUI toolkits.  For
example, if you are using GTK, then make sure that you properly use GTK thread-aware protocol if you ever
modify GTK objects from a kmotion module callback.

####kmotion_extras.py

This is a pure python module which provides some wrappers around the raw KMotion c++
classes.  Take a look at `kmotion_extras.py` for examples of how to define callbacks.

It is recommended to use the `KMotionX` and `Interpreter` classes in kmotion_extras rather than directly
using `kmotion.KMotion` or `kmotion.GCodeInterpreter`, which are the most low-level interfaces.

`KMotionX` makes a nicer interface to compiling and executing C programs on the Kflop, via the `ThreadManager`
class.  Use of this class improves efficiency, since it tracks C code file dependencies (like make)
to avoid unnecessary re-compilation (and even downloading).  It also supports alternative compilers
such as the Texas instruments compiler (cl6x), however that support is still experimental.

##Extra

[It still doesn't work. Troubleshoot](KMotionX/doc/Troubleshooting.md)


[Advanced stuff](KMotionX/doc/Advanced.md)

