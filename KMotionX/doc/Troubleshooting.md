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
