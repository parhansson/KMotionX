KMotionX
========

Linux/Unix (Mac OS X ) port of Dynomotions KMotion

KMotionX is more of a patch than a port of KMotion (Except for KMotionServer). The goal has been to make as small modifications as possible to the orginal source. This will make it easier to keep up with every new release.
    Builds with at least gcc-4.7 on linux and Visual Studio 2008 on windows.


See it in action on youtube
https://www.youtube.com/watch?v=oPTJwcre0hA


KMotionX also includes a JNI binding to use expose the GCodeInterpreter and KMotionDLL API to the Java world. See advanced.

####Who might be interested in this.
Anyone that would like to operate their KFlop controlled machine from their phone, tablets or anything with a web browser. I have ported the KMotionServer, KMotionDLL and GCodeInterpreter. I had plans to create a Java GUI but have now dropped that idea and created a HTML5 web application for that purpose. Drag an svg file into browser window and it will instantly be converted into executable GCode. Although I must admit it is still in alpha. There are lots of missing features to just use it out of the box. Some things might be hard wired for my current machine setup. Hence in this stage you might have to do some changes yourself. Or just wait until I have fixed it.


![Image of KMXWeb](KMotionX/doc/images/main.png)


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
Linux at least GCC 4.6 toolchain
libudev-dev
cmake
boost
libusb
libftdi
udev rule


##Build and install
This guide contains four sections. How to install on Mac OS X, Ubuntu, Rasbian (Raspberry Pi) and a common section for KMotion libraries.

Setup and install required dependencies on your platform

[Mac OS X](KMotionX/doc/MacOSX.md)

[Ubuntu](KMotionX/doc/Ubuntu.md)

[Raspberry Pi](KMotionX/doc/RaspberryPi.md)

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
Or try the new web application. Start and surf to http://localhost:8080 in a web browser preferably Chrome.
```
./kmxWeb -document_root ../KMotionX/KMotionXCNC
```

##Extra

[It still doesn't work. Troubleshoot](KMotionX/doc/Troubleshooting.md)


[Advanced stuff](KMotionX/doc/Advanced.md)
