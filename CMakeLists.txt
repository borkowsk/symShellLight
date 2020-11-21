# CMAKE configuration for SYMSHELLLIGHT libraries wbsyshSVG wbsyshX11
######################################################################
# Define X11 for compiling X11 version of library
# Define WINDOWS for compiling MS Windows version
# Define WITHTESTS if you want build test aplications!

cmake_minimum_required(VERSION 2.8)

project(symshelllight)

include_directories("./")

if(true) # SVG SYMSHELL LIB
add_library( wbsyshSVG STATIC
    "SVG/errorent.c"
    "SVG/symshsvg.cpp"
    "UTILS/wb_pchar.cpp" #Używane wewnątrz modułu
    "UTILS/sshdefs.c"
    "UTILS/sshutils.cpp"
    "UTILS/sshmenufdummy.c" #dummy functions of menus
)
endif()

if(X11) # only for X11 LIB
add_definitions( -DHAVE_X11 -DUSE_X11 )
add_library( wbsyshX11 STATIC 
    "X11/errorent.c"
    "X11/symshx11.c"
    "X11/wbx11ico.c"
    "UTILS/sshdefs.c"
    "UTILS/sshutils.cpp"
    "UTILS/sshmenufdummy.c" #dummy functions of menus
)
endif()

if(WINDOWS) # only for WINDOWS - NOT TESTED IN THIS VERSION (after 2016)
add_library( wbsyshWIN STATIC
    "WIN/errorent.c"
    "WIN/sshmenufwin.c"
    "WIN/symshwin.c"
    "UTILS/sshutils.cpp"
    "UTILS/sshmenufdummy.c" #dummy functions of menus
    )
endif()


## PROGRAMY TESTOWE  ##
## TEST APPLICATIONS ##
#######################

if(X11 AND WITHTESTS ) #X11 tests
add_executable("TestmousX11" "TESTS/Testmous.cpp"
    )
add_executable("TestpaleX11" "TESTS/Testpale.cpp"
    )
add_executable("TestpolyX11" "TESTS/Testpoly.cpp"
    )
add_executable("TestrandX11"
        "TESTS/Testrand.cpp"
        "UTILS/wb_randg.c"
        "UTILS/wb_rand.cpp"
    )
add_executable("TestsymsX11" "TESTS/Testsyms.cpp"
    )
add_executable("TestsyshX11" "TESTS/Testsysh.cpp"
    )
add_executable("TurmitX11" "TESTS/Turmit.cpp"
    )
add_executable("CAGasX11" "TESTS/CAGas.cpp"
    )

target_link_libraries("TestmousX11" wbsyshX11 X11 Xpm rt)
target_link_libraries("TestpaleX11" wbsyshX11 X11 Xpm rt)
target_link_libraries("TestpolyX11" wbsyshX11 X11 Xpm rt)
target_link_libraries("TestrandX11" wbsyshX11 X11 Xpm rt)
target_link_libraries("TestsymsX11" wbsyshX11 X11 Xpm rt)
target_link_libraries("TestsyshX11" wbsyshX11 X11 Xpm rt)
target_link_libraries("TurmitX11"   wbsyshX11 X11 Xpm rt)
target_link_libraries("CAGasX11"    wbsyshX11 X11 Xpm pthread rt)

#add_executable("LinktestX11" "TESTS/Linktest.cpp"    )
#target_link_libraries("LinktestX11" wbsyshX11 X11 Xpm rt)
endif()

if( WINDOWS AND WITHTESTS ) #X11 tests
add_executable("TestmousWIN" "TESTS/Testmous.cpp"
    )
add_executable("TestpaleWIN" "TESTS/Testpale.cpp"
    )
add_executable("TestpolyWIN" "TESTS/Testpoly.cpp"
    )
add_executable("TestrandWIN"
        "TESTS/Testrand.cpp"
        "UTILS/wb_randg.c"
        "UTILS/wb_rand.cpp"
    )
add_executable("TestsymsWIN" "TESTS/Testsyms.cpp"
    )
add_executable("TestsyshWIN" "TESTS/Testsysh.cpp"
    )
add_executable("TurmitWIN" "TESTS/Turmit.cpp"
    )
add_executable("CAGasWIN" "TESTS/CAGas.cpp"
    )

target_link_libraries("TestmousWIN" wbsyshWIN  rt)
target_link_libraries("TestpaleWIN" wbsyshWIN  rt)
target_link_libraries("TestpolyWIN" wbsyshWIN  rt)
target_link_libraries("TestrandWIN" wbsyshWIN  rt)
target_link_libraries("TestsymsWIN" wbsyshWIN  rt)
target_link_libraries("TestsyshWIN" wbsyshWIN  rt)
target_link_libraries("TurmitWIN"   wbsyshWIN  rt)
target_link_libraries("CAGasWIN"    wbsyshX11  pthread rt)

#add_executable("LinktestWIN" "TESTS/Linktest.cpp"    )
#target_link_libraries("LinktestWIN" wbsyshX11 X11 Xpm rt)
endif()


if(WITHTESTS) #SVG tests
add_executable("TestmousSVG" "TESTS/Testmous.cpp"
    )
add_executable("TestpaleSVG" "TESTS/Testpale.cpp"
    )
add_executable("TestpolySVG" "TESTS/Testpoly.cpp"
    )
add_executable("TestrandSVG"
        "TESTS/Testrand.cpp"
        "UTILS/wb_randg.c"
        "UTILS/wb_rand.cpp"
    )
add_executable("TestsymsSVG" "TESTS/Testsyms.cpp"
    )
add_executable("TestsyshSVG" "TESTS/Testsysh.cpp"
    )
add_executable("TurmitSVG" "TESTS/Turmit.cpp"
    )
add_executable("CAGasSVG" "TESTS/CAGas.cpp"
    )

target_link_libraries("TestmousSVG" wbsyshSVG  rt)
target_link_libraries("TestpaleSVG" wbsyshSVG  rt)
target_link_libraries("TestpolySVG" wbsyshSVG  rt)
target_link_libraries("TestrandSVG" wbsyshSVG  rt)
target_link_libraries("TestsymsSVG" wbsyshSVG  rt)
target_link_libraries("TestsyshSVG" wbsyshSVG  rt)
target_link_libraries("TurmitSVG"   wbsyshSVG  rt)
target_link_libraries("CAGasSVG"    wbsyshSVG pthread rt)

add_executable("LinktestSVG" "TESTS/Linktest.cpp"    )
target_link_libraries("LinktestSVG" wbsyshSVG rt)
endif()

#KONIEC - THE END
#********************************************************************/
#*              SYMSHELLLIGHT  version 2020-11-21                   */
#********************************************************************/
#*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
#*            W O J C I E C H   B O R K O W S K I                   */
#*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
#*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
#*    GITHUB: https://github.com/borkowsk                           */
#*                                                                  */
#*                               (Don't change or remove this note) */
#********************************************************************/
