source /home/parallels/Projects/SDK/environment-setup-cortexa9hf-vfp-neon-poky-linux-gnueabi
rm -rf CMakeFiles
rm CMakeCache.txt
rm cmake_install.cmake
rm Makefile
cmake . -DCMAKE_TOOLCHAIN_FILE=OEToolchainConfig.cmake
make
cd ./..
