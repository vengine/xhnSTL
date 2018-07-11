cmake_minimum_required (VERSION 2.6)
PROJECT(test C CXX)
# Initialize CXXFLAGS.
set(CMAKE_CXX_FLAGS                "-Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
#set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

INCLUDE_DIRECTORIES(/home/parallels/Projects/SDK/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/include/
                    /home/parallels/Projects/xhnSTL/Linux/
                    /home/parallels/Projects/xhnSTL/xhnSTL/xstl/
                    /home/parallels/Projects/xhnSTL/xhnSTL/linux/
                    /home/parallels/Projects/xhnSTL/xhnSTL/xlibc/)

LINK_DIRECTORIES(/home/parallels/Projects/SDK/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/lib/
                 /home/parallels/Projects/xhnSTL/Linux/
                 /var/nfs/)

add_definitions(-DLINUX -DBIT_WIDTH=32)
ADD_EXECUTABLE(test
    test.cpp
)

target_link_libraries(test xhnSTL)

#set(CMAKE_INSTALL_RPATH /var/nfs)
