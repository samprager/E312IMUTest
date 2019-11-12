# E312IMUTest

A simple program that shows how to get IMU data from USRP E310/E312s.

## Installing RTIMULib

Link to library on git:
https://github.com/RPi-Distro/RTIMULib

Setting up SDK for cross-compiling for E310/E312:
https://kb.ettus.com/Software_Development_on_the_E310_and_E312#Installing_the_SDK

From within an active E310/E312 SDK, cross-compile and install the RTIMULib with:
```
git clone https://github.com/RPi-Distro/RTIMULib.git
cd RTIMULib/Linux
mkdir build
cd build
cmake -Wno-dev -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchains/oe-sdk_cross.cmake -DCMAKE_INSTALL_PREFIX=/usr ../
make -j2
make install
```
Now simply copy the compiled library files from the SDK to the E312.

The easiest way is to mount the SDK prefix on the embedded device using sshfs:
```
$ ssh root@e3xx # After this, we're logged onto the embedded device
$ pwd # Check where we are
/home/root
$ mkdir usr # Create a directory to mount our prefix, this only needs doing once
$ sshfs dev@desktop:prefix/ usr/ # This will mount the prefix on the development machine to ~/usr
$ ls usr/ # The actual output of this may differ
environment-setup-armv7ahf-vfp-neon-oe-linux-gnueabi
site-config-armv7ahf-vfp-neon-oe-linux-gnueabi
sysroots/
usr/
version-armv7ahf-vfp-neon-oe-linux-gnueabi
```

And copy the necessary files:
```
cp -r usr/usr/bin/RTIMU* /usr/bin/
cp -r usr/usr/include/RT*.h /usr/include/
cp -r usr/usr/include/IMUDrivers /usr/include/
cp usr/usr/lib/libRTIMULibGL.so.7.2.1  /usr/lib/
ln -s /usr/lib/libRTIMULibGL.so.7.2.1 /usr/lib/libRTIMULibGL.so.7
ln -s /usr/lib/libRTIMULibGL.so.7 /usr/lib/libRTIMULibGL.so
cp usr/usr/lib/libRTIMULib.so.7.2.1 /usr/lib/
ln -s /usr/lib/libRTIMULib.so.7.2.1 /usr/lib/libRTIMULib.so.7
ln -s /usr/lib/libRTIMULib.so.7 /usr/lib/libRTIMULib.so
```

## Cross compilation for E312

To compile the program with RTIMULib support on the SDK (with prefix `~/e312_cross` for example):
```
cmake -Wno-dev -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchains/oe-sdk_cross.cmake -DCMAKE_INSTALL_PREFIX=/usr -DRTIMULib_INCLUDE_DIR=~/e312_cross/usr/ -DRTIMULib_LIBRARY=~/e312_cross/usr/lib/libRTIMULib.so ../
make -j4 # This may take a while
make -j4 install DESTDIR=~/e312_cross
```
