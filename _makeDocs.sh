#!/bin/sh
(whereis doxygen | grep "/doxygen") && echo "OK, doxygen installed"
(whereis doxywizard | grep "/doxywizard") && echo "OK, doxywizard installed"

if ! test -d "DOCS/"
then 
    mkdir DOCS/
else
    echo "DOCS/ directory already exist"
fi 

echo;echo "RUNNING Doxygen:" ;echo

#run Doxygen directly
#doxygen Doxyfile

#or run Doxygen via wizzard
doxywizard Doxyfile 
