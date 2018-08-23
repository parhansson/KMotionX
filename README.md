KMotionX
========

Linux/Unix (Mac OS) port of Dynomotions KMotion

KMotionX is a port to Dynomotions KMotion libraries.
This is just the core functionality of the libraries. I think it's all there an no features should be missing.
Please raise an issue if you find something.
The complicated features found in CNC control applications such as KMotionCNC is not in this repository. 
See [KMotionXCNC](https://github.com/parhansson/KMotionXCNC "CNC application")

#### Current status KMotion 4.34

Runs on Linux and Mac OS.

Ported libraries and executables
KMotionDLL.dll -> libKMotion
GCodeInterpreter.dll -> libGCodeInterpeter
KMotionServer.exe -> KMotionServer
TCC.exe > tcc67


See it in action on youtube
https://www.youtube.com/watch?v=oPTJwcre0hA

## Build and install
This guide contains four sections. How to install on Mac OS X, Ubuntu, Rasbian (Raspberry Pi) and a common section for KMotion libraries.

Setup and install required dependencies on your platform

[Mac OS X](KMotionX/doc/MacOSX.md)

[Ubuntu](KMotionX/doc/Ubuntu.md)

[Raspberry Pi](KMotionX/doc/RaspberryPi.md)

## Install KMotionX

###### 1. Build KMotionX
Install git if not already installed
```
sudo apt-get install git
```

Clone repository
```
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
Install project (some platforms (MacOS) do not require 'sudo' just make install')
```
sudo make install
```

###### 2. Install KFLOP device rules (Linux only)
This will install a rule that tell your system to grant read and write access to the kflop device for users in group "plugdev"
If your user is not in that group fix users groups or change the rule before pluging the device in.
```
sudo cp KMotionX/usb/etc/udev/rules.d/10.kflop.rules /etc/udev/rules.d/
```

###### 3. Execute examples or install [KMotionXCNC](https://github.com/parhansson/KMotionXCNC "CNC application")
Plug in your KFlop to an available USB port.
KMotionServer will start automatically in background when needed.
When KMotionDLL is rebuilt make sure to kill running server 'killall KMotionServer'

Start executeGCode example
```
cd bin
./executeGCode
```

## Extra

[It still doesn't work. Troubleshoot](KMotionX/doc/Troubleshooting.md)


[Advanced stuff](KMotionX/doc/Advanced.md)
