KMotionX
========

Linux/Unix (Mac OS X ) port of Dynomotions KMotion

KMotionX is more of a patch than a port of KMotion (Except for KMotionServer). The goal has been to make as small modifications as possible to the orginal source. This will make it easier to keep up with every new release.

KMotionX also includes a JNI binding to use expose the GCodeInterpreter and KMotionDLL API to the Java world.

####Who might be interested in this.
Not that many actually. I have only ported the KMotionServer, KMotionDLL and GCodeInterpreter. I have not ported any GUI applications such as KMotionCNC. Hence the only way to execute is to use one of the examples or make a program yourself.
In the future I plan to make a working GUI application in Java. Wich will be possible to use on Linux.


#####Changes from original source
- Source code for KMotionDLL and GCodeInterpreter has been patched with "ifdef" directives.(I know it is a bit ugly)
- Changed include directives to build on case sensitive file systems #include "stdafx.h" to "StdAfx.h"
- Implemented thread handling to make use of Posix instead of Windows thread API
- Option to use libftdi instead of ftd2xx driver (libftdi works alot better on Linux)
- Added latest TCC compiler 0.9.26 patched with changes made in the original version TCC67 (0.9.16 or whatever version that was)

#####Changes to be made
- Remove dependency of CString
- Implement callback pattern for MessageBox and AfxMessageBox


#####Features to be implemented
- Add Compile and load coff program to init KFLOP
- Execute on PC (No problem)
- Concatenation of error message in CheckSoftLimitsArc
- ListLocations KMotionDLL_Direct
- LoadCoff (almost done needs testing)


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
Jump to section Install KMotionX


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
Skip this step if ”gcc --version” is 4.7 or later.
Install gcc and g++ 4.7. This command installs gcc and g++  4.7 but does not change to 4.7 as default. If you would like to do that here is a guide. (switching-gccg-versions.html) If you do you need to adjust make file to use default installed version.
```
sudo apt-get install gcc-4.7 g++-4.7
```
######3. Install libusb
Installing libusb, there are four options here. I choose option 4.

Option 1: Not needed, I will use ftd2xx driver

Option 2: Already installed

Option 3: Install with apt-get. I have not tried this.
sudo apt-get install libusb-1.0.0

Option 4: Install latest version from source

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

######4. Install libftdi

Skip this step if you will be using ftd2xx driver. However libftdi works a lot better on linux libftdi is an open source ftdi driver that might be used as replacement when running on Linux or MacOSX

Option 1: Install package (Not tested on Rasbian)
sudo apt-get install libftdi-dev

Option 2: Install latest version from source
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

######5. Update linker cache
Im not sure if this is actually needed. But it can’t hurt.
sudo ldconfig

##Install Java
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
./configure
make
```

```
cd tcc-0.9.26
./configure —enable-cross
make
cd ..
```

######2. Install KFLOP device rules
```
sudo cp KMotionX/usb/etc/udev/rules.d/10.kflop.rules /etc/udev/rules.d/
```

######3. Execute examples
Start KMotionServer in background
```
cd bin
./KMotionServer &
Start ExecuteGCode
./ExecuteGCode
```

