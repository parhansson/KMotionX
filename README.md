KMotionX
========

Linux/Unix (Mac OS X ) port of Dynomotions KMotion

KMotionX is more of a patch than a port of KMotion (Except for KMotionServer). The goal has been to make as small modifications as possible to the orginal source. This will make it easier to keep up with every new release.
    Builds with at least gcc-4.7 on linux and Visual Studio 2008 on windows.

See it in action
https://www.youtube.com/watch?v=oPTJwcre0hA

KMotionX also includes a JNI binding to use expose the GCodeInterpreter and KMotionDLL API to the Java world.

####Who might be interested in this.
Anyone that would like to operate their KFlop controlled machine from their phone, tablets or anything with a web browser. I have ported the KMotionServer, KMotionDLL and GCodeInterpreter. I had plans to create a Java GUI but have now dropped that idea and created a HTML5 web application for that purpose. Drag an svg file into browser window and it will instantly be converted into executable GCode. Although I must admit it is still in alpha. There are lots of missing features to just use it out of the box. Some things might be hard wired for my current machine setup. Hence in this stage you might have to do some changes yourself. Or just wait until I have fixed it.


#####Changes from original source
- Source code for KMotionDLL and GCodeInterpreter has been patched with "ifdef" directives.(I know it is a bit ugly)
- Changed include directives to build on case sensitive file systems #include "stdafx.h" to "StdAfx.h"
- Implemented thread handling to make use of Posix instead of Windows thread API
- Option to use libftdi instead of ftd2xx driver (libftdi works alot better on Linux)
- Added latest TCC compiler 0.9.26 patched with changes made in the original version TCC67 (0.9.16 or whatever version that was)

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
Linux at least GCC 4.7 toolchain
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

######1. Install g++
```
sudo apt-get install g++
```
######2. Install libftdi
Skip this step if you will be using ftd2xx driver. However libftdi works a lot better on linux libftdi is an open source ftdi driver that might be used as replacement when running on Linux or MacOSX
```
sudo apt-get install libftdi-dev
```
Jump to section "Install KMotionX" or "Install Java" if you will use the Java binding.

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

######2. Install gcc
Skip this step if ”gcc --version” is 4.7 or later. The current latest version available is 4.8
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

###Install drivers from source (Optional)
Installing packages with apt-get might not always result in the latest version depening on your system.
Here is how you install USB drivers from source. Most people would be satisfied with the version supplied by the package manager on your system.

######1. Install libusb

Install dependencies
```
sudo apt-get install libudev-dev
```

```
wget http://sourceforge.net/projects/libusb/files/libusb-1.0/libusb-1.0.18/libusb-1.0.18.tar.bz2
tar -xvf libusb-1.0.18.tar.bz2
cd libusb-1.0.18
./configure --prefix=/usr --disable-static
make
sudo make install
```

######2. Install libftdi

Skip this step if you will be using ftd2xx driver. However libftdi works a lot better on linux libftdi is an open source ftdi driver that might be used as replacement when running on Linux or MacOSX

Install dependencies
```
sudo apt-get install cmake
```
```
wget http://www.intra2net.com/en/developer/libftdi/download/libftdi1-1.1.tar.bz2
tar -xvf libftdi1-1.1.tar.bz2
cd libftdi1-1.1
mkdir build
cd build
cmake  -DCMAKE_INSTALL_PREFIX="/usr" ../
make
sudo make install
```

######3. Update linker cache
Im not sure if this is actually needed. But it can’t hurt.
sudo ldconfig

##Install Java (Optional)
If you would like to build the java binding you need to install a JDK

######Mac OS X
If you are going to use the java binding you will sort this out yourself. I Would recommend version 1.7

######Ubuntu 
```
sudo apt-get install openjdk-7-jdk
```

######Rasbian
Preinstalled


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
git clone https://github.com/parhansson/KMotionX.git
```

```
cd KMotionX
```
Configure build for your platform and flavours
```
./configure
```
Build project
```
make
```

Build Tiny C Compiler

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

######3. Execute examples
Plug in your KFlop to an available USB port.
KMotionServer will start automatically in background when needed. 
When KMotionDLL is rebuilt make sure to kill running server 'killall KMotionServer'

Start executeGCode example
```
./executeGCode
```
Or try the new web application. Start and surf to http://localhost:8080 in a modern web browser.
```
./kmxWeb -document_root ../KMotionX/examples/webgui/site
```

####Troubleshooting
executeGCode example says "Operation not permitted"
  Check device permissions
```
ls -la /dev/kflop
```
should reveal something like this where 001/005 might be other values
```  
lrwxrwxrwx 1 root root 15 Sep 22 18:28 /dev/kflop -> bus/usb/001/005
```
then check permissions on target (replace 001/005 with values from previous output)
```
ls -la /dev/bus/usb/001/005
```
should reveal
```
crw-rw-r-T 1 root plugdev 189, 4 Sep 22 18:28 005
```
This means users in plugdev group have read and write access to the device

If this is correct go on and check your users groups
Check that your user is in group plugdev
```
groups
```
Check that plugdev is present
```
pi adm dialout cdrom sudo audio video plugdev games users netdev input spi gpio
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
a suitable GUI toolkit such as GTK.

In order to be as efficient as possible with (slow) interpreted languages like Python, there is some extra
functionality in swig_extras.cpp.  This module provides some derived classes for both the basic KMotionDLL and
the GCodeInterpreter.  In the latter case, there is an efficient polling method which automatically calls
back to Python code whenever any event of interest occurs.

All Python callbacks from the kmotion module 

Multi-thread handling for Python is supported, which is important since the kmotion module will often be
running in a separate thread, which will call back to Python code.  Although this works fine at the
Python level, you must be careful about calling non thread-aware code, such as some GUI toolkits.  For
example, if you are using GTK, then make sure that you properly use GTK threading calls if you ever
modify GTK objects from a kmotion module callback.



