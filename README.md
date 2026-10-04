KMotionX
========

Linux/Unix (Mac OS) port of Dynomotions KMotion

KMotionX is a port to Dynomotions KMotion libraries.
This is just the core functionality of the libraries. I think it's all there an no features should be missing.
Please raise an issue if you find something.
The complicated features found in CNC control applications such as KMotionCNC is not in this repository. 
See [KMotionXCNC](https://github.com/parhansson/KMotionXCNC "CNC application")

#### Current status KMotion 5.3.2

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

The default libftdi build uses `pkg-config` to locate libftdi1 and libusb. Install
`pkg-config` (or `pkgconf`) and the development packages described in the
platform guides below before running `./configure`.

[Mac OS X](KMotionX/doc/MacOSX.md)

[Ubuntu](KMotionX/doc/Ubuntu.md)

[Raspberry Pi](KMotionX/doc/RaspberryPi.md)

## Install TI compiler

[Download compiler](https://www.ti.com/tool/C6000-CGT)

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
By default, KMotionX installs under `$HOME/.local` and `make install` does
not need `sudo`. To install system-wide instead, configure with
`--prefix=/usr/local` and use `sudo make install`.
If the libraries are installed under a custom prefix, make their `.pc` files
visible before configuring:
```
export PKG_CONFIG_PATH="/path/to/prefix/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"
./configure
```
You can check discovery with `pkg-config --modversion libftdi1 libusb-1.0`.
For the proprietary ftd2xx driver instead, run `./configure --ftd2xx`.
If the driver is outside your compiler's library search path, specify its
directory with `./configure --ftd2xx --ftd2xx-libdir=/path/to/driver/lib`.
`--libdir` controls where KMotionX installs its own libraries; it is not a
search path for dependencies. The build uses the build directory and the
configured KMotionX installation directory to locate its libraries at runtime.
Build project
```
make
```
Install project
```
make install
```
Add `$HOME/.local/bin` to your `PATH` if you want to run the installed commands
by name.

###### 2. Install KFLOP device rules (Linux only)
This will install a rule that tell your system to grant read and write access to the kflop device for users in group "plugdev"
If your user is not in that group fix users groups or change the rule before pluging the device in.
```
sudo cp KMotionX/usb/etc/udev/rules.d/10.kflop.rules /etc/udev/rules.d/
```
Reload rules
```
sudo udevadm control --reload-rules && udevadm trigger
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
