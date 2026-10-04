### Ubuntu

This is only tested on 14.04.

###### 1. Install build tools
```
sudo apt-get install g++ pkg-config
```
`pkg-config` supplies the include and linker flags for the default libftdi build.
If it is already installed, there is no need to install it again.

###### 2. Install libftdi and libusb development packages
Skip libftdi if you are using the ftd2xx driver. The default build uses the
open-source libftdi driver.
```
sudo apt-get install libftdi1-dev libusb-1.0-0-dev
```
Check that both packages are visible with
`pkg-config --modversion libftdi1 libusb-1.0` before running `./configure`.
