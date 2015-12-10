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

##Tiny C Compiler 0.9.26 (Optional)
Build Tiny C Compiler

(On Linux install texi2html and texinfo ```sudo apt-get install texi2html texinfo```)

```
cd KMotionX/tcc-0.9.26
./configure —enable-cross
make
cd ../..
```

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
