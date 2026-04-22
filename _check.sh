#!/bin/bash
#Checking for required dependencies
EDIT=nano
echo -e $COLOR1"Running" `realpath $0` $NORMCO
source "./screen.ini" > /dev/null

if [ -f "error.txt" ];
then
     echo -e $COLERR"Error during previous run:\n"
     cat error.txt
     rm -f error.txt
     echo -e $NORMCO"\nCleared."
     exit 0;
fi

set -e # https://intoli.com/blog/exit-on-errors-in-bash-scripts/


echo -e $COLOR4"\n\tThis script stops on any error!\n\tWhen it stop, remove source of the error & run it again!\n"
     
echo -e $COLOR1"Test for required software:\n"$COLOR2 
(gcc --version 2> error.txt || (echo -e $COLERR"Error running gcc!"$NORMCO && exit 1))
echo -e $COLOR1"\n GCC  OK\n"$COLOR2
(g++ --version 2> error.txt || (echo -e $COLERR"Error running g++!"$NORMCO && exit 1))
echo -e $COLOR1"\n G++  OK\n"$COLOR2
(make --version 2> error.txt || (echo -e $COLERR"Error running make!"$NORMCO && exit 1))
echo -e $COLOR1"\n MAKE  OK\n"$COLOR2
(cmake --version 2> error.txt || (echo -e $COLERR"Error running cmake!"$NORMCO && exit 1))
echo -e $COLOR1"\n CMAKE  OK\n"$COLOR2
((echo "rofi..." && rofi -v 2> error.txt) || (echo -e $COLERR"Error running rofi!"$NORMCO && exit 1))
echo -e $COLOR1"\n ROFI TOOL OK\n"$COLOR2

echo -e $COLOR1"Compilation platform:\n"$COLOR2

echo -e "Version:"$COLERR  `g++ -dumpversion` $COLOR2

echo -e "Machine:"$COLERR  `g++ -dumpmachine` $COLOR2

echo -e $COLOR1"\n Platform OK\n"$COLOR2

echo -e $COLOR2"Directories:"$COLERR
g++ --print-search-dirs

echo -e $COLOR1"\nRequired library packages:"
echo -e $COLOR2"==========================\n"

./_check_libs.sh

cat << EOF >> tmpX11.cpp
#include <X11/Xlib.h>   // sudo apt install libx11-dev ? 
#include <X11/Xutil.h>
#include <X11/xpm.h>    // sudo apt install libxpm-dev ?
EOF

g++ -c tmpX11.cpp

echo -e $COLOR1'\n libx11-dev and libxpm-dev are configured for compilation.'$COLOR2

rm -f tmpX11.cpp

echo -e $COLOR2"\nNow you can try to make whole library...\n"$COLOR1

echo    "	cmake ."
echo -e "	nano CMakeLists.txt" $COLOR3  "#if something was wrong." $COLOR1
echo    "	make"
echo    "       ./_makeDocs.sh"

echo -e $COLOR2 "\nEND" $NORMCO

#Dependencies installation under UBUNTU:
# $ sudo apt install gcc
# $ sudo apt install g++
# $ sudo apt install make
# $ sudo apt install cmake
# $ sudo apt install rofi
#
# $ sudo apt install libx11-dev
# $ sudo apt install libxpm-dev

