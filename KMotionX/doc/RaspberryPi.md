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

######2. Install gcc 4.8
It might work with gcc 4.6 but it will not tested by me. On Raspberry Pi it seems to have more options for different ARM processors.
And why not just use the newer stuff?
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
