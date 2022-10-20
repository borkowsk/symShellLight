# CMAKE configuration for SYMSHELLLIGHT libraries wbsyshSVG wbsyshX11
######################################################################
# Define X11 for compiling X11 version of library
# Define WINDOWS for compiling MS Windows version
# Define WITHTESTS if you want build test applications!

cmake_minimum_required(VERSION 2.9)
project(symshelllight)

set( X11 true ) #DEFAULT
#set( WINDOWS true ) #NOT TESTED!
set( WITHTESTS false )

if (MSVC)
    #warning level 4 and/or all warnings as errors if you want
    #add_compile_options( /W4 ) #  /WX
else()
    # A lots of warnings if you want
    #add_compile_options( -Wall )
endif(MSVC)

include_directories("./")
#see -> https://cmake.org/cmake/help/v3.16/manual/cmake-compile-features.7.html#manual:cmake-compile-features(7)
#message("C++ compiler flags: ${CMAKE_CXX_FLAGS}")

if(true) # SVG SYMSHELL LIB
add_library( wbsyshSVG STATIC
    "SVG/symshsvg.cpp"
    "UTILS/sshdefs.c"
    "UTILS/sshutils.cpp"
    "UTILS/strtorgb.cpp"
    "UTILS/wieloboki.cpp"
    	"UTILS/wb_pchar.cpp" #used internally
	"UTILS/wb_rand.cpp"
	"UTILS/wb_randg.c"
    "UTILS/sshmenufdummy.c" #dummy functions of menus
     "SVG/errorent.c"  
	)
#target_compile_features(wbsyshSVG PUBLIC cxx_std_11)
endif()

if(X11) # only for X11 LIB
add_definitions( -DHAVE_X11 -DUSE_X11 )
add_library( wbsyshX11 STATIC 
    "X11/symshx11.c"
    "X11/wbx11ico.c"
    "UTILS/sshdefs.c"
    "UTILS/sshutils.cpp"
    "UTILS/strtorgb.cpp"
    "UTILS/wieloboki.cpp"	
	"UTILS/wb_pchar.cpp"
	"UTILS/wb_rand.cpp"
	"UTILS/wb_randg.c"
    "UTILS/sshmenufdummy.c" #dummy functions of menus
    "X11/errorent.c"
	)
#target_compile_features(wbsyshX11 PUBLIC cxx_std_11)
endif(X11)

if(WINDOWS) # only for WINDOWS - NOT TESTED IN THIS VERSION (after 2016)
add_library( wbsyshWIN STATIC
    "WIN/symshwin.c"
    "UTILS/sshutils.cpp"
    "UTILS/strtorgb.cpp"
    "UTILS/wieloboki.cpp"
	"UTILS/wb_pchar.cpp"
	"UTILS/wb_rand.cpp"
	"UTILS/wb_randg.c"	
    "WIN/sshmenufwin.c"
    #"UTILS/sshmenufdummy.c" #dummy functions of menus
    "WIN/errorent.c"
    )
#target_compile_features(wbsyshWIN PUBLIC cxx_std_11)
endif(WINDOWS)


## PROGRAMY TESTOWE  ##
## TEST APPLICATIONS ##
#######################

if(X11 AND WITHTESTS ) #X11 tests
add_executable("TestmousX11"
		"EXAMPLES/Testmous.cpp"
    	)
add_executable("TestpaleX11"
		"EXAMPLES/Testpale.cpp"
    	)
add_executable("TestpolyX11"
		"EXAMPLES/Testpoly.cpp"
		)
add_executable("TestrandX11"
        "EXAMPLES/Testrand.cpp"
        "UTILS/wb_randg.c"
        "UTILS/wb_rand.cpp"
    	)
add_executable("TestsymsX11"
		"EXAMPLES/Testsyms.cpp"
		)
add_executable("TestsyshX11"
		"EXAMPLES/Testsysh.cpp"
		)
add_executable("TurmitX11"
		"EXAMPLES/Turmit.cpp"
		)
add_executable("CAGasX11"
		"EXAMPLES/CAGas.cpp"
		)

target_link_libraries("TestmousX11" wbsyshX11 X11 Xpm rt)
target_link_libraries("TestpaleX11" wbsyshX11 X11 Xpm rt)
target_link_libraries("TestpolyX11" wbsyshX11 X11 Xpm rt)
target_link_libraries("TestrandX11" wbsyshX11 X11 Xpm rt)
target_link_libraries("TestsymsX11" wbsyshX11 X11 Xpm rt)
target_link_libraries("TestsyshX11" wbsyshX11 X11 Xpm rt)
target_link_libraries("TurmitX11"   wbsyshX11 X11 Xpm rt)
target_link_libraries("CAGasX11"    wbsyshX11 X11 Xpm pthread rt)

#add_executable("LinktestX11" "EXAMPLES/Linktest.cpp"    )
#target_link_libraries("LinktestX11" wbsyshX11 X11 Xpm rt)
endif()

