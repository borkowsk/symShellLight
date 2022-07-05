#!/bin/bash
#Checking for required dependencies

EDIT=nano

set -e # https://intoli.com/blog/exit-on-errors-in-bash-scripts/
echo "Running" `realpath $0`
echo -e "\n\tThis script stops on any error!\n\tWhen it stop, remove source of the error & run it again!\n"
     
echo -e "Test for required software:\n" 
gcc --version 
echo -e "\n GCC  OK\n"
g++ --version
echo -e "\n G++  OK\n"
make --version 
echo -e "\n MAKE  OK\n"
cmake --version 
echo -e "\n CMAKE  OK\n"

echo -e "Test for required library packages:\n"
cat << EOF >> tmpX11.cpp
#include <X11/Xlib.h>   // sudo apt install libx11-dev ? 
#include <X11/Xutil.h>
#include <X11/xpm.h>    // sudo apt install libxpm-dev ?
EOF

g++ -c tmpX11.cpp
echo -e "\n libxpm-dev  OK\n"

rm -f tmpX11.cpp

#Dependencies installation under UBUNTU:
# $ sudo apt install gcc
# $ sudo apt install g++
# $ sudo apt install make
# $ sudo apt install cmake
# echo Instaling development versions of non standard libraries
# echo libxpm-dev depends on libx11-dev, so both will be installed by this command:
# $ sudo apt install libxpm-dev

