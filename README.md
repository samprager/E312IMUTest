# E312IMUTest

## Cross compilation for E312
to build with RTIMULib

cmake -Wno-dev -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchains/oe-sdk_cross.cmake -DCMAKE_INSTALL_PREFIX=/usr -DRTIMULib_INCLUDE_DIR=~/e312_cross/usr/ -DRTIMULib_LIBRARY=~/e312_cross/usr/lib/libRTIMULib.so ../
make -j4 # This may take a while
make -j4 install DESTDIR=~/e312_cross
