filepath=$(cd "$(dirname "$0")"; pwd)
echo $filepath
cd $filepath
rm -rf CMakeFiles
rm CMakeCache.txt
rm cmake_install.cmake
rm Makefile
rm -rf xhnSTL

cd ./UnitTest
./clean.sh

