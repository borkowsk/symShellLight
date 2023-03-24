#!/bin/bash
#Checking for required dependencies

set -e # https://intoli.com/blog/exit-on-errors-in-bash-scripts/

source "./screen.ini"
EDIT=nano

echo -e $COLOR1"Running" `realpath $0` $COLERR
echo -e "\n\tThis script stops on any error!\n\tWhen it stop, remove source of the error & run it again!\n"
     
echo -e $COLOR1"Test for required software:\n"$COLOR2 
gcc --version 
echo -e $COLOR1"\n GCC  OK\n"$COLOR2
g++ --version
echo -e $COLOR1"\n G++  OK\n"$COLOR2
make --version 
echo -e $COLOR1"\n MAKE  OK\n"$COLOR2
cmake --version 
echo -e $COLOR1"\n CMAKE  OK\n"$COLOR2

echo -e $COLOR1"Compilation test:\n"$COLOR2

echo -e "Version:"$COLERR  `g++ -dumpversion` $COLOR2

echo -e "Machine:"$COLERR  `g++ -dumpmachine` $COLOR2

echo -e $COLOR2"Directories:"$COLERR
g++ --print-search-dirs

echo -e $COLOR2"Required library packages:"$COLERR

cat << EOF >> tmpX11.cpp
#include <X11/Xlib.h>   // sudo apt install libx11-dev ? 
#include <X11/Xutil.h>
#include <X11/xpm.h>    // sudo apt install libxpm-dev ?
EOF

g++ -c tmpX11.cpp

echo -e $COLOR1'\n libx11-dev && libxpm-dev  OK'$COLOR2

rm -f tmpX11.cpp

echo -e $COLOR2"\nNow you can try to compile..."$COLOR1

echo    "	cmake ."
echo -e "	nano CMakeLists.txt" $COLOR3  "#if something was wrong." $COLOR1
echo    "	make"

echo -e $COLOR2 "\nEND" $NORMCO

#Dependencies installation under UBUNTU:
# $ sudo apt install gcc
# $ sudo apt install g++
# $ sudo apt install make
# $ sudo apt install cmake
# echo Instaling development versions of non standard libraries
# echo libxpm-dev depends on libx11-dev, so both will be installed by this command:
# $ sudo apt install libxpm-dev