if( WINDOWS AND WITHTESTS ) #X11 tests
add_executable("TestmousWIN"
		"EXAMPLES/Testmous.cpp"
    	)
add_executable("TestpaleWIN"
		"EXAMPLES/Testpale.cpp"
    	)
add_executable("TestpolyWIN"
		"EXAMPLES/Testpoly.cpp"
    	)
add_executable("TestrandWIN"
        "EXAMPLES/Testrand.cpp"
        "UTILS/wb_randg.c"
        "UTILS/wb_rand.cpp"
    	)
add_executable("TestsymsWIN"
		"EXAMPLES/Testsyms.cpp"
    	)
add_executable("TestsyshWIN"
		"EXAMPLES/Testsysh.cpp"
    	)
add_executable("TurmitWIN"
		"EXAMPLES/Turmit.cpp"
    	)
add_executable("CAGasWIN"
		"EXAMPLES/CAGas.cpp"
    	)

target_link_libraries("TestmousWIN" wbsyshWIN  )
target_link_libraries("TestpaleWIN" wbsyshWIN  )
target_link_libraries("TestpolyWIN" wbsyshWIN  )
target_link_libraries("TestrandWIN" wbsyshWIN  )
target_link_libraries("TestsymsWIN" wbsyshWIN  )
target_link_libraries("TestsyshWIN" wbsyshWIN  )
target_link_libraries("TurmitWIN"   wbsyshWIN  )
target_link_libraries("CAGasWIN"    wbsyshWIN  )

#add_executable("LinktestWIN" "EXAMPLES/Linktest.cpp"    )
#target_link_libraries("LinktestWIN" wbsyshWIN )
endif()


if(WITHTESTS) #SVG tests
add_executable("TestmousSVG" "EXAMPLES/Testmous.cpp"
    )
add_executable("TestpaleSVG" "EXAMPLES/Testpale.cpp"
    )
add_executable("TestpolySVG" "EXAMPLES/Testpoly.cpp"
    )
add_executable("TestrandSVG"
        "EXAMPLES/Testrand.cpp"
        "UTILS/wb_randg.c"
        "UTILS/wb_rand.cpp"
    )
add_executable("TestsymsSVG" "EXAMPLES/Testsyms.cpp"
    )
add_executable("TestsyshSVG" "EXAMPLES/Testsysh.cpp"
    )
add_executable("TurmitSVG" "EXAMPLES/Turmit.cpp"
    )
add_executable("CAGasSVG" "EXAMPLES/CAGas.cpp"
    )

if( WINDOWS )
	target_link_libraries("TestmousSVG" wbsyshSVG  )
	target_link_libraries("TestpaleSVG" wbsyshSVG  )
	target_link_libraries("TestpolySVG" wbsyshSVG  )
	target_link_libraries("TestrandSVG" wbsyshSVG  )
	target_link_libraries("TestsymsSVG" wbsyshSVG  )
	target_link_libraries("TestsyshSVG" wbsyshSVG  )
	target_link_libraries("TurmitSVG"   wbsyshSVG  )
	target_link_libraries("CAGasSVG"    wbsyshSVG  )

	add_executable("LinktestSVG"
			"EXAMPLES/Linktest.cpp"
			)

	target_link_libraries("LinktestSVG" wbsyshSVG  )
else()
	target_link_libraries("TestmousSVG" wbsyshSVG  rt)
	target_link_libraries("TestpaleSVG" wbsyshSVG  rt)
	target_link_libraries("TestpolySVG" wbsyshSVG  rt)
	target_link_libraries("TestrandSVG" wbsyshSVG  rt)
	target_link_libraries("TestsymsSVG" wbsyshSVG  rt)
	target_link_libraries("TestsyshSVG" wbsyshSVG  rt)
	target_link_libraries("TurmitSVG"   wbsyshSVG  rt)
	target_link_libraries("CAGasSVG"    wbsyshSVG pthread rt)

	add_executable("LinktestSVG"
			"EXAMPLES/Linktest.cpp"
			)

	target_link_libraries("LinktestSVG" wbsyshSVG rt )
endif()

endif()
#KONIEC - THE END
#********************************************************************/
#*              SYMSHELLLIGHT  version 2022-10-20                   */
#********************************************************************/
#*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
#*            W O J C I E C H   B O R K O W S K I                   */
#*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
#*    GITHUB: https://github.com/borkowsk                           */
#*                                                                  */
#*    Instytut Studiów Społecznych im. Profesora Roberta Zajonca    */
#*    Uniwersytetu Warszawskiego                                    */
#*    PL.WWW: http://iss.uw.edu.pl/                                 */
#*                                                                  */
#*    Robert Zajonc Institute for Social Studies                    */
#*    University of Warsaw                                          */
#*    EN.WWW: http://iss.uw.edu.pl/en/                              */
#*                                                                  */
#*                               (Don't change or remove this note) */
#********************************************************************/

