# E312IMUTest

A simple program that shows how to get IMU data from USRP E310/E312s.

## Installing RTIMULib

Link to IMU library on git:
https://github.com/RPi-Distro/RTIMULib

Setting up SDK for cross-compiling for E310/E312:
https://kb.ettus.com/Software_Development_on_the_E310_and_E312#Installing_the_SDK

From within an active E310/E312 SDK, cross-compile and install the RTIMULib with:
```
git clone https://github.com/RPi-Distro/RTIMULib.git
cd RTIMULib/Linux
mkdir build
cd build
cmake -Wno-dev -DCMAKE_TOOLCHAIN_FILE=../../../uhd/host/cmake/Toolchains/oe-sdk_cross.cmake -DCMAKE_INSTALL_PREFIX=/usr ../
make -j4
make -j4 install DESTDIR=~/prefix
```
Note that you must provide a path to the appropriate cmake Toolchain provided by uhd.

Now simply copy the compiled library files from the SDK to the E312. The easiest way is to mount the SDK prefix on the embedded device using sshfs:
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

## Fixing RTIMULib i2c errors

In some cases the `/etc/RTIMULib.ini` file is not created properly. Specifically, device type, I2CBus, and I2CSlaveAddress may not be set correctly..

If this is the case, the following errors may appear:

```
root@ettus-e3xx-sg3:~# E312IMUTest
Settings file not found. Using defaults and creating settings file
Failed to open I2C bus 1
Failed to open SPI bus 0, select 0
No IMU detected
Using fusion algorithm RTQF
No IMU found
```

or 

```
root@ettus-e3xx-sg3:~# E312IMUTest
Do something here with example option input: for example
Settings file /etc/RTIMULib.ini loaded
Using fusion algorithm RTQF
min/max compass calibration not in use
Ellipsoid compass calibration not in use
Accel calibration not in use
I2C read error from 0, 117 - Failed to read MPU9150 id
I2C read error from 0, 114 - Failed to read fifo count
Press Ctrl + C to stop streaming...
I2C read error from 0, 114 - Failed to read fifo count
I2C read error from 0, 114 - Failed to read fifo count
I2C read error from 0, 114 - Failed to read fifo count
```

### Steps to fix

You should be able to fix the issue by following these steps:

1. Force creation of a new /etc/RTIMULib.ini file if necessary:

```
root@ettus-e3xx-sg3:~# mv /etc/RTIMULib.ini /etc/RTIMULib.ini.backup
```

2. Now run the program. You should see the following error output

```
root@ettus-e3xx-sg3:~# E312IMUTest
Do something here with example option input: for example
Settings file not found. Using defaults and creating settings file
Failed to open I2C bus 1
Failed to open SPI bus 0, select 0
No IMU detected
Using fusion algorithm RTQF
No IMU found
```

3. In the newly created /etc/RTIMULib.ini file make the following changes:

```
IMUType=2

#
# Fusion type type -
#   0 - Null. Use if only sensor data required without fusion
#   1 - Kalman STATE4
#   2 - RTQF
FusionType=2

#
# Is bus I2C: 'true' for I2C, 'false' for SPI
BusIsI2C=true

#
# I2C Bus (between 0 and 7)
I2CBus=0

#
# SPI Bus (between 0 and 7)
SPIBus=0

#
# SPI select (between 0 and 1)
SPISelect=0

#
# SPI Speed in Hz
SPISpeed=500000

#
# I2C slave address (filled in automatically by auto discover)
I2CSlaveAddress=105
```

Note that the `I2CSlaveAddress=105` corresponds to the hex address `0x69`

4. Reboot the device and run the program

A complete example of a working RTIMULib.ini file can be found in the `config` folder.
